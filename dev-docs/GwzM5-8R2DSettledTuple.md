# R2-D settled tuple — the evidence document for the 5.2 dual

Date: 2026-08-23
Step: R2-D **Phase 5 Step 5.1** — `GwzM5-8R2D-Plan.md` §4 Step 5.1.
Object: gwz-core, base `514f8e6` (Phase 4 SETTLED) **plus this step's delta**
— **LANDED as the settled tuple tree `d45458d`** ("Land R2-D Step 5.1: the
settled tuple's gate train, evidence, and last test debt", 2026-08-23). The
5.2 settled dual reviews `d45458d`; every gate tail below was produced on
content byte-identical to it, and the boundary checker is `ok` at the landed
tree with zero pin moves.
(three files; §7).

This is the one document the Phase 5.2 settled dual reads first. Everything in it
was executed on this host, at this tree, for this step — no gate tail is copied
forward from an earlier step's report. Where a claim is a *citation* rather than
an execution it says so.

**Re-run discipline.** The train was run once, then §2.1's and §2.3's repairs
landed, then `cargo fmt`, the focused suite and the full lib suite were **re-run
on the final tree** and the tails below are the second run's. Clippy-from-clean
and the boundary-checker unit suite likewise post-date their repairs. The
integration binaries, the protocol-currentness tests and the Python gates are
unaffected by a Rust doc comment and a Python assertion, and their tails are the
single run.

---

## 1. Verdict of the gate train

**Every gate is GREEN**, after one repair this step's train discovered (§2.3).

| # | Gate | Result |
| --- | --- | --- |
| 1 | `cargo fmt --all -- --check` | clean |
| 2 | Focused `cargo test --lib checked_artifact::` | **400 / 0** |
| 3 | Full `cargo test --lib` | **1569 / 0**, 1 ignored |
| 4 | Integration binaries ×4 | **50 / 0** total |
| 5 | Protocol regeneration / currentness | **3 / 0** by name |
| 6 | Boundary checker | `ok (15 visible entries, 5 classified modules)` |
| 7 | Boundary-checker unit suite (65) | **65 / 0** *(after §2.3's repair; 64/1 before)* |
| 8 | Release-boundary suite (6) | **6 / 0** |
| 9 | Merge-doc gate (147 assertions) + its suite (3) | **ok / 3 / 0** |
| 10 | Merge-compatibility gate + its suite (14) | **ok / 14 / 0** |
| 11 | L2-04 retained-reader harness (86 + validate + gate-ready) | **86 / 0**, 24 tuples |
| 12 | Clippy, all-targets/all-features, from `cargo clean -p` | **zero diagnostics** |

Verbatim tails are in §8.

---

## 2. What the train found

Step 5.1 exists to run the whole train once, together, on the settled tree. It
earned that in three places.

### 2.1 A stale in-tree audit note (fixed)

`coordinator/execution.rs` told an auditor that `mod bootstrap`, `mod capability`,
`mod namespace` and `mod coordinator` "each carry a blanket
`#[allow(dead_code)]` at `checked_artifact/mod.rs`", and concluded that the
allows inside the coordinator subtree are therefore **inert**. Step 4.3's own
narrowing (settle item 7) falsified that: it deleted the `coordinator` blanket
and moved the cover inward onto `mod identity`. So the allow on `mod execution`
is now **live and load-bearing**, and the note was inviting a future reader to
delete it as decoration.

Corrected in place, and the correction is measured rather than asserted:
removing that allow produces **29** dead-code warnings.

### 2.2 The `runtime.*` per-key evidence gap (reported, not papered)

See §4.6. It is the one place where the per-key evidence Step 5.1 owes cannot be
produced, and the tuple says so plainly.

### 2.3 A checker companion that drifted at the 4.2 landing (fixed)

`test_windows_exact_handle_publication_is_source_protected` failed:

```
AssertionError: 'protected source allowlist changed' not found in
'checked-artifact boundary: failed
- protected source tree changed: checked_artifact/platform.rs
'
```

**Cause, and it is not this step's.** Step 4.2 split the Windows durability
anchor out into `platform/anchor.rs`; that landing converted
`checked_artifact/platform.rs` from a **flat** pin to a **tree** pin so the new
child could not sit outside any manifest (the Step-4.2 review's §9.3 pin-shape
opinion, adopted at the landing). The checker's message changed with the pin
shape; this companion assertion did not. It was therefore red at `514f8e6`, and
Step 5.1's train is the first thing to run this suite since the conversion —
which is precisely the failure mode a once-per-phase full train exists to catch.

**Repair:** the assertion now names the pin as well as the class
(`"protected source tree changed: checked_artifact/platform.rs"`). Naming the pin
is deliberate: the old generic prefix would have passed on *any*
protected-source finding, and that latitude is what let the drift hide.

**Attribution note for the dual:** this step's own working-tree delta touches
`bootstrap/managed/tests_writer_matrix.rs`, `coordinator/execution.rs` and this
test file only. It moves no pinned file, so the failure cannot be its
consequence; the checker itself is `ok` on the delta (§8).

---

## 3. Tree identity

| | |
| --- | --- |
| Base commit | `514f8e6` — "Land R2-D Step 4.3: coexistence decided, residuals dispositioned — Phase 4's step list closes (review GO)" |
| Working delta | 3 files, +60 / −7 (§7) |
| Boundary checker on the delta | `ok` — **no pin moves** |
| Census | 165 fault keys (`fault_v1.rs` rows counted: 165; `EXPECTED_KEY_COUNT = 165`) |

---

## 4. Per-key executed fault evidence (L1-16 / L2-14 form)

Every row below was verified **by name** against the tree — the key's variant
grepped for its `fault_v1::hit(...)` production site, and the matrix constant
read to confirm the key literally appears in the list the runner consumes.
Nothing here is transcribed from the §3.5 map.

Path prefix for all sites: `src/checked_artifact/`.

**Both-variants convention.** Each family's matrix runner is invoked from a pair
of `#[test]`s ending `_on_a_workspace_target` / `_on_a_git_directory_target`,
except `catalog_bootstrap`, which expresses the pair as two separately named
tests over the same 25-key list.

### 4.1 `admission` — 19 / 19 executed

Sites: `capability/pre_catalog/provider/admission_mutation.rs` (all 19).
`admission/driver.rs` holds zero — it decides, it never mutates.
Matrix: `admission/tests_fault_matrix.rs`, `ADMISSION_MATRIX: [Fault; 19]`,
consumed by `run_interruption_matrix`; `reconcile_executed_keys` proves the list
equals the family. Repeated-boundary subset: 3 keys.
Tests: `admission_interruption_restart_convergence_matrix_on_a_{workspace,git_directory}_target`;
`repeated_same_boundary_crashes_keep_stable_slots_on_a_{workspace,git_directory}_target`.

| key | site | in matrix | repeated |
| --- | --- | --- | --- |
| occupancy_observe | `:51` | ✓ | |
| capacity_check | `:68` | ✓ | |
| preparing_scratch_create | `:354`→`:410` | ✓ | ✓ |
| preparing_scratch_write | `:355`→`:418` | ✓ | |
| preparing_scratch_flush | `:356`→`:431` | ✓ | |
| preparing_publish | `:385`→`:191` | ✓ | |
| preparing_reobserve | `:386`→`:199` | ✓ | |
| staging_create | `:212` | ✓ | |
| reservation_create | `:364`→`:410` | ✓ | ✓ |
| reservation_write | `:365`→`:418` | ✓ | |
| reservation_flush | `:366`→`:431` | ✓ | |
| staging_flush | `:268` | ✓ | ✓ |
| final_publish | `:296` | ✓ | |
| final_reobserve | `:307` | ✓ | |
| idle_scratch_create | `:359`→`:410` | ✓ | |
| idle_scratch_write | `:360`→`:418` | ✓ | |
| idle_scratch_flush | `:361`→`:431` | ✓ | |
| idle_publish | `:381`→`:191` | ✓ | |
| idle_reobserve | `:382`→`:199` | ✓ | |

### 4.2 `durable_leaf` — 11 / 11 executed

Sites: `capability/pre_catalog/provider/leaf_observation.rs` (all 11).
Matrix: `provider/tests_leaf_fault_matrix.rs`,
`DURABLE_LEAF_MATRIX: [(Fault, LeafArmV1); 14]` — 14 rows over 11 distinct keys,
because three boundaries are shared by the Exact and Absent arms of the two-sided
proof. `reconcile_executed_keys` dedups and proves family equality.
Tests: `durable_leaf_interruption_restart_convergence_matrix_on_a_{…}_target`;
`repeated_same_boundary_crashes_mutate_nothing_on_a_{…}_target` (2 keys).

| key | site | arm(s) | repeated |
| --- | --- | --- | --- |
| first_open | `:387` | Exact, Absent | |
| first_identity | `:389` | Exact | |
| first_content | `:495`→`:509` | Exact | |
| file_flush | `:174` | Exact | ✓ |
| namespace_barrier | `:352` | Exact, Absent | ✓ |
| parent_revalidate | `:362` | Exact, Absent | |
| name_revalidate | `:185` | Exact | |
| handle_revalidate | `:204` | Exact | |
| length_revalidate | `:220` | Exact | |
| content_revalidate | `:496`→`:509` | Exact | |
| missing_revalidate | `:274` | Absent | |

### 4.3 `namespace` — 11 / 11 executed

Sites: `capability/pre_catalog/provider/namespace_mutation.rs` (all 11).
`namespace/host.rs` holds zero — it validates capabilities and never mutates.
Matrix: `namespace/tests_fault_matrix.rs`, `NAMESPACE_MATRIX: [Fault; 11]`;
repeated subset 3.
Tests: `namespace_interruption_restart_convergence_matrix_on_a_{…}_target`;
`repeated_same_namespace_boundary_crashes_keep_stable_slots_on_a_{…}_target`.

| key | site | repeated |
| --- | --- | --- |
| parent_revalidate | `:226` | |
| source_retain | `:242` | ✓ |
| destination_reserve | `:82`→`:265` | |
| pre_publish_reobserve | `:83`→`:278` | |
| publish_no_replace | `:84`→`:290` | |
| published_reobserve | `:85`→`:302` | |
| parent_barrier | `:334` | ✓ |
| retirement_reserve | `:88`→`:265` | |
| pre_retire_reobserve | `:89`→`:278` | ✓ |
| retire_exact | `:90`→`:290` | |
| retired_reobserve | `:91`→`:302` | |

### 4.4 `record` — 13 / 13 executed

Sites split, and the split is the family's own shape: four bounded-parse stages
live with the R1 parse owner, nine with the binding owner.
Matrix: `provider/tests_authority_record_matrix.rs`, `RECORD_MATRIX: [Fault; 13]`,
partitioned into 9 repeatable + 4 unrepeatable, with
`the_repeatability_taxonomy_accounts_for_every_boundary` proving the partition.
Tests: `record_interruption_restart_convergence_matrix_on_a_{…}_target`;
`repeated_same_boundary_crashes_keep_stable_slots_on_a_{…}_target`.

| key | site | class |
| --- | --- | --- |
| bounded_read | `protocol/authority_record.rs:359` | repeatable |
| decode | `protocol/authority_record.rs:365` | repeatable |
| canonical_reencode | `protocol/authority_record.rs:377` | repeatable |
| binding_validate | `protocol/authority_record.rs:388` | repeatable |
| scratch_create | `authority_record_binding.rs:503` | repeatable |
| scratch_write | `authority_record_binding.rs:511` | repeatable |
| scratch_flush | `authority_record_binding.rs:524` | repeatable |
| active_publish | `authority_record_binding.rs:552` | unrepeatable |
| active_reobserve | `authority_record_binding.rs:569` | unrepeatable |
| retirement_reserve | `authority_record_binding.rs:623` | repeatable |
| retire_exact | `authority_record_binding.rs:636` | unrepeatable |
| retired_reobserve | `authority_record_binding.rs:654` | unrepeatable |
| terminal_relation_validate | `authority_record_binding.rs:456` | repeatable |

### 4.5 `managed_bootstrap` — 28 / 30 executed, across three matrices

All sites: `capability/pre_catalog/provider/managed_mutation.rs`. The
`namespace` owner holds none.

**Step 2.3 — `namespace/tests_managed_matrix.rs`, `MANAGED_MATRIX: [Fault; 8]`**

| key | site | class |
| --- | --- | --- |
| parent_revalidate | `:433` | repeated |
| staging_directory_publish | `:615` | single-crossing |
| final_directory_reopen | `:844` | repeated |
| final_directory_reobserve | `:669` | repeated |
| component_reobserve | `:635` | repeated |
| marker_retire | `:718` | single-crossing |
| marker_retired_reobserve | `:788` | single-crossing |
| final_identity_reobserve | `:794` | single-crossing |

**Step 3.1b — `bootstrap/managed/tests_intent_matrix.rs`, `MANAGED_INTENT_MATRIX: [Fault; 15]`**, 13 repeated + 2 single-crossing: `initial_intent_scratch_{create,write,flush}` (`:1005-1007`→`:1094/:1106/:1112`), `initial_intent_{publish,reobserve}` (`:1032-1033`→`:1152/:1155`), `successor_scratch_{create,write,flush}` (`:1010-1012`), `successor_scratch_reobserve` (`:1133`), `successor_{publish,reobserve}` (`:1036-1037`), `prior_generation_{retire,reobserve}` (`:1040-1041`), `final_intent_retire` and `final_intent_retired_reobserve` (`:1044-1045`, both single-crossing).

**Step 3.2 — `bootstrap/managed/tests_writer_matrix.rs`, `MANAGED_WRITER_MATRIX: [Fault; 5]`**

| key | site | class |
| --- | --- | --- |
| staging_directory_create | `:493` | single-crossing |
| ownership_marker_create | `:923` | repeated |
| ownership_marker_write | `:935` | single-crossing |
| ownership_marker_flush | `:941` | single-crossing |
| staging_directory_flush | **two** sites, `:519` and `:950` | single-crossing |

**New at Step 5.1** (Phase 3 settle disposition 5, §6): the same five keys also
carry multi-component interruption/restart/convergence rows, on both variants.

**The two never-boundary keys.** `managed_bootstrap.preflight` and
`managed_bootstrap.plan_complete` have **zero** injection sites anywhere —
verified: each variant's only occurrence in the repo is its own row in
`fault_v1.rs`. Their evidence is the **Phase 3 settle determination** filed at
freeze §3.5 (2026-08-23, at the Step 3.3 landing `3a45619`): they name plan-level
*states*, not durable edges, and are proved by the purpose-policy matrix's
ProofOnly/Scheduled arms and `coordinator/tests_execution.rs` — which is the
correct evidence form for a state, as fault injection is for an edge. The
key-by-key siteless proof in `fault_expected_keys.rs` is their permanent
machine-checked evidence. This is exactly the reading Step 5.1's evidence duty
was amended to at that settle.

**One soft gap, carried openly.** `staging_directory_flush` has two sites but
only one interruptible one: site B (the managed parent's flush) runs only on a
creating drive and therefore only after site A has already fired, so an arm on
the key always resolves to A. Making B independently interruptible needs a
site-ordinal-aware arm or a sixth key, and a sixth key would move the frozen 165
census. Recorded at the key's own classification in `tests_writer_matrix.rs`, and
adjudicated *honest* at the Step-3.2 landing (settle disposition 4).

### 4.6 `runtime` — 18 keys declared executed, **0 keys with per-key evidence**

This is the tuple's one genuine evidence gap and it is stated rather than
smoothed. `FAULT_FAMILY_ACTIVATION` declares `runtime` as
`Executed("R2-A/R2-B runtime bootstrap and catalog lease")`, and
`fault_expected_keys.rs` hard-codes an exemption so the "an executed family must
have sites" assertion skips it.

Verified against the tree: **none of the 18 `runtime.*` keys has a
`CheckedArtifactFaultKeyV1` injection site, and none appears in any matrix.**
What exists instead is a disjoint mechanism — `bootstrap/runtime/fault.rs`'s own
`RuntimeBootstrapFault` enum with **six** variants, driven from six targeted
substitution-rejection tests:

| `RuntimeBootstrapFault` | production site | driving test |
| --- | --- | --- |
| FinalLeaseOpen | `bootstrap/runtime/mod.rs:140` | `changed_final_identity_between_open_and_lock_is_rejected` |
| FinalLeaseLock | `bootstrap/runtime/mod.rs:145` | `substituted_runtime_parent_after_final_lock_is_rejected`; `changed_linked_worktree_git_indirection_after_lock_is_rejected` |
| CatalogInitialRetentionComplete | `catalog_lease.rs:102` | `linked_membership_drift_after_initial_retention_rejects_before_preparation_in_both_orders` |
| CatalogPreparation | `catalog_lease.rs:136` | `target_reacquisition_mismatch_after_preparation_is_read_only`; `linked_membership_drift_after_preparation_rejects_before_final_acquisition` |
| CatalogFinalLeaseOpen | `catalog_lease/target.rs:347` | `replaced_git_lock_between_open_and_lock_is_rejected` |
| CatalogFinalLeaseLock | `catalog_lease/target.rs:352` | `substituted_git_target_after_final_lock_is_rejected` |

**Three things follow, and the dual should rule on the third.**

1. The freeze **does** record the exception — §3.5's closing paragraph says
   `runtime.*` "is recorded as executed on a different mechanism from the rest",
   naming this enum and these files. So the tuple is not discovering an
   undeclared state; it is measuring what the declaration is worth.
2. What the declaration is worth is **less than the other five families'**. There
   is no name correspondence between the 18 stable keys and the 6 enum variants;
   the six tests are substitution-rejection tests, not
   interruption/restart/convergence matrices; and none is variant-paired. So for
   these 18 keys Step 5.1 cannot produce the per-key table §4.1-§4.5 produce.
3. **Open question for the 5.2 dual:** either the 18 keys are vocabulary with no
   owner and should be re-reserved like `cleanup.*`/`barrier.*`/`terminal.*`
   were, or the family's "executed" claim should be restated to mean what it
   actually is. Both are census-neutral. Recorded in §9 as an open item rather
   than decided here — restating a frozen §3.5 row is a reviewed edit, not an
   evidence step's to take.

For completeness: `catalog_bootstrap` (25/25) **is** fully executed — sites in
`provider/mutation.rs` (9), `provider/directory_mutation.rs` (15),
`provider/aggregate.rs` (1); both-variant matrices at
`restart_and_substitution_matrix_covers_every_catalog_bootstrap_fault_key` and
`restart_and_substitution_matrix_covers_git_directory_targets`.

### 4.7 The reserved families — 38 keys, zero sites, all re-reserved for R2-E

Verified per key: every variant of these three families was grepped as
`CheckedArtifactFaultKeyV1::<Variant>` and as the test alias `Fault::<Variant>`
across all of `src/`; **every one returns zero hits outside `fault_v1.rs`.**

| family | keys | non-activation record |
| --- | --- | --- |
| `cleanup` | 11 | freeze §3.5 `cleanup.*` non-activation record (Step 4.1 landing). Its edges are the action-scoped `CleanupWorklistV1` lifecycle and the three `CleanupAliasV1` retirements — all need an `AdmittedActionV1`; plan §5 items 1-2. |
| `barrier` | 16 | freeze §3.5 `barrier.*` non-activation record (Step 4.2 landing). Its edges are the frozen **roaming-anchor** protocol, whose `BarrierIntentV1::issue` refuses without a reservation and an in-schedule ordinal. |
| `terminal` | 11 | freeze §3.5 `terminal.*` non-activation record (Step 4.2 landing). Every key is the admitted action directory's terminal retirement into the catalog's retired root — §4.4's "E7's Phase-4 half and the terminal retirement edges". |

### 4.8 Census arithmetic

165 total = 18 `runtime` + 25 `catalog_bootstrap` + 19 `admission` + 11
`durable_leaf` + 11 `namespace` + 13 `record` + 30 `managed_bootstrap` + 11
`cleanup` + 16 `barrier` + 11 `terminal`. Counted from `fault_v1.rs`; equals
`EXPECTED_KEY_COUNT`. **Keys with a per-key executed table in this document:
19 + 11 + 11 + 13 + 28 + 25 = 107.** Keys with a filed determination instead: 2.
Keys re-reserved with a record: 38. Keys declared executed on another mechanism,
without per-key evidence: 18 (§4.6). 107 + 2 + 38 + 18 = 165.

---

## 5. LOC / cohesion audit and the ChangeBudget rows

### 5.1 Per-step rows

Buckets: **prod** = `src/**/*.rs` with no `tests` path component and basename not
starting with `tests`; **test** = the remaining `src/**/*.rs`; **scripts** =
`scripts/**`. A **quasi** column isolates the part of *prod* that is really test
code living in `interface_tests/`, `catalog_tests/` and friends — the literal
rule counts it as production, and the audit should not pretend otherwise.

| # | sha | step | prod net | *quasi* | test net | scripts |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `d32b2c9` | Phase 0 freeze (parked) | +547 | *431/0* | +191 | 2/2 |
| 2 | `2ed2435` | Phase 0 round-2 remediation | +86 | *87/1* | 0 | — |
| 3 | `3e60529` | M5b (parked) | 0 | — | +1398 | 1/1 |
| 4 | `8c1624a` | M5b acceptance | 0 | — | +47 | 7/1 |
| 5 | `5a7ff0f` | Steps 1.1+1.2 | +1162 | *113/2* | +761 | 9/2 |
| 6 | `c13f773` | Step 1.3 | +123 | *19/4* | +416 | 1/1 |
| 7 | `bf438ed` | Phase 1 settle remediation | +80 | — | +426 | 1/1 |
| 8 | `d2cf755` | Steps 2.1+2.2 | +1579 | *74/15* | +2074 | 6/2 |
| 9 | `c2564ba` | Steps 2.3+2.4 | +2281 | *246/13* | +1875 | 13/1 |
| 10 | `8b83a2c` | D3 durable cursor | +888 | — | +1201 | 3/3 |
| 11 | `e72e376` | Step 3.1 | +837 | — | +533 | 2/2 |
| 12 | `fcec69e` | Step 3.1b | +558 | *74/30* | +478 | 1/1 |
| 13 | `7169d89` | Step 3.2 | +77 | *38/21* | +581 | 1/1 |
| 14 | `3a45619` | Step 3.3 | +519 | — | +503 | 1/1 |
| 15 | `9c454ce` | Step 4.1 | +187 | *54/0* | +294 | 20/10 |
| 16 | `51a9cba` | Step 4.2 | +370 | *85/0* | +682 | 14/4 |
| 17 | `514f8e6` | Step 4.3 | +151 | *85/5* | +65 | 4/4 |
| | | **TOTAL** | **+9445** | *1306/91* | **+11525** | **+49** |

Test:prod net = **1.22 : 1** as bucketed; **1.55 : 1** once the quasi lines are
re-attributed to test, which is the honest reading.

**dev-docs.** Zero churn inside `gwz-core/dev-docs/` across the whole range — the
program's documents live in the parent repo, where the matching range is
**+26797 / −61 = +26736 across 57 files**, i.e. **1.27×** the total code net.
That ratio is the shape of this program and belongs in the record.

### 5.2 The two compositions the settle records specify

- **D3 (`8b83a2c`)** — recorded composition **+2192 / −103**; the excess is the
  **755-line §8 suite**. Measured here as prod +888 / test +1201 (= +2089 net over
  the two `src/` buckets, +2192/−103 over all buckets including scripts), which
  reconciles.
- **Step 4.2 (`51a9cba`)** — recorded **net +1061**, composed **≈28% production /
  ≈20% bound relocation / ≈52% evidence**. The bound-relocation fifth is the
  `platform.rs` → `platform/anchor.rs` split the Step-4.1 review ordered by name;
  it is movement, not new logic, and the ChangeBudget row should read it that way.
  Second consecutive over-budget Phase-4 step, recorded per the reviewer's
  guidance rather than trimmed.

### 5.3 Dead-code reality — the R2-E-awaiting figure

| measure | value |
| --- | --- |
| `dead_code` warnings on the tree as it stands | **0** (whole crate emits zero diagnostics of any kind) |
| items hidden by the module-level blankets (measured at Step 4.3 by deleting them) | **481** |
| global measure recorded at the Phase 4 settle | **1657 spans / 85 files** |
| `#[allow(dead_code)]` sites under `checked_artifact/` | **29 across 12 files** |
| whole crate | 56 sites across 28 files |

**The measured 0 is not evidence of no dead code; it is evidence that the lint is
switched off over most of the subsystem.** Seven subtree blankets remain in
`checked_artifact/mod.rs` — `bootstrap`, `capability`, `entry`, `fault_v1`,
`leaf`, `namespace`, `protocol` — covering **103 of 160 files and 34,941 of
54,962 lines: 63.6% of `src/checked_artifact/`**. Every hidden item is
R1/R2-frozen interface whose first consumer is R2-E; Step 4.3's narrowing found
**zero orphans**, two stale covers (deleted) and two over-broad ones (moved
inward).

*Measurement caveat worth carrying:* a naive `grep '#\[allow(dead_code'` returns
9 for `checked_artifact/`, which is wrong in both directions — it counts prose
inside doc comments and misses the 21 multi-line attributes. The 29 above is from
a paren-matching scan.

### 5.4 File-size outliers against the 1,000-line cohesion trigger

| lines | file | class |
| --- | --- | --- |
| 5351 | `protocol/generated.rs` | generated — exempt |
| 1362 | `workspace_ops/merge/v1_lifecycle/tests/forward.rs` | test |
| **1251** | `checked_artifact/capability/pre_catalog/provider/managed_mutation.rs` | **production — over** |
| 1188 | `git/tests/g12.rs` | test |
| **1097** | `artifact/mod.rs` | **production — over, pre-R2-D** |
| 1032 | `git/tests/g01.rs` | test |
| **1006** | `workspace_ops/handle_materialize.rs` | **production — over, pre-R2-D** |
| 993 | `checked_artifact/bootstrap/managed/tests_provider.rs` | test, just under |
| 884 | `checked_artifact/platform.rs` | **post-split; was 1,093** |

**One R2-D-owned production file is over the trigger: `managed_mutation.rs` at
1,251.** It is the single owner-private mutation file that holds *all* 28
activated `managed_bootstrap.*` injection sites plus the writer, intent and
marker edges — the concentration is the design (owners hold no sites), so the
split, if one is wanted, is along the edge families rather than arbitrary. Filed
in §9 as an R2-E/R2-F candidate, not as a Step 5.1 action: splitting the file that
holds 28 verified injection sites during the evidence step would invalidate the
evidence the step exists to produce.

`platform.rs` at 884 is the Step-4.2 split's result and is **under** the trigger.
The other two over-trigger production files predate R2-D and are outside this
lane's owners.

---

## 6. The settle-assigned test debt — discharged

**Phase 3 settle disposition (5): multi-component writer interruption rows.**

Step 3.2 drove the five writer boundaries on a deliberately **one-component** row,
because every boundary inside `stage_component` is crossed once *per component*
and the single-crossing criterion is written for a boundary crossed once per
drive — a fact the probe caught loudly rather than letting it pass. The coverage
that shape deferred is the interruption whose resume must finish a **half-staged**
row.

Added in the matrices' own idiom, reusing the shared harness:

| new test | variant |
| --- | --- |
| `managed_writer_multi_component_interruption_restart_convergence_on_a_workspace_target` | workspace |
| `managed_writer_multi_component_interruption_restart_convergence_on_a_git_directory_target` | Git-directory |

Each drives all five writer boundaries on `RowShapeV1::TwoComponent`
(`.gwz/stash/bundles`, two missing components, five generations) — **5 rows per
variant, 10 total** — asserting convergence to the settled census twice, so
"converged" means *settled* rather than merely reached.

**What they deliberately do not assert, and why it matters.** No partition, no
single-crossing probe. Both are claims about a boundary crossed once per drive;
on this shape they are false, which is the exact finding that made Step 3.2 pick
the one-component row. Re-asserting them here would reintroduce the
misclassification the probe exists to catch. The keys' classification stays the
one-component matrix's; these rows add convergence coverage, not a second
taxonomy.

---

## 7. This step's delta

| file | +/− | why |
| --- | --- | --- |
| `checked_artifact/bootstrap/managed/tests_writer_matrix.rs` | +36 | §6, the settle-assigned rows |
| `checked_artifact/coordinator/execution.rs` | +12 / −6 | §2.1, the stale audit note |
| `scripts/checks/test_check_checked_artifact_boundaries.py` | +12 / −1 | §2.3, the drifted companion |

**3 files, +60 / −7, net +53** — inside the <300 budget.
**No digest pin moves**; the boundary checker is `ok` on the delta.

---

## 8. Gate tails, verbatim

```
$ cargo fmt --all -- --check
(clean, no output)

$ cargo test --lib -p gwz-core checked_artifact::
test result: ok. 400 passed; 0 failed; 0 ignored; 0 measured; 1170 filtered out; finished in 37.37s

$ cargo test --lib -p gwz-core
test result: ok. 1569 passed; 0 failed; 1 ignored; 0 measured; 0 filtered out; finished in 700.98s

$ cargo test --test diff_render_spike -p gwz-core
test result: ok. 10 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.14s
$ cargo test --test protocol -p gwz-core
test result: ok. 29 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.22s
$ cargo test --test publish_workflow -p gwz-core
test result: ok. 9 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s
$ cargo test --test rename -p gwz-core
test result: ok. 2 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.13s

$ cargo test --test protocol -p gwz-core generated_
test taut_command_forces_utf8_for_generated_source_files ... ok
test generated_checked_artifact_protocol_is_current ... ok
test generated_protocol_is_current ... ok
test result: ok. 3 passed; 0 failed; 0 ignored; 0 measured; 26 filtered out; finished in 0.19s

$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)

$ python3.13 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py
Ran 65 tests in 447.459s
FAILED (failures=1)               # BEFORE the §2.3 repair — pre-existing at 514f8e6
$ python3.13 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py   # after
Ran 65 tests in 437.128s
OK

$ python3.13 -m unittest scripts/checks/test_release_boundary.py
Ran 6 tests in 0.044s
OK

$ python3.13 scripts/checks/check_merge_docs.py
merge document consistency: ok (11 sources, 147 assertions)
$ python3.13 -m unittest scripts/checks/test_check_merge_docs.py
Ran 3 tests in 0.501s
OK

$ python3.13 scripts/checks/check_merge_compatibility_predicates.py dev-docs/GwzM5-8I2CompatibilityPredicates.json --core .
validated 7 migration rules and 7 runtime bindings
$ python3.13 -m unittest scripts/checks/test_merge_compatibility_predicates.py
Ran 14 tests in 0.017s
OK

$ python3.13 -m unittest discover -s scripts/retained_readers -p 'test_*.py'
Ran 86 tests in 311.851s
OK
$ python3.13 scripts/retained_readers/retained_reader_harness.py validate scripts/retained_readers/manifest.json
{"status": "valid", "tuple_count": 24}
$ python3.13 scripts/retained_readers/retained_reader_harness.py gate-ready scripts/retained_readers/manifest.json
{"status": "manifest-ready", "tuple_count": 24}

$ cargo clean -p gwz-core && CLIPPY_CONF_DIR="$PWD" cargo clippy -p gwz-core --all-targets --all-features -- -D warnings
     Removed 1616 files, 1.6GiB total
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 12.80s
(zero diagnostics)
```

**Two gates could not be executed here, and neither is this tuple's to run:**
the retained-reader **matrix** job downloads released reader binaries over the
network (five platform runners) and is CI's; `protocol/regen.py --check` needs a
PyPI `taut-proto` venv and is `scripts/release.py`'s. The protocol *currentness*
property is covered locally by the two named tests above, which regenerate and
byte-compare.

---

## 9. The platform record

| run | tree | Windows | macOS-14 | ubuntu-24.04-arm |
| --- | --- | --- | --- | --- |
| 16 | `6c7c8f3` | 1399 / **4** / 1 | — | — |
| 17-18 | `c2d2f15` | **18**: 1472 / 0 / 1 | 1521 / 0 / 1 | 1510 / 0 / 1 — *first green ARM run in the program's history* |
| 19 | `9c454ce` (4.1) | **1506 / 0 / 1** ✅ `32607411300` | **1544 / 0 / 1** ✅ | **1555 / 0 / 1** ✅ `32607412448` |
| 20 | `51a9cba` (4.2) | **1526 / 0 / 1** ✅ `32613728318` | **1565 / 0 / 1** ✅ | **1576 / 0 / 1** ✅ `32613729322` |
| 21 | `514f8e6` (4.3) | **1528 / 0 / 1** ✅ `32616954369` | **1567 / 0 / 1** ✅ | **1578 / 0 / 1** ✅ `32616955672` |

**Three consecutive fully-green matrix trees — 19, 20, 21 — on all three arms.**
Nine of nine arm-runs green, zero failures, across the three Phase-4 landings.

Run 16's four failures were the namespace fault matrix's first Windows execution,
all at baseline with one typed message — the 2.2 barrier anchor-readiness gap,
diagnosed and fixed at `6b8b76e`, extinct from run 18 onward.

Run 21's Windows arm was still in flight while this document was drafted and
**completed green before it was finished**, so the slot the brief reserved for a
TODO is filled rather than deferred: `32616954369` at `514f8e6`,
**1528 passed / 0 failed / 1 ignored** in 2811 s, conclusion `success`.

Probe runs against this program's `cfg(windows)` semantics, for the record:
`32612243125` (anchor retirement, 82/0, final) — and the cycle that produced it
caught a fixture bug red-then-green, which is the probe working as intended.

---

## 10. Escalation ledger — final for the Phase 5 dual

Copied from the checkpoint's PHASE 4 SETTLED record:

- **Riding:** 2.3 (1 P1 + 3 P2), 2.4 (P0 + P1 + P2), anchor (1 P1 + 4 P2),
  3.1 (1 P1 + 1 P2), 3.1b (recorded), 4.3 (1 P2, remediated at landing).
- **Discharged on record:** 2.2, 3.3 (round 2), 4.2 (round 2).
- **Plus the named class audit:** *caller-supplied restatement* — fired at 2.4
  and at 3.3; **hardened at the seam by 4.3** (settle item 8), which moved the
  action-digest check from the consumer into
  `CheckedAuthorityObservationOwnerV1::observe` itself and put the derivation
  obligation on the issuer's signature.

Step 5.1 adds no escalation: its own findings (§2.1, §2.3) are record- and
companion-class and were fixed in the step.

---

## 11. Open-item register

### 11.1 Re-reserved for R2-E

| item | record |
| --- | --- |
| `cleanup.*` — 11 keys | freeze §3.5 non-activation record (Step 4.1) |
| `barrier.*` — 16 keys | freeze §3.5 non-activation record (Step 4.2) |
| `terminal.*` — 11 keys | freeze §3.5 non-activation record (Step 4.2) |
| The whole §10 consumer conversion table | plan §5 item 1 |
| First production catalog activation | plan §5 item 2; gated by §11.3 |
| The 481-item / 63.6%-of-subsystem frozen surface behind the seven blankets | §5.3 |

**Added at the settled-dual round-2 remediation (2026-08-23):**

- `authority_name` embeds no identity digest (the non-self-checking-name
  design; descends from the Step-4.1 review's E21 P2 finding) — R2-E must
  weigh a self-checking rename or a compensating observation when it gives
  the family its next consumer (checkpoint 4.1 record; State [P3-2]).
- `BarrierIntentV1::issue` (`protocol/barrier.rs:39`) accepts caller-asserted
  anchor/home identity facts with no observation check — the named
  caller-restatement class's one remaining seam shape, production-unreachable
  today (sealed witness, zero production callers). **BINDING R2-E
  obligation**: apply the 4.3 observe-or-refuse pattern before the roaming
  anchor gains its first admitted-action caller (Code [P3-1]).
- Anchor nits inside the accepted same-user boundary (Code [P3-5]): `verify`
  rides the legacy unbounded `read_to_end`; the retired-ordinal parse accepts
  non-canonical renderings (convergence unaffected); `unsupported.rs` claims
  the Linux support profile (fail-closed shields it). R2-E/R2-F hygiene items.
- The ten multi-component writer rows (5.1's delta) have no native
  Windows/ARM execution record — runs 19-21 predate `d45458d`; expected-green,
  ledger entry owed at the next matrix dispatch (Code [P3-4]).
- `root_fault_matrix` perf pricing: **318.71 s (release, `b91bdeb`)** is the
  citable price (driver-corroborated at 327.6 s wall); 576.03 s stands as the
  historical D3-review-tree figure. **PRICED-NOT-OPTIMIZED, ruling signed
  2026-08-24** *[date corrected in place 2026-08-24 per the R4b-G Evidence
  axis [P3-3]: it read "2026-08-23", a date on which neither the ruling nor
  this row existed — both landed in gwz-dev `d378db9` at 2026-08-24 10:24:43,
  and the ruling cites a 327.6 s corroboration measured 2026-08-24. Note this
  row postdates the block header above it, which is dated 2026-08-23 and is
  left unedited as the header of the block it names.]* — the cost is the §8
  crash matrix's evidence weight, accepted; A1-activation-review input per
  `GwzM5-8ThinA1Amendment.md` §1 / L1-19. **The "~343 s release" figure this
  ruling corrects came from the lane owner's session notes — an out-of-band
  recollection. It is not from the A1 brief, not from any other brief, and not
  from any document in the corpus; an exhaustive search of the workspace and
  its full history finds no such figure** *[provenance stated 2026-08-24 per
  Evidence axis [P3-3], which found "the A1 brief" to be the one unsourced
  authority reference in the record and required the source be named or the
  figure marked out-of-band]*. Independently corroborated by the R4b-G
  Evidence axis at **352.48 s release on `78badbc`** — inside the 319-353
  band, decisively not 576. Record: `GwzM5-8R4bG-Evidence.md` §8 item 3
  addendum.
- The L2-05 merge-doc gates are wired into no workflow — **wire them into CI
  before any lane relies on L2-05 mechanically** (both axes concur; Code
  [P3-6], tuple §11.3). *[R4b-G note 2026-08-24: still true at `78badbc`,
  independently re-verified by both R4b-G axes; the three new R4b-G tools
  (privacy probe, M4 map checker, aggregate driver) join this CI-wiring item on
  the A1-activation register per the Correctness axis's J-7 ruling.]*

**Added at the R4b-G dual's round-2 remediation (2026-08-24):**

- **C-1 — the O8 "every M4" byte-equivalence clause is measured NOT MET
  (13/39). [P2] — OPEN, BLOCKING-FOR-A1 per L1-19
  (`AgentProcessRules.md:392-401`).** Both R4b-G axes converged: 7 ADAPTED +
  6 UNLISTED = 13 bound; **22 UNBOUND; 4 NO FIXTURE**. The 22 are **not** an
  unmeasured behavioural space — the frozen accepted compatibility contract
  disposes of them categorically:
  `GwzM5-8I2CompatibilityContract.md:117-125` ("A1 deliberately whitelists
  only seven one-member-workspace, `Finalizing`, normal-mode shapes …
  Marker/lock-only prefixes, multi-member workspaces, selected root, born
  root, drift, pending actions, recovery, preservation, rollback, and terminal
  rows are not A1 migration rules") and `:159-165` ("**Zero whitelist matches
  is not an error** … Completed and aborted v0 records remain v0 and use
  byte-preserving archival. Archived v0 uses only the archive
  decoder/projection"). Every one of the 22 falls in a named exclusion class,
  and 18 carry v0 byte/restart pins executed green inside g23 111/0. What is
  missing is the **per-scenario record** where
  `check_merge_compatibility_predicates.py` looks. **The class cite does not
  cover the whole residue — four rows are live and named:**
  `J-NO-PUBLICATION-UNBORN` (the only UNBOUND row whose class membership is
  ambiguous — whether the whitelisted no-publication descriptor matches the
  unborn-root twin is untested per-case; Evidence axis [P3-1]); and
  **`F-BASELINE`, `F-MARKER`, `F-LOCK`** — all three `Finalizing`-state, i.e.
  inside the only state the whitelist adapts, with **adaptation disposition
  unstated and untested** (no test drives `adapt_open` on them; Correctness
  axis C-1). **Pre-A1 exposure is ZERO**, verified in code by the Correctness
  axis: three independent `cfg(test)` gates
  (`merge/mod.rs:23-24`, `record_wire/mod.rs:13-14`, `merge/mod.rs:44-50`)
  stand between production and the v1 writer/migration dispatch, so the
  unbound dispositions have no reachable consumer at this tuple. **Owner: the
  A1 activation review's input register**, routed under
  `GwzM5-8ThinA1Amendment.md:43-55` / `:242-244` branch (a)/(b). Record:
  `GwzM5-8R4bG-Evidence.md` §12.7.
- **C-2 — four M4 scenarios have no durable fixture at all. [P3]** —
  `B-NOT-STARTED`, `B-PREPARING-EMPTY` (R0 §4 row B's own recorded gap,
  re-verified still true), `AC-NOPUB-UNBORN`, `AP-PRESERVED` (live-workspace
  GC evidence exists at `characterization_preservation_v0.rs:442` and
  `gc.rs:46`, but no archive-projection fixture, exactly as
  `GwzM5-8R0Inventory.md` §5.2 records). Undispositionable either way until
  fixtures exist; a regression touching them is invisible to every battery.
  Both axes concur at P3. **Owner: the A1 activation review's input register**,
  alongside C-1. Record: `GwzM5-8R4bG-Evidence.md` §12.7.
- **M5b-IMPL settled review — OWED PRE-A1.** The frozen dependency statement
  (`GwzM5-8M5bNoFfDesign.md:976-986`, "**Dependent on R4b-G settling — must
  wait:** … Merging and settled-review of M5b-IMPL") was overtaken on its
  *merge* leg — `3e60529`/`8c1624a` reached `b91bdeb`'s ancestry ahead of
  R4b-G — and that sequencing deviation is adjudicated **ACCEPTED WITH
  RECORD** in `CurrentProgramCheckpoint.md`'s J-1 record, **ratified by both
  R4b-G axes at round 1** on independently re-verified facts (prod net **0**
  for both commits at §4's LOC audit rows 3-4, `:375-376`; the D3 dual's
  round-2 byte-identity re-verdicts; the F-3 standing guard proved
  fail-closed). **The named before-A1 remedy is NOT triggered.** What survives
  is the *review*, not the merge: the **M5b-IMPL settled review remains owed
  before A1 activation**, on the tier recorded at its freeze (mandated-dual by
  default; single-axis only for test/fixture-confined diffs). R4b-G's GO
  unblocks it; it is not discharged by it.

**Added at the pre-A1 closures (2026-08-24):**

- C-1 — CLOSED at gwz-core `26f48f5`: the four live residue rows
  dispositioned from the frozen contract (two UNLISTED, two REFUSED-typed,
  the J twins split on the born-root exclusion — with the enumeration's
  labels corrected: the ADAPTED case is the UNBORN twin), tested through
  the real decode path with a positive control; the M4 map binds 41 named
  tests. An explicit A1-activation-review input.
- M5b settled [P2-1] (the T-5 no_ff envelope pair) — **CLOSED BY
  LANE-OWNER NARROWING, ruled 2026-08-24**: the retained-reader matrix's
  frozen contract keys, name-derives, and exactness-checks envelope
  classification entirely on `(schema, record_schema_version)`
  (`retained_reader_matrix.py:47-52/:83-88/:177-183`,
  `retained_reader_harness.py:93-95`) — the record body, where `mode`
  lives, is structurally incapable of participating, which mechanizes the
  mode-blindness property in the tooling itself. The literal fixture
  requires a five-surface frozen-contract change plus regeneration of
  executed-binary evidence over the network (blockers measured, not
  assumed). The candidate pair is BUILT and SAVED with digests
  (`scratchpad/insurance-t5/CANDIDATE-generator-t5-pair.NOT-APPLIED.patch`;
  e4ea14de… / a1cb103c…) and RIDES ANY FUTURE evidence regeneration
  (R2-F's native-evidence gate is the natural carrier). The A1 activation
  review judges this ruling as its input.

### 11.2 Pinned to R2-F

- **T-5 candidate pair — OWNED CARRIER: the R2-F native-evidence
  regeneration (binding per the A1 Safety review's T-5 acceptance).** The
  built `mode: no_ff` v1 envelope pair (digests `e4ea14de…` /
  `a1cb103c…`) is durably archived at
  `dev-docs/GwzM5-8T5CandidatePair.patch` (moved from session scratch at
  the A1 Completeness review's [P2-1]); it lands with the first evidence
  regeneration that re-runs the retained-reader matrix against released
  binaries, alongside the five-surface variant-key rule change enumerated
  in the T-5 closure report.


| item | record |
| --- | --- |
| **Quarantine/relocation execution** — the legacy private area moves out of the catalog Final directory | `GwzM5-8R2DPhase4Closure.md` §2.7; freeze §5 decision 2; plan §5 item 7 |
| MAX_PATH product exposure (~173-char `ca1-*` names), retired *by* that relocation *[FALSIFIED 2026-09-01, R2-F R1.3: the relocation landed (gwz-core `027da5b`) and retires nothing here — trace §5.2's arithmetic (+4 under `.git/`; 160/173 chars are hex) plus the landed one-name-two-directories design, which keeps the `ca1-*`-bearing legacy area in place. See Phase4Closure §2.4's dated note. Anchor re-pointed: the checkpoint text drifted from :1022-1024 to :3192-3194.]* | checkpoint :3192-3194 (was :1022-1024) |
| Legacy-path removal / dead-code allowances | plan §5 item 3 |
| Native power-loss semantics (the dirent-barrier residual's companion) | `DirentBarrier-ReviewState.md:393-395` |
| Native Windows/Linux fault evidence as a release gate | plan §5 item 4 |
| Twin-seam unification: `publish_verified_leaf_no_replace` duplicates ~25 lines of `publish_verified_no_replace`'s verification; both halves are pinned so drift is reviewed, but nothing pins their **equivalence** | Step-4.1 review [P3-1] |

### 11.3 Settle caveats the 5.2 dual should weigh

1. **The A1 coexistence gate.** No production catalog activation at
   `.gwz/checked-artifacts` until the relocation lands. Fail-closed in code today:
   `recover_or_create` has no production caller (re-verified at this tuple).
   *[SATISFIED 2026-09-01 (R2-F R1.3): the relocation LANDED — R1.1 at
   gwz-core `027da5b` (the catalog composes `catalog-final`; the legacy
   writer keeps `checked-artifacts` under its own variant, inside the
   collision domain). The fail-closed property outlived the prose: the R1.2
   activation tripwire (`interface_tests/catalog_activation_pin.rs`,
   `bb52dc0`) pins the production caller count at ZERO until E4.1 moves it
   deliberately, in the same reviewed commit as the caller it adds. Gate
   LIFTED — production catalog activation is now Phase E4's to take.]*
2. **The test-door dependency.** Step 3.2's Git-directory purpose-policy arm
   places the managed prefix under that target's own retained root because the
   Step-2.3 door (`retain_managed_parent_at_for_test`) stages with `fs::write`
   and can never reach the writer boundaries. Kept as a documented `cfg(test)`
   door; revisit on an R2-E production caller (settle disposition 2). The
   Git-directory catalog's workspace-root binding is an R2-E input (disposition 3).
3. **`runtime.*` — 18 keys declared executed with no per-key evidence (§4.6).**
   The dual should rule: re-reserve, or restate the declaration. Census-neutral
   either way.
4. **`managed_mutation.rs` at 1,251 lines** — the one R2-D-owned production file
   over the cohesion trigger (§5.4).
5. **`staging_directory_flush`'s second site** is announced but not independently
   interruptible; a sixth key would move the frozen census (§4.5).
6. **The operator policy question.** The pre-A1 checklist items — operator escape
   runbook, panic-invariant conversion packages, preservation-cursor decision —
   are parallel-program debts (plan §5 item 7), not R2-D's, and are unresolved.
7. **Two L2-05 gates are wired into no workflow.** `check_merge_docs.py` and
   `test_check_merge_docs.py` are named hard gates in `AgentProcessRules.md` §4,
   but no workflow runs them: their default workspace root resolves to the parent
   checkout and the manifest points at `gwz-cli/docs/*`, which a gwz-core-only CI
   checkout cannot satisfy. They are green here because this host has the sibling
   checkout. **A gate that only passes on a developer's machine is not a gate**;
   filed for the dual as a CI-wiring item.
8. **Second consecutive over-budget Phase-4 step** (4.2 at net +1061, 4.3 within);
   compositions recorded rather than trimmed, per the reviewer's ChangeBudget
   guidance (§5.2).
