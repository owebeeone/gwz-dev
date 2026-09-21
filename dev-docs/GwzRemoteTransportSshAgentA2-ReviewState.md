# SSH Agent A2 — State-Axis Review

**Review object:** A2 native signing checkpoint at gwz-core `131989bc9ec6fe6e8c0803e60a9e72654dcc1a25`, controlled by `dev-docs/GwzRemoteTransportSshAgentA2.md`; implemented, aggregate review pending, 2026-09-21.  
**Baseline:** root `1b3726bd2fa9976e7061aff2511d750d5475455a`; gwz-core `131989bc9ec6fe6e8c0803e60a9e72654dcc1a25`; gwz-core-evidence `109c2a6e97504bd075db5ac65676daa4688abe57`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show`; cached native sources were matched to the evidence hashes.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics, ownership, cancellation, recovery, and fail-closed behavior. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

I reviewed:

- `GwzRemoteTransportSshAgentA2.md` §§Boundary and ownership/Evidence and limits, the accepted helper design §§4–8, accepted A1 lifecycle contract, `CurrentProgramCheckpoint.md`, and `EVIDENCE.md`.
- `agent_auth.rs:1–246`, including trust admission, per-key state, callback ABI, signed-payload parsing, signature validation, native allocation transfer, retry handling, and authenticated handoff.
- Direct interactions with `agent_client.rs`, `agent_job.rs`, `agent_socket.rs`, `ssh_connection.rs`, and `ssh_channel.rs`.
- All seven A2 tests and their native fixture support.
- Evidence run `2026-09-21-agent-a2`: README, inputs, preserved red/green history, final source hashes, and full-gate record.
- Hash-matched cached sources: ssh2 0.9.6 `session.rs`; libssh2-sys 0.3.3 `libssh2.h`, `session.c`, and `userauth.c`. These establish the exact callback signature, default `malloc/free` pairing, callback timing, signature copying/freeing, and EAGAIN state retention.
- The permitted Rust 1.95 offline locked aggregate command, using the external target directory. It passed 61 executions: seven A2 and 54 retained tests; the documented child-only case remained deliberately ignored.

The complete tuple was verified unchanged before and after review. Root contained only the two declared untracked owner prompt files; member trees were clean. The three new-source SHA-256 values matched the evidence manifest.

## 2. Invariant analysis

**Trust precedes credential effects.** `authenticate` checks cancellation, validates the username, and compares the session’s actual host-key bytes with the independently supplied trusted bytes before setting up or opening the agent. The wrong-host test records no identity-list or sign request and observes connection shutdown.

**Per-key state is bounded and non-replayable.** Identities are enumerated once, bounded by the accepted A1 codec, then attempted in order with a fresh stack-owned `Signer`. A server rejection advances once; callback, agent, cancellation, parsing, or native errors terminate. `invoked` prevents a repeated native callback from replaying an agent sign request. Successful Ed25519 and RSA SHA-256/512 cases traverse an initially rejected identity and record one sign operation.

**Native retry preserves ownership.** `Session::raw` holds the exclusive native-session guard only around each libssh2 call. The `Signer`, key, username, agent, and control remain live at stable locations across EAGAIN returns. Pinned `userauth.c` retains packet state after network EAGAIN and does not retain the callback or its input buffer after callback return. The paused-sshd test exercises retry followed by both successful resumption and cancellation.

**Allocator transfer is paired and exactly once.** ssh2’s `Session::new` supplies null allocator hooks; pinned `session.c` therefore installs its default `malloc/free`. The callback uses Unix `libc::malloc` only after algorithm, response shape, and cancellation checks pass, copies from the Rust allocation, and transfers that pointer once. Pinned `userauth.c` frees it after copying and also on the later packet-reallocation failure path. Validation errors and caught panics transfer nothing. Windows remains enclosed out.

**Cancellation and destruction fail closed.** Agent list/sign operations and native retry polling share one `Control` and absolute deadline. Checks surround agent progress, callback publication, native calls, and final handoff. Cancellation or expiry during a withheld signature closes the agent socket, drops the owned connection, shuts down TCP before native-session destruction, and becomes observable only after helper join and disposal. A stalled sign does not prevent an independent SSH channel from completing.

**Successful ownership transfer is joined.** `Job::poll_result` cannot release the connection until the supervisor has joined the helper. After transfer, dropping the consumed job cannot cancel or destroy the connection; the successful test performs a Git upload-pack exchange through the transferred owner. Failure and unclaimed success remain owned until disposal.

**Unwind and malformed-state containment hold.** The Rust callback body is enclosed by `catch_unwind`; the injected channel panic becomes an error, with TCP and agent cleanup observed. Wrong algorithm and structurally short signatures are rejected locally before native allocation or a second sign request. No SHA-1 fallback code is returned to libssh2.

## 3. Risks and next action

This checkpoint does not provide allocator-OOM injection, heap census, Windows/Linux primitive qualification, selected-source reconstruction, production discovery/handshake/trust-file setup, or A3 pool accounting and observations. Those limits are stated accurately and do not weaken this Unix fixture result.

Accept A2. The next gate is A3 integration of this joined authenticated owner into the physical pool/backend, including observable setup failure, cleanup refusal, connection charging, and authentication observations. Production activation and the deferred platform/source batch remain separate gates.
