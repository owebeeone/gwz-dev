You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: Code
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportSshN2b-ReviewCode.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: 3d411d8c84a93839cd04ddc4262e37c31c27e53b
- gwz-core: 0f476cb9c67bce00e24afbdef6caee57fb6cd3dd
- gwz-core-evidence: 7cea42eb004ab439519dd86293852adf74e050cd
- gwz-transport: 50bb6b1275b26f7c2a38493f06a1638d220a6c7a
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: N2b selected admission/pool-worker integration at core 0f476cb9; transport deadline extensions at gwz-transport 50bb6b12; evidence run at evidence 7cea42eb. Compare prior accepted N2a without reopening it. Production routing and backend activation remain absent.
- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportSshN2b.md at 0f476cb9c67bce00e24afbdef6caee57fb6cd3dd
- Out of scope: Only generated N2b review prompts are untracked at root. The N2b implementation is the committed core/transport diff from their prior accepted heads; evidence is the committed selected-key-n2b run.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; review-loop; retained reviewers are explicitly requested.
- Controlling documents to check the object against: GwzRemoteTransportSshN2b.md; accepted GwzRemoteTransportSshN2a.md; GwzRemoteTransportSshSelectedIdentityDesign.md §3–4; GwzRemoteTransportSshProductionSetup.md; GwzRemoteTransportRequirements.md G1–G3; accepted A1/A2/A3/N1 and CurrentProgramCheckpoint.md. Pool deadline contract in gwz-transport pool modules and network_timeouts tests.
- Explicitly deferred (do not report as findings): N3 production module/dependency/backend attachment, all network entry points, HTTPS, platform and selected-source qualification, production activation, and the operator-deferred batch. Do not report N2a findings already closed or demand public API/CLI changes..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
- Attack admission before pool lookup, exact absolute deadline propagation from caller through file Job, queue, checkout, connection and handoff; cancellation/late-result behavior and no path reopen/fallback.
- Attack strong Entry pin ownership through connecting/idle/active/reclaim/disposal, candidate versus proven promotion, reuse after current-file reread, changed bytes and concurrent same-Key fanout at pool capacity one.
- Attack worker progress fairness, admission bounds, request permit release, reply timeout races, shutdown/panic/retained cleanup containing both Admissions and PoolHost, pending-admission status and no false cleanup_complete.
- Attack transport pool absolute-deadline changes across allocation, connection, interaction pause/resume, timeout/error mapping, old API compatibility and direct tests.
- Check selected-pool tests and evidence actually prove the stated gate and stay within the 500 production/900 test N2b scope.
- Return concrete P0–P2 findings with state sequences and closure tests; do not broaden into N3 or platform/source qualification.

COMMANDS
Workspace /Users/owebeeone/limbo/gwz-dev. Read-only: git show/diff/log/rev-parse, rg, cat, gwz status; inspect committed evidence. Optional focused runs: CARGO_TARGET_DIR=/tmp/gwz-ssh-integration-target cargo +1.95.0 test --manifest-path gwz-core/tests/transport_ssh/Cargo.toml --offline --locked --test selected_pool ; cargo +1.95.0 test --manifest-path gwz-transport/Cargo.toml --offline --locked --test network_timeouts . No edits, Git mutations, remote operations or unbounded builds. Verify tuple at start/end. Final complete report only.

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

AXIS: CODE — architecture, interfaces, call graphs, and compatibility reality.
Attack: interface contracts vs. actual call sites; ownership and visibility;
API/wire compatibility with retained readers and older writers; error paths
and hidden panic/allocation paths; whether the diff does what its DRAFT doc
claims and nothing it forbids.

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
