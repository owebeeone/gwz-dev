# M5-8 thin-A1 gate-chain amendment

Date: 2026-08-16

Status: **DRAFT — round-2 revision of 2026-08-22, pending focused
re-verdicts** (process/scope amendment tier per
`GwzProcessOptimization.md` §4.2; Consistency and Safety axes,
cross-model per §4.3, two-round remediation cap in force — this
revision is the one merged remediation, addressing
ReviewConsistency P1-1/P1-2 + P2-1..P2-4 and ReviewSafety
P1-1/P1-2/P1-3 + P2-1/P2-2).

Authority: `dev-docs/GwzFasterProposal.md` — operator instruction of
2026-08-16; passing that file is the L1-28 decision. This amendment is
its §3 Step B deliverable: it satisfies L1-08 by naming the superseded
clauses verbatim, stating the new controlling rule, saying why, and
listing the affected documents. It is deliberately small. It reopens
no I2 wire, adds no types, and folds in no C3/R2-E/escape scope.

## 1. New controlling rule

Quoted from `GwzFasterProposal.md` §2 (the full §2 is quoted verbatim
in `CurrentProgramCheckpoint.md`, "Operator decision 2026-08-16 — thin
A1"):

> **Thin A1.** A1 enables the v1 writer and `--no-ff` on the accepted
> R4b lifecycle after R2-D settles and M5b's already-bound proofs (T-6
> + clean-tree re-cut) are green. R2-E, R2-F, R3, R4, R5, and R6 are
> **not** A1 gates. They remain real work; they are hardening and
> consumer conversion, scheduled after A1 or in parallel, not in front
> of it.

Named accepted residual (ordered by the same instruction): merge
store, archive, stash bundles, and related consumers keep their
current call graphs through A1; `recover_or_create` stays without a
production caller; legacy writers may still mutate inside
`.gwz/checked-artifacts` until R2-E. This residual is exactly the
R2-D defer-out (`GwzM5-8R2D-Plan.md` §5 items 1-5).

**Also part of the named residual (ReviewSafety P1-1): the R4b
lifecycle acceptance debt.** The v1_lifecycle P2/P3/P4 lanes —
~3.5k production lines (`GwzM5-8ProgressReviewF5.md` §2.1) — are
implemented but not yet independently accepted; their only scheduled
independent acceptance was the now-deferred R6, and the open
accepted-real D3/D4 P2 classes (dirty-boolean recovery predicates;
`preservation/cursor.rs:277-345`) had their closure scheduled in
R3/R4. Under thin A1 this debt **rides to the A1 activation review**
— dual, at §4.2's activation tier — which must either (a) subsume a
settled-tree acceptance of the `v1_lifecycle` tree with an
operator-signed named-residual disposition for the D3/D4 classes, or
(b) treat them as blocking per L1-19. §4 cross-references this
obligation; it is a named exception, not a silent weakening.

Additionally controlling, from the same §2: no further pre-A1 I2
contract trains as A1 prerequisites (the drafted
`GwzM5-8OperatorEscapeAmendment.md` and any further panic-invariant or
escape-wire freeze are non-gating, second-lane); and the R2-D review
caps — **three duals maximum** (Phase 0 interface freeze; Phase 1
admission kernel — carrying the instruction's own hedge, "if you
still treat Idle↔Preparing as a durable-transition kernel", which
this program does; Phase 5 settled-tree gate), all interior steps
single-axis with automatic escalation on P0/P1/P2.

## 2. Superseded clauses, verbatim

1. `GwzM5-8R4bP1P2-RemPlan-4.md:5-7` (Status): "R2 is next and R4b-G
   remains gated by R2-R6". **Superseded**: R4b-G on the A1 path is
   gated by the R2-D settled gate alone; R2-E/R2-F and R3-R6 are
   post-A1 or parallel.
2. `GwzM5-8R4bP1P2-RemPlan-4.md:1142-1143` (§R6): "Repeat the two
   independent reviews against the exact settled tuple. R4b-G resumes
   only when both report no open P0/P1/P2." **Superseded as an A1
   gate**: the R6 independent settled-tree re-reviews remain the
   acceptance gate of the post-A1/parallel hardening chain itself,
   but R4b-G no longer waits on them.
3. `GwzM5-8R4bP1P2-RemPlan-4.md:1238` (§"controlling six-step
   sequence"): "Only then does step 4 of the controlling six-step
   sequence—R4b-G—begin." **Superseded** the same way: "only then"
   now binds the hardening chain's own completion claim, not R4b-G.
4. `CurrentProgramCheckpoint.md`, SCOPE CORRECTION 2026-08-16 (as it
   stood until this train's checkpoint commit): "Next per resume
   order — SCOPE CORRECTION 2026-08-16: the gate to R4b-G is
   **RemPlan-4's R2-R6 chain** (`GwzM5-8R4bP1P2-RemPlan-4.md` §4), not
   merely two tracked items; … then R3 complete-checkout/aggregate
   handoff, R4 executable-C7 reconciliation, R5 settled gate with real
   three-platform evidence jobs, R6 independent settled-tree
   re-review." **Replaced** in the same commit that files this
   amendment by the thin-A1 gate-chain paragraph.
5. `GwzM5-8R2D-Plan.md` §1, first controlling-document bullet: "the
   R2–R6 chain that gates R4b-G". **Reinterpreted**: the chain remains
   the catalog program's order of work; it is no longer the A1 gate.
6. `GwzM5-8R2D-Plan.md` §6 ("Nominal: 4 dual gates (0.1, Phase 1,
   Phase 3/4 settles, 5.2)"), the §4 Phase 3/Phase 4 "Gate: dual"
   sentences, and the §9 adoption record's confirmation of those
   tiers. **Superseded** by the three-dual cap of §1 above: the Phase
   3 and Phase 4 settles run single-axis with automatic escalation.
   (The plan document is not rewritten; this clause plus the
   checkpoint tier record control.)

7. `GwzM5-8R4bR2ConsumerCheckpoint.md:21-23`: "R3-R6 in
   `GwzM5-8R4bP1P2-RemPlan-4.md` remain required after R2. R4b-G does
   not begin until those phases and their independent settled-tree
   reviews pass." **Superseded as an A1/R4b-G gate**: R3-R6 remain
   required work of the hardening chain; R4b-G no longer waits on
   them. (ReviewConsistency P1-1; ReviewSafety P1-2.)
8. `GwzM5-8R4bR2ConsumerCheckpoint.md:404` (§15 acceptance gate):
   "Only then may RemPlan-4 R3 begin. R4b-G remains behind R3-R6."
   **Superseded as an A1/R4b-G gate** the same way; the first
   sentence (R3's own precondition) is untouched.
9. `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:605-607`: "R2-D through
   R2-F follow accepted R2-C. R3-R6 and R4b-G shared-router/
   cross-driver integration follow, then the two final full-tree R4b
   reviews. M5b and A1 remain later." **Superseded as sequencing
   authority for R4b-G/M5b/A1**: R2-D still follows accepted R2-C
   (unchanged); R4b-G, M5b, and A1 now follow R2-D settle; R2-E/R2-F
   and R3-R6 run post-A1 or parallel.
10. `GwzM5-8M5bNoFfDesign.md:987-989` (frozen GO/GO design, §8.2
    area): "The R4b-G gate itself is behind the RemPlan-4 R2-R6 chain
    (scope correction of 2026-08-16 …); M5b inherits that
    transitively and takes no dependency on its internals."
    **Superseded as a dependency statement**: R4b-G is behind R2-D
    settle alone; M5b's inheritance clause tracks the new gate the
    same transitively-no-internals way. Because the design is frozen,
    the correction lands as a dated annotation pointing here, applied
    after this amendment's review passes — the design's semantics,
    ceiling, and T-6 binding are untouched (§5 row updated
    accordingly). (ReviewConsistency P1-2; ReviewSafety P1-2.)
11. `GwzM5-8R4bR2ConsumerCheckpoint.md:375-386` (§14), ninth stop
    condition — R2 stops on "production merge-v1 decode, write,
    migration, or dispatch reachability". **Re-scoped, not deleted**:
    it binds every pre-A1 R2 package unchanged; from A1 activation
    onward, production v1 reachability is governed by the A1
    activation review's acceptance (with the §1 acceptance-debt
    obligation), and the post-A1 R2-E/R2-F packages run under the
    remaining eight §14 stop conditions, which are unchanged.
    (ReviewSafety P1-2.)
12. `GwzProcessOptimization.md:106-114` (adopted process rule):
    "Dual peer-blind review stays mandatory at: interface freezes,
    durable-transition kernels, amendments, aggregate settled-tree
    gates, activation, and release boundaries." **Operator-overridden
    in one respect (L1-28, `GwzFasterProposal.md` §2)**: the R2-D
    Phase 3 and Phase 4 settles — durable-transition kernels by the
    adopted plan's own classification (`GwzM5-8R2D-Plan.md:387-388`,
    `:426-428`) — run single-axis with automatic escalation on
    P0/P1/P2 under the three-dual cap. The rule is otherwise intact
    (this amendment itself takes the mandated dual; Phase 0/1/5,
    R4b-G, A1 activation, and release boundaries keep theirs). The
    override is the operator's accepted risk: the recorded review
    history shows axis-divergent detection at settles (R2-C2 round 3:
    State GO while Code NO-GO on a probe-proven acquisition-window
    gap), which escalation-on-finding cannot reproduce — named here
    so the trade is explicit, not silent. (ReviewSafety P1-3.)

Secondary status-class, deferred, or draft-document sentences that
restate the old chain (e.g. `GwzM5-8R4bR1InterfaceCheckpoint.md:7`,
`GwzM5-8R4bR1Interface-RemPlan.md:6`) are live-status paragraphs
auto-superseded by the checkpoint under rulebook §7.3 and are named
here so that supersession is explicit rather than inferred; the
R4b-G execution checklist `GwzM5-8R4bG-EvidenceInventory.md`
(:8/:121/:374/:597), whose frame assumes the R2-R6-settled tree,
is DRAFT material re-framed by a banner per §5. (ReviewConsistency
P2-4; ReviewSafety P1-2 items 3-4.)

No other live sentence is known to sequence R3-R6 in front of R4b-G
or A1. If the re-verdict round finds one, it is added here by
remediation, not by a new document.

## 3. Why this is sound

The coupling of R2-E-R6 to A1 was a gate-chain sentence, not a
physical dependency of writing v1 records:

- The v1 writer rides the R4b reverse lifecycle — accepted at its
  design, interface, and R0/R1 checkpoints, with the P2/P3/P4
  implementation lanes' independent-acceptance debt **named in §1**
  and riding to the A1 activation review rather than silently waived
  — plus the frozen I2 contracts and the already-accepted amendments
  (exact-evidence, durable cursor). None of those artifacts depend on
  consumer conversion (R2-E), closure matrices (R2-F), or R3-R6 for
  the correctness claims they have actually been reviewed to.
- The durable kernel edges the writer itself exercises are R2-D scope
  (Phases 2-4 convert the four legacy rename edges and retire the
  legacy Windows anchor), and R2-D **remains** an A1 gate with its
  stop clauses intact.
- The deferred residual runs today's call graphs — the same graphs v0
  runs in production — currently green on Windows (matrix run 13,
  1322/0/1), macOS (1359/0/1), and the ubuntu ext4 probe; the fourth
  leg, ubuntu-24.04-arm, remains red (1094/266 — one EBADF substrate
  fault plus cascades, tracked as its own non-gating package). The
  residual is hardened by the post-A1 chain without wire or outcome
  changes.
- Review capacity is concentrated where the recorded history shows
  duals earning their cost — freezes, kernels, and settled trees
  (R2-C2's settled round 3: State GO while Code NO-GO on a
  probe-proven acquisition-window gap; the escape amendment's round 1:
  a 3-vs-6 P1 split with one blind-convergent finding). That same
  record is why the Phase 3/4 downgrade is booked in §2.12 as a named
  operator override with its risk stated, not as a cost-free
  redirection: single-axis escalation fires only when the scheduled
  axis finds something, and the recorded cases are axis-divergent.

## 4. What does not change

Unwaived, per `GwzFasterProposal.md` §5 and restated here as binding
on every package this amendment touches:

- `AgentProcessRules.md` L1-03, L1-13, L1-14, L1-15, L1-16, L1-17,
  L1-19, L1-26, L1-32; wire-format freeze-first; the L2-04
  retained-reader harness.
- Sealed source-associated publication
  (`GwzM5-8R2CCatalogBootstrapAmendment.md` §4.1) and the R2 stop
  clauses (`GwzM5-8R4bP1P2-RemPlan-4.md:1082-1085`): no successful
  converted path may call the Windows no-op parent sync, perform an
  unbounded protocol read, allocate a fresh retry scratch name, lack
  pre-reserved cleanup capacity, or mutate the private/managed tree
  before capability and collision acceptance.
- v1 remains `cfg(test)` until the A1 activation review; no production
  `recover_or_create` caller lands before the Phase 4.3 coexistence
  decision; D2 foreign-filter policy stays release-gated; M5b stays
  semantics-installation-only under its zero-production-line ceiling
  with T-6 + the clean-tree re-cut.
- The R2-F/R5 **native-evidence release gates** are unwaived and
  unmoved (`GwzM5-8R2CCatalogBootstrapAmendment.md:667`;
  `GwzM5-8R4bP1P2-RemPlan-4.md:1131-1138`): they remain the
  native-platform barrier between the A1-enabled v1 writer and
  shipped users — thin A1 moves them off the A1 gate, not off the
  release gate. (ReviewSafety P2-1.)
- All nine §14 stop conditions bind every pre-A1 R2 package
  unchanged; §2.11 re-scopes only the ninth's post-A1 application.
- The §1 acceptance-debt obligation binds the A1 activation review:
  the v1_lifecycle P2/P3/P4 acceptance and the D3/D4 named-residual
  disposition are that review's explicit inputs, per L1-19.
- P0/P1/P2 handling, executed fault evidence, and handle-bound
  publication are untouched; thin A1 changes what A1 waits for, not
  how work is proven.

## 5. Affected documents

| Document | Effect |
| --- | --- |
| `GwzM5-8R4bP1P2-RemPlan-4.md` | Clauses §2.1-2.3 superseded; after this amendment's dual review passes, a one-line supersession banner is added pointing here. The document is not rewritten. |
| `CurrentProgramCheckpoint.md` | Quotes the decision (§2 of the instruction), rewrites the gate-chain paragraph, records the three-dual R2-D tiers and the second-lane split — done in the commit that files this draft. |
| `GwzM5-8R2D-Plan.md` | §1 chain-bullet reinterpreted and §4/§6/§9 tier sentences superseded per §2.5-2.6; not rewritten. Everything else in the adopted plan (phases, stop clauses, defer-outs, owner decisions) stands. |
| `GwzM5-8OperatorEscapeAmendment.md` | Unchanged content; recorded second-lane and non-gating for A1; its round-1 dual reports are filed. |
| `GwzM5-8M5bNoFfDesign.md` | Frozen design: semantics, ceiling, and T-6 binding untouched; its :987-989 dependency clause is superseded per §2.10 and receives a dated annotation pointing here after this review passes. |
| `GwzM5-8R4bG-EvidenceInventory.md` | DRAFT checklist whose frame assumes the R2-R6-settled tree (:8/:121/:374/:597, incl. a verbatim restatement of the superseded SCOPE CORRECTION at :121); receives a one-line re-frame banner pointing here after this review passes; its evidence items themselves are unaffected. |
| `GwzM5-8R4bR2ConsumerCheckpoint.md` + its RemPlan | Clauses §2.7-2.9 superseded as A1/R4b-G gates and §2.11 re-scoped; the documents are not rewritten. Status-class restatements elsewhere are §7.3-auto-superseded as named in §2's closing note. |
| I2 contracts, A1 decision packet | Untouched. No wire, type, or outcome change anywhere in this amendment. |

## 6. Discharge

On GO/GO from the dual review: status flips to ACCEPTED, the RemPlan-4
banner lands, and the checkpoint references this amendment as the
controlling gate-chain text. On findings: one merged remediation round
under the two-round cap; a third architectural root cause is
redesign-or-accept per the instruction.
