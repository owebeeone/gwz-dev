# GWZ Connection remediation plan — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzConnectionRemediationPlan.md` at root `468975076f94cd761aa07ba4b47c2c4af797a892`, dated 2026-10-19, status DRAFT. This reviews readiness to produce a bounded contract and small proof; it is not an API freeze, implementation acceptance or release gate.

**Baseline:**

| Repository | HEAD verified at start and end |
| --- | --- |
| root | `468975076f94cd761aa07ba4b47c2c4af797a892` |
| gwz-core | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| gwz-py | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| gwz-cli | `5193db962e672c54cd20ab2ef8448d904129a559` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| gwz-transport | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| gwz-sspi | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

The object and substantive document/source evidence were read through `git show <pinned-SHA>:<path>`. Current standing instruction files were read from disk.

**Date:** 2026-10-19  
**Axis:** Consistency against the controlling document graph, internal obligations, existing interfaces, requirement provenance and authorized scope, including the proposed lifecycle surface. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. The draft provides a coherent route to its next contract-review handoff. It does not authorize source changes or establish that the proposed library is already qualified.

---

## 0. Evidence base

Read and checked:

- The entire review object, lines 1–204.
- Root `AGENTS.md` and `AGENTS_GWZ.md`; member instructions in gwz-core, gwz-cli, gwz-git, gwz-transport and gwz-sspi. Checked the tuple members for member-local `AGENTS_GWZ.md`; none existed.
- Root `AgentProcessRules.md`, particularly the authority, characterization, ownership and amendment rules; L1-18 through L1-24; operator decisions, stop conditions and archive policy. Read `GwzProcessOptimization.md`, including its overriding review-granularity and two-reviewer rules.
- Root `GwzTransportReleaseRemediationPlan.md`, particularly §§1–4, M2–M5 and scope control. This supplies the Connection plan’s immediate operator-requirement and milestone context.
- Root `GwzConnectionReuseDesign.md`: endpoint/binding ownership; configuration and eligibility; cross-operation revalidation; capacity; authentication; lifetime; failure isolation; deployment; trust; accepted decisions and amendment/status trail.
- Root `GwzCoreSessionCrateMap.md` §§1–4 and placement decisions; `GwzLocalCloneLibraryBoundaries.md` §§1–3.
- Root `CurrentProgramCheckpoint.md`’s current entries, as historical context. Its old tuples and outstanding-work summaries were not substituted for the pinned source inventory.
- Core `GWZRequirements.md` and `GWZDesign.md`’s incorporated transport, session, credential-helper, SSH-clock, SSPI-composition and sequenced-stream amendments.
- Core `GwzRemoteTransportRequirements.md`, including G1–G7, C1–C8, placement/credential requirements and message-stream requirements.
- Core `GwzRemoteTransportDesign.md` §§1–4.1.1, §§7–10.2: dependency boundaries, in-process delivery, placement, eligibility, pool ownership, timeout domains and native timeout representation.
- Core `GwzGitLibraryDesign.md`, including ownership, dependencies and future remote-adapter responsibilities; `GwzGitLibraryApi.md`.
- Core `GwzTransportTR18-OperatorDecisions.md`’s controlling decisions and relevant authentication, HTTP-route and platform qualifications.

Pinned implementation evidence:

| Evidence | Checked consequence |
| --- | --- |
| Python `native/src/route/transport.rs:1–9,131–158,176–208` | Each transport operation receives its own route and invokes core’s cancellable transport entry. |
| Python `native/src/client_host.rs:32–86` | A Client retains a native authentication route and operation registry; this is not a persistent transport pool. |
| Core `src/transport_host/cancellable.rs:135–186,189–195,213–259` | The entry builds an owned runtime, finishes its request and shuts that runtime down. |
| Core `src/git/endpoint/https_worker/native/sspi.rs:18–70` | `NativeHost` owns a supervisor and spawn gate, not endpoint pools. |
| Core `src/transport_host/driver_tests.rs:75–160` | Existing reuse tests deliberately retain one runtime. They do not exercise successive Python caller-owned runtimes. |
| Core `src/transport_host/mod.rs:257–343` and `session/local_link.rs:1–109` | Current construction already connects driver and endpoint ports through an in-process message pump. |
| Core `session/capacity.rs:40–98`; transport `src/mux/mod.rs:242–268` | Registration retains lifetime identifiers and has finite exhaustion behavior; simply retaining the runtime is insufficient for indefinite reuse. |
| Transport `README.md:15–78,105–169`; `tests/pool_stream.rs:1–65` | Discrete-message stream and lease mechanics already admit a process-free fake endpoint proof. Physical delivery, timer service and health decisions remain host responsibilities. |
| Git `src/lib.rs:1–59` | The existing library is a read-only foundation, not an already implemented remote communication consumer. |

Inspection commands included `pwd`, start/end `git rev-parse HEAD` for every tuple member, targeted `rg`, pinned `git show`, `git log`, and read-only diffs. The core and Python source/manifest diffs from the audit baselines to the reviewed heads were empty. This supports the plan’s statement that subsequent archival commits did not change the cited implementation facts.

No files were written. No builds, tests, network operations, Git mutations or additional agents were used. No current-round peer report or excluded companion-draft prose was read.

## Problem, requirement provenance, and scope

The requested outcome is a Git-independent SSH and HTTP/HTTPS communication boundary, independently testable without the rest of GWZ, consumed by the Git interface and used by real CLI/Python callers. Authentication sources, secret ownership and package placement must become explicit design decisions. The operator also requires structural prevention of ownership mistakes and useful reuse in the transport release.

The observed ownership problem is confirmed. Python captures an operation route and enters `with_cancellable_local_transport_native_noted`; core constructs `Owned { executor, runtime }` and calls `runtime.shutdown()` when that operation ends. Retaining a Client’s SSPI supervisor and spawn gate does not retain that runtime’s pools. The internal driver test proves reuse when a runtime survives, which is a different composition.

The plan addresses that distinction directly. Its required owner at construction, separate operation bindings, isolated finish/cancel, finite lifecycle policies and real-consumer acceptance prevent a successful internal reuse test from being mistaken for Python reuse.

I considered these narrower routes:

- **Retain the current runtime unchanged:** insufficient. The pinned registration code retains identifiers and rejects exhaustion; the current capacity path also refuses conflicting live policies. The plan correctly requires lifecycle and compatibility decisions rather than prescribing indefinite retention.
- **Use existing transport state machines with fake connectors:** supported by concrete source evidence. `tests/pool_stream.rs` already demonstrates message/lease coordination without Git, network or a physical carrier. This supports the proposed early proof without establishing that the new owner already exists.
- **Require the whole session/server programme or a new carrier:** necessity is not established, and the plan explicitly excludes these as prerequisites. Existing `LocalLink` construction provides concrete evidence for an in-process route.
- **Upgrade or modify reference dependencies first:** no blocker establishing that necessity was found. The plan preserves separate evidence and authorization requirements.

The current editable scope is the Connection draft and its review records. Future implementation components are identified as proposals, subject to an approved cross-repository boundary and explicit authorization. git2-rs, libgit2 and other dependencies remain reference-only.

The proposed dependency and responsibility changes will affect existing crate-map and Git-boundary clauses. The draft recognizes that fact and requires the next package to identify affected authoritative clauses before implementation. It does not silently declare those clauses superseded.

**Architectural-root classification:** the existing per-operation ownership composition is an architectural problem being repaired. I found no new architectural root cause in the remediation plan and no architectural finding for the remediation-cap accounting.

## 2. Invariant analysis

### Responsibility and dependency boundaries

The proposed graph preserves the important separation: gwz-transport owns pure stream/mux/pool mechanics; Connection owns reusable resource coordination; gwz-git owns Git behavior and binding adaptation; core supplies workspace policy and composition.

I attacked the graph for a hidden core dependency and for absorption of workspace or application-result ownership. Neither was established. Lines 55–65 exclude application result registries, require ports without a dependency back into core and expressly withhold permission to create Cargo edges.

The current crate map permits only specified role/dependency arrangements and keeps GWZ application encoding in core. The plan does not purport to override that policy by drawing a diagram. Sections 5–7 require package dependencies, adapter placement and explicit amendments as outputs of the contract phase. The eventual package arrangement remains reviewable before it becomes implementation authority.

Git-service selection and smart-HTTP semantics moving behind the Git boundary are consistent with the requested enduring single-repository library. They require a later contract and migration; the draft does not claim the G0 library already provides them.

### Identity, configuration and authentication

The plan preserves username/host identity without repository-based pool identities, while allowing effective port and security inputs to partition eligibility. That is consistent with transport C2 and the reuse design’s configuration/identity separation.

It does not promise that idle expiry provides immediate credential revocation. Its references to the accepted reuse design preserve a route to the existing cross-operation possession/trust checks and accepted lifetime decisions. Those details must be made explicit in the forthcoming contract.

Section 5 correctly refuses to treat the original gh-only wording as the complete current authentication authority. The incorporated SSPI requirements and TR1.8 decisions demonstrate why reconciliation is necessary: Windows logon authentication, helper-backed Basic, Digest refusal and native HTTP routing have distinct accepted boundaries. The draft requires preservation of the approved matrix rather than granting arbitrary credential discovery.

The secret-placement discussion accurately identifies the controlling constraints: environment snapshots remain in core; named endpoint and SSPI exceptions do not constitute a general secret-placement exemption. Opaque capabilities or owned inputs and precise amendments must precede a move.

### Deadlines, failure and resource ownership

The plan requires fixed established deadlines, distinct timeout domains, reconciliation of zero behavior and retained cleanup capacity. It does not choose a new universal timeout value or erase the accepted distinctions between SSPI’s fixed logical Open deadline, SSH setup-clock accounting and active-I/O budgets.

Operation finish/cancel is separated from host shutdown. Fault scope and uncertain connection disposition are expressly required. The plan also distinguishes transport completion from Git success and forbids inferring replay safety from byte accounting.

The resource obligations are satisfiable without moving application result registries into Connection: transport terminal records, outstanding admissions, helper work, buffered bytes and identifier retirement can be bounded within their respective owners. The responsibility table prevents interpreting these obligations as authorization for an application-session subsystem.

### Sequence and evidence

The small fake proof precedes native adaptation and production migration. Its dependencies are the reviewed contract and selected placement, not a completed session host.

Acceptance distinguishes three different claims:

1. Independent tests establish library invariants.
2. Native tests establish adapter/platform behavior.
3. Actual client tests establish composition and reuse.

Counting actual connection creation avoids the existing internal-test/composition confusion. Seeded tests are required to respect the delivery contract, so varying interleavings does not authorize arbitrary loss or duplication behavior.

No acceptance clause requires network, Git or helper execution in the fast test tier. Native and consumer tiers are separately identified.

### Proposed lifecycle surface

Read cold, `ConnectionHost` communicates ownership of communication resources. Construction and shutdown are paired; operation admission/registration, stream acquisition, lease disposition, cancellation and observation are all included in the required sketch. No missing lifecycle half or settled misleading guarantee was identified.

The draft explicitly declines to freeze Rust signatures. Full API Surface review remains necessary at the contract phase, including defaults, cancellation outcomes, shutdown completion and remaining caller obligations.

### Authority and supersession

The plan makes no current supersession claim and contains no purported completed superseded-clause list to validate. Its next handoff requires explicit amendments, and step 1 requires review of affected authoritative clauses.

Archived material remains authority where applicable. Historical dangling citations were not treated as defects, and excluded companion drafts were not used to make this plan coherent.

## 3. Risks and next action

The contract phase still has consequential work: preserve the accepted cross-operation SSH checks; distinguish timeout domains precisely; reconcile HTTP’s current native-route disposition; allocate Git-specific HTTP behavior; and settle package edges, secret inputs and process-wide spawn obligations. These are explicit design outputs of this draft, not omitted implementation prerequisites or findings against its readiness.

The single next action is to produce the promised contract, placement comparison, clause-specific amendments and small-proof test specification for review. Source implementation remains separately authorized.
