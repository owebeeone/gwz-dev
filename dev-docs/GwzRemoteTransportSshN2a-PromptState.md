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
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportSshN2a-ReviewState.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: c33005d576e59ef46222e465285ae1eccf7ceedd 
- gwz-core: 42f90094dae33eef1cd4828d2c97cadda2761f43
- gwz-core-evidence: b5375aceb57773cf79ba25b443cafa2ada38bc08
- gwz-transport: 28f5afb3938a2aa8af0e1e8d5b07779add6ab776
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: N2a implementation latest core commit: three ssh_key_*.rs files, two isolated test binaries, fixture base64 pin, N2a checkpoint documentation and private evidence. Compare core 9f35c294..HEAD. No production routing activation.
- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportSshN2a.md at 42f90094dae33eef1cd4828d2c97cadda2761f43
- Out of scope: Only generated root N2a PromptCode/PromptState documents. No other uncommitted source. Existing code only provides seam context; do not report unrelated preexisting defects.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; review-loop; operator explicitly retains old reviewers.
- Controlling documents to check the object against: gwz-core/dev-docs/GwzRemoteTransportSshSelectedIdentityDesign.md N2a, GwzRemoteTransportSshProductionSetup.md, GwzRemoteTransportRequirements.md G1–G3, accepted N1/A1/A2/A3, CurrentProgramCheckpoint.md. N2a budget600 production/900 tests, actual553/603.
- Explicitly deferred (do not report as findings): N2b worker admission-before-pool/resource pins/combined cleanup and capacity-one fanout integration, N3 production module/dependency/backend attachment, platform and selected-source batch, production activation. Accepted unencrypted native parse/sign nonpreemptibility is supervised, not a hard physical-kill promise. Existing SSH config/endpoint-management choices are not reopened..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
- Attack ssh_key_snapshot ownership, slot/byte reservations before IO, allocation capacity accounting, cancellation/late disposal, mutex scope/unwind, exact-byte same-Key concurrent interning and monotonic token lifetime. Fixed buffers intentionally retain full charge (15 distinct buffers under16MiB); do not assume shrink happened.
- Attack ssh_key_container exact armor/base64/DER/OpenSSH framing, encrypted KDF rejection before native auth, full input scan/control checkpoints and<=256 scratch, error sanitization and no secret-derived tokens/logs. Native parser remains responsible for cryptographic usability.
- Attack ssh_key_auth trust-before-credential, immutable selected bytes, EAGAIN/cancel/deadline behavior, failure disposal order, successful joined-live promotion vs admitted/proven candidate semantics, no alternate identity/agent/file/Git fallback.
- Verify tests and evidence justify scope; contracts between N2a and future N2b must be usable without an architectural surprise. Current files are isolated fixture modules pending N3 attachment.
- Return concrete reproducible findings and original counterexamples, not broad speculative hardening. Both axes review same tuple; no current-round peer report access.

COMMANDS
Workspace /Users/owebeeone/limbo/gwz-dev. Read-only: git show/diff/log/rev-parse, rg, cat, gwz status; inspect committed evidence. Optional targeted test runs: CARGO_TARGET_DIR=/tmp/gwz-ssh-integration-target cargo +1.95.0 test --manifest-path gwz-core/tests/transport_ssh/Cargo.toml --offline --locked --test selected_key --test key_container . Runtime fixtures/outputs /tmp only. No source/fixture modifications, Git mutations, remote operations or other builds. Verify all tuple SHAs start/end; final complete report only.

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
