# GWZ Credential + Connection: joint design review package

Date: 2026-10-19. Status: **joint design accepted after one remediation round**. See
[merged verdict](GwzCredentialApiDesign-Verdict.md).

## Scope and authority

The operator requested review-loop review of the Credential API together with
Connection as a whole, to prevent conflated responsibilities and testing.
Only Credential design and joint review records may change. The accepted
Connection contract is a reference: required amendments must be identified
explicitly, not applied silently. No source, repository creation, native tests,
release, dependency changes, lane merges, disposal or push are authorized.

This is a new joint object, not a reopening of the accepted pure stream ordering
algorithm. Both axes must inspect the composition, endpoint-local authority,
provider state and teardown, and independent testing of small changes.

The same two prior Connection API reviewers are reused under the operator's
standing instruction. They retain earlier Connection context but remain
peer-blind for the current round. Both inherit the session model; no different
model-family independence is claimed. No third reviewer is permitted.

Two remediation rounds maximum; reviewers classify new architectural roots.
A third architectural root stops remediation for an operator decision. Accepted
Connection's earlier roots belong to its earlier object, not this new joint gate;
regressions of its accepted invariants remain in scope here.

Untracked anti-pattern/Git/convergence/route/SSH drafts and the earlier draft
archive are unrelated. Private evidence and other lanes are reference-only.
CurrentProgramCheckpoint was read and is historical/stale for this remediation;
this per-object package is the bounded live record instead of expanding it.

## Initial checkpoint

Root: `158a940b961941ebe0bd5c47c771dd9658156a04`. Credential file SHA-256: `a3cd847f1495b8f3d0a7b2f09c01cdf47a9e8fd718c1021c46aa4707ed9e8dcf`.

Only the Credential draft was committed for this checkpoint. Connection remains unchanged.

## Generated Consistency prompt

Generated from the canonical review-loop prompt template and the selected axis.

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
  nothing else. It will be filed verbatim as dev-docs/GwzCredentialApiDesign-ReviewConsistency.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: 158a940b961941ebe0bd5c47c771dd9658156a04
- gwz-core: 9e8edb80e730e86e44894d79d18cef551f70e6e5
- gwz-py: 5b6b66579867d96d21d5d5d003aaa69f50960b4e
- gwz-cli: 5193db962e672c54cd20ab2ef8448d904129a559
- gwz-git: a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf
- gwz-transport: a27817b9c86fd60a07a9e77d62b1f5e94a5cf252
- gwz-sspi: a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e
- git2-rs: d13951f7e0bfb6e0efcee1207ac5b140adefa455
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: Joint design review: GwzCredentialApiDesign.md at this tuple, composed with the ACCEPTED GwzConnectionApiContract.md at the same root. Judge the WHOLE credential/connection boundary, not Credential in isolation. This is a proposed API/design acceptance gate, not implementation or native qualification. An unspecified lifecycle that blocks coherent composition must be identified rather than assumed fixed later. Preserve the existing accepted Connection ordering/ownership findings and trace needed amendments explicitly; do not blindly redo its unrelated algorithm review.

- Controlling DRAFT document: dev-docs/GwzCredentialApiDesign.md at 158a940b961941ebe0bd5c47c771dd9658156a04
- Out of scope: All untracked anti-pattern/Git/convergence/route/SSH drafts and history snapshot; other lanes and private evidence runs; no modifications/builds/experiments.

AUTHORITY AND DEFERRALS
- Process authority: AGENTS.md, AGENTS_GWZ.md, dev-docs/AgentProcessRules.md and GwzProcessOptimization.md; review-loop skill already named. Do not change any files.
- Controlling documents to check the object against: dev-docs/GwzConnectionApiContract.md and its accepted verdict; dev-docs/GwzConnectionRemediationPlan.md; gwz-core/dev-docs/GwzRemoteTransportRequirements.md placement P1-P9 and later operator decisions; gwz-sspi public API/docs as needed. Trace cited earlier amendments where material, not an entire corpus audit.
- Original task authority: Operator wants fewer conflated connection/credential responsibilities and independently cheap tests for small changes. Operator selected a separate Credential repo as reasonable, requested the API document, then explicitly requested review-loop review WITH the current Connection library as one whole. Creation/source/refactor/release are not authorized. Do not treat repository creation or a provider rewrite as prerequisites without concrete necessity evidence.
- Requested deliverables and scope: Lane owner may edit Credential design and joint review/remediation records only. Connection contract is accepted/reference-only for this checkpoint: identify exact proposed amendments if needed rather than assume permission or consistency. Source repos/dependencies reference-only. Draft does not create any crate or freeze an unwritten implementation.
- Explicitly deferred (do not report as findings): Future physical carrier/iroh, arbitrary credential sources, new secret storage, remote credential lending. Current SSH/HTTPS placement and existing approved provider behavior are NOT deferred..
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
- Read Credential public API cold FIRST, then Connection API. Include Surface section: first-use build/prepare/authenticate/finish/cancel/close/drop, failure and defaults; no CLI help exists for unimplemented API.
- Trace dependency edges and ports: no reverse core/Git/Connection dependency, no duplicate admission/cleanup domains. Is provider-specific state/conversation mapping concrete enough for implementers?
- Test the claimed isolation and necessity against existing NativeHost/NativeCaller/Port, SSH OpenRequest, current helper/signing seams and original-entry capture. No native builds required; distinguish source feasibility from actual qualification.
- Identify what stays in network/trust adapters vs credential providers, especially SSH handshake signing, proof verification, HTTPS challenge selection/output and connection-bound state. Does scope select mandatory behavior or postpone the central boundary decision?
- Verify exact amendments/compatibility with accepted Connection §§3,7,8,10,13-15 and placement authority. Both APIs must compose as one real path.

- Both axes: label each NEW ARCHITECTURAL ROOT explicitly (reviewer classification). Use one finding per independent root, no speculative padding. Report exact joint composition gaps as P2 where concrete; no implementation evidence is claimed.

COMMANDS
Working directory /Volumes/projects/limbo/gwz-dev. Allowed: pwd; git [-C repo] rev-parse HEAD; git show SHA:path; git diff; git log; git status --short; rg, sed, cat, nl, ls, read-only Python for text/hashes. Verify all nine HEADs start/end. No builds/tests/network/mutations/new agents. Read pinned objects; do not read the other current-round report or untracked unrelated plans.

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

SURFACE SECTION — appended as a required section to one of the two axis
prompts (usually Consistency), never a third reviewer. The interface as the
person using it meets it. For this section, read FIRST and cold, before any
code or design or plan document: the object's `--help`
output at every level, its user-facing docs pages, and the existing command
families' `--help` for comparison. Attack: where each command sits against
the families that already exist (would a user look for it there?); names
and one-line summaries read cold (do they say what the thing does, to
someone who does not know the design?); lifecycle pairs (every install has
an uninstall, every create a remove, every write an undo — present, named
symmetrically, documented together); every option has a stated default;
then do the first-day walkthrough from `--help` alone — install it, use it
once, undo it, and report every point where you had to guess, could not
find the next command, or found no command at all. A defect here is what
ships forever; file it as P2 when it will need a compatibility break to
fix after release, P3 otherwise.
For this API, the cold read is the two public API sections, not nonexistent --help; record missing lifecycle/default semantics as relevant.

MANDATORY COMPLETE REPORT FORMAT
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

Generated from the canonical review-loop prompt template and the selected axis.

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
  nothing else. It will be filed verbatim as dev-docs/GwzCredentialApiDesign-ReviewSafety.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: 158a940b961941ebe0bd5c47c771dd9658156a04
- gwz-core: 9e8edb80e730e86e44894d79d18cef551f70e6e5
- gwz-py: 5b6b66579867d96d21d5d5d003aaa69f50960b4e
- gwz-cli: 5193db962e672c54cd20ab2ef8448d904129a559
- gwz-git: a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf
- gwz-transport: a27817b9c86fd60a07a9e77d62b1f5e94a5cf252
- gwz-sspi: a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e
- git2-rs: d13951f7e0bfb6e0efcee1207ac5b140adefa455
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: Joint design review: GwzCredentialApiDesign.md at this tuple, composed with the ACCEPTED GwzConnectionApiContract.md at the same root. Judge the WHOLE credential/connection boundary, not Credential in isolation. This is a proposed API/design acceptance gate, not implementation or native qualification. An unspecified lifecycle that blocks coherent composition must be identified rather than assumed fixed later. Preserve the existing accepted Connection ordering/ownership findings and trace needed amendments explicitly; do not blindly redo its unrelated algorithm review.

- Controlling DRAFT document: dev-docs/GwzCredentialApiDesign.md at 158a940b961941ebe0bd5c47c771dd9658156a04
- Out of scope: All untracked anti-pattern/Git/convergence/route/SSH drafts and history snapshot; other lanes and private evidence runs; no modifications/builds/experiments.

AUTHORITY AND DEFERRALS
- Process authority: AGENTS.md, AGENTS_GWZ.md, dev-docs/AgentProcessRules.md and GwzProcessOptimization.md; review-loop skill already named. Do not change any files.
- Controlling documents to check the object against: dev-docs/GwzConnectionApiContract.md and its accepted verdict; dev-docs/GwzConnectionRemediationPlan.md; gwz-core/dev-docs/GwzRemoteTransportRequirements.md placement P1-P9 and later operator decisions; gwz-sspi public API/docs as needed. Trace cited earlier amendments where material, not an entire corpus audit.
- Original task authority: Operator wants fewer conflated connection/credential responsibilities and independently cheap tests for small changes. Operator selected a separate Credential repo as reasonable, requested the API document, then explicitly requested review-loop review WITH the current Connection library as one whole. Creation/source/refactor/release are not authorized. Do not treat repository creation or a provider rewrite as prerequisites without concrete necessity evidence.
- Requested deliverables and scope: Lane owner may edit Credential design and joint review/remediation records only. Connection contract is accepted/reference-only for this checkpoint: identify exact proposed amendments if needed rather than assume permission or consistency. Source repos/dependencies reference-only. Draft does not create any crate or freeze an unwritten implementation.
- Explicitly deferred (do not report as findings): Future physical carrier/iroh, arbitrary credential sources, new secret storage, remote credential lending. Current SSH/HTTPS placement and existing approved provider behavior are NOT deferred..
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
- Trace a shared host with two operations/different caller authority, SSH explicit/ambient reuse proof and HTTP request-bound versus NTLM/Negotiate connection-bound auth, redirects/final TLS CBT and selected endpoint.
- Attack offered versus peer-authenticated reporting, cross-operation token/scope leakage, cancellation before/after native effect, lost/drop futures, owner close while session survives, retained cleanup reservations and physical pool retirement. Identify exact ownership transitions missing in either side.
- Check bounded provider inputs/secrets/results/conversations, trusted local output capability, native worker containment and launch-safety gate, fixed deadline/zero behavior. No invented arbitrary signing or fallback authority.
- Compare existing provider-port alternative and independent-test requirements; prevent scope inflation into a new supervisor/domain/network/auth framework. Promised reduced testing is not permission to waive relevant platform/composition checks.

- Both axes: label each NEW ARCHITECTURAL ROOT explicitly (reviewer classification). Use one finding per independent root, no speculative padding. Report exact joint composition gaps as P2 where concrete; no implementation evidence is claimed.

COMMANDS
Working directory /Volumes/projects/limbo/gwz-dev. Allowed: pwd; git [-C repo] rev-parse HEAD; git show SHA:path; git diff; git log; git status --short; rg, sed, cat, nl, ls, read-only Python for text/hashes. Verify all nine HEADs start/end. No builds/tests/network/mutations/new agents. Read pinned objects; do not read the other current-round report or untracked unrelated plans.

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

MANDATORY COMPLETE REPORT FORMAT
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


## Remediation round 1 checkpoint

Both initial axes returned NO-GO (two P2 each), converging on two architectural
roots. Reports are filed verbatim. The lane owner corrected one document in a
single patch; no source or accepted Connection contract changed.

Root: `c0fe3cf8185087a61780b8a61e347c95efc83c4f`. Reference member heads unchanged.
Credential SHA-256: `f729ed2b8298782d1648a5da0a2085810acf32ee92aff7361192a87823d28a5d`.

### Generated Consistency round-1 prompt

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
  nothing else. It will be filed verbatim as dev-docs/GwzCredentialApiDesign-ReviewConsistency-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: c0fe3cf8185087a61780b8a61e347c95efc83c4f
- gwz-core: 9e8edb80e730e86e44894d79d18cef551f70e6e5
- gwz-py: 5b6b66579867d96d21d5d5d003aaa69f50960b4e
- gwz-cli: 5193db962e672c54cd20ab2ef8448d904129a559
- gwz-git: a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf
- gwz-transport: a27817b9c86fd60a07a9e77d62b1f5e94a5cf252
- gwz-sspi: a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e
- git2-rs: d13951f7e0bfb6e0efcee1207ac5b140adefa455
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: Joint design review: GwzCredentialApiDesign.md at this tuple, composed with the ACCEPTED GwzConnectionApiContract.md at the same root. Judge the WHOLE credential/connection boundary, not Credential in isolation. This is a proposed API/design acceptance gate, not implementation or native qualification. An unspecified lifecycle that blocks coherent composition must be identified rather than assumed fixed later. Preserve the existing accepted Connection ordering/ownership findings and trace needed amendments explicitly; do not blindly redo its unrelated algorithm review.

- Controlling DRAFT document: dev-docs/GwzCredentialApiDesign.md at c0fe3cf8185087a61780b8a61e347c95efc83c4f
- Out of scope: All untracked anti-pattern/Git/convergence/route/SSH drafts and history snapshot; other lanes and private evidence runs; no modifications/builds/experiments.

AUTHORITY AND DEFERRALS
- Process authority: AGENTS.md, AGENTS_GWZ.md, dev-docs/AgentProcessRules.md and GwzProcessOptimization.md; review-loop skill already named. Do not change any files.
- Controlling documents to check the object against: dev-docs/GwzConnectionApiContract.md and its accepted verdict; dev-docs/GwzConnectionRemediationPlan.md; gwz-core/dev-docs/GwzRemoteTransportRequirements.md placement P1-P9 and later operator decisions; gwz-sspi public API/docs as needed. Trace cited earlier amendments where material, not an entire corpus audit.
- Original task authority: Operator wants fewer conflated connection/credential responsibilities and independently cheap tests for small changes. Operator selected a separate Credential repo as reasonable, requested the API document, then explicitly requested review-loop review WITH the current Connection library as one whole. Creation/source/refactor/release are not authorized. Do not treat repository creation or a provider rewrite as prerequisites without concrete necessity evidence.
- Requested deliverables and scope: Lane owner may edit Credential design and joint review/remediation records only. Connection contract is accepted/reference-only for this checkpoint: identify exact proposed amendments if needed rather than assume permission or consistency. Source repos/dependencies reference-only. Draft does not create any crate or freeze an unwritten implementation.
- Explicitly deferred (do not report as findings): Future physical carrier/iroh, arbitrary credential sources, new secret storage, remote credential lending. Current SSH/HTTPS placement and existing approved provider behavior are NOT deferred..
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
- Read Credential public API cold FIRST, then Connection API. Include Surface section: first-use build/prepare/authenticate/finish/cancel/close/drop, failure and defaults; no CLI help exists for unimplemented API.
- Trace dependency edges and ports: no reverse core/Git/Connection dependency, no duplicate admission/cleanup domains. Is provider-specific state/conversation mapping concrete enough for implementers?
- Test the claimed isolation and necessity against existing NativeHost/NativeCaller/Port, SSH OpenRequest, current helper/signing seams and original-entry capture. No native builds required; distinguish source feasibility from actual qualification.
- Identify what stays in network/trust adapters vs credential providers, especially SSH handshake signing, proof verification, HTTPS challenge selection/output and connection-bound state. Does scope select mandatory behavior or postpone the central boundary decision?
- Verify exact amendments/compatibility with accepted Connection §§3,7,8,10,13-15 and placement authority. Both APIs must compose as one real path.

- Both axes: label each NEW ARCHITECTURAL ROOT explicitly (reviewer classification). Use one finding per independent root, no speculative padding. Report exact joint composition gaps as P2 where concrete; no implementation evidence is claimed.

COMMANDS
Working directory /Volumes/projects/limbo/gwz-dev. Allowed: pwd; git [-C repo] rev-parse HEAD; git show SHA:path; git diff; git log; git status --short; rg, sed, cat, nl, ls, read-only Python for text/hashes. Verify all nine HEADs start/end. No builds/tests/network/mutations/new agents. Read pinned objects; do not read the other current-round report or untracked unrelated plans.

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

SURFACE SECTION — appended as a required section to one of the two axis
prompts (usually Consistency), never a third reviewer. The interface as the
person using it meets it. For this section, read FIRST and cold, before any
code or design or plan document: the object's `--help`
output at every level, its user-facing docs pages, and the existing command
families' `--help` for comparison. Attack: where each command sits against
the families that already exist (would a user look for it there?); names
and one-line summaries read cold (do they say what the thing does, to
someone who does not know the design?); lifecycle pairs (every install has
an uninstall, every create a remove, every write an undo — present, named
symmetrically, documented together); every option has a stated default;
then do the first-day walkthrough from `--help` alone — install it, use it
once, undo it, and report every point where you had to guess, could not
find the next command, or found no command at all. A defect here is what
ships forever; file it as P2 when it will need a compatibility break to
fix after release, P3 otherwise.
For this API, the cold read is the two public API sections, not nonexistent --help; record missing lifecycle/default semantics as relevant.

MANDATORY COMPLETE REPORT FORMAT
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

REMEDIATION ROUND 1 (same reviewer, context retained)
Read dev-docs/GwzCredentialApiDesign-RemPlan-1.md at the new root. Prior-round reports (both axes) are legitimate shared inputs; never read the other current-round report. Inspect the entire diff from 158a940b961941ebe0bd5c47c771dd9658156a04 to c0fe3cf8185087a61780b8a61e347c95efc83c4f -- dev-docs/GwzCredentialApiDesign.md.
Re-run your original counterexamples against the revised joint contract, with a prior-finding closure table and changed-range analysis before section 0. No executed code proof is claimed. One of two remediation rounds is used. Two architectural roots have been identified on this new joint object (terminal/accounting; operation/provider mapping). Explicit-key handoff is part of the second root. Explicitly classify each NEW ARCHITECTURAL root: a third new root stops the lane; do not conflate bounded refinements of the original two roots with independent new architecture. No implementation/native tests or upgrades are authorized.
The same pair continues under the operator's instruction to use old reviewers, overriding fresh-replacement guidance. This is not additional reviewer headcount.

```

### Generated Safety round-1 prompt

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
  nothing else. It will be filed verbatim as dev-docs/GwzCredentialApiDesign-ReviewSafety-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: c0fe3cf8185087a61780b8a61e347c95efc83c4f
- gwz-core: 9e8edb80e730e86e44894d79d18cef551f70e6e5
- gwz-py: 5b6b66579867d96d21d5d5d003aaa69f50960b4e
- gwz-cli: 5193db962e672c54cd20ab2ef8448d904129a559
- gwz-git: a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf
- gwz-transport: a27817b9c86fd60a07a9e77d62b1f5e94a5cf252
- gwz-sspi: a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e
- git2-rs: d13951f7e0bfb6e0efcee1207ac5b140adefa455
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: Joint design review: GwzCredentialApiDesign.md at this tuple, composed with the ACCEPTED GwzConnectionApiContract.md at the same root. Judge the WHOLE credential/connection boundary, not Credential in isolation. This is a proposed API/design acceptance gate, not implementation or native qualification. An unspecified lifecycle that blocks coherent composition must be identified rather than assumed fixed later. Preserve the existing accepted Connection ordering/ownership findings and trace needed amendments explicitly; do not blindly redo its unrelated algorithm review.

- Controlling DRAFT document: dev-docs/GwzCredentialApiDesign.md at c0fe3cf8185087a61780b8a61e347c95efc83c4f
- Out of scope: All untracked anti-pattern/Git/convergence/route/SSH drafts and history snapshot; other lanes and private evidence runs; no modifications/builds/experiments.

AUTHORITY AND DEFERRALS
- Process authority: AGENTS.md, AGENTS_GWZ.md, dev-docs/AgentProcessRules.md and GwzProcessOptimization.md; review-loop skill already named. Do not change any files.
- Controlling documents to check the object against: dev-docs/GwzConnectionApiContract.md and its accepted verdict; dev-docs/GwzConnectionRemediationPlan.md; gwz-core/dev-docs/GwzRemoteTransportRequirements.md placement P1-P9 and later operator decisions; gwz-sspi public API/docs as needed. Trace cited earlier amendments where material, not an entire corpus audit.
- Original task authority: Operator wants fewer conflated connection/credential responsibilities and independently cheap tests for small changes. Operator selected a separate Credential repo as reasonable, requested the API document, then explicitly requested review-loop review WITH the current Connection library as one whole. Creation/source/refactor/release are not authorized. Do not treat repository creation or a provider rewrite as prerequisites without concrete necessity evidence.
- Requested deliverables and scope: Lane owner may edit Credential design and joint review/remediation records only. Connection contract is accepted/reference-only for this checkpoint: identify exact proposed amendments if needed rather than assume permission or consistency. Source repos/dependencies reference-only. Draft does not create any crate or freeze an unwritten implementation.
- Explicitly deferred (do not report as findings): Future physical carrier/iroh, arbitrary credential sources, new secret storage, remote credential lending. Current SSH/HTTPS placement and existing approved provider behavior are NOT deferred..
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
- Trace a shared host with two operations/different caller authority, SSH explicit/ambient reuse proof and HTTP request-bound versus NTLM/Negotiate connection-bound auth, redirects/final TLS CBT and selected endpoint.
- Attack offered versus peer-authenticated reporting, cross-operation token/scope leakage, cancellation before/after native effect, lost/drop futures, owner close while session survives, retained cleanup reservations and physical pool retirement. Identify exact ownership transitions missing in either side.
- Check bounded provider inputs/secrets/results/conversations, trusted local output capability, native worker containment and launch-safety gate, fixed deadline/zero behavior. No invented arbitrary signing or fallback authority.
- Compare existing provider-port alternative and independent-test requirements; prevent scope inflation into a new supervisor/domain/network/auth framework. Promised reduced testing is not permission to waive relevant platform/composition checks.

- Both axes: label each NEW ARCHITECTURAL ROOT explicitly (reviewer classification). Use one finding per independent root, no speculative padding. Report exact joint composition gaps as P2 where concrete; no implementation evidence is claimed.

COMMANDS
Working directory /Volumes/projects/limbo/gwz-dev. Allowed: pwd; git [-C repo] rev-parse HEAD; git show SHA:path; git diff; git log; git status --short; rg, sed, cat, nl, ls, read-only Python for text/hashes. Verify all nine HEADs start/end. No builds/tests/network/mutations/new agents. Read pinned objects; do not read the other current-round report or untracked unrelated plans.

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

MANDATORY COMPLETE REPORT FORMAT
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

REMEDIATION ROUND 1 (same reviewer, context retained)
Read dev-docs/GwzCredentialApiDesign-RemPlan-1.md at the new root. Prior-round reports (both axes) are legitimate shared inputs; never read the other current-round report. Inspect the entire diff from 158a940b961941ebe0bd5c47c771dd9658156a04 to c0fe3cf8185087a61780b8a61e347c95efc83c4f -- dev-docs/GwzCredentialApiDesign.md.
Re-run your original counterexamples against the revised joint contract, with a prior-finding closure table and changed-range analysis before section 0. No executed code proof is claimed. One of two remediation rounds is used. Two architectural roots have been identified on this new joint object (terminal/accounting; operation/provider mapping). Explicit-key handoff is part of the second root. Explicitly classify each NEW ARCHITECTURAL root: a third new root stops the lane; do not conflate bounded refinements of the original two roots with independent new architecture. No implementation/native tests or upgrades are authorized.
The same pair continues under the operator's instruction to use old reviewers, overriding fresh-replacement guidance. This is not additional reviewer headcount.

```


## Final outcome

Both axes returned GO on root `c0fe3cf8185087a61780b8a61e347c95efc83c4f`, with zero open P0–P3.
Four initial P2 findings, two architectural roots, one correction round; no new
root in re-review. Design-only acceptance; no implementation or native evidence.
