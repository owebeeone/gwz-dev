# GWZ Connection remediation plan — SAFETY-AXIS REVIEW

**Review object:** `dev-docs/GwzConnectionRemediationPlan.md` at root `468975076f94cd761aa07ba4b47c2c4af797a892`; DRAFT dated 2026-10-19. Plan readiness only.

**Baseline:**

| Repository | SHA |
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

The object and substantive source evidence were read using `git show` at these revisions. Current instruction files and process rules were inspected separately.

**Date:** 2026-10-19  
**Axis:** Safety — ownership, cancellation, degraded paths, cleanup, authentication, disclosure, sustained bounds and scope. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. This approves the draft’s route to a reviewed contract and bounded proof. It does not approve an API freeze, implementation, activation or release.

---

## 0. Evidence base

Inspected:

- The complete review object, lines 1–204.
- Root `AGENTS.md`, `AGENTS_GWZ.md`, and current member instructions in core, CLI, Git, transport and SSPI. Checked for top-level instruction files in Python, git2-rs and libgit2; none were present.
- `AgentProcessRules.md`, particularly amendment discipline, independent review, severity and actionable-finding rules; `GwzProcessOptimization.md`, particularly platform feasibility, review limits and the current review-granularity amendment.
- The predecessor `GwzTransportReleaseRemediationPlan.md`, particularly §§1–4 and M2–M4.
- `GwzConnectionReuseDesign.md`, particularly §§1–12, its amendment notices and verification requirements.
- `GwzCoreSessionCrateMap.md` §§1–4 and `GwzLocalCloneLibraryBoundaries.md` §§1–3.
- Targeted requirements/design extracts from core’s `GWZRequirements.md` and `GWZDesign.md`, covering session ownership, environment capture, transport compatibility and communication layering.
- Core’s `GwzRemoteTransportRequirements.md` C1–C8, P1–P9 and S1–S9; corresponding ownership, lifecycle, pooling and replay clauses in `GwzRemoteTransportDesign.md`.
- Core’s `GwzGitLibraryDesign.md`, including ownership and future-operation boundaries; `GwzGitLibraryApi.md`, including the existing G0 limitations.
- Core’s `GwzTransportTR18-OperatorDecisions.md`, including authentication, plain HTTP, trust sources, spawn-gate and later mechanism decisions.
- Current entries in `CurrentProgramCheckpoint.md` as historical context, without treating them as an October-19 source inventory.

Pinned source checks:

- Python `native/src/route/transport.rs:1–210`: captures operation inputs and invokes the per-operation cancellable transport entry.
- Core `src/transport_host/cancellable.rs:1–311`: constructs an operation-owned runtime and shuts it down after the action.
- Core `src/transport_host/driver_tests.rs:75–160`: reuse tests retain one runtime across requests; overlapping admission is tested within that retained runtime.
- Transport `src/mux/mod.rs:242–294`: request tombstones consume a lifetime registration bound; bootstrap belongs to the mux session.
- Core `src/git/spawn_gate.rs:1–95`: explicitly documents that separate hosts have separate gates.
- Core `src/git/endpoint/https_worker/native/sspi.rs:18–87`: constructs a native host with its gate and supervisor launch guard.
- SSPI `src/supervisor/values.rs:50–94`: provides the existing launch-guard callback and documents retained accounting while acquisition waits.

All nine repository heads matched the required tuple at the start and end. Inspection used read-only commands only. No files, builds, tests, network or host operations were performed. No current-round review report was read.

## Problem, requirement provenance, and scope

The requested outcome is a small, Git-independent communication library covering SSH and HTTP/HTTPS, independently testable and consumed through the Git boundary, with useful CLI/Python reuse and structural protection against ownership mistakes.

The observed lifetime defect is concrete. Python’s transport route gives each operation its own runtime, and core’s cancellable entry ends that runtime after the operation. Sharing a native authentication host does not preserve the transport pool. Existing internal reuse tests retain their runtime explicitly and therefore cannot establish reuse through the Python client’s actual lifecycle.

The proposed remedy addresses that defect at its ownership boundary. It requires the persistent owner at construction, separates operation bindings from shared resources, and requires production consumers to demonstrate actual connection reuse. Retaining today’s runtime indefinitely is correctly rejected as sufficient: the mux retains registration tombstones to a finite lifetime bound, and its bootstrap is session-scoped.

The plan’s broader constraints trace to existing authority:

- Endpoint-owned reuse, bounded allocation, explicit identity compatibility and request-scoped HTTP authentication come from the accepted transport requirements.
- Per-operation bindings, physical-fault scope and cross-operation authentication checks come from the accepted reuse design, subject to its amendments.
- Core snapshot placement, narrow ports and independent library tests come from the crate map and library-boundary rules.
- The Git-independent library and restored Git consumption come from the operator’s remediation request.

No dependency upgrade, new repository, daemon, carrier or whole-session host is established as a prerequisite. Existing source provides concrete evidence that eligible reuse already works inside a retained runtime and that SSPI already exposes a launch-guard seam. This supports investigating a localized ownership/composition repair before proposing infrastructure changes; it does not prove the eventual contract or native composition.

The authorized editable boundary is the Connection plan and its review records. Core, transport, Git, CLI, Python and SSPI are potential later implementation participants only after explicit boundary approval. git2-rs, libgit2 and other dependencies remain reference-only. The other three remediation drafts were not used to supply missing requirements.

**Architectural-root classification:** the existing operation/runtime lifetime coupling is structural and is appropriately addressed by this plan. No new architectural root cause was identified in the reviewed draft.

## 2. Invariant analysis

**Operation cancellation must not own shared-host shutdown.**  
I attacked cancellation before registration, after insertion, during lease acquisition and during cleanup. Clauses 1–5 require construction ownership, distinct resource scopes, explicit registration provenance and retained capacity until cleanup is known. These requirements prevent copying the current operation-owned shutdown path unchanged into the persistent API. Exact registration outcomes remain required contract outputs, rather than unsafe defaults prescribed here.

**Physical faults must have an honest scope.**  
The plan assigns fault scope to Connection and requires failure testing. Its incorporated reuse design distinguishes binding failures from physical-owner failures, including failures that terminate active exchanges across bindings. Operation isolation therefore does not establish immunity from shared physical failure. No present clause authorizes returning failed or uncertain resources as healthy.

**Long-lived reuse must remain bounded.**  
Clauses 8 and 15–16 cover endpoint capacity, idle reaping, waiting admissions, helper/native work, terminal retention, buffered bytes and identifier retirement. Testing beyond the existing lifetime-ID bound directly attacks the known failure of simply retaining today’s runtime. The plan expressly preserves stale-message protection.

**Authentication and configuration must not cross operation boundaries accidentally.**  
Clauses 6–10 distinguish compatibility, authenticated identity, trust, proxy routing and operation-local inputs. Section 5 requires reconciliation with the approved credential matrix and existing secret-placement exceptions. It does not authorize arbitrary credential sources, transferring core snapshots into a library, or inventing immediate revocation guarantees for busy SSH connections.

**HTTP transport reuse must not imply account or Git-result reuse.**  
The plan separates generic HTTP connection/authentication mechanics from Git service semantics and requires that ownership decision before moving code. Clause 17 rejects inferring Git success or safe mutation replay from transport accounting. Existing C3/C8 and stream-lifecycle requirements continue to constrain stale replacement, request-scoped credentials and cleanup.

**Native process safety must not be claimed from a per-host mutex.**  
The source confirms the cross-host limitation: separate hosts construct separate gates. Section 6 explicitly requires resolving that hazard and prohibits claiming process-wide exclusion from a per-host gate. Thus the known hazard remains an explicit design obligation rather than an accepted safety property.

**Independent tests must lead to real consumer evidence.**  
The small proof uses an explicit owner and deterministic providers. Sustained and failure proofs follow before consumer migration. Acceptance additionally requires native qualification and real CLI/Python composition, measuring physical connection creation. An unused extraction or internal hit counter cannot satisfy completion.

These attacks did not establish a consequential defect in the plan’s prescribed rules or sequence.

## 3. Risks and next action

The main remaining risk is in the forthcoming contract: it must turn the stated obligations into precise registration outcomes, lease dispositions, cleanup accounting, fault scope and identifier retirement. The native decision must state its process-safety coverage, including multiple independently constructed clients and any caller obligations; sharing one client’s gate is insufficient evidence.

Authentication reconciliation must preserve the later approved platform matrix and cross-operation SSH checks without silently importing obsolete assumptions from the older reuse design. Native feasibility must precede freezing any newly proposed platform primitive, as the process amendment requires.

These are explicit next-stage obligations, not defects requiring a finished API at this draft-plan gate.

**Next action:** prepare the contract, placement comparison, named authoritative amendments and small-proof test specification described by the plan’s handoff. Keep source and dependency changes behind the stated implementation authorization boundary.
