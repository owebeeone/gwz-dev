# Cancellation design A r3: Consistency-axis review (round 3)

**Review object:** `/Volumes/projects/limbo/gwz-dev-docs-cancel/dev-docs/GwzCancelInterruptDesign-DRAFT.md`, revision 3. This is the working tree of the lane at root `6d20ef6f`.
- Hash at start: `3760dd61c9af6442945977f19b3bd1abf8eec5c3`.
- Hash at end: `3760dd61c9af6442945977f19b3bd1abf8eec5c3` (unchanged).

**Date:** 2026-11-03

**Axis:** Consistency. This review checks:
- internal contradictions;
- agreement with D-V (every bullet, :999-1069), every cited document and every cited code line;
- the superseded-clause table (§8), and whether every row can actually pass;
- placement against convergence plan §4 and the 2.3 plan;
- hand-offs to B, C and D.

**Verdict: NO-GO.** Counts: 0 P0, 0 P1, 2 P2, 8 P3.
- No new finding is architectural. The architectural count stays at 2.
- Every round-2 finding ID of both reviews (13 Consistency, 15 Safety) is closed on this axis (§1).
- **Pre-commit:** I will give GO on a revision that resolves P2-1 and P2-2 as their corrections state, provided it adds no new P0-P2 on this axis.
  - Both corrections are scoping or return-value text on things that already exist. Neither needs a new thread, process, handler or protocol, so neither would stop the design under the cap.
  - The P3 items should close in the same pass but do not block.

## 0. Evidence base

**Pins**
- Lane root HEAD is `6d20ef6f`. The plan's diff against `48feeaad` is only the appended :1063-1069 and the "Open:" line (verified with `git diff`).
- Member heads match the citation line: gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`, libgit2 `b172e3d18`.
- gwz-core main is now `7a03d10e`, and gwz-transport main is `d8e34b3`. Both moved past the pins (Phase 1 R2b). Every gwz-core and gwz-transport cite was read at its pin (`b745f1fa`, `281a28d`) with `git show`.
- CPython 3.10.15 and 3.11.16 were read from `~/.asdf/installs/python/`.

**Authority**
- Plan §10 D-V, :999-1069, every bullet: OQ2 reversed; OD-C1 to OD-C6 (:1030-1031); review-cap resolutions (1)-(3) (:1035-1045); the split (:1046-1054); round-1 decisions (i)-(iii) (:1055-1062); round-2 O4 (a) and O5 (:1063-1069).
- OD-C1 to OD-C6 wording at `9d5a4d4c` §6.
- OD-C7 (a)-(e) at `history/GwzCancellationAmendment-DRAFT.md:694-702`.
- Revision 2's own O4 and O5 text at `6d20ef6f` :382-389, to check what the operator was shown.

**Placement**
- Plan §4 graph and ownership (:315, :322-331); Phase 3 (:712-745).
- 2.3 plan 2.3i, 2.3j, 2.3k, 2.3l, §4, totals (:181-250), and :71.

**Prior round**
- Both round-2 reviews.
- `-RemPlan-2.md`, including fix-pass GAP-1 to GAP-7.

**Hand-offs**
- B is `GwzChildLifetimeDesign-DRAFT.md`, revision 3, on disk, blob `0cbb9081`. I read §0, §1, §3.1, §3.2, §3.6-§3.8, §4, §7 (R-B4, R-B13), OD-B1, OD-B2 and §8, for the hand-off only.
- C and D do not exist yet.

**Build and gate facts**
- Root `MODULE.bazel`: the `@crates` hand-pinned hub.
- `gwz-cli/BUILD.bazel` deps.
- gwz-core `scripts/checks/{check_bazel_pin_drift,check_crate_versions,check_local_clone_boundaries}.py`.
- `process_globals_allowlist.json` roots.

**Method**
- Every `file:line` in §2, §3, §5, §6 and §8 was opened with sed or `git show`.
- I grepped the five repositories for handler APIs; there are none.
- Nothing was built, run or modified, and no git mutation was made.

## 1. Prior-finding closure table

| ID | Round-2 finding | Status | What I checked in r3 |
|---|---|---|---|
| C-P2-1 | Write gate candidate-only | **Closed** | §3.4 "Carriage, both builds": `run` owns the `WriteGate`. There is an unconditional `Git2Backend::with_write_gate` beside `spawn_gate` (`backend.rs:26`). The ordinary dispatch backend is `dispatch.rs:91-96`, and a candidate `ProcessGate` copy goes through `with_process_gate`. The other `Git2Backend::new()` sites are `log_exec.rs:118` (read-only) and `hook/family.rs` (not installed). A9 and row 6.9 say "both builds". |
| C-P2-2 | Server lifecycle commands got Force/`_exit` | **Closed** | §3.1 "Server lifecycle mode" (no slot, bound, gate, claim or forced exit); C exits 130. §8 row 6's 2.3k red row keeps the socket/record removal after a second SIGINT, and row 6.1 covers it. The `stop`/`status`/`list` gap is new: P2-1. |
| C-P3-1 | 2.3s installs before a server command exists | **Closed** | 2.3s calls neither function from `run` (§3.1 Install). 2.3k installs (§8 row 6). 2.3s edits only the `mod` line. |
| C-P3-2 | Ordering, allowlist, totals, 3.0d Owns | **Closed for the named items** | 2.3r-1 and 2.3r-2 come after 2.3s, matching B §4 and B §8 row 1. gwz-core's allowlist has no `signal`/`sigaction` entry at the pin (checked). There is one absolute totals sentence (5,580 + 6,120 across 15 verified arithmetically), and B §8 row 4 now defers to it. 3.0d owns `Cargo.lock` and `BUILD.bazel`. Further Owns gaps are new: P3-3. |
| C-P3-3 | Graph lacked `3.0f → 3.1c` | **Closed** | §8 row 1 and §5 3.1c "After" both carry it. |
| C-P3-4 | Direct-output check scope | **Closed** | 3.1c owns every flagged file, 2.3k's included. Test sources are excluded. Row 6.2 passes `src/tests/`. The stdio-channel question is new: P3-6. |
| C-P3-5 | "First threads" against B | **Closed** | §3.1 now says "first threads after the registry's", and C creates any registry first. This matches B §3.1 "creation precedes credentials" and B §4 3.1. |
| C-P3-6 | O4/O5 consistency | **Closed as asked** | O4 (a) and O5 are applied. A3 and R8 name a further interrupt and R9. R15's Windows cell is "beyond (OD-C1)". A3's list is still incomplete (P3-1), and the narrowing is unlabelled (P3-2). |
| C-P3-7 | "Beyond" definition | **Closed** | The column is now "Covered by", naming clauses. R6 states the re-materialize block (`transport.rs:32`). One cell is misclassified: P3-7. |
| C-P3-8 | Windows legs | **Closed** | Rows 6.1, 6.11, 6.12 and 6.15 mark the M and L legs. W uses CTRL_BREAK and CREATE_NEW_PROCESS_GROUP. |
| C-P3-9 | Cites and provenance | **Closed** | `https_adapter/worker.rs:268` (`dispose_all`) and `:340` (`pending_cleanup_count`), `local-import/src/lib.rs:508-509` and `operations.rs:28` were all verified. §0 now has the sentence on the "cancellable entry". |
| C-P3-10 | `gwz_response` and row 8 | **Closed** | §8 rows 15 and 16 name it. Row 8 adds "before the response was claimed" and excludes `hook` and `claude-code-setup`. The new over-claims are P2-1 and P3-2. |
| C-P3-11 | `diff`/`log` claim; Force without Cancel | **Closed** | §3.6 says the claim is taken at the first stdout write. §3.1 says "until Cancel or Force", and row 6.3 matches. |
| S-P2-1 | Resume after `wait()` | **Closed** | G4 is in place. `unpark` on a closed gate stops the thread, in both the table and G4. The fetch, clone, push and join callbacks are covered. R6 covers the cut removal, R7 the join residue (`apply.rs:178-183`, `handle_init_from_sources.rs:227-238` verified), and rows 6.4 and 6.6 test it. |
| S-P2-2 | Interrupt before the host exists | **Closed** | The slot has an `Interrupted` state, checked before the host and before the frame (§3.2). §8 row 4 carries it into plan 3.1, and row 6.11 tests it. |
| S-P2-3 | Capped pre-registration record | **Closed** | The record holds one entry per awaited call, plus `forget_cancel`. The record lives in `ClientHost` methods beside `cancel_operation` (`client_host.rs:164`). `register` is at `operations.rs:156` and `shims::operation_id` at `shims.rs:79-81`. Row 6.16 tests it. |
| S-P2-4 | gwz-py later SIGINTs | **Closed (Record)** | R11 gives the counts: 3 on 3.10; 4 on 3.11 (`runners.py:129, :71`; `thread.py:31, :37` verified). It is marked "beyond", and row 6.16 records the counts. |
| S-P2-5 | Pager fed while blocked | **Closed** | §3.7 spans the pager from spawn to exit (`pager.rs:107-115, :123-128, :138-145` verified). Row 6.15 has the 1 MiB leg. |
| S-P3-1 | Clone checkout residue | **Closed** | §3.5 row 2 and R6 match `clone.c:394-407`. |
| S-P3-2 | Slot atomicity | **Closed** | §3.2: one mutex, and a non-blocking `close()` (`owner.rs:21-32` verified). |
| S-P3-3 | Claim loser | **Closed** | §3.6 says the loser blocks until Force exits. Row 6.13 tests 100 runs. |
| S-P3-4 | LOGOFF/SHUTDOWN under `user32` | **Closed (Record)** | §3.1 and R9 carry the **(OS)** fact, and the `Cargo.toml` feature is cited. Its "Covered by" label is P3-2. |
| S-P3-5 | G1/R10 writing-phase I/O | **Closed** | G1 lists the pack trailer, the clone before its first object, and the push disconnect (`remote.c:3070` verified). R10 matches. |
| S-P3-6 | Panic, `dbg!`, unwind, reader death | **Closed** | `dbg!` is in the check, guards are restored on unwind, the reader runs under `catch_unwind`, and the panic case is in R8. Rows 6.1, 6.2 and 6.4 test these. The R8 label is P3-7. |
| S-P3-7 | Orphan after R14 | **Closed** | R14 names the orphaned pager or `forall` program. |
| S-P3-8 | Notice invites a tearing interrupt | **Closed** | The Force notice carries the warning, and §8 row 8's paragraph includes the notices. |
| S-P3-9 | A4 in the ordinary build | **Closed** | A4 and R17 point to B's R-B13, which reads the same in B r3. |
| S-P3-10 | Nested-frame refusal | **Closed** | Only outermost frames are refused. G3 refuses parks and `outside`. §3.5's last row, R18, and rows 6.4 and 6.6 cover it. |

## 2. New findings

### P2-1: `gwz server stop`, `status` and `list` have no interrupt handling, but §3.6 and §8 row 8 freeze 130 for them

**Architectural:** no. A scoping clause fixes it.

**Root cause.** Nothing installs the module for these three commands, yet the design promises 130 for them:
- 2.3k puts the whole `gwz server` family in one run-own-lifecycle branch "like `hook`" (2.3 plan :210). That family is `start`, `stop [--force]`, `status`, `list` and `stdio` (:211-213).
- A's install covers only part of the family:
  - §8 row 6 makes 2.3k call `install` "for `server start`, its background copy and `server stdio`".
  - §3.1 makes 3.1c install "for every command but `hook`, `claude-code-setup` (R12) and that branch".
- So `stop`, `status` and `list` get no handler and keep the OS default.
- §3.6 "Status" lists only `hook` and `claude-code-setup` as exceptions. §8 row 8 freezes "130, whatever the aggregate … or a `server` lifecycle command stopped by one; not `hook` or `claude-code-setup`".
- §4's C hand-off lists only "the server lifecycle mode", and no §7 row names the gap.

The other reading of "that branch" (the three lifecycle commands only) is no better. Under it, `stop`, `status` and `list` get 3.1c's full mode outside dispatch. That gives them a slot nobody checks, the bound, and Force's forced `--json` envelope written after their own `--json` record. There is no claim rule for them, which is round 2's C-P3-11 again.

**Location:** §3.1 "Install" and "Server lifecycle mode"; §3.6 "Status"; §4 C; §7 (R12); §8 rows 6 and 8.

**Violated invariant:**
- D-V O4 (a) (:1065-1066): every interrupt source "take[s] the interrupt path and exit[s] 130".
- §9: "none narrowed".
- A frozen surface row must match the mechanism.

**Reproduction** (after 2.3k and 3.1c):
1. Run `gwz --json server stop` against a server whose detached wait is in progress (2.3i's "one more bound").
2. Send SIGTERM, or Ctrl-Break on Windows.
3. gwz dies by the signal: 143, or `STATUS_CONTROL_C_EXIT`. There is no `--json` record. MachineOutput's new row says 130.

**Impact:**
- An unrecorded narrowing of an operator decision.
- A public exit-code row that is false for three commands.
- Consumers of the `--json` records get no output.

**Correction.** Choose one:
- (a) 2.3k calls `install` in lifecycle mode for the whole `server` family, with C delivering 130 (§8 row 6, §4 C).
- (b) Add `stop`, `status` and `list` to R12 as "beyond", and exclude them in §3.6 and in §8 row 8.

**Closure test:**
- §3.1, §3.6, §4 C, §7 and §8 rows 6 and 8 name one owner and one exit behaviour for each `server` subcommand.
- §8 row 6's 2.3k red rows (or R12) cover `stop` interrupted under each O4 (a) source.

### P2-2: moving `diff` and `log` onto `Output`, whose write errors are ignored, removes their stop on a closed consumer (EPIPE)

**Architectural:** no. `Output` already exists; only a return value is added.

**Root cause.** The writer swallows the error the producers rely on, and nothing hands it back:
- §3.1 "Writer" says that once installed, one module thread owns stdout, and "write errors are ignored".
- 3.1c "moves every direct write onto `Output`", including `pager.rs:130, :146` (`PatchSink::Stdout`) and `log_exec.rs:33, :36`, which are in 3.1c's file list.
- Those sites rely on the write error:
  - `pager.rs:11-15` documents "a closed pipe surfaces as `SinkError::BrokenPipe` … the pager-quit → ProducerStop path".
  - `diff_exec.rs:271` maps `BrokenPipe` to `StopReason::ConsumerGone`.
  - `log_exec.rs:204` and `:286` return early with exit 0 on `BrokenPipe`.
- Once the writer swallows EPIPE, neither command learns the consumer is gone. The design states no replacement signal.

**Location:** §3.1 "Writer"; §5 3.1c; §6 has no row for it.

**Violated invariant:**
- Agreement with the cited code's contract (pager-quit/ProducerStop).
- A9 ("one behaviour … unless §3 says"), since this is a behaviour change §3 does not declare.

**Reproduction** (after 3.1c):
1. Run `gwz log | head -1` on a long history, or `gwz diff --no-pager | head -1` on a large workspace diff.
2. Today both stop at the first write after `head` exits.
3. After 3.1c both walk and render the whole history or diff into a writer that discards it, then exit 0.

**Impact:**
- A compatibility and performance regression on a common shell idiom: time and CPU proportional to the full output.
- The documented ProducerStop path becomes dead code.

**Correction.**
- The writer records the first stdout write error.
- `Output`'s send returns `ConsumerGone` from then on (never blocking), and `diff` and `log` map it to their existing `BrokenPipe` paths.
- §3.1 states this.
- The notices on stderr are unaffected.

**Closure test:** a 3.1c row is red against "errors ignored":
- `gwz log | head -1` on a 50,000-commit fixture and `gwz diff --no-pager | head -1` on a 10,000-file change each exit 0 within 2 s.
- The core spool stops (`ConsumerGone` is observed).

### P3-1: A3's "only … ends a write in progress" list contradicts §7

- **Location:** A3; §7 R11, R12, R14, R15.
- **Problem.** A3 lists only SIGKILL, a further interrupt after Force, and Windows' end after close, logoff or shutdown. The table it summarizes says more:
  - R14 names SIGQUIT "at any time" (OD-C7 (b)), which A3 omits.
  - R11 and R15 tear gwz-py CLI writes.
  - R12 leaves `hook` and `claude-code-setup` to the OS default.
  - D-V resolution (1) names a crash and power loss.
- **Correction.**
  - Scope A3 to installed gwz-cli commands and add SIGQUIT and a crash/power loss.
  - Or state the list as "the §7 rows R8, R9, R11, R12, R14, R15".

### P3-2: Windows end states and the O4 (a) narrowing are labelled as "covered"

- **Location:** §8 row 8; R9 "Covered by"; R15; §9.
- **Problems:**
  - §8 row 8 freezes 130 for "console close, logoff, shutdown". Yet R9 and §3.1 say:
    - LOGOFF and SHUTDOWN never reach a candidate-build process (`user32`, **(OS)**);
    - after CTRL_CLOSE the OS ends gwz unless Force finishes first.
  - R9's "Covered by … O4 (a)" is wrong for LOGOFF and SHUTDOWN: O4 (a) orders the interrupt path, and these events never reach it.
  - Revision 2's O4 told the operator it "changes … R15's O4 cells" (rev 2 :386), and R15's cell was "the Windows events with O4". Revision 3 makes those events "beyond (OD-C1)" while §9 says "none narrowed".
- **Correction.**
  - Qualify row 8 with "when gwz exits by itself".
  - R9: LOGOFF and SHUTDOWN under `user32` become "beyond (OS)".
  - §9 names R15's Windows events and R9's LOGOFF/SHUTDOWN as narrowings of O4 (a) that D puts to the operator.

### P3-3: Owns lists miss gate-enforced build files

- **3.0d (`crates/write-gate/`).** A new crate under `crates/` also needs:
  - `scripts/checks/local_clone_inventory.json`: LBT-001, "every package directory under `crates/` is classified" (`check_local_clone_boundaries.py:17`);
  - `check_crate_versions.py` `EXPECTED_CRATES` (:85; "one more … is a publish-order and release-script change"), to be bumped after B's 2.3r-1 bump;
  - the release publish order, since gwz-core depends on the crate normally.

  B's 2.3r-1 lists the first two for `crates/children`. A omits them.
- **2.3s (gwz-cli adds `libc`, `windows-sys`).**
  - gwz-cli's crates.io deps are hand-pinned in root `MODULE.bazel` (the `@crates` `from_specs` hub, :14-17, :34-45) and in the `gwz-cli/BUILD.bazel` deps (:34-39).
  - They track root `Cargo.lock`, and the pins are guarded by `check_bazel_pin_drift.py`.
  - 2.3s's Owns names none of these, so 2.3s is a three-repository landing (gwz-core, gwz-cli and the root), not two.
  - The gate reads only `[dependencies]` (:181), so a `[target.'cfg(…)'.dependencies]` entry escapes it, while the Bazel build fails.
- **Correction.**
  - Add these files to the 3.0d and 2.3s Owns.
  - State the root half and its landing order beside "gwz-core half merges first".

### P3-4: the main thread's writer drain is unbounded, and row 6.12's last leg contradicts the text

- **Location:** §3.4 Force step (6) ("else Force waits for the main thread's exit", unbounded); §3.6 ("up to the close bound plus 1 s"); row 6.12.
- **Problem.** Row 6.12's leg is `--jsonl pull`, consumer stalled, then SIGTERM.
  1. After Cancel the dispatch ends and takes the claim.
  2. Its final document blocks in the writer.
  3. Force starts at the bound (5 s) and waits a further close bound + 1 s (6 s).
  4. So the exit can come at about 11 s.

  The row requires "within the bound plus 1 s".
- **Correction.** Choose one:
  - the main thread waits at most 1 s for the writer after its final document, as step 8 does, with step 6's text made bounded to match;
  - or the row says "bound + close bound + 1 s".

### P3-5: row 6.9 [3.0f] asserts 3.1c's notice

- **Problem.** "The notice names member and phase" needs 3.1c's writer and Force. 3.0f lands before 3.1c (graph), so this leg cannot be seen red and then green in 3.0f.
- **Correction.**
  - Assert the label `wait()` returns in 3.0f.
  - Move the notice leg to a 3.1c row.

### P3-6: lifecycle-mode output and `server stdio`'s stdout channel are unspecified

- **Location:** §3.1 "Writer", "Server lifecycle mode", "Check"; §4 C.
- **Problems:**
  - `install` starts the writer, which "owns stdout and stderr". Lifecycle mode does not say whether the writer runs or whether §3.6's notices ("every output mode") are written.
  - `server stdio`'s stdout is its protocol channel (2.3j `server/stdio.rs`).
  - If 2.3k's gwz-cli code passes `io::stdout()` to it, `check_direct_output.py` flags the call, and 3.1c must "move it onto `Output`". `Output` is line-oriented and drops event lines after Cancel, so the protocol stream cannot go through it.
- **Correction.**
  - State the writer's and notices' behaviour in lifecycle mode.
  - Either add the stdio hand-off site to the check's exceptions or put it in gwz-core.
  - List this in §4 C.

### P3-7: R8's panic on a stalled stderr is classed "(i)"

- **Problem.** Decision (i) orders waiting for local writes. A thread blocked writing a panic message to a stalled stderr is not a local write. The unbounded wait it causes is A's choice (RemPlan-2 replaced a panic hook with it).
- **Correction.** Mark that case "beyond" in R8's "Covered by" and add it to §9's "Beyond" list.

### P3-8: text drift

- **§4 point (8).** It says "B's row should say so", but B r3 §8 row 4 already does ("A r3 §8 row 7's totals sentence … supersedes this clause in either order"). The RemPlan's "Unresolved" item is closed.
- **Path convention.** §0 says bare `src/…` and `scripts/…` are gwz-core's. Yet §5 3.1c's Owns (`src/lib.rs`, `scripts/{check_direct_output,…}.py`) and §3.1 "Writer" (`src/interrupt/`, `src/hook/`) are gwz-cli's, while row 2.3s prefixes `gwz-cli/`.
- **2.3 plan :71.** Its "about 4,300 implementation lines" is not superseded beside :249-250.
- **§8 row 7.** It gives no §3 anchor for the 2.3s paragraph. B anchors its 2.3r paragraphs "after 2.3l (:220-230)".

## 3. Verified citations

All were opened at the pins in §0. Each matches unless a finding above says otherwise.

**D-V and plans**
- Plan :315, :322-331 (trunk :325; Phase 5 :329-331), :712, :717-720, :721-728, :736-738.
- D-V :999-1069 (OQ2 :1005-1010; :1030-1031; :1035-1045; :1046-1054, with "cancellable entry" at :1050; :1055-1062; :1063-1069).
- 2.3 plan :71, :181-188 (the quoted 2.3i phrases match exactly at :186 and :188), :194-205, :207-218, :240-242, :249-250.
- OD-C1 to OD-C6 at `9d5a4d4c`; OD-C7 at history :694-702.

**gwz-cli**
- `lib.rs:161-178` (`worker_host::early`, `--build-info` :167, help :174), :192, :226-285 (`diff`, `hook`, `claude-code-setup`, `log` outside dispatch), :303-344.
- `dispatch.rs:57-89, :82-87, :91-96`; `render_exit.rs:82-95`.
- `pager.rs:11-15, :107-115, :123-128, :130, :138-145`; `diff_exec.rs:271, :349`; `log_exec.rs:118, :204, :286`; `forall.rs:260`.
- Other `Git2Backend::new()` sites: `log_exec.rs:118`, `hook/family.rs:63, :149, :187`.
- `.github/workflows/ci.yml:26-31`; `scripts/process_globals_allowlist.json`.

**gwz-core (`b745f1fa`)**
- Build and composition: `lib.rs:47-51`; `composition.rs:21-35`; `backend.rs:26, :75, :118-125`; `contract.rs:42`.
- Fan-out: `par_map_per_host.rs:35-60`, with all 7 callers in 6 files at the cited lines.
- Transport: `transport.rs:32, :41, :55-59, :73, :368, :718, :740`; `transport_support.rs:89-92, :130`; `repository_support.rs:30-35`; `repository.rs:31, :56, :81, :112`.
- Merge transactions: `merge_prepared.rs:248-249, :266, :280, :339`; `merge_recovery.rs:143-144, :175, :197-198, :251`.
- Member operations: `handle_commit.rs:179-186`; `apply.rs:176-184`; `handle_init_from_sources.rs:225-239`.
- Copies and locks: `handle_local.rs:72`; `family_merge.rs:424`; `local_clone/wait.rs:3-6` (a poll exists, :11ff); `session_host/served.rs:81`; `local-import/src/lib.rs:508-509`.
- Bounds and HTTPS: `context.rs:80`; `https_auth.rs:28, :190`; `owner.rs:253-258, :296-299`; `https_worker.rs:377`; `runner.rs:226`; `file_worker.rs:91-98`; `https_adapter/worker.rs:268, :340`.
- Checks: `check_process_globals.py:103-104, :540-541`; allowlist roots `src/lib.rs`, `crates/*/src/lib.rs`, with no `signal` or `sigaction` entry; `run_tests.py:316-327, :472-479, :552`.

**gwz-transport (`281a28d`)**
- `host.rs:6-10`; `host/owner.rs:21-32, :50-59`; `host/config.rs:22, :36`; `operation.rs:343-344, :414-417` (`Closing(TerminalReason)`).
- Contract :269, :272-275 (root `dev-docs/`).

**libgit2**
- `remote.c:3070`; `clone.c:394-407`; `checkout.c:1886-1891`; `fetch.c:218-223`.
- The rest were verified by both round-2 reviews and are unchanged.

**gwz-py and CPython**
- `bridge.py:28-35, :298-316, :307, :318`; `cli.py:76, :142-156`; `client_host.rs:164, :226-231`; `shims.rs:79-81`; `operations.rs:28, :156, :197-209`; `test_process_globals.py:11-23`.
- Per-operation design §2.5 :100-106; README :225.
- 3.11 `runners.py:71, :117-129, :150-157`; 3.10 `runners.py:44-63`; 3.11 `concurrent/futures/thread.py:31, :37`.

**Documents**
- MachineOutput (gwz-cli) :66-69, :142-143, :280-287, :669-673, :954-964 (codes are PascalCase, so `"Cancelled"` is consistent).
- `ErrorCatalog.md:35` (the quoted "A result's first error;" matches).
- `GwzCoreSessionDesign.md:687`; f9 report :285 and :296 ("Findings outside scope", item 2).

**B r3 hand-off points (1)-(8)**
- (1)-(7) match B §3.1, §3.7, §4, §8 rows 1 and 4, R-B4 and R-B13.
- B cites A revision 2 :179-180 (step 4) and :183-184 ("A calls no `close()`"), verified at `6d20ef6f`.
- (8) is now satisfied (P3-8).

### SURFACE (what A freezes)

**Exit status**
- 130 when an interrupt preceded the claim, from SIGINT, SIGTERM or SIGHUP, or Windows CTRL_C, CTRL_BREAK, CTRL_CLOSE, LOGOFF or SHUTDOWN. This overrides 0, 1 and 2 and a death by signal.
- Exceptions:
  - `hook` and `claude-code-setup` (R12);
  - Windows LOGOFF and SHUTDOWN in the candidate build, and CTRL_CLOSE past the OS timeout (P3-2);
  - `server stop`, `status` and `list` (P2-1).
- Server lifecycle commands exit 130, delivered by C.
- The gwz-py CLI exits 130 on SIGINT, and dies by default on SIGTERM, SIGHUP, CTRL_BREAK and CTRL_CLOSE.

**JSON**
- The first own `errors` entry is `{"code":"Cancelled","message":"interrupted",…nulls}`, placed after the member copies.
- `aggregate_status` is unchanged, so `Ok` with exit 130 is possible.
- A top-level error yields two entries.

**Forced envelope**
- `meta: null`, no members, message `interrupted; forced exit, see stderr for what stopped`.
- `--jsonl` gets no final response, and human or porcelain stdout gets nothing more.

**stderr, every output mode**
- The Cancel notice and the Force notice, including the further-interrupt warning.
- `gwz: stopped: <member path> (<phase>)`
- `gwz: transfer ended: <member path> (fetch|push|clone)[; the remote may have received it]`
- `gwz: <n> progress lines dropped`
- `gwz: cleanup still pending`, which replaces the `gwz-alpha` line.

**Other**
- `forall`: `interrupted: <k> of <m> members not run`.
- Pipe behaviour: early stop on a closed consumer is lost for `diff` and `log` unless P2-2 is fixed.
- gwz-py: `CancelledError.gwz_response`. Cancelling the awaiting task cancels a transport-route operation first.
- ErrorCatalog `cancelled` gains its interrupt sentence.

## 4. Risks and next action

**Risks**
- **Highest: P2-1.** An operator decision (O4 (a)) and a frozen exit-code row are silently narrowed for three 2.3k commands that fall between A's two install sites and C's lifecycle mode.
- **Second: P2-2.** Routing `diff` and `log` through a writer that ignores errors removes the EPIPE stop that both cited sites document.
- **Pins behind main.** gwz-core main (`7a03d10e`) and gwz-transport main (`d8e34b3`) have moved past the pins.
  - `https_adapter/worker.rs`'s `pending_cleanup_count` moved from :340 to :343.
  - gwz-core `7f571a18` makes the runner's tests accept `CleanupPending` "as the product's reviewed contract". Its `assert_ended_by` accepts either cause, so B's and 3.0e's change stays compatible, but the step briefs should re-pin.
- **Line budget.** Revision 3 is at 397 of 400 lines. Each correction above should replace text rather than add it.
- **Not yet built.** The 3.1 `HostOwner`, 2.3k, and C's acceptance of the lifecycle mode do not exist yet. P2-1's fix needs C's agreement.

**Next action**
1. Revision 4 resolves P2-1 and P2-2 as corrected, and closes P3-1 to P3-8 in the same pass.
2. The lane owner confirms B r3 needs no further edit for point (8).
3. A closure check on this axis against the new blob follows. Under the pre-commit above, the exit is GO with no new P0-P2.
