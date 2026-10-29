# Hotfix 1.0.18 (root push proof): remediation plan, round 2 (final)

Date: 2026-10-27. After [Code r2](GwzHotfix1018-ReviewCode-2.md) (NO-GO: R2-1 P2, R2-2 P3) and [State r2](GwzHotfix1018-ReviewState-2.md) (NO-GO: P2-A) on gwz-core `659118d2` (`hotfix/v1.0.18`). Every round-1 finding, and the operator-approved OOS-1, is closed on both axes; no false proof found. No architectural root cause. This is round 2 of 2: a further P2 stops the lane for an operator decision.

**Blind convergence:** both axes found, from opposite sides, that round 1's `Unheld → FETCH_FIRST` rule names the wrong remedy. State P2-A: an advertised object the member can never hold (`refs/pull/*`, a tag to a tree) while the LOCKED commit is held → fetch-only forever, publish remedy hidden. Code R2-1: the LOCKED commit is unheld while every advertised object is held (the commit was never published) → fetch-only forever, publish remedy hidden.

## Blocking (one rule closes both)

| ID | Disposition | Closure rows |
| --- | --- | --- |
| State P2-A + Code R2-1 | Name fetch alone only when BOTH the locked commit is unheld AND some advertised object is unheld (the only case a fetch can bring the locked commit in). Locked unheld but every advertised object held → `Diverges` (the commit is not in the advertised history; caller's publish/fetch/check-remotes remedy). Locked held but an advertised object unheld → that object proves nothing (`Diverges`); caller's remedy. The fetch-only text also says what to do if the retry is still refused (the full remedy), so even a fetch that cannot bring the commit in leads somewhere. Fix the `FETCH_FIRST` and g26 doc comments. | Native: (a) unfetched `refs/pull/1/head` + unpublished `work` → full remedy before and after fetch; (b) held annotated tag to a tree + unpublished `work` → full remedy before and after fetch; (c) locked commit never published, all advertised held → full remedy (Code's probe); State's existing branch/tag rows stay fetch-only and pass after fetch. g26: `lack_commits(&[APP_HEAD])` with all advertised held → full remedy; advertised missing + locked held → full remedy. |

## Non-blocking

| ID | Disposition |
| --- | --- |
| Code R2-2 | Pin the equality short-circuit in `pushed_source_descends`: the positive OOS-1 row asserts no dependency `ReadAdvertisement` after the member push. |

## Process

Same agent, one patch on `659118d2`; gates: fmt, clippy as release.py, `run_tests.py --lib`; the lane owner commits; both reviewers re-verdict P2-A/R2-1 (and R2-2).

## Status

Status: **accepted at gwz-core `17b965f7` (branch `hotfix/v1.0.18`, on `v1.0.17` `cc2018c0`: `00e3349a`, `659118d2`, `17b965f7`) after GwzHotfix1018-ReviewCode-3.md and GwzHotfix1018-ReviewState-3.md reported GO (2026-10-27); this accepts the root-push proof fix and the operator-approved OOS-1 only. Release (scripts, v1.0.18 tags, GitHub releases) and the forward-port to main await the operator.**
