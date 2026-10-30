# Step 1.7 driver-only construction — CODE-AXIS RE-CHECK, ROUND 3 (short form)
**Review object:** plan step 1.7 at gwz-core `a681cf26`. Its parent is `b0ac8de7`. It amends `25941ce9`, and only `src/transport_host/driver_only_tests/rewrites.rs` changed. gwz-transport `96938616` and root `34251a9c` are unchanged. **Baseline:** `25941ce9` (code-axis GO, `GwzDriverOnly-Step1.7-ReviewCode-3.md`). **Date:** 2026-10-30
**Axis:** Code. Independent, adversarial, read-only. Filed verbatim by the lane owner as `GwzDriverOnly-Step1.7-ReviewCode-4.md`.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 0 P3 open. The round-3 GO carries to `a681cf26`, which differs only in a fixture. The fix and the "fixture only" classification are both confirmed. One correction to my own earlier evidence is recorded below; it does not change any verdict.

## Prior-finding closure table

| Item | What it was | Change in `25941ce9..a681cf26` | Evidence | Status |
|---|---|---|---|---|
| Windows receipt `p1-7-20261030-c`: two includeIf rows red | `a_conditional_include_rewrite_to_a_refused_route_is_refused_by_name_for_the_clone_and_the_read` and `…_to_a_served_route_is_refused_only_when_it_is_another_kind` failed on dabeest with `GitCommandFailed`, not `UnsupportedOperation` (`rem3-red-dabeest.log`). | New `include_rules_under_temporary` (`rewrites.rs:349–384`). It writes the condition as `gitdir/i:` followed by the canonical temp directory with the verbatim `//?/` or `//?/UNC/` prefix removed (`spelled_as_a_git_directory`). It appends a marker key (`gwzrules.conditional = applied`) to the rules file. It then makes a repository under the temp directory and asserts the marker is visible there, so a non-matching pattern fails loudly instead of failing the row on another ground. | **Root cause, dabeest matrix** (`rem3-probe2.log`, 8 patterns against a repository under `%TEMP%`, which is an 8.3 name, `HLIFX0~1`):<br>- libgit2 sees the gitdir as `C:/Users/…/Temp/hlifx0a1b/…/.git/`, the long name without `\\?\`.<br>- The verbatim form (`//?/C:/…`, the old fixture's pattern) never matched.<br>- The 8.3 form never matched.<br>- The stripped canonical form matched, with and without `/i`.<br>**Green on dabeest:** 24 passed (`rem3-green-dabeest.log`), on a tree whose `rewrites.rs` is byte-identical to `a681cf26`'s (md5 `d99dfaeb…` on both).<br>**Self-check:** the macOS mutant pointing the condition at a non-matching directory fails, naming the condition (`rem3-mac-mutant.log`).<br>**Local:** my run on the lane at `a681cf26` passes 31 of 31, including both includeIf rows (below). | Closed (fixture) |
| Classification "fixture only; product never reached" | — | — | **The red failure was not a product miss.** Because the rule was never applied, the clone went to the fixture's `https://example.invalid` host on the HTTPS route and failed closed (`GitCommandFailed`). `refuse_unjudged_remote` compares URLs, never paths, so libgit2's gitdir spelling cannot affect it.<br>**The product's Windows behaviour is still unproven until the queued `-d` receipt.** The fixed rows are the first Windows test of the created-remote observation. | Confirmed |
| Code round-2 P3-1 / P3-2 and round-1 findings | Closed in round 3. | Untouched. | — | Closed |

## Changed-range analysis

- **Scope:**
  - One test file: `rewrites.rs`, 60 lines added and 21 removed.
  - No production, script or inventory change.
  - The two rows now share one helper, and their assertions are unchanged.
  - The ratchet, candidate-switch inventory and boundary check are unaffected.
- **Correctness of the helper:**
  - `config_text` ends every rule with a newline (`routes_and_binding.rs:186–190`), and so does the served row's text. The appended `[gwzrules]` section therefore starts its own line.
  - The self-check runs inside the re-executed child, so it reads the same `HOME` and global configuration as the row (`child_process.rs`).
  - It probes a repository created under `std::env::temp_dir()`, which is also where the clone target and the read's scratch repository live. That is patterns E and H in the dabeest matrix, which matched.
  - `gitdir/i:` widens the match to case variants. That is harmless for a temporary-directory fixture and needed on Windows.
  - The `//?/UNC/` → `//` mapping matches libgit2's UNC spelling. The dabeest run did not exercise a UNC `%TEMP%`, so that mapping is untested but low-risk.
- **No new root cause.** This is a test-fixture portability defect, not architectural, and has no product counterpart.
- **Evidence correction to my rounds 2 and 3.** My "lane" runs `lane-candidate-r2.log` (30 passed) and `lane-candidate-r3.log` (33 passed) did not recompile the lane. They ran my probe copy's test binary.
  - **Cause:** the copy's path dependencies pointed at the lane siblings, so cargo gave it the same metadata hash (`gwz_core-4434882afdd95cfc`). Its fingerprint followed the copy's dep-info, so the binary was reused.
  - **Impact on substance: none.** Those copies were `rsync`s of exactly `43f4ef0c` and `25941ce9` plus only my probe tests and test-only accessors. The lane rows that ran were those commits' rows; the probe rows are the extra 4 and 2.
  - **Corrected numbers:** round 2 is 26 lane rows plus 4 probes, and round 3 is 31 lane rows plus 2 probes. My round-3 report printed 33 for that run; 31 of those were lane rows.
  - **This round's run is genuine.** `lane-candidate-r4.log` shows `Compiling gwz-core v1.0.18 (/Volumes/projects/limbo/gwz-dev-p1-7/gwz-core)`, and the command `RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real cargo +1.95.0 test --lib --locked -- driver_only route_tests` gave 31 passed, 0 failed, with no probe rows.
  - **Fix for future runs:** use a separate `CARGO_TARGET_DIR` for any probe copy.
- **Tuple checked at start and end; it did not move.** Root `34251a9c`, gwz-core `a681cf26` with a clean tree, gwz-transport `96938616`.
- **Not run:** Windows; the `-d` receipt is queued.

**Verdict: GO** for `a681cf26` on the code axis. The GO carries from `25941ce9`. Whether the observed-remote check works on Windows is left to the queued `-d` receipt.
