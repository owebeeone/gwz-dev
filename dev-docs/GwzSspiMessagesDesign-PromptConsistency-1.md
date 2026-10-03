You are an independent, adversarial, READ-ONLY reviewer. Your job is to try to
refute this object's fitness, not to appreciate it. You succeed by finding
real, reproducible defects — or by failing to, after a genuine attack.

ROLE AND OUTPUT
- Axis: Consistency
- Another reviewer is attacking the same object on a different axis in
  parallel. You must not see, request, or reason about their report. Your
  verdict is formed from your own evidence alone. (Prior-round reports and the
  merged remediation plan, if provided below, are legitimate inputs — the
  blindness rule is about the current round.)
- Your final message must be the COMPLETE report in the mandated format, and
  nothing else. It will be filed verbatim as dev-docs/GwzSspiMessagesDesign-ReviewConsistency-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root / gwz-sspi / gwz-core: 05403cea8018cf14a793977ddec2c0c7136ccc54 / 7ea900e77f272dd6e0d64f566c59fb29322f5738 / 8cb3a3f01d79699a5ad07b6ec7cfc78321224d31 — verify all three HEADs start/end
- Object: root 92d6d5e..05403cea8018cf14a793977ddec2c0c7136ccc54; member 12f4822..7ea900e77f272dd6e0d64f566c59fb29322f5738; private taut schema/design/tooling
- Controlling DRAFT document: dev-docs/GwzSspiMessagesDesign.md at 05403cea8018cf14a793977ddec2c0c7136ccc54
- Out of scope: Untracked root SSH N2b prompts/route draft, core bug report, old evidence timeout run, adjacent lanes. This object’s untracked PromptConsistency/PromptSafety are authorized outputs. Do not read peer reports.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md and review-loop skill
- Controlling documents to check the object against: Root GwzSspiMessagesDesign.md, GwzSspiDesign.md revision 2 §§2–7, GwzSspiPlan.md step 1, GwzSspiCallerGuide-DRAFT.md, GwzSspiAcceptance.md; member docs/WireProtocol.md, docs/Architecture.md, docs/Testing.md, protocol/*, scripts/regen_schema.py, tests/schema/test_schema.py and .github/workflows/ci.yml; core GwzTransportWindowsParityDesign.md §8 and SSPI banners.
- Explicitly deferred (do not report as findings): Rust caller values/codecs, production framing/state/supervisor/native SSPI and installed worker/build-manifest implementation are subsequent gates and are not claimed here. No real credentials allowed in tooling. Their outcomes are deferred; attack implementability/shape of requirements. No remote/publication/native qualification/activation claimed..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
AXIS: CONSISTENCY — the document against its controlling graph.
Attack: internal contradictions between sections; agreement with every
controlling contract/design it cites (verify quotes verbatim at the cited
lines); exactness of superseded-clause lists; whether its own test/evidence
sections are satisfiable as written; unstated impacts on documents it does
not cite.
- Exact schema/type/tag agreement with standalone semantics and accepted design.
- Digest first input, actual identity, binding representation, mechanism observation, HTTP/provider caps.
- Every ordered phase/round/Finish/Error; malformed/late frames, terminal publication, cleanup proof.
- Secret projection stop/reference limitations, synthetic-test claims, artifact/version/module provenance.
- Independent CI/archive/contract shape; no application protocol, knob, clock, retry or dependency expansion.
Classify new architectural root causes. Complete report roughly 900 words maximum, no speculative/style padding.

COMMANDS
cwd /Volumes/projects/limbo/gwz-dev. Allowed inspection: git rev-parse HEAD; git -C gwz-sspi rev-parse HEAD; git -C gwz-core rev-parse HEAD; git show exactSHA:path; git diff exact ranges above; rg/nl/sed/cat; read-only Python JSON/hash inspection. Allowed tests only: /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/schema-tools/bin/python -B gwz-sspi/scripts/regen_schema.py --check; same Python -B -m unittest discover -s gwz-sspi/tests/schema -v. No builds, writes or Git mutations. Read committed package, verify tuple start/end.

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

REPORT TEMPLATE:
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

REMEDIATION ROUND 1: Continue same reviewer at operator standing preference.
Read dev-docs/GwzSspiMessagesDesign-RemPlan-1.md and prior reports (legitimate prior-round input).
Changed ranges root de941b21b0b0e694ae529a817eb75b128f292a98..05403cea8018cf14a793977ddec2c0c7136ccc54 and member 8379af881c04bd59e1e470c853e828a0037f7944..7ea900e77f272dd6e0d64f566c59fb29322f5738. Retrace original P2-1 counterexample. Corrected suite 12/12. Surface amendment now separately reviewed; do not read its current report.
Add required closure table and changed-range classification:
## Prior-finding closure table
| ID | Disposition claimed | Verified on corrected tree | Status |
{One row per prior finding. "Verified" means the ORIGINAL counterexample was
re-run/re-traced on the new tuple — a claim of fixing is not closure.}

## Changed-range analysis
{What actually changed since the reviewed revision, and whether any change
falls outside the dispositions — new-root-cause candidates go here, and NEW
ARCHITECTURAL root causes must be labeled as such: the two-round cap turns on
that classification, and it is the reviewer's call, not the implementer's.}
