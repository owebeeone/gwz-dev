# R0 retained-reader post-CI interface review

Date: 2026-08-02

## Verdict

The two failures in exact-SHA CI run 30749238987 are narrowly diagnosed, and
the proposed remedy is directionally correct. It should **not** be implemented
or accepted exactly as written, however. Two P2 false-positive risks must be
closed in the interface first:

1. allowing an optional `.git/info/exclude` mutation and merely recording its
   resulting digest does not prove that the resulting boundary content is
   correct; and
2. a fixture identity that silently drops every unclassified `.git` entry can
   miss behavior-affecting pseudorefs, in-progress Git state, index flags,
   object corruption, alternates, sparse state, attributes, hooks, or packed
   object state.

There are no P0 or P1 findings. The remedy is ready to implement after the
bounded interface amendments below. The resulting code is not ready to commit
until the frozen retained-reader line budget is also reconciled.

## Finding classification

- **P0:** none.
- **P1:** none.
- **P2:** three findings below.
- **P3:** two specification/coverage refinements below.

## CI evidence

Run 30749238987 executed commit
`7d1d86512bdda3b6473d9d7d35cb2081097a8bbf`.

- Both Ubuntu and Windows harness-unit jobs passed.
- macOS x86_64 and macOS arm64 passed their reader matrices; macOS arm64 also
  passed the checked portable-evidence comparison and uploaded evidence plus
  attestation.
- Linux x86_64 and Linux arm64 each produced exactly 38 rows: 31 passed, the
  absent `gwz-py` v0.10.0 tuple was declared unsupported, and exactly six
  failed. The six are the two continuation cases across the three durable-v0
  readers. Every failure has exactly one error:
  `dynamic mutation did not match exactly one contract:
  text:.git/info/exclude`. Their execution, typed output, exit status, and
  postconditions otherwise passed.
- Windows fixture generation succeeded. The matrix then rejected all 37
  executable rows before reader execution because each generated fixture's
  logical identity differed from the checked identity; the undistributed
  `gwz-py` v0.10.0 tuple remained the one declared-unsupported row.

This validates the proposed scope: there is no evidence here of a retained
reader behavior or wire incompatibility. Linux found one omitted mutation
class. Windows found that the fixture golden still includes platform-variable
Git administration bytes.

## P2 findings

### [P2-1] An observed exclude digest is not a correctness assertion

Adding `text:.git/info/exclude` as a non-overlapping dynamic mutation with a
cardinality of `0..1` is the correct way to represent the platform-dependent
snapshot delta. The snapshot includes file mode, so an atomic rewrite can be a
real changed path even when the authoritative bytes are unchanged. The
existing normalized-mutation code will retain both the observed count and the
content digest when the path changes.

That alone is insufficient. Linux evidence is uploaded, but it is not compared
against a checked Linux golden. An old reader could write arbitrary exclude
content, satisfy the path/cardinality contract, pass all current
postconditions, and produce a successful row whose bad digest is only
observed. This recreates the durable-content false-positive class closed in
the Harness-2 remediation. `.git/info/exclude` is not incidental bookkeeping:
it is the published root/member boundary, is persisted in publication intent,
and is explicitly verified by current merge finalization.

Keep the optional `0..1` mutation, but also assert the final bytes. The most
bounded correction is to extend the existing complete merge-record semantic
check for the two continuation archives:

- `baseline_boundary_sha256` must equal the SHA-256 of
  `baseline_boundary_text`;
- `boundary_sha256` must equal the SHA-256 of `boundary_text`; and
- the actual root `.git/info/exclude` bytes must equal `boundary_text`.

Those booleans then participate in the already expected merge-record semantic
digest, just as the existing marker and lock consistency checks do. This avoids
a new postcondition kind and binds the live boundary to the durable publication
authority. Exact bytes, not newline normalization, are appropriate here because
the writer's own candidate hash is byte-exact.

Required negative test: a continuation-shaped workspace with the expected
changed path but corrupted exclude content must fail its semantic
postcondition. Required positive tests: both no path delta (`count = 0`) and a
mode/rewrite delta with the exact candidate bytes (`count = 1`) pass, and the
count/content observation changes the normalized invariant.

### [P2-2] Fixture Git-state exclusion must be explicit, not catch-all

The proposed logical categories are sound: retain all non-`.git` workspace
content and the behavior-affecting Git state, while excluding known
non-authoritative bookkeeping and directory-layout noise. The unsafe part is
implementing that as “retain the named categories; ignore everything else
under `.git`.”

That would silently ignore state that can change reader behavior, including:

- pseudorefs and operation state such as `ORIG_HEAD`, `MERGE_HEAD`,
  `CHERRY_PICK_HEAD`, `REVERT_HEAD`, `AUTO_MERGE`, rebase/sequencer and bisect
  state;
- `info/attributes`, `info/grafts`, sparse-checkout state and object
  alternates;
- hooks or other executable repository policy;
- pack-backed object state and replacement refs; and
- unclassified future Git administration files.

`ORIG_HEAD` is not hypothetical: the generated completed-message fixtures
already contain `member/.git/ORIG_HEAD`. It is outside `.git/refs/**`, so a
simple HEAD/refs allowlist drops it.

Use an explicit-exclusion model:

1. Ignore directory entries inside `.git`; their empty-directory layout is not
   semantic.
2. Ignore only a reviewed list of non-authoritative files: the current bounded
   set is `COMMIT_EDITMSG`, `description`, reflogs, and maintenance diagnostics
   such as `gc.log`.
3. Canonicalize and retain HEAD, the complete ref map, every present pseudoref,
   the semantic index, local config, `info/exclude`, object identity, and any
   recognized in-progress Git state.
4. For any other `.git` file, either retain its normalized/raw identity or fail
   with an unclassified-admin-state error. Never silently continue.

A canonical `git for-each-ref` map avoids binding identity to loose-versus-
packed ref storage. HEAD must separately retain symbolic-versus-detached state,
its symbolic target, and resolved OID. Pseudorefs must be captured separately
because `for-each-ref` does not enumerate them.

### [P2-3] Object filenames do not prove object integrity

A loose-object path contains its claimed OID and permits compression bytes to
vary portably, so OID inventory is a useful identity component. A filename
alone does not prove that the stored bytes decode to that object. Corrupting a
loose object in place leaves the proposed identity unchanged. Ignoring
`objects/pack` and `objects/info/alternates` can likewise make the identity
blind to the actual available object database.

For the bounded generated fixtures, either:

- require loose-only storage, reject packs/alternates, inventory every loose
  OID, and run `git fsck --strict --no-reflogs --unreachable` for every root and
  member repository; or
- use Git's all-object enumeration independent of loose/packed storage and
  additionally validate object integrity.

The first option is smaller and matches the current generator. All current
fixtures pass the strict fsck command, including their deliberately unreachable
prepared objects. Add a negative test that corrupts bytes at an existing loose
object path and requires fixture identity calculation to fail; merely writing
a new object tests inventory sensitivity but not integrity.

## P3 refinements

### [P3-1] Define “semantic index” and “canonical config” precisely

The current `git ls-files --stage -z` representation retains path, mode, OID,
and conflict stage while ignoring platform-variable index encoding and stat
cache. That is a good base. It does not retain skip-worktree,
assume-unchanged/fsmonitor flags, or resolve-undo state, all of which can affect
behavior. Either include normalized flag/resolve-undo observations or assert
that the canonical fixtures contain none and fail identity calculation if they
appear.

The generator already writes a canonical local config byte-for-byte. Retaining
that exact local file digest is safer and smaller than a parser that might sort
away order-sensitive duplicate values. If config is instead canonicalized via
Git, the interface must preserve duplicate values and their effective order and
must exclude global/system configuration explicitly.

### [P3-2] The requested TDD matrix must include index and durable files

The acceptance tests should prove all sides of the interface, not only a
sample. At minimum, separate mutations must show identity changes for:

- HEAD target and detached/symbolic state;
- a normal ref and the existing `ORIG_HEAD` pseudoref;
- local config and `info/exclude`;
- an index entry/stage and its allowed flag state;
- object addition/removal plus same-path object corruption;
- a durable `.gwz/merge` record or `gwz.conf` artifact; and
- ordinary non-`.git` worktree content.

Tests should show identity stability only for the explicit ignored set:
`COMMIT_EDITMSG`, `description`, reflogs, reviewed maintenance diagnostics, Git
directory entries, and CRLF-versus-LF storage of textual refs. Adding an
unclassified behavior-affecting admin file such as `MERGE_HEAD` must change the
identity or fail closed.

## Recommended bounded implementation contract

### Continuation mutation

- Add exactly one dynamic entry to each of the two continuation cases:
  `text:.git/info/exclude`, minimum 0, maximum 1.
- Keep it out of the exact set so platforms that perform no observable rewrite
  remain valid.
- Retain its count and exact resulting content digest in the normalized
  mutation identity.
- Bind the actual exclude bytes to the archived candidate boundary text/hash in
  the existing merge-record semantic postcondition.
- Add positive `count=0`/`count=1` and corrupt-content negative tests.

### Fixture identity

Construct a canonical payload with two explicit sections:

- `workspace`: every non-`.git` entry, preserving kind, symlink target, size
  and content while discarding only platform file modes; and
- `repositories`: for the root and member repositories, canonical HEAD/ref/
  pseudoref state, semantic index, exact canonical local config, exact
  `info/exclude`, validated object inventory, and classified Git operation
  state.

Ignore only reviewed volatile bookkeeping. Unknown Git admin files fail closed
or remain in the identity. Keep the implementation in the existing fixture/
semantic modules; no new production, protocol, or runtime module is warranted.

Regenerate `fixture-contract.json`, all case `fixture_sha256` values, checked
evidence input/source/fixture identities, and the six continuation invariant
rows. Verify that result keys, typed outcomes, exit codes, postcondition counts,
artifact identities, and every unrelated invariant remain unchanged.

## Change-budget gate

The accepted retained-reader checkpoint is already exactly **6,600 lines across
23 files**, matching the frozen closeout ceiling in
`GwzM5-8ChangeBudget.md`. The necessary implementation and TDD additions cannot
be appended while continuing to claim that exact budget. Do not compress or
delete meaningful regression coverage to manufacture compliance.

Before commit, record a lead-approved, narrowly justified post-CI portability
allowance in the ledger, keep the file count at 23, and remeasure the exact
physical line count. This is a plan-compliance gate, not a production-scope
expansion: production LOC, wire/record behavior, and public capability delta
remain zero.

## Decision

**Approve the diagnosis: yes. Approve the remedy direction: yes, with the P2
interface amendments above. Approve implementation verbatim as proposed: no.**

Implementation may begin once the live-boundary assertion, explicit/fail-closed
Git-admin classification, and object-integrity rule are part of the interface.
Commit/push should wait for the complete TDD matrix, regenerated identities,
full local harness suite, reconciled budget, and an independent post-
implementation review. R0 acceptance and R1 dispatch still require a passing
exact-SHA rerun on both harness jobs and all five platform lanes.
