# R0 retained-reader post-implementation review

Date: 2026-08-03

## Verdict

**Approved for commit and the exact-SHA cross-platform evidence run.**

No P0, P1, P2, or P3 source finding remains in the retained-reader portability
remedy. The final code closes the false-positive paths identified in
`GwzM5-8R0Review-Harness-5.md`, including the additional nested Git-admin,
index-flag, and authoritative-output symlink cases found during this
post-implementation review. Checked evidence was regenerated after the last
source correction and compares equal under the reviewed portable projection.

This is not yet final R0 platform acceptance. A committed-SHA rerun must still
pass both harness-unit jobs and all five required Linux, Windows, and macOS
behavioral lanes. In particular, Windows must execute the applicable readers,
and both Linux architectures must exercise the optional physical
`.git/info/exclude` rewrite while passing the exact live-boundary assertion.

## Outstanding findings

- **P0:** none.
- **P1:** none.
- **P2:** none.
- **P3:** none.

The only remaining gate is the committed-SHA cross-platform run, not an
implementation finding.

## Findings discovered and closed during this review

### Nested Git administration was initially too broad

The first post-implementation snapshot ignored the complete `.git/hooks` and
`.git/branches` trees and did not classify physical files nested below refs or
object storage. Direct probes demonstrated that all of the following could be
added without changing fixture identity:

- an executable `hooks/pre-commit` that exits nonzero;
- a legacy `.git/branches` authority file;
- `refs/heads/main.lock`;
- an unclassified `objects/info` file; and
- external symlink indirection for authoritative `config` or `info/exclude`.

These were P2 authenticity false negatives because they can alter reader
behavior while satisfying the fixture golden.

The final implementation recursively inventories the real Git administration
tree, rejects symlinks and non-regular entries, and uses a closed
classification for files and directories. Only reviewed editor, description,
reflog, maintenance, sample-hook, and storage-layout state is ignored. Active hooks, legacy
branch authority, ref/reflog locks and namespace-blocking directories,
unknown object metadata, filesystem indirection, and unclassified operation
state fail closed. Replacement refs remain visible in the complete semantic
ref map, while unclassified loose replacement-ref storage is rejected. The
regressions cover both file and directory forms.

### Intent-to-add was initially absent from semantic index identity

The initial index model retained path bytes, mode, stage, OID, skip-worktree,
assume-unchanged, fsmonitor, and resolve-undo state, but a normal staged empty
file and an intent-to-add empty file produced the same identity. A first
porcelain-status correction caught a present intent-to-add path but missed the
same index entry after its worktree file was deleted.

The final implementation compares binary cached-name observations under
Git's `--ita-visible-in-index` and `--ita-invisible-in-index` modes and rejects
any difference. Independent probes confirm that both present and deleted
intent-to-add entries now fail closed. Valid stage-1/stage-2 unmerged entries
remain representable, while skip-worktree, assume-unchanged, fsmonitor-valid,
resolve-undo, and intent-to-add state are rejected explicitly.

### Authoritative postconditions initially followed symlinks

The first otherwise-complete remedy used `Path.is_file()` before reading the
live `.git/info/exclude`. A leaf symlink to an external same-byte file
therefore produced the same merge-record semantic digest and satisfied the
optional mutation contract. The same primitive appeared on other durable
postcondition surfaces. This was a P2 false negative: evidence could attest to
external indirection instead of a durable workspace-owned output.

The final implementation uses a shared regular-leaf check and guarded text
reader. It rejects symlinks and non-regular entries for:

- the live boundary, publication marker, lock, and candidate-hash files;
- single and set-valued YAML semantic observations;
- archived-record baseline preservation;
- generic `path:file` and `path:directory` postconditions;
- ref and reflog semantic normalization; and
- object and index semantic mutation identities.

Repository postconditions also reject a symlinked repository leaf. Regressions
cover the live boundary, archived record, lock YAML, and generic file path,
alongside the existing non-regular Git-admin directory cases. An independent
same-byte archived-record symlink probe now produces both the expected path
failure and YAML-semantic failure.

### Stage and detached-HEAD coverage was completed

The final regression matrix now proves that unmerged index stages change
identity and that detaching HEAD at the same OID changes identity while
retaining `symbolic: null`. This closes the remaining P3 test-completeness
portion of the Harness-5 contract.

## Accepted implementation

### Live workspace-boundary assertion

Both continuation cases allow exactly zero or one physical mutation of
`text:.git/info/exclude`. Optionality applies only to whether the physical path
changes. The archived merge-record semantic check independently requires:

- `baseline_boundary_sha256` to match `baseline_boundary_text`;
- `boundary_sha256` to match `boundary_text`; and
- the live root `.git/info/exclude` to be a regular non-symlink file whose
  bytes equal `boundary_text` exactly.

The normalized mutation identity retains occurrence count and resulting
content. Positive zero/one mutation tests and a corrupted-content negative
test pass. The exact Linux count-one path remains a required CI execution
gate, not an unverified local claim.

### Storage-independent repository identity

Fixture identity now has separate durable-workspace and repository-semantic
sections. Each root/member repository retains:

- SHA-1 object and repository formats;
- symbolic/detached/unborn HEAD state and resolved OID where applicable;
- the complete `for-each-ref` name/OID/type/symref map;
- present `ORIG_HEAD` state, with all other unclassified pseudorefs or Git
  operation state rejected;
- byte-safe index path, mode, stage, and OID tuples plus explicit extended-
  state rejection;
- exact-order local config and canonical text-boundary content; and
- the complete all-object OID/type/size set after strict full-object `fsck`.

The identity is unchanged after loose-to-pack object conversion, packed refs,
`update-server-info`, monolithic and split commit graphs, and MIDX creation.
Object addition changes identity; missing or same-path-corrupt objects fail.
Alternates and unclassified object authority fail. Ref line-ending
normalization and non-UTF-8 index path bytes are covered explicitly.

### Evidence integrity and behavioral preservation

Checked macOS arm64 evidence was regenerated after the final
authoritative-symlink correction. A fresh matrix run compares equal under the
reviewed portable projection, and the checked evidence SHA-256 is
`fb777940f0d14fda3a2125b763bebd7e79d6b8bea9463c001c73bee2a5cd65a3`.

The final matrix contains exactly 38 expected result keys:

- 37 executed rows passed;
- one unavailable `gwz-py` v0.10.0 tuple is declared unsupported; and
- no row failed or was silently omitted.

Compared with the pre-remedy checked evidence, the result-key set is identical.
Only the six continuation rows changed `after_invariant_sha256`: two cases
across the three durable-v0 readers. Exit codes, typed outcomes,
postcondition counts, artifact identities, and every unrelated invariant are
unchanged. The fixture generator itself is unchanged; fixture and archive
semantic digests changed because the identity/evaluator contract became more
complete, not because v0 fixture record bytes changed.

## Verification performed

Focused late-delta verification:

```text
Focused boundary/archive/lock/path symlink regressions
passed

Independent same-byte archived-record symlink probe
path and YAML semantic postconditions both failed closed

python3 -m compileall -q scripts/retained_readers
passed

retained_reader_harness.py validate
status=valid, tuple_count=36

retained_reader_harness.py gate-ready
status=manifest-ready, tuple_count=36

git diff --check
passed
```

Final compatibility closure after evidence regeneration:

```text
python3 -m unittest discover -s scripts/retained_readers -p 'test_*.py'
Ran 76 tests in 78.695s — OK

retained_reader_harness.py validate
status=valid, tuple_count=36

retained_reader_harness.py gate-ready
status=manifest-ready, tuple_count=36

macOS arm64 retained-reader matrix
38 rows: 37 passed, 1 declared unsupported, 0 failed

retained_reader_evidence.py compare
status=equal

checked evidence SHA-256
fb777940f0d14fda3a2125b763bebd7e79d6b8bea9463c001c73bee2a5cd65a3

python3 -m compileall -q scripts/retained_readers
passed

git diff --check
passed
```

Additional independent probes covered repacking, packed refs, commit-graph and
MIDX layout, object corruption, missing objects, normal ref and ORIG_HEAD
changes, valid unmerged stages, all reviewed index flags, active hooks,
legacy-branch state, file/directory ref locks, unknown object metadata, and
authoritative-file symlinks.

## Scope and budget

The retained-reader package is **7,071 physical lines across 23 files**, below
the reviewed **7,100-line/23-file** ceiling. The largest Python module is 497
lines; all implementation and test modules remain below 500 lines.

The implementation diff is confined to existing
`gwz-core/scripts/retained_readers/**` test/tool/documentation files. The
workspace-level changes outside that directory are the R0 inventory, budget,
and review documents. There is no merge production-code, public protocol,
wire-value, record-schema, record-byte, or released-feature delta.

## Decision

The retained-reader portability remedy is ready to commit. The full 76-test
suite, a fresh macOS arm64 matrix, and portable evidence equality all pass
against the regenerated checked file. R0 may be accepted and R1 dispatched
only after the committed exact SHA passes both harness-unit jobs and all five
required behavioral platform lanes with complete evidence and attestations.
