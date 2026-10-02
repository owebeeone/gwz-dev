# TR2.5 step 2 — CLI transport setting

Date: 2026-10-03. Status: **CLI settled at `c4a588f8be4e91926deff2156dce00c7062f4184`, pending independent review**. No acceptance, merge, release or platform qualification is claimed.

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
