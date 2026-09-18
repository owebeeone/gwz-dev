# GWZ Remote Transport draft — SAFETY ORIGINAL-FINDING CLOSURE

**Review object:** Four-document gwz-core change from `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` to `05842b38e55f109ed3663555680751811a72eb9b`:

- `dev-docs/GWZRequirements.md`
- `dev-docs/GWZDesign.md`
- `dev-docs/GwzRemoteTransportRequirements.md`
- `dev-docs/GwzRemoteTransportDesign.md`

**Date:** 2026-09-19  
**Scope:** Focused retrace of original Safety findings P1-1 and P2-1 through P2-3 only.  
**Verdict: GO for these four original findings.** All four are closed at the draft documentation-contract level.

## Filed-testimony integrity

The filed original report at root commit `3a0b8fa975bc013673ee919b087f69da9f3853ff`, `dev-docs/GwzRemoteTransportDesign-ReviewSafety.md` (blob `d5fdb522ebbd0f9ebe7f77118011c4368051a9e9`), matches the returned Safety testimony. No transcription discrepancy was found in its review object, baseline, verdict, four findings, closure tests, or requested next action.

## Closure results

| Finding | Counterexample retrace and revised contract | Status |
|---|---|---|
| **P1-1 — Credential-bearing HTTPS URLs can bypass the `gh`-only boundary and cross the carrier** | The original gap was at `GwzRemoteTransportDesign.md:127-151,390-420`, against `GwzRemoteTransportRequirements.md:147-150,212-213`. The revision now rejects all HTTPS userinfo, including username-only forms, query, and fragment before encoding `Open`, invoking a helper, or causing a network effect; it repeats validation after effective-URL rewriting and before following redirects (`GwzRemoteTransportDesign.md:175-194`). The structured destination has no password, HTTP-userinfo, Authorization, query, fragment, or raw-URL field (`:185-194`). Requirements G1/G4 make the refusal normative (`GwzRemoteTransportRequirements.md:138-158`), and the acceptance matrix requires local/carried and redirect sentinel tests with no secret in messages or errors (`GwzRemoteTransportDesign.md:642`). The original sequence now stops before its credential can enter the carrier or HTTP adapter. | **CLOSED** |
| **P2-1 — Process-wide transport registration has no coexistence or external-synchronization boundary** | The original proposal used process-wide registration and thread-local context at `GwzRemoteTransportDesign.md:360-370`. The revision requires a per-remote callback with owned context and prohibits process-global registration, synthetic-scheme substitution, and thread-local routing (`GwzRemoteTransportDesign.md:507-513`). If safe per-remote binding is unavailable, endpoint support cannot be enabled or advertised; global registration is explicitly not an acceptable substitute (`:515-523`). Requirements G7 and D3 repeat that boundary (`GwzRemoteTransportRequirements.md:169-171,295`), while the acceptance and qualification sections require pre-existing custom and ordinary foreign libgit2 traffic before, during, and after GWZ construction, plus concurrent context isolation (`GwzRemoteTransportDesign.md:640,663`). The original global race and unbound-factory failure path is therefore outside the permitted implementation contract. | **CLOSED** |
| **P2-2 — `Close` is legal from `Active` even though it neither requires nor performs `EndWrite`** | The original inconsistency spanned `GwzRemoteTransportDesign.md:127-139,210-217,225-235,398-404` and `GwzRemoteTransportRequirements.md:231-237`. The revision defines `Close` as ordered after `EndWrite` and carrying the same final offset (`GwzRemoteTransportDesign.md:220-223`). API close from every active or half-closed state starts its deadline immediately, stops new writes, emits remaining credited `Data`, emits `EndWrite(final_offset)` if needed, and only then sends `Close(final_offset)` (`:358-370`). Unread reverse data is explicitly surrendered, drained/discarded under the same bound, and reported in `Closed`; cleanup cannot be mistaken for Git success, while `Cancel` remains immediate abandonment (`:372-380`). Requirements S5 makes that sequence normative (`GwzRemoteTransportRequirements.md:251-257`), and the acceptance matrix covers all four half-states, pending data, exhausted credit, unread reverse traffic, and racing cancellation (`GwzRemoteTransportDesign.md:633`). The formerly legal stuck sequence now has one bounded outcome. | **CLOSED** |
| **P2-3 — Payload and aggregate limits begin only after an unbounded serialized message has been accepted and decoded** | The original limits began after ingress at `GwzRemoteTransportDesign.md:153-208`. The revision requires every serialized carrier to reject an oversized frame before allocating or reading its declared body and forbids accepting an unbounded message before inspecting its length (`GwzRemoteTransportDesign.md:257-266`). It specifies finite bootstrap and stream caps (`:268-275`), requires budgeted decoding or nonallocating preflight before the generic decoder, checks length, node, integer, depth, and unknown-field budgets before allocation/descent, and charges encoded storage, decode scratch, decoded copies, and queued output concurrently (`:277-287`). In-process construction receives equivalent admission limits. Requirements S9 states the same ingress invariant (`GwzRemoteTransportRequirements.md:267-271`), and the acceptance matrix includes oversized/truncated frames, deep unknown collections, metadata, and tiny-message storms (`GwzRemoteTransportDesign.md:643`). The original allocation-before-rejection counterexample is excluded by the revised contract. | **CLOSED** |

## Evidence and limits

The commit and tree objects were verified unchanged at both the beginning and end:

| Object | Commit | Tree |
|---|---|---|
| Root review inputs | `3a0b8fa975bc013673ee919b087f69da9f3853ff` | `445c63ee7e6a692b8453b297ea94265437dbbddc` |
| Original gwz-core object | `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` | `a2f7e28b2d9fbc0a7b61f54675c4835dc03d6f17` |
| Revised gwz-core object | `05842b38e55f109ed3663555680751811a72eb9b` | `226102463b82fd1d334663245df78fe1a00cd963` |

The comparison was restricted to the four named documents and the original findings. No builds or implementation tests were run or claimed for this documentation gate. This GO closes P1-1 and P2-1 through P2-3 only; it does not prejudge fresh-review findings or implementation qualification.
