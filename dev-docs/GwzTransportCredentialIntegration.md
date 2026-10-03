# Credential transport integration — macOS

2026-10-03. Status: **accepted for host-local combined macOS integration at
validated root `5be088f50808e930cc78be8db0671486256dac4a`, core
a92a7990081475e349b182a332c7553731f4fb5b, transport
1aab733783e06b25cb5d2321d71ec0b34417a29c, CLI
f925e1165c2b2d368a00277594450b010a95867a, Python
5aeff4bfb11da048f1174db2b1045ed0c9b6c80a and evidence
662d89828b478a2acce8c0308834db7d17c872f7. This accepts the requested
ordinary/candidate composition checks only; it is not release acceptance.**

## Source and integration correction

Credential production source has Code2/State2 GO; unchanged help has Surface1
GO, as gwz-core/dev-docs/GwzTransportCredentialHelpersAcceptance.md records.
GWZ family merge merge_op_93443_1790979898498_0001 fast-forwarded five members
into MAIN. No source conflict resolution changed the reviewed code.
Unrelated drafts temporarily preserved in a targeted GWZ stash were restored:
all 27 preflight files match original hashes; the temporary stash retired.

The first full Python runs found two stale tests, not established production
defects: the historical wire projection omitted accepted additive code 75, and
an old gh fixture assumed automatic invocation instead of configured helpers
and retained obsolete Authentication text. One two-file test-only correction
preserves the historical hash, exact names/slots, helper refusal, no-extra-
connection proxy refusal and credential redaction. Independent interior Code
review GO is filed verbatim as GwzTransportCredentialIntegration-ReviewCode.md.
The initial full REDs and intermediate focused RED remain preserved; they are
not waived or overwritten. Corrected focused ordinary 3/both 4/transport 4 passed,
then all three complete Python suites passed on the settled corrected source.
No generated protocol or production code changed for this correction.

## Actual combined-source evidence

| Component | Ordinary | Transport candidate | Both candidate switches |
| --- | --- | --- | --- |
| CLI | Full suite and strict all-target Clippy pass | Full suite and strict all-target Clippy pass | Not a requested separate CLI gate |
| Core | Full phase runner passes; native remainder 2103 passed/0 failed/1 ignored | Full phase runner passes; native remainder 2672 passed/0 failed/7 ignored | Full phase runner passes; native remainder 2672 passed/0 failed/7 ignored |
| Python | 992 passed,18 candidate-only skips | 1010 passed,0 skipped | 1010 passed,0 skipped |

Core runners include normal preceding phases with 18/129/1 passes, package integrations with 69 passes
and doctests. These are phase counts, not an additive unique-test census.
Standalone transport passes 185 default and 203 sequenced tests, each with 2
inherited ignores and doctests. Strict transport library Clippy passes.
Core candidate library Clippy exits0 retaining 49 warnings, explicitly unwaived.
Python strict library Clippy all three modes, generators/protocol drift,
source guards and fresh module provenance checks pass. CLI source guards pass.
Exact core commit gates and unchanged checker 7 unit tests were already green.

Rust 1.95, retained debug/incremental profiles and bounded four-thread core
runs. Candidate manifests/source/dependency paths all anchor actual MAIN;
separate caches prevent ownership conflicts. CLI ordinary target was handed
to Python before its cross-driver fixture. That fixture rebuilt the CLI with
changing binary hashes but identical accepted source revisions/digests; no
stale binary was substituted. Native extension hashes stayed unchanged through
test-only correction and final suites; unnecessary native builds/lints were
not repeated. Default-concurrency predecessor SSH failure remains historical,
with isolated and bounded-concurrency passes, not a relaxed timing assertion.

CLI receipts precede the independent Python test-only/root-doc transition;
its compiled CLI/core inputs remained unchanged. Core explicitly records the
original and corrected tuples while its compiled inputs stayed frozen. Final
Python full-run start/end snapshots exactly match corrected six-head tuple,
tracked/untracked hashes and module hashes. Root verified the final gate exits and
recorded suite log hashes and all 27 preserved draft hashes before this documentation commit.

Coordinator records now reside in the private evidence member; public builds do
not depend on private evidence:

- main-credential-cli-integration/IntegrationReport.md and exact gate/hash receipts.
- main-credential-core-integration/IntegrationValidation.md and integration-receipt.json,
  SHA-256: `4cd60ac323f731ed98745550acdf2bf220680d6f0bb159b03176d4ee4a18d7cd`.
- main-credential-py-integration/final-gates/report.json,
  SHA-256: `42b3494ceabe68cf268f309ab8bf964ff202a5e3beb6ab03ff2306ffccc7dcd3`.

These directories are retained under
`gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-03-coordinator-artifact-relocation/imported/prewarm/`.
Their original bytes and hashes are unchanged. The former external paths are
historical; [the artifact-location record](GwzTransportArtifactLocations.md)
maps them to the archive and consolidated retained build/runtime copies.
Production review records and acceptance live in gwz-core/dev-docs; full test-
correction Code report lives beside this document. Retained ignores/warnings,
external-candidate revision-unavailable provenance and ordinary core dirty=true
for known untracked drafts are disclosed, not broadened to release qualification.

## Remaining release work

Windows TR1.8 design/provider/identity/trust and parity proof remains NO-GO.
Complete its bounded proof and design review before implementation/qualification.
Deferred platform/performance and selected-source/package checks remain one
later batch, followed by aggregate release/activation review under the accepted
1.1.0 plan. Core warnings and inherited CLI formatting debt remain unwaived.
Native Git/OS copies and initial configuration-discovery limitations retain their
accepted scope. Supplied-carrier/iroh and session/server outcomes remain deferred.

No push, tag, publication, activation or alpha installation occurred. tr2-22
root history remains retained at f8212e7540121f193b8159135c2c6363170dc537;
member-only merge does not authorize lane disposal. Its older Python WIP stash
and hashed external backup remain preserved; do not pop over accepted MAIN.
