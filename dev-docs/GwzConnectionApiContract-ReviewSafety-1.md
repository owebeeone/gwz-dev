# GWZ Connection API contract — SAFETY-AXIS REVIEW, remediation round 1

**Review object:** `dev-docs/GwzConnectionApiContract.md`, revision 1, and `dev-docs/GwzConnectionApiContract-RemPlan-1.md` at root `011d3eb0a487dbf62e10432034a0105be2518a22`; DRAFT dated 2026-10-19. Interface/design gate only.

**Baseline:**

| Repository | SHA |
| --- | --- |
| root | `011d3eb0a487dbf62e10432034a0105be2518a22` |
| gwz-core | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| gwz-py | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| gwz-cli | `5193db962e672c54cd20ab2ef8448d904129a559` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| gwz-transport | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| gwz-sspi | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

Documents and substantive source were read using pinned `git show`; the complete contract delta was read using `git diff d0848ed0fc0fc090cd7dde1a112e085b7e58505f 011d3eb0a487dbf62e10432034a0105be2518a22 -- dev-docs/GwzConnectionApiContract.md`.

**Date:** 2026-10-19  
**Axis:** Safety — ownership, cancellation, cleanup, ordering, sustained bounds, authentication, physical faults, deadlines and scope. Independent, adversarial, read-only. Nothing here relies on the parallel reviewer’s current-round report. Filed verbatim by the lane owner.

**Verdict: NO-GO** — original Safety P2-1 is closed; one new P2 finding blocks. Zero P0, P1 or P3 findings. I pre-commit to GO on a revision resolving P2-2 as specified.

---

## Prior-finding closure

| Prior finding | Result | Counterexample re-run against revision 1 |
| --- | --- | --- |
| Safety P2-1 — held permit can outlive host revocation without a defined registration outcome | **Closed at the design gate** | Lines 221–244 cover Reserved and Registered claims under the same authority mutex. Unpolled close, owner drop, driver failure and domain close/drop revoke a held permit. Subsequent register returns a terminal NeverRegistered Operation; open returns the terminal reason with Effect::None and no adapter call. Reservations release exactly once. |
| Safety P2-1 — registration versus close barrier race | **Closed at the design gate** | Registration winning produces Registered(binding_id) included in revocation cleanup. Revocation winning produces NeverRegistered without live publication. Lines 238–244 expressly exclude a late insertion after the cancellation sweep. Matrix line 786 requires both orders. |

These are textual design closures. No implementation or executed regression result is claimed. The original finding remains non-architectural.

## Changed-range analysis

The whole changed range was reviewed.

- **Domain lifecycle, lines 87–91, 126–132 and 170–186:** adds an owner/handle split and mandatory retained-work service. Final close/drop now has an explicit destination for pending ownership after every ordinary handle and ticket disappears.
- **Permit revocation, lines 221–244:** resolves the original Safety counterexample without changing the infallible register signature.
- **Neutral sequencing seam, lines 333–417:** gives generic SSH/HTTP starts a Git-independent route through the factored ordering kernel. It also introduces a new split between issuing an AppliedEvent and performing the byte-state transition. That split is P2-2.
- **Errors and walkthrough, lines 675–689 and 721–756:** make retained-charge refusal and final domain teardown observable.
- **Proof matrix and amendments, lines 784–800 and 827:** add the new closure cases and identify the sequencing clauses being amended.

The review continues with the same Safety reviewer under the operator’s recorded “use the old reviewers” instruction. No additional reviewer was created.

## 0. Evidence base

This round inspected:

- The complete contract diff from revision 0 to revision 1.
- Revision 1 contract lines 77–282, 296–431 and 665–845 with line numbers.
- The complete round-1 remediation plan.
- The prior Safety report and the original authority/reference evidence retained in this reviewer’s context.
- Transport `src/sequenced.rs:255–258`, `525–613`: the current callback must perform the ordinary mux/stream transition atomically; Applied resolution and sequence advancement follow callback success.
- Transport `src/stream/incoming.rs:4–150`, especially `29–53` and `93–104`: byte-state application checks exact offsets and close prerequisites.
- Core’s authoritative archived `GwzTransportSequencedStreamDesign.md:17–43`: application precedes Applied resolution; buffered receipt is distinct from application; terminal and cleanup ownership remain charged.
- The unchanged baseline source observations supporting the original ownership, authentication, native-safety and scope analysis.

All nine repository heads matched the required tuple at both the start and end.

Only read-only inspections were performed. No files, builds, tests, network operations, mutations or further agents were created. No current-round peer report or excluded companion draft was read.

## Problem, requirement provenance, and scope

The requested outcome remains a concrete, independently testable, Git-independent communication contract with persistent reuse, explicit authentication boundaries, actual gwz-git consumption, and structural prevention of the observed ownership churn.

The evidence supporting that outcome is unchanged: today’s Python route enters an operation-owned transport runtime, while successful internal reuse tests explicitly retain their runtime. Retaining that existing runtime indefinitely would encounter its lifetime registration and bootstrap constraints.

The added neutral sequencing seam addresses a necessary consequence of the Git-independent requirement: generic SSH/HTTP operations cannot require manufactured Git metadata merely to enter ordered streams. Factoring the existing algorithm and retaining its authored Git wrapper is a localized remedy. It does not establish a need for a new carrier, daemon, dependency upgrade or whole-session programme.

Explicit final retention is likewise necessary to make the existing Pending-cleanup promise meaningful after owner destruction. A stepped fake suffices for the portable proof; native retention remains a qualification obligation.

The editable scope remains the contract and review records. All source repositories and dependencies remain reference-only. P2-2 can be corrected within that scope by defining the ordering/application commit boundary. It does not require source implementation to close this design gate.

## 1. Findings

### [P2-2] AppliedEvent issuance has no paired byte-application commit boundary

**Location:** `GwzConnectionApiContract.md:345–352` and `370–383`, contrasted with the retained ordering requirements at `311–324` and amendment line `827`.

**Violated invariant:** Applied must mean that the ordered state-machine transition occurred. Per-stream correctness must not depend on subsequent host scheduling preserving the order of separately owned tokens.

The new API exposes `take_applied() -> AppliedEvent`. The token is an owned, non-Clone event/receipt, and the byte machine subsequently consumes it. The text specifies readiness checking before minting the token, but gives no acknowledgment or atomic-consumption rule connecting that later byte transition back to StreamOrder’s sequence advancement and Applied result.

A concrete state trace is:

1. ReplyStart has activated the byte machine. Its next received byte offset is zero.
2. StreamOrder admits Data sequence 1 carrying byte A at offset zero and makes token T1 available.
3. Before T1 enters the byte transition, StreamOrder admits Data sequence 2 carrying byte B at offset one and makes T2 available. The defined interface has no outstanding-token acknowledgment preventing this advancement.
4. Separate progress work takes the owned tokens and T2 reaches the byte machine first.
5. The retained byte logic expects offset zero and reports Protocol for T2. A valid stream has failed solely because of scheduling after the ordering kernel.

A terminal can similarly reach byte-state handling before an earlier issued token has committed. Non-Clone prevents duplicating a token; it does not preserve the consumption order of distinct tokens or tie Applied resolution to actual consumption.

This is not a request to make malicious adapters safe. It identifies an omitted obligation at a newly introduced internal ownership boundary. The current receiver expressly requires its apply callback to perform the stream transition atomically or return WouldBlock without effect. It advances the expected sequence and publishes Applied only after callback success. Revision 1 replaces that clear boundary with an owned-token handoff without specifying the corresponding commit half.

The physical-readiness guard at lines 379–383 addresses minting a token before the byte machine exists. It does not address multiple issued tokens, application ordering, or terminal advancement after token issuance.

**Impact:** A conforming implementation lacks a defined mechanism for keeping byte-state application and sequence acknowledgment atomic. It must invent another protocol or depend on host scheduling, recreating the ordering dependency this contract is intended to remove.

**Required correction:** Preserve one explicit application linearization point.

The smallest supported route is the existing atomic callback model: StreamOrder grants a lane-bound application capability only within the synchronous byte-transition callback; successful byte application commits the sequence and then produces an Applied receipt. WouldBlock preserves ownership and sequence position without publishing Applied.

If owned tokens remain, specify their consumption/acknowledgment pair, finite in-flight ownership, invalidation and terminal interaction. Later events must not commit or become Applied ahead of an uncommitted predecessor. Merely instructing a host task to process outputs in order is insufficient for the stated protocol-owned guarantee.

**Smallest closure/regression tests:**

- Pause the first event between order eligibility and byte application, then submit its successor and orderly terminal. Neither may commit or report Applied ahead of the predecessor.
- Exercise WouldBlock at that boundary. The predecessor remains charged and retryable exactly once, while an unrelated lane progresses.
- Race cancellation with that boundary. The result is either an already committed prefix followed by cancellation, or superseded uncommitted work; no delayed token applies after its terminal fence.
- Retain the existing profile-3 vectors and both generic traces against the same factored implementation.

At this gate, closure requires the explicit interface rule and proof specification. Executed tests belong to the later implementation gate.

**NEW ARCHITECTURAL root cause: YES.** The new seam divides ordering commitment from byte-state mutation between separately owned values without defining their commit protocol. This is distinct from the original permit-revocation omission and the prior Git-dependent-start root recorded in the merged plan. One new architectural root is identified by this report.

## 2. Invariant analysis

**Held permits cannot revive revoked owners.**  
The original close/drop/driver-failure trace now has two explicit outcomes under one mutex. Captured-input settlement occurs outside that lock while retaining its charge. Unpolled or dropped close futures cannot reopen registration.

**Final domain teardown retains real ownership.**  
With hosts A and B sharing a domain, A may remain Pending while B completes. Domain close/drop seals both and returns within one overall bound. Dropping both tickets and every domain handle leaves A’s generation owned by the RetentionPort. B’s completion cannot release A’s permit; late verified disposal releases A once. Retention must be reserved before effects, so failed transfer cannot orphan existing native work.

**Generic start authority is bounded and operation-scoped.**  
PendingLane reserves storage without effect authority. Start tickets name prevalidated bounded opening records. Foreign or stale tickets cannot cause effects. RequestStart can enqueue connector work only while its claim is live; a full work queue blocks that lane while other lanes progress. ReplyStart success follows physical authorization, and Opening cannot yield an Exchange earlier.

**Physical readiness is explicitly gated.**  
Data arriving before ReplyStart is buffered. A non-ready byte machine causes WouldBlock before token minting; rejection or cancellation resolves the lane. This closes the specific premature-readiness trace, but does not close P2-2’s subsequent application-commit gap.

**Unchanged safety boundaries remain intact.**  
The patch does not relax generation routing, cleanup receipts, finite admission, operation-local cancellation, cross-binding SSH proofs, native connection-bound HTTP security scope, fail-closed process qualification, effect/replay boundaries, clock rules or real-consumer migration gates. The sustained-operation and native-qualification claims remain future proof obligations rather than claimed implementation results.

## 3. Risks and next action

Native retention and process safety still require focused qualification. The sequenced kernel remains non-default and unreviewed; neither this review nor a future document GO activates it.

**Next action:** define the atomic ordering-to-byte application boundary for P2-2, preserving the original permit closure and new retainer lifetime rules, then return the revised pinned contract for focused Safety re-verdict. No source or dependency changes are required for this correction.
