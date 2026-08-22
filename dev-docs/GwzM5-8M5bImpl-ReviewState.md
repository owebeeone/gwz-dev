# M5b-IMPL review — STATE axis (round 1)

Date: 2026-08-22
Axis: STATE (adversarial dual, peer-blind — `GwzM5-8M5bImpl-ReviewCode.md`
not read; it did not exist when this review began and was never opened).
Object: gwz-core commit `3e60529` ("Park the M5b no-ff semantics package
pre-review"), diff `c40e712..3e60529`.
Authorities read end-to-end before judging: `GwzM5-8M5bNoFfDesign.md`
(FROZEN, GO/GO at `66117b0`), the checkpoint's "M5b package DELIVERED and
PARKED" block (`CurrentProgramCheckpoint.md:39-61`),
`GwzM5-8ThinA1Amendment.md` (ACCEPTED — §1 acceptance-debt paragraph, §2.10),
`GwzM5-8DurableCursorAmendment.md` (D3 wire: per-owner `noop_commit` skip
rows + `reset_commit` bit, serde-optional, absent-when-unset),
`GwzM5-8ExactEvidencePlatformAmendment.md` (Clauses A/B′), and the F-1 gate
(`continue_op/execution.rs` + `tests/g23/continue_v0_gate.rs`).

## 0. Review basis and tree condition

Every object read in this review was taken from **committed blobs**
(`git show 3e60529:<path>`), never the working tree. Tree-condition record,
stated honestly: at review start the gwz-core working tree carried exactly
the warned-about other-lane files (`M src/checked_artifact/admission/mod.rs`,
untracked `src/checked_artifact/admission/tests.rs` — outside
`workspace_ops/`); **mid-review the other lane began actively editing the
tree** (pre_catalog provider files, protocol admission, and
`workspace_ops/merge/model/v0.rs` appeared modified, transiently
non-compiling). Validity of the executed evidence below is unaffected: the
suite runs recorded in §6 were compiled and launched **before** that edit
wave, from a tree whose `workspace_ops/` files were byte-identical to
`3e60529` (verified via `git status` at launch), and the long partition run
executed a binary built at that point. Two late corroborating runs
(`acceptance_v0`, `transition_matrix_v0`) rebuilt against the other lane's
later tree state and are weighted as corroboration only.

Diff inventory (verified against `git diff --name-only c40e712..3e60529`):

| File | Delta | Class |
| --- | --- | --- |
| `scripts/checks/check_checked_artifact_boundaries.py` | 1 line changed | protected-tree digest pin refresh for `workspace_ops/merge/v1_lifecycle/mod.rs` (the pin covers the module tree root + descendants + file set, so adding test files legitimately changes it). Verified correct: the checker runs green over a tree whose `v1_lifecycle/` equals the committed state. Only the pin line changed; scan logic and allowlists untouched. Tooling, not production. |
| `src/workspace_ops/merge/store/tests.rs` | +28 | cfg(test): T-2 no-ff body variant |
| `src/workspace_ops/merge/v1_lifecycle/tests/forward.rs` | +582/−26 | cfg(test): 6 P-FWD suites + shared harness |
| `src/workspace_ops/merge/v1_lifecycle/tests/mod.rs` | +3 | cfg(test): module wiring |
| `.../tests/no_ff_determinism.rs` | +279 (new) | cfg(test): 4 P-DET suites |
| `.../tests/no_ff_wire.rs` | +239 (new) | cfg(test): 3 P-WIRE suites + T-3/T-4 tripwires |
| `.../tests/reverse_no_ff.rs` | +319 (new) | cfg(test): 5 P-REV suites |

**Zero production lines: HOLDS.** All Rust changes are inside cfg(test)
compilation (`v1_lifecycle` is `#[cfg(test)]` at module root; `store/tests.rs`
is the store's test module). The 7th file is CI tooling whose only change is
the mechanically-required digest refresh (the R2-C2 lane rule: digest refresh
as the last pre-commit step). ≤10-file ceiling holds (7). Suite count claim
verified: 1 (store) + 6 (forward) + 4 (determinism) + 5 (reverse) + 5 (wire)
= **21 new suites**, matching the checkpoint block. Gross test insertions
1,424 (1,425 total − 1 py), net +1,398 — inside the raised 1,450 ceiling,
above the original 1,200 (see §7, ceiling ruling).

## 1. No-ff semantics vs the frozen design (mandate item 1)

### 1.1 TrueMergeCommit only via the frozen action; ambiguity classes are the design's

- `no_ff_external_fast_forward_is_ambiguous_never_adopted` (forward.rs:304)
  freezes the two-parent action, externally fast-forwards the target to
  `source_commit`, drives Continue through the production `service::run`
  seam, and asserts: `RecoveryRequired`, `resulting_commit.is_none()`
  ("an external fast-forward is never adopted as the frozen two-parent
  result"), `pending_action.is_some()`, head untouched by gwz. The
  classification is produced by the production classifier, not the test:
  `status/pending.rs:117-151` yields NotStarted only at the exact
  `before_commit` with the frozen spec revalidated
  (`validate_prepared_merge_upstream_state`), Completed only when
  `commit_matches_prepared_merge` field-matches (parents in order, message
  bytes, tree, both identity lines), everything else falling through to
  Ambiguous — exactly the design's §5.1/§5.2 sentence and its
  "fails on parent shape" mechanism. No invented outcome class exists
  anywhere in the package: every asserted state is a pre-existing enum
  value (`PendingActionReconciliation::{NotStarted, Completed,
  ExpectedConflict, Ambiguous}`, `OperationState::{RecoveryRequired, …}`).

### 1.2 §4.1 seven-input determinism — complete, nothing silently dropped

`no_ff_commit_oid_is_a_pure_function_of_the_frozen_action`
(no_ff_determinism.rs:26) is the strongest admissible form of the proof:
`offline_commit_oid` (no_ff_determinism.rs:222-249) reconstructs the commit
in a **second clone taken before execution** from exactly the persisted
action — parent[0] = `before_commit`, parent[1] = `source_commit`, tree =
`commit_spec.tree_oid`, message = `commit_message`, author and committer
rebuilt from `PendingGitSignature` including `time_seconds` and
`timezone_offset_minutes` — through the same libgit2 `commit()` constructor
production uses, and the test pins **OID equality** with the production
result. Because a commit OID is content-addressed over the full object
bytes, the seventh input (encoding/extra headers: none) is pinned
implicitly and *cannot* be silently dropped: any hidden input, header, or
message normalization difference would change the OID and fail the test.
The reconstruction consumes nothing but the persisted action (the clone has
never seen the integration commit — asserted by freezing before mutation).

Identity pinning per §4.3 is real, not asserted: `commit_file`
(`workspace_ops/tests/g02.rs:797-814`) writes `user.name`/`user.email`
("GWZ Test"/"gwz@example.invalid") into the repo-local config on every
fixture commit, so `merge_signatures`' effective-identity resolution is
environment-independent on CI; wall-clock time stays unpinned, which is
immaterial under from-freeze determinism because `time_seconds` rides
*inside* the frozen action that both sides of the OID comparison consume.

Arm coverage note (no finding): the OID-purity suites use the
fast-forwardable arm. This generalizes to the clean-true-merge arm because
execution is one code path over the frozen `TrueMergeCommit(spec)` — the
arms differ only in which `tree_oid` preparation froze — and production
self-verifies message bytes/tree/parents/signatures on every execution
(`merge_prepared.rs` self-verify). The clean-true-merge cell's own
mode-blindness is separately pinned (§1.5).

### 1.3 §4.2 crash windows and resume legality

- **Pre-mutation window**:
  `no_ff_preparation_persists_the_frozen_action_before_any_git_mutation`
  (forward.rs:347) + the `freeze_without_mutation` harness
  (forward.rs:1018) inject a panic at the instant the physical executor
  receives the participant action; the suite asserts the durable store
  already carries the identical action (`stored_action == frozen`) with
  head still at `before_commit` — preparation-persists-before-mutation
  ordering proven at the executor boundary, not inferred. Restart completes
  with the spec's tree and **frozen timestamps** entering the commit.
- **Post-publication window**: `no_ff_reexecution_after_crash_is_byte_identical`
  (no_ff_determinism.rs:54) crashes after ref publication, before the
  outcome write; asserts the published head equals the offline OID, then
  restart **adopts** (5 finalization-only executions — the pre-existing
  restart suite's exact count) with the same OID. The wall clock never
  re-enters.
- **Mid-execution window (the §4.2 sentence)**: the crash-window suite
  (no_ff_determinism.rs:93) reproduces the `merge_prepared.rs:328-333`
  shape exactly — frozen tree checked out into worktree+index, ref still at
  `before_commit` — then proves the full fail-closed protocol in the
  design's order: **abandonment refused** (`MergeRecoveryRequired`, frozen
  action surviving the refusal, head untouched), **continue** durably
  records the typed stop (`RecoveryRequired`, no adoption), a second
  continue is refused `RecoveryEvidenceMismatch` — which is precisely the
  committed mechanism at `authority/observe/forward.rs:497-501`
  (Ambiguous → `RecoveryEvidenceMismatch` in recovery resume verification;
  the record-then-refuse sequencing is the v1 recovery protocol the design
  cites, not a deviation) — and then the operator restore step: before-state
  worktree restored → reconciliation re-yields NotStarted → re-execution
  produces **the identical offline-computed OID**. That last assertion is
  the strongest witness of the §4.2 claim and lands verbatim as designed.
  Resume legality in the window is therefore: nothing resumes automatically,
  nothing is adoptable, both exits are typed, and the only path out
  reproduces the pure function's bytes.

### 1.4 Freeze-point authority

`frozen_signature_timestamps_survive_restart` (no_ff_determinism.rs:172)
pins persist → decode → execute → adopt equality of
`time_seconds`/offset on both identity lines — M5b-W1's first clause as an
executable row. Mode authority never re-derives: the suites set
`fixture.model.mode` and the only production consumption is
`prepared_mode(record.mode)` (`authority/observe/forward.rs:230-236`,
`NoFf → ForceMergeCommit`), unchanged by this diff.

### 1.5 The clean-true-merge and conflict cells

`no_ff_clean_true_merge_matches_normal_mode_bytes` (forward.rs:181) runs
Normal and NoFf over divergent-clean fixtures: frozen kind
`TrueMerge`/`Commit`, frozen tree ≠ source tree (the merge-index tree —
asserted), executed parents `[before, source]`, executed tree == frozen
tree, and the tree OID equal across modes ("mode-blind" — meaningful
because trees are content-addressed). `no_ff_true_merge_conflict_row_and_resolution_commit`
(forward.rs:229) proves the divergent row byte-comparable across modes at
the frozen-action level: kind, expected result, **commit_message**, and
tree equal between NoFf and Normal for both the conflict row
(`ExpectedConflict`, no commit_spec) and the resolution row
(`ResolveConflict`), with two-parent resolution parents asserted. Message
equality across fixtures is meaningful because `test_record` pins constant
ids. Read against the design's own from-freeze doctrine (cross-operation
OID equality is deliberately out of scope), this discharges the F-2 row:
every mode-sensitive input to the commit bytes is proven mode-blind.

## 2. Reverse/rollback semantics — §5.3 (mandate item 2)

All five P-REV rows exist under the design's exact names
(reverse_no_ff.rs:36, 41, 165, 200, 256).

### 2.1 `abort_refuses_to_abandon_a_created_two_parent_commit` — the adoption-then-rollback ruling

The suite creates the two-parent commit with the outcome write missing
(`execute_then_crash`), verifies the action is still pending, then drives a
full service-level Abort. Result: `resulting_commit == created` ("the
two-parent result is adopted, never abandoned"), participant `RolledBack`,
`pending_action` retired, operation `Aborted`, head restored to
`before_commit`. **This is the design's sentence.** §5.3: "an
already-created two-parent commit can never be 'abandoned': it reconciles
Completed first (`forward.rs:296-371`), and ambiguous observations
authorize no reverse entry" — and once Completed, the §5.3 abort path for a
completed participant *is* `ResetIntegrated` anchored on the recorded
result. Adoption-then-rollback is therefore not an invented third outcome;
it is the composition of the two frozen rows, and the test proves the
abandonment shortcut was not taken (an abandonment would have left
`resulting_commit` empty — asserted non-empty).

Durable end-state readability for other consumers, checked: the terminal
record (Aborted / RolledBack / `resulting_commit` retained /
`pending_action` and `pending_rollback` cleared) was **written by the
production service itself** through the validating checked store — it is
the same completed-then-rolled-back shape the accepted normal-mode
lifecycle produces, with only the (classifier-invisible) parent count
differing. Status reconciliation has nothing left to classify (no pending
action); rollback/preservation observers anchor on the recorded result
commit, which is retained as evidence rather than fabricated or erased;
D3's cursor derivation is anchor-based and mode-blind, so the shape feeds
it unchanged. No consumer is handed a state it cannot read.

### 2.2 Abandonment rows and the mid-window refusal

`assert_abandonment` (reverse_no_ff.rs:58-162) is the strongest form
available at the level the tree owns abandonment at:

- The NotStarted fact is **earned from the real repository against the real
  frozen action** (`reconcile_pending_action(...) == NotStarted`) — that
  reconciliation of a durable two-parent action is exactly the no-ff claim
  M5b owes, and it runs through the production classifier.
- The transition is then driven authority-first: `next_action` → bound
  `ExactObservationFact::Abandon` → `resolve_observation` →
  `transition::prepare`, asserting RollingBack/Preserving, action container
  retired **with its unknown fields** (an injected `m5b_probe` extension
  rides in and leaves with the container — record contract §8 row 382), no
  fabricated outcome, head at `before_commit`, worktree bytes untouched.
- The refusal legality of the same mechanism under ambiguity is proven at
  the *service* level by the crash-window suite (abandonment refused,
  `MergeRecoveryRequired`, frozen action surviving).

### 2.3 Rollback and preservation of the completed two-parent result

`rollback_resets_a_no_ff_merged_participant_to_before_commit` completes a
real no-ff integration through the production seam, then drives
`PendingRollbackActionV1::Participant { ResetIntegrated, RolledBack }`
through the service: head restored to the exact `before_commit`, the
rollback consumed (`pending_rollback.is_none()`), restored bytes checked
raw (`README.md == b"base\n"`) and the integration-introduced file gone —
the Clause A blob-exact expectation exercised on the Unix lane exactly as
§5.4 scopes it (no Windows-closure claim anywhere in the package; no
re-tightening of Clause B′ residue tolerance — no suite asserts private-area
emptiness). Mode never appears in any reverse classifier input — the
anchor is the recorded commit. One assertion in this suite is inert
(P3-2 below). `preservation_backup_ref_anchors_on_the_two_parent_result`
separates the owner anchor from the live tip with a real post-integration
commit and pins `owner.anchor == result`, `live_commit == live`, and the
frozen backup-ref name — journal contract §2's anchor sentence for the
two-parent case.

## 3. Composition (mandate item 3)

- **D3 order-independence: VERIFIED.** The implementer's claim holds under
  adversarial reading of every new assertion. No suite asserts the absence
  of preservation-cursor rows; no suite byte-compares serialized
  record/journal content in a preservation or reset flow (the only
  record-text assertions are *contains/not-contains of injected probe
  keys*, no_ff_wire.rs:146-155); the two structural-equality assertions on
  durable containers (`stored_action == frozen`, decoded `commit_spec ==
  spec`) sit inside `pending_action`/`commit_spec`, which D3's
  `noop_commit`/`reset_commit` never touch; `owners.len() == 1` counts
  derived preservation owners, which D3's marker rows do not add to. The
  suites pass in either landing order, and reverse_no_ff.rs's module header
  states the position in the file itself.
- **F-1 / T-6: HOLDS.** The gate files are absent from the diff — therefore
  byte-identical to pre-package state by construction — and both named
  suites executed green in this review
  (`v0_resume_rejects_forged_no_ff_mode_row`,
  `v0_resume_rejects_forged_two_parent_action_over_fast_forwardable_pair`,
  plus the gate's divergent-pair positive control). The package consumes
  the landed gate (T-3's allowlist cites its typed message and its
  fixtures) without touching it.
- **Thin A1: independent.** The new files import only `v1_lifecycle`
  internals, merge model/status, the fixture helpers, git2/serde_yaml/std —
  no catalog, checked-artifact, R2-E, or R3-R6 surface. Nothing in the
  package presupposes artifacts thin A1 moved off the gate.

## 4. Mode isolation (mandate item 4)

`ff_only_and_normal_matrices_are_unchanged_under_the_m5b_tree`
(forward.rs:384) is **not tautological**: its five rows are hard-coded
literal expectations transcribed from the frozen matrix (Normal+ff-able →
durable `FastForward`/`FastForwarded`; Normal+divergent → `TrueMerge`/
`Merged`; FfOnly+ff-able → `FastForward`; NoFf+ff-able → `TrueMerge`;
FfOnly+divergent → no frozen action, `Halted`, `MergeValidationFailed`),
so any drift in Normal/FfOnly behavior fails the suite against constants,
not against the code under test. The FfOnly rejection maps to the committed
`validate_mode` (`authority/observe/forward.rs:220-228`). The deeper
guarantee is structural: the diff contains zero production-reachable lines,
so pre-package Normal/FfOnly behavior is unchanged **by construction**, and
the executed evidence closes the loop: the full `workspace_ops::merge::`
partition ran green in this review (532 passed / 0 failed, 959.7s) on a
binary whose merge tree is byte-identical to `3e60529`, with
`acceptance_v0` (7/0) and `transition_matrix_v0` (4/0) green as
corroboration. The two pre-existing no-ff suites were extended by zero
semantic edits (the forward.rs hunks around them are imports and
visibility only; their assertions, including the 5-execution restart
count, are intact and re-ran green).

## 5. The stopped abandonment item (mandate item 5) — RULING: routing SOUND, no under-delivery

Facts verified in this review, not taken from the checkpoint:

1. The guard is exactly where the checkpoint says and is **structurally
   mode-blind**: `transition/reduce/participant.rs:105-118`
   (`AbandonNotStartedAndBeginRollback`: `abandon(...)`, entry-origin
   check, anticipated-model match, binding at
   `"@operation"/"begin_rollback"/"preflight"`) and the `abandon` helper
   (:220-241: operation state ∈ {Executing, Halted}, proof bound at
   `"participant_action"/"not_started"`, `pending_action` present →
   cleared). No line reads `mode`, action kind, or commit spec. The diff
   does not touch `transition/` — the behavior is pre-existing.
2. The abandonment suites drive the **same level as the tree's accepted
   precedent**, verbatim: `dispatcher_attempt_matrix::
   abort_and_preserve_abandon_only_their_bound_not_started_owner` (:237)
   uses the identical `for_test` doors and the identical
   `next_action → resolve_observation → prepare` chain. M5b's suites are
   strictly stronger than the precedent at that level (real-repo-earned
   NotStarted, worktree byte assertions, unknown-field retirement).
3. The frozen design does **not** oblige the service seam for P-REV: §7's
   placement note binds the `service::run` seam requirement to
   "Suites P-FWD/P-DET" (Q5) only, and §5.3's cited abandonment machinery
   is the transition kernel (`transition/mod.rs`, `transition/reverse_entry.rs`,
   `transition/effect.rs`) — the level the suites exercise.
4. Fixing service-level reachability would require production lines inside
   `v1_lifecycle` non-test code — a mandated-dual edit under Q6's rule and
   a violation of the ratified 0-production-line ceiling. The design's own
   §8.3 sentence makes that a return-to-freeze, not an M5b-IMPL patch.
5. The routing target is the correct surface: thin-A1 §1 already binds the
   v1_lifecycle P2/P3/P4 acceptance debt (the lanes containing this
   reducer) to the A1 activation review "dual, at §4.2's activation tier",
   with §4 restating it as that review's explicit input per L1-19. A
   mode-blind service-level abandonment hole is precisely an
   activation-readiness defect: pre-A1 it is unreachable from production
   by the §6 boundary; at A1 it must either be fixed or block.

Therefore: testing abandonment at the authority/reducer level **is
sufficient for the design's §5.3 obligations as frozen**, and the routing
of the service-level unreachability to the A1 activation review's
acceptance-debt surface is **sound, not a gap** — with one strengthening
recorded as P3-3: the "Normal-mode control probe fails identically"
sentence currently has no committed executable witness; the mode-blindness
is corroborated here by code reading, but the A1 review deserves a pinned
refusal-shape test rather than a checkpoint sentence.

## 6. Wire obligations (mandate item 6) and executed evidence

- **T-2, before any row read**:
  `installed_but_disabled_v1_no_ff_body_reports_a1_before_any_row_is_read`
  (store/tests.rs:142) writes a raw open v1 file whose body carries
  `mode: no_ff` and a two-parent `pending_action` fragment that is
  deliberately not a decodable record — so the asserted
  `UnsupportedRecordVersion` with `MergeRecordRequiredWave::A1` context can
  only have come from the envelope gate, proving no body row was reached.
  Green.
- **Forged non-canonical spec** (no_ff_wire.rs:24): the space-padded name
  round-trips the durable store (its successful `load_open` is itself the
  proof the wire bounds admit it), executes through libgit2's crud-trim,
  and the still-pending action then classifies Ambiguous on restart-based
  reconciliation — `RecoveryRequired`, no adoption, second continue
  `RecoveryEvidenceMismatch`. This encodes §4.6's corrected sentence
  including the round-2 nuance (the ambiguity materializes because the
  crash kept the action pending). Availability-only, no wrong evidence.
- **Two-parent restart reconciliation rows** (no_ff_wire.rs:68):
  NotStarted at the exact freeze; intent-mismatch → Ambiguous;
  merge-head-mismatch → Ambiguous; Completed only at the exact created
  commit. The ExpectedConflict leg of the classification table is exercised
  by the forward conflict suite's stop-and-resume.
- **Unknown-field round-trip** (no_ff_wire.rs:115): probes inside both
  `pending_action` and `commit_spec` survive a rewrite that keeps the
  action pending (recovery stop via injected worktree drift) and retire
  with the container on exact reconciliation.
- **No new outcome, no third source of truth, v0 untouched**: every
  asserted class/state pre-exists this package; the only authorities any
  suite consults are the persisted action and `reconcile_pending_action`
  (the offline reconstruction is §4.1's definition made executable, not a
  new authority); no v0 production or v0-suite file is in the diff.

**Executed evidence (this review, committed-tree binary):** all 21 new
suites green; both T-6 suites + positive control green; boundary checker
green (digest pin verified); `workspace_ops::merge::` partition 532/0
(total 532+867 = 1,399 tests in that binary — matching the checkpoint's
recorded suite size); `acceptance_v0` 7/0 and `transition_matrix_v0` 4/0
(corroborating runs, post-edit tree). The only red anywhere this session
was the other lane's transient mid-edit compile state, outside this
object.

## 7. Ceiling and settle-obligation posture

- **Test-line ceiling raise 1,200 → 1,450 (checkpoint block, "subject to
  the M5b dual review"): RATIFIED from this axis.** The raise followed
  §8.3's return-to-the-freeze rule in substance — recorded, bounded,
  package-scoped, with the 0-production-line and ≤10-file ceilings
  untouched. The overage is real (gross +1,424) but every suite maps 1:1
  to a §6/§7 design name (roster verified complete in §1-§6 above) and
  ~230 of forward.rs's diff lines are shared harness/visibility plumbing
  consumed by all three new files. Against the program's recorded 2-10x
  overrun history, +18.7% on the "honest risk" budget with the hard
  ceiling intact is within the freeze's intent.
- **Settle items correctly parked, none silently dropped** (named in the
  checkpoint block): T-5 retained-reader no-ff envelope fixture (the
  existing `cases.json` already carries the v0 legacy-defect cases; the v1
  envelope pair is the settle addition), M5b-W1 record-contract banner,
  ChangeBudget row, §5.4/Q7 classification-ledger sentence, clean-tree
  tuple re-cut. Per design §8.1 item 3 these are settled-acceptance
  criteria, not IMPL-package criteria; the checkpoint block names each.
- Review-tier note: the diff is test-confined plus one tooling pin, so
  Q6's rule would have permitted single-axis; the program ran the recorded
  M5b dual anyway (resume-policy gate list). No objection.

## 8. Findings

**P0: none. P1: none.**

**[P2-1] The T-3/T-4 structural tripwires do not execute on every push —
the frozen design's named lane property is not delivered.**
Design §6 T-4 (resolved variant), verbatim: "The State axis's
machine-checked-not-convention-checked requirement is preserved: the
structural scan executes in CI on every push via the checker lane."
Delivered form: both scans are cargo tests
(`no_ff_wire.rs:158` `no_writer_output_or_positive_fixture_serializes_the_no_ff_wire_row`,
`:180` `force_merge_commit_construction_sites_stay_v1_lifecycle_only`) —
exact against the committed tree (independently re-derived here:
`ForceMergeCommit` appears in exactly `git/gitbackend/contract.rs`,
`git/tests/g12.rs`, `v1_lifecycle/authority/observe/forward.rs`; literal
`mode: no_ff` in exactly `continue_op/execution.rs`, `store/tests.rs`,
`tests/g23/continue_v0_gate.rs`; `merge_prepared.rs` consumes the mode via
equality with `AllowFastForward` at :165, so the design's older
"production matches on the variant" citation no longer bites). But the only
push-triggered workflow (`checked-artifact-boundary.yml`) runs the Python
checker, its unittests, and `cargo clippy --all-targets` — **no cargo
test** — so on pushes between dispatched matrix/release runs the scans do
not execute. A production `ForceMergeCommit` construction site or literal
no-ff serialization landing outside `v1_lifecycle/**` (where the digest pin
would not fire) would pass push CI and surface only at the next executed
test run. Compensating controls exist (clippy compiles all targets on
push; the v1_lifecycle digest pin trips any edit of the only construction
file; T-2/T-1/T-6 production gates; R4b-G's call-graph gate and the M5b
settle gate both run the suite), so this is a cadence weakening, not an
unsoundness — but it is a specific frozen State-axis property, recorded at
freeze as the justification for the structural-scan variant, and it is not
met as written. **Remediation (cheap, at or before M5b settle):** run the
two named tests in the push workflow (one
`cargo test --lib force_merge_commit_construction_sites_stay_v1_lifecycle_only no_writer_output_or_positive_fixture_serializes_the_no_ff_wire_row`
step), or port the two needle scans into
`check_checked_artifact_boundaries.py` (checker lane proper); a recorded
freeze erratum accepting the weaker lane is the fallback if the operator
prefers no CI change.

**[P3-1]** The FfOnly+divergent matrix row asserts non-freezing at the
capture level only (`runtime.frozen.is_none()`, forward.rs:439-443:
"ff_only must never freeze a true-merge action"). `validate_mode` fires
pre-persist in code, but the durable claim deserves its own assertion: add
`assert!(stored_action(&fixture).is_none())` to pin that no action was
persisted either.

**[P3-2]** Inert assertion in the rollback suite
(reverse_no_ff.rs:241-244): `commit_facts(&fixture.member,
&fixture.before).tree == before_tree` compares the immutable before
commit's tree to itself and pins nothing. The row's substance is carried by
the raw `README.md` byte check and `source.txt` absence; replace the inert
line with a worktree-vs-`before_commit`-tree cleanliness assertion (empty
diff/status) to widen the Clause-A witness from one file to the image, or
delete it.

**[P3-3]** The mode-blindness of the service-level abandonment
unreachability rests on an un-committed experiment ("Normal-mode control
probe fails identically", checkpoint block). This review corroborated it
structurally (participant.rs:105-118, abandon() :220-241 read no
mode/kind), but the A1 activation review should inherit an executable
witness: commit a small probe pinning the current service-level refusal
shape for a NotStarted action in both Normal and NoFf modes (equal
outcomes), as an A1-checklist input.

**[P3-4]** Q5 seam coverage is partial by necessity: four new suites enter
through the production `service::run` seam (`run_production`,
forward.rs:936-947, `V1Runtime`-bounded and sealed), but
instrumentation-dependent rows (up-to-date zero-execution count,
crash-injection rows) go through cfg(test) `run_test` because wrappers
cannot be `V1Runtime`. Acceptable — both seams share `run_with_runtime`
byte-for-byte — but A1's re-pointing checklist should name the
wrapper-based rows as needing adaptation rather than verbatim reuse.

**[P3-5]** T-3's scan sees only the literal `mode: no_ff` in `.rs` files at
file granularity: serde-constructed fixtures (as g23's deliberate negatives
now are) and runtime writer outputs are invisible to it, and a new literal
inside an allowlisted file would not trip it. The residual is behaviorally
self-closing (a green positive v0 no-ff fixture is impossible against
`validate.rs:12-14`, the T-6 gate, and `UnsupportedLegacyMode`), but the
tripwire comment should state the literal-spelling scope so A1's inverter
does not over-read it, and the settle gate may extend the needle set
(e.g. `mode:no_ff`, YAML-fragment constructors) cheaply.

## 9. Verdict

**GO.** Zero P0, zero P1; one P2 (tripwire execution lane vs the frozen
every-push property — remediable at or before M5b settle without touching
the package's semantics); five P3s. The installed no-ff semantics are
durable-state-correct against the frozen design: the two-parent commit is
proven a pure function of the persisted action by offline OID equality
including both crash windows; the reverse rows consume the frozen action
through the design's own classifiers with adoption-then-rollback (not
abandonment) for a created commit and a durable end-state every consumer
already reads; composition with D3 (either landing order), the landed F-1
gate (byte-identical, green), and thin A1 is clean; Normal/FfOnly matrices
are pinned against constants and unchanged by construction (zero
production lines — verified, with the checker-pin refresh verified
correct); and the stopped abandonment item is correctly scoped out and
soundly routed to the A1 activation review's acceptance-debt surface. The
raised test ceiling is ratified from this axis. P2-1 and the P3s route to
the M5b settle gate; none blocks package acceptance.
