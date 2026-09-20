# Generated canonical L2-A review prompt

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
  nothing else. It will be filed verbatim as dev-docs/GwzNoFallbackBindingPort-ReviewCode-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- root: a02f91f66b4efe483e44c3fa4e85dfb6d94c19e0
- core: 5eb29f073a536901b23f96c3d4b1d05ac59ac01c
- git2-rs: e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68
- transport: 28f5afb3938a2aa8af0e1e8d5b07779add6ab776
- taut: 733e8a78897a90f017f4726e4331aed95e8cb977
- Object: L2-A binding member port; core 0fedb1a6..f9794d55, fork dffaf272..e883be38; only 7 named files
- Controlling DRAFT document: gwz-core/dev-docs/GwzNoFallbackBindingPort.md at f9794d55; Surface reads only public README/help, not this design
- Out of scope: Concurrent L1/L3/L4 characterization edits and integrator wiring/checkpoint handoff in core are authorized but OUT OF SCOPE. Read immutable commits; do not require those working files clean. Only current review prompt/report files permitted in root. Named L2 files are frozen, no commits will move tuple during review.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; operator retains old reviewers and requests economical execution
- Controlling documents to check the object against: Core GwzNoFallbackCheckpoint.md, GwzNoFallbackPlan.md, GwzRemoteTransportNativeBinding.md, binding-pin.json; Surface is restricted to tests/transport_native/README.md and public help
- Explicitly deferred (do not report as findings): Replacement implementation in other lanes; production dependency activation, package naming/publication, all-consumer/platform matrix, SSH/pool activation. No Rust API delta beyond exactly previously reviewed binding patch. Review new source-mode admission/isolation and public tool input, not those deferred outcomes.
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
AXIS: CODE — architecture, interfaces, call graphs, and compatibility reality.
Attack: interface contracts vs. actual call sites; ownership and visibility;
API/wire compatibility with retained readers and older writers; error paths
and hidden panic/allocation paths; whether the diff does what its DRAFT doc
claims and nothing it forbids.

Verify exact release/source/patch identity, sole native registry-edge change, no hidden graph changes, copy-before-build isolation, unrelated/missing/extra/symlink drift refusal, archive-mode compatibility. Source mode must not modify checkout/cache/production state. Do not require auxiliary fork workspace-wide tests: only external root git2 library consumption is scoped. Surface: walk both documented input modes and help/cleanup; no code or internal design reads. Keep review targeted and report concise evidence.

COMMANDS
At /Users/owebeeone/limbo/gwz-dev: read-only git show/log/diff/ls-tree/rev-parse/status, rg/cat/sed/nl/wc/hash. No mutation/network. Code/State may run python3 -m unittest discover -s gwz-core/tests/transport_native -p test_prove.py. Surface may execute prove.py --help and documented source/archive commands (temporary sources and ignored build outputs only). Other axes need not repeat costly native tests; inspect recorded evidence. Recheck tuple start/end, do not read peer current-round reports or prompts. Return complete concise report.

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

## Report format

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

## Focused remediation round 1

Read GwzNoFallbackBindingPort-RemPlan.md and core diff f9794d55..5eb29f07. Only runner baseline construction/tests/report changed: use fixed-release ls-tree and cat-file instead of archive. Binding/manifests/publichelp/README unchanged. State verifies original P2-1 export-ignore counterexample closure; Code reviews the same bounded correction. Add prior-finding closure and changed-range analysis. Seven Python guards pass including red-to-green real repository regression; positive source and archive native modes pass seven tests each. Actual pinned-release temporary clone attack also refused CONTRIBUTING.md omission. You may run Python guards; no need repeat costly native modes. Return concise exacttuple report, ideally under500words.
