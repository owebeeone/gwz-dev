# Step 2.3c (member lock manager) — Code-AXIS REVIEW
**Review object:** gwz-core 3d4cff0a in lane p2-3bc; evidence 2026-10-31-p2-3c
**Baseline:** I read `git -C gwz-core show 3d4cff0a` in full, plus the files it touches at 3d4cff0a. I read the controlling documents at lane root e6037850. All builds and tests ran in my own `rsync -aL` copy (`build-targets/review-p2-3c-code-src`, target dir `review-p2-3c-code-target`, `TMPDIR=/Volumes/gwztmp`, rustc 1.95.0, Mac only). The copy is now deleted.
**Date:** 2026-10-31
**Axis:** Code. Architecture, interfaces, the call graph from `served/dispatch.rs` to every step that updates refs, the lock key, public API, and parity.
**Verdict: NO-GO** — 0 P0, 0 P1, 1 P2, 6 P3. I commit to GO on a revision that closes P2-1 with the closure rows below. The P3s may be fixed or recorded as carried; none of them blocks.

## 0. Evidence base

- **Tuple, checked at start and end:**
  - root e6037850 (index has the lane's existing staged `gwz.conf` changes and untracked operator drafts; nothing else);
  - gwz-core 3d4cff0a, parent 76507c49, clean;
  - gwz-transport a3f9ed0;
  - gwz-core-evidence f5fcb4f6.
- **Documents read:**
  - Step2.3-Plan C10 and C13 rows, and §3 2.3b and 2.3c;
  - `GwzSplitCommandChannel-Step2.2c-RemPlan.md:55-65`;
  - `GwzCoreSessionDesign.md` §5.1 and §5.3;
  - contract §15 (the `OpenFailure::Fault` sentence: 2.3b context, not judged here);
  - ConvergencePlan :675-680;
  - the evidence README;
  - gwz-core `AGENTS.md` and `RELEASE.md`.
- **Baseline in my copy.** The step's four row groups (`session_host::members`, `workspace_ops::tests::g29`, `g26::tag_member_step`, `session_host::served::tests`): 39 passed, 0 failed.
- **Mutations re-run in my copy.** I used the drafter's `mutate.py`, retargeted at my copy, plus one mutation of my own (S). The source was restored after each run and checked identical to the lane by `diff -r`.

| id | result in my copy | compared with the drafter's record |
|---|---|---|
| D (no wake on cancel) | 9 rows red. Six `core_dispatch_*` served rows also never ended, and the binary was killed at 900 s. | Record says 6 red, no hang (see P3-4). |
| G (key ignores the member) | 2 rows red (`different_members_do_not_wait_for_each_other`, `fetches_of_different_members_still_overlap`). `a_handler_port_without_a_token_waits_and_with_one_is_cancellable` deadlocked itself, killed at 900 s. | Record says 2 red, no hang (see P3-4). |
| R (tag fetch unlocked) | 3 red, 36 passed. | Matches `mutate-3c-fifth-pass.out` exactly. |
| S, mine (key ignores the workspace) | 1 red (`different_members_do_not_wait_for_each_other`, which has a `/elsewhere` arm). | The workspace part of the key is pinned. |

- **Reviewer rows (§1 P2-1).** I added three served rows that run `CoreDispatch` fetches with real git. The control passes. The symlink row and the case-variant row are red: the fetch completed (`Noop`) while another holder held the same member, and the cancel got `OperationExpired` because the call had already been answered. `/Volumes/gwztmp` is case-insensitive, so the case-variant row ran rather than skipped.
- **Source checks in my copy:** all exit 0, nothing new.
  - `check_cfg_boundaries.py --skip-repo gwz-cli --skip-repo gwz-py` (the copy has no gwz-cli or gwz-py);
  - `check_process_globals.py`;
  - `check_windows_parity.py`.
- **Not re-run:** the lane owner's WSL gate (`p2-3bc-remote-20261031-a`, OVERALL PASS) and the Windows receipt (`p2-3bc-20261031-a`, 4391 passed, 3 named host gaps). I took both from `p2-3bc-logs/remote-a.log` and `win-p2-3bc-20261031-a.log`.
- **Host load:** load average reached 140 during the runs (other lanes). Timings are not meaningful.

## Problem, requirement provenance, and scope

- **Requirement.** C10 (plan :675-680, carried from the 2.2b review) asks for design §5.1's member lock manager in the host context. It must:
  - block rather than refuse;
  - recover from poisoning;
  - wake a wait on cancellation;
  - be shared, so "even sessions that share a host context never update the same member's refs at once".

  The plan's step text names the fetch and push member steps. Its red rows are: fetch beside fetch waits then both succeed; two sessions never update a member at once; a wait wakes on cancel; a panic while holding fails no later operation; different members do not wait.
- **Widening.** The lane owner extended the lock to pull_head's preflight fetches (member, root, merge plan) and to tag push and tag fetch. This is necessary, not optional. Those calls are network ref updates of the same refs (`backend.fetch` updates `refs/remotes/*`; `tag_fetch` and the tag `push_prepared` write tags and tracking refs). Leaving them out would leave the §5.1 invariant false on paths the served host dispatches. All edits stay inside gwz-core, the plan's owner.
- **Size.** The plan budgets about 300 implementation lines and 300 row lines. Implementation is close to that (members.rs is 179 lines plus about 250 lines of plumbing). Rows are about 1,300 lines. That is above the estimate, but it is not a defect.
- **Mechanisms, each necessary:**
  - the manager (a mutex-protected set plus a condition variable);
  - the guard type, with the release type-erased so the `operation` layer names nothing of the host;
  - the `MemberSteps` port;
  - the per-call manager for direct callers, which keeps CLI and gwz-py behaviour exactly as before (`MemberLockManager::try_lock` had no production caller);
  - `PreflightPorts`, which exists only for clippy's argument limit (lane owner accepted).
- **Exclusion.** The dry-run push helpers take no lock. A dry run plans with local reads only (`push_state::planned_push` calls only `read_ref`), so the exclusion is sound, apart from P3-3.

## 1. Findings

### P2-1 — The lock key is a path spelling, not the workspace's identity: two spellings of one workspace never exclude each other

- **Root cause.** `MemberKey.workspace` (`session_host/members.rs:30`, built at `:81`) is the `root` its caller resolved. Every caller gets that root from `resolve_request_workspace_root`:
  - `handle_fetch.rs:99`;
  - push and pull through `resolve_guard_root_from_request`;
  - `handle_tag.rs:85-96`.

  That function only normalises the path lexically (`normalize_absolute_path` → `lexical_normalize`, which by its own doc "does not resolve symlinks"). `discover_workspace_root` walks the spelling it is given. So the key identifies a request's spelling of the root. The contended resource is the member repository's ref store on disk.
- **Invariant violated.** Design §5.1 says sessions of one host context "never update the same member's refs at once". The plan's 2.3c row says "two sessions of one host never update a member's refs at once". C10 is closed only for callers that happen to agree on one spelling.
- **Reproduction** (my copy, `session_host/served/tests.rs`, using the drafter's `rooted_workspace`, `Session::on`, `root_only`, `handler_started`):
  ```rust
  let (temp, root) = rooted_workspace();
  let alias = temp.path().join("alias");               // or root.with_file_name("WS") on a case-insensitive volume
  std::os::unix::fs::symlink(&root, &alias).unwrap();
  let host = HostContext::new();
  let holder = host.members().lock(&root, "@root", None).unwrap();
  let mut session = Session::on(&host, &alias, Arc::new(CoreDispatch::new(Git2Backend::new())));
  let fetch = session.send("fetch", None, encode(&FetchRequest { meta: RequestMeta {
      selection: Some(root_only()), ..meta(&alias, "fetch-alias") } }.to_cbor()), Lane::Call);
  handler_started(&mut session, "op-1");
  let cancel = session.cancel(fetch);
  // expected (lock honoured): row Failed/cancelled, as with the resolved spelling
  // observed: row Noop, error None; the cancel answers OperationExpired (the call had already finished)
  ```
  Results:
  - Control with the resolved spelling: `Failed`/`Cancelled`, and the cancel reply is `Ok`.
  - Symlink spelling: `Noop`, the cancel gets `OperationExpired`.
  - Upper-case `WS` on `/Volumes/gwztmp`: the same.
- **Impact.** Concurrent fetches (or fetch beside pull, tag fetch or push) of one member, through two spellings in one host context, run their exchanges and ref updates at once. One can again fail on Git's ref lock, which is exactly the failure C10 exists to remove. This is a failed member row, not corruption. The spellings occur in practice:
  - **gwz-cli `--root`** is resolved lexically (`gwz-cli/src/clirequest/invocation.rs:46`). On this host `/Users/owebeeone/limbo` is a symlink to `/Volumes/projects/limbo`.
  - **macOS:** default APFS is case-insensitive, and `/tmp`, `/var` and `/etc` are symlinks to `/private/…`.
  - **Windows:** paths are case-insensitive, and 8.3 short names occur (the hosted job's TEMP is one, per gwz-core `AGENTS.md`). `lexical_normalize` folds only the drive letter and the verbatim prefix.
  - **Linux:** symlinks and bind mounts.

  So the fault also has a parity side: it shows up more on Mac and Windows. The project already keys the server on "the canonical absolute workspace root" (`GwzSplitCommandChannel.md:25`, plan 2.3f), so the member lock disagrees with the server's own idea of which workspace it is.
- **Correction.** Key the workspace by its filesystem identity. Canonicalise once in `MemberLockManager::lock` (`std::fs::canonicalize`; on Windows it yields on-disk case and a stable verbatim form), using the same function 2.3f will use for `ws-<key>`. If canonicalisation fails, either refuse or fall back to the lexical path, and say which in the module table. An alternative is to bind the canonical root into `BoundMemberLocks` once per call, so that `enter` takes only the member.
- **Closure test.** The three rows above, made permanent: symlink (Unix, inside `cfg_if!`), case variant (skipped on a case-sensitive volume), and the resolved-spelling control. All must answer `cancelled`. Add a mutation that drops the canonicalisation; both variant rows must go red.

### P3-1 — Module docs disagree with the code and the commit (stale after revision 3)

- **Locations:**
  - `session_host/members.rs:12` lists only "`tag`'s push";
  - `members.rs:13` says "`tag`'s fetch (`tag_fetch` …) is not plumbed yet and is reported as a finding";
  - `served/dispatch.rs:11` says "`tag` (its push)";
  - `operation/membermutationguard.rs:8` says "a handler sees only `MemberSteps`", but the four public handlers construct `session_host::members::BoundMemberLocks::per_call()` themselves;
  - `membermutationguard.rs:33` says "What a fetch or push handler takes", but pull and tag take it too.
- **Invariant:** module tables state behaviour. The code (`handle_tag.rs:358-364`), the commit message and the evidence README say tag fetch is locked.
- **Reproduction:** read the lines above against `handle_tag.rs:361`.
- **Impact:** a later step may "fix" a finding that does not exist, or miss a real one.
- **Correction:** rewrite the rows to match. The tag row names push and fetch; the exclusion row names only the dry-run helpers.
- **Closure:** a doc diff, plus a grep finding no "not plumbed" text.

### P3-2 (ARCHITECTURAL) — A new edge from the handler layer into the session host closes a module cycle

- **Root cause.** `workspace_ops/handle_fetch.rs:28`, `push_member.rs:10`, `pull_head_member_preflight/handle.rs:9` and `handle_tag.rs:43` import `crate::session_host::members::BoundMemberLocks`. At 76507c49 no `workspace_ops` file names `session_host`; only `git/endpoint/*` used `session_host::environment`. The session host already depends on `workspace_ops` through `served/dispatch.rs`, so this creates a cycle: `session_host::served` → `workspace_ops` → `session_host::members`.
- **Invariant:** the change's own claim that the guard module sits "below the handlers" and "names nothing of the session host". It also cuts against the crate map's direction: session host pieces move into crates that core composes.
- **Impact:** extracting either side later has to untangle the cycle first. Today the impact is on maintainability only.
- **Correction:** pick one.
  - Give the port its own per-call implementation beside it in `operation`, so handlers name only `operation`.
  - Move `MemberLockManager` and `BoundMemberLocks` into `gwz-session-host`. It already owns `CancellationToken`, and the manager needs only std plus the token. Core's `MemberMutationGuard::new` would wrap the crate's guard.
- **Closure:** `git grep -n session_host -- src/workspace_ops` finds nothing outside `tests/`.

### P3-3 — The exclusion rests on dead, unlocked push branches; nothing checks that call sites stay inside a member step

- **Root cause.** `push_member` and `push_root` take `dry_run`, and all four callers pass `true` (`push_member.rs:150, :154, :178, :182`). Their live branches, `backend.push` at `:541` and `:635`, are therefore unreachable. They are real network ref updates with no member guard. The step's claim that every path is locked is true only because those branches are dead. Nothing enforces it: a future handler that calls `backend.fetch`, `push_prepared`, `tag_fetch` or `push` outside `steps.enter` compiles and passes every row.
- **Invariant:** the step's claim that every network ref update takes the lock; the standing rule to remove dead code once proven dead; the standing rule to prefer fast source checks.
- **Reproduction:** grep the four callers; all pass the literal `true`.
- **Impact:** the guarantee is latent, not structural.
- **Correction:**
  - remove the `dry_run` parameter and the push branches (proven dead above);
  - add a fast syntax-aware check (like the `check_target_selection_boundary` family) that every `backend.{fetch,push,push_prepared,tag_fetch}` call in `workspace_ops` sits inside a `steps.enter` guard scope.
- **Closure:** the check passes on the tree and fails on mutations I, J, R. The existing push rows stay green.

### P3-4 — The served rows hang instead of failing under the regressions they exist to catch; the recorded D and G results predate the final rows

- **Root cause.** `Session::reply` (`served/tests.rs:324-330`) checks its 20 s deadline only between frames, while `self.client.recv()` blocks with no bound. The `core_dispatch_*` rows hold the member until after the reply, so under mutation D the row deadlocks: the reply waits for the fetch, the fetch waits for the holder. Separately, `a_handler_port_without_a_token_waits_and_with_one_is_cancellable` holds `@root` and enters `mem_01` on one per-call port. Under mutation G that self-deadlocks.
- **Invariant:** a red row fails in bounded time and names itself (diagnosability).
- **Reproduction:** see §0. D hung 6 served rows and G hung one row; both binaries were killed at 900 s. The drafter's records for D (second pass) and G (first pass) show no hang. They ran before `handler_started`'s settle and before the per-call tail of that test existed.
- **Impact:** no false green, since a hung run fails CI, but a regression costs 15 minutes and does not name the row.
- **Correction:**
  - use `recv_timeout` against the deadline in `reply`;
  - in that one row, take the two per-call entries in sequence rather than nested.
- **Closure:** re-run D and G; every red row fails within its own bound, with no harness kill.

### P3-5 — Tag push finds its member key by searching paths, with a silent `@root` fallback

- **Location:** `handle_tag.rs:339-344`.
- **Root cause:** `member_roots.iter().position(..).map_or_else(|| "@root".to_owned(), ..)`. The search always succeeds today, because `push_plans` is built from `member_roots`. If it ever missed, a member would be keyed as the root, which locks the wrong resource and leaves the member unlocked.
- **Correction:** carry the member id with the plan (zip `selected` into `plan_tag_pushes`' output, as the fetch arm already zips `member_roots` with `selected`), or `expect`.
- **Closure:** the existing tag rows stay green, and the fallback no longer exists.

### P3-6 — Public API removal in a published 1.x crate is not recorded for the release

- **Root cause.** The change removes `gwz_core::operation::MemberLockManager`. It also changes `pub struct MemberMutationGuard<'a>` to `MemberMutationGuard`, dropping the lifetime. That is a second break, which the commit's "MemberMutationGuard stays" understates. gwz-core is published to crates.io under semver (`RELEASE.md:78-80, :93`), and the next release is 1.1.0, a minor bump.
- **Workspace callers:** none. I grepped gwz-cli, gwz-py, taut, gwz-transport, scripts and docs; only history documents mention the type.
- **Impact:** a downstream crates.io user would break on a minor update. Likelihood is near zero. The guard type is also now nameable outside the crate but impossible to obtain there.
- **Correction:** record both breaks in the 1.1.0 release decision or notes. Either make the guard `pub(crate)` in the same breaking change, or say why it stays public.
- **Closure:** the record exists and names both items.

## 2. Invariant analysis

- **Call graph.** These are the `served/dispatch.rs` arms that reach network ref updates, and what they lock:

| arm | step | lock | row |
|---|---|---|---|
| fetch | `fetch_one`, `handle_fetch.rs:423` | per member and `@root` | I |
| push | members `:359`, root `:419` | yes | J, K |
| pull_head | member preflight fetch `member_preflight.rs:88` | yes | L |
| pull_head | root fetch `root_lock.rs:55` | yes | M |
| pull_head | merge-plan root fetch `pull_head_merge_preflight.rs:90` | yes | N |
| tag | push `:345` | yes | O |
| tag | fetch `:361` | yes | R |

  - The dispatch arms themselves are pinned by mutations B, B2, P and Q.
  - Not locked, and correctly so: `ls_remote` reads and dry-run plans.
  - `materialize` and `clone_repo_member` clone new repositories, and `merge`/`local_family` do local `fetch_anonymous`. All are W class under the workspace mutator lock, not §5.1 member steps (see §4).
  - No ref-updating network path the host serves is left unlocked, except the dead branches in P3-3.
- **Blocking, not refusing.** `lock` waits on a condition variable (mutation H gives 15 red rows). The drafter's note is sound: a tag push and a push meet as a mutator-lock refusal, not a wait, because the workspace registry (CS2.9) does not exist yet. The tag rows use a holder instead, which is correct.
- **Cancellation.** The token sets its flag before running callbacks (`token.rs:56-61`). `on_cancel` checks and registers under the callbacks lock. `wake` takes `held` before notifying, and the waiter checks its token under `held`. Together these rule out a lost wakeup. A token already cancelled refuses before any wait. Mutation D is red (with P3-4's caveat).
- **Fetch and push versus pull and tag on cancel.** Fetch and push report a cancelled row inside a completed call; pull and tag fail the call. This matches §5.3: a cancelled network row keeps its code, and other gate crossings fail the call. It is not a defect.
- **Poison and panic.** `Release::drop` runs during unwinding, and `held()` recovers from poisoning. Mutations E and F are red.
- **Deadlock.** No path holds two member guards at once:
  - fetch and push take one guard per member inside `par_map`;
  - tag takes them in sequence;
  - pull's guard scopes are closed before the next step.

  The workspace mutator lock is a try-lock and never waits, so lock order has no cycle. A detached worker keeps its guard until its step ends, which is what §5.1 asks for across a close bound.
- **Key.** The member part is unique within a workspace (`mem_` ids versus `@root`), pinned by mutation G. The workspace part is pinned by mutation S. It is a spelling, not an identity (P2-1).
- **Interfaces and compatibility.** The public handlers keep their signatures and wrap the new `pub(crate)` `*_with_member_steps` functions. The changed `pub(crate)` `_in` functions have only in-crate callers, and the ordinary and candidate shapes both build (lane gates). The public API change is P3-6.
- **Repo rules.**
  - Braces: Rust always requires them.
  - Conditional compilation: the only cfg is `cfg(test)` inside `cfg_if!` at `members.rs:160`.
  - No globals: the manager lives in `HostShared`, and `check_process_globals` reports nothing new.
  - No production code under `tests/`.
  - gwz-transport is untouched.
- **Parity.** The new code is pure Rust with no cfg, so the plan's `ci-only` waiver holds, and the Windows receipt `p2-3bc-20261031-a` covers the lane tip anyway. The commit could cite that receipt label instead of the waiver. That is cosmetic. The parity weakness is semantic and sits in P2-1: case and short-name spellings.
- **Red rows the plan names:** all present and mutation-red. The "fetch beside fetch" row is handler-level over the tracking double (`g29.rs:121`), not real git in one served session. It is still deterministic and red under mutations I, A and H.

## 3. Risks and next action

- **Next action:** fix P2-1 in `members.rs` (canonical workspace identity) with the three closure rows and a mutation, then re-review that diff alone. Within the same revision, fixing P3-1 and P3-4 is cheap. P3-2, P3-3, P3-5 and P3-6 may be fixed or carried with a record.
- **Risk for 2.3g/2.3h:** any second constructor of `ServedSession` (the byte-stream session) must take `host.members()`. Mutation C protects only `serve_in_process`.
- **Limitation, not a finding:** `notify_all` wakes every waiter of every member on each release, and waiters take a freed member in no promised order (documented). The number of waiters is bounded by sessions × 8 running, so this is acceptable for now.

## 4. Out-of-scope observations

- W-class clones (`materialize` `apply.rs:63`, `clone_repo_member`) write a member's refs from the network with no member lock. §5.1 keeps an N step from running beside a W operation through the class rules and the workspace registry (CS2.9, C20), not the member lock. That registry is not built yet, so a served fetch beside a clone of the same member in one host is currently possible. This belongs to 2.3's registry work, not to 2.3c.
- The `TrackingBackend` now answers `ahead_behind` with 0/0 for every test that uses it. This was not observed to weaken any existing assertion; I am noting it so later rows that need ahead/behind values set them explicitly.
