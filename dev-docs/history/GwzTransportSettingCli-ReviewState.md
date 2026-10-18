# TR2.5 CLI transport setting — STATE-AXIS REVIEW

**Review object:** gwz-cli implementation diff `0164e66376dac204910552148c62cb6c5c55ed03..c4a588f8be4e91926deff2156dce00c7062f4184`, plus root `dev-docs/GwzTransportSettingCli-Implementation.md` at `b6153ec34cdb96972c1ceed057a073c3e7f03afc`; settled, pending independent review, dated 2026-10-03.

**Baseline:** Root `b6153ec34cdb96972c1ceed057a073c3e7f03afc`; gwz-cli `c4a588f8be4e91926deff2156dce00c7062f4184`; gwz-core `2e64e88a28c332ed422cc390adc76738dc701bb1`. CLI change baseline `0164e66376dac204910552148c62cb6c5c55ed03`. Product sources were inspected using committed `git show` and `git diff`; supporting searches located relevant call paths. The exact tuple matched at both start and end.

**Date:** 2026-10-03

**Axis:** State — effect ordering, immutable selection, native admission bypass, policy preservation, crash/restart consequences, target scans, and faithful reporting. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: NO-GO** — three P2 findings block. No P0, P1 or P3 findings. I pre-commit to GO on a revision that resolves P2-1, P2-2 and P2-3 as specified.

---

## 0. Evidence base

The review followed `/Volumes/projects/limbo/gwz-lanes-prewarm-20261003/cli-review/State.txt` in workspace `/Volumes/projects/limbo/gwz-dev-tr2-5-cli`.

Authority inspected:

- Root and member instructions, including `AGENTS_GWZ.md`.
- `dev-docs/AgentProcessRules.md`, especially L1-13 through L1-21; `dev-docs/GwzProcessOptimization.md` §8.
- `/Users/owebeeone/.claude/skills/review-loop/SKILL.md`.
- Root implementation package, canonical `GwzTransportSettingCli-ReviewInputs.md`, current checkpoint’s TR2.5 entry, and `GwzTransportHandoff.md` §6.1.
- Core `GwzTransportOffSwitchDesign.md` revision 3, principally §§2–7, 9–10.
- Relevant release-amendment clauses: §§3.6–3.7, 3.13, 3.16–3.17 and 3.19.

Implementation inspected:

- All 13 changed paths in the CLI diff, including candidate boundaries, help constants, inventories, user documentation, unit tests and process workflows.
- CLI `src/lib.rs:157–344`; `src/globalargs/transport.rs:1–198`; `src/globalargs/dispatch.rs:1–80, 251–316, 368–388`; invocation construction and policy population.
- CLI `src/globalargs/render_exit.rs:7–79`; `src/append_branch_summary/response_listing.rs:242–255`; `src/append_branch_summary/machine.rs:19–86, 159–182`; `src/clirequest/common.rs:107–186`.
- Core transport resolver, global-file resolution, ignored scan, text helpers and transport-scope predicate.
- Core `src/workspace_ops/handle_tag.rs:48–180, 266–282, 341–385`; `src/workspace_ops/publication.rs:739–779`.
- Core timeout initialization and inherited local transport entry.

Executed reviewer commands were inspection only: tuple/status reads, committed-source reads and diffs, searches, hashes, candidate `fetch -h` and `fetch --help`, and ordinary `fetch --help`. No builds, tests, edits or Git mutations were performed.

Both binary hashes matched the supplied artifact receipt:

- Candidate: `964b060d09422a70d83a809b98968f5a384df550354802fee7033256f69961dc`
- Ordinary: `0513cff45a18fd826ca0fe056bd311aa2c7a65b3b0d22aec42f11fb71267d3f9`

The supplied private campaign README and ordinary/candidate Rust 1.95 test and Clippy logs were inspected. They record successful local suites and strict CLI Clippy. Those receipts do not exercise the counterexamples below. The findings are reproducible source-path deductions, not claims of reviewer-executed tests.

Start/end status identified the prompt’s out-of-scope root drafts and core private BugReport; CLI had no reported dirt. Their contents were not inspected.

## 1. Findings

### [P2-1] Metadata-only enrichment silently drops the setting on remote tag listings

**Location:** CLI `src/globalargs/transport.rs:135–143`, particularly the return when the rendered object has no object-valued `meta`. Supporting path: `src/globalargs/dispatch.rs:255–264`, `src/globalargs/render_exit.rs:63–69`, and `src/append_branch_summary/response_listing.rs:249–255`.

**Violated invariant:** Off-switch design §10 requires `meta.transport_setting` on transport-scope responses whenever the setting is nondefault or ignored/skipped values exist. Absence means exactly default gwz with nothing ignored or skipped.

**Reproduction/state sequence:**

1. Use a disposable workspace whose selected repositories have valid local or `file://` origins.
2. Run candidate `gwz --transport native --remote origin tag --list --json`, then the equivalent `--jsonl` invocation.
3. Tag list with a remote is in transport scope. Resolution produces a native report and dispatch selects the native backend.
4. Core remote tag list returns `tags: Some(tags)` (`handle_tag.rs:266–282, 374–385`).
5. CLI converts this to `ArtifactListing::Tags`, whose machine rendering is a `{"kind":"tags","entries":[...]}` object without `meta`.
6. `TransportReport::render` returns that object unchanged because `meta` is absent.

No SSH/HTTPS fixture is needed: the design expressly includes transport-scope commands whose remotes are local/file routes. Explicit `--transport gwz` and reports containing ignored/skipped values suffer the same omission.

**Impact:** Machine consumers lose the selected transport and source on a supported network command. A native selection is indistinguishable from the documented default-by-absence state. JSONL has no diagnostic event to recover the missing information.

**Required correction:** Support the listing response shape when adding the optional setting. Preserve the ordinary/default listing payload and existing entries; when the setting must be present, provide the required `meta.transport_setting` without changing the tag data or emitting a setting event.

**Closure/regression test:** Exercise actual remote tag-list rendering in JSON and JSONL with native and explicit gwz selections, plus ignored/skipped information. Assert the required setting, unchanged tag entries, silent stderr and no new event. Also assert that default remote listings and local-only tag listings retain their existing payloads and absence rules.

### [P2-2] JSON path fields contain human display escapes instead of the actual paths

**Location:** CLI `src/globalargs/transport.rs:108–125`, covering deciding `file`, `ignored[].file` and `skipped[].file`. Core’s called helper is `src/transport_setting/text.rs:106–125`.

**Violated invariant:** Design §10 distinguishes escaped human path display from JSON strings. Machine `file` fields must name the actual file; JSON encoding supplies the necessary escaping.

**Reproduction/state sequence:**

1. Construct a report whose deciding global file has a valid UTF-8 path containing an actual newline, for example a directory component consisting of `a`, newline, then `b`.
2. `TransportReport::json` calls core `path_text`, which changes that newline into the two characters backslash and `n`.
3. Serde then encodes the backslash. After JSON decoding, the consumer receives a literal `\n` sequence instead of the newline in the real path.
4. A different path whose component already contains literal backslash followed by `n` produces the same decoded field.

The same conversion is applied to ignored and skipped paths. A global-file example is reachable through a `HOME` containing a newline and a valid `.gitconfig` selecting native.

**Impact:** The machine representation is ambiguous and can identify the wrong filesystem location. Consumers cannot reliably locate or remove the configuration named in the report. Human output remains safely escaped; this finding concerns the machine value.

**Required correction:** Pass the underlying path text to JSON serialization without human control-character escaping. Retain `path_text` for notes, refusals and verbose human display.

**Closure/regression test:** Serialize and deserialize deciding, ignored and skipped paths containing newline, tab and another control character; assert that decoded values equal the original valid UTF-8 paths. Include distinct actual-newline and literal-backslash-`n` paths and assert they remain distinct. Retain one-line human-output checks.

### [P2-3] Success-only decoration omits setting information from operation errors

**Location:** CLI `src/lib.rs:305–336`. The report is applied only in `Ok(response)` at lines 312–315; the `Err(error)` arm directly prints `render_error_json` or `human_message_with_transport`.

**Violated invariant:** Design §§5 and 10 exempt error records whose `meta` is null, rather than every error. A transport-scope error with object-valued metadata must retain the applicable setting. Human verbose output must name the selected transport and source before its authentication rows.

**Reproduction/state sequence:**

1. Select native, or explicitly select gwz, and run a remote tag operation against a disposable fixture that records an authentication attempt and then refuses it.
2. The tag handler attaches transport observations to the error (`handle_tag.rs:341–347`).
3. `publication::attach_transport_error` constructs object-valued response metadata when observations are present (`publication.rs:754–779`).
4. `CliError::from_model` preserves that metadata.
5. `run()` enters `Err(error)`. Its JSON/JSONL renderer emits the non-null metadata and authentication rows but never applies `TransportReport::render`.
6. Human `--verbose` prints authentication rows through `CliError::human_message_with_transport`, but never prints the report’s transport/source line.

This also has a deterministic rendering-level counterexample: an execution error carrying `Some(ResponseMeta)` and a resolved native report reaches a branch that never consults the report.

**Impact:** The setting disappears precisely on failures where transport choice is necessary to diagnose the result. Machine readers see authentication metadata but cannot recover the native selection or deciding source. Human verbose errors omit the promised selection/source information.

**Required correction:** Apply report enrichment to execution-error rendering as well as successful responses. Preserve error records with null metadata byte-for-byte and preserve exit behavior. Add the required selection/source line to human verbose errors before authentication rows, without adding machine notices or events.

**Closure/regression test:** Cover execution errors with non-null metadata in JSON and JSONL and verify the setting and authentication rows survive. Verify a verbose human error includes the selection/source line exactly once before authentication rows. Keep the null-meta error regression, and verify non-network errors remain unchanged.

## 2. Invariant analysis

**Resolution before effects held on the inspected path.** Invocation construction validates and assembles a request. `prepare_transport` resolves before the ignored scan, policy fill, timeout configuration, backend construction and dispatch. A malformed deciding form returns a usage error and exits 2 before reaching those later stages. The setting resolver short-circuits flag, then environment, then global configuration; lower forms cannot refuse a command already decided by a higher form.

**Native bypass held.** Dispatch filters the runtime path with the resolved native boolean before calling `with_local_transport`. Native reaches `Git2Backend::new()` directly. I found no failure branch that retries the operation on the other transport.

**Explicit policy and timeout preservation held.** Native uses `get_or_insert` for 50 jobs and 8 per host, preserving explicit values and other policy fields. Timeout uses the explicit value when supplied, including zero; omitted values select 3 or 9 seconds. The timeout setter precedes backend construction.

**Selection is immutable after resolution.** The report is local to the invocation. Later global-file changes cannot change its route. The resolver consumes the supplied environment snapshot and reads no process environment itself. The inherited endpoint entry separately captures its environment; this review does not claim that this package eliminates that older read.

**Scope and scan delegation held at the CLI seam.** All nine operation arms match the shared transport-scope vocabulary, with remote tag scope delegated to the core predicate. Local-only tags and other commands return before resolving or scanning. Ignored values are advisory and do not select or refuse the transport. The existing core scan owns selection, layout guards and file bounds; the CLI introduces no independent repository discovery.

**Crash/restart attack found no new durable grammar.** Resolution, report construction, policy fill and timeout selection introduce no persistent record. A kill between these stages loses only in-memory preparation. A restart resolves a new invocation. The CLI preserves the existing handler dispatch and mutation ownership. This is a bounded conclusion about the reviewed diff, not a fresh qualification of every inherited handler.

**Human escaping held; machine identity failed.** Notices use core’s escaped value, scope and location helpers, and removal advice delegates shell quoting to core. The defect in P2-2 is reuse of that human representation in JSON.

**Default output preservation held on the ordinary response path.** `json()` returns `None` for default gwz with no ignored/skipped values, leaving the original rendered string intact. Machine notices are suppressed. Null-meta errors remain untouched. The output attacks nevertheless refuted completeness on tag listings and metadata-bearing operation errors.

**Interface shape matched the accepted selection model.** Candidate fetch help states gwz/native, all three forms, override/removal instructions and native defaults. Ordinary fetch help retains the original surface. No additional interface-shape finding was identified.

## 3. Risks and next action

The supplied local green gates do not prove Linux/Windows behavior, live SSH/HTTPS route observations, no-fallback receipts, MaxStartups behavior or measured timeout/performance outcomes. Those remain explicitly deferred, along with release qualification and TR2.6 aggregate acceptance. Inherited formatting and full-core candidate Clippy debt were not treated as findings.

The next action is one bounded remediation patch addressing P2-1 through P2-3, followed by focused regression receipts and original-reviewer closure verification on a newly settled tuple. No route, durable format or recovery-policy redesign is required by these findings.
