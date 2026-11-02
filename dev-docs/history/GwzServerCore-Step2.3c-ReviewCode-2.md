# Step 2.3c round 2 (member lock manager) — Code-AXIS REVIEW
**Review object:** gwz-core fc27ccd6 (on 783b306c) in lane p2-3bc2; evidence 447592e4 rem1
**Baseline:** I read `git -C gwz-core diff 783b306c fc27ccd6` in full and the whole of every file where the boundary moved, at fc27ccd6:
- `operation/memberlockmanager.rs` and its tests;
- `operation/membermutationguard.rs` and `operation/mod.rs`;
- `session_host/{context,served,served/dispatch}.rs`;
- the served tests (`tests.rs`, `tests/member_locks.rs`, `tests/spellings.rs`) and `test_support/spellings.rs`;
- the nine network call sites in `workspace_ops`;
- `scripts/checks/check_member_step_boundary.py` and its unit test, `run_tests.py`, and `.github/workflows/checked-artifact-boundary.yml`.

I read the controlling documents and the prior-round reviews at lane root 57279067. Builds and tests ran in my own copy only:
- source: `rsync -aL` of gwz-core, gwz-transport, gwz-git, git2-rs, gwz-sspi, gwz-credential and taut into `build-targets/review-p2-3c-r2-code-src`;
- target dir: `review-p2-3c-r2-code-target`;
- environment: `TMPDIR=/Volumes/gwztmp`, `GWZ_TEST_GIT=real`, rustc 1.95.0, python3.13 (and 3.11 for the new check's unit test, which is CI's version);
- Mac only.

The copy, the target and my tools have been deleted.
**Date:** 2026-10-31
**Axis:** Code. Module boundary and dependency direction, the call graph from `served/dispatch.rs` to every network ref update, the new source check, the API narrowing, interface changes, and parity.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 5 P3 (all new, none architectural). Every prior 2.3c finding is closed on this tree; I checked each original counterexample. The P3s may be fixed or recorded as carried; none blocks. The Windows half of the parity conclusions depends on receipt `p2-3bc2-20261031-a`, and the Linux deviation (7) depends on WSL `p2-3bc2-20261031-b` (§4).

## 0. Evidence base

**Tuple**, verified at start, after the host restart and at end:
- lane root 57279067 (only the 7 untracked operator drafts in `dev-docs/`);
- gwz-core fc27ccd6, clean;
- gwz-transport 281a28d2, clean;
- gwz-core-evidence 447592e4.

Nothing moved.

**Interruption.** A host restart killed my check-mutation run part-way and left mutation U-R applied in my copy only. I restored that file from the lane. `diff -r` then showed the copy's `src`, its `scripts` and every sibling identical to the lane before any further run, and again after every run.

**Baseline in my copy:**
- the step's groups (`operation::memberlockmanager`, `session_host::served`, `workspace_ops::tests::{g29,g26,g17}`): 98 passed, 0 failed, no `SKIPPED ROW` (the case-variant rows ran);
- the implementer's wider filter (`session_host operation:: workspace_ops::tests remote_lane`): 622 passed, 1 ignored, which matches `raw/2-green-ordinary.log`;
- `clippy --lib --tests --locked -D warnings`: clean;
- `cargo fmt --check`: clean;
- source checks, all exit 0: `check_member_step_boundary`, `check_process_globals`, `check_windows_parity`, `check_target_selection_boundary`, `check_filesystem_boundary`, and `check_cfg_boundaries --skip-repo gwz-cli --skip-repo gwz-py`;
- the new check's unit test: 24 tests OK on 3.11 and 3.13, and 29 tests together with `test_run_tests_parallel_checks`.

**Implementer's mutations re-run in my copy.** I used the drafter's `mutate.py`, retargeted at my copy, with its digest-confirmed restore.

| id | my result | evidence README |
|---|---|---|
| S (no canonicalization) | 5 red (symlink and case rows at manager and served level, plus no-resolve), 10 s | 5 red |
| D (no cancel wake) | 18 red in 51 s, no hang | 18 red in 51 s |
| G (key ignores member) | 2 red in 20 s, no hang | 2 red in 20 s |
| T (tag delete unlocked) | 1 red | 1 red |
| check I, T, X, Z | check exit 1 on each | exit 1 |

**Reviewer mutations.** Rows were run on the harness filters. K2 and K3 were also run on a wider filter that adds all of g26, g17 and the whole served module (98 rows).

| id | edit | check | rows |
|---|---|---|---|
| U-L, U-M, U-O, U-R | `let _guard = steps.enter(..)?;` → `let _ = steps.enter(..)?;` (guard dropped at once) at the pull member fetch, pull root fetch, tag push and tag fetch | **exit 0** | **48/48 green** |
| V | pull root: `if let Err(e) = steps.enter(..) { return Err(e); }` (guard is a temporary) | **exit 0** | not run |
| W | tag delete: step removed and receiver renamed (`let git = backend; git.push(..)`) | **exit 0** | 1 red (the delete row) |
| K2 | `plan_tag_pushes` tags every plan `"@root"` | n/a | **98/98 green** |
| K3 | tag fetch enters `"@root"` for every repository | n/a | **98/98 green** |

**Reviewer row (copy only).** A manager row locks `$HOME/.review-…` and then its macOS firmlink spelling `/System/Volumes/Data/Users/…`:
- the two paths have the same `(dev, ino)`;
- `canonicalize` returns the two spellings unchanged;
- the second `lock` acquired while the first was held.

The row is red. The temporary directory was removed afterwards (§2, P3-N1).

**Not run by me:** WSL `p2-3bc2-20261031-b`, Windows receipt `p2-3bc2-20261031-a`, and the Linux container rows. I read the Linux results from `raw/2-linux-*.log`. Host load during my runs reached about 50.

## Problem, requirement provenance, and scope

**Requirement.** C10 (plan :36 and §3 2.3c; convergence plan :675-680; design §5.1 "Serialization"):
- the host context's member lock manager blocks rather than refuses;
- it recovers from poisoning;
- a wait wakes on cancel;
- sessions sharing a host context never update one member's refs at once.

The round-1 remediation is bounded by the RemPlan dispositions. Every mechanism in fc27ccd6 traces to one of its rows:

| Mechanism | RemPlan row |
|---|---|
| canonical key, refuse on failure | S-1 / P2-1 |
| manager moved into `operation` | P3-C6 / P3-2 |
| `dry_run` and dead push tails removed; new source check | P3-C2 / P3-3 |
| tag delete takes the step | P3-C3 |
| tracking-ref read inside the guard | P3-C4 |
| waiter count | P3-C5 |
| bounded `reply` | P3-4 |
| member id carried with the plan | P3-5 |
| `pub(crate)` | P3-C7 / P3-6 |

**Scope.** All changes are in gwz-core, plus the pin move to gwz-transport 281a28d, which I checked is the single P3-B3 doc sentence on `Wait` in `remote/core.rs`. The served-tests split is necessary under the repo's 1000-line ceiling (1115 lines before, 731 now). Nothing outside the authorized boundary was changed.

**Size.** The non-test `src` diff is +296/−232 lines, inside the step's budget.

**Judgement of the declared deviations:**

1. **Watchdog instead of `recv_timeout` — acceptable.** The in-process `FrameSource` has only `recv` (`crates/session-channel/src/in_process.rs:189`). Adding a timed receive would change a published crate. D and G fail inside their bounds with no harness kill (re-run above).
2. **`contact` inlined — acceptable.** The "enclosing block at its own depth" rule holds (Z is red). But the rule proves that an `.enter(` token is present, not that its guard is live (P3-N2).
3. **Spot check by a 2 s bound — acceptable.** The two remaining sleeps are bounded condition waits, not "it is waiting" windows. One residual: the 200 ms `LockingDispatch` step makes the "never at once" peak rows probabilistic under a lock-less mutant. They are deterministic on the correct tree.
4. **Split — fine.** `served/tests.rs` is 731 lines; `tests/member_locks.rs` is 426.
5. **`plan_tag_pushes` signature — fine.** Its only callers are `handle_tag.rs:136` and `g17.rs:64`. The plan's member id is not pinned by any row (P3-N3).
6. **Linux mutation subset — acceptable on this axis.** The only platform-dependent code is `canonicalize`, and S was in the subset (3 red on Linux; the case rows skip there).
7. **Four Linux failures outside the change — not decidable by me.** They are g13's `read_dir` order and g23's btrfs `NoDurableIdentity`. None reaches a member step (the call graph in §3 does not include them). The judgement depends on WSL `p2-3bc2-20261031-b`.
8. **`pub(crate)` — verified.** The public items removed between 6c219892 and fc27ccd6 are exactly:
   - `MemberLockManager` with `try_lock`;
   - `MemberMutationGuard<'a>`;
   - the `pub use membermutationguard::*` re-export.

   No caller exists in gwz-cli, gwz-py, taut, gwz-transport, `scripts/`, or gwz-core's `tests/`, `examples/` and `crates/`. The rem1 README "Release record" names both breaks.

## 1. Prior-finding closure table

| ID | Disposition | What I checked on this tree | Status |
|---|---|---|---|
| S-1 = P2-1 (P2) | Fix: `canonicalize` key; refuse on failure | Original counterexample: a symlink spelling and a case spelling through served `CoreDispatch` fetches, with a holder of the resolved spelling. Both rows (`spellings.rs`) now answer `cancelled`; the control passes. The case rows ran on `/Volumes/gwztmp` (no skip). Manager rows: symlink, case, two genuine workspaces, no-resolve, cancelled-first. Mutation S gives 5 red. `memberlockmanager.rs:107` refuses with `IoError` and holds nothing. | **Closed** (residual spelling class: P3-N1) |
| P3-C6 = P3-2 (P3, ARCH) | Fix: manager into `operation` | `git grep session_host -- src/workspace_ops`: only `gwz_session_host` (the crate) in `tests/g29.rs:11` and `tests/g26/tag_member_step.rs:9`. `src/operation` names no `crate::session_host`; `context.rs:43` and `served.rs:43` import `crate::operation::MemberLockManager`. The cycle `session_host::served` → `workspace_ops` → `session_host` is gone. The check rejects an added import (mutation X). | **Closed** |
| P3-C2 = P3-3 (P3) | Fix: dead tails removed, source check | `push_member` and `push_root` no longer take `dry_run`, and no `backend.push` remains in them. The only production `backend.push` is the tag delete (`handle_tag.rs:277`), under a step. The check is wired into `run_tests.py:554`, the boundary workflow (check and unit test), and `test_run_tests_parallel_checks`. It fails on I, T and Z. | **Closed**; the check's false negatives are P3-N2 |
| P3-C1 = P3-1 (P3) | Fix: module tables | No "not plumbed" in `src`. `dispatch.rs:11-12`, `memberlockmanager.rs:13-15` and `membermutationguard.rs:8-9` agree with the code: tag push, fetch and delete are locked, and the only exclusion is the dry-run plans. | **Closed** (one overclaim: P3-N1) |
| P3-C3 (P3) | Fix: tag delete takes the step | `handle_tag.rs:269-280`: `steps.enter(&root, member_id)` around the delete push. Row `a_remote_tag_delete_waits…` exists; T is red (1). | **Closed** |
| P3-C4 (P3) | Fix: tracking-ref read inside the guard | `member_preflight.rs:87-98`, `root_lock.rs:54-64`, `pull_head_merge_preflight.rs:89-99`: `read_ref(remote_ref)` is in the guard's block. Pull rows green. | **Closed** |
| P3-C5 (P3) | Fix: waiter count | `wait_for_waiters` and `assert_waiting` replace the time windows in the manager, g29, tag and served rows. A gives 24 red (drafter's log). D, G and S each fail on "no call is waiting" within bounds in my runs. | **Closed**; the count lives in production code, not in the test arm (P3-N5) |
| P3-4 (P3) | Fix: bounded `reply`; sequential per-call entries | `served/tests.rs:332-358` watchdog (Deviation 1). D: 18 red in 51 s; G: 2 red in 20 s; no kill. These are the original hangs: D was killed at 900 s and G deadlocked in round 1. | **Closed** |
| P3-5 (P3) | Fix: member id with the plan | `plan_tag_pushes` returns `(member id, repo, refspec)` (`handle_tag.rs:443-471`). The `position(..).map_or_else(|| "@root")` fallback is gone (grep). | **Closed** (no row pins the key: P3-N3) |
| P3-C7 = P3-6 (P3) | Fix and record | Narrowing verified (Deviation 8); the record is in the rem1 README and names both breaks. | **Closed** |
| Code §3 risk (record for 2.3g) | Carry: a second `ServedSession` constructor must take `host.members()` | Still true: `serve_in_process` is the only constructor (`served.rs`). This is a carried record, not a 2.3c defect. | **Recorded / carried** |
| Code §4 (record for 2.3i) | Carry: W-class clones are unlocked | Unchanged (`materialize`, `clone_repo_member` take no step). It is C20 registry work. | **Recorded / carried** |

## 2. New findings

None is a new architectural root cause.

### P3-N1 — The key is the canonical path, not the filesystem's identity; one spelling class remains, and the case-variant rows' oracle is circular

- **Root cause.**
  - `MemberKey.workspace` is `std::fs::canonicalize(workspace)` (`operation/memberlockmanager.rs:107`), which is `realpath(3)` on Unix. That unifies symlinks, macOS case and Windows short names. It does not unify two paths to one directory that are not links:
    - macOS firmlinks;
    - Linux bind mounts;
    - Linux case-folding (`casefold`) directories, where `realpath` keeps the given case.
  - The module table (`:8`) and the struct doc (`:33`) call the key "the root's filesystem identity", and the round-1 impact list named bind mounts.
  - The test oracle `test_support/spellings.rs:30` decides "same directory" with that same `canonicalize`. Where canonicalization does not fold a case spelling that the volume does fold, the row prints `SKIPPED ROW: … case-sensitive` instead of failing.
- **Violated invariant.** Design §5.1 says sessions of one host context never update one member's refs at once. The module's own claim is identity.
- **Reproduction** (my copy, manager row; §0 has the full row):
  ```rust
  let real = home.join(".review-…");            // on the Data volume via /Users
  let other = Path::new("/System/Volumes/Data").join(real.strip_prefix("/")?);
  // (dev, ino) equal; canonicalize(real) = /Users/…, canonicalize(other) = /System/Volumes/Data/Users/…
  let held = manager.lock(&real, "@root", None)?;
  // second.lock(&other, "@root", None) on a thread: acquired while held; wait_for_waiters(1, 3 s) = false
  ```
- **Impact.** Two such spellings of one workspace in one host context do not exclude each other: the Git ref-lock failure returns for them. Likelihood is low. The firmlink spelling must be typed by someone; bind mounts are mostly in containers. It is a residual of the S-1 class, not its original counterexamples, which are closed. There is also a parity side: Linux casefold volumes skip the case rows with a false reason.
- **Correction.** Pick one:
  - Key on file identity where std offers it: `(dev, ino)` via `std::os::unix::fs::MetadataExt` on Unix. Keep `canonicalize` on Windows, where `GetFinalPathNameByHandleW` gives the on-disk path. Put the arms inside `cfg_if!`.
  - Or keep the path key, change "filesystem identity" to "canonical path" in the table, and record the unhandled spellings.

  Either way, make `case_variant` decide "same directory" by metadata identity (`(dev, ino)` on Unix; `metadata(&variant).is_ok()` in the fresh temp directory otherwise), not by `canonicalize`.
- **Closure test.** Each of the following must hold:
  - A firmlink manager row (macOS, inside `cfg_if!`) waits.
  - Under a mutation that drops the identity key, that row goes red.
  - `case_variant` on a casefold or non-folding volume reports "same directory" without calling `canonicalize`.

### P3-N2 — Neither the new check nor the rows prove that the guard is held across the network call

- **Root cause.**
  - `check_member_step_boundary.py:163-168` accepts any `.enter(` token earlier in an enclosing block at its own depth. It does not check that the result is bound to a live name for the call's duration.
  - The `UPDATE` pattern (`:34`) only sees receivers whose name ends in `backend`.
  - The rows prove that a step *waits* for a holder, and fetch/pull overlap. A step that waits, releases at once and then runs its exchange unlocked passes them, except where a double counts overlap.
- **Violated invariant.** The check's own docstring (`:12-17`, "holds its guard over the call"). The commit says the check refuses "any … call that no enclosing block's member step entry guards".
- **Reproduction** (my copy, §0):
  - `let _ = steps.enter(..)?;` at the pull member fetch, pull root fetch, tag push and tag fetch (U-L, U-M, U-O, U-R): check exit 0, and all 48 harness rows green.
  - `if let Err(e) = steps.enter(..) { return Err(e); }` (V): check exit 0.
  - A renamed receiver with the step removed (W): check exit 0. Only the delete row catches it.
- **Impact.** Today every site binds `_guard` or `Ok(_guard)` (I read all nine), so there is no live defect. The tripwire is weaker than stated: the most natural refactor slip, `let _ =`, which `#[must_use]` also does not flag, passes every gate.
- **Correction:**
  - In the check, require the `.enter(` to be the initializer of a `let <ident>` whose name is not `_`, or the scrutinee of a `match`/`and_then` whose arm or closure binds a name. Reject an `.enter(` inside an `if`/`while` condition.
  - Widen the receiver match to any `.(fetch|push|push_prepared|tag_fetch)(` on a `GitBackend`-typed name. For example, scan for the method names and allow-list the `Vec::push` shapes, as the existing `test_a_vec_push_is_not_a_network_update` does.
  - Add a test-only `MemberLockManager::is_held(workspace, member)` and have the tracking doubles' `fetch`, `push_prepared` and `tag_fetch` assert that their member is held.
- **Closure test.** U-L, U-M, U-O, U-R, V and W each make the check exit 1, and U-L, U-M, U-O and U-R each turn at least one row red.

### P3-N3 — No row pins the tag arms' member key for a non-root member

- **Root cause.** Every `tag_member_step` row and every served tag row holds and observes only `@root`. Nothing checks that tag push, fetch or delete enter the right `mem_…` key.
- **Violated invariant.** P3-5's intent: a member is keyed by its own id, not as the root.
- **Reproduction.**
  - K2 (`plan_tag_pushes` tags every plan `"@root"`): 98/98 rows green.
  - K3 (tag fetch enters `"@root"` for every repository): 98/98 green.
- **Impact.** The exact regression P3-5 described can return unseen. A member's tag push or fetch would lock the root's key and leave the member's refs unlocked against a member fetch. The code is correct today (`handle_tag.rs:133-137, :353, :370, :274`).
- **Correction.** One row per tag op (push, fetch, delete) on a workspace with a `mem_…` member:
  - a holder of that member makes the op wait;
  - a holder of `@root` alone does not.
- **Closure test.** K2 and K3 red, plus a K4 for delete.

### P3-N4 — Canonicalization adds a new refusal on the direct-caller path, where it buys nothing (limitation; trigger not reproduced here)

- **Root cause.** `BoundMemberLocks::per_call()` (`memberlockmanager.rs:170`) serves a single call. Every `enter` of one call passes the same `root` value (fetch, push, pull and tag each pass their one resolved root), so the lexical spelling is already a sound key there. Yet `lock` now canonicalizes on every step and refuses with `io_error` if that fails.
- **Violated invariant.** Round 1's basis for the per-call manager: CLI and gwz-py behaviour "exactly as before". The codebase's own `normalize_path` (`workspace_ops/normalize_path.rs:7`) falls back deliberately when `canonicalize` fails.
- **Reproduction.** Not reproduced on the Mac. A failing `canonicalize` on a directory that otherwise works is a known `std` limitation on some Windows volumes, where `GetFinalPathNameByHandleW` is unsupported (some RAM disks and virtual filesystems). That is why crates like `dunce` and `normpath` exist. On such a volume every CLI or gwz-py fetch, push, pull and tag row would fail `io_error` where 1.0.x succeeded. W-class merge already canonicalizes fatally (`merge/v1_lifecycle/checked.rs:96`), so the exposure is not entirely new.
- **Correction.** Keep the RemPlan's refuse-not-fall-back rule for the host manager. Let the per-call manager key on the lexical root, for example a `canonical: bool` on `BoundMemberLocks` or a separate key function. Say so in the module table.
- **Closure test.** A manager row: a per-call `enter` on a root that does not canonicalize succeeds, and the host manager's `enter` on the same root refuses with `io_error`. Mutation S2 stays red for the host path.
- **Dependency.** Whether any Windows host in the matrix exhibits the trigger depends on the receipt (§4).

### P3-N5 — The "test-only" waiter count is production state

- **Location.** `State::waiting` and `Shared::arrived` (`memberlockmanager.rs:43, :52`), with `arrived.notify_all()` on every wait start (`:123-124`). These are production code; only the observers sit in the `cfg_if!` test arm.
- **Violated invariant.** RemPlan P3-C5 ("a test-only waiter count … inside the existing `cfg_if!` test arm"). The commit message says "a test-only observer". The README's "What changed" is accurate, but the deviation list omits this.
- **Impact.** Negligible cost: one counter and one `notify_all` per wait under a lock already held. This is a documentation and disposition mismatch only.
- **Correction.** Pick one:
  - Declare the deviation and justify it: the count must be maintained where the wait happens.
  - Or move the counting behind `cfg_if!` test arms, with a no-op production arm.
- **Closure.** Text.

## 3. Invariant analysis

**Module boundary and direction.**
- The new graph is:
  - `workspace_ops` → `operation::{MemberSteps, BoundMemberLocks}`;
  - `session_host::{context, served, served::dispatch}` → `operation::{MemberLockManager, BoundMemberLocks}`;
  - `operation::memberlockmanager` → `operation::membermutationguard`, `model`, and the `gwz_session_host` crate (`CancellationToken`, `Refused`).
- No `crate::session_host` path appears in `operation` (one doc mention only) or in `workspace_ops` (production and tests). There is no cycle among the three, including test code: the tests name only the `gwz_session_host` crate.
- `operation` now depends on the `gwz-session-host` crate, which gwz-core already depends on unconditionally (`Cargo.toml:79`). This is acceptable under the crate map: core composes the host crates.
- Pre-existing, outside this change: `operation` and `workspace_ops` already import each other (`operation/commit_log/request.rs:11, :67-70`, `operation/push_event.rs:119`, at 6c219892 too). "Below the handlers" is true of the new module file, not of `operation` as a layer (§5).

**The port and its entry points.**
- `MemberSteps` (`enter(&Path, &str)`, `Sync`) is unchanged in shape.
- The four `*_with_member_steps` functions are called only by the dispatch arms (`dispatch.rs:162-236`) and the rows.
- The public handlers keep their signatures and build `BoundMemberLocks::per_call()`.

**Call graph from `served/dispatch.rs` to network ref updates.** These nine production sites are the complete set: a grep for `.fetch(`, `.tag_fetch(`, `.push_prepared(` and `.push(<repo>…` over `src`, excluding tests and backends, finds nothing outside `workspace_ops`.

| arm | site | step | §5.1 |
|---|---|---|---|
| fetch | `handle_fetch.rs:427` | `enter(root, member_id).and_then(\|_guard\| …)`, through `read_ref(after)` and `ahead_behind` | met |
| push | `push_member.rs:358-361` (members), `:416-420` (root) | `match enter { Ok(_guard) => push_prepared }` | met |
| pull_head | `member_preflight.rs:91-97`, `root_lock.rs:57-63`, `pull_head_merge_preflight.rs:92-98` | `let _guard` block, with the fetch and the tracking-ref read | met |
| tag push | `handle_tag.rs:353-356` | `let _guard`, with the plan's member id | met |
| tag fetch | `handle_tag.rs:370-371` | `let _guard` | met |
| tag delete | `handle_tag.rs:274-279` | `let _guard` (new) | met |

- Not locked, correctly: `ls_remote` and `ls_remote_url` reads; `remote_has_tag` (a read before the delete step); the dry-run plans (`push_member` and `push_root` now only plan; `planned_push` reads only); tag fetch's dry run, which returns before any traffic.
- The W-class `materialize`, `clone_repo_member` and local-family `fetch_anonymous`/`push_anonymous` are outside §5.1's member steps (carried to 2.3i).
- With the `dry_run` tails removed, no unlocked network ref update remains on any served path.

**Manager semantics (unchanged, re-checked):**
- token registration precedes canonicalization, so a cancelled token answers `cancelled` first;
- `wake` takes the state lock before notifying, so no wakeup is lost;
- `Release::drop` runs on unwind;
- poisoned locks are recovered by `into_inner`;
- `waiting` is incremented and decremented around the wait under the lock.

Mutations D, E, F, G and H stay red (D and G in my copy; E, F and H from the drafter's logs).

**Deadlock.**
- Each site holds one guard and drops it before the next.
- The mutator lock is a try-lock taken first.
- `par_map` holds one guard per worker.
- Under D and G the rows now fail in bounded time.

**Repo rules.**
- New cfg only inside `cfg_if!` (`test_support/mod.rs`, `spellings.rs`, the memberlockmanager test arm and its rows).
- No globals or thread-locals (`check_process_globals` exit 0).
- No production code under `tests/`; the new `test_support/spellings.rs` is test support under `src/test_support`, as before.
- gwz-transport does not depend on gwz-core.
- Braces: Rust requires them.

**Parity.**
- Mac: verified.
- Windows: `canonicalize` yields `\\?\` verbatim on-disk-case paths. Every key goes through it, so the prefix is consistent, short names are expanded, and case is folded. That is in theory only: the NTFS case-variant rows are the proof, and they run in the receipt.
- Linux: symlinks are unified; casefold directories and bind mounts are not (P3-N1).

**Agreement.**
- The commit message, module tables and README agree with the code, except P3-N1 ("identity") and P3-N5 ("test-only").
- `Windows-receipt: p2-3bc2-20261031-a` is in the trailer. Its truth depends on the receipt now running.

## 4. Risks and next action

**Next action.** Land 2.3c on this axis once the receipt and WSL are green. Fixing P3-N2 and P3-N3 is cheap and removes the two regressions the gates cannot see today. P3-N1, P3-N4 and P3-N5 may be fixed or carried with a record.

**Depends on runs I could not see:**
- Windows receipt `p2-3bc2-20261031-a`:
  - the case-variant rows on NTFS;
  - the verbatim form of the key;
  - whether any Windows venue trips P3-N4;
  - the truth of the commit's receipt trailer.
- WSL `p2-3bc2-20261031-b`: Deviation 7 (the four Linux rows, g13 and g23 ×3). The implementer argues they are unrelated from their messages; their code paths take no member step (§3 call graph). A green ext4 run settles it.

**Carried (unchanged):**
- 2.3g: any second `ServedSession` constructor must take `host.members()`.
- 2.3i: W-class clones are unlocked until the workspace registry exists.

**Limitation:** under a lock-less mutant, the `LockingDispatch` "never at once" peak rows rely on two 200 ms steps overlapping. They are deterministic on the correct tree, probabilistic only as mutation detectors.

## 5. Out-of-scope observations

- **Pre-existing `operation` ↔ `workspace_ops` mutual import** (`operation/commit_log/request.rs`, `operation/push_event.rs`; present at 6c219892). The new manager file itself sits below the handlers, but the `operation` layer does not. This is structural debt for the operator to decide on, not 2.3c's.
- **The tag delete's check-then-act.** `remote_has_tag` runs before the member step. Two concurrent deletes of one tag both see it, and the second one's push of `:refs/tags/x` meets a ref that is already gone. This race is older than the lock, which neither causes nor fixes it. Worth a row when the registry (C20) serializes W and N operations.
- **2.3b's P3-B1, P3-B2 and P3-B4** stay with the lane owner at landing, per the RemPlan. P3-B3's sentence is in gwz-transport 281a28d, and the pin points at it.
