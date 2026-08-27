# R2-E E0.3 dual #1 — STATE axis review of the E0.2 semantics amendment

Date: 2026-08-27. Axis: **State** (census truth, frozen-text consistency,
determination compatibility, the accounting). Peer-blind: no Code-axis material
was read, requested, or inferred from. Adversarial, read-only.

**Object:** `dev-docs/GwzM5-8R2E-SemanticsAmendment-DRAFT.md` (1,396 lines,
read in full).

**Verdict:** **NO-GO — round-2 remediable. The semantic core stands; the
accounting does not.**

Two [P1] and six [P2] findings. Per plan §2 ("automatic escalation on
P0/P1/P2") and `AgentProcessRules.md:1825` ("P0/P1/P2 blocks"), a P1 blocks.
Every one of the eight is a document edit; none requires re-deriving a decision,
and none touches the 38 keys' semantics, the O6 resolution, the §5 decision or
the O8 mechanism, all of which I sustain. One remediation round should clear it.

---

## 1. What I verified, and how

Everything below was counted or opened by me, not inherited from the draft.

- The fault vocabulary and the fixture's expected list were **machine-counted**
  from `gwz-core/src/checked_artifact/fault_v1.rs` and
  `interface_tests/fault_expected_keys.rs`, and the draft's 38 named keys were
  **set-differenced** against the vocabulary in both directions.
- All three re-reservation quotes and all twelve §8 annotation targets were
  opened in `GwzM5-8R2DInterfaceFreeze.md` and compared line by line.
- The +19 citation drift was re-derived from
  `GwzM5-8I2CompatibilityContract.md` and from git.
- The O8 arithmetic was re-derived from `GwzM5-8R4bG-Evidence.md` §12.7-§12.9.
- The §11.3 accounting was re-derived from `GwzM5-8R2DSettledTuple.md` §11.1-§11.3.
- Code spot checks: the census surfaces, the `WorkspaceMutatorLock` call-site
  census, `interior::completed_record`'s two `?`-guards, `authority.rs`'s three
  name renderers, `coordinator/execution.rs`'s authority gates.

---

## 2. THE CENSUS INVARIANT — VERIFIED CLEAN. Counts shown.

### 2.1 The vocabulary, counted by me

`gwz-core/src/checked_artifact/fault_v1.rs`, every `Variant => "family.key"` row:

| family | count | lines |
| --- | --- | --- |
| `runtime.*` | 18 | `:35-52` |
| `catalog_bootstrap.*` | 25 | `:53-77` |
| `admission.*` | 19 | `:78-96` |
| `record.*` | 13 | `:97-109` |
| `namespace.*` | 11 | `:110-120` |
| `durable_leaf.*` | 11 | `:121-131` |
| **`barrier.*`** | **16** | **`:132-147`** |
| `managed_bootstrap.*` | 30 | `:148-177` |
| **`cleanup.*`** | **11** | **`:178-188`** |
| **`terminal.*`** | **11** | **`:189-199`** |
| **TOTAL** | **165** | |

18+25+19+13+11+11+16+30+11+11 = **165**.
`interface_tests/fault_expected_keys.rs` `EXPECTED_STABLE_KEYS` holds **165**
rows at `:8-172`; `EXPECTED_KEY_COUNT: usize = 165` at **`:174`**. The draft's
`fault_v1.rs` family cites (`:132-147`, `:178-188`, `:189-199`) and its
`:174`/`:172` cites are **exact**.

### 2.2 The three activation records' rows, counted by me

Parsed every `| N | \`family.key\` |` row of §2.3, §3.4 and §4.3:

| record | rows | ordinals | vs vocabulary |
| --- | --- | --- | --- |
| §2 `cleanup.*` | **11** | 1-11, contiguous, no dup | exact set match, **and exact vocabulary order** |
| §3 `barrier.*` | **16** | 1-16, contiguous, no dup | exact set match, **and exact vocabulary order** |
| §4 `terminal.*` | **11** | 1-11, contiguous, no dup | exact set match, **and exact vocabulary order** |
| **TOTAL** | **38** | | |

- **Named-but-not-in-vocabulary (i.e. MINTED): ∅.** Zero keys minted.
- **In-vocabulary-but-not-named: ∅.** No re-reserved key is unaccounted.
- 11+16+11 = **38** = the plan §0 corrected figure. The 67 error does not recur.
- 165 − 38 = **127** keys in the seven other families, untouched by this
  amendment. No row of any other family is edited or re-keyed.
- **Retired: 0.**

### 2.3 The reserved-with-determination terminal key

`terminal.authority_release` (`fault_v1.rs:199`) is present as row #11 of §4.3
with `Function` = `**none**` and is dispositioned in §4.3's
"Reserved with reason" block. §4.5's `TERMINAL_MATRIX: [Fault; 10]` and the
§4 record header's "10 of 11 executed, 1 reserved with a determination" are
mutually consistent, and 10 + 1 = 11 closes the family. ✓

### 2.4 Still formally Reserved at E0 close

`fault_expected_keys.rs`:

- `:381-382` — `("cleanup", Reserved("R2-D phase 4 (legacy leaf edge conversion)"), 11)`
- `:386-387` — `("barrier", Reserved("R2-D phase 4 (Windows retirement closure)"), 16)`
- `:391-392` — `("terminal", Reserved("R2-D phase 4 (terminal retirement edges)"), 11)`

All three still `Reserved`. §9 item 2 declares they stay `Reserved` until E1.2 /
E2.3 / E3.2 flip them in the converting commits, and §9 item 3 declares no
production code lands. The draft's row cites (`:380-384`, `:385-389`,
`:390-394`) are **exact**. ✓

### 2.5 RULING ON THE CENSUS

> **The census invariant HOLDS, machine-verified, on every axis the mandate
> names: 165 total, ZERO minted, ZERO retired, all 38 re-reserved keys present
> and accounted (11+16+11), the one reserved-with-determination terminal key
> correctly carried, and every family row still formally `Reserved` at E0
> close. This is the amendment's strongest section and I sustain it without
> reservation.**

The census *statements* around it are not equally clean — see [P1-1] and
[P2-6], both of which are prose/arithmetic defects sitting beside a correct
underlying census, not census errors.

---

## 3. Findings

### [P1-1] The injection-source file count is wrong in two places, and the two are inconsistent with each other — and one is an instructed commit edit

The declared source list today is **nine** files (`FAULT_INJECTION_SOURCES`,
`fault_expected_keys.rs:419`+): `mutation.rs`, `directory_mutation.rs`,
`aggregate.rs`, `admission_mutation.rs`, `leaf_observation.rs`,
`namespace_mutation.rs`, `managed_mutation.rs`, `protocol/authority_record.rs`,
`authority_record_binding.rs`. This matches freeze `:528`'s "**nine**".

Now apply the amendment's own three routing decisions:

| decision | site file | new declared file? |
| --- | --- | --- |
| **C-2** (`:246-262`) | `namespace_mutation.rs` — *already declared*, and §2.3 (`:303`) says so: "already a declared entry at `fault_expected_keys.rs:449-452`" | **no** |
| **B-1** (`:507-519`) | `barrier_mutation.rs` — `NEW@E2.1` | **yes, one** |
| **T-C** (`:760-772`) | `admission_mutation.rs` — *already declared*, §4.3 (`:777-778`) says so: "already a declared `FAULT_INJECTION_SOURCES` entry, `fault_expected_keys.rs:432-435`" | **no** |

**Exactly one new file. 9 → 9 → 10 → 10.** The count moves once, at E2.

The draft says otherwise, twice, and differently:

1. **§2.5 (`:386-387`)** makes it an E1.2 commit duty: "the §3.5 inventory
   addendum's file count (freeze `:525-532`, "nine" → ten), all in the same
   commit — RemPlan §10's duty." **E1.2 adds no file.** This instructs E1.2 to
   land a false inventory edit under RemPlan §10's duty.
2. **§8 row 11 (`:1276`)** — proposed frozen annotation text — says the list
   "moves from **nine** files to **eleven** as E1/E2/E3 land", then enumerates
   in the same sentence one new file and two existing ones. **The row's own
   enumeration refutes its own number.** 9 + 1 = 10.

Eleven is the count under DECISION C-2's *rejected* alternative (a
`cleanup_mutation.rs`), which the amendment does not adopt; ten-at-E1 is a count
under no reading at all.

**Why P1, not P3.** This is the same defect the freeze already graded once. The
§3.5 injection-site inventory carries its own correction header — "**(corrected
in round 2 — Code P1-2, State P2-1)**" (freeze `:943`) — recording that the
round-1 inventory was false and that "the recorded inventory was false and the
reserved-family scan was porous" (`:955-958`). Installing a fresh false count
into the addendum that correction produced, *and* instructing a converting
commit to write it, is that P1/P2 recurring in the object under review.

**Cure.** §2.5: strike the count edit from E1.2's duty list (E1.2 owes the
`FAULT_FAMILY_ACTIVATION` row edit and the source doc-comment extension, but no
addendum count move). §8 row 11: "**nine → ten**, the single move landing at
E2 with `barrier_mutation.rs`; E1 and E3 add sites to already-declared files
and move no count." Add the count trace to §9 item 1 so the aggregate is stated
once.

---

### [P1-2] The gate that actually governs O2/E4.1 — the A1 coexistence gate — was never restated by E0.1(b) and is absent from §5.3's precondition set. Its cure is pinned to R2-F.

The chain of custody breaks in three hops:

1. **Plan E4.1 (`GwzM5-8R2E-Plan.md:133-135`)**: "the O2 activation rides here:
   `recover_or_create`'s first production caller, **gated by the E0.1(b) §11.3
   restatement**." The plan deliberately delegates the gate's *content* to
   E0.1(b).
2. **E0.1(b) (`GwzM5-8R2E-E01ReachTraces.md:66-95`)** opens: "`GwzM5-8R2DSettledTuple.md`
   §11.3 ('Settle caveats the 5.2 dual should weigh', :798+) binds the
   activation as follows", then lists five rows. **Those five rows are not
   §11.3's.** They are §11.1's round-2 remediation block —
   `authority_name` (`:649-652`), `BarrierIntentV1::issue` (`:653-658`), the
   anchor nits (`:659-662`), the ten writer rows (`:663-665`) — plus §11.1's
   481-item row (`:645`). The real §11.3 (`:798-828`) has **eight** numbered
   caveats and shares none of its five with E0.1(b)'s list.
3. **§11.3 item 1 (`GwzM5-8R2DSettledTuple.md:800-802`)** is the gate:

   > **The A1 coexistence gate.** No production catalog activation at
   > `.gwz/checked-artifacts` until the relocation lands. Fail-closed in code
   > today: `recover_or_create` has no production caller (re-verified at this
   > tuple).

   And the relocation is **not R2-E's**. `GwzM5-8R2DSettledTuple.md` §11.2
   ("Pinned to R2-F", `:791`) carries "**Quarantine/relocation execution** — the
   legacy private area moves out of the catalog Final directory". The program
   checkpoint agrees, at `CurrentProgramCheckpoint.md:541-547`: "THE
   COEXISTENCE DECISION (audit P3-3, A1-gating) IS MADE AND RECORDED:
   **quarantine/relocation** … **execution pinned to the R2-F relocation
   package**; the A1 gate stays fail-closed in code."

**So E4.1 — the step this amendment's §5 is entirely about — cannot land its O2
half until an R2-F package lands, and nothing in the R2-E chain says so.**
`relocation` and `quarantine` appear **zero** times in `GwzM5-8R2E-Plan.md`,
**zero** times in `GwzM5-8R2E-E01ReachTraces.md`, and **zero** times in the
amendment draft.

The amendment is not unaware of the gate — §9 item 4 (`:1296-1299`) cites it
correctly at `GwzM5-8R2DSettledTuple.md:800-802` and says it is "untouched".
But *untouched* is not *satisfiable*. §5.3 enumerates E4.1's hard preconditions
as **five**, closes with "E4.1 may not land until all five hold", and omits the
one that is a cross-lane ordering dependency on a different milestone.

**Cure.** (a) Add §5.3 item 0, above the five: *E4.1's O2 half is additionally
gated by settled-tuple §11.3 item 1 — no production catalog activation until
the quarantine/relocation package lands, and that package is pinned to R2-F
(§11.2; checkpoint `:547`). E4.1 may land its `WorkspaceMutatorLock` conversion
half without it; `recover_or_create`'s first production caller may not.* (b)
Record the E0.1(b) mis-citation on the record, in the same form the plan §0 used
for 67 → 38, so the corpus stops propagating "§11.3" for §11.1's rows. (c)
Either split E4.1 into E4.1a (conversion) / E4.1b (O2 activation, R2-F-gated),
or re-own O2 — that is a lane-owner call, not mine, but the amendment must
surface it rather than leave it to be discovered at E4.

---

### [P2-1] §7 is titled "The §11.3 consumption table" and consumes §11.1's rows; §11.3's eight caveats are unaccounted, and item 2 — named an R2-E input twice — is dropped

The body of §7 is honest ("Every row of E0.1(b)"), and against E0.1(b)'s five
rows the accounting is **complete and correct: five in, five out** (see §6
below). The heading is not. Because the amendment will be cited downstream as
having consumed §11.3, the real §11.3 needs its own row-by-row close. Here it
is, as I make it:

| §11.3 item | subject | amendment's treatment |
| --- | --- | --- |
| 1 (`:800-802`) | A1 coexistence gate | cited in §9 item 4 as "untouched"; **missing from §5.3** → [P1-2] |
| 2 (`:803-808`) | test-door dependency; "**revisit on an R2-E production caller**"; "The Git-directory catalog's workspace-root binding **is an R2-E input** (disposition 3)" | **SILENTLY DROPPED — zero mentions** |
| 3 (`:809-811`) | `runtime.*` 18 keys declared executed with no per-key evidence | untouched-by-design; already ruled by the freeze annotation at `:465-479` ("restate the declaration"; "Counts. 165 total, unchanged; no key minted, none retired; 18/165 unchanged") |
| 4 (`:812-813`) | `managed_mutation.rs` at 1,251 lines | **CONSUMED** — DECISION C-2's cohesion arithmetic cites it correctly by item number (`:258`) |
| 5 (`:814-815`) | `staging_directory_flush`'s second site | **CONSUMED** — DECISION T-D's precedent; cited correctly by item number (Appendix C, `:1381`) |
| 6 (`:816-818`) | operator policy question | untouched-by-design (parallel-program debts) |
| 7 (`:819-825`) | two L2-05 gates wired into no workflow | untouched-by-design; owner is R2-F per A1 record §7 (`GwzM5-8A1ActivationRecord.md:167-172`) |
| 8 (`:826-828`) | over-budget Phase-4 steps | historical |

**Item 2 is the drop.** The settled tuple names it an R2-E input twice in one
row, and the *first* thing R2-E does that touches it is E1.2's cleanup matrix,
which must drive on both target variants including the Git-directory one — the
exact arm the test door serves. §2.5 requires "both target variants" without
noting that the Git-directory arm's door is a `cfg(test)` dependency the settle
asked R2-E to revisit, and freeze `:596-600` records the constraint in terms
("its Git-directory arm must drive through the Step-2.3 door
(`retain_managed_parent_at_for_test`) or place its managed prefix under that
target's own retained root, because a Git-directory catalog has no `.gwz`
ancestor"). E1.2 / E2.3 / E3.2 inherit it unwarned.

**Cure.** Retitle §7 "The E0.1(b) row consumption table (E0.1(b) rows 1-5,
sourced from settled tuple §11.1)", and add a short §7.5 closing the real
§11.3 in the eight-row form above, with item 2 given an owner (E1.2, in-commit,
with the door's disposition recorded) and item 1 routed to [P1-2]'s cure.

---

### [P2-2] O8's row denominators are unstated, and the figure the plan carries is superseded

The amendment adopts a mechanism (§6.3) whose consequence is a re-partition of
E5.1/E5.2's scope, and states no number anywhere in §6.

The live arithmetic, which I re-derived from `GwzM5-8R4bG-Evidence.md`:

- 39 M4 scenarios: 7 ADAPTED + 6 UNLISTED = **13 bound**; **22 UNBOUND**;
  **4 NO FIXTURE** (§12.7, settled tuple `:696-736`).
- §12.9 closed four of the 22 (`J-NO-PUBLICATION-UNBORN`, `F-BASELINE`,
  `F-MARKER`, `F-LOCK`). **§12.9(d) (`:1467-1470`) states the live residue in
  terms**: "**18 UNBOUND rows remain** (10 progress + 8 archive)".
- The ten archive shapes = **8** of the 18 UNBOUND + **2** in the NO-FIXTURE
  bucket (`AC-NOPUB-UNBORN`, `AP-PRESERVED`), which is what makes §6.4's
  "simultaneously (a) two of C-2's four unfixtured scenarios and (b) the two
  archive shapes riding [P2-1] option (i)" true.
- §12.7 closure (i)'s "**14** unbound progress shapes" is the **pre-§12.9**
  figure. Post-§12.9 it is **10**.

Now the amendment's own §6.3 ruling — *"No registry rows for archive shapes …
Cheap closure (i)'s registry rows are for the **progress** shapes only"* — which
I **sustain as correct** against §12.7 (iii) and §12.4. Its arithmetic
consequence is:

- **E5.1 owes 10 registry rows**, not the plan's "the 18 registry rows"
  (`GwzM5-8R2E-Plan.md:155-156`) and not O8's "the 18-UNBOUND per-scenario
  registry rows" (`:48`).
- **E5.2 owes 8 archive-corpus rows** (by clause, §12.9-disposition-table shape)
  **+ 2 PENDING-FIXTURE rows** carried to R2-F (§6.4 ✓).
- 10 + 8 = 18 ✓ — the plan's 18 is right as a *debt count* and wrong as a
  *registry-row count*. That distinction is the whole content of §6.3, and §6
  never draws it in numbers.

The amendment also never routes the **other two** NO-FIXTURE scenarios
(`B-NOT-STARTED`, `B-PREPARING-EMPTY`). L6 bullet 1 sends all four of C-2's to
R2-F; §6.4 names only the two archive ones. A reader of §6 alone cannot tell
whether R2-E owns the other two.

**Cure.** One numbered paragraph in §6.1 stating: 18 live per-scenario records
= 10 progress (registry rows, E5.1) + 8 archive (standalone corpus, E5.2);
+ 2 archive PENDING-FIXTURE → R2-F; + 2 non-archive NO-FIXTURE → R2-F, not
R2-E's. Correct the plan's O8/E5.1 "18 registry rows" on the record in the same
form plan §0 used for 67 → 38. Cite `§12.9(d)` (`:1467-1470`) as the authority
for 18 = 10 + 8, and `§12.4` (`:1194`) as the enumeration E5.2 works from.

---

### [P2-3] DECISION T-D's stated ground is not what the `staging_directory_flush` precedent holds, and it contradicts the amendment's own DECISION C-1

**The outcome is right. The ground is not, and it is going into frozen text.**

T-D (`:794-822`, Appendix B `:1336`) rests on: *"Binding this key to reading (b)
would announce **two keys at one boundary**, which is the mirror image of the
`staging_directory_flush` case the freeze resolved the other way — one key, two
sites … One boundary must have one name."*

Two problems.

**(a) The precedent does not say that.** Freeze `:645-648` reads:
`staging_directory_flush` is announced "at both of the writer's directory
flushes, the staged interior's and the managed parent's, **because both are the
state that key names and minting a second would move the frozen census**." The
holding is *one key may serve many boundaries*, and the operative ground is
**census**. Binding `terminal.authority_release` at reading (b)'s boundary mints
nothing — the key already exists and is already inside 165 — so the precedent's
census ground does not transfer, and its converse ("one boundary must have one
name") is not among its holdings. `durable_leaf.*`'s annotation (freeze
`:492-495`, "14 rows over 11 distinct keys … three boundaries shared") shows the
corpus is comfortable in the many-to-one direction and has never ruled on the
one-to-many one.

**(b) The amendment's own §2 does the opposite thing, three times.**
DECISION C-1 (`:220-236`) is precisely "another family already announces this
edge, so our key names the **post-edge state**", quoting
`managed_mutation.rs:1018-1027` and `managed_bootstrap.prior_generation_retire`
as the executed precedent. Under C-1's rule, `terminal.authority_release`'s
reading (b) has an obvious activated form — *"the admission record is `Idle`
and the catalog's next admission has not consumed it yet"* — which is
structurally identical to §2.3 key #8 (`cleanup.alias_retire`, activated) and
#9 (`cleanup.retired_alias_reobserve`, activated, sitting beside
`namespace.retired_reobserve`). T-D applies the opposite rule to the same shape
and never reconciles the two.

**What carries the determination instead.** Reading (a), alone and unaided.
`RetainedWriteAuthorityV1` is "**Deliberately neither `Copy` nor `Clone`**
(Step-3.3 review [P3-3])" (`coordinator/execution.rs:321-327`, verified), so
release is a move-out — an in-process event a restart cannot observe. That is
exactly and only the Phase-3 settle's ground: "not durable edges: no create,
write, flush, publish, retire, or reobserve occurs at either" (freeze
`:697-698`). And the same doc comment already anticipates R2-E: "R2-E's
consumers take it as the argument that says 'this replacement may proceed'"
(`:324-325`).

**Cure.** Re-ground T-D on reading (a) only. Demote reading (b) from "a
candidate binding rejected on one-boundary-one-name" to "a mis-reading of the
key's name: the return-to-idle edge is `admission.*`'s, fully executed at five
keys (`fault_v1.rs:92-96`), and `terminal.authority_release` does not name it."
Delete the "one boundary must have one name" sentence, or, if the lane wants
that doctrine, state it as a **new** ruling with its own grounds rather than as
an inference from `staging_directory_flush` — and then reconcile it with
DECISION C-1 and cleanup keys #8/#9 in the same breath.

**Determination-compatibility verdict, separately:** measured against the
Phase-3 settle determination's *form* (freeze `:692-706`), T-D reproduces seven
of its eight elements faithfully — named key, "never gains a fault boundary",
the state-vs-edge reason, the every-edge-bearing-key-executed statement, the
"correct evidence form for a state, as fault injection is for an edge" sentence,
"remains in the vocabulary; census stays 165/11", the key-by-key siteless proof
as permanent evidence, and §4.5's `PartiallyExecuted` duty. The eighth is
[P2-4]. **T-D is form-compatible; its ground is what fails.**

---

### [P2-4] §8 row 12 asserts something none of the three records says

§8's last row (`:1277`) proposes, as annotation content on §3.5's total-165
statement: *"Unchanged, and restated in each activation record: 165 total, no
key minted, **no key retired**."*

The three record headers, as drafted:

- §2 (`:176`): "Counts: 165 total, unchanged; no key minted; `cleanup.*` stays 0/11 until E1.2 lands."
- §3 (`:395-396`): "Counts: 165, unchanged; no key minted; `barrier.*` stays 0/16 until E2.3 lands."
- §4 (`:681`): "Counts: 165, unchanged; no key minted."

**None restates "no key retired."** The house form does: the `runtime.*`
annotation (freeze `:476`) — "165 total, unchanged; no key minted, none
retired; 18/165 unchanged" — and the Phase-3 settle determination (freeze
`:702`) — "census stays 165/30, no key retired". So the claim is not merely
unsupported; it names the very clause the corpus uses and the records omit.

Two secondary asymmetries in the same three headers: §4's alone omits the
"stays 0/11 until E3.2 lands" clause its two siblings carry; and T-D's own
census sentence ("census stays 165/11", `:818-819`) likewise drops "no key
retired" where its Phase-3 model has it.

**Cure.** Add "no key retired" (and §4's "stays 0/11 until E3.2 lands") to the
three record headers and to T-D, which is one word each and makes §8 row 12
true; or weaken row 12. The first is correct — the records are census
statements and should carry the full census clause.

---

### [P2-5] §5.3's five E4.1 preconditions govern the message and omit the ordering

DECISION §5.2 moves the durable-identity refusal from lock acquisition to
catalog consumption. That relocates a refusal **inside** an operation that is
already holding `WorkspaceMutatorLock`. §5's four grounds address availability
(Ground 2), doctrine scope (Ground 3) and witness freshness (Ground 4) — none
addresses **restart legality**: whether the later refusal can occur after the
operation's first durable mutation, and whether the state it leaves converges.

This is not hypothetical shape-hunting: `v1_lifecycle/checked.rs:127-134`
acquires the lease as an operation prologue (`V1MutationLease::acquire`, verified
in code) and holds it across the operation, and `archive.rs:97` acquires the lock
*before* the source-absence check it serialises. Whether the catalog consumption
in an E4-converted path precedes or follows durable work is an E4-time property
that today's tree cannot answer — which is exactly why it belongs in the
precondition set rather than being discovered at E4.

The five conditions as drafted are otherwise **complete against E0.1(a)**:
item 1 = the typed actionable message, item 3 = the runbook class, item 4 = the
release-notes line — E0.1(a) consequence 1's three elements, all three present
(`GwzM5-8R2E-E01ReachTraces.md:52-57`); item 2 = the scoping this decision
implies; item 5 = the `unsupported.rs` sweep, correctly folded in on E0.1(b)
row 3's own instruction. And §5.3's preamble ("E4.1 may not land until all five
hold") honours E0.1(a)'s "before, not after, the first production caller". ✓

**Cure.** Add a sixth condition: *the refusal is proved to occur before the
operation's first durable mutation, or the partially-mutated state it leaves is
proved convergent on restart — driven, not asserted, in E4.1's own rows.*
(And a seventh, per [P1-2].)

---

### [P2-6] The census paragraph's fixture cite covers 22 of the 38 keys

§1.1 (`:31-33`): "The three families' key strings are already in the vocabulary
at `fault_v1.rs:132-147` (barrier), `:178-188` (cleanup), `:189-199` (terminal),
and already in the fixture's expected list at **`fault_expected_keys.rs:151-172`**."

The three `fault_v1.rs` cites are exact. The fixture cite is not: `:151-172` is
**cleanup (`:151-161`) + terminal (`:162-172`) = 22 keys**. `barrier.*`'s
sixteen are at **`:105-120`**. The claim is true of all 38; the citation proves
it for 22.

This is the one paragraph in the document whose job is to prove "no key is
minted", so its cite has to close. **Cure:** `fault_expected_keys.rs:105-120`
(barrier), `:151-161` (cleanup), `:162-172` (terminal).

---

### [P3] findings

- **[P3-1] Systematic 1-2 line drift on the quote windows.** The quotes
  themselves are faithful; the ranges open early or late. Measured:
  barrier semantic sentence cited `:768-781`, actually **`:766`**-`:781`
  (draft `:72`); barrier re-reservation cited `:786-790`, actually
  **`:787`**-`:790` (draft `:74`) — note plan §0 cites `:787` correctly;
  `staging_directory_flush` precedent cited `:646-653`, actually
  **`:645-648`** (draft `:811-812`); the E4-retire clause cited `:1272-1276`,
  actually **`:1273-1276`** (draft `:730`); E0.1(b) row 1 cited
  `E01ReachTraces:71-77`, actually **`:72-77`** (draft `:410`). Cleanup's
  `:707`/`:712-721`/`:726-733`, terminal's `:791`/`:792-799`/`:873-882`,
  admission's `:481`, and all twelve §8 targets are **exact**.
- **[P3-2] §6.2 mis-attributes the archive-corpus barrier.** It offers
  §12.9(c)'s `assert_ne!(record.state, OperationState::Finalizing)` as "the
  structural reason the corpus cannot be widened". That barrier is
  `Finalizing`-specific; archive rows are Completed/Aborted and would pass it.
  The real ground is §12.7 (iii) — "no registry vocabulary in which an archive
  shape could be bound" — which §6.4 states correctly ("archived v0 never enters
  `adapt_open`"). Move the attribution.
- **[P3-3] DECISION T-A carries no contingency marker.** T-A's claim that the
  E4-retire clause's "no in-vocabulary target exists" is untrue of the action
  directory reads, in the freeze's own sentence, as the *conclusion* of a
  conjunction whose second conjunct is the empty-root requirement — i.e. T-A is
  contingent on OPEN-T1, which §8 row 1 does say ("blocked on OPEN-T1") but
  Appendix B's T-A row does not. C-3 carries "carries ordering condition
  OPEN-C1"; T-A should carry "contingent on OPEN-T1" in the same place.
- **[P3-4] §7.1's "next consumer" framing.** `authority_name` renders
  `"ca1-{family}-{action}.authority"` with no identity (`authority.rs:226-228`,
  verified) against `goal_name`/`source_name`'s 16-byte hex (`:234-236`,
  `:238-240`, verified) — the measurement is exact. But the checked path does
  not *consume* `authority_name`; it replaces it with digest-derived slot names.
  So the settle's trigger ("when it gives the family its next consumer",
  `GwzM5-8R2DSettledTuple.md:651-652`) does not fire in R2-E at all, and the
  honest disposition is *stronger* than the one drafted: the legacy name's only
  consumer remains the legacy `finish()`, which R2-F deletes. Recast A-1 that
  way; keep the E4.7 condition, which is a good guard either way.
- **[P3-5] Three E6 loose ends.** (a) O11's UX moves into E4.1, which empties
  plan step **E6.3** — and E0.1(a) answered O11's "if reach is not real" branch
  negative, so E6.3's "dated no-work record" branch is also void; the
  amendment never disposes of E6.3. (b) §7.2 moves `unsupported.rs` out of
  **E6.2** without restating what E6.2 still holds. (c) O12's fourth item —
  the abort rewrite-set bound guard tie (`GwzM5-8R2E-Plan.md:52`, R1.2 (b),
  g12:737-739) — appears **nowhere** in the amendment; it is not owed to E0.2,
  but O12's other three are all consumed here and its ledger row should say so.
- **[P3-6] `gc_archived` cite drift.** §6.4 cites `v1_lifecycle/archive.rs:112-118`
  for `gc_archived` and `:108-111` for the allowance reason; actual are
  **`:110-116`** and **`:106-109`**. L6's own cite (`archive.rs:110`) is correct
  and the draft drifted +2 from it.
- **[P3-7] §6.3's contract quote stops short of its cited range.** The quote
  ends at "…decoder/projection below."; the cite `:178-184` includes
  `:183-184`'s unquoted sentence ("A v0 `mode: no_ff` open row remains
  `UnsupportedLegacyMode` before resume/mutation"). Cite `:178-183` or quote the
  rest.
- **[P3-8] §6 never points E5.2 at the enumeration.** The ten archive shapes are
  enumerated at `GwzM5-8R4bG-Evidence.md` §12.4 (`:1194`+, "Table B — the 10
  archive shapes"). §6 cites §12.7/§12.8/§12.9 and not §12.4, so E5.2 inherits a
  mechanism with no pointer to its own row list.

---

## 4. Frozen-text consistency — the ledger

### 4.1 The three re-reservation quotes

| quote | cited | actual | byte-match |
| --- | --- | --- | --- |
| `cleanup.*` row | `:707` | `:707` | **exact** |
| `cleanup.*` semantic sentence | `:712-721` | `:711`(tail)-`:721` | **exact text** |
| `cleanup.*` re-reservation | `:726-733` | `:726-733` | **exact** |
| `barrier.*` row | `:763` | `:763` | **exact** |
| `barrier.*` semantic sentence | `:768-781` | `:766-781` | **exact text**, window early → [P3-1] |
| `barrier.*` re-reservation | `:786-790` | `:787-790` | **exact text**, window early → [P3-1] |
| `terminal.*` row | `:791` | `:791` | **exact** |
| `terminal.*` record | `:792-799` | `:792-799` | **exact** |
| §4.4 arm-table resolution | `:873-882` | `:873-882` | **exact**, ellipsis faithful |
| `admission.*` house form | `:481` | `:481` | **exact** |

**No re-reservation text is altered, paraphrased or silently trimmed.** The
"left as written / this annotation is the sanctioned mechanism" discipline is
observed in all three records and restated in §8 rows 1, 3, 6, 10.

### 4.2 The twelve §8 annotation targets

All twelve name a **real frozen row**; I opened each:

| # | target | verified |
| --- | --- | --- |
| 1 | §4.3 row **E7** `:1083` | ✓ "action directory retirement into the retired root \| 1/4 \| P1 with destination recheck \| `publication.rs:157-173`" |
| 2 | §4.3 **E12** `:1105` / **E13** `:1106` | ✓ both |
| 3 | §4.3 **E14** `:1107` | ✓ "backend `barrier` \| 2.2 \| P5 \| `platform.rs:271` \| `platform.rs:526` documented no-op" |
| 4 | E10/E14 activation annotation `:1108-1199` | ✓; its ground quoted from `:1116-1118` is faithful |
| 5 | §4.1 row **P5** `:980` | ✓ |
| 6 | §4.4 Class 1 arm table last row `:1382` | ✓ **verbatim** match to §8's rendering |
| 7 | §4.4 Class 2 (C-3) `:1398-1450`; ruling quote `:1443-1450` | ✓ **exact** |
| 8 | E4-retire activation record `:1247-1325`; clause `:1272-1276`; open question `:1313-1325` | ✓ (window → [P3-1]) |
| 9 | §4.3 **E22** `:1245`; Step-4.2 `:850-856` | ✓ both |
| 10 | §3.5 map rows `:707` / `:763` / `:791` | ✓ |
| 11 | inventory `:943-968`; addendum `:525-532` | ✓ targets real — **content wrong → [P1-1]** |
| 12 | total-165 statement `:922-923` | ✓ target real — **content wrong → [P2-4]** |

**Ten of twelve contradict nothing standing. Two do: rows 11 and 12.**

### 4.3 Contradiction sweep against standing frozen text

I checked each annotation's one-line content against the row it annotates and
found no other collision. Notably clean: row 3/row 4's handling of the E10/E14
annotation (it correctly confines that annotation's claim to *which arm a caller
takes*, which freeze `:905-909` states in the same terms — "The annotation's
subject is which arm a caller takes on Windows; that is untouched"); row 6's
resolution of the last arm-table row, which uses the same
conditional-resolution mechanism E16/E17/Step-4.2 used (freeze `:618`, `:874`);
and row 9's declining of the legacy nonce-orphan reclamation, which matches
freeze `:855`'s "Reclamation is an optional R2-E item, not a debt of this step"
and correctly records the decline so the option does not lapse silently.

---

## 5. The §5 decision as doctrine — RULING: option (ii) is CORRECT and consistent

**I sustain DECISION §5.2.**

**Availability, verified.** I enumerated `WorkspaceMutatorLock::acquire` /
`try_acquire` call sites across `gwz-core/src` myself. The production set is
**exactly nine**, and the draft's nine `file:line` pairs are **exact**:
`handle_create_repo.rs:35`, `handle_init_from_sources.rs:91`,
`merge/abort/mod.rs:44`, `merge/continue_op/coordinator.rs:13`,
`merge/gc.rs:167`, `merge/runtime/mutation_guard.rs:44`,
`v1_lifecycle/archive.rs:97`, `:126`, `v1_lifecycle/checked.rs:129`. (Four
further hits are test-only: `merge/runtime/tests/mutation_guard.rs:20/:26/:53`,
`workspace_ops/tests/g02.rs:281`.) **Six of the nine never consume the catalog.**
Option (i) would refuse `gwz repo create` and `init-from-sources` — operations
with no catalog relation at all — on a whole filesystem class. Ground 2 holds
as measured.

**Doctrine, swept.** I looked for any prior ruling that binds fail-closed at
*capability acquisition* rather than at *dereference*. There is none. The
corpus's statements all bind at the point of use:

- `AgentProcessRules.md:1823-1824` (the L1 compact block): "Persist typed
  authority **before external mutation** and fail closed on **ambiguous
  recovery**" — mutation and recovery, not acquisition.
- The freeze's own Step-4.2 remediation, which the draft cites correctly at
  `:814-825`: "E16's standard admits a permanent typed refusal only where the
  wedge state is unreachable or off the supported table, and this one was
  neither" — the corpus's one recorded case of over-refusal, and it was **fixed
  as a defect**. This is the closest precedent to the question and it points at
  option (ii).
- `GwzM5-8A1DecisionPacket.md:150-155` / `:222-225`, ratified at
  `GwzM5-8A1ReleaseR2-ReviewState.md:640-644` and `:709-711`: the attribute-driven
  residual is frozen as "permanent fail-closed doctrine" — i.e. a *named residual
  class that refuses where the capability is absent*, precisely option (ii)'s
  shape, never a pre-emptive product-wide disable.
- The one apparent counter-example — the advisory-lock capability refusal that
  *does* fire at `try_acquire` (`bootstrap/runtime/mod.rs:237-243`) — is not one:
  advisory exclusivity is a proof the lock itself dereferences. Ground 3 draws
  exactly that line and draws it correctly.

**Against the thin-A1 / §10 "runtime bootstrap only" reading.** The frozen §10
row is byte-exact as quoted (`GwzM5-8R4bR2ConsumerCheckpoint.md:272`:
"| `WorkspaceMutatorLock` | runtime bootstrap only | acquire final lease before
open-operation gate and mutation |"). Option (ii) is the reading that makes
"runtime bootstrap only" mean something; option (i) would make the runtime lease
a catalog-capability gate, which is what the row's declared purpose denies.
E0.1(a) itself called option (ii) "the conservative reading" (`:62-63`). §5.2's
frozen-text statement (`:1019-1023`) says this cleanly and I would sign it.

**RULING:** *probe-at-consumption is consistent with every fail-closed ruling in
the corpus; no ruling binds fail-closed at capability acquisition; and the §10
row's declared purpose is load-bearing exactly as §5.2 says.* The E4.1
precondition text carries all three of E0.1(a)'s bound elements plus two
correct additions — **but the set is incomplete on ordering ([P2-5]) and on the
A1 coexistence gate ([P1-2])**, and it is those two omissions, not the
doctrine, that hold §5 back.

---

## 6. §6 / O8 — RULING, and the citation-drift adjudication

**Does the two-tier split satisfy L6 or narrow it? — IT SATISFIES IT.**

- L6's binding is "the registry rows and the parametric test land as one
  package" (`GwzM5-8A1ActivationRecord.md:161-166`). §6.1 restates it as "in one
  commit. Neither may land alone; the precheck walk test does not discharge
  either" — **faithful, and slightly stronger** (commit ⊂ package). The
  quotation of L6 is content-exact.
- All four items L6 routes to R2-E's archive/GC sub-package are touched: the
  18-UNBOUND record debt (§6.1 → E5.1), the mechanism decision (§6.3), the two
  archive shapes (§6.4), `gc_archived` (§6.4 tail → E4.4).
- The two-tier split does not narrow the clause, because the clause has two
  claims. §12.7 (iii) states O8's archive owing as "proof that operations
  *finishing under v1* produce archives byte-equivalent to v0's" while the
  contract's byte-preservation clause is about records that *remain v0*. Tier 1
  binds the second, tier 2 binds the first with the instrument the seven ADAPTED
  cases already use. **The rejected alternative is correctly rejected**: a single
  byte gate is unsatisfiable for tier 2 by construction, and §6.3 names the exact
  failure mode that would follow (a permanently-red gate, or a lane reporting the
  clause met on v0-origin archives alone).

**Is the PARTIAL statement preserved? — YES, explicitly and correctly.** §6.4
(`:1152-1154`): "**E5.2 must not report the O8 archive clause met on these two
rows**, and the PARTIAL statement (`GwzM5-8R4bG-Evidence.md:1367-1395`) survives
R2-E unchanged for them." I verified §12.8 spans `:1367`-`:1395` (§12.9 opens at
`:1397`) and that §12.8's own words are "A green byte-equivalence battery is
**not** claimed by this gate and must not be cited from it." The draft's
disposition token (**DISPOSITIONED-PROJECTION-ONLY, UNFIXTURED**, rows
**PENDING-FIXTURE** with R2-F named) is the right shape and preserves the gate.

What §6 does **not** do is state its numbers — [P2-2].

### 6.1 The +19 citation drift — ADJUDICATED

**The drift is real, the draft's replacement numbers are correct, and the
mechanism is a single dated insertion.**

- **Mechanism.** gwz-dev commit **`4b9f078`** ("A1 activation commit:
  operator-signed record set lands", **2026-08-25 02:13:43 +1000**) touched this
  file and only this file, with a diffstat of
  `dev-docs/GwzM5-8I2CompatibilityContract.md | 19 +++++++++++++++++++` —
  **exactly 19 insertions**: one blank line at `:82` plus the 18-line dated
  blockquote at **`:83-100`** ("[Dated annotation, 2026-08-25 — A1 row partial
  engagement. Frozen text above unchanged.]"). Frozen §4 text ends at `:81`;
  what was `:83` is now `:102`.
- **The drift is therefore +19 for every line from old `:83` onward and ZERO for
  lines `:1-:82`.** The draft calls it "a uniform **+19-line** drift" (`:1088`);
  that is true of the two passages at issue and **not** true of the file. State
  it as *"+19 for all content below the 2026-08-25 annotation"*.
- **Correct current line numbers, verified by me directly (not by arithmetic):**
  - the whitelist passage ("A1 deliberately whitelists only seven
    one-member-workspace…" through "…are not A1 migration rules.") is at
    **`:136-144`** — 9 lines, matching R4b-G's 9-line `:117-125`.
  - the "Zero whitelist matches…" passage is at **`:178-184`** — 7 lines,
    matching R4b-G's 7-line `:159-165`.
  - **Both of the draft's numbers are CORRECT.** (Its quote stops at `:183`
    mid-line; see [P3-7].)
- **The drift is corpus-wide, not R4b-G-local.** The draft tells E5.2 to
  re-verify *R4b-G's* cites. The same stale pair survives in at least six
  documents, one of which is on R2-E's own inherited register:
  `GwzM5-8R2DSettledTuple.md:702` **and `:706`** (the C-1 row);
  `GwzM5-8R4bG-Evidence.md` §12.7 (`:1298`-`:1305`), **§12.9(d) (`:1469`)**, and
  `:1187` (which cites `:123`, now **`:142`**);
  `GwzM5-8R4bG-ReviewEvidence.md:119`; `GwzM5-8R4bG-ReviewCorrectness.md:546`;
  `GwzM5-8M5bNoFfDesign.md:112`, `:166`, `:234`, `:321`, `:375` and
  `GwzM5-8M5bNoFf-ReviewCode.md:58`, `:79` (which cite `:145` → now **`:164`**
  and `:163-165` → now **`:182-184`**).

**RULING ON HOW E5.2 MUST CITE:**

> **Content-anchored, not line-anchored.** E5.2 (and any lane citing the
> compatibility contract) must cite the contract by **§ number plus a quoted
> anchor phrase**, with the line number offered as a convenience and marked as
> of a stated date — e.g. *"`GwzM5-8I2CompatibilityContract.md` §5, 'Zero
> whitelist matches is not an error … byte-preserving archival' (`:178-184` as
> of 2026-08-27)"*. The contract is a **live annotatable frozen document**: its
> own annotation mechanism inserts dated blockquotes above frozen text and
> shifts every line below them, so a bare line cite decays by construction on
> the next annotation. E5.2 must **not** silently re-point R4b-G's, the settled
> tuple's, or M5b's existing cites — those are dated records and are left as
> written, per the same "left as written / annotation is the sanctioned
> mechanism" discipline this amendment applies to the freeze. What E5.2 owes is
> a short dated drift note recording the +19 and the mechanism, so the next
> reader resolves the stale numbers rather than re-deriving them. The
> amendment's §6.3 note is the right instinct; it should be scoped to the whole
> corpus rather than to R4b-G alone, and should name commit `4b9f078` as the
> mechanism.

---

## 7. §7's five-row accounting — RULING: complete against E0.1(b), mis-titled against §11.3

Against E0.1(b)'s own five rows, **five in, five out**, and each disposition is
well-formed:

| E0.1(b) row | disposition | my assessment |
| --- | --- | --- |
| 1 `BarrierIntentV1::issue` | **CONSUMED** (§3.1-§3.2) | ✓ obligation quoted byte-exact from settled tuple `:653-658`; ordering constraint quoted from E0.1(b) and made load-bearing on the plan; residual OPEN-B1/B2 correctly declared |
| 2 `authority_name` weigh | **TAKEN** (§7.1) | ✓ **well-formed**; see below |
| 3 anchor nits | **SPLIT** 1 consumed / 2 deferred (§7.2) | ✓ **faithful to E0.1(b)'s own instruction** |
| 4 ten writer rows' native ledger debt | **DEFERRED**, owner + trigger + form + verifier | ✓ well-formed deferral |
| 5 481-item frozen surface | **DEFERRED to E7.2** with denominators restated | ✓ and it *improves* on the plan by carrying both figures (481 blanket-hidden / 1657 spans across 85 files) and by requiring the count to fall |

**Row 3's split — faithful.** E0.1(b) row 3 (`:83-88`) itself carries the
instruction: "note the adjacency to (a): the capability-refusal UX work should
sweep this claim". §7.2 moves `unsupported.rs` into §5.3 item 5 and defers the
other two to E6.2 with per-nit terms (the bound, the canonical parse, and why
convergence is unaffected). The split follows E0.1(b)'s text, not the drafter's
convenience. **Sustained.**

**Row 2 — taken, and well-formed against the settle's trigger.** The settle
requires the weigh "when the family gains its next consumer"
(`GwzM5-8R2DSettledTuple.md:651-652`); E0.1(b) permits either taking it in E0.2
or "explicitly deferred to the consuming E4 step with the weigh recorded"
(`:80-82`). Taking it at E0.2 discharges the constraint *a fortiori* — earlier
than the trigger — and the weigh is genuinely performed (both alternatives
measured in code, three grounds for rejection, an E4.7 reopening condition
attached). **The deferral question does not arise, and the taking is
well-formed.** My only quarrel is with its framing → [P3-4]: the checked path is
the family's *replacement*, not its next consumer, and saying so makes the same
conclusion stronger.

**The mis-titling and the eight-row §11.3 close** are [P2-1].

---

## 8. The OPEN register — form, and the B1/T1 process ruling

### 8.1 Form: all five rows well-formed

Each of OPEN-B1, B2, B3, C1, T1 states (i) the exact question in interrogative
form, (ii) at least one candidate mechanism where one exists, (iii) an explicit
"*Evidence that would settle it*" clause, and (iv) an owner. Appendix A's table
is consistent with the five in-text rows on id, grade, blocking status and
owner. §9 item 6 lists the same five. **No OPEN row is asserted without its
settling evidence, and no OPEN row is silently graded.** Sustained.

### 8.2 B1 and T1 are REAL — I verified both in code

Both findings turn on two `?`-guards inside one function
(`capability/pre_catalog/provider/interior.rs`):

- `:349` `let retired = empty_directory_identity(interior, Slot::RetiredActions)?;`
- `:350` `let roaming = exact_file_identity(interior, Slot::RoamingAnchorHome, ROAMING_ANCHOR_BYTES)?;`

and `empty_directory_identity` (`:808-818`) returns `Some` **only** for
`RawCatalogInteriorFactV1::EmptyDirectory`, which the observer produces only when
the first `entries()` item is `None` (`:466-474`), reached because `observe`
passes `slot == InfrastructureSlotV1::RetiredActions` as the
`probe_empty_directory` argument (`:108-113` — verified verbatim). Every
`file:line` the draft gives for B1 and T1 is **accurate**.

So: while the roaming anchor is away, `completed_record` → `None`; and the
instant one retired action directory is resident, `completed_record` → `None`.
**Both findings are real, and the draft's "same defect class" note is right** —
they are two guards encoding one static-catalog assumption, met by a lifecycle.

### 8.3 RULING ON THE PROCESS SHAPE

The register routes both to "**dual #1 to rule**; E2.1/E3.1 to implement"
(Appendix A `:1316-1317`), and §4.4's note asks the dual to rule them as one
package. **I decline the routing as drafted, and rule as follows.**

**(a) The dual may not author a frozen-surface widening.** Plan §2 charters dual
#1 because the amendment "extends the frozen §3.5/§4.3 surface — the same class
as R2-D's Phase 0", and plan E0.3's output is binary: "GO/GO unblocks E1-E3 in
parallel." Nothing charters a reviewer to *write* an extension of
`completed_record`, the predicate `retain_completed_catalog` (`completed.rs:64-74`),
the retained-catalog revalidation (`:183-211`) and the publication destination
recheck (`publication.rs:225`, `:264`) all depend on. The drafter's own sentence
is right about the danger and wrong about the remedy: *"a reviewed edit the dual
should authorize explicitly rather than inherit from a table row"* (`:627-629`)
— correct that it must not be inherited; incorrect that authorising an
unauthored, unreviewed edit is different in kind from inheriting one.

**(b) Nor may the amendment ship OPEN-carrying into an unblocked E2/E3.** A GO
that unblocks E1-E3 in parallel while E2.1 and E3.1 are each blocked on an
unruled frozen-surface contradiction converts a blocking finding into an
implementation-time decision, taken by whichever lane reaches it first, in a
worktree, under a <500-LOC step budget. That is the failure mode the two-dual
tiering exists to prevent.

**(c) The shape the corpus already uses for this exact class.** Freeze
`:1443-1450`'s Class 2 / C-3 ruling *was itself made in a Phase-0 freeze
document* and implemented in a later phase's converting commit — the ruling in
frozen text, the implementation in the package. And the E4-retire record's
"**Open question carried to the re-verdict**" (freeze `:1313-1325`) is the
precedent for a named open question routed to a *named subsequent gate* with
the decision's shape pre-stated. Both say the same thing: the widening's text
belongs in an amendment, reviewed as an amendment.

> **RULING — the cure decision lands in an E0 remediation round, as an
> amendment addendum, not as a dual ruling and not at E2.1/E3.1.**
>
> 1. **This dual rules — and I do rule — that OPEN-B1 and OPEN-T1 are REAL,
>    that they are one defect class, and that the Class-2 (C-3) widening is the
>    correct mechanism class for both**: extend the *provider's reading* of a
>    closed vocabulary, mint nothing (freeze `:1449-1450`). That is reviewer
>    work and I have done it. I also concur with the drafter's recommendation of
>    candidate (i) over candidate (ii) for B1, on the drafter's own evidence
>    (a): the descriptor consumes the anchor's **durable identity**, a rename
>    preserves the object, so a moved-and-returned anchor reproduces
>    byte-identical `RetiredActionsDescriptor` / `CatalogFormat` rows — which
>    candidate (ii)'s copy-not-move does not, and which additionally falsifies
>    keys #6/#10's own names.
> 2. **The lane authors the widening as `E0.2b` — one addendum, both readings**
>    (the roaming-anchor-away home row; the populated retired root under
>    `RETIRED_ROOT_BUDGET_V1`), with each widened arm's refusal condition, its
>    bounded reader, and its census statement. One package, per §4.4's note:
>    E2.1 and E3.1 then share one precondition step rather than two.
> 3. **E0.3 round 2 reviews E0.2b** under plan §2's two-round remediation cap.
>    GO then unblocks E1-E3.
> 4. **Fallback, if the lane wants to start work before E0.2b exists:** a
>    partial GO unblocking **E1 only**. `cleanup.*` carries no blocking OPEN —
>    OPEN-C1 is an in-commit ordering proof, correctly graded non-blocking, with
>    the right consequence stated ("if unprovable, C-3 reopens"). E2 and E3 stay
>    gated on E0.2b. This preserves one of three parallel lanes at zero risk and
>    is my recommended path if schedule pressure is real.
>
> **What I rule out explicitly:** shipping the amendment with B1/T1 marked
> "ruled by dual #1" on the strength of a reviewer's paragraph, and shipping it
> OPEN-carrying with E2/E3 unblocked. Either would put an unreviewed edit to
> `completed_record` inside a <500-LOC implementation step.

---

## 9. The ledger close-check — O1 through O12

| # | obligation | status against this amendment |
| --- | --- | --- |
| **O1** | §10 consumer conversion table, executed | **ADVANCED** — E4.1's blast radius decided (§5.2) and its gate specified (§5.3); §9 item 4 correctly declares the table unexecuted |
| **O2** | first production catalog activation | **ADVANCED WITH A GAP** — §5 presumes E4.1 carries it; the governing gate (§11.3 item 1, relocation pinned to R2-F) is **not in the precondition set** → **[P1-2]** |
| **O3** | legacy writers stop mutating inside `.gwz/checked-artifacts` | **ADVANCED** — §7.1's E4.7 condition binds E4.7 to record, per converted consumer, whether `cleanup.rs`'s `finish()` is still reachable; that is O3's own close-out surface |
| **O4** | `cleanup.*` semantics + injection, 11 keys | **DISCHARGED for E0** — §2, 11/11, duties stated for E1.1/E1.2 |
| **O5** | `barrier.*` semantics + injection, 16 keys | **DISCHARGED for E0** — §3, 16/16 |
| **O6** | BINDING: `BarrierIntentV1::issue` observe-or-refuse | **DECIDED** — §3.2, per-fact, all three parts of the Step-4.3 precedent applied (owner observes; owner refuses; issuer's signature carries the derivation obligation); the ordering constraint is quoted and made load-bearing |
| **O7** | `terminal.*` semantics + injection, 11 keys | **DISCHARGED for E0** — §4, 10 activated + 1 determined; ground of the determination → **[P2-3]** |
| **O8** | archive/GC sub-package | **DECIDED** — §6.3/§6.4 mechanism sustained; **row denominators unstated and superseded** → **[P2-2]** |
| **O9** | [P3-R2-2] composed-path upgrade-failure test + moved-pin restoration | **untouched-by-design** — E6.1's, not E0.2's. Note the amendment builds the machinery plan E6.1 says it "reuses"; a forward pointer would be hygiene, not a debt |
| **O10** | [P3-R2-1] `AtomicUpgradeFault` hygiene | **untouched-by-design** — E6.2's; but E6.2's residual content is not restated after §7.2's move → [P3-5] |
| **O11** | `DurableObjectIdentity` reach + capability-refusal UX | **ADVANCED** — E0.1(a) answered the reach; §5.3 folds the UX into E4.1's gate. **E6.3's disposition is left dangling** (both its branches are now void) → [P3-5] |
| **O12** | four riders: abort-bound guard tie; `authority_name` weigh; anchor nits; 481-item reconciliation | **THREE OF FOUR CONSUMED** (§7.1, §7.2, §7.4). **The abort rewrite-set bound guard tie is unmentioned** → [P3-5] |

**Silent drops found: two.** §11.3 item 2 (the test door / Git-directory
workspace-root binding, named an R2-E input twice) → [P2-1]; O12's
abort rewrite-set bound guard tie → [P3-5]. **Everything else is either
advanced by this document or untouched-by-design with a standing owner.**

---

## 10. What I sustain

So the record is not read as uniformly negative: the following are correct,
verified, and should survive remediation unchanged.

1. **The census.** 165 / 0 minted / 0 retired / 38 accounted / three families
   still `Reserved`. Machine-verified, key-for-key, in vocabulary order.
2. **The three re-reservation quotes**, byte-exact, with the left-as-written
   discipline observed.
3. **Ten of the twelve §8 annotation targets**, each naming a real frozen row
   with content that contradicts nothing standing.
4. **The O6 resolution** (§3.2): the per-fact table is correct against the code
   it cites, the owner-minted-witness mechanism applies all three parts of the
   Step-4.3 precedent, the rejected alternative is rejected on the precedent's
   own ground, and the typed REFUSE arm names a real reachable state.
5. **The §5 decision**, on doctrine and on availability — nine call sites,
   exact; six of them catalog-unrelated; no corpus ruling binds fail-closed at
   acquisition.
6. **The O8 two-tier mechanism**, which satisfies L6 rather than narrowing it,
   and **the PARTIAL statement's preservation**, which is explicit and correct.
7. **The +19 drift note's replacement numbers** (`:136-144`, `:178-184`) —
   both correct, independently re-derived.
8. **OPEN-B1 and OPEN-T1 as findings** — real, accurately cited, correctly
   classed together. Finding them is the best work in the document.
9. **§7's five-row accounting** against E0.1(b), and row 3's split in
   particular.
10. **DECISION T-D's outcome**, on reading (a)'s ground.

---

## 11. Remediation list (one round)

**Blocking:**

1. [P1-1] Fix the source-file count: strike E1.2's count edit (§2.5); §8 row 11
   → "nine → ten, at E2".
2. [P1-2] Add the A1 coexistence gate to §5.3 as a precondition above the five,
   with the R2-F relocation pin named; record E0.1(b)'s §11.3/§11.1
   mis-attribution on the record.
3. [P2-1] Retitle §7; add §7.5 closing §11.3's eight items; give item 2 an
   owner.
4. [P2-2] State O8's numbers (10 registry / 8 archive-corpus / 2 PENDING-FIXTURE
   / 2 non-R2-E) and correct the plan's "18 registry rows" on the record.
5. [P2-3] Re-ground T-D on reading (a); delete or independently justify
   "one boundary must have one name"; reconcile with C-1.
6. [P2-4] Add "no key retired" to the three record headers and to T-D; add
   §4's "stays 0/11 until E3.2 lands".
7. [P2-5] Add the refusal-ordering / restart-legality precondition to §5.3.
8. [P2-6] Fix §1.1's fixture cite to the three ranges.
9. Process: adopt §8.3's ruling — author `E0.2b` for the B1/T1 widening, or take
   the E1-only partial GO.

**Non-blocking, fold in:** [P3-1] the five citation windows; [P3-2] the §12.9(c)
attribution; [P3-3] T-A's contingency marker; [P3-4] A-1's "next consumer"
framing; [P3-5] E6.2/E6.3/O12's fourth rider; [P3-6] `gc_archived`'s lines;
[P3-7] §6.3's quote range; [P3-8] the §12.4 pointer.

---

**VERDICT: NO-GO — round-2 remediable. Census clean; §5 doctrine sustained;
B1/T1 real and routed to an E0.2b addendum, not to this dual's signature.**

*Filed verbatim. State axis, R2-E E0.3 dual #1, 2026-08-27.*

---
---

# Round 2 — focused re-verdict (2026-08-27)

Axis: **State**, peer-blind (no Code-axis material read; the addendum's own
quotations of the Code ruling are treated as the addendum's claims and were
re-derived independently where load-bearing).

**Object:** the **pair** — `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` (E0.2) and
`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` (E0.2b, 1,327 lines, read in
full), **addendum controlling** — plus the corrected
`GwzM5-8R2E-E01ReachTraces.md` and `GwzM5-8R2E-Plan.md` E4.1 row.

**VERDICT: CONDITIONAL GO.**

- **GO on the semantics object.** Both round-1 [P1]s are cured, all six [P2]s
  are dispositioned acceptably, the census re-counts clean over the pair, and
  E1/E2/E3 are unblocked.
- **NO-GO on §7.6-§7.8 as operator decision-support**, on two new [P2]s. The
  sequencing analysis must not reach the operator on an eight-of-nine
  enumeration, or with a table that contradicts the ground it supports. **Both
  corrections strengthen option (b); neither reverses the recommendation.**

The split is legitimate because the two are separable: §7.6 relief 1 is
correct — E1-E3 land production-unreachable code exactly as R2-D's Phases 1-4
did, and the relocation gate governs E4 only.

---

## R2.1 THE SEQUENCING PREMISE — ruled: TRUE for what it maps, NOT COMPLETE

### R2.1.1 I attacked it four ways and could not break it for the eight rows it maps

| attack | result |
| --- | --- |
| **`.git/info/exclude` (`:279`)** — its frozen ordering *forbids* the managed door ("**never bootstrap `.git/info`**"), so if any row escapes, this is it | **fails.** It is a "checked Git-directory artifact action", so it needs an `AdmittedActionV1`; `ActionNamespace` is constructible **only** `from_admitted` (`namespace/mod.rs:85-93`) → admission → catalog. Concede. |
| **`WorkspaceMutatorLock` (`:272`)** — I did not accept "already converted"; I re-tested the *ordering* half of its frozen row ("acquire final lease **before open-operation gate** and mutation") at every production site | **fails — the ordering already holds.** `mutation_guard.rs:44` acquires before `enforce_open_merge_gate` at `:47`; `continue_op/coordinator.rs:13` before `discover_open` at `:14`; `abort/mod.rs:44` before `discover_open` at `:52`; `archive.rs:97` before `open_record_present` at `:98`; `:126` before `any_open_record_present` at `:127`. The lock row genuinely has no residual conversion work. Concede. |
| **The "existing parent" rows (`:274`, `:278`)** — "no parent creation" / "existing managed parent", so no bootstrap door | **fails.** No bootstrap, but still admission. Concede. |
| **The door census itself** — is there a third door? | **fails.** `OpaqueRetainedCatalogV1`'s only entry chain is `WorkspaceRuntimeLease::catalog_mutation_lease()` → `recover_or_create` (E0.1(a) `:32-36`), so *any* catalog use **is** O2. The two-door claim at `coordinator/execution.rs:141`/`:199` is structurally sound, and I verified both call `from_retained_catalog` on an `OpaqueRetainedCatalogV1`. |

**So the catalog-dependency claim is TRUE for all eight rows §7.6 maps.** I
sustain that, and with it the collapse of option (c) *as a partition of the
substantive §10 conversions*.

### R2.1.2 [P2-R1] The §10 table has NINE rows. §7.6 maps EIGHT.

`GwzM5-8R4bR2ConsumerCheckpoint.md` §10 runs `:270` (header), `:271`
(separator), **`:272-280` = nine consumer rows**. §7.6's table accounts for
`:272`, `:273`, `:274`, `:275`, `:276`+`:277` (collapsed at E4.5), `:278`+`:279`
(collapsed at E4.6) — **eight**. Missing:

> `:280` | **v1 checked store/root/bundle paths** | same purposes and artifact actions | **test-gated until A1; no legacy raw writer** |

The plan's O1 (`GwzM5-8R2E-Plan.md:41`) enumerates the same eight and drops the
same row, so §7.6 inherits the gap rather than creating it. **The omission is
not harmless:**

- **It is the one §10 row whose gate has already fired.** "Test-gated until A1",
  and A1 shipped 2026-08-25 (`4b9f078`; core `1a31851`). Every other row's
  precondition is still ahead of it; this one's is behind it.
- **Its second clause is catalog-free, and it is not met.** Verified for this
  round: `v1_lifecycle/store/rewrite.rs:6` imports
  `crate::durable_fs::{rename_durable, sync_dir}`; `store/archive.rs` and
  `archive.rs` do likewise; and **`v1_lifecycle/` holds zero non-test
  `checked_artifact` references** (the only hits are a doc comment at
  `start.rs:15` and test modules). Post-A1 `no-ff → v1` is live end-to-end
  (`GwzM5-8I2CompatibilityContract.md:85-87`), so this is a **production** raw
  writer on the v1 path — precisely what `:280` forbids once its test-gate
  expires.
- **O1 cannot be reported DISCHARGED without it**, under either sequencing
  option, and no plan step owns it.

**Cure.** Add row `:280` to §7.6's table with its door analysis; add it to plan
O1's enumeration on the record (the 67→38 / 18→10 form); and give its
"no legacy raw writer" half an owner — either a pin in an E4 step or a dated
accepted-residual record. Its *substantive* half ("same purposes and artifact
actions") is catalog-bound like the rest, so it does not revive option (c).

### R2.1.3 [P2-R2] O3 has a catalog-free discharge route under option (b), and §7.6's table denies it

§7.6 rows O3/E4.7 as "**YES, transitively** — requires E4.2-E4.6 landed". That
is true of the *plan's* E4.7 mechanism and false of O3 *as an obligation*.

O3's ledger text is a **location** property, not a conversion property:
`GwzM5-8R2E-Plan.md:43` — "Legacy writers stop mutating **inside**
`.gwz/checked-artifacts`".

And I verified the addendum's own §7.4 fact and pushed it one step further:
**the legacy private parent has exactly one non-test owner.** Tree-wide grep of
`gwz-core/src`:

- `checked_artifact/policy.rs:33-42` — `CheckedArtifactPolicy::private_parent()`,
  the definition;
- `checked_artifact/observation.rs:93` — `let quarantine_parent =
  policy.private_parent();`, **the only consumer**.

(Every other `private_parent` hit is a *different* function — the catalog
owner's own `create_git_private_parent` / `has_private_parent` /
`require_create_private_parent` — not the legacy policy's.)

**Relocating that one function moves every legacy write out of
`.gwz/checked-artifacts`, and O3 becomes literally true with zero conversion,
zero admission, zero catalog.**

This **cuts for the proposal, not against it.** It is exactly what ground 4
asserts — "The circularity breaks only if relocation lands as a legacy-only
change before conversion" — and it means §7.6's table and §7.8's ground 4
**contradict each other**, with the table in the wrong. Two consequences:

1. §7.6 **understates option (b)'s benefit**: under (b), relocation discharges
   O3 directly rather than merely unblocking E4.7.
2. §7.8(a) **overstates option (a)'s cost**: it says (a) gates "E4.1 through
   E4.7 entire, and with them O1, O2 **and O3**". O3 is not *gated* by
   relocation — it is *satisfied* by it. Under (a) the honest statement is that
   O3 is **discharged by R2-F's package and re-owned**, not blocked; the blocked
   pair is O1 and O2. **An overstated cost on the rejected option is a defect in
   decision-support material even when the recommendation is right.**

**Cure.** Correct the O3 row of §7.6's table; restate §7.8(a)'s cost as
O1 + O2 blocked, O3 re-owned; and promote §7.4's single-owner finding into
ground 4, where it is the fact that makes the circularity *breakable* rather
than merely real.

### R2.1.4 Ground 4 — RULED: it HOLDS, and it is the ground that turns preference into structure

I tested all three legs and each is real:

| leg | verification |
| --- | --- |
| E4.7 retires legacy writers **behind the converted paths** | plan `:146-148` states exactly that |
| the converted paths need the catalog | §7.6, sustained at R2.1.1 |
| the catalog needs relocation | §11.3 item 1 (`SettledTuple:800-802`); mechanism verified at `interior::exact_row`'s refusal of any non-`ActiveAction`, non-infrastructure child (`interior.rs:414-417`), reached via `RootEntryNameV1::parse` → `Foreign` (`slots.rs:434-436`) |

It is a genuine circularity, and [P2-R2] supplies the fourth leg that breaks
it: relocation is a **single-owner, legacy-only** edit. **Ground 4 sustained
and strengthened.**

I also re-verified the other four grounds, since the operator will rely on
them:

- **Ground 1** (relocation is catalog-free and self-contained) — **verified**:
  one owner, one consumer, as above.
- **Ground 3** (R2-F is downstream of relocation, not upstream) — **verified
  verbatim**: `ConsumerCheckpoint:304-305` — "**R2-F — closure.** Remove legacy
  successful paths and run complete local and native-platform fault/restart
  gates"; and `GwzM5-8R2DPhase4Closure.md:124-126` — "R2-F's deletion of the
  legacy family **removes a directory rather than un-widening a frozen
  grammar**". Relocation enables R2-F.
- **Ground 5** (MAX_PATH banked a release earlier) — **verified** at
  `GwzM5-8R2DPhase4Closure.md:127-131`.
- **The cost statement** — **verified verbatim** at `:132-133`: "the move is a
  durable relocation of live production state, needing its own package."

**All five grounds are sound.** The charter change is the operator's ruling and
I take no position on it; my finding is that the analysis supporting it is true
on its grounds and must be completed on its enumeration before it is put.

### R2.1.5 The fallback's form — RULED WELL-FORMED

§7.8's fallback names all four elements my round-1 [P1-2] cure required:
(i) option (a) **taken explicitly rather than by default**; (ii) a **cross-lane
dependency row** on the plan §1 ledger naming R2-F's relocation package as the
blocker; (iii) E4 re-scheduled after it; (iv) E7 acceptance closing the rows as
**re-owned with a named carrier** rather than DISCHARGED — with "what must not
happen is the status quo" stated in terms. **Sustained**, with two amendments
forced by the findings above: the blocked set is **O1 + O2** (O3 re-owned, not
blocked — [P2-R2]), and **O1's close must additionally carry §10 row `:280`**
([P2-R1]), which no option discharges.

---

## R2.2 Appendix C State list — item by item

### Item 1 — §7.2 / §7.3 restatements, line by line: **VERIFIED EXACT. No second labeling error.**

Checked every cite against `GwzM5-8R2DSettledTuple.md:634-828`:

| claim | verified |
| --- | --- |
| §11 `:634`; §11.1 `:636-775`; §11.2 `:776-797`; §11.3 `:798-828` | **all four boundaries exact** (`:775` blank, `:776` = "### 11.2 Pinned to R2-F") |
| base table `:638-645`, six rows | exact (`:638` header, `:639` separator, `:640-645` six rows) |
| addition-block headers `:647`, `:694`, `:752` | all three exact |
| block-1 sub-cites a-f: `:649-652`, `:653-658`, `:659-662`, `:663-665`, `:666-686`, `:687-692` | **all six exact** |
| block-2 sub-cites g-i: `:696-727`, `:728-736`, `:737-750` | **all three exact** |
| block-3 sub-cites j-k: `:754-759`, `:760-774`; §11.2's T-5 row `:778-786` | **all exact** |
| §7.3's eight item ranges `:800-802`, `:803-808`, `:809-811`, `:812-813`, `:814-815`, `:816-818`, `:819-825`, `:826-828` | **all eight exact** |

**Content spot-checks, all correct**, and one better than required: §7.3 item 3
disposes `runtime.*` as "ALREADY RULED, not R2-E's… frozen text at freeze
`:434-479`". I opened it — freeze `:434-436` is the "`runtime.*` evidence
restatement (2026-08-23, ruled by the 5.2 settled dual's State axis; adjudicates
the open item Step 5.1 filed at `GwzM5-8R2DSettledTuple.md` **§4.6/§11.3(3)**)".
The frozen annotation names §11.3 item 3 **by number**. The disposition is not
merely defensible; it is corroborated by the ruling's own text.

Also verified: §7.2's base-table row 5 reproduces the settled tuple's own
pointer — "First production catalog activation | plan §5 item 2; **gated by
§11.3**" (`:644`) — which is the pointer E0.1(b) followed into the wrong
section. Naming it there is the right place to break the chain.

### Item 4 — the census clause: **CURED in substance; [P3-R1] on form**

The three record headers and T-D's census sentence gain "**no key retired**",
and §4's header gains "**`terminal.*` stays 0/11 until E3.2 lands**". Grounded
on the house form I cited in round 1 (freeze `:476`; `:702`). **§8 row 12 is
then true as written.** Sustained.

**[P3-R1]** — the cure is *stated*, not *written verbatim*, unlike every other
frozen-text edit in the addendum (§6 gives §8 row 11 verbatim; §3.3 gives §8
row 6; §7.5 gives item 0; the [P2-5] cure gives item 6). The three activation
records are the strings that actually enter freeze §3.5 and are the package's
primary deliverable; leaving their final header text to reconstruction is
against the pair's own quote-preserving discipline. **Write the three headers
out in full before filing.**

### Item 5 — the count arithmetic including T-C′: **VERIFIED**

`FAULT_INJECTION_SOURCES` (`fault_expected_keys.rs:419`+) declares **nine**
files today, re-counted for this round. T-C′ splits `terminal.*` across
`namespace_mutation.rs` (keys #1-#5) and `admission_mutation.rs` (keys #6-#10)
— **both already declared** (`:449-452`, `:432-435`), verified. So:

| step | new declared file | count after |
| --- | --- | --- |
| today | — | **9** |
| E1 (C-2 → `namespace_mutation.rs`) | no | **9** |
| E2 (B-1 → `barrier_mutation.rs`) | **yes, one** | **10** |
| E3 (T-C′ → two already-declared files) | no | **10** |

**9 → 9 → 10 → 10. One move, at E2.** §6's table and its three corrections
(strike E1.2's duty; §8 row 11 verbatim; §9 item 1's trace) are exactly the
cure my round-1 [P1-1] specified. **Sustained.**

### Item 6 — the O8 denominators: **VERIFIED against both sources**

- **`GwzM5-8R4bG-Evidence.md:1473`** reads, on that exact line: "**18 UNBOUND
  rows remain** (10 progress + 8 archive)". The cite is line-exact.
- **§12.4 Table B** (`:1194`+, "Table B — the 10 archive shapes") holds **ten**
  rows: `AC-CANDIDATE`, `AC-NOPUB-BORN`, `AC-NOPUB-UNBORN`, `AA-PREACCEPTANCE`,
  `AA-CANDIDATE-COMPLETE`, `AA-CANDIDATE-PARTIAL`, `AP-PRESERVED`,
  `AL-OPTIONAL-MISSING`, `AL-UNKNOWN`, `AR-C` — and **exactly two** carry
  "none" in the Fixture column: `AC-NOPUB-UNBORN` (`:1200`) and `AP-PRESERVED`
  (`:1204`).

**10 archive = 8 fixtured/UNBOUND + 2 NO-FIXTURE**, independently confirmed
from Table B rather than inferred. So **10 progress + 8 archive = 18**;
**+2 PENDING-FIXTURE → R2-F**; **+2 non-archive NO-FIXTURE (`B-NOT-STARTED`,
`B-PREPARING-EMPTY`) not R2-E's at all**. The correction's form matches plan
§0's 67→38 precedent, and the §12.4 pointer my [P3-8] asked for is added.
**Sustained in full.**

---

## R2.3 Round-1 findings — disposition audit

### The two [P1]s

**[P1-1] file count — CURED.** Verified at R2.2 item 5. Both wrong statements
corrected, the false E1.2 commit duty struck, the replacement annotation given
verbatim, the aggregate trace added to §9 item 1.

**[P1-2] the A1 coexistence gate — CURED, and the chain is repaired at all
three hops.** I opened both corrected sources:

1. **`GwzM5-8R2E-E01ReachTraces.md` §(b)** now opens with a dated correction
   block naming the defect ("the section extraction matched the first '11.3'
   occurrence without verifying the boundary"), the real §11.3's eight items,
   **item 1 as the A1 coexistence gate**, the **R2-F pin (§11.2:791)**, and the
   consequence ("E4.1 is therefore gated on R2-F's relocation execution (or an
   explicit re-sequencing decision)"), while leaving the five rows in place as
   §11.1 content with their consumptions intact. **This says what my finding
   required, and it preserves the original text rather than rewriting it.**
2. **`GwzM5-8R2E-Plan.md` E4.1** carries a correction naming §11.3 item 1, the
   gate, and the R2-F pin.
3. **§7.5** gives precondition **item 0** verbatim, above the five, with the
   mechanism (not just the rule): a catalog activated over resident legacy
   residue is refused by `interior::exact_row`, and on Windows unconditionally.

**Sustained.** The gate is now impossible to miss at E4.1.

### The six [P2]s

| round-1 finding | disposition | my ruling |
| --- | --- | --- |
| **[P2-1]** §7 mis-titled; §11.3's eight unaccounted; item 2 dropped | §7.1 retitle (verbatim), §7.3 eight-row close, §7.7 item 2 consumed | **CURED, and §7.7 exceeds the cure.** It gives item 2 an owner (**E4.2**, in-commit) *and* lands an earlier duty on **E1.2/E2.3/E3.2** — each family matrix must state which Git-directory route it takes, route (a) the Step-2.3 door or route (b) the managed prefix under the target's own retained root. That is the warning I said the E-steps would otherwise inherit unwarned. Sustained. |
| **[P2-2]** O8 denominators | §8's [P2-2] cure | **CURED**, verified R2.2 item 6 |
| **[P2-3]** T-D's ground | re-grounded; "one boundary must have one name" **DELETED** | **CURED — and the re-grounding SURVIVES.** See below. |
| **[P2-4]** §8 row 12 unsupported | census clause completed | **CURED**, [P3-R1] on form |
| **[P2-5]** §5.3's missing ordering condition | item 6, verbatim | **CURED.** The preamble correctly becomes "**all seven**" (item 0 + the original five + item 6), and the cure cites the two facts I gave (`checked.rs:127-134`'s prologue lease; `archive.rs:97`'s pre-gate acquisition). Sustained. |
| **[P2-6]** §1.1's fixture cite | three ranges | **CURED**: `:105-120` (barrier 16), `:151-161` (cleanup 11), `:162-172` (terminal 11) — re-verified this round. |

### T-D — ruled: the re-grounding SURVIVES the deletion

The question is whether reading (a) alone carries a permanent siteless
determination once "one boundary must have one name" is gone. It does, on the
Phase-3 determination's own two-part structure:

1. **The key names a state, not a durable edge.** `RetainedWriteAuthorityV1` is
   "**Deliberately neither `Copy` nor `Clone`**" (`coordinator/execution.rs:321-327`,
   re-verified), so release is a move-out — restart-invisible. That is freeze
   `:697-698`'s test met exactly: "no create, write, flush, publish, retire, or
   reobserve occurs".
2. **Every edge-bearing key of the family is executed** — 10 of 11, all naming
   edges.

The one residue the deletion exposes is that demoting reading (b) is now an
**interpretive** claim ("the key does not name that edge"). **Frozen text
closes it, and the addendum does not cite it:** the `terminal.*` non-activation
record's own scope sentence, freeze `:795-797` — "**Every key of the family
names the admitted action directory's terminal retirement into the catalog's
retired root**". The admission record's return to `Idle` is the catalog's
re-admission capacity, not the action directory's retirement, so reading (b) is
outside the family's frozen scope by the freeze's own words.

**Ruling: T-D survives on reading (a). [P3-R2] — add the freeze `:795-797` cite,
which converts the demotion from interpretation into frozen text.**

I also confirm the addendum's Code-side correction (i) to T-D's evidence:
`admission_mutation.rs:296` and `:307` are **`AdmissionFinalPublish`** and
**`AdmissionFinalReobserve`** — a different edge — verified by grep, so the
E0.2 draft's citation of them as idle-key hits was wrong and the strike is
right. **[P3-R3]:** my own grep locates the five idle keys' *references* at
`:359-361` and `:381-382` (the key lists), which is a third set of numbers
again; whichever the lane files should say whether it means the `hit()` call
site or the key reference.

### The drift-note appendix — READY TO FILE, sustained

Appendix A reproduces my §6.1 adjudication accurately and I re-verified each
element: the mechanism (`4b9f078`, 2026-08-25 02:13:43 +1000, **+19** on that
file and no other — one blank at `:82` plus an **18-line** blockquote at
`:83-100`; frozen §4 ends `:81`; old `:83` → new `:102`); the extent statement
(**+19 below the annotation, ZERO for `:1-82`**, explicitly not a whole-file
offset); the corrected numbers (`:136-144`, `:178-184`); the **six-document**
stale-cite list, including `SettledTuple:702`/`:706` on R2-E's own inherited
register; the content-anchored citing rule; and the "do not silently re-point
dated records" clause. **File it as written.**

---

## R2.4 THE CENSUS — re-counted over the amended pair. CLEAN.

Re-parsed both documents' key tables and set-differenced against
`fault_v1.rs`:

| | E0.2 draft | E0.2b addendum | union |
| --- | --- | --- | --- |
| keys named in tables | **38** (cleanup 11, barrier 16, terminal 11) | **8** (barrier #6-#13, §1.5's rewrite) | **38 distinct** |
| minted (named ∉ vocabulary) | **0** | **0** | **0** |
| ordinal conflicts across the pair | — | — | **0** — every addendum row reuses its E0.2 ordinal |
| omitted (vocabulary ∌ named) | **0** | — | **0** |

- Vocabulary total re-counted: **165** (`fault_v1.rs`), matching
  `EXPECTED_KEY_COUNT` at `:174`.
- 11 + 16 + 11 = **38**; 165 − 38 = **127** other-family keys untouched.
- All three rows still `Reserved` (`fault_expected_keys.rs:382`, `:387`, `:392`).
- **Zero retired**, now stated in the records themselves (R2.2 item 4).

**The addendum's four new/replacement decisions move no census:**

| decision | artefact | census |
| --- | --- | --- |
| **B-5** | a freshly created 22-byte file at the reserved leaf | a file, not a key — **0** |
| **B-6** | uniform cross-platform alias creation | **0** |
| **B-4** | #10 / #12 as two boundaries | both already in the vocabulary at their E0.2 ordinals — **0** |
| **T-B′** | a new `DestinationRecheckV1` variant | lives in `publication.rs`, **not** `fault_v1.rs` — **0** |
| **T-C′** | terminal sites split across two already-declared files | **0**, and no file-count move (R2.2 item 5) |

### T-B′ — mints no census key, and Class 1 is compatible with my round-1 adjudication

- **No key.** `DestinationRecheckV1` is a publication-owner type
  (`publication.rs:41-47` in the freeze's snapshot), not a
  `CheckedArtifactFaultKeyV1`. This is the identical shape to E0.2's DECISION
  B-3 (a third `DirentBarrierClass`), which I sustained in round 1 on exactly
  this ground.
- **Class 1 grounding verified verbatim.** Freeze `:1362-1364`:
  "`DestinationRecheckV1` (`:41-47`) **is** an enum (`None` |
  `PreRetirementFinal { … }`), so its arm genuinely is a new variant."
  T-B′ uses the freeze's **enum** bullet where E0.2's T-B used the **struct**
  bullet (`:1358-1361`, about `DirectoryInteriorRecheckV1`). The correction is
  the freeze's own sentence applied to the right type.
- **Compatible with round 1.** I made no finding T-B′ contradicts: I verified
  §8 row 6's target (`:1382`) and the Class 1 criterion (`:1356-1364`) were
  real, and graded [P3-3] that **T-A** was contingent on OPEN-T1 — separately
  cured (Appendix B now marks it "contingent on OPEN-T1"). §3.3's replacement
  text for §8 row 6 keeps the row's target and changes only its content, which
  is the sanctioned mechanism.
- **The class attribution is stated and correct:** T-B′ is **Class 1** (the
  recheck-arm class); T1's widening is **Class 2**. They are adjacent — T-B′'s
  catalog half reads through T1's widened predicate — and E3.1 owes both,
  separately justified. Saying so is right.

### Also noted

**[P3-R4]** §1.5's preamble says rows "#1-#5, **#9**, #14, #15, #16 are
unchanged", but the table restates **#9** with its semantic altered — the E0.2
row required "its identity **and the anchor's residency**"; §1.5's row keeps
the identity check and drops the residency clause (correctly, since under B-5
the home anchor never travels). Eight rows are restated, not seven. Align the
preamble with the table.

---

## R2.5 Remediation list for round 2

**Blocking §7.6-§7.8 only:**

1. **[P2-R1]** Add §10 row `:280` to §7.6's table with its door analysis; add it
   to plan O1's enumeration on the record; give its catalog-free "no legacy raw
   writer" half an owner (a pin in an E4 step, or a dated accepted-residual
   record). O1 cannot close without it.
2. **[P2-R2]** Correct §7.6's O3 row; restate §7.8(a)'s cost as **O1 + O2
   blocked, O3 re-owned**; promote §7.4's single-owner finding
   (`policy.rs:33-42` → `observation.rs:93`) into ground 4, where it is the fact
   that makes the circularity breakable.

**Non-blocking, fold in before filing:**

3. **[P3-R1]** Write the three activation-record headers out verbatim with the
   completed census clause — they are the strings that enter freeze §3.5.
4. **[P3-R2]** Add freeze `:795-797` to T-D, closing the demotion of reading (b)
   with frozen text.
5. **[P3-R3]** Say whether T-D's replacement cites name `hit()` call sites or
   key references.
6. **[P3-R4]** Align §1.5's preamble with its own eight-row table on #9.

---

**ROUND 2 VERDICT: CONDITIONAL GO — GO on the semantics object (both [P1]s
cured, all six [P2]s dispositioned, E1-E3 unblocked); NO-GO on §7.6-§7.8 as
operator decision-support on [P2-R1] and [P2-R2].**

**PREMISE RULING: TRUE but NOT COMPLETE.** The catalog-dependency claim holds
for all eight §10 rows §7.6 maps — I attacked it four ways and broke none — so
option (c) collapses as a partition of the substantive conversions. But the §10
table has **nine** rows and §7.6 maps **eight**, and the catalog-free set is
**not empty**: it contains row `:280`'s "no legacy raw writer" half (gate
already fired, currently unmet in production) and O3's relocation-discharge
route (`private_parent()`, one owner, one consumer). Both omissions
**strengthen** ground 4 and option (b); neither reverses the recommendation.
Ground 4 holds and is the ground that turns preference into structure; all five
grounds verified sound; the fallback's form is well-formed subject to the two
corrections above.

**CENSUS RULING: CLEAN over the amended pair.** 165 total, **38** accounted
(11 + 16 + 11), **zero minted**, **zero retired** — and the "no key retired"
clause is now carried by the records themselves rather than only asserted about
them. The addendum's eight restated barrier rows reuse their E0.2 ordinals with
no conflict; B-5, B-6, B-4, T-B′ and T-C′ each move no census; T-B′ mints a
`publication.rs` enum variant, not a fault key, and its Class 1 grounding is
verbatim-correct at freeze `:1362-1364` and compatible with everything I ruled
in round 1. The declared-source count is **9 → 9 → 10 → 10**, one move, at E2,
T-C′ included.

*Filed verbatim. State axis, R2-E E0.3 dual #1 round 2, 2026-08-27.*
