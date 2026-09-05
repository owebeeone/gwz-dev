# GWZ local clone — library boundaries and testing

Status: **PROPOSED 2026-09-05 revision 1**. Local adoption decision for
the local-clone plan, following F51 and the second GPT-6/Sol reviews.
The packages, commands and gates below are planned, not implemented or
measured. The compiled interface checkpoint is an LCM1.0 deliverable.

## 1. Scoped policy adoption

Adopt LBT-001 through LBT-012 from the canonical
[Library Boundary and Testing Policy](../../glade-wz/dev-docs/LibraryBoundaryAndTestingPolicy.md)
for **new local-clone libraries and their new public boundaries**. Read
that policy before implementing these packages. Reviewed policy snapshot:
2026-09-05, SHA-256
`dcc4fbd2b45caf928978a090208951ef14759ef0589e820b94f8475fccf07c10`.
Keep the shared rules there; this document supplies GWZ's scope, package
choices, owners, test budgets and enforcement plan. A policy change needs
an explicit local review before changing this adoption baseline.

Existing core, Git backend, merge lifecycle, CLI and Python packages are
outside this package-classification rollout. Their existing rules still
apply. New adapters in core are in scope for narrow-port/API review and
focused integration tests, but do not claim an independently compiled core
test target. This is a named composition boundary, not permission to move
new library logic into core. No existing-library migration is authorized.

The goal is parallel implementation with a seconds-scale local test loop.
Each functional library below is a separate private path crate. A filtered
test of the existing `gwz-core` crate does not satisfy that isolation.
Only declared dependencies belong in a library's build closure; unrelated
unfinished implementations must not be required to compile it.

## 2. Packages, roles and permitted production dependencies

All package directories are under `gwz-core/crates/`. Cargo names use the
`gwz-` prefix shown below. These are internal packages in the existing
repository, not new GWZ member repositories or a public plugin ABI.
Use explicit package edition/version metadata so a standalone core checkout
does not depend on the outer workspace's package inheritance.

In the dependency column, names omit `gwz-`. These are the complete allowed
**first-party production** edges. Third-party, dev, build, optional and
target-specific dependencies also need the gate in §6; this table is not
permission to add arbitrary libraries.

| Directory / Cargo package | Role and rationale | Owner lane | Allowed first-party dependencies |
|---|---|---|---|
| `copy-contract` / `gwz-copy-contract` | Contract: tree-copy request, result, cancellation and `TreeCopier`; no platform implementation | C, interface owner | none |
| `refcopy` / `gwz-refcopy` | Implementation: native/ordinary `SystemTreeCopier` implements `TreeCopier` | R | copy-contract |
| `repo-contract` / `gwz-repo-contract` | Contract: repository observations, typed OIDs, `RepoInspector` and bounded `ObjectReader`; no Git implementation | C, with I review | none |
| `repo-inspect` / `gwz-repo-inspect` | Implementation: local Git/filesystem reads implement the repository contracts | I | repo-contract |
| `work-detector` / `gwz-work-detector` | Pure: classify supplied observations and decoded GWZ evidence; no I/O | W | repo-contract |
| `history-check` / `gwz-history-check` | Integration: bounded traversal through an injected read-only `ObjectReader`; no deletion or fetch | H | repo-contract |
| `family-model` / `gwz-family-model` | Pure: family data, row transitions, names, remote-token resolution; deterministic values | F | none |
| `family-store-contract` / `gwz-family-store-contract` | Contract: read view, lock session and family metadata mutations; depends on the small pure family model | C, with S review | family-model |
| `family-store` / `gwz-family-store` | Implementation: YAML, ordinary publication and OS advisory locking implement the store contract | S | family-store-contract, family-model |
| `local-import` / `gwz-local-import` | Integration: pairing, capture, retained imports and push through its own narrow transport port | X | repo-contract, family-model |
| `workspace-install` / `gwz-workspace-install` | Integration: copy/construction/install ordering via injected ports | N | copy-contract, repo-contract, family-model, family-store-contract |
| `repo-factory` / `gwz-repo-factory` | Integration: bare/clean construction through its own repository-builder port | B | repo-contract |
| `local-disposal` / `gwz-local-disposal` | Integration: compose fresh safety observations and one-shot removal through ports | D | repo-contract, family-model, family-store-contract, work-detector |
| `local-testrepo` / `gwz-local-testrepo` | Harness/tool: tiny real Git/temporary-tree fixtures; dev-only, no application dependency | T | repo-contract |

The three contract packages create useful substitution/build boundaries;
there is no global common package. Pure functions need no marker traits.
Integration packages own interfaces for the external actions they consume.
Core implements those ports by translating to concrete libraries or existing
GWZ helpers. For example, installation consumes a construction port; its
core adapter calls `repo-factory`, so installation tests do not compile an
unfinished factory. Disposal similarly consumes a history-check port rather
than depending on the concrete history verifier.

No new crate may depend on `gwz-core`, a driver, taut-generated messages,
checked-artifact code, or the full-system test harness, even as a dev
dependency. Public contracts use owned domain values, standard paths and
typed errors; they do not expose `git2`, OS handles, core model errors or
protocol request types. The boundary adapter performs those translations.

`local-testrepo` uses Git library calls, fixed identities/timestamps and
small files. Only real-I/O adapter tests use it. Pure-library fast tests
use plain values and in-memory graphs, not this I/O harness. Small shared
conformance helpers live behind an explicit `contract-tests` feature of
their owning contract crate, enabled only by dev dependencies; those
helpers add no concrete adapter or application dependencies.

## 3. Interface checkpoint

These are the required boundary operations and type owners, not a claim
that Rust declarations already exist. Lane C lands the actual compiling
public signatures, complete structs/enums and contract tests before the
consumer lanes branch. No lane invents a competing result/evidence type.
An unresolved public type or a required port with only a placeholder
success implementation fails the checkpoint.

| Owner package | Public operations / owned values | Required behavior |
|---|---|---|
| copy-contract | `TreeCopier::copy_tree(&CopyRequest, &dyn Cancellation) -> Result<CopyReport, CopyError>`; request owns paths, exclusions and Auto/OrdinaryOnly mode; report owns native/ordinary counts, logical bytes and warnings; error owns failed path, category and partial report | New/admitted-empty destination, never hardlink source; exclusions before copy; cancellation/errors retain partial destination; unsupported-native fallback is classified; success says nothing about family readiness or crash durability |
| repo-contract | `RepoInspector::{inspect_layout, observe_work, inventory_history}`; `ObjectReader::{retained_roots, read_object}`; owns `RepositoryInfo`, `RepoKey`, `ObjectId`, `WorkObservation`, `ProtectedRoots`, `Observation<T>`, `ReadLimits`, object data/edges and typed read errors | `ObjectId` includes format; byte/path names need not be UTF-8; observations distinguish known/unknown, conflicts, ignored data and suppression flags; bounded reads cannot fetch, rewrite index or run maintenance |
| work-detector | `classify_work(&WorkObservation, &GwzEvidence) -> WorkReport`; owns decoded `GwzEvidence`, hazard entries and clean/dirty/unknown report | Core decodes evidence, including malformed/unsupported evidence as unknown; no core/protocol types; truncated diagnostics never erase a hazard |
| history-check | `check_history(&ProtectedRoots, &[Witness], &dyn ObjectReader, Limits, &dyn Cancellation) -> HistoryOutcome`; owns witness descriptions, coverage, limits, its cancellation port and Verified/Unpreserved/Unknown outcomes | Bound roots and owned bookkeeping; retained complete reachable graphs required; no persisted proof; read failure/cancellation/limit is unknown, never verified |
| family-model | `resolve_remote_token(&FamilyView, &RemoteToken, Verb) -> Resolution`; `validate_transition(&FamilyView, &FamilyChange) -> Result<ValidatedChange, Refusal>`; owns names, ids, rows, index version, state observations and transition values | Pure decisions, explicit non-ready states, no repair or filesystem effects; see resolution table below |
| family-store-contract | `FamilyStore::read_view(&FamilyLocation) -> Result<FamilyView, StoreError>`; `FamilyStore::try_lock(&FamilyLocation) -> Result<Session, StoreError>`; `FamilySession::{reread, apply}`; owns location, session contract, store errors and partial metadata effects | Read cannot create a lock file; session owns one OS lock until drop; apply rereads/validates and handles only matching index/pointer/allocation-marker changes; all mutation requires that live session; no multi-file atomicity promise |
| local-import | `prepare_import(&ImportRequest, &mut dyn LocalTransport, &dyn Cancellation) -> Result<ImportedSource, ImportError>`; `push_local(&PushPlan, &mut dyn LocalTransport) -> PushReport`; owns pairing/vector, transfer id/ref, per-repo effects and `LocalTransport` | Port requires resolve-source, collision-check, anonymous explicit fetch/push and received-OID reads; all pairing/source validation before first transfer; complete vector before engine entry; partial imports are retained and reported |
| workspace-install | `install(&InstallRequest, &mut dyn FamilySession, &dyn TreeCopier, &mut dyn InstallPorts) -> Result<InstallReport, InstallError>`; owns install request/result/partial effects and `InstallPorts` | Narrow required ports for source inspection/recheck, repository construction and GWZ configuration installation; use store session for every family write; final manifest then ready; errors leave incomplete state |
| repo-factory | `construct(&FactoryRequest, &mut dyn RepoBuildPort) -> Result<FactoryReport, FactoryError>`; owns captured-vector/mode/branch request and builder port | Build independent local repositories, no borrowed object store; check every selected branch/ref before allocation; no implicit network, hidden commit or family metadata write |
| local-disposal | `dispose(&DisposeRequest, &mut dyn FamilySession, &mut dyn DisposalPorts) -> Result<DisposeReport, DisposeError>`; owns policy inputs, named waivers, removal report and ports for fresh evidence/history/removal | Fresh intact-ready checks, clean/preserved default, disposing write before one-shot removal; error/cancellation reports remaining or unobservable paths; no rollback/replay; keep changes only matching family metadata |

Every service error identifies its operation, typed cause and known partial
effects; uncertain effects stay uncertain. Cancellation is checked between
bounded work units and does not promise preemption of a blocking OS call.
Requests are invocation-local values, not reusable delete authorizations.
The real declarations must specify ownership/lifetimes, per-call resource
bounds, allowed call order and error mapping. Ports have meaningful required
methods without permissive default bodies. Integration packages' small
contract suites exercise a fake and the real core adapter on the applicable
invariants; fake-only tests do not establish adapter correctness.

### One resolver, with verb-specific results

Resolve only from a valid observed family view; an unreadable/malformed
view refuses rather than pretending no family exists. Family bindings are
revalidated under the family lock before effects. Ordinary remote fallback
does not create a family lock file.

| Family observation | Merge family selector | Pull/push remote token |
|---|---|---|
| Ready row | Bound member, including explicitly selected root | Bound member |
| Creating/disposing/missing or invalid member | `UnknownLocal` with state detail, as design §6 specifies | Lifecycle refusal; never fall through |
| No row for token | `UnknownLocal` | GitRemote candidate; existing remote lookup yields the remote or `missing_remote` |
| No token | Ordinary Git-ref merge path | Existing defaults and request-over-policy precedence |

Git-remote existence is resolved by the existing per-repository remote
lookup, not guessed by this pure function. One test table covers both
verbs, all states and origin fallback. Core wrappers use this function for
merge, push and pull; drivers only encode/present requests.

### Writes, locks and transport

`family-store` is the sole writer of index, pointers and allocation
markers. Installation, keep, disband and disposal call its session port.
The installer owns destination configuration ordering, not a second family
writer. Its configuration adapter continues using the sanctioned existing
GWZ installation helpers; no manual `gwz.conf/` edits are introduced.

The store owns a small OS advisory try-lock implementation with handle
lifetime release (`flock` on supported Unix hosts, `LockFileEx` on Windows)
and ordinary temporary-write/rename with checked available flushes. Do not
reach into private checked-artifact locks or the single-caller pinned
`verified_write` helper. No create-file-as-lock/stale-lock protocol, line
count promise, or new durability framework. Select and test the platform
wrapper in LCM1.0; unsupported locking refuses family mutations.

The existing Git backend currently resolves named remotes. Anonymous local
fetch/push is **new adapter work**. Lane C owns the additions to the backend
contract, `transport.rs`, its pinned delegate file and tracking test double,
for both fetch and push together. Lane X calls only its narrow port. The
adapter takes admitted local repository paths and explicit refspecs, creates
an anonymous remote, uses no credentials/network helpers, and preserves
existing push branch protections. No named remote is persisted.

Core validates request shape and rejects unsupported family dry-run before
creating a family lock file or importing refs. After successful import it
calls the existing public merge entry with the common local source ref and
family selector cleared. Hold only the family lock. Library tests exercise
this boundary through ports; real merge start/continue/abort belongs in
focused integration tests. Pull needs its own source-preparation adapter;
changing the existing remote-name string to an arbitrary local ref is not
an implementation.

## 4. File ownership and landing discipline

Each package lane owns only its directory from §2, including its manifest
and tests. Lane C owns the three contract directories and their shared
type changes; F owns the pure family-model API after the initial checkpoint.
Contract changes require affected lane review and tests, not concurrent
uncoordinated edits. Concrete owners propose changes to C/F.

| Lane | Additional exclusive files / integration responsibility |
|---|---|
| C — core integration | Outer/core Cargo manifests and lock resolution; core protocol and generated bindings; core requirements/design; `src/lib.rs`, `src/workspace_ops/mod.rs`, `src/operation/push_event.rs`; backend contract, transport/delegates/tracking double; existing protected-boundary pin updates; new local boundary gate, CI/build targets; `src/local_clone/` adapters and `src/workspace_ops/handle_local.rs` |
| T — fixtures | `crates/local-testrepo/` and its documented tiny fixture API; reusable fixed identity/time/object-format choices |
| R/I/W/H/F/S/X/N/B/D | Corresponding package row only; core adapter requests go to C, never direct edits to a sibling or pinned compiler root |
| CR — Rust driver | Local command parsing/presentation/tests in gwz-cli; C supplies protocol and core entry points |
| CP — Python driver | New `cli_local_family.py` and focused parsing/presentation tests; preserve the existing unrelated `cli_local.py`; C supplies regenerated protocol/native dispatch integration |

C lands one compiling integration skeleton after the contracts: crate
registration, plain module declarations, new dispatch slots, both transport
ports/adapters and test-double support, and generated protocol. Unimplemented
feature slots refuse as unsupported; they must not return success. Re-pin
only changed protected files, with reviewed reasons and per-commit checks.
Later deliberate changes to a pinned seam still go through C and the same
checks; “one initial re-pin” is not a claim that all future pin changes are
forbidden. A family wrapper need not modify `merge/mod.rs`.

At the skeleton exit, every selected package command works from both the
outer GWZ checkout and a standalone core checkout. Test an unrelated
implementation compile error in a disposable fixture: the target's build
must still pass if that sibling is outside its declared closure. Invalid
manifests can still break Cargo workspace discovery; isolation is not a
promise to tolerate malformed workspace configuration.

## 5. Fast feedback and integration tiers

Every package in §2 has the concrete fast command
`cargo test -p <Cargo-package> --lib` from a checkout containing it. For
example, `cargo test -p gwz-history-check --lib` compiles the repository
contract and its own algorithm/tests, not `gwz-repo-inspect` or core.
All unit and applicable contract tests belong in that target. These
commands become runnable only when the LCM1.0 skeleton lands.

| Tier | Contents and command | Project target, not measured performance |
|---|---|---|
| A — each library | Its exact `cargo test -p … --lib`; deterministic values, tiny temp files/real Git repos where applicable; fake ports for orchestration; no process spawn, service, sleeps or full workspace | Test execution ≤ 2 s; warm incremental compile plus tests ≤ 10 s **per package** on the recorded development host |
| B — adapter slice | `cargo test -p gwz-core --lib local_clone::tests::<slice>`; real copy/Git/engine/store adapters, subprocess lock smoke and selected native-copy execution | Test execution ≤ 10 s and warm command ≤ 30 s per slice; split a growing slice; cold core build recorded separately |
| C — milestone/system | Existing protocol drift, Rust/Python parity, merge regressions and native platform jobs; `gwz-py/run_tests.py` remains broad acceptance | Separate CI/milestone budget, recorded at LCM1.0; never required on every library edit |

No real sleeps for contention tests: use barriers or explicit scheduling.
Native-copy capability/large-tree performance cases use explicit Tier B/C
selection, report unavailable native support as a skip, and must assert the
native path actually executed before claiming native acceptance. Forced
ordinary-copy behavior and fault/fallback classification stay in Tier A.

Record host/OS/filesystem, toolchain, package and code SHA, command, test
count, execution time, warm edit/build/test wall time and cold build time
separately at LCM1.0 and each milestone. Record actual edits used for warm
measurements. Targets above are not claims about existing core tests or
fresh dependency compilation. Budget overruns require a smaller fixture,
dependency correction, or a reviewed exact exception with owner and
retirement condition; do not relabel an exhaustive suite “fast.”

Public-contract changes run their conformance suites, real implementations
and affected consumer packages. Internal changes start with the owning
package; changed adapters also run their relevant Tier B slice. System
assurance remains at milestone/release boundaries. Each new behavior starts
with a failing test, including failure/edge cases; the skeleton compiling
alone is not evidence that its implementations satisfy the contracts.

## 6. Architecture gate and evidence

LCM1.0 supplies `gwz-core/scripts/checks/check_local_clone_boundaries.py`
and a machine-readable inventory matching §2. The proposed command is
`python3 scripts/checks/check_local_clone_boundaries.py` from core, invoked
as a failing CI job. It is development tooling, never a runtime dependency.
No such gate exists yet, and a workflow does not configure hosting branch
protection by itself.

The gate inventories every library manifest under `crates/`, rejects new
unclassified packages, and checks resolved package identities and declared
edges through Cargo metadata. Include normal, build, optional, target and
dev dependencies, including renamed dependencies. Inspect declared edges
that inactive features omit from a selected resolved graph. Reject core,
driver or system-harness imports into library test closures. Negative
fixtures cover an unclassified crate, reversed contract edge, aliased
forbidden dependency, and optional/target/dev dependency violations.

Use compiler witnesses and shared behavioral suites for required trait
implementations; review semantic relevance, error completeness and public
type leakage. If a source syntax check is added, parse Rust and report its
limits; string matching for `trait` is not proof. This bounded gate is not
a transitive third-party audit or a macro/semantic API verifier. Future
exceptions need exact package/boundary, reason, review owner and retirement
condition (LBT-012); there are no unnamed future exceptions.

The checkpoint is ready when the ownership/dependency inventory, compiled
interfaces, contract/fake tests, skeleton, real copy/import spikes and
measured fast commands agree, and an independent reviewer has examined the
exact built commit and recorded GO for parallel development. The reviewer
must not be the skeleton implementer. Record findings, fixes and verified
closure of blocking issues; see plan LCM1.0d. Until then: **GO for LCM1.0;
parallel feature lanes wait for that reviewed checkpoint**. Documentation
adoption does not close
implementation evidence requirements from any review.
