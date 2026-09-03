# GWZ M5–M8 R0 Retained-reader Compatibility Re-review

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

Scope:

- the final uncommitted retained-reader portability remedy in
  `gwz-core/scripts/retained_readers`;
- every finding and required invariant in `GwzM5-8R0Review-Compat-5.md`;
- behavioral equivalence of the logical fixture identity across Git storage
  layouts and platforms;
- fail-closed Git administration, object, index, pseudoref, and publication
  boundary handling; and
- regenerated checked macOS arm64 evidence and a fresh actual matrix run.

## Verdict

**Pass.** I found no remaining P0, P1, P2, or P3 defect in the corrected
implementation, tests, cases, fixture contract, or checked evidence. The two
P2 findings and one P3 finding from Compat-5 are closed without weakening the
retained-reader behavioral contract.

The coherent R0 retained-reader change set is ready to commit and push. R0
acceptance and R1 dispatch still require the normal exact-commit CI gate on
Linux x86_64, Linux arm64, Windows x86_64, macOS x86_64, and macOS arm64, with
each lane's evidence and attestation bound to the same commit. That external
matrix is an outstanding acceptance condition, not a source-code finding.

## Findings by severity

- **P0:** none.
- **P1:** none.
- **P2:** none.
- **P3:** none.

## Compat-5 closure

### Optional workspace-boundary mutation

The two continuation cases retain the narrowly observed
`.git/info/exclude` mutation cardinality of zero or one. Acceptance no longer
depends on that path/count classification alone. The complete merge-record
semantic postcondition now proves that:

- `baseline_boundary_sha256` matches `baseline_boundary_text`;
- `boundary_sha256` matches `boundary_text`; and
- the live root `.git/info/exclude` is a regular file whose exact bytes equal
  the candidate `boundary_text`.

The negative regression changes the live boundary to another syntactically
valid rule set and proves that the semantic postcondition fails. A second
regression replaces the boundary with a symlink to an external file containing
the exact expected bytes and also proves rejection. Therefore the optional
physical rewrite cannot admit arbitrary content or filesystem indirection.
Compat-5 P2-1 is closed.

### Storage-independent repository identity

Each repository now contributes one semantic record rather than selected raw
`.git` paths. It retains:

- SHA-1 object format and repository format zero;
- symbolic, detached, or unborn HEAD state and the resolved OID where present;
- the complete sorted ref map from Git plumbing;
- `ORIG_HEAD` when present;
- the binary staged index tuples: raw path bytes, mode, conflict stage, and
  object OID;
- the complete object OID/type/size set, including unreachable objects;
- exact canonical local config and workspace-boundary content; and
- every durable non-`.git` entry, with only the reviewed platform metadata
  normalization.

`git fsck --strict --no-reflogs --unreachable` proves object integrity and
completeness before identity is accepted. `cat-file --batch-all-objects`
provides the storage-independent complete object inventory. The identity is
stable under object repacking, ref packing, server-info generation,
monolithic and split commit graphs, multi-pack indexes, bitmaps, reverse
indexes, and cruft-pack metadata while still detecting object addition,
removal, and corruption.

Compat-5 P2-2 is closed.

### Closed Git-administration classification

The physical Git administration tree is inventoried as regular files and
directories without following symlinks. Only explicit storage representations
and reviewed non-authoritative bookkeeping are accepted. Physical ref and
reflog files/directories are correlated with the semantic ref set; arbitrary
lock-shaped directories cannot hide behind a prefix allowance.

Tests prove fail-closed behavior for corrupt or missing objects,
`MERGE_HEAD`, active hooks, legacy branch authority, file- and directory-form
ref/reflog/index locks, unknown object-info files, symlinked config or exclude
files, and unclassified hook directories. Replacement refs, alternates,
unsupported formats, and other unknown administration state are rejected by
the same closed classification rather than silently discarded.

The same regular-file rule is applied to post-run authoritative boundary,
marker, lock, candidate-hash, YAML, archived-record, and generic file
postconditions. Mutation normalization does not dereference YAML, ref, log,
object, or index symlinks. Regressions reject same-content symlinks for the
workspace boundary, archived merge record, lock YAML, and generic `path:file`
contract, closing the late durable-output indirection false negative found
during this review.

Reviewed incidental state remains portable: reflog bytes,
`COMMIT_EDITMSG`, repository description, `gc.log`, sample hooks, maintenance
storage, and textual ref line endings do not alter the logical identity.

### Binary paths and index flags

The NUL-delimited staged index stream remains binary throughout parsing, and
raw path bytes are retained as base64. The portable parser regression includes
a non-UTF-8 path, and conflict-stage tuples remain represented without
mistaking valid uppercase `M` rows for assume-unchanged state.

Skip-worktree, assume-unchanged, fsmonitor-valid, resolve-undo, and
intent-to-add state are rejected. Intent-to-add is checked by comparing the
binary cached diff under Git's visible and invisible ITA modes; this catches
both present and deleted intent-to-add paths. The attached-versus-detached
same-OID regression separately proves that HEAD mode cannot collapse.

Compat-5 P3-1 and the broader index-flag invariant are closed.

## Exact-SHA CI attempts and Windows portability corrections

### Run 30752346550: generator and masked harness failures

Exact-SHA run `30752346550` executed commit
`77fd8d8e93fa20e3c9d8f47f68f09681d08d33ed`. The Ubuntu harness unit job
passed. The Actions UI marked the Windows harness unit job successful, but its
log actually ended `FAILED (failures=1, errors=18)`. Linux x86_64, Linux arm64,
macOS x86_64, and macOS arm64 all executed the applicable retained readers
successfully; the macOS arm64 lane also passed the checked portable-evidence
comparison.

The Windows false success was a P2 CI-gate defect. The workflow ran unittest,
manifest validation, and gate-ready validation in one default PowerShell
block. PowerShell continued after unittest returned nonzero; the two later
commands succeeded, so the step inherited the final zero exit status and hid
the test failure. The harness step now explicitly uses Bash, whose Actions
invocation is fail-fast (`-e -o pipefail`), and a workflow regression pins that
shell on the exact multi-command step.

The Windows behavioral lane failed before any retained reader executed. Its
fixture-generation step returned success, but the matrix's pre-execution
identity check rejected repository `.` because `.git/index` was absent. The
failure is a generator portability boundary, not released-reader behavior or
record compatibility.

`_canonicalize_git_dir` previously sent the line-oriented
`git update-index --index-info` payload through Python text-mode stdin. On the
Windows subprocess path that permits newline translation before Git receives
the payload. The correction routes the exact same payload through the existing
binary `_git_input` helper, which explicitly UTF-8 encodes once and passes
bytes to `subprocess.run` without text mode.

The regression pins the real canonicalization call path to `_git_input`; the
existing helper regression independently asserts byte input and absence of
text mode.

The unmasked Windows log also exposed two platform-dependent tests. The
boundary test had written its supposedly byte-exact initial and negative
values through text mode, so Windows CRLF translation made the initial
boundary check false and prevented the later corruption from changing the
semantic digest. It now writes exact bytes throughout. The non-UTF-8 snapshot
test had asked Windows to decode a POSIX byte filename that its Unicode path
model cannot represent; it now injects the raw `os.fsencode` result and tests
the same base64 fallback without depending on host path decoding. These
changes preserve the assertions and remove no coverage.

Fresh generation still matches all reviewed fixture identities. An independent
pre-fix/post-fix comparison on macOS found both the complete physical snapshot
and logical fixture-set identity unchanged. Therefore no case or
fixture-contract rebaseline is warranted; checked evidence was regenerated
only because its complete evaluator-source provenance correctly became stale.

The generator and CI/test corrections are approved; the P2 gate finding is
closed in source, with no remaining P0–P3 finding. Because they change the
generator, workflow, and tests after run `30752346550`, both harness jobs and
all five behavioral lanes must rerun at the new exact commit. The passing
old-commit jobs cannot be combined with later Windows results for R0
acceptance.

### Run 30753147192: fail-fast gate and two remaining test boundaries

Replacement run `30753147192` executed commit
`2677b398bd883040f70e2a0b18da2feda9d7d27c`. The Ubuntu harness unit job
passed. The corrected Bash step caused the Windows harness job to fail
accurately at unittest with exactly two errors; manifest validation and
gate-ready validation did not run afterward. Because the matrix depends on
both harness jobs, all behavioral lanes were correctly skipped. This proves
the P2 false-success remedy is effective.

The first remaining error was confined to the adversarial missing-object
test. Generated loose objects are deliberately read-only. The corrupt-object
branch already restored owner write permission before replacing the object,
but the missing-object branch attempted to unlink the read-only file directly,
which Windows rejects. Restoring write permission before the shared branch
preserves the intended assertion: identity calculation must fail because a
required object is actually absent, not because test setup could not remove
it.

The second error was checked-evidence input freshness. Git's Windows checkout
had converted retained-reader JSON inputs to CRLF, so byte-exact manifest/case
digests differed from the LF checked evidence even though the committed data
was unchanged. The root `.gitattributes` now declares
`scripts/retained_readers/** text eol=lf`. `git check-attr` confirms that the
manifest, cases, checked evidence, generator sources, schemas, and other files
under that evidence-bound tree are all checked out as LF text. A regression
now asks `git check-attr text eol` for every current retained-reader file and
requires the effective result to be `text: set` and `eol: lf`; it therefore
catches comments, later overrides, and newly added files rather than merely
matching one policy line.

A late independent harness re-review found one P3 completeness gap in the
first form of this correction: `.gitattributes` was not included in the
workflow's push/pull-request path filters, and the first regression only
searched for the literal policy line. The workflow now includes
`.gitattributes` in both filters, and the same workflow regression pins both
entries. This closes the gap: a change to the checkout policy triggers the
gate, and the gate validates the effective attributes rather than the source
text of the policy.

Both corrections are test/infrastructure portability changes. They do not
alter fixture bytes, logical identities, cases, released-reader inputs, record
schemas, or merge behavior. The checked evidence digest therefore remains
unchanged. The second-run corrections are approved with no remaining P0–P3
finding, but a new exact-commit run of both harness jobs and all five
behavioral lanes is still required.

## Evidence and exact-result-set review

The regenerated checked evidence is bound to:

- all 14 evaluator/generator source digests and their aggregate digest;
- six reviewed fixture identities and fixture-set digest
  `bbbf6892988a969627e76b3244f2e872410a53927c792668d51ece0f8806d28b`;
- the exact manifest, cases, artifact, runtime, platform, and evaluator
  identities; and
- the complete expected result-key set, including the explicitly
  undistributed Python tuple.

A fresh independent macOS arm64 run against the checksum-pinned artifact cache
produced exactly 38 rows: 37 passed, one was declared unsupported, and zero
failed. Its portable semantic projection compared equal to the checked
evidence. The checked evidence SHA-256 is
`80330cc6fb6879c54c1e9b255b5d6880d3583155a88db2d0948a1886a0e93b9e`.

The portable comparison excludes only the reviewed Git-version and Python
runtime-version/executable identity fields. Fixture identity, result rows,
typed outcomes, mutations, postconditions, artifact identities, and all
durable semantic observations remain compared.

## Verification performed

- Complete retained-reader unit/adversarial suite: **79 passed, 0 failed**.
- Focused final compatibility regression set: **10 passed, 0 failed**, then
  **4 passed, 0 failed** after the final intent-to-add refinement, and **6
  passed, 0 failed** after the generalized no-symlink refinement.
- Python byte compilation: passed.
- Manifest validation: **36 tuples**, valid.
- Gate-ready validation: **36 tuples**, ready.
- Fresh macOS arm64 behavioral matrix: **37 passed, 1 declared unsupported,
  0 failed**.
- Fresh-to-checked portable evidence comparison: `equal`.
- Diff whitespace check: passed.
- Post-CI Windows call-path regression and pre-fix/post-fix fixture comparison:
  passed; physical and logical fixture identities are unchanged.
- Windows fail-fast workflow, exact-boundary-byte, and portable non-UTF path
  regression set: **7 passed, 0 failed**.
- Read-only missing-object and LF checkout-policy focused regression set:
  **4 passed, 0 failed**.
- Final effective-attribute/workflow regression class: **2 passed, 0 failed**;
  the missing-object adversarial class also passed independently.
- Retained-reader change budget: **7,095 lines / 23 files**, within the
  approved **7,100 / 23** allowance; every Python implementation/test module
  remains below 500 lines (maximum 497).

## Approval decision

- **Compat-5 remediation complete:** approved.
- **Post-CI Windows P2 gate remediation complete:** approved in source;
  replacement exact-SHA evidence required.
- **Second-run Windows test/checkout portability remediation:** approved in
  source; replacement exact-SHA evidence required.
- **Late P3 attribute-gate completeness remediation:** approved and closed in
  source; replacement exact-SHA evidence required.
- **Commit and push the coherent retained-reader change set:** approved.
- **Run the required exact-commit five-platform CI matrix:** required next.
- **Accept R0 or dispatch R1 before that CI evidence is green:** not approved.
