# p1c-https (1.3c/C2(b) per-exchange HTTPS auth) — Code-AXIS REVIEW
**Review object:** gwz-core dabe7dc6 (+ evidence 9e9db70a), 2026-10-31
**Baseline:** lane root 5bc56837; gwz-core object dabe7dc6 (parent 34cdf51a), lane HEAD de03168e; gwz-core-evidence 9e9db70a, lane HEAD 85bb605c (a merge whose diff from 9e9db70a does not touch `runs/2026-10-31-p1c-https/`); gwz-transport/gwz-connection read-only at lane HEAD. I read the sources with `git -C gwz-core diff 34cdf51a dabe7dc6` and by reading the files at lane HEAD. `git diff dabe7dc6 de03168e` is empty for `src/git/endpoint`, `src/transport_host/connection_host` and `src/transport_host/mod.rs`, so the object holds unchanged on the merge. The tuple was verified at start and end and did not move.
**Date:** 2026-10-31
**Axis:** Code (architecture, interfaces, call graphs, compatibility). Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: NO-GO** — 0 P0, 0 P1, 1 P2, 1 P3. I pre-commit to GO on a revision that resolves P2-1 as specified.

## 0. Evidence base

- **Diff read in full** (9 files, +521/-75): `https_adapter.rs`, `https_adapter/{auth,exchange,pipe,worker}.rs`, `https_worker/reuse_tests.rs`, `connection_host/{https_auth_tests,https_tests}.rs`, `transport_host/mod.rs`.
- **Call graph traced** at lane HEAD:
  - Host side in gwz-connection: `instance.rs` `openings` → `fits` scope check → `last != pending.binding` → `authorize_reuse` → `serve` → `start_exchange(…, &pending.auth)`, and `ConnectCommand.auth`. The `ports.rs` contracts were read too.
  - Adapter side in gwz-core: `HttpsAdapter::start_exchange` → `pipe::exchange` → `Command::Start` → `Worker::start` (`prior`) → `exchange::run` → `exchange` → `auth::answer` / `auth::lookup` → `https_auth::lookup_until`, which caches nothing.
  - Production: `endpoint_environment::capture_operation` / `endpoint_config_native`, `cancellable::Owned::build`, `local_command::build_runtime`, and `session_host/served/dispatch.rs`.
- **Construction sites, by grep:**
  - `HttpsInputs {`: only `transport_host/mod.rs:580` and `https_tests.rs:58`.
  - `Common {`: only `worker.rs:132`.
  - `Work {`: only `pipe.rs:125`, reached only from `start_exchange`.
  - None in gwz-py or gwz-cli.
  - The Windows-only `https_sspi_tests.rs` uses the unchanged `Case::with` signature.
- **Tests on the candidate shape** (my own copy tree, `rsync -aL` excluding targets; `TMPDIR=/Volumes/gwztmp`, `RUSTFLAGS='--cfg gwz_transport_candidate'`, `GWZ_TEST_GIT=real`, `cargo +1.95.0 test --locked --lib`):
  - `connection_host::https`: 15 passed, including both R4 rows (GH_TOKEN and GIT_CONFIG_GLOBAL), the Denied row, the retention row, the push row and the no-helper row.
  - `reuse_tests`: 2 passed. `https_adapter`: 1 passed.
- **My mutation, instance-level auth put back:** `Common` gained a `first: Mutex<Option<Config>>`, and every exchange used the first operation's helper. Four rows went red:
  - `a_changed_gh_token…` ("B's request did not carry B's own credential");
  - `a_changed_git_config_global_helper…`;
  - `an_operation_whose_context_holds_no_helper…`;
  - `an_authenticated_idle_connection_is_reused_only_by_an_equal_context…`.

  I reverted it.
- **My probe for P2-1:** a row appended to the copy tree's `https_tests.rs`. It is red on the object and green with the POST pre-authentication predicate disabled (control). Details are in P2-1.
- **Checks:** `check_windows_parity.py`, `check_cfg_boundaries.py` and `check_process_globals.py` each report "nothing new".
- **Evidence:**
  - All 20 digests in `manifest.json` match.
  - No log in `raw/` contains a fixture token or a `Basic` credential (grep, by file name only).
  - `red-2.log` and the five `red-*-mutation.log` files say what the README claims.
- **Clean-up:** the copy tree and the target dir were deleted. One plain run log is kept: `/Volumes/projects/limbo/build-targets/p1c-https-review-code-run1.log` (test names and results only, no credential).

## Problem, requirement provenance, and scope

- **Observed problem.** It is recorded in contract §15's 1.3c row and in `history/GwzPhase1Integration-ReviewState-2.md`. `start_exchange` ignored its `AuthContext`, and challenges were answered with `HttpsInputs::auth`, the instance's helper. R4 (plan §3, "a helper never runs with another operation's environment") was therefore unmet on the real adapter.
- **Authority.**
  - The operator's 2026-10-31 go for step 1.8.
  - The plan's "1.8 preparation" note ("p1c-https closes C2(b)") and its "Carried to 1.8 by name" list.
  - Brief items 1–4. Brief item 3 explicitly names "a pre-authenticated POST on an already-authenticated connection" as an option, to be implemented if small.

  The deliverables trace to these sources.
- **Remedy versus problem.**
  - The helper now comes only from `work.auth`, through `OperationAuth::of`.
  - `HttpsInputs` and `Common` hold no helper.
  - My mutation shows the rows detect a return to instance-level auth.
  - This addresses the observed problem directly, with a localized change.
- **Component authorization.** The one-line removal of `auth: https.auth.clone()` in `transport_host/mod.rs::https_factory_inputs` is outside the owned list. It is demonstrably necessary: that is the only production constructor of `HttpsInputs`, and the removed field would not compile. `HttpsEndpointConfig.auth` itself stays, because the legacy session and `ssh_password_helpers` still use it. The edit is authorized by necessity.
- **Scope drift.** The POST pre-authentication changes behaviour beyond what the brief asked for. It changes how an anonymous POST is sent on any connection a helper token ever authenticated, not only on the advertisement's own connection. That expansion is P2-1.

## 1. Findings

### P2-1 — POST pre-authentication keys on a sticky, connection-wide "ever helper-authenticated" record, so an anonymous POST to another repository on the same connection now fails

**Root cause.** In `src/git/endpoint/https_adapter/exchange.rs:206-220`, the predicate is `!advertisement(service) && work.prior == Binding::HelperToken`. `work.prior` is set at `worker.rs:258-262` from `Scopes`. `Scopes::bound` (`https_adapter.rs:146-156`) never clears a per-exchange binding ("a later anonymous exchange does not clear it").

So `prior == HelperToken` means "some earlier exchange of this binding, on any repository path of this origin, was helper-authenticated". The code comment claims "its server is known to ask for one", but the record does not establish that for this POST's path. A pool key is the origin, never the repository (contract §7.1), and one binding serves many repositories of one origin. Every POST on such a connection therefore runs a helper lookup first and fails if the lookup fails.

**Violated invariant.**
- Contract §7.2 preserves "anonymous and configured helper paths where allowed": a request the server serves anonymously must not be made to depend on a credential.
- The change is meant to add a capability (an authenticated push), not to remove the anonymous route.
- Before dabe7dc6, this POST was sent anonymously and succeeded. Git does the same, since it would never carry repository X's authentication state into repository Y.

**Reproduction** (run, candidate shape, macOS):
1. A fixture server answers 401 (Basic) to `/private` requests without Authorization, and 200 to `/public` regardless.
2. The fake-git helper declines (`exit 1`) when its `url=` request contains `public`, and otherwise answers a synthetic credential. This is what `credential.useHttpPath` with a per-path store gives.
3. One operation (one binding) runs three exchanges:
   - `GET /private` advertisement: challenged, answered, `Reusable`. Connection C is now `HelperToken`.
   - `GET /public` advertisement on C: anonymous 200.
   - `POST /public/git-upload-pack` on C (`case.accepted() == 1`, so it is the same connection).
4. Result on dabe7dc6: the client reads `PermissionDenied` (`ErrorCode::Authentication`), and the server receives 0 POSTs for `/public`.
5. Control: the same row with the predicate disabled (`&& false`) passes, and the POST reaches the server with no credential.

**Impact.**
- Once Phase 3 routes production through the host, a workspace with an anonymously readable member and a private member on one origin can fail the public member's fetch. This happens whenever the helper declines, errors, times out (`Timeout`, `GateTimeout`, `AllocationTimeout`) or is unavailable for that path.
- It also adds a helper spawn to every such POST, and can prompt interactively (for example, Git Credential Manager on an unseen path) for a request that needs no credential.
- No credential crosses a binding: the host's lessee check plus `authorize_reuse` keep a `HelperToken` connection with its authenticating binding. So this is a correctness and compatibility defect, not exposure.
- Production does not use the host before Phase 3, so the impact is latent but sits inside the deliverable.

**Required correction.** Pre-authenticate a POST only when this connection's helper authentication was for the same destination:
- Record in the connection's scope record (adapter-local, never a secret) which repository path the helper-token exchange was for: the path, or a digest of the path.
- Set `prior`, or a separate `authenticated_path`, from it.
- Pre-authenticate only when the POST's `work.path` equals it. Otherwise send the POST anonymously, exactly as before dabe7dc6; a 401 then stays the deferred, typed refusal.

Optionally, also require the same service family. That is not needed for git-http-backend-style servers, which accept a credential on either service.

**Closure / regression test.** The probe above, as a standing row in `connection_host/https_auth_tests.rs`: the `/public` POST succeeds, reaches the server once, and carries no Authorization. Plus `a_push_is_authenticated_on_the_connection_its_advertisement_was`, which must stay green.

### P3-1 — The deferred failure mode (a POST on a connection its advertisement did not authenticate) has no standing row on the host adapter

**Location.** `connection_host/https_auth_tests.rs`, where the row is missing. The behaviour lives at `exchange.rs:235-252`, with `classify(401, non-advertisement)` → `Fail(Authentication)`.

**Invariant.** Contract §7.2 "challenged POST refusal" and §9 "no replay". The deferral the drafter calls a Phase 3 prerequisite rests on this failure mode staying safe: a typed `Authentication`, the POST sent once, no credential attached, the connection discarded.

**State sequence.**
- A binding's POST lands on an anonymous idle connection: a fresh one, its own earlier anonymous one, or another binding's equal-scope anonymous one that `authorize_reuse` passes at once.
- The server answers 401.
- Today that ends `Authentication` with no replay. The red push-mutation log shows this, but only transiently.
- No green row pins it. Phase 3's planned work on this exact path (answering or avoiding the POST challenge) could change it to a replay, or to a credential sent on an unrelated connection, without a red row.

**Impact.** It is a coverage gap on a secret-handling path that a recorded deferral depends on. There is no current defect.

**Correction.** Add a row in which the push POST is forced onto an anonymous connection (for example, a POST with no prior advertisement on that connection). Assert:
- `PermissionDenied`;
- `Disposition::Discarded`;
- exactly one POST recorded, with digest `None`.

**Closure.** That row green, and red under a mutation that answers a POST's 401.

## 2. Invariant analysis

- **The exchange takes its operation's own `AuthContext`.**
  - `Work.auth` is cloned from the `start_exchange` argument (`pipe.rs:130`).
  - `Work` has one construction site, and the adapter has no redirect, retry or reconnect path inside it: a redirect gives `UnsupportedOperation`, and a request that never started gives `Io` and no replay.
  - The helper used by `answer` (advertisement 401) and by the POST lookup derives only from `work.auth`.
  - `Common` and `HttpsInputs` hold no helper.
  - A context of another provider yields `None`, and the challenge is refused (row 6, plus my mutation).
  - `ConnectCommand.auth` is dropped at the end of `Worker::connect` and is never used for an exchange.

  No route remains where an instance's or a previous operation's helper answers. **Holds.**
- **The native (logon-session) route** is still the instance's `NativeCaller`. The brief scoped that route as "keep behaviour", so it is not judged here (see §4).
- **Scope combination.**
  - Operation scope: `Identity::HttpsScoped` partitions the idle match. The `fits` check at lease (`instance.rs`) is unchanged.
  - Per-connection scope: the `HelperToken` or `LogonSession` record makes `authorize_reuse` return `false`, which is `Unprovable`, for any other binding.
  - At lease time both apply, in order: scope fit first, then the cross-binding proof.
  - The retention row shows the AND: an equal context of another binding gets a new connection (the scope passes, `authorize_reuse` refuses), and the `authorize_reuse`-always-true mutation turns it red. A different context gets a new connection through the partition.

  **Holds.**
- **`command.identity` echo.** It is unchanged by the diff: `connector.start(&command.key, &command.identity, …)`, and `Connected` carries what `poll_connected` returns. The evidence's echo mutation is red. **Holds.**
- **POST `prior` is the same binding's.** A `HelperToken` connection is leased only to its last lessee, because `authorize_reuse` refuses per-exchange connections, and the host sets the lessee before each exchange. A non-`OperationAuth` context fails before sending anything (`helper.ok_or(Authentication)`). So the pre-authentication can never put a credential on another binding's connection.
- **POST pre-authentication on anonymous connections.** It never adds a credential to an anonymous connection (`prior == Anonymous` skips it). But it is too broad within one binding (P2-1).
- **Fresh lookup.** `auth::lookup` → `lookup_until` spawns the helper each time; there is no cache in `https_auth`. The `Secret` and its header are dropped right after the request is built, and the `HeaderValue` is marked sensitive. **Holds.**
- **Legacy `reuse_tests` deletion.** It is correct and hides no open defect.
  - The legacy worker has one helper per endpoint (`Client::auth`, from `HttpsEndpointConfig.auth`).
  - The only production builders of a runtime (`cancellable::Owned::build`, `local_command::build_runtime`) build one runtime per request, from one snapshot.
  - The session host's `configure_transport_runtime` only sets timeouts.
  - So two bindings with different helpers never reach one legacy route in production.
  - The kept assertion `a_auth == b_auth` characterizes the per-operation cache and compares digests.
  - The per-binding rule is now proven on the host adapter (both R4 rows, plus my mutation).
  - Keying by `Input.session` was tried and abandoned for a stated reason (`raw/abandoned/`).
- **Compatibility.** `HttpsInputs.auth` and `Common.auth` had no other setters (see the grep in §0). The Windows-only test file compiles against an unchanged signature.
- **Repository rules.**
  - All new control flow is braced.
  - No new `#[cfg]` outside `cfg_if!`.
  - No global or thread-local.
  - Test code stays under `src/…/*tests*.rs` with `#[cfg(test)]` module gating.
  - Parity, cfg and globals checks report nothing new.
- **Secret handling in the rows.** Assertions compare SHA-256 digests. Failure messages print no header. The evidence logs hold no token.

## 3. Risks and next action

- **Revision for P2-1:** bind the pre-authentication to the repository path that was authenticated, add the probe as a row, and re-run `connection_host::https`, `reuse_tests` and `https_adapter` plus the Windows receipt. P3-1's row can ride in the same revision.
- **Lane owner's clerical work at merge (not graded):**
  - The §15 1.3c row still states "HTTPS routes must be named uniquely per binding, never by the caller's operation name or `Input.session`" and cites `reuse_tests.rs:141`/`:149`. That obligation needs to be amended to the finding above (a legacy per-operation characterization, with the per-binding rule now held by the host adapter). Otherwise the controlling document keeps an obligation the code has declared non-applicable.
  - The same row (and the 1.5 row's "how §7.2's per-connection scope combines with the operation scope … belongs to C2(b)") should state the AND as implemented: `HttpsScoped` partition, then `authorize_reuse`'s per-exchange refusal. The pool's `scope_https` stays unused.
  - The POST-challenge deferral should be named as a Phase 3.1 prerequisite in the plan.
- **Windows receipt** `p1c-https-20261031-a` should cover every `https_auth_tests` row, notably the `GIT_CONFIG_GLOBAL` row (real Git plus `SystemRoot`) and the refusing-helper row, which uses `sh.exe`. I ran Mac only, and cite no Linux or Windows result.

## 4. Out-of-scope observations

- **Native caller is per instance (hypothesis, not demonstrated reachable).** `NativeCaller` is captured per operation on the original entry (`NativeHost::caller` → `capture_caller`, `native/sspi.rs:68`, `:121`), but the host adapter holds one per instance (`HttpsInputs.native`). Today each runtime has its own host, so the instance is per operation. If Phase 3 shares a host across operations from different caller identities with equal profiles, a Negotiate/NTLM challenge of one operation would be answered with another's captured caller. That is the native-route analogue of C2(b). Suggest that Phase 3's design state whether the caller belongs in the `AuthContext` or in the profile. It does not block this deliverable: the brief kept the native path unchanged.
- **Helper spawn cost (optimization opportunity, not a defect).** Every advertisement on a helper-token connection still goes out anonymously, takes a 401, and then spawns the helper. Every pre-authenticated POST spawns the helper again, so multi-round protocol-v0 negotiation spawns one helper per round. Worth measuring in the Phase 3 sweep.
