# R2-E Phase E1 — interior review (single-axis, peer-blind)

**Object.** Branch `e1-cleanup`, exactly one commit `4a0b01a` over base
`8597d32` (`origin/main`). Eleven files, +1,280 / -14.
**Reviewer posture.** Read-only on tracked files; every claim below was
re-derived from the tree or from an executed gate, not from the commit
message. Probes were run against scratch copies outside the repo.
**Charter.** `GwzM5-8R2E-Plan.md` §1/§1.1/§2/§3 Phase E1;
`GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §2;
`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §4/§6.1/§6.2/§6.3/§7.7/§8
(**controlling**); `GwzM5-8R2DInterfaceFreeze.md` §3.5.

---

## VERDICT: **GO.** No ESCALATE.

Highest grade: **P3**. Six findings, all P3, none blocking, none
requiring a remediation round. The C-2 / §6.1 adjudication is **resolved
in favour of the object as built** — see §2.

---

## 0. Gate tails, verbatim

Host: darwin (arm64), worktree at `4a0b01a`.

```
$ cargo check --all-targets
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 6.53s

$ cargo fmt --all -- --check
(no output; exit 0)

$ cargo clippy --all-targets --all-features
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 14.09s
CLIPPY_DONE
[exited with code 0]

$ cargo test --lib -p gwz-core tests_cleanup_matrix
test result: ok. 8 passed; 0 failed; 0 ignored; 0 measured; 1590 filtered out; finished in 7.07s

$ cargo test --lib -p gwz-core namespace::tests_fault_matrix
test result: ok. 4 passed; 0 failed; 0 ignored; 0 measured; 1594 filtered out; finished in 4.12s

$ cargo test --lib -p gwz-core admission::tests_fault_matrix
test result: ok. 4 passed; 0 failed; 0 ignored; 0 measured; 1594 filtered out; finished in 6.26s

$ cargo test --lib -p gwz-core interface_tests::fault_expected_keys
test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1592 filtered out; finished in 0.03s

$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
(exit 0)
```

Two further gates run because the per-OS pins claim them:

```
$ cargo test --lib -p gwz-core checked_artifact::
test result: ok. 408 passed; 0 failed; 0 ignored; 0 measured; 1190 filtered out; finished in 43.07s

$ cargo test --lib -p gwz-core -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::
test result: ok. 932 passed; 0 failed; 1 ignored; 0 measured; 665 filtered out; finished in 54.08s
```

**408 and 932 are exactly the darwin pins this commit writes.** The
darwin marker is EXECUTED, not derived, and I re-executed it.

Digest-necessity probe (mirror tree outside the repo, the base digest
restored into a scratch copy of the checker; a control copy of the
shipped checker in the same mirror passes):

```
--- control: unmodified copy in the same mirror ---
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
--- probe: base digest restored ---
checked-artifact boundary: failed
- protected source tree changed: checked_artifact/capability/pre_catalog.rs
```

So the `pre_catalog.rs` tree re-pin is against **actual bytes**, is
necessary, and is not a no-op.

---

## 1. Axis 1 — key semantics, row by row against §2.3

Every one of the eleven announced boundaries names the durable state its
§2.3 row binds. Verified by reading the site, not the comment.

| # | Key | Site placement verified | Verdict |
| --- | --- | --- | --- |
| 1 | `worklist_scratch_create` | hit fires immediately after `open_with`, **before** `set_len(0)`/`seek` and before `write_all` | ✔ "open for write, nothing written" |
| 2 | `worklist_scratch_write` | after `write_all`, before `sync_all` | ✔ "written, not yet flushed" |
| 3 | `worklist_scratch_flush` | after `file.sync_all()`, **before** `sync_directory_edge` | ✔ "bytes durable, the dirent may not be" |
| 4 | `worklist_publish` | first statement of `observe_cleanup_worklist_row`, before any read | ✔ post-edge, "durable and unread" |
| 5 | `worklist_reobserve` | after `read_cleanup_worklist` → `read_and_bind_cleanup_worklist`, which refuses a worklist that does not `matches_reservation` | ✔ bounded re-read + reservation refusal |
| 6 | `source_reobserve` | after `observe_cleanup_alias(source_leaf)` | ✔ fact observed into `CleanupPhysicalFactV1` |
| 7 | `destination_reobserve` | after `observe_cleanup_alias(destination_leaf)` | ✔ the pair `classify` consumes |
| 8 | `alias_retire` | first statement of `observe_cleanup_retirement`, after the caller's `retire_exact` | ✔ post-edge, "durable and unread" |
| 9 | `retired_alias_reobserve` | after the `retired != Exact(expected)` refusal | ✔ "proved to be the fingerprint the row named" |
| 10 | `row_complete` | after `sync_directory_edge(&action.handle, …)` | ✔ the §2.3 row's own prescribed shape |
| 11 | `completion_reobserve` | after every row classifies `Complete` | ✔ whole-worklist proof |

`DurableLeafFingerprintV1::new(identity, len, sha256)` is built from the
observation's own `identity`/`bytes` — identity, length and sha256, as
§2.3 row #6 requires.

**DECISION C-1 — honoured, and the `namespace.*` matrix still fires.**
`ActionNamespaceEdgeV1` is **unchanged** in this diff: no `Cleanup`
variant was added, so nothing is silenced. The drive's retirement path is
`ActionNamespace::retire_exact` → `validate_operation` (which calls
`revalidate_action_directory`, announcing `namespace.parent_revalidate`)
→ `HostActionNamespaceV1::retire_exact` → `execute` → `execute_edge(
ActionNamespaceEdgeV1::Retire, …)`, which fires `faults()[0..3]` =
`namespace.retirement_reserve` / `pre_retire_reobserve` / `retire_exact`
/ `retired_reobserve` on a **single unconditional path**. The worklist
publish likewise fires the four `Publish` keys. `cleanup.alias_retire` is
the post-edge state, in `managed_bootstrap.prior_generation_retire`'s
shape. The `namespace.*` family's `Executed` claim is unweakened, and its
own matrix is green (4 passed, above).

**DECISION C-3 — honoured, in its E0.2b-simplified form.**
`write_cleanup_worklist_scratch` is a structural clone of
`write_managed_intent_scratch` (`managed_mutation.rs:1064-1101`),
including the record-bound pre-check, `durable_write_options(create_new)`,
the `set_len(0)`+`seek` rewrite arm, the parent flush, and the post-write
re-read proof `observed.bytes != bytes`. No slot is minted; no ordering
condition is claimed. I re-ran the addendum §4 grep on this tree:
`BaseActionSlotV1::RecordScratch` had **zero write paths** before E1 —
the three `record.scratch_*` sites are all in
`authority_record_binding.rs`, which takes
`BaseActionSlotV1::AuthorityScratch` at `:486` (re-read and confirmed).
The slot's first and only user is this worklist.

---

## 2. Axis 2 — **THE ADJUDICATION: C-2's fallback clause vs addendum §6.1**

### 2.1 The arithmetic, measured independently

| Fact | Measured |
| --- | --- |
| `namespace_mutation.rs` at base `8597d32` | 414 lines |
| at `4a0b01a` | **697 lines** |
| `git diff --numstat` | **+285 / -2** (net +283) |
| C-2's fallback trigger | "above ~250 lines" — **fired** |
| Program cohesion trigger (ConsumerCheckpoint §11 `:324-326`) | "A file approaching 1,000 lines triggers a cohesion review; a **new owner** targets fewer than 500 lines" |

So: C-2's own local trigger fires; the **program's** trigger does not, and
the sub-500 target is written for a *new owner*, which this file is not.
At 697 the file sits at 70% of the program trigger.

Ranking within `provider/` (production files only), measured:

```
managed_mutation.rs        1251
interior.rs                 866
directory_mutation.rs       742
authority_record_binding.rs 729
namespace_mutation.rs       697   <-- fifth
leaf_observation.rs         675
```

(`namespace/host.rs` = 855.) The commit's claim "is not the subsystem's
largest" is **true**; its enumeration is incomplete — see finding F2.

### 2.2 Is the fallback implementable, and what does it actually cost?

Implementable: **yes.** A `cleanup_mutation.rs` taking
`&RetainedActionNamespaceV1` would use the `pub(super)`
`RetainedActionNamespaceV1::handle()` accessor and its own free
`observe_regular_file`, exactly as `managed_mutation.rs:1194` does. No
widening of the sealed capability's private surface is needed. So the
fallback is **not** blocked on mechanism.

It is blocked on exactly one thing, and I verified the mechanism myself:
`the_declared_injection_sources_are_every_production_source_holding_sites`
(`fault_expected_keys.rs:744-758`) rescans `src/checked_artifact` for
production sources naming `CheckedArtifactFaultKeyV1` and asserts **set
equality** with `FAULT_INJECTION_SOURCES`. A new production file under
`provider/` holding `cleanup.*` sites would therefore have to be declared
or the gate fails closed — moving the declared count **9 → 10 at E1**.

That contradicts CONTROLLING text in two places:

- §6.1: *"**Exactly one new file. The count moves once, at E2: 9 → 9 → 10
  → 10.**"* — with the E1 row of its own table reading
  "`namespace_mutation.rs` — already declared | New declared file? **no** |
  Count after **nine**", written *by name against DECISION C-2*.
- §6.3's amended §2.5, which is E1.2's binding duty list: *"the
  `FAULT_INJECTION_SOURCES` **doc-comment** extension … — **and no
  inventory count move (§6.1)**"*.

### 2.3 RULING

> **Keeping all eleven sites in `namespace_mutation.rs` is the CORRECT
> resolution of the pair's internal inconsistency. The fallback must NOT
> be taken at E1, and §6.1 must NOT be re-baselined here.**
> Grade: **no finding against the object.** The train's own flag of the
> conflict is graded **P3-informational** (correct, complete, and the
> right thing for an interior step to do).

Grounds, in order of weight:

1. **Precedence is unambiguous and specific.** The E0.2b addendum is
   controlling (plan §1.1; the freeze's own 2026-08-27 §3.5 activation
   record: "as amended at E0.2b … addendum controlling"). §6.1 is not a
   general rule that happens to collide with C-2 — it is the addendum
   **re-deciding this exact routing question**, with C-2 named in the
   table cell, after C-2's fallback clause was already on the record. The
   later, more specific, controlling statement wins.

2. **The two instruments are not of equal kind.** C-2's ~250 is a local
   authorial heuristic derived from an estimate the same sentence
   concedes ("the cleanup additions are ~120"). §6.1 is a correction to a
   **machine-enforced inventory** in a defect class this program has
   already paid for once — the addendum cites it: "The E0.2 text
   instructed E1.2 to land a false inventory edit under RemPlan §10's
   duty; that is the same defect class the freeze already graded once and
   corrected in round 2 (freeze `:943` … `:955-958`, 'the recorded
   inventory was false and the reserved-family scan was porous')."
   Trading a false inventory for 283 lines of cohesion pressure is the
   wrong trade.

3. **Cost asymmetry.** Cohesion pressure at 697 is relievable by any
   later step at any time, at zero inventory cost, in a file no pin
   freezes. Taking the fallback now would force a freeze-adjacent
   correction (§6.1's "exactly one new file", §6.3's E1.2 duty clause,
   and freeze `:525-532`'s "nine") **inside an interior, single-axis
   review step** — precisely the class of edit the plan's §2 tiering
   reserves for a dual.

4. **The program's own cohesion instrument is not tripped.** 697 < 1,000,
   and the sub-500 clause governs new owners.

**Forward note, not binding on E1 and offered to the E2/E3 owner.**
DECISION T-C′ routes terminal keys #1-#5 into this same file at E3. At
this train's density (285 added lines for 11 keys ≈ 26/key) that lands
`namespace_mutation.rs` near 820-830 — still under the trigger, but
visibly approaching it. If a `cleanup_mutation.rs` (or a
`terminal_mutation.rs`) is ever wanted, the cheapest moment is **E2**,
where §6.1's count is *already* moving and E2.3 already owes the dict's
dated deliberate-extension comment; widening "exactly one new file" to
two there is an amendment to a number in motion rather than one invented
by an interior step. Recorded so the question is inherited rather than
re-derived.

---

## 3. Axis 3 — the two E1-STATED semantics: grounded, not invented

### (a) The source row each alias retires out of — **GROUNDED**

The derivation added at `namespace/mod.rs` (`cleanup_retirement`) is
`Source → SourcePayload`, `Goal → GoalPayload`, `Authority → Authority`.
I verified it against production facts, not against the comment:

- `coordinator/schedule.rs:32-42` (`derive_new_reservation`) is real and
  derives exactly four masks: `Observe`/`Replace`-with-equal-goal →
  `ProofOnly`; `ParentOnly` → `0`; `Replace` with `expected == Missing` →
  `0b110`; `Replace` with `expected == Exact{..}` → `0b111`; `Remove` →
  `0b101`.
- `CleanupAliasV1::bit()` (`protocol/cleanup.rs:24-30`) is
  `Source=1, Goal=2, Authority=4`.
- The masks therefore say: no expected leaf ⇒ **no `Source` alias**; no
  goal leaf (`Remove`) ⇒ **no `Goal` alias**; `Authority` always. That is
  the pairing, read off the coordinator's own derivation.
- Independent confirmation that `SourcePayload`/`GoalPayload` are the
  request's expected/goal leaves:
  `authority_record_binding.rs:48` ("the payload slots this owner
  observes are `SourcePayload` and `GoalPayload`") and `:211`/`:220`,
  which stream the source into `SourcePayload` and the goal into
  `GoalPayload`.
- The destination names corroborate: `RetiredSourceAlias` /
  `RetiredGoalAlias` / `RetiredAuthorityAlias`.

Nothing is minted: both sides come from the frozen `BaseActionSlotV1`
vocabulary through the existing `action_destination`.

**Stated where a future reader will meet it:** yes, twice — an inline
comment at the derivation itself (`namespace/mod.rs`) and the
`CleanupRetirementDestination::source_leaf()` doc in `roles.rs`. Both
cite `coordinator/schedule.rs:39-49`.

### (b) The 16 KiB read bound — **GROUNDED**, consequence accurate

- `ProtocolRecordKindV1::CleanupWorklist.max_bytes()` = `16 * 1024`
  (`protocol/codec.rs`). Real.
- `execute_edge` reads its source through `observe_regular_file(leaf,
  kind, …)`, capped by `kind.max_bytes()` and never by the object's own
  length — the doc on `retain_source` says so and the code does it.
- `leaf_observation.rs:12-14` really does say "This file names no
  protocol record kind, so a payload bound can never be a record bound",
  so the frozen vocabulary genuinely carries no record kind for a payload
  leaf. The "one bound for all three" reasoning is sound.
- **The consequence is accurate.** An over-bound row does **not** become
  `CleanupPhysicalFactV1::Other`; `observe_cleanup_alias` calls
  `observe_regular_file`, which returns
  `CheckedFsError::ambiguous(label, "namespace object exceeds its frozen
  record bound")`. A typed refusal, exactly as stated — the row refuses
  rather than retires.
- **The statement of record is load-bearing on the retirement path.** I
  traced it: `retirement.source_bound()` → `retain_scheduled_source(leaf,
  kind)` → `retain_source` → `execute` (`host.rs:576-582`) passes
  `retained.kind` into `execute_edge`. So the bound really does cap the
  retirement's in-window re-read.

**Is the two-place duplication acceptable?** It is a defect, but a small
one — **finding F1, P3**. See §7.

---

## 4. Axis 4 — the `AdmittedActionV1` duty ([P2-4] as amended)

Verified by reading the fixture, not the comments.

**First attempt — a real admission.** `CleanupFixture::new` runs, inside
a real `with_catalog` session (real lease, real `recover_or_create`
through the sole sealed catalog owner):

```rust
ActionAdmissionOwnerV1::from_retained_catalog(catalog).resume_or_admit(&reservation)
```

That is **byte-for-byte the call the production coordinator makes** —
`coordinator/execution.rs:138-140`, inside
`ScheduledCheckedActionV1::admit`:
`ActionAdmissionOwnerV1::from_retained_catalog(catalog).resume_or_admit(&self.reservation)?`.
The cure's citation is accurate.

**The real token is consumed by attempt #1 and only attempt #1.** It is
held as `admitted: RefCell<Option<AdmittedActionV1>>` and
`CleanupFixture::attempt()` does
`self.admitted.borrow_mut().take().unwrap_or_else(|| handoff(&self.reservation, &self.identity))`.
So the first attempt of every fixture — which in the interruption rows is
the *crashed* attempt — drives the real admitted action, and every
restart rebuilds through `protocol/admission/test_support.rs` via
`tests_fault_matrix::handoff` → `admit_observed_action`. Exactly the
cure's shape.

**The deviation is inherited and cited.** The module header (lines 29-39)
quotes `namespace/tests_fault_matrix.rs:20-35`'s reasoning verbatim in
substance — the action directory is no longer *exact* once a namespace
edge has published its first row (`protocol/admission/owner.rs:29-38`) —
and names **"item 6 of the Phase 3 settle docket"**. I re-read
`tests_fault_matrix.rs:20-39` and the citation is faithful, including the
"`retain_action_namespace` still fails closed if the reconstructed
identity is not the resident one" safety clause. No new owner is minted.

**The reservation is coordinator-derived for the three-alias rows.**
`coordinator_reservation()` builds a real `CheckedActionRequestV1` via
`synthetic_leaf_request` (a pre-existing `#[cfg(test)]
pub(in crate::checked_artifact)` helper — **not** widened by this commit)
and runs it through `derive_new_reservation`, asserting `Reserve(..)`.
Nothing hand-built.

**One declared, necessary deviation — F6, P3-informational.** The
one-alias fixtures state their schedule directly
(`ActionCapacityReservationV1::new(..)`), because §2.5 *requires* a
one-alias row for the single-crossing half and the coordinator can only
derive `0b111`/`0b110`/`0b101`/`0` — there is no coordinator-derivable
one-alias mask. Those fixtures still admit through the same real
`resume_or_admit` seam. §2.4's stricter "not through a hand-built
reservation" phrasing is superseded by addendum §8's controlling
restatement, which binds the *admission*, not the reservation's
provenance. The module header declares the choice and its reason. **No
action.**

---

## 5. Axis 5 — matrix honesty

**`CLEANUP_MATRIX: [Fault; 11]`** — literal, and
`reconcile_executed_keys()` proves it equals the vocabulary's own
`cleanup.*` set by projecting `Fault::all()`. All 11 keys are present in
`fault_v1.rs:178-188` and unchanged.

**Both variants, real durable edges.** `TargetVariantV1::{Workspace,
GitDirectory}`, eight `#[test]` entry points (4 shapes × 2 variants).
`Fixture::new` does a real `git2::Repository::init`; the Git arm resolves
`commondir()` and leases through
`CatalogLeaseTargetRequestV1::repository_common_git_directory`, so the
two variants write to genuinely different durable roots. Every
`attempt()` re-enters `with_catalog`, re-acquiring the lease and
re-recovering the catalog — a real fresh-process shape, not a
re-used handle. The interruption is a `panic!` caught by
`catch_unwind`, the house form; `hit()` does `slot.take()` **before**
invoking the callback, so the arm is consumed exactly once.

**The ONE-ALIAS row for single-crossing** — present, and the probe is the
`run_single_crossing_probe` shape reproduced faithfully: crash once,
**re-arm with a panicking callback**, require the next drive to settle,
then assert `take_armed_fault()` (which is `slot.take().is_some()` — true
iff the arm never fired). The claim is genuine, and I verified *why* it
holds from `classify_cleanup_row` (`protocol/cleanup.rs:383-401`): a
crash at any of `alias_retire` / `retired_alias_reobserve` /
`row_complete` leaves `(source=Missing, destination=Exact(expected))`,
which classifies `Complete`, so the next drive routes past all three.
`run_one_alias_convergence` separately proves the same row converges
across all eleven keys, so the probe is not proved only by an absence.

**The 12-round repeated-boundary subset with stated reasons** —
`REPEATED_CRASH_ROUNDS = 12`, `REPEATED_BOUNDARIES: [Fault; 8]`, with a
per-boundary reason in the const's doc. I checked each reason against the
drive:
- `source_reobserve` / `destination_reobserve`: step 1 runs on every
  attempt for every reserved alias. ✔
- the three scratch keys: step 2 is gated on
  `!scheduled_row_is_resident(worklist_leaf)`, and a crash at any of the
  three leaves the worklist absent. ✔
- `worklist_publish` / `worklist_reobserve`: step 3 runs
  unconditionally on every attempt. ✔
- `completion_reobserve`: step 5 runs unconditionally. ✔

**Reconciliation partitioning (8 + 3 = 11)** — proved, and proved
*exactly*: `reconcile_executed_keys` chains `REPEATED_BOUNDARIES` and
`SINGLE_CROSSING_BOUNDARIES`, sorts, and asserts equality with the sorted
vocabulary projection. A duplicate in one list plus an omission in the
other would change the multiset and fail. Genuine partition proof.

**The deliberate deviation from the namespace matrix's cardinality
assertion — SOUND, and in fact stronger.** The substitute is three
assertions:
1. `observed ⊆ scheduled_names()` per round — a whitelist built from
   `ActionSlotV1::name` (the production derivation) over the reservation's
   own alias mask;
2. the sorted child vector is **identical** across all twelve rounds;
3. `rows.len() <= settled_rows.len()`.

(1) is a *direct* proof of the no-fresh-retry-name property the rule
names; the namespace matrix's cardinality equality is only an indirect
proxy for it and would not catch a retry name that replaced an expected
one. The stated reason for the deviation is also correct: I confirmed
from the measured output that the interrupted directory legitimately
holds 4 children at `source_reobserve` against a settled 5, so equality
genuinely cannot transfer. And the whitelist is a real cross-check, not a
restatement: the drive uses the **production** names, so a divergence
between `alias_source_slot` (test) and `cleanup_retirement`'s
`source_slot` (production) makes the drive write a name outside the
whitelist and fails here. **Accept the deviation.**

**Measured evidence lines** (excerpt, `--nocapture`):

```
cleanup.completion_reobserve | workspace | rounds=12 | slots-stable=yes | rows=[
  "action-<d>-cleanup-worklist-v1", "action-<d>-reservation-v1",
  "action-<d>-retired-authority-alias-v1", "action-<d>-retired-goal-alias-v1",
  "action-<d>-retired-source-alias-v1"] | converged=yes
cleanup.alias_retire | workspace | single-crossing=yes | restart=settled | re-crossed=no
```

---

## 6. Axis 6 — companions, and Axis 7 — no weakening

| Duty | Verified |
| --- | --- |
| `FAULT_FAMILY_ACTIVATION` cleanup row `Reserved(…)` → `Executed("R2-E phase E1 (cleanup worklist lifecycle)")`, count still 11 | ✔ same commit |
| `only_the_families_with_executed_matrices_are_executed_today` gains `"cleanup"` | ✔ (the explicit two-place literal edit the test's doc demands) |
| `FAULT_INJECTION_SOURCES` **doc-comment** extension, **no count move** | ✔ — the list still holds nine entries; the added prose explicitly states the count does not move and names E2 as the single mover |
| `CATALOG_PUBLICATION_CALL_COUNTS` unmoved | ✔ **machine-verified**: `git diff … | grep -c CATALOG_PUBLICATION` = **0**, and the checker's per-file counts + set-equality assertion pass (exit 0). The matrix header also states the confirmation §6.2(a) demands |
| `PROTECTED_SOURCE_TREE_DIGESTS["…/pre_catalog.rs"]` re-pinned | ✔ against actual bytes — see the digest-necessity probe in §0 |
| Git-directory route statement (§7.7) | ✔ present, and correct: the matrix takes **neither** route, because every cleanup boundary is inside the retained action directory; `retain_managed_parent_at_for_test` is untouched in this diff, and E4.2's ownership of the door and of the workspace-root binding is restated |
| R4b-G per-OS markers | ✔ darwin **400 → 408 EXECUTED** (I re-executed: 408 passed); linux **410 → 418** carries the driver's own pre-existing `FIRST-DISPATCH-EXPECTED` marker (the convention exists at `run_r4bg_aggregate_gates.py:79`), is explicitly "NOT measured by this step", and states the override rule "if the Linux battery reports another number, that measured number wins". Remainder pins unmoved, and I re-measured the darwin remainder at 932 |
| Driver prose tense fix | ✔ "counts toward that gate **is** 400" → "**was** 400 … at the split §3.1 records", plus the clarifying clause that the four §3.1 numbers are historical and `_fault_count` holds the live pins. Necessary, since the 400 would otherwise contradict the new 408 |
| Census 165, cleanup 0/11 → 11/11, no key minted or retired | ✔ `EXPECTED_KEY_COUNT = 165` unchanged; `fault_v1.rs` has **no diff** in this commit |
| No `protocol/generated.rs` diff | ✔ zero |
| Driver-holds-zero | ✔ `grep -rn "fault_v1::hit" src/checked_artifact/namespace/` returns **nothing**. `host.rs` gains five entry points and zero sites, exactly as it holds none for E12/E13/E15/E16 and the Step-3.1b intent lifecycle |
| The one visibility widening | ✔ **exactly one**: `Fixture::action_children` private → `pub(super)`. I diffed the base: `Fixture`, `TargetVariantV1`, `with_catalog`, `handoff`, `slot_leaf`, `action_directory`, `path` were **already** `pub(super)`. `pub(super)` is the minimum (the new module is a sibling under `namespace/`). No production visibility widened |

**Whole test/checker surface diffed — nothing weakened.** No test
deleted, no assertion removed, no allowlist widened. The only deletions
in the entire commit are import lines, one enum-variant line replaced by
its successor, one function signature widened, and the two docstring
sentences rewritten. `reserved_fault_families_have_no_injection_sites_before_their_package`
is untouched; its `Executed` arm now *requires* cleanup to have sites,
and it passes.

**Precedent conformance of the entry-point validation.** The three
observation/scratch entry points do a binding-only check
(`validate_cleanup_retirement`) rather than `validate_operation`. This is
**not** a weakening: it is exactly the Step-3.1b shape —
`validate_generation_slots` (`host.rs:339-349`) is also binding-only, and
the two durable *renames* (the publish and each retirement) both route
through `validate_operation`, which calls `revalidate_action_directory`
and announces `namespace.parent_revalidate`. §2.4 item 2's "every cleanup
edge passes `validate_operation`" is satisfied for the edges §2.1 names.
The scratch write is a P2 write-through, like
`write_managed_intent_scratch`, not a namespace edge. Two of the five
entry points do not even need a binding check, because they derive their
leaf from `self.publish_destination(role)` rather than from the caller —
slightly tighter than the precedent.

---

## 7. Findings

All **P3**. None blocking. No remediation round required.

**F1 — [P3] The 16 KiB alias bound has two independent statements and no
machine tie.**
`CleanupRetirementDestination::source_bound()` (`namespace/roles.rs`) is
declared the statement of record, and it *is* load-bearing on the
retirement path. But the fact-observation path states the same bound
independently: `observe_cleanup_alias`
(`capability/pre_catalog/provider/namespace_mutation.rs`) writes
`ProtocolRecordKindV1::CleanupWorklist` as a literal and links to
`source_bound` only in prose ("whose statement of record is
`CleanupRetirementDestination::source_bound`"). If a later step (E4 is
named as the first to place real payloads in these rows) changes
`source_bound()`, the observation and the retirement diverge silently and
the stated consequence — "a row this family could not retire through
`execute_edge` is refused when it is observed rather than fingerprinted
as though it could be" — is falsified with no gate firing. In a
subsystem whose norm is machine-enforced pins, a prose-only coupling is a
defect. Blast radius is bounded (a typed refusal or an unusable
fingerprint, never corruption), and the two agree today, hence P3.
*Cure, one parameter:* `host.rs::observe_cleanup_row_facts` already holds
the `retirement`; thread `retirement.source_bound()` through
`observe_cleanup_row_facts` into `observe_cleanup_alias`. Then the
statement of record is the only statement. (`observe_cleanup_completion`
would need the bound carried in its `rows` tuple or alongside it.)

**F2 — [P3] The commit's cohesion ranking is incomplete.**
It cites `host.rs` 855, `interior.rs` 866, `managed_mutation.rs` 1,251 as
the files larger than 697, omitting `directory_mutation.rs` 742 and
`authority_record_binding.rs` 729 — also larger, and both inside
`provider/`. `namespace_mutation.rs` is the **fifth** largest production
file in that directory, not the third. The substantive claim (under the
1,000-line trigger, not the subsystem's largest) is unaffected and true.
Record-only defect; the corrected ranking is carried in §2.1 above and
should be carried into the E1.3 landing record rather than by rewriting
the commit.

**F3 — [P3] `run_r4bg_aggregate_gates.py`'s `_fault_count` docstring now
contradicts itself.**
Line ~73 still reads *"Both values are EXECUTED, never derived from
intent"* while the R2-E block immediately below derives linux 418 from
`+8` and marks it FIRST-DISPATCH-EXPECTED. The train fixed the adjacent
instance of exactly this staleness (the module docstring's "is 400" →
"was 400 … at the split §3.1 records") and left this one. A reader who
stops at the first paragraph is misled about the linux pin's provenance.
*Cure:* one qualifying clause on the older sentence pointing forward to
the E1 block.

**F4 — [P3, strengthening] DECISION C-1's ground is structural, not
asserted.**
C-1's entire justification is that routing through the Step-2.2 backend
keeps the four `namespace.*` retirement keys firing rather than silencing
them. I verified this holds structurally (unchanged
`ActionNamespaceEdgeV1`, single unconditional path through
`execute_edge`), but the cleanup matrix does not *assert* it. A three-line
probe — arm `NamespaceRetireExact`, drive the one-alias fixture, assert
it fired — would make C-1's ground machine-held. Not owed by §2.5 or
§6.3's duty lists, so this is an available strengthening rather than a
missed duty.

**F5 — [P3, informational] Keys #8/#9/#10 leave byte-identical durable
trees under process-stop injection.**
`observe_cleanup_retirement`'s `sync_directory_edge` is a deliberate
re-flush of a directory `execute_edge` already flushed — the code says so
("the resident twin of the unannounced `sync_directory_edge` at the tail
of `execute_edge`"), and §2.3 row #10 prescribed exactly this shape. The
consequence is that a crash at `alias_retire`, at
`retired_alias_reobserve` and at `row_complete` all leave the same durable
state, so those three rows converge trivially. This matches the executed
`namespace.*` house form, where post-edge observation keys likewise name
proof progress rather than distinct filesystem states, and the
distinguishing power the flush *does* carry is only observable under real
power loss — which plan §1 places in R2-F. Recorded so the E7 settle is
not surprised. **No action.**

**F6 — [P3, informational] The one-alias fixtures state their reservation
directly.** See §4. Declared in the module header, forced by a genuine
conflict between §2.4 and §2.5, compliant with addendum §8's controlling
restatement, and admitted through the real seam regardless. **No action.**

---

## 8. E1's OWN completion shape — the authoritative statement

Requested by the lane owner for the cross-family slot-home check against
`terminal.*` keys #1-#2. This is **E1's shape only**; no judgement is
offered on the terminal train.

**What `cleanup.completion_reobserve` (key #11) accepts.** For an
admitted action whose schedule reserves alias set `M ⊆ {Source, Goal,
Authority}`, `observe_cleanup_completion` fires **only** when all of the
following hold simultaneously:

1. The `BaseActionSlotV1::CleanupWorklist` row
   (`action-<digest>-cleanup-worklist-v1`) is resident, is a canonical
   non-symlink regular file, is **≤ 16 KiB**, decodes as a
   `CleanupWorklistV1`, and `matches_reservation` against the **resident**
   `ActionCapacityReservationV1` (four digests plus the alias set).
2. The worklist's row set is exactly `M`.
3. For **every** alias `a ∈ M`, with
   `S(a) ∈ {SourcePayload, GoalPayload, Authority}` and
   `D(a) ∈ {RetiredSourceAlias, RetiredGoalAlias, RetiredAuthorityAlias}`:
   - **`S(a)` is ABSENT** from the action directory
     (`CleanupPhysicalFactV1::Missing`), and
   - **`D(a)` is PRESENT** as a canonical non-symlink regular file
     of ≤ 16 KiB whose `DurableLeafFingerprintV1` — durable identity,
     byte length, sha256 — equals the worklist row's `expected`
     fingerprint **exactly** (`CleanupPhysicalFactV1::Exact(expected)`).

   Any other `(source, destination)` combination classifies
   `CleanupResolutionV1::Ambiguous` (`protocol/cleanup.rs:383-401`) and
   `observe_cleanup_completion` returns a typed refusal. In particular
   `(Exact, Missing)` classifies `Retire`, not `Complete`, so a
   half-finished worklist cannot satisfy key #11.

**The measured residual directory.** For the three-alias schedule
(`Replace` over an exact expected leaf, mask `0b111`), the settled action
directory contains **exactly five rows**, identical on both target
variants — captured from the matrix's own evidence line:

```
action-<digest>-cleanup-worklist-v1
action-<digest>-reservation-v1
action-<digest>-retired-authority-alias-v1
action-<digest>-retired-goal-alias-v1
action-<digest>-retired-source-alias-v1
```

and for the one-alias schedule, exactly three
(`…-cleanup-worklist-v1`, `…-reservation-v1`,
`…-retired-source-alias-v1`).

**Gone at completion, by construction:**
`action-<digest>-source-payload-v1`,
`action-<digest>-goal-payload-v1`,
`action-<digest>-authority-v1` (consumed by the three retirement
renames), and `action-<digest>-record-scratch-v1` (consumed by the
worklist publish rename).

**The tension, stated neutrally for the lane owner's cross-check.**
Amendment §4.3 gives `terminal.authority_reobserve` (#1) as "the action's
`Authority` row … has been re-read bounded" and
`terminal.payload_reobserve` (#2) as "the `SourcePayload` / `GoalPayload`
rows … have been re-read". E1's completion requires all three of those
slots to be **Missing**. So a terminal drive that re-reads those live
slots *after* a completed cleanup worklist reads absence, while a
terminal drive that re-reads them *before* cleanup completion cannot also
satisfy `terminal.cleanup_reobserve` (#3), which §4.3 joins directly to
E1's key #11. The two constraints are, on E1's shape as built, mutually
exclusive on the same slots at the same instant. E1 takes no position on
the resolution — the retired-alias rows carry byte-identical content
under different names, so a resolution exists; choosing it is the E3
owner's, and the cross-family check is the lane owner's at landing. This
section is offered as the authoritative statement of E1's half.

---

## 9. What E1 owed and delivered (§6.3's amended §2.5, item by item)

| §6.3 duty | Status |
| --- | --- |
| `CLEANUP_MATRIX: [Fault; 11]`, every key interrupted+restarted+converged, both variants | ✔ |
| Twelve-round repeated-boundary rows over a declared subset | ✔ 8 boundaries, reasons stated |
| Single-crossing classification declared per row, `run_single_crossing_probe` shape, on a **one-alias** row | ✔ |
| Matrix-to-fixture reconciliation in the `reconcile_executed_keys` shape | ✔ plus an exact-partition proof |
| `FAULT_FAMILY_ACTIVATION` row edit | ✔ same commit |
| `FAULT_INJECTION_SOURCES` doc-comment extension, **no count move** | ✔ |
| Git-directory route statement (§7.7) | ✔ neither route; door untouched |
| R4b-G per-OS fault markers | ✔ darwin executed, linux marked |
| `PROTECTED_SOURCE_TREE_DIGESTS[…pre_catalog.rs]` re-pinned | ✔ verified against bytes |
| Stated confirmation that `CATALOG_PUBLICATION_CALL_COUNTS` does not move | ✔ stated and machine-verified |

Nothing owed is missing.

---

*Filed by the E1 interior reviewer, 2026-08-27. Single-axis, peer-blind.
Verdict **GO**, no ESCALATE. Six P3 findings, none blocking; the C-2 /
§6.1 conflict is ruled in favour of the object as built, with the
forward note in §2.3 handed to the E2/E3 owner.*
