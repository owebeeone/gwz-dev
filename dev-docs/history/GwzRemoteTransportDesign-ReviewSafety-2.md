# GWZ Remote Transport draft — SAFETY ROUND-2 RE-VERDICT

**Review object:** Four-document gwz-core revision from `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` to `05842b38e55f109ed3663555680751811a72eb9b`:

- `dev-docs/GWZRequirements.md`
- `dev-docs/GWZDesign.md`
- `dev-docs/GwzRemoteTransportRequirements.md`
- `dev-docs/GwzRemoteTransportDesign.md`

**Prior-round inputs:** Root remediation plan and original reports at `3a0b8fa975bc013673ee919b087f69da9f3853ff`.  
**Date:** 2026-09-19  
**Axis:** Safety — degraded and mixed-version paths, irreversible and uncertain effects, disclosure, resource bounds, stuck states, and process-wide blast radius.  
**Verdict: GO at the draft documentation-contract stage.** All original findings are closed and the changed-range interaction review found no new blocking Safety issue.

## Evidence integrity

The filed original Safety report at `dev-docs/GwzRemoteTransportDesign-ReviewSafety.md` matches the original returned testimony. No transcription discrepancy was found.

The immutable objects were verified before the focused closure and again after the full changed-range pass:

| Object | Commit | Tree |
|---|---|---|
| Root remediation inputs | `3a0b8fa975bc013673ee919b087f69da9f3853ff` | `445c63ee7e6a692b8453b297ea94265437dbbddc` |
| Original gwz-core object | `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` | `a2f7e28b2d9fbc0a7b61f54675c4835dc03d6f17` |
| Revised gwz-core object | `05842b38e55f109ed3663555680751811a72eb9b` | `226102463b82fd1d334663245df78fe1a00cd963` |

Review remained read-only and peer-blind to the current Consistency round and unfinished fresh-review output.

## Prior-finding closure

| Finding and architectural root | Revised closure | Status |
|---|---|---|
| **P1-1 — credential boundary:** HTTPS URL credentials could bypass `gh` and enter the carrier. | HTTPS userinfo, query, and fragment now refuse before `Open`, helper use, or network effects; redirects receive the same validation. The structured destination has no credential, Authorization, query, fragment, or raw-URL field (`GwzRemoteTransportDesign.md:175-195`; `GwzRemoteTransportRequirements.md:138-158`; acceptance case at design `:642`). | **CLOSED** |
| **P2-1 — process-global ownership:** unsafe global transport registration could race with or intercept unrelated libgit2 activity. | The design now requires per-remote callback ownership and prohibits global registration, synthetic schemes, prefix replacement, and thread-local lookup. Missing safe binding prevents enablement and advertisement (`GwzRemoteTransportDesign.md:507-528`; requirements `:169-171,295`; qualification and coexistence cases at design `:640,663`). | **CLOSED** |
| **P2-2 — lifecycle completion:** graceful `Close` was admitted without a required `EndWrite`, permitting stuck HTTP bodies or divergent implicit EOF handling. | API close now starts one deadline, stops writes, emits remaining credited `Data`, emits `EndWrite(final_offset)` if needed, then sends `Close` with the same offset. Reverse data is explicitly drained/discarded for bounded cleanup and reported without implying Git success (`GwzRemoteTransportDesign.md:220-223,350-380`; requirements `:251-257`; acceptance case at design `:633`). | **CLOSED** |
| **P2-3 — ingress resource boundary:** payload limits applied only after an unbounded serialized value could be accepted and decoded. | Serialized carriers must reject oversized declarations before reading or allocating their bodies. Fixed bootstrap and stream caps, bounded preflight/decoding, unknown-field limits, aggregate encoded-plus-decoded accounting, and equivalent in-process admission are required (`GwzRemoteTransportDesign.md:257-295`; requirements `:267-271`; acceptance case at design `:643`). | **CLOSED** |

## Changed-range Safety analysis

| Interaction examined | Result |
|---|---|
| **Binding, stale sessions, and mixed versions** | `Bind/Bound` performs no Git-host, credential, or helper effect; the mux validates the acknowledgement before installing the binding, and the endpoint rechecks the bound scheme and policy before lease allocation or credential access. Limits can only narrow, disconnect invalidates the binding and all streams, and reconnect requires a fresh session id (`GwzRemoteTransportDesign.md:86-118`). Missing fields mean unsupported, old drivers remain limited to ordinary local requests, and explicit nonlocal requests to an old or unbound endpoint refuse rather than speculate (`:580-595`). No stale-session or capability-only path reaches a host effect. |
| **Route classification, aliases, and credential refusal** | Effective URLs are classified before network effects. Every supported SSH spelling reaches the same callback and canonical pool scheme; HTTP/git are explicit local-only compatibility cases; explicit nonlocal and redirect attempts cannot fall through to a core socket or built-in transport (`GwzRemoteTransportDesign.md:150-173`; requirements `:208-212`). Credential validation precedes serialization and is repeated at the endpoint and across redirects, including percent-encoding reconstruction rules (`GwzRemoteTransportDesign.md:175-195`). |
| **Close ordering, backpressure, and uncertain effects** | Pending writes remain subject to credit and the close deadline; exhausted credit ends in cancel/discard rather than an unbounded wait. Premature or offset-inconsistent wire `Close` is a protocol failure. Reverse draining returns credit only into a bounded discard sink and remains under the same deadline. `Closed` records discarded response data and is never publication or Git-success evidence (`GwzRemoteTransportDesign.md:358-380`). This composes with the existing no-replay rule after possible remote effect. |
| **Ingress bounds protecting bootstrap and multiplexed streams** | The fixed bootstrap envelope is bounded before negotiation, so `Bind/Bound` does not create a pre-negotiation allocation gap. Length, nesting, collection, metadata, allocation, frame-count, and concurrent encoded/decoded/queued-storage budgets apply before allocation or descent, including unknown fields (`GwzRemoteTransportDesign.md:257-295`). Unsafe framing fails its carrier and releases its leases; unrelated carriers remain live. |
| **Per-remote callback, compatibility routes, and rollout** | Only explicitly configured GWZ remotes receive the owned callback. Native HTTP/git and unrelated normal/custom libgit2 operations keep their transports. Callback payload lifetime, panic/error translation, remote forms, and coexistence are qualification conditions; absence of the safe binding leaves the old implementation active and prevents endpoint advertisement (`GwzRemoteTransportDesign.md:507-528,654-670`). This contains both runtime and rollout blast radius. |
| **Top-level contract alignment** | The amendments consistently state Bind/Bound gating, explicit local HTTP/git treatment, credential-free HTTPS routing, per-remote binding, ordered close, and pre-allocation bounds (`GWZRequirements.md:40-50`; `GWZDesign.md:42-52`). Neither top-level document weakens the detailed contract. |

## New findings

No new P1, P2, or P3 Safety finding was identified in the changed ranges or their interactions. The revision introduces no new unresolved architectural root within this draft-stage review object.

The remaining safe-binding extension, SSH/HTTPS library and platform qualification, numeric tuning, and executable acceptance cases are implementation work. The documents guard those dependencies by prohibiting capability advertisement before qualification, so their unfinished state is not a documentation-stage Safety blocker.

## Full-axis verdict

**GO.** The revised four-document object closes P1-1 and P2-1 through P2-3, preserves the previously sound fail-closed, no-replay, identity-isolation, pool-accounting, and bounded-cleanup invariants, and introduces no new blocking Safety defect in the changed-range interactions.

No implementation acceptance, test execution, build result, schema freeze, or performance result is claimed.
