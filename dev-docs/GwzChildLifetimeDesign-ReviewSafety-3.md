# Cancellation design B r3 — Safety-AXIS REVIEW (round 3)

**Review object:** `/Volumes/projects/limbo/gwz-dev-docs-cancel/dev-docs/GwzChildLifetimeDesign-DRAFT.md`, revision 3, 399 lines. The hash was `0cbb9081363ab26786635b13280df11550a8a0b7` at the start and at the end, so the object did not change. | **Date:** 2026-11-03 | **Axis:** Safety. This covers orphaned processes, kills that reach the wrong process, stale locks, partial state and misreported effects; signal and fork races; Windows jobs and consoles; the forced path and the third interrupt (O5); and whether each residual is bounded and can be recognised.

**Verdict: NO-GO.** Counts: 0 P0, 0 P1, 1 P2 (new), 3 P3 (new).
- All 18 round-2 findings are closed: Safety N-P2-1, N-P2-2 and N-P3-1 to N-P3-6; Consistency N-P2-1 and N-P3-1 to N-P3-9.
- None of the new findings is architectural, so design B's architectural count stays at 0.
- The P2 closes with text: the scope of the registry's existing creation lock is stated for Unix, plus a seam leg in a test row. It needs no new mechanism.
- **Pre-commit:** I will give GO on a revision that does three things:
  1. closes N-P2-1 as its correction states;
  2. adds no new P0-P2;
  3. either resolves N-P3-1 to N-P3-3 or defers each one explicitly with an owner.

## 0. Evidence base

- **Pin.** `git hash-object` gave `0cbb9081…` at both ends. The lane root is at `6d20ef6f`.
- **Read in full:**
  - B r3.
  - Both round-2 reviews (Safety and Consistency).
  - RemPlan-2, including its fix pass G1-G7.
- **Controlling decision.** Plan §10 D-V, :999-1069. That covers OQ2 at :1005-1008, resolution (2) at :1039-1043, decisions (i)-(iii) at :1055-1062, and O4 (a) and O5 at :1063-1069.
- **Design A r3, read at its hand-offs only** (it is the lane's working copy, modified):
  - §3.1 "Unix": SIGINT, SIGTERM and SIGHUP are caught. The handler has a PID check. SIGQUIT keeps its default.
  - §3.1 "Giving control back" (R14).
  - §3.4 "Force", steps (1)-(8): step 1 restores the dispositions; step 4 is `kill_killable()`; step 8 is `_exit`.
  - §4 "B points A relies on".
  - §3.8 (R15).
  - §7 rows R8, R9, R14 and R15.
- **Code at the pins.**
  - gwz-core `b745f1fa`, read through `git show`, because the lane's checkout is at `7f571a18`:
    - `https_auth/runner.rs:100-150`: the helper is waited for before success, so `retire` follows the leader's exit.
    - `process_tree.rs:36-95`, `spawn_gate.rs:415-460`, `composition.rs:15-45`.
    - `commit_log/mod.rs:296-300, :345-361`.
    - `https_auth.rs:28, :142, :175, :190`.
    - `verify_checkout_state.rs:124-129`.
  - gwz-git `9bc4ea9` `src/native_commit/cli.rs:1-80`: commit always passes `-m`.
  - gwz-core `handle_tag.rs:226-231`: a signed tag without a message is refused, so no editor is launched under the CLI route.
- **Toolchain.** Rust std 1.95.0 `sys/process/unix/unix.rs`:
  - :85-150: the `env_read_lock` is held across the fork, and the CLOEXEC status pipe is used.
  - :187-189: `do_fork` is `libc::fork()`.
  - :336-412: `do_exec`, which runs setpgid, then the SIGPIPE reset, then the closures, then `execvp`. At :352-353 the comment reads "Inherit the signal mask from the parent rather than resetting it".
  - :463: any closure rules out `posix_spawn`.
  - :990-996: `kill` after the reap does nothing.
- **Platform facts relied on, not measured:**
  - POSIX discards a pending signal when its action is set to `SIG_IGN`.
  - A fork child starts with no pending signals.
  - glibc's and macOS's `fork()` run `pthread_atfork` handlers, and glibc's `posix_spawn` runs none.
  - Linux runs `exit_files` before `exit_notify`.
- **Method.** Reading only. Nothing was built or run, nothing was modified, and no git mutation was made.

**Provenance and scope.**
- Every mechanism still traces to D-V (2) and (3), decisions (i)-(iii), O4 (a) and O5, or to necessity shown in rounds 1 and 2.
- OD-B4 is surfaced as an operator decision and routed through D. It is not a silent narrowing.
- None of the operator's settled choices is attacked here: OD-B1 (a), OD-B2 (a), OD-B3 (a), Force never exiting mid-write, and the third interrupt ending gwz.

## 1. Prior-finding closure table

| ID | Status | What I checked in r3 |
|---|---|---|
| S-N-P2-1 | **Closed (recorded)** | OD-B1 (:340) and R-B4 (:356) name every death that tears a commit, each verified against A r3: SIGKILL, a crash, the OOM killer, an interrupt after Force began (O5, R14), SIGQUIT (A r3 §3.1 "Unix", row 6.1), a gwz-py host's default SIGTERM or SIGHUP (A R15), `os._exit`, a foreign `exit()`, an `exec`, and Windows CTRL_CLOSE, LOGOFF or SHUTDOWN (R9). The 1.0.18 comparison is stated: a signal to gwz alone let the commit finish, and a group signal let `git` remove its locks. OD-B4 (:343) leaves the reaper's `Wait` signal at (a) and hands it to D for the operator. (b) is recommended. The refusal of a SIGKILL follow-up after a bound is sound: once gwz is gone nothing pins the group ID. Windows stays at (a). B29 asserts (a) and records W; B18 covers gwz-cli SIGKILL and SIGQUIT. My round-2 closure test is met for (a). |
| S-N-P2-2 | **Closed** | §3.4 "Cost" (:215-220) prices the per-spawn fork: page-table copying, a charge under strict overcommit, and a 3.4 measurement. The `OutOfMemory` variant and message are at :111. R-B19 is listed, §8 row 6 has the README sentence, and B5 and B19 carry the injected `ENOMEM` legs. `vfork` is left as the operator's option. |
| S-N-P3-1 | **Closed** | §3.2 (:134-143): the spawning thread blocks every signal; the child ignores the four signals, then resets caught handlers, then applies `ignore_interrupts`, then writes the record, and empties the mask last. The reaper is forked blocked and resets handlers (:191, :196). B8 has the window seam. Two residual gaps in this list are filed below as N-P3-1 and N-P3-2. |
| S-N-P3-2 | **Closed** | §3.3 (:165-167): on `ECHILD` the entry is unregistered at once, later kills do nothing, and the status is `StatusLost`, never a made-up one. B11 traces every `kill(2)`, and B27 covers the helper's `StatusLost` message. |
| S-N-P3-3 | **Closed** | §3.3 (:162-164): a 1 ms backoff doubling to 100 ms on `ESRCH` with no exit reported, marked **(OS)**. B25 has the 50 ms `ESRCH` seam under the CPU cap. |
| S-N-P3-4 | **Closed** | §3.4 (:197-200): the reaper's write-end copy is closed by number; the fallback runs on failure, not by kernel version; `_exit(1)` if only 0-3 cannot be confirmed. B4 runs with seccomp and `/proc` hidden: "never neither". |
| S-N-P3-5 | **Closed** | §3.3 (:174-178): a waiter reaps only after its acknowledgement or after the reaper has been reaped; `close()` counts an exit it observed as ended. R-B2 records the full lifeline with a recognizer (state `T`). B7 has the writer-panic leg; B25 has the 5,000-spawn leg. I accept the decline of a non-blocking lifeline: `EAGAIN` would call a stopped reaper dead. |
| S-N-P3-6 | **Closed** | §3.1 "Without a registry" (:121-123); R-B13; B28. The group kill comes before the reap, which is better than my round-2 "std's wait". |
| C-N-P2-1 | **Closed** | §3.1 (:107-119): eight fixed messages; `io::Error::other(fault)`; a per-site table for the helper, the CLI route, `rev-list` and exit. `runner.rs:115-121` and `:145` are in 2.3r-2's Owns. B27 and B19 match the table. I verified `https_auth.rs:142, :175, :190`, `verify_checkout_state.rs:124-129` and `commit_log/mod.rs:296-300, :359-361`. |
| C-N-P3-1 | **Closed** | §4 2.3r-1 Owns (:284) lists root `MODULE.bazel` and its lock, the inventory, `check_crate_versions.py` and the allowlist entries. §8 row 1 and "Waves" name them as shared and merged serially. |
| C-N-P3-2 | **Closed**, with a remaining gap | §3.1 "Value" names the registry's own creation lock. §3.1 and §3.7 name `ProcessGate::kill_killable()` and `close_children()`. Its scope is stated for Windows creations only, which is N-P2-1 below. |
| C-N-P3-3 | **Closed** | As S-N-P3-5. |
| C-N-P3-4 | **Closed** | Requirements are BR1-BR8 and rows B1-B29. BR4 quotes D-V :1006, which I verified. |
| C-N-P3-5 | **Closed** | **(reg)** appears only on the W helper legs (B13, B18, B19); B3's W leg is unmarked. |
| C-N-P3-6 | **Closed** | The §8 heading reads "row 6 by step 3.2"; §4 3.2 owns `gwz-py/README.md`. |
| C-N-P3-7 | **Closed** | The §8 note names OD-C2 (a), `3448c561` :245-246. |
| C-N-P3-8 | **Closed** | OD-B2's "Not chosen" records an own group without a registry, with its cost. |
| C-N-P3-9 | **Closed** | §3.2 "Windows targets differ" (:153-157); R-B7's recognizer is given per platform. |

## 2. New findings

### N-P2-1: On Unix, `close()` and `kill_killable()` have no stated exclusion against an entry spawn already in flight. The reaper can then SIGKILL a `Wait` child while gwz lives, or the child can write a record into a reused descriptor.

- **Location:**
  - §3.1 "Value" (:86-88): "its own creation lock (§3.2)".
  - §3.2 "Unix" (:134-143), where no lock is named.
  - §3.2 "Windows" (:150-151), which is the only place the creation lock is scoped: "`close()` takes the creation lock, so a creation in flight is in its job or terminated".
  - §3.4 "Its end" (:208-211).
  - §3.7 (:258-259).
  - §3.8 (:276-277): `close_children()` "refuses that thread's later spawns".
  - BR5 (:37); B22 (:327); B26 (:331).
- **Violated invariant:**
  - BR5: a registered `git commit` or `git tag` is "never killed while gwz lives: not by … `close()`".
  - The lifeline write end is written only by gwz and by a registered child between fork and exec (§3.5).
  - The member report is honest (BR7 and A6): B26 expects a later member to be refused `Closed`.
- **Root cause.**
  - An entry spawn on Unix is not atomic. It passes the `Closed` and `Capacity` checks, std forks, the child's closure writes the register record on its inherited copy of the write end, and the child execs.
  - Only after that does the parent record a live entry. With a large gwz-py heap the fork alone takes milliseconds (§3.4 "Cost").
  - The design scopes the creation lock to Windows creations only. On Unix, `close()` and `kill_killable()` as written:
    1. set `Closed`;
    2. act on "every live" entry;
    3. in `close()`'s case, close the write end.

    None of these steps excludes or awaits a spawn that is already past its check.
- **Reproduction (L or M, gwz-py after 3.2; this is B26's own scenario).**
  1. A daemon thread T1 loops over members with `client.commit(...)`. The main thread calls `sys.exit(0)`, and the atexit join runs `close_children()` on T0.
  2. T1's entry spawn for member 2's `git commit` (`Group`, `Wait`) passes the `Closed` check and enters std's fork.
  3. T0's `close()` sets `Closed`. Member 2 is not yet a live entry, so `close()` sees no live `Wait` entry, kills nothing, and closes the write end.
  4. T1's child writes its register record on its inherited copy, which succeeds because the reaper still holds the read end, and then execs `git`. Exec closes the last copy, and the reaper reads the register record, then end of file.
  5. The reaper sends SIGKILL to `git`'s group while gwz-py lives. `output` returns death by SIGKILL, so member 2 reports `git commit failed:` instead of `Closed`.
  6. Under load the reaper runs late. `git` may by then hold `index.lock` with `pre-commit` running, which leaves R-B4's residue while gwz lives.
  - **Variant 1.** Step 3 closes the write end before T1's fork, and another thread's `open` reuses that descriptor number. The child's closure then writes the 16-byte register record into that file. The child shares the open file description, so the write lands at the parent's offset in, for example, a pack or index being written.
  - **Variant 2.** The reaper has already exited at end of file, so the child gets `EPIPE` and the member reports `ReaperGone` instead of `Closed`.
  - **`kill_killable()` (Force step 4).** A `Kill` spawn in flight past the check survives step 4, contrary to A's "every `Kill` child ends now". The exposure is bounded: a `rev-list` step is short, a helper is cancelled by its owner after step 1's host close, and the reaper kills either at step 8's exit.
- **Impact:**
  - A BR5 breach and a torn commit while gwz is alive.
  - A misreported member.
  - A rare write into a descriptor gwz does not own.
  - All of this is on the path the design itself specifies: B26 and R-B17.
- **Correction (text only, using the existing lock).** State for Unix what §3.2 already states for Windows:
  - Every entry spawn holds the registry's creation lock from its `Closed` and `Capacity` check until std's spawn returns and the entry is recorded live. A spawn that fails instead has its token unregistered.
  - `close()` and `kill_killable()` take that lock to set `Closed` and to snapshot the live entries.
  - `close()` closes the write end only after that point.
  - No lock is taken in the child, because the fork check comes before any lock and the closure takes none.
  - Spawns already serialize under the gate's hold, so the cost is nil.
- **Closure test (B22 and B26 legs, M and L).** A seam holds an entry spawn after its check, and separately between fork and exec, while `close()` or `kill_killable()` runs. Pass conditions:
  - the spawn is either refused `Closed` before its fork, or its `Wait` child runs to exit and `close()` returns after it;
  - the reaper's trace shows no kill of that token;
  - no write reaches a descriptor other than the lifeline (traced).
- **Architectural:** no. It extends an existing lock's scope to Unix.

### N-P3-1: The child's pre-exec ignore list leaves out SIGTERM, which O4 (a) makes an interrupt source, so a group-directed SIGTERM in the fork-to-setpgid window kills a `Wait` child before exec

- **Location:** §3.2 "Unix" (:137-140), "SIGINT, SIGQUIT, SIGTSTP and SIGHUP `SIG_IGN`"; B8 (:313); B15 (:320).
- **Root cause.**
  - A r3 catches SIGTERM (O4 (a)).
  - In the child, `kill %1`, `timeout` without `--foreground`, or a supervisor sending SIGTERM to gwz's group leaves a SIGTERM pending, because the signal is blocked from the fork onward.
  - The closure then resets every caught handler to `SIG_DFL` and empties the mask, so the child dies by SIGTERM before exec.
  - std reports `Ok` for a child that dies before exec (:141-149 reads end of file on the status pipe), so `git commit` "fails" with empty stderr.
  - The ignore list covers only terminal-generated signals. Since round 2, O4 (a) has put SIGTERM in the same class.
- **Impact:** No torn write, because nothing ran. The member is misreported, and decision (ii) ("let finish") is not honoured for SIGTERM in that window, although it is honoured for SIGINT.
- **Correction:** Add SIGTERM to the window's `SIG_IGN` set with the same "unless gwz ignored it" restore. Optionally state that any other signal sent to gwz's group in that window keeps its default.
- **Closure test:** B8's `Group` `Wait` leg adds SIGTERM to the harness's group inside the seam: the child runs to exit.
- **Architectural:** no.

### N-P3-2: "Last, an empty mask" departs from std and from 1.0.18, both of which give the child the spawning thread's mask

- **Location:** §3.2 (:143); B8 (:313), which asserts "the child's mask is empty"; §2 (:68), which cites std's "mask inherited".
- **Root cause.**
  - std 1.95's `do_exec` deliberately inherits the mask (:352-353), and its `posix_spawn` path sets no mask either.
  - 1.0.18's CLI route therefore gave `git` and its hooks the mask of the host or launcher.
  - B blocks every signal around the spawn, which is correct. It then sets the child's mask to empty instead of the mask it saved before blocking.
  - A launcher or embedding application that blocks a signal for its children loses that.
- **Impact:** a parity deviation with a small practical surface (`git` is in its own group). It becomes C's concern for `Process` children, which stay in gwz's group.
- **Correction:** The closure restores the spawning thread's mask saved before the block, as std does. B8 asserts that equality, and that a mask blocking SIGINT and SIGPIPE is carried through.
- **Closure test:** B8 as amended.
- **Architectural:** no.

### N-P3-3 (PLAUSIBLE, OS): Every fork B introduces runs the host's `pthread_atfork` handlers, which falsifies the reaper's launch contract in an embedding host and adds an unrecognizable silent mode

- **Location:**
  - §3.4 "Launch contract" (:195): "Only async-signal-safe calls, no allocation, no return into Rust or Python".
  - §3.4 "Creation" (:191) and "Cost" (:212-220).
  - R-B2 and R-B3 recognizers (:354-355); §8 row 6 (:382).
- **Root cause.**
  - glibc's and macOS's `fork()` run every registered `pthread_atfork` prepare handler in the parent and every child handler in the new process before `fork` returns.
  - std's fork path calls `libc::fork()` (:187-189). glibc's `posix_spawn`, which 1.0.18's `rev-list` and CLI route used, runs none.
  - In gwz-py the host's C libraries (BLAS thread pools, gRPC fork support and others) may register handlers that are not async-signal-safe, or that take locks or start threads. Three consequences follow:
    1. A child handler that blocks in the reaper leaves a reaper that is alive, never reads, and is not in state `T`. BR1 is lost silently: `ReaperGone` is never raised, and R-B2's recognizer does not fire.
    2. Every entry spawn now runs the host's prepare and child handlers, which is a behaviour change not priced in "Cost".
    3. The reaper also keeps the host's file mappings and SysV shared-memory attachments for the host's lifetime, for example the disk space of a mapped file the host deletes. The README says only "a copy of memory".
- **Correction:**
  - Linux: fork the reaper through a call that skips atfork handlers (`_Fork` on glibc 2.34+, or a raw `clone(SIGCHLD)`). It is still a forked process under D-V (2).
  - macOS, and every per-spawn fork: name the handlers in §3.4 "Cost", in a residual with the recognizer "a reaper alive and not stopped after gwz died", and in the README sentence, together with the mappings.
- **Closure test:** a B6 or B7 leg (L) in which the harness registers a `pthread_atfork` child handler that blocks before `create`. The result is that the reaper still ends a child at the harness's SIGKILL (Linux), or the outcome is recorded (macOS).
- **Architectural:** no.

## 3. Verified citations

| Citation | Result |
|---|---|
| Plan :1006 (BR4's quote), :1039-1043, :1055-1062, :1063-1069 (O4 (a), O5) | Correct |
| A r3 §3.1 "Unix" (SIGQUIT keeps its default; PID check), "Giving control back" (R14), §3.4 Force steps 1, 4, 8, §4 "B points", R9, R14, R15, row 6.1 | Correct as B cites them. A §4 point 6 says "A's PID check covers B's child before `exec`", and B no longer relies on it (RemPlan hand-off 5). The two are consistent. |
| std `unix.rs:93-94` (fork under `env_read_lock`, no signal blocking), `:141-149`, `:187-189` (`libc::fork`), `:342`, `:352-353` (mask inherited), `:383-385`, `:463`, `:990-996` | Correct. :352-353 supports N-P3-2; :187-189 supports N-P3-3. |
| gwz-core `runner.rs:110-121, :141-145`; `process_tree.rs:57-75, :71, :79-90` | Correct. The helper waits for its status before success (supports §3.3's retire order). |
| `https_auth.rs:28, :142, :175, :190`; `commit_log/mod.rs:296-300, :347, :354-361`; `verify_checkout_state.rs:124-129` | Correct |
| `spawn_gate.rs:431-454`; `composition.rs:20-40` | Correct. The gate's hold is in gwz-core, so the crate cannot take it, which is why N-P2-1's fix uses the registry's own lock. |
| gwz-git `native_commit/cli.rs:17-28` (`run`), `:31-50` (`-m` always); gwz-core `handle_tag.rs:226-231` (no unsigned-message editor path) | Correct. No editor reaches the background group. |
| Round-2 citations of `owner.rs`, `windows.rs`, `handshake_local_rows.rs:447`, `v1.0.18` lines and gwz-cli exit sites | Not re-read. r3 does not change them, and round 2 verified them at the same pins. |

## 4. Risks and next action

- **Highest risk: N-P2-1.** The race sits on the exact path the design specifies for gwz-py's close (B26, R-B17). It can kill a running commit while gwz lives, or write into a descriptor gwz does not own. The fix is one sentence in §3.2 and §3.4 plus a seam leg.
- **Accepted, watched.**
  - **OD-B4 stays at (a) until D puts it to the operator.** Under (a), every death of the host during a commit leaves `index.lock`. This is now stated honestly, with B18 and B29 asserting it, and the operator should see (b) early.
  - **R-B1 and the forced path.** Force's step-4 kills can leave zombies still registered at step 8's `_exit`. That is R-B1's "dies after a child exited and before its unregistration", reached routinely, not by chance. Exploiting it needs a PID wrap between init's reap and the reaper's kill (microseconds), so it is negligible but not recognizable afterwards. Optionally, Force could wait, with a bound, for the unregistration of the `Kill` entries it killed.
- **Optional, not a finding: inherited SIGCHLD `SIG_IGN` (Linux).** gwz-cli started with SIGCHLD ignored makes every registered status `StatusLost`. A CLI-route commit that landed then reports "failed to run git commit: child-process exit status lost". This matches 1.0.18's `ECHILD`, and the message is clearer. Resetting SIGCHLD to `SIG_DFL` in `run` before the registry would remove both the failure and R-B1's exposure in gwz-cli. That change needs A's `signal` allowlist kind.
- **Unverified (OS) facts**, carried as hypotheses until their rows run:
  - discarding a pending signal under `SIG_IGN` (B8);
  - kqueue on a zombie and the `ESRCH` backoff (B25);
  - `proc_pidinfo` in the forked reaper (B6);
  - atfork behaviour (N-P3-3).
- **Next action.**
  1. A fix pass closes N-P2-1: the Unix lock scope in §3.2 and §3.4, and B22 and B26 seam legs.
  2. It resolves or defers N-P3-1 to N-P3-3 with an owner.
  3. A reviewer confirms the new blob against this report's pre-commit.

  This round found no architectural root cause, so the cap does not stop B.
