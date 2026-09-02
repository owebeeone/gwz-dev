# GwzM5-8 DR-1 E8.3 Reconciliation Design — peer-blind review, STATE/GUARANTEE axis

Date: 2026-09-03. Reviewer: Fable (State axis; blind to the Code/mechanism axis).
Subject: `GwzM5-8DR1-Reconciliation-Design.md` (DRAFT 2026-09-03, base `ffd4f95`).
Tree read: gwz-core main `d6830cd` (the design's own base-moved note holds: no
mechanism file differs from `ffd4f95`). Time-box 30 min, met; items not reached
are marked UNVERIFIED. Method: reading only — no build, no suite, no edit.

Binding records read: DR1-Charter; FilesystemIdentity-Design (E8.1) §4.4/§4.7/§6/§7;
CapabilityFreeRuling; CapabilityFreeAmendment §3/§5/§6/§7-brackets; RecordRootAmendment
§1-§7; E4-Close §2-§6; E45B-Report; CurrentProgramCheckpoint position 2026-09-02 E4.5-B
(ruling (a) verbatim).

Code read at `d6830cd`: `checked_artifact/residue.rs:106-231`, `classification.rs:93-199`,
`authority.rs:101-145,:196-240`, `transition.rs:40-49,:118-126`, `entry.rs:36-41,:159-165`,
`cleanup.rs` (grep), `bootstrap/managed/provider.rs:51-62`; `merge/abort/evidence.rs:34-55,
:125-172,:269-400`; `merge/abort/preflight.rs:105-115`; `merge/root/abort.rs:110-150,:372-388`;
`merge/root/artifact_facts.rs:1-40`; `v1_lifecycle/service.rs:100-126`;
`v1_lifecycle/finalization/execute.rs:66-100`; `publication/live.rs:57-135`;
`git/gitbackend/recovery_support.rs:355-366`.

---

## Verdict (one line)

**GO-WITH-CONDITIONS — S1 may be chartered now (drive-and-record only; its RED-1
drive will expose [P1-1] itself); §2 is NOT adoptable as E8.3's complete answer until
[P1-1] is designed (a third component: who converges the leaf and retires the
counterpart residue), Q1 is re-phrased per [P1-2], and the marker-conversion
question of [P2-1] is put to the operator. One re-review round of §2.4/§2.6/§3.4.**

---

## Findings

### P1

**[P1-1] The cure classifies but nothing executes — the ledger's `Aborted` cells have no
execution path, and the reconciled residue is never retired.**
The design's reconciliation lives entirely on the observer side: I10 says the cure "only
ever converts `Ambiguous → Before/Recoverable`"; I2 says the survey "never writes, never
adopts, never repairs — every *action* stays … inside `replace_exact`/`remove_exact`";
§2.2 says `inspect_family` "cannot be widened in place … Widening it would weaken the
writer." Put together, on the driven row: after S3 the abort's pending-Marker arm returns
`Before` ⇒ `FS::Candidate` (design §2.4(A)); `observe_v1_evidence_rollback` therefore
reports `Before`, and `execute_v1_evidence_rollback` (`abort/evidence.rs:131-172`) calls
`artifact_facts::remove_exact(root, marker, marker_yaml)` (`:160-164`) →
`CheckedArtifact::remove_exact` → its OWN `classify_remove_exact` → the un-widened
`inspect_family(Bytes(marker_yaml), None)` → the forward residue is `foreign`
(`residue.rs:179-181,:205-206`) → `Ambiguous` (`classification.rs:141-143`) →
`Err("removal evidence is ambiguous")` (`transition.rs:121-126`). The lock/boundary arms
hit the identical wall through `write_checked` → `replace_exact` (`transition.rs:44-49`,
"replacement evidence is ambiguous"), and `root/abort.rs:380` / `preservation_root.rs:477-488`
likewise. **The user outcome moves from `Ok(RecoveryRequired)` to a typed `Err` — not to
`Aborted`.** Three consequences the design does not state:
1. *The reconciled classification is wrong on the facts for the marker.* The leaf is
   `Missing`, which is the reverse operation's POST state (removed), not its pre state;
   §2.4(A)'s "the leaf is still at the reverse operation's pre state" is false. The honest
   answer for a never-published goal is `After`/`Baseline` (skip the step) — which I10
   forbids ("never converts After into anything"; the cure only ever yields Before).
2. *For the detach-window leaves (L3/L4/L5/L6/L7) no component performs the physical
   convergence.* The leaf is ABSENT; the baseline bytes exist only as the detached
   `.source` in the private area. Restoring them (or rolling forward via the staged
   `.goal` and then reversing) is a rename under a lease — exactly what I2 says the
   survey never does and what the direction-bound writer refuses to do.
3. *Residue retirement is undesigned, and on path-stable leaves it is a regression.*
   Today's raw forward writer leaves no residue, so a later merge never sees one. After
   S4 converts the lock/boundary, a forward interruption + a "reconciled" abort leaves the
   forward family on disk; merge #2's `replace_exact(baseline₂ → candidate₂)` on the same
   `family_key` (same root, same path) enumerates it, marks it `foreign`, and refuses —
   a stranding delayed to the next merge. `cleanup.rs` retires only the writer's OWN
   family (`:70`, `:117`, `:202`); no foreign-family retirement exists.
**Cure:** design the third component explicitly — a *reconciling convergence* on the
already-acquired `CheckedArtifact` under the observer's lease (roll-forward via `.goal`
or roll-back via `.source`, then retire the family with the existing `cleanup.rs`
primitives), with I2 re-scoped ("the survey never repairs; the *reconciler* repairs
under the writer's lease and only on an exact counterpart") and I10 corrected (the
never-published case classifies `After`). Re-size S3 accordingly. State per row which
component (observer / reconciler / writer) produces the user-visible outcome.

**[P1-2] Q1's "charter S5 and convert `rewrite.rs::commit`" is barred as offered.**
RR §1b: `commit` is shared by the activated forward arms and the plain-lease reverse arms
(`service.rs:119-124`, read: `ResumeStart | Continue ⇒ acquire_activated`, everything
else ⇒ `acquire`), so converting it puts every standard-path v1 abort on the identity
probe. RR §4's Plain-Lease Probe Clause leaves two live options (split by lease; prove
the reverse reach absent) — the third ("amend the list") is CLOSED by ruling (A). The
design's §3.3 treats S5 as a *discovery* risk only ("discovery must take no probe") and
never mentions §1b; §5.3's S5 row amends RR §2/§6, O13 row and P-2 but not the lease
split. **Cure:** re-phrase Q1 as "re-affirm, or charter S5 = discovery reconciliation
(no probe) + a lease-split of `commit` — noting that the reverse arm's rewrite stays raw
in every outcome, so the RR §2 exception is re-affirmed *for the reverse arm* regardless."
S5 sizing (~200 LOC) is UNVERIFIED by the design's own note and cannot hold once the
split is included.

### P2

**[P2-1] Converting the marker in S4 vs ruling (a)'s "Do not convert it" — needs the
operator's word again, as a one-line question the design does not ask.**
The ruling (checkpoint position 2026-09-02 E4.5-B, verbatim): "The marker write at
execute.rs:45 joins lock and boundary in the dated residual, on the directional-residue
ground … Do not convert it. Do not lift 'no (C) inside E4' for an observer cure … DR-1's
agenda gains: directional-residue class, classifier widening, preservation-bundle audit."
Reading: the ruling was made INSIDE E4, chose packet option (a) whose text routes "the
directional-residue class *with its observer cure*" to DR-1, and dated the residual on a
ground the cure removes — so a post-cure conversion is within the ruling's *spirit*. But
its *letter* is unqualified ("Do not convert it"), and no adopted record names `:45`'s
post-cure conversion: the charter's forward-arm line (`GwzM5-8DR1-Charter.md:76-77`)
says "converts only with the reader-side reconciliation above" for **`:48/:51` only**,
and CapFree §7's ruled bracket (`CapabilityFreeAmendment.md:565-568`) says "NONE of the
three … arms converts; all three stay raw." Verdict: **the design may propose it, but
S4's marker conversion needs the operator's word — add it to §6** (it is absent from the
eleven). Cheap; not a blocker for S1-S3.

**[P2-2] I1's "no silent adoption" is asserted where the token's own boundary says it
must be re-litigated; the exclusion proof is at the classifier, and its ground moves
under (c) P2.**
`bootstrap/managed/provider.rs:51-62` (read): "A later step that uses a resident record
to decide adoption of state this action did not create makes determinism load-bearing
for exclusion, and must re-litigate the token then." The survey is exactly such a step
(the abort adopts a leaf state from a residue it did not mint). I1 says the boundary is
"untouched"; it should instead STATE its exclusion argument: (a) enumeration bound =
`family_prefix(family_key)` = SHA-256(root durable identity + canonical path)
(`authority.rs:196-202`, `residue.rs:116,:127`) — enforced at the survey; (b) exact
counterpart = byte-swapped `(expected, goal)` + C2/C3 identities (I3) — enforced at the
classifier, not the survey; (c) the same-user boundary is out of scope per E16. Two
facts I1 omits: **L3 and L6 are one leaf** (`gwz.conf/gwz.lock.yml`, `artifact::LOCK_PATH`),
so one family is written by two different observer/writer pairs (merge publication/abort
and selected-root rollback) and (b) is the only thing separating them; and **(c) P2's
naming rebase replaces (a)'s ground** with a nonce-derived prefix (E8.1 §4.2/§4.4) — a
copied workspace image carrying the same instance id becomes enumerable. K1 must carry
the *invariant* forward, not just the survey code.

**[P2-3] The abort sentence: §3.4 is written against CapFree §6's text, which E8.1 §4.7
retires; the two compose only as tiered sentence + scoped addendum.**
E8.1 §4.7 (adopted) replaces filesystem-naming with claims and tiers and makes the
re-verifying abort "recover exactly at tier 3, and at tier 2 within the session …
Outside that, they refuse with `IdentityUnprovable`." The design's sentence (§3.4)
keeps CapFree §6's filesystem list, the legacy-probe parenthetical and a drifted cite
(`linux.rs:136-151` → `:142`/`:132` at `d6830cd`, per the design's own note), and
claims `--abort` "clears a merge interrupted at any point of its publication" — false at
T1/T2-after-reboot (E8.1 refuses before reconciliation) and false today per [P1-1].
Composed form: E8.1's tiered sentence + "within the tier at which it re-verifies, such an
abort reconciles an interrupted publication from the merge's own private residue …".
Also: E8.1 §4.4's corrected rule ("degrade the classification, never foreign, never
invisible") vs I4 (`actions.len() > 1 ⇒ unreadable ⇒ Ambiguous`) — the survey must
distinguish tier-degraded from foreign or (c) P3 re-litigates S2. K2 acknowledges the
ten homes but names the wrong base text.

**[P2-4] The transitions table is silent on four cells; the design says S3 covers them.**
(i) Abort, `pending == None` arms (`abort/evidence.rs:284,:299,:313`): `evidence_shape_is_exact`
runs `file_states(root, record, None)` FIRST (`:390-393`); a Missing lock/boundary →
`classify_file(missing_is_baseline=false)` → `Other` → `preflight_v1_evidence` `Err`
(`:34-55`) before any pending arm is reached. S3 says the None arms "consult the survey",
but `artifact_facts::observe` returns `RegularFileFact` (Missing/Bytes/Invalid,
`artifact_facts.rs:8-13`) with no residue channel; I9 forbids a new door. The cell is
UNSPECIFIED. (ii) `abort/preflight.rs:111` (`@root` observe: Missing manifest/lock →
`MergeRecoveryRequired`) — listed among the 13 moving call sites (§2.3) but absent from
§2.4. (iii) L11 (`regular_file_equals` NotFound → false) — "folded into S3", no row.
(iv) The user-visible surface per row is mixed (`Err` vs `Ok(RecoveryRequired)`): the
E45B transcript shows the FIRST abort returns `Ok(RecoveryRequired)` and the SECOND
`Err(RecoveryEvidenceMismatch)`; the ledger should say which the user sees at each row.

**[P2-5] Tier-2 "keep deferred" re-defers what an adopted record assigned to this round.**
CapFree §5: "DR-1 mints the sub-surface BY AMENDMENT WITH ITS OWN DUAL and names the
execution carrier"; charter §4 deliverable 3 says the same. §4.2's disposition needs a
dated bracket at CapFree §5 and at the charter's deliverable 3 on the operator's word
(Q9 asks it — fine as a question; not yet a grounded disposition). Ruling point 4
permits deferral, so the operator can say yes in one line.

### P3

- **[P3-1]** I6 "on every reachable v1 abort" → "on every *post-publication* v1 abort":
  `preflight_v1_evidence` returns `Ok(())` when `record.publication` is `None`
  (`abort/evidence.rs:39-41`); pre-publication aborts acquire nothing before or after.
- **[P3-2]** §4.4 "a read dependency of the abort path, which it was not before" is
  overstated: the post-publication abort already reads `.gwz/checked-artifacts` through
  `inspect_family` inside every `classify_*`/`observe` door (`classification.rs:140`);
  the survey widens WHAT is read. The binding order itself is sound; RelocationPlan §2
  + CapFree §4 are the right homes.
- **[P3-3]** §4.5 record homes are under-named: add CapFree §3's `:275` inventory row
  (`archive.rs` `remove_file: 1`), CapFree §5's O8 paragraph ("conditional on (C)"),
  RR §6's inverted bracket (archive files "permanent-documented in all three rows"),
  plan `:53`/`:512-513`. The (C)-cannot-revive argument is a record argument (design
  admits, UNVERIFIED 4) and is sound on ruling (A)'s closure.
- **[P3-4]** §4.6: the FROZEN cell is `GwzM5-8R4bR2ConsumerCheckpoint.md` §10 `:279`;
  CapFree §3's `:279` dispositions row ("E4.6-B converts … through a new git-directory
  replace door") also needs a bracket. Both are frozen-tier; one dual can carry both.
- **[P3-5]** §4.8 `load_archived` is a `pub` `MergeStore` trait method (design: trait
  default `store/mod.rs:56-58`). No hit in `GwzM5-8R2DInterfaceFreeze.md` (grep); whether
  the trait surface is frozen elsewhere is UNVERIFIED — check before deleting.
- **[P3-6]** I4 (bundle residue-orphaning sweep): the design does not say WHERE the sweep
  runs. If inside the reverse arm's existing door (`v1_write_bundle_checked` → `finish()`),
  it is on the plain lease but on an already-probing path (CapFree §6 path (ii)) — fine;
  it must never be triggered from `gwz stash` or the v0 preserve arm (carved). Say so.
- **[P3-7]** L7's new residual line has no adopted home: E4-Close §2's register lists
  raw arms only. Propose a dated bracket at E4-Close §2 (or CapFree §6) — answer Q7 "now".
- **[P3-8]** §4.4's binding order and I5 are consistent with RR §2; S4c's bracket is the
  right vehicle for re-affirmation. If S5 is taken, RR §2 (the exception clause), §3 P-1
  (shrinkage arm), §3 P-2 (tripwire asserts the door absent), §6 all move — design §5.3
  names them; add §1b's lease split ([P1-2]).

---

## The guarantee ledger under the cure (as this axis reads the mechanism)

Legend: RR = `RecoveryRequired`; Err(x) = typed refusal; "design" = §2.4's cell; "review"
= this axis's reading at `d6830cd` with §2.2/I2/I10 held as written.

| Leaf | Window | Observer / arm | BEFORE (today) → user | AFTER per design → user | AFTER per review → user | Design table |
| --- | --- | --- | --- | --- | --- | --- |
| L2 marker | (iii) | abort, pending=Marker (`evidence.rs:307-311`) | foreign→Ambiguous→Other → `Ok(RR)`, then `Err(RecoveryEvidenceMismatch)` | Before→Candidate → `Ok(Aborted)` | Before→Candidate→`remove_exact`→writer foreign → **`Err("removal evidence is ambiguous")`**; residue persists | DISAGREES ([P1-1]) |
| L2 marker | (iii) | abort, pending=None (`:313-318`) | Missing, `missing_is_baseline` → Baseline | unchanged | unchanged | agrees |
| L2 marker | (i) | — | none (expected `Missing`, no detach) | none | none | agrees |
| L2 marker | (iii) | forward `snapshot` on resume | Missing→Baseline prefix→WriteMarker→same-direction `replace_exact`→Recoverable→completes | "n/a, unchanged" | already converges (L8 precedent) | silent that it converges |
| L3 lock (after S4) | (i) | forward `snapshot` (`live.rs:110-112`) | Missing→`None`→Ambiguous → RR origin | normalise to expected → WriteLock → same-direction Recoverable → completes | agrees; new acquisition under `acquire_activated` only (I7) | agrees |
| L3 lock (after S4) | (i) | abort, pending=None, `evidence_shape_is_exact` first (`:390`) | Missing→`classify_file(false)`→Other → preflight `Err` → RR | "None arms consult the survey" | `observe` has no residue channel; **UNSPECIFIED** | silent ([P2-4]) |
| L3 lock (after S4) | (i)+(iii) | abort, pending=Lock (`:277-282`) | foreign→Ambiguous→Other → Err | Before→Candidate → exact | then `write_checked(cand→base)` on absent leaf + foreign residue → **`Err("replacement evidence is ambiguous")`**; baseline bytes sit in `.source`, nobody restores them | DISAGREES ([P1-1]) |
| L3 lock | (iii) | next merge's forward `replace_exact` after a "reconciled" abort | n/a today (raw forward leaves no residue) | not stated | stale forward family → foreign → **refusal on merge #2** — regression | silent ([P1-1] c) |
| L4 boundary (after S4) | (i) | forward `snapshot` + empty-baseline case | Missing→None unless baseline empty → RR | normalised in `snapshot` before `:123`; `acceptance/publication.rs` untouched (I8) | agrees | agrees |
| L4 boundary (after S4) | (i)+(iii) | abort, pending=Boundary / None | as L3 | as L3 | as L3 | as L3 |
| L5/L6 root manifest/lock | (iii) shipped | `root/abort.rs:123-145` `transition_state` | Ambiguous → refusal | Before → "existing Before handling" | then `:380` `write_checked` → writer foreign → Err | DISAGREES ([P1-1]) |
| L5/L6 | (i) same direction | same | Recoverable | unchanged | unchanged | agrees |
| L6 = L3 path | ownership | one family, two writer pairs | — | not stated | separated only by I3's byte pairing | silent ([P2-2]) |
| @root preflight | (i)/(ii) | `abort/preflight.rs:111` observe | Missing → RR | not stated | unchanged unless routed | silent ([P2-4]) |
| L7 preservation-root leaves | (iii) shipped | `preservation_root.rs:207` `observe_transition` | Ambiguous → refusal `:209-210` | Before → existing logic | then `replace_relative` → writer foreign → Err; reachability UNVERIFIED (design UNVERIFIED 5) | DISAGREES ([P1-1]) |
| L8 bundle | (i) same direction, members ≥ 2 | `checked_bundle.rs:85` | Recoverable → converges | unchanged | agrees — the proof case | agrees |
| L8 bundle | raw-sibling orphaning | `stash::write_bundle`, 6 `gwz stash` sites | orphaned residue, no observer | unchanged; I4 sweep (~120 LOC) | agrees; trigger site unstated ([P3-6]) | agrees |
| L1 record root | (ii) discovery-dead | `classify_open_record` `.gwz/merge` only | no open merge; `--abort` cannot find id | unchanged by S1-S4; S5 conditional | agrees; S5 as offered barred by RR §1b ([P1-2]) | agrees (explicit) |
| L1 record root | (i) rewrite | carved (RR §2) | atomic `rename_durable` — no window | unchanged | agrees | agrees |
| L9 catalog records | (i)/(ii) | `matches_attempt` reconciler | closed | unchanged | agrees | agrees |
| L11 exact-file set | (iii) inherited | `regular_file_equals` NotFound→false | root drift | "folded into S3" | no row; UNSPECIFIED | silent ([P2-4]) |

**Net:** every cell the design turns green on the abort side and on the shipped L5/L6/L7
rows depends on a component the design does not contain. The forward-side cells (L3/L4
window (i)) are sound because the retry is same-direction and the boundary's own
`Recoverable` convergence exists. The record root and the carved rows are honestly stated.

---

## Answers to the seven questions put to this axis

1. **Ledger:** above. The design's table agrees on the forward side, the carved rows, L8,
   L9 and the record root; it disagrees on every abort-side "Aborted" cell ([P1-1]) and is
   silent on the None arms, `@root` preflight, L11 and the next-merge regression.
2. **No silent adoption:** the invariant is I1 + I3; enumeration is enforced at the survey
   (family prefix), the counterpart proof at the classifier. The design says the token's
   boundary is "untouched" where `provider.rs:51-62` says such a step must re-litigate it
   ([P2-2]); the argument is defensible once stated, and must be carried through (c) P2.
3. **Record root:** a direction-free family survey gives the record root NO discoverer —
   the survey is a method on an acquired `CheckedArtifact` for a KNOWN path; discovery is
   the problem of not knowing the path. "Stays carved unless S5" is honest about RR §2
   clause (b) becoming contingent (RR §2 anticipates it in terms). S5 needs an RR amendment:
   yes — §2 (the exception clause), §3 P-1 and P-2, §6 — AND a `commit` lease split under
   §1b/§4 ([P1-2]).
4. **Capability-free list:** S1-S4 put no probe on merge/commit/create/abort/GC — abort-side
   work rides acquisitions the post-publication abort already makes ([P3-1] wording);
   the forward `snapshot` acquisition is `acquire_activated`-only (`service.rs:119-124`,
   read). The twenty carved writers are untouched. S5's discovery survey is the one
   red-line risk and the design names it. I4's trigger site must be stated ([P3-6]).
5. **Abort sentence:** the design's sentence and E8.1 §4.7 do NOT compose as written
   ([P2-3]); they compose as tiered sentence + scoped reconciliation addendum, and only
   after [P1-1] makes the addendum true.
6. **§4 dispositions:** O14 — grounded (charter §2, CapFree §5, RR §5); amends Freeze §8/§9,
   own dual. Tier-2 — re-defers CapFree §5's assignment; bracket + operator ([P2-5]).
   Legacy retirement — grounded (CapFree §4); amends RelocationPlan §2 + CapFree §4.
   `gc_archived` — grounded; amends O13 (plan §1.1) + CapFree §3 `:275` row + CapFree §5 O8
   + RR §6 bracket ([P3-3]); operator's (Q2). Row `:279` — grounded (charter §2, prep §3);
   amends ConsumerCheckpoint §10 `:279` + CapFree §3 row ([P3-4]). Bundle audit — corrects
   the ruling-quoted agenda wording by bracket at E45B flag 9 / charter `:119`; L7 needs a
   new residual home ([P3-7]). A1-era allowances — no record owns them; no amendment;
   check the trait surface ([P3-5]). `CatalogOwnerV1` — grounded (E4-Close §5 C7(ii)),
   digest re-pin only. Inventory row — grounded (E4-Close §5 C7(iv)), no pin.
7. **Sequencing:** running S1-S4 parallel with (b) satisfies E8.1 §7 item 4 ("before or
   with (c) P3") under all three options and breaches no operator constraint ("do not
   start (C) inside E4" — E4 is closed). The (iii) judgement is a recommendation, fine.
   §6's eleven questions are one-line-answerable; two are missing (the marker's
   conversion, [P2-1]) or mis-phrased (Q1, [P1-2]).

---

## Conditions for GO

1. [P1-1] — add the reconciler component to §2 (who converges, who retires, under which
   lease), correct I2/I10, re-size S3, re-issue §2.4's abort-side rows and §3.4's sentence.
   Re-review one round.
2. [P1-2] — re-phrase Q1 with §1b's lease split and "reverse arm stays raw regardless".
3. [P2-1] — add Q12: "may `execute.rs:79` convert in S4 after S3 lands, given ruling (a)?"
4. [P2-2], [P2-3], [P2-4], [P2-5] — text folds by the lane owner; no re-review needed
   beyond condition 1's round.
5. P3 items — lane-owner folds.

S1 (drive RED-1, pin NEW-1, correct the four record homes + one in-tree comment) is
GO now under the design's own drive-first rule (Q10) — nothing in S1 depends on [P1-1].

---

## Round 2 — 2026-09-03 — confirmation of REVISION 2's folds (text diff only; 15-min box)

Method: REVISION 2 read against round 1's findings. No new tree reads; every code
cite below is one round 1 verified at `d6830cd`.

| Finding | Status | Where / how |
| --- | --- | --- |
| **[P1-1]** third component | **FOLDED DIFFERENTLY — acceptable with ONE text fold** | §0 headline; §2.5 `reconcile_counterpart` pre-pass: survey → converge (restore the detached `.source`, `transition.rs:290-301` inverted) → retire the counterpart family via `cleanup.rs` `:87/:89`, `:132/:134`, `:222/:224`, all-or-nothing, on the caller's already-held lease; invoked from `file_states` (once, up front), `abort/preflight.rs:110-118`, `root/abort.rs:123-145`, `preservation_root.rs:207`; restorer named per leaf; retirement timing "before it returns" (the next-merge regression row added at §2.4(A)); I2 and I10 corrected (the `Before`-only claim withdrawn; `After` for a never-published goal); I4 explicitly does not subsume the retirement. Abort-side cells filled with the user-visible outcome and the producing component. **The one untruthful cell — the driven row.** §2.4(A) L2 pending==Marker says `After` "with the `foreign` gate bypassed" and "survey + observer. No reconciler needed — no detach happened"; §2.5 fact 1 repeats "No reconciler, no writer arm". But nothing in the design bypasses `classification.rs:141-143` for the marker: the forward family (authority + staged `.goal`) is still on disk, `classify_remove` → `inspect_family` → `foreign` → `Ambiguous` → `Other` → `RecoveryRequired`, exactly today's RED-1 — and E4.5-B's ablation is the proof that the residue must GO. The mechanism that clears the gate exists in the design — the reconciler's **retire half** — and the reconciler's own contract ("converge to an endpoint if … exact counterpart; converge AND retire, or neither") already covers it with converge = no-op. **Fold owed (lane owner, before S3 charters):** the reconciler runs for `MissingReplace` counterparts in *retire-only* mode; the L2 row and §2.5 fact 1 read "reconciler (retire-only) + observer", and the per-leaf restorer line keeps "nobody restores" for L2/L1-create while saying who *retires*. Nit: the same cell says `execute_v1_evidence_rollback` "then calls `remove_exact`, which classifies `After`" — `execute_v1_evidence_rollback` refuses unless the observation is `Before` (`abort/evidence.rs:131-136`); on `After` the lifecycle advances without calling it. The user outcome (`Ok(Aborted)`) holds either way. |
| **[P1-2]** Q1 / S5 | **FOLDED AS STATED** | §3.3 gains the lease ground (RR §1b; `service.rs:119-124`; RR §4's two live routes, the third closed by (A)); "the reverse arm's rewrite stays RAW in every outcome"; S5 = discovery reconciliation + `commit` lease split, two stop-triggers; UNVERIFIED 10 records the sizing. Q1 re-phrased verbatim to that. |
| **[P2-1]** marker conversion vs ruling (a) | **FOLDED AS STATED (and extended)** | §6 Q12, quoting the letter, CapFree §7 `:565-568`, charter `:76-77`; extended to `:88`/`:98`. Correct extension: the same "none of the three arms converts" bracket covers them. |
| **[P2-2]** I1 exclusion proof | **FOLDED AS STATED** | I1 rewritten as the three-part proof (survey-enforced bound; classifier-enforced counterpart; E16 same-user scope), quoting `provider.rs:58-61`'s re-litigation sentence; L3/L6 one family in I1 and in the matrix L6 row; §5.1 "K1 carries an INVARIANT" — (c) P2's charter must re-state and re-prove I1. |
| **[P2-3]** abort sentence | **FOLDED AS STATED** | §3.4 rewritten as E8.1 §4.7's tiered sentence + an addendum scoped "within the tier at which it re-verifies"; the filesystem/legacy-probe parenthetical and the drifted `linux.rs` cite dropped; "at any point" qualified; tier-degraded vs foreign folded into S2's charter and K1; K2 re-based on E8.1 as owner of the base text. Nits, no re-review: §3.4 labels E8.1's sentence "adopted, unchanged" but paraphrases it ("needs no such filesystem" for "needs no filesystem capability at all"; "within the session" for "within the session that minted the evidence") — the ten-homes edit must quote E8.1 §4.7 verbatim; a duplicated heading fragment ("Three further consequences … :Three further consequences …"); Q5 still reads as if §3.4 lands first and E8.1 folds it, whereas K2 now says E8.1 lands the sentence and the addendum lands with S4 — align Q5 to K2. |
| **[P2-4]** four silences | **FOLDED AS STATED** | (i) None arms: the pre-pass, `observe` untouched (§2.3 last paragraph, §2.4(A) row, S3); (ii) `abort/preflight.rs:110-118` row (and matrix L5/L6); (iii) L11 row — read-only, downstream of the pre-pass, S3 guarantees ordering; (iv) every (A) row carries "→ user" and "by which component". |
| **[P2-5]** tier-2 | **FOLDED AS STATED** | §4.2 re-grounded: a re-deferral of CapFree §5's / charter §4 item 3's assignment, conditional on Q9, dated brackets at both homes. Q9 re-phrased. |
| **[P3-1]** I6 wording | FOLDED AS STATED | "every **post-publication** v1 abort", `abort/evidence.rs:39-41` cited. |
| **[P3-2]** §4.4 read dependency | FOLDED AS STATED (sharpened) | Reads it already; the new dependency is that the abort becomes a *writer* of the area (restore-rename + foreign-family retirement) — correct, and the better ground for the binding order. Q8 updated. |
| **[P3-3]** §4.5 homes | FOLDED AS STATED | Plan §1.1 O13; CapFree §3 `:275` row; CapFree §5 O8; RR §6 bracket; plan `:53`/`:512-513`. (§5.3's §4.5 row still names only "O13 + the two allowances" — harmless; §4.5 governs.) |
| **[P3-4]** §4.6 homes | FOLDED AS STATED | ConsumerCheckpoint §10 `:279` (frozen) + CapFree §3's `:279` dispositions row; one dual. (§5.3's §4.6 row not updated — harmless.) |
| **[P3-5]** `load_archived` | FOLDED AS STATED | UNVERIFIED 9; "settle it before deleting". |
| **[P3-6]** I4 trigger site | FOLDED AS STATED | Inside `v1_write_bundle_checked`'s `finish()` path (already-probing plain-lease path, CapFree §6 (ii)); never from `gwz stash` / v0 preserve; plus the correct note that I4's predicate does not fire on a counterpart residue. |
| **[P3-7]** L7 home | FOLDED AS STATED | §4.7: no adopted home (register lists raw arms only); dated bracket at E4-Close §2 (or CapFree §6); Q7 answered "now". |
| **[P3-8]** S5 amendment set | FOLDED DIFFERENTLY — acceptable | The `commit` lease split is named in §3.3, Q1 and UNVERIFIED 10; §5.3's S5 row is unchanged (RR §2, §6, O13 row, P-2). Add RR §1b/§4 to that row when S5 is chartered, if ever. |

One further nit for S2b's charter, not a condition: I2's "all-or-nothing" needs a
crash-consistency table — a crash between the restore-rename and the retirement
leaves the leaf at the forward action's `expected` with the family's `.source`
gone and its authority + `.goal` present; the reconciler's re-entry must classify
that as "converge = no-op, retire" (NEW-2b should pin re-entrancy).

**Round 2 verdict: CONFIRMED — adopt REVISION 2 as E8.3's answer; S1 is
charterable now.** One lane-owner text fold is owed before S3's charter (the
[P1-1] marker cell: retire-only reconciler for `MissingReplace` counterparts);
it changes no component, no size and no invariant — the reconciler's stated
contract already implies it — so it is not a re-review.
