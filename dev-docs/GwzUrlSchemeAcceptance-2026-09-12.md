# GWZ URL scheme: acceptance evidence

Date: 2026-09-12. Plan: `GwzUrlSchemePlan.md` (step 3.2). Baseline:
`GwzUrlSchemeBaseline-2026-09-11.md`. Work happened in the root workspace,
one commit per plan step, each member commit paired with a root commit that
records the lock.

## 1. What changed for each kind of user

- **Reader without SSH keys.** `gwz clone --url-scheme https <root url>` (or
  `GWZ_URL_SCHEME=https`) now delivers every public member over https. The
  private member is skipped quietly, exactly as before, and `gwz status` lists
  it as not materialized. The choice is remembered in `.gwz/url-scheme.yml`,
  so a later `gwz materialize --lock` in that workspace needs no flag. Without
  the flag the reader still fails, but the error now names the member and ends
  with the remedy.
- **Contributor with SSH keys.** Nothing changed. The bare clone produced the
  same seven ssh remotes as the baseline; the only difference in the JSON is
  the new `url_resolution` object per cloned member, reporting `manifest`,
  `default`, `derived: false`.
- **Manifests.** Untouched everywhere. `repo sync` behaviour is unchanged by
  this feature (see §5).

## 2. Binary and environment

- Binary: `target/debug/gwz` built from gwz-core `4282bfb` and gwz-cli
  `6357007` (this tree). It was used only on scratch clones under the session
  scratchpad, never on a workspace. Workspace operations kept using the
  installed `gwz 1.0.10`.
- Reader environment: a temporary `HOME` holding only a copy of
  `~/.ssh/known_hosts`, `SSH_AUTH_SOCK` unset, `XDG_CONFIG_HOME` pointing at the
  same temporary home (baseline §12). This removes both the ssh-agent and the
  `gh`/keychain https credentials that make this Mac non-anonymous.
- Contributor environment: the normal shell (ssh-agent authenticates as
  `gripd`; see baseline §1).

## 3. Cases, all against GitHub, 2026-09-12 05:22 to 05:27 local

| # | who | command | result |
|---|---|---|---|
| 1 | reader | `--json clone --url-scheme https https://github.com/owebeeone/gwz-dev.git ws1` | exit 0, `aggregate_status: Ok`; 7 members `Ok`, each `url_resolution` = `{scheme: https, source: request, derived: true, host_known: true}` with `git@github.com:owebeeone/<repo>.git -> https://github.com/owebeeone/<repo>.git`; member remotes recorded as https; `gwz-core-evidence` absent on disk and listed by `gwz status` as not materialized; `.gwz/url-scheme.yml` = `schema: gwz.url-scheme/v1, scheme: https, recorded_by: clone` |
| 2 | reader | `GWZ_URL_SCHEME=https` + `--json clone git@github.com:owebeeone/gwz-dev.git ws2` | exit 0; `meta.message` = "cloned the workspace root from https://github.com/owebeeone/gwz-dev.git (derived from git@github.com:owebeeone/gwz-dev.git)"; root remote https; members as in case 1 |
| 3 | reader | in ws1, remove `gwz-py`, `--json materialize --lock` with no flag | exit 0; `mem_gwz_py` re-cloned with `{scheme: https, source: workspace, derived: true}`; the six members already checked out carry `url_resolution: null` and were not touched (1 transport row) |
| 3b | reader | in ws1, remove `taut`, `--verbose materialize --lock` | human output: `url scheme: https (from .gwz/url-scheme.yml)` then `taut: git@github.com:owebeeone/taut.git -> https://github.com/owebeeone/taut.git` |
| 4 | contributor | `--json clone /Users/owebeeone/limbo/gwz-dev ws3` (bare) | exit 0; 7 members `Ok` with `{scheme: manifest, source: default, derived: false}`; remotes `git@github.com:...` unchanged; no `.gwz/url-scheme.yml`; 8 transport rows as in the baseline |
| 5 | reader | bare `--json clone https://github.com/owebeeone/gwz-dev.git ws5` | exit 1, `Failed`, `members: []`; the single error now carries `member_id: mem_gwz_cli` and reads `SSH key authentication failed (no usable identity in the ssh-agent); run ssh-add or check your SSH setup; for public repositories, retry with --url-scheme https or set GWZ_URL_SCHEME=https`; human output identical on stderr |
| 6 | contributor | `--json clone --url-scheme ssh https://github.com/owebeeone/gwz-dev.git ws6` | exit 0; `meta.message` reports the root derived to `git@github.com:owebeeone/gwz-dev.git`; members `{scheme: ssh, source: request, derived: false}` (already ssh form); root and member remotes scp form |
| 7 | anyone | `--json clone --url-scheme https ssh://git@github.com:2222/owebeeone/gwz-dev.git ws7` | exit 1, `UrlSchemeUnavailable`: "workspace root: cannot derive https form for ssh://git@github.com:2222/owebeeone/gwz-dev.git: nonstandard SSH port 2222; use --url-scheme manifest for this run, or record a remote in the wanted form with `gwz repo sync`"; no directory created, no network access |

Total scratch footprint: 274 MB, removed after the run.

## 4. Test suites at the commits

| member | runner | result |
|---|---|---|
| gwz-core | `python3 scripts/run_tests.py` (fake and native backends) | 2122 passed, 0 failed at step 2.1 and 2123 at step 3.3 (2112 before the feature; the new derivation, state, plumbing and sync tests account for the difference) |
| gwz-cli | `cargo test -p gwz --locked` | 264 passed, 0 failed at steps 2.2 and 3.3 (three new rendering tests, two new binary workflow tests) |
| gwz-py | `GWZ_RUST_BIN=<root target>/debug/gwz .venv/bin/python run_tests.py` | at step 1.2: 815 passed, 2 failed, see §5; step 2.3 result recorded below when its commit lands |

gwz-py at step 2.3 (`GWZ_RUST_BIN` set to the root workspace build): 838 passed, 1 failed, the pre-existing merge-artifact test of §5; `test_native_module_reports_compiled_core_provenance` passes with extension and binary built from gwz-core `4282bfb`.

## 5. Findings outside this feature (not fixed here)

- **gwz-py's runner prefers a stale binary.** `run_tests.py` builds the CLI
  into the root workspace target, then looks for `gwz-cli/target/debug/gwz`
  first; a leftover Sep 8 binary there satisfied the check, so the parity tests
  ran against an old gwz and failed on old behaviour. Passing `GWZ_RUST_BIN`
  explicitly avoids it. Cheap fix: prefer the workspace binary when both exist.
- **Pre-existing gwz-py failure.**
  `test_merge_cli_cross_driver.py::test_clean_merge_jsonl_reports_verified_publication_artifacts_in_order`
  expects four publication artifacts and now sees five: gwz-core `a6469cc`
  ("preserve integrity markers through clone and merge", 2026-09-11) added
  `gwz.conf/markers/conf-integrity.yml`. It fails identically with the
  installed 1.0.10 as the Rust driver. The gwz-py expectation belongs to that
  change.
- **`test_native_bridge.py::test_native_module_reports_compiled_core_provenance`**
  compares the extension's compiled core provenance with the Rust binary's; it
  fails whenever the two were built from different core trees. It failed once
  during this work for that reason (the extension was rebuilt while step 2.1
  was applied but uncommitted) and is re-verified with step 2.3.
- **gwz-py `Cargo.lock`** drifts from gwz-core 1.0.8 to 1.0.10 under
  `maturin develop`; left unstaged, it belongs to the release refresh.
- **`repo sync` after an https clone** would copy the https remote into the
  manifest (baseline §7). Step 3.3 of the plan addresses it and landed: a configured remote that differs from the manifest only by URL scheme on a known host keeps the recorded URL, is reported in the member's `url_resolution` and in the response message, and `--force` records the configured form; covered by `repo_sync_keeps_a_manifest_url_that_differs_only_by_scheme_unless_forced`.

## 6. Commits

| step | member | commit | root lock commit |
|---|---|---|---|
| 0.1 docs | root | `c69d88c` | (root) |
| 1.1 derivation | gwz-core | `200be4e` | `322982e` |
| 1.2 protocol | gwz-core | `3e1500a` | `7301053` |
| 1.2 protocol | gwz-py | `09c4fd0` | `ba1a789` |
| 2.1 plumbing | gwz-core | `4282bfb` | `86ee7fc` |
| 2.2 CLI | gwz-cli | `6357007` | `ed63e7a` |
| 3.1 docs | gwz-cli | `103a409` | `1e30642` |
| 2.3 gwz-py | gwz-py | `6343ff5` | `5befaa4` |
| 3.3 repo sync | gwz-core, gwz-cli | `d97704b`, `010b8df` | `443525b` |
| 3.2 this note | root | (this commit) | (root) |

No version bump, no tag, no push.
