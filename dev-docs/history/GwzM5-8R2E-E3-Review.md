# R2-E Phase E3 — interior review (single-axis, peer-blind), 2026-08-27

**VERDICT: NO-GO — ESCALATE.** One [P1], two [P2], five [P3]. The lane owner
takes the second look before this train lands.

**Object.** Branch `e3-terminal`, worktree
`/private/tmp/claude-501/-Users-owebeeone-limbo/3a4f2794-661b-40df-b586-1f52221017f3/scratchpad/e3-worktree`,
two commits over `94da3e5`:

- `b9cb795` — R2-E E3.1 precondition: the T1 widening.
- `04c8a66` — R2-E E3: the `terminal.*` sites and the executed matrix (10 of 11).

**Authority reviewed against.** `GwzM5-8R2E-Plan.md` §0-§1.1, §2, §3 Phase E3;
`GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §4.1-§4.5;
`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §2, §3, §6.1-§6.3, §8 ([P2-4],
[P2-7], State [P2-3]), §9 — controlling;
`GwzM5-8R2DInterfaceFreeze.md` §3.5 terminal records, §4.4 Class 1 / Class 2 and
their 2026-08-27 E0 annotations, §4.3 row E7, §6, the §3.1 persisted-home pin.

**Evidence discipline.** Every claim below rests on the diff, the charter, or a
probe I executed myself. Two probes were run in a byte-clone of the worktree
(`scratchpad/probe-tree`, `cp -Rc`), never in the object. Nothing under version
control was edited, staged, or committed.

---

## 0. Findings, by grade

| # | Grade | Axis | Finding |
| --- | --- | --- | --- |
| F1 | **[P1]** | 2 (the bound) | The T1 widening makes `interior::observe` ↔ `interior::observe_slot` **mutually recursive with no depth limit**. A nested `retired-actions-v1` chain in the retired root aborts the process (`SIGABRT`, stack overflow) instead of returning a typed refusal. Reproduced. |
| F2 | **[P2]** | 5 (freeze surface) | Freeze §4.4's arm table gains a **Class-1 source-interior arm it does not assign** (`DirectoryInteriorExpectationV1::TerminalActionDirectory`). The train flags it and hands the ruling here. **RULE: forced, shape acceptable, and the arm table DOES need a row.** ESCALATE — this is a frozen-surface edit an interior single-axis review cannot authorise. |
| F3 | **[P2]** | 5 (the keys' semantics) | §4.3 rows **#2 and #3 announce durable state the code does not bind**. #2 performs no read of the payload rows at all; #3 never invokes the frozen `Complete` classifier and proves residency only — and the matrix fixture proves it must not, since its worklist rows carry a fabricated fingerprint the classifier would reject. Recorded in doc comments, **not** among the commit's flagged deviations, and §4.3's rows are unamended. ESCALATE — the activation record headed for freeze §3.5 would install a false semantic. |
| F4 | [P3] | 4 (T-C′) | The one new forward returns `&Dir`. Its doc cites `retain_action_namespace`'s contract ("never the catalog handle and never a path") but restates it as "a typed capability **or a sibling-owner handle**" — words the quoted sentence does not contain. Not a capability escape (see §4); a doc-honesty nit. |
| F5 | [P3] | 3 / 6 | `reserve_retired_slot` **asserts** `CatalogAdmissionOccupancyV1::Idle` rather than observing it, undisclosed. Understates the credit rule's `outstanding` by one whenever a durable `Preparing` admission is resident. |
| F6 | [P3] | 7 (matrix honesty) | E0.2b §8's corrected [P2-4] duty — "drive the **first** attempt through a real admitted action" — is not literally met: every attempt, first included, uses the test-only issuer. Mirrors the cited precedent exactly, so inherited rather than introduced. |
| F7 | [P3] | 7 | The converged restart path returns `Ok(())` without the retired-root flush (#8) or the catalog-root barrier (#9). Matches the corpus idiom; recorded, not charged. |
| F8 | [P3] | 8 (companions) | A **third** protected tree (`checked_artifact/catalog.rs`) had to be re-pinned, beyond §6.2(b)'s two-tree inventory. Correct and honestly recorded in the dict; the addendum's §6.2(b) inventory now understates what E1/E2 will hit. |

Escalation rule applied: F1/F2/F3 are [P1]/[P2] ⇒ **ESCALATE**.

---

## 1. Axis 1 — authorization fidelity

**Verdict: clean.** Every predicate edit is inside E0.2b §2's exact scope, and I
walked the whole predicate surface rather than sampling.

**The three gates, and only the three.** A tree-wide grep for `EmptyDirectory`
returns ten hits and no eleventh:

- the variant declaration (`provider.rs:210`);
- the producer (`interior.rs:497`);
- gate 1 — `completed_record`, now `retired_root_identity(interior)?`
  (`interior.rs:373`);
- gate 2 — `retain_directory`'s match (`completed.rs:405`);
- gate 3 — `require_named_directory_identity`'s match (`completed.rs:491`);
- `staging_plan`'s **preserved** arm (`interior.rs:234`);
- the two helpers (`interior.rs:884`, `:906`) and `snapshot.rs:210`.

So E0.2b §2.2's claim that the package is exactly three gates is **exhaustive on
this tree**, and no fourth `EmptyDirectory` requirement at `RetiredActions`
survives the widening.

**`staging_plan` is preserved by name, and pinned.** `interior.rs:247` carries an
explicit `Some(RawCatalogInteriorFactV1::RetiredActionRoot { .. }) => { return
StagingPlanV1::Other; }` arm ahead of the catch-all, with the §2.2 quotation as
its ground. It is additionally pinned by a source-text assertion —
`tests_retired_root.rs:231` requires the literal arm text to be present — so a
future reshaping of the fact type cannot silently widen the bootstrap's adoption
grammar via fall-through. That discharges §2.4 item 1's "preserved by name"
clause with more than a comment.

**Blast radius of the new fact.** `observe` passes `probe_empty_directory` as
`slot == InfrastructureSlotV1::RetiredActions` (`interior.rs:112`) — unchanged —
so `RetiredActionRoot` can only ever be minted for the retired root. The old
`empty_directory_identity(interior, slot)` took a slot parameter and had exactly
one caller; replacing it with the slot-fixed `retired_root_identity(interior)` is
a narrowing, not a widening.

**Nothing else changes semantics.** `interior.rs`'s only other edit across the
two commits is the additive `ActionInteriorObservationV1::is_reservation_exact`
(commit 2); `completed.rs`'s are the two match widenings plus two additive
methods. The `snapshot.rs` framing adds tag `4` — no collision with the
interior-fact tags 1/2/3 already in `frame_catalog_interior`, and an empty
retired root still frames as tag 1 with its identity alone, so no already-recorded
digest moves. `RetiredActionsDescriptor` / `CatalogFormat` are untouched:
`completed_record` still consumes the retired root's own durable identity, which
a child addition does not change.

**§6 / §3.1.** Zero new durable records, slots, names, or fault keys. The new
fact is provider-private; the digest it feeds is the in-memory pre-catalog
snapshot, not a persisted record.

**§2.4 item 3 — the first production caller.** `admission/driver.rs:125-135`
replaces the bare `census.active_actions >= MAX_ACTIVE_ACTION_DIRS` stop with
`CatalogOccupancyV1::new(...).can_admit_new()`, and the owed-to-Phase-4 note is
withdrawn at its own site with the refuting ground stated. The hardcoded
`CatalogAdmissionOccupancyV1::Idle` **is honest here**: the call sits inside the
`AdmissionDriveV1::Idle` arm, so `Idle` is observed, not assumed. (Contrast F5.)

---

## 2. Axis 2 — the bound. **[P1] F1**

### 2.1 What the charter demanded

E0.2b §2.4 item 2: *"A **bounded** reading of the retired root … with the count
checked **explicitly against `MAX_RETIRED_ACTION_DIRS`** … and **not** inherited
from `interior::observe`'s own caps"*. §3.2 ground 3 repeats it for the T-B′ arm
and adds *"a stated sentence that the arm does not inherit its bound"*.

### 2.2 What is discharged

The explicit comparison is present at **all three** new readings, and the stated
sentence at two of them:

- `interior::retired_root_identity` — `*infrastructure_rows == 0 &&
  *retired_action_dirs <= MAX_RETIRED_ACTION_DIRS`, with the not-inherited
  paragraph in its doc comment naming the three real constants
  (`MAX_INTERIOR_ENTRIES`/`MAX_ROOT_ENTRIES` = 74, `MAX_ACTIVE_ACTION_DIRS` = 64).
- `interior::retired_action_dirs` — the same guard.
- The T-B′ arm (`publication.rs`) — `!retired.rows.is_empty() ||
  retired.action_rows.contains(child) || retired.action_rows.len() >=
  MAX_RETIRED_ACTION_DIRS`, with its own not-inherited paragraph.

The `>=` in the arm versus `<=` in the read predicates is correct, not a drift:
the arm is about to *add* one row, so refusing at 64 leaves at most 64 after the
edge, which is exactly what the predicates then accept.

The guard fails closed in both directions of a future constant edit: lowering
`MAX_RETIRED_ACTION_DIRS` is caught by the explicit comparison; raising it above
`MAX_ACTIVE_ACTION_DIRS` leaves the reused reader capping earlier, i.e. refusing
more than required. Good.

**The infrastructure-rows-must-be-zero refusal is real.** I did not take it on
the code's word. The object carries two driven rows for it —
`a_retired_root_holding_an_infrastructure_slot_name_still_refuses`
(plants `retired-actions-v1`) and
`a_retired_root_holding_an_infrastructure_row_refuses_the_retirement`
(plants `catalog-format-v1`, and additionally asserts the refusal lands **before**
the rename) — and both are green in my own execution (§9). The mechanism is as
documented: `exact_row` is parent-independent, so an infrastructure-slot name
planted in the retired root classifies into `rows` rather than being refused, and
`rows.is_empty()` is the clause that catches it.

### 2.3 What is NOT discharged — F1

**Depth is not bounded at all, and a corrupt retired root aborts the process.**

`observe_slot`'s new arm (`interior.rs:509`) calls `observe(&child, platform)?`.
`observe` in turn calls `observe_slot(..., slot == RetiredActions, ...)` for every
infrastructure-slot child (`interior.rs:109-114`). `exact_row` is
**parent-independent** (`interior.rs:418-442`: it walks `InfrastructureSlotV1::ALL`
with no notion of which parent it is reading), so a `retired-actions-v1` child of
the retired root is a perfectly classifiable infrastructure row — and the pair
re-enters itself. There is no depth counter, and every budget in the loop
(`CatalogNameBudgetV1`, `MAX_INTERIOR_ENTRIES`, `MAX_ACTIVE_ACTION_DIRS`) is
allocated **per level**.

Before `b9cb795` there was no recursion: `observe_slot` had no self-call at all.
Verified against the base:

```
$ git show 94da3e5:src/.../provider/interior.rs | grep -n "[^_a-z]observe("
63:pub(super) fn observe(          <- the definition only

$ grep -n "[^_a-z]observe(" src/.../provider/interior.rs        # at HEAD
64:pub(super) fn observe(
509:            let inner = observe(&child, platform)?;          <- new
```

**Probe (executed, in the clone).** A test that plants a chain of nested
`retired-actions-v1` directories under a freshly created catalog's retired root
and then runs `recover_or_create`:

```
=== depth 200 ===
PROBE depth=200 outcome=Err(Ambiguous { fact: "catalog bootstrap owner",
                                        detail: "aggregate catalog facts are ambiguous" })
test ...probe_nested_retired_root_chain_is_a_typed_refusal_not_a_stack_overflow ... ok

=== depth 700 ===
thread '...probe_nested_retired_root_chain...' has overflowed its stack
fatal runtime error: stack overflow, aborting
error: test failed, to rerun pass `--lib`
Caused by:
  process didn't exit successfully: `.../gwz_core-... ` (signal: 6, SIGABRT: process abort signal)

=== depth 2000 ===
thread '...' has overflowed its stack
fatal runtime error: stack overflow, aborting
... (signal: 6, SIGABRT: process abort signal)

=== depth 8000 ===   (same)
```

So the widened reading is bounded in *width* and unbounded in *depth*, and the
transition from "typed refusal" to "process abort" happens somewhere between 200
and 700 levels — a number that is a property of the host's thread stack, not of
any check in the code.

**Why this is [P1] and not a note.**

1. It contradicts the authorization's own operative word. §2.4 item 2 authorises
   a **bounded** reading; §3.2 ground 3 spends a paragraph on which constant
   bounds it. An unbounded recursion is not a bounded reading, whatever the
   count check says.
2. It is on the path of **every catalog consumer**, not only of retirement.
   `completed_record` runs in every recovery and in every publication
   acquisition window (E0.2b §2.2's own table). The abort is reachable from
   `recover_or_create` — my probe reaches it exactly there.
3. It converts the subsystem's contractual failure mode into its opposite. This
   owner's whole discipline is "observe defensively, refuse typed": foreign
   children refused, byte reads bounded, entry counts bounded, allocation
   fallible-checked (`reserve_one`). `SIGABRT` is none of those.
4. It is introduced by this train, untested, and the object's own
   infrastructure-row row plants an **empty** nested directory — one level — so
   the existing coverage stops exactly short of the recursion.

**Not asserting a fix**, but the shape is small: the retired root's children need
counting and classifying, not a recursive interior observation, so either a
depth argument threaded through `observe`/`observe_slot` with a hard refusal, or
a dedicated non-recursive retired-root reader, closes it. Either way the fix
belongs in `b9cb795`'s own scope and wants its own row.

---

## 3. Axis 3 — T-B′ shape

**Verdict: clean, and matched to §3.2 field-for-field.**

The variant is exactly the authorised shape:

```
TerminalRetiredRoot {
    catalog_root: &'a Dir,
    catalog_identity: &'a DurableObjectIdentityV1,
    expected: &'a CatalogBootstrapRecordV1,
    absent_child: RootEntryNameV1,
}
```

**Arm (a)** observes `destination_dir` (the retired root) and proves the three
properties §3.2 names: the named child free
(`retired.action_rows.contains(child)` — and `observe` has already refused any
foreign or malformed-recognized child outright, so an occupied name cannot hide
in another class); every resident child an `ActiveAction` row
(`!retired.rows.is_empty()`); and the count within `MAX_RETIRED_ACTION_DIRS`,
compared explicitly with the not-inherited sentence attached. The
`let RootEntryNameV1::ActiveAction(child) = absent_child else { … }` refusal
additionally makes an infrastructure-slot *destination* a typed refusal rather
than an unreachable branch.

**Arm (b)** observes `catalog_root` and re-proves `completed_record` plus the
`retired_record` exactness — the same pair the `AdmissionCatalogInterior` arm
uses, read through T1's widened predicate.

**Both are inside the acquisition window.** `publish_verified_no_replace`
(`publication.rs:184`) opens and holds the rename-source handle at `:193`, checks
source identity, runs the source-interior recheck, then `match
&destination_recheck` at `:274` — and only after all of it calls
`platform::rename_open_source`. The new arm sits in that match. **No destination
recheck runs outside the window anywhere**: `TerminalRetiredRoot` is constructed
at exactly one site (`admission_mutation::retire_action_directory`) and consumed
only by the primitive. `reserve_retired_slot`'s `symlink_metadata` pre-check is a
pre-flight refusal (key #6's own boundary), not a recheck, and the in-window arm
re-proves the same freeness — the same relationship `require_absent` already has
with the admission arm.

**Class attribution** is right: `DestinationRecheckV1` is an enum, so freeze
`:1362-1364`'s struct-vs-enum criterion makes the extension a variant, and the E0
annotation on the arm-table row (freeze `:1512-1524`) already says so in the
freeze's own text.

---

## 4. Axis 4 — T-C′. **[P3] F4**

**Site split, verified by grep, not by claim.** Ten `terminal.*` `hit()` sites:
five in `namespace_mutation.rs` (`:353`, `:380`, `:390`, `:415`, `:429` — keys
#1-#5) and five in `admission_mutation.rs` (`:375`, `:446`, `:461`, `:490`,
`:501` — keys #6-#10). **Zero in `admission/driver.rs`** — the driver decides and
never mutates, and the fixture's doc says so; the diff adds no `hit()` there.
Both files were already declared `FAULT_INJECTION_SOURCES` entries, and the
declared list is byte-unchanged (the diff contains no `+`/`-` line naming a
source file), so §6.1's "no count move at E3" holds.

**Exactly one new capability forward.** `RetainedCompletedCatalogV1::retired_root`
(`completed.rs`). No second forward: keys #1-#5 read the action directory through
`retain_action_namespace`, which already exists.

**Is the `&Dir` a handle escape? — RULING: no.** Three grounds:

1. The precedent the doc cites is real and load-bearing:
   `RetainedActionNamespaceV1::handle(&self) -> &Dir` at
   `namespace_mutation.rs:197`, `pub(super)`, consumed by sibling modules inside
   the same sealed provider owner. A `&Dir` crossing between siblings of
   `provider/` is an established shape, not a new one.
2. The new accessor is **tighter than its precedent**: `retired_root` carries no
   visibility modifier at all, so it is private to `completed.rs`. The handle
   reaches `admission_mutation` only as a call argument. Nothing is callable from
   outside the file, and nothing leaves the provider owner.
3. E0.2b §8's own text authorises "the retired-root **handle**, minted by
   `RetainedCompletedCatalogV1` in the shape `retain_action_namespace` already
   uses" — the handle is what the decision names.

**But [P3] F4 stands on the doc's wording.** The comment quotes
`retain_action_namespace`'s contract — *"never the catalog handle and never a
path"* — and then paraphrases it as *"a typed capability **or a sibling-owner
handle**, never a path"*. The quoted sentence does not contain the disjunct; it
is a consumer-facing contract about a *typed capability* return. The
authorisation for a sibling `&Dir` comes from the `handle()` precedent, not from
that sentence. A future reader chasing the citation will not find the words the
comment attributes to it. Fix is one clause: cite `namespace_mutation.rs:197` as
the shape and `retain_action_namespace` only for "never a path".

---

## 5. Axis 5 — the keys' semantics against §4.3's rows. **[P2] F2, [P2] F3**

### 5.1 Row-by-row

| # | §4.3's announced boundary | What the code binds | Verdict |
| --- | --- | --- | --- |
| 1 `authority_reobserve` | "re-read bounded and **proved to be the authority record this action's retirement is entitled to retire**" | exactly one of the two scheduled homes resident; the resident one opened no-follow and read within `ProtocolRecordKindV1::Authority`'s byte bound. No decode, no binding compare. | Weakest of the four but defensible: the name is derived from this action's digest, so "entitled to retire" is carried by name derivation + residency + bound. |
| 2 `payload_reobserve` | "**have been re-read** and **are the ones the action digest was derived over**" | `symlink_metadata` + `is_file() && !is_symlink()` on the single resident home. **No read at all**, and no digest relation. | **Materially narrower.** |
| 3 `cleanup_reobserve` | "re-read and **every row classifies `Complete`**" (anchor `protocol/cleanup.rs:323-333`, `:394-398`) | worklist read bounded and bound to the reservation; then, per row, live-leaf absent && retired-alias resident. The frozen `classify_cleanup_row` is **never called**. | **Materially narrower.** |
| 4 `reservation_reobserve` | "re-read and equals the admitted reservation's record digest" | decoded and compared: `&decoded != expected \|\| decoded.record_digest() != self.reservation`. | **Exact.** |
| 5 `directory_flush` | "the action directory's own flush has returned" | `sync_directory_edge(&self.handle, …)`. | Exact. |
| 6 `retired_slot_reserve` | destination row free **and** the occupancy credit available via `CatalogOccupancyV1` | `symlink_metadata` NotFound + `CatalogOccupancyV1::new(A-1, R+1, Idle)`. | Exact but for F5's asserted `Idle`. |
| 7 `action_directory_retire` | "P1 no-replace rename of the whole action directory into the retired root, with the **new** `TerminalRetiredRoot` recheck (T-B′)" (§3.3's amended anchor) | exactly that. | Exact. |
| 8 `retired_directory_reobserve` | "re-opened no-follow and its durable identity proved equal to the action directory's" | reopen + `encode_identity` compare, then the retired-root flush. | Exact. |
| 9 `catalog_barrier` | catalog root's dirent barrier, `ExactInterior` class | `private_barrier(final_directory, DirentBarrierClass::ExactInterior, …)`. | Exact. |
| 10 `terminal_revalidate` | "`completed_record` is `Some` **and** the retained-catalog revalidation passes" | both, in that order. | Exact. |

### 5.2 F3 — the two narrowings, and why they are load-bearing

Row #3's narrowing is not incidental: the matrix **could not pass** with the
frozen classifier. `classify_cleanup_row` returns `Complete` only for
`(source Missing, destination Exact(value)) if value == row.expected()` — a
`DurableLeafFingerprintV1` equality. The fixture writes 26 literal bytes into each
retired alias and builds the worklist rows from
`DurableObjectIdentityV1::linux_ext4([7;16], 1, vec![9;8])` on a macOS host, with
a synthetic `[5;32]` digest. Under the frozen classifier every row would be
`Ambiguous`. So the residency-only reading is what makes the ten rows green.

That is not, by itself, wrong — a retirement gate may reasonably be weaker than
`cleanup.*`'s own completion proof, and #2's doc comment supplies a real ground
for its narrowing (a payload's length is never a protocol-record bound). What is
wrong is the **record**. Three things compound:

- §4.3's rows are the text that enters **freeze §3.5** as the `terminal.*`
  activation record — the package's primary deliverable. Installed as written,
  the freeze would carry "have been re-read", "are the ones the action digest was
  derived over", and "every row classifies `Complete`" against boundaries that
  bind none of the three.
- The commit flags **one** deviation (the source-interior arm) and states it
  loudly. These two are not in that list; they are visible only to a reader of
  the doc comments, and #3's comment uses the word "classify complete" while
  using a different rule.
- E0.2b §9 item 5 explicitly leaves "the 38 key-semantic rows other than §1.5's
  eight" standing as drafted, so nothing upstream has already amended them.

**ESCALATE.** Either §4.3 rows #2 and #3 are amended to the boundaries the code
actually binds (my preference — the code's rule is the defensible one), or the
code is strengthened to the announced rule and the fixture rebuilt on real
fingerprints. An interior single-axis review cannot pick between those on the
freeze surface.

### 5.3 F2 — the unassigned Class-1 source arm. RULING

The train's flagged deviation is **correct on the facts**, and I re-verified both
halves against the freeze rather than against the commit message:

- Freeze §4.4's arm table (`:1505-1510`) has four assigned rows plus "any further
  retirement-destination arm | E7's Phase-4 half and the terminal retirement
  edges | Phase 4" — a **destination** row. The 2026-08-27 E0 annotation beneath
  it resolves that row to T-B′ and nothing else.
- The closing paragraph (`:1525-1531`) is the freeze's own argument for why a
  directory publish *forces* an arm: "`PublicationSourceV1::Directory` has no
  'no interior recheck' form, amendment §8.13 structurally rejects raw provider
  renames, and the checker's bare-identifier scan … fail closed on any rename
  outside the sealed primitive. So a managed directory publish has exactly one
  route, and that route demands an arm."

**Is its existence forced?** Yes. `PublicationSourceV1::directory` requires a
`DirectoryInteriorRecheckV1`, whose `expected` is a
`DirectoryInteriorExpectationV1`, and none of the three existing variants can
serve: `CatalogStaging` runs `staging_plan`; `ManagedStaging` compares an
ownership marker; `AdmissionStaging` requires `is_exact`, i.e.
`extra_children == 0`, which is false of every directory that has run an action.
There is no fourth publication route and no "no recheck" form. Forced.

**Is its shape minimal?** Effectively yes, with one caveat on the wording. It
drives the same bounded `interior::observe_action_interior` (capped by
`MAX_ACTION_SLOTS`, no recursion) over the same frozen `ActionSlotV1` grammar and
requires the same `Exact` resident reservation, dropping exactly one clause. The
commit calls it "the narrowest arm that can exist"; strictly, a narrower one
could exist — it could require the extra children to be exactly the completed
row set, which is the very shape key #3 computes a few frames earlier. I would
not hold the train to that; but "the narrowest that can exist" is an overclaim and
"the narrowest that reuses the admission arm's own reader" is the honest phrase.

**Does the arm table need a row? — YES.** The freeze's Class-1 discipline exists
precisely so that an arm is assigned before it is written; the same paragraph
records what happens otherwise ("Had this freeze kept the 'one extension'
wording, Phase 2.3 would have hit a mid-lane freeze-level stop the first time it
published a staged component directory"). Landing a Class-1 arm with no table row
would leave the table describing a surface it no longer covers. The row should
read as its siblings do — *terminal source-interior (the retiring action
directory's interior is a lived-in action directory, not a staged one) | E7's
Phase-4 half | R2-E E3.1* — installed by the same annotation mechanism the E0
annotations already use.

**ESCALATE** — it is a frozen-surface amendment, and the train correctly says the
review owns it. This review's ruling is that it is owed and must be written
before the train lands.

---

## 6. Axis 6 — T-D

**Verdict: clean.**

- `terminal.authority_release` is **siteless**: a grep for
  `CheckedArtifactFaultKeyV1::Terminal` over production sources returns ten hits
  and no `TerminalAuthorityRelease`.
- The **per-key** guarantee survives: `FaultFamilyActivationV1::PartiallyExecuted`
  drives the existing scan
  (`reserved_fault_families_have_no_injection_sites_before_their_package`,
  `fault_expected_keys.rs:679-715`), which asserts a site for every activated
  suffix and **no** site for every non-activated key of the family. No
  prefix-collision hazard among the eleven suffixes.
- Census **165 unmoved**: `EXPECTED_KEY_COUNT` untouched, the `terminal` row's
  count stays `11`, the eleven stable keys are unchanged, no key minted, none
  retired. `TERMINAL_EXECUTED_KEYS.len() == 10` is asserted with its own message.
- The re-grounding is on reading (a) alone and quotes it correctly; reading (b) is
  attributed to `admission.*`'s five idle keys, matching E0.2b §8's cure, and the
  deleted "one boundary must have one name" sentence does not reappear anywhere
  in the object.
- The family lands `PartiallyExecuted("R2-E phase E3 …", TERMINAL_EXECUTED_KEYS)`;
  `only_the_families_with_executed_matrices_are_executed_today` now expects
  `[managed_bootstrap, terminal]`, which is an addition, not a relaxation.

---

## 7. Axis 7 — matrix honesty. **[P3] F6, [P3] F7**

**Shape.** `TERMINAL_MATRIX: [Fault; 10]` in crossing order; both target variants
(`Workspace`, `GitDirectory`) for all three drivers; `REPEATED_CRASH_ROUNDS = 12`;
`reconcile_executed_keys()` reconciles the matrix against the *vocabulary*
(`Fault::all()` filtered to `terminal.` minus the one declared exclusion), so a
key added to the family without a row fails here rather than escaping.

**Is the stop a process stop? — No, and that is the house form.** The driver is
`run_next_terminal_fault(key, || panic!("simulated terminal process stop"))`
caught by `std::panic::catch_unwind`; the "restart" is a fresh *catalog session*
(fresh lease acquisition + `recover_or_create`), not a fresh OS process. I checked
whether this train invented that: it did not. The identical idiom is in
`admission/tests_fault_matrix.rs:272`, `namespace/tests_fault_matrix.rs:404`,
`namespace/tests_managed_matrix.rs:192`, `catalog/bootstrap/tests.rs:376`,
`bootstrap/managed/tests_provider.rs:523`, `tests_leaf_fault_matrix.rs:389`, and
`tests_authority_record_matrix.rs:497`. Corpus-wide convention; **no finding
against this object**, recorded so the verdict is not read as endorsing it as a
real power-loss proof.

**Real durable edges.** The composite's durable edges are the action-directory
flush (#5), the cross-parent rename (#7), the retired-root flush (#8) and the
catalog-root barrier (#9); #1-#4, #6 and #10 are reads by construction, per
§4.3's own table. Every one of the ten is interrupted, restarted, converged, and
then re-proved unmutated by a further fresh session (`again == settled`). The
settled state is asserted positively, not just by equality: the active row gone
from the catalog root, the retired row resident.

**Classifications match the row shapes.** `SINGLE_CROSSING_BOUNDARIES` is exactly
the four post-rename keys (#7-#10) — correct, because the rename is the commit
point and `retire_admitted_action` converges by observation once
`retired_resident`. The complement (#1-#6) is repeatable — correct: five are pure
reads and #5 is a flush whose crash leaves the same rows resident. 6 + 4 = 10,
every row classified. The single-crossing claim is **machine-checked** in the
Step-3.2 probe's shape (re-arm, drive, require the arm still armed via
`take_armed_fault()`), not asserted in prose.

**Git-directory route statement** is present and specific: the family touches no
managed parent, so the Step-2.3 `cfg(test)` door is off its path.

**No weakened or deleted assertions — audited, not sampled.** I dumped every
removed line across `94da3e5..HEAD` (33 lines). They are: two tree digests and one
publication count (replaced), one `_fault_count` line (replaced), five import
lines (re-sorted), the seven-line withdrawn driver note plus its `if`, two
`let Some(EmptyDirectory …)` heads (widened), the old
`empty_directory_identity` signature and its `match`, the old entries probe, the
`13` caller count (→ 14), the `Reserved("R2-D phase 4 …")` row (→
`PartiallyExecuted`), and the one-element `partial` vector (→ two elements).
**No assertion is deleted; none is weakened.**

**F6 [P3] — the corrected [P2-4] duty is not literally met.** E0.2b §8 binds
E3.2 to "drive the **first** attempt through a real admitted action
(`coordinator/execution.rs:137-152` → `ActionAdmissionOwnerV1::resume_or_admit`)"
and to rebuild only the *restarts* through the test-only issuer. `Fixture::admit`
does run a real `resume_or_admit` — but it keeps only
`.directory_identity().clone()` and discards the `AdmittedActionV1`, and
`attempt()` builds a fresh `handoff()` through `admit_observed_action` for **every**
attempt, the first included. `AdmittedActionV1` derives `Clone` and carries no
lifetime (`protocol/admission.rs:283-287`), so it could have been carried out of
the closure. Mitigations: the shape mirrors `namespace/tests_fault_matrix.rs`
exactly (its `admit()` returns the identity and line 289 passes
`handoff(expected, identity)`), the deviation is inherited-and-cited as the
addendum instructs, and `retain_action_namespace` still fails closed if the
reconstructed identity is not the resident one. Recorded so the lane owner can
decide whether the duty meant this or meant more.

**F7 [P3] — the converged path skips both flushes and the barrier.**
`retire_admitted_action` returns `Ok(())` on `retired_resident` without running
key #8's retired-root flush or key #9's barrier. Consequence: a process that
converges on another process's post-rename, pre-flush state reports the
retirement complete on the strength of a rename nobody has flushed. This is the
corpus's standing convergence idiom (`bootstrap/managed/provider.rs:351`'s
`installed_resident` skip is the same shape), so it is **not a regression this
train introduces** — recorded because the terminal family is the first to make the
window span a *cross-parent* rename. The both-parents-resident case is refused
rather than converged over, which is the right call and is stated.

---

## 8. Axis 8 — companions. **[P3] F5, [P3] F8**

**`CATALOG_PUBLICATION_CALL_COUNTS` 2 → 3, with its dated comment.** Present, in
the dict's established form, and the arithmetic is checkable rather than
inherited — I checked it: base six (`mutation.rs` 1 + `directory_mutation.rs` 5),
four dated extensions adding seven → thirteen, this one making **fourteen**, the
**fifth** dated extension. The per-file dict sums to 14
(1+5+3+1+2+2). Matches E0.2b §6.2(a) and §6.3's §4.5 duty verbatim.

**The Rust mirror 13 → 14** (`capability_permit.rs`) moves in the same commit with
the same record, and the comment explicitly notes that E0.2b §6.2 named only the
Python twin — an honest addition to the inventory.

**Tree digests — verified against the actual tree bytes, not read.** The checker
recomputes `PROTECTED_SOURCE_TREE_DIGESTS` (`check_checked_artifact_boundaries.py:930`)
and passes, so both moved pins are correct:
`checked_artifact/capability/pre_catalog.rs` → `3d0445b4…` and
`checked_artifact/catalog.rs` → `5f10b937…`. `checked_artifact/platform.rs` is
unmoved and correctly so — nothing in the diff touches that tree.

**F8 [P3].** The `catalog.rs` tree is a **third** pin, beyond the two E0.2b
§6.2(b) inventoried; it moves because the terminal retirement is forwarded through
`catalog/bootstrap.rs`. The train records this in the dict with a dated comment
saying so. That is the right handling — the note is that §6.2(b)'s inventory (and
therefore E1's and E2's duty lists, which quote it) now understates the pin set,
and the lane owner should fold the third tree back into the addendum so E1/E2 do
not rediscover it as a surprise.

**`FAULT_INJECTION_SOURCES`** — doc-comment extension present, list byte-unchanged,
count stays nine. §6.1's "the single move is E2's" holds.

**Per-OS markers — which values are executed, which derived.**
`_fault_count("414 passed", "424 passed")`. The docstring is explicit and correct:
**darwin 400 → 414 is EXECUTED** on this lane's macOS host; **linux 410 → 424 is
DERIVED** from the same +14 delta and is marked **FIRST-DISPATCH-EXPECTED**, with
the instruction that the lane owner re-measures on the Linux leg at landing,
citing `1bcb925`'s precedent. The remainder partition is stated not to move, and
that is true — all fourteen added tests are under `checked_artifact::`. The delta
reconciles: `tests_retired_root.rs` has **6** `#[test]`s and
`tests_terminal_fault_matrix.rs` has **8**; 6 + 8 = 14. **I re-measured the darwin
value independently and got exactly 414** (§9).

**F5 [P3] — an asserted occupancy term.** `reserve_retired_slot` builds
`CatalogOccupancyV1::new(active-1, retired+1, CatalogAdmissionOccupancyV1::Idle)`.
`Idle` is never observed on this path — `retire_admitted_action` reads no
admission record. `validate` computes
`outstanding = active_action_dirs + (admission == PreparingWithoutFinal)`
(`bounds.rs:132-133`), so asserting `Idle` drops one from `outstanding` whenever a
durable `Preparing` admission is resident — a state that survives a crash and is
therefore reachable. At full occupancy the retirement can consume the credit slot
the frozen rule reserves for that pending admission, whose later resume then
refuses. The failure direction is a spurious later refusal, not corruption, and
the edge has no production caller until E4 — hence [P3]. The doc comment says
"the occupancy it must satisfy is the post-edge one" and does not disclose the
assumption; it should, or the term should be observed. (The driver's use of the
same constant is **not** affected: there it sits inside the observed
`AdmissionDriveV1::Idle` arm.)

---

## 9. Axis 9 — gates. Verbatim tails

All run by me, in the object worktree, at `04c8a66`.

**`cargo check --all-targets`**
```
    Checking gwz-core v0.11.0 (/private/tmp/.../scratchpad/e3-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 10.63s
```

**`cargo fmt --all -- --check`** — no output, exit 0.

**`CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings`**
```
    Checking gwz-core v0.11.0 (/private/tmp/.../scratchpad/e3-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 26.44s

[exited with code 0]
```

**The terminal matrix — `cargo test --lib -p gwz-core tests_terminal_fault_matrix -- --nocapture`**
```
terminal.terminal_revalidate | git-directory | interrupted=yes | restart=settled | catalog=7 retired=1 action=5 | resume=no-mutation
test ...terminal_interruption_restart_convergence_matrix_on_a_git_directory_target ... ok

test result: ok. 8 passed; 0 failed; 0 ignored; 0 measured; 1596 filtered out; finished in 8.32s
```
(Rows printed for all ten keys on both variants; the twelve-round repeated rows
print `slots-stable=yes … converged=yes`, and the single-crossing rows print
`single-crossing=yes | restart=settled | re-crossed=no`.)

**`cargo test --lib -p gwz-core interface_tests::fault_expected_keys`**
```
test ...reserved_fault_families_have_no_injection_sites_before_their_package ... ok

test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1598 filtered out; finished in 0.03s
```

**`cargo test --lib -p gwz-core interface_tests::capability_permit`**
```
test ...catalog_publication_uses_one_source_associated_seam ... ok

test result: ok. 11 passed; 0 failed; 0 ignored; 0 measured; 1593 filtered out; finished in 0.00s
```

**`cargo test --lib -p gwz-core tests_retired_root`**
```
test ...a_retired_root_holding_an_infrastructure_slot_name_still_refuses ... ok
test ...a_retired_root_holding_an_action_row_still_recovers_and_revalidates ... ok
test ...an_admission_publishes_into_a_retired_root_populated_catalog ... ok

test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1598 filtered out; finished in 0.30s
```

**The R4b-G fault partition, re-measured — `cargo test --lib -p gwz-core checked_artifact::`**
```
test result: ok. 414 passed; 0 failed; 0 ignored; 0 measured; 1190 filtered out; finished in 118.22s
```
Exactly the darwin pin. The EXECUTED half of the per-OS marker is confirmed
independently.

**`python3.13 scripts/checks/check_checked_artifact_boundaries.py`**
```
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
EXIT=0
```

**`python3.13 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py`**
— the landing gate the object had **not** completed pre-review. Run to completion:
```
.....................................................................
----------------------------------------------------------------------
Ran 69 tests in 819.578s

OK

[exited with code 0]
```

**Reviewer probes (executed in the clone, never in the object).** Tails quoted in
§2.3: depth 200 → typed refusal, ok; depth 700 / 2000 / 8000 → `fatal runtime
error: stack overflow, aborting` … `(signal: 6, SIGABRT: process abort signal)`.

---

## 10. What the lane owner is being asked to rule

1. **F1 [P1]** — the depth bound. Blocking in my judgement: the authorisation
   E3.1 spent a precondition step on says "bounded", and on this tree the widened
   reading aborts the process. It wants a fix and a row in `b9cb795`'s own scope.
2. **F2 [P2]** — write the freeze §4.4 arm-table row for the terminal
   source-interior arm (the arm itself is forced and its shape is acceptable), and
   soften "the narrowest arm that can exist" to what it is.
3. **F3 [P2]** — reconcile §4.3 rows #2 and #3 with what the code binds, before
   the activation record enters freeze §3.5. Amending the rows is the honest
   route; strengthening the code is the other.
4. **F4-F8 [P3]** — the contract paraphrase, the asserted `Idle`, the
   first-attempt issuer, the converged-path flush skip, and folding the third
   protected tree into E0.2b §6.2(b)'s inventory.

Everything else in the train — the three-gate package and `staging_plan`'s named
preservation, T-B′'s fields and both in-window arms, the explicit
`MAX_RETIRED_ACTION_DIRS` comparisons and their not-inherited sentences, T-C′'s
capability split with a driver holding zero sites and exactly one new forward,
T-D's siteless key with the census at 165, the ten-row matrix on both variants
with its repeated-boundary and machine-checked single-crossing classifications,
and every companion inventory — I verified and found sound.

---

# Round 2 — remediation re-verification, 2026-08-27

**VERDICT: GO for landing**, subject to three landing conditions the lane owner
discharges in the landing commit (§R2.9). Round 1's [P1] and both [P2]s are
**cured and re-verified by execution**; all five [P3]s are dispositioned; one new
[P3] nit (F9) is introduced by the remediation. Nothing is escalated to the E7
dual as a defect; §R2.9 names the one item that becomes an E7-dual question *if*
the lane owner declines this review's F2 ruling.

**Object.** Two remediation commits appended to `e3-terminal`:

- `a31e118` — remediation [P1 F1]: bound the retired-root reading by
  construction, not by recursion.
- `7f23484` — remediation [P2 F3, P2 F2, P3 F4-F7]: bind what the rows announce.

Round 1's section above is left as written. Both remediation commits were diffed
in full; my probes were run in a fresh byte-clone (`scratchpad/probe-tree2`),
never in the object, and are written independently of the object's own helpers.

---

## R2.1 F1 [P1] — **CURED**

### The code

`interior::read_retired_root` is a dedicated single-level reader: one
`directory.entries()` pass, each name charged to the budget, capped, then
classified flat through `CatalogRootRowClassV1::classify`. Every property the
round-2 duty names checks out.

**It calls neither `observe` nor `observe_slot` — grep-exhaustive.** The whole
of `interior.rs` now contains exactly three relevant lines:

```
64:pub(super) fn observe(              <- definition
109:        let fact = observe_slot(    <- the only call to observe_slot, inside observe
453:fn observe_slot(                    <- definition
495:            let retired = read_retired_root(&child)?;   <- the only call to read_retired_root
575:fn read_retired_root(               <- definition
```

There is **no call to `observe(` anywhere except its own definition**. The
mutual recursion is gone as a structural fact, not as a guarded one. And
`read_retired_root` is file-private with a single call site, so no sibling can
reintroduce a second entry into it: a tree-wide grep for `read_retired_root`
outside `interior.rs` returns only two doc-comment mentions.

**No other recursion path survives.** `observe_slot`'s directory arm now has
exactly two outcomes for the retired root — `EmptyDirectory` or
`RetiredActionRoot` — and neither re-enters an interior observation.
`observe_action_interior` and `observe_managed_component_interior` are flat
loops; neither calls `observe`.

**The entry cap is on the ENTRY count, before classification cost accrues.** The
loop body is `budget.charge_os_str(&name)?` → `if budget.entry_count() >
MAX_RETIRED_ACTION_DIRS { return Err(unsupported(…)) }` → `match classify(…)`.
`CatalogNameBudgetV1::charge` increments `entry_count` exactly once per charged
name (`catalog/enumeration.rs:243`), so `entry_count()` is a count of *entries*,
not of accepted rows, and the refusal precedes the classifier. The cap is
`MAX_RETIRED_ACTION_DIRS` itself — not `MAX_INTERIOR_ENTRIES`, not
`MAX_ACTIVE_ACTION_DIRS`, and not the name budget's `MAX_CATALOG_PARENT_ENTRIES_V1`
(4 096, which the retired cap now pre-empts at 65). The not-inherited paragraph
moved with the reader and still names the three real constants. E0.2b §2.4 item 2
and §3.2 ground 3 are discharged more directly than before.

**`observe`'s duplicate-row refusal is preserved.** `read_retired_root` sorts the
collected digests and refuses `"multiple native entries resolve to one retired
action row"` on any adjacent pair — the same shape as `observe`'s `action_rows`
check. (`observe`'s *infrastructure*-slot duplicate check has no counterpart, and
needs none: any infrastructure name in the retired root is an unaccepted row and
the predicate refuses the whole root on `unaccepted_rows != 0`.)

**`infrastructure_rows` → `unaccepted_rows` is an honest rename, and a widening
of what is counted**, not a narrowing: infrastructure-slot names, scheduled and
retired slot names, malformed-recognized names, non-ASCII names and foreign names
all land in it, and all three consumers (`retired_root_identity`,
`retired_action_dirs`, and the T-B′ arm through them) still require it to be zero.

### The behaviour change I checked for, and cleared

A foreign or malformed child of the retired root used to make `observe` return
`Err` via `exact_row`; it now increments `unaccepted_rows`, and the refusal comes
from the predicate instead. I traced whether that could turn a hard refusal into
a soft classification anywhere:

- `retain_completed_catalog` runs `completed_record` **before** `retain_directory`
  (gate 2), so gate 2's `RetiredActionRoot { identity, .. }` match — which does
  not itself check `unaccepted_rows` — is only ever reached behind the check.
- `revalidate` runs `require_named_directory_identity` (gate 3) at `:355-359` and
  then `completed_record` at `:361-366` **in the same frame**, so a retired root
  with unaccepted rows passes gate 3 and is refused four lines later. `revalidate`
  as a whole refuses.
- `staging_plan` refuses `RetiredActionRoot` by name, unchanged and still pinned.

Both round-1 refusal rows (`…_holding_an_infrastructure_slot_name_still_refuses`,
`…_holding_a_foreign_child_still_refuses`) are green, so the end-to-end refusal is
preserved in fact and not only in argument.

**One reduction in analysis depth, examined and cleared (no finding).**
`read_retired_root` classifies by exact bytes and no longer consults
`platform.parent_mode()`, so `exact_row`'s platform-equivalent-alias refusal
("infrastructure alias is noncanonical") does not apply inside the retired root.
It cannot produce a false accept: `classify` admits only exact
`action-<lowercase-hex>-v1`, every other spelling becomes an unaccepted row, and
duplicate digests are refused. Worth one guard sentence for whoever ever widens
the acceptance clause — the collision analysis would have to come back with it.

### The probes (executed, independent of the object's helpers)

Both planted and torn down by my own `chdir` walk, so nothing here shares code
with `plant_nested_retired_chain` / `remove_nested_retired_chain`.

```
running 2 tests
test ...probe_r2_a_full_retired_root_at_the_bound_still_recovers ... PROBE-B entries=64 outcome=Ok(())
ok
test ...probe_r2_nested_chain_refuses_typed_and_never_aborts ... PROBE-A depth=3000 outcome=Err(Ambiguous { fact: "catalog bootstrap owner", detail: "aggregate catalog facts are ambiguous" })
ok

test result: ok. 2 passed; 0 failed; 0 ignored; 0 measured; 1607 filtered out; finished in 22.81s
```

- **PROBE A** is round 1's reproduction, re-run at **depth 3000** — more than four
  times the 700 that aborted the pre-remediation tree. Typed refusal, no abort.
  The [P1] is cured.
- **PROBE B** is the cap's *other* side, which the object's own rows do not cover:
  they prove 65 entries refuse, but not that **64 still recovers**. If the entry
  cap were off by one in the refusing direction, a legitimately full retired root
  would be unobservable and OPEN-T1 would be back at the boundary. It recovers:
  `Ok(())`. The cap sits in exactly the right place, and it composes with the
  T-B′ arm's `>= MAX_RETIRED_ACTION_DIRS` (which refuses adding a 65th).

The object's own new rows are the right ones and are green: a nested chain at
depth 1024 on **both** target variants, and a past-the-bound refusal at 65.

---

## R2.2 F3 [P2] — **CURED**, both halves

### Row #3 — the code strengthened to the announced rule

`require_completed_cleanup_worklist` now supplies two `CleanupPhysicalFactV1`
values per row and requires
`worklist.classify(index, &source, &destination) == Some(CleanupResolutionV1::Complete)`.
The rule is `protocol/cleanup.rs`'s own `classify_cleanup_row`, unchanged and
uncopied, so the announced semantic and the code are now literally the same
sentence — which is what the round-1 finding asked for. The residency
restatement is gone.

**The facts are observed through the production identity provider.**
`cleanup_physical_fact` opens the alias no-follow, takes
`super::HostPlatform.file_identity(&file)` — the same provider the rest of the
owner uses — and builds `DurableLeafFingerprintV1::new(durable_identity, length,
streamed_digest)`. `Complete` requires `value == row.expected()`, i.e. identity
**and** length **and** digest, so a green matrix is now real evidence rather than
a residency check dressed as one.

**The fixture no longer invents anything.** `place_completed_action_rows` writes
each alias and then calls `observed_alias_fingerprint`, which opens the file
through `cap_std` and takes its durable identity from the same `HostPlatform`.
The fabricated `DurableObjectIdentityV1::linux_ext4([7;16], 1, vec![9;8])` and
`[5;32]` digest that made the round-1 matrix green are deleted. (Small prose
inaccuracy, not a finding: the commit says the alias is "observed for its durable
identity, its length and its content digest" — the identity is observed; the
length and digest are computed from the bytes the fixture just wrote. They are
provably equal to a read-back, and the classifier's `Complete` arm returning true
is the proof that they match what production streams.)

### The length bounding is sound, and its overflow arm is right

`let limit = expected.length();` — the length recorded on the **bound** worklist
row, i.e. a field of a protocol record already read under
`ProtocolRecordKindV1::CleanupWorklist`'s own byte bound and already bound to the
resident reservation by `read_and_bind_cleanup_worklist`. Never a length the
object under inspection supplies. That keeps ConsumerCheckpoint §8's
"a payload's length is never a protocol-record bound" rule intact: the payload is
never read *into* a record-sized buffer at all.

The stream is a fixed `[u8; 8192]` buffer, so memory is constant regardless of
object size. Three exit arms, all checked:

- `length > limit` mid-stream → `Other` (refusal), with at most one chunk of
  overshoot;
- EOF with `length != limit` (a short object) → `Other`;
- EOF with `length == limit` → `Exact(fingerprint)`.

`Other` classifies `Ambiguous`, which is a refusal — so both the overrun and the
underrun fail closed. Accepted cost, recorded not charged: a worklist row whose
recorded length is enormous makes the digest pass O(object size) in time (still
O(1) in memory) before returning `Other`; the worklist is bound to the
reservation, and the alias is a leaf the action itself wrote, so this is the same
cost profile as any content digest in this owner.

### Row #2 — the determination is precise and honest

The amended semantic is stated at the site, dated, attributed to the review, and
matches the code **exactly**: "the source and goal payload rows are resident in
exactly one of their two scheduled homes, each a canonical regular file, read not
at all." That is `require_single_scheduled_home(..., bound: None, ...)`
line-for-line — one-of-two residency, `is_file() && !is_symlink()`, no read.

Its two grounds are cited, and I checked both rather than accepting them:

- *"a payload's length is never a protocol-record bound"* — ConsumerCheckpoint §8
  :236-237, the same ground the round-1 comment gave, and the reason no frozen
  record budget can size a payload read.
- *"the digest relation the row names is the authority record's"* —
  `require_leaf_digest` (`coordinator/execution.rs:296-310`) takes a
  `CheckedLeafFactV1` and its own error text says "the authority observation binds
  a different expected content", so the relation is proved against a
  `CheckedAuthorityObservationV1` (real, `authority_record_binding.rs:588`) that a
  terminal retirement does not hold. `record.binding_validate` is a real key
  (`fault_v1.rs:100`). The citations hold.

The determination is **flagged for the lane owner to carry into the freeze §3.5
activation record in place of row #2's drafted text**. That flag is the right
disposition and becomes landing condition R2.9(b) — the code and the record now
agree only if the record is actually amended.

---

## R2.3 F2 [P2] — disposition accepted

No code change, per this review's ruling. The arm's doc now records, two-way,
that freeze §4.4's arm table is owed its row at the E3 landing and quotes the row
text this review drafted. The overclaim is corrected precisely: "the narrowest arm
that can exist" becomes "the narrowest arm that reuses the admission arm's own
reader", and the narrower arm that *could* exist is named (requiring the extra
children to be exactly the completed row set). Honest.

Residual: **landing condition R2.9(a)** — the table row itself still has to be
written into the freeze.

---

## R2.4 F4 [P3] — cured

The forward's doc now splits the two halves correctly: the *shape* (a `&Dir`
between siblings of `provider/`) is credited to
`RetainedActionNamespaceV1::handle`'s precedent, with the note that this accessor
is tighter (no visibility modifier, private to its file, the handle crossing only
as a call argument); the *never a path* half quotes
`retain_action_namespace`'s sentence and explicitly says that sentence "is about a
typed capability return and does not itself sanction a sibling `&Dir`". That is
exactly the correction asked for, and it goes one better by naming E0.2b §8's own
"the retired-root **handle**" as the authorising text.

---

## R2.5 F5 [P3] — cured, and the new code is sound

`reserve_retired_slot` now takes an `admission` parameter, supplied by
`observed_admission_occupancy(&observed)` — read from the catalog root's own
`ActionAdmissionActive` row, from the observation `retire_admitted_action`
already takes.

I checked the new function against `admission_record_row`'s real return shape
(`Missing` when the row is absent; `Other` for a non-regular-file, an unbounded
byte fact, **or** a decode failure):

- `Ok(Missing)` → `Idle`
- `Ok(Exact(record))` where `record == ActionDirectoryAdmissionV1::idle()` → `Idle`
- everything else, including `Other` and any `Err` → `PreparingWithoutFinal`

The catch-all is the **conservative** arm, and the doc says why correctly:
`PreparingWithoutFinal` *adds* one to `validate`'s
`outstanding = active + (admission == PreparingWithoutFinal)`, so it can only make
the retirement refuse, never let it consume a credit. Choosing
`PreparingWithFinal` would charge one *less* and would additionally engage
`validate`'s `PreparingFinalMissing` rule, so it would need a second observation —
the doc states that too, and it is right. `PreparingFinalMissing` is unreachable
from this call site because that variant is never passed. Net effect: the
retirement now refuses exactly one case more than before (`R + A == 64` with a
pending admission), which is the case the frozen credit rule exists to reserve.

**F9 [P3] NEW — the `#[allow(clippy::too_many_arguments)]` reason is now stale.**
`retire_action_directory` took seven arguments and now takes eight, but the
`reason` string still enumerates the old set ("two retained parents, two
identities, the frozen bootstrap record, the reservation, and both observed
occupancy terms") and does not mention the observed admission term — and its "two
identities" was already one more than the function's single identity parameter.
The lint suppression still functions; this is a documentation-accuracy nit for the
landing commit, graded [P3] because the coordinator asked for regressions the
remediation introduced and this is one.

---

## R2.6 F6 [P3] — cured, literally

`Fixture::admit` now returns the `AdmittedActionV1` that
`ActionAdmissionOwnerV1::resume_or_admit` issued, and `attempt` takes it directly.
`admit_observed_action` survives in the file **only inside a doc comment** — a
grep finds one hit, at line 211, in prose. The test-only issuer is gone from the
matrix's code path entirely, and with it the inherited
`namespace/tests_fault_matrix.rs:25-34` deviation, so E0.2b §8's corrected [P2-4]
duty is met on its own terms rather than by inheritance. The restart still
reconstructs the *capability* (`retain_action_namespace` re-proves the resident
directory's identity against this handoff) but never the admission decision,
which is the right split and is stated at the site.

All ten matrix rows, the twelve-round repeated rows and the single-crossing probe
are green against the real token on both target variants (§R2.8), so this is a
strengthening of the evidence, not just of the prose.

---

## R2.7 F7 [P3] / F8 [P3] — dispositions

**F7.** The converged-restart early return now cites both corpus precedents
(`bootstrap/managed/provider.rs`'s `installed_resident` skip and
`namespace/tests_fault_matrix.rs`'s resident-row return) and states the
consequence the terminal family is first to carry, including the reachable-state
enumeration ("rename discarded → re-enter the edge", or "rename durable with
either or both parents unflushed → this branch"). That is the disclosure round 1
asked for. The underlying property is corpus-wide and outside this train's scope;
if the lane ever wants the idiom itself re-examined, the E7 dual is the venue —
**optional, not a condition of this landing.**

**F8.** Unchanged and still open: the third protected tree
(`checked_artifact/catalog.rs`) is correctly pinned and dated in the dict, but
E0.2b §6.2(b)'s two-tree inventory — which E1's and E2's duty lists quote — still
understates it. **Landing condition R2.9(c).**

---

## R2.8 Gates — verbatim tails (round 2, at `7f23484`)

**`cargo check --all-targets`**
```
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 0.11s
```

**`cargo fmt --all -- --check`** — no output, exit 0.

**`CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings`**
```
    Checking gwz-core v0.11.0 (/private/tmp/.../scratchpad/e3-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 6.87s
```

**Terminal matrix — `cargo test --lib -p gwz-core tests_terminal_fault_matrix`**
```
test ...terminal_interruption_restart_convergence_matrix_on_a_workspace_target ... ok
test ...terminal_interruption_restart_convergence_matrix_on_a_git_directory_target ... ok

test result: ok. 8 passed; 0 failed; 0 ignored; 0 measured; 1599 filtered out; finished in 6.26s
```

**`cargo test --lib -p gwz-core tests_retired_root`**
```
running 9 tests
test ...the_bootstrap_staging_plan_still_refuses_a_populated_retired_root ... ok
test ...a_retired_root_holding_a_malformed_action_name_still_refuses ... ok
test ...a_retired_root_holding_a_foreign_child_still_refuses ... ok
test ...a_retired_root_holding_an_action_row_still_recovers_and_revalidates ... ok
test ...a_retired_root_holding_an_infrastructure_slot_name_still_refuses ... ok
test ...a_retired_root_past_the_frozen_retired_bound_refuses ... ok
test ...an_admission_publishes_into_a_retired_root_populated_catalog ... ok
test ...a_nested_retired_root_chain_is_a_typed_refusal_on_a_git_directory_target ... ok
test ...a_nested_retired_root_chain_is_a_typed_refusal_on_a_workspace_target ... ok

test result: ok. 9 passed; 0 failed; 0 ignored; 0 measured; 1598 filtered out; finished in 2.57s
```

**`cargo test --lib -p gwz-core interface_tests::fault_expected_keys`**
```
test ...reserved_fault_families_have_no_injection_sites_before_their_package ... ok

test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1601 filtered out; finished in 0.03s
```

**`cargo test --lib -p gwz-core interface_tests::capability_permit`**
```
test ...catalog_preflight_surface_has_no_path_plus_lease_or_callback_seam ... ok

test result: ok. 11 passed; 0 failed; 0 ignored; 0 measured; 1596 filtered out; finished in 0.00s
```

**The R4b-G fault partition, re-measured — `cargo test --lib -p gwz-core checked_artifact::`**
```
test result: ok. 417 passed; 0 failed; 0 ignored; 0 measured; 1190 filtered out; finished in 64.23s
```
Exactly the builder's re-pinned darwin value. The +17 delta reconciles:
`tests_retired_root.rs` now holds **9** `#[test]`s (6 + the two nested-chain
variants + the past-the-bound row) and `tests_terminal_fault_matrix.rs` holds
**8**; 9 + 8 = 17. The linux value `427` remains **DERIVED** and is still marked
FIRST-DISPATCH-EXPECTED for re-measurement on the Linux leg at landing — the
correct treatment, unchanged.

**`python3.13 scripts/checks/check_checked_artifact_boundaries.py`**
```
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```
Both re-pinned tree digests (`…/pre_catalog.rs` = `588b105f…`,
`…/catalog.rs` unchanged from E3's own re-pin) are therefore verified against the
actual tree bytes, and no other protected tree moved.

**`python3.13 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py`**
```
.....................................................................
----------------------------------------------------------------------
Ran 69 tests in 533.193s

OK
```

**Reviewer probes** — §R2.1, run in `scratchpad/probe-tree2`, tails quoted there.

---

## R2.9 Verdict and residuals

**GO for landing.** Round 1's [P1] F1 and [P2] F3 are cured and re-verified by
execution; [P2] F2 was ruled in the train's favour and its residual is a
mechanical freeze edit; F4, F5, F6 are cured; F7 is dispositioned; F8 and the new
F9 are documentation items.

**Landing conditions — the lane owner discharges these in the landing commit:**

- **(a) [from F2]** Write the freeze §4.4 arm-table row for the terminal
  source-interior arm, by the same dated-annotation mechanism the 2026-08-27 E0
  annotations use: *terminal source-interior (the retiring action directory's
  interior is a lived-in action directory, not a staged one) | E7's Phase-4 half |
  R2-E E3.1*. The arm's doc already promises this row; landing the code without it
  leaves the table describing a surface it no longer covers. **If the lane owner
  declines this review's ruling that the row is owed, that disagreement — and only
  that — is the item for the E7 dual.**
- **(b) [from F3]** Carry row #2's dated determination into the freeze §3.5
  `terminal.*` activation record, replacing E0.2 §4.3 row #2's drafted text. The
  code and the record agree only once this is done; the train flags it and cannot
  do it itself.
- **(c) [from F8]** Fold the third protected tree
  (`checked_artifact/catalog.rs`) into E0.2b §6.2(b)'s inventory, so E1 and E2
  expect three re-pins rather than rediscovering the third.

**Landing-time nits, no gate:** F9's stale `#[allow(clippy::too_many_arguments)]`
reason string on `retire_action_directory`, and the commit-prose claim that the
fixture "observes" the alias length and digest when it observes only the identity.

**Optional E7-dual item, not a condition:** F7's corpus-wide convergence idiom —
a converging process reporting a durable edge complete on the strength of another
process's unflushed rename. Correctly disclosed here; out of this train's scope.

**Re-measure at landing:** the linux fault-partition value `427`, as the driver's
own docstring instructs.
