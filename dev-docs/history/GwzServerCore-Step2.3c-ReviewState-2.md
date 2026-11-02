# Step 2.3c round 2 (member lock manager) — State-AXIS REVIEW
**Review object:** gwz-core fc27ccd6 (on 783b306c) in lane p2-3bc2; evidence 447592e4 rem1
**Baseline:** How I read the sources:
- I read `git -C gwz-core diff 783b306c fc27ccd6` in full, and every touched file whole at fc27ccd6. The files I read whole include `operation/memberlockmanager.rs` and its tests, `membermutationguard.rs`, `served/dispatch.rs`, the two served test files, `test_support/spellings.rs`, `push_member.rs`, `handle_tag.rs`, `handle_fetch.rs`, the three pull preflight sites and `check_member_step_boundary.py`.
- I read the round-1 reviews, the RemPlan, the rem1 README and tools, plan §3 2.3c and the C10 row, and design §5.1.
- I ran builds, tests and mutations only in an `rsync -aL` copy, `build-targets/review-p2-3c-r2-state-src`, with its own target dir and `TMPDIR=/Volumes/gwztmp`. I deleted the copy, its target, my tools, logs and probe directories afterwards.
- I used one Linux probe container, `review-p2-3c-r2-state-ct`. It ran with `--rm` from the existing `p2-3a-linux-img`, with no image or volume created. It is gone.

This review was interrupted by a host restart and resumed. I re-verified the copy against the lane before continuing.

**Date:** 2026-10-31
**Axis:** State: adversity and fail-closed direction. This covers key identity, lock ordering, cancel and waiter-count races, panic and guard lifetime, unlocked paths and the new source check, and the `@root` key.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 2 P3 (R2S-1, R2S-2). Every 2.3c row of the RemPlan is closed on this tree; the closure table is §1.
- Neither P3 is a new architectural root cause.
- No pre-commit clause is needed: the P3s may be fixed or carried with a record.

## 0. Evidence base

**Tuple.** Checked at the start, again after the restart, and at the end. It did not move.
- Lane root 57279067. The only untracked files are the operator's drafts in `dev-docs/`.
- gwz-core fc27ccd6, clean.
- gwz-transport 281a28d. Against 0fa9306 it is one file, `remote/core.rs`, +2/−1: the `Wait` sentence, P3-B3.
- gwz-core-evidence 447592e4. The only untracked item is the pre-existing `2026-09-22-alpha-setup-timeout/`.

**The evidence matches the committed object.**
- The rem1 README says the gates ran on an uncommitted tree over 783b306c.
- I compared `patches/gwz-core-tracked.diff` (27 files) with `git diff 783b306c fc27ccd6` for those files, ignoring `index` lines: identical.
- The seven `.new` files are byte-equal to the committed files: memberlockmanager.rs and its tests, member_locks.rs, spellings.rs (both), check_member_step_boundary.py and its unittest.
- So the evidence covers the committed object.

**Runs on the Mac** (rustc 1.95.0, `GWZ_TEST_GIT=real`):

| Run | Result |
|---|---|
| `memberlockmanager`, `session_host::served`, `g29`, `g26::tag_member_step`, `g17` | 59 passed, 0 failed |
| `check_member_step_boundary.py` | ok |
| Its unittest (python3.13) | 24 passed |
| `check_cfg_boundaries.py --skip-repo gwz-cli --skip-repo gwz-py` | nothing new (the two skips are SKIPPED GATE; the copy has neither checkout) |
| `check_process_globals.py`, `check_windows_parity.py` | nothing new |
| `clippy --lib -D warnings` | clean |

**Mutations re-run.** I used the implementer's `mutate.py`, retargeted at my copy and with my reviewer rows skipped. `RESTORE-DIGEST EQUAL` after each.

| Mutation | Rows red here | README |
|---|---|---|
| A (no hold) | 24 | 24 |
| D (no cancel wake) | 18; no hang, the run is bounded | 18 |
| S (no canonicalization) | 5 | 5 |
| T (tag delete unlocked) | 1 | 1 |
| Check mutations I, J, K, L, M, N, O, R, T, X, Y, Z | each exits 1 | as recorded |

**Reviewer probes** (in my copy, not part of the lane):

| Probe | Result |
|---|---|
| Manager row: macOS firmlink spelling | **red** (R2S-1) |
| Linux probe: bind mounts and case-folding mount | two keys for one directory (R2S-1) |
| `let _ =` mutation at four sites | check ok, clippy clean, 98 rows green (R2S-2) |
| Nine crafted inputs to the check | eight missed, one control caught (R2S-2) |

**Not available to me:** the WSL matrix (label `p2-3bc2-20261031-b`) and the Windows receipt `p2-3bc2-20261031-a`; both were still re-running. These conclusions depend on them:
- that the case-variant rows run (not skip) and pass on NTFS;
- that `std::fs::canonicalize` gives one key for case and 8.3 short-name spellings on the hosted Windows TEMP;
- that the four Linux failures outside the change (deviation 7) pass on ext4.

**Host load:** load average was 4 to 26 during my runs. Timings are indicative only.

## Problem, requirement provenance, and scope

**Requested outcome.** C10 (plan §3 2.3c; design §5.1): one blocking, poison-recovering, cancellable member lock in the host context, so that "even sessions that share a host context never update the same member's refs at once". This round's mandate is the RemPlan's dispositions of the round-1 findings.

**Necessity of each mechanism:**
- **Canonical key.** Needed: it closes S-1/P2-1, a lexical key that fails open across spellings.
- **Refusing on a resolve failure.** Needed: it keeps the failure direction closed. A fallback to the spelling would quietly reopen S-1 (mutation S2).
- **Moving the manager into `operation`.** Needed to break the cycle (P3-C6/P3-2). `HostContext` still owns the one instance, and `workspace_ops` names only `operation` (grep finds no `session_host` in production files there; the tests name only the `gwz_session_host` crate).
- **Removing `dry_run` and the dead `backend.push` tails.** Required by the standing rule on dead code. I confirmed the four callers passed the literal `true`.
- **The source check.** Asked for by both round-1 axes. It is wired into `run_tests.py` and the workflow's "complete entry and call graph" step with its unittest, and `run_boundary_job.py` reads that step.
- **The waiter count.** Needed for deterministic "it is waiting" rows (P3-C5).
- **The watchdog in `Session::reply`.** A reasonable substitute for `recv_timeout`. Adding a timed receive would change a published crate, which is out of the step's scope.

**Scope.** All edits are in gwz-core, plus one sentence in gwz-transport (P3-B3, authorized by the RemPlan).
- The plan text says 2.3c owns `session_host/members.rs`. The move to `operation` is authorized by the RemPlan's P3-C6 disposition, so this is not a scope widening.
- `plan_tag_pushes` changed signature (deviation 5). It is `pub(super)` with one other caller (g17), so it is internal.
- gwz-transport still does not depend on gwz-core.

**Declared deviations, judged:**

| # | Judgment |
|---|---|
| 1, watchdog | **Accepted.** The bound is the same 20 s. `drop(finished)` disconnects rather than times out, so a reply that has arrived cannot be closed out, except within microseconds of the 20 s edge. D ran bounded here (18 red, no hang). |
| 2, `contact` inlined | **Accepted.** Inlining removes the allow-list need, and the check's "enclosing block at its own depth" rule bites (mutation Z). Its weakness is R2S-2, not the inlining. |
| 3, spot check and remaining sleeps | **Accepted.** Both remaining waits are deadline-bounded polls on a flag, not windows a verdict rests on. The `inside` poll could use the count's condvar later. |
| 4, test split | **Accepted.** The repo's split rule asks for it. |
| 5, `plan_tag_pushes` ids | **Accepted.** It removes the `@root` fallback (P3-5). |
| 6, Linux mutation subset | **Accepted.** The other mutations touch no platform-dependent code. S is the platform-sensitive one, and it ran there. |
| 7, four Linux failures without a baseline | **Not assessed beyond the diff surface.** The diff touches neither g13, g23 nor handle-capability code, so unrelatedness is plausible. It is not demonstrated: the WSL run (b) is the venue. This is not a 2.3c defect. |
| 8, `pub(crate)` with the record in the README | **Accepted** per the RemPlan. Note that the record sits in the private evidence member; see §4. |

## 1. Prior-finding closure table

| ID | Disposition | What I checked on this tree | Status |
|---|---|---|---|
| S-1 = P2-1 (P2) | Fix: canonical key; refuse on resolve failure | **Code:** `lock` canonicalizes after the token registration and refuses with `IoError` naming the root. There is no path to a lexical key. **Original counterexamples:** the symlink and case-variant rows are green at manager and served level (real git; `/Volumes/gwztmp` is case-insensitive, so the case rows ran, not skipped). **Mutation:** S makes those 4 rows plus the no-resolve row red. The control (two real workspaces) is green. A residual spelling class remains: R2S-1. | **Closed** for the classes the RemPlan named (symlink, case, short name); the short-name class is Windows only, pending the receipt |
| P3-C6 = P3-2 (P3, ARCH) | Fix: manager into `operation` | `git grep session_host -- src/workspace_ops` finds no production hit. Check mutation X exits 1. `context.rs` and `served.rs` import `crate::operation::MemberLockManager`. The cycle is gone. | **Closed** |
| P3-C2 = P3-3 (P3) | Fix: remove `dry_run` and the tails; add the check | The `backend.push` tails and `dry_run` are gone from `push_member` and `push_root`. The check passes on the tree, and check mutations I, J, K, L, M, N, O, R, T, Y, Z exit 1. It is wired into `run_tests.py` and the workflow. Its gaps are R2S-2, a tripwire weakness, not a reopening of P3-C2's counterexample. | **Closed** |
| P3-C1 = P3-1 (P3) | Fix: module tables | `git grep "not plumbed" -- src` finds nothing. The tables in `memberlockmanager.rs`, `membermutationguard.rs` and `dispatch.rs` list tag push, fetch and remote delete, and the dry-run-only exclusion. Over-claim: line 83 and table line 8 say "every spelling"; see R2S-1. | **Closed** |
| P3-C3 (P3) | Fix: tag remote delete takes the step | `handle_tag.rs:275` takes `let _guard = steps.enter(&root, member_id)?` around `backend.push(.., ":refs/tags/..")`. The row is green, and mutation T makes it red (rows and check). | **Closed** |
| P3-C4 (P3) | Fix: pull reads the tracking ref inside the guard | Read all three sites (`member_preflight.rs:87-98`, `root_lock.rs:54-64`, `pull_head_merge_preflight.rs:89-99`). `read_ref(remote_ref)` is inside the block holding `_guard`. The pull rows are green. | **Closed** |
| P3-C5 (P3) | Fix: waiter count; assert on it | `waiting` is counted under the state mutex and incremented before `Condvar::wait`, which releases the mutex atomically. An observer therefore cannot see the count before the waiter is parked. A spurious wake decrements and re-increments under the lock, so it is never seen at 0. `wait_for_waiters` checks its predicate under the lock, so no notify is lost. A makes 24 rows red, deterministically. | **Closed** |
| P3-4 (P3, Code) | Fix: bounded replies; sequential per-call entries | D: 18 red, no harness kill. The handler-port row takes `@root` and then `mem_01` in sequence. (G was not re-run by me; the README records 2 red in 20 s.) | **Closed** |
| P3-5 (P3, Code) | Fix: tag push carries its member id | `plan_tag_pushes` returns `(member id, repo, refspec)`, zipped from `selected`. The `position(..).map_or_else(|| "@root")` fallback is gone. O (README) and the tag rows are green. | **Closed** |
| P3-C7 = P3-6 (P3) | Fix and record | Both types are `pub(crate)`. The rem1 README "Release record" names both breaks: `MemberLockManager` removed, and `MemberMutationGuard<'a>` lost its lifetime and became crate-private. | **Closed** (see §4 on where the record lives) |
| P3-B3 (2.3b) | Fix: one sentence | `0fa9306..281a28d` is that sentence only. | **Closed** |
| P3-B1, P3-B2, P3-B4; Code §3 risk; Code §4 | Record, or lane owner at landing / 2.3g / 2.3i | Not this object's to close. Not assessed. | Carried (not 2.3c code) |

## 2. New findings

### R2S-1 (P3) — the key is the canonical path *string*, so spellings that `realpath` does not merge are still two keys; the closure row's detector cannot see them

- **Root cause.** `MemberKey.workspace` is `std::fs::canonicalize(root)`. On Unix that is libc `realpath`, which:
  - resolves symlinks;
  - on macOS, also corrects case;
  - does not merge firmlinks, bind mounts or names on a case-folding Linux volume.

  So one directory can still have two keys (`operation/memberlockmanager.rs:106-112`).
- **The detector is circular.** `test_support::case_variant` decides "case-insensitive" by `canonicalize(variant) == canonicalize(path)`: it asks the function under test. On a volume that folds case where `realpath` keeps the spelling, it prints `SKIPPED ROW` instead of going red.
- **Violated invariant.** The module's own claims: line 83 says "every spelling of one root is one workspace", and table line 8 says "a symlink, a case variant or a short name of one workspace is one key". §5.1's "never update the same member's refs at once" holds only for spellings that `realpath` merges.
- **Reproduction:**
  - **macOS, manager level, red.** A reviewer row in my copy with `root = /private/tmp/…/fl/ws` and `spelled = /System/Volumes/Data/private/tmp/…/fl/ws`. Both have dev/ino (16777229, 690839897). They canonicalize to the two different strings, and `assert_waiting` fails with "1 lock call(s) should be waiting; 0 are". `/Users/<u>` against `/System/Volumes/Data/Users/<u>` is the same pair on every macOS 10.15+ system.
  - **Linux container, glibc `realpath`.** `-v X:/a -v X:/b`: `/a` and `/b` have dev/ino (36, 2383754) and canonicalize to `/a` and `/b`. On the virtiofs mount of an APFS directory, `/a/casedir` and `/a/CaseDir` are one inode, yet canonicalize keeps each spelling. `case_variant` would therefore SKIP there.
- **Impact.** This is bounded.
  - It needs one host context to receive two such spellings of one workspace.
  - If 2.3f keys servers by the same canonicalization, those spellings land in different host contexts anyway. There the member lock never applied, and the behaviour is the pre-2.3c Git ref-lock refusal on one member row.
  - No corruption.
  - It is a residue of S-1's class, not a reopening of the spellings the RemPlan named.
- **Correction:**
  - (a) Narrow the module text to what `canonicalize` gives: symlinks, `..`, and case or short names where the platform's resolver corrects them. Name firmlinks, bind mounts and Linux case-folding volumes as not merged.
  - Or (b) key on filesystem identity: `(st_dev, st_ino)` on Unix, volume serial plus file index on Windows. Put that in one `workspace_identity` function that 2.3f's `ws-<key>` also uses, so the lock and the server never disagree.
  - Either way, make `case_variant` detect sameness with dev/ino or file-id rather than `canonicalize`.
- **Closure test.** For (b): the firmlink row (macOS, inside `cfg_if!`) waits. For (a): a doc diff, plus `case_variant` reporting a case-folding volume by inode, whether it then runs or explicitly skips.
- **Architectural?** No. The key choice is local to the manager.

### R2S-2 (P3) — guard lifetime over the exchange is pinned by nothing at the `let` sites: `let _ = steps.enter(..)?` releases before the network call and passes the check, clippy and every row

- **Root cause.** The guard is held by a binding. At the `let` sites, binding it to `_` drops it at once:
  - tag push `handle_tag.rs:353`;
  - tag fetch `:370`;
  - tag delete `:275`;
  - pull member preflight `member_preflight.rs:91`;
  - (also root `root_lock.rs:57` and merge plan `pull_head_merge_preflight.rs:92`).

  `enter` still *blocks* while another holder holds the member. Every row asserts "waits", "nothing ran while held" and "succeeds after". So a guard released before the exchange satisfies them all.

  Three tools miss it:
  - **The check.** It accepts any `.enter(` earlier in an enclosing block, bound or not. Its docstring says the `.enter(` "holds its guard over the call".
  - **rustc.** `#[must_use]` does not fire on `let _ =`. rustc's own suggested fix for an unused `steps.enter(..)?;` is to write `let _ = …`.
  - **clippy.** Its default set does not flag it.

  The fetch (`.and_then(|_guard| …)`) and push (`match … { Ok(_guard) => … }`) sites stay held even if respelled `_`, because closure arguments and match scrutinees live to the end of their scope.
- **Violated invariant.** `membermutationguard.rs` table line 6 says the guard is held "from before the exchange to after the ref update". The RemPlan P3-C2 check is meant to refuse an unlocked network update in `workspace_ops`.
- **Reproduction.** In my copy, I replaced `let _guard =` with `let _ =` at the tag push, tag fetch, tag delete and pull member preflight sites:
  - `check_member_step_boundary.py` reports "ok";
  - `cargo clippy --lib -D warnings` is clean;
  - `memberlockmanager`, `session_host::served`, `g29`, `g26` and `g17` pass 98, fail 0;
  - `a_pulls_preflight_fetch_beside_a_fetch_…` passed 6 of 6 under the mutation, because the fetch thread wins the race and the pull's `enter` waits.
- **Other inputs the check misses.** The crafted inputs `findings()` also misses:
  - an explicit `drop(guard)` before the call;
  - an unrelated `span.enter()`;
  - a receiver not named `*backend` (`git.fetch(`);
  - UFCS `GitBackend::fetch(backend, …)`;
  - a chained receiver `backend.inner().fetch(`;
  - `fetch_anonymous`, `push_anonymous`;
  - a closure created under the guard and called after it.

  The unguarded control is caught. These are within the check's self-described "token scan" limit. The `let _` and `drop` cases contradict its stated rule.
- **Impact.** No current defect: every site binds `_guard` today. A one-token edit, the one rustc suggests, silently turns the lock into "wait, then run unlocked", and nothing goes red. That reopens the C10 race on tag and pull paths.
- **Correction:**
  - (1) In the check, require the `.enter(` result to be bound to a named, non-`_` binding (`let <ident>`, `|<ident>|`, `Ok(<ident>)`) or to be a match scrutinee or `and_then` receiver, and refuse a `drop(<that ident>)` before the call. Add `fetch_anonymous` and `push_anonymous` to the call set, or list them with a reason.
  - (2) Add one "held over the exchange" assertion to the tracking and recording doubles: their `fetch`, `tag_fetch`, `push` and `push_prepared` record `manager.held_count() >= 1`, or a probe `lock` that must wait while they run, and the rows assert it.
  - Structurally better, if the operator wants it: a closure port, `steps.step(root, id, || backend.fetch(..))`, so a guard cannot be dropped early. This would change the port's one-argument shape, which the RemPlan kept, so it is the operator's call.
- **Closure test.** The `let _` mutation at each of the six `let` sites makes the check exit 1 and makes at least one row per site red.
- **Architectural?** No. It is a tripwire and coverage gap.

## 3. Invariant analysis

**Key identity and the fail-closed direction.**
- The token registration happens first, so a cancelled call answers `cancelled` before the root is looked at (a row pins this).
- Canonicalize runs next. On failure it returns `IoError` naming the root, with nothing held and nothing counted.
- No path keys on the spelling.
- `..` and symlinks merge, and macOS case merges (confirmed: `/VOLUMES/GWZTMP/rv-state/casedir` resolves to `/Volumes/gwztmp/rv-state/CaseDir`). Windows verbatim and case are pending the receipt. Residuals are in R2S-1.

**A root replaced or no longer resolving between locks.**
- Each `lock` canonicalizes independently.
- A root renamed mid-call makes later members refuse `IoError`, which fails closed.
- A new directory at the old path while the old one is held gets the same key and waits: over-exclusion, which is safe.
- The handler's git operations use the spelled path. A symlink re-pointed between `lock` and the exchange is a pre-existing TOCTOU of path-based handlers, not of the lock.

**Two workspaces with one key.**
- This needs two distinct directories to canonicalize to one string. That cannot happen concurrently; a canonical path names one directory at a time.
- A sequential replacement is covered above.

**Lock ordering.**
- No step holds two member guards at once. Fetch and push take one per `par_map` item. Push takes `@root` after the member map completes. Tag takes one per loop iteration. Pull's guard blocks close before the next step.
- Push, tag and pull take the workspace mutator first, as a try-lock that never waits. Nothing takes the mutator while holding a member guard.
- Inside `lock`, locals drop in reverse order on every exit: state guard, then key, then registration. So the token's callback lock is never taken while the state mutex is held. The cancel callback takes only the state mutex. No inversion.

**Cancel or error part-way through a multi-member step.**
- Fetch and push: the refused member's row is `cancelled`, and the other members proceed or refuse. Each guard is item-scoped.
- Tag and pull: `?` exits, earlier guards have already dropped, and earlier tag transfers stay done. That is the old behaviour, recorded in round 1.

**Cancel wake-up races.**
- `wake()` takes the state mutex, then notifies. The waiter checks `is_cancelled` under the mutex and parks atomically. The token sets its flag before running callbacks. So no notify is lost.
- The waiter count is never observed before the waiter has parked (§1, P3-C5).
- One limit: `waiting` is manager-wide, not per key. A row that waits for count 1 while another member's waiter exists could be satisfied vacuously. None of the current rows has that shape, and the two-waiter row asserts count 2 and then exactly 1.

**Panic while holding.**
- `Release::drop` runs during unwinding and recovers a poisoned state mutex.
- The state mutex is held only across a set operation or a counter step.
- Mutations E and F are recorded red in the README. I did not re-run them.

**Guard lifetime.** Each site's guard spans the exchange and its ref reads:
- fetch: `read_ref` before, `fetch`, `read_ref` after, `ahead_behind`;
- push: `push_prepared`, including libgit2's tracking update;
- pull: `fetch` plus the tracking-ref read;
- tag: push, fetch and delete per iteration.

That is true of the code as written. What pins it is R2S-2.

**Unlocked ref-updating paths after the dead branches were removed.**
- In `workspace_ops` production files, every `backend.{fetch,push,push_prepared,tag_fetch}` follows an `.enter(`; the check confirms this, and I grepped the multi-line call sites too.
- The dry-run plans are read-only: `planned_push` and `validate_remote_identity` read, and the remote-reading preflight `read_before_transfers` writes no refs.
- Outside `workspace_ops`, `local_clone::transport` calls `fetch_anonymous` and `push_anonymous`. The check does not scan them. They belong to the W-class local imports (§5 below).

**`@root` against a member keyed `@root`.**
- Member ids must parse with the `mem_` prefix (`artifact/validation.rs:35-51`), so no member can be `@root`.
- Every root key comes from a literal `"@root"`: fetch `handle_fetch.rs:211`, push, pull root and merge plan; or from `selected` for tag, where the resolver yields `"@root"`.
- With the fallback search gone, tag push cannot key a member as the root.

**One manager per host.** `member_locks(worker)` reaches `scope.session().members()`, which is the host's clone. Two host contexts share no manager (control row green). This is unchanged from round 1.

**Repo rules on the new code:**
- **conditional compilation:** inside `cfg_if!` everywhere (`test_support/mod.rs`, `spellings.rs`, the manager's test arm, the Unix rows); `check_cfg_boundaries` reports nothing new;
- **globals:** none; `check_process_globals` reports nothing new;
- **`tests/`:** no production code under it;
- **braces:** Rust requires them;
- **parity:** the symlink rows are Unix only, Windows' spelling row is the case variant, and it is pending the receipt.

## 4. Risks and next action

**Next action.** GO for 2.3c. R2S-1 and R2S-2 may be fixed in a follow-up patch or carried with a record:
- R2S-1(a) is a doc and test-helper change;
- R2S-2(1) is a check change of about 40 lines;
- R2S-2(2) adds one assertion per double.

**Dependencies.** The WSL run `-b` and the Windows receipt `-a` must still show:
- the case-variant rows running and green on NTFS;
- the four Linux-only failures green on ext4.

My GO on S-1's Windows half depends on that receipt.

**Hypothesis, not reproduced (Windows).** Windows `std::fs::canonicalize` (`GetFinalPathNameByHandleW`) is known to fail on some virtual volumes, such as RAM disks and some FUSE-like drivers.
- If it does, every member step there now refuses `IoError`, including direct gwz-cli and gwz-py callers. Their per-call manager gains nothing from identity, because one call uses one spelling.
- This fails closed, but it would regress availability on such volumes.
- If the receipt host has no such volume, consider letting `BoundMemberLocks::per_call` key by spelling, which is sound within one call. Any change must not weaken the served path.

**Residual limitations, unchanged from round 1:**
- no hold bound on a hung exchange; waiters stay cancellable;
- no waiter order;
- tag and pull hold the mutator during a member wait.

**Release record location.** The P3-C7 record lives in gwz-core-evidence, a private evidence member, and gwz-core keeps no unreleased-notes file. The 1.1.0 checklist owner must know to read it there.

## 5. Out-of-scope observations

- **W-class local imports.** `local_clone/transport.rs:101-125` writes refs through `fetch_anonymous` and `push_anonymous` with no member lock, and `check_member_step_boundary.py` neither scans that module nor matches those names.
  - These are W-class local imports under the workspace mutator. §5.1 separates them from N steps through the class rules and the workspace registry (2.3i), not through the member lock.
  - The RemPlan's 2.3i record names only `materialize` and `clone_repo_member`. I suggest extending it to name the local-import fetch and push. I did not trace exactly which served arms reach them.
- **Push plans from pre-guard reads.** Push's preflight (`planned_push`, read-before-transfers) reads tracking refs before the member guard. The transfer itself is under the guard, and the remote enforces fast-forward, so a stale plan can only produce a rejected push, not a wrong ref. This has the same shape as the old P3-C4, but it is plan-only and no ref is written.
- **A row that cannot catch an early release.** `a_pulls_preflight_fetch_beside_a_fetch_of_one_member_…` passes 6 of 6 even under the early-release mutation (R2S-2), because the fetch thread reliably takes the member first. It only catches a lock-less pull.
- **Cleanup.** I could not delete my empty probe directory `/Volumes/gwztmp/rv-state` with `rm -rf`: a built-in safety check refuses top-level removals on a mounted volume. I removed it with `rmdir`, and it is gone.

Files referenced:
- `/Volumes/projects/limbo/gwz-dev-p2-3bc2/gwz-core/src/operation/memberlockmanager.rs`
- `/Volumes/projects/limbo/gwz-dev-p2-3bc2/gwz-core/src/test_support/spellings.rs`
- `/Volumes/projects/limbo/gwz-dev-p2-3bc2/gwz-core/scripts/checks/check_member_step_boundary.py`
- `/Volumes/projects/limbo/gwz-dev-p2-3bc2/gwz-core/src/workspace_ops/handle_tag.rs`
- `/Volumes/projects/limbo/gwz-dev-p2-3bc2/gwz-core/src/workspace_ops/pull_head_member_preflight/member_preflight.rs`
- `/Volumes/projects/limbo/gwz-dev-p2-3bc2/gwz-core/src/workspace_ops/pull_head_member_preflight/root_lock.rs`
- `/Volumes/projects/limbo/gwz-dev-p2-3bc2/gwz-core/src/workspace_ops/pull_head_merge_preflight.rs`
- `/Volumes/projects/limbo/gwz-dev-p2-3bc2/gwz-core/src/local_clone/transport.rs`
