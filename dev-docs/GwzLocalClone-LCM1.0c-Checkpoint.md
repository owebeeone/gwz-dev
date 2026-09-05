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
