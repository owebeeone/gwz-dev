# GWZ local clone — LCM1.0c checkpoint record (lane C)

> **Status (lane owner, 2026-09-05): accepted at gwz-core
> `a8eae4e428abe296bb105b693443ee47a9caf045` / gwz-cli
> `86840f67e23a9ef04312df0dd7c8f781cc476e04` / gwz-py
> `afcd5a396ccaf5e64884f8f1cec2f66273ac08bb` with this record, after
> `GwzLocalClone-LCM1.0c-ReviewCode-2.md` (Fable 5.1, medium) and
> `GwzLocalClone-LCM1.0c-ReviewState-2.md` (Opus 5) both reported GO on that
> revision; this accepts the LCM1.0c skeleton checkpoint (plan stage 1.0d)
> only.** Round 1 (`-ReviewCode.md`, `-ReviewState.md`): NO-GO, three P2 root
> causes (C-P2-1, S-P2-1 = C-P3-4, S-P2-2) and seven distinct P3s; remediated
> in one commit (`LCM1.0c-rem1`, plan `GwzLocalClone-LCM1.0c-RemPlan.md`);
> remediation rounds used: 1 of 2. Round 2 opened four new P3s
> (C2-P3-1, C2-P3-2, S2-P3-1, S2-P3-2), all non-architectural, none
> blocking; they are lane C follow-ups queued before lane S writes the real
> store (S2-P3-1, C2-P3-1, C2-P3-2) and alongside operator item §7.6
> (S2-P3-2). Escaped defects: none known. Parallel feature lanes (W1) may
> open on this tuple. Root checkpoint commit pending operator approval.


Status: **BUILT 2026-09-05; remediation round 1 applied (see the
"Remediation round 1" section at the end), awaiting the LCM1.0d round-2
re-review.**
This is the evidence record of plan revision 4 stage 1.0c ("contract and
skeleton checkpoint") for the boundary document revision 1. Everything
below is measured on the tuple in §1 unless labelled otherwise; targets
from the boundary document are quoted as targets, never as results.
Written by the skeleton implementer; it is not the 1.0d review.

## 1. Exact tuple

| Repository | Before | After LCM1.0c | Branch | Working tree |
|---|---|---|---|---|
| gwz-dev (root) | `0c9f715b480fa5c7cec13ec99b2c233f016ef314` | `fc8b18966f24c14f422147d16f61a0dd19159670` | `main` | committed at `fc8b189` ("LCM1.0c checkpoint: pin gwz-core 52468ca …"): the root `Cargo.lock` (the gwz-core `0.13.0 → 0.14.0` line plus the thirteen new path packages) and this document. Closes §7.7. (Round-1 remediation then re-edits this document in the working tree; the lane owner commits the root checkpoint.) |
| gwz-core | `87207c2e61e18aa11ce494a4113e29d20f6ea465` | `52468cac728b7b5a877071187de2cb04196821ec` | `main` | clean |
| gwz-cli | `c04b488efe7f5188e6c4fecebf74a4d63f20113f` | `86840f67e23a9ef04312df0dd7c8f781cc476e04` | `main` | clean |
| gwz-py | `65fc6678a1df7a0415fcb86891d50120be79033c` | `afcd5a396ccaf5e64884f8f1cec2f66273ac08bb` | `main` | clean |

gwz-core commits, in landing order (each passes the boundary checker on its
own tree — `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 87207c2
HEAD`: six `lane gate: ok` lines, 48.9 s):

| SHA | Commit | Deliverable |
|---|---|---|
| `7faf475` | allocate the local clone family protocol and amend core authority | 1 |
| `a69f460` | contract crates and package skeletons under crates/ | 2, 3 |
| `f6b1283` | core integration skeleton, anonymous transport ports and re-pins | 4 |
| `06d7a00` | local clone library boundary gate and CI job | 6 |
| `e169877` | standalone-checkout Tier A form (manifest-path CI step, `cfg(any(test, feature))` conformance modules), privacy probe harness copies `crates/` | 6, §5 |
| `52468ca` | lib remainder pin 1018 darwin (measured) / 1019 linux (derived) | §5.5 |

gwz-cli `86840f6` (compilation-required field only); gwz-py `afcd5a3`
(deliverable 5). No push, no tag, no branch, no attribution trailer.

Authority read and followed, in the prescribed order: boundaries revision
1, plan revision 4, architecture revision 3, design revision 8 (§7 numbers
re-verified against `gwz.taut.py` at `87207c2` before allocating:
`ActionKind` ended at `log=26`, `MergeRequest` F(8) was
`filesystem_strict`, `PushRequest.remote` F(2), `OperationPolicy.remote`
F(5)), the adoption and F51 reviews, the canonical policy (SHA-256
`dcc4fbd2b45caf92…` verified), AgentProcessRules L1-05/06/12/17/20/25/28/29
and L2-01/02/03, the gwz-core docs and the regeneration scripts.

Host for every measurement: Apple M3 Pro (12 cores, 36 GiB), macOS 26.6.2
(Darwin 25.6.0 arm64), APFS, Rust 1.95.0 via `rust-toolchain.toml`
(`cargo 1.95.0 (f2d3ce0bd 2026-03-21)`), Python 3.13.12 for every
`tomllib` script (`python3` on this host is 3.10 and cannot run the
checkers; CI uses 3.11).

## 2. What landed, per deliverable

### 2.1 Authority and protocol (`7faf475`)

`protocol/gwz.taut.py`, name-for-name from design §7: `ActionKind.
clone_local_workspace=27`, `local_family=28`; enums `LocalCloneMode`
(verbatim=0, clean=1, bare=2) and `LocalFamilyOp` (list=0, dispose=1,
disband=2); `CloneLocalWorkspaceRequest` (meta 1, name 2, dest 3, mode 4,
branch 5); `LocalFamilyRequest` (meta 1, op 2, name 3, keep 4,
force_hazards 5, `List(STR)`); envelope-only `CloneLocalWorkspaceResponse`
and `LocalFamilyResponse`; the two `GwzCore` service methods;
`MergeRequest.local_source_name = F(9)`. `CloneWorkspaceRequest.url` is
untouched. Regenerated with `protocol/regen.py` (taut-proto 0.9.1):
`src/protocol/generated.rs`, `protocol/corpus/*`; `docs/MessageCatalog.md`
regenerated (it had drifted since gwz-log/M5d — the regeneration carries
that backlog, 531 inserted lines, none hand-edited).

Two allocations the design names are deliberately **not** made (§7):
`CloneLocalWorkspaceRequest` tag 6 (`--from`) and the `list` payload of
`LocalFamilyResponse`. Both are held with comments in the schema and are
operator questions in §7 of this document; neither blocks W1.

Pins moved, each with its dated reason and a measured-additive proof:
`protocol/check_log_additive.py` `71bf6b92… → 3c34bd74…` (the pre-log
projection rendered on both trees and diffed: 242 added lines, 0 removed,
every added object one of the items above); the same fingerprint in gwz-py
`scripts/check_protocol_drift.py`; the `MergeRequest` parity hex in
`tests/protocol.rs` (`a8…08f6 → a9…08f609f6`, slot 9 null).

Core authority amended: `dev-docs/GWZRequirements.md` REQ-160..REQ-165;
`dev-docs/GWZDesign.md` "Local Clone Family" and the two anonymous ports in
the Git Backend sketch; `docs/Protocol.md`, `docs/RustApi.md` (handler map,
`local_clone` module), `docs/ErrorCatalog.md` (no new code allocated;
`UnknownLocal` recorded as open), `docs/WorkspaceArtifacts.md` (family
files, format v1), `docs/GitBackend.md` (with the skeleton commit).

Engine guard: `validate_merge_request` refuses a request that still carries
`local_source_name` on every op (`merge_validation_failed`), so the selector
is wrapper-owned by construction. This is a three-line addition to
`src/workspace_ops/merge/validate.rs` (not a pinned file; `merge/mod.rs`
untouched); the reviewer should accept or reject it explicitly.

### 2.2 Contracts (`a69f460`)

| Crate | What is compiled and tested |
|---|---|
| `gwz-copy-contract` | `TreeCopier::copy_tree(&CopyRequest, &dyn Cancellation) -> Result<CopyReport, CopyError>`; request owns paths, `Exclusion::RelativePath` and `CopyMode::{Auto, OrdinaryOnly}`; report owns native/ordinary/directory/symlink counts, logical bytes and warnings; error owns failed path, category (incl. `Cancelled`, `DestinationNotEmpty`, `Unimplemented`) and the partial report. `contract_tests`: `run_all` (8 cases: exclusions before copy, empty-destination admission, non-empty refusal without writing, missing source, pre-cancellation with an accurate partial report, ordinary-only reports no native files, never hardlinks + independent writes, symlinks stay links), the std-only `OrdinaryTreeCopier` fake that passes it, and `ScriptedTreeCopier` for consumers |
| `gwz-repo-contract` | `RepoInspector::{inspect_layout, observe_work, inventory_history}`, `ObjectReader::{retained_roots, read_object}`; `ObjectId` carries `ObjectFormat` (Sha1/Sha256, length-checked); `BytePath`; `Observation<T>::{Known, Unknown(reasons)}`; `WorkObservation` with conflicts, ignored data, suppressed entries with physical state, sparse absence, native operation, stashes; `ProtectedRoots`/`RootSource`; `ReadLimits`; `ObjectRecord` with edges; typed `LayoutError` (design §4.0 hazards enumerated) and `ReadError`. `contract_tests`: `InMemoryObjectReader` (limits, missing, repeatable), `GraphFixture::small`, `object_reader_conformance` (5 cases), `ScriptedRepoInspector` defaulting to refusal/unknown, `inspector_conformance` |
| `gwz-family-store-contract` | `FamilyStore::{read_view, try_lock}` with `type Session: FamilySession`; `FamilySession::{root, reread, found, apply, install_pointer, remove_pointer}`; `FamilyLocation`, `FamilyObservation::{NoFamily, Family{root, source, view}}`, `StoreError` (Busy, LockingUnsupported, Malformed, Oversize, ConflictingMetadata, PointerTargetInvalid, NoFamily, Refused, Io, **Partial{completed effects}**, Unimplemented), `MetadataEffect`, `AppliedChange`. `contract_tests`: `StoreFixture` port, `run_all` (6 cases: reads create no lock file, found/reread round trip, second try_lock is Busy until drop, refused transition writes nothing, apply writes only the matching index change and Disband is repeatable, malformed/oversize refuse), `InMemoryFamilyStore` fake (faithful lock, reread/validate, partial effects, pointer following) that passes it |

Every contract type is owned plain data; no `git2`, OS handle, core model
error or protocol type crosses the boundary. Cancellation is a polled port
(no preemption promise). Requests are invocation-local values.

### 2.3 Package skeletons (`a69f460`)

`gwz-family-model` (pure, **implemented**): names (`MemberName::parse`
rejects `root`/`origin`/`HEAD`/`FETCH_HEAD`, empty, `/`, `:`), ids, rows,
the frozen format-v1 constants and field names, `TargetObservation` →
`ListState` classification and `project_list` (observation-only),
`validate_transition` (allocate/record-error/ready/disposing/remove/disband
with typed `Refusal`s: name/path/nesting collisions, inside-root paths,
wrong state, allocation mismatch), and **the one resolver**
`resolve_remote_token(view: Option<&FamilyView>, token: Option<&RemoteToken>,
verb) -> Resolution`. The verb-specific table test
(`resolve::tests::one_table_covers_every_verb_state_and_fallback`) covers
merge/pull/push × {no token, root, ready A, creating B, disposing C, absent
Z, origin, HEAD, upstream} plus the no-family case: merge non-ready →
`UnknownLocal{state}`, pull/push non-ready → `LifecycleRefusal`, absent →
`GitRemoteCandidate` (existence decided by core's existing lookup, not
here), no token → `NoToken`. Signature note: `view` is `Option<&FamilyView>`
so "no family" is a valid observation distinct from a malformed one (which
the store refuses before this function is reached).

Skeletons with frozen signatures, complete owned types and ports, recording
fakes, and typed refusal **before any port/session call** (each asserted by
its Tier A test): `gwz-refcopy` (`SystemTreeCopier`, `probe_native`,
`NativeMechanism`), `gwz-repo-inspect` (`LocalRepoInspector`,
`LocalObjectReader`), `gwz-work-detector` (`classify_work` → `Unknown`,
never `Clean`; `GwzEvidence`, `Hazard`, `WorkReport{truncated}`),
`gwz-history-check` (`check_history` → `Unknown`, never `Verified`;
`Witness`, `Limits` 100 000 roots / 256 MiB, own `Cancellation`),
`gwz-family-store` (`YamlFamilyStore`, `LockedFamilySession`),
`gwz-local-import` (`prepare_import`, `push_local`, `LocalTransport` port,
`TransferId`/`refs/gwz/local-imports/`, `RecordingTransport` fake),
`gwz-workspace-install` (`install`, `InstallPorts`, `RecordingInstallPorts`),
`gwz-repo-factory` (`construct`, `RepoBuildPort`, `RecordingBuildPort`),
`gwz-local-disposal` (`dispose`, `DisposalPorts`, `HazardWaiver`
vocabulary `open-merge`/`dirty`/`unpreserved-history`,
`RecordingDisposalPorts`). `crates/local-testrepo` is lane T's and was not
created; the inventory pre-classifies it as pending.

Manifests: exact Cargo names from boundaries §2, `edition = "2024"`,
`rust-version = "1.95"`, `publish = false`, first-party dependencies exactly
the allowed edges (contract-tests features only through dev-dependencies),
no third-party dependency in any crate yet. `gwz-core/Cargo.toml` depends on
all thirteen (core is the composition root; this is what makes `-p <name>`
resolvable from a standalone core checkout, where path dependencies are
not workspace members). `gwz-core/Cargo.lock` gained exactly those thirteen
entries. Note for later lanes: gwz-core's `clippy.toml` merge-writer
disallow list is inherited by `crates/*` through clippy's parent walk; the
copy contract's test-support module carries a scoped `allow` with a reason
(as `src/lib.rs` does), and crates doing ordinary file I/O will need the
same.

Signature notes against boundaries §3, for the reviewer (none changes a
contract; each is a refinement the table left open):

- `resolve_remote_token(Option<&FamilyView>, Option<&RemoteToken>, Verb)` —
  both inputs optional so "no family" and "no token" are explicit inputs.
- `install(...)` takes a fifth `&dyn Cancellation` (the copy contract's
  port) and returns `Result<InstallReport, InstallFailure>` where
  `InstallFailure { error: InstallError, effects }` carries the completed
  effects beside the typed cause; `dispose(...)` returns
  `Result<DisposeReport, DisposeFailure>` for the same reason.
- `FamilySession` adds `root`, `found`, `install_pointer` and
  `remove_pointer` to the table's `reread`/`apply`: founding a family and
  the destination pointer/marker writes are store-owned effects and must be
  reachable only through the live session.
- `LocalTransport` names its five required methods `resolve_source`,
  `ref_exists`, `fetch_anonymous`, `push_anonymous`, `read_ref` (the table's
  resolve-source, collision-check, anonymous explicit fetch/push and
  received-OID reads).
- `gwz-history-check` and `gwz-local-import` each own a `Cancellation`
  trait (neither may depend on the copy contract).

### 2.4 Core integration skeleton (`f6b1283`)

The one coordinated protected edit: `pub mod local_clone;` in `src/lib.rs`;
`mod handle_local; pub use handle_local::*;` in `src/workspace_ops/mod.rs`;
`ActionKind::{CloneLocalWorkspace, LocalFamily}` and the two
`OperationRequest` variants in `src/operation/push_event.rs`; the two
required ports on `GitBackend` (`git/gitbackend/contract.rs`), their
`transport.rs` bodies, the two `delegate!` rows in `git/gitbackend.rs`, and
the `TrackingBackend` implementation (records both ports, injects one
failure). `merge/mod.rs` untouched; no `#[path]`, no `include!`; tests only
under `tests` path segments.

`src/local_clone/`: `errors.rs` (library error → existing `GwzErrorCode`
mapping: collisions → `path_collision`, missing rows → `member_not_found`,
busy lock → `open_operation`, undecodable family metadata →
`manifest_invalid`, unimplemented store → `unsupported_operation`),
`request.rs` (shape validation for the three slots), `transport.rs`
(`BackendLocalTransport` implementing `gwz_local_import::LocalTransport`
over the ports), `family_merge.rs` (the wrapper), `tests/` (Tier B slices).
`src/workspace_ops/handle_local.rs`: `handle_clone_local_workspace`,
`handle_local_family`, `handle_merge_with_local_family` (the entry drivers
dispatch every merge through; a request without the selector reaches
`handle_merge_with_events` unchanged).

Refusal order in every slot, observed on a real workspace
(`local_clone::tests::request`): attribution + shape → unsupported family
`dry_run` (`unsupported_operation`) → workspace discovery → family
observation through the store **contract** (`gwz-family-store::read_view`,
which refuses `Unimplemented` at this checkpoint) → only then reservation /
copy / import. Result: every local-family operation returns
`unsupported_operation` naming the verb/mode, with no lock file, index,
pointer, marker, copy, import ref or record created. Malformed family start
requests refuse `invalid_request` before that; a selector reaching the
engine directly is refused by the engine.

### 2.5 Driver wiring (gwz-py `afcd5a3`, gwz-cli `86840f6`)

gwz-py: protocol package regenerated (`scripts/regen_protocol.py`, taut-proto
0.9.1), drift pin moved with reason, `test_protocol.py` pins 27/28 and the
two enums; the generated `MergeRequest` dataclass gained a required
`local_source_name` field, so `GwzClient.merge_request` sets it to `None`,
the two test constructions follow, and `test_codec.py`'s parity hex moves
`a8…08f6 → a9…08f609f6` byte-identical to the Rust pin;
`native/src/dispatch/local_family.rs` routes
`clone_local_workspace` (call + submit) and `local_family` (call) through
the core slots, `dispatch/merge.rs` calls `handle_merge_with_local_family`,
`Cargo.lock` gains the thirteen path crates. No `cli_local_family.py` was
needed to import or compile; the existing unrelated `cli_local.py` is
untouched. gwz-cli: the one `MergeRequest` literal gains
`local_source_name: None`; parsing and the entry switch are lane CR's.

### 2.6 Local boundary gate (`06d7a00`)

`scripts/checks/local_clone_inventory.json` (fourteen packages, roles,
owners, rationales, complete first-party/third-party allowlists for normal
and dev kinds, `role_edges`, forbidden names, policy SHA) and
`scripts/checks/check_local_clone_boundaries.py`, which reads each manifest
through `cargo metadata --no-deps --manifest-path` (no build, lock or
network) and rejects: unclassified crates; absent/renamed/publishable/
inheriting/nested-workspace classified crates; any declared edge — normal,
build, dev, optional, target-specific, renames resolved to the real package
name — outside the allowlist; forbidden dependencies in any kind; role
direction violations; a harness in a non-dev position; test closures that
reach a forbidden or unclassified package; a package without a tested lib
target. Coverage printed honestly on every run: declared edges and closures
only — no transitive third-party audit, macro expansion, Rust parsing,
trait proof or public-type leakage analysis (those are the conformance
suites, compiler witnesses and review). `test_check_local_clone_boundaries.py`:
17 tests including the §6 negative fixtures (unclassified crate, reversed
contract edge, aliased forbidden dependency, optional/target/dev
violations) plus aliased sibling, harness-as-normal, transitive closure,
missing expected package, renamed package, nested workspace, malformed
inventory. CI: job `local-clone-boundary` in
`.github/workflows/checked-artifact-boundary.yml` runs the gate, its suite
and every library's `cargo test --locked -p <name> --lib --all-features`
from the runner's standalone checkout. A workflow does not configure
branch protection by itself.

## 3. Gate commands and counts (measured on gwz-core `06d7a00`..`52468ca` in the outer checkout; the rows re-run after e169877 and 52468ca are marked)

Round-1 remediation evidence inputs (added to the evidence list, LCM1.0c-rem1):
the two peer-blind reviews `dev-docs/GwzLocalClone-LCM1.0c-ReviewCode.md`
(Fable 5.1, medium) and `dev-docs/GwzLocalClone-LCM1.0c-ReviewState.md`
(Opus 5), the review prompt `dev-docs/GwzLocalClone-LCM1.0cReviewPrompt.md`,
and the remediation plan `dev-docs/GwzLocalClone-LCM1.0c-RemPlan.md`. The
round-1 gate results are tabulated in the "Remediation round 1" section at
the end of this document.

| Gate | Result |
|---|---|
| `cargo fmt --all -- --check` | clean (root workspace: gwz-core, gwz-cli, all crates) |
| `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings` (gwz-core) | clean |
| `cargo clippy -p <crate> --all-targets --all-features -- -D warnings` × 13 | clean |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | `ok (24 visible entries, 9 classified modules)` |
| `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 87207c2 HEAD` | ok at each of the six commits 7faf475, a69f460, f6b1283, 06d7a00, e169877, 52468ca (48.9 s) |
| Fault-battery pins by `--list` partition (lib binary at HEAD vs a worktree at 87207c2) | checked_artifact:: 459 / 459, v1_lifecycle:: 266 / 266, remainder 1019 / 1004 listed (each incl. 1 ignored); pin moved 1004 -> 1018 darwin measured, 1019 linux derived (52468ca) |
| `python3.13 scripts/checks/run_r4bg_aggregate_gates.py fault:3` / `fault:4` (executed at 52468ca) | ok: checked-artifact census `459 passed` (53.3 s); lib remainder `1018 passed` (52.4 s). Partitions 1–2 (v1 lifecycle 265 + release-profile 1) are unmoved by `--list` and were executed inside the whole-suite run |
| `python3.13 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py` | 74 tests OK, 701 s (cold probe target after the disk cleanup, contended by a base-commit build) |
| `python3.13 -m unittest scripts/checks/test_release_boundary.py` | OK (6 tests) |
| `python3.13 scripts/checks/run_r4bg_aggregate_gates.py privacy` | ok: sealed v1 lifecycle compile probes 96.7 s, 8 probes OK (after the harness copies `crates/`, e169877) |
| `python3.13 scripts/checks/run_r4bg_aggregate_gates.py call-graph` | ok: structural boundary 7.6 s; checker suite and compiler probes 685 s; release boundary 0.2 s |
| `python3.13 protocol/regen.py --check` | `OK -- committed protocol artifacts are current` |
| `cargo test -p gwz-core --test protocol` | 36 passed (incl. `generated_protocol_is_current`, `log_addition_preserves_the_complete_pre_log_wire_projection`) |
| `cargo test --lib workspace_ops::merge::v1_lifecycle::tests::no_ff_wire` | 7 passed |
| `python3.13 scripts/checks/check_local_clone_boundaries.py` | `ok` — 13 packages, 26 declared edges, 0.5 s |
| `python3.13 -m unittest scripts/checks/test_check_local_clone_boundaries.py` | 17 tests OK, 1.9 s |
| `cargo test -p gwz-core --lib local_clone` | 13 passed (Tier B slices `request`, `transport`, plus unit tests) |
| `cargo test -p gwz-core --lib operation::` | 135 passed |
| gwz-py: `cargo check --tests`, `cargo clippy --all-targets -- -D warnings`, `cargo fmt --check` | clean |
| gwz-py: `.venv/bin/python scripts/check_protocol_drift.py` | OK `sha256:c238a3ac…` (full IR) |
| gwz-py: `pytest src/tests/test_protocol.py` | 6 passed |
| gwz-cli: `cargo check -p gwz --tests` | clean |
| Whole gwz-core suite, once, at the end: `cargo test -p gwz-core --no-fail-fast` | lib 1743 passed / 0 failed / 1 ignored (776 s); integration 1+10+2+36+9 passed; 13 min 24 s wall (§5.5) |

`check_merge_docs.py` was not run: no merge document moved.

## 4. Tier A timings (measured; targets are ≤ 2 s execution, ≤ 10 s warm)

Full per-library tables are in §5.3 (standalone clone, cold) and §5.4
(outer checkout, warm edit loop). Summary across all 13 libraries, host
Darwin 25.6.0 arm64, cargo 1.95.0:

| Measure | Range | Target |
|---|---|---|
| Harness execution time (`finished in`) | 0.00–0.03 s | ≤ 2 s |
| Warm edit -> build -> test wall, outer checkout | 0.69–0.91 s | ≤ 10 s |
| Cold build + test wall from a fresh standalone clone | 0.75–1.17 s | (no target; recorded for scale) |
| No-op re-run wall, outer checkout | 0.06–1.17 s | — |

The libraries are still skeletons (1–11 tests each, no third-party
dependencies), so these are floor values: they show the structure costs
nothing, not what a filled-in library will cost. The composition root's
own suite is the comparison point (§5.5).

## 5. Isolation evidence

### 5.1 Two checkouts, two Cargo facts (measured)

The outer gwz-dev checkout is a Cargo workspace (`gwz-dev/Cargo.toml`,
members `gwz-core` and `gwz-cli`); every library under `gwz-core/crates/`
is a path dependency of `gwz-core` and therefore an automatic workspace
member there. A standalone gwz-core checkout is **not** a workspace root
(`gwz-core/Cargo.toml` has no `[workspace]` table, and adding one is
refused by the outer workspace: "multiple workspace roots found in the same
workspace", verified in a scratch replica). Two consequences, both verified
on the standalone clone at `06d7a00`:

- `cargo test -p <name> --lib` works from the outer checkout for all 13
  libraries. From the standalone checkout it works only for the six
  libraries **without** `[dev-dependencies]` (copy-contract,
  repo-contract, family-model, family-store-contract, work-detector,
  local-import); for the seven with dev-dependencies cargo refuses:
  `package `gwz-refcopy` cannot be tested because it requires
  dev-dependencies and is not a member of the workspace`. Features cannot
  be selected for non-members either (`cannot specify features for
  packages outside of workspace`), which is why every contract crate now
  compiles its conformance module under `cfg(any(test, feature =
  "contract-tests"))` and no Tier A command needs a feature flag.
- `cargo test --manifest-path crates/<dir>/Cargo.toml --lib` (equivalently
  `cd crates/<dir> && cargo test --lib`) works for all 13 from **both**
  checkouts: through the outer workspace when run under gwz-dev, and as the
  library's own implicit root when run standalone (where it generates an
  untracked per-crate `Cargo.lock` and `target/`, now git-ignored). The CI
  job uses this form without `--locked`.

So the boundary document's exact command holds in the outer checkout and
in the standalone checkout for the dependency-free libraries; the
manifest-path form is the one that holds everywhere. The principled fix —
making gwz-core a nested workspace root that the outer workspace excludes,
the pattern the root already uses for `taut-shape-rs` — needs the root
repository's `Cargo.toml` to change and Bazel's `MODULE.bazel` manifests
re-verified, so it is an operator decision (§7 item 6), not something this
lane did on its own.

### 5.2 Unrelated sibling compile error (boundaries §4)

In the fresh standalone clone, `crates/refcopy/src/lib.rs` was given a
syntax error (`fn deliberately_broken( { this is not rust`), then:

| Command (manifest form) | Result |
|---|---|
| gwz-history-check `--lib` | PASS, 1 test (closure: repo-contract only; family-model and refcopy are both outside it) |
| gwz-workspace-install `--lib` | PASS, 2 tests (depends on copy-**contract**, not on the refcopy implementation) |
| gwz-family-model `--lib` | PASS, 11 tests |
| gwz-refcopy `--lib` | FAIL: `error: this file contains an unclosed delimiter` |
| `cargo build -p gwz-core` | FAIL (the composition root depends on every library) |

The tree was restored and `git status` was clean afterwards. This is the
boundaries §4 property in action: a broken implementation crate stops its
own Tier A command and the composition root, and nothing else.

### 5.4 Outer checkout, warm edit loop (`cargo test -p <name> --lib` from the gwz-dev root)

Same host and toolchain, after the workspace's incremental caches had been
deleted (so the first no-op run of each library rebuilt what it needed).
The "warm edit" column is the measured time from appending one comment
line to the library's `src/lib.rs` through `cargo test -p <name> --lib`
completing, with the edit reverted afterwards:

| Library | No-op run | Warm edit -> build -> test |
|---|---|---|
| gwz-copy-contract | 0.06 s | 0.89 s |
| gwz-repo-contract | 0.06 s | 0.91 s |
| gwz-family-model | 0.91 s | 0.77 s |
| gwz-family-store-contract | 1.17 s | 0.78 s |
| gwz-refcopy | 0.96 s | 0.71 s |
| gwz-repo-inspect | 1.03 s | 0.70 s |
| gwz-work-detector | 0.87 s | 0.72 s |
| gwz-history-check | 0.73 s | 0.72 s |
| gwz-family-store | 1.01 s | 0.69 s |
| gwz-local-import | 0.90 s | 0.73 s |
| gwz-workspace-install | 0.77 s | 0.71 s |
| gwz-repo-factory | 0.75 s | 0.76 s |
| gwz-local-disposal | 1.00 s | 0.75 s |

Every library's edit loop is under one second in both checkouts. The
composition root's own suite is the comparison point (§5.5).

### 5.5 Composition-root suite, once

`cargo test -p gwz-core --no-fail-fast` from the gwz-dev root, at
`e169877`, on the same host: lib binary 1743 passed / 0 failed / 1 ignored
in 776 s of harness time; the integration binaries (1, 10, 2, 36, 9 tests)
and doc-tests all passed; 13 min 24 s wall for the whole command. That is
the loop a change to the composition root pays for; a change to a library
pays about a second (§5.3, §5.4). The one `error: could not apply ...`
line in the captured output is a test's expected git stderr, not a
failure (0 failed).

The lib remainder of the fault battery moves with this lane's 15 new
`#[test]` rows (13 under `local_clone::`, one in
`workspace_ops::merge::validate::tests`, one in `workspace_ops::tests::g01`;
none cfg-gated): re-pinned in `run_r4bg_aggregate_gates.py` with the
measured counts and the usual provenance paragraph (§3 table).

### 5.3 Standalone Tier A run (fresh clone at `e169877`, cold)

`git clone gwz-core` into a scratch directory, no `target/` anywhere, then
`cargo test --manifest-path crates/<dir>/Cargo.toml --lib` for each of the
13 libraries in the same order as §2 (host: Darwin 25.6.0 arm64, cargo
1.95.0). Every library compiled from nothing and ran its tests in about a
second of wall time:

| Library | Tests | Wall (cold, from a fresh clone) |
|---|---|---|
| gwz-copy-contract | 5 | 0.80 s |
| gwz-repo-contract | 7 | 0.80 s |
| gwz-family-model | 11 | 0.79 s |
| gwz-family-store-contract | 4 | 1.03 s |
| gwz-refcopy | 1 | 0.79 s |
| gwz-repo-inspect | 1 | 0.88 s |
| gwz-work-detector | 2 | 0.75 s |
| gwz-history-check | 1 | 0.80 s |
| gwz-family-store | 1 | 1.05 s |
| gwz-local-import | 4 | 0.93 s |
| gwz-workspace-install | 2 | 1.08 s |
| gwz-repo-factory | 2 | 0.88 s |
| gwz-local-disposal | 3 | 1.17 s |

The `-p` form in the same clone: the six dependency-free libraries pass
with identical test counts; the other seven are refused with the
non-member dev-dependency error quoted in §5.1 (matrix recorded in the
lane log).

## 6. Stubbed, refusing, deferred

- Every local-family operation refuses `unsupported_operation` after shape
  validation and the family observation; nothing creates a family file.
- `gwz-family-store` refuses `Unimplemented` for every operation (lane S).
- `gwz-refcopy::SystemTreeCopier::copy_tree` refuses `Unimplemented` and
  writes nothing (lane R); `probe_native` reports `Unknown`.
- `gwz-repo-inspect` refuses/`Unknown` for every observation (lane I).
- `gwz-work-detector::classify_work` returns `Unknown` (lane W);
  `gwz-history-check::check_history` returns `Unknown` without a read (lane H).
- `gwz-local-import::prepare_import` refuses `Unimplemented` before any
  transport call; `push_local` refuses every item (lane X).
- `gwz-workspace-install::install`, `gwz-repo-factory::construct`,
  `gwz-local-disposal::dispose` refuse before any port/session call
  (lanes N, B, D).
- The family-merge wrapper stops at the family observation; resolution,
  locked import and engine delegation are documented in
  `family_merge.rs` and land with S and X.
- gwz-py: no `cli_local_family.py` (lane CP); gwz-cli: no `--remote`/
  `--local` parsing (lane CR).
- Bazel: `MODULE.bazel` lists three manifests; crate_universe splices path
  dependencies per `gwz-core/BUILD.bazel`'s taut-shape note, so no BUILD
  files were added for the crates. **Not verified** (no bazel run); listed
  as an open item.
- **Hook-path preflight tests (plan §3 "1.0c", third paragraph) — DEFERRED to
  lane I, recorded here (LCM1.0c-rem1, Code P3-5).** The skeleton carries only
  the vocabulary (`gwz-repo-contract`'s `LayoutHazard::{EscapingConfig,
  UnresolvableConfig}`; `gwz-repo-inspect` refuses `Unimplemented`); no
  preflight and no test exist yet. Lane I owns "test valid internal and
  escaping relative hook paths at source preflight before reservation," using
  lane T's fixtures (`crates/local-testrepo`), and must pin one `LayoutHazard`
  outcome per fixture:
    - an escaping relative hook path (`../…` out of the repo) ⇒ `EscapingConfig`;
    - a valid internal relative hook path ⇒ admitted layout (no hazard);
    - configuration that cannot be resolved (missing/unreadable hook config,
      or a `core.hooksPath` that does not resolve) ⇒ `UnresolvableConfig`;
    - each of the above additionally for a **bare** repository and for a
      **push-hook working directory**, so the working-directory variants are
      not lost between this checkpoint and LCM1.1.
  These fixtures are lane I's TDD start; see the §8 lane I brief.

## 7. Open questions for the operator (L1-28)

1. **`CloneLocalWorkspaceRequest` tag 6 wire name.** Design §7 names it
   `from`, a keyword in both generated languages (Rust and Python); tautc
   does not escape keywords. Tag 6 is held unallocated. Options:
   `from_source`, `source`, `from_member`. Needed before LCM3.2, not W1.
2. **`LocalFamilyResponse` list payload.** Design §8.1 shows the `gwz local
   list` output but §7 allocates only the envelope. Proposal:
   `members = F(2, List(LocalFamilyMemberEntry))` with `name`, `kind`,
   `recorded_state`, `observed_state`, `path`, `last_error`, mirroring
   `gwz_family_model::ListRow`. Needed before LCM1.1c.
3. **`UnknownLocal` error code.** The design's family-only merge refusal has
   no `GwzErrorCode`; the catalog ends at 61. Proposal: `unknown_local=62`,
   or map to an existing code (`missing_remote` would blur the design's
   distinction). Needed before LCM1.2.
4. **Engine guard placement.** The three-line `local_source_name` refusal in
   `merge/validate.rs` is the compiled form of "the inner engine must not
   resolve the family name"; confirm it may stay (it changes no existing
   behavior; the field did not exist before).
5. **Push into checkout members.** libgit2's local transport refuses every
   push into a non-bare repository (observed, §5). Lane X's family push
   (LCM2.4) therefore reaches bare hubs only through `push_anonymous`; a
   checkout member must be published through the receiver-side
   `fetch_anonymous`. Confirm this is acceptable under design §6.1/§8.2 or
   narrow §8.2's example.

6. **Workspace layout for the standalone core checkout.** Option A: add
   `[workspace] members = ["crates/*"]` to `gwz-core/Cargo.toml` and
   `exclude = ["gwz-core"]` in the root `gwz-dev/Cargo.toml` (gwz-cli keeps
   its path dependency; `cargo test -p gwz-core` still works from the root
   because gwz-core has no dev-dependencies; library Tier A commands run
   from `gwz-core/`; Bazel `MODULE.bazel` manifests must be re-verified).
   Option B (landed): keep the layout and use the manifest-path command in
   CI and standalone. Recommendation: A, once the root manifest can be
   committed together with the gwz-core change.
   **S-P3-3 retirement condition (LCM1.0c-rem1).** Until this is resolved, the
   local-clone boundary gate refuses any classified crate that *declares* a
   third-party dependency while the CI Tier A step runs unlocked (the
   `--locked`-less `--manifest-path` loop). That guard retires when either
   Option A lands (gwz-core its own workspace, excluded from the root as
   `taut-shape-rs` is) or per-crate `Cargo.lock` files are committed and
   `--locked` is restored on the CI Tier A step — at which point lane I may
   add `git2` to `gwz-repo-inspect`.
7. **Root `Cargo.lock` (gwz-dev, C-owned).** Resolving the 13 new path
   packages added their entries to the root workspace lock (104 lines,
   nothing else changes). It is left uncommitted in the root working tree;
   it must land with the gwz-core submodule bump, or a `--locked` build
   from the root fails.
8. **Fault-battery darwin base was one high.** `--list` on a worktree at
   87207c2 lists the lib remainder at 1004 including the ignored row, i.e.
   1003 passed on darwin, while 87207c2 had set darwin to the linux
   measurement (1004). The per-OS one-row offset in that partition is
   therefore still real; 52468ca pins 1018 darwin (measured) / 1019 linux
   (derived), and the landing dispatch's ubuntu leg should confirm 1019.

## 8. What the other lanes need next

- **T (local-testrepo):** create `crates/local-testrepo` with the exact
  inventory entry (role harness, `first_party: ["gwz-repo-contract"]`,
  `third_party: ["git2", "tempfile"]`, dev-only). The gate already
  classifies it as pending; flip `expected` to `present` through C. Use
  `gwz_repo_contract::contract_tests::GraphFixture` as the shape the real
  reader conformance expects.
- **1.0a (R):** implement `SystemTreeCopier` against
  `gwz_copy_contract::contract_tests::run_all` (currently refusing); native
  smoke tests must assert `NativeMechanism`/warnings show the native path
  ran; keep forced-ordinary cases in Tier A.
- **1.0b (T, C):** the real port is `GitBackend::fetch_anonymous` /
  `push_anonymous` and the adapter is `local_clone::transport::
  BackendLocalTransport`; `local_clone::tests::transport` already imports
  under `refs/gwz/local-imports/t1` and pushes into a bare hub. The
  multi-repository fixture check, OID vector verification and the
  engine-entry composition (conflict/continue/abort, explicit root) are the
  remaining 1.0b work, using T's fixtures.
- **I (repo-inspect):** own the hook-path preflight tests deferred in §6
  (LCM1.0c-rem1, Code P3-5). Start from lane T's `crates/local-testrepo`
  fixtures and drive `gwz-repo-inspect`'s layout inspection to the
  `LayoutHazard` outcome each fixture requires — `EscapingConfig` for an
  escaping relative hook path, an admitted layout for a valid internal
  relative one, `UnresolvableConfig` for unresolvable hook configuration —
  each also for a bare repository and a push-hook working directory. The
  skeleton refuses `Unimplemented` today; the failing preflight test is lane
  I's TDD start.
- **S (family-store) — first failing test:** wire
  `gwz_family_store_contract::contract_tests::run_all` through a real
  temp-directory `StoreFixture` for `YamlFamilyStore`. As of LCM1.0c-rem1
  `run_all` covers the pointer/marker half (marker-before-pointer ordering,
  `StoreError::Partial { completed: [MarkerWritten] }` on a scripted pointer
  failure via the new `StoreFixture::fail_next` hook, repeatable
  `remove_pointer`, `ConflictingMetadata`, `PointerTargetInvalid`, and the
  `removing_the_row_before_the_pointer_is_refused_and_leaves_no_orphan`
  ordering case), all green against `InMemoryFamilyStore`. Lane S's real-store
  `run_all` is expected RED at this checkpoint (the store refuses
  `Unimplemented`) and is lane S's first failing test; a filesystem fixture
  implements `fail_next` by making the target path unwritable rather than
  queuing a scripted failure.
- **1.0d reviewer:** review gwz-core `7faf475..52468ca` (six commits, tip
  `52468ca`), `86840f6` (gwz-cli) and `afcd5a3` (gwz-py) with this document;
  re-run §3; examine §5's isolation experiment; check the contracts against
  boundaries §3 line by line; decide §7 items 4–6 or route them to the
  operator, and hand items 7 and 8 to the operator as they stand. For the
  round-1 remediation, see the "Remediation round 1" section below and the new
  tuple it records.


## 9. Remediation round 1 (LCM1.0c-rem1)

2026-09-05. Lane C landed **one** gwz-core commit
(`a8eae4e428abe296bb105b693443ee47a9caf045`) on `main`, message `LCM1.0c-rem1: …`, resolving the
two peer-blind NO-GO verdicts' bounded findings per
`dev-docs/GwzLocalClone-LCM1.0c-RemPlan.md`. No gwz-cli or gwz-py change was
needed. No pinned compiler-root or source-digest file was touched. The record
edits above (§1, §2.5, §3, §5.2, §6, §8, this section) stay in the gwz-dev
working tree for the lane owner to commit with the root checkpoint.

**New tuple.** gwz-dev root checkpoint `fc8b189` (this record re-edited,
uncommitted); gwz-core **`a8eae4e428abe296bb105b693443ee47a9caf045`** (was `52468ca`); gwz-cli
`86840f67e23a9ef04312df0dd7c8f781cc476e04` (unchanged); gwz-py
`afcd5a396ccaf5e64884f8f1cec2f66273ac08bb` (unchanged).

**Finding → change → closure test.** IDs are `C-` = ReviewCode, `S-` = ReviewState.

| ID | What changed (file) | Closure test — result |
|---|---|---|
| **C-P2-1** | `src/git/gitbackend/transport.rs` `admitted_local_peer` canonicalises the admitted directory and hands libgit2 its `url::Url::from_file_path` (`file://…`) so the transport table matches `file://` before the `:` heuristic; caller-facing `file://` inputs still refused; `docs/GitBackend.md` updated | `local_clone::tests::transport::anonymous_ports_stay_local_for_a_peer_path_containing_a_colon` (`#[cfg(unix)]`, source+bare hub under a `:` path; fetch creates the import ref, push updates the hub, `remotes()` empty) — **PASS** |
| **S-P2-1 = C-P3-4** | `FETCH_HEAD` measured, not described: the fetch test plants a record before the fetch and asserts the observed truth; `contract.rs`, `docs/GitBackend.md`, `dev-docs/GWZDesign.md`, and the test comment aligned — libgit2 1.9.7 truncates `FETCH_HEAD` to empty on every fetch (`truncate_fetch_head`) even with `update_fetchhead(false)`, so a prior record does not survive; the file is outside the port's promise | `local_clone::tests::transport::fetch_anonymous_imports_an_explicit_refspec_without_persisting_a_remote` (asserts the planted record is truncated to empty) + the absent-before arm in `adapter_maps_every_port_method_onto_the_backend` — **PASS** |
| **S-P2-2** | `crates/family-store-contract/src/lib.rs` call-order clause replaced: pointer/marker install only after the `creating` row; pointer/marker removal strictly before `RemoveRow`/`Disband`; a pointer whose row is gone must not be produced. The reference fake now refuses `RemoveRow`/`Disband` that would strand a pointer (new additive `StoreError::PointerStillInstalled`) | `contract_tests::run_all` case `removing_the_row_before_the_pointer_is_refused_and_leaves_no_orphan` (the orphaning order is refused; the required order leaves no orphan) — **PASS** |
| **S-P3-1 = C-P3-3** | `StoreFixture::fail_next(&mut self, root, StoreOperation)` added; `run_all` extended with the pointer half: marker-before-pointer ordering, `Partial { completed: [MarkerWritten] }` on a scripted pointer failure, repeatable `remove_pointer`, `ConflictingMetadata`, `PointerTargetInvalid` | `cargo test -p gwz-family-store-contract --lib` (run_all via `in_memory_store_satisfies_the_conformance_suite`) — **PASS**; the real-store `run_all` wiring is lane S's first failing test (§8) |
| **C-P3-2** | `crates/copy-contract/src/contract_tests.rs`: countdown-`Cancellation` case with a non-empty partial, `#[cfg(unix)]` read-only-destination `DestinationUnwritable` case; both assert `partial` counts equal the destination's actual contents and the source is untouched; in-crate negative unit test rejects a lying fake | `cargo test -p gwz-copy-contract --lib` (`ordinary_copier_satisfies_the_conformance_suite`, `the_accuracy_check_rejects_a_copier_that_lies_about_its_partial_report`) — **PASS** |
| **C-P3-1** | `src/workspace_ops/merge/validate.rs` exposes the engine start gate as `validate_merge_start_shape` + `MergeRequest::validate_merge_start_shape` (inherent method, so no re-export through the pinned `merge/mod.rs`); `validate_family_merge` runs it on the projected request (selector cleared, placeholder import ref) before returning the selector | `local_clone::request::tests::family_merge_shape_runs_the_engine_start_gate_before_any_import`, `local_clone::tests::request::a_malformed_family_start_is_refused_with_the_engine_code_before_any_import` (whitespace message ⇒ `merge_validation_failed`, no `refs/gwz/local-imports/*`), `workspace_ops::merge::validate::tests::merge_start_shape_is_the_engine_start_gate_and_start_only` — **PASS** |
| **S-P3-2** | `scripts/checks/test_v1_lifecycle_privacy_probe.py` and `scripts/checks/test_check_checked_artifact_boundaries.py` copy `crates/` through `copy_probe_dir` (`ignore=shutil.ignore_patterns("target", "Cargo.lock")`); unit test over the helper in each | `…::test_probe_copy_omits_git_ignored_build_output` (both files) — **PASS** |
| **S-P3-3** | `scripts/checks/check_local_clone_boundaries.py` refuses when any classified crate manifest declares a third-party dependency while the CI Tier A step is unlocked (workflow `--locked`-less `cargo test --manifest-path` loop detected, or an explicit `ci_tier_a_unlocked` inventory flag); retirement condition in §7 item 6 | `scripts/checks/test_check_local_clone_boundaries.py::…::test_declared_third_party_with_unlocked_tier_a_is_rejected` (fails unlocked, passes locked); current tree passes — **PASS** (18 tests) |
| **C-P3-5** | This record: §6 hook-path preflight deferral naming lane I, lane T's fixtures and the `LayoutHazard` outcome per fixture; §8 lane I brief | the deferral text and the §8 brief above |
| **Optional ride-along** | `.github/workflows/checked-artifact-boundary.yml`: `--component clippy` on the local-clone-boundary job and `cargo clippy --manifest-path … --all-targets -- -D warnings` in the Tier A loop | CI only; the 13 crates already clippy-clean locally |

**Gate results (measured on this commit's tree, host Darwin 25.6.0 arm64,
cargo 1.95.0, python3.13).**

| Gate | Result |
|---|---|
| `cargo fmt --all -- --check` | clean |
| `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings` (gwz-core) | clean |
| `cargo clippy --manifest-path crates/{copy-contract,family-store-contract}/Cargo.toml --all-targets -- -D warnings` | clean |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | `ok (24 visible entries, 9 classified modules)` |
| `python3.13 scripts/checks/check_local_clone_boundaries.py` | `ok` — 13 packages, 26 edges |
| `python3.13 -m unittest scripts/checks/test_check_local_clone_boundaries.py` | 18 tests OK (was 17; +S-P3-3 negative fixture) |
| `python3.13 protocol/regen.py --check` | `OK -- committed protocol artifacts are current` |
| `cargo test -p gwz-core --lib local_clone` | 16 passed (was 13; +3) |
| `cargo test -p gwz-core --lib workspace_ops::merge::validate` | 7 passed (was 6; +1) |
| `cargo test -p gwz-core --lib workspace_ops::tests::g01::tracking_backend` | 1 passed |
| `cargo test -p gwz-copy-contract --lib` | 6 passed (was 5; +1 negative) |
| `cargo test -p gwz-family-store-contract --lib` | 4 passed (run_all now covers the pointer half) |
| `cargo test -p gwz-family-store --lib` | 1 passed (compiles against the new `StoreError` variant) |
| `run_r4bg_aggregate_gates.py fault:4` (lib remainder) | ok `1022 passed` |
| `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 87207c2 HEAD` | `lane gate: ok` at every commit |

**Remainder re-pin (same commit).** The lane adds four `#[test]` rows to the
lib remainder partition and removes none — three under `local_clone::` (the
C-P2-1 colon case `#[cfg(unix)]`, the C-P3-1 unit and Tier B rows) and one in
`workspace_ops::merge::validate::tests` — so
`scripts/checks/run_r4bg_aggregate_gates.py` moves lib remainder **darwin
1018 → 1022 (measured), linux 1019 → 1023 (derived, +4, every row cfg-free on
a unix host)**; `checked_artifact::` 459 and `v1_lifecycle::` 266 are unmoved.
The copy-contract and family-store-contract suite additions are in their own
crates and do not touch the gwz-core lib census.
