# Step 1.7 driver-only construction: STATE-AXIS RE-REVIEW (round 2)
**Review object:** gwz-core `43f4ef0c` (parent `b0ac8de7`; amended from `edfc623e`, which was reviewed in round 1 and is still in the object store). Plan step 1.7: `DriverOnlyBuilder`/`DriverOnly`, the cli route refusals in `identity.rs`/`transport.rs`/`transport_binding.rs`, and `check_driver_only_boundary.py` with its test and wiring. **Baseline:** lane `/Volumes/projects/limbo/gwz-dev-p1-7`. Root `34251a9c` and gwz-transport `96938616` are unchanged and reference only. **Date:** 2026-10-30
**Axis:** State: durable-state semantics and adversity. Independent, adversarial, read-only. Filed verbatim by the lane owner as `GwzDriverOnly-Step1.7-ReviewState-2.md`.
**Verdict: NO-GO.** 0 P0, 0 P1, 1 P2, 1 P3. All three round-1 findings are closed. The new P2 is a residual of the round-1 P1-1 root cause, made reachable by this revision's declared IPv6 change. I pre-commit to GO on a revision that resolves P2-1 as specified.

## Prior-finding closure table

| Round-1 ID | Status | Evidence on `43f4ef0c` |
|---|---|---|
| **P1-1:** the check and the binding classify SCP-like URLs differently (a `\` before the first `:` leads to native libssh2 from the core) | **Closed** | (i) One route classifier. `local_only_route` is now `bound_route(url) == Bound::Unbound` (`identity.rs`), and `bound_route` (`transport_binding.rs:177`) is exactly what `configure` binds on. Row `the_check_refuses_exactly_what_the_binding_does_not_bind` covers 37 spellings, including the Windows spellings `.\x:y`, `\x:y`, `C:\x`, UNC, `[::1]:repo`, `ext::cmd` and `%66ile://`. Those spellings are pure-classifier rows that run on every platform. (ii) In a cli context, `configure` installs `refuse_unbound` (a forced `smart_transport` that errors with the route's name) and never returns with the native route left in place. This covers the Windows `serves_http_natively` branch too, which is tested with the Windows policy forced on in `the_binding_in_a_cli_context_never_leaves_the_native_route_in_place`. **My original probe B re-run:** `[git\x@127.0.0.1:58999]:source.git` gives `UnsupportedOperation "…the local path route…"` and `native connect from the core: false`. Control B2 is unchanged and goes to the client endpoint (`"ssh setup timeout … attempt 4 of 4"`). Mutations M8 and M9 are killed. |
| **P2-1:** `ls_remote_url` checked the caller's URL but bound the insteadOf-rewritten one | **Closed** | Every URL-taking entry now judges the URL libgit2 will bind, via `identity::judged_url` (cli only; other placements keep the caller's URL): `ls_remote_url` uses `remote_anonymous(url).url()`, `perform_push` uses `pushurl().unwrap_or(url())`, and `clone`/`read_remote_file` use `Remote::create_detached(url).url()`. The judged URL is also the one passed to `configure`. **My original probe A re-run:** `ls_remote_url` with a repository insteadOf rule from `https://` to `file://` now gives `UnsupportedOperation "…the file:// route…"`, and `fetch` and `ls_remote` refuse likewise. No refs were written. New rows cover repository insteadOf, pushInsteadOf (named and anonymous remotes) and global-snapshot insteadOf to `file://`, loopback `http://` and `git://`, each with zero listener accepts. M10 and M11 are killed. One residual is a conditional-include gap, recorded as P3-1 below. |
| **P3-1:** the scratch-directory ordering in `read_remote_file` was unprotected (M7 survived) | **Closed** | The row `a_remote_file_read_refused_by_route_makes_no_scratch_directory` runs its refusals in a child test process with `TMPDIR`/`TMP`/`TEMP` set to a path that does not exist. `tempfile::tempdir()` does not create parents, so if the scratch directory came before the route check, the read would fail with an I/O error instead of the route's name, and the row would fail. The row also asserts the path is still absent and that the child ran exactly one row (`"1 passed"`). **Judgement: this does protect the ordering.** It fails for any reorder that puts `tempdir()` before `for_remote`. It holds on Unix (`std::env::temp_dir` reads `TMPDIR`) and on Windows (`GetTempPath2` reads `TMP`, then `TEMP`, and does not check that the path exists). Its limit is that it detects only writes under the process temp root, which is the only write site in this function. M7 is now killed (exit 101). |

## Changed-range analysis (`edfc623e..43f4ef0c`)
- **`transport_binding.rs`.** It gains `Bound`/`bound_route`, the single route classifier, and `refuse_unbound`. `configure` branches on `bound`.
  - Non-cli contexts are unchanged: the branches are the same order (`is_https_remote`, then `Destination::parse == Ok(None)`), and only cli takes `refuse_unbound`.
  - **Declared cli changes:** IPv6 SCP spellings (`[::1]:repo`) now go to the endpoint (previously refused as "local path"). Malformed SCP (`Destination::parse` returns `Err`) is now admitted as `Ssh` and fails at the endpoint's parse, with no connection.
  - **New residual root cause:** the identity selection in `resolve_remote` was not moved onto `bound_route`. See P2-1. This is not a new architectural root cause. It is the remaining half of round-1 P1-1's "two SCP classifiers" root cause, on the identity side.
- **`identity.rs`.** `local_only_route` and `unbound_route` are now driven by `bound_route`, and `judged_url` is added with a non-candidate stub that returns the caller's URL.
  - The `cfg_if` discipline is kept: the new `use` sits inside the candidate branch, and no bare `#[cfg]` is added.
  - `scp_like` is now used only by `resolve_remote`'s SSH and identity decision. That is the site of P2-1.
- **`transport.rs`.** Every URL-taking entry judges and binds the same URL, and the judged URL is computed before any effect.
  - In `clone_repo_named` it is computed before `RepoBuilder::clone` (only `ensure_clone_target_is_empty`, a read, comes first). In `read_remote_file` it is computed before `tempdir()`.
  - `create_detached` reads libgit2's default configuration (process search path), the same sources libgit2's clone reads. It does no I/O beyond the config read.
  - On a mismatch between the judged and the bound URL, the forced transport (`callbacks.transport`) still keeps libgit2's own transport out. That fail-closed backstop is why P3-1 below is only P3.
- **`backend.rs`.** `remote_callbacks_for_test` is added inside `cfg_if! { if #[cfg(test)] … }` and is not compiled into production.
- **`driver_only.rs`.** The behaviour-table text changed; there is no logic change.
- **`driver_only_tests.rs`.** It adds the `Recorder` (a loopback accept counter), the backslash, binding, insteadOf, pushInsteadOf, global-insteadOf and scratch rows, and a `GlobalConfiguration` guard. The guard moves libgit2's process-wide global search path, serialized only against its own row; see §4.
- **Scripts and CI.** `check_driver_only_boundary.py` is wired into `run_tests.py` `source_checks` and into the boundary workflow, and its unittest is added to the workflow. The inventory and the `gwz_git_boundary.json` counts are updated. These are Code-axis matters, with no state impact.

## 0. Evidence base
- **Tuple.** Checked at the start of round 2 and again at the end (19:17 AEDT): root `34251a9c`, gwz-core `43f4ef0c`, gwz-transport `96938616`. The gwz-core worktree was clean, and the tuple did not move.
- **Read:**
  - `dev-docs/GwzDriverOnly-Step1.7-RemPlan.md` and the full `git diff edfc623e 43f4ef0c`;
  - the implementer's `p1-7-logs/rem1-*` files: `rem1-green-final.log` (551 passed) and `rem1-mutation/run.out` (M1–M11 all exit 101, restored);
  - libgit2 1.9.7 `remote.c`: `git_remote_create_with_opts` and `canonicalize_url` (the Windows UNC rewrite gives a `/`-path, which `Destination` classes local, so it is refused) and `git_remote_create_detached`.
- **Run.** I copied the tree to `…/scratchpad/probe17b/gwz-core` with `cp -RL`; `diff -rq src` was identical before I appended the probes. I appended my unchanged round-1 probes A, B and B2 plus two new probes, C and D. I ran `TMPDIR=/Volumes/gwztmp RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real CARGO_TARGET_DIR=…/p1-7-review-state cargo +1.95.0 test --lib --locked -- driver_only route_tests probe_ --nocapture --test-threads=1`.
  - Result: **55 passed, 0 failed.** That includes all 21 of the lane's `driver_only_tests` rows and all 6 `route_tests` rows, run on the identical source.
- **Not run:** the ordinary-shape build (the implementer's `rem1-check-ordinary.log` exists), clippy, the full suite, and the Windows receipt and WSL matrix (queued; out of scope). No network was used and no `SSH_AUTH_SOCK` was touched.

## Problem, requirement provenance, and scope
- **Requested outcome** (plan 1.7, operator go 2026-10-30, the brief): every request is `cli`. Local-only routes (`file`, clone-family, `http://`, `git://`) and the native route are refused before any effect, with zero agent or key access by the core.
- **Remediation scope.** Every change traces to a round-1 blocking ID in the remediation plan (State P1-1, P2-1, P3-1; Code P2-1, P3-1, P3-2). The edits stay in the step's own files: the route-refusal site, `transport.rs`, the binding, `driver_only*` and the check scripts.
- **Effect on the shared backend outside cli:**
  - `bound_route` reproduces the old `configure` branch order exactly.
  - `judged_url` returns the caller's URL outside cli, and its `connecting` closure never runs outside cli. `create_detached` and `remote.url()` are therefore not evaluated for local or native placements.
  - The `refuse_unbound` branch is cli-only.
  - **Conclusion:** local and native placements behave as before.
- **Declared cli behaviour changes, judged:**
  - IPv6 SCP to the endpoint: correct for the route (git, libgit2 and `Destination` all read it as SSH). It exposes P2-1 on the identity side.
  - Malformed SCP refused by the binding: fail-closed, with no connection.
  - Global insteadOf for clone and read via `create_detached`: correct for unconditional rules. Conditional includes are not covered (P3-1).

## 1. Findings

### P2-1: Identity selection still uses `scp_like`, the old SCP classifier, so an IPv6 SCP spelling that is now routed as SSH is treated as a non-SSH destination (residual of round-1 P1-1's root cause, newly reachable)
- **Location:** `identity.rs::resolve_remote` (`:327`), `if !ssh_scheme && !scp { … return Ok(None) }`, with `scp = scp_like(url)`. `scp_like` is false when the text before the first `:` is a single character, which `[::1]:repo` and every `[::…]` literal are.
- **Violated invariant:** one classification per URL. The route that carries a request and the identity rules applied to it must agree. Split design §4.6 and the brief: invocation and per-remote identities are applied to SSH destinations and are ignored only for non-SSH ones. Under cli, `bound_route` now says `[::1]:repo` is SSH and the request goes to the endpoint's SSH route, while identity selection says it is non-SSH.
- **Reproduction** (probe D, through the lane's `Harness` and a cli `DriverOnly` request scoped with `with_transport`, using a per-remote identity for `origin` that the endpoint accepts):
  - `validate_url_identity(git@[::1]:repo) = Ok(())`
  - `validate_url_identity([2001:db8::1]:repo) = Ok(())`
  - `validate_url_identity(ssh://[::1]/repo) = Ok(())`
  - `validate_url_identity([::1]:repo) = Err(InvalidRequest "a per-remote SSH identity override names a non-SSH destination; …")`

  For the invocation default (`--identity`), the same branch returns `Ok(None)` with no error. The SSH route is then bound with `selected = None` (`transport_binding.rs` `HostRoute`), so the endpoint authenticates with its agent instead of the selected key.
- **Impact:**
  - A per-remote override is refused with a false reason.
  - Worse, `--identity PATH` is silently dropped for these spellings, and fetch and push go out under whatever identity the client's agent offers first. That can be a different account than the operator selected, on a mutating push.
  - The repository-configured identity path is skipped the same way.
  - Before this revision these spellings were refused, which was fail-closed. The revision's declared IPv6 change admits them, so this is a regression this revision introduces.
- **Required correction:** make `resolve_remote`'s SSH decision use the same classifier as the route. In candidate builds, treat a URL as SSH when `bound_route(url) == Bound::Ssh` (keep `ssh_scheme` for the scheme forms), at least in cli context, and preferably for all host contexts so that the local placement agrees too. Alternatively, keep refusing under cli every spelling where `bound_route` and `scp_like` disagree, until they are unified.
- **Closure test:** a route-table row asserting that for every URL in `the_check_refuses_exactly_what_the_binding_does_not_bind`'s list, `bound_route(url) == Bound::Ssh` implies identity selection treats it as SSH. Plus probe D as a harness row: `[::1]:repo` with a per-remote override is admitted, and with a default identity the selected identity reaches `HostRoute` (assert `selected` is `Some`).

### P3-1: The judged clone URL does not see conditional includes (`includeIf "gitdir:"`), so a clone rewritten by one is refused by the endpoint, unnamed, instead of by the core by name before any effect
- **Location:** `transport.rs::rewritten_by_global_configuration` (`:77`). `git2::Remote::create_detached(url)` reads the default configuration without a repository, so `includeIf "gitdir:…"` never matches. libgit2's clone later reads the new repository's configuration, where it does match.
- **Violated invariant:** the remediation plan's P2-1 disposition, "every URL-taking entry checks the URL libgit2 will bind, before any effect", with a named refusal.
- **Reproduction** (probe C): put a global config containing `[includeIf "gitdir:<clones>/"] path = rules.cfg`, where `rules.cfg` maps `https://example.invalid/inc` to a `file://` source, then call `clone_repo("https://example.invalid/inc", <clones>/c1)` under cli.
  - The detached remote gives the URL `https://example.invalid/inc`, so the check admits it as https.
  - The clone then fails with `GitCommandFailed "HTTPS endpoint request failed: InvalidRequest"`, and the target does not exist afterwards.
- **Impact:** the forced transport held, so no native route ran, nothing was read locally and no credential was used. However:
  - (a) the refusal is the endpoint's unnamed `InvalidRequest`, not the step's named route refusal;
  - (b) the rewritten `file://` URL, a core filesystem path, is sent over the carrier to the client endpoint;
  - (c) libgit2 initializes and then removes the clone target before failing, so the target is created transiently.

  This requires a conditional include in the core user's global config, so it is bounded.
- **Required correction:** under cli, judge the URL the clone's remote actually receives. For example, in the `remote_create` callback (`builder.remote_create(...)`), compare `repo.remote(remote, url)?.url()` with `judged` and return a named refusal when they differ or the new one is unbound. That refuses before the fetch, and libgit2 cleans up the target. Alternatively, document conditional includes as endpoint-refused and leave a named row.
- **Closure test:** probe C as a row asserting `UnsupportedOperation` naming `file://`, no carrier open, and no target.

## 2. Invariant analysis (attacks that failed are part of the result)
- **No native route under cli.** Every libgit2 network entry builds its callbacks through `remote_callbacks` → `configure`. In a host context, `configure` now always sets `callbacks.transport`: HTTPS, SSH, or refusing. libgit2 never reaches `transport_find_fn`'s native SSH, local, HTTP or git transports. Probes B and C confirm this, and so does the binding row with Windows policy forced on.
  - The Unix and Windows spelling sets agree because classification no longer depends on `Path::is_absolute`. Windows UNC `\\server\…` is canonicalized by libgit2 to `//server/…` and classed local.
- **Check and binding agree for each operation:**
  - fetch, tag_fetch and ls_remote: `remote_handle.url()` is used for both the check and the binding;
  - ls_remote_url: `remote_anonymous(url).url()` for both;
  - push and push_prepared: `pushurl().unwrap_or(url())` for both. The `local_push_remote` substitute is reachable only when `plan.url` is local, which is already refused;
  - clone and read_remote_file: `create_detached(url).url()` for both. Conditional includes are the exception (P3-1), and the forced backstop holds there.
- **Before any effect.**
  - Under cli, the judged URL is computed before any write: in clone, only the `ensure_clone_target_is_empty` read precedes it, and the row asserts no target. In `read_remote_file` it precedes `tempdir()`, guarded by the P3-1 closure row.
  - `refuse_unbound` marks the attempt failed when `configure` runs, before any connection, and is reached only when the check has been bypassed (today, only by the test helper).
- **Lifecycle, races and restarts.** Unchanged since round 1: native refusal before `Session::driver`, placement and budget refusals before registration, `closed` checked under the lock with the session refusing `begin` after close, and drop closing the session. The judged-URL computation adds no state.
  - A concurrent edit of global config between `create_detached` and libgit2's clone falls back to the forced-transport backstop, the same as P3-1.
- **Identity cache.** Refusals are still never cached, and the cache key now holds the judged URL, so a rewrite change produces a new key.
- **Messages.** Named per route and carrying no URL text; unchanged. The refusing transport reuses the same refusal text.
- **cfg discipline.** All new conditional code is in `cfg_if!`. The test-only helper is inside `cfg(test)`. There is no bare `#[cfg]` on imports.
- **Windows parity.** The classifier rows covering `.\x:y`, `\x:y`, `C:\x` and UNC run on every platform. The binding row forces the Windows HTTP policy. The `TMP`/`TEMP` child row covers Windows temp resolution. The queued receipt runs all of them.

## 3. Risks and next action
- **NO-GO** on P2-1 only. It is bounded: one predicate in `resolve_remote` moves onto `bound_route`, plus two rows.
- P3-1 should land with it or be recorded as an endpoint-refused case with a named row. It does not block.
- I pre-commit to GO on a revision that resolves P2-1 as specified.

## 4. Out-of-scope observations (follow-up proposals; not findings)
1. **Test-only process global.** `GlobalConfiguration` (`driver_only_tests.rs`) moves libgit2's process-wide global config search path, serialized only against its own row.
   - In a parallel full-suite run, any other test that resolves libgit2's global config during that window sees the temporary `.gitconfig`; for example, `scoped_evidence.rs:181` calls `repo.signature()`.
   - Its rules touch only `example.invalid` URLs, but a test relying on a global `user.name` could flake.
   - Proposal: run these rows in a child process, as the scratch row does, rather than mutating the search path in the shared test binary.
2. **The local placement still has the same identity split as P2-1.** Proposal: unify the classifiers for all placements, which P2-1's correction does if it is applied to all host contexts.
3. **Transport-helper spellings still go to the endpoint as SSH.** `ext::`/`foo::` are routed to the endpoint as SSH hosts (fail-safe). Proposal: name them in the §4.5 client admission table (2.3).
