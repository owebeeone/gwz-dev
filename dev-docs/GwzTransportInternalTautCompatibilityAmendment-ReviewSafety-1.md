# GWZ internal Taut compatibility amendment — SAFETY-AXIS RE-REVIEW 1

**Object:** `gwz-core/dev-docs/GwzTransportInternalTautCompatibilityAmendment.md` at core HEAD  
**Tuple:** root `ac72a3c984348d9c9a8c7bc56c4590fee17bf378`; gwz-core `343cccc4032be47f8369c4eb297e3a6f50c87f11`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`  
**Axis:** Safety; independent, peer-blind, read-only re-review  
**Verdict: GO — P0: 0, P1: 0, P2: 0, P3: 0**

## 0 Evidence base

I reviewed the committed amendment and its change from core commit `e063bb020bc0d9023eff9fc0fa3f6bacbc2f8d8e`. I checked the prior Consistency and Safety reports and remediation plan, `AgentProcessRules.md` as amended by `GwzProcessOptimization.md`, remote transport requirements §5.1 G6 and design §10, placement design §§3, 6, 8 and 9, the sequenced requirements and design, `GWZRequirements.md`, `GWZDesign.md`, and `gwz-transport/protocol/transport.taut.py`. I did not read the current peer’s report. No files were changed, and no build or test was run. Unrelated working-tree changes were outside the reviewed object. The tuple matched at the start and end.

| Prior finding | Closure on this tuple |
| --- | --- |
| Consistency P2-1: the waiver could extend from internal transport Envelopes to outer GWZ requests and old/new driver/core interoperation | **Closed.** C1 limits waived bytes and readers to the `gwz-transport` Envelope and expressly preserves the outer schema and ordinary-local interoperation. C2 retains G6, old/new fixtures, capability affinity and explicit-cli/old-core refusal. Precedence limits each placement-design waiver to historical internal Envelope bytes. |
| Prior Safety review: no finding | No reopened safety finding. The narrowed boundary does not remove an active-session safeguard. |

The changed range comprises the opening scope sentence, C1, an added C2 sentence, and Precedence. C3 is unchanged. I found no new architectural root cause in that range.

## 2 Invariant analysis

An ordinary-local request from a new driver to an old core uses the GWZ request/response boundary, even when it contains no transport Envelope. C1 and Precedence now exclude that boundary from the waiver. The placement design’s ordinary-local old/new fixtures and G6’s wire-compatibility rule therefore remain required. Conversely, an explicit-cli request to an old core must be refused before operation submission. A capability result obtained from core A cannot authorize dispatch to replacement core B: C2 preserves receiver affinity, and placement §3 still requires a pinned receiving generation or refusal.

For the internal transport path, a profile-3-only Bind offered to a profile-1/2 endpoint must reject before Git, credential or pool effects; C2 forbids downgrade. After regeneration, current profile-1/2 operations still need functional proof. C1 requires Rust and Python projections to change together, while placement §8 still requires the same-build CLI/core and Python/core consumer paths. The waived tests concern historical transport bytes and old transport readers, not those current paths.

A malformed or oversized Envelope must fail bounded validation before dispatch. For profile 3, a positive-stream frame without a positive `message_seq` must fail; a `Closed` delivered before earlier Data cannot report graceful success until the ordered prefix and close prerequisites apply. C2–C3 and the unsuperseded sequenced design retain these rules. Allowing an absent sequence to encode as null on bootstrap or profiles 1/2 creates no authority or ordering exception.

## 3 Risks and next action

This verdict accepts the **draft amendment’s safety scope**, not implementation or activation. The implementation gate still needs executed evidence for ordinary-local old/new fixtures, explicit-cli/old-core refusal before submission, receiver replacement, current Rust/Python profile-1/2 operation paths, malformed and bounded decoding, and profile-3 sequencing. Proceed to the independent review verdict merge on this tuple.
