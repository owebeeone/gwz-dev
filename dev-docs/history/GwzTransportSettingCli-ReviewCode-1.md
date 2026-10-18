# GwzTransportSettingCli — CODE-AXIS REVIEW

**Review object:** TR2.5 CLI implementation at `90fdb108f2a91ead456e07053da108b721300cd3`, with root implementation record and merged remediation plan at `52adfa0b8dba8752623d0ef5ada142105c48ac03`. Remediation re-verdict round 1; settled, pending original-reviewer closure.

**Baseline:** Previous reviewed tuple: root `b6153ec34cdb96972c1ceed057a073c3e7f03afc`, CLI `c4a588f8be4e91926deff2156dce00c7062f4184`, core `2e64e88a28c332ed422cc390adc76738dc701bb1`. Revised tuple: root `52adfa0b8dba8752623d0ef5ada142105c48ac03`, CLI `90fdb108f2a91ead456e07053da108b721300cd3`, core unchanged at `2e64e88a28c332ed422cc390adc76738dc701bb1`. Sources were read with `git show` at these recorded revisions. Changed range: CLI `c4a588f8be4e91926deff2156dce00c7062f4184..90fdb108f2a91ead456e07053da108b721300cd3`.

**Date:** 2026-10-03

**Axis:** Architecture, interfaces, call graphs, and compatibility reality. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on their current-round reports. Filed verbatim by the lane owner.

**Verdict: GO** — both original Code P2 findings are closed; no open or new P0, P1, P2 or P3 findings. This accepts the reviewed CLI implementation on the Code axis only. It does not claim aggregate acceptance or deferred qualification.

---

## 0. Evidence base

The review used `/Volumes/projects/limbo/gwz-dev-tr2-5-cli`.

- Verified root, CLI and core HEADs at start and end. All matched the revised tuple above. CLI status was clean; root retained the same five unrelated untracked SSH-prompt/route-mapping documents. Those documents were not inspected.
- Retained the original Code review’s controlling authority: off-switch design revision 3, amendment 2, root handoff §6.1, standing instructions, AgentProcessRules as amended by GwzProcessOptimization §8, and the review-loop skill.
- Read committed `dev-docs/GwzTransportSettingCli-RemPlan.md` and the revised implementation record at root `52adfa0b8dba8752623d0ef5ada142105c48ac03`. The merged prior-round dispositions were legitimate re-review inputs; no current peer report was consulted.
- Inspected the complete eight-file remediation diff: 269 insertions and 19 deletions.
- Retraced the corrected production seams:
  - CLI `src/globalargs/transport.rs:100–167`, path serialization, response decoration and execution-error helper;
  - CLI `src/lib.rs:305–338`, actual success/error call sites, channels and exit behavior;
  - CLI `src/globalargs.rs`, candidate helper visibility;
  - CLI `src/clirequest/common.rs:148–186`, retained error metadata and authentication rendering;
  - CLI `src/append_branch_summary/machine.rs:159–183`, retained machine error shape;
  - CLI `src/globalargs/render_exit.rs:33–78`, retained listing and response rendering;
  - unchanged core `workspace_ops/handle_materialize/clone_workspace.rs:110–156` and `workspace_ops/publication.rs:754–779`, the original non-null-meta error sequence.
- Inspected all added regressions:
  - `src/globalargs/transport_tests.rs:190–304`;
  - `tests/transport_workflows.rs:77–179`.
- Inspected the changed machine-output contract, paired setter/remover documentation, and candidate inventory.
- Read private remediation receipts under `campaigns/transport-qualification/runs/2026-10-03-tr25-cli-remediation-1/`: README, source hashes, unit red/green logs, workflow red/closure logs, ordinary/candidate full-suite logs, ordinary Clippy and candidate Clippy closure logs, source guards and conditional-boundary results.
- Verified every changed file’s SHA-256 against `source-sha256.json`; all eight matched.
- The supplied unit red receipt reproduces the original failures: missing native error setting and decoded newline path changed to literal backslash-n. The green receipt records all 10 focused unit tests passing. The final workflow closure receipt records all six process workflows passing.
- Full ordinary/candidate suite receipts pass on explicit Rust 1.95.0. The candidate full receipt precedes the final additional process-error test; that final test and strengthened event assertion are covered by the separate six-test closure receipt. Both strict CLI Clippy receipts complete successfully. Source guards and conditional-boundary checks pass.
- Verified binary hashes against `artifacts-round1.json`:
  - Candidate: `8e9fd3fd7c5c85fec294713a4b399cc7c6e2f586745d4a5ce6a3af7302bd21f5`;
  - Ordinary: `20d1cf7da093371729a6b8f9ecc2428f6f13148fff2d545558d511bff34ece55`.
- Ran permitted candidate `fetch --help` and ordinary `fetch --help`. Candidate help contains the explicit `$HOME/.gitconfig` setter/remover and the explanation of `GIT_CONFIG_GLOBAL`; ordinary help retains its prior option surface.

No files were modified, and no builds, tests or network operations were run by this reviewer. Executed regression evidence above was supplied by the lane owner.

## 1. Findings and prior-finding closure

No new findings.

| Prior Code finding | Status | Closure evidence |
| --- | --- | --- |
| P2-1 — Execution errors bypass the transport-setting report | **Closed** | The actual `run()` execution-error branch now calls `render_execution_error`, which applies the report to retained error rendering. Non-null metadata receives the required setting; null metadata remains untouched. Unit coverage verifies native/gwz, JSON/JSONL, authentication rows, default omission and null-meta controls. Human coverage verifies one selection line before auth rows. Final process coverage verifies the actual Err branch, exit 1, machine silence, null-meta omission and a single human selection line. |
| P2-2 — Human path escaping changes JSON path identity | **Closed** | Deciding, ignored and skipped JSON fields now serialize underlying path text rather than `path_text`. Unit coverage decodes JSON and JSONL and checks exact newline, tab, escape and literal backslash-n identities in every field, preserves member-ID characters, distinguishes newline from literal backslash-n, and retains one-line human output. |

**P2-1 counterexample retrace.** An authenticated clone of a plain Git repository still reaches the unchanged core `WorkspaceNotFound` path and attaches authentication metadata. `CliError::from_model` still retains it. The corrected execution-error branch now passes that error through the new helper and existing report decorator before output. Consequently, object-valued metadata carries the setting while the existing error and authentication content survives. The null-meta exception remains enforced by the decorator’s object check. This closes the original call-path bypass; no live-route execution is claimed.

**P2-2 counterexample retrace.** A newline-bearing global path now reaches JSON serialization with its newline intact. JSON supplies the wire escape, and decoding restores the actual character. The same correction applies to ignored and skipped paths. Human notes and verbose output continue using the existing escaped text methods. The regression directly exercises both representations and their distinction.

## 2. Invariant analysis

**Changed-range analysis.** The patch remains within CLI reporting, tests and the bounded documentation correction:

- A private candidate helper connects execution errors to the existing report decorator. The ordinary branch preserves the old rendering, channels and exit code.
- Three JSON path expressions now use underlying path text. Resolution, target scanning and human escaping are unchanged.
- Remote tag listings gain an empty metadata object only when a required setting exists and the retained listing has `kind: "tags"` with no metadata. Default output exits before this enrichment. Local-only tags have no report and therefore retain their existing payload.
- Candidate help and auth documentation use a paired explicit-file setter/remover that bypasses `GIT_CONFIG_GLOBAL`.
- The additional candidate boundary is inventoried. No core, route, policy, wire or durable-state implementation changed.

**Architectural-root-cause classification.** This is bounded remediation behind the accepted reporting contract. The new helper is private and does not change a shared interface or backend call graph. Tag-list enrichment implements the already-required optional CLI metadata; it does not redefine transport selection. No new architectural root cause was found, and the remediation-cap stop condition is not triggered.

The additional patch changes withstand the following attacks:

- **Error compatibility:** Authentication rows and error fields originate in retained renderers. The decorator augments only object-valued metadata. Default and null-meta controls remain unchanged.
- **JSONL integrity:** Setting rendering adds no event and preserves trailing records. Process listing tests reject Diagnostic records and retain entries.
- **Tag boundaries:** Explicit native/gwz and ignored/skipped cases receive the field; default and local-only cases retain absence. Tests exercise actual file-origin remote listings in both machine modes.
- **Lifecycle instructions:** Help and docs agree on both explicit-file commands. The supplied isolated-environment workflow sets and removes the supported setting while leaving the alternate `GIT_CONFIG_GLOBAL` file unchanged.
- **Scope preservation:** Network classification, resolver precedence, native runtime bypass, omitted-value policy fill and timeout selection are unchanged from the originally inspected implementation. Candidate/ordinary boundaries remain explicit.

## 3. Risks and next action

Deferred release, Linux/Windows, SSH/HTTPS route/no-fallback, MaxStartups, timeout and performance qualification remain deferred. Full-core candidate Clippy debt and inherited `partial_errors.rs` formatting debt remain separately recorded. This verdict does not waive either or close TR2.6.

The non-null error regression exercises the production helper with constructed authentication metadata, while process coverage verifies its actual call site and null-meta behavior. The original authenticated-clone sequence was independently retraced through unchanged core source; it was not executed during this review.

**Next action:** File this Code re-verdict verbatim and combine it with the other original reviewers’ closure verdicts on the same settled tuple. No further Code remediation is required.
