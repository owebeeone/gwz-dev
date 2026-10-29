# GWZ Connection API contract — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzConnectionApiContract.md`, revision 2, DRAFT, and `dev-docs/GwzConnectionApiContract-RemPlan-2.md`, at root `f4aff9b0b6524711b5065eb0d088e41ef43ecabe`.

**Baseline:**

| Repository | Pinned HEAD |
| --- | --- |
| `.` | `f4aff9b0b6524711b5065eb0d088e41ef43ecabe` |
| `gwz-core` | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| `gwz-py` | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| `gwz-cli` | `5193db962e672c54cd20ab2ef8448d904129a559` |
| `gwz-git` | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| `gwz-transport` | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| `gwz-sspi` | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| `git2-rs` | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| `libgit2` | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All nine HEADs matched at the beginning and end. Substantive inspection used pinned Git objects.

**Date:** 2026-10-19.

**Axis:** Consistency, including API Surface: contract coherence, controlling authority, interface composition, lifecycle completeness, scope and recurrence prevention. Independent, adversarial, read-only. Nothing here relies on the other axis’s current-round report. Filed verbatim by the lane owner.

**Verdict: GO** — no open Consistency findings and no new P0–P3 findings. The original three Consistency closures remain valid. The revised application boundary also resolves the inherited prior-round ordering counterexample on this axis. This is acceptance of a design/interface contract only.

---

## Prior-finding closure table

| Prior finding | Result | Independent retrace |
| --- | --- | --- |
| **Consistency P2-1 — generic exchanges lacked a usable sequencing/byte-stream entry** | **Remains closed** | Section 6.1 still selects neutral start tickets, a bounded typed opening table and a version-free OrderedStreamMachine. The callback replacement preserves the non-Git SSH/HTTP route and removes no required opening or reply transition. No dummy GitService or profile-3-to-profile-2 envelope cast is required. |
| **Consistency P3-1 — CleanupDomain lacked final lifecycle operations** | **Remains closed** | Domain construction/owner close, retained-service ownership and the two-host final-shutdown recipe remain unchanged. Host B’s completion cannot release host A’s pending generation; dropping the last public ticket/handle does not abandon it. |
| **Consistency P3-2 — InputBudget was absent from AdmissionError** | **Remains closed** | Lines 708–722 retain explicit `AdmissionError::InputBudget`, build-time `InvalidConfig(InputBudget)` and refusal before registration, endpoint construction or effects. |
| **Safety P2-1 — acquired permit could outlive revocation without a defined registration outcome** | **No regression found** | Lines 221–244 retain one authority transition over Reserved and Registered claims, terminal NeverRegistered outcomes and both register-versus-revocation race orders. |
| **Safety P2-2 — Applied receipt could precede actual byte application** | **Resolved in the revised contract on this axis** | Lines 348 and 359–410 remove the owned-token handoff. The event and charge remain kernel-owned; the synchronous callback performs the actual transition before sequence advancement or Applied publication. WouldBlock has no effect, and cancellation shares the application guard. The originating Safety reviewer supplies its separate re-verdict. |

These are document-level closures and regression checks. No executed implementation proof is claimed.

**Architectural-root accounting:** No NEW ARCHITECTURAL root is identified in this round. The cumulative historical count remains two; this report does not identify a third root or trigger the lane-stop rule.

## Changed-range analysis

The entire contract delta from `011d3eb0a487dbf62e10432034a0105be2518a22` to `f4aff9b0b6524711b5065eb0d088e41ef43ecabe` was inspected.

The substantive correction is concentrated in §6.1:

- `take_applied() -> AppliedEvent` becomes `apply_ready(lane, callback)`.
- CandidateEvent is borrowed only for a synchronous, lane-guarded callback.
- Actual byte/control mutation precedes sequence advancement and Applied publication.
- A no-effect WouldBlock preserves the same event, sequence and charge.
- Cancellation and abortive fencing serialize with application.
- Callback rejection or unwind fences uncertain state without publishing Applied or replaying it.
- OrderedStreamMachine applies inside that callback; it receives no event requiring later host-scheduled application.

The proof traces, invariant matrix and amendment row were updated to use this same boundary. Inspection found no remaining affirmative AppliedEvent/take_applied path; the remaining `take_applied` mention explicitly prohibits that queue.

The change restores the existing kernel’s application model while retaining the neutral opening and version-free byte entry selected in revision 1. It neither introduces another ownership protocol nor makes scheduler ordering part of correctness.

The same two reviewers continue under the recorded operator instruction “use the old reviewers.” The round-2 remediation plan and prior-round Safety report are legitimate inputs. No current-round peer report was accessed.

## 0. Evidence base

This round inspected:

- The complete pinned contract diff for revision 1 → revision 2.
- `GwzConnectionApiContract-RemPlan-2.md`, lines 1–49.
- Prior-round `GwzConnectionApiContract-ReviewSafety-1.md`, including its P2-2 trace and required correction.
- Current contract:
  - §6.1, lines 333–450;
  - retained-domain rules, lines 163–186;
  - registration and revocation, lines 213–244;
  - error surface, lines 706–724;
  - construction and final-domain walkthrough, lines 754–789;
  - invariant matrix and amendment row, lines 820–866;
  - review/authorization boundary, lines 868–879.
- Pinned transport `src/sequenced.rs`:
  - lines 255–258: atomic application or no-effect WouldBlock;
  - lines 525–549: callback success before application marking and Applied resolution;
  - lines 552–589: ordered draining and retention on WouldBlock;
  - lines 592–613: expected-sequence advancement and terminal recording.
- The unchanged controlling-authority and source evidence retained from the earlier reviews: root instructions/process rules; accepted Connection plan and verdict; reuse design; crate map/library boundaries; canonical core and remote-transport contracts; sequenced-stream design; Git API/design; Windows/native contracts and operator decisions; actual Python/core runtime construction.

Commands were read-only `git rev-parse`, `git diff`, pinned `git show` and text inspection. No files were written, builds/tests run, network accessed, dependencies changed or additional agents launched.

## Problem, requirement provenance, and scope

The requested outcome remains a concrete, independently testable, Git-independent communication contract with persistent connection ownership, isolated operations, explicit authentication/trust inputs, production gwz-git consumption and structural prevention of the observed churn.

The underlying problem remains established by the unchanged source: Python’s supported transport route constructs an operation-owned runtime, and core’s cancellable entry finishes and shuts it down. Retaining a native authentication host does not preserve that pool. Retaining the existing runtime indefinitely would also preserve finite lifetime-registration and bootstrap constraints.

The neutral sequencing seam addresses a necessary consequence of the generic communication requirement. Revision 2’s callback correction addresses a demonstrated flaw in that seam: separating an Applied receipt from the actual byte transition would restore a scheduler-dependent ordering requirement.

The chosen remedy follows the existing supported kernel route. Pinned `sequenced.rs` already requires an atomic apply callback and advances/resolves only after success. Reusing that model is a bounded correction, supported by concrete source evidence. It does not establish a need for another carrier, daemon, dependency upgrade, repository or whole-session programme.

The editable scope remains contract and review records. Every implementation repository and dependency remains reference-only. The correction changes the proposed pure-kernel interface, not source, wire schema, native mechanisms or release activation.

No unsupported prerequisite or unauthorized scope expansion blocks this revision. Native process safety, retained native cleanup and actual consumer migration remain separate obligations; they are not treated as completed or made prerequisites for the portable fake proof.

## 1. Findings

No new findings.

No open Consistency P0, P1, P2 or P3 remains on the reviewed contract.

## 2. Invariant analysis

### Application precedes Applied

The prior-round counterexample no longer has an allowed intermediate ownership state.

Hold Data sequence 1 at eligibility. Its event and charge remain inside StreamOrder. A WouldBlock callback mutates neither ordering nor byte state. Sequence 2 and an orderly terminal cannot enter their application callbacks or become Applied ahead of sequence 1.

On resume, sequence 1’s callback performs the actual byte transition. Only success permits expected-sequence advancement and Applied publication. No separately owned T1 exists that could be delayed while T2 reaches the byte machine first.

This agrees with the retained sequenced-stream contract and the current receiver’s callback model. The receipt contains identifiers/progress, not unapplied work.

### Cancellation contains uncommitted application

Cancellation uses the same lane application guard. The legal order is therefore either:

- byte mutation commits, then the fence fixes the remaining outcome; or
- the fence supersedes the uncommitted event before its callback can mutate state.

The borrowed candidate cannot survive the callback and apply after the fence. Callback rejection or unwind fences the lane without reporting Applied or retrying potentially mutated state. Partial mutation is expressly incompatible with WouldBlock.

Physical cleanup remains separately owned; a sequence or cancellation receipt does not free a physical permit.

### Independent lanes retain progress

The callback performs only bounded in-memory transitions or bounded Start queue insertion. It cannot block, await or perform I/O. A full work queue or unavailable byte machine produces no-effect WouldBlock for that lane.

The revised trace requires another lane to complete while the predecessor remains blocked. This preserves protocol-owned ordering without adding a host-wide application-order barrier.

The lane application guard is also compatible with the existing authority-lock order: payload/application locking must follow the domain authority mutex where both are required. The correction does not authorize reverse acquisition or effectful work under those locks.

### Generic starts and version separation remain expressible

The original generic SSH trace remains:

1. Register an Operation and reserve a charged lane.
2. Submit RequestStart with a ticket for bounded non-Git command metadata.
3. Apply its bounded connector-queue insertion while the operation claim is live.
4. Apply ReplyStart after physical open/authorization.
5. Apply reordered body events through the synchronous ordered byte callback.
6. Settle stream outcome and physical cleanup separately.

The HTTP trace retains separate response status/headers and ordered body delivery. Data before readiness remains buffered. Neither generic trace requires GitService, a new serialized Open or reinterpretation of a version-3 envelope.

The Git profile retains its authored first messages and wire sequence numbers in its wrapper. OrderedStreamMachine remains a separate version-free entry; current profile-1/2 interfaces remain compatibility paths.

### Public lifecycle and errors remain coherent

The public construction recipe still creates the cleanup-domain owner/capability, supplies the capability before host build, admits/registers independent operations, and closes host and domain owners explicitly.

A held permit consumed after close remains terminal NeverRegistered, with Effect::None and exactly-once reservation accounting. Registration winning the authority race remains Registered and included in revocation cleanup.

In the two-host case, A’s pending generation remains retained after B completes and after final domain close/drop. Ticket/handle destruction cannot free it. Confirmed disposal releases the matching generation once.

Over-budget AuthContext admission still has a precise pre-registration error. The callback correction changes none of these lifecycle or error outcomes.

### Prevention and adoption boundaries remain intact

The revision retains bounded admission, per-binding identifiers/tombstones, generation-qualified completion, operation-local policy/authentication state, honest pending cleanup, independent progress and separate portable/native/real-consumer gates.

The contract still requires one factored ordering algorithm and shared conformance vectors. It does not accept a duplicate ordering implementation in Connection or an internal fixture as proof of actual Python reuse.

## 3. Risks and next action

Implementation must enforce the callback’s scoped borrowing, no-effect WouldBlock, lane fencing and application-before-receipt rule. The text now specifies those obligations; this review does not prove their implementation.

The smallest later regression set is: blocked predecessor versus successor/terminal; unrelated-lane progress; cancellation before and after byte commit; both generic opening traces; retained profile-3 vectors; and the unchanged permit/domain/input-budget cases.

The next action is to merge the two independent round-2 verdicts for this exact document revision. This GO accepts the reviewed interface/design only and authorizes no source implementation, dependency change, native qualification, activation or release.
