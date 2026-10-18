# SSH agent A2 native signing checkpoint — CODE-AXIS REVIEW

**Review object:** `gwz-core` `131989bc9ec6fe6e8c0803e60a9e72654dcc1a25`, controlling checkpoint `dev-docs/GwzRemoteTransportSshAgentA2.md`, status “implemented; aggregate Code/State review pending,” dated 2026-09-21.  
**Baseline:** root `1b3726bd2fa9976e7061aff2511d750d5475455a`; gwz-core-evidence `109c2a6e97504bd075db5ac65676daa4688abe57`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Product and evidence files were read from immutable Git objects; pinned native sources were read from the local Cargo registry and matched the recorded hashes.  
**Date:** 2026-09-21  
**Axis:** Code architecture, FFI contracts, ownership, error paths, and compatibility with the accepted helper design. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 native-error classification defect blocks. I pre-commit to GO on a revision that resolves P2-1 as specified and preserves the otherwise verified A2 invariants.

---

## 0. Evidence base

I read the complete prompt; A2 checkpoint; accepted helper design §§3–8; retained A1 contract; current checkpoint; `EVIDENCE.md`; all 246 lines of `agent_auth.rs`; all 552 lines of A2 tests/support; fixture manifest/lock changes; and the directly interacting `agent_client`, `agent_job`, `agent_socket`, and `SshConnection` ownership paths.

I inspected ssh2 0.9.6 `Session::new`, `raw`, authentication, host-key and destruction paths, plus libssh2-sys 0.3.3 `libssh2.h`, `session.c`, and `userauth.c`. Their hashes match the evidence manifest. The final A2 source/test hashes also match `inputs.json`.

I did not rerun the optional suite. The archived exact-source logs record seven A2 tests and a 61-execution full pass, including 54 retained executions and 64 fragmentation seeds.

The tuple was verified unchanged at start and end. Root/core trees remained `2d926d737fa620a951b727dd1898cec0130b3a5e` and `8c54c61b34aa5be2f3e8268a1a7b5717d1ab4ce7`. All member SHAs remained pinned and clean; root contained only the two permitted untracked A2 prompts.

## 1. Findings

### [P2-1] `PUBLICKEY_UNVERIFIED` conflates server rejection with native transport/protocol failure

**Location:** `src/git/endpoint/agent_auth.rs:93-116`, especially the unconditional next-key branch at `:107-110`; pinned libssh2 `src/userauth.c:1679-1686` and `:1876-1879`; controlling checkpoint `GwzRemoteTransportSshAgentA2.md:14-15`.

The checkpoint requires only server refusal to advance to the next identity; native failures must terminate the connection. The Rust loop instead treats every `LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED` as a key rejection.

Pinned libssh2 uses that same return code for more than rejection. During the unsigned probe, any non-EAGAIN `_libssh2_packet_requirev` failure or short response is rewritten to `PUBLICKEY_UNVERIFIED` at `userauth.c:1679-1686`. The final signed-response path similarly rewrites packet/protocol failure at `:1876-1879`.

A concrete sequence is:

1. The agent enumerates one or more valid identities.
2. GWZ sends the first public-key probe.
3. The peer disconnects, or the native packet reader fails before a valid response.
4. libssh2 overwrites the underlying error with `PUBLICKEY_UNVERIFIED`.
5. `authenticate` breaks to the next identity and re-enters authentication on the failed session. With one identity it instead falls through to `PermissionDenied`.

This violates the documented sequencing, can attempt another identity after a native failure, and can diagnose a transport/protocol failure as credential rejection. Existing tests distinguish EAGAIN from ordinary refusal but do not exercise a hard native failure at this boundary.

Distinguish proven server rejection from the overloaded native error. If the pinned API cannot preserve the origin reliably, fail closed on ambiguous `PUBLICKEY_UNVERIFIED` rather than advancing; adjust any signed-rejection fallback claim accordingly.

**Closure regression:** terminate a two-identity attempt when the SSH peer closes or sends a malformed response after the first public-key offer, prove no second identity is offered or signed, and preserve a non-credential error. Retain a separate valid server-refusal case proving the intended next-key path.

## 2. Invariant analysis

The remaining attacks held:

- Host-key equality is checked before agent construction or access. Empty, mismatched, and invalid usernames fail before signing.
- The SSH socket and session become nonblocking before native authentication. Agent and native waits spend the same tested absolute deadline and respond to cancellation.
- Callback userdata, username, key and control remain live across native EAGAIN calls. The exclusive `Session::raw` guard prevents concurrent native access.
- The callback validates the signed payload’s message, username, service, method, selected key and trailing bytes. Only Ed25519 and RSA SHA-256/512 proceed.
- The callback never returns `ALGO_UNSUPPORTED`, so libssh2’s RSA/SHA-1 fallback is not entered. A repeated callback fails instead of replaying an agent request.
- Rust unwinding is contained. Signature size is checked before native allocation; default Unix malloc/free pairing and every observed native free path match the pinned source.
- Agent state is dropped before successful connection handoff. Failure and cancellation destroy the agent and shut down TCP before joined disposal; claimed success remains usable after dropping `Job`.
- The checkpoint accurately limits evidence to the Unix local fixture. It does not activate production routing, A3 pool integration, Windows, selected-source qualification, or a public API.

## 3. Risks and next action

Allocator OOM injection and a heap census remain explicitly unclaimed. The generic agent factory also relies on A3 preserving the tested capture of the same `Control`; that production call site does not yet exist. Platform primitives, selected-source reconstruction, discovery, handshake/trust-file I/O, and physical-pool observations remain valid deferred gates.

Correct P2-1 with one hard-native-failure regression, rerun the focused/full fixture gate, and return this axis for a focused re-verdict before A2 acceptance.
