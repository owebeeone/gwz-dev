You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: Code
AXIS: CODE — architecture, interfaces, call graphs, and compatibility reality.
Attack: interface contracts vs. actual call sites; ownership and visibility;
API/wire compatibility with retained readers and older writers; error paths
and hidden panic/allocation paths; whether the diff does what its DRAFT doc
claims and nothing it forbids.
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzSspiHosts-ReviewCode-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- gwz-dev: 6b670283179f45f0dbbe38b1f6c300b326e50100
- gwz-sspi: 14d834b311200b0984e7041e4a34419c59501119
- gwz-cli: 0c7dfaf0199731648d2360358284010b2b4575c1
- gwz-py: c3f5f7d6b614155e413db0036242849010ef1149
- reference gwz-core: 8cb3a3f01d79699a5ad07b6ec7cfc78321224d31
- Object: installed hosts4a; SSPIe31b17e..14d834b, CLI061385f..0c7dfaf, Pye9e228c..c3f5f7d; root3066630..6b670283179f45f0dbbe38b1f6c300b326e50100
- Controlling DRAFT document: dev-docs/GwzSspiHostsCheckpoint.md at root 6b670283179f45f0dbbe38b1f6c300b326e50100 (Surface reads caller docs only)
- Out of scope: untracked SSH N2b prompts/route draft, core bug report, alpha-timeout evidence, generated current prompts/reports; adjacent members unchanged

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md amended by dev-docs/GwzProcessOptimization.md; root/member AGENTS instructions
- Controlling documents to check the object against: Code/State: accepted GwzSspiDesign revision2 §§2,4–6; GwzSspiPlan step4; GwzSspiHostsCheckpoint.md. Member HostPackaging/WorkerEntry and implementation tests. Surface: caller docs listed below only.
- Explicitly deferred (do not report as findings): HTTP/core composition4b (finite clock translation, honest source/facts), Digest refusal/amendment, Windows activation/full qualification/native installed Hello execution, Linux installed-image qualification, publishing/remote availability/CI. Existing untouched Python all-target Clippy/format debt and stale standalone lock reconciliation are disclosed. Do not treat these outcomes as4a blockers or broaden scope; audit claimed4a behavior/shape and all its actual release entry points..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REMEDIATION ROUND 1
This is a focused re-verdict on your own original findings and the complete changed range. Read your own initial report and the merged GwzSspiHosts-RemPlan.md as legitimate prior-round inputs; do NOT read current peer reports/prompts. Verify all five corrected HEADs start/end. Classify any new architectural root cause or material architecture/interface/ownership/platform change explicitly; do not assume extracted proofs still hold. Include a prior-finding closure table. Source process-free regression tests and actual owner-recorded portable gates may be inspected; no build, execution or write is authorized. No implementer self-closure. Assess private build staging/no-replace publication and normalized metadata hooks as actual changes, not just claimed fixes. Current root checkpoint records limits. Surface reads only its own original report, its P3 disposition in the merged plan, and permitted cold caller docs; no implementation/design/checkpoint or other dispositions are Surface evidence. Return your COMPLETE standalone report as final output only.

REVIEW AREAS
Attack the complete25-file host diff against accepted SSPI boundary: trusted packaging inputs/actual targets and Cargo dependency resolution; compiled metadata vs runtime receipt; CLI early bootstrap before all ordinary initialization; Python actual image provenance and fail-closed selection; PEP517/candidate/sdist/extracted/publish entry points; ordinary Cargo/editable behavior; registry pinning; malformed/absent/mismatched input; no new HTTP carrier/core implementation claim. Read code call graph and relevant public tests. Selected input fingerprint is not binary attestation.

COMMANDS
cwd /Volumes/projects/limbo/gwz-dev. Read-only git rev-parse/status/show/diff/log, rg/cat/sed/nl/stat/file. Verify all5 HEADs start/end. No writes/builds/Git mutations/compiler probes/native campaigns/process execution. Read committed source/docs using git show or clean member files. Code/State may inspect public tests and owner recorded gates, plus external normal build artifacts under /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/hosts; these are portable packaging/source evidence, not Windows runtime proof. Surface only Git HEAD/status, its prior report/disposition and listed cold caller docs. Do not read current peer reports/prompts or drafter narrative. Existing old native gate acceptance is legitimate authority, not a current peer verdict.

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

# GWZ SSPI installed hosts4a — Code-AXIS REVIEW

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
