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
> **Follow-up 1 (`LCM1.0c-fu1`, §10, 2026-09-05):** the four round-2 P3s and
> the two Code round-2 residuals landed as one gwz-core commit `79c55667472503c66cbd45e2a4d9e7fce9919cdb` on
> `a8eae4e`; lane S starts on the corrected `family-store-contract` (§8).


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
  temp-directory `StoreFixture` for `YamlFamilyStore`. As of LCM1.0c-fu1
  (§10) `run_all` has 15 cases, all green against `InMemoryFamilyStore`: the
  index half (6) and the pointer/marker half (9) — marker-before-pointer
  ordering, `StoreError::Partial { completed: [MarkerWritten] }` on a scripted
  pointer failure via `StoreFixture::fail_next`, repeatable `remove_pointer`,
  `ConflictingMetadata` (reached through a row recorded at `../root-two`,
  where a second family's index lives), `PointerTargetInvalid` (family two's
  own `<root-two>/../ws-A`, which already holds family one's pointer), the
  `RemoveRow` **and** `Disband` ordering refusals, `PathMismatch` for a
  destination that is not the row's path, and a non-canonical spelling of the
  row's path (`<root>/./../ws-A`) that must be accepted, must protect the row,
  and must be removable by name. What the corrected contract requires of the
  real store and its fixture:
  - `StoreFixture::member_workspace(root, relative)`: `create_dir_all` the
    join, return the join. `fresh_root` must return a root whose parent is
    fixture-private (rows are recorded at `../<name>`).
  - **One resolution.** `install_pointer` resolves `root.join(row.path)` and
    `destination` through the same canonicalisation (`std::fs::canonicalize`;
    both must exist — the orchestrator allocates the destination first, and a
    missing destination is `Io { operation: WriteMarker }`) and refuses
    `PathMismatch` when they differ, *before* reading the destination's own
    metadata (`ConflictingMetadata`, `PointerTargetInvalid`). `remove_pointer`
    and the `RemoveRow`/`Disband` guard resolve the row's path the same way; a
    recorded path that no longer resolves (directory gone) holds no pointer —
    `remove_pointer` reports nothing and the guard does not refuse.
  - Effects name `destination` as passed (install) and `root.join(row.path)`
    (remove); the suite compares them exactly.
  - The `Disband` guard is derived from the rows (a filesystem store cannot
    enumerate pointers) and names the first row, in name order, whose pointer
    stands — never a `"*"` sentinel.
  - In-crate, outside `run_all` (the fake has no notion of existence or
    symlinks): the missing-destination `Io { WriteMarker }` arm, and
    optionally a `#[cfg(unix)]` symlinked-parent spelling of the row's path.
  Lane S's real-store `run_all` is RED today (the store refuses
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

## 10. Post-acceptance follow-up 1 (LCM1.0c-fu1)

2026-09-05. Lane C landed **one** gwz-core commit (`79c55667472503c66cbd45e2a4d9e7fce9919cdb`) on `main`,
message `LCM1.0c-fu1: …`, closing the four round-2 P3 findings (Code C2-P3-1,
C2-P3-2; State S2-P3-1, S2-P3-2) and the two Code round-2 residuals
(`GwzLocalClone-LCM1.0c-ReviewCode-2.md` §3: the unpinned "updates no `origin`
tracking ref" clause, and the fake's silent `fail_next` fallback) before lane
S starts on the real store. No gwz-cli or gwz-py change; no pinned file; no
new dependency; `crates/family-model` (lane F) untouched. The gwz-core
`#[test]` census is unchanged (1748 rows: 16 `local_clone` + 1732 filtered),
so `run_r4bg_aggregate_gates.py` is not re-pinned. This record's edits (status
block, §8 lane S brief, this section) stay uncommitted in gwz-dev for the lane
owner.

**New tuple.** gwz-core **`79c55667472503c66cbd45e2a4d9e7fce9919cdb`** (was `a8eae4e`); gwz-cli
`86840f67e23a9ef04312df0dd7c8f781cc476e04` and gwz-py
`afcd5a396ccaf5e64884f8f1cec2f66273ac08bb` unchanged; gwz-dev root: this
record re-edited, uncommitted.

**Finding → change → closure test.** Paths are gwz-core; `lib.rs` and
`contract_tests.rs` are `crates/family-store-contract/src/`. RED was observed
before each implementation step: the un-resolving fake failed the repaired
`PointerTargetInvalid` case exactly as S2-P3-1 predicts, and both checker
fixtures failed `False is not true` against the per-line parser.

| ID | What changed (file:line) | Closure test — result |
|---|---|---|
| **S2-P3-1** | Contract: call-order clause gains "One resolution of the recorded path" (`lib.rs:311-323`); `install_pointer` declaration states the destination is the store's own resolution of the row's recorded path, must exist, and the refusal order (`lib.rs:344-361`); `remove_pointer` resolves the same way (`lib.rs:367-373`); `MetadataEffect` says which spelling `workspace` carries (`lib.rs:100-105`). Additive `StoreError::PathMismatch { member, recorded, requested }` (`lib.rs:165-175`, Display `:246`); `src/local_clone/errors.rs:64` maps it to `invalid_request`. Fake: one lexical `resolve` (`contract_tests.rs:612`) keys every map and serves `install_pointer` (`:987` refuses `PathMismatch` before the destination's metadata), `remove_pointer` and both guard arms through `installed_pointer_of` (`:823`). Repaired cases: `…over_another_familys_pointer_is_pointer_target_invalid` (`:242`; family two installs at its own `member_workspace(&root_two, "../ws-A")`, family one's pointer pre-planted there) and `…into_a_destination_holding_an_index_conflicts` (`:205`; the row is recorded at `../root-two`, where family two's index lives, because handing `install_pointer` the root is now a `PathMismatch`) | `run_all` cases `a_pointer_installed_through_a_non_canonical_spelling_is_the_rows_pointer` (`:386`: `root.join("./../ws-A")` accepted; `apply(RemoveRow)` refused `PointerStillInstalled { member: "A" }` while it stands; `remove_pointer` reports `PointerRemoved`; then `RemoveRow` succeeds) and `installing_a_pointer_anywhere_but_the_rows_path_is_refused_without_effects` (`:337`: `PathMismatch { member: "A" }`, `NoFamily` observed at both paths, `remove_pointer` reports nothing, the row is removable) — **PASS** |
| **C2-P3-1** | `StoreFixture::member_workspace(&mut self, root, relative) -> PathBuf` (`contract_tests.rs:35-43`; fake `:1107` = join; a filesystem fixture `create_dir_all`s then joins); free `destination_of` removed; `founded_with_a_creating_member` (`:96`) and both `root_two`s (`:222`, `:258`) route through it; `fresh_root` doc requires a fixture-private parent (`:31-34`); contract says a missing destination is `Io { operation: WriteMarker }` and the store creates nothing above `.gwz/` (`lib.rs:349-357`) | the fake's `run_all` unchanged-green (7 passed); the real closure is lane S's `run_all` with a store that creates nothing above `.gwz/` (§8) — **PASS** (fake) / lane S (real) |
| **C2-P3-2** | `run_all` case `disbanding_before_the_pointers_is_refused_and_leaves_no_orphan` (`contract_tests.rs:427`); the fake's `Disband` guard is row-derived and names the member (`:931-945`; no `"*"` sentinel); `PointerStillInstalled` doc and Display describe both refusals: "member `A` still has its clone pointer installed at …; remove the pointer before removing the row or disbanding the family" (`lib.rs:156-164`, `:240-245`) | the new case (`Disband` → `Err(PointerStillInstalled { member: "A" })`, index intact; `remove_pointer`; `Disband` → `Ok([IndexRemoved])`; `reread` → `None`) and `lib.rs:415` `ordering_and_path_refusals_name_the_member_and_the_paths` (asserts "disbanding" present, `*` absent) — **PASS** |
| **S2-P3-2** | `scripts/checks/check_local_clone_boundaries.py:277-330`: `tier_a_commands` drops comment lines and joins `\`-continuations (`:280-294`) before matching; `tier_a_unlocked` — the inventory flag wins; else every `.github/workflows/*.yml`/`*.yaml` is scanned; no workflow ⇒ unlocked iff `crates_dir` exists (`:321`); workflows but no recognisable Tier A command, or an unreadable workflow ⇒ unlocked; else unlocked iff ANY `cargo test … --manifest-path` command lacks `--locked` (`:330`) | `scripts/checks/test_check_local_clone_boundaries.py:493` `test_tier_a_command_split_across_continuations_is_unlocked_and_refused` (wrapped, `--locked`-less loop ⇒ `True`; the gate refuses the declared `tempfile` edge with "runs unlocked") and `:509` `test_locked_on_every_tier_a_command_is_locked_and_anything_less_is_not` (no workflow ⇒ `True`; no Tier A command ⇒ `True`; `--locked` on the wrapped command and on a second file's command ⇒ `False`, gate passes with the "locked Tier A step" note; one unlocked command in the second file ⇒ `True`, gate refuses) — **PASS**, 20 OK (was 18); the real tree still passes and evaluates `True`, with exactly the standalone Tier A loop line parsed |
| **Residual: tracking ref** | `src/local_clone/tests/transport.rs:46-66`: after `fetch_anonymous`, `references_glob("refs/remotes/*")` on the receiver is asserted empty | `local_clone::tests::transport::fetch_anonymous_imports_an_explicit_refspec_without_persisting_a_remote` — **PASS** |
| **Residual: `fail_next` fallback** | `contract_tests.rs:1128-1145`: an unknown root panics ("is not a root this fixture handed out") instead of scripting the last store's failure | `contract_tests::tests::fail_next_on_a_root_the_fixture_never_handed_out_panics` (`:1177`) — **PASS**; plus `the_fakes_resolution_is_lexical_and_root_bounded` (`:1159`) over `resolve` |

**Ownership note.** The brief's "additive `Refusal` arm" would live in
`crates/family-model` (lane F). It was not needed: the pure model never sees
the requested destination — the mismatch is a store decision between the
session's own resolution of the row and the caller's argument — so the typed
refusal is the contract crate's `StoreError::PathMismatch`, and no
family-model change is proposed. The suite was measured on the working tree,
which at the time carried lane F's uncommitted family-model edits; this
commit references no item they add (only `MemberRow`, `validate_transition`
and the existing constants), so it is independent of them.

**Gate results (this commit's tree, host Darwin 25.6.0 arm64, cargo 1.95.0,
python3.13; run from gwz-dev unless noted).**

| Gate | Result |
|---|---|
| `cargo fmt -p gwz-family-store-contract -- --check`, `… -p gwz-family-store …`, `… -p gwz-core …` | clean |
| `cargo clippy -p gwz-family-store-contract --all-targets -- -D warnings`, `… -p gwz-family-store …`, `… -p gwz-core …` | clean |
| `cargo test -p gwz-family-store-contract --lib` | 7 passed (was 4; `run_all` 15 cases, was 12) |
| `cargo test -p gwz-family-store --lib` | 1 passed |
| `cargo test -p gwz-core --lib local_clone` | 16 passed, 1732 filtered (census 1748, unchanged) |
| `python3.13 scripts/checks/check_local_clone_boundaries.py` (gwz-core) | `ok` — 13 packages, 26 edges |
| `python3.13 -m unittest scripts/checks/test_check_local_clone_boundaries.py` | 20 OK (was 18) |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | `ok (24 visible entries, 9 classified modules)` |
| `PYTHON=python3.13 scripts/checks/check_lane_commits.sh a8eae4e HEAD` | `lane gate: ok` at all 11 post-`a8eae4e` commits (nine from the parallel lanes, lane F's `2055408` — this commit's parent — and `79c5566`) |

**Not done, and why.** The missing-destination `Io { WriteMarker }` arm and a
symlinked-parent spelling are not `run_all` cases: the in-memory fake has no
notion of directory existence or symlinks, so they are lane S in-crate tests
(§8). `docs/GitBackend.md` needed no sentence: the tracking-ref assertion
pins a clause the contract already states.

## 11. Post-acceptance follow-up 2 (LCM1.0c-fu2, operator rulings + W1 proposals)

Written by the lane owner: lane C landed `fu2a`/`fu2b`/`fu2c` and was cut off
by a usage limit before the tail; the lane owner committed the re-pin
(`fu2d`) and this section, and verified every gate below on the pinned
1.95.0 toolchain from `gwz-core/`.

**Tuple.** gwz-core `616fc18` (four commits on `0d7b53d`); gwz-cli `196f911`;
gwz-py `ad17c50`; gwz-dev `acdbcbf` plus the uncommitted root files listed
below.

| Ruling / proposal | Landed | Evidence |
|---|---|---|
| R1 `--from` wire name `copy_source` (design §11 item 11) | `fu2a` — taut tag 6, regenerated Rust + Python, core decode/refusal | `regen.py --check` OK; `local_clone` slice green |
| R2 `LocalFamilyResponse.members` (item 12) | `fu2a` — `LocalFamilyMemberEntry` mirroring `gwz_family_model::ListState`; core projects it for op=list behind the still-`Unimplemented` store | new `local_clone::list` tests |
| R3 `unknown_local = 62` (item 13) | `fu2a` — catalogs + `errors.rs` + family-merge wrapper carry the state detail | `local_clone` slice green |
| R4 workspace layout Option A (item 14) | `fu2b` — gwz-core is its own Cargo workspace (14 members); root excludes it; CI Tier A is `cargo test -p <name> --lib --locked` + locked clippy; the unlocked-Tier-A guard retired per §7.6; per-crate lock/target ignores removed | all 13 crates `--locked` green in-tree; root `cargo build -p gwz --locked` ok |
| W1, H1, H3, F1, F2, R1, R2 contract proposals | `fu2c` — all additive: `WorkObservation.unknown`, `UnknownKind::Cancelled`, a coordination-record root source, `Refusal` non-exhaustive with typed `PathNotNormalised`/`AllocationCollision`, `classify_dispose_target` at the request gate, `CopyWarningKind::NativeUnavailable`, `CopyError::refused_with` | Tier A counts below |
| H2 adapter rule (one `check_history` call per witness store) | `fu2c` — documented where the core adapter will live; no signature change | doc comment; carried to lane X/D briefs |
| Escaped defect: gwz-py stale `PRE_LOG_WIRE_SHA256` | `ad17c50` — re-pinned with the reason | gwz-py fast suites 113 passed, no red |

**Gates (lane owner, 2026-09-06, rustc 1.95.0 from `gwz-core/`).** Checked-artifact
boundary ok (24 visible entries, 9 classified modules); local-clone boundary ok;
its unittest OK; `protocol/regen.py --check` OK; `cargo fmt --all -- --check`
clean; Tier A `--locked` for all 13 crates green (copy-contract 6, family-model 29,
family-store-contract 7, family-store 1, history-check 30, local-disposal 3,
local-import 4, refcopy 35, repo-contract 7, repo-factory 2, repo-inspect 1,
work-detector 32, workspace-install 2); `check_lane_commits.sh 0d7b53d HEAD` ok at
all four commits; gwz-cli `cargo test -p gwz` 224 passed and the CLI reference
check ok; gwz-py drift `sha256:08c45973…` and 113 fast tests passed; root
`cargo build -p gwz --locked` ok with gwz-core excluded.

**Census and re-pin (`fu2d`).** `--list` 1754 rows; `checked_artifact::` 459 and
`v1_lifecycle::` 266 unmoved; lib remainder 1029 listed, two `#[ignore]` rows in
the lib (one in each of the remainder and v1_lifecycle partitions), so darwin
executes 1028 — measured directly with the two partition skips. Linux 1029 is
derived (+6 cfg-free rows) and first-dispatch-expected.

**Left uncommitted for the lane owner (root repo).** `Cargo.toml` (the
`exclude = ["gwz-core"]` and its rationale), `dev-docs/GwzLocalCloneDesign.md`
(revision 9, recording rulings 11–16), `dev-docs/GwzLocalCloneLibraryBoundaries.md`
(revision 2, §5 Tier A form and §6 gate/CI), and this section.

**Not done.** Bazel was not run in this follow-up (`bazel query //...` remains the
named command); the probe harnesses were not re-run under the new workspace table
(their 16-minute suites were out of budget) — both carry into the next lane C task.

**What the next lanes need.** I (repo-inspect + the hook-path preflight), T
(`local-testrepo`), S (`YamlFamilyStore` against the corrected pointer contract) and
R (native copy path) are unblocked by Option A and may declare their third-party
dependencies. CR wires `--from` to `copy_source` and renders `local list`; CP
renders the same payload and adds the shared parity fixture.

## 12. Post-acceptance follow-up 3 (LCM1.0c-fu3, cross-driver rulings + W2 proposals)

2026-09-06, lane C. The operator's four cross-driver rulings of 2026-09-06
(core halves), the W2 lanes' contract proposals, two defects and two debts.
Four gwz-core commits on `main`, each on an explicit pathspec, each passing
the lane gate on its own tree; one compile-required commit in each driver.
The four W3 lanes (X, N, B, D) landed eight commits in the same range and
were never touched: every contract change below is additive, and their Tier
A stays green on the final tree (local-import 22, workspace-install 27,
repo-factory 21, local-disposal 27).

**Tuple.** gwz-core **`96226c9579ba64e47d216e9d1408f724aa4cd89d`** (lane C:
`bf3fc35` fu3a, `205ac09` fu3b, `8652b87` fu3c, `96226c9` fu3d, after the
W3 commits `ce54ba8`..`1659e0c` on `7e962d2`); gwz-cli
**`4c5d7fd6a2dab1d43a966eacc6837962c080e115`** (one literal); gwz-py
**`30885cd1f165427a5d6b04a08c95cbcded0e3860`** (regenerated protocol, two
pins, two literals); gwz-dev root `0bd0d9f` plus the uncommitted files
listed at the end.

### 12.1 The four rulings (core halves)

| Ruling | Decision | Change (file:line, gwz-core unless noted) | Test / evidence | Drivers' half |
|---|---|---|---|---|
| 1. One parity fixture in gwz-core | Landed as the single source: `protocol/fixtures/cli_parity/local_family_cases.json` (fu3b), merged from gwz-py's `parser_cases.json` message rows (30 + 15) and gwz-cli's `local_family_cases.json` (24 + 17) into **30 message cases, 23 refusals**, `_schema`-documented (dotted `fields` paths; `message_contains` shared needles plus `driver_message_contains` per driver; `drivers` scoping). Disagreements decided by design §7: the two push rows carry `meta.policy.remote: null`; `clone-local-empty-name` is both drivers'. Folded: gwz-cli's `--force wat` into `local-dispose-force-unknown-name`; gwz-py's python-only push row into `push-remote-family-hub`. One row scoped `rust`: `local-dispose-hazards-without-force` (argparse rejects the operand before any handler, so gwz-py's harness sees no usage error). | the fixture's `_schema.location` names each driver's relative path | Verified without editing a driver: gwz-py's own harness against the file — 30 cases, 22 refusals, 0 failures; the built `gwz` binary against every rust-scoped refusal — all pass except `clone-local-empty-name`, where the Rust CLI still encodes and core answers `InvalidRequest: … a clone name must not be empty` (exactly the ruling-4 change) | CR: re-point `src/tests/g12.rs` at `../../../gwz-core/protocol/fixtures/cli_parity/local_family_cases.json` (the `include_str!` form; g02 already reaches the sibling for `merge_response.json`), resolve dotted `fields` paths, read `driver_message_contains.rust`, honour `drivers`, delete `tests/fixtures/cli_parity/local_family_cases.json`. CP: point `PARITY_FIXTURE` at `Path(__file__).resolve().parents[3] / "gwz-core/protocol/fixtures/cli_parity/local_family_cases.json`, add the `driver_message_contains.python` needles, drop the `message_cases`/`message_refusals` blocks from `parser_cases.json` (its argv lists stay). Values that changed for CR: push rows' policy field; the empty-name refusal. |
| 2. snake_case enum values in machine output | Not decided anywhere in gwz-core: the generated Rust enums (`src/protocol/generated.rs`, `LocalMemberKind`/`LocalMemberState`/`LocalObservedState`) carry `wire()`/`from_wire()` and derive `Debug` only — no `Display`, no serde; the Python enums' member names are already the snake_case wire names. The spelling is purely the Rust CLI's `format!("{:?}")` in `gwz-cli/src/local_list_render.rs:116-118`. Recorded in design §11 item 18. No core change. | — | — | CR: render `kind`/`recorded_state`/`observed_state` through the crate's existing `member_kind_word`/`recorded_state_word`/`observed_state_word` (`local_list_render.rs:14-40`) in the JSON/JSONL path too. CP: nothing (`cli_render.py:97` already emits `value.name`). Note for the lane owner: the same `{:?}` PascalCase spelling is the pre-existing convention for `merge_render.rs` and the `merge_response.json` fixture (`"Merged"`, `"GitCommandFailed"`), while gwz-py renders `merged`; that wider divergence is outside this ruling. |
| 3. `LocalFamilyResponse.root_path` | Allocated: `protocol/gwz.taut.py:1956` `root_path=F(3, STR, optional=True)` — the observed root (index directory, reached through the pointer from a clone), present exactly when `members` is; regenerated `src/protocol/generated.rs`, `src/cbor.rs` (unchanged), `protocol/corpus/*`, `docs/MessageCatalog.md`; gwz-py `src/gwz/protocol/generated/{api.py,gwz.ir.json}` regenerated. Populated: `src/local_clone/list.rs:80` `root_path(&FamilyObservation)` (spelled as `handle_ls` spells `abspath`), wired at `src/workspace_ops/handle_local.rs:111`. Docs: `docs/Protocol.md`, `docs/RustApi.md`, `dev-docs/GWZDesign.md` protocol table; design §7/§8.1/§11 item 19 (root, uncommitted). | `local_clone::list::tests::the_root_path_is_the_observed_root_and_absent_outside_a_family` (`list.rs:279`; RED first: E0425 + E0063), `local_clone::tests::request::local_family_ops_refuse_unsupported_without_writing` asserts `root_path == None` outside a family (`tests/request.rs:135`); slice 23 passed (was 22) | CR: add `"root_path"` to the JSON object in `local_list_render.rs` and join it with each member's `path` for the human table (§8.1's absolute paths); the literal in `src/tests/g12.rs:783` already carries `root_path: None` (gwz-cli `4c5d7fd`). CP: `render_family_listing` joins `response.root_path` with `entry.path`; JSON already carries the field (dataclass rendering); the two test literals carry `root_path=None` (gwz-py `30885cd`). |
| 4. `push --remote` encodes once; empty `--name` refuses at the driver | Core needs **no change**: `resolve_push_remote` (`src/workspace_ops/push_member.rs:397`) and `resolve_root_push_remote` (`:369`) read `PushRequest.remote` first and fall back to `OperationPolicy.remote` (request-over-policy, boundaries §3), so a driver that encodes the token once binds identically and an older policy-only caller still works. Empty name: `validate_clone_local` refuses `""` (and the reserved names) as `invalid_request` (`src/local_clone/request.rs:271` pins it) — core's validation is sufficient; the driver refusal is the earlier answer. Recorded in design §7 and §11 item 20. | `workspace_ops::tests::g08::push_remote_binds_from_the_request_field_first_and_the_policy_second` (`g08.rs:251`; a characterisation pin, green on first run: both set → request wins; each alone binds; neither → member's push remote / root `missing_remote`) | CR: `src/clirequest/invocation.rs:202` must build the push `meta` without `policy.remote` (the token goes to `PushRequest.remote` only); `src/clirequest/workspace.rs` `local_request` must refuse `--name ""` before encoding with a message naming `--name`. CP: already conforms (`cli_mutation.py:207` moves the token; `cli_local_family.py:185` refuses the empty name) — only the fixture re-point. |

**Census and re-pin (fu3a).** `--list` 1756 rows (was 1754); `checked_artifact::` 459 and `v1_lifecycle::` 266 unmoved; lib remainder 1031 listed, one `#[ignore]` row, so darwin executes **1030** — MEASURED (`cargo test -p gwz-core --lib --locked -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::` → `1030 passed; 1 ignored`, 55.9 s); linux **1031** DERIVED (+2, both rows cfg-free), first-dispatch-expected. `scripts/checks/run_r4bg_aggregate_gates.py:758` moved 1028/1029 → 1030/1031 with the dated reason.

**Pins moved (ruling 3), one fingerprint of one schema.** `gwz-core/protocol/check_log_additive.py:73`, `gwz-py/scripts/check_protocol_drift.py`, `gwz-py/src/tests/test_log_protocol.py`: `26f0d16ffebdcdc26bbbe682a6347688781cd202694333dbb0c066d087fb6b4e` → `2eca6469ed1281e77a95f1e419aa4065002aa94c77507a73ada6f6f9c8bb5503`, MEASURED additive: the pre-log projection rendered on `7e962d2` and on the edited schema and diffed — 11 added lines, 0 removed, 1 hunk, the one `root_path` field object; the old pin reproduced on the old tree. gwz-py packaged-IR drift check `sha256:180c224e…` OK; `regen_protocol.py --check` OK.

### 12.2 The W2 proposals

All landed in fu3c (`8652b87`), additively — new fields with defaults, new items, defaulted fixture methods, doc pins; no signature of an existing item changed. Paths are `crates/<crate>/src/`.

| ID | Decision | Change (file:line) | Test |
|---|---|---|---|
| I-1 | Pinned: `RepositoryInfo::{path,git_dir,common_dir}` are resolved (`canonicalize`) paths; `HeadState::{Attached,Unborn}::branch` is the full ref name | repo-contract `lib.rs:182` (`HeadState` doc), `:190` (field docs); the conformance suite asserts all three absolute and the `refs/` prefix in `observations_are_repeatable` (`contract_tests.rs:393`) — additive because lane I's inspector already conforms (`repo-inspect/src/lib.rs` "Spellings this crate fixes") | repo-inspect Tier A 88 passed through the suite; the scripted test now uses an absolute temp path (`/repo` is relative on Windows) |
| I-2 | Accepted: `ProtectedRoots.unknown: Vec<UnknownReason>` + `is_complete()`; **consumer obligation recorded** — a non-empty `unknown` is an incomplete inventory and must refuse like `Observation::Unknown`; lane I switches to per-root reasons only after lanes H and D honour it | repo-contract `lib.rs:470` (field + doc), `:475` (`is_complete`). Compelled literals: history-check `tests.rs:42`, repo-inspect `lib.rs:262` (`normalise_roots`, still `Vec::new()` — `history.rs` keeps whole-observation `Unknown`), local-testrepo `repo.rs:779` | `repo_contract::tests::protected_roots_are_complete_only_when_nothing_is_unknown` (`lib.rs:734`); every consumer crate's Tier A green |
| I-3 | Recorded, not aggregated: `ReadFailed` reports the read failure alone; hazards observed before it are not carried (it is not a layout verdict; the caller refuses either way and the next successful inspection reports them) | repo-contract `lib.rs:249` doc | doc pin; no behaviour change |
| I-4 | Recorded obligation: `observe_work`/`inventory_history` gain no `ReadLimits`/cancellation now — a defaulted method would be a permissive default (boundaries §3); LCM2's work-loss scan (lane D) is the first consumer and the trait gains a bounded method through C then | repo-contract `lib.rs:660` (`RepoInspector` doc) | — |
| T-1 | **Ruled**: an annotated tag is emitted **once**, as `RootSource::AnnotatedTag { name: "refs/tags/<t>" }` at the tag object's id, never also as `Ref` (the target is an edge of that object); a lightweight tag stays `Ref`. Lane I already conforms; lane T's fixture did not | repo-contract `lib.rs:516` (`AnnotatedTag` doc), `:505` (`Ref` doc); `contract_tests.rs:160` `GraphFixture::tagged`. **Compelled**: local-testrepo `repo.rs:777` `protected_roots` emits `AnnotatedTag` for a tag object; its test expectation `tests.rs:334` follows | `the_tagged_fixture_reports_the_annotated_tag_once_at_the_tag_object` (`contract_tests.rs:483`, both formats), `#[should_panic]` `a_reader_spelling_the_annotated_tag_as_a_ref_fails_exact_equality` (`:514`); local-testrepo Tier A 39 passed |
| T-2 | Accepted as an allowance, not a widening of exact equality: `RootSourceKind` + `RootSource::kind()`; `object_reader_conformance_allowing(reader, fixture, &[kinds])` and `reports_the_fixture_roots_allowing` — every fixture root exactly, `unknown` empty, extra roots only of allowed kinds; `object_reader_conformance` stays exact (`GraphFixture` unchanged, so no literal breaks) | repo-contract `lib.rs:548`, `:559`; `contract_tests.rs:189`, `:209` | `root_source_kinds_name_every_source` (`lib.rs:752`), `an_extra_root_of_an_allowed_kind_passes_the_allowing_comparison` (`contract_tests.rs:533`), `#[should_panic]` `an_extra_root_of_another_kind_fails_the_allowing_comparison` (`:553`); repo-contract Tier A 13 passed (was 7) |
| T-3 | Pinned the libgit2 `RepositoryState` → `NativeOperation` table (`Rebase`/`RebaseInteractive`/`RebaseMerge` → `Rebase`; `ApplyMailbox`/`ApplyMailboxOrRebase` → `ApplyMailbox`, i.e. `rebase-apply/` without a marker is `ApplyMailbox`, not `Other`; `Other` reserved, libgit2 reports none) — lane I's `work.rs:99` mapping, now the contract's | repo-contract `lib.rs:442` doc | local-testrepo's own mapping test (`tests.rs:755`) agrees |
| S-1 | Accepted as a hook: `StoreFixture::clear_failures(root)` (defaulted no-op), called by the suite after the observed `Partial` and before the retry; a durable obstruction clears itself there and needs no bracketing wrapper. Lane S's `BracketedStore` keeps working (it clears itself) and may be retired | family-store-contract `contract_tests.rs:64` (trait), `:207` (suite call); in-memory fixture counts calls | `in_memory_store_satisfies_the_conformance_suite` asserts `clear_failures_calls == 1` (`:1314`); real store `run_all` 38 passed |
| S-2 | Pinned (lane S's implementation): the destination-metadata step reads regular files only; a non-regular node at the pointer/marker path is neither `ConflictingMetadata` nor `PointerTargetInvalid` — the following write replaces it as a node (symlink, never followed) or fails on it (`Io`/`Partial`, directory); a non-regular node at the *index* path refuses `Malformed` before any effect | family-store-contract `lib.rs:380` | doc pin; lane S's in-crate tests cover the real store |
| S-3 | Pinned: a pointer that does not decode or names another family is nobody's pointer — neither protects the row nor is removed; retained and listed; a corrupt pointer cannot brick `dispose --keep` | family-store-contract `lib.rs:323` (call-order clause), `:404` (`remove_pointer` doc) | doc pin; lane S's `standing_pointer`/`pointer_of` implement it |
| S-4 | **Accepted, and it changes the frozen refusal order**: `Refused(Refusal::PathCollision { path: <this row's recorded path>, holder: <other member> })` — the model's existing variant, no new `StoreError` — after `PathMismatch` and before the destination's own metadata; row-derived and **symmetric** (while both rows stand, an install for either refuses), never blocking `remove_pointer`. Reachable only through a spelling the model's lexical normalisation cannot see (symlink, case folding), so `StoreFixture::alias_workspace` (defaulted `None`) lets a fixture alias a directory; the in-memory fake gained an alias table (`key()`), so the case runs against the reference fixture | family-store-contract `lib.rs:364` (refusal order), `contract_tests.rs:76` (hook), `:781` (fake `key`), `:1107` (fake check), `:219` (case, last in `run_all`) | the `run_all` case `installing_a_pointer_where_another_rows_path_resolves_is_a_path_collision` (RED first: my first ordering hit the symmetric refusal, `Refused(PathCollision { path: "../ws-A", holder: "B" })`, which is the rule working); `alias_calls == 1` proves it ran. The real store's fixture returns `None` today, so the case is **skipped for lane S until it implements `alias_workspace` with a symlink** |
| R-1 | Reworded: "not guaranteed either way" (native clones may carry ACLs/xattrs/timestamps) | copy-contract `lib.rs:178` | doc |
| R-2 | Reworded: "one destination-side handle" (file or directory) | copy-contract `lib.rs:274` | doc |
| R-3 | Stated: a natively cloned file is a single work unit; `Auto` has no mid-file cancellation point | copy-contract `lib.rs:45` (`Cancellation` doc) | doc |
| W-1 | Confirmed closed: lane I sets `observation.unknown` (`repo-inspect/src/work.rs:95`); work-detector consumes it (`lib.rs:274`) | — | work-detector Tier A 32 passed |
| W-2 | Deferred: no consumer composes layout hazards into `GwzEvidence` yet (layout hazards refuse at the layout port); a typed channel is an additive `GwzEvidence` field for lane W when lane D's disposal needs it (LCM2). `EvidenceItem { kind, state }` stays the interim | — | — |
| H-1/H-3 | Decided: the seam **stays the adapter's** — `LocalRepoInspector::with_coordination_roots([CoordinationRoot { record, object, oid }])` is the inspector's constructor, not a port method; `RootSource::CoordinationRecord` is the whole contract surface; reported from `inventory_history`, excluded from `retained_roots` | repo-contract `lib.rs:535` doc | history-check Tier A 30 passed |

### 12.3 Defects and debts

- **Windows-target clippy (pre-existing).** Reproduced: `CopyError` is exactly 128 bytes on `x86_64-pc-windows-msvc` (`PathBuf` is 32 bytes there). Fixed by a justified `#[allow(clippy::result_large_err)]` at `TreeCopier::copy_tree` (copy-contract `lib.rs:287`; boxing `partial` would change a public field every consumer reads). The same foreign-target run then fired at five of refcopy's own `Result<_, CopyError>` helpers (`ordinary.rs:63/133/164/220`, `tests.rs:955`), which the port's allow cannot reach; a crate-level allow with the same justification landed in `crates/refcopy/src/lib.rs:43` (fu3d) — **outside lane C's ownership, reported to lane R**, who may replace it. `cargo clippy -p <every cross-checkable crate> --all-targets --locked --target x86_64-pc-windows-msvc -- -D warnings`: 12 crates clean.
- **CI foreign-target step (recommended; implemented).** `.github/workflows/checked-artifact-boundary.yml:92`: after the Tier A loop, `rustup target add x86_64-pc-windows-msvc` and clippy on that target over `check_local_clone_boundaries.py --list-cross-checkable` (`:420`) — the present crates whose declared closure (own normal + dev edges, transitive normal edges, each crate's third-party set) names no native-build dependency (`git2` → `libgit2-sys` needs the target's C toolchain): today all but `gwz-repo-inspect` and `gwz-local-testrepo`. The Linux target is the runner's host, covered by the existing loop; not added as a redundant `--target`. Unit test `test_cross_checkable_list_excludes_every_native_build_closure` (`test_check_local_clone_boundaries.py:614`, real inventory + a synthetic closure); suite 23 OK (was 22).
- **Bazel (fu2b debt): FAILED, root cause the workspace table itself.** `bazel query //...` from gwz-dev (bazel 9.1.1): first blocked by a pre-existing symlink loop `/Users/owebeeone/limbo/gwz-dev/.razel-exec → … bazel-gwz-dev/.razel-exec` ("infinite symlink expansion"; a razel artifact, not the table); narrowed to `bazel query //gwz-core:all`, which fails in crate_universe: `Error: When splicing manifests, manifests are not allowed to from from different workspaces. Saw manifests which belong to the following workspaces: /Users/owebeeone/limbo/gwz-dev/gwz-core/Cargo.toml, /Users/owebeeone/limbo/gwz-dev/Cargo.toml` — `MODULE.bazel`'s `crate.from_cargo(manifests = ["//:Cargo.toml", "//gwz-core:Cargo.toml", "//gwz-cli:Cargo.toml"])` splices one Cargo workspace and Option A made gwz-core a second. `bazel build //gwz-core:gwz_core` therefore **unverified**. Cure (root files, lane owner): a second `crate.from_cargo` hub for gwz-core keyed to `//gwz-core:Cargo.lock`, or a `crate_universe` splice of the outer workspace with gwz-core as a path member again; plus `.bazelignore` (`.razel-exec`) or removal of the stale symlink so `//...` walks.
- **Probe harnesses (fu2b debt): re-run, one probe each, under the Option A table.** `python3.13 -m unittest scripts.checks.test_v1_lifecycle_privacy_probe.V1LifecyclePrivacyProbeTest.test_sealed_names_exist_and_compile_inside_the_perimeter` — OK, 29.8 s (three `cargo check --all-targets` compiles of the copied gwz-core workspace, warm target); `python3.13 -m unittest scripts.checks.test_check_checked_artifact_boundaries.CheckedArtifactBoundaryTest.test_compiler_rejects_a_redirected_v1_root` — OK, 21.7 s (one clippy compile of the copied tree plus the checker). Both copy `crates/` and the root `Cargo.lock`; the copied `gwz-core/Cargo.toml` is its own workspace, which is what the table changed. The full suites were not run.

### 12.4 Gates (this follow-up's final tree, gwz-core `96226c9`; Darwin 25.6.0 arm64, cargo 1.95.0, python3.13, from gwz-core)

| Gate | Result |
|---|---|
| `cargo fmt --all -- --check` | clean (0 diffs; formatting applied per touched crate, never `--all`, because the W3 lanes' files were uncommitted in the tree) |
| `cargo clippy -p <touched> --all-targets --locked -- -D warnings` (copy-contract, repo-contract, family-store-contract, history-check, repo-inspect, local-testrepo, refcopy, gwz-core with `--all-features`) | clean |
| Tier A `cargo test -p <name> --lib --locked`, all 14 crates | copy-contract 6, family-model 29, family-store-contract 7, family-store 38, history-check 30, local-disposal 27, local-import 22, refcopy 48, repo-contract 13, repo-factory 21, repo-inspect 88, work-detector 32, workspace-install 27, local-testrepo 39 — all passed |
| `cargo test -p gwz-core --lib --locked local_clone` / `push_remote_binds` | 23 passed / 1 passed |
| lib remainder (measured for the pin) | 1030 passed, 1 ignored |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | ok (24 visible entries, 9 classified modules) |
| `python3.13 scripts/checks/check_local_clone_boundaries.py` + `-m unittest scripts/checks/test_check_local_clone_boundaries.py` | ok; 23 OK |
| `python3.13 protocol/regen.py --check` | OK |
| foreign-target clippy loop (the new CI step, run locally) | 12 crates clean |
| `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 7e962d2 HEAD` | `lane gate: ok` at all 12 commits, 94 s |
| gwz-cli `cargo test -p gwz --locked` (root workspace) | 231 passed (166 + 26 + 6 + 29 + 4), 0 failed |
| gwz-py `python -m pytest src/tests -q` with `GWZ_RUST_BIN` | 689 passed (without the binary: 655 passed + 34 setup errors in `test_log_real_workspace.py`, the env-gated real-workspace suite) |

### 12.5 Left uncommitted for the lane owner (root repo)

`dev-docs/GwzLocalCloneDesign.md` (revision 10: status, §7 `root_path` and the push/empty-name rows, §8.1 the root_path join and snake_case note, §11 items 17–20) and this record (§12). The member pins to record through `gwz` (gwz.conf is system-managed): gwz-core `96226c9`, gwz-cli `4c5d7fd`, gwz-py `30885cd`. No root `Cargo.lock` change (no dependency moved).

### 12.6 What the next lanes must do

- **CR**: (1) re-point `src/tests/g12.rs` at `include_str!("../../../gwz-core/protocol/fixtures/cli_parity/local_family_cases.json")`, resolve dotted `fields`, read `driver_message_contains.rust` beside `message_contains`, honour `drivers`, drop `tests/fixtures/cli_parity/local_family_cases.json`; (2) `local_list_render.rs:116-118`: snake_case via the existing `*_word` functions; (3) render `root_path` in JSON and join it with `path` for the human table; (4) `clirequest/invocation.rs:202`: no `policy.remote` on push; (5) `clirequest/workspace.rs` `local_request`: refuse `--name ""` naming `--name`.
- **CP**: (1) re-point `PARITY_FIXTURE` at `parents[3] / "gwz-core/protocol/fixtures/cli_parity/local_family_cases.json"`, read `driver_message_contains.python`, remove the two message blocks from `parser_cases.json`; (2) join `root_path` with `path` in `render_family_listing`; nothing for rulings 2 and 4.
- **H**: `check_history` treats a non-empty `ProtectedRoots.unknown` as an unknown inventory (contract doc, `repo-contract/src/lib.rs:470`). **D**: same for disposal's fresh checks. **I**: only then switch `inventory_history` to per-root unknowns; may adopt `object_reader_conformance_allowing(&[Reflog, Stash])` in `tests/objects.rs`. **S**: implement `StoreFixture::alias_workspace` with a symlink so the S-4 case runs on the real store; `clear_failures` lets `BracketedStore` go. **R**: keep or replace the crate-level `result_large_err` allow. **W**: W-2 revisited at LCM2. **Lane owner**: the Bazel `MODULE.bazel` cure and the `.razel-exec` loop (root).

## 13. LCM1.1 wiring (lane C, core integration)

2026-09-06, lane C. The three dispatch slots stop refusing: `gwz clone
--local --name A [dest]` (verbatim), `gwz local list`, `gwz local dispose
<name> --keep` and `gwz local disband` run end to end over the libraries for
the first time. Five gwz-core commits on `96226c9`, each on an explicit
pathspec, each passing the lane gate on its own tree; no gwz-cli or gwz-py
change; no pinned file touched (`src/lib.rs`, `src/workspace_ops/mod.rs`,
`merge/mod.rs`, `gitbackend.rs`, `checked_artifact/**` all at their pinned
digests -- the adapters register under `src/local_clone/mod.rs`, which is not
pinned). LCM1.2 (the family merge's import through lane X's `prepare_import`
and the transport adapter, then the delegation) was **not started**, as
briefed: `family_merge.rs` steps 5-6 still refuse `unsupported_operation`
after resolution.

**Tuple.** gwz-core **`81fcaf225f96fd8d1fce1b3bfc4cb44f15bb21e4`** (lane C:
`861131d` LCM1.1a, `87e2d4d` LCM1.1b, `1bc771b` LCM1.1c, `b55b506` LCM1.1d,
`81fcaf2` LCM1.1e); gwz-cli `4c5d7fd` and gwz-py `30885cd` unchanged; gwz-dev
root `01984d4` plus the uncommitted files listed in §13.7.

### 13.1 What each port is wired to (`src/local_clone/adapters/`)

| Port | Adapter | Behind it |
|---|---|---|
| `InstallPorts::snapshot_source` | `install::capture_source` (taken **before** the family lock) over `inventory` | core's own traversal of the included repositories (root, manifest members, unmanaged nested repositories keyed `nested:<path>`; excluded subtrees skipped; symlinks never followed; `.git` never entered), `gwz-repo-inspect::LocalRepoInspector::inspect_layout` on each with every repository's hazards **aggregated** into one `LayoutError::Unsupported`, `git2` for local branch and remote names, SHA-256 over manifest+lock bytes, the open-merge probe |
| `InstallPorts::recheck_source` | the same capture again, compared whole; the first difference is the `Drift` detail | |
| `InstallPorts::observe_destination` | `install::CoreInstallPorts` | the filesystem (`exists`/`nonempty`/`is_workspace`), `gwz-family-store::YamlFamilyStore::observe_workspace` (index present, pointer, marker -- the new read-only store method), `.gwz/merge` presence, §4.1's at-ready column as `residual` (family lock, catalog, checked-artifacts, stash bundles, every repository's `.git/worktrees`), and `dependencies` = §4.0 dest-complete: `inspect_layout` of every destination repository, HEAD equal to the frozen source HEAD, and every protected root's object graph complete in the destination's own store through `gwz-history-check::check_history` (one call per repository, the repository as its own witness) |
| `InstallPorts::allocate_destination` | `std::fs::create_dir`; an existing empty directory is admitted | |
| `InstallPorts::install_destination_git` | `git_config::install_destination_git` | `git2` local-config edits: `remote.<name>.url`/`.pushurl` values that `gwz-repo-factory::origin_is_kept` rejects are removed; refspecs and `refs/remotes/*` stay; reported as `<repository>: <remote>` |
| `InstallPorts::construct_repositories` | refuses `Unimplemented` | `gwz-repo-factory` was **not** dropped in: the port shape fits (`FactoryRequest` from the snapshot, a `RepoBuildPort` over `git2`), but the builder port needs `transfer_objects` + checkout over the backend and the factory's follow-ups (`RecaptureLock`, `WriteFamilyRoot`, `WriteManifestLast`) need `recapture_configuration` to write a real lock for a constructed destination -- LCM2.3/LCM3.1 work, not cheap; clean and bare refuse at `create::clone_local` before any observation |
| `InstallPorts::recapture_configuration` | `CoreInstallPorts` | verbatim: the copied `gwz.conf/gwz.lock.yml` is read back (`artifact::read_lock`; an undecodable copy stops before the manifest) and reported `lock_recaptured: false`, nothing generated -- the copied lock *is* the destination's state byte for byte; clean/bare `Unimplemented` |
| `InstallPorts::publish_manifest` | `CoreInstallPorts` | `artifact::write_manifest(dest, source manifest)` -- the typed writer regenerates the conf-integrity marker over the final manifest and the copied lock; `marker_regenerated` = `inspect_conf_integrity(dest) == Verified` |
| `TreeCopier` | `gwz-refcopy::SystemTreeCopier`, `CopyMode::Auto` | `exclusions::verbatim_exclusions`: the ten fixed §4.1 copy-time rows (family index, lock, marker, pointer, catalog-final, checked-artifacts, `.gwz/locks`, `.gwz/merge`, `.gwz/stash/bundles`, `gwz.conf/gwz.yml`) plus `<git dir>/worktrees` per included repository; `gwz.conf/gwz.lock.yml` and `gwz.conf/markers/conf-integrity.yml` are copied and then superseded by publication |
| `FamilySession` | `gwz-family-store::YamlFamilyStore::try_lock` on the canonical addressed workspace (a clone locks its root through its pointer) | founding (`found`) when `reread` is `None`; the family id and both allocation ids are minted by core (`member_paths`: `fam_<32 hex>`, `alloc_<32 hex>` via `getrandom`) |
| `DisposalPorts::observe_target` | `disposal::CoreDisposalPorts` | the store's `observe_member_target` for the target (`Missing` short-circuits), the inventory traversal of the target tree, `inspect_layout`/`observe_work`/`inventory_history` per repository (a layout the inspector refuses becomes an `UnsupportedLayout` unknown), GWZ evidence fail-closed: merge = the probe (`Open`/`None`/`Unknown`), stash = `Unknown` when any bundle record exists (decoding into coordination roots is LCM2.1) |
| `DisposalPorts::check_history` | `CoreDisposalPorts` | **one `gwz-history-check::check_history` call per witness store** (lane H's rule H2), each with a `LocalObjectReader` over exactly one surviving repository, witnesses paired by identity (`RepoKey::Root` -> the root's repository and every other ready member's root; `Member{id}` -> the member with the same manifest id in each survivor; `nested:` -> no witness), outcomes combined per root: covered by any single witness whole -> `Preserved`; else `Unknown` if any witness was unknown, else `Unpreserved` naming the roots; an incomplete `ProtectedRoots` (I-2) answers `Unknown` before any read |
| `DisposalPorts::remove_directory` | `removal::remove_tree` | depth-first, `symlink_metadata` on every entry, a link removed as a link (never entered), the target itself must be a real directory; stops at the first error naming what remains |
| `LocalTransport` | `transport::BackendLocalTransport` (unchanged, LCM1.0c) | |

The compositions: `local_clone::create::clone_local` (mode -> observation
-> destination and recorded path -> capture before the lock -> lock ->
found -> `install`; a refusal that reserved nothing un-founds a family this
invocation founded; a failure after the row reports the step, the cause,
every completed effect and what is retained), `local_clone::dispose::keep`
(`gwz_local_disposal::dispose` with `DisposePolicy::Keep`; the addressed
workspace is canonicalised before the lock so the library's lexical
root/cwd comparison holds) and `local_clone::dispose::disband` (every
member's `remove_pointer`, then `FamilyChange::Disband`; a workspace in no
family is a `Noop`, an interrupted disband repeats). The open-merge probe
is `workspace_ops::open_merge_probe` = the merge store's own
`classify_open_record`, passed as a function pointer because
`workspace_ops::merge` is private to `workspace_ops` (a probe that cannot
classify a record is an error, never "no merge").

**The first real `gwz clone --local`.** `local_clone::tests::create::
root_to_a_verbatim_create_is_independent_installed_and_ready`, on a
workspace built with `gwz-local-testrepo` (a root repository plus member
`app`, one commit each, registered through `handle_create_workspace` and
`handle_add_existing_repo`, then an untracked `app/notes.txt` and an
unstaged `README` edit). Response message, verbatim: `created local clone
\`A\` at <tmp>/root-A (verbatim; recorded as ../root-A; 42 files copied (42
natively, 0 ordinarily), 44 directories, 0 symlinks, 10423 logical bytes; 0
remote URL(s) removed; family fam_<32 hex>, founded)`. What it produced:
`<tmp>/root-A` with `.gwz/family-root` and `.gwz/local-clone-allocation`
regenerated, no `.gwz/local-family.yml`, no `.gwz/local-family.lock`, no
`.gwz/merge`, no `.gwz/locks`, no `app/.git/worktrees`; `gwz.conf/gwz.yml`
byte-equal to the source's and the lock byte-equal, conf integrity
`Verified`; the dirt carried; the root's index founded with row `A`
`ready`, path `../root-A`, `checkout`, `verbatim`, source `.`; the family
readable from A through its pointer; the source untouched; every
destination repository's common dir inside the destination; a commit made
in `A/app` absent from the source's object store. 42 of 42 files went
through Apple `clonefile` (APFS).

### 13.2 What still refuses, and why

| Refusal | Where | Why |
|---|---|---|
| `--clean`, `--bare` | `create::clone_local`, before any observation | LCM3.1 / LCM2.3; `construct_repositories`, clean/bare `recapture_configuration` and `publish_manifest` answer `Unimplemented` behind it |
| `--from` (`copy_source`) | `validate_clone_local` | LCM3.2 (unchanged) |
| ordinary `dispose <name>` (no `--keep`) | `handle_local_family`, after the family observation | its fresh work/history checks are LCM2.1; the ports are wired and exercised directly (§13.3) but the slot refuses before any effect; `--keep` with hazards stays malformed |
| family merge steps 5-6 | `family_merge::handle`, after resolution | LCM1.2 -- lane X's `prepare_import(&ImportRequest { transfer, receivers, sources, selected, selector }, &mut BackendLocalTransport, &cancellation)` and the delegation to `handle_merge_with_events`, out of scope by the brief |
| a family `dry_run` | request shape | unchanged |

### 13.3 Tier B slices (`cargo test -p gwz-core --lib --locked local_clone::tests::<slice>`; Darwin 25.6.0 arm64, cargo 1.95.0; execution time as reported by the harness, warm)

| Slice | Tests | Cost | What it measures |
|---|---|---|---|
| `create` | 5 | 0.60 s | root -> A verbatim (above); an `alternates` hazard refused `unsupported_operation` **before reservation** with `family_files_absent` at the root (no lock file, no index, no pointer, no marker) and no destination; an install cancelled after the pointer and before the manifest (`CancelWhenExists(dest/.gwz/family-root)`) leaving row `A` `creating` with `last_error` "publish manifest: install cancelled", effects `[RowAllocated, DestinationAllocated, TreeCopied, DestinationGitInstalled, PointerInstalled, ConfigurationInstalled, ErrorRecorded]`, the pointer, marker, copied tree and copied lock present and **no manifest** (the manifest really is last); root -> A -> B from A (B recorded on the root with `source_path ../root-A`, B pointing at the root, `list` from B = root, A, B), a taken name and an occupied destination refused `path_collision` with nothing reserved; the fixture itself |
| `list` | 2 | 0.27 s | root -> A: `list` from the root, from A and from inside `A/app` reports `root` then `A` `ready` (`kind 0, recorded 1, observed 0`, path `../root-A`) with `root_path` = the root directory in all three, and leaves both trees byte-identical (a full-tree digest before and after); the interrupted create lists `creating`/`incomplete` with its diagnostic and nothing removed or promoted; the tree removed by hand lists `missing` |
| `dispose` | 5 | 0.62 s | `dispose A --keep`: pointer and marker gone, every other file byte-identical, the row gone, A in no family, a repeat `member_not_found`; keep on the interrupted create (no manifest appears); `disband` from clone B with A's pointer already removed by hand: A's marker, B's pointer and marker and the index removed, every tree byte-identical, a repeat `Noop`; ordinary deletion refused `unsupported_operation` with nothing touched; the ports directly -- `observe_target` (Present/Matches/Matches, two repositories, the copied untracked note observed, merge evidence `None`, history known and complete), `check_history` `Preserved` through the root's paired witness and `Unpreserved` naming a commit made only in A, a `nested:` key `Unpreserved` with no witness, `remove_directory` on a scratch tree |
| `request` | 4 | 0.13 s | the pinned refusal order, now: clean mode unsupported before any family file; ordinary dispose unsupported; a disband outside any family a `Noop` writing nothing |
| `transport` | 7 | 0.07 s | unchanged |
| `local_clone::adapters` (unit) | 10 | 0.01 s | minted ids, the default sibling destination, the recorded path (`../gwz-dev-A`, `../../../../tmp/lanes/A`, the inside/itself/above refusals), the intended destination (`dest=.` is the invocation directory, as `git clone <url> .`), the §4.1 exclusion set row for row, the remover (a link out of the tree removed as a link, a symlinked target refused), the remote-URL rule (https and scp-style kept; a path, `file:`, a token URL and a `../peer` push URL dropped; refspecs kept; idempotent) |

Whole `local_clone` filter: 45 tests, 1.6 s. Each real-workspace test builds
its own fixture (two `git init`, two commits, the two public handlers) --
about 60 ms -- so the slices are far inside the 10 s Tier B target.

### 13.4 The two discrepancies

- **Design §4 step 2 ("allocate the destination and write its marker").**
  Resolved by **amending the design text** (root file, uncommitted: design
  revision 11, §4 steps 2-3, §11 item 21), not by splitting the store's
  writes. The store's `install_pointer` writes marker-then-pointer in one
  call, and that order is the recoverable one the contract froze
  (LCM1.0c-rem1 State P2-2); the copy contract admits only a new or empty
  destination, so a marker written in step 2 would make the destination
  non-empty before the copy. Splitting the store into two calls would give
  the orchestrations a second ordering to get wrong for no gain: the row
  is still reserved before any destination effect, and a marker without a
  pointer is exactly the interrupted shape `local list` already classifies.
- **§4.1's "remove in install" row had no named owner.** Given to
  `install_destination_git` (`adapters/git_config.rs`), and **confirmed as
  installation's job in every mode**, not the factory's for clean and bare:
  the two share one rule, `gwz-repo-factory::origin_is_kept` -- install
  strips what a verbatim copy inherited, and the factory's `set_origin` is
  only ever handed a URL that rule keeps, so a constructed destination has
  nothing to strip and the port reports nothing removed. Recorded in
  design §4.1 (the paragraph after the table) and §11 item 22 (root,
  uncommitted), and in gwz-core `dev-docs/GWZDesign.md`. Only the URL keys
  are removed; fetch refspecs and `refs/remotes/<name>/*` are copied
  history and stay.

### 13.5 Crate edits outside `src/`, with reasons

| Path | Edit | Reason |
|---|---|---|
| `Cargo.toml`, `Cargo.lock` | `[dev-dependencies] gwz-local-testrepo = { path = "crates/local-testrepo" }`; one lock line (`gwz-core`'s dependency list) | the Tier B slices build real workspaces with lane T's harness; dev edge only; the root gwz-dev `Cargo.lock` is unchanged and `--locked` there (verified with `cargo metadata --locked --offline` at the root) |
| `crates/family-store/src/lib.rs`, `tests.rs` (lane S) | additive: `YamlFamilyStore::observe_workspace(workspace, family_id, root, allocation) -> WorkspaceObservation { Missing, Unobservable{detail}, Present(WorkspaceMetadata { index, pointer, marker }) }` and `observe_member_target(root, view, row) -> TargetObservation`; one in-crate test | the port-shape addition the wiring proved necessary: `gwz local list` and disposal's fresh evidence read the pointer and marker at each row's resolved path, and the format (including the marker schema, `pub(crate)`) is the store's, so the observation-only reading lives there rather than as a shadow decoder in core. Not a `FamilyStore` contract method (that would have widened the frozen contract and the in-memory fake for a read core composes concretely anyway). Decision recorded in its doc: a pointer `Matches` only when it names this family **and** this root (resolved), so a moved root lists `mismatched` (design §11 item 4, fail closed). Tier A 39 (was 38); `run_all` untouched |
| `scripts/checks/run_r4bg_aggregate_gates.py` | lib remainder pin 1030/1031 -> **1052 darwin (MEASURED) / 1053 linux (DERIVED)**, dated reason | 22 new `#[test]` rows, all under `local_clone` (adapters 10, tests::create 5, tests::list 2, tests::dispose 5); census 1778 rows (was 1756); `checked_artifact::` 459 and `v1_lifecycle::` 266 unmoved. **Process note:** the rows landed in LCM1.1a-d and the pin followed in LCM1.1e rather than in each commit -- a deviation from the brief's "same commit", reported here |
| `docs/RustApi.md`, `docs/ErrorCatalog.md`, `dev-docs/GWZDesign.md` | LCM1.1 status, the adapters, what still refuses, the codes reused (nothing new allocated) | documentation |

No other crate was edited. `crates/local-testrepo`, `repo-inspect`,
`refcopy`, `workspace-install`, `local-disposal`, `history-check`,
`repo-factory`, `family-model`, the three contracts and `local-import` are
consumed as they stand; no port shape in any of them changed.

### 13.6 Gates (final tree, gwz-core `81fcaf2`; Darwin 25.6.0 arm64, cargo 1.95.0, python3.13, from gwz-core)

| Gate | Result |
|---|---|
| `cargo fmt --all -- --check` | clean (run before every commit) |
| `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features --locked -- -D warnings` | clean (before every commit; one `if_same_then_else` in the store's observation fixed before LCM1.1a) |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | ok (24 visible entries, 9 classified modules) |
| `python3.13 scripts/checks/check_local_clone_boundaries.py` + its unittest | ok (14 packages, 39 edges); 23 OK |
| `python3.13 protocol/regen.py --check` | OK (no protocol change) |
| Tier A `--locked`: family-store 39, workspace-install 27, local-disposal 27, repo-inspect 88, local-testrepo 39 | all passed |
| `cargo test -p gwz-core --lib --locked local_clone` | **45 passed**, 1.6 s (was 23) |
| lib remainder (`-- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::`) | 1052 passed; 1 ignored; 53.4 s |
| `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 96226c9 HEAD` | `lane gate: ok` at all five commits, 39 s |
| root `cargo metadata --locked --offline` | ok (root lock unchanged) |

Not run: the 16-minute probe suites, Bazel (the fu3 debt stands), gwz-cli
and gwz-py suites (no driver change; the CR/CP lanes own their trees).

### 13.7 Left uncommitted for the lane owner (root repo)

`dev-docs/GwzLocalCloneDesign.md` (revision 11: status, §4 steps 2-3, the
§4.1 paragraph after the exclusion table, §11 items 21-22; supersedes
revision 10, SHA-256 `d06e9eebb2538fff67898c47df6c8dd31c8a335fc36d0aade6bf14ee8a0c6c1d`)
and this record (§13). The member pin to record through `gwz`: gwz-core
`81fcaf225f96fd8d1fce1b3bfc4cb44f15bb21e4`; gwz-cli `4c5d7fd` and gwz-py
`30885cd` unchanged. No root `Cargo.lock` change.

### 13.8 Residual risks and what the next lanes must know

- **Cost of dest-complete.** `observe_destination` walks every destination
  repository's protected object graph through `gwz-history-check` (headers
  and edges, memoised). On the fixtures it is milliseconds; on a workspace
  the size of gwz-dev it is seconds and scales with the object count.
  Design §4.0 asks for it ("validate reachable object connectivity");
  `Limits::default()` (100 000 roots, 256 MiB bookkeeping) bounds it and a
  limit is an `Incomplete` destination, never a silent pass. If the cost
  proves too high on real trees, the bound -- not the check -- is the
  lever.
- **Nested bare repositories** (a directory that is itself a Git directory,
  with no `.git` entry) are not inventoried: they are copied as ordinary
  directories and neither inspected nor independence-checked. Design §4.0
  names "unmanaged/ignored nested repositories"; the traversal recognises
  them by a `.git` entry (directory or gitfile). A `.git` gitfile is found
  and refused (`GitFile`), as the design requires.
- **Error codes.** Nothing new was allocated; `io_error` carries source
  drift, copy failures, an incomplete destination and a cancelled install,
  and `unsupported_operation` carries a §4.0 source hazard. The messages
  name the step, the typed cause, the effects and what is retained, which
  is what an operator acts on; a dedicated code is a driver-facing question
  for the lane owner (design §7 allocates none).
- **Founding and refusal.** A create that founds a family and is then
  refused before reservation removes the empty index it just wrote
  (`Disband` on a memberless family); the lock file `.gwz/local-family.lock`
  created by `try_lock` stays, as it does after any locked operation. A
  §4.0 hazard is refused before the lock, so that case leaves nothing at
  all (measured).
- **`recapture_configuration` in verbatim mode reports `lock_recaptured:
  false`**: nothing is recaptured because the copied lock is the
  destination's exact state; the library requires recapture only for
  clean/bare. When LCM3.1 lands, the port must write a real lock for a
  constructed destination.
- **Windows** builds of the new adapters are unverified here (the
  foreign-target clippy step in CI covers only the crates); the remover
  has a `cfg(windows)` link arm (`remove_dir` then `remove_file`).
- **CR/CP**: nothing changes on the wire. The create's response carries
  its summary in `response.meta.message`; `local dispose --keep` and
  `disband` answer `LocalFamilyResponse` with empty `members`, no
  `root_path`, and a message; a disband outside any family is
  `AggregateStatus::Noop`. The parity fixture is untouched.
- **Lane X (LCM1.2)** picks up at `family_merge::handle` after
  `resolve_family_merge`: build `ImportRequest` (transfer id, receivers
  from the addressed workspace's lock, sources from the bound member's
  path, `selected`, `SourceSelector::Head` or `Ref`), `prepare_import` over
  `BackendLocalTransport`, then `handle_merge_with_events` with the
  selector cleared and `source_ref = import_ref`. The family lock is the
  store session from `try_lock`; hold it across both.

## 14. LCM1.1 fixes 1-3 (lane C, core integration)

2026-09-06, lane C. The three things the LCM1.1 wiring (§13) left open:
the overloaded error codes, the unbounded destination-completion check,
and the two drivers' untested path joins. The measurement for fix 2 found
a defect the wiring had not seen: dest-complete would have **falsely
refused every real repository** in this workspace after the copy. Commits
(each on an explicit pathspec, no pinned file touched, no attribution
trailer): gwz-core **two** (fixes 1+2 together -- they share
`create.rs`, `tests/create.rs` and the remainder pin, and neither half
compiles alone -- then the fix 3 fixture), gwz-cli one, gwz-py one.

**Tuple.** gwz-core **`8bb88049684f0c24a207f83a2e82f32eaf48942a`** (lane C:
`a398450` fixes 1+2, `8bb8804` fix 3, on top of the concurrent Bazel
lane's `2c30ebe`, which is not lane C's); gwz-cli
**`5e5f69b`**; gwz-py **`88f8d2b`**; root `6b55f61` plus the two
uncommitted files in §14.7. Baseline: gwz-core `81fcaf2`, gwz-cli
`ed132ea`, gwz-py `d9e81a0`. `check_lane_commits.sh 81fcaf2 HEAD`: `lane
gate: ok` at `2c30ebe`, `a398450` and `8bb8804`.

### 14.1 Fix 1 -- the four local-create codes (`GwzErrorCode` 63-66)

Allocated after `unknown_local = 62` in `protocol/gwz.taut.py`; regenerated
`src/protocol/generated.rs` (+12 lines; the corpus vectors are unchanged
-- no message changed), gwz-py `src/gwz/protocol/generated/{api.py,
gwz.ir.json}`; `docs/MessageCatalog.md` through its generator (+4 rows);
`docs/ErrorCatalog.md` (four rows and a rewritten "Local Clone Family"
section with the call-site table), `docs/Protocol.md`, `docs/RustApi.md`,
gwz-core `dev-docs/GWZDesign.md`; design §7 and §11 item 23 (root). The
mapping is one table, `src/local_clone/errors.rs::{layout_code,
install_port_code, install_refusal_code, install_error_code}`, which
`create.rs` now calls (`port_error` for the capture before the lock,
`failure_error` for the install).

| Code | Outcome | Call site (typed cause) | Why its own code |
|---|---|---|---|
| `unsupported_source_layout` (63) | a design §4.0 source-layout hazard (gitfile, external common dir, alternates, escaping link or configuration, partial clone, environment override), or a `.git` entry that is not a repository; refused before reservation, nothing written | `InstallPortError::Layout(LayoutError::Unsupported \| NotARepository)` at `capture_source` (before the lock) and `InstallRefusal::SourceLayout` at admission | it is a refusal of the *source*, which the operator repairs (dissociate, convert, move the hooks) -- not "this build lacks the feature", which `unsupported_operation` now means exactly |
| `copy_failed` (64) | the tree copy stopped: permission, space, I/O, metadata, an uncopyable entry; row and partial destination retained, source unchanged (design §4, §12) | `InstallError::Copy` for every `CopyErrorCategory` except `DestinationNotEmpty` (still `path_collision`), `Unimplemented` (`unsupported_operation`) and `Cancelled` (below) | the operator's next step is `dispose --keep` and a retry after fixing the cause; an `io_error` said nothing about the retained row |
| `source_drift` (65) | the source moved between the snapshot and publication (design §4 step 3, §12) | `InstallPortError::Drift` from `recheck_source` (`InstallError::Source`) | the cure is quiescence (design §2), not disk or permissions |
| `destination_incomplete` (66) | a completion rule failed before ready -- §4.0 dest-complete (a missing object, HEAD off the frozen HEAD, an inadmissible layout, a walk past the ceiling), §4.1's at-ready column, `LockNotRecaptured`, `MarkerNotRegenerated` -- or the install was cancelled | `InstallError::Incomplete`, `InstallError::Cancelled`, `CopyErrorCategory::Cancelled` | the retained shape `local list` reports as `creating/incomplete`; design §4 step 4 names "errors or interruption" as one outcome, and the message says which |

Kept: `unsupported_operation` for clean/bare, `--from`, ordinary dispose, a
family dry-run, `LayoutError::Unimplemented` and every `Unimplemented`
port/store/copier; `io_error` for `LayoutError::ReadFailed` (an inspector
that could not read, lane I proposal I-3), `InstallPortError::
{Construction, Configuration, Destination}`, a path that does not resolve,
the store's `Io`/`Partial`, disposal's `Port`/`RemovalStopped`. **Not
allocated:** a cancellation code -- the wired slot passes `NeverCancelled`,
so a cancelled install has a call site but no producer outside tests;
folding it into `destination_incomplete` describes the state the operator
faces and the message carries "install cancelled". `DisposeError::Unknown`
(unknown disposal evidence, LCM2.1, unreachable behind the ordinary-dispose
refusal) stays `unsupported_operation` until it has a producer.

**Tests.** `local_clone::errors::tests::install_failures_map_onto_the_four_local_create_codes`
(every variant of the three enums and every copy category, plus
distinctness from each other and from 14/28); `tests::create`:
`a_source_hazard_refuses_before_reservation_and_leaves_nothing` now asserts
63, `an_interrupted_create_leaves_a_creating_row_and_an_inspectable_directory`
66, and three provocations -- `source_drift_before_publication_is_source_drift_with_the_row_retained`
(a cancellation port that creates a branch in the root repository at the
`Reserve` checkpoint, after the snapshot: 65, effects through
`PointerInstalled`, `last_error` "source drift", no manifest),
`an_object_missing_from_the_destination_store_is_destination_incomplete`
(a port that deletes `app`'s HEAD tree object from the destination once
the copier has reproduced it: 66, "objects missing from the destination
store", the tree's id, no manifest),
`an_unreadable_source_file_is_copy_failed_with_the_partial_destination_retained`
(`#[cfg(unix)]`, a 0o000 file in `app`; `clonefile` refuses it as `cp -c`
does: 64, `DestinationAllocated` without `TreeCopied`, no pointer, source
untouched). `tests/protocol.rs::error_code_wire_values_are_pinned` pins
63-66, their `from_wire` round trip and their distinctness from 14 and 28.
The `LocalFamilyMemberEntry` hex pin `a601614102000301040405672e2e2f77732d4106f6`
is **unmoved** in `tests/protocol.rs` and gwz-py `test_codec.py` (no
message changed; both green).

**Pins moved, one fingerprint of one schema.** `gwz-core/protocol/
check_log_additive.py`, `gwz-py/scripts/check_protocol_drift.py`,
`gwz-py/src/tests/test_log_protocol.py`:
`2eca6469ed1281e77a95f1e419aa4065002aa94c77507a73ada6f6f9c8bb5503` ->
`0a173de982aaa93225e26581d678b4722356afc967fb4543de531708900cf981`,
MEASURED additive: the pre-log projection rendered on `81fcaf2` and on the
edited schema and diffed -- 4 added lines, 0 removed, 3 hunks, the four
enum members as map keys; the old pin reproduced on the old tree.
`regen.py --check` OK; gwz-py packaged-IR drift check OK
(`sha256:9f624806ae5e485e92a7ed47503591f6a4c8bb10d9f1b877aaf61245cbacf5cc`).

**Drivers.** Both render a `ModelError` code generically (gwz-cli
`{:?}` of the model enum; gwz-py the PascalCase label of the wire name),
so no rendering code changed; the tests pin it. gwz-cli `src/tests/g12.rs`:
`the_four_local_create_codes_are_presented_as_typed_refusals` (human
`Code: message`, `--json` label and message, wire 63-66, distinct from
`UnsupportedOperation`/`IoError`) and
`a_source_layout_hazard_reaches_the_driver_as_unsupported_source_layout`
(a real workspace whose root repository holds `objects/info/alternates`:
`gwz clone --local --name A` refuses 63 through the driver, "Alternates",
"nothing was reserved", no index founded). gwz-py
`src/tests/test_cli_local_family.py`: four `REFUSALS` rows (human and
`--json` presentation, 20 parametrised cases) and `test_protocol.py` pins
63-66 beside 14 and 28. The argv parity fixture
`local_family_cases.json` is **untouched**: its `core_refuses` rows are the
dry-run, `--from`, unknown-hazard and `unknown_local` refusals, and none of
the four new outcomes is decidable from a command line.

### 14.2 Fix 2 -- the destination-completion check, measured and bounded

**Measured first** (Darwin 25.6.0 arm64, Apple M-series; `check_history`
with the repository as its own witness, `Limits::default()`, exactly as
`install.rs:200-226` ran it; a temporary harness, not committed):

| Repository | Store (loose + packed) | Roots | Outcome before | Cost before (warm / cold) |
|---|---|---|---|---|
| gwz-core | 2 044 + 7 166 = 9 210 | 374 | **Unpreserved, 30 roots** (reflog entries, `stash@{1}`) | 152 ms / 380 ms |
| gwz-cli | 1 609 + 309 = 1 918 | 220 | **Unpreserved, 12 roots** | 80 ms / 271 ms |
| gwz-dev root | 4 957 + 0 | 473 | **Unpreserved, 3 roots** (`stash@{1..3}`) | 262 ms / 778 ms |
| taut | 68 + 1 462 = 1 530 | 24 | Verified, 1 378 objects | 13.6 ms (9.9 µs/object) / 25 ms |
| synthetic, loose, 1 000 commits | 5 000 | 1 002 | Verified | 0.27 s (54 µs/object) |
| synthetic, loose, 4 000 commits | 20 000 | 4 002 | Verified | 1.35 s (67 µs/object) |
| synthetic, loose, 16 000 commits | 80 000 | 16 002 | Verified | 9.0 s (112 µs/object) |
| synthetic, packed (`git repack -adq`), 16 000 commits | 80 000 | 16 002 | Verified | 4.9 s (61 µs/object) |

Two findings. (1) **The cost**: linear in the reachable object count,
roughly one second per 10-16 k objects, superlinear on a large loose store
(directory lookups), with bookkeeping of about 186 bytes per distinct
object at peak (14.9 MB for 80 k) -- so `Limits::default()`'s 256 MiB was
an implicit ceiling of about 1.4 M objects that nothing stated. (2) **A
defect**: `check_history`'s witness rule (`is_eligible_witness_root`)
rightly excludes a witness's own reflog and stash entries -- operation
state is not durable retention -- so with the destination as its own
witness, every protected root that only a reflog or stash entry names is
"no eligible retained witness root reaches it": Unpreserved. Every real
repository here has such roots (an amended commit, an older stash); the
LCM1.1 fixtures have one commit each and never did. A real `gwz clone
--local` of this workspace would have been refused after the copy with
"objects missing from the destination store", the row left `creating`.

**What changed.** Dest-complete is now a *connectivity* walk, not a
preservation proof:

- `gwz-history-check::check_connectivity(protected, reader, limits,
  cancellation) -> ConnectivityOutcome {Complete(ConnectivityCoverage),
  Incomplete(Vec<MissingObject{root, missing}>), Unknown}` -- additive
  (crate `lib.rs`, a `# Connectivity` section in the crate doc; `check_
  history` untouched, disposal still uses it): every protected root is
  walked from itself, an object is read once however many roots reach it,
  and the outcome is per root -- the first object found missing beneath it,
  or the root's own object. Same `Limits`, same memoisation, same
  cancellation points, same `Unknown > Incomplete > Complete` precedence;
  an incomplete inventory (`ProtectedRoots::unknown`, contract I-2) is
  `Unknown` before any read. Tier A: 9 new rows (`a_reflog_only_root_with_a_complete_subgraph_is_connected`
  pins both answers side by side -- Unpreserved to `check_history`, Complete
  to `check_connectivity`, every object once; a missing blob below one root
  names that root and the blob; a root whose own object is missing; shared
  subgraphs read once; an incomplete inventory, uninterpreted evidence, the
  root cap, the bookkeeping cap and cancellation all `Unknown` and never
  `Complete`; no roots; an unreadable store), 39 in all (was 30).
- `src/local_clone/adapters/object_census.rs` (new, registered in
  `adapters/mod.rs`): `census_of(common_dir)` counts the destination store
  before the walk -- loose object files by shape under `objects/xx/`, and
  every pack index's fan-out total (v2 at offset 8, v1 at 0), 1-24 ms on
  the repositories above -- and `connectivity_limits(roots, census)`
  derives the walk's `Limits` from what was copied: `max_roots` exactly the
  roots the inventory found (the walk starts from all of them and needs no
  cap of its own; the 100 000 default could refuse a reflog-heavy store for
  nothing), `max_bookkeeping_bytes` = min(256 MiB, 512 B x census + 4 MiB).
  The census is an upper bound on what the walk can visit (an object both
  loose and packed counts twice, never zero times). Four tests (a synthetic
  store with v1 and v2 indexes and non-object files; agreement with `git
  count-objects` before and after `git repack -adq`; an unreadable store;
  the limits).
- `adapters/install.rs::dependencies` calls `check_connectivity` with those
  limits and records a `RepositoryVerification {key, roots, objects_visited,
  census, elapsed}` per repository (`CoreInstallPorts::verifications`);
  `Incomplete` and `Unknown` details now carry the census, the root count
  and, for a ceiling, the bookkeeping allowed. `create.rs::CreateReport`
  gains `verification` and the message a clause: `dest-complete: 2
  repositories, 23 objects verified of 23 in store, 1 ms`.

**Measured after** (`check_connectivity`, derived limits, warm):

| Repository | Census | Roots | Objects visited | Cost after | Bookkeeping / budget |
|---|---|---|---|---|---|
| gwz-core | 9 242 | 375 | 8 781 | **105 ms (12 µs/object)**, census 5.5 ms | 1.03 MB (117 B/object) / 8.9 MB |
| gwz-cli | 1 921 | 221 | 1 842 | 55 ms (30 µs/object), census 4.8 ms | 0.27 MB / 5.2 MB |
| gwz-dev root | 4 957 | 473 | 3 830 | 250-280 ms (65-73 µs/object), census 11 ms | 0.64 MB / 6.7 MB |
| taut | 1 530 | 24 | 1 378 | 7.7 ms (5.6 µs/object), census 0.9 ms | 0.17 MB / 5.0 MB |

All four `Complete`. The per-object cost is unchanged in kind (it is the
same `read_object` loop); the real repositories are faster than the
false-refusing walk because nothing gathers witness roots or tracks
origins. A whole `gwz clone --local` of this workspace's five repositories
(about 19 k objects) now spends about 0.45 s in dest-complete.

**What is and is not verified now.** Verified, for every repository that
stands at the destination: an admitted layout (`inspect_layout`: no
external common dir, alternates, escaping link or configuration), HEAD
equal to the frozen source HEAD, and every protected root the destination
holds -- every ref, `HEAD`, every retained reflog entry, every stash entry,
any coordination root -- with its exact object and entire subgraph
readable from the destination's own store; each object read once; the
walk bounded by exactly those roots and by a bookkeeping budget derived
from the copied store's census, never above the library's 256 MiB, which
is the **documented outer ceiling: about 1.4 million objects per
repository** (measured 186 B/object peak; roughly 90-150 s at the measured
rates), past which the walk answers `Unknown(LimitExceeded)` and the
create refuses `destination_incomplete` naming the ceiling, the census and
the roots, with the row and directory retained. Not verified: object
*content* integrity (the reader trusts the store's own hashing; a corrupt
object that still parses passes, as it would `git fsck --connectivity-only`);
objects no root reaches (dangling by construction -- a `gwz init` root with
an unborn HEAD reports "0 objects verified of 6 in store"); nested bare
repositories (§13.8, unchanged); and wall-clock time -- the ceiling is in
objects, and a lower one (or a source-side census refusal before
reservation, which would spare the operator a refusal after a long copy)
is a product decision left to the lane owner.

**Tests** (gwz-core, beyond the crate's): `tests::create::a_source_with_reflog_only_history_creates_and_reports_the_verified_objects`
-- RED before the fix (the false refusal reproduced: `app`'s only commit
amended so the original is reflog-only, and a stash in `app` naming two
commits no ref does), now creates with `app` reporting exactly 8 objects
(two commits, their shared tree and blob, the stash commit, its index
commit, the stashed tree and blob), the census bounding every walk, and
the message carrying the clause; `an_object_missing_from_the_destination_store_is_destination_incomplete`
proves the walk still catches a missing object after the change.

### 14.3 Fix 3 -- the two drivers' path joins, pinned together

`gwz-core/protocol/fixtures/cli_parity/local_family_listing_cases.json`
(beside the argv fixture; `_schema`-documented like it): fourteen cases,
each a `root_path` (string or null), a member `path` and the display path
both drivers must render -- the seven the brief named (`plain-child`,
`sibling-through-parent`, `nested-member`, `two-level-escape`,
`absent-root`, `empty-root`, `already-absolute-member-path`) plus the root
row itself, an absent root for the root row, a root with a trailing
separator, a parent reference inside the member path, current-directory
segments, an escape above a relative root and an escape at the filesystem
root. Expected values are spelled with `/`; a driver on a host whose
separator differs compares after mapping it (neither driver's suites run
on Windows today -- both Windows CI legs build and package only -- so that
is a stated allowance, not a measured one). gwz-cli
`src/tests/g12.rs::the_listing_fixture_cases_render_the_expected_display_path`
(`include_str!` of the sibling, `local_list_render::member_path`, unique
ids, the seven required ids present); gwz-py `test_cli_local_family.py::
test_listing_fixture_case_renders_the_expected_display_path` (one
parametrised row per case, `cli_local_family.member_display_path`, skips
with the reason when the sibling checkout is absent) and
`test_listing_fixture_covers_the_required_shapes_once_each`.

**The drivers disagreed on one case, `already-absolute-member-path`.**
gwz-cli's `Path::new(root).join(path)` lets an absolute member path
replace the root (`/tmp/lanes/A`); gwz-py's
`os.path.join(root_path, *path.split("/"))` split the path first, the
leading empty segment lost the absoluteness, and the result nested under
the root (`/Users/limbo/gwz-dev/tmp/lanes/A`). The fixture keeps the
conventional answer -- the path as the row records it, never a fabricated
nested one -- and gwz-py's join was corrected to
`os.path.normpath(os.path.join(root_path, path))` (`normpath` already
maps `/` to the host separator, so the split was never needed). The wire
never carries an absolute member path; the disagreement was latent. The
other thirteen cases agreed on the first run in both drivers.

### 14.4 The drivers' stale rows (a finding at the baseline)

Five gwz-cli tests were red at the baseline tuple itself -- gwz-cli
`ed132ea` against gwz-core `81fcaf2` -- because they pinned every family
verb as `UnsupportedOperation` (the pre-LCM1.1 answer) and §13.6 records
that the driver suites were not re-run after the wiring: `g12::
clone_local_dispatches_and_refuses_typed` (the verbatim clone is served
and succeeded, into `../dest-a` beside the temporary directory),
`g12::local_family_verbs_dispatch_and_refuse_typed` (`dispose --keep` is
`MemberNotFound` for a workspace in no family, `disband` a served `Noop`),
`local_family_workflows::{family_verbs_reach_core_and_report_its_typed_refusal,
a_refused_family_verb_carries_no_listing,
jsonl_streams_the_operation_lifecycle_then_the_refusal}` (`disband`).
Rewritten to core's measured answers through the real binary: ordinary
`dispose` with or without a waiver stays `UnsupportedOperation` (exit 1,
`event, event, response` on `--jsonl`); `dispose --keep` is
`MemberNotFound` naming the verb and the workspace; `disband` outside a
family exits 0 as `Noop` ("nothing to disband") and founds nothing; the
verbatim clone is served into a destination the test owns
(`clone_local_dispatches_verbatim_and_refuses_the_unbuilt_modes_typed`),
and a new `the_verbatim_lifecycle_is_served_end_to_end` drives `clone
--local -> local list (root_path beside the rows) -> dispose --keep
(every file retained, pointer gone) -> disband (index gone, tree retained)`
through the binary. Clean, bare, `--from`, dry-run and the three
`merge --remote` rows are unchanged.

### 14.5 Gates (final trees; Darwin 25.6.0 arm64, cargo 1.95.0, python3.13, from gwz-core unless noted)

| Gate | Result |
|---|---|
| `cargo fmt --all -- --check` | clean (gwz-core; gwz-cli `cargo fmt -p gwz -- --check` clean) |
| `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features --locked -- -D warnings` | clean |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | ok (24 visible entries, 9 classified modules) |
| `python3.13 scripts/checks/check_local_clone_boundaries.py` + `-m unittest scripts/checks/test_check_local_clone_boundaries.py` | ok; 23 OK |
| `python3.13 protocol/regen.py --check` | OK |
| `protocol/.regen-venv/bin/python protocol/check_log_additive.py` | OK `0a173de9...` |
| `cargo test -p gwz-core --lib --locked local_clone` | **54 passed** (was 45; +9) |
| Tier A `cargo test -p gwz-history-check --lib --locked` | **39 passed** (was 30; +9); no other crate touched |
| `cargo test -p gwz-core --test protocol --locked` (`error_code_wire_values_are_pinned`, `local_clone_follow_up_2_allocations_are_pinned`) | passed |
| lib remainder census (`--list`) | 1787 rows (was 1778); `checked_artifact::` 459 and `v1_lifecycle::` 266 unmoved; remainder 1062 listed = **1061 executed darwin** (+9), linux 1062 DERIVED; `run_r4bg_aggregate_gates.py` re-pinned 1052/1053 -> 1061/1062 **in the same commit as the rows** |
| gwz-cli `cargo test -p gwz --locked` (from gwz-cli) | **240 passed** (was 236 at the brief's count, 5 of them red against `81fcaf2`): lib 170 (+3), `local_family_workflows` 7 (+1), diff 26, local 29, publish 4, release 2, rename 2 |
| gwz-cli `python3 scripts/generate_cli_reference.py --check` | exit 0 |
| gwz-py `pytest src/tests/test_protocol.py test_codec.py test_log_protocol.py test_cli_local_family.py` (`.venv/bin/python`) | **173 passed** (138 with the HEAD test files against the regenerated package, one of them the moved pin; +35: 20 refusal rows, 14 fixture cases, 1 coverage) |
| gwz-py `scripts/check_protocol_drift.py` | OK |
| `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 81fcaf2 HEAD` | see the final report (run after the commits) |

Not run: the 16-minute probe suites; the full lib-remainder execution
(the `--list` census is the brief's method); Bazel (a concurrent lane owns
it); Windows (unverified here, as in §13.8).

### 14.6 Pins moved (old -> new)

- `gwz-core/protocol/check_log_additive.py`, `gwz-py/scripts/check_protocol_drift.py`,
  `gwz-py/src/tests/test_log_protocol.py`: `2eca6469...` -> `0a173de9...`
  (MEASURED additive, 4 added / 0 removed / 3 hunks).
- `gwz-core/scripts/checks/run_r4bg_aggregate_gates.py`: lib remainder
  1052 / 1053 -> 1061 / 1062 (census; +9 rows, all `local_clone::`).
- `tests/protocol.rs` and gwz-py `test_codec.py` hex pins: **unmoved**.
- `docs/MessageCatalog.md`: regenerated (+4 rows).

### 14.7 Left uncommitted for the lane owner (root repo)

`dev-docs/GwzLocalCloneDesign.md` (revision 12: status, §7 the four codes,
§11 items 23-25; supersedes revision 11, whose SHA-256 the status line
records) and this record (§14). The member pins to record through `gwz`:
gwz-core `8bb8804`, gwz-cli `5e5f69b`, gwz-py `88f8d2b`. No root
`Cargo.lock` change (no dependency moved; `gwz-core/Cargo.toml` and its
lock are unchanged).

### 14.8 Residual risks and what the next lanes must know

- **A lower dest-complete ceiling, or a source-side one**, is the lane
  owner's call: the walk is bounded and reported, but a 1.4 M-object
  repository walks for minutes before refusing, after the copy. A census
  of the *source* store before reservation would refuse before any effect.
- **`check_history` still carries the I-2 obligation** (a non-empty
  `ProtectedRoots::unknown`); `check_connectivity` honours it from birth.
- **Cancellation has no producer** in the wired slot; when a driver-side
  cancellation port lands, decide whether a cancelled create deserves a
  code of its own or stays `destination_incomplete`.
- **`InstallPortError::Configuration` for an undecodable copied lock**
  maps to `io_error`; `manifest_invalid` would fit better, but the port
  error carries only a string. Left as is.
- **The listing fixture's Windows allowance** is unmeasured; the first
  driver suite to run on Windows measures it.

## 15. LCM1.0c follow-up 4 (lane C, core integration): two invariants made enforced

2026-09-06, lane C. Two invariants that held by habit now hold by a gate and
by code: the hand-pinned Bazel hub for gwz-cli tracks the outer `Cargo.lock`
(§15.1), and the local clone family record is ignored by git at every
workspace because the create writes the exclude, not because a copy happened
to carry it (§15.2). Commits (each on an explicit pathspec, no pinned file
touched, no attribution trailer): gwz-core **two** -- `603d628` the gate,
`63f1332` the privacy enforcement with its Tier B slice and the remainder
re-pin (one commit: the rows and the pin move together); gwz-cli **one**
(`8fd6df6`, a comment naming the gate); gwz-py untouched.

**Tuple.** gwz-core **`63f1332`** (on `8bb8804`), gwz-cli **`8fd6df6`** (on
`5e5f69b`), gwz-py **`88f8d2b`** (unchanged), root `1d5f8a3` plus the two
uncommitted files in §15.5. `PYTHON=python3.13 scripts/checks/check_lane_commits.sh
8bb8804 HEAD`: `lane gate: ok` at `603d628` and `63f1332`.

### 15.1 The Bazel pin-drift gate (`gwz-core/scripts/checks/check_bazel_pin_drift.py`)

**The hazard.** Under workspace layout Option A (§11) `crate.from_cargo`
cannot splice the outer virtual workspace (MODULE.bazel's header records the
three reasons), so the `@crates` hub is `crate.from_specs` with gwz-cli's
three direct crates.io dependencies pinned **by hand** -- `clap =4.6.6`
(`derive`, `wrap_help`), `console =0.16.4`, `serde_json =1.0.151` -- and cargo
and Bazel agreed only while a human kept those pins in step with
`//:Cargo.lock`. The comment said so; this gate is the check the comment
could not be.

**What it compares.** Four files, three of them inputs the brief named:

| Input | What is read | How |
|---|---|---|
| root `MODULE.bazel` | every `crate.spec` scoped to the hub -- `package`, `version`, `features`, `default_features`, `repositories` -- and the `crate.from_specs(name = "crates")` declaration | `ast.parse` (Starlark is a syntactic subset of Python), no evaluation; a file `ast` cannot parse is an error |
| `gwz-cli/Cargo.toml` | the `[dependencies]` entries that come from a registry (a `path`/`git`/`workspace` entry is not a hub member; a rename resolves to its `package`), each with `features` and `default-features`; `[dev-dependencies]`/`[build-dependencies]` are not hub members because gwz-cli's BUILD declares no `rust_test` | `tomllib` |
| root `Cargo.lock` | the version each direct dependency resolved to, read off the cli package's **own edge** (`name`, or `name version` when the lock holds two versions of it -- an unspelled edge into an ambiguous lock is an error, not a guess) | `tomllib` |
| `gwz-cli/BUILD.bazel` | the `@crates//:<name>` labels the crate consumes (`--no-cli-build` leaves them out) | a regex over labels |

**Refuses (exit 1)** when a spec's version is not the lock's (either side
moving), when it is not an exact `=x.y.z` pin, when a feature set or
`default_features` differs from the manifest, when a direct registry
dependency has no spec (added), when a spec names no direct dependency
(removed), when the lock has no edge to a declared dependency, when a spec
is unscoped or shared with another hub (it would inject itself into
gwz-core's splice), and when the BUILD labels drift from the spec set.
**Errors (exit 2, never a pass):** a missing or unreadable input, an
unparseable `MODULE.bazel`, a hub that is not declared, an ambiguous lock
edge. Every finding names the file and line.

**Negative fixtures** (`test_check_bazel_pin_drift.py`, 15 rows, all
synthetic four-file trees in a temporary directory so the checker, not Bazel
or cargo, is the rejector): `test_bumped_version_is_rejected_from_either_side`
(the lock moved under the pin; the pin moved off the lock),
`test_changed_feature_is_rejected` (a feature the manifest gained, one the
pin dropped, `default-features = false` without `default_features = False`,
and the matching pin passing), `test_added_direct_dependency_is_rejected`
(with and without the lock edge), `test_removed_direct_dependency_is_rejected`,
`test_a_dependency_the_lock_does_not_describe_is_a_finding`,
`test_inexact_pin_is_rejected` (`"4.6.6"`, `"^4.6.6"`, `"=4.6"`, `">=4.6.6, <5"`),
`test_unscoped_and_shared_specs_are_rejected` (and another hub's spec
ignored), `test_build_labels_must_equal_the_spec_set` (both directions, and
`--no-cli-build`), `test_missing_or_unparseable_input_is_an_error_not_a_pass`
(each of the four inputs, an absent root, a truncated `crate.spec(`, a
missing `from_specs`). Positive: the real tree (skipped with its reason on a
checkout with no gwz-dev root), a synthetic tree, a renamed dependency,
path and dev dependencies not hub members (a pin for `gwz-core` refused as
naming no dependency), two lock versions of a direct dependency resolved by
the spelled edge, and the parsers on the real shapes.

**Proof it bites**, on the tree as it stands (each edit reverted, `git diff
--quiet` and a byte comparison against a saved copy confirming it):

| Edit | Result |
|---|---|
| none | `bazel pin drift: ok` -- 3 pins, 3 direct registry dependencies, 3 BUILD labels, lock version 4 with 188 packages |
| `MODULE.bazel` clap `=4.6.6` -> `=4.6.7` | exit 1: `MODULE.bazel:95: crate.spec 'clap' pins 4.6.7 but Cargo.lock resolved 4.6.6 (version drift)` |
| `MODULE.bazel` clap loses `"wrap_help"` | exit 1: `crate.spec 'clap' features ['derive'] differ from gwz-cli/Cargo.toml's ['derive', 'wrap_help'] (feature drift)` |
| `Cargo.lock` console `0.16.4` -> `0.16.5` (a pretend `cargo update`) | exit 1: `MODULE.bazel:104: crate.spec 'console' pins 0.16.4 but Cargo.lock resolved 0.16.5 (version drift)` |

**Where it lives, and where it belongs.** It is in
`gwz-core/scripts/checks/` as briefed, resolving the gwz-dev root from its
own location exactly as `check_merge_docs.py` does (`parents[3]`; every path
overridable). Honestly: it does not belong to gwz-core. Its inputs are two
root-repo files and two gwz-cli files, and the events that produce drift --
a `cargo update` at the root, a dependency bump in gwz-cli, an edit of
`MODULE.bazel` -- never touch gwz-core, so gwz-core's CI can never run on
the commit that introduces a drift; and if the job fetched the sibling
repositories it would check their `main`, not the tuple the root's
`gwz.conf` pins, and put a red on gwz-core pull requests for a drift made
elsewhere. The principled home is the **root repository**, owner of
`MODULE.bazel` and `Cargo.lock` and the only checkout where every input
exists by construction. The root has no `scripts/` and no CI today, and
`gwz-core/scripts/checks/` already houses two other workspace-root gates for
the same reason (`check_merge_docs.py`, `check_m4_scenario_map.py`, both
"NOT wired here" in the CI file and run in the landing gate set), so the
gate stays beside them until the multi-repo checkout of the R2-D settled
tuple §11.3 item 7 / R2-F CI work lands. **What the lane owner must move
then:** the two files to `gwz-dev/scripts/checks/` (`DEFAULT_WORKSPACE_ROOT`
and the test's `WORKSPACE_ROOT` become `parents[2]`), the invocation into
the root's gate, and the CI step below out of gwz-core (or left, for the
unit tests). Until then it is a landing-gate-set command, run from gwz-core
inside the workspace: `python3.13 scripts/checks/check_bazel_pin_drift.py`.

**CI wiring** (gwz-core `.github/workflows/checked-artifact-boundary.yml`,
job `local-clone-boundary`, the job that runs the boundary checkers): a
step running `python -m unittest scripts/checks/test_check_bazel_pin_drift.py
-v`. The live check is **not** wired there and is deliberately not guarded
by a file-exists test -- the single-repo checkout has none of its inputs
(the L2-05 blocker class), and a gate that passes when its inputs are
absent is no gate; the step's comment says so. The real-tree row skips
naming the reason on that runner and runs here. `MODULE.bazel`'s comment
and gwz-cli's `BUILD.bazel` comment now name the gate beside the `grep` they
used to offer.

**Coverage limits, stated.** The gate compares declared pins with the
lock's resolution of the *direct* dependencies; it does not compare the
transitive graph cargo-bazel resolves into `MODULE.bazel.lock` with
`Cargo.lock`'s (the two are resolved separately by design, per the
MODULE.bazel header), and a `rust_test` added to gwz-cli's BUILD would need
the hub to carry `[dev-dependencies]` too, which the gate would then have to
learn (documented in its docstring).

### 15.2 The family record's git privacy, enforced

**What held, and how.** The record -- `<root>/.gwz/local-family.yml` and
its lock, `<clone>/.gwz/family-root` and `<clone>/.gwz/local-clone-allocation`
-- was ignored by git because `/.gwz/` sits in the root repository's
`.git/info/exclude` managed block, which `workspace_ops::ensure_workspace_exclude`
regenerates on every *other* mutation verb (local, never committed), and
because a verbatim copy carries `.git/info/exclude` along. The create wrote
it nowhere: not at the destination, not at the root it founded the index
in. Nothing tested any of it.

**Could it have leaked?** For the shipped verbatim mode from a root that
gwz created or any gwz mutation verb had touched: **no** -- the block was
there and the copy carried it (measured now: A's exclude byte-identical to
the root's, the block once). Two ways it could have, both reproduced as the
preconditions of the new rows and both now closed:

1. **A stripped source.** With A's `.git/info/exclude` removed, `git
   status` at A lists `.gwz/family-root` and `.gwz/local-clone-allocation`,
   and a clone B of A inherited no block (the copy carries what the source
   has). A member's exclude is regenerated by nothing but the verbs run
   *at A*. This is exactly the state a **constructed** destination (clean
   LCM3.1, bare LCM2.3) would have been in from birth: `gwz-repo-factory`
   builds a fresh repository and inherits nothing.
2. **The founding root.** With the root's exclude removed, founding wrote
   `.gwz/local-family.yml` into a root that showed it as untracked -- one
   `git add -A` at the root from a commit -- and the create, the only verb
   that writes at the root without going through `sync_workspace_boundary`,
   did not regenerate the block.

Never at risk, and now proven rather than assumed: the tracked
configuration. `gwz.conf/gwz.yml` at a destination is the source's bytes
(§13), `gwz.conf/gwz.lock.yml` names commits and branches, the
conf-integrity marker names digests; the family id, the member name, the
recorded path and every host path live only in `.gwz/`. And no family
binding is a git remote: install strips path and `file:` URLs (§4.1's last
row, `gwz_repo_factory::origin_is_kept`), so `capture` and `repo sync`
have nothing family-shaped to record.

**What changed** (gwz-core `63f1332`):

- `local_clone/adapters/git_config.rs::ensure_managed_exclude(backend,
  workspace, manifest)`: the managed block through the existing helper --
  `read_lock_or_empty` for the lock when the workspace holds one, then
  `workspace_ops::ensure_workspace_exclude` (idempotent, operator lines
  preserved, never committed). A workspace with no `.git` is refused
  `InstallPortError::Configuration` ("no root repository to hold the
  managed exclude block") rather than given a stray `.git/info/`; a bare
  root (design §4.3) has a `.git` and no working tree, so the block is
  written inside it and hides nothing, harmlessly. One rule, two callers:
- `CoreInstallPorts::install_destination_git` runs it at the destination
  in **every mode**, after the remote-URL strip and before the pointer
  (the port order the library fixes). A verbatim copy inherited the file
  and the write is a no-op; a constructed destination gets the block it
  would otherwise never have. `CoreInstallPorts` is now
  `CoreInstallPorts<'_, B: GitBackend>` holding the handler's backend.
- `create::clone_local` runs it at the **family root** under the family
  lock, after `reread` and before the index is founded or rewritten, on
  every create (the root's manifest is the capture's when the root is the
  copy source, read when the source is a clone). A failure refuses with
  `io_error` (`install_port_code(Configuration)`) and "nothing was
  reserved": nothing founded, no row.
- `handle_clone_local_workspace`'s `_backend` (unused since LCM1.1) is now
  passed through `clone_local<B>` to the ports; the six test call sites
  pass `Git2Backend::without_credential_helpers()`.

**What the tests now prove** (`local_clone::tests::privacy`, a new Tier B
slice on real workspaces built with `gwz-local-testrepo` and the public
handlers; 5 rows, 0.86 s warm; plus one unit row in `git_config`):

| Row | What it asserts |
|---|---|
| `every_family_file_is_ignored_by_the_destination_and_the_root_after_a_create` | after `root -> A`, at A and at the root: each of the four family paths, `.gwz`, `.gwz/locks/workspace.lock` and `.gwz/merge/open.yaml` is ignored (`is_path_ignored`, present or not); no `.gwz` entry in `git status` (untracked included, ignored excluded) nor in the index (`git ls-files`); the block exactly once, with `/.gwz/` and `/app/`; the member's own repository holds none; an operator line added to the source's exclude survives in both; A's exclude is byte-identical to the root's (the install's write is a no-op on an inherited block) and the root's is what the operator left (regenerating an identical block writes nothing) |
| `the_tracked_configuration_carries_no_family_binding` | the pointer holds the family id and the root path and the index holds the family id and `../root-A` (the data exists, confined to `.gwz/`); every file under `gwz.conf/` at the root and at A contains none of: the family id, `../root-A`, `family`, the root path, A's path, the temporary tree's path; no recorded remote URL names one; A's manifest is the source's byte for byte |
| `a_destination_gets_its_exclude_from_the_install_not_from_the_source` | `root -> A`; A's exclude removed (precondition asserted: A's pointer is not ignored and `git status` at A lists the record); `A -> B`: B ignores the record with the block once and `/app/`, while A still has no exclude -- the block came from the install, not the copy |
| `every_create_regenerates_the_roots_exclude` | the root's exclude removed (precondition: the index path is not ignored); `root -> A` founds a family whose index is ignored, the block naming `/app/`, and A inherits it; removed again, `A -> B` (source A, not the root) regenerates it once more; B ignores the record |
| `no_family_binding_is_a_git_remote_and_capture_and_repo_sync_write_none` | the source member carries an https origin, a filesystem-path remote at a sibling checkout (the shape a family binding would take if it were a remote) and a `file:` mirror; at A no remote URL of the root or `app` is `file:`, absolute, or names the sibling, the root or A; `origin` keeps its https URL; `remote.sibling.url` and `remote.mirror.url` are gone from A's config while their fetch refspecs stay; the source keeps all three; then `handle_capture` and `handle_repo_sync` on the root and on A leave `gwz.conf/` free of the family id, `../root-A`, `family`, the root path and A's path, and the record still private. The URL rule itself is `git_config::tests::filesystem_and_credential_urls_go_and_ordinary_origins_stay` and is not duplicated; this row proves the port runs on every repository of a real destination |
| `git_config::tests::the_managed_exclude_block_needs_a_root_repository_and_is_idempotent` (unit) | a directory with no `.git` is refused typed and gains none; an existing repository gets the block once with the operator's lines kept, a second run changes nothing, and the four family paths plus `app/anything` are ignored while `README` is not |

Already covered elsewhere and not duplicated: the helper's block semantics
(`workspace_ops::tests::g12::exclude_*`: member paths, idempotence, user
lines, the lock/manifest union) and `.git/info/exclude` being in the copy
set (`adapters::exclusions` row).

**Two observations from the remotes row.** libgit2 lists a remote by its
`url`/`pushurl` keys, so once install strips them `sibling` and `mirror`
vanish from `git remote` while their fetch refspecs stay in config; `repo
sync` at A therefore reports `app` `Ok` and records only `origin` (no
empty-URL row reaches the manifest's validator). And `repo sync` at the
**root** records the operator's own `file:///srv/mirrors/app.git` mirror
into `gwz.yml`, as it always has for git config -- the operator's binding,
not the family's, so the row's markers name only family workspaces.

### 15.3 Gates (final trees; Darwin 25.6.0 arm64, cargo 1.95.0, python3.13, from gwz-core unless noted)

| Gate | Result |
|---|---|
| `cargo fmt --all -- --check` | clean |
| `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features --locked -- -D warnings` | clean |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | ok (24 visible entries, 9 classified modules) |
| `python3.13 scripts/checks/check_local_clone_boundaries.py` + `-m unittest scripts/checks/test_check_local_clone_boundaries.py` | ok (14 packages, 39 edges); 23 OK |
| `python3.13 scripts/checks/check_bazel_pin_drift.py` + `-m unittest scripts/checks/test_check_bazel_pin_drift.py` | ok (3 pins, 3 dependencies, 3 labels); **15 OK** |
| `python3.13 protocol/regen.py --check` | OK (the protocol did not move) |
| `cargo test -p gwz-core --lib --locked local_clone` | **60 passed** (was 54; +6) |
| Tier A of touched crates | none touched (every edit is under gwz-core `src/`, `scripts/`, `.github/`) |
| lib remainder census (`--list`) | 1793 rows (was 1787); `checked_artifact::` 459 and `v1_lifecycle::` 266 unmoved; remainder 1068 listed = **1067 executed darwin**, and **MEASURED**: `cargo test -p gwz-core --lib --locked -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::` -> `1067 passed; 0 failed; 1 ignored` in 54.7 s; linux 1068 DERIVED (+6, no cfg gate) |
| `run_r4bg_aggregate_gates.py` | re-pinned 1061/1062 -> **1067/1068 in the same commit as the rows** (`63f1332`); `--list` still parses |
| `bazel query //...` (root, after the `MODULE.bazel` comment edit) and `bazel query //gwz-cli/...` (after the BUILD comment) | both resolve; no full `bazel build` (disk 8.4 GiB free, not needed) |
| gwz-cli | comment-only edit; no cargo target changed |
| `PYTHON=python3.13 scripts/checks/check_lane_commits.sh 8bb8804 HEAD` | `lane gate: ok` at `603d628` and `63f1332` |

Not run: the 16-minute probe suites; Windows.

### 15.4 Pins moved (old -> new)

- `gwz-core/scripts/checks/run_r4bg_aggregate_gates.py`: lib remainder
  1061 / 1062 -> 1067 / 1068 (census and measured; +6 rows, all
  `local_clone::`, none cfg-gated; dated reason in the docstring).
- No protocol pin moved (no message changed; `regen.py --check` OK).

### 15.5 Left uncommitted for the lane owner (root repo)

`MODULE.bazel` (the "check with" comment now names the gate and its home;
a comment only -- `bazel query //...` resolves) and this record (§15). The
member pins to record through `gwz`: gwz-core `63f1332`, gwz-cli `8fd6df6`,
gwz-py `88f8d2b` (unchanged). No root `Cargo.lock` change.

### 15.6 Residual risks and what the next lanes must know

- **The gate's home** is the root repository; until the root has a gate
  set it is a landing-gate-set command from gwz-core (§15.1). A lane that
  bumps a gwz-cli dependency or runs `cargo update` at the root must run it
  and re-pin `MODULE.bazel` (then `bazel mod deps --lockfile_mode=update`).
- **Clean and bare** (LCM3.1 / LCM2.3) get the destination's block for free
  through `install_destination_git`, which the port order already runs
  after construction; the construction lane must not reorder it. A bare
  root's block sits inside a bare `.git` and hides nothing -- if the bare
  layout ever gains a working tree at the root, the block is already there.
- **The root's block is regenerated by every create, not by `dispose` or
  `disband`**, which remove record files rather than add them; a root
  whose exclude is stripped between creates is healed by the next create
  or any other mutation verb, as member paths always were.
- **`remote.<name>.fetch` survives the URL strip** at a destination.
  libgit2 lists a remote by its URL keys, so `gwz` and `repo sync` no
  longer see it (measured: `repo sync` at A is `Ok`); plain `git remote`
  still lists the section, and `git fetch <name>` there fails with
  "'<name>' does not appear to be a git repository" -- the same message as
  for a remote that never existed (measured on a scratch repository).
  Harmless; whether install should drop the whole remote section rather
  than its URL keys is a product call for the lane owner.
