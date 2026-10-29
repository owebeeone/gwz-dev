# GWZ Connection remediation plan — draft review package

Date: 2026-10-19. Status: **CLOSED — Consistency GO and Safety GO on the pinned
draft, initial round; zero findings and zero remediation rounds. No API freeze
or implementation GO.** See [verdict](GwzConnectionRemediationPlan-Verdict.md).

## Scope and original authority

Scope: judge whether this DRAFT REMEDIATION PLAN is a sound, bounded route to a later reviewed API and small proof. This is NOT an interface freeze or implementation/release gate. No concrete public Rust signatures are frozen here; identify plan defects that cause wrong decisions, unsafe permissions or an unbounded/unsatisfiable implementation sequence. Do not demand implementation or a finished API merely to review a plan. Do not exempt an unsafe proposed rule just because the label says DRAFT.
Original operator authority: four documents requested after a read-only audit. The requested communication library is Git-independent, handles SSH and HTTP/HTTPS, is independently testable without the rest of GWZ, and is consumed by the Git interface. Authentication/key/certificate sources and package/repository placement must be decided explicitly. Operator requires a structural fix making ownership mistakes harder, restoration of gwz-git consumption, useful CLI/Python reuse in the transport release, and no quiet further deferrals. They explicitly do not require a physical wire carrier or whole core server/session programme for in-process embedding. This review is the user's instruction 'Run the review loop on the gwzonnection doc'. Only the Connection draft and its review records may be edited by lane owner. Reviewers are READ-ONLY. No source, dependency, member, protocol implementation or adjacent-plan changes are authorized.
Archive policy: previously controlling docs remain reference authority under history/reference or history/parked; historical dangling links are intentionally not repaired. Read actual archived paths when needed. Do not file a finding merely for an old dangling archival citation. Core canonical requirements/design and root crate-map constraints still apply unless explicitly amended. Open decisions in this plan are design outputs, not permission for implementation.

## Pinned object and tuple

Object: `dev-docs/GwzConnectionRemediationPlan.md` at root `468975076f94cd761aa07ba4b47c2c4af797a892`. SHA-256: `6335e68c5058095b890246079e1ad814788d581d9e70f1d7d7c356345d288403`.

- `.`: `468975076f94cd761aa07ba4b47c2c4af797a892`
- `gwz-core`: `9e8edb80e730e86e44894d79d18cef551f70e6e5`
- `gwz-py`: `5b6b66579867d96d21d5d5d003aaa69f50960b4e`
- `gwz-cli`: `5193db962e672c54cd20ab2ef8448d904129a559`
- `gwz-git`: `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf`
- `gwz-transport`: `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252`
- `gwz-sspi`: `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e`
- `git2-rs`: `d13951f7e0bfb6e0efcee1207ac5b140adefa455`
- `libgit2`: `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`

Only this root document is being reviewed. Other repos are pinned reference-only inputs.
The three other new plans and existing untracked documents are out of scope.
No builds, native experiments or infrastructure work are authorized by this package.

## Review checkpoint and bounds

Dual draft-plan review: Consistency and Safety. Surface applicability is folded
into Consistency; a concrete API Surface freeze remains a later contract gate.
Two isolated reviewers, same reviewers for text-only remediation, at most two
remediation rounds; third architectural-root stop rule applies. The review-loop
skill's canonical prompt template generated the prompts below. No third reviewer.
Reviewers inherit the session model offered by this harness; no cross-family
review model is available in the exposed subagent catalog. Independence is by
isolated context and peer blindness; cross-family diversity is not claimed.

This package is the per-object checkpoint; the older aggregate checkpoint remains
historical context and is already oversized. Do not expand it for this new review.
Final result and round count are recorded here. Filing review outputs is part of
the explicitly requested loop; existing oversized documents remain separate debt.

## Generated Consistency prompt

```text
You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: Consistency
AXIS: CONSISTENCY — the document against its controlling graph.
Attack: internal contradictions between sections; agreement with every
controlling contract/design it cites (verify quotes verbatim at the cited
lines); exactness of superseded-clause lists; whether its own test/evidence
sections are satisfiable as written; unstated impacts on documents it does
not cite.
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzConnectionRemediationPlan-ReviewConsistency.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: 468975076f94cd761aa07ba4b47c2c4af797a892
- gwz-core: 9e8edb80e730e86e44894d79d18cef551f70e6e5
- gwz-py: 5b6b66579867d96d21d5d5d003aaa69f50960b4e
- gwz-cli: 5193db962e672c54cd20ab2ef8448d904129a559
- gwz-git: a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf
- gwz-transport: a27817b9c86fd60a07a9e77d62b1f5e94a5cf252
- gwz-sspi: a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e
- git2-rs: d13951f7e0bfb6e0efcee1207ac5b140adefa455
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: dev-docs/GwzConnectionRemediationPlan.md at root 468975076f94cd761aa07ba4b47c2c4af797a892
- Controlling DRAFT document: dev-docs/GwzConnectionRemediationPlan.md at 468975076f94cd761aa07ba4b47c2c4af797a892
- Out of scope: Three other remediation drafts, pre-existing untracked SSH/route drafts, private evidence dirt and authorized review package/reports. All source is reference-only and pinned; no implementation is under review.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md; review-loop SKILL.md read at dispatch.
- Controlling documents to check the object against: Root dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; current root/member AGENTS.md and AGENTS_GWZ.md. Read CurrentProgramCheckpoint.md current entries as historical context, not an accurate October-19 source inventory. Target root document's predecessor GwzTransportReleaseRemediationPlan.md; GwzConnectionReuseDesign.md; GwzCoreSessionCrateMap.md; GwzLocalCloneLibraryBoundaries.md. Core dev-docs/GWZRequirements.md, GWZDesign.md, GwzRemoteTransportRequirements.md, GwzRemoteTransportDesign.md, GwzGitLibraryDesign.md, GwzGitLibraryApi.md, GwzTransportTR18-OperatorDecisions.md. Relevant incorporated core history/reference transport amendments if needed. Other three uncommitted remediation plans are explicitly OUT OF REVIEW; do not depend on their working-tree prose to make the reviewed Connection plan coherent.
- Original task authority: Scope: judge whether this DRAFT REMEDIATION PLAN is a sound, bounded route to a later reviewed API and small proof. This is NOT an interface freeze or implementation/release gate. No concrete public Rust signatures are frozen here; identify plan defects that cause wrong decisions, unsafe permissions or an unbounded/unsatisfiable implementation sequence. Do not demand implementation or a finished API merely to review a plan. Do not exempt an unsafe proposed rule just because the label says DRAFT.
Original operator authority: four documents requested after a read-only audit. The requested communication library is Git-independent, handles SSH and HTTP/HTTPS, is independently testable without the rest of GWZ, and is consumed by the Git interface. Authentication/key/certificate sources and package/repository placement must be decided explicitly. Operator requires a structural fix making ownership mistakes harder, restoration of gwz-git consumption, useful CLI/Python reuse in the transport release, and no quiet further deferrals. They explicitly do not require a physical wire carrier or whole core server/session programme for in-process embedding. This review is the user's instruction 'Run the review loop on the gwzonnection doc'. Only the Connection draft and its review records may be edited by lane owner. Reviewers are READ-ONLY. No source, dependency, member, protocol implementation or adjacent-plan changes are authorized.
Archive policy: previously controlling docs remain reference authority under history/reference or history/parked; historical dangling links are intentionally not repaired. Read actual archived paths when needed. Do not file a finding merely for an old dangling archival citation. Core canonical requirements/design and root crate-map constraints still apply unless explicitly amended. Open decisions in this plan are design outputs, not permission for implementation.
- Requested deliverables and scope: Connection plan review, bounded text-only remediation and same-axis re-review. Dual Consistency/Safety, maximum two reviewers and two remediation rounds. No implementation authorization.
- Explicitly deferred (do not report as findings): No operator-required outcome may be silently deferred. Exact API signatures, repository selection and native adapter implementation are explicit later design/implementation outputs, not requirements to already exist for this draft-plan review..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.
  Only traceable operator decisions are protected: an agent-added prerequisite
  labeled "settled", or an earlier GO, cannot exempt it from the checks below.

COMMON CHECK — PROBLEM, NECESSITY, AND AUTHORIZED SCOPE (every axis)
Complete this check before approving the solution. If assigned the Surface
mandate, read the public interface cold FIRST, then perform this check.
- Identify the requested outcome and concrete observed problem. Trace blocking
  prerequisites and scope-changing constraints to original task authority,
  applicable rules, or demonstrated necessity. Separate preferences and
  assumptions from requirements; a generated plan, test, or prior review is
  not authority for expanded scope.
- Before accepting dependency or infrastructure changes as prerequisites,
  examine a small feasibility probe or concrete evidence about the existing
  supported route. Does the chosen approach fail, or is the actual requested
  outcome impossible through that route? Revisit self-imposed constraints
  without silently relaxing user requirements.
- Does the remedy address the observed problem? Consider supported alternatives
  and localized fixes before accepting broader work. Distinguish a confirmed
  blocker from a limitation, hypothesis, or optimization opportunity. Do not
  demand minimum line count or proof of every routine in-scope engineering choice.
- Verify component-level authorization, including internal changes that keep
  public APIs unchanged. Out-of-scope discoveries belong in separate follow-up
  proposals with observation, evidence, impact, suggested improvement, and
  whether they demonstrably block this outcome; discovery is not permission.
- Record evidence and conclusions in the required report section. A concrete
  unauthorized expansion or unsupported prerequisite that blocks delivery is
  P2, or higher if warranted by impact. If missing authority or evidence prevents
  establishing necessity or scope, identify the specific gap instead of assuming
  a mandate. Optional external shortcomings alone do not block this deliverable.

REVIEW AREAS
- Inspect section 3 dependency/responsibility graph against core crate-map/library-boundary rules, gwz-git purposes, pure gwz-transport charter and actual existing interfaces.
- Sections 4-5: does the plan preserve approved SSH identity/config/reuse, auth mechanism, fixed deadline and secret/protocol placement contracts? Do claimed required amendments and open decisions allow a coherent later contract without silently expanding scope?
- Sections 6-8: can a small independently tested reuse proof precede broad migration under the written sequence? Is a whole session, new crate/repo or dependency route accidentally made prerequisite without evidence?
- Verify observed Python per-operation ownership and no need for a new physical carrier against current source.
- Read the proposed API naming/lifecycle cold as a PLAN surface check; report misleading lifecycle authority or implied guarantees, but no fabricated --help or hypothetical public API freeze. Record that full API Surface review remains for contract phase.
- Include common problem/necessity/authorization check and classify any architectural root cause explicitly. Concrete issue reproductions should be small traces or contradictory clauses, not demands to implement source now.

COMMANDS
Working directory /Volumes/projects/limbo/gwz-dev. Inspection only: pwd; git rev-parse HEAD and git -C REPO rev-parse HEAD for each tuple repo at start/end; git show SHA:path; git log; git diff; git status; rg/rg --files (search members directly, ignored by root searches); cat/sed/nl; Python standard-library read-only inspection. No writes, builds, tests, network or host operations. Read object via git show pinned-root-SHA:dev-docs/GwzConnectionRemediationPlan.md. Check exact source claims with pinned git show in corresponding repo. Do not read any current-round review reports or peer messages. Keep investigation bounded to consequential plan clauses and their immediate authority/source evidence; optional external defects go into risks with nonblocking scope classification.

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

MANDATED FINAL REPORT TEMPLATE
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

## Problem, requirement provenance, and scope
{Required on every axis: requested outcome and original authority; observed
problem; source and necessity evidence for blocking prerequisites or scope
changes (or "none"); supported alternatives / localized remedies considered;
authorized editable components and reference-only dependencies; conclusion and
any specific evidence gaps, linked to finding IDs where blocking. Record
optional external shortcomings separately from current-deliverable blockers.}

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
```

## Generated Safety prompt

```text
You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: Safety
AXIS: SAFETY — what the text permits to go wrong.
Attack: degraded and mixed-version paths; irreversible steps and their
preconditions; disclosure/privacy scale; stuck states reachable under the
text's own rules; whether "never worse than the status quo" claims survive
concrete interleavings; scope creep that widens blast radius.
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzConnectionRemediationPlan-ReviewSafety.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: 468975076f94cd761aa07ba4b47c2c4af797a892
- gwz-core: 9e8edb80e730e86e44894d79d18cef551f70e6e5
- gwz-py: 5b6b66579867d96d21d5d5d003aaa69f50960b4e
- gwz-cli: 5193db962e672c54cd20ab2ef8448d904129a559
- gwz-git: a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf
- gwz-transport: a27817b9c86fd60a07a9e77d62b1f5e94a5cf252
- gwz-sspi: a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e
- git2-rs: d13951f7e0bfb6e0efcee1207ac5b140adefa455
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: dev-docs/GwzConnectionRemediationPlan.md at root 468975076f94cd761aa07ba4b47c2c4af797a892
- Controlling DRAFT document: dev-docs/GwzConnectionRemediationPlan.md at 468975076f94cd761aa07ba4b47c2c4af797a892
- Out of scope: Three other remediation drafts, pre-existing untracked SSH/route drafts, private evidence dirt and authorized review package/reports. All source is reference-only and pinned; no implementation is under review.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md; review-loop SKILL.md read at dispatch.
- Controlling documents to check the object against: Root dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; current root/member AGENTS.md and AGENTS_GWZ.md. Read CurrentProgramCheckpoint.md current entries as historical context, not an accurate October-19 source inventory. Target root document's predecessor GwzTransportReleaseRemediationPlan.md; GwzConnectionReuseDesign.md; GwzCoreSessionCrateMap.md; GwzLocalCloneLibraryBoundaries.md. Core dev-docs/GWZRequirements.md, GWZDesign.md, GwzRemoteTransportRequirements.md, GwzRemoteTransportDesign.md, GwzGitLibraryDesign.md, GwzGitLibraryApi.md, GwzTransportTR18-OperatorDecisions.md. Relevant incorporated core history/reference transport amendments if needed. Other three uncommitted remediation plans are explicitly OUT OF REVIEW; do not depend on their working-tree prose to make the reviewed Connection plan coherent.
- Original task authority: Scope: judge whether this DRAFT REMEDIATION PLAN is a sound, bounded route to a later reviewed API and small proof. This is NOT an interface freeze or implementation/release gate. No concrete public Rust signatures are frozen here; identify plan defects that cause wrong decisions, unsafe permissions or an unbounded/unsatisfiable implementation sequence. Do not demand implementation or a finished API merely to review a plan. Do not exempt an unsafe proposed rule just because the label says DRAFT.
Original operator authority: four documents requested after a read-only audit. The requested communication library is Git-independent, handles SSH and HTTP/HTTPS, is independently testable without the rest of GWZ, and is consumed by the Git interface. Authentication/key/certificate sources and package/repository placement must be decided explicitly. Operator requires a structural fix making ownership mistakes harder, restoration of gwz-git consumption, useful CLI/Python reuse in the transport release, and no quiet further deferrals. They explicitly do not require a physical wire carrier or whole core server/session programme for in-process embedding. This review is the user's instruction 'Run the review loop on the gwzonnection doc'. Only the Connection draft and its review records may be edited by lane owner. Reviewers are READ-ONLY. No source, dependency, member, protocol implementation or adjacent-plan changes are authorized.
Archive policy: previously controlling docs remain reference authority under history/reference or history/parked; historical dangling links are intentionally not repaired. Read actual archived paths when needed. Do not file a finding merely for an old dangling archival citation. Core canonical requirements/design and root crate-map constraints still apply unless explicitly amended. Open decisions in this plan are design outputs, not permission for implementation.
- Requested deliverables and scope: Connection plan review, bounded text-only remediation and same-axis re-review. Dual Consistency/Safety, maximum two reviewers and two remediation rounds. No implementation authorization.
- Explicitly deferred (do not report as findings): No operator-required outcome may be silently deferred. Exact API signatures, repository selection and native adapter implementation are explicit later design/implementation outputs, not requirements to already exist for this draft-plan review..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.
  Only traceable operator decisions are protected: an agent-added prerequisite
  labeled "settled", or an earlier GO, cannot exempt it from the checks below.

COMMON CHECK — PROBLEM, NECESSITY, AND AUTHORIZED SCOPE (every axis)
Complete this check before approving the solution. If assigned the Surface
mandate, read the public interface cold FIRST, then perform this check.
- Identify the requested outcome and concrete observed problem. Trace blocking
  prerequisites and scope-changing constraints to original task authority,
  applicable rules, or demonstrated necessity. Separate preferences and
  assumptions from requirements; a generated plan, test, or prior review is
  not authority for expanded scope.
- Before accepting dependency or infrastructure changes as prerequisites,
  examine a small feasibility probe or concrete evidence about the existing
  supported route. Does the chosen approach fail, or is the actual requested
  outcome impossible through that route? Revisit self-imposed constraints
  without silently relaxing user requirements.
- Does the remedy address the observed problem? Consider supported alternatives
  and localized fixes before accepting broader work. Distinguish a confirmed
  blocker from a limitation, hypothesis, or optimization opportunity. Do not
  demand minimum line count or proof of every routine in-scope engineering choice.
- Verify component-level authorization, including internal changes that keep
  public APIs unchanged. Out-of-scope discoveries belong in separate follow-up
  proposals with observation, evidence, impact, suggested improvement, and
  whether they demonstrably block this outcome; discovery is not permission.
- Record evidence and conclusions in the required report section. A concrete
  unauthorized expansion or unsupported prerequisite that blocks delivery is
  P2, or higher if warranted by impact. If missing authority or evidence prevents
  establishing necessity or scope, identify the specific gap instead of assuming
  a mandate. Optional external shortcomings alone do not block this deliverable.

REVIEW AREAS
- Sections 4-5 ownership: cancellation/registration provenance, lease failure and uncertain cleanup, deadlines, authentication/config changes, native worker and process-safety boundaries. Hunt unsafe allowed interleavings or scope gaps, using source only to substantiate them.
- Does long-lived reuse plan require finite admission/results/identifier turnover and reject stale traffic without importing the whole application session host? Can independent operation cancellation coexist with shared physical resource failure under the text?
- Secret/trust/auth interfaces and snapshot placement: identify what the plan wrongly permits, distinguishing explicitly gated future decisions from an unsafe contract already prescribed.
- HTTP/SSH reuse meaning and message stream semantics: connection vs operation vs request/stream cleanup, replay/partial failure consequences; no assumption that stream success is Git success.
- Independent test/real-consumer acceptance: are the tests satisfiable and likely to catch the known lifetime failure, cross-client native hazard and sustained bound? Avoid requiring every possible platform test merely for plan acceptance.
- Include common problem/necessity/authorization check and classify any architectural root cause explicitly. No runtime or release GO is sought.

COMMANDS
Working directory /Volumes/projects/limbo/gwz-dev. Inspection only: pwd; git rev-parse HEAD and git -C REPO rev-parse HEAD for each tuple repo at start/end; git show SHA:path; git log; git diff; git status; rg/rg --files (search members directly, ignored by root searches); cat/sed/nl; Python standard-library read-only inspection. No writes, builds, tests, network or host operations. Read object via git show pinned-root-SHA:dev-docs/GwzConnectionRemediationPlan.md. Check exact source claims with pinned git show in corresponding repo. Do not read any current-round review reports or peer messages. Keep investigation bounded to consequential plan clauses and their immediate authority/source evidence; optional external defects go into risks with nonblocking scope classification.

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

MANDATED FINAL REPORT TEMPLATE
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

## Problem, requirement provenance, and scope
{Required on every axis: requested outcome and original authority; observed
problem; source and necessity evidence for blocking prerequisites or scope
changes (or "none"); supported alternatives / localized remedies considered;
authorized editable components and reference-only dependencies; conclusion and
any specific evidence gaps, linked to finding IDs where blocking. Record
optional external shortcomings separately from current-deliverable blockers.}

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
```
