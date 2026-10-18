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
  nothing else. It will be filed verbatim as dev-docs/GwzRemoteTransportHttpsH2-ReviewState-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- .: e97f79e9667c9d31871d3d9210651909eb6a4255
- gwz-core: 7a5195eca4cdfcd10ede7c3aafa16b2dcdcfb2bc
- gwz-transport: aa40936d0805e8cb60f8027615abe20d4f2045e4
- taut: bcf98b64d465fc54841121b6d1a2d46940f81a3c
- gwz-cli: 7db07bbdefd2897c07fd0f9e550bf032bd8b1314
- gwz-py: d07d55dacb1725d9306be9c04d157ac29a78e000
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348
- gwz-core-evidence: acd7e4f15f97be9ac4b6414a6e1099c932548df9
- Object: H2 correction 1 diff gwz-core 1d3a759acdcc0c977e24199e92f98cf8ca59a129..HEAD plus core/dev-docs/GwzRemoteTransportHttpsH2.md; root checkpoint and private H2 evidence.
- Controlling DRAFT document: gwz-core/dev-docs/GwzRemoteTransportHttpsH2.md at 7a5195eca4cdfcd10ede7c3aafa16b2dcdcfb2bc
- Out of scope: four old untracked N2b prompt files; this review prompt and other H2 prompt/report outputs.

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md; CurrentProgramCheckpoint.md; review-loop skill.
- Controlling documents: gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md (especially §8 H2), GwzRemoteTransportHttpsH1.md, GwzRemoteTransportPlacementC.md, GwzRemoteTransportPlacementDesign.md and GwzRemoteTransportRequirements.md; core/docs/Embedding.md and TransportPlacement.md. Resolve historical filenames from live files if moved.
- Explicitly deferred: platform and selected-source qualification together; production/public constructor activation, physical wire/iroh, external GitHub/real-account tests, system TLS/proxy parity and release. Deferrals cover outcomes only; integration shape and lifecycle remain in scope.

REVIEW AREAS
- Correction 1: read dev-docs/GwzRemoteTransportHttpsH2-RemPlan-1.md and your original H2 report; the merged plan/prior reports are legitimate prior-round input. Re-trace your ORIGINAL counterexamples on this tuple. Focus on the changed range; do not repeat the full initial review. Add a prior-finding closure table and changed-range analysis; label any NEW ARCHITECTURAL root cause explicitly for the two-round cap. Original Code P2-1 and State P2-2 are the same retry correlation root.
- Private HTTPS endpoint integration in transport_host/{mod,session,session/driver,request,https_endpoint}.rs; no shadow carrier/protocol, real mux receipts and request lifetime.
- All network funnels and same-process Rust/Python embedding; capability mismatch before effects; local HTTP/git compatibility; private refusal classification; truthful observations. Inspect tests and runtime paths, not counts alone.
- Shared physical capacity in endpoint/{shared_reservation,ssh_local,https_worker}.rs. Per-route callback ownership in gitbackend/transport_binding.rs and endpoint/https_remote.rs.
- Exact H2 scope/evidence; Placement C State P3-1 cleanup assertion and gated physical disposal coverage.
- State emphasis: cancellation at preparation/stream/HTTP/header boundaries; route and helper retirement, detached runtime lifetime, terminal ordering, shared reservation accounting, failed cleanup truthfulness and reuse after failure.

COMMANDS
Working directory /Users/owebeeone/limbo/gwz-dev. Read-only rg/sed/cat/git show/diff/log/status/rev-parse and private evidence reads are allowed. No build or dependency resolution; disk is limited. Optional targeted execution of the already-built binary /tmp/gwz-placement-b-backend-target/debug/deps/gwz_core-93f14a6e615c0a84 with exact filters and --test-threads=1 is allowed; tests create only disposable temporary fixtures. Source files and manifests under review must not be mutated. Verify all nine HEADs at start/end. Never read the other axis's current report or prompt. Return full report only, do not write report yourself.

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
