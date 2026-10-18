# TR2.5 Python transport settings — STATE-AXIS REVIEW

**Review object:** TR2.5 Python implementation `b2369f1d0bf7..5bf260d040964a3dd9ec606b58a625bc74ac4afc`, including `gwz-py/dev-docs/GwzPyTransportSetting-Implementation.md` at the settled Python SHA. Status: implementation draft, not accepted. Workspace: `/Volumes/projects/limbo/gwz-dev-tr2-5-py`.

**Baseline:** Root `b256a791f1dd05c04caedd8391ef146ff4d3d8fa`; gwz-py `5bf260d040964a3dd9ec606b58a625bc74ac4afc`; gwz-core `2e64e88a28c332ed422cc390adc76738dc701bb1`. Changes were read with `git diff b2369f1d0bf7..5bf260d040964a3dd9ec606b58a625bc74ac4afc`; controlling documents with `git show` and supporting source with numbered reads. The tuple and statuses matched at start and end.

**Date:** 2026-10-03

**Axis:** State machines, mutation ordering, races, lock scope, failure direction, cancellation and cleanup. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1, P2 or P3 findings identified within the specified implementation scope.

---

## 0. Evidence base

Read the canonical `py-review/State.txt`, root `AGENTS.md` and `AGENTS_GWZ.md`, core `AGENTS.md`, process rules concerning independent reviews and actionable findings, `GwzProcessOptimization.md` §8, and `/Users/owebeeone/.claude/skills/review-loop/SKILL.md` with its canonical prompt template. Read the settled root checkpoint’s TR2.5 Python entry and `GwzTransportHandoff.md` §6.1.

Checked the implementation against:

- Core `GwzTransportOffSwitchDesign.md` §§2–4 and §§6–10.
- Release-plan TR2.5/TR2.6 clauses and amendment 2 §3.17.
- Python `GwzPyPerOperationTransportDesign.md` §§2–3.
- The complete Python implementation package.

Inspected the production diff and these supporting paths:

- `native/src/client_host.rs:32–208,230–269`: host-owned notices, both native entries, registration and execution.
- `native/src/route/transport.rs:29–265`: snapshots, detached resolution, diagnostics, native selection and metadata rewriting.
- `native/src/route.rs:17–25` and `route/native.rs:13–44`: candidate and ordinary boundaries.
- `native/src/client_host/operations.rs:119–395`: registration, admission, cancellation, close and ticket cleanup.
- `native/src/dispatch/mod.rs:138–227,448–600`: submit validation, recording, spawning and terminal failure paths.
- `src/gwz/_transport_notices.py:1–45`, `cli.py:74–154`, `cli_shared.py:341–349`.
- `src/gwz/client.py:185–209,271–367,421–427` and `bridge.py:216–354`: constructor policy, timeout ownership and exception mapping.
- New setting/notice tests, modified client tests, route unit tests, and existing host/transport integration tests.
- Core resolver, global-file lookup, repository scan and timeout code, as supporting dependencies rather than additional implementation under review.
- README’s transport-selection documentation.

Inspected the supplied `/tmp/tr25-py-*` receipts. Their terminal summaries report:

| Receipt | Recorded result |
|---|---|
| Ordinary full suite | 992 passed, 18 skipped |
| Both-switch full suite | 1,010 passed |
| Ordinary/candidate Rust unit suites | 25/29 passed |
| Ordinary/candidate Python-target Clippy | Finished successfully |
| Final ordinary focused suite | 56 passed |
| Final setting suite | 8 passed |
| Transport-only integration | 18 passed |
| Conditional-boundary guard | Nothing new |

These are supplied execution receipts; this reviewer ran no builds or tests. The package explicitly distinguishes broad runs from the rebuilt focused runs following its two lint corrections.

Independent SHA-256 checks matched `py-review/artifacts.json`:

- Both-switch final extension: `66bb05fd89a2409bfe9b25b3164f6d149eb43863c111f6b1f59c5d5bec1397ed`.
- Transport-only extension: `77d1704a066fd8b5d2d4fd876c698ac52da8782df473b1bccaf88424fded52f5`.

Python status was clean throughout. Root’s five untracked SSH-prompt/route-draft paths and core’s untracked private BugReport remained unchanged and excluded. No current peer report was read. No files, repository history or reviewed artifacts were modified.

## 2. Invariant analysis

**Refusal precedes registration and external effects.** Both `call` and `submit` invoke `network()` before dispatch. Setting refusals and warning exceptions propagate from route capture before `Operations::register`. Submit recording and worker spawning occur later still. A malformed setting or warning promoted to an error therefore creates neither a host ticket nor a submitted operation record. The supplied setting tests exercise both native entries; bridge exception chaining preserves the original warning as the cause.

**Detached resolution does not allow a closed host to admit new work.** I traced a close occurring while configuration resolution releases the GIL. Close marks the operations state as closing under its lock. When capture returns, registration checks that state and refuses. The capture phase owns only local request bytes and diagnostic state; it has not started the handler. No operation can bypass admission through this interval.

**Callbacks run without the notices lock.** The mutex-protected set insertion ends before Python logging is called. Concurrent captures of one file yield one successful insertion, and logging callbacks cannot deadlock by re-entering that mutex. Logging import, lookup and emission failures are ignored. The set belongs to each `ClientHost`, so another Client has its own reporting opportunity. It records a reporting attempt even when logging fails; it does not turn that failure into transport authority or operation progress.

**Each operation keeps its captured route.** Environment pairs are collected with the GIL held before detached file resolution. Both resolution and later transport capture use those captured pairs. Python callbacks or subsequent operations may change the environment, but cannot change this operation’s selected route. Files are read afresh for subsequent operations. Native and transport execution are exclusive enum arms; neither failure path retries the operation on the other route.

**Native policy fills preserve explicit values.** Native selection fills only absent concurrency and host limits with 50 and 8. The constructor’s `None` preserves absence, while explicit 32 and other positive constructor values reach request policy; per-call values override them. Rewriting updates encoded request metadata and the metadata passed to execution together. Existing policy fields are preserved by cloning. The Rust unit test checks decoded/encoded equality and preservation of explicit 32.

**Native selection creates no transport runtime.** Capture returns the native arm before transport identity resolution or cancellation-token construction. Execution constructs an ordinary backend directly. The absence of a running canceller is deliberate: waiting native work remains cancellable through admission state, whereas running native cancellation returns `UnsupportedOperation`. Native work still occupies a host slot. Close waits only to the established bound and reports unfinished work conservatively.

**Failure paths do not strand admission state.** Once registered, the ticket owns its registration and eventual slot. Dispatch validation failures, worker-spawn failures and unwinding drop that ticket; ordinary completion calls `finish`. The new resolution and diagnostic steps occur before acquiring it. This change introduces no additional durable record, restart grammar or filesystem write requiring a recovery transition.

**Timeout ownership remains singular.** Route selection performs no timeout mutation. Both routes continue consuming the existing process-wide clock, which defaults to nine seconds and refuses a different value after backend use freezes it. The Python CLI configures it before invoking its command handler. The changed help and README state that ownership and default.

**CLI presentation restores its hooks on exceptional exit.** The context manager restores saved logger handlers, propagation, level and `warnings.showwarning` in `finally`. A warning promoted to an error or an operation exception unwinds through that restoration. Nontransport warnings delegate to the saved warning hook. The focused notice test verifies ordinary restoration; exceptional restoration follows the same unconditional `finally`.

**Conditional boundaries remain explicit.** Candidate setting code stays inside the existing Unix transport module boundary. Ordinary builds supply an empty notice context and retain their native route. Modified Rust control-flow bodies are braced; the supplied source guard reports no new conditional-boundary occurrences.

## 3. Risks and next action

This GO covers the specified Python implementation, supported by source tracing and inspection of supplied receipts. It does not establish the explicitly deferred platform, live-route, release packaging, performance or aggregate TR2.6 outcomes.

The focused tests do not deterministically pause a detached scan while another thread closes the host, or drive simultaneous first notices through a re-entrant logging handler. Those interleavings were traced here and did not expose a defect; they are useful future regression coverage, not executed evidence claimed by this report.

The next action is for the lane owner to file this report verbatim and combine the independent verdicts for the settled tuple.