# SSH Agent A2 Remediation 1 — State-Axis Re-Review

**Review object:** A2 remediation 1 at gwz-core `61da27a63a8df42ee92eab909be23d31db665005`, delta from `131989bc9ec6fe6e8c0803e60a9e72654dcc1a25`, controlled by `dev-docs/GwzRemoteTransportSshAgentA2.md`; remediation implemented, re-review pending, 2026-09-21.  
**Baseline:** root `53d60168cf2e2e5dc59cb4fa831276a5f69884fd`; gwz-core `61da27a63a8df42ee92eab909be23d31db665005`; gwz-core-evidence `e242351237c2a1bc006c8f6c795f2b56b5ef947f`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Terminal-failure state, identity sequencing, ownership, and fail-closed recovery. Independent, adversarial, read-only; no current-round peer output was consulted.

**Verdict: GO** — no P0–P3 findings. The directly affected terminal-failure behavior is closed.

---

## 0. Evidence base

I reviewed the complete canonical prompt, committed remediation plan, controlling A2 document, core diff `131989bc..61da27a`, corrected `agent_auth.rs:43–135`, the new regression at `tests/transport_ssh/tests/agent_auth.rs:285–337`, fixture support at `support/agent_auth.rs:217–225`, and private `agent-a2-rem-1` red/green evidence and source hashes.

Pinned libssh2 1.11.1 source confirms the classification boundary: during this authentication path, `LIBSSH2_ERROR_AUTHENTICATION_FAILED` uniquely records an explicit unsigned-key refusal, while `LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED` also represents malformed keys, callback failure, packet/transport failure, and final signed rejection.

I independently ran the permitted Rust 1.95 offline locked aggregate gate using the external target directory. All 62 executions passed: eight A2 and 54 retained tests; the documented child-only test remained deliberately ignored. The exact tuple was unchanged at review end. Root contained only the two declared untracked prompt files; member trees were clean.

## 1. Closure and changed-range analysis

| Item | Status | State evidence |
|---|---|---|
| Original State findings | None | Original State verdict remained GO. |
| Direct effect of Code P2-1 | Closed | Only `AUTHENTICATION_FAILED` advances; every ambiguous native failure terminates as `Other`, without offering another identity or misreporting a credential rejection. |
| Test-only observer seam | Accepted | Enclosed by `cfg(test)`; production `authenticate` always supplies a no-op observer. It exposes only the borrowed key and native return code after the native call and changes no ownership, callback, allocator, or retry state. |

The preserved red run reproduces the former error: after a real native EAGAIN and TCP shutdown, libssh2 returned `PUBLICKEY_UNVERIFIED`, and the old branch entered authentication with the second identity. The corrected regression uses the same two real agent identities and native API, records the actual key/return boundary, observes the overloaded error as the final return, proves every native call used only the first identity, records no sign request, returns `Other`, and observes TCP and agent closure.

The retained Ed25519 and RSA SHA-256/512 success cases still prove that an explicit first-key refusal advances to the approved second key. Thus the correction preserves the one legitimate transition while eliminating the ambiguous one.

## 2. Invariant analysis

The authentication state grammar is now closed and fail-safe:

- `EAGAIN` retains the same key, callback state, deadline, and native session for bounded retry.
- Explicit unsigned-key refusal advances once to the next enumerated identity.
- Callback errors, malformed keys, signed rejection, packet failure, and transport failure terminate the owned connection because their shared native code cannot be safely distinguished.
- Terminal ambiguity remains a non-credential `Other` result, so the caller cannot infer that credentials were rejected.
- Termination unwinds the stack-owned signer and agent, then drops `SshConnection`; A1 exposes completion only after helper join and disposal.
- The observer runs after native return and before classification, borrows no state beyond that call, and is absent from the production interface.

The correction does not alter host-trust admission, callback ABI, allocator transfer, signature validation, cancellation, or authenticated handoff.

## 3. Risks and next action

The original A2 limits remain: no allocator-OOM or heap census, deferred platform/source qualification, no production discovery or activation, and no A3 pool accounting or observations.

Accept remediation 1 and retain A2 GO. Proceed to A3 integration under its separate ownership, cleanup, accounting, and observation gate.
