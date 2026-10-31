# Step 1.7 driver-only construction — CODE-AXIS RE-REVIEW, ROUND 2 (final under the cap)
**Review object:** plan step 1.7 at gwz-core `25941ce9`. Its parent is `b0ac8de7`. It amends `43f4ef0c`, which is still in the object store, and is unpushed. The changes are a single SSH classifier for identity selection under cli, a check of the clone's created remote, the global-configuration test rows moved into a child process, `driver_only_tests.rs` split into `driver_only_tests/*.rs`, the clippy dead-code fix, three new inventory sites, and the ratchet record. **Baseline:** previous object `43f4ef0c` (changed range `43f4ef0c..25941ce9`). gwz-transport `96938616` and root `34251a9c` are unchanged and were used for reference only. **Date:** 2026-10-30
**Axis:** Code (architecture, interfaces, call graphs, compatibility). Independent, adversarial, read-only. Filed verbatim by the lane owner as `GwzDriverOnly-Step1.7-ReviewCode-3.md`.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 0 P3 open. Every prior code-axis finding is closed, and this round raises no new finding.

## Prior-finding closure table

| ID | Round | What it was | What changed | Closure evidence | Status |
|---|---|---|---|---|---|
| P2-1 | 1 | The route check judged the caller's URL; libgit2 bound the insteadOf-rewritten one. | Closed in round 1. Unchanged here. | The lane's rewrite rows pass (33 of 33 below), and M10 and M11 are killed in `rem2-mutation`. | Closed (round 1) |
| P3-1 | 1 | Boundary check wired into no gate. | Closed in round 1. Unchanged. | `check_driver_only_boundary.py` prints ok. Its unit test passes. The wiring assertion is still in it. | Closed (round 1) |
| P3-2 | 1 | Deny-list missed the local-transport entries and `as` aliases. | Closed in round 1. Unchanged. | — | Closed (round 1) |
| P3-1 | 2 | Test rows moved libgit2's process-wide global search path inside the shared, parallel test binary. | `GlobalConfiguration`, `set_search_path`, `reset_search_path` and the static mutex are gone. The three global-configuration rows (`rewrites.rs:174`, `:220`, `:280`) re-execute themselves alone with `--test-threads=1` in a child of the test binary, through `driver_only_tests/child_process.rs`. The child has `HOME` and `USERPROFILE` set to the row's directory, `XDG_CONFIG_HOME` pointed at a missing directory, and `GIT_CONFIG_NOSYSTEM=1`. | `grep -rn set_search_path src` returns 0 lines. `rem2-p3-1-grep.log` agrees. M11 is still killed. My probe `probe_r3_includeif_gitdir_clone` uses the same child mechanism and behaved as intended. | Closed |
| P3-2 | 2 | The clone and remote-file judges predicted the rewrite from the default configuration and missed `includeIf "gitdir:"` in the new repository's configuration. The outcome was fail-closed but unnamed. | `clone_repo_named` and `read_remote_file` now install a `remote_create` callback (`transport.rs:53–59`, `:727–732`). It creates the remote and then calls `identity::refuse_unjudged_remote`. Under cli, a created URL that differs from the judged one is refused in one of two ways. If the route is unserved, the refusal names that route, followed by "after the route was judged". If it is a served route of another kind, the refusal is "different route". A served route of the same kind passes. The attempt records the refusal, so `attempt.error()` returns the named error. libgit2 removes the target it made. | My round-2 probe, re-run on `25941ce9` (`probe_r3_includeif_gitdir_clone`, child process):<br>- `file://` through `includeIf`: named refusal, target absent afterwards.<br>- `http://` through `includeIf`: named refusal, target absent afterwards.<br>- `https://` → `ssh://` through `includeIf`: refused as "different route", target absent afterwards.<br>- 0 listener connections.<br>Lane rows `a_conditional_include_rewrite_to_a_refused_route_is_refused_by_name_for_the_clone_and_the_read` and `…_to_a_served_route_is_refused_only_when_it_is_another_kind` pass. M12 and M14–M17 are killed. | Closed |
| §3 wording | 2 | The IPv6 test name and comment said "served" or "reaches the endpoint". The ratchet entry was not re-dated. | The test is now `an_ipv6_scp_spelling_is_bound_to_the_ssh_route_and_gwz_cannot_open_it`, and its comment states the measured core-side refusal. The `gwz_git_boundary.json` `transport.rs` entry now has `dated` 2026-10-30, and its `responsibility` names the effective-URL resolution. | Diff read. `check_filesystem_boundary.py` exits 0. | Closed |

## Changed-range analysis

- **Identity classifier** (`identity.rs` `ssh_destination`, `is_cli`; this settles the other axis's round-2 P2).
  - **Change:** under cli, identity selection treats a URL as SSH when it has an SSH scheme or when `transport_binding::bound_route(url) == Bound::Ssh`. That is the classifier the check and the binding already use. Outside cli, and in ordinary builds, the old `ssh_scheme || scp_like` rule stands unchanged.
  - **Pinning:** the row `identity_selection_reads_every_url_the_binding_binds_to_ssh_as_ssh_under_cli` over the shared 37-entry `ROUTE_TABLE`, and mutation M13.
  - **Measured end to end** (my `probe_r3_ipv6_scp`, cli, invocation `default_identity`). For `[::1]:repo`, `git@[::1]:repo`, `[2001:db8::1]:repo` and `:x`:
    - the route is not refused as local;
    - the identity is selected (`Some(path)`); round 2 measured `None` for `[::1]:repo`;
    - `ls_remote_url` and `clone` both fail with `invalid SSH destination`;
    - the clone target is absent.
  - **Judgement:** the split is gone, so no spelling selects an identity on one classifier and routes on another. The end-to-end outcome is still a core-side refusal at the SSH destination parse, as I measured in round 2. Nothing reaches the endpoint and nothing connects.
  - **Side effect:** malformed SCP spellings such as `:x` now select the identity under cli. Before the open refuses, the client endpoint's `check_identity` is asked about the selected path. That call is `Effect::None` and names only a path the client itself supplied, so I accept it.
  - **Scope:** confined to `is_cli_context()`. The local-placement classifier split is left unchanged and recorded by the RemPlan as a finding, which is correct under the step's cli confinement.
- **Observed-remote check** (`refuse_unjudged_remote`, `refusal_after_creation`).
  - **Ordering:** it runs inside libgit2's clone, after the new repository exists and before any connection. "Before any effect" therefore becomes "nothing left behind and nothing connected" for this conditional-include residual only. Unconditional global rules are still refused before the target exists; the row asserts the words are absent there.
  - **Fail-closed paths:**
    - A non-UTF-8 created URL becomes `""` (`unwrap_or_default`), which is refused as a local path.
    - The comparison is between two libgit2-canonicalised URLs: the detached judge and the repository remote.
    - Outside cli the function returns `Ok(())` at once, and `read_remote_file`'s new callback reproduces libgit2's default (`git_remote_create(repo, name, url)`). Local placement is therefore unchanged.
  - **Same-kind rewrite passes.** For example, `https://a` rewritten to `https://b` keeps the bound route, and the endpoint admits the new destination. That is split §3.5's rule.
  - **Cross-kind rewrite** (`https://` to `ssh://`) is refused by name. At `b0ac8de7` and `43f4ef0c` this case already failed under cli, but unnamed, because the HTTPS route was handed an SSH URL. It is not a regression. §4 records it as a limitation.
- **`backend.rs`:** `remote_callbacks_for_test` now sits in its own `cfg_if!` block (`all(test, gwz_transport_candidate, any(unix, windows))`). This fixes the clippy `dead_code` failure the WSL gate found on `43f4ef0c`. The conditional section has an explicit boundary, as the workspace rule requires.
- **`candidate_switch_inventory.txt`:** adds `fn remote_callbacks_for_test`, `fn is_cli` and `fn ssh_destination`. `check_candidate_switches.py` exits 0.
- **Test split** (rust-split): `driver_only_tests.rs` is now a 38-line module root plus seven files of 38–346 lines each. That is within the repository's ≤500 rule. Rows and helpers are re-exported with `pub(crate) use`, and no function body was split. The lane runs 33 rows, up from 30 in round 2.
- **Ratchet:** `transport.rs` stays at git2 27 and producers 58, the same counts I accepted in round 2. Only `dated` and `responsibility` changed.
- **Implementer's outside finding 2** (candidate-shape clippy reports 123 errors): **this is pre-existing on `b0ac8de7`.**
  - I ran `RUSTFLAGS='--cfg gwz_transport_candidate' CLIPPY_CONF_DIR=… cargo +1.95.0 clippy --no-deps --all-targets --all-features --locked -- -D warnings` on a `git archive b0ac8de7` extraction (log `clippy-candidate-b0ac.log`). It gave "123 previous errors".
  - The error kinds and file multiset are identical to the implementer's `rem2-clippy-candidate.log` on `25941ce9`. The single positional difference is `transport_binding.rs:186` → `:229`: `https_policy_for`, which is used only by tests and was moved down 43 lines by this step's insertion.
  - CI's clippy job (`checked-artifact-boundary.yml:138`) runs the ordinary shape only, so the candidate-shape debt is ungated. It is not this step's.
- **No new architectural root cause.** The two changes close the last residuals of round-1 P2-1's root cause (judge and bind disagreeing) on both the route side and the identity side.

## 0. Evidence base

- **Tuple checked at start and at end; it did not move.** Lane root `34251a9c`, gwz-core `25941ce9` with a clean tree, gwz-transport `96938616`.
- **Read:**
  - `git diff 43f4ef0c 25941ce9`, all 13 files. That includes the new `driver_only_tests/{child_process,identity_split,rewrites}.rs` read in full.
  - `dev-docs/GwzDriverOnly-Step1.7-RemPlan-2.md`. I did not open the other axis's reports.
  - The implementer's `rem2-*` logs and `rem2-mutation/M1–M17`. All 17 are killed.
- **Runs**, all with `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p1-7-review-code` and `TMPDIR=/Volumes/gwztmp`:
  - **Lane, candidate shape:** `RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real cargo +1.95.0 test --lib --locked -- driver_only route_tests` gave 33 passed, 0 failed (`lane-candidate-r3.log`). This includes the three child-process rows.
  - **Source checks:** `check_driver_only_boundary.py` printed ok, and `check_candidate_switches.py` and `check_filesystem_boundary.py` each exited 0. `python3.13 -B -m unittest scripts/checks/test_check_driver_only_boundary.py` passed.
  - **Reviewer probes** (copy `.../scratchpad/probe3/gwz-core` of `25941ce9` with sibling paths pointed at the lane, plus two test-only accessors; candidate shape; `--test-threads=1`): `probe_r3_includeif_gitdir_clone` (child process) and `probe_r3_ipv6_scp`. Log `probe3-candidate.log`. These are my round-2 includeIf and IPv6 probes, re-run.
  - **Baseline clippy:** candidate-shape clippy on a `git archive b0ac8de7` extraction (`clippy-candidate-b0ac.log`).
- **Not run:** ordinary-shape tests (the implementer type-checked and ran clippy on the ordinary shape: `rem2-check-ordinary.log`, and `rem2-clippy-ordinary.log` exits 0); the full suite; Windows. Receipt `p1-7-20261030-c` is queued and out of scope.

## Problem, requirement provenance, and scope

- **Requested outcome:** unchanged (plan 1.7; brief deliverables 1–3; split §4.3 and §4.6).
- **Authority for this round:** both axes' round-2 reports, as merged in RemPlan-2.
  - The identity change rests on the same authority as the route classifier: placement design §2 (no selected identity may be silently replaced by ambient credentials, and no unsupported route may fall back).
  - The observed-remote check rests on split §3.5 and §4.6.
- **Local and native placements:** no behaviour change.
  - `ssh_destination` keeps `scp_like` outside cli.
  - `refuse_unjudged_remote` returns at once outside cli.
  - `read_remote_file`'s new callback is libgit2's default remote creation.
- **File ownership:** within the brief. The clippy and inventory fixes are needed for the gates. The candidate-clippy debt is correctly left alone as outside scope, and as shown above it is pre-existing.

## 1. Findings

None open. Round-2 P3-1 and P3-2 are closed (table above). I found no new P0–P3 defect.

## 2. Invariant analysis

**Attacks that failed:**

- **Judge versus bound URL, all entries.**
  - `ls_remote_url` and `perform_push` judge the handle libgit2 uses.
  - `clone_repo_named` and `read_remote_file` judge the predicted URL and now also check the created remote.
  - With unconditional, conditional (`includeIf "gitdir:"`), `file://`, `http://` and cross-kind rewrites, every refusal is named, there are zero connections, and no target is left (probes, lane rows, M10–M12 and M14–M17).
- **Native route under cli:** `configure` still installs a transport on every cli path (M8). The observed-remote check refuses earlier and by name.
- **Identity:**
  - No URL in the 37-entry table, nor any spelling I probed, gets a different SSH verdict for routing and for identity under cli.
  - A selected identity is recorded on every attempt (`identity_split.rs` rows; M13).
  - The bracketed-IPv6 outcome is a core-side "invalid SSH destination" with no connection, so no ambient credential is used in place of the selected one.
- **Test isolation:** no row changes libgit2's process-wide search path. The child rows inherit the parent's environment plus explicit overrides, run one row each, and fail the parent unless "1 passed" appears.
- **Unchanged from earlier rounds and re-confirmed:** builder surface, native-setting refusal, placement refusal, boundary check and its gate wiring, HostHandle and `snapshot()` decision.

## 3. Risks and next action

- **Next action:** record GO for the code axis. Finalise the §15 row from the implementer's corrected text. It may now say "refused by name before any connection". For a conditional-include rewrite it should say "the clone's target is created and removed". For bracketed-IPv6 SCP spellings it should say "refused at the SSH destination parse".
- **Pending gate:** Windows receipt `p1-7-20261030-c`. The backslash rows, the binding row and the child-process rows are the platform-sensitive ones. The child sets `USERPROFILE` as well as `HOME` for libgit2's Windows lookup.

## 4. Out-of-scope observations (follow-up proposals; not findings)

1. **Cross-kind conditional rewrites are refused, not served, by a cli clone.**
   - **Observation:** a common work/personal split pattern puts `url."ssh://…".insteadOf https://…` (or the SCP form) inside an `includeIf "gitdir:"` file. A cli clone of the `https://` URL into that directory is refused as "different route". It already failed unnamed at `b0ac8de7`.
   - **Impact:** a limitation, not a regression. It does not block 1.7.
   - **Proposal:** for 2.3 or phase 3, clone as `init` → create the remote → judge and bind the observed URL → fetch → checkout, so the callbacks are built for the URL actually used.
2. **Candidate-shape clippy debt is ungated.**
   - **Observation:** the 123 errors, pre-existing on `b0ac8de7`, are invisible to CI because the boundary job runs clippy on the ordinary shape only.
   - **Status:** a confirmed coverage gap, outside this step.
   - **Proposal:** a separate cleanup, then a candidate-shape clippy line in the boundary workflow.
3. **Carried from round 2, still parked:** process globals in `Git2Backend::new()`, and the local-placement identity-classifier split. The RemPlan records the latter.
