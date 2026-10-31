# Step 1.7 driver-only construction: STATE-AXIS REVIEW
**Review object:** gwz-core `b0ac8de7..edfc623e` (plan step 1.7, the driver-only construction). It covers `DriverOnlyBuilder`/`DriverOnly` in `src/transport_host/driver_only.rs` and `driver_only_tests.rs`, the named route refusals in `src/git/gitbackend/transport_support/identity.rs` and `transport.rs`, and `scripts/checks/check_driver_only_boundary.py` with its test. **Baseline:** lane `/Volumes/projects/limbo/gwz-dev-p1-7`, gwz-core `edfc623e`, gwz-transport `96938616` (reference only), root `34251a9c`. **Date:** 2026-10-30
**Axis:** State: durable-state semantics and adversity. Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: NO-GO.** 0 P0, 1 P1, 1 P2, 1 P3. I pre-commit to GO on a revision that resolves P1-1 and P2-1 as specified.

## 0. Evidence base
- **Tuple.** Checked at the start and again at the end (18:32 AEDT): root `34251a9c`, gwz-core `edfc623e`, gwz-transport `96938616`. The gwz-core worktree was clean at both checks, and the tuple did not move.
- **Read:**
  - the brief `build-targets/tools/brief-1.7-driver-only.md` and the drafter's report `review-1.7/drafter-report.md`;
  - the plan text: root plan §4 step 1.7 and 2.3, and split design §4.3, §4.5 and §4.6;
  - the full diff `b0ac8de7..edfc623e`;
  - the route-binding path: `transport_binding.rs::configure`, `ssh_destination.rs::parse`/`local`, `transport_support.rs` (callbacks and credentials), and `transport.rs` (every remote operation);
  - the identity cache in `identity.rs::for_remote`;
  - session close and begin (`session/close.rs`, `session/requests.rs`, `session.rs::driver`);
  - the callers of the clone-family ports (`local_clone/transport.rs`, `family_merge.rs`, `transport_scope.rs`, gwz-cli and gwz-py dispatch);
  - in libgit2 1.9.7: `remote.c` (`apply_insteadof`), `transport.c` (`transport_find_fn`) and `net.c` (`git_net_url_parse_scp`);
  - in git2-rs: `RemoteCallbacks::smart_transport`, which sets the forced `transport` callback.
- **Runs:**
  - Lane focused rows: `TMPDIR=/Volumes/gwztmp RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real CARGO_TARGET_DIR=…/p1-7-review-state cargo +1.95.0 test --lib --locked -- driver_only route_tests` gave **17 passed**, exit 0.
  - `python3.13 -B -m unittest test_check_driver_only_boundary` gave 13 OK. `check_driver_only_boundary.py` and `check_spawn_gate_minting.py` both pass.
  - Drafter's mutation log (`p1-7-logs/mutation/run.out`): M1–M6 killed, **M7 (`read-before-route`) survived**, exit 0.
- **Probes.** I copied gwz-core with `cp -RL` to `…/scratchpad/probe17/gwz-core`, symlinked its siblings to the lane, appended three tests to `driver_only_tests.rs` using the lane's own `Harness`, and ran them with the same target dir and flags plus `-- probe_ --nocapture --test-threads=1`. Results, quoted in §1:
  - `probe_a_insteadof_rewrite_after_the_route_check`
  - `probe_b_backslash_scp_takes_the_native_route`
  - `probe_b2_control_without_backslash`
- **Not run:** the ordinary-shape build (the drafter's `check-ordinary.log` exists; `transport_host` is candidate-only in `lib.rs:47-49`), clippy, the full suite, the Windows receipt and the WSL matrix (queued, out of scope). No network was used and no `SSH_AUTH_SOCK` was touched; the probe's SSH peer is a loopback listener that drops the connection before authentication.

## Problem, requirement provenance, and scope
- **Requested outcome** (plan 1.7, operator go 2026-10-30, brief items 1–3): a core-side host whose every request is `cli`.
  - Local-only routes (`file`, clone-family, `http://`, `git://`) are refused "before any effect".
  - The native libgit2 route is refused "with zero agent or key access".
  - The drafter's proposed §15 row adds: "Core never falls back to libgit2's native transport under cli."
- **Changes to the shared backend** (`identity.rs`, `transport.rs`):
  - (a) The cli refusal in `resolve_remote` is rewritten as `local_only_route` with per-route names. I compared the old and new predicates term by term. The admitted set is identical (ssh/git+ssh/ssh+git/https schemes plus `scp_like`), so only the message changed, and only under `is_cli_context()`.
  - (b) `refuse_clone_family_under_cli` fires only under a cli host context. In-scope operations never reach `fetch_anonymous`/`push_anonymous`: their only caller is `family_merge`, and `merge` is not in `transport_scope::Operation`, so it runs on an own backend with no host context. No existing cli or local caller changes behaviour.
  - (c) The reorder in `read_remote_file` affects every placement, but only removes an effect: no scratch tempdir is made before an identity or route error. The old message text is relied on nowhere in gwz-core, gwz-py, gwz-cli, gwz-transport or dev-docs (grep).
  - **Conclusion:** all three changes are authorized by plan 1.7 and §4.6 "Routes", and are confined to the cli placement or strictly reduce effects.
- **Drafter decisions, judged:**
  - (a) No `HostHandle`: acceptable. The brief explicitly allows "records that 2.2c refuses it", and it adds no state.
  - (b) A source check instead of trybuild: acceptable for state. `DriverOnly`/`State` fields are private to the module, so an inherent impl elsewhere cannot assemble a host. The scan is token-based (for example, a `Session::driver` fn-pointer alias would evade `DRIVER_CALL`); that is a Code-axis matter.
  - (c) No OS-level recording double: **not sufficient**. P1-1 shows a native libssh2 session started from the core, which neither the source check nor canary-free messages can see.
  - (d) Explicit timeouts: acceptable. They remove a read of process-global state.
  - (e) `Some(Local)`/`None` refused rather than rewritten: fail-closed and correct (rewriting would break `RequestContext::validate` equality). 2.3 can stamp `cli` at the server edge.
- **Scope note on P1-1 and P2-1.** Both root causes predate `edfc623e`: the classifier and `ls_remote_url` are unchanged. They are in scope because step 1.7's deliverable and its proposed §15 row assert the guarantee they falsify, and because the review brief names URL rewriting and URL classification as attack areas. Fixing them does not require leaving the brief's file ownership: `identity.rs`, `transport.rs` and the route-refusal site are the step's own.

## 1. Findings

### P1-1: The route check and the route binding classify SCP-like URLs differently. A backslash before the first top-level `:` passes the cli route check as SSH and is then served by libgit2's native SSH transport from the core
- **Location:**
  - `identity.rs` `scp_like` (`:360-368`) and `local_only_route` (`:414ff`): a URL with no `://` is admitted when the text before the first `:` has more than one character and no `/`.
  - `ssh_destination.rs::local` (`:147-166`) is bracket-aware and treats a `\` (or `/`) before the first top-level `:` as a **local path**, so `Destination::parse` returns `Ok(None)`.
  - `transport_binding.rs::configure` (`:283-292`) treats `Ok(None)` as "neither SSH nor HTTPS" and returns without installing the forced transport: "libgit2's native route, … on every platform".
  - libgit2 `transport_find_fn` then selects its own SSH transport for any URL containing `:` (Windows: after a directory test).
- **Violated invariant:** plan 1.7 says "the native libgit2 route is refused … with zero agent or key access". The proposed §15 row says "Core never falls back to libgit2's native transport under cli". The brief requires refusal "before any effect" for every route the client's endpoint does not serve.
- **Reproduction** (probe B, through the lane's own `Harness`, `DriverOnly` and a cli request):
  1. Set the receiver's `origin` to `[git\x@127.0.0.1:<port>]:source.git` and call `backend.ls_remote(&receiver, "origin")`.
  2. Output: `PROBE-B listener accepted a connection from 127.0.0.1:55020`, then `ls_remote = Err(… "failed to start SSH session: Failed getting banner")`, which is libssh2's native error. `native connect from the core: true`.
  3. Control B2 (the same URL without `\`) goes to the client endpoint instead: `"ssh setup timeout: no progress for 3 seconds (--ssh-timeout 3; attempt 4 of 4)"`.
  4. A realistic spelling exercises the same path without brackets: `CORP\user@githost:repo.git` (a Windows domain user in SCP form). On Windows, `.\x:y` and `\x:y` also match: `Path::is_absolute` is false for them, `scp_like` admits them, and `local()` marks them local.
- **Impact:** the served core opens its own socket and runs libssh2 for fetch, ls-remote and **push** (`perform_push` uses the same `for_remote` → `remote_push_options(&plan.url)` path). Authentication then goes through `credential_callback` on the core:
  - with no selected identity, `remote_credential` → `Cred::ssh_key_from_agent` reads the core's agent;
  - with a selected identity, which under cli is an opaque client path, `explicit_credential` → `Cred::ssh_key(.., &identity.path)` makes libssh2 read that path **on the core's filesystem**.

  This is the agent and key access that 1.7 and §4.3 ("no endpoint, no agent") exist to exclude, and on push it is a mutating remote effect made with the wrong side's credentials. It is not P0 only because I did not complete an authenticated exchange: the probe drops the connection before authentication, by design.
- **Required correction:** make the cli route decision fail closed on any disagreement with the binding. Both of these are needed:
  - (i) In `resolve_remote` under cli, refuse when `Destination::parse(url)` is `Ok(None)` (or `Err`) for a URL that `local_only_route` admits as SCP-like. Simplest: build the cli check on `Destination::parse` plus `is_https_remote`, the same predicates `configure` binds on, so there is one classifier.
  - (ii) In `configure`, in a cli host context, never return with the native route left in place. The `Ok(None)` and the Windows `serves_http_natively` branches should install a refusing transport, or record an error the operation surfaces, instead of `return`. This also closes drafter finding 2 as defence in depth.
- **Closure tests:**
  - A route-table row asserting, for each of `a\b:c`, `CORP\user@host:r.git`, `[u\x@127.0.0.1:1]:r` and (Windows) `.\x:y`, that the cli check refuses whenever `Destination::parse` is not `Ok(Some)` and the URL is not https.
  - A harness row like probe B (a loopback listener that records accepts, for `ls_remote`, `fetch` and `push`) asserting a named refusal and zero accepts. It must run in the Windows receipt.
  - A source or unit check that `configure` has no early `return` in cli context that leaves `callbacks.transport` unset.

### P2-1: `ls_remote_url` checks the caller's URL but binds and connects on the URL after `url.<base>.insteadOf`, so a rewrite to `file://` (or `http://`, `git://`) is served natively after the check passed
- **Location:**
  - `transport.rs::ls_remote_url` (`:178-188`): `repo.remote_anonymous(url)` applies insteadOf (libgit2 `remote.c:253-255`), but `identity::for_remote(.., url)` checks the raw `url`.
  - `advertised_refs` (`:200-210`) then passes `remote_handle.url()`, the rewritten URL, to `remote_callbacks` → `configure`. For `file://`/`http://`/`git://`, `configure` returns early (P1-1's branch) and libgit2 serves the rewritten URL natively.
  - The other operations are consistent:
    - `fetch`/`tag_fetch`/`ls_remote` check `remote_handle.url()` (already rewritten);
    - `clone`/`read_remote_file`/`push` check and bind the same raw URL, and libgit2's later rewrite lands in the forced transport, so they fail closed. Probe A confirmed `fetch` and `ls_remote` refuse.
- **Violated invariant:** the same as P1-1. A refused route is served after the check, with an effect.
- **Reproduction** (probe A):
  1. In the receiver, set `git config url.<file:///…/source>.insteadOf https://example.invalid/source` and call `backend.ls_remote_url(&receiver, "https://example.invalid/source", "origin", None)` under a `DriverOnly` cli request.
  2. Output: `Ok([GitRemoteRef { name: "HEAD", target: "ffcbefb4…" }, { "refs/heads/main", "ffcbefb4…" }])`, the source repository's refs read by libgit2's local transport on the core.
  3. In the same run, `fetch` and `ls_remote` refuse with the `file://` route name.
- **Impact:** every push preflight reaches this path (`publication.rs:61/251/324/739`), and so do push prepare (`push_plan.rs:33/80`) and the tag ops (`handle_tag.rs:169`). A rule in the core's member config or the core user's `~/.gitconfig` makes the served core:
  - read other local repositories' advertisements (`file://`);
  - or open native `http://`/`git://` connections from the core. On native HTTP, `credential_callback` reaches `Cred::default()`, which is the logon session's credentials on Windows, or the configured helpers when the policy allows them.

  This is read-only (an advertisement), hence P2 rather than P1.
- **Required correction:** run the route check on the URL that will actually be bound. In `ls_remote_url`, call `for_remote` on `remote.url()` after `remote_anonymous` (and on `pushurl()` where a push direction applies), or refuse in cli context when the rewritten URL differs in route from the checked one. P1-1(ii) closes the native fallthrough generally.
- **Closure test:** a harness row like probe A for `ls_remote_url` with insteadOf rewrites to `file://`, `http://` (loopback listener) and `git://`, asserting a named refusal, zero accepts and no ref read. Add a row for push prepare with a `pushInsteadOf` rewrite.

### P3-1: The "route before scratch directory" ordering in `read_remote_file` is unprotected (M7 survived)
- **Location:** `transport.rs::read_remote_file` (`:684-686`). The row `a_remote_file_read_from_a_local_only_route_is_refused` asserts only the refusal text.
- **Violated invariant:** brief item 3, "no filesystem write" before a refusal. The drafter's mutation M7 (moving `tempfile::tempdir()` above `for_remote`) exits 0, so the claim "a refused route writes nothing" for this site rests on code order alone.
- **Interleaving:** a regression makes the tempdir under `TMPDIR`, then refuses. `TempDir` drop removes it on the normal path, but a kill or abort between `tempdir()` and the drop leaves `…/.tmpXXXX` behind on the core.
- **Correction and test:** in the row, point the process temp directory (or an injected scratch root) at an empty directory and assert it is still empty after each refused route. Re-run M7 and expect a failure.

## 2. Invariant analysis (attacks that failed are part of the result)
- **Native refusal before any effect.** `build` calls `refuse_native` before `Session::driver`, so no session, thread, port or spawn gate exists on refusal.
  - `transport_setting::resolve` reads only the snapshot and the global config file it names. No agent, key or helper is reached, and no process-global timeout state is read.
  - A malformed setting is refused, which is fail-closed. A flag of `gwz` over a native variable correctly does not read the variable.
  - The setting is resolved once at build. A later `gwz.transport native` cannot reach a request, because a cli backend never consults the setting (P1-1 aside).
- **Placement and budget refusal ordering.** `validate_meta` → `require_cli` → `refuse_budget_for_cli` all run before the lock, before `RequestContext`, and before `session.begin`. A refused request registers nothing.
- **Shutdown and lifecycle.**
  - `shutdown` sets `closed` under the lock, then closes and cleans up. A request racing past the `closed` check hits `Session::begin` on a closed mux and fails. Its guard drop runs the existing cancel path, so no half-registered request survives.
  - Double shutdown reports 0 pending.
  - Drop without shutdown: `State::drop` closes the session, and in-flight requests (which hold an `Arc<Session>`) see carrier loss.
  - A build refused after `refuse_native` (`unique()`/mux failure inside `Session::driver`) leaves nothing: `State` is constructed only after success.
  - These are the same semantics as `TransportRuntime::open_request`. No new stuck state was found.
- **Concurrency.** Requests share one driver session and one spawn gate, as the runtime's cli route does. The lock is held only to read `closed` and clone two Arcs.
- **Identity cache.** Refusals are never inserted into `backend.identities.resolved`, so a cache hit cannot bypass the route check: a hit implies the same `(repo, remote, url)` already passed.
- **Before-any-effect at the other sites.**
  - `clone` reads only in `ensure_clone_target_is_empty` before the check; the row asserts no target.
  - `fetch`, `tag_fetch`, `ls_remote` and `push` do only `open_repo`/`find_remote` reads before the check; the rows assert no refs and no `FETCH_HEAD`.
  - The clone-family guard precedes `admitted_local_peer`, which only reads (`is_dir`/`canonicalize`). M3 is killed.
- **Forced-transport direction.** When `configure` installs the SSH or HTTPS route, git2-rs `smart_transport` sets `callbacks.transport`. A later libgit2 insteadOf or pushInsteadOf rewrite to a local URL is therefore handed to gwz's transport, not the native one. This is fail-closed and covers clone, `read_remote_file` and push.
- **URL adversity: fail-closed in the check except for P1-1.** Each spelling below is refused, as LocalPath or Other("unrecognised"):
  - uppercase `FILE://`;
  - leading whitespace (` ssh://…`);
  - percent-encoded schemes (`%66ile://`);
  - the empty URL;
  - `C:\…` and `C:/…`;
  - UNC paths;
  - `./x:y`;
  - `ftp://`.

  `file:x` is SCP-like (host `file`) for both git and libgit2, and here goes to the endpoint, where the client decides. `ext::…`/`foo::url` are admitted as SCP host `ext`/`foo` and bound to the endpoint's SSH route. That is not native and not local (see §4).
- **Messages.** The refusal text carries no part of the URL: the userinfo canary row passes, and `Other` keeps only `[A-Za-z0-9+.-]{1,16}`, lowercased. Native refusals name the selecting form and the config file path only. There are no environment values (canary row). The per-route names are fixed strings, so they are stable for scripts.
- **Non-cli placements.** Unchanged, as shown in the scope section.
- **cfg discipline.** All new conditional code is in `cfg_if!`. There is no bare `#[cfg]` on an import or declaration. `driver_only.rs` has no `cfg`, and the source check enforces that.
- **Windows parity.**
  - `driver_only_tests` is `cfg(test)` only and runs on Windows. Its loopback listener and `Url::from_file_path` fixtures are portable, and the local-path rows rely on `Path::is_absolute`, which is true for Windows drive paths.
  - The queued receipt will exercise the native-refusal rows and the existing route rows. It will **not** exercise the P1-1 Windows spellings (`.\x:y`, `\x:y`, `DOMAIN\user@host:r`) unless the closure rows are added.

## 3. Risks and next action
- **NO-GO** on P1-1 and P2-1. Both falsify the step's headline guarantee (no native route and no core-side agent or key access under cli) with reproducible probes that use the lane's own harness. Both have bounded fixes inside the step's own files.
- **Required revision:**
  1. One cli route classifier shared with the binding (P1-1(i)).
  2. No native fallthrough from `configure` in a cli context (P1-1(ii)).
  3. `ls_remote_url` checks the bound URL (P2-1).
  4. The P3-1 tempdir assertion.
  5. The closure rows above, with the P1-1 Windows spellings in the Windows receipt.
- The drafter's "OS-level recording double" deferral (decision c) should be revisited for 2.3. A PID or socket recorder on the core would have caught P1-1.
- I pre-commit to GO on a revision that resolves P1-1 and P2-1 as specified. P3-1 should land with it but does not block.

## 4. Out-of-scope observations (follow-up proposals; not findings)
1. **The same classifier split affects other placements.** P1-1's split (`identity::scp_like` versus `ssh_destination::local`) also makes the **local** in-process placement serve `DOMAIN\user@host:repo` on libgit2's native SSH instead of gwz's transport. It is pre-existing and not a cli-refusal defect. Proposal: unify the classifiers for all placements, which P1-1(i) largely does.
2. **IPv6 SCP spellings are misnamed and refused under cli.** `[::1]:repo` and `[2001:db8::1]:repo` are refused as "local path" because `scp_like` sees a one-character host `[`. git, libgit2 and `Destination::parse` all treat them as SSH. This is pre-existing and fail-closed, but it is a parity gap for IPv6-literal remotes.
3. **Transport-helper syntax goes to the endpoint as SSH.** `foo::url` (git's transport-helper syntax, e.g. `ext::`, `fd::`) is classed as SCP host `foo` and sent to the client endpoint's SSH route. That is harmless, since no helper runs, but the refusal would be clearer if named. Candidate for the §4.5/2.3 admission table.
4. **Unchecked drafter claim.** The drafter's own finding 1 (registering `check_driver_only_boundary.py` and its test in `run_tests.py` and the boundary workflow) stands. I did not check discovery by glob.
