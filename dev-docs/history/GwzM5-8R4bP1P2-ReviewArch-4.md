# R4b-P P1/P2 settled-tree architecture re-review 4

Date: 2026-08-12

Reviewed exactly:

- workspace commit `729a1d32e99db23a259860efd018bab1daad71ed`;
- `gwz-core` commit `e5838ccae81ffc638fb7197c17bca9e91137c633`;
- `gwz-cli` commit `3cca145c0b32410f250f640730ed7ca18f1da59f`;
- workspace baseline `b6cfabc` and `gwz-core` baseline `7d1a3e6`;
- the accepted `GwzM5-8R4bP1P2-RemPlan-3.md`;
- `GwzM5-8R4bP1P2-ReviewArch-3.md`; and
- `GwzM5-8R4bP1P2-ReviewFaults-3.md`.

I did not read or coordinate with the other reviewer assigned to this settled
tree. Its review file appeared as an untracked workspace file after this
review had started and was left unread and untouched. Production and test
sources remained at the exact commits above.

## Verdict

**NO-GO for accepting RemPlan-3 or starting R4b-G.**

There is no P0 finding. Eight P2 implementation/evidence blockers and one P3
structural-authority gap remain. The new checked-artifact authority, explicit
placement policy, staged-goal transition, live rollback aggregate, and
declarative vocabulary are substantial corrections, but the implementation
does not yet satisfy the frozen checked-family identity, complete-checkout,
cursor-relative aggregate, reconciliation, or executable C7 contracts.

## Findings

### [P2-1] Checked-family identity does not implement platform path equivalence or prove Linux reboot stability

**Where:**

- `gwz-core/src/checked_artifact/identity.rs:166-192`;
- `gwz-core/src/checked_artifact/identity.rs:225-235`;
- `gwz-core/src/checked_artifact/identity.rs:258-292`;
- `gwz-core/src/checked_artifact/identity.rs:319-327`;
- `gwz-core/src/checked_artifact/identity.rs:575-595`; and
- `gwz-core/src/checked_artifact/authority.rs:195-202`.

**Violated contract.** RemPlan-3 sections 3.1 and 3.2 require a reboot-stable
artifact-root identity and a canonical root-relative component sequence that
reflects the artifact filesystem's actual equivalence rules. A filesystem
with unrepresentable per-directory modes must be rejected before authority
publication. Exit criteria 1 and 2 require an alias-hidden action to remain in
the same family and classify ambiguous.

**Evidence.** `canonical_path_identity` queries one root-wide boolean and
applies it independently to every component. Linux hardcodes all components
as case-sensitive raw bytes. This misses casefolded ext4/F2FS directories.
Windows hardcodes all components as case-insensitive and uses Rust Unicode
`to_lowercase`; it neither queries per-directory case sensitivity nor uses the
filesystem's actual name-equivalence operation. Thus Linux can derive two
families for two names of one leaf, while Windows can collapse two distinct
leaves in a case-sensitive directory into one family.

Linux durable identity also accepts any successful `name_to_handle_at` and
combines that handle with raw `fstatfs().f_fsid`. There is no capability check
that the filesystem promises the identifier pair to remain stable across the
supported reboot model. In contrast, the macOS implementation explicitly
checks `VOL_CAP_FMT_PERSISTENTOBJECTIDS`. A successful handle query proves
that a handle exists now, not the persistence promise required by the frozen
contract.

**Failure scenario.** Begin a checked action beneath a Linux casefolded
directory using one spelling, stop with authority/source/goal residue, and
reacquire the same physical leaf through an equivalent spelling. The second
acquisition derives a different family prefix and does not enumerate the old
authority. It can publish a second authority instead of reporting ambiguity.
The same loss of family visibility can occur after reboot if an accepted
filesystem changes the unqualified filesystem identifier.

**Impact.** Old source/goal authority can be hidden from the operation that
must own it, defeating the one-family invariant and permitting conflicting or
stranded recovery. On Windows, false family collisions can block unrelated
leaves. This is a foundational D1 contract failure, not only missing test
coverage.

**Required correction.** Make path equivalence a component-by-component
platform capability over retained parent handles, including per-directory
mode. Use the filesystem's exact comparison/canonicalization contract or
reject the layout before publication. On Linux, admit a durable identity only
when the filesystem exposes the persistence guarantee required by the design;
otherwise return typed `UnsupportedOperation`. Add an injectable equivalence
and identity seam on every host, real casefold/case-sensitive-directory tests
where supported, and restart/reboot-simulation tests proving that both
spellings find and retain the original family.

### [P2-2] The family byte limit can make GWZ's own valid action permanently foreign after publication or detachment

**Where:**

- `gwz-core/src/checked_artifact/residue.rs:18-19`;
- `gwz-core/src/checked_artifact/residue.rs:97-143`;
- `gwz-core/src/checked_artifact/observation.rs:210-237`;
- `gwz-core/src/checked_artifact/transition.rs:81-108`; and
- `gwz-core/src/checked_artifact/transition.rs:181-309`.

**Violated contract.** RemPlan-3 requires bounded protocol parsing while each
GWZ-created transition remains restart-closed. Oversized/foreign protocol
records must remain visible and ambiguous, but the bound must not reclassify
the operation's own exact artifact payload after GWZ has published authority
or moved the source.

**Evidence.** `inspect_family` limits the sum of every family file to 1 MiB,
including the goal and quarantined source payloads. Those payloads are not
bounded to 1 MiB before authority publication. Every file is first read to end
into memory; only afterwards is the accumulated size checked. A replacement
goal larger than 1 MiB therefore becomes `foreign` immediately after staging.
A removal source larger than 1 MiB becomes `foreign` after it has already been
renamed out of the managed path.

**Failure scenario.** Remove a checked preservation artifact larger than 1
MiB. GWZ publishes the small authority, moves the exact source into the family,
then the next family inspection exceeds `MAX_FAMILY_BYTES` and refuses
cleanup. Every retry sees the same self-created family as foreign; the managed
leaf remains missing and the source remains quarantined. Large bundle
artifacts make this an ordinary input, not a pathological one.

**Impact.** A supported action can enter permanent recovery after its managed
mutation. The unbounded `read_to_end` also means the advertised family bound
does not bound memory or I/O before classification.

**Required correction.** Separate bounded recovery-protocol metadata from
artifact payload accounting. Stream/hash payloads with pre-read metadata
limits and preserve exact identity checks; if payload size is intentionally
unsupported, reject it before publishing authority or moving any name. Add
greater-than-1-MiB replacement and removal restart tests, including failure
after detach, exact retained bytes/identity, and terminal exhaustion.

### [P2-3] Complete checkout erases tracked/index-visible checked-artifact paths

**Where:**

- `gwz-core/src/git/gitbackend/preservation_image.rs:188-193`;
- `gwz-core/src/git/gitbackend/preservation_image.rs:196-228`; and
- `gwz-core/src/git/tests/g15.rs:453-482`.

**Violated contract.** RemPlan-3 section 3.5 explicitly says `.gwz`, reserved
trees, and member paths are never blanket exclusions and that tracked or
index-visible drift below them remains in the complete image. Exit criterion
6 requires every checkout overlay to be owned by an exact alternate observer.

**Evidence.** `raw_excluded_paths` unconditionally prepends
`.gwz/checked-artifacts` to both the worktree and index overlay lists. The
overlay then clears every descendant from the live and expected image. The
new regression test commits `.gwz/checked-artifacts/protocol`, changes its
worktree bytes, and asserts that the supposedly complete comparator still
returns true. No alternate aggregate projection proves that tracked file.

**Failure scenario.** A completed rollback participant has a tracked or staged
file below `.gwz/checked-artifacts`; it drifts after the durable terminal row.
The aggregate terminal-participant check reports exact and permits the next
owner's physical mutation or terminal rollback.

**Impact.** D3's completed-prefix proof remains incomplete, and the test suite
currently pins the behavior opposite to the accepted design.

**Required correction.** Exclude only verified untracked control-plane
residue after proving the private ignore boundary. Never erase a tracked or
index-visible row. Alternatively, reject such a tracked collision before the
first mutation. Reverse the existing test and add index semantic/type/stage,
worktree, and untracked service-level rows asserting no later-owner call.

### [P2-4] Complete checkout does not observe a submodule's live gitlink HEAD

**Where:**

- `gwz-core/src/git/gitbackend/preservation_image.rs:53-143`;
- `gwz-core/src/git/gitbackend/preservation_image.rs:188-193`;
- `gwz-core/src/git/gitbackend/preservation_image.rs:443-463`; and
- `gwz-core/src/workspace_ops/merge/abort/participants.rs:167-192`.

**Violated contract.** RemPlan-3 section 3.5 requires complete checkout
equality including gitlink facts for every terminal participant before every
later mutation.

**Evidence.** `live_entries` records Git's dirty summary, including worktree
modification status, but `checkout_matches_commit_with_overlay` discards that
summary. For a gitlink directory `read_worktree` then manufactures the
worktree value from the index OID; it never reads the nested checkout HEAD.
Consequently an independently moved submodule HEAD compares equal to the
superproject commit and index.

**Failure scenario.** Complete one participant's rollback, check out a
different commit inside one of its submodules without staging the gitlink, and
resume the workspace rollback. `terminal_v1_participant_is_exact` accepts the
participant and permits the next repository mutation.

**Impact.** A live user checkout fact named by the complete-prefix contract is
not owned by any observer.

**Required correction.** Observe the nested repository's exact HEAD for a
gitlink and compare it with the expected gitlink OID, or conservatively reject
any dirty gitlink status. Add direct comparator and aggregate service tests
for moved, missing, unreadable, and exact submodule checkouts.

### [P2-5] Validly stored out-of-order terminal rows escape aggregate verification

**Where:**

- `gwz-core/src/workspace_ops/merge/model/v1/validate/lifecycle.rs:22-56`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback_prefix.rs:186-225`.

**Violated contract.** RemPlan-3 section 3.5 says every participant already
recorded `Aborted` or `RolledBack` contributes an exact live terminal
projection before every mutation. Exit criterion 6 applies that aggregate at
every participant/no-op owner.

**Evidence.** v1 lifecycle validation accepts every participant-state
arrangement in `RollingBack`. For a current participant, `completed_prefix`
examines only the reverse-order slice before that cursor and filters terminal
rows within the slice. A terminal row outside that reverse-contiguous slice is
not checked even though the stored model is valid.

**Failure scenario.** With selected targets `[mem_a, mem_b]`, persist a valid
rolling-back record with `mem_a=RolledBack` and `mem_b=FastForwarded`, making
`mem_b` current in reverse order. Drift `mem_a`, then resume. The aggregate
checks no completed participant and permits the physical reset of `mem_b`.

**Impact.** The service mutates a later owner while a durable terminal row has
drifted, violating the aggregate's central no-later-mutation guarantee.

**Required correction.** Either make model validation reject every
non-contiguous reverse terminal layout, or include every terminal participant
in the aggregate regardless of cursor and reject an impossible ordering. Add
a directly seeded stored-record service test that drifts the out-of-order
terminal row and asserts byte-identical record plus zero Git/artifact calls.

### [P2-6] Selected-root physical `After` temporarily leaves lock/index facts unowned

**Where:**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback_prefix.rs:69-105`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback_prefix.rs:155-184`;
- `gwz-core/src/workspace_ops/merge/abort/participants.rs:95-131`;
- `gwz-core/src/workspace_ops/merge/abort/participants.rs:254-277`; and
- `gwz-core/src/workspace_ops/merge/abort/evidence.rs:81-103`.

**Violated contract.** The section 3.5 ownership table requires the selected-
root participant to supersede evidence only with a complete terminal ref,
HEAD, native, checkout, and index projection; evidence continues to own the
baseline boundary and absent marker. Every shared-root fact must have one
live owner through the handoff.

**Evidence.** A pending selected-root participant observed physically `After`
sets `selected_root_checkout_supersedes_evidence`, but the current root is not
yet in `completed_participants`, so no selected-root terminal or root-metadata
projection is checked. The reduced post-supersession evidence observer checks
only boundary worktree bytes and marker absence. Meanwhile the participant's
own checkout observer excludes the publication candidate paths. Lock
worktree/index and marker index facts can therefore change after a crash
following the ref reset and before `FinishParticipant` is persisted.

**Failure scenario.** Stop after the selected-root ref reset, alter the lock
or marker index while keeping the ref/HEAD and noncandidate checkout exact,
then resume. The pending participant still observes `After`; the aggregate
accepts only boundary/marker worktree residue and permits the successor record
rewrite. The next aggregate may detect drift, but the pending journal and
participant owner have already been consumed.

**Impact.** The evidence-to-selected-root handoff advances without a complete
owner for all shared-root facts, contrary to the frozen supersession table.

**Required correction.** In the pending selected-root `After` window, require
the exact selected-root terminal checkout/index projection and the still-owned
evidence boundary/marker projection before issuing a successor. Add ref-reset
restart tests with lock worktree, lock index, marker worktree, marker index,
and semantic-index drift; each must retain the pending action byte-for-byte.

### [P2-7] Matching executor diagnostics override real preservation ambiguity

**Where:**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver.rs:109-128`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver.rs:191-200`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver/reconciliation.rs:47-55`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/dispatcher_reconciliation.rs:216-220`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/dispatcher_reconciliation.rs:390-445`.

**Violated contract.** RemPlan-3 section 3.6 says matching `Success` or
`Failed` plus a fresh real `Ambiguous` fact has no diagnostic-derived outcome;
only the position's representable ambiguity/recovery rule applies. An
executor result is diagnostic, never authority.

**Evidence.** The general `reconcile` relation correctly chooses `Ambiguous`
before considering a matching attempt. The special
`PreservationAmbiguous` resolver bypasses that relation: if any matching
attempt exists, `preservation_attempt_without_pending_goal` returns a
diagnostic rejection before `ambiguous` can run. Existing tests deliberately
expect the late executor error and special-case preservation so it does not
enter `RecoveryRequired`.

**Failure scenario.** A preservation physical action reports either success
or failure, then the fresh observer finds a third-form family/prefix
ambiguity. The invocation returns the stale executor diagnostic instead of
persisting the representable recovery overlay and retaining the exact pending
action.

**Impact.** Fresh live state does not own the durable decision. The user must
retry merely to obtain the recovery state the first invocation already had
authority to persist.

**Required correction.** Route `PreservationAmbiguous` through the same
attempt/fact reconciliation relation after validating action and prefix
bindings. Matching success and failure must both apply the exact ambiguity
rule; stale/mismatched/consumed attempts must still reject. Replace the tests
that pin diagnostic precedence and add service rows asserting exact recovery
bytes and retained pending action for both diagnostics.

### [P2-8 proof gap] C7 counts a vocabulary but does not execute the declared per-row service matrix

**Where:**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/reconciliation.rs:7-59`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/root_rows.rs:7-26`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/root_rows.rs:86-101`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/rollback_rows.rs:3-35`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/request_dispatch.rs:19-60`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/c7_matrix/matrix_spec.rs:77-118,359-407`.

**Violated contract.** Section 3.6 and exit criterion 8 require every declared
row to be crossed with its complete attempt/fresh-fact alphabet through real
observers and terminal service exhaustion, with exact calls, bytes, identity,
journal, and ambiguity assertions.

**Evidence.** The 635 physical and 40 causal counts repeatedly invoke the same
pure `reconcile(class, attempt, fact)` function. The row identity is assigned
to an unused variable; rollback, non-root, and checked-artifact identities are
discarded or replaced with a dummy enum value. The 1,350 root count merely
collects tuples. `request_dispatch` forges one backup observation and one
rollback observation with `for_test`; it does not seed or execute each row.

The cardinality is also incomplete by construction: `EvidencePending` is in
`HANDOFFS` but excluded from both owners' `legal_handoffs`, and the test
asserts its exclusion. Checked-artifact rows contain seven coarse labels with
no platform/fault-boundary dimension. Action-free positions are only counted.
The focused C7 suite has seven tests and completes in 0.04 seconds, consistent
with table arithmetic rather than the declared service cross-product.

**Impact.** C7 can remain green when a real observer, dispatcher, journal, or
checked platform boundary violates the relation. P2-7 is a concrete example:
the pure relation passes while production resolver behavior is opposite.

**Required correction.** Make one generated executor consume the declarative
row identity, directly seed every legal durable row, install real physical
`Before`/`After`/`Ambiguous`/error forms, and drive production observer,
resolver, executor, store, and restart to exhaustion for all five requests.
Derive cardinalities from executed unique cases and assert exact record and
artifact bytes, calls, identities, retained ambiguity, journal consumption,
and terminal state. Include both absent forms, every platform durability/fault
attempt, causal-parent variants, source-equals-goal, and every action-free
position.

### [P3-1] Rollback-prefix request binding and observer-only construction are not structurally enforced

**Where:**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority.rs:109-153`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/binding.rs:54-87`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback_prefix.rs:240-250`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver/execution.rs:33`.

**Violated contract.** Section 3.5 requires `VerifiedRollbackPrefix` to bind
the exact request, aggregate position, current owner/action/phase, and record;
only an observer-private constructor may issue it. Section 3.6 requires a
compile/privacy and call-graph gate proving executors cannot construct or
receive the proof.

**Evidence.** The payload contains position and projection fields but no
request/current owner/action/phase. Its binding is always the fixed
`@operation/rollback_prefix/aggregate_verified` tuple. `AuthorityIssuer` and
`for_observer` are `pub(super)` in the `authority` module, so sibling resolver
code already constructs the purported observer issuer. Current grep finds
prefix issuance on observer paths, but that is convention, not the required
structural boundary, and no negative compile/static gate pins it.

**Impact.** This is not a demonstrated current replay exploit because binding
still includes exact record digest/location/operation identity and current
callers issue prefixes during observation. It is nevertheless an open
architecture gate: future resolver/executor code can construct authority, and
two requests at the same record/position are not distinguished by the token.

**Required correction.** Move the prefix type and constructor behind an
observer-private module boundary, accept and bind `BoundObservationRequest`,
request, position, exact owner/action/phase, and keep the executor API at the
non-authoritative `require_rollback_aggregate -> ()` seam. Add the specified
negative compile/static call-graph test.

## D1-D4 closure audit

| Item | Assessment |
| --- | --- |
| D1 checked-artifact transition | **Open.** Immutable authority, staged durability, explicit policy, and conditional cleanup are real improvements, but platform family identity is not equivalent/reboot-qualified (P2-1), and the byte bound can make a valid self-created action permanently foreign (P2-2). |
| D2 quarantine placement | **Materially closed.** Callers provide explicit workspace/Git-directory policy, and the implementation rechecks an opened rename domain. P2-1 still affects the authority identity layered above placement. |
| D3 complete rollback aggregate | **Open.** Tracked private-prefix facts and live gitlinks are omitted (P2-3/P2-4); out-of-order terminal rows and the selected-root handoff leave terminal/shared facts unchecked (P2-5/P2-6). |
| D4 declarative C7 | **Open.** The vocabulary exists, but the generated service/fault/restart evidence and several declared dimensions do not (P2-8). |

The special reconciliation defect P2-7 and privacy gap P3-1 are additional
RemPlan-3 exit-criterion failures. The checked native-stash boundary retained
from D2 and the earlier root result-commit artifact verifier were not found to
regress in this review.

## Verification performed

On the exact reviewed production/test commits:

- workspace, `gwz-core`, and `gwz-cli` commit IDs matched the review request;
- `cargo test -q -p gwz-core c7_matrix --lib -- --test-threads=1` passed 7
  tests in 0.04 seconds;
- `cargo test -q -p gwz-core dispatcher_reconciliation --lib --
  --test-threads=1` passed 6 tests in 0.13 seconds; and
- no production code or tests were changed.

Those green focused tests are consistent with the findings. C7 mostly checks
pure relations/counts, and the dispatcher tests currently assert the
diagnostic-precedence behavior identified in P2-7.
