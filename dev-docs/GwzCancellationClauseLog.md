# Cancellation designs A and B: clause log

Date: 2026-11-03. Status: **record of the lane owner's application of the superseded/amended-clause tables of
`GwzCancelInterruptDesign-DRAFT.md` (A, §8) and `GwzChildLifetimeDesign-DRAFT.md` (B, §8) at their acceptance**
(A at blob `8facabd8`, B at blob `4d470544`; plan §10 D-V, "Designs A and B accepted (operator, 2026-11-03)").
Documents only; no code is edited. History files are never edited.

"Line before" is the target's line in the lane before this pass (the same as at root `6d20ef6f`, which both designs cite).
"Plan" is `dev-docs/GwzTransportReleaseConvergencePlan.md`, "2.3 plan" `dev-docs/GwzServerCore-Step2.3-Plan.md`,
"MachineOutput" `gwz-cli/docs/MachineOutput.md`; paths are workspace-relative.

| Design | Row | Target file:line before | Result |
|---|---|---|---|
| A | 1 | Plan:315 (§4 graph, Phase 3) | applied: the line replaced by the row's graph line |
| A | 2 | Plan:712 (Phase 3 heading) | applied: "(three lanes then one, 19–23 lane-days)" |
| A | 3 | Plan:717-720 (after 3.0b) | applied: bullets 3.0d, 3.0e, 3.0f, 3.1c, 3.2b inserted after :720, each with A §5's content, size and "After", citing A §5 |
| A | 4 | Plan:721-728 (3.1) | applied: the sentence appended to 3.1, after its "Prerequisite" paragraph (:728) |
| A | 5 | 2.3 plan:186-188 (2.3i) | applied: the handler clause and the red row "a second signal or `force`" replaced as written |
| A | 6 | 2.3 plan:207-217 (2.3k) | applied: the text added before "Size ~500 + 400." (its first word capitalized as a sentence); "After" gains 2.3s |
| A | 7 | 2.3 plan:220-230 (after 2.3l), :240 (wave 2 lane 2), :249-250 (totals), :71 | applied: the 2.3s paragraph after 2.3l (after B's 2.3r-1 and 2.3r-2); wave 2 lane 2 "2.3f listener files, then 2.3s interrupt source"; totals "about 5,580 implementation lines and 6,120 rows across 15 steps" (supersedes B row 4's totals clause); :71 takes the same figure (see B row 4); 2.3r-1 and 2.3r-2 after 2.3s come from B row 1's text |
| A | 8 | MachineOutput:954-964 (Exit Codes) | applied: the row added to the table; a "### Interrupted commands" subsection after :964 with A §3.6's entry, its place, the forced envelope, the `--jsonl` rule and the notices |
| A | 9 | MachineOutput:280-287 (Notes) | applied: appended after :287 |
| A | 10 | MachineOutput:668-673 (JSONL; row cites :669-673) | applied: appended after "available to the driver." (:673) |
| A | 11 | MachineOutput:142-143 | applied: appended to the bullet |
| A | 12 | MachineOutput:66-69 ("and one error entry", :68) | applied: the parenthesis added after "one error entry" |
| A | 13 | `gwz-core/docs/ErrorCatalog.md:35` | applied: "A result's first error;" replaced; the remedy column appended |
| A | 14 | `dev-docs/GwzCoreSessionDesign.md:687` | applied: appended to the bullet |
| A | 15 | `gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md:103-106` (§2.5) | applied: a bullet added after :106 |
| A | 16 | `gwz-py/README.md:225` (paragraph ending :226) | applied: appended after :226 |
| A | note | "the helper-owner contract's `CleanupPending` clauses" | not applied: history file (A §8's closing note says they stay unedited) |
| B | 1 | 2.3 plan:220-230 (after 2.3l) | applied: the 2.3r-1 and 2.3r-2 paragraphs after 2.3l, before A's 2.3s |
| B | 2 | 2.3 plan:202 (2.3j "After") | applied as written |
| B | 3 | 2.3 plan:234 (critical path), :240-241 (waves 2 and 3, lane 3) | applied: :234 replaced; wave 2 lane 3 "2.3e address, then 2.3r-1 registry", wave 3 lane 3 "2.3r-2 spawns, then 2.3j credentials and stdio"; the reasons split by step: the r-1 clause appended to wave 2's "Why" cell, the r-2 clause to wave 3's |
| B | 4 | 2.3 plan:71, :249-250 | applied with A row 7: :71 "Summed in §4's totals sentence: about 5,580 implementation lines and 6,120 rows." (B's pointer, A's figure); "Review: dual on a, d, j, l and r-2"; B's totals clause (5,280 / 5,820 / 14 steps) superseded by A row 7's sentence, as B's row says |
| B | 5 | Plan:721-728 (3.1), :729-735 (3.2), :739 (3.4) | applied: appended to 3.1 (after A row 4's sentence) and to 3.2; for 3.4 the "and …" clause joined the measured list, before "; evidence run" |
| B | 6 | `gwz-py/README.md:163` ("Platform And Status") | not applied: B §8 assigns this row to step 3.2, not to acceptance |
| B | 7 | `dev-docs/GwzConnectionApiContract.md:289` (§6, Composition/CleanupDomain) | applied: appended to "State owned" |
| B | note | TR1.6 "Its cost" (:76) and OQ2 (:398); r1's OD-C2 (a) (`3448c561` :245-246); `gwz-core/dev-docs/history/GwzTransportCredentialHelpers-ReviewCode-2.md:66` | not applied: history file (B §8's closing note says they stay unedited) |

## Observations outside the rows (not applied)

- Plan §9 "Cost" (:894-903 before) still gives Phase 3 as 11–13 lane-days and the 0–6 total as 130–160; A row 2 changed
  only the Phase 3 heading to 19–23. No row amends §9; the lane owner decides whether to align it.
- Root main's `c8774495` splits 2.3g into 2.3g1 and 2.3g2 in the 2.3 plan; this lane's figures (15 steps) count 2.3g as one.
  Merging this lane onto main needs the step count and totals reconciled there.
- Active-file cap (AgentProcessRules L1-33): root `dev-docs/` held 86 active files before this pass, over the 75 cap; this
  log makes 87. No touched document exceeds 2,500 lines. Candidates for archival at the next stable checkpoint: A's and
  B's round-1 and round-2 reviews (eight files) and round-1 RemPlans (two), which the accepted designs supersede.
