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
  nothing else. It will be filed verbatim as dev-docs/GwzGitLibraryWindows-ReviewState-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: 5d11a6c3444d4a58c774fc083db1dbbd4e621fd8 
- gwz-core: f1029847cd964a012ccec12495f897f444acdcd1
- gwz-core-evidence: 040e3ab4db7871350650382d01d4f7c5f62d7615
- gwz-git: aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: Q4 focused State P2-1 closure: evidence diff b9456f7a..HEAD adds Cargo ancestor/home config refusal, guard regression and complete native windows-f rerun. Core diff44d27ad6..HEAD is documentation only. Public fixtures and all product/fork/library source are unchanged. Apply dev-docs/GwzGitLibraryWindows-RemPlan.md; do not redo unrelated initial review.
- Controlling DRAFT document: gwz-core/dev-docs/GwzGitLibraryWindows.md at f1029847cd964a012ccec12495f897f444acdcd1
- Out of scope: Only one owner-generated State re-review prompt file. Product/fork/library Rust, manifests/locks remain unchanged. All prior Q3/Q2 data out of changed range.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; retained reviewers per operator.
- Controlling documents to check the object against: GwzGitLibraryWindows.md, GwzGitLibraryQualification.md, GwzGitLibraryNativeConsumers.md, EVIDENCE.md, evidence AGENTS.md and CurrentProgramCheckpoint.md.
- Explicitly deferred (do not report as findings): Complete Windows five-consumer matrix, other native platforms, full operation/network coverage, independently fetchable sources, publication, production activation, fallback removal. Honest limits remain in scope..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
- Attack platform executable-mode treatment, unchanged bytes/type/link admission, Git-for-Windows source preparation and its exact provenance.
- Attack fixture file URL correctness and retained explicit native file-transport behavior; actual native Windows per-remote/library/probe results and macOS regression.
- Attack command/status/source/lock/artifact accounting, D: containment, configuration isolation, failed-attempt retention and public claim accuracy. This is isolated qualification, not activation. No new product API surface.

COMMANDS
Read-only git show/log/diff/rg/cat and gwz status. Optional focused closure test: python3.14 -B -m unittest discover -s gwz-core-evidence/campaigns/git-library/runner -p test_windows.py from workspace root. No new builds, remote actions or campaign reruns. Raw native Windows and macOS logs are committed. Check tuple start/end. Return concise complete report only; no writes.

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

AXIS: STATE — durable-state semantics and adversity.
Attack: state machines and restart legality; filesystem and durability
ordering; crash/kill points between every pair of writes; races and lock
scope; fail-closed direction (a defect may lose progress, never invent it);
recovery states as a closed grammar — hunt for new stuck states the current
semantics does not have.

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

Include a prior-finding closure table and changed-range analysis. Verify the original external-parent-config counterexample, including effective search paths and the fresh native run. Preserve prior reports verbatim; use the correct gwz-git revision from this tuple (initial State report had a transcription typo).
