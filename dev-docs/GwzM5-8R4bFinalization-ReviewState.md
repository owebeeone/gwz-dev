# R4b-F finalization state-machine and recovery review

Date: 2026-08-10

Baseline: `gwz-core` `cec3d5bf800b44e6db85a51edb3ede057b330851`

Reviewed scope: the current exact dirty R4b-F implementation and its focused
tests, the frozen R4b transition design, the I2 record/action contracts, and the
revised R4b-F change-budget row. This was a read-only review of production and
test code. The reviewer changed only this memo.

R4b-X participant execution, R4b-P preservation/rollback/status, R4b-G
aggregate wiring, production activation, CLI/protocol dispatch, and later
version behavior are out of scope.

## Verdict

**GO.** R4b-F may close and its accepted interface may be consumed in the
ledger's dependency order. No P0, P1, P2, or P3 finding remains.

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

The two state/restart blockers found during review are closed: accepted
born-detached no-publication work no longer enters an attached-only publication
base, and every restart after acceptance re-verifies the exact metadata
worktree bytes and raw index entries before publication authority is issued.
The later symlink-parent and unborn-evidence compatibility regressions are also
closed on the reviewed bytes.

## State-machine and authority result

The concrete finalizer preserves the accepted R4b-TI/TR/S/A boundary:

- only a checked `StoredV1Record` and record-bound observation request enter the
  observer;
- participant completion and acceptance are observed before the acceptance
  transition is durably committed;
- publication classification consumes the persisted accepted workspace;
- candidate preparation is durably recorded before evidence creation;
- `CommittingEvidence` is durable before the scoped evidence commit;
- the exact evidence commit, tree, selected-root result, and candidate hashes
  are recorded before candidate publication begins;
- `PublishingCandidate` is durable before marker, lock, boundary, or index
  mutation;
- candidate publication is observed as complete before the verifying phase;
- publication is reverified before `Complete`, and `Complete` is reverified
  before the operation becomes terminal; and
- every physical action is authorized by an opaque exact observation and then
  rechecked under the retained mutation lease immediately before mutation.

The executor cannot bypass the checked observer. A wrong lease, wrong physical
action, stale record, changed exact prefix, changed evidence commit, or changed
accepted input fails before the requested mutation. Same-owner retry logic is
action-specific: an exact successor may advance, while the same action without
progress and a different action attempt both reject.

## Accepted-root and no-publication result

The accepted-root verifier handles all three frozen root forms directly:

- born attached requires the exact commit and symbolic branch;
- born detached requires the exact detached commit; and
- unborn attached requires the exact symbolic branch with no commit.

All forms require a clean native Git state and exact manifest/lock worktree and
raw stage-0 index entries. The raw-index check rejects duplicate/staged-conflict
entries, non-regular modes, wrong blobs, assume-valid, skip-worktree, and other
extended flags. Worktree verification independently requires regular,
non-executable, byte-exact files, so index and worktree state cannot be
conflated.

Born-attached, born-detached, and unborn-attached unchanged roots complete
without a candidate or physical action. In particular, detached no-publication
completion does not request an attached publication branch and does not inspect
or rewrite an otherwise unused marker parent.

For a selected root, acceptance reads manifest and lock bytes from the exact
durable root result. The evidence commit's sole parent is that selected result,
`root_merge_commit` records it, and the marker identifies the same result. For
an unselected born root, the evidence parent is the accepted baseline commit.
For an unselected unborn root, evidence is the released parentless first commit.

## Evidence and publication exactness

The scoped evidence primitive builds an isolated in-memory index from the
accepted parent tree, replaces only the candidate lock and marker, verifies the
resulting tree, locks `HEAD` and the attached branch ref, rechecks the expected
ref under the lock, creates the commit object, atomically publishes the ref,
and self-verifies the result. It leaves the real index and unrelated staged,
unstaged, and untracked work untouched.

Evidence adoption verifies the exact parent count and parent, exact message,
exact candidate tree, attached branch/ref, composition tree, and ordered
candidate hashes. Operational errors propagate as errors; only typed semantic
drift becomes ambiguity/recovery.

The attached-unborn path retains the exact released v0/I2 evidence shape:
`parent = None` and exactly two candidate files/hashes, lock plus marker. The
workspace manifest remains in its accepted worktree/index state and is not
silently added to the evidence commit. The exact v0 durable-window
characterization passes with this shape.

Candidate restart classification combines:

- regular-file hashes for lock, marker, and `.git/info/exclude`;
- exact `Baseline`, `Marker`, `Lock`, or `Boundary` prefix reconstruction;
- exact raw-index `Pre` or `Staged` form; and
- worktree alignment before accepting a staged terminal form.

The resolver covers every prefix/index cell, the absent/mixed forms, both
single-equality cases, and the degenerate case where lock and boundary already
equal their baselines. Only the exact next action is issued. Any mixed,
third-state, executable, symlink, worktree-diverged, or noncanonical-index form
becomes fail-closed ambiguity and enters recovery without overwrite.

## Crash and restart result

Real-Git fault injection crashes immediately after each physical mutation:

1. evidence commit;
2. marker write;
3. lock write;
4. workspace-boundary write; and
5. candidate index staging.

Every restart adopts the already completed mutation and executes exactly the
remaining action suffix. It neither repeats the completed action nor skips a
later action. The degenerate marker case independently proves the shorter
`WriteMarker -> StageIndex -> Complete` graph, including crashes after marker
write and index staging.

Additional restart cases prove:

- a crash after durable acceptance rejects later manifest/lock worktree,
  index, or index-flag tamper before candidate creation;
- pre-acceptance noncanonical metadata index flags cannot receive acceptance;
- post-evidence manifest drift enters `RecoveryRequired` with literal
  `Finalizing` origin and preserves the drift;
- a tampered marker, mixed publication index, or staged-candidate worktree edit
  enters recovery without another physical write; and
- exact evidence and candidate state converge to one completed composition
  commit without duplicate evidence.

## Path safety

Acceptance always verifies that the metadata parent is a real directory.
Publication-required work separately verifies the marker and Git-info parent
chains before candidate preparation, evidence/ref mutation, and every later
physical-action preflight. `symlink_metadata` is used, so a symlinked owned
parent is not followed as a directory.

The Real-Git symlink regression proves that a symlinked marker parent is
rejected while the record is still at candidate preparation, before evidence,
ref movement, or an external write. Its no-publication twin proves that an
irrelevant marker-parent symlink is preserved when no publication is required.
Leaf file classification independently rejects symlinks and other non-regular
forms before publication mutation.

## Budget and ownership

The revised F ceiling is a reviewed scope correction for the exact observer,
raw-index, equality-matrix, operational-error, symlink, and third Real-Git test
responsibilities. It adds no field, phase, transition, wire shape, production
reachability, or user-visible behavior.

| Measure | Current | Ceiling | Result |
| --- | ---: | ---: | --- |
| Production-bearing lines | 1,989 | 2,100 | Pass |
| Production paths | 17 | 18 | Pass |
| Focused test/tool/doc lines | 1,681 | 1,800 | Pass |
| Test/tool/doc paths | within reviewed allocation | 16 | Pass |

Every dedicated production or test responsibility owner remains below 500
lines. The largest reviewed production owner is 477 lines and the largest
focused test owner is 439 lines.

## Verification

The reviewer independently ran on the settled bytes:

- `cargo test -p gwz-core workspace_ops::merge::v1_lifecycle -- --nocapture`:
  101 passed;
- `cargo test -p gwz-core git::tests::g13 -- --nocapture`: 7 passed;
- `cargo test -p gwz-core
  v0_changed_merge_windows_have_named_exact_durable_shapes -- --nocapture`:
  1 passed;
- `cargo test -p gwz-core --lib`: 907 passed, 0 failed, 1 ignored;
- `cargo fmt --all -- --check`: passed;
- `cargo clippy -p gwz-core --lib --tests -- -D warnings`: passed; and
- `git diff --check`: passed.

The settled package gate additionally passes all four integration-test groups,
strict all-target/all-feature Clippy, documentation generation, the docs
checker, checker unit tests, and compatibility rules.

## Closeout

No required high-risk state/restart case is missing from R4b-F. The finalizer
is accepted only at the production-disabled v1 boundary reviewed here. This GO
does not activate v1, authorize a new durable shape, or pre-accept R4b-X,
R4b-P, R4b-G, or aggregate release behavior.
