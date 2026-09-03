# R2-D Phase 4 Step 4.3 — coexistence closure, the dirent-barrier residual, and the Phase 4 settle items

Date: 2026-08-23
Object: gwz-core, base `51a9cba` (Step 4.2 landed).
Step: R2-D Phase 4 **Step 4.3** — `GwzM5-8R2D-Plan.md` §4 Step 4.3, the final
step of Phase 4's step list.

This document is the sanctioned record for the two halves plan §4 Step 4.3
names, plus the settle-assigned items routed here. §2 is **A1-gating** and
carries a decision the lane owner rules on at review; §3 records a residual
verified closed in code; §4-§6 are the settle items and the two Step-4.2
records fit to lift into the freeze.

---

## 1. What this step closes, at a glance

| Item | Source | Disposition |
| --- | --- | --- |
| (a) Coexistence acceptance criterion | plan §4 Step 4.3(a); audit P3-3 (`GwzM5-8R2C2PublicationAudit.md:80-89`); owner decision §7.2 | **Decision packet + both record variants** (§2). Recommendation: **quarantine/relocation**, following §7.2's adopted direction. |
| (b) Dirent-barrier resume-window residual | plan §4 Step 4.3(b); `GwzM5-8R2C2DirentBarrier-ReviewState.md:126`, routing `:393-395`; owner decision §7.5 | **Correction (a) — the in-line fix — is already implemented and tested.** Verified, not re-implemented (§3). |
| Settle item 8 — seam-level action binding | checkpoint PHASE 3 SETTLED disposition (8) | **Implemented** with the paired consumer obligation and the seam-level negative row (§4). |
| Settle item 7 — blanket `allow(dead_code)` | checkpoint PHASE 3 SETTLED disposition (7) | **Narrowed**; full exposed inventory reported (§5). |
| Step 3.3 round-2 P3s | checkpoint PHASE 3 SETTLED | **Both fixed** (§6.1). |
| Step 4.2 [P3-6] racing-drive row | `GwzM5-8R2DStep42-Review.md` R2.6 | **Row added** (§6.2). |
| Step 4.2 [P3-7] `Invalid` no-exit refusal | `GwzM5-8R2DStep42-Review.md` R2.6 | **Derivation record filed** (§6.3), liftable. |

---

## 2. (a) The coexistence acceptance criterion — decision packet

### 2.1 The criterion, restated from the plan

> after this phase, either the legacy leaf grammar inside `.gwz/checked-artifacts`
> is admission-owned/grammar-legal **or** a reviewed quarantine/relocation
> decision is recorded **before any production catalog activation**
> (plan §4 Step 4.3(a)).

### 2.2 The physical fact, re-verified at `51a9cba`

The legacy leaf writers' private root **is** the catalog Final directory. Both
derive the same path: `policy.rs`'s `private_parent()` →
`CatalogPrivateNameV1::Final` → `checked-artifacts`, under
`CatalogPrivateRootV1::{Workspace,GitDirectory}`. There is one directory, and
two grammars claim it.

**Grammar A — the catalog interior** (`provider/interior.rs`). `exact_row()`
admits exactly two shapes and refuses everything else with
`ambiguous("catalog interior", "catalog directory contains an unowned child")`:
the ten `InfrastructureSlotV1::ALL` names, and `ActiveAction(digest)` rows.
The root budget is `MAX_ROOT_ENTRIES = MAX_ACTIVE_ACTION_DIRS + 10 = 74`
(`protocol/bounds.rs:21-23`) — note Phase 1's C-3 observer extension
superseded freeze §4.4 Class 2's cap *value* (`MAX_INTERIOR_ENTRIES = 10`
→ 74); the **zero-headroom property itself stands** — every one of the 74
entries is accounted with per-entry name-byte budgets, so there is still no
headroom for a foreign family, which strengthens the Option 2 case rather
than weakening it.

**Grammar B — the legacy leaf family**, as it stands after Steps 4.1 and 4.2:

| Name | Lifetime | Length |
| --- | --- | --- |
| `ca1-<family64>-<action64>.authority` | per in-flight action | 143 |
| `ca1-<family64>-<action64>-<id32>.source` | per in-flight action | **173** |
| `ca1-<family64>-<action64>-<id32>.goal` | per in-flight action | 171 |
| `.ca1-<family64>-<action64>-authority.scratch` | staging; consumed by its publish | 152 |
| `.ca1-<family64>-<action64>-goal.scratch` | staging; consumed by its publish | 147 |
| `.ca1-durability-anchor-<id32>` | **permanent by design** (Windows) | 55 |
| `.ca1-durability-anchor-<id32>.roundtrip` | inside a barrier window | 65 |
| `.ca1-anchor-scratch-v1` | crash residue | 22 |
| `.ca1-anchor-retired-<n>` | **permanent**, one per foreign stranding | 21+ |
| `.ca1-scratch-<kind>-<hex32>` | pre-4.2 nonce orphans, tolerated | 55 |

Not one of these is grammar-legal under A. So a completed catalog in that
directory becomes **unobservable** the moment any of them is resident:
`interior::observe` → `exact_row` → `ambiguous`, which is the predicate
`retain_completed_catalog` and the retained-catalog revalidation both rest on.

### 2.3 The asymmetry that decides it — new since adoption

The audit called this "interior ambiguity … the moment a catalog completes".
Re-verified at `51a9cba`, it is sharper than that, and in a direction the
adoption record could not have known:

- **Off Windows the collision is transient.** `prepare_private` is `Ok(())`, so
  no anchor exists; after a clean `finish()` the family's three aliases are
  removed and the two staging names were consumed by their publications, so the
  directory returns to empty. Only in-flight actions and crash residue collide.
- **On Windows the collision is permanent and unconditional.** The durability
  anchor is resident after the first checked write and is *permanent by design*
  — the E10/E14 annotation states it outright ("the anchor is permanent by
  design and `finish()` never removes it"), which is the whole reason the
  `AnchoredPrivateArea` class exists. A Windows workspace that has ever written
  a checked artifact can therefore **never** host an observable catalog at that
  path, no matter how quiet the tree is.
- **Step 4.2 added a second permanent shape**, `.ca1-anchor-retired-<n>`, whose
  count is bounded by real crash occurrences rather than by a constant. A
  grammar extension would have to admit an open-ended family into a root whose
  budget is 74 entries with per-entry name-byte accounting.

### 2.4 The two options, on evidence

**Option 1 — convert-in-place** (legacy leaf grammar becomes admission-owned).
Widen grammar A to admit grammar B: extend `CatalogRootRowClassV1` and
`exact_row`, and charge the legacy names against the root budget.

- *Cost, correctness:* it permanently widens the catalog's closed interior
  grammar to admit a legacy shape that R2-F is chartered to **delete**. It also
  re-opens the exact-evidence contamination class the E10/E14 annotation
  records as diagnosed and fixed: admission refuses an action directory whose
  `extra_children` is nonzero, and the anchor is precisely such a child.
- *Cost, budget:* `.source` at 173 bytes against `MAX_NAME_BYTES = 255` and
  `METADATA_ACCOUNTING_BYTES_PER_ENTRY = 256` is affordable per entry, but the
  open-ended retired-anchor family has no natural bound to charge.
- *What it does not fix:* the MAX_PATH exposure stays exactly where it is.
- *Status:* the **edge** half of this option is already done — Steps 4.1 and 4.2
  converted E18-E22 in situ through P1, which freeze §5 decision 2 describes as
  convert-in-place's own path. What remains of Option 1 is the *grammar* half,
  and only that.

**Option 2 — quarantine/relocation** (move the legacy private area out of the
catalog Final directory).

- *Benefit:* the two grammars stop sharing a directory, so grammar A stays
  closed and R2-F's deletion of the legacy family removes a directory rather
  than un-widening a frozen grammar.
- *Benefit:* it retires the MAX_PATH exposure the checkpoint tracks (`~173-char
  ca1-* names; the private-area relocation option under .git/ would retire it —
  candidate for R2-F scope`, checkpoint :1022-1024). Step 4.2 did **not** widen
  that exposure: the longest name is still the 173-byte `.source`, and 4.2's
  deterministic staging name is 152.
  *[FALSIFIED 2026-09-01, R2-F R1.3 records train (trace
  `GwzM5-8R2F-RelocationTrace.md` §5.2; constants pinned at R1.1, gwz-core
  `027da5b`): the relocation does NOT retire this exposure. Under `.git/` the
  same leaf is four characters LONGER (197 → 201 beyond root); 160 of the 173
  name characters are the 64+64+32 hex triple, so retirement can only come
  from shortening the names, not from moving them. The landed relocation
  moved only the CATALOG's name (`catalog-final`); the legacy area holding
  the `ca1-*` names keeps `.gwz/checked-artifacts` by design (one new name,
  two directories). The checkpoint anchor has also drifted: the tracked text
  now sits at checkpoint :3192-3194.]*
- *Cost:* the move is a durable relocation of live production state, needing its
  own package.
- *Where it executes:* **not here.** Freeze §5 decision 2 already settles this:
  "the move itself executes on the R2-F-candidate relocation package, on the
  same P1 family, not inside R2-D … Phase 4.3 only *decides* compatibly", and
  plan §5 item 7 keeps MAX_PATH and relocation execution out of this lane.

### 2.5 What §7.2 already adopted, and whether anything moves it

Plan §9 records the adopted §7.2 decision verbatim:

> **Coexistence (Phase 4.3) — direction: quarantine/relocation preferred**,
> because it also retires the MAX_PATH exposure (checkpoint :144-147);
> convert-in-place remains live as fallback. The binding reviewed decision is
> made at Phase 4.3 per the plan text — in any case before any production
> catalog activation — and Step 0.1's freeze memo carries the Track-P
> analysis for both options so the 4.3 decision is evidence-based, not
> re-scoped.

So §7.2 carries an adopted **direction** and explicitly reserves the **binding**
decision for this step. Nothing found at `51a9cba` moves the direction, and two
things reinforce it: the Windows permanence of the anchor (§2.3), which makes
Option 1's grammar widening unconditional rather than transient, and the
open-ended retired-anchor family Step 4.2 added.

### 2.6 RECOMMENDATION — flagged for the lane owner's ruling

> **Adopt Option 2, quarantine/relocation**, as §7.2's adopted direction
> already prefers, with execution pinned to the R2-F relocation package and an
> explicit A1 gate: **no production catalog activation at
> `.gwz/checked-artifacts` until the relocation lands.** Option 1 is not
> refused on the merits of its edge half — that half is already built — but its
> remaining grammar half would widen a frozen closed grammar to admit an
> open-ended legacy family that R2-F deletes, and would not retire MAX_PATH.

Both record variants are drafted below so the landing lifts the chosen one.

### 2.7 Record variant A — quarantine/relocation (recommended)

> **Audit P3-3 coexistence acceptance criterion — CLOSED by decision
> (2026-08-23, lane owner, at the Step 4.3 landing).** The binding reviewed
> decision plan §7.2 reserved for Phase 4.3 is **quarantine/relocation**: the
> legacy leaf private area is relocated out of the catalog Final directory, so
> the catalog interior grammar stays closed and the legacy family's names never
> become catalog rows.
>
> The criterion's own alternative — "the legacy leaf grammar … is
> admission-owned/grammar-legal" — is **declined**, on three facts re-verified
> at `51a9cba`: (i) grammar-legalising it would permanently widen
> `exact_row`'s closed interior grammar to admit a shape R2-F is chartered to
> delete; (ii) the family now includes two *permanent* Windows names — the
> durability anchor, permanent by design per the E10/E14 annotation, and Step
> 4.2's `.ca1-anchor-retired-<n>`, whose count is bounded by real crash
> occurrences rather than a constant — so on Windows the collision is
> unconditional rather than transient, and the admitted family would be
> open-ended against a 74-entry root budget; (iii) admission refuses an action
> directory with nonzero `extra_children`, which is the exact-evidence
> contamination class the E10/E14 annotation records as already diagnosed and
> fixed.
>
> **Execution is not R2-D's.** Freeze §5 decision 2 already assigns the move to
> the R2-F-candidate relocation package "on the same P1 family, not inside
> R2-D", and plan §5 item 7 keeps MAX_PATH and relocation execution out of this
> lane; Phase 4.3 decides compatibly and no more. What this step therefore
> installs is the **gate**, and it is A1-gating: *no production catalog
> activation at `.gwz/checked-artifacts` until the relocation lands.* That gate
> is already fail-closed in code rather than only in prose —
> `recover_or_create` has no production caller (re-verified at `51a9cba`), and
> plan §5 item 2 forbids giving it one in R2-D.
>
> The decision also retires the tracked MAX_PATH product exposure (checkpoint
> :1022-1024, "~173-char `ca1-*` names; the private-area relocation option
> under `.git/` would retire it"). Step 4.2 did not widen it: the longest name
> is still the 173-byte `.source`, and the deterministic staging name Step 4.2
> introduced is 152.

*[The retirement clause of the record above is FALSIFIED — 2026-09-01, R2-F
R1.3, same note as §2.4: measured at the R1.1 landing (gwz-core `027da5b`),
the relocation retires nothing here (+4 chars under `.git/`; 160 of 173 chars
are hex), and the landed design deliberately left the `ca1-*`-bearing legacy
area at `.gwz/checked-artifacts`. The exposure is live product surface owned
by the name-shortening class, not by relocation. The rest of the record —
grammar separation, the A1 gate — stands and executed as written.]*

### 2.8 Record variant B — convert-in-place (the fallback, if ruled)

> **Audit P3-3 coexistence acceptance criterion — CLOSED by decision
> (2026-08-23, lane owner, at the Step 4.3 landing).** The binding reviewed
> decision plan §7.2 reserved for Phase 4.3 is **convert-in-place**: the legacy
> leaf grammar inside `.gwz/checked-artifacts` becomes admission-owned, and the
> catalog interior grammar is extended to admit it.
>
> The **edge** half is already landed: Steps 4.1 and 4.2 converted E18-E22 in
> situ through P1, which is what freeze §5 decision 2 describes as this
> option's path. What this decision additionally commits to is the **grammar**
> half, which is *not* implemented by this step and is an explicit obligation on
> the package that first activates a production catalog: extend
> `CatalogRootRowClassV1` and `provider/interior.rs`'s `exact_row` to classify
> the legacy family as owned rows, and charge them against `MAX_ROOT_ENTRIES`
> (74) and the per-entry name-byte accounting.
>
> Three consequences are accepted with the decision and must be carried into
> that package: (i) the widened grammar admits a shape R2-F is chartered to
> delete, so R2-F must un-widen it; (ii) two of the family's names are
> permanent on Windows — the durability anchor and
> `.ca1-anchor-retired-<n>`, the latter open-ended in the number of real crash
> occurrences — so the budget charge cannot be a constant; (iii) admission's
> `extra_children: 0` refusal must be reconciled with a resident anchor, which
> is the exact-evidence contamination class the E10/E14 annotation records as
> diagnosed and fixed. The MAX_PATH exposure (checkpoint :1022-1024) is **not**
> retired by this option and stays an R2-F candidate on its own.
>
> The A1 gate is unchanged either way: no production catalog activation until
> the chosen half is complete. It is fail-closed in code —
> `recover_or_create` has no production caller at `51a9cba`, and plan §5 item 2
> forbids giving it one in R2-D.

---

## 3. (b) The dirent-barrier resume-window residual — verified CLOSED in code

Plan §7.5 left this conditional: "in-line fix at 4.2/4.3 if it fits the step
budget, else the bounded §6-style erratum + R2-F power-loss pin". **Neither
branch needs to be taken now: correction (a) is already implemented, and it is
the in-line fix the residual specified.** Step 4.3 verifies and records it
rather than re-doing it.

The residual (`DirentBarrier-ReviewState.md:126-188`) is that a drive
interrupted at `catalog_bootstrap.scratch_write`/`.scratch_flush` restarts,
classifies past the scratch edge, and reaches `Complete` without any process
having flushed the containing root — so a strict explicit-parent-fsync model
allows the `.gwz` dirent to revert. Its **required correction (a)** was: "issue
the same idempotent root barrier once in the common Ready-edge mutation
prologue (the `execute_owner_*` dispatch layer, keeping preflight read-only)".

Verified at `51a9cba`, by inspection of the tree rather than of the comment
that claims it:

| Claim | Evidence |
| --- | --- |
| The prologue exists | `capability/pre_catalog.rs` `ready_edge_prologue()` — revalidate, then `provider::finish_ready_edge_root_barrier(&self._retained_root)` |
| It is idempotent and root-scoped | `provider/mutation.rs` `finish_ready_edge_root_barrier` → `finish_private_parent_edge(retained.root().handle())` |
| **Every** Ready owner edge passes through it | counted per function: `execute_owner_scratch`, `execute_owner_publish_active`, `execute_owner_prepare_or_rewrite_staging`, `execute_owner_publish_final`, `execute_owner_retire_active`, `execute_owner_complete` — **1 call each**. `execute_owner_create_and_retry` has none, correctly: it is the edge that *creates* the private parent, and `create_git_private_parent` issues the barrier itself |
| Preflight stayed read-only | the barrier is called only from the owner-mutation prologue, not from `preflight_catalog_target` |
| The boundary is keyed | `catalog_bootstrap.ready_edge_root_flush` (`fault_v1.rs:55`), announced at the barrier |
| The specified regression test exists | `catalog/bootstrap/tests.rs` `resume_after_scratch_interrupt_reissues_the_root_barrier_before_completion` — crash at `CatalogBootstrapScratchWrite`, then a **non-panicking callback armed at the barrier's key on the resume drive**, asserting the slot is consumed. That is exactly the mechanism the review prescribed |
| Windows | unchanged deliberate no-op, per the same §6 durability model the residual's own review accepted |

**Landed at** gwz-core `9939b02` ("Land the filter-policy (D1+D2) and R2-F
missing-tests packages") — the barrier, its key, and its regression test
together.

> **Dirent-barrier resume-window residual — CLOSED by in-line fix (record,
> 2026-08-23, Step 4.3).** Plan §7.5's conditional resolves to its **first**
> branch: correction (a) of `DirentBarrier-ReviewState.md:175-181` is
> implemented, so neither the §6-style erratum nor an R2-F power-loss pin is
> filed. `ready_edge_prologue` issues the idempotent containing-root barrier at
> the head of all six Ready owner edges, making `Complete` unreachable without
> a root barrier issued by the completing process; `execute_owner_create_and_retry`
> is correctly exempt because it is the edge that creates that parent. The
> boundary carries its own key (`catalog_bootstrap.ready_edge_root_flush`) and
> the review's prescribed regression test drives it on the resume drive. Landed
> at `9939b02`; verified against the tree, not against its comment, at
> Step 4.3. The residual's companion R2-F item — native power-loss semantics —
> is unaffected and stays where it was.

---

## 4. Settle item 8 — seam-level action binding

**The defect class.** `CheckedAuthorityObservationV1::owner_issue` sets
`action_digest: reservation.action_digest()` — it *copies the binding field out
of the caller's argument*. That is the caller-supplied-restatement class the
checkpoint records as having fired twice (Step 2.4 [P1], Step 3.3 [P1]). An
observation whose payloads were streamed under action B but which is issued
against action A's reservation therefore mints a record that agrees with
itself, and only a downstream consumer that remembers to check provenance can
refuse it.

**The fix, at the root.** `AuthorityObservationFactsV1` gains
`action_digest`; `AuthorityFactsIssuerV1::issue` takes it; and
`CheckedAuthorityObservationOwnerV1::observe` refuses when it disagrees with
the resident reservation, beside the `request_owner_binding` check already
there. The production transaction supplies `self.proof.action()` — the digest
the streaming capability actually read through.

**The paired consumer obligation, stated at the seam** (the reviewer's
refinement, adopted): the action digest's provenance *rests on slot-name
derivation*. Unlike the identities beside it there is no durable field inside a
payload saying "I belong to action X"; what makes the digest trustworthy is
that `observe_streamed_payloads` builds every slot name it streams from the
digest it was given, so a proof's bytes are physically the bytes of that
digest's rows. The obligation — pass the digest you derived your reads from,
never one a caller handed you — is written on `AuthorityFactsIssuerV1::issue`
itself, because that signature is the only place a future transaction author
will look.

**Evidence.** `interface_tests/durable_records.rs`
`an_observation_carrying_another_actions_digest_is_refused_at_the_seam`: two
reservations sharing one `request_owner_binding` (one merge owner, two actions
— which is what makes the case realizable), an observation issued against the
target while carrying the other's digest, refused **at the seam** with the
seam's own detail pinned; and the honest pairing still issues, so the gate is a
pairing check rather than a blanket refusal. It needs no help from the Step-3.3
consumer gate.

The Step-3.3 cross-action row keeps its own job: it now passes the *target's*
digest (which the seam accepts) over the *other* action's provenance identity
(which the seam does not inspect), so it isolates the consumer gate exactly as
before. The two smuggles and the two gates are now distinct and both are
executed.

---

## 5. Settle item 7 — the blanket `allow(dead_code)` narrowing

**Method.** All eleven module-level `allow(dead_code)` attributes in
`checked_artifact/mod.rs` were deleted and the tree recompiled, giving the true
inventory of what the eleven module-level allows were hiding: **481 unique dead
items across 50 files**. That figure is the *blanket-hidden subset*, not the
global measure: a `--force-warn dead_code` sweep over the whole subsystem
(Step-4.3 review [P2-1]) counts **1657 dead-code spans across 85 files**, the
difference being items already behind narrower allows (for example
`coordinator/execution.rs`'s 26 R2-E-gated items). Both figures are recorded
in the Phase 4 settle record for Step 5.1's LOC/cohesion audit.
Each remaining allow was then judged against the module it covers.

**Result — four of eleven narrowed or deleted, and the tree is warning-clean:**

| Module | Dead items | Action |
| --- | --- | --- |
| `admission` | **0** | allow **deleted** — its reason ("Step 0.1 freezes the admission owner seam before phase 1 implements its driver") was falsified by Phase 1 |
| `catalog_names` | **0** | allow **deleted** |
| `catalog` | 54, all in three named children | allow **moved inward** to `mod bootstrap/classifier/enumeration` in `catalog.rs`; `catalog/scratch.rs` needs none |
| `coordinator` | 31, all in one child | allow **moved inward** to `mod identity` in `coordinator/mod.rs`; `execution` already carried its own, `schedule` needs none |
| `bootstrap` | 82 | kept, reason refreshed |
| `capability` | ~90 | kept, reason refreshed |
| `entry` | 14 | kept, reason rewritten (it named "production consumer activation"; the accurate gate is R2-E) |
| `fault_v1` | 7 | kept, reason refreshed |
| `leaf` | 4 | kept, reason refreshed |
| `namespace` | 51 | kept, reason refreshed |
| `protocol` | ~144 | kept, reason refreshed |

The six identical reasons that read "R1 freezes interfaces before R2 consumer
conversion" now read "frozen interface awaiting R2-E consumer conversion (plan
§5 item 1); narrowed at Phase 4 Step 4.3 to the subtrees that still carry one",
which is true of the tree as it stands.

**Genuinely-dead items the narrowing exposes: none outside the frozen
surface.** Every one of the 481 is R1/R2-frozen interface whose first consumer
is R2-E, and the four narrowings above are exactly the places where that had
stopped being true. This is a different outcome from the two findings the Step
3.2 review caught, and it is reported as such rather than dressed up: the
narrowing found no orphan to delete, it found two stale covers and two
over-broad ones. The 481-hidden / 1657-global surface is the honest measure of how much
of this subsystem still awaits R2-E, and it belongs in Step 5.1's LOC/cohesion
audit as a number rather than as a feeling.

---

## 6. The remaining routed items

### 6.1 Step 3.3 round-2 P3s

- **Cross-action row detail string.** `an_observation_streamed_under_another_action_must_not_authorize`
  asserted only `is_err()`. It now pins the gate's own detail —
  `"the authority observation was streamed under another action's retained
  directory"` — so the row proves *which* gate fires. This mattered immediately:
  settle item 8 added an earlier refusal on the same path, and without the pin
  the provenance gate could have rotted untested behind it.
- **Stranded `leaf_digests` doc line.** The first line of its doc comment ("A
  leaf request of the given operation, over a fixed workspace-relative path.")
  belonged to `leaf_request`; deleted.

### 6.2 Step 4.2 [P3-6] — the racing-drive row lands; native execution arrives with the landing's Windows matrix (the row skips on macOS via the hard-link-identity guard)

`a_racing_drive_that_takes_the_ordinal_first_loses_and_the_next_drive_converges`
plants `retired_name(0)` from a hook at `BeforeAnchorAliasRetirement` — after
this drive's survey computed the ordinal and before its edge reaches it. The
no-replace publication refuses, the loser publishes nothing, and the next drive
re-surveys and takes ordinal 1. The row is real contention (the hook fires between the survey and the
edge); on this host it exercises the refusal branch, and the retirement
branch executes natively at the landing's Windows matrix run.

### 6.3 Step 4.2 [P3-7] — derivation record for `survey`'s `Invalid`, liftable

> **`AnchorState::Invalid` is a permanent typed refusal, by design (record,
> 2026-08-23, Step 4.3; discharges the Step-4.2 review's [P3-7]).** Five survey
> arms return `Invalid`: more than two anchor-prefixed entries, an entry under
> the anchor prefix matching neither the identity-addressed name nor its
> round-trip alias, duplicate final or round-trip entries, and — added at Step
> 4.2 — a `.ca1-anchor-retired-` name whose ordinal does not parse; the
> fifth is the mutual-mismatch catch-all (`platform/anchor.rs:359`), the
> arm for an anchor-prefixed pair that agree with neither the addressed
> name nor each other. Every one
> refuses for ever, with no in-code exit.
>
> This is the standard [P2-2] was held to, and it is satisfied here for the
> reason [P2-2] was not. **The trigger of every `Invalid` arm is foreign
> contamination of the reserved grammar**: no window of this protocol can
> produce any of the five states. The protocol renames and never links, so it
> cannot make two names for one object; it writes anchor-prefixed names only
> through `anchor_name`, so it cannot make an unrecognised one; and it writes
> retired names only through `retired_name`, so it cannot make an unparseable
> ordinal. [P2-2]'s wedge was categorically different: the protocol wedged
> itself on a retirement **it had created**, at a state two ordinary crashes
> reach. Producing an `Invalid` requires an actor with write access inside
> `.gwz/checked-artifacts` planting a name inside a reserved prefix — squarely
> outside the accepted cooperating-same-user boundary, which is the same
> boundary E16's "refused permanently and by design, typed rather than silent"
> rests on.
>
> The refusal is typed and never silent (`"private durability anchor is missing
> or ambiguous while family state exists"`), it mutates nothing
> (`a_foreign_link_that_re_homes_the_anchor_is_refused_without_mutation` asserts
> the directory listing is unchanged across it), and the operator exit is the
> ordinary one for foreign contamination of a reserved namespace: remove the
> foreign name. **Every reachable post-crash state still has a typed exit** —
> that claim, made in the Step-4.2 block, was checked window by window and
> stands; `Invalid` is reachable only from outside the protocol.

---

## 7. Verification appendix — commands executed for this step

All from `gwz-core/`, `CARGO_TARGET_DIR=…/scratchpad/p41-target`, foreground.

```text
git rev-parse HEAD                                   → 51a9cba (clean at start)
cargo check --all-targets -p gwz-core                → Finished, zero diagnostics
cargo test --lib -p gwz-core checked_artifact::      → see the landing report
grep/count: ready_edge_prologue per execute_owner_*  → 6 edges × 1, create_and_retry × 0
dead-code inventory with all mod.rs allows removed   → 481 unique items / 50 files
CATALOG_PUBLICATION_CALL_COUNTS                      → 6 files, 13 calls (unchanged)
capability_permit.rs caller-inventory literal        → 13 (unchanged)
recover_or_create production callers                 → none (re-verified)
cargo clippy --all-targets --all-features -D warnings (from cargo clean -p)
                                                     → zero diagnostics
cargo fmt --all -- --check                           → clean
cargo test --lib (full)                              → 1567 passed / 0 failed / 1 ignored
L2-04 retained-reader harness (python3.13)           → 86 OK, tuple_count 24
boundary checker (python3.13)                        → 4 pin moves (mod.rs flat;
                                                       pre_catalog/catalog/platform
                                                       trees), recomputed at the
                                                       landing per the lane owner's
                                                       duty; green at the landed tree
global dead-code sweep (--force-warn dead_code)      → 1657 spans / 85 files
```
