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
  nothing else. It will be filed verbatim as dev-docs/GwzSspiSupervisor-ReviewCode-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- gwz-dev root: 255d05e09c0433e27dd7afeaa9f9fe04699cad05 
- gwz-sspi a75485cbdd03607909d11637c07f97548dd7902c
- reference-only gwz-core 8cb3a3f01d79699a5ad07b6ec7cfc78321224d31
- Object: root 526451cea593e7d9550a28b3c86cf09a0dee2e20..255d05e09c0433e27dd7afeaa9f9fe04699cad05; member 44879481fbd54dab84b99fecdadc89a34a84dcbd..a75485cbdd03607909d11637c07f97548dd7902c
- Controlling DRAFT document: dev-docs/GwzSspiSupervisorCheckpoint.md at 255d05e09c0433e27dd7afeaa9f9fe04699cad05
- Out of scope: untracked SSH N2b prompts/route mapping and unrelated core/evidence dirt; current prompt/report outputs; all other members unchanged

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by GwzProcessOptimization.md; explicit scope/evidence instructions
- Controlling documents to check the object against: accepted dev-docs/GwzSspiDesign.md revision 2 + section 8, GwzSspiPlan.md step 2, GwzSspiSupervisorCheckpoint.md, member docs/Architecture.md, docs/Testing.md, docs/WireProtocol.md
- Explicitly deferred (do not report as findings): native SSPI/provider and worker_entry (step 3); Windows runtime qualification, installed core/CLI/Python composition, remote CI, activation/publication. Parent implementation and native adapter correctness remain IN scope..
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
Public async API and actual call graph; private phase/kind/Error.phase/round admission; Hello before secrets; no OS metadata/creation/I/O waits in poll; ownership and error paths; Windows unsafe scratch/attributes/handles/Job/suspended membership/identity provenance; standalone dependency and archive/CI claims. Inspect supervisor, protocol/supervision and audited Windows modules against accepted intent.

COMMANDS
cwd /Volumes/projects/limbo/gwz-dev. Read-only git rev-parse/status/show/diff/log, rg/sed/cat/nl/file inspection; verify all three HEADs at start/end. No writes, mutations, builds, campaigns, native workers or compiler probes. Optional direct pure unit binary /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/supervisor/debug/deps/gwz_sspi-e95571679a8a0127 with filters. Owner passed gates are in checkpoint; compilation is not native execution.

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

MANDATED REPORT FORMAT
# GwzSspiSupervisor — Code-AXIS REVIEW

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

RE-VERDICT ROUND 1 — CONTINUE ORIGINAL REVIEWER
Original reviewed root d2821a2db90aa641b1af6f80cadaf1aba9b35a0c and member
fb6a5fc6d1d013b3e7e9f6955d7b6dabafd51d7a. Corrected tuple above is fixed.
Changed ranges: original root/member .. corrected HEAD.
Merged disposition/closure plan: dev-docs/GwzSspiSupervisor-RemPlan.md.
Prior-round reports are legitimate inputs now; do not read current-round peer
reports/prompts. Re-run/re-trace YOUR original counterexamples on corrected bytes,
not merely drafter claims. Owner gates passed, full tests and archive verified.
No writes/builds/native campaigns/compiler probes; same pure prebuilt test binary
may be run by Code/State. Surface remains docs-only; own prior report permitted,
merged plan's Surface disposition only, no code/design/plan reading.

Before evidence section, add:
## Prior-finding closure table
| ID | Disposition claimed | Verified on corrected tree | Status |

## Changed-range analysis
Describe actual changed range and whether any part falls outside dispositions.
Label NEW ARCHITECTURAL root causes explicitly; reviewers classify, not drafter.
Private extraction/owned completion ports retain existing OS ownership/effects;
public precise capture restores accepted owned-future intent. Independently verify
that statement; identify a material architecture/contract change if present.
No finding self-closes. Return COMPLETE standalone report with binary verdict.
