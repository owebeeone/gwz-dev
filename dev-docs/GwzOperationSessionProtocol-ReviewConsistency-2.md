# GWZ operation-session protocol — focused Consistency re-verdict

**Date:** 2026-09-23  
**Verdict:** **NO-GO** — 0 P0, 0 P1, 1 P2, 0 P3. This is a design verdict only.

The read-only review covered the committed protocol design and caller guide in `gwz-dev`, and the paired draft capacity amendment in `gwz-core`. The exact tuple matched at both the start and end:

| Repository | Commit |
| --- | --- |
| gwz-dev | `c5b497bc0934b8970d5abba7022b5da020ff7f72` |
| gwz-core | `d3951dcfc04d2f09c9c7a025dec41ee3163c4058` |
| gwz-py | `45bcd7b3ea102ca927935cee1b41b43934d68140` |
| gwz-transport | `46e65a9a888fbd4a5bbeace946996581dcf23333` |

I read the round-2 remediation plan and controlling committed clauses and source. I made no changes, ran no builds or tests, and did not inspect a current-round peer prompt or report.

## Prior-finding closure

| Prior Consistency finding | Corrected-text result |
| --- | --- |
| P2-1 — incomplete capacity supersession | **Closed.** The core amendment now replaces retry §3 item 7’s entire installation rule, both relevant §6 bullets, and S1.4’s first three sentences. They agree that an admitted operation pins the capacity epoch before its first lease. The held-before-Open A / lower B / higher B counterexample has one outcome: lower B may join without resize; higher B refuses; a later quiescent operation may install new caps. |
| P2-2 — caller ID can exceed the fallback reservation | **Closed at the design gate.** Admission rejects IDs above 256 UTF-8 bytes before `Accepted`; generated IDs and fallback text are bounded, mandatory fallback fields are limited, and a generated-schema measurement must verify the encoded charge before activation. A 256-byte ID fits the stated contract; a 257-byte ID refuses. |
| P2-3 — failed response loses the typed action response | **Closed.** Failed handle, unary and stream calls now attach the decoded generated action response to `GwzOperationError.response`, including conflicted merge recovery fields. The output-limit case explicitly has `response=None` and an effect-bearing fallback. |

## Changed-range analysis

I compared root `ee11f44efa5a0796a71d874ffaa4bc570a611a4e..c5b497bc0934b8970d5abba7022b5da020ff7f72` and core `8756fd6b32443b0ac63287ee5b4a3335e8cf0894..d3951dcfc04d2f09c9c7a025dec41ee3163c4058`. The core change closes the exact supersession gap. The root and guide changes add caller-ID bounds, typed failure parity, owner binding for legacy reads, cheap session-open negotiation, event-cursor behavior, a public fetch signature, and a 30-second local-handler cancellation qualification.

The new blocking issue is in the changed orphan-lifetime claim. It combines that 30-second handler bound with the existing five-second transport cleanup timeout and treats the latter as completed cleanup. The controlling Python transport design and committed core cleanup path allow a cleanup **report with work still pending**.

## §0 — Finding

### P2-1 — **NEW ARCHITECTURAL ROOT CAUSE:** a timed-out cleanup report is counted as completed orphan cleanup

**Location:** `dev-docs/GwzOperationSessionProtocolDesign.md:211–215,363–378`; accepted `gwz-py/dev-docs/GwzPyTransportDesign.md:90–95`; committed `gwz-core/src/transport_host/session.rs:30,656–679`.

**Violated invariant and reproduction:** The protocol says a lost route keeps its session charged until cleanup completes. The correction also promises that qualified local workers finish within 30 seconds, “bounded endpoint cleanup takes at most five more seconds,” and the orphaned session charge retires within 35 seconds. The accepted Python design expressly preserves nonzero `pending_local_work` in the final cleanup report. Core’s cleanup routine reaches its five-second deadline and may return that nonzero report; the deadline does not establish that the pending physical work has finished.

Admit a qualified operation, block physical disposal, then lose the owner or route. Its handler satisfies the 30-second bound, but transport cleanup reaches five seconds with `pending_local_work > 0`. At 35 seconds, retaining the session charge violates the new retirement promise; releasing it violates the design’s requirement to keep unfinished cleanup owned and charged. Repeating the close call cannot turn the timed-out report into proof that the work joined.

**Impact:** The receiver either strands an orphan charge beyond the advertised bound or claims final cleanup while physical work remains. Filling the 32 session slots with this state defeats the remediation’s abandoned-route recovery case.

**Required correction:** Freeze an ownership contract for residual physical work. Either prove and enforce actual physical termination within a published bound, or transfer still-pending work to a separately bounded, charged cleanup owner with explicit session-retirement and reporting semantics. A five-second return of `CleanupReport` alone cannot serve as termination proof.

**Closure/regression test:** Block physical disposal past five seconds while a qualified handler terminates and the route is lost. Assert that every pending worker retains a charged owner, no final report asserts completed cleanup prematurely, and receiver capacity recovers within the published bound. Exercise repeated close and owner-loss races.

## §1 — Verdict and cap

**NO-GO** because P2-1 is open. **P2-1 is a new architectural root cause**, not an omitted sentence in the prior three corrections: the design needs an owner for work that outlives the existing cleanup deadline. Under the adopted two-round remediation cap, this finding stops another bounded correction loop and calls for a redesign and re-freeze of that ownership boundary. This verdict does not assess implementation or release readiness.
