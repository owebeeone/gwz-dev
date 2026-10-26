# Step 1.6 cross-binding reuse authorization (gwz-transport 9f8f091, gwz-core 726e9062): code-axis review

**Review object:** gwz-transport `70516c0e..9f8f091b1c`, limited to 1.6's part: `a21e972` plus the merge resolution in `exchange.rs`, `instance.rs` and `ports.rs`. 1.5's `bd7cf4f6` is reference only. Also gwz-core `2900809f..726e90628a`, the gwz-cli and gwz-py lockfiles, and evidence run `2026-10-25-p1-6`.
**Baseline:** gwz-transport `70516c0e` (1.3b) and gwz-core `2900809f`.
**Date:** 2026-10-25
**Axis:** Code (architecture, interfaces, call graphs, compatibility, and the cryptographic construction as code).
**Verdict: NO-GO.** P0 0, P1 0, P2 1, P3 5. I commit in advance to GO once P2-1 is fixed with the bounded remedy below and its closure rows pass. That remedy is about 60 production lines in the crate plus 2 rows. The P3s can be fixed in the same round or recorded.

## 0. Evidence base

- **Tuple at start and end (unchanged):**
  - lane root `fc5e7b73`
  - gwz-core `726e90628a` (clean)
  - gwz-transport `9f8f091b1c` (clean, equal to `origin/main`)
  - gwz-cli `91dab04522`
  - gwz-py `3b7af0d9fb`
  - gwz-core-evidence `d3b930a81f`. One untracked directory, `runs/2026-09-22-alpha-setup-timeout/`, was there at both start and end; it is not part of this run.
  - Other members: gwz-git `e3bc2a41f6`, gwz-sspi `a2978a8f3f`, gwz-credential `6d78c593a4`, git2-rs `d13951f7e0`, libgit2 `b172e3d187`, taut `913e0dc35c`, taut-shape `89916c4d01`, taut-shape-rs `959f0f048a`, taut-shape-py `da79bfb743`, bazel-gwz-dev `de0a5071f6`.
- **Object size differs from the brief.** gwz-core `2900809f..726e9062` is two commits: `9a9a659e` (pin move only) and `726e9062`. `726e9062` is **20 files, +2109/−24**, not the 18 files and +1211 in the brief. `reuse_auth.rs` is 702 lines and `https_worker/reuse_tests.rs` is 196. The crate commit `a21e972` is 6 files, +374/−28. The merge adds 6 files, +371/−36 over `bd7cf4f6`.
- **Read in full:**
  - crate: `instance.rs`, the `ports.rs` diff, the `fakes.rs` diff, `reuse_tests.rs`, and the combined merge diff (`git show --cc 9f8f091`)
  - gwz-core: the `agent_auth.rs` and `agent_keys.rs` proof code, the `ssh_pool.rs` lease, `retire_idle` and `Proofs` code, the `ssh_worker/runner.rs` `screen` and `settle_proofs` code, `ssh_local.rs` evidence recording, `ssh_network::recheck_trust`, the `https_pool.rs` `serves_binding` path, and every `p1_6_` row
  - the pool machine's allocation and abandon paths, `open_request.rs` expiry and Capacity mapping, and the Cargo.toml dependency tables
- **Commands run (load 4.6 to 19 throughout):**
  - `cargo test -p gwz-connection` with `CARGO_TARGET_DIR=…/review-p1-6-code`: **63 passed**. The directory was deleted afterwards.
  - `RUSTFLAGS='--cfg gwz_transport_candidate' cargo test --lib p1_6_` in the warm directory, with `SSH_AUTH_SOCK` unset: **12 passed**. None of the 10 build warnings is in a file 1.6 touched.
  - `… cargo test --lib p1_6_an_expired_helper -- --nocapture` printed `p1-6 finding probe: binding B's request carried binding A's token`.
- **Evidence README and raw files:**
  - The Windows receipt `d` (hosted-like run) lists the 13 `p1_6_` rows, including `…_real_sspi`.
  - Mutation logs M1–M6 and the merge logs are present.
  - The probe line the README cites is in none of the filed logs (P3-3).

## Problem, requirement provenance, and scope

- **Requested outcome** (plan §4 step 1.6; contract §7.1 and §7.2; reuse design §4):
  - Before another binding's idle connection is leased, there is an SSHSIG possession proof (ambient keys) or an explicit-key equality check, plus a known_hosts re-check.
  - HTTPS connections authenticated by a helper token or the logon session are never leased across bindings.
  - Red rows: key removed, known_hosts changed, expired helper token, and real SSPI on Windows.
- **Scope-changing constraints:**
  - The new dependency `ring =0.17.14` traces to reuse design §14 C9 ("core lacks a verifier … The crate is chosen under the release's pin rules"). It is authorised.
  - No verifier was already in the production resolve. The consumer locks gain ring, untrusted, getrandom 0.2 and windows-sys 0.52, so none was there before. gwz-core's rustls, tokio-rustls and russh are dev-dependencies only. gwz-sspi is Windows-only.
  - The new crate port `authorize_reuse` is named in contract §4.1. It is authorised.
  - The lane owner's fail-closed decision on P-521 departs from C9, which lists P-521. It is not a defect, but it must be recorded (§3).
- **Component authorisation.** Every component touched is authorised: the gwz-transport crate, gwz-core, the two lockfile-only commits, and the evidence run.
- **Files touched outside the brief.** All are necessary, not expansion:
  - `Fault::Refuse`: the only way to withdraw a key from the fixed-key pipe agent on Windows.
  - The `real_sspi.rs` row: the brief requires the logon-session row on dabeest.
  - The two `3.3.json` rows: the Windows-parity gate failed without them (Linux run 1).
  - `runner.rs`: the call site where the worker hands out leases.
- **Out-of-scope discoveries** are listed under §3 as observations.

## 1. Findings

### P2-1: The reuse proof is fire-and-forget at the port, so a late verdict from one binding's proof serves another binding's lease (ARCHITECTURAL)

- **Location:**
  - gwz-transport `crates/gwz-connection/src/ports.rs:155-159`: `Completion::Reuse { connection, verdict }` carries no binding or work generation.
  - `ports.rs:166-170`: `ReuseCommand { connection, binding }` has no proof token.
  - `ports.rs:222`: no cancel.
  - `instance.rs:512-520`: `reused()` matches `authorizing` by `item.lease.connection() == connection` only.
  - `instance.rs:278-288`: `abandon` returns the lease to idle without telling the adapter.
- **Violated invariants:**
  - Contract §4.1: "All completions include the endpoint, binding, connection and work generation they belong to. Duplicate or late completions never release another generation's reservation."
  - Contract §7.1: the proof is the requesting binding's, and "Only binding cancellation cancels the shared job."
  - A connection id identifies a connect uniquely, but over time it carries several proofs. It cannot stand in for a proof generation.
- **Reproduction (by code path; deterministic):**
  1. Binding A finishes, so connection X is idle with last lessee A.
  2. Binding B opens. The adapter holds proof P(X, B).
  3. B's wait is dropped. `abandon` releases X as `Reusable`; the lessee is still A.
  4. Binding C opens. X is leased, `last` = A ≠ C, so `authorize_reuse(X, C)` runs and `authorizing = [C on X]`.
  5. P(X, B) ends `Authorized`. `reused(X)` finds C's entry and calls `serve(C)`. C's own verdict later finds nothing.
  6. With `Revoked`, C's lease is discarded on B's verdict instead.
  - The fake hides this: `fakes.rs` `end_reuse` ends every recorded command.
- **Impact:**
  - Binding C gets a connection another binding last used on the strength of a different binding's proof.
  - For an `ssh-add -c` key, B's confirmation authorises C, or C is never asked.
  - An abandoned or cancelled proof cannot be stopped, so its agent prompt and job budget run to the adapter's own clock.
  - The bug is latent at this tuple: no gwz-core adapter implements `authorize_reuse` yet (fakes only; README "Limitations"). That is why it is P2 and not P1. It becomes live with the 1.3b/1.3c adapter.
- **Correction:**
  - Make the proof a typed work item: `ReuseCommand { connection, binding, lease }` (or a host-minted `ProofId`), echoed in `Completion::Reuse`.
  - `reused()` matches on `(connection, lease)` and drops the completion when that lease is no longer the one in `authorizing`.
  - Add `EndpointAdapter::cancel_reuse(token)` (§4.1 `cancel_work`), called from `abandon` and `fail_in_flight`.
- **Closure test:** a crate row with the fake ending only B's command. It must assert that C is not served and that C's own verdict serves C. A second row asserts that `cancel_reuse` is issued on the dropped opening.

### P3-1: An agent that answers a `rsa-sha2-512` request with `ssh-rsa` is treated as a withdrawn key, so its connections are retired

- **Location:** gwz-core `src/git/endpoint/agent_keys.rs:240` (a mismatched name returns `Proof::Invalid`), then `agent_auth.rs:406` (`Possession::Refused`), then `ssh_worker/runner.rs` `settle_proofs` (`refused` retires every idle connection with that key).
- **Violated invariant:** reuse design §4, "Why retire": a negative answer must mean no session can show the connection's authority. A key the transport cannot check is `Unprovable` (`agent_keys.rs` doc on `Proof::Unsupported`).
- **Reproduction:**
  1. Use the supported downgrading agent (`Signed::Downgraded`, `agent_auth.rs:288-292`; the TR2.8 list §3 case 3; `ssh_tests/key_fixture` can script it). Binding A authenticates.
  2. Binding B leases A's idle connection. The proof reply is named `ssh-rsa`, so it is `Refused`.
  3. A's connection and every idle connection with that key are retired, on every cross-binding lease.
- **Impact:** For a supported agent class, connections are torn down and reopened again and again, and a revocation is reported where nothing was revoked. It fails closed, with no exposure.
- **Correction:** In `verify_proof`, an `ssh-rsa` reply to the `rsa-sha2-512` request returns `Proof::Unsupported`. The connection goes back to idle and the opener gets a fresh one.
- **Closure test:** a downgrade-agent row asserting the verdict is `Unprovable` and that A's idle connections survive (pool counts).

### P3-2: RSA keys outside 2048–8192 bits are still sent to the agent before being classed `Unprovable`

- **Location:** `agent_keys.rs:219-221`. `provable()` excludes only DSA and P-521; the modulus size is checked only after signing (`verify_proof`, the RSA arm).
- **Violated invariant:** the evidence README says "P-521, DSA and other RSA sizes are `Unprovable`: never sent to the agent". The row comment in `reuse_auth.rs` says "A key no binding can prove is never sent to the agent".
- **Reproduction:** add `-t rsa -b 1024` to `p1_6_the_agent_proves_possession_of_each_key_type…`. The expected result is `Unprovable`, but `agent.signs()` goes up by 1.
- **Impact:** an extra agent round trip whose result is thrown away. For a confirm key, the user is shown a prompt that gwz then ignores.
- **Correction:** `provable(blob)` reads the modulus bit length from the blob (as `KeyType::key` already does) and returns false outside 2048–8192.
- **Closure test:** the RSA-1024 case in that row with `asked = 0`.

### P3-3: The helper-token red row records the token crossing bindings as a log line nobody captures, and the evidence log it cites does not contain it

- **Location:** gwz-core `src/git/endpoint/https_worker/reuse_tests.rs:126-137` (`eprintln!` in a passing test, so libtest swallows it). The evidence README's "Finding probe (`raw/mac/green-gwz-core-rows-1.log`)" points at a log with no such line; a grep of all of `raw/` finds no probe output.
- **Violated invariant:** process rule that a reported finding is backed by filed evidence.
- **Reproduction:** `cargo test --lib p1_6_an_expired_helper -- --nocapture` prints "binding B's request carried binding A's token". This review has now reproduced it. The row passes either way.
- **Impact:** The reported finding is not reproducible from the run. The row will not change when routes become per-binding, or if this regresses afterwards.
- **Correction:** Assert the current behaviour as a named known defect (`assert_eq!(a_auth, b_auth, "OPEN: routes keyed by operation name …")`), or add an `#[ignore]` row for the desired behaviour that names its owning step. File the `--nocapture` output.
- **Closure test:** the assertion is in the row and the log is in the evidence.

### P3-4: No crate row covers the merged composition (scope fit before proof) or sealing during a proof

- **Location:** `crates/gwz-connection/src/reuse_tests.rs`. Its 5 rows never pair a scope misfit with a last lessee from another binding (`instance.rs:399-450`), and never seal the host with an opening in `authorizing` (`instance.rs:306-310`).
- **Violated invariant:** the lane-owner claim that the merge ordered "scope fit first, then the cross-binding proof", and the brief's review area on sealing with leases waiting for a proof.
- **Reproduction (by inspection):** none of the 63 crate tests reaches these paths with a proof in flight. Two edits would leave all 63 passing: moving the lessee check above the fit check, or deleting the `authorizing` drain in `fail_in_flight`.
- **Impact:** both semantics can regress silently in 1.5's in-flight misfit remediation.
- **Correction and closure test:** two rows.
  - A misfit lease last used by binding A is retired with no `authorize_reuse` call (`reuses()` empty).
  - The host is sealed while a proof is held: the opening gets `Closing`, the lease is discarded, and the late verdict is ignored.

### P3-5: Cancelling the only waiter's open cancels the shared proof job and loses a proof that would have passed

- **Location:** gwz-core `ssh_worker/runner.rs:611-616`. When `shared.waiters` is empty, `swap_remove` drops the job. `OpenRequest::expired` (`open_request.rs:185-203`) treats the per-open `cancelled` flag as expiry.
- **Violated invariant:**
  - Contract §7.1: "Only binding cancellation cancels the shared job."
  - Reuse design §4: "A passed proof is recorded for the binding even when the request that started the job has already failed", and "at most … one human confirmation per operation and key".
- **Reproduction:**
  1. Binding B's open is the only waiter on a confirm-key proof (the Unix proxy can delay the reply).
  2. Cancel that open.
  3. B's next open on another binding's idle connection signs again: `signs()` goes up by 2.
- **Impact:** a second confirmation prompt, and a second agent round trip, for the same binding and key. This is bounded.
- **Correction:** Keep a job with no waiters until it ends. It is already bounded by the deadline it started with, so `deadline` stays. Record a passed verdict into `Proofs` with no waiters to serve. Cancel only on the binding's end, which the crate host will own.
- **Closure test:** the reproduction above, with `signs()` going up by exactly 1.

## 2. Invariant analysis (attacks that failed, with evidence)

- **The exact signed bytes:**
  - `proof_data` (`agent_auth.rs`) builds `"SSHSIG"`, then string(namespace), string(""), string("sha512") and string(SHA-512(nonce)): 135 bytes. Row `p1_6_the_proof_signs_exactly…` asserts them byte for byte.
  - The OpenSSH oracle row checks the namespace, the empty reserved field and `sha512` inside `ssh-keygen -Y sign`'s blob. It then verifies OpenSSH's signature against **our** `proof_data(SHA512(message))` for Ed25519, P-256, P-384 and RSA-3072. A verification can only pass if both sides signed identical bytes, so this oracle tests the same construction, including the hash algorithm string.
  - Damaged signatures and damaged data both fail. P-521 is `Unsupported`.
  - Because the blob starts with the `SSHSIG` preamble, it cannot parse as SSH user-authentication data, so neither signature can stand in for the other.
- **RSA:** `proof_algorithm` is `rsa-sha2-512` and `flags()` gives 4. The reply's name must equal it (`:240`). The signature is left-padded to the modulus length before ring's `RSA_PKCS1_2048_8192_SHA512`.
- **The key a signature is verified against** is the blob the server accepted (`authenticate_recorded` returns `key.clone()` on rc 0), not whatever the agent lists. Certificates are verified by their embedded key; the field order of nonce, then point, or e then n, matches the certificate formats.
- **ECDSA and security keys:** `fixed_ecdsa` rejects a value longer than the field, an empty value and trailing bytes. Security keys must carry exactly 5 trailing bytes. The signed data is SHA-256(app), flags, counter, then SHA-256(data), with ring hashing SHA-256 for P-256, which matches PROTOCOL.u2f.
- **The nonce:** 32 bytes from `getrandom::fill` (getrandom 0.3) in a stack array inside the job. Only its SHA-512 enters the heap `Vec`. Nothing logs `data` (grep of the diff).
  - A `getrandom` failure maps to `Refused`. That would retire connections, but such a failure is not reachable in practice, so it is noted and not a finding.
- **Key types that fail closed:** DSA and P-521 never reach the agent (agent sign count +0, asserted). Withdrawn keys are `Refused` (asserted for every type). Forged signatures prove nothing (Unix: last byte flipped; Windows: wrong algorithm name, so on Windows the ring failure path is covered only by the oracle row).
- **known_hosts re-check:** it reuses the setup's `read_regular` (at most 4 MiB, regular file), `validate_lines`, `load_known` and libssh2 `check_port`, so hashed entries and the `[host]:port` form behave exactly as at setup. The names are rebuilt as at setup by `trust_names` (the lowercased key host, plus the host as the URL wrote it). It checks the recorded `trusted` host key. A missing file is the `NotListed` refusal. The red row returns `ErrorCode::Trust`, and after the file is restored the open gets a fresh connection.
- **Explicit keys:** the pool leases only an equal `Identity::Explicit(token)`. `screen` asserts equality and discards on a mismatch, which cannot happen. The changed-bytes row shows the old connection is never reused.
- **Password and helper SSH connections** carry no `Evidence`, so the lease goes back to idle and the opener asks fresh: they are never shared across bindings.
- **Per-binding cache:** it holds public blobs and host keys only, bounded at 256 bindings by 64 entries.
  - `Lessee::First` is the setup's own proof. The pool machine leases a new connection only to the request that opened it (`allocation.rs:~280`); an abandoned opening is closed, never handed on (`lifecycle.rs` `fail_request`).
- **Shared job:** keyed by (binding, key, host), under the latest waiter's deadline, restarted on an aggregate timeout for later joiners. An expired waiter leaves alone and its lease returns `Reusable`.
  - A full job budget gives `WouldBlock`, which maps to `ErrorCode::Capacity` (`open_request.rs:104`), matching the design's table. No row covers this path.
- **Retirement:** `retire_idle` matches on the same key (possession) or the same host and host key (trust), through the idle-loss path. A lease that raced in is seen by `lost()` and becomes `Dead`, then fresh (`ssh_worker.rs:292`). The pool-count row shows only B's connection is left.
- **Crate default:** `authorize_reuse` returns false, which the host takes as `Unprovable`: the lease goes back to idle and the opener asks fresh (row `p1_6_an_unprovable…`).
- **Merge resolution:**
  - Fit is checked first (`instance.rs:399-429`, a misfit is discarded and asked again under 1.5's bound), then the proof (`:430-450`).
  - 1.5's `served` flag moved into `serve()`, with the same semantics.
  - `abandon` by slot covers both the submitted request and the request it asked again with.
  - `fail_in_flight` drains `authorizing`.
  - No dead code; clippy is clean (merge log).
- **HTTPS:**
  - `Identity::Https` (plain TLS) serves any binding.
  - `HttpsScoped` from another binding goes back to idle, followed by exactly one fresh lease within the remaining allocation.
  - The owner session is the input's (`prepare.rs:242`).
  - Helper-token and logon-session rows, including real SSPI, are green on Windows receipt `d`.
- **Repository rules:** no new `static`, `thread_local!` or `OnceLock`. `#[cfg]` appears only inside `cfg_if!`. Control-flow bodies are braced. The test module is `cfg(test)` (`endpoint/mod.rs:80`). Every touched `.rs` file is under 1000 lines (`runner.rs` 725, `ssh_pool.rs` 875, `reuse_auth.rs` 702).

## 3. Risks and next action

**Next action:** fix P2-1 (proof token, `cancel_reuse`, two rows), and P3-1 to P3-5 or record them. Then re-review the crate diff only.

**Seam and observations (not findings):**
- **Two last-lessee records.** The crate keeps `Setup.lessee` by `BindingId`; gwz-core's `PoolHost.lessee` uses the bridge `session_id`. When the 1.3b/1.3c adapter wraps the worker, exactly one must own the record, or proofs will run twice or under mismatched binding identities.
- **Binding identity in gwz-core.** gwz-core uses the bridge `session_id`. In-process drivers mint it with `unique()`. Reuse design T3 forbids client-chosen ids. A remote placement's Bind id is client-chosen. I did not establish whether such an endpoint serves several sessions at 1.1.0 scope (hypothesis).
- **Helper token crosses bindings (confirmed in the test rig).** HTTPS routes are keyed by operation name, so binding B sends A's helper token (reproduced). This is pre-existing and belongs to the multi-binding step (C2(b)). Reachability in production at this tuple is not established by me.
- **Pool terminology.** "The pool has no last-lessee or `retire_idle`" is true of gwz-transport's `PoolMachine`. gwz-core's `PoolHost` has both. Revocation closes as `Discarded` or idle-closed, not `Revoked` (T4 not done).
- **Untested key classes.** Certificate and security-key proof paths exist but have no row. A certificate oracle can be built with `ssh-keygen -s` without hardware.
- **Dependency weight.** ring brings in duplicates: getrandom 0.2 beside 0.3, and windows-sys 0.52 beside 0.61. The publish chain is unchanged in kind (cc/asm, as for libssh2-sys).
- **Structural debt for the operator to decide.** `runner.rs` grew from 354 to 725 lines, and it is shared with 1.3b. A `ssh_worker/reuse.rs` split would isolate `screen` and `settle_proofs`.
- **Interaction with 1.5's misfit remediation.** That remediation (a fresh resubmission under the original deadline) should merge with 1.6's `ask_again`, which asks fresh from the template and adds no deadline. The crate's openings carry no deadline today, and `ReuseCommand` carries none.

**Contract §15 text for the lane owner to record (step 1.6 row):**
> 1.6 (gwz-transport `a21e972`, merged `9f8f091`; gwz-core `726e9062`): `EndpointAdapter::authorize_reuse` → `Completion::Reuse{…, verdict: Authorized|Revoked|Unprovable}`. A false return or the default counts as Unprovable, and the binding never shares. The last lessee is kept in the instance's per-connection `Setup`. Scope fit comes before the proof. The proof is correlated by [lease/proof token after P2-1] and cancelled by `cancel_reuse`. gwz-core's ambient proof is SSHSIG under namespace `gwz-connection-reuse-v1@owebeeone.github.io`, hash `sha512`, RSA `rsa-sha2-512`, verified with ring `=0.17.14` (candidate table; C9). Classes that cannot be proven and are never reused across bindings: P-521 (departing from C9's P-521 list, by lane-owner decision), DSA, RSA outside 2048–8192, and agents that downgrade RSA to `ssh-rsa`. gwz-core's lease path is today's SSH worker, which takes the bridge session id as the binding. It has its own lessee and `retire_idle` over idle loss, because gwz-transport's pool has no T3/T4 (open). HTTPS: `HttpsScoped` connections (helper token, logon session) are leased only to their last binding; plain TLS is shared. Open: routes keyed by operation name carry one binding's helper token to another (owner C2(b)). There is no gwz-core adapter implementing `authorize_reuse` yet (1.3b/1.3c).
