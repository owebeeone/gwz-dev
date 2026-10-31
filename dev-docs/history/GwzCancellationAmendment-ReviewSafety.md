# GwzCancellationAmendment-DRAFT r1: Safety-axis review

**Review object:** dev-docs/GwzCancellationAmendment-DRAFT.md, blob 3448c561 (root 9d5a4d4c), revision 1, DRAFT
**Baseline:** root 9d5a4d4c; gwz-core b745f1fa (code citations checked at the amendment's stated citation root a519ee58 / gwz-core 07e99924; the cited gwz-core paths did not change between 07e99924 and b745f1fa); gwz-cli c0ba717; gwz-py 2185cc6; libgit2 b172e3d18; gwz-transport 281a28d. Sources were read with `git show <rev>:<path>`, `git grep <rev>` and plain reads of the working tree. Nothing was built or run.
**Date:** 2026-11-01
**Axis:** Safety: what the text allows to go wrong. Independent, adversarial, read-only. The other axis ran in parallel and nothing here relies on it. Filed verbatim by the lane owner.
**Verdict: NO-GO.** 0 P0, 0 P1, 11 P2, 5 P3. Every blocking finding has a bounded fix in the amendment text. P2-5 and P2-7 also need the operator to choose a route. I pre-commit to GO on a revision that resolves P2-1 to P2-11 as specified.

---

## 0. Evidence base

- **Object pin.** `git rev-parse 9d5a4d4c:dev-docs/GwzCancellationAmendment-DRAFT.md` returned 3448c561a37f60149b977290da8a62d1f34481f2 at the start and at the end. The working file's `git hash-object` returned the same value both times. The content did not move during the review.
- **Controlling decision.** `GwzTransportReleaseConvergencePlan.md` §10 D-V, :999-1031: OQ2 is reversed ("Ctrl-C leaves no orphaned process, stale lock or partial state behind"), and OD-C1 to OD-C6 were decided as recommended on 2026-11-01.
- **Code read for this review:**
  - https_auth: `process_tree.rs` (Unix spawn/kill/retire/drained), `process_tree/windows.rs`, `owner.rs:253-303`, `pipes.rs:9-23`, `lookup.rs:126`
  - Other gwz-core: `operation/commit_log/mod.rs:330-358`, `artifact/{encoding,durable_write,mod}.rs`, every `write_atomic*` call site, `verified_write.rs`, `merge/v1_lifecycle/store/rewrite.rs:219-235`, `crates/family-store/src/{lib,lock,publish}.rs`, `crates/ids/src/lib.rs`, `operation/{workspace_mutator_lock,memberlockmanager}.rs`, `merge/runtime/mutation_guard.rs`, `gitbackend/transport_support.rs:81-98`, `served/dispatch.rs:99-108`, `service/workers.rs:213-236`, `handle_local.rs:72`, `check_process_globals.py:102-105`
  - gwz-cli: `globalargs/dispatch.rs:65-82`
  - gwz-py: `bridge.py:29-35`, `native/src/route/transport.rs`, `pyproject.toml` (`[project.scripts]`)
  - libgit2: `util/futils.c:28-50` (mktmp names), `libgit2/indexer.c:168-232`, `pack.c:1050-1072`, `util/filebuf.c:44-52`
- **Documents read:** the parked server design :800-815, :1100-1112, :1212-1228; the split doc §4.2-§4.4 (at 9d5a4d4c and at a519ee58); session design :680-692 and §5.3/§8 headings; contract §5 :250-290; TR1.6 :70-80 and :394-400; Step 2.3 plan 2.3g, 2.3i, 2.3j and 2.3k.
- **Platform facts relied on (stated, not measured here):**
  - A terminal's SIGINT, SIGQUIT and SIGHUP, and shell job-control signals (`kill %1`), go to the whole foreground process group.
  - A CTRL_CLOSE handler that returns lets Windows terminate the process.
  - Rust's `eprintln!` panics when the write fails.
  - Rust std on Apple platforms creates pipes with `pipe()` followed by a separate `FD_CLOEXEC` step, not `pipe2`.
  - git prunes only `tmp_*` files in the object directory.
  - OpenSSH reads prompts from `/dev/tty`. A background process group that reads the terminal is stopped by SIGTTIN.

## Problem, requirement provenance, and scope

- **Requested outcome:** a design amendment, with no code, that meets D-V's reversed OQ2. Ctrl-C cancels end to end (the client, every helper it started, and a serving core's in-flight call). Each side detects a dead peer and cleans up. Nothing is left behind: no orphan, no stale lock, no partial state.
- **Where each mechanism comes from:**
  - Interrupt source (§3.1): the requirement directly. Before this, nothing turned Ctrl-C into a cancel (§2, confirmed: no handler in gwz-cli, gwz-core or gwz-py).
  - Safe points (§3.2): needed for "no partial state".
  - Child registry (§3.4): the requirement ("every helper process").
  - Lifeline reaper (§3.4): needed, because SIGKILL and macOS leave no other armed kill. It is also a new process type (a widened blast radius) and needs a complete contract (P2-1 to P2-5).
  - Temp sweep (§3.5): needed for "no partial state". It is also a new deletion path (P2-9).
  - `GIT_ELOCKED` reporting (§3.5): "no stale lock" combined with OD-C3.
  - Served-form rules (§3.6) and step placements (§4): the requirement's "serving core" clause.
- **Scope widening:** none found. Every mechanism traces to D-V.
- **Scope narrowing:**
  - C4's "never published" category admits leftover files that nothing ever removes (P2-8).
  - C2 is absolute but contradicts the retained `retire` semantics without saying so (P2-2).
  - OD-C6 leaves pager and `forall` children outside C2 without recording the exception (P3-4).
- **Deferred:** the choices made in OD-C1 to OD-C6 are not attacked. Their shape (specification, bounds, lifecycle) is.

## 1. Findings

### P2-1: The reaper is a new process type with no launch contract, so it dies together with gwz

- **Location:** §3.4 "macOS and Linux: a lifeline reaper"; §6 OD-C2 (a).
- **Violated invariant:** C2 ("however gwz ends: an orderly exit, a cancel, SIGKILL, a crash, or the console closing").
- **Root cause:** the text does not say what process group or session the reaper is in, what signal dispositions it has, or what environment, working directory and descriptors it gets. It does not say what happens if the reaper fails to start, or how gwz notices the reaper's own death (C3 requires every dependency to detect its peer's death; gwz depends on the reaper). Started the default way, the reaper inherits gwz's process group and default dispositions. Helpers deliberately lead their own groups (`process_tree.rs:71`).
- **Interleavings:**
  1. A `gwz fetch` is blocked in a sleeping helper. The user presses Ctrl-\ (SIGQUIT is not in C1's list), or the shell runs `kill -9 %1`, or a supervisor sends `kill -KILL -<pgid>`. gwz and the reaper die in the same delivery. The helper's own group survives, reparented to init.
  2. First Ctrl-C: the reaper, in the foreground group, dies of SIGINT while gwz starts its Cancel. Then gwz is SIGKILLed during cleanup. Nothing is armed and the helper is orphaned.
  3. The terminal closes. SIGHUP reaches gwz and the reaper. The reaper dies. gwz's Cancel path prints to the hung-up tty, `eprintln!` panics, and gwz dies before it kills anything.
- **Impact:** exactly the orphan the operator forbade, on the most common interrupt paths. Rows 1-3 send signals only to gwz's own PID, so all of them pass.
- **Required correction:** a normative reaper launch contract:
  - its own process group or session (so no terminal or job signal reaches it);
  - SIGINT, SIGQUIT, SIGHUP, SIGTERM, SIGTSTP and SIGPIPE ignored;
  - `env_clear`, working directory `/`, and only the lifeline and control descriptors inherited;
  - on start failure, gwz refuses to spawn the child (fail closed) and says so;
  - gwz watches the reaper (its exit or a pipe from it) and treats its death the same way.
- **Closure test:** rows with a sleeping helper and (a) SIGKILL to gwz's whole process group, (b) SIGQUIT to the group, (c) SIGINT to the group then SIGKILL to gwz, (d) a SIGHUP hang-up. In each, the helper's group is gone within 1 s. A further row kills the reaper alone and shows gwz refuses the next helper spawn.

### P2-2: When a group is unregistered is undefined against the leader's reap and against `retire`

- **Location:** §3.4 ("gwz tells the reaper each child's process group as it starts and ends"); C2.
- **Violated invariants:** C2, and the rule that the reaper never signals a group gwz does not own.
- **Root cause:**
  - On Unix, gwz cannot tell when a group "ends": `drained()` is `true` once the leader is reaped (`process_tree.rs:92-95`).
  - A successful helper is `retire`d and "what the helper left running is left running" (`process_tree.rs:12-13, 89-90`). §7 keeps that behaviour. For example, `git credential-cache` starts a daemon that stays in the helper's group.
  - The text fixes no order between registration and the spawn, or between unregistration and gwz reaping the leader.
- **Interleavings:**
  1. "Ends" means the group is empty. The retired group stays registered, and gwz's orderly exit closes the lifeline. The reaper SIGKILLs the credential-cache daemon after every gwz command. That is a regression against git and 1.0.18.
  2. "Ends" means the leader was reaped, but the message is sent after `wait()`. gwz is SIGKILLed between the reap and the write. The PID is recycled as an unrelated group leader (every shell job is one). The reaper's `kill(-G, SIGKILL)` kills the user's unrelated job.
  3. A long-lived `gwz server` registers retired groups for hours. The daemons time out, the PIDs recycle, and the server crashes: the reaper kills recycled groups.
  4. In the window between the spawn and the "start G" message, a gwz SIGKILL leaves the helper unregistered and orphaned.
- **Impact:** either kills the wrong processes or breaks credential caching, depending on how the implementer reads the text. In both cases C2 as written cannot be met and cannot be tested.
- **Required correction:**
  - Registration is in effect before the child can run unsupervised. For example, the child blocks before `exec` until registration is acknowledged, or registration goes through the reaper.
  - Unregistration is acknowledged before gwz reaps the leader. The zombie keeps the group ID from being reused.
  - `retire` unregisters.
  - C2 states the retire exception explicitly: processes a successful helper deliberately leaves running are not gwz's.
- **Closure test:**
  - A helper fixture succeeds and leaves a lingering descendant in its group. After gwz exits normally, and after gwz is SIGKILLed, the descendant is alive.
  - A reaper trace shows no kill is ever sent for a group whose leader gwz has reaped.
  - SIGKILL injected immediately after the spawn returns: the helper's group is still gone within 1 s.

### P2-3: The registry kills process groups, but two of its named children do not lead one

- **Location:** §3.4 "Registry" (which names `git rev-list` and "the `stdio`/`ssh` child"); §3.6; parked server design :1218-1224.
- **Violated invariant:** a reaper or registry kill reaches only the registered child's processes.
- **Root cause:**
  - `git rev-list` is spawned with no group (`commit_log/mod.rs:330-355`).
  - The `ssh` child keeps the client's controlling terminal for prompts, with SIGINT and SIGQUIT ignored (parked :1212, :1218). For `/dev/tty` reads not to be stopped by SIGTTIN, it must stay in gwz's foreground process group.
  - The amendment says the registry and reaper act on "each child's process group", and does not say what is registered for a child that leads none.
- **Interleaving:** the user runs `gwz --server ssh://h/w fetch | tee log`. gwz is SIGKILLed. The reaper's registered group for `ssh` is gwz's own group, so `kill(-pgid)` also kills `tee` and any other pipeline member (and the reaper, if it is still in that group). If the implementer instead moves `ssh` into its own group, host-key and passphrase prompts stop on SIGTTIN and the command hangs until the bound.
- **Impact:** the wrong processes are killed, or the parked design's prompt contract silently breaks.
- **Required correction:** typed registry entries:
  - group leader → `kill(-pgid)`;
  - single process → `kill(pid)` under the same zombie-pinning order as P2-2, with `pidfd` where Linux has it.
  
  `git rev-list` moves to its own group (it needs no terminal). `ssh` is registered as a single process, and the amendment states that `ssh`'s own children (`ProxyCommand`) end through their pipes.
- **Closure test:** with gwz SIGKILLed during an `ssh://` call inside a `| tee` pipeline, `tee` survives and `ssh` is gone. A prompting-`ssh` fixture under a pty still reaches its prompt. A `rev-list` cancel kills only `rev-list`.

### P2-4: The reaper, the Force path and the job SIGKILL the local `stdio` core in the middle of a write, bypassing its orderly PeerGone path

- **Location:** §3.6 "The `stdio` and `ssh://` children" ("the lifeline reaper (or the job) ends the child ... with the same result at the core"); §3.3 step 2 ("Synchronously end every live child tree").
- **Violated invariant:** C4 inside the core. The claim "same result at the core" is also false.
- **Root cause:** for `--server stdio`, the registered child is the core itself (parked :1107, `setsid`). SIGKILL is not PeerGone. The orderly path (pipe EOF → channel closure → cancel to safe points, `workers.rs:213-235`) never runs. On Windows a `KILL_ON_JOB_CLOSE` job has the same effect.
- **Interleaving:** a `gwz --server stdio` merge or local phase is between safe points, for example in the middle of `write_manifest_and_lock` or a libgit2 ref update. The user presses Ctrl-C twice, or the client is SIGKILLed. Force, the reaper or the job SIGKILLs the stdio core. A ref `.lock`, a `pack_git2_*` temp or a torn working-tree file is left. Had the pipe simply closed, the core would have stopped at its next safe point.
- **Impact:** the served form leaves the C4 residue that the in-process form would avoid.
- **Required correction:** the stdio core's primary end is the closing of its pipe (C3).
  - The client's registry, Force and reaper first close its stdin and send SIGTERM, which the core's own shutdown handles (2.3i).
  - They SIGKILL only after the session's `close_wait`.
  - On Windows, the stdio child is not placed in a kill-on-close job, or such a job is used only as a backstop after a grace period.
- **Closure test:** the client is SIGKILLed while the stdio core sits at a gated point in the middle of a write. Afterwards no `*.lock`, no `pack_git2_*` and no torn file are left, and the core's log shows a channel-closure cancel.

### P2-5: gwz-py has no reaper path and no C1 path (architectural)

- **Location:** §3.1 "gwz-py"; §3.4 ("gwz re-executed in a hidden `__reaper` mode, so nothing new ships"); §4 table (gwz-py gets only "second interrupt as Force", step 3.2).
- **Violated invariants:** C2 and C6 parity.
- **Root cause:**
  - gwz-py runs gwz-core inside a Python process (`gwz._gwz_core`). Its only entry point is the `gwz-py = "gwz.cli:main"` script, so the current executable is the Python interpreter, and re-executing it in `__reaper` mode cannot work.
  - gwz-py starts helpers through `with_cancellable_local_transport_native_noted` (`route/transport.rs:20-21, 194`).
  - An embedding application that forks without exec (for example Python's `multiprocessing` "fork" start method) carries the lifeline write end into its workers, so the reaper never sees EOF.
  - gwz-py installs no handler (OD-C1), so its CLI dies on SIGTERM or SIGHUP with no cancel.
- **Interleavings:**
  1. A long-lived gwz-py application is killed by the OOM killer during a lookup: the helper is orphaned.
  2. The terminal running `gwz-py fetch` closes: SIGHUP kills Python before any cancel, and the helpers are orphaned.
  3. An application that forks worker processes is SIGKILLed: the reaper (if one existed) waits for the workers, and the helpers live on.
- **Impact:** C2 fails for an entire host, and §5 has no gwz-py row.
- **Required correction:**
  - Name a reaper route gwz-py can use: a small reaper binary shipped in the wheel, or a reaper loop entered directly after `fork` that uses only async-signal-safe calls.
  - Assign it in §4.
  - State the embedding constraint (fork without exec keeps the lifeline open) and the gwz-py CLI's handling of SIGTERM and SIGHUP.
  - Add rows.
- **Closure test:** gwz-py (library and CLI) is SIGKILLed during a sleeping-helper lookup, and the helper's group is gone within 1 s on macOS and Linux. Row: the gwz-py CLI gets SIGHUP during the lookup.

### P2-6: The interrupt handler never gives up control, and the Force path can block or panic, so gwz can become impossible to Ctrl-C

- **Location:** §3.1 (self-pipe, "a later one `Force`", "installed once by `main`"); §3.3 step 2 ("print one line ... and exit").
- **Violated invariants:** C5 ("forces the exit"); never worse than today.
- **Root cause:** the text never restores the default signal disposition (not after Force starts, not on a third interrupt, not when the per-command source is dropped). The Force path writes to stdout or stderr, which may be broken or blocked. It specifies neither a non-blocking self-pipe nor SA_RESTART.
- **Interleaving:**
  1. The user runs `gwz fetch 2>&1 | tee fetch.log` and a helper never exits. Ctrl-C reaches the whole pipeline group, so `tee` dies at once.
  2. Five seconds later the bound fires Force on the interrupt thread. Its one-line print hits EPIPE and `eprintln!` panics, which kills only that thread.
  3. From then on every SIGINT writes a byte nobody reads. gwz never exits and ignores Ctrl-C.
  
  A Force blocked on a full stdout pipe whose reader has stopped behaves the same way. The same swallowing applies after the source is dropped while `main` is still flushing. Today, Ctrl-C ends gwz at once.
- **Impact:** a process that only SIGKILL can end. That is worse than today, and P2-1 also leaves the helpers orphaned at that point.
- **Required correction:**
  - Force writes with a single non-blocking `write(2)` whose errors are ignored, never a panicking macro, and then calls `_exit` or `ExitProcess` without unwinding.
  - The handler restores the default disposition once Force begins and once the source is dropped, so the next interrupt applies the OS default.
  - A third interrupt re-raises with the default disposition.
  - The self-pipe is `O_NONBLOCK`, and the handler saves and restores `errno`.
  - The SA_RESTART choice is stated.
- **Closure test:** rows (a) the `| tee` interleaving above exits within bound + 1 s, (b) a Force blocked on a full stdout pipe dies on the next SIGINT, (c) SIGINT after the source is dropped ends the process.

### P2-7: C4 and C5 conflict: the forced exit lands inside write sequences that safe points cannot bound (architectural)

- **Location:** §3.2 "Local phases" ("A phase between two safe points runs to its next one; the cleanup bound (C5) covers the wait"); C4; C5 ("the forced path ... still leaves only state C4 allows").
- **Violated invariant:** C4.
- **Root cause:** the stretch between two safe points has no upper bound. Examples: a libgit2 checkout writes files in place; `local clone` copies a large file; indexing a large pack. After the 5 s bound, or on a second Ctrl-C, Force exits wherever the phase is. Nothing in §3 makes those states "complete, never published, or recognized" by the next command.
- **Interleaving:** `gwz switch` or a merge checkout of a large tree. Ctrl-C, then 5 s pass. Force exits halfway through the checkout. The working tree has torn and half-updated files and maybe `index.lock`. The next `gwz status` shows modifications the user never made, and the user may commit them.
- **Impact:** a "partial state" that D-V forbids, with no recovery, while the text asserts C4 for the forced path. Row 3 checks only children.
- **Required correction:** decide and state one of:
  - (a) Force kills children and network work immediately, but a local write sequence already in progress runs to its next safe point before the exit. The bound then covers only waiting.
  - (b) C4 lists each forced-exit residue per handler (checkout, copy, index) with the next command's recognizer and its recovery.
- **Closure test:** a row forces an exit during a gated large checkout and asserts either a consistent tree at exit, or the next command's named recognition and recovery.

### P2-8: C4's "never published" clause silently narrows D-V, so libgit2's temp packs stay forever

- **Location:** C4 ("either complete, never published, or recognized and resolved"); §3.5 (no mention of libgit2 temps); row 4 (graceful path only).
- **Violated invariant:** D-V's "no ... partial state behind".
- **Root cause:**
  - libgit2's indexer stages `objects/pack/pack_git2_<16 hex>` (`futils.c:36`, `indexer.c:168, 202`).
  - It removes that file only on its own error path or when the indexer is freed (`indexer.c:221-228`, `pack.c:1071-1072`). Neither runs on Force, SIGKILL or a crash.
  - git prunes only `tmp_*` in the object directory, so neither gwz nor `git gc` ever removes `pack_git2_*`.
  - C4 calls such a file "never published" and therefore allowed.
- **Interleaving:** a native or transport fetch stalls on the network. The progress callback (§3.2) does not fire, so the abort is not observed. The user presses Ctrl-C once. After 5 s the bound forces the exit. A partial pack, possibly hundreds of MB, stays in the member's object store permanently.
- **Impact:** permanent garbage on the forced path a single Ctrl-C reaches, and the operator's "no turds" requirement narrowed without a decision.
- **Required correction:** remove "never published" as an allowed end state unless something named removes or reports the file. In §3.5, cover libgit2's `objects/pack/pack_git2_*` (and `tmp_object_git2_*`): age-gated removal under the member's cross-process exclusion, or a report with its path, as for `*.lock`.
- **Closure test:** a row forces an exit during a stalled fetch, then runs the next command, and asserts that no `pack_git2_*` remains or that it is reported by path.

### P2-9: The temp sweep is a new deletion path whose pattern and directories reach files gwz does not own

- **Location:** §3.5 "gwz's temps"; §2 "Leftover state"; row 6.
- **Violated invariants:** delete only gwz's own dead temps; C4.
- **Root cause:**
  - The trigger, "the next durable write into the same directory", applies to every `write_atomic` target. Those include:
    - the workspace root's `AGENTS.md`/`CLAUDE.md` (`workspace_bootstrap.rs:299-305`);
    - `.claude/` (`claude_settings.rs:124-125`);
    - `.git/info/exclude` (`sync_workspace_boundary.rs:65, 121`). That writer is "capability-free", and the text gives no evidence that it holds the mutator lock.
  - The workspace mutator lock excludes only gwz writers in that workspace. It does not exclude editors, agents or other tools writing in those directories.
  - `<name>.*.tmp` does not anchor gwz's ID shape, which is `{prefix:016x}-{seq:x}` (`ids/src/lib.rs`).
  - The text says nothing about symlinks or file types.
  - It misses the family store's `.{name}.tmp.{id}` (`family-store/src/publish.rs:150-168`, written under a different lock).
  - It runs only when a later write lands in the same directory.
- **Interleaving:** an editor or agent atomically saves `AGENTS.md` as `AGENTS.md.<rand>.tmp` while `gwz` writes `AGENTS.md`. gwz deletes the other program's live temp, and that program's rename fails or loses the save. A user's `gwz.yml.old.tmp` is deleted silently. A family-store temp left by a crash is never swept. A directory that is rarely written keeps its temps, despite row 6's "removed by the next command".
- **Impact:** user data loss through a new deletion path, plus missed leftovers.
- **Required correction:**
  - Sweep only gwz-owned directories (`gwz.conf/`, `.gwz/`), from an inventory of every gwz temp shape and the lock each writer holds.
  - Match the exact regex for each shape.
  - Remove only regular files, checked with `lstat` and never through a symlink.
  - Run the sweep when the lock is acquired, not on whichever write happens next.
  - Exclude the checked-artifact directories, which have their own recovery (`verified_write.rs` and the boundary checks), unless their owner agrees.
- **Closure test:**
  - A user file `AGENTS.md.backup.tmp`, a symlink named like a gwz temp, and a temp with a non-gwz ID shape all survive.
  - Dead gwz temps in each inventoried directory and a dead family temp are removed by the next mutator, even when it writes elsewhere.

### P2-10: The `GIT_ELOCKED` report uses the mutator lock as a staleness test, but network ref writers do not hold it

- **Location:** §3.5 "Git's `*.lock` files" ("says no gwz mutation holds the workspace (when gwz can take the mutator lock), and gives the removal command"); row 7.
- **Violated invariant:** never invite the removal of a live lock.
- **Root cause:** `fetch`, `push`, `pull_head` and `tag` network ref updates take only the in-memory member lock of their host context (`memberlockmanager.rs` header table). The workspace mutator lock is taken by `WorkspaceMutationGuard` users (commit, stage, tag, materialize, merge), not by `fetch`. A concurrent `gwz fetch` in another process, or a served core, holds `refs/remotes/…/x.lock` or `packed-refs.lock` while the mutator lock is free. Plain `git` and IDE auto-fetch do too.
- **Interleaving:** process A runs `gwz fetch` and holds `refs/remotes/origin/main.lock`. Process B runs `gwz commit`, meets `GIT_ELOCKED`, takes the mutator lock, and prints "no gwz mutation holds the workspace … `rm …/main.lock`". The user runs it. A's rename fails, or a later writer races it and a ref update is lost.
- **Impact:** a destructive instruction backed by a false claim.
- **Required correction:** the message must not infer staleness from the mutator lock. It should state the lock's path and age and say to confirm that no `git` or `gwz` process (including a server) is running. The other option is to make network ref updates take a cross-process member exclusion first, and state that.
- **Closure test:** a two-process fixture (a fetch held at a ref lock, and a commit that meets `GIT_ELOCKED`). The message makes no "no gwz mutation" claim and names the conditions to check before removal.

### P2-11: The "possible remote effect" report for a push has no carrier on the native and forced paths

- **Location:** §3.5 "Partial Git transfers" ("a cancelled push reports `Effect::Possible` once its receive-pack exchange has started, as the contract already says"); §3.3 step 2 ("print one line naming any operation that did not reach a terminal"); OD-C4 ("JSON response marking the command `cancelled`").
- **Violated invariant:** a cancelled push whose remote effect is unknown is never reported as having no effect.
- **Root cause:**
  - `Effect` exists only on the transport endpoint (`https_worker/serve.rs:151`, `placement_endpoint/attachments.rs`).
  - The native `git2` push path has no Effect at all, yet §3.2 makes native transfers cancellable through the callbacks.
  - The Force path and the served client's second interrupt end operations that never reach a terminal, so no Effect is ever computed.
- **Interleaving:**
  1. A native `file://` or `http://` push: the pack is uploaded, the server applies the update, and the cancel is observed in the sideband callback. gwz reports the member "cancelled" with no effect, but the remote ref moved and the local tracking ref is stale.
  2. A forced exit during a transport push after receive-pack started: JSON says `cancelled`. An automation retries, or assumes nothing changed.
- **Impact:** a wrong report of remote state, which the safety axis explicitly lists.
- **Required correction:** track each push's phase. For every cancelled or non-terminal push past the start of the upload, on the native, transport, forced and served paths, report `possible remote effect` in text and JSON (`cancelled` plus `effect: possible`).
- **Closure test:** rows for (a) a native push cancelled after its upload, with a delaying server hook, and (b) a forced exit during a transport push. Both report a possible effect.

### P3-1: On macOS the lifeline's write end can leak into a child spawned at the same time

- **Location:** §3.4 ("close-on-exec everywhere else"; the reaper starts with "the first child").
- **Root cause:** on Apple platforms, Rust std sets `FD_CLOEXEC` in a separate step after `pipe()`. The spawn gate does nothing on Unix ("Unix has no inheritance window to close", `process_tree.rs:55-56`). The reaper starts lazily, while parallel member operations are already spawning.
- **Consequence:** a concurrently spawned child, possibly a retired daemon, inherits the write end. After gwz dies the reaper sees no EOF, so it and the registered groups outlive gwz. This is plausible (inferred from std's implementation), not observed.
- **Correction:** create the lifeline under the spawn gate's exclusive hold, or before any thread starts.
- **Closure test:** under concurrent spawns, no child's descriptor table contains the lifeline (checked with `lsof`).

### P3-2: Keeping the helper's stdin open gives no signal

- **Location:** §3.4 "The helper side of the pipe".
- **Root cause:** the direct child is `git credential fill`. It stops reading at the blank line of `url=…\n\n` (`lookup.rs:126`) and gives its helpers pipes of its own, so no process ever reads gwz's still-open stdin.
- **Consequence:** a tested behaviour (`pipes.rs:4-16`) changes, and a claimed second signal does not exist.
- **Correction:** drop the change, or rescope it to children that do read stdin until EOF.
- **Closure test:** none needed if the change is dropped. Otherwise, a row showing a real child seeing EOF.

### P3-3: On Windows only the helper has a job, and the CTRL_CLOSE handler must block

- **Location:** §3.1 Windows; §3.4 Windows bullet; §3.6.
- **Root cause:**
  - Only the credential helper is in a `KILL_ON_JOB_CLOSE` job. The text does not commit `git rev-list` or the `stdio`/`ssh` child to jobs (§3.6 says only "(or the job)").
  - A CTRL_CLOSE handler that signals and returns lets Windows terminate the process before the forced path runs.
- **Consequence:** a gwz that is `TerminateProcess`ed, or whose console is closed, leaves children without jobs.
- **Correction:** every registered child gets a job on Windows (subject to P2-4 for the stdio child). The CTRL_CLOSE handler waits for the forced path to finish, up to the OS limit.
- **Closure test:** `TerminateProcess` and console-close rows with `rev-list` and an `ssh` child running.

### P3-4: OD-C6's lifecycle against C2 and C5 is incomplete

- **Location:** C2; C5; §3.3 step 2; §3.4; OD-C6.
- **Root cause:** Force "ends every child tree", but pager and `forall` children are never killed. The text does not say whether the 5 s bound forces an exit while a `forall` child that handles or ignores SIGINT is still running.
- **Consequence:** gwz exits 130 and leaves the child running on the terminal. That is an orphan C2 forbids, and nothing records it as an exception.
- **Correction:** state either that the bound does not apply while a foreground pager or `forall` child runs (gwz waits, as git waits for its pager), or record the C2 exception.
- **Closure test:** a row with a `forall` child that traps SIGINT.

### P3-5: OD-C1's single owner collides with 2.3i's server signal handler

- **Location:** §3.1 "The unavoidable global"; OD-C1 ("gwz-core and gwz-py install none"); Step 2.3 plan 2.3i :186-187 ("SIGINT/SIGTERM and Windows Ctrl-Break through a handler that sets a flag only", in gwz-core `server/`).
- **Root cause:** the same gwz binary would install two handlers for one process-global, and the last installation silently wins.
- **Consequence:** on `gwz server --foreground`, either the CLI's Cancel or the server's five-step shutdown is lost.
- **Correction:** the server's shutdown consumes the interrupt source gwz-cli passes it, and the allow-list has one entry.
- **Closure test:** Ctrl-C on `gwz server --foreground` runs the shutdown sequence, and a second Ctrl-C ends the detached wait.

## 2. Verified citations

| Citation | Result |
|---|---|
| TR1.6 `:75-76` (cost of §3.2), `:398` (OQ2) | Correct. |
| Split doc `:232, :235, :239-240` | Correct at the stated root a519ee58 (`:235` is "Client interrupts"). Drifted by 7 lines at 9d5a4d4c (now `:239`, `:242`). The split doc changed in 7fa70514 and 83bd4c8f. `:235` does not say replies "carry the core's cleanup report" as §3.6 claims. |
| Parked server design `:809, :1107, :1218-1224` | `:809` is the "Background start" row (the stdio-child row is `:810`). `:1107` and `:1218-1224` are correct. Unchanged since a519ee58. |
| Session design `:687` | Correct (detached worker can be torn by exit). |
| Contract §5 `:253-284` | Correct (five-second cleanup bound, `:271-272`). |
| `check_process_globals.py:103-104` | Correct (`signal`, `sigaction` in `LIBC_FUNCS`). |
| `gwz-cli/src/globalargs/dispatch.rs:67-80` | Correct (non-cancellable entries). |
| `served/dispatch.rs:100-107` | Correct. |
| `handle_local.rs:72` | Correct (`NeverCancelled`). |
| `transport_support.rs:90-93` | Correct (callback returns `true`). |
| `process_tree.rs:57-95` (Unix group, kill) | Correct. Also shows `retire` and `drained` (P2-2). |
| `owner.rs:253-303` | Correct (kill before reap; drop path). |
| `process_tree/windows.rs:65-133` | Job with `KILL_ON_JOB_CLOSE` confirmed at `:65-80`. |
| `pipes.rs:9-16` | Correct (writes, then `shutdown`). |
| `commit_log/mod.rs:330-358` | Correct (no group). |
| `workers.rs:213-235` | Correct (cancel running, settle unstarted). |
| `artifact/encoding.rs:47-56` | Correct for that writer. §2's "durable writes stage `<name>.<id>.tmp`" is incomplete: `verified_write.rs:64`, `rewrite.rs:229` and `family-store/publish.rs:168` use other shapes (P2-9). |
| `workspace_mutator_lock.rs:42-50`, `family-store/src/lock.rs` | Advisory locks confirmed. |
| `libgit2/src/util/filebuf.c:44-52` | Correct (`GIT_ELOCKED`, no staleness check). |
| gwz-py `bridge.py:29-35`, `route/transport.rs:20-21, 44, 167-173, 194` | Correct. The `Native` route has no `Canceller` (`:166-169`), which bounds §3.1's "already cancels". |
| Not re-verified on this axis | `token.rs:72-132`, `gate.rs:140-158`, `cancellable.rs:40-68`, `requests.rs:37-75`, `memberlockmanager.rs:99-120` (header table read instead), carrier `mod.rs:312-327`, `inbound.rs`, `writer.rs`, `carrier/local/windows.rs:560-572`, gwz-transport `remote/core.rs`, `pager.rs`, `forall.rs`. |

## 3. Risks and next action

- **Highest risk:** the reaper (P2-1 to P2-5). It is a new process type that can kill the wrong processes, die alongside gwz, or not exist at all (gwz-py). Its contract must be complete before step 3.0c is briefed. Its dual review should replay the interleavings above.
- **Second:** the forced path (P2-6 to P2-8). As written, a single Ctrl-C on a stalled operation reaches Force within 5 s, and Force can hang, tear files or leave packs.
- **Next action:** revision 2 of the amendment resolves P2-1 to P2-11 as specified, with the closure rows added to §5. The operator chooses the route for P2-5 (how gwz-py gets a reaper) and for P2-7 (option (a) or (b)). Re-review on this axis against the new blob.

## 4. Out-of-scope observations

- **Possible defect, existing code (not introduced by the amendment):** `ProcessTree::kill` on cancel and drop signals `-pgid` after the leader may already have been reaped elsewhere (`owner.rs:253-303` orders kill before `wait`, which is safe in itself). Any other caller that reaps first would race PID reuse. Worth a source-check rule once P2-2's ordering is written down.
- **Limitation, existing:** when gwz runs as PID 1 in a container, it does not reap orphaned grandchildren (retired helper descendants), so zombies accumulate. This does not block D-V.
- **Consistency:** the parked design's 10 s `ssh` wait after `session.close` (`:1223`) does not match C5's 5 s bound. That belongs to the other axis.
