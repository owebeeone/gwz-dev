# Step 1.5 profile and partition (gwz-transport bd7cf4f6, gwz-core aaf6efb4): State-axis review

**Review object:** gwz-transport `70516c0e..bd7cf4f605` (`crates/gwz-connection/src`, 10 files) and gwz-core `2900809f..aaf6efb43f`. The gwz-core range holds two commits: `9a9a659e`, which moves the pin to `22b53dc`, and `aaf6efb4`, which carries the step (the pin plus `https_auth.rs`, `endpoint_environment.rs` and `endpoint_environment_tests.rs`). The object also includes evidence run `2026-10-25-p1-5` (gwz-core-evidence `4eaf8613d8`).
**Baseline:** gwz-transport `70516c0e` (step 1.3b); gwz-core `2900809f`.
**Date:** 2026-10-25.
**Axis:** State.
**Verdict: NO-GO. 0 P0, 0 P1, 3 P2, 5 P3.** I commit in advance to GO on the bounded remedies named in P2-1, P2-2 and P2-3 if their closure tests pass.

## 0. Evidence base

**Tuple at start and end:** the same.
- gwz-transport `bd7cf4f605` (parent `70516c0e`), clean.
- gwz-core `aaf6efb43f` (parent `9a9a659e`, grandparent `2900809f`), clean.
- gwz-core-evidence `4eaf8613d8`. One untracked directory was already there before this review: `runs/2026-09-22-alpha-setup-timeout/`.
- Other members: gwz-git `e3bc2a41f6`, gwz-cli `9ecc2b7c1b`, gwz-py `22e4f133a4`, gwz-sspi `a2978a8f3f`, gwz-credential `6d78c593a4`, git2-rs `d13951f7e0`, libgit2 `b172e3d187`, taut `913e0dc35c`. All clean.
- `bd7cf4f6` is on `origin/main`.

**What I read:**
- Every hunk of both diffs.
- The pool code the new code relies on:
  - `src/pool/allocation.rs:4-147, 153-300`: `schedule`, `open_for`, `connected`.
  - `src/pool/machine.rs:210-280`: `request_until`, `take`.
  - `src/pool/lifecycle.rs:116-160, 270-330`.
  - `src/pool/mod.rs:213-245`: `Request.fresh` and `tag`.
  - `src/pool/limit.rs:153`.
- The host registry (`host.rs:398-420`) and binding serials (`host.rs:514-530`, `exchange.rs:76-115`).
- In core:
  - `https_tls.rs` (`SharedTls` `OnceLock`).
  - `https_backend/native_stack.rs`.
  - `machine_proxy.rs`, `bypass.rs` and `proxy_string.rs` (the `Debug` derivations and their 8192/4096 bounds).
  - `agent_source.rs`, `pipe_name.rs` and `pageant/pin.rs` (the `Debug` forms).

**Committed tree against the gated tree:**
- The committed gwz-transport diff is byte-equal to `raw/sources/gwz-transport.patch`.
- The committed gwz-core `src/` diff is equal to the patch's `src/` part.
- The only difference is the pin: it was gated at `70516c0e` and committed at `bd7cf4f6` (see P3-5).

**Builds:**
- I ran `CARGO_TARGET_DIR=…/p1-profile-crate cargo test -p gwz-connection` once, at load 37: **58 passed**. The tree stayed clean.
- I did not run the gwz-core candidate filter, because the load was 42 to 60 for the rest of the review. For core I rely on `core-green-1.log` (60 passed) and `core-green-2.log` (15 passed), and I read the red and mutation logs.
- Gate run c summary: OVERALL PASS. 2383/0/1 ordinary, 3808/0/9 candidate, `check_windows_parity` PASS.

**Controlling text read:**
- Contract §4.1, §7, §7.1, §7.2, §8 and §15, including the 1.3b row.
- Reuse design §3–§6 and §13.
- Plan §1 (threat model), the R4 rows (:169, :283), step 1.5 (:420), 1.6, 1.8 and the exit.
- `GwzSplitCommandChannel.md` §3.6.
- The lane brief.

## Problem, requirement provenance, and scope

**Requested outcome** (plan 1.5, brief):
- Each operation's profile is captured once at entry.
- The registry serves only equal profiles.
- The helper environment and tokens live only in the per-operation `AuthContext`, so equal-profile clients cannot run each other's helper.
- R4: changing `HOME`, the agent, the CA, the proxy, `GIT_CONFIG_GLOBAL` or `GH_TOKEN` gives `reused: false`, then reuse resumes, with no stale event.

**`GIT_CONFIG_GLOBAL`/`GH_TOKEN` as context, not profile: correct, not a defect.**
- Contract §7 requires it: "helper environment and account tokens … are not profile fields. Their compatibility is checked by the lease/auth rules."
- The intent of R4 is met:
  - A connection set up under one scope is never leased to another scope (`instance.rs:343-350`).
  - Every exchange gets its own opening's context (`instance.rs:412-416`).
  - I found no path that presents a stale token.
- The new-instance part of the plan's 1.5 sentence applies to profile fields only.

**Provenance of the port change.** The brief's file ownership names the registry and `instance.rs`. The implementation also changes:
- `ports.rs`: `ConnectCommand.auth` and the third parameter of `start_exchange(…, &AuthContext)`.
- `exchange.rs`, `operation.rs`, `admission.rs` and `binding.rs`.

The change is necessary for the outcome: an adapter otherwise has no route to the operation's context. The contract's port table (§7) and its §15 row ("named adapters receive … AuthContext") support it, and no file the brief forbids was touched. It does not block delivery. Two things are missing: the contract §15 record (P3-3) and a retention rule for the port (P3-2).

**Out-of-scope items, recorded as observations only:**
- 1.6 possession proof.
- 1.3c route naming. The gate-b `RouteKey` change was reverted cleanly; no residue is in the committed tree.
- Not wiping `https_auth::Config` (implementer's finding 3) predates this step. Contract §7 keeps the existing secret handling, and 1.5 adds no copy that outlives its binding (§2 below).

## 1. Findings

### P2-1: A scope misfit retires the offered connection and resubmits a new pool request

**Root cause:** scope fit is checked after the pool has selected a connection, outside the pool's eligibility rules. A misfit is handled by retiring the offered connection and submitting a new pool request.

**Location:**
- `gwz-connection/src/instance.rs:343-372`: the misfit branch, with `release(lease, Discarded)` at :353 and `pool.request(pending.template.clone())` at :362.
- The pool's idle match (`gwz-transport/src/pool/allocation.rs:16-47`) compares key, identity and `reusable` only.

**Violated invariants:**
- Contract §8 retires an idle entry only "when idle entries fill a shared ceiling" (the oldest eligible one), and "selection retries within the same allocation bound".
- Fairness is FIFO among eligible requests.
- Reuse design §5: "a request waits within its allocation deadline".
- The cap's own justification ("each pass retires one of the key's connections, so more passes than the key can hold mean nothing will fit") does not hold under concurrency.

**Reproduction:** each effect below follows directly from the code.
- **Retire with free capacity.**
  - Operation A (scope a) finishes, leaving one idle connection.
  - Operation B (scope b, equal profile) opens. The idle loop runs before `open_for`, so B is offered A's idle connection even though capacity is free.
  - B retires it and opens its own connection.
  - A's next operation misses, retires B's idle connection, and so on.
  - Equal-profile clients that alternate therefore never keep a reusable connection. This also hits SSH connections when only `GH_TOKEN` differs, because the scope covers the whole environment.
  - The crate row `a_changed_auth_context_on_an_equal_profile…` builds this in as expected behaviour: `closes() == 1` at `host_tests.rs:426`.
- **Bound reset and lost FIFO place.**
  - Each resubmission calls `request_until(…, None)` (`pool/machine.rs:233-243`), which gets a new `RequestId` (tail of the FIFO) and a new allocation deadline of `now + allocation_timeout_ms`.
  - An opening can wait up to (`per_user_host` + 2) × 30 s instead of 30 s.
  - Each misfit puts the opening behind every request that arrived meanwhile.
- **Spurious `NoFittingConnection`.**
  - With the pool's 32 connections full of scope-a leases, B waits.
  - Each time an A exchange ends while B is at the head, B is handed the idle connection, retires it and moves to the tail. New A requests then go ahead of it and take the freed slots.
  - After 34 such offers, B fails `NoFittingConnection` ("none will fit"), although a fitting connection would have been opened for it had it kept its place and deadline.

**Impact:**
- A conforming operation can lose progress or fail under legal concurrent load.
- Another binding's idle connections are destroyed without any capacity need.
- The allocation bound is silently extended.
- The fail direction stays closed: no context crosses scopes.

**Correction (not architectural; one owner).** Either of these:
- **(a)** Make scope a pool eligibility field: an opaque compatibility token on `Request` and `Entry`, compared beside `identity` in the idle match and set from the opening request. Other-scope idle entries are then retired only by the existing full-ceiling eviction. The misfit branch and its cap go away. This needs authorization for a `src/pool` edit.
- **(b)** A bounded change inside `instance.rs`: on a misfit, release the untouched lease as `Reusable` and resubmit once with `fresh: true` through `request_until(…, original absolute deadline)`. This gives at most one misfit per opening and no retirement outside eviction.

**Closure tests:**
1. A, then B (another scope), then A: B opens a new connection, `closes() == 0`, and A's second exchange has `reused: true`.
2. With the fake clock, a misfit opening still times out at its original allocation deadline.
3. With a saturated scope-a load and a waiting scope-b opening, B is served within its deadline and never gets `NoFittingConnection`.

### P2-2: An oversized canonical profile refuses every network operation on the production path

**Root cause:** the canonical profile is not bounded to fit `MAX_PROFILE_BYTES` (16 KiB, `operation.rs:93`) given the bounds its inputs are allowed to reach. When the limit is exceeded, every network operation on the **production** legacy path is refused.

**Location:**
- `endpoint_environment.rs:269`: field 12 is `format!("{machine:?}")`.
- `:267`: field 11, the raw `no_proxy` joined.
- `:189`: `.map_err(|_| invalid("endpoint profile exceeds its bound"))?`, reached from `endpoint_config_native` (`:101`), which `local_command.rs:98` and `cancellable.rs:226` call for every network operation.

**Violated invariant:** an input the capture accepts must not turn a previously working configuration into a refusal (fail-closed for invalid inputs only). Contract §7: an invalid profile fails before effects, and this profile is valid.

**Reproduction:**
- The derived `Debug` of `MachineProxy::Named` prints each bypass `Rule::Host.pattern: Vec<u8>` as decimal bytes, about 4.6× the text, plus about 50 bytes per rule.
- `Bypass::parse` accepts lists up to 8192 characters (`bypass.rs:10`).
- My estimate of the Debug size for N rules of the form `*.appNNN.corp.example.com`:

| Rules | Bypass text | Field 12 bytes |
|---|---|---|
| 100 | 2.6 KB | ≈15.9 KB |
| 227 | 5.9 KB | ≈36 KB |

- So a Windows machine with a legal WinHTTP bypass list of about 2.7 KB, or more, is refused "endpoint profile exceeds its bound" on every network operation. Before this commit, the same machine worked.
- A long Unix `NO_PROXY` hits the same refusal, though less likely.

**Impact:** a stuck refusal for a conforming user, Windows above all. It is an availability regression in the shipped path, and the refusal text does not name its cause.

**Correction:**
- Encode variable-length fields compactly and in bounded form: the captured raw access, proxy and bypass strings as bytes, not `Debug`; or the SHA-256 of any field over a fixed size.
- Add a static check that the sum of the field bounds stays under `MAX_PROFILE_BYTES`.
- Name the field in any remaining refusal.

**Closure test:** `capture_operation` with `MachineProxy::Named` and an 8192-character bypass list, and with a 16 KiB `NO_PROXY`, succeeds. Two such captures that differ in one rule give different profiles.

### P2-3: On OpenSSL builds the trust content behind the platform roots is not in the profile

**Root cause:** on OpenSSL builds (Linux), the profile records only the paths that select the platform roots, not their contents. The instance loads that content lazily, once, at its first connection.

**Location:**
- `endpoint_environment.rs:347`: `CA_FILE_VARIABLES = ["GIT_SSL_CAINFO"]` on non-Apple Unix.
- `:354-365`: `platform_roots` is a lazy loader over `SSL_CERT_FILE`/`SSL_CERT_DIR`, with probed defaults when both are unset.
- Fields 8 and 9 hold the path values only.
- `https_tls.rs:55-63`: `SharedTls` builds the roots once per connector (instance), in a `OnceLock`, at the first connection.

**Violated invariants:**
- Contract §7: profile equality covers the "actual CA/trust content and policy".
- Reuse design §3: "CA certificates | the file named by `GIT_SSL_CAINFO` **or `SSL_CERT_FILE`**, read at derivation".
- The CA case of R4.

**Reproduction (Linux):**
- Operation 1 has `SSL_CERT_FILE=/p/bundle.pem` and connects, so the instance loads the bundle.
- The bundle at the same path is replaced: a root is removed or added.
- Operation 2 captures an equal profile, is served by the old instance and its old roots, and gets `reused: true`.
- With neither variable set, a distribution CA update during a long-lived host (gwz-py) behaves the same way.
- The R4 CA row passes only because it uses `GIT_SSL_CAINFO`. The gate ran on Linux and did not cover this case.

**Impact:**
- Trust is stale in the fail-open direction: a removed root is still trusted for the instance's life.
- The trust content is read at the first connection, not "captured once at entry".
- Before reuse, each operation's runtime loaded its own roots.

**Assessment of the implementer's finding (2):**
- **The macOS/Windows half is refuted.** Without a loader, `native_stack.rs:24-37` adds no snapshot, so Security.framework and Schannel evaluate the live system store on every handshake. No stale instance can result.
- **The OpenSSL half is a defect.**

**Correction (bounded):**
- At capture on OpenSSL builds, resolve the verify paths and read their certificate bytes once, using the same reader as `verify_paths`.
- Put a SHA-256 of that content into the canonical bytes, beside the paths.
- Carry the captured certificates in the profile inputs, so the instance trusts exactly what was captured.

**Closure test (Linux):** same `SSL_CERT_FILE` path, changed content: a new instance with `reused: false`, then `true`. Restoring the original content reuses the first instance.

### P3-1: The capture does not refuse a profile whose inputs carry proxy authorization

**Location:** `endpoint_environment.rs:242-252` (field 10 omits `Proxy.authorization`) and `capture_operation`.

**Detail:**
- Reuse design §3 says "the registry refuses a configuration that carries it".
- Today's parsers set `None` (`:296`, `machine_proxy.rs:111`), so this is latent.
- If any path sets authorization in the future, equal canonical bytes would put a second client on the first client's instance and its proxy credential.

**Correction:** refuse in `capture_operation` when `tls.proxy.authorization.is_some()`.

**Closure test:** a unit test that an authorization value is refused.

### P3-2: No adapter retention rule for `ConnectCommand.auth`

**Location:** `ports.rs:127-137`.

**Detail:**
- The adapter receives an owned clone of the opener's `AuthContext`.
- Nothing says it must drop that clone when the connect completes or is cancelled, or must not use it for later exchanges.
- `no_operations_auth_context_outlives_it_in_the_host` checks the host only.

**Impact:** a conforming-looking 1.3b/1.3c adapter could keep A's context on the connection and outlive A's operation, which goes against contract §7's retention rule.

**Correction:**
- Document the rule.
- Add a fake-adapter conformance row: after the connect completes, `strong_count` returns to the binding's count.

### P3-3: The port signature change is not recorded in contract §15

**Location:** the 1.3b row of contract §15 records `start_exchange(connection, &ExchangeSpec)`. 1.5 adds `&AuthContext` and `ConnectCommand.auth` and leaves no record.

**Impact:**
- The 1.3b/1.3c adapters in the trunk lane must adopt the new signature.
- The cross-lane change is undocumented.

**Correction:** add a §15 row at merge.

### P3-4: R4 and the agent rows do not run where they claim to

**Location:** the evidence README's Limitations and its "Windows receipt: skipped".

**Detail:**
- 1.5 has no fixture-server or sshd rows; the brief asked for them and the production factory is absent.
- Plan 1.8 lists the fixture rows of 1.3 and 1.6 only, yet the Phase 1 exit requires R4 on the hosted-like suite on three platforms.
- The platform-neutral agent rows, and the claim that "Windows records a refusal that shows the value", have never run on Windows.
- Reading the code, Windows does partition on agent identity: the `Debug` of `PipeName` and of `PageantPin { window, process }` differ for distinct agents.
- P2-2's Windows-only failure shows that "no `cfg(windows)` branch touched" is not enough to justify skipping the receipt.

**Correction:**
- Carry R4 (fixture/hosted-like) into 1.8 by name.
- Take a Windows receipt of the `profile_partition` rows with the P2-2 fix.

### P3-5: Evidence and tuple mismatch on the pin

**Location:** the README's source tuple and gate run c.

**Detail:**
- Both record pin `70516c0e`, with the lane's working-tree gwz-transport.
- The commit pins `bd7cf4f6`, which no gate ran against.
- The risk is low: `bd7cf4f6` touches only `crates/gwz-connection`, so the IR digest is unchanged, and it is on `origin/main`.
- The README's "with no stale context" maps to no 1.5 row.

**Correction:**
- Note the pin difference in the README.
- Read the transport-candidate CI after the push.
- Map "no stale event" to `operation_tests::a_late_event_of_binding_g_is_rejected_by_g_plus_one…` (1.2).

## 2. Invariant analysis (attacks that failed, with evidence)

- **The wrong context reaching a connect.**
  - A connect's context is found by tag. The tag is `x{binding serial}-{n}` (`exchange.rs:93`). The serial is never reissued by its host (`host.rs:514-519`), and instances are per host (`host.rs:398-420`), so tags are unique within an instance. A resubmission keeps its single `Pending`.
  - The pool leases a newly connected connection only to the request that opened it (`allocation.rs:283-292`), and a request in `Opening` state is never offered an idle connection (`:145`).
  - So a new connection's recorded scope is its lessee's own.
  - An opener that has gone gives scope `None`, so the connection never fits; the cancelled connect is closed (`:268-279`).
- **The wrong context reaching an exchange.**
  - `start_exchange` receives `&pending.auth`, the context of the opening that holds the lease, and only after `setup.scope == Some(scope)` matched (`instance.rs:343-350, 412-416`).
  - M1 and M5 are killed. The crate row `each_operations_connects_and_exchanges_carry_its_own_auth_context` and the core R4 token sequence pass.
- **A stale token after `GH_TOKEN` changes.**
  - The token is never stored in the instance, the profile or its inputs. `EndpointInputs` holds `EndpointSettings` and TLS/proxy config only, and the proxy's authorization is always `None` (see P3-1).
  - A connection of the old scope is never leased to the new scope.
- **A context copy outliving the operation.**
  - Holders are the binding (`BindingShared.auth`) and the instance's `Pending` until the slot is filled, the opening is abandoned (`instance.rs:233-246`, which pumps at once) or the host fails in flight.
  - `strong_count` returns to 1 after drop and after finish (crate row).
  - The legacy runtime's helper clone lives for its own operation, as before.
  - Outside the host, the only open path is the adapter (P3-2).
- **M4 equivalence.**
  - Confirmed. With the `retain` removed, `abandon` still pumps under the same lock (`:244`). `pool.take` returns `Stale` for the abandoned request (`machine.rs:261`), so `openings()` removes the entry before the lock is released.
  - During that pump's `actions()`, no `Connect` can carry the abandoned tag, because the pool derives tags from live requests (`lifecycle.rs:294-296`).
- **Scope hash collision or leak.**
  - The encoding is unambiguous: each field is prefixed with its u64 length, with a domain tag, the executable, the entry count, and sorted (name, value) pairs.
  - The bytes are private. `AuthScope` has no accessor, and its `Debug` and `AuthContext`'s are silent (`operation.rs:187`).
  - It never crosses the carrier (split §3.6). Its pre-image includes the whole environment, so it is no practical token oracle.
  - At most, a second client in the same process can learn from `reused: true` that an operation with an identical environment ran before. That is negligible.
- **Profile equality.**
  - Digest, then bytes (`same_as`). M3 is killed, and the collision row passes.
  - The inputs are not compared, and every input field has a canonical counterpart except `Proxy.authorization` (P3-1) and the platform-roots content (P2-3).
  - On my reading, `Debug` encodings are injective for the fields used: OsStr/PathBuf escape backslashes, and the `Rule` order is kept, which only adds partitions.
- **CA file TOCTOU.** The bytes are read once, digested and parsed from the same buffer (`:390-412`). The connector trusts the parsed certificates, so a change after capture is never used under the old digest.
- **HOME or agent changed, or agent socket replaced.** HOME and the agent are profile fields, so a change gives a new instance (CM2 killed). Replacing the agent at the same path is left to 1.6's possession proof, by design (reuse design §3).
- **`reused` truthfulness.** `served` is swapped only on a fitting lease (`:346`). A connection's first exchange reports `false`, later ones `true`. M6 is killed.
- **Retire-path leak.** A retired lease is released `Discarded` and closed. The `connections` entry is removed on `Closed`, `IdleClosed` or a failed `Connected` (`:249-268`). Its budget is held until disposal, as §8 requires. Nothing leaks.
- **Gate run b.** It failed because the reverted `RouteKey` keyed by the per-open session; nothing of it remains (`grep` finds no `bindings_that_share_an_operation_name` test in `src`).

## 3. Risks and next action

**Next action.** Land the three bounded P2 remedies, each with its closure test:
- P2-1, as (a) a pool eligibility token (needs `src/pool` authorization) or (b) the `instance.rs` alternative of a `Reusable` release plus a `fresh` resubmission under the original deadline.
- P2-2: a bounded canonical encoding.
- P2-3: a digest of the OpenSSL trust content, captured once.

Fold the P3 corrections in with them: the refusal, the port rule, the §15 row, the README pin note and the 1.8 carry. Then take a Windows receipt of the `profile_partition` rows and file a re-verdict.

**Residual risks after the remedies:**
- Reuse of HTTPS and SSH across scopes stays disabled. That is stricter than contract §7.2, consistent with plan 1.6, and costs one handshake per client and scope.
- Real-adapter conformance (P3-2) can only be proven once 1.3b/1.3c adapters adopt the new port.
