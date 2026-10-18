# TR2.5 step 2 — CLI transport setting

Date: 2026-10-03. Status: **CLI remediation settled at `90fdb108f2a91ead456e07053da108b721300cd3`, pending original-reviewer closure**. Initial review at `c4a588f8be4e91926deff2156dce00c7062f4184` is NO-GO. No acceptance, merge, release or platform qualification is claimed.

Authority: gwz-core/dev-docs/GwzTransportOffSwitchDesign.md revision 3 (§§2–7, 9–10), GwzTransportReleasePlanAmendment-2.md and GwzTransportHandoff.md §6.1. D2/D3/E3 and the recommended OQ dispositions remain unchanged. Process: AgentProcessRules.md as amended by GwzProcessOptimization.md §8 and the review-loop skill. The lane owner owns all Git/GWZ operations and reviewer dispatch.

Baseline: root `12f11c7949919834fe8858247dc4c0cd49a8134b`, CLI `0164e66376dac204910552148c62cb6c5c55ed03`, core `2e64e88a28c332ed422cc390adc76738dc701bb1`. This package resumes the inherited CLI draft; core, Python, credential APIs and unrelated drafts are unchanged.

The candidate CLI exposes `--transport gwz|native`; core's resolver owns flag/environment/global-file precedence and target scanning. The CLI owns notices, verbose selection text and optional `meta.transport_setting`. Native selection skips `with_local_transport`, fills only omitted policy values with 50 jobs and 8 per host, and chooses a 3 second default timeout. Gwz retains 9 seconds. Explicit values, including timeout zero, are preserved. A local-only tag and non-network commands do not resolve or scan. Machine modes print no setting notices or extra JSONL events. Null-meta error records and the default JSON payload are unchanged.

All candidate switch sites are inventoried. Help has a cohesive candidate/ordinary constants module; ordinary generated CLI reference stays unchanged. The startup `env::vars_os` read is inventoried as an immutable driver snapshot imposed by the entry contract. The `transport_tests.rs` path edge is confined to the enclosing candidate and test blocks and requires root's same-commit acknowledgment when settling.

Test-first evidence: the new candidate help regression failed with `missing Defaults to 100; 50 with --transport native.` before the help constants were implemented. Focused regression tests cover precedence/refusal, omitted/explicit native defaults and timeouts, notices, verbose source text, JSON presence, retained authentication rows, JSONL trailing records and null-meta errors. Real-binary integration tests cover each deciding form, explicit gwz over native environment, native over malformed environment, silent machine modes, non-network immunity, human dry-run/ignored-root notices and ordinary-build rejection of the flag.

Candidate manifests were freshly prepared with this lane's core prepare.py in `/Volumes/projects/limbo/gwz-tr25-cli61-candidate-20261003/`. The external CLI's core points to that prepared manifest and its git2-rs dev dependency is anchored absolutely to this lane. External `gwz-core` links back to this lane for the pre-existing source-reading tests. Ordinary target is the lane root's `target`; candidate target is `target/candidate-transport`; debug and incremental profiles were retained.

## Local gates

The final executed gate results and raw logs are filed in the private evidence member, `campaigns/transport-qualification/runs/2026-10-03-tr25-cli-setting/` (private access required).

- Full ordinary and candidate CLI suites, including process workflows: passed with explicit Rust 1.95.0 (normal runners, exit 0).
- Ordinary and candidate strict CLI Clippy (`--all-targets -- -D warnings`): passed with explicit Rust 1.95.0; one new collapsible-if diagnostic was corrected. This does not claim the separately recorded full-core candidate Clippy debt is fixed or waived.
- Candidate inventory/process-global tests: passed.
- Conditional compilation boundaries across all disabled platform branches: passed.
- Changed Rust files were formatted; workspace formatting remains red only in inherited `src/tests/g02/partial_errors.rs`, outside scope and unmodified.
- No source-mutation/compiler campaign probes were run.

Linux/Windows execution, disposable SSH/HTTPS route and no-fallback receipt qualification, 32-member MaxStartups behavior and silent-peer wall-clock measurements remain in the deferred platform/transport qualification batch. Local policy and timeout tests prove configured values, not measured network timing. Review must not describe those deferred outcomes as executed.

## Settlement and review

The root owner committed the CLI via GWZ at the revision above and acknowledges the candidate/test-confined `transport_tests.rs` path edge. The root record commit completes settlement; reviewer prompts name its exact root/CLI/core tuple. Independent Code and State plus Surface reviewers use the canonical review-prompt template. Review this implementation and its authority; Surface reads only help and user docs. One merged remediation patch, original-reviewer closure verification and the two-round cap apply. TR2.6's aggregate Phase 2 review remains owed.

## Round-1 bounded remediation, 2026-10-03

The combined patch follows [RemPlan](GwzTransportSettingCli-RemPlan.md).
Code P2-1 / State P2-3 share the candidate execution-error rendering seam:
`run()` now invokes `render_execution_error` in its Err branch, decorating
object-valued metadata and human verbose output while retaining null-meta
omission, channels and exit 1. Code P2-2 / State P2-2 serialize the underlying
path string in deciding/ignored/skipped JSON fields, keeping human escaping
in human output. State P2-1 enriches remote `kind: tags` listings only when
required, retaining entries and default/local-only absence. Surface P3-1's
paired setter/remover use explicit `$HOME/.gitconfig` with `--file`, bypassing
`GIT_CONFIG_GLOBAL`, and help/docs explain that distinction.

Before the correction, focused tests reproduced missing error settings,
corrupted decoded path identity, missing remote-tag settings and the help
recipe gap. The corrected 10 focused unit tests and 6 process workflows pass;
they exercise actual error rendering with ResponseMeta/authentication rows,
real binary null-meta/human errors, JSON/JSONL controls, exact control-character
path decoding, file-origin remote tag listings, ignored/skipped values,
unchanged entries, no Diagnostic events, local/default absence and the isolated
setter/remover lifecycle under an alternate GIT_CONFIG_GLOBAL.

Full ordinary/candidate suites, both strict CLI Clippy runs, inventory/process
guards and conditional boundaries pass on Rust 1.95.0. Formatting remains red
only in unchanged inherited partial_errors.rs. Private raw red/green receipts:
`campaigns/transport-qualification/runs/2026-10-03-tr25-cli-remediation-1/`.
No core/interface, route or policy changes; no Git/GWZ operations or self-closure.
Root must settle the corrected tuple and return to the original reviewers.
