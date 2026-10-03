You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: Surface
AXIS: SURFACE — the interface as the person using it meets it.
You read NO code and NO design or plan document: only the object's `--help`
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
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzSspiHttpsComposition-ReviewSurface.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- gwz-dev HEAD fe40ba9b23e31da95358441ae5214a7cadb31b31
- gwz-core HEAD 8cb3a3f01d79699a5ad07b6ec7cfc78321224d31
- gwz-sspi HEAD 616e32cceeea1b7df1d7bbe1c1695a409a733f6d
- gwz-cli HEAD 0c7dfaf0199731648d2360358284010b2b4575c1
- gwz-py HEAD ded47130af23720099e7b6a92ccb9a161bb5db9a
- gwz-transport HEAD 1aab733783e06b25cb5d2321d71ec0b34417a29c
- Object: dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md at fe40ba9b23e31da95358441ae5214a7cadb31b31; committed DRAFT proposal only, no implementation acceptance
- Controlling DRAFT document: No controlling design supplied to Surface; only caller-facing guide above at fe40ba9b23e31da95358441ae5214a7cadb31b31
- Out of scope: Untracked SSH N2b prompts, route-mapping draft, core bug report, alpha-timeout evidence and these generated audit prompts. Drafter stopped. Product code unchanged and reference only.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md; review-loop canonical template
- Controlling documents to check the object against: Read ONLY dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md and existing dev-docs/GwzSspiCallerGuide-DRAFT.md, plus tuple metadata. Do not follow design/plan/review links or read code. Proposed signature snippets in caller docs are allowed.
- Explicitly deferred (do not report as findings): Operator outcome for timeout-zero/native-refusal remains pending, explicitly deferred rather than presumed approved; API/refusal shape and accuracy stay in scope. GO means coherent proposed contract only, not final freeze/implementation authorization before that decision. Native Windows/provider/EPA/Digest/trust/proxy/Pageant qualification, activation, physical wire/iroh and release/push/tag/publishing are deferred. Missing future implementation/tests is not a draft defect; feasibility/satisfiability are in scope..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
Read the guide cold: exact capture/start signatures, sharing/lifetime for concurrent members/retries, defaults, errors and refusal, fixed deadline and pending zero choice, finish/cancel/drop/shutdown. Walk through capture on original caller, move/share, one auth attempt and teardown from docs alone. Do not compile hypothetical APIs or infer semantics from design/code.

COMMANDS
Working directory /Volumes/projects/limbo/gwz-dev. At START and END: git rev-parse HEAD and git -C <each named member> rev-parse HEAD. Read committed object with git show fe40ba9b23e31da95358441ae5214a7cadb31b31:dev-docs/GwzSspiHttpsCallerGuide-DRAFT.md. Inspection only: sed/nl/rg (members via explicit paths or rg -uu), git show/log/diff/status. No builds/tests/writes/Git mutations/source probes/remote commands. Surface reads caller docs and tuple metadata only. Return complete report; owner files it verbatim.

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

Canonical report format:

# HTTPS SSPI composition proposal — Surface-AXIS REVIEW

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

