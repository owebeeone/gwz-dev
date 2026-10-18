# SSH agent A2 remediation 1 — CODE-AXIS REVIEW

**Review object:** `gwz-core` `61da27a63a8df42ee92eab909be23d31db665005`, remediation delta from `131989bc9ec6fe6e8c0803e60a9e72654dcc1a25`; controlling checkpoint `dev-docs/GwzRemoteTransportSshAgentA2.md`, status “remediation 1 implemented; retained re-review pending,” dated 2026-09-21.  
**Baseline:** root `53d60168cf2e2e5dc59cb4fa831276a5f69884fd`; gwz-core-evidence `e242351237c2a1bc006c8f6c795f2b56b5ef947f`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources and evidence were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Code error classification, identity sequencing, native-call observation, and direct ownership interactions. Independent, adversarial, read-only. Nothing here relies on the parallel current-round review.

**Verdict: GO** — Code P2-1 is closed. No new P0–P3 finding was established in the remediation or its direct interactions.

---

## 0. Evidence base

I read the complete re-verdict prompt, merged remediation plan, revised A2 checkpoint, and the full `131989b..61da27a` diff. I inspected the corrected authentication loop, test-only observer boundary, native-disconnect regression, fixture socket handle, and retained explicit-rejection success paths.

The corrected source and two test/support hashes independently match the private remediation `inputs.json`. Counts are 267 source lines, including the test-only entry, and 617 test/support lines, within the revised 350/620 limits.

I did not rerun the optional suite. The archived evidence records the regression failing against the original branch because native authentication entered with the second identity. The corrected focused gate passes eight A2 tests, and the full gate records 62 passing executions, including 54 retained executions.

The tuple was verified unchanged at start and end. Root/core trees remained `3af7c3fe5265a1ae2b062652396109e1a96a3b08` and `65b7440a97d0c45380f076eb5b5ff453f1b1d4a3`. All member SHAs remained pinned and clean; root contained only the two permitted untracked A2 remediation prompts.

## 1. Prior-finding closure and changed-range analysis

| Prior finding | Status | Closure evidence |
|---|---|---|
| Code P2-1 — `PUBLICKEY_UNVERIFIED` conflates server rejection with native transport/protocol failure | **Closed** | `agent_auth.rs:112-120` now advances only for `LIBSSH2_ERROR_AUTHENTICATION_FAILED`. Every other terminal native result, including ambiguous `PUBLICKEY_UNVERIFIED`, returns `io::ErrorKind::Other` immediately. The outer identity loop therefore cannot offer another key after that result and cannot collapse it to final `PermissionDenied`. |

The regression at `tests/transport_ssh/tests/agent_auth.rs:285-337` exercises the original counterexample through real libssh2. It retains two distinct agent identities, pauses the SSH peer, shuts down the test TCP handle after the first observed EAGAIN, and observes a final native `PUBLICKEY_UNVERIFIED`. It proves every native call used only the first identity, no agent signing request occurred, the returned error is `Other`, and both TCP and agent owners close. The archived red run confirms the same observer/test reached the second identity under the original branch.

The test observer is enclosed by `cfg(test)` at `agent_auth.rs:126-135` and re-exported only under the same boundary. The ordinary private entry always supplies a no-op observer. Observation happens after the exclusive raw-session guard is released and changes no callback, allocator, signing, or production ownership behavior.

The retained Ed25519 and RSA success cases still begin with an explicitly rejected identity and then authenticate with the approved identity, preserving the unambiguous `AUTHENTICATION_FAILED` next-key path.

## 2. Invariant analysis

The focused attacks held:

- Ambiguous native failure is terminal, preserves a non-credential error, and cannot advance identity state.
- Explicit unsigned-probe authentication rejection still advances exactly once to the next listed key.
- The native observer neither replaces nor stubs libssh2; the regression records actual key and return-code boundaries.
- Failure still drops the agent and shuts down the owned TCP connection before joined disposal.
- Callback ABI, signer lifetime, algorithm checks, allocator transfer, cancellation, and successful connection handoff are unchanged from the reviewed A2 object.
- The checkpoint accurately records that final signed rejection is also terminal because libssh2 reports it through the ambiguous code.
- No production activation, A3 integration, public API, wire change, or deferred platform/source qualification is claimed.

## 3. Risks and next action

Allocator OOM injection and heap census remain explicitly unclaimed. Platform primitives, selected-source reconstruction, discovery, handshake/trust-file I/O, and A3 physical-pool observations remain separate gates.

Accept remediation 1 for the A2 Code axis and proceed to aggregate A2 acceptance before starting A3 integration.
