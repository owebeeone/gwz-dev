# Interruptible SSH Agent Helper Design — Safety-Axis Review

**Review object:** Document-only design in `gwz-core/dev-docs/GwzRemoteTransportSshAgentDesign.md` at core `a91846eb0328106cb76cc0aa90846590aafd72df`; status DRAFT for dual design review, dated 2026-09-21. No implementation or activation is included.  
**Baseline:** root `efb0d2a698755f3c1804f67495c4c9ded48e547d`; gwz-core `a91846eb0328106cb76cc0aa90846590aafd72df`; gwz-core-evidence `5ea96433628b3bec8a8365525f8ee314d01cce99`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed documents and source were read through immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Credential, ownership, cancellation, deadline, cleanup, disclosure, and degraded-path safety. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

I read the complete canonical prompt and the full 269-line design. Supporting inspection covered:

- `GwzRemoteTransportDesign.md` §§7, 8, and 10 and the route, credential, deadline, and shutdown contracts.
- `GwzRemoteTransportRequirements.md` G1/G2 and P6–P9.
- The accepted shared-worker design and current `ssh_pool.rs`/`ssh_worker.rs` ownership, deadline, cancellation, and disposal contracts.
- Pinned libssh2 1.11.1 `userauth.c` callback/retry paths and `agent.c` signing behavior, plus the ssh2 0.9.6 agent wrapper and existing blocking-agent characterization.

The native source confirms the relevant design assumptions: the public-key callback may recur after `EAGAIN`; libssh2 frees callback-produced signature memory with the session allocator; and RSA SHA-2 agent flags depend on libssh2’s negotiated user-auth method. The design treats all three as mandatory binding proofs rather than presumed behavior.

No build or test was run because this is a documentation-only gate and the prompt authorizes none. The exact tuple matched at both review boundaries. Root status contained only the two explicitly excluded generated review prompts.

## 2. Invariant analysis

**Trust and credential locality hold.** The first slice accepts only an already connected, handshaken, fixture-trusted owned session. Production ordering requires host-trust validation before any signing or authentication. Agent addresses, key lists, signing inputs, and signatures stay at the executing endpoint. Explicit file identity remains fail-closed and cannot fall back to an ambient agent. Exact-agent and encrypted-key support remain unavailable until separately qualified.

**Helper ownership is singular and closed.** One helper owns the native session, TCP owner, agent handle, codec, and callback state for one setup attempt. Sessions are neither cloned nor concurrently accessed. Success can transfer only after helper exit and join; panic or failed join invalidates the result. Cancellation or owner loss destroys an unclaimed success.

**Publication and cancellation races are resolved safely.** Completion claim and cancellation share one lock, cancellation is monotonic, and notifications occur after lock release. Generation-tagged IDs prevent stale completion or cancellation from reaching replacements. The worker advances time before considering completion, so exact expiry defeats simultaneous success.

**Cancellation does not depend on optimistic thread behavior.** Every agent operation uses nonblocking handles, bounded readiness waits of at most 20 ms, absolute monotonic deadlines, and cancellation checks before and after progress and publication. Partial connect/write/header/body states remain interruptible. A dropped receiver or JoinHandle is explicitly insufficient. Unsupported transports refuse rather than falling back to opaque blocking calls.

**Exceptional cleanup remains owned.** Live, completed-unjoined, and quarantined helpers retain a global permit. Cleanup overrun reports failure, refuses further endpoint opens, and transfers ownership to the bounded process supervisor; it does not acknowledge disposal or release pool capacity. Eventual result destruction and join precede permit release. Recreated endpoints cannot bypass the global cap, and no thread is killed or detached.

**Resource growth is finite.** The global helper cap is 64, with one retained job/result/control record per permit. Protocol frames, identity count, key blobs, signing data, signatures, readiness turns, and retries all have explicit limits. Agent comments are discarded. Malformed lengths, truncation, unexpected messages, unsupported algorithms, and stalled peers fail the attempt without recursive retry or outstanding-sign replay.

**The signing bridge is gated at the dangerous boundary.** The callback must preserve state across native retries, prevent Rust unwinding through FFI, return only validated raw signature bytes, match the negotiated algorithm and RSA flags, and transfer session-allocator-owned memory exactly once. Failure to establish allocator/CRT pairing blocks the adapter, particularly on Windows. Ed25519 and modern RSA fixtures, callback retry tests, panic tests, and shutdown-during-sign tests are mandatory before acceptance.

**Scope claims remain accurate.** The design adds no public API, wire message, forwarding service, identity namespace, or production capability. DNS, trust-file access, key-file I/O, prompts, platform cancellation primitives, allocator ABI, agent discovery, selected-source distribution, and production routing remain explicit later gates. Their deferred outcomes are not presented as qualified.

## 3. Risks and next action

The helper architecture still depends on platform-specific proof that every admitted agent transport can create and cancel owned I/O within the stated bounds. The private libssh2 bridge also needs proof of negotiated-method recovery, callback retry stability, and allocator pairing. The design correctly makes failures in those proofs capability blockers.

Proceed to A1 only: implement the isolated bounded codec/client and complete helper lifecycle with fake-agent tests, including cap exhaustion, every publication/cancellation boundary, cleanup quarantine, and eventual reap. A2 signing and A3 production ownership remain separate reviewed gates; platform and selected-source qualification remain deferred rather than waived.
