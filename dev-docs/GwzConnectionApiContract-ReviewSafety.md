# GWZ Connection API contract — SAFETY-AXIS REVIEW

**Review object:** `dev-docs/GwzConnectionApiContract.md` at root `d0848ed0fc0fc090cd7dde1a112e085b7e58505f`; revision 0, DRAFT dated 2026-10-19. Interface/design gate only.

**Baseline:**

| Repository | SHA |
| --- | --- |
| root | `d0848ed0fc0fc090cd7dde1a112e085b7e58505f` |
| gwz-core | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| gwz-py | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| gwz-cli | `5193db962e672c54cd20ab2ef8448d904129a559` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| gwz-transport | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| gwz-sspi | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

The object, substantive reference documents and source extracts were read using `git show` at the pinned revisions.

**Date:** 2026-10-19  
**Axis:** Safety — cancellation, ownership, retained cleanup, sustained bounds, authentication, ordering, physical faults, deadlines and scope. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks; zero P0, P1 or P3 findings. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

Read and inspected:

- The complete review object, lines 1–684.
- Root `AGENTS.md` and `AGENTS_GWZ.md`.
- `AgentProcessRules.md`, particularly amendment discipline, exact-tuple review, actionable findings and independent closure; `GwzProcessOptimization.md` §§3–4 and §8.
- `/Users/owebeeone/.claude/skills/review-loop/SKILL.md`.
- `GwzConnectionRemediationPlan.md` §§1–8, its merged verdict, and its prior Safety report as historical evidence.
- `GwzConnectionReuseDesign.md`, particularly §§1–8: observed runtime defects, instance/binding ownership, configuration equality, cross-operation proofs, capacity, lifetime and fault scope.
- `GwzCoreSessionCrateMap.md` §§1–4 and its amendment notices; `GwzLocalCloneLibraryBoundaries.md` §§1–2.
- Targeted clauses in core’s `GWZRequirements.md` and `GWZDesign.md` covering in-process operation, captured context, transport composition, credentials and sequenced delivery.
- Core’s `GwzRemoteTransportRequirements.md`, particularly G5–G7, C1–C8, P1–P9 and S1–S9; corresponding binding, lifecycle, replay and timeout clauses in `GwzRemoteTransportDesign.md`.
- Core’s `GwzGitLibraryDesign.md` §§Decision, Ownership, Placement and G0; `GwzGitLibraryApi.md` opening and ownership surface.
- Core’s `GwzTransportTR18-OperatorDecisions.md`, including the accepted authentication matrix, helper/native distinctions, launch-window decisions and later platform rulings.
- Root `GwzWindowsHttpsIntegrationDesign-DRAFT.md` §4 and qualification obligations; `GwzSspiDesign.md` caller, conversation, deadline and retained-cleanup clauses.
- Core’s archived authoritative `GwzTransportSequencedStreamDesign.md`, particularly §§1–5.
- `CurrentProgramCheckpoint.md` opening entries as historical context, without treating its older source inventory as this review’s tuple.

Focused source inspections:

- Python `native/src/route/transport.rs:131–210`: captured operation inputs and invocation of the cancellable transport entry.
- Core `src/transport_host/cancellable.rs:213–311`: construction and final shutdown of an operation-owned runtime.
- Core `src/transport_host/driver_tests.rs:75–160`: reuse and overlap tests explicitly retaining one runtime.
- Transport `src/mux/mod.rs:242–294`: lifetime request tombstones and binding bootstrap.
- Transport `Cargo.toml:13–29`, `src/lib.rs:14–24`, and `src/sequenced.rs:1–145`: pure-kernel boundary and explicitly unstable, non-default profile-3 implementation.
- Core `src/git/endpoint/ssh_key_snapshot.rs:14–146`: finite key-registry charge and explicit-key byte identity.
- Core `src/git/spawn_gate.rs:1–95`: documented per-host exclusion and its process-wide limitation.

All nine heads matched the required tuple at the start and end. Only read-only inspection commands were used. No files, builds, tests, network operations, repository mutations or further agents were created. No current-round peer report or excluded companion draft was read.

## Problem, requirement provenance, and scope

The operator requested a concrete API contract and review that addresses the observed churn: a Git-independent SSH/HTTP(S) communication library, explicit persistent ownership, independent tests, authentication and trust boundaries, actual gwz-git consumption, and useful reuse through CLI/Python consumers.

The central defect is evidenced by the current source. Python passes each operation into a cancellable entry that constructs and ultimately shuts down its own transport runtime. The existing reuse fixture retains a runtime explicitly. It therefore demonstrates a supported reuse mechanism, while also showing why that fixture does not establish reuse through today’s Python lifecycle.

Keeping the existing runtime alive indefinitely is insufficient: the mux retains request tombstones up to its lifetime limit and owns one bootstrap session. The contract’s separation of persistent endpoint instances from operation bindings addresses that concrete problem without requiring a replacement application session subsystem.

The package placement is a selected implementation proposal, expressly subject to later authorization. The current editable boundary is the contract and its review records. Core, transport, Git, CLI, Python and SSPI remain reference-only implementation candidates; git2-rs, libgit2 and other dependencies remain reference-only dependencies.

No dependency upgrade, new repository, daemon, physical carrier or whole-session migration is established as a prerequisite. Existing in-process delivery, retained-runtime reuse and narrow adapter seams support the proposed portable proof. The contract also keeps native process-safety qualification separate from that proof and does not substitute fail-closed native unavailability for the promised release parity.

P2-1 concerns an unresolved transition inside the authorized API design. Its correction requires neither broader scope nor a different architecture.

Optional external shortcomings remain separate: the existing native launch gate is per host, and the sequenced kernel is unreviewed and non-default. The contract accurately records both limitations and places qualification gates before their production claims. Neither observation authorizes source changes or blocks reviewing the portable interface.

## 1. Findings

### [P2-1] A held AdmissionPermit has no defined registration outcome after host revocation

**Location:** `GwzConnectionApiContract.md:93–97`, `177–193`, `203–210`, and `558–561`.

**Violated invariant:** Host shutdown must synchronously prevent subsequent effectful operation creation, and every admission/registration outcome must preserve explicit ownership provenance.

The contract specifies that `AdmissionPermit::register` is infallible and changes `Reserved -> Registered` under one lock. Host close/drop seals admission and cancels operations. It does not specify what happens to an already acquired permit that has not registered, or how that permit observes a host-progress failure.

A credible sequence is:

1. A caller successfully acquires permit P. P owns an active reservation and preallocated registration storage, but no Operation has been registered.
2. Another task calls `owner.close()`. Its synchronous sealing transition occurs before the returned close future is polled. There are no registered operations to cancel.
3. P remains alive outside the host’s operation table.
4. The caller invokes `P.register()`. The specified registration rule publishes a Registered binding and returns an Operation; it contains no revoked-permit branch or terminal return rule.
5. The caller opens an exchange through that Operation.

The document does not determine whether step 4 refuses registration, returns an already terminal Operation, or publishes a live binding. Its general shutdown promise suggests refusal, but the normative signature and registration transition do not express the outcome. If implementations follow the listed transition without an additional revocation rule, work can be registered after shutdown’s cancellation sweep. If implementations independently invent a rule, the supposedly frozen ownership contract has left consequential behavior to implementation.

The same gap exists when the host progress driver dies between permit acquisition and registration.

**Impact:** A shutdown race can create work outside the completed cancellation sweep, or produce inconsistent registration and cleanup behavior across adapters. This is an interface defect; it is not a claim that current code implements the proposed API or already exhibits this failure.

**Required correction:** Define permit revocation and registration together with host close/drop and driver failure under the same authority transition.

The smallest correction can preserve the synchronous, infallible signature: explicitly revoke outstanding Reserved claims, require `register` on a revoked claim to return an already sealed terminal Operation without publishing active work, and define its outcome and exactly-once reservation release. Alternatively, select and document a fallible registration result with equally explicit provenance.

Whichever shape is selected must state that no permit obtained before revocation can later produce an effect-capable binding, and that this guarantee holds with close unpolled or dropped.

**Smallest closure/regression tests:**

- Acquire P, construct an unpolled HostClose, then register P and attempt an open. Assert the selected terminal/refusal result, zero endpoint/auth/network effects, and exactly-once reservation release.
- Repeat with HostOwner drop and host-progress failure.
- Barrier-race register against close on both sides of the registration linearization point. Either the binding belongs to shutdown’s cancellation accounting, or registration takes the revoked-permit path; no third outcome is allowed.

At this design gate, closure requires the explicit transition and corresponding proof specification. Execution belongs to the portable-owner implementation gate.

**NEW ARCHITECTURAL root cause: NO.** This is a missing revocation transition within the selected owner/permit architecture. No new owner, dependency or subsystem is required.

## 2. Invariant analysis

**Registration and unwind provenance.**  
The preallocated claim and its insertion-point update address the earlier “call failed to return, therefore NotRegistered” mistake. Registration performs no I/O, callbacks or fallible allocation, and the unwind guard uses the recorded claim. The remaining failed attack is P2-1: the pre-registration claim’s relationship to host revocation is unspecified.

**Cancellation is distinct from physical cleanup.**  
Dropping an Opening cannot abandon native ownership; late success must be disposed or verified. Exchange abandonment cannot assert healthy reuse. Pending cleanup retains its physical permits in an externally owned CleanupDomain, and tickets have no release authority. Duplicate or late completions cannot settle a different generation. These rules defeat cancellation-as-disposal and replacement-host capacity bypass traces.

**Operation faults and physical faults have different scopes.**  
Logical cancellation affects the operation’s binding and leases. Physical connection failure discards that connection; endpoint-worker failure faults the instance and its affected exchanges. Host-driver death seals the host while retaining honest cleanup ownership. Thus the contract does not falsely promise that operations sharing physical owners are immune to those owners’ failure.

**Sustained operation does not depend on host rollover.**  
The host supplies fresh binding identities; the 256-request and 4,096-stream lifetime limits remain binding-local. Retired binding traffic cannot create records, and tombstones retire with settled bindings. Admission precedes worker creation, with synchronous queue refusal. Caller-owned terminal results, finite diagnostics, charged AuthContext retention and domain-owned cleanup provide explicit bounds against the audited registration-exhaustion and queued-thread counterexamples.

**Authentication remains tied to the requesting operation.**  
Profile equality compares canonical bytes after a digest hit. Explicit identity never falls back; SSH cross-binding reuse incorporates fresh possession/trust requirements and explicit-key equality. Request-scoped HTTP authentication does not inherit an account from TLS reuse. Connection-bound native authentication retains caller/origin/connection-generation scope, and final-origin TLS validation and CBT precede credential publication. Provider charge and retention conformance prevent treating an opaque or cloned capability as free storage.

**Native launch safety is honestly gated.**  
The existing source explicitly admits that different hosts have different gates. Section 7.3 rejects that as proof about arbitrary inheriting spawns and refuses an unqualified native path before effects. It preserves the Windows parity obligation while avoiding an unsupported process-library prerequisite for the portable proof. No native-safe implementation is accepted by this review.

**Ordering belongs to the protocol.**  
The contract requires sequenced-profile processing before stream effects, reserved control capacity, bounded reorder state and terminal fences. The incorporated design distinguishes Buffered from Applied, pins port generations, rejects stale context, and forbids orderly close from skipping accepted data. Its current implementation remains behind a separate conformance gate; document GO cannot activate it.

**Stream progress does not authorize replay or imply Git success.**  
Partial writes expose accepted progress; cancellation cannot retract an accepted prefix. EOF requires validated completion. SSH exec/HTTP-engine submission establishes possible effect even without acknowledgment, preventing stale replacement from becoming ambiguous mutation replay. Git result interpretation remains in gwz-git.

**Deadlines do not silently renew.**  
One clock origin, disabled-timeout representation, named pause/requeue transitions and immutable native-conversation cutoffs address repeated polls, redirects, 401s, reused connections and native rounds. Cleanup remains independently bounded when network timing is disabled. The later helper-Basic and adaptive-requeue amendments are preserved explicitly.

## 3. Risks and next action

The remaining implementation risks are substantial but correctly gated: adapter cleanup receipts are trusted claims requiring conformance; native process safety requires focused qualification; profile-3 ordering requires its own proof; and real CLI/Python reuse requires production-consumer evidence rather than an internal fixture.

**Next action:** amend the permit/host-revocation transition and add the three closure cases for P2-1, then return the revised pinned contract for focused Safety re-verdict. No source implementation or dependency change is required to close this design finding.
