You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: State
AXIS: STATE — durable-state semantics and adversity.
Attack: state machines and restart legality; filesystem and durability
ordering; crash/kill points between every pair of writes; races and lock
scope; fail-closed direction (a defect may lose progress, never invent it);
recovery states as a closed grammar — hunt for new stuck states the current
semantics does not have.

- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportPlacementC-ReviewState.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: f3ad29ae5aa55ebd4e558f3f11a078e6b837196e
- gwz-core: c5dd307142e6958160efabf36a8521b5f104c157
- gwz-transport: 03d3011b3ae9b8205bcf07f7f7862194af114856
- taut: bcf98b64d465fc54841121b6d1a2d46940f81a3c
- gwz-cli: 7db07bbdefd2897c07fd0f9e550bf032bd8b1314
- gwz-py: d07d55dacb1725d9306be9c04d157ac29a78e000
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- gwz-core-evidence: d096a9dcf0d43e79ea32bced5b802bf8a877ce1d
- Object: Phase4 PlacementC candidate aggregate. Core diff feeab90ce11804a2983a1f53d8c12f9e54cb4d27..c5dd307142e6958160efabf36a8521b5f104c157; root checkpoint/evidence in same tuple; other seven members unchanged.
- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportPlacementC.md at c5dd307142e6958160efabf36a8521b5f104c157
- Out of scope: four old untracked N2b root prompt files plus current-round generated prompts/reports only. No implementation edits during review.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md. Operator requests larger batches and retained reviewers.
- Controlling documents to check the object against: gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md (operator in-process C clarification controls); gwz-core/dev-docs/GwzRemoteTransportPlacementC.md; gwz-core/docs/TransportPlacement.md; dev-docs/CurrentProgramCheckpoint.md. Surface exception: user-facing docs only.
- Explicitly deferred (do not report as findings): Physical wire and split-process qualification, iroh, carrier/framing construction, platform and selected-source qualification (operator-deferred together), production/frontend activation, HTTPS, publication/release. C is now only embedding in current CLI/core and Python/core messages in SAME process plus plausible future wire. Do not reinstate former real-carrier proof. No whole-core passing claim. No current user API/schema change: this is proof, not activation. Initial adapter red is a captured unimplemented-boundary test, not a historical product defect..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
Attack fixture assertions for actual bidirectional progress, bounded forwarding and paused Data, preserved large payload, once-only operation dispatch, cancellation/closure and worker cleanup. Check same-process Python/GIL/cache ownership and test failures. Inspect source and evidence for overclaims: Rust core-generated types are the same held by CLI; Python uses its actual codec inside Rust process; the complete frontend dispatchers are not activated. Future wire mapping is plausibility only.

COMMANDS
From /Users/owebeeone/limbo/gwz-dev: read-only git show/diff/status/log/rev-parse, rg, file reads/hashes and retained raw logs allowed. Code/State may run existing focused tests using RUSTFLAGS="--cfg gwz_transport_candidate" cargo +1.95.0 test --manifest-path /tmp/gwz-placement-c-backend/Cargo.toml --locked --offline --target-dir /tmp/gwz-placement-b-backend-target --lib <focused-test-name> -- --test-threads=1. No probes/file writes or broad rebuilds. Source proof sufficient; economical report <=1200 words unless concrete findings need more. Surface docs only and tuple checks.

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

Retained reviewers are explicitly requested. Review this C object independently of your B verdict, without reading peer current reports. Complete final report (lane owner files verbatim):
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
