# GWZ open decisions

Status: register opened 2026-09-18. Small decisions taken by the operator
that are not big enough for a plan of their own, so they do not get lost in
chat. One row each: what was decided, which repository owns it, and what
triggers doing it. Larger lines of work live in their plans: gwz-cli
`dev-docs/GwzClaudeIntegrationPlan.md`, gwz-core
`dev-docs/GwzLaneCleanFixesPlan.md`; lane problems live in
`GwzLaneIssues.md`.

| ID | Decision | Owner | Trigger |
| --- | --- | --- | --- |
| D1 | `gwz merge --remote` gains `--wait <secs>` (GwzLaneCleanFixes R21 says every family command; the R20/R21 lane stopped at the `local` verbs because merge's flags live in `merge.rs`). | gwz-cli, gwz-core | closed: gwz-cli `8c8b66a`, gwz-core `7e76c3e3`, released in 1.0.14 |
| D2 | gwz-py's human `gwz local list` table gains the owner column; its `--json` already carries it. | gwz-py | closed: gwz-py `6f891c30`, released in 1.0.14 |
| D3 | A private member the clone omitted (quiet clone, `GwzUrlSchemePlan.md`) is reported by `gwz ls` as `materialized: false` with a reason (`private, skipped`), not `true`. The omission itself stands. | gwz-core | closed: gwz-core `f4816a7c`, gwz-cli `1c050fe`, released in 1.0.14 |
| D4 | `gwz-core/src/entry.rs` stays unsplit as the trust anchor (`GwzRustSplitPlan.md`). Deliberately not done. | gwz-core | none |
| D5 | The gearu conversion of the three release scripts stays parked. | all three | after gwz 1.0.14 ships |
| D6 | gwz-py's `run_tests.py` prefers the workspace `target/debug/gwz` when gwz-cli is a workspace member and falls back to `gwz-cli/target/debug/gwz` only outside one; the stale `gwz-cli/target` in gwz-dev is deleted (GwzLaneIssues L5). | gwz-py, gwz-dev | closed: gwz-py `4adc636`, released in 1.0.14; `gwz-cli/target` deleted 2026-09-18 |
| D7 | The review loop's Surface axis (added to the skill 2026-09-18 after `GwzClaudeIntegration-ReviewSurface.md`) is named in `AgentProcessRules.md` as mandatory at every interface freeze. | gwz-dev | closed: root `AgentProcessRules.md` L1-18 amended 2026-09-18 |
| D8 | The cost measurements (Claude Code plan S3.2: block-sharing ratios per filesystem, per-file copy time, the pre-lock inventory interval; and S3.5's dispose timing) are postponed indefinitely. The placeholder table (APFS, XFS, btrfs 94%; ReFS 70%; ext4, NTFS 0%) and the 300 s wait default stand, and the guide keeps its "Unmeasured" markers for them. | gwz-cli | none scheduled; only if the guard or the timeouts misbehave in use |

Closed rows move to the bottom with their closing commit.
