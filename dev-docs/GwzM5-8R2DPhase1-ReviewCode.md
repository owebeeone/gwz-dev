# R2-D Phase 1 settled review — CODE axis

Date: 2026-08-22
Axis: **CODE** (adversarial; dual #2 of the program's three retained R2-D
duals; round 1 of the two-round cap)
Object: the complete Phase 1 package on gwz-core — commits **`5a7ff0f`**
(Steps 1.1+1.2: six acceptance tests + admission driver + C-2 arms + C-3
grammar) and **`c13f773`** (Step 1.3: fault activation) over base
**`c40e712`**, reviewed as
`git -C gwz-core diff c40e712 c13f773 -- src/checked_artifact scripts/checks/check_checked_artifact_boundaries.py`
(22 files, +2,563/−91).
Peer-blind: `GwzM5-8R2DPhase1-ReviewState.md` was not read or consulted.
Reviewer: Claude (Fable 5), CODE-axis settle reviewer.

In-range third-party commits, attributed and handled: `3e60529`/`8c1624a`/
`558f834` are the M5b train (workspace_ops-side; their only touch inside this
surface is the checker digest-pin churn swept in and corrected by `558f834` —
the range diff's `workspace_ops/merge/v1_lifecycle/mod.rs` pin line is that
train's, not Phase 1's); `7a97f56` (CRLF normalization of two `include_str!`
pins in `r2d_seam_freeze.rs:221-224`) is inside the surface and was reviewed:
content-only, correct, improves Windows-runner determinism. Per the record,
`5a7ff0f` reached origin early beneath a hotfix push; this review judges the
committed object as published. All committed state was read via
`git show c13f773:<path>`; the working tree carries another lane's uncommitted
`workspace_ops` files and was not used as evidence.

---

## 1. Gates executed on a pristine extraction of `c13f773`

Method: `git archive c13f773 | tar -x` into a fresh directory (no worktree, no
git mutation), toolchain `1.95.0` per `rust-toolchain.toml`, isolated
`CARGO_TARGET_DIR`.

| Gate | Result |
| --- | --- |
| `scripts/checks/check_checked_artifact_boundaries.py` (Python 3.12) | **ok** — "checked-artifact boundary: ok (15 visible entries, 5 classified modules)". Digest pins coherent at `c13f773` on pristine bytes; `CATALOG_PUBLICATION_CALL_COUNTS` (1/5/**2**) enforced green. |
| `cargo test --lib 'checked_artifact::'` | **271 passed; 0 failed; 0 ignored** (15.4 s) — exactly the parked claim, both fault-matrix variants in-suite. |
| `rustfmt --edition 2024 --check` scoped to all 21 changed `.rs` files | clean |
| `cargo test --lib 'r2d_seam_freeze'` | **6 passed; 0 failed** — all five seam pins plus the spike-is-test-only pin hold. |
| `cargo test --lib 'tests_admission_spike'` | **2 passed; 0 failed** — Track-P spike still green at `c13f773`. |
| `cargo clippy --lib --all-features` and `--lib --all-targets --all-features` | **clean with zero allowances**. The `-A clippy::nonminimal_bool` dispensation was not needed: the known `cursor.rs` finding lives only in the other lane's uncommitted working tree, not in the committed object. My allowance usage is therefore exactly none — strictly inside the granted bound. |

---

## 2. Mandate point 1 — nine-step sequence (driver.rs + admission_mutation.rs vs ConsumerCheckpoint §7 :203-224)

**Ordering.** `admission/driver.rs` is a pure decision loop (`resolve` over the
active/scratch slot-state pair, then `classify_handoff` for the staging steps);
`admission_mutation.rs` executes exactly one bounded durable edge per
iteration. Traced against §7: steps 1-2 and 8 share the read-only
`observe()` path (no mutation — pinned by acceptance test 3's
pre-assertions); step 3 = `WriteAdmissionScratch(preparing)` →
`RetireAdmissionRecord` (only when an active record is resident) →
`PublishAdmissionRecord`; step 4 = `CreateStagingDirectory` (`create_dir`,
itself no-replace); step 5 = `WriteResidentReservation` (inside staging,
deterministic `ActionSlotV1::Base(Reservation)` name); step 6 =
`PublishStagingAction` (sealed no-replace publish onto
`RootEntryNameV1::ActiveAction(digest)`); step 7 = the same
write-ahead/retire/publish triple with `idle()`; step 9 = `owner.admit` —
handoff only from `is_idle` + `staging Missing` + `exact_identity_for`
(exact reservation, `extra_children == 0`). The resolve matrix's residual arms
stop with typed ambiguity; a `Preparing` record for a different action
classifies `Other` and stops (driver.rs:161-170). The conflict stop of §7
:205-207 is explicit at driver.rs:114-118.

**Durability points.** `write_durable_record` (admission_mutation.rs:395) is
create → hit → `set_len(0)`/`seek`/`write_all` → hit → `sync_all` →
`verify_open_file` → `verify_named_file` (fresh open by name, identity+bytes)
→ parent `sync_directory_edge` → hit — the P2 family shape, reusing the
catalog owner's own `durable_write_options` (widened `fn` → `pub(super)` only,
inside the same provider). Both publishes reobserve after the rename (E5) and
flush the parent. `create_staging_directory` reopens no-follow and re-proves
identity before the parent flush.

**No mutation before capacity.** The driver takes the complete
coordinator-derived reservation as input; the first durable edge is step 3,
after the full observation + census gate. Acceptance test 6
(`capacity_covers_every_slot_family_before_the_first_action_mutation`) proves
all five §7 :223-224 families are non-trivially present in the resident
reservation and that no reserved slot exists in the published action directory.

**Deterministic indexed names / no nonce.** Grep of every added line in the
range for `rand|random|nonce|SystemTime|Instant|timestamp|uuid|thread_rng|
getrandom` finds **zero production hits** — the only matches are doc-comment
citations of the stop clause and `std::process::id()` in the three
**test-fixture** temp-dir names (the accepted `catalog/bootstrap/tests.rs:19-44`
pattern). Every production name is one of the three frozen `ActionAdmission*`
slot names, the derived `action-<hex>-v1` row, or the digest-indexed
reservation slot. The driver's stop-clause citation
(`GwzM5-8R4bP1P2-RemPlan-4.md` §4 R2 stop clause :1089-1092, "allocates a
fresh retry scratch name") was verified against the cited file at those exact
lines. Acceptance test 5 byte-compares the full catalog child listing across
three retries and grammar-validates every name. **Satisfied.**

---

## 3. Mandate point 2 — C-2 arms

**Shapes match the freeze exactly.** `DirectoryInteriorRecheckV1.expected`
generalizes from `&CatalogBootstrapRecordV1` to the new two-arm
`DirectoryInteriorExpectationV1` (`CatalogStaging` | `AdmissionStaging`) —
the freeze's "generalization of that struct or its field type", not a variant
bolted onto a struct. `DestinationRecheckV1` gains the genuine new variant
`AdmissionCatalogInterior { durable_identity, expected, absent }`. Both are
lifetime-parameterized reference holders with no encode path — in-memory only.

**`publish_verified_no_replace` core byte-untouched — proven by whole-function
diff** (102 → 133 lines, extracted from both commits and diffed): the only
changes are (a) the source-interior recheck block now matches on the
expectation enum, with the `CatalogStaging` arm preserving the pre-existing
observe → `staging_plan == Complete` logic and error string token-for-token,
and (b) one new destination-recheck match arm. Source acquisition, the
acquisition window, identity compare, retained source handle, and the
`rename_open_source` no-replace rename are untouched bytes.

**Verification lives in `interior.rs` within the same owner** —
`observe_action_directory` / `observe_action_interior` / `observe_reservation`
(bounded: `MAX_ACTION_SLOTS` cap, `Capacity.max_bytes()` bounded read,
`try_reserve_exact` before allocation, no-follow opens, §7 :220-221 exactness
predicate `is_exact`). **No platform file touched**: the range diff for
`src/checked_artifact/platform*`, `namespace/`, `protocol/bounds.rs`, and
`protocol/generated.rs` is empty. The destination arm additionally restates
no-replace as an in-window expectation (`absent` row must not be occupied) —
a strictly stronger recheck on newly-introduced edges only.

**Caller pin extended deliberately, same commit**: six-site pin → eight at
`interface_tests/capability_permit.rs` (the two admission publishes, E3 + E4)
and `CATALOG_PUBLICATION_CALL_COUNTS` gains
`admission_mutation.rs: 2`, with the freeze-citing comment; digest pins
refreshed in the same commit and green on pristine. The E3/E4 edge labels in
both comments match freeze §4.3's numbering exactly. **Satisfied** (one
inventory deviation on E4's retire half — finding [P2-1] below).

---

## 4. Mandate point 3 — C-3 grammar, widening-only argument, file by file

- **`protocol/slots.rs` (+49)**: adds only `CatalogRootRowClassV1`, a
  classifier **over** the frozen `RootEntryNameV1` grammar and frozen
  `InfrastructureSlotV1` vocabulary — no new slot, record, or name; the §3.1
  persisted-home pin holds (`InfrastructureSlotV1` enum, `ALL`, and the name
  table untouched). First production caller of `RootEntryNameV1::parse`, as
  the freeze predicted.
- **`interior.rs` `exact_slot` → `exact_row`**: the ten-slot alias refusal
  ("resolves to a platform alias of an owned catalog name") and the
  unowned-child refusal ("catalog directory contains an unowned child") are
  **byte-identical**; the only newly-admitted class is
  `ActiveAction` (ASCII-only by grammar — `native_ascii_bytes` sends any
  non-ASCII child to the unchanged refusal; malformed-recognized spellings
  such as `action-00-v1` still refuse as unowned children).
- **`observe()` bound**: `MAX_INTERIOR_ENTRIES` moves 10 →
  `MAX_ROOT_ENTRIES` (= 74 = 10 + 64, all three constants pre-frozen in the
  untouched `protocol/bounds.rs`); the infrastructure sub-bound keeps the
  byte-identical "catalog interior exceeds the ten-slot bound" message; action
  rows are separately capped at `MAX_ACTIVE_ACTION_DIRS` and duplicate-checked
  after a deterministic sort. Every previously-accepted interior (≤10
  infrastructure rows, no aliens) is accepted identically; every previously
  refused interior remains refused — for over-bound interiors the *message*
  can now be the licensed new "frozen root-entry bound" / unowned-child
  refusal instead of the old ten-slot message, which is inherent to the
  freeze's own "the cap and the grammar have to move together" license and
  changes no acceptance. `bounds_slots.rs` pins the partition
  (74 = 10 + 64, `MAX_INFRASTRUCTURE_ENTRIES == ALL.len()`).
- **`staging_plan`**: the only change is removing the three `ActionAdmission*`
  slots from the un-complete gate; `CatalogBootstrapRetired` keeps its refusal
  and the remaining plan logic is untouched — refusal-removal is the exact
  fact-2 license.
- **`completed_record`**: same shape — `CatalogAnchorB` keeps its refusal; the
  five completeness checks (empty retired root, exact roaming/anchor/
  descriptor/format) are untouched bytes. This is precisely the widening the
  freeze's "Consequence" paragraph demands so §7 step 8's reobservation
  survives a resident admission.
- **`snapshot.rs`**: action rows join the fresh-observation digest
  (count-framed, sorted, fixed-width) so same-length action-row substitution is
  caught like infrastructure substitution. Verified **in-memory only** —
  `collision_snapshot_digest` is compared at revalidation
  (`filesystem.rs:290`) and never serialized; no wire impact.
- **Alias/unowned byte-identity claim of the checkpoint: verified true.**
  Newly admitted rows are exactly the action/admission rows. **Satisfied.**

---

## 5. Mandate point 4 — the three judgment calls

**(i) Visibility widening `pub(super)` → `pub(in crate::checked_artifact)`
(`protocol/admission/owner.rs`: `ObservedActionDirectoryV1{,::exact}`,
`CatalogAdmissionOwnerV1{,::new,::classify_handoff,::admit}`, plus the
module-level `pub(in crate::checked_artifact) use owner::*`). RATIFIED.**
The frozen Seam-1 signature and its three structural properties are untouched;
`r2d_seam_freeze` is green on pristine, including the strict three-item
surface pin over `admission/mod.rs` with its forbidden-authority token scan.
A full grep of added lines finds **no** `pub(crate)`/bare-`pub` anywhere in
the range — exposure is exactly as claimed. `AdmittedActionV1`'s fields stay
module-private, so the handoff type still cannot be forged outside
`protocol/admission`; what widened is the ability of checked_artifact-internal
code to *call* the classifier, which is the freeze's own "the physical driver
is the missing half of that classifier" design. The one real cost — a sibling
module could feed `admit()` synthetic observations — is a same-crate
discipline question already policed by the checker's module rules and the
seam pins, and the classifier bodies are bit-identical (only signatures and
the doc comment changed). Note the replaced owner.rs doc comment deliberately
retires the old "neither raw observations nor this issuer can be named by a
checked-artifact sibling" promise; the replacement text states the new
boundary accurately.

**(ii) E4 as retire-then-publish over P1 no-replace. RATIFIED — I do not read
E4 as requiring an atomic replacing rename.** §7 steps 3/7 say "persist", not
"replace atomically"; a replacing rename would contradict the family's
no-replace posture; and with `completed_record` requiring an **empty**
`RetiredActions` root, no in-vocabulary rename target for a superseded
admission record exists without minting a name. The convergence test does
what it claims: `every_admission_record_install_window_converges_to_the_settled_state`
(`driver/tests.rs:166`) reconstructs the windows **on disk** with raw
`fs::remove_file`/`fs::write` of the two slot files — "retired, scratch idle"
(post-retire/pre-publish), "preparing, scratch idle" (pre-retire), and "triad
absent" — and asserts convergence to the identical settled child-set and the
identical handoff, with the scratch cleared. Restart-closure of the same
windows under real interruption is separately proven by the matrix (all four
scratch-publish/reobserve keys crash-and-restart green on both variants).
One inventory defect in how the retire half is *recorded* is [P2-1] below;
one coverage nicety is folded into its remediation.

**(iii) Absent-active ≡ idle, zero-write. RATIFIED with direct evidence.**
The "triad absent" window row asserts the driver performs **no mutation at
all**: the resumed catalog equals the settled set *minus* the active-record
row (`settled_without_record`), the scratch stays absent, and the handoff is
identical. Code-side, `(Missing, Missing)` resolves to the idle drive and
`admit` succeeds against the exact published row without executing any edge —
zero durable writes, verified by child-set equality. The semantic question
(whether an absent active record *is* `idle()`) is the State axis's to rule;
as implemented, the record carries no field beyond "not preparing", and the
zero-write claim is test-proven.

---

## 6. Mandate point 5 — Step 1.3 substance

- **Idiom.** `fault_v1.rs` is byte-untouched across the range and its 19
  admission keys pre-existed at `c40e712` (verified: `git show c40e712` lists
  all 19) — **no key minted**. Sites announce with `hit(key)` immediately
  **after** the operation the key names: create after the create-open, write
  after `write_all` (before `sync_all`), flush after the parent
  `sync_directory_edge`, publish after `publish_verified_no_replace`,
  reobserve after the post-publish verification — one restart-visible
  boundary per key, per the `fault_v1.rs:3-5` contract. Spot-verified on all
  eleven physical `hit` sites.
- **The `AdmissionRecordRowV1` threading.** The row enum (Preparing | Idle |
  Reservation) selects both the stable fault triple **and** the diagnostic
  fact; `fact()` returns exactly the strings the call sites passed literally
  before ("admission scratch", "resident reservation"), so production
  behavior is byte-identical — and the row is derived by the same
  `== idle()` equality the driver resolves on, no new predicate.
  `install_faults` reads the row from the scratch bytes the edge is about to
  publish — the same durable fact the edge was resolved from; production
  code path unchanged (`#[cfg(test)]` only).
- **The matrix actually interrupts and restarts.** Per key:
  `run_next_at(key, || panic!(...))`, `catch_unwind` around a **full** attempt
  (fresh lease → sealed recovery → drive), an explicit
  "fault point was not reached" assertion proving the boundary fired, then a
  fresh-process settle with convergence to a grammar-derived baseline,
  scratch-clearance, and a **second** settle proving the same handoff identity
  and no further mutation. Both target variants run it (workspace and
  Git-common-directory via the real lease path). `reconcile_executed_keys()`
  reconciles the 19-row matrix against `Fault::all()` filtered to
  `admission.` — a 20th key without a matrix row fails the suite.
- **The two 12-round boundaries are genuinely re-crossable — verified in
  code.** `admission.preparing_scratch_create` crashes after the create-open,
  leaving an empty scratch that decodes `Other` and routes back through the
  same `WriteAdmissionScratch` edge to the same key; `admission.staging_flush`
  crashes after the staging flush, leaving an exact staging that routes back
  through `PublishStagingAction` to the same key. Twelve rounds > the
  eight-edge/eight-row nominal capacity; the census asserts an identical
  durable slot set every round and staging holding only the derived
  reservation row. **The write-boundary non-repeatability reasoning is
  code-true**: a crash after `write_all` leaves complete decodable bytes, so
  the restart resolves past the write edge (`(_, Preparing)` →
  install; reservation `Exact` → publish) and the key cannot re-fire. The
  comment's *exclusivity* claim, however, is overbroad — finding [P3-1].

---

## 7. Mandate point 6 — inventories

- **Counts held 19/165**: `fault_v1.rs` untouched; the activation table row
  flips `Reserved` → `Executed("R2-D phase 1 step 1.3 (R2-C3 admission)")`
  with the count still 19; `EXPECTED_KEY_COUNT` untouched at 165; the
  executed-family set test now pins exactly
  `{admission, catalog_bootstrap, runtime}`.
- **`FAULT_INJECTION_SOURCES` 3 → 4** with `admission_mutation.rs` declared;
  the completeness anchor
  (`the_declared_injection_sources_are_every_production_source_holding_sites`)
  is untouched and still rescans the production tree — it verified the fourth
  source "the designed way" (and the driver holds zero sites: `driver.rs`
  never names `CheckedArtifactFaultKeyV1`). The freeze memo carries the dated
  activation update naming the fourth source (preamble :36-41) and the §3.5
  row records the executed state.
- **`CATALOG_PUBLICATION_CALL_COUNTS` admission entry = 2: true** — exactly
  two `publish_verified_no_replace(` calls in `admission_mutation.rs`
  (record install :163ff, staging publish :252ff), enforced by both the
  checker (green) and the eight-site pin test.
- **capability_permit 6 → 8: accurate** (1 + 5 + 2, test green).
- **Digest pins coherent at `c13f773`**: checker green on the pristine
  extraction; the `558f834` incident's corrective ritual held — the driver
  lane re-applied its counts entry inside its own package as instructed.

---

## 8. Mandate point 7 — gates

See §1. All green on pristine `c13f773`; 271/0 exactly as parked; clippy
clean with zero allowances (the granted `nonminimal_bool` dispensation was
unnecessary on the committed object and was not used).

LOC disposition, independently recomputed: production (non-test-file)
additions across the range ≈ 1,239 raw / ≈ 930 non-comment-non-blank lines
(vs the parked "~768 code, +1,051 raw" at `5a7ff0f` plus Step 1.3's +114 —
consistent order; my count includes `#[cfg(test)]` fault-site lines inside the
production files). The overage vs the aspirational <500 is real, disclosed,
and structurally attributable on the diff itself: `interior.rs` +284,
`publication.rs` +79, and the C-2/C-3 halves of `admission_mutation.rs` carry
the two extension classes the round-2 freeze assigned to Phase 1 after the
plan budget was set. No ChangeBudget ceiling exists for R2-D; the global rules
make <500 a target, not a limit. No finding.

---

## 9. Findings

### [P0] — none.

### [P1] — none.

### [P2-1] E4's retire half executes an un-inventoried physical primitive; the freeze's edge inventory and the plan's publication-seam rule do not record it

`capability/pre_catalog/provider/admission_mutation.rs:153-160`:

```rust
fn retire_admission_record(final_directory: &Dir) -> Result<(), CheckedFsError> {
    final_directory
        .remove_file(OsStr::new(
            InfrastructureSlotV1::ActionAdmissionActive.name(),
        ))
```

Freeze §4.1 admits five primitive families (P1 sealed rename-publication, P2
write-through+flush, P3 identity, P4 bounded enumeration, P5 barrier) — none
covers unlink. Freeze §4.3's E4 row names "P2 + P1" only, and plan §4 Step 1.2
says "all namespace edges through the sealed publication primitive"; removing
a name from the catalog root is a namespace edge, and this one is a bare
`remove_file` + parent flush. The §4.2 spike never exercised an unlink on
either platform, and no `admission.*` key sits on this boundary (the frozen
19 predate the shape; correctly, none was minted).

Why this is P2 and not P1: the shape itself is **ratified** under judgment
call (ii) — the write-ahead scratch is durable and verified before the
remove; both crash interleavings re-resolve convergently
(`(Idle|Preparing, opposite)` → retire again; `(Missing, Exact)` → install),
proven on disk by the install-window test and under interruption by the
matrix; the operation is idempotent so its durability is not
correctness-critical (ConsumerCheckpoint §8 :242-243 is not violated on
Windows — a resurrected entry only causes one more retire round, and the
whole lane's Windows durability closure is deferred to Phase 4.2 by the
frozen activation map anyway); `remove_file` is not a *new platform
capability* in the Track-P sense (it is already production-used at
`cleanup.rs:87/:127/:157`, `platform.rs:408`, `transition.rs:234`); and with
`completed_record` requiring an **empty** `RetiredActions` root, no
in-vocabulary rename target exists for a superseded admission record — the
alternatives are minting a name (freeze-forbidden) or a replacing rename
(contrary to the family). The defect is that the binding inventory now
understates the kernel's physical surface.

**Minimal remediation (doc + one test row, no production change):**
(a) append an activation-record-style note to freeze §4.3's E4 row (and/or
§4.1) in the acceptance train, naming the retire half's physical shape
(`cap_std::fs::Dir::remove_file` of the frozen active-record name + P2 parent
flush), its idempotence/convergence evidence, and the empty-retired-root
reason no admitted family could express it; (b) extend
`every_admission_record_install_window_converges_to_the_settled_state`
(`driver/tests.rs:166`) with the preparing-variant post-retire window
(active absent, scratch = `preparing`, action not yet published) so both
record kinds' post-retire windows are test-instantiated rather than the idle
one plus structural argument (~12 lines).

### [P3-1] The repeated-crash comment's exclusivity claim is not code-true

`admission/tests_fault_matrix.rs:323-329`:

```
/// A boundary is repeatable only when the durable state its crash leaves
/// resolves back to the same edge; most `admission.*` boundaries are crossed
/// once ... The two
/// crossed here are the repeatable ones, and they are also the two rows a
/// retry could be tempted to re-name:
```

By the same code logic the comment states, at least four other boundaries are
genuinely repeatable: `admission.reservation_create` (crash after the
create-open leaves an empty reservation row; `classify_expected_prefix(&[],
expected)` is `PartialExpectedPrefix` — `codec.rs:198-204` — so
`has_rewritable_reservation` routes every restart back through
`write_durable_record` to the same key; a slot-bearing row a retry could be
"tempted to re-name", exactly the property the chosen pair is justified by),
`admission.idle_scratch_create` (empty scratch decodes `Other`;
`(Preparing, Other)` → `ReplacePreparingWithIdle` re-enters the same helper),
and trivially `admission.occupancy_observe` / `admission.capacity_check`.
The chosen two remain valid §12 evidence (both genuinely repeatable, 12 > the
8-edge/8-row nominal, slots stable, both variants); the *selection* is fine,
the *exclusivity sentence* is wrong, and the checkpoint's "the two genuinely
re-crossable boundaries" phrasing inherits it.

**Minimal remediation:** reword the sentence to "two of the repeatable
boundaries, chosen because both leave a resident row a retry could be tempted
to re-name" (or equivalent); optionally add `admission.reservation_create` as
a third 12-round row (~6 lines) since it is the other slot-bearing repeatable
boundary.

### [P3-2] Dead `#[allow(unused_imports)]` with a stale reason on the classifier re-export

`protocol/admission.rs:18-22`:

```rust
#[allow(
    unused_imports,
    reason = "R1 exports the admission classifier before its R2-D physical half consumes it"
)]
pub(in crate::checked_artifact) use owner::*;
```

The reason is false as committed — the physical half consumes every re-exported
item in the same commit (`driver.rs` uses `CatalogAdmissionOwnerV1`;
`interior.rs` uses `ObservedActionDirectoryV1` in production). Proven dead: with
the attribute removed on the pristine extraction, both `cargo check --lib` and
`cargo check --lib --profile test` compile with zero warnings.

**Minimal remediation:** delete the attribute (or convert to `#[expect]`,
which would have flagged the staleness itself).

---

## 10. Verdict

Zero P0, zero P1; one P2 (freeze-inventory append + one test row, dischargeable
in the acceptance train); two P3. All seven mandate points verified; all gates
green on the pristine committed object; all three queued judgment calls
ratified on code evidence.

**VERDICT: GO** (0 × P0, 0 × P1, 1 × P2, 2 × P3).

---

# Focused re-verdict (round 2)

Date: 2026-08-22
Axis: **CODE** (round 2 of 2 — focused confirmation pass)
Object: gwz-core **`bf438ed`** ("Apply the Phase 1 settle round-2
remediation"; local, UNPUSHED; base `c13f773` + remediation files only —
10 files, +535/−29), verified against the **committed** revision via
`git show bf438ed:<path>` and a fresh pristine `git archive` extraction; the
working tree's four other lanes' uncommitted files were not used as evidence.
Peer-blind vs `GwzM5-8R2DPhase1-ReviewState.md` maintained.

## Gates re-executed on pristine `bf438ed`

| Gate | Result |
| --- | --- |
| boundary checker (Python 3.12) | **ok** — the `pre_catalog.rs` tree digest is applied in-commit (`446cb7…` → `d7d5c4…`); pins coherent on pristine bytes. |
| `cargo test --lib 'checked_artifact::'` | **274 passed; 0 failed** (271 + the 2 new capacity tests + the namer pin). |
| `rustfmt --edition 2024 --check`, all 9 changed `.rs` files | clean |
| `cargo clippy --lib --all-targets --all-features` | **clean, zero allowances** (matches the remediator's record). |
| `r2d_seam_freeze` / `tests_admission_spike` | 6/6 and 2/2 — no seam regression. |

Two for-the-record notes, neither a code defect: **(a)** the remediator's
recorded "302/0 focused suite" is not reproducible against the committed
object — the anchored filter yields **274/0** and the unanchored form
**278/0**; the suite is green either way, so this is an evidence-recording
discrepancy only (most plausibly a dirty-tree or transcription artifact), and
the number that stands for the settle is my pristine 274/0. **(b)** a
supplementary full-`--lib` run (my own belt-and-braces, not a mandated gate —
every changed file lies inside the focused scope) was still executing when
this section was appended, dominated by the known >585 s pre-existing
`workspace_ops` matrix test; its result does not gate this verdict and will
be reported to the coordinator when it lands.

## Per-finding disposition

**[P2-1] — RESOLVED, beyond prescription.** (a) The dated activation record
is appended to freeze §4.3 (:620-672) in the §3.5 activation-record form,
explicitly filed on "Code [P2-1] ≡ State [P2-1]" (the two axes converged
blind on the same defect): it names the E4-retire physical shape
(`cap_std::fs::Dir::remove_file` of the frozen `ActionAdmissionActive` name +
P2 parent flush, at `retire_admission_record`), states why no admitted family
covers it and why every rename alternative was structurally unavailable
(no-replace family + empty-`RetiredActions` completeness + no-minting), and
records write-ahead protection, idempotence (including the Windows
delete-pending degradation argument and the §8 :242-243 non-violation),
namespace-enforced ordering, and the convergence evidence. It also corrects
the checkpoint's "retire-then-publish over the no-replace primitive"
shorthand. (b) The prescribed preparing-variant post-retire window landed in
`driver/tests.rs` ("post-retire, scratch preparing", fresh reservation,
action row not yet published, convergence to an exactly-predicted child set),
**plus** two rows I did not demand: step 3's pre-retire window
`(Idle, preparing)` reconstructed directly, and a torn-scratch window
(truncated canonical bytes — non-empty proper prefix decodes `Other` by the
self-digest argument, and step 3 truncate-rewrites the same compile-time slot
name). Both post-retire windows of both record kinds are now on-disk
evidence, not structural argument.

**[P3-1] — RESOLVED as prescribed, with the optional extension taken.** The
exclusivity sentence is gone; the rewritten comment
(`tests_fault_matrix.rs:321-340`) states the write boundaries' true
non-repeatability mechanism, names the selection principle (resident row a
retry could be tempted to re-name), **names the boundaries left out** —
`idle_scratch_create` (repeatable, but re-crosses the slot name
`preparing_scratch_create` already proves stable) and the two trivially
re-crossable observation boundaries — and is code-true on every claim I
checked. `Fault::AdmissionReservationCreate` joins the 12-round set (now
three boundaries × 12 rounds × both variants); the loop's census assertions
hold for the new row (round-stable staging holding exactly the derived
reservation row).

**[P3-2] — RESOLVED.** The dead `#[allow(unused_imports)]` and its stale
reason are deleted; the bare re-export stands
(`protocol/admission.rs:18`), and the zero-warning clippy gate on pristine
`bf438ed` re-proves what my round-1 removal probe showed.

## Ruling on the new capacity code (State-axis [P1] fix, Code-axis sanity)

The two-sided capacity refusal is correct and minimal. **Driver half**
(`driver.rs:82-115`): the gate sits in the `Idle` arm after `owner.admit` and
after the conflict stop, so resume of an exact existing action is untouched
and the refusal fires before the first durable write; the comparison
`census.active_actions >= MAX_ACTIVE_ACTION_DIRS` is off-by-one-correct (the
64th action admits at count 63; the 65th refuses at 64, and at this point the
count cannot include this action's own row — an exact row already returned
via `admit`, a non-exact one already stopped as a conflict). The accompanying
unreachability comment on `has_unowned_row` is accurate (`exact_row` refuses
malformed/foreign before the census can charge one) and keeping the stop as
defence-in-depth is reasonable; the "owed to Phase 4"
`CatalogOccupancyV1::can_admit_new` citation is real (`bounds.rs:74/:144`)
and the deferral is sound — no retirement edge exists in Phase 1, and unlike
the active bound, retirement credit cannot make the catalog unobservable.
**Commit-point half** (`publication.rs:219-240`): the bound is re-proved
inside the acquisition window from the same single destination observation,
gating **only** `ActiveAction` destinations (`full = false` for the
infrastructure arm — correctly, or a full catalog could never finish its own
step-7 record install), with a distinct typed refusal
("publication would exceed the frozen active-action bound") before the
untouched compound refusal. A fresh whole-function extraction diff of
`publish_verified_no_replace` (`c13f773` → `bf438ed`) confirms the
acquisition window, identity compare, retained handle, no-replace rename, and
both other recheck arms are byte-identical; the only change is the
`(occupied, full)` tuple and the guarded early return. Both halves carry
exact-match typed-refusal tests (`stopped_with`), including the driver-bypass
path (in-flight admission rewound on disk, root filled, refusal at the
rename, staging preserved for retry, zero mutation). The `interior.rs`
changes are the `staging_plan` triad **restore** (byte-identical to the R1
four-slot list — code-side this narrows round 1's C-3 widening to exactly the
predicate the §4.4 "Consequence" chain needs, `completed_record`; no Phase-1
flow consumed the dropped half, and it closes the stray-record adoption
hazard my round-1 report noted as unreachable-but-real) plus the documented
no-panic invariant over `infrastructure_slot().expect(..)`. The new
`tests_namer_pin.rs` (a State-axis P3 remediation inside my surface) is a
sound rescan-idiom pin on judgment call (i)'s accepted cost: it rescans
production sources for the two widened classifier names against a declared
five-module set (scan and exclusion logic checked; the declared set matches
the tree; the file excludes itself by the `tests` prefix), with the
lane-owner relocation note to `interface_tests/` recorded. No new findings.

## Verdict

[P2-1] resolved; [P3-1] resolved; [P3-2] resolved; the new capacity code is
correct, placed right, race-closed at the commit point, and fully typed-refusal
tested; all mandated gates green on the pristine committed object; no new
findings at any severity. Unresolved items: none.

**ROUND-2 VERDICT: GO** (all round-1 findings resolved; 0 new).
