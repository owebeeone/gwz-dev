# No-fallback first checkpoint — State

Generated from the review-loop canonical template.

You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: State
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzNoFallbackCheckpoint-ReviewState.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: d98922e03b837d030477f1d9a696fef2464b8b17
- core: dd47810ece5980cfa35017ae0dfd7a8f33701e80
- git2-rs: f42a01267a3042b26d30e9d8acf286c6c739bd8a
- transport: 28f5afb3938a2aa8af0e1e8d5b07779add6ab776
- taut: 733e8a78897a90f017f4726e4331aed95e8cb977
- Object: gwz-core/dev-docs/GwzNoFallbackCheckpoint.md and its baseline GwzNoFallbackPreparation.md; P2 first-package planning gate only
- Controlling DRAFT document: gwz-core/dev-docs/GwzNoFallbackCheckpoint.md at dd47810ece5980cfa35017ae0dfd7a8f33701e80
- Out of scope: only root dev-docs/GwzNoFallbackCheckpoint-Prompt{Code,State}.md are untracked authorized inputs. Peer current prompts/reports are forbidden. No production implementation yet.

AUTHORITY AND DEFERRALS
- Process authority: AgentProcessRules.md as amended by GwzProcessOptimization.md; operator explicitly retains old reviewers and requests economical execution.
- Controlling documents to check the object against: gwz-core/dev-docs/GwzNoFallbackPlan.md; GwzNoFallbackPreparation.md; GwzRemoteTransportNativeBinding.md; GwzRemoteTransportAdapterFoundation.md; relevant core GWZDesign/GWZRequirements and root CurrentProgramCheckpoint clauses. Existing code only as needed to verify concrete feasibility.
- Explicitly deferred (do not report as findings): Route choice and replacement implementation in lanes 1/3/4 deliberately follow characterization. L2-A is an unpublished member-backed binding candidate, not release or activation. P2 freezes only first packages, no open-ended umbrella permission. Existing runtime signatures are retained; new source-mode runner surface gets review at implementation. User go resumes implementation subject to this gate; no extra approval is needed after GO..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
AXIS: STATE — durable-state semantics and adversity.
Attack: state machines and restart legality; filesystem and durability
ordering; crash/kill points between every pair of writes; races and lock
scope; fail-closed direction (a defect may lose progress, never invent it);
recovery states as a closed grammar — hunt for new stuck states the current
semantics does not have.

Check P2 satisfies prior missing-gate findings: exact ownership, bounded budgets, compatibility baseline, implementation order, all-consumer/platform activation separation. Verify first packages are implementable on inspected source and do not imply full four-lane completion. Source alignment must preserve pinned Rust/native identity and must not treat cloned HEAD as qualified.

COMMANDS
From /Users/owebeeone/limbo/gwz-dev: git rev-parse HEAD; git -C gwz-core rev-parse HEAD; git -C git2-rs rev-parse HEAD; corresponding git status --short and git diff --check. Also read-only git show/log/diff/ls-tree, rg, sed, cat, nl, wc. No file writes, tests, builds, network or git mutation. Recheck all named SHAs at start/end. Do not read peer current-round prompt/report. Keep targeted and return concise report.

SEVERITY AND VERDICT CONTRACT
- Findings use IDs P0-n / P1-n / P2-n / P3-n:
  P0 = active corruption, data loss, credential exposure, or false composition.
  P1 = likely destructive or unrecoverable release blocker.
  P2 = concrete correctness, recovery, compatibility, parity, or
       diagnosability defect.
  P3 = bounded robustness, coverage, maintainability, or documentation defect
       with a concrete consequence.
- Verdict is GO or NO-GO. NO-GO while any P0, P1, or P2 is open.
- Each finding: ONE root cause, exact location, violated invariant, credible
  reproduction or state/interleaving sequence, impact, required correction,
  and a closure/regression test. Separate independent root causes.
- Style preferences and speculative unease are not defects. Do not pad.
  Interface shape is not style: wrong command placement, a misleading name,
  a missing half of a lifecycle pair, or an option without a default is a
  finding (P2 or P3), on every axis.
- If your verdict is NO-GO but every blocking finding has a bounded,
  text-or-code-fixable remedy, you may pre-commit: "I pre-commit to GO on a
  revision that resolves {IDs} as specified." This makes the re-verdict cheap
  and is encouraged when honest.

## Report format

# {OBJECT} — {AXIS}-AXIS REVIEW

**Review object:** {object at exact SHA / doc path + status + date}
**Baseline:** {per-repo SHAs; note how sources were read, e.g. `git show HEAD:`}
**Date:** {date}
**Axis:** {one line: mandate}. Independent, adversarial, read-only. The other
axis runs in parallel; nothing here relies on it. Filed verbatim by the lane
owner.

**Verdict: {GO | NO-GO}** — {counts, e.g. "two P1 and three P2 findings
block"}. {If NO-GO and honest: pre-commit-to-GO clause naming the finding IDs.}

---

## 0. Evidence base
{What was actually read/run: files with line ranges, documents with sections,
commands with results. This section is what makes the verdict auditable.}

## 1. Findings
### [P1-1] {one-line root-cause title}
{Location · violated invariant · reproduction or state sequence · impact ·
remedy · closure test.}
{… one subsection per finding, severity-ordered. Omit section if none.}

## 2. Invariant analysis
{The invariants attacked and the evidence they held — attacks that FAILED are
part of the result; they are what a GO rests on.}

## 3. Risks and next action
{Residual risks below the finding bar; the single next action this verdict
implies.}

## Focused follow-up

Read GwzNoFallbackCheckpoint-RemPlan.md. Correction range core d20f7e7..dd47810 changes only review-axis requirements. Apply the actual State mandate, using your prior source inspection where valid. Verify the corrected entry gate and report any concrete remaining issue. Include a prior-finding closure table and changed-range analysis. Code reviewer also adjudicates original Consistency P2-1: text must require exact-tuple Code/State GO before edits. No implementation edits or source alignment have begun. No full repeated audit is needed where prior evidence remains valid.
