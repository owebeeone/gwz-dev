# R2-D Phase 3 Step 3.2 — `managed_bootstrap.*` writer-key activation and the purpose policy matrix: single-axis State review

Date: 2026-08-22. **Single-axis State-lead** (`GwzM5-8R2DInterfaceFreeze.md` §9:
"Everything else is **single-axis with automatic escalation on any P0/P1/P2**:
… Phase 3 steps 3.1-3.3 **including the Phase 3 settle**"). Round 1 of a
two-round cap. Peer-blind: the implementer's report was read as *claims to
verify*, never as evidence; every number below was recounted and every gate
re-run by this review. **Escalation is NOT triggered** — no P0, P1 or P2. Five
P3 findings file and continue (§9 : "P3 findings file and continue; they do not
become packages and do not enlarge R2-D").

Object: the uncommitted Step 3.2 edits over gwz-core base `fcec69e` ("Land R2-D
Step 3.1b: the managed intent record's durable lifecycle (review GO)"), exactly
ten files:

| File | Disposition |
| --- | --- |
| `checked_artifact/bootstrap/managed/tests_writer_matrix.rs` | **NEW** |
| `checked_artifact/bootstrap/managed/tests_purpose_policy.rs` | **NEW** |
| `checked_artifact/bootstrap/managed.rs` | modified |
| `checked_artifact/bootstrap/managed/provider.rs` | modified |
| `checked_artifact/bootstrap/managed/tests_provider.rs` | modified |
| `checked_artifact/bootstrap/managed/tests_intent_matrix.rs` | modified |
| `checked_artifact/capability/pre_catalog/provider/managed_mutation.rs` | modified |
| `checked_artifact/fault_v1.rs` | modified |
| `checked_artifact/interface_tests/fault_expected_keys.rs` | modified |
| `checked_artifact/namespace/host.rs` | modified |

Baseline: gwz-core `fcec69e`; gwz-dev repo HEAD `0cf4e4d`.

**The dirty set is exactly these ten, and no other lane is in the tree.**
`git status --porcelain` in `gwz-core` returns exactly eight ` M` and two `??`
rows, matching the table; `git status` in `gwz-dev` is clean. So unlike the
Step-3.1b round no scratch reconstruction was needed and every gate below was
run **in the live tree**. Attribution of the one checker finding was still
*constructed rather than assumed*: I extracted `src/checked_artifact` at
`fcec69e` into scratch and re-ran the checker's own `source_tree_digest` over
both trees (§2).

---

## 1. Object inventory and what the authorities actually owe

Plan §4 Step 3.2 (`GwzM5-8R2D-Plan.md:372-379`), quoted whole because the whole
of it is the scope:

> **Step 3.2 — `managed_bootstrap.*` activation + purpose policy matrix**
> (<500 test LOC): all four purposes
> (`MergeStore`/`MergeArchive`/`PreservationBundles`/`RootPreservationMarkers`),
> overlap rejection (a missing `.gwz/merge` is `MergeStore`; `MergeArchive`
> legal only with an existing `.gwz/merge` — ConsumerCheckpoint §9 :253-256),
> every component/generation/marker boundary, repeated-crash slot stability.

I read `GwzM5-8R4bR2ConsumerCheckpoint.md:249-266` directly. §9 :253-256 says a
missing `.gwz/merge` "is prepared as `MergeStore`", and a `MergeArchive` action
"is legal only when `.gwz/merge` already exists because a source merge record
has been validated". The plan's paraphrase is faithful.

Freeze §3.5's deferral record and its Step-3.1b supersession clause
(`GwzM5-8R2DInterfaceFreeze.md:468-495`) name the duty exactly: five injection
sites for `staging_directory_create`, `ownership_marker_create`,
`ownership_marker_write`, `ownership_marker_flush`, `staging_directory_flush` in
`managed_mutation.rs`; their interruption/restart/convergence and
repeated-boundary rows on both target variants; the `PartiallyExecuted` list
edit, **23 → 28** as superseded; counts held at 165 throughout; and the
Git-directory arm to go through the Step-2.3 door **or** place its managed
prefix under that target's own retained root.

**Scope ruling on `preflight` / `plan_complete`: the implementer is right and I
verify it independently.** The plan's 3.2 clause is "every
component/generation/marker boundary". Neither key names a component, a
generation or a marker; both name plan-level states, and no step has given
either an edge. The plan's 3.2 text does not mention them by name anywhere
(`grep` over §4 Phase 3 confirms). So the brief's exception clause ("if the
plan's 3.2 text explicitly assigns them to 3.2") does **not** fire, the step
correctly left them alone, and the Step-3.1b review's [P3-1] settle docket
stands untouched. Both remain proved siteless key by key (§5).

---

## 2. Gate train — executed by this review, in the live tree

Every cargo call used
`CARGO_TARGET_DIR=…/scratchpad/p31-target`. To defeat a cached-green illusion I
ran `cargo clean -p gwz-core` before the clippy gate and re-ran it with `-v`, so
the clippy result below is a **real recompilation**, and I confirmed from the
`-v` transcript that clippy emits **two** `--crate-name gwz_core` rustc
invocations, **one of them carrying `--test`** — i.e. the `-D warnings` gate
genuinely lints the new `#[cfg(test)]` code rather than skipping it.

```
cargo check --all-targets                     Finished dev profile        [clean]

cargo clean -p gwz-core; CLIPPY_CONF_DIR="$PWD" \
  cargo clippy --all-targets --all-features -- -D warnings
                                              EXIT=0, 1 "warning" line, and that
                                              line is cargo's non-root-profile
                                              notice, not a lint

cargo test --lib checked_artifact::
  test result: ok. 360 passed; 0 failed; 0 ignored; 0 measured; 1170 filtered out

cargo test --lib …::tests_writer_matrix -- --nocapture
  test result: ok. 4 passed; 0 failed                       [evidence lines, both variants]
cargo test --lib …::tests_purpose_policy
  test result: ok. 10 passed; 0 failed
cargo test --lib …::tests_intent_matrix
  test result: ok. 4 passed; 0 failed
cargo test --lib …::tests_provider
  test result: ok. 12 passed; 0 failed
cargo test --lib …::interface_tests::fault_expected_keys
  test result: ok. 6 passed; 0 failed
cargo test --lib …::interface_tests::capability_permit
  test result: ok. 8 passed; 0 failed

rustfmt --edition 2024 --check <the 10 files>              RUSTFMT_EXIT=0

python3.13 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: failed
  - protected source tree changed: checked_artifact/capability/pre_catalog.rs
  EXIT=1                                    [exactly one finding, and it is this step's]
```

**Checker attribution, constructed.** `PROTECTED_SOURCE_TREE_DIGESTS` covers
`pre_catalog.rs` *and its whole descendant directory* (`source_tree_digest`,
`:769-785`), which is why a `provider/managed_mutation.rs` edit trips a pin
named for `pre_catalog.rs`. I re-ran the checker's own digest function over the
`fcec69e` tree extracted into scratch and over the live tree:

| Pinned tree | pin | at `fcec69e` | live |
| --- | --- | --- | --- |
| `bootstrap/runtime/catalog_lease.rs` | `1a13b933…` | match | match |
| `capability/path.rs` | `23e46dbd…` | match | match |
| **`capability/pre_catalog.rs`** | `05e4d248…` | **match** | **`3989a177…` differs** |
| `catalog.rs` | `9efa557b…` | match | match |
| `workspace_ops/…/observe.rs`, `…/mod.rs` | — | (not in the extract) | match |

The base matched the pin and the live tree does not, so the single finding is
caused entirely by this step's six `hit(` calls and the doc-comment edit in
`managed_mutation.rs`. Nothing else moved. It is a land-time tree-digest pin,
the lane owner's — no companion or fixture edit is owed by the step.

**Test-count delta reconciles.** `#[test]` counts, base vs live:
`tests_provider.rs` 12 → 12, `tests_intent_matrix.rs` 4 → 4,
`fault_expected_keys.rs` 6 → 6, `tests_writer_matrix.rs` 0 → **4**,
`tests_purpose_policy.rs` 0 → **10**. No test was deleted or renamed away;
346 + 14 = 360, matching the run.

---

## 3. Priority 1 — the five sites, and the two-sites-one-key adjudication

Every claimed site is real, in the claimed position, `#[cfg(test)]`-bounded, and
placed *after* the physical statement so a crash there means the statement
completed:

| Key | Statement | Site |
| --- | --- | --- |
| `staging_directory_create` | `self.handle.create_dir(&name)` `:489` | `:492-495`, inside the `NotFound` arm only |
| `ownership_marker_create` | `staged.open_with(&name, &options)` `:920` | `:922-925` |
| `ownership_marker_write` | `file.write_all(&marker.encode_canonical())` `:932` | `:934-937` |
| `ownership_marker_flush` | `file.sync_all()` `:938` | `:940-943` |
| `staging_directory_flush` (A) | `sync_directory_edge(staged, "flush managed staging interior")` `:945` | `:949-952` |
| `staging_directory_flush` (B) | `sync_directory_edge(&self.handle, "flush managed staging creation")` `:511` | `:518-521`, inside `if created` |

Recounts: `fault_v1::hit` in `managed_mutation.rs` moved **14 → 20** (five keys,
six calls); `namespace/host.rs` holds **0** and the whole `namespace` tree holds
zero `CheckedArtifactFaultKeyV1` references outside its `tests*` files — so the
liftable block's "the `namespace` owner still holds none" is true of the tree.
`create_dir(` occurs exactly once in the file. The other four
`sync_directory_edge` calls (`:618`, `:767`, `:768`, `:1115`) belong to E15/E16
and to the 3.1b scratch writer and correctly announce their own keys, not this
one. No writer boundary is left unannounced.

### The ruling: two sites under one key is honest naming, not a census dodge

Four reasons, in order of weight.

1. **The shape was specified by the authorities before this step existed.** The
   Step-3.1 review's own edge table (`GwzM5-8R2DStep31-Review.md:188`) reads
   "`staging_directory_flush` | the two `sync_directory_edge` calls (staged
   interior; parent on create)". The implementer's claim that the prior review's
   table specified this shape is **true, verbatim**. The freeze's deferral
   record likewise allocates exactly five keys to the edges of `stage_component`
   + `write_or_rewrite_marker`.
2. **Minting a sixth is not a step's move.** It would take `EXPECTED_KEY_COUNT`
   165 → 166 — the census freeze §3.5 pins and every annotation since Step 1.3
   has held ("no key minted"). That is a freeze edit.
3. **The two sites are the same state class**: the same `sync_directory_edge`
   primitive on a directory handle, in the same writer, making the same staged
   row durable. The key names a state, not a call site; that is the vocabulary's
   own convention (cf. `component_reobserve`, which marks "a fresh process chose
   the restart path", not a syntactic location).
4. **It is disclosed at the code**, with a comment at each site naming the other
   and the reason, and again in the liftable §3.5 block.

Against it stands one real consequence, filed as **[P3-1]**: site B can never be
the interruption point, and one sentence of the object's own shape rationale
says the opposite.

---

## 4. Priority 2 — the probe, and the misclassification it caught

**`run_single_crossing_probe` genuinely distinguishes.** I traced the mechanism
(`tests_provider.rs:564-598`) rather than trusting the description:

- The crash drive is wrapped in `catch_unwind`; the **re-armed** drive is not.
  If the boundary were re-crossed, `fault_v1::hit` takes the slot and invokes
  the callback, which panics with "a single-crossing boundary was re-crossed by
  the drive after its crash", and that panic escapes `drive()` and fails the
  test. There is no `catch_unwind` on the production path to swallow it.
- The trailing `assert!(take_armed_fault(), "the probe's arm vanished without
  firing")` closes the other side: a pass requires the drive to **settle** *and*
  the arm to still be installed. "Not crossed" is therefore proved, not merely
  not-disproved.
- The arm is thread-local and each `#[test]` runs on its own thread, and
  `run_next_at` asserts no live arm is being replaced, so no cross-row leakage
  can manufacture a pass.

**`take_armed_fault` has no production reachability.** It is `#[cfg(test)]`, as
are `NEXT_FAULT`, `run_next_at` and `hit`; `CheckedArtifactFaultKeyV1` is
`pub(super)` on a private `mod fault_v1`. In a non-test build the function does
not exist. Verified by reading `fault_v1.rs:203-238`, not by assertion.

**The caught misclassification is real.** `execute_row` calls `stage_component`
once per component (`provider.rs:352-356`, guarded by
`phase == InstallComponents && !installed_resident`), so on a two-component row
a crash at component 0's crossing leaves component 1's crossing ahead: the next
drive re-enters the boundary once more and then never again — neither repeatable
nor single-crossing under the stated criterion. The probe fires and fails. That
is exactly the account given, and it checks out against the code.

**The one-component row is justified, not evasive** — with one caveat filed as
[P3-2]. `RowShapeV1` (`tests_provider.rs:355-379`) makes the shape a *declared*
property of each matrix with its reason attached, and the intent matrix keeps
its two-component `.gwz/stash/bundles` row, which it needs to reach a
mid-retirement interruption. Mid-retirement multi-component coverage therefore
still exists and is still exercised (4/0 on `tests_intent_matrix`, unchanged in
key set: 15 keys, 13 repeatable, 2 single-crossing). What the one-component
choice does *not* cover is a crash at a **writer** boundary on a multi-component
row — see [P3-2].

**The retroactive probe does run on 3.1b's two single-crossing keys**
(`tests_intent_matrix.rs:142`, `SHAPE = TwoComponent`,
`SINGLE_CROSSING_BOUNDARIES = [final_intent_retire,
final_intent_retired_reobserve]`), and both pass. Those two are per-row, not
per-component, so the two-component shape does not compromise the probe there.

---

## 5. Priority 3 — the classification, key by key, against the code

**`ownership_marker_create` is repeatable, and the rewrite path is real and
converges.** `write_or_rewrite_marker` (`:901-955`) opens the frozen name
`managed_marker_name()` — a constant, so **no retry name is ever allocated** —
`create_new` when absent, existing-only when present. A crash at `:922` leaves
the marker present and empty. The next drive: staging row resident so
`created == false`; `observe_managed_component_interior` is **not** exact;
`write_or_rewrite_marker` re-entered; `symlink_metadata` → regular file →
`create_new = false` → `set_len(0)` + rewind + `write_all` + `sync_all` +
interior flush → re-observation exact. Converges, same name, cardinality cannot
grow. The 12-round row proves it empirically on both variants
(`rounds=12 | slots-stable=yes | converged=yes`).

The other four, each routing checked against the code:

| Key | Declared routing | Verdict |
| --- | --- | --- |
| `staging_directory_create` | next drive takes the `Ok(_) => false` arm | **True**: `:479-483`, the create is reached only from the `NotFound` arm |
| `ownership_marker_write` | bytes are the expected ones ⇒ interior exact ⇒ whole rewrite skipped | **True**: `:501-507`'s `is_exact()` guard |
| `ownership_marker_flush` | same state, one boundary later | **True** |
| `staging_directory_flush` | both instances sit at or after the point where the interior is exact; parent flush runs only on a creating drive | **True**, and see [P3-1] for what follows from it |

The comment on `ownership_marker_write` is careful in the right place — it says
in so many words that a simulated process stop is not a power loss and points at
the freeze's E9/E16 records for that reasoning. That is the honest form.

`assert_boundary_partition` (`:481-504`) machine-checks that the two classes
partition the matrix with no overlap and no orphan, and `reconcile_executed_keys`
(`:465-477`) ties the matrix to the fixture's activated stable-key list. Both
run in each of the four tests.

---

## 6. Priority 4 — the purpose policy matrix against plan §9 :253-256

Ten tests, five bodies × two variants, all green.

| Plan clause | Test | Verified |
| --- | --- | --- |
| all four purposes | `merge_start_bootstraps_its_pair_*` (`MergeStore` + `PreservationBundles`), `root_preservation_markers_bootstraps_*`, `merge_archive_bootstraps_over_a_resident_store_*` | all four **bootstrapped**, on both variants |
| production constructors only | `for_merge_start`, `try_for_durable_merge`, `for_archive` | all three are `pub(in crate::checked_artifact)` production items (`managed.rs:98/:111/:128`); none is `#[cfg(test)]` |
| no test-door bootstrapping | — | `retain_managed_parent_at_for_test` appears in `namespace/tests_managed.rs` **only**; neither new suite nor the shared harness touches it |
| §9 negative half | `merge_archive_is_refused_without_a_merge_store_*` | refused, and `!exists(".gwz/merge")` — nothing created. Mechanism confirmed: `MergeArchive.minimum_retained_parent_count() == 2` (`managed.rs:75-80`) against an observed count of 1, refused at `owner.rs:110-116` as `plan_mismatch("provider retained prefix violates the purpose ownership policy")`. Assertion strength: see **[P3-3]** |
| §9 positive half | `merge_archive_bootstraps_over_a_resident_store_*` | `rows().len() == 1`, `purpose() == MergeArchive`, `retained_existing_parent_count() == 2`, `missing_suffix().len() == 1` — exactly its own `done` component over a resident store |
| proof-only settle | `a_fully_present_purpose_set_plans_no_row_*` | `is_proof_only()` and `rows().is_empty()` after a real bootstrap |

**The matrix drives real durable state, not mocked observations.** Each run goes
through `with_catalog` → a real workspace runtime lease or a real Git catalog
lease → `recover_or_create` → `RetainedManagedParentProviderV1::from_retained_catalog`
→ the real `preflight`/`bind`/`execute`, then asserts against
`path.is_dir()` on the actual filesystem (`.gwz/merge`, `.gwz/stash/bundles`,
`gwz.conf/markers`, `.gwz/merge/done`). The file's own header draws exactly the
right distinction from `interface_tests/managed_plan_binding.rs`, which is the
synthetic-provider test — that one proves the owner enforces the policy against a
provider told what to report; this one proves the composition. Both are wanted;
neither substitutes.

---

## 7. Priority 5 — scope, counts, and the publication seam

**`preflight` / `plan_complete` untouched and still siteless-proven.**
`grep -rn ManagedBootstrapPreflight|ManagedBootstrapPlanComplete src/` excluding
`fault_v1.rs` returns **0** for each. They are absent from
`MANAGED_BOOTSTRAP_EXECUTED_KEYS`, so the `PartiallyExecuted` arm's second loop
(`fault_expected_keys.rs:626-648`) proves each of them siteless **individually**,
not family-wide. The settle docket stands, and the fixture's new prose names it
("their disposition is a Phase 3 settle determination, not this fixture's
(Step-3.1b review [P3-1])"). No disposition was taken.

**Counts, recounted.** `EXPECTED_KEY_COUNT` = **165**, byte-unchanged.
`managed_bootstrap.` keys in `fault_v1.rs` = **30**. Executed list length
asserted at **28** (`fault_expected_keys.rs:784-791`), so reserved = 2. Arithmetic
23 + 5 = 28 and 7 − 5 = 2 both hold, and 28 + 2 = 30. **No key minted.**

**Publication seam, recounted at base and live.** `publish_verified_no_replace(`
across `src/`: **21** at `fcec69e`, **21** live — byte-identical count.
`managed_mutation.rs` = **2**. `capability_permit.rs` is not in the dirty set at
all and still asserts the caller inventory at **13** (`:139`). `publication.rs`,
`protocol/` and `scripts/checks/` are likewise untouched (`git status` on each
returns empty). No companion is owed, and none was added.

---

## 8. Priority 6 — the two Step-3.1b P3 fixes

**[P3-2] is properly fixed, by the stronger of the two offered remedies.**
`write_bootstrap_intent_scratch` now calls `BootstrapIntentRowV1::Scratch.leaf(slots)`
(`namespace/host.rs:290-296`). The variant is constructed; all four
intent-row entry points (`write_bootstrap_intent_scratch`,
`observe_bootstrap_intent_row`, `read_bootstrap_intent_row`,
`bootstrap_intent_row_is_resident`) now name the row by role and take the leaf
from the schedule-derived slots through the selector. The enum's stated rule —
"names rows by *role* and never by leaf" — is now true at **every** call site
rather than two of three. The three `*_leaf()` accessors survive only as the
selector's own arms plus one `retain_scheduled_source` call and the
`interface_tests/namespace_roles.rs` fixture, which is the right residue.

**[P3-3] both comments now state what the code does.**

1. `ownership_token` (`provider.rs:555-563`) now says the derivation is the
   first-generation seed only and that later drives take the token from the
   resident record. I verified that against `execute_row`: `ownership_token(plan)`
   is reached **only** in the `IntentResumeV1::Fresh` arm (`:413-425`), and
   `resume_intent` (`:189-231`) returns `Fresh` only when no generation of the
   row is resident either active or retired. The corrected comment is true, and
   it does not overstate — it explicitly scopes the module header's boundary to
   the read-back as self-consistency rather than exclusion.
2. The fixture ordering sentence is inverted correctly. `RowFixture::new`
   (`tests_provider.rs:392-417`) calls `prepare_prefix` and *then* `plan_for`,
   and the new comment says the declared prefix must exist before the plan is
   made because the preflight walks it. Matches the code.

---

## 9. Priority 7 — the Git-directory arm

**Route (b) chosen; the stated impossibility of route (a) is TRUE.** I read the
Step-2.3 door's fixture: `namespace/tests_managed.rs:120-138` places the
ownership marker with `fs::write(staging.join(name(&managed_marker_name())), …)`
and its own doc comment says so — "(Phase 3's `staging_directory_create` and the
three `ownership_marker_*` boundaries), placed by the fixture". It never calls
`stage_component`, so none of the five writer boundaries is reachable through
that door. The route-(a) option the freeze offered is genuinely dead for *these*
keys, and the implementer says so accurately.

**Route (b) drives the real provider on real Git-directory state.**
`Fixture::target_root(GitDirectory)` is `git2::Repository::open(root).path()` —
the actual Git directory — and `prepare_prefix` creates only the declared prefix
(`.gwz` or `gwz.conf`) there with `fs::create_dir_all`. Everything else is
production: the Git catalog lease (`CatalogLeaseTargetRequestV1::repository_common_git_directory`),
`recover_or_create`, the real admission seam, the real provider, every intent
record. Only the managed parent's *home* is fixture-placed, which is the
follow-up-3 owner decision, and `tests_provider.rs:110-124` documents that at
length rather than hiding it.

**The production refusal is still pinned.**
`a_git_directory_target_refuses_the_workspace_rooted_managed_paths`
(`tests_provider.rs:963-971`) is unchanged and green in the 12/0 provider run: a
Git-directory target with no prepared prefix still refuses the workspace-rooted
managed paths. The purpose-policy and matrix arms do not weaken it — they prepare
the prefix explicitly, which is a different fixture state, not a different
production rule.

---

## 10. Priority 8 — is the liftable §3.5 block fit to lift verbatim?

**Yes, with one one-sentence addition ([P3-4]).** Checked clause by clause.

- **House form**: opens exactly as the Step-2.3 and Step-3.1b annotations do —
  "`managed_bootstrap.*` activation annotation (2026-08-22, Step 3.2 landing;
  discharges …)" — and closes with the same **Counts** paragraph carrying the
  permit inventory and the publication companion. Matches.
- **Arithmetic**: 28 executed / 2 reserved; 23 → 28; 7 → 2; 165 total; no mint.
  All four recounted true (§7).
- **True of the tree**: the five key names, the site file, "the `namespace` owner
  still holds none", the two flush sites, the matrix file, "one repeated-boundary
  row at twelve rounds and four named single-crossing exclusions", the probe's
  mechanism, the retroactive application to 3.1b, `RowShapeV1`, the purpose
  matrix's file and contents, and the publication/permit sentence — each verified
  against code or a recount above. I found **no false statement**.
- **No indicative assertion about undone work.** The block says the two remaining
  keys' "disposition is the Phase 3 settle determination the Step-3.1b review's
  [P3-1] docketed, not this step's" — it claims no disposition it has not made.
  It does reuse the word "reserved" that [P3-1] called a category error, but it
  immediately names the open determination, so the sentence is descriptive of the
  fixture's current state rather than a claim that the category is settled. Not a
  finding.
- **Missing**: which of the two Git-directory routes the freeze offered was taken
  — see **[P3-4]**.

---

## 11. Budget

| Measure | Claimed | Recounted | Verdict |
| --- | --- | --- | --- |
| net test code lines | 357 vs <500 | **357** | within, by 143 |
| `tests_writer_matrix.rs` | 154 lines / 64 code | 154 / 64 | exact |
| `tests_purpose_policy.rs` | 273 lines / 192 code | 273 / 192 | exact |
| `tests_provider.rs` | +254 code | +254 | exact |
| `tests_intent_matrix.rs` | −159 code | −159 | exact |
| fixture (`fault_expected_keys.rs`) | +6 code | +6 | exact |
| production code lines | +40 | **+38** | claim is 2 high, conservatively |

("Code lines" = non-blank, non-`//`.) The +38 splits `managed_mutation.rs` +25,
`namespace/host.rs` +5, `fault_v1.rs` +4, `bootstrap/managed.rs` +4 (the two
`#[cfg(test)] mod` declarations), `provider.rs` +0 (comment only). Roughly 8 of
the 38 are themselves `#[cfg(test)]` scaffolding. Total across all ten files:
+658 raw lines, +395 code lines. Over-reporting one's own production delta by
two lines is the harmless direction; recorded, not filed.

---

## 12. Findings

### [P3-1] `staging_directory_flush`'s second site can never be the interruption point, and the object's shape rationale states the opposite

On any drive that reaches site B (`managed_mutation.rs:518-521`, the managed
parent's flush inside `if created`), site A (`:949-952`, the staged interior's
flush) has already fired: B runs only when `created` is true, and a
freshly-created staging row always has an inexact interior, so
`write_or_rewrite_marker` — and therefore A — always precedes it. An arm on
`ManagedBootstrapStagingDirectoryFlush` therefore **always** lands on A, and B is
announced but never armed-into. After a crash at A the parent flush is skipped
for ever on that row (`created == false` on every later drive), so that
trajectory is also never exercised. It is benign — under a real power loss the
staging row would simply be re-created and converge — but it is untested and
unrecorded.

Worse, one sentence of the object asserts the contrary:
`tests_writer_matrix.rs:44-49` — "A one-component row crosses each boundary
exactly once per drive" — and `tests_provider.rs`'s `RowShapeV1` doc — "The
criterion is written for a boundary crossed once per drive, so the writer matrix
drives a one-component row". Both are false for this one key, which crosses
twice on a creating drive; the key's own `SINGLE_CROSSING_BOUNDARIES` comment
("Both instances of this key …") already contradicts them. The **classification
still holds** — the probe passes for the right reason — so nothing unsound ships.

This is the honest cost of the two-sites-one-key naming, which I rule sound (§3):
minting a sixth key would move the frozen 165 census, and the shape was
specified by the Step-3.1 review's own table before this step existed. Remedy:
qualify the two shape sentences ("each boundary except `staging_directory_flush`,
whose two sites the arm always resolves to the first"), and record in the §3.5
block that the parent flush is announced but not independently interruptible.
Whether it should become interruptible — a site-ordinal-aware arm, or a sixth
key with a census move — is a settle question, not this step's.

### [P3-2] The one-component shape was applied to rows that did not need it; multi-component writer interruption is unexercised

Only `run_single_crossing_probe` and `run_repeated_crashes` depend on the
once-per-drive criterion. `run_boundary_matrix` — plain interrupt/restart/
converge — does not: it would run correctly on a two-component row and would
cover the trajectory the probe proved is *different in kind* (crash at component
0's writer boundary with component 1's crossing still ahead). Today no test
crashes at a writer boundary on a multi-component row: the writer matrix is
one-component only, and the intent matrix crashes only at intent-lifecycle
boundaries. Nothing is lost relative to base — these keys had no rows at all —
so this is coverage not gained rather than coverage removed, and the plan's
clause ("every component/generation/marker boundary") is satisfied per boundary.
Remedy, if wanted: call `run_boundary_matrix` for the writer set under both
shapes, ~5 extra rows per variant, leaving the probe and the repeated row on the
one-component shape. File and continue.

### [P3-3] The §9 refusal is asserted as bare `is_err()`; nothing pins that it is the policy refusal

`merge_archive_is_refused_without_an_existing_merge_store`
(`tests_purpose_policy.rs:170-182`) asserts `run.plan.is_err()` and that nothing
was created. It does not assert *why*. The plan clause it exists to discharge is
specifically the ownership policy, and an `is_err()` can pass for an unrelated
reason (a lease failure, a bad prerequisite) without anyone noticing. Mitigating:
the positive twin is identical except for the prepared prefix and asserts
`retained_existing_parent_count() == 2`, so the differential ties the refusal
causally to the missing merge store; and the pre-existing house pattern in this
owner (`a_git_directory_target_refuses_…`) is likewise bare. Remedy: assert the
refusal's detail string or its `CheckedFsError` discriminant, one line.

### [P3-4] The liftable §3.5 block does not record which Git-directory route was taken

The Step-3.1b annotation posed the choice in the freeze itself — "its
Git-directory arm must drive through the Step-2.3 door
(`retain_managed_parent_at_for_test`) or place its managed prefix under that
target's own retained root". The block that will replace that forward-looking
instruction says only that rows exist "on both target variants" and never says
which route was taken, that route (a) turned out to be unreachable for these
keys, or that the Git-directory managed parent is fixture-placed with follow-up
3 attached. A settle reader working from the freeze alone would not learn it. The
durable in-tree record does exist (`tests_provider.rs:110-124`), which is why
this is P3 and not higher — the same reasoning the Step-3.1b review used for its
own [P3-1]. Remedy: one sentence in the block.

### [P3-5] Two fixture helpers are left with no callers

`Fixture::managed_root` and `Fixture::prepare_managed_root`
(`tests_provider.rs:137`, `:152`) lost their only callers when `IntentFixture`
was replaced by the `RowShapeV1`-parameterised `RowFixture`; `grep -rn
managed_root src/` returns the two definitions and nothing else. rustc's
`dead_code` does not fire on them — I confirmed the clippy gate really does lint
the `#[cfg(test)]` unit (§2), so this is a lint blind spot, not a skipped gate.
Remedy: delete both, or fold `managed_root` into the `RowShapeV1` prefix logic.

---

## 13. Verdict

**GO.** No P0, P1 or P2, so freeze §9's automatic escalation is **not**
triggered and no routing decision is owed.

The step delivers exactly what the plan's Step 3.2 assigns and exactly what the
freeze's deferral record — as superseded at the 3.1b landing — owes: five real
injection sites at the five converted statements, interruption/restart/
convergence rows on both target variants, a twelve-round repeated-boundary row,
the 23 → 28 list edit with 165 held and no key minted, and a purpose policy
matrix that drives all four purposes and both directions of ConsumerCheckpoint
§9 :253-256 through production constructors against real durable state. It stays
out of the `preflight`/`plan_complete` disposition, correctly, and the plan's
text supports that reading independently.

Two things raise it above compliance. The **single-crossing probe** converts a
half of the partition that three prior matrices asserted only in prose into a
machine check, applies it retroactively to Step 3.1b, and its first act was to
fail on a classification that had been carried unchallenged — the reviewable
form of a self-correction. And **`RowShapeV1`** turns row shape from an
accident of each fixture into a declared property with its reason attached,
which is the durable half of that lesson.

The five P3s are all record-or-coverage refinements; none touches soundness, and
[P3-1] is the honest residue of a naming decision the authorities made before
this step, not a defect this step introduced.

| Finding | Grade | Disposition |
| --- | --- | --- |
| [P3-1] `staging_directory_flush`'s second site is never the interruption point; two shape sentences say otherwise | P3 | qualify the two sentences + one clause in the §3.5 block; interruptibility is a settle question |
| [P3-2] one-component shape applied to rows that did not need it; multi-component writer interruption unexercised | P3 | file and continue; optional ~5 rows/variant |
| [P3-3] the §9 refusal is pinned only as `is_err()` | P3 | file and continue; one-line detail assertion |
| [P3-4] the liftable §3.5 block omits the Git-directory route taken | P3 | one sentence, lane owner's, in the same edit that lands the block |
| [P3-5] two fixture helpers left with no callers | P3 | file and continue |

Standing items, recorded not filed: `preflight`/`plan_complete` remain the one
open activation item and are the Phase 3 settle's per the Step-3.1b review's
[P3-1]; `retain_managed_parent`'s production visibility remains the standing
item from Step 3.1. Neither moved, and neither should have.

---

## 14. Verification appendix — commands executed for this review

Gates were run **in the live tree** (`/Users/owebeeone/limbo/gwz-dev/gwz-core`),
which the dirty-set check licensed: the object is the only uncommitted work in
either repo. `CARGO_TARGET_DIR` = `…/scratchpad/p31-target` on every cargo call.

```
# the object is the whole dirty set — the precondition for gating in place
(gwz-core) git rev-parse HEAD                       fcec69eb79afaede…
(gwz-core) git status --porcelain                   8 " M" + 2 "??" = the 10 files
(gwz-dev)  git status --porcelain                   empty
(gwz-dev)  git rev-parse HEAD                       0cf4e4df95287de9…

# checker attribution, constructed rather than assumed
git archive fcec69e src/checked_artifact | tar -x -C <scratch>
python3.13 -c "<load the checker; run source_tree_digest over both trees>"
  pre_catalog.rs   pin=05e4d248… base=05e4d248… live=3989a177…   base_ok=True live_ok=False
  the other three checked_artifact trees:               base==live==pin
(gwz-core) python3.13 scripts/checks/check_checked_artifact_boundaries.py
  - protected source tree changed: checked_artifact/capability/pre_catalog.rs
  EXIT=1                                    [one finding, this step's, alone]

# clippy really recompiled, and really linted the cfg(test) unit
cargo clean -p gwz-core
CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -v -- -D warnings
  EXIT=0 | "--crate-name gwz_core" invocations: 2 | of those carrying --test: 1
  lines matching ^warning: 1  (cargo's non-root-profile notice)

cargo check --all-targets                                    Finished [clean]
cargo test --lib checked_artifact::           360 passed; 0 failed
  …::tests_writer_matrix -- --nocapture         4 passed; 0 failed
  …::tests_purpose_policy                      10 passed; 0 failed
  …::tests_intent_matrix                        4 passed; 0 failed
  …::tests_provider                            12 passed; 0 failed
  …::interface_tests::fault_expected_keys       6 passed; 0 failed
  …::interface_tests::capability_permit         8 passed; 0 failed
rustfmt --edition 2024 --check <the 10 files>                RUSTFMT_EXIT=0

# counts and pins, recounted not assumed
grep -c 'fault_v1::hit' managed_mutation.rs            20   (base 14, +6 calls / 5 keys)
grep -c 'fault_v1::hit' namespace/host.rs               0
grep -rn 'CheckedArtifactFaultKeyV1' namespace/*.rs     0 outside tests* files
grep -c 'managed_bootstrap\.' fault_v1.rs              30
grep -c 'create_dir(' managed_mutation.rs               1
per-variant site scan under capability/:
  StagingDirectoryCreate 1 | OwnershipMarker{Create,Write,Flush} 1 each
  StagingDirectoryFlush  2                              [the two-sites-one-key shape]
  ManagedBootstrap{Preflight,PlanComplete}: 0 references outside fault_v1.rs
grep -rn 'publish_verified_no_replace(' src/     base 21 | live 21
  managed_mutation.rs 2 | capability_permit.rs :139 asserts 13
grep -rn 'retain_managed_parent_at_for_test' src/  → namespace/tests_managed.rs only
git status --porcelain -- capability_permit.rs publication.rs protocol scripts/checks
                                                        empty [all untouched]
EXPECTED_KEY_COUNT                                     165, byte-unchanged
MANAGED_BOOTSTRAP_EXECUTED_KEYS.len() asserted at       28
#[test] counts base→live: provider 12→12, intent 4→4, fixture 6→6,
  writer 0→4, purpose 0→10                              [nothing deleted; 346+14=360]

# LOC, recounted (non-blank, non-//)
per-file code deltas: writer +64, purpose +192, provider +254, intent −159,
  fixture +6  →  test total +357 (<500)
  managed_mutation +25, host +5, fault_v1 +4, managed.rs +4, provider.rs +0 → +38
```

No git write operation was performed at any point; every git call was
`show`/`archive`/`status`/`log`/`rev-parse`/`diff`. `cargo clean -p gwz-core`
touched only the scratch target directory. The only repo-adjacent file written
by this review is this document.
