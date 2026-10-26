# Step 1.5 profile and partition (gwz-transport bd7cf4f6, gwz-core aaf6efb4): CODE-AXIS REVIEW

**Review object:** gwz-transport `70516c0e..bd7cf4f605` (`crates/gwz-connection/src`, 10 files, +520/−61). gwz-core `2900809f..aaf6efb43f`: 4 files, +670/−10. The range is two commits, not one: `9a9a659e` moves the pin to `22b53dc`, and `aaf6efb4` is the step. Evidence run `2026-10-25-p1-5` at gwz-core-evidence `4eaf8613d8`.
**Baseline:** gwz-transport `70516c0e` (1.3b), gwz-core `2900809f`.
**Date:** 2026-10-25.
**Axis:** Code.
**Verdict:** **NO-GO**, with 0 P0, 0 P1, 3 P2 and 2 P3 findings. I pre-commit to GO once P2-1 to P2-3 are closed by the bounded remedies below. P3-1 and P3-2 do not gate.

## 0. Evidence base

- **Tuple at start and end:** identical. gwz-transport `bd7cf4f605`, gwz-core `aaf6efb43f`, gwz-core-evidence `4eaf8613d8`. Other members: gwz-git `e3bc2a41f6`, gwz-sspi `a2978a8f3f`, gwz-cli `9ecc2b7c1b`, gwz-py `22e4f133a4`, git2-rs `d13951f7e0`, taut `913e0dc35c`, libgit2 `b172e3d187`.
  - Every tree is clean except gwz-core-evidence. It has one untracked directory that predates this review and is unrelated to it (`runs/2026-09-22-alpha-setup-timeout/`).
  - `bd7cf4f6` is contained in gwz-transport `origin/main`.
- **Diffs read in full:** `git diff 70516c0e HEAD` and `git diff 2900809f HEAD`.
  - The evidence `raw/sources/*.patch` files are byte-equal to the committed diffs, ignoring `index` lines.
  - The one exception is the pin line. The gated tree carried `70516c0e`; the commit carries `bd7cf4f6`.
  - So the evidence README's tuple (gwz-core `9a9a659e` + patch, pin `70516c0e`) describes the same code with a different pin. The committed pin is the correct one.
- **Pool semantics checked:** `gwz-transport/src/pool/{mod,allocation,lifecycle,machine,limit}.rs`. This covers `Request.tag`, `Action::Connect.tag`, the FIFO `schedule`, `connected`, `release`, `take`, `abandon`, `request_until` and `user_host_total`.
- **Trunk lane checked, read-only:** `/Volumes/projects/limbo/gwz-dev-p1-trunk`, gwz-core `fb26d4dc` plus its 1.3c working tree.
- **Commands run:** both at a load below 40, as the brief requires.
  - `uptime` read 22.3. `CARGO_TARGET_DIR=…/p1-profile-crate cargo test -p gwz-connection`: **58 passed**.
  - `uptime` read 36.8. `CARGO_TARGET_DIR=…/p1-profile-candidate RUSTFLAGS='--cfg gwz_transport_candidate' cargo +1.95.0 test --lib --locked endpoint_environment`: **13 passed**, both `profile_partition` rows included.
- **Logs read:** evidence README, `raw/mutation/round2/summary*.tsv`, `raw/remote/c/summary.txt` (OVERALL PASS), and `build-targets/p1-profile-logs/CHECKPOINT.md`.

## Problem, requirement provenance, and scope

- **Requested outcome (plan §4 step 1.5; contract §7):**
  - The profile is captured once at entry.
  - The registry serves only equal profiles.
  - The helper environment and tokens live only in the per-operation `AuthContext`, so equal-profile Clients never run one another's helper.
  - Row R4: changing `HOME`, the agent, the CA, the proxy, `GIT_CONFIG_GLOBAL` or `GH_TOKEN` gives `reused: false`, then reuse resumes, and no stale event reaches the new operation.
- **Components touched:** the gwz-connection crate, gwz-core and evidence only, as authorized.
- **Scope-changing constraint: the adapter port change.** `start_exchange(connection, &ExchangeSpec, &AuthContext)` and `ConnectCommand.auth` are outside the brief's listed ownership. `ports.rs` is not on its forbidden list.
  - Necessity is demonstrated. Contract §7's port table names `AuthContext` or the captured context as an input of both the SSH identity/trust adapter and the HTTP credential adapter. The brief forbids putting the context in the profile or the instance. With the instance shared across contexts, handing the context over with each connect and each exchange is the only remaining route. `EndpointSpec` deliberately carries no context (`ports.rs:89-91`).
  - Provenance traces to the plan text ("move into the per-operation authentication context") and §7. I judge it authorized. The integration record it requires is P3-1.
- **Out-of-scope observations:**
  - The trunk lane's adapters implement the old port (P3-1).
  - `RouteKey` is keyed by operation name. I assessed the record of this, not a fix (P3-1).

## 1. Findings

### P2-1: Retrying after a lease of another scope submits a new, non-fresh pool request, so the opening loses its FIFO place and its allocation deadline, and the bound is unsound

- **Location:** `gwz-transport/crates/gwz-connection/src/instance.rs:350-372`. The cap is at `:354-357`; the re-request is `state.pool.request(pending.template.clone())` at `:362`. The template is captured at `:193`.
- **Violated invariants:**
  - Contract §8: "Fairness is FIFO among eligible requests".
  - Reuse design §5, as §15 retains it: allocation-deadline rules, under which a request's deadline is not extended.
  - The module's own claim (`instance.rs:14-15`): "each pass retires one of the key's connections, so more passes than the key can hold mean nothing will fit".
- **What the code does:**
  - `pool.request` assigns a new `RequestId`, which goes to the back of the FIFO in `allocation.rs:8-14`.
  - It also sets a fresh `now + allocation_timeout_ms` (`machine.rs:239-245`).
  - The re-request stays non-`fresh`, so the next other-scope connection that goes idle is leased to it again (`allocation.rs:16-47`).
  - The retired connection counts toward `user_host_total` while it closes (`limit.rs:153`). The room it frees therefore goes to an older waiter of the other scope (`open_for` runs in id order), which opens a replacement of that scope.
- **Reproduction (pool default `per_user_host = 32`, which `Instance::new` uses):**
  1. 32 scope-a exchanges are leased on one instance, and B (scope b, equal profile) opens. B waits.
  2. Each cycle runs as follows:
     - A new scope-a opening queues behind B.
     - One scope-a exchange finishes clean. The connection goes idle and is leased to B, the oldest waiter.
     - B misfits: Discarded, which is still counted while it closes. B re-requests and lands behind the new A waiter.
     - The close completes. The freed room goes to the A waiter, which opens a scope-a replacement.
  3. B gains one misfit per cycle. At 34 it fails with `OpenFailure::NoFittingConnection`. Before 1.5, B was served on the first idle connection.
  4. Each misfit also restarts B's 30 s allocation clock, so B can wait about 34 × 30 s before failing.
- **Test gap:** `an_opening_that_no_connection_ever_fits_…` only produces misfits through `rescope_for_test` on B's own connects, so this path is untested.
- **Impact:**
  - In a 1.2.0 session host, two Clients on one profile with different `GH_TOKEN` (SSH is partitioned too; see P3-2) would see these spurious typed failures or overlong waits. In the Client with the minority scope, a fetch with 32 or more member jobs makes this likely.
  - Every misfit also retires a healthy connection of the other Client.
  - Not reachable in a single-scope 1.1.0 CLI process.
- **Root cause:** local. Its architectural background is that scope is enforced after the lease, outside the pool's eligibility test.
- **Correction (bounded):**
  - Record the opening's absolute allocation deadline at `submit`.
  - On a misfit, re-request with `fresh: true` through `request_until(template, Some(original_deadline))`. A fresh request never takes an idle connection; it opens its own, which carries its own tag and therefore its own scope. That makes at most one misfit per opening, and the cap stays a pure guard.
  - Correct the comments at `instance.rs:14-15` and `operation.rs` (`NoFittingConnection`).
- **Closure test:** in the fakes, run the sequence above for more than `per_user_host + 1` cycles.
  - Assert that B ends with a fitting exchange (`reused() == false`) after at most one misfit.
  - Assert that B's wait fails at its original deadline, not a later one.
  - The existing bound test must still pass.

### P2-2: Dropping an opening after a misfit addresses a stale `RequestId`, which orphans its pool request and its `AuthContext`, starts an exchange after the drop, and lets `finish` report Complete

- **Location:** `instance.rs:235-240` (`abandon` matches `pending.request != request` and calls `pool.abandon(request)`), `instance.rs:367` (a misfit overwrites `pending.request`) and `exchange.rs:383` (`OpenWait::drop` passes the RequestId saved at submit).
- **Violated invariants:**
  - Contract §5: dropping the wait cancels the opening, and nothing is started for it afterwards.
  - Contract §7: a completed operation's snapshot is not retained.
  - Contract §8: "no fabricated Complete".
  - The method's own doc: "its request and its context leave the instance".
- **Reproduction:** with the existing fakes.
  1. `asynchronous = true`. Register `op` with `AuthContext::new(64, AuthScope::new([1;32]), provider.clone())`.
  2. Open a wait and poll it until Pending.
  3. Call `op.instance_for_test().rescope_for_test([9;32])`, then `complete_connects()` and `progress.run()`. This produces one misfit, and `pending.request` now holds the re-request's id.
  4. `drop(wait)`. Observe `Arc::strong_count(&provider) == 3`, where 2 is expected: the instance still holds the `Pending`. The pool still holds the live re-request.
  5. `now(op.finish())` yields `OperationEnd { cleanup: Complete }`. `retire_if_done` (`host.rs:609-636`) cancels no pool request.
  6. `complete_connects()` and `progress.run()`.
     - The re-request's connect runs with the dropped operation's context.
     - `start_exchange(…, &pending.auth)` runs, and `world.exchanges == 1`.
     - It is then aborted because the slot is abandoned.
- **Wrong evidence claim:** the README calls mutation M4 ("abandon keeps the opening") an equivalent mutant. That holds only when there has been no misfit.
- **Impact:**
  - A network effect occurs after cancellation. For an HTTPS adapter that sends credentials at exchange start, that means credentials are used for an opening the caller already dropped.
  - The full helper environment, `GH_TOKEN` included, outlives the operation until the request is served or reaches allocation timeout.
  - The completion receipt says Complete while the operation's work is still live.
  - Further misfits by the orphan retire other scopes' connections.
  - The window is bounded at about 30 s, or until the request is served.
- **Root cause:** one, local. The opening's identity in the instance is mutable, and the handle keeps a copy of it.
- **Correction (bounded):** `abandon` identifies the pending by its slot (`Arc::ptr_eq`), or by its tag, and calls `pool.abandon` with that pending's current `request`. `OpenWait` passes the slot alone.
- **Closure test:** the sequence above. After the drop, assert a strong count of 2, `world.exchanges == 0` after the further pumps, and no live pool request. Rerun M4, which must now be killed.

### P2-3: Windows profile bytes embed the `Debug` text of the machine proxy's bypass rules, so a bypass list well under its 8192-character grammar limit exceeds the 16 KiB bound and refuses every operation in the production legacy path

- **Location:**
  - `gwz-core/src/transport_host/endpoint_environment.rs:268-269`: field 12 is `format!("{machine:?}")`.
  - `:187-189`: `with_inputs` fails, mapped to "endpoint profile exceeds its bound".
  - `:101`: `endpoint_config_native` now runs `capture_operation`. Its callers are `local_command.rs:98`, `cancellable.rs:226`, and `endpoint_environment.rs:80` and `:88`.
  - `gwz-connection/src/operation.rs:93-96`: `MAX_PROFILE_BYTES = 16 KiB`.
- **Mechanism:** `MachineProxy::Named` → `Named { bypass: Arc<Bypass> }` → `Bypass { rules: Vec<Rule>, own: Vec<IpAddr> }`, all derived `Debug`.
  - `Rule::Host { pattern: Vec<u8> }` prints each byte as a decimal number followed by ", ". That is roughly a 6× expansion of the list text.
  - The bypass grammar itself accepts up to 8192 characters (`machine_proxy/bypass.rs:10,38`).
- **Violated invariants:**
  - Compatibility and parity. A machine configuration accepted at `2900809f` is refused at `aaf6efb4`.
  - Contract §7 asks for canonical nonsecret bytes. It does not ask for this build's `Debug` layout.
- **Reproduction (computed from the derived formats):** 120 bypass entries such as `*.svc001.corp.example`, a 2,639-character list, give 16,798 bytes of rule `Debug` alone, before the other fields. `capture_operation` then returns the error. Every SSH and HTTPS operation fails, because `endpoint_config_native` builds both configurations, and the profile it fails on is one production does not yet consume (`OperationCapture.profile` is `allow(dead_code)`).
- **Impact:** a Windows-only regression on corporate machines with a named proxy and a list of a few KB. No receipt would catch it: the Windows receipt was skipped, and no row uses a long list.
- **Root cause:** one, local. A variable-size field's encoding is unbounded relative to the bound.
- **Correction (bounded):**
  - Encode the machine setting canonically from its bounded inputs: access type, the proxy string (≤ 4096), the bypass list text normalized as the rules parse it (≤ 8192), and the own addresses as octets.
  - Size `MAX_PROFILE_BYTES`, or core's own check, from the sum of the field maxima, so that any capture the grammars accept fits.
  - Optionally, do not fail the legacy path on a profile it does not use.
- **Closure test:** a platform-neutral row, built with the `machine_proxy::testing` constructor, captures a `Named` setting with an 8192-character bypass list and asserts that `capture_operation` succeeds. It also asserts that two lists differing in one rule give unequal profiles.

### P3-1: The step's obligations on the trunk lane are not recorded where that lane will read them

There are two items with one root cause: the handoff record is missing.

- **Location:**
  - `gwz-connection/src/ports.rs:135-137,181-189` (the new port).
  - Evidence README: it never mentions the port change. Its line 62 is the only `RouteKey` note.
  - Contract §15's 1.3b row still states `start_exchange(connection, &ExchangeSpec)`.
- **What the trunk lane will hit:**
  - Its adapters were written for the old port: `ssh_adapter.rs:279`, `transport_host/connection_factory.rs:182`, the 1.3c working-tree `https_adapter.rs:264`, and `connection_host/tests.rs:182` (`AuthContext::new(64, Arc::new(()))`).
  - None of these compile against gwz-transport `origin/main` (`bd7cf4f6`). Its pin or sibling update will break, and its HTTPS adapter must take the helper from each exchange's context rather than its factory.
  - The note that "routes must be named uniquely per binding" exists only as an explanation of a reverted change. I verified that the record is accurate for today's endpoint: `https_endpoint.rs:324` creates `session::unique()` per request registration. But it is not stated as a requirement on 1.3c.
- **Consequence:** compile breaks are loud. The routing rule is not. A host-owned adapter that passes the caller-chosen operation name to `RouteKey::new` (`https_policy.rs:61`, `https_worker/prepare.rs:99`) would share pinned routes across bindings, and with them a redirect target to which another Client's credentials would then be sent.
- **Correction:** add the §15 row in §3 below, and record the 1.3c obligation in that row or in the plan's 1.3c text.
- **Closure:** both texts exist, and the 1.3c review checks them.

### P3-2: The auth scope applies to every scheme, so SSH connections are partitioned by the helper environment, which SSH never uses

- **Location:**
  - `instance.rs:343-349` (scope check for every lease).
  - `https_auth.rs:58-77`: the scope covers the whole environment.
  - `endpoint_environment_tests.rs`, `one_operation`: the R4 row asserts no reuse after a `GH_TOKEN` change on `Key::ssh(…)`.
- **Requirement provenance:** none found for this behaviour.
  - Plan R4 says "never reuses an *incompatible* connection".
  - The step text is about running another Client's *helper*.
  - SSH reuse authorization is contract §7.1's possession, key equality and known_hosts, which is lane 1.6's.
- **Consequence:** in 1.2.0 hosts, Clients whose environments differ in any byte cannot share SSH connections that §7.1/§4 exist to let them share. It also widens the exposure to P2-1. This is an implementation preference turned into a requirement.
- **Correction:** either ask the operator to ratify it as the conservative rule and record it in §15, or apply the scope only to HTTPS keys and leave SSH to 1.6.
- **Closure:** the ruling is recorded. If SSH is narrowed, add a row where SSH with a changed `GH_TOKEN` reuses.

## 2. Invariant analysis (attacks that failed, with evidence)

- **No secret in the profile or its inputs.** Attack failed.
  - `EndpointInputs { local: EndpointSettings { ssh, pool, io_timeout_ms }, tls }` (`transport_host/mod.rs:206-217`) holds no environment or helper.
  - `Proxy.authorization` is always `None`, because proxies with URL credentials are refused (`endpoint_environment.rs:441-468`, `machine_proxy.rs:111`).
  - The canonical bytes leave authorization out.
- **Every input field is in the canonical bytes.** Attack failed.
  - `ca_roots` is covered by its content SHA-256.
  - `no_proxy` is normalized to lower case, sorted and deduplicated. Matching is case-insensitive (`https_connection.rs:177-184`), so the normalization is sound.
  - Home, home source, agent, pool, timeout, the machine setting, and `SSL_CERT_FILE`/`SSL_CERT_DIR` are present. Their content is compared by path only, which is deferred finding 2.
  - So the first operation's inputs are safe to share across equal profiles.
- **Byte equality after the digest.** Holds.
  - `PartialEq` is `same_as`: digest, then canonical bytes, and inputs are ignored (`operation.rs:122-133`).
  - The registry uses `same_as` (`host.rs:408`), which is unchanged.
  - The collision row and mutation M3 cover it.
- **Silent `Debug`.** Holds. `EndpointProfile`, `AuthScope` and `AuthContext` all print fixed text. `OperationAuth` has no `Debug`.
- **`provider: Arc<dyn Any>`** predates this step (1.1). It matches §7's "opaque … provider-owned bounded object", so no explicit type is hidden.
- **Can a caller reach another operation's context?** Attack failed.
  - No public accessor goes from `Operation` or `Exchange` to the instance's contexts.
  - Tags are unique per host (`x{binding serial}-{n}`, `exchange.rs:93`; serials are never reissued, `binding.rs:23`).
  - The pool returns the tag only for the request that opened the connection (`lifecycle.rs:291-317`), and a connected connection goes to that request (`allocation.rs:280-292`).
  - So a connect never carries another operation's context, and an exchange always gets its own pending's context (`instance.rs:378-380`; mutations M2 and M5 killed).
  - The only leak found is P2-2.
- **The 1.3b reconciliation.** Holds.
  - `pump.rs` is untouched in the range.
  - `poll_finish`, `IdleClosed` handling and `ExchangeSpec::git` are intact.
  - The 58 tests are 1.3b's 51 plus 7 new rows, and all pass.
  - No new `allow(dead_code)` in the crate. Its two test helpers are `cfg_if!` `cfg(test)` code in `src/`.
- **gwz-core `allow(dead_code)`.** One, on `OperationCapture.profile`, with a reason; its production consumer is 1.3c. That is acceptable under the release-scoped rule. See P2-3 for its side effect.
- **R4 interpretation.** Treating `GIT_CONFIG_GLOBAL` and `GH_TOKEN` as context, sharing the instance with `reused: false` and then resuming, agrees with the plan text ("served … only on an equal profile … helper environment … into the per-operation authentication context") and with §7 ("helper environment, account tokens … are not profile fields").
  - The capture reads proxy and CA only from the environment or the machine, never from Git configuration, so `GIT_CONFIG_GLOBAL` cannot change profile fields.
  - "No stale event": completions are routed by `ConnectionId`. A retired connection's `Closed` and `IdleClosed` only update the pool, and `connections` entries are removed (`instance.rs:150-173`). No path delivers an old connection's event to a new operation.
  - The README restates this as "no stale context". The crate has no observation surface yet; record that.
- **Proxy row on Unix.** Through the host, the row uses `MachineProxy::Refused`, not `HTTPS_PROXY`. The environment proxy is covered by the encoder row together with the existing `tls_config` path. Adequate.
- **The legacy runtime.** Its helper comes from the context's `OperationAuth` clone. That is equivalent to the old `Some(Config)`, and the clone is per operation.
- **Platform and style rules.** No new `cfg(windows)`. `cfg_if!` is used only for `cfg(test)`. All control flow is braced. Only `const` items are added: no globals and no thread-locals. No production code under `tests/`. File sizes: `endpoint_environment_tests.rs` 932, `endpoint_environment.rs` 509, `fakes.rs` 688, `exchange.rs` 499, `instance.rs` 437; none needs a split.
- **Deferred finding 1** (retiring idle connections of another scope instead of keeping them). Once P2-1 is fixed, this is a recorded reuse limitation, not a §7/§8 defect.
  - §8 forbids closing *active* exchanges, not ineligible idle ones.
  - §7.1 already retires connections on a failed proof.
  - Its harmful interaction is P2-1, which is a separate root cause: the re-request and its bound.

## 3. Risks and next action

- **Next action:** close P2-1 to P2-3 with their closure tests (the crate for P2-1 and P2-2, gwz-core for P2-3). Rerun the M4 mutation and the crate and core focused suites. On the Code axis, re-review only those hunks.
- **Residual risks:**
  - The Windows receipt was skipped. The new rows' Windows path (`AgentSource::Refused`, the `Named` machine setting) has no Windows run.
  - The README's tuple should record the committed `aaf6efb4`/`bd7cf4f6` mapping and the two-commit range.
- **Text the lane owner should add to contract §15**, as a new row:

> | §7 (`AuthContext`, `EndpointProfile`); §4.1 (`EndpointAdapter` ports); §8 (lease) | Recorded at step 1.5 (gwz-transport `bd7cf4f6`, gwz-core `aaf6efb4`):
> - `AuthContext::new(charge, AuthScope, provider)`. `AuthScope` is an opaque `[u8; 32]` with a silent `Debug`.
> - The operation's binding holds the context. An endpoint instance lends a waiting opening's context to the connect the pool starts for that opening's tagged request, and keeps only each connection's setup scope.
> - `EndpointProfile::with_inputs(canonical, digest, inputs)`: equality is the digest, then the canonical bytes; inputs are not compared. Every input must be represented in the canonical bytes, which are bounded by the sum of the field maxima.
> - Adapter ports: `ConnectCommand.auth: Option<AuthContext>` and `start_exchange(connection, &ExchangeSpec, &AuthContext)`, superseding the 1.3b row's two-argument form.
> - Lease rule: a connection serves only an operation whose scope equals its setup scope. A lease of another scope is retired (Discarded), and the opening asks once more for a fresh connection under its original allocation deadline. Past the bound it fails with `OpenFailure::NoFittingConnection`.
> - `Exchange::reused()` reports whether the connection served an earlier exchange.
> - Core: the helper's executable and environment, and with them every token, live only in `https_auth::OperationAuth`. Its scope is the SHA-256 of a domain tag, the executable and the sorted environment. They are never in the profile's bytes or its inputs.
> - Open:
>   - Whether the scope applies to SSH (operator ruling, P3-2).
>   - The trust store is compared by path, not content.
>   - The host-owned HTTPS adapter (1.3c) must name routes uniquely per binding or request, never by the caller's operation name.
>   - The crate has no observation surface for R4's "no stale event". |
