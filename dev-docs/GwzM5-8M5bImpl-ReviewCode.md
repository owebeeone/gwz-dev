# M5b-IMPL review — CODE axis (adversarial, round 1 of 2)

Date: 2026-08-22
Axis: CODE (peer-blind vs the State reviewer; `GwzM5-8M5bImpl-ReviewState.md` not read)
Object: gwz-core commit `3e60529` ("Park the M5b no-ff semantics package pre-review"),
diff `c40e712..3e60529` — 6 cfg(test) files under `src/workspace_ops/merge/`
(1,424 inserted test lines, 26 deleted), plus the same-commit tree-pin refresh in
`scripts/checks/check_checked_artifact_boundaries.py` (1 line) and the checkpoint
delivery record (`CurrentProgramCheckpoint.md:39-61`).
Authority: `GwzM5-8M5bNoFfDesign.md` (FROZEN 2026-08-16, GO/GO), read end to end
(1,163 lines). All code review was performed against a pristine `git archive`
extraction of `3e60529` (the working tree carries another lane's untracked
red-by-design `src/checked_artifact/admission/tests.rs`; at `3e60529` the committed
`admission/mod.rs` declares no `mod tests`, so the committed tree is unaffected —
verified).

Verdict: **GO** — 0 P0, 0 P1, 1 P2, 3 P3.

---

## 1. Zero-production-line verification (the package's central claim) — PROVEN

The claim is not merely consistent; it is mechanically proven at three independent
levels on the pristine extraction:

1. **Diff surface.** `git diff --numstat c40e712..3e60529` touches exactly seven
   files: six `.rs` test files (`store/tests.rs` +28, `v1_lifecycle/tests/forward.rs`
   +556/-26, `tests/mod.rs` +3, `tests/no_ff_determinism.rs` +279,
   `tests/no_ff_wire.rs` +239, `tests/reverse_no_ff.rs` +319) and the Python checker
   (+1/-1, not a build input). No `Cargo.toml`, no `build.rs` (none exists in the
   crate), no other file.
2. **Gating chain, re-derived.** `merge/mod.rs:23-24` — `#[cfg(test)] mod
   v1_lifecycle;` (with the compile sentinel at :25-26) removes the entire
   `v1_lifecycle/**` subtree, including all five changed files there, from non-test
   builds before file load. `store/mod.rs:378-379` — `#[cfg(test)] mod tests;` does
   the same for `store/tests.rs`. Visibility audit: every new item in the changed
   files is `pub(super)`/private inside the cfg(test) subtree; nothing is exported
   across the boundary.
3. **Byte-effect on the non-test build.** `cargo build --locked` (non-test) on the
   pristine extraction is green; its dep-info
   (`deps/gwz_core-38e97a061b3ca858.d`, 282 source files) contains **none of the six
   changed files and no `v1_lifecycle` path at all** — the compiler never reads a
   changed byte in a production build, so the production artifact is byte-unaffected
   by this package.

The §8.3 hard ceiling "production-reachable delta 0 lines / 0 files" **holds**.

## 2. Obligation coverage — design §6/§7 → the 21 delivered suites

Census: exactly 21 new `#[test]` fns (forward.rs 6, no_ff_determinism.rs 4,
reverse_no_ff.rs 5, no_ff_wire.rs 5, store/tests.rs 1), matching the checkpoint's
"21 new green suites". Every delivered suite carries a design-named §6/§7 identity;
no extra-scope suite exists.

**P-FWD** (design §7): 6 NEW rows, all delivered under the design's exact names in
`v1_lifecycle/tests/forward.rs` — `no_ff_up_to_date_adopts_verify_up_to_date_without_execution`
(:145, asserts zero executor invocations and untouched head),
`no_ff_clean_true_merge_matches_normal_mode_bytes` (:181, asserts the frozen tree is
the merge-index tree — `assert_ne!` vs the source tree — and cross-mode tree/parents
equality; see P3-1 for the message-bytes gap),
`no_ff_true_merge_conflict_row_and_resolution_commit` (:229, cross-mode equality of
kind/expected_result/commit_message/commit_spec for both the conflict and resolution
freezes), `no_ff_external_fast_forward_is_ambiguous_never_adopted` (:304),
`no_ff_preparation_persists_the_frozen_action_before_any_git_mutation` (:347),
`ff_only_and_normal_matrices_are_unchanged_under_the_m5b_tree` (:384 — the named
rows: ff_only+true-merge rejected typed (`MergeValidationFailed`, Halted, nothing
frozen), normal ff stays a durable `FastForward`). The two "exists" rows (:84, :106)
are present, unrewritten, and re-run green.

**P-DET**: 4/4 delivered in `tests/no_ff_determinism.rs` under the design's exact
names (:26, :54, :93, :172).

**P-REV**: 5/5 delivered in `tests/reverse_no_ff.rs` (:36, :41, :165, :200, :256).
The order-independence instruction (State P2-2) is honored: the module doc states
it (:9-11) and no assertion in the file names preservation-cursor rows or their
absence.

**P-WIRE**: 3 NEW rows delivered in `tests/no_ff_wire.rs` —
`forged_non_canonical_signature_spec_executes_then_never_reconciles` (:24),
`two_parent_restart_reconciliation_rows` (:68, including the intent-mismatch and
merge-head-mismatch Ambiguous rows citing `integration.rs:180-189`),
`no_ff_record_unknown_fields_survive_rewrite_and_retire_on_reconciliation` (:115 —
probes inside both `pending_action` and `commit_spec`, survival through a real
durable rewrite (the RecoveryRequired transition), retirement on exact
reconciliation asserted on the record text). The "matrix accept/reject rows —
exist" row re-runs green.

**Tripwires**: T-1 exists and re-runs green
(`validate::tests::custom_messages_validate_while_no_ff_remains_reserved`, exact-name
run, 1 passed). T-2's no-ff **body** variant is delivered:
`store/tests.rs:141` `installed_but_disabled_v1_no_ff_body_reports_a1_before_any_row_is_read`
— production `FileMergeStore.discover_open`, typed `UnsupportedRecordVersion` with
`MergeRecordRequiredWave::A1`, envelope-level ("before any row is read"). T-3
delivered as a source-literal scan (finding P2-1 below). T-4 delivered as the
mention-scan `force_merge_commit_construction_sites_stay_v1_lifecycle_only`
(`no_ff_wire.rs:180`; see §3 and P3-3). T-6 verified (§5).

**Routing claims, verified against the design's own text:**

- **T-5 → retained-reader lane**: design §6 words T-5 as "T-5 retained-reader
  manifest lane: v0.10.2 rejects the v1 no-ff envelope pair … fixture addition to
  the existing harness, not a new harness"; no retained-reader/v0.10.2 harness
  exists in gwz-core `src/` (grep-verified), and §8.1 item 3 places "tripwires
  T-1..T-6 in the gate" at **settled acceptance**, not at IMPL parking. Routing is
  design-consistent.
- **T-4's CI mechanism → checker lane**: design §6 T-4 (resolved variant) says "the
  structural scan executes in CI on every push via the checker lane". No such
  checker exists yet under `scripts/checks/` (listing verified); the delivered unit
  scan is machine-checked in CI via the test suite in the interim. Consistent, with
  the record-completeness nit filed as P3-3.
- **W1 / ledger sentence / ChangeBudget row / clean-tree re-cut → acceptance
  ritual**: design §8.1 item 3 defines settled acceptance as "suites green, ledger
  row reconciled, tripwires T-1..T-6 in the gate …; freeze tuple re-cut against a
  clean committed tree", and §8.1 item 1 constrains W1 only with "lands on the
  record contract only after both reviews accept" (a not-before, satisfied by the
  2026-08-16 IF GO/GO; the record contract carries no M5b banner yet —
  grep-verified, consistent). Routing is design-consistent **except** the
  ChangeBudget row, where design D6 (§2.2) says the row "must be added at freeze
  time" — freeze time has passed and `GwzM5-8ChangeBudget.md` contains no M5b row
  at all (grep-verified). Filed as P3-2.

**Q5 seam**: satisfied. `service::run` and the cfg(test) `run_test` delegate to the
same `run_with_runtime` body (`service.rs:69-98`); the delivered `run_production`
helper (`tests/forward.rs:930-948`) drives the production `service::run` for every
plain-runtime no-ff suite, and its doc comment records the rule ("only
fault-injecting wrappers (which cannot be `V1Runtime`) go through `run_test`" —
wrappers are excluded from `V1Runtime` by the sealed `ProductionRuntime` bound,
`service.rs:42-53`). A1 can re-point the suites by swapping the entry fn without
rewriting them.

## 3. Test substance — adversarial checks

- **P-DET offline reconstruction is genuinely independent.** `clone_member`
  (`no_ff_determinism.rs:212-216`) clones the member **before** execution, while
  the integration commit exists nowhere (`frozen_no_ff` freezes via a pre-mutation
  crash; head asserted at `before`). `offline_commit_oid` (:220-245) reads only
  `PendingMergeAction` fields — `before_commit`, `source_commit`,
  `commit_message`, `commit_spec.{tree_oid,author,committer}` — the seven §4.1
  inputs, against the clone's ODB (`repository.commit(None, …)`, no ref moved). A
  libgit2 local clone copies/links the object store and sets no alternates, so
  objects created later in the original are invisible to it; temporally the
  offline OID is computed before the production run in all three suites that use
  it. Equality with the production `resulting_commit` is content-addressed OID
  equality. This is the strongest form the design asked for. (Minor note, not a
  finding: the reconstruction consumes the dispatch-captured action; capture ≡
  disk is separately proven by `stored_action(...) == frozen` at
  `tests/forward.rs:354` and `decoded == spec` at `no_ff_determinism.rs:177-178`.)
- **Identity pinning claim is code-true**: `commit_file` sets repo-local
  `user.name`/`user.email` (`workspace_ops/tests/g02.rs:797-807`), the g02
  precedent the design §4.3 names.
- **`no_ff_external_fast_forward_is_ambiguous_never_adopted`** tests what it
  claims: action frozen pre-mutation; external `fast_forward` to `source`;
  production continue yields `RecoveryRequired`, row not Merged,
  `resulting_commit` none ("never adopted"), `pending_action` retained, head left
  at `source` (no mutation authorized). Sound against
  `status/pending.rs:117-151`: NotStarted requires live head == `before_commit`;
  Completed requires `commit_matches_prepared_merge`, which fails on parent shape
  for the externally fast-forwarded head; fall-through is Ambiguous.
- **§4.2 mid-execution window** (`no_ff_determinism.rs:93-169`): the suite
  reproduces the exact post-crash state the design defines for the
  `merge_prepared.rs:328-333` window — frozen tree materialized into
  worktree+index via `checkout_tree` with **no ref moved**, head asserted still at
  `before_commit` — rather than injecting a crash inside the backend (the harness
  seam cannot split `execute` internally; the classifier consumes only the
  resulting repo state, so the reproduction is faithful to the classifier's
  inputs). It then asserts every §4.2 sentence in order: abandonment refused
  (`MergeRecoveryRequired`, frozen action survives, head untouched), continue
  records the typed stop (`RecoveryRequired`, no adoption), a second continue is
  refused (`RecoveryEvidenceMismatch` — the design's cited code), operator restore
  → re-execution produces the **identical offline-computed OID**.
- **`abort_refuses_to_abandon_a_created_two_parent_commit`**
  (`reverse_no_ff.rs:165-197`) matches design §5.3's sentence exactly ("an
  already-created two-parent commit can never be 'abandoned': it reconciles
  Completed first"): created-but-unrecorded commit + service-seam Abort →
  `resulting_commit` adopted equal to the created OID ("adopted, never
  abandoned"), then rollback proceeds from that recorded anchor
  (`RolledBack`/`Aborted`, head restored to `before_commit`). The row's other
  parenthetical, "ambiguity path asserts typed stop", is discharged in the P-DET
  window suite (:108-126, abandonment refused `MergeRecoveryRequired` under
  ambiguity) — covered in-corpus, noted for the obligation map.
- **T-3/T-4 needles are runtime-spelled and not self-hits**: `format!("mode:{}no_ff", ' ')`
  and `format!("{}MergeCommit", "Force")` — verified the literals do not occur in
  the scan file; the T-4 allowlist was independently reproduced by grep
  (`ForceMergeCommit` occurs in exactly `git/gitbackend/contract.rs`,
  `git/tests/g12.rs`, `v1_lifecycle/authority/observe/forward.rs`;
  `merge_prepared.rs` reaches its forced arm as the fall-through after an
  `== AllowFastForward` guard and never names the variant). The mention-scan
  over-approximates construction sites in the safe direction, and
  `GitPreparedMergeMode` derives only `Clone, Copy, Debug, Eq, PartialEq`
  (`contract.rs:141-146`) — no serde/Default, so the variant cannot be constructed
  without its name appearing.

## 4. Stopped item — service-level abandonment: claim VERIFIED empirically

Checkpoint claim: "service-level abandonment of a NotStarted frozen action is
unreachable MODE-BLIND (pre-existing reducer guard at
`transition/reduce/participant.rs:105-119`; Normal-mode control probe fails
identically)".

- **Static**: the abandonment chain — `reduce/participant.rs:105-131` (the
  `AbandonNotStartedAndBeginRollback/Preservation` arms), `abandon()` (:220-241:
  requires state ∈ {Executing, Halted}, bound proof, `pending_action.is_some()`),
  `transition/reverse_entry.rs` (entry kinds/predecessors), and the reverse
  observer (`authority/observe/reverse/rollback.rs:144-196`, which does mint the
  Abandon fact on NotStarted) — contains **zero** references to
  `MergeExecutionMode`/`NoFf`. The only mode-consulting sites in the entire
  non-test v1 lifecycle are `validate_mode`/`prepared_mode`
  (`authority/observe/forward.rs:220-236`), exactly the two design §5.1 froze.
- **Empirical (this review's own probe, in a scratch copy of the extraction)**:
  a service-seam `run(…, V1LifecycleRequest::Abort)` over a clean, genuinely
  NotStarted frozen action fails
  `code=MergeRecoveryRequired, msg="v1 transition predecessor or authority
  mismatch"` with the action still pending — **identically for `mode: no_ff` and
  for Normal mode**. The mode-blindness claim and the "unreachable at service
  level" claim are both true; the package did **not** test at the wrong level.
- **Precedent claim verified**: `dispatcher_attempt_matrix.rs:236-321`
  (`abort_and_preserve_abandon_only_their_bound_not_started_owner`) is the tree's
  existing abandonment harness at exactly this authority/reducer level, for a
  mode-blind owner. The delivered `assert_abandonment`
  (`reverse_no_ff.rs:57-162`) strengthens the precedent: the `NotStarted` fact is
  **earned** from the real repository against the real frozen two-parent action
  (`reconcile_pending_action == NotStarted` asserted before any `for_test` proof
  is minted), and the record-contract §7 assertions (pre-action state untouched,
  no fabricated outcome, unknown fields retiring with the container) are all
  present.
- **Routing target exists**: the thin-A1 amendment's §1 acceptance-debt surface
  (`GwzM5-8ThinA1Amendment.md:44,136,187,242`) is real; the checkpoint routes the
  service-level gap there. Note for the A1 reviewer this record implies: the
  record-contract §7 abandonment mechanism is currently not dischargeable
  end-to-end through the service seam **for any mode** — a pre-existing R4b-lane
  gap, correctly not fixed inside a 0-production-line package.

## 5. T-6 — verified both ways

- `git diff c40e712..3e60529 -- src/workspace_ops/tests/g23/continue_v0_gate.rs`
  is **empty** (0 bytes) — byte-identical to pre-package.
- Both named suites run green by `--exact` name on the pristine extraction:
  `v0_resume_rejects_forged_two_parent_action_over_fast_forwardable_pair` and
  `v0_resume_rejects_forged_no_ff_mode_row` (2 passed, 0 failed).

## 6. Ceiling — the 1,450 raise record is accurate and its justification code-true

- **Counts**: gross test insertions = 28+556+3+279+239+319 = **1,424 exactly**
  (the checkpoint's number; deletions 26, all in forward.rs, net 1,398). Files:
  **6** cfg(test) files (+1 checker script) — the ≤10-file ceiling holds.
  Production: **0** (§1). 1,424 ≤ 1,450.
- **"Every line maps to a design-named §6/§7 obligation"**: verified at suite
  granularity — all 21 delivered suites carry design-named identities (18 §7 rows
  + T-2/T-3/T-4); zero unnamed scope.
- **"~140 lines are shared harness"**: conservative. My count of the forward.rs
  shared-helper additions (the `pub(super)` region: `run_production`, `Crash`,
  `CapturingRuntime`, `freeze_without_mutation`, `frozen_no_ff`,
  `execute_then_crash`, `record_path/record_text`, `inject_unknown_field`,
  `stored_action`, `head_commit`, `CommitFacts`/`commit_facts`/`commit_tree`,
  `Kind::CleanDivergent` fixture arm) is ~220 non-blank inserted lines (~194-216
  net of the 26 deletions), plus 3 mod-registration lines and ~128 file-local
  helper lines in the three new files. The record **understates** the harness
  fraction, which strengthens rather than weakens its justification: the overrun
  is harness and design-mandated breadth, not scope creep.
- **Rule fit**: the record is an explicit lane-owner decision, cites "§8.3's
  return-to-the-freeze rule", is scoped "for this package only", holds the
  0-production and ≤10-file ceilings unchanged, and subjects itself to this dual
  review — i.e., not a quiet widening. Whether a checkpoint block (vs a design
  revision + ledger row) is the proper *form* of a return-to-freeze is the lane
  owner's/State axis's process call; from the CODE axis the numbers are accurate
  and the justification is code-true. The ledger-row absence is P3-2.

## 7. Gates — all green on the pristine extraction of `3e60529`

- `cargo build --locked` (non-test): green (and dep-info proves §1).
- `cargo fmt --check`: clean (exit 0).
- `cargo clippy --locked --all-targets`: clean.
- Four new/extended v1_lifecycle suites (forward, no_ff_determinism,
  reverse_no_ff, no_ff_wire): **36 passed, 0 failed** (6.55s).
- `store::tests`: 19 passed (includes the new T-2 body row).
- P-V0 re-runs: `acceptance_v0` + `transition_matrix_v0` + `g23`: **122 passed,
  0 failed**; T-1 pin test green by exact name.
- Full lib suite minus the one pre-existing >585s test
  (`root_fault_matrix::every_root_physical_and_successor_boundary_recovers_without_repeating_mutation`,
  outside this package's file set, green in CI at run 13 per the checkpoint):
  **1,391 passed, 0 failed, 1 ignored** (586.9s). Binary total 1,393 tests —
  consistent with the checkpoint's 1,399 (= 1,393 committed + the other lane's 6
  untracked red-by-design admission tests, which are absent from the committed
  tree).
- Boundary checker (`check_checked_artifact_boundaries.py`, python3.13): **ok**
  ("15 visible entries, 5 classified modules") — the same-commit v1_lifecycle
  tree-pin refresh (the single-line digest change for
  `workspace_ops/merge/v1_lifecycle/mod.rs`) is correct for the committed tree.

## 8. Findings

### P2-1 — T-3 as delivered under-implements the frozen §6 T-3 mechanism

Design §6 T-3 (frozen): "the assert therefore scans **writer outputs and
positive-path fixtures** only, with the negative-fixture files enumerated as the
exact allowlist" — and the enumerated negative fixtures are
`workspace_ops/tests/g23/compatibility_v0.rs:94-97`,
`compatibility_v0_edges.rs:38,68,164-180`, `atomic_upgrade_v0.rs:79-85`.

Delivered (`no_ff_wire.rs:158-172`,
`no_writer_output_or_positive_fixture_serializes_the_no_ff_wire_row`): a **source-
literal** scan for the runtime-spelled needle `"mode: no_ff"` with the allowlist
`[continue_op/execution.rs, merge/store/tests.rs, g23/continue_v0_gate.rs]`.

The design-enumerated negative fixtures are not hits for this needle because they
forge their rows **programmatically** (`no_ff.mode = MergeExecutionMode::NoFf;`
then `serde_yaml::to_string` — verified at `compatibility_v0.rs:94-97` and
`atomic_upgrade_v0.rs:79-85`). Consequently the delivered wire cannot trip on the
exact class of fixture the frozen allowlist was built around: a future
**positive-path v0 fixture** written the same way as the existing negatives
(clone a record, set `MergeExecutionMode::NoFf`, serialize) would not be caught,
and no writer-output scan exists. The scan is green, runtime-spelled, and useful
for the literal-YAML class, but it is a different mechanism from the frozen text
with a real blind spot.

Not a P1 because: the guarded boundary has independent enforcement (T-1 request
rejection, T-2 envelope rejection, the landed v0 forged-action resume gate — T-6
green — and the M5a "unsupported" creation row), and §8.1 item 3 formally binds
tripwires at settled acceptance, where remediation can land.

**Remediation (minimal)**: keep the literal scan and add a second runtime-spelled
needle scan (e.g. `format!("{}::NoFf", "MergeExecutionMode")`) over v0-side
source, with the design-enumerated negative-fixture files plus the legitimate
model/validate/dispatch/v1-lifecycle sites as the exact allowlist — or implement
the writer-output form the frozen text describes. Land by M5b settled acceptance.

### P3-1 — F-2 row: cross-mode message-byte equality not asserted

`no_ff_clean_true_merge_matches_normal_mode_bytes` (`tests/forward.rs:181-226`)
discharges the F-2 row ("produces a commit byte-for-byte equal to normal mode's")
via frozen kind/expected_result, `tree != source tree`, per-mode parents/tree
equality with the frozen spec, and cross-mode **tree** equality — but never
compares `commit_message` bytes across the two modes (the conflict-row suite does:
`tests/forward.rs:295`). `CommitFacts.message` is already captured; the assertion
is one line. Timestamp fields are legitimately excluded (two separately frozen
operations differ by design §4.2), but message bytes are mode-independent and
should be pinned. **Remediation**: collect `facts.message` per mode and assert
equality alongside `trees[0] == trees[1]`.

### P3-2 — ChangeBudget ledger row for M5b still absent; raised ceiling lives only in the checkpoint

Design §2.2 D6: an M5b row "must be added at freeze time per the R0 acceptance
pattern"; §8.3 defines its content. `GwzM5-8ChangeBudget.md` contains no M5b
mention (grep-verified, this date). The checkpoint routes the row to "the M5b
settle" without noting that D6's "at freeze time" has already passed, and the
1,200→1,450 raise is currently recorded nowhere in the budget ledger itself.
**Remediation**: add the M5b row (production 0/0 hard; test/fixture ≤1,450 for
this package with the raise provenance and the 1,424 actual; ≤10 files; wire and
protocol deltas none) at or before settled acceptance, so the ledger — the budget
authority — carries the ceilings the package was judged against.

### P3-3 — T-4's checker-lane CI scan neither landed nor named as owed

Frozen §6 T-4 (resolved variant): "the structural scan executes in CI on every
push **via the checker lane**". `scripts/checks/` contains no such scan (listing
verified); the delivered unit test covers the property and runs in CI via the
test suite, but the checkpoint's acceptance-ritual list (T-5, §5.4/Q7 sentence,
W1, ChangeBudget, re-cut) does not name the checker-lane scan as an owed settle
item, leaving the frozen sentence undischarged and untracked. **Remediation**:
either land the checker-lane scan by settled acceptance or record, in the
checkpoint at settle, the explicit substitution decision that the in-suite scan
(`no_ff_wire.rs:180`) is T-4's permanent CI mechanism.

## 9. Verdict

**GO.** 0 P0, 0 P1, 1 P2, 3 P3. The package's central claim (zero production
lines) is mechanically proven; all 21 delivered suites map to design-named §6/§7
obligations with no extra scope; the routing claims for T-5, T-4-CI, W1, the
ledger sentence, the ChangeBudget row, and the clean-tree re-cut check out against
the design's own §6/§8.1 text (with the D6 "at freeze time" tension recorded as
P3-2); T-6 is byte-identical and green; the stopped item's mode-blind
unreachability claim was verified both statically and by an independent
service-seam probe in both modes; the ceiling raise record is numerically exact
and its harness justification conservative; and every gate — build, fmt, clippy,
the four suites, store, P-V0 corpora, the full lib suite (1,391/0), and the
boundary checker with its same-commit pin — is green on a pristine extraction of
`3e60529`. The P2 and P3 items are remediations for (or explicit records at) M5b
settled acceptance; none blocks acceptance of the parked package.
