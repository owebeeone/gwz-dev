# DRAFT GWZ SSPI process boundary — SAFETY-AXIS REVIEW

**Review object:** DRAFT revision 1 of `dev-docs/GwzSspiDesign.md`, its implementation plan, caller guide and controlling graph at root `377c5e29882e353e42dbe75031b175374b213f7f`.

**Baseline:** root `377c5e29882e353e42dbe75031b175374b213f7f`; gwz-core `d78a664e3c5a325c6f12be409eb7645c1c1b51d0`; evidence `1beb1d204c824701ddbd033c7f89df9a3561f5e5`. Object and controlling documents were read through `git show` at these explicit commits. All three HEADs matched at the beginning and end of the full review.

**Date:** 2026-10-03

**Axis:** Safety: what the complete mechanism contract permits to go wrong across admission, identity, containment, IPC, secrets, deadlines, cancellation, cleanup and host lifecycle. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on their reports. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks; one P3 finding is nonblocking. I pre-commit to GO on a revision that resolves P2-3 as specified.

---

## 0. Evidence base

### Scope correction

The initial canonical prompt contained a stray instruction restricting this reviewer to the caller guide. An initial limited-scope report followed that restriction. The lane owner subsequently corrected the canonical prompt and explicitly instructed this reviewer to perform the full Safety review on the unchanged committed tuple.

This report supersedes that initial report as the Safety verdict. It preserves and dispositions both initial observations below. No current peer report or peer finding was supplied or read.

### Documents inspected

At the root commit:

- `dev-docs/GwzSspiDesign.md`, all seven sections, including numbered lines 1–51, 115–143 and 145–237.
- `dev-docs/GwzSspiPlan.md`, all five steps and its acceptance/implementation boundary.
- `dev-docs/GwzSspiCallerGuide-DRAFT.md`, all lines 1–74.
- `dev-docs/GwzCoreSessionCrateMap.md`, its pending SSPI exception and §1 rules; additional package/composition context.
- `dev-docs/GwzLocalCloneLibraryBoundaries.md`, its adoption, dependency/API boundaries, interface checkpoint and testing/gate obligations.
- `dev-docs/AgentProcessRules.md`, particularly L1-01/02, L1-16–20 and L1-28/29.
- `dev-docs/GwzProcessOptimization.md`, particularly preserved rules, physical feasibility policy and §8 review granularity.

At the core commit:

- `dev-docs/GwzTransportWindowsParityDesign.md`, including the explicit SSPI-only supersession amendment and §§2, 7–11.
- `dev-docs/GwzTransportCredentialHelpersDesign.md`, authentication selection, admission/cleanup, secret ownership, route scope, pooling, rejection and retry rules.
- `dev-docs/GwzTransportCredentialHelperTimingAmendment.md`, bounded helper provenance, carrier validation and precise supersessions.
- `dev-docs/GwzTransportSshHelperClockAmendment.md`, connection-scoped timing authority, local-phase ownership, terminal arbitration and supersession boundary.
- `dev-docs/GwzTransportCredentialHelperConfigurationViewAmendment.md`, preserved policy, configuration isolation, ownership/deadlines, Windows exclusion and implementation-contact limitations.
- `dev-docs/GWZDesign.md` and `dev-docs/GWZRequirements.md`, relevant pending SSPI banners, accepted helper amendments, session controls, secret/protocol boundaries and concurrency requirements.
- The three native reports linked by SSPI design §7: `GwzTransportWindowsAuthAlternativeFeasibility.md`, `GwzTransportWindowsSspiWorkerFeasibility.md` and `GwzSspiCreationContainmentFeasibility.md`.

At the evidence commit:

- README and manifest for `2026-10-03-tr1-8-sspi-process-worker`.
- That run’s `raw/probe-v2.stdout.txt`: retained native context, normal cleanup, EOF, oversized-frame refusal, live-wait admission refusal, controlled stall termination and parent-loss results.
- README and manifest for `2026-10-03-tr1-8-sspi-job-list`.
- That run’s `raw/probe-v1.stdout.txt`: creation-time membership, suspended parent loss, resumed descendant containment and invalid-Job refusal.

The process-worker successful stdout hash recorded by its manifest is `135c0ad5ac2d6bfc074d48d541c19eaf35e5cb722148df8ecdb4737b387d663e`. The creation-time stdout hash is `13cc78dfb5d3b8dbb0db7d9e123da9714d39a281d9dc9eb76769a917a320c8d6`. These are recorded artifact identities, not newly recomputed hashes.

Also read the specified review-loop skill and Microsoft’s primary documentation for [Digest InitializeSecurityContext](https://learn.microsoft.com/en-us/windows/win32/secauthn/initializesecuritycontext--digest) and [Digest input buffers](https://learn.microsoft.com/en-us/windows/win32/secauthn/input-buffers-for-the-digest-challenge-response).

### Commands and limitations

Used only committed-document inspection through `git show`, `sed`, `rg` and `nl`, reads of the prompt/instructions, primary-source browsing, and these tuple checks:

- `git rev-parse HEAD`
- `git -C gwz-core rev-parse HEAD`
- `git -C gwz-core-evidence rev-parse HEAD`

All three produced the baseline SHAs at review start and end. No files were written, no repository mutations occurred, and no builds, tests or native experiments were run. Native results below are inspected historical evidence, not executions by this reviewer.

## 1. Findings

### [P2-3] Closed Begin payload cannot carry the required initial Digest challenge

**Location:** `GwzSspiDesign.md` §4, lines 131–136; `GwzSspiCallerGuide-DRAFT.md`, lines 23–24 and 34–36; inherited Windows parity §8, lines 316–323.

**Root cause and classification:** Protocol/API boundary defect: the closed first-message contract omits a required authentication input. This is a bounded schema correction, not a defect in the process-containment architecture.

**Violated invariant:** Every required authentication input owned by `AuthRequest` must have a specified, bounded path into the worker before the native call that consumes it.

The caller guide requires Digest’s actual method, percent-encoded URI and initial challenge. Windows parity §8 preserves the entire validated challenge as native input. Microsoft likewise specifies the server’s initial HTTP challenge in Digest’s token input buffer. [Digest input-buffer contract](https://learn.microsoft.com/en-us/windows/win32/secauthn/input-buffers-for-the-digest-challenge-response).

However, the design declares Protocol v1 closed and lists Begin as package, target, identity, CBT and optional Digest method/URI. It supplies no initial-challenge field. `start` stops after Hello; the first public step must be `None`, sends Begin and performs the first Initialize. Challenge messages drive subsequent Initialize calls.

**Reproduction/state sequence:**

1. A mechanism qualification fixture constructs a Digest request with explicit identity, validated initial challenge, actual method and URI.
2. `start` verifies Hello.
3. The caller invokes the required `step(None)`.
4. Begin can carry method/URI but cannot carry the initial challenge under the enumerated closed payload.
5. The worker must omit the required input, invent an additional field/message, or change the first-step ordering. None satisfies the stated contract.

**Impact:** A conforming implementation cannot realize the documented Digest request shape. Implementation contact would require an unreviewed secret-bearing schema or state-machine change.

Digest provider qualification remains deferred. This finding does not demand a successful provider outcome or product activation; it concerns the already-specified interface and private protocol.

**Required correction:** Explicitly carry the initial Digest challenge in Begin as bounded owned secret bytes, required for Digest and prohibited where inappropriate. Specify its validation, zeroizing codec ownership and use by the first Initialize. Alternatively, revise the first-step protocol coherently, preserving verified Hello before secrets and avoiding an undocumented extra round.

**Closure/regression test:** A fake-worker schema/state walkthrough must round-trip a Digest request’s initial challenge through the documented first step into the first native-call input. Cover missing, oversized and wrong-package fields, cancellation before Begin, and zeroization on refusal. Ordinary Negotiate/NTLM first-step behavior must remain explicit. No successful native Digest authentication is necessary to close this shape defect.

### [P3-1] Worker cooperative budget has no defined protocol carrier

**Location:** `GwzSspiDesign.md` §4, lines 131–143, and §6, lines 202–204.

**Root cause and classification:** Documentation-level protocol completeness defect: the design promises worker-visible timing data without assigning it to the closed protocol.

**Violated invariant:** An independently executing worker cannot perform documented budget checks without a defined clock/budget input.

**Reproduction/state sequence:** The parent translates the operation deadline and starts a worker using only the listed nonsecret bootstrap handles. Hello, Begin and Challenge contain no remaining-budget value, yet §6 requires the worker to receive remaining budget for cooperative checks. An implementer must invent its carrier and interpretation.

**Impact:** Implementations may disagree about whether the worker receives a fresh allowance, a stale duration or a parent-clock timestamp. The parent remains the deadline authority, so this omission does not establish late-token publication or loss of containment; its consequence is an incomplete implementation contract.

**Required correction:** Identify the budget carrier, units, sampling point and worker interpretation, including how a shortened parent deadline affects it. Alternatively, remove the worker-budget promise and explicitly rely on parent enforcement. Neither choice may reset or extend the parent deadline.

**Closure/regression test:** A fake-clock walkthrough delays Begin and shortens the parent deadline. Worker cooperative checks, if retained, must use the specified budget semantics, while parent expiry still forbids late publication.

## 2. Invariant analysis

### Disposition of the initial limited-scope observations

| Initial ID | Disposition under the full Safety scope |
|---|---|
| P2-1, unavailable actual Negotiate mechanism | **Withdrawn as a blocking finding.** The counterexample introduced a Kerberos-only caller policy absent from the controlling contract. Windows parity deliberately selects native Negotiate/NTLM behavior without requiring mutual authentication or prohibiting NTLM. The full design retains that policy and rejects substitution of an independent implementation. The caller-only absence therefore did not establish a violation of this object’s accepted policy. |
| P2-2, unbounded queued request ownership | **Withdrawn as a demonstrated P2.** The full contract bounds native records and charged threads, defines Waiting as having no native record, removes dropped waiters, prohibits growing IPC queues and forbids uncharged native work or replacement workers against quarantined slots. The initial sequence did not distinguish caller-created outstanding futures from supervisor-retained native cleanup ownership. It did not prove that the complete contract permits the claimed unbounded retained-native state. |

These withdrawals reflect inspection of the controlling graph, not remediation or peer influence. Implementation review should still verify waiter bookkeeping and the practical memory behavior of admission.

### Containment and crash attacks

The Create→Assign parent-loss window is expressly closed. The child is created suspended with creation-time JOB_LIST and an explicit HANDLE_LIST; failure cannot select subsequent assignment or an in-process fallback. Records and permits precede OS effects. A late CreateProcess return after cancellation produces a contained child that is killed without resume or secrets.

Native evidence supports this primitive on the tested Windows 11/OpenSSH Job hierarchy: suspended creation membership, parent-loss child exit, resumed descendant membership/exit and invalid-Job creation refusal. It does not prove every supported host hierarchy, packaged worker or blocked provider. The design accurately retains those qualification limits.

### Cancellation, late completion and cleanup attacks

The publication state lock separates logical failure from physical cleanup. Cancellation revokes publication before termination; late frames cannot revive a conversation. No blocking/native work runs under that state lock.

A kill request, EOF, PID disappearance or failed wait cannot release a permit. Reaped requires held-handle exit, empty Job and completed local IPC/launch threads. Stuck creation, termination or I/O remains quarantined. This admits bounded stuck capacity rather than unbounded replacement work.

Dropped futures and supervisor teardown retain owned supervision. An expired shutdown reports outstanding IDs and does not manufacture cleanup confirmation. Tombstone eviction returns Unknown. These rules defeated premature-release, detached-buffer and false-cleanup counterexamples.

### Identity, CBT and route attacks

Hello compares actual primary-token SID, authentication LUID and session ID before credentials or challenges are sent. Impersonation and changed primary identity are refused. The object does not claim interactive desktop identity from the session-0 probe.

Binding originates from the actual verified final-origin TLS connection. Missing binding cannot select an empty fallback. Connection loss, redirect, peer/binding change and route retirement end the conversation. The inherited route scope includes operation, route/service and pinned U; matching logon identity alone cannot authorize reuse.

Native token completion remains separate from remote authentication success. Authentication rejection is terminal; eligible pre-effect retries create fresh conversations on fresh leases. Partial or complete POST is not replayed to obtain credentials.

### IPC and disclosure attacks

Frames are bounded before allocation, with exact message exhaustion, closed types/states and bounded token/text/binding inputs. Each pipe has one writer and one pending request/response per conversation. Build/schema mismatch refuses before Begin.

The design makes zeroizing generated encode/decode ownership a stop condition if the codec cannot provide it. Native buffers and initialized handles have explicit normal cleanup; forced exit makes no physical-erasure or LSASS/Pageant cancellation claim. Credentials are excluded from argv, environment, disk and diagnostic payloads.

The concrete protocol omissions in §1 remain; the surrounding secrecy and ownership requirements did not otherwise permit a reproducible disclosure path in this document review.

### Clocks, helper provenance and scope

SSPI consumes the existing absolute operation/setup deadline, with no per-round allowance or helper-local 120-second budget. It does not borrow SSH’s local helper phases or pause network clocks.

Helper selection, configuration preparation and execution stay in core. SSPI timeouts cannot gain helper-budget provenance. The accepted SSH clock and configuration-view amendments remain confined to their stated domains; no Windows configuration-view implementation is inferred from the Unix mechanism.

The Windows amendment enumerates superseded clauses and leaves authentication policy, route/retry rules and helper spawning intact. Mechanism GO cannot remove product guards, freeze the full Windows draft or qualify provider parity.

### Host and dependency lifecycle

CLI dispatch precedes normal parsing, logging and runtime startup. Python uses its bundled absolute executable without a child interpreter. Missing/mismatched workers fail explicitly; application removal/upgrade owns worker removal/replacement.

The library has no dependency back to core, Git, drivers or transport implementation. The placement/secret exception is explicitly scoped, while public GWZ encoding stays in core. No unauthorized repository creation, release or activation follows from this draft.

## 3. Risks and next action

Real blocked providers, interactive SSO, Digest/EPA interoperability, TLS binding lifetime, trust, proxy, Pageant and complete Windows qualification remain separate gates. The inspected native timings are individual observations, not worst-case termination guarantees. Same-user or privileged memory inspection and external provider activity are not eliminated by private pipes or Jobs.

The next action is one bounded document correction: complete the closed first-message contract for Digest’s initial challenge and resolve the worker-budget carrier, then obtain a focused re-verdict on P2-3 before standalone implementation begins.
