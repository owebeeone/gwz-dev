# TR2.5 Python transport settings — CODE-AXIS REVIEW

**Review object:** gwz-py implementation `b2369f1d0bf72f7fbbd5949d92c4c75a6fbc24b5..5bf260d040964a3dd9ec606b58a625bc74ac4afc`, controlled by `gwz-py/dev-docs/GwzPyTransportSetting-Implementation.md` at the settled Python SHA. Implementation draft; not accepted.

**Baseline:** root `b256a791f1dd05c04caedd8391ef146ff4d3d8fa`; Python baseline `b2369f1d0bf72f7fbbd5949d92c4c75a6fbc24b5`, settled Python `5bf260d040964a3dd9ec606b58a625bc74ac4afc`; core `2e64e88a28c332ed422cc390adc76738dc701bb1`. Changed source and controlling documents were inspected through exact-SHA `git show` and the specified baseline-to-settled diff.

**Date:** 2026-10-03

**Axis:** Architecture, interfaces, call graphs, compatibility, and failure behavior. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on their reports. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1, P2, or P3 findings.

---

## 0. Evidence base

The review used `/Volumes/projects/limbo/gwz-dev-tr2-5-py`. No files were changed, and no builds, tests, or mutation probes were executed.

The root, Python, and core HEADs were checked using `git rev-parse HEAD` and `git status --short` at the start and end. Both checks returned the exact tuple above. Python was clean. Root retained the five explicitly excluded SSH-prompt/route-mapping drafts; core retained the explicitly excluded private BugReport. Their status inventories did not change, and their contents were not read.

Authority inspected:

- Root `AGENTS.md` and `AGENTS_GWZ.md`.
- `AgentProcessRules.md`, especially L1-17 through L1-19 and §9’s independent Code-review mandate.
- `GwzProcessOptimization.md` §8.
- Root handoff §6.1.
- Core `GwzTransportOffSwitchDesign.md` §§2–4 and §§6–10.
- Core release plan’s TR2.5/TR2.6 rows and amendment 2 §3.17.
- Python `GwzPyPerOperationTransportDesign.md`, including its TR1.5 amendment, and the complete implementation package.

The complete 14-file Python diff was inspected. Detailed source examination covered:

- `native/src/client_host.rs:32–209`: shared entry, transport scope, diagnostics-before-registration ordering.
- `native/src/route.rs:17–25` and `route/native.rs:13–45`: candidate and ordinary branches.
- `route/transport.rs:49–193,203–273`: capture, resolver consumption, notices, native bypass, cancellation controls, and paired metadata rewriting.
- `route/transport_tests.rs:16–191`: defaults, explicit limits, native bypass, and identity rewriting.
- `src/gwz/client.py:167–208,271–338`: constructor validation and policy precedence.
- `src/gwz/_transport_notices.py:11–45`, `cli.py:74–111,140–154`, and `cli_shared.py` global-option/default and metadata construction.
- New diagnostics and setting tests, changed constructor tests, README’s transport section, and packaging configuration.
- Retained bridge exception handling, native error conversion, dispatch’s submitted-operation path, and host registration/cancellation/close implementation.

Unchanged core consumption was checked in `transport_scope.rs`, `transport_setting.rs`, its global resolver and repository scanner, `resolve_jobs.rs`, `resolve_per_host.rs`, transport binding, timeout configuration, and transport request metadata validation.

Supplied receipts were inspected, rather than rerun:

| Receipt | Reported result |
|---|---|
| Ordinary pinned full suite | 992 passed, 18 skipped |
| Both-switch pinned full suite | 1,010 passed |
| Ordinary/candidate Rust unit suites | 25 / 29 passed |
| Ordinary/candidate Clippy | Completed successfully |
| Final ordinary focused suite | 56 passed |
| Final candidate settings suite | 8 passed |
| Transport-only integration suite | 18 passed |
| Source guards | 2 passed |
| Conditional-boundary guard | 1,333 files; no new occurrences |

The two supplied extension hashes were independently calculated with `shasum -a 256` and matched `py-review/artifacts.json`: final both-switch `66bb05fd…1397ed`; transport-only `77d1704a…ed52f5`. These receipts corroborate the package’s validation account; they are not independent execution by this reviewer.

## 2. Invariant analysis

**Both native entries enforce the same decision before registration.** `call` and `submit` invoke `ClientHost::network`, which obtains transport scope from core’s shared predicate. Resolver refusals and native warning exceptions propagate before `operations.register`. Submitted-operation records and worker creation occur later. The added `expect` is supported by the immediately preceding successful `Operation::from_method` in `transport_meta`; no user-controlled method reaches it without that check.

**Capture remains per operation and releases the GIL for filesystem work.** Environment pairs are captured before `py.detach`. Resolution and the repository scan use the captured snapshot and cloned metadata inside the detached closure. The selected route is subsequently retained through waiting and execution; no failure path reselects native or rereads the live environment.

**Native selection bypasses the transport runtime.** The native branch returns before identity-home rewriting and cancellation-token creation. Its execution constructs `Git2Backend::new()` without a host context. The unchanged core binding therefore installs no gwz transport. A running native operation has no canceller and receives `UnsupportedOperation`; waiting cancellation and bounded close remain supplied by the existing host.

**Native defaults preserve explicit values and metadata agreement.** `get_or_insert` fills only missing concurrency and host limits with 50 and 8. The shared rewrite helper replaces encoded request metadata and updates the decoded metadata together. Retained dispatch decodes those same bytes, and core’s request-context equality check remains intact. Constructor `None` leaves the limit absent; explicit 32 and other positive values are retained; per-call values override constructor values. CLI parser defaults are also absent, so omitted CLI limits do not accidentally defeat native defaults.

**Ordinary builds retain their route behavior.** The existing `cfg_if` boundary selects the ordinary native implementation, whose capture ignores the added arguments and leaves request bytes unchanged. No new switch occurrence or directly attached conditional declaration was introduced. Core’s unchanged defaults still resolve absent limits to 100 and 32.

**Diagnostics have the accepted failure semantics.** Native selection calls a single package warning location, allowing standard warning filtering and bridge exception chaining. Repository notices use logger `gwz` at WARNING. Import, logger lookup, and logging failures are discarded; the per-file mutex is released before invoking Python logging, avoiding a lock held across an application callback. Per-file state belongs to `ClientHost`.

**CLI presentation is scoped and restored.** `cli_notices` saves and restores handlers, propagation, level, and `warnings.showwarning` through `finally`; unrelated warnings delegate to the prior hook. It surrounds the CLI run in every output mode, as required because Python’s machine output gains no setting field.

**Authority and documentation agree on the implemented distinction.** Handoff §6.1’s abbreviated logging sentence is clarified by the accepted off-switch design §4 and the Python design’s explicit amendment: native selection is a `UserWarning`; ignored repository values are logging records. The implementation follows that specific contract. README and timeout help describe selection/removal, candidate availability, defaults, cancellation, and the single process clock consistently with the code.

## 3. Risks and next action

This verdict covers the settled Python implementation and unchanged core interfaces it consumes. It does not establish deferred platform, release, disposable SSH/HTTPS route, performance, distributable-wheel, or TR2.6 aggregate acceptance. Supplied broad suites preceded the two documented lint-only source changes; final focused and Rust/lint receipts cover the final forms.

The next action is for the lane owner to combine the independently filed reviews and apply the package acceptance gate. This GO does not authorize merge, push, tag, publication, or installation.
