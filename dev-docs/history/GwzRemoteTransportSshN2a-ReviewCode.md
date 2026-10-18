# GwzRemoteTransport SSH N2a Code Review

## Verdict

**NO-GO**

The snapshot registry, quota ownership, container classifier, cancellation handling, and proof-promotion ordering are coherent within the bounded N2a scope. Two P2 findings block acceptance.

No P0, P1, or P3 findings were identified.

## Reviewed object

| Repository | Commit | Tree |
|---|---|---|
| root | `c33005d576e59ef46222e465285ae1eccf7ceedd` | `47116805f3467a0e1293562b62cc80e399f5c10f` |
| gwz-core | `42f90094dae33eef1cd4828d2c97cadda2761f43` | `141dfbe834a0821542bbb2dc8157fb6a84c1b92d` |
| evidence | `b5375aceb57773cf79ba25b443cafa2ada38bc08` | `0411e40d23e6de487e14ba94f1b70866350e18cd` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| git2-rs | `ce78628308e11b4e8901d5061602619109bce21a` | `28fc6ad905653db47a7705706e1edea0f39d7054` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |

The tuple was verified unchanged at review start and end. Member repositories were clean. Root contained only the two permitted untracked N2a review prompts.

## Findings

### P2-1 — Native transport and protocol failures are reported as credential rejection

**Location:** `gwz-core/src/git/endpoint/ssh_key_auth.rs:68-75`, interacting with `ssh_setup.rs:293-305`.

Every `userauth_pubkey_memory` error except `EAGAIN` becomes `io::ErrorKind::PermissionDenied`. The setup boundary then reports `PermissionDenied` as protocol `Authentication`.

That classification is false for concrete results produced by the pinned native implementation. `libssh2-sys` 0.3.3 `userauth.c` can return:

- `LIBSSH2_ERROR_ALLOC` while constructing the request (`userauth.c:1794`);
- `LIBSSH2_ERROR_SOCKET_SEND` after transport send failure (`:1856`);
- `LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED` when receiving the response fails or produces no packet (`:1878`);
- the same `PUBLICKEY_UNVERIFIED` value for a rejected key or username (`:1897`).

Thus, a disconnect during selected-key authentication is externally reported as an authentication failure. The existing agent path already recognizes this ambiguity and maps non-explicit failure to `Other` at `agent_auth.rs:112-120`.

**Required closure:** retain `EAGAIN` polling; map only an unambiguous authentication-failure result to `PermissionDenied`; map `PUBLICKEY_UNVERIFIED`, socket/protocol failures, allocation failure, and other ambiguous terminal results to truthful non-authentication errors while failing closed and disposing the connection. Add a native or injected disconnect/response-failure regression proving no retry or fallback and an external non-`Authentication` result.

### P2-2 — The mandatory encrypted-work-factor evidence uses malformed surrogates

**Locations:** `tests/transport_ssh/tests/key_container.rs:77-80,117-123`; `tests/transport_ssh/tests/selected_key.rs:416-440`; accepted design `GwzRemoteTransportSshSelectedIdentityDesign.md:254-260`; checkpoint claim `GwzRemoteTransportSshN2a.md:45-49`.

The accepted gate requires zero native-auth calls for encrypted PKCS#8 with extreme PBKDF2 parameters, traditional encrypted PEM, and OpenSSH keys with extreme bcrypt rounds, without executing those KDFs.

The committed cases do not supply those inputs:

- encrypted PKCS#8 contains only bytes `[1, 2]`;
- traditional encrypted PEM contains `AQ==`;
- OpenSSH declares `bcrypt` but uses 16 or 64 raw `0xff` bytes rather than valid `string salt + u32 rounds` KDF options;
- `native_calls` is incremented by a continuation after `load()`, rather than observing the actual native-auth entry boundary.

These tests prove rejection of malformed encrypted-looking data, but they do not prove the accepted extreme-work-factor counterexamples or support the checkpoint’s broad claim.

**Required closure:** add syntactically valid extreme-parameter encrypted PKCS#8 and OpenSSH bcrypt fixtures plus a representative traditional encrypted PEM fixture. Observe the real native-auth dispatch seam, or an exact test wrapper around it, and prove zero calls, `InvalidInput`, and complete slot/byte recovery. N2b’s active-stream and combined-pool checks remain deferred.

## Verification

The permitted locked/offline Rust 1.95 command completed successfully: all 11 `selected_key` and 5 `key_container` tests passed. The archived evidence fingerprints match the committed source, test, manifest, and lockfile blobs. The archived broader suite was inspected but not rerun.

This review accepts no N2b/N3 routing, platform qualification, selected-source activation, or production capability claim.
