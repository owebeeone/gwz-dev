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

- **BURN-DOWN WINDOW CLOSED (operator, 2026-08-22, ~10% Fable
  remaining: "prolong by reducing Fable usage").** Interior reviews
  revert to Opus single-axis effective immediately (Steps 2.3/2.4
  review on Opus); the only remaining Fable spends this week are the
  in-flight ARM diagnosis and D3's two focused re-verdicts; Phase 5
  dual, R4b-G, and the A1 activation review hold for the quota
  reset; lane-owner operation minimized (delegation-first, batched
  bookkeeping). The window note below is historical.
- **QUOTA BURN-DOWN WINDOW (operator, 2026-08-22, ~17h until the
  weekly reset): "use the remaining quota".** For this window only:
  interior single-axis reviews (Phase 2 steps) run on the FABLE tier
  instead of Opus; the deferred-by-economy ARM64 EBADF substrate
  package (Fable-tier deep diagnosis) is launched; Phase 2 Steps
  2.1 and 2.2 launched in parallel (Opus implementation, per the
  plan's own §4.4 pipelining — legal behind the Phase 1 settle
  in-review). At quota reset the tier policy below resumes
  unchanged. Concurrent lanes at window open: Phase 1 settle dual
  (Fable ×2), D3 cursor (Opus), Steps 2.1 + 2.2 (Opus ×2), ARM64
  EBADF (Fable). Fixture-collision rule in force: 2.1 owns the
  `durable_leaf.*` region and 2.2 the `namespace.*` region of
  fault_expected_keys.rs; per-lane scratch CARGO_TARGET_DIRs
  mandatory; commits serialize through the lane owner.
- **Refined tier policy (supersedes the 2026-08-16 economy policy's
  reviewer line; suspended only as stated in the window note
  above):** Fable ONLY at the program-level dual gates —
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
  per tree precedent.

  **M5b ACCEPTED 2026-08-22 at GO/GO — round-1 clean on both axes**
  (the program's first): Code GO (0 P0/P1, 1 P2, 3 P3; zero-production
  proven via diff surface + cfg chain + non-test dep-info; all 21
  suites mapped to design obligations; the stopped abandonment item
  independently re-probed — modes fail identically, routing ruled
  correct) and State GO (0 P0/P1, 1 P2, 5 P3; determinism, reverse,
  composition, mode isolation verified; ceiling raise ratified).
  Acceptance train dispositions: both P2s closed — the T-3 second
  scan (`no_ff_mode_mentions_stay_inside_the_pinned_surface`, 16-file
  NoFf surface pin with the pre-A1 subsumption argument) and the
  push-lane CI step running the tripwire scans in the boundary
  workflow (also discharges Code P3 on T-4's CI scan); M5b-W1 applied
  to `GwzM5-8I2RecordContract.md` §7 (dated banner, document-only);
  the §9-Q7 class-membership sentence recorded in the classification
  ledger; the ChangeBudget M5b row filed (test-ceiling raise 1,450
  granted+ratified). Clean-tree re-cut: satisfied by both reviewers'
  pristine-extraction gate runs at `3e60529` (Code: full lib
  1,391/0/1, boundary checker ok; State: 532/0 merge partition,
  21/21 suites, T-6 by name). T-5 remains the retained-reader lane's
  per the design's own assignment. Remaining State/Code P3s
  (FfOnly durable assertion, inert tree assertion, F-2 cross-mode
  bytes, Q5 partiality note) are file-and-continue, listed in the
  reports. **M5b's A1 proof obligations (thin A1 leg two) are now
  standing: T-6 + the accepted package.** Pushed with this train.

  **R2-D Phase 1 Steps 1.1+1.2 PARKED pre-settle** (2026-08-22): the
  physical admission driver landed green — all six Step-1.1
  acceptance tests pass UNEDITED on first run (byte-untouched,
  verified); `checked_artifact::` 267/0 (257 pre-existing + 6
  acceptance + 4 new unit); Track-P spike still green; no wire
  change (`protocol/generated.rs` untouched; no new slot, record,
  purpose, or phase); C-2 arms and C-3 observer grammar implemented
  exactly per the frozen §4.4 classes with the memo-clause map in
  the driver report. Production LOC ~768 code (+1051 raw) vs the
  aspirational <500 — lane-owner disposition: accepted for the
  settle review, structural cause being the round-2 freeze assigning
  BOTH extension classes to Phase 1 after the plan's budget was set;
  no ChangeBudget ceiling exists for R2-D and no §16-22 stop rule
  fires. THREE judgment calls explicitly queued for the Phase-1 dual
  settle (dual #2): (i) visibility widening of the R1 classifier
  types to `pub(in crate::checked_artifact)` (seams unchanged, pin
  test green); (ii) E4 install as retire-then-publish over the
  no-replace primitive with all three crash windows proven
  convergent — re-spike if the reviewer reads E4 as requiring an
  atomic replacing rename; (iii) absent-active ≡ idle semantics.
  **Steps 2.1 + 2.2 LANDED 2026-08-22 (one combined train).** 2.1
  (LeafObserver production impl, durable_leaf 11/11 executed): Code
  single-axis round-1 NO-GO on one real P1 — sync_all on the
  read-only handle breaks E9 on Windows — fixed with a documented
  platform-routed arm + non-vacuous os-error-5 canary; round-2 GO;
  escalation State axis GO (0 P0/P1; its P2 discharged by the freeze
  §4.3 E9 annotation landed with this train: writer-class-conditional
  durability, per-platform ExactDurable meaning, MissingDurable
  negative space — binding on Step 2.4's caller). 2.2 (namespace
  backend, namespace 11/11 executed): State single-axis GO; its
  auto-escalation trigger was a P2 that IS the landing commit's own
  checker counts entry — lane-owner adjudication: discharged at
  landing, second-axis scrutiny folds into the Phase 5 settled dual
  per the three-dual cap; repeatability-taxonomy comment corrected
  at landing per its P3-1. Both tree pins + the counts entry
  validated GREEN on a pristine overlay before commit. Phase 2
  remaining: Steps 2.3 + 2.4 (launched on landing).

  **Steps 2.3 + 2.4 LANDED 2026-08-22 (one combined train, gwz-core
  `c2564ba`).** 2.3 (managed backend ops, E15/E16 real): State
  single-axis round-1 NO-GO (1 P1 — the checker counts entry, a
  verbatim repeat of 2.2's finding; 3 P2, 6 P3); round-2 GO with all
  ten closed — §3.5 managed_bootstrap activation filed (8/30
  PartiallyExecuted, 22 proved siteless), 4×2 repeat taxonomy with a
  partition assertion, and the E16 cross-parent atomicity record
  (Branch A: wedge proved unreachable — the rename is the commit
  point, the three post-crash states all carry green matrix rows,
  EXDEV and foreign removal refuse typed; the reviewer ruled Branch A
  correct on the merits and a recovery arm UNSOUND, since the two
  causes are indistinguishable from durable evidence). 2.4 (authority
  parse/proof split): Code single-axis round-1 NO-GO (proven P0 —
  `cfg(windows)` const fn E0015, Windows did not compile; P1 —
  proof/parent provenance unbound; P2 — taxonomy 4-of-9); round-2 GO
  — the P0 closed structurally (one always-compiled body over
  `cfg!(windows)`, zero platform predicates left; the reviewer
  reproduced the forced-flip proof and endorsed it as stronger than
  its own cfg-swap), the streamed proof now carries its own
  provenance (the transaction lost its parent parameter; typed
  fail-closed join guard precedes the boundary announcement), and the
  taxonomy is a machine-checked 9/4 partition. Both escalation
  triggers RECORDED (2.3: 1 P1 + 3 P2; 2.4: P0 + P1 + P2);
  lane-owner routing for both: second-axis scrutiny folds into the
  Phase 5 settled dual (dual #3) per the three-dual cap.
  `capability_permit` callers 11→13 with the 2.4 file joined to the
  inventory at landing; the shared pre_catalog tree pin recomputed on
  the pristine overlay (checker green pre-push; the overlay pin
  matched the 2.3 lane's independently reported union value). The ARM
  exact_source fixture fix (distinct inode by construction) rode this
  train. Landing-brief duty conflict RESOLVED going forward (the
  mechanism behind this P1 and 2.2's identical finding, surfaced by
  the 2.3 implementer): checker counts/allowlist companions are the
  CONVERTING PACKAGE's same-commit duty; tree digest pins remain the
  lane owner's land-time duty, computed from the pristine overlay.

  **D3 LANDED 2026-08-22 (gwz-core `8b83a2c`) — dual settled GO/GO at
  round 2.** Round 1: Code GO (2 P2, 3 P3) / State NO-GO (1 P1 — four
  undelivered §8 legs; 2 P2, 3 P3). One merged remediation: all four
  §8 legs green in their designated homes (identical classification
  proven image-capture-free at the real seam against a paying
  degraded control; crash-window reproof and convergence;
  rollback-entry preflight made the structurally sole catcher by
  fixture geometry; byte-identical bundles; post-GC retention),
  CleanupMarkers v0/v1 fork (a marker-only row stays an empty row →
  ContradictoryEvidence on the v0 leg; §5 row content on v1),
  computed backfill at both reset edges, rename landed with the old
  identifier extinct. State P3-2 adjudicated IN THE IMPLEMENTER'S
  FAVOR: the reviewer withdrew its own decode-reject proposal as
  overreach (it would have bricked a reachable §4-protected shape)
  and accepted the write-edge marker-carry closure; the residual
  (fail-closed illegal-row refusal pre-publish) is recorded with a
  named follow-up to carry the stash pair. Code round 2 unconditional
  GO. Budget reconciled on the record: +2192/−103 vs §9's ~700–1,100
  — the excess is the 755-line §8 acceptance suite. Bonus erratum
  surfaced by the §8.6 seam: post-GC is projection-only (no durable
  rewrite) — strictly stronger terminal-plane immutability than
  documented. The three workspace_ops checker pins recomputed at
  landing. Spend-limit incident #2: the monthly limit killed the Code
  re-verdict mid-gates; resumed from transcript zero-loss after the
  operator reset and returned its GO. The conservation holds
  (Phase 5 dual, R4b-G, A1 activation) are LIFTED by the reset; the
  refined tier policy stands.

  **ARM diagnosis CLOSED (both clusters attributed; non-gating per
  thin A1).** Cluster (b) exact_source: ext4 recycles the freed
  inode; the fixture's assert_ne precondition broke while the
  production refusal held on every failing run; fixed fixture-side
  (rode the 2.3/2.4 train). Cluster (a) 29 g15 + 17 v1_lifecycle:
  Linux-RUNNER-only, never green on any Linux, NOT ARM-specific — a
  faithful arm64 parity rig (ubuntu:24.04, loop-ext4, PPA git 2.55,
  runner config, both uids) is fully green. Probe branch
  `probe/g15-gate-dump` (run 32561108291) named the cause: gate 4
  `files::observe_boundary` false 78/78 because `.git/info/exclude`
  is mode 0755 — the RUNNER IMAGE's git template tree carries the
  executable bit, `git init` copies it, the fixture's `fs::write`
  preserves the mode, and the leaf observer classifies executables
  Invalid BY DESIGN (observation.rs:216). Single-variable local
  proof: chmod 0755 on that file alone reproduces the verbatim CI
  message. libgit2-built fixtures (no template copy) pass —
  consistent with the v1 split. Remediation is fixture-side
  (pin/normalize the boundary-file mode), queued on the clean tree.
  OPERATOR POLICY QUESTION recorded for post-A1: a real repository
  initialized from an executable template tree (Ubuntu images ship
  exactly this) receives a typed root-preservation refusal — doc
  note vs a mode-tolerant arm for this one boundary file. Memo:
  `GwzArmPreservationHandoffDiagnosis.md`.

  **Windows anchor package LANDED 2026-08-22 (gwz-core `6b8b76e`) —
  review GO round 1, escalation trigger RECORDED (1 P1 + 4 P2),
  routed to the Phase 5 dual.** Root cause of the run-16 class: P5's
  Windows arm demanded a resident durability anchor only the
  checked-artifact private area may retain; E14 runs the barrier on
  the never-anchored retained action directory. Fix:
  `platform::DirentBarrierClass` — the caller states its writer
  class; seven legacy private-area sites keep the round trip;
  E10/E14's ExactInterior arm is a documented no-op in E9's
  writer-class-conditional sense (anchoring the action dir refuted
  structurally: admission exactness + exact-capture re-entry; the P2
  re-route refuted as a family move buying zero physical
  difference). Probe-verified GREEN on windows-2022 pre-landing
  (probe/namespace-anchor-win, run 32569434565, 22/0). Round-2
  remediation closed the review's record findings: the falsified E9
  residual-ordering claim corrected at five carrier sites including
  the shipped foreign-leaf refusal string; the §4.3 E10/E14
  activation annotation landed in contract form (win cells + §4.1 P5
  two-arm cell edited with it; the DurableNamespace-witness and
  parent_barrier-row negative space recorded directly; E9's clause
  superseded with its frozen text preserved verbatim). Both pins
  (platform.rs flat, pre_catalog tree) recomputed on the overlay and
  matched the lane's independently reported values. TRACKED: run 18
  must return the legacy AnchoredPrivateArea suites green (the
  trimmed probe exercised none of them) — the ledger carries the
  acceptance item.

  **Step 3.1 LANDED 2026-08-22 (gwz-core `e72e376`) — review GO
  round 1 (conditional), escalation trigger RECORDED (1 P1 + 1 P2),
  routed to the Phase 5 dual; both escalating findings were
  lane-owner dispositions, no remediation round.** The
  managed-parent bootstrap consumer: all three trait methods real
  (bounded no-follow preflight planning only the missing suffix —
  the Step-2.3 populated-components caution discharged;
  depth-tolerant revalidation; per-row execute_bound through
  stage/observe/retire/reprove). Zero publication sites added,
  permit 13 byte-untouched, census 165 with zero flips. Review
  adjudications: deterministic ownership token CORRECT AS BUILT
  (forced by the R2 stop clause; guarantee is self-consistency, NOT
  exclusion — re-litigate if E17 read-back or an R2-E consumer ever
  adopts a directory this action did not create); the
  retain_managed_parent re-export removal permitted and load-bearing
  for 3.3; refusal fail-closed structurally. LANE-OWNER DISPOSITIONS
  on the escalations: [P1-1] the E17 remainder (durable successor +
  prior-generation retirement; the partial-retirement restart window
  wedges ≥2-component rows) had no budgeted home in the plan — Step
  3.1b is MINTED for it, before the Phase 3 settle, same
  implementer; [P2-1] the five edges-converted-without-sites are now
  durably recorded as the §3.5 deferral record (this landing's
  freeze edit) with 3.2's exact debt enumerated (sites, rows,
  PartiallyExecuted 8→13). 3.1's P3s ride to 3.1b/3.2: the
  retain_managed_prefix comment corrections, the vanished-component
  settled-row pin, and 3.2's Git-directory arm blocker (test door or
  follow-up 3). Budget 980 code vs <500 aspirational — reviewed as
  load-bearing, recorded for the settle.

  **Step 3.1b DELIVERED 2026-08-22, under State-lead single-axis
  review.** The E17 durable managed-intent lifecycle: initial
  scratch/publish/reobserve, per-generation successor,
  prior-generation retirement, final retirement; resume_intent walks
  the resident chain link-by-link (each retired record's intent_id is
  the next link's expected predecessor — verified, not adopted); the
  review's [P1-1] wedge case (≥2-component row interrupted inside
  marker retirement) now converges, driven by name. Token read-back
  from the resident record with the self-consistency-not-exclusion
  boundary stated. FIFTEEN keys activated same-commit with sites and
  matrix rows (executed 8→23, reserved 22→7, counts held 165, no
  mint) — the 3.1 [P2-1] lesson absorbed; E17's two §4.4 arms
  resolve to none by the E16 mechanism; publication seam unchanged
  (21 sites, permit 13). Budget 812 code (~1.6×). Liftable §3.5
  annotation drafted. Open after 3.1b: 3.2's five writer keys +
  preflight/plan_complete disposition (reviewer asked to rule),
  follow-up 3, 3.3 glue.

  **ARM fixture package LANDED 2026-08-22 (gwz-core `c2d2f15`,
  direct-with-record per the fixture-class precedent — red-green
  proof standard exceeded review).** Mode pinning at fixture
  creation: g15 write_pinned at all five write sites + a permanent
  no-executable-leaf regression pin; v1 pin_fixture_boundary_mode in
  the four fixture builders owning the 17 rows. Red-green under a
  faithful runner simulation (scratch HOME + executable
  init.templatedir): red = exactly 29 g15 + the 17 named v1 rows;
  green = full population under plain AND simulated HOME.
  **ATTRIBUTION CORRECTION #3 (L1-16), recorded in the memo:** the
  probe's "libgit2 does not copy templates" premise was FALSE
  (git2-rs sets EXTERNAL_TEMPLATE by default; the copy maps
  executable→0755); all 17 v1 rows are libgit2-built, and the true
  discriminator is REPUBLICATION — write_atomic republication lands
  a fresh 0644 inode, so the passing fixtures were accidentally
  immune. Validation dispatched at `c2d2f15`: platform 32573547362
  (the program's first shot at ARM green), Windows 32573548540 (run
  18 — the anchor package's tracked acceptance item: legacy
  AnchoredPrivateArea suites + the namespace four).

  **Incident record (lane owner, 2026-08-22): reconcile stashes
  swept a live lane's edits, twice.** My post-landing reconciles
  (`git stash push -- <list> && git reset --hard origin/main`)
  stashed a REMEMBERED file list; the ARM fixture lane had
  meanwhile extended its edit set (v1 fixture files), and reset
  --hard reverted those uncommitted edits — twice. The lane detected
  the interference, restored from its own insurance copies, and
  re-verified every number on the post-landing tree; nothing was
  lost. RITUAL (6), effective immediately: the stash keep-list for
  any reconcile is computed from live `git status` at that moment
  (full complement of every path not in the train being landed),
  never from a remembered lane inventory; and lanes are told at
  launch to keep insurance copies of uncommitted work in their
  scratch area (this one did, which is why the burn cost nothing).

  **Step 3.1b LANDED 2026-08-22 (gwz-core `fcec69e`) — review GO
  round 1 (conditional, State-lead), escalation trigger RECORDED,
  routed to the Phase 5 dual.** E17 real; the partial-retirement
  wedge closed (the reviewer confirmed the fixture reproduces the
  3.1 [P1-1] case exactly and the base would have refused it
  permanently); chain linkage verified adversarially (binds action,
  reservation AND owner via matches_reservation — no fabricated
  higher generation, no cross-action adoption); both §4.4 arms
  resolve to none by direct read. Fifteen keys activated
  same-commit: executed 8→23, reserved 22→7, counts 165 held.
  Freeze: the 3.1b §3.5 annotation lifted at this landing, with the
  review's [P2-1] supersession clause added to the 3.1 deferral
  record (the five writer keys now sit among 7 reserved; 3.2's list
  edit is 23→28). Production LOC alone (440) was inside the <500
  target — the overrun is entirely mandated matrix evidence.
  DOCKETED SETTLE-BLOCKING for the Phase 3 settle (review [P3-1]
  ruling): `preflight` and `plan_complete` need a disposition — a
  step that owns them or a recorded determination they never gain
  boundaries; "reserved indefinitely" ruled incoherent now their
  converting package has landed. Folded into 3.2's brief: the
  BootstrapIntentRowV1::Scratch selector bypass ([P3-2]) and two
  inverted comments ([P3-3]).

  **PLATFORM MILESTONE 2026-08-22: the ENTIRE matrix is green
  simultaneously at `c2d2f15` — the first time since the R2-D fault
  families began landing.** Windows run 18 (32573548540): 1472/0/1 —
  the anchor acceptance item DISCHARGED (all seven legacy
  AnchoredPrivateArea suites green under the classed barrier; the
  namespace four extinct at `6b8b76e` as the probe predicted).
  Platform sibling (32573547362): ubuntu-24.04-arm 1510/0/1 — THE
  FIRST GREEN ARM RUN IN THE PROGRAM'S HISTORY (the 46-row
  template-mode class extinct name-for-name per the red-green) —
  and macos-14 1521/0/1. Remaining platform debt: none open; the
  3.1b/3.2 suites debut on Windows in the next full run
  (pre-attributed expected-green in the ledger); the g15 probe
  branches are cleanup candidates (operator's call, standing).

  **Step 3.2 LANDED 2026-08-22 (gwz-core `7169d89`) — review GO
  round 1, CLEAN (no P0/P1/P2, escalation NOT triggered; second
  round-1 clean of the program after M5b).** The five writer keys
  activated: managed_bootstrap.* at 28/30 executed, counts 165 held,
  the docketed pair (preflight/plan_complete) the only reserve. The
  single-crossing probe mechanism (crash → re-arm → next drive must
  settle without firing) machine-checks the partition's other half,
  caught a real once-per-component misclassification, and runs
  retroactively on 3.1b's keys; row shape is now a declared matrix
  property. Purpose policy matrix: four purposes via production
  constructors, §9 overlap rejection both directions. Two-sites-
  one-key for staging_directory_flush adjudicated honest (the 3.1
  review's own table specified it). The 3.2 §3.5 annotation lifted
  at this landing WITH the review's [P3-4] route-(b) sentence. Five
  P3s ride: shape-sentence corrections and dead fixture helpers fold
  into Step 3.3; the is_err-only §9 assertion and the over-applied
  one-component shape are recorded for the settle. Budget: 357 net
  test lines vs the plan's <500 — WITHIN; production +38. Phase 3
  remaining: Step 3.3 coordinator glue, then the Phase 3 settle
  record (dispositions: preflight/plan_complete,
  retain_managed_parent).

  **PHASE 3 SETTLED 2026-08-23 (single-axis regime per freeze §9 —
  the plan's Phase-3/Phase-4 "Gate: dual" lines are superseded by
  thin-A1's three-dual cap; recorded deliberately).** Steps landed:
  3.1 `e72e376`, 3.1b `fcec69e`, 3.2 `7169d89`, 3.3 `3a45619`.
  Step 3.3's round-1 P1 — authorize_write bound a CALLER-CHOSEN
  reservation — closed round 2 by comparing the observation's
  OBSERVED retained-parent identity (the one binding fact a caller
  cannot supply); the reviewer proved the check load-bearing by
  deleting it in scratch (only the cross-action row failed) and
  ruled the landed form STRONGER than its own remedy 2. Second
  occurrence of the caller-supplied-restatement class (first: 2.4
  P1) — named as a Phase 5 dual audit item: every gate that copies
  binding fields from an argument. SETTLE DISPOSITIONS (docket of
  8, all verified true of the tree by the 3.3 reviewer):
  (1) preflight/plan_complete — DETERMINED never-gain-boundaries;
  dated §3.5 determination filed at this settle; census 165 stands;
  5.1's evidence duty reading amended accordingly. (2)
  retain_managed_parent — KEEP as the documented cfg(test) door
  retainer; revisit only on an R2-E production caller. (3)
  Git-directory workspace-root binding — R2-E input; the typed
  preflight refusal is production-safe today. (4)
  staging_directory_flush second site — ACCEPTED one-key/two-sites
  (interior-first ordering proven; no sixth key for zero coverage
  gain). (5) multi-component writer interruption rows — assigned to
  Phase 5.1's evidence train. (6) partial-handoff resume via the
  test door — R2-E interface input; documented settled-tuple caveat;
  the admission-classifier widening belongs with real consumers.
  (7) blanket allow(dead_code) narrowing — assigned to Phase 4.3.
  (8) seam-level action binding in AuthorityObservationFactsV1 —
  assigned to Phase 4.3 (NOT deferred to R2-E: the defect class has
  fired twice), in the action-digest form with the reviewer's
  refinement adopted: it rests on slot-name derivation, so it lands
  PAIRED with an explicit consumer obligation stated at the seam.
  Also to 4.3: the 3.3 round-2 P3s (detail-string pin on the
  cross-action row; the stranded leaf_digests doc line). Escalation
  ledger riding to the Phase 5 dual: 2.3, 2.4, anchor, 3.1, 3.1b
  (2.2's discharged at its landing; 3.3's discharged at round 2)
  plus the named class audit. Phase 4 launches on `3a45619`.

  **Step 4.1 LANDED 2026-08-23 (gwz-core `9c454ce`) — review GO
  round 1, no escalation (6 P3).** E18-E21 convert in situ through a
  sealed composition of P1's arms carrying the legacy identity
  vocabulary. SCOPE DIVERGENCE RATIFIED (lane owner): my brief's
  publish_verified_no_replace route was WRONG — HostPlatform-bound
  to the closed support table while the legacy writer is the live
  path on any persistent-handle filesystem; the reviewer added a
  fourth leg (the route would not even compile — pub(super)) and
  ruled the twin seam SOUND with both halves pin-protected.
  **E21 latent defect, graded P2 as it stood, found and closed by
  this step:** authority_name embeds no identity digest, so the old
  code completed a same-byte authority substitution outright;
  adversarial-only, same-user boundary, hence P2 not P1. Recorded
  for the settle and the metrics table. cleanup.* flips ZERO keys —
  duty never attached (all 11 need AdmittedActionV1); re-reserved
  for R2-E via the §3.5 record lifted at this landing (with the
  reviewer's one-word placement fix). Landing adoptions:
  transition.rs and residue.rs gain flat digest pins (both sealed);
  platform.rs pin recomputed; the platform.rs split (1,093 lines) is
  BOUND TO STEP 4.2 BY NAME. Riding notes: twin-seam unification
  docketed for R2-F ([P3-1]); the authority_name
  non-self-checking-name design note carries to R2-E; [P3-4] (one
  fault variant, four sites — split or record deliberate) routed to
  4.2; the macOS-narrowing prose error ([P3-2]: it is MNT_LOCAL, not
  persistent-id absence) corrected in the lifted record. L2-04
  harness green locally (86 tests, 24 tuples); its network matrix
  job runs automatically on push (path filter covers
  scripts/checks/**) — no dispatch needed. Both matrices dispatched
  at `9c454ce` (Windows 32607411300 = run 19, first native execution
  of the 3.1b/3.2/3.3/4.1 suites; platform 32607412448).

  **Step 4.2 LANDED 2026-08-23 (gwz-core `51a9cba`) — review round 1
  GO with §9 escalation RECORDED (2 P2, 5 P3); round 2 GO, both P2s
  closed AT THE MECHANISM, escalation DISCHARGED.** The legacy
  Windows anchor machinery retired: nonce → one deterministic
  staging name (the standing R2 stop-clause violation removed);
  alias remove_file → durable retirement onto ordinal-indexed names
  chosen smallest-free from observed state (round 1's fixed
  destination was a REACHABLE permanent wedge — deleted, not derived
  away; the reviewer independently re-derived that count-based
  naming would have wedged inside the fix). All ten boundaries
  driven incl. both retirement boundaries, natively probe-verified
  (probe/anchor-retirement-win, final run 32612243125, 82/0, with
  the hard-link pin proving the retirement branch executed; the
  probe cycle itself caught a fixture bug red-then-green). [P3-2]
  fixed in-package: authority::scratch_name's nonce on 4.1's
  converted E20/E21 — Phase 4's own trigger condition — now
  action-scoped deterministic; getrandom gone from authority.rs.
  SCOPE DIVERGENCES RATIFIED (lane owner): barrier.* (16) and
  terminal.* (11) flip ZERO keys — both vocabularies name frozen
  admitted-action protocols (the roaming anchor; terminal
  retirement), consumer conversions outside R2-D; the legacy anchor
  is the roaming protocol's ANCESTOR, not an instance; all 27 keys
  re-reserved for R2-E via the §3.5 records lifted at this landing.
  MACOS PLATFORM FACT pinned by measurement: per-hard-link
  ATTR_CMN_OBJPERMANENTID means the stranded-alias state was never
  reachable there. Landing adoptions (both reviewer-blessed):
  platform.rs converts FLAT→TREE pin (covers the anchor module +
  its 548-line evidence file); authority.rs gains a flat pin as the
  private family's naming authority. PROBE DISCLOSURES accepted:
  four pushes total to probe/anchor-retirement-win (remediation +
  fixture fix beyond the one authorized), same branch only, verified
  by the reviewer (origin/main untouched, no tags). LANDING INCIDENT
  (ritual 2 vindicated): my overlay rsync silently dropped the new
  platform/anchor/ subdirectory (git status collapses new dirs to
  one line; --files-from without -r copies them empty) — caught
  BEFORE push by the pin cross-check against the reviewer's
  independently computed tree digest; anchor files copied, pin
  recomputed to the matching value. Riding to 4.3/settle: [P3-6]
  the racing-drive window has no executed row (three of four
  executed + the gap named); [P3-7] survey's Invalid is a permanent
  no-exit refusal on foreign contamination (the block's
  reachable-state claim stays precise); the 4.2 review's round-1
  [P3-4] freeze-cite drift fixed at this landing against the landed
  tree. Budget: net +1062 across both rounds (the dual corrected the round-1 figure by one) (≈28% production /
  ≈20% bound relocation / ≈52% evidence) — second consecutive
  over-budget Phase-4 step, composition recorded per the reviewer's
  ChangeBudget guidance. Escalation ledger for the Phase 5 dual
  gains: 4.2 round-1 (2 P2, discharged round 2 — recorded as
  discharged). Run 20 dispatched at `51a9cba` (Windows 32613728318,
  platform 32613729322).

  **Step 4.3 LANDED and PHASE 4 SETTLED 2026-08-23 (gwz-core
  `514f8e6`; single-axis regime per thin-A1, the plan's Phase-4 dual
  line superseded on record).** Review GO round 1 (1 P2, 7 P3 — all
  record-class; corrected at the landing per the review's own
  values, including the lift-blocking exact_row fix, the honest
  dead-code figures, and the [P3-6] rationale clause; escalation
  RECORDED and routed to the Phase 5 dual). THE COEXISTENCE
  DECISION (audit P3-3, A1-gating) IS MADE AND RECORDED:
  **quarantine/relocation**, per §7.2's adopted direction plus two
  facts new since adoption (Windows anchor permanence makes the
  grammar collision permanent; the retirement family is
  crash-bounded, not constant-bounded); execution pinned to the
  R2-F relocation package; the A1 gate stays fail-closed in code.
  Record: dev-docs/GwzM5-8R2DPhase4Closure.md §2.7. The
  dirent-barrier resume-window residual is verified CLOSED in code
  (all six Ready edges through the keyed prologue; regression
  pinned; R2-F power-loss companion survives). Settle item 8
  LANDED: the 2.4 seam refuses mis-paired action digests itself,
  consumer obligation on the signature, two smuggles/two gates both
  executed. Settle item 7 LANDED: blankets narrowed; dead-code
  reality recorded FOR STEP 5.1'S AUDIT: **481 items were hidden by
  the module-level allows; the global measure is 1657 spans / 85
  files**, all R1/R2-frozen interface awaiting R2-E, zero orphans.
  PHASE 4 TOTALS: E18-E22 converted; the R2 stop-clause nonce class
  extinct (anchor + authority family); raw-rename surface = one
  delegation reference; cleanup./barrier./terminal. all
  re-reserved for R2-E with sanctioned records (the census stands
  at 165; the executed arithmetic is the settled tuple §4.8's
  (the "51" this record first carried reconciled with no sum —
  corrected on the dual's finding), with managed_bootstrap 28/30); three consecutive fully-green matrix
  trees. ESCALATION LEDGER — FINAL FOR THE PHASE 5 DUAL: riding =
  2.3 (1 P1+3 P2), 2.4 (P0+P1+P2), anchor (1 P1+4 P2), 3.1
  (1 P1+1 P2), 3.1b (recorded), 4.3 (1 P2, remediated at landing);
  discharged-on-record = 2.2, 3.3 (round 2), 4.2 (round 2); plus
  the named class audit (caller-supplied restatement — fired 2.4,
  3.3; hardened at the seam by 4.3). NEXT: Phase 5.1 evidence
  train, then the 5.2 settled dual (Fable ×2, cross-model per the
  plan's own line, two-round cap).

  **Step 5.1 LANDED 2026-08-23 — THE SETTLED TUPLE TREE IS gwz-core
  `d45458d`.** The twelve-gate train ran and recorded verbatim
  (dev-docs/GwzM5-8R2DSettledTuple.md, 683 lines, run-21 slot
  filled); it caught and repaired a red gate (the boundary unit
  suite's companion assertion, broken by MY 4.2-landing flat→tree
  pin conversion — repaired and HARDENED: the generic prefix would
  have passed on any finding) and a stale audit note (4.3's own
  narrowing falsified it; corrected and measured at 29 live
  warnings). Evidence: 107 keys tabled site+row+both-variants; 2
  determinations; 38 reserved with records; ONE GAP REPORTED NOT
  PAPERED — runtime.*'s 18 keys are declared Executed on a disjoint
  six-variant mechanism with no per-key correspondence and a
  hard-coded exemption; §3.5 declares the exception, but its worth
  is below the other families' standard — FILED FOR THE 5.2 DUAL
  with two census-neutral options (re-reserve like cleanup.*, or
  restate the frozen claim). Phase-3 settle item 5 discharged (10
  multi-component rows, correctly refusing partition claims).
  Platform record: nine of nine green — three consecutive
  fully-green trees on all three arms. NEW OPEN ITEMS for the dual:
  the L2-05 merge-doc gate is wired into NO workflow (passes only
  where the sibling gwz-cli checkout exists — a gate that only
  passes on a developer's machine is not a gate; CI-wiring item);
  the two not-run gates scoped honestly (the network matrix job —
  CI's; regen.py --check — the release venv's).
  managed_mutation.rs at 1,251 lines is the one over-trigger file,
  deliberately unsplit during the evidence step. Budget +53 net vs
  <300. THE 5.2 SETTLED DUAL LAUNCHES ON `d45458d` — Fable ×2 per
  the tier policy (the plan's "cross-model" line is satisfied in
  axis, not vendor; recorded as a deliberate reading), two-round
  cap, carrying the full escalation ledger and the runtime.*
  adjudication.

  **R2-D IS SETTLED — 2026-08-23, dual #3 of 3 consumed, GO/GO at
  round 2 within the two-round cap. The settled object is gwz-core
  `b91bdeb` with the freeze as amended at the remediation commit.**
  Round 1: Code GO (0 P0/P1/P2, 6 P3 — the named-class audit
  returned an 18-gate table with an affirmative no-live-instance
  statement; the one remaining seam SHAPE, BarrierIntentV1::issue,
  is production-unreachable and now a BINDING R2-E obligation) /
  State NO-GO (1 P1 — THE AGGREGATE CATCH THE DUAL EXISTS FOR: the
  three Phase-2 activations never got their §3.5 amendments, 35
  keys misdeclared "reserved", fallen in the seam between Phase 1's
  row-edit and Phase 3's annotation disciplines; + the runtime.*
  adjudication RULED RESTATE with drafted text; 3 P3). One merged
  document-class remediation: the reviewer-drafted annotations
  lifted verbatim (mechanically verified 45/45 and 44/44 lines),
  the comment-clause fix at `b91bdeb`, the register completed, the
  exactness bundle corrected. Round 2: Code GO unconditional /
  State GO — "the map is now true for all ten families; what
  remains open is exclusively named P3 residue filed where the next
  lane reads it. R2-E may start behind this gate." The deferred
  second-axis design is VINDICATED on the record: every riding
  escalation got its scrutiny and the dual caught the one defect
  class only an aggregate view could see. Reports:
  GwzM5-8R2DSettled-Review{Code,State}.md (rounds 1+2 appended).
  THIN-A1 STATUS: R2-D settled ✓ and M5b bound proofs green on the
  settled tree ✓ — the A1 activation's two preconditions stand.
  Remaining: R4b-G, then A1 activation.

  **R4b-G evidence executed 2026-08-23
  (dev-docs/GwzM5-8R4bG-Evidence.md, 851 lines): 22 PASS / 7 FAIL /
  6 RE-FRAMED (5 evidenced) / 2 DEFERRED-BY-AMENDMENT / 1 PENDING.**
  Members verified at the tuple pins (gwz-cli 3cca145 139/0; gwz-py
  929efb0 330/0 with regen --check green; taut f008419); the
  acceptance object stated (the ~25k-line R4b reverse lifecycle;
  GO unblocks the M5b-IMPL settled review and the escape
  implementation packages). Remediation of F-1..F-5/F-7 dispatched
  (matrices for F-6 dispatched at b91bdeb = run 22); new citable
  measurement root_fault_matrix 318.71s release at b91bdeb (vs the
  576.03s on record). LANE-OWNER ADJUDICATION OF J-1, on record for
  the dual: M5b-IMPL (3e60529, 8c1624a) reached b91bdeb's ancestry
  ahead of R4b-G against the must-wait clause
  (GwzM5-8M5bNoFfDesign.md:976-986) — the deviation is REAL and was
  a sequencing artifact of the 2026-08-22 acceptance train, before
  thin-A1 re-framed R4b-G behind the R2-D settle. Ruled ACCEPTED
  WITH RECORD on four legs: M5b measured zero production lines
  under its ceiling with T-6 and the clean-tree re-cut; its dual
  was round-1 clean on both axes; the leaned-on call-graph gate
  (F-3) lands with this remediation as a standing guard rather
  than a retroactive claim; and the D3 dual's round-2 re-verdicts
  verified the five M5b surfaces byte-identical on both axes
  (`GwzM5-8D3Impl-ReviewCode.md:447-448`: `store/tests.rs` at
  exactly +2/−0, M5b byte-identity intact;
  `GwzM5-8D3Impl-ReviewState.md:550-552`: M5b 37/0 with all five
  files byte-identical) — **[leg restated 2026-08-24 per the
  R4b-G Evidence axis's P2-1: the original text mis-attributed
  this verification to the settled dual, whose reports do not
  mention M5b; the fact was true, its citation was not.]** The
  R4b-G dual judges the acceptance; if it rules the deviation
  disqualifying, the remedy is the M5b-IMPL settled review
  running BEFORE A1 rather than concurrent with it.

  **R4b-G DUAL ROUND 1 (2026-08-24, peer-blind, at the tuple
  gwz-core `78badbc` / gwz-cli `3cca145` / gwz-py `929efb0` /
  taut `f008419`): Correctness GO-conditional (1 P2, 6 P3) /
  Evidence NO-GO (1 P2, 6 P3)** — reports
  `GwzM5-8R4bG-Review{Correctness,Evidence}.md`. Merged round-2
  remediation executed 2026-08-24: **tooling at the next gwz-core
  train; records in this commit.** C-1 (the "every M4"
  byte-equivalence clause measured NOT MET at 13/39) is **bound to
  the A1 activation review's input register as BLOCKING-FOR-A1 per
  L1-19** (`AgentProcessRules.md:392-401`), routed under
  `GwzM5-8ThinA1Amendment.md:43-55`; C-2 (4 unfixtured scenarios)
  rides the same register at P3. **J-1 is RATIFIED by both axes**
  on independently re-verified facts (Correctness §5 J-1;
  Evidence §6), **the before-A1 remedy is NOT triggered**, and the
  **M5b-IMPL settled review is recorded as owed pre-A1** on its
  frozen tier. Records landed this pass:
  `GwzM5-8R4bG-Evidence.md` (J-1 leg 4, F-7 provenance, the C-1
  reconciliation and the PARTIAL gate condition),
  `GwzM5-8R2DSettledTuple.md` §11.1 (perf-pricing date, C-1, C-2,
  M5b-IMPL), `GwzM5-8M5bNoFfDesign.md:976-986` (annotation),
  `GwzWindowsMatrix-Classification.md` (run-number header note).

  **R4b-G IS ACCEPTED — 2026-08-24, both axes GO at round 2 within
  the two-round cap; the round-1 conditional CONVERTED with its
  condition satisfied on the record and verified, not waived. THE
  ACCEPTED TUPLE, restated literally per the Evidence axis's close
  ruling: gwz-core `1bd885f` / gwz-cli `3cca145` / gwz-py
  `929efb0` / taut `f008419`.** Zero open P0/P1/P2 against the
  gate object. The acceptance record carries: the byte-equivalence
  battery is PARTIAL (7 proven + 6 refusal-pinned of 39) and must
  not be cited as green (evidence §12.8); C-1 [P2, BLOCKING-FOR-A1
  per L1-19, four live rows named] and C-2 [P3] are bound on the A1
  input register; J-1 stands ratified with its leg 4 restated to
  the D3 dual. CI at the tuple: retained-readers green at
  `1bd885f`; boundary lane  at commit time (scripts-only delta
  over the green `78badbc`; the close is not conditioned on it).
  UNBLOCKED: the M5b-IMPL settled review (launching) and the
  operator-escape implementation packages (second lane, still
  BLOCKED ON OPERATOR HANDOFF — unchanged). PRE-A1 QUEUE, exact:
  (1) the C-1 closure package — state and test the adaptation
  dispositions for F-BASELINE/F-MARKER/F-LOCK and
  J-NO-PUBLICATION-UNBORN; (2) the M5b-IMPL settled review per the
  frozen dependency statement; then (3) THE A1 ACTIVATION REVIEW,
  whose input register is complete (the acceptance-debt record,
  the D3/D4 residual dispositions, the signed perf pricing,
  C-1/C-2). OPERATOR ITEM (non-gating): the ChangeBudget
  charging-convention row is filed OPEN with both reproducible
  figures (2,028 uniform / 582 review-artifact) — the ruling is
  the operator's.

  **THE PRE-A1 QUEUE IS COMPLETE — 2026-08-24. The A1 activation
  review launches on the tuple gwz-core `26f48f5` / gwz-cli
  `3cca145` / gwz-py `929efb0` / taut `f008419`.** (1) C-1 CLOSED
  at `26f48f5`: all four live rows dispositioned FROM the frozen
  contract with the derivations quoted, tested through the real
  decode path; new finding surfaced and pinned — the enumeration's
  J-twin labels were backwards (the contract's born-root exclusion
  makes the UNBORN twin the adapted one); the driver's partition
  off-by-one (1572 vs 1573) reported not silently fixed. (2) The
  M5b-IMPL SETTLED REVIEW is GO (0 P0/P1, 1 P2, 3 P3) — the J-1
  obligation discharged pre-A1 on its frozen tier; six trains of
  drift accounted to the line. Its [P2-1] (T-5's no_ff envelope
  pair absent) is CLOSED BY LANE-OWNER NARROWING per the finding's
  own offered remedy, on structural grounds the closure lane
  measured (envelope classification is header-keyed; the body
  cannot participate); the built candidate pair is saved with
  digests and rides any future evidence regeneration. Its [P3-1]
  (the forged-action gate's ChangeBudget row) is FILED
  existence-first with the commit named, numbers withheld under
  the OPEN convention. (3) The A1 register, final: the ~3.5k
  acceptance-debt named exception; the D3/D4 residual dispositions;
  the signed perf pricing (318.71 s); C-1 closed; C-2 [P3]; the
  T-5 narrowing (the review judges it); the PARTIAL
  byte-equivalence statement; M5b settled GO. The activation
  review's object: enable the v1 writer + --no-ff per thin-A1 §2,
  with the §14 ninth-stop-clause post-A1 re-scope and the R2-F/R5
  native-evidence RELEASE gates explicitly unmoved.

  **THE A1 ACTIVATION REVIEW, ROUND 1 (2026-08-24, peer-blind, on
  the accepted tuple): Safety GO-CONDITIONAL / Completeness
  GO-CONDITIONAL — zero tree defects on either axis.** Reports
  `GwzM5-8A1Activation-Review{Safety,Completeness}.md`. The Safety
  report's §2 is THE BINDING ACTIVATION SPEC: six compile gates,
  four runtime gates (the NoFf-refusal fall COUPLED with the
  message-validation exclusion fall), both CLI unhides, and the
  must-not-flip list. Safety conditions: [P1-1] F-MARKER/F-LOCK v0
  resume recovery pinned post-activation; [P2-1] a Finalizing
  pre-check gating the adaptation preflight; [P2-2] CI wiring of
  the unwired checkers; [P2-3] the landing record set incl. the
  OPERATOR-SIGNED D3/D4 dirty-boolean residual disposition (sites
  cursor.rs:305-306/:332-333/:498) — **the operator signature is
  the halting item: branch (a) accept-as-residual or branch (b)
  remediate first; without it activation is NO-GO.** Completeness:
  the 364 routed phrases swept (17 delivered / 12 judged-here /
  14 post-A1 / 2 leaked, both placed); its [P2-1] (T-5 artifact
  durability) CLOSED pre-build — candidate pair archived to
  `GwzM5-8T5CandidatePair.patch` + the R2-F carrier owner row in
  the settled tuple §11.2; round-2 conditions: the [P2-1] legs and
  the activation record carrying the report's §3 enumeration; P3
  folds owed to the landing record set (P3-1 Q3 citing line, P3-2
  abandonment-witness placement, P3-3 owners at register close,
  P3-6 register consumption statement, F5 §9 dispositions).

  **THE A1 ACTIVATION PACKAGE IS DELIVERED — 2026-08-24, in-tree
  (uncommitted) over the accepted tuple; report filed verbatim as
  `GwzM5-8A1ActivationPackage-Report.md`; ROUND 2 DISPATCHED to
  both axes on the tree + filed report.** 88 files gwz-core
  (+1852/−609; new: `merge/model/version.rs`,
  `merge/v1_lifecycle/start.rs`, `tests/g23/a1_activation.rs`),
  3 gwz-cli, 2 gwz-py. Lands: §2.1 all six compile gates FELL (G6
  REPLACED not un-gated — `_for_r3_tests` re-exports renamed to
  production decode names, fault injectors keep cfg; the gate was
  wider than the six coordinates — 97 further cfg(test) markers +
  28 cfg_attr(Serialize) fell; the fall measured behaviour-neutral
  at 1572/0); §2.2 R1+R2 in one edit, R3 =
  `classify_merge_record_header` with PRODUCTION{v0,v1} (T-2
  inverted at header dispatch + archive decoder), R4 PARTIAL — THE
  ROUND-2 DECISION: `select_record_version = max(floor, semantic)`
  is production and NoFf→V1 writes v1 end-to-end today, but
  ACTIVE_WRITER_FLOOR stays V0 — raising it was MEASURED to break
  every ordinary start (no production v1 owner for root
  participants / dry-run prediction / drift-conflict surfaces /
  the v0 event stream); one-line change when that owner lands, the
  max proven by test. §2.3 both CLI surfaces (docs tripwire
  inverted, CLI.md regenerated); §2.4 the production callers
  (V1Router with sealed MergeAuthorityBackend; classify_open_record
  envelope probe; NEW `create_open` + `handle_start_durable_v1`;
  dead-code warnings 921→63 when dispatch landed). Conditions
  [P1-1] / [P2-1 via option (i), archive shapes riding as named
  residual] / [P2-2 — L2-05 unwired with the reason recorded] /
  [P3-1: T-1/T-2 inverted, T-3 re-pinned 16→19, T-4 unchanged] MET
  per report; must-not-flip verified 9/9, F-3 still 0 hits. Gates
  at delivery: core 1582/0/1 (census 1583, +10), cli 139/0, py
  330, L2-04 86/0 tuple_count 24, scenario map ok, docs 147
  assertions ok. EXPECTED-RED, landing-train duty (report §6): 5
  boundary digests (merge/mod.rs root manifest; artifacts.rs +
  plan.rs flat; observe.rs + v1_lifecycle/mod.rs tree), 3 R4b-G
  count markers (fault 254→255 and 917→926; byte-equivalence
  114→119), and 11/69 checker probe-harness tests (G1's
  blanket-allow expiry broke the probe compile; LANE-OWNER RULING
  on record: apply the builder's minimal fix — emit
  `#[allow(dead_code)]` with the injected probe — at landing;
  the guarded property itself is intact, F-3 scan 0 hits). Named
  residuals (report §7): the R4 ordinary-start row; ONE BEHAVIOUR
  PIN MOVED deliberately (the g23 finalization nested-fault test's
  final window now asserts the migration; v0-fault-injector
  coverage on the authority path reduced for the seven whitelisted
  shapes — flagged to round 2); positive executed evidence that
  migrated Boundary/Staging prefixes keep the v0-equivalent next
  action (contract §4); the gc_archived family's
  no-production-caller allowance. Insurance:
  `scratchpad/insurance-a1/` M-A…M-I-delivery. **THE LANDING
  HALTS ON THE OPERATOR SIGNATURE.**

  **A1 ROUND 2 IS GO/GO — 2026-08-25.** SAFETY GO (0 P0 · 0 P1 ·
  1 P2 · 5 P3; report §"Round 2" appended): the package accepted
  against the enumeration with every quoted gate re-run by the
  axis (census 1583 reproduced; py 330/0 against a native module
  the axis REBUILT itself — the builder's py tail ruled
  not-evidence, [P3-R2-4]); the wider compile-gate fall ruled
  in-spec (the 28 Serialize cfg_attrs are logically G2); **R4's
  V0 ordinary-start floor RULED an ACCEPTED NAMED RESIDUAL** on
  four grounds, with three binding record conditions — (a) the
  ordinary-start v1 owner becomes a first-class named milestone
  and the floor raise lands WITH it as one reviewed change, (b) a
  dated annotation on the frozen contract §2 creation-matrix A1
  row, (c) the release-time retained-reader manifest describes
  SHIPPED behaviour (ordinary=v0, no-ff=v1); the moved pin
  ACCEPTED with the compensating obligation named — the
  eligible-row upgrade-failure fallback, carrier R2-E
  ([P3-R2-2]); **[P2-R2-1]: the probe-harness cure is FOUR
  classes** (the recorded allow-emit fix cures only
  probe-compile; the sentinel reinstated UN-GATED; the
  seam-surgery string updated; the digest re-pins) — applying
  only class (a) would land a RED push on the newly wired CI
  step. COMPLETENESS GO (0 P0/P1/P2 · 2 P3; §"Round 2"
  appended): its round-2 conditions closed ([P2-1] byte-identical
  archive at `47bbd7a` + the §11.2 OWNED-CARRIER row; the §3
  enumeration verified in-tree with executions g23 119/0, no_ff
  32/0, version 15/0, map 39/41/13); **THE LANDING SPECIFICATION
  L1-L14 filed for verbatim folding** (L7/L8 added to the fold
  list by the axis); [P3-7] a fresh `unreachable!` v1 arm at
  decode.rs:123 (typed-twin conversion, L13); [P3-8] three named
  blockers lacked named carriers. LANE-OWNER RULINGS for the
  record set: the ordinary-start v1 owner is MINTED AS **M5c** (a
  first-class named milestone on the register; the `version.rs:39`
  floor raise rides it as ONE reviewed change per contract §9
  discipline); the archive/GC family (gc_archived route,
  18-UNBOUND record debt, the archive-equivalence mechanism
  decision, the two archive shapes riding [P2-1] option (i)) is
  owned by **R2-E's archive/GC consumer sub-package**; the
  moved-pin coverage restoration IS [P3-R2-2] at R2-E (the two
  axes converged on the same arm); L10 item 2: `cargo fmt
  --check` release-lane-only ADOPTED-AS-IS; L5: the
  abandonment-witness probe LANDS with the train (the
  commit-the-probe arm); L13: the typed-twin conversion LANDS.
  **LANDING TRAIN DISPATCHED IN-TREE** (the package builder
  resumed): the four-class cure, L13, L5's probe, the [P3-R2-5]
  doc nit — then count markers and digest re-pins computed from
  the FINAL tree, then the full gate set incl. the boundary
  harness 69/69 and a fresh-maturin py run. THE COMMIT STILL
  HALTS ON THE OPERATOR SIGNATURE (L1; the site set verified
  in-tree: cursor.rs:283-308/:310-335 fallthroughs,
  phase.rs:187,198, observe/finalization.rs:217, cursor.rs:498).

  **THE OPERATOR SIGNATURE IS IN — 2026-08-25, BRANCH (a), verbatim
  "sign branch (a)".** The D3/D4 dirty-boolean residual over the L1
  site set is ACCEPTED AS A NAMED RESIDUAL; remediation, if any, is
  post-A1 work scheduled by the operator. Recorded in
  `GwzM5-8A1ActivationRecord.md` §1 with the formal disposition
  text; the Safety [P2-3] signature hinge is satisfied. **THE
  LANDING HALT IS LIFTED.** The landing now waits only on the
  landing train's green report, then: overlay ritual (pins
  recomputed from pristine extraction, checker+gates green in
  overlay), the coordinated `gwz commit` (records + contract
  annotation + lock re-pin + package), the exact-ref gwz-core push,
  and the three-arm matrix dispatch at the landing commit (L11).

  **A1 IS LANDED — 2026-08-25. THE ACTIVATION TUPLE: gwz-core
  `1a31851` (exact-ref pushed, main 26f48f5 → 1a31851) / gwz-cli
  `3000916` (local) / gwz-py `3d19dcd` (local) / taut `f008419`
  (unchanged). The v1 merge writer and `--no-ff` are live on the
  mainline.** The landing train's tree duties completed green
  first: the four probe-harness cure classes PLUS A FIFTH the train
  discovered and cured (cfg-agreement on injected observer-caller
  probes — pre-A1 the `v1_preservation_image` call site was itself
  v1-gated so edge and call agreed; G1 broke the agreement; cure
  gates the injected statement `#[cfg(test)]`, checker
  cfg-indifferent); L13's typed twin (`RecordDecodeError::Body`,
  "the v0 decoder received a v1 record"); **L5's abandonment
  witness PASSES with EQUAL shapes** —
  `service_level_abandonment_of_a_not_started_action_is_mode_blind`
  (no_ff_wire.rs:319; both modes `Refused {
  MergeRecoveryRequired, "v1 transition predecessor or authority
  mismatch" }`; non-vacuity verified; zero NoFf tokens, T-3 stands
  at 19 files) — M5b [P3-3]'s unreachability executable for the
  first time; the sentinel reinstated un-gated; [P3-R2-5] doc nit;
  counts 256/926/119 and the five digests re-pinned from the final
  tree (merge/mod.rs `76e4830e…`, v1_lifecycle/mod.rs `74a416a6…`
  moved past the report — sentinel + witness). Final tree: census
  1584 (1583+1 ignored), harness 69/69, checker ok, clippy clean,
  R4b-G batteries green, cli 139/0, py 330/0 on a fresh maturin
  build (01:42:48 .so). gwz-py Cargo.lock refresh ruled in (record
  §8). LANDING RITUAL CLEAN: overlay at origin tip `26f48f5`, 91
  files `rsync -rR`, checker green IN OVERLAY (the pin
  cross-check), check/fmt/clippy-from-clean/focused (5+2+1+1)
  green, commit, exact-ref push, live reconcile with LIVE SET ==
  LANDED SET verified by diff before reset — zero stash incidents.
  Root activation commit (this one): the operator-signed record
  `GwzM5-8A1ActivationRecord.md` (all slots filled), the frozen
  contract §2 A1-row dated annotation, the ChangeBudget L12 row,
  the classification L11 dated update, `gwz.lock.yml` captured to
  the tuple. MATRIX AT THE LANDING COMMIT: windows `32749489320` +
  platform `32749492896` dispatched at `1a31851`; push-triggered
  boundary + retained-readers (`32749441874`/`32749441866`) running
  with the newly wired [P2-2] step; pre-attributed expected-green:
  the +11 package tests and C-1's 3 first-native executions.
  **LANDING-COMMIT CI INCIDENT + CORRECTIVE `8e40fa8` —
  2026-08-25.** At `1a31851`: Windows/Platform/Retained-readers
  GREEN; the boundary run `32749441874` FAILED on the newly wired
  [P2-2] step's FIRST command — `check_m4_scenario_map.py` resolves
  its map doc ONE LEVEL ABOVE the checkout by design
  (`:59 ROOT.parent/dev-docs/GwzM5-8R4bG-Evidence.md`), absent on a
  single-repo runner: the SAME blocker class the workflow's own
  L2-05 comment records, missed by the builder, both round-2 axes,
  and the landing train because every execution was
  inside-workspace. Corrective `8e40fa8` (overlay ritual, exact-ref
  push, live reconciled): the m4 map un-wired with the blocker
  recorded beside L2-05's — same owner, same R2-F
  multi-repo-checkout cure (tuple §11.3 item 7); the privacy and
  call-graph batteries STAY WIRED (verified no workspace-root
  reads; their contents ran green in CI's earlier step at
  `1a31851`). [P2-2] substance restated in activation record §17
  (the addendum). LESSON MINTED (wiring class): local-green proves
  nothing about single-repo-runner topology — wiring acceptance
  requires executing the wired command under the CI checkout
  topology, or an explicit path-resolution topology audit, BEFORE
  the push. Boundary run at `8e40fa8`: **`32754064482` GREEN**
  (completed:success in ~41 min — the wired privacy + call-graph
  batteries executing for real in CI, pinned counts and all). With
  it, EVERY CI surface at the activation is green: Windows
  `32749489320`, Platform `32749492896`, Retained readers
  `32749441866` at `1a31851`; boundary at the corrective
  `8e40fa8`.

  **THE v0.11.0 RELEASE TRAIN IS CHARTERED AND LAUNCHED —
  2026-08-25.** Plan: `GwzM5-8A1ReleasePlan.md` (gates G1-G6, phases
  R1-R6). Operator decisions on record: **vNEXT = v0.11.0**
  (verbatim "v0.11.0"); **R4 = branch (i)** (verbatim "branch (i)")
  — the narrow §4.4 wedge-runbook slice is handed off to the lane
  (docs only; escape implementation packages remain second-lane
  blocked); R6 timing and the R5 member-push/tag scheduling remain
  OPEN. On "launch R1/R2/R3": three parallel Opus builders launched
  in isolated gwz-core worktrees at `8e40fa8` — R1 (Decision 2 A′
  pre-mutation foreign-filter refusal, ~350 LOC, focused State
  review to follow), R2 (Decision 1 B birth-time CRLF pins + the
  un-pinned matrix sentinel + decided-annotation texts delivered
  not applied, ~200 LOC, focused State review to follow), R3
  (retained-reader A1 generation + release-lane battery wiring
  behind the §17 topology audit + version reconciliation to
  v0.11.0 + release-notes draft, interior single-axis). Shared-file
  discipline: builders write only their own worktrees; every
  dev-docs edit is the lane owner's at landing. Landing order:
  each phase lands as its own reviewed train, R1 first if
  contention arises (G1 is the forbidden-ordering gate). R4
  (runbook) queues behind R1's final refusal surface.

  **R1 DELIVERED WITH A MATERIAL DISCOVERY — 2026-08-25: Decision 2
  A′ WAS ALREADY LANDED at origin/main**, by `9939b02` (2026-08-16,
  "Land the filter-policy (D1+D2) and R2-F missing-tests packages",
  + follow-up `90d3f8a`) — the same day the decision packet was
  written. The code closed; THE LEDGER NEVER RECORDED IT (no dev-doc
  mentions refuse_foreign_filtered_rewrites, 9939b02, or
  filter-policy), which is exactly how the release plan came to
  re-schedule R1.1/R2.1 as unstarted work. **[CORRECTED 2026-08-25
  per R1.2 [P2 F-1], condition C1: the preceding sentence is FALSE
  as written. The ledger DID record the decisions — the D2/A′
  discharge is annotated at
  `GwzM5-8ExactEvidencePlatformAmendment.md:156-175`, D1's closure
  at `:108-121` — and a State review DID run at the `9939b02`
  landing (its commit message: "State review GO, F2 same-train
  condition discharged"; findings F1/F2/F3/F5/F6 traceable by
  content in the amendment landed in the same commit — including
  the `.gitattributes` asymmetry itself, recorded as State F1 [P3]
  at `:117-121`). What was never produced is the REPORT ARTIFACT —
  the D1+D2 package is the sole landing of that week with none —
  and `GwzM5-8A1ReleaseR1-ReviewState.md` is now that artifact.
  The true failure mode: the amendment recorded it, the searchable
  program surfaces (checkpoint, review inventory, plan) did not,
  so both the release plan and the R1 builder's grep-scoped search
  missed it. Decision ledger annotation per C1: decided A′, landed
  `9939b02`, reviewed-at-landing (report unfiled), re-reviewed and
  GO at `8e40fa8` (`GwzM5-8A1ReleaseR1-ReviewState.md`).]** The R1 package reduced
  to: the conformance audit (predicate matches the packet's four A′
  clauses; both recovery sites refuse pre-mutation, verified against
  the ref-transaction ordering; the clone-funnel disable_filters
  site correctly excluded as creation-time); ONE missing test built
  (+77, `checked_rollback_proceeds_when_the_configured_filter_is_
  outside_the_rewrite_set` — the rewrite-set-scoping sentinel,
  MUTATION-VERIFIED: widening the diff scope trips it); full gates
  green in the r1 worktree (g12 21/0, lib 1584/1, checker ok, zero
  pins moved). ONE REAL FINDING, empirically probed: the
  `.gitattributes`-ASYMMETRY RESIDUAL — the attribute stack is read
  pre-checkout (AttrCheckFlags::default), so when .gitattributes is
  itself in the rewrite set and only the TARGET side carries the
  coverage, the rollback proceeds and re-creates the exact wedge
  precondition A′ refuses (probe: ref moved, raw bytes under
  restored configured filter). Narrow (coverage must arrive WITH
  the checkout) but real; the builder correctly did not widen scope.
  **R1.2 FOCUSED STATE REVIEW DISPATCHED** on the landed object +
  the test package, with the asymmetry decision, the provenance
  question (was 9939b02's A′ share ever reviewed? if not this
  review is the owed one per packet §4 step 1), and five
  review-focus items in its mandate; report files as
  `GwzM5-8A1ReleaseR1-ReviewState.md`. ADJACENT: R1 observed
  Decision 1 B ALSO landed at 9939b02 (both pin edges +
  cfg(windows) doctrine sentinel; the windows-matrix un-pinned CRLF
  sentinel genuinely absent) — the R2 builder was RESCOPED
  mid-flight to audit-plus-gaps, not re-implementation. Plan
  correction pending both audits: R1.1/R2.1 were audit work, not
  build work; the release-train estimate shrinks accordingly.

  **R2 DELIVERED — 2026-08-25 — and the provenance finding
  hardened: NO FILED REVIEW EXISTS for `9939b02`'s D1 OR D2 share.**
  The commit message self-reports "State review GO"; the dev-docs
  inventory has a review file for every comparable package and none
  for filter-policy/D1/D2 — only secondary liveness checks (M5b
  settled §5 bookkeeping; A1 Completeness item 11 inheriting it).
  The packet-mandated focused State reviews were never filed; the
  R1.2/R2.3 reviews now running are the FIRST FILED REVIEWS of that
  landed production code, scoped to the whole objects. R2's package
  (r2-crlf worktree, 418 lines, ZERO production): the landed-B
  audit (pins content/placement correct; complete NINE-EDGE
  inventory of production worktree writers with per-edge coverage —
  the disable_filters/pins division of labour is by-design, gate
  G2's "filters-off at every edge" phrasing imprecise, restate at
  landing); funnel claim verified (all four clone sites route
  through; two notes: default-trait-impl inheritance for future
  backends, partial un-pinned dir on failed clone); THE THREE GAP
  TESTS (stash round-trip with live CONTROL arm — NEW EVIDENCE:
  the stash_save filtered-reset smudge measured ON MACOS, the
  exposure is non-Windows-only in practice; the creation-time-only
  STRUCTURAL GUARD, red-proven against an injected mid-life pin;
  the un-pinned sentinel test, red CRLF-vs-LF); and the
  `crlf-sentinel` windows-matrix job — TWO HALVES MUST DIFFER
  (pinned lane count-pinned green under hostile GIT_CONFIG_GLOBAL;
  #[ignore] expected-fail sentinel inverted — if it ever passes
  the lane fails loudly as vacuous), §17 topology audit recorded
  in-workflow, REHEARSED end-to-end locally (lane exit 0).
  Baseline moves +2 passed +1 ignored (1585/2i); checker green,
  zero pins. Pre-existing (proven on pristine 8e40fa8, not R2's):
  tests/protocol.rs 2 failures from local taut-proto!=0.8.1 (CI
  installs it). Annotation DELIVERY with a correction: the
  amendment's OPEN DECISION entries were ALREADY closed in-body on
  2026-08-16 — but the HEADER (:11-13) is STALE against its own
  body; four anchor texts (A-D) delivered for lane-owner
  application after R2.3's GO, incl. the tripwire-discharge and
  classification updates. **R2.3 FOCUSED STATE REVIEW DISPATCHED**
  (first filed review of the landed object + the gap package + the
  ten focus items + the four anchor texts; report files as
  `GwzM5-8A1ReleaseR2-ReviewState.md`; its item-7 ruling decides
  whether crlf-sentinel also rides release.yml via R3's G4).

  **R3 DELIVERED — 2026-08-25 (r3-release worktree, 6 files
  +106/−4).** Leg A (G3): generation
  `v0-v1-dual-decode-v0-writer-floor` for v0.11.0, shipped-behaviour
  description (v0+v1 decode; --no-ff→v1; ordinary v0, floor V0,
  M5c named); tuple_count CORRECTLY UNMOVED at 24 (generations add
  no reader rows — reasoning pinned in-comment beside the
  assertion); the brief's "manifest is unpinned" guess was WRONG
  and the verify-first order caught it —
  `evidence-macos-aarch64.json` `inputs.manifest_sha256` re-pinned
  (old 9b1af0e3… → dacdb187…), soundness argued (result set is
  reader-keyed; validate_result_set re-ran green) and flagged to
  review as the one possible rubber stamp. Leg B (G4): topology
  audit found `byte-equivalence`'s FIRST command IS
  check_m4_scenario_map.py — wiring the battery whole would have
  reproduced run 32749489320's sibling failure exactly; wired
  selection `fault byte-equivalence:2 unknown-field privacy`
  (~27 min measured upper-bound), the runner printing PARTIAL for
  the partitioned selector so the gap stays legible; call-graph
  EXCLUDED on verified redundancy (the existing boundary step runs
  its three commands verbatim; markers exit-code-equivalent; ~21
  min for zero signal, re-add trigger recorded); LINUX-ONLY on
  evidence (Windows counts differ by cfg-gating and were NEVER
  MEASURED — an unmeasured count pin is §17's lesson in a new
  dress); blocker comments extended to release.yml naming both
  workspace-root checkers, owner R2-F §11.3 item 7. Leg C (G6):
  version convention ESTABLISHED FROM HISTORY (crate == tag minus
  v, standalone 2-line chore commit; the v0.10.4/v0.10.5 anomaly is
  the narrow-branch release BY DESIGN — v0.10.4 not an ancestor of
  v0.10.5); 0.10.4→0.11.0 applied on the dev line (1+/1− each
  file); member proposal CORRECTS THE PLAN — gwz-cli `0.2.0-dev` /
  gwz-py `0.0.0` are dev placeholders the release-branch commit
  overwrites (version + path→git-tag re-point at the tag, operator
  R5 territory; gwz-py wheel version derives from the tag via
  dynamic versioning, no pyproject edit); full release-notes draft
  delivered with an R1/R2-reconciliation caveat. OUT-OF-PACKAGE
  G6 DEFECT found and FIXED by the lane owner: gwz-cli
  `docs/commands/merge.md:374-377` still claimed --no-ff "not yet
  available"/typed-unsupported and the synopsis omitted it —
  contradicting the shipped headline feature, uncovered by the
  docs checker's 147 assertions (gap recorded); fixed at gwz-cli
  `cf0d16d` (synopsis + accurate --no-ff section), g00 3/0 green.
  **[CORRECTED 2026-08-25 per the R3 review's [P1-1]: "uncovered
  … (gap recorded)" is FALSE. The checker's assertions COVER that
  prose with four required statements
  (merge_command_deferred_heading, …_ff_only_and_message_current,
  …_no_ff_deferred, …_no_ff_unsupported_is_typed) — they were
  PINNING THE STALE PROSE: the assertions should have moved at the
  `3000916` unhide and did not, and L2-05's non-wiring is why the
  red was never seen. The R3 builder's "checker passes so its
  assertions do not cover this" was true only of the pre-fix tree,
  and the lane owner propagated the negative claim into this
  record without verifying it at the pinning surface — the same
  failure class as the C1 correction above, same day. `cf0d16d`
  therefore leaves the cross-repo docs gate RED (4 findings) until
  the four assertions are updated in gwz-core's
  merge_docs_manifest.json — the prose is now TRUE and stays; the
  checker aligns in the R3 landing.]**
  **R3 INTERIOR SINGLE-AXIS REVIEW DISPATCHED** (the 8 focus items
  + the cf0d16d fix verification + the notes' claims checked
  against the tree; report files as `GwzM5-8A1ReleaseR3-Review.md`;
  its evidence-digest ruling is the load-bearing one).

  **R1.2 VERDICT — 2026-08-25: GO WITH CONDITIONS; GATE G1
  SATISFIED at `8e40fa8` + the R1 test package; the §4 forbidden
  ordering CLEARED — v0.11.0 may proceed past G1.** Report
  `GwzM5-8A1ReleaseR1-ReviewState.md` (0 P0/P1; 4 P2 = F-1
  checkpoint falsehood [corrected above, C1 DONE], F-2 the packet's
  fix premise is FALSE — git2 0.21/libgit2-sys 0.18.7 bind no
  git_attr_get_ext/INCLUDE_COMMIT, the amendment's "~15-LOC
  hardening" is unimplementable at this pin [C4], F-3 the
  recovery_support.rs:24-29 comment overstates the harm — the
  reviewer's own probe shows NO in-gwz wedge (libgit2 status runs
  no config-command drivers; realized harm = real-git-visible
  divergence, porcelain re-checkout remedy) [C2-notes; comment fix
  rides R6], F-4 the asymmetry itself; P3s: F-5 the Delta::Deleted
  exclusion is UNTESTED (mutation M2 flips nothing), F-8 plan
  :62-63 phrasing stricter than the packet's boundary).
  Adjudications: (a) **ACCEPTED AS A NAMED RESIDUAL** — already
  found at landing as State F1 [P3], not closable at the current
  dependency pin, mild realized harm, common direction over-refuses
  safely — SHIPS NAMED AND TRIPWIRED: C2 (verbatim residual text
  §3(a) → checkpoint register + G6 notes, by message shape not
  error code), C3 (doctrine sentinel in g12 per the house ritual,
  ~40 test-only lines converting the builder's probe, red means
  someone closed the residual and the frozen texts move), owner
  §R6 renormalize; (b) abort bound CONFIRMED over eight cases,
  guard-tying recommended; (c) FILE_THEN_INDEX intended,
  INCLUDE_HEAD does not exist in this git2; (d) lfs
  allowlist-by-name acceptable-with-record (CI fixture hermeticity
  load-bearing); (e) lock-before-preflight CONFORMS (the packet's
  boundary is set_target, not lock_ref). Reviewer's independent
  mutation matrix: M1 sole-detector confirms the builder's
  sentinel; M3/M4 each detected; M2 exposed F-5. Gates reproduced:
  g12 25/0, lib 1584/0/1, checker ok, zero pins. R1 LANDING
  PACKAGE: the +77 test + C3 sentinel + F-5 test (recommended,
  taken) in the r1-aprime worktree; C2/C4/F-8 dev-docs
  applications at landing (lane owner).

  **R2.3 VERDICT — 2026-08-25: GO WITH CONDITIONS; GATE G2
  SATISFIED at `8e40fa8` + the R2 gap package (0 P0/P1 · 3 P2 ·
  9 P3).** Report `GwzM5-8A1ReleaseR2-ReviewState.md`. The landed
  B code correct AS NEW: pins the right two keys repo-local
  (verified against libgit2 level ordering), STRUCTURALLY unable to
  go mid-life (no_reinit(true) makes re-pinning an existing repo an
  error; RepoBuilder::clone materializes before config can exist —
  disable_filters is the only pre-pin-window mechanism);
  pub(super) holds; the 9939b02 test proven non-vacuous by the
  reviewer's own mutation. FRAMING CORRECTED: the production
  worktree-writer inventory is TWELVE rows, not nine (the stash
  RESTORE edge stash.rs:31-61 was omitted — default
  StashApplyOptions ⇒ filters active, covered by birth pins), and
  "only the clone funnel is filters-off" is false (the two Clause A
  recovery edges are too). [P2-1] MEASURED DEFECT in R2's guard:
  function_slice terminates on literal "\n}\n" and silently
  falls back to whole-file on CRLF working trees — the guard
  degrades EXACTLY ON WINDOWS (proven: a real relocated-pin defect
  fails on LF, passes on CRLF); fix = the in-tree precedent
  r2d_seam_freeze.rs:219-223 (G2-c2). [P2-3] provenance hardened:
  the amendment CITES a review that does not exist (:168 "the
  landing review (F6)"; the :108-121 closure annotation) — the
  citations dangle even though R1.2 traced the review's findings;
  condition on anchor A. Fixture argument STRENGTHENED: libgit2
  ignores GIT_CONFIG_GLOBAL on gwz's open path (repository.c
  use_env gating) — an env-var fixture would have been INERT;
  repo-local hostile is the only correct form. CONDITIONS: G2-c1
  the sentinel lane executes at the RC (pinned half "6 passed",
  un-pinned red — R5 evidence item); G2-c2 the CRLF-normalization
  guard fix; G2-c3 anchors only with the §7 conditions; G2-c4
  restate gate G2's text (its literal "filters-off at every edge"
  would break the porcelain parity the amendment preserved).
  **ITEM-7 RULING: YES [P2-2], routed to R3/G4, before the tag** —
  the CRLF class must ride release.yml; PREFERRED FORM: convert the
  un-pinned sentinel to #[should_panic] (rides every existing lane;
  pages when the residual is closed) leaving only the Windows-leg
  count-pinned exact-name step to add to release.yml's verify job
  (NOT a duplicate windows job). ANCHOR SIGNING: A signed subject
  to A-i (date closures to 9939b02; assert the traceable facts,
  not an unfiled review's existence) + A-ii (repair the dangling
  citations same pass); B unsigned text-unseen — §7 of the report
  carries the restatement the reviewer WOULD sign (use it); C/D
  signed subject to C-i (attribute-driven residual qualifier
  MANDATORY), C-ii (scope to gwz-BORN), C-iii ("proven
  non-vacuously off-Windows" — g11:48 has never run on Windows).
  ROUTED: R2 builder resumed for G2-c2 + the #[should_panic]
  conversion + windows-matrix adjustment + package-level P3s;
  the release.yml Windows count-pin step folds into R3's landing
  package on its review's return.

  **R3 REVIEW VERDICT — 2026-08-25: NO-GO AS FILED ([P1-1] + 3
  P2); G3 GO-with-conditions, G4 GO, G6-versions GO,
  G6-notes/companion NO-GO; with conditions 1-4 closed G3/G4/G6
  are satisfied — none require new code.** Report
  `GwzM5-8A1ReleaseR3-Review.md`. [P1-1] = the lane owner's
  `cf0d16d` broke the green docs gate (corrected in the R3
  delivery record above); cure = four assertion updates in
  gwz-core `merge_docs_manifest.json` + checkpoint correction
  [DONE]. [P2-1] the generation description omits that an
  ordinary v0 record MIGRATES to v1 on resume (whitelisted
  Finalizing+Normal, store/mod.rs:254-262 MayAdapt) — a v1 record
  can exist with NO --no-ff start; add the migration clause.
  [P2-2] the decode-generations register has a HOLE at
  v0.10.3/4/5 — v0.10.4/5 are materially NOT the v0.10.2
  generation (PRODUCTION_R3 {v0:true,v1:false}, typed
  Unsupported+required_wave, not record_unreadable); constrain
  the entry's stated meaning or add the missing generation.
  [P2-3] the notes' "v0.10.2–v0.10.5 → record-unreadable" claim
  is wrong for two of four releases and unevidenced for a third
  (harness pins only v0.9.2/v0.10.2); narrow or split it.
  MECHANICS ALL CONFIRMED: byte-equivalence:1 IS the m4 checker;
  the runner's failed-before-partial ordering verified by
  execution; the version convention holds at all eight tags;
  v0.10.5 lives on origin/hotfix/v0.10.5, NOT an ancestor of
  HEAD, carries NO code main lacks — v0.11.0 STRICTLY SUPERSEDES
  it (the notes should state this). THE EVIDENCE-DIGEST RE-PIN
  RULED SOUND, verified eight ways (reader-keyed result set;
  manifest deliberately not in EVIDENCE_SOURCE_NAMES), with
  [P3-7] narrowing the RULE: artifacts are embedded-not-rederived,
  so "readers didn't change" suffices only because the delta is
  confined to decode_generations. OUT-OF-SCOPE FIND, ROUTED TO
  THE R3 LANDING: **`release.py:527-530` HARD-CODES an AI
  co-author trailer into release commits** — violates the
  standing attribution order (settings enforce it only for the
  lane's own commits, not tooling-authored ones); v0.11.0 dodges
  via the pre-bump no-commit path, later releases would not;
  one-line removal rides R3 with this record. R3 BUILDER RESUMED
  with conditions 1-4 + the R2.3 item-7 release.yml Windows
  count-pin step + the trailer removal.

  **R1 IS LANDED — 2026-08-25, gwz-core `a6ef094` (exact-ref
  pushed, main 8e40fa8 → a6ef094; live tree fast-forwarded
  clean).** One file, +299/−0, test-only (`src/git/tests/g12.rs`):
  the rewrite-set scoping test (M1 sole detector), F-5's
  deletion-clause test (M2 sole detector, built + red-proven this
  round), and C3's doctrine sentinel
  `doctrine_sentinel_target_side_attribute_coverage_escapes_the_foreign_filter_gate`
  (g12:1211; pins TODAY'S behaviour — the rollback proceeds, the
  ref moves, raw bytes under now-live coverage, and
  `!status.is_dirty` pinning the F-3 fact that gwz's status cannot
  see it; 45-line doc comment carries the full ritual incl. the
  red-means-frozen-texts-move clause). Lane-owner verification at
  landing: diff sha matched insurance
  (`6a915aa3…`), checker green, g12 23/0 re-run. Builder gates:
  lib 1586/0/1, clippy from clean. Zero pins. RECORD APPLICATIONS
  LANDED WITH THIS ENTRY: C4 (packet :269-271 bracketed
  correction — get_attr wraps git_attr_get, target-tree attribute
  read unimplementable at this pin; amendment :117-121 correction
  — the "~15-LOC hardening" unachievable as written, F1 accepted
  as named residual with the sentinel + R6 owner), F-8 (plan R1.1
  boundary phrasing aligned to set_target). **C2 — THE RESIDUAL
  REGISTER ENTRY, verbatim per R1.2 §3(a):**

  > **A′ NAMED RESIDUAL — target-side attribute coverage
  > (`.gitattributes` asymmetry).** The Decision 2 A′
  > foreign-filter refusal (`refuse_foreign_filtered_rewrites`,
  > `src/git/gitbackend/recovery_support.rs`:46-95) reads the
  > *pre-checkout* attribute stack (`git2::AttrCheckFlags::default()`
  > = `FILE_THEN_INDEX`). When `.gitattributes` is itself inside
  > the rewrite set and the foreign `filter` coverage exists ONLY
  > on the target side — i.e. the recovery checkout restores the
  > coverage together with the bytes — the gate does not fire: the
  > recovery-grade rollback or abort proceeds, the ref moves, and
  > the covered path is left holding raw blob bytes under a
  > now-active configured clean driver. The reachable harm is
  > divergence visible to real `git` on those paths (gwz's own
  > libgit2-based status cannot see it — it does not run
  > config-command drivers; see F6,
  > `GwzM5-8ExactEvidencePlatformAmendment.md`:167-172); the
  > remedy is a porcelain re-checkout of the affected paths.
  > **[Precision, 2026-08-25, per the R4 review's [P0-1] executed
  > evidence: "porcelain re-checkout" must be read as FORCE
  > re-materialization — delete the affected paths, then
  > `git checkout -- <paths>`. A bare `git checkout --` is
  > silently insufficient: after the filters-off rewrite the
  > index carries the raw file's stat, `git status` prints
  > nothing, and the checkout skips the paths as up-to-date while
  > the divergence persists. The runbook carries the working
  > sequence; the g12 doctrine sentinel's doc comment saying
  > "porcelain re-checkout" gains the same precision in the next
  > gwz-core train.]**
  > Reaching it requires the coverage to arrive WITH the checkout
  > (e.g. rolling back across a merge that deleted the covering
  > `.gitattributes`); the common direction — rolling back a
  > change that ADDED coverage — over-refuses and is safe. **Not
  > closable at the current dependency pin:** reading attributes
  > from an arbitrary tree needs `git_attr_get_ext` /
  > `git_attr_options.attr_commit_id`, which `libgit2-sys
  > 0.18.7+1.9.6` does not bind and `git2 0.21`'s
  > `AttrCheckFlags` does not expose. First recorded as State F1
  > [P3] at landing (`9939b02`, 2026-08-16); re-confirmed
  > empirically and re-accepted at R1
  > (`GwzM5-8A1ReleaseR1-ReviewState.md` §3(a)). Owner: the §R6
  > `gwz repair --renormalize` package, which already shares this
  > predicate and is the natural place to harden it.

  C2's user-facing leg (the release notes, by message shape not
  error code, per F6's mechanism not the stale code comment)
  lands with the notes finalization in R5. G1 IS FULLY DISCHARGED.

  **R4 LAUNCHED — 2026-08-25, on R1's discharge (branch (i),
  docs-only).** The runbook builder drafts the v0.11.0
  wedge-and-refusal runbook in a gwz-cli worktree at `cf0d16d`:
  classes A (A′ refusal, by message shape), B (the named residual,
  porcelain re-checkout), C (adopted-CRLF availability), D (the
  §4.2/§4.4 quarantine + manual surgery made documented), E (LFS
  pointer bytes), F (stop-and-collect-evidence); truthful to
  `a6ef094`/`cf0d16d`, no tooling promised, escape design stays
  DRAFT and un-referenced in user text. Single-axis review next.

  **R3 CONDITIONS 1-6 APPLIED — 2026-08-25 (r3-release worktree,
  now 8 files +194/−19; all gates green incl. docs gate restored
  to "ok (11 sources, 147 assertions)").** [P1-1] closed with the
  COUNT HELD AT 147 — the figure is settled-tree evidence carried
  in 12 dev-docs files, so merge_command restructured to 3
  required + 1 FORBIDDEN (regex verified both directions: misses
  the true prose, CAUGHT the pre-fix stale sentence — the class
  cannot return silently). [P2-1] migration clause added
  (whitelist-eligible Finalizing/Normal v0 rows migrate on
  resume/abort — v1 records without --no-ff, stated). [P2-2]
  closed via option (b) with tag-level re-verification: v0.10.2/3
  carry ZERO record_wire files (no dispatcher at all!); two new
  generations (v0-strict-envelope-typed-unsupported{,-narrow})
  added; evidence digest re-derived AGAIN (→ b35699c4…); tuples
  still 24, L2-04 86. [P2-3] the older-reader table rebuilt with
  per-row evidence tiers (harness-pinned vs source-derived) —
  v0.10.4/5 give the BEST diagnostic of any older release. Item 5:
  the Windows count-pin step added with the count DERIVED from
  source (5 plain + 1 cfg(windows) = 6), FIRST-DISPATCH-EXPECTED,
  source-presence guard closing the libtest zero-match vacuity;
  goes live when R2 lands (skip-safe until). Item 6: the
  release.py Co-Authored-By trailer REMOVED with an in-comment
  rationale; repo swept, no other occurrence. Two notes
  corrections forced by evidence: the refusal is NOT a new error
  code (DirtyMember + message shape, matching R1.2 §2.5) and
  CRLF is NOT Windows-only (R2's macOS measurement). Base moved
  to a6ef094 — zero overlap with R3's 8 files, applies cleanly.
  LANDING ORDER: R2 → R3 (makes the count pin live at R3's
  landing) → R4.

  **R2 IS LANDED — 2026-08-25, gwz-core `bed072a` (exact-ref
  pushed, main a6ef094 → bed072a; live fast-forwarded).** Four
  files +513/−4 (one production touch, comment-only:
  repository_support.rs precedence ordering per [P3-4]): the
  G2-c2 guard fix (LF-normalized include_str! inputs per the
  r2d_seam_freeze precedent, BOTH failure modes loud — the
  relocated-pin mutation detected under both encodings, CRLF
  false-positive control green); the sentinel converted to
  #[should_panic] per item-7 (class-death probe proven: fails
  loudly when the fixture's smudge source is disabled); the
  crlf-sentinel job reshaped (both halves green-when-correct;
  anti-vacuity on the two exact counts); [P3-1] is_test_source
  cfg(test) fallback deleted; [P3-2] guard asserts both pinned
  keys; [P3-5] closed by the conversion; [P3-9] → R2-F backlog.
  Inventory RECONCILED: the reviewer's TWELVE-row table (E1-E13)
  is the record — E13 (stash_apply/pop restore, filters active,
  covered by birth pins, exercised g11:94-115) was missing and
  the "only the funnel is filters-off" prose was wrong. Landing
  verification: diff content byte-identical to insurance (stat
  block appended explains the sha delta); combined-tree overlay
  gates at a6ef094 green (git::tests 142/0, checker ok, clippy
  clean); zero pins. Full lib at package base 1586/0/1 (the
  sentinel leaving #[ignore] moves 2i→1i). **THE ANCHORS ARE
  APPLIED** under the signing conditions: amendment header
  status-correction [A-i], the dangling-citation correction
  [A-ii], the tripwire discharge [C-i..C-iii + should_panic +
  E13 cite], the classification update [same], and gate G2's
  text restated from the reviewer's own signable form [G2-c4].
  **G2-c1's RC obligation, corrected wording: at the RC the
  crlf-sentinel lane must record pinned half `ok. 6 passed; 0
  failed` AND sentinel half `ok. 1 passed; 0 failed` (no failing
  half — the invariant moved into the counts).** G2 IS FULLY
  DISCHARGED up to the RC-execution evidence item. R3's staged
  Windows count-pin step is now LIVE-able (the six names exist on
  main).

  **R3 IS LANDED — 2026-08-25, gwz-core `07e1ac1` (exact-ref
  pushed, main bed072a → 07e1ac1; live fast-forwarded). R1+R2+R3
  ARE ALL ON MAIN.** Eight files +194/−19: the v0.11.0 generation
  with the resume-migration clause + the two tag-verified
  register-hole generations (digest re-derived, tuples hold 24);
  the docs gate re-pinned to post-A1 truth at the held count 147
  with the forbidden-regex regression guard; release.yml's
  Linux-only battery step + the Windows CRLF count-pin step (LIVE
  now that R2's six names are on main; FIRST-DISPATCH-EXPECTED);
  version 0.11.0 per the tag-history convention (--locked green);
  the release.py Co-Authored-By trailer REMOVED. Overlay gates at
  bed072a all green (checker; docs gate against live gwz-cli
  cf0d16d; harness 86/OK; L2-04 24/24; check --locked; fmt;
  YAML). Lock captured to `07e1ac1`. Push CI at the landing sha
  under watch. GATES G3/G4/G6 DISCHARGED per the R3 review's
  close ("with 1-4 closed, G3, G4 and G6 are satisfied for
  v0.11.0"). REMAINING TO THE TAG: R4 (runbook draft in flight →
  single-axis review → gwz-cli landing), the notes FINALIZATION
  (R3's corrected draft + C2's user-facing leg + the runbook
  pointer — reconciliation caveats now resolvable against landed
  surfaces), then R5: member pushes (operator), three-arm matrix
  at the RC with G2-c1's sentinel-lane evidence (pinned `6
  passed` + sentinel `1 passed`) and the T-5 pair regeneration on
  its §11.2 carrier, the release record — and THE TAG (operator).

  **R4 REVIEW VERDICT — 2026-08-25: NO-GO as drafted; six local
  edits convert to GO; the page's structure, what-was-mutated
  discipline, and message-first diagnosis "the best this program
  has produced — ship unchanged" apart from the edits.** Report
  `GwzM5-8A1ReleaseR4-Review.md`. TWO P0s FOUND BY EXECUTION, not
  reasoning: [P0-1] the porcelain-re-checkout remedy (class A
  follow-up + class B's ONLY remedy) FAILS SILENTLY AND CERTIFIES
  ITS OWN SUCCESS — after a filters-off rewrite the index carries
  the raw stat, status prints nothing, `git checkout --` skips as
  up-to-date, and the runbook's "expect no output" verification
  passes on the unrepaired worktree (proven on size-preserving AND
  size-changing filters); the working form is delete-then-checkout
  (force re-materialization). PROPAGATED beyond the page: the
  release notes corrected, the C2 residual register text given a
  bracketed precision note, and the g12 doctrine-sentinel doc
  comment queued for the same precision in the next gwz-core
  train. [P0-2] the class-C paste block DESTROYS a mid-conflict
  member — `git rm --cached -r .` refuses non-zero but the
  `reset --hard` on the next line runs anyway, wiping hand
  resolutions, MERGE_HEAD, and stages; the fence must be in-block
  and fail-stop. P1s: the class-C block ineffective for the
  attribute-driven triggers its own section names; the armed
  intermediate state (clean -fdx becomes whole-tree delete)
  unexplained; "ordinary merges are not affected" FALSE (the
  migration truth again — Normal Finalizing v0 rows migrate on
  abort and reach the comparator). RULINGS: force-abandon silence
  WRONG — one sentence added ("there is deliberately no
  force-abandon; parked IS the end state"); the sidecar-less
  restore SHIPS with two instruments (literal diff vs the
  evidence copy; `ls .gwz/merge/*.yaml` empty-check); the
  `quarantine/` DIRECTORY name ships ([Q7] is open over CLI
  surface only; never name a flag; "park" settled as the verb).
  Confirmed true by the reviewer: class D invisibility, the gate
  sets exact, all message texts byte-exact, the v1_rollback
  bound, indexing ships as-is, g00 3/3 and the docs gate 147
  green WITH the R4 edits. R4 BUILDER RESUMED on the edit list.

  **R4 IS LANDED + THE PRECISION MICRO-TRAIN — 2026-08-25. THE
  v0.11.0 TRAIN'S BUILD PHASES ARE COMPLETE.** R4: gwz-cli
  `6b7e75a` (local; docs/MergeRecovery.md 665 lines/4,121 words +
  4 indexing files, +692 total) — the review's two executed P0s
  fixed and RE-EXECUTED on three fixture shapes (the
  delete-then-checkout remedy with touch-sweep detection since
  status is blind; the class-C block now && -chained and
  guard-led, proven to halt at the refusal even when the human
  skips the guard; the attribute-vs-config router; the
  force-abandon closing sentence; the parked-restore instruments;
  "park" the verb, `quarantine/` the directory); g00 3/3;
  links/anchors validated; landing verification: page sha
  byte-exact vs insurance (166555d9…), diff +27 across the 4
  indexing files. Micro-train gwz-core `a6ce8a8` (exact-ref
  pushed, 07e1ac1 → a6ce8a8): R4 [P2-5] — the recovery guide is
  now a docs-gate SOURCE pinning the recovery-path bound
  paragraph (so M5c cannot silently invert the page); the marker
  moved to the EXECUTED count "ok (12 sources, 155 assertions)"
  (the derived guess of 148 was wrong — a new source inherits the
  7 global forbidden; the gate's own output is the pin source,
  today's lesson applied); the g12 sentinel comment's remedy
  corrected per [P0-1]. Docs gate/checker/fmt/g12 green at the
  landing. Lock captured (core `a6ce8a8`, cli `6b7e75a`, py
  `3d19dcd`). NOTE: the settled "147 assertions" figures in
  historical records remain true OF THEIR TREES; 155 is the
  figure from `a6ce8a8` forward. GATES: G1-G4 + G6 DISCHARGED;
  G2-c1 and G5 are the RC evidence items. **REMAINING = R5
  ONLY:** operator pushes gwz-cli (`6b7e75a`) and gwz-py
  (`3d19dcd`); three-arm matrix at the RC (first Windows
  execution of the CRLF proofs + the count-pin confirmation +
  G2-c1's two-half sentinel evidence); the T-5 pair regeneration
  on its §11.2 carrier; the release record; THE TAG (operator;
  release.yml verify green on both legs at the published tag).
  The notes (`GwzReleaseNotes-v0.11.0.md`) are FINAL as of the
  P0-1 correction, pending only the RC evidence stamp.

  **THE v0.11.0 RC EVIDENCE IS COMPLETE — 2026-08-25. ALL FOUR
  RC-HEAD RUNS GREEN at `a6ce8a8`** (Platform `32802563914`,
  Windows `32802562170`, boundary `32802495545` with the wired
  batteries, retained-readers `32802495544`; push CI green at
  every train commit). **G2-c1 DISCHARGED with both verbatim
  tails** (pinned `ok. 6 passed; 0 failed`; sentinel
  `- should panic ... ok` + `ok. 1 passed; 0 failed`). BOTH
  FIRST-DISPATCH CONFIRMATIONS HELD: the Windows count pin at
  exactly 6 (the cfg derivation confirmed on the runner) and the
  stash proof's FIRST WINDOWS EXECUTION green — the C-iii
  evidence caveat is CLOSED (born-repo stash closure now proven
  ON Windows). G1-G6 ALL DISCHARGED. Plan G5's T-5 clause
  corrected in place (over-eager; T-5 stays on the R2-F carrier).
  Release record filed: `GwzMergeCheckpoint-v0.11.0.md` (RC
  tuple, gate ledger, evidence verbatim, named residuals with
  owners, post-tag resume order; tag slot open). **THE TRAIN IS
  COMPLETE. REMAINING = THE THREE OPERATOR ACTIONS:** push
  gwz-cli `6b7e75a`, push gwz-py `3d19dcd`, tag `v0.11.0` at
  gwz-core `a6ce8a8` with `GwzReleaseNotes-v0.11.0.md` as the
  release body (+ the member release-branch commits per the R3
  proposal); release.yml verify then runs on the published tag.

  **THE TAUT-PROTO 0.9.1 BUMP — 2026-08-25, operator-directed
  ("v0.9.1 was released just now - we should test against that
  first"; option (b) chosen verbatim "b").** TESTED FIRST, all
  read-only: 0.9.1 regenerates gwz-core's three committed outputs
  + both corpora BYTE-IDENTICAL (scratch venv, tautc 0.9.1);
  gwz-py api.py byte-identical; gwz.ir.json additively enriched
  (per-interaction descriptors + unary entry) and green through
  codec load, drift check under the wheel, protocol tests 5/5,
  full suite 330/330. LANDED: gwz-core `8008bf6` (exact-ref
  pushed; 3 workflow pins + publish_workflow.rs tripwire +
  protocol/regen.py generator version; currency harness 29/29
  under a 0.9.1 TAUT_PYTHON — also clearing the pre-existing
  local red; retained-reader wheel refs DELIBERATELY UNMOVED,
  frozen reader bootstraps); gwz-py `6e1d52f` (local; generator
  guard + release-script pin + regenerated IR per the lock-step
  doctrine). FOOTGUN RECORDED, not fixed (operator's tool-design
  call): check_protocol_drift.py prefers the VENDORED ../taut/src
  over the installed wheel — in the dev workspace it compares
  against the taut dev head (987e4d14…, matching neither released
  wheel) and reports drift, while the release flow's temp
  worktree correctly checks the pinned wheel (OK d0c205c8…);
  contradicts regen_protocol.py's own anti-shadowing defense.
  RC RE-ESTABLISHED at `8008bf6`: matrices `32846951548` (win) /
  `32846954270` (plat) dispatched + push CI `32846765849`/
  `32846765913`, under watch; release record RC tables moved;
  lock captured (core `8008bf6`, py `6e1d52f`). The release
  scripts now run against 0.9.1 end-to-end (gwz-py release.py
  installs 0.9.1 in its flow). TRAILER-CHIP NOTE: the operator
  started the spawned "remove release.py trailer" session — that
  work already landed at `07e1ac1`; the session should find a
  clean tree and no-op.

  **THE VENDORED TAUT SNAPPED TO v0.9.1 — 2026-08-25,
  operator-directed ("../taut/src should be snapped at the v0.9.1
  release tag").** `git checkout --detach v0.9.1` → taut at
  `5cd26a1`; the old pin `f008419` proven an ANCESTOR — the member
  was simply BEHIND the release, which is why its IR export
  matched neither released wheel. CONSEQUENCE: the drift-checker
  footgun of the previous entry is DISSOLVED in practice —
  `check_protocol_drift.py` in the live workspace now prints OK
  `d0c205c8…` (vendored == wheel); the local-preference stays as
  designed and is harmless while the snap discipline holds.
  RITUAL: a taut-proto release bump has THREE legs — the pins
  (workflows/tripwire/guards), the regenerated gwz-py IR, and the
  member snap to the same tag. RECORD-KEEPING SLIP, corrected
  here: root commit `5120219` carries this entry's message but
  only the lock capture — the record edits had silently failed on
  a drifted cwd before it; this commit applies them, plus TWO
  omissions found in the same inspection: the four release review
  reports (`GwzM5-8A1ReleaseR{1,2}-ReviewState.md`,
  `GwzM5-8A1ReleaseR{3,4}-Review.md`) had been UNTRACKED since
  filing — cited everywhere, committed nowhere — and the root
  `Cargo.lock` still recorded gwz-core 0.10.4 (the 0.11.0
  coherence refresh rides here). Lesson: `gwz add` of nonexistent
  relative paths + a chained commit produced an Ok/Ok that
  committed something other than the named intent — inspect
  `git show --stat` after any commit whose preceding step
  errored.

  **THE v0.11.0 RC EVIDENCE IS COMPLETE AT `8008bf6` —
  2026-08-25.** Windows `32846951548` GREEN / Platform
  `32846954270` GREEN / boundary `32846765849` GREEN /
  retained-readers `32846765913` GREEN ON RERUN — its windows
  harness leg first failed `test_timeout_kills_descendant_process`
  (assertFalse(marker.exists()) — a descendant-kill race; ubuntu
  leg green; the bump's five-file diff provably does not touch the
  harness; the identical tree passed on rerun) — recorded as an
  environment flake with a hardening chip filed for the operator
  (bounded poll-until-absent instead of the immediate assert).
  ALL GATES G1-G6 STAND DISCHARGED AT THE FINAL RC. THE RELEASE
  IS HANDED TO THE OPERATOR: the three release scripts in order
  (gwz-core, gwz-cli, gwz-py — each `v0.11.0 --push`; core takes
  its already-at-version no-commit path; the member scripts
  reconcile release branches against the pushed mains and mint
  their tags), then the GitHub release from tag v0.11.0 with
  `GwzReleaseNotes-v0.11.0.md` as the body, which fires the
  release.yml verify on both legs.

  **v0.11.0 IS TAGGED — 2026-08-25. The three release scripts run
  by the lane on the operator's verbatim "go ahead and run the
  release scripts": gwz-core `v0.11.0`→`8008bf67b7` (main+tag
  atomic, full gate stack green in the script's standalone
  worktree incl. regen-venv-on-0.9.1 "committed protocol
  artifacts are current"), gwz-cli `v0.11.0`→`dec5e3bd47`
  (release branch reconciled+pushed), gwz-py
  `v0.11.0`→`f53a7c64c6` (lock verified pinning
  git+tag=v0.11.0#8008bf6). Member mains pushed (cli `6b7e75a`,
  py `5f6689a`). TWO EXECUTION INCIDENTS, cured+recorded in the
  release checkpoint: ENOSPC at the core gate (session cargo-cache
  bloat; cleaned 20GB, re-run green) and THE TRUNCATED-INVENTORY
  PIN MISS (maturin develop downgraded the venv to 0.8.1 from
  pyproject's runtime dep — one of four sites a head-truncated
  grep cut from the original inventory; fixed at py `5f6689a`,
  untruncated sweep clean; lesson: pin sweeps run untruncated and
  end with a zero-match residual grep). Release verify dispatched
  at the tag (`32946137285`, under watch). REMAINING FOR THE
  OPERATOR: publish the GitHub release page from tag v0.11.0 with
  `GwzReleaseNotes-v0.11.0.md` as the body. Post-release queue
  per the release checkpoint: R2-E first.

  **v0.11.0 IS PUBLISHED — 2026-08-26.** Three GitHub releases
  created together on the immutable tags (v0.10.5 convention;
  operator waived the redundant pre-publication CI wait — the
  release script's own gates were the full suite at the tag).
  Event machinery fired: cli dist (binaries/installers), py
  Publish (wheels), core event-verify (tag-copy 22.04 leg re-fail
  EXPECTED, recorded; aligned dispatch verify continues as extra
  evidence). Version lock verified before publication: all three
  tags v0.11.0; both member release branches pin
  gwz-core git+tag=v0.11.0. THE RELEASE PLAN IS EXECUTED
  END-TO-END. Remaining watch: the artifact pipelines' completion.
  Post-release queue: R2-E first, per the release checkpoint.

  **THE R2-E PLAN IS DRAFTED — 2026-08-26, on the operator's
  "draft the R2-E plan": `GwzM5-8R2E-Plan.md`.** Twelve-row
  obligation ledger consolidated from the charter sources (the §10
  conversion table, the 38 re-reserved keys — census CORRECTED
  from the checkpoint's unsourced "67" — the BINDING
  BarrierIntentV1::issue obligation, the archive/GC sub-package,
  the review-donated riders O9-O12, the DurableObjectIdentity
  reach question). Phases E0-E7: E0 freezes the object (reach
  traces + the §3.5 semantics amendment + dual #1), E1-E3 install
  the three families (parallel-friendly), E4 executes the
  conversion table row-by-row (incl. recover_or_create's first
  production caller gated by §11.3 and the O3 legacy-writer
  expiry), E5 the archive/GC evidence package, E6 the hardening
  riders, E7 the settled dual + three-platform acceptance + the
  ledger's row-by-row close. Two duals max, interior single-axis,
  gates-not-LOC scheduling. Three operator decisions open
  (parallelism width; the executable-template policy;
  quota-vs-schedule). First action on "go": E0.1's read-only
  reach traces.

  **R2-E IS OPEN — E0.1 COMPLETE, 2026-08-26 (operator "go").**
  The reach traces filed as `GwzM5-8R2E-E01ReachTraces.md`,
  read-only, at main `94da3e5`. (a) **DurableObjectIdentity is NOT
  production-reached in v0.11.0**: every merge mutation acquires
  the checked runtime bootstrap (WorkspaceMutatorLock wraps
  try_acquire_workspace_runtime — locks/dirs/revalidation ONLY, no
  probes), while every identity/rename-domain probe sits in the
  catalog lease behind catalog_mutation_lease() →
  recover_or_create = zero production callers; v1_lifecycle has
  zero non-test checked_artifact references (its checked store is
  its own durable_fs module). The 22.04 verify failure was
  fixture-only exposure, confirmed. **The exposure arrives at
  E4.1** — two bindings minted: the capability-refusal UX is an
  E4.1 HARD PRECONDITION, and the probe blast radius
  (every-mutation vs catalog-consuming-only) is an explicit E0.2
  amendment decision. (b) §11.3's activation gate restated row by
  row — incl. the ORDERING CONSTRAINT made explicit: E2.2
  (BarrierIntentV1::issue observe-or-refuse) strictly precedes any
  E4 row admitting roaming-anchor actions; the authority_name
  weigh is taken in E0.2 or explicitly deferred to its consuming
  E4 step. NEXT: E0.2 drafts the semantics amendment against this
  note; E0.3 dual #1 follows.

  **R2-E E0.2 DRAFTED — 2026-08-26; filed
  `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` (1,392 lines); E0.3
  DUAL #1 DISPATCHED.** All 38 keys with per-key semantics and
  real sites (EXISTS/NEW@ convention); §5 blast-radius DECIDED
  option (ii) — probe only at catalog consumption (option (i)
  would refuse NINE production sites incl. repo creation that
  never touches the catalog); §3 O6 RESOLVED — OBSERVE both facts
  via an owner-minted RoamingAnchorHomeWitnessV1 + one typed
  REFUSE (anchor stranded), the Step-4.3 pattern, E2.2-before-E4
  quoted; §6 the O8 mechanism decided; §7 all five §11.3 rows
  accounted; twelve §4.3 annotation targets. **TWO BLOCKING OPEN
  ROWS — genuine design defects found by the drafting, same
  class (a completion predicate written before the catalog had a
  lifecycle): OPEN-B1 completed_record requires the roaming
  anchor RESIDENT (interior.rs:350) so the anchor cannot roam —
  a crash in the window leaves a catalog no process can retain;
  OPEN-T1 completed_record requires an EMPTY RetiredActions root
  (interior.rs:349) so the FIRST terminal retirement un-completes
  the catalog, contradicting MAX_RETIRED_ACTION_DIRS=64 and the
  occupancy retirement-credit model.** The dual is asked to rule
  them together as one precondition package. Also found: +19-line
  citation drift between R4b-G's contract cites and the current
  contract (the A1-era annotations shifted lines) — E5.2 inherits
  whichever numbers the record blesses; the dual verifies.

  **E0.3 STATE AXIS: NO-GO, ROUND-2 REMEDIABLE — 2026-08-26 (2
  P1 · 6 P2 · 8 P3; report `GwzM5-8R2E-E03-ReviewState.md`).**
  THE AGGREGATE CATCH, AGAIN, AND IT IS THE LANE'S: [P1-2] —
  E0.1(b) restated §11.1's five rows UNDER THE LABEL §11.3; the
  real §11.3 has eight items sharing none of them, and its item 1
  (SettledTuple:800-802) is the A1 COEXISTENCE GATE: no
  production catalog activation until the relocation lands —
  and relocation is PINNED TO R2-F (§11.2:791). E4.1's gate was
  lost in a three-hop handoff; "relocation"/"quarantine" appear
  ZERO times in the plan, E0.1, and the draft. [P1-1]: the
  injection-source file count wrong in two places AND
  self-disagreeing (truth: nine → TEN, one new file at E2; the
  draft says nine→ten as an E1.2 duty and nine→eleven in §8 —
  the freeze's own inventory-defect class recurring inside the
  object). CLEAN/SUSTAINED: the census machine-verified (165;
  38 rows exact order; zero minted/omitted/retired); the §5
  doctrine sustained on the axis's own nine-site enumeration
  (no corpus ruling binds fail-closed at acquisition; option
  (ii) is what makes "runtime bootstrap only" mean something).
  B1/T1 VERIFIED REAL independently — one defect class — with
  the process ruling: the dual rules, THE LANE AUTHORS E0.2b
  (one addendum, both cures: Class-2/C-3 mechanism, candidate
  (i) for B1), round 2 reviews it under the two-round cap;
  fallback = partial GO unblocking E1 only. The +19 drift
  adjudicated: commit `4b9f078`'s 19 insertions; drift is
  +19-below-the-annotation only; the draft's numbers RIGHT;
  stale cites in SIX documents; ruling = content-anchored
  citing, no re-pointing of dated records, one corpus-wide
  drift note naming the commit. REMEDIATION HOLDS for the Code
  axis (running) — merged round per house pattern; the E0.1(b)
  mislabel correction lands with it.

  **E0.3 CODE AXIS: NO-GO, REVISE-AND-REFREEZE — 2026-08-26 (3
  P1 · 8 P2; report `GwzM5-8R2E-E03-ReviewCode.md`). ROUND 1
  CLOSES NO-GO/NO-GO WITH BLIND CONVERGENCE ON THE DEFECTS AND A
  GROUNDED DIVERGENCE ON THE CURE.** Both B1/T1 derivations
  CONFIRMED independently pre-reading — and one level DEEPER:
  the chain terminates in `Ambiguous` (classifier→bootstrap),
  which has NO recovery arm — the failure is "no process can
  EVER RECOVER this catalog", permanent loss. Code P1s: (1) the
  draft's B1 mechanism (i) is UNIMPLEMENTABLE (completed_record
  sees bare ActionDigestV1s, no interiors; BarrierIntentV1
  carries no roaming-anchor identity field) — REFUSED; adopt
  (ii) COPY-NOT-MOVE in the hard-link shape, which touches
  completed_record/retain_file/require_named_file_identity NOT
  AT ALL; (2) DECISION T-B unimplementable (one observation of
  destination_dir reused; a retired-root interior can never
  satisfy completed_record — §8 would freeze the wrong arm-table
  resolution); (3) OPEN-C1 REFUTED at source — RecordScratch has
  ZERO write paths tree-wide (the authority scratch is
  AuthorityScratch): the feared sharing does not exist, C-3
  simplifies. P2 spine: each precondition package is THREE
  gates not one; the amendment's own key #12 destroys the
  catalog permanently under any move-based roam (reinforcing
  copy-not-move); O6 applies precedent parts (i)/(iii) but not
  (ii) — the callee's read side re-checks five facts and none of
  the three identities. VERIFIED CORRECT: the nine sites
  (counted), §5 whole, §6 + the drift catch, C-1, B-3 Windows,
  census-vs-green-fixture; "the drafter's citation discipline is
  the best I've audited here." PACKAGE RULING (diverges from
  State): RULE B1/T1 SEPARATELY — T1 is §4.4-Class-2-sanctioned
  observer-reading widening (three gates, census.retired
  evidence struck); B1's correct fix needs no widening at all.
  **MERGED REMEDIATION ADOPTED: the State axis's PROCESS (lane
  authors E0.2b; round 2 re-verdicts under the two-round cap)
  carrying the Code axis's CURES (B1 copy-not-move; T1
  three-gate Class-2; ruled separately inside the addendum),
  plus State P1-1 (file count 9→10 once, at E2), State P1-2
  (the REAL §11.3: E4.1 gated on the R2-F-pinned relocation —
  the addendum analyzes which E4 rows truly need catalog
  activation and PROPOSES the sequencing options; likely an
  operator plan-shape decision), O6 part-(ii) read-side
  identity checks, the T-B re-resolution, both axes' P2/P3
  lists, and the corpus-wide drift note naming `4b9f078`.**

  **E0.2b DELIVERED — 2026-08-26; filed
  `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` (1,327 lines);
  ROUND 2 DISPATCHED to both axes.** All eight merged-mandate
  items dispositioned. ONE GROUNDED OVERRIDE of a round-1 ruling,
  flagged first for round-2 Code: the hard-link sub-shape of
  copy-not-move is MACOS-FATAL — the tree's own doctrine test
  (anchor/tests.rs:278-324,
  hard_link_identity_sharing_is_what_the_retirement_rows_assume)
  proves per-hard-link ATTR_CMN_OBJPERMANENTID re-homes the FIRST
  link's identity when the second is created; hard-linking the
  home would break completed_record on every subsequent
  observation. B-5 fresh-independent-copy adopted; the essential
  ruling (predicate untouched) survives; #12 no longer
  catalog-fatal. T-B′ = a NEW DestinationRecheckV1 variant in
  CLASS 1 (the freeze's enum criterion — E0.2 had applied the
  struct half); OPEN-C1 closed; O6 gains the read-side identity
  refusal AND a framing correction (the right precedent shape is
  the observe/retain inverse, not AuthorityFactsIssuerV1); counts
  nine→ten-once-at-E2; §11.1/§11.3 fully restated with verified
  boundaries. **THE SEQUENCING ANALYSIS: the catalog-free E4 set
  is EMPTY (every §10 row reaches the catalog via admission
  execution.rs:141 or managed bootstrap :199) — option (c)
  collapses; the A1 coexistence gate blocks ALL of E4. PROPOSAL:
  option (b), pull quarantine/relocation INTO R2-E as its own
  phase (relocation is catalog-free and self-contained; R2-F's
  deletion charter is DOWNSTREAM of it; leaving it cross-lane is
  circular via E4.7). This is a CHARTER CHANGE contradicting
  three standing assignments → the operator's ruling once round
  2 verifies the premise; fallback = option (a) taken explicitly
  with a cross-lane dependency row.** Three new OPEN rows (B7
  native-measure; B8; R1 BLOCKING for whichever lane owns
  relocation); B3 strengthened (catalog-legal leaf names).

  **E0.3 ROUND 2, CODE AXIS: GO CONDITIONAL — 2026-08-26 (1 P2 ·
  4 P3; §Round 2 appended to its report). THE OVERRIDE RATIFIED
  on independent re-derivation** — and found WORSE than the
  addendum stated: macos.rs:77/:88 makes the macOS durable
  identity BE ATTR_CMN_OBJPERMANENTID; the executed doctrine test
  pins per-hard-link allocation with second-link re-homing; even
  the retained-handle loop (completed.rs:204-217, fgetattrlist
  live from the vnode) breaks. The round-1 hard-link sub-shape
  WITHDRAWN; B-5 fresh-independent-copy DOUBLY RATIFIED (the
  addendum's second reason also verified: BarrierIntentV1 binds
  no alias-identity field — the sub-shape was over-stated even
  on Linux/Windows); the essential ruling STANDS (completed_record
  untouched; E2.1 authorized to edit no catalog predicate). All
  four Appendix-C items CONFIRMED (T-B′ Class 1 correct per the
  freeze's own enum terms, checker boundary-clean over all 1,254
  lines; T-C′ exactly one forward; read-side placement provably
  right — protocol/ never imports cap_std; rows #6-#13 no recheck
  arms). The reviewer withdrew one of its own round-1 findings
  (B-6 contamination cost) and downgraded another
  (retired-root bound safe only by two constants both being 64 —
  P3). **[P2-R1] the one condition: a THIRD machine-enforced
  inventory absent from all three convergence obligations —
  CATALOG_PUBLICATION_CALL_COUNTS (E3 moves 2→3, the seventh
  deliberate extension) and PROTECTED_SOURCE_TREE_DIGESTS (trips
  on EVERY converging commit; the reviewer reimplemented
  source_tree_digest and reproduced both pinned digests to
  confirm) — fold into §2.5/§3.6/§4.5 + four P3 clauses.**
  Awaiting State round 2 (the premise attack + census recount);
  on its verdict the [P2-R1] fold lands and the ripened
  sequencing decision goes to the operator.

  **E0.3 ROUND 2, STATE AXIS: CONDITIONAL GO — 2026-08-26 (2 new
  P2 · 4 new P3; §Round 2 appended). GO ON THE SEMANTICS OBJECT
  — both round-1 P1s cured at every hop, all six P2s dispositioned,
  CENSUS CLEAN over the amended pair (165, 38 accounted, zero
  minted; T-B′ mints an enum variant not a fault key, Class 1
  verbatim-correct), §11 restatements LINE-EXACT (no second
  labeling error), T-D survives its deletion — E1-E3 ARE
  UNBLOCKED. NO-GO ONLY on §7.6-§7.8 as operator
  decision-support:** the premise ruled TRUE BUT NOT COMPLETE —
  the axis attacked it four ways, could not break the
  catalog-dependency of any mapped row (re-testing the lock row's
  ordering clause at all five production sites), BUT §7.6 mapped
  EIGHT of the table's NINE rows. **[P2-R1]: the missing row
  (:280, "v1 checked store/root/bundle paths — test-gated until
  A1; no legacy raw writer") is the one whose gate ALREADY FIRED:
  v1_lifecycle's store is a PRODUCTION RAW WRITER post-A1
  (rewrite.rs:6 durable_fs; zero checked_artifact references) —
  catalog-free, currently unmet, absent from plan O1, owned by no
  step.** [P2-R2]: O3 has a catalog-free discharge route the
  table denies — the legacy private parent has EXACTLY ONE
  non-test owner (policy.rs:33-42 → observation.rs:93); relocate
  that one function and O3 is literally true with zero catalog;
  the blocked pair is O1+O2, not O1+O2+O3 — §7.6 understates
  option (b)'s benefit AND §7.8(a) overstates option (a)'s cost.
  Ground 4 HOLDS, strengthened to breakable circularity. BOTH
  ROUND-2 VERDICTS ARE CONDITIONAL-GO CLASS; the conditions are
  the reviewers' own prescriptions = lane-owner folds, not a
  third round (the two-round cap holds). FOLD DISPATCHED to the
  drafter: Code's two-inventory convergence fold + 4 P3; State's
  nine-row map with row :280's NEW OBLIGATION surfaced for
  ownership (plan O13 candidate), the O3 route correction, the
  census-clause verbatim fix, the family-scope cite. THE
  SEQUENCING DECISION REACHES THE OPERATOR ON THE CORRECTED MAP.

  **THE E0 FOLD IS COMPLETE AND VERIFIED — 2026-08-26; the
  addendum re-filed at 1,746 lines (FINAL DRAFT, both round-2
  verdicts named).** All eleven conditions applied and
  lane-verified (the two inventories in all three convergence
  obligations with the arithmetic shown — thirteen publication
  call sites today, E3's the fourteenth and fifth dated
  extension; the NINE-row §7.6 map with row :280 and §7.6.1's
  O13; §7.6.2's O3 correction; O1+O2 as the true blocked pair;
  the seven verbatim record headers with "none retired"; the
  T-D family-scope closure). O13 OWNERSHIP ANALYSIS: split — the
  substantive conversion RIDES E4.2/E4.3 as a scope clause (same
  store, two-steps-one-store drift avoided); the "no legacy raw
  writer" half is a PIN landing at the next opportunity + a
  dated accepted-residual record covering the A1-to-E4.2
  interval ("not silence, which is the status quo"). THE
  SEQUENCING DECISION IS RIPE, on six grounds with the corrected
  accounting: option (b) pull relocation into R2-E (ground 4
  structural and BREAKABLE — the legacy private parent's single
  non-test owner makes relocation a single-owner legacy-only
  edit discharging O3 outright, handing the lane the whole
  catalog-free critical path in one run) vs option (a) explicit
  cross-lane dependency on O1+O2 with O3 re-owned and E4
  rescheduled. PRESENTED TO THE OPERATOR. On the ruling: the E0
  LANDING (freeze application + plan reshape incl. O13 + the
  drift note, one coordinated train), then E1-E3 launch.

  **THE OPERATOR RULED: OPTION (a) — 2026-08-27, one-line reply
  verbatim "a".** Relocation STAYS R2-F's; the addendum's option-(b)
  pull-forward proposal is DECLINED. The §7.8 fallback executes in
  full at `GwzM5-8R2E-Plan.md` §1.1: the CROSS-LANE DEPENDENCY ROW
  (O1+O2 blocked on R2-F's quarantine/relocation package); O3
  recorded DISCHARGED-BY-THAT-PACKAGE-AND-RE-OWNED, not blocked;
  Phase E4 re-scheduled after it (E1-E3 and E5.1/E5.2 NOT gated —
  the addendum §7.6.2's reliefs); the E7 close form set (O1/O2
  close *re-owned with a named carrier* if relocation hasn't landed
  by then, O1's close carrying §10 row :280); O13 MINTED with its
  split ownership; OPEN-R1 ROUTED TO R2-F with the relocation
  package (blocking for that package's owner).

  **THE E0 LANDING TRAIN EXECUTED — 2026-08-27. R2-E E0 IS CLOSED;
  E1-E3 ARE LAUNCHED.** (1) FREEZE APPLICATION:
  `GwzM5-8R2DInterfaceFreeze.md` gained the THREE §3.5 activation
  records (the addendum [P2-4] cure's final headers, verbatim, each
  with a dated filing note naming the amendment pair) and the ELEVEN
  further E0 annotations (E0.2 §8's rows as amended: E7 with the
  T1-widening authorization; E12/E13 C-1; E14 B-3; the E10/E14
  ground gaining the borrow case under B-5; P5's three Windows arms;
  Class 1's last row resolving to T-B′; Class 2 naming T1's widening
  its precedent and B-5 needing NO sanction; the E4-retire clause
  scoped; E22's reclamation DECLINED; the inventory nine→ten-at-E2
  statement; the total-165 restatement) — grep-verified 3 records +
  11 annotations, table rows intact, freeze 1,798 → 1,950 lines.
  (2) PLAN RESHAPE: status ADOPTED; §1.1 E0 amendments block (O13
  with the DATED ACCEPTED-RESIDUAL RECORD covering A1→E4.2/E4.3;
  the cross-lane row with the ruling verbatim; O3/O8/O11/O6/O12
  rows; E6.3 VOID; OPEN-C1 STRUCK); the E4 gate note (SEVEN
  preconditions; the load-bearing "runtime bootstrap only"
  statement; E2.2-before-E4 ordering); E4.2/E4.3 O13 scope clauses;
  §5 items 1/3 exercised. (3) AMENDMENT PAIR: both statuses →
  LANDED (addendum controlling); §7.8 carries the dated RULED line.
  (4) THE O13 RAW-WRITER PIN LANDED — gwz-core `8597d32`
  (checker-only micro-train, authorized by the dual-reviewed
  addendum §7.6.1 "landing now"): `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES`
  pins the complete executed non-test `durable_fs` surface
  (archive.rs, store/archive.rs, store/rewrite.rs — the addendum
  cited one file; the executed sweep found THREE, pinned all),
  set-equality BOTH directions, both probes executed RED in the
  overlay before commit (growth → "gained a raw durable_fs writer";
  stale entry → "must be retired deliberately"), checker green on
  pristine+pin, exact-ref pushed 94da3e5..8597d32. [CI CLOSED
  2026-08-27: Checked-artifact boundary run 33032497893 SUCCESS
  (~39m); Retained merge readers 33032497890 SUCCESS (8m15s) —
  both push legs green at 8597d32.] (5) E1-E3 LAUNCHED IN PARALLEL (three Opus builders,
  isolated gwz-core worktrees e1-cleanup/e2-barrier/e3-terminal,
  briefs binding the amendment pair addendum-controlling, no-push
  no-tags no-trailers; lane owner lands sequentially with pins
  re-executed at each landing) — §5 item 1 exercised per the
  standing recommendation; operator may override. NEXT: builder
  reports → interior reviews (E1.3/E2.3/E3.2) → sequential
  landings; the freeze inventory-addendum "nine"→"ten" edit rides
  E2's landing commit.

  **E1+E3 TRAINS DELIVERED; E3 ROUND 1 NO-GO/ESCALATED; FABLE
  RULINGS ISSUED — 2026-08-27.** E1 (`4a0b01a`, one commit, rebased
  onto 8597d32): eleven cleanup.* sites + [Fault; 11] matrix green
  both variants, checker suite 69/69 OK, no count move; flags: C-2's
  ~250-line fallback tripped at 285 lines and deliberately NOT taken
  (taking it moves FAULT_INJECTION_SOURCES 9→10 at E1 against
  controlling addendum §6.1 — the pair's one internal inconsistency,
  reviewer adjudicates); the 16 KiB alias-retirement bound stated
  (E4 input); Linux marker 418 derived, landing re-measure. E3
  (`b9cb795` precondition + `04c8a66`): T1 widening at exactly three
  gates, T-B′/T-C′/T-D landed, matrix [Fault; 10] green both
  variants. LANE-OWNER CROSS-CHECK EXECUTED AND PASSED: E1's
  completion (classify==Complete: live Missing + retired
  Exact-fingerprint) and E3's terminal precondition (live gone,
  retired resident, per bound worklist row) implement the IDENTICAL
  slot pairing; E3's exactly-one-of-two-homes keys dissolve the
  §2-vs-§4.3 ordering tension E1 flagged; nuance recorded: E3
  residency-level vs E1 fingerprint-level (see F3). E3 REVIEW
  (GwzM5-8R2E-E3-Review.md, filed with this commit): NO-GO, 1 P1 /
  2 P2 / 5 P3. [P1 F1] the widening's observe_slot↔observe
  re-entry is DEPTH-UNBOUNDED — reviewer REPRODUCED stack-overflow
  SIGABRT at nested retired-actions-v1 depth 700 (200 → typed
  refusal), reachable from recover_or_create; contradicts the
  authorization's 'bounded reading'. [P2 F2] the forced
  source-interior arm (train deviation D-1) — FABLE RULING:
  existence forced, shape minimal-acceptable; freeze §4.4 arm table
  GAINS THE ROW as a dated annotation at landing (lane owner's
  edit); second-axis scrutiny rides dual #2 (E7.1) per the
  deferred-escalation design. [P2 F3] §4.3 rows #2/#3 announce
  fingerprint-depth binding the code does not perform (residency
  only; matrix green DEPENDS on fixture's fabricated fingerprints)
  — FABLE RULING: disclosure-integrity class; remediation must
  bind-what-the-row-says (real fingerprints in fixture; classify at
  #3 if right-layered) OR amend the rows' announced semantics as
  dated determinations (T-D's honesty pattern), never keep the gap.
  REMEDIATION ROUND DISPATCHED to the E3 builder (round 2 = final
  under the two-round cap): F1 fix = dedicated single-level bounded
  retired-root reader, no re-entry, plus the nested-chain refusal
  row at would-have-overflowed depth; F3 per ruling; P3s F4-F7 per
  report; F8 (FIVE machine-enforced inventories, not three) is the
  lane owner's records duty. E1 review in flight; E2 builder in
  flight.

  **THE CITATION-DRIFT NOTE, FILED (2026-08-27, per the addendum's
  Appendix A — the lane owner's filing choice is THIS checkpoint,
  the corpus's resolution index; the compatibility contract itself
  is NOT annotated with it, precisely because a ~50-line insertion
  there would re-drift every line cite below `:82` a second time).**
  The note, verbatim:

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

  **THIN-A1 IS DELIVERED — M5's activation gate is closed.** The
  post-A1 queue as chartered: R2-E (67 re-reserved keys [CORRECTED 2026-08-26 at the R2-E plan: the freeze's records and the Completeness bucket (c) say THIRTY-EIGHT — cleanup 11 + barrier 16 + terminal 11; the 67 was a lane-introduced figure with no source]; binding
  obligations incl. [P3-R2-1], [P3-R2-2], the archive/GC consumer
  sub-package, BarrierIntentV1::issue observe-or-refuse), R2-F
  (native evidence, the T-5 pair carrier, C-2's four fixtures, the
  L2-05 multi-repo-checkout cure), **M5c** (minted — the v1
  ordinary-start owner + the floor raise as one reviewed change),
  the escape packages (second lane, BLOCKED ON OPERATOR HANDOFF),
  R3-R6 per RemPlan-4. Operator items open (non-gating): the
  ChangeBudget charging-convention ruling; the executable-template
  policy question; probe-branch cleanup.

  **Windows run 16 (32559979514, at `6c7c8f3`): 1399/4/1.** The
  share-delete fix HELD (os-32 extinct) and all 14 admission tests
  are green. The 4 failures are the namespace fault matrix — its
  first Windows execution — all failing at BASELINE, pre-injection,
  with one typed message: "checked action namespace barrier: private
  durability anchor is not ready" (tests_fault_matrix.rs:376). A
  deterministic Windows arm gap in 2.2's barrier anchor-readiness
  (E9-family territory); diagnosis launched on the clean tree; the
  managed matrix (landed with 2.3) is expected to hit the same wall
  in run 17 — same class, pre-attributed here. Validation runs at
  `8b83a2c` dispatched: 32568481700 (Windows), 32568483432
  (platform).

  **ARM64 attribution correction #2 (L1-16):** the platform run at
  the twin fix proves EBADF EXTINCT (zero os-error-9 in the log; the
  three reverse-preservation EBADF rows cleared), but 46 of the 49
  prior failures were NEVER its cascades: 29 g15::root_preservation
  + 17 v1_lifecycle failures persist with
  `PreservationEvidenceMismatch: "root preservation preparation
  requires the exact durable handoff"` (support.rs:424) — a distinct
  first-Linux-execution class in the preservation-root durable
  handoff — plus one new first-execution test fragility
  (exact_source inode-reuse assumption; tmpfs/ext4 recycles inode
  numbers). ARM now 1374/47. Fable diagnosis package launched;
  non-gating throughout (thin A1).

  **PHASE 1 SETTLED 2026-08-22 at GO/GO — dual #2 of 3 consumed.**
  Round 1: State NO-GO (1 P1 — the 65th-admission catalog-bricking
  capacity gap, a genuine kernel catch; 2 P2, 4 P3) / Code GO (1 P2,
  2 P3), with blind convergence on the E4 retire-edge inventory gap.
  One merged remediation at `bf438ed` (capacity refusal on both the
  new-admission and commit-point paths, typed and test-proven;
  route (ii) on the retire edge — freeze §4.3 activation record, NO
  key mint, 19/165 stands, ratified by the State re-verdict; triad
  refusal restored; all P3s). Round-2 re-verdicts: State GO (zero
  new findings; `can_admit_new` deferral ruled not-a-gap) and Code
  GO (capacity code ruled correct; the remediator's 302/0 vs the
  reviewer's 274/0-anchored count noted as an evidence-recording
  discrepancy only, green both ways). The admission kernel is
  settled; Phase 2.3/2.4 and Phase 3 unblock behind their
  dependencies' reviews. Pushed with the ARM64 train; both matrices
  dispatched at the push.

  **Step 1.3 DELIVERED and PARKED with this record (2026-08-22):**
  all 19 `admission.*` keys given injection sites in the one
  owner-private mutation file (driver holds zero — it decides, never
  mutates); interruption/restart/convergence executed per-key on BOTH
  target variants; 12-round same-boundary crashes at the two genuinely
  re-crossable boundaries prove stable slots (the write-boundary
  non-repeatability finding is recorded in the report for the settle
  reviewers); activation flipped Reserved→Executed with EXPECTED_KEY
  counts HELD at 19/165 — no key minted; `checked_artifact::` 271/0;
  the memo §3.5 row and source-count note updated as the activation
  record. **The Phase 1 package (Steps 1.1-1.3) is complete; the dual
  settle (dual #2 of 3) is launching on it.** Note for reviewers on
  the record: commits 5a7ff0f (1.1+1.2) reached origin early beneath
  a hotfix push (incident + ritual below); the settle judges the
  committed object as published. Driver-lane disclosures on record:
  its one whole-crate `cargo fmt` slip canonicalized whitespace in
  up to five of the D3 lane's in-progress files (no semantic change;
  not reverted to avoid destroying uncommitted work).

  Incident record (L1-16, lane owner, 2026-08-22): the M5b acceptance
  commit `8c1624a` accidentally swept the Phase-1 driver's in-flight
  checker inventory lines (a CATALOG_PUBLICATION_CALL_COUNTS entry
  for a not-yet-committed module) via a whole-file `gwz add`, and its
  v1_lifecycle tree pin had been computed from the dirty shared tree
  — the pushed checker crashed on pristine extraction. Corrected
  within minutes at `558f834` (pristine-worktree corrective commit:
  foreign lines removed, pin recomputed from committed content,
  checker green on pristine). Rituals hardened: shared mutable files
  are staged from pristine worktrees or per-hunk while lanes are in
  flight, and digest pins are computed from the pristine extraction
  of the commit being built, never the dirty tree. The driver lane
  was notified to re-apply its counts entry as part of its own
  package.
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
| gwz-core | `78badbc` = origin/main — **the R4b-G gate tooling train** ("Land the R4b-G gate tooling: privacy probe, call-graph guard, M4 scenario checker, aggregate driver"), sitting one commit over **the R2-D settled object `b91bdeb`** (2026-08-23; the `d32b2c9` row above the earlier correction was the 2026-08-16 pause state, long since landed through the Phase 0→5 trains) — **[row re-pinned 2026-08-24 per the R4b-G Evidence axis [P3-6](i) and the Correctness axis C-7.2: it named `b91bdeb` and was one commit stale; it now tracks the tooling train, which is the R4b-G dual's review object]** | R2-D SETTLED at `b91bdeb`; matrix green runs 18-22 (run 22 = `b91bdeb`). **No native matrix run exists at `78badbc` itself**; `git diff b91bdeb..78badbc` is exactly the five `scripts/checks/` files, **+582/−0, zero `src/` lines**, so run 22's evidence carries to this pin by construction (Correctness axis C-7.4) |
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
  Recorded **NON-GATING for A1** per thin A1. **ACCEPTED 2026-08-22
  at GO/GO** (operator handoff "burn it" lifted the second-lane block
  for remediation): round-1 dual NO-GO/NO-GO (9 P1, 0 P0) → one
  merged round-2 revision at root `20f1654` (all 14 P1/P2 + every P3
  inside the immutable Q1-Q10 envelope; consent-round redesign:
  one round anchored to the shown digest, atomic both-side collapse
  writes, crash ends the round) → round-2 re-verdicts GO/GO with
  zero unresolved and R-1/R-2/R-3 confirmed unconditionally on both
  axes. Acceptance ritual applied §7 to all five contracts as dated
  banners + 22 §-local annotations with exact replacement texts
  (frozen prose byte-preserved — the doc gate pins it); doc gate
  green at baseline (11 sources, 147 assertions). The escape wire is
  now the FOURTH accepted train on the I2 contracts; implementation
  lands as its own reviewed package(s) after R4b-G, at or before A1
  (Q3). Side finding routed: the accepted
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
dependency statement. **[Annotation 2026-08-24 (R4b-G Evidence axis
P2-1's bookkeeping ask; Correctness axis C-7.1): this sentence is
overtaken by events and is retained, not struck, as the text the J-1
adjudication is against. The M5b-IMPL merge (`3e60529`, `8c1624a`)
preceded R4b-G; that sequencing deviation is adjudicated
ACCEPTED-WITH-RECORD in the J-1 record above, ratified by both R4b-G
axes at round 1. What still stands from this sentence is the
**settled review**, not the merge: the M5b-IMPL settled review remains
owed pre-A1 on the tier recorded here.]** Tracked: round-3 N-4 (P3
cross-axis wording reconciliation, next docs pass). D3 amendment:
State re-verdict GO at
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
