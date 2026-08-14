# Agent process rules

Date: 2026-08-15

Status: **documentation-only process handoff, accurate through the R2-C2 merge
checkpoint on 2026-08-15. This file describes the process as practised; it
does not amend a GWZ design, accept an implementation checkpoint, activate a
feature, or change a gate.**

The operator requested this filename in place of the prompt's original
`GwzDevProcessRulebook.md` filename. This file is therefore the sole
deliverable for that prompt.

Amended 2026-08-15: the operator adopted
`dev-docs/GwzProcessOptimization.md` as the controlling process amendment
(two-track freeze, tiered review depth, two-round remediation cap, ledger
tooling, cross-model review, current-state checkpoint, history archiving).
That document controls where it differs from this one.

## Changelog

- 2026-08-15 (2): amended per `AgentProcessRules-ReviewF5.md` (corrected the
  Refactor.md section citations in L1-04/L3-03/L3-04, the L1-18 ledger
  citation, and the L1-17 grounding claim; added §14.1 item 6, §14.2 item 7,
  §14.4 item 9, and the HOLD/NON-GO vestigial note). Recorded operator
  adoption of `GwzProcessOptimization.md`, which controls where it differs
  from this file. Added L1-33 (archive superseded documents). Changes to this
  file itself follow L1-08: bounded amendment, changelog entry, operator
  sign-off. The initial extraction was never separately committed; the
  amended text is this file's first committed state, so the pre-amendment
  baseline is recoverable only from `AgentProcessRules-ReviewF5.md`'s
  description (its consistency review, P3-5).
- 2026-08-15: initial extraction from the M5-M8 program, its predecessor merge
  milestones, review prompts, remediation cycles, change-budget ledger,
  mechanical gates, and v0.10.5 release checkpoint.

## 1. Purpose, scope, and acceptance bar

This is the externalized working ruleset used to run the GWZ merge program.
It separates rules into three layers:

- **L1** is language- and project-independent. The quoted normative text of
  each L1 rule may be copied verbatim into a fresh agent's standing
  instructions. The following provenance and grounding paragraphs are GWZ
  commentary and are not part of that copyable text.
- **L2** binds the process to Rust, Cargo, generated protocol code, CI, and
  native-platform evidence.
- **L3** binds it to this GWZ multi-repository workspace and its actual paths,
  names, commands, and release policy.

The acceptance bar for this document is: a competent engineer or agent with no
prior exposure to this workspace, given this rulebook and its templates, must
be able to execute one complete package cycle—design, freeze, dual review,
implementation, settled-tree review, acceptance, and ledger update—with
artifacts indistinguishable in form and rigor from the existing corpus.

This rulebook uses `MUST`, `MAY`, and `NEVER` normatively. Descriptive text
without one of those words is context, not a new rule.

### 1.1 Provenance labels

- `[given]` means the operator, `AGENTS.md`, or `AGENTS_GWZ.md` stated the rule.
- `[derived]` means the rule is necessary to satisfy a given rule or an
  accepted product invariant.
- `[emergent]` means the agent introduced the convention during execution.
  Each emergent rule names the earliest concrete artifact found during this
  extraction.
- `[unexercised]` means a rule is useful but has no concrete instance in the
  workspace. It remains a proposal, not a claim about current practice.

Every rule below has a trigger, action, verification, and grounding. A
filename citation means the cited file and the named section or status text,
not merely that a similarly named file exists.

### 1.2 Operational vocabulary

| Term | Exact operational meaning |
| --- | --- |
| proposal | A broad recommendation and dependency model. It is not a frozen interface merely because it is detailed. |
| wave | An ordered program stage with its own semantic or release boundary. |
| work package | A bounded unit with one purpose, dependency edges, ownership, budget, tests, and exit gate. |
| checkpoint | A reviewable boundary that freezes an interface, records an implementation state, or separates a releasable line. |
| sub-checkpoint | A named smaller boundary inside a package, used when later lanes need an earlier stable interface. |
| lane | Work that may proceed independently after its shared inputs are frozen. |
| frozen | Implementers may consume the named shape or decision but may not change its meaning. A change requires an amendment and re-review. |
| normative | Authoritative within the document's declared scope. Normative does not imply accepted, implemented, or released. |
| controlling | The specific latest document or amendment that wins where its declared scope differs from older text. |
| accepted | The required reviewers returned the gate's passing verdict against an exact settled tuple and no blocking finding remains. |
| settled | The reviewed repositories are at recorded commits, with intended changes committed and unrelated dirt explicitly identified. |
| implemented | Code exists and its declared local gates pass. It is not automatically accepted or releasable. |
| released | Immutable tags and artifacts were produced from the recorded source and release-platform gates passed. |
| inventory | A list of cases, fault keys, paths, or expected checks. It is not executed evidence. |
| executed evidence | A command, test, fault injection, CI run, or artifact inspection that ran against an identified tree and recorded its result. |

Grounding: the distinctions are exercised by the status and scope clauses in
`dev-docs/GwzM5-8I1DirectionMemo.md`,
`dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md`,
`dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md`, and
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

## 2. Load-bearing rules, ranked

1. **Freeze semantics before mutation.** Without it, code silently chooses
   product and recovery policy.
2. **Persist a typed owner before each external mutation.** Without it, a
   crash creates an unclassifiable state.
3. **Preflight the complete coordinated operation before its first mutation.**
   Without it, a later member failure leaves earlier members changed.
4. **Review an exact settled tuple independently.** Without it, findings and
   evidence refer to moving or different code.
5. **Treat every P0/P1/P2 as blocking unless a gate explicitly says
   otherwise.** Without it, concrete correctness defects become release debt.
6. **Characterize existing behavior and compatibility before refactoring.**
   Without it, a movement-only change can alter released semantics or bytes.
7. **Separate policy owners from orchestration and drivers.** Without it,
   policy becomes duplicated and later recovery paths drift.
8. **Distinguish executed matrices from inventories.** Without it, enumerated
   fault points are mistaken for proof.
9. **Use amendments with explicit precedence; never silently edit history.**
   Without it, reviewers cannot know which decision controls.
10. **Stop when scope, authority, budget, platform evidence, or document
    agreement fails.** Without it, local progress overwhelms program safety.

Grounding: rules 1-10 are repeatedly enforced by
`dev-docs/GwzM5-8Refactor.md` §§7, 19, 21-22,
`dev-docs/GwzM5-8ChangeBudget.md`,
`dev-docs/GwzDevCodeM2aReviewPriompt.md`, and
`gwz-core/dev-docs/GwzMergeDesign.md` §§5-12.

### 2.1 Cosmetic or vestigial conventions

- A reviewer designation such as `F5`, `Code`, `State`, or `FS` is useful for
  traceability but has no safety meaning by itself.
- Title capitalization varies across the corpus and has no operational effect.
- Early verdict phrases such as “ready for M2b” are vestigial; current gates
  use `GO` or `NO-GO` plus severity counts. The Aug-2026 amendment-era
  spellings `HOLD` and `NON-GO` are likewise vestigial and read as `NO-GO`.
- Numeric re-review suffixes are bookkeeping. Independence, exact tuple, and
  closure evidence are the substantive rules.
- Very long status paragraphs are not load-bearing. They have become a drift
  risk and should be replaced by concise status plus a checkpoint reference.

Grounding: compare `dev-docs/GwzDevCodeM2aReviewPriompt.md` with the later
`dev-docs/GwzM5-8R2C2OwnerInterface-ReviewCode-2.md`, and compare the status
blocks in `dev-docs/GwzM5-8ChangeBudget.md` and
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`.

## 3. L1: universal standing instructions

Only the quoted text in each rule is the copyable L1 instruction.

### L1-01 — Load authority before acting

> `[given]` Before any work, read the repository's standing instructions and
> every more-specific instruction file governing the target path. When they
> conflict, stop, quote both rules, and obtain or record a controlling
> decision; never resolve the conflict silently.

Trigger: a new session or a change of target directory. Action: read the
instruction chain from workspace root to target. Verification: the handoff or
review lists the files read and records conflicts. Grounding: root `AGENTS.md`
delegates to `AGENTS_GWZ.md`; `gwz-core/AGENTS.md` adds TDD, dependency, design,
and protocol rules.

### L1-02 — Establish an exact baseline

> `[derived]` Before design or implementation, record the exact repository
> revisions, branch or lineage, worktree state, dependency pins, and relevant
> released behavior. Do not build a plan on an unidentified or dirty baseline.

Trigger: a new program or package. Action: capture revisions and status.
Verification: a baseline table and clean-state assertion exist. Grounding:
`dev-docs/GwzM5-8ChangeBudget.md` “Baseline revisions” and
`dev-docs/GwzM5-8R0Inventory.md`.

### L1-03 — Characterize before changing

> `[derived]` Before changing behavior or structure, characterize the existing
> state machine, mutation inventory, compatibility surface, and observable
> outputs with tests or fixtures. An inventory names what must be proved; it is
> not itself proof.

Trigger: work touches released behavior, durable state, compatibility, or a
large refactor. Action: create a characterization package first. Verification:
fixtures fail if the old behavior or bytes drift. Grounding:
`dev-docs/GwzM5-8R0Inventory.md` and the R0 package in
`dev-docs/GwzM5-8ChangeBudget.md`.

### L1-04 — Decompose by dependency and semantic boundary

> `[emergent]` Organize a program as ordered waves. Within a wave, define
> bounded work packages with explicit prerequisites, owned outputs, forbidden
> scope, and exit gates. Use sub-checkpoints when downstream work needs a
> stable interface before the whole package can close.

Trigger: the design spans more than one independently reviewable concern.
Action: draw the dependency order and assign package IDs. Verification: no
package can start without its prerequisites, and every package has an exit
criterion. First exercised in `gwz-core/dev-docs/GwzMergePlan.md` and made
explicit in `dev-docs/GwzM5-8Refactor.md` §§16-17.

### L1-05 — Freeze lead-owned interfaces before parallel lanes

> `[derived]` Before parallel implementation, the lead must freeze the shared
> vocabulary, owner types, visibility boundaries, call signatures, file
> ownership, compatibility obligations, and integration order. Parallel lanes
> may implement behind that boundary but must not widen or reinterpret it.

Trigger: two or more lanes share types, state, files, or call paths. Action:
complete and review a lead interface checkpoint. Verification: each lane has a
disjoint ownership row and compiles against the frozen interface. Grounding:
`dev-docs/GwzM5-8R1Ownership.md`, `dev-docs/GwzM5-8R2aOwnership.md`, and
`dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md` §1.

### L1-06 — Give each package one coherent owner

> `[emergent]` A package must own one cohesive decision or mechanism. List the
> files it may change, the files it may only call, and the concepts it must not
> absorb. Crossing an ownership boundary requires a lead-approved handoff.

Trigger: package planning or a discovered cross-package edit. Action: record
the ownership manifest or stop for handoff. Verification: the final diff stays
within the manifest or the ledger records an approved correction. First
explicit in `dev-docs/GwzM5-8ChangeBudget.md` package ownership rows.

### L1-07 — Define freeze words precisely

> `[emergent]` Use “normative” only for authoritative text within a declared
> scope; “frozen” only when implementers may no longer choose its meaning;
> “controlling” only when precedence over named earlier text is explicit; and
> “accepted” only after the required independent gate passes on an exact
> settled checkpoint. Never use any of these words as a synonym for
> implemented or released.

Trigger: drafting a design, checkpoint, amendment, or status line. Action: use
the narrowest accurate term. Verification: the document names scope, reviews,
and exact checkpoint where applicable. First made operational in
`dev-docs/GwzM5-8I1DirectionMemo.md` and
`dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md`.

### L1-08 — Amend; do not silently reinterpret

> `[derived]` If implementation or review disproves a frozen decision, stop.
> Write a bounded amendment that names the superseded text, states the new
> controlling rule, explains why, lists affected interfaces and tests, and
> receives the same class of review as the original freeze before work resumes.

Trigger: a frozen contract is unimplementable, unsafe, ambiguous, or
contradicted by evidence. Action: author and review an amendment. Verification:
both old and new documents contain an unambiguous precedence trail. Grounding:
`dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md` opening and §§1-2.

### L1-09 — Re-freeze material semantic changes

> `[derived]` Re-freeze after any change to a wire shape, durable state,
> transition vocabulary, mutation owner, public behavior, compatibility rule,
> platform trust boundary, shared interface, visibility boundary, or package
> ownership. A correction that changes only implementation behind the exact
> accepted contract may use a remediation re-review instead.

Trigger: review or implementation proposes one of those changes. Action:
classify it as amendment or bounded remediation. Verification: the controlling
document and review report agree on the classification. Grounding:
`dev-docs/GwzM5-8I2RecordContract.md` amendment preamble and
`dev-docs/GwzM5-8R4bP1P2-RemPlan-4.md`.

### L1-10 — Budget before implementation

> `[emergent]` Give every package quantitative ceilings for semantic
> production change, moved production code, tests/tools/docs, files, and wire
> or protocol change. Measure moved code separately from new behavior. Stop
> for scope review when any ceiling is exceeded by more than 20 percent, a new
> production owner appears, a declared wire delta changes, or another package's
> files are crossed.

Trigger: a package is frozen or its measured scope changes. Action: create or
update the ledger through a reviewed scope decision. Verification: the package
report compares actuals with each ceiling. First exercised in
`dev-docs/GwzM5-8ChangeBudget.md` introduction and package tables.

### L1-11 — Keep movement and behavior separate

> `[derived]` Do not combine mechanical movement with semantic change when the
> two can be reviewed separately. Land or checkpoint movement first, preserve
> observable behavior, and then change policy through the new owner.

Trigger: a refactor also introduces a feature or bug fix. Action: split the
work into movement and semantic packages or commits. Verification:
characterization passes after movement and the semantic diff is isolated.
Grounding: R1 rules in `dev-docs/GwzM5-8ChangeBudget.md` and
`dev-docs/GwzM5-8Refactor.md` §19.

### L1-12 — Use test-first implementation

> `[given]` For each bounded behavior, write or identify a failing test first,
> implement the smallest contract-compliant change, make the focused test
> green, then refactor without broadening scope.

Trigger: implementation begins. Action: follow red, green, refactor.
Verification: the package record names the pre-change failure or missing
fixture and the post-change passing result. Grounding: `gwz-core/AGENTS.md`
and the TDD sequences in
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`.

### L1-13 — Persist authority before mutation

> `[derived]` Every external mutation must have one typed, durable owner written
> before the mutation starts. The owner must bind exact preconditions, intended
> result, restart observations, retry behavior, and rollback evidence. Never
> infer authority from a mutable live state after restart.

Trigger: a package adds or reroutes a filesystem, repository, database, or
remote mutation. Action: name and persist its owner first. Verification: fault
tests cover not-started, completed-exactly, and ambiguous windows. Grounding:
`dev-docs/GwzM5-8Refactor.md` §7 and
`dev-docs/GwzM5-8R4bTransitionDesign.md`.

### L1-14 — Preflight the whole coordinated operation

> `[derived]` A coordinated operation must validate every selected participant
> and every planned action before mutating the first participant. If any
> preflight fails, all participants remain unchanged.

Trigger: an operation can mutate more than one target. Action: construct and
validate the complete immutable plan. Verification: a last-target failure test
proves the first target is untouched. Grounding:
`gwz-core/dev-docs/GwzMergeDesign.md` §§5, 9-10 and the required review areas
in `dev-docs/GwzDevCodeM2aReviewPriompt.md`.

### L1-15 — Treat recovery states as a closed grammar

> `[derived]` For every durable edge, define the complete set of legal
> observations. Classify anything outside that set as typed ambiguity and
> perform no mutation. Never guess, adopt foreign state, or silently skip
> missing evidence.

Trigger: durable state can be observed after interruption. Action: write a
closed classifier and transition table. Verification: adversarial fixtures
cover every legal state and representative near-misses. Grounding:
`dev-docs/GwzM5-8R4bTransitionDesign.md` §§4-9 and
`dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md` §§3, 8.

### L1-16 — Record evidence, not conclusions

> `[emergent]` Evidence records must identify the exact tree, command or
> scenario, platform, counts, result, and any unexecuted remainder. A list of
> test names, fault keys, or expected cases is inventory and must never be
> reported as an executed matrix.

Trigger: a gate result or review claim is written. Action: record executable
facts and label inventory separately. Verification: a reader can rerun the
command or locate the CI artifact. First explicitly corrected in
`dev-docs/GwzMergeCheckpoint-v0.10.5.md` “Exact R2-C2 blocker” and “Resume
order.”

### L1-17 — Settle before independent review

> `[emergent]` Before a settled-tree review, commit the intended checkpoint,
> record exact repository revisions, and make the tree clean except for
> explicitly authorized report outputs. Do not ask reviewers to infer which
> uncommitted bytes are in scope.

Trigger: implementation claims readiness for acceptance. Action: create an
exact checkpoint and review prompt. Verification: each reviewer independently
rechecks the tuple at start and end. First explicit in
`dev-docs/GwzDevCodeM2aReviewPriompt.md`. Accuracy note (2026-08-15): R4b
checkpoint-era reviews recorded exact tuples but ran on uncommitted trees
that were later squashed into two large commits, so the reviewed
intermediate states are not recoverable from git; commit-before-review
became consistent practice at the P1 freeze (operator instruction in
Appendix A.5).

### L1-18 — Use two independent review axes

> `[emergent]` Critical interfaces, durable-state code, and release boundaries
> require two independent reviews. One reviewer emphasizes architecture,
> interfaces, call graphs, and compatibility; the other emphasizes state,
> filesystem, crash, race, and fault behavior. Reviewers must not read each
> other's current-round report before forming their verdict.

Trigger: a mandatory dual-review point is reached. Action: launch isolated
role prompts against the same tuple. Verification: two standalone reports
exist and describe independent methods. First exercised in
`dev-docs/GwzDevCodeM2aReviewPriompt.md`; mandated per package by the
ledger's review-gate clauses in `dev-docs/GwzM5-8ChangeBudget.md` (for
example the R4b-TI/TR dual-review sentences). Note: this independence is
procedural — historically both reviewers were the same model under different
role prompts. Per the adopted `GwzProcessOptimization.md` §4.3, reviews now
run cross-model (reviewer model ≠ implementer model) whenever two models are
available.

### L1-19 — Use one severity and verdict contract

> `[emergent]` Findings use `P0-n`, `P1-n`, `P2-n`, or `P3-n`. P0 is active
> corruption, data loss, credential exposure, or false composition; P1 is a
> likely destructive or unrecoverable release blocker; P2 is a concrete
> correctness, recovery, compatibility, parity, or diagnosability defect; P3
> is a bounded robustness, coverage, maintainability, or documentation defect
> with a concrete consequence. A gate is `NO-GO` while any P0, P1, or P2 is
> open. P3 blocks only when the checkpoint explicitly requires zero P0-P3 or
> the P3 violates an exit criterion.

Trigger: a reviewer classifies a finding or returns a verdict. Action: apply
the definitions and list counts. Verification: every finding has a reproducible
failure path and every verdict follows the counts. First defined in
`dev-docs/GwzDevCodeM2aReviewPriompt.md`; current `GO`/`NO-GO` form appears in
`dev-docs/GwzM5-8R4bTransitionImplementation-ReviewState.md`.

### L1-20 — Findings must be actionable and singular

> `[emergent]` Each finding must name one root cause, exact location, violated
> invariant, credible reproduction or state sequence, impact, required
> correction, and regression test. Separate independent root causes. Do not
> report style preferences or speculative unease as defects.

Trigger: a reviewer finds a problem. Action: write the complete finding
record. Verification: the remediation plan can map one disposition and one
closure test to the ID. First explicit in
`dev-docs/GwzDevCodeM2aReviewPriompt.md`.

### L1-21 — Close findings through evidence and re-review

> `[emergent]` A finding closes only after the correction, regression test, and
> relevant focused gates pass and an independent re-review verifies the
> original counterexample on the corrected tree. The implementer must not
> self-close a blocking finding. A review attempt with no report and verdict
> does not count.

Trigger: remediation is complete. Action: run a suffixed re-review round.
Verification: the new report names each prior finding as closed or still open
and gives exact evidence. First explicit in
`dev-docs/GwzDevCodeM2a-Rem2ReviewPrompt.md`; the “attempt does not count” rule
is recorded in `dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §1.

### L1-22 — Start a new review round when the object changes materially

> `[derived]` Start a new numbered review round when remediation changes a
> shared interface, architecture, mutation boundary, compatibility rule,
> platform assumption, or previously reviewed call graph, or when a new root
> cause is found. A focused suffix is sufficient only for a truly isolated
> non-semantic correction whose surrounding proof remains valid.

Trigger: a correction lands after a review. Action: classify its review scope
and issue the next prompt. Verification: the report states the prior tuple,
new tuple, reviewed delta, and regression scope. Grounding: the `-2` through
`-11` R4b reviews listed in
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §1.

### L1-23 — Use file size as a cohesion alarm, not a quota

> `[given]` At roughly 1,000 lines, perform a written cohesion review rather
> than an automatic split. When splitting is warranted, create responsibility
> owners below roughly 500 lines. Split smaller files earlier if they become a
> dumping ground for unrelated concepts. Prefer planning those ownership
> boundaries before implementation to avoid churn.

Trigger: a file approaches the threshold or accumulates unrelated policy.
Action: review responsibilities and either document cohesion or split by
owner. Verification: the package design and ledger record the decision.
Grounding: operator instructions reproduced in Appendix A and
`dev-docs/GwzM5-8ChangeBudget.md` “Measured merge baseline.”

### L1-24 — Make documentation part of the compatibility surface

> `[derived]` When a capability, version boundary, state machine, or public
> command changes, update every authoritative design, plan, user guide, machine
> output contract, and release statement in the same package. Mechanically pin
> load-bearing cross-document claims where practical.

Trigger: public or durable semantics change. Action: update the declared
document set and its assertion manifest. Verification: documentation-current
checks pass and no document claims a deferred feature. Grounding:
`gwz-core/scripts/checks/merge_docs_manifest.json` and
`gwz-core/scripts/checks/check_merge_docs.py`.

### L1-25 — Commit reviewable checkpoints

> `[emergent]` Commit at accepted or reviewable package boundaries. Keep
> mechanical movement, semantic changes, remediation, and release-only version
> changes distinguishable when doing so materially improves review or lineage.
> Do not split commits merely because multiple repositories participate in one
> coordinated change.

Trigger: a package reaches a stable review or integration boundary. Action:
commit the coordinated logical unit. Verification: commit messages and diffs
identify the package and preserve exact multi-repository pins. First exercised
in the M1/M2a checkpoint table in
`dev-docs/GwzDevCodeM2aReviewPriompt.md`; narrow release lineage is recorded in
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### L1-26 — Keep tags immutable and supersede explicitly

> `[given]` Never move or replace a published tag to hide a bad release. Mark
> the release superseded, preserve it as a traceability point, correct the
> source, and publish a new version. Release notes must say what supersedes
> what and why.

Trigger: a tagged release is defective or incomplete. Action: retain the tag
and create a corrective release. Verification: old and new releases remain
discoverable and notes are accurate. Grounding:
`dev-docs/GwzMergeCheckpoint-v0.10.5.md` and
`dev-docs/GwzReleaseReview-v0.10.5-F5.md` §§2-3.

### L1-27 — Separate released and development lineages when necessary

> `[emergent]` When advanced work is not releasable, a release may be cut from
> the last accepted narrow checkpoint. Record both lineages and never merge
> them merely to make history look linear. The release must contain only the
> intended delta and the development restart point must remain explicit.

Trigger: current development contains unaccepted or platform-failing work but
an earlier coherent subset is releasable. Action: release the narrow lineage
and write a checkpoint record. Verification: tree comparisons prove both
scope and exclusion. First exercised in
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`; independently reviewed in
`dev-docs/GwzReleaseReview-v0.10.5-F5.md`.

### L1-28 — Reserve operator decisions

> `[emergent]` The operator decides product scope, behavior changes beyond an
> accepted design, trust or platform carve-outs, public compatibility breaks,
> release version and timing, destructive history operations, waivers of a
> blocking gate, and material scope expansion. The agent may inspect, design,
> implement, test, and remediate within already accepted authority.

Trigger: work reaches one of the reserved decisions. Action: stop and present
the exact evidence and alternatives. Verification: the operator's decision is
quoted or embodied in a controlling artifact before proceeding. First visible
through operator-directed phase splits in
`gwz-core/dev-docs/GwzMergePlan.md` and the release boundary in
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### L1-29 — Stop on missing authority or failed proof

> `[derived]` Stop when documents conflict, a required invariant is undecided,
> a blocking review finding remains, a budget or ownership trigger fires, the
> tree contains overlapping unexplained changes, generated artifacts are
> stale, a required platform or compatibility gate fails, or restart behavior
> would require guessing. Do not patch around an architectural stop condition.

Trigger: any listed condition is observed. Action: preserve the state, record
the blocker, and design or request the missing authority. Verification: no
further scoped mutation occurs until the blocker is resolved. Grounding:
`dev-docs/GwzM5-8Refactor.md` §21 and
`dev-docs/GwzMergeCheckpoint-v0.10.5.md` “Exact R2-C2 blocker.”

### L1-30 — Work usefully while blocked

> `[derived]` When blocked, exhaust safe read-only checks, isolate the minimal
> failing state, record exact commands and evidence, identify what new
> authority or external change is required, and ask one concise question. Do
> not declare a blocker merely because work is difficult or slow.

Trigger: an external dependency, decision, platform, or authority prevents
completion. Action: narrow and document the blocker. Verification: the next
session can resume from the checkpoint without repeating discovery. Grounding:
the Windows backlog and resume order in
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### L1-31 — Preserve context in artifacts, not memory

> `[emergent]` Long work must leave a restartable paper trail: current scope,
> exact tuple, controlling documents, open findings, last executed evidence,
> and next ordered actions. A new session must be able to resume without
> relying on private conversational memory.

Trigger: a checkpoint, context boundary, handoff, interruption, or release.
Action: update a checkpoint, ledger, or remediation status without claiming
more than is proved. Verification: cold-start reading reconstructs current
state. First exercised in the status appendices of
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`; the cleaner model is
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### L1-32 — Activation is a separate reviewed change

> `[derived]` A disabled implementation must remain unreachable until every
> producer and consumer, compatibility matrix, recovery path, and platform gate
> for the state it can create is accepted. Enable the writer or dispatcher in
> one small, separately reviewed activation change.

Trigger: a new durable format or semantic version is implemented behind a
gate. Action: keep it disabled, complete its consumers, then review activation.
Verification: old binaries reject it safely and the new binary can resume
every state it writes. Grounding: A1 rules in
`dev-docs/GwzM5-8I2CompatibilityContract.md` §§1-2 and
`dev-docs/GwzM5-8Refactor.md` §22.4.

### L1-33 — Archive superseded documents

> `[given]` Keep the live documentation directory limited to documents that
> currently carry weight. Move a document to `dev-docs/history/` only when it
> is fully superseded and nothing current cites it as controlling: never a
> controlling document, the latest review round of any object, a document
> named by a checker manifest, or one cited by the standing rulebook. Move
> with `git mv` so lineage is preserved, and keep a README in the history
> directory stating that nothing in it is normative.

Trigger: a checkpoint closes, or superseded rounds accumulate. Action: build
the candidate list, exclude anything cited by current documents or manifests,
move the remainder. Verification: documentation gates still pass and no
current document's citation dangles. Origin: operator instruction,
2026-08-15, at the process-adoption handoff.

## 4. L2: toolchain and platform bindings

### L2-01 — Rust quality gate

`[given]` Trigger: a Rust package or integration checkpoint is ready. Action:
run focused tests during development, then `cargo fmt --all -- --check`, the
appropriate `cargo test` suites, and Clippy across all relevant targets and
features with warnings denied. Verification: record exact commands and counts;
unavailable tools are explicit gaps, not silent skips. Grounding:
`gwz-core/AGENTS.md`, `gwz-core/dev-docs/GwzMergePlan.md` §15, and the command
records in `dev-docs/GwzM5-8R4bTransitionImplementation-ReviewState.md`.

### L2-02 — Generated protocol authority

`[given]` Trigger: protocol schema or projected output changes. Action: edit
the Taut source, regenerate Rust and Python bindings, and never hand-edit the
generated files or create a shadow protocol. Verification:
`python protocol/regen.py --check` and cross-driver fixtures pass. Grounding:
`gwz-core/AGENTS.md`, `dev-docs/GwzM5-8I2ProtocolContract.md`, and
`gwz-core/protocol/regen.py`.

### L2-03 — Cross-driver parity

`[derived]` Trigger: a response, error, record projection, or CLI feature
changes. Action: update Rust CLI, Python/native bridge, JSON, JSONL, and human
rendering from the same core semantics. Verification: canonical error-bearing
and success fixtures compare complete shapes across drivers. Grounding:
`dev-docs/GwzM5-8I2ProtocolContract.md` and the remediation history in
`dev-docs/GwzDevCodeM2a-RemPlan.md`.

### L2-04 — Retained-reader compatibility

`[derived]` Trigger: a durable record becomes writable or an existing record is
rewritten. Action: run checksum-pinned older binaries in isolated runtimes
against the exact fixture matrix; do not substitute current code for an old
reader. Verification: supported readers either preserve semantics or reject
before mutation, and evidence identifies the binary, platform, fixture, and
result. Grounding: `gwz-core/scripts/retained_readers/manifest.json`, its
schema and harness, and R0 in `dev-docs/GwzM5-8ChangeBudget.md`.

### L2-05 — Document consistency manifest

`[emergent]` Trigger: a merge wave changes active capability or version
language. Action: update the reviewed assertion manifest with required and
forbidden statements. Verification:
`python gwz-core/scripts/checks/check_merge_docs.py` reports all sources and
assertions green. First implemented in
`gwz-core/scripts/checks/merge_docs_manifest.json` and
`gwz-core/scripts/checks/check_merge_docs.py`.

### L2-06 — Structural boundary gates are defense in depth

`[emergent]` Trigger: safety depends on source ownership, sealed call paths, or
forbidden writers. Action: combine compiler-enforced visibility/sealing with a
positive source/call-graph manifest and adversarial checker tests. Never rely
only on a denylist regex. Verification: alias, re-export, fully-qualified,
module-loader, include, callback, commented-marker, and new-file probes fail.
First evolved through
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py` and the review
sequence catalogued in
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §1.

### L2-07 — Source loading is part of the trusted graph

`[derived]` Trigger: a source-tree digest or sealed module is a safety gate.
Action: bind the compiler route from package target through every parent module
to the protected tree, reject alternate `#[path]` and `include!` routes, and
cover every descendant byte. Verification: root-redirection and out-of-tree
helper probes fail under both checker and compiler. Grounding: eleventh-review
closure in `dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §1.

### L2-08 — Filesystem names are native data

`[derived]` Trigger: code enumerates or compares paths on Unix or Windows.
Action: preserve native bytes or units, charge explicit bounds, and prove the
platform's lookup-equivalence semantics; never skip non-UTF-8 or non-ASCII
names merely because they cannot be represented as a string. Verification:
non-UTF-8 Unix and non-ASCII/case-fold Windows/macOS probes pass or reject
fail-closed. Grounding:
`dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md` §3 and the C1 reviews.

### L2-09 — Native durability is platform-specific

`[derived]` Trigger: correctness depends on rename, sync, identity, advisory
locking, or case behavior. Action: specify and test the admitted primitive per
supported platform; no-op platform branches are not evidence. Verification:
native fault tests and release workflows pass on each supported platform.
Grounding: `dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md` §4.1 and
`dev-docs/GwzMergeCheckpoint-v0.10.5.md` platform evidence.

### L2-10 — POSIX namespace trust boundary

`[derived]` Trigger: Linux or macOS lacks an admitted atomic
compare-open-object-and-rename primitive. Action: retain the checked handle
across a relative no-replace rename, keep the parent lease and collision proof,
and state the cooperating-GWZ/same-user namespace assumption explicitly.
Never describe that sequence as an atomic compare-and-rename. Verification:
substitution tests cover the admitted boundary and docs preserve the caveat.
Grounding: `dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md` §4.1 and §8.13.

### L2-11 — Windows exact-handle publication

`[derived]` Trigger: a checked source object is published on Windows. Action:
rename the exact identity-checked handle with the admitted Win32 API and
write-through semantics. Verification: path substitution after handle open
cannot change the object moved. Grounding:
`dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md` §4.1.

### L2-12 — CI workflows are executable release contracts

`[emergent]` Trigger: a mandatory local or release gate is declared. Action:
wire it into pull-request/push and release workflows at the exact checked-out
revision; prevent release-script flags or branches from bypassing it.
Verification: workflow-text tests plus mocked release-flow tests exercise every
tag/version branch. First hardened in
`gwz-core/.github/workflows/checked-artifact-boundary.yml`,
`gwz-core/.github/workflows/release.yml`, and
`gwz-core/scripts/checks/test_release_boundary.py`.

### L2-13 — Platform release evidence

`[given]` Trigger: a supported-platform release is proposed. Action: build and
test the released source on Windows, macOS, Linux x86-64, and Linux ARM64 as
applicable to the deliverable. Verification: successful workflow run IDs and
artifact checksums are recorded. Release builds count as platform evidence;
inventory or local compilation alone does not. Grounding: operator instruction
in Appendix A and `dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### L2-14 — Fault matrices execute each edge

`[derived]` Trigger: a package declares interruption keys or durable edges.
Action: inject a stop at every key, restart in a fresh process or equivalent
authority context, and prove convergence or typed read-only ambiguity.
Verification: results are per-key, not a string-inventory assertion. Grounding:
the R2-C2 gap recorded in `dev-docs/GwzMergeCheckpoint-v0.10.5.md` and exit
test 9 in `dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md`.

### L2-15 — Hidden allocation and panic paths count

`[emergent]` Trigger: a bounded/fallible API claims no abort-on-allocation or
panic behavior. Action: audit library calls and transitive helpers, not only
explicit allocations or `unwrap`. Verification: maximum-bound and allocation
failure tests plus source/compiler guards cover hidden scratch allocation.
First exposed by `sort_by` in
`dev-docs/GwzM5-8R2C0Interface-ReviewCode-2.md` and closed in
`dev-docs/GwzM5-8R2C0Interface-ReviewCode-3.md`.

## 5. L3: GWZ workspace bindings

### L3-01 — Use the installed GWZ coordinator

`[given]` Trigger: workspace-wide status, staging, commit, or push is needed.
Action: use `/Users/owebeeone/.cargo/bin/gwz` and the commands `gwz status`,
`gwz add`, `gwz commit`, and `gwz push`. Never substitute per-repository Git
loops. Verification: `gwz status --json` reports every member and lock match.
Grounding: `AGENTS_GWZ.md`.

### L3-02 — Read GWZ authority in order

`[derived]` Trigger: a fresh merge-program session starts. Action: read, in
order: `AGENTS.md`, `AGENTS_GWZ.md`, target-repository `AGENTS.md`,
`gwz-core/dev-docs/GWZRequirements.md`,
`gwz-core/dev-docs/GWZDesign.md`,
`gwz-core/dev-docs/GwzMergeDesign.md`,
`gwz-core/dev-docs/GwzMergePlan.md`,
`dev-docs/GwzM5-8Refactor.md`, the package ledger row, the latest controlling
checkpoint/amendment, and the latest two review rounds. Verification: the
session can state current package, blocker, exact tuple, and next gate before
editing. Grounding: source-of-truth order in
`dev-docs/GwzDevCodeM2aReviewPriompt.md` plus current R2-C checkpoint records.

### L3-03 — Package identifier grammar

`[emergent]` Trigger: a new GWZ merge package is added. Action: use `R` for a
refactor/recovery infrastructure checkpoint, `I` for a design/interface freeze,
`M` for feature implementation, and `A` for writer/dispatcher activation.
Preserve program order in the number. Use `-A`, `-B`, `-C` for parallel or
ordered slices, `-I0` for a lead interface checkpoint, and numeric suffixes
such as `C0`-`C3` for sub-checkpoints. Verification: the dependency table
defines every ID before work begins. Grounding: package tables in
`gwz-core/dev-docs/GwzMergePlan.md` and
`dev-docs/GwzM5-8Refactor.md` §17. Note: `R` has been used for both refactor and
recovery-oriented packages; the name alone is not semantic authority.

### L3-04 — Current major-wave map

`[emergent]` Trigger: sequencing M5-M8 work. Action: preserve this dependency
shape unless amended: R0 characterization; R1 ownership; R2a v0 integration
seam; M5a custom messages; I1/I2 v1 direction/contracts; R4a acceptance
extraction; disabled R3 record/adapters; R4b consumer/finalizer conversion;
disabled M5b no-ff; A1 v1 activation; then I6/M6/A2 branch lifecycle,
I7/M7/A3 snapshot source, and I8/M8/A4 partial composition. Verification: no
later writer activates before its I/M/A gates. Grounding:
`dev-docs/GwzM5-8Refactor.md` §§16-17 and
`gwz-core/dev-docs/GwzMergePlan.md` §§13, 18-19.

### L3-05 — Merge policy lives in core

`[given]` Trigger: a merge decision or protocol shape is added. Action: keep
policy in `gwz-core`; CLI and Python drivers parse requests and render core
results without independently deciding lifecycle policy. Keep core independent
of CLI. Verification: parity tests and call-graph review find no driver-only
policy. Grounding: `gwz-core/AGENTS.md`,
`gwz-core/dev-docs/GwzMergePlan.md` §§14.2, 14.5.

### L3-06 — Taut is the sole protocol source

`[given]` Trigger: a GWZ request, response, event, or error field changes.
Action: change Taut schema and regenerate all bindings; do not create a Rust- or
Python-only wire type. Verification: protocol generation and Rust/Python
parity pass. Grounding: `gwz-core/AGENTS.md` and
`dev-docs/GwzM5-8I2ProtocolContract.md`.

### L3-07 — Merge-document gate follows active capability

`[emergent]` Trigger: any M5a-A4 capability gate changes. Action: update
`gwz-core/scripts/checks/merge_docs_manifest.json` and all named sources in the
same reviewed package. Verification: `check_merge_docs.py` reports the expected
source and assertion counts. Grounding: manifest description and the R0 gate
in `gwz-core/dev-docs/GwzMergePlan.md` §15.

### L3-08 — Record versions are semantic floors

`[derived]` Trigger: a new merge operation is created. Action: core selects
`max(active_writer_floor, highest_requested_semantic_version)` from immutable
typed intent; drivers never choose the record version. Existing v1+ records do
not auto-upgrade, archived records never migrate, and v0 migration follows
only the frozen whitelist. Verification: creation and retained-reader matrices
cover v0-v4. Grounding:
`dev-docs/GwzM5-8I2CompatibilityContract.md` §§1-5.

### L3-09 — v1 remains disabled until A1

`[derived]` Trigger: test-gated v1 code exists. Action: keep production writer,
migration dispatch, and no-ff start unreachable through R3, R4b, and M5b;
activate together only in A1. Verification: production dispatch tests prove no
v1 creation before A1 and the activation diff is small and reviewed.
Grounding: `dev-docs/GwzM5-8Refactor.md` §22.4 and
`gwz-core/dev-docs/GwzMergePlan.md` §19.

### L3-10 — Multi-repository version and pin coordination

`[derived]` Trigger: a GWZ release spans core, CLI, and Python. Action: release
compatible versions across all required modules, update CLI/Python core pins
and lock resolution to exact tagged commits, and commit the workspace lock from
observed state. Verification: checkpoint tables show each tag commit and the
installed artifacts resolve the intended core. Grounding:
`dev-docs/GwzMergeCheckpoint-v0.10.5.md` “Released line” and
`dev-docs/GwzReleaseReview-v0.10.5-F5.md` §3.

### L3-11 — Python is release-required

`[given]` Trigger: a coordinated GWZ release is proposed. Action: include
`gwz-py`; build current native bindings in a real virtual environment, run its
tests, install the wheel into a clean environment, and run an end-to-end smoke
test. Verification: the Python publish run and PyPI artifact succeed on the
supported platform matrix. Grounding: operator instruction in Appendix A and
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### L3-12 — Preflight tags are disposable; releases are immutable

`[emergent]` Trigger: release CI cannot prove the exact prospective tag tree
without a tag ref. Action: create a clearly named disposable
`vX.Y.Z-preflight.*` tag at the exact candidate, run all release workflows,
delete the preflight tag after evidence is captured, then create the immutable
release tag only from the proven tree. Verification: run IDs point to the
expected SHA and no preflight tags remain after success. First exercised in
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### L3-13 — Release checkpoints separate scope from backlog

`[emergent]` Trigger: a release is cut while later work remains active. Action:
write `dev-docs/GwzMergeCheckpoint-v<version>.md` with released lineage,
included/excluded features, exact local and CI evidence, immutable supersession,
development tuple, exact blocker, backlog, and resume order. Verification: an
independent release review spot-checks each claim. First exercised in
`dev-docs/GwzMergeCheckpoint-v0.10.5.md` and
`dev-docs/GwzReleaseReview-v0.10.5-F5.md`.

### L3-14 — GWZ release-platform set

`[given]` Trigger: binaries or wheels are released. Action: cover Windows
x86-64, macOS x86-64/ARM64, Linux x86-64, and Linux ARM64; record any
component-specific exception explicitly before release. Verification:
checksummed artifacts and workflow run IDs exist for each promised target.
Grounding: Appendix A and `dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### L3-15 — Current merge restart point

`[emergent]` Trigger: resuming the development line after this handoff. Action:
start from `dev-docs/GwzMergeCheckpoint-v0.10.5.md`, then read the current
`dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md`,
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`, and latest R2-C2 review.
Do not assume the release checkpoint tuple is the current development tuple;
verify with `gwz status --json`. Verification: current core and root commits,
open C2 findings, and platform evidence are restated before edits. Grounding:
those three documents and `AGENTS_GWZ.md`.

## 6. Program structure and package lifecycle

### 6.1 Structural hierarchy

The process uses this hierarchy:

```text
program
  wave (semantic/release boundary)
    package (bounded owner, budget, exit gate)
      lead checkpoint or I0 (shared interface)
      lane A / lane B / lane C (parallel only after freeze)
      consumer or integration checkpoint
      activation package (separate when durable/public behavior turns on)
```

`[emergent]` Trigger: a package has more than one owner or can expose a partial
state. Action: create a lead checkpoint and name integration order before
dispatching lanes. Verification: at most one lane owns each mutable file or
policy decision, and integration does not require redesigning the shared API.
Grounding: `dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md` and R4b's TI/TR/S/A/X/P
dependency graph in `gwz-core/scripts/checks/merge_docs_manifest.json`.

### 6.2 Full package cycle

`[emergent]` Every package MUST follow this cycle:

1. **Baseline:** record exact revisions, clean/dirty state, pins, and prior
   behavior.
2. **Characterize:** add or identify old-behavior and compatibility evidence.
3. **Design:** state scope, invariants, ownership, dependencies, budget,
   forbidden changes, tests, and stop conditions.
4. **Freeze:** obtain required design/interface review and mark exact authority.
5. **Implement:** work TDD-first within ownership and budget.
6. **Integrate:** run focused gates continuously, then full declared gates.
7. **Settle:** commit the coordinated checkpoint and record exact tuple.
8. **Review:** run independent Code/Architecture and State/Filesystem reviews.
9. **Remediate:** write a finding-disposition plan, correct, and re-review.
10. **Accept:** mark accepted only when the gate's verdict rule passes.
11. **Ledger:** record actual LOC/files/wire delta and any reviewed scope
    correction.
12. **Handoff:** state the next legal package and remaining blocks.

Verification: a package is incomplete if any numbered artifact or gate is
missing. Grounding: the R0-R4b history in
`dev-docs/GwzM5-8ChangeBudget.md` and the review/remediation sequence in
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`.

### 6.3 Parallelism decision

`[derived]` Trigger: considering parallel agents. Action: parallelize only
after shared types and ownership are frozen, and only when lanes can be tested
independently. Keep state-machine transitions, wire allocation, and activation
lead-owned. Verification: lane outputs merge without policy resolution. If
they do not, the package was too interconnected and must be serialized.
Grounding: `dev-docs/GwzM5-8R1Ownership.md`,
`dev-docs/GwzM5-8R2aOwnership.md`, and R4b interface checkpoints.

## 7. Document taxonomy and lifecycle

### 7.1 Naming and required content

| Type | Naming | Purpose and required sections | Example |
| --- | --- | --- | --- |
| proposal | `<Program>Refactor.md` or `<Feature>Proposal.md` | Date/status; recommendation; scope/out-of-scope; invariants; current model; target model; packages/dependencies; tests; risks; stop/acceptance | `dev-docs/GwzM5-8Refactor.md` |
| direction memo | `<Program><I#>DirectionMemo.md` | closed scope; decisions frozen now; explicit deferrals; downstream constraints; exit criteria | `dev-docs/GwzM5-8I1DirectionMemo.md` |
| frozen contract | `<Program><I#><Subject>Contract.md` | exact types/bytes/enums/validation; compatibility; ownership; tests; change control | `dev-docs/GwzM5-8I2RecordContract.md` |
| transition design | `<Program><Package>TransitionDesign.md` | closed states/events/actions; legal transitions; owners; proof tokens; unknowns; crash matrix; exit | `dev-docs/GwzM5-8R4bTransitionDesign.md` |
| interface checkpoint | `<Program><Package>InterfaceCheckpoint.md` or `Ownership.md` | exact tuple/scope; owners; visibility; signatures; file manifest; consumer handoff; tests; review gate | `dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md` |
| amendment | `<ControllingSubject>Amendment[-N].md` | documents controlled; exact superseded clauses; problem evidence; replacement decisions; affected types/tests; mandatory review and hash | `dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md` |
| remediation plan | `<ReviewedObject>-RemPlan[-N].md` | input reviews/tuple; finding dispositions; contract decisions; packages/owners/budgets; tests; integration; exit/re-review | `dev-docs/GwzM5-8R4bP1P2-RemPlan-4.md` |
| review | `<Object>-Review<Role>.md` | reviewer/date/tuple; scope; verdict/counts; findings; invariant analysis; commands/results; risks; next action | `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewCode.md` |
| re-review | `<Object>-Review<Role>-N.md` | same as review plus prior-finding closure table and changed-range analysis | `dev-docs/GwzM5-8R2C0Interface-ReviewCode-3.md` |
| ledger | `<Program>ChangeBudget.md` | baseline; counting method; per-package ceilings/ownership; actuals; approved corrections; mandatory reviews | `dev-docs/GwzM5-8ChangeBudget.md` |
| checkpoint/release record | `<Feature>Checkpoint-vX.Y.Z.md` | released scope/lineage; exclusions; exact evidence; development state; blockers; resume order | `dev-docs/GwzMergeCheckpoint-v0.10.5.md` |
| review prompt | `<Object>ReviewPrompt.md` or historical spelling | role/output; read-only rules; independence; exact tuple; authority; deferrals; review areas; commands; report template | `dev-docs/GwzDevCodeM2aReviewPriompt.md` |
| inventory | `<Program><Package>Inventory.md` | exact baseline; call/state/mutation/file inventory; characterization cases; gaps | `dev-docs/GwzM5-8R0Inventory.md` |
| evidence record | `<Subject>Evidence[-Platform].json/md` | exact source/artifact identity; platform/runtime; command/case; result; integrity hash | `gwz-core/scripts/retained_readers/evidence-macos-aarch64.json` |

`[emergent]` Trigger: creating one of these artifacts. Action: use its type's
sections and naming; do not overload a review into a remediation plan or a
checkpoint into a design. Verification: the document can be classified from
its name, scope, and headings. First consolidated here from the cited corpus.

### 7.2 Lifecycle and exact status language

`[emergent]` Documents move through:

```text
draft -> normative/frozen candidate -> independently reviewed -> accepted
      -> amended (old remains readable) -> superseded (old is not current)
```

Use these exact patterns:

```text
Status: **draft; not implementation authority**.

Status: **normative for <scope>; implementation remains blocked pending
<reviews/gate>**.

Status: **accepted at <exact tuple or semantic SHA> after <review files>
reported GO; this accepts <scope> only**.

Amended <date> by `<Amendment.md>`. This document remains authoritative only
as amended for <scope>.

Status: **superseded for <exact scope> by `<NewDocument.md>` as of <date>.
Historical evidence and already-completed gates remain valid only where the
new document says they do**.
```

Trigger: status changes. Action: edit the status and add a changelog entry;
never merely append a contradictory paragraph. Verification: a reader can
identify one current authority without scanning implementation. Grounding:
the good amendment wording in `dev-docs/GwzM5-8I2RecordContract.md` and the
supersession wording in `dev-docs/GwzDevCodeM2a-RemPlan.md`.

### 7.3 No status smearing

`[derived]` Trigger: a document's status would need more than a concise
paragraph or includes multiple later implementation rounds. Action: keep the
document's own status narrow and point to a current checkpoint or ledger row.
Verification: historical design text is not rewritten to masquerade as a live
project dashboard. Grounding: this corrects the drift visible in the opening
of `dev-docs/GwzM5-8ChangeBudget.md` and the eleven-round preamble of
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`.

## 8. Exact document templates

The templates are minimal exact skeletons. Add subject-specific sections but
do not omit the named fields.

### 8.1 Proposal

```markdown
# <Program> proposal

Date: YYYY-MM-DD
Status: **draft; not implementation authority**.

## Changelog
## Recommendation
## Scope
## Out of scope
## Existing behavior and baseline
## Invariants
## Target architecture
## Program waves and dependency graph
## Work packages, ownership, and budgets
## Compatibility and migration
## Test architecture and mechanical gates
## Risks and open decisions
## Stop conditions
## Acceptance criteria
```

### 8.2 Direction memo

```markdown
# <Program> <I#> direction memo

Date: YYYY-MM-DD
Status: **normative for <direction>; not a wire or implementation freeze**.

## Changelog
## Closed scope
## Decision 1: <name>
## Decision 2: <name>
## Explicit deferrals and later owners
## Constraints on the next contract
## Exit criteria
```

### 8.3 Frozen contract

```markdown
# <Program> <I#> <subject> contract

Date: YYYY-MM-DD
Status: **frozen candidate; implementation blocked pending dual review**.

## Changelog and amendments
## Scope and non-scope
## Exact vocabulary, types, and values
## Validation and cross-field invariants
## Ownership and visibility
## Persistence / wire / compatibility behavior
## Adapters and old-version behavior
## Error and projection rules
## Test matrices
## Change-control and re-freeze triggers
## Exit gate
```

### 8.4 Transition or interface design

```markdown
# <Program> <package> <transition|interface> design

Date: YYYY-MM-DD
Status: **frozen candidate; not implementation authority until reviewed**.

## Scope and controlling inputs
## Closed states and vocabulary
## Ownership and visibility
## Exact public/internal interfaces
## Legal transitions or call graph
## Mutation and durable-owner mapping
## File ownership manifest
## Parallel lanes and integration order
## Fault, state, and compatibility matrices
## Stop conditions
## Exit gate
```

### 8.5 Amendment

```markdown
# <Subject> amendment <N>

Date: YYYY-MM-DD
Status: **draft correction; affected implementation is stopped**.

## Changelog
## Documents and clauses controlled
## Evidence that the frozen rule fails
## Replacement decisions
## Exact supersession text
## Interface, wire, ownership, and platform effects
## Migration or no-migration rationale
## Required tests
## Re-freeze and review gate
```

### 8.6 Remediation plan

```markdown
# <Object> remediation plan <N>

Date: YYYY-MM-DD
Status: **planned; findings remain open until independent re-review**.

## Inputs and exact checkpoint
## Assessment: bounded correction or redesign
## Finding disposition
| Finding | Accept/merge/reject | Root cause | Correction owner | Regression proof |
| --- | --- | --- | --- | --- |
## Contract decisions to freeze first
## Work packages, files, budgets, and dependencies
## Test-first sequence
## Integration order
## Complete verification matrix
## Re-review prompt and exit criteria
```

### 8.7 Review and re-review

```markdown
# <Object> independent <role> review[-N]

Date: YYYY-MM-DD
Reviewer designation: <XX>

## Verdict
**GO|NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

## Exact reviewed tuple and worktree state
## Scope and authority read
## Prior-finding closure (required for re-review)
## Findings
### [P2-1] <single root-cause title>
- Where:
- Violated invariant:
- Reproduction/state sequence:
- Impact:
- Required correction:
- Regression test:
## Invariant and matrix verification
## Commands and exact results
## Residual risks / unexecuted evidence
## Final tuple recheck and next action
```

Finding IDs reset by report and severity: `[P<0-3>-<ordinal>]`. A remediation
plan preserves the original ID and report name; it does not renumber the
finding into a new history.

### 8.8 Change-budget ledger

```markdown
# <Program> change-budget ledger

Date: YYYY-MM-DD
Status: **<concise current accepted package boundary>; details in <checkpoint>**.

## Counting rules and stop triggers
## Baseline revisions and clean-state proof
## Measured baseline concentrations
## Frozen package ceilings
### <Package> — <purpose>
Intentional production behavior delta:
Wire/protocol delta:
| Ownership slice | Production LOC add/delete | Moved LOC | Test/tool/doc LOC | Files touched/created |
| --- | ---: | ---: | ---: | ---: |
Allowed files/owners:
Forbidden scope:
Mandatory reviews:
## Executed package reports
```

Exact package report:

```text
package:
baseline revision:
production LOC added/deleted:
production LOC moved:
test/tool/doc LOC added/deleted:
production files touched/created:
test/tool/doc files touched/created:
wire/protocol delta (expected / actual):
ceiling result:
scope exceptions requested:
```

### 8.9 Checkpoint or release record

```markdown
# <Feature> checkpoint — <version or package>

Date: YYYY-MM-DD
Status: **checkpoint record; not acceptance for excluded work**.

## Purpose
## Included scope and exact lineage
## Explicit exclusions
## Local executed evidence
## CI/platform executed evidence and artifact identities
## Tag/version/supersession record
## Development-line tuple retained
## Exact open blockers
## Backlog and platform gaps
## Ordered resume procedure
```

### 8.10 Review-prompt file

```markdown
# Independent <object> <role> review prompt

You are conducting an independent, read-only <role> review of <object>.
Write only `dev-docs/<Object>-Review<Role><-N>.md`.

Do not modify code, tests, protocol, existing documentation, workspace
artifacts, or another reviewer's report. Do not stage, commit, or push.
Do not read the peer current-round report before completing your verdict.

## Exact repository and checkpoint tuple
## Sources of truth in precedence order
## Intended scope and explicit deferrals
## Required review method
## Role-specific review areas
## Required commands and exact evidence format
## Severity and GO/NO-GO decision rules
## Required report structure
## Final tuple and worktree recheck
```

This skeleton reconciles the scope, independence, tuple, severity, and report
requirements in `dev-docs/GwzDevCodeM2aReviewPriompt.md` and the later
`GwzDevCodeM2a-Rem2ReviewPrompt.md` / `Rem3ReviewPrompt.md` files.

## 9. Canonical independent-review role prompts

### 9.1 Code/architecture/interface reviewer

```text
Role: independent Code/Architecture reviewer.

Review the exact settled tuple below as safety-critical software. Do not read
the peer current-round report. Write only the designated report; make no other
workspace change and do not stage, commit, or push.

First verify the tuple and worktree. Read the controlling requirements,
designs, accepted amendments, package checkpoint, ledger row, prior findings,
and complete current implementation. Diff reading is insufficient: follow all
callers, implementors, serializers, adapters, generated surfaces, and gates.

Emphasize: semantic completeness; owner and visibility boundaries; ways an
alternative implementation, alias, callback, re-export, loader, or driver can
bypass the intended interface; old/new compatibility; activation reachability;
and whether the type system and compiler enforce the written design.

Construct concrete adversarial counterexamples and run focused probes in an
exact-tree temporary copy when useful. Do not trust green tests or the
remediation narrative. Use P0-P3 and GO/NO-GO exactly as defined in the
rulebook. Report exact commands, counts, unexecuted evidence, and final tuple.
```

### 9.2 State/filesystem/fault reviewer

```text
Role: independent State/Filesystem/Fault reviewer.

Review the exact settled tuple below as safety-critical durable-state
software. Do not read the peer current-round report. Write only the designated
report; make no other workspace change and do not stage, commit, or push.

First verify the tuple and worktree. Read the controlling requirements,
designs, accepted amendments, package checkpoint, ledger row, prior findings,
and complete current implementation. Reconstruct the state machine and every
physical mutation independently of the tests.

Emphasize: whole-operation preflight; crash boundaries; restart from fresh
authority; path/identity substitution; lock lifetime and ordering; wrong-kind,
alias, capacity, and native-name behavior; durability and sync; not-started,
completed-exactly, and ambiguous observations; rollback; and read-only failure.

Execute fault and race probes in an exact-tree temporary copy when useful.
Distinguish an enumerated key from an executed restart case. Do not trust green
tests or the remediation narrative. Use P0-P3 and GO/NO-GO exactly as defined
in the rulebook. Report exact commands, counts, platform limits, unexecuted
evidence, and final tuple.
```

### 9.3 Re-review addition

Append this exact block to either role prompt:

```text
This is re-review round <N>. Reproduce each prior finding on its original
checkpoint when practical, then prove it closed or still open on the current
tuple. Review the correction's complete changed call graph and look for new
root causes introduced by the remedy. A prior finding is not closed merely
because its named test passes. The report must include a prior-finding closure
table and an independent whole-object regression verdict.
```

## 10. Change-budget mechanics

### 10.1 Counting

`[emergent]` Trigger: freezing or closing a package. Action:

- count handwritten production additions/deletions separately from moved
  production lines;
- count tests, tools, and docs separately;
- count physical files touched and created;
- count deleted and replacement paths separately;
- count a shared file in every package whose review surface it affects, but
  once in the aggregate total;
- exclude generated output from handwritten LOC while still counting its
  schema source and generated file count; and
- declare wire/protocol delta as exact expected versus actual.

Verification: the package report uses the exact fields in §8.8 and conservative
movement accounting; similarity heuristics may not hide semantic growth.
Grounding: R1 and R2a measured execution in
`dev-docs/GwzM5-8ChangeBudget.md`.

### 10.2 Twenty-percent tolerance and correction

`[emergent]` Trigger: any numeric ceiling exceeds 120 percent, a new production
module appears, wire delta changes, or an ownership boundary is crossed.
Action: stop implementation, explain why the original estimate was wrong,
separate required proof work from scope creep, revise the ceiling only through
a reviewed scope decision, and update both package plan and ledger. Verification:
the ledger keeps original and revised ceilings with rationale and actuals.
Grounding: R0 and R1 adjustment narratives in
`dev-docs/GwzM5-8ChangeBudget.md`.

### 10.3 File cohesion

The controlling current rule is L1-23. The historical R0 use of 500-line
“slices” was a package budgeting technique, not a universal hard file limit.
A cohesive 518-line module was accepted under a documented 550-line exception;
that exception does not raise the general target. Grounding:
`dev-docs/GwzM5-8ChangeBudget.md` R1.

## 11. Evidence and mechanical gates

### 11.1 Evidence record

Every gate record MUST include:

```text
object/package:
exact root and member revisions:
worktree state and allowed exceptions:
platform/architecture/runtime/tool versions:
command or fault scenario:
executed case count, passed/failed/ignored:
artifact/run ID and checksum when external:
unexecuted or unsupported cases:
verdict consequence:
```

`[derived]` Trigger: claiming a gate passed. Action: fill every applicable
field. Verification: an independent reviewer can associate the evidence with
one immutable tree. Grounding: `dev-docs/GwzMergeCheckpoint-v0.10.5.md` and
the exact command sections in current review reports.

### 11.2 Existing gate registry

| Gate | What it enforces | Existing artifact |
| --- | --- | --- |
| GWZ workspace status/lock | exact coordinated members and lock pins | installed `gwz status --json`; `AGENTS_GWZ.md` |
| formatting | canonical Rust formatting | `cargo fmt --all -- --check` |
| Rust tests | focused and full behavior | Cargo test suites named in `gwz-core/dev-docs/GwzMergePlan.md` §15 |
| Clippy | compiler/lint boundary, denied warnings | release and checked-artifact workflows |
| protocol regeneration | Taut generated code current | `gwz-core/protocol/regen.py --check` |
| cross-driver fixtures | Rust/Python/JSON/JSONL parity | I2 contracts and driver tests |
| retained readers | actual older binary behavior | `gwz-core/scripts/retained_readers/` |
| merge docs | required/forbidden capability statements | `gwz-core/scripts/checks/check_merge_docs.py` and manifest |
| compatibility predicates | migration registry/corpus agreement | `gwz-core/scripts/checks/check_merge_compatibility_predicates.py` |
| checked-artifact boundary | positive source graph, visibility, writer containment | `gwz-core/scripts/checks/check_checked_artifact_boundaries.py` |
| release boundary | exact target and no bypass branches | `gwz-core/scripts/checks/test_release_boundary.py` |
| workflow wiring | gates run on PR/push/release exact checkout | `.github/workflows/` in core/CLI/Python |
| package smoke | wheel installs and native bridge runs cleanly | `gwz-py/.github/workflows/package-smoke.yml` |
| native platform | supported build/test/artifact matrix | release run IDs in `GwzMergeCheckpoint-v0.10.5.md` |

`[derived]` Trigger: a package exit gate is written. Action: select every
applicable existing gate and add subject-specific focused tests. Verification:
the checkpoint lists commands and results; “tool unavailable” remains an open
gap unless the controlling gate permits it. Grounding: cited artifacts.

### 11.3 Proposed rulebook doc-gate entries—proposal only

The following entries SHOULD be added to a future documentation manifest, but
this task MUST NOT implement them:

```json
{
  "id": "agent_process_rules",
  "path": "dev-docs/AgentProcessRules.md",
  "required": [
    {"id": "rulebook_date", "value": "Date:"},
    {"id": "rulebook_status_no_authority", "value": "does not amend a GWZ design"},
    {"id": "rulebook_l1", "value": "## 3. L1: universal standing instructions"},
    {"id": "rulebook_l2", "value": "## 4. L2: toolchain and platform bindings"},
    {"id": "rulebook_l3", "value": "## 5. L3: GWZ workspace bindings"},
    {"id": "rulebook_honesty", "value": "## 14. Honest process assessment"},
    {"id": "rulebook_conflicts", "value": "## 15. Conflicts and unresolved precedence"},
    {"id": "rulebook_bootstrap", "value": "## 16. Bootstrap kit"},
    {"id": "rulebook_acceptance", "value": "## 17. Acceptance self-assessment"}
  ],
  "forbidden": [
    {"id": "rulebook_false_go", "value": "Status: **accepted implementation"}
  ]
}
```

Proposed checker extensions:

- parse every `### L1-`, `### L2-`, and `### L3-` rule and require one
  provenance label, Trigger, Action, Verification, and Grounding;
- verify every cited workspace-relative path exists;
- verify every cited section anchor (for example `§17`) exists in the cited
  document, not merely the path;
- require every emergent rule to contain “First” or an explicit origin;
- reject status paragraphs above a reviewed line limit and require a
  checkpoint link instead;
- require review reports to contain verdict, severity table, exact tuple,
  commands, residual risks, and final tuple recheck;
- require re-review reports to contain prior-finding closure;
- require accepted documents to name their controlling reports and semantic
  hash or exact tuple; and
- require a superseded document to name its replacement and exact scope.

These are `[unexercised]` proposals. The existing merge-doc checker only
performs normalized literal/regex presence and absence checks; it does not
parse these semantics.

## 12. Git and release operating procedure

### 12.1 Development checkpoint

`[emergent]` Trigger: a package is ready for independent review. Action:

1. run focused gates;
2. inspect `gwz status --json` and preserve unrelated changes;
3. stage the logical workspace change with installed GWZ;
4. commit one coordinated checkpoint with package ID and purpose;
5. verify member commits and workspace lock match;
6. record the tuple in the review prompt; and
7. do not mutate it while reviews run.

Verification: reviewers see the same exact tuple and only create authorized
report files. Grounding: `AGENTS_GWZ.md` and the tuple sections in review
prompts/reports.

There is no consistently exercised universal branch-name or pull-request
policy in this corpus. A proposed “one branch per package” rule would be
`[unexercised]` and is intentionally not claimed here. The practised invariant
is exact lineage and settled commits, not a particular branch spelling.

### 12.2 Release procedure

`[derived]` Trigger: the operator chooses a version and the release candidate
meets its design gates. Action:

1. select the exact release lineage and write intended inclusions/exclusions;
2. coordinate core, CLI, and Python versions and exact pins;
3. run local core, CLI, Python, protocol, docs, formatting, Clippy, package, and
   smoke gates from clean environments;
4. use disposable preflight tags when CI requires tag context;
5. require Windows, macOS, Linux x86-64, and Linux ARM64 evidence for promised
   artifacts;
6. create immutable final tags only at proven commits;
7. publish artifacts and checksums;
8. write the release checkpoint; and
9. independently review scope, lineage, artifacts, notes, and supersession.

Verification: the checkpoint's table and workflow run IDs match the tags and
published artifacts. Grounding: v0.10.5 checkpoint and release review.

## 13. Session mechanics and operator interaction

### 13.1 Cold start

`[emergent]` Trigger: a fresh agent/session receives the project. Action:

1. read all applicable AGENTS files;
2. run read-only coordinated status and record exact tuple/dirt;
3. read baseline requirements/design, then feature design and plan;
4. read the current ledger row and latest checkpoint;
5. read accepted amendments in precedence order;
6. read the latest two complete review rounds, not every historical review at
   first;
7. inspect current code and diff for the active package;
8. state current package, controlling authority, open finding, next legal
   action, and forbidden action; and
9. only then edit or run expensive gates.

Verification: another maintainer can correct the stated status before work is
mutated. Grounding: `dev-docs/GwzDevCodeM2aReviewPriompt.md` source order and
the restart instructions in `dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

### 13.2 Long-context management

`[emergent]` Trigger: work spans multiple sessions, agents, outages, or many
review rounds. Action: keep state in committed plans/checkpoints and append
small dated implementation checkpoints only when they remain the owning
document; otherwise create a new checkpoint. Record exact tuple, tests, open
findings, and next action. Verification: resumption does not require replaying
chat. Grounding: dated R2 checkpoints in
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` and the cleaner v0.10.5
checkpoint.

### 13.3 Separate role sessions

`[emergent]` Trigger: a lead interface, durable transition kernel, settled
consumer checkpoint, or release boundary is ready. Action: run Code and State
reviews in independent sessions with report-only permissions and peer-report
blindness. Use a new round after remediation. Verification: reports have
distinct reasoning and exact tuple checks. Grounding: M2a review prompt and
R4b dual reports.

### 13.4 Non-blocking progress communication

`[emergent]` Trigger: work takes more than a short command or encounters an
unexpected result. Action: tell the operator what is being checked, what has
been learned, and whether authority changed; do not narrate every command.
Verification: the final handoff remains self-contained and the operator can
intervene before scope diverges. Grounding: practised throughout this program;
there is no repository-enforced artifact, so this conversational rule is
marked `[unexercised]` for mechanical verification.

### 13.5 Reserved operator decisions and stop response

When a reserved decision in L1-28 appears, the agent MUST present:

```text
decision required:
controlling current rule:
evidence forcing the decision:
safe options and consequences:
recommended option:
work preserved / mutation not performed:
```

When no authority is missing, the agent SHOULD make bounded implementation
assumptions and proceed. When authority is missing, it MUST stop rather than
turn an assumption into product policy. Grounding: the decision to split M4,
defer M6-M8, and cut v0.10.5 from the narrow line is reflected in
`gwz-core/dev-docs/GwzMergePlan.md` and
`dev-docs/GwzMergeCheckpoint-v0.10.5.md`.

## 14. Honest process assessment

### 14.1 Rules followed inconsistently

1. **No status smearing was not followed.**
   `dev-docs/GwzM5-8ChangeBudget.md` and
   `dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` accumulated long live
   program histories in their status/preamble. This helped context recovery
   but made the owning scope harder to see and created stale claims.
2. **Freeze-before-code was sometimes weakened into code-discovery.** The
   first-catalog design needed repeated correction after implementation/review
   exposed absent-parent, lease association, native alias, allocation, and
   publication identity gaps. The later amendment is strong, but the earlier
   interface should not have been called settled so soon.
3. **Review rounds were allowed to become the architecture process.** Eleven
   R4b re-review rounds found real defects, but several should have triggered
   an earlier architecture reset instead of another local guard.
4. **Exact platform evidence arrived late.** Windows exposed compile and test
   failures after substantial C1 work. Native CI should have run at the first
   platform-bound interface, not near release/checkpoint.
5. **Documentation version wording was once treated too literally.** Earlier
   discussion fixated on v0.10.0 text after v0.10.2 had become the meaningful
   corrected release. The proper rule is to express release intent without
   baking a provisional version into durable design claims.
6. **Settle-before-review was not practiced through most of R4b.** The
   TI-P0.2 checkpoints were reviewed as uncommitted trees later squashed into
   two ~19k-line commits, leaving accepted intermediate states unrecoverable
   from git and their acceptance evidence non-reproducible. The
   commit-per-checkpoint discipline began at the P1 freeze and is now L1-25
   as practiced going forward.

### 14.2 Rules that failed or produced their own defect class

1. **Regex/source denylist gaming.** The structural gate initially missed
   function-pointer aliases, fully qualified trait implementations, imported
   aliases, adjacent callbacks, commented-out guards, `.inc`/`include!`
   sources, and module-root redirection. The review history is catalogued in
   `dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §1. The correction was
   positive compiler-route and whole-tree containment, but source digests
   remain expensive and brittle.
2. **Green tests as false confidence.** Several counterexamples compiled and
   passed all current gates until reviewers injected an unreviewed
   implementation or substituted source object. Passing tests did not prove
   the owner class was closed.
3. **Fault-key inventory mistaken for matrix.** R2-C2 originally asserted the
   list of 21 keys, not interruption and restart at every key. The v0.10.5
   checkpoint explicitly corrects that claim.
4. **Stable sort violated fallible-allocation claims.** Explicit `try_reserve`
   was insufficient because Rust's stable sort allocated hidden scratch.
   `GwzM5-8R2C0Interface-ReviewCode-2.md` found it; Code-3 accepted the in-place
   correction.
5. **Path verification followed by path rename created a TOCTOU defect.** The
   final check dropped identity before mutation. C2 re-review proved foreign
   bytes could be moved. This was an architecture defect, not a missing second
   check; the amendment now requires one sealed source-associated primitive.
6. **ASCII-only classification skipped native aliases.** A non-ASCII APFS
   equivalent could evade classification. C1 re-review forced native-name
   charging and fail-closed semantics.
7. **The change budget never bound.** Every major ceiling revision went
   upward (R4b-P production 1,450 → 14,000, roughly 10x; the R0 harness 500
   → 7,100) and no ceiling ever forced descoping. The mechanism's real value
   was forcing re-review at scope escalation; as cost control it was a
   one-way ratchet. Corrected by `GwzProcessOptimization.md` §2.1: keep the
   escalation triggers, retire the hand-kept arithmetic, and require a
   ceiling revision to answer "what is cut" before "what is raised."

### 14.3 Deliberate redundancy versus drift

Deliberate redundancy:

- requirements, design, plan, interface checkpoint, review prompt, review,
  ledger, and checkpoint repeat the active boundary so a cold-start agent need
  not reconstruct it from code;
- Rust/Python/human/JSON/JSONL fixtures repeat one semantic result to detect
  driver drift;
- two reviewers repeat exact-tuple and gate checks because their adversarial
  axes differ; and
- retained-reader fixtures repeat current behavior through actual old binaries.

Drift-producing redundancy:

- status paragraphs copied across old plans;
- version/capability prose repeated without manifest assertions;
- repeated remediation preambles listing every review round;
- multiple descriptions of platform support that were not all tied to the
  same run IDs; and
- frozen contract text amended by prose in several later files without a
  concise authority index.

### 14.4 Changes I would make if authorized

These are proposals, not current rules:

1. Create one small `CurrentProgramCheckpoint.md` containing only exact tuple,
   controlling-doc graph, accepted packages, open findings, and next action;
   prohibit live status histories in designs and ledgers.
2. Add a machine-readable authority manifest with document scope, semantic
   hash, supersedes edges, required reviewers, and acceptance state.
3. Cap a remediation cycle at two rounds; a third new architectural root cause
   forces redesign and re-freeze.
4. Run minimal native Linux/macOS/Windows interface probes at the first
   platform-bound checkpoint, not at final integration.
5. Prefer compiler/type-system closure over source-text security gates; use
   source manifests only to detect loading/call-graph expansion.
6. Generate review prompts from one canonical template plus role-specific
   sections and exact tuple, eliminating prompt drift and misspellings.
7. Add a machine-readable fault matrix whose rows require an executed result,
   exact source revision, restart mode, and artifact link.
8. Separate release-intent names from provisional version numbers until the
   operator selects the final version.
9. A ceiling revision must state what is descoped before it may state what is
   raised.

Adoption note (2026-08-15): proposals 1, 3, 4, and 6, plus 9 and the
two-track freeze and tiered-review rules, were adopted by the operator via
`GwzProcessOptimization.md`; proposals 2, 5, 7, and 8 stay `[unexercised]`.

## 15. Conflicts and unresolved precedence

### 15.1 Overall design versus feature design

`gwz-core/AGENTS.md` says:

> Do not add behavior beyond what `GWZDesign.md` (authoritative) and
> `GWZRequirements.md` (baseline) specify; update them before expanding scope.

The M2a review prompt says `gwz-core/dev-docs/GwzMergeDesign.md` is “merge
behavioral authority,” and later I1/I2 documents say
`dev-docs/GwzM5-8Refactor.md` remains behavioral authority for their scope.

Conflict: there is no single explicit precedence clause reconciling the
overall authority with feature-specific accepted amendments. Practised
resolution has been to require agreement across all of them and run the
merge-doc checker; this rulebook does not silently declare one superior. A
future authority manifest should resolve it.

### 15.2 Historical 500-line language versus current file rule

Early R0 text in `dev-docs/GwzM5-8ChangeBudget.md` speaks of separate
500-line slices. The operator's later standing rule and the same ledger's
“Measured merge baseline” use about 1,000 lines as a review trigger, with
post-split owners below about 500 and earlier cohesion splits when needed.

Resolution: the later operator rule controls general file cohesion; the R0
500-line values remain package-specific budgets. The 1,000 threshold is not a
hard automatic split.

### 15.3 Release build evidence versus specialized native proof

The operator says release builds are platform evidence. The catalog amendment
and R2-F plan require native fault/identity evidence for platform-specific
durability and recovery claims.

These are not interchangeable. A successful release build/test run proves the
release gate it executes; it does not prove an unexecuted power-loss or
identity fault matrix. Conversely, a focused native fault run does not prove
packaging. Both texts remain controlling in their scopes.

### 15.4 Current status claims versus later review files

Several old documents say a package is accepted or pending at an older round,
while later suffixed reviews find new blocking defects. Example: the R2
consumer remediation preamble records GO/GO for earlier interface work, while
the C2 re-review later reports a publication race.

Resolution: acceptance remains valid only for its exact tuple and declared
scope. The latest review of the same object controls whether work may proceed;
it does not retroactively erase historical evidence. This is why checkpoint
and exact tuple are mandatory.

### 15.5 Requested output filename

The attached task prompt requests `dev-docs/GwzDevProcessRulebook.md`; the
operator's direct request says to put the output in
`dev-docs/AgentProcessRules.md`. The direct request controls this deliverable.
No alias file was created because the task also says documentation-only and
asks for the output at one explicit path.

### 15.6 Platform carve-outs

The current accepted catalog amendment specifies exact Windows handle rename
and an explicit Linux/macOS same-user POSIX trust boundary. The v0.10.5 release
record promises Windows, macOS, and Linux artifacts. A conversational
willingness to tolerate a potential hole outside Linux/macOS cannot silently
change those accepted design and release claims. Any such support carve-out
requires an operator decision, amendment, public support statement, and
release-gate change. This document makes none.

## 16. Bootstrap kit

### 16.1 Minimal files for an empty repository

Create these before the first implementation package:

```text
AGENTS.md
dev-docs/ProgramProposal.md
dev-docs/ProgramChangeBudget.md
dev-docs/CurrentProgramCheckpoint.md
dev-docs/templates/DirectionMemo.md
dev-docs/templates/FrozenContract.md
dev-docs/templates/InterfaceCheckpoint.md
dev-docs/templates/Amendment.md
dev-docs/templates/RemediationPlan.md
dev-docs/templates/Review.md
dev-docs/templates/ReviewPrompt-Code.md
dev-docs/templates/ReviewPrompt-State.md
scripts/check_docs.py
scripts/doc_manifest.json
```

The templates may initially be copied from §8 and §9. In a multi-repository
workspace, add one root coordinator instruction equivalent to
`AGENTS_GWZ.md`, repository-local AGENTS files, and a machine-readable lock of
member revisions.

### 16.2 Minimal standing instructions

Copy the quoted L1 rules into `AGENTS.md`, or include this compact subset by
reference:

```text
Read all applicable instructions and controlling documents before work.
Record an exact clean baseline and characterize released behavior first.
Freeze shared semantics, ownership, compatibility, budgets, and tests before
parallel implementation. Work test-first and never let implementation invent
policy. Persist typed authority before external mutation and fail closed on
ambiguous recovery. Stop for scope, ownership, authority, platform, or
blocking-review failures. Review exact committed checkpoints through two
independent axes; P0/P1/P2 blocks. Amend frozen decisions explicitly and
re-review them. Record executed evidence separately from inventory. Keep tags
immutable and preserve restartable checkpoint artifacts.
```

This compact block is a summary; the full L1 rules control when using this
rulebook.

### 16.3 First-session procedure

1. Write `AGENTS.md` with authority, mutation, testing, review, and operator
   decision rules.
2. Record repository/toolchain/platform baselines and current release behavior.
3. Create a characterization package with tests for public output, durable
   bytes, failure behavior, and compatibility.
4. Draft the proposal with scope, invariants, out-of-scope, waves, package IDs,
   dependency graph, and activation boundary.
5. Create the change-budget ledger with initial ceilings and file ownership.
6. Identify the first shared semantic decision; write a direction memo if
   product policy is unresolved, then a frozen contract or interface
   checkpoint.
7. Generate Code and State review prompts against an exact committed design
   tuple.
8. Run two independent report-only reviews. Do not let them read each other.
9. If either returns P0/P1/P2, write a remediation plan; if the correction
   changes semantics, write an amendment and repeat the freeze review.
10. Mark the design accepted only after both gate verdicts pass.
11. Begin the first implementation package TDD-first within the frozen budget
    and ownership.
12. After implementation, settle, dual-review, remediate, accept, measure, and
    write the current checkpoint.

Verification: the resulting proposal, ledger, contract, two prompts, two
reports, and checkpoint match the templates and identify one exact tuple.
These bootstrap filenames are `[unexercised]` outside GWZ, but the procedure is
grounded in R0, I1/I2, and R4b.

## 17. Acceptance self-assessment

### 17.1 Result

This document **substantially meets** the acceptance bar. It provides:

- a complete trigger-action-verification lifecycle;
- reusable L1 instructions plus Rust/CI and GWZ bindings;
- program/package/lane and identifier rules;
- precise freeze and precedence semantics;
- budget measurement and stop rules;
- dual-review roles, severity, verdict, closure, and round rules;
- implementation, durability, evidence, git, release, session, and operator
  rules;
- exact document, ledger, review, and prompt skeletons;
- existing and proposed mechanical checks;
- explicit conflicts and an unsoftened failure assessment; and
- an empty-repository bootstrap procedure.

### 17.2 Known gaps

1. The corpus has no single machine-readable authority/supersession graph, so
   the precedence model is reconstructed from prose and may miss an obscure
   historical amendment.
2. Commit granularity is practised but not governed by a consistent branch/PR
   policy; this document refuses to invent one.
3. Some conversational operator decisions have no repository artifact. They
   are reproduced in Appendix A where available but cannot be mechanically
   verified.
4. Review role names evolved (`F5`, `56`, `FS`, `State`, `Code`); the canonical
   role split here normalizes practice rather than reproducing every label.
5. Existing status-smear documents prevent a purely mechanical answer to
   “what is current”; cold start still needs the latest checkpoint and review
   tuple.
6. The current R2-C2 code checkpoint is ongoing. This rulebook describes its
   process and known review history but does not claim that C2 or v1 is
   accepted or releasable.
7. The proposed bootstrap filenames and doc-gate extensions have not been
   exercised outside GWZ and are marked accordingly.

A newcomer should be able to reproduce the form and rigor of one package
cycle. To be literally indistinguishable from an experienced run, they would
still need project-domain competence and access to the actual platform and
retained-binary environments; no process document can replace those facts.

## Appendix A — standing operator instructions, verbatim

Only operator-visible workspace and conversational instructions are reproduced
here. Hidden runtime or service instructions are not project process inputs.

### A.1 Root `AGENTS.md`

```text
Read and follow `AGENTS_GWZ.md` before doing any work in this workspace.
```

### A.2 `AGENTS_GWZ.md`

````text
<!-- gwz-managed-file: sha256=1377a97512381fb61757d4387293d833b2c57329b37b6c3bb60924dc319d20e8 -->

# GWZ Workspace

This repository is managed by GWZ, a multi-repository workspace tool.

For workspace-wide status, staging, and commits, use `gwz status`, `gwz add`,
and `gwz commit`. Do not substitute per-repository Git loops.

Install `gwz` from the latest release:

```sh
curl --proto '=https' --tlsv1.2 -LsSf \
  https://github.com/owebeeone/gwz-cli/releases/latest/download/gwz-installer.sh | sh
```

Or install from source:

```sh
cargo install --git https://github.com/owebeeone/gwz-cli
```

If the workspace is not cloned yet:

```sh
gwz clone <workspace-git-url> [directory]
```

If this root repository is already cloned:

```sh
gwz materialize --lock
gwz status
```

Docs:

- `gwz --help`
- Quick Start: https://owebeeone.github.io/gwz-cli/QuickStart/
- Full documentation: https://owebeeone.github.io/gwz-cli/
````

### A.3 `gwz-core/AGENTS.md`

```text
# AGENTS

Follow the root `AGENTS.md` rules.

- Work TDD-first: failing test, implementation, green tests, then refactor.
- Keep `gwz-core` independent of `gwz-cli`.
- Do not add behavior beyond what `GWZDesign.md` (authoritative) and `GWZRequirements.md` (baseline) specify; update them before expanding scope.
- Protocol payloads are taut-defined; do not create a shadow protocol.
```

### A.4 File-size and cohesion instructions

```text
1000 lines is the threshold - i.e. once a file reaches 1000LOC then split to files less that 500LOC.
```

```text
Also, this is not a hard rule - if for example you reach 1001LOC, use your judgement,
```

```text
One more nuance - if you do find that a sub 500loc sized file is becoming a god file - i.e. you realize the file is becoming a dumping ground for too many concepts - then consider whether it should be split sooner than the 1000LOC (or even 500LOC) threshold. Obviously, one should decide to reduce this kind of churn in the planning phases rather than leave it to orgranic evolution.
```

### A.5 Review/freeze sequence instruction

```text
No -
Next is to freeze and review P1—not write more P1 code.
1 Commit the current P1 checkpoint with installed gwz, giving reviewers an exact SHA.
2 Run two independent settled-tree reviews. P1 preservation and the already-implemented P2 rollback can be reviewed in parallel.
3 Remediate and re-review any P0/P1/P2 findings.
4 Once P1 and P2 are accepted, complete R4b-G:shared router integration;
aggregate restart/fault matrices;
v0 compatibility and byte-equivalence;
unknown-field, privacy, and call-graph gates;
P3/P4 status/archive cross-driver integration.

5 Run the two final full-tree R4b reviews.
6 Then proceed to M5b and finally A1 activation.
```

### A.6 Platform and release instructions

```text
Release build is the platform exidence.
```

```text
note, the release builds are done for windows, macos, linux x86 and linux arm64
```

```text
gwz-py is needed fo a release
```

```text
wheb you mention the posix portability issue, doees that mean Linux and Macos? I'm ok to carve out non Linux or Macos as having a potential hole.
```

This last instruction records the operator's willingness to consider a
support/trust carve-out. Section 15.6 explains why it is not, by itself, an
accepted change to the current Windows design or released platform promise.

### A.7 Workspace-agent instruction decision

```text
Truly, why not just refer to AGENTS_GWZ.md?
```

This instruction resulted in the one-line root `AGENTS.md` reproduced above.

## Appendix B — high-value artifact index

- Overall requirements: `gwz-core/dev-docs/GWZRequirements.md`
- Overall design: `gwz-core/dev-docs/GWZDesign.md`
- Merge behavioral design: `gwz-core/dev-docs/GwzMergeDesign.md`
- Merge implementation/release plan: `gwz-core/dev-docs/GwzMergePlan.md`
- M5-M8 proposal/program: `dev-docs/GwzM5-8Refactor.md`
- Change-budget ledger: `dev-docs/GwzM5-8ChangeBudget.md`
- Characterization inventory: `dev-docs/GwzM5-8R0Inventory.md`
- Direction freeze: `dev-docs/GwzM5-8I1DirectionMemo.md`
- I2 contracts: `dev-docs/GwzM5-8I2ProtocolContract.md`,
  `dev-docs/GwzM5-8I2RecordContract.md`,
  `dev-docs/GwzM5-8I2ActionJournalContract.md`, and
  `dev-docs/GwzM5-8I2CompatibilityContract.md`
- Transition design: `dev-docs/GwzM5-8R4bTransitionDesign.md`
- Interface checkpoints: `dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md` and
  `dev-docs/GwzM5-8R4bR2ConsumerCheckpoint.md`
- Current catalog amendment:
  `dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md`
- Long-running remediation/checkpoint record:
  `dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`
- Canonical early review prompt: `dev-docs/GwzDevCodeM2aReviewPriompt.md`
- Current C2 reviews: `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewCode.md`,
  `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewCode-2.md`, and
  `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewState.md`
- Release checkpoint and independent review:
  `dev-docs/GwzMergeCheckpoint-v0.10.5.md` and
  `dev-docs/GwzReleaseReview-v0.10.5-F5.md`
- Document consistency gate:
  `gwz-core/scripts/checks/merge_docs_manifest.json` and
  `gwz-core/scripts/checks/check_merge_docs.py`
- Checked-artifact boundary gate:
  `gwz-core/scripts/checks/check_checked_artifact_boundaries.py`
- Retained-reader harness: `gwz-core/scripts/retained_readers/`
