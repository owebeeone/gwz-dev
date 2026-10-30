# Gate fixes — CODE-AXIS REVIEW

**Review object:** gwz-core `b0ac8de7..94f23ab2` (7 files under `scripts/` and `.github/workflows/`), plus the installed tool changes. The tool changes are `/Volumes/projects/limbo/build-targets/tools/{remote-gates.zsh,remote-gates.md,lanes-1024-common.md}` against their `.pre-gate-fixes` copies in `/Volumes/projects/limbo/build-targets/archive/`.
**Baseline:** gwz-core parent `b0ac8de7` (origin/main); tools as archived 2026-10-27.
**Date:** 2026-10-30 **Axis:** Code (single review; escalation to a second axis follows any P0/P1/P2)
**Verdict: NO-GO** — P0 0, P1 0, P2 2, P3 3. I pre-commit to GO on a revision that closes P2-1 and P2-2 with their closure tests, and records P3-1..P3-3 as fixed or as explicit operator decisions.

## 0. Evidence base

**Tuple.** The lane is `/Volumes/projects/limbo/gwz-dev-gate-fixes/gwz-core`. HEAD was `94f23ab2bef69f047dd83948014173f2f029ff47` at start and at end, parent `b0ac8de720fddb2ae784ed04c0e96e22ab0f4eed`, and `git status` was clean. The installed `remote-gates.zsh`, `.md` and `lanes-1024-common.md` are byte-identical to `tools/staging-gate-fixes/`. The diff against the archive shows only gate-fixes edits.

**Read:** the full 7-file diff; `run_tests.py` main and the pin/session functions; `check_lane_commits.sh`; the `local-clone-boundary` job's library step; `remote-gates.zsh` (runner, `gate()`, sync); `win-receipt.zsh`; `CHANGES.md`; the brief; and the drafter's logs in `build-targets/gate-fixes-logs/`.

**Run (read-only, `-B`):**
- `python3.13 -B -m unittest scripts.test_windows_lane_check scripts.checks.test_run_boundary_job scripts.checks.test_run_tests_session_crates` gives 108 tests OK. The same 108 pass under `python3.11` (CI's version).
- The neighbouring Windows tests (parity, receipts, qualification boundary, candidate switches, workflow siblings) give 112 tests OK.
- `zsh -n remote-gates.zsh` is clean, and `bash -n` of the embedded runner heredoc is clean.
- `check_windows_parity.py` exits 0.
- The unmodified `windows_lane_check.transport_archive_command()` run in the lane's real `gwz-transport` gives a 4.9 MB tar of 976 entries: tracked files, 836 `.git/` entries, and nothing under `target/`.
- Two probes of my own, described under P2-1 and P2-2. The first is a `checkout-index` run into the scratchpad with a faked host global config. The second runs the library-step script through the module's own `run_bash` with a shell-function `cargo`.

**Drafter logs.** The red runs are genuine (`fix1-red.log`, `fix2-red.log`, `fix3-red.log`). `fix4-lane-commits-demo.log` shows the four cases A–D. `fix5-*` shows the old and new check compile the same tree.

**WSL gate `gate-fixes-20261030-a`.** The final `summary.txt` is not present. `gate-fixes-logs/remote-launch.log` shows `lane-commits` PASS (10 s), `run_tests` PASS (2533 passed, 150 s), `boundary` PASS (602 tests, 115 s), `candidate-lib-test-profile` PASS (40 s), and `candidate-lib-plain` started at 07:00:40Z. It covers the happy path only and cannot see P2-1 or P2-2.

**Not available:** no Windows receipt exists for this tuple. The commit message already carries `Windows-receipt: gate-fixes-20261030-a`. The new staging code has therefore never run against the real host.

## Problem, requirement provenance, and scope

| Fix | Requirement and authority | Verdict on scope |
|---|---|---|
| 1. Receipt stages the lane's gwz-transport | Brief item 1. The fetch site was `SIBLING_PINS` in `stage_script` (a GitHub `git fetch` of the pin). The brief says "tracked files, like the other members", and "fails unless an explicit flag allows it... say which you chose". | Refusing on HEAD != pin is the brief's own judgement clause and matches `run_tests.py`'s existing policy. Refusing on tracked changes is an extension that traces to the brief's "would certify code CI does not build" rationale. Both are acceptable. Staging untracked files departs from "tracked files" (P3-1). |
| 2. Three session crates, run unconditionally | Brief item 2. | In scope. The list equals `crates/session-*`. |
| 3. Library step in `run_boundary_job.py` | Brief item 3, "mirror the workflow's exact commands". | In scope. The mirror is not exact (P2-2). |
| 4. `lane-commits` gate | Brief item 4. | In scope. |
| 5. Retire `gwz_session_candidate` | Brief item 5. | In scope. gwz-core has no live naming left; only retired-switch assertions in tests and history docs remain. |

Out-of-scope observations (not defects of this change, not prerequisites):
- `gwz-cli` and `gwz-py` still name `gwz_session_candidate` (inventories, tests, `gwz-py/src/tests/test_ci_workflows.py`). Several one-off scripts under `build-targets/` still pass it: `capacity-gates.sh`, `fixA-gates.sh`, `decide-legs.sh` and others.
- `CHANGES.md` says `win-receipt.zsh` needs no change; see P3-3.

## 1. Findings

### [P2-1] The Mac's whole `.git` is shipped to the host, so the "Windows-style" checkout is LF, not CRLF

- **Location:** `scripts/windows_lane_check.py`. `transport_archive_command()` adds `printf '.git\0'`. `stage_script(..., host_checkout=True)` then runs `git -C .../ws/gwz-transport checkout-index -a -f`, and its docstring and the `transport_facts` print claim "CRLF under the host's core.autocrlf".
- **Violated invariant:** the receipt must build what a hosted Windows job builds. Hosted Windows has `core.autocrlf=true` and CRLF checkouts. The CRLF owner export of `protocol/transport.ir.json` was the S-P2-1 failure class (`findings-2026-10-25.md` item D). The old path got CRLF from a fresh `git init`, fetch and checkout on the host. The `HOSTED_LIKE_OWNER_EOL` probe exists for this reason.
- **Reproduction.** The lane's `gwz-transport/.git/config` contains `autocrlf = false` and `eol = lf`, and is archived verbatim, so it becomes the host repo's local config. Repo-local config overrides global and system config.
  1. Run `GIT_CONFIG_GLOBAL=<file with autocrlf=true> GIT_CONFIG_SYSTEM=/dev/null git checkout-index -f --prefix=<scratch>/ Cargo.toml protocol/transport.ir.json` inside the lane's gwz-transport.
  2. `git config --show-origin core.autocrlf` reports `file:.git/config false`.
  3. The written files contain 0 CRs (`grep -c $'\r'` gives 0).

  The host step is therefore a no-op for line endings. The receipt will report `HOSTED_LIKE_OWNER_EOL lf` and still PASS. `test_a_clean_tree_is_written_again_by_the_hosts_git_as_a_windows_checkout_is` only asserts that the script text contains `checkout-index`, so it cannot see this.
- **Impact:**
  - The Windows receipt loses its CRLF coverage and can pass code that hosted Windows CI fails. This regresses the old fetch-based behaviour on the exact point the drafter documents as preserved.
  - Shipping the entire `.git` also copies the lane's `config` (remote URLs, any `credential.*`, `includeIf`, `core.fsmonitor` or `core.hooksPath`) to the shared host. This lane's config holds only an SSH remote URL, but nothing bounds other lanes.
  - A lane whose `.git` is a gitfile, or uses `objects/info/alternates`, would stage a dangling pointer. The host then fails in `checkout-index` or `rev-parse` with an opaque error. This lane has neither, so I did not reproduce it. There is no preflight.
- **Required correction:**
  - Do not ship the Mac `.git` as is. Either build the host repo minimally, or archive `.git` without `config` and let the host's own config apply. The suite needs only `HEAD` to resolve, and `checkout-index` needs the index and the objects.
  - Alternatively, ship the files and set `core.autocrlf` explicitly on the host from its global config.
  - Refuse in `transport_facts` when `.git` is not a plain directory, or when `objects/info/alternates` exists.
  - Fail or loudly flag the receipt when the clean-tree path ends with `HOSTED_LIKE_OWNER_EOL lf`.
- **Closure test:**
  1. A unit test that actually executes `transport_archive_command()` and the host checkout step against a temp repo whose local config says `autocrlf=false`, under a global `autocrlf=true`, and asserts CR bytes in a tracked text file.
  2. A test that the archived tree contains no `.git/config`, or a sanitised one.
  3. Preflight refusal tests for a gitfile and for alternates.
  4. On the real receipt, `HOSTED_LIKE_OWNER_EOL crlf`.

### [P2-2] The library step runs without errexit, so a failing crate can pass the gate

- **Location:** `scripts/checks/run_boundary_job.py`, `run_bash()` uses `['bash', '-c', wrapped]` for the library script.
- **Violated invariant:** the brief requires mirroring the workflow's exact commands. The step has no `shell:` key, so Actions runs it as `bash -e {0}`. There, `test -n "$names"` aborts an empty list and any failing `cargo test`, `--doc` or `clippy` aborts the job. Locally nothing aborts, and the script's exit status is that of the last command (the final crate's clippy, or 0 for a zero-iteration `for`).
- **Reproduction** (from the lane, using the module's own `run_bash` and the real workflow text):
  - A `cargo` shell function that fails the first crate's `--lib` test, prepended to the real library script, exits **0**.
  - `names=""` exits **0**.
  - Only a failure in the last command exits 1.

  Hence a red crate-A `cargo test --lib`, `--doc` or clippy is invisible unless the last crate's clippy also fails. The same holds if `check_local_clone_boundaries.py --list-present` crashes: the gate "passes" having tested zero crates.
- **Impact:** the feature exists to catch what the core groups miss (doctests and clippy of the other 15 crates). In practice it fails open for 17 of 18 crates. `test_a_failing_library_script_fails_the_run` patches `subprocess.run` and returns exit 1, so it tests Python plumbing, not the shell semantics. The 602-test WSL PASS cannot discriminate.
- **Required correction:** run the library script as Actions does, with `bash -e`, so `test -n` and failing cargo calls abort. Keep it a single script so the loop stays whole. Also confirm that `names="$(python …)"` aborts under `-e` when `python` fails.
- **Closure test:**
  1. A test that runs the real extracted script with a stub `cargo` function (as in my reproduction) and asserts a non-zero exit when the first crate fails.
  2. The same when the list is empty.
  3. The same when the `--list-present` call fails.

### [P3-1] Untracked files are staged from gwz-transport and are only reported

- **Location:** `windows_lane_check.py`, `worktree_listing(None, …)` and `transport_facts`.
- **Issue:** the brief says "tracked files, like the other members it copies". In `--ref` mode gwz-core is a `git archive` (committed content only), but gwz-transport includes any untracked, unignored file, which is only printed and recorded as `untracked`. With HEAD == pin and no tracked changes, an untracked file can still alter a Cargo build. Cargo auto-discovers `build.rs`, `src/bin/*`, `tests/*.rs` and `examples/*`, and a tracked file can reference a module that was never committed.
- **Consequence:** the receipt can pass on a tree CI cannot build. Mitigation exists in the recording and the printed list.
- **Correction:** stage tracked files only unless the mode is `--worktree`, or refuse (or require a flag) when the transport tree has untracked files and the core source is a ref. Alternatively record the decision explicitly as an operator choice.
- **Closure test:** a test with a status listing containing `??` in `--ref` mode, asserting refusal or exclusion from the archive.

### [P3-2] A PIN MISMATCH receipt is indistinguishable from a clean one at the point of use

- **Location:** `windows_lane_check.py`, `execute()` end and `Receipt.passed()`.
- **Issue:** the mismatch is printed once, before the host is touched, ahead of a run that can last about 20 minutes. The final line is still `receipt: … (PASS)`, and top-level `passed` is true. `check_windows_receipts.py` never reads `receipt.json`, so the `Windows-receipt: <label>` trailer is accepted either way. The JSON does carry `matches_pin: false` and `pin_mismatch_allowed: true`. The brief asked for "recorded loudly".
- **Correction:** repeat it in the final line (for example `(PASS, PIN MISMATCH allowed)`). Optionally make `check_windows_receipts.py` surface `matches_pin: false` for a trailer's label when the receipt directory is available.
- **Closure test:** the final output line for the allowed-mismatch case contains `PIN MISMATCH`.

### [P3-3] A refusal happens after the single Windows slot is booked

- **Location:** `tools/win-receipt.zsh` (unchanged) with the new `Refused` in `execute()`.
- **Issue:** the wrapper polls for `BUSY.lock` (up to 150 × 2 min), and only then runs `windows_lane_check.py`. The new pin/dirty refusal fires after the booking, so a lane whose gwz-transport is off-pin queues for hours and then fails within seconds. `CHANGES.md` states "win-receipt.zsh needs no change". The slot queue is the known bottleneck (`windows-receipt-warm-slot.md`).
- **Correction:** run the transport preflight before booking, for example a `--preflight-only` mode or an equivalent `windows_lane_check.py` call from the wrapper. If you decide to keep it as is, state the cost in `remote-gates.md`.
- **Closure test:** a wrapper run with an off-pin tree exits 2 without touching `BUSY.lock`.

## 2. Invariant analysis

Held:
- **Fix 2 aggregation.** `run_session_crates` runs every crate and keeps the first non-zero code. `main` runs it after all core groups. Fail-fast `SystemExit`s before it are preserved when `--no-fail-fast` is absent. `--test-filter` runs skip it, as documented. `SESSION_CRATES` equals the three `crates/session-*` packages, guarded by a manifest-reading test. The `Aggregate` tests drive the real `main` and were red against the old code. No Windows-specific code is added. The two newly run crates (channel, contract) are untested on Windows until the receipt runs (see Section 3).
- **Fix 1, other cases.** An absent, unreadable or non-git tree is refused before any host call, with a message naming `--transport-tree`. A symlinked directory works: `git -C` and `cwd=` follow it. The pin is read from the ref being receipted (`git show REF:…`) or from the working-tree file. Core and transport pins are consistent with `run_tests.py`'s 40-hex first-line rule. `--allow-transport-pin-mismatch` is plumbed to the hosted suite only when HEAD != pin, which is correct. The staged archive is well formed. Submodules and tracked symlinks: gwz-transport has none.
- **Fix 3, other parity points.** Working directory is `ROOT`, matching `working-directory: gwz-core`. The `python` shim matches the existing steps. `--skip-library` and `--skip-clippy` print `SKIPPED GATE` and keep the loop whole. A renamed step fails loudly (`SystemExit`). The crate list comes from `--list-present` (18 packages), which equals the 18 directories under `crates/`. The module docstring says "every crate under crates/", but the loop actually walks the inventory's present packages; they coincide today. The workflow-level `GWZ_TEST_GIT: real` is not exported, and no crate reads it. `RUSTUP_TOOLCHAIN` is documented, not enforced, as for the existing clippy line.
- **Fix 4.**
  - Range: `origin/main..HEAD`, fork point printed.
  - Staleness: an old `origin/main` only widens the range to already-pushed commits; a lane ahead of main is judged correctly.
  - Exit propagation: no `set -e` is needed because `bash -c` returns the script's status. `gate()` records FAIL, sets `OVERALL=1`, and the summary lists it. `check_lane_commits.sh`'s usage exit cannot be misread as NEEDS-ARGS (no "required" in its text).
  - WSL copies: the rsync carries `.git`. Files are root-owned, and the run showed 10 s PASS under the dropped-capability wrapper.
  - Quoting: the script is single-quoted inside a quoted heredoc, with no embedded apostrophes.
  - A missing `origin/main` ref fails with a fetch instruction.
- **Fix 5.** The rename to `candidate-lib-test-profile` still compiles `cfg(test)` code. Cargo documents that `cargo check --profile test` enables test mode. The old command and the new one compile the same tree to the same 10 warnings (logs `fix5-*`). `check_candidate_switches.py` refuses any live naming in gwz-core. The description in `remote-gates.md` matches the code.
- **Tests (Python).** The drafter's red logs are genuine. Fix 2's tests exercise `main`. Fix 1's and fix 3's mock `subprocess.run` and assert script text, so they do not exercise the shell or git behaviour where P2-1 and P2-2 live.

Broken: the host-side line-ending parity (P2-1) and the shell-level mirror of CI's `bash -e` (P2-2). Weakened: the "tracked files" and "loud" mismatch requirements (P3-1, P3-2), and the slot-queue cost (P3-3).

## 3. Risks and next action

1. **Revise and re-review.** Close P2-1 and P2-2 with their closure tests, then fix or record P3-1..P3-3. I will return GO on that revision. If the revised `windows_lane_check.py` changes shape, re-run its unit tests under python3.13 and python3.11.
2. **Receipt after revision.** Run the Windows receipt with a fresh label (`--hosted-like --taut-proto …`) and confirm `HOSTED_LIKE_OWNER_EOL crlf` and `transport.matches_pin: true`. The commit message's `Windows-receipt: gate-fixes-20261030-a` is a placeholder for a receipt that has not run, so its label must match what is actually run.
3. **Windows load.** Two more session crates (channel, contract) now run under `run_tests.py` in the hosted-like suite and the unfiltered Windows jobs, including clippy. The hosted-like watchdog limits (no-progress 1200 s) should be watched on the first receipt.
4. **WSL summary.** The WSL `summary.txt` for `gate-fixes-20261030-a` was not present when I finished; I read the in-flight progress log. Cite the final summary once it lands.
5. **Hygiene.** Out-of-scope observations (sibling repos and old one-off scripts that still name the retired switch) need no action for this deliverable.
