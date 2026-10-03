RE-VERDICT: remediation round 1, continuing original reviewer per operator preference.
Verify updated tuple at start/end. Return complete concise standalone report with
prior-finding closure table and changed-range analysis; retrace each ORIGINAL
counterexample, not merely correction claims. Classify NEW architectural roots.
No file writes/current-peer-report reads. Prior-round reports are legitimate.
Merged plan: dev-docs/GwzSspiDesign-RemPlan-1.md at updated root tuple.
Read ALL revised package documents and complete changed range 377c5e2..f2029e4
against unchanged core/evidence graph. Address all your P2/P3 findings; no new
implementation/provider proof is claimed. Full Windows still NO-GO separately.

You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: Safety
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzSspiDesign-ReviewSafety-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: f2029e4b1739c0214138675dfb16abdb44f6a0d7; gwz-core d78a664e3c5a325c6f12be409eb7645c1c1b51d0; evidence 1beb1d204c824701ddbd033c7f89df9a3561f5e5 settled HEAD tuple
- Object: DRAFT SSPI design package at root f2029e4, core d78a664; this axis reads the full controlling graph
- Controlling DRAFT document: dev-docs/GwzSspiDesign.md at f2029e4b1739c0214138675dfb16abdb44f6a0d7
- Out of scope: Untracked old root SSH N2b prompts/route mapping; core bug report; evidence alpha timeout run; other lanes; no implementation

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md and /Users/owebeeone/.claude/skills/review-loop/SKILL.md
- Controlling documents to check the object against: dev-docs/GwzSspiDesign.md, GwzSspiPlan.md, GwzSspiCallerGuide-DRAFT.md, GwzCoreSessionCrateMap.md, GwzLocalCloneLibraryBoundaries.md; core dev-docs/GWZDesign.md, GWZRequirements.md, GwzTransportWindowsParityDesign.md explicit amendment, GwzTransportCredentialHelpersDesign.md, GwzTransportCredentialHelperTimingAmendment.md, GwzTransportSshHelperClockAmendment.md, GwzTransportCredentialHelperConfigurationViewAmendment.md; native reports linked by design §7. Imported Windows old tuples are historical, not current authority.
- Explicitly deferred (do not report as findings): Full Windows provider/interactive SSO/Digest/EPA/trust/proxy/Pageant qualification, product activation and repository creation/implementation. Do not waive mechanism identity/containment/cleanup/API shape..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
AXIS: SAFETY — what the text permits to go wrong.
Attack: degraded and mixed-version paths; irreversible steps and their
preconditions; disclosure/privacy scale; stuck states reachable under the
text's own rules; whether "never worse than the status quo" claims survive
concrete interleavings; scope creep that widens blast radius.
Attack bounded ownership and capacity, startup/crash/kill, private IPC and secret codec contract, actual identity/CBT, cancellation vs completion, deadline and helper provenance, exact supersession/scope gates, host installation/lifecycle. Classify architectural root causes explicitly. This axis reads the full design and controlling graph; no peer reports. Package is not implemented; test text permits rather than absent code.

COMMANDS
cwd /Volumes/projects/limbo/gwz-dev. Allowed: git rev-parse HEAD; git -C gwz-core rev-parse HEAD; git -C gwz-core-evidence rev-parse HEAD; git show <SHA>:<allowed-path>; git diff 377c5e2..f2029e4 -- <allowed-root-path>; git -C gwz-core diff ba2df3b..d78a664 -- <allowed-core-path>; rg/sed/nl/read-only Python of allowed documents; primary Microsoft sources as needed. Read committed bytes, verify all three HEADs at start/end. No writes/builds/tests/mutations.

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
  revision that resolves finding IDs as specified." This makes the re-verdict cheap
  and is encouraged when honest.

REPORT TEMPLATE (return filled complete report):
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
