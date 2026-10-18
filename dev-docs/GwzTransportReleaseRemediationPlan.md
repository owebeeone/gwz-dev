# GWZ transport release: architecture and delivery remediation

Date: 2026-10-19. Status: **DRAFT — operator-requested remediation process;
read-only anti-pattern audit delegated first. Implementation and formal review
have not started under this document.**

## 1. Purpose and authorization

Recover a useful transport release by repairing its library boundaries and
proving the intended reuse through real callers. Completing more local tasks,
passing more isolated reviews, or publishing an unused library does not by
itself achieve that outcome.

The operator requested four components:

1. Restore the `gwz-git` boundary so Git implementation and dependency changes
   are absorbed there rather than repeatedly reaching workspace coordination.
2. Establish a small communication/reuse library between Git and `gwz-transport`,
   independently testable across platforms without the rest of GWZ.
3. Reconcile the release commitments and restore enough of the original benefits
   to make this release worthwhile. Do not silently move further work to 1.2.0.
4. Audit the affected code for anti-patterns, including globals, oversized
   contexts, ownership problems, concurrency hazards and oversized files.

Repair and prevention are equally required outcomes. Closing the current findings
without constraining how the supported APIs are subsequently used is incomplete.

**Current authorized work:** this process/plan document and the read-only
anti-pattern audit, which the operator directed to precede boundary redesign.
This does not
authorize source edits, a new repository, dependency upgrades, lane merges,
publication or wholesale adoption of an older session programme. The later
implementation boundary must be recorded and approved before implementation.

This document is in the workspace's `dev-docs/` because the remediation spans
several members. It does not silently supersede accepted requirements or designs;
the reconciliation and reviewed amendments below must name affected clauses.

## 2. Evidence baseline and limits

Read-only observations were made on these local heads:

| Repository | Commit |
| --- | --- |
| root | `f66d6984efdc8aeaab5b051c0a297647e6f1350a` |
| gwz-core | `35df1cbb9dc5329eaf3953976e360ad7cdf041c3` |
| gwz-py | `271d73c3dc1888388b0e1f3ad3c4e29ceb9521ec` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| gwz-transport | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |

These heads are not a declaration that the workspace is clean or settled.
Concurrent work exists; the root already had staged configuration changes and
untracked documents when this file was created. They are outside this edit.
Refresh the complete tuple and lane inventory at the first milestone.

Relevant observations, rather than a complete new code audit:

- The [Git library design](../gwz-core/dev-docs/GwzGitLibraryDesign.md) and
  [no-fallback plan](../gwz-core/dev-docs/GwzNoFallbackPlan.md) explicitly describe
  an enduring single-repository Git boundary. The library's current public
  surface is read-only groundwork, including opening a repository and reading
  owned commit records; that alone does not establish the intended boundary.
- The [accepted reuse design](GwzConnectionReuseDesign.md) already separates
  shared endpoint instances from per-operation bindings. It describes why
  retaining the current runtime unchanged is insufficient. Its later amendments
  and the actual implementation must be reconciled, not bypassed.
- `gwz-py/native/src/route/transport.rs` currently documents and implements a
  per-operation transport route. Sharing a native host/gate is not evidence of
  sharing its transport pool across operations.
- Existing runtime reuse tests and the partial `gwz-session-host` crate are
  recovery inputs. Their existence does not prove real-client persistent reuse
  or require adopting the whole application session/server design.
- A preliminary scan found nine tracked handwritten source/schema/test/tool
  files over 1,500 lines in core and CLI, excluding recognizable generated files.
  This was an inventory, not an assessment of every file or an automatic split
  instruction. Generated files need separate classification.

No builds, performance experiments or formal peer reviews were run to establish
this document. Existing campaign results must retain their exact source tuple,
scope and limitations when cited; an old result is not qualification of a new
composition.

## 3. Release outcome first

Before agreeing a migration, write a finite acceptance table for the release.
For every original or subsequently approved commitment, record its requirement
source, present behavior, executable acceptance scenario, evidence and remaining
blocker. Distinguish approved requirements from implementation preferences.

The proposed minimum value to substantiate is:

- A workspace network operation reuses eligible connections across repositories
  and phases, with the agreed concurrency and per-host limits.
- Successive network operations on one long-lived Python client reuse eligible
  connections rather than rebuilding a pool for each operation.
- Overlapping operations are independent: cancelling or finishing one does not
  destroy another operation's healthy work or the shared owner's eligible idle
  connections. Physical connection failure has an explicit, tested fault scope.
- Deadlines, backpressure, bounded resources and truthful cleanup remain valid
  through the actual library and client entry points.
- Connection counts and elapsed time demonstrate the benefit under a controlled,
  repeatable workload. Set numerical acceptance criteria before the comparison;
  define cold, warm and sustained cases rather than selecting a favorable result.

The table must also explicitly reconcile original endpoint/credential placement
promises and prior deferrals. They cannot disappear behind the reuse repair.
Internal message embedding and independent stream delivery must be distinguished
from a future physical wire carrier; a new physical carrier must not become a
prerequisite without demonstrated necessity and operator authorization.

Do not imply reuse across separate CLI processes without an explicitly owned
long-lived service. Within-command CLI pooling and cross-operation Python reuse
are different acceptance scenarios.

The operator's no-further-deferral direction is a constraint. If an agreed outcome
cannot be delivered with the proposed boundary, expose the failed assumption,
cost and alternatives. Do not resolve it by silently moving it to another release
or weakening acceptance. This plan does not reverse earlier decisions by fiat.

## 4. Boundary repair and audit

### 4.1 Git library

Map the relevant single-repository Git behavior currently in core to its
intended owner. Identify concrete migrations, including native binding adapters
and applicable commit/tag/filter behavior, rather than assuming every use of a
Git type must move in one pass.

The target is a supported `gwz-git` API that core actually consumes. Native types,
error translation, necessary binding adaptations and dependency-specific behavior
stay behind the agreed boundary. Workspace fan-out, member selection, workspace
locks, markers and application results remain core responsibilities.

An upstream fix may replace a library implementation; it does not remove the
library's purpose or require returning its callers to core. Keep C/binding patches
minimal. Test the existing supported dependency route before proposing an upgrade
as necessary. Publishing scaffolding alone is not justification for upgrading.

### 4.2 Communication and reuse library

Design a narrow supported boundary with explicit long-lived ownership and
independent operation handles. It must make correct ownership the ordinary API
path rather than require each caller to attach matching contexts and gates.

Specify who owns pools, connection eligibility, configuration partitions,
operation registration, admission, cancellation, failure isolation and shutdown.
Decide each responsibility once; do not introduce duplicate owners in adjacent
layers. An explicit, narrowly scoped context is appropriate. A service-locator
context that exposes unrelated resources or permits accidental duplicate pools
is not an adequate repair.

Evaluate the existing endpoint/binding design, transport machinery and extracted
host primitives before selecting repository placement or a package name. The
operator's requested library sits between Git and transport; its exact dependency
graph is a design deliverable. Do not equate it with the whole application session
host, and do not require server dispatch, a new wire format or a new repository
merely to obtain in-process reuse.

The operator further specified a Git-independent communication library, consumed
by the Git interface, with SSH and HTTP/HTTPS coverage and independent testing.
Its requirements must explicitly allocate authentication, key/certificate
management and credential-source selection. Evaluate whether those need a separate
authentication boundary; do not assume all policy or secrets belong inside the
communication library. The design must recommend repository placement, with
dependency, test and release consequences, before any repository is created.

Use explicit ports for platform and credential policy. Preserve applicable secret
ownership and taut protocol boundaries, or document precisely the amendment a
proposed move needs. The smaller library must be independently testable, not a
wrapper that imports core back into its tests.

### 4.3 Anti-pattern audit

Audit the real acceptance paths first, expanding only when a finding demonstrates
a dependency. For each finding record location/evidence, requirement provenance,
classification, practical impact, proposed repair, owner and whether it blocks
the release outcome. Separate confirmed defects, risks and optional improvements.

Inspect:

- Mutable globals and thread-locals; unrecorded exceptions imposed by dependencies.
- Oversized contexts, hidden dependencies and constructors that accidentally
  allocate independent pools, gates or supervisors instead of sharing the owner.
- Pool/operation lifetime coupling; unclear drop responsibilities; inaccurate
  cleanup reports; detached tasks, hanging reads and unbounded retained state.
- Locks across blocking calls or awaits; inconsistent lock order; unbounded
  queues; blocking work on async executors; cancellation that misses a worker.
- Duplicated production/candidate paths, bypasses of the intended supported API,
  and invariants maintained manually at several callers.
- Deadline resets, nested retries, stale configuration/identity assumptions,
  credential-bearing diagnostics and error translation that loses failure scope.
- Tests that mirror internals but never prove composition, reuse or failure
  recovery through the public API.
- Oversized or mixed-responsibility files, generated-code provenance, and explicit
  control-flow/conditional-compilation boundaries on disabled platforms too.

Use the existing [agent process rulebook](AgentProcessRules.md), L1-23, the
[split-files skill](</Users/owebeeone/.codex/skills/split-files/SKILL.md>) and
installed `rust-split`. The operator clarified that the approximately
1,000-line trigger is discretionary and splits should occur between stable steps,
coordinated with affected lanes. Do not add a new hard limit or duplicate checker.
For each material split candidate, select a checkpoint and cohesive boundaries;
keep mechanical moves separate from behavior changes and preserve attributes.

### 4.4 Prevent recurrence, not only repair current callers

For each accepted audit finding, specify both its repair and the structural
constraint or regression evidence that prevents the same mistake returning.
Distinguish properties the API makes impossible from those tests detect and
those that remain caller obligations. Do not claim a test proves an invariant
outside its scope or that a written rule alone enforces ownership.

- **Required ownership at construction.** A supported pooled Git/communication
  entry takes its intended owner explicitly. Avoid successful construction with
  unrelated default state that a later builder call must repair. Any native or
  alternate route is explicit, rather than a silent consequence of missing context.
- **Separate resource scopes.** Express process, host/client, operation, connection
  and stream lifetimes independently. Operation finish/cancel must not own shared
  host shutdown. Process-wide native hazards need a process-valid mechanism;
  merely passing one gate consistently within each client does not prove safety.
- **Finite lifecycle policies.** Declare admission, waiting work, completed-result
  retention, identifier/generation turnover and shutdown bounds. Test sustained
  use beyond a single generation and beyond normal concurrency, not just a happy
  pair of operations. Do not remove stale-message protection to obtain reuse.
- **One operation input contract.** Specify the immutable configuration and
  authentication inputs used by subordinate routes. No adapter should quietly
  recapture ambient state at a later boundary without an explicit contract.
- **Independent contract tests.** Exercise the supported library API with fake
  connectors, clocks and credential sources, including cancellation, fault scope,
  reuse eligibility and cleanup. Native adapters get their own focused platform
  tests. Turn confirmed audit counterexamples into lasting regression cases.
- **Real-consumer proofs.** CLI/Python acceptance must traverse the same supported
  API that production uses. An internal test retaining an owner cannot prove
  caller-level reuse if the actual caller constructs and destroys another owner.
- **Dependency boundaries that remain enforced.** Use existing architecture and
  source guards where applicable, and public API visibility/dependency direction
  to keep Git internals and communication ownership out of workspace coordination.
  Amend existing guards only where a demonstrated gap warrants it; do not create
  a parallel rulebook or checker suite as a substitute for API design.
- **Review recurrence and convergence.** Reviews assess actual consumers and
  ownership across calls, not just local implementations. When the same class of
  defect recurs, explain which prior invariant was unenforced and repair that
  boundary rather than only adding another caller-specific patch. Report release
  acceptance progress and outstanding cost at the existing milestone checkpoints.

The boundary design must include an invariant table: requirement, resource owner,
API enforcement, independent regression test, real-consumer evidence, and any
remaining caller obligation. This is a concrete review object, not a promise that
all future defects can be eliminated.

## 5. Milestones and decision gates

Steps are single goals; aim for less than 500 lines of implementation per step
where practical. This is a budgeting aid, not a reason to fragment a coherent
review or impose another hard size rule. Record estimated moved code separately
from new behavior. Group completed steps into meaningful review packages.

### M1 — Anti-pattern audit first, then release reconciliation

1. Capture the source tuple and conduct the read-only anti-pattern audit of
   critical caller paths, using the operator's four components and existing
   requirements as scope. Produce the evidence-backed findings and ownership map
   before choosing the replacement library boundaries or beginning migration.
2. Reconcile active lanes: merged, ready, unfinished, superseded or parked.
   Associate each with a release requirement and the relevant audit findings.
3. Produce the finite release acceptance table and original-promise reconciliation,
   informed by the audit. Neither this table nor a new design is a prerequisite
   for starting the already-authorized audit.

**Exit:** evidence-backed inventory, explicit release outcomes and a finite set of
boundary defects. No new implementation or dependency work is hidden in the audit.

### M2 — Reviewed boundary design and migration contract

1. Specify the Git API responsibilities and the core callers that will migrate.
2. Specify the communication/reuse API, resource owners, dependency graph,
   configuration/identity rules and failure semantics; compare existing designs.
3. Define the early proof, acceptance criteria, migration sequence, affected
   requirements, recurrence-prevention invariants and rollback approach. Run a
   bounded design review for technical consistency, lifecycle safety, requirement
   provenance, prevention at actual consumer boundaries and authorized scope.

**Exit:** a reviewed, concrete repair, not an uncosted new session programme.
Repository creation and source changes remain subject to implementation approval.

### M3 — Independently testable reuse proof

1. Build the smallest library/API slice using existing machinery where suitable.
2. Prove one long-lived owner and two successive operations reuse one eligible
   connection with independent operation registrations.
3. Prove overlap/cancellation isolation, configuration or identity incompatibility,
   stale-connection recovery, registration-boundary behavior and bounded shutdown.
4. Review the complete slice and report measured results and remaining obstacles.

Fast tests use deterministic fakes and in-memory discrete messages. Add seeded
schedule/chunk/read-size tests where they exercise lifecycle and backpressure;
record seeds and traces for exact reproduction. Native platform behavior belongs
in separate focused library/adaptor tests. Do not require the full GWZ application
suite to establish each library invariant.

**Exit:** a public-API proof on the intended library boundary. Keeping a runtime
alive or observing an internal pool hit alone does not pass. If the proof fails,
report the failed assumption before expanding the architecture or migration.

### M4 — Real consumers and recovered Git boundary

1. Migrate the agreed single-repository behavior into `gwz-git` and make core use
   its API, preserving characterization tests and externally required behavior.
2. Wire the shared communication/reuse owner through core and the Python client;
   retain independent application-operation and transport-stream delivery.
3. Prove real CLI workspace pooling and successive/overlapping Python operations,
   including cancellation, errors, configuration changes and client shutdown.
4. Retire replaced paths only after equivalent behavior is demonstrated; perform
   scheduled mechanical splits at settled boundaries and review the combined slice.

After M3 settles the shared interfaces, Git migration and caller integration may
use separate lanes with explicit file ownership. Do not parallelize moving the
same files or changing both sides of an unsettled interface. Coordinate host tests.

**Exit:** core consumes the restored libraries and the actual client acceptance
scenarios pass. An unused package or test-only route cannot satisfy this milestone.

### M5 — Integrated release qualification

1. Run the focused cross-platform library/native matrix and the affected consumer
   integration tests, then the required combined build/behavior/release checks.
2. Measure connection counts, cold/warm elapsed time and sustained resource use
   through shipped client entry points against the declared baseline and criteria.
3. Review the final composition, user-facing behavior and remaining audit findings;
   reconcile activation, packaging and publication prerequisites on the same tuple.

**Exit:** every accepted release scenario has current evidence; promised behavior
has no unresolved correctness blocker. Publication is a separate authorized action.

## 6. Convergence, evidence and scope control

At each milestone report: which user-visible acceptance rows now pass, which
blockers remain, what was removed from the remaining work, and any change in cost
or scope. A new blocker needs evidence and an explanation of its effect on the
fixed finish line. Do not replace the remaining list with another rolling programme.

Reuse existing tests and evidence where their scope and source remain applicable.
Run targeted tests for local changes, integration tests for changed boundaries and
the full required qualification at the settled integration/release checkpoint.
This reduces repeated broad campaigns without dropping applicable repository gates.
Do not rerun unaffected native matrices merely because prose or a pure library
changed; equally, portable tests cannot prove native authentication or cleanup.

Read `EVIDENCE.md` before creating or relocating campaign artifacts. Keep raw
campaign evidence and private runners in the designated private evidence member,
build outputs outside it, and public library tests independent of private material.

Likely implementation components are `gwz-git`, core's Git/endpoint/transport host
adapters, the selected reuse library, and CLI/Python composition. Confirm exact
editable paths at M2. `git2-rs`, libgit2, taut and platform libraries are reference
dependencies unless a demonstrated blocker and separate authorization expands
scope. A design preference, audit finding or review suggestion is not permission
for an upgrade, a new framework or unrelated cleanup.

Creating this plan launched no implementation, merges, pushes, tags or releases.
The operator subsequently requested a separate read-only anti-pattern audit;
that audit is delegated and comes first. Next: use its findings to reconcile
release acceptance and ownership, followed by the bounded boundary design and
its review. Implementation remains separately authorized work.
