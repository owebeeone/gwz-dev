# GWZ operation-session protocol — focused Safety re-verdict

Date: 2026-09-23  
Axis: Safety  
Verdict: **NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 2 |
| P3 | 0 |

## Exact tuple

| Repository | Commit |
| --- | --- |
| gwz-dev | `c5b497bc0934b8970d5abba7022b5da020ff7f72` |
| gwz-core | `d3951dcfc04d2f09c9c7a025dec41ee3163c4058` |
| gwz-py | `45bcd7b3ea102ca927935cee1b41b43934d68140` |
| gwz-transport | `46e65a9a888fbd4a5bbeace946996581dcf23333` |

The four commits matched at the start and end of review.

## Prior-finding closure

| Prior Safety finding | Focused re-verdict |
| --- | --- |
| P0-1, legacy cross-client disclosure | **Closed in the design.** Version-0 writes and event/result/merge reads now require the originating Client or route; unowned module-level reads fail closed in a version-1-capable build (`GwzOperationSessionProtocolDesign.md:139–152`). The prior two-client collision and foreign-read sequence is prohibited. Implementation proof remains a later gate. |
| P2-1, indefinitely charged abandoned local work | **Closed for its original local-handler sequence.** Version-1 admission now refuses an unqualified local handler, explicitly including today’s asynchronous local clone; qualified local handlers must terminate within 30 seconds of cancellation (`GwzOperationSessionProtocolDesign.md:55–67, 361–378`). The new end-to-end orphan bound has different gaps; see P2-1 and P2-2 below. |
| P2-2, oversized caller ID defeats terminal fallback | **Closed in the design.** Admission rejects IDs above 256 UTF-8 bytes, bounds the other fallback fields, and requires a generated-schema measurement below the 4 KiB reservation (`GwzOperationSessionProtocolDesign.md:115–137, 221–226`). The original 8 KiB-ID sequence now refuses before `Accepted`. |
| P2-3, capability preflight reads endpoint environment on refused placement | **Closed in the design.** Session open is now the sole version/placement negotiation path and must run before the existing endpoint-constructing `transport_capabilities` path. An unbound CLI request refuses before environment access (`GwzOperationSessionProtocolDesign.md:79, 154–159, 317–328`). |

## Changed-range analysis

Against root `ee11f44efa5a0796a71d874ffaa4bc570a611a4e`, this correction changes local-handler qualification and orphan timing, cheap negotiation, legacy owner binding, bounded terminal metadata, event cursors, and caller-facing failure forms. The caller guide now documents the expanded fetch signature, cleanup facts, and terminal outcomes. Against core `8756fd6b32443b0ac63287ee5b4a3335e8cf0894`, the capacity amendment replaces the full no-lease installation rules in retry §3(7), §6, and S1.4. Its quiescence rule now counts admitted scopes before first Open. I found no new capacity-transition counterexample in that changed text.

The new Safety risk is concentrated in the root design’s claim that every orphaned session charge retires within 35 seconds (`GwzOperationSessionProtocolDesign.md:363–378`). That claim depends on both handler termination and actual endpoint cleanup.

## 0. Evidence and limits

This was a read-only, peer-blind design review. I read the committed review object and remediation plan, the relevant accepted transport and Python designs, and committed core/Python source for feasibility. I used `git show`, `git diff`, `rg`, and line-numbered reads. I did not inspect current-round peer reports, change files, or run builds or tests. These are contract findings, not an implementation or release judgment.

## 1. Findings

### [P2-1] A five-second cleanup report is treated as completed physical cleanup — **NEW ARCHITECTURAL ROOT CAUSE**

- **Location and root cause:** `GwzOperationSessionProtocolDesign.md:363–378` asserts that bounded endpoint cleanup takes at most five seconds and therefore an orphaned session charge retires within 35 seconds. The same design requires charges to remain until child work actually joins (`:366–368`), and the core capacity amendment defines quiescence as having no unfinished physical cleanup (`GwzRemoteTransportCapacityAmendment.md:89–100`). In committed core, `src/transport_host/session.rs:666–679` returns from `cleanup()` at its five-second deadline even when `pending_local_work` is nonzero. The guide itself defines a nonzero value as cleanup still outstanding (`GwzOperationSessionCallerGuideDraft.md:110–118`).
- **Violated invariant:** A bounded wait or report must not be counted as completed cleanup. Session and physical-capacity charges cannot retire while their physical work remains pending.
- **Reproduction:** Admit network work, then lose its route while endpoint cleanup is delayed beyond five seconds. The local handler may join within its required bound, but `Session::cleanup()` returns at its deadline with `pending_local_work > 0`. Releasing the orphan’s charge by 35 seconds undercounts unfinished work and permits new admissions before the capacity amendment’s quiescent condition. Keeping the charge correctly makes the published 35-second retirement guarantee false.
- **Impact:** The text permits either false completed cleanup and physical-capacity over-admission, or a stranded charged session despite its stated recovery bound.
- **Required correction:** Separate a timed cleanup report from proof that physical work has ended. Specify who retains ownership and capacity charges when the report is pending, and how that state eventually retires. If actual termination cannot be proved within a finite bound, revise the orphan-recovery architecture and its advertised deadline; a five-second reporting timeout cannot supply that proof.
- **Closure test:** Hold physical cleanup beyond five seconds after route loss. Verify that a nonzero pending report never causes early session or capacity-charge retirement, that no incompatible new capacity epoch installs, and that the eventual retirement rule has a proved finite bound or a truthful pending-state contract.

This is a **new architectural root cause** in the corrected object: the recovery bound assumes a stronger physical cleanup primitive than the accepted host exposes. Under `GwzOperationSessionProtocol-RemPlan-2.md:17–19, 35–41`, it triggers the two-round stop rule for redesign or explicit acceptance rather than another bounded correction.

### [P2-2] The 35-second orphan bound does not constrain network-handler termination

- **Location and root cause:** `GwzOperationSessionProtocolDesign.md:55–67` requires a proved bounded termination contract for admitted handlers, but the published 30-second limit and qualification rule are specifically for *local* handlers (`:361, 369–374`). The text nevertheless promises retirement of an orphaned session charge within 35 seconds without limiting an admitted network handler to 30 seconds. Current Python dispatch runs the network Git handler synchronously and reaches `request.finish()` only after that handler returns (`gwz-py/native/src/transport_session.rs:394–414`).
- **Violated invariant:** A receiver-wide orphan-retirement deadline must cover every accepted execution scope, including a network handler delayed after cancellation.
- **Reproduction:** Accept a network fetch whose handler has a proved but longer bounded cancellation path, or blocks in local Git processing after transport cancellation is signalled. Lose the route. That handler can remain active past 30 seconds while satisfying the draft’s unspecified “bounded” network requirement; close must retain its charge and cannot meet the stated 35-second deadline.
- **Impact:** Abandoned network work can occupy receiver session slots beyond the advertised recovery bound. Repeating it can exhaust the 32-session receiver limit.
- **Required correction:** Publish and enforce a termination bound for every accepted network handler that fits the end-to-end orphan bound, including its local Git phases; otherwise narrow or replace the 35-second guarantee and define the resulting admission recovery. Refusal before `Accepted` must cover handlers that lack the required proof.
- **Closure test:** Block a network handler after acceptance but outside an active transport wait, then lose its route. Verify either pre-admission refusal for an unqualified handler or handler join, completed cleanup, and charge retirement within the published end-to-end bound.

## 2. Verdict and process consequence

The four prior Safety counterexamples are closed in the revised contract. The new 35-second claim does not survive delayed physical cleanup or a network handler with no matching numeric termination limit. **NO-GO** follows from P2-1 and P2-2.

P2-1 is a **new architectural root cause**. The remediation plan’s two-round cap therefore calls for stopping this lane for redesign or explicit acceptance, not drafting another bounded correction.
