# Current program checkpoint

Date: 2026-08-15
Status: **the single current-state authority for the GWZ merge program.
Update at every checkpoint boundary; keep concise; history belongs in git,
not in this file. Live status paragraphs in other documents are superseded
by this file (rulebook §7.3).**

## Exact tuple

| Repository | Commit | Note |
| --- | --- | --- |
| gwz-dev (root) | this coordinated commit | lock is authoritative |
| gwz-core | this coordinated commit | R2-C2 round-3 remediation (supersedes `c436180`) |
| gwz-cli | `3cca145` | Close R4b P1/P2 remediation gate |
| gwz-py | `929efb0` | Implement R4b reverse merge lifecycle |
| taut | `f008419` | 0.8.x fallible from_cbor line |

Exact hashes for "this coordinated commit" rows are pinned by
`gwz.conf/gwz.lock.yml` in the same commit; the next docs-only commit
restates them literally (per ReviewCode-3 P3-5).

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
   filed as RemPlan §10. **Complete 2026-08-15**: the §6 parent-creation
   edge is keyed (`catalog_bootstrap.git_parent_create` /
   `.git_parent_reobserve`, family now 23 keys, inventory 163) and the
   matrix is extended to Git-directory targets
   (`restart_and_substitution_matrix_covers_git_directory_targets`, all 23
   keys interrupted+restarted+converged); workspace matrix completeness
   assertion now spans both target variants; checked_artifact suite
   241 passed / 0 failed (macOS host); boundary checker green after the
   deliberate protected-tree digest update for the two edited trees; doc
   gate green. Native Linux/Windows execution remains at the R2-F gate.
3. Settled-tree R2-C2 re-reviews, round 3 (on `c436180`): **State-2 GO**
   (0 open P0-P2; two new P3s routed — the Git-parent dirent-barrier gap to
   the next matrix package/§6 errata, the Windows destination residual to
   R2-F criteria) and **Code-3 NO-GO** (P2-1 directory-interior
   acquisition-window gap, probe-proven; P3-1 checker blind to raw renames
   outside two files, exit-criterion violating; P3-2 comment overclaim;
   P3-3 digest-discipline break at `95d292f`; P3-4 allocation parity; P3-5
   stale tuple table). **Round-3 remediation implemented in this commit**:
   in-window interior re-check inside the sealed primitive for directory
   sources + destination-interior re-check for retirement (Code-3 option
   a), two new hooks + two inverted-probe regression tests (243/243 green);
   subsystem-wide raw-rename caller inventory in the boundary checker with
   exact allowlist + two adversarial checker unit tests; platform.rs
   comment corrected and §4.1 erratum filed (verification assigned to
   round 4); `try_reserve_exact` parity in the primitive; same-commit
   digest refresh (P3-3 discipline adopted as a lane rule). **Round-4
   focused dual re-reviews pending on this commit** — both axes, because
   the remediation changed durable-state publication behavior after
   State-2's GO.
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
