# Cancellation design B r2: Safety-axis review (round 2)

**Review object:** `/Volumes/projects/limbo/gwz-dev/dev-docs/GwzChildLifetimeDesign-DRAFT.md`, revision 2, 398 lines. The hash was `9724eae66f13de48959d7bc3dcdad1d4dcd6d423` at the start and the same at the end, so the object did not change. | **Date:** 2026-11-01 | **Axis:** Safety. This covers what the text allows to go wrong: orphaned processes, kills that reach the wrong process, stale locks, partial state, misreported effects, signal and fork races, Windows job and console behaviour, the forced path, and whether each residual is bounded and can be recognised.

**Verdict: NO-GO.** Counts: 0 P0, 0 P1, 2 P2 (new), 6 P3 (new). All 25 round-1 findings are closed: Safety P2-1 to P2-5 and P3-1 to P3-10, Consistency P2-1, P2-2 and P3-1 to P3-8. None of the new root causes is architectural, so design B's architectural count stays at 0. Each new P2 can be closed by a cost or residual statement plus one operator choice. Neither needs a new mechanism. **Pre-commit:** I will give GO on a revision 3 that closes N-P2-1 and N-P2-2 as specified below, adds no new P0-P2, and either resolves each P3 or defers it explicitly with an owner.

## 0. Evidence base

- **Pin.** `git hash-object` gave `9724eae6…` at the start and at the end.
- **Member checkouts.** Every member checkout matches the design's pins and has a clean tree: gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`, gwz-sspi `6f83551`, git2-rs `d13951f`. The root is at `48feeaad`.
- **Controlling decision.** Plan §10 D-V, :999-1062, all bullets, including decisions (i)-(iii) at :1055-1062.
- **Prior round.** I read both round-1 reviews and the RemPlan in full.
- **Design A.** Read only for the hand-offs, from the committed r1 (`git show HEAD:dev-docs/GwzCancelInterruptDesign-DRAFT.md`). The working copy is modified and I did not read it. Relevant A r1 facts:
  - A's Unix handler covers SIGINT, SIGTERM and SIGHUP. It uses `sigaction` with a handler that writes one byte to a pipe whose write end is non-blocking (A §3.1, :135-137).
  - Force runs `kill_killable()` at step 4 and `close()` at step 6, then `_exit` (A :207-210).
  - The gwz-py CLI keeps Python's default for SIGTERM and SIGHUP (A §3.8 :267, R15).
- **Code read.**
  - gwz-core: `commit_log/mod.rs:326-362`, `spawn_gate.rs:420-460`, `process_tree.rs:36-92`, `owner.rs:75-99, :250-305`, `process_tree/windows.rs:44-62, :113-160`, `windows/launch.rs:1-4`, `composition.rs:20-50`, `driver_only.rs:59-63, :150-154`, `https_worker.rs:300-305`, `runner_tests.rs:297-301`, `handle_commit.rs:175-190` (the member loop is sequential), `Cargo.toml:128-153`, and the `TH32CS_SNAPTHREAD` users.
  - Tag `v1.0.18`: `repository.rs:295-306` and `refs.rs:170-184`.
  - gwz-git: `native_commit.rs:75-100` and `native_commit/cli.rs:17-50`. `run` maps a non-success status to an error, and `verify::head_after` checks that HEAD advanced.
  - gwz-cli: `lib.rs:155-215` and every `process::exit` site.
- **Toolchain sources.**
  - Rust std 1.95.0, `sys/process/unix/unix.rs`: the fork path at :60-160 (plain `libc::fork`, no signal blocking, handlers inherited until exec, and `Ok(0)` taken as success when the child dies before exec); `do_exec` at :280-410; `posix_spawn` eligibility at :455-470 (any `pre_exec` closure means no posix_spawn, while a process group alone is allowed via `POSIX_SPAWN_SETPGROUP`, :722-724).
  - Rust std `os/unix/process.rs:220-221`: `setsid` is unstable.
  - tokio 1.53.1 `net/unix/pipe.rs:419, :967`.
  - Go 1.26.4 `os/wait_waitid.go:5-8`.
- **Platform facts relied on, not measured.**
  - glibc's `posix_spawn` child resets caught handlers to `SIG_DFL` while every signal is blocked. macOS `posix_spawn` is a single syscall.
  - Linux keeps `SIG_IGN` for SIGCHLD across `execve`.
  - git's lockfile code removes `*.lock` files on SIGINT, SIGHUP, SIGTERM, SIGQUIT and SIGPIPE.
  - Under `vm.overcommit_memory=2`, `fork` charges the child for the parent's whole private writable memory, but a `CLONE_VM|CLONE_VFORK` spawn is not charged.
  - Older container seccomp profiles (Docker before 20.10) reject `close_range` with `EPERM`.
- Nothing was built or run, and nothing was modified.

**Provenance and scope.** Every mechanism still traces to D-V (2) and (3), to decisions (i)-(iii), or to round-1 necessity. Two silent narrowings remain:
- N-P2-1: the reaper's choice of SIGKILL for `Wait` groups at gwz's death has no decision behind it, and its cost against 1.0.18 is misstated.
- N-P2-2: the child-side registration forces a full `fork` on every registered spawn, and that cost is not priced anywhere.

The operator's choices themselves are not attacked here (OD-B1 (a), OD-B2 (a), OD-B3 (a), and Force never exiting mid-write).

## 1. Prior-finding closure table

| ID | Status | What I checked |
|---|---|---|
| C-P2-1 | closed | OD-B1 (:336-343) states the Windows cost: a hidden console prompt, a wait through Cancel and Force, and 1.0.18's shared console cited at `v1.0.18` `repository.rs:299-305` and `refs.rs:172-183` (verified). R-B10 (:365) has a W recognizer. B15's W leg records without asserting. The kept-console variant is recorded as not chosen. |
| C-P2-2 | closed | B1 (:29-31) lists its exceptions. B5 (:37-40) is scoped to registered `Wait` entries, with R-B13 as the exception. R-B5 (:360) names Force and the Windows console. R-B13 (:368) names the interrupt mid-write. OD-B2 (:344-346) states the cost. The registry-less set (the non-candidate build, the old entries until 3.3, `DriverOnly` until C) is named the same way in §2, §3.1, §3.7, §4 "Interim", OD-B2 and R-B13. |
| C-P3-1 | closed | §8 row 7 (:384) now reads "of every child a gate with a registry starts" and points to §3.6. |
| C-P3-2 | closed | §3.6 "The helper on Windows" (:212-213). |
| C-P3-3 | closed | The 2.3k row is gone. §3.7 "To C" (:237-238) hands 2.3k's registry access to C. |
| C-P3-4 | closed | §3.1 (:93-96) names the `Spec` and the three changed callers. 2.3r-2's Owns lists them. A slip in wording, not a safety issue: "`output` … return the crate's `Child`" should say `Output`. |
| C-P3-5 | closed | Capacity is a live-entry count on every platform (:116-117). B10's token clause is M and L only. B21 uses `IsProcessInJob` against each registry job. |
| C-P3-6 | closed | B7 (:42-44) traces the change. The f9 report exists at `/Volumes/projects/limbo/build-targets/winflk-logs/f9-report.md`. |
| C-P3-7 | closed | §2 "The gate" lists the hostless backends and shows that none reaches a spawn site. The 1.0.18 lines are cited at the tag. |
| C-P3-8 | closed | B5 cites decisions (i) and (ii). |
| S-P2-1 | closed | §3.3 (:140-145): Linux uses `waitid(WNOWAIT)`; macOS uses kqueue `NOTE_EXIT` with an `ESRCH` path; a stop is never an exit; nothing spins. The watch thread for the reaper uses the same rule (:166). B25 covers SIGSTOP and SIGTTIN. One residual gap in the `ESRCH` path is filed as N-P3-3. |
| S-P2-2 | closed (option (a)) | B4 (:35-36), §3.3 (:154-159), R-B1 (:356) and §8 row 6 now say that gwz's own kills are exposed and that process-wide reaping is unsupported. The RemPlan's rejection of option (b), a fresh check before each kill being itself check-then-kill, holds. A deterministic subset remains as N-P3-2. |
| S-P2-3 | closed | `close()` and the last drop wait without a bound for every `Wait` entry (:179-182). gwz-py's close runs it (:242-243). R-B4 lists `os._exit` and a foreign `exit()`. R-B17 is listed. B26 is added. The gwz-cli exit-site argument holds: the exits at `lib.rs:162, :175-177, :187, :207, :350` come before any command, the others follow the command's return, and the commit loop `handle_commit.rs:179-187` is sequential. |
| S-P2-4 | closed | OD-B1 states hung hooks and signers, the unbounded wait through Cancel and Force, gwz's death as the only way out, `index.lock` against 1.0.18's SIGINT cleanup, and Windows. Option (c) is recorded as not chosen under decision (ii). The comparison with 1.0.18 at gwz's death is still missing; that is N-P2-1, which is new. |
| S-P2-5 | closed for the reaper's fork | §3.4 "Cost" (:183-186), R-B16, the 3.4 gwz-py measurement, the README row, and the exact `ReaperGone` message. The same cost applied to each spawn's fork is unpriced: N-P2-2. |
| S-P3-1 | closed | `close()` takes the gate's hold (:132-133). R-B14 has a recognizer. B12 has a seam. |
| S-P3-2 | closed | Descriptors are enumerated rather than closed up to the limit (:169-171). B4 runs at the hard maximum. Handling of a failed sweep is N-P3-4. |
| S-P3-3 | closed | 64-bit tokens that are never reused, `EINTR` retried, a full table leads to `_exit(1)`, and the core owns the write end (:151-153, :172-176). What the waiter does after the writer dies is N-P3-5. |
| S-P3-4 | closed | The fork check runs first, before any lock, in every method and `Drop` (:113-115). B9 is extended. |
| S-P3-5 | closed | R-B2 (:357) names `pkill -9 gwz`, `killall -9 python` and the OOM killer. |
| S-P3-6 | closed | The gate takes a `Spec` and returns the crate's `Child`, which has no std `wait` (:93-96). The dispute about the source check is accepted: a std wait on the crate's type does not compile. |
| S-P3-7 | closed | `kill(-pgid)` for `Group` and `Session`, at the reaper (:175) and at release (:148). B24 tests it. |
| S-P3-8 | closed | B7 traces the change. `pending_cleanup_count()` is at `owner.rs:77`, and `https_worker.rs:303` counts it (verified). The 30 s comes from `runner_tests.rs:299` (verified). The path is fixed. |
| S-P3-9 | closed | "Creation precedes credentials" is stated as an invariant (:101-102). `spawn_gate()` carries no registry. The minting check allows `create` only in `composition.rs`. The 2.3r-2 brief names the invariant. Plan 3.2's composition is module-level (:729), so creation happens once, at import. |
| S-P3-10 | closed (recorded) | R-B15 is handed to C (:194-195, :236). |

## 2. New findings

### N-P2-1: At gwz's death the reaper SIGKILLs a running `git commit`. On 1.0.18 that commit finished. R-B4 and OD-B1 leave out the most common trigger and a less destructive kill.

- **Location:** §3.4 "Loop" (:174-175, "sends SIGKILL to every entry still registered") and "Its end" (:181-182). §3.6, `git commit` row ("reaper; its job (R-B4)"). §6 OD-B1 (:338-342). §7 R-B4 (:359).
- **Violated invariant:**
  - D-V OQ2 ("no … stale lock or partial state").
  - Decision (ii), that commit and tag are allowed to finish.
  - The rule that an operator decision states its true cost against the parity reference, 1.0.18 (D-V :1003-1004).
- **Root cause.**
  - On 1.0.18 the CLI-route `git commit` ran in gwz's group (`v1.0.18:repository.rs:299-305`). Whenever gwz died by a process-directed signal (SIGKILL, a crash, the OOM killer, SIGTERM or SIGHUP to a Python host), git kept running and finished the commit. A group-directed signal reached git, and git's lockfile code removed `index.lock`. Neither path left a stale lock.
  - B moves git into its own group and arms the reaper to SIGKILL that group at end of file. Every death of the host now tears a commit that is in progress, not only a hung one.
  - Decision (i) accepted that "only SIGKILL ends a hung write". It does not decide that any death of the host must tear a write that would otherwise finish.
  - OD-B1's cost compares only with "1.0.18's Ctrl-C". R-B4's trigger list ("SIGKILL, a crash, an interrupt after Force restored the default", `os._exit`, a foreign `exit()`) leaves out:
    - the default-action death of a gwz-py host by SIGTERM or SIGHUP. This is how systemd, Kubernetes and `kill <pid>` stop a service. A §3.8/R15 covers only the gwz-py CLI, and embedding applications are not covered at all.
    - Windows CTRL_CLOSE, logoff and shutdown (A R9).
    - an `exec` of the host. The lifeline write end is close-on-exec, so the reaper kills every entry, `Wait` included, while the same PID lives on.
  - The reaper's choice of signal for `Wait` groups was never put to the operator.
- **Reproduction (L or M, gwz-py after 3.2).**
  1. A Python service runs `client.commit(...)`. The `pre-commit` hook takes 3 s, and git holds `index.lock`.
  2. `systemctl stop` (with KillMode=mixed) or `kill <pid>` sends SIGTERM to the Python process. Python dies by the default action, without running `atexit`.
  3. The lifeline's end of file reaches the reaper, which sends SIGKILL to git's group.
  4. `index.lock` remains, and the hook has run only partly.
  - Under 1.0.18 the same sequence finishes the commit. The same happens for gwz-cli under `kill -9`, CI hard timeouts or OOM: 1.0.18 finished the commit, B tears it.
- **Impact:** a stale lock and partial hook effects on the ordinary stop path of every gwz-py service. This is a parity regression of exactly the kind D-V refuses, and the operator decided OD-B1 against an incomplete comparison.
- **Correction.**
  - **Required:** restate OD-B1's cost and R-B4's "When" to cover:
    - every death of the host while a commit or tag is running, named (default-action SIGTERM and SIGHUP of a gwz-py host, Windows console close, logoff and shutdown, `exec`);
    - the 1.0.18 comparison (git finished or cleaned up).
  - **Operator choice for `Wait` entries at the reaper's end of file:**
    - (a) SIGKILL, as written;
    - (b) SIGTERM to the `Wait` group, so git removes its own `*.lock` files as on 1.0.18, followed by SIGKILL after a bound (a `nanosleep` keeps the reaper async-signal-safe), or with no follow-up and the surviving hook named as a residual;
    - (c) leave `Wait` groups to finish. This conflicts with B1 and needs its own residual.
  - Windows stays at (a), because there is no console event for a `CREATE_NO_WINDOW` git. That must be stated.
- **Closure test:** a new row (M, L). A gwz-py script whose commit has a sleeping hook receives SIGTERM.
  - Under (b) or (c), no `index.lock` remains, and under (b) git and the hook are gone within the bound.
  - Under (a), the row records `index.lock` as the accepted R-B4 residue.
  - Repeat with gwz-cli under SIGKILL. W leg: record the result, do not assert it.
- **Architectural:** no. It is a policy choice inside the reaper's loop plus a cost statement.

### N-P2-2: The child-side registration forces a full `fork` of the host for every registered spawn, and that cost is not priced

- **Location:** §2 (:74, "`pre_exec` … forces the fork path, `:463`"). §3.2 Unix (:121-125). §3.4 "Cost" (:183-186). R-B16 (:371). §4 3.4 (:255). §8 row 6.
- **Violated invariant:** D-V (2) costs and OD-B3 must be priced. Compatibility with 1.0.18 for gwz-py.
- **Root cause.**
  - std uses `posix_spawn` only when the command has no `pre_exec` closure (`unix.rs` :455-470); a process group alone is still allowed.
  - Today `rev-list` (`commit_log/mod.rs:330-351`: `current_dir`, two env values, no `PATH`) and 1.0.18's `git commit` and `git tag` take that path. On glibc this is `CLONE_VM|CLONE_VFORK`, so no commit charge; on macOS it is a single syscall.
  - B's registration closure moves every registered spawn onto `libc::fork()` of the whole host.
  - §3.4 and R-B16 price only the reaper's one fork.
  - In gwz-py the host is the Python interpreter:
    - Each `rev-list` step of `gwz log -- <path>` (one spawn per `--max-count=1` step, thousands per walk) and each member's commit copies page tables proportional to the heap.
    - Under `vm.overcommit_memory=2`, each fork must charge the full private writable memory again, on top of the reaper's retained copy. So spawns fail intermittently with std's plain `fork: ENOMEM`, not with R-B16's named message, even after the reaper's fork succeeded.
- **Reproduction (L, gwz-py, 2 GiB heap, `overcommit_memory=2`, a commit limit just above 2× the heap).** The reaper forks successfully. `client.log(path=…)` then fails partway through with `could not run local path history cursor: … Cannot allocate memory`, where 1.0.18 completed. With overcommit on, the same walk's time grows by the cost of one fork per step (not measured; 3.4 measures only the reaper).
- **Impact:**
  - gwz-py loses function intermittently, under a message nobody can recognise.
  - Path-filtered history becomes slower in proportion to the host's size.
  - Neither cost appears in OD-B3, §3.4, R-B16 or the README text the operator will freeze.
- **Correction:**
  - State this cost in §3.4 "Cost", R-B16 (as a sibling with its exact message) and §8 row 6.
  - Add to 3.4 a per-spawn measurement: the `rev-list` step rate and the fork latency with a 2 GiB gwz-py heap, on M and L.
  - Optionally, have the crate map a spawn's `ENOMEM` from `fork` to a named `RegistryFault`.
  - If the cost is not acceptable, the alternative is a vfork-style spawn inside the crate. That is an operator decision.
- **Closure test:** B5 or B19 gains a gwz-py leg in which an injected `fork` `ENOMEM` on an entry spawn, after the reaper has been created, raises the documented message. The 3.4 evidence records the per-spawn fork time.
- **Architectural:** no. Closing it needs a stated and measured cost; a change of mechanism is optional.

### N-P3-1: On the fork path, A's handlers run inside a registered child before `exec`, and §3.2 unmasks before it ignores

- **Location:** §3.2 Unix (:121-125), with the order "an empty signal mask; SIGINT and SIGQUIT `SIG_IGN` for `ignore_interrupts`". §3.4 "Launch contract" (:167-168). §3.7 (:231-232, "B changes no disposition in gwz's process").
- **Root cause.**
  - std's fork path neither blocks signals nor resets handlers (`unix.rs` :93-118, :345-350). Until `setpgid` in `do_exec`, and until `exec` for a `Process` child that stays in gwz's group, the child is in gwz's foreground group and still has A's SIGINT, SIGTERM and SIGHUP handler.
  - That handler writes one byte to A's pipe. The child's copy of the write end is gwz's own pipe.
  - So a Ctrl-C that lands in that window delivers two count bytes. A promotes the first Ctrl-C to **Force**, and Force restores the default dispositions (A :146-147). The next Ctrl-C then kills gwz by the default action, and the reaper kills git with `index.lock` left (R-B4).
  - Separately, emptying the mask before setting `SIG_IGN` lets a pending SIGINT run A's handler, or take the default action, in a `Process { ignore_interrupts }` child before the ignore takes effect.
  - glibc's `posix_spawn`, which B replaces, closes both issues.
- **Exposure:** about one `fork` window per spawn. `gwz log -- <path>` spawns continuously, so a Ctrl-C during a long walk has a small but real chance of being counted twice.
- **Correction:**
  - The crate blocks all signals in the spawning thread around the spawn.
  - Its closure first sets every signal A catches to `SIG_DFL` (`SIG_IGN` for `ignore_interrupts`), and only then empties the mask.
  - The reaper's contract also resets every other caught handler to `SIG_DFL` before it unmasks.
- **Closure test:** B8 gains a leg (M, L). A seam holds a registered child between fork and `setpgid`; SIGINT is sent to the harness's group. A's pipe receives exactly one byte, and a `Process { ignore_interrupts }` child with SIGINT pending at the closure runs its program.
- **Architectural:** no.

### N-P3-2: An exit seen only as `ECHILD` has no specified status and does not disarm gwz's own kills

- **Location:** §3.3 (:143-144, ":156-159"). B11 (:292-294).
- **Root cause.**
  - When the waiter receives `ECHILD`, the design *knows* the pin is lost, yet the release kill of a `Kill` entry (`output` releases "at once", :147-148) still sends `kill(-pgid)` to an ID that may be free.
  - The RemPlan's argument against S-P2-2 (b), that a check before each kill is itself check-then-kill, does not apply to a recorded `ECHILD`.
  - For gwz-cli this is not an application's choice: Linux keeps an inherited SIGCHLD `SIG_IGN` across `exec`.
  - The status that `exited()` and `output` report after `ECHILD` is also unspecified.
    - 1.0.18 returned an error here: std's wait fails with `ECHILD`.
    - gwz-git's `run` (`cli.rs:17-28`) trusts the status, so a made-up failure reports a commit that did happen as failed.
    - `head_after` catches a made-up success.
- **Correction:**
  - An entry whose exit was seen as `ECHILD` is marked so that every later gwz-side kill of it is a no-op.
  - `exited()` and `output` return an `Io` error naming the lost status, never a made-up `ExitStatus`.
- **Closure test:** B11 traces every `kill(2)`. Under SIGCHLD `SIG_IGN` no gwz-side signal follows the `ECHILD`, and the `output` of a `Wait` child returns the named error.
- **Architectural:** no.

### N-P3-3 (PLAUSIBLE, OS): The macOS `ESRCH` path says nothing about a child that is exiting but not yet a zombie, or about a stale stop report

- **Location:** §3.3 (:141-143).
- **Root cause.**
  - kqueue registration can be refused with `ESRCH` while the process is between `exit` and zombie (XNU's `proc_find` refuses exiting processes; unverified). At that moment `waitid(WEXITED|WNOHANG|WNOWAIT)` reports no exit, and possibly a stale `CLD_STOPPED`.
  - The text says only "is an exit *once* … reports", and gives no way to wait for that without a spin, which B25 forbids.
  - If the exit is missed, a `Wait` entry stays live forever, so `close()` waits without a bound. A `Kill` entry is never released.
- **Correction:** on `ESRCH` with no exit reported, retry with a stated bounded backoff (for example 1 ms doubling to 100 ms) until an exit code appears.
- **Closure test:** a seam in M forces `ESRCH` while `waitid` reports nothing for 50 ms. The exit is seen within the backoff, and the CPU cap holds.
- **Architectural:** no.

### N-P3-4: A failed descriptor sweep leaves the reaper holding the lifeline write end, which silently loses B1

- **Location:** §3.4 "Launch contract" (:169-171).
- **Root cause.**
  - The reaper's copy of the write end is closed only by the sweep.
  - The fallback is keyed on the kernel version ("Linux 5.9+, else"), not on failure. Seccomp profiles reject `close_range` with `EPERM` or `ENOSYS`.
  - Nothing is stated for a missing `/proc`.
  - If the sweep fails, the reaper never reads end of file. Children outlive gwz, the reaper is alive, and no spawn is refused, so R-B2's recognizer never fires.
- **Correction:**
  - Close the reaper's write-end copy explicitly, by number, before the sweep.
  - Fall back to the next method when one fails.
  - If the reaper cannot reach "only 0-3", it calls `_exit(1)`, so that `ReaperGone` is recognizable.
- **Closure test:** B6 or B7 runs on L with `close_range` failing through a seccomp filter, and with `/proc` hidden: either the group ends within 1 s or `ReaperGone` is raised, never silent coverage loss.
- **Architectural:** no.

### N-P3-5: Writer-thread death and a full lifeline leave the waiter and `close()` paths unspecified

- **Location:** §3.3 "The writer thread" (:151-153) and "Order" (:148-149). B7, B25.
- **Root cause.**
  - After the writer dies, a waiter that "waits for its acknowledgement" before reaping either:
    - blocks forever, so `close()` waits without a bound on a `Wait` child that has already exited; or
    - reaps without unregistering, which is R-B1 while the reaper is still alive.
  - With the reaper stopped (R-B2), about 4,096 records (64 KiB of pipe) fill the lifeline. After that, the closure in each child and the writer block, and gwz hangs. `gwz log -- <path>` gets there in about 2,000 steps. B25 stops the reaper for only 5 s, with few spawns.
- **Correction:**
  - After the writer dies, waiters never reap; zombies stay pinned until gwz exits.
  - `close()` counts an exited entry as ended.
  - R-B2 states the hang caused by a full lifeline, or writes become non-blocking, with `EAGAIN` treated as `ReaperGone`.
- **Closure test:** B7's writer-panic leg also exits a `Wait` child, and `close()` returns. B25 gains a leg with 5,000 records written while the reaper is stopped, recording the observed behaviour.
- **Architectural:** no.

### N-P3-6: The registry-less mode of the crate's `Child` ships but is unspecified

- **Location:** §3.1 (:94-98, "over a std child when the gate has no registry"). §3.6 (:206-209). §4 "Interim" (:261-262). OD-B2.
- **Root cause.**
  - This mode carries:
    - the non-candidate build's `rev-list`, `git commit` and `git tag` in 1.1;
    - the candidate's helper before 3.1 and 3.2;
    - `DriverOnly`, a long-lived served core in which PID wrap is realistic, until C.
  - The text does not say:
    - whether `Spec`'s target is applied (the helper today is its own group, but OD-B2 (a) says `git commit` stays in gwz's group);
    - how exit is observed;
    - whether `kill()` or a group kill can follow std's reap.
  - The helper's Unix arm loses tokio in 2.3r-2, so its owner's `tree.kill()` (`owner.rs:253-256, :288-291`) depends on this mode.
- **Correction:** specify the mode as follows:
  - the target is applied as each call site states (the helper `Group`; `rev-list` and the CLI route stay in gwz's group, "as today");
  - exit is observed by std's wait;
  - every kill is a no-op once std has reaped (as std's own `Child::kill` behaves);
  - no group kill after the reap.
- **Closure test:** a gwz-core row (2.3r-2) with a gate that has no registry. A helper cancelled after its leader was reaped sends no signal, and a CLI-route commit stays in the harness's group.
- **Architectural:** no.

## 3. Verified citations

| Citation | Result |
|---|---|
| Plan D-V :999-1062; decisions (i)-(iii) :1055-1062; OQ2 :1005-1008; resolution (2) :1039-1043 | Correct |
| `commit_log/mod.rs:330-358, :337, :347, :354-356` | Correct (std `wait_with_output` at :354-356) |
| `spawn_gate.rs:431-436, :438-446, :450-454` | Correct |
| `process_tree.rs:42, :57-75, :71, :79-86, :89-90` | Correct |
| `owner.rs:77, :97, :253-303, :298` | Correct |
| `https_worker.rs:303`; `runner_tests.rs:299` (30 s) | Correct |
| `process_tree/windows.rs:44-48, :54-62, :115-118, :153-183`; `windows/launch.rs:1-3` | Correct |
| `handshake_local_rows.rs:447` (`TH32CS_SNAPTHREAD`) | Correct |
| `composition.rs:30-35, :37-40`; `driver_only.rs:61, :152` | Correct |
| `Cargo.toml:132-139` (tokio `process` in the candidate table), `:150-152` dev table | Correct (the table header is at :150) |
| `v1.0.18:repository.rs:299-305`; `v1.0.18:refs.rs:172-183` | Correct |
| gwz-git `native_commit.rs:88-95`; `cli.rs` commit at :31-50 | Correct |
| gwz-cli `lib.rs:162, :175-177, :187, :207, :235, :248, :256, :259, :274, :281, :288, :299, :326, :344, :350` | Correct |
| std `unix.rs:342` (setpgid), `:352-372`, `:383-385`, `:408`, `:463`, `:141-149`; `os/unix/process.rs:220-221` | Correct. The fork path's lack of signal blocking (:93-118) supports N-P3-1. |
| tokio `net/unix/pipe.rs:419, :967` | Correct (`from_owned_fd`) |
| Go `os/wait_waitid.go:5-6` | Correct |
| f9 report beside the root checkout | Exists |
| A r1 :135-137, :146-147, :207-210, :267 | Correct, at root `HEAD` |
| Not re-verified on this axis | gwz-sspi `launch.rs`, git2-rs `cred.rs`, `GwzConnectionApiContract.md:289`, gwz-py README :163, the parked design lines. Round 1 verified them, and r2 does not change them. |

## 4. Risks and next action

- **Highest risk: the reaper's SIGKILL of `Wait` groups (N-P2-1).** Under B, the ordinary stop of a gwz-py service, and any gwz death, tears a commit that 1.0.18 would have finished. The operator decided OD-B1 against a comparison with Ctrl-C only. Choice (b), SIGTERM so that git removes its own locks, is cheap and stays within D-V (2)'s "reads the lifeline and calls `kill()`".
- **Second: unpriced fork cost (N-P2-2).** The child-side registration makes every registered spawn a full fork of the host. gwz-py needs this cost stated and measured before 3.2 is briefed.
- **Then the P3s:**
  - the signal window in the fork path, which is a cross-design interaction with A's handler (N-P3-1);
  - three liveness gaps in the waiter, the reaper and the writer that end in an unbounded `close()` or a silent loss of cover (N-P3-3, N-P3-4, N-P3-5);
  - the registry-less mode that ships in 1.1 (N-P3-6).
- **Hand-off watch (not B's defect):** §3.7 depends on A revision 2 dropping A r1 :216-217. A's working copy is uncommitted and outside this scope.
- **Next action:**
  1. Revision 3 closes N-P2-1 (restated cost, R-B4's triggers, and the operator's choice of the `Wait` signal) and N-P2-2 (the cost stated in §3.4, R-B16 and the README, plus the 3.4 measurement), with their rows.
  2. It resolves N-P3-1 to N-P3-6, or defers each with an owner.
  3. A fresh Safety reviewer re-reviews the new blob.
