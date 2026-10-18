# SSH N2a merged remediation 1 — CODE-AXIS REVIEW

**Review object:** N2a merged remediation 1 at `gwz-core` `dbc7616f7163de4d2bd78b839a50f7a2bc3c92a9`; controlling draft `dev-docs/GwzRemoteTransportSshN2a.md`; 2026-09-21.  
**Baseline:** root `f92a34d9e097b2f947e8d8c544c3bf748fd2d068`; core `dbc7616f7163de4d2bd78b839a50f7a2bc3c92a9`; evidence `f90ce95e83fd17e87be18bf18937f5e470472fc4`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read through `git show` and immutable diffs.  
**Date:** 2026-09-21  
**Axis:** Code architecture, interfaces, call graphs, error behavior, and evidence compatibility. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — both prior P2 findings are closed, and changed-range inspection found no new P0–P3 defect.

---

## 0. Evidence base

I read the canonical re-review prompt, merged RemPlan-1, original Code report, revised checkpoint, the complete core diff `42f90094..dbc7616f`, and the remediation evidence at `gwz-core-evidence` `f90ce95e`. Focused source inspection covered:

- `ssh_key_auth.rs:50-89`;
- `ssh_key_container.rs:26-63,227-289`;
- `selected_key.rs:429-672`;
- `key_container.rs:147-192`;
- the retained setup error mapping and ownership seams.

The seven hashes in remediation `sources.json` exactly match the committed source, tests, manifest, and lockfile blobs. Archived evidence contains the native disconnect red (`Authentication`) and corrected result (`Io`), focused green runs, deterministic scan evidence, and the final locked/offline suite with all executed tests passing.

I independently ran the permitted Rust 1.95 locked/offline focused command. All 13 `selected_key` and six `key_container` tests passed.

The exact six-repository tuple and tree objects were identical at review start and end. Member repositories were clean; root contained only the two permitted untracked `PromptCode-1` and `PromptState-1` files.

## 1. Findings

No new findings.

### Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| Code P2-1 — native transport/protocol failures reported as credential rejection | **Closed** | `ssh_key_auth.rs:68-84` now retries only `EAGAIN`, maps only explicit `LIBSSH2_ERROR_AUTHENTICATION_FAILED` to `PermissionDenied`, and maps ambiguous `PUBLICKEY_UNVERIFIED` plus other terminal failures to sanitized `Other`. `selected_key.rs:429-490` handshakes a real connection, pauses the server, shuts down the socket during authentication, and proves the setup boundary reports `Io`, performs one setup attempt, does not promote the token, and releases quota. Archived red evidence reproduces the old `Authentication` result. |
| Code P2-2 — malformed surrogates substituted for required extreme encrypted containers | **Closed** | `selected_key.rs:492-670` generates valid encrypted OpenSSH, traditional PEM, and PKCS#8 containers cheaply, validates their framing, then changes OpenSSH bcrypt rounds to `u32::MAX` and DER-located PBKDF2 iterations to 8,388,607 without executing either extreme KDF. An exact guarded wrapper around native dispatch records zero calls for all encrypted inputs and one real successful call for the unencrypted control. Each encrypted input returns `InvalidInput`, and registry usage returns to `(0, 0)`. |

## 2. Invariant analysis

The original error counterexample no longer holds. All ambiguous or infrastructure failures terminate immediately as `Other`; only `EAGAIN` repeats the same native operation. No branch opens another identity, agent, file, Git path, or setup attempt. Existing field order still destroys the connection before releasing the snapshot pin on rejected handoff.

The encrypted-container counterexamples now exercise the required valid framing. PKCS#8 validates PBES2 and PBKDF2 OIDs before changing the existing three-byte iteration integer. OpenSSH validates the cipher, KDF, salt string, and round field before changing the final `u32`. Traditional encrypted PEM verifies its `DEK-Info` header. Because the positive control traverses the same wrapper into real authentication, zero encrypted dispatches are meaningful rather than an unexercised counter.

The changed classifier scan introduces no ownership or allocation regression. `checked_scan` checks control before every 128-byte chunk, computes offsets within the bounded 1 MiB input, and propagates cancellation through NUL, leading-whitespace, newline, and trailing-text scans. The test-only exposure remains inside an enclosing `cfg_if!` boundary. The deterministic barrier test proves cancellation after the first 128 predicate calls without visiting a later chunk. Production size remains 598 lines and tests 868 lines, within the accepted limits.

## 3. Risks and next action

This verdict accepts only the isolated N2a snapshot, classifier, native-memory-authentication, and proof-promotion checkpoint. N2b worker admission, resource pins, combined cleanup and capacity-one fan-out; N3 attachment; platform qualification; selected-source qualification; and production activation remain deferred.

The next action is to accept N2a remediation 1 and proceed to the separately reviewed N2b integration gate.
