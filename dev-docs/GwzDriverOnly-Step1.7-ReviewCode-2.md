# Step 1.7 driver-only construction — CODE-AXIS RE-REVIEW (round 1 remediation)
**Review object:** plan step 1.7 at gwz-core `43f4ef0c`. Its parent is `b0ac8de7`. It amends the round-1 object `edfc623e`, is unpushed, and `edfc623e` is still in the object store. The object is driver-only construction, the cli route refusal moved onto the URL libgit2 binds, a single route classifier, a refusing binding, and the boundary check with its gate wiring. **Baseline:** round-1 object `edfc623e` (changed range `edfc623e..43f4ef0c`). gwz-transport `96938616` and root `34251a9c` are unchanged and were used for reference only. **Date:** 2026-10-30
**Axis:** Code (architecture, interfaces, call graphs, compatibility). Independent, adversarial, read-only. Filed verbatim by the lane owner as `GwzDriverOnly-Step1.7-ReviewCode-2.md`.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 2 P3 (both new, neither blocking). All three round-1 findings are closed.

## Prior-finding closure table

| Round-1 ID | What it was | What changed | Closure evidence | Status |
|---|---|---|---|---|
| P2-1 | The route check judged the caller's URL; libgit2 bound the insteadOf-rewritten one. `ls_remote_url` served `file://` natively and made a core-side `http://` connection. | `identity::judged_url` judges the URL the operation will connect to, under cli only. That is the anonymous remote's `url()` in `ls_remote_url`; `pushurl().unwrap_or(url())` in `perform_push`; and `git2::Remote::create_detached(url).url()` in `clone_repo_named` and `read_remote_file`. The same URL is passed to `configure`. `configure` also installs a refusing transport for any unbound route in a cli context (`transport_binding.rs` `refuse_unbound`), so libgit2's own transport cannot run whatever the judge decides. | My round-1 counterexample, re-run on `43f4ef0c` (reviewer probe `probe_r2_original_insteadof_counterexamples`):<br>- `ls_remote_url` with a `file://` rewrite and with an `http://` rewrite: named refusal for each.<br>- `push_prepared` through an anonymous remote, `file://` and `http://`: named refusal for each.<br>- `clone_repo` with global `file://` and `http://` rewrites: named refusal, target not made.<br>- `read_remote_file` with the same two global rewrites: named refusal.<br>- Listener connections: 0. Peer gained no ref.<br>The implementer's rows and mutations M8, M10 and M11 are killed. | Closed. A narrow residual for clone is filed as new P3-2 (fails closed). |
| P3-1 | Boundary check wired into no gate. | `run_tests.py` `source_checks` and `.github/workflows/checked-artifact-boundary.yml` now run the check, and the workflow runs its unit test. `test_run_tests_parallel_checks.py` lists it. A unit test asserts the wiring. | Diff read. `rem1-wiring.log` shows a seeded `use super::Handoff;` failing. I ran the unit tests: 24 OK. | Closed |
| P3-2 | Deny-list missed the local-transport entries; `as` aliases evaded it. | Added a deny-list entry for `with_*local_transport*`, `TransportRuntime`, `cancellable` and `local_command`. Added an import allow-list (`ALLOWED_IMPORTS`). An `as` alias is refused. Every `Session::name` must be a call of `Session::driver`. | My round-1 copies re-run on the new check (`scratchpad/chk2`):<br>- v1 (`super::with_cancellable_local_transport`) is caught.<br>- v2 (`Session as Host; Host::endpoint`) is caught twice: as an alias and as outside the allow-list.<br>- A new v3 (`crate::transport_host::cancellable::run`) is caught. | Closed |

## Changed-range analysis

- **`transport_binding.rs`.**
  - `bound_route` (Https / Ssh / Unbound, built on `is_https_remote` and `ssh_destination::Destination::parse`) is now the one classifier. Both `configure` and the cli check (`identity::local_only_route`) use it.
  - The test `the_check_refuses_exactly_what_the_binding_does_not_bind` pins equivalence over 37 spellings.
  - In a cli context, `configure` returns no Unbound branch without installing a transport. Local placement keeps its native branches, including Windows `serves_http_natively`, unchanged.
  - Calling `attempt.failed` at configure time is harmless. Every caller connects or returns before the attempt is read.
- **`identity.rs`.**
  - `judged_url` runs its `connecting` closure only under cli. In non-cli and ordinary builds the requested URL is borrowed, so local placement is byte-for-byte unchanged on every entry.
  - `unbound_route` only names a route that is already Unbound. Its naming logic is the round-1 classifier minus the admission branch.
- **`transport.rs`.**
  - `ls_remote_url` and `perform_push` judge the handle libgit2 uses, which is the same object.
  - `clone_repo_named` and `read_remote_file` predict the rewrite with a detached remote. libgit2 `remote.c:233–256` confirms that a detached remote applies insteadOf from `git_config_open_default` with no repository and no I/O beyond reading the config.
  - The prediction misses conditional includes evaluated against the new repository (new P3-2). The binding change above makes that miss fail closed.
- **Declared behaviour change: "IPv6 SCP now goes to the endpoint".**
  - **Measured, not as declared** (probe `probe_r2_ipv6_scp_outcome`, candidate shape): `[::1]:repo`, `git@[::1]:repo` and `[2001:db8::1]:repo` each fail core-side in about 0.2 ms with `GitCommandFailed "invalid SSH destination"`.
  - The cause: `Destination::parse` keeps the brackets and then refuses a host containing `[` (`ssh_destination.rs:62–73`). No connection is made, nothing crosses to the client, and the clone target is not left behind.
  - At `edfc623e`, `[::1]:repo` was refused before any effect as "local path", which was the wrong name. `[2001:db8::1]:repo` already passed the old check and failed the same way.
  - **Judgement: acceptable.** It fails closed, and the new words are more accurate than "local path".
  - **Inaccuracy to fix in wording only:** the RemPlan declaration and the `an_ipv6_scp_spelling_is_ssh_as_git_and_the_binding_read_it` comment say these are served or reach the endpoint. They are not; gwz cannot serve an IPv6 SCP spelling at all. Neither the §15 row nor the findings file should claim otherwise.
  - The identity selector still uses `scp_like`, so `[::1]:repo` selects no identity while `git@[::1]:repo` does (probe `probe_r2_ipv6_scp_identity`). This is moot because the open refuses. It is recorded in §4.
- **Declared behaviour change: "malformed SCP now refused by the binding".**
  - Probe: `:x` gives "invalid SSH destination" for `ls_remote_url` and `clone`, in under 1 ms, with no target and no connection.
  - **Judgement: acceptable.** It is a fail-closed diagnosability change: an unnamed parse refusal replaces the named "local path". It does not open a route.
  - `[host]:repo` is unchanged from `edfc623e`. Both revisions send it to the SSH route, and git reads it the same way.
- **`gwz_git_boundary.json` ratchet** (`transport.rs` git2 count 26→27, producers 53→58).
  - The rise is exactly one `git2::Remote` token (`create_detached`) and five `git_error` uses: two in `rewritten_by_global_configuration`, one in `ls_remote_url`, two in `perform_push`.
  - **Necessity: demonstrated.** Judging the bound URL before any effect needs libgit2's rewrite of a remote. The handle-based entries already hold git2 handles. Moving the detached prediction into gwz-git is the boundary's eventual direction, but gwz-git is outside this step's file ownership.
  - **Judgement: accepted as the reviewed act** the file's `about` text requires ("a new exception is a reviewed act"; counts "never rise").
  - **Recommendation, not a finding:** bump the entry's `dated` to 2026-10-30, and name the effective-URL resolution in `responsibility`, so the audit trail records this rise.
  - `check_filesystem_boundary.py` passes.
- **`candidate_switch_inventory.txt`:** adds `enum LocalOnlyRoute` and `mod route_tests`, which `edfc623e` introduced without inventory entries. `check_candidate_switches.py` passes now. I missed this in round 1; it is now closed.
- **Tests and scripts:**
  - The new rows include a process-separated scratch row with `TMPDIR` set to a missing path, which kills M7.
  - The new rows also include rows that move libgit2's process-wide global search path inside the shared test binary (new P3-1).
- **No new architectural root cause.**
  - New P3-2 is a residual of round-1 P2-1's root cause, the judge predicting the bound URL rather than observing it. It is now confined to clone and the remote file read, and the binding fails it closed.

## 0. Evidence base

- **Tuple checked at start and at end; it did not move.** Lane root `34251a9c`, gwz-core `43f4ef0c` with a clean tree, gwz-transport `96938616`.
- **Read:**
  - `git diff edfc623e 43f4ef0c`, all 13 files.
  - `dev-docs/GwzDriverOnly-Step1.7-RemPlan.md`. I did not open the other axis's report.
  - libgit2 `remote.c` `git_remote_create_with_opts` (`:207–262`) and `create_detached` (`:372`); `sysdir.c` `git_sysdir_set` (`:504–540`), `git_sysdir_get` (`:490–499`) and `find_sysdir_file` (`:550–580`).
  - `ssh_destination.rs` `parse`, `local` and `scp`; `https_destination.rs` `parse`; `request.rs` open paths; `session/driver/opening.rs:22`.
  - The `gwz_git_boundary.json` header.
  - The implementer's `rem1-*` logs and `rem1-mutation/M1–M11`. M1–M11 are all killed, including M7 (previously surviving) and M8–M11 (new).
- **Runs**, all with `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p1-7-review-code` and `TMPDIR=/Volumes/gwztmp`:
  - **Lane, candidate shape:** `RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real cargo +1.95.0 test --lib --locked -- driver_only route_tests` gave 30 passed, 0 failed (`lane-candidate-r2.log`).
  - **Python:** `python3.13 -B -m unittest scripts/checks/test_check_driver_only_boundary.py scripts/checks/test_run_tests_parallel_checks.py` gave 24 OK. These checks each exit 0: `check_driver_only_boundary`, `check_filesystem_boundary`, `check_candidate_switches`, `check_spawn_gate_minting`, `check_windows_parity`, and `check_cfg_boundaries --skip-repo gwz-cli --skip-repo gwz-py`.
  - **Reviewer probes outside the lane** (copy `.../scratchpad/probe2/gwz-core` of `43f4ef0c` with sibling paths pointed at the lane; candidate shape; `--test-threads=1`):
    - `probe_r2_original_insteadof_counterexamples`;
    - `probe_r2_includeif_gitdir_clone_divergence`;
    - `probe_r2_ipv6_scp_identity`;
    - `probe_r2_ipv6_scp_outcome`.
    - Two test-only accessors were added to the copy's `backend.rs`.
    - Logs: `probe2-candidate.log`, `probe5-candidate.log`.
  - **Check counterexamples v1–v3** against the new script with `--root`.
- **Not run:** ordinary-shape tests (the implementer type-checked it: `rem1-check-ordinary.log`); the full suite; Windows. The Windows receipt and WSL matrix are queued and out of scope.

## Problem, requirement provenance, and scope

- **Requested outcome:** unchanged from round 1 (plan 1.7; brief deliverables 1–3; split §4.3 and §4.6).
- **Authority for remediating** is the two round-1 reports. Their findings converge: Code P2-1 and State P2-1 have one root cause.
  - Routing every cli context through the refusing binding rests on placement design §2 ("Failure never falls back"; no "key/helper access or Git-host network effect" for unsupported routes). The change is confined to `is_cli_context()`.
- **Local and native placements:** no behaviour change. `judged_url` borrows the requested URL outside cli, and `configure`'s non-cli branches are untouched.
- **File ownership:**
  - `run_tests.py` and the workflow were brief-adjacent. The RemPlan records that the gate-fixes lane edits the same lists and that the lane owner resolves the textual merge. That is a sequencing note, not a scope breach: the edit is the minimum needed to make the plan's red row a gate.
  - `gwz_git_boundary.json`'s rise is a demonstrated necessity, judged above.
- **The RemPlan's out-of-scope items are correctly parked:** IPv6 SCP wording, transport-helper syntax, process globals in `Git2Backend::new`, and the local-placement `DOMAIN\user` split.

## 1. Findings

### P3-1 (NEW) — Test rows move libgit2's process-wide global search path inside the shared lib test binary, which runs rows on parallel threads

- **Location:** `src/transport_host/driver_only_tests.rs`, `struct GlobalConfiguration`. It calls `git2::opts::set_search_path(ConfigLevel::Global, …)` and later `reset_search_path`, guarded by a `static GLOBAL_CONFIGURATION: Mutex<()>`. The rows that use it include `a_global_insteadof_rewrite_to_a_refused_route_is_refused_for_the_clone_and_the_read`.
- **Violated invariant:**
  - Test isolation, and the crate's own pattern for process state: the same file runs its `TMPDIR` row in a child process, `a_remote_file_read_refused_by_route_makes_no_scratch_directory`.
  - libgit2's documented contract that search paths are set before other operations.
  - The mutex serialises only the rows that take it. Every other libgit2 call in the binary takes no lock.
- **State and interleaving** (by code reading; not reproduced):
  - `git_sysdir_set` rewrites `git_sysdir__dirs[GLOBAL].buf` with `git_str_sets`, which may reallocate. A concurrent `find_sysdir_file` on another test thread holds `scan = git_str_cstr(syspath)` into that same buffer, unsynchronised. That is a use-after-free window (`sysdir.c:521`, `:558–566`).
  - Separately, any concurrent row that opens a repository or default configuration during the window reads the test's temporary global file instead of the runner's. For example, `~/` include resolution in `transport_setting` uses libgit2's process-wide home.
  - `scripts/run_tests.py` sets no `--test-threads`, so the suite runs rows in parallel.
- **Impact:** nondeterministic crashes or failures elsewhere in the lib suite on every platform and in CI. The suite is gwz's primary gate. The failure would be hard to attribute.
- **Correction:** run the global-configuration rows in a child process of the test binary, as the scratch row already does. Point `HOME` and `XDG_CONFIG_HOME` (or `GIT_CONFIG_GLOBAL` if the fork honours it) at the row's own directory there, so libgit2 initialises with that path. Remove `set_search_path` and `reset_search_path` and the static mutex.
- **Closure test:** grep shows no `opts::set_search_path` under `src/`, except where a source check restricts it to a re-executed child. The rows still kill M11.

### P3-2 (NEW; residual of round-1 P2-1's root cause, not a new architectural one) — The clone's route judge predicts the rewrite from the default configuration, but libgit2 rewrites with the new repository's configuration, which includes `includeIf "gitdir:"`

- **Location:** `src/git/gitbackend/transport.rs` `rewritten_by_global_configuration` (`git2::Remote::create_detached(url)`), used by `clone_repo_named` and `read_remote_file`.
  - The comment claims "libgit2 applies exactly those to a detached remote".
  - libgit2's clone creates its remote with `git_repository_config_snapshot` of the new repository (`remote.c:236–238`). That snapshot evaluates conditional includes against the new gitdir. `git_config_open_default`, used for the detached remote, cannot (`:239–241`).
- **Violated invariant:** the RemPlan's own closure rule for P2-1 ("checks the URL libgit2 will bind") and the drafter's named-refusal contract (plan 1.7: refused "by name" before any effect).
- **Reproduction** (probe `probe_r2_includeif_gitdir_clone_divergence`):
  - **Setup:** the global file holds `[includeIf "gitdir:<work>/"] path = <work.gitconfig>`, and that file maps `https://example.invalid/w.git` to the local `file://` source and `https://example.invalid/wh` to a loopback `http://` listener.
  - **Result:** `clone_repo("https://example.invalid/w.git", <work>/clone)` and the `http` variant each return `GitCommandFailed "HTTPS endpoint request failed: InvalidRequest"`. The target does not exist afterwards and there are 0 listener connections.
  - The judge saw `https://`, so `configure` installed the HTTPS route. libgit2 then handed it the rewritten URL, and the core-side `https_destination::Destination::parse` refused it.
  - The mirror case also exists: a conditional rule rewriting a local base to `https://` would be refused falsely.
- **Impact:**
  - Fail-closed, thanks to the binding change: no native route, no connection, no target left.
  - The user gets an unnamed, misleading HTTPS error instead of the route's name.
  - The comment and the proposed §15 wording overstate the mechanism.
- **Correction:** observe instead of predicting, or narrow the claim.
  - **Preferred:** in `clone_repo_named`'s `remote_create` callback, compare the created remote's `url()` with the judged URL. On a mismatch, refuse with the created URL's route name. libgit2 removes the target it made on clone failure, as the probe shows.
  - For `read_remote_file`, do the same, or state that its scratch path matches no `gitdir` rule in practice.
  - **Otherwise:** correct the comment to name the exception and record it as a known residual in the §15 row.
- **Closure test:** the probe above, asserting the named `file://` and `http://` refusals.

## 2. Invariant analysis

**Attacks that failed:**

- **The round-1 insteadOf counterexamples.** All are now refused by name before any effect: `ls_remote_url`, anonymous `push_prepared`, `clone_repo` and `read_remote_file`, each against `file://` and `http://` rewrites, with zero connections and no ref, target or scratch directory. Named-remote `fetch` and `push` were already correct.
- **Native route in a cli context.**
  - Under cli, `configure` now installs a transport on every path: Https, Ssh, or `refuse_unbound`. The Windows `serves_http_natively` branch is skipped under cli.
  - So libgit2's own transport, and with it `credential_callback`'s agent, helper and default-credential arms, cannot run for a cli request even if the judge errs. P3-2 is an instance where the judge errs and this holds.
  - Covered by `the_binding_in_a_cli_context_never_leaves_the_native_route_in_place` and M8.
- **Check versus binding agreement.**
  - Both are built on `bound_route`.
  - The backslash spellings (`a\b:c`, `CORP\user@host:r.git`, `[u\x@127.0.0.1:1]:r`, and on Windows `.\x:y`, `\x:y`) are refused as "local path" with zero connections (implementer rows; M9).
  - The identity selector still uses `scp_like`. The only divergence I found is the `[::…]` spellings, and those are refused at open (§4).
- **Boundary check evasions.**
  - v1, v2 and a new v3 are caught.
  - Fully qualified, non-`use` paths still bypass the import allow-list. Every known endpoint-making name and module is also on the deny-list, so I found no compiling evasion.
  - `Git2Backend::new()` without `with_host_context` is not forbidden by the script. It is caught by the runtime route rows, because every route row would succeed natively.
- **Gate wiring:** present in `run_tests.py`, in the workflow check list and in the workflow unittest list. `run_boundary_job.py` replays these.
- **Local placement and ordinary builds:** unchanged. `judged_url` borrows outside cli. The ordinary `judged_url` and `refuse_clone_family_under_cli` are identities, and the ordinary shape type-checks (implementer log).
- **Unchanged from round 1 and re-confirmed:** the builder's public surface, the native-setting refusal, placement refusal, and the HostHandle/`snapshot()` decision.

## 3. Risks and next action

- **Next action:** record GO for the code axis. Fix P3-1 before the next full-suite gate if practical: it is the one change here that can make unrelated rows flake. P3-2 and the wording fixes can land with it.
- **Wording to correct** in the §15 row and the findings file:
  - IPv6 SCP spellings are refused at the SSH route as "invalid SSH destination". They are not served and do not reach the endpoint.
  - The clone judge reads the default configuration. Conditional includes are a known residual that fails closed (P3-2), unless that residual is fixed.
- **Ratchet:** bump `dated` and `responsibility` on the `transport.rs` entry of `gwz_git_boundary.json`.
- **Pending gates:** the Windows receipt and the WSL matrix remain queued. The backslash rows and the binding row are the platform-sensitive ones.

## 4. Out-of-scope observations (follow-up proposals; not findings)

1. **Identity classifier is a third classifier.**
   - **Observation:** `identity::resolve_remote` decides SSH-ness with `scp_like`, while the route and the binding use `Destination::parse`. For `[::1]:repo`, an invocation `--identity` is not selected and a per-remote override is refused as "names a non-SSH destination". For `git@[::1]:repo` and `[2001:db8::1]:repo` an identity is selected (probe `probe_r2_ipv6_scp_identity`).
   - **Status:** moot today, because every bracketed-IPv6 SCP spelling is refused at open. It becomes a real credential-selection defect the day the SSH destination parser accepts IPv6 SCP hosts.
   - **Proposal:** derive the identity's SSH test from `bound_route` when that parser changes.
2. **Process globals in `Git2Backend::new()`** (`ensure_server_timeout`, `process_environment`): unchanged since round 1. The RemPlan parked it in the findings file; I agree.
3. **The `edfc623e` inventory gap:** `LocalOnlyRoute` and `route_tests` had no `candidate_switch_inventory.txt` entries. It is fixed in this range. My round-1 review did not run `check_candidate_switches.py`; it does now.
