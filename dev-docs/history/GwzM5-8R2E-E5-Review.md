# R2-E Phase E5 — interior single-axis review

**Date:** 2026-08-28
**Axis:** interior single-axis, peer-blind (no context shared with the builder)
**Tree under review:** gwz-core `cf4213a` (branch `e5-archive`)
**Base:** `c11c5ef` (= origin/main)
**Intermediate:** `221cd89` (E5.1), reviewed and gated separately
**Reviewer worktree:** `.../scratchpad/e5-review-worktree`, detached, created from
`cf4213a`; no tracked file mutated, nothing committed, the `e5-archive` ref
untouched (`git rev-parse e5-archive` → `cf4213a…` at close).
**Host:** Darwin 25.5.0 / arm64; Python 3.13 (`python3.13`).

**Verdict: GO-WITH-CONDITIONS.** One [P1], four [P2] (two escalating), six [P3].
The train's substance is sound: it drives the production `adapt_open` and the
production archival act, it does not weaken a single assertion, and its
deviations are flagged honestly. The [P1] is an arithmetic defect in the
forward-stated M4 map pin that would leave the battery red *after* the
remediation it names; it is a one-line fix, and it is measured below, not
argued.

---

## 1. The obligation ledger, re-derived

Derived independently from: plan `GwzM5-8R2E-Plan.md` §3 Phase E5 as amended by
§1.1 (the O8-denominator row, `:127-133`, and the L6 one-commit rule);
`GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §6.1/§6.3/§6.4;
`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §8 [P2-2] cure and Appendix A;
`GwzM5-8A1ActivationRecord.md` §6 (L6); `GwzM5-8R4bG-Evidence.md` §12.3/§12.4/
§12.7/§12.8/§12.9; `GwzM5-8R0Inventory.md` §4 and §5.2.

**My derivation of "the ten progress shapes"** (independent of the builder's
list): §12.3 Table A carries 14 UNBOUND progress rows — A-EXECUTING, A-AWAITING,
A-HALTED, A-PRE-PRESERVE, A-PRE-ROLLBACK, F-BASELINE, F-MARKER, F-LOCK,
G-VERIFYING, H-PRESERVING-CANDIDATE, H-PRESERVING-PREFIX, I-EVIDENCE-ROLLBACK,
J-NO-PUBLICATION-UNBORN, K-COMPLETED-NOPUB-OPEN. §12.9 moves four (F-BASELINE,
F-MARKER, F-LOCK, J-NO-PUBLICATION-UNBORN). Remainder = ten, and it is exactly
the builder's ten. Denominators agree.

| # | Obligation | Source | Status | Evidence |
| --- | --- | --- | --- | --- |
| E5-1 | **10 registry rows** for the ten UNBOUND progress shapes | plan §1.1 `:127-133`; E0.2b §8 [P2-2] | **DEVIATED-WITH-RECORD — RATIFIED** | 9 rows in `dev-docs/GwzM5-8I2CompatibilityPredicates.json` `valid_unlisted_corpus`; `G-VERIFYING` clause-cited at `compatibility_unbound_v0.rs:526-561`. Adjudication §4.A. |
| E5-2 | **One** parametric `adapt_open` refusal test over every non-whitelisted shape | A1 record §6 L6; amendment §6.1 | **MET** | `compatibility_unbound_v0.rs:481` `v0_unbound_progress_shapes_are_refused_by_adapt_open`; executed (E5.1 gate, §6). |
| E5-3 | Rows **and** test land **in one commit** (L6: "neither may land alone") | A1 record §6; amendment §6.1 `:1049-1051` | **MET** | Both in `221cd89`. `git show 221cd89` file list = 6 files; the JSON's +10 lines are the 9 rows plus the comma-only re-add of `rollback/participant`. |
| E5-4 | The test must drive `adapt_open`, **not** the precheck walk | A1 record §6 L6, explicit | **MET** | Chain traced: `compatibility_unbound_v0.rs:494` → `compatibility_v0.rs:504-514` (`decode_production_v0` → `adapt_open_v0` → `assert_eq!(…ValidUnlisted)` → `atomic_upgrade_v0::assert_valid_unlisted`). Grep for precheck symbols over both new files: two **comment** hits only (`:7`, `:475`), zero calls. |
| E5-5 | Each row asserts its exact R0 §4 combination | §12.9's disposition-table shape; plan §3 | **MET, with [P3-4]/[P3-5]** | `assert_r0_combination` `:390-468`, run before the drive. All ten combinations are distinct — no relabeled duplicate *within* the ten (see [P2-1] for the cross-table one). |
| E5-6 | Compatibility-checker companion **in the same commit** | plan §3 E5.1; ritual 5 | **MET** | `check_merge_compatibility_predicates.py` +3 states and `compatibility_v0.rs`'s inventory pin, both in `221cd89`. |
| E5-7 | Scenario-map companion **in the same commit** | plan §3 E5.1 | **DEVIATED-WITH-RECORD** | Structurally impossible from a gwz-core commit: `check_m4_scenario_map.py:59` resolves `MAP_DOC = ROOT.parent/dev-docs/…`, i.e. the gwz-dev root repo. The driver carries a forward-stated DOC-PENDING marker instead. **The forward pin is wrong — [P1-1].** |
| E5-8 | **8 archive-corpus rows**, tier 1 executed per shape (SHA256 archived == SHA256 preceding open v0) | E0.2b §8 [P2-2]; amendment §6.3 tier 1 | **MET** | `archive_equivalence_v0.rs:317` `archived_v0_shapes_are_byte_preserved_from_their_open_records`; digest pair taken across the real archival act (§4.C). |
| E5-9 | **2 PENDING-FIXTURE rows**, carrier R2-F named, O8 clause **not** reported met | amendment §6.4 `:1147-1157` | **MET** | `AC-NOPUB-UNBORN`, `AP-PRESERVED`: both tiers `pending-fixture`, `test: null`, carrier names R2-F. Machine-enforced at `check_merge_compatibility_predicates.py:473-481` and runtime-enforced at `archive_equivalence_v0.rs:409-431`. |
| E5-10 | Tier 2 declared, no fake execution | amendment §6.3 tier 2 | **MET, with [P2-3]** | `owed` on all ten; `tier2.test` is `null` on every row and the checker forbids a test on an unexecuted tier. Carrier pre-emption: [P2-3]. |
| E5-11 | Archive rows live in a **standalone** corpus, cited **by clause**, never in the migration registry | amendment §6.3 "No registry rows for archive shapes" | **MET** | New top-level `archive_corpus`; `check_m4_scenario_map.py:144-147` reads only `fixture_corpus`+`valid_unlisted_corpus`, so the registry-row denominator is unaffected (measured: still 22). |
| E5-12 | §12.8 PARTIAL statement survives unchanged | amendment §6.4; §12.8 | **MET** | No gwz-dev/dev-docs file touched. Completeness-language sweep over every added line: every PARTIAL mention is protective (`archive_equivalence_v0.rs:36-42`, `:338`, `:385-387`). No upgrade language anywhere in docs, markers, comments, or test names. |
| E5-13 | Content-anchored citing rule | E0.2b Appendix A | **MET** | All five citations re-verified against the live contract (§5). |
| E5-14 | `gc_archived` production route rides **E4.4**, not E5 | amendment §6.4 `:1159-1164`; plan O8 | **MET (out of scope, untouched)** | Zero diff under `v1_lifecycle/archive.rs`. |
| E5-15 | No production source; no fault-key/census/wire surface | plan §4 non-goals; standing rituals | **MET** | §2. |

---

## 2. Scope verification

`git diff --stat c11c5ef..cf4213a` — 8 files, +1381 / −8.

```
 dev-docs/GwzM5-8I2CompatibilityPredicates.json     |  23 +-
 scripts/checks/check_merge_compatibility_predicates.py | 139 ++++-
 scripts/checks/run_r4bg_aggregate_gates.py         |  73 ++-
 scripts/checks/test_merge_compatibility_predicates.py  | 111 ++++
 src/workspace_ops/tests/g23.rs                     |   2 +
 src/workspace_ops/tests/g23/archive_equivalence_v0.rs   | 440 ++++++++
 src/workspace_ops/tests/g23/compatibility_unbound_v0.rs | 561 +++++++++
 src/workspace_ops/tests/g23/compatibility_v0.rs    |  40 +
```

- **No production source.** Everything under `src/` is inside
  `src/workspace_ops/tests/g23/`, which hangs off `#[cfg(test)] mod tests`.
  The two `g23.rs` lines are `mod` registrations.
- **`compatibility_v0.rs`'s +40 has no production reach and no weakening.** It
  is exactly: a 6-line dated doc comment on `operation_state` (`:530-535`);
  three match arms (`:543-545`); and 31 lines extending the expected inventory
  inside the existing `#[test] i2_runtime_binding_inventories_equal_the_registry`
  (`:641-677`). The load-bearing lines are **untouched and outside the diff**:
  the whitelist-closure assertion `compatibility_v0.rs:499-502` and
  `assert_ne!(record.state, OperationState::Finalizing)` at `:503`.
- **Zero fault-key/census/wire surface.** `git diff c11c5ef..cf4213a | grep -E
  "FAULT_|EXPECTED_KEY_COUNT|ACTIVE_WRITER_FLOOR|protocol/generated"` → no match
  (exit 1). Nothing under a `checked_artifact` protected tree; no protocol or
  generated file.
- **Removed lines: EIGHT, not seven.** The builder's report says "the only
  removed lines in the whole range: the seven pin/marker strings". There are
  eight. The eighth is the `rollback/participant` `valid_unlisted_corpus` row,
  re-added byte-identical apart from a trailing comma. Benign in substance;
  the claim as written is false — **[P3-1]**. The other seven are the pin and
  marker strings, and no test, assertion, or corpus row is deleted or weakened.

---

## 3. Commit split and per-commit greenness (Ritual 7)

**Split is clean.** `git show 221cd89` touches 6 files and contains zero
occurrences of `archive_corpus`, `archive_equivalence`, `ARCHIVE_SHAPES`,
`tier1`, `tier2`, `byte-preserved` (grep exit 1). `git show cf4213a`'s hits on
`valid_unlisted_corpus` / `compatibility_unbound` are diff **context** lines and
§12.9(c) quotations in comments only — no E5.1 content lands in E5.2.

Gate tails for `221cd89` at §6.2. Full lib partitions were not re-run at the
intermediate: the tip's three disjoint partitions are green, the intermediate
differs from the tip by two added `#[test]` fns in a new module plus checker
files, and `cargo check --all-targets` plus the g23 battery at the intermediate
cover the compile and behavioural surface the intermediate adds.

---

## 4. Adjudications

### A. G-VERIFYING — **RATIFIED (9 + 1)**

Every link of the builder's argument verifies.

1. **G-VERIFYING is a `Finalizing` shape.** `GwzM5-8R0Inventory.md` §4 row G:
   "step `VerifyingPublication` or `Complete`; operation `Finalizing` or
   `Completed`" — the pairing is `VerifyingPublication`↔`Finalizing`
   (G-VERIFYING) and `Complete`↔`Completed` (G-COMPLETE-PRE-ARCHIVE). Confirmed
   in code: `characterization_publication_v0.rs:184` is exactly
   `assert_eq!(record.state, OperationState::Finalizing, "{label}");` for the
   verifying-publication window. The new fixture asserts the same
   (`compatibility_unbound_v0.rs:425-434`).
2. **The corpus cannot hold it without being weakened.** A
   `valid_unlisted_corpus` row for it would need `operation_state: "finalizing"`,
   which (a) `check_merge_compatibility_predicates.py:575-578` rejects against
   `VALID_UNLISTED_STATES`, (b) `compatibility_v0.rs:536-547`'s `operation_state`
   maps to a `panic!`, and (c) would require deleting
   `assert_ne!(record.state, OperationState::Finalizing)` at `:503`. That is
   precisely §12.9(c)'s "widening it to admit these rows would weaken the
   registry, not extend it".
3. **No other row lands without weakening.** `fixture_corpus` is the ADAPTED
   corpus — a row there asserts the shape is a whitelisted A1 migration rule,
   which G-VERIFYING is not (contract §4 names seven shapes, verifying
   publication is not among them). The only non-weakening alternative is minting
   a *third* standalone corpus, as E5.2 did for archives — but §12.9(c) already
   set the precedent for `Finalizing` residue rows and it is the clause-cited
   disposition, applied to three such rows (F-BASELINE, F-MARKER, F-LOCK)
   without a corpus. Minting one for the fifth would leave those three
   inconsistent. **I cannot construct a tenth row that does not weaken the
   registry or invent an inconsistency. OVERTURN fails; RATIFY.**
4. **The disposition is in §12.7's second form and is genuinely executed.**
   `g_verifying_is_dispositioned_by_clause` (`:526-561`) builds the verified
   descriptor, asserts `i2_whitelist_matches(descriptor) == []` (an executed
   zero-match, not a string), then drives `decode_production_v0` →
   `adapt_open_v0` → `ValidUnlisted` and `assert_valid_unlisted` for byte
   preservation. The structural claim in the comment — that the registry's
   closed `publication_step` enum lacks `verifying_publication` — is **true**
   (measured: `normalization.enums.publication_step` =
   `["absent","validating_results","preparing_candidate","committing_evidence","publishing_candidate","complete"]`)
   but is asserted in prose, not executed: **[P3-2]**.

**Dated corrections owed in `GwzM5-8R4bG-Evidence.md` (lane owner):**

- §12.9(d) / E0.2b §8 [P2-2]: "E5.1 owes **10** registry rows" → **9 registry
  rows + 1 clause-cited disposition**. The 10 is right as a *shape* count and
  wrong as a *registry-row* count, in the same form §0 used for 67 → 38 and
  E0.2b used for 18 → 10.
- §12.9(c): its Finalizing ground reaches **three** of its four rows
  (F-BASELINE, F-MARKER, F-LOCK) **plus G-VERIFYING**, which §12.9 never
  considered — four Finalizing rows, not five. It does **not** ground
  `J-NO-PUBLICATION-UNBORN`: part (i) is ADAPTED via an existing `fixture_corpus`
  row (the whitelist *does* adapt `Finalizing`), and part (ii) is a terminal
  `Completed` shape. See **[P2-4]** and **[P2-1]**.

### B. Vocabulary widening — **SOUND**

- Three tokens added, all non-`Finalizing`: `executing`, `awaiting_resolution`,
  `halted`, in `VALID_UNLISTED_STATES` (`check_merge_compatibility_predicates.py:130-140`)
  and `operation_state()` (`compatibility_v0.rs:543-545`).
- `Finalizing` remains absent from both, and falls through to
  `other => panic!("state {other:?} is not in the I2 valid-unlisted corpus")`.
- The closure assertion (`:499-502`, "every rule is open+finalizing") and
  `assert_ne!(…, Finalizing)` (`:503`) are outside the diff entirely.
- **It cannot silently admit a `Finalizing` row later**: three independent
  refusals stand (Python closed set; Rust `panic!`; the `assert_ne!`), and the
  registry's own `normalization.enums.operation_state` is `["finalizing"]` for
  whitelist descriptors only — a separate vocabulary.
- Dated comments present on both sites, each naming §12.9(c) and the ground.
- **Gap:** no weaken-and-raise unit test covers `VALID_UNLISTED_STATES` (a
  pre-existing gap, now carrying three more tokens), nor the new
  `ARCHIVE_DISPOSITIONS` / `TIER_STATUSES` closed sets — **[P3-3]**.

### C. E5.2 two-tier — the archival act is real; the six-bases disclosure is wrong by one

**Tier 1 drives the production archival act.** `terminal_before_archive`
(`:172-258`) reaches each shape's terminal state behind
`FaultingMergeStore::new(FinalizationFault::BeforeArchive)`, whose one-shot
`fired` flag (`fixtures.rs:109-113`) refuses `archive()` exactly once and then
delegates to `FileMergeStore.archive` → `store/mod.rs:152-154` →
`store/archived.rs:23-60`, which is `rename_durable(&source, &destination)` of
the exact bytes. So the close in the test body **is** the production done-record
write path.

**The digest pair is genuinely pre/post, from different files.**
`archive_equivalence_v0.rs:346-373`: asserts `open.is_file()` **and**
`!done.exists()` *before* reading, digests the open bytes, runs the close,
asserts `!open.exists()`, then digests `done`. There is no path on which both
digests are read after the act or from the same file, and no re-encode: the
comparison is bytes-to-bytes over a rename. The test also asserts the archived
record is the claimed shape via `load_archived` (`:263-302`), so a digest cannot
pass on a drifted record.

**Six distinct bases → it is FIVE.** `ARCHIVE_ROWS` (`:88-129`) carries
`Base::CompletedCandidate` (AC-CANDIDATE, AL-UNKNOWN),
`Base::CompletedNoPublication` (AC-NOPUB-BORN, AL-OPTIONAL-MISSING, AR-C), and
three distinct `Base::Aborted(fault)` values. Eight rows minus three shared =
**five** distinct durable bases, not six. The module doc's own sentence
(`:44-52`) exists to stop the count being over-read and over-states independence
by one — **[P2-2]**.

**The byte-claim is about the archival act, correctly.** I verified the three
overlay rows independently: `av0_d`'s distinguishing feature is applied by
`mutate_archive` *after* the archive exists
(`characterization_archive_v0.rs:88-93`); `unknown_retention`'s unknown fields
likewise (`:329-340`); AR-C's contradictions likewise (`:352-392`). So no
durable **open** v0 record with those features exists on this tree to be
byte-preserved *through* archival, and the builder is right that byte
preservation is a property of the act. Each row still has its own workspace,
digest pair and corpus row, and a regression fails on its own name.

**Consequence the review states plainly, because the corpus does not.** Because
the act is a rename, tier-1 digest equality is shape-independent: the eight rows
are eight *per-scenario records* (which is exactly what §12.7 asks for) over
five archival acts, and the property they add over one another is a named row,
not independent evidence. The three overlay rows' `tier1.subcase` names
(`av0_d`, `unknown_retention`, `ar_c`) are **not** the constructions the
identically-named existing fixtures build — `assert_archive_shape` dispatches on
`row.base`, not `row.shape` — and the corpus `fixture` field ("av0_d /
CompletedEvidenceGap") describes Table B, not the tier-1 construction. Not a
defect given the disclosure, but the Table B companion must not read as
"AL-UNKNOWN's unknown fields survive archival"; nothing proves that.

**The two PENDING-FIXTURE rows.** Not reported met on either tier; carrier names
R2-F in-corpus; the checker enforces the pair exactly
(`check_merge_compatibility_predicates.py:473-481`) and the runtime test
enforces it from the other side (`archive_equivalence_v0.rs:409-431`). Correct.

**Tier 2.** `owed` on all ten, `test: null` on all ten, and
`validate_archive_tier` (`check_merge_compatibility_predicates.py:408-429`)
forbids a test on an unexecuted tier (`:426-427`) and requires a carrier
(`:428`). No fake execution anywhere.

### D. Checker companions — **sound, with two small gaps**

Verified in `check_merge_compatibility_predicates.py`: `archive_corpus` added to
`TOP_KEYS` with `exact_keys` enforcement; `ARCHIVE_SHAPES` closed **and order-
enforced** against §12.4 Table B; `ARCHIVE_DISPOSITIONS` / `TIER_STATUSES` /
`ARCHIVE_KEYS` / `TIER_KEYS` all closed; clause must contain the contract name,
a `§` and a quoted anchor (`:450-456`); executed tier ⇒ live binding via
`validate_fixture` (which checks the `fn` exists and the subcase string is
exported, `:291-304`) and `carrier is None` (`:421`); unexecuted tier ⇒ carrier
text and `test`/`subcase` both `None` (`:426-428`); and the E0.2b §8
denominators machine-enforced — `executed_tier1 == 8` (`:475`) and
`pending == {AC-NOPUB-UNBORN, AP-PRESERVED}` (`:479`).

The nine new unit tests are in the established weaken-and-raise form and each
relaxation is caught: row dropped, order swapped, bogus subcase, unexecuted tier
claiming a binding, carrier nulled, tier-1 downgraded, a third row declaring
itself pending, the pending pair swapped, clause de-anchored. Gaps: [P3-3].

**Markers.** Compat battery → "validated 7 migration rules, 7 runtime bindings,
and 10 archive shapes" (**measured green**, §6.1). g23 119 → 122, +3 = exactly
the three new `#[test]` fns (one in `compatibility_unbound_v0`, two in
`archive_equivalence_v0`); `g_verifying_is_dispositioned_by_clause` is a plain
`fn`, correctly not counted. Lib remainder darwin 932 → 935 **MEASURED green**;
linux 933 → 936 DERIVED, marked FIRST-DISPATCH-EXPECTED in the established form,
and the +1 darwin/linux delta is preserved across the move, so the derivation is
internally consistent. `checked_artifact::` 446 and `v1_lifecycle::` 256
re-measured unchanged — both confirmed green here. M4 map marker DOC-PENDING —
**[P1-1]**.

### E. CI impact of the forward pin — **no workflow goes red at `cf4213a`**

Answered from workflow files plus checker source, and confirmed by executing the
map checker.

- **`check_m4_scenario_map.py` is wired into NO workflow.** It was deliberately
  un-wired on 2026-08-25 (`8e40fa8`) after run 32749441874 failed on it;
  `checked-artifact-boundary.yml:49-57` and `release.yml:232-241` both record the
  un-wiring and its cure (R2-F's multi-repo checkout). Its `MAP_DOC` resolves
  `ROOT.parent/dev-docs/…` (`check_m4_scenario_map.py:59`), which does not exist
  on a single-repo runner. **It does not "skip loudly" — it is simply not
  invoked;** in a standalone clone it would *fail* if invoked ("M4 scenario map:
  unreadable"), which is why it is un-wired. That is the J-7 scope disclosure's
  outcome.
- **PR / push-to-main lanes** run only `checked-artifact-boundary.yml`
  (`run_r4bg_aggregate_gates.py privacy call-graph` — the `privacy` and
  `call-graph` batteries contain no compatibility or byte-equivalence command)
  and `retained-readers.yml` (path filters match this diff), whose merge step is
  `check_merge_compatibility_predicates.py … --core .` plus its unittest suite —
  both **green at the tip** (§6.1), on both the ubuntu and windows legs.
- **`release.yml`** (release-published / dispatch only) runs
  `SELECTION="fault byte-equivalence:2 unknown-field privacy"` (`:224`).
  `byte-equivalence:2` is 1-based (`run_r4bg_aggregate_gates.py:261`,
  `commands = [all_commands[int(index) - 1]]`), so it selects command **2**, the
  g23 suite ("122 passed") — **not** the M4 map, which is command 1. The
  `compatibility` battery is not selected at all, so `check_merge_docs.py`
  (commands 3–4) never runs in CI either.
- **`platform-matrix.yml` / `windows-matrix.yml`** are `workflow_dispatch`-only.
  `linux-identity-probe.yml`'s path filters do not match.

**Answer: NO CI workflow goes red at `cf4213a` before the root-repo docs edit
lands.** The DOC-PENDING redness is confined to the *local* landing gate set —
which is exactly where [P1-1] bites, because the pin is wrong for the landing it
describes.

`check_merge_docs.py` is **noted-and-skipped** here on a verified basis: its
manifest (`scripts/checks/merge_docs_manifest.json`) names 12 sources —
`dev-docs/GwzM5-8I2{ActionJournal,Compatibility,Record}Contract.md`,
`dev-docs/GwzM5-8R4bTransitionDesign.md`, `dev-docs/GwzM5-8Refactor.md`, five
`gwz-cli/docs/*`, and `gwz-core/dev-docs/GwzMerge{Design,Plan}.md`. **The diff
touches none of them** and touches nothing under gwz-dev/dev-docs or
gwz-cli/docs. It runs for real at landing.

### F. Tier-2 opinions for the lane owner (opinions only — I do not rule)

**(i) Is it true that no v1-finished archive fixture exists on this tree?
YES — verified independently.** `record_wire/archive/mod.rs`'s
`archived_fixture_for_test` builds `v1_record(Shape::CompletedCandidate)` and
serializes it — a **synthetic** record, never the product of an operation.
Every `.gwz/merge/done/` interaction in `v1_lifecycle/tests/` is a hand-written
`write_done(...)` of hand-built bytes: `status.rs:136-166`
(`open_status_retries_after_an_archived_copy_appears` writes the synthetic v1
bytes straight into the done dir), `gc.rs:301-311`, `archive.rs:159-175`. No
v1_lifecycle test runs an operation to completion and reads back
`.gwz/merge/done/`. Tier 2 has no fixture to stand on.

**(ii) Is E4.4 a sound carrier? Sound in kind, encumbered in schedule —
and it must not be recorded as ruled.** E4.4 is the right *place*: E0 §6.4
already routes `gc_archived` there (`:1159-1164`) and E4.4 is "terminal archive
source-only", the step that would make an operation finish under v1 into an
archive. But **E4 as a whole is gated on R2-F's relocation** under the operator's
ruling (a) (plan §1.1 `:100-113`; `CurrentProgramCheckpoint.md:2143`), so
routing tier 2 to E4.4 makes it transitively blocked on a cross-lane dependency
— and the corpus rows say nothing about that. My opinion: E4.4 is the correct
carrier *if* the lane owner accepts the dependency, and the carrier string should
then say so. Recording it in a machine-enforced corpus **before** the ruling is
[P2-3].

**(iii) The comparable-sub-surface problem — the builder is RIGHT, and
understates it.** `ArchivedMergeProjection` (`model/archive_projection.rs:24-28`)
has exactly **three** fields: `source_version`, `terminal_outcome`, `acceptance`.
A v1-produced archive projects `source_version: V1` and
`ArchivedAcceptanceProjection::SupportedPersisted` (`record_wire/archive/v1.rs:12`);
a v0-produced archive of the same scenario projects `source_version: V0` and
`LegacyComplete` (`record_wire/archive/v0.rs:80`, `:127`). So **two of the three
fields of the whole frozen projection surface differ by construction**, and only
`terminal_outcome` can be equal. Tier 2 as literally written in amendment §6.3
("equals, on the frozen projection surface") is therefore **not satisfiable at
all** without a defined comparable sub-surface — the E0 decision anticipated a
differing header pair, not a differing acceptance discriminant.

**Was the builder right not to mint one? YES, emphatically.** §6.3's rejected
alternative exists to stop a lane "satisfy[ing] the clause by comparing only
v0-origin archives and reporting the O8 clause met", and minting a sub-surface
that excludes `source_version` and the acceptance discriminant would be the same
move in a different costume — narrowing the comparison until the clause looks
met, chosen by the lane that benefits. This belongs in an amendment with a
dual, not in an implementation step. The right disposition today is exactly what
landed: tier 2 `owed` on every row, with the open design question filed.

---

## 5. Citations — all five re-verified against the live contract

Verified by reading `GwzM5-8I2CompatibilityContract.md` directly on 2026-08-28.
Section titles also verified (`§4` = "Open v0 compatibility result", `§5` =
"Migration eligibility and atomic boundary", `§7` = "Archive decoder and GC
rules").

| Cite | Where written | Verdict |
| --- | --- | --- |
| §4 `:136-144` "A1 deliberately whitelists only seven one-member-workspace, `Finalizing`, normal-mode shapes … are not A1 migration rules." | `compatibility_unbound_v0.rs:18-24`, `:513-517` | **exact** at `:136-144` |
| §5 `:170-172` "No v0 `RecoveryRequired`, `Preserving`, or `RollingBack` row is A1 migration-eligible. The adapter never invents a recovery origin or reverse owner from an unjournaled v0 window." | `compatibility_unbound_v0.rs:25-28` | **exact** at `:170-172` |
| §5 `:178-184` "Zero whitelist matches is not an error. Open read-only status leaves bytes unchanged …" | `compatibility_unbound_v0.rs:29-31`, `:518-520` | **exact**, spans `:178-179` inside `:178-184` |
| §5 `:178-184` "Completed and aborted v0 records remain v0 and use byte-preserving archival" (eight tier-1 rows' clause) / "Archived v0 uses only the archive decoder/projection below" (the two PENDING rows' clause) | JSON `archive_corpus`, `archive_equivalence_v0.rs:22-25` | **exact**, at `:181-182` and `:183-184` |
| §7 `:214-250` "Archive projection reads only the exact done-record bytes … never rewrites the archive." | JSON PENDING rows, `archive_equivalence_v0.rs:26-31` | **exact** at `:216-218`; `:214` is the §7 header, so the range is the section, not the sentence — acceptable under the content-anchored rule |

The dates carried are `as of 2026-08-28`, matching the rule's form. No bare
line-anchored cite was written.

---

## 6. Executed gates — verbatim tails, direct exit codes

Every command run **bare** in the review worktree; the exit status captured
immediately and never through a pipe.

### 6.1 Tip — `cf4213a`

```
fmt exit=0
check exit=0
clippy exit=0
checked_artifact exit=0
remainder exit=0
v1_lifecycle exit=0
boundary exit=0
boundary-units exit=0
compat exit=0
compat-units exit=0
```

`cargo fmt --check` — no output, exit 0.

`cargo check --all-targets`:
```
    Checking gwz-core v0.11.0 (.../e5-review-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 21.94s
```

`CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets -- -D warnings`:
```
    Checking gwz-core v0.11.0 (.../e5-review-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 12.37s
```

`cargo test --lib checked_artifact::`:
```
test result: ok. 446 passed; 0 failed; 0 ignored; 0 measured; 1193 filtered out; finished in 52.79s
```

`cargo test --lib -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::`:
```
test result: ok. 935 passed; 0 failed; 1 ignored; 0 measured; 703 filtered out; finished in 57.35s
```
The one ignored test is
`operation::workspace_mutator_lock::tests::child_process_observes_lock_contention`;
**pre-existing, not new** — `git grep "#\[ignore\]" c11c5ef --
src/operation/workspace_mutator_lock.rs` → `c11c5ef:…:279:    #[ignore]`, and
the diff touches nothing under `src/operation/`.

`cargo test --lib workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix`:
```
test result: ok. 256 passed; 0 failed; 0 ignored; 0 measured; 1383 filtered out; finished in 346.43s
```

`python3.13 scripts/checks/check_checked_artifact_boundaries.py`:
```
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

`python3.13 scripts/checks/test_check_checked_artifact_boundaries.py`:
```
----------------------------------------------------------------------
Ran 69 tests in 514.177s

OK
```

`python3.13 scripts/checks/check_merge_compatibility_predicates.py dev-docs/GwzM5-8I2CompatibilityPredicates.json --core .`:
```
validated 7 migration rules, 7 runtime bindings, and 10 archive shapes
```

`python3.13 scripts/checks/test_merge_compatibility_predicates.py`:
```
----------------------------------------------------------------------
Ran 23 tests in 0.049s

OK
```

`check_merge_docs.py` — **noted and skipped**, on the verified basis in §4.E:
its manifest names 12 sources and the diff touches none of them. It runs for
real at landing.

### 6.2 Intermediate — `221cd89` (Ritual 7)

Checked out in the review worktree (`git checkout --detach 221cd89`;
`git status --porcelain` empty), gated, then restored to `cf4213a` (verified:
`git rev-parse HEAD` → `cf4213a…`, working tree clean).

```
checkout exit=0
check exit=0
boundary exit=0
compat exit=0
compat-units exit=0
e51-tests exit=0
inventory exit=0
g23 exit=0
boundary-units exit=0
restore exit=0
```

`cargo check --all-targets`:
```
    Checking gwz-core v0.11.0 (.../e5-review-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 5.08s
```

`python3.13 scripts/checks/check_checked_artifact_boundaries.py`:
```
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

`python3.13 scripts/checks/test_check_checked_artifact_boundaries.py`:
```
----------------------------------------------------------------------
Ran 69 tests in 517.073s

OK
```

`python3.13 scripts/checks/check_merge_compatibility_predicates.py dev-docs/GwzM5-8I2CompatibilityPredicates.json --core .`:
```
validated 7 migration rules and 7 runtime bindings
```
— the pre-E5.2 marker, matching `221cd89`'s own driver pin. Correct: E5.1 does
not create the archive corpus.

`python3.13 scripts/checks/test_merge_compatibility_predicates.py`:
```
----------------------------------------------------------------------
Ran 14 tests in 0.018s

OK
```
— the pre-E5.2 suite size, again matching `221cd89`.

`cargo test --lib workspace_ops::tests::g23::compatibility_unbound_v0::` (the
E5.1 test by name):
```
test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 1636 filtered out; finished in 2.36s
```

`cargo test --lib workspace_ops::tests::g23::compatibility_v0::i2_runtime_binding_inventories_equal_the_registry`
(the row/runtime inventory pin, the other half of the L6 package):
```
test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 1636 filtered out; finished in 0.00s
```

`cargo test --lib workspace_ops::tests::g23::`:
```
test result: ok. 120 passed; 0 failed; 0 ignored; 0 measured; 1517 filtered out; finished in 31.96s
```
— matching `221cd89`'s own g23 pin of "120 passed" exactly. **The intermediate
commit is green on every gate it pins**, with the sole exception of the M4 map
battery, which is DOC-PENDING by construction at both commits (§6.3, [P1-1]).

### 6.3 The M4 map checker, measured

The checker was executed read-only against the **real** `GwzM5-8R4bG-Evidence.md`
by symlinking `dev-docs` beside the review worktree (so `ROOT.parent/dev-docs`
resolves) and supplying `--test-list` from `cargo test --lib -p gwz-core -- --list`
at `cf4213a` (1639 tests). No repository file was written.

**(a) At the tip, unpatched — RED, as disclosed:**
```
M4 scenario map: failed
- registry case is unclaimed by the M4 map: open/awaiting-resolution
- registry case is unclaimed by the M4 map: open/executing
- registry case is unclaimed by the M4 map: open/halted
- registry case is unclaimed by the M4 map: preserving/candidate
- registry case is unclaimed by the M4 map: preserving/pre-acceptance
- registry case is unclaimed by the M4 map: preserving/root-prefix
- registry case is unclaimed by the M4 map: rollback/evidence
- registry case is unclaimed by the M4 map: rollback/pre-acceptance
- registry case is unclaimed by the M4 map: terminal/completed-no-publication
map-exit=1
```

**(b) Against a patched COPY carrying the §12.3 Table A companion only:**
```
M4 scenario map: ok (39 scenario rows, 42 named tests, 22 registry rows all claimed)
exit=0
```

**(c) Against a patched COPY carrying BOTH companions the builder's report owes
(§12.3 Table A's ten rows AND §12.4 Table B's eight rows citing
`archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records`):**
```
M4 scenario map: ok (39 scenario rows, 43 named tests, 22 registry rows all claimed)
exit=0
```

The pin in the tree at `cf4213a` is **42**. See [P1-1].

---

## 7. Findings

### [P1] — landing-blocking until remediated

**[P1-1] The forward-stated M4 map pin is short by one named test, so the
DOC-PENDING remediation it names will NOT turn the battery green.**

`scripts/checks/run_r4bg_aggregate_gates.py:230-231` pins
`"M4 scenario map: ok (39 scenario rows, 42 named tests, 22 registry rows all
claimed)"`. E5.1 (`221cd89`) moved that marker 41 → 42 and its comment block
explains exactly that step: "named tests go 41 -> 42 (the one new test)". **E5.2
(`cf4213a`) never touched the marker** — the only markers it moved are the fault
count, the compatibility marker and the g23 count (verified:
`git diff 221cd89..cf4213a -- scripts/checks/run_r4bg_aggregate_gates.py`
contains no `named tests` line). But E5.2's own owed companion edit introduces a
**second** new named test into the `<!-- m4-map -->` region: §12.4 Table B's
eight rows citing
`archive_equivalence_v0::archived_v0_shapes_are_byte_preserved_from_their_open_records`.
`check_m4_scenario_map.py:134-138` collects test paths into a **set**, so ten
Table A rows citing one test contribute +1 and eight Table B rows citing another
contribute +1 — 41 + 2 = **43**.

Measured, not argued: §6.3(b) yields 42 with the Table A companion alone;
§6.3(c) yields **43** with both. Consequence: after the lane owner lands the
companion edits the builder itself specifies, `run_r4bg_aggregate_gates.py
byte-equivalence` command 1 **stays RED**, now failing on
`expected … absent` rather than on unclaimed cases. The disclosed remediation
does not clear the gate.

*Fix (one line, plus a comment):* the pin must read `43 named tests`, with the
E5.2 driver comment recording its own +1 the way the E5.1 block records the
first. If instead the lane owner lands only the Table A companion in this
window, 42 is correct and the Table B companion becomes a second DOC-PENDING
step — but then §12.4 Table B's eight rows stay UNBOUND while the corpus records
them tier-1 executed, which is worse. Landing both and pinning 43 is the right
order.

*No CI workflow is affected* (§4.E) — this is a local landing-gate defect, and
that is precisely why it must be fixed at landing rather than discovered later.

### [P2] — fix or dated determination

**[P2-1] ESCALATES — `terminal/completed-no-publication` is filed against the
same durable object §12.9 already dispositioned by clause as
`J-NO-PUBLICATION-UNBORN`(ii), undisclosed; and it refutes §12.9(c)'s stated
ground for that row.**

`compatibility_unbound_v0.rs:360-383` builds K-COMPLETED-NOPUB-OPEN as
`init_one_member_workspace` + `branch_create(remote, "feature/source", "HEAD")` +
`FaultingMergeStore::new(FinalizationFault::BeforeArchive)` +
`invoke_with_store(request(false))`. `compatibility_residue_v0.rs:102-118`'s
`unchanged_member_fixture(label, born=false)` is the **identical** construction,
and `:405-439`
`v0_no_publication_terminal_twins_stay_v0_for_born_and_unborn_roots` drives it
with the **identical** assertions (`Completed` / step `Complete` / no candidate /
no composition / empty hashes) through the same real path to `ValidUnlisted` +
byte-exact — for both twins, and additionally pinning `baseline.root_head` /
`root_branch`. The new K arm asserts nothing about the root, so it does not even
state which twin it is; by construction it is the **unborn** one — J's own twin.

This is not a weakening and not a wasted row: K-COMPLETED-NOPUB-OPEN is a
genuine §12.3 Table A row that the ledger asks E5.1 to bind, and "one durable
object, two R0 rows" is already a sanctioned pattern (§12.6 records
`terminal/completed` binding both `G-COMPLETE-PRE-ARCHIVE` and
`K-COMPLETED-CANDIDATE-OPEN`). But three things now need a dated determination:

1. §12.9(c)'s "none of the four gained a registry row … the
   `valid_unlisted_corpus` cannot express a `Finalizing` shape" is **false as a
   ground for J(ii)**, a terminal `Completed` shape — and E5.1's own new row,
   `operation_state: "completed"`, proves the corpus can express that state.
2. The §12.3 companion will show K bound by a registry row while J(ii) stays
   DISPOSITIONED-UNLISTED for the same durable object. No note covers that.
3. §12.6's registry-row arithmetic should gain a line in its own form: 22 rows,
   22 shape labels, but the durable-object count is lower because
   `terminal/completed` binds two R0 rows and `terminal/completed-no-publication`
   binds an object §12.9 already dispositioned.

Escalates because the fix changes the ledger's narrative, not just a number.

**[P2-2] `archive_equivalence_v0.rs:44-52` says "six distinct durable bases";
it is FIVE.** Eight rows over `CompletedCandidate` (×2), `CompletedNoPublication`
(×3) and three distinct `Aborted(fault)` values = five. The sentence exists to
stop the count being over-read and over-states independence by one. It is landed
in a tracked evidence-bearing module doc and repeated verbatim in the builder's
report; the Table B companion should not inherit it.

**[P2-3] ESCALATES — the tier-2 carrier `R2-E E4.4` is machine-enforced in the
corpus while the builder's own report says the ruling is the lane owner's and
unmade.** All eight `byte-preserved-v0-origin` rows carry `"carrier": "R2-E E4.4
terminal-archive consumer (…)"`, validated by
`check_merge_compatibility_predicates.py:426-428`. A carrier assignment is a
ledger act. Worse, it is silently encumbered: E4 is gated on R2-F's relocation
under the operator's ruling (a), so the corpus makes tier 2 transitively blocked
on a cross-lane dependency it does not name. Either ratify E4.4 and say so in
the carrier string, or re-point the rows.

**[P2-4] "§12.9's table names four `Finalizing` residue rows and misses this
fifth" is inaccurate, and it is landed in the tree** at
`compatibility_unbound_v0.rs:46` and `:525`. §12.9's four are F-BASELINE,
F-MARKER, F-LOCK (all `Finalizing`) and `J-NO-PUBLICATION-UNBORN`, whose part
(i) is ADAPTED at a `Finalizing` window through an **existing `fixture_corpus`
row** — the whitelist adapts `Finalizing`, so Finalizing-ness is not what
excluded it — and whose part (ii) is terminal `Completed`. The Finalizing ground
reaches three of the four, plus G-VERIFYING: **four**, not five. The G-VERIFYING
adjudication is unaffected; the framing owed to §12.9(c) differs from the one
written. Fix the comment and write the §12.9(c) correction as in §4.A.

### [P3] — recorded, may ride

**[P3-1] "Only removed lines: the seven pin/marker strings" is false — there are
eight.** The eighth is `rollback/participant`'s corpus row, re-added
byte-identical apart from a trailing comma. Substance verified benign; the claim
is not accurate as written.

**[P3-2] The G-VERIFYING structural claim is prose, not executed.**
`compatibility_unbound_v0.rs:511-512` asserts in a comment that the registry's
closed `publication_step` enum "does not carry [`verifying_publication`] at all,
so zero rules can equal it". It is **true** (verified), but the test only
executes `i2_whitelist_matches(descriptor) == []` — zero matches for *this*
descriptor. One assertion over the whitelist would make the structural claim
executed rather than asserted.

**[P3-3] The new closed sets gain no weaken-and-raise unit tests.**
`VALID_UNLISTED_STATES` (+3 tokens) has none — a pre-existing gap now carrying
more weight; nothing in the suite proves `finalizing` is rejected at
`check_merge_compatibility_predicates.py:575-578`. `ARCHIVE_DISPOSITIONS` and
`TIER_STATUSES` likewise gain no test (the nine new tests cover shapes, order,
bindings, carriers, the pending pair, and clause anchoring).

**[P3-4] Six of the ten progress shapes are produced by post-hoc field
re-statement, not by driving production into the window.**
`pre_acceptance_in_state` (`:205-212`) for A-EXECUTING / A-PRE-PRESERVE /
A-PRE-ROLLBACK; direct `record.state` assignment for H-PRESERVING-CANDIDATE,
H-PRESERVING-PREFIX and I-EVIDENCE-ROLLBACK (`:319-356`). Disclosed per arm, and
sound — the records survive `write_open`'s verify-after-write
(`store/mod.rs:140-149`) and the v1 lifecycle validator, so they are legal
durable shapes, and `adapt_open`'s classification gate reads exactly the field
that was re-stated. Recorded because the per-scenario record will read as if
production reached these windows.

**[P3-5] The module doc's stated separation ground for row I is not what the
test asserts.** `:52-60` says the row-A shapes carry "no reversed participant,
which is precisely what rows H and I retain", but `I-EVIDENCE-ROLLBACK`'s
construction (`:349-356`) creates no `Aborted`/`RolledBack` participant and
`assert_r0_combination` (`:453-458`) does not assert one. The rows are still
genuinely distinct — A-PRE-ROLLBACK asserts publication absent, I-EVIDENCE-
ROLLBACK asserts a composition commit present — so only the stated ground is
wrong.

**[P3-6] Landing caution for the §12.3 companion.**
`g_verifying_is_dispositioned_by_clause` is a plain `fn`, not a `#[test]`. If
G-VERIFYING's Table A row cites it as a test path,
`check_m4_scenario_map.py:139-141` fails with "map names a test that does not
exist" (`known` comes from `cargo test --lib -- --list`, which lists only
`#[test]` fns). That row must cite
`compatibility_unbound_v0::v0_unbound_progress_shapes_are_refused_by_adapt_open`.

### Adversarial sweep — what I looked for and did not find

- **Relabeled duplicates among the ten progress shapes:** none. All ten assert
  distinct R0 §4 combinations (state × publication presence × candidate ×
  composition × prefix × hashes), and the three closest neighbours to already-
  bound rows (`preserving/stash`, `rollback/participant`) are separated by
  positive assertions of absent preservation evidence and absent reversed
  participants (`:400-414`). The cross-table duplicate is [P2-1].
- **A byte-preservation test that could pass against a rewriting archival:** no.
  `!done.exists()` is asserted before the open digest; `!open.exists()` after;
  the digests come from two different files across the act.
- **Overlay fixtures making the byte-claim about the overlay:** no — the three
  overlay rows deliberately do not apply their overlays, and the module doc says
  so. The reading risk is in the Table B companion's wording, noted in §4.C.
- **`unwrap`/`panic` masking a refusal-path assertion:** none. Every `unwrap` is
  on a construction step whose failure is loud; the fault-injection arms assert
  `store.fired.get()` or use `unwrap_err()`, so a fault that silently failed to
  fire panics rather than passing.
- **Order / wall-clock / platform flake:** none found.
  `record.participants` is a `BTreeMap` (`model/v1/record.rs:33`), so the
  `values_mut().find(...)` in `pre_acceptance_halted` is deterministic; clocks
  are `FixedClock::new(TimestampMs(1_700_000_000_000))` and ids
  `SequentialIdProvider`; all paths are `Path::join` with forward slashes, the
  established cross-platform pattern in this suite; none of the three new tests
  carries a `cfg` gate, which is what makes the linux +3 derivation legitimate.
  The Python side reads every file with an explicit `encoding="utf-8"`, so the
  `§` in the clause survives the Windows leg of `retained-readers.yml`.
- **Weakening in `compatibility_v0.rs`'s +40:** none. Enumerated in §2.

---

## 8. Verdict

**GO-WITH-CONDITIONS.**

The package does what the ledger asks and does it through the real production
paths. The parametric test drives `adapt_open`, not the precheck walk, and lands
in one commit with its rows; tier 1 drives the real done-record write and
compares real pre/post bytes; the two unfixtured shapes are declared, not
claimed; tier 2 is `owed` everywhere with no fake execution; the §12.8 PARTIAL
statement stands; the load-bearing `Finalizing` closure is untouched; every
content-anchored citation verifies; and all executed gates are green.

Conditions, all at landing:

1. **[P1-1] Fix the M4 map pin to `43 named tests`** and record E5.2's own +1 in
   the driver comment — otherwise the docs edit that is supposed to clear the
   battery leaves it red. Re-run `check_m4_scenario_map.py` against the real
   Evidence doc after landing the edits and confirm the marker.
2. **Land both dev-docs companions in the same landing window** — §12.3 Table A
   (ten rows: nine case ids plus G-VERIFYING DISPOSITIONED-UNLISTED) **and**
   §12.4 Table B (eight rows to tier-1 byte-preserved, two to PENDING-FIXTURE
   with R2-F named). One without the other leaves the tree self-inconsistent.
3. **[P2-1] and [P2-4] — write the dated §12.9 corrections**: §12.9(d)/E0.2b §8's
   "10 registry rows" → 9 + 1; §12.9(c)'s Finalizing ground reaches three of its
   four rows plus G-VERIFYING; and the K / J(ii) same-object note with the
   §12.6-form arithmetic line.
4. **[P2-3] Rule on the tier-2 carrier** before the corpus's E4.4 string is
   treated as settled, and name the R2-F dependency in the carrier text if E4.4
   is ratified.
5. **[P2-2] Correct "six distinct durable bases" to five** in
   `archive_equivalence_v0.rs` and do not let the wrong figure reach §12.4.
6. Re-measure the linux lib-remainder count (936, DERIVED) at the three-platform
   dispatch, per the FIRST-DISPATCH-EXPECTED convention already recorded.

[P3-1]…[P3-6] are recorded and may ride.

**G-VERIFYING deviation: RATIFIED (9 registry rows + 1 clause-cited
disposition).** **Tier-2 sub-surface: the builder was right not to mint one.**
**CI: no workflow goes red at `cf4213a`.**
