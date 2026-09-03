# R0 retained-reader post-implementation review

Date: 2026-08-03

## Baseline-selection correction

This notice supersedes every release tag, tuple/result count, evidence digest,
test count, and package-size value in the historical review narrative below.
The reviewed policy now selects the latest successful pre-change
durable-record release and leaves its exact pin to the executable manifest. For
this checkpoint that pin is v0.10.2; v0.9.2 remains only the separate
pre-record downgrade lane.

The corrected contract has 24 reader/platform tuples and checked macOS arm64
evidence with 26 passing rows and no unsupported-release placeholder. The
evidence SHA-256 is
`5873033e684cd2d05d236417c3b791d8629c3add1c697a3196b01fda84d1fa53`.
The suite has 80 passing tests. The package is 6,968 lines across 23 files,
with a largest Python module of 497 lines. The frozen reader/command contract
is now enforced unconditionally by public validation and has a rename-all plus
drop-command regression.

## Verdict

**Approved for commit and a replacement exact-SHA cross-platform evidence
run.**

No P0, P1, P2, or P3 source finding remains in the retained-reader portability
remedy. The final code closes the false-positive paths identified in
`GwzM5-8R0Review-Harness-5.md`, including the additional nested Git-admin,
index-flag, and authoritative-output symlink cases found during this
post-implementation review. It also closes the Windows fixture-generation
failure and the subsequent Windows harness false-green risk exposed by the
committed-SHA platform runs. Checked evidence is regenerated after the final
evidence-bound generator correction and a separate fresh matrix run compares
equal under the reviewed portable projection.

This is not yet final R0 platform acceptance. Run `30752346550` passed the
Ubuntu harness job and the Linux and macOS behavioral lanes. GitHub marked the
Windows harness job successful, but its unit command actually reported one
failure and 18 errors that later successful validation commands masked. The
Windows behavioral lane then failed before reader execution. A replacement
committed-SHA run must pass both harness-unit jobs and all five required Linux,
Windows, and macOS behavioral lanes. In particular, Windows must execute the
applicable readers, and both Linux architectures must exercise the optional
physical `.git/info/exclude` rewrite while passing the exact live-boundary
assertion.

Replacement attempt `30753147192` confirmed that the corrected Windows
harness step now fails fast. It exposed exactly two remaining host-dependent
test/input errors: an intentional missing-object unlink against a read-only
loose Git object, and evidence source digests changed by CRLF checkout
translation. Both are corrected in the current source. Because those
corrections postdate the run, another committed-SHA run remains required.

## Outstanding findings

- **P0:** none.
- **P1:** none.
- **P2:** none.
- **P3:** none.

The only remaining gate is a replacement committed-SHA cross-platform run,
not an outstanding implementation finding.

## Findings discovered and closed during this review

### Read-only Git objects and CRLF checkout still affected Windows tests

After fail-fast behavior was restored, run `30753147192` reported exactly two
Windows harness errors. First, the missing-object adversarial test attempted
to unlink a generated loose object whose Windows file attributes were
read-only. The test failed during setup instead of reaching the intended
missing-object identity assertion. The shared corrupt/missing-object setup now
restores owner write permission before either mutation. This is the same
preparation the corrupt-object branch already required and does not relax the
fixture evaluator.

Second, Windows checkout translated retained-reader JSON inputs from LF to
CRLF. Evidence provenance deliberately hashes exact input bytes, so the
checked macOS evidence correctly rejected those translated sources as stale.
The new root `.gitattributes` policy declares
`scripts/retained_readers/** text eol=lf`, covering the complete retained-reader
source and evidence-input tree. All tracked files under that path are text or
JSON; no tracked binary is forced through text normalization. Independent
`git check-attr` probes report `text: set` and `eol: lf` for every current file
in the retained-reader package.

The first form of the regression only substring-matched the declaration, and
the retained-reader workflow path filters did not include `.gitattributes`.
That was a P3 CI-contract gap: a comment or later overriding attribute could
leave the test green, while an attributes-only change could bypass the
workflow entirely. The closed regression now invokes `git check-attr text eol`
for every current retained-reader file and asserts the complete effective
result. Both the pull-request and push filters explicitly include
`.gitattributes`, and the workflow regression pins both trigger entries.

These are test and checkout-policy corrections, not production reader
changes. The full current suite passes on macOS, checked evidence freshness
passes, and the checked evidence SHA-256 remains unchanged.

### The Windows harness step initially could mask a failed test command

The harness job grouped the unit suite, manifest validation, and gate-ready
check into one multi-command step without selecting a shell. GitHub Actions
therefore used PowerShell on Windows, where a nonzero exit from an external
test command did not stop the script before later successful validation
commands. The step could consequently report success after a failed unit
suite. This was a release-blocking R0 evidence-integrity defect.

The workflow now explicitly selects `bash` for that step, giving all harness
platforms the GitHub Actions fail-fast Bash invocation used by the behavioral
steps. A regression isolates the named workflow step, requires `shell: bash`,
and requires it to precede the multi-command `run` block.

The two unit failures exposed on Windows were test portability defects rather
than retained-reader defects. The live-boundary regression now writes exact
UTF-8 bytes instead of allowing host newline translation through text mode.
The non-UTF-8 path-key regression now mocks `os.fsencode` to supply the same
invalid byte sequence on every host instead of asking the host filesystem
codec to construct such a path. Both retain the original assertions while
removing platform-dependent setup. These workflow and test-only changes are
outside the checked evidence source set; current evidence freshness still
passes.

### Windows index canonicalization initially used text-mode stdin

The first exact-SHA run failed in the Windows behavioral lane before any
retained reader executed because the generated root repository had no
`.git/index`. Fixture generation rebuilt canonical indexes with
`git update-index --index-info` through Python text-mode standard input. On
Windows that path could translate the LF-delimited index-info payload, so Git
rejected the input after the generator had removed the original index. This
was a release-blocking R0 portability defect, not evidence of a retained-reader
behavioral incompatibility.

The correction routes index-info through the generator's existing binary
`_git_input` helper. That helper UTF-8 encodes the exact payload and invokes
Git without text mode or newline translation. The new regression pins the
canonicalizer to the binary helper; the existing helper-level regression pins
the exact input bytes and absence of text mode. Generated fixture identities
still match the reviewed fixture contract on macOS. The checked evidence binds
the generator source hash, so its freshness failure after this correction is
expected and correctly fail-closed.

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

Checked macOS arm64 evidence was regenerated after the Windows generator
correction. Its SHA-256 is
`80330cc6fb6879c54c1e9b255b5d6880d3583155a88db2d0948a1886a0e93b9e`.
A separate fresh matrix run contains all 38 expected rows and compares equal
under the reviewed portable projection.

The final matrix contains exactly 38 expected result keys:

- 37 executed rows passed;
- one unavailable `gwz-py` v0.10.0 tuple is declared unsupported; and
- no row failed or was silently omitted.

Compared with the pre-remedy checked evidence, the result-key set is identical.
Only the six continuation rows changed `after_invariant_sha256`: two cases
across the three durable-v0 readers. Exit codes, typed outcomes,
postcondition counts, artifact identities, and every unrelated invariant are
unchanged. Fixture and archive semantic digests changed earlier because the
identity/evaluator contract became more complete, not because v0 fixture
record bytes changed. The later Windows generator correction only transports
the already-defined canonical index payload to Git without text translation;
generated logical fixture identities remain unchanged.

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

Final compatibility closure after the Windows generator, checkout, and
CI/test portability corrections:

```text
python3 -m unittest discover -s scripts/retained_readers -p 'test_*.py'
Ran 79 tests in 72.431s — OK

retained_reader_harness.py validate
status=valid, tuple_count=36

retained_reader_harness.py gate-ready
status=manifest-ready, tuple_count=36

macOS arm64 retained-reader matrix
38 rows: 37 passed, 1 declared unsupported, 0 failed

retained_reader_evidence.py compare
status=equal

checked evidence SHA-256
80330cc6fb6879c54c1e9b255b5d6880d3583155a88db2d0948a1886a0e93b9e

python3 -m compileall -q scripts/retained_readers
passed

git diff --check
passed
```

Focused verification of the Windows correction:

```text
index canonicalization routes through binary _git_input
passed

_git_input preserves exact bytes and does not enable text mode
passed

generated fixtures match the reviewed logical contract
passed

checked macOS evidence freshness before regeneration
failed closed with evidence source provenance is stale

checked macOS evidence freshness after regeneration
passed as part of the full 79-test suite
```

Focused verification of the CI/test portability correction:

```text
workflow harness step requires fail-fast bash
passed

live-boundary regression uses exact bytes
passed

non-UTF-8 path identity uses a portable fsencode observation
passed

focused suite
3 tests in 0.138s — OK
```

Focused verification of the post-run `30753147192` corrections:

```text
missing/corrupt Git-object adversarial identity
passed

retained-reader LF checkout policy
passed

checked evidence source freshness
passed

generated fixtures match the reviewed logical contract
passed

focused suite
4 tests in 18.632s — OK

all current retained-reader git check-attr observations
text=set, eol=lf

retained-reader pull-request and push path filters
.gitattributes included in both
```

Additional independent probes covered repacking, packed refs, commit-graph and
MIDX layout, object corruption, missing objects, normal ref and ORIG_HEAD
changes, valid unmerged stages, all reviewed index flags, active hooks,
legacy-branch state, file/directory ref locks, unknown object metadata, and
authoritative-file symlinks.

## Scope and budget

The retained-reader package is **6,968 physical lines across 23 files**, below
the reviewed **7,100-line/23-file** ceiling. The largest Python module is 497
lines; all implementation and test modules remain below 500 lines.

The implementation and harness diff is confined to the root checkout
attributes, retained-reader workflow, and existing
`gwz-core/scripts/retained_readers/**` test/tool/documentation files. The
workspace-level changes outside those paths are the R0 inventory, budget, and
review documents. There is no merge production-code, public protocol,
wire-value, record-schema, record-byte, or released-feature delta.

## Decision

The Windows generator, checkout-policy, and test corrections and the complete
retained-reader portability remedy are ready to commit, with no remaining P0,
P1, P2, or P3 finding. Regenerated checked macOS arm64 evidence, the 79-test
full suite, and portable evidence equality all pass. R0 may be accepted and R1
dispatched only after the replacement committed exact SHA passes both
harness-unit jobs and all five required behavioral platform lanes with
complete evidence and attestations.
