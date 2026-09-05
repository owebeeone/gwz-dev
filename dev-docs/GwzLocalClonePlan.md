# GWZ local clone — phased delivery plan

Status: **PROPOSED 2026-09-05 revision 4**. Retains best-effort scope
and adds explicit library boundaries, parallel ownership, test budgets and
review remediation. Product design revision 8; architecture revision 3;
[local library policy adoption](GwzLocalCloneLibraryBoundaries.md) revision 1.
No feature code, compiled checkpoint or implementation acceptance claimed.
Earlier independent reviews remain attached to their recorded inputs.

Keep **LCM1 = MVP**, **LCM2 = delete + bare**, **LCM3 = the rest**.
“The rest” remains the working interpretation of “the test”; tests
accompany each delivered behavior.

## 1. Scope and engineering budget

Deliver independent named local repositories, useful family exchange,
and conservative disposal with ordinary filesystem/Git operations.
Native copy-on-write is an optimization over a correct ordinary copy.

**Do not build for this feature:** a new filesystem identity framework,
checked-artifact catalog, persistent proof store, replayable deletion
journal, merge-record version, global mutator-admission migration, or
merge lock-handoff redesign. Existing GWZ merge behavior stays intact.
If an integration needs those changes, simplify that integration before
expanding the project.

Accepted tradeoffs:

- The operator quiesces involved endpoints and deletion witnesses,
  including unrelated GWZ commands. Other uninvolved lanes may work.
- One advisory family lock serializes local-family mutations/exchanges;
  ordinary commands retain their current locks.
- Interrupted copies/deletions can leave partial directories requiring
  inspection and manual cleanup. Reads never resume mutation.
- Imported commits remain under ordinary Git refs. No automatic pruning
  in this version; retained objects cost disk space.
- Ordinary temp-write/rename and available flush helpers are sufficient;
  no new cross-platform power-loss or hostile-path-replacement guarantee.

Required behavior remains: errors are reported, copies are independent,
source data is not intentionally changed, nested Git escape paths refuse,
and ordinary disposal refuses dirty/unknown/unpreserved work. Existing
named force waivers apply only to an intact ready tree; they do not make
partial deletion replayable. The user-selected preservation default stays.

### MVP workflow

| Capability | LCM1 scope |
|---|---|
| Create | `gwz clone --local --name A [dest]`, verbatim source copy with exclusions and fresh destination metadata |
| Family | Root-owned names/index, clone-from-clone in the same family, one advisory family lock |
| Inspect | Observation-only `gwz local list`, including incomplete/missing/error states |
| Work | Existing GWZ operations in a completed independent lane |
| Integrate | `gwz merge --remote A [ref]` imports commits, then calls the existing engine using a local Git ref |
| Retire | `dispose A --keep` and pointer-only disband; directory contents retained |
| Clients | Core, Rust CLI and Python agree on supported commands/errors/output |

Unsupported modes fail before mutation. LCM1 does not delete directories,
including failed copies. Later milestones also perform no automatic
incomplete-directory cleanup. Public clean/branch/source selection,
bare creation, directory disposal, and family pull/push arrive as below.

```sh
cd ~/limbo/gwz-dev
gwz clone --local --name A ../gwz-dev-A
# Work in A with ordinary gwz add / gwz commit.
cd ~/limbo/gwz-dev
gwz merge --remote A
# Include root explicitly when needed:
# gwz --target @all --target @root merge --remote A
gwz local dispose A --keep
```

Commands are proposed, not implemented. Quiesce involved workspaces
while each local command runs; resolve any ordinary merge preconditions.
The first preview ends at LCM1.1; the complete MVP ends at LCM1.2.

## 2. Implementation baseline and seams

Supporting checkout recorded for the revision-3 reviews on 2026-09-05:

| Repository | HEAD |
|---|---|
| gwz-dev | `27cc4861ad9db7cb5db119105804599a7748e248` |
| gwz-core | `87207c2e61e18aa11ce494a4113e29d20f6ea465` |
| gwz-cli | `c04b488efe7f5188e6c4fecebf74a4d63f20113f` |
| gwz-py | `65fc6678a1df7a0415fcb86891d50120be79033c` |

Current input design revision 8 SHA-256:
`4768268458885779e0d51ad5646bad7254d5eb783992f954c1e19f6eaff12924`.
These are planning observations, not a settled implementation acceptance.
Record the actual code/schema tuple when implementation begins.

| Existing location | Intended use / required addition |
|---|---|
| `gwz-core/protocol/gwz.taut.py` | Distinct local requests and family-source field; regenerate both clients; core derives family identity |
| `gwz-cli/src/clirequest/workspace.rs` | Thin local-clone argument path alongside existing URL clone |
| `gwz-core/src/git/gitbackend/transport.rs` and backend contract/delegates | Existing methods resolve named remotes. Add local-only anonymous fetch **and push** with explicit refspecs, plus tracking-double support, in the integration skeleton |
| `gwz-core/src/workspace_ops/merge/plan.rs` and `start.rs` | Feed the imported local source ref to existing planning/start; preserve root selection and lifecycle |
| `gwz-core/src/artifact/conf_integrity.rs` | Install final destination manifest/lock consistency through existing helpers |
| Existing mutation guards and v1 lifecycle leases | Leave their physical locking and internal ownership unchanged |

New family files have one writer, the `family-store` implementation,
reached by callers through its contract/session. Existing protected files
still use their sanctioned helpers. New copy/inspection/policy/orchestration
libraries are separate Cargo targets; a filtered core test is not sufficient.
The [boundary document](GwzLocalCloneLibraryBoundaries.md) owns package
classification, complete dependency edges, interfaces and file ownership.

Shared compiler roots, protocol generation, backend port/delegate changes,
Cargo registration and digest re-pins are serialized through lane C. Package
lanes do not edit these files. The existing pin checker runs per commit;
new module registration is an expected protected edit, not an incidental
exception. Register the planned entry points together in LCM1.0.

## 3. Milestones and stages

| Milestone | Stages | Exit |
|---|---|---|
| LCM1 — MVP | 1.0 small integration check; 1.1 create/family/keep; 1.2 named merge | Create → work → integrate → retain |
| LCM2 — delete + bare | 2.1 work/history checks; 2.2 one-shot delete; 2.3 bare factory; 2.4 family push | Explicit disposal plus a usable bare share point |
| LCM3 — the rest | 3.1 clean/branch creation; 3.2 explicit source; 3.3 family pull | Remaining current product scope and final composition tests |

### LCM1.0 — freeze interfaces, skeleton and small integration checks

**1.0a: copy smoke test.** On the development platform, copy a representative
workspace/build directory using the native path and forced ordinary copy.
Check bytes, permissions, symlinks, independence and error reporting.
Record native versus ordinary copy wall time here, not only at LCM3.
Use ordinary filesystem APIs. Additional platform adapters get their own
smoke tests when enabled; comprehensive native identity/durability proof
is outside scope. A platform without a tested native optimization uses
its tested ordinary-copy path.

**1.0b: import adapter and fixture check.** Lane T owns the tiny dev-only
`local-testrepo` crate, with fixed identity/time and explicit object format.
Do not import core's private test helpers into a leaf. Using lane C's real
new anonymous transport port, in disposable multi-repository fixtures,
fetch source commits under the same fresh `refs/gwz/local-imports/<id>`
name in every receiver. Verify all OIDs, then invoke existing merge using
that local source ref, including conflict/continue and explicit root.
Hold only the family lock around the wrapper, never an outer receiver
workspace lock. Leave the import refs present after completion/failure.
Prove this composes with existing engine entry points; do not design a
new lease or source-ref cleanup protocol.

**1.0c: contract and skeleton checkpoint.** Lane C first amends authoritative
core requirements/design and taut; confirm current schema allocations.
Freeze the boundary document's public operations, complete data/error types,
family metadata format, remote-token resolution and narrow ports in actual
compiling declarations. Include meaningful contract tests/fakes, cancellation
and partial-effect semantics; success-returning stubs are not implementations.

Land the private path crates, package classification/dependency inventory,
architecture gate and CI invocation, core dispatch skeleton, both anonymous
transport methods and tracking-double support, and generated Rust/Python
protocol. Shared compiler-root/digest updates have one owner. Unimplemented
modes refuse. Preserve existing Python `cli_local.py`; use a separate local
family command module. Verify Cargo targets in both outer and standalone
core checkouts; retain any supported Bazel targets without exposing a second
package ownership path.

Use the real contracts for copy/store conformance and fake import/install/
disposal ports. Refuse local-create/family-exchange dry-run before any write,
including creating a family lock file; malformed family start requests also
refuse before imports. Test valid internal and escaping relative hook paths
at source preflight before reservation. Implement the shared resolver's
verb-specific state/fallback table once, in `family-model`.

**1.0d: independent skeleton review.** Once the skeleton is built and its
checks pass, an agent who did not implement it reviews the exact code
commit, contracts, ownership/dependency inventory and recorded test timings.
Check isolation against unrelated unfinished code, faithful fakes/shared
conformance, real copy/transport adapters, partial effects and best-effort
scope. Record reproducible findings and a GO/NO-GO for parallel feature
development. Fix blocking findings and have the reviewer verify those fixes
before opening feature lanes. This is a review of the built skeleton, not
another review of documents alone; it does not certify unbuilt features.

**Exit:** compiled signatures and ownership match the documentation; the
skeleton and negative dependency-gate fixtures pass; each package's exact
fast command runs within the measured target or has an explicit reviewed
exception. Record host/toolchain/SHA and execution/warm/cold timings. The
real copy/import spike verifies the external adapters, including root and
existing engine entry points. Cold compilation is not billed as test time.
The independent skeleton review records GO with blocking findings resolved.
Only then open parallel feature implementation lanes. Full library behavior
remains TDD work after the checkpoint. If an adapter fails, simplify that
adapter instead of expanding the merge lifecycle or filesystem guarantees.

### LCM1.1 — create, list and retain

**1.1a: family model/store.** Shared token resolution, name/path validation,
canonical-path/pointer checks, a small allocation marker, locked index
updates and a 1 MiB encoded index limit. List reads/reports only. Local mutation/exchange commands refuse
nonready rows; ordinary GWZ commands are not globally retrofitted.

**1.1b: copy/install.** Inspect every included repository, including
ignored/unmanaged nested ones, before reservation. Refuse gitfiles,
external objects/configuration (including escaping relative hook paths)
and unsupported layouts before reservation. Apply exclusions
during traversal; omit source family index/lock/pointer and runtime
identity. Install fresh metadata, final manifest last, then ready row.
Detected source drift or failed writes stop and retain the destination.

**1.1c: explicit exits and drivers.** Incomplete rows remain incomplete;
no automatic promotion, copy resume or tree cleanup. Implement keep,
explicit repeatable pointer-only disband, and Rust/Python presentation.
Keep from an incomplete tree does not claim to repair it.

**Exit:** root → A → B has one namespace and independent data. Test
ordinary copy failures, interrupted writes/copy, name collisions, source
exclusions, nested Git refusals and list's lack of side effects. Every
reported failure leaves inspectable data; no universal crash-recovery
claim or exhaustive filesystem-state matrix is required.

### LCM1.2 — named merge through retained imports

Reject unsupported dry-run/invalid start shape before lock-file creation
or imports. Resolve via the shared family model, then pair all selected
repositories, including root when selected;
capture source OIDs; fetch under a fresh collision-checked import ref;
verify all received OIDs before engine entry. Failed partial imports may
leave refs. Retry with a new id. Imported refs survive ordinary Git GC.

Delegate to the existing engine with the local source ref and existing
selection/policy. Do not persist family remotes, change `origin` refs,
prehold a receiver lock, alter merge records, or register imported refs
with the strict merge backup-ref cleanup grammar.

**MVP exit:** commit and merge by family name; conflict/continue/abort;
source detach/advance after start; retained imports after failed start;
source objects survive ordinary GC. Run both drivers and confirm ordinary
non-family merges retain their behavior. Two independent lanes may each
have an open merge; simultaneous mutation of the same involved workspace
is outside the local command's quiescence contract.

### LCM2.1 — work detector and read-only preservation check

Library implementation can start alongside LCM1.1 after the interface
checkpoint; it does not wait for the MVP command path.

Inspect root, declared and nested repositories; staged/unstaged/untracked/
ignored work; native and GWZ unfinished operations; refs, HEAD, reflogs,
stashes and coordination evidence. Handle status-suppression flags with
actual file observation or explicit unknown. Git status is not the oracle.

Verify all protected history is reachable from locally available refs/HEAD
in eligible surviving family repositories. Persistent local-import refs
are eligible; existing temporary merge refs are not. Patch equivalence
and cached origin state do not suffice. Unknown evidence/resource limits
refuse. The result is in-memory and valid only for the current invocation;
there is no persisted proof or proof-artifact transaction.

**Exit:** precise dirty/unpreserved/unknown explanations, including root,
old reflogs, stash coordination, nested repos, flagged files and bare Git
fixtures. This library cannot delete anything.

### LCM2.2 — explicit one-shot deletion

Under the family lock and endpoint/witness quiescence assumption, validate
an intact ready target, run fresh checks, write disposing and remove the
validated directory. Refuse root/cwd/replacement paths. Ordinary deletion
requires clean work and preservation; explicit named hazard waivers keep
the existing ready-tree semantics. Boolean-only force is insufficient.

On any error, stop and report partial effects. An interrupted disposing
row cannot resume deletion. Keep may detach its remainder; manual cleanup
is acceptable. Only explicit dispose may remove a stale row for an absent
target. Never let list or another command perform directory deletion.

**Exit:** preserved clean lane deletes; unique/unknown work refuses;
partial deletion stops; keep retains the remainder; reads never retry.
Tests cover ordinary failure/interruption points and mistakes, not a new
power-loss recovery protocol. Bare-specific cases repeat in LCM2.4.

### LCM2.3 — bare repository factory

Build independent root/member bare repositories from captured objects,
refs and HEADs; use the common destination installer and recapture helpers.
No source worktree data or borrowed objects. Public clean mode and branch
selection remain LCM3. Failed creation is retained like any other copy.

### LCM2.4 — family push and useful hub

Use the anonymous push port landed in LCM1.0 and the shared resolver;
add explicit local push refspecs through the existing Git adapter. Keep
checked-out-branch refusal, selection and partial-result semantics. Do not
change hub HEAD or record family URLs in configuration. Complete driver
support and test actual hubs with the preservation/deletion code.

**LCM2 exit:** B pushes a branch to hub; D integrates it by named merge;
a hub with unique history cannot be ordinarily deleted.

### LCM3.1 — clean checkout and branch selection

Extend the repository factory with worktree checkout and `-b`. Validate
all collisions before reservation. Use one captured HEAD vector and
recapture lock/desired configuration before ready. Report generated root
configuration changes without making a hidden root commit.

### LCM3.2 — explicit source selection

Implement `--from <name|path>` using existing copy/inventory helpers.
Caller chooses the registering family; source selection chooses content.
Refuse ambiguous/unsupported foreign-family cases rather than silently
reparenting. Cross-family concurrent operations require operator quiescence.

### LCM3.3 — family pull and final acceptance

Use the shared resolver and retained-import approach with an explicit
source-preparation adapter to existing same-branch pull semantics; preserve
origin fallback. The existing name-based fetch is not already this adapter. Complete Rust/Python parity and the
full workflow examples. Test clean/verbatim/bare composition and measure
copy performance. This supplements tests already shipped in LCM1/LCM2.

## 4. Parallel lanes and validation

Milestone numbers describe product release gates, not a requirement that
all implementation waits for the preceding milestone. C owns integration
and contracts; the package lanes own only their directories. Stage work
starts against the compiled contracts and faithful fakes. Integration waits
for the listed real dependencies. Lane ownership and exact paths are in the
[boundary document](GwzLocalCloneLibraryBoundaries.md#4-file-ownership-and-landing-discipline).

| Stage / work | Lane | Prerequisites for implementation | Real integration prerequisites | Focused command(s), proposed |
|---|---|---|---|---|
| 1.0c contracts, inventory, skeleton | C with F | This architecture and policy scope | Before opening feature lanes: compile contracts, gate and generation checks | Boundary gate; each contract package `cargo test -p … --lib` |
| 1.0a copy spike | R | Can experiment during contract work | Final spike uses frozen copy contract | `cargo test -p gwz-refcopy --lib`; selected native Tier B smoke |
| 1.0b fixture/import spike | T, C | T fixture design can run alongside 1.0c | Final adapter check consumes new real transport port | `cargo test -p gwz-local-testrepo --lib`; core `local_clone::tests::import` slice |
| 1.0d independent skeleton review | Independent reviewer; C fixes findings | Built skeleton, passing checks, exact commit and measurements from 1.0a–c | GO and verified blocking-finding fixes before W1 | Re-run relevant package/gate/adapter checks; written review tied to the commit |
| 1.1a family model/store | F, S | Completed 1.0 checkpoint | Real store and path checks for create/list/keep | `cargo test -p gwz-family-model --lib`; `cargo test -p gwz-family-store --lib` |
| 1.1b copy/inspection/install | R, I, N | Completed 1.0; install can use fake ports | Refcopy, repo-inspect, family-store, configuration adapter | `cargo test -p gwz-refcopy --lib`; `cargo test -p gwz-repo-inspect --lib`; `cargo test -p gwz-workspace-install --lib` |
| 1.1c keep/disband/drivers | D, CR, CP, C | Frozen protocol, family model/store contract; no full MVP dependency for parsing | Real family store plus Rust/Python dispatch | Disposal Tier A; focused driver parsing tests; parity at 1.1 exit |
| 1.2 import/merge | X, C | Completed 1.0; fake local transport | Real transport, family store/model, inspection, existing merge adapter | `cargo test -p gwz-local-import --lib`; core import/merge Tier B slices |
| 2.1 work/history | W, H | Completed 1.0; repo-contract plain observations/reader fakes | Real repo-inspect + core GWZ evidence adapter for acceptance | `cargo test -p gwz-work-detector --lib`; `cargo test -p gwz-history-check --lib` |
| 2.2 removal | D, C | Completed 1.0; fake fresh-evidence/history/removal ports; D sequences after keep API | 1.1 family/store/install, 2.1 actual work/history, real removal adapter | `cargo test -p gwz-local-disposal --lib`; disposal Tier B slice |
| 2.3 bare / 3.1 clean/branch | B, N, C | Completed 1.0; builder contract/fakes; B owns both modes | Real Git builder and 1.1 installer; selected-mode fixtures | `cargo test -p gwz-repo-factory --lib`; install/factory Tier B slices |
| 2.4 push / 3.3 pull | X, C | Completed 1.0; shared resolver/transport port; X sequences after import API | Real 1.2 import adapter and 2.3 hub; pull source-preparation integration | `cargo test -p gwz-local-import --lib`; push/pull Tier B slices |
| 3.2 source selection | N, I, C | Completed 1.0; existing source/inventory ports | 1.1 installer and supported source-layout checks | Install/inspection Tier A; source-selection Tier B slice |

**Suggested scheduling:** W0 is the contract/skeleton/fixture and real-spike
checkpoint, followed by its independent review. W1 runs R, I, F, S, W and H
independently, plus driver parsing;
N, X, B and D may also develop their packages against frozen ports as
capacity permits. W2 integrates create/list/keep and named merge, then work/
history and disposal when their real adapters pass. W3 composes bare/push
and clean/source/pull. One lane owns shared files across stages: never split
bare and clean, or import and push, into conflicting concurrent edits.
Product exits remain LCM1 → LCM2 → LCM3.

Tier A is each library's independent command: target **≤2 seconds execution,
≤10 seconds warm edit/build/test per package**. Tier B is a real-adapter
slice, target ≤10 seconds execution and ≤30 seconds warm command, separately
measured. Tier C is protocol, cross-driver, native-platform and existing
broader acceptance. Native-path execution/forced fallback, independent
copied bytes, flagged edits, partial effects and read-only list all retain
direct-observation tests. Record cold builds separately and measure copy
performance at 1.0a. No fast target has yet been measured for these packages.

For drivers, C freezes exact new test module/filter names in 1.0c. Python
protocol checks and focused local-family parsing tests are fast checks;
`gwz-py/run_tests.py` remains broad acceptance. The existing core/driver
build closure is not advertised as an isolated library target. Public
contract changes run all affected consumers plus adapter slices; internal
library changes start with the owning package. Real declared dependencies
remain dependencies; a filter does not avoid their compilation.

Run generated protocol/client and CLI reference checks for changed schemas
and commands, plus focused existing merge regressions for the adapter.
Update authoritative contracts before behavior. Preserve the current
protected-code checker and per-commit checks; C owns needed reviewed
re-pins. Test the new package dependency gate with negative fixtures, and
run it in CI. No new catalog or global admission audit is required.

This policy strengthens boundaries and feedback without restoring native
durable identity, exhaustive crash recovery or deletion replay. Existing
merge guarantees remain unchanged. Documentation corrections alone do not
satisfy the compiled checkpoint or implementation evidence. No release is
authorized here.

## 5. Revised complexity and effort

Estimates are focused engineer-days for an engineer familiar with the
codebase, including implementation, focused tests, both drivers, docs and
ordinary remediation. They assume ordinary filesystem/Git helpers and the
existing merge engine are reusable. They exclude release/queue time and
new platform infrastructure. Additional platform work is estimated when
selected; all-platform durability certification is not hidden in the total.

| Milestone | Complexity | Days |
|---|---|---:|
| LCM1 — MVP | 3/5, moderate | 9–16 |
| LCM2 — delete + bare | 4/5, high because deletion still checks history | 7–12 |
| LCM3 — the rest | 3/5, moderate | 4–7 |
| Total | Best-effort scope with enforced new-library boundaries | **20–35** |

| Stage | Days |
|---|---:|
| LCM1.0 | 2–4 |
| LCM1.1 | 5–8 |
| LCM1.2 | 2–4 |
| LCM2.1 | 4–6 |
| LCM2.2 | 1–2 |
| LCM2.3 | 1–2 |
| LCM2.4 | 1–2 |
| LCM3.1 | 2–3 |
| LCM3.2 | 1–2 |
| LCM3.3 | 1–2 |

Revision 4 adds **1–2 engineer-days** to revision 3's 19–33 total for the
explicit contract/package skeleton, fixture ownership and bounded policy
gate. F51's narrower six-leaf correction estimate was 1–2 days inside the
old spike budget; this plan also isolates orchestration libraries and adds
local policy enforcement. LCM1.0's 2–4-day allowance includes the independent
skeleton review and routine corrections; substantive redesign findings
require re-estimation. Do not silently assume that extra work is free.
These are summed effort ranges, not calendar promises or a division by
agent count. Parallel library implementation shortens waiting; shared
contract/integration ownership and milestone acceptance remain serial.

The earlier 37–66-day estimate was reduced because durable identity,
continuous merge-lease integration, proof persistence, deletion replay
and automatic import cleanup have been removed from scope. This is still
judgment, not measured productivity. Re-estimate after the small LCM1.0
fixture. Keep its experiments bounded; failure is a reason to simplify
or explicitly revise scope, not begin another filesystem subsystem.

## 6. Review disposition and remaining gate

The originating GPT-6/Sol reviewers closed their seven revision-6 concerns
at the revised contract/planning level: no extra receiver lease, mandatory
nested Git inventory, observation-only list, retained ordinary import refs,
flag-aware physical work observation, bounded in-memory history checks,
and core-derived family identity. Implementation acceptance remains future.

The second independent round found isolation, family dry-run and relative
hooks preflight gaps. F51 added missing ownership/interfaces, shared pinned
files, a nonexistent anonymous transport seam, shared resolution, packaging,
timing, store primitives, fixtures and start-order gaps. This revision and
[architecture revision 3](GwzLocalCloneImplementationArchitecture.md) address
those in the plan; details and qualified dispositions are recorded in
[the reconciliation](GwzLocalClone-ReviewPlanArch-Adoption.md).

**GO for LCM1.0. Parallel implementation waits for its compiled checkpoint
and independent skeleton review.**
Complete interfaces, negative dependency checks, measured fast loops and
real adapters remain required evidence, not facts established by these doc
edits. The prior independent reports are preserved unchanged.
