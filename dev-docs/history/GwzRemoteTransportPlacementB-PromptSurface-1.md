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
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportPlacementB-ReviewSurface-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: 93334058352828b1069b198d795c5860a395dc81
- gwz-core: 4f06384397a67d3dcae4856a93fd032499fda5dc
- gwz-transport: 03d3011b3ae9b8205bcf07f7f7862194af114856
- taut: bcf98b64d465fc54841121b6d1a2d46940f81a3c
- gwz-cli: 7db07bbdefd2897c07fd0f9e550bf032bd8b1314
- gwz-py: d07d55dacb1725d9306be9c04d157ac29a78e000
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- gwz-core-evidence: a2180f71f9f4f16ecc639eecd25125b980ea54f3
- Object: Phase4 PlacementB candidate aggregate. Core diff 6b9be8a2634700a3f47483beb99a582e7acd20b3..4f06384397a67d3dcae4856a93fd032499fda5dc; root checkpoint/evidence in same tuple; other seven members unchanged.
- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportPlacementB.md at 4f06384397a67d3dcae4856a93fd032499fda5dc
- Out of scope: four old untracked N2b root prompt files plus current-round generated prompts/reports only. No implementation edits during review.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md. Operator requests larger batches and retained reviewers.
- Controlling documents to check the object against: gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md; GwzRemoteTransportPlacementB.md; gwz-core/docs/TransportPlacement.md; dev-docs/CurrentProgramCheckpoint.md. Surface exception: user-facing docs only.
- Explicitly deferred (do not report as findings): Real supplied-carrier qualification C, carrier/framing construction, platform and selected-source qualification (operator-deferred together), production activation, HTTPS, publication/release. Initial TDD red capture incomplete and disclosed, not a full TDD claim. No whole-core passing claim. Review candidate correctness now; do not demand deferred outcomes..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
Read only gwz-core/docs/TransportPlacement.md, gwz-core/docs/Embedding.md and gwz-transport/README.md, not implementation/plans. Trace create/install/register/request/message-forward/use/cancel/finish/remove/shutdown from the guide. Check candidate scope, defaults, ownership, lifetime, request IDs and both forwarding directions. No CLI command added. Exact guide compilation is evidenced by owner; your task is cold surface evaluation.

COMMANDS
From /Users/owebeeone/limbo/gwz-dev: read-only git show/diff/status/log/rev-parse, rg, file reads/hashes and retained raw logs allowed. Code/State may run existing focused tests using RUSTFLAGS="--cfg gwz_transport_candidate" cargo +1.95.0 test --manifest-path /tmp/gwz-placement-b-backend/Cargo.toml --locked --offline --target-dir /tmp/gwz-placement-b-backend-target --lib <focused-test-name> -- --test-threads=1. No probes/file writes or broad rebuilds. Source proof sufficient; economical report <=1200 words unless concrete findings need more. Surface docs only and tuple checks.

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

Correction 1: read dev-docs/GwzRemoteTransportPlacementB-RemPlan.md and your own initial report. Recheck original counterexamples and changed-range regressions, include prior-finding closure table. Code/State may run the existing isolated endpoint checks with CARGO_TARGET_DIR=/tmp/gwz-placement-b-ssh-target cargo +1.95.0 test --manifest-path gwz-core/tests/transport_ssh/Cargo.toml --locked --offline --test placement_endpoint check_tests -- --test-threads=1. Surface: user-facing docs are unchanged from your GO; verify unchanged bytes and give same-tuple confirmation economically. No public API/schema changes; user explicitly requires old reviewers. Classify any new architectural root.
Retained reviewers are explicitly requested. Review this larger B object independently of your A verdict, without reading peer current reports. Complete final report (lane owner files verbatim):
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
