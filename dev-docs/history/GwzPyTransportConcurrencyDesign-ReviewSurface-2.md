# Python concurrent operations caller guide — SURFACE-AXIS REVIEW

**Review object:** `gwz-py/dev-docs/GwzPyConcurrentOperations.md` at `5541b851266da7267f49531b98c061af1fd670e1` (DRAFT, 2026-09-24)  
**Baseline:** `gwz-py` HEAD `5541b851266da7267f49531b98c061af1fd670e1`, verified at start and end. Compared with `124e50030afb6f7c0e8edd90c37838b0136981c2` using the guide-only diff.  
**Date:** 2026-09-24  
**Axis:** Independent, adversarial, read-only Python caller surface. Nothing here relies on another current-round review.

**Verdict: GO** — one P3 documentation finding; no P0–P2 finding.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| S1 | Define cancellation’s terminal result, effect, and completion guarantee. | Re-ran the cancelled-push walkthrough. Lines 21–25 inspect the cancelled handle; line 35 says `cancel()` joins cleanup and terminal publication, and `result()` then raises `GwzOperationError` with a `Cancelled` response and `effect="possible"` unless the operation completed first. It also warns against replaying the push. | Closed |
| S2 | Give `TransportRecordLimit` a phase, payload, and reconciliation path for unary callers. | Re-ran a unary push that raises this error. Line 49 limits it to after admission, identifies the exception and its operation/request IDs and available evidence, sets `effect="possible"`, and directs the caller to inspect and compare the remote ref before deciding what to do. | Closed |
| S3 | State direct cross-thread and event-loop rules. | Re-ran start on thread B, then cancel, result, and close on thread A. Line 53 explicitly permits this sequence and distinguishes a handle from a pending `asyncio.Task`. | Closed |
| S4 | State Python option placement, capacity defaults, and retry meaning. | Re-ran two default starts and a conflicting per-host setting. Lines 39–47 give the defaults, derived limits, three *extra* setup retries, and an admission example. | Closed |
| S5 | Define event replay and independence from `result()`. | Re-ran result-before-events and partial-events-before-result. Line 35 permits both: each iterator replays from the beginning, multiple iterators are independent, and events need not be drained before `result()` completes. | Closed |

## Changed-range analysis

The guide-only diff adds cancelled-result inspection to the walkthrough, specifies result and event sequencing, replaces the defaults paragraph with a Python-facing capacity table, expands record-limit recovery, clarifies close-time access, and adds direct cross-thread guidance. Those changes address S1–S5. The unchanged promise at line 35 that every network command with a stream form has a `start_<verb>` counterpart remains hard to apply to the compound names shown in the public README. That is a documentation discovery issue, not a new architectural root cause.

## 0. Evidence base

I read `gwz-py/dev-docs/GwzPyConcurrentOperations.md` at HEAD, its diff from `124e50030afb6f7c0e8edd90c37838b0136981c2`, `gwz-py/README.md`, `gwz-py/src/README.md`, and the prior Surface-1 report. I read `AGENTS_GWZ.md` for workspace instructions and the review prompt template for report format. I read **no source code, design or plan document, or other review**; I ran no imports or tests. `git rev-parse HEAD` returned the assigned SHA at both checks. The guide states at line 3 that these methods are not active in the current candidate, so this verdict concerns the proposed caller contract.

## 1. Findings

### [P3-1] The full `start_*` method family is not discoverable by name

**Location and invariant:** Guide line 35 promises `start_<verb>` for every network command with a stream form, but names only `start_fetch` and `start_push`. The README names the existing `clone_repo_member_stream(...)` method at line 91 and mentions clone, materialize, pull, and push stream forms at lines 38–39. A caller should be able to identify the exact handle-returning counterpart of a documented stream method without guessing how a compound method name is transformed.

**Reproduction and impact:** A caller using the README’s clone example wants an operation ID before progress begins. The guide establishes the purpose of `start_*`, but never explicitly identifies whether the clone entry point is `start_clone_repo_member` or another name, or whether it takes the same arguments as `clone_repo_member_stream`. The first-day path stalls at method selection. This is a bounded documentation defect while the API is still a draft.

**Required correction:** Add a compact mapping of each network stream method to its exact `start_*` name and state whether their operation arguments match. Include the clone counterpart used with the README example.

**Closure test:** Starting with only the README and caller guide, identify the exact method and arguments to start a clone, obtain its handle, cancel it, inspect its result, release it, and close the Client without guessing a name.

## 2. Invariant analysis

The two-operation walkthrough now starts independent fetches, cancels one, inspects its possible effect, obtains the other result, releases both records, and exits through Client close. The guide distinguishes operation IDs from request IDs, locates capacity errors before admission, and gives a recovery route for a possible-effect unary push. Event replay and result access are sequenced, and direct calls across event loops are explicitly supported. The S1–S5 counterexamples therefore no longer refute the documented contract.

The remaining discovery gap concerns the exact names of additional handle-returning methods. No claim about implemented behavior was tested or inferred from source.

## 3. Risks and next action

The draft status means runtime parity remains unverified by this docs-only review. Add the method mapping in P3-1 before publishing the guide as the caller entry point; the GO verdict has no blocking surface finding.
