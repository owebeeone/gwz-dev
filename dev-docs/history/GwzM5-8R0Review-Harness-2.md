# R0 retained-reader harness remediation re-review

## Verdict

The remediation closes the original artifact-tuple, schema-enforcement,
machine-output, derived-cache, URL, Git-environment, process-tree, and archive
alias defects. The new adversarial suite exercises the former repros and is a
substantial improvement.

Two P2 correctness gaps remain. They allow checked evidence to appear current
while omitting a required result, and allow an expected mutated path to carry
incorrect durable content without failing the case. There are no P0 or P1
findings, but these P2s block committing the change as the accepted R0
checkpoint and block dispatching R1 under the documented `R0 -> R1` gate.

The absence of Linux/Windows workflow results is a separate evidence condition,
not an implementation defect. After the two P2s are corrected and committed,
the exact external gate in the final section must pass on that commit before R0
is accepted and R1 begins.

## Remaining findings

### [P2-1] Checked evidence can be stale or incomplete while its guard remains green

**Where:** `gwz-core/scripts/retained_readers/test_retained_reader_cases.py:34-65`;
`gwz-core/scripts/retained_readers/retained_reader_evidence.py:29-67,143-222`;
`gwz-core/.github/workflows/retained-readers.yml:85-104`

The checked-evidence test validates manifest/cases digests, a hard-coded result
count, known case names, status values, two provenance literals, and canonical
JSON formatting. It does not require the exact expected
`(reader, case)` result set or uniqueness. Replacing one result with a duplicate
of another leaves all current predicates true while reducing the unique result
set from 38 to 37. The omitted reader/case result is therefore invisible.

The test also does not recompute or compare the checked
`generator_sha256`, `evaluator_sha256`, fixture mapping/set, or the complete
execution-evaluator source identity. Replacing the checked generator digest
with 64 zeroes still satisfies the current test predicate. In addition,
`evaluator_sha256` covers only `retained_reader_fixture.py` and
`cases.schema.json`; it excludes `retained_reader_matrix.py`,
`retained_reader_evidence.py`, `retained_reader_schema.py`,
`retained_reader_process.py`, and `retained_reader_runtime.py`, all of which can
change the meaning or production of evidence.

The workflow generates and uploads fresh evidence but no longer compares its
stable semantic projection with `evidence-macos-aarch64.json`. Consequently the
required workflow can pass while the checked artifact remains stale. Exact
byte comparison is not appropriate for Git/Python patch-version provenance,
but removing comparison entirely leaves the checked-evidence claim unenforced.

Build the exact expected result keys from `manifest.json` and `cases.json`,
including one explicit unsupported row, and require equality and uniqueness.
Recompute the fixture contract/set and every checked source digest. Define a
complete harness/evaluator digest over all evidence-affecting modules. Finally,
compare a portable semantic projection of the CI-generated macOS evidence with
the checked evidence; keep volatile Git/Python/run identities in the separate
attestation or explicitly exclude only those reviewed fields from comparison.

### [P2-2] Mutation contracts pin paths but not the contents of critical durable outputs

**Where:** `gwz-core/scripts/retained_readers/cases.json:29-52,110-130,295-307`;
`gwz-core/scripts/retained_readers/retained_reader_fixture.py:213-288,323-438`;
`gwz-core/scripts/retained_readers/retained_reader_matrix.py:176-188`

Replacing broad `allow` patterns with exact paths and bounded dynamic classes
closes the unrelated-path deletion issue, but the evaluator still compares only
the set of changed path names. For the continue and legacy-merge cases,
`gwz.conf/gwz.lock.yml`, the root branch/index/object files, and publication
marker content are authorized mutations without content-sensitive
postconditions. The merge-record check preserves four unknown baseline fields,
and member Git checks are useful, but neither proves the accepted lock bytes,
root publication result, or the remainder of the archived record.

An independent synthetic repro supplied the exact current continue mutation
set and valid typed output while assigning a deliberately corrupt digest to
`gwz.conf/gwz.lock.yml`; `evaluate_expectation` returned no errors. The
normalized after-invariant also hashes exact path names and dynamic pattern
declarations, not the changed entries' values. For dynamic classes it does not
even retain the observed count. Thus different durable bytes can yield the same
passing invariant and checked evidence.

Add semantic observations for the complete accepted lock/YAML hash and member
row, exact root and member ref targets, normalized root publication commit/tree,
marker meaning, and complete archived-record semantics including preserved
unknown fields. Hash those observed values into `after_invariant_sha256`.
Where output is fully deterministic, use exact file or normalized-structure
digests; where UUID/object paths are intentionally variable, normalize their
names but retain their counts and content hashes. Apply the same protection to
the v0.9.2 legacy lock rewrite.

## Original finding disposition

| Original finding | Re-review disposition |
|---|---|
| P2-1 missing platform tuple/support reclassification | **Closed.** Exact reader/platform cross-product and frozen R0 support classes are enforced and adversarially tested. |
| P2-2 unfrozen fixture/evidence identity | **Fixture identity closed; checked-evidence freshness remains open as P2-1 above.** Fixtures have reviewed logical digests and hostile Git environment isolation. |
| P2-3 checked schemas not enforced | **Closed.** The fail-closed stdlib evaluator enforces the checked schemas; typo, invalid union, unknown field, and duplicate tests pass. |
| P2-4 invalid JSON/null outcome accepted | **Closed.** Every checked case uses a typed JSON contract and evidence rejects absent/unparseable outcomes. |
| P2-5 broad mutation allowlists | **Path-scope portion closed; durable-content proof remains open as P2-2 above.** |
| P2-6 unverified derived cache execution | **Closed.** Only verified raw objects are cached; trees and venvs are fresh per matrix run and interpreter identity participates in runtime identity. |
| P3-1 arbitrary mutable HTTPS provider | **Closed.** Only reviewed GitHub/Python-hosted forms are accepted. |
| P3-2 inherited Git-control environment | **Closed.** Generation uses an allowlisted environment and forces SHA-1; the hostile-environment repro passes. |
| P3-3 process/Windows extraction isolation | **Implementation closed.** Descendant termination and Windows-normalized archive rejection are tested; actual Windows execution remains an external CI condition. |

## Independent verification

Run locally from `gwz-core` on macOS arm64:

- full retained-reader unit suite: **57 passed**;
- Python syntax compilation for the retained-reader package: **passed**;
- manifest validation: **passed**, 36 tuples;
- `gate-ready`: **passed**, 36 tuples;
- prior missing `windows-aarch64` tuple repro: **rejected**;
- prior misspelled postcondition repro: **rejected**;
- prior token-only `--json` repro: **rejected as invalid JSON**;
- prior arbitrary HTTPS provider repro: **rejected**;
- hostile Git environment, derived-cache poison, descendant timeout, and
  Windows archive alias adversarial tests: **passed**;
- checked-evidence duplicate/omission repro: **still passed the current guard**;
- stale generator-identity repro: **still passed the current guard**; and
- corrupt accepted-lock content under the exact continue path contract:
  **still produced no expectation error**.

The third-party `jsonschema` package was not available in this environment, so
the checked schemas were exercised through the repository's fail-closed stdlib
validator and negative tests rather than a second Draft 2020-12 implementation.
I did not reacquire or execute the external release-artifact matrix during this
re-review.

## Exact remaining external evidence gate

Once P2-1 and P2-2 are corrected, create and push the candidate R0 checkpoint.
Do not mark R0 accepted or dispatch R1 until the required
`Retained merge readers` workflow succeeds on that exact commit with:

1. `Harness unit tests (ubuntu-24.04)`;
2. `Harness unit tests (windows-2022)`;
3. the `linux-x86_64` actual-reader behavioral matrix;
4. the `windows-x86_64` actual-reader behavioral matrix;
5. the `linux-aarch64` supported-artifact lane;
6. the `macos-x86_64` supported-artifact lane; and
7. the `macos-aarch64` supported-artifact lane.

Every matrix job must upload both normalized evidence and its attestation bound
to that evidence digest and the same commit/run identity. The Linux and Windows
x86_64 behavioral jobs are the mandatory R0 compatibility evidence; the other
three jobs complete the supported-platform artifact evidence. Windows arm64
remains an explicit unsupported tuple, not a job or a skip. Repository rules
must make the workflow required, and the coordinated release gate must invoke
its `workflow_call` entry for record-affecting Rust/Python driver changes that
do not independently trigger the core repository's path filters.
