# GwzTransportSettingCli — CODE-AXIS REVIEW

**Review object:** TR2.5 CLI implementation diff `0164e66376dac204910552148c62cb6c5c55ed03..c4a588f8be4e91926deff2156dce00c7062f4184`, plus root `dev-docs/GwzTransportSettingCli-Implementation.md` at `b6153ec34cdb96972c1ceed057a073c3e7f03afc`. Status: settled implementation, pending independent review; no release qualification claimed.

**Baseline:** Root HEAD `b6153ec34cdb96972c1ceed057a073c3e7f03afc`; gwz-cli HEAD `c4a588f8be4e91926deff2156dce00c7062f4184`; gwz-core HEAD `2e64e88a28c332ed422cc390adc76738dc701bb1`. CLI change baseline `0164e66376dac204910552148c62cb6c5c55ed03`. Product sources and controlling documents were read with `git show` at these recorded revisions; the complete CLI diff was inspected.

**Date:** 2026-10-03

**Axis:** Architecture, interfaces, call graphs, and compatibility reality. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on their reports. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block; no P0, P1 or P3 findings. I pre-commit to GO on a revision that resolves **P2-1 and P2-2** as specified, with regression evidence and no material scope expansion.

---

## 0. Evidence base

The review used workspace `/Volumes/projects/limbo/gwz-dev-tr2-5-cli`.

- Verified root, CLI and core HEADs at review start and end. All three matched the exact tuple above throughout.
- Root `git status --short` showed only the five inherited untracked SSH-prompt/route-mapping documents named outside this review’s scope. CLI status was clean. Those unrelated documents were not reviewed.
- Read root `AGENTS_GWZ.md`, core `AGENTS.md`, the supplied standing conditional-boundary rules, and the canonical generated Code prompt.
- Read the review-loop skill at `/Users/owebeeone/.claude/skills/review-loop/SKILL.md`, root `CurrentProgramCheckpoint.md`’s TR2.5 record, `AgentProcessRules.md`’s baseline/review/severity/lifecycle rules, and `GwzProcessOptimization.md` §8.
- Read the complete implementation package and canonical ReviewInputs, root handoff §6.1, core off-switch design revision 3—especially §§2–7 and 9–10—and amendment 2’s candidate-switch, native-route and review-boundary provisions.
- Inspected all 13 changed CLI files. Main production inspection:
  - `src/globalargs.rs`, complete diff;
  - `src/globalargs/parser.rs`, global option changes;
  - `src/globalargs/dispatch.rs:1–34, 368–388`, selected execution and scope;
  - `src/globalargs/transport.rs:1–198`, complete;
  - `src/globalargs/transport_help.rs:1–22`, complete;
  - `src/lib.rs:157–345`, process entry, preparation, execution and rendering.
- Inspected every added regression in `transport_tests.rs` and `tests/transport_workflows.rs`, the modified g09 help tests, switch inventory, process-global allowlist, and both documentation changes.
- Traced retained interfaces through:
  - CLI `globalargs/invocation.rs:14–42`, request construction;
  - CLI `clirequest/invocation.rs`, request metadata and policy;
  - CLI `clirequest/common.rs:148–186`, preservation and human rendering of error metadata;
  - CLI `append_branch_summary/machine.rs:159–183`, machine error rendering;
  - CLI `globalargs/render_exit.rs:7–79`, success and JSONL rendering;
  - core `transport_scope.rs`, complete;
  - core `transport_setting.rs`, resolver and report types;
  - core `transport_setting/text.rs:50–125`, human text escaping;
  - core `transport_setting/scan.rs:22–170`, scan delegation and targets;
  - core `session_host/environment.rs:74–150`, snapshot construction;
  - core `workspace_ops/publication.rs:739–779`, authentication metadata attached to errors;
  - core `workspace_ops/handle_materialize/clone_workspace.rs:22–156`, a concrete top-level error after authentication.
- Read the supplied campaign README and relevant results in `ordinary-195.log`, `candidate-195.log`, `ordinary-clippy-195.log` and `candidate-clippy-195.log`. The logs record passing ordinary and candidate CLI suites and completed strict CLI Clippy runs on Rust 1.95.0. The candidate suite includes the eight new unit regressions and three process workflows. These are supplied executed receipts, not reviewer reruns.
- Verified both binary SHA-256 hashes against `artifacts.json`:
  - Candidate: `964b060d09422a70d83a809b98968f5a384df550354802fee7033256f69961dc`;
  - Ordinary: `0513cff45a18fd826ca0fe056bd311aa2c7a65b3b0d22aec42f11fb71267d3f9`.
- Ran only permitted binary help: candidate root help, `help fetch`, `fetch -h`, `fetch --help`, `push --help`, `pull --help`, `auth --help`, and ordinary `fetch --help`. They exited successfully and exposed the intended candidate/ordinary distinction.

No files were modified, and no build, test, source-mutation probe or network operation was run by this reviewer. Additional owner execution was requested for the counterexamples below; this report does not claim those executions occurred.

## 1. Findings

### [P2-1] Execution errors bypass the transport-setting report

**Location:** gwz-cli `src/lib.rs:305–335`, specifically the `Err(error)` branch at lines 322–335. Related retained interfaces: `src/clirequest/common.rs:148–186` and `src/append_branch_summary/machine.rs:161–181`.

**Violated invariant:** Accepted off-switch design §§5 and 10 require selection/source reporting for scoped verbose commands and require `meta.transport_setting` whenever its presence condition holds. The error exception applies to records whose `meta` is **null**. A scoped error with real response metadata remains subject to the setting contract.

**Reproduction/state sequence:**

1. Select native explicitly and clone an authenticated disposable SSH repository that is ordinary Git but lacks the GWZ workspace manifest.
2. Authentication produces transport observations; clone then returns `WorkspaceNotFound` at core `handle_materialize/clone_workspace.rs:151–156`.
3. The handler’s error path calls `attach_transport_error` at lines 115–118. Core `publication.rs:754–779` attaches a non-null `ResponseMeta` containing the authentication observations.
4. `CliError::from_model` retains that metadata.
5. CLI execution reaches `src/lib.rs:322`. `--json` or `--jsonl` calls `render_error_json` directly. Unlike the success branch, it never applies `transport_report.render`.
6. The emitted record has non-null metadata and authentication rows but no `transport_setting`, despite explicit native selection. Human `--verbose` uses the same bypass and prints authentication rows without the required selection/source line.

This is a static call-path reproduction, not a claim of executed route qualification. A simpler preflight error also demonstrates the missing verbose selection line, though its null metadata correctly excludes the JSON field.

**Impact:** The operation’s failure output loses its selected transport and provenance precisely where diagnosis matters. For non-null-meta machine errors, omission contradicts the documented meaning of absence—default gwz with nothing ignored or skipped—even though native was explicitly selected.

**Required correction:** Apply the candidate report to execution-error rendering as well as successful response rendering. Preserve null-meta errors without injecting metadata; retain authentication rows and JSONL event records; emit the verbose selection line once for scoped human errors. Ordinary output must remain unchanged.

**Closure/regression test:** Exercise the actual error-rendering branch with a `CliError` carrying `ResponseMeta` and authentication rows. Check explicit native and explicit gwz in JSON and JSONL, retained auth rows, unchanged event records, and a single human verbose selection/source line. Retain a null-meta byte-preservation control and a default-setting omission control. A disposable authenticated clone that fails workspace validation provides a process-level confirmation without live accounts.

### [P2-2] Human path escaping changes the identity of JSON path fields

**Location:** gwz-cli `src/globalargs/transport.rs:108–126`: deciding `file` at line 109, `ignored[].file` at line 120, and `skipped[].file` at line 125.

**Violated invariant:** Accepted design §10 distinguishes one-line escaping for human notes/messages from JSON strings carrying paths and IDs. CLI `docs/MachineOutput.md:210–216` states that these fields name the actual files.

**Reproduction/state sequence:**

1. Use a valid UTF-8 absolute HOME path containing a newline, for example a directory whose component is `home` followed by a newline and `name`.
2. Place `[gwz] transport = native` in its `.gitconfig`, leave the flag and variable unset, and perform a candidate dry-run fetch with JSON output in a valid disposable workspace.
3. Core resolves the real `PathBuf` as the deciding global file.
4. `TransportReport::json` passes that path through `transport_setting::path_text`.
5. Core `text.rs:106–125` explicitly implements human escaping: the newline becomes two characters, backslash and `n`.
6. `serde_json` then serializes that already-escaped string. After ordinary JSON decoding, `meta.transport_setting.file` contains literal backslash-`n`, not the actual newline-containing path.

The same transformation affects ignored and skipped paths. A real newline path and a path containing literal backslash-`n` can consequently become indistinguishable in these fields.

**Impact:** Machine consumers receive a different filename, cannot reliably locate or remove the reported setting, and cannot round-trip path identity. This is a compatibility and diagnosability defect; valid JSON alone does not preserve the contract.

**Required correction:** Supply the underlying path string to JSON serialization, letting JSON perform its own escaping. Keep `path_text` for human output. Apply the correction consistently to deciding, ignored and skipped file fields.

**Closure/regression test:** Construct reports with valid UTF-8 paths containing newline, tab, an escape character and literal backslash-`n`. Serialize and decode JSON, then assert exact equality with the underlying path strings for all three path-bearing field forms. Assert that newline and literal backslash-`n` paths remain distinct, while human notices and verbose lines remain escaped onto one line. Include JSONL rendering coverage.

## 2. Invariant analysis

The following attacks did not reveal a defect:

- **Precedence ownership:** The CLI converts Clap’s enum to the accepted core type and calls `transport_setting::resolve`. It does not duplicate global-file precedence or value validation. A decisive flag reaches core before lower forms are examined.
- **Network classification:** The new preparation match contains the same nine operation kinds as dispatch and core’s `Operation` inventory. Tag delegates remote participation to the shared predicate; local-only tags and other command families return without resolution or scanning.
- **Native runtime bypass:** The selected native boolean is passed before `with_local_transport`. Its filter excludes native, which reaches `Git2Backend::new()` without constructing the transport runtime. No retry-to-native fallback was added.
- **Policy ownership:** Native fills only missing concurrency and per-host fields with `get_or_insert`. Existing explicit values survive. The timeout chooser preserves explicit zero and other values, with 3 seconds for omitted native and 9 for omitted gwz.
- **Refusal ordering:** Core resolver failure returns before timeout configuration, backend construction and dispatch. It renders as usage exit 2. Non-network requests return before resolver validation.
- **Human notices:** Native and ignored-value notices are printed before dispatch only in human output. The shared core methods supply escaped human values, scope text and shell-safe remediation text.
- **Success output compatibility:** Default gwz with no ignored/skipped values returns the existing serialized payload unchanged. Nondefault success reports augment an existing metadata object. Authentication rows are retained. JSONL setting rendering preserves the trailing records and adds no setting event.
- **Conditional boundaries:** New candidate sections use enclosing `cfg_if!` blocks. Ordinary builds receive an empty flattened argument struct and retain the ordinary help constants. Windows activation remains deferred as the controlling design states.
- **Help and inventory:** Candidate help includes the accepted defaults, precedence, undo forms and native retry semantics. Root short help retains its established shape. Switch inventory and immutable environment-snapshot inventory were updated.
- **Test path edge:** `transport_tests.rs` is enclosed by candidate and test boundaries, and root’s settled package/checkpoint explicitly acknowledges the edge.

These conclusions concern inspected interface and call-path shape. They do not establish the deferred network timing, connection-count or platform outcomes.

## 3. Risks and next action

The supplied tests cover success reporting and null-meta preservation but do not exercise the execution-error report boundary or decoded control-character path identity; that explains why the two defects can coexist with green local gates.

Release qualification, Linux/Windows execution, disposable route/no-fallback measurements, MaxStartups behavior, timeout measurements, full-core candidate Clippy debt and TR2.6 aggregate acceptance remain explicitly deferred. This review adds no finding for those deferred outcomes.

**Next action:** Produce one bounded remediation patch for P2-1 and P2-2, record its regression receipts, settle the revised tuple, and return it to this reviewer for closure verification.
