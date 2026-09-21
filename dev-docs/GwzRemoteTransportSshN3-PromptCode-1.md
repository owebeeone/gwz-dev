# N3 remediation1 — retained Code closure review

Continue under the canonical role, evidence/severity and read-only instructions
in GwzRemoteTransportSshN3-PromptCode.md. The exact replacement tuple is:
- .: 7f0a844b1bb851eedd3792eb13c0194b2231a190
- gwz-core: c79c7f13aebfcf582d0df75cff469d452e3477f1
- gwz-transport: a6562e654b52705b72ef1f793ae2045c320cee47
- gwz-core-evidence: 36d29397faae5205e1e16812f9f573a122665b7f
- git2-rs: ce78628308e11b4e8901d5061602619109bce21a
- libgit2: b172e3d187a4b6866fd9f696f40a1b8e7f56d348

Object: changed core range2f12bbd..c79c7f13aebfcf582d0df75cff469d452e3477f1,
controlling scope/results GwzRemoteTransportSshN3.md including remediation1
clarifications. Transport remains unchanged. Root report/prompt/RemPlan files are
legitimate inputs; current opposite-axis re-review output remains forbidden.

Read dev-docs/GwzRemoteTransportSshN3-RemPlan.md and both prior reports. Verify
original counterexamples and all consolidated dispositions, with particular focus
on your own findings. Add the canonical Prior-finding closure table and
Changed-range analysis before evidence. Label any new architectural root cause.
Return complete report verbatim as GwzRemoteTransportSshN3-ReviewCode-1.md;
do not write it. Check tuple and trees at start/end. Ignore only preexisting
root N2b prompts and these generated closure prompts. Do not spawn agents.

Same test permissions as the first prompt; additionally these focused gates are
allowed against the external target:
cargo +1.95.0 test --manifest-path gwz-core/tests/transport_ssh/Cargo.toml --offline --locked --target-dir /tmp/gwz-n2b-rem1-evidence-target --test local_endpoint --test pump --test cleanup_capacity

Owner final evidence: backend7/default8/SSH126 pass, one ignored SSH extended
campaign; unchanged transport prior94 pass/2 ignored. New private archive
run2026-09-22-backend-n3-rem1 contains raw reds/greens, source hashes and limits.
No source/platform/production qualification. Budget remains production1200/tests1600;
actual added523 production across22 files, tests1020 across8 files including README
and preparation harness. Do not rerun full suites; use focused closure commands.

The service correction deliberately carries one local stream-scoped boolean,
never arbitrary remote text; complete untruncated canonical refusal + no stdout
is published before EOF. It matches native empty-advertisement classification
without claiming exit/cleanup proof. Current git2-rs Read reduces io errors to Net;
the candidate clone boundary recognizes only the fixed internal marker. Future
CLI-hosted disposition mapping is still a separate unactivated phase.
