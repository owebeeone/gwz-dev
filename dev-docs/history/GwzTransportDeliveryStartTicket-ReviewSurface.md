# Start-ticket Python caller guide — surface review

**Object:** `dev-docs/GwzOperationStartTicketCallerGuideDraft.md`  
**Date:** 2026-09-23  
**Axis:** Python caller experience, reviewed without implementation or design documents  
**Verdict: NO-GO** for the draft caller contract. This is a documentation and proposed API verdict; no implementation or release was reviewed.

**Baseline verified at start and end:** workspace `2ac74b10668e25e63eec83e258e17aeb91cdf91d`; `gwz-core` `590eefe26a0be6b59fda1f190a94d927b88938bb`; `gwz-py` `45bcd7b3ea102ca927935cee1b41b43934d68140`; `gwz-transport` `46e65a9a888fbd4a5bbeace946996581dcf23333`.

**Evidence base:** The draft guide at the stated workspace commit and the user-facing `gwz-py/README.md` at the stated Python commit. No code, design, plan, or other review was examined.

## Findings

### P2 — A start that never yields a handle has no complete caller lifecycle

**Location:** Guide lines 5, 27–29, 63, 65, 79, 81, and 88.  
**Invariant:** Every retained, capacity-charged start needs a way for its owner to learn its final state, observe pending cleanup, and release it or know precisely when it expires.  
**Sequence:** A caller starts a ticket, then cancels it while admission is pending; alternatively, `accepted()` receives `RetainedCapacityFull`. Neither path necessarily yields an `OperationHandle`. The guide defines `wait_cleanup()` and `release()` only on that handle, yet says completed tickets should be released to recover one of 64 Client ticket slots. It does not define a ticket-side final status, cleanup observation, release method, or ticket expiry for these paths.  
**Impact:** A caller cannot determine when pre-accept cleanup is final or how to reclaim a slot after a handleless outcome. Repeating this path can fill the Client’s ticket capacity with no documented recovery action.  
**Correction:** Define the terminal and cleanup states of `StartTicket`, including what `accepted()` and `result()` raise after pre-accept cancellation or refusal. Specify a ticket-side release or an explicit automatic slot-expiry rule, with its timing.  
**Regression test:** Fill admission capacity, obtain handleless refused tickets, and cancel another ticket during admission. Verify each exposes final or pending cleanup, can be recovered by `start_seq`, and frees its ticket slot through the documented rule.

### P2 — Recovery after a cancelled acceptance await does not guarantee access to the handle

**Location:** Guide lines 27, 48–61, and 65.  
**Invariant:** Cancelling an await must not lose the operation handle needed for events, cleanup, and release if acceptance won the race.  
**Sequence:** `a.accepted()` completes as its waiter is cancelled. The example retains `a` and shows `a.cancel()`, but does not state that `accepted()` may be awaited again to retrieve the same handle. `a.result()` returns a response, not a handle. `Client.ticket(start_seq)` retrieves the ticket, but the guide does not say how that ticket re-exposes an already accepted handle.  
**Impact:** A caller can retain the start identity yet lack a documented way to read events, observe cleanup, or release the operation terminal.  
**Correction:** Promise that `accepted()` is repeatable and returns the same operation identity after waiter cancellation, or provide an explicit ticket method for retrieving the accepted handle. Show that recovery in the cancellation example.  
**Regression test:** Cancel a waiter at the acceptance-delivery race, recover the ticket through `Client.ticket(start_seq)`, obtain its handle, read its outcome, and release it while a second ticket completes unaffected.

### P2 — `StartUnknownOrExpired` has no specified inspection surface

**Location:** Guide lines 63, 69, and 83.  
**Invariant:** Once commit may have been sent, an unknown status must expose enough structured evidence for the caller to avoid treating uncertainty as a safe retry.  
**Sequence:** A commit reply is lost and owner-checked status later becomes unknown or expired. The guide says the ticket reports `StartUnknownOrExpired` “with effect uncertainty for caller inspection.” It does not say which await raises it, which field carries the effect assessment, or whether a handle, terminal, or cleanup progress can exist in this state. The documented `effects` fields belong to `OperationCleanupProgress`, normally reached through a handle that the caller may never receive.  
**Impact:** A caller cannot write the promised recovery branch or reliably distinguish a proved no-effect expiry from a possibly effectful committed start.  
**Correction:** Specify the exact exception/result shape and method for this state, including structured commit and effect evidence and the required manual inspection action when evidence remains uncertain.  
**Regression test:** Lose the commit reply and status until expiry. Assert the documented method returns the documented typed state and effect fields, and that a caller cannot accidentally interpret it as a safe automatic retry.

### P3 — The cancelled-wait example leaves its successful peer’s terminal charged

**Location:** Guide lines 48–61.  
**Invariant:** An example demonstrating independent tickets should show the lifecycle pair for every successful handle it creates.  
**Sequence:** The example obtains `b_response = await b.result()` and ends without retrieving and releasing `b`’s handle. Repeating the pattern retains terminals and ticket slots until expiry.  
**Impact:** Copying the example into a sustained caller can produce avoidable capacity refusals.  
**Correction:** Obtain `b`’s handle and release it after consuming the result, preferably in `finally`; show the documented completion path for `a` once the handleless-start contract is settled.  
**Regression test:** Run the example pattern repeatedly beyond 64 starts and verify that completed peers do not exhaust ticket capacity.

No P0 or P1 finding is supported by this evidence.

## Invariant analysis

| Caller scenario | Draft contract | Remaining gap |
| --- | --- | --- |
| Two default tickets | Distinct `start_seq` values and owner-scoped cancellation are clear. | The second example omits peer release. |
| Cancel a pending start | Commit prevention before permit is clear. | Final state, cleanup observation, and slot release without a handle are undefined. |
| Cancel an acceptance await | The ticket survives waiter cancellation. | Repeatable handle retrieval after an acceptance race is not promised. |
| Recover a ticket | `tickets()` and `ticket(start_seq)` preserve identity on the same Client. | Recovery of the accepted handle or handleless final state is incomplete. |
| Result, events, cleanup, release | Handle methods, event gaps, local completion proof, and terminal retention are described. | These methods cannot cover a start that never yields a handle. |
| Close with pending work | `ClosePending`, later `close()`, and retained same-Client reads are described. | A handleless ticket still lacks its own observation path. |
| Full capacity | Error classes and several retry conditions are described. | Reclaiming a completed, handleless Python ticket slot is unspecified. |
| Unknown server status | The guide correctly forbids automatic resubmission after possible commit. | The caller-facing uncertainty value and access path are unspecified. |
| Event-loop shutdown | The guide warns that streams fail and ownership remains. | It offers no reattachment path; callers must preserve the loop and Client long enough to inspect or close work. |

**Next action:** Complete the `StartTicket` state and recovery contract, including repeatable acceptance, handleless cleanup and release, and the structured `StartUnknownOrExpired` outcome. Then update the examples to exercise those paths.

