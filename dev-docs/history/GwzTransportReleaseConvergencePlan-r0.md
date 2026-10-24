# GWZ transport release: useful scope and convergence plan

Date: 2026-10-19. Status: **DRAFT — release recovery proposal; not a release GO,
scope amendment, implementation authorization or new deferral.**

This is the fourth plan under the
[remediation process](GwzTransportReleaseRemediationPlan.md). Read it with
[GWZ Connection](GwzConnectionRemediationPlan.md),
[Git boundary recovery](GwzGitBoundaryRemediationPlan.md), and
[anti-pattern repair/prevention](GwzAntiPatternRemediationPlan.md).

## 1. Why this plan exists

The transport effort accumulated useful protocol, native authentication and
correctness work, but the audited supported Python path still constructs and
destroys a transport runtime for each operation. Persistent reuse and client-side
placement promises became entangled with a larger deferred session programme.
An indefinitely renewing list of individually reasonable tasks is not convergence.

The operator requires a useful transport release and no further quiet deferrals.
This draft proposes bringing the observable reuse outcome back into the release,
restoring the library boundaries and qualifying actual consumers. Existing
approved decisions remain in force until explicit reconciliation/amendment;
this draft does not cancel them simply by calling a different scope the minimum.

Authorized now: four planning documents and their handoff. Proposed implementation
spans gwz-core, gwz-git, the selected communication package placement,
gwz-transport and necessary CLI/Python/native adapters. Upstream dependencies,
new server/carrier work and unrelated features remain reference-only. No merges,
repository creation, upgrades, publication, tags or release are authorized here.

## 2. Settled checkpoint and what has not happened

The documentation cleanup is committed. Root commit `39b342ed` records the final
archive/rule pass, following member documentation commits:

| Repository | Documentation checkpoint |
| --- | --- |
| gwz-core | `9e8edb80` |
| gwz-cli | `5193db9` |
| gwz-py | `5b6b665` |
| taut | `913e0dc` |
| taut-shape | `89916c4` |
| taut-shape-rs | `959f0f0` |
| taut-shape-py | `da79bfb` |

These are archive commits, not new transport qualification. Archived contents
were preserved and references intentionally left unchanged. Prior approvals and
unfinished obligations remain valid according to their recorded amendments.

Before these four drafts, the active set was 70 files across the workspace,
including 38 in root and 14 in core. The drafts add four root files. Two retained
root documents still exceed the new 2,500-line document cap:
`CurrentProgramCheckpoint.md` (8,388 lines at cleanup) and `GwzM5-8Refactor.md`
(3,364). This is unresolved cleanup, not an approved general exception or a
claim of compliance. The operator explicitly requested these four drafts after
archival; do not use that instruction to justify further document proliferation.

Unrelated root route-mapping and SSH prompt drafts, and private evidence-member
untracked files, were left untouched. Source code was not changed by this
remediation session. The four new drafts are not reviewed or committed merely
because the archive work is committed.

## 3. Finite acceptance contract to settle before implementation

| ID | User-visible outcome | Acceptance scenario / required observation | Current gap |
| --- | --- | --- | --- |
| R1 | Workspace network operations avoid repeated eligible handshakes | Actual CLI command across same-endpoint member repositories/phases; connection creations respect configured capacity and show reuse | Reconcile main/candidate activation and measure actual installed path |
| R2 | A long-lived Python client keeps eligible connections across calls | Two successive fetch/list operations on one client, then sustained calls; count real connection creation and compare cold/warm timing | Current supported route owns a runtime per operation |
| R3 | Concurrent operations are independent | Overlap A/B; cancel A; B completes and C subsequently reuses healthy capacity | Must prove through production API, not only host internals |
| R4 | Identity/configuration changes do not reuse incompatible resources | Explicit input changes partition/invalidate as specified; stale messages cannot affect a new binding | Reviewed ownership/partition contract needed |
| R5 | Timeouts, retries and cleanup stay bounded and truthful | Fixed deadline through auth rounds/retries, cancellation and failure; no early release of still-owned native capacity | Reconcile existing timeout-zero/live-lease/retry clauses and composed callers |
| R6 | Long-lived use is sustainable | Exceed lifetime-ID bound; saturate submissions; abandon/completely consume results; memory, tasks and native work remain bounded | Retention, admission and generation policies need repair |
| R7 | Credentials/endpoints can reside at their intended placement | Demonstrate the agreed core/client placement through current in-process CLI/Python envelopes, independent transport delivery and real Rust-side endpoint | Original requirement and later deferrals must be reconciled explicitly |
| R8 | Shipping is supportable across platforms | Native SSH/HTTPS/auth/trust plus installed CLI/Python tests on the promised platform matrix, exact artifact/source receipts | Existing evidence is not qualification of the repaired composition |

The first review must trace each row to original requirements and operator
decisions, naming the exact amendments affected. R7 cannot silently disappear
because reuse is easier to measure. Physical byte transport/iroh and cross-process
CLI persistence are separate questions; neither is required just to prove the
current in-process API. Do not claim reuse between separate CLI invocations
without a persistent service owning the resources.

Set numeric performance criteria before running comparisons: fixed representative
workload, baseline binary/source tuple, cold/warm/sustained cases, latency
distribution, connection/auth counts, concurrency and machine load. No numeric
speed claim is approved by this draft. A local pool-hit counter is insufficient;
an extra connection per repository/call can invalidate the intended benefit.

## 4. Reconcile the existing work; do not restart it

Use the retained core documents as the current entry points:

- [Requirements](../gwz-core/dev-docs/GwzRemoteTransportRequirements.md) and
  [transport design](../gwz-core/dev-docs/GwzRemoteTransportDesign.md).
- [Remaining work](../gwz-core/dev-docs/GwzV110RemainingPlan.md),
  [qualification cell table](../gwz-core/dev-docs/GwzV110S56CellTable.md), and
  [operator decisions](../gwz-core/dev-docs/GwzTransportTR18-OperatorDecisions.md).
- [Git design](../gwz-core/dev-docs/GwzGitLibraryDesign.md) and
  [API](../gwz-core/dev-docs/GwzGitLibraryApi.md).

Refresh the actual lane inventory with GWZ, then inspect source and final
reviews. Label each lane merged, ready, blocked, unfinished, superseded or
parked, with its acceptance row, exact source tuple, remaining defect and next
action. A registered lane is not automatically unmerged work. No agent or lane
should be relaunched merely because a historical handoff says “next.”

Keep existing activation, large-blob correctness, proxy/credential exposure,
packaging and selected-source questions separate. Determine which demonstrably
blocks an acceptance row; do not bundle a broader feature/dependency programme
with a necessary correction. A correctness defect in promised behavior is still
a blocker even if its review severity is P2. Historical NO-GO records that later
accepted remediation closed are not automatically current gates.

## 5. Bounded sequence with visible exit criteria

### C1 — Review one contract and adoption package

Combine the completed audit with the Connection API/placement decision and Git
consumer map. Resolve authentication/secret placement, process-wide spawn safety,
bounded resources, deadline semantics and requirement provenance. Reconcile R1–R8
and prior deferrals explicitly; no automatic adoption of the full session roadmap.

Exit: one reviewed implementation boundary and a finite blocker table. If a new
prerequisite is proposed, first show failure of the existing supported route.

### C2 — Prove the smallest useful ownership boundary

After implementation authorization, implement the standalone small proof: one
owner, two operations, real reuse eligibility, cancellation isolation, bounded
admission/results/generation turnover and truthful cleanup using deterministic
fakes and seeded message schedules. Keep Git and platform I/O out of fast tests.

Exit: the supported API, not private internals, enforces/proves the invariants.
If this requires a much larger programme, stop expansion and report the failed
assumption, cost and smallest alternative. Do not quietly move the outcome again.

### C3 — Adopt the boundary through Git and actual callers

Migrate the selected single-repository Git behavior and communication capability;
wire core fan-out, CLI lifetime and Python client lifetime to the correct owners.
Prove R1–R7 through actual entry points and current message embedding. Remove
replaced paths and add targeted boundary/adoption regression guards.

Exit: one vertical path works from client to endpoint and back with connection
counts, failure isolation and sustained behavior. A published unused library or
an internal fixture does not pass.

After C2 settles interfaces, non-overlapping Git migration and caller/native
adapter packages may run in parallel. Do not parallelize both sides of an
unsettled interface or mechanical file moves across overlapping lanes.

### C4 — Qualify the settled composition and release artifacts

Run focused platform adapter checks and affected integration tests, then the
required combined build/performance/packaging/selected-source batch. Retain the
existing mandatory Windows compile/behavior gates for changed trigger paths.
Use installed CLI and Python artifacts, not just development fixtures. Record
workers/helpers, interpreter/toolchain, source tuple and cleanup results.

Reconcile prior Windows Digest, channel binding, native identity, cancellation,
Pageant and real-account HTTPS rows against the retained cell table. Do not
restart already qualified primitives without a changed assumption, but do not
promote a primitive probe into proof of the integrated application path.

Exit: all agreed acceptance rows have matching evidence; outstanding obligations
are either closed or explicitly resolved by the operator. Only then seek final
release authorization. No publish/tag/push is implied by this draft.

## 6. Report convergence, not activity

At each complete package report:

1. Which acceptance scenario now works through which production entry point?
2. Which invariant/defect is closed, with evidence and its limits?
3. Which finite blockers remain, and did remaining work grow or shrink?
4. What was moved versus behaviorally changed, and which old path was retired?
5. Did a new requirement/prerequisite appear? Identify its provenance and obtain
   scope authorization instead of silently extending the plan.

Review complete coherent packages. Repeated rediscovery of one defect class
triggers a check of the enforcing boundary, not another caller-specific patch.
Keep the four plans as the live package; archive incorporated intermediate review
records under the agreed policy rather than recreating hundreds of active files.

## 7. Resume handoff

The user ended this session after requesting these four drafts. No implementation
agents were dispatched for them. No formal review cycle, tests, builds, dependency
changes, publication or source implementation was performed under these plans.

Resume by reading the anti-pattern plan first, then the Connection and Git plans,
and review this finite acceptance proposal against the recorded operator
decisions and current lane/source state. Resolve the two oversized-document
dispositions at a coordinated checkpoint; do not rewrite archived references.
The next authorization is for the reviewed small proof and its concrete scope,
not a blanket restart of every historical transport/session lane.
