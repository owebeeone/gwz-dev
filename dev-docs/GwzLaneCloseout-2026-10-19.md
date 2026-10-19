# Lane closeout, 2026-10-19: final disposition and remaining obligations

Status: **closeout record.** It reports the operator's closeout authorization of 2026-10-19 as carried out, and
it is the remediation programme's M1 step 2 input ("reconcile active lanes",
[GwzTransportReleaseRemediationPlan.md](GwzTransportReleaseRemediationPlan.md) §5). It authorizes nothing: no
refactoring, Connection work, feature, lane, investigation, tag, publication or activation. Every item under
"Remaining obligations" and "Decisions for the operator" needs a new instruction.

## 1. The authorization and what completion means

The operator authorized seven tasks:
1. preserve the paused work in the private evidence repository;
2. complete, review and merge the credential-redaction fix, then the large-blob correction, then the filter-tooling fix, each as its own package;
3. assess the constructor guard together with the reported ungated spawn, with a bounded corrective package if confirmed;
4. preserve and pause the feature, activation and scaffolding work;
5. dispose the redundant lanes through GWZ;
6. reconcile Python's tested dependency tuple and push the settled main commits;
7. record the result here.

Completion means three things: the work is recoverable, the baseline is corrected, and the redundant lanes are gone.

## 2. Preserved work

The archive is the private evidence repository, campaign `workspace-lanes`, run `2026-10-19-lane-closeout`, at
gwz-core-evidence `1e22800c`, which is pushed.
- It holds 322 lane-member records for all 23 lanes. 25 members in 12 lanes held unique content.
- `runner/verify_lanes.py` rebuilt every member from the archive alone and matched both HEAD and the working-tree fingerprint: **322 PASS**.
- The run also holds the lanes' pause notes (`notes/`), the separated D50 patch, and the small out-of-tree logs and receipts the notes cite.
- Recovery steps are in the campaign README.
- The operator's own untracked files are pinned by sha256 and stay in the workspace; they were not copied. These are the `2026-09-22-alpha-setup-timeout` run and the root dev-docs drafts.

## 3. Packages landed (merged in this order, pushed)

| Package | Commits | Review | Windows |
|---|---|---|---|
| D50: the `gwz.transport` setting reader stops repeating libgit2's config parse message | gwz-core `21e39505`, reviews `c12d2910` | Code GO and State GO; 3 P3 each, converging blind on two of them | `d50-20261019-a` PASS; the fast-forward merge keeps that tree |
| D42: push producer-busy follow-ups (large-blob correction) | gwz-core `f69b1112`, re-verdict `c8f256cf`, merge `6c07e8c9` | State GO in round 2; 3 new P3 (N1–N3) | `blobfix-20261019-b` PASS; merged commit `merge-blobfix-20261019-a` PASS |
| Filter tooling: `run_tests.py --test-filter` | gwz-core `87c4d751`, `ea625856`, `a7e677d1`, reviews to `efd68aa2`, merge `4d4b6dde` | Code: NO-GO in round 1 (P2-1, a filter matching nothing passed) and round 2 (P2-2, a filter matching only `#[ignore]` tests passed); GO in round 3 | scripts only; not a trigger path |
| Bounded corrective: `gwz log`'s path cursor spawns under the backend's spawn gate | gwz-core `650dd4e0`, gwz-py `02d79ca`, gwz-cli `cea7bd4`, reviews, merge `e0abfe0e` | Code GO and State GO; 2 and 3 P3 | `loggate-20261019-a` PASS; merged commit `merge-loggate-20261019-a` PASS |
| Python tuple: gwz-py switch tests updated to the D31/D47 code | gwz-py `f7cacfb`, review `71d2d07` | Code GO, no findings | n/a; `run_tests.py --candidate` 1062 passed |
| Python tuple: gwz-core's gwz-py pin moved from `950064d` to `71d2d07` | gwz-core `461ddf4b` | — | ci-only |

The spawn-gate assessment, read-only:
- (a) `commit_log/mod.rs` spawned `git rev-list` ungated. Exposure is **CONFIRMED** in gwz-py on Windows, in candidate builds. The CLI and 1.0.17 are not exposed. The package above fixes it.
- (b) `TransportRuntime::build_native` takes a default gate. **NOT CONFIRMED** as an exposure: only tests reach it.

**Pushed.** gwz-py `71d2d07`, then gwz-core `461ddf4b`, gwz-cli `cea7bd4` and gwz-core-evidence `1e22800c`. Each was
verified with `ls-remote`. Before the push:
- the lane gate passed over all 21 unpushed gwz-core commits;
- the boundary job passed;
- the CI-equivalent transport candidate suite gave 4006 passed and 1 failed. The failure is
  `address_fallback_stops_at_the_first_ssh_negotiation`, a 1 s stall row that failed at load average about 16 and passed 3 of 3 when rerun alone. Its code path is unchanged in this cycle.

**Root is not pushed.** See §6, decision 2. No tags, publication or activation were done.

**CI after the push.** See §7.

## 4. Lane disposition

All 23 lanes were disposed through `gwz local dispose`, after two checks. The first is the archive verification
above. The second is a pre-disposal check run twice (`disposal_check.py`). It found every member either contained
in main, meaning HEAD is an ancestor, the tree is clean and every untracked byte is in main, or equal to its
verified record: **0 unsafe**.

GWZ's hazards were waived as follows:
- `dirty`: all lanes except agentwords and amend41.
- `unpreserved-history` as well: gitproxy, relscaf, https611, sspi012 and words2. The gap this leaves is in §6, decision 4.

| Lanes | Disposition |
|---|---|
| agentwords, amend41 | reused, refreshed from main, for the D50 and spawn-gate packages; merged |
| blobfix, filterfix | completed, reviewed, merged (§3) |
| cap72, d14two, exactagent, gapsa, gapsb, https611, sspi012, win92, words2 | already merged before this closeout |
| ea-p1, ea-p2, ea-p3 | D38 Phase A, paused and preserved (§5) |
| gitproxy | D43/D51 round 2, paused and preserved; its D50 setting-reader half landed separately (§3) |
| gatecheck | D52 source check, paused and preserved (§5) |
| c3b | S7.1 C3b activation, paused and preserved (§5) |
| relscaf | publication scaffolding and bootstrap checklist, paused and preserved (§5) |
| s63 | WPP 6.3: no measurements taken; paused and preserved |
| blobpush | superseded by blobfix |
| fsmonitor-hang | host-condition investigation, abandoned |

Owned outputs were removed:
- the lane build targets in `build-targets`, 199 GB down to 6 GB;
- 27 lane directories on dabeest;
- the WSL lane directory, 9.4 GB down to 119 MB;
- the park folders.

Kept as warm caches: `build-targets/pushgate-ct` with its prepared tree `pushgate-cand` (the transport build of
today's main), dabeest `E:/gwz-tests/merge-loggate-20261019-a`, and WSL `measure-20261015`. Everything older
than this cycle on dabeest was left in place.

## 5. Remaining obligations (paused, not deferred)

These keep their 1.1.0 standing under their existing decisions. Nothing here is moved to 1.2.0. Each lane's state
and exact next step are in the archive's `notes/<lane>.md` and `lanes.json`.

**D38 exact agent-key selection** (ea-p1, ea-p2, ea-p3): Phase A, steps 1.1 to 1.3, of 14 steps.
- 1.1: 21 of 22 rows green.
- 1.2: 7 of 7 rows green.
- 1.3: tests written; the code is a draft held in the archive.
- Open for the operator: the certificate-fingerprint reading (ea-p1), and the change that makes `offered()` fire once per key, which also affects the ambient path (ea-p2).

**D43/D51 git `http.proxy`** (gitproxy):
- Round 2 fixes all eight round-1 blockers (P0-1 to P0-6, P2-1, P2-2), each with a test.
- Still to run: the ordinary leg, the boundary job, the Windows receipt, and the evidence README and manifest.
- The proxy-reader half of D50 (3 lines in `proxy_config.rs`) depends on this code.

**D52 spawn-gate source check** (gatecheck):
- Built, but its 51-test rerun after the refactor has not been done.
- Its allowlist must drop the `ungated_spawns` entry, which the log-gate package has now resolved.
- It must add `next_unfiltered` as a spawn root, and make `build_native`'s fallback an explicit `host_origin`.
- gwz-cli is not scanned.

**S7.1 C3b activation** (c3b):
- The recipe is in the archive (`CHANGE-RECIPE.md`, the apply script, the patches).
- It must be re-run on current main.
- Open: 62 + 116 clippy errors in transport code (7 of them a `MutexGuard` held across `await`, each to be judged), and sshd provisioning in the workflow.

**Publication scaffolding** (relscaf):
- gwz-git scaffolding `6ec091c` is not pushed.
- The bootstrap checklist (`10555a84` plus an edit) is preserved.
- git2-rs `main` (`4069b04`) carries the upstream merge. gwz-core still pins the tested `d13951f`.
- A release from the tested fork commit is possible by setting gearu's `project.branch`. It needs the three-test expectation fix.

**WPP 6.3 Windows evidence** (s63): not started; staging only.

## 6. Decisions for the operator (outside the closeout's scope)

1. **Released gwz 1.0.17 prints proxy passwords.** A malformed `[http] proxy` line makes libgit2's message carry
   the rest of the line. The generic `git_error` mapping (`src/git/verify_checkout_state.rs:132-141`, 28 files)
   and at least three other paths pass it through.
   - Reproduced with the installed 1.0.17 on `status`, `--json status`, `commit` and `fetch` (D50 run, step 3).
   - On the transport build, the D50 refusal tells users to use `--transport` or `GWZ_TRANSPORT`, which leads them into that generic path.
   - D50's own fix covers only the setting reader.
2. **Root push is blocked.**
   - The workspace lock includes `mem_gwz_credential`, which has no fetch URL, so `gwz push` refuses root.
   - My root record commit `8c3e5036` swept in another session's staged registration of that member, together with its `gwz.yml` and lock changes. I should have checked root's index first. That session has since committed `4125de0e` ("Record accepted isolated credential member") on top.
   - Root can be pushed once `gwz-credential` has a remote, or after its owner decides otherwise.
3. **The spawn gate is unfair** (a hypothesis, raised by both log-gate reviewers). The HTTPS helper launch polls `try_hold` every 1 ms. A pathspec `log` across many members now takes the gate once per entry, from up to 100 threads. On Windows that could starve the helper launch into a Timeout. That would fail safe (Timeout, not a leak), but it is a new failure. Probe it before relying on it (the review gives the row).
4. **The disposal waived `unpreserved-history` for https611, sspi012 and words2**, where my check had verified only HEAD lineage and the working tree. Their HEADs were contained in main. Any other refs they held were not inspected, and they are gone. sspi012's were most likely the pre-squash commits of the approved D31 squash, whose content is in main.
5. **Smaller items:**
   - the program checkpoint's recorded gwz-py inventory digest is stale, and was already stale before this cycle;
   - the inventory header and a test docstring still cite amendment 2 at its pre-archive path;
   - the load-sensitive 1 s stall row (§3).

## 7. CI on the pushed commits (all green)

| Repository and commit | Run | Result |
|---|---|---|
| gwz-core `461ddf4b` | Checked-artifact boundary 39200165633 | success |
| gwz-core `461ddf4b` | Retained merge readers 39200165586 | success |
| gwz-core `461ddf4b` | Transport candidate 39200165609 | success |
| gwz-core `461ddf4b` | Windows matrix 39200165637 | success |
| gwz-cli `cea7bd4` | CI 39200178285 | success |
| gwz-py `71d2d07` | CI 39199326896, rerun | success |
| gwz-py `71d2d07` | Transport candidate 39199326891, rerun | success |

Both gwz-py runs first failed: gwz-py was pushed before gwz-core, as the pin rule needs, and its CI built against
the gwz-core main of that moment, which had the old `handle_log` signature. They were rerun once gwz-core was
pushed. Each repository's CI builds against the other's main, so the first-pushed side is red for one cycle.

## 8. Open P3 findings from the reviews (not blocking; for the owners)

- **D50:**
  - `parse_failure`'s guards are not pinned by a table test;
  - the `Refusal` and module docs still say "libgit2's message";
  - non-parse fallbacks (include depth, unreadable include) are labelled "could not be parsed";
  - the fallback wording repeats itself.
- **D42:**
  - N1: the design's evidence pointers and §7 window figures;
  - N2: the trigger row's stock-server comments;
  - N3: the gate rows' 300 ms sleep.
- **Log gate:**
  - the gwz-py row exercises `run_log`, not dispatch;
  - the archived gwz-py candidate log predates the inventory line;
  - the receipt's core-only digest is not recomputable, and the Windows shape logs are not archived;
  - the gate row's proof rests on a 500 ms window;
  - three diff helpers on the log route build private-gate backends. They spawn nothing today.
- **Filter tooling:** the optional `windows_lane_check.run_entry` guard for zero-test filtered entries.
