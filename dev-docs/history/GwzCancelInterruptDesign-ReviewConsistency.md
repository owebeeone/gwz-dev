# Cancellation design A r1 — Consistency-AXIS REVIEW

**Review object:** /Volumes/projects/limbo/gwz-dev/dev-docs/GwzCancelInterruptDesign-DRAFT.md (hash at start `d516bac88df5cef86f91b35c03019db8a8642464`, at end `d516bac88df5cef86f91b35c03019db8a8642464`, unchanged) | **Date:** 2026-11-01 | **Axis:** Consistency. I checked the design against D-V, every document and code line it cites, its superseded-clause table, the convergence plan and the 2.3 plan, and its hand-offs to designs B, C and D.

**Verdict: NO-GO.** 0 P0, 0 P1, 3 P2, 9 P3. I pre-commit to GO on a revision that resolves P2-1, P2-2 and P2-3 as specified below, provided the fixes add no new P0-P2 on this axis. The P3s should be closed in the same pass, but they do not block.

## 0. Evidence base

- **Pins.** Root `b4b827d6`; gwz-core `b745f1fa`; gwz-cli `c0ba717`; gwz-py `2185cc6`; gwz-git `9bc4ea9`; git2-rs `d13951f`; libgit2 `b172e3d18`. These match the design's citation heads. CPython 3.11.16 and 3.10.15 runners were read from local installs. Revision 1 of the amendment was read from `git show 9d5a4d4c:dev-docs/GwzCancellationAmendment-DRAFT.md`.
- **Documents read.** The object in full; convergence plan §4 (:300-331, :705-745) and §10 D-V (:999-1056); 2.3 plan 2.3i/2.3k/Q2 (:180-217, :320-335); design B (`GwzChildLifetimeDesign-DRAFT.md`, blob `48b992d9`) §3.1-§3.8, §4 and §7, for hand-offs only; history amendment r2 :690-715; round-1 review IDs cited by A; MachineOutput.md; ErrorCatalog.md:35; GwzCoreSessionDesign.md §5.3 and :687; GwzConnectionApiContract.md :264-270; gwz-py per-operation design §2.5; gwz-py README:220-228; the f9 report.
- **Method.** Every `file:line` citation in §2, §3, §6 and §8 was opened at the pinned head. Code facts were probed with grep and awk; no build was run. Nothing was modified.

## Problem, requirement provenance, and scope

D-V (OQ2 reversed; OD-C1 to OD-C6; resolutions (1) to (3); the split) is the authority. Tracing each mechanism back to it:

- **Traced to a D-V clause:**
  - one interrupt source (OD-C1);
  - first-interrupt cancel and the 5 s bound (OQ2, OD-C4);
  - Force with the write rule and file-granular safe points (resolution (1));
  - exit 130 plus JSON marking (OD-C4);
  - pager and `forall` rule (OD-C6);
  - gwz-py `Canceller` first (resolution (2)).
- **Traced to demonstrated necessity, not to a clause:**
  - the `write-gate` crate and the fetch split, which are what resolution (1) needs to hold;
  - the file-worker cause fix in 3.0e, from the f9 finding 2 and D-V's "cleans up".
- **Narrowings that A surfaces properly:** O2 (`Wait` children are not killed at Force) and O3 (OD-C7 (a) to (d)). The "beyond D-V (1)" rows of §7 are flagged for D.
- **Narrowings or widenings A does not surface:**
  - R1 is classed "no", which hides stale ref locks (P2-3).
  - The forced `--jsonl` path drops OD-C4's marking (P3-6).
  - SIGTERM and SIGHUP are mapped to exit 130 without D-V provenance (P3-7).
- **Scope:**
  - The A/B/C/D split in §0 matches D-V's split bullet.
  - The gwz-py caller-cancellation part (resolution (2), second clause) is assigned to A. D-V allows that ("carry over to whichever design owns each mechanism"), and B does not claim it.

## 1. Findings

### P2-1 (ARCHITECTURAL) 3.1c cancels through the per-operation entry that Phase 3 itself replaces (3.1) and retires (3.3)

- **Location:** §3.2 "What it cancels"; §3.4, "The gated entry passes it to the runtime"; §5 rows 3.0d and 3.1c and the graph; §8, which has no row for plan 3.1 or 3.3.
- **Violated invariant:** the design's placement must agree with the plan it amends, and no step may build on code that a later step in the same graph deletes.
- **Evidence:**
  - Plan 3.1 (:721-723): "`globalargs/dispatch.rs` builds one `HostOwner` per command through the Phase 1 API… The old entry points stay until 3.3."
  - Plan 3.3: "`with_local_transport*` and `with_cancellable_local_transport*` removed after 3.1 and 3.2 have both landed."
  - A's graph runs `{3.1, 3.0d, 3.0e} → 3.1c` and `{3.0f, 3.1c, 3.2b} → 3.3`.
  - A's 3.1c still "runs the network command through the cancellable entry's gated form (3.0d) in place of `with_local_transport_native*` (`dispatch.rs:66-79`)". 3.0d builds that gated form in `cancellable.rs`.
  - At 3.1c's start, 3.1 has already replaced `dispatch.rs:66-79`, and 3.3 later deletes the entry family 3.1c routes through.
  - The fallback clause "under 3.1's host, the command's request takes the same token and gate" cites no host API.
  - The contract cancels a host operation through `Operation::cancel_handle() -> OperationCancel` (`GwzConnectionApiContract.md:114, :149, :264`), not through a session-host `CancellationToken` from `CallControls::new`.
  - `transport_host/mod.rs:270-271` says requests move onto the connection host in Phase 3. A's gate carriage ("the runtime's state holds the gate… each request's context takes it") is therefore designed against the runtime that is being retired.
  - The supporting claim "gwz-core gives the CLI no per-admission hook" is likewise uncited for HostOwner.
- **Interleaving:** 3.1 lands on the HostOwner path, then 3.1c re-routes the CLI through `with_cancellable_local_transport_native*` (gated), then 3.3 removes that family. Either 3.3 breaks the CLI's only cancellation route, or 3.1c silently diverges from its own text.
- **Impact:** the release-path design of the first and second interrupts is unimplementable as written, and lanes in Phase 3 would collide on the meaning of 3.1/3.3.
- **Correction:**
  - State 3.1c's cancel against the 3.1 host. Name the API by which the command's operation is cancelled (`OperationCancel`, or a token passed at admission, with a cite).
  - State how the write gate reaches the host's per-request backends.
  - Drop "the gated cancellable entry" from 3.0d, or keep it and amend 3.3.
  - Add §8 rows for plan 3.1 and 3.3.
- **Closure test:** §3.2 and §3.4 name only APIs that exist after 3.1, each with a cite. No step in §5's graph edits or depends on code that a successor step removes. §8 lists the 3.1 and 3.3 text changes.

### P2-2 O1's recommended route cannot happen in the plan's order: the server lifecycle (Phase 2) cannot take a module that lands in Phase 3

- **Location:** §3.1 Install; §4 bullet C; §7 R12; §9 O1; §5 placement of 3.1c.
- **Violated invariant:** a hand-off must be satisfiable in the plan's order without breaking an operator decision (OD-C1: "gwz-core and gwz-py install none").
- **Evidence:**
  - 2.3 plan 2.3i (:181-190) is a gwz-core step (`server/{run,shutdown,log}.rs`) with "SIGINT/SIGTERM and Windows Ctrl-Break through a handler that sets a flag only". It has the red row "a second signal or `force` ends the detached wait".
  - 2.3k (:207-217) is the gwz-cli server family. Both are Phase 2 (waves 3 and 4).
  - A's module is created by 3.1c, which sits in Phase 3 "(after Phases 1 and 2)".
  - O1 recommends "C makes the server lifecycle take A's module", and §3.1 excludes the server lifecycle commands from installation "until C" (R12).
- **Interleaving:** 2.3i must land in Phase 2. Either it keeps its gwz-core handler (breaking OD-C1 as decided), or it ships without one, its signal rows cannot pass, and `gwz server start --foreground` dies by the OS default until a Phase 3 step.
- **Impact:** the gap appears only as an open option in O1. A's step table offers no earlier step that would make the recommended option possible, so the operator is asked to pick an option that the schedule rules out.
- **Correction:** either
  - split the interrupt source (pipe, handler, reader, `Interrupts` value, the `signal` check kind) into a small gwz-cli step placed before 2.3i/2.3k in Phase 2, with 3.1c keeping Cancel/Force/output; or
  - state that 2.3i's handler and its signal rows move to C in Phase 3, and add an §8 row amending 2.3i's text.

  Present O1's alternatives with that cost.
- **Closure test:** the plan graph shows the interrupt module before every step whose red rows send a signal. No step text in the 2.3 plan still installs a handler in gwz-core.

### P2-3 (ARCHITECTURAL) Three of the "seven" checkout safe points sit inside a held libgit2 ref transaction, so Force leaves stale `<ref>.lock` files, and R1 hides this

- **Location:** §3.5 table row 1; §7 R1 ("a partly checked-out tree, index and HEAD old … no (file-granular stop)"); row 6.8.
- **Violated invariant:** OQ2's "no … stale lock". D-V (1) accepts residue only from SIGKILL, a crash or power loss; a stop at a safe point must leave only file-granular checkout state.
- **Evidence:**
  - `merge_prepared.rs:248-249` calls `transaction.lock_ref(&local_ref_name)`, then checks out at `:264-266` (the fast-forward), then `set_target`/`commit` at `:268-271`.
  - `merge_recovery.rs:143-144` locks, checks out at `:166-175`, and drops the transaction at `:182`.
  - `merge_recovery.rs:197-198` locks, checks out at `:246-251`, and commits at `:256`.
  - git2-rs `lock_ref` calls `git_transaction_lock_ref` (`git2-rs/src/transaction.rs:40-43`). This reaches `loose_lock` → `git_filebuf_open(... ref_path ...)` (`libgit2/src/libgit2/refdb_fs.c:1175-1179`), which creates `<ref>.lock` on disk.
  - A's `safe_point` "blocks for good", and Force then `_exit`s (step 8), so the transaction's unlock never runs.
- **Reproduction:**
  1. Run `gwz merge` on a fast-forward of N files.
  2. Interrupt twice while the checkout is at file k.
  3. Result: `refs/heads/<branch>.lock` remains. The next `gwz merge`, `gwz merge --continue`/`--abort` or `gwz pull` on that member fails with `GIT_ELOCKED` until the user deletes the lock by hand.
- **Impact:** R1 understates the residue and classes it "no", so a stale lock that OQ2 forbids is never put to the operator. Merge recovery, the documented recovery command, is blocked.
- **Correction:**
  - Add the invariant "a safe point is placed only where the thread holds no Git lock file".
  - The three checkouts under a held transaction run to their end, as in R8, with the notice naming them.
  - Alternatively, keep their safe points, split R1 with the `.lock` path marked "beyond D-V (1)", and add it to §9.
- **Closure test:** extend row 6.8 to Force at file k of a fast-forward merge (`merge_prepared.rs:264`) and of both recovery checkouts. After exit, no `refs/**/*.lock` exists, and the member's next `gwz merge --abort` succeeds.

### P3-1 The §8 table omits clauses the design contradicts, and one citation is off by one

- **Location:** §8; §3.6.
- **Omitted clauses:**
  - `MachineOutput.md:67-69`: a top-level error has "meta: null, no members, and one error entry". §3.6 makes an interrupted execution error's `errors` "[that entry, its own error]".
  - `MachineOutput.md:670-672`: "A failed invocation … ends with one structured error response. Event-stream completion is not published until that final response…". A forced `--jsonl` stream has none.
  - `GwzCoreSessionDesign.md:687`: "the same exposure the CLI has under Ctrl-C". A narrows that exposure (r1 said so explicitly), and the sentence becomes stale.
- **Off-by-one:** row 7 cites `:141-142` for "Any other entry in `errors` is an error the operation reports in its own right"; the sentence is at `:142-143`. §2 has the same off-by-one.
- **Correction:** add the three rows and fix the citation.
- **Closure test:** every live clause that §3.6 or §3.4 contradicts appears in §8 with exact lines.

### P3-2 The long-local-phase inventory is incomplete

- **Location:** §2 "Long local phases"; §3.5 row 1 ("the seven checkout sites") and the closing paragraph.
- **Missing sites:**
  - `merge_prepared.rs:339`: `repo.checkout_tree(tree.as_object(), None)`, a full checkout for the non-fast-forward merge, also under the `:248` lock. It is missing from §2 and §3.5, which contradicts "every other write sequence writes a bounded set per member".
  - `git/gitbackend/preservation.rs:344`: a second production `stash_save`.
- **Counting mismatch:** §2 lists eight checkout sites, the clone included, but §3.5 says "seven" without saying that the clone is excluded because it is parked whole.
- **Correction:** list both sites and classify them; per P2-3, `:339` runs to its end. Say "seven (the clone is parked, R6)".
- **Closure test:** the §2 list matches a grep of `checkout_tree`, `reset(` and `stash_save` in non-test gwz-core source.

### P3-3 The `signal` check's coverage and B's allowlist entries conflict with A's text

- **Location:** §3.1 Check; row 6.5; §5 3.0d Owns.
- **Problems:**
  - gwz-transport has no process-globals check or allowlist. Only gwz-cli and gwz-py carry `process_globals_allowlist.json`, and only gwz-cli's CI references the script. Yet A runs the kind "for … gwz-transport", and row 6.5 asserts "each of the four repositories' runs". 3.0d owns no gwz-transport, gwz-py or gwz-cli CI file.
  - "Only gwz-cli's allowlist has an entry" contradicts B §3.7, which puts entries "in `scripts/checks/process_globals_allowlist.json` for `crates/children`" in gwz-core.
  - Moving `signal` and `sigaction` from the `env` kind (`check_process_globals.py:103-104, :540-541`) to `signal` re-kinds B's 2.3r-2 entries in a gwz-core allowlist that 3.0d does not own.
- **Correction:** name the repositories that actually run the check and the files that wire it, state B's entries under the new kind, and add the gwz-core allowlist to 3.0d's Owns.
- **Closure test:** row 6.5 lists only runs that exist after 3.0d, and the gwz-core check stays green with B's entries.

### P3-4 The write-gate plumbing for `par_map_per_host` is unspecified, and the only plumbing-free form is a thread-local, which A8 forbids

- **Location:** §3.4 "Where sequences open and park"; §5 3.0d Owns.
- **Evidence:** `par_map_per_host(items, global_limit, per_host_limit, host_of, f)` (`par_map_per_host.rs:35-47`) has no backend or context parameter. "Each worker as `writing` when the caller was writing" therefore needs the gate passed by the six callers (`handle_fetch.rs:154`, `push_member.rs:337`, `publication.rs:245`, `handle_materialize/apply.rs:43`, `member_validation.rs:33, :49`, `handle_init_from_sources.rs:147`), none of which is in 3.0d's Owns. A thread-local would break A8 and the no-globals rule.
- **Also:** `MemberLockManager` is built only on the served path (`session_host/served.rs:81`), so its `outside` bracket is C's concern, not the CLI's.
- **Correction:** state the parameter and list the callers in Owns, or route through `OperationServices`.
- **Closure test:** 3.0d's Owns names every file whose signature changes.

### P3-5 Hand-off wording with B is loose

- **Location:** §3.4 Force steps 5-6; §3.1 Install; §3.4 last sentence.
- **Problems:**
  - B §3.7 says A calls `close()` "only after the last write sequence has ended". A's `wait()` returns when writers are "parked, stopped or ended", which is a weaker guarantee. It is harmless today because a parked or stopped thread holds no live `Wait` child, but the two contracts differ.
  - A installs "right after parsing … before any gwz thread or child". B's 3.1 amendment forks the reaper "after `worker_host::early` and before any thread". The relative order is unstated, and A's "before any … child" is false if B forks first.
  - "Ended by B's reaper when the process exits" omits Windows, where the job does it (B §3.6).
- **Correction:** align the wording and state the call order in `run`.
- **Closure test:** A and B use the same precondition for `close()`, and both name one order in `run`.

### P3-6 OD-C4's "JSON response marking the command cancelled" is not delivered on two paths, and neither is recorded

- **Location:** §3.6 After Force; §3.1 Windows.
- **Problems:**
  - A forced `--jsonl` command writes no response object, so it has no marking. This is a narrowing of OD-C4 that is not listed in §7 or §9; R13 covers only truncation.
  - On Windows, CTRL_CLOSE goes straight to Force with no Cancel first. A main thread that took the claim before seeing the interrupt can write an unmarked document and `exit(0|1)` while Force runs, which races A4's "exits 130".
- **Correction:** record both in §7 or §9, or make the main thread consult the interrupt state after taking the claim.
- **Closure test:** a residual row or §9 item names each case.

### P3-7 SIGTERM and SIGHUP mapped to exit 130 lack D-V provenance, yet §8 row 4 freezes them

- **Location:** §1 A4; §3.1 Unix; §8 row 4.
- **Evidence:** D-V speaks of Ctrl-C. OD-C4 is "130 … (128 + SIGINT, the shell convention)". The SIGTERM/SIGHUP enumeration comes from r1 C1, which is source history, not authority. The shell convention for SIGTERM is 143.
- **Correction:** trace the choice or put it to the operator (§9).
- **Closure test:** §1 or §9 cites the authority for non-SIGINT sources mapping to 130.

### P3-8 Notice modes contradict each other

- **Location:** §3.2 Wait; §8 row 5.
- **Evidence:** §3.2 says "Human output gets one writer line at Cancel". §8 row 5 says "The interrupt and forced-exit notices are written to stderr in every output mode."
- **Correction:** pick one rule and state it in both places.
- **Closure test:** §3.2, §3.6 and row 5 agree mode by mode.

### P3-9 Citation and fact exactness

- **Locations and errors:**
  - §7 R17 lists "closed stderr" as a panic trigger. Rust's std swallows `EBADF` on stdout and stderr, so a closed fd 2 does not panic; only EPIPE (or EIO) does. The `tee` case stands.
  - §3.8 cites 3.11's `asyncio/runners.py:150-157` for 3.10. Those lines do not exist in 3.10, where `asyncio.run` reaches the bridge only through `_cancel_all_tasks` in its `finally`. A `KeyboardInterrupt` raised during a task step there ends the task without cancelling it.
  - §3.8 says "`ClientHost.call`", but the Python method is `NativeCoreBridge.call` (`bridge.py:318`). `ClientHost` is the native class.
  - The f9 path `build-targets/winflk-logs/f9-report.md` is relative to `/Volumes/projects/limbo`, not to the workspace, and the heading is "Findings outside scope".
  - §5 gives 3.0d "After: Phase 1", while the graph places it "(after Phases 1 and 2)". The new graph line also drops the original `3.0`/`3.0b → 3.3` edges.
  - "3.0e adds it if missing" leaves a fact undetermined that a design should settle.
- **Correction:** fix each item.
- **Closure test:** re-verify each cite at its pinned head.

## 2. Verified citations

Verified exactly at the pinned heads (each marked ✓ in my checks):

- **D-V and plan.** D-V :999-1056, OQ2 :1005-1010, OD-C1–C6 :1030-1031, resolutions :1035-1045, split :1046-1055. Plan :315, :325, :329-331, :712, :717-720, :721-723. History r2 OD-C7 :695-702. Round-1 IDs Safety P2-6 and P2-11, Consistency P2-2. 2.3 plan :186-187.
- **gwz-cli.** `lib.rs:160-178, :228-300, :323-326, :340-344`; `dispatch.rs:57-89, :66-79, :82-87, :91-96`; `render_exit.rs:82-95`; `git_status_json.rs:71`; `pager.rs:138-145, :155-170`; `forall.rs:203-217, :260`.
- **gwz-core transport and gate.** `cancellable.rs:10-11, :40-88, :145-147, :160, :178`; `gate.rs:140, :156`; `connection_host.rs:438-449`; `mod.rs:269, :463-467, :504`; `driver_only.rs:152`; `backend.rs:26, :120-125`; `request.rs:31-32`.
- **gwz-core gitbackend and checkouts.** `transport.rs:32, :41, :67, :110, :150, :223, :235, :340, :379, :493-494, :718, :740`; `refs.rs:33, :134`; `repository.rs:31, :56, :81, :108-115, :151-169`; `merge_prepared.rs:264`; `merge_recovery.rs:166, :246`; `pull_head_plan.rs:182`; `root_lock.rs:166`; `stash.rs:18, :44, :59`; `stash_support.rs:18-26`.
- **gwz-core copy, helpers and checks.** `copy-contract/lib.rs:39-52`; `refcopy/ordinary.rs:14-17, :321, :477, :648`; `local-import/lib.rs:337-339, :529, :583`; `handle_local.rs:72`; `family_merge.rs:424`; `runner.rs:213-228`; `https_auth.rs:28, :190`; `owner.rs:77, :163, :253-276`; `https_worker.rs:377`; `file_worker.rs:91-98`; `context.rs:80`; `check_process_globals.py:103-104, :540-541, :810-811`.
- **libgit2 and git2-rs.** libgit2 `remote.c:1352, :1391, :1394, :1409` (prune at :1420-1430 is reproducible through git2-rs `Remote::prune`, :426); `checkout.c:1886-1891, :2686`; `reset.c:154, :159, :167`. git2-rs `remote.rs:248, :265, :341`; `repo.rs:782-787`; `stash.rs:117`; `index.rs:49-51`.
- **gwz-py and CPython.** `bridge.py:28-35, :298-316`; `client_host.rs:164-171, :226-231`; `shims.rs:79-80`; `operations.rs:55, :194-227, :202-208, :216-222`; `cli.py:142-156`; CPython 3.11 `runners.py:150-157`.
- **Documents.** `GwzConnectionApiContract.md:269`; MachineOutput :67-69, :280-287, :669-670, :954-964; `ErrorCatalog.md:35`; gwz-py design §2.5 :103-106; gwz-py `README.md:225`.
- **"No handler today".** Confirmed by grep across gwz-cli, gwz-core, gwz-transport, gwz-py (Rust and Python) and gwz-git.

Discrepancies are those listed under P3-1 and P3-9.

## SURFACE (what A freezes)

- **Exit status.** 130 for SIGINT, SIGTERM and SIGHUP; Windows CTRL_C, CTRL_BREAK and CTRL_CLOSE; for every command except `hook`, `claude-code-setup` and the server lifecycle commands; also for the gwz-py CLI on SIGINT.
- **JSON entry (`--json`).** `{"code":"Cancelled","message":"interrupted",…all-null fields}` as the first own entry of `errors`. `aggregate_status` is unchanged, so `Ok` is possible with exit 130. A top-level error carries two entries (conflicts with MachineOutput :67-69, P3-1).
- **Forced envelope (`--json`).** The top-level shape with `meta: null` and message `interrupted; forced exit before the command finished`.
- **`--jsonl` forced.** No final response object (P3-1, P3-6).
- **stderr lines (writer).**
  - `gwz: interrupted; cancelling (interrupt again to stop now)`
  - `gwz: forced exit; waiting for local writes to reach a safe point`
  - `gwz: stopped between files: <member path> (<phase>)`
  - `gwz: transfer ended: <member path> (fetch|push|clone)[; the remote may have received it]`
  - Mode coverage is inconsistent (P3-8).
- **`forall`.** `interrupted: <k> of <m> members not run`.
- **Undefined.** The new text of the `pending_local_work` line, which today reads `gwz-alpha: transport cleanup has N pending local jobs`.
- **gwz-py CLI.** `GwzBridgeError("interrupted", code="Cancelled")` rendered as a top-level error with exit 130; member results are lost (parity gap with gwz-cli, recorded only).

## 3. Risks and next action

- **Highest risk:** P2-1. The design's carriage of the cancel and the gate is aimed at the per-operation entry architecture that Phase 3 replaces.
- **Ordering problem:** P2-2. The one-source rule (OD-C1) and Phase 2's server lifecycle cannot both hold under the current step order.
- **Behavioural problem:** P2-3. Force leaves stale ref locks that OQ2 forbids and that R1 mislabels.
- **Next action:**
  1. Revision 2 re-anchors §3.2/§3.4 on the 3.1 host API.
  2. It moves or splits the interrupt module so it can precede 2.3i/2.3k, or amends 2.3i explicitly.
  3. It adds the "no held Git lock" safe-point invariant.
  4. It closes the P3 table, inventory and citation gaps.
  5. Re-review on this axis against the new blob.

## 4. Out-of-scope observations

- D-V (:1009) still names `GwzCancellationAmendment-DRAFT.md` at the root, though the file now lives in `history/`. This is a convergence-plan reference; A works around it with the root pin.
- `crates/copy-contract/src/lib.rs:36` places `#[cfg(any(test, feature = "contract-tests"))]` directly on `pub mod contract_tests;`. Existing code; under the standing conditional-compilation rule this is a recorded migration item, not A's.
- `pager.rs:156` uses `if cfg!(windows)`, the macro form, which the rule does not cover.
