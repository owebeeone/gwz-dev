# GWZ M5–M8 R0 Retained-Reader Final Compatibility Re-review

Date: 2026-08-01

Scope:

- the residual remediation following
  `dev-docs/GwzM5-8R0Review-Compat-2.md`;
- the frozen retained-reader manifest, cases, fixtures, content-semantic
  evaluator, checked evidence, and workflow under
  `gwz-core/scripts/retained_readers/**` and
  `gwz-core/.github/workflows/retained-readers.yml`; and
- the actual v0.9.2, v0.10.0, and v0.10.2 readers available on macOS arm64.

## Verdict

There are **no P0, P1, P2, or residual P3 compatibility findings**. The two
P3 findings from `GwzM5-8R0Review-Compat-2.md` are resolved. From the
retained-reader compatibility side, the R0 implementation checkpoint is ready
to commit.

R0 is not yet accepted and R1 must not be dispatched immediately after the
local commit. The committed checkpoint must first pass the required external
workflow on that exact SHA. This is a platform-evidence condition that can
only be satisfied after a commit exists; it is not an implementation defect
and does not block creating the checkpoint commit.

## Findings

No implementation findings.

## Resolution of the two prior P3 findings

### Compat-2 P3-1: checked-evidence freshness was not fully enforced

**Resolved.** `retained_reader_evidence.py` now defines a 14-file canonical
evidence source set covering the generator, evaluator, matrix orchestration,
evidence normalization/comparison, runtime/bootstrap, process handling,
schemas, semantic reducer, and fixture contract. Checked evidence carries the
individual source hashes plus their aggregate hash. Validation recomputes and
compares:

- the manifest and case input hashes;
- the exact unique expected `(reader, case)` result set, including the explicit
  unsupported row;
- all source hashes and their aggregate;
- the generator hash;
- every fixture identity and the fixture-set identity; and
- the complete provenance shape.

The checked-evidence unit test regenerates the fixtures and calls this complete
validator. Adversarial tests reject a duplicate result substituted for a
required result and a stale evaluator-source digest. The workflow also compares
fresh macOS arm64 evidence with the checked portable semantic projection,
excluding only reviewed host-variable Git/Python version and Python executable
digest fields.

I ran the full actual-reader lane twice using the exact recorded CPython
3.10.15 executable. Both normalized evidence files had SHA-256
`c466aef2a855e65c9056367b795bbec62a3227f217770340611c761ec90e60be`;
they were byte-identical to each other and to the checked evidence. Both
portable comparison invocations returned `equal`.

### Compat-2 P3-2: archive status and GC did not use a custom-message archive

**Resolved.** `archived-v0` is now derived from
`custom-message-pending`, transitions the operation and participant to
`aborted`, removes the pending action, and retains
`commit_message: custom retained-reader message`. It contains no `mode: no_ff`.
The generated fixture identity matches the reviewed fixture contract.

Archive-only status and explicit GC execute against this fixture through:

- v0.10.0 Rust;
- v0.10.2 Rust; and
- v0.10.2 Python.

All six reader/case executions passed. Status is read-only and checks the
complete archived-record semantic digest, including the custom message and
baseline evidence. GC deletes exactly the bound archive path; the case's input
fixture identity proves which complete archive was supplied.

## Frozen contract assessment

The manifest remains an exact six-reader by six-platform contract: 36 tuples,
each required or explicitly unsupported according to the frozen R0 support
classification. Validation and `gate-ready` both report 36 tuples. The case
document contains 13 cases and the macOS arm64 evidence contains the exact 38
unique expected results: 37 passing executable reader/case rows plus the one
explicitly undistributed `gwz-py` v0.10.0 row.

The command contract still covers:

- v0.9.2 Rust/Python workspace status and legacy branch merge as the explicit
  pre-record downgrade boundary;
- v0.10.0 Rust and v0.10.2 Rust/Python status, continue, abort, preserve, and
  GC where applicable;
- custom-message execution, post-execution reconciliation and rollback;
- wrong-message fail-closed behavior with no mutation;
- custom-message archive projection and deletion; and
- the frozen dormant-v0 `no_ff` incompatibility.

Case validation requires the frozen command set and canonical fixture digest
for every case. Evidence validation derives its result keys from the complete
manifest/case relationship rather than trusting a count or case-name subset.

## Durable-content and unknown-field assessment

The 13 cases carry 59 postconditions, including 17 content-semantic checks.
The evaluator now observes content rather than only changed path names:

- complete canonical lock/member data;
- marker count and normalized marker meaning;
- root publication branch, parent, message, tree/commit semantics;
- root and member Git-index semantics;
- Git refs, parent counts, and rollback targets; and
- the complete archived merge record, with publication consistency checks and
  all otherwise unknown fields included in the semantic digest.

Every applicable v0.10.0 rewrite/archive case separately compares the exact
`lock_yaml`, `manifest_yaml`, `lock_commit_sha256`, and
`manifest_commit_sha256` values before and after the old reader rewrites the
record. The whole-record semantic digest supplements that targeted check, so a
change to the top-level fixture-only unknown field or any other durable record
content also fails. Adversarial tests confirm that unknown-field removal or
alteration, lock-member corruption, marker-meaning changes, and stale durable
contents are detected.

Normalized mutation evidence retains exact content identities and, for dynamic
object/marker classes, both the observed count and content digest. Consequently
two runs cannot compare equal merely because the same path patterns changed.

## Checked evidence and local verification

Performed from `gwz-core` on macOS arm64:

- retained-reader unit/adversarial suite: **62 passed**;
- Python syntax compilation for every retained-reader module: **passed**;
- manifest validation: **valid, 36 tuples**;
- `gate-ready`: **manifest-ready, 36 tuples**;
- independent contract audit: **13 cases, 59 postconditions, 17
  content-semantic checks, 14 source-provenance entries**;
- exact result-set audit: **38 expected, 38 present, all unique**;
- fixture regeneration: **all fixture and fixture-set identities match the
  reviewed contract**;
- full offline macOS arm64 matrix, run twice with the recorded CPython 3.10.15
  runtime: **37 passed, one declared unsupported, zero failures per run**; and
- checked-evidence comparison: **both runs portable-equal and byte-identical
  to the checked file**.

The executed-commit abort, wrong-message rejection, preserve-abort,
custom-message archived status, custom-message archive GC, unknown-field
rewrite, complete durable-content, legacy pre-record, and dormant-no-ff rows all
passed through their actual pinned retained readers.

## Required external committed-SHA evidence gate

After committing the R0 checkpoint, run the `Retained merge readers` workflow
on that exact commit. Do not accept R0 or dispatch R1 until all of the following
are green:

1. `Harness unit tests (ubuntu-24.04)`;
2. `Harness unit tests (windows-2022)`;
3. `linux-x86_64` actual-reader behavioral matrix;
4. `windows-x86_64` actual-reader behavioral matrix;
5. `linux-aarch64` supported-platform lane;
6. `macos-x86_64` supported-platform lane; and
7. `macos-aarch64` supported-platform lane, including portable comparison with
   the checked evidence.

Every platform job must upload both normalized evidence and its separate
attestation. Each attestation must bind the evidence digest, platform,
`GITHUB_SHA`, and `GITHUB_RUN_ID`. The Linux and Windows x86_64 executions are
the mandatory cross-platform behavioral proof; Linux arm64 and both macOS
architectures complete the supported-platform evidence. Windows arm64 remains
an explicit unsupported manifest tuple rather than a missing or silently
skipped job.

## Gate decision

- **Commit the R0 checkpoint:** approved; no P0/P1/P2 implementation blocker.
- **Treat the local checkpoint as complete platform evidence:** not approved.
- **Dispatch R1 after the exact committed-SHA workflow gate above succeeds and
  its evidence/attestations are retained:** approved from the retained-reader
  compatibility side.
