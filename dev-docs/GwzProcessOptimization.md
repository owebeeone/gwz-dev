# GWZ process optimization plan — speed without quality loss

Date: 2026-08-15
Status: **adopted 2026-08-15 by operator decision at the implementation
handoff (Claude/F5 takes the merge lane). Controlling process amendment to
`AgentProcessRules.md` where the two differ. Rollout per §7;
`CurrentProgramCheckpoint.md` records live state.**
Author: Claude (F5), from the verified findings in `GwzM5-8ProgressReviewF5.md`,
`GwzReleaseReview-v0.10.5-F5.md`, and `AgentProcessRules-ReviewF5.md`.

Objective ordering per operator: (1) quality of code — long-horizon
correctness under adversity, structurally enforced; (2) speed to solution.
Every change below names the guardrail that preserves (1) while buying (2).
Estimated aggregate effect: 30-50% cycle-time reduction at equal-or-better
defect outcomes. That estimate is a judgment; §6 makes it measurable and
every relaxation reversible.

## 1. What does not change

The load-bearing core stays exactly as written in `AgentProcessRules.md`:
characterization before change (L1-03); typed durable owner before mutation
(L1-13); whole-operation preflight (L1-14); closed recovery grammar (L1-15);
evidence vs inventory (L1-16); settled tuples for acceptance reviews (L1-17);
P0/P1/P2 block (L1-19); amendment discipline (L1-08/09); tag immutability
(L1-26); activation as a separate reviewed change (L1-32); wire-format
freeze-first and the retained-reader behavioral harness (L2-04). These are
what my reviews verified actually bought the quality.

## 2. Cut pure overhead (no quality interaction)

**2.1 Ledger arithmetic → tooling.** Retire hand-maintained LOC/path
charging, double-charging rules, and per-slice reconciliation. A script
(`git diff --numstat` classifier) reports production/test/moved per package;
the ledger keeps only two numbers per package (production LOC, files) plus
the qualitative stop triggers, which become review-time checks: new
production owner, wire-delta change, ownership crossing, >120% growth. A
ceiling revision must state what is descoped before it may state what is
raised.
Guardrail: the triggers that ever mattered are kept; only the arithmetic
goes. Evidence: the budget never bound (R4b-P 1,450→14,000 with no
descoping); reconciliation itself generated findings (P1-SR4, manifest
omissions); ~25% of amendment text was bookkeeping.

**2.2 One current-state document.** Create `CurrentProgramCheckpoint.md`
(exact tuple, controlling-document graph, accepted-through, open findings,
next action). Strip live status histories from `GwzM5-8Refactor.md` and the
ledger (one-time cleanup). Guardrail: L1-31 is strengthened, not weakened —
context lives in one place instead of smeared across three. Evidence: the
stale-I2 hazard and the §8/§15 contradictions are direct products of status
smearing; cold starts re-derive currency every session.

**2.3 Fix the contract-authority debt now.** Supersession banners in both I2
contracts pointing at the controlling amendments; correct the
TransitionDesign §1 authority sentence; extend `check_merge_docs.py`
manifest with the retired phase spellings, the banner requirement, and
section-anchor verification (the rulebook's own P2-1 class). Guardrail:
this is a pure quality fix that also saves every future reader a wrong turn.
Phase split: the banner text and manifest string entries are docs-adjacent
Phase 0 work; the section-anchor checker extension itself is Phase 2 tooling.

**2.4 Generate review prompts.** One canonical template + role section +
tuple, generated per review (rulebook §14.4 proposal 6). Kills prompt drift
and a manual step per round.

**2.5 History archiving (operator-added at adoption).** Superseded documents
that no longer carry weight move to `dev-docs/history/` under rulebook rule
L1-33: only fully superseded documents nothing current cites as controlling;
never controlling documents, latest review rounds, checker-manifest entries,
or rulebook-cited files; moves via `git mv`; `history/README.md` states that
nothing in the directory is normative. Applies to each repository's dev-docs
directory; sweeps recur at checkpoint boundaries. Guardrail: the citation
check before each move, and documentation gates green after.

## 3. Reorder where freeze-first fights physics

**3.1 Two-track freeze policy.** Track W (wire/durable formats): freeze
first, as now — this worked. Track P (physical I/O: rename, sync, identity,
locking, case behavior): a mandatory implementation spike on each target
platform *before* the contract freezes, proving the admitted primitive
exists and behaves. Freeze checklist gains three lines: one durable phase
per durability boundary; admitted primitive named per platform; no compound
action lacking a portable primitive. Guardrail: rigor is moved earlier, not
removed. Evidence: the compound-phase error survived six review gates and
died on first implementation contact; same pattern for the Windows dir-sync
no-op and the C2 verify/drop/rename seam. Freeze-then-discover produced the
largest amendment cascades in the corpus — this is the single biggest
schedule item after review volume.

**3.2 Native CI at the first platform-bound checkpoint** (rulebook proposal
4), not at release. A minimal Windows/macOS lane on the boundary crates.
Evidence: two Windows compile defects plus a 98-failure matrix arrived at
release time; each would have been a same-day fix at introduction time.

## 4. Bound the loops

**4.1 Two-round remediation cap** (rulebook proposal 3). A third new
architectural root cause on the same object stops the lane: redesign and
re-freeze instead of another guard. Evidence: eleven rounds on R2; the
rulebook's own honesty section says rounds became the architecture process.
Guardrail: the cap forces the *higher*-quality outcome (redesign) exactly
when evidence says the design is wrong. Reviewers, not the implementer,
classify whether a finding is a new architectural root cause. A third round
limited to non-architectural corrections is permitted; any architectural
root cause found in it triggers the cap.

**4.2 Tiered review depth.** Dual peer-blind review stays mandatory at:
interface freezes, durable-transition kernels, amendments, aggregate
settled-tree gates, activation, and release boundaries. Interior
implementation checkpoints behind an already-accepted interface get one
review, axis alternating (Code/State), with automatic escalation to the
second axis on any P0, P1, or P2, or on reviewer request.
Bounded-remediation re-reviews are single-axis by default. A checkpoint's
review tier is recorded in `CurrentProgramCheckpoint.md` when its interface
freezes — it is not chosen mid-lane by the implementer. Guardrail: the recorded
frame-divergence catches (State GO vs Arch NON-GO) happened at settled-tree
and interface gates — which keep dual review; the interior R4b defects were
overwhelmingly caught by gates and implementation contact. Monitored by
§6; reverts per-tier if escaped defects appear.

**4.3 Cross-model review.** Reviews run on a different model than the
implementer (Claude reviews GPT-5.6 lanes or vice versa). Same cost,
strictly more defect diversity than same-model role prompts; directly
addresses the independence ceiling (AgentProcessRules-ReviewF5 P3-1).

**4.4 Pipeline lanes.** While reviewers hold checkpoint N, the implementer
starts N+1 behind the frozen interface (already legal under L1-05; make it
the default schedule rather than the exception).

## 5. Reduce per-change mechanical cost in the code

**5.1 Table-driven transition specs** before the v2-v4 vocabularies land:
one declarative row per variant (predecessors, footprint, effect) from which
reducer guards, dispatcher classification, and matrix expectations derive.
Cuts the current ~6 hand-written touchpoints per semantic operation and
shrinks the drift surface. Bounded package; pays into every future wave.

**5.2 No new bespoke harnesses without a coverage claim.** A new test
apparatus must name the failure mode existing matrices do not cover.
Existing matrices stay. Evidence: harness growth is the dominant test-mass
cost; marginal bespoke harnesses added little over the closed-world
matrices.

## 6. Measure it — the safety argument

Per checkpoint, record four numbers in `CurrentProgramCheckpoint.md`:
sessions/wall-clock to acceptance; review rounds; defects by discovery phase
(freeze review / implementation contact / settled review / post-acceptance);
escaped defects (post-acceptance discoveries, the quality bottom line).

Success looks like: rounds down, implementation-contact discoveries down
(physics moved earlier), escaped defects flat at ~zero. Tripwire: any
escaped P0-P2 attributable to a relaxed tier reverts that specific
relaxation (not the whole plan); attribution is made by the re-reviewing
reviewer, not the implementer. This makes "speed without quality loss"
an observable claim instead of a promise.

## 7. Rollout

- **Phase 0 — docs only, hours, parallel-safe now.** §2.2, §2.3, and the
  AgentProcessRules amendment (its four P2 corrections + these deltas).
  Reviewed as one amendment round, cross-model.
- **Phase 1 — apply to R2-C2 remainder immediately.** Fault-matrix execution
  under the L1-16 evidence template; the two settled R2-C2 reviews stay dual
  (mandated tier) and run cross-model; two-round cap in force from now.
- **Phase 2 — tooling, one small package.** Ledger script, prompt generator,
  metrics fields (§2.1, §2.4, §6).
- **Phase 3 — before M6/I6.** Two-track freeze policy and native-CI lanes
  wired in (§3); transition-spec table package (§5.1) lands before the v2
  vocabulary multiplies its absence.

Adoption requires an operator decision (L1-28) and, per the process's own
rules, the rulebook amendment gets an independent review — which is cheap
and should be the first exercise of §4.3.
