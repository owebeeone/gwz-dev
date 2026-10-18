# GwzLocalClone — PLAN / ARCHITECTURE REVIEW: parallel-implementation readiness (F51)

**Review objects (working tree, 2026-09-05):**
`dev-docs/GwzLocalCloneDesign.md` revision 7, SHA-256
`5fe266c7b947f147f3522c8f857d2ec4cb4fed77fd9e09f876b42b2c4a8ec3a8`;
`dev-docs/GwzLocalClonePlan.md` revision 3, SHA-256
`64579bdec8c3a0341f4403d97206c9293b5aee76a1a60dc113f5de58a4d4c873`;
`dev-docs/GwzLocalCloneImplementationArchitecture.md` revision 2, SHA-256
`65b4a9a0dcd4ca7ba092235af91d719f0182c19c2825294a8dbb3604b4eba6ae`.
The design is modified against its last commit (`5d43a0d`); the plan and
architecture are untracked. The SHAs above identify what was reviewed.
**Baseline:** gwz-dev `27cc486`; gwz-core `87207c2`; gwz-cli `c04b488`;
gwz-py `65fc667` — identical to the plan §2 tuple.
**Date:** 2026-09-05. **Reviewer:** Fable 5.1 (F51), independent.
**Axis:** readiness for parallel implementation — one owner per library,
explicit interfaces and dependencies, independently runnable fast tests,
whether the packaging enforces that isolation, fast versus integration
tiers, and runtime targets versus measured results.
**Independence and method:** the other `GwzLocalClone-ReviewPlanArch-*`
reports and the Summary were not opened. No input was edited. Read-only
against the tree plus cheap checks: `git rev-parse`, `shasum`, `grep`, the
taut schema, the boundary checker's pin tables, two builds of the existing
gwz-core test binary (no suite run; one mtime-only `touch` of
`gwz-core/src/lib.rs` to force an incremental rebuild — `git status` clean
afterwards), and a scratch replica of the Cargo layout to verify per-crate
test behaviour. Scope respected: no correction below adds filesystem
durability, catalog, identity, journal or recovery machinery.

**Verdict: NO-GO for opening parallel LCM1.1+ lanes on the documents as
they stand** — 0 P0 / 0 P1 / 4 P2 / 5 P3. **GO now** for the three LCM1.0
spikes plus the fixture crate (§5, wave W0); their outputs *are* the
corrections. **GO for the W1 lanes** once the one-page interface
checkpoint and the one skeleton commit in §4 land. Estimated cost of the
corrections: 1–2 focused days, inside the plan's LCM1.0 budget.

---

## 0. Claims verified against the tree

| Claim | Where | Result |
|---|---|---|
| Plan §2 HEAD tuple (lines 81–86) | `git -C <repo> rev-parse HEAD` × 4 | **holds**, all four exact |
| Plan §2 design SHA `5fe266c7…` (line 89) | `shasum -a 256` | **holds** |
| Design §7 live numbers: `clone_workspace=19` unchanged, 27/28 free, `MergeRequest` F(8)=`filesystem_strict` and F(9) free, `PushRequest.remote`=F(2), `OperationPolicy.remote`=F(5), `CloneWorkspaceRequest.url`=F(2) required, `PullHeadRequest` carries only `meta` | `gwz-core/protocol/gwz.taut.py:142-169, 818-830, 1448-1453, 1616-1617, 1625-1630, 1661-1676` | **holds**; plan 1.0c's "verify allocation" can record this SHA |
| Plan §2 six seams exist (lines 95–99) | `gwz.taut.py` 2083 lines; `clirequest/workspace.rs` 113; `transport.rs` 166; `merge/plan.rs` 440; `merge/start.rs` 99; `artifact/conf_integrity.rs` 505 | **holds** |
| Plan §2 `transport.rs` = "anonymous local fetch with explicit import refspec" (line 97) | `transport.rs:45-58` (`fetch` → `find_remote(name)`, empty refspec list), `:148-160` (`push` → `find_remote(name)`); trait `git/gitbackend/contract.rs:209,213,741` name-based only | **does not hold** — no anonymous-path transport exists (P2-3) |
| Design §3.2 / Arch §7: no change to `WorkspaceMutationGuard`, `V1MutationLease`, lock handoff | `merge/runtime/dispatch.rs:194-204` takes the start guard; `merge/start.rs:73-79` drops it before the non-reentrant lease; `handle_merge_with_events` is `pub` (`dispatch.rs:125`) and both drivers call it (`gwz-cli/src/globalargs/dispatch.rs:198`, `gwz-py/native/src/dispatch/merge.rs:67`) | **holds** if the family wrapper calls that public entry (note N3) |
| Plan §4: protected code checks apply only "when a protected seam is actually touched" (lines 274–277) | `scripts/checks/check_checked_artifact_boundaries.py:20-46` pins `src/lib.rs`, `src/workspace_ops/mod.rs`, `src/workspace_ops/merge/mod.rs`; flat pins include `git/gitbackend.rs`; `check_lane_commits.sh` runs the checker per commit | **understated** — every module/handler addition touches a pin (P2-2, P2-3) |
| Arch §2: "each has focused tests runnable without a CLI"; separate packages "not a prerequisite" (lines 37–40) | one crate, 656 files, 206,513 lines, 1,729 unit tests in one binary; measured §1 | **holds for speed on a warm tree; isolation unenforced** (P3-1) |
| Arch §9 figures are expectations; "No implementation or native-platform acceptance tests were run" (lines 330–349) | §9 | **correctly labelled**; no test-time target or measurement anywhere (P3-2) |
| Nothing pre-landed: `UnknownLocal`, `refs/gwz/local-imports`, `local_family`, `family_root`, `refcopy` | `grep -r` over gwz-core/gwz-py | **holds** — planning only |

## 1. Measurements (this host, gwz-core `87207c2`; **measured, not targets**)

| What | Command | Result |
|---|---|---|
| Warm no-op build of the lib test binary | `cargo test -p gwz-core --lib --no-run` | 0.17 s; binary 59 MB |
| Incremental rebuild after an mtime bump of `src/lib.rs` | same | **5.4 s** wall |
| Filtered run of one existing module | `cargo test -p gwz-core --lib artifact::conf_integrity` | 12 tests, 0.20 s test time, **1.2 s** wall; 1,717 filtered out |
| Path-crate behaviour, scratch replica (`outer/app/crates/leaf`) | `cargo test -p leaf` | runs only `leaf`'s tests, both under a root workspace (auto-member) and in a standalone package checkout |

Reading: on a warm tree the single-crate inner loop is seconds, not
minutes, so the case for separate crates is enforcement of dependency
direction, not speed (P3-1). A fresh worktree pays the full dependency
build once regardless of packaging. The whole-crate suite remains the
default in CI (`platform-matrix.yml:43`, `windows-matrix.yml:37`), and no
document names a smaller command for a lane (P3-2).

## 2. Findings

### P2-1 — No ownership manifest and no frozen interfaces for six of nine libraries (L1-05 / L1-06 exit unmet)

- **Location.** Arch §2 table (lines 43–53) has no owner column and no
  file manifest. Signatures exist only for `refcopy` (§3 lines 76–82),
  `repo_inspect`/`work_detector` (§4 lines 132–137) and `history_check`
  (§5 lines 176–179). `family_model`, `family_store` (§6), `local_import`
  (§7), `workspace_install`/`repo_factory` and `local_disposal` (§8) are
  prose. Named but undefined owner types: `CopyReport`, `CopyError`,
  `RepositoryInfo`, `WorkObservation`, `ProtectedRoots`, `WorkReport`,
  `Verified(coverage)`, `Unpreserved(items)`, `Unknown(reasons)`, and the
  `gwz_evidence` input (§4 line 136; §4 line 163 "Core supplies decoded
  GWZ merge/stash evidence"). "Small ports" for Git (§2 line 69) are
  unnamed. Plan §2 lines 102–106 assign the family files "one small core
  owner" but no lane; plan §3 has no lane column.
- **Invariant violated.** L1-05: "each lane has a disjoint ownership row
  and compiles against the frozen interface."
- **Failure path.** `work_detector` and `history_check` both consume
  `repo_inspect` output; without a frozen `RepositoryInfo`/`ProtectedRoots`
  each lane stubs its own. At LCM2.2 the deletion gate composes both: the
  same repository is `Known` on one path and `Unknown` on the other, and
  the refusal decision becomes lane-dependent — a diagnosability defect
  that no single lane's tests can see.
- **Correction (smallest).** One page — an owner-lane column plus, per
  library, "files it may change / may only call / must not absorb"
  (L1-06); frozen Rust signatures and owner types for the six missing
  libraries; the evidence value type defined in `work_detector` and filled
  by core (plain data, no core types in the leaf); integration order. Land
  it as the output of 1.0c, before any 1.1 lane starts.
- **Closure evidence.** Disjoint rows; the skeleton (P2-2) compiles with
  stub bodies against those signatures.

### P2-2 — Three digest-pinned compiler-root files are a shared serialization point the plan does not name

- **Location.** `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:20-46`
  (`PROTECTED_COMPILER_ROOT_DIGESTS`: `src/lib.rs`,
  `src/workspace_ops/mod.rs`, `src/workspace_ops/merge/mod.rs`), enforced
  per commit by `scripts/checks/check_lane_commits.sh` from
  `.github/workflows/checked-artifact-boundary.yml:25`. Every new top-level
  module is a `pub mod` line in `src/lib.rs:16-36`; every new handler is a
  `mod handle_*` plus `pub use` in `src/workspace_ops/mod.rs:1-42`. Plan §4
  lines 274–277 treats a protected touch as incidental.
- **Invariant violated.** Per-commit green lane gate (the checker's own
  rationale; deviations 95d292f/b923109 are the recorded precedent).
- **Failure path.** Lane A adds `mod handle_local_clone;` and re-pins the
  digest; lane B adds `mod handle_local_family;` on a branch carrying the
  old pin. B's commits are red under `check_lane_commits.sh` (each commit's
  exact tree), and after rebase the two dated re-pins conflict textually
  and in their recorded reasons. Multiply by every lane that adds a module.
- **Correction (smallest).** One skeleton commit in LCM1.0c by the lead
  lane: `pub mod local_clone;` in `lib.rs`; `mod handle_local; pub use
  handle_local::*;` in `workspace_ops/mod.rs`; the new `OperationRequest`
  variants in `operation/push_event.rs:96` (unpinned); the regenerated
  protocol; empty submodules for all nine libraries; **one** dated digest
  re-pin. Every other lane's ownership row states: no edits to
  `src/lib.rs`, `src/workspace_ops/mod.rs`, `src/workspace_ops/merge/mod.rs`,
  `src/git/gitbackend.rs`. The family-merge interception does not need
  `merge/mod.rs` at all (note N3).
- **Closure evidence.** The checker passes on the skeleton commit; no later
  lane commit changes a pinned digest.

### P2-3 — The "anonymous local fetch" seam does not exist; adding it crosses a pinned file and a test double

- **Location.** Plan §2 line 97 and design §6.2 lines 466–467 assume an
  anonymous local transport with explicit refspecs. `transport.rs:45-58`
  resolves a *named* remote (`find_remote`) and fetches with an empty
  refspec list; `push` (`:148-160`) likewise; the `GitBackend` trait
  (`git/gitbackend/contract.rs:209,213,741`) is name-based only; the
  `Git2Backend` delegates live in the **pinned** `git/gitbackend.rs:77,132`;
  the trait has two implementors (`gitbackend.rs`,
  `workspace_ops/tests/g01/tracking_backend.rs`).
- **Invariant violated.** Design §6 lines 438–440 / §6.2 line 499: family
  bindings are never persisted as git remotes — so a name-based transport
  cannot serve them; and P2-2's pin discipline.
- **Failure path.** The LCM1.2 lane adds `fetch_anonymous` to the trait,
  edits the pinned delegate file and the test double; the LCM2.4 lane adds
  `push_anonymous` to the same three places — a second re-pin and
  conflicting trait/double edits. The alternative — direct `git2` calls
  inside `local_import` — bypasses `GitBackend`, so the tracking double
  cannot observe family transfers and parity tests cannot inject failures.
- **Correction (smallest).** In the same skeleton commit (P2-2): trait
  methods `fetch_anonymous(path, url, refspecs: &[&str]) -> GitFetchResult`
  and `push_anonymous(path, url, refspec) -> GitPushResult`, their
  `transport.rs` bodies (`Remote::create_anonymous`, explicit refspecs, no
  credential helpers — local paths only), the two `delegate!` lines, and
  the `TrackingBackend` implementation. Quote the two signatures in design
  §6.2 so 1.0b tests the real port.
- **Closure evidence.** 1.0b's adapter check runs through the port;
  `impl GitBackend for` count stays at two; one re-pin total.

### P2-4 — Family-name resolution is owned by no library, and its three call sites are absent from the plan

- **Location.** Design §6 lines 416–427 (pull/push: `ready` family name →
  bind; absent → git remote; neither → `missing_remote`; non-ready →
  lifecycle refusal) and lines 433–436 (merge: family-only, `UnknownLocal`).
  Arch §2 line 49 gives `family_model` only "decide whether a requested
  local operation may proceed". Code sites: merge —
  `merge/runtime/dispatch.rs:194-204` (before `guarded_workspace_root`);
  push — `push_member.rs:369-385` (`resolve_root_push_remote`) and
  `:397-412` (`resolve_push_remote`), both reading `request.remote` then
  `meta.policy.remote` then git remotes; pull —
  `pull_head_merge_preflight.rs:378` (`root_remote_name`) after
  `pull_head_member_preflight.rs:45` has already taken the guard. Plan
  stages 1.2, 2.4 and 3.3 each say "resolve names" without a shared
  function or file.
- **Invariant violated.** L2-03 cross-driver/cross-verb parity; design
  §2 lines 60–63 and §6 lines 411–414 (a non-ready row never falls through
  to a same-named git remote).
- **Failure path.** Three lanes write three resolvers. `gwz push --remote
  A` while A is `creating` falls back to a git remote named `A` in one
  resolver and refuses in another; `missing_remote` versus `UnknownLocal`
  becomes verb-dependent in a way the §7 table (lines 555–575) forbids.
- **Correction (smallest).** One pure function in `family_model`,
  `resolve_remote_token(index_view, token, verb: Merge | PullPush) ->
  Bound(member) | GitRemote(name) | LifecycleRefusal(state) | UnknownLocal |
  Missing`, frozen in the checkpoint; the three verb lanes call it and own
  only the call-site files above.
- **Closure evidence.** The §7 CLI→message table executed as a table test
  against the function in Tier A.

### P3-1 — The packaging leaves isolation unenforced

- **Location.** Arch §2 lines 37–40 ("path crate if useful … separate
  Cargo packages … not a prerequisite"); plan §2 lines 105–106. gwz-core
  is one crate (`gwz-core/Cargo.toml` `[lib] path = "src/lib.rs"`). Nothing
  in one crate stops `use crate::checked_artifact::…` from inside
  `history_check`; the mermaid graph (lines 55–67) is prose and omits
  `workspace_install`/`repo_factory` and `local_disposal`, so their allowed
  dependencies are unstated.
- **Consequence.** Reviewers must re-read every lane's imports; the
  checker's digests do not cover new modules; a lane can couple to the
  checked-artifact subsystem the design excludes (§1 lines 11–14) without
  any gate noticing.
- **Correction (smallest).** The six leaf libraries as path crates under
  `gwz-core/crates/<name>` — `refcopy`, `repo_inspect`, `work_detector`,
  `history_check`, `family_model`, `family_store` — with `gwz-core`
  depending on them and none depending on `gwz-core` (Cargo rejects the
  cycle). Verified (§1): `cargo test -p <leaf>` runs the leaf alone under
  the gwz-dev root workspace and in a standalone gwz-core checkout. The
  three integration modules stay in gwz-core under `local_clone/`. Add the
  two missing nodes and their edges to the diagram.
- **Closure evidence.** `cargo metadata` lists the six packages; no leaf
  `[dependencies]` names `gwz-core`; each `cargo test -p <leaf>` is green
  in isolation.

### P3-2 — Fast and integration tiers, the time budget and the measurement points are not separated

- **Location.** Arch §2 "Test independently with" column (lines 45–53)
  mixes pure inputs with "competing processes" and "existing merge tests";
  plan §4 lines 266–271; plan 1.0a lines 118–124 (smoke test, no timing);
  plan 3.3 line 263 (the only measurement, scheduled last); arch §9 lines
  330–349 (expected costs, labelled; no test-time figure anywhere). Python:
  `gwz-py/run_tests.py:84-91` is `maturin develop` plus the full pytest run
  against a freshly built gwz-cli — no fast tier exists for the driver
  lane, although `scripts/regen_protocol.py`, `scripts/check_protocol_drift.py`,
  `gwz-core/tests/protocol.rs::generated_protocol_is_current` and
  `src/tests/test_protocol.py` already form one.
- **Consequence.** Lanes default to the whole-crate suite or skip tests;
  "independently runnable in seconds" is unverifiable because neither a
  target nor a measurement exists.
- **Correction (smallest).** Define the tiers by command: **Tier A** =
  `cargo test -p <leaf>` (no gwz-core, no process spawn, git2-built
  micro-repositories allowed, native-copy smoke behind `#[ignore]` with an
  explicit skip message), target ≤ 10 s warm per crate; **Tier B** =
  `cargo test -p gwz-core --lib local_clone::` (real workspaces, existing
  engine) and `gwz-py/run_tests.py` (cross-driver), budget stated
  separately. Record wall times at the 1.0 exit and each milestone exit as
  "measured on <host>, <SHA>"; give arch §9 a "measured" column that stays
  empty until then. Move the copy-performance measurement from 3.3 into
  1.0a's exit, where a representative tree is already copied.
- **Closure evidence.** Per-lane Tier A commands listed; measured lines
  present with host and SHA.

### P3-3 — `family_store`'s lock and write primitives are unresolved, and two of the three existing helpers are fenced

- **Location.** Design §3.1 lines 123–125 ("reuse available ordinary
  flush helpers"), arch §3 lines 114–115 ("available flush helpers") and
  arch §6 lines 228–229 ("normal temporary write/rename"); arch §2 line
  50. Existing:
  `AdvisoryLock` is `pub(super)` at
  `checked_artifact/bootstrap/runtime/advisory.rs:5`, under the pinned
  `bootstrap/runtime/mod.rs`; `verified_write::write_atomic_verified` is
  `pub(crate)` with a whole-source single-caller pin
  (`check_checked_artifact_boundaries.py:517, 1724-1744` — any new caller
  fails); `durable_fs::{rename_durable, rename_noreplace, sync_dir}` are
  `pub(crate)` (`durable_fs.rs:7,20,29`), inventoried only under
  `v1_lifecycle/`, so callable from a gwz-core module but not from a leaf
  crate.
- **Consequence.** The lane either edits pinned files to widen
  `AdvisoryLock`, trips the writer pin, or invents a lock whose
  stale-after-crash behaviour contradicts "refusing busy" (design §3.2
  line 161) without saying so.
- **Correction (smallest; adds no durability machinery).** `family_store`
  owns a ≤ 40-line std-only implementation in its crate: `flock` /
  `LockFileEx` try-lock on `root/.gwz/local-family.lock` (same shape as
  `advisory.rs`, copied not shared, released with the handle) and
  temp-file + rename + parent sync for the index; arch §6 states it is
  deliberately not the checked-artifact writer. Test with two `File`
  handles in one process (flock and `LockFileEx` conflict per open file
  description), so Tier A needs no process spawn; one spawned-process case
  may stay in Tier B.
- **Closure evidence.** Checker passes with no pinned-file edits;
  busy-refusal test green in Tier A.

### P3-4 — The shared multi-repository fixture builder has no owner, and the existing helpers cannot be reused

- **Location.** Arch §2 rows at lines 46, 48, 51, 52 ("Real small
  repositories", "Multi-repository fixtures", "Shared install fixtures");
  plan 1.0b line 126. Existing helpers are `pub(crate)` inside gwz-core
  test modules (`workspace_ops/tests/g01.rs:375` `write_pull_fixture`,
  `tests/g00.rs:607,638`, `diff/tests/fixture.rs:46` `init_repo`) —
  unreachable from leaf crates, and a dev-dependency on gwz-core would pull
  the whole crate into every leaf test build (defeating P3-1).
- **Consequence.** Five lanes each write a builder; fixtures diverge
  (object format, bare versus checkout, autocrlf and identity pinning —
  see `tests/g02.rs:1462-1475`); `history_check` cases stop reproducing
  `work_detector` cases.
- **Correction (smallest).** One dev-only crate `gwz-core/crates/testrepo`
  (git2-based: init, commit, branch, tag, stash, reflog entry, bare clone,
  nested repository, gitfile, alternates) owned by the 1.0b lane and landed
  at the 1.0 exit; leaf crates take it as a `[dev-dependencies]` entry;
  gwz-core Tier B tests may use it too.
- **Closure evidence.** No second `Repository::init` helper in the leaf
  crates.

### P3-5 — The plan has no prerequisite / lane map, so independent work is serialized behind the MVP

- **Location.** Plan §3 table lines 110–114 and stage text 116–263; §5
  stage days 302–312. No stage lists prerequisites or frozen interfaces.
  LCM2.1 (`work_detector` + `history_check`, 4–6 days, the largest stage)
  depends only on `repo_inspect` and the fixture crate yet is scheduled
  after LCM1.2; 1.1c and LCM1's "Clients agree" exit depend on 1.0c's
  regenerated protocol, which is not named as their prerequisite; 2.3
  depends on 1.1b's installer; 2.4 and 3.3 depend on P2-3's port and
  P2-4's resolver.
- **Consequence.** L1-04's verification ("no package can start without its
  prerequisites") cannot be applied; wall-clock becomes the sum of stages
  instead of the critical path.
- **Correction (smallest).** One table: stage → prerequisites (landed
  stages + frozen interfaces) → lane → Tier A command. A workable wave
  order is in §5.
- **Closure evidence.** The table exists; each row's Tier A command is
  green before the row is claimed.

## 3. Notes (checked; not findings)

- **N1.** `gwz-py/src/gwz/cli_local.py` and `src/tests/test_cli_local.py`
  already exist with an unrelated meaning (client-side forall/clone
  rendering). The driver lane must not name the `gwz local` verb module
  `cli_local`.
- **N2.** The boundary checker fails closed on any new `#[path = …]`
  attribute anywhere under `src/` (`APPROVED_RUST_PATH_EDGES`, 43 entries;
  scan at `check_checked_artifact_boundaries.py:1843-1870`) and on
  `include!` loaders; new modules use plain `mod` declarations. Files with
  a `tests` path segment are excluded from the production scans
  (`production_rust_files`, `:1430-1437`), so lane tests belong in `tests/`
  submodules.
- **N3.** Arch §7 line 269 ("intercept the family request before the
  normal merge mutation guard") is satisfiable without editing `merge/`:
  a function in the local subtree resolves and imports under the family
  lock, rewrites `source_ref` to the import ref, clears
  `local_source_name`, and calls the public `handle_merge_with_events`
  (`merge/runtime/dispatch.rs:125`), which then takes its own guard at
  `:194-204`. The family lock (`root/.gwz/local-family.lock`) and the
  receiver's `V1MutationLease` are different files, so the "no
  self-conflict" claim holds (`merge/start.rs:73-79`).
- **N4.** If the bazel/razel path is still exercised (`MODULE.bazel.lock`
  2026-08-05), each leaf crate needs its own `rust_library`/`rust_test`
  target; crate_universe splices path dependencies automatically, as
  `gwz-core/BUILD.bazel` notes for taut-shape.
- **N5.** The design's §7 numbers, the plan's HEAD tuple and the plan's
  design SHA are all exact against the tree (§0). The `gwz-core` standalone
  CI (`platform-matrix.yml`, `windows-matrix.yml`) runs the whole crate;
  a per-leaf `cargo test -p <leaf>` job is a one-line addition once P3-1
  lands, and is not required for the GO below.

## 4. The interface checkpoint — smallest correction set, in landing order

1. **Checkpoint page** (P2-1, P2-4, P3-2, P3-5): owner/file manifest per
   library; frozen signatures and owner types for the six unsigned
   libraries; `resolve_remote_token`; Tier A/B commands with targets; the
   stage → prerequisite → lane table. One page; no contract change.
2. **Skeleton commit** (P2-2, P2-3, P3-1): the six leaf crates as empty
   packages; `local_clone/` subtree in gwz-core with empty submodules;
   `pub mod` / `mod handle_local` lines; `OperationRequest` variants;
   `fetch_anonymous`/`push_anonymous` on `GitBackend` with `transport.rs`
   bodies, the two `delegate!` lines and the `TrackingBackend` impl;
   regenerated protocol (Rust + Python); **one** dated digest re-pin.
3. **`testrepo` dev crate** (P3-4), landed with 1.0b.
4. **`family_store` primitives** (P3-3): decided in the checkpoint page,
   implemented by that lane in W1.

None of these touches durability, catalog, identity, journal or recovery
behaviour; none changes design revision 7's contract.

## 5. Lane map after the checkpoint (what may actually run in parallel)

| Wave | Lanes (independent within the wave) | Prerequisite | Tier A command |
|---|---|---|---|
| W0 | 1.0c checkpoint + skeleton; 1.0a copy smoke; 1.0b adapter check; `testrepo` | none | `cargo test -p gwz-core --lib local_clone::` (skeleton compiles) |
| W1 | `refcopy`; `family_model` + `family_store`; `repo_inspect`; `work_detector`; `history_check`; driver argument parsing (gwz-cli, gwz-py) | W0 | `cargo test -p <leaf>`; `cargo test -p gwz-cli <filter>`; `pytest src/tests/test_protocol.py` |
| W2 | 1.1b installer (`workspace_install`); 1.2 `local_import` + family merge; 2.2 `local_disposal` | W1 crates of each | Tier B filters |
| W3 | 2.3 bare factory; 2.4 family push; 3.1 clean/branch; 3.2 `--from`; 3.3 family pull | W2 installer / port / resolver | Tier B filters; `run_tests.py` at milestone exits |

LCM2.1's two libraries move from "after MVP" to W1: they are leaf work
with no dependency on the MVP path. The plan's milestone exits (LCM1 →
LCM2 → LCM3) are unchanged; only the start order of the work inside them
is.

## 6. Verdict

**0 P0 / 0 P1 / 4 P2 / 5 P3.** By L1-19, the gate is **NO-GO** while a P2
is open — and on the asked axis the P2s are exactly the missing
preconditions for parallel work: no ownership rows or frozen interfaces
(P2-1), an unplanned shared write point in three pinned files (P2-2), a
transport port that does not exist and crosses a pinned file when added
(P2-3), and a resolver that three lanes would each invent (P2-4). None is
a defect in the best-effort contract itself, which reads consistently and
matches the tree everywhere I checked.

- **GO now** for wave W0 (1.0a, 1.0b, 1.0c, `testrepo`): independent, and
  their outputs are §4.
- **NO-GO now** for opening W1+ lanes on the current documents.
- **GO for W1** once §4 items 1–3 have landed and been checked against
  the closure evidence above.

**Next action.** Lead lane lands §4 items 1–2 as the LCM1.0c deliverable
and requests a short re-review of the checkpoint page against P2-1..P2-4.
