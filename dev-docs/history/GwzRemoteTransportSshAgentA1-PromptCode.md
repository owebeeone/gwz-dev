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
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportSshAgentA1-ReviewCode.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: 6da3ce07e0ccbca36eec4bbf23b9315add645f98 
- gwz-core: 309696601227494f60942aea96124ca4eaeebda5
- gwz-core-evidence: 0792238da91b2466ca8ce9dc752e9dc901c0bad4
- gwz-transport: 28f5afb3938a2aa8af0e1e8d5b07779add6ab776
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: A1 implementation checkpoint: latest core commit adds agent_job.rs, agent_client.rs, agent_socket.rs, agent_client/agent_capacity tests, isolated socket2/libc manifest/lock dependencies, documentation; private agent-a1 evidence. No native signing or production pool/backend activation.
- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportSshAgentA1.md at 309696601227494f60942aea96124ca4eaeebda5
- Out of scope: Only two owner-generated root review prompt files. Production module/dependency activation, adjacent unrelated lanes and prior accepted qualification packages are outside changed range.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; retained reviewers per operator.
- Controlling documents to check the object against: GwzRemoteTransportSshAgentA1.md and accepted GwzRemoteTransportSshAgentDesign.md; EVIDENCE.md; CurrentProgramCheckpoint.md; supporting retained worker/pool contracts.
- Explicitly deferred (do not report as findings): Operator postpones platform and selected-source qualification as one later batch. A1 qualifies only local agent codec/client and generic helper lifecycle. A2 native signing, A3 pool/backend/endpoint-refusal integration are separate required gates, not falsely claimed here. Native pending-connect cancellation and send-buffer exhaustion are explicitly unexecuted on this Mac; script/native evidence is labelled. No public API/wire surface..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
- Attack cancellation, timeout, publication/claim/join/disposal races and whether helper/global capacity survives abandonment and cleanup overrun.
- Attack codec bounds, malformed frames, redacted errors, RSA flag/signature checks, partial I/O, no replay and native nonblocking socket loops.
- Inspect 52 passing executions, 64 printed fragmentation seeds, local backlog refusal versus pending claim, spawn-failure and 64-helper capacity proof. Assess identity representation refinement (fresh Arc cell, no reused routable ID) and bounded non-panicking result Drop precondition.
- Current code is preactivation only; do not attribute A3 endpoint state or native signing to A1. Check honest scope and evidence hashes.

COMMANDS
From workspace root: read-only git show/log/diff/rg/cat, gwz status. Optional focused tests ONLY: CARGO_TARGET_DIR=/tmp/gwz-ssh-integration-target cargo +1.95.0 test --manifest-path gwz-core/tests/transport_ssh/Cargo.toml --offline --locked -- --nocapture. Do not edit/write files, including reports. No platform or selected-source qualification, network fetches or additional campaigns. Verify tuple at start/end. Return complete concise report.

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
