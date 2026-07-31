# GWZ M5–M8 R0 Retained-Reader Compatibility Review

Date: 2026-08-01

Scope:

- the compatibility corrections in `dev-docs/GwzM5-8Refactor.md`;
- `dev-docs/GwzM5-8R0Inventory.md`;
- `gwz-core/scripts/retained_readers/**` and the retained-reader workflow; and
- the released v0.9.2, v0.10.0, and v0.10.2 reader generations.

## Verdict

The historical-reader classification is now factually correct:

- v0.9.2 is a pre-record downgrade boundary, not a v0 decoder;
- v0.10.0 is the durable-v0 reader that treats the later baseline evidence as
  unknown fields; and
- v0.10.2 knows those fields and the dormant `NoFf` variant, while recovery
  can still re-derive normal fast-forward behavior.

The artifact inventory is also factual. I acquired every required object in
the manifest from its recorded URL; all 26 unique required reader/runtime
artifacts matched their pinned SHA-256 digest. The explicit absence of a
distributed `gwz-py` v0.10.0 artifact is correct, and the source tags and
released command surfaces agree with the manifest.

There are no P0 or P1 findings. Four P2 findings prevent the retained-reader
work from closing R0 and therefore prevent the executable plan's R0 → R1 gate
from opening. One P3 should be corrected in the same pass.

## Findings

### [P2-1] Required cross-platform evidence has not yet run

**Where:** `gwz-core/.github/workflows/retained-readers.yml:39-95`,
`gwz-core/scripts/retained_readers/README.md:115-126`, and
`dev-docs/GwzM5-8R0Inventory.md:342-346`.

The workflow correctly defines Linux x86_64 and Windows x86_64 behavioral
jobs, plus Linux arm64 and both macOS artifact lanes. At review time, however,
the only durable result is the checked macOS arm64 evidence. The README calls
the Linux/Windows runs remaining work, and the inventory correctly says R0 is
not accepted until those runs and supported-platform smoke evidence exist.

This directly leaves proposal §§22.1.4 and 15.8 unsatisfied. `gate-ready`
proves metadata completeness; it does not execute a non-native artifact.

**Required correction:** commit the R0 foundation, run the workflow on that
exact commit, require all five supported runner jobs to pass, retain the
normalized outputs, and record their workflow run/commit identity in the R0
handoff. Do not open R1 merely because local macOS and metadata gates pass.

### [P2-2] The custom-message abort case does not exercise post-mutation recovery

**Where:**
`gwz-core/scripts/retained_readers/generate_retained_reader_fixtures.py:237-249,261`,
and `gwz-core/scripts/retained_readers/cases.json:42-63`.

`custom-message-pending` leaves the member at `before_commit`; the exact
prepared true-merge commit has not executed. The case named
`v0-custom-message-pending-abort-after-restart` therefore observes
`PendingActionReconciliation::NotStarted` and archives an abort without
reconciling or rolling back a commit. Its mutation policy deliberately permits
only `.gwz` changes, confirming that no member mutation is under test.

That does not prove the important abort half of the message-authority claim:
after Git created the exact prepared commit but before its outcome row was
persisted, an old reader must recognize the commit using the frozen custom
message, adopt it, and roll it back. Continue proves execution uses the
message; the present abort case does not prove reconciliation uses it.

**Required correction:** add a second canonical fixture whose member HEAD is
the exact two-parent prepared commit, with the operation record still carrying
the pending action. Run abort through v0.10.0 Rust and v0.10.2 Rust/Python;
assert successful exact adoption and rollback to `before_commit`, removal of
the feature path, one-parent baseline HEAD, and an archived Aborted record.
Add a negative twin whose commit message differs and assert fail-closed,
no-mutation behavior.

### [P2-3] The field-unknown v0.10.0 lane does not prove unknown-field preservation

**Where:** `gwz-core/scripts/retained_readers/cases.json:18-63`, and
`gwz-core/scripts/retained_readers/generate_retained_reader_fixtures.py:180-234`.

The canonical record intentionally contains the four baseline fields added
after v0.10.0: exact lock/manifest YAML and their committed digests. Continue
and abort make v0.10.0 rewrite/archive the record, which is the right test
shape, but the postconditions check only that the archive exists. They never
inspect the archived YAML. A reader could drop or alter every unknown baseline
field and the matrix would still pass.

The v0.10.0 generation exists in this matrix specifically to prove that
additive unknown evidence survives rewriting. Proposal §15.1 also requires
record-rewrite equivalence and preserved unknown fields.

**Required correction:** add a byte/YAML postcondition that proves all four
fields and their exact values survive each v0.10.0 mutating case. Prefer a
general postcondition comparing a declared set of paths/values in the input
open record and output archive, with a deliberate drop/alter failure test.

### [P2-4] The checked evidence does not bind the fixture or Python runtime used

**Where:**
`gwz-core/scripts/retained_readers/retained_reader_evidence.py:39-101`,
`gwz-core/scripts/retained_readers/retained_reader_matrix.py:142-164,421-447`,
and `gwz-core/scripts/retained_readers/test_retained_reader_cases.py:34-63`.

The evidence input identity contains only the manifest and cases digests. It
does not contain the generated fixture-tree digest, fixture-generator digest,
Git version, Python executable/version/architecture, or the runtime identity
actually used. The Python cache identity likewise includes the runtime
manifest and wheel digest but not the interpreter. A cache created with one
Python can therefore be reused when `--python` names another.

I reproduced the checked evidence semantically on macOS arm64, but the local
run used Python 3.14.3 while the evidence labels its runtime only as
`cpython-3.10-abi3`; the normalized file cannot reveal that distinction. CI is
configured to install Python 3.10, but the checked JSON by itself does not
prove it was produced by that job or interpreter.

**Required correction:** record and validate a canonical fixture-set digest,
the generator digest/revision, actual platform identity, Python implementation
and exact version/architecture, and the workflow commit/run identity. Include
the resolved interpreter identity in the Python runtime cache key/marker and
fail when it does not satisfy the manifest runtime contract. Extend the
checked-evidence freshness test accordingly.

### [P3-1] Command coverage and mutation checks are weaker than the declared gate

**Where:** `gwz-core/scripts/retained_readers/cases.json:18-164`, and
`gwz-core/scripts/retained_readers/retained_reader_matrix.py:207-239,366-410`.

The manifest declares preserve and GC available on the durable-v0 readers,
but no case invokes either command. Case validation requires only that each
runnable reader appears somewhere; it does not require every applicable
available command named by proposal §15.1.3. Mutating cases also allow broad
`text:member/*`, `text:.git/*`, and `text:.gwz/merge/*` patterns, so an
unintended mutation within those trees can pass if the handful of required
paths and final postconditions still match.

**Required correction:** make case validation consume a per-shape applicable
command matrix and fail when an applicable command lacks a case. Add
custom-message preserve and archived status/GC cases before M5a. Narrow broad
mutation globs or supplement them with explicit invariant postconditions for
source refs, unrelated refs/files, baseline artifacts, and archive contents.

## Checked evidence assessment

The macOS arm64 behavior reported in
`evidence-macos-aarch64.json` is reproducible: I regenerated all 22 executable
reader/case results with zero failures, observed the one declared unsupported
`gwz-py` v0.10.0 tuple, and obtained a semantically identical normalized file.
The no-ff fixture reliably pins the unsafe one-parent fast-forward result for
both v0.10.x Rust readers and the v0.10.2 Python reader. The v0.9.2 status and
legacy merge cases reliably prove that both old surfaces ignore the open v0
record, with the legacy merge advancing the member while leaving the record in
place.

Accordingly, I trust those observed outcomes. I do **not** regard the checked
JSON as a self-contained, durable release-gate proof until P2-4 binds it to
the fixture bytes and actual runtime. It also cannot substitute for the
required Linux/Windows jobs in P2-1.

## Verification performed

- Historical source inspection at `v0.9.2`, `v0.10.0`, and `v0.10.2` for
  command availability, durable model fields, pending-action reconciliation,
  and the v0.10.0→v0.10.2 baseline-field diff.
- `python3 -m unittest -v scripts/retained_readers/test_retained_reader_harness.py scripts/retained_readers/test_retained_reader_matrix.py scripts/retained_readers/test_retained_reader_cases.py`: 37 passed.
- `retained_reader_harness.py gate-ready`: manifest ready, 36 declared
  surface/release/platform tuples.
- Network acquisition of every required manifest artifact: 26 unique objects,
  all SHA-256 checks passed.
- Full macOS arm64 actual-reader matrix regeneration: 22 passed, one declared
  unsupported, zero failures; normalized evidence semantically matched the
  checked file.

## R1 gate decision

**R1 is not unblocked by the retained-reader side of R0.** There is no P0/P1,
but P2-1 through P2-4 are compatibility-evidence defects or missing mandatory
evidence. Correct the fixtures/provenance checks, run the committed workflow
on all required lanes, and have the resulting evidence re-reviewed before the
lead opens R1.
