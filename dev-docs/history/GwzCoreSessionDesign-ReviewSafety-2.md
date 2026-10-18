# GwzCoreSessionDesign — SAFETY-AXIS REVIEW, REVISION 2 (final remediation round)

**Review object:** `dev-docs/GwzCoreSessionDesign.md` revision 2 at root `58ea74bd70b24d762409a8cf2852fedd94f9897d` (whole; DRAFT, no implementation or activation authority); gwz-core `730e7119baea6df6e6cfbe731586323d2a183836` (`dev-docs/GWZDesign.md` "Core session host" and `dev-docs/GWZRequirements.md` "Core session host amendment" DRAFT sections; `scripts/checks/check_process_globals.py` with the new `process` kind, `process_globals_allowlist.json` at 29 items, `test_check_process_globals.py`); gwz-py `685ecdc80e165e28f96e84cf68a9be7caed33c1d` (`dev-docs/GwzPyDesign.md` "Draft session pointer"); gwz-transport `67ed9b16fa0d560a0912ee846c1aa2a9066cb318` (`scripts/process_globals_allowlist.json`, `scripts/test_process_globals.py`, `.github/workflows/contracts.yml`). Prior tuple: root `cb5d6f2`, gwz-core `a8b27f2`, gwz-py `0905f73`, gwz-transport `36ae2b1`. Round-1 re-review filed as `GwzCoreSessionDesign-ReviewSafety-1.md`; merged verdict `GwzCoreSessionDesign-Verdict-1.md` and plan `GwzCoreSessionDesign-RemPlan-1.md` at root `ae62fbc`.
**Baseline:** root `58ea74b`, gwz-core `730e711`, gwz-py `685ecdc`, gwz-transport `67ed9b1`, verified by `git rev-parse HEAD` at 00:32:53 AEST and again at 00:37:39 after the last read; unchanged. Committed content read with `git show`/`git diff`/`git grep` at those SHAs. The checker was run read-only with `python3 -B` on all three repositories, plus its unit tests. No builds, other tests, writes or network. The other axis's round-2 report was not seen; its round-1 findings were taken from RemPlan-1's summaries only.
**Date:** 2026-09-25
**Axis:** Safety — what the text permits to go wrong: degraded and mixed-version paths, irreversible steps and their preconditions, disclosure scale, stuck states reachable under the text's own rules, "never worse than the status quo" claims under concrete interleavings, scope creep. Independent, adversarial, read-only. Filed verbatim by the lane owner as `GwzCoreSessionDesign-ReviewSafety-2.md`.

**Verdict: GO** — 0 P0, 0 P1, 0 P2, 5 P3 (P3-23 to P3-27, all bounded corrections, none a new architectural root cause). P2-8 and P3-6 through P3-13 are CLOSED at the contract level; none of their sequences reproduces on revision 2. The six applied choices stay within RemPlan-1's dispositions. The five new P3 findings are nonblocking and belong in the implementation plan; none stops the lane.

---

## 0. Evidence base

Documents:
- `dev-docs/GwzCoreSessionDesign.md` at `58ea74b` (646 lines, whole) and `git diff cb5d6f2 58ea74b` on it (127 insertions, 63 deletions).
- `dev-docs/GwzCoreSessionDesign-Verdict-1.md` and `dev-docs/GwzCoreSessionDesign-RemPlan-1.md` at `ae62fbc` (whole).
- `git -C gwz-core diff a8b27f2 730e711` (paired sections; checker lines 6–11, 379–388, 574–578; allowlist; unit test lines 31–45).
- `git -C gwz-py diff 0905f73 685ecdc` (pointer paragraph, lines 315–321).
- `git -C gwz-transport diff 36ae2b1 67ed9b1` (workflow lines 24–30 and 42; allowlist; `scripts/test_process_globals.py`).

Checker runs (read-only): gwz-core "982 files, 29 allowlisted items (17 debt, 12 permanent); nothing new", exit 0; `--list` prints 29 items including the five `process` occurrences (`https_auth.rs:317` `Command::new`, `refs.rs:172,233`, `repository.rs:300`, `transport.rs:348`, `commit_log/mod.rs:320` `Command::new("git")`); gwz-py "17 files, 10 allowlisted (10 debt)", exit 0; gwz-transport "26 files, 1 allowlisted (1 permanent)", exit 0, `--list` shows `src/pool/machine.rs:7 NEXT_POOL`; `python3 -B -m unittest gwz-core/scripts/checks/test_check_process_globals.py`: 11 tests OK.

Source facts verified for the new material:
- `gwz-core/src/git/gitbackend/transport_support.rs` at `730e711` lines 255–268: the default backend's credential callback returns `Cred::ssh_key_from_agent`, then `git2::Cred::credential_helper(&config, url, username)` when `credential_helpers == AllowConfigured` (the `Git2Backend::new()` default, `backend.rs` 45), then `Cred::default()`.
- `gwz-core/src/operation/commit_log/mod.rs` 311–335: the path walk spawns `git rev-list` (`Command::new("git")`, `.env("GIT_OPTIONAL_LOCKS","0")`, no `env_clear`).
- `gwz-core/src/diff/log_service.rs` 38–44, 132–142 (`StopWhen::LastReader`), 227–234 (`end_stream` fires `ProducerStop` only "if it was the last reader"), 388–391 (`DiffLogRegistry::release` is an explicit `remove`).
- `check_process_globals.py` 507 (allowlist key is path, kind, name), 540 (occurrence count must match), 566 (list format).

## 1. Findings

### [P3-23] Two live W operations in different sessions that share a host context are not serialized; the second fails on the try-lock, and §5.1's list of "already held" sources omits the case
- **Location:** §5.1 line 205 ("'Already held' can therefore come only from another process, a session with another host context, or a worker detached from a closed session"), lines 206 (detached workers only), 201 (W exclusivity is per session), §5.6 lines 297–307 (host context holds the member lock manager and the detached registry, not W admission), §15.9 (tests share the helper budget, not W serialization). Source: every W handler acquires the `flock` try-lock (round 1: `mutation_guard.rs` 109, `workspace_mutator_lock.rs` 13–23).
- **Violated invariant:** admission decisions are the host's; an admitted operation must not fail on a lock the host could have serialized (the rule the contract applied to push and to detached workers); §5.1's enumeration claims to be exhaustive.
- **Sequence:** Clients A and B in one process share the bridge's default host context. Both submit `materialize` on workspace W. Each session's W exclusivity admits its own; A's handler takes the `flock`; B's handler is refused "workspace mutator lock is already held" and B's operation ends `Failed(UnsupportedOperation)`. Nothing queued it, and the refusal's attribution list does not include A. Today's two Clients behave the same, so this is not worse than the status quo; the contract's sentence is wrong and its host context is the natural place to fix it.
- **Impact:** spurious W failures between sessions of one process; misattributed refusals. The `flock` prevents overlap, so no corruption.
- **Remedy:** extend the detached-worker rule to live workers: while any session sharing the host context runs a W (or a push) on a workspace, another session's W on that workspace waits in its queue; keep the per-session table. Correct the attribution sentence.
- **Regression test:** two Clients in one process submit W operations on one workspace; both complete in turn with no `UnsupportedOperation`.
- **Classification:** bounded correction.

### [P3-24] In ordinary builds, libgit2's native HTTPS runs the configured credential helper as a child process with the live environment, inside the `git2` crate, where neither the snapshot nor the checker reaches; the contract does not state the exception
- **Location:** §5.6 line 289 ("A later change to the process environment does not change a session's credentials or trust context"), line 293 (child processes spawned with `env_clear()` plus the snapshot; four sites named), O9 line 82 ("The session's child processes get only its environment snapshot"), §5.8 (lists timeouts and cancellation as the ordinary-build degradations, not credentials). Source: `transport_support.rs` 261–263 calls `git2::Cred::credential_helper(&config, url, username_from_url)` under the default `AllowConfigured` policy; the helper program named by git configuration runs as a child of the process with its live environment, from code the checker's scan roots do not include and that is not a `Command::new` in gwz-core.
- **Violated invariant:** the environment-stability claim of §5.6 and O9's child-process rule, in the default wheel (the degraded path).
- **Sequence:** ordinary build; a Client opens a session; the application later changes `HOME`, `GIT_CONFIG_GLOBAL` or a credential-helper-related variable; the session's next HTTPS `fetch` on the default backend runs `git credential-<helper>` with the changed environment and may obtain a different credential from the one the snapshot fixed. In transport builds the same applies only to the native `git://`, `http://` and `file://` remotes §5.8 already names.
- **Impact:** the stability claim is false for the default wheel's HTTPS authentication, and the ratchet cannot see the spawn; a reader of §5.6 would not expect it.
- **Remedy:** state in §5.6 and §5.8 that in ordinary builds, and for libgit2's native remotes in transport builds, credential helpers are libgit2's and run with the live environment; add `Cred::credential_helper` to the checker's flagged calls as a `process` occurrence with a `permanent` entry naming that exception; have §16 list it beside the other ordinary-build degradations.
- **Regression test:** the checker flags a fault-injected `Cred::credential_helper` call; the ordinary-build test suite's environment-stability assertion (§15.8) is scoped to transport builds or asserts the documented exception.
- **Classification:** bounded correction.

### [P3-25] A byte-format `diff` whose output log is never read stays open until session end, so 64 unread diffs make the next refuse
- **Location:** §5.4 lines 262–266 (a log is released "when its producer has sealed or closed it and its last reader stream has ended", by `log.output` end stream, or at session end; "At most 64 are open at once. The next open is refused"), §4.2 line 153. Source: `log_service.rs` 227–234 (`end_stream` fires `ProducerStop` only "if it was the last reader"; a stream that never existed ends nothing), 388–391 (release is an explicit remove).
- **Violated invariant:** refusal before effect is meant for a client exceeding its bounds, not for one that used a documented request shape 64 times; the open-log limit should bound live readers, not unread output.
- **Sequence:** a client calls `diff` with a byte format 64 times and reads only the manifests (the `Client.diff` and `Client.diff_output` calls are separate, `client.py` 1081/1162). Each call mints a log, the producer seals it, no reader stream ever opens, so nothing fires the last-reader release. The 65th `diff` is refused `transport_session_full`. Recovery requires opening a reader stream on each stale `log_id` and ending it, which the client has no reason to know.
- **Impact:** a session whose `diff` stops working after 64 unread patch diffs; recoverable but obscure.
- **Remedy:** when the open-log limit is reached, release the oldest sealed log with no reader stream before refusing (mirroring the operation table's eviction of delivered records), and state that `end_stream` on a stream that never opened ends nothing.
- **Regression test:** 65 byte-format diffs with no reader succeed; a log with a live reader stream is never released by that path.
- **Classification:** bounded correction.

### [P3-26] The bridge's retained view of result replies for cancelled waits has no bound and no eviction signal
- **Location:** §10 line 451 ("A result reply whose waiter was cancelled is not dropped. The bridge keeps it in its view of that operation until release"), §5.4 lines 255–257 (the host may evict a delivered record without telling the bridge).
- **Violated invariant:** G9 bounded per-session resources; the contract bounds the host's table but not the bridge's mirror of it.
- **Sequence:** a long-lived service cancels result waits routinely (task cancellation on request timeouts) and never calls `release_operation`; each occurrence leaves a result in the bridge's view; the host evicts the record under pressure, the bridge keeps its copy; the view grows for the session's lifetime.
- **Impact:** a slow memory leak proportional to cancelled result waits; small per entry, unbounded in count.
- **Remedy:** cap the retained view at the operation-table size, oldest first, and drop an entry when a later call on that operation reports `operation_expired` or when the operation is released.
- **Regression test:** 1,000 cancelled result waits without release leave at most the table size of retained results.
- **Classification:** bounded correction.

### [P3-27] The bridge's per-process default host context is created "on first use" with no creation discipline, so concurrent first opens can create two and silently lose the sharing guarantees
- **Location:** §5.6 line 305 and §10 line 445 ("The Python bridge creates one per process on first use"), §9 line 410, §15.9 ("Two Clients in one process share the SSH helper budget", a sequential test). O7's principle is that the party creates the handle before the action.
- **Violated invariant:** one host context per driver process, on which the member-lock sharing (§5.1 line 204), the detached-worker wait (§5.1 line 206) and the SSH budget (§5.6 line 307) all rest.
- **Sequence:** a server starts N workers that each open a Client concurrently; two first opens race; each creates a host context; the two Clients then share nothing: fetch member steps on one member run concurrently, a detached worker of one is invisible to the other, and helper budgets double. Nothing reports it; §15.9's sequential test passes.
- **Impact:** silent absence of the cross-session guarantees the revision introduced; concurrent member updates fall back to git's own ref locking.
- **Remedy:** create the default host context once, under a process-wide lock or at bridge import, before any session opens; have `open` accept only the host context passed explicitly or the bridge's default.
- **Regression test:** 32 Clients opened concurrently from 32 threads share one host context (one supervisor thread; one helper budget).
- **Classification:** bounded correction.

## 1a. Closure of prior findings and safety check of the Consistency dispositions

| Finding | Status | Revised location | Original sequence on revision 2 |
| --- | --- | --- | --- |
| P2-8 `remote_identity` classified R while taking the mutator lock and writing config | CLOSED | §4.2 line 141, §5.1 table line 201, §15.4 lines 572–573; RemPlan-1 B10 | Does not reproduce: `remote_identity get` queues behind a running materialize and then answers; `set`/`unset` queue as W; no W fails "already held" because of it. The other seven direct methods were verified in round 1 not to take the lock; §15.4 adds the core test. |
| P3-6 detached worker keeps the `flock`; attribution; no way to wait | CLOSED | §5.1 lines 205–206, §5.6 lines 297–307, §8 step 5 and line 400, §15.9 | Does not reproduce for sessions sharing the host context (the retry queues until the worker ends); a session with another host context gets a refusal naming possible holders. Residual: P3-23 (live workers of other sessions are not covered). |
| P3-7 control frames behind admission resolution | CLOSED | §5.1 lines 186–193, §8 step 2, §15.5, §16 | Does not reproduce: control frames and reads are handled on the reading thread; a cancel settles a pending call before effect; close does not wait beyond the bound for a stuck admission thread. Residual: a call whose own resolution hangs is settled only at the bound (disclosed); direct calls still wait behind the admission thread (§3 below). |
| P3-8 delivery mark vs consumption | CLOSED | §5.4 line 255 (every declared view; merge = result + response), §10 lines 451 and 470, §15.7 | None of the three sequences reproduces. Residual: P3-26 (the bridge copy is unbounded). |
| P3-9 detached terminal indistinguishable | CLOSED | §6 line 362, §8 step 5, §15.9 | Does not reproduce: a second `errors` entry marks detachment. |
| P3-10 snapshot confidentiality and lifetime | CLOSED | §5.6 lines 290–292, §10 line 446, §12 line 486, §15.8 | Does not reproduce: secret-bearing rule, lossless capture, drop at session end, host binary started with the snapshot. |
| P3-11 child processes inherit the live environment | CLOSED | §5.6 line 293, §5.7 row, allowlist `process` entries (five sites, including `git rev-list`, which my finding missed), checker lines 379–388, §15.8 | Does not reproduce for gwz-core's own spawns. Residual: P3-24 (the `git2` crate's credential-helper child). |
| P3-12 gwz-transport outside the ratchet | CLOSED | §5.7 lines 323–328; gwz-transport allowlist, test and workflow; checker run passes with one `permanent` item | Does not reproduce. |
| P3-13 non-increasing `call_id` | CLOSED | §3 line 100, §4.1 line 132, O2 line 66, §15.1 | Does not reproduce: any non-increasing id ends the session. |

RemPlan-1 dispositions for the Consistency findings, judged for safety:
- **P3-14 receipt before admission:** no safety problem; it is what closed P3-7. Two notes: resolution must run outside the table lock the reading thread takes for control frames (implementation guidance, §3), and direct calls are not exempted from waiting behind the admission thread.
- **P3-15 `diff.output` release rule:** creates P3-25 for logs no reader ever opens; otherwise sound.
- **P3-16 gated appends and registry-owned spools:** no problem; it makes close step 7 safe by construction.
- **P3-17 reply kinds:** no problem; a cancelled unary call's `SessionError{cancelled}` fits the bridge's cancel-and-join.
- **P3-18 `call_id` monotonicity:** no problem; it closed P3-13.
- **P3-19 O9 wording:** no problem.
- **P3-20 O7 scope and deprecations:** no problem; §16 lists the two legacy handles.
- **P3-21 host-context scope:** closed P3-6 as far as detached workers go; leaves P3-23 and P3-27.
- **P3-22 request messages for scalar methods:** no problem; `OperationCancelRequest` "exactly one of the two set" implies an `invalid_request` for zero or two, which the text should say (a nit for the other axis).

## 1b. Rulings on the six applied choices

1. **§13 reply messages (`CleanupReport`, `OperationReleaseResponse`).** Within: the plan's P3-22 disposition defines request bodies; replies are the necessary complement, append-only. `pending_local_work` narrows from `usize` to `u32` on the wire, which is harmless.
2. **Five spawn sites, four `debt`, `gh` `permanent`.** Within: the plan said the check flags every production `Command::new`; listing what it found is the mechanism. The fifth site (`git rev-list`, `commit_log/mod.rs` 320, on the `log` path) is one my P3-11 missed; the disposition covers it. P3-24 is the remaining spawn the check cannot see.
3. **`operation_id` assigned at receipt.** Within (the P3-14 disposition names it). Pending records are bounded by the outstanding-call limit; a refused call's id is spent and a cancel on it reads `operation_expired`, which the bridge treats as finished.
4. **Per-process default host context held by the bridge; `NativeCoreBridge` accepts one; `Client` unchanged.** Within (the P3-21 disposition says exactly this). The driver-held default is outside O9's scope by design. P3-27 asks for the creation discipline.
5. **gwz-transport CI checks out gwz-core's default branch.** Within: the plan required "a gwz-core checkout beside it". Note that the same workflow pins its taut-generator checkout to a ref (line 21) but not gwz-core, so the guard's rules and gwz-transport's CI outcome follow another repository's HEAD; a robustness point, not a safety defect.
6. **O9 names libgit2's timeout as the permanent example; §5.6 heading.** Within (the P3-19 disposition).

No choice goes beyond RemPlan-1's dispositions.

## 2. Invariant analysis

- **O1–O3, O7 at receipt.** Records, tokens, gates and `operation_id` exist when the frame is read; a cancel before `accepted` finds them; refusal at admission settles the record before effect; close settles pending calls itself at the bound. One terminal per operation holds (host-written for never-started and detached cases). The refusal path spends ids cleanly.
- **Control on the reading thread.** Cancels, close and log reads no longer wait behind resolution. The text does not say that resolution runs outside the table lock the reading thread uses to settle cancels; if an implementation resolved under that lock, the round-1 stall would return (§3).
- **Admission and classes.** Still no deadlock: member locks are single-resource and now blocking across sessions sharing a host context, a wait in progress wakes on cancellation, one push per workspace, W excludes N and W per session, direct methods take no mutator lock (verified for all seven; `remote_identity` moved to W). Cross-session W serialization is the gap (P3-23).
- **Host context.** One per driver process by default; member lock manager and detached-worker registry shared; SSH budgets and the single supervisor loop hold per process as the agent design states; drop stops the supervisor once jobs finish (bounded by the jobs' own deadlines). The default's creation race is P3-27. A detached worker that never returns makes later W on its workspace wait without bound, but that wait is cancellable and the alternative (refusal) was round 1's complaint.
- **Retention and delivery.** Every declared view must be delivered before eviction; the bridge keeps cancelled result replies; stream helpers deliver or release in `finally`; `open` validates that the table holds running plus queued. The bridge copy's bound is P3-26; the open-log limit's zero-reader case is P3-25.
- **Gates, logs and spools.** Appends, seal and close are gated; the registry owns spool handles; after revocation appends are ignored, so close step 7 deletes spools safely. A detached `log` producer cannot write a released spool.
- **Environment.** Driver capture, lossless bytes, secret-bearing rule, drop at session end, `env_clear` plus snapshot for gwz-core's four `git` spawns and `gh`; the checker's `process` kind catches new `Command::new` sites (unit-tested for `std` and `tokio` forms). What remains outside: the `git2` crate's credential-helper child (P3-24) and libgit2's own configuration reads, which are process-wide by nature.
- **Ratchet.** All three repositories pass with the inventories the contract cites (29, 10, 1); unit tests pass; the lists can only shrink. gwz-transport's test fails hard when no gwz-core checkout is found, which is the right default for CI.
- **Process exit.** Finalizer closes and joins within the close bound; detached workers may be torn by exit (disclosed); the per-process host context is dropped at interpreter exit after sessions close.
- **Status quo.** Nothing found worse than today beyond what §16 discloses. P3-23 and P3-24 are status-quo behaviours that the revision's text now describes incorrectly or incompletely; P3-25 to P3-27 are bounded gaps in new mechanisms.
- **Wire proof.** The host binary is now started with the client's snapshot, removing the one adapter difference round 1 noted; the two runs still cannot show process-global coupling, and the check is the stated cover.

## 3. Risks and next action

Residual risks below the finding bar:
- Resolution should run outside the table lock that settles cancels and starts queued operations; the text implies it but does not say it.
- Direct calls (`status`, `transport_capabilities`) are not among the frames exempted from waiting behind a hung admission thread; under a hung resolution the `Client`'s per-call capability precheck stalls too. A hung resolution dooms the process on that filesystem anyway.
- gwz-transport's CI pins its taut-generator checkout but not gwz-core, so a checker change on gwz-core's default branch can fail or weaken gwz-transport's guard without a gwz-transport change.
- The Taut message `CleanupReport` shares its name with core's Rust `transport_host::CleanupReport`; the generated type will clash in gwz-core unless renamed or namespaced (the other axis's domain).
- A detached worker that never returns holds its `flock` and its host-context registration until process exit; sharing sessions wait, other processes see the lock held (disclosed in §16).

Next action: accept the design contract at this tuple, carry P3-23 to P3-27 into the implementation plan as bounded corrections (the two text-only ones, P3-23's attribution sentence and P3-24's exception, can land with the plan; the rest are implementation rules with the regression tests above), and keep the contract a draft until the implementation phases the proposals' §9 lists reach their own gates.
