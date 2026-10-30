# Step 1.7 driver-only construction: STATE-AXIS RE-REVIEW (round 3, final under the cap)
**Review object:** gwz-core `25941ce9`, plan step 1.7. Its parent is `b0ac8de7`; it amends `43f4ef0c`, which was reviewed in round 2 and is still in the object store. **Baseline:** lane `/Volumes/projects/limbo/gwz-dev-p1-7`. Root `34251a9c` and gwz-transport `96938616` are unchanged and reference only. **Date:** 2026-10-30
**Axis:** State: durable-state semantics and adversity. Independent, adversarial, read-only. Filed verbatim by the lane owner as `GwzDriverOnly-Step1.7-ReviewState-3.md`.
**Verdict: GO.** 0 P0, 0 P1, 0 P2, 0 P3 open. Every prior finding is closed, and there is no new architectural root cause.

## Prior-finding closure table

| ID (round) | Status | Evidence on `25941ce9` |
|---|---|---|
| **P1-1** (round 1): the check and the binding classified SCP-like URLs differently, so libgit2's native SSH ran from the core | Closed (round 2), still closed | The one classifier `bound_route` and the cli `refuse_unbound` binding are unchanged in this diff. The rows moved by the split are all present: the old and new `#[test]` name sets differ only by the four added rows. |
| **P2-1** (round 1): the route was judged on the caller's URL but bound on the rewritten one | Closed (round 2), still closed | The `judged_url` call sites are unchanged. Clone and read now also observe the remote libgit2 actually created; see round-2 P3-1. |
| **P3-1** (round 1): the scratch-directory ordering in `read_remote_file` was unprotected | Closed (round 2), still closed | The row moved to `driver_only_tests/scratch.rs`. M7 is still killed (`rem2-mutation/run.out`). |
| **P2-1** (round 2): identity selection used `scp_like`, a third classifier, so `[::1]:repo` was routed as SSH but treated as a non-SSH destination for identity | **Closed** | In cli context, `resolve_remote` now decides SSH with `ssh_destination(is_cli(backend), url)`, which is `ssh_scheme \|\| bound_route(url) == Bound::Ssh`. Local and native placements keep `scp_like`, and the plan records that split. **Probe D re-run:** `validate_url_identity` with a per-remote override is now `Ok(())` for `git@[::1]:repo`, `[::1]:repo`, `[2001:db8::1]:repo` and `ssh://[::1]/repo` (round 2: `[::1]:repo` gave `Err "…names a non-SSH destination"`). The malformed spelling `:x` is `Ok`: it is SSH-bound and then fails at the destination parse. `a\b:c` is still refused by name as a local path. The route-table row `identity_selection_reads_every_url_the_binding_binds_to_ssh_as_ssh_under_cli` pins agreement over all 37 spellings, and M13 is killed. **Correction to my round-2 impact statement:** the implementer's end-to-end rows (`rem2-red-rows.log` against `rem2-green-rows.log`) show the pre-fix tree recorded `Ambient` as the selection source for `[::1]:repo`, so the selected identity really was dropped. But every bracketed-IPv6 SCP spelling then ends core-side with `GitCommandFailed "invalid SSH destination"` and no connection. The "endpoint authenticates with its agent" consequence I stated in round 2 was therefore not reachable. The realised defect was the false per-remote refusal plus the recorded identity drop. The fixed tree records `InvocationDefault`/`InvocationRemote` on every attempt (rows `an_invocation_identity_is_kept…` and `a_per_remote_identity_is_kept…`). |
| **P3-1** (round 2): the clone and read judge read the default configuration and missed `includeIf "gitdir:"` | **Closed** | `clone_repo_named` and `read_remote_file` install a `remote_create` callback that creates the remote, then calls `identity::refuse_unjudged_remote`. In cli context, it refuses an unbound created URL by its route name (with an "after the route was judged" suffix) and refuses a change of bound class with a fixed message. A same-class rewrite is admitted and still rides the forced transport. **Probe C re-run** (my round-2 shape, run in a child process with its own `HOME`): `clone("https://example.invalid/inc")`, rewritten to `file://` by the conditional include, now gives `UnsupportedOperation "…the file:// route…(the new repository's configuration rewrote the remote to this route after the route was judged)"`. The target does not exist afterwards and the clones directory has 0 entries (round 2: unnamed `"HTTPS endpoint request failed: InvalidRequest"`, with the core path sent to the endpoint). The lane rows `a_conditional_include_rewrite_to_a_refused_route…` and `…_to_a_served_route_is_refused_only_when_it_is_another_kind` cover `file://`, `http://`, `git://`, https→ssh and https→https. M12 and M14–M17 are killed. |
| Round-2 §4 item 1: the test rows moved libgit2's process-wide global search path | Resolved (follow-up) | `GlobalConfiguration`, `set_search_path` and `reset_search_path` are gone (a grep of `src/` finds none). The global-configuration rows re-execute the test binary as a child (`child_process.rs`) with `HOME`, `USERPROFILE` and `XDG_CONFIG_HOME` pointed at the row's directory, and fail unless exactly one row ran and passed. |

## Changed-range analysis (`43f4ef0c..25941ce9`)
- **`identity.rs`.**
  - `ssh_destination(cli, url)` replaces the inline `scp_like`/`ssh_scheme` pair in `resolve_remote`, and `is_cli` gains a non-candidate stub.
  - Behaviour changes only in cli context: there, identity selection follows `bound_route`, the same classifier as the route check and the binding. Local and native placements are unchanged.
  - `refuse_unjudged_remote` is cli-gated inside, and the ordinary build gets a stub that always returns `Ok`.
  - `refusal_after_creation` appends a fixed suffix and carries no URL text.
  - The `cfg_if!` discipline is kept.
- **`transport.rs`.**
  - `clone_repo_named`'s existing `remote_create(repo.remote(remote, url))` callback moves after `attempt` is created and gains the observation.
  - `read_remote_file` gains a `remote_create` callback in every placement: **`repo.remote(name, url)` is exactly libgit2's `default_remote_create`** (`git_remote_create(out, repo, name, url)`, with `name` being the clone's default `origin`). Outside cli, the observation returns `Ok` immediately, so non-cli behaviour is identical. The same pattern was already used unconditionally in `clone_repo_named` before this step. **Judged acceptable.**
  - Ordering: libgit2 creates the clone's repository before the callback runs. A refusal from the callback lets libgit2 remove a target it created (probe C: target absent, clones directory empty). The read's scratch repository lives under the `TempDir` and is removed on drop.
  - The observation compares `created.url()`, the fetch-direction URL that clone fetches with. An unreadable URL maps to `""`, which is refused as a local path, so it fails closed.
- **`backend.rs`.** The test helper's gate is narrowed to `all(test, gwz_transport_candidate, any(unix, windows))`. It is not compiled into production.
- **Tests.** `driver_only_tests.rs` was split with rust-split into `driver_only_tests/*.rs`. I read it as a pure move: the old and new test-name sets are identical except for four added rows (two conditional-include rows and two identity rows). The submodules use `use super::*`, which the boundary check permits outside `driver_only.rs`.
- **Scripts.** Only the inventory and `gwz_git_boundary.json` counts and date changed; there is no logic change.
- **No new architectural root cause.** Both round-2 findings were residuals of round-1 causes (two classifiers; judged versus bound URL), and both are closed at their origin.

## 0. Evidence base
- **Tuple.** Checked at the start of round 3 and again at the end (20:29 AEDT): root `34251a9c`, gwz-core `25941ce9`, gwz-transport `96938616`. The gwz-core worktree was clean, and the tuple did not move.
- **Read:**
  - `dev-docs/GwzDriverOnly-Step1.7-RemPlan-2.md` and the full `git diff 43f4ef0c 25941ce9`;
  - the new `driver_only_tests/{identity_split,rewrites,child_process}.rs`;
  - `rem2-red-rows.log` and `rem2-green-rows.log` (the OBSERVED lines), `rem2-green-final.log` (556 passed) and `rem2-mutation/run.out` (M1–M17 all exit 101, restored).
- **Run.** I copied the tree with `cp -RL` to `…/scratchpad/probe17c/gwz-core`; `diff -rq src` was identical before I appended the probes. I appended my probes C (now run in a child process, because the global search-path guard is gone) and D. I ran `TMPDIR=/Volumes/gwztmp RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real CARGO_TARGET_DIR=…/p1-7-review-state cargo +1.95.0 test --lib --locked -- driver_only route_tests probe_ --nocapture --test-threads=1`.
  - Result: **57 passed, 0 failed.** That includes all lane `driver_only_tests` and `route_tests` rows on the identical source; outputs are quoted above.
- **Not run:** the ordinary-shape build and clippy (implementer logs `rem2-check-ordinary.log` and `rem2-clippy-*.log` exist), the full suite, and the Windows receipt (label `p1-7-20261030-c`) and WSL matrix (queued; out of scope). No network was used and no `SSH_AUTH_SOCK` was touched.

## Problem, requirement provenance, and scope
- **Provenance.** Every change traces to a round-2 ID in `RemPlan-2`: State P2-1, State P3-1 (the same as Code P3-2), and Code P3-1 (my round-2 §4 item 1). All edits stay in the step's own files.
- **Effect outside cli:**
  - `ssh_destination` returns the old predicate.
  - `refuse_unjudged_remote` returns `Ok` immediately.
  - The `read_remote_file` callback reproduces libgit2's default.

  Local and native placements are unchanged. The local-placement classifier split is recorded by the plan as a finding, not fixed here, which is correct under the step's scope.
- **Declared behaviour, judged:**
  - Bracketed-IPv6 SCP under cli is SSH-routed, keeps its selected identity, and is refused core-side at the destination parse ("invalid SSH destination") with no connection. That is fail-closed.
  - A conditional-include rewrite is refused by name, or by class change, before any connection.
  - A same-class rewrite is served on the forced transport.

## 1. Findings
None open.

## 2. Invariant analysis (attacks that failed are part of the result)
- **No native route under cli.** Unchanged since round 2: `configure` always installs a forced transport in a host context, so a divergence between the judged and bound URL can no longer reach libgit2's own transport. Probe C now shows the core refusing by name before the transport is even asked.
- **One classification per URL under cli.** The route check, the binding and identity selection all derive from `bound_route` plus `ssh_scheme`, and the 37-spelling route-table rows pin the check-to-binding and binding-to-identity agreement. Probe D shows no spelling routed as SSH but treated as non-SSH for identity. `:x` is admitted to identity selection and then refused at the destination parse; that ordering means only that the client endpoint is asked to check the identity before the parse refusal, as for any SSH remote.
- **Created versus judged URL.** The remaining divergence channel (repository-scoped configuration applied at creation) is observed inside the `remote_create` callback before any fetch. Unbound URLs are refused by name, a different bound class is refused, and the same class rides the forced transport to the client, which admits destinations in 2.3.
  - A concurrent edit of the global config between the judge and the clone falls into the same observation.
- **Before any effect.** Clone reads only before the judge; a callback refusal leaves no target (probe C and the lane rows). The read refuses before `tempdir()` (the scratch child row; M7 killed), and its post-creation refusal runs inside the `TempDir`.
- **Lifecycle, cache and messages.** Unchanged since round 1: refusals are never cached, refusal texts carry no URL, and the native refusal and placement refusals come before registration.
- **Test isolation.** No row mutates a process global in the shared binary any more. The child re-execution asserts that exactly one row ran. `GIT_CONFIG_NOSYSTEM` is not honoured by libgit2, but a host system config could only add rules for the probe's `example.invalid` URLs, so it is harmless.
- **Windows parity.** The classifier rows are pure and platform-independent. The child environment sets both `HOME` and `USERPROFILE`, and the row paths use forward slashes. The queued receipt runs all of them.

## 3. Risks and next action
- **GO.** The remaining release dependency is outside this review: the queued Windows receipt `p1-7-20261030-c` and the WSL matrix must be green on `25941ce9`.
- The lane owner should finalise the contract §15 row from the implementer's corrected text. "Before any effect" and "the core never falls back to libgit2's native transport under cli" now hold for every URL-taking entry, including created-remote rewrites.

## 4. Out-of-scope observations (follow-up proposals; not findings)
1. **IPv6 SCP spellings are unusable under cli.** Every bracketed-IPv6 SCP spelling is refused core-side by the session driver's destination parse (`"invalid SSH destination"`, `session/driver/opening.rs`), while git and libgit2 accept them. This is fail-closed, but it is a capability gap with an unnamed message. Propose a follow-up to accept or name it.
2. **The local-placement classifier split remains.** Identity selection there still uses `scp_like` while routing uses `Destination::parse`, which can split on `DOMAIN\user@host:r`. It is recorded by the plan as a finding; propose unifying it in a later step.
3. **Transport-helper spellings still go to the endpoint as SSH hosts.** `ext::`/`foo::` remain fail-safe. Name them in the §4.5 admission table (2.3).
