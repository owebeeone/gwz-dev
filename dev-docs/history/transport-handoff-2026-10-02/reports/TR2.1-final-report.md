All five fixes are committed in the lane, each test-first, and every gate you listed passes on the final tree. Three points need a decision from you:
- **Merge refusal code:** the merge refusal (item 4) uses MergeValidationFailed, not the InvalidRequest you named.
- **gwz-cli formatting:** `cargo fmt --check` fails on one file from main.
- **One transport-leg rerun:** the transport leg needed a rerun because of machine load.

## Commits
| Fix | gwz-core | gwz-cli | root |
|---|---|---|---|
| 1. `--ssh-timeout` / `--max-retries` help | — | `5f2c159` | `3c9858e` |
| 2. Refuse `--max-retries` on the Cli placement (P3-1) | `6ac41f6` | — | `73615b4` |
| 3. Refuse `--max-retries` above u32::MAX (P3-2) | `2758db3` | `0164e66` | `8c511b2` |
| 4. Refuse `--max-retries` on merge (P3-3) | `828093f` | — | `e6df053` |
| 5. Backstop counts each attempt's 5 s cleanup | `7bb2fb0` | — | `85c9778` |

## Help strings (for the §8 erratum)
**`--ssh-timeout` help**
- Before: "Per-attempt stall limit for setup and for a body read; stalled setup is retried (0 = no timeout, default 9)"
- After: "Per-attempt stall limit for SSH setup and for a body read; stalled setup is retried (0 = no timeout, default 9)"

**`--ssh-timeout` long help** (only these three sentences change)
- Before: "The clock applies to setup, and it applies to a stalled read during a fetch, push, or pull body."
  After: "The clock applies to SSH setup, and it applies to a stalled read during a fetch, push, or pull body on SSH and HTTPS."
- Before: "SSH and HTTPS use this same stall clock and the same 30 second setup budget."
  After: "HTTPS setup has no stall clock, only the 30 second setup budget that SSH setup also has."
- Before: "At these defaults, a setup that makes no progress is reported after at most 4 times 9 seconds plus 1, 2, and 4 seconds of waits and under 1 second of jitter, about 44 seconds."
  After: "At these defaults, an SSH setup that makes no progress is reported after at most 4 times 9 seconds plus 1, 2, and 4 seconds of waits and under 1 second of jitter, about 44 seconds, and an HTTPS setup that makes no progress after 4 times 30 seconds plus the same waits, about 128 seconds."
- Kept unchanged, because it holds on HTTPS too: "0 disables this stall clock and the 30 second setup budget on both SSH and HTTPS." On HTTPS, 0 removes both the request's read deadline and its connect budget.

**`--max-retries` long help**
- Before: "--ssh-timeout sets only the per-attempt stall."
- After: "--ssh-timeout sets only the stall of an SSH setup attempt and of a body read."

**Docs:** I regenerated `docs/CLI.md`. All 43 changed hunks are this flag's long help, and nothing else in it was stale.

**Two related issues I left alone:**
- **GET reads on HTTPS:** for a GET, the same allowance also times the wait for the response headers, so the body read gets only what remains.
- **Ordinary build's help:** it still mentions the transport's retries and `--max-retries`, which that build doesn't have. That text belongs to TR1.5 and TR2.5.

## Fail before, pass after
1. **Help:** the `g09` help pin failed in both builds, and the `--max-retries` pin in the candidate build, against the old text. After the change both pass, and the `CLI.md` check passes after regeneration.
2. **Cli placement:** a Cli-placed request with `Some(0)` was accepted. It is now refused at `request()` with UnsupportedOperation naming the Cli placement. A request without a budget still opens, with the driver's record at the default 3.
3. **Range:** `u32::MAX + 1` was accepted, and gwz-cli parsed 4294967296. Both are now refused before the request registers, and `u32::MAX` is kept.
4. **Merge:** a merge request with `max_retries: Some(0)` was accepted. It is now refused on all five merge operations. The inventory checker reported two new switch sites until I added their lines, giving 20 in total.
   - Decision for you: it fails with MergeValidationFailed, the code merge uses for every policy field it refuses, including `--jobs` and `--max-per-host`. Using InvalidRequest would make this the only field refused differently. Say if you want InvalidRequest anyway.
5. **Backstop:**
   - **Does the risk exist?** Yes, an attempt can delay its failure report by the cleanup allowance. The pool host disposes a failed setup before reporting it, on both SSH and HTTPS, and a timed-out setup waits for its thread to stop, bounded by 5 s.
   - **The fix:** each attempt's backstop now includes that 5 s. The default goes from 159 s to 164 s per attempt.
   - **Test:** a new test checks the backstop meets §5's full bound at stalls of 1 s, 4.999 s and 9 s. It failed at a 1 s stall, and the existing test failed on 159 s. Both pass now.

## Gates on the final tree
| Gate | Result |
|---|---|
| gwz-core candidate, transport switch | 2,755 passed, 0 failed, 7 ignored (rerun; first run failed one test, see below) |
| gwz-core candidate, both switches | 2,755 passed, 0 failed, 7 ignored |
| gwz-core ordinary | 2,326 passed, 0 failed, 1 ignored |
| gwz-cli | ordinary 253 + 92, candidate 254 + 92, inventory and process-global checks pass |
| gwz-py candidate | 1,000 passed, extension built from this tree |
| Consumer | Python 27 passed, regenerator check passes, archive proof passes (compat 11 of 11) |
| Checkers | boundary ok, switch inventory 20 sites, process-global and cfg-boundary nothing new |
| clippy | gwz-core ordinary `-D warnings` passes; candidate 85 old warnings, none on the lane's lines; gwz-cli 0 warnings in both builds |
| rustfmt | gwz-core clean; gwz-cli fails only on `src/tests/g02/partial_errors.rs`, which arrived unchanged from main's `236f753` |
| Lane gate from `ab48966f` | 23 commits, 22 ok; red only at main's `341a669`, whose approval landed in main's next commit `8f3ecc3` |

- **The transport-leg rerun:** on its first run the HTTPS throughput test missed its timing bound (2.0–3.7 s against 1.5 s). The machine's load average was 69 to 118 from other work, and the test is timing-sensitive. The rerun at lower load passed everything, and so did the both-switches leg.

## Setup and disk
- **Scratchpad:** I prepared a new candidate manifest for gwz-core and one for gwz-cli under the new scratchpad. gwz-cli's tests resolve `../gwz-core` from their manifest, so the scratchpad also holds a `gwz-core` link to the lane; without it, three gwz-cli candidate tests fail.
- **Build outputs:** all of them are in the lane and kept, about 3.5 GB: `candidate-target`, `candidate-target-both`, `candidate-target-py`, `target` and `gwz-core/target`. gwz-py's recipe refuses a destination inside the workspace, so I built its extension next to the lane and then moved it into `candidate-target-py/pyc`.
- **Free disk:** 855 GiB on the external disk, 99 GiB on the internal disk.
