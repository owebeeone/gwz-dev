# Current program checkpoint

Date: 2026-08-22 (resumed)
Status: **the single current-state authority for the GWZ merge program.
Update at every checkpoint boundary; keep concise; history belongs in git,
not in this file. Live status paragraphs in other documents are superseded
by this file (rulebook §7.3).**

## RESUMED 2026-08-22 — parallel execution, refined tier economy

Operator resumed 2026-08-22: "proceed now with parallelization the
focus (reduce calendar cost) but also optimize model use so we use
fable token only where it makes a difference." Recorded consequences:

- **Refined tier policy (supersedes the 2026-08-16 economy policy's
  reviewer line):** Fable ONLY at the program-level dual gates —
  R2-D's three duals, the M5b dual, R4b-G, the A1 activation review,
  this amendment-tier work — plus escalations and deep diagnosis.
  Interior single-axis reviews (R2-D Phases 2-4 steps) run on the
  Opus tier with automatic escalation to a Fable pass on any P0/P1/P2
  finding. Drafting/implementation stays Opus; mechanical stays
  Sonnet.
- **Parallel structure:** 2-3 lanes + reviews shadowed by the next
  package (the §4.4 pipeline rule). Wave 1 launched 2026-08-22:
  (i) Phase 0 dual review (Fable ×2, Code+State, peer-blind) on the
  parked `d32b2c9`; (ii) Phase 1 Step 1.1 failing tests (Opus)
  shadowing it; (iii) M5b implementation (Opus) on its own subtree;
  (iv) thin-A1 round-2 remediation applied inline — **and ACCEPTED
  2026-08-22 at GO/GO** (Consistency GO / Safety GO focused
  re-verdicts, both appended to the report files; every round-1
  P0-P2 resolved in one merged round; the banner pass — RemPlan-4
  supersession banner, M5b dated annotation, EvidenceInventory
  re-frame, and the Refactor/R1 naming sentence — landed with
  acceptance; `GwzM5-8ThinA1Amendment.md` is now the controlling
  gate-chain text).
  **Phase 0 freeze ACCEPTED 2026-08-22 at GO/GO** — dual #1 of 3
  consumed (Code GO after the round-2 [P1-3] pin catch at `c40e712`;
  State GO with C-3 ruled real/Phase-1-owned/not-Track-W and the
  §3.1 pin verified). `GwzM5-8R2DInterfaceFreeze.md` is FROZEN: five
  seams, two extension classes (C-2 recheck arms, C-3 observer
  grammar) with per-phase assignments, §3.1 persisted-home pin — all
  binding on Phases 1-5. Tier recording per plan §7.6: done (this
  section + the thin-A1 section). Freeze train PUSHED to origin
  through `c40e712`; the M5b park (`3e60529`) stays local pending
  its own dual. Tracked: both Track-P spike cases green on the next
  full Windows matrix run (dispatched at push). Wave 2 LAUNCHED:
  Phase 1 Step 1.2 driver (Opus; makes the six red admission tests
  green, implements C-2 arms + C-3 observer grammar under the frozen
  rules) and the D3 durable-cursor implementation lane (Opus).

  **M5b package DELIVERED and PARKED pre-review** (2026-08-22): the
  no-ff semantics installation per the frozen design — 21 new green
  suites across 6 `cfg(test)` files, 1,424 test lines, **0 production
  lines** (§8.3 hard ceiling holds), fmt/clippy green, both T-6
  suites green and their file byte-identical to HEAD, full lib suite
  green in partitions (1,399 tests; the only failures are the Phase-1
  lane's 6 red-by-design admission tests). **Freeze-ledger decision
  (lane owner, per §8.3's return-to-the-freeze rule, subject to the
  M5b dual review):** the package's test-line ceiling is raised
  1,200 → 1,450 for this package only — every line maps to a
  design-named §6/§7 obligation and ~140 lines are shared harness;
  the 0-production-line and ≤10-file ceilings are unchanged.
  **Stopped and tracked (not M5b's to fix):** service-level
  abandonment of a NotStarted frozen action is unreachable
  MODE-BLIND (pre-existing reducer guard at
  `transition/reduce/participant.rs:105-119`; Normal-mode control
  probe fails identically) — routed to the R4b acceptance-debt
  surface at the A1 activation review (thin-A1 amendment §1);
  the abandonment suites are written at the authority/reducer level
  per tree precedent. T-5 (retained-reader manifest), the §5.4/Q7
  ledger sentence, the M5b-W1 record-contract item, the ChangeBudget
  row, and the clean-tree re-cut are acceptance-ritual steps, on the
  lane owner at the M5b settle.
- The escape amendment stays second-lane/blocked on operator handoff
  (unchanged). The paused-state facts below remain the baseline the
  waves execute against.

## Pause record 2026-08-16 (baseline for the resume)

Operator paused the program ("pause after all the current tasks are
finished and create a checkpoint") after the thin-A1 instruction was
executed. All in-flight agents completed before the pause; the tree is
clean; nothing is mid-edit. **gwz-core local HEAD `d32b2c9` is one
commit ahead of `origin/main` (`90d3f8a`) and is deliberately
UNPUSHED**: it parks the R2-D Phase 0 freeze package, whose acceptance
gate (dual review) has not run — push only with a review GO/GO train
or by operator instruction. Everything else is committed in the root
repo only (docs), which is never pushed by the implementor.

State at pause, by lane:

1. **R2-D (my lane, the A1 path).** Plan ADOPTED (§9 record). Phase 0
   package DRAFTED and PARKED at `d32b2c9`: freeze memo
   `GwzM5-8R2DInterfaceFreeze.md` (DRAFT, 692 lines — five frozen
   seams, per-platform Track-P table for all 22 edges, six adopted
   owner decisions, three-dual tier statement), backend trait delta
   (four `managed_operation_unavailable` defaults → required),
   admission owner skeleton, 11 green scaffolding tests incl. the
   macOS-green admission publish/retire spike
   (`tests_admission_spike.rs`, 2 tests) against the sealed
   `publish_verified_no_replace` family, RemPlan §10 append-only
   activation-map annotation, digest pins refreshed (checker green).
   Gates: fmt/clippy/checker green; full suite partitioned
   1370/1371 pass, 0 fail — the one unexecuted test
   (`root_fault_matrix::every_root_physical_and_successor_boundary_
   recovers_without_repeating_mutation`, workspace_ops, >585s alone)
   is pre-existing, outside this package's file set, and was green in
   CI at `90d3f8a` (run 13); confirm on the next matrix run.
   Track-P verdict: NO new platform primitive; ONE in-seam Phase-1
   obligation recorded as memo contingency C-2 (admission arms for
   `DirectoryInteriorRecheckV1.expected` / `DestinationRecheckV1`
   inside the sealed primitive — protocol-shaped extension, not a
   bypass). Memo §7 corrects plan line-drift, including one material
   correction: `LeafObserver` has ZERO impls on this tree (the plan's
   `contracts.rs:183/:236` citation was wrong); Phase 2.1 writes the
   first.
2. **Thin-A1 gate-chain amendment (my lane).** DRAFT committed;
   round-1 dual FILED: Consistency NO-GO (2 P1 — missed supersession
   targets `GwzM5-8R4bR2ConsumerCheckpoint.md:21-23`/`:404`,
   `…-RemPlan.md:605-607`, and the frozen M5b `:987-989` dependency
   clause vs §5's "Untouched" row) and Safety NO-GO (3 P1 — the ~3.5k
   LOC of R4b P2/P3/P4 lifecycle lanes whose only scheduled
   independent acceptance was the deferred R6 must be NAMED as an
   A1-activation-review exception; more missed live sentences + the
   ConsumerCheckpoint §14 ninth stop clause needs an explicit
   post-A1 re-scope; the three-dual cap must name the operator
   override of §4.2 and stop citing the empty metrics table). All
   five P1s are text/disclosure fixes that keep the descope intact.
3. **Operator-escape amendment (second lane, parked).** DRAFT
   committed; round-1 dual FILED: Code NO-GO (3 P1) / State NO-GO
   (6 P1), 0 P0 anywhere; blind convergence on the unamended
   cursor-derivation contract. Non-gating for A1. Remediation is
   round 2 of 2 and is BLOCKED ON OPERATOR HANDOFF (thin A1 lane
   split) — this implementor does not pick it up.
4. **Tracked, untouched:** D3 cursor implementation (A1 window);
   ARM64 EBADF substrate package (lead linux.rs:173-181); MAX_PATH
   relocation (4.3 decides); v0 wedge runbook reproductions (Q9,
   second lane); panic class-B conversions (second lane); M5b N-4
   wording; probe-branch cleanup (operator's call); R2-D plan §6
   step-count nit.

**Resume order (estimated first hour):**
(a) verify tuple: root HEAD = the pause-checkpoint commit, gwz-core
local `d32b2c9` unpushed over `90d3f8a`, trees clean;
(b) thin-A1 remediation — one merged patch for the 5 P1s + focused
re-verdicts from the same two reviewers (contexts intact), then the
RemPlan-4 supersession banner on GO/GO;
(c) launch the R2-D Phase 0 dual review (dual #1 of 3, peer-blind,
Code+State axes, cross-model where available) on the parked package;
(d) start Phase 1 (R2-C3) failing tests the moment (c) is in flight
(pipeline rule, `GwzFasterProposal.md` §2); push the Phase 0 train on
its GO/GO. (b) and (c) may run in parallel.

## Exact tuple

| Repository | Commit | Note |
| --- | --- | --- |
| gwz-dev (root) | this docs commit (the pause checkpoint) | literal restatement per ReviewCode-3 P3-5 |
| gwz-core | `d32b2c9` local, **unpushed** (origin/main `90d3f8a`, Windows GREEN run 13, macOS GREEN) | parked R2-D Phase 0 package, pre-review |
| gwz-cli | `3cca145` | Close R4b P1/P2 remediation gate |
| gwz-py | `929efb0` | Implement R4b reverse merge lifecycle |
| taut | `f008419` | 0.8.x fallible from_cbor line |

Exact hashes for "this coordinated commit" rows are pinned by
`gwz.conf/gwz.lock.yml` in the same commit; the next docs-only commit
restates them literally (per ReviewCode-3 P3-5).

Workspace lock pins verified equal to member HEADs at writing. Released
line: v0.10.5 (see `GwzMergeCheckpoint-v0.10.5.md`; v0.10.4 superseded).

## Ownership

Implementation lane: **Claude (F5)**, by operator decision 2026-08-15,
effective from the tuple above. The previous implementer's lane is closed at
`da58135`; per L1-06 no other agent writes to the merge/catalog lane without
an operator-recorded handoff. Reviews run cross-model where possible
(`GwzProcessOptimization.md` §4.3).

## Process authority

`AgentProcessRules.md` (as amended 2026-08-15) plus the adopted
`GwzProcessOptimization.md`, which controls where they differ. Tiered review
depth, two-round remediation cap, two-track freeze, and history archiving
(L1-33) are in force from this checkpoint. (Two-track freeze: the policy is
in force now; its checklist and CI wiring land in Phase 3, before the next
contract freeze at I6.) Recorded review tiers: R2-C2 settled re-review —
dual, cross-model where available (mandated tier).

## Operator decision 2026-08-16 — thin A1

Authority: `dev-docs/GwzFasterProposal.md` (operator instruction; passing
that file is the L1-28 decision; committed with this checkpoint update).
Its §2, quoted verbatim:

> **Thin A1.** A1 enables the v1 writer and `--no-ff` on the accepted R4b
> lifecycle after R2-D settles and M5b's already-bound proofs (T-6 +
> clean-tree re-cut) are green. R2-E, R2-F, R3, R4, R5, and R6 are **not**
> A1 gates. They remain real work; they are hardening and consumer
> conversion, scheduled after A1 or in parallel, not in front of it.
>
> Residual you are ordered to accept and name: merge store, archive,
> stash bundles, and related consumers keep their current call graphs
> through A1; `recover_or_create` stays without a production caller;
> legacy writers may still mutate inside `.gwz/checked-artifacts` until
> R2-E. That residual is already the R2-D defer-out (`GwzM5-8R2D-Plan.md`
> §5 items 1–5). Coupling those items to A1 was a later gate-chain
> sentence, not a physical dependency of writing v1 records.
>
> **No further pre-A1 I2 contract trains.** A1 ships on the I2 contracts
> already frozen, plus amendments already accepted (including the durable
> cursor). The drafted operator-escape amendment
> (`GwzM5-8OperatorEscapeAmendment.md`, ~1,760 lines) and any further
> panic-invariant or escape-wire freeze are **not A1 gates**. Keep the
> v0 wedge runbook owed on its own (Q9). Do not launch mandated dual
> review of a third I2 train as a prerequisite to A1.
>
> **R2-D review caps (already adopted; now binding on this package).**
> Dual peer-blind review only at (a) the Phase 0 interface freeze, (b)
> the Phase 1 admission kernel if you still treat Idle↔Preparing as a
> durable-transition kernel, and (c) the Phase 5 settled-tree gate.
> That is three duals maximum, not four. Interior steps are single-axis
> with automatic escalation on P0/P1/P2. Two-round remediation cap:
> a third new architectural root cause on the same object is
> redesign-or-accept, not RemPlan-5. P3s file and continue; they do not
> become packages and they do not enlarge R2-D. While Phase 0 is in
> review, start Phase 1 failing tests (`GwzProcessOptimization.md` §4.4).
>
> **Track P before the freeze is reviewed.** Before
> `GwzM5-8R2DInterfaceFreeze.md` goes to dual review, spike the admission
> publish/retire path on macOS and Windows against the already-sealed
> `publish_verified_no_replace` family. Do not freeze the four
> `managed_operation_unavailable` defaults into required methods until
> each new physical edge names an admitted primitive per platform
> (`GwzProcessOptimization.md` §3.1). Policy is already in force; apply
> it to this freeze.
>
> **Lane split.** You remain the sole writer on merge/catalog/R2-D
> (L1-06). Pre-A1 docs (escape amendment review, panic-conversion
> packages, runbook) and M5b proof/doc work are a second lane. Do not
> pick them up. Record the split in the checkpoint. If a second agent
> is not yet handed off, leave those items listed as "blocked on
> operator handoff" and continue R2-D.

Recorded consequences (its §3 Step A):

- **R2-D settle is the last catalog gate on the A1 path.** The gate-chain
  paragraph below is rewritten accordingly; the superseded clauses are
  named, with verbatim quotes, in `GwzM5-8ThinA1Amendment.md` (drafted
  with this update; mandated dual review as a process/scope amendment).
- **R2-D review tiers, recorded now (supersedes the four-dual listing
  in `GwzM5-8R2D-Plan.md` §4/§6/§9):** dual peer-blind at the Phase 0
  interface freeze, the Phase 1 admission kernel, and the Phase 5
  settled-tree gate — three duals maximum. Interior steps, including
  the Phase 3 and Phase 4 settles, are single-axis with automatic
  escalation on P0/P1/P2. Two-round cap; a third architectural root
  cause on one object is redesign-or-accept.
- **Second lane — blocked on operator handoff; this implementor does
  not pick these up:** operator-escape amendment review handling (its
  Code+State dual review was already in flight when this instruction
  arrived; it runs to completion and its reports file as NON-GATING
  for A1 — remediation and any acceptance ritual are second-lane),
  panic-conversion packages (audit item 5, incl. the two reachable
  class-B sites), the v0 wedge runbook reproductions/publication (Q9),
  and M5b proof/doc work (incl. the N-4 wording erratum).
- **Moved off the A1 gate list (tracked, non-gating):** the
  operator-escape amendment and any further I2 wire trains; panic
  conversions; the ARM64 EBADF substrate package; MAX_PATH relocation
  (Phase 4.3 still only *decides* coexistence); D2 stays release-gated
  (unchanged). Per the instruction's §5, unchanged and still binding:
  L1-03/13/14/15/16/17/19/26/32, wire-freeze-first + L2-04
  retained-reader harness, sealed publication (§4.1), v1 `cfg(test)`
  until the A1 activation review, M5b's zero-production-line ceiling
  with T-6 + clean-tree re-cut, and the R2-D stop clauses
  (RemPlan-4 :1082-1085) — thin A1 waives none of these.
- **Track P before freeze review:** the Phase 0 freeze memo must name
  an admitted primitive per new physical edge per platform (macOS and
  Windows spike against the sealed C2-proven family) before the freeze
  goes to dual review; the four `managed_operation_unavailable`
  defaults are not frozen into required methods until it does.

## Accepted through

- M5-8 packages through R4b P1/P2 remediation: closed (eleventh-round GO/GO).
- R2-C0: accepted (Interface-ReviewCode-3 / ReviewState-3).
- R2-C1: accepted (AggregateClassifier -2 rounds, no P0-P2).
- R2-C2: **accepted** 2026-08-15 at root `f7ba323` / gwz-core `0d8382e`
  after the round-4 GO/GO (see Next ordered actions item 3 for the full
  round history, deviation record, and closed obligations).

## Open findings and obligations

- Amendment-package dual review (mandated tier for amendments): filed as
  `GwzProcessAdoption-ReviewConsistency.md` (GO, 5×P3) and
  `GwzProcessAdoption-ReviewSafety.md` (NO-GO, 1×P2 + 4×P3). All findings
  from both reports were corrected in this same checkpoint; the safety
  reviewer's focused re-verdict on the corrections is recorded in
  `GwzProcessAdoption-ReviewSafety.md` §"Focused re-verdict". Model note:
  this round ran dual same-model (Claude) with fresh-context isolation
  because the second model is not invocable from this session; a
  cross-model pass by the operator's other agent remains open as an
  optional strengthening.
- R2-C2: execute the complete per-fault matrix (L2-14 form), then two
  settled-tree re-reviews (dual — mandated tier), cross-model.
- Windows backlog from the C1-era tree: two isolated compile corrections
  (`a350746`, `d84a30d` in the release-diagnostic clone) to port or
  supersede deliberately; 98-failure matrix to classify (checkpoint doc
  "Windows backlog").
- Pre-A1 checklist (`GwzM5-8ProgressReviewF5.md` §9), status 2026-08-15:
  **item 1 (I2 re-freeze) CLOSED** — the wire-doc banners/TD §1 landed
  2026-08-11 upstream; this docs commit adds the fourth stale contract
  (Compatibility, Amendment-1 §4 normal-build split) with banner + gate
  coverage (11 sources / 147 assertions), single-axis review GO
  (`GwzM5-8I2Refreeze-ReviewConsistency.md`, 2 informational P3s).
  **Item 3: all ten escape decisions DECIDED 2026-08-16 and the
  amendment DRAFTED.** `GwzM5-8OperatorEscapeDesign.md` §10 dispositions:
  Q1-Q7 = the design's recommendations (sixth top-level record field;
  the design's protocol shape; wire pre-A1 with implementation at or
  before A1; byte-identical restore; `--forget-refs` accepted; user
  docs + error pointer; quarantine/restore/force-abandon names).
  **Operator decisions**: Q8 `--reason` OPTIONAL, never mandatory;
  Q9 = (b) runbook-first, no v0 backport unless later requested;
  Q10 = per-side consent with the provable-collapse single-prompt
  refinement ("adopt both recommendations and proceed with the
  amendment"). `GwzM5-8OperatorEscapeAmendment.md` (1,760 lines,
  DRAFT) now carries the wire/transition/protocol deltas — third
  pre-A1 train on the I2 contracts; its §6.4 is the three-train
  composition authority; its §10 records design-citation corrections
  (e.g. `--force` not `--destructive`) and drafter refinements
  pending review adjudication (separate `BeginRollbackOverridden`
  edge because the design's predecessor set could not reach its own
  U1 wedge; doctrine rule 1 refined; `MergeQuarantine*` type prefix;
  16-hex confirm token). **Round-1 dual review COMPLETE and filed**
  (peer-blind, cross-axis): Code NO-GO — 0 P0 / 3 P1 / 5 P2 / 6 P3
  (`-ReviewCode.md`; verdict: all three P1s sentence-scale contract-
  delta closure gaps, core wire/transition engineering sound; wire
  allocations, sixth-field serde story, entry-edge repair all
  verified); State NO-GO — 0 P0 / 6 P1 / 5 P2 / 4 P3
  (`-ReviewState.md`; consent-digest chain, ordinary-abort terminal
  leak, rule-8 decidability, sidecar lifecycle). Blind convergence
  note: both axes independently found the unamended cursor-derivation
  contract (ActionJournal §2 :178-192 — Code P1-1 ≡ State P1-3).
  Recorded **NON-GATING for A1 and second-lane** per thin A1
  (`GwzFasterProposal.md` §2/§4): the merged remediation round (round
  2 of 2 under the cap) and any acceptance are blocked on operator
  handoff, not this implementor's work. Side finding routed: the accepted
  durable-cursor amendment's "post-GC record rewrite" phrasing is
  inaccurate (`merge/gc.rs:196` shapes the response projection only)
  — wording erratum for its next docs pass, no behavior involved.
  (Design DRAFT status otherwise unchanged; the amendment supersedes
  it where they disagree, per the amendment's authority clause;
  v0-line wedge runbook stays owed independent of A1 per Q9.)
  **Item 5 audit
  half done** — `GwzM5-8PanicInvariantAudit.md` (104 sites: 82 class-A
  proven, 2 class-B reachable with typed twins, 11 class-C; ~300-LOC
  pre-A1 conversion plan in 7 packages; 3 of the review's original sites
  already resolved by the landed P1 slice). Item 4 (durable
  preservation-cursor) is DECIDED — D3 adopted, wire accepted
  (`GwzM5-8DurableCursorAmendment.md`); its implementation package
  belongs to the A1 window, not started per thin A1 §3. Still open:
  `decode.rs:86` removal tied to the A1 diff; the item-5 conversion
  packages are SECOND-LANE (blocked on operator handoff, non-gating).
- Rulebook P3 residue: section-anchor checking in the doc gate (Phase 2
  tooling).

## Windows platform campaign (2026-08-15)

Ledger: `GwzWindowsMatrix-Classification.md`; diagnosis:
`GwzWindowsMatrix-ExactEvidenceDiagnosis.md`. Burn-down 126 → 52 unique
failures across runs 1-6 (W1 index canonicalization, W2 fixture branches,
W3 read-only fsync, W4 publication sharing: all extinct at zero); run 6b
replay bit-identical (deterministic tail, no flakes). Run-7 train
(`f2fceaf`, dispatched 31886821459) carries the sharing-tail disposition
(2 fixed / 6 via the parent.rs publish-handle fix / 15 OS-impossible
injections gated with a positive Windows guarantee test), the os-87
rename twin fix, CRLF/longpaths fixture pins, the W6 generator install,
and cache-on-failure. **CAMPAIGN COMPLETE 2026-08-16: run 11 (`f36d20d`) is GREEN —
1306/0/1 + 29/0; the platform matrix is ACCEPTED** (eleven-run
burn-down 126 → … → 1 → 0; the run-9 regression and its mechanism are
recorded in the ledger; the finalization_root closure came via the
`probe/finalization-diagnosis` single-test probe unmasking a fixture
identity gap). **Revalidated at run 13 (`90d3f8a`): Windows 1322/0/1;
the sibling platform run's macos-14 leg is 1359/0/1 — first macOS
green. Three of four platform legs green; the ubuntu-24.04-arm leg
stays 1094/266/1 (one EBADF substrate fault + cascades, its own
tracked package; lead: linux.rs:173-181 errno allowlist).**
Matrix-green does NOT close the tripwired residuals
(real-Windows exact-evidence satisfiability; stash_save filtered
reset) nor the amendment's OPEN DECISIONS — all remain tracked below.
Next per gate chain — **thin A1** (operator decision 2026-08-16,
`GwzFasterProposal.md` §2, superseding this checkpoint's SCOPE
CORRECTION of the same date; superseded clauses quoted in
`GwzM5-8ThinA1Amendment.md`, ACCEPTED 2026-08-22 GO/GO): **R2-D
settle is the last catalog gate
on the A1 path.** A1 enables the v1 writer and `--no-ff` on the
accepted R4b lifecycle after R2-D settles (its Phase 5 dual gate) and
M5b's already-bound proofs (T-6 + clean-tree re-cut) are green.
R2-E, R2-F, R3, R4, R5, and R6 are NOT A1 gates — real work, kept,
scheduled after A1 or in parallel as hardening/consumer conversion,
with the named accepted residual recorded in the thin-A1 section
above. Position: R0-L and R1 accepted; **mid-R2** (the
catalog-bootstrap slice C0/C1/C2 accepted; R2-D ADOPTED and in Phase
0, TDD-gated per the amendment).
Scoping: `GwzM5-8R2D-Plan.md` is **ADOPTED 2026-08-16** (lane owner;
§9 adoption record carries the six §7 decision dispositions — C3 as
Phase 1 badged R2-C tail; quarantine/relocation preferred direction
for 4.3 coexistence, decided before catalog activation; backend-trait
delta confirmed for the 0.1 freeze; fault map confirmed; dirent
resume-window conditional; tiers recorded at freeze). Execution
begins at Phase 0: `GwzM5-8R2DInterfaceFreeze.md` + failing-test
scaffolding under one mandated dual review.
`GwzM5-8R2F-EvidenceMap.md` (R5/R2-F platform-evidence gap map vs the
green matrix) remains adopted-in-part (this-week list executed).

**A1 decisions ADOPTED 2026-08-16** (operator delegation "proceed with
your recommendations when the decision packet arrives";
`GwzM5-8A1DecisionPacket.md`): **D1** real-Windows satisfiability =
Option B, creation-time filter neutralization (autocrlf=false + eol=lf
pins at create_repo, clone filters-off at the transport funnel;
renormalize command for adopted worktrees post-A1; plus the permanent
fail-closed doctrine note for ident/eol=crlf/foreign residue and an
un-pinned CRLF matrix sentinel ending CI blindness). **D2**
foreign-filter policy = A′ refined refusal (pre-checkout attribute
inspection over the rewrite set, non-passthrough filters refused
pre-mutation, lfs allowlisted) — **release-gated, not A1-gated**: must
land before the next release cut, which is the first to carry the
amendment's disable_filters code. **D3** durable preservation cursor =
minimal durable cursor (per-owner no-op skip rows + reset-completion
bit) as a pre-A1 I2 amendment (mandated dual review; implementation
may trail into the A1 package; may share the escape-design amendment
window without hard-coupling to its pending owner decisions).
Implementation: D1+D2 as one filter-policy package (focused State
review; dual where the doctrine note freezes text); D3 amendment
drafting begins now.

**M5b-IF FROZEN 2026-08-16 (GO/GO)** at design `66117b0`
(`GwzM5-8M5bNoFfDesign.md`; reviews + re-verdicts filed in
`GwzM5-8M5bNoFf-Review{Code,State}.md`): M5b is
semantics-installation-only — the amended contracts already carry all
no-ff wire; the delta is proofs/tripwires/W1 doc amendment under a
ratified zero-production-line ceiling. Settled acceptance is BOUND to
T-6 (the "v0 forged-action resume gate" package — the freeze review's
Code F-1 P1 found production v0 resume executes forged
two-parent-over-ff-able actions today — landed with its two named
suites green) and to a clean-tree tuple re-cut. M5b-IMPL review tier
recorded at freeze: mandated-dual by default, single-axis only for
test/fixture-confined diffs. IMPL merge waits for R4b-G per the frozen
dependency statement. Tracked: round-3 N-4 (P3 cross-axis wording
reconciliation, next docs pass). D3 amendment: State re-verdict GO at
`e9396a9`; Code re-read pending; acceptance ritual (§7 contract
annotations) on its GO.
Cleanup candidates for the operator: remote branches
`probe/exact-evidence-diagnosis` and `probe/finalization-diagnosis`
(throwaway probes, safe to delete).

Historical record of the campaign's final stretch: run 7 left 24
survivors, all in the (b) exact-evidence cluster. The
**exact-evidence amendment is ACCEPTED 2026-08-16** and lands in this
commit: `GwzM5-8ExactEvidencePlatformAmendment.md` (recovery-grade rewrite
edges blob-exact; checked-artifact private area invisible to the
preservation-image model and recovery cleanliness predicates), dual
review round 1 Code **GO** / State doc-only NO-GO → document-only
remediation → State focused re-verdict **GO** (reports:
`GwzM5-8ExactEvidenceAmendment-ReviewCode.md` / `-ReviewState.md` with
appended re-verdict); three red-green Unix repros; four amended
contracts carry acceptance annotations; probe branch confirmed the
runner's system `core.autocrlf=true` precondition. Run 8 dispatched on
this commit. **Open review debts from acceptance (tracked, not
closed)**: the foreign-filter policy OPEN DECISION (clean-idempotence
precondition; git-crypt-class wedge) and the real-Windows raw-byte
satisfiability follow-up (unrewritten smudged files; ordinary CRLF
worktrees remain outside the satisfiable set — ledger tripwire in
`GwzWindowsMatrix-Classification.md`). New tracked items from the campaign:
resolver Publication-arm diagnosability (`execution.rs:16-21` masks
publication failure causes); MAX_PATH product exposure (~173-char
`ca1-*` names; the private-area relocation option under `.git/` would
retire it — candidate for R2-F scope); rollback preflight anchor-dirt
(in the amendment lane's scope addition).

## Next ordered actions

1. ~~Verify the §4.1 sealed-primitive implementation~~ **Done 2026-08-15**:
   `GwzM5-8R2C2PublicationAudit.md` — §4.1 satisfied, no P0-P2, four P3
   dispositions recorded (reserved fault families rescope; sealing
   perimeter extension as bounded package; legacy coexistence to R2-D
   scope; three strict-window tests).
2. Execute the complete per-fault interruption/recovery matrix; record
   per-key executed evidence (not inventory). Progress 2026-08-15: the
   21-key `catalog_bootstrap.*` matrix and the full checked_artifact suite
   executed green on this host (240 passed / 0 failed, macOS,
   workspace-target variant; command `cargo test --lib checked_artifact::`);
   the three P3-4 strict-window tests are implemented
   (`mutation_tests.rs`: in-place byte drift, destination-in-window,
   kind-swap-in-window) and pass; the reserved-families rescope note is
   filed as RemPlan §10. **Complete 2026-08-15**: the §6 parent-creation
   edge is keyed (`catalog_bootstrap.git_parent_create` /
   `.git_parent_reobserve`, family now 23 keys, inventory 163) and the
   matrix is extended to Git-directory targets
   (`restart_and_substitution_matrix_covers_git_directory_targets`, all 23
   keys interrupted+restarted+converged); workspace matrix completeness
   assertion now spans both target variants; checked_artifact suite
   241 passed / 0 failed (macOS host); boundary checker green after the
   deliberate protected-tree digest update for the two edited trees; doc
   gate green. Native Linux/Windows execution remains at the R2-F gate.
3. Settled-tree R2-C2 re-reviews, round 3 (on `c436180`): **State-2 GO**
   (0 open P0-P2; two new P3s routed — the Git-parent dirent-barrier gap to
   the next matrix package/§6 errata, the Windows destination residual to
   R2-F criteria) and **Code-3 NO-GO** (P2-1 directory-interior
   acquisition-window gap, probe-proven; P3-1 checker blind to raw renames
   outside two files, exit-criterion violating; P3-2 comment overclaim;
   P3-3 digest-discipline break at `95d292f`; P3-4 allocation parity; P3-5
   stale tuple table). **Round-3 remediation implemented in this commit**:
   in-window interior re-check inside the sealed primitive for directory
   sources + destination-interior re-check for retirement (Code-3 option
   a), two new hooks + two inverted-probe regression tests (243/243 green);
   subsystem-wide raw-rename caller inventory in the boundary checker with
   exact allowlist + two adversarial checker unit tests; platform.rs
   comment corrected and §4.1 erratum filed (verification assigned to
   round 4); `try_reserve_exact` parity in the primitive; same-commit
   digest refresh (P3-3 discipline adopted as a lane rule). **Round-4
   verdicts: Code-4 GO (0 open P0-P2, 2 new P3) and State-3 GO (0 open
   P0-P2, 1 new P3) — `GwzM5-8R2C2OwnerInterface-ReviewCode-4.md` /
   `-ReviewState-3.md`. R2-C2 is ACCEPTED at root `f7ba323` / gwz-core
   `0d8382e` under RemPlan §9 item 12.**

   Deviation record (per Code-4 [P3-2], L1-16): the remediation's first
   coordinated commit (root `dea0953` / gwz-core `b923109`) landed with the
   boundary gate red — the platform.rs flat source pin was stale after a
   comment-only edit, and a piped invocation masked the checker's exit
   code on the lane; the item-3 sentence above ("same-commit digest
   refresh") therefore overstated adherence for that commit. Corrected one
   commit later (`0d8382e`, pin-only). Lane rules now: gate exit codes
   checked directly (never through a pipe), digest refresh as the literal
   last pre-commit step, and a per-commit lane-gate mechanism is a tracked
   obligation below.

   Tracked obligations from rounds 3-4, status 2026-08-15 (2):
   **Done** — bare-identifier raw-rename counting + alias/fn-pointer probe
   tests (`641f03c`; the rebinding evasion both round-4 reviewers found now
   fails closed, and the §8.13 syntactic-vs-functional interpretation
   carries no load); per-commit lane gate as a mechanism (`89b414a`:
   `scripts/checks/check_lane_commits.sh` runs every pushed commit's own
   boundary checker over the push range, wired into
   `checked-artifact-boundary.yml` with full-history checkout; validated
   red against both historical deviation commits and green on the
   post-floor range; documented floor `ca520e4` excludes the two recorded
   pre-mechanism reds); I2 supersession items verified already fixed
   upstream (banners, updated bodies, TD §1 "as amended", manifest
   coverage — landed 2026-08-11 on the dev line) and hardened with five
   retired-spelling/stale-claim forbidden tripwires (`ca520e4`, doc gate
   now 138 assertions); DirectoryInteriorRecheckV1 doc-comment pointer
   fixed (`641f03c`).
   **Done** — State-2 [P3-1] Git-parent dirent barrier: **CLOSED** at
   gwz-core `660f46c` (AlreadyExists-arm barrier + containing-root flush
   anchored to the scratch edge — the §3/§6 durability-claim anchor;
   family 24 keys, inventory 164, both matrices executed; entrant-arm
   regression drives the real AlreadyExists path). Focused State-axis
   review GO, no escalation:
   `GwzM5-8R2C2DirentBarrier-ReviewState.md`. Its two P3s: the
   resume-window residual (exact-scratch resume paths skip the anchor —
   strictly narrower than what it replaced; correction options recorded in
   the report) routes to the next durability package or a §6-style erratum
   alongside the R2-F power-loss item; the comment/label pass (§5→§3/§6
   miscite, Windows-arm rationale, error label) is applied in the same
   closure commit.
   **Resolved 2026-08-15 (3)** — the standing CI red below was diagnosed
   as a `clippy::redundant_guards` lint in the Linux-only
   `provider/platform/linux.rs:177` (compiled by Ubuntu CI, never by macOS
   dev machines — which is why every local gate missed it; the
   probe-lockfile hypothesis was wrong: the probe already pins the
   lockfile). Fixed by folding the guard into its pattern (gwz-core
   `6edb9cb`); boundary run `31880974224` is the workflow's first recorded
   green push run, which also constitutes the per-commit lane gate's first
   successful CI execution. Lane ritual gains: cross-target
   `cargo clippy --target x86_64-unknown-linux-gnu` where the host
   toolchain permits (full-crate blocked by native build scripts; CI
   remains the authority). Historical record follows:
   standing CI red (pre-takeover): the push-triggered
   `checked-artifact-boundary.yml` run has failed on every push since at
   least 2026-08-14 (runs 31839074966, 31839893461, 31840579164,
   31880023029). Failing element: unit test
   `test_approved_outside_source_target_cannot_hide_an_observer_caller`,
   whose compiler probe builds a temp copy WITHOUT the lockfile (fresh
   `crates.io`/git resolution) and dies on `error: redundant guard`
   (clippy escalation) — environment-coupled, passes locally with warm
   caches. Fix package queued: pin the probe to the repository lockfile
   (`--locked` + copy `Cargo.lock` into the probe tree) and re-verify; the
   per-commit lane-gate step in the same workflow is masked by this
   earlier failure and its own outcome is unknown on CI until fixed.
   Also open — legacy
   interior digest-pinning or conversion at R2-D; Windows
   destination-window + object-binding native tests at R2-F; §2.2
   status-strip **done** at `53323d0` (Refactor.md −99 lines to a
   three-sentence status + changelog, ledger −10, judgment calls recorded
   in the strip report; doc gate 138 green; review-basis list converted to
   series form, unblocking eight more L1-33 archivals — 18 superseded
   rounds now live in `dev-docs/history/`). Remaining archive candidates
   stay pinned by latest-round closure tables and the RemPlan preamble; a
   small L1-08 amendment to L1-33 (closure-table citations do not pin,
   since archived files keep their names) is queued as a future docs
   package.
4. ~~Port/supersede the two Windows compile corrections~~ **Done by
   determination 2026-08-15**: both diagnostic-clone commits were located in
   `/Users/owebeeone/limbo/gwz-core-v0.10.5-narrow` and verified already
   incorporated on the dev line by `f532b1a` — `a350746` ("keep Windows
   anchor names native": all three hunks, the `anchor_roundtrip_name`
   helper, and its `remains_native` test present verbatim in
   `platform.rs:418/495/500/521+`) and `d84a30d` ("use stable Windows file
   identity": `open_named_path`/`identity_from_file` helpers widened,
   re-exported at `record_wire/mod.rs:48`, consumed at
   `abort.rs:103/249/261`, in the equal-or-stricter test-gated form). This
   resolves ReviewCode-3's f532b1a scope-bleed residual: that bleed was the
   unlabeled Windows port. Native Windows compile/run of the incorporated
   form remains unverified from this host (libz-sys cross-compile limit)
   and is owed at the R2-F gate or the next Windows dispatch run.
   **Remaining from this item — superseded 2026-08-15**: push was
   authorized, the dispatch-only `windows-matrix.yml` workflow was added,
   and the classification/burn-down is running as the Windows platform
   campaign (see that section above; ledger in
   `GwzWindowsMatrix-Classification.md`).
5. ~~I2 supersession banners + TD §1 + status-strip~~ **Done**: wire-doc
   banners/TD §1 landed 2026-08-11; status-strip at `53323d0`; the
   Compatibility-contract banner + gate coverage land in this docs commit
   (review GO — see the pre-A1 bullet above).
6. Then platform-matrix acceptance (campaign section above) → R4b-G →
   M5b → A1 per `GwzMergeCheckpoint-v0.10.5.md` resume order.

## Metrics (per checkpoint; §6 of the optimization plan)

| Checkpoint | Sessions | Review rounds | Found at freeze / impl / settled / escaped |
| --- | ---: | ---: | --- |
| (baseline starts with the next accepted checkpoint) | | | |
