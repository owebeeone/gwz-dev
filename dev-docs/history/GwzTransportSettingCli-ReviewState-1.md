# TR2.5 CLI transport setting — STATE-AXIS REVIEW

**Review object:** Remediation round 1 at gwz-cli `90fdb108f2a91ead456e07053da108b721300cd3`, plus root `dev-docs/GwzTransportSettingCli-RemPlan.md` and `dev-docs/GwzTransportSettingCli-Implementation.md` at `52adfa0b8dba8752623d0ef5ada142105c48ac03`; settled, pending original-reviewer closure, dated 2026-10-03.

**Baseline:** Previously reviewed root `b6153ec34cdb96972c1ceed057a073c3e7f03afc`, CLI `c4a588f8be4e91926deff2156dce00c7062f4184`, core `2e64e88a28c332ed422cc390adc76738dc701bb1`. Revised tuple: root `52adfa0b8dba8752623d0ef5ada142105c48ac03`, CLI `90fdb108f2a91ead456e07053da108b721300cd3`, unchanged core `2e64e88a28c332ed422cc390adc76738dc701bb1`. Changed range: CLI `c4a588f8be4e91926deff2156dce00c7062f4184..90fdb108f2a91ead456e07053da108b721300cd3`. Sources were inspected with committed `git show` and `git diff`. The revised tuple matched at both start and end.

**Date:** 2026-10-03

**Axis:** State — original counterexample closure, faithful transport reporting, effect ordering, native bypass, policy preservation and crash/restart consequences. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on their current-round reports. The merged prior-round remediation plan is a legitimate input. Filed verbatim by the lane owner.

**Verdict: GO** — all three original State P2 findings are closed. No open or new P0, P1, P2 or P3 findings. No new architectural root cause was identified.

---

## 0. Evidence base

This is the original State reviewer’s focused re-verdict in `/Volumes/projects/limbo/gwz-dev-tr2-5-cli`, continuing the initial canonical review and its authority and deferrals.

Read at the revised root commit:

- `dev-docs/GwzTransportSettingCli-RemPlan.md`, including all merged dispositions and required closure evidence.
- `dev-docs/GwzTransportSettingCli-Implementation.md`, including the round-1 implementation and limitation record.

Inspected the complete eight-file remediation diff. Principal corrected production locations:

- CLI `src/globalargs/transport.rs:100–167`: underlying path serialization, optional metadata for tag listings, and shared execution-error rendering.
- CLI `src/lib.rs:305–338`: success and execution-error rendering call sites, output channels and exit behavior.
- CLI `src/globalargs.rs`: candidate-confined re-export of the error renderer.
- Updated help, machine-output documentation and candidate inventory.

Inspected the added regressions in:

- `src/globalargs/transport_tests.rs:189–304`.
- `tests/transport_workflows.rs`, particularly actual process errors, the file-origin tag fixture and paired configuration recipes.

Supplied execution evidence was read from the private campaign:

`gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-03-tr25-cli-remediation-1/`

Evidence included README commands and dispositions, source hashes, unit red/green logs, workflow red and closure logs, ordinary/candidate full-suite results, ordinary and final candidate strict Clippy results, conditional-boundary and source-guard receipts.

The focused red receipts reproduce the original missing execution-error setting, corrupted decoded path, and missing remote-tag setting. The green unit receipt records 10 passing focused tests. The final workflow closure receipt records six passing process workflows, including the added actual execution-error path and strengthened Diagnostic-event assertions. Full ordinary/candidate suites and final strict CLI Clippy receipts are green on explicit Rust 1.95.0. The final workflow test additions are covered by the separate closure run, rather than falsely attributed to the earlier full-suite run.

All eight current changed-file SHA256 values matched `source-sha256.json`. Both current binaries matched `/Volumes/projects/limbo/gwz-lanes-prewarm-20261003/cli-review/artifacts-round1.json`:

- Candidate: `8e9fd3fd7c5c85fec294713a4b399cc7c6e2f586745d4a5ce6a3af7302bd21f5`
- Ordinary: `20d1cf7da093371729a6b8f9ecc2428f6f13148fff2d545558d511bff34ece55`

Reviewer-executed commands were inspection only: tuple/status reads, committed source reads and diffs, searches, hashes and permitted candidate `fetch --help`. No builds, tests, writes or Git mutations were performed. Current-round peer reports were not inspected.

Start/end status retained the previously identified out-of-scope root drafts and core private BugReport; CLI had no reported dirt. Those out-of-scope contents were not read.

## 1. Findings and prior-finding closure

No new findings.

| Original State finding | Disposition | Original counterexample verification |
| --- | --- | --- |
| P2-1 — metadata-only enrichment drops remote tag settings | **Closed** | `TransportReport::render` now creates an object-valued `meta` specifically for a `kind: tags` listing when the setting is required. The actual binary file-origin fixture passes in JSON and JSONL for native, explicit gwz, ignored and skipped cases, retaining entries and silent stderr. Default and local-only listings retain absence. |
| P2-2 — human path escapes corrupt JSON path identity | **Closed** | All three JSON path fields now use underlying path text. Tests serialize and decode newline, tab, escape and literal backslash-n paths in deciding, ignored and skipped fields in both machine modes. Decoded equality and distinct newline/backslash-n identities pass; human notes and verbose text remain one line. |
| P2-3 — success-only decoration omits execution-error settings | **Closed** | The actual `run()` execution-error branch now calls the shared candidate renderer. Non-null metadata/authentication-row tests pass for explicit native and gwz in JSON and JSONL. Default, no-report and null-meta controls preserve original output. Human selection precedes authentication rows exactly once. The real process error workflow also passes, confirming branch wiring, channels, exit 1 and null-meta omission. |

### Closure reasoning

**P2-1:** The original remote tag handler and listing renderer remain intact. The corrected enrichment runs only after `json()` determines that a setting is required. It adds metadata to the listing without replacing `kind` or `entries`. A default report returns the original rendered string before enrichment, and local-only tag invocations still produce no report. The fixture contains a real advertised tag and asserts unchanged entries, so the regression does not pass merely because the listing is empty. The final receipt also checks both `kind` and `event_kind` for unintended Diagnostic records.

**P2-2:** The deciding file now uses `location.file.to_string_lossy().into_owned()`; ignored and skipped files use `to_string_lossy()` directly in JSON construction. These preserve valid UTF-8 control characters until Serde performs JSON encoding. Human display continues to use the existing escaped helpers. The exact original collision between newline and literal backslash-n is explicitly tested and no longer occurs.

**P2-3:** `run()` calls `render_execution_error` before choosing the existing output channel. The helper renders the original error first, then applies the same report logic used for successful responses. Object-valued metadata therefore receives the setting, while null metadata remains null. Human verbose rendering prepends the selection/source line before the error’s authentication rows. The unit regression exercises the exact helper now called by production, with non-null `ResponseMeta` and authentication data; the separate process workflow exercises the actual production Err arm. No live authentication qualification is inferred from those rendering tests.

## 2. Invariant and changed-range analysis

**Changed range is bounded remediation.** The eight-file patch changes CLI reporting, regression tests, the configuration lifecycle recipe and its documentation, and the candidate inventory. Core remains at the original SHA. No route, protocol field, durable format, mutation owner or policy rule changed.

**New architectural root-cause classification: none.** The helper extraction shares error-report enrichment with the existing renderer. The tag-list correction supports an existing output shape under the already accepted metadata contract. Underlying path serialization restores the specified machine representation. These are implementation corrections behind the accepted interface, not a redesign or a new architectural repair round.

**Resolution and native bypass remain intact.** The remediation does not change `prepare_transport` ordering or dispatch selection. Resolution/refusal still precedes scanning, policy fill, timeout configuration and backend creation. Native still bypasses `with_local_transport`; no fallback branch was introduced.

**Policy and timeout ownership remain intact.** The original fill-only behavior for 50 jobs and 8 per host is unchanged. Explicit values, including timeout zero, remain preserved. Rendering corrections cannot alter the request’s resolved route or policy.

**Output absence remains meaningful.** Default gwz with nothing ignored/skipped exits enrichment before JSON parsing. Tag metadata is added only when required. Error enrichment retains the object-valued/null distinction. Non-network errors receive no transport report. Ordinary-build error rendering retains its original content, channels and exit behavior.

**JSONL event preservation remains intact.** Report decoration modifies the rendered response record and preserves trailing records. The existing event-preservation unit control remains green, and process listing workflows assert no setting Diagnostic event. No core event numbering or operation-log producer changed.

**Human/path safety remains intact.** Raw path characters are confined to JSON serialization. Human notes, source lines and removal advice retain their escaping/quoting helpers. The added tests verify one-line human output for the adversarial path cases.

**Configuration lifecycle correction is consistent with resolver scope.** Candidate help and auth documentation now pair setter and remover using `--file "$HOME/.gitconfig"` and explicitly explain bypassing `GIT_CONFIG_GLOBAL`. The supplied isolated lifecycle workflow proves activation and removal while leaving the alternate file unchanged. This review identifies no additional defect in that bounded correction; it does not substitute for the Surface reviewer’s own closure.

**Crash/restart semantics remain unchanged.** The corrected data and helpers are invocation-local rendering. They add no writes, durable owner or restart transition. The original State review’s bounded conclusion that this CLI change creates no new durable recovery grammar still holds.

## 3. Risks and next action

GO closes this reviewer’s three findings for the stated settled tuple and implementation scope. It does not claim release, Linux/Windows, live SSH/HTTPS route/no-fallback, MaxStartups, measured timeout/performance or TR2.6 aggregate qualification. Those deferrals remain unchanged. Inherited formatting and full-core candidate Clippy debt remain identified rather than waived.

The next action is for the lane owner to file this report verbatim and merge the required current-round reviewer verdicts before recording acceptance.
