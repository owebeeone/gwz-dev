# R4b-P P1/P2 third remediation plan

Date: 2026-08-12

Status: **accepted for implementation after independent filesystem and
state-machine confirmation reviews**. R4b-G and A1 remain blocked until this
remediation's settled implementation and acceptance gates pass.

## 1. Scope and controlling evidence

This plan responds to the two independent reviews of the second remediation
checkpoint:

- workspace commit `1c6bbba13586cb8be50576301072b38f0e0a3463`;
- `gwz-core` commit `7d1a3e6a33cf2e4b5d882db1f7196f16ea7cc87d`;
- `gwz-cli` commit `3cca145c0b32410f250f640730ed7ca18f1da59f`;
- `GwzM5-8R4bP1P2-ReviewArch-3.md`; and
- `GwzM5-8R4bP1P2-ReviewFaults-3.md`.

Both reviews are NO-GO. There is no P0 finding. D2, the checked native-stash
boundary, is closed and is not redesigned here. The selected-root
result-commit artifact verifier is also retained. This plan closes the
remaining checked-artifact, complete-checkout, live-prefix, and generated C7
evidence gaps without changing the v1 wire model, adding a durable lifecycle
phase, or enabling the production v1 decoder/dispatcher.

Where this plan is more specific than `GwzM5-8R4bP1P2-RemPlan-2.md`, it
supersedes that plan's physical implementation detail. The I2 action journal,
R4b transition design, and six-step delivery sequence remain controlling.

## 2. Consolidated blockers

| ID | Defect | Source findings |
| --- | --- | --- |
| D1 | Checked-artifact residue is not durably bound to the retained parent and source identity; removal can execute ambiguity and staging is not restart-closed | Faults P1-1/P1-2/P1-3; Arch P1-1 |
| D2 | Quarantine placement is inferred from Git discovery and can cross filesystems for worktree artifacts | Arch P2-2 |
| D3 | Rollback entry excludes more than the publication-owned path set, while later rollback skips live proof of already completed participants | Faults P2-1; Arch P2-1 |
| D4 | C7 remains a fixture-derived subset with synthetic ambiguity rather than the declared legal service matrix | Faults P2-2; Arch P2-3 |

The two reviews describe D1 at different depths but agree on the same failure:
after detach and restart, a changed parent or same-byte replacement can be
mistaken for the action's original authority and the retained original can be
deleted. The correction is therefore one transition redesign, not a guard
added only to `remove_exact`.

## 3. Frozen architecture

### 3.1 Artifact-root policy is explicit

`CheckedArtifact::acquire` no longer discovers a repository to choose private
storage. Its caller supplies one of two explicit policies:

```text
WorkspaceArtifact {
    artifact_root,
    private_root: artifact_root/.gwz/checked-artifacts
}

GitDirectoryArtifact {
    artifact_root: retained_git_directory,
    private_root: retained_git_directory/gwz/checked-artifacts
}
```

The policy is a typed internal value, not two loosely related paths. Worktree
files always use the workspace policy, even when the worktree has a
discoverable repository. Files whose artifact root really is the retained Git
directory use the Git-directory policy. Linked worktrees and separate Git
directories therefore do not redirect a worktree artifact into another
filesystem.

The constructor retains opened no-follow handles for the artifact root,
managed parent, and private parent and obtains two typed platform facts before
publishing authority or scratch state:

```text
DurableObjectIdentity =
    Linux { durable_filesystem_id, persistent_file_handle }
  | Mac { volume_uuid, persistent_object_id }
  | Windows { volume_guid, file_id: [u8; 16] }

InvocationObjectIdentity =
    Unix { device: u64, inode: u64 }
  | Windows { volume_guid, file_id: [u8; 16] }

RenameDomainProof =
    LinuxMountId(u64)
  | MacMountedFileSystem(fsid)
  | WindowsMountedVolume(volume_guid)
```

Durable identity is persisted in authority and must be stable across the
supported process-crash, power-loss, and reboot model. Linux uses an opened
object's persistent filesystem file handle plus a durable filesystem
identifier; macOS uses a volume UUID plus a filesystem-promised persistent
object ID; Windows uses a volume GUID plus the complete 128-bit `FileIdInfo`
ID. The existing truncated `MetadataExt::ino()` and a Windows volume serial are
not durable authority. A filesystem that does not promise and expose these
facts returns a typed pre-mutation `UnsupportedOperation`; automatic reboot
recovery is not silently converted into permanent ambiguity.

Invocation identity is used for immediate same-process substitution checks.
`RenameDomainProof` is invocation-local, is never serialized, and proves only
that the currently opened managed and private parents share one atomic rename
domain. Linux uses mount ID (not only `st_dev`) so a nested or bind mount is not
mistaken for the same domain; macOS and Windows use their currently mounted
filesystem/volume facts. If the two handles cannot be proved equal, the policy
is unsupported before authority, goal, or source residue publication. An
`EXDEV` or equivalent is never converted to copy/delete.

The private hierarchy is excluded from the relevant managed classifier by the
already verified workspace/Git ignore boundary. Tests assert opened rename
domain identity, not lexical ancestry, and cover a nested mount when the host
can create one. Windows exact-tree evidence must execute the full-width
identity and checked-artifact behavioral tests; a successful target build is
not sufficient durability evidence.

### 3.2 One immutable authority record owns a checked-artifact action

The current filename convention is replaced by a private, immutable action
authority. This authority is filesystem-private state, not a v1 wire field.
It is created before goal staging or source detachment and contains:

```text
CheckedArtifactAuthorityV1 {
    schema,
    artifact_family_key,
    action_key,
    durable_artifact_root_identity,
    operation: Replace | Remove,
    canonical_path_identity,
    expected_kind_and_sha256,
    goal_kind_and_sha256,
    durable_retained_parent_identity,
    retained_source: Missing | Existing { durable_identity },
}
```

Two keys are deliberately separate:

- `artifact_family_key` binds only the durable artifact-root identity and a
  platform-equivalent canonical **root-relative component sequence**. It owns
  every action ever retained for that leaf and does not contain the current
  parent object's identity.
- `action_key` binds the family, replace/remove operation, exact expected
  kind/bytes, and exact goal kind/bytes.

Canonical path identity is obtained through a platform seam that reflects the
artifact filesystem's case and normalization equivalence for the complete
root-relative component sequence. Raw lossless encoding and
`to_string_lossy` are both insufficient. Parent handles may be consulted to
query filesystem rules, but no parent object identity enters the path or
family key. Filesystems with per-directory equivalence modes that cannot yield
one stable root-relative identity are rejected before authority publication.
Tests inject case-insensitive and normalization-insensitive equivalence on
every host, exercise real aliases where the host volume supports them, and
replace the retained parent before reacquiring by both the original and an
alias spelling. Both reacquisitions must find the old family.

Every acquire, classify, and execute enumerates the complete artifact family
before it may publish authority. Exactly one authority matching the requested
action may proceed. Another action, duplicate authority, malformed entry,
different parent/source binding, or an alias naming the same leaf is
`Ambiguous`. Thus a new expected/goal tuple or path spelling cannot hide an
older residue.

The authority record is a versioned private recovery protocol. It uses one
canonical bounded binary encoding with a fixed magic/version, lossless bounded
identities and fixed-width hashes, a 4 KiB path-identity ceiling, a 16 KiB record ceiling,
and bounded family entry count/bytes. Unknown versions, oversized values, and
noncanonical encodings remain visible and classify the family as
`Ambiguous`; they are never interpreted as another version.

Authority publication is restart-closed:

1. observe the retained parent and exact source, including source identity;
2. write a uniquely named scratch file outside the family namespace;
3. flush and revalidate that file;
4. rename it no-replace to the deterministic authority name;
5. establish the quarantine platform durability barrier; and
6. reread the exact authority before it can authorize another step.

A crash before step 4 leaves no family authority and no managed mutation. The
unpublished scratch file is inert and is never treated as authority. A crash
after step 4 is closed by reopening, revalidating, flushing, and re-establishing
the platform durability barrier for the exact record. A conflicting or
malformed family entry is `Ambiguous`; it is never repaired by overwriting or
deleting it.

Correctness never depends on scratch garbage collection. Recognizable orphan
scratch is retained in this remediation; a lease-held, identity-checked GC and
its storage/privacy bounds are an explicit pre-A1 P3 item. Foreign entries are
never collected merely because their names resemble scratch.

### 3.3 Goal staging is its own durable transition

The deterministic `.goal` file is never written in place. Goal staging uses:

```text
Absent -> ScratchWritten -> ScratchFlushed -> GoalPublished -> GoalDurable
```

The scratch file is unique and outside the family namespace. Only after exact
bytes and full-width file identity have been observed and the file has been
flushed is it renamed no-replace to an identity-bearing family goal name. On
restart, a visible goal becomes `GoalDurable` only after no-follow reopen,
encoded-identity verification, exact-byte verification, another file flush,
and another platform durability barrier.

Partial unpublished scratch files are inert and do not block or authorize the
action. A partial, malformed, wrong-identity, wrong-byte, extra, or
different-identity duplicate family goal is foreign and makes the action
`Ambiguous`. Exactly one authority-bound source/destination pair with the same
full identity is the crash-produced `RecoverableDuplicate` form defined below.
No exact visible bytes alone are accepted as evidence of a completed file or
directory durability barrier.

The platform contract is explicit:

- Unix uses atomic handle-relative no-replace rename, then syncs the
  destination directory before the source directory for every cross-directory
  move. Same-directory mutations require one directory sync.
- Windows uses an opened-source, opened-destination, handle-relative
  no-replace rename with write-through semantics. Quarantine namespace
  retirement uses one separately owned `DurabilityAnchor` per private
  directory and the accepted write-through round-trip rename protocol before
  the next destructive edge. The anchor is outside every artifact-family
  prefix, so family enumeration cannot mistake it for action residue. Every
  handle and name is reobserved after the barrier.
- If a supported Windows filesystem cannot provide the full identity or the
  write-through causal guarantee, the action fails before managed mutation and
  the design returns to review; directory `sync_all` is not claimed.

For a cross-directory move the causal order is: source file flushed;
destination name published; destination namespace made durable; source-name
retirement made durable; then both sides reobserved. Managed goal/absence is
durable before quarantined-source cleanup; source cleanup is durable before
authority retirement. Crash-created source/destination names with the same
encoded full identity are a distinct `RecoverableDuplicate` form. Same bytes
with different identity remain `Ambiguous`.

`DurabilityAnchor` is a bounded versioned platform protocol, not family state.
Its exact fixed bytes and full durable identity are bound by its
identity-bearing reserved name. It is created and made write-through durable
before the first family authority and then persists for the private
directory's lifetime; actions never retire it. Policy acquisition recognizes
only these anchor forms:

| Final name | Round-trip name | Result |
| --- | --- | --- |
| exact anchor | absent | `Ready` |
| absent | same exact anchor | `NeedsReturn` and write-through rename to final |
| same-identity exact aliases | same-identity exact aliases | `NeedsRetireAlias` after final durability |
| missing while no family authority/residue exists | absent | create a new anchor before the action |
| any other form, including missing with retained family state | any | typed recovery/unsupported before family mutation |

Anchor creation, first/second round-trip rename, alias retirement, and
reobservation each have before/after fault points. This closed anchor protocol
is checked before family enumeration on Windows and is separately tested; it
does not alter the artifact-family residue table.

The fault vocabulary includes before/after boundaries for:

- authority scratch creation, write, file flush, publication, and parent
  barrier;
- goal scratch creation, write, file flush, publication, and parent barrier;
- source detach, destination durability, source retirement, and both
  reobservations;
- managed-goal publication, managed destination durability, quarantine source
  retirement, and both reobservations; and
- exact source cleanup, authority cleanup, and quarantine-parent barriers.

### 3.4 Closed residue classifier and conditional cleanup

The classifier reads the exact authority first, then the identity-bearing
source/goal residues and managed leaf. `S` means the exact authority-bound
quarantined source and `G` the exact durable staged goal. “Same alias” means
two names proven to carry the same full durable object identity, not merely the
same bytes. It implements the three operation tables literally.

Source-equals-goal replacement is a separate, highest-priority proof-only
operation. It never enters an existing-source replacement transition:

| Authority | Any family residue | Managed leaf | Result |
| --- | --- | --- | --- |
| absent | absent | exact durable source/goal | `After` / `ProofOnly` |
| any other form | any | any | `Ambiguous` |

The successful row performs zero authority, staging, detach, publication,
cleanup, or alias-retirement mutations and preserves the managed object's
identity. Pre-existing authority or residue is not treated as cleanup work for
this otherwise no-op invocation: it is `Ambiguous`, retains every name, and
requires explicit recovery. A missing, changed, or unreadable managed leaf is
likewise not proof of completion: missing or changed is `Ambiguous`, while an
unreadable leaf returns the operational observer error. C7 gives this
operation its own cardinality and asserts zero physical calls, unchanged
object identity, and unchanged family state.

Existing-source replacement applies only when exact `source != goal`:

| Authority | Source residue | Staged goal | Managed leaf | Result |
| --- | --- | --- | --- | --- |
| absent | absent | absent | exact source | `Before` |
| absent | absent | absent | exact durable goal | `After` |
| exact/current | absent | absent or `G` | exact bound source | `BeforeBound` |
| exact/current | `S` | `G` | missing | `RecoverableDetached` |
| exact/current | `S` | absent | exact durable goal | `RecoverablePublished` |
| exact/current | `S` | absent or `G` | same source alias | `RecoverableDuplicateSource` |
| exact/current | `S` | same goal alias | exact durable goal | `RecoverableDuplicateGoal` |
| exact/current | absent | absent | exact durable goal | `After` after authority cleanup |
| anything else | any | any | any | `Ambiguous` |

Missing-source replacement:

| Authority | Source residue | Staged goal | Managed leaf | Result |
| --- | --- | --- | --- | --- |
| absent | absent | absent | missing | `Before` |
| absent | absent | absent | exact durable goal | `After` |
| exact/current | absent | absent | missing | `BeforeBound` |
| exact/current | absent | `G` | missing | `RecoverableStaged` |
| exact/current | absent | same goal alias | exact durable goal | `RecoverableDuplicateGoal` |
| exact/current | absent | absent | exact durable goal | `After` after authority cleanup |
| any | present | any | any | `Ambiguous` |
| anything else | any | any | any | `Ambiguous` |

Existing-source removal:

| Authority | Source residue | Staged goal | Managed leaf | Result |
| --- | --- | --- | --- | --- |
| absent | absent | absent | exact source | `Before` |
| absent | absent | absent | missing | `After` |
| exact/current | absent | absent | exact bound source | `BeforeBound` |
| exact/current | `S` | absent | missing | `RecoverableDetached` |
| exact/current | `S` | absent | same source alias | `RecoverableDuplicateSource` |
| exact/current | absent | absent | missing | `After` after authority cleanup |
| any | any | present | any | `Ambiguous` |
| anything else | any | any | any | `Ambiguous` |

A same-byte/different-identity source/goal pair is always `Ambiguous`. A
provably same-full-identity duplicate follows only its named duplicate path:
make the desired destination durable, retire the undesired alias durably, and
reobserve. A source residue naming another parent, an identity mismatch, a
changed current parent, or any foreign family entry is `Ambiguous`. Unit tests
enumerate every cell and reject every unlisted cross-product.

Both `replace_exact` and `remove_exact` exhaustively dispatch the classifier:

- source-equals-goal `After` / `ProofOnly` returns success without residue or
  authority retirement;
- an ordinary transition's `After` returns success after safe
  residue/authority retirement;
- the named `Before*`, `Recoverable*`, and `RecoverableDuplicate*` facts may
  continue only through their distinct exact transitions;
- `Ambiguous` returns a typed recovery error before opening a mutation path.

Cleanup is an exact conditional operation, not `remove_file(name)`. Immediately
before removing a quarantined source it revalidates:

- the exact authority and its retained parent/source identities;
- the current mapping of the retained parent;
- the quarantined source's encoded and observed identity and exact bytes; and
- the exact durable managed goal (or durable absence for removal).

If any proof fails, cleanup retains every name. Cleanup never follows a
symlink, substitutes a same-byte identity, removes a different-identity
duplicate, or restores over a non-missing destination. It retires a
same-full-identity crash alias only through the explicit durable duplicate
transition above. Authority retirement occurs last and is
followed by the platform quarantine-retirement barrier. A crash during
retirement can only leave a closed `After` form that repeats verification and
cleanup without repeating managed mutation.

The workspace mutation lease is the cleanup concurrency boundary. Cooperating
GWZ/Git writers cannot mutate the private namespace while the action runs;
arbitrary raw mutation of `.gwz`/the retained Git directory is outside the
supported concurrency contract. Cleanup nevertheless reopens and verifies the
full identity immediately after a deterministic pre-linearization hook, then
uses the handle-relative platform retirement primitive. Tests substitute a
same-byte/different-identity family entry at that hook and require retained
names plus `Ambiguous`; the design does not claim atomic compare-and-unlink
against an uncooperative writer.

### 3.5 Complete rollback authority is a cursor-relative aggregate

The implementation seam is split so only the observer can issue authority:

```text
classify_rollback_aggregate(backend, canonical_root, validated_model, position)
    -> RollbackAggregateFacts                 // non-authoritative

require_rollback_aggregate(...)
    -> ()                                     // executor recheck only

issue_verified_rollback_prefix(stored_record, bound_request, position, facts)
    -> VerifiedRollbackPrefix                 // observer-private
```

`VerifiedRollbackPrefix` binds the checked record digest and canonical open
location, workspace/merge/operation identity, exact aggregate position,
request, current owner/action/phase, and projection hash. Reverse-entry
preflight evaluates the anticipated model only inside the existing sealed
preview visitor and folds the aggregate result into
`VerifiedRollbackEntryPreflight`; it never issues a prefix proof for a raw
anticipated record. The executor retains the mutation lease and exact
persisted action, calls only the non-authoritative `require` function, and
cannot name or receive the opaque proof type.

The aggregate position is closed:

```text
RollbackAggregatePosition =
    ReverseEntry
  | EvidencePending(step)
  | BetweenParticipants(next_member_or_root)
  | ParticipantPending(member_id, kind)
  | NoMutationParticipant(member_id)
  | SelectedRootMetadataPending(step)
  | Exhaustion
  | RecoveryPending(exact_pending_action)
```

The aggregate projection assigns every shared root fact to one live owner:

| Cursor position | Root ref/HEAD/native and complete checkout | Boundary and marker | Lock/manifest worktree | Relevant index entries |
| --- | --- | --- | --- | --- |
| reverse entry / evidence pending | current evidence-step projection | current evidence-step projection | current evidence-step projection plus complete checkout | current evidence-step projection |
| evidence complete; before selected-root participant completes | exact evidence-result root at accepted base; checkout excludes only lock/marker | evidence-result baseline boundary and absent marker | evidence-result baseline lock; manifest remains in complete checkout | evidence-result baseline candidate entries |
| after selected-root participant; before/during root metadata | exact selected-root terminal ref/HEAD/native and checkout at `before_commit`; worktree-only overlay paths are excluded as named below | retained evidence-result baseline boundary and absent marker | current root-metadata step/result projection | selected-root terminal commit index, with no blanket index exclusion |
| exhaustion without selected root | exact evidence-result root/checkout | exact evidence-result boundary/marker | evidence-result lock; manifest in checkout | evidence-result baseline candidate entries |
| exhaustion with selected root | exact selected-root terminal root/checkout | retained evidence-result boundary/marker | exact operation-baseline manifest/lock | selected-root terminal commit index |

The table applies only when publication evidence exists. With no evidence,
root facts are owned by the ordinary participant/no-mutation projection or the
unchanged operation baseline as appropriate. Every ordinary participant
already recorded `Aborted` or `RolledBack` contributes an exact live terminal
projection: validated path, clean native state, attached target branch, exact
HEAD and target ref at `before_commit`, and complete checkout equality
including semantic flags, staged/unstaged/untracked/type/rename/conflict and
gitlink facts.

The evidence result is decomposed so supersession is explicit. Before selected
root completes, it owns the root ref/HEAD, candidate paths/index, boundary,
and marker. After selected-root participant completion, that participant
supersedes evidence ref/HEAD/native and commit-index facts; evidence continues
to own only the baseline boundary and absent marker. Selected-root metadata
then owns manifest/lock worktree bytes phase by phase and finally at the exact
operation baseline. No earlier whole-form observer is rerun after a later
owner intentionally supersedes one of its facts.

Complete checkout accepts domain-specific overlays, not one path-erasing list:

```text
CheckoutOverlay {
    worktree_paths: exact paths proved by another aggregate projection,
    index_paths: exact paths proved by another aggregate projection,
}
```

Before selected-root rollback, lock/marker worktree and index facts may be
overlaid only because the current evidence projection proves them. After the
selected-root participant, manifest/lock worktree bytes may be overlaid only
while the root-metadata projection proves them; the index remains exact to the
participant's terminal commit. `.gwz`, `gwz.conf/.tmp`, manifest member
directories, and reserved-prefix trees are never blanket exclusions. Tracked
or index-visible drift below any such prefix is therefore retained in the
complete image and rejects.

The aggregate is required before reverse entry, before every next-owner/no-op
observation, in the executor immediately before a physical mutation, during
rolling-back recovery-origin verification, and before exhaustion. Recovery
behavior follows the frozen representability rule:

- reverse-entry and action-free `RollingBack` mismatches return a typed error
  with no record rewrite;
- a mismatch with the exact pending rollback action may enter
  `RecoveryRequired(origin=RollingBack)` while retaining that action
  byte-for-byte;
- an executor mismatch returns only a diagnostic; the fresh post-attempt
  observer decides whether the pending form is representable ambiguity; and
- `ResumeFromRecovery` proves both the complete aggregate prefix and exact
  current action before issuing `VerifiedRecoveryOrigin`.

Thus an action-free mismatch never invents an illegal recovery overlay. A
representable pending-action mismatch writes the overlay at most once; later
requests are byte-stable until the aggregate and action become exact.

### 3.6 C7 is declarative, row-typed, and coverage-closed

Test discovery by executing a favored fixture is removed. A concept-owned
`matrix_spec` module declares the exact durable domains:

- root owner `PublicationRoot` and selected participant `@root`;
- distinct absent-handoff forms `NoCandidate` and `EvidencePending`;
- candidate forms `Baseline/Pre`, `Marker/Pre`, `Lock/Pre`, `Boundary/Pre`,
  conditional degenerate `Marker/Staged`, and `Boundary/Staged`, with the
  exact publication/equality guards from amendment 1;
- all five non-`Status` requests admitted by the frozen dispatcher:
  `ResumeStart`, `Continue`, `Abort`, `Preserve`, and `Archive`;
- the complete 22-phase root vocabulary and the short graphs for
  `NoCandidate`/`EvidencePending`;
- both physical participant rollback kinds, `RecordNoMutationAbort`, every
  evidence/root-metadata step, and every non-root preservation row; and
- action-free begin, finish, cursor, recovery, and exhaustion positions.

The source rows are explicit and are never derived by tracing execution.
Every row declares its legal observation and attempt alphabets:

```text
PhysicalTransition:
    fresh observation = Before | After | real Ambiguous | operational error
    prior attempt = None | matching Success | matching Failed
                  | stale/mismatched | consumed second attempt

CausalParentTransition:
    fresh observation = Before | AfterNeedsDurability
                      | real Ambiguous | operational error
    prior attempt = None | matching Success | matching Failed
                  | stale/mismatched | consumed second attempt

ProofOnly:
    After, real Ambiguous where physically representable, operational error

ActionFree:
    exact proof, prefix mismatch/error
```

Every ordinary physical preservation/rollback row generates the complete
legal fact × attempt relation:

| Prior attempt | Fresh fact | Required result |
| --- | --- | --- |
| none | `Before` | execute exactly once, then reobserve |
| none | `After` | durable successor, zero executions |
| none | real `Ambiguous` | zero executions and no outcome; use only the position's representable recovery/no-rewrite rule |
| none | operational read error | retain owner, issue no successor, and execute zero times |
| matching `Success` or `Failed` | `Before` | retain owner; no successor and no second execution in the invocation |
| matching `Success` or `Failed` | `After` | advance from fresh live proof, regardless of diagnostic |
| matching `Success` or `Failed` | real `Ambiguous` | no outcome; use only the position's representable recovery/no-rewrite rule |
| any matching attempt | operational read error | retain owner and issue no successor |
| stale, mismatched, or consumed second attempt | any | reject without progress or execution |

`Failed + After` therefore advances, `Success + Before` does not fabricate
completion, and neither diagnostic is authority. Counting matching success
and failure separately, every ordinary physical row has five attempt classes
crossed with four fresh-fact classes: exactly 20 declared cells.

Every causal-parent row instead implements this separate complete relation:

| Prior attempt | Fresh fact | Required result |
| --- | --- | --- |
| none | `Before` | execute the bound physical parent step exactly once, then reobserve |
| none | `AfterNeedsDurability` | execute the bound idempotent durability barrier exactly once, then reobserve |
| none | real `Ambiguous` | zero executions and no outcome; use only the position's representable recovery/no-rewrite rule |
| none | operational read error | retain owner, issue no successor, and execute zero times |
| matching `Success` | `AfterNeedsDurability` | advance to the durable successor from the fresh causal proof |
| matching `Failed` | `AfterNeedsDurability` | retain owner; issue no successor and do not retry in this invocation |
| matching `Success` or `Failed` | `Before` | retain owner; issue no successor and do not execute a second time in this invocation |
| matching `Success` or `Failed` | real `Ambiguous` | no outcome; use only the position's representable recovery/no-rewrite rule |
| any matching attempt | operational read error | retain owner and issue no successor |
| stale, mismatched, or consumed second attempt | any | reject without progress or execution |

Counting matching success and failure separately, every causal-parent row also
has exactly 20 declared cells: five attempt classes crossed with `Before`,
`AfterNeedsDurability`, real `Ambiguous`, and operational error. Ordinary
`After` is not in this alphabet. The required empty-parent classifier always
emits `AfterNeedsDurability`; optional and already-established parents are
separate `ProofOnly` rows. Thus only matching
`Success + AfterNeedsDurability` may advance directly, while a no-attempt
`AfterNeedsDurability` executes its barrier once and reobserves.

Source-equals-goal and optional/already-established parent rows are
`ProofOnly`; stash/reset `Complete` and exhaustion are `ActionFree`. The
required empty-parent form uses `AfterNeedsDurability` /
`PreservationDurabilityPending`, not ordinary `After`. Its alphabet includes
the Unix parent-sync and both Windows write-through round-trip rename
boundaries, restart before successor rewrite, and matching failure/retry.
No synthetic `Before` or `Ambiguous` form is fabricated for a proof-only row;
real source-equals-goal family residue remains the physically representable
`Ambiguous` form declared in section 3.4.

Legal base rows are crossed only with their declared alphabets and all five
requests at `Preserving`/`RollingBack` dispatch. Checked-artifact rows add the
authority/staging/detach/publication/cleanup durability boundaries from
sections 3.2-3.4. Rollback ambiguity is installed through real production
inputs: wrong native conflict/checkout, completed-prefix drift, third-form
evidence/root artifact, wrong ref, semantic index flags, and changed parent;
no row constructs `BoundAmbiguityEvidence` directly.

Exhaustive enum matches plus independent cardinalities for publication shapes,
candidate forms, conditional degenerate forms, owners, requests, phases,
physical/no-op/action-free rows, observation alphabets, executor diagnostics,
attempt/fact products, and platform attempts fail when a legal decision is
missing. Assertions cover total and per-action physical calls, no later-owner
call on prefix drift, exact successor or retained record bytes, exact
managed/quarantine bytes and identities, bundle/stash state, pending journal
retention, and restart to terminal exhaustion. The existing root-inclusive
canonical bundle remains a byte fixture.

A compile/privacy gate proves that executor modules cannot construct or
receive `VerifiedRollbackPrefix`, observer proof constructors remain private,
and no raw model/path call can issue replayable authority.

## 4. Implementation sequence

### R0 — independent interface checkpoint

Two independent reviewers assess sections 3.1-3.6 before production edits:

- one for filesystem identity, durability, Windows, separate-Git-dir, and
  restart closure; and
- one for rollback authority, legal state-machine coverage, and C7 feasibility.

The first pass is recorded in `GwzM5-8R4bP1P2-RemPlan-3-ReviewFS.md` and
`GwzM5-8R4bP1P2-RemPlan-3-ReviewState.md`; both were NO-GO. The second focused
pass is recorded in the matching `-2` files; it closed the original state
findings but found the remaining stable-family/reboot/barrier and physical
attempt-matrix gaps. The matching `-3` reviews confirmed those corrections and
identified only the source-equals-goal table overlap and the missing explicit
no-attempt/causal-parent cells. Both narrow findings are incorporated above.
The final confirmations are recorded in the matching `-4` files; both report
GO with no open P0/P1/P2 design finding. R1 may begin from this accepted
interface. No ad hoc code patch preceded this checkpoint.

### R1 — checked-artifact authority and staging

1. Introduce full-width platform object identity, rename-domain identity,
   filesystem-equivalent path identity, explicit artifact-root policy, and the
   bounded private protocol.
2. Implement restart-closed authority and staged-goal publication using the
   frozen Unix/Windows durability contracts.
3. Implement the operation-specific closed classifiers, same-identity crash
   duplicate recovery, source-equals-goal proof-only classification, and exact
   conditional cleanup.
4. Add direct transition tests for every table row, truncated/full-width
   and reboot-stable identity distinction, alias equivalence, nested mount,
   identity substitution, parent replacement, Windows durability-anchor
   states, partial scratch, foreign family entry, source-equals-goal zero-call
   identity preservation, and every two-directory fault boundary.
5. Compile and run the synthetic Windows identity/platform seam tests before
   converting a consumer; retain exact-tree Windows behavioral execution for
   R5.
6. Run the focused checked-artifact suite before converting a consumer.

Stop and return to design review if the implementation needs a v1 wire field,
cannot obtain reboot-stable full-width identity, stable path equivalence, or
invocation-local rename-domain facts,
cannot establish the Windows write-through causal guarantee, cannot
distinguish inert unpublished scratch from action authority, cannot keep both
names on ambiguity, or needs a copy/delete cross-filesystem fallback.

### R2 — explicit policy at every consumer

1. Classify every checked-artifact caller as workspace-root or Git-dir owned.
2. Convert preservation bundle, publication evidence/root rollback,
   selected-root metadata rollback, and root-preservation file consumers.
3. Add consumer-crossed replacement/removal restarts for parent replacement,
   same-byte identity substitution, same-identity crash duplicates,
   staged-file boundaries, both sides of cross-directory durability, and
   cleanup failures.
4. Add linked-worktree/separate-Git-dir tests and an optional real cross-device
   and nested-mount test when the host exposes those facilities; the opened
   rename-domain assertion is mandatory on every host.

### R3 — exact rollback prefix and selected-root scope

1. Extract pure aggregate fact classification and exact terminal-participant,
   evidence-result, and selected-root overlay projectors beside the existing
   rollback observers.
2. Add the cursor-relative aggregate authority at entry, every owner/no-op
   observation, execution, recovery resume, and exhaustion while keeping proof
   issuance observer-private.
3. Replace path-wide exclusions with domain-specific checkout overlays and the
   exact alternate observer required for each overlaid fact.
4. Add multi-owner service tests that stop after a durable evidence or
   participant successor, drift the completed prefix, and resume with every
   admitted request.
5. Cross `Aborted`/`RolledBack`, branch/HEAD/ref/native state, semantic flags,
   staged/unstaged/untracked/type/rename/conflict drift, and tracked content
   beneath each reserved prefix. Assert no later mutation and no false terminal
   abort.
6. Cross action-free and pending-action prefix mismatches, repaired and
   unrepaired recovery retries, exact record-byte rules, shared-root
   supersession, and privacy/call-graph gates.

### R4 — replace C7 with the declared matrix

1. Add the explicit publication-shape/handoff/phase/action/position
   specification and per-domain cardinality gates.
2. Seed each row directly at its durable phase; do not trace a fixture to
   decide what to test.
3. Generate only each row's legal physical, causal-parent, proof-only, and
   action-free alphabets. Include both `RestoreParent` phases, both absent
   forms, all six candidate forms, both owners, all five non-status requests,
   no-mutation/evidence/root steps, and non-root rows.
4. Cross every ordinary physical and causal-parent row with no attempt,
   matching `Success`, matching `Failed`, stale/mismatched, consumed-second,
   fresh before/after/ambiguity, and operational-read outcomes exactly as its
   declared relation permits. Pin 20 legal attempt/fact cells per ordinary
   physical row and 20 per causal-parent row; ordinary rows use `After`, while
   causal-parent rows use `AfterNeedsDurability` and never synthesize ordinary
   `After`.
5. Assert exact total/per-target calls, successor/retained bytes, identities,
   journal/recovery disposition, and terminal exhaustion.
6. Retire fixture-derived/deduplicated and synthetic-ambiguity tests once the
   new matrix subsumes their assertions.

### R5 — settled implementation checkpoint

Run:

- all new focused transition, consumer, rollback-prefix, and generated matrix
  tests;
- all preservation, rollback, reverse-lifecycle, and G15 suites;
- `cargo test --all-targets --no-fail-fast`;
- `cargo fmt --all -- --check`;
- `cargo clippy --workspace --all-targets -- -D warnings`;
- protocol regeneration checks;
- merge-document consistency and checker unit tests;
- compatibility-predicate validation and unit tests; and
- `git diff --check` in the workspace and changed members.

Use installed `gwz` for workspace status, staging, and the single settled
implementation commit. Record exact workspace, core, and any other changed
member commits. Exact-tree macOS, Linux x86, and Linux ARM64 release builds
remain platform evidence. Windows additionally runs the full-width identity
and checked-artifact behavioral/fault suite on the exact release runner; a
build alone does not close this remediation.

### R6 — repeat the independent acceptance gate

Return the exact settled commit tuple, both `-3` reviews, and this remediation
plan to two independent reviewers. R4b-G resumes only when both report no open
P0/P1/P2 defect. Any new blocker is consolidated into one reviewed design;
another patch chain is not accepted.

## 5. Path and cohesion manifest

Expected checked-artifact owners:

- `gwz-core/src/checked_artifact/mod.rs` — public internal types and wiring;
- `checked_artifact/identity.rs` — full-width object, rename-domain, and path
  identity;
- `checked_artifact/root_policy.rs` — explicit artifact/private-root policy and
  pre-residue rename-domain proof;
- `checked_artifact/protocol.rs` — bounded canonical private-protocol encoding;
- `checked_artifact/authority.rs` — immutable family/action binding;
- `checked_artifact/observation.rs` — no-follow exact observations;
- `checked_artifact/staging.rs` — authority/goal scratch publication;
- `checked_artifact/residue.rs` — family scan and closed residue facts;
- `checked_artifact/transition.rs` — replace/remove orchestration only;
- `checked_artifact/platform.rs` — handle-relative no-replace and barriers;
- `checked_artifact/fault.rs` — deterministic boundary vocabulary; and
- focused `checked_artifact/tests/` owners split by policy, staging,
  classification, cleanup, restart, and consumer durability.

Expected rollback owners remain the existing abort participant/preflight and
v1 reverse authority/executor modules. One pure aggregate-classification owner
sits beside the rollback observer; authority binding remains in the observer,
and the executor imports only the pure require seam. The domain-specific
checkout overlay remains beside preservation-image comparison. Generated
matrix specification, root preservation rows, and rollback rows are separate
test owners; no single catch-all matrix file is introduced.

The 1,000-line threshold is a cohesion review trigger, not a hard limit. If a
file becomes a concept dump sooner, split it then; split children target below
500 lines. Amend this manifest before adding another responsibility owner.

## 6. Exit criteria

This remediation is accepted only when:

1. every family residue is durably bound to reboot-stable full-width
   artifact-root, retained-parent, operation, exact source/goal,
   canonical-root-relative-path, and original source identities, while the
   invocation-local rename-domain proof is re-established before mutation;
2. a changed parent, same-byte/different-identity source, partial/malformed
   family entry, alias-hidden action, or foreign residue is `Ambiguous` and no
   name is removed; only a provably same-full-identity crash duplicate follows
   its distinct recovery path;
3. a visible staged goal cannot authorize publication until its file and
   namespace durability have been re-established on that invocation;
4. destination publication, source retirement, source cleanup, and authority
   retirement follow the frozen Unix/Windows causal order, and every fault
   boundary restarts to a closed form without repeated managed mutation;
5. quarantine placement is explicit, outside the managed classifier, and in
   the proved same atomic rename domain for workspace, linked-worktree,
   separate-Git-dir, nested-mount, Windows, macOS, and Linux layouts;
6. rollback entry, publication-evidence result, every participant/no-op owner,
   selected-root metadata, recovery resume, execution, and exhaustion are
   guarded by the cursor-relative live aggregate, with every shared-root fact
   assigned to its last unsuperseded owner and every checkout overlay backed by
   an exact alternate observer;
7. action-free mismatches do not rewrite the record, pending-action ambiguity
   is the only rolling-back recovery overlay, and the executor cannot issue or
   receive replayable prefix authority;
8. the declarative C7 matrix covers both absent forms, all six candidate forms,
   all owners/phases/actions/no-op/action-free positions, all five admitted
   requests, the complete executor-diagnostic × fresh-observation relation,
   row-specific physical/causal/proof/error alphabets, and platform boundaries
   through real observers and terminal exhaustion, including exactly 20 legal
   attempt/fact cells per ordinary physical row, exactly 20 per causal-parent
   row, and a distinct source-equals-goal proof-only row with zero physical
   calls and unchanged managed-object identity and family state;
9. all local/static/document/platform gates, including exact-tree Windows
   behavioral durability evidence, pass on one settled tree; and
10. two independent re-reviews report no open P0/P1/P2 defect.

Only then does step 4 of the controlling six-step sequence—R4b-G—begin.
