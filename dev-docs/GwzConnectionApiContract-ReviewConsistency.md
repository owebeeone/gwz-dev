# GWZ Connection API contract — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzConnectionApiContract.md`, revision 0, DRAFT, at root `d0848ed0fc0fc090cd7dde1a112e085b7e58505f`; dated 2026-10-19.

**Baseline:**

| Repository | Pinned HEAD |
| --- | --- |
| `.` | `d0848ed0fc0fc090cd7dde1a112e085b7e58505f` |
| `gwz-core` | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| `gwz-py` | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| `gwz-cli` | `5193db962e672c54cd20ab2ef8448d904129a559` |
| `gwz-git` | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| `gwz-transport` | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| `gwz-sspi` | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| `git2-rs` | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| `libgit2` | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All nine HEADs matched at the beginning and end. Substantive document and source inspection used `git show <pinned-SHA>:<path>`.

**Date:** 2026-10-19.

**Axis:** Consistency, including API Surface: internal coherence, controlling contracts, amendment precision, consumer feasibility and recurrence prevention. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks; two P3 findings are also recorded. No P0 or P1 findings. I pre-commit to GO on a revision that resolves P2-1 as specified, provided the correction introduces no additional defect.

---

## 0. Evidence base

The public capabilities/signatures and first-use material in §§4, 11 and 12 were read before the controlling design/source investigation. The complete 684-line contract was subsequently inspected.

The consequential evidence inspected was:

- Root `AGENTS.md` and `AGENTS_GWZ.md`.
- `dev-docs/AgentProcessRules.md`, including L1-08/09, L1-18 through L1-22, L1-33 and structural-boundary rules; `GwzProcessOptimization.md`, including remediation accounting and the current two-reviewer limit.
- `/Users/owebeeone/.claude/skills/review-loop/SKILL.md`.
- `GwzConnectionRemediationPlan.md`, lines 1–208, and its merged verdict, lines 1–68.
- `GwzConnectionReuseDesign.md`, particularly §§1–12, its amendment list and incorporated-amendment notices.
- `GwzCoreSessionCrateMap.md`, lines 1–207, and `GwzLocalCloneLibraryBoundaries.md`, particularly §§1–3.
- Core `GWZDesign.md`, including the current native, session, environment and ownership amendments; relevant `GWZRequirements.md` transport/session provisions.
- Core `GwzRemoteTransportRequirements.md`, especially G6, C1–C8 and placement rules; `GwzRemoteTransportDesign.md`, especially §§2–3 and §7.
- Core `GwzGitLibraryDesign.md`, lines 1–198, and `GwzGitLibraryApi.md`, lines 1–130.
- `GwzWindowsHttpsIntegrationDesign-DRAFT.md`, especially §§1–5 and its D19 clock amendment.
- `GwzSspiHttpsCompositionDesign-DRAFT.md`, especially §§1–7.
- Core `GwzTransportTR18-OperatorDecisions.md`, including TD1–TD4, D6, D19 and later process, proxy and production-integration decisions.
- Incorporated core `dev-docs/history/reference/GwzTransportSequencedStreamDesign.md`, lines 1–45.
- Pinned transport source:
  - `Cargo.toml`, lines 1–32;
  - `src/lib.rs`, lines 1–24;
  - `protocol/transport.taut.py`, lines 1–121;
  - `src/sequenced.rs`, particularly lines 65–109, 157–219 and 270–402;
  - `src/stream/mod.rs`, lines 1–169;
  - `src/stream/incoming.rs`, lines 1–80;
  - `src/mux/mod.rs`, lines 225–325;
  - `src/codec/validate.rs`, lines 39–210;
  - `tests/sequenced.rs`, lines 1–180.
- Actual-consumer evidence:
  - Python `native/src/route/transport.rs`, lines 1–235;
  - core `src/transport_host/cancellable.rs`, lines 1–160;
  - core `src/git/endpoint/https_remote.rs`, lines 1–180;
  - core `src/git/endpoint/https_worker/serve.rs`, lines 60–180.

Commands were inspection-only: `git rev-parse`, pinned `git show`, `rg`, `cat`, `nl`, `sed` and read-only Python parsing of the assigned canonical prompt. No files were written, builds/tests run, network accessed, dependencies changed or further agents launched. No current-round peer report or excluded companion draft was read.

## Problem, requirement provenance, and scope

The requested outcome is a concrete, reviewed communication API that addresses recurring ownership churn: independent Git-free tests, explicit persistent ownership across operations, explicit authentication/trust boundaries, restored production consumption through gwz-git and structural prevention of the audited mistakes.

The concrete lifetime problem is confirmed by the supported route. Python’s pinned route explicitly stores an operation’s environment/token and invokes the cancellable core entry for that operation (`native/src/route/transport.rs:40–46, 176–207`). That core entry describes and owns runtime construction, finish and shutdown (`cancellable.rs:9–13, 25–38`). Retaining a native authentication host consequently does not retain the transport connection pool. The existing reuse design separately identifies finite mux registrations and mixed operation/instance ownership as reasons that merely retaining today’s runtime is insufficient.

The selected owner/binding separation addresses that demonstrated problem. The selected crate location is a proposal requiring adoption, not existing implementation authority. Its dependency direction preserves the pure transport kernel, supplies native mechanisms through core adapters and avoids a reverse dependency into core. A new repository, dependency upgrade, physical carrier or complete session programme is not shown necessary, and the contract does not make one a prerequisite.

The authorized editable scope remains this contract and its review records. All source repositories and upstream dependencies are reference-only in this review. Native qualification, implementation, packaging and release activation remain later gates; the promised useful production outcome remains required.

The unresolved prerequisite is narrower: the contract requires its generic communication API to use an existing sequencing interface whose opening grammar is Git-specific, without selecting the seam that reconciles them. This is P2-1. Its remedy can remain inside the proposed communication crate and a bounded pure-kernel interface change; it does not justify widening the programme.

Existing process-launch and platform-qualification shortcomings are already identified as external qualification obligations. They are not additional findings against this document gate merely because they remain unfinished.

## 1. Findings

### [P2-1] Generic exchanges have no specified entry into the required sequencing kernel

**Location:** Contract §6, lines 245–273; §7.2, lines 362–372; §9, lines 455–479; §13, lines 605–609; §15, lines 666–671.

**Violated invariant:** The independent generic API must be implementable through its specified ports and kernel, without inventing Git semantics, a shadow opening protocol or scheduler-owned ordering.

The contract requires every bound adapter to feed the sequenced kernel before stream application. It also requires generic SSH exec and HTTP exchanges, keeps existing envelopes as a profile decoded/encoded by core, and says generic in-process callers need no alternate serialized Open format.

The pinned kernel does not expose an already-open generic-stream entry:

- `src/sequenced.rs:84–95` permits only `Open` or `CheckIdentity` as the initiator’s first message.
- `Outbound::enqueue` rejects a different first message at lines 178–186.
- Admission validates the complete envelope and its body.
- The authored `Open` requires `GitService` (`protocol/transport.taut.py:83–86`); its service enumeration contains only the four Git operations (`:18–19`).
- `CheckIdentity` is an explicit-key check, not a generic stream-opening substitute (`src/codec/validate.rs:193–200`).
- The existing stream machine independently accepts only profile 1/2 configuration (`src/stream/mod.rs:90–92`); incoming messages must match that configured version (`src/stream/incoming.rs:18–27`).

These facts do not mean implementation must already exist. They mean that “use the current sequenced profile” leaves an interface decision unresolved.

**Credible trace:**

1. The standalone fake proof constructs a host without core or Git.
2. It opens `SshExec` for an arbitrary non-Git command, or a generic HTTP request.
3. Its adapter establishes the exchange through typed Rust commands.
4. The first initiator body/control message reaches the required sequencing kernel.
5. Starting with Data/EndWrite fails the existing first-message rule. Starting with Open requires manufacturing a Git service and a context whose relationship to the generic command is unspecified.
6. Feeding profile-3 output into the current stream machine also requires an unspecified integration seam.

The contract does not choose whether generic opens are admitted outside the Git envelope, whether the sequencing kernel gets a neutral already-open entry, or how that entry and the existing Git profile share stream identity, ordering and terminal authority.

**Impact:** The portable proof cannot establish the advertised generic, protocol-ordered composition from the specified interface alone. An implementer must make an architectural choice during implementation, potentially introducing precisely the hidden protocol or Git coupling the contract forbids. A later conformance gate can detect failure, but does not settle this design boundary.

**Required correction:** Select and specify the smallest neutral kernel seam. For example, define explicit generic opening/registration authority and how an already-admitted stream enters sequencing, while retaining the existing Git Open profile in its adapter. State the profile/version transition into the byte-stream machine, sequence ownership and terminal receipts. Add the exact affected sequencing-contract clauses to §15. No new serialized schema is required merely to make that local seam explicit.

**Smallest closure test:** At document re-review, walk a non-Git SSH exec and generic HTTP request through the selected typed entry, first data, reordered controls and terminal settlement, naming each interface transition. Specify a subsequent portable fake test that uses the actual selected sequencing/stream seam, manufactures no GitService value for generic traffic, and demonstrates independent progress when another opening stalls. Execution belongs to the authorized implementation gate.

**NEW ARCHITECTURAL root cause: YES.** This is one root: an unresolved boundary between generic exchange admission and the Git-specific sequencing/profile interface. The version-transition evidence is part of that same missing composition decision, not a separately counted root.

### [P3-1] CleanupDomain has a required final lifecycle but no public lifecycle surface

**Location:** Contract §4, lines 83–134 and 155–160; §5, lines 212–217; §12, lines 577–587.

**Violated invariant:** A public resource owner needs a complete construction/teardown recipe, especially when it retains work after host teardown.

The domain is required in HostConfig, must outlive every host, retains failed-host work and owns final shutdown at the composition root. The public surface and walkthrough cover only HostOwner shutdown. They provide no domain construction/sharing/final-shutdown operation or drop outcome.

**Credible trace:** A composition creates two hosts on one domain. Closing the first returns Pending; closing the second completes. The composition now needs to terminate the domain while preserving the first ticket’s resource ownership. The public contract does not state the operation to perform, whether its shutdown is awaitable/bounded, or the consequence of dropping the final domain authority.

**Impact:** Independent adapters and caller integrations must invent this final lifecycle. The omission weakens the otherwise explicit ownership recipe and makes teardown conformance ambiguous.

**Required correction:** Name the domain construction and sharing capabilities and its final shutdown/drop semantics. A small owned owner/handle recipe is sufficient; this does not require a general process framework.

**Closure test:** Extend the public walkthrough with two hosts sharing one domain, one pending disposal, both host closes and final composition shutdown. Identify who retains the pending generation and how its ticket remains observable.

**NEW ARCHITECTURAL root cause: NO.** The domain’s responsibility is already selected; this completes its public lifecycle description.

### [P3-2] InputBudget is required at admission but placed in the opening/stream error surface

**Location:** Contract §10, lines 507–515; §11, lines 538–545.

**Violated invariant:** A specified failure must be representable in the error type returned at its required failure boundary.

Section 10 requires retained-charge exhaustion to return `InputBudget` before registration. `admit` is the validation/reservation entry. Section 11’s admission-error list omits InputBudget, then places it with opening/stream errors.

**Credible trace:** An AuthContext reports a retained charge above the per-admission cap. `HostHandle::admit` must reject it before registration, but its enumerated AdmissionError outcomes do not contain the prescribed category. Projecting InvalidInput or waiting until open would contradict one of the two sections.

**Impact:** Callers and conformance fixtures receive inconsistent instructions for classifying a bounded-input refusal.

**Required correction:** Put InputBudget in AdmissionError and state whether build-time configuration limits use BuildError or the same subordinate reason.

**Closure test:** Specify an over-budget AuthContext admission that returns `AdmissionError::InputBudget` with no registration, endpoint construction or physical effects.

**NEW ARCHITECTURAL root cause: NO.** This is a bounded error-surface correction.

## 2. Invariant analysis

**Cold API walkthrough.** Once supplied configuration and ports exist, the host/operation lifecycle is understandable: build returns separate owner and submission capabilities; admission precedes registration; cancellation authority exists before open; finish consumes the operation; owner close seals the host. HTTP response-head retrieval has explicit once-only, cancellation and wrong-scheme behavior. No public healthy-return boolean lets an application pool an uncertain lease. The walkthrough stops being complete at generic sequencing entry (P2-1) and final CleanupDomain teardown (P3-1). Input-budget refusal has the bounded error mismatch in P3-2.

**Owner isolation held under the textual attack.** HostHandle and OperationCancel cannot close a host. Operation finish/drop affects its binding; shared physical failure is distinguished from logical cancellation. Separate binding IDs and compound routing keys prevent request-ID collisions between operations. Checked identifiers and per-binding lifetime ceilings avoid making the old 256-request limit a host-lifetime rollover condition.

**Admission and cleanup rules are materially stronger than the audited route.** The text reserves finite admission/control storage before workers, makes registration one synchronous transition and explicitly accounts for unwind after insertion. Cancellation and pending disposal do not free physical capacity. Late/duplicate completion carries generations, while dropped tickets have no release authority. These rules prevent several known ownership mistakes without claiming that opaque types prove trusted native adapters correct.

**Package direction and scope held.** The proposal keeps gwz-transport pure, puts coordination above it, and initially leaves native mechanisms behind core adapters. The placement exception and reduced communication owner are expressly proposed amendments. The small proof precedes native qualification and actual consumer migration. No dependency upgrade or whole-session implementation is smuggled into its prerequisites.

**Reuse and authentication coherence mostly held.** Capacity follows the accepted no-install/no-clamp semantics rather than reinstalling a live pool’s limits. Idle/max-age behavior and per-operation SSH possession/trust checks follow the reuse design as amended. The contract distinguishes request-scoped HTTPS credentials from native connection-bound security scope. It retains plain HTTP’s current native/local compatibility route and refuses unsupported selected placement before effects.

**Clock exceptions were not treated as contradictions merely because earlier prose is stricter.** The current Windows integration amendment expressly names D19’s helper pause and adaptive new-attempt disposition. The contract preserves those named transitions, native timeout-zero refusal and fixed native cutoffs rather than reviving an unconditional older “no pauses” sentence. Native qualification and process-safety claims remain gates, not accepted runtime facts.

**Consumer proof is correctly separated from contract proof.** The pinned Python/core route confirms the lifetime defect. The proposed migration requires actual core → gwz-git consumption and real Python operations; internal host reuse counters alone cannot pass it. The synchronous libgit2 bridge is explicitly assigned to a worker independent of message/timer progress.

**Sequenced feature status is stated honestly.** The source’s non-default, unstable and unreviewed status agrees with §6 and §15. Document GO would not activate that feature. P2-1 concerns the missing composition decision, not a demand for premature feature qualification.

## 3. Risks and next action

Native ProcessSafety qualification, installed-worker behavior, platform identity/trust semantics and actual consumer adoption remain substantial later risks. This review neither qualifies them nor makes them prerequisites for the portable fake proof.

The next action is a bounded contract revision selecting the generic sequencing/stream seam and its exact amendments, with the two smaller public-surface corrections included. Re-review that changed interface against a newly pinned document. Source implementation, native qualification and release activation remain unauthorized by this verdict.
