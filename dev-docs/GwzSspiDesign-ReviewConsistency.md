# GWZ SSPI design package — CONSISTENCY-AXIS REVIEW

**Review object:** DRAFT SSPI design package, controlled by `dev-docs/GwzSspiDesign.md`, revision 1, at root `377c5e29882e353e42dbe75031b175374b213f7f`; dated 2026-10-03.

**Baseline:** Root `377c5e29882e353e42dbe75031b175374b213f7f`; gwz-core `d78a664e3c5a325c6f12be409eb7645c1c1b51d0`; evidence `1beb1d204c824701ddbd033c7f89df9a3561f5e5`. Review sources were read as committed bytes through `git show`, with targeted `nl`, `sed` and `rg` inspection.

**Date:** 2026-10-03.

**Axis:** Consistency against the controlling document graph, including internal contracts, supersessions, lifecycle shape and satisfiable implementation obligations. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks; two P3 findings are nonblocking. I pre-commit to GO on a revision that resolves P2-1 as specified, provided the correction introduces no material contract deviation.

---

## 0. Evidence base

The start and end checks independently returned the exact prescribed tuple:

| Repository | Start HEAD | End HEAD |
|---|---|---|
| root | `377c5e29882e353e42dbe75031b175374b213f7f` | unchanged |
| gwz-core | `d78a664e3c5a325c6f12be409eb7645c1c1b51d0` | unchanged |
| gwz-core-evidence | `1beb1d204c824701ddbd033c7f89df9a3561f5e5` | unchanged |

Inspected:

- Root `GwzSspiDesign.md`, lines 1–251; `GwzSspiPlan.md`, lines 1–39; and `GwzSspiCallerGuide-DRAFT.md`, lines 1–74.
- `GwzCoreSessionCrateMap.md`, particularly its pending SSPI exception, §1 dependency/protocol/secret rules and §§7–9 authority and review obligations.
- `GwzLocalCloneLibraryBoundaries.md`, particularly §§1–3 and §§5–6: standalone boundaries, public contracts, test isolation and dependency enforcement.
- Core `GWZDesign.md` and `GWZRequirements.md`: pending SSPI banners, accepted helper amendments, session ownership, environment and child-process rules, and public-protocol separation.
- `GwzTransportWindowsParityDesign.md`: the explicit SSPI-only amendment and §§2, 7–11; exact Digest input requirements at lines 316–323 and route/retry rules at lines 334–355.
- `GwzTransportCredentialHelpersDesign.md`: authentication selection, lookup/admission clocks, secret ownership, connection scopes, rejection and retry, and its supersession/test sections.
- `GwzTransportCredentialHelperTimingAmendment.md`: exact helper provenance, budget bounds and supersessions.
- `GwzTransportSshHelperClockAmendment.md`: shared clock authority, phase publication, immutable terminal results, retained ownership and required regression boundaries.
- `GwzTransportCredentialHelperConfigurationViewAmendment.md`: configuration ownership, preparation clocks, retained cleanup and precise supersessions.
- The three feasibility documents linked from design §7: `GwzTransportWindowsAuthAlternativeFeasibility.md`, `GwzTransportWindowsSspiWorkerFeasibility.md` and `GwzSspiCreationContainmentFeasibility.md`.
- `AgentProcessRules.md`, `GwzProcessOptimization.md`, and `/Users/owebeeone/.claude/skills/review-loop/SKILL.md`.
- Allowed root/core package diffs, including the pending-authority banners.

The lane owner corrected an accidental Surface-only restriction in the prompt during review. I had already followed the Consistency mandate and read the design and controlling graph; the clarification did not change the evidence or verdict. No current-round peer report was read or requested.

No files were written, builds or tests run, or repository state mutated. This review checks what the DRAFT contract permits and requires; it does not claim implementation evidence.

## 1. Findings

### [P2-1] The closed worker protocol cannot deliver Digest’s required initial challenge

**Location:** `GwzSspiDesign.md`, lines 131–138; `GwzSspiCallerGuide-DRAFT.md`, lines 23–24 and 31–36; controlling Windows parity §8, lines 316–323.

**Root-cause classification:** Architectural interface-contract omission: a required native input has no legal carrier in the frozen conversation bootstrap.

**Violated invariant:** The caller contract and private IPC must express every input required by the retained native authentication contract. Deferring Digest qualification does not defer the shape of the advertised Digest API.

Windows parity §8 requires the Digest input to include “the entire validated challenge token” alongside the actual method and percent-encoded URI. The caller guide consequently requires an initial challenge in `AuthRequest`.

The design declares protocol v1 closed. Its `Begin` payload includes selected package, target, identity, CBT and optional Digest method/URI, but no initial challenge. The first `step(None)` sends `Begin` and performs the first Initialize. Subsequent Initialize calls run only from `Challenge`.

**Reproduction:** Construct a Digest `AuthRequest` containing synthetic explicit credentials, method, URI, binding and initial challenge C. Complete `start`, then invoke the mandated `step(None)`. The worker receives `Begin` without C and must perform the first Initialize. Sending C through a later `Challenge` is too late; using `step(Some(C))` first violates the public API. Adding an undeclared field or an extra pre-Initialize exchange changes the closed protocol.

**Impact:** A conforming implementation cannot satisfy both the public Digest request contract and the specified worker sequence. This is a mechanism/API defect independent of whether the native Digest parity campaign ultimately succeeds.

**Required correction:** Give the initial Digest challenge an explicit bounded, zeroizing carrier in `Begin`, define its presence/absence validation by package, and state that the first Digest Initialize consumes it. Alternatively, remove Digest from this version’s API and protocol through an explicit scope amendment.

**Closure/regression test:** A fake-worker contract test must follow `start` → `step(None)` and observe the exact supplied synthetic challenge, method and URI at the first Digest Initialize boundary. Cover missing/oversized initial challenge and inappropriate challenge fields on other packages. These are future implementation tests; closure of this document finding first requires the corrected committed API/protocol text.

### [P3-1] Normal finish has no specified path for a successfully started, unbegun conversation

**Location:** `GwzSspiDesign.md`, lines 135–139 and 170–175; `GwzSspiCallerGuide-DRAFT.md`, lines 23–25.

**Root-cause classification:** Non-architectural lifecycle-contract incompleteness.

**Violated invariant:** Every publicly obtainable conversation state needs an explicit disposal path consistent with the advertised method contract.

`start` returns after verified Hello, before Begin or native authentication. The public `finish(self)` contract promises normal cleanup and confirmed exit without stating a precondition. The closed worker protocol specifies Finish as legal from a successfully begun nonterminal state, leaving the returned pre-Begin conversation outside that specified normal-finish domain.

**Reproduction:** Await `start` successfully, decide authentication is unnecessary, and call `finish` without calling `step`. The caller has followed the stated API. The implementation must invent either pre-Begin Finish handling, an EOF-based normal shutdown path, or a refusal/forced-cancellation outcome.

**Impact:** Callers and implementers cannot determine whether this ordinary disposal sequence succeeds normally or returns a protocol/state error. The explicit cancel/drop path bounds the consequence, so this is P3.

**Required correction:** Specify normal finish from the post-Hello/pre-Begin state, including worker exit and confirmation behavior with no native handles initialized; or document an explicit `finish` precondition and the required disposal method for that state.

**Closure/regression test:** Exercise `start` → `finish` with zero Initialize calls. Assert the documented outcome, no token publication, confirmed process/Job/thread cleanup on successful normal disposal, and permit release only after confirmation.

### [P3-2] The implementation plan does not schedule its mandatory library secret-boundary reviews

**Location:** `GwzSspiPlan.md`, lines 8–22 and 35–36; `GwzSspiDesign.md`, lines 245–247; `GwzProcessOptimization.md`, §8.

**Root-cause classification:** Non-architectural review-plan omission.

**Violated invariant:** The implementation schedule must preserve the explicitly required per-step reviews at secret boundaries.

Design §7 requires “per-step reviews at secret/interface boundaries.” Process optimization §8 retains per-step dual review for secret handling. Plan step 1 introduces secret codecs and the caller API; step 3 introduces native credential storage and disposal. The plan explicitly schedules a kernel review in step 2 and adapter reviews in step 4, but groups steps 1–3 into one reviewable library chunk without naming the separate secret-boundary gates.

**Reproduction:** Execute the numbered plan literally: implement secret codecs, implement/review the kernel, implement native credential handling, then proceed to host composition. No explicit gate prevents proceeding beyond either library secret boundary before its required dual review.

**Impact:** The plan is not a reliable executable schedule of the design’s review obligations. The design and process authority still require those reviews, which bounds this to P3 rather than granting an actual waiver.

**Required correction:** Name the codec/API and native secret-handling dual gates and their stop points. Keep cohesive implementation chunks and avoid per-function review; those choices are compatible with explicit secret-boundary gates.

**Closure/regression test:** A cold walkthrough of the revised plan must identify the required Consistency/Safety or Code/State review before proceeding beyond each named library secret boundary, plus the existing composition and aggregate gates.

## 2. Invariant analysis

**Bounded ownership and admission:** The design consistently charges Launching, Authenticating, Closing, Reaping and Quarantined records. Caller future drop transfers cleanup to context-owned storage; only Reaped releases capacity. I found no text authorizing replacement workers against quarantined slots or detached, uncharged native calls.

**Creation and parent loss:** Creation-time Job attachment, explicit handle inheritance, suspension, membership verification and fail-closed launch agree with the creation-containment report. The new amendment clearly distinguishes this primitive from the historical suspended-then-assigned experiment and from the unchanged Git-helper launcher.

**Cancellation versus completion:** Publication revocation precedes termination and reaping. Late frames cannot publish or revive a conversation. Kill requests, EOF and PID disappearance are explicitly insufficient cleanup evidence. Retained ownership survives async-runtime teardown. These provisions defeat the obvious cancellation/early-capacity-release counterexamples at the design level.

**Identity and connection binding:** Primary-token SID/LUID/session matching and impersonation refusal narrow CurrentLogon honestly. They do not claim interactive identity parity. Binding belongs to the actual final-origin TLS connection; peer/connection change retires the conversation. The exact Digest bootstrap omission is reported separately.

**Secret codec and cleanup:** The schema must be taut-authored, and ordinary generated secret copies or Debug/Clone force a reviewed correction before production use. Normal cleanup and forced exit have distinct guarantees. The design does not infer physical erasure or external-provider cancellation from process termination. These are implementation obligations, not established production behavior.

**Deadlines and helper provenance:** One parent absolute deadline spans admission, launch, Hello and native rounds. SSPI does not acquire helper allowance, pause network clocks or fabricate `helper_budget_ms`. Accepted helper interaction/allocation and SSH phase mechanisms retain their own authority. No contradiction requiring a new helper timing amendment was established.

**Supersession and activation:** Windows parity’s explicit amendment limits acceptance to the SSPI boundary and leaves authentication policy, connection/retry rules and helper spawning in force. Pending banners do not activate product sources or qualify the full Windows draft. Historical source tuples are clearly identified. Standalone repository placement and private IPC are expressly proposed exceptions, with core application encoding retained.

**Host lifecycle:** CLI early self-dispatch and Python’s installed dedicated worker use the same library boundary. Worker paths are explicit and installed; mismatch is fail-closed. Application removal/upgrade includes its worker. No new public CLI setting or worker-selection environment variable is introduced.

**Evidence satisfiability:** Feasibility claims distinguish controlled worker stalls from blocked provider preemption, and creation containment from authentication qualification. Future fake/native/composition tests are appropriately described as obligations. P2-1 prevents one advertised API/protocol contract test from being satisfied without changing the contract; the remaining deferred qualification rows are not findings.

## 3. Risks and next action

Native provider behavior, interactive SSO, complete Digest/EPA/TLS-lifetime parity, external provider effects and installed host qualification remain explicitly deferred. Their absence does not independently block this bounded review.

The next action is one bounded document correction resolving P2-1, preferably also P3-1 and P3-2, followed by Consistency verification on a newly settled tuple. Do not begin implementation on this NO-GO contract.
