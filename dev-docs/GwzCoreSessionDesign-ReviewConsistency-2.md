# GwzCoreSessionDesign — CONSISTENCY-AXIS REVIEW (remediation round 2 re-verdict)

**Review object:** `dev-docs/GwzCoreSessionDesign.md` revision 2 at root `58ea74bd70b24d762409a8cf2852fedd94f9897d` (DRAFT contract design, revision 2, dated 2026-09-25, "review required; no implementation or activation authority"), applying `dev-docs/GwzCoreSessionDesign-RemPlan-1.md` (root `ae62fbcaac56b3809044c5b187e7fb1196f76a0d`) to revision 1 (root `cb5d6f2`); the paired DRAFT sections at gwz-core `730e7119baea6df6e6cfbe731586323d2a183836` (`dev-docs/GWZDesign.md` lines 9–15; `dev-docs/GWZRequirements.md` lines 9–23) with the revised `scripts/checks/check_process_globals.py` (new `process` kind), `process_globals_allowlist.json` (29 items: 17 debt, 12 permanent) and `test_check_process_globals.py`; gwz-py `685ecdc80e165e28f96e84cf68a9be7caed33c1d` (`dev-docs/GwzPyDesign.md` "Draft session pointer", now naming the host-context constructor); gwz-transport `67ed9b16fa0d560a0912ee846c1aa2a9066cb318` (`scripts/process_globals_allowlist.json`, `scripts/test_process_globals.py`, `.github/workflows/contracts.yml`). Previous tuple: root `cb5d6f2`, gwz-core `a8b27f2`, gwz-py `0905f73`, gwz-transport `36ae2b1`.
**Baseline:** the four SHAs above, verified identical at start (00:32 AEST) and at end (00:36 AEST); gwz-transport's uncommitted `Cargo.toml` change is out of scope. Committed content read with `git show` / `git diff` / `git … grep`; working-tree reads only for taut and taut-shape-py. The only executed programs were the permitted read-only checker runs and its unit tests. The other axis's current-round report was not seen; the merged `Verdict-1` and `RemPlan-1` were used as permitted.
**Date:** 2026-09-25
**Axis:** Consistency — internal contradictions; agreement with every cited contract, design, schema and source, quotes verified at source; exactness of superseded-clause lists; satisfiability of §15; the paired core paragraphs making the same claims with no extra MUSTs; the gwz-py pointer's accuracy; plus the closure of P3-14..P3-22, the consistency of the Safety dispositions (B10, P3-6..P3-13) with the rest of the contract, and the ruling on the six applied choices. Independent, adversarial, read-only. Filed verbatim by the lane owner as `dev-docs/GwzCoreSessionDesign-ReviewConsistency-2.md`.

**Verdict: GO** — 0 P0, 0 P1, 0 P2, 5 P3 (new, IDs P3-23 to P3-27). P3-14..P3-22 are all CLOSED. Every RemPlan-1 disposition for the Safety findings (B10, P3-6..P3-13) is stated consistently with the rest of the contract, with one wording clash filed below as P3-25. All six applied choices stay within RemPlan-1's dispositions. No new finding is an architectural root cause; each is a bounded text or configuration correction that does not block acceptance of the design contract.

---

## 0. Evidence base

- `dev-docs/GwzCoreSessionDesign.md` @ 58ea74b, lines 1–646 (all), and `git diff cb5d6f2 58ea74b` on it.
- `dev-docs/GwzCoreSessionDesign-Verdict-1.md` @ ae62fbc, lines 1–41 (all); `dev-docs/GwzCoreSessionDesign-RemPlan-1.md` @ ae62fbc, lines 1–58 (all).
- gwz-core `git diff a8b27f2 730e711` (docs, checker, allowlist, test); `dev-docs/GwzRemoteTransportSshAgentDesign.md` lines 68–75 (unchanged); handler sources at 730e711: `src/workspace_ops/handle_remote_identity.rs` 10–48 (guard at line 20, `Set` writes at 37), `src/workspace_ops/push_member.rs` 13–23, 58–69 (`guarded_workspace_root_for_request_in`), `src/workspace_ops/merge/runtime/mutation_guard.rs` 204–213 and its 12 non-test users, the 13 direct guard/lock sites under `src/workspace_ops` and `src/operation_context.rs`; `src/workspace_ops/target_listing.rs` 80 (`resolve_forall_targets`), `handle_ls.rs`, `handle_list_snapshots.rs`, `src/diff/handle_diff.rs`, `src/status/status_member.rs`, `src/operation/push_event.rs` (`handle_log`) — none references the mutator lock or guard; spawn sites `src/git/endpoint/https_auth.rs` 317–327 (`env_clear()` + `envs(config.environment)`), `src/git/gitbackend/refs.rs` 172, 233, `repository.rs` 300, `transport.rs` 348, `src/operation/commit_log/mod.rs` 320 (no `env_clear`).
- gwz-py `git diff 0905f73 685ecdc` (pointer); `native/src/dispatch/read.rs` 24–34, 102, 289–290 (today's direct dispatch of `remote_identity` and `resolve_forall_targets`).
- gwz-transport `git diff 36ae2b1 67ed9b1` (allowlist, test, workflow lines 21–45).
- taut-shape-py `log/_generated.py` 87–114 (unchanged); schema facts carried over from rounds 1 and 2 (`GwzErrorCode` tail, `TransportCapabilitiesResponse`, `GwzError`, `ResponseMeta`, `InvocationContext`).
- Checker runs from the workspace root: gwz-core "982 files, 29 allowlisted items (17 debt, 12 permanent); nothing new" (exit 0; `--list` shows exactly five `process` items at the files §5.7 names); gwz-py "17 files, 10 allowlisted items (10 debt, 0 permanent); nothing new" (exit 0); gwz-transport "26 files, 1 allowlisted items (0 debt, 1 permanent); nothing new" (exit 0; `--list`: `src/pool/machine.rs:7 static NEXT_POOL AtomicU64`); `python3 -B -m unittest gwz-core/scripts/checks/test_check_process_globals.py`: 11 tests, OK.

## 1. Closure tables

### 1a. P3-14..P3-22 against revision 2

| ID | Status | Revised location | Original counterexample on the revised text |
| --- | --- | --- | --- |
| P3-14 (cancel handling across the reader/admission split) | CLOSED | O7 line 80; §4.2 lines 161–162; §5.1 lines 186–193; §5.3 line 240; §7 line 381; §8 step 2; §15.5 line 582; §16 line 633 | No longer reproduces: the reading thread creates the record, token and gate at receipt; control frames are handled on the reading thread; a cancel of a not-yet-admitted call cancels its token and admission settles it `Cancelled` before any effect. |
| P3-15 (`diff.output` release) | CLOSED | §4.2 line 153; §5.4 lines 262–266; §15.7 line 593 | "Open" is defined (creation until release); release on seal/close plus last-stream-end through `DiffLogRegistry::release`, on `log.output` end stream, and at session end. |
| P3-16 (log appends ungated) | CLOSED | §5.6 lines 315, 318–319; §15.9 line 606 | Appends, seal and close are gated; the registry writes the spool; a producer never holds the file handle; after revocation appends are dropped. |
| P3-17 (reply kinds) | CLOSED | §4.2 line 147 ("Reply kinds"), line 162; §5.3 lines 240–242; §15.5 line 583 | `SessionError` with the terminal's first `GwzError` and `ResponseMeta` for `Cancelled`/`Failed`-without-response; `SessionReply` when a response exists; waiting direct calls settle `Cancelled`. |
| P3-18 (`call_id` monotonicity) | CLOSED | O2 line 66; §3 line 100; §4.1 line 132; §15.1 line 555; GWZRequirements bullet 2 | A non-increasing `call_id` is a protocol error; the outstanding-reuse rule is subsumed. |
| P3-19 (O9 absolutes) | CLOSED | O9 line 82; GWZDesign paragraph line 13; GWZRequirements bullet 6 | "other than the inventoried `permanent` entries … such as libgit2's server timeout in ordinary builds". |
| P3-20 (O7 scope; legacy handle) | CLOSED | O7 line 77; §14 line 531; §16 line 634; GWZRequirements bullet 4 | O7 stated for "any threaded or async API in core and the extension"; the two legacy exceptions are deprecated with removal tied to proposals §9 phases 3 and 5. (§5.2 line 228 still says "stays for the legacy path only" without the word "deprecated"; §16 supplies it — not a contradiction.) |
| P3-21 (host context) | CLOSED | §2 host-context row; §5.6 lines 297–307; §9 line 410 (`HostContext()`); §10 line 445; §11 line 478; §14 line 522; GwzPyDesign pointer | The constructor is a listed extension entry point; one host context per driver process by default; §14 now says the SSH agent design's process-wide scope becomes per-host-context and holds per process by default. Quotes at design lines 68–69 and 74 verified. |
| P3-22 (`operation.result` body) | CLOSED | §4.1 line 130; §13 lines 495–500; §15.2 line 561 | `OperationResultRequest { operation_id: str (1) }` plus the four new methods' requests. |

### 1b. RemPlan-1 dispositions for the Safety findings, checked for consistency with the rest of the contract

| Disposition | Where stated | Consistent with the rest of the contract? |
| --- | --- | --- |
| B10 / P2-8 — `remote_identity` reclassified W; direct methods take no mutator lock | §4.2 lines 136–141; §5.1 table lines 199–201; §15.4 lines 572–573; GWZDesign line 15; GWZRequirements bullet 8 | Yes. Source verified: `handle_remote_identity.rs` line 20 takes the mutation guard for all three ops (line 25 passes `dry \|\| op == Get` as the planned flag, which per gwz-cli's own comment still holds the mutator lock) and `Set` writes configuration (line 37); none of the eight remaining direct handlers (`status`, `ls`, `resolve_forall_targets` at `target_listing.rs` 80, `list_snapshots`, `diff`, `log`, `transport_capabilities`, `configure_transport_runtime`) references the guard or the lock. Note: RemPlan-1 says "the seven that remain"; there are eight. The contract states no count and §15.4's core test covers "no R method", so the object is unaffected. |
| P3-6 — host context contents, surface, default, detached-worker registry, drop, §14, §5.1 attribution | §2; §5.1 lines 204–206; §5.6 lines 297–307; §8 steps 5 and prose; §9; §10 line 445; §11; §14 line 522; §15.9 lines 607–608 | Yes. Member lock manager moved to the host context (§5.6 line 300, §5.1 line 204, §16 line 639 all agree); the detached-worker wait is stated identically in §5.1, §8 and GWZRequirements bullet 10. |
| P3-7 — receipt before admission | O3 line 68; O7 line 80; §4.3 line 179; §5.1 lines 186–193; §5.3 line 240; §7 line 381; §8 step 2; §16 line 633 | Yes, except the stale "never admitted" wording at §4.2 line 165 (P3-26) and the unstated parking of held reads on the reading thread (P3-24). |
| P3-8 — delivered once | §5.1 line 213; §5.4 lines 255–256; §10 lines 451, 470; §15.7 line 594; GWZRequirements bullet 9 | Yes, with one clash between §10 lines 450 and 451 for a result reply on a closed loop (P3-25). "Every view its method declares" is defined as the result plus, for a merge, the response — consistent with `MergeOperationHandle.result` (client.py 1333–1346) and with `_stream_call`, which reads only the result. |
| P3-9 — detached marker | §6 line 362; §8 step 5; §4.2 line 174; §13 line 508; §15.9 line 605; GWZDesign line 15 | Yes. |
| P3-10 — snapshot handling | §5.6 lines 290–292; §10 line 446; §12 line 486; §15.8 lines 599; GWZRequirements bullet 6; GWZDesign line 13 | Yes, except §5.6 line 285 still says `os.environ` (P3-23). |
| P3-11 — child processes | §5.6 line 293; §5.7 row 9 and prose line 328; §15.8 lines 600–601; O9 line 82; GWZRequirements bullet 6; GWZDesign line 13 | Yes. The five sites in the allowlist match the checker's `--list` output and the contract's list; `gh` at `https_auth.rs` 317–327 uses `env_clear()` and is correctly `permanent`; the four `git` spawns use no `env_clear` and are correctly `debt`. |
| P3-12 — gwz-transport coverage | §5.7 lines 323–326; §15.8 line 601; O9 line 82; GWZRequirements bullet 6; GWZDesign line 13 | Yes as text; the CI pin is P3-27. The transport test resolves `GWZ_CORE_CHECKOUT` relative to its root and falls back to the sibling checkout, as §5.7 says. |
| P3-13 — `call_id` monotonicity | as P3-18 above | Yes. |

## 2. Ruling on the six applied choices

All six stay within RemPlan-1's dispositions; the plan's fresh-round condition is not triggered.

1. **§13 also names the reply messages** (`CleanupReport { pending_local_work: u32, peer_cleanup_confirmed: bool }` for `operation.cancel`/`session.close`; `OperationReleaseResponse {}`). Within. §4.1 line 130 requires every call's `response` to be a declared message; the plan's request-only list was incomplete for the new methods' `out` types, and naming them completes the same disposition (P3-22) without adding behaviour. The `u32` narrowing of today's `usize` is safe for a job count; the Taut name `CleanupReport` coexists with `gwz_core::transport_host::CleanupReport` on a different path (noted in §5).
2. **Five spawn sites; four debt, `gh` permanent; `git rev-list` added.** Within. RemPlan-1 P3-11 says the `process` kind "flags every production `Command::new`", listing inherited-environment spawns as debt and `env_clear` spawns as permanent; the checker found the commit-log walk's `rev-list`, and §5.6/§5.7 list all five. Verified at source (evidence base).
3. **`operation_id` assigned at receipt.** Within: RemPlan-1's receipt-before-admission disposition names "for an operation method, its `operation_id`" among the pending record's contents.
4. **Default host context one per process, held by the Python bridge; `NativeCoreBridge` accepts one; `Client` unchanged.** Within: RemPlan-1 P3-6 states exactly this; the GwzPyDesign pointer (685ecdc lines 315–321) says the same. Holding it at the driver edge, not in core or the extension, is what O9 and "no static holds a host context" require.
5. **gwz-transport's CI checks out gwz-core's default branch; locally the sibling checkout.** Within the disposition's words ("the contracts workflow gains that checkout"); the unpinned ref is a robustness gap filed as P3-27, not an overreach.
6. **O9 names libgit2's server timeout; §5.6 heading includes the host context.** Within: RemPlan-1 P3-19's wording is reproduced verbatim, and the heading change follows P3-6.

## 3. Findings

### [P3-23] §5.6 contradicts itself on how the Python bridge captures the environment, and `open`'s snapshot type is unstated
- **Location:** §5.6 line 285 ("The Python bridge takes it from `os.environ` when it opens the session") versus §5.6 line 291 and §10 line 446 ("captured losslessly: from `os.environb` on POSIX and `os.environ` on Windows"); §9 line 411 (`options` carry "the endpoint environment captured by the bridge" — element type unstated).
- **Violated invariant:** one rule per fact; the RemPlan-1 P3-10 "Capture" disposition ("captured losslessly, with `os.environb` on POSIX").
- **Reproduction:** an implementer following line 285 captures `os.environ` (str, surrogateescape-decoded) on POSIX and passes str pairs; the extension, following line 291, expects byte pairs — the two halves of the bridge disagree, and a non-UTF-8 value round-trips only if the str path re-encodes with `os.fsencode`, which nothing says.
- **Impact:** the "lossless" claim depends on which sentence is followed; the `open` option's type is left to the implementer.
- **Required correction:** delete or align line 285 with line 291, and state in §9 that the snapshot is a sequence of byte-string pairs on every platform (Windows values encoded from `os.environ` as the extension specifies).
- **Regression test:** on POSIX, an environment value containing a non-UTF-8 byte reaches a session-path child process unchanged.
- **Classification:** bounded correction.

### [P3-24] "The reading thread handles … log reads at once" is unreconciled with reads being held up to 30 seconds
- **Location:** §5.1 line 192 ("The reading thread handles `operation.cancel`, `session.close` and log reads at once"); §4.2 line 152 (a read is held until a record, completion, the wait or session end); §5.5 (the read wait is the host's own timer); §1 line 47.
- **Violated invariant:** the host never blocks the channel (proposals §5; O5); "control frames and reads never wait for admission" (§5.1, GWZRequirements bullet 9).
- **Reproduction:** an implementer takes "handles at once" literally and services a read synchronously on the reading thread; an `events.subscribe` read on an idle operation holds that thread for up to 30 s, during which no frame — including a cancel or `session.close` — is read. Reads also serialise behind one another. The text never says a held read is parked as a waiter and completed by the producer's gated append/seal/close or by the host's read timer.
- **Impact:** a literal implementation defeats the very rule the sentence states; §15.10's "1024 held reads plus a cancel and a close complete without host blocking" would fail.
- **Required correction:** state that the reading thread registers a read as a parked waiter without blocking, and that a parked read is completed by the log's gated append, seal or close, by the read timer, or by session end.
- **Regression test:** with 1024 reads parked on idle logs, a cancel and a close are answered within their own bounds (already §15.10, made unambiguous by the text).
- **Classification:** bounded correction.

### [P3-25] §10's closed-loop drop rule and its keep-the-result rule conflict for a result reply whose loop has closed
- **Location:** §10 line 450 ("If that loop has closed … the reply is dropped and the pump continues") versus line 451 ("A result reply whose waiter was cancelled is not dropped. The bridge keeps it in its view of that operation until release"); §5.4 line 255 (delivery is counted when the view is delivered); §15.12 line 625 ("Replies for closed loops are dropped").
- **Violated invariant:** RemPlan-1 P3-8 ("a result reply that arrives after its waiter was cancelled is kept … instead of being dropped"); no undelivered outcome is lost on the client side once the host counts it delivered.
- **Reproduction:** a task awaiting `operation.result` on loop L is cancelled and L is closed before the reply arrives; line 450 says drop, line 451 says keep. If dropped, the host has counted the record delivered (the reply was sent), the record becomes evictable, and a retry from another loop can get `operation_expired` — the Safety P3-8 gap for the closed-loop case.
- **Impact:** the two sentences give opposite instructions for one event; the safer behaviour (keep) needs no loop, so nothing prevents it.
- **Required correction:** make line 450 "… the reply is dropped, except a result or response reply, which the bridge keeps in its per-operation view (line 451) regardless of the loop's state"; align §15.12.
- **Regression test:** close the issuing loop before the result reply arrives; `operation_result` from another loop returns the result.
- **Classification:** bounded correction.

### [P3-26] §4.2 still says a target "the session never admitted" gets `operation_not_found`, contradicting revision 2's receipt-time records
- **Location:** §4.2 line 165 ("A target that the session never admitted gets `operation_not_found`") versus §4.2 lines 161–162 ("A cancel may also arrive … even while the call is still being admitted"; "Cancelling a call not yet admitted … settles it `Cancelled`"), §5.1 line 186 and §7 line 381.
- **Violated invariant:** one rule per case within one bullet list.
- **Reproduction:** a cancel names a `call_id` that has been received but not yet admitted; line 162 says settle `Cancelled`, line 165 says `operation_not_found`.
- **Impact:** an implementer reading line 165 alone reintroduces the round-1 race for the resolution window.
- **Required correction:** "A target the session never received — a `call_id` above the highest received, or an unknown `operation_id` — gets `operation_not_found`."
- **Regression test:** §15.5 line 582's latched-resolution cancel (already present) plus a cancel of a never-received `call_id` asserting `operation_not_found`.
- **Classification:** bounded correction.

### [P3-27] gwz-transport's contracts job checks gwz-core out unpinned, so its pass/fail depends on gwz-core's moving head, and §5.7 does not say which gwz-core the check runs against
- **Location:** gwz-transport `.github/workflows/contracts.yml` @ 67ed9b1 lines 24–28 (`repository: owebeeone/gwz-core`, `path: .gwz-core`, no `ref`) beside the pinned taut-generator checkout in the same job (line 21, `ref: bcf98b64…`); contract §5.7 line 326 ("which its CI runs with a gwz-core checkout beside it"); §15.8 line 601.
- **Violated invariant:** a gate's result must be a function of the tuple under test; the workflow's own pinning convention.
- **Reproduction:** this very round added a detection kind (`process`) to the checker in gwz-core. Any such change on gwz-core's default branch can turn gwz-transport's contracts job red with no change in gwz-transport, and a gwz-transport release (Phase 8 step 2 publishes it first) then waits on an allowlist edit whose trigger lives in another repository.
- **Impact:** a spuriously failing release gate and an underspecified §15.8 claim ("passes in … gwz-transport" — against which checker?).
- **Required correction:** pin the gwz-core checkout by `ref` as the taut-generator is, and state the pin-bump step in §5.7; or state explicitly that gwz-transport checks against gwz-core's default branch and accept the coupling.
- **Regression test:** the workflow's checkout step names a SHA; a documentary check that §5.7 states the pinning rule.
- **Classification:** bounded correction.

## 4. Invariant analysis

Attacks that failed (evidence the invariant held):
- **Paired paragraphs make the same claims, no extra MUSTs.** Each GWZRequirements bullet traces to the contract: bullet 1 → §1/§11; 2 → O2/§3; 3 → O3/§5.2; 4 → O7/§16; 5 → O8/§5.6; 6 → O9/§5.6/§5.7; 7 → §6; 8 → §5.1/§4.2 (`remote_identity` W; direct methods take no mutator lock); 9 → §5.1 line 213, §5.4 line 255, §5.1 line 192; 10 → §8 steps 3–5 and §5.1 line 206; 11 → §13. The GWZDesign paragraph's new sentences (increasing call IDs; records at receipt; control frames and reads never wait; host context contents and default; permanent-entry exception; child processes and the snapshot's secrecy; three allowlists; `remote_identity` in W; no mutator lock for direct reads; detached results and registry; scalar-parameter request/reply messages) each appear in the contract.
- **gwz-py pointer.** GwzPyDesign lines 315–321 match §9 (`HostContext()` plus four operations), §10 line 445 (one per process, passed to every session), and §14 line 524.
- **Quotes verified at source.** SSH agent design lines 68–69 and 74 (§14 line 522); GWZDesign line 222 (§14 line 521); GwzPyDesign 282–285, 300, 306–308 (§14 lines 524–526); GwzPyTransportDesign clauses in §14 lines 528–543 unchanged from round 2 and still exact.
- **Source facts new in revision 2.** `remote_identity` takes the guard and writes configuration (B10 premise) — verified. Push takes the mutator lock through `guarded_workspace_root_for_request_in` (`push_member.rs` 58; `mutation_guard.rs` 204–213 returns a `WorkspaceMutationGuard`), so §5.1 line 205 is a true source claim; the helper's twelve users plus the nine direct guard sites cover branch, stash, materialize, pull-head preflight, repo lifecycle, create/add/sync repo, bootstrap, merge, commit, stage, capture, snapshot, tag, init and create-workspace, so "as W operations do" is accurate. The eight direct handlers reference neither the guard nor the lock. The five spawn sites and their environment handling match the allowlist and §5.6/§5.7 exactly.
- **Checker and CI wiring.** The `process` kind is keyed by program literal (test at `test_check_process_globals.py` 34–46 covers a literal and a non-literal program); gwz-core's allowlist has 29 entries (17 debt, 12 permanent) as the coordinator stated and as the run reports; gwz-py's and gwz-transport's runs are green; gwz-transport's test is discovered by `unittest discover -s scripts -p 'test_*.py'` with `GWZ_CORE_CHECKOUT=.gwz-core`, and resolves the path relative to its root as §5.7 describes.
- **Schema.** `OperationCancelRequest { call_id? , operation_id? }` with exactly one set matches §4.2's target rule; `CleanupReport` fields match `TransportCleanup` (bridge.py 20–25); `TransportCapabilitiesResponse.cancellation` at tag 3 follows fields 1–2; the two error codes follow `transport_record_limit=74`; taut-shape's `LogEndStream{log_id, stream_id}` supports §4.2 line 153 for all three log methods.
- **Limits and accounting.** §1 line 51 now validates table ≥ running + queued (round-2 risk closed); 8 + 64 ≤ 128; 1024 + 64 per queue; ≤ 16 running targets + close within the 64-frame control reserve; `remote_identity` moving to W changes no limit.
- **Method classification.** Still 37 methods, each once: 8 direct, 3 log reads, 1 result, 25 operation methods (24 from before plus `remote_identity`).
- **§14 exactness.** The SSH agent design entry now states the scope change; the transport-design lists are unchanged and exact; the two deprecated legacy exceptions are named in §14 line 531 and §16 line 634.
- **§15 satisfiability.** Every new item (15.1 lower id, 15.2 codec round trips, 15.4 `remote_identity` and R-method lock test, 15.5 latched resolution and waiting direct call, 15.7 diff release and delivered-once, 15.8 secrecy, `GIT_CONFIG_GLOBAL`, three-repo checks, 15.9 marker, gated `log` producer, host-context waits and shared budget) has a stated rule to test against, subject to the wording fixes in P3-23..P3-26.
- **Six choices.** All within RemPlan-1 (section 2).

## 5. Risks and next action

Residual risks below the finding bar:
- RemPlan-1's "the seven that remain" undercounts the direct methods (eight); the object states no count, and §15.4's core test must cover all eight.
- `operation.cancel` on a direct call replies "with that operation's cleanup report"; a direct call has no transport, so the report's values `(0, true)` should be stated when the tests are written.
- The host context's detached-worker registry learns that a worker "ends" through a signal outside the revoked gate (a join or drop guard); the mechanism is unstated but implementable.
- An admission thread stuck in resolution past the close bound is neither a worker nor counted in `pending_local_work`; §8 step 2 leaves it running without a report line.
- The Taut message name `CleanupReport` coincides with `gwz_core::transport_host::CleanupReport`; the generated type will sit at the crate root under the same name on a different path — rename one at implementation if the crate re-exports both.
- The Python bridge's per-process host context is a module-level singleton on the driver edge, outside the Rust checker's scope; that is the contract's stated placement, not a gap.

Next action: GO on the design contract at this tuple. The five P3 corrections are text and configuration changes for the lane owner to fold into the implementation plan or a final editorial pass; no further Consistency round is needed for acceptance. Acceptance covers the design contract only — not implementation, platform proof or release — as RemPlan-1's re-verdict clause states. No finding in this round is an architectural root cause, so the lane-stop condition is not met on this axis.
