# GwzM5-8 R2-E Plan — checked-artifact consumer conversion and the re-reserved semantics

Date: 2026-08-26. Author: the implementation lane. Status: PROPOSED.
First post-v0.11.0 lane per the release checkpoint's resume order.

## 0. Object, charter sources, and one census correction

**The object:** R2-E converts the production consumers onto the
checked-artifact boundary and installs the fault semantics the R2-D
freeze re-reserved to this lane. At its close: the §10 consumer
conversion table is executed, `recover_or_create` has its first
production caller (the first production catalog activation), legacy
writers no longer mutate inside `.gwz/checked-artifacts`, and the
cleanup/barrier/terminal fault families are injected and matrix-green.

**Charter sources, consumed directly:** `GwzM5-8R2D-Plan.md` §5
items 1-2 (the defer-outs this lane discharges);
`GwzM5-8R4bR2ConsumerCheckpoint.md` §10 (the conversion table) and
§11 (packages/ownership); `GwzM5-8R2DInterfaceFreeze.md` §3.5's dated
re-reservation records (cleanup :727-728, barrier :787, terminal
:794/:880) and §4.3; `GwzM5-8A1Activation-ReviewCompleteness.md`
§1.3 bucket (c) (the routed R2-E stream);
`GwzM5-8A1ReleaseR2-ReviewState.md` [P3-R2-1]/[P3-R2-2];
`GwzM5-8A1ReleaseR1-ReviewState.md` §(b) (the recommended guard
tie); the checkpoint's L6/L14 carrier rulings (the archive/GC
sub-package); `GwzMergeCheckpoint-v0.11.0.md` (the shipped
residuals this lane owns).

**Census correction, on the record:** the program checkpoint and the
lane memory have said "67 re-reserved keys." The freeze's own
records and the Completeness review's bucket (c) say **38**: cleanup
11 + barrier 16 + terminal 11. 38 is correct; the 67 was a
lane-introduced error, corrected in the checkpoint with this plan's
commit. The full census stays 165 (`EXPECTED_KEY_COUNT`) — the 38
exist unbound; R2-E binds them without moving the census.

## 1. The consolidated obligation ledger

| # | Obligation | Source | Phase |
| --- | --- | --- | --- |
| O1 | The §10 consumer conversion table, executed: WorkspaceMutatorLock, first merge record, merge record rewrite, terminal archive source-only, stash bundles, markers, workspace checked mutation, `.git/info/exclude` | ConsumerCheckpoint §10; R2-D plan §5 item 1 | E4 |
| O2 | First production catalog activation — `recover_or_create` gains its production caller (gated by settled tuple §11.3) | R2-D plan §5 item 2; bucket (c) | E4 |
| O3 | Legacy writers stop mutating inside `.gwz/checked-artifacts` | thin-A1 §2's accepted residual, expiring here | E4 |
| O4 | cleanup.* semantics + injection, 11 keys (all need `AdmittedActionV1` — the duty attaches now that admission is production) | freeze :727-728; checkpoint Phase-5 record | E1 |
| O5 | barrier.* semantics + injection, 16 keys | freeze :787 | E2 |
| O6 | **BINDING**: `BarrierIntentV1::issue` observe-or-refuse — the last shape of the caller-supplied-restatement class, production-unreachable at the settle | settled dual Code round 1; bucket (c) | E0 decide, E2 implement |
| O7 | terminal.* semantics + injection, 11 keys (terminal retirement edges) | freeze :794/:880 | E3 |
| O8 | The archive/GC consumer sub-package: the 18-UNBOUND per-scenario registry rows + cheap closures (i)+(ii) landing TOGETHER (registry rows + one parametric `adapt_open` refusal test over every non-whitelisted scenario); the archive-equivalence mechanism decision; the two archive shapes riding A1 [P2-1] option (i); the `gc_archived` production route | Evidence §12.7; L6 ruling; A1 record §6 | E0 decide, E5 |
| O9 | [P3-R2-2] the eligible-row upgrade-failure fallback: one executed composed-path test (whitelisted Finalizing row, atomic upgrade `Err(_)` → v0 completes; needs a filesystem-level fault — production hardcodes `AtomicUpgradeFault::None`) — also the moved-pin coverage restoration (both axes converged on this arm) | R2.3 report; A1 record §14 | E6 |
| O10 | [P3-R2-1] `AtomicUpgradeFault` hygiene: cfg-split signature or sealed constructor (injected variants production-compiled but inert) | R2.3 report | E6 |
| O11 | The `DurableObjectIdentity` production-reach question: does the v0.11.0 `--no-ff` v1 store hit the identity probe on Linux? (decides whether capability-refusal UX and docs work exist) + the capability-refusal UX if reach is real | release verify diagnosis, memory note | E0 answer, E6 if real |
| O12 | Recommended riders: the abort rewrite-set bound guard tied (g12:737-739, R1.2 (b)); the `authority_name` self-checking-name weigh; the anchor nits; the 481-item frozen-surface reconciliation at the settle | R1.2; bucket (c) | E6/E7 |

Explicitly NOT R2-E (owners on record): C-2's four fixtures, T-5
regeneration, the multi-repo CI checkout, legacy-path removal,
MAX_PATH, native power-loss evidence (all R2-F); the ordinary-start
v1 owner + floor raise (M5c); the escape implementation packages
(second lane, operator handoff); renormalize (R6); the per-predicate
rejection names (F5 §9 item 6 — unowned candidate, not adopted
here).

## 2. Review tiering (the adopted process, applied)

Two duals maximum: **dual #1 at the E0 semantics amendment** (it
extends the frozen §3.5/§4.3 surface — the same class as R2-D's
Phase 0), **dual #2 at the E7 settled tree**. Every interior step:
single-axis peer-blind with automatic escalation on P0/P1/P2;
two-round remediation cap; reports filed verbatim to dev-docs as
`GwzM5-8R2E<Step>-Review*.md`. Schedule in gates and review rounds,
not LOC (the R2-D plan's own lesson) — nominal: 1 amendment round +
8-10 interior reviews + 1 settled round. All landings by the
standing rituals (pristine overlay, executed pins, untruncated
sweeps with zero-match residual greps, per-OS counts where measured).

## 3. Phases

### Phase E0 — the object frozen (milestone: R2-E is defined, decided, and dual-reviewed)

- **E0.1 — the reach traces** (read-only, ~0 LOC): (a) O11 — a
  bounded trace from `handle_start_durable_v1`/the checked v1 store
  into `checked_artifact::identity` on Linux, answering whether any
  v0.11.0 production path executes the `name_to_handle_at` probe;
  (b) the O2 gate check — restate what tuple §11.3 requires before
  production catalog activation. Both answers land as a dated note
  the E0.2 amendment cites.
- **E0.2 — the semantics amendment** (docs, one train): dated §3.5
  activation records installing injection semantics for all 38 keys
  (cleanup's AdmittedActionV1-bound duties; barrier incl. the O6
  observe-or-refuse RESOLUTION — decide observe vs refuse per shape,
  recorded; terminal retirement edges); the O8 archive-equivalence
  mechanism DECISION; the §4.3 E-table annotations these imply.
  Quote-preserving, per the sanctioned annotation mechanism.
- **E0.3 — dual #1**, peer-blind Code+State on the amendment.
  GO/GO unblocks E1-E3 in parallel.

### Phase E1 — cleanup.* installed (11 keys; milestone: family matrix-green)

- **E1.1** — injection sites in the owner-private mutation files per
  the Phase-1 pattern (driver holds zero), semantics per E0.2
  (<500 LOC).
- **E1.2** — the cleanup fault matrix: every key
  interrupted+restarted+converged; census statements; checker
  count/pin companions in the SAME commit (ritual 5 — including the
  R4b-G driver's per-OS fault markers, which move with any lib-test
  addition).
- **E1.3** — single-axis review; land.

### Phase E2 — barrier.* installed (16 keys + O6; milestone: family matrix-green, the restatement class extinct)

- **E2.1** — barrier injection semantics (<500 LOC).
- **E2.2** — `BarrierIntentV1::issue` per E0's resolution: the seam
  stops accepting a caller-supplied restatement (observe) or refuses
  typed (refuse); the last named-class shape closes (<300 LOC).
- **E2.3** — the barrier fault matrix + companions; review; land.

### Phase E3 — terminal.* installed (11 keys; milestone: family matrix-green)

- **E3.1** — terminal retirement-edge semantics (<500 LOC).
- **E3.2** — matrix + companions; review; land.

E1/E2/E3 are parallel-friendly after E0 (distinct families, distinct
owner files) with the shared-driver duty split observed; if run in
parallel, isolated worktrees per lane and the lane owner lands
sequentially.

### Phase E4 — the consumer conversion (milestone: production writes go through the boundary)

One step per §10 table row, each a reviewed <500-LOC package
converting that consumer onto the checked boundary with its frozen
ordering; the rows are parallel-friendly except where the table's
ordering couples them:

- **E4.1** — `WorkspaceMutatorLock` runtime bootstrap (the O2
  activation rides here: `recover_or_create`'s first production
  caller, gated by the E0.1(b) §11.3 restatement).
- **E4.2** — first merge record (`MergeStore` + `PreservationBundles`
  bootstrap ordering).
- **E4.3** — merge record rewrite (exact store; unknown fields and
  exact reread preserved).
- **E4.4** — terminal archive source-only (+ the O8 `gc_archived`
  production route lands with the archive consumer).
- **E4.5** — stash bundles + markers (two rows, one package if they
  share the parent-durability seam).
- **E4.6** — workspace checked mutation + `.git/info/exclude`.
- **E4.7** — O3 close-out: the legacy in-place writers inside
  `.gwz/checked-artifacts` retire behind the converted paths; the
  A1-era allowances (`catalog.rs:10-16` class) expire with dated
  annotations.

Each step: single-axis review, matrix at the landing, T-6-class
tripwires re-verified.

### Phase E5 — the archive/GC evidence sub-package (milestone: the unbound scenario space machine-recorded)

- **E5.1** — the 18 registry rows + the parametric `adapt_open`
  refusal test, ONE package (the L6 ruling: they land together);
  the two archive shapes' dispositions; scenario-map and
  compatibility-checker companions in the same commit.
- **E5.2** — archive-equivalence per E0's decision; review; land.

Parallel-friendly with E4 (registry/test surface vs production
conversion surface).

### Phase E6 — donated hardening riders (milestone: the review-debt ledger empty)

- **E6.1** — O9: the composed-path upgrade-failure test (fs-level
  fault injection; reuses E1-E3's injection machinery — schedule
  after any one of them lands).
- **E6.2** — O10 hygiene + the abort-bound guard tie + the anchor
  nits (one small package).
- **E6.3** — O11's consequence IF reach is real: the
  capability-refusal UX (typed message naming the filesystem
  capability) + runbook/notes lines; if reach is not real, a dated
  no-work record closes O11.

### Phase E7 — the settled gate (milestone: R2-E accepted)

- **E7.1** — full-tree settled review, dual #2 (peer-blind
  Code+State), carrying the E1-E6 escalation second-axis scrutiny
  per the deferred-escalation design.
- **E7.2** — three-platform matrix acceptance at the settled tree
  (win/plat dispatches; per-OS battery pins re-measured where
  counts moved); the 481-item frozen-surface reconciliation; the
  acceptance record with census statements and the ledger's
  row-by-row close (O1-O12 each DISCHARGED or re-owned with a
  named carrier).

## 4. Non-goals

Everything in the "Explicitly NOT R2-E" list of §1; any change to
`ACTIVE_WRITER_FLOOR`; any new wire (protocol/generated.rs stays
zero-diff); any release cut (v0.11.1+ is its own train on this
plan's completion or the operator's call).

## 5. Open operator decisions (one line each, none blocking E0.1)

1. **Parallelism width** — run E1/E2/E3 as three parallel lanes
   (faster, three worktrees + the shared-driver ritual load) or
   sequentially (calmer). Lane recommendation: parallel, the
   R2-D interior proved the discipline.
2. **The executable-template policy** (recorded post-A1 operator
   question): if decided, its typed refusal lands as an E4 rider;
   if not, it stays an open operator item.
3. **Scheduling vs quota** — E0 is cheap (one read-only trace + one
   docs train + one dual); the E1-E7 implementation spend is
   R2-D-interior scale. If this week's pool is tight, E0 now and
   the implementation lanes after the Friday reset.

## 6. First action on approval

E0.1's reach traces (read-only) start immediately on "go"; E0.2
drafts against their answers; dual #1 follows. No tree mutation
before the E0.3 GO/GO.
