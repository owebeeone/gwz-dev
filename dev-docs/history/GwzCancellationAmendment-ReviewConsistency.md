# GwzCancellationAmendment-DRAFT r1: Consistency-axis review

**Review object:** dev-docs/GwzCancellationAmendment-DRAFT.md, blob 3448c561 (root 9d5a4d4c), revision 1, DRAFT

**Baseline:**
- Repositories as read: root 9d5a4d4c; gwz-core b745f1fa (the document's code citations are stated at gwz-core 07e99924 and were checked there, with drift to b745f1fa noted); gwz-cli c0ba717; gwz-py 2185cc6; gwz-transport 281a28d; libgit2 b172e3d18; gwz-sspi 6f83551.
- The pin was checked at the start and again at the end. `git rev-parse 9d5a4d4c:dev-docs/GwzCancellationAmendment-DRAFT.md` and `git hash-object` on the working file both gave 3448c561a37f60149b977290da8a62d1f34481f2 both times.
- How sources were read: `git show <sha>:<path>` with line-numbered extracts, `grep`, and the cited documents at root 9d5a4d4c and at a519ee58, the document's citation root. Installed `gwz 1.0.18` was read cold with `gwz --help`, `gwz help forall`, `gwz help fetch` and `gwz help __reaper`, and `gwz __reaper` was run once.
- Nothing was written, built or mutated.

**Date:** 2026-11-01

**Axis:** Consistency. The document is checked against its controlling graph: D-V, TR1.6, the split document, the session design, the parked server design, contract §5, the convergence plan, the 2.3 plan, and the code it cites. The review is independent, adversarial and read-only. The other axis runs in parallel and nothing here relies on it. The lane owner files this report verbatim.

**Verdict: NO-GO.** P0: 0, P1: 0, P2: 10, P3: 9. I pre-commit to GO on a revision that resolves P2-1 to P2-10 as specified, with one condition. P2-4 and P2-8 each need one of the options stated in them, and each choice must be recorded as the operator's where it changes an accepted contract.

---

## 0. Evidence base

**The amendment, all 265 lines, read in full.** The r0→r1 diff (03dcb4e0..9d5a4d4c) changes only the status line and the §6 heading.

**D-V and the convergence plan.**
- D-V is at `GwzTransportReleaseConvergencePlan.md:999-1031`.
- The plan's §4 step graph is at :300-331 and Phase 3 at :712-742.

**The 2.3 plan, `GwzServerCore-Step2.3-Plan.md`.**
- Sub-steps 2.3g to 2.3l are at :154-231.
- Q2 is at :329-333.

**TR1.6.** `gwz-core/dev-docs/history/reference/GwzTransportCredentialHelpersDesign.md`: §3.1 to §3.3 at :62-91, and §12 OQ2 and OQ3 at :398-406.

**The split document, `GwzSplitCommandChannel.md`.**
- Read at 9d5a4d4c (400 lines) and at a519ee58 (393 lines).
- §3.3 is at :109-139, §4.3 at :232-242 at the root, and §4.4 at :244-249.

**The session design, `GwzCoreSessionDesign.md`.**
- Header: accepted, frozen.
- Sections read: §4.2 (:177-188), §5.3 (:257-270), §5.5 (:291-293), §8 (:424-442) and §16 (:681-692).

**The parked server design, `history/parked/GwzCoreServerDesign.md`.** :586-590, :670-673, :805-812, :1103-1107 and :1214-1224.

**Contract, `GwzConnectionApiContract.md`.** :184-194 (the five-second host close bound) and §5 :210-284.

**Process.** `AgentProcessRules.md` L1-08 (:268-279).

**Code.** Every §2 citation, listed in §2 below, plus these:
- gwz-cli: `globalargs/render_exit.rs:82-95`, `worker_host.rs` and `docs/MachineOutput.md:954-964`.
- gwz-core: `docs/ErrorCatalog.md:35`, `protocol/generated.rs:1133-1142` (`AggregateStatus`), `family-store/src/publish.rs:150-168`, `refcopy/src/ordinary.rs:615`, `verified_write.rs:64`, the Windows helper `launch.rs:224` and `check_process_globals.py`.
- gwz-py: `bridge.py:246-315`, `client.py:416-420`, `cli.py:142-156`, `client_host/operations.rs:205-290`, `worker_host.rs:1-30`, `dev-docs/GwzPyPerOperationTransportDesign.md` §2.5 and `README.md:225`.
- gwz-transport: `crates/gwz-connection/src/remote/core.rs:337-505`.

## Problem, requirement provenance, and scope

**What was asked for.** D-V reverses OQ2:
- Ctrl-C cancels end to end: the client, every helper it started, and a serving core's in-flight call.
- Each side detects that a peer has died and cleans up.
- Nothing is left behind: "no orphaned process, stale lock or partial state".

The deliverable is a design amendment only.

**Where each mechanism comes from.**

| Mechanism | Traces to | Assessment |
|---|---|---|
| Interrupt source (§3.1) | D-V "Ctrl-C cancels" | Necessary. |
| Cancellable entries and native abort (§3.2) | D-V "client … in-flight" | Necessary. |
| Safe points (§3.2) | D-V "no partial state" | Necessary, but they change §5.3 (P2-4). |
| Child registry and lifeline reaper (§3.4) | D-V "every helper"; "no orphaned process" | Necessary on macOS and Linux. The shape is incomplete (P2-7) and the scope over-reaches (P2-8). |
| Temp sweep (§3.5) | D-V "partial state" | Proportionate, but the inventory is incomplete (P3-2). |
| `ELOCKED` reporting (§3.5, OD-C3) | D-V "stale lock" | This narrows D-V. The narrowing is sanctioned by OD-C3 but not stated as a residual (P2-5). |
| Served-form rules (§3.6) | D-V "serving core's in-flight call" | Necessary. They silently change the split document's bound (P2-3). |
| Step placements (§4) | Plan rule (~500 lines, phases) | Inconsistent with the plan (P2-1). |
| Helper stdin kept open (§3.4) | C3 "a helper notices gwz" | Not effective (P3-3). |

**Widening.**
- §3.2 widens scope into gwz-core gitbackend native transfers. This is justified by C1, because ordinary builds otherwise cannot be cancelled.
- The safe points "between members" widen the session design's cancellation semantics (P2-4).

**Narrowing.**
- The forced path, which a second Ctrl-C or the 5 s bound reaches, can leave Git `*.lock` files and torn non-atomic writes.
- D-V says "no stale lock or partial state" unconditionally.
- The amendment's own requirement paragraph (:9-12) repeats D-V verbatim, while C4, C5 and §3.5 narrow it without naming it a residual (P2-5).

## 1. Findings

### P2-1: §4 places pieces against the plan's phase order and the 2.3 sub-step ownership (ARCHITECTURAL)

**Location.** §4 table (:207-217) and §3.6 (:196-203).

**Violated invariant.** Plan §4 (:302, :315-321) says:
- "Phase 3 … (after Phases 1 and 2)".
- "Nothing runs in parallel on both sides of an unsettled interface".
- Each step aims at under 500 implementation lines.

The 2.3 plan's ownership and Q2 also apply.

**The reproduction, from the text alone:**
1. 2.3j (Phase 2) must start the `stdio`/`ssh` child "through the registry (§3.4)" (§3.6, §4). The registry is built in new step 3.0c (Phase 3), which runs after all of Phase 2. 2.3j also owns `server/launch.rs` "(… pure)" (2.3 plan :194-197), meaning argument vectors and the environment, not a spawn. So "2.3j gains it" either adds a spawn to a pure module or needs a registry that does not exist yet.
2. 2.3k (Phase 2) gains the "client interrupt protocol over the carrier". Its interrupt source is 3.1c (Phase 3, "after 3.1"). Under 2.3 plan Q2(A) (:329-333), 2.3k carries no operation calls: "the C7 rows that need `gwz --server X fetch` move to 3.1". So `operation.cancel` for "the command's calls" has no consumer in 2.3k. 2.3k is also already ~500 + 400, and adding ~150 puts its implementation at ~650, over the ~500 rule.
3. The row "carrier loss cancels every live operation including local phases", which stays with 2.3g, needs the local handlers to observe the token. Today they "read no token yet" (`served/dispatch.rs:101-102`). The safe points that make the row pass arrive in 3.1c-2 (Phase 3). Row 8's "release its member lock" has the same dependency.
4. The Phase 3 graph `3.0 ∥ 3.0b ∥ 3.1 ∥ 3.2 → 3.3 → 3.4 → 3.5` is not amended:
   - 3.0c, 3.1c, 3.1c-2 and 3.1c-3 are not placed relative to 3.3 or 3.4.
   - 3.1c's "cancellable entry in dispatch" runs after 3.1 has already moved dispatch to `HostOwner`, and 3.3 removes `with_cancellable_local_transport*`.
   - §3.2 says gwz-cli moves to the cancellable entries "now", which contradicts 3.1c's "after 3.1".
5. "Native-transfer abort … step 3.1c or its own small step" leaves a gwz-core gitbackend change with no decided owner. 3.1c is a gwz-cli step.

**Impact.**
- Every Phase 2 served-form row that §5 depends on (row 8) cannot be red-then-green in its owning step.
- Lanes would edit across an unsettled interface.
- The convergence plan's step list and sizes are silently changed.

**Required correction.**
- Either move the served-form pieces to Phase 3 (a 3.1 or 3.1c sub-step that owns the call-carrying client's interrupt and the child spawn through the registry), or move the registry and interrupt source ahead of 2.3j and 2.3k, and state that order.
- Place every new step in the Phase 3 graph relative to 3.1, 3.2 and 3.3.
- Delete "now" or the dispatch item.
- Fix the native-abort owner.
- State that the amendment amends plan §4 and the 2.3 plan's 2.3g, 2.3j and 2.3k, with sizes within ~500.

**Closure test.** For each §4 row, its "After:" predecessor exists earlier in the plan's graph, and every §5 row names one owning step whose predecessors build everything the row needs.

### P2-2: OD-C1 ("gwz-core … install[s] none") contradicts 2.3i's gwz-core signal handler and the split document's server signal semantics

**Location.** OD-C1 (:241-244), §3.1 (:103-106), §3.6 (:191) and §7.

**Violated invariants.**
- The 2.3 plan, 2.3i (:181-187), owns gwz-core `server/{run,shutdown,log}.rs` and installs "SIGINT/SIGTERM and Windows Ctrl-Break through a handler that sets a flag only".
- Split document §4.3 (:239 at root) says SIGINT and SIGTERM on a server run the server design's §6 shutdown.
- L1-08: an amendment names the text it supersedes.

**Reproduction.**
- `gwz server start --foreground` runs in a gwz-cli process.
- By OD-C1, gwz-cli `main` installs the one handler and maps SIGINT to `Cancel` of "the command's operations" (§3.3).
- 2.3i, which lands earlier in Phase 2, installs a second handler in gwz-core for the same signals with shutdown semantics.
- §3.6 cites "the core's own `Force` is its shutdown's second signal (2.3i)" but does not say which handler owns the signal, or whether 2.3i's handler moves to gwz-cli.

**Impact.**
- Two process-global installers exist for the same signals, so the second installation silently replaces the first.
- gwz-core would carry an allow-list entry that OD-C1 forbids.
- 2.3i implements against a rule that the amendment reverses without saying so.

**Required correction.** State that 2.3i's handler moves to the gwz-cli `interrupt/` module and that the server command receives the interrupt source as a value, with Cancel/Force mapped onto the server shutdown's first and second signal. Alternatively, record a second sanctioned owner in OD-C1. Either way, list 2.3i as amended.

**Closure test.** A source check counts exactly one signal and console-handler installation site across gwz-cli, gwz-core and gwz-py. A `server start --foreground` SIGINT row shows the §6 shutdown order.

### P2-3: the served form's first-interrupt wait changes silently from the session close bound to 5 s

**Location.** C5 (:38-41) and §3.6 "Client interrupt" (:183-185).

**Violated invariant.** All of these use the session design's close bound (`close_wait`, 60 s default) for the client's wait:
- Split document §4.3 (:242 at root, cited as :235): "first Ctrl-C sends `operation.cancel` and waits up to the close bound".
- Parked server design :671: "waits up to the close bound for its reply".
- Session design :56 and :430 define that close bound.

C5 instead cites contract §5 (:253-284) as "the host's close bound, 5 s". The contract's five-second host close bound is at :189, which is §4.1, not in the cited range. That is a different bound with the same name.

**Reproduction.**
- A served `gwz commit` is between safe points.
- The client's first Ctrl-C sends `operation.cancel`.
- The core replies only "once the target is terminal" (session design :183), which a local handler may reach only within `close_wait`.
- The client's 5 s wait expires and the client force-closes the carrier. Every served cancel of local work therefore ends as carrier loss, and the cleanup report the split document promises is never received.

**Impact.**
- An unlisted amendment of an accepted surface rule.
- 2.3k and 3.1 implementers get contradictory bounds.
- The cleanup report in §3.6 is effectively unreachable for local work.

**Required correction.**
- Name the bound used in each form: in-process, the host close bound of 5 s (contract :189); served, either `close_wait` or 5 s.
- If 5 s, list split document §4.3 and parked :671 as amended, and state what the user sees when the core's reply arrives after the bound.
- Fix the citation.

**Closure test.** A served row with a local handler that needs 10 s to reach its safe point shows the specified bound and outcome.

### P2-4: safe points "between members" amend session design §5.3 without listing it, and introduce cross-member partial outcomes

**Location.** §3.2 "Local phases" (:123-127) and §7 (:262-263): "does not change … the session design's `operation.cancel`".

**Violated invariant.** Session design §5.3 (:266-267), accepted and frozen: "a local handler observes cancellation only at gate crossings. Once it holds what it needs, such as its member and workspace locks, it runs to its end and reports its own outcome." L1-08 also applies, along with D-V's "no … partial state".

**Reproduction.**
- `gwz commit` across 5 members takes the workspace lock and commits member 1.
- Ctrl-C arrives.
- At the "between members" safe point the handler stops. Members 1 and 2 are committed and members 3 to 5 are not.
- Under §5.3 it would have run to the end.

**Impact.**
- This changes an accepted contract's cancellation semantics.
- It creates exactly the multi-repository partial state D-V forbids.
- The reported outcome is unspecified: `Cancelled`, `Partial`, per-member rows, or the `cancelled` (73) error placed first.
- Neither the session design's header nor its amendment trail records the change.

**Required correction.** Choose and state one of two options:
- (a) Safe points are only before a handler takes its locks, and between members for read-only or idempotent per-member steps. A handler that holds its locks runs to its end, as §5.3 says.
- (b) Amend §5.3 explicitly. Define the outcome and aggregate status of a partially completed W operation. Add the amendment line to the session design's header.

Either way, relate "safe points" to §5.3's "gate crossings".

**Closure test.** A row cancels a multi-member W operation after member 1 and asserts the specified state and outcome. Under (a), all members are done or none are.

### P2-5: the forced path cannot deliver C4 or C5 as stated; the :687 "narrowing" and the residual against D-V are misdescribed

**Location.**
- C5 (:40-41): "the forced path … still leaves only state C4 allows".
- §3.3 step 2 (:134-137).
- The "What it amends" paragraph (:18-19): ":687 is narrowed by §3.5".
- The requirement paragraph (:9-12).

**Violated invariant.**
- D-V: Ctrl-C leaves "no … stale lock or partial state".
- Session design :687: "A local write it has in progress can be torn by exit".

**Reproduction.**
- A first Ctrl-C reaches `gwz pull` while libgit2 checks out a large tree, which is one phase between safe points.
- The 5 s bound passes, or the user presses Ctrl-C again (§3.3 step 2).
- gwz exits with "no destructor-dependent cleanup". The working tree is half updated, and `index.lock` may remain.
- §3.5 covers only atomic temp-and-rename writes, which never tear, and Git locks, which are reported rather than resolved.
- No mechanism or row makes the forced-path state "complete, never published, or recognized and resolved", and §5 row 3 checks only "no child left".

**Impact.**
- C5's guarantee is false for every non-atomic write sequence longer than 5 s.
- The :687 exposure is not narrowed, because §3.5 addresses temp litter, not torn writes.
- The forced path is reachable by the bound alone, so even a patient user can be left with stale Git locks.
- D-V's unconditional requirement is narrowed without stating the residual.

**Required correction.**
- State the forced-path residual explicitly as the operator-accepted exception to D-V: which write sequences may tear, what remains, and how the next command recognizes it.
- Or require the forced path to wait for a W phase's current safe point and say what bounds that wait.
- Restate the :687 change accurately.

**Closure test.** A row forces an exit during a checkout and asserts the specified post-state and the next command's recognition.

### P2-6: gwz-py's interrupt path is misdescribed; the first interrupt does not cancel today, and the change to gwz-py's public lifecycle is not stated

**Location.** §3.1 "gwz-py" (:107-111) and §4 row 3.2 (~80 lines).

**Violated invariant.** D-V requires cancellation end to end for every client, and the §2 facts must be accurate.

**What the code shows.**
- `bridge.py:304-313` (`_run_native`) catches `asyncio.CancelledError` and only `await _await_completion(worker)`. It never calls `cancel_operation`.
- `_await_completion` (:28-35) "keep[s] waiting … through repeated caller cancellation".
- The `Canceller` (`route/transport.rs:44, :152, :167-173`) is invoked only from `cancel_operation` and `close` (`client_host/operations.rs:216-227, :274-284`).
- The gwz-py CLI (`cli.py:147`, `asyncio.run`) therefore waits on Ctrl-C for the native operation to finish, for example a helper's 120 s bound.

**Reproduction.** Run `gwz-py fetch` against a sleeping helper and press Ctrl-C. Nothing is cancelled until the helper's bound passes.

**What else §4 leaves out.**
- The ~80-line "second interrupt as Force" does not address the first interrupt.
- It does not give the gwz-py CLI an exit status (OD-C4 says "every platform", but not "every client").
- It does not address the documented native-route carve-out. The design §2.5 and `README.md:225` say a native operation's cancel is `UnsupportedOperation`, and §3.2's native abort makes that cancellable.

**Impact.** C1 and D-V are unmet for gwz-py, and plan 3.5's Surface review of "gwz-py's public lifecycle" would see an unannounced change.

**Required correction.**
- Correct the fact.
- Specify that caller cancellation of an awaiting call cancels its operation through the `Canceller` before waiting, bounded by the cleanup bound.
- Specify the gwz-py CLI's exit status.
- State the amendments to gwz-py design §2.5 and its README.
- Re-size the 3.2 piece.

**Closure test.** A gwz-py CLI row sends SIGINT during a sleeping-helper lookup. The helper's group is gone within the bound, and the specified exit status follows.

### P2-7: OD-C2 (a)'s reaper is not specified completely

**Location.** §3.4 "a lifeline reaper" (:150-157) and OD-C2 (:245-247).

**Violated invariants.** The §6 contract (names, defaults, bounds, complete lifecycle), C2 and C3, C6 (no new global state), and accurate facts.

**Gaps, each concrete:**
1. **Executable in gwz-py.** "gwz re-executed in a hidden `__reaper` mode, so nothing new ships" fails in gwz-py. There, `current_exe()` is the Python interpreter. gwz-py's own SSPI worker is a separate file shipped "beside the loaded extension" (`gwz-py/native/src/worker_host.rs:18-30`). The reaper is placed in gwz-core (3.0c), which has no route to an executable from the composition root. Contrast the existing `NativeHost::new(worker descriptor)`.
2. **Version.** gwz-cli's existing early mode (`worker_host::early`) gates the re-exec on a compiled build fingerprint. The reaper has no gate. If an upgrade replaces the binary mid-run, the new image may not know `__reaper`, and the text gives no policy for a reaper that fails to start: refuse the spawn, or proceed and accept the orphan.
3. **The reaper's own signals.** Its process group and disposition are not stated. If it stays in gwz's foreground group with default dispositions, the same Ctrl-C (or SIGHUP) kills it. A later SIGKILL of a gwz stuck in cleanup then orphans the helper, which is exactly row 2's case preceded by a SIGINT. No row covers this.
4. **Ownership.** "The first child of a process starts one small reaper", one per process, is process-global state unless the composition root owns it. §3.4 says the registry is "never global", but the per-process reaper is not assigned to `HostOwner` or gwz-py's composition object.
5. **Death of the reaper.** gwz's detection of a dead reaper is not covered by C3.

**Impact.** C2 is unmet on macOS and Linux for gwz-py, and the mechanism can silently fail closed-to-orphan in gwz-cli.

**Required correction.**
- State the reaper's executable source per host process type, its version gate, and the start-failure policy.
- State its process group and signal disposition: its own session, ignoring SIGINT, SIGHUP and SIGTERM.
- Name its owner (the composition root).
- State how gwz detects the reaper's death and what it then does.

**Closure test.** Rows:
- SIGINT, then SIGKILL of gwz, leaves no helper.
- A gwz-py process killed by SIGKILL leaves no helper.
- A reaper that cannot start produces the specified outcome.

### P2-8: C2's universal scope contradicts TR1.6 §3.3 and OD-C6, and "a group ends" is undefined

**Location.** C2 (:27-29) and §3.4 "gwz tells the reaper each child's process group as it starts and ends" (:153).

**Violated invariant.**
- TR1.6 §3.3 "Completion" (:86) stands, because D-V reopened only OQ2 and OQ4: "a helper's background child … nothing kills that child on success".
- OD-C6 says the pager and `forall` children are not registered and "never kills them".
- L1-08 applies.

**Reproduction.**
- With `credential.helper=cache`, the helper starts `git-credential-cache--daemon`, which is meant to outlive the lookup.
- Reading A: C2 ("every process gwz starts ends no later than the operation that started it") kills it at operation end, or the reaper does at gwz's exit. That breaks credential caching on every command.
- Reading B: "ends" means the leader was reaped, which is the current Unix `drained()` (`process_tree.rs:92-95`). Then the group is unregistered and the daemon outlives gwz, contrary to C2.
- Likewise, `gwz forall -- sleep 1000` followed by SIGKILL of gwz leaves `sleep` running, contrary to C2's "however gwz ends: … SIGKILL".

**Impact.**
- C2 is unsatisfiable as written.
- Implementers must choose between breaking credential caching and violating C2.
- The TR1.6 §3.3 interaction is not stated.

**Required correction.**
- Scope C2 to the processes gwz waits on, until their operation's terminal: a helper and its group until the lookup completes; `rev-list`; the stdio and ssh child.
- State explicitly that a successful helper's surviving background children and the OD-C6 user programs are outside C2, citing TR1.6 §3.3.
- Define "a group ends" for the registry and the reaper.

**Closure test.** A row with a recording helper that leaves a background child on success: the child survives a normal exit and is killed on cancel.

### P2-9: the `GIT_ELOCKED` message claims evidence of staleness that a concurrent `gwz fetch` defeats

**Location.** §3.5 "Git's `*.lock` files" (:171-173), C4 and §5 row 7.

**Violated invariant.** C4: only "safe removal" is advised. The installed help for `gwz fetch` also says: "It never writes workspace artifacts: no lock … one of the few network verbs that still runs while a merge is open". So fetch writes refs without the workspace mutator lock.

**Reproduction.**
- `gwz fetch` (or a served core's fetch) is updating `refs/remotes/origin/main` and holds `refs/remotes/origin/main.lock`.
- A concurrent `gwz pull` meets `GIT_ELOCKED` on it.
- It can take the mutator lock, so its error "says no gwz mutation holds the workspace" and gives the removal command.
- The user removes a live lock, and two writers race on the ref.

**Impact.** The surface advice can lead to ref corruption. The message also cannot see plain `git` processes, which §3.5 itself acknowledges.

**Required correction.**
- Do not present taking the mutator lock as evidence of staleness.
- The message names the path, says that gwz cannot tell whether a process is using it, and advises removal only after the user confirms that no `git` or `gwz` process is running in that repository.
- Give the per-platform removal command (`rm` / `del` / `Remove-Item`).

**Closure test.** A row holds a fetch's ref lock while another command meets it, and asserts that the message makes no "not held" claim.

### P2-10: OD-C4 is incompletely specified, with no field, no schema, and no forced-path output, and documented exit codes are contradicted without being listed

**Location.** OD-C4 (:252-253) and §3.3 (:131-137).

**Violated invariant.**
- The §6 contract: names and defaults.
- `gwz-cli/docs/MachineOutput.md:954-964`: "GWZ maps aggregate status to process exit codes", which allows only 0, 1 and 2 (`render_exit.rs:82-95`).
- `AggregateStatus` (`protocol/generated.rs:1133-1142`) has no `Cancelled`.
- The error `cancelled` (73) exists (`ErrorCatalog.md:35`).

**Reproduction.** `gwz --json fetch` is interrupted. The text does not say:
- which JSON field says `cancelled`: a new `AggregateStatus` (a wire and schema change, append-only, with gwz-py decoding impact), the `errors[0].code`, or both;
- what the aggregate is when some members completed;
- whether the forced path writes any JSON or only the "one line";
- whether that line goes to stdout, which would corrupt `--json` consumers, or to stderr.

130 is not in the documented exit table.

**Impact.**
- Machine consumers have no stable contract.
- A schema change is implied but not owned.
- The documentation contradiction is unstated.

**Required correction.**
- Name the field and value.
- State whether `AggregateStatus` gains `Cancelled`, with its wire number and owning step.
- Define forced-path output per output mode, with the stream for each.
- List `MachineOutput.md`'s exit table and `docs/CLI.md` as amended.
- State the gwz-py CLI's status (see P2-6).

**Closure test.** `--json` rows for a first and a second interrupt parse as the specified document, and the exit status is 130 on three platforms.

### P3-1: citations drift, are misattributed, or dangle

**Location.** :16, :184 and :203, plus the header at :6.

**Defect.**
- At the reviewed root 9d5a4d4c, split document :232, :235 and :239-240 point to the §4.3 heading, "Peers", "SIGINT/SIGTERM" and "Stale socket". They were correct at a519ee58, but 7fa70514 added 7 lines before r0 was committed.
- :235 is cited for "replies … carry the core's cleanup report". That text is session design :183.
- "fact list gap 11" (:203) is defined nowhere in the repository.
- Code citations at 07e99924 have drifted at b745f1fa: `carrier/mod.rs` CloseReason :312→:322, and `local/windows.rs` :560-572→:606-613.

**Impact.** An auditor at the reviewed tuple lands on the wrong clauses.

**Correction.** Re-anchor every citation to one root, and define or drop "gap 11".

**Closure.** Every cited line at the stated root contains the quoted substance.

### P3-2: §2's temp facts and §3.5's sweep inventory are incomplete

**Location.** §2 :83-84, §3.5 :165-168, and §5 row 6.

**Defect.**
- "nothing sweeps a temp a crash left" ignores the family store's `.tmp.` marker "the store's cleanup looks for" (`family-store/src/publish.rs:150-168`).
- The sweep pattern `<name>.*.tmp` misses `.{name}.tmp.{unique}` (family store), `.gwz-refcopy.*.*.tmp` (`refcopy/src/ordinary.rs:615`) and `gwz-memory-*.tmp` (`filesystem.rs:51`).
- The premise "every durable writer holds the mutator lock" is asserted, not shown.
- §3.5's row ("live writer in another workspace") and §5 row 6 ("a live writer's temp") differ.

**Impact.** C4's "every file gwz writes" is not met for these writers.

**Correction.** Inventory the temp shapes and their writers' locks, and align the two row texts.

**Closure.** One row per temp shape.

### P3-3: keeping the helper's stdin open does not reach the helper, and it changes TR1.6 §3.3 "Pipes" without listing it

**Location.** §3.4 :158-160.

**Defect.**
- gwz's child is `git credential fill`. Its input ends at the blank line (TR1.6 §3.1), after which git stops reading.
- The helper's stdin is git's own pipe, which git closes.
- So no helper sees gwz's EOF.
- TR1.6 §3.3 "The input is written, then closed" is amended silently.

**Impact.** The text claims a C3 path that does not exist.

**Correction.** Drop the bullet, or restate it as `git` noticing gwz's death and show that git notices it.

**Closure.** No row depends on this path, or a row shows it working.

### P3-4: §5 rows are not all red-first, and signal coverage is incomplete

**Defect.**
- Row 2's Windows leg is green before any change: §3.4 itself says `KILL_ON_JOB_CLOSE` "already does this".
- There are no rows for SIGTERM, SIGHUP, CTRL_BREAK or CTRL_CLOSE (C1 lists all of them), and none for "SIGINT, then SIGKILL" (P2-7).
- Rows 3 to 8 name no platforms.

**Impact.** "Each seen red first" cannot be honoured as written.

**Correction.**
- Mark row 2's Windows leg as a regression row.
- Add the missing signal rows.
- Name platforms per row.

**Closure.** Every row's red evidence is archived per named platform.

### P3-5: OD-C1's "gwz-core and gwz-py install none" is not mechanically enforced

**Location.** OD-C1, and §2 :49.

**Defect.**
- `check_process_globals.py:103-104, :540-541` matches only `libc::signal` and `libc::sigaction`, and classes them `env` (not global state, per its docstring).
- It does not see `SetConsoleCtrlHandler`, `signal_hook`, `nix::sys::signal` or `ctrlc`.
- §2's "forbids … as process-global state" is therefore imprecise.

**Impact.** A Windows handler, or a crate-based handler, in gwz-core or gwz-py would pass.

**Correction.** Name the check extension (identifiers and kind) in the owning step.

**Closure.** Mutation rows that add each of these spellings to gwz-core fail the check.

### P3-6: the native-transfer abort is overclaimed

**Location.** §3.2 :119-122.

**Defect.**
- `transfer_progress` is installed only `if let Some(progress)` (`transport_support.rs:89-93`).
- No callback runs during connect, negotiation or credential phases.
- Push abort (`push_transfer_progress`) is not named.
- Only a `file://` fetch row exists (row 4).

**Impact.** "This closes the ordinary-build gap" holds only for the pack-transfer phase of fetch.

**Correction.**
- Install the callbacks unconditionally.
- Name push.
- State the uncovered phases and that the C5 bound covers them.

**Closure.** Rows for push and an SSH native fetch.

### P3-7: OD-C6's unbounded wait conflicts with C5's bound

**Location.** OD-C6 (:257-258), §3.4 :144-146, and C5.

**Defect.**
- "gwz waits for the current one to exit" has no bound, while C5 forces an exit at 5 s or on the second interrupt.
- §3.3's Force ends only registered trees, so a forced gwz exits while `less` (which ignores SIGINT) still owns the terminal, beside the shell.
- Which rule wins is unstated.

**Impact.** Either gwz hangs past C5, or the terminal is left shared.

**Correction.** State the rule. For example: Force waits for the foreground pager or `forall` child without a bound, and says so on stderr. Or Force returns only after that child exits.

**Closure.** A pager row and a SIGINT-ignoring `forall` row.

### P3-8: the child registry is a new state owner that contract §6 and the retained-child rule do not account for

**Location.** §3.4 :141-143 and §7.

**Defect.**
- Contract §6 assigns "retained disposal" to Composition/CleanupDomain.
- TR1.6 §3.3 retains an unreaped `git` "with its slot … as pending cleanup".
- The amendment adds "the owning host's child registry" while §7 says the contract is unchanged.
- Who kills versus who retains, and the order between them, is unstated.

**Impact.** Two owners of one child would double-account cleanup or kill a retained child early.

**Correction.** Place the registry inside the CleanupDomain, or amend contract §6.

**Closure.** A row where a child is killed after its slot is retained, with the cleanup report counting it exactly once.

### P3-9: the per-step review tier contradicts plan §4's review rule

**Location.** §4 :219-220.

**Defect.** Plan §4 (:303-306) has one review per phase, with per-step dual reviews only for secret-handling or wire-format steps. The amendment adds "single (State)" per-step reviews to Phase 3 steps.

**Impact.** Process drift and review cost without a recorded decision.

**Correction.** Defer to the 3.5 phase review, keeping dual reviews only where the plan's rule applies (the stdio/ssh spawn). Or record the departure as a decision.

**Closure.** The §4 tiers match the plan's rule.

## 2. Verified citations (each citation checked, with result)

### Documents

| Citation | Result |
|---|---|
| TR1.6 :75-76 (§3.2 process group; "Its cost") | Holds. :76 contains "a helper still waiting outlives it". |
| TR1.6 :398 (OQ2) | Holds. |
| `GwzSplitCommandChannel.md:232, :235, :239-240` | Holds at a519ee58; wrong at 9d5a4d4c (P3-1). :235 does not state the "cleanup report"; session design :183 does. |
| `GwzCoreSessionDesign.md:687` | Holds verbatim. |
| Parked :809 (stdio child, new session) | Holds. |
| Parked :1107 (terminal interrupt) | Holds. |
| Parked :1218-1224 (ssh SIGINT/SIGQUIT ignored, termination, 10 s wait) | Holds. |
| Contract §5 :253-284 ("5 s, the host's close bound") | The five-second cleanup bound is at :274 (§5). The host close bound is at :189 (§4.1). It is mislabelled relative to the split document's "close bound" (P2-3). |
| D-V text against C1-C6 | C1 to C3 match. C4 and C5 narrow D-V (P2-5). |
| Split document `ServerAlive*` 15 s × 3 = 45 s | Holds (§4.4). |

### Code

| Citation | Result |
|---|---|
| `check_process_globals.py:103-104` | Holds (`signal`, `sigaction` in `LIBC_FUNCS`), but classed `env` (P3-5). |
| `gwz-cli/src/lib.rs:160-330` | Holds. No handler. `worker_host::early` precedes dispatch. |
| `token.rs:72-132`, `gate.rs:140-158` | Holds. |
| `cancellable.rs:40-68` | Holds. |
| `session/requests.rs:37-75` | Holds. Cancels the owner, engines, streams and checks. |
| `gwz-py/native/src/route/transport.rs:20-21, 194, :44, :167-173` | Holds as code. The §3.1 claim that a KeyboardInterrupt cancels the operation is false (P2-6). |
| `gwz-cli/src/globalargs/dispatch.rs:67-80` | Holds (`with_local_transport_native*`). |
| `served/dispatch.rs:100-107` | Holds ("read no token yet"). |
| `memberlockmanager.rs:99-120` | Holds. |
| `handle_local.rs:72` | Holds (`NeverCancelled`). |
| `transport_support.rs:90-93` | Holds (returns `true`; installed conditionally). |
| `process_tree.rs:57-95` | Holds (`process_group(0)`, `kill(-pgid, SIGKILL)`). |
| `owner.rs:253-303` | Holds (terminate and Drop kill). |
| `process_tree/windows.rs:65-133` | Holds (`KILL_ON_JOB_CLOSE`). The helper is created with `CREATE_NO_WINDOW` (`launch.rs:224`). |
| `pipes.rs:9-16` | Holds (write, then `shutdown`). |
| `commit_log/mod.rs:330-358` | Holds (no group, `wait_with_output`). |
| `pager.rs:147-165`, `forall.rs:231-262` | Holds. |
| gwz-sspi `KILL_ON_JOB_CLOSE` | Holds (`supervisor/windows/launch.rs:278`). |
| `carrier/mod.rs:312-327`, `inbound.rs:358-367`, `writer.rs:344-378` | Holds at 07e99924. `mod.rs` has drifted +10 at b745f1fa. |
| `carrier/local/windows.rs:560-572` | Holds at 07e99924; at :606-613 at b745f1fa. |
| `session-host/src/service/workers.rs:213-235` | Holds (steps 2 to 4, `close_wait`). |
| `workspace_mutator_lock.rs:42-50`; `family-store/src/lock.rs` | Holds (advisory). |
| `artifact/encoding.rs:47-56` | Holds as code. "Nothing sweeps" is overstated (P3-2). |
| `libgit2/src/util/filebuf.c:44-52` | Holds (`GIT_ELOCKED`). |
| `gwz-py/src/gwz/bridge.py:29-35` | Holds (actually :28-35). |
| gwz-transport `remote/core.rs` | Holds at `crates/gwz-connection/src/remote/core.rs:337-347, :501-505`: revoke, then `carrier_lost`. |

## 3. Risks and next action

### Surface: the user-facing behaviour this would freeze

The installed `gwz 1.0.18` was read cold. `gwz --help` and `gwz help <cmd>` document no exit codes, interrupts or signals. `MachineOutput.md` documents only 0, 1 and 2. `gwz __reaper` today is "unrecognized subcommand" with a usage error, and `gwz help __reaper` is "unknown help topic".

- **First Ctrl-C.** Cancel, wait up to 5 s, "print what was cancelled", exit 130.
  - Not stated: the stream (stdout or stderr), the format in `--json` mode, and the per-member rows (P2-10).
  - In the served form, the 5 s bound contradicts the split document's close bound (P2-3).
- **Second Ctrl-C, or the bound.** One line naming the unfinished operations, exit 130.
  - Unstated: the stream, and `--json` well-formedness.
  - It may leave Git locks or torn writes, and the user is not told which (P2-5).
- **Exit status 130 on every platform.** This is new against the documented table (P2-10).
  - On Windows it replaces today's default Ctrl-C exit, `STATUS_CONTROL_C_EXIT`. The operator chose this, but it must be documented.
  - The gwz-py CLI's status is unstated (P2-6).
- **`--json` "cancelled".** No field is named (P2-10).
- **The `GIT_ELOCKED` message.** It names the path and gives a removal command. Its "no gwz mutation holds the workspace" clause is unsafe (P2-9), and the per-platform command is unstated.
- **Pager and `forall`.** They keep the terminal's interrupt. The interaction with the bound is unstated (P3-7). The default with an unbounded wait is not stated.
- **Hidden `__reaper` mode.** "Hidden" is not specified. gwz-cli has no completion generator, and `help.rs` filters clap subcommands marked hidden. An argv early-dispatch mode like `worker_host::early` would bypass clap, help and `cli_reference`. What a user who runs `gwz __reaper` gets is unspecified:
  - If the reaper reads process groups from an inherited stdin, an interactive user could type a group ID and have gwz SIGKILL it.
  - It needs a bootstrap gate, like the SSPI worker's fingerprint, and a defined refusal with an exit code (P2-7).

### Risks

These are hypotheses for the implementing steps and are not counted in the verdict.
- **Process-group ID reuse.** Between a group emptying and gwz telling the reaper that it ended, a SIGKILL of gwz leaves a stale group ID in the reaper's set. If that ID is reused, the reaper kills an unrelated group.
- **Fork without exec.** In gwz-py, an embedding application's `fork()` without `exec` (multiprocessing's fork start method) duplicates the lifeline's write end, because close-on-exec does not apply. The reaper then never sees EOF.
- **Closed terminal.** On SIGHUP, the "print what was cancelled" write to a closed terminal can fail. Rust's `println!` panics on a write error.

### Next action

Revise to r2, addressing P2-1 to P2-10:
- Re-place §4 against the plan's graph and amend the plan and the 2.3 plan explicitly.
- Reconcile OD-C1 with 2.3i.
- Name the bounds per form.
- Decide P2-4 (a) or (b) and P2-8's C2 scope, and record them where they amend accepted text.
- Correct gwz-py's path.
- Complete the reaper and OD-C4 specifications.
- Fix the `ELOCKED` text.

Re-anchor the citations (P3-1). Then re-review both axes.

## 4. Out-of-scope observations

- **TR1.6 OQ3 (b)'s window-prompting helper.** It interacts with cancel: a sign-in window killed at cancel leaves no window. That is consistent with D-V, but the user-visible effect, a GCM window vanishing on Ctrl-C, is not described anywhere. This is a documentation limitation, not a defect, and does not block.
- **Contract amendment trail.** The contract header's amendment trail and the session design's header list amendments by document. If accepted, this amendment must add its lines to both (L1-08), and it does not currently say so. This is subsumed by P2-3, P2-4 and P3-8.
- **Sections the amendment does not cite.** The parked server design's :586-590 shutdown steps use "close bound" and `close_wait` interchangeably. That pre-existing ambiguity contributes to P2-3 and should be cleaned when 2.3i lands. It is a limitation and does not block.
