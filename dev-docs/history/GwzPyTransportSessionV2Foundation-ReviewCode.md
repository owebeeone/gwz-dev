# Python transport session v2 candidate foundation — CODE-AXIS REVIEW

**Review object:** Candidate foundation diff from root `78a46ef58bdfc3fac0c5a6600597465c9436c20d`, core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`, and Python `cddb38204fbbd11808cc3c414807aa66a5910ce0` to the settled tuple below.  
**Baseline:** Root `9cb11fd5d561ee108525357cdaafc11af611a1371`; core `dfb7533ba8cd2eba59fc1d8ba8365a74b7413371`; Python `33a1f3f4e2ab9a7b97fd03246f7bb3f1905dfead`. Sources were read with `git diff`, `rg`, `sed`, and `cat`. All three HEADs matched at the start and end.  
**Date:** 2026-09-24  
**Axis:** Architecture, interfaces, call graphs, and compatibility reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P1 and three P2 findings block. I pre-commit to GO on a revision that resolves P1-1 and P2-1 through P2-3 as specified, provided the focused closure tests confirm the original counterexamples.

---

## 0. Evidence base

I read the accepted `dev-docs/GwzPyTransportSessionV2Design.md` §§1–8 and `dev-docs/GwzPyTransportSessionV2ImplementationCheckpoint.md`; the core diff and `src/transport_host/{mod,request,session}.rs`; the Python diff and `native/src/{transport_session,operations,dispatch/mod,lib}.rs`, `src/gwz/{bridge,client}.py`; the generated enum projections; and the changed core, native, and Python tests. I inspected the process authority in the review-loop skill and canonical report template. No files were changed and no builds or tests were run.

The checkpoint explicitly defers the full byte ledger, expiry timer, public `start_*` handles, rollover, and platform gates. Their absence is not a finding here. The findings concern behavior already present in the candidate call paths and count-limited ledger.

## 1. Findings

### [P1-1] Pre-start cancellation can launch a second, unowned operation

**Location:** `gwz-py/src/gwz/bridge.py:339–370`; `gwz-py/native/src/transport_session.rs:284–290, 725–732, 413`.

**Violated invariant:** Cancelling an issued operation before native admission must produce no effects, and the later worker must remain bound to that same public operation ID.

**Counterexample:** Fill Python’s default thread executor so the `asyncio.to_thread` call is queued. `_run_native()` reserves public ID A, then the Python task is cancelled. `cancel_inner(A)` sees only the request mapping, removes it, and reports pre-admission cleanup. When the queued native call finally starts, `operation_for_request()` finds no mapping and reserves ID B for the same request. That call may proceed to core registration and Git work. `_run_native()` waits for the worker, then raises cancellation for A; the caller is not given B or its outcome. The existing queued-worker test uses a fake session and does not exercise this native mapping transition.

**Impact:** A pre-effect cancellation can be followed by an unreported push or other network effect. The original ID cannot retrieve or cancel that work.

**Required correction:** Keep cancellation authority attached to the queued call. Pass its public ID into the native invocation, or retain a cancelled reservation/tombstone until that specific queued invocation acknowledges it and refuses before registration. Do not infer identity again from a request ID after cancellation.

**Closure test:** Queue a real native `call()` and `submit()` behind a blocked executor, cancel before either native entry point runs, then release the executor. Assert no core registration or Git effect occurs, no replacement ID is issued, and the original ID has the retained pre-effect cancellation outcome.

### [P2-1] Native Client lookups can fall through to the process-global legacy store

**Location:** `gwz-py/src/gwz/bridge.py:252–260, 469–518`; `gwz-py/native/src/lib.rs:183–203`; `gwz-py/native/src/operations.rs:11, 62–75`.

**Violated invariant:** Every native-session lookup must verify that Client’s nonce and issued serial before reading that Client’s ledger. The module-level compatibility store is separate.

**Counterexample:** `_operation_source()` returns the module whenever the current session says an ID is foreign or never issued. If the legacy module store contains a completed `op_<request-id>` record, a native Client that supplies that ID reads the legacy result through `Client.operation_result()`, event lookup, or merge-response lookup. Even with no matching legacy record, a foreign ID receives a generic module-store error instead of the session’s typed `InvalidRequest`.

**Impact:** Native Client result ownership is not enforced at the Python routing boundary; compatibility records can cross into a session lookup.

**Required correction:** When a bridge has a current native session with `issued_operation`, route every session ledger lookup to that session and let `validate_operation_id()` classify foreign, never-issued, and expired IDs. Use module-level functions only for a bridge that has no native session.

**Closure test:** Create a module-level legacy result and two native Clients. Verify neither Client can read the legacy ID or the other Client’s ID through result, event, or merge lookup; both must receive `InvalidRequest`. Verify the module-level compatibility call still reads its own record.

### [P2-2] Pre-effect failures retain unreachable records and can exhaust the 64-record ceiling

**Location:** `gwz-py/native/src/transport_session.rs:259–281, 413–420, 460–485, 543–564, 890–907`; `gwz-py/native/src/operations.rs:84–100`; `gwz-py/src/gwz/bridge.py:339–370`.

**Violated invariant:** A pre-registration refusal must permit a new attempt with the same caller request ID once the conflict clears; retained issued records must remain discoverable or releasable.

**Counterexample:** A direct `call()` allocates a ledger record before native admission. On a capacity conflict, it removes the request-to-operation mapping and returns the error, but `end_admission()` does not terminalize or discard the record. The normal Python helper does not expose the internally issued ID on that error. Repeating this refusal fills the 64-record store; a later retry then fails `TransportSessionFull` even after the capacity conflict clears. The `submit()` path writes a failure result, but likewise returns its pre-acceptance error without exposing the issued ID to the ordinary caller. The deferred `recent_operations()` API cannot recover these records in this candidate.

**Impact:** Transient, pre-effect refusals can make a live Client unable to retry until it is closed. A retained direct-call record can also leave a result waiter blocked indefinitely.

**Required correction:** Terminalize every issued pre-effect record. Make its ID available through the error or an already-returned handle when retention is intended; otherwise release the internal helper’s record after preserving the typed refusal and issued high-water history.

**Closure test:** Repeatedly trigger more than 64 pre-registration capacity refusals through each normal Python call form, clear the conflict, and verify a retry with the same request ID can be admitted. For any intentionally retained refusal, verify its ID is available and its result lookup terminates with the retained typed error.

### [P2-3] Explicit CLI placement constructs the local endpoint before refusal

**Location:** `gwz-py/native/src/transport_session.rs:413–465`; `gwz-core/src/transport_host/mod.rs:187–205`; `gwz-core/src/transport_host/request.rs:369–379`.

**Violated invariant:** Design §3 requires a typed internal request for explicit CLI placement to receive `UnsupportedOperation` before runtime construction, credential access, or mutation.

**Counterexample:** `call_inner()` validates the metadata, then calls `runtime()` before passing it to `TransportRuntime::request()`. `runtime()` constructs the local endpoint from the environment. With `HOME` absent, an explicit CLI-placement request fails during endpoint construction with the endpoint-HOME error. With a valid environment, it constructs the endpoint before core observes that no CLI endpoint is installed and returns an unavailable error. Neither path provides the required early `UnsupportedOperation`.

**Impact:** Unsupported placement has environment-dependent behavior and can access endpoint configuration before refusal.

**Required correction:** Detect explicit CLI placement from the decoded request metadata in the native admission path, return typed `UnsupportedOperation`, and terminalize or release its issued record according to P2-2 before calling `runtime()`.

**Closure test:** Submit explicit CLI-placement requests through direct and asynchronous forms with `HOME` absent and present. Assert the same typed `UnsupportedOperation`, no endpoint construction or credential access, no core registration, and no stranded record.

## 2. Invariant analysis

The core capacity path passed the static call-graph attack: `admit_client_request()` checks for a different live capacity before `ClientRequest::new()` registers the endpoint request ID, while equal installed capacity returns without reinstalling pools. The new tests cover equal capacity under a held lease and a different capacity between leases. This is a source-level conclusion; tests were not run in this read-only review.

The native nonce plus serial distinguishes two Clients, and native lookup validates issued range and retained-record presence. The failure in P2-1 is the Python bridge’s choice to bypass that validator for foreign IDs. The generated GWZ error enum appends values 73 and 74 in the schema and both projections; I found no changed method set or enum-value collision. The outer worker path marks unconfirmed cleanup and wakes waiters on its handled panic path. The checkpoint does not claim physical cleanup proof for arbitrary panics, and I make no such finding.

## 3. Risks and next action

The deferred byte bounds, timer, public handles, rollover, and platform proof remain release gates. They do not excuse the four implemented-path defects above. Correct these roots in one patch, run the focused closure tests, and return the revised settled tuple for a Code re-verdict.