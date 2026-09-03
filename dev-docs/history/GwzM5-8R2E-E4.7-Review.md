# GwzM5-8 R2-E E4.7 — single-axis peer-blind review (Opus, interior tier)

Date: **2026-09-02**. Reviewer: an independent peer-blind interior-tier review
session for the implementation lane. Axis: **the whole step, single axis** —
allowance-class disposition, the three residual sentences, the amendment
corrections' in-tree echoes, the `finish()`-reachability record, pins/digests,
gates, cap.

**Candidate:** one commit `d8c140f` on `e4/e4-7-closeout`, base gwz-core main
`f563446`. **Cap:** 460 whole handwritten lines.
**Worktree:** `…/scratchpad/e4-e47-wt`, byte-clean at `d8c140f` before and after
this review. **Target:** `…/scratchpad/e4-e47-target` (reviewer-exclusive; no
second target created). Python: `/opt/homebrew/bin/python3.13`. Direct exit
codes throughout. No commit, no push, no tag, no trailer was made by this
review.

**Authorities consumed, treated as falsifiable:**
`GwzM5-8R2E-E47-CharterPrep.md` (in full; measured at `0dae0d5`);
`GwzM5-8R2E-CapabilityFreeAmendment.md` §4 / §7 + every post-adoption bracket;
`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`; `GwzM5-8R2E-E45B-Report.md`;
`GwzM5-8R2E-E7-Acceptance.md` (O10 `:179` / O12 `:181`); the staged builder
brief `…/scratchpad/e47-brief.md` (R1–R4); and the operator's ruling (a) of
2026-09-02 as supplied verbatim to this review.

---

## 0. VERDICT

> **GO-WITH-CONDITIONS.** One **P2** and seven **P3**. No P1. Every condition is
> text-only, foldable by the lane owner at landing inside the remaining 100
> lines of cap; C1–C4 move digests that must be re-pinned in the same fold.

The step does what it says it does. The strongest evidence in its favour is
mechanical and is reproduced in §2 below: with comments and string **contents**
masked by the checker's own `mask_non_code`, the diff across all seventeen
`.rs` files contains **zero added code lines** and exactly **six removed
attributes**. There is no production behaviour change, no signature change, no
control-flow change, and no test added, moved or renamed. All twenty-five
machine digests recompute correct; all pins hold; every expiry is proven; every
"still bites" claim reproduces; the ruling (a) quote is byte-verbatim in both
homes.

The P2 is that the four **inert-allow** measurements written into the tree are
net `clippy` **diagnostic-count** deltas reported in **item-count** language.
The disposition they support is right; the number is not what the words say.

---

## 1. Reproduction environment and gates

All commands run in `…/scratchpad/e4-e47-wt` at `d8c140f` with
`CARGO_TARGET_DIR=…/scratchpad/e4-e47-target`.

| Gate | Command | Exit |
| --- | --- | --- |
| fmt | `cargo fmt --check` | **0** (no output) |
| check | `cargo check --all-targets` | **0** |
| clippy | `cargo clippy --all-targets -- -D warnings` | **0** (0 diagnostics) |
| boundary checker | `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | **0** — `checked-artifact boundary: ok (18 visible entries, 8 classified modules)` |
| boundary suite | `python3.13 -m unittest scripts.checks.test_check_checked_artifact_boundaries` | **0** — `Ran 69 tests in 570.459s … OK` |
| predicates | `python3.13 scripts/checks/check_merge_compatibility_predicates.py` | **0** — `validated 7 migration rules, 7 runtime bindings, and 10 archive shapes` (run with the CI's own arguments, `dev-docs/GwzM5-8I2CompatibilityPredicates.json --core .`) |
| predicates suite | `python3.13 -m unittest scripts.checks.test_merge_compatibility_predicates` | **0** — `Ran 27 tests … OK` |
| merge docs | `python3.13 scripts/checks/check_merge_docs.py` | **0** — `merge document consistency: ok (12 sources, 155 assertions)` |
| merge docs suite | `python3.13 -m unittest scripts.checks.test_check_merge_docs` | **0** — `Ran 3 tests … OK` |
| M4 scenario map | `python3.13 scripts/checks/check_m4_scenario_map.py` | **0** — `M4 scenario map: ok (39 scenario rows, 43 named tests, 22 registry rows all claimed)` |
| privacy probe | `python3.13 -m unittest scripts.checks.test_v1_lifecycle_privacy_probe` | **0** — `Ran 8 tests in 95.747s … OK` |
| release boundary | `python3.13 -m unittest scripts.checks.test_release_boundary` | **0** — `Ran 6 tests … OK` |

### 1.1 Partitions — measured from this review's own `--list`-verified snapshot

`cargo test --lib --no-run`, then the freshly built `gwz_core-*` binary copied
out of `target/debug/deps` and driven as a snapshot (so the counts cannot drift
under a rebuild mid-run).

| Partition | Filter driven on the snapshot | Result | Driver pin |
| --- | --- | ---: | ---: |
| `--list` total | `--list`, `grep -c ': test$'` | **1839** | 1839 ✓ |
| `checked_artifact::` | `checked_artifact::` | **457 passed**, 0 ignored, 1382 filtered | 457 ✓ |
| `v1_lifecycle::` | `workspace_ops::merge::v1_lifecycle:: --skip root_fault_matrix` | **266 passed**, 0 ignored, 1573 filtered | 266 ✓ |
| lib remainder | `--skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::` | **1114 passed + 1 ignored**, 724 filtered | 1114 + 1 ✓ |
| `g23::` | `workspace_ops::tests::g23::` | **130 passed**, 1709 filtered | 130 ✓ |

All four reproduce, on darwin, exit 0. *Procedural note: the lane owner cut
item (8)'s partition re-measurement from this review's scope mid-session, on
the ground that a comment/reason/allow-only diff cannot move a partition and
the landing verification owns the full run. The cut arrived after these runs
had already completed, so the measured numbers are reported rather than
discarded; the cut's own two-fact proof also holds independently —* `--list`
*is 1839 and, per §2, no hunk adds, removes or gates a* `#[test]`. Two notes on
method, so a later reader can repeat it exactly:

- **The `v1_lifecycle::` partition is the driver's own, i.e. WITH
  `--skip root_fault_matrix`.** Driven unskipped it is **267**; the extra row is
  `…v1_lifecycle::reverse::preservation::tests::root_fault_matrix::every_root_physical_and_successor_boundary_recovers_without_repeating_mutation`,
  which `run_r4bg_aggregate_gates.py:485-486` runs as its own `--release`
  battery expecting `1 passed`. The builder's 266 is **correct**; this review
  measured 267 first and reconciled against the driver's definition rather than
  filing a phantom finding.
- **Arithmetic closes:** 457 (CA) + 267 (v1, unskipped) + 1115 (remainder names)
  = **1839** = the `--list` total. The single ignored test crate-wide is
  `operation::workspace_mutator_lock::tests::child_process_observes_lock_contention`,
  and it is in the remainder — which is why the remainder alone carries the
  "+ 1 ignored".

**The aggregate driver `scripts/checks/run_r4bg_aggregate_gates.py` is NOT
edited** — verified: `git diff f563446..d8c140f -- scripts/` touches
`check_checked_artifact_boundaries.py` and nothing else. That is the right
outcome for this step and is independently corroborated: no `#[test]` was added,
removed or moved anywhere in the diff, and (see §2) no removed allow was
gating a `cfg(test)` item, so no partition could move.

### 1.2 Cap

`git diff f563446..d8c140f --numstat`, summed: **360 added / 92 removed** across
**18 files**. Reviewer-measured, agrees with the builder's report. Cap 460;
**100 lines of headroom remain**, which is where every condition below fits.

---

## 2. Is this a docs-and-allowances step, or did production behaviour move?

**Answer: docs and allowances. Proven mechanically, not read.**

Method: load `mask_non_code` out of the candidate's own
`check_checked_artifact_boundaries.py:1191` (the function the boundary checker
uses to blank comments and string/char *contents* while keeping newlines), apply
it to both sides of every `.rs` file in the diff, drop blank lines, and diff.

Result — the complete set of masked differences in the whole change:

```
DIFFERS  src/checked_artifact/bootstrap/managed.rs                (0 added / 4 removed)
DIFFERS  src/checked_artifact/capability/pre_catalog/provider.rs  (0 added / 4 removed)
DIFFERS  src/checked_artifact/mod.rs                              (0 added / 4 removed)
DIFFERS  src/checked_artifact/namespace/mod.rs                    (0 added / 4 removed)
DIFFERS  src/checked_artifact/observation.rs                      (0 added / 4 removed)
DIFFERS  src/operation/workspace_mutator_lock.rs                  (0 added / 4 removed)
IDENTICAL  catalog.rs · cleanup.rs · coordinator/execution.rs · coordinator/mod.rs ·
           entry.rs · interface_tests/catalog_activation_pin.rs ·
           namespace/tests_cleanup_matrix.rs · git/tests/g12.rs ·
           merge/gc.rs · v1_lifecycle/archive.rs · v1_lifecycle/finalization/execute.rs
```

Each of the six is the removed attribute and nothing else. **Zero added code
lines in the entire change.** In particular:

- `catalog.rs` is masked-IDENTICAL, so flag 5's declined option (b) was
  genuinely declined: the `pub(in crate::checked_artifact) use bootstrap::{…}`
  list is byte-unchanged.
- `catalog_activation_pin.rs` is masked-IDENTICAL, so its only change is the
  contents of an `assert!` message — not behaviour.
- `execute.rs` is masked-IDENTICAL: the three raw arms are untouched, exactly as
  ruling (a) requires.

---

## 3. The sweep, re-run independently

An independent extractor was written (balanced-bracket scan over every
`#[allow` / `#![allow` / `#[expect` / `#[cfg_attr(… allow …)]` in `src/`), run
against **both** `f563446` (extracted with `git archive`) and `d8c140f`, with
the prep's CORRECTED needle set plus the brief's R1 needles (`E4.5`, `E4.6`,
`E4.5/6`, `convertible`) and R2 needles (`markers`, `RootPreservationMarkers`,
`bootstrapped by nothing`, `fresh workspace`).

| | `f563446` | `d8c140f` |
| --- | ---: | ---: |
| allow/expect/cfg_attr attributes in `src/` | 176 | 170 |
| Tier-3, byte-identical "A1 activation: reached only by this tree's own suites" | **23** | **23** |
| non-Tier-3 attributes matching the needle set | 26 | 23 |

**Reconciliation with the builder's 24 actionable.** Of the 26 base hits, five
are the prep's declared non-members (`coordinator/mod.rs:41-44` schedule glob;
`interface_tests/fault_expected_keys.rs:219-222`;
`protocol/authority_record.rs:50-53`; and the two flag-4 sites
`merge/store/archived.rs:3-9` / `merge/store/mod.rs:44-50`), leaving 21. Three
further members carry **no** needle at all and were found only by authority:
`operation/workspace_mutator_lock.rs:44-47` (reason "R2-C0 freezes the checked
catalog borrow…"), `checked_artifact/namespace/mod.rs:20-23` (its text is
"Phase E4, itself sequenced…" — note it contains neither `E4.` nor `R2-E`, so
**the prep's own corrected needle set also misses it**), and
`observation.rs:155-158` (R3, from the E4.5-B report's flag 6). 21 + 3 = **24
actionable. CONFIRMED.**

At `d8c140f` the 23 non-Tier-3 hits decompose as 16 re-reasoned + 2 kept
(`gc_archived`) + the same 5 non-members. **The arithmetic closes exactly**
(24 actionable − 6 expired = 18 = 16 + 2). **The builder's sweep is complete;
no actionable site was missed.** The Tier-3 23 are byte-identical to base and
untouched.

### 3.1 R1 — the "movers" falsehoods

Sweeping `E4.5|E4.6` over `src/` **and** `scripts/` at `f563446` returns
exactly two claims that E4.5/6-B still MOVES the `execute.rs` arms:

1. `bootstrap/managed.rs:48` — *"shrink only as the remaining ACTIVATED-LEASE
   forward arms convert (E4.5/6-B)"*;
2. `check_checked_artifact_boundaries.py:536` — *"the movers left are E4.5/6-B's
   three `finalization/execute.rs` arms"*.

Both are addressed at `d8c140f` (the first by the expiry's replacement bracket
at `managed.rs:51`, the second by the appended bracket at the checker's
`:562-573`). Every other `E4.2-E4.6` occurrence is the allowance class itself,
dispositioned at its own site. **Builder's claim CONFIRMED.**

### 3.2 R2 — the withdrawn premises

- `bootstrapped by nothing` — **0 hits** in `src/` and `scripts/`.
- `fresh workspace` (case-insensitive) — **0 hits**.
- `RootPreservationMarkers` — 10 hits, all executable code or a fixture doc;
  none asserts who creates the parent.
- `gwz.conf/markers` / `conf/markers` — 30 hits; every one is a path constant,
  a test fixture, or `artifact/conf_integrity.rs`'s own (correct) statement that
  the marker directory is workspace-creation's.

**Flag 3 CONFIRMED by independent sweep: neither withdrawn premise has an
in-tree echo.**

---

## 4. THE SIX EXPIRIES — all six reproduce

The negative control first: `src/lib.rs` carries **no** crate-level
`dead_code` or `unused_imports` blanket (only
`#![allow(clippy::disallowed_methods, …)]` and a `collapsible_if` on the
taut-generated `cbor` module), and neither `mod operation` (lib.rs:24),
`mod workspace_mutator_lock` (`operation/mod.rs:15`), `mod entry`
(`checked_artifact/mod.rs:38`), `mod observation` (`:74`) nor `mod managed`
(`bootstrap.rs:7`) carries an ancestor allow. So `cargo clippy --all-targets
-- -D warnings` exiting **0** at `d8c140f` — which includes the plain `--lib`
(i.e. `not(test)`) target — is by itself a proof of use for five of the six.

| # | Site (base `f563446`) | Item | Proof of use, re-measured |
| --- | --- | --- | --- |
| 1 | `operation/workspace_mutator_lock.rs:44-47` | `catalog_mutation_lease` | `git grep` at `d8c140f`: **four** production callers — `merge/runtime/dispatch.rs:456`, `v1_lifecycle/checked.rs:166`, `:184`, `:185`. Commit message's "four" CONFIRMED. |
| 2 | `checked_artifact/mod.rs:28-33` | `pub(crate) mod entry` | lib-target clippy green with the allow gone ⟹ nothing in `entry.rs` is dead. |
| 3 | `checked_artifact/namespace/mod.rs:47-51` | `use host::{HostActionNamespaceV1, retain_action_namespace}` | Both names consumed **through this re-export** at `bootstrap/managed/provider.rs:76-77`, used at `:164/:191/:246/:288/:334/:410` and called at `:411`. The parent `mod namespace` blanket is `dead_code`, a **different lint**, so it cannot shadow an `unused_imports` allow. |
| 4 | `checked_artifact/observation.rs:155-158` | `parent_is_canonical` | Two production callers, `entry.rs:267` and `entry.rs:388`. Commit message's "two" CONFIRMED. The attribute was `cfg_attr(not(test), …)`, so the `--lib` target is precisely the arm that would have fired. |
| 5 | `checked_artifact/bootstrap/managed.rs:44-52` | `use provider::*` (glob) | Green. The builder's ground (a glob `unused_imports` fires only when *nothing* it brings in is used) is correct rustc behaviour and the removal confirms it. |
| 6 | `capability/pre_catalog/provider.rs:41-44` | `mod leaf_observation` | **The one that needed the blanket-lifted control, and it holds.** Removing `checked_artifact/mod.rs`'s `mod capability` blanket at `d8c140f` (allow already gone) yields exactly 12 diagnostics — and **zero** of them names anything under `provider/leaf_observation`. So nothing in that module is dead: the allow suppressed nothing even in a blanket-free world. The prep expected "re-reason"; the builder's contrary measurement is **correct**. |

**All six expiries are sound.** #6 in particular is an improvement on the prep,
not a deviation from it.

---

## 5. THE SIXTEEN RE-REASONED PERMANENT — adjudicated

Method: for each site, remove the attribute (balanced-bracket excision), run
`cargo clippy --all-targets --message-format=short -- -D warnings`, count
`: error: ` lines, restore. Baseline at `d8c140f`: **rc=0, 0 errors.** 26 probe
runs total, all serialised on the single shared target.

### 5.1 The twelve that bite today — every "still bites" claim reproduces

| Site (`d8c140f`) | rc | errors | Reason's claim | Verdict |
| --- | ---: | ---: | --- | --- |
| `mod.rs:11` `mod bootstrap` | 101 | 44 | permanent, removal is DR-1's | TRUE |
| `mod.rs:22` `mod capability` | 101 | 12 | ″ | TRUE |
| `mod.rs:40` `mod fault_v1` | 101 | 4 | ″ | TRUE |
| `mod.rs:52` `mod leaf` | 101 | 4 | ″ | TRUE |
| `mod.rs:63` `mod namespace` | 101 | 24 | ″ | TRUE |
| `mod.rs:77` `mod protocol` | 101 | 52 | ″ | TRUE |
| `catalog.rs:18` `mod bootstrap` | 101 | 1 | *"removal reddens on **both names**"* | **TRUE, exactly** — the single diagnostic is `catalog/bootstrap.rs:178: methods `retire_admitted_action` and `observe_roaming_anchor_home` are never used` |
| `catalog.rs:28` `mod classifier` | 101 | 1 | classifier's unreached vocabulary | TRUE (`current_historical_collision_digest` never read) |
| `catalog.rs:37` `mod enumeration` | 101 | 5 | enumeration surface | TRUE |
| `catalog.rs:51` re-export | 101 | 1 | *"MEASURED … the allow bites on exactly **ONE** name, `CatalogOwnerV1`"* | **TRUE, exactly** — `catalog.rs:52:25: unused import: CatalogOwnerV1`, and nothing else |
| `coordinator/mod.rs:15` `mod execution` | 101 | 8 | *"reddens on the E0.2 §7.4 interior"* | TRUE (`authorize_write`, `RetainedWriteAuthorityV1`, `require_leaf_digest`, …) |
| `coordinator/mod.rs:28` `mod identity` | 101 | 10 | the rest gains no consumer | TRUE |

The six `checked_artifact/mod.rs` blankets total 140 diagnostics across the
crate — consistent with E7-Acceptance §3's 481-item blanket set once rustc's
grouping is accounted for. **No "permanent" reason in this group is a lie.**

### 5.2 The four inert ones — the class adjudication

All four ARE inert today. Removing each **alone** leaves clippy at **rc=0, 0
errors**. So prep stop-trigger 2 fired *on its face*.

**Adjudication of the disposition: the builder is RIGHT to keep them, and its
argument is the correct one.** Grounds, in order of weight:

1. **Stop-trigger 2's stated premise is falsified here.** Its text is *"If
   removing a 'permanent' allow is green, **the item is live** and the reason
   would be a lie."* Measured: the items are **dead**; only the ALLOW is
   shadowed by an ancestor blanket. The trigger's conclusion does not follow
   from its own premise once the premise is checked.
2. **"Expire" would have recorded a falsehood.** The prep defines EXPIRE as
   *"remove the allow and prove `cargo check` + `clippy -D warnings` still
   pass, **i.e. the item is used**"*. Expiring these four on the green would
   have asserted exactly the thing that is false. Between the two dispositions
   §4 offers, KEEP is the honest one.
3. **The tree's own precedent is settled and landed.** `bootstrap/managed.rs:9-14`
   already carries an inert documenting allow *and says so* — pre-existing, not
   an E4.7 invention.
4. **The builder did not conceal it.** The inertness is stated at each of the
   four sites and in the commit message. Under the E4 standing order a
   surfaced-and-argued trigger is the lane owner's to rule, not a silent pass.

**But the measurement itself does not say what the tree claims it says. See
P2-1.** The four deltas were re-measured and every *net scalar* reproduces
exactly:

| Site | blanket-only | blanket + inner | **net delta** | builder's number |
| --- | ---: | ---: | ---: | ---: |
| `bootstrap/managed.rs:20` (under `mod bootstrap`) | 44 | 45 | **+1** | +1 ✓ |
| `provider.rs:21` `authority_record_binding` (under `mod capability`) | 12 | 32 | **+20** | +20 ✓ |
| `provider.rs:37` `barrier_mutation` (under `mod capability`) | 12 | 14 | **+2** | +2 ✓ |
| `namespace/mod.rs:38` `mod host` (under `mod namespace`) | 24 | 31 | **+7** | +7 ✓ |

---

## 6. FINDINGS

### P2-1 — the four inert-allow numbers are net diagnostic-count deltas written into the tree as item counts

**Sites:** `checked_artifact/bootstrap/managed.rs:17-19`
(*"it becomes load-bearing the moment that blanket goes — **one dead item
surfaces beneath it**"*); `capability/pre_catalog/provider.rs:18-19`
(*"**twenty dead items surface** beneath this allow once … the `mod capability`
blanket is lifted"*); `provider.rs:35-36` (*"**two dead items surface** beneath
this allow once the `mod capability` blanket is lifted"*);
`checked_artifact/namespace/mod.rs:33-34` (*"lifting … the `mod namespace`
blanket **surfaces SEVEN dead items** beneath this allow"*). The same four
numbers are asserted in the commit message as *"(+1, +20, +2 and +7 **dead
items** respectively)"* under the banner *"so 'permanent' is a measured claim
rather than a repeated promise"*.

**Mechanism.** The net error-count delta is not a count of anything surfacing.
`rustc`'s dead-code pass reports the *outermost* dead item and suppresses
reports nested inside it, so removing an inner allow both *adds* new
diagnostics and *absorbs* previously separate ones. The net scalar is the
difference of those two movements. Taking the **set** difference of the
diagnostics instead:

| Site | reason says | net error-count delta | **new diagnostics** | **named dead items** |
| --- | --- | ---: | ---: | ---: |
| `managed.rs:20` | "**one** dead item surfaces" | +1 | **3** | **3** — `ManagedParentPurpose::Archive` variant (`managed.rs:353`), `ALL` (`:63`), `provider.rs:134::row` |
| `provider.rs:21` | "**twenty** dead items surface" | +20 | **20** | **21** (one diagnostic names `retained_parent_identity` *and* `payload_lengths`) |
| `provider.rs:37` | "**two** dead items surface" | +2 | **2** | **4** — `Active`, `Retired` (`barrier_mutation.rs:263`), `OwnDrive`, `Stranded` (`:314`) |
| `namespace/mod.rs:38` | "**SEVEN** dead items" | +7 | **14** | **43** across `namespace/{backend.rs,host.rs,mod.rs,roles.rs}` — incl. the whole `PublishRoleV1` / `PublishDestination` / `CleanupRetirementDestination` / `ScheduledBarrierOrdinal` family |

Under the reading the words assert ("N dead items surface beneath this allow"),
**all four are wrong**. Under a charitable "N = diagnostics" reading, **two of
four are still wrong** — `managed.rs` (1 vs 3) and `namespace/mod.rs` (7 vs 14,
a 2× understatement in diagnostics and a **6×** one in items). Only the
net-scalar reading reproduces, and the net scalar is not a property of the
allow at all.

**Why it matters at P2 rather than P3.** This is not a stray cite. These four
numbers are the *entire* epistemic content of the step's most contested
disposition, and they are the reason the class was kept rather than expired.
The DR-1 reader who takes `managed.rs`'s "one dead item" at face value will
conclude the allow is nearly worthless and delete it; measured, it hides three,
including an unconstructed enum variant. The same reader at `namespace/mod.rs`
is told SEVEN and will find **forty-three**, spanning four files — the entire
`PublishRoleV1` / `PublishDestination` / `CleanupRetirementDestination` role
vocabulary plus sixteen `host.rs` methods and seven `namespace/mod.rs` ones.
A step whose whole product is documentation accuracy should not ship four
measured-sounding numbers of which one is right.

**Condition C1** (text only, ~6 lines): restate the four either as what was
measured (*"removing this allow with the `mod X` blanket also lifted moves
`clippy -D warnings`'s error count by +K"*) or as the measured item counts
(**3 / 21 / 4 / 43**). Either is fine; they must agree with each other and with
the commit message. Moves the `pre_catalog.rs` tree digest;
`managed.rs` and `namespace/mod.rs` are unpinned.

### P3-1 — the `gc_archived` family's "true extent" record is itself under-enumerated

**Site:** `v1_lifecycle/archive.rs:112-126`, the extent claim at `:113-118`. The record's whole purpose is to
correct CapFree §5's three-name under-enumeration, and it says: *"The family is
**SEVEN functions and one struct across TWO files**, not the three … §5
names"*, listing `gc_archived`, `gc_archived_with_hook`, `remove_archive`,
`require_same_archive` here plus `merge/gc.rs`'s `preflight_archived_cleanup`,
`delete_preflighted_backup_refs`, `require_backup_refs_absent` and
`PreparedArchivedCleanup`, *"which have no other caller"*.

**Measured** (remove `archive.rs`'s allow alone → rc=101, 14 diagnostics; the
dead set is exactly what is reachable only from `gc_archived`), the family is:

- `archive.rs` — **8** functions, not 4: the record's four **plus**
  `any_open_record_present` (`:193`, called only at `:152`), and this file's
  own `require_real_directory` (`:254`, called only at `:195`),
  `require_regular_file` (`:266`, called only at `:209`) and `io_error`
  (`:282`). Confirmed by caller grep that each has no other caller *in this
  file* — the same-named helpers in `record_wire/location.rs`,
  `root/abort.rs`, `store/atomic_upgrade.rs`, `root/finalization.rs` and
  `store/archived.rs` are distinct file-local functions and are unaffected.
- `merge/gc.rs` — 3 functions and **2** structs, not 1: `PreparedArchivedCleanup`
  **plus** `ArchivedBackupArtifact` (`gc.rs:22-29`, all six fields).
- **A third file the record does not name:** `v1_lifecycle/archive_result.rs`
  loses `ValidatedArchivedMerge`'s `source_version`, `destination_bytes`,
  `destination_sha256` fields and its `source_version`, `destination_bytes`,
  `destination_sha256`, `cleanup` methods. The struct itself survives (shared
  with `status.rs:3`), but four of its members are family-only.

So: **11 functions + 2 structs across two files, plus 4 members of a shared
struct in a third**, against a recorded 7 + 1 across two. The builder appears
to have transcribed the prep's §4.1 table rather than re-measuring, even though
the KEEP disposition's own measurement (which reddens at 14 diagnostics) puts
the full list on screen. It matters because this record is precisely what DR-1
will size the delete-or-rebuild choice from, and it reproduces §5's failure mode
one level up. **Condition C2** (~4 lines; moves the `v1_lifecycle/mod.rs` tree
digest, which this step moves anyway).

### P3-2 — the `:108-111` cite for that allowance is stale at two homes, and this step widened the drift

At `f563446` the `gc_archived` allow sits at `archive.rs:112`; at `d8c140f`,
after this step's 15-line doc bracket, it sits at **`:127-136`**. Two cites
still say `:108-111`:

1. `src/workspace_ops/merge/v1_lifecycle/archive.rs:231` — the in-tree
   carved-site annotation, *"the DEAD `remove_archive` arm behind the
   `:108-111` allowance"*. A plain comment, in a file this step edited and
   re-digested; nothing barred fixing it.
2. `scripts/checks/check_checked_artifact_boundaries.py:527` — the same string
   as the `CAPABILITY_FREE_RAW_WRITER_INVENTORY` row's description. The brief
   said *"do NOT touch the inventory's rows"*, which covers this one; note the
   key-set digest hashes **keys only**, so a description fix would move no pin.

**Condition C3:** fix (1) in the fold; route (2) to the next pins package or
DR-1 with a named line, rather than leaving it silent.

### P3-3 — the four inert allows' `reason=` strings carry no inertness qualifier

The inertness and the delta live in the adjacent `///` / `//` block at all four
sites; the `reason=` strings themselves read only *"… PERMANENT pending DR-1
(E4.7, 2026-09-02)."* A reader who greps attributes — which is exactly what
this step's own sweep does, and what the next sweep will do — sees a permanence
claim with no signal that the allow is currently shadowed. This matches the
tree's landed precedent (`managed.rs:9-14` also puts it in the doc), so it is
not a defect against precedent; but the class exists to make silence impossible,
and an attribute-only reader is the class's own audience. **Condition C4**
(~4 short clauses).

### P3-4 — two A1-era allowances with a discharged forward reference left with no dated disposition

`src/workspace_ops/merge/store/archived.rs:3-9` and
`src/workspace_ops/merge/store/mod.rs:44-50` both read *"R1-frozen archived-load
… remains **until the A1 activation review**"*. A1 shipped **2026-08-25** —
the identical fact that made `observation.rs:155-158` an expiry under the
brief's R3. They are a **fourth** A1-era wording that the prep captures in
neither §1.3 (Tier-2 actionable), §1.4 (no-change) nor §1.6 (the 23 Tier-3):
**the prep misses them outright**, and the builder is the first to name them
(flag 4).

Reviewer-measured, and the builder's note is exactly right:

- `store/archived.rs:3-9` — removal **rc=0, 0 errors**: the allow is
  **redundant**. This is `observation.rs`'s case precisely.
- `store/mod.rs:44-50` — removal **rc=101, 1 error**
  (`store/mod.rs:44:8: method \`load_archived\` is never used`): **load-bearing**.

**Adjudication: leaving them untouched is correct on the letter** — they are
outside amendment §4's `E4.2-E4.6` class and no adopted record owns them, and a
builder inventing scope is the worse failure. **But** the outcome is that the
step which exists to stop allowances becoming "permanent by silence" closes
with two allowances in exactly that state, both measured and both reported.
**Condition C5:** one dated line each (or one line in the phase-close record)
recording them as swept at E4.7, out of the `E4.2-E4.6` class, and routed to
DR-1 with their measurements.

### P3-5 — `protocol/authority_record.rs:50-53` (flag 7): correct exclusion, but note the shape

Reason: *"Step 2.4 installs the seam; plan §4 Step 3.3 wires its production
consumer."* Step 3.3 has landed and did not wire it, so this is the same
defect shape — a forward reference to a completed step. It is R2-D-era, not
R2-E's class; the prep classified it *"adjacent, not a member; flag only"* and
the builder followed. **Correct application.** Worth one line in the DR-1
handoff so the next sweep does not have to rediscover it.

### P3-6 — the lock arm's residual sentence says "the boundary's" where it means "the checked boundary's"

`execute.rs:82-87`: *"a `Bytes → Bytes` replacement, so **the boundary's**
`replace_exact` detaches before publishing"* — sitting two arms above
`PublicationPhysicalAction::WriteBoundary`. Here "the boundary" is the *checked*
boundary; the boundary arm's own sentence (`:91-97`) avoids the collision
(*"a `Bytes → Bytes` replacement **whose** `replace_exact` detaches"*). The
wording is the brief's, verbatim, so this is not a builder deviation — but
these sentences are written to be read by DR-1 years out. One word: "the
**checked** boundary's".

### P3-7 — `abort/evidence.rs` and `live.rs` are cited bare in all three residual sentences

Real paths, verified at `d8c140f`: `src/workspace_ops/merge/abort/evidence.rs`
and
`src/workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication/live.rs`.
From `v1_lifecycle/finalization/execute.rs`, a bare `abort/evidence.rs` reads as
a sibling under `v1_lifecycle/` — and **there is no `v1_lifecycle/abort/`
directory**, so the cite resolves only by whole-tree search. Again the brief's
wording. Cheap to qualify to `merge/abort/evidence.rs` and
`…/publication/live.rs`.

---

## 7. THE THREE RESIDUAL SENTENCES — verified against `f563446` / `d8c140f`

**The header's quote of ruling (a) is BYTE-VERBATIM.** Extracted from
`execute.rs`'s `//!` block, un-prefixed and whitespace-normalised, then compared
character-for-character with the ruling as supplied to this review: **identical,
540 characters**. The commit message's quote is **identical** too.

**Arm and sentence placement, re-measured:**

| | `f563446` | `d8c140f` sentence | `d8c140f` arm |
| --- | ---: | ---: | ---: |
| marker (`write_atomic(marker_yaml)`) | `:45` | `:71-78` | **`:79`** |
| lock (`write_atomic(LOCK_PATH)`) | `:48` | `:82-87` | **`:88`** |
| boundary (`publish_workspace_exclude_candidate`) | `:51` | `:91-97` | **`:98`** |

The ruling's `:45` and the amendment's `:48`/`:51` are confirmed correct at the
base sha, and the header says so explicitly — a correctly self-documenting cite
drift.

**The marker's mechanism, cite by cite, all re-opened at `d8c140f`:**

| Claim in the sentence | Verified |
| --- | --- |
| the abort's reverse-pair `classify_remove` | `merge/abort/evidence.rs:307-311` — `transition_file(artifact_facts::classify_remove(root, marker_path_v1(record)?, candidate.marker_yaml.as_bytes())?)` ✓ |
| reads as `Ambiguous` at `classification.rs:175-177` | **UNMOVED** — `git diff f563446..d8c140f -- src/checked_artifact/classification.rs` is empty; `:175-177` is exactly `if residue.authority.is_some() && !authority_current { return Ok(ExactTransition::Ambiguous); }` ✓ |
| → `abort/evidence.rs` → `Other` | `evidence.rs:325-332` `transition_file`: `Ambiguous => FileState::Other` ✓ |
| → `RecoveryRequired`, stranding `gwz merge --abort` | the E4.5-B report's driven-and-ablated transcript; not re-driven here (no probe budget in a docs step), cited correctly ✓ |
| "distinct from and stronger than the detach window below" | matches the E4.5-B report's own words ("a *distinct and stronger* ground … not a detach window but a directional-residue window") ✓ |
| cites `dev-docs/GwzM5-8R2E-E45B-Report.md` and ruling (a) | both present ✓ |

**The lock's and boundary's observation-dead window:**

| Claim | Verified |
| --- | --- |
| the shipped **abort** observer refuses the absence | `evidence.rs:298-305` / `:313-319` call `classify_file(observe(LOCK_PATH), lock_yaml, baseline_lock_yaml, **false**)`; `classify_file` (`:343-360`) maps `Missing` with `missing_is_baseline = false` to `FileState::Other` ✓ |
| the shipped **forward** observer refuses the absence | `…/publication/live.rs:113` destructures `let (FileDigest::Regular(lock), marker, boundary) = … else { return Ok(None) }` — a missing lock aborts the snapshot, and `resolve_candidate(_, None)` (`:59-93`) returns `PublicationResolution::Ambiguous` ✓ |
| the marker is *not* in this class | `evidence.rs:313-319` passes `missing_is_baseline = **true**`, so `Missing → Baseline` — exactly the E4.5-B report's "the leaf itself is fine" ✓. The two grounds really are distinct. |
| the boundary carries row `:279`'s frozen cell-2 wording to DR-1 | present at `:96-97` ✓ |

**All three sentences are substantively correct.** The only defects are the two
citation-form nits, P3-6 and P3-7.

---

## 8. THE `finish()`-REACHABILITY RECORD — every claim re-traced

The block is `cleanup.rs:146-200`, immediately above the **unmoved** `:201`
recheck.

| Claim | Re-traced at `d8c140f` |
| --- | --- |
| the recheck is UNCONDITIONAL within `finish()` | `if let Some(source) = residue.source {` opens at `:96` and **closes at `:145`**; the record block is `:146-200` and `let rechecked = self.inspect_family(expected, goal)?;` at `:201` is at the function's top level ✓ |
| `finish()` is reached only through `finish_replace`/`finish_remove` | `cleanup.rs:26` `fn finish` is private; `finish_replace` (`:9`) and `finish_remove` (`:18`); their **only** callers crate-wide are `transition.rs:106` and `transition.rs:167` ✓ |
| **E4.1** and **E4.2 parent half** do not reach `finish()` | `git grep "CheckedArtifact::"` across `checked_artifact/{catalog.rs,catalog/,capability/,bootstrap/,coordinator/}` returns **two doc-comment mentions and zero code sites** (`bootstrap/managed.rs:10`, `coordinator/execution.rs:28`). Neither path constructs a `CheckedArtifact`, so `finish()` is unreachable — re-measured, prep §2 CONFIRMED ✓ |
| E4.2 parent half takes the checked retirement | `namespace_mutation.rs:284` = `self.require_absent(&destination_name, label)?;` and `:306` = `PublicationSourceV1::regular_file(&source.encoded_identity, &source.bytes),` — both cites **exact** ✓ |
| **E4.2 record half** reaches `finish()` | `entry.rs:394` = `artifact.replace_exact(&CheckedArtifactFact::Missing, goal)` → `transition.rs:105` `publish_goal` then `:106` `finish_replace` — **`:105-106` exact** ✓ |
| cite drift re-pointed: `residue.rs:578` | `pub(super) fn rebarrier_exact` is at `:578` ✓ |
| E7-Acceptance's O12 row is `:181`, **not** the `:179` §4 and the plan cite | `:181` is the O12 row; **`:179` is the O10 row**, exactly as the block says ✓ |
| the prep's conditional fourth row (E4.5-B's marker arm) is **VACATED not answered** | correct and required — ruling (a) forecloses the arm; answering a vacated row would have been the error ✓ |
| VERDICT: reopen condition CHECKED, NOT MET; **DECISION A-1 STANDS** | follows from the four traces; no converted consumer bypasses **both** mechanisms ✓ |

**The record is correct, complete against E0.2 §7.1's duty, and the strongest
single artefact in the change.** Note in its favour: it correctly identifies
that the E4.2 record half is the *strongest* form of the A-1 answer (the
compensating observation runs on the first converted production write) rather
than merely an acceptable one, and it correctly refuses to answer a row the
ruling vacated.

---

## 9. DIGESTS AND PINS — recomputed independently

Every digest in the candidate's checker was recomputed with the checker's own
`source_tree_digest` re-implemented from `:1264-1280` (roots named `mod.rs`
digest the whole parent subtree; any other root covers itself plus its
same-stem directory; per-file `sha256` for the flat map).

```
FLAT  (18 entries)  — all 18 OK
TREE  (7 entries)   — all 7 OK
BAD = 0
```

**Exactly six moved vs `f563446`**, all comment/`reason`-only (§2 proves the
comment-only part mechanically):

| moved | kind | site |
| --- | --- | --- |
| `checked_artifact/entry.rs` | flat | checker `:71` |
| `checked_artifact/mod.rs` | flat | checker `:73` |
| `operation/workspace_mutator_lock.rs` | flat | checker `:88` |
| `checked_artifact/catalog.rs` | tree | checker `:266` |
| `checked_artifact/capability/pre_catalog.rs` | tree | checker `:265` |
| `workspace_ops/merge/v1_lifecycle/mod.rs` | tree | checker `:269` |

The other 19 digests verify **unchanged**, which is itself a proof that their
files are byte-identical to `f563446`. Coverage was cross-checked in the other
direction too: every edited file is either under one of the six moved digests
(`provider.rs` → `pre_catalog.rs` tree; `archive.rs` and
`finalization/execute.rs` → `v1_lifecycle/mod.rs` tree) or genuinely unpinned
(`cleanup.rs`, `coordinator/{mod,execution}.rs`, `bootstrap/managed.rs`,
`namespace/{mod,tests_cleanup_matrix}.rs`, `observation.rs`,
`interface_tests/catalog_activation_pin.rs`, `git/tests/g12.rs`,
`merge/gc.rs` — the last because `merge/gc.rs` is **not** under `v1_lifecycle/`).
**No edited file slipped past a digest that did not move.**

**Pins, all re-measured:**

| Pin | Expected | Measured |
| --- | --- | --- |
| `PRODUCTION_CALLER_COUNT` | 1 | **1** ✓ |
| `CAPABILITY_FREE_RAW_WRITER_INVENTORY` | 20 rows, byte-unchanged | **20 rows, raw-identical to `f563446`** ✓ |
| its key-set digest | `867c580f625d7efe0cf72dcc8e0ad01e36268d1478829a469eb0f57953dbd385` | **recomputed identical** ✓ |
| `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` (O13) | 3 rows, `archive.rs: {sync_dir: 2}` | **raw-identical**; `archive.rs` still `{"sync_dir": 2}` ✓ — the KEEP disposition leaves the shrinkage arm unfired |
| `V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS` | 3 entries | **raw-identical** ✓ |
| `ENTRY_REFERENCES` | data unchanged, comment only | **data-identical, raw differs** ✓ — exactly the claim |
| `ENTRY_ITEMS` | untouched | **raw-identical** ✓ |
| `PROTECTED_COMPILER_MODULES` | untouched | **raw-identical** ✓ |
| record-root tripwire | unmoved | `store/rewrite.rs` row and its exception string unchanged ✓ |

The O13 comment block's *"'Until E4.4' above is spent: E4.4 as chartered does
not start and E4.7 retires none of the three"* was already landed by the pins
package; E4.7's duty there was verification, and it verifies.

### 9.1 The two KEPT `gc_archived` allowances

- `v1_lifecycle/archive.rs:127-136` — removal **rc=101, 14 errors**. Genuinely
  load-bearing; the KEEP is honest. Prep §4.3(B)'s byte-exact reason text is
  reproduced **verbatim** at both homes (compared character-for-character).
- `merge/gc.rs:43-52` — removal alone **rc=0, 0 errors**. **Redundant today**,
  because `archive.rs`'s allowance seeds the family's liveness. The builder
  measured this and **wrote it into the tree** (`gc.rs:37-39`: *"MEASURED at
  E4.7: this allow is today redundant … but it is KEPT, because it is the
  record that this half of the family travels with the other"*). That is the
  right call and an honest one — deleting it would orphan the record for the
  downstream half, and the family's fate is DR-1's.
- Removing both together: rc=101, 16 errors.

---

## 10. FLAG ADJUDICATIONS

**Flag 1 — g12.rs path/content. UPHELD, and the brief was wrong, not the
builder.** The brief cited `src/workspace_ops/tests/g12.rs:834`; no such file
exists. The prep §1.2 had it right: `src/git/tests/g12.rs:834`, and that is
where the content was rewritten. Verified in agreement with the landed pins
text: `catalog_names.rs:41-46` reads *"still live until **DR-1** decides its
retirement — E4.7's legacy in-place-writer clause RE-OWNS there … CapFree §4"*;
`g12.rs:834-838` now reads *"still live until **DR-1** decides its
retirement. [E4.7 … RE-OWNS to DR-1 … which is the landed reading at
`checked_artifact/catalog_names.rs`; this cite disagreed with it in-tree and now
does not.]"* **They agree.** `catalog_names.rs` is untouched, as instructed.

**Flag 2 — the inert class. ADJUDICATED: disposition SOUND, number WRONG.**
See §5.2 (the four grounds on which KEEP beats EXPIRE) and **P2-1** (the four
numbers). The builder was right not to stop and right to keep; it was wrong to
write a net scalar as an item count.

**Flag 3 — R2 echoes do not exist in-tree. CONFIRMED by independent sweep**
(§3.2): zero hits for either withdrawn premise.

**Flag 4 — the two "A1 activation review" allowances. CORRECT ON THE LETTER,
THIN ON THE RECORD.** See **P3-4**. The measured note is confirmed exactly:
`store/mod.rs`'s is load-bearing (`load_archived` never used), `archived.rs`'s
is redundant. Note that these two are a prep **omission**, not a builder one —
they appear in none of §1.3 / §1.4 / §1.6.

**Flag 5 — option (b) declined. DECLINE UPHELD. Do NOT overturn.** Dropping
`CatalogOwnerV1` from `pub(in crate::checked_artifact) use bootstrap::{…}` is a
change to a re-export **surface**, which is (i) outside the brief's explicit
"allows, reasons, comments only", and (ii) outside amendment §4's
expire-or-re-reason menu — it is a third thing. The prep itself called it *"the
one behaviour-free surface **narrowing** in the step"*, which concedes the
category. Taking a prep *recommendation* over a *written charter constraint*
would have been the error. The decline is recorded in the reason string itself,
so DR-1 inherits it visibly, and the accompanying measurement is exactly true
(§5.1: the allow bites on precisely one name). **Nit, not a condition:** unlike
the `gc_archived` delete, the option is not routed to DR-1 *by name*; one clause
would fix that.

**Flag 6 — the stale `:108-111` inventory cite. P3** (P3-2). The description is
prose inside a data row's value; the key-set digest hashes keys only, so fixing
it costs no pin. The brief's "do NOT touch the inventory's rows" is a fair
defence for the checker home, but not for the identical cite at
`archive.rs:231`, which is a plain comment in a file this step already
re-digested — and which this step's own 15-line insertion made 15 lines more
wrong.

**Flag 7 — `authority_record.rs:50-53`. CORRECT.** Verified R2-D-era
(`unused_imports`; "Step 2.4 installs the seam; plan §4 Step 3.3 wires its
production consumer"), adjacent to the class, not a member. See P3-5 for the
one-line note owed.

**Flag 8 — the appended dated bracket rather than an in-place edit.
ACCEPTABLE, and required.** Prep stop-trigger 4 says *"Report the collision
rather than overwriting"*; the amendment's mechanism is dated brackets. The
landed E4.4-6-B bracket's second sentence (*"the movers left are E4.5/6-B's
three arms"*) is false post-ruling (a), and the E4.7 bracket at
`check_checked_artifact_boundaries.py:562-573` corrects it explicitly at the
same site. Data verified unchanged (`ENTRY_REFERENCES` data-identical;
`ENTRY_ITEMS` raw-identical). The prep's `:398` cite for this comment was
already superseded by the pins package before E4.7 opened.

*Observation, no action:* the same landed-text situation is handled the other
way at `checked_artifact/mod.rs:10`, where the builder **edited** the pins
package's sentence in place (deleting "`entry`," from the class-member list)
and appended an explanation. That is defensible — leaving "entry" in a list of
"the allows below" would dangle at a removed attribute — but the mechanism is
inconsistent within one commit. Worth one sentence in the phase-close record.

---

## 11. WHAT THE REVIEW COULD NOT FALSIFY

Recorded so a later reader knows the boundary of this axis:

- **Nothing was left unverified for want of a run.** Every gate, probe and
  partition in §1 and §5 was executed by this review at `d8c140f` before the
  lane owner's mid-session scope cut (which withdrew item (8)'s partitions and
  told the review not to re-run the compile-probe suites). Nothing was re-run
  after the cut, and no result below rests on the builder's report alone.
- The E4.5-B wall itself was **not re-driven**. The residual sentences cite a
  driven, ablated transcript in `GwzM5-8R2E-E45B-Report.md`; every *static*
  cite in the mechanism chain was re-opened and verified (§7), but
  `Ok(RecoveryRequired)` was not reproduced in this session. A docs step is not
  the place to spend that budget, and ruling (a) has already settled the
  disposition.
- The **matrix** (three platforms) is not this review's; per plan `:628` this is
  a single-axis interior review with the matrix at the landing.
- Nothing. `namespace/mod.rs:38`'s two `rustc`-elided "multiple methods"
  groups were expanded by re-running that probe with the default (non-`short`)
  message format and reading the listed signatures — 16 methods at
  `namespace/host.rs:121` and 7 at `namespace/mod.rs:138` — so the 43 is exact,
  not a lower bound.

---

## 12. CONDITIONS (all foldable by the lane owner at landing)

| # | From | Act | Digest impact |
| --- | --- | --- | --- |
| **C1** | P2-1 | Restate the four inert-allow numbers as what was measured (net `clippy -D warnings` error-count delta) **or** as the measured item counts (3 / 21 / 4 / 43). Make the commit message agree. | `pre_catalog.rs` tree |
| **C2** | P3-1 | Correct `archive.rs:113-118`'s family extent to the measured set (add `any_open_record_present`, this file's `require_real_directory` / `require_regular_file` / `io_error`, `gc.rs`'s `ArchivedBackupArtifact`, and `archive_result.rs`'s four family-only members). | `v1_lifecycle/mod.rs` tree |
| **C3** | P3-2 | Re-point `archive.rs:231`'s `:108-111` to `:127-136`; name the checker's `:527` twin as a routed item. | `v1_lifecycle/mod.rs` tree |
| **C4** | P3-3 | Fold "inert under the `mod X` blanket at `checked_artifact/mod.rs`; load-bearing by +K if it goes" into each of the four `reason=` strings. | `pre_catalog.rs` tree |
| **C5** | P3-4 | One dated line each at `merge/store/archived.rs:3-9` and `merge/store/mod.rs:44-50` (or one phase-close line) recording them as swept, out of class, routed to DR-1 with their measurements. | none |
| **C6** | P3-6, P3-7 | "the **checked** boundary's"; full paths for `merge/abort/evidence.rs` and `…/publication/live.rs`. | `v1_lifecycle/mod.rs` tree |
| **C7** | P3-5, flag 5 nit, flag 8 obs. | Three one-line record entries: `authority_record.rs:50-53` to DR-1's sweep; the `CatalogOwnerV1` narrowing named as a DR-1 agenda sub-item; the mod.rs:10 in-place edit noted. | none (record document) |

Estimated fold: **~25-40 lines**, against **100 lines of remaining cap**. Three
tree digests (`pre_catalog.rs`, `v1_lifecycle/mod.rs`) and no flat digest move;
they must be recomputed **after the last edit** with `source_tree_digest`'s own
algorithm and re-pinned in the same commit.

---

## 13. VERDICT LINE

> **GO-WITH-CONDITIONS** — no P1; one P2 (the four inert-allow numbers are net
> `clippy` diagnostic-count deltas written into the tree as item counts, two of
> them wrong under every reading) and seven P3, all text-only and foldable by
> the lane owner at landing within the remaining 100 lines of cap, with three
> tree digests re-pinned in the same fold. The step is what it claims to be:
> masked of comments and string contents, the whole change is six removed
> attributes and zero added code lines; all 25 digests recompute correct; every
> pin holds; all six expiries and all twelve "still bites" claims reproduce; the
> ruling (a) quote is byte-verbatim in both homes; and the `finish()`-
> reachability record is correct, complete and correctly refuses to answer the
> row ruling (a) vacated.
