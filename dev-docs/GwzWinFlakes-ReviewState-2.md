# Windows flake hardening — STATE-AXIS RE-VERDICT (round 1)
**Review object:** gwz-core `298b0783` (parent `b0ac8de7`, amended from `7db78119`) and gwz-core-evidence `f9f67799` (amended from `973168e9`), run `campaigns/transport-qualification/runs/2026-10-30-win-flakes/`. **Baseline:** `7db78119` for the changed range, `b0ac8de7` for the whole object. **Date:** 2026-10-30 **Axis:** State.
**Verdict: GO** — P0 0, P1 0, P2 0, P3 3 open (R-1 to R-3, all recorded or accepted). This discharges the pre-committed GO on S-1.

## Prior-finding closure table
| ID | Prior | Disposition | Verification | Status |
|---|---|---|---|---|
| S-1 | P2 | Bound restored: `EXIT_BOUND = 2.5 s`, with a justification against `OPEN_CLEANUP_MS`. Doc wording corrected. The ~5 s read window is documented. | My probes on the new tree, listed below. | **Closed**, residual in R-1 |
| S-2 | P3 | `kill_tree` now uses `Stdio::null()` for stdout and stderr with `.status()`, in both cfg variants. | The row is green on Mac and on dabeest (102/102). `rem1-red-killtree` is red 3/3 against the previous killer. | **Closed** |
| S-3 | P3 | The completion `expect` is dropped. The helper now returns `()`. | Probes (below) and dabeest loops (1199/1200 heavy). The "no baseline" claim is honestly stated. | **Closed**, residual is F-1 (see R-3) |
| S-4 | P3 | Accepted and recorded under Limitations. | The README has the entry. | **Accepted** |
| S-5 | P3 | The guard now separates `Timeout` from `Disconnected`. The `kill_tree` row now has a root plus a child. | A forced panic gives the new message. The root-only killer fails the row. | **Closed** |

## Changed-range analysis (`git diff 7db78119 298b0783`: 4 test files, +91 −40)
- **`close_tests.rs`.**
  - `EXIT_BOUND` (2.5 s) is now the `recv_timeout` on the exit. `Timeout` and `Disconnected` are reported separately.
  - The gate is opened before either failure is reported, so a failure does not wait for the close. A thread panic no longer misreports as "waited".
  - The doc claim that pool `cleanup_timeout_ms` can only tighten the open's 5 s bound is correct. `validate_deadlines` / `tightens` requires `value <= configured`, and `network_deadlines` hard-codes `OPEN_CLEANUP_MS = 5_000`.
- **`ssh_controlled_server.rs`.** Doc-only. It now says a client may end a held close sooner, at 5 s after the handoff. This corrects my original wording finding.
- **`kex_retry_tests.rs`.** The helper's return value and the `expect` are removed; the `settled` assertions are untouched.
- **`https_worker/native/tests.rs`.**
  - The `kill_tree` variants silence the killer's output instead of capturing it. `.status()` returns the exit code and the null devices cannot block, so exit semantics are unchanged and the fix stays correct.
  - The row is now a driver, killer, root and leaf. The killer holds the pipe the leaf inherits, waits for a READY line before killing, then reads to EOF with a 60 s bound. EOF arrives only when both processes are gone.
  - The Windows-only logic remains behind `cfg_if!`, and no `#[cfg]` sits on an item.
- **Authorisation.** All four files are within the brief's items 1 to 3, or fix my filed findings. Nothing was added outside the plan, so scope did not widen.

## 0. Evidence base
- **Tuple.** gwz-core `298b0783` and gwz-core-evidence `f9f67799` were the same at the start and the end, both clean. The evidence repo shows only the unrelated untracked `2026-09-22-alpha-setup-timeout/`.
- **Focused re-run.** I rebuilt a throwaway copy of the lane tree (outside the lane, since deleted). With the probe env unset, the close row, the kex row and the `kill_tree` row pass. I did not re-run the `ssh_controlled_server` fixture rows; they are unchanged apart from a comment.
- **Probes** (copy of the lane tree with env-gated edits):

  | Probe | Result |
  |---|---|
  | stand-in: 4000 ms before `finish()` | **fails**: "had not finished 2.5s after it began … it waited for the close" |
  | 4000 ms before the count read | passes (within the ~5 s window) |
  | 6000 ms before the count read | fails on the count, `(0, 0)` |
  | 2000 ms wait | passes (below half the bound, by design) |
  | product that waits until 5 s after the handoff | fails |
  | the same, with a 3 s stall before the exit begins | **passes** (the implementer's documented limit) |
  | panic in the exit's thread | "its thread panicked … not a wait for the close" |
  | `settled` never reruns / capped at 2 / one extra run | all fail the row; unmutated passes |
  | killer that ends only the root | fails after 120 s ("the root's child outlived kill_tree") |
- **Evidence cross-checks.**
  - `raw/rem1.diff` equals `git diff 7db78119 298b0783` (ignoring `index` lines), and `raw/lane.diff` equals `git diff b0ac8de7 298b0783`.
  - The 174 files in `manifest.json` all hash correctly, with none unlisted.
  - The sha256 of the four changed files in `tree-final.txt` match the lane files.
  - The dabeest loop counts match the README. `rem1-loop-close` ran 102 (median 678 ms), `rem1-loop-close-heavy` 600, `rem1-loop-kex` 102, `rem1-loop-kex-heavy` 1200 with 1 failure, and `rem1-loop-killtree` 102, all with 0 failures except the heavy kex loop.
  - `rem1-red-killtree` is red 3/3 against the previous killer. The log shows taskkill output naming both the root and its child, which also proves the grandchild is now exercised.
- **Not checked** (queued, out of scope): the Windows receipt `win-flakes-20261030-b` and the WSL matrix.

## Problem, requirement provenance, and scope
Unchanged. The brief's items 1 to 3 cover the changes, and the round-1 edits implement my filed findings. The only item outside the plan is F-1, judged under R-3.

## 1. Findings
No open P0, P1 or P2. Three P3 notes.

### R-1 (P3): the exit bound has two documented blind spots, and one comment is stale
- **Judgment on the implementer's note.** Starting the 2.5 s bound at the exit's start, rather than at `close()`, is acceptable. A product that waited is caught only if the exit began under 2.5 s after the handoff. The probe confirms the limit: with a 3 s stall before the exit and a product that waits to 5 s, the row passes. That needs both a regression and a ≥2.5 s stall, and the alternative would false-red healthy runs on any stalled thread. It is recorded in the README Limitations and the doc comment. The count assertion still catches stalls past ~5 s.
- **Stale comment.** The `EXIT_BOUND` comment says the whole test "took 1.6 s at most in 102 runs". The new `rem1-loop-close` shows a maximum of 4657 ms. The six slowest runs, 4.4 to 4.7 s, are the first six of the width-6 batch, so cold start is the likely explanation. That is my inference; the evidence does not attribute them. Reword the comment or attribute the figure.

### R-2 (P3): S-2 has no behavioural red on pipe creation, accepted
- No red can exist for pipes that are created and drained. The change is a mechanical simplification and keeps the Windows red against the original killer.

### R-3 (P3): F-1 — the kex row's unchanged last scenario still ends early on a non-kex connect error (1/1200 heavy)
- **Judgment.** F-1 belongs to this step. It is the same function the brief names for item 2, on the same cause class as my S-3 (an environment error ending a run before its stand-in), and the fix is a test-only assertion with no retry, sleep or skip. It is not a prerequisite for GO, because it is P3, the rate is about 0.1% and only at 1.5× oversubscription, and it does not touch the hosted failure that was fixed.
- **Why it fails.** `run-536.log` shows `assert_eq!(runs, TRIES)` seeing 2 runs when a real connect ended `Uncategorized`.
- **Suggested fix.** Replace `runs == TRIES` with `runs <= TRIES`, and require `runs == TRIES || the last run's real_kex() == 0`, which is what the new row asserts for the other cases. Keep an assertion that an early end carried no real −8.
- **Closure test.** With `settled` mutated to stop early after a real −8, the scenario must still fail.

## 2. Invariant analysis
- **Gate and ordering.** Unchanged and still sound: the gate task wakes the session loop on release, the hold is set when the EOF is processed, and a never-released gate leaks nothing. The only fixture change is a doc comment.
- **Fail-closed direction.** The close row now fails when the exit waits ≥2.5 s after it begins, and the count assertion fails when the read is stalled past ~5 s. Both failures are reported without hanging, and the gate is opened before the message. Residual false-green: a regression whose wait is under 2.5 s, or the stall case in R-1.
- **Kex row.** It still kills all three `settled` mutations and no longer asserts host health for the last run.
- **`kill_tree` row.** It now exercises a descendant. On Unix the leaf shares the root's process group, so `kill -pgid` reaches it. On Windows `taskkill /T` reaches it, and the red log shows both PIDs terminated.
- **Repository rules.** Braces, `cfg_if!` and no-globals conventions are followed. No new `#[cfg]` is attached to an item. `kex_retry_tests.rs` is at 907 lines, below the 1000-line threshold.

## 3. Risks and next action
- Update the stale `EXIT_BOUND` comment (R-1). Decide on F-1 (R-3), which I recommend including as one assertion change. Neither blocks the GO.
- Run the queued Windows receipt (`win-flakes-20261030-b`) and rerun the close-row dabeest loop on the amended tree.

## 4. Out-of-scope observations
- The 5 s per-open cleanup is a hard-coded constant (`OPEN_CLEANUP_MS`), so bounded-wait rows cannot be made stronger by test configuration.
- The `kill_tree` root-only mutation takes the full 120 s to fail because the orphaned leaf holds the pipe open. That is acceptable for a regression signal, but a bounded read would fail faster.
- S-4's 103-minute compile remains unexplained and is on the follow-up list.
- `E:/gwz-tests/wflk-loop-20261030-a` on dabeest is left for the lane owner to remove.
