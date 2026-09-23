# GwzTransportParallelInterfaces — CONSISTENCY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzRemoteTransportSetupFailureAmendment.md` at `f3640463bd1c69d29322de9d92902eff54af7b8e` and `gwz-py/dev-docs/GwzPyTransportDesign.md` at `4a884174a0a1a3a9f8d9aa756017d1b8c6034813`; draft interface package, 2026-09-23  
**Baseline:** root `4ad1aa3d00c7f3ac5b725cc271dc94d28b96a326`; core `f3640463bd1c69d29322de9d92902eff54af7b8e`; Python `4a884174a0a1a3a9f8d9aa756017d1b8c6034813`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`. All sources were read as committed blobs with `git show`.  
**Date:** 2026-09-23  
**Axis:** Document consistency against controlling authority, current APIs, compatibility, and satisfiable verification. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves **P2-1** and **P2-2** as specified.

---

## 0. Evidence base

The four HEADs matched at both review boundaries. Reviewed both draft documents, V1.1.0 S1/S6/Phase 8, RetryPlan §§4–6, AlphaTimeoutPlan §5, RemoteTransportDesign §§4/10, the pinned transport taut/generated decoder and pool error path, and the pinned Python bridge/client/native shim plus core `TransportRuntime` API. No builds, tests, writes, dirty source, or peer reports were used.

## 1. Findings

### [P2-1] Failure-cause enum has no frozen wire discriminants

**Location:** SetupFailureAmendment lines 21–25 and 82–100.  
**Root cause:** The draft fixes `Failure.setup_cause` at key 4 but names seven enum variants without assigning their integer values. Every existing transport taut enum assigns explicit integers, and the amendment retains profile v2 while claiming old/new compatibility.

**Counterexample:** One implementation assigns `stall=1, aggregate=2`; another generated schema orders them differently. Both satisfy the prose, but a v2 failure from one is decoded as the wrong cause by the other, changing retry eligibility without a decode error.

**Impact:** The proposed interface is not a reproducible schema freeze; implementation must invent compatibility-significant wire values.

**Required correction:** Assign an explicit, unique integer to every `SetupFailureCause` variant and declare those mappings frozen.

**Closure test:** Golden CBOR for all seven variants; new-reader/old-writer and retained-old-reader/new-writer checks; unknown discriminant remains a decode error.

### [P2-2] Close is unordered against first lazy host construction

**Location:** Python design §§2–3, especially lines 46–71, 80–90, 102–123, and verification lines 192–207.  
**Root cause:** The design promises that awaited `close()` cancels, finishes, and shuts down the host, but defines no Rust-owned terminal state or ordering for close racing the first network operation’s lazy construction.

**Counterexample:** Task A begins first-host construction. Before installation, task B executes `close()`, observes no active request or installed runtime, and returns. A then installs the runtime and starts network work after close completed.

**Impact:** Work may outlive context exit, the retained cleanup report can omit that host, and the “one lifecycle owner” guarantee is false.

**Required correction:** Freeze a Rust session lifecycle in which close atomically prevents new construction/admission, joins any in-progress construction, disposes a constructor result that loses the race, and permanently refuses later network calls.

**Closure test:** Barrier-driven close-during-construction and close-during-admission tests proving no post-close request, one shutdown, retained cleanup facts, and typed refusal afterward.

## 2. Invariant analysis

The typed field otherwise preserves `ErrorCode`, `Effect`, and fingerprint meaning; older decoding ignores bounded unknown key 4, missing key 4 becomes `None`, and ambiguous `Timeout`/`Unavailable` fails closed. The classifier matches the accepted retry set and remains in core. The Python package amendment removes duplicate lifecycle ownership, preserves the acyclic `gwz-py → gwz-core → gwz-transport` release order, keeps local-only operations outside endpoint construction, and retains Rust-owned clocks, credentials, pooling, placement, and cancellation.

## 3. Risks and next action

Correct the two interface gaps in the drafts, add their named tests to the verification gates, settle a new four-repository tuple, and run a focused re-verdict. No product implementation is accepted by this review.
