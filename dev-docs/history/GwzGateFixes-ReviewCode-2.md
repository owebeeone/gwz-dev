# Gate fixes — CODE-AXIS REVIEW (re-verdict, round 1)

**Review object:** gwz-core `b0ac8de7..dc192389` (HEAD `dc192389534874f441c7ae53698df84558dbd934`, parent `b0ac8de7`; the lane commit `94f23ab2` was amended, unpushed). Also the installed tools `win-receipt.zsh`, `remote-gates.md`, `lanes-1024-common.md` (and `remote-gates.zsh`), and the wrapper test `staging-gate-fixes/test_win_receipt.py`.
**Baseline:** prior reviewed revision `94f23ab2`; the NO-GO report filed as `dev-docs/GwzGateFixes-ReviewCode.md`.
**Date:** 2026-10-30 **Axis:** Code (re-verdict; filed as `GwzGateFixes-ReviewCode-2.md`)
**Verdict: NO-GO** — P0 0, P1 0, P2 1 (new), P3 0 open. P2-1, P2-2 and P3-1..P3-3 are closed. I pre-commit to GO on a revision that closes P2-3 below.

## Prior-finding closure table

| ID | Prior finding | Status | Evidence |
|---|---|---|---|
| P2-1 | Mac `.git` shipped; host checkout stays LF | **Closed mechanically.** A new defect (P2-3) sits behind it. | I re-ran the original probe on the new code. The unmodified `transport_archive_command()` run in the lane's real gwz-transport gives 962 entries and 4.9 MB. It holds no `.git/config`, `hooks`, `logs`, `info`, `target` or `__pycache__`, and it does hold `.git/HEAD` and `.git/index`. I unpacked it into the scratchpad and wrote `TRANSPORT_HOST_CONFIG`. The resulting config has only `repositoryformatversion` and `bare`. `git rev-parse HEAD` resolves to `96938616…`. With a host global `autocrlf=true`, `checkout-index -a -f` writes CRLF (34 CR lines in `Cargo.toml`). With the global unset it writes LF (0 CR). Gitfile, symlink, missing-`.git` and alternates are refused by name before any host call (`check_transport_git_dir`, 3 new tests). The new `RealTransportTree` class executes the archive and the checkout against a temp repo with local `autocrlf=false` and a global `autocrlf=true`. |
| P2-2 | Library step ran without errexit, so a failing crate could pass | **Closed.** | I re-ran the original stub-`cargo` probe through the module's own `run_bash(script, errexit=True)` on the real workflow text. First crate's `--lib` fails: exit 1 (was 0). Empty list: 1 (was 0). `--list-present` failing: 1. A `--doc`-only failure mid-loop (gwz-refcopy): 1. All pass: 0. `main` calls `run_bash(script, errexit=True)`; the single-line check steps keep the default. 4 new shell-level tests cover first-crate, control, empty list and failing list. |
| P3-1 | Untracked transport files staged in `--ref` mode | **Closed.** | `stage_untracked=options.worktree` selects `git ls-files --others`. In `--ref` mode untracked files are not archived and the output says so. In `--worktree` mode they are staged and recorded (`untracked_staged`). The archive probe above shows the same entry count only because this lane's transport has no untracked files. Two tests assert both modes. |
| P3-2 | PIN MISMATCH invisible at the end | **Closed.** | The final line is `(PASS, PIN MISMATCH allowed)`, and likewise for FAIL and TIMEOUT, driven by `transport['pin_mismatch']`. Test added. |
| P3-3 | Refusal came after the slot was booked | **Closed.** | `--preflight-only` returns after `transport_facts` and before any host call. `win-receipt.zsh` runs it before the `BUSY.lock` loop. A refusal exits with the check's status (2) and logs the reason. A lane whose check lacks the option is detected by `grep` and books as before. The installed wrapper equals the staged one, and `zsh -n` is clean. `python3.13 -B -m unittest test_win_receipt` gives 5 OK. |

## Changed-range analysis (`94f23ab2..dc192389`, 4 files, +376/−43)

- **`run_boundary_job.py`:** adds `errexit` to `run_bash`; only the library step passes it, matching Actions' `bash -e` for a step with no `shell:`. Nothing else changed. The docstring is updated.
- **`windows_lane_check.py`, archive and host side:**
  - `transport_archive_command(untracked)` builds one `find .git … -prune -o -print0` listing and archives with `tar --no-recursion`.
  - `transport_host_lines` writes the minimal config and then runs `checkout-index`.
  - `check_transport_git_dir` runs first in `transport_facts`.
  - Residual: the minimal config sets no `core.filemode`, `symlinks` or `ignorecase`, so git's defaults apply. That is harmless for gwz-transport, which tracks no symlinks. A partial-clone lane (promisor objects absent) would fail in `checkout-index`; no such lane exists, and I did not reproduce it.
- **`windows_lane_check.py`, receipt side:** adds `--preflight-only`, the `pin_mismatch` and `untracked_staged` facts, and the final-line note. It also adds the new gate `require_crlf=transport['host_checkout']`, so a clean-tree `--hosted-like` receipt fails unless `HOSTED_LIKE_OWNER_EOL` is `crlf`. This gate is the source of P2-3.
- **Tests:** 125 tests (3.13) and 118 (3.11) pass, up from 108. Both new real-process classes run the real archive and the real `bash -e`, not mocks. The red-before-green claims for the wrapper and the shell probes are credible given the live-wrapper red log.
- **Tools:** `win-receipt.zsh`, `remote-gates.md` and `lanes-1024-common.md` are installed and byte-identical to staging.
- **Architectural root cause (new, labelled as such):** the new CRLF gate encodes an environmental assumption about the Windows host's Git configuration that no recorded evidence supports. The code is mechanically correct against a host whose global `core.autocrlf` is `true`; the host's actual state was never checked. See P2-3.

## 0. Evidence base

**Tuple.** `dc192389534874f441c7ae53698df84558dbd934` at start and at end, parent `b0ac8de7`. `git status` was clean.

**Run (read-only, `-B`):**
- `python3.13 -B -m unittest scripts.test_windows_lane_check scripts.checks.test_run_boundary_job scripts.checks.test_run_tests_session_crates` gives 125 OK. The same two changed modules under `python3.11` give 118 OK.
- `test_win_receipt` gives 5 OK, and `zsh -n win-receipt.zsh` is clean.
- The three probes described in the closure table.

**Drafter's logs** `rem1-*` (red then green for fix 1, fix 2 and the wrapper; unit runs on 3.11 and 3.13; the real-lane preflight and archive probe) are consistent with my reproductions.

**Fleet history.** `grep` over all 19 recorded receipts that carry `owner_export_line_endings` (`/Volumes/projects/limbo/build-targets/*-receipts/*/receipt.json`, 2026-10-25 to 2026-10-30, all on the dabeest host) shows `"lf"` in every one. They all used the earlier fetch-based staging, with git `/c/Program Files/Git/cmd/git` 2.55.0.windows.3.

**Remote and Windows runs.**
- Gate `gate-fixes-20261030-a` (previous tuple) finished with OVERALL PASS: `run_tests` 2533 passed, `boundary` 602, `candidate-test` 4049. It does not speak to the current tuple.
- Gate `gate-fixes-20261030-b` (current tuple) has started (`remote-b.log`); no summary exists yet.
- `win-b.log` and `run-b.status` do not exist yet. The Windows receipt for `-b` is therefore unobserved. I cite no result for either.

## Problem, requirement provenance, and scope

- **Authority and deferrals.** Unchanged from round 1. The round-1 remediation plan's fixes trace to the NO-GO findings. `--preflight-only`, the final-line note, the `.git` pruning and the preflight refusals are remedies for the named findings, not widened scope. Out-of-scope items (other lanes, the gwz-py pin gate, the Windows row-count step, the foreign-target clippy step, sibling repos naming the retired switch) are untouched.
- **New scope-changing constraint.** "A clean-tree hosted-like receipt fails unless the owner export is CRLF" is not in the brief. It traces to the round-1 P2-1 closure test ("the real receipt shows `HOSTED_LIKE_OWNER_EOL crlf`"), which I wrote. That closure test presumed the host produced CRLF. The receipts show it does not. A requirement of mine was converted into a hard fail without checking the premise (see P2-3).
- **Correction to round 1.** My P2-1 impact text said the receipt "regresses the old fetch-based behaviour on the exact point the drafter documents as preserved". The recorded evidence shows the old path also gave `lf` on this host in all 19 receipts. P2-1 as a defect stands: the shipped Mac config could never yield CRLF, and it could carry credentials or break on alternates. But the CRLF coverage was never actually exercised by receipts before this change. My severity framing there overstated the regression.

## 1. Findings

### [P2-3] The new CRLF gate fails every clean-tree hosted-like receipt on the current host (NEW; architectural root cause)

- **Location:** `scripts/windows_lane_check.py`. `run_entry(..., require_crlf=transport['host_checkout'])` sets `entry['passed'] = False` unless `owner_export_line_endings == 'crlf'`. `TRANSPORT_HOST_CONFIG` writes a config with no `core.autocrlf`, so the host's own config decides.
- **Violated invariant:** a gate must test a property the host can produce, and its closure must be shown on the real host. The gate assumes the dabeest Git has `core.autocrlf=true` globally or at system level. Recorded evidence says it does not.
- **Reproduction (evidence-based):**
  1. All 19 existing receipts on this host record `owner_export_line_endings: "lf"` (list in section 0).
  2. The same checkout step now produces CRLF only if the host's global/system config has `autocrlf=true`. My local probe shows LF with the global unset.
  3. With nothing in the minimal config to change that, the in-flight receipt `gate-fixes-20261030-b` (`--worktree --tests --hosted-like`, clean transport tree, so `host_checkout` is true) is predicted to end `FAILED line endings: HOSTED_LIKE_OWNER_EOL is 'lf'`.

  This is a prediction from the 19-receipt record; it is unconfirmed until `win-b.log` lands. A separate possibility, that the probe `grep -q "$(printf '\r')"` itself misreports CRLF under MSYS grep, is equally unproven. It gives the same remedy, because the gate then cannot be trusted either way.
- **Impact:** every `scripts/`-touching lane's required Windows receipt goes red with a message that blames the lane's gwz-transport checkout. It queues the one slot for a failure no lane edit can fix. The unit test `RealTransportTree` sets the host global to `true` itself, so it cannot detect the mismatch.
- **Required correction (choose one and say which):**
  - **(a)** Make the host's behaviour explicit and honest. Add `autocrlf = true` to `TRANSPORT_HOST_CONFIG`, mirroring the hosted runner image's system config (`windows-matrix.yml` documents `core.autocrlf=true` at SYSTEM). Then CRLF is guaranteed by the script and the gate is meaningful. Re-run the real-process test with the host global unset, expecting CR bytes. First check for new CRLF-only failures on the host; the owner-pin normalisation fixed S-P2-1, but other transport-tree readers may exist.
  - **(b)** Drop `require_crlf` to record-only (keep `owner_export_line_endings` in the entry) until the host's Git config is verified and fixed by the operator.
- **Closure test:**
  1. A real-process test: archive and checkout into a temp "host" with global `autocrlf` unset (and then `true`), asserting CR in a tracked text file under the chosen option.
  2. The real receipt `gate-fixes-20261030-b` (or its successor) showing `HOSTED_LIKE_OWNER_EOL crlf` (option a), or passing with `lf` recorded and no gate on it (option b).

## 2. Invariant analysis

- **Errexit parity (P2-2):** the local library step now fails exactly where the Actions step fails. Verified at shell level on the real workflow text, not only in mocks.
- **Host repository fidelity (P2-1):** the host repo has a minimal config, a valid HEAD and the real objects and index. Nothing of the Mac's config, hooks, logs or info is shipped. Gitfile, symlink, alternates and missing-`.git` trees are refused by name before the host is touched. The suite's pin check still reads HEAD.
- **Source fidelity (P3-1):** `--ref` stages tracked files only; `--worktree` stages and records untracked ones.
- **Loudness (P3-2) and queue cost (P3-3):** the mismatch note reaches the last line; refusals no longer cost a slot wait.
- **Broken:** only the CRLF gate's premise (P2-3). The gate is enforced on a clean tree and is therefore the sole path through which an environmental assumption can fail an otherwise-good receipt.

## 3. Risks and next action

1. Close P2-3 (option a or b above) and re-review. I will return GO on that revision.
2. Do not wait on the in-flight Windows receipt `gate-fixes-20261030-b`. If it finishes before the revision, its log is the confirming or refuting observation for P2-3. If it shows `crlf`, withdraw P2-3 and replace it with a note recording the host's actual config.
3. Watch the first receipt for the host-config and watchdog effects of the two newly run session crates under hosted-like (carried over from round 1).
4. The WSL gate `-b` (current tuple) was still running when I finished; cite its summary once it lands.
5. Out of scope, recorded only: sibling repos and old one-off scripts under `build-targets/` still name `gwz_session_candidate`.
