# SSH agent A1 implementation checkpoint — CODE-AXIS REVIEW

**Review object:** `gwz-core` `309696601227494f60942aea96124ca4eaeebda5`, controlling checkpoint `dev-docs/GwzRemoteTransportSshAgentA1.md`, status “implemented; aggregate review pending,” dated 2026-09-21.  
**Baseline:** root `6da3ce07e0ccbca36eec4bbf23b9315add645f98`; gwz-core `309696601227494f60942aea96124ca4eaeebda5`; gwz-core-evidence `0792238da91b2466ca8ce9dc752e9dc901c0bad4`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. All source and evidence were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Code architecture, ownership, interfaces, call graphs, error paths, and compatibility with the accepted helper design. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 interface defect blocks; one P3 coverage defect remains. I pre-commit to GO on a revision that resolves P2-1 as specified and preserves the otherwise verified invariants.

---

## 0. Evidence base

I read the complete prompt, A1 checkpoint, accepted helper design §§3–8, current checkpoint, `EVIDENCE.md`, the complete changed source and tests, isolated manifest/lock changes, retained worker/pool contracts, and private `agent-a1` evidence plus hashes. The three source hashes match `inputs.json`; production source is 550 lines and focused tests are 560 lines.

The authorized Rust 1.95 offline locked command completed successfully: 52 passed executions, one intentionally ignored child entry invoked by its parent, 64 printed fragmentation seeds, and no failures.

The tuple and worktree state were verified before and after review. Root retained only the two authorized untracked prompts. Root/core trees remained `343adb8ba64bc51b96db684ce1e4a474b86ed3c7` and `1c7cc81679c8b01ab8d68ff379c0899287039561`.

## 1. Findings

### [P2-1] Owned-channel extraction resets the protocol’s irreversible state

**Location:** `src/git/endpoint/agent_client.rs:12-29`, especially `Agent::into_channel`; affected invariants at `:30-61` and `:101-163`. The test at `tests/transport_ssh/tests/agent_client.rs:417-433` demonstrates extraction after an `InvalidInput` failure.

The accepted design requires one enumeration per helper, irreversible failure, and no reconnect/replay after a malformed, truncated, or partially written operation. Those properties are stored only in `Agent.failed` and `Agent.enumerated`. `into_channel(self) -> C` returns the live underlying channel and discards both guards.

A caller can enumerate successfully, extract the channel, construct a fresh `Agent`, and send a second enumeration. More seriously, after a partial write or malformed response marks the first wrapper failed, the caller can extract that potentially desynchronized channel, rewrap it, and issue another request. The method is `pub(crate)`, so A2 activation would expose this bypass across core rather than confining it to test inspection.

Remove the production channel-extraction seam and keep channel ownership sealed until `Agent` is dropped. Tests can inspect writes through an externally shared fake-channel record. If an extraction operation remains necessary, it must consume only a demonstrably pristine, never-used channel; failed or enumerated agents must close rather than return it.

**Closure regression:** prove that a successful enumeration cannot produce a second identity request on the same owned channel, and that malformed/partial/unsupported operations drop or close the channel with no rewrap path. The interface itself should prevent extraction after use or failure.

### [P3-1] Cancellation tests do not cover every required publication/claim/join boundary

**Location:** accepted design `GwzRemoteTransportSshAgentDesign.md:240-246`; A1 tests `agent_client.rs:153-233` and `agent_capacity.rs:19-106`.

The tests cover cancellation before late result publication, cleanup overrun, repeated cancellation, abandonment, eventual reap, and ordinary successful claim. They do not deterministically hold and cancel a job after result publication but before thread exit/join, or after join but before result claim. Those are explicitly required TDD boundaries and are distinct from cancelling a closure while it is blocked.

A future ordering regression in those narrow windows could pass the current suite. Add deterministic test-only barriers around publication and post-join/pre-claim, then verify cancellation discards the result once, releases the permit only after destruction, and cannot revive success. Also retain a claimed-success case proving later `Job` drop does not destroy the transferred owner.

## 2. Invariant analysis

The remaining attacks held:

- Frame, identity, blob, signing-input and signature bounds precede untrusted-body allocation or copying; malformed/trailing fields and algorithm mismatches fail with redacted errors.
- Partial reads/writes retain offsets and never replay accepted bytes. RSA SHA-2 flags and raw-signature extraction are correct for the admitted methods.
- Unix sockets are nonblocking before connect; connect completion checks socket error, and every wait is bounded by cancellation/deadline quantum.
- Completion and cancellation arbitrate under one state lock. Threads are joined only after `is_finished`; failed, panicked, cancelled, or abandoned results are destroyed outside arbitration locks.
- The global 64-helper permit survives caller loss and cleanup overrun and is released only after join and result disposal/transfer.
- Spawn failure releases its permit without running setup effects. Exact expired start refuses the closure.
- Evidence accurately limits native pending-connect, send-buffer, signing, platform, source-distribution, and A3 integration claims. No public API, wire, authentication, or production activation is introduced.

## 3. Risks and next action

A2 still must prove concrete native-result destruction, callback/allocator ownership, and signing retry behavior. A3 still owns pool capacity, endpoint refusal, and observable shutdown integration. Platform and selected-source qualification remain valid deferred gates.

Apply one bounded A1 correction that seals the channel state and adds the missing deterministic lifecycle-boundary tests, then rerun the same focused gate and return this axis for focused re-verdict.
