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
  nothing else. It will be filed verbatim as dev-docs/GwzNoFallbackNativeFix-ReviewSurface.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: 62c2f122c28ededdefb7af32058f4b058b018dcb ; core01d6f6624472620c215693243f7ac3865aeb31a4; git2-rs4c1caabbce7d56426c763dd94114052302b23e4c; libgit2HEADb172e3d187a4b6866fd9f696f40a1b8e7f56d348; upstream-facing branch fe618d0f5de9e506b9714643afc42d2fcba6e984
- Object: Only tests/transport_native/README.md and prove.py observable help and execution
- Controlling DRAFT document: gwz-core/dev-docs/GwzNoFallbackNativeFix.md (Surface: do not read design) at 01d6f6624472620c215693243f7ac3865aeb31a4
- Out of scope: only untracked owner review prompts; no other code in scope

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md amended by GwzProcessOptimization.md
- Controlling documents to check the object against: accepted NativeFix design and linked boundaries; Surface: only public README/help
- Explicitly deferred (do not report as findings): production activation/fallback removal, full-platform/all-consumer qualification, publication, type-consistency hardening of existing ENOTFOUND cases; these remain explicit future gates.
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
- Read README/help cold; prerequisites, initialized unpublished submodule, two mutually-exclusive modes and defaults, cleanup, scope and first-day walkthrough. Read no code/design/plan.

COMMANDS
At workspace root: read-only git show/diff/log/ls-tree/rev-parse/status, cat/sed/rg. Allowed tests: python3 -m unittest discover -s gwz-core/tests/transport_native -p test_prove.py; rustfmt +1.95.0 --check gwz-core/tests/transport_native/tests/local_fetch.rs. Surface may run python3 gwz-core/tests/transport_native/prove.py --help and both --git2-source git2-rs and --git2-archive /Users/owebeeone/.cargo/registry/cache/index.crates.io-1949cf8c6b5b557f/git2-0.21.0.crate. No other mutations/builds. Verify tuple at both ends.

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
