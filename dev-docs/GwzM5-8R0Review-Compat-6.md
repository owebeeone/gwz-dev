# GWZ M5–M8 R0 Retained-reader Compatibility Re-review

Date: 2026-08-03

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
`fb777940f0d14fda3a2125b763bebd7e79d6b8bea9463c001c73bee2a5cd65a3`.

The portable comparison excludes only the reviewed Git-version and Python
runtime-version/executable identity fields. Fixture identity, result rows,
typed outcomes, mutations, postconditions, artifact identities, and all
durable semantic observations remain compared.

## Verification performed

- Complete retained-reader unit/adversarial suite: **76 passed, 0 failed**.
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
- Retained-reader change budget: **7,071 lines / 23 files**, within the
  approved **7,100 / 23** allowance; every Python implementation/test module
  remains below 500 lines (maximum 497).

## Approval decision

- **Compat-5 remediation complete:** approved.
- **Commit and push the coherent retained-reader change set:** approved.
- **Run the required exact-commit five-platform CI matrix:** required next.
- **Accept R0 or dispatch R1 before that CI evidence is green:** not approved.
