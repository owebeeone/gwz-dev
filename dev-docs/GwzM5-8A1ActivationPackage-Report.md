# GwzM5-8 A1 Activation Package — Builder's Report (filed verbatim)

Filed 2026-08-24 by the implementation lane. Builder: the activation
package agent (Opus), completed 2026-08-24. Package state at filing:
IN-TREE, UNCOMMITTED, over the accepted tuple gwz-core `26f48f5` /
gwz-cli `3cca145` / gwz-py `929efb0` / taut `f008419` (88 / 3 / 2
dirty files respectively — counts verified against `git status` at
filing). The build spec was the Safety axis's round-1 §2 enumeration
(`GwzM5-8A1Activation-ReviewSafety.md`). Round 2 of the A1 activation
review verifies this package in-tree. The report below is the
builder's final message, verbatim.

---

The A1 activation package is built and gated. Report follows.

## Verdict

The package lands **§2.1 (all six compile gates), §2.2 R1/R2/R3 and R4's mechanism, §2.3 (both CLI surfaces), the §2.4 production callers, and conditions 1–4**. `--no-ff` works end-to-end in production: the writer floor writes a v1 record and the merge publishes a real two-parent integration commit.

**One enumerated item lands partially and is the round-2 decision:** R4's floor is engaged for `--no-ff` but not for ordinary/custom starts (§2 below).

---

## 1. Package map vs the §2 enumeration

### §2.1 — compile gates (all six FELL)

| # | Coordinate now | Disposition |
|---|---|---|
| G1 | `merge/mod.rs:23` `mod v1_lifecycle;` | **FELL.** `cfg(test)` + the `:25-26` sentinel const removed; `v1_lifecycle/mod.rs`'s blanket `#![allow(dead_code)]` **expired** and was replaced by ~20 item-level allowances, each carrying a reason |
| G2 | `model/mod.rs:7` `pub(crate) mod v1;` | **FELL** |
| G3 | `acceptance/mod.rs:2-3` `mod v1; mod v1_candidate;` | **FELL** (incl. the file's cfg re-export block) |
| G4 | `record_wire/mod.rs:13-14` `mod open_v0; mod unknown_fields;` | **FELL** |
| G5 | `store/mod.rs:14` `mod atomic_upgrade;` (+ re-export `:24`) | **FELL** |
| G6 | `merge/mod.rs:51` | **REPLACED, not un-gated.** `adapt_open_v0_for_r3_tests`→`adapt_open_v0`, `decode_v0/v1_for_r3_tests`→`decode_production_v0/v1` (+ new `decode_production` registry entry), `upgrade_open_v0_for_r3_tests`→`upgrade_open_v0`, `decode_archived_for_r3_tests` folded into `decode_archived`. Fault injectors and `archived_fixture_for_test` **kept their cfg** |

The gate was **wider than the six coordinates**: 97 further `#[cfg(test)]` markers across the v0 tree gated v1-facing helpers (`abort::observe_v1_*`, `preserve::v1_*`, `root::*_v1_*`, `marker::*_v1_*`, `acceptance::finalization_next_action_for_*`, `MergeStatusRecordView::from_v1`, `to_v1_response`), plus 28 `#[cfg_attr(test, derive(Serialize))]` on the v1 model. All fell. **Measured: the compile-gate fall alone is behaviour-neutral — 1572/0 at that milestone, zero test delta.**

### §2.2 — runtime gates (four changed; the coupled pair moved together)

| # | Coordinate | Disposition |
|---|---|---|
| **R1** | `validate.rs:12` | **FELL.** The typed refusal is gone; NoFf routes to the v1 lifecycle |
| **R2** | `runtime/dispatch.rs:256` | **FELL IN THE SAME EDIT.** The `mode != Some(NoFf)` exclusion is gone; every start validates its custom message before creation. Both sites carry cross-referencing comments naming the coupling |
| **R3** | `store/mod.rs:333` + `record_wire/header.rs:30` | **CHANGED.** The hard-coded v0 pair test is replaced by `classify_merge_record_header`. `InstalledMergeRecordVersions::PRODUCTION_R3{v0,!v1}` → `PRODUCTION{v0,v1}` (**T-2 inverted**); `V0_ONLY` retained as the v0 store's own set. New `decode_production` → `DecodedRecord::{V0,V1}`; v2–v4 keep the frozen typed projection |
| **R4** | `start/record.rs:55` | **CHANGED — see the partial-engagement note below** |

### §2.4 — production callers that landed

- `runtime/dispatch.rs` gained an **envelope router** (`V1Router`, `AuthorityV1Router`/`AbsentV1Router`). `handle_merge`/`handle_merge_with_events` bounds narrowed `GitBackend`→`MergeAuthorityBackend` (sealed to `Git2Backend`; every existing caller already passed it). The cfg(test) DI seam keeps the wider bound and fails closed.
- `store/mod.rs:206` `classify_open_record` — header-only envelope probe; v1 records route to the v1 lifecycle before any v0 handler is reached.
- **New: `v1_lifecycle/store/rewrite.rs:35 create_open`** — the v1 record-creation owner. It did not exist at the tuple; `commit` requires a lease-covered base digest, so nothing could bring a v1 record into being.
- **New: `v1_lifecycle/start.rs:66 handle_start_durable_v1`** + `handle_v1_command` — `service.rs:69 run` gained its merge-visible production entry, exactly as §2.4 anticipated.
- `dispatch.rs:394 adapt_before_mutating` — the adaptation preflight (conditions 1 and 2 below).
- Read-only status/GC/archive gained the v1 projection: `decode_archived` now installs v1 (T-2).

**Evidence the routing is real:** dead-code warnings in the v1 tree fell **921 → 63** when production dispatch landed.

### R4 — the one partial item

`select_record_version(requested) = max(ACTIVE_WRITER_FLOOR, semantic)` is production (`model/version.rs`), wired at the creation site, with v2–v4 rejecting **before** record creation. `RequestedSemantics::NoFf → V1`, so `--no-ff` writes v1 today.

`ACTIVE_WRITER_FLOOR` is **`V0`, not A1's `V1`** (`version.rs:39`, with a doc comment stating this). I engaged `V1` and measured it: **every ordinary start broke** — the v0 store refuses the v1 envelope, and there is no production v1 owner for the ordinary start (root participants, dry-run prediction, drift/conflict response surfaces, the v0 event stream). That owner is a milestone, not this package. Raising the constant is a one-line change once it lands; nothing else in the selection moves. A test pins that `RecordVersion::V1.max(...)` is `V1` for every installed semantic, so the `max` is proven, not assumed.

---

## 2. Must-not-flip verification

| # | Item | Verified | Evidence |
|---|---|---|---|
| 1 | `recover_or_create` uncalled in production | **UNTOUCHED** | Diff touches exactly one file under `checked_artifact/` (`coordinator/identity.rs`, the `CheckedOwnerRecordVersion::V1` match arm G1 forced). `catalog.rs:10-16` R2-E allowance intact verbatim |
| 2 | D2 foreign-filter | **UNTOUCHED** | No diff in `git/gitbackend/recovery_support.rs`, `merge_recovery.rs`, `git/tests/g12.rs` |
| 3 | v2/v3/v4 semantics | **UNTOUCHED + newly enforced** | `RecordVersion` still closed `{V0,V1}`; zero v2+ body types/discriminants; `select_record_version` rejects A2/A3/A4 before creation |
| 4 | T-6 / v0 forged-action resume | **HELD** | `v0_resume_rejects_forged_two_parent_action_over_fast_forwardable_pair`, `v0_resume_rejects_forged_no_ff_mode_row` — **2/2 green** |
| 5 | Legacy call graphs + durability residue | **UNTOUCHED** | `durable_fs.rs` no diff; no-op Windows `sync_dir` intact; pid+sequence temp naming intact (`store/mod.rs:439`). One `fs::read` added (the envelope probe) — same unbounded class as the existing v0 record read, rides P3-3 unchanged |
| 6 | `observation.rs:216` / `preservation_root.rs:23` | **UNTOUCHED** | Zero diff both files; lines verified in place |
| 7 | R2-F/R5 release gates | **UNMOVED** | No release-workflow, manifest, or retained-reader-generation change |
| 8 | `protocol/generated.rs` | **UNTOUCHED** | Zero diff — no new wire |
| 9 | R2-E families / census 165 | **UNTOUCHED** | `EXPECTED_KEY_COUNT: usize = 165` intact |

Also verified: the call-graph gate's F-3 property (**`v1_lifecycle/` names no v0 persistence item**) still holds at 0 hits — the new v1 start owner creates through the checked v1 store.

---

## 3. Condition dispositions

**[P1-1] — MET.** The preflight's non-`Upgraded` arms (`ValidUnlisted` **and every typed refusal**) leave the v0 lifecycle in command. New test `post_activation_resume_completes_the_refused_v0_crash_prefixes` drives F-MARKER and F-LOCK through post-activation dispatch, asserts the row reached the preflight (`version==V0`, `adaptation==MayAdapt`) so the refusal arm really is traversed, and requires resume to `Completed`. `characterization_publication_prefix_v0` (all four prefixes) also stays green.

**[P2-1] — MET via option (i).** `AdaptationPrecheck::{MayAdapt,Skip}` on the envelope probe; only `Finalizing` + normal-mode rows enter the preflight. `the_adaptation_precheck_admits_only_finalizing_normal_mode_v0_rows` walks six non-Finalizing states plus FfOnly/NoFf and requires `Skip`. B-NOT-STARTED and B-PREPARING-EMPTY cannot reach `validate_v0_structure` through the new path. *The two archive shapes ride as named residual, per the finding.*

**[P2-2] — MET.** `checked-artifact-boundary.yml` gains `check_m4_scenario_map.py` + `run_r4bg_aggregate_gates.py privacy call-graph` (driven through the runner so expected counts are pinned, not just exit codes). L2-05 **not wired**, with the reason recorded in a workflow comment: the manifest asserts against `gwz-cli/docs/*` resolved from the workspace root, and the job checks out gwz-core alone.

**[P3-1] — MET.** T-1 inverted (both the gate and M5b's designed marker, renamed `custom_messages_and_no_ff_both_validate_after_activation`). T-2 inverted at two sites (header dispatch + archive decoder). **T-3 re-pinned** 16→19 files, each new entry annotated, and the expired "the v1 writer is cfg(test)" rationale replaced. **T-4 re-verified unchanged** — `ForceMergeCommit` construction sites are still v1_lifecycle-only. T-6 untouched.

---

## 4. Files + LOC

| Repo | Files | Added |
|---|---|---|
| gwz-core | 85 modified + 3 new = **88** | +1109 tracked, +743 new-file = **1852** / −609 |
| gwz-cli | 3 | +14 / −3 |
| gwz-py | 2 | +16 / −3 |

New: `merge/model/version.rs` (195), `merge/v1_lifecycle/start.rs` (245), `tests/g23/a1_activation.rs` (303). gwz-core split: production src 68 files, test tree 15, workflow 1.

---

## 5. Gate tails (verbatim)

```
cargo check --all-targets            Finished `dev` profile ... in 10.99s     [0 warnings, 0 errors]
cargo fmt --all --check              clean
clippy -D warnings (from clean -p)   Finished `dev` profile ... in 12.75s
gwz-core lib (disjoint partition, final tree):
  v1_lifecycle−root_fault_matrix   test result: ok. 255 passed; 0 failed
  root_fault_matrix                test result: ok.   1 passed; 0 failed
  checked_artifact::               test result: ok. 400 passed; 0 failed
  remainder                        test result: ok. 926 passed; 0 failed; 1 ignored
  = 1582 passed / 0 failed / 1 ignored = 1583 census (baseline 1573, delta +10)
gwz-cli    80+26+25+4+2+2 = 139 passed; 0 failed
gwz-py     330 passed in 31.36s
L2-04      Ran 86 tests ... OK  |  {"status":"valid","tuple_count":24}  |  {"status":"manifest-ready","tuple_count":24}
check_m4_scenario_map.py           M4 scenario map: ok (39 scenario rows, 41 named tests, 13 registry rows all claimed)
check_merge_compatibility_...py    validated 7 migration rules and 7 runtime bindings
check_merge_docs.py                merge document consistency: ok (11 sources, 147 assertions)
R4b-G batteries: unknown-field ok · privacy ok (108s) · compatibility ok (after the docs-tripwire inversion)
                 fault / byte-equivalence / call-graph — count-pin and probe-harness only, below
```

---

## 6. Pin report (attributed — all lane-owner's at landing; I did not apply them)

**Boundary checker — 5 digests moved** (`checked-artifact boundary: failed`, verbatim above):

```
PROTECTED_COMPILER_ROOT_DIGESTS
  src/workspace_ops/merge/mod.rs                        56c4035320df9de290431b15309770a4904ad430da4df13b8cb85d47f5a806e5  (was d7b2662b…)
PROTECTED_SOURCE_DIGESTS
  workspace_ops/merge/preserve/artifacts.rs             a4e9f7055c3d8aeacd494c08629223dbb15fea0a2e271303b0c27f11833be696  (was e9a30d32…)
  workspace_ops/merge/preserve/plan.rs                  3730179e156151c4a853752ec769712d3ae81bd21e7729b892ab4cb14474ff89  (was 880d4905…)
PROTECTED_SOURCE_TREE_DIGESTS
  workspace_ops/merge/v1_lifecycle/authority/observe.rs d16fa8bf67f8656c56b3c51d6625712efcc970dfd51afefa77557df5b3fcae38  (was f0f09701…)
  workspace_ops/merge/v1_lifecycle/mod.rs               6c687c48854975e3d81ee79ac9c78a188f65a394450e8886f2bcf51c4e497a22  (was 0719565e…)
```
Attribution: G6's re-export replacement (`merge/mod.rs`); G1's cfg removal in the preserve v1 seams; G1 + the new `v1_lifecycle/start.rs` (tree manifests).

**R4b-G driver count markers — 3 moved** (all partitions green; only the expected strings are stale):

| Battery | Command | Expected | Actual |
|---|---|---|---|
| fault | `v1_lifecycle:: --skip root_fault_matrix` | `254 passed` | **255** (+1: the v1 start owner's inline test) |
| fault | `-- --skip checked_artifact:: --skip v1_lifecycle::` | `917 passed` | **926** (+9: 5 activation + 4 writer-floor) |
| byte-equivalence | `workspace_ops::tests::g23::` | `114 passed` | **119** (+5: the activation suite) |

Unchanged: `1 passed`, `400 passed`, `75 passed`.

**Probe-harness consequence of G1 — 11 of 69 boundary-checker tests fail.** `test_check_checked_artifact_boundaries.py:1141` injects an unused `probe_v0_persistence` into `v1_lifecycle/service.rs` and requires the compile to *succeed* so the checker can reject it. That relied on the blanket `#![allow(dead_code)]` the review told me to expire; the probe build now fails `-D dead-code`. This is harness mechanics, not the guarded property — the F-3 v1→v0 persistence scan itself still reports **0 hits**. Minimal fix (boundary lane's file): emit `#[allow(dead_code)]` with the injected probe text. I did not apply it — the checker's own doctrine forbids altering it without the R4b-G lane owner's ruling.

**One docs tripwire I did invert**, because it is part of §2.3's surface: `merge_docs_manifest.json`'s `forbidden` assertion `generated_cli_no_ff_must_remain_hidden` became the required `generated_cli_no_ff`. `gwz-cli/docs/CLI.md` was regenerated (`scripts/generate_cli_reference.py --write`) — the unhide made it stale and `g00::cli_reference_doc_matches_generated_clap_help` caught it.

---

## 7. Named residuals for round 2

1. **R4's ordinary-start row** — the package's one incomplete enumerated item, with the blocking owner named and measured.
2. **One behaviour pin moved, deliberately:** `g23::finalization::resumed_finalization_persists_each_phase_before_a_nested_mutation_fault`. Its final window left a `CommittingEvidence` row — a whitelisted shape — so the preflight now migrates it and the v1 lifecycle completes it; the v0 fault injector no longer applies. I rewrote that window to assert the migration (`source_version == V1`) so a whitelist-eligibility regression still trips it, with the reasoning in-comment. **Consequence worth the review's attention:** activating migration reduces v0-fault-injector coverage on the authority path for the seven whitelisted shapes.
3. **Positive evidence found:** `characterization_publication_prefix_v0`'s Boundary and Staging prefixes *are* whitelisted, so they now migrate and continue on v1 — and the test stays green unchanged. That is executed evidence that v1 continuation retains the v0-equivalent next action (contract §4).
4. `v1_lifecycle::archive::gc_archived` and `gc.rs`'s archived-cleanup family have **no production caller**: v1 GC keeps its typed open-record refusal and archived GC goes through the version-agnostic `decode_archived`. Marked with a reasoned allowance naming the missing route.

Insurance: nine milestone snapshots in `scratchpad/insurance-a1/` (`M-A` … `M-I-delivery`), each with per-repo patches, the three new files, and the gate outputs. No git operations were performed.
