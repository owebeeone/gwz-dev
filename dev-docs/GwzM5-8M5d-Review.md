# M5d — review and sizing (DR-1 ship 2)

**Status:** REVIEW 2026-09-03, implementation lane (Fable). Answers the
operator's ask "M5c is superseded by M5d — review it, let me know how much
work that is." Design only; nothing built, nothing tagged. Reviewed object:
`GwzM5-8M5d-Charter.md` (operator-authored, untracked, controlling).
Evidence: line census of `gwz-core/src/workspace_ops/merge/` at `57502e4`
plus a read-only entanglement map (bucket classification, v1 imports from
v0 files, gates and pins, test corpus classes, docs and drivers).

## 1. Headline

About twice ship (1) in builder packages (9 against 5), with two dual
reviews instead of none, and one close landing that removes ~16,600 lines
and moves every pinned count in the tree. M5d writes less new code than
M5c would have and deletes more; the risk moves from runtime (a kept v0
lifecycle) to pins and paperwork (five aggregate counts, four digest
pins, one derived checker, three amendments).

| | Production (gwz-core) | Tests and fixtures | Docs, pins, gates |
|---|---|---|---|
| Delete | ~7,800 (4,484 whole files + ~3,300 halves of mixed files) | ~8,800 (class (i), 29 files) | 7 raw-writer rows; 3 of 7 compatibility-registry sections; 9 of 11 v0 retained-reader cases rewritten as refuse pins |
| Re-point / re-type | ~600 (relocate `mod v1_rollback` ×4, preserve v1 fns, `write_atomic_verified`; version-agnostic `gwz stage`, `frozen_manifest`, start fork) | ~9,325 re-run under v1 (class (ii), 36 files; expect ~2,500 lines touched, shared g23 harness re-typed) | 19 `merge_docs_manifest.json` rows + 1 global forbid; ~30 prose lines in 5 docs; 5 aggregate pins; 4 digest pins; A1 record, creation matrix, checkpoint |
| Write new | ~1,000 (parity ~500, raw create ~150, envelope-only classifier ~120, cfg(test) floor seam ~30, checker replacement ~80, misc) | ~1,500 (parity ~600, raw create ~200, §2 refuse ~300, archive/refuse fixture conversions ~400) | 3 amendments: I2 contract §4–§5; CapabilityFree §3 raw-writer set; F-3 replacement invariant (J-1 ruling) |

Kept on purpose: ~20,450 production LOC (shared vocabulary, v1 model,
record_wire, status/acceptance/plan/integration, ~1,225 archive-v0
reader, ~114 open-envelope detection). Kept tests: ~5,500 v1 suites
outside `v1_lifecycle/` plus the 23,515 inside it.

## 2. What the charter gets right (no change asked)

- One implementation, floor `V1`, v0 engine out of the binary (§1). The
  entanglement map confirms the engine is separable: 21 whole files and
  bounded halves of 24 mixed files, with the split lines identifiable
  (`mod v1_rollback` regions, `_v1` / `_view` symbol families).
- Open v0 is not a merge (§2). Today every gate over-decodes: the
  open-merge gate, the mutation guard, `gwz stage`, id-less `--status`,
  `--gc` and dispatch all call `FileMergeStore::discover_open`, which runs
  the full v0 body decoder to obtain a `merge_id`. The header reader and
  header classifier in `record_wire/header.rs` already suffice; the
  envelope-only helper is ~60 lines plus six call sites (the start gate
  at `merge/start.rs:74-80` is the sixth, not in the charter's list).
- Best effort, say so, do not over-build (§3). The v1 create door
  (`checked_artifact/entry.rs:541 create_merge_store_record`, called only
  from `v1_lifecycle/store/rewrite.rs:63`) is the one forward-path checked
  door. The crash-recovery decision is taken in `v1_lifecycle/start.rs`
  before any lease and is already passed into `service::run`, so the raw
  arm is a mode on that door, not a new coordinator.
- Surfaces that stay (§4) are the real work and are correctly named.
  Today a completed v1 start returns `archived_status_response` with empty
  `repos` and default counts, and v1 emits one `operation_state_changed`
  plus the diagnostic; `handle_stage.rs:34,137,270,342` and
  `merge/root.rs:31` are typed on the v0 record.
- One reviewed change for the close (§7), dual on it and on the contract
  amendment.

## 3. Flags (decisions or charter corrections)

Ordered by consequence.

**F1 — §9 step (1) cannot land the refusal before the floor.** Ordinary,
`--ff-only` and `-m` starts still write v0 (`version.rs:39
ACTIVE_WRITER_FLOOR = V0`; `merge/start.rs:93-122`). A landed §2 refusal
would break `--continue` / `--abort` on every ordinary conflicted merge on
`main` until step (4). What can land early: the contract amendment and
the envelope-only classifier (behaviour-preserving today). The refusal
arm itself lands in the close. One-line charter fix.

**F2 — §8 "no production v0 merge type remains" conflicts with §2's
archive projection.** Projecting `done/` v0 needs the v0 archive decoders
(`record_wire/archive/{v0,v0_evidence,v0_audit}.rs` + `decode_v0` +
`from_v0`, ~1,225 LOC) and the `MergeOperationRecordV0` serde struct
(~30 lines). Two options:
(a) keep them archive-decoder-private (rename the module, narrow
visibility, amend §8 to "no *open* v0 merge type"); cost is holding, not
building; the compatibility battery shrinks to its 10 archive shapes.
(b) take the one-line amendment "archived v0 unread too": deletes a
further ~1,225 production + ~1,200 test LOC and the whole compatibility
battery, but pre-0.14 history disappears from `--status <id>` and gc
listing, and gc must still delete `done/` v0 files by envelope or they
become immortal residue.
Recommendation: (a), the charter's own default.

**F3 — raw create grows a set that may only shrink.** The capability-free
raw-writer inventory (`check_checked_artifact_boundaries.py:544-573`) is
key-set pinned and its amendment says the sets only shrink. Deleting the
engine removes seven v0 rows; the relocated raw primitive called from
`entry.rs` adds one forward-path row. Needs a `GwzM5-8R2E-CapabilityFreeAmendment.md`
§3 revision, folded into the S1 dual. Operator OK needed since the
amendment's own words forbid growth.

**F4 — the F-3 derived seam fails closed on success.** The checker derives
`V0_PERSISTENCE_SEAM_FLOOR` from `merge/mod.rs`'s `use store::{…}`
re-exports and errors if that shape is restructured away
(`check_checked_artifact_boundaries.py:361-387`); its comment requires the
R4b-G lane owner's ruling to remove or weaken it (judgment call J-1).
Proposed replacement property, stated in S1 for the ruling:
`v1_lifecycle/` names the neutral raw primitive's module nowhere; only
`checked_artifact/entry.rs` calls it. Same masked-identifier scan, new
floor set.

**F5 — ship (1) is not live on the ordinary path today.** The v0 start
emits no crash-recovery warning and never reads `filesystem_strict`
(`merge/start.rs:113` is the only read, and it feeds `v1.start`). An
ordinary `--filesystem-strict` start on an odd volume runs without
refusing. `--no-ff` on a handle-less volume warns and then refuses at the
v1 create door (ship (1) §4.1 limit). Not in any release (no 0.14.0
before this close), and the floor raise fixes it for free. If the
operator wants strict honoured on ordinary starts before the close, it is
~30 lines (hoist the decision into the shared prologue); otherwise leave
it to S7.

**F6 — the charter's §5 misses the largest test class.** It names engine
suites (delete) and shipped-default suites (flip), but ~9,325 LOC in 36
files assert ordinary-path *product* behaviour (`g23/{gc,preserve,
root_abort,continue_merge,start,abort_recovery,root_start,finalization,
root_recovery,drift,fixtures}.rs`, `status/tests/*`, `plan/tests.rs`,
`participant_semantics/status_tests/*`, `model/tests.rs`, `tests/protocol.rs`
in gwz-core's integration tests). Deleting them loses product coverage;
they must be re-pointed to v1. No writer-floor test seam exists
(`with_writer_floor` is not in the tree), so a ~30-line cfg(test)
override on `select_record_version` is the enabler: re-point under
floor `V1` before the flip, each group landing green on `main`, which
also surfaces the §4 parity gaps early instead of at the close.

**F7 — `model/v0.rs` is the shared vocabulary, not the v0 record.**
`MergeBaseline`, `MergeParticipantRecord`, `PreservationEvidence`,
`PublicationProgress`, `OperationDrift`, `PendingMergeAction`,
`ConflictFileEvidence`, `MergeRecordError` live there and are embedded by
`MergeOperationRecordV1` (`model/v1/record.rs:6-9`) and imported by
`v1_lifecycle/authority.rs:20`. The plan renames it (`model/common.rs`)
and keeps ~240 of 268 lines. Plan fact; no decision.

**F8 — three v1 observation families live inside v0 files.** `mod
v1_rollback` occupies `abort/evidence.rs:24-522`,
`abort/participants.rs:79-341`, `abort/preflight.rs:26-272`,
`root/abort.rs:93-483` (~1,400 lines), and the `v1_*` preservation API is
spread across `preserve/{plan,artifacts,checked_bundle}.rs`. File-level
deletion would take v1's reverse and preservation observation surface
with it; the close relocates them into neutral modules (~250 lines of
moves). Three of those files carry protected source digests.

**F9 — the raw-create proof needs Linux.** The leaf handle probe cannot
be made to fail on macOS in tests. Proof is the R0-L workflow's real
overlay mount (already present) plus a `--no-ff` start row there, or a
cfg(test) seam on the leaf probe. Cost is in the table.

## 4. Steps implied (builder packages, aspirational < 500 LOC each except the close)

| Step | Content | Size | Review |
|---|---|---|---|
| S1 | I2 contract §4–§5 amendment; CapabilityFree §3 revision (7 rows out, 1 in); F-3 replacement invariant text for the J-1 ruling | ~150 doc lines | dual (Consistency + Safety) |
| S2 | Envelope-only open-record classifier (header read, no body decode) at the six sites, behaviour-preserving; cfg(test) writer-floor seam | ~150 prod + ~150 tests | by reading |
| S3 | Parity A: v1 events per participant, per durable transition, per artifact on start, continue, abort; verified on `--no-ff` and under the seam | ~250 prod + ~300 tests | by reading |
| S4 | Parity B: completed responses from the archived v1 record (repo rows, counts, publication step, preservation, start decorations); `gwz stage` on the v1 body; dry-run pin | ~300 prod + ~300 tests | by reading |
| S5 | Raw create on the v1 door for the Unsupported / handle-fail arm; relocate `write_atomic_verified`; one diagnostic; R0-L overlay proof row | ~150 prod + ~200 tests + workflow row | by reading |
| S6a–c | Re-point class (ii) suites under floor `V1` (three landings: g23 harness + start/continue/finalization; gc/preserve/root_*; merge-internal status/plan/model/participant_semantics + gwz-cli/gwz-py pins) | ~9,325 re-run, ~2,500 touched | by reading + green trains |
| S7 | The close, one reviewed change on a branch built in ~4 commits: floor `V1`; §2 refusal arm; delete engine (A + halves + class (i)); rename `model/v0.rs`; relocate v1 families; checker, digests, aggregate pins, compatibility registry, retained-reader cases, docs manifest, prose; A1 record and creation-matrix annotations; release-notes draft | −16,600 / +600 | dual + every gate re-pinned once |

Order: S1 → S2 → (S3 ∥ S4 ∥ S5) → S6a–c → S7. S3–S5 are verifiable on
`--no-ff` today. S7 does not land before S1–S6. Matches charter §9 except
that (1)'s refusal moves into (4) (F1).

Ratio to ship (1): W1–W5 were 5 packages, no duals, landed in one lane
day. M5d is 9 packages, 2 duals, and S7 alone is an order of magnitude
larger than any ship (1) step; the wall-clock lives in S6 and S7's
per-commit-green trains and the two full re-pins.

## 5. Pinned counts and files that move at the close (for the landing checklist)

- `run_r4bg_aggregate_gates.py`: lib remainder 1,125 / 1,126; g23 135;
  record_wire 75; compatibility "7 migration rules, 7 runtime bindings,
  10 archive shapes" → archive-only; checked_artifact 459 / 476 if
  `entry.rs` gains tests; docs gate "12 sources, 155 assertions".
- `check_checked_artifact_boundaries.py`: `PROTECTED_COMPILER_ROOT_DIGESTS`
  (`merge/mod.rs`), `PROTECTED_SOURCE_DIGESTS` (`preserve/{artifacts,
  checked_bundle,plan}.rs`), `PROTECTED_SOURCE_TREE_DIGESTS`
  (`v1_lifecycle/mod.rs`, `authority/observe.rs`), raw-writer key-set SHA,
  F-3 floor (F4).
- `dev-docs/GwzM5-8I2CompatibilityPredicates.json`: `migration_whitelist`,
  `fixture_corpus`, `valid_unlisted_corpus`, `rejection_reasons`,
  `migration_policy`, `descriptor_schema` go; `archive_corpus` (10) stays
  under F2(a). `check_m4_scenario_map.py` "22 registry rows" moves.
- `scripts/retained_readers/manifest.json`: new generation entry; 9 open-v0
  cases become §2 refuse pins; 2 archive cases stay.
- Drivers: `gwz-cli/src/tests/g02.rs:292` (V0 pin), `m2c.rs:58-59`
  (literal v0 record); `gwz-py/src/tests/test_merge_cli_cross_driver.py:144-163`
  and `test_native_merge_operations.py:143-144` (literal open v0 → §2 pin).
  No protocol change: `source_version` keeps its V0 variant for archived
  projection.
- Docs: `gwz-cli/docs/commands/merge.md:378-386`, `MachineOutput.md:243-248,335-338`,
  `gwz-core/docs/OperationModel.md:207-217`, `MergeRecovery.md:315-317`;
  manifest rows incl. `merge_command_no_ff_writes_v1_while_ordinary_stays_v0`.

## 6. Not done here

No build. No commit of this review or of the charter (operator's file).
No tag. No M6. Reverse-path raw is not a step (charter §3).
