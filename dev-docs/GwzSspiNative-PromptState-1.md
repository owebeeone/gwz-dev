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
  nothing else. It will be filed verbatim as dev-docs/GwzSspiNative-ReviewState-1.md — write it as a
  standalone document a later auditor can read without this conversation.

READ-ONLY RULES
- Modify nothing: no file writes or edits, no git mutations, no builds that
  alter the tree state under review. Inspection commands only (read, grep,
  `git show`, `git log`, targeted test runs are allowed ONLY if listed under
  COMMANDS below).
- Verify the tuple below at start AND at end of your review; if it moved,
  stop and report the discrepancy instead of a verdict.

EXACT TUPLE (the object under review — nothing else is in scope)
- gwz-dev: bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e
- gwz-sspi: 425e13dc011c42e94fdea31779a8e5967aedc82b
- reference gwz-core: 8cb3a3f01d79699a5ad07b6ec7cfc78321224d31
- gwz-core-evidence: 9adf06beab7a15d1e5c22ec1cecf8e35966e1d7c
- Object: remediation 1; member 610964282663c3b7844c9620d063e40d9ee76258..425e13dc011c42e94fdea31779a8e5967aedc82b; root 40fd121fbc2d9e6e727ed1a1b2501275200a1797..bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e
- Controlling DRAFT document: dev-docs/GwzSspiNativeCheckpoint.md and GwzSspiNative-RemPlan.md at root bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e (Surface reads caller docs only)
- Out of scope: untracked SSH N2b prompts/route draft, unrelated core bugreport/evidence alpha campaign, current review prompt/report outputs; adjacent members

AUTHORITY AND DEFERRALS
- Process authority: dev-docs/AgentProcessRules.md as amended by dev-docs/GwzProcessOptimization.md
- Controlling documents to check the object against: NativeCheckpoint/RemPlan and accepted GwzSspiDesign revision2, Plan step3, member Architecture/Testing/WorkerEntry/WireProtocol. Surface: caller docs only; no design/plan/code.
- Explicitly deferred (do not report as findings): Digest remains refused pending missing H(Entity) contract amendment; installed fingerprint producer and core/CLI/Python step4; full Windows qualification, completed remote authentication/TLS/EPA/blocked-provider/descendants; publication/activation/remote CI.
  Deferrals cover a decision's OUTCOME only. Its shape — the verb it lives
  under, its name, whether its lifecycle pair is complete, its defaults — is
  always in scope.

REVIEW AREAS
Retrace State P2-1 helper failure ownership (after spawn/PID/observer/unwind, held handles, finite actual wait, scratch disposal) and P3-1 suspended-child Job/parent-death evidence with fixture-owned no-kill discrimination. Inspect changed fixture guard, private creation extraction, query audit and all failure/unwind paths. Classify any NEW ARCHITECTURAL cause explicitly.

COMMANDS
cwd /Volumes/projects/limbo/gwz-dev. Read-only git rev-parse/status/show/diff/log, rg/cat/sed/nl/stat/file. Verify four HEADs start/end. No writes/builds/Git mutations/compiler probes/native campaigns/process execution. Owner corrected campaign is private campaign runs/2026-10-03-sspi-native-remediation-1; recorded native checks passed, final owned census 0. Code/State may inspect its raw receipts and runner evidence. Optional prebuilt pure Darwin binary /Volumes/projects/limbo/evidence-build-cache/gwz-sspi/native/debug/deps/gwz_sspi-e95571679a8a0127 filtered worker::tests or supervisor::fixture_cleanup only, no ignored tests. Surface no test execution; caller docs and own original report only. Do not read current peer reports/prompts.

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

FOCUSED RE-VERDICT
Continue your original context. Original reports are filed verbatim; merged dispositions are legitimate prior-round inputs. Verify your own original counterexamples; a claimed fix does not close a finding. Add prior-finding closure table and changed-range analysis before section0. Label NEW ARCHITECTURAL root causes explicitly; report whether the bounded patch changes the authority/architecture/proofs materially. Full final report and binary verdict required.

# GWZ SSPI native worker remediation 1 — State-AXIS REVIEW

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

Add before section0:
## Prior-finding closure table
| ID | Disposition claimed | Verified on corrected tree | Status |

## Changed-range analysis
State what changed and whether any change falls outside dispositions or changes material architecture/proofs.

