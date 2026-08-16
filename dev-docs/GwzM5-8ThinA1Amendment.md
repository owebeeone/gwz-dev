# M5-8 thin-A1 gate-chain amendment

Date: 2026-08-16

Status: **DRAFT — pending mandated dual review** (process/scope
amendment tier per `GwzProcessOptimization.md` §4.2; Consistency and
Safety axes, cross-model per §4.3, two-round remediation cap in force).

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

Additionally controlling, from the same §2: no further pre-A1 I2
contract trains as A1 prerequisites (the drafted
`GwzM5-8OperatorEscapeAmendment.md` and any further panic-invariant or
escape-wire freeze are non-gating, second-lane); and the R2-D review
caps — **three duals maximum** (Phase 0 interface freeze, Phase 1
admission kernel, Phase 5 settled-tree gate), all interior steps
single-axis with automatic escalation on P0/P1/P2.

## 2. Superseded clauses, verbatim

1. `GwzM5-8R4bP1P2-RemPlan-4.md:4-6` (Status): "R2 is next and R4b-G
   remains gated by R2-R6". **Superseded**: R4b-G on the A1 path is
   gated by the R2-D settled gate alone; R2-E/R2-F and R3-R6 are
   post-A1 or parallel.
2. `GwzM5-8R4bP1P2-RemPlan-4.md:1142-1143` (§R6): "Repeat the two
   independent reviews against the exact settled tuple. R4b-G resumes
   only when both report no open P0/P1/P2." **Superseded as an A1
   gate**: the R6 independent settled-tree re-reviews remain the
   acceptance gate of the post-A1/parallel hardening chain itself,
   but R4b-G no longer waits on them.
3. `GwzM5-8R4bP1P2-RemPlan-4.md:1240` (§"controlling six-step
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

No other live sentence is known to sequence R3-R6 in front of R4b-G
or A1. If the dual review finds one, it is added here by remediation,
not by a new document.

## 3. Why this is sound

The coupling of R2-E-R6 to A1 was a gate-chain sentence, not a
physical dependency of writing v1 records:

- The v1 writer rides the accepted R4b reverse lifecycle, the frozen
  I2 contracts, and the already-accepted amendments (exact-evidence,
  durable cursor). None of those artifacts depend on consumer
  conversion (R2-E), closure matrices (R2-F), or R3-R6 for their
  reviewed correctness claims.
- The durable kernel edges the writer itself exercises are R2-D scope
  (Phases 2-4 convert the four legacy rename edges and retire the
  legacy Windows anchor), and R2-D **remains** an A1 gate with its
  stop clauses intact.
- The deferred residual runs today's call graphs — the same graphs v0
  runs in production — currently green on Windows (matrix run 13,
  1322/0/1), macOS (1359/0/1), and the ubuntu ext4 probe; they are
  hardened by the post-A1 chain without wire or outcome changes.
- Review capacity is redirected, not reduced: the three retained duals
  sit exactly on the interface freeze, the durable-transition kernel,
  and the settled tree — the three places the program's history shows
  duals finding architectural defects (per the checkpoint's process
  metrics table).

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
- P0/P1/P2 handling, executed fault evidence, and handle-bound
  publication are untouched; thin A1 changes what A1 waits for, not
  how work is proven.

## 5. Affected documents

| Document | Effect |
| --- | --- |
| `GwzM5-8R4bP1P2-RemPlan-4.md` | Clauses §2.1-2.3 superseded; after this amendment's dual review passes, a one-line supersession banner is added pointing here. The document is not rewritten. |
| `CurrentProgramCheckpoint.md` | Quotes the decision (§2 of the instruction), rewrites the gate-chain paragraph, records the three-dual R2-D tiers and the second-lane split — done in the commit that files this draft. |
| `GwzM5-8R2D-Plan.md` | §1 chain-bullet reinterpreted and §4/§6/§9 tier sentences superseded per §2.5-2.6; not rewritten. Everything else in the adopted plan (phases, stop clauses, defer-outs, owner decisions) stands. |
| `GwzM5-8OperatorEscapeAmendment.md` | Unchanged content; recorded second-lane and non-gating for A1; its in-flight dual review completes and files. |
| I2 contracts, M5b design, A1 decision packet | Untouched. No wire, type, or outcome change anywhere in this amendment. |

## 6. Discharge

On GO/GO from the dual review: status flips to ACCEPTED, the RemPlan-4
banner lands, and the checkpoint references this amendment as the
controlling gate-chain text. On findings: one merged remediation round
under the two-round cap; a third architectural root cause is
redesign-or-accept per the instruction.
