# Interruptible SSH agent helper — CONSISTENCY-AXIS REVIEW

**Review object:** `gwz-core` `a91846eb0328106cb76cc0aa90846590aafd72df`, `dev-docs/GwzRemoteTransportSshAgentDesign.md`, 269-line DRAFT dated 2026-09-21; document-only design, with no implementation or activation claimed.  
**Baseline:** root `efb0d2a698755f3c1804f67495c4c9ded48e547d`; gwz-core `a91846eb0328106cb76cc0aa90846590aafd72df`; gwz-core-evidence `5ea96433628b3bec8a8365525f8ee314d01cce99`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable objects with `git show`; pinned crate-native source was matched through the committed `libssh2-sys 0.3.3` lock entry.  
**Date:** 2026-09-21  
**Axis:** Consistency against the controlling transport design, requirements, accepted worker contract, private endpoint/pool implementation, and pinned native signing behavior. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1, or P2 findings; one P3 documentation finding does not block this draft-stage design gate.

---

## 0. Evidence base

I read the complete review prompt and the complete design, plus:

- `GwzRemoteTransportDesign.md` §§7, 8, 10 and 10.1–10.2.
- `GwzRemoteTransportRequirements.md` G1–G7, C1–C8, and P1–P9.
- Accepted `GwzRemoteTransportSshWorker.md`, especially lines 24–36, 55–70, 90–97, and 152–170.
- Pinned `ssh_pool.rs`, `ssh_worker.rs`, `ssh_connection.rs`, `ssh_endpoint.rs`, and `ssh_remote.rs`.
- Pinned worker characterization records in evidence `5ea9643`.
- Locked `libssh2-sys 0.3.3` `agent.c` and `userauth.c`, including agent framing/signature extraction, RSA SHA-2 flags, callback retry state, allocator use, and native `EAGAIN` paths.

No build or test was run, as required for this documentation review. The exact tuple was verified at both start and end. Root retained only the two authorized untracked review prompts; every member SHA remained unchanged. Root and core trees were respectively `bc616a58540abf88392a83e40b8e18d7866d9fe6` and `ca193681d1d5b65f519994034c4e53a3275d2e5a`.

## 1. Findings

### [P3-1] The claimed superseded worker clause does not exist

**Location:** `GwzRemoteTransportSshAgentDesign.md:14-20`, against `GwzRemoteTransportSshWorker.md:30-36,63-70`.

The new design says it “supersedes only” a worker-document candidate in which the shared worker services agent I/O. The accepted worker document contains no such candidate. It explicitly forbids moving the opaque blocking agent call into the shared worker, then proposes a bounded endpoint-local agent client plus signing callback.

This violates the review requirement that superseded-clause lists identify the controlling change exactly. The behavioral design remains coherent because both documents require the shared worker to remain nonblocking, but the false supersession edge can mislead later amendment and recovery work about what the accepted baseline authorized.

Replace the sentence with an exact refinement statement: the new design selects the helper-thread realization of the worker document’s already-required bounded setup seam and does not supersede that prohibition. Separately retain the expressly identified private contract refinements at lines 201–209.

**Closure check:** compare the corrected authority paragraph against the cited worker lines and verify that every claimed superseded clause exists verbatim, or that the text says it is a refinement rather than a supersession.

## 2. Invariant analysis

The substantive attacks held:

- Setup ownership remains exclusive. The helper owns the session, TCP owner, agent state, and callback state during authentication; publication never clones native ownership, and success is not usable until termination and join.
- Cancellation is stronger than a caller-side timeout. Agent operations use owned nonblocking handles, absolute deadlines, bounded readiness waits, and a wake path; late or unclaimed success is destroyed.
- Pool accounting remains conservative. Capacity is reserved before spawn and retained through disposal; completed-unjoined and quarantined jobs retain a process-wide permit, and endpoint recreation cannot bypass the global helper cap.
- Exact deadline precedence agrees with the accepted pool: time advances before completion, expiry defeats simultaneous success, and zero disables only the normal network deadline.
- The design explicitly identifies the required private `Resource::Drop` and worker-shutdown refinements rather than pretending the current synchronous-drop and fire-and-signal behavior already satisfies supervised cleanup.
- Trust precedes signing and authentication. Explicit file identity remains fail-closed with no ambient fallback; exact-agent/encrypted-key behavior remains unavailable pending separate qualification.
- Agent framing, allocation limits, one-request sequencing, RSA SHA-2 flags, returned-signature shape, callback retry stability, FFI unwind containment, and session-paired allocation all correspond to concrete pinned-native behavior and have explicit implementation gates.
- Platform cancellation, allocator ABI, source distribution, discovery, and production activation remain accurately deferred. The document makes no implementation-test or physical-capability claim.

## 3. Risks and next action

Allocator pairing, Windows cancellation, selected-source distribution, and real platform behavior remain unresolved activation gates by design; they are not defects in this reviewed draft.

Correct P3-1’s authority wording, then—subject to the independent peer verdict—accept the design as authority for A1. This GO does not accept an implementation, freeze physical capability, or authorize production activation.
