# Python transport session v3 foundation — SAFETY-AXIS RE-REVIEW

Review object: committed DRAFT `dev-docs/GwzPyTransportSessionV3FoundationDesign.md`, with its draft core requirements/design paragraphs and Python caller-guide note.  
Baseline SHAs: root `21fac9f4236cd12df7719a92f039ae5cd787f067`; gwz-core `e7b4c499a2e5e2bfe8be0db2fbe067d6599a506f`; gwz-py `f6ae40afaa67ddaa561002dcee54d3c522739dd1`. All three HEADs matched at the start and end.  
Date: 2026-09-24. Axis: Safety. Read-only design review; no files, Git state, tests or builds were changed.

**Verdict: NO-GO — two new P2 findings.** The five prior safety counterexamples have explicit design corrections. Two ownership boundaries still permit an issued operation to remain unsettled or a completed admission’s watchdog to close its live generation.

## Prior-finding closure table

| Prior finding | Claimed disposition and original counterexample re-traced | Status |
| --- | --- | --- |
| S-P1-1, dead placement supervisor strands phase 2 | Section 6 now starts a wall-clock watchdog independently of mux `drive()`, requires supervisor exit to close the generation, and wakes `ready()` and `finish()`; §11 gives the stopped-supervisor case a bounded consumed refusal. The original sequence—supervisor exits after `begin()` while `ready()` waits—has a stated termination path. | Closed in the revised design for that sequence; implementation and fault-injection proof remain required. The separate completion race is P2-2 below. |
| S-P2-1, handler panic settles before finish | Sections 5.2 and 7.2 catch handler unwind while retaining `TransportRequest`, then await `finish()` before publishing `Failed`. Drop with unconfirmed cleanup is reserved for finish unwind or lost ownership. | Closed in the revised design; the pending-finish panic test in §12 remains an implementation gate. |
| S-P2-2, release erases an owed close summary | Close marks the exact live set at entry (§8). `settle` copies each owed summary while holding the session mutex, before release can remove its record (§9). | Closed in the revised design; the release/close race test in §12 remains an implementation gate. |
| S-P2-3, mux tombstone precedes witness update | The witness is armed `MayHaveRegistered` before entering each mux registration. An unwind after insertion closes the generation and publishes no false retry permission (§6). | Closed in the revised design; the post-tombstone unwind test in §12 remains an implementation gate. |
| S-P2-4, cancellation can invert lock order | Section 8 now explicitly takes the session mutex first and invokes core cancellation after releasing locks, consistent with §7.4. | Closed in the revised design; the lock-order assertion and competing cancel/settle/close test remain implementation gates. |

## Changed-range analysis

The root diff from `b35ea74bf7e73c15777a3e0fb18587d05faffb17` changes the V3 draft, records the first reviews and remediation plan, and updates the workspace member pins. The core diff from `58e25012449ee8f4609daba4939e7157e99ea488` changes the draft V3 paragraphs in `GWZDesign.md` and `GWZRequirements.md`. The Python diff from `d29d450508138bda9251e797afdb21d71d20d8dc` changes the caller-guide retry note.

**NEW ARCHITECTURAL — P2-1:** Issuance remains on the Python side, while the only proposed settlement guard begins after native entry. A failed Python worker submission can cross that gap without a `Claim`.

**NEW ARCHITECTURAL — P2-2:** The independent watchdog can mutate a generation, but the design gives no single outcome handoff between watchdog expiry and phase 2 returning `Ready`. Its lifetime can overlap accepted ownership.

## 0. Evidence base

I read `AGENTS_GWZ.md` and `EVIDENCE.md`; `AgentProcessRules.md` as amended by `GwzProcessOptimization.md`; the accepted V2 design, V2 implementation checkpoint and third verdict; the first V3 verdict, prior Safety report and merged remediation plan; the changed V3 draft and paired core and Python docs; and relevant committed core, mux and Python bridge source. Source describes the candidate’s starting point, not an implementation of V3.

Read-only commands included `git rev-parse HEAD` for all three repositories at start and end; `git diff prior..current` for the draft and member docs; `git diff --` to confirm the reviewed draft and prior reports had no working-tree changes; and `rg`, `nl -ba` and `sed` for cited sections. No test or build was run, as required by this design review.

## 1. Findings

### P2-1 — A Python submission failure leaves an attempted issued operation unclaimed

**Location:** V3 §§7.2 and 10 (`GwzPyTransportSessionV3FoundationDesign.md:195–199, 236–238`); existing bridge `gwz-py/src/gwz/bridge.py:330–365, 385–410, 418–447`. V2 §2 (`GwzPyTransportSessionV2Design.md:13–17`) requires one-shot `accepted()` to terminalize a pre-effect refusal.

**Violated invariant and reproduction:** V3 puts `Claim` at the first statement *inside* native `py.detach` and says the bridge’s issuance and pre-entry cancellation path remain unchanged. The bridge creates `asyncio.to_thread(native_call, ...)` at line 359. If the loop’s default executor has been shut down, that task raises before invoking native `call` or `submit`. `_run_native` attaches the operation ID to the exception and rethrows it; the outer method converts it to a bridge error. No native `Claim` existed, and no path settles the issued record. An explicit handle can likewise encounter a bridge-side encoding failure before native entry.

**Impact:** An attempted admission returns an error while its record remains `Issued`. Its retained result cannot agree with the returned refusal; repeated failures can occupy the 64-record limit until manual release or close. The claim guard cannot prove terminality across this earlier boundary.

**Required correction and closure test:** Give the Python issue-to-native-entry interval an explicit settlement owner. On a failure proved to occur before native claim, settle the issued ID with the same typed, nonconsumed refusal returned to the caller. Make the handoff to native claim atomic or idempotent so a racing worker cannot produce a second terminal. Shut down an event loop’s default executor, then attempt both handle admission and implicit `call`/`submit`; verify one typed retained terminal per issued ID, matching returned errors, zero slots and claims, and no record-limit buildup after repeated attempts. Cover bridge-side encoding failure for an already-issued handle.

### P2-2 — Watchdog expiry and `Ready` have no exclusive completion boundary

**Location:** V3 §6 (`GwzPyTransportSessionV3FoundationDesign.md:144–158`), native admission flow (§7.2, line 199), and the accepted handoff (§7.3, lines 205–214). The paired draft core requirements require watchdog expiry to close and wake the generation.

**Violated invariant and reproduction:** Core starts an independently acting watchdog before registration. The text requires expiry to close the generation, while the admission caller may return `Ready` after `ready().await` and hand the `TransportRequest` to an accepted worker. It specifies neither a disarm/join step nor an atomic race decision. Resolve `ready()` at the five-second boundary while pausing the watchdog callback just before its close action. Phase 2 can return `Ready`, native can write `Accepted` and open the worker gate, and the already-triggered callback can then close that generation. The reverse ordering can allow a `Ready` return after closure unless readiness checks the same decision.

**Impact:** A successful admission can lose its transport generation after Git work is permitted, or publish `Accepted` for a closed generation. Cancellation, close and result attribution then depend on timing rather than the stated `Ready` versus `Consumed` boundary.

**Required correction and closure test:** Define a one-shot phase-2 outcome shared by the watchdog and admission path. `Ready` must win only while the generation is still open, and winning must disarm or join any callback that could close it; expiry must win before `Consumed` and prohibit a later `Ready`. Hold the watchdog callback at its close point while `ready()` completes and repeat with the order reversed. Assert exactly one outcome: either a live accepted request whose generation remains open, or a consumed refusal with conservative cleanup. No delayed watchdog may close a generation after `Ready` wins.

## 2. Invariant analysis

The revised witness prevents a hidden mux insertion from becoming a false `request_id_consumed=false` refusal. Catching handler panic before `finish()`, retaining close summaries before release, and taking session before record locks resolve the prior safety sequences at the design level. The new gaps occur on either side of those proofs: Python may fail before `Claim` exists, and an external watchdog may act after phase 2 has handed ownership to `AcceptedOperation`.

The watchdog’s independent clock correctly addresses a dead placement supervisor while `ready()` is pending. To make that bound safe, timeout must also be exclusive with successful completion. The Python entry boundary similarly needs a settlement owner covering the interval from issuance until native `Claim` takes ownership.

## 3. Risks and next action

Keep the V3 foundation at **NO-GO**. Specify and re-review the Python issuance handoff and the watchdog/`Ready` race decision on a new exact tuple. The deferred ledger, handle, rollover, stress, platform and wheel gates remain separate; this report neither accepts implementation nor changes their outcomes.