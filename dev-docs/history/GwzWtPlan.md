# GwzWt Implementation Plan

Status: **ADOPTED 2026-09-01** — P0.1 ran GO-WITH-CONDITIONS
(`GwzWt-P0.1-Review.md`: 0 P0 / 4 P1 / 7 P2 / 7 P3), every finding folded
same day (§6), including two corrections to the ADOPTED design executed
under the design's own governs-rule (its §5.5 stale exclude instruction;
the new W-CMP-4 merge refusal). One item remains PENDING THE OPERATOR and
is flagged in §2. Authority: `GwzWtDesign.md` (adopted, as corrected) —
its W-* rows are the requirement surface; the design governs on
disagreement. **Implementation is chartered for non-Fable agents.**

The plan inherits probe-verified ground: the design review verified the
seam, the gitfile hazard, git2's per-operation capabilities, the
catalog-lease sharing, and the branch tri-state against git's real
errors; P0.1 additionally verified the seam's unconditional root
boundary-sync, the merge-evidence read fan-out, and the aggregate-gate
pin surface.

## 1. Scope decisions

1. **Deliverable set** = the design's v0: `gwz worktree
   add|list|remove|prune` (+`repair` SHOULD), the W-CMP compat scope
   (now four rows), intent records, three client channels, the W-CMD-6
   parity proof with its one named exception (W-CMP-4). No release cut.
2. **Module home:** `gwz-core/src/operation/worktree/` per the recorded
   ruling; intent records in a sibling artifact module.
3. **Risk concentrate:** S1.1 rewrites `workspace_exclude_path`, whose
   measured fan-out (P0.1 F7) is six production merge-evidence read
   sites across five files plus eleven test references (six under
   `workspace_ops/merge/v1_lifecycle/tests/`), and whose infallible
   signature becomes fallible — the churn propagates. Its proof set:
   byte-identical primary behavior; the merge-evidence hash regression;
   a COMPOSITE-side fixture proving the cure (the resolution returns
   the commondir path inside a real linked worktree — P0.1 F8: prove
   the fix, not only the non-regression); full partitions;
   real-workspace battery.
4. **Frozen-surface position (P0.1 F7), stated:** gwz-log's
   `v1_lifecycle` no-DIFF freeze was that plan's own charter clause,
   not a program rule; it does NOT bind GwzWt. S1.1's diffs into the
   merge trees are confined to the resolution call sites and their
   tests, protected by the §1.3 proof set. The checked-artifact
   digest-pinned trees remain out of bounds (that guard is machine-
   enforced, not plan prose). Flagged for the operator's awareness; the
   lane owner takes the position on the recorded evidence.
5. **LOC basis:** handwritten incl. tests, generated excluded. HARD
   caps are stop-and-report boundaries. **No step carries more than one
   semantic decision** (the gwz-log actuals: every ≥350-LOC step with
   two decisions died at the cap; the ≤250 steps did not — P0.1 F5's
   table).
6. **The -B rule, with its terminus (P0.1 F16):** a step dying at the
   cap is re-chartered once as `<step>-B` (in-place in §4, base and
   delta named). A -B dying returns to the operator; no -C exists under
   any framing.

## 2. Process — the review loop at feature tier

As gwz-log's plan §2, with these deltas and one restoration:

- **Build/Review/Landing mechanics:** unchanged from the sibling —
  Opus builders in isolated worktrees (hygiene ritual; insurance
  copies); single-axis peer-blind Opus reviews against the step's
  NAMED rows (§5 is the authority); reports VERBATIM to
  `gwz-dev/dev-docs/GwzWt-<Step>-Review.md`, committed with landings;
  two-round cap, round 2 final, then §1.6; pristine-overlay landings,
  direct exit codes, ritual 7; gwz-cli docs gate on command-doc moves;
  gwz-py suite + both-repo drift checks on schema moves.
- **Escalations — the parking rule, verbatim and restored (P0.1 F3):**
  auto-escalation to Fable on any P0/P1/P2 stands as policy; while the
  Fable pool is exhausted, an escalation PARKS the step until the quota
  reset rather than waiving the second axis; parked escalations are
  recorded in the step's review file; **no step lands with an unserved
  escalation.** The verified-fact carve-out (lane owner re-executes the
  evidence, recorded) is standing from the gwz-log era.
  **The proportionality carve-out is PROPOSED AND PENDING, not
  adopted** — the round-1 draft of this section claimed operator
  adoption with no estate record (P0.1 F3; the only recorded state is
  `GwzLogPlan.md` §5 item 2's "operator's ruling requested").
  RATIFICATION REQUEST, placed where the operator will read it: one
  line adopts or rejects *"a P2 whose complete remedy is a lane-owner
  document edit, executed and recorded in the adoption trail, is
  served without a Fable pass"* — to be recorded in `GwzLogPlan.md` §5
  item 2 (where the rule was minted) and cited here. Until then, such
  P2s PARK per the sentence above.
- **Aggregate-gate pin duty (P0.1 F6):** every gwz-core landing that
  moves a lib partition count updates
  `run_r4bg_aggregate_gates.py`'s pins IN THE SAME COMMIT, darwin
  measured / linux derived-and-marked FIRST-DISPATCH-EXPECTED per the
  driver's own convention (a measured number wins), provenance in the
  dated docstring block — the builder's same-commit duty, checked by
  the step review. Note the inherited baseline: the sibling's linux
  remainder is itself still FIRST-DISPATCH-EXPECTED.
- **Real-workspace battery, pre-named (P0.1 F18):** S1.1, S1.4, and
  S2.3 trigger it by known diff surface; the rule ("any landing
  touching `workspace_ops/` or the boundary-sync/merge-evidence
  surface") stands for surprises.
- **Traceability:** every implemented v0 row, MUST or SHOULD, lands
  with a named test; S4.2 sweeps §5.

## 3. Phases

### Phase 0 — adoption

- **P0.1 — plan review.** EXECUTED 2026-09-01: GO-WITH-CONDITIONS, all
  findings folded (§6). This plan is adopted; builds may launch.

### Phase 1 — foundations (milestone: compat and plumbing landed) — all ∥

- **S1.1 — gitfile-safe resolution (W-CMP-1)** *(gwz-core; HARD cap
  **350** — raised from 250 on F7's measured fan-out; the §1.3 risk
  concentrate with its full proof set incl. the composite-cure
  fixture)*. Resolution through the repository handle; the write path
  already holds a backend, the six merge-evidence READ sites gain
  fallible resolution — the signature propagation is the step's one
  semantic decision, stated: readers resolve via the repository they
  already observe, refusals typed. Grep audit + allowlist as a checked
  assertion.
- **S1.2 — the AD1 helper** *(gwz-core; cap 150)*. Env sanitation,
  availability + version probe (≥ 2.30), typed refusal. Existing AD1
  sites not migrated.
- **S1.3 — protocol surface (W-CMD-7 wire half)** *(cap 250)*.
  Additive messages + both-repo regen + untouched-slots assertion.
  **Stub ownership map (P0.1 F17):** S1.3 lands refusal stubs only;
  each Phase-2 step fills ITS verb's dispatch — add → S2.2a, list →
  S2.4, remove/prune/repair → S2.5 — named here so no stub is orphaned.
- **S1.4 — intent records + nesting helper** *(cap 250; real-workspace
  battery at landing — its clone adoption touches `workspace_ops/`)*.
  The per-composite artifact (path, members, root_base, created_at);
  the shared parent-walking helper adopted by `gwz clone` (shipped-
  behavior change, flagged to its review by name).

### Phase 2 — the core verbs (milestone: composites create, list, retire in core)

- **S2.1 — aggregating preflight + plan (owns W-CMP-3, W-CMD-2,
  W-LIF-1, W-SAF-1, W-ENV-1's plan-refusal half, W-ENV-2, W-ENV-3)**
  *(after S1.4; cap 400)*. Collect-every-refusal planning: nesting/
  emptiness with resume precedence; the branch tri-state — create base
  = lock pin/snapshot, root base = primary HEAD at plan time,
  none/all/some with refuse-on-mixed, both git conflict classes
  absorbed — **the aggregated refusal naming every conflicting repo,
  branch, and holder path** (refusal content restored per P0.1 F13);
  member-kind and zero-git-member refusals; ref-format authority;
  floor probe via S1.2.
- **S2.2a — the add executor (owns W-CMD-1, W-ENV-4's detached-default
  half)** *(after S2.1 + S1.1 + S1.2 + S1.3; HARD cap 300; split from
  the round-1 S2.2 per P0.1 F5)*. **Executor decision, made here: a
  bespoke executor in `operation/worktree/` — it does NOT reuse
  `apply_materialize_plans`** (which unconditionally boundary-syncs
  the root it creates and rolls back with `remove_dir_all`; P0.1 F1's
  verified hazard) — **and by construction never reaches
  `sync_workspace_boundary` for any constituent, root included** (the
  write-set proof cites this). Root-first; intent record written after
  plan acceptance, before root creation (design §5.5 order — P0.1
  F12's ambiguity resolved to the design); CLI-delegated creation with
  git2 self-verification, **each verification opening its own
  per-thread `Repository` — no `Worktree` handle crosses threads**
  (the Send/Sync confinement, stated); per-repo tolerant results
  under the jobs bound; detached-at-pins default with recorded
  root_base. No dependency on S2.3 — independence achieved by the
  bespoke-executor decision, not by ordering.
- **S2.2b — resume + crash windows (owns W-LIF-2, W-LIF-3)** *(after
  S2.2a; cap 250)*. Idempotent re-add (present-and-correct → skip);
  partials stay legal, no rollback; the five crash-window fixtures
  (before intent; after intent, no worktrees; root-only; mid-member;
  complete).
- **S2.3 — the boundary rules inside composites (owns W-CMP-2,
  W-CMP-4)** *(after S1.1; cap 200; real-workspace battery at
  landing)*. Linked-root detection; boundary-sync verbs skip the
  exclude write with a structured note; the divergent-member-set
  residual demonstrated by fixture; **and the W-CMP-4 structural-merge
  refusal** (typed, teaching "run merges in the primary") — the
  evidence-stability hazard P0.1 F4 exposed, closed by refusal rather
  than left as a silent window.
- **S2.4 — list + health (owns W-CMD-3, W-ENV-6's list half, W-OBS-1's
  core half)** *(after S1.4; cap 300)*. Enumeration from `git worktree
  list` on the root repo; health incl. locked-with-reason and ref;
  managed/unmanaged/incomplete; lost-record degradation to unmanaged.
- **S2.5 — remove + prune + repair (owns W-CMD-4, W-CMD-5, W-CMD-8,
  and W-ENV-6's prune half — the orphaned-intent cleanup, cited here
  per P0.1 F14)** *(after S1.4 + S1.2; cap 400)*. Dirty refusal;
  locked refusal even under `--force`; per-repo tolerant, idempotent;
  prune flags pinned never-unlock/never-touch; repair via CLI.

∥: S2.3, S2.4, S2.5 mutually independent after their deps; S2.1 →
S2.2a → S2.2b is the long pole.

### Phase 3 — the clients (milestone: three channels ship)

*(Split per P0.1 F11 — the round-1 S3.1 had re-grown the client-bundle
class; and the ordering rationale is stated: py mirrors AFTER the cli
flag surface stabilizes, the sibling's own reasoning.)*

- **S3.1a — cli surface + exit mapping (owns W-CMD-7's cli half)**
  *(after Phase 2; cap 250)*. The `gwz worktree` subcommand tree, ALL
  flags in one place, lowering, exit mapping via the existing seam.
- **S3.1b — cli rendering + docs (owns W-OBS-1's render half,
  W-OBS-2's cli half, W-ENV-1's cli-lossy half — the non-UTF-8
  render/flag behavior, owned per P0.1 F9)** *(after S3.1a; cap
  250)*. List table, per-repo result tables, `gwz.worktree/v0` machine
  output; command docs + docs gate.
- **S3.2 — gwz-py mirror (owns W-CMD-7's py half, W-OBS-2's parity
  half, W-ENV-1's py-lossy-parity half)** *(after S3.1a + S3.1b +
  S1.3; cap 400)*. The `cli_*` mirror, client API, `cli_render`
  pattern, byte-parity assertions against captured cli output incl.
  the lossy edge, flag tri-states.

### Phase 4 — proof and settle (milestone: parity proven; GwzWt accepted)

- **S4.1 — the parity + lifecycle battery (owns W-CMD-6, W-ENV-4's
  cross-filesystem half, W-ENV-5, W-SAF-2, W-SAF-3, W-SAF-4)** *(tests
  only; after Phase 3; cap 500)*. Real-workspace fixtures via actual
  `gwz init`/`gwz commit`; **the verb list derived from W-CMP-1's
  blast-radius enumeration (P0.1 F4): status / diff / log / commit /
  branch / stash / capture / snapshot / push+pull (local bare remote)
  / stage / repo / materialize** — each asserted against primary
  behavior; **the W-CMP-4 merge refusal asserted** (structural merge
  in a composite refuses teachingly; no merge-evidence window exists
  to test because the design closed it); the write-set proof (shared
  `info/exclude` + merge evidence byte-untouched across every
  composite operation); catalog-lease sharing demonstrated beside the
  cited per-worktree mutator-guard test; `+snapshot`/`+lock` against
  the composite's own conf; a cross-filesystem composite fixture;
  unmanaged m0-shape, locked-constituent, and resume-window CLI-level
  fixtures.
- **S4.2 — matrix + settle (owns W-ENV-4's Windows half)** *(cap 150 —
  workflow/docs handwritten; P0.1 F10's "uncapped, row-less" cured)*.
  Windows/platform legs (the named first-dispatch obligation, a real
  gate); the traceability sweep over §5 (MUST + implemented SHOULD);
  single-axis settle review; release to the operator.

## 4. Dependency sketch

```text
P0.1 ──┬── S1.1 ──┬────────────────── S2.3 ──┐
       ├── S1.2 ──┼── (S2.2a deps)           │
       ├── S1.3 ──┤                          │
       └── S1.4 ──┬── S2.1 ── S2.2a ── S2.2b ┼── S3.1a ── S3.1b ── S3.2 ── S4.1 ── S4.2
                  ├── S2.4 ──────────────────┤
                  └── S2.5 ──────────────────┘
```

(S2.2a additionally consumes S1.1/S1.2/S1.3 and — by decision, not
ordering — has NO dependency on S2.3.)

## 5. Coverage matrix (the review authority; P0.1's four deltas folded)

| Row | Owner | Note |
| --- | --- | --- |
| W-CMD-1 | S2.2a | |
| W-CMD-2 | S2.1 | refusal content per the design, verbatim |
| W-CMD-3 | S2.4 | |
| W-CMD-4 | S2.5 | |
| W-CMD-5 | S2.5 | |
| W-CMD-6 | S4.1 | verb list from W-CMP-1's enumeration; one named exception (W-CMP-4) |
| W-CMD-7 | S1.3 (wire) + S3.1a (cli) + S3.2 (py) | three named halves |
| W-CMD-8 | S2.5 | |
| W-CMP-1 | S1.1 | |
| W-CMP-2 | S2.3 | |
| W-CMP-3 | S2.1 | |
| W-CMP-4 | S2.3 | the merge refusal (design row added at P0.1 F4) |
| W-LIF-1 | S2.1 | |
| W-LIF-2 | S2.2b | |
| W-LIF-3 | S2.2b | |
| W-SAF-1 | S2.1 | |
| W-SAF-2 | S4.1 | proof; S2.2a's never-reaches-boundary-sync construction cited |
| W-SAF-3 | S4.1 | |
| W-SAF-4 | S4.1 | |
| W-ENV-1 | S2.1 (plan refusals) + S3.1b (cli lossy) + S3.2 (py parity) | three named halves (P0.1 F9) |
| W-ENV-2 | S2.1 | |
| W-ENV-3 | S2.1 | |
| W-ENV-4 | S2.2a (detached default) + S4.1 (cross-FS) + S4.2 (Windows) | three named halves (P0.1 F10) |
| W-ENV-5 | S4.1 | |
| W-ENV-6 | S2.4 (list half) + S2.5 (prune half) | both halves brief-cited (P0.1 F14) |
| W-OBS-1 | S2.4 (core) + S3.1b (render) | |
| W-OBS-2 | S3.1b (cli) + S3.2 (parity) | |

S1.2/S1.4 own mechanism, not rows; their consumers' rows cite them.

## 6. Adoption trail

- **P0.1 (2026-09-01): GO-WITH-CONDITIONS** — 0 P0 / 4 P1 / 7 P2 /
  7 P3, all folded same day. [F1/F5] S2.2 split into S2.2a/S2.2b with
  the bespoke-executor decision made in the brief (never reaches
  boundary-sync — the seam's unconditional root sync verified at
  `handle_materialize.rs:392`) and the Send/Sync confinement stated.
  [F2] the design's stale §5.5 exclude instruction removed (design
  edit, dated). [F3] the proportionality carve-out RESTATED as
  proposed-and-pending with the ratification request placed in §2, and
  the parking rule reinstated verbatim — the round-1 claim of operator
  adoption had no estate record and is withdrawn. [F4] S4.1's verb
  list derived from W-CMP-1's enumeration (+stage/repo/materialize),
  and the merge evidence-stability hazard closed by the NEW design row
  W-CMP-4 (structural merge refuses in composites), owned by S2.3,
  asserted by S4.1. [F6] the aggregate-gate pin duty added to §2.
  [F7] the frozen-surface position stated (§1.4) and S1.1's cap raised
  to 350 on the measured fan-out. [F8] the composite-cure fixture
  added to S1.1's proof set. [F9/F10] W-ENV-1 and W-ENV-4 split into
  named halves; S4.2 gained a cap and its row. [F11] Phase 3 split
  with the ordering rationale stated. [F12] intent-record timing
  pinned to the design's order. [F13] W-CMD-2's refusal content
  restored. [F14] W-ENV-6's prune half brief-cited. [F15] §7's size
  claim corrected to the measured ≈48% of ~8,900. [F16] the -B
  terminus stated. [F17] the stub-ownership map added to S1.3.
  [F18] battery-triggering steps pre-named. **The plan is ADOPTED**;
  builds launch on the operator's word, with S1.1 ∥ S1.2 ∥ S1.3 ∥
  S1.4 the entry set.

## 7. Budget summary

Handwritten-LOC targets: Phase 1 ≈ 1,000 (S1.1 350 HARD, S1.2 150,
S1.3 250, S1.4 250); Phase 2 ≈ 1,850 (400, 300 HARD, 250, 200, 300,
400); Phase 3 ≈ 900 (250, 250, 400); Phase 4 ≈ 650 (500, 150) —
**≈ 4,400 total incl. tests, ≈ 48% of gwz log's ~8,900 landed
handwritten LOC** (measured, per P0.1 F15 — the round-1 "quarter" claim
was off by ~2×). No step carries two semantic decisions; the two HARD
caps guard the two steps history says are the dangerous shapes.
