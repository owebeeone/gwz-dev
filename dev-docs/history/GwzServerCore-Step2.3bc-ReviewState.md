# Steps 2.3b and 2.3c (carried connection fixes; member lock manager) — State-AXIS REVIEW

**Review object:** gwz-transport a3f9ed0; gwz-core 76507c49 and 3d4cff0a in lane p2-3bc; evidence 2026-10-31-p2-3b and -p2-3c
**Baseline:** I read `git show` of the three commits against their parents (363e0f9, 8b930275, 76507c49) in lane /Volumes/projects/limbo/gwz-dev-p2-3bc. I read the touched files at their committed state and the controlling documents named in the brief. Builds and tests ran only in `rsync -aL` copies under `/Volumes/projects/limbo/build-targets/review-p2-3bc-state-{1,2}`. Both copies and the Linux container and volume are deleted.
**Date:** 2026-10-31
**Axis:** State: adversity and fail-closed direction (lock ordering, cancel races, release on panic, guard lifetime, key identity, waiter slots, stream ceiling, capacity wait).
**Verdict: 2.3b GO; 2.3c NO-GO.**
- 2.3b: 0 P0, 0 P1, 0 P2, 4 P3.
- 2.3c: 0 P0, 0 P1, 1 P2 (S-1), 7 P3.
- Pre-commit: 2.3c becomes GO on a revision that resolves S-1 with its closure rows (lane root/symlink spelling and the served two-spelling row). The P3s need no further review round. P3-C1, P3-C2 and P3-C3 should ride along in the same revision, because each is a few lines.

## 0. Evidence base

**Tuple verified at start and at end.**
- Lane root `e6037850ed1d`, gwz-core HEAD `3d4cff0a` and gwz-transport HEAD `a3f9ed0`.
- Both repos are clean. The root has only the staged gwz.conf files and the operator's untracked drafts.
- The tuple did not move.

**Runs on the Mac.**
- gwz-core (rustc 1.95.0, `GWZ_TEST_GIT=real`, TMPDIR=/Volumes/gwztmp): `session_host::members`, `workspace_ops::tests::g29`, `g26::tag_member_step`, `session_host::served::tests`. 39 passed.
- gwz-connection (rustc 1.96.0): the whole lib suite. 214 passed on the object; 217 with my 3 added rows.

**Linux.** A new `rust:1.95-bookworm` container (`review-p2-3bc-state-ct2`, volume `review-p2-3bc-state-vol`) ran the same core rows (39 passed) and the whole gwz-connection suite (214 passed). Both were removed afterwards.

**Windows.** The lane owner's receipt p2-3bc-20261031-a was not given to me, so I did not assess it.

**Mutations I re-ran**, in my copy, not the lane. Each went red on the rows its author named:
- Core, using the drafter's `mutate.py` adapted to my copy:
  - A (`enter` returns an unheld guard): 21 red.
  - E (guard does not release while panicking): `a_panic_while_holding_a_guard_does_not_fail_later_operations` red.
  - J (push member step unlocked): `a_push_waits_for_the_member_another_step_holds` and `a_cancelled_push_wait_fails_its_members_row_with_cancelled` red.
  - O (tag push unlocked): 3 red.
- Transport, mutations of my own:
  - M1 (every Registered future shares slot 0): the 17-waiter row red.
  - M2 (clamp 1024 in place of 64): the pinned-ceiling row and the 128-streams row red.
  - M3 (`Capacity` answers `Fault(Capacity)` again): the abort row red.
  - Source diffed identical to the lane after each.

**Reviewer rows I added in my copy (not part of the lane).**
- Transport, 3 rows, all green:
  - A wait queued behind a retiring route resolves `Cancelled` on `cancel_handle().cancel()` when the client never answers.
  - A dropped waiting open gives its ceiling slot back.
  - 70 futures with mixed re-polls under fresh wakers and every third dropped: each kept future is woken once, by its newest waker only, and zero waiters remain.
- Core, manager level, 4 rows:
  - 3000-round cancel/release race with a hung-waiter timeout: green.
  - 12 threads × 400 acquire/cancel on two members, checking mutual exclusion and no hang: green.
  - Symlink spelling of one workspace: red (S-1).
  - Served level, two sessions of one host on two spellings: red (S-1).

**Evidence directories.** `runs/2026-10-31-p2-3b/patches/carried_rows.rs.new` and `runs/2026-10-31-p2-3c/patches/members.rs.new` equal the committed files.

## Problem, requirement provenance, and scope

**2.3b (C13).**
- **Requested outcome.** Close the three 2.2c P3s: State P3-5 (waiter eviction), Code P3-7 (`BindBounds` and `RemoteConfig` built apart), Code P3-8 (`Fault(Capacity)` right after an abort). The operator's go ("yes, start 2.3d and 2.3b") plus plan §3 2.3b authorize it.
- **Necessity.** Each mechanism is needed.
  - Slots: eviction at 16 is the observed lost wakeup.
  - One ceiling: a core above 64 faults a lagging client's binding.
  - Capacity wait: the abort row shows `Fault(Capacity)`.
- **Alternative considered.** Rejecting `streams_per_binding > 64` instead of clamping would be louder. The clamp is documented (README, field doc) and the plan text asked for the ceiling, so I treat it as a choice, not a defect.
- **Scope.** Owned files only. The plan lists `lane.rs` but it needed no edit, because `BindBounds::core` already called `config.streams()`. The allow-list `reconciled_commit` was not touched. That is correct, since the list did not change and `check_process_globals` reports "nothing new".

**2.3c (C10).**
- **Requested outcome.** Design §5.1: a blocking, poison-recovering, cancellation-aware member lock in the host context, so sessions of one host never update a member's refs at once.
- **Provenance.** Plan §3 2.3c and the 2.2b carried item.
- **Widened plumbing.** The lane owner widened the plumbing from "fetch and push member steps" to pull preflight fetches (member, root, merge plan) and tag push and fetch.
  - I judge the widening necessary. A lock that leaves a ref-updating path unlocked does not deliver "never update a member's refs at once".
  - It stays inside the plan's "narrow port": the public handler signatures are unchanged and the bodies gain one `steps` argument.
  - The authority text covers it.
- **Drafter's notes, judged.**

| Note | Judgment |
| --- | --- |
| `operation::MemberLockManager` removed | Accepted by the lane owner. Its one caller was a test. The semver note is P3-C7. |
| Tag push cannot meet a push in one host | Sound. Both take the workspace mutator try-lock and the second fails fast. The member lock matters for fetch/pull-preflight beside push/tag. |
| `PreflightPorts` | Fine. |
| Test doubles gain `set_remote_tracking`, a 0/0 `ahead_behind` and `tag_fetches()` | They sit under `src/…/tests/`, not the top-level `tests/` the repo rule names. |
| Clamp at 64 for the core mux, `RemoteOp::open` and `BindBounds::core` | All three use `streams()`; the only other reader of the field is its own doc comment. |

## 1. Findings

### S-1 (2.3c, P2): the lock key is a lexical path, so the same workspace under two spellings gets two keys and no exclusion

- **Root cause.** `MemberKey.workspace` is the `PathBuf` that `resolve_request_workspace_root` produced (`session_host/members.rs:30`, built at `:80`).
  - That path is only lexically normalized (`workspace_ops/normalize_path.rs:66`; `normalize_absolute_path` → `lexical_normalize`) and never canonicalized.
  - A served session accepts a request only if `root == session.root()` literally (`session_host/served.rs:287`).
  - So two sessions of one host context, one serving `…/ws` and one serving `…/ws-link` (a symlink to it), both pass `resolve`, and the manager treats them as unrelated workspaces.
- **Violated invariant.** Design §5.1 and the module's own first row: "the sessions of one host context never update a member's refs at once" for "that workspace and member". The mutator lock is a file lock and is spelling-independent, so the two locks disagree on workspace identity. The failure direction is open: the old Git ref-lock refusal returns.
- **Reproduction** (my copy; red on the lane tree).
  - Manager level: `lock(&real,"mem_01")` held, then `lock(&link,"mem_01")` acquires within 150 ms.
  - Served level: `host = HostContext::new()`, `Session::on(&host,&root,…)` and `Session::on(&host,&link,…)`, one `fetch` each through `LockingDispatch`. `steps.peak == 2`, where the lane's same-spelling row asserts 1.
  - The spelling pair is real on the operator's machine: `/Users/owebeeone/limbo` is a symlink to `/Volumes/projects/limbo`.
  - Windows has the same shape through case, 8.3 names and verbatim prefixes, since the key is compared byte-wise.
- **Impact.** Sessions reached by different spellings of one workspace can race a member's fetch/push ref update exactly as before 2.3c, with a Git ref-lock error as the visible symptom. I found no corruption path. Today it needs a driver that opens sessions with different spellings in one host context. 2.3g's per-workspace server likely hides it, but the manager is built to be host-wide and must not depend on that.
- **Correction.** Key on a canonical identity. Either canonicalize `workspace` inside `MemberLockManager::lock` (one `fs::canonicalize` per step against a network exchange; fall back to the lexical path only if canonicalize errors) or canonicalize the root once when `serve_in_process` builds the session. Better still, key on the member's canonical repository path, which is what is actually contended. Document the choice in the module table.
- **Closure tests.**
  1. A manager row: `real` and a symlink to it are one key, so the second `lock` waits.
  2. The served row above, with `peak == 1`.
  3. A control that two genuinely different workspaces do not wait.

### 2.3c P3s

- **P3-C1 (documentation contradicts code).** `session_host/members.rs:13` says `tag`'s fetch "is not plumbed yet and is reported as a finding", but 3d4cff0a plumbs it. The commit message, the evidence README and mutation R all say so, and `handle_tag.rs` Fetch takes the guard. Delete the clause. Correction and closure: edit the clause and grep it.
- **P3-C2 (dead, unlocked network ref-update path).**
  - `push_member` and `push_root` (`push_member.rs:460`, `:568`) have non-dry-run tails that call `backend.push` (`:541`, `:635`) with no lock.
  - Every caller passes `dry_run = true`, so the tails are dead.
  - They are `pub(crate)`, so the next caller gets an unlocked network push. The commit message's exclusion ("the dry-run push helpers") describes the call sites, not the code.
  - Remove the dead branches in this change (the standing rule), or take the guard there. Closure: the functions lose the `dry_run` parameter or the branch, and the exclusion text becomes true.
- **P3-C3 (undeclared exclusion).** `tag --remote … delete` pushes `:refs/tags/<name>` unlocked (`handle_tag.rs:267`). It updates no local ref unless the remote's fetch refspec maps tags, so it is probably sound. It is the only served network write absent from both the locked list and the exclusion list. Add it to the exclusion list with the reason, or lock it for symmetry.
- **P3-C4 (pull's tracking-ref read after the guard).** The guard covers `backend.fetch` only (`member_preflight.rs:88-91`, `root_lock.rs:55`, `pull_head_merge_preflight.rs:90`). `read_ref(remote_ref)` follows (`member_preflight.rs:92`). A fetch in another session can move the tracking ref in between, so the plan can name a commit that is not what this call fetched. The objects exist, so it is safe, but it is nondeterministic. `fetch_one` reads under the guard; pull should too, or the module table should say reads sit outside the step.
- **P3-C5 (negative rows rest on a timing window).** About 14 rows prove "it is waiting" by `recv_timeout(150–250 ms).is_err()` (`members/tests.rs`, `g29.rs:21`, `tag_member_step.rs:14`). Under load a lock-less mutant can pass them. The mutation passes were red here, so the rows do bite, but they are not deterministic. Add a test-only waiter count (like `held_count`) and assert on it.
- **P3-C6 (ARCHITECTURAL: layering inversion).** `workspace_ops` (`handle_fetch.rs:28`, `push_member.rs:10`, `pull_head_member_preflight/handle.rs:9`, `handle_tag.rs:43`) now imports `session_host::members::BoundMemberLocks`. At 76507c49 `workspace_ops` named no `session_host`. `operation/membermutationguard.rs:10` claims the layer "names nothing of the session host".
  - The `MemberSteps` port is the right seam. The per-call manager and `BoundMemberLocks` should live below it (in `operation`) with `session_host` supplying only the host-wide manager.
  - It works, so this is a structural debt for the operator to decide, not a blocker.
- **P3-C7 (public API removal).** `gwz_core::operation::MemberLockManager` is gone and `MemberMutationGuard` is now unconstructible outside the crate. The lane owner accepted this. Record it in the changelog. Nothing in gwz-cli, gwz-py, scripts or crates uses either name.

### 2.3b P3s

- **P3-B1 (limitation: the capacity wait has no bound of its own).**
  - An open at the bound after an abort waits for the client's terminal. It resolves on terminal, `cancel`, `cancel_all` or carrier loss (`abort_all`).
  - It does not time out if a misbehaving client keeps the carrier up and never answers an aborted route. The mux gives such a route no deadline.
  - My row shows the wait is cancellable. This is acceptable for 2.3b, and the old `Fault(Capacity)` was the wrong answer for the benign case. Record it in the contract and consider a retire deadline later.
- **P3-B2 (closure item: contract §15).**
  - `GwzConnectionApiContract.md` still says "an open … can answer `Fault(capacity)` … carried to step 2.3 (2.2c re-verdict Code P3-8)".
  - The lane owner records the agreeing sentence at landing. The sentence must also say that `streams_per_binding` is held to 64 and that the 65th open is `BindingLimit`.
  - GO for 2.3b stands on that sentence being written before landing.
- **P3-B3 (invariant stated only in a comment).**
  - `wait_for` now keeps one waiter per `Wait`. That is correct for Registered, which has per-future slots.
  - For `Opening` and `Ending` it relies on a single owning future. That holds (`RemoteOpenWait` and `RemoteFinish` own their `Live`), but a second waiter on the same `(serial, token)` would silently replace the first. The old 16-entry list tolerated it.
  - Add one sentence to the `Wait` enum doc saying `Opening` and `Ending` have exactly one owner.
- **P3-B4 (silent clamp).** `RemoteConfig { streams_per_binding: 128 }` reads as 64 with no signal. Documented in the field doc and README. A debug assertion or a log at `handle_with` would make a composition mistake visible.

## 2. Invariant analysis

**Lock ordering and deadlock (2.3c).**
- There are seven acquisition sites: fetch member and root, push member and root, pull member fetch, pull root fetch and merge-plan root fetch, tag push, tag fetch. Each takes exactly one key and drops its guard at the end of the exchange.
- No step holds two member keys, so there is no hold-and-wait between members.
- Order against other locks:
  - Push and tag take the workspace mutator try-lock first and the member lock second.
  - Nothing holds a member guard while taking the mutator, so the order cannot invert.
  - Because the mutator is a try-lock, it never waits behind a member wait.
- `par_map_per_host` is per call, with no process-wide limiter. A holder never waits on a waiter's per-host slot.
- Inside the manager, `held` is held only across one set operation or the wait. The cancel callback takes `held` briefly and notifies; `is_cancelled` is a lock-free atomic. No lock is taken in both orders.
- **Fail-closed:**
  - If the manager is unreachable, `member_locks(worker)?` refuses before any effect.
  - If `enter` is refused, the step does not run.
  - The one fail-open direction is S-1.

**Multi-member steps and mid-way cancel.**
- Fetch and push: a cancelled wait fails that member's row with `cancelled` (`refuse_member_step`; for fetch the guard sits inside `and_then`). Queued items refuse immediately, because a cancelled token refuses before the wait.
- Pull preflight and tag: the error fails the call. In pull the preflight precedes any local write, so it fails closed. In tag push, earlier members' pushes stay done. That is the old mid-loop-error behaviour, not new.
- Every path releases through `Release::drop` on `?`, unwind and normal exit.

**Cancel wake-up.**
- The waiter checks `is_cancelled` under `held` and parks via `Condvar::wait`, which releases `held` atomically. `wake()` takes `held` then notifies, and runs after the token flag is set (`cancel()` swaps first, then drains callbacks). A cancel between check and wait therefore cannot be lost.
- `on_cancel` refuses once cancelled, and registration is atomic against `cancel()`.
- Release removes under the lock then notifies, so a freed member cannot be missed.
- My 3000-round race and the 12-thread stress found no hang and no double hold.

**Panic while holding.**
- The guard releases on unwind (mutation E red).
- The `held` mutex is poisoned only if a panic occurs inside a one-key set operation; recovery via `into_inner` is correct there.
- The member's own half-finished Git ref update is not tracked, and a stale `.lock` from an abort would surface as the pre-2.3c ref-lock error. I treat that as the same limitation as before, not a regression.

**Guard lifetime.**
- Fetch holds from before `read_ref(before)` to after `read_ref(after)` and `ahead_behind`.
- Push holds across `push_prepared`, which includes libgit2's tracking update, in both the member and root arms. The `Ok(_guard)` binding is a named variable, not `_`.
- Tag holds per iteration across the exchange.
- Pull holds only across the fetch (P3-C4).

**`@root` against a member named `@root`.** Manifest member ids must parse with the `mem_` prefix (`artifact/validation.rs:38`, `records.rs:74`). A member cannot be named `@root`, so the key is unambiguous. Fetch, push, pull and tag all key the root as `"@root"`.

**One manager per host.**
- `HostContext` owns the manager; `serve_in_process` is the only `ServedSession` constructor and clones the host's.
- The fetch, push, pull_head and tag arms all call `member_locks(worker)`. Mutations B, B2, C, P, Q in the drafter's table cover the arms.
- Per-call managers exist only on the public direct-caller handlers (no host context, by design).
- Two sessions of one host share the manager. S-1 is only about the key's identity.

**Any ref-updating path reachable without the lock.**
- After S-1, the unlocked network paths are the dead non-dry-run `push` tails (P3-C2) and `tag … delete` with a remote (P3-C3).
- Clone and materialize write only fresh member paths. I found no `fetch` call in materialize or merge.
- The dry-run exclusion is sound: those helpers read and plan only, and every caller passes `true`.

**2.3b: waiters.**
- 17 futures on one registration are each woken once; 70 with mixed re-polls and drops are each woken once, by the newest waker.
- A dropped future's `RegisteredSlot` removes its waiter. It stays a field, not a `Drop` on the borrowing future.
- A re-poll replaces its waker and adds none.
- `forget` runs under the state lock but only retains and drops wakers. I could not construct a re-entrant drop of another slot there, because a future being dropped is not the one whose waker is last.

**2.3b: ceiling and capacity.**
- The ceiling is `min(streams_per_binding, 64)` for the mux, `RemoteOp::open` (`active >= streams` → `BindingLimit`) and `BindBounds::core`. The client's end is the fixed `STREAMS` of 64.
- Counting `Queued` exchanges means the 65th open is `BindingLimit` at once, with no binding fault. Row and my abort variant confirm this.
- Open after an abort: `active` excludes ended exchanges, so one more open is admitted, the mux is full (the route is still in flight), and `Capacity` now parks the exchange as `WouldBlock` does. `issue_opens` returns on the first block, which keeps FIFO order.
- The wait ends on route retire or `cancel`.
- `Capacity` from `Mux::start` is transient. The only permanent source is an i64 id overflow, which is unreachable.

## 3. Risks and next action

**Next action for 2.3c.**
1. Fix S-1 (canonical key) with the three closure rows.
2. In the same revision, fix P3-C1 (a doc line), P3-C2 (delete the dead branches) and P3-C3 (list the exclusion).
3. Record P3-C4 to P3-C7 in the lane's findings document.

**Next action for 2.3b.**
1. Write the §15 sentence at landing (P3-B2).
2. Add the `Wait` owner sentence (P3-B3).

**Residual risks (limitations, not defects).**
- A member lock is held for a whole network exchange with no hold bound. A hung exchange stalls other sessions' steps of that member until the holder's transport timeout. Waiters are cancellable.
- Waiters take a freed member in no order, as documented.
- The tag push or fetch that waits holds the workspace mutator for the wait, so a W operation meanwhile gets "already held". That is the existing try-lock behaviour.
- Landing order: the pin moves to a3f9ed0, so gwz-transport must be on its GitHub main first.

## 4. Out-of-scope observations

- Separately from the harness, the drafter's `mutate.py` edits the lane's own `gwz-core/src` in place (restoring it afterwards). Running it in a copy, as I did, avoids touching the lane tree.
- The evidence README for 3b notes `clippy --workspace --all-targets` fails in untouched root-package integration tests (`raw/3b-clippy-root-all-targets-preexisting.log`). I did not verify it.
- `FetchResult::Failed` is the summary for a fetch row that was refused by a cancelled lock wait, although nothing was contacted. The member row carries the `cancelled` code, so only the repo-summary label is imprecise.

Files: `/Volumes/projects/limbo/gwz-dev-p2-3bc/gwz-core/src/session_host/members.rs`, `/Volumes/projects/limbo/gwz-dev-p2-3bc/gwz-core/src/session_host/served.rs`, `/Volumes/projects/limbo/gwz-dev-p2-3bc/gwz-core/src/workspace_ops/push_member.rs`, `/Volumes/projects/limbo/gwz-dev-p2-3bc/gwz-core/src/workspace_ops/handle_tag.rs`, `/Volumes/projects/limbo/gwz-dev-p2-3bc/gwz-core/src/workspace_ops/normalize_path.rs`, `/Volumes/projects/limbo/gwz-dev-p2-3bc/gwz-transport/crates/gwz-connection/src/remote/core.rs`, `/Volumes/projects/limbo/gwz-dev-p2-3bc/gwz-transport/crates/gwz-connection/src/remote/backend.rs`.
