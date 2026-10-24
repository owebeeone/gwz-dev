# GWZ Connection API contract — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzConnectionApiContract.md`, revision 1, DRAFT, and `dev-docs/GwzConnectionApiContract-RemPlan-1.md`, committed at root `011d3eb0a487dbf62e10432034a0105be2518a22`.

**Baseline:**

| Repository | Pinned HEAD |
| --- | --- |
| `.` | `011d3eb0a487dbf62e10432034a0105be2518a22` |
| `gwz-core` | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| `gwz-py` | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| `gwz-cli` | `5193db962e672c54cd20ab2ef8448d904129a559` |
| `gwz-git` | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| `gwz-transport` | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| `gwz-sspi` | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| `git2-rs` | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| `libgit2` | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All nine HEADs matched at the beginning and end. Documents and substantive source were inspected through pinned Git objects.

**Date:** 2026-10-19.

**Axis:** Consistency, including API Surface: controlling-contract agreement, interface composition, lifecycle completeness, scope authorization and recurrence prevention. Independent, adversarial, read-only. Nothing here relies on the other axis’s current-round report. Filed verbatim by the lane owner.

**Verdict: GO** — the original Consistency P2-1, P3-1 and P3-2 are closed at the design/interface level. No new P0–P3 findings. This accepts the reviewed contract, not implementation, feature activation, native qualification or release.

---

## Prior-finding closure table

| Prior finding | Result | Independent retrace and evidence |
| --- | --- | --- |
| **Consistency P2-1 — generic exchanges lacked an entry into the required sequencing kernel** | **Closed for this design gate** | Revision 1 §6.1, lines 333–417, selects a neutral StreamOrder start/ticket interface and a version-free OrderedStreamMachine. Generic opening metadata stays in Connection’s bounded typed table; no GitService is manufactured. Existing Git envelopes remain a validated wrapper. The new byte entry consumes ordered tokens rather than rewriting a profile-3 envelope for today’s profile-1/2 machine. The non-Git SSH and HTTP traces now name the opening, reply, reordering and byte-application transitions. Section 15, line 827, records the affected sequencing/byte-entry amendment. |
| **Consistency P3-1 — CleanupDomain lacked its final lifecycle surface** | **Closed** | Lines 87–91 define construction and owner close; lines 129–132 define ownership and immediate sealing; lines 170–186 specify retention beyond host/domain-owner drop. The two-host teardown recipe at lines 748–756 resolves the original A-pending/B-complete case without releasing A’s generation or requiring its caller to keep polling. |
| **Consistency P3-2 — InputBudget was missing from AdmissionError** | **Closed** | Lines 675–689 explicitly place retained-charge refusal in AdmissionError and distinguish invalid build-time limits as `BuildError::InvalidConfig(InputBudget)`. An over-budget AuthContext now has the prescribed refusal before registration, endpoint construction or effect. |

These are reviewer closures of document counterexamples. No implementation test execution is claimed.

**Architectural-root accounting:** No new architectural root is identified in this round. The previously identified generic sequencing-composition root remains one historical root and is now resolved in the contract.

## Changed-range analysis

The whole contract diff from `d0848ed0fc0fc090cd7dde1a112e085b7e58505f` to `011d3eb0a487dbf62e10432034a0105be2518a22` was read, together with the complete remediation plan.

The changed range was attacked in four groups:

1. **Neutral sequencing and byte-stream composition.** The revision makes an explicit pure-kernel interface change instead of assuming the pinned kernel already supports generic opening. It separates start authority, ordered ownership and byte validation. It also preserves a Git-envelope wrapper and requires one factored ordering algorithm.
2. **Acquired-permit revocation.** The added state transition covers Reserved claims as well as registered operations. Consuming a revoked permit produces a terminal NeverRegistered operation, preserving the infallible register signature without publishing work after shutdown.
3. **Final retained-domain lifecycle.** The new owner/handle pair and mandatory RetentionPort provide the missing teardown recipe. Retention must be reserved before effects and survives loss of public tickets and domain handles.
4. **Errors, walkthroughs, matrix and amendments.** The added error categories agree with the new terminal states; the examples cover final domain shutdown; the matrix includes the original counterexamples; the sequencing amendment states that its seam is new implementation work.

The remediation plan’s use of the same two reviewers follows the recorded operator instruction “use the old reviewers.” No third reviewer was introduced, and the implementer’s dispositions were not treated as evidence of closure.

## 0. Evidence base

This round inspected:

- The entire pinned contract diff.
- Revision 1’s changed sections and surrounding ownership, admission, stream, error, walkthrough, test-gate and amendment provisions:
  - §§4–5, lines 77–265;
  - §6 and §6.1, lines 285–417;
  - §§11–16, lines 673–845.
- `GwzConnectionApiContract-RemPlan-1.md`, lines 1–44.
- The unchanged pinned transport source underlying the original counterexample:
  - `src/sequenced.rs:84–109`: first-message and terminal classification;
  - `src/sequenced.rs:157–219`: profile-3 enqueue, first-message checks, sequence allocation and bounded queues;
  - `src/sequenced.rs:339–372`: receive/application boundary;
  - `src/stream/mod.rs:83–115`: today’s profile-1/2 byte-machine configuration restriction.
- The incorporated accepted `gwz-core/dev-docs/history/reference/GwzTransportSequencedStreamDesign.md:7–43`, covering wire numbering, start authority, application receipts, two-direction terminals, reorder budgets and compatibility.
- The unchanged authority/source evidence established in the initial review: root instructions and process amendments; accepted Connection remediation plan and verdict; reuse design; crate map and library boundaries; canonical core requirements/design; remote transport requirements/design; Git library design/API; Windows HTTPS integration and native composition contracts; operator decisions; actual Python/core runtime construction and teardown.

Read-only commands were `git rev-parse`, pinned `git show`, `git diff`, and text inspection. No files were written, tests/builds run, network accessed, dependencies changed or additional agents launched. The other reviewer’s current-round report was not accessed.

## Problem, requirement provenance, and scope

The original requested outcome remains a concrete communication API that prevents recurring ownership churn while supporting a useful production transport: Git-independent tests, explicit persistent ownership, independent operations, explicit credential/trust boundaries and production consumption through gwz-git.

The observed problem remains established by the unchanged pinned route. Python’s transport route gives each operation its own runtime, and core’s cancellable entry owns that runtime’s finish and shutdown. Keeping the native authentication host alive therefore does not preserve the communication pool. The accepted reuse design also explains why retaining today’s runtime unchanged is insufficient: finite lifetime registrations and operation-scoped state must be separated from physical owners.

Revision 1 remains a documentation proposal within the authorized boundary. It changes neither source nor dependencies. Its selected kernel factoring is traceable to the original generic, Git-independent and protocol-ordered communication requirements, and to the concrete first-message/version mismatch demonstrated in the initial review. It is not an unrelated infrastructure prerequisite.

The selected correction is bounded: keep the current authored Git envelope as a profile wrapper, factor ordering into a neutral kernel interface and add a byte-state entry that accepts ordered events. This does not require a generic serialized opening schema, new physical carrier, new repository or whole-session implementation.

The retained-domain addition completes an already selected ownership responsibility. A stepped fake can supply retention in the portable proof. Native retention qualification remains a separate obligation; it is not silently made a prerequisite for the fake proof.

Only contract/review records are editable in this lane. All implementation repositories and dependencies remain reference-only. No missing scope authorization or unsupported prerequisite blocks this revised design. Existing native/platform shortcomings remain later qualification obligations, not newly authorized work.

## 1. Findings

No new findings.

All three prior Consistency findings are closed as recorded above. The additional registration-revocation changes were independently checked on this axis; their acceptance here does not rely on a current-round peer verdict.

## 2. Invariant analysis

### Generic opening and ordered byte delivery

The original counterexample now has a complete design route:

- A registered Operation owns bounded opening metadata.
- `reserve` charges the lane before start authority exists.
- RequestStart carries a ticket for prevalidated metadata and can enqueue connector work only while the operation claim remains live.
- A blocked connector queue preserves that lane’s start position while another lane progresses.
- ReplyStart success follows physical open/authorization; only then does Opening yield an Exchange.
- The selected OrderedStreamMachine consumes moved AppliedEvent tokens for the matching lane.
- Body data arriving before readiness remains buffered. The application boundary returns WouldBlock rather than minting an applied token and losing it afterward.
- Reordered data reaches the byte transition only after its sequence predecessors. Byte-offset/window validation remains independently required.
- Orderly terminals retain their predecessor requirements; abortive starts/cancellation use the retained terminal-fence rules. Physical cleanup remains the adapter’s separate receipt.

The generic SSH `printf probe` trace consequently requires no fabricated Git metadata. The generic HTTP trace has its status/head from the HTTP adapter and body events through the same neutral order core. Stalling another RequestStart does not impose a host-wide ordering barrier.

The Git wrapper preserves its authored wire first messages and numbering while mapping them to the neutral start origin. This is a mapping between interfaces, not an on-wire renumbering or version cast. The selected version-free byte entry resolves the separate fact that today’s stream configuration admits only profiles 1/2.

The revision expressly requires factoring one algorithm and running both existing profile-3 vectors and generic traces against it. It does not claim the new interfaces or their qualification already exist.

### Reserved claims cannot revive a closed host

The acquired-permit attack now has two explicit legal outcomes under one authority mutex:

- **Register wins:** the claim becomes Registered and is included in revocation’s cleanup accounting.
- **Revocation wins:** the claim is revoked, its reservations are released once, and subsequent register returns a sealed NeverRegistered Operation.

An unpolled close is sufficient to revoke. The terminal Operation’s open returns the specific reason with Effect::None, and finish preserves registration provenance plus input-cleanup status. Dropping it cannot release the same reservation again.

Domain close/drop and host-progress failure participate in the same transition. Captured-input settlement and destructors remain outside locks while their charge stays retained. The added rule therefore closes the late-insertion ambiguity without making register fallible or calling adapters during registration.

### Domain teardown preserves physical ownership

The original two-host case is now resolved:

1. A and B are built with clones of the same domain capability.
2. A closes with pending disposal; its generation remains charged.
3. B closes successfully without clearing A’s record.
4. Final domain close seals all hosts and returns Complete or Pending within one overall bound.
5. Dropping tickets, handles and the domain owner does not stop the retained cleanup service or free A’s physical permit.
6. Confirmed late disposal updates the matching statuses and releases that generation once.

Retention reservation precedes native effects. Failure to reserve it refuses work before effects. The retainer has cleanup authority without new-work admission authority. This completes the public lifecycle while keeping native lifetime claims subject to qualification.

### Error and public-surface coherence

The revised construction walkthrough starts with explicit domain construction, installs its capability in configuration before host build, uses independent operation admission/registration, then closes host and domain owners. The configuration builder is configuration assembly before construction, not a late repair of an already-created host.

InputBudget now has a defined admission outcome. Build-time invalid limits have a separate construction classification. Revoked-permit terminal reasons are explicitly representable on open and in terminal provenance.

The unchanged exchange surface retains once-only HTTP response-head retrieval, wrong-scheme/state outcomes, stream half-close, finish/abort, cancellation and host shutdown. Application code still cannot return an uncertain lease as healthy.

### Scope, compatibility and prevention

The correction retains:

- separate persistent owners and operation bindings;
- bounded admission before worker creation;
- generation-qualified completion and stale-event rejection;
- finite per-binding identifier/tombstone state;
- operation-local capacity, retry, authentication and observations;
- protocol-owned ordering independent of the scheduler;
- caller-independent cleanup progress;
- distinct portable, native and actual-consumer gates.

Profile-1/2 interfaces remain compatibility paths. Existing profile-3 terminal, reorder, queue and generation rules remain conformance requirements. No feature is activated, and the document does not turn its selected interface into a claim that source or native behavior has been qualified.

## 3. Risks and next action

The principal implementation risk is preserving application receipts, byte semantics, terminal fences and resource charges while factoring the current ordering code. The selected design makes that risk testable; it does not prove the factoring correct in advance.

The smallest later implementation evidence remains: non-Git SSH and HTTP fake traces through the actual factored kernel; existing profile-3 conformance vectors; both register/revocation race orders; retained two-host cleanup after all public handles drop; and over-budget admission with zero effects. Native and production-consumer proofs remain separate gates.

The next action is to merge the two independent re-verdicts for this exact document revision. This GO authorizes no source implementation, dependency change, native qualification, activation or release.
