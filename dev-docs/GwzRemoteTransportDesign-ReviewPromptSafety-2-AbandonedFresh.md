Status: abandoned on operator instruction "use the old reviewers". No completed report or verdict; not a consumed review round.

# Transport draft review prompt — Safety

Generated from `/Users/owebeeone/.claude/skills/review-loop/references/review-prompt-template.md`.
Gate: draft-stage dual review; round 2, remediation round 1. Model binding: gpt-5.6-sol, xhigh, fresh context (cross-model review under process §4.3).

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
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportDesign-ReviewSafety-2.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE
- /Users/owebeeone/limbo/gwz-dev: 3a0b8fa975bc013673ee919b087f69da9f3853ff (read the immutable commit, not working-tree files)
- /Users/owebeeone/limbo/gwz-dev/gwz-core: 05842b38e55f109ed3663555680751811a72eb9b (read the immutable commit, not working-tree files)
- /Users/owebeeone/limbo/gwz-dev/gwz-cli: 07ad6620f2e26a2b8d70ae0cb8808dd5584058e1 (read the immutable commit, not working-tree files)
- /Users/owebeeone/limbo/gwz-dev/gwz-py: 9c9ac1b5c5bbd800e8454817b99f107cc74d25d2 (read the immutable commit, not working-tree files)
- /Users/owebeeone/limbo/gwz-dev/taut-shape: 74f375c9d3521f3e98110862dcf89ec64a3b6d6c (read the immutable commit, not working-tree files)
- /Users/owebeeone/limbo/gwz-dev/taut-shape-rs: df1303656583121e043f7a925c02958571951735 (read the immutable commit, not working-tree files)

This is a draft-stage design review, not an API/schema freeze or implementation acceptance. The object is precisely the four-document change in gwz-core from c2ca71b7595a0ce332f6f7f4c47db9d0c62ac230 to 05842b38e55f109ed3663555680751811a72eb9b: dev-docs/GwzRemoteTransportRequirements.md, dev-docs/GwzRemoteTransportDesign.md, dev-docs/GWZRequirements.md and dev-docs/GWZDesign.md. The transport design is the controlling DRAFT; the transport requirements record operator policy. Core source and other repositories are supporting evidence only, at their listed commits.

Operator decisions to respect: local core or CLI endpoint selected via mux; taut bidirectional data messages with variable-length BYTES, bounded buffering and first-byte timer (evaluate 100ms), full-buffer/flush immediate emission; endpoint owns SSH/HTTPS, trust and credentials; ordinary endpoint-client protections, distributed endpoint management deferred; SSH pools username+host+effective port with explicit identity compatibility, no repository component; gh-only HTTPS authentication; endpoint lifetime, 60s idle reaping; one active stream per connection initially; no silent route fallback. Do not reopen these outcomes. Concrete flaws in their specified semantics/shape remain in scope.

No implementation, generated schema, dependency or platform support is claimed. Qualification of concrete SSH/HTTP libraries, tag assignment and numeric tuning is explicitly pending. Do not report mere absence of implementation as a defect. Assess whether the design's own contract is coherent and implementable. This draft does not freeze a user-facing CLI/API, so this dispatch is the requested two-axis review, not a third Surface gate.

Other agents are retiring documents and editing protocol tests. ALL working-tree/index changes, new review artifacts, source edits outside the four-document diff, and later commits are outside the object. Read every review source with git show PIN:path, and use git grep PIN for searches. Verify each PIN resolves to the same commit and tree at start/end; record current HEAD movement separately but do not substitute HEAD content. If any pinned object cannot be verified, stop and report no verdict. Never open peer current-round reports or prompts. Round-1 reports and the merged RemPlan at root pin 3a0b8fa975bc013673ee919b087f69da9f3853ff are legitimate inputs: dev-docs/GwzRemoteTransportDesign-ReviewConsistency.md, dev-docs/GwzRemoteTransportDesign-ReviewSafety.md and dev-docs/GwzRemoteTransportDesign-RemPlan.md. This is a fresh-context round because binding/handshake/lifecycle semantics changed. Focus on the original counterexamples and changed-range interactions, and label any newly discovered architectural root cause explicitly for the two-round cap. Do not report missing future implementation as a defect. Do not spawn additional agents or write files.

AUTHORITY AND DEFERRALS
Process authority: root dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md at the root pin; review-loop SKILL and this canonical template. Root CurrentProgramCheckpoint.md is the existing merge-program ledger; this new transport draft is not a claim of completion of that older programme. Check the authoritative core requirements/design amendments and their older clauses, transport requirements D1–D15 and G/C/P/S requirements; relevant pinned taut-shape stream contract and GWZ schema/native transport source. Use root GwzRemoteAuthProposal references if needed, locating their committed path. Future peer/iroh networking, endpoint permissions, forwarded signing, portable identity namespaces, multi-channel SSH and OpenSSH config parsing are deferred outcomes; their boundaries remain reviewable.

REVIEW AREAS
AXIS: SAFETY — what the text permits to go wrong.
Attack: degraded and mixed-version paths; irreversible steps and their
preconditions; disclosure/privacy scale; stuck states reachable under the
text's own rules; whether "never worse than the status quo" claims survive
concrete interleavings; scope creep that widens blast radius.

- Trace adversarial sequences through design §§3–10: open/cancel races, backpressure in both directions, flush/end-write/close, carrier death, timers and connection reuse.
- Attack endpoint/path selection, explicit identity compatibility, HTTPS gh auth, mixed-version behaviour and reporting with concrete counterexamples.
- Check resource bounds, allocation/cleanup ownership and whether Git publication can be duplicated, falsely acknowledged or hang indefinitely under the text.
- Respect deferred endpoint permissions and existing endpoint trust assumptions; report specific defects permitted inside the accepted scope, not hypothetical new security programmes.

COMMANDS
Working directory: /Users/owebeeone/limbo/gwz-dev. Allowed inspection only: git -C REPO rev-parse PIN^{commit}; git -C REPO rev-parse PIN^{tree}; git -C REPO show PIN:PATH (optionally pipe to nl -ba/sed); git -C REPO grep -n PATTERN PIN -- PATH; git -C /Users/owebeeone/limbo/gwz-dev/gwz-core diff c2ca71b7595a0ce332f6f7f4c47db9d0c62ac230 05842b38e55f109ed3663555680751811a72eb9b -- dev-docs; git ls-tree/log read-only; rg/cat/sed for the skill/template or installed pinned library source only. No test/build commands for this docs-only gate. No file writes, index/ref changes, network experiments, or peer-report reads. Cite document lines from the pinned git-show output.

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

Include a prior-finding closure table and changed-range analysis before section 0. Changed design range: gwz-core 3d2cc92c4a233e40ed049c8fe79bc555d39b272f..05842b38e55f109ed3663555680751811a72eb9b, limited to the four object documents. This immutable range excludes other agents' archive/protocol changes. Read-only git diff of that range is allowed.

REPORT FORMAT
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
