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
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportHttpsDesign-ReviewSafety-2.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: bcdca800ab19fb767f6e7d2ab8107f12dab48810
- gwz-core: 2ea02835a15a9f56afdda43ccbcadec66b5b776e
- gwz-transport: 03d3011b3ae9b8205bcf07f7f7862194af114856
- taut: bcf98b64d465fc54841121b6d1a2d46940f81a3c
- gwz-cli: 7db07bbdefd2897c07fd0f9e550bf032bd8b1314
- gwz-py: d07d55dacb1725d9306be9c04d157ac29a78e000
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- gwz-core-evidence: d096a9dcf0d43e79ea32bced5b802bf8a877ce1d
- Object: gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md plus all correction-2 controlling amendments, embedding guide, root checkpoint and dev-docs/GwzRemoteTransportHttpsDesign-RemPlan.md; documentation-only design re-admission.
- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md at 2ea02835a15a9f56afdda43ccbcadec66b5b776e
- Out of scope: Four old N2b prompts plus current generated HTTPS correction prompts/reports. Prior reports and merged RemPlan are allowed inputs. No implementation edits.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md. Operator requests retained reviewers and larger batches.
- Controlling documents to check the object against: For Surface only: gwz-core/docs/TransportPlacement.md and docs/Embedding.md. Other axes: gwz-core/dev-docs/GWZRequirements.md, GWZDesign.md, GwzRemoteTransportRequirements.md, GwzRemoteTransportDesign.md, GwzRemoteTransportPlan.md, GwzRemoteTransportPlacementDesign.md; dev-docs/CurrentProgramCheckpoint.md. Candidate source interfaces may be inspected.
- Explicitly deferred (do not report as findings): Actual implementation, native platform and selected-source checks (operator-deferred single batch), production activation/public constructors (separate Surface gate), physical wire/split-process/iroh, release. No new fields/CLI/constructors; scheme-specific observation behavior is explicitly amended and receives Surface review. Assess feasibility and explicit obligations, do not demand implementation results at a design gate..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
Attack the HTTPS design state sequences: anonymous/auth transitions, token isolation, redirect origins/routes, POST replay/publication effects, pool disposal, cancellation, blocking jobs, independent pumps, credit/memory bounds, helper children and cleanup accounting. Demand concrete counterexamples and bounded remedies. This is document admission, not a passing implementation claim.

COMMANDS
Workspace /Users/owebeeone/limbo/gwz-dev. Read-only git show/diff/rev-parse/status, file reads, rg. Primary linked upstream docs/source may be checked read-only. No builds/probes/writes. Keep report <=1200 words unless concrete findings require more.

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

Final focused re-review: Consistency closes P2-4 OpenFailed lifecycle only plus changed range; Safety confirms its GO for this bounded correction; Surface confirms unchanged public documentation bytes. Return concise <=500 words with tuple and closure. Merged plan and prior reports allowed. Surface reviews documentation only. Do not demand executed behavior tests at this document gate. Do not read current peer report.
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
