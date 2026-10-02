# TR2.5 CLI transport setting — remediation round 1

Date: 2026-10-03. Status: **NO-GO pending one bounded remediation patch and original-reviewer closure**.

Reviewed tuple: root `b6153ec34cdb96972c1ceed057a073c3e7f03afc`,
CLI `c4a588f8be4e91926deff2156dce00c7062f4184`,
core `2e64e88a28c332ed422cc390adc76738dc701bb1`.
Code reports two P2 findings; State reports three P2 findings; Surface reports
GO with one P3. Code and State independently converge on the error-rendering
and path-identity defects. Three blocking root causes remain, not five.

The accepted off-switch design is unchanged. No route, wire, durable-state or
policy redesign is proposed. Make one combined patch in the CLI lane, then
settle it through the root owner and return to the same reviewers. No reviewer
finding is self-closed. The two-round remediation cap remains in force.

| Findings | Disposition | Required closure |
| --- | --- | --- |
| Code P2-1; State P2-3 | Apply transport reporting to execution errors as well as success; retain null-meta omission and exit behavior. | Exercise actual execution-error rendering with non-null ResponseMeta/authentication rows in JSON and JSONL; explicit native/gwz, default omission and null-meta preservation controls; human verbose selection line exactly once before auth rows; unchanged event records and non-network errors. |
| Code P2-2; State P2-2 | Serialize actual UTF-8 path text in deciding/ignored/skipped JSON fields; human escaping remains human-only. | Serialize/decode newline, tab, escape and literal backslash-n paths in all three fields; exact path equality and distinct identities, JSONL coverage, one-line human controls. |
| State P2-1 | Enrich remote tag listing responses that have no meta when the setting is required, without changing default/ordinary/local-only payloads. | Actual remote tag-list JSON/JSONL against local/file origins: native, explicit gwz, ignored/skipped cases; unchanged entries, silent stderr/no setting event, default and local-only controls. |
| Surface P3-1 | Correct the setter/remover recipes under GIT_CONFIG_GLOBAL; keep the lifecycle pair together. | Isolated HOME with an alternate GIT_CONFIG_GLOBAL: revised instructions activate native and remove it from a supported file; alternate file behavior explicit. Verify long help and auth docs agree. |

Scope: CLI transport reporting/rendering, focused regression tests and the
bounded help/document correction. Core resolver and all sibling lanes remain
unchanged. Check candidate inventories/source guards for any changed boundaries.
Run meaningful focused regressions and ordinary/candidate CLI checks on explicit
Rust 1.95.0. Do not broaden into compiler/source-mutation campaigns or the
deferred release/platform/performance batch. Existing fmt and full-core
candidate Clippy debt remain identified, not waived.

The root owner files all complete reviewer reports verbatim, commits the revised
tuple, supplies exact changed ranges and evidence, and merges re-verdicts.

## Implementer evidence — pending original-reviewer verification

One combined CLI patch implements the dispositions above. Local validation on
Rust 1.95.0 is green: full ordinary/candidate CLI suites, ordinary/candidate
strict CLI Clippy, 10 focused unit regressions, 6 process workflows, candidate
inventory/process-global tests and the conditional boundary guard. Formatting
continues to report only inherited partial_errors.rs. The initial focused reds
and green receipts, source hashes and commands are preserved privately at
`campaigns/transport-qualification/runs/2026-10-03-tr25-cli-remediation-1/`.

No finding is closed by this paragraph. The Err arm's shared candidate helper,
exact JSON path serialization, required tag-list metadata and paired --file
recipes are the changed seams. Root owns the revised commit tuple and re-verdict
requests to the original Code, State and Surface reviewers. All prior deferrals
and the two-round cap remain in force.
