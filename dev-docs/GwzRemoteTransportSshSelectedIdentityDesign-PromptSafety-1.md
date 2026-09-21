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
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportSshSelectedIdentityDesign-ReviewSafety-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: a9ad12dcafb51d77e7d0fbd28fac97934e070b09 
- gwz-core: 35df881b7075d7031082f61e0b99b838341149e1
- gwz-core-evidence: e842abf855e58de3c4381855fbc1b8374485a7cd
- gwz-transport: 28f5afb3938a2aa8af0e1e8d5b07779add6ab776
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- Object: N2 merged design remediation 1 versus core120fb9f1379025c66ea00cbdfd5af7f5815859db: canonical identical candidate tokens for first fan-out and bounded pre-native encrypted-container refusal, with G1/ProductionSetup refinements. Documentation only; no executable change or activation. N1 remains accepted.
- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportSshSelectedIdentityDesign.md at 35df881b7075d7031082f61e0b99b838341149e1
- Out of scope: Only two root generated SelectedIdentityDesign PromptConsistency-1/PromptSafety-1 files. Prior reviews and merged RemPlan-1 are legitimate inputs; remain current-peer-blind. Read current implementation only as grounding for intended seams; do not report unrelated preexisting defects.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; review-loop; operator explicitly keeps retained reviewers.
- Controlling documents to check the object against: N2 draft and dev-docs/GwzRemoteTransportSshSelectedIdentityDesign-RemPlan-1.md; prior own report; GwzRemoteTransportRequirements.md G1–G3; GwzRemoteTransportSshProductionSetup.md; accepted A1/A2/A3/N1; current checkpoint. Current ssh_endpoint/worker/setup/shutdown, transport Identity, backend identity selection and native in-memory auth sources ground feasibility.
- Explicitly deferred (do not report as findings): N3 production backend/wire attachment, platform/selected-source qualification and capability activation. N2a/N2b are later implementation gates, not claimed done. Existing identity precedence and explicit-no-agent-fallback remain authoritative. Do not add endpoint-management or SSH-config requirements..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
- Focused re-review only; use retained context for unchanged proofs. Safety closes its P2-1 encrypted-KDF path; both axes verify owner P2-O1 first-fan-out incompatibility correction.
- Canonicalize live exact-byte same-Key candidates including unproven ones. Equality names immutable material, not authentication; existing physical resource native-ready/authenticated/reusable gate is mandatory. Each request and connection retains independent cancellation/deadline. Check no stale/different bytes cross, no failed sharer promotion/poisoning; barrier batch test at capacity1 must use one successful connection.
- Preflight accepts one unambiguous unencrypted armor block only: no additional headers/blocks/garbage; PKCS8 encrypted label refuses; traditional headers refuse; OpenSSH requires cipher/kdf none and empty options. Streaming maintained base64 decoder, fixed256 scratch reserved before file read, no work-factor processing. Accepted PKCS8 has definite DER/version framing, native usability authority remains. Review against native encrypted-container/KDF path; adversarial high-KDF fixtures must show zero native calls, recovery and active-stream progress without executing KDF.
- G1 representation restrictions, budgets600 production/900 tests N2a, existing N2b bounds and deferred N3/platform gates are explicit. No new cryptography/parser of key math or public/wire change. Return closure table and GO/NO-GO with concrete changed-range findings. No need to re-read unchanged world.

COMMANDS
Read-only inspection: git show/diff/log, rg, cat, gwz status. No builds, edits, report writes, native experiments or remote operations. Verify tuple start/end. Inspect pinned local source as needed. Final output complete report.

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

AXIS: SAFETY — what the text permits to go wrong.
Attack: degraded and mixed-version paths; irreversible steps and their
preconditions; disclosure/privacy scale; stuck states reachable under the
text's own rules; whether "never worse than the status quo" claims survive
concrete interleavings; scope creep that widens blast radius.

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
