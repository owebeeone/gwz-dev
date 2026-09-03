# GwzM5-8 R2-E E4.7 — charter prep (read-only investigation)

Date: 2026-09-02. Author: a read-only investigation session for the
implementation lane. **Status: INPUT TO E4.7's CHARTER. Not an amendment,
not a plan change, not a build.** No file in `gwz-core` was modified; no
build, no test, no commit was run. Every claim below carries a `file:line`
citation opened and read in this session.

Trees read: `gwz-core` at main **`0dae0d5`** ("E4.3-B landing nit: the
`production_rust_files` cite re-pointed to `:982-989`"; `git status` clean)
and `gwz-dev/dev-docs` at `3351895`.

Authorities consumed, treated as falsifiable throughout:
`GwzM5-8R2E-CapabilityFreeAmendment.md` (ADOPTED) §3, §4's E4.7 bullet, §5,
§7 + its post-adoption note and its E4.5/6-B disposition note;
`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`; `GwzM5-8R2E-Plan.md` §1
(O1 `:46`, O3, O8 `:53`, O12, O13 `:88-115`, O14 `:138-151`), §3 Phase E4
(`:359-615`), the E4.7 step `:597-615` and the DR-1 row `:616-626`;
`GwzM5-8R2E-E7-Acceptance.md` §4/§5/§6/§7;
`GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §7.1 (`:1228-1263`);
`GwzM5-8R2E-RecordRootAmendment.md`; `GwzM5-8R2E-E45-6B-CharterPrep.md`;
`GwzM5-8R4bR2ConsumerCheckpoint.md` §10 (`:268-290`).

---

## 0. Headline answers

1. **The allowance class is BIGGER than §4's list, and the §4 string sweep
   as prescribed would MISS its own §5 member.** The prescribed needles
   (`E4.2-E4.6`, `awaiting R2-E consumer conversion`, `until E4.7`,
   `R2-E Phase E4`, `plan §5 item`, `first production consumer(s)`,
   `conversion arrives`) do **not** match `archive.rs:110` — the
   `gc_archived` allowance §5 dispositions by name — nor its downstream twin
   at `merge/gc.rs:33`. Both belong to a different A1-era reason family
   ("A1 activation: …no production caller until that route lands"). The
   charter must widen the needle set (§1.0 below).

2. **Two allows in the named class have already EXPIRED in fact and are
   suppressing nothing** — removing them will not fail `cargo check`:
   `operation/workspace_mutator_lock.rs:44-47` (four production callers
   found) and `checked_artifact/namespace/mod.rs:47-51` (both re-exported
   names consumed through that exact path). A third,
   `checked_artifact/mod.rs:27-32` (`pub(crate) mod entry`), is a strong
   expiry candidate: all thirteen doors have production callers.

3. **`finish()` reachability, per converted consumer: E4.1 NO, E4.2's
   parent half NO, E4.2's record half YES, E4.5-B's marker arm YES
   (conditional).** The A-1 decision does **NOT** reopen: the two consumers
   that bypass `finish()` take the *checked* retirement instead, and the two
   that take the legacy path execute `cleanup.rs:146-158` unconditionally.

4. **`gc_archived` recommendation: DELETE the family — but the blast radius
   is three functions wider than §5 says, and the deletion MOVES two
   machine pins.** `merge/gc.rs`'s `preflight_archived_cleanup`,
   `delete_preflighted_backup_refs` and `require_backup_refs_absent` have
   *no other caller*; and `archive.rs`'s `sync_dir` count drops 2 → 0,
   firing the O13 inventory's SHRINKAGE arm, which the CapFree §3 mechanism
   makes an amendment-tier event. Recommendation and its record in §4.

5. **A scope seam between the pins package (E4.4-6-B) and E4.7 must be
   settled at charter time.** CapFree §7's gwz-core list gives the PINS
   PACKAGE "`archive.rs:108-111` and `catalog_names.rs:44` re-reasoned" and
   "the `E4.2-E4.6`-range allowance class annotated by name for E4.7"; §5
   gives E4.7 the `gc_archived` disposition. Both steps therefore write the
   same lines. The pins package has **not landed** at `0dae0d5` (its builder
   commit `b99bfb7` is in the object store, not on main).

---

## 1. THE ALLOWANCE CLASS, EXHAUSTIVELY

### 1.0 The sweep actually run, and the needle correction owed

Method: every `#[allow(...)]` / `#![allow(...)]` attribute in `src/` was
extracted with its full multi-line `reason` (169 attributes, 120 carrying a
`reason`), then filtered on the union of the §4 needles plus `A1 activation`,
`production caller`, `production consumer`, `route lands`. Doc comments and
`//` comments were swept separately for the same strings, and `scripts/`
was swept for the checker's own members.

**The needle set §4 prescribes is not sufficient.** Two facts:

- `archive.rs:110`'s reason — the one §5 names — contains none of the
  prescribed strings. Its text is *"A1 activation: landed v1 surface the
  activation's dispatch does not route yet. … so this family has no
  production caller until that route lands."*
- `merge/gc.rs:33` carries that **byte-identical** reason and is named by
  no authority at all.

**Needle set the charter should write instead** (superset, verified to
catch every member below): `R2-E`, `E4.` , `A1 activation`, `until E4.7`,
`plan §5 item`, `conversion arrives`, `production consumer`,
`production caller`, `route lands`, `converts`, `converted consumer`.

### 1.1 Tier 1 — the amendment-NAMED members (16 allow sites)

Reasons are given abbreviated; each was read in full.

| # | Site (0dae0d5) | Lint / item | Amendment cite | Disposition |
| --- | --- | --- | --- | --- |
| 1 | `checked_artifact/mod.rs:10-14` | `dead_code` / `mod bootstrap` | `mod.rs:12` | RE-REASON PERMANENT |
| 2 | `checked_artifact/mod.rs:16-20` | `dead_code` / `mod capability` | `mod.rs:18` | RE-REASON PERMANENT |
| 3 | `checked_artifact/mod.rs:27-32` | `dead_code` / `pub(crate) mod entry` | `mod.rs:29-31` | **EXPIRY CANDIDATE — MEASURE** |
| 4 | `checked_artifact/mod.rs:35-39` | `dead_code` / `mod fault_v1` | `mod.rs:37` | RE-REASON PERMANENT |
| 5 | `checked_artifact/mod.rs:42-46` | `dead_code` / `mod leaf` | `mod.rs:44` | RE-REASON PERMANENT |
| 6 | `checked_artifact/mod.rs:48-52` | `dead_code` / `mod namespace` | `mod.rs:50` | RE-REASON PERMANENT |
| 7 | `checked_artifact/mod.rs:57-61` | `dead_code` / `mod protocol` | `mod.rs:59` | RE-REASON PERMANENT |
| 8 | `checked_artifact/catalog.rs:17-23` | `dead_code` / `mod bootstrap` | `catalog.rs:19-22` | RE-REASON PERMANENT |
| 9 | `checked_artifact/catalog.rs:25-30` | `dead_code` / `mod classifier` | `catalog.rs:27-29` | RE-REASON PERMANENT |
| 10 | `checked_artifact/catalog.rs:32-37` | `dead_code` / `mod enumeration` | `catalog.rs:34-36` | RE-REASON PERMANENT |
| 11 | `checked_artifact/catalog.rs:44-48` | `unused_imports` / `use bootstrap::{…}` | `catalog.rs:46` | RE-REASON PERMANENT *or* narrow the re-export |
| 12 | `checked_artifact/coordinator/mod.rs:14-17` | `dead_code` / `mod execution` | `coordinator/mod.rs:16` | RE-REASON PERMANENT |
| 13 | `checked_artifact/bootstrap/managed.rs:15-20` | `dead_code` / `mod provider` | (implied by "`bootstrap/managed.rs`") | RE-REASON PERMANENT |
| 14 | `checked_artifact/bootstrap/managed.rs:43-51` | `unused_imports` / `use provider::*` | `managed.rs:45-46` | **ALREADY DONE at E4.3-B** — verify only |
| 15 | `operation/workspace_mutator_lock.rs:44-47` | `dead_code` / `catalog_mutation_lease` | `workspace_mutator_lock.rs:44-47` | **EXPIRES — DELETE the attribute** |
| 16 | `v1_lifecycle/archive.rs:108-111` | `dead_code` / `gc_archived` | CapFree §5 | see §4 |

**Per-site evidence and the byte-exact text to write.**

**#15 — `operation/workspace_mutator_lock.rs:44-47`. THE ONE PROVEN
EXPIRY.** Current reason (`:46`): *"R2-C0 freezes the checked catalog borrow
before the C1 owner consumes it"*. FALSE at `0dae0d5`:
`WorkspaceMutatorLock::catalog_mutation_lease` (`:48-52`) has **four**
production callers —
`workspace_ops/merge/runtime/dispatch.rs:453`
(`guard.catalog_mutation_lease()`, the A1 adapter's viability window) and
`workspace_ops/merge/v1_lifecycle/checked.rs:161`, `:179`, `:180`
(`lease._guard.catalog_mutation_lease()`, `acquire_activated` and
`acquire_for_merge_start`). The allow suppresses nothing.
**E4.7 writes: delete lines `44-47` entirely.** `cargo check` must stay
green; if it does not, the reading is wrong and this is a stop-trigger
(§7). This is the E4.1 [P3-5] stale allow the amendment names first.
GATE: `operation/workspace_mutator_lock.rs` is flat-pinned at
`check_checked_artifact_boundaries.py:73` — the digest moves.

**#3 — `checked_artifact/mod.rs:27-32`. THE STRONGEST REMAINING EXPIRY
CANDIDATE.** Current reason (`:29-31`): *"the checked entry-point inventory
is consumed by the legacy leaf writers that are converted, and by R2-E for
the rest; production activation of the remainder is R2-E's (plan §5 item
1)"*. Measured: `entry.rs` exposes 17 functions (`:36,43,53,65,73,84,92,100,
109,118,128,139,150,309,345,371,401`) and **all thirteen legacy doors plus
the three E4.1/E4.2 doors have at least one production caller outside
`entry.rs`** (counts, excluding `interface_tests/` and `tests/`:
`observe_merge_root_artifact` 1, `replace_merge_root_artifact` 1,
`classify_replace_merge_root_artifact` 1, `remove_merge_root_artifact` 1,
`classify_remove_merge_root_artifact` 1,
`observe_merge_preservation_workspace` 2,
`observe_merge_preservation_git_directory` 2,
`replace_merge_preservation_workspace` 1,
`classify_merge_preservation_workspace` 1,
`observe_merge_preservation_bundle` 1,
`classify_merge_preservation_bundle` 3,
`replace_merge_preservation_bundle` 1, `prepare_merge_store_parents` 1 —
the last is the permanent v0-store caller ConsumerCheckpoint `:282-286`
records). `render_catalog_refusal` (`:401`) is `pub(super)` and called at
`:312`, `:350`. **E4.7 must MEASURE this one** (remove, `cargo check`); if
green, delete; if not, the dead residue is a private helper and the reason
becomes the truthful narrow one. Do not assume either outcome.

**#1/2/4/5/6/7 — `checked_artifact/mod.rs`'s six subtree allows.** All six
carry the identical reason *"frozen interface awaiting R2-E consumer
conversion (plan §5 item 1); narrowed at Phase 4 Step 4.3 to the subtrees
that still carry one"* (`:12,18,37,44,50,59`). These are BLANKET module
allows over whole subtrees. Removal would surface the blanket-hidden set
E7-Acceptance §3 measured as **481 items across 50 files** — so removal
genuinely fails `cargo check` and the disposition is RE-REASON, not expire.
**Text E4.7 should write (byte-exact, one string, all six sites):**

> `reason = "frozen interface, PERMANENT: the R2-E consumer conversion \`
> `          named here does not arrive — E4.4-E4.6 do not start and rows \`
> `          \`:275\`-\`:279\`'s writers are carved out (dev-docs/\`
> `          GwzM5-8R2E-CapabilityFreeAmendment.md §3/§7, ADOPTED \`
> `          2026-09-02, on the operator's ruling of the same date); \`
> `          narrowed at Phase 4 Step 4.3 to the subtrees that still carry \`
> `          one. Any future removal is DR-1's, not an E4 step's."`

**#8 — `catalog.rs:17-23`.** Reason `:19-22` names two items as still dead:
`retire_admitted_action` and `observe_roaming_anchor_home`. VERIFIED still
dead — `retire_admitted_action` appears only as definitions/delegations
(`catalog/bootstrap.rs:178,182`, `capability/pre_catalog.rs:195,201`,
`provider/completed.rs:239`) plus one test caller
(`provider/tests_terminal_fault_matrix.rs:380`);
`observe_roaming_anchor_home` likewise (`catalog/bootstrap.rs:189,192`,
`capability/pre_catalog.rs:211,215`, `provider/completed.rs:177`) plus two
test callers (`namespace/tests_barrier_matrix.rs:241,655`). Removal fails
`cargo check`. **Disposition RE-REASON**: replace the trailing clause
*"whose first production consumers are the E4.2-E4.6 conversions"* with
*"which have no production consumer and will not gain one: E4.4-E4.6 do not
start (CapabilityFreeAmendment §7); PERMANENT pending DR-1."*

**#9 — `catalog.rs:25-30`** and **#10 — `catalog.rs:32-37`.** Same shape;
the trailing clauses *"whose readers are the E4.2-E4.6 consumers and its
interface suites"* (`:29`) and *"its first production reader is an
E4.2-E4.6 consumer"* (`:36`) become *"…and its interface suites; no further
production reader arrives (CapabilityFreeAmendment §7) — PERMANENT pending
DR-1."*

**#11 — `catalog.rs:44-48`.** Reason `:46-47` claims `CatalogOwnerV1` is
re-exported for a consumer that will name it. MEASURED: of the four
re-exported names at `:49-51`, `CatalogOwnerEdgeV1` is consumed through
this re-export (`capability/pre_catalog.rs:13-15`),
`OpaqueRetainedCatalogV1` likewise (`coordinator/execution.rs:81`,
`bootstrap/managed/provider.rs:74`), `recover_or_create` likewise
(`entry.rs`), but **`CatalogOwnerV1` is named only inside its own module
(`catalog/bootstrap.rs:17,19,236,239`, `catalog/bootstrap/tests.rs`) and as
two STRING LITERALS in `interface_tests/capability_permit.rs:310,335`.**
So the allow still bites on exactly one name. Two sound dispositions:
(a) RE-REASON permanent, or (b) drop `CatalogOwnerV1` from the re-export
list and delete the allow — cleaner, and the string-literal pins at
`capability_permit.rs:310,335` are unaffected because they are strings.
**Recommend (b)**, flagged as the one behaviour-free surface narrowing in
the step. GATE: moves the `catalog.rs` TREE digest
(`check_checked_artifact_boundaries.py:237`).

**#12 — `coordinator/mod.rs:14-17`.** Reason `:16`: *"Step 3.3 wires the
machinery; conversion arrives consumer by consumer across E4.2-E4.6"*.
`mod execution` HAS production callers since E4.2 (`entry.rs:351,360`
import `admit_merge_start_managed_parents` /
`execute_merge_start_managed_parents`), but E0.2 §7.4 records
*"`coordinator/execution.rs`'s 26 R2-E-gated items"*
(`SemanticsAmendment-DRAFT.md:1290`), so the module is partially dead and
removal fails. **RE-REASON PERMANENT:** *"Step 3.3 wired the machinery;
E4.2 converted the merge-start consumer (`entry.rs`); the REMAINING
interior gains no consumer — E4.4-E4.6 do not start
(CapabilityFreeAmendment §7). PERMANENT pending DR-1."*

**#13 — `bootstrap/managed.rs:15-20`.** Reason `:17-19` ends *"what stays
dead is the surface no converted consumer reads yet"* — the "yet" is the
stale word. **RE-REASON**, dropping "yet" and adding the permanence ground,
matching the sibling at `:43-51` which E4.3-B already updated.

**#14 — `bootstrap/managed.rs:43-51`.** **ALREADY RE-REASONED at E4.3-B**
(reason `:45-50` already cites `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`
and says "…are permanent by decision"). E4.7's act here is a verification,
not an edit — with one nit: the reason still says the imports "shrink only
as the remaining ACTIVATED-LEASE forward arms convert (E4.5/6-B)", and
CapFree §7's E4.5/6-B disposition note has since reduced that to E4.5-B's
single marker arm. **One-clause fold owed**, not a rewrite.

### 1.2 Tier 1 (continued) — the NON-allow prose members (5 + 2 checker)

| Site | Text today | Disposition |
| --- | --- | --- |
| `checked_artifact/entry.rs:299-300` | "E4.2-E4.6 / convert the consumers that will read it." | REWRITE: *"E4.2 converted the merge-record consumers; no further conversion arrives (CapabilityFreeAmendment §7)."* GATE: flat digest `:56`. |
| `checked_artifact/catalog_names.rs:44` | "…a directory that is still live until E4.7 —" | REWRITE: the legacy area's retirement is **DR-1's** now. *Overlaps the pins package's own list (CapFree §7) — settle ownership at charter time.* |
| `src/git/tests/g12.rs:834` | "…the writer that fills it is still live until E4.7." | REWRITE, same ground. §7's post-adoption note (6) names this member. |
| `interface_tests/catalog_activation_pin.rs:43` | "A SECOND production namer is E4.2-E4.6 work and must move this pin again, deliberately." | REWRITE: E4.5-B adds **no** `recover_or_create` namer (E45-6B prep §5.1), so the pin stays at 1 permanently unless DR-1 reopens. §7's note (6) names it. |
| `interface_tests/catalog_activation_pin.rs:136` | assert message: "a further one is an E4.2-E4.6 conversion and moves this pin in its own reviewed commit" | REWRITE the message string, same ground. |
| `scripts/checks/check_checked_artifact_boundaries.py:341,366` | ":275`'s `archive.rs` and `store/archive.rs` are the terminal-archive row's, E4.4's" / "The two ARCHIVE rows keep their retire-on-conversion marker until E4.4 (§6)" | **This is the amendment's "checker `:358`" cite** — line 358 at `0dae0d5` falls inside this comment block (`:326-368`). Both sentences are FALSE post-CapFree: the archive rows are carved and never retire. The pins package owns the O13-block re-annotation (CapFree §7); E4.7 verifies it landed and closes the residue. |
| `scripts/checks/check_checked_artifact_boundaries.py:398` | "A SECOND caller is an E4.2-E4.6 conversion and moves this row deliberately" (in `ENTRY_REFERENCES`) | **NOT named by any authority.** Same rewrite as `catalog_activation_pin.rs:43`. |

### 1.3 Tier 2 — members the SWEEP finds that NO authority names (6 actionable)

These are the sweep's payload. Each matches a prescribed §4 needle or the
corrected needle set, and none appears in §4's list or §7's post-adoption
note.

| # | Site | Lint / item | Reason today | Disposition |
| --- | --- | --- | --- | --- |
| 17 | `coordinator/mod.rs:23-28` | `dead_code` / `mod identity` | "…R2-E converts the merge consumer that reads the rest **(plan §5 item 1)**" | RE-REASON PERMANENT (matches the `plan §5 item` needle) |
| 18 | `capability/pre_catalog/provider.rs:15-18` | `dead_code` / `mod authority_record_binding` | "its consumer is the replacement path **R2-E converts**, not Step 3.3" | RE-REASON PERMANENT |
| 19 | `capability/pre_catalog/provider.rs:25-28` | `dead_code` / `mod barrier_mutation` | "the production consumer is **R2-E Phase E4**" | RE-REASON PERMANENT (matches the `R2-E Phase E4` needle exactly) |
| 20 | `capability/pre_catalog/provider.rs:41-44` | `dead_code` / `mod leaf_observation` | "the binding's consumer is the replacement path **R2-E converts**" | RE-REASON PERMANENT |
| 21 | `namespace/mod.rs:20-23` | `dead_code` / `mod host` | "entry-point reachability is **Phase E4**, itself sequenced behind R2-F's relocation" | RE-REASON PERMANENT — **and it carries a SELF-DECLARED obligation on this step**, below |
| 22 | `merge/gc.rs:31-34` | `dead_code` / `preflight_archived_cleanup` | byte-identical to `archive.rs:110` | **travels with the `gc_archived` disposition (§4)** |

**#21 is the most consequential Tier-2 find.** `namespace/mod.rs:10-19`'s
doc says, verbatim:

> *"So the allow's expiry is **E4's landing, wherever R2-F puts it**, not
> "R2-E" generically; if E4 has not landed by the R2-E settle, E7 owes this
> allow a dated re-owning rather than letting it become permanent by
> silence."*

E7 accepted without discharging it (it is not in E7-Acceptance §4/§5/§7).
E4 has now landed (E4.1/E4.2/E4.3-B). **The dated re-owning is E4.7's**, by
that sentence's own terms, and the answer is now knowable: the promised
entry point exists but the six barrier role methods E2 added still have no
production caller, so the allow is PERMANENT pending DR-1. E4.7 must write
that dated re-owning at `namespace/mod.rs:10-19` and `:20-23`, or the
allow becomes "permanent by silence" — precisely what the sentence forbids.

### 1.4 Tier 2 (continued) — allows the sweep finds that need NO change (3)

- `coordinator/mod.rs:40-43` (`unused_imports`, `use schedule::*`) — its
  reason is already terminal ("this hop serves the interface-test
  contracts") and its doc `:33-39` already records the R2 forward
  reference as DISCHARGED. NO CHANGE.
- `interface_tests/fault_expected_keys.rs:219` (`dead_code`,
  `Reserved(&'static str)`) — "unconstructed since the R2-E E1-E3 landings,
  retained for the map's carried re-reservation clause". Terminal; and
  E7-Acceptance `:280-282` already records it in the settle census.
  NO CHANGE.
- `protocol/authority_record.rs:50-53` (`unused_imports`) — "Step 2.4
  installs the seam; plan §4 Step 3.3 wires its production consumer". This
  is an **R2-D-era** forward reference, not R2-E's class. Adjacent, not a
  member; flag only.

### 1.5 A THIRD PROVEN EXPIRY — `namespace/mod.rs:47-51`

Not in any authority's list. `unused_imports` over
`pub(in crate::checked_artifact) use host::{HostActionNamespaceV1,
retain_action_namespace};` (`:52`). Reason `:49-50` already says "REACHED
from the entry point at R2-E E4.2 … (2026-09-01)". **MEASURED: both names
are consumed through exactly this re-export** —
`bootstrap/managed/provider.rs:75-78` is
`use crate::checked_artifact::namespace::{ActionNamespace,
BootstrapGenerationSlots, BootstrapIntentRowV1, HostActionNamespaceV1,
retain_action_namespace};`, with `retain_action_namespace` called at
`provider.rs:411`. **The allow suppresses nothing. EXPIRES — delete lines
`47-51`.** Same measurement discipline as #15: if `cargo check` reddens,
the reading is wrong and it is a stop-trigger.

### 1.6 Tier 3 — the A1 "own suites" residue class (23 sites), SWEPT, NO CHANGE

Twenty-three allows carry the byte-identical reason *"A1 activation:
reached only by this tree's own suites; the compile gate's blanket
`dead_code` allowance expired with the activation, so the residue is named
item by item."* Sites: `merge/acceptance/v1.rs:39`;
`merge/model/v1/canonical.rs:64,73,101,115`;
`merge/record_wire/decode.rs:15,45,83`;
`v1_lifecycle/authority.rs:38,70`;
`v1_lifecycle/authority/dispatcher.rs:143`;
`v1_lifecycle/authority/drift.rs:25`;
`v1_lifecycle/authority/resolver/observation.rs:116`;
`v1_lifecycle/authority/resolver/reconciliation.rs:2,12,25,38`;
`v1_lifecycle/reverse/execute/preservation.rs:148`;
`v1_lifecycle/service.rs:60`; `v1_lifecycle/tests/forward.rs:1102`;
`v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs:301`;
`v1_lifecycle/transition/mod.rs:93,320`.

**These are NOT the E4.7 class.** Their reason states a terminal FACT
("reached only by this tree's own suites"), promises no future consumer,
and names no R2-E step. E4.7's record should say they were swept and
dispositioned NO CHANGE, so a later reader does not mistake the silence for
an omission. They matter to the charter only because the corrected needle
set (`A1 activation`) catches them: the builder must be told in advance
that 23 of the hits are no-ops.

### 1.7 Stale in-tree prose the sweep finds outside the allowance class

Not allows, but the same staleness and cheap to fold in the same commit:

- `coordinator/execution.rs:159-160` — *"that conversion is the plan's
  minted O14, decided at **E4.6's chartering**."* FALSE: O14 moved to DR-1
  (CapFree §5; plan `:145-151`'s bracket). CapFree §7's root-side list
  re-points O14's *dev-docs* homes but not this in-tree one.
- `coordinator/execution.rs:28-29` — *"`entry.rs` still routes the BUNDLE
  parent through `CheckedArtifact::prepare_parent` — that conversion is a
  **later E4 row's**."* FALSE: ConsumerCheckpoint `:282-286`'s new bracket
  records that promise as permanently unreachable.
- `bootstrap/managed.rs:9-14` — *"`entry.rs` calls
  `CheckedArtifact::prepare_parent` **until R2-E converts it**."* Same
  ground.
- `capability/pre_catalog/provider.rs:20-24` and `:36-40` — the doc twins
  of allows #19 and #18.
- `namespace/tests_cleanup_matrix.rs:24` — "reserves its widening for
  R2-E's *consumer* conversion".

### 1.8 Summary count

| Class | Sites | Expire | Re-reason permanent | No change | Delete-or-permanent |
| --- | ---: | ---: | ---: | ---: | ---: |
| Tier 1 named allows | 16 | 1 proven + 1 candidate | 12 | 1 (verify only) | 1 |
| Tier 1 named prose / checker | 7 | — | 7 rewrites | — | — |
| Tier 2 unnamed allows | 6 | 1 proven (§1.5, a 7th site) | 4 | — | 1 |
| Tier 2 no-change allows | 3 | — | — | 3 | — |
| Tier 2 stale prose | 5 | — | 5 rewrites | — | — |
| Tier 3 A1-suites class | 23 | — | — | 23 | — |
| **Actionable allow sites** | **23** | **2 proven + 1 candidate** | **16** | **1** | **2** |

**Headline split for the charter: of 23 actionable allow sites, 2 EXPIRE
(proven, removal is safe), 1 is an expiry CANDIDATE that must be measured,
2 are the `gc_archived` family's delete-or-permanent pair, and 18 are
RE-REASONED AS PERMANENT** with CapFree §3/§7 as the authority. A further
12 prose/comment/checker sites need rewrites, and 23 Tier-3 allows are
swept-and-untouched.

**Which removals would fail `cargo check` (a genuinely dead item) vs merely
stale reasons:**

- **Genuinely dead — removal FAILS** (18): all six `checked_artifact/mod.rs`
  subtree allows (the 481-item blanket set, E7-Acceptance §3);
  `catalog.rs:17/25/32` (two named items verified still dead + the
  classifier/enumeration vocabulary); `catalog.rs:44` (the one unconsumed
  name `CatalogOwnerV1`); `coordinator/mod.rs:14` (the 26 R2-E-gated items,
  E0.2 §7.4) and `:23`; `bootstrap/managed.rs:15` and `:43`;
  `provider.rs:15/25/41`; `namespace/mod.rs:20`.
- **Merely stale — removal SUCCEEDS** (2 proven, 1 candidate):
  `workspace_mutator_lock.rs:44-47`; `namespace/mod.rs:47-51`;
  `checked_artifact/mod.rs:27-32` (candidate).

---

## 2. THE `finish()`-REACHABILITY RECORD (E0.2 §7.1's duty)

**The duty, verbatim** (`GwzM5-8R2E-SemanticsAmendment-DRAFT.md:1256-1263`):

> **Condition attached, for E4.7.** This decision rests on the legacy
> compensating observation continuing to run wherever the legacy `finish()`
> still executes. E4.7 must record, per converted consumer, whether
> `cleanup.rs`'s `finish()` is still reachable for it; if any converted
> consumer bypasses both the checked retirement and the legacy recheck,
> this decision reopens. *[2026-09-02: this duty SURVIVES E4.7's re-scope
> (`GwzM5-8R2E-CapabilityFreeAmendment.md` §4) — E4.7 keeps it; the
> converted consumers are E4.1, E4.2 and the E4.5/6-B forward arms; the
> legacy writer's own retirement is DR-1's, not E4.7's.]*

**The two mechanisms, located at `0dae0d5`.**

*The legacy recheck.* `cleanup.rs:26-180` `finish()`, reached ONLY through
`finish_replace` (`:9-16`) and `finish_remove` (`:18-24`), whose only
callers are `transition.rs:106` (`replace_exact`) and `transition.rs:167`
(`remove_exact`) — i.e. the pre-catalog `CheckedArtifact` boundary and
nothing else. The A-1 compensating observation is `cleanup.rs:146-158`
(re-inspect the family; require `rechecked.authority.as_ref() ==
Some(authority)`, no source residue, no goal residue, `observe_durable() ==
desired`) followed by `rebarrier_exact` at `:160` → `residue.rs:578-604`.
**It is UNCONDITIONAL within `finish()`**: the source-retirement block that
precedes it (`:96-145`) is guarded by `if let Some(source) = residue.source`,
but `:146-179` sits at the function's top level and runs on every call,
including a `Missing → Bytes` creation.

*The checked retirement.* The pre-catalog provider's source-associated
rename: `namespace_mutation.rs::execute_edge` (`:271-…`) proves the
destination free (`require_absent`, `:284`), re-observes the source
(`:288-297`), then publishes with
`PublicationSourceV1::regular_file(&source.encoded_identity,
&source.bytes)` (`:306`) through `publish_verified_no_replace`
(`publication.rs:196`), which verifies identity (`:213`) and bytes
(`:221-237`). *Cite drift note: A-1 cites `namespace_mutation.rs:280-288`
and `:263-265`; at `0dae0d5` those are `:306` and `:284`. `cleanup.rs:146-158`
is UNMOVED; `residue.rs:570-596` is now `:578-604`. E4.7's record must
re-cite against its own close tree.*

**The record, per converted consumer.**

| Converted consumer | Door | `finish()` reachable? | Checked retirement taken? | Reopens? |
| --- | --- | --- | --- | --- |
| **E4.1** — `activate_workspace_catalog` (`entry.rs:309-313`) | `catalog::recover_or_create` (`catalog/bootstrap.rs:233-237`) | **NO** | **YES** | **NO** |
| **E4.2 parent half** — `bootstrap_merge_start_parents` (`entry.rs:345-363`) | `admit_/execute_merge_start_managed_parents` (`coordinator/execution.rs:117,161`) | **NO** | **YES** | **NO** |
| **E4.2 record half** — `create_merge_store_record` (`entry.rs:371-391`) | `CheckedArtifact::acquire` + `replace_exact(Missing, goal)` (`entry.rs:390`) | **YES** | n/a — takes the legacy recheck | **NO** |
| **E4.5-B marker arm** (conditional) — `execute.rs:45` | `artifact_facts::write_checked` (`root/artifact_facts.rs:39-51`) → `entry::replace_merge_root_artifact` (`entry.rs:43`) → `replace_exact` | **YES** (predicted) | n/a | **NO** (predicted) |

**Traces.**

*E4.1 and E4.2's parent half bypass `finish()` but not the checked
retirement.* Neither path constructs a `CheckedArtifact` at all: a sweep of
`CheckedArtifact::` across `checked_artifact/catalog*`,
`checked_artifact/capability/`, `checked_artifact/bootstrap/` and
`checked_artifact/coordinator/` returns **only two doc-comment mentions**
(`bootstrap/managed.rs:10`, `coordinator/execution.rs:28`) and zero code
sites. `recover_or_create` (`catalog/bootstrap.rs:240-250`) drives
`CatalogOwnerEdgeV1` steps (`:258,269,275,282,289,295,300`) through the
pre-catalog permit, whose publications are
`publish_verified_no_replace` in `directory_mutation.rs:246,341,601,622,638`
and `mutation.rs:241`. `execute_merge_start_managed_parents`
(`coordinator/execution.rs:161-172`) reaches the same primitive through
`bootstrap_managed_parents` → the provider's `execute_row` →
`namespace_mutation.rs:301`. **Both take the checked retirement, so neither
bypasses "both". The decision does not reopen for either.**

*E4.2's record half takes the legacy recheck.* `create_merge_store_record`
ends at `artifact.replace_exact(&CheckedArtifactFact::Missing, goal)`
(`entry.rs:390`) → `transition.rs:105-106` (`publish_goal` then
`finish_replace`) → `cleanup.rs:15` → `finish()`. Because `expected` is
`Missing`, `authority.retained_source` is not `Existing`, so
`detach_existing` is skipped (`transition.rs:91-104`'s else arm) and
`finish()`'s source-retirement block is inert — **but `:146-158`, the exact
lines A-1 rests on, execute, and the non-self-checking `authority_name`
(`cleanup.rs:159`) is used only AFTER that recheck has proved the family
closed.** This is the strongest form of the A-1 answer: the compensating
observation runs on the first converted production write.

*E4.5-B's marker arm, conditionally.* Per `GwzM5-8R2E-E45-6B-CharterPrep.md`
§7.1 the recommended route is "reuse `artifact_facts`
(`root/artifact_facts.rs:39-51` / `:69-75`); **no new `entry.rs` door**" —
i.e. `entry::replace_merge_root_artifact` (`entry.rs:43-51`), which calls
`replace_exact` at `:50`. The prep §0.2 establishes the expected fact is
`Missing` (`WriteMarker` issues only from the `Baseline` prefix, which
requires `marker_absent`), so the shape is E4.2's exactly and `finish()`
is reachable with the same inert-source / live-recheck profile.
**E4.7's record must be written conditionally: "if E4.5-B lands via
`artifact_facts::write_checked`, `finish()` is reachable and A-1 holds; if
E4.5-B is instead built on a new door that bypasses `transition.rs`, the
record is re-taken before O1 closes."**

**VERDICT for E4.7's record: the E0.2 §7.1 decision does NOT reopen. No
converted consumer bypasses both mechanisms.** The `authority_name` rename
A-1 rejected stays rejected, and DR-1 inherits the question only through
the legacy writer's retirement (CapFree §4), not through A-1.

---

## 3. THE A-1 RIDER'S REOPEN CONDITION

**Home of the condition:** `GwzM5-8R2E-SemanticsAmendment-DRAFT.md:1256-1263`
(quoted in full in §2 above). Its DECISION is A-1 at `:1228-1254`; the
decision register row is `:1378`.

**Home of the travelling record:** `GwzM5-8R2E-E7-Acceptance.md`'s **O12
row**, which at `3351895` is line **`:181`**, not `:179` — CapFree §4 and
the plan `:612` both cite `:179`, which is now the O10 row. *Citation drift
of two lines, presumably from the §4 pointer bracket inserted at
`:15-20`.* The O12 row's operative words: *"rider 2 (A-1 re-framed) **with
its E4.7 reopen condition traveling to the E4 resumption**"*, and its
trailing bracket: *"[2026-09-02: the A-1 rider's E4.7 reopen condition
SURVIVES E4.7's re-scope and stays E4.7's to check —
`GwzM5-8R2E-CapabilityFreeAmendment.md` §4.]"* A second home is
E7-Acceptance `:308` (the residual table's E4 row, "the A-1 rider's E4.7
reopen condition").

**What E4.7 must check:** exactly the §2 matrix — per converted consumer,
is `cleanup.rs`'s `finish()` still reachable, and if not, is the checked
retirement taken instead.

**What the tree shows now:** the §2 verdict — NOT reopened. Two consumers
take the checked retirement (E4.1, E4.2-parents), one takes the legacy
recheck (E4.2-record), and E4.5-B's arm will take the legacy recheck on the
recommended route. **E4.7's record closes the rider as CHECKED — CONDITION
NOT MET — DECISION A-1 STANDS**, with the three drifted cites re-pointed at
the close tree and the E7-Acceptance `:181` line-number correction noted so
the next reader is not sent to the O10 row.

---

## 4. THE DEAD `gc_archived` FAMILY

### 4.1 What it is, and its true extent

| Item | Site | Callers at `0dae0d5` |
| --- | --- | --- |
| `gc_archived` | `v1_lifecycle/archive.rs:112-118` | **test-only** — `v1_lifecycle/tests/gc.rs:18,34,47,65,112,130,167` |
| `gc_archived_with_hook` | `v1_lifecycle/archive.rs:120-150` | `gc_archived` (`:117`) + **test-only** `tests/gc.rs:197,211` |
| `remove_archive` | `v1_lifecycle/archive.rs:191-208` | `gc_archived_with_hook` (`:148`) ONLY |
| `require_same_archive` | `v1_lifecycle/archive.rs:210-226` | `gc_archived_with_hook` (`:141,147`) ONLY |
| `preflight_archived_cleanup` | **`merge/gc.rs:35-…`** (allow at `:31-34`) | `archive.rs:136` ONLY |
| `delete_preflighted_backup_refs` | **`merge/gc.rs:114-…`** | `archive.rs:137` ONLY |
| `require_backup_refs_absent` | **`merge/gc.rs:126-…`** | `archive.rs:142` ONLY |
| `PreparedArchivedCleanup` | **`merge/gc.rs:18-29`** | the three above ONLY |

**Finding CapFree §5 does not carry: the family is SEVEN functions plus one
struct across TWO files, not the three §5 names.** `merge/gc.rs`'s cleanup
trio and its `PreparedArchivedCleanup` struct are downstream-dead of
`gc_archived` and carry the byte-identical A1 allowance reason at
`gc.rs:31-34`. `acquire_archived` (`archive.rs:62`) is **NOT** part of the
family — it has live callers at `:91,103` on the archive-recovery path and
must survive either disposition.

### 4.2 What its tests pin

`v1_lifecycle/tests/gc.rs` (390 lines) holds six `gc_archived` tests plus
shared fixtures:
`gc_deletes_valid_no_ref_v0_and_v1_archives` (`:11`),
`gc_requires_global_open_record_absence_and_a_supported_archive` (`:27`),
`gc_restarts_from_an_absent_prefix_and_keeps_stash_bundles` (`:53`),
`gc_full_preflight_retains_every_ref_on_later_mismatch_or_missing_repo` (`:89`),
`gc_full_preflight_rejects_a_symbolic_ref_before_any_deletion` (`:143`),
`gc_rereads_archive_identity_and_every_ref_before_unlink` (`:194`).
The seventh test in the file,
`post_gc_retention_retires_marker_only_rows_and_keeps_markers_on_stash_rows`
(`:331`), does **not** call the family and must survive.

They pin: v0/v1 archive decode parity on the delete path; the typed
`ErrorCode::OpenOperation` refusal when any open record exists; restart from
a partially deleted ref set with stash bundles preserved; all-or-nothing
ref retention on later mismatch; symbolic-ref rejection before any
deletion; and the identity + ref re-read immediately before unlink. **These
are genuine semantics — but semantics of a route with no production
consumer, and CapFree §5 says the route "has no consumer to arrive" unless
DR-1's (C) resurrects the archive conversion.**

### 4.3 The two dispositions, costed

**(A) DELETE the family, its tests, and the allowances.**

- Removes ~120 production lines (`archive.rs:106-150,191-226`) + ~110
  (`merge/gc.rs:18-29,31-150` minus `cleanup_error`, which
  `handle_gc` may share — **the builder must check `cleanup_error`'s other
  callers before cutting**) + ~330 test lines.
- **Machine-pin cost 1 — the O13 inventory SHRINKS.**
  `archive.rs`'s two `sync_dir` references are `use crate::durable_fs::sync_dir;`
  (`:13`) and `sync_dir(done)` inside `remove_archive` (`:207`). Deleting
  `remove_archive` drops the call, and the now-unused `use` must go too, so
  `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES`'s row
  `"workspace_ops/merge/v1_lifecycle/archive.rs": {"sync_dir": 2}`
  (`check_checked_artifact_boundaries.py:370`) becomes **0 and the file
  leaves the map**. Under CapFree §3 the map is fail-closed BOTH ways and
  **shrinkage means "a conversion of a carved arm may not land without
  revising this amendment"** — the P-1 message class. A deletion is not a
  conversion, but the checker cannot tell them apart, so the step must
  edit the data row AND record why, which is an amendment-adjacent act.
  CapFree §3's own §7 note (4) also lists `v1_lifecycle/archive.rs` among
  the 3 of 19 carved files that are digest-pinned.
- **Machine-pin cost 2 — two digests move.**
  `PROTECTED_SOURCE_TREE_DIGESTS["workspace_ops/merge/v1_lifecycle/mod.rs"]`
  (`:240`) covers the whole `v1_lifecycle/` subtree (verified:
  `source_tree_digest`, `:1065-1081`, roots named `mod.rs` cover their
  parent directory), so both `archive.rs` and `tests/gc.rs` are inside it.
  `merge/gc.rs` is unpinned.
- **Record needed:** a dated deletion record naming (i) the O13 row's
  removal with CapFree §5 as authority, (ii) the six retired tests and the
  properties that leave with them, (iii) the explicit statement that if
  DR-1's (C) resurrects the archive conversion, the route is rebuilt from
  the record rather than resurrected from git.
- **Irreversibility:** low — `git` retains it, and DR-1 would rebuild
  against a changed boundary anyway.

**(B) KEEP with a permanent-pending-DR-1 reason.**

- Zero production churn. Two reason rewrites (`archive.rs:108-111`,
  `merge/gc.rs:31-34`), byte-exact:

  > `reason = "PERMANENT PENDING DR-1: the checked archive route this family \`
  > `          was built for has no consumer to arrive — the archive is carved \`
  > `          out (dev-docs/GwzM5-8R2E-CapabilityFreeAmendment.md §3, ADOPTED \`
  > `          2026-09-02) and E4.4 does not start (§7). O8's gc_archived route \`
  > `          RE-OWNS to DR-1, conditional on (C) resurrecting the archive \`
  > `          conversion (§5). The live GC deletion path is store/gc.rs and \`
  > `          store/retention.rs, which this family is NOT."`

- No pin moves at all: the counts, the tree digest and the map row all
  stay. The six tests keep executing and keep the semantics honest for a
  DR-1 rebuild.
- **Cost:** ~230 production lines of dead code and ~330 test lines carried
  across a phase boundary on the strength of a conditional.

### 4.4 RECOMMENDATION: **(B) KEEP with the permanent-pending-DR-1 reason.**

Three grounds, in order of weight:

1. **(A) makes E4.7 an amendment-adjacent step.** The O13 shrinkage arm was
   installed at E4.3-B precisely to make an unreviewed reduction impossible;
   firing it inside a docs-and-allowances step inverts the mechanism's
   purpose. CapFree §3's own words for shrinkage are "may not land without
   revising this amendment". E4.7 is not the tier that revises it.
2. **DR-1 decides the route's fate within one step of E4.7.** CapFree §5
   routes `gc_archived` to DR-1 conditional on (C), and plan `:616-626`
   opens DR-1 immediately after E4.7. Deleting a family whose owner
   convenes next is churn with a real chance of being undone.
3. **The seven-item extent §5 does not name is itself an argument for
   caution.** A charter written to §5's three names would have left
   `merge/gc.rs`'s trio orphaned and its allowance stale — exactly the
   failure mode E4.7 exists to close. Keeping the family lets E4.7 fix the
   *records* (both allowances, the extent, the live-path distinction)
   without a cut whose full surface no adopted record has enumerated.

**Route the DELETE option to DR-1 as a named agenda sub-item** ("delete the
`gc_archived` family, or rebuild it against (C)'s boundary"), so the choice
is made where the architecture is, and record in E4.7 that (A) was
considered, costed and deferred — not overlooked.

---

## 5. O1's CLOSE — THE DISCHARGED TEXT, ROW BY ROW

### 5.1 The obligation as re-scoped

Plan `:46`'s bracket, verbatim:

> *[2026-09-02: obligation text RE-SCOPED (the SECOND re-scope, after ruling
> "a"): "executed as far as E0.2 §5.2 permits — every row dispositioned by
> name: converted (E4.1, E4.2, the `finalization/execute.rs` forward arms),
> carved by dated exception (`GwzM5-8R2E-CapabilityFreeAmendment.md`; the
> record-root amendment), or already-converted-and-dated (its §6)"; closes
> DISCHARGED at E4.7 against this text, citing O14 as RE-OWNED to DR-1 —
> `GwzM5-8R2E-CapabilityFreeAmendment.md` §4.]*

Framing (i) at E7-Acceptance `:15-22` supplies the reading discipline: R2-E
closes against its **amended** object, and the amendment — not silence — is
the authority that re-scopes it.

### 5.2 The nine rows, dispositioned by name

`GwzM5-8R4bR2ConsumerCheckpoint.md` §10 rows `:272`-`:280`:

| Row | Consumer | Disposition | Evidence at `0dae0d5` |
| --- | --- | --- | --- |
| `:272` | `WorkspaceMutatorLock` | **CONVERTED — E4.1** | `entry.rs:309-313` `activate_workspace_catalog`; production callers `dispatch.rs:453`, `checked.rs:161`; pinned at exactly one namer by `interface_tests/catalog_activation_pin.rs:45` |
| `:273` | first merge record | **CONVERTED — E4.2**, both halves | parents: `entry.rs:345-363` → `coordinator/execution.rs:117,161`; leaf: `entry.rs:371-391` `create_merge_store_record`, called from `store/rewrite.rs:63`; frozen clause one driven at `v1_lifecycle/tests/checked.rs:12` |
| `:274` | merge record rewrite | **CARVED BY EXCEPTION — RR §2** (the record-root exception), permanent | `store/rewrite.rs` keeps `rename_durable`; checker row `:372` + `V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS` (`:382-384`) |
| `:275` | terminal archive source-only | **CARVED BY EXCEPTION — CapFree §3**; no forward arm exists | `store/archive.rs` raw writers (checker row `:371`); the archive move is atomic (`rename_noreplace`) |
| `:276` | ordinary/merge stash bundle | **CARVED BY EXCEPTION — CapFree §3**; E4.5-B VACUOUS here (no forward bundle writer) | CapFree §7; E45-6B prep §7.1 (`forward.rs:71-76` refuses `Preservation`) |
| `:277` | commit or merge marker | **SPLIT: raw arms CARVED (CapFree §3); forward arm `execute.rs:45` CONVERTED by E4.5-B — PENDING** | E45-6B prep §0.2/§7.1: `:45` is window-free, E4.5-B chartered as one step in E4.2's shape |
| `:278` | workspace marker/lock checked mutation | **SPLIT: raw arms CARVED (CapFree §3); forward arm `execute.rs:48` = the [R2-P3-1] DATED RESIDUAL** | CapFree §7's E4.5/6-B note: detach-then-publish opens an observation-dead window; E4.6-B does not open |
| `:279` | `.git/info/exclude` | **SPLIT: raw arms CARVED (CapFree §3); forward arm `execute.rs:51` = the [R2-P3-1] DATED RESIDUAL**; cell-2 wording → DR-1 | same note; the shipped v1 reverse writer already uses the WORKSPACE door (`abort/evidence.rs:148-153`) |
| `:280` | v1 checked store/root/bundle paths | **PERMANENT-DOCUMENTED in all three inventory rows** (creation converted E4.2; rewrite carved RR; archive carved CapFree) — the O13 inventory never empties | plan `:88-115`'s three brackets; checker `:326-374` |

Plus, outside the table: **already-converted-and-dated** — CapFree §6's
three abort paths (published evidence `abort/evidence.rs:148-165`;
`--preserve` with an integrated participant `preservation/entry.rs:102-104`;
a selected `@root` `validate/journal.rs:253-258` → `root/abort.rs:131,145,380`),
shipped with A1's v1 reverse-path conversion and now scoped BY PATH.

### 5.3 The DISCHARGED text E4.7 should write

Cast in E7-Acceptance §4's row form (`:168-182`), so the close is
row-shaped like every other:

> | O1 | **DISCHARGED at E4.7 against the re-scoped text (plan `:46`, the
> SECOND re-scope)** — every §10 row dispositioned by name: **converted**
> `:272` (E4.1, `entry.rs:309`), `:273` (E4.2, `entry.rs:345`/`:371`),
> `:277`'s forward arm (E4.5-B, `execute.rs:45`); **carved by dated
> exception** `:274` (`GwzM5-8R2E-RecordRootAmendment.md` §2), `:275`,
> `:276`, and the listed-operation writers of `:277`/`:278`/`:279`
> (`GwzM5-8R2E-CapabilityFreeAmendment.md` §3, on the operator's
> 2026-09-02 ruling); **dated residual** `:278`/`:279`'s forward arms
> `execute.rs:48,:51` ([R2-P3-1], CapFree §7's E4.5/6-B disposition —
> the observation-dead window); **permanent-documented** `:280` via O13,
> which never empties; **already-converted-and-dated** — CapFree §6's three
> abort paths, scoped BY PATH. O13's substantive half closes with this row.
> **O14 is cited as RE-OWNED to DR-1**, not as discharged: the write-authority
> fork and the legacy in-place-writer retirement decide there
> (CapFree §4/§5). | `GwzM5-8R2E-CapabilityFreeAmendment.md` §3/§4/§6/§7;
> `GwzM5-8R2E-RecordRootAmendment.md` §2/§4; plan `:46`, `:88-115`,
> `:138-151`; E7-Acceptance `:170` (this row's prior RE-OWNED form) and
> its framing (i) `:15-22`; `GwzM5-8R2E-E45-6B-CharterPrep.md` §7.1 |

**Two conditions on that text, both to be stated in the charter:**

1. **`:277` reads "converted" only if E4.5-B actually lands.** CapFree §7:
   *"should E4.5/6-B fail its own loop terminally, the arms fall to the
   [R2-P3-1] residual record and O1's text is re-annotated — not
   silently."* E4.7 therefore sequences **after** E4.5-B, or it writes
   `:277`'s cell conditionally and re-annotates.
2. **The close must not read O14's re-own as a discharge.** Plan `:145`
   requires O1's close to cite O14's disposition; CapFree §4 chose the
   sequencing explicitly ("not awaiting DR-1's disposition, which is
   post-phase"). The row above satisfies both by naming O14 as re-owned.

**Verification against E7-Acceptance's O1 row form:** row `:170` currently
reads **RE-OWNED with a named carrier**, with two dated brackets already
appended, the second of which says *"O1's obligation text is now RE-SCOPED
a second time (plan `:46`) and closes DISCHARGED at E4.7 citing O13,
`GwzM5-8R2E-CapabilityFreeAmendment.md`, the record-root amendment, and O14
as RE-OWNED to DR-1."* **The text above matches that promise exactly**, and
E4.7's act at that row is a third dated bracket recording the discharge and
pointing at E4.7's own record document — not a rewrite of the row.

---

## 6. THE CLOSE-OUT RECORDS LIST

E4.7's record document IS the Phase-E4 close. Modelled on
`GwzM5-8R2E-E7-Acceptance.md`'s form, it must contain:

1. **Header and object.** Status line, the close tree's exact SHA, the
   charter source (plan `:597-615`), the authorities (CapFree §4 as the
   narrowing instrument; the operator's ruling verbatim where it bites),
   and the two framing statements the phase closes against — the re-scoped
   milestone (plan `:359`) and the re-scoped O1 (plan `:46`).

2. **The ledger close, row by row** (E7-Acceptance §4's table form):
   **O1** DISCHARGED (§5.3 above); **O2** DISCHARGED (E4.1 landed the
   production activation; the tripwire pins one namer);
   **O3** — no act: DISCHARGED already at R1.3 2026-09-01, and the
   "stronger E4.7 full-retirement mechanism" is RE-OWNED to DR-1
   (CapFree §4) — record the re-own, do not reopen the discharge;
   **O8** — the `gc_archived` half dispositioned (§4), the rest already
   split-closed and re-owned to DR-1;
   **O12** — the A-1 rider's reopen condition CHECKED and NOT MET (§3),
   which is the last live sub-item of a row already DISCHARGED at E7;
   **O13** — substantive half closes with O1; the inventory recorded as
   permanent in all three rows;
   **O14** — RE-OWNED to DR-1, cited not closed.

3. **The pin census at the close tree** (E7-Acceptance §6's form):
   CA / v1 / remainder measured on darwin with the linux derivation, in the
   E4.2/E4.3-B idiom (the last recorded is CA 457 / v1 260 / remainder
   1110+1 darwin MEASURED at E4.3-B — checkpoint `:2760-2761`); plus the
   machine-pin family's state: `PROTECTED_SOURCE_DIGESTS`,
   `PROTECTED_SOURCE_TREE_DIGESTS`, `ENTRY_REFERENCES`,
   `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` +
   `V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS`, the capability-free
   inventory the pins package installs, and
   `interface_tests/catalog_activation_pin.rs`'s count (expected: still 1).

4. **The matrix acceptance by citation** — three platforms at the close
   tree, in E7-Acceptance §2's form; if E4.7 is docs-and-allowances only,
   the citation is the landing's own matrix run rather than a fresh
   three-platform campaign, and the record must say which.

5. **The residual table** (E7-Acceptance §7's form), carrying forward:
   [R2-P3-1] (`execute.rs:48,:51` staying raw); the capability-free carved
   inventory as a permanent documented exception; the record-root
   exception; the abort sentence's by-path narrowing dated as cured only by
   DR-1's (C); row `:279`'s cell-2 discrepancy; the `gc_archived` family on
   the recommended disposition; tier-2 archive equivalence; anything the
   pins package left named.

6. **The DR-1 handoff** — its agenda transcribed from CapFree §5 (the three
   ruling items: (C), reader-side record reconciliation, O14's fork; the
   six lane-routed items: tier-2's sub-surface, the record-root
   re-examination, §6's already-shipped narrowing, the legacy in-place-writer
   retirement + the retire-the-area question, O8's route, and — added by
   this prep — the `gc_archived` delete-or-rebuild choice), plus **the
   operator's pending home question, restated not answered**: *"DR-1 opens
   after E4.7, outside Phase E4 and outside the plan's phase list — is it
   R2-E's (a new phase E8) or a new lane?"* (CapFree §5; plan `:625-626`).

7. **The checkpoint entry** — one entry carrying: the phase close, the
   allowance-class disposition split with its counts, the `finish()`
   reachability verdict, the `gc_archived` disposition, O1's DISCHARGED
   close, and — per CapFree §4 — **one sentence making the E4.7 → DR-1
   re-own of the legacy-writer retirement visible to the operator**, plus
   the NEXT line naming DR-1.

---

## 7. RECOMMENDED CHARTER SHAPE

### 7.1 One step, two builders' worth of work, one review

**E4.7 = ONE reviewed step, docs + allowances, no production behaviour
change.** Split by artefact, not by review:

- **Opus builder (gwz-core half):** the 23 actionable allow sites (2 proven
  deletions, 1 measured candidate, 18 re-reasons, 2 on the `gc_archived`
  disposition), the 12 prose/comment/checker rewrites, the digest and
  count-map re-pins the edits force, and a green full battery. **Cap ~400
  LOC** (almost all comment text; production deletions are ~8 lines).
- **Lane owner (root half):** the record document
  `GwzM5-8R2E-E47-PhaseClose.md` (§6's contents), the dated brackets at
  every dev-docs home CapFree §4 names, and the checkpoint entry.

**Review tier:** single-axis interior review, per plan `:628` ("Each step:
single-axis review, matrix at the landing, T-6-class tripwires
re-verified"). This step is not amendment-tier: it executes CapFree §4, it
does not amend it. **If the `gc_archived` DELETE option is chosen instead of
the recommended KEEP, the tier question reopens** — see §4.3(A)'s shrinkage
argument.

### 7.2 Gates — allowance edits move digests; name the trees

Every edit below moves a machine pin. The charter must name them so the
builder re-pins deliberately rather than discovering a red gate:

| Edited file | Pin that moves | Site |
| --- | --- | --- |
| `checked_artifact/mod.rs` | `PROTECTED_SOURCE_DIGESTS` (flat) | checker `:58` |
| `checked_artifact/entry.rs` | `PROTECTED_SOURCE_DIGESTS` (flat) | checker `:56` |
| `operation/workspace_mutator_lock.rs` | `PROTECTED_SOURCE_DIGESTS` (flat) | checker `:73` |
| `checked_artifact/catalog.rs` | `PROTECTED_SOURCE_TREE_DIGESTS` (tree: `catalog.rs` + `catalog/**`) | checker `:237` |
| `checked_artifact/capability/pre_catalog/provider.rs` | `PROTECTED_SOURCE_TREE_DIGESTS` (tree: `pre_catalog.rs` + `pre_catalog/**`) | checker `:236` |
| `v1_lifecycle/archive.rs`, `v1_lifecycle/tests/gc.rs` | `PROTECTED_SOURCE_TREE_DIGESTS` (tree: `v1_lifecycle/**` via its `mod.rs` root) | checker `:240` |
| `v1_lifecycle/archive.rs` (only if DELETED) | `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` row + the shrinkage arm | checker `:370` |
| `checked_artifact/interface_tests/catalog_activation_pin.rs` | none (assert strings only) | — |
| `checked_artifact/coordinator/mod.rs`, `bootstrap/managed.rs`, `catalog_names.rs`, `namespace/mod.rs`, `merge/gc.rs`, `git/tests/g12.rs` | **unpinned** — no digest | — |

Tree-digest semantics verified at `check_checked_artifact_boundaries.py:1065-1081`:
a root named `mod.rs` covers its parent directory recursively; any other
root covers itself plus its same-stem directory.

Second gate class: `ENTRY_REFERENCES` (checker `:392-419`) is a comment-plus-data
inventory whose comment at `:398` is a class member; editing the comment
does not move the data, but the builder must not "tidy" the data sets.

Third gate: `interface_tests/catalog_activation_pin.rs`'s
`PRODUCTION_CALLER_COUNT` must stay **1** and `OWNER_CALL_SHAPED` must stay
**3** — E4.7 converts nothing.

### 7.3 Stop-triggers

1. **An "expired" allow whose removal reddens `cargo check`.** Fires for
   `workspace_mutator_lock.rs:44-47`, `namespace/mod.rs:47-51`, or
   `checked_artifact/mod.rs:27-32`. Meaning: this prep's reachability
   reading is wrong. **STOP, re-reason as permanent instead of deleting,
   and report the failing item** — do not add a narrower allow to make the
   deletion stick.
2. **A re-reason that turns out to be an expiry.** If removing a
   "permanent" allow is green, the item is live and the reason would be a
   lie. **Report before writing it.**
3. **The `gc_archived` DELETE path firing the O13 shrinkage arm.** If the
   operator or lane owner overrides the §4.4 recommendation, the checker
   will refuse the reduction by design. **STOP and route to the amendment
   tier** — do not edit `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` to silence
   it.
4. **The pins package having already written a line E4.7 is chartered to
   write** (`archive.rs:108-111`, `catalog_names.rs:44`, the checker's O13
   block, the `E4.2-E4.6` class annotations). **Report the collision rather
   than overwriting**; the seam is §0.5's open item.
5. **E4.5-B not landed when E4.7 opens.** Then `:277` cannot read
   "converted" and O1's close is conditional. **Report and either wait or
   write the conditional form explicitly** — CapFree §7 forbids the silent
   version.
6. **A `finish()`-reachability trace that contradicts §2.** If E4.5-B
   landed on a door that bypasses `transition.rs`, A-1's reopen condition
   is live and this is an escalation, not a record.

### 7.4 Sequencing

```
  [main 0dae0d5]
        │
        ├── pins package E4.4-6-B  (builder b99bfb7 exists; NOT on main)
        ├── GC decode fix (standalone, in flight)
        ├── E4.5-B (marker arm; carries the :48/:51 residual sentence)
        │
        └── E4.7  ← docs + allowances; its record document IS the phase close
                 │
                 └── DR-1  (chartered once the operator names its home)
```

E4.7 opens **after** E4.5-B lands, for two reasons this prep establishes:
O1's `:277` cell (§5.3 condition 1), and the `finish()`-reachability record's
fourth row (§2), which is conditional until the arm's door is known. It
opens after the pins package for the third: the pins package writes the
class annotations E4.7 then expires, and running them in the other order
means E4.7 writes text the pins package overwrites.

**E4.7's record document IS the Phase-E4 close** — plan `:597-615` gives it
"the phase close-out records", and no separate acceptance step exists
between it and DR-1. It should therefore be named and shaped as an
acceptance record (`GwzM5-8R2E-E47-PhaseClose.md`), not as a step report.

---

## 8. Falsification log — what this session checked against the authorities

| Authority claim | Checked | Result |
| --- | --- | --- |
| CapFree §4's allowance class is the class | full `#[allow]` extraction + string sweep | **INCOMPLETE** — 6 unnamed actionable members found (§1.3), and the prescribed needles miss §5's own `archive.rs:110` |
| `workspace_mutator_lock.rs:44-47` is a stale allow (E4.1 [P3-5]) | caller grep | **CONFIRMED, and stronger** — 4 production callers; the allow suppresses nothing |
| §5's `gc_archived` family is three items | caller graph across both files | **UNDER-ENUMERATED** — 7 functions + 1 struct across 2 files; `merge/gc.rs:31-34` carries the twin allowance |
| §5: "either deletes the family or re-reasons" is a free choice | O13 shrinkage arm + tree digest | **CONSTRAINED** — deletion fires the fail-closed shrinkage arm and moves a tree digest |
| A-1's reopen condition lives at E7-Acceptance `:179` | line read | **DRIFTED** — `:179` is the O10 row; O12 is `:181` |
| A-1's checked-path cites (`namespace_mutation.rs:280-288`, `:263-265`; `residue.rs:570-596`) | line reads | **DRIFTED** — now `:306`, `:284`, `:578-604`; `cleanup.rs:146-158` is UNMOVED |
| E0.2 §7.1: `finish()` may be bypassed by a converted consumer | four traces | **NOT REALISED** — no consumer bypasses both mechanisms; A-1 does not reopen |
| `catalog.rs:19-22`'s two named dead items are still dead | caller greps | **CONFIRMED** — production sites are definitions/delegations only |
| E4.5-B adds a `recover_or_create` namer | E45-6B prep §5.1 + `ENTRY_REFERENCES` | **NO** — the pin stays at 1 |
| The pins package has landed | `git log`, `git cat-file` | **NO** — `b99bfb7` is in the object store, not on main `0dae0d5` |
| `namespace/mod.rs`'s `mod host` allow was discharged at E7 | E7-Acceptance §4/§5/§7 sweep | **NO** — its self-declared "E7 owes this allow a dated re-owning" is undischarged and falls to E4.7 |
