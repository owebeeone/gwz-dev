# GWZ M5–8 Progress Review (F5) — quality and architecture of the work to date

Date: 2026-08-11
Reviewer: Claude (Fable 5), independent point-in-time review requested by the maintainer.
Snapshot: outer `gwz-dev` at `9b8d562`; `gwz-core` at `064f06e` plus an actively
changing working tree (see §2.3). Review object: the whole GwzM5-8 program —
design documents, implemented code, and process — not any single document.

Method: five parallel deep reviews (main design doc; R4b design chain; core
v1 lifecycle code; integration/consumer code and uncommitted work; process,
ledger, and review-loop health), cross-checked against each other, plus direct
verification by the lead reviewer: an isolated-worktree test run of committed
HEAD, test runs of the dirty tree at two points in time, `cargo fmt`/`clippy`
checks, ledger claim spot-checks, and file-level confirmation of every
load-bearing citation reported below. Coverage gaps are listed in §10.

---

## 1. Verdict

**The work is of unusually high quality, and the program's own claims about
itself verify under independent spot-checking.** Committed HEAD passes
1034/1034 tests (independently re-run in an isolated worktree), clippy is
clean, the ledger's measured numbers are honest, the review gates have caught
real defects — including one data-loss-class P0 design bug and a false-green
Windows CI run — and the production-safety claim (nothing since M5a is
reachable in a release build) is structurally true and was verified from both
the definition side and the consumer side.

The risks are not correctness risks. They are, in order of concern:

1. **Sustainability/schedule** — the process weight is aerospace-grade for a
   single-maintainer pre-1.0 tool, every package has overrun its initial
   budget by 2–10x, and after ~12 days the only shipped user-visible feature
   is custom merge messages. The most probable failure mode of the program is
   not a bad merge record; it is M6–M8 never shipping.
2. **The A1 activation cliff** — the machinery has concentrated all
   integration risk into one future flip that enables the v1 writer,
   migration, and no-ff dispatch at once over ~25k lines that have never run
   in anger.
3. **Contract-document staleness** — the frozen I2 contracts no longer
   describe the implemented wire format while the normative chain still names
   them wire-authoritative (§6.1). Cheap to fix, dangerous to leave.
4. **Two design gaps worth resolving before A1** — no operator escape from
   permanently unresolvable states, and the non-durable preservation cursor
   (§3.3).

## 2. Verified state snapshot

### 2.1 Program position

Accepted per the ledger and confirmed consistent with git and review-doc
verdicts: R0, R1, R2a, M5a (released), I1/I2, R4a, R3, and R4b checkpoints
TI, TR, S, A, F, X, P0, P0.1, P0.2 including the §14 non-publishing Cargo
Dist platform gate (real, `workflow_dispatch`-only, SHA-bound, verified green
on five targets). P1 is unpaused and in flight right now. P2 (rollback,
~1,941 lines), P3 (status/protocol, ~1,096), P4 (archive/GC, ~508) are
implemented but **not yet independently accepted** — ~3.5k production lines of
review debt ahead of R4b-G's settled-tree double review. Then M5b, then A1 —
the single activation point. M6–M8 have not started.

**No open P0–P3 findings anywhere.** Every one of the ~48 review documents'
chains terminates in GO/PASS; every recorded finding has a recorded closure.

### 2.2 Independent verification results

- Committed HEAD (`064f06e`): **1034 passed / 0 failed / 1 ignored** in an
  isolated worktree; `cargo clippy --lib` clean.
- Ledger spot-checks all pass: `git/gitbackend/contract.rs` = 837 lines
  (claimed <950); post-extraction `gitbackend/preservation.rs` = 455
  (claimed <850); the frozen 23-path owner manifest's files exist at claimed
  sizes; diff `3be3485..064f06e` is consistent with the claimed 2,738 net +
  450 moved; the gwz-py `"record": None` one-line fixture fix exists as
  described; `MergeResponse.record` is field 10 in `protocol/generated.rs`.
- Production gating verified structurally: `#[cfg(test)] mod v1_lifecycle;`
  (`merge/mod.rs:23`) — the typed lifecycle is not compiled into release
  binaries at all; every v1 wrapper elsewhere is individually `cfg(test)`;
  production decoders pin `PRODUCTION_R3 { v0: true, v1: false }` and reject
  v1 files with a typed `required_wave: A1` error. Consumer-side sweep found
  exactly the two admitted production deltas (field-10 projection with an
  all-`None` stub for open v0; stricter canonical acquisition/decode/worklist
  validation on status-by-id and GC) and no other leak.

### 2.3 The working tree is a moving target — and the guardrails visibly work

A concurrent session was landing P1 while this review ran. Observed live:

- ~09:00: 3 modified files (+676); `cargo test` **failed to compile** (E0505
  in a `cfg(test)` fn, `preserve/plan.rs:618`, plus unused-import warnings).
- Mid-review the diff grew to 10 files (+1,896), then consolidated to ~+1,032;
  the preservation observer was refactored while being read.
- `.../observe/reverse/preservation.rs` crossed the repo's 1,000-line cohesion
  trigger (1,059 lines) and was then split, during the review, to 136 lines +
  four sub-modules each ≤522 — the cohesion rule being honored in real time.
- Final rerun: compiles; **1030 passed / 3 failed** — and the three failures
  are the structure-enforcement tests themselves (`reverse_entry` source gate,
  `reverse_router` closed-delegate checks) tripping on the in-flight module
  split. The meta-tests are doing exactly their job mid-refactor.

Normal for WIP; the P1 slice must land as one commit with tests green,
`cargo fmt` applied (currently 8 files are unformatted, including committed
R4b observer files — fmt is evidently not CI-enforced), and without the
orphaned-helper window observed earlier (helpers landed hours before their
consumers).

### 2.4 Scale

| Quantity | Value |
| --- | ---: |
| merge module | ~64k LOC (~26.5k in test-path files; ~41%) |
| v1_lifecycle | ~12.1k impl + ~12.8k test LOC, 166 tests |
| M5-8 dev-docs | 62 files, ~24.9k lines (~13.8k of it review memos) |
| doc:production ratio | ~1.4 doc lines per measured production line |
| comments in new production code | ~20 lines per 11.9k LOC (house style) |
| R4b-P budget trajectory | 1,450 → 4,500 → 4,800 → 8,250 → 14,000 |
| elapsed | ~14 days (proposal 2026-07-28 → today), 47 gwz-core commits |

## 3. Architecture assessment

### 3.1 What is genuinely strong

- **The layered lifecycle model is sound and in places excellent.** The
  intent / typed-durable-action / observation / outcome / acceptance
  separations hold together; the single best decision in the program is
  persisting the complete `AcceptedWorkspace` (exact lock bytes + hash + full
  member audit) *before* the publication-required decision — it converts
  finalization and restart from "recompute from live state" into "apply
  frozen bytes or report typed drift" and contains the M8 policy blast
  radius. The metadata-base vs operation-baseline split (composition
  authority vs rollback authority) is subtle and correct.
- **The versioning doctrine is better-grounded than most.** Compatibility is
  defined as *old-reader behavioral equivalence*, not decode success, and the
  hazard is empirical, not hypothetical (the dormant v0 `NoFf` variant a
  released reader would decode and then silently fast-forward). The ordered
  disjoint envelope classifier, header-only future-version registry,
  no-clamping creation rule, evidence-only archive projection, and
  digest-pinned retained-binary harness are all correct fail-closed
  constructions.
- **The R4b typed-transition architecture is enforceable, not aspirational.**
  Closed 53-variant transition/effect vocabulary with `cfg(test)` variant-count
  tripwires; pure reducers that clone, mutate, restamp, and re-validate the
  whole record per transition; proof tokens whose issuance is sealed by module
  privacy (only `authority::observe` can mint; any rewrite changes the digest
  and invalidates every outstanding token — the anti-TOCTOU spine); and the
  footprint verifier as deliberate double-entry bookkeeping — the store
  re-derives the actual old→new field diff and requires exact set-equality
  with the transition's declared footprint, so a reducer physically cannot
  touch an undeclared field. Trust concentrates in kernel + store, as designed.
- **The reverse lifecycle is a true mirror, not a bolt-on** — same service
  loop, observer/executor traits, proof scheme, and store; asymmetries are
  argued, not accidental. Amendment 2's replacement of the compound
  `NormalizeRoot`/`RestoreRoot` phases with one-durable-phase-per-durability-
  boundary graphs, exact commit-derived `C0`/`C1` forms, and a per-row
  goal-first Before/After/`AfterNeedsDurability`/Ambiguous classifier is a
  real correction of a real physical-atomicity error, cleanly propagated
  through the R4b chain and the code (verified: the old phase spellings are
  gone from `model/v1/journal.rs`).
- **The v0/v1 seam is clean.** Shared-core + thin version wrappers
  (`MergeStatusRecordView::{from_v0,from_v1}`, `V1AcceptanceRecord`), correct
  dependency direction throughout, and reuse of v0 sub-shapes inside the v1
  record (one participant/publication codebase) — the best integration
  decision in the tree. `record_wire/location.rs` deserves singling out:
  TOCTOU-hardened dual-read with inode/volume identity, symlink refusal,
  Windows reparse handling, and fault-injection hooks.
- **The I1 direction memo is the best-written artifact in the set** — closed
  scope table, crisp directional rules, and a self-destruct tripwire ("if I2
  needs a branch action, I1 has failed"). It proves the main doc's content
  could be factored into artifacts of that quality.

### 3.2 The A1 activation cliff

Everything since M5a is production-unreachable *by design*. That makes every
review gate honest — and concentrates all integration risk into the single
future change that flips on the v1 writer, open-v0 migration, and no-ff
dispatch together. Mitigations exist (production-dispatch fixture runs, the
retained-binary harness, A1's own review), but the plan should treat A1 as
the riskiest package of the program, not the smallest: pin an explicit A1
entry checklist now (see §9), including removal of the
`unreachable!("the R3 production decoder does not install v1")` at
`record_wire/decode.rs:86` in the same change that installs v1, and renaming
the `decode_v1_for_r3_tests` / `acquire_for_test` production-path aliases
that will otherwise mislead at activation.

### 3.3 Two design gaps to resolve before A1

- **No operator escape from permanently unresolvable states.** Between-action
  ambiguity in `Preserving`/`RollingBack` is deliberately unrepresentable —
  typed error, no rewrite, no mutation — and `AbortOperation` requires full
  rollback exhaustion while archive requires a terminal record. An externally
  mangled or permanently deleted member repository (not exotic for a
  multi-repo tool) therefore leaves a merge record that can never
  terminalize, with no documented force-abandon, quarantine, or manual-repair
  runbook anywhere in the four R4b documents. "Exact or nothing" is coherent;
  the nothing branch needs an operator story before v1 records become real.
  Related: the universal "typed drift, publish nothing" answer never
  specifies the user's remediation path from a drifted accepted state.
- **The non-durable preservation cursor is the design's weakest joint.**
  Because the wire persists neither no-op owner skips nor a reset-completion
  bit, a pure decoder cannot distinguish a legitimately-later pending owner
  from regressed earlier work; the compensation — mandatory live re-proof of
  the entire cursor prefix before any classify/advance/execute — is correct
  but shifts restart correctness from durable bytes onto repeated live
  observation, and is asymmetric with rollback's fully record-derivable
  cursor. Amendment 1 already proved the frozen wire bends when correctness
  demands (it added the durable handoff field). A few bytes of durable cursor
  would delete this entire live-proof class. Pre-A1 is the last cheap moment.

### 3.4 Main-design concerns (lower urgency)

- The cumulative version ladder (v2 branch ⊂ v3 snapshot ⊂ v4 partial) bakes
  the M6→M7→M8 product order into released registry metadata; if users want
  partial participation before branch lifecycle, the allocation must be
  re-planned against shipped binaries. Defensible choice; unacknowledged cost
  — the §20 risk table should carry the row.
- The same-version additive-change corridor is priced out of use by its own
  proof burden (behavioral equivalence across every retained generation) yet
  its full machinery (§15.5 rewrite preservation, §18.5, much of §15.8's
  recurring matrix) is kept forever. A simpler "record changes bump the
  version" rule would delete a permanent cost with negligible loss.
- The §15.3.3 normative archive table has no row for a *supported* version's
  archive that terminated without persisted acceptance (e.g. v1 aborted
  before the acceptance freeze) — the code handles it; the normative table
  has the hole, and test bullets mandate exercising "every row".
- §4's non-negotiable invariants omit the record-level concurrency /
  single-writer exclusion model entirely; it first appears as mechanism in
  R4b-S. By the doc's own standard it belongs in §4.2.

## 4. Code quality

### 4.1 Strengths

Disciplined narrow visibility (`pub(super)`, `pub(in ...)`) everywhere; no
`unsafe`, no `debug_assert`-only invariants; typed `ModelError` rejections
with exhaustive, wildcard-free matches on closed enums (new variants break
compilation by construction); every `allow(...)` carries a `reason =` tied to
the activation plan; excellent module cohesion (reducers split by family, all
heart files <500 lines); style fully consistent with the older codebase.

### 4.2 Findings

**P1**

1. **Frozen I2 contracts diverge from the implemented wire.**
   `GwzM5-8I2ActionJournalContract.md` still freezes the compound
   `NormalizeRoot`/`RestoreRoot` phase enums and `root_publication_prefix`;
   `GwzM5-8I2RecordContract.md` still says "four" added top-level fields and
   omits `preservation_publication_handoff` from its collision list. The
   implemented wire (`model/v1/journal.rs`, `record.rs:47`) is the amended
   11/10-phase vocabulary plus the fifth field — authorized by Amendment 2,
   but the I2 files self-describe as frozen wire authority,
   `R4bTransitionDesign.md` §1 still points readers at them for wire truth,
   both carry "Amended 2026-08-09" notes proving the annotation mechanism
   exists and was simply not used for the actual wire change, and the
   `check_merge_docs.py` gate covers neither I2 doc nor the retired
   spellings. A future reader who "pins the frozen contract" reconstructs the
   wrong on-disk format. Fix is an afternoon: supersession banners in both I2
   docs, correct the TD §1 authority sentence to "as amended", add
   `normalize_root`/`restore_root`/`root_publication_prefix` as forbidden
   strings in `merge_docs_manifest.json`, and add both I2 docs to the gate.
2. **Land the P1 slice atomically.** The preserve helpers existed for hours
   with zero consumers, the tree compiled red in between snapshots, three
   structure tests currently fail, no dedicated tests yet cover the new
   planner/oracle/executor paths, and 8 files are unformatted. None of this
   should survive into the commit.

**P2**

3. **Panic-style invariants in A1-destined code.** Six
   `let … else { unreachable!() }` plus a chained double-`unwrap` in the
   preservation observer, naked `unwrap()`s in
   `observe/finalization/publication.rs` (459, 616, 681),
   `publication/live.rs`, and `observe/reverse/rollback.rs` (364–495), and a
   `.position(...).unwrap()` in the new `v1_write_bundle_checked` — each
   leaning on an invariant established in a *different* function, in a module
   whose own philosophy elsewhere is typed rejection. Inert today
   (`cfg(test)`), latent production panics the day A1 flips the gate. Audit
   and convert before A1.
4. **Error opacity.** Every reducer `require()` failure collapses to one
   message — "v1 transition predecessor or authority mismatch" — under
   `ErrorCode::MergeRecoveryRequired`, which is also semantically wrong for
   stale proofs and dispatch rejections; ~25 distinct candidate-validation
   failures share one message (`acceptance/publication.rs:334`); archive
   decode discards all underlying detail (`archive/mod.rs:97`). Field
   debugging of a rejected transition will be near-impossible. Even a static
   predicate name per `require` site would fix it.
- 5. **Semantic duplication with drift risk.** Durable-identifier format
  strings (`refs/gwz/merge/{id}/{key}/head`, `stash_{id}`) re-derived by
  `format!` at ~8 non-test sites with no shared constructor; cleanup
  enumeration exists three times (`archive/cleanup.rs` `from_v0`/`from_v1`
  verbatim twins + gc's independent `preflight_backup_artifacts`, while the
  validated worklist stays production-dead until P4); `publication_required`
  has three definitions (state-class v0 vs commit-comparison v1 ×2) with no
  validator enforcing the reconciling invariant; `has_halt_cause` and the
  "successful participant states" list are copy-pasted across 4+ sites.
6. **The v1 preservation oracle is weaker than its own data.** The
  pending-stash acceptance arm ignores the journal's `stash_object_id`,
  `message`, `head_commit`, and `phase`; `ResetAttachedRef` falls through the
  backup-ref matrix as if no action were pending; and v0's pending-action /
  unresolved-index plan-time checks were dropped without any v1 layer
  re-asserting them at preservation time. Tighten while it is still test-only.

**P3 (selection)**

7. Dead scaffolding: throwaway struct constructions solely to silence unused
   imports (`preserve/artifacts.rs:548-557`).
8. `blob_oid` infers SHA-1 vs SHA-256 from commit-string *length* while
   `GitObjectAlgorithmV1` exists as the typed source of truth — exactly the
   stringly inference this refactor exists to kill.
9. One owner, three encodings: `"@root"` (record id) / `"root"` (ref key) /
   `"@publication-root"` (authority binding).
10. Layering inversion: `model/v1/validate/journal.rs:103` calls up into
    `workspace_ops::merge::acceptance` — the model validator depending on the
    operational layer.
11. v1 acceptance drops v0's `BornDetached→BornAttached` live-observation
    upgrade — plausibly deliberate (record authority), but it is a real
    v0/v1 behavioral divergence to re-confirm at promotion.
12. `gc.rs` decodes the same archive bytes twice; unknown-field matrix
    container seeding is a hand-maintained walk with no closure check, so a
    future nested container can silently skip survival testing;
    `participant_field` retirement matching is depth-permissive (weakens the
    tripwire, not the overlay); test-effect corpus is harvested by calling
    other `#[test]` fns through a `thread_local` — clever, deterministic,
    intricate.
13. Comment density is ~0.2% (house style, old and new code alike). With the
    design docs carrying 100% of the explanatory burden, the code is
    unreadable without them — fine while the doc web is maintained, a real
    onboarding/bus-factor cost (§7). The proof-token scheme in particular
    (`token!`, `BoundValue`, sealed issuance) deserves module-level rustdoc.

## 5. Test-suite character

**High signal, deliberately expensive.** The matrix tests are *not*
tautological: they check the real dispatcher/reducers against hand-written
in-file expectation tables, with closed-world enforcement (variant-count
cross-checks force every new transition/effect into coverage). Genuine
semantic negatives abound (wrong recovery-origin proofs rejected at the
checked reducer; observation replay across request types rejected). The
standout is the unknown-field survival matrix: "future" fields seeded into
every container of the raw YAML, exact overlay survival asserted after every
effect variant — a real forward-compat durability proof. Ten v1 test files
drive real git repositories (conflicting merges, exact conflict-abort
rollback, injected-fault TOCTOU races, untracked-file ambiguity), and the
dedicated v0 characterization suites make the seam's "no behavior change"
claim falsifiable rather than asserted. Costs to accept knowingly: ~77
lines/test (setup weight), hand-maintained legality tables, and closure
assertions that convert every schema change into mandatory multi-file test
edits — this suite will resist decay and *also* resist casual change.

## 6. Documentation quality

Normative precision, where it matters, is genuinely excellent (must/may/never
discipline, closed enumerations, the §15.2 classifier, §21 stop conditions).
The problems are structural:

- **`GwzM5-8Refactor.md` (3,468 lines) has drifted from design into
  design + changelog + status ledger + review-response record.** Volatile
  status is smeared across a run-on header block, per-section status lines,
  and multi-paragraph histories embedded inside normative package
  definitions. Amendment scars contradict the text: §8 still demands M8
  policy closure "before the record checkpoint," contradicting §23 and the
  entire I1/I2 strategy (a relic of the abandoned single-checkpoint
  architecture); §1's "two explicit checkpoints" introduces a list of five+;
  §15.2 cites "the thirteen classes named by §15.3.2" which no longer names
  them; §15.3.2's first sentence denies being the classifier its third
  paragraph defines; ghost references to excised "A–M labels" and a missing
  `AV0-A` row; header date 2026-07-30 on content through 2026-08-11 with no
  changelog. Each restatement of a rule (the no-ff rule appears ~65 times) is
  a future divergence site.
- **The append-only correction convention makes currency locally
  undecidable.** `R4bReverseLifecycleInterface.md` §1's freeze declaration is
  negated by §16–17 of the same file; which sentence is normative requires
  topologically sorting status paragraphs across four documents.
- Recommended, in order of leverage: (1) the I2 supersession fix (§4.2 #1);
  (2) extract all status into the ledger and stamp the Refactor doc as a
  stable normative artifact with a changelog; (3) before A1, consolidate the
  four-document R4b chain into one "as-built" document (the doc gate then
  tracks that plus the I2 contracts); (4) fix the §8/§1/§15.2/§15.3.2 stale
  text. The I1 memo shows the target quality per artifact.

## 7. Process assessment

**The machinery is honest and it works.** Verified evidence, not vibes: a
review found a genuine P0 (the P0.1 amendment would have lost the publication
handoff across crash/restart, argued with line-level citations); round 6 of
the R0 harness chain caught GitHub marking a failed Windows CI job successful
(1 failure + 18 errors masked); TR's first implementation was rejected by
both reviewers; the two reviewers genuinely diverged on the §13 settled code
(GO vs NON-GO); the §14 platform gate's insistence on an exact invocation was
vindicated when the first real run failed on absolute artifact paths; and a
mechanical doc-drift gate (87 assertions) polices the narrative. Every ledger
spot-check this review ran came back true.

**Where it fails:**

- **The budget never binds.** Every ceiling that was hit was revised upward
  and ratified (R4b-P: 1,450 → 14,000, ~10x; R0 harness 500 → 7,100). Its
  real function — forcing re-review at each escalation — is useful; as cost
  control it is a one-way ratchet, and "measured under ceiling" mostly
  measures how recently the ceiling was raised. Files hug their limits
  (499/498/493/492/490-line owners against a 500 rule).
- **Reviews validate paperwork, not physics.** The P0.2 root cause — treating
  marker leaf + lock leaf + `.git/index` as one atomic action — was baked
  into I2 at freeze and survived *six* review gates; implementation contact
  exposed it (and the Windows directory-sync no-op, and the missing
  pre-clean parent edge) within days. A "one durable phase per durability
  boundary" checklist item at contract-freeze time would have prevented the
  largest amendment in the chain. Shift review effort from ledger arithmetic
  to physical-model checklists at interface freezes.
- **Mega-commit granularity erases the audit trail.** The entire R4b so far
  is two ~19k-insertion commits; every "independently accepted checkpoint"
  was reviewed as an uncommitted tree whose exact state is now
  unrecoverable. Traceability lives entirely in prose. Committing per
  accepted checkpoint costs nothing and fixes this.
- **"Independent review" is procedural, not organizational** — same voice,
  same frame, same-day turnaround. The role separation demonstrably has
  value (§13 divergence), but nobody in the loop is positioned to challenge
  the frame itself; that remains the maintainer's job (see §8).
- ~25% of the amendment text is budget bookkeeping with no product or safety
  content; replace the hand-maintained line/path ledgers with
  `git diff --stat` tooling.

## 8. Delivery risk

1. **A1 cliff** (§3.2) — schedule the unwrap audit, contract re-freeze, and
   operator-story work *before* it, not as part of it.
2. **Review debt**: P2–P4 acceptance + R4b-G settled-tree review over the
   full ~25k-line tree; every prior settled review spawned a multi-round
   remediation cascade; R4b-G will be large.
3. **Estimation**: every package overran 2–10x; M5b/A1/M6–M8 plans should be
   assumed similarly underestimated. At the current trajectory M6–M8 imply
   another ~100k+ lines of evidence.
4. **Bus factor 1**: a 25k-line interlocking doc web plus comment-free code
   is navigable only by its author and agents mid-flight. The consolidation
   in §6 is the mitigation.
5. **Opportunity cost**: 12 days, one shipped feature. The durability
   engineering is real and the hazard class is real (the dormant-NoFf
   incident) — but the ratio of proof to product is extreme, and the features
   users are waiting for are all on the far side of A1.

## 9. Recommended pre-A1 checklist (ranked)

1. Re-freeze the I2 contracts (supersession banners + TD §1 fix + doc-gate
   coverage + retired-spelling tripwires). — hours of work, closes the worst
   documentation hazard.
2. Land the P1 slice atomically: tests green, new paths tested, fmt applied;
   adopt commit-per-accepted-checkpoint from here on; add `cargo fmt --check`
   to CI.
3. Write the operator escape story for permanently unresolvable states
   (force-abandon/quarantine/manual-surgery runbook) and the
   drifted-acceptance remediation path.
4. Decide the durable preservation cursor question now — last cheap moment.
5. Audit and convert panic-style invariants (`unreachable!`/`unwrap`) on all
   A1-destined paths to typed rejections; tie `decode.rs:86` removal and the
   `*_for_r3_tests`/`*_for_test` renames to the A1 change itself.
6. Disambiguate rejection errors (per-predicate names; a distinct error code
   for authority/predecessor rejection vs genuine recovery).
7. Consolidate the duplicated semantics: ref/stash format-string
   constructors, cleanup `from_v0`/`from_v1`, `publication_required`,
   `has_halt_cause`, success-state predicates; make gc consume the validated
   worklist an explicit P4 acceptance criterion.
8. Doc consolidation per §6; add the §15.3.3 missing row; add the §4
   record-concurrency invariant; fix §8/§1/§15.2 stale text.
9. Before I6: reconsider the same-version corridor (J2) and acknowledge the
   version-ladder reordering cost (J3) in §20's risk table.
10. Process rebalance: physical-model checklists at freeze time; ledger by
    tooling, not by hand; and make the next ceiling revision answer "what do
    we cut" at least once before "what do we raise".

## 10. Method & coverage notes

Read in full: `GwzM5-8Refactor.md`, `I1DirectionMemo`, both I2 record/journal
contracts, `R4bTransitionDesign`, `R4bReverseLifecycleInterface`, both
Interface Amendments, `ChangeBudget`, and the latest review rounds of every
chain (plus sampled earlier rounds). Code: full reads of the transition
kernel, reducers, store, checked/authority layers, acceptance, record_wire,
preserve WIP, and sampled test files; global greps for panic/unsafe/dead-code
patterns. Not conformance-checked (highest-value follow-up targets): the
acceptance-audit invariants (Record contract §3 items 6–12), the preimage
grammar (Journal contract §4), `authority/resolver.rs` and the executor
bodies, and most test-file bodies. All findings above cite the specific
file/section; line numbers into the observer/preserve files may drift as the
in-flight P1 split lands.

Snapshot integrity caveat: the working tree changed at least three times
while this review ran (§2.3); statements about "the WIP" are timestamped to
2026-08-11 and should be re-checked against the P1 commit that eventually
lands.
