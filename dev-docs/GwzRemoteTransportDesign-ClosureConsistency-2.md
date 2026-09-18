# GWZ Remote Transport Consistency Findings — Focused Closure Report

**Closure object:** Comparison of the four transport documents from gwz-core `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` to `05842b38e55f109ed3663555680751811a72eb9b`.

**Verdict: GO for original Consistency findings P2-1 and P2-2 only.** Both original counterexamples now have one explicit, internally consistent outcome. This is not a fresh full-axis or implementation-readiness verdict.

## Filing verification

The filed report at root `3a0b8fa975bc013673ee919b087f69da9f3853ff`, `dev-docs/GwzRemoteTransportDesign-ReviewConsistency.md`, matches my returned testimony. The verdict, evidence pins, two findings, counterexamples, corrections, closure tests, and pre-commit to GO were preserved. **No transcription discrepancy found.**

## Finding closure

| Original finding | Revised normative locations | Counterexample retrace | Status |
|---|---|---|---|
| **P2-1 — No defined pre-effect capability-binding exchange** | `GwzRemoteTransportRequirements.md:161–167` requires a session-bound taut Bind/Bound acknowledgement before Open, invalidation on disconnect, and separation from core capabilities. `GwzRemoteTransportDesign.md:64–118` defines Bind, Bound and BindRejected; carrier-session and endpoint identifiers; version/scheme/policy intersections; minimum limit selection; acknowledgement before installation; pre-allocation scheme checking; reconnect invalidation; local parity; and the distinction from `transport_capabilities`. `GwzRemoteTransportDesign.md:197–224` places binding before the stream inventory and binds Open to the acknowledged endpoint/session. `GwzRemoteTransportDesign.md:641` specifies version, scheme, limit, stale-session, absent-binding and zero-effect acceptance cases. `GWZRequirements.md:40–44` and `GWZDesign.md:42–46` carry the amendment into the authoritative documents. | An older or incompatible endpoint now yields no common version and `BindRejected`; no speculative Open occurs. An SSH-only endpoint cannot accept an HTTPS Open because both mux and endpoint check the bound intersection before lease allocation, credential access, helper invocation or host connection. Missing bindings refuse, payload limits select the bounded minimum, disconnect invalidates capabilities, and stale Bound/Open messages cannot attach to a replacement session. Core service capability reporting cannot substitute for endpoint binding. | **CLOSED** |
| **P2-2 — No disposition for existing network schemes and SSH aliases** | `GwzRemoteTransportRequirements.md:12–20` narrows the endpoint programme to SSH/HTTPS and names HTTP/git as local-only. `GwzRemoteTransportRequirements.md:138–145` preserves native local HTTP/git while requiring pre-effect refusal for nonlocal placement; lines `208–212` require every admitted SSH spelling to use one adapter and pool scheme without core-socket fallthrough. `GwzRemoteTransportDesign.md:14–16` narrows the former blanket claim. Its route table and rules at `150–173` explicitly cover SCP syntax, `ssh://`, `ssh+git://`, `git+ssh://`, `https://`, `http://`, `git://`, file/local forms, and unknown schemes. All SSH spellings canonicalize to `ssh`; HTTP/git remain native only for local/default placement; explicit CLI placement returns `UnsupportedOperation` before core socket/helper access. `GwzRemoteTransportDesign.md:642` specifies per-spelling, canonical-pool, retained-local and unsupported-nonlocal acceptance evidence. `GWZRequirements.md:42–44` and `GWZDesign.md:44–46` carry this disposition into the authoritative documents. | Existing `git://` and `http://` operations retain their native local behavior instead of silently bypassing a promised endpoint. Explicit nonlocal requests refuse before effects. SCP and all three native SSH scheme spellings select the same endpoint adapter and canonical pool, so an uncovered alias cannot fall through to libgit2’s built-in core transport. File/local and unknown schemes also have explicit outcomes. | **CLOSED** |

## Evidence integrity and scope

The immutable objects resolved identically at the start and end:

| Object | Commit | Tree |
|---|---|---|
| Root remediation inputs | `3a0b8fa975bc013673ee919b087f69da9f3853ff` | `445c63ee7e6a692b8453b297ea94265437dbbddc` |
| Original gwz-core object | `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` | `a2f7e28b2d9fbc0a7b61f54675c4835dc03d6f17` |
| Revised gwz-core object | `05842b38e55f109ed3663555680751811a72eb9b` | `226102463b82fd1d334663245df78fe1a00cd963` |

Root and gwz-core HEADs equalled the respective remediation and revised pins at the end. The inspected revision was limited to the four named documents: 244 insertions and 30 deletions. No current-round prompt or report was read. No implementation tests or builds were run or claimed; the acceptance rows remain specifications for later executable validation.
