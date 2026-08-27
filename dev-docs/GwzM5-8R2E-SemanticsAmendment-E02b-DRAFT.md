# R2-E E0.2b — semantics amendment addendum (DRAFT, 2026-08-27)

Status: **LANDED 2026-08-27 at the E0 close — CONTROLLING over the E0.2 base
draft.** The operator's §7.8 sequencing ruling: **option (a)** (one-line reply
"a", 2026-08-27) — relocation stays R2-F's; the fallback's four elements and
O13's minting are executed at `GwzM5-8R2E-Plan.md` §1.1; the freeze application
(three §3.5 records, eleven annotations) landed the same day. *(Was: FINAL
DRAFT — round-2 conditions folded, for the E0 landing.)* Author:
the implementation lane (E0.2 drafter). Object: the E0.2 amendment
(`GwzM5-8R2E-SemanticsAmendment-DRAFT.md`), amended here rather than rewritten.
The pair — E0.2 + this addendum, **addendum controlling** — is the E0 deliverable.

**Ruling sources consumed in full.** Round 1: `GwzM5-8R2E-E03-ReviewCode.md`
(NO-GO, 3×P1 / 8×P2 / 12×P3) and `GwzM5-8R2E-E03-ReviewState.md` (NO-GO,
2×P1 / 6×P2 / 8×P3). **Round 2:** the same two files' appended re-verdicts —
Code **GO conditional on [P2-R1]** (+4 P3), with the addendum's macOS override
**independently re-derived and RATIFIED** (`ReviewCode.md` §R1, which found it
"strictly worse than the defect it cures" and withdrew the hard-link
sub-shape); State **CONDITIONAL GO** — GO on the semantics object with E1-E3
unblocked and the census re-counted clean, **NO-GO on §7.6-§7.8 as operator
decision-support** on [P2-R1] and [P2-R2] (+4 P3), with §7.2/§7.3's restatements
verified **line-exact** and the premise ruled **"TRUE for what it maps, NOT
COMPLETE"**. Plus the corrected `GwzM5-8R2E-E01ReachTraces.md` (whose (b) now
carries the §11.3/§11.1 correction block this addendum discharges).

**Both round-2 condition sets are applied in this file.** Appendix C maps every
one to the section that carries it. Nothing else changed.

**Discipline:** the E0.2 draft's own house rules stand — quote-preserving,
every code claim cited at a `file:line` opened and read for **this** document,
every decision stating its grounds and its rejected alternative, every
undecidable question written as an OPEN row with its settling evidence. Nothing
outside this file was written; no source was edited; no git operation was run.

**One structural note before the rulings.** Both axes converged blind on
OPEN-B1 and OPEN-T1, and the Code axis carried them one level deeper than the
E0.2 draft did: the failure is not "no process can retain the catalog" but the
**recovery classifier's `Ambiguous` arm, which has no recovery path**. Verified
independently for this addendum:

```
capability/pre_catalog/provider/interior.rs:173-179   Final arm:
        completed_record(..).is_some() ? ExactOwned : Other
catalog/classifier.rs:266-273  the only tuple yielding
        CatalogBootstrapRecoveryDecisionV1::Complete requires ExactOwned
catalog/classifier.rs:274      _ => Ambiguous
catalog/bootstrap.rs:283-288   Ambiguous => Err("aggregate catalog facts are ambiguous")
```

Every cure below is measured against that consequence: **a wrong reading here
bricks a user's catalog permanently, with no in-code exit.** That standard is
what forces §1's new refusal and §7's sequencing analysis.

---

## 0. Ruling map — what this addendum changes in the E0.2 draft

| # | Item | E0.2 text affected | Disposition here |
| --- | --- | --- | --- |
| 1 | **B1 cure** — candidate (i) REFUSED; adopt copy-not-move | §3.4 rows #6-#13, §3.5 OPEN-B1, Appendix A/B | **§1** — rewritten. Plus **one new [P1]-grade refusal**: the Code ruling's *hard-link sub-shape* is macOS-fatal; the shape adopted is **fresh copy**. |
| 2 | **T1 cure** — Class-2 widening AUTHORIZED, three gates, `census.retired` struck | §4.4 OPEN-T1 | **§2** — rewritten. **B1 and T1 are ruled separately.** |
| 3 | **T-B re-resolved** — field generalization unimplementable | §4.2 DECISION T-B, §4.3 row #7, §8 row 6 | **§3** — replaced by DECISION **T-B′** (a new `DestinationRecheckV1` variant carrying a second observation input; Class **1**, not Class 2). |
| 4 | **OPEN-C1 CLOSED** — `RecordScratch` has zero write paths | §2.2 DECISION C-3, §2.3 row #1, Appendix A | **§4** — simplified; the E1 proof obligation struck. |
| 5 | **O6 COMPLETED** — precedent part (ii) missing | §3.2, §3.4 row #5 | **§5** — read-side identity refusal added. |
| 6 | **The three machine-enforced inventories** — injection sources nine → ten; the publication caller registry; the protected source-tree digests | §2.5, §3.6, §4.5, §5.2, §8 row 11 | **§6** — §6.1 corrects the first; §6.2 adds the two Code round-2 [P2-R1] named; **§6.3 rewrites all three convergence obligations verbatim**; §6.4 banks a fifth ground for §5.2. |
| 7 | **§11.1 / §11.3 restatement + A1 coexistence gate + the NINE-row E4 map + sequencing** | §5.3, §7 title, §9 item 4 | **§7** — full verified restatement of both sections; the gate added; **§7.6 is nine rows (row `:280` added, O13 proposed); §7.6.2 corrects O3's row**; sequencing proposed with the accounting corrected. |
| 8 | Both axes' remaining P2/P3, rounds 1 **and 2** | throughout | **§8** — dispositioned line by line, four tables. |

---

## 1. THE B1 CURE — copy-not-move, in the fresh-copy shape

### 1.1 Candidate (i) is REFUSED, on the Code axis's grounds, re-verified here

The E0.2 draft recommended candidate (i) — widen `completed_record` to accept
the anchor's absence paired with a resident `BarrierIntentActive(ordinal)`
reconciliation record. **It is not implementable.** Three independent code
facts, each re-verified for this addendum:

1. **`completed_record` cannot see an action-directory record.** Its input is
   `RawCatalogInteriorObservationV1`, whose action rows are
   `action_rows: Vec<ActionDigestV1>` — bare digests, no interiors
   (`capability/pre_catalog/provider.rs:196`, with the deliberate design note at
   `:190-195`). A `BarrierIntentActive(ordinal)` record lives *inside* an action
   directory; reaching it would make a predicate that runs on every catalog
   recovery **and inside every publication acquisition window**
   (`publication.rs:224`, `:239`) descend into up to 64 action directories
   (`bounds.rs:1`) of up to 261 slots each (`bounds.rs:16`, asserted `== 261` at
   `:70`).
2. **The intent record does not carry the fact the predicate needs.**
   `BarrierIntentV1`'s eleven bound fields (`protocol/barrier.rs:19-32`) are the
   four reservation digests, the ordinal, `catalog_anchor_identity`,
   `private_home_parent_identity`, `private_home_name`,
   `target_parent_identity`, `target_path_profile`, `reserved_target_leaf` and
   the derived `intent_id`. **There is no roaming-anchor identity field**, and
   `InfrastructureRecordV1::owner_issue_for_catalog` requires one
   (`protocol/infrastructure_record.rs:86-97`, consumed at `interior.rs:352-359`).
   The only other durable source is the `RetiredActionsDescriptor` bytes
   themselves, which would convert `completed_record` from *derive-and-compare*
   into *decode-and-trust*.
3. **Any move-based scheme is fatal at key #12 independently of (i).** Once the
   anchor object is retired into an action directory, restoring the home row
   needs a **new** file — a new inode, hence a new `DurableObjectIdentityV1`.
   The `RetiredActionsDescriptor` and `CatalogFormat` rows bind the *old*
   identity (`interior.rs:352-362`); no descriptor-rewrite path exists in a
   completed catalog. So the E0.2 draft's own reconciliation key destroyed the
   catalog **by design**, not only in a crash window.

**The axis conflict, recorded.** The State axis concurred with candidate (i)
(`GwzM5-8R2E-E03-ReviewState.md` §8.3 item 1) on the ground that "the descriptor
consumes the anchor's **durable identity**, a rename preserves the object, so a
moved-and-returned anchor reproduces byte-identical `RetiredActionsDescriptor` /
`CatalogFormat` rows." **That ground is true and insufficient.** It describes
the *returned* state; the defect is the *away window*, in which the row is
absent and `exact_file_identity` returns `None` regardless of what the object's
identity will be when it comes back (`interior.rs:350`, `:797-806`, `:763-784`).
And fact 3 above falsifies it outright for the stranded path, which is a
*designed* state, not a crash. **The Code axis's refusal governs.**

### 1.2 NEW FINDING — the Code ruling's hard-link sub-shape is macOS-fatal. [P1-grade]

The Code axis adopted (ii) "in a specific shape": *"create the target-parent
alias as a hard link off the resident `roaming-anchor-home-v1` object rather
than a byte copy"*, on the ground that "the alias shares the home object's
durable identity (Linux `name_to_handle_at` resolves the inode …; Windows file
IDs are shared across hard links)".

**On macOS it does not, and the tree carries an executed test that says so.**

`gwz-core/src/checked_artifact/platform/anchor/tests.rs:278-302`, the fixture's
own doc comment, verbatim:

> Whether two names for one object carry the same durable identity here.
>
> Linux does (`name_to_handle_at` encodes the inode, which hard links share) and
> so does Windows (`FILE_ID_INFO.FileId` is per file). **macOS does not**:
> `ATTR_CMN_OBJPERMANENTID` is allocated per hard link, and **creating the second
> link also re-homes the first onto an indirect node with a fresh id**. Measured,
> not assumed — the fixture above asks the platform rather than reading a `cfg`.

and its pinning test, `hard_link_identity_sharing_is_what_the_retirement_rows_assume`
(`anchor/tests.rs:308-324`), asserts `!shared` on macOS. The macOS provider does
use that attribute: `commonattr: libc::ATTR_CMN_OBJPERMANENTID`
(`capability/pre_catalog/provider/platform/macos.rs:88`). The same platform fact
is already recorded in frozen text, at freeze `:840-848`.

**The consequence, traced:** hard-linking `roaming-anchor-home-v1` on macOS
re-homes the **home** object onto a fresh id. Then, at the very next
observation:

- `completed_record` `:350` re-derives the roaming anchor's identity, feeds it
  into `owner_issue_for_catalog` (`:352-359`) and compares the canonical bytes
  against the resident `RetiredActionsDescriptor` and `CatalogFormat` rows
  (`:360-362`). Derived ≠ resident ⇒ `None` ⇒ `Ambiguous` ⇒ permanent `Err`.
- Independently, `require_named_file_identity`
  (`completed.rs:358-376`, called for `RoamingAnchorHome` in the loop at
  `:227-244`) compares the fresh row identity against the identity retained at
  `:100-104`. Same failure, on every `revalidate`.

So the hard-link shape converts B1 from *a crash-window defect on all platforms*
into *a deterministic first-barrier catastrophe on macOS* — strictly worse than
the defect it cures. It cannot be adopted.

**The Code axis's essential ruling survives intact**: what made hard-link
attractive was that `completed_record`, `retain_file` and
`require_named_file_identity` stay **untouched**. That property belongs to
**copy-not-move as such**, not to the hard-link sub-shape — a freshly created,
independent file in the target parent touches the home object on no platform.

### 1.3 DECISION B-5 — the alias is a fresh, independent copy

> **DECISION B-5 (E0.2b).** The target-parent alias is a **freshly created
> regular file** carrying exactly `ROAMING_ANCHOR_BYTES`
> (`b"GWZ-ROAMING-ANCHOR-V1\n"`, `interior.rs:28`), written through the P2
> family (`durable_write_options` + `sync_all` + parent flush) under the
> schedule-derived `reserved_target_leaf`. It is **not** a hard link and **not**
> a rename of the catalog's own `roaming-anchor-home-v1` row. The catalog's home
> row is never opened for write, never linked, never renamed, and never removed
> by any `barrier.*` edge.
>
> **B1's resolution is therefore: `completed_record` UNCHANGED,
> `retain_file` UNCHANGED, `require_named_file_identity` UNCHANGED. E2.1
> receives no authorization to edit any of them, and needs none.**

**Grounds.** (a) It is the only shape that is safe on all three platforms
(§1.2). (b) It leaves all four B1 gates untouched, which is the property the
Code ruling identified as decisive. (c) The frozen slot budget supports a
per-barrier alias rather than one travelling object: `ActionSlotV1::all()`
pushes **three** slots per ordinal — `BarrierIntentActive`,
`BarrierIntentRetired`, `RetiredRoamingAnchorAlias`
(`protocol/slots.rs:169-175`) — over `MAX_BARRIER_INVOCATIONS_PER_ACTION = 64`
(`bounds.rs:5`, `BARRIER_ACTION_SLOTS = 3 * 64` at `:10-11`). A retirement
destination budgeted for *every* ordinal is the shape of a scheme where every
barrier ends in an alias disposal. *(Counter-reading acknowledged and recorded:
one could budget all 64 because any of them could strand. It is strong evidence,
not proof; it is also the only frozen evidence that speaks to the question.)*

**Rejected alternatives, both stated.**
- *Hard link* — refused on §1.2's platform evidence. Recorded rather than
  silently dropped, because it is the Code axis's own recommendation and a
  future reader must find the reason here.
- *Move (candidate (i) or any rename of the home row)* — refused on §1.1.

**What copy-not-move costs, stated honestly.** The alias no longer shares the
home object's identity, so keys #7 and #11 become **residency-and-bytes**
proofs rather than identity proofs. That loses nothing the frozen record could
have expressed: `BarrierIntentV1` has no alias-identity field
(`protocol/barrier.rs:19-32`), so no restart could ever have re-derived the
alias's identity from durable state, on any shape. The rows below say so
explicitly rather than implying an identity check that is not there.

### 1.4 DECISION B-6 — the alias is created unconditionally, on every platform

The physical barrier the alias exists to enable is Windows-only: off Windows
`private_barrier` is `sync_parent(dir)` for both classes
(`platform.rs:377-384`), and no anchor is involved.

> **DECISION B-6 (E0.2b).** E2.1 creates and disposes of the alias on **every**
> platform, not only Windows.

**Grounds.** A platform-conditional alias would make keys #6, #7, #10, #11, #12
and #13 platform-conditional rows whose non-Windows arms are vacuous — the
E9/E10 "documented no-op" record shape, which the corpus accepts only where a
platform *cannot* provide the property (`platform.rs:526-567`). Here it can;
the alias is a 22-byte file. Uniform creation keeps every key's matrix row a
real process stop across a real durable edge on all three platforms, which is
the matrix's own claim (`admission/tests_fault_matrix.rs`).
**Rejected alternative:** create only under `cfg(windows)`. Rejected because it
buys one file per barrier and pays with six keys' worth of cross-platform
evidence.

**Why B-6's cost is bounded — the line the decision must carry** (Code
[P3-R3]). A reader meets freeze `:778-781` — landing the anchor inside an
admitted action directory "requires teaching admission's `extra_children: 0`
grammar the reserved target leaf, which is the very contamination class the
E10/E14 annotation records as diagnosed and fixed" — and concludes that B-6
reopens that class on every platform. **It does not.** The barrier-time path is
`validate_operation` → `RetainedActionNamespaceV1::revalidate`
(`namespace_mutation.rs:202-230`), which checks binding and durable identity and
**counts no children**. The `extra_children: 0` gate lives solely in the
admission classifier (`protocol/admission/owner.rs:29-38`, `has_exact`), i.e. at
`resume_or_admit` — which is already unreachable after the first published row
and is docketed as Phase 3 settle item 6 (`namespace/tests_fault_matrix.rs:20-35`,
`:27-33`). So the alias adds one child to an already-non-exact directory and
breaks nothing new.

### 1.5 The rewritten §3.4 rows

Rows **#6, #7, #8, #9, #10, #11, #12, #13** are restated below — **eight rows,
not seven** *(State [P3-R4]: the preamble previously said #9 was unchanged; the
table alters it, correctly, by dropping the E0.2 row's "and the anchor's
residency" clause, which under DECISION B-5 names a state that no longer
exists — the home anchor never travels)*. Rows #1-#5, #14, #15 and #16 are
unchanged from the E0.2 draft except where §5 (O6) extends #5. Site file
unchanged: `capability/pre_catalog/provider/barrier_mutation.rs` (`NEW@E2.1`).

| # | Key | **Rewritten semantic** | Function (status) | Note |
| --- | --- | --- | --- | --- |
| 6 | `barrier.anchor_outbound` | the target parent holds, under the reserved leaf, a **freshly created** regular file carrying exactly `ROAMING_ANCHOR_BYTES`, written through and flushed, with the parent flushed — durable and unread. **The catalog's `roaming-anchor-home-v1` row is untouched by this edge.** | `create_target_anchor_alias` (`NEW@E2.1`) | P2 family; precedent `managed_mutation.rs:1088-1115` |
| 7 | `barrier.anchor_outbound_reobserve` | the alias has been re-opened no-follow and proved to be a regular file whose bytes are exactly `ROAMING_ANCHOR_BYTES`. **This is a residency-and-bytes proof, not an identity proof** — the intent binds no alias identity (`protocol/barrier.rs:19-32`) | same | precedent `namespace_mutation.rs:292-302` |
| 8 | `barrier.target_barrier` | `private_barrier` over the **target parent** has returned, with the third `DirentBarrierClass` (E0.2 DECISION B-3), whose Windows arm round-trips the **supplied** alias by its reserved leaf rather than surveying for a resident anchor | `barrier_target_parent` (`NEW@E2.1`) | `platform.rs:363-374`, `:519-567` |
| 9 | `barrier.target_reobserve` | the target parent has been re-observed and its durable identity still equals the `target_parent_identity` the intent bound — **this one is a real identity check**, because that field exists (`protocol/barrier.rs:28`) | same | unchanged from E0.2 |
| 10 | `barrier.anchor_return` | the alias **this drive created** has been retired onto `RetiredRoamingAnchorAlias(ordinal)` in the action directory — durable and unread. "Return" now names the alias's return into the action's own custody, not a rename of the home row | `retire_target_anchor_alias` (`NEW@E2.1`) | E13's shape; post-edge key per DECISION C-1 |
| 11 | `barrier.anchor_return_reobserve` | two-sided: the retired row has been re-read at its ordinal slot **and** the target parent proved to hold nothing under the reserved leaf | same | E11's two-sided absence shape (freeze `:1104`) |
| 12 | `barrier.target_alias_retire` | a **stranded** alias — found at the target by a drive that did **not** create it, i.e. the restart path — has been retired onto `RetiredRoamingAnchorAlias(ordinal)`, durable and unread. **Under copy-not-move this edge is no longer catalog-fatal:** it retires a derived 22-byte object and the home row never moved | same helper, restart entry (`NEW@E2.1`) | freeze `:775-777` assigns this slot to the stranded case explicitly |
| 13 | `barrier.target_alias_reobserve` | the restart path's post-read proof of the retired row | same | |

**DECISION B-4 — #10 and #12 are two boundaries, not one key at two sites.**
They share one helper but differ in durable pre-state: #10 is entered by the
drive holding its own write handle's identity in memory and its intent row at
`BarrierIntentActive(ordinal)`; #12 is entered by a fresh process that cannot
prove the resident alias is its own. A restart distinguishes them from the
intent record alone. This is the E15-install / E15-restart split the corpus
already keys separately (`managed_bootstrap.marker_retire` vs
`component_reobserve`, freeze `:544-551`), not the `staging_directory_flush`
one-key-two-sites case.
**Rejected alternative:** collapse #10 into #12 and give #10 the alias's
*removal*. Rejected because removal is the shape Step 4.2 deliberately replaced
with durable retirement (freeze `:806-812`), and because retirement preserves
evidence where a foreign object may sit at the reserved leaf.
**If E2.1 finds the two indistinguishable in code**, the honest adjustment is
one line — #12 becomes the restart *observation* and #10 the edge — and it
reopens this decision rather than being taken silently.

### 1.6 OPEN-B1, restated in its final form

> **OPEN-B1 — CLOSED at E0.2b.** The defect is real and confirmed by both axes;
> its cure is **DECISION B-5** (fresh-copy alias), which requires **no change to
> `completed_record`, `retain_file`, `require_named_file_identity`, or any other
> catalog-owner predicate**. E2.1 is authorized to build the alias lifecycle and
> is **not** authorized to edit any catalog completion predicate. The row moves
> from OPEN to DECIDED; what remains open in this family is OPEN-B2 (the target
> parent's binding, §8) and OPEN-B3 (the reserved leaf's grammar, §8), neither
> blocking.

---

## 2. THE T1 CURE — the Class-2 widening, authorized, at three gates

**Ruled separately from B1, per the Code axis's package ruling.** The two are
not one defect class:

- **T1 is an observer-reading defect.** `completed_record` reads the retired
  root too narrowly; the remedy is a strictly-widening read. The identity the
  predicate derives — `empty_directory_identity` returns the retired root's own
  durable identity (`interior.rs:808-818`) — is unaffected by a child addition,
  so the descriptor and format bytes stay byte-identical.
- **B1 is not an observer defect at all.** Its correct fix touches no predicate
  (§1.6).

Bundling them would hand E2.1 a standing authorization to widen
`completed_record` for a defect whose fix does not need it. **The E0.2 draft's
§4.4 note asking the dual to rule them as one package is withdrawn.**

### 2.1 The authorization

> **T1 widening — AUTHORIZED (E0.2b, both axes concurring; Code axis RULING T1,
> State axis §8.3 item 1).** The provider's *reading* of the retired root is
> widened; **nothing is minted** — no record, no slot, no name, no fault key.
> This is squarely the shape freeze `:1443-1450` sanctions:
>
> > C-3 is an **observer/slot-grammar extension in the same provider owner** …
> > It is not a Track-W event and needs no new durable record, slot, purpose,
> > action, or phase … What Phase 1 must extend is the *provider's reading* of
> > that vocabulary, not the vocabulary.
>
> Owner: **R2-E E3.1**, as its own precondition step, reviewed with its
> evidence. Census unchanged: 165 total, no key minted, none retired.

### 2.2 THREE gates, not one — the corrected package

The E0.2 draft named only `completed_record`. Two further, independent gates
inside `completed.rs` each require `EmptyDirectory` on their own, and a
`completed_record` widening alone leaves both standing. All three verified for
this addendum:

| # | Gate | Line | Called from | Requires |
| --- | --- | --- | --- | --- |
| 1 | `interior::completed_record`'s `empty_directory_identity(interior, Slot::RetiredActions)?` | `interior.rs:349` (helper `:808-818`; the fact minted only at `:465-474`, reached because `observe` passes `slot == RetiredActions` as `probe_empty_directory` at `:108-113`) | every recovery, and every publication acquisition window (`publication.rs:225`, `:264`) | `RawCatalogInteriorFactV1::EmptyDirectory` |
| 2 | `retain_directory` | `completed.rs:305-331`, the `EmptyDirectory` match at **`:310`** | `completed.rs:105-109` | `EmptyDirectory` at `RetiredActions` |
| 3 | `require_named_directory_identity` | `completed.rs:378-397`, the `EmptyDirectory` match at **`:383`** | `completed.rs:245-249`, on **every** `revalidate` | `EmptyDirectory` at `RetiredActions` |

**A fourth surface E3.1 must preserve deliberately, not widen:**
`interior::staging_plan` matches `EmptyDirectory` for `RetiredActions` too
(`interior.rs:216-237`). It should keep refusing a populated retired root
*during bootstrap* — a catalog being built must not find action rows already
retired into it. If E3.1 reshapes the fact type, that arm must be preserved by
name.

**Not a gate, and worth saying so:** `revalidate`'s handle-identity loops at
`completed.rs:204-217` (files) and `:218-225` (the retired directory) `fstat`
the **retained handles**, which survive a child addition. They do not gate.

### 2.3 The `census.retired` evidence is STRUCK

The E0.2 draft offered, as supporting evidence, "the catalog-root census already
carries a `retired` counter (`protocol/admission.rs:438`, charged at `:449`)".
**That is a misreading and is withdrawn.** `CatalogRootRowCensusV1::retired`
counts `CatalogRootRowClassV1::Retired(slot)` rows, a class covering exactly two
names — `InfrastructureSlotV1::RetiredActions` and
`InfrastructureSlotV1::CatalogBootstrapRetired` (`protocol/slots.rs:475-478`) —
and it is charged over the **catalog root's** children (`interior.rs:82-83`).
The retired root's own children are never enumerated by it; in a completed
catalog `census.retired == 2`, a constant.

**The evidence that carries T1, restated without it:**

- `protocol/bounds.rs:2` — `MAX_RETIRED_ACTION_DIRS: usize = 64`.
- `:62-67` — `RETIRED_ROOT_BUDGET_V1 = CatalogBudgetV1::new(64, 64*255, 64*256)`,
  with **no production consumer**. A scan budget for 64 entries in a root that
  must hold zero is incoherent.
- `CatalogOccupancyV1::validate` `:118-142` — `RetiredLimitExceeded` at
  `:126-128`, and the **retirement-credit** rule at `:132-136`
  (`retired_action_dirs + outstanding > 64`), whose only purpose is to reserve
  retired-root room for actions that will later retire into it. `can_admit_new`
  (`:144-148`) and `can_resume` (`:150-155`) are both arithmetic over
  `retired_action_dirs`.
- **The R2-D author's own note, now quoted in full** (the E0.2 draft truncated
  it before its last clause, which is the clause that matters).
  `admission/driver.rs:100-110`:

  > Owed to Phase 4: `CatalogOccupancyV1::can_admit_new` (`protocol/bounds.rs`)
  > additionally charges the retirement-credit inequality against the retired
  > root, whose bounded count this observation does not carry. Wiring it belongs
  > to the phase that lands retirement; **unlike the active bound, exhausting
  > retirement credit cannot make the catalog unobservable.**

  That last clause is the belief T1 refutes: on this tree a **single** retired
  child makes the catalog unobservable — and unrecoverable, per §0's classifier
  chain. Quoting it in full strengthens the row rather than weakening it.
- Today `RetiredActions` is only ever *created* empty
  (`directory_mutation.rs:108-109`, the single `StagingPlanV1::CreateRetiredActions`
  arm) and never populated, so T1 is a **forward** defect: no shipped catalog is
  broken by it, and E3.1 is the first step that could break one.

### 2.4 What E3.1 owes, in one list

1. The three gates above, widened together, plus `staging_plan`'s arm preserved
   by name.
2. A **bounded** reading of the retired root, accepting **only** children that
   classify `RootEntryNameV1::ActiveAction` — i.e. the observation's `rows` must
   be **empty**, because an infrastructure-slot name planted in the retired root
   would otherwise be classified into `rows` rather than refused
   (`interior.rs:99-115`) — with the count checked **explicitly against
   `MAX_RETIRED_ACTION_DIRS`** (`bounds.rs:2`) and **not** inherited from
   `interior::observe`'s own caps (Code [P3-R1]; the constants are enumerated at
   §3.2). DECISION T-A supplies the name and both axes sustain it —
   `RootEntryNameV1::parse` classifies `action-<hex>-v1` as `Valid` at
   `slots.rs:444`, parent-independently.
3. The first production caller of `CatalogOccupancyV1`, discharging
   `admission/driver.rs:104-110`'s note.
4. Its own rows: a retired-root-populated catalog recovers, revalidates and
   publishes; a retired root holding any non-`ActiveAction` child still refuses.
5. Census statement: 165, no key minted, none retired.

---

## 3. T-B RE-RESOLVED — DECISION T-B′

### 3.1 Why the field generalization is unimplementable

The E0.2 draft resolved freeze §4.4's last open arm-table row (`:1382`) to "no
new variant — one field generalization of `AdmissionCatalogInterior`'s `absent`
field". The code refutes it, verified for this addendum at
`publication.rs:234-275`:

```
DestinationRecheckV1::AdmissionCatalogInterior { durable_identity, expected, absent } => {
    let fresh = interior::observe(destination_dir, &HostPlatform)?;      // :239  ONE observation
    let (occupied, full) = match absent { … }                            // :240-248
    if full { … }                                                        // :257-262
    if occupied
        || interior::completed_record(durable_identity, &fresh, expected).is_none()   // :264
        || !matches!(interior::retired_record(&fresh), … )               // :265-268
```

A **single** observation of `destination_dir` serves both the `absent` check and
the completed-catalog proof. Both existing producers pass the catalog root as
`destination_dir` (`admission_mutation.rs:177-189`, `:276-294`). A terminal
retirement's `destination_dir` is the **retired root**, and
`interior::observe` of the retired root can never satisfy `completed_record` —
it holds no `CatalogFormat`, `CatalogAnchorA`, `RoamingAnchorHome`,
`RetiredActions` or `RetiredActionsDescriptor` row, not before T1's widening and
not after it. Moving `absent`'s parent does not help: the **observation input**
is what must differ.

Nor can the destination be a two-component leaf: `rename_open_source` takes
`destination: &OsStr` (`platform.rs:86-93`, `:106-111`) and the whole owner is
single-component no-follow throughout.

### 3.2 DECISION T-B′ — a new variant carrying a second observation input

> **DECISION T-B′ (E0.2b), replacing DECISION T-B.** Freeze §4.4's arm-table row
> "any further retirement-destination arm | E7's Phase-4 half and the terminal
> retirement edges | **Phase 4**" (`:1382`) resolves to: **a new
> `DestinationRecheckV1` variant carrying a second observation input** — the
> catalog root's retained handle alongside the retired root that arrives as
> `destination_dir`. Owner: **R2-E E3.1**.
>
> Shape: `DestinationRecheckV1::TerminalRetiredRoot { catalog_root: &Dir,
> catalog_identity: &DurableObjectIdentityV1, expected: &CatalogBootstrapRecordV1,
> absent_child: RootEntryNameV1 }`, whose arm (a) observes `destination_dir` —
> the retired root — proving the named child row **free**, every resident child
> a `RootEntryNameV1::ActiveAction` row (i.e. the observation's `rows` are
> **empty**, so an infrastructure-slot name planted in the retired root refuses
> rather than being classified into `rows`), and the count **within
> `MAX_RETIRED_ACTION_DIRS`, checked explicitly**; and (b) observes
> `catalog_root`, proving `completed_record` still holds under T1's widened
> reading — both **inside the acquisition window**.

**Grounds.**
- It is **Class 1's own sanctioned shape for an enum.** Freeze `:1362-1364`
  draws the criterion explicitly: "`DestinationRecheckV1` … **is** an enum …,
  so its arm genuinely is a new variant." The E0.2 draft applied the *struct*
  half of the criterion to an enum. This corrects that, using the freeze's own
  sentence.
- Carrying a `&Dir` inside the variant is not a capability escape: the type is
  `pub(super)` inside the provider owner (`publication.rs:79`),
  `publish_verified_no_replace` already takes two `&Dir` arguments
  (`:132-140`), and the recheck arms are documented as
  "lifetime-parameterized reference holder[s] with no encode path … built by
  the mutation owners, consumed read-only inside `publish_verified_no_replace`,
  and dropped" (`publication.rs:50-53`).
- The retired-root half can reuse `interior::observe`: `exact_row` classifies
  `action-<hex>-v1` children as `ActiveAction` and returns them in `action_rows`
  (`interior.rs:84-91`, `:410-413`), and refuses every other child outright
  (`:414-417`).
  **But the arm must NOT inherit that reader's bound** (Code [P3-R1], accepted;
  the E0.2b round-1 text cited the wrong constant twice). Reused,
  `interior::observe`'s effective caps are `MAX_INTERIOR_ENTRIES = MAX_ROOT_ENTRIES
  = 74` (`interior.rs:40`, enforced `:79-81`), `MAX_ACTIVE_ACTION_DIRS = 64`
  on the action rows (`interior.rs:85-87`; `bounds.rs:1`), and the name budget's
  `MAX_CATALOG_PARENT_ENTRIES_V1 = 4_096` (`catalog/enumeration.rs:8`) — **none
  of them `MAX_RETIRED_ACTION_DIRS`, and none of them
  `RETIRED_ROOT_BUDGET_V1`'s `(64, 16_320, 16_384)`** (`bounds.rs:62-67`). The
  reused reader is numerically safe today **only because `bounds.rs:1` and `:2`
  are both 64**, which silently couples the retired-root bound to the active
  one. **E3.1 owes an explicit `MAX_RETIRED_ACTION_DIRS` count check in the arm,
  and a stated sentence that the arm does not inherit its bound from
  `interior::observe`** — otherwise a future edit to either constant decouples
  them without any check failing.

**Rejected alternatives.**
- *Move the recheck out of `publish_verified_no_replace`* — refused: the
  re-proof would leave the acquisition window, which is the entire purpose of
  the recheck class (`publication.rs:32-37`).
- *Two-component destination* — refused on `rename_open_source`'s signature.
- *Field generalization* — refused on §3.1.

**Class attribution, stated because the coordinator's brief asked where it
lands:** T-B′ is **Class 1** (the recheck-arm class), not Class 2. T1's widening
is Class 2. They are adjacent — T-B′'s catalog half *reads* through T1's widened
predicate — but they are different sanctions and E3.1 owes both, separately
justified.

### 3.3 Consequential edits

- **§4.2 DECISION T-B** — replaced by T-B′ above.
- **§4.3 row #7** (`terminal.action_directory_retire`) — its anchor becomes
  "the P1 no-replace rename of the whole action directory into the retired root
  with the **new** `TerminalRetiredRoot` destination recheck (DECISION T-B′)".
- **§8 row 6** — the annotation content becomes: *"resolves to **a new
  `DestinationRecheckV1` variant carrying a second observation input**, owned by
  R2-E E3.1, on Class 1's own struct-vs-enum criterion (freeze `:1362-1364`);
  the field-generalization reading is refuted by `publication.rs:239`'s single
  observation of `destination_dir`."*
- **Appendix B** — the T-B row is replaced by T-B′ with the rejected alternative
  restated.

---

## 4. OPEN-C1 CLOSED — DECISION C-3 simplified

The Code axis refuted the premise at the source, and I re-verified it:
`install_authority_record` takes its scratch from
**`BaseActionSlotV1::AuthorityScratch`**, not `RecordScratch` —
`authority_record_binding.rs:486`:

```
let scratch = slot_name(action, BaseActionSlotV1::AuthorityScratch)?;
```

with `write_authority_scratch` (`:488`) holding the three `record.scratch_*`
hits (`:508`, `:516`, `:529`). The `record.*` keys are named after the fault
**family** — the durable-record lifecycle, `fault_v1.rs:97-109` — not after a
slot. A tree-wide grep for `BaseActionSlotV1::RecordScratch` returns exactly two
non-definition hits, `namespace/roles.rs:20` and `:34` (the `PublishRoleV1`
routing), and **zero write paths**; `PublishRoleV1` carries `AuthorityScratch`
(`roles.rs:18`) and `RecordScratch` (`:20`) as distinct roles, which is itself
the disproof.

> **DECISION C-3, simplified (E0.2b).** The cleanup worklist's scratch row is
> `BaseActionSlotV1::RecordScratch` (`protocol/slots.rs:106`/`:140`), an
> **entirely unconsumed base slot** whose first and only user is the cleanup
> worklist. No slot is minted. **There is no sharing and no ordering
> condition.** The write-or-rewrite discipline of
> `write_managed_intent_scratch` (`managed_mutation.rs:1058-1064`, `:1073-1101`)
> still applies, because a leftover scratch from an interrupted drive is this
> drive's own residue.
> Rejected alternative unchanged: minting `CleanupWorklistScratch` would mint a
> persisted name (freeze §6, §3.1 persisted-home pin) and move
> `BASE_ACTION_SLOTS` from 13 (`bounds.rs:9`) and the `MAX_ACTION_SLOTS == 261`
> assertion (`bounds.rs:70`).

**Struck from the E0.2 draft:** OPEN-C1 in full (§2.2's "Ordering condition,
binding on E1.1 — OPEN-C1" paragraph); §2.3 row #1's parenthetical "(DECISION
C-3, with OPEN-C1's ordering condition)" → "(DECISION C-3)"; Appendix A's
OPEN-C1 row; Appendix B's C-3 row's "carries ordering condition **OPEN-C1**"
clause; §9 item 6's mention. **E1.1 and E1.2 owe no interleaving proof.**

The drafter flagged this for attack (E0.2 Appendix C, Code item 4) and the
prediction held: the tree answered it in one grep, and the E0.2 row's own
settling-evidence sentence ("no consumer of either exists today, so the question
cannot be answered by reading the tree") was backwards. Recorded as a lesson for
E0.3 round 2's own OPEN rows: an OPEN row whose settling evidence is "the tree
cannot answer this" must be re-tested against a grep before it is written.

---

## 5. O6 COMPLETED — the read-side identity refusal

### 5.1 What was missing

The E0.2 §3.1 correctly enumerates the Step-4.3 precedent's three parts and
§3.2's mechanism implements only two of them — (i) the owner observes, and
(iii) the issuer's signature carries the derivation obligation. **Part (ii), the
owner's refusal on disagreement, was applied only to the mint side.**

Verified for this addendum: `read_and_bind_barrier_intent`
(`protocol/barrier.rs:238-256`) re-checks exactly five facts on read —
`action_digest`, `request_owner_binding`, `reservation_digest`,
`schedule_digest`, `ordinal` (`:244-249`) — and **none** of
`catalog_anchor_identity`, `private_home_parent_identity`, `private_home_name`.
`decode_canonical` (`:120-159`) rebuilds through `from_bound_fields`
(`:140-152`), bypassing `issue` entirely. So a drive resuming after a crash
reads caller-asserted identities off disk and acts on them, however tight
`issue` becomes: **the restatement class survives restart** under the E0.2
mechanism.

### 5.2 The completion

> **O6 RESOLUTION, completed (E0.2b).** In addition to the mint-side mechanism
> of E0.2 §3.2 (the owner-minted `RoamingAnchorHomeWitnessV1`; the derivation
> obligation on `issue`'s signature; `private_home_name` derived and dropped as
> a parameter), the resolution now carries the **read side**:
>
> **The owner re-mints the witness on every resume and refuses when the resident
> intent's three identity facts disagree with it.** Concretely: the barrier
> owner, holding `RetainedCompletedCatalogV1`'s retained `catalog_anchor`
> (`completed.rs:44`, `:95-99`), `roaming_anchor` (`:45`, `:100-104`) and
> `final_directory` (`:42`, `:85`) capabilities, re-derives the witness and
> compares it against the record returned by `read_and_bind_barrier_intent`,
> refusing typed on any of the three. The comparison lives with the **owner**,
> beside `read_and_bind_barrier_intent`'s five existing checks — not inside
> `BarrierIntentV1::decode_canonical`, which has no capability and must stay a
> pure codec.
>
> **Key #5's semantic is extended accordingly.** `barrier.intent_reobserve` now
> names: *the active intent has been re-read bounded, bound to the resident
> reservation and ordinal by `read_and_bind_barrier_intent`'s five checks, **and
> its three identity facts proved equal to the witness the owner re-minted from
> its own retained capabilities.*** The E0.2 row documented the five-check read
> as sufficient; it is not.

### 5.3 A correction to the mechanism's own framing

E0.2 §3.2 says the witness lands "in the shape `AuthorityFactsIssuerV1::issue`
uses". That shape is the wrong way round: `AuthorityFactsIssuerV1` is handed
**to the caller**, which then supplies the facts
(`protocol/authority_record/owner/host.rs:104-119`), and the issuer's own doc
(`:53-73`) says the protection "is the provenance obligation documented above,
**not** the argument shape". Copied literally, E2.2 would rebuild the defect.

> **The correct shape is the inverse, and it already exists in the same owner:**
> `RetainedCompletedCatalogV1::observe_admission` (`completed.rs:142-147`) and
> `retain_action_namespace` (`:169-181`), whose in-code contract is *"The caller
> receives the typed observation only — never a handle"*. E2.2 mints the witness
> **inside** the pre-catalog provider owner and hands the caller a typed value it
> cannot construct.

### 5.4 The REFUSE arm's reachable states, restated

E0.2 §3.2 claimed "exactly one reachable state" blocks the witness mint — the
anchor not at home. Two corrections:

1. **Under DECISION B-5 the anchor never leaves home, so "anchor away" is
   reachable in ZERO states.** The E0.2 REFUSE arm has no trigger at all as
   written.
2. **At least two other states do block the mint**, and both are already typed
   refusals in the callee's own owner: the catalog anchor's identity changed
   (`completed.rs:211-216`, "retained catalog file identity changed", and
   `:358-376` at the named slot), and the final directory is no longer the named
   catalog (`completed.rs:195-203`).

> **REFUSE arm, restated.** The witness is mintable only from a
> `RetainedCompletedCatalogV1` that revalidates. Where revalidation fails — a
> changed catalog-anchor identity, a final directory that is no longer the named
> catalog, or a catalog whose completion predicate no longer holds — the seam
> **refuses typed and mints nothing**, and the drive's exit is the catalog
> owner's own recovery path, not the barrier's. The read side (§5.2) adds a
> second refusal: a resident intent whose identity facts disagree with the
> re-minted witness is refused typed rather than resumed.

---

## 6. The machine-enforced inventories the converging commits must move

The E0.2 draft named **one** — `FAULT_INJECTION_SOURCES` — and got its
arithmetic wrong. There are **three**, all machine-enforced, and the other two
were absent from all three convergence obligations (Code [P2-R1]). §6.1 corrects
the first; §6.2 adds the other two; **§6.3 gives the amended text of §2.5, §3.6
and §4.5 verbatim**; §6.4 banks one further ground for §5.

### 6.1 `FAULT_INJECTION_SOURCES` — NINE → TEN, one move, at E2

**The truth, re-verified for this addendum.** `FAULT_INJECTION_SOURCES`
(`interface_tests/fault_expected_keys.rs:419`+) declares **nine** files:
`mutation.rs`, `directory_mutation.rs`, `aggregate.rs`, `admission_mutation.rs`,
`leaf_observation.rs`, `namespace_mutation.rs`, `managed_mutation.rs`,
`protocol/authority_record.rs`, `authority_record_binding.rs`. This matches
freeze `:528`'s "**nine**".

Applying the amendment's own three routing decisions:

| Step | Decision | Site file | New declared file? | Count after |
| --- | --- | --- | --- | --- |
| E1 | C-2 | `namespace_mutation.rs` — already declared (`fault_expected_keys.rs:449-452`) | **no** | **nine** |
| E2 | B-1 | `barrier_mutation.rs` — `NEW@E2.1` | **yes, one** | **ten** |
| E3 | T-C / T-C′ (§8) | `admission_mutation.rs` and `namespace_mutation.rs` — both already declared (`:432-435`, `:449-452`) | **no** | **ten** |

**Exactly one new file. The count moves once, at E2: 9 → 9 → 10 → 10.**

Corrections, both of which enter frozen or commit-binding text:

- **§2.5 (E1.2's duty list) — STRIKE the count edit.** E1.2 owes the
  `FAULT_FAMILY_ACTIVATION` row edit (`fault_expected_keys.rs:380-384`) and the
  `FAULT_INJECTION_SOURCES` **doc-comment** extension (`:397-418`, recording that
  `namespace_mutation.rs` now holds cleanup sites too), but **no inventory count
  move**. The E0.2 text instructed E1.2 to land a false inventory edit under
  RemPlan §10's duty; that is the same defect class the freeze already graded
  once and corrected in round 2 (freeze `:943`, "(corrected in round 2 — Code
  P1-2, State P2-1)"; `:955-958`, "the recorded inventory was false and the
  reserved-family scan was porous").
- **§8 row 11 — the annotation content becomes:** *"the declared-and-anchored
  production source list moves **nine → ten**, the single move landing at **E2**
  with `barrier_mutation.rs`; E1 and E3 add sites to already-declared files
  (`namespace_mutation.rs`, `admission_mutation.rs`) and move no count. The
  completeness anchor (`fault_expected_keys.rs:414-418`) proves the list
  exhaustive at each landing."*
- **§9 item 1 gains the count trace**, so the aggregate is stated once:
  *"the declared-source count moves 9 → 10 across E1-E3, once, at E2."*

### 6.2 The two further inventories — the sealed-publication caller registry and the protected source-tree digests

Both live in `gwz-core/scripts/checks/check_checked_artifact_boundaries.py`,
both fail closed, and **neither the E0.2 draft nor E0.2b round 1 mentioned
either.** Verified live for this pass.

**(a) `CATALOG_PUBLICATION_CALL_COUNTS`** (`:726-746`) pins the sealed
primitive's callers by file **and exact count**. The dict as it stands, matching
the tree:

```
mutation.rs 1 · directory_mutation.rs 5 · admission_mutation.rs 2
namespace_mutation.rs 1 · managed_mutation.rs 2 · authority_record_binding.rs 2
```

`check()` additionally asserts set equality between the dict's keys and every
production file under `provider/` that calls the primitive, so a new caller file
fails closed. **This is not a routine digest bump.** The dict carries four dated
comments, each recording a deliberate extension by a named step and quoting the
freeze's own rule — *"The caller count stays at six production sites until a
phase deliberately extends it"* (`:720-725` Step 1.2, `:730-732` Step 2.2,
`:734-738` Step 2.3, `:740-744` Step 2.4).

- **E1 — no move.** Cleanup's retirements route through
  `RetainedActionNamespaceV1::execute_edge`, which already holds
  `namespace_mutation.rs`'s single counted call site
  (`namespace_mutation.rs:280-288`).
- **E2 — no move, and the amendment must say so explicitly.** DECISION B-1 mints
  a **new file under `provider/`**, so a reader will assume the dict moves. It
  does not: rows #10/#12 retire the alias through `execute_edge` — a
  same-directory rename passing `&self.handle` for both source and destination
  (`namespace_mutation.rs:280-283`) — and `barrier_mutation.rs` opens no sealed
  primitive of its own. **Banking this negative is part of the obligation**,
  because the set-equality check would otherwise be the first thing to tell E2.
- **E3 — MOVES.** T-B′'s terminal retirement is catalog root → **retired root**,
  and `execute_edge` is same-directory only, so it needs its own
  `publish_verified_no_replace` call in `admission_mutation.rs`: **2 → 3**.
  Named as the Code axis names it, **the seventh deliberate extension of a
  freeze-governed caller count** — and, so the number is checkable rather than
  inherited: the freeze's base is **six** sites (`mutation.rs` 1 +
  `directory_mutation.rs` 5); four dated extensions have added seven more, for
  **thirteen** today; E3's is the **fourteenth** production call site and the
  **fifth** dated extension of this dict. E3.2 owes the edit **and** its
  justifying dated comment in the dict's own established form.

**(b) `PROTECTED_SOURCE_TREE_DIGESTS`** (`:156-164`) pins whole module trees by
path-and-byte manifest; the comment is explicit that "a nested helper, **a new
source file**, or a changed module edge fails closed" (`:153-155`). The two
entries R2-E disturbs, both reproduced by the Code axis against the live tree:

- `checked_artifact/capability/pre_catalog.rs` = `4e06f84d…` (`:159`) — its
  descendant root is `pre_catalog/`, covering `provider/interior.rs`,
  `completed.rs`, `publication.rs`, `namespace_mutation.rs`,
  `admission_mutation.rs` and any new `barrier_mutation.rs`. **Trips at E1, E2
  and E3 — every converging commit.**
- `checked_artifact/platform.rs` = `febdc28b…` (`:161`) — **trips at E2**,
  because DECISION B-3 adds the third `DirentBarrierClass` variant to that file
  (`platform.rs:363-374`).

### 6.3 The three convergence obligations, amended verbatim

The following replace the closing duty lists of E0.2 §2.5, §3.6 and §4.5. Each
is the full list the converging commit owes; the additions are the last two or
three bullets.

> **§2.5 — E1.2 owes, in the converting commit:** `CLEANUP_MATRIX: [Fault; 11]`,
> every key interrupted+restarted+converged on both target variants; twelve-round
> repeated-boundary rows over a declared subset; single-crossing classification
> declared per row, driven by `run_single_crossing_probe`
> (`bootstrap/managed/tests_provider.rs:564`), on a **one-alias** row; the
> matrix-to-fixture reconciliation in the `reconcile_executed_keys` shape; the
> `FAULT_FAMILY_ACTIVATION` row edit `Reserved(…)` → `Executed(…)`
> (`fault_expected_keys.rs:380-384`); the `FAULT_INJECTION_SOURCES`
> **doc-comment** extension (`:397-418`) recording that `namespace_mutation.rs`
> now holds cleanup sites — **and no inventory count move (§6.1)**; the
> Git-directory route statement (§7.7); the R4b-G driver's per-OS fault markers;
> **`PROTECTED_SOURCE_TREE_DIGESTS["checked_artifact/capability/pre_catalog.rs"]`
> re-pinned**; and **a stated confirmation that
> `CATALOG_PUBLICATION_CALL_COUNTS` does not move**, because cleanup retirements
> route through `execute_edge`.
>
> **§3.6 — E2.3 owes, in the converting commit:** `BARRIER_MATRIX: [Fault; 16]`
> on both target variants with the same interruption/restart/convergence,
> repeated-boundary and single-crossing discipline; the Windows arm of
> `barrier.target_barrier` **executed natively, not skipped**, in the Step-4.2
> standard's shape (`hard_link_identity_sharing_is_what_the_retirement_rows_assume`,
> freeze `:836-838`) — which is also OPEN-B7's probe; the
> `FAULT_FAMILY_ACTIVATION` row edit (`fault_expected_keys.rs:385-389`); the
> `FAULT_INJECTION_SOURCES` **count move 9 → 10** with `barrier_mutation.rs`
> declared and its doc-comment entry; the §3.5 inventory addendum's file count
> (freeze `:525-532`, "nine" → **ten**); the Git-directory route statement;
> the R4b-G per-OS markers; **`PROTECTED_SOURCE_TREE_DIGESTS` re-pinned for BOTH
> `…/pre_catalog.rs` and `…/platform.rs`** (the latter because DECISION B-3 adds
> the third `DirentBarrierClass` variant); and **a stated confirmation that
> `CATALOG_PUBLICATION_CALL_COUNTS` does not move**, notwithstanding the new
> file under `provider/`, because the alias retirements route through
> `execute_edge`.
>
> **§4.5 — E3.2 owes, in the converting commit:** `TERMINAL_MATRIX: [Fault; 10]`
> — ten, not eleven, per §4.3's determination — on both target variants with the
> same discipline; the
> `PartiallyExecuted("…", TERMINAL_EXECUTED_KEYS)` fixture row with its
> key-by-key siteless proof for `terminal.authority_release`
> (`fault_expected_keys.rs:187-222`, `:390-394`); the `FAULT_INJECTION_SOURCES`
> doc-comment extension for the T-C′ split across two already-declared files —
> **and no count move (§6.1)**; the Git-directory route statement; the R4b-G
> per-OS markers;
> **`PROTECTED_SOURCE_TREE_DIGESTS["checked_artifact/capability/pre_catalog.rs"]`
> re-pinned**; and
> **`CATALOG_PUBLICATION_CALL_COUNTS["…/provider/admission_mutation.rs"] 2 → 3`,
> with the dated deliberate-extension comment the freeze's caller-count rule
> requires** — the fourteenth production call site (§6.2(a)).

### 6.4 One further ground for DECISION §5.2, banked

`operation/workspace_mutator_lock.rs` is itself **byte-pinned**:
`PROTECTED_SOURCE_DIGESTS["operation/workspace_mutator_lock.rs"]` =
`0d9b034e…` (`check_checked_artifact_boundaries.py:57`). So option (i) — probing
the durable identity at lock acquisition — would edit a **byte-pinned file** for
a proof nothing at that point uses. That is a fifth, previously unstated ground
for §5.2's option (ii), and E0.2 §5.2 gains it as one clause beside Grounds 1-4.

---

## 7. §11.1 and §11.3, restated in full — the A1 coexistence gate and the E4 sequencing

### 7.1 The mis-attribution, on the record

> **CORRECTION, recorded in the form plan §0 used for 67 → 38.** E0.1(b) opened
> "`GwzM5-8R2DSettledTuple.md` §11.3 ('Settle caveats the 5.2 dual should weigh',
> :798+) binds the activation as follows" and then listed **§11.1's** rows. The
> section boundaries, verified directly for this addendum:
> **§11 `:634`; §11.1 `:636-775`; §11.2 `:776-797`; §11.3 `:798-828`.**
> E0.1(b)'s five rows are §11.1 content — `authority_name` `:649-652`,
> `BarrierIntentV1::issue` `:653-658`, the anchor nits `:659-662`, the ten writer
> rows `:663-665`, and §11.1's 481-item row `:645`. **The real §11.3 has eight
> numbered caveats and shares none of its items with that list.** The rows'
> content is unaffected and their E0.2 consumptions stand on the §11.1 basis;
> what was lost is §11.3 itself, whose item 1 is the gate that governs O2.
> **The corpus must stop citing "§11.3" for §11.1's rows.**

**Consequential retitle:** E0.2 §7's heading becomes *"The E0.1(b) row
consumption table (E0.1(b) rows 1-5, sourced from settled tuple §11.1)"*, and
its table header column "§11.3 row" becomes "E0.1(b) row (§11.1 source)". The
body was already honest ("Every row of E0.1(b)"); only the label was wrong.

### 7.2 §11.1 "Re-reserved for R2-E" (`:636-775`) — the full restatement

The section opens with a six-row table and then carries three dated addition
blocks. Restated in full, with each row's disposition **in R2-E as of this
addendum**:

**The base table (`:638-645`):**

| item | record | disposition |
| --- | --- | --- |
| `cleanup.*` — 11 keys | freeze §3.5 non-activation record (Step 4.1) | **CONSUMED** — E0.2 §2, all 11 |
| `barrier.*` — 16 keys | freeze §3.5 non-activation record (Step 4.2) | **CONSUMED** — E0.2 §3 as amended by §1/§5 here, all 16 |
| `terminal.*` — 11 keys | freeze §3.5 non-activation record (Step 4.2) | **CONSUMED** — E0.2 §4, 10 activated + 1 determined |
| The whole §10 consumer conversion table | plan §5 item 1 | **ADVANCED, GATED** — §7.4-§7.6 below |
| First production catalog activation | plan §5 item 2; **gated by §11.3** | **GATED** — §7.5; this is the pointer E0.1(b) followed to the wrong section |
| The 481-item / 63.6%-of-subsystem frozen surface behind the seven blankets | §5.3 | **DEFERRED to E7.2** with both denominators (E0.2 §7.4) |

**Added at the settled-dual round-2 remediation, 2026-08-23 (`:647-692`):**

| # | item | disposition |
| --- | --- | --- |
| a | `authority_name` embeds no identity digest (`:649-652`) | **TAKEN** — E0.2 §7.1 DECISION A-1, re-framed per §8 below |
| b | `BarrierIntentV1::issue` accepts caller-asserted anchor/home identity facts; **BINDING R2-E obligation** (`:653-658`) | **CONSUMED** — E0.2 §3.2 + §5 here (read side) |
| c | Anchor nits inside the accepted same-user boundary (`:659-662`) | **SPLIT** — `unsupported.rs` → E4.1 precondition; `verify`'s `read_to_end` and the retired-ordinal parse → E6.2 |
| d | The ten multi-component writer rows' native Windows/ARM execution ledger debt (`:663-665`) | **DEFERRED** — next E-landing matrix, classification ledger |
| e | `root_fault_matrix` perf pricing, PRICED-NOT-OPTIMIZED (`:666-686`) | **untouched-by-design** — a signed ruling, no R2-E duty |
| f | The L2-05 merge-doc gates wired into no workflow (`:687-692`) | **untouched-by-design** — owner R2-F per A1 record §7 (`GwzM5-8A1ActivationRecord.md:167-172`) |

**Added at the R4b-G dual's round-2 remediation, 2026-08-24 (`:694-750`):**

| # | item | disposition |
| --- | --- | --- |
| g | **C-1** — the O8 "every M4" byte-equivalence clause measured NOT MET (13/39) (`:696-727`) | **CLOSED pre-A1** at gwz-core `26f48f5` (see i); its residue is O8's, consumed at E0.2 §6 + §8's denominators |
| h | **C-2** — four M4 scenarios with no durable fixture (`:728-736`) | **NOT R2-E's** — owner R2-F fixtures/native-evidence lane per L6 bullet 1; two of the four are §6.4's archive shapes (PENDING-FIXTURE), two (`B-NOT-STARTED`, `B-PREPARING-EMPTY`) are **not R2-E's at all** (§8) |
| i | M5b-IMPL settled review OWED PRE-A1 (`:737-750`) | **untouched-by-design** — discharged pre-A1 |

**Added at the pre-A1 closures, 2026-08-24 (`:752-774`):**

| # | item | disposition |
| --- | --- | --- |
| j | C-1 CLOSED at `26f48f5` (`:754-759`) | **consumed as input** to E0.2 §6's mechanism |
| k | M5b settled [P2-1] — the T-5 pair, CLOSED BY LANE-OWNER NARROWING (`:760-774`) | **untouched-by-design** — carrier is R2-F (§11.2 `:778-786`) |

### 7.3 §11.3 "Settle caveats the 5.2 dual should weigh" (`:798-828`) — the eight items, closed row by row

| # | lines | item | **disposition in R2-E** |
| --- | --- | --- | --- |
| **1** | `:800-802` | **The A1 coexistence gate.** "No production catalog activation at `.gwz/checked-artifacts` until the relocation lands. Fail-closed in code today: `recover_or_create` has no production caller (re-verified at this tuple)." | **THE GOVERNING GATE ON O2 — §7.5.** Added to E0.2 §5.3 as precondition **item 0**. |
| **2** | `:803-808` | **The test-door dependency.** Step 3.2's Git-directory arm places the managed prefix under that target's own retained root because the Step-2.3 door (`retain_managed_parent_at_for_test`) stages with `fs::write` and can never reach the writer boundaries. "Kept as a documented `cfg(test)` door; **revisit on an R2-E production caller** (settle disposition 2). **The Git-directory catalog's workspace-root binding is an R2-E input** (disposition 3)." | **CONSUMED HERE — §7.7.** Silently dropped by E0.2; given an owner and terms below. |
| **3** | `:809-811` | `runtime.*` — 18 keys declared executed with no per-key evidence; "the dual should rule: re-reserve, or restate the declaration. Census-neutral either way." | **ALREADY RULED, not R2-E's.** The 5.2 dual ruled *restate*, and the ruling is in frozen text at freeze `:434-479` with its own census statement ("165 total, unchanged; no key minted, none retired; 18/165 unchanged"). R2-E opens no duty; the natural future carrier named there is the R2-F native fault-evidence gate. |
| **4** | `:812-813` | `managed_mutation.rs` at 1,251 lines — the one R2-D-owned file over the cohesion trigger | **CONSUMED** — DECISION C-2's and B-1's cohesion arithmetic (E0.2 §2.2, §3.3), which cite it by item number. |
| **5** | `:814-815` | `staging_directory_flush`'s second site is announced but not independently interruptible; a sixth key would move the census | **CONSUMED as precedent**, but its holding is corrected — see §8, State [P2-3]. |
| **6** | `:816-818` | The operator policy question (escape runbook, panic-invariant conversion, preservation-cursor) — "parallel-program debts, not R2-D's, and unresolved" | **untouched-by-design.** Not R2-E's; the plan's §5 item 2 records the executable-template policy as an open operator item. |
| **7** | `:819-825` | Two L2-05 gates wired into no workflow — "a gate that only passes on a developer's machine is not a gate" | **untouched-by-design** — owner R2-F release-lane CI (A1 record §7). R2-E adds no reliance on L2-05. |
| **8** | `:826-828` | Second consecutive over-budget Phase-4 step; compositions recorded rather than trimmed | **historical.** R2-E's own budget rows are the plan's, per step. |

### 7.4 What the A1 coexistence gate actually protects — verified

The gate is not bureaucratic. Verified for this addendum, the legacy checked
artifact writes **into the catalog's own Final directory**:

- `policy.rs:33-42` — `CheckedArtifactPolicy::private_parent()` returns
  `CatalogPrivateNameV1::Final.relative_path(CatalogPrivateRootV1::Workspace)`
  for a workspace artifact and the Git-directory twin for the other, i.e.
  **`.gwz/checked-artifacts`** / `gwz/checked-artifacts`
  (`catalog_names.rs:36-40`, leaf bytes at `:39`).
- `observation.rs:93` sets `quarantine_parent = policy.private_parent()`, and
  `residue.rs:45-57` opens exactly that path as the legacy private area.
- The catalog's Final directory is the same name
  (`CatalogPrivateNameV1::Final => b"checked-artifacts"`, `catalog_names.rs:39`).

And a `ca1-*` child makes the catalog **unobservable**, one level earlier than
even §0's classifier chain: `interior::exact_row` refuses any child that is
neither an infrastructure slot nor an `ActiveAction` row —
`Err(CheckedFsError::ambiguous("catalog interior", "catalog directory contains
an unowned child"))` (`interior.rs:414-417`), reached because
`RootEntryNameV1::parse` returns `Foreign` for a name not starting with
`action-` (`slots.rs:434-436`).

So: **activating the catalog at `.gwz/checked-artifacts` on any workspace that
holds legacy residue yields a catalog that `interior::observe` refuses outright.**
On Unix the residue is transient (`finish()` removes it, `cleanup.rs:86-94`,
`:131-139`, `:166-174`); on **Windows it is unconditional**, because the
durability anchor is permanent by design and `.ca1-anchor-retired-<n>`
accumulates with crashes (freeze `:180-190` of `GwzM5-8R2DPhase4Closure.md`
records exactly this as ground (ii) of the decision).

The relocation's pin, quoted: `GwzM5-8R2DSettledTuple.md` §11.2 `:791` —
"**Quarantine/relocation execution** — the legacy private area moves out of the
catalog Final directory" — pinned to **R2-F**; and
`GwzM5-8R2DPhase4Closure.md` §2.7 `:192-200`: "**Execution is not R2-D's** …
What this step therefore installs is the **gate**, and it is A1-gating: *no
production catalog activation at `.gwz/checked-artifacts` until the relocation
lands.*"

### 7.5 The cure to E0.2 §5.3 — precondition item 0

> **§5.3 item 0 (E0.2b), above the five.** E4.1's **O2 half** is additionally
> gated by settled-tuple §11.3 item 1: *no production catalog activation at
> `.gwz/checked-artifacts` until the quarantine/relocation package lands*, and
> that package is pinned to **R2-F** (`GwzM5-8R2DSettledTuple.md` §11.2 `:791`;
> `GwzM5-8R2DPhase4Closure.md` §2.7 `:192-200`; `CurrentProgramCheckpoint.md`
> the coexistence record). The gate is fail-closed in code today —
> `recover_or_create` has no production caller — and it is not a formality: a
> catalog activated over resident legacy residue is refused by
> `interior::exact_row` (`interior.rs:414-417`) and, on Windows,
> unconditionally so.
>
> **E4.1's `WorkspaceMutatorLock` half may land without it. `recover_or_create`'s
> first production caller may not.**

### 7.6 Which E4 rows truly require catalog activation — the NINE-row analysis

**Correction first, on the record.** The E0.2b round-1 table mapped **eight**
rows. `GwzM5-8R4bR2ConsumerCheckpoint.md` §10 has **nine** consumer rows,
`:272-280`. The missing row is `:280`, and **the plan's O1
(`GwzM5-8R2E-Plan.md:41`) enumerates the same eight and drops the same one**, so
the table inherited the gap rather than creating it. It is corrected below, in
the form plan §0 used for 67 → 38 and §8 uses for 18 → 10: *the §10 table is
nine rows; O1's enumeration is eight; the ninth is row `:280` and it is
addressed by no plan step.*

Every §10 row's *converted* form reaches the catalog through one of exactly two
doors, both of which consume `OpaqueRetainedCatalogV1`:

- admission — `ActionAdmissionOwnerV1::from_retained_catalog(catalog)`
  (`coordinator/execution.rs:141`), required by every `Replace`/`Remove`;
- managed-parent bootstrap —
  `RetainedManagedParentProviderV1::from_retained_catalog(catalog)`
  (`coordinator/execution.rs:199`), required by every "when missing" parent row.

| §10 row (`ConsumerCheckpoint:272-280`) | plan step | catalog required? | door |
| --- | --- | --- | --- |
| `WorkspaceMutatorLock` — "runtime bootstrap only" | E4.1, conversion half | **NO** | already converted at v0.11.0 (`operation/workspace_mutator_lock.rs:35-38`); §5's decision keeps it probe-free |
| *(O2 — `recover_or_create`'s first production caller)* | E4.1, activation half | **YES, definitionally** | it *is* the activation |
| first merge record — `MergeStore` + `PreservationBundles` when missing | E4.2 | **YES** | managed-parent bootstrap |
| merge record rewrite — exact existing `MergeStore` | E4.3 | **YES** | admission (a checked leaf action) |
| terminal archive source-only — `MergeArchive` when missing | E4.4 | **YES** | managed-parent bootstrap |
| ordinary/merge stash bundle; commit/merge marker | E4.5 | **YES** | managed-parent bootstrap |
| workspace marker/lock checked mutation; `.git/info/exclude` | E4.6 | **YES** | admission |
| **`:280` — v1 checked store/root/bundle paths** — "same purposes and artifact actions \| test-gated until A1; no legacy raw writer" | **NO STEP OWNS IT** | **SPLIT: the substantive half YES; the "no legacy raw writer" half NO** | §7.6.1 |
| O3 close-out — legacy writers stop mutating inside `.gwz/checked-artifacts` | E4.7 as planned | **the plan's E4.7 mechanism: YES, transitively. O3 *as an obligation*: NO** | §7.6.2 |

**Finding, corrected: the catalog-free set is NOT empty.** It contains two
things the round-1 table denied, both of which cut *for* option (b):

1. **Row `:280`'s "no legacy raw writer" clause** — catalog-free, its gate
   already fired, currently unmet in production, owned by no step (§7.6.1).
2. **O3's relocation-discharge route** — catalog-free, single-owner (§7.6.2).

What *does* collapse is option (c) **as a partition of the substantive §10
conversions**: for all eight rows the round-1 table mapped, the catalog
dependency is real, and the State axis attacked it four ways without breaking
it (`.git/info/exclude`'s "never bootstrap `.git/info`" ordering; the
`WorkspaceMutatorLock` row's ordering half, which already holds at all five
merge sites; the two "existing parent" rows; and a census of the doors, which
found no third). Neither of the two catalog-free items above is a *conversion* —
one is a raw-writer prohibition, the other a relocation — so neither revives
option (c).

#### 7.6.1 Row `:280` — the row no step owns, and the obligation it implies

The row, verbatim (`GwzM5-8R4bR2ConsumerCheckpoint.md:280`):

> | v1 checked store/root/bundle paths | same purposes and artifact actions | test-gated until A1; no legacy raw writer |

Three facts, each verified for this pass:

1. **Its gate has already fired.** "Test-gated until A1", and **A1 shipped
   2026-08-25** (gwz-dev `4b9f078`; the activation record's landing). Every
   other §10 row's precondition is still ahead of it; this one's is behind it.
2. **Its second clause is catalog-free, and it is not met.**
   `v1_lifecycle/store/rewrite.rs:6` is
   `use crate::durable_fs::{rename_durable, sync_dir};` — a **raw** durable
   writer — and `v1_lifecycle/` holds **zero non-test `checked_artifact`
   references** (the single tree-wide hit is a doc comment at `start.rs:15`).
   Post-A1, `no-ff → v1` is live end-to-end
   (`GwzM5-8I2CompatibilityContract.md:86`, the dated A1 annotation), so this is
   a **production** raw writer on the v1 path — precisely what the row forbids
   once its test-gate expires.
3. **O1 cannot be reported DISCHARGED without it**, under any sequencing option,
   and no plan step owns it: `relocation`/`quarantine` aside, neither
   `GwzM5-8R2E-Plan.md` §1's O1 enumeration nor any E4 step names the v1 store.

> **PROPOSED NEW PLAN OBLIGATION — O13: the v1 store's raw-writer clause.**
> *Convert the v1 checked store/root/bundle paths per §10 row `:280`'s frozen
> ordering — "same purposes and artifact actions" — and discharge its "no legacy
> raw writer" clause, whose test-gate expired at A1.* Added to
> `GwzM5-8R2E-Plan.md` §1's ledger, and to O1's enumeration on the record, in
> plan §0's correction form.

**Ownership analysis, in one paragraph.** O13 has two halves with different
owners and they should not be bundled. The **substantive** half — routing the v1
store's writes through checked artifact actions for the same purposes — is
catalog-bound like every other §10 row, so it belongs in Phase E4 and is gated
exactly as its siblings are; and because `v1_lifecycle/store` is the *same
store* that E4.2 (first merge record, `MergeStore` + `PreservationBundles`) and
E4.3 (merge record rewrite, "exact existing `MergeStore`") convert, giving it
its own parallel E4 row would put two steps on one store and invite the drift
the §10 table's frozen ordering exists to prevent. **Recommendation: the
substantive half rides E4.2/E4.3 as an explicit scope clause rather than
becoming a tenth E4 row** — E4.2 owns the store's creation path and E4.3 its
rewrite path, which is exactly the `rewrite.rs` surface the raw writer sits on.
The **"no legacy raw writer"** half, by contrast, is catalog-free, already due,
and is a *pin*, not a conversion: it should land **now**, as a precondition
pin on E4.2 — a boundary-checker assertion that `v1_lifecycle/` names no
`durable_fs` raw writer once the converted path exists, with a **dated
accepted-residual record covering the interval** between A1 (gate fired) and
E4.2 (clause met), so the corpus stops carrying an expired gate silently. If the
lane prefers not to pin it, the honest alternative is the same dated
accepted-residual record with an explicit expiry named — but not silence, which
is the status quo.

**How row `:280` changes the §7.8 balance: it cuts for option (b).** It is a
**second** catalog-free obligation that is due *now* and blocked by nothing,
sitting beside relocation. Under option (a) both wait on another lane's schedule
while an expired gate stands unrecorded; under option (b) R2-E holds the whole
catalog-free critical path — relocation, O3, and O13's pin — and can land them
in one ordered run before E4 opens. It also removes the last reading on which
option (c) looked attractive: the catalog-free set is real, but it is *not
E4 conversion work*, so "partial E4" remains the wrong frame even now that the
set is non-empty.

#### 7.6.2 O3's row, corrected — it has a catalog-free discharge route

The round-1 table rowed O3 as "**YES, transitively** — requires E4.2-E4.6
landed". That is true of the **plan's E4.7 mechanism** and false of **O3 as an
obligation**, and the distinction matters because it is the fact that makes
§7.8's ground 4 breakable rather than merely true.

O3's ledger text is a **location** property, not a conversion property —
`GwzM5-8R2E-Plan.md:43`: "Legacy writers stop mutating **inside**
`.gwz/checked-artifacts`."

And the legacy private parent has **exactly one non-test owner**, verified
tree-wide:

- `checked_artifact/policy.rs:33-42` — `CheckedArtifactPolicy::private_parent()`,
  the definition;
- `checked_artifact/observation.rs:93` — `let quarantine_parent =
  policy.private_parent();`, **the only consumer**.

(Every other `private_parent` hit in the subsystem is a different function — the
catalog owner's own `create_git_private_parent` / `has_private_parent` /
`require_create_private_parent` — not the legacy policy's.)

> **Corrected O3 row.** Relocating that one function moves every legacy write
> out of `.gwz/checked-artifacts`, and **O3 becomes literally true with zero
> conversion, zero admission, zero catalog.** Under option (b), relocation
> **discharges** O3 directly rather than merely unblocking E4.7. Under option
> (a), O3 is **discharged by R2-F's relocation package and re-owned** — it is
> not *blocked*. The plan's E4.7 step remains the mechanism for retiring the
> legacy writers *entirely* (which is R2-F's deletion charter's neighbourhood),
> but that is a stronger property than O3's text asks for.

**Three reliefs worth stating, because they bound the damage:**
1. **E1, E2 and E3 are NOT gated.** The fault families' matrices are `cfg(test)`
   and build their catalogs through test-only construction; plan §5 item 2's ban
   is on *production* activation. E1-E3 land production-unreachable code exactly
   as R2-D's Phases 1-4 did. The relocation gate blocks E4, not E1-E3.
2. **E5 is only half gated.** E5.1 (registry rows + the parametric `adapt_open`
   refusal test) and E5.2 (archive-corpus rows) touch the merge-record
   compatibility surface, not the checked-artifact catalog; only `gc_archived`'s
   production route (E4.4's rider) is gated.
3. **Two obligations are catalog-free and due now** — O13's raw-writer pin
   (§7.6.1) and O3's relocation route (§7.6.2). Neither waits on the gate;
   both wait only on an owner.

### 7.7 §11.3 item 2 — what it requires, and its consumption

Item 2 names R2-E twice and asks two different things.

**(a) "Revisit on an R2-E production caller."** The `cfg(test)` door is
`retain_managed_parent_at_for_test`
(`capability/pre_catalog/provider/managed_mutation.rs:383`). The trigger is the
first R2-E step that gives the managed-parent provider a **production** caller —
which by §7.6 is **E4.2**. Until then E1/E2/E3 inherit the door unchanged.

> **CONSUMED (E0.2b).** Owner: **E4.2**, in-commit, with the door's disposition
> recorded — either the door is retired because the production caller supplies a
> real retained managed parent, or it survives with a dated reason. **And a duty
> lands earlier, on E1.2 / E2.3 / E3.2:** each family matrix must drive on both
> target variants (E0.2 §2.5), and its **Git-directory arm must state which route
> it takes** — route (a), the Step-2.3 door, or route (b), the managed prefix
> under that target's own retained root — because "a Git-directory catalog has
> no `.gwz` ancestor" (freeze `:596-600`, restated at `:672-680`). The E0.2 draft
> required both variants without warning the E-steps that one of them has a
> `cfg(test)` dependency the settle asked R2-E to revisit. Stating the route is
> a one-line record per matrix; discovering it mid-step is a lost cycle.

**(b) "The Git-directory catalog's workspace-root binding is an R2-E input
(disposition 3)."** Freeze `:678-680` attaches it as "the owner decision it
awaits": when the catalog target is a Git directory, a managed parent's
workspace root is fixture-placed today.

> **ROUTED (E0.2b), not taken.** The question — *for a Git-directory catalog,
> which durable root binds a managed parent's prefix?* — is an E4 design input
> with no evidence in the tree to settle it (no production Git-directory managed
> parent exists). **Owner: E4.2**, answered in the same commit as (a), with the
> answer recorded against freeze `:672-680`. It is recorded here so the input is
> not lost a second time.

### 7.8 The sequencing proposal

Three options were posed. Measured against §7.6 **as corrected**:

- **(a) E4.1 waits on R2-F's relocation as a cross-lane dependency.** Because
  every substantive §10 conversion is catalog-bound, this does not gate E4.1
  alone — it gates **E4.1 through E4.7 entire**. **The blocked pair is O1 and
  O2**, and O1's close must additionally carry §10 row `:280` (§7.6.1), which no
  option discharges by itself. **O3 is not blocked by relocation — it is
  *satisfied* by it** (§7.6.2), so under (a) O3 is **discharged by R2-F's
  package and re-owned**, not stalled. *(The round-1 text said "O1, O2 and O3";
  overstating the rejected option's cost is a defect in decision-support even
  when the recommendation is right, and it is corrected here.)* R2-E would close
  with two of its ledger obligations undischarged and re-owned to a lane it does
  not control, a third discharged elsewhere, and a fourth — O13 — newly minted
  and unowned. Honest, cheap, and it makes the plan's largest phase externally
  blocked.
- **(b) The relocation is pulled forward into R2-E as its own phase.**
- **(c) Partial E4.** Collapses **as a partition of the substantive conversions**
  (§7.6). The catalog-free set is *not* empty — it holds O13's raw-writer pin and
  O3's relocation route — but neither is E4 conversion work, so "partial E4"
  remains the wrong frame.

> **PROPOSAL (E0.2b): option (b) — pull the quarantine/relocation execution into
> R2-E as a new phase, ordered before E4 and parallel to E1-E3. Recommended, with
> the explicit note that this is a CHARTER CHANGE and therefore the operator's or
> lane owner's ruling, not this addendum's to take.**

*(RULED 2026-08-27: **option (a)** — the operator, one-line reply verbatim
"a". The proposal above is declined; the fallback below is executed at
`GwzM5-8R2E-Plan.md` §1.1 — the cross-lane dependency row on O1+O2, O3
re-owned, E4 re-scheduled, the E7 close form with O1 carrying row `:280`,
O13 minted with its split ownership, and OPEN-R1 routed to R2-F.)*

**Grounds.**

1. **Relocation is catalog-free and self-contained.** Its code surface is
   `CheckedArtifactPolicy::private_parent()` (`policy.rs:33-42`) — the single
   owner of the legacy private parent, consumed once at `observation.rs:93` —
   plus a durable migration of resident residue. It depends on nothing in R2-E
   and nothing in R2-F. It can start the day E0 closes.
2. **It is the critical path for everything E4 owns.** Every substantive §10
   conversion is gated on it (§7.6). Leaving it cross-lane converts R2-E's
   largest phase into a blocked set and makes **O1 and O2** undischargeable
   inside the lane — and, because relocation is also O3's discharge route
   (§7.6.2), it exports a third obligation the lane could have closed itself.
3. **R2-F's charter is downstream of it, not upstream.** R2-F is "closure —
   remove legacy successful paths" (`ConsumerCheckpoint:304-305`), and the
   Phase-4 closure records that relocation is what makes that deletion "remove a
   directory rather than un-widen a frozen grammar"
   (`GwzM5-8R2DPhase4Closure.md:124-126`). Relocation enables R2-F; it does not
   need R2-F.
4. **R2-E already owns relocation's necessary condition — and the circularity
   it exposes is BREAKABLE, by a single-owner edit.** O3 — "legacy writers stop
   mutating inside `.gwz/checked-artifacts`" — is plan §1's E4.7 row, and the
   plan's mechanism for it is circular: **(i)** E4.7 retires the writers *behind
   the converted paths* (plan `:146-148`); **(ii)** the converted paths need the
   catalog (§7.6, sustained under four independent attacks); **(iii)** the
   catalog needs relocation (§11.3 item 1, mechanism verified at
   `interior.rs:414-417` via `slots.rs:434-436`). All three legs are real.
   **The fourth leg is what breaks it, and it is a fact about the tree, not a
   preference:** the legacy private parent has **exactly one non-test owner** —
   `policy.rs:33-42` defines it, `observation.rs:93` is its only consumer — so
   relocation is a **single-owner, legacy-only** edit that needs no conversion,
   no admission and no catalog (§7.6.2). Landing it before conversion breaks the
   cycle at leg (iii) and **discharges O3 outright** at the same time. That is
   precisely option (b), and it is why ground 4 is the ground that turns a
   preference into a structural argument.
5. **It banks the MAX_PATH retirement a release earlier**
   (`CurrentProgramCheckpoint.md:1022-1024`; `GwzM5-8R2DPhase4Closure.md:127-131`),
   which is the product benefit that decided the coexistence direction in the
   first place.
6. **It gives the lane the whole catalog-free critical path in one run.**
   Relocation, O3's discharge, and O13's raw-writer pin (§7.6.1) are all
   catalog-free and all due now; under (b) they land in one ordered sequence
   before E4 opens, under (a) they are split across two lanes with an expired
   §10 gate standing unrecorded in the interval.

**Costs and risks, stated.** It is "a durable relocation of live production
state, needing its own package" (`GwzM5-8R2DPhase4Closure.md:132-133`) — a
migration with its own restart matrix, its own three-platform acceptance, and a
Windows arm that must move a *permanent* anchor. It also moves work into a lane
whose budget the plan sized without it. And it contradicts three standing
assignments — freeze §5 decision 2, plan §5 item 7, and
`GwzM5-8R2DPhase4Closure.md` §2.7 — which is why it is a ruling, not a decision.

**Fallback if (b) is declined:** option (a), taken explicitly rather than by
default — with four elements, two of them corrected this round:
(i) the plan's §1 ledger gains a **cross-lane dependency row on O1 and O2**
naming R2-F's relocation package as the blocker; (ii) **O3 is recorded as
discharged by that package and re-owned, not blocked** (§7.6.2); (iii) E4 is
re-scheduled after it; and (iv) R2-E's E7 acceptance closes those rows as
*re-owned with a named carrier* rather than DISCHARGED — with **O1's close
additionally carrying §10 row `:280`** (§7.6.1), which no sequencing option
discharges on its own. **What must not happen is the status quo**, in which the
plan schedules E4 with no mention of the gate and the lane discovers it at E4.1.

**A note the operator may want:** under either option the *sequence* is the
same — relocation, then activation, then conversion, then legacy retirement.
Only the owning lane differs.

---

## 8. Both axes' remaining P2/P3 — disposition tables, rounds 1 and 2

**Code axis.**

| Id | Finding | Disposition |
| --- | --- | --- |
| [P1-1] | B1 candidate (i) not implementable | **FIXED** — §1.1 |
| [P1-2] | T-B not implementable as a field generalization | **FIXED** — §3 (T-B′) |
| [P1-3] | OPEN-C1 refuted; C-3's premise wrong | **FIXED** — §4 |
| [P2-1] | Both precondition packages are three gates | **FIXED** — §2.2 for T1; B1's rows are **moot** under §1.6 and are struck rather than restated |
| [P2-2] | A move-based roam is fatal at key #12 | **FIXED** — §1.1 fact 3 and §1.5 row #12 |
| [P2-3] | O6 applies precedent parts (i)/(iii) but not (ii) | **FIXED** — §5.2, §5.3 |
| [P2-4] | §2.4/§2.5's "real admitted action" duty is not executable | **FIXED, corrected below** |
| [P2-5] | `census.retired` is a misreading; quote `driver.rs` in full | **FIXED** — §2.3 |
| [P2-6] | The REFUSE arm's "exactly one reachable state" is wrong | **FIXED** — §5.4 |
| [P2-7] | T-C splits the terminal family across a capability boundary | **FIXED** — DECISION T-C′ below |
| [P2-8] | The declared-source count is wrong in both places | **FIXED** — §6 |
| [P3] a-l | Citation drift | **FOLDED** — below |

**[P2-4] cure — the corrected admitted-action duty.** E0.2 §2.4 bound E1.2 to
drive every row through `ScheduledCheckedActionV1::admit`. That is impossible
for the restart half, and the tree already records why:
`namespace/tests_fault_matrix.rs:20-35` states that once a namespace edge has
published its first row the action directory is no longer *exact*
(`protocol/admission/owner.rs:29-38` requires `extra_children: 0`), which is
precisely the state a second admission must refuse, and that resuming that
handoff from durable state "is **not owned by any landed step**".

> **CORRECTED DUTY (E0.2b).** E1.2 / E2.3 / E3.2 drive the **first** attempt
> through a real admitted action (`coordinator/execution.rs:137-152` →
> `ActionAdmissionOwnerV1::resume_or_admit`). **Every restart rebuilds the
> handoff through the test-only issuer** (`protocol/admission/test_support.rs`),
> inheriting `namespace/tests_fault_matrix.rs:20-35`'s documented deviation and
> citing it. This is a recorded limitation of the current tree, not a weakening:
> the `admission/tests_fault_matrix.rs` precedent E0.2 §2.5 calibrates on does
> not transfer, because there the action directory is the *subject* of admission
> rather than a post-admission mutation target.
>
> **And the restoration already has an owner — the E0.2 draft simply did not
> know it.** The same in-code note continues: "Resuming that handoff from durable
> state is **not owned by any landed step**: Step 3.3 considered it and declined
> … the only route to reconstruct an `AdmittedActionV1` in that state is
> `protocol/admission/test_support.rs`, so closing it means widening the frozen
> admission classifier (`GwzM5-8R2DInterfaceFreeze.md` §3.1). **It is item 6 of
> the Phase 3 settle docket.**" (`namespace/tests_fault_matrix.rs:27-33`). So
> R2-E **inherits** the deviation and cites the docket item; it mints no new
> owner. `retain_action_namespace` still fails closed if the reconstructed
> identity is not the resident one (`:33-34`), which is what makes inheriting it
> safe rather than merely convenient.

**[P2-7] cure — DECISION T-C′.** `admission_mutation.rs::execute` takes only
`final_directory: &Dir` (`:79-84`) — the catalog root. It holds neither the
action directory nor the retired root. But of §4.3's ten activated keys, #1-#5
are reads and a flush of the **action directory** — the capability
`RetainedActionNamespaceV1` owns (`namespace_mutation.rs:122-129`, retained
`:135-171`) — and #6-#8 need the **retired root**, which is private to
`RetainedCompletedCatalogV1.retired_actions` (`completed.rs:46`, retained
`:105-109`) and forwarded nowhere.

> **DECISION T-C′ (E0.2b), replacing T-C.** The `terminal.*` sites **split by
> capability**, not by family:
> - **Keys #1-#5** (`authority_reobserve`, `payload_reobserve`,
>   `cleanup_reobserve`, `reservation_reobserve`, `directory_flush`) →
>   `capability/pre_catalog/provider/namespace_mutation.rs`, which already owns
>   the retained action directory. No new capability forward.
> - **Keys #6-#10** (`retired_slot_reserve`, `action_directory_retire`,
>   `retired_directory_reobserve`, `catalog_barrier`, `terminal_revalidate`) →
>   `capability/pre_catalog/provider/admission_mutation.rs`, which owns the
>   catalog root, **plus one new owner-private forward**: the retired-root
>   handle, minted by `RetainedCompletedCatalogV1` in the shape
>   `retain_action_namespace` already uses (`completed.rs:169-181`, "The caller
>   receives the retained namespace capability only — never the catalog handle
>   and never a path"). E3.1 mints exactly one forward and states it.
>
> Both files are already declared `FAULT_INJECTION_SOURCES` entries
> (`:449-452`, `:432-435`), so §6's count is unaffected: **still nine → ten, at
> E2.** Rejected alternative: keep all ten in `admission_mutation.rs` and forward
> the action directory as well — rejected because that would mint *two* forwards
> where one suffices and would put action-directory reads in the catalog-root
> owner.

**[P3] fold-ins, Code axis.** All accepted; the corrected cites are:
(a) anchor identity is `namespace/mod.rs:208`, not `:209`;
(b) the forwarded three are `:208-210 → :221-223`;
(c) the barrier re-reservation is `:787-790`;
(d) the E4-retire clause is `:1273-1276`;
(e) `E01ReachTraces.md:53-57` and `:72-77`;
(f) the revalidation predicate is `completed.rs:250-265`, and `:183-211` is the
head of `revalidate` — the separate gates are at `:227-249`;
(g) `RETIRED_ROOT_BUDGET_V1` is `:62-67` in both places (E0.2 §4.1 and §4.4
disagreed);
(h) `CatalogOccupancyV1::validate` is `:118-142`;
(i) **T-D's evidence is corrected, and the two cite senses are now
distinguished** (State round-2 [P3-R3], which found a third set of numbers and
asked which sense is meant). `:296`/`:307` are `AdmissionFinalPublish` /
`AdmissionFinalReobserve` — a different edge — and are **struck**. Of the
replacements: `admission_mutation.rs:191`, `:199`, `:410`, `:418`, `:431` are
the **`hit()` call sites**, and they are *parameterized* (`hit(faults[N])`,
`hit(install[N])`), so they name no key on their own; the **key references** —
where the five idle keys are actually named — are `:359-361` (`write_faults`'s
`Idle` arm: `AdmissionIdleScratchCreate` / `Write` / `Flush`) and `:381-382`
(`install_faults`'s idle arm: `AdmissionIdlePublish` / `AdmissionIdleReobserve`).
**T-D cites the key references**, because what the determination must show is
that five existing keys already name the return-to-idle edge — not where a
fault fires;
(j) §7's header — fixed at §7.1;
(k) §1.4's `EXISTS` marker has an **empty domain** — all 37 activated rows are
`NEW@`; §1.4 gains one sentence saying so, since it is the correct consequence
of the families being re-reserved precisely because their edges do not exist;
(l) §5.2's nine call sites span **eight files** (`archive.rs` supplies `:97` and
`:126`).

**Code axis — round 2.**

| Id | Finding | Disposition |
| --- | --- | --- |
| [P2-R1] | A third machine-enforced inventory is missing from every convergence obligation | **FIXED** — §6.2 (the two inventories) and **§6.3** (the three obligations rewritten verbatim); §6.4 banks the extra §5 ground |
| [P3-R1] | The retired root's bound would be the *active* constant | **FIXED** — §3.2 ground 3 rewritten with the three real constants and an explicit `MAX_RETIRED_ACTION_DIRS` check made E3.1's duty; §2.4 item 2 aligned |
| [P3-R2] | T-B′'s arm text should repeat the non-`ActiveAction` refusal | **FIXED** — stated in both places, §3.2's shape (`rows` empty) and §2.4 item 2 |
| [P3-R3] | B-6 should carry one line on why its cost is bounded | **FIXED** — §1.4's closing paragraph (`namespace_mutation.rs:202-230` counts no children; the gate is `owner.rs:29-38` at `resume_or_admit`, already unreachable and docketed) |
| [P3-R4] | Cross-parent retirement is latent behind OPEN-B2 | **FIXED** — OPEN-B2 gains the coupling clause (§10 / Appendix B) |

**State axis.**

| Id | Finding | Disposition |
| --- | --- | --- |
| [P1-1] | Injection-source count wrong twice, inconsistently | **FIXED** — §6 |
| [P1-2] | The A1 coexistence gate absent from §5.3; E0.1(b) mis-cited | **FIXED** — §7.1, §7.5, and the sequencing analysis §7.6-§7.8 |
| [P2-1] | §7 mis-titled; §11.3's eight items unaccounted; item 2 dropped | **FIXED** — §7.1 (retitle), §7.3 (eight-row close), §7.7 (item 2 consumed) |
| [P2-2] | O8 denominators unstated and superseded | **FIXED** — below |
| [P2-3] | T-D's ground is not the `staging_directory_flush` holding | **FIXED** — below |
| [P2-4] | §8 row 12's "no key retired" is unsupported | **FIXED** — below |
| [P2-5] | §5.3 omits refusal-ordering / restart legality | **FIXED** — below |
| [P2-6] | §1.1's fixture cite covers 22 of 38 keys | **FIXED** — below |
| [P3-1..8] | Assorted | **FOLDED** — below |

**State axis — round 2.**

| Id | Finding | Disposition |
| --- | --- | --- |
| [P2-R1] | §7.6 maps eight of the §10 table's nine rows; row `:280` dropped | **FIXED** — §7.6 is now the nine-row map; §7.6.1 states the finding in full and proposes **O13**, with the ownership analysis and the §7.8 balance effect |
| [P2-R2] | O3's row denies its catalog-free discharge route; §7.8(a) overstates its cost | **FIXED** — §7.6.2 (corrected row), §7.8(a) (**O1 + O2** blocked, O3 re-owned), §7.8 ground 4 (the single-owner fact promoted in as the fourth leg) |
| [P3-R1] | The census clause is stated, not written verbatim | **FIXED** — the three headers are written out in full below |
| [P3-R2] | T-D's demotion of reading (b) is interpretive; frozen text closes it | **FIXED** — freeze `:795-797` added below |
| [P3-R3] | Which sense do T-D's replacement cites name? | **FIXED** — Code [P3] fold-in (i) above now distinguishes `hit()` call sites from key references and states which T-D cites |
| [P3-R4] | §1.5's preamble contradicts its own table on #9 | **FIXED** — §1.5's preamble now says eight rows restated, with #9's alteration named |

**[P2-2] cure — O8's denominators, stated once.**

> **E0.2 §6.1 gains this paragraph.** The live per-scenario debt is **18 rows**,
> and §12.9(d) states the partition in terms: "**18 UNBOUND rows remain** (10
> progress + 8 archive)" (`GwzM5-8R4bG-Evidence.md:1473`). Under E0.2 DECISION
> §6.3 (no registry rows for archive shapes):
> - **E5.1 owes 10 registry rows** — the progress shapes — not "the 18 registry
>   rows" the plan carries (`GwzM5-8R2E-Plan.md:155-156`, and O8's row at `:48`).
>   §12.7 closure (i)'s "**14** unbound progress shapes" is the **pre-§12.9**
>   figure; post-§12.9 it is **10**. *Corrected on the record, in the form plan
>   §0 used for 67 → 38: the plan's 18 is right as a debt count and wrong as a
>   registry-row count.*
> - **E5.2 owes 8 archive-corpus rows** (by clause, in §12.9's disposition-table
>   shape) **+ 2 PENDING-FIXTURE rows** carried to R2-F.
> - 10 + 8 = 18 ✓.
> - The **other two** NO-FIXTURE scenarios — `B-NOT-STARTED` and
>   `B-PREPARING-EMPTY` — are **not R2-E's**: L6 bullet 1 routes all four of
>   C-2's to the R2-F fixtures/native-evidence lane, and only the two *archive*
>   ones additionally touch E5.2's corpus. Stated so a reader of §6 alone can
>   tell.
> - E5.2's row list is enumerated at **§12.4, "Table B — the 10 archive shapes"**
>   (`GwzM5-8R4bG-Evidence.md:1194`+) — the pointer E0.2 §6 omitted.

**[P2-3] cure — T-D re-grounded on reading (a) alone.** The State axis is right
that the `staging_directory_flush` holding is *one key may serve many
boundaries*, on a **census** ground ("minting a second would move the frozen
census", freeze `:645-648`), and that its converse is not among its holdings —
`durable_leaf.*`'s "14 rows over 11 distinct keys … three boundaries shared"
(freeze `:492-495`) shows the corpus comfortable in the many-to-one direction
and silent on the other. And DECISION C-1 does the opposite thing three times.

> **DECISION T-D, re-grounded (E0.2b).** `terminal.authority_release` never
> gains a fault boundary, on **reading (a) alone**: `RetainedWriteAuthorityV1`
> is deliberately neither `Copy` nor `Clone` (`coordinator/execution.rs:321-327`),
> so release is a move-out — an in-process event no restart can observe. That is
> exactly and only the Phase-3 settle's ground: "not durable edges: no create,
> write, flush, publish, retire, or reobserve occurs at either" (freeze
> `:697-698`).
> **Reading (b) is demoted from "a candidate rejected on one-boundary-one-name"
> to a mis-reading of the key's name, and FROZEN TEXT closes the demotion**
> (State [P3-R2]; without this cite the demotion would rest on interpretation).
> The `terminal.*` non-activation record's own scope sentence, freeze
> `:795-797`, reads: *"Every key of the family names the admitted action
> directory's terminal retirement into the catalog's retired root — §4.4's 'E7's
> Phase-4 half and the terminal retirement edges' — and each needs an
> `AdmittedActionV1` and a production catalog."* The admission record's return
> to `Idle` is the **catalog's re-admission capacity**, not the action
> directory's retirement, so reading (b) is **outside the family's frozen scope
> by the freeze's own words**. That edge is `admission.*`'s and is fully executed
> at five keys, named at `admission_mutation.rs:359-361` and `:381-382`
> (`fault_v1.rs:92-96` for the vocabulary); `terminal.authority_release` does
> not name it.
> **The sentence "One boundary must have one name" is DELETED.** It was inferred
> from a precedent that does not hold it, and it contradicts DECISION C-1 and
> cleanup keys #8/#9. If the lane wants that doctrine it must be minted as a new
> ruling with its own grounds and reconciled with C-1 in the same breath — this
> addendum does not mint it.
> Census sentence corrected: "census stays 165/11, **no key retired**."

**[P2-4] cure — the census clause completed, and the three headers WRITTEN OUT
VERBATIM.** The house form carries the full clause (the `runtime.*` annotation,
freeze `:476`: "165 total, unchanged; no key minted, none retired"; the Phase-3
determination, `:702`: "census stays 165/30, no key retired"), and the three
activation-record headers are the strings that actually enter freeze §3.5 — the
package's primary deliverable — so they are given here in final form rather than
left to reconstruction (State [P3-R1]). **These three paragraphs replace the
E0.2 draft's §2, §3 and §4 record headers as written.**

> **`cleanup.*` activation record (2026-08-27, R2-E Step E0.2 semantics
> amendment as amended at E0.2b; annotates the Step-4.1 non-activation record
> above, which is left as written since this annotation is the sanctioned
> mechanism).** This record binds semantics; the flip to `Executed` is R2-E
> **Phase E1**'s, in the commit that converts the edges, per RemPlan §10's duty.
> **Counts. 165 total, unchanged; no key minted, none retired; `cleanup.*` stays
> 0/11 until E1.2 lands.**

> **`barrier.*` activation record (2026-08-27, R2-E Step E0.2 as amended at
> E0.2b; annotates the Step-4.2 non-activation record above, left as written).**
> This record binds semantics; the flip to `Executed` is R2-E **Phase E2**'s, in
> the commit that converts the edges. **Counts. 165 total, unchanged; no key
> minted, none retired; `barrier.*` stays 0/16 until E2.3 lands.**

> **`terminal.*` activation record (2026-08-27, R2-E Step E0.2 as amended at
> E0.2b; annotates the Step-4.2 non-activation record above and its §4.4
> arm-table resolution, both left as written).** Proposed outcome: **10 of 11
> executed, 1 reserved with a determination** — the family lands as
> `FaultFamilyActivationV1::PartiallyExecuted`
> (`interface_tests/fault_expected_keys.rs:209`), whose per-key siteless proof
> keeps the `Reserved` arm's guarantee for the eleventh. **Counts. 165 total,
> unchanged; no key minted, none retired; `terminal.*` stays 0/11 until E3.2
> lands.**

With these, **§8 row 12 is true as written** — "Unchanged, and restated in each
activation record: 165 total, no key minted, no key retired" — and T-D's own
census sentence reads "census stays 165/11, no key retired."

**[P2-5] cure — §5.3 gains a sixth condition.**

> **§5.3 item 6 (E0.2b).** The refusal is proved to occur **before the
> operation's first durable mutation**, or the partially-mutated state it leaves
> is proved **convergent on restart** — driven, not asserted, in E4.1's own
> rows. DECISION §5.2 relocates the durable-identity refusal from lock
> acquisition to catalog consumption, i.e. **inside** an operation already
> holding `WorkspaceMutatorLock`; whether that consumption precedes or follows
> durable work is an E4-time property today's tree cannot answer
> (`v1_lifecycle/checked.rs:127-134` acquires the lease as an operation prologue
> and holds it across the operation; `archive.rs:97` acquires the lock *before*
> the source-absence check it serialises). It belongs in the precondition set
> rather than being discovered at E4. **With §7.5's item 0, E4.1's precondition
> set is seven, and the preamble becomes "E4.1 may not land until all seven
> hold."**

**[P2-6] cure — §1.1's fixture cite.** Corrected to three ranges:
`fault_expected_keys.rs:105-120` (barrier, 16),
`:151-161` (cleanup, 11), `:162-172` (terminal, 11) — verified for this
addendum. The E0.2 cite `:151-172` proved 22 of the 38.

**[P3] fold-ins, State axis.** [P3-1] the five quote windows (`:766-781`,
`:787-790`, `:645-648`, `:1273-1276`, `E01ReachTraces:72-77`) — corrected, and
they agree with the Code axis's (c)/(d)/(e). [P3-2] §6.2's attribution moves
from §12.9(c)'s `Finalizing` assertion (which archive rows would pass, being
Completed/Aborted) to §12.7 (iii)'s "no registry vocabulary in which an archive
shape could be bound". [P3-3] Appendix B's T-A row gains "**contingent on
OPEN-T1**". [P3-4] A-1 is recast: the checked path does not *consume*
`authority_name`, it **replaces** it with digest-derived slot names, so the
settle's trigger ("when it gives the family its next consumer",
`GwzM5-8R2DSettledTuple.md:651-652`) does not fire in R2-E at all and the honest
disposition is *stronger* than drafted — the legacy name's only consumer remains
the legacy `finish()`, which R2-F deletes; the E4.7 condition is kept as a guard.
[P3-5] E6 loose ends: **E6.3 is now empty and is dispositioned here** — O11's UX
moved into E4.1 and E0.1(a) answered the "reach is not real" branch negative, so
both of E6.3's branches are void; **E6.3 closes with a dated no-work record at
E7.2**. E6.2 retains the two anchor nits (§7.2) plus O10's `AtomicUpgradeFault`
hygiene and the abort rewrite-set bound guard tie. **O12's fourth rider — the
abort rewrite-set bound guard tie (`GwzM5-8R2E-Plan.md:52`, R1.2 (b),
g12:737-739) — is named here for the first time and routed to E6.2**, so the
ledger row reads three consumed at E0.2 + one owned at E6.2. [P3-6]
`gc_archived` is `v1_lifecycle/archive.rs:110-116` with the allowance reason at
`:106-109` — L6's own `archive.rs:110` is correct and E0.2 drifted +2. [P3-7]
§6.3's contract quote is re-cited `:178-183`, or the `:183-184` sentence is
quoted too. [P3-8] the §12.4 pointer is added (see [P2-2] cure).

---

## 9. What this addendum does NOT do

1. **No census move.** 165 total; zero minted; zero retired; 11 + 16 + 11 = 38
   accounted; all three families still `Reserved`
   (`fault_expected_keys.rs:380-394`) at E0 close. Every decision above was
   checked against this, and §1's B-5, §3's T-B′ and §8's T-C′ were each chosen
   partly because they move no census. **The three machine-enforced inventories
   move as follows, stated once so the aggregate is not reconstructed:** the
   declared-source count moves **9 → 10 across E1-E3, once, at E2**;
   `CATALOG_PUBLICATION_CALL_COUNTS` moves **once, at E3** (`admission_mutation.rs`
   2 → 3, the fourteenth production call site) and **not at E1 or E2**; and
   `PROTECTED_SOURCE_TREE_DIGESTS` re-pins at **every** converging commit
   (`…/pre_catalog.rs` at E1, E2 and E3; `…/platform.rs` additionally at E2).
   Details and the per-commit duty lists are §6.
2. **No production code.** No injection site, no `DirentBarrierClass` variant,
   no `DestinationRecheckV1` variant, no `completed_record` edit. §2 *authorizes*
   the T1 widening; it does not write it, and the authorization is scoped to
   E3.1's own reviewed step.
3. **No `completed_record` authorization for B1.** §1.6 records the opposite:
   E2.1 must land the alias lifecycle with the catalog predicates **unchanged**.
4. **No charter change.** §7.8's option (b) is a *proposal*; moving the
   relocation out of R2-F contradicts three standing assignments and is the
   operator's or lane owner's ruling.
5. **No re-derivation of what both axes sustained.** The census, the three
   re-reservation quotes, ten of the twelve §8 targets, the §5 blast-radius
   decision's doctrine and availability grounds, the O8 two-tier mechanism, the
   PARTIAL statement's preservation, and the 38 key-semantic rows other than
   §1.5's eight all stand as drafted.
6. **It does not close OPEN-B2 or OPEN-B3.** Both remain open, non-blocking,
   owned by E2.1 — and both are **strengthened** at round 2. OPEN-B3: the
   reserved target leaf must be **catalog-legal** wherever the target parent is
   catalog-owned, because `interior::exact_row` classifies `.ca1-*` as `Foreign`
   and refuses the whole interior (`interior.rs:414-417`) — stronger than "a
   dotted name in a user-visible directory". OPEN-B2 gains the coupling clause
   at §10.
7. **It does not own O13.** §7.6.1 *proposes* it as a new plan obligation with
   an ownership analysis; minting a ledger row is the lane owner's edit to
   `GwzM5-8R2E-Plan.md` §1, not the addendum's.
8. **It resolves no new open question silently.** §10 lists the three it opens
   and the one clause it adds to a standing row.

---

## 10. New open questions this addendum raises

| Id | Question | Grade | Owner |
| --- | --- | --- | --- |
| **OPEN-B7** | Does the Windows P5 round trip deliver the same dirent ordering when it renames a **freshly created** alias rather than a long-resident anchor? `anchor::round_trip` (`platform/anchor.rs:183-204`) renames a resident file and back; the roaming arm renames a file created moments earlier in the same drive. *Evidence that would settle it:* a native Windows probe in E2.1's own matrix, in the shape of `hard_link_identity_sharing_is_what_the_retirement_rows_assume` (`anchor/tests.rs:308-324`) — measure the platform, do not read a `cfg`. | non-blocking; E2.1 must measure | E2.1 |
| **OPEN-B8** | Under DECISION B-4, are #10 and #12 distinguishable in code at the point of retirement, or does the drive reach one helper with no durable signal of which entry it took? *Evidence:* whether the intent record's resident row (`BarrierIntentActive(ordinal)` vs `BarrierIntentRetired(ordinal)`) plus the in-memory write-handle identity partition the two entries. If not, #12 becomes the restart observation and #10 the edge. | non-blocking; reopens B-4 if negative | E2.1 |
| **OPEN-R1** | Does the relocation need to *move* resident legacy residue, or may it leave it and relocate only future writes? On Unix `finish()` clears the residue (`cleanup.rs:86-94`, `:131-139`, `:166-174`), but Windows retains the permanent durability anchor and accumulating `.ca1-anchor-retired-<n>` names, so a leave-in-place relocation would keep the catalog unobservable on Windows. *Evidence:* whether any Windows workspace can be proved anchor-free before activation — the Phase-4 closure says it cannot (`GwzM5-8R2DPhase4Closure.md:180-190`). Recorded so whoever owns the relocation package inherits the question rather than re-deriving it. | **blocking for whichever lane owns the relocation** | operator's routing (§7.8) |

**And one clause added to a standing row (Code [P3-R4]):**

> **OPEN-B2 gains this coupling clause.** Rows #10 and #12 are same-directory
> renames **only because the target parent is currently pinned to the action
> directory** (`namespace/host.rs:165-171`, `:746-753`), which is what lets them
> route through `RetainedActionNamespaceV1::execute_edge`
> (`namespace_mutation.rs:280-283`, `&self.handle` for both source and
> destination) and is why §6.2(a) can bank "E2 moves no publication caller
> count". **If OPEN-B2 widens the target to a retained managed parent, #10 and
> #12 become cross-parent renames**, `execute_edge` no longer serves them, and
> three things open at once: `CATALOG_PUBLICATION_CALL_COUNTS` moves at E2 after
> all, a new sealed-primitive call site appears in `barrier_mutation.rs`, and the
> rename-domain question (`namespace_mutation.rs:158`) applies to a pair of
> parents rather than one. **E2.1 must see this coupling before it answers
> OPEN-B2**, because the answer changes E2.3's convergence obligation (§6.3).

---

## Appendix A — the corpus-wide citation-drift note, ready to file

Offered as a dated note the lane owner may file verbatim into
`GwzM5-8I2CompatibilityContract.md`'s neighbourhood or into the checkpoint. Its
content is the State axis's §6.1 adjudication, which I re-verified.

> **Citation-drift note (2026-08-27, R2-E lane, filed at E0.2b).**
>
> **Mechanism.** gwz-dev commit **`4b9f078`** ("A1 activation commit:
> operator-signed record set lands", 2026-08-25 02:13:43 +1000) inserted a
> single dated annotation into `dev-docs/GwzM5-8I2CompatibilityContract.md` —
> one blank line plus an 18-line dated blockquote — for a diffstat of exactly
> **+19** on that file and no other. Frozen §4 text ends at `:81`; what was
> `:83` is now `:102`.
>
> **Extent.** The drift is **+19 for every line below the 2026-08-25 annotation
> and ZERO for lines `:1-:82`.** It is *not* a uniform whole-file offset, and
> the E0.2 draft's "uniform +19-line drift" is true only of the two passages at
> issue.
>
> **Corrected current line numbers**, verified directly rather than by
> arithmetic:
> - the whitelist passage ("A1 deliberately whitelists only seven
>   one-member-workspace…" … "…are not A1 migration rules.") — **`:136-144`**
>   (9 lines, matching R4b-G's 9-line `:117-125`);
> - the "Zero whitelist matches is not an error…" passage — **`:178-184`**
>   (7 lines, matching R4b-G's 7-line `:159-165`).
>
> **Six documents still carry the stale pair**, one of them on R2-E's own
> inherited register:
> - `GwzM5-8R2DSettledTuple.md:702` **and `:706`** (the C-1 row);
> - `GwzM5-8R4bG-Evidence.md` §12.7 (`:1298-1305`), **§12.9(d) (`:1469`)**, and
>   `:1187` (which cites `:123`, now **`:142`**);
> - `GwzM5-8R4bG-ReviewEvidence.md:119`;
> - `GwzM5-8R4bG-ReviewCorrectness.md:546`;
> - `GwzM5-8M5bNoFfDesign.md:112`, `:166`, `:234`, `:321`, `:375`;
> - `GwzM5-8M5bNoFf-ReviewCode.md:58`, `:79` (citing `:145` → now **`:164`**,
>   and `:163-165` → now **`:182-184`**).
>
> **THE CITING RULE, binding on E5.2 and on any lane citing the compatibility
> contract: cite content-anchored, not line-anchored.** Give the § number plus a
> quoted anchor phrase, with the line number offered as a convenience and marked
> as of a stated date — e.g. *"`GwzM5-8I2CompatibilityContract.md` §5, 'Zero
> whitelist matches is not an error … byte-preserving archival' (`:178-184` as of
> 2026-08-27)"*. The contract is a **live annotatable frozen document**: its own
> annotation mechanism inserts dated blockquotes above frozen text and shifts
> every line below them, so a bare line cite decays by construction at the next
> annotation.
>
> **What must NOT be done:** silently re-pointing R4b-G's, the settled tuple's,
> or M5b's existing cites. Those are dated records and are **left as written**,
> under the same "left as written / this annotation is the sanctioned mechanism"
> discipline the R2-E amendment applies to the freeze. This note is the
> resolution mechanism: a reader meeting a stale number resolves it here rather
> than re-deriving it.

---

## Appendix B — revised registers

**OPEN register (replaces E0.2 Appendix A):**

| Id | Row | Grade | Blocking | Owner |
| --- | --- | --- | --- | --- |
| ~~OPEN-B1~~ | **CLOSED at E0.2b** — cure is DECISION B-5; `completed_record` unchanged | — | no | — |
| ~~OPEN-C1~~ | **STRUCK** — premise refuted (`authority_record_binding.rs:486`) | — | no | — |
| **OPEN-T1** | **AUTHORIZED at E0.2b** — Class-2 widening at three gates (§2); implementation is E3.1's precondition step | design | E3.1's own precondition, no longer a phase blocker | E3.1 |
| **OPEN-B2** | The barrier target is pinned to the action directory (`namespace/host.rs:165-171`) — **carries the §10 coupling clause: widening it makes #10/#12 cross-parent, moves `CATALOG_PUBLICATION_CALL_COUNTS` at E2, and changes E2.3's convergence obligation** | scope | no | E2.1 |
| **OPEN-B3** | The reserved target leaf's grammar — **strengthened**: must be catalog-legal wherever the target is catalog-owned (`interior.rs:414-417`) | hygiene / correctness | no | E2.1 |
| **OPEN-B7** | Windows round trip over a freshly created alias (§10) | evidence | no; E2.1 must measure | E2.1 |
| **OPEN-B8** | #10 / #12 distinguishability (§10) | design | no; reopens B-4 | E2.1 |
| **OPEN-R1** | Relocation must move resident residue (§10) | design | **yes, for the relocation package** | operator's routing |

**Decision register (deltas to E0.2 Appendix B):**

| Id | Decision | Status |
| --- | --- | --- |
| C-3 | worklist scratch = `RecordScratch`, an unconsumed slot | **SIMPLIFIED** — no ordering condition |
| T-A | retired child reuses the derived `ActiveAction` name | **UNCHANGED**, now marked *contingent on OPEN-T1* |
| ~~T-B~~ | field generalization | **REPLACED by T-B′** |
| **T-B′** | a new `DestinationRecheckV1` variant carrying a second observation input (Class 1) | **NEW** — §3 |
| ~~T-C~~ | all terminal sites in `admission_mutation.rs` | **REPLACED by T-C′** |
| **T-C′** | terminal sites split by capability: #1-#5 → `namespace_mutation.rs`, #6-#10 → `admission_mutation.rs` + one retired-root forward | **NEW** — §8 |
| T-D | `terminal.authority_release` reserved with determination | **RE-GROUNDED on reading (a)**; "one boundary must have one name" deleted; the demotion of reading (b) now closed by frozen text (freeze `:795-797`) |
| **B-4** | #10 and #12 are two boundaries sharing one helper | **NEW** — §1.5 |
| **B-5** | the target alias is a fresh independent copy; not a hard link, not a move | **NEW** — §1.3 |
| **B-6** | the alias is created on every platform | **NEW** — §1.4 |
| A-1 | the `authority_name` weigh | **RE-FRAMED** — the checked path replaces rather than consumes; trigger does not fire in R2-E |
| O6 | observe-or-refuse | **COMPLETED** — read-side refusal added; witness shape corrected to the `observe_admission` inverse |
| E4.1 blast radius | option (ii) | **UNCHANGED**; precondition set grows from five to **seven**; a fifth ground banked at §6.4 (`workspace_mutator_lock.rs` is byte-pinned) |
| O8 | two-tier mechanism | **UNCHANGED**; denominators now stated (10 / 8 / 2 / 2) |

**Proposed ledger addition (not minted here — §9 item 7):**

| Id | Proposed obligation | Source | Proposed owner |
| --- | --- | --- | --- |
| **O13** | *The v1 store's raw-writer clause* — convert the v1 checked store/root/bundle paths per §10 row `:280`'s frozen ordering, and discharge its "no legacy raw writer" clause, whose test-gate expired at A1 | `ConsumerCheckpoint:280`; State round-2 [P2-R1] | **substantive half → E4.2/E4.3 as a scope clause** (same store); **raw-writer pin → an E4.2 precondition pin, landing now**, with a dated accepted-residual record covering A1 → E4.2 (§7.6.1) |

---

## Appendix C — round-2 conditions, closed; and what the E0 landing verifies

**Round 2 closed GO-conditional (Code: 1 P2, 4 P3) and CONDITIONAL GO (State: GO
on the semantics object, NO-GO on §7.6-§7.8 as decision-support; 2 P2, 4 P3).
Every condition of both sets is applied above. The map, for the landing check:**

| Condition | Applied at |
| --- | --- |
| Code [P2-R1] — the third machine-enforced inventory in all three convergence obligations | **§6.2** (the two inventories, with the E1/E2/E3 movement analysis) and **§6.3** (§2.5 / §3.6 / §4.5 rewritten verbatim); **§6.4** banks the extra §5 ground; the aggregate is stated once at **§9 item 1** |
| Code [P3-R1] — the retired root's bound would be the active constant | **§3.2** ground 3 (three real constants named; explicit `MAX_RETIRED_ACTION_DIRS` check made E3.1's duty) and **§2.4** item 2 |
| Code [P3-R2] — repeat the non-`ActiveAction` refusal in T-B′'s arm | **§3.2** shape (`rows` empty) and **§2.4** item 2 — said once in each place |
| Code [P3-R3] — B-6's bounded cost | **§1.4** closing paragraph |
| Code [P3-R4] — cross-parent retirement latent behind OPEN-B2 | **§10** coupling clause; **Appendix B** OPEN-B2 row |
| State [P2-R1] — the nine-row map, row `:280`, O13 | **§7.6** (nine rows) and **§7.6.1** (the finding in full, O13 proposed, the ownership analysis, the §7.8 balance effect) |
| State [P2-R2] — O3's row, §7.8(a)'s cost, ground 4 | **§7.6.2** (corrected row), **§7.8(a)** (O1 + O2 blocked; O3 re-owned), **§7.8 ground 4** (the fourth leg promoted in) |
| State [P3-R1] — the census clause verbatim | **§8**, the [P2-4] cure — three headers written out in full |
| State [P3-R2] — close T-D's demotion with frozen text | **§8**, the [P2-3] cure — freeze `:795-797` quoted |
| State [P3-R3] — which sense do T-D's cites name | **§8**, Code fold-in (i) — `hit()` call sites vs key references, and which T-D uses |
| State [P3-R4] — §1.5's preamble vs its table on #9 | **§1.5** preamble — eight rows restated, #9's alteration named |

**What the E0 landing verifies, in order.** (1) The three convergence
obligations at §6.3 name every inventory each converging commit must move —
this is the one condition that would red CI rather than corrupt semantics.
(2) The three activation-record headers at §8 are the strings that enter freeze
§3.5; they are now final text, not instructions. (3) §7.6 is nine rows and §7.8's
accounting is O1 + O2 blocked with O3 re-owned. (4) The census is unmoved: 165,
zero minted, zero retired, 38 accounted, three families still `Reserved`.
(5) Nothing in the pair authorizes a `completed_record` edit for B1, and the T1
authorization is scoped to E3.1's own reviewed precondition step.

**The three things this pair hands the operator as decisions, not findings:**
the §7.8 charter question (option (b) vs (a)); O13's minting and its ownership
(§7.6.1); and OPEN-R1's routing, which follows whichever lane takes the
relocation.
