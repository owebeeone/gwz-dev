# R4b-P P1/P2 settled-tree fault/recovery re-review 4

Date: 2026-08-12

Reviewed exactly:

- workspace commit `729a1d32e99db23a259860efd018bab1daad71ed`;
- `gwz-core` commit `e5838ccae81ffc638fb7197c17bca9e91137c633`;
- `gwz-cli` commit `3cca145c0b32410f250f640730ed7ca18f1da59f`;
- baseline workspace commit `b6cfabc6292a06f5e90c7f4c9eb1b4b58598dc49`;
- baseline `gwz-core` commit
  `7d1a3e6a33cf2e4b5d882db1f7196f16ea7cc87d`;
- `GwzM5-8R4bP1P2-RemPlan-3.md`; and
- the prior settled-tree findings in `GwzM5-8R4bP1P2-ReviewArch-3.md`
  and `GwzM5-8R4bP1P2-ReviewFaults-3.md`.

I reviewed this tree independently from the concurrent architecture reviewer.
The requested workspace/core/CLI commits matched and the GWZ workspace was
clean before review. The production v1 decoder/dispatcher remains disabled;
the findings therefore block R4b-G and later activation rather than describe
an already enabled release path.

## Verdict

**NO-GO for proceeding to R4b-G.** There is no P0 finding. Two P1 durability/
authority defects, one P2 rollback-prefix defect, and one P2 acceptance-proof
gap remain. The thread-local fault hook fixes the parallel-test interference,
the immutable authority and closed residue tables materially correct the
previous destructive restart defects, and the completed-participant aggregate
is now consulted at entry, observation, execution, recovery, and exhaustion.
Those improvements do not close the blockers below.

## Findings

### [P1-1] Windows path identity can bind one durable authority to two distinct leaves

**Where:**

- `gwz-core/src/checked_artifact/identity.rs:575-595`;
- `gwz-core/src/checked_artifact/observation.rs:78-103`;
- `gwz-core/src/checked_artifact/authority.rs:101-119`; and
- `gwz-core/src/checked_artifact/authority.rs:195-222`.

**Violated contract.** RemPlan-3 section 3.2 requires canonical path identity
to reflect the artifact filesystem's equivalence for the complete
root-relative sequence. It explicitly requires rejection before authority
publication when a filesystem has per-directory equivalence modes that cannot
produce one stable root-relative identity. Exit criteria 1 and 2 require an
authority to remain bound to one canonical leaf and forbid alias-hidden
recovery state.

**Evidence.** The Windows implementation unconditionally returns
`case_sensitive = false` and lowercases every component. Windows supports
case-sensitive directories, so two different leaves such as `dir/A` and
`dir/a` can exist while this implementation gives them the same path
identity, family key, and (for the same expected/goal bytes) action key. No
directory-mode query or unsupported-path rejection occurs. Because
`matches_request` trusts the derived path identity and artifact-root identity,
an authority created for one spelling is accepted as the request authority for
the other spelling when their retained parent and source form also match.

**Failure scenario.** In a case-sensitive Windows directory, start a
missing-source replacement for `dir/A` and interrupt after its exact goal is
durably staged. Reacquire the distinct missing leaf `dir/a` with the same
goal. The second request derives the same family/action keys, accepts the first
authority and staged goal, and can publish that goal to `dir/a`. The action
has crossed leaf identity even though `dir/A` and `dir/a` are distinct
filesystem objects.

**Impact.** Restart or a second action can mutate the wrong path under a
supported Windows directory mode. This defeats the immutable-family boundary
that was introduced to close D1.

**Required correction.** Query and bind the effective case-sensitivity and
normalization/equivalence mode for every component of the retained path, using
the Windows filesystem's own comparison semantics. If one stable
root-relative identity cannot be produced, return typed
`UnsupportedOperation` before private-directory or authority publication.
Do not approximate Windows equivalence with Unicode `to_lowercase`.

**Regression evidence required.** On the exact Windows runner, enable a
case-sensitive directory, create `dir/A`/`dir/a` as distinct leaves under one
retained parent, and prove their family identities cannot collide or
cross-recover. Also cover a
mode change/replaced component between acquisition and retry and the
filesystem's non-ASCII case-equivalence rules.

### [P1-2] `observe_durable` does not make a visible file durable, and its Windows namespace barrier is a no-op

**Where:**

- `gwz-core/src/checked_artifact/transition.rs:17-37`;
- `gwz-core/src/checked_artifact/classification.rs:103-122,195-200`;
- `gwz-core/src/checked_artifact/residue.rs:477-496` (the private-entry path
  demonstrates the missing file-flush step that is required);
- `gwz-core/src/checked_artifact/platform.rs:140-150`;
- `gwz-core/src/checked_artifact/observation.rs:39-49`;
- `gwz-core/src/checked_artifact/transition.rs:220-243`; and
- `gwz-core/src/checked_artifact/tests/removal_recovery.rs:121-147`.

**Violated contract.** RemPlan-3 section 3.3 says exact visible bytes alone
cannot prove a completed file or directory durability barrier. The Windows
contract requires every namespace retirement to have a write-through causal
guarantee. Section 3.4 requires same-identity duplicate retirement to make the
desired destination durable, retire the undesired alias durably, and
reobserve. Exit criterion 4 requires the complete destination/source ordering
at every restart boundary.

**Evidence.** `observe_durable` reads the leaf, calls only `sync_dir`, and reads
the leaf again. It never flushes the exact opened file whose bytes/identity it
accepts. This contrasts with `rebarrier_exact`, which explicitly opens and
`sync_all`s the file before the parent barrier. Both proof-only
source-equals-goal and an authority-free `After` row rely on
`observe_durable`, so freshly visible but unflushed contents can authorize a
durable lifecycle successor.

On Windows, `sync_dir` calls `sync_parent`, which always returns success
without a barrier. That no-op is also used after creating private-parent
components and after deleting a managed same-identity source alias. The latter
delete is `remove_file`, not the write-through rename primitive. The only
same-identity source-alias regression is Linux-only, and none of the checked-
artifact restart arrays exercises the Windows anchor boundaries.

**Failure scenarios.**

1. Another writer places the exact goal bytes at the managed leaf but has not
   flushed the file. GWZ classifies the authority-free row as durable `After`,
   persists the lifecycle successor, and a later power loss can retain the
   successor while losing/corrupting those bytes.
2. On Windows, restart in `RecoverableDuplicateSource`, remove the managed
   alias, accept the no-op `sync_parent`, and continue to goal publication or
   source cleanup. A power loss can retain the old alias or lose ordering
   relative to the later write-through edge even though GWZ has advanced.
3. On first Windows use, private hierarchy creation is followed by the same
   no-op. Barriers performed only inside the new directory do not establish
   the durability of that directory's name in its ancestor before it owns a
   detached source.

**Impact.** The implementation can certify a durable checked-artifact outcome
without a durable file or namespace fact. In the duplicate/private-directory
cases this can lose the only retained source or reopen an already completed
managed mutation after power loss.

**Required correction.** Make managed durable observation use one retained
no-follow handle: verify bytes/identity, flush that exact file, establish the
platform namespace barrier, then reobserve the same identity and bytes. Split
Windows rename durability from general namespace durability; do not expose a
successful no-op as a barrier for directory creation or deletion. Implement
the accepted write-through/anchor protocol for every such namespace, or fail
before mutation where Windows cannot establish it.

**Regression evidence required.** Add before/after file-flush and parent-
barrier restart tests for proof-only and authority-free `After` rows. On
Windows, execute anchor creation, both round-trip renames, alias retirement,
private-parent creation, same-identity source/goal duplicate recovery, and
reobservation faults through a real consumer to terminal restart. A successful
Windows build is not this behavioral evidence.

### [P2-1] Complete rollback-prefix proof still erases tracked state under the checked-artifact private prefix

**Where:**

- `gwz-core/src/git/gitbackend/preservation_image.rs:161-193,196-228`;
- `gwz-core/src/git/tests/g15.rs:453-495`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback_prefix.rs:107-136`.

**Violated contract.** RemPlan-3 sections 3.5/R3 state that reserved-prefix
trees are never blanket exclusions and that tracked or index-visible drift
beneath them remains in the complete image and rejects. Exit criteria 6 and 7
require the cursor-relative aggregate to cover the complete terminal
participant checkout before any later owner or exhaustion authority.

**Evidence.** `raw_excluded_paths` unconditionally prepends
`.gwz/checked-artifacts` to both the worktree and index overlay lists.
`checkout_matches_commit_with_overlay` applies those lists to both the live
image and expected commit tree, clearing index entries as well as worktree
facts. The new G15 test deliberately commits a tracked file under this prefix,
changes it, and asserts that the complete checkout still matches. Every
terminal participant proof in `rollback_prefix` ultimately uses this
comparator, so the aggregate inherits the omission.

**Failure scenario.** A selected root or ordinary member has a tracked
`.gwz/checked-artifacts/protocol` in its rollback destination commit. After
that participant is durably recorded `Aborted`/`RolledBack`, change or stage
the file and resume any admitted request. The aggregate erases the expected
and live path, can authorize the next participant, and can eventually certify
terminal abort despite completed-prefix drift.

**Impact.** D3 is not complete: a later repository can be mutated and a false
whole-workspace rollback can be reported while a completed repository differs
from its exact destination.

**Required correction.** Do not add the private prefix to the index overlay.
Ordinary ignored/untracked private residue is already outside Git's
non-ignored checkout image; if a tracked collision with the reserved private
namespace is unsupported, reject it explicitly before mutation rather than
deleting it from both sides of the proof. Keep any worktree-only operational
overlay separate and backed by its exact checked-artifact observer.

**Regression evidence required.** Replace the current acceptance test with
tracked/index/worktree/type/rename/conflict drift below the private prefix,
and cross it through a stopped-after-completed-participant service fixture for
both terminal participant states and all five admitted requests. Assert
byte-identical record, zero later-owner calls, and no terminal abort.

### [P2-2] C7 cardinalities are not bound to the generated rows or the real service relation

**Classification:** acceptance-proof gap, not a demonstrated production
state-machine defect.

**Where:**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/root_rows.rs:6-102`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/reconciliation.rs:6-58,104-158`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/request_dispatch.rs:18-69`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs:16-151,222-262`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_ambiguity_matrix.rs:15-123,144-187`; and
- `gwz-core/src/checked_artifact/fault.rs:5-45` plus the hard-coded boundary
  arrays in `checked_artifact/tests/recovery_protocol.rs:163-222`.

**Violated contract.** RemPlan-3 sections 3.6/R4 and exit criterion 8 require
every explicit legal row to be seeded at its durable phase and crossed through
the real observer, resolver, executor diagnostic, successor bytes/calls, and
terminal exhaustion. The exact 20-cell relation is required per ordinary
physical and causal-parent row, with real ambiguity/operational-error forms
and enum/cardinality gates that fail when a legal decision is missing.

**Evidence.** The advertised 1,350 root rows are collected as tuples and only
counted; no tuple is passed to a fixture or production service. The advertised
635 physical and 40 causal rows discard row identity into `_identity`/`_request`
and repeatedly call the same pure `reconcile(class, attempt, fact)` function.
For non-root and checked-artifact rows, distinct row identities are explicitly
mapped to `RollbackRow::RecordNoMutationAbort` before counting. These tests can
remain green if a row cannot be seeded, its production observer emits the
wrong fact, its action dispatches incorrectly, or its successor bytes/calls
are wrong.

The real root fault matrix is useful but materially smaller: it runs 54/58
canonical physical handoff/target cases starting with `Preserve`, then resumes
with only `Abort`/`Preserve`. The root ambiguity matrix uses one canonical
handoff fixture for all 22 phases and, except for backup/bundle, creates one
unrelated untracked file rather than the phase's physical third form. The
five-request test injects one synthetic preservation and one synthetic
rollback observation instead of binding all generated rows to production
observation. Finally, the checked-artifact fault enum is not iterable and its
restart arrays omit all Windows-only anchor faults, so the cardinality gate
cannot detect a newly omitted boundary.

**Failure scenario.** Regress only `SelectedRoot/BoundaryStaged/Archive` at
`Reset(RestoreIndex)`, make a required-empty `RestoreParent` observer emit an
ordinary `After`, or omit one Windows anchor boundary. The 1,350/635/40 counts
and generic 20-cell relation still pass because the row identity never reaches
the real observation/service/execution path. The existing real matrices do
not instantiate that complete row/form/request combination.

**Impact.** D4 remains unproved. This is precisely a false-green generated
matrix: the production reconciliation function is well unit-tested, but the
claim that every legal durable row composes through it is not established.

**Required correction.** Give each declarative row a constructor that seeds
its exact durable record and physical state, then drive all five requests
through `service::run` with the row's real `Before`/`After`/representable
`Ambiguous`/operational-error observer forms and matching success/failure/
stale/consumed attempts. Assert exact pre/post record bytes, exact per-target
and total calls, artifact identities/bytes, and terminal exhaustion. Make the
fault-boundary vocabulary exhaustively enumerable (including Windows anchor
and checked-artifact subtransitions) so adding or omitting a boundary changes
an independently pinned count.

**Regression evidence required.** The generated service matrix itself must
fail if any declared row is skipped. Include an intentionally omitted-row
meta-test or coverage-key set comparison proving generated keys equal the
independently declared legal keys; do not derive both sets from the same row
iterator.

### [P3-1] The family-byte bound is applied only after an unbounded file read

**Where:**

- `gwz-core/src/checked_artifact/residue.rs:97-143`; and
- `gwz-core/src/checked_artifact/observation.rs:187-237`.

**Violated contract.** RemPlan-3 section 3.2 requires bounded family entry
count/bytes and says malformed or oversized family entries remain visible and
classify `Ambiguous`.

**Evidence.** `inspect_family` calls `observe_leaf_exact`, which uses
`read_to_end`, before adding the resulting allocation to `MAX_FAMILY_BYTES`.
A single matching family file of arbitrary size is therefore fully allocated
and read before the one-MiB family bound is checked. The authority decoder's
16-KiB limit is likewise reached only after this unbounded read.

**Failure scenario.** A malformed/corrupt matching authority or goal entry is
very large. Reacquisition can exhaust memory or spend unbounded I/O before it
returns the required typed ambiguous state. A legitimate goal larger than the
family cap is also written first and only then strands its own action as
foreign.

**Impact.** Corrupt recovery state can turn a fail-closed recovery check into
process denial of service, and over-cap requests can publish unrecoverable
residue instead of rejecting before authority.

**Required correction.** Inspect no-follow metadata length before allocation,
enforce per-entry and cumulative remaining budgets while streaming, and
reject over-cap requested goal bytes before publishing authority or scratch.
Revalidate length/identity after the bounded read.

**Regression evidence required.** Sparse/large authority, goal, source, and
foreign family entries must return typed ambiguity without allocating or
reading beyond the configured budget; an over-cap legitimate request must
leave no authority or family residue.

## Closure audit

| Item | Review conclusion |
| --- | --- |
| D1 checked-artifact identity/durability | **Open.** Parent/source authority, restart-closed staging, cleanup identity, and thread-local fault isolation are materially corrected. Windows path equivalence and durable visible-file/namespace proof remain P1 blockers. |
| D2 quarantine placement | **Structurally closed.** Callers now select workspace or Git-directory policy explicitly and prove the opened rename domain. The Windows namespace-durability issue in P1-2 remains separate. |
| D3 rollback aggregate | **Open (P2-1).** The cursor-relative aggregate and selected-root supersession are present at all intended call sites, but the complete checkout primitive still deletes tracked private-prefix state from its authority image. |
| D4 generated C7 | **Open as an acceptance-proof gap (P2-2).** The vocabulary and pure reconciliation relation are explicit, and real rollback ambiguity is much stronger, but row cardinalities are not service-bound and Windows checked-artifact boundaries are not coverage-closed. |

## Verification performed

On the exact requested tree:

- installed `gwz status --json`: clean workspace and exact workspace/core/CLI
  commits matched;
- `cargo test -q -p gwz-core c7_matrix --lib -- --test-threads=1`: 7 passed;
- `cargo test -q -p gwz-core
  complete_checkout_excludes_only_the_checked_artifact_private_tree --lib`:
  1 passed, confirming that tracked private-prefix drift is intentionally
  erased by the current comparator; and
- a local Windows cross-check was attempted but could not compile native zlib
  from macOS because the MSVC/vcpkg build environment was unavailable. It is
  not Windows behavioral evidence and is not treated as a product failure.

The green C7 tests are consistent with P2-2 because they test tuple counts and
the pure reconciliation function. No Windows anchor fault appears in the
checked-artifact test sources. Exact-tree Windows behavioral/fault execution
therefore remains an unmet R5 gate independent of the code findings above.

## Exit decision

Do not resume R4b-G. Correct P1-1, P1-2, and P2-1, replace the false-green C7
cardinality with a production-bound generated matrix, then rerun the complete
local/static/document and exact-tree Windows behavioral gates on one settled
tree. Repeat both independent acceptance reviews. Under RemPlan-3's rule, any
open P0/P1/P2 makes this checkpoint **NO-GO**.
