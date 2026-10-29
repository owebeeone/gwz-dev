# GWZ Connection API contract — SAFETY-AXIS REVIEW, remediation round 2

**Review object:** `dev-docs/GwzConnectionApiContract.md`, revision 2, and `dev-docs/GwzConnectionApiContract-RemPlan-2.md` at root `f4aff9b0b6524711b5065eb0d088e41ef43ecabe`; DRAFT dated 2026-10-19. Interface/design gate only.

**Baseline:**

| Repository | SHA |
| --- | --- |
| root | `f4aff9b0b6524711b5065eb0d088e41ef43ecabe` |
| gwz-core | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| gwz-py | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| gwz-cli | `5193db962e672c54cd20ab2ef8448d904129a559` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| gwz-transport | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| gwz-sspi | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

The object and substantive reference evidence were read at pinned revisions. The complete revision-2 contract delta was inspected using `git diff 011d3eb0a487dbf62e10432034a0105be2518a22 f4aff9b0b6524711b5065eb0d088e41ef43ecabe -- dev-docs/GwzConnectionApiContract.md`.

**Date:** 2026-10-19  
**Axis:** Safety — ownership, cancellation, cleanup, ordering, sustained bounds, authentication, physical faults, deadlines and scope. Independent, adversarial, read-only. Nothing here relies on the parallel reviewer’s current-round report. Filed verbatim by the lane owner.

**Verdict: GO** — both prior Safety findings are closed at the design gate. Zero open P0, P1, P2 or P3 findings. This accepts the reviewed interface/design scope only.

---

## Prior-finding closure

| Finding | Result | Independent closure evidence |
| --- | --- | --- |
| Safety P2-1 — held AdmissionPermit lacks a revoked-registration outcome | **Closed; closure preserved** | Lines 221–244 retain Reserved/Registered revocation under the registration authority mutex. A revoked permit yields a terminal NeverRegistered Operation, with Effect::None and no adapter effects. Both registration-versus-revocation orders remain explicit. |
| Safety P2-2 — AppliedEvent issuance lacks a byte-application commit boundary | **Closed** | Lines 348 and 359–382 replace the owned event handoff with a synchronous borrowed CandidateEvent callback. Actual mutation precedes sequence advancement and Applied. The candidate cannot escape; WouldBlock retains the unchanged event and charge. Cancellation shares the lane guard. Lines 395–410 apply this rule to OrderedStreamMachine. |
| Safety P2-2 — required counterexample coverage | **Closed at specification level** | Lines 440–445 and matrix line 832 require the held-predecessor, successor/terminal, other-lane and cancellation-before/after-commit cases against the actual callback seam. No executed proof is claimed. |

Safety P2-1 remains non-architectural. Safety P2-2’s previously recorded architectural root is resolved by this correction. **NEW ARCHITECTURAL roots identified in this round: zero.** The review does not add to the two roots recorded in the remediation plan.

## Changed-range analysis

The entire changed range was reviewed.

- The revision annotation changes from revision 1 to revision 2.
- Section 6.1 replaces `take_applied() -> AppliedEvent` with `apply_ready(lane, callback)`.
- The added commit-boundary clauses require actual bounded mutation inside a synchronous, exclusive lane callback. Receipts contain identifiers/progress rather than unapplied work.
- WouldBlock explicitly has no effect, preserves sequence position and retains the event’s charge.
- Cancellation and abortive fencing share the application guard. Rejection or unwind faults the lane without Applied or retrying uncertain mutation.
- OrderedStreamMachine consumes the borrowed candidate only inside that callback.
- The proof traces, invariant matrix and clause-specific amendment row consistently adopt the corrected boundary.

No owner, authentication mechanism, native primitive, dependency, feature activation or wire schema is added by this round.

The same Safety reviewer continues under the operator’s recorded instruction to use the old reviewers. No additional reviewer was created.

## 0. Evidence base

This round inspected:

- The complete contract diff from root `011d3eb0a487dbf62e10432034a0105be2518a22` to `f4aff9b0b6524711b5065eb0d088e41ef43ecabe`.
- The complete `GwzConnectionApiContract-RemPlan-2.md`.
- Revision-2 contract lines 163–244, 333–450 and 815–865.
- The prior Safety reports and the original authority/reference evidence retained in this reviewer’s context.

The retained source evidence includes:

- Transport `src/sequenced.rs:255–258` and `525–613`: existing atomic apply callback, sequence advancement and Applied resolution.
- Transport `src/stream/incoming.rs:4–150`: exact byte offsets, credit and terminal prerequisites.
- Core’s authoritative archived `GwzTransportSequencedStreamDesign.md:17–43`: application-versus-buffering distinctions, terminal fences and retained ownership.
- Python/core operation-runtime construction and shutdown, retained-runtime reuse fixtures, lifetime mux bounds, key-registry accounting and the documented per-host native spawn-gate limitation.

The original authority review covered root instructions, process rules and review-loop skill; the accepted Connection remediation plan; reuse design and amendments; crate-map/library boundaries; canonical core requirements/design; transport, Git and current native-authentication contracts.

All nine heads matched the required tuple at the start and end.

Only read-only inspections were performed. No files, builds, tests, network operations, mutations or further agents were created. No current-round peer report or excluded companion draft was read.

## Problem, requirement provenance, and scope

The requested outcome remains a concrete, independently testable, Git-independent communication contract with persistent ownership and reuse, explicit authentication boundaries, actual gwz-git consumption, and structural prevention of the observed ownership churn.

The underlying problem remains supported by source evidence: Python enters an operation-owned runtime that core ultimately shuts down, while existing reuse fixtures retain their runtime explicitly. Simply retaining that runtime indefinitely would encounter its lifetime registration and bootstrap constraints.

The revised callback boundary directly addresses Safety P2-2. It uses the existing kernel’s supported atomic application model instead of introducing another event-acknowledgment protocol. This is a localized correction within the authorized contract scope.

The neutral start seam remains justified by the Git-independent requirement. Generic SSH/HTTP callers must reach ordered byte streams without manufactured Git metadata or a version cast. The selected factoring preserves one ordering algorithm and retains the authored Git profile as its wrapper.

No dependency upgrade, new repository, daemon, physical carrier or whole-session programme becomes a prerequisite. The editable boundary remains the contract and review records; source repositories and dependencies remain reference-only. Implementation, native qualification, production migration and release acceptance remain later gates.

No unsupported prerequisite or unauthorized scope expansion was established in this round.

## 1. Findings

No open findings.

## 2. Invariant analysis

**Original P2-2 trace: predecessor held before byte application.**  
Sequence 1 remains kernel-owned while eligible. If its callback returns WouldBlock, it mutates nothing and retains the same sequence position and charge. Sequence 2 and orderly terminal 3 cannot enter their application callbacks or become Applied ahead of it. There is no owned token available for another task to apply first. The original trace therefore cannot reach its erroneous offset failure.

**Application succeeds before acknowledgment.**  
The callback performs the actual byte/control transition. Only success advances the sequence and publishes Applied. A receipt cannot represent work still awaiting host-scheduled application. OrderedStreamMachine’s byte-offset and credit checks remain part of that transition.

**WouldBlock preserves other-lane progress.**  
The guard is per lane, and callbacks cannot block, await, perform I/O or invoke foreign code. A blocked predecessor returns WouldBlock rather than retaining an executing callback. Another lane can apply its own ready work. This preserves independent progress without making host scheduling responsible for same-stream correctness.

**Cancellation before and after commit.**  
Cancellation uses the same lane guard. If its fence wins, the uncommitted candidate is superseded and cannot survive the callback scope. If byte application wins, the committed prefix precedes cancellation. Neither outcome permits delayed post-fence application.

**Rejection and unwind cannot become replay.**  
A rejected transition or callback unwind faults/fences the lane without publishing Applied or retrying potentially mutated state. WouldBlock cannot conceal partial mutation. The correction therefore does not replace the original ordering defect with duplicate application after an uncertain callback result.

**Physical readiness remains separate from ordering.**  
RequestStart admits bounded connector work only while its operation claim is live. ReplyStart success follows physical open/authorization. Data cannot mutate byte state while the machine is unavailable; it remains retained under WouldBlock. Failed starts and cancellation fence buffered traffic while physical cleanup remains owned and charged.

**Permit revocation remains closed.**  
Acquire P, construct an unpolled close, then register/open P: the result remains terminal NeverRegistered with no adapter effects. Owner drop, driver failure and domain close/drop use the same revocation rule. Registration winning the race is included in registered cleanup accounting; revocation winning prevents live publication.

**Final retained-cleanup ownership remains intact.**  
With host A pending and host B complete, final domain close/drop does not release A’s generation. Dropping tickets and ordinary handles leaves the RetentionPort responsible until verified disposal or process exit. B’s completion cannot settle A’s permit, and a replacement host cannot reset retained accounting.

**Unchanged safeguards remain applicable.**  
The patch preserves bounded admission before worker creation, binding-local lifetime limits, stale-generation rejection, operation-local cancellation, honest physical-fault scope, SSH possession/trust checks, HTTP request and connection-bound security scopes, process-safety qualification, fixed deadline rules and conservative effect/replay accounting.

## 3. Risks and next action

The contract still depends on future implementation evidence: actual callback atomicity and resource charges, retained cleanup after owner destruction, native process safety, sequenced-profile conformance, and real CLI/Python reuse. These remain explicit gates rather than accepted implementation claims.

**Next action:** merge the two review verdicts and record acceptance of the exact revision-2 interface/design tuple if both axes are GO. Any portable proof implementation requires its separately authorized boundary.
