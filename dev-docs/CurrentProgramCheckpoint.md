# Current program checkpoint

Date: 2026-08-15
Status: **the single current-state authority for the GWZ merge program.
Update at every checkpoint boundary; keep concise; history belongs in git,
not in this file. Live status paragraphs in other documents are superseded
by this file (rulebook §7.3).**

## Exact tuple

| Repository | Commit | Note |
| --- | --- | --- |
| gwz-dev (root) | `2e6706f` + this docs checkpoint | coordination |
| gwz-core | `da58135` | Use retained Windows destination paths |
| gwz-cli | `3cca145` | Close R4b P1/P2 remediation gate |
| gwz-py | `929efb0` | Implement R4b reverse merge lifecycle |
| taut | `f008419` | 0.8.x fallible from_cbor line |

Workspace lock pins verified equal to member HEADs at writing. Released
line: v0.10.5 (see `GwzMergeCheckpoint-v0.10.5.md`; v0.10.4 superseded).

## Ownership

Implementation lane: **Claude (F5)**, by operator decision 2026-08-15,
effective from the tuple above. The previous implementer's lane is closed at
`da58135`; per L1-06 no other agent writes to the merge/catalog lane without
an operator-recorded handoff. Reviews run cross-model where possible
(`GwzProcessOptimization.md` §4.3).

## Process authority

`AgentProcessRules.md` (as amended 2026-08-15) plus the adopted
`GwzProcessOptimization.md`, which controls where they differ. Tiered review
depth, two-round remediation cap, two-track freeze, and history archiving
(L1-33) are in force from this checkpoint. (Two-track freeze: the policy is
in force now; its checklist and CI wiring land in Phase 3, before the next
contract freeze at I6.) Recorded review tiers: R2-C2 settled re-review —
dual, cross-model where available (mandated tier).

## Accepted through

- M5-8 packages through R4b P1/P2 remediation: closed (eleventh-round GO/GO).
- R2-C0: accepted (Interface-ReviewCode-3 / ReviewState-3).
- R2-C1: accepted (AggregateClassifier -2 rounds, no P0-P2).
- R2-C2: **open.** The `ReviewCode-2` NO-GO (post-verification publication
  race) drove the accepted sealed-primitive design
  (`GwzM5-8R2CCatalogBootstrapAmendment.md` §4.1). Implementation has
  proceeded through the Windows retained-handle/destination-path commits
  (`e90de60`, `e07eb28`, `da58135`). Completeness against §4.1's four edges
  and the executed per-fault interruption/recovery matrix are **unverified**;
  no post-fix re-review has run.

## Open findings and obligations

- Amendment-package dual review (mandated tier for amendments): filed as
  `GwzProcessAdoption-ReviewConsistency.md` (GO, 5×P3) and
  `GwzProcessAdoption-ReviewSafety.md` (NO-GO, 1×P2 + 4×P3). All findings
  from both reports were corrected in this same checkpoint; the safety
  reviewer's focused re-verdict on the corrections is recorded in
  `GwzProcessAdoption-ReviewSafety.md` §"Focused re-verdict". Model note:
  this round ran dual same-model (Claude) with fresh-context isolation
  because the second model is not invocable from this session; a
  cross-model pass by the operator's other agent remains open as an
  optional strengthening.
- R2-C2: execute the complete per-fault matrix (L2-14 form), then two
  settled-tree re-reviews (dual — mandated tier), cross-model.
- Windows backlog from the C1-era tree: two isolated compile corrections
  (`a350746`, `d84a30d` in the release-diagnostic clone) to port or
  supersede deliberately; 98-failure matrix to classify (checkpoint doc
  "Windows backlog").
- Pre-A1 checklist (`GwzM5-8ProgressReviewF5.md` §9): I2 supersession
  banners (next docs task), operator escape story for unresolvable states,
  durable preservation-cursor decision, panic-invariant audit,
  `decode.rs:86` removal tied to the A1 diff.
- Rulebook P3 residue: section-anchor checking in the doc gate (Phase 2
  tooling).

## Next ordered actions

1. ~~Verify the §4.1 sealed-primitive implementation~~ **Done 2026-08-15**:
   `GwzM5-8R2C2PublicationAudit.md` — §4.1 satisfied, no P0-P2, four P3
   dispositions recorded (reserved fault families rescope; sealing
   perimeter extension as bounded package; legacy coexistence to R2-D
   scope; three strict-window tests).
2. Execute the complete per-fault interruption/recovery matrix; record
   per-key executed evidence (not inventory). Progress 2026-08-15: the
   21-key `catalog_bootstrap.*` matrix and the full checked_artifact suite
   executed green on this host (240 passed / 0 failed, macOS,
   workspace-target variant; command `cargo test --lib checked_artifact::`);
   the three P3-4 strict-window tests are implemented
   (`mutation_tests.rs`: in-place byte drift, destination-in-window,
   kind-swap-in-window) and pass; the reserved-families rescope note is
   filed as RemPlan §10. Remaining: extend the matrix to Git-directory
   targets; key or rescope the §6 parent-creation edge; then settle and
   launch the dual R2-C2 re-reviews (item 3).
3. Launch the two settled-tree R2-C2 re-reviews (dual, cross-model) once 1-2
   are green on a committed checkpoint.
4. Port/supersede the two Windows compile corrections; classify the Windows
   matrix before adding catalog behavior.
5. I2 supersession banners + TransitionDesign §1 authority correction +
   the §2.2 status-strip of `GwzM5-8Refactor.md` and the ledger (docs-only
   package, may interleave; the status-strip also unblocks the remaining
   L1-33 history sweep).
6. Then R4b-G → M5b → A1 per `GwzMergeCheckpoint-v0.10.5.md` resume order.

## Metrics (per checkpoint; §6 of the optimization plan)

| Checkpoint | Sessions | Review rounds | Found at freeze / impl / settled / escaped |
| --- | ---: | ---: | --- |
| (baseline starts with the next accepted checkpoint) | | | |
