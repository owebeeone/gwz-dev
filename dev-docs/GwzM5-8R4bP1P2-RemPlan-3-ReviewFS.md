# R4b-P third remediation filesystem-interface review

Date: 2026-08-12

Reviewed:

- `GwzM5-8R4bP1P2-RemPlan-3.md`, especially sections 3.1-3.4, R1/R2,
  the path manifest, and exit criteria 1-5;
- the exact settled findings in `GwzM5-8R4bP1P2-ReviewArch-3.md` and
  `GwzM5-8R4bP1P2-ReviewFaults-3.md`;
- the controlling `GwzM5-8R4bP1P2-RemPlan-2.md`; and
- the settled checked-artifact implementation in
  `gwz-core/src/checked_artifact/`, its current callers, the root-preservation
  Windows publication primitive, and the identity implementation supplied by
  `cap-fs-ext` 4.0.2.

This was a read-only architecture review. I did not modify the remediation
plan or production/test code and did not coordinate conclusions with the
rollback/C7 interface reviewer.

## Verdict

**NO-GO for R1 implementation as currently frozen.**

The authority-record/staged-goal direction is the right correction, and it can
be implemented without adding a v1 action, lifecycle phase, or wire field.
However, two P1 durability/identity promises and two P2 namespace/placement
contracts are not yet sufficiently defined to be implemented safely on every
claimed release platform. They need to be folded into sections 3.1-3.4 before
production edits begin.

## Findings

### [P1-1] The frozen identity type is not strong enough to make cleanup exact on every supported Windows filesystem

**Where:** RemPlan-3 lines 84-103, 129-134, 177-190, 392-402;
`gwz-core/src/checked_artifact/observation.rs:193-195`; and
`cap-fs-ext-4.0.2/src/metadata_ext.rs:15-21`.

**Defect.** The plan persists an unqualified `identity` for the artifact root,
retained parent, source, and staged goal, but does not freeze its platform
representation or the condition under which an identity is unavailable. The
settled implementation's identity is `(MetadataExt::dev(),
MetadataExt::ino())`. `cap-fs-ext` explicitly documents that its Windows inode
value does not represent ReFS's 128-bit file identifier. Encoding that existing
pair more carefully would therefore still truncate a supported filesystem's
object identity.

The defect matters at the destructive edge, not merely in diagnostics. A
same-byte replacement whose truncated identity collides with the retained
source can pass the proposed cleanup proof and be removed as though it were
the quarantined original. A build of the Windows target cannot establish the
required identity semantics.

**Required correction.** Freeze a typed, encoded `FilesystemObjectIdentity`
contract. On Windows it must include a volume identity and the full file ID
returned from an opened handle (including a 128-bit ID where the filesystem
provides one); on Unix it may use the opened-object device/inode pair. The
policy constructor must fail with a typed, pre-mutation unsupported/recovery
result when a stable full-width identity cannot be obtained. The authority,
family/residue names, and cleanup checks must all use that same type. Add a
Windows compile/test seam that exercises a full-width synthetic ID and pins
its lossless encoding, plus exact-tree behavioral coverage on the actual
Windows release runner. Do not claim all Windows layouts by target build
alone.

### [P1-2] “Parent barrier” is not a frozen cross-platform durability primitive, and the publication/retirement order can still lose the only durable source

**Where:** RemPlan-3 lines 105-148, 177-190, 282-305, 396-402;
`gwz-core/src/checked_artifact/platform.rs:126-133`; and
`gwz-core/src/git/gitbackend/preservation_root/parent.rs:291-348`.

**Defect.** Sections 3.2-3.4 repeatedly require the quarantine or managed
parent to be “synced,” but the settled Windows `checked_artifact::sync_parent`
is deliberately a no-op because a normal directory handle cannot be flushed
portably. The separate root-preservation implementation uses write-through
rename operations as a Windows-specific barrier instead. RemPlan-3 does not
choose or specify an equivalent Windows protocol, yet R5 treats an exact-tree
build as platform evidence.

The ordering is also incomplete for cross-directory moves. Source detach
correctly names both parent barriers, but managed-goal publication names only
the managed-parent barrier. The goal move removes a name from quarantine and
adds one to the managed parent; both namespace effects and their order matter
before source cleanup and authority retirement. Without an enforceable
destination-first publication barrier followed by an enforceable quarantine
barrier, a power failure can persist source/authority cleanup while losing or
reverting the managed-goal publication. The claimed closed `After` form then
has no authoritative source from which to recover. A less severe ordering can
also produce a same-identity duplicate staged/managed goal, which the plan
currently classifies as foreign rather than as a crash-produced form.

**Required correction.** Define the platform contract and ordering explicitly,
not as a generic call named `sync_parent`:

1. a flushed authority/goal file;
2. atomic no-replace publication with a supported durability operation;
3. for each cross-directory rename, durable destination publication before
   durable source-name retirement, with both directory effects covered;
4. durable managed goal/absence before quarantined-source cleanup; and
5. durable source cleanup before authority retirement.

Specify the Windows write-through/rename or other supported primitives for
publication, removal, and retirement, and return to design review if Windows
cannot provide the stated guarantee. Add before/after faults for each side of
each cross-directory move and cleanup ordering. The classifier must either
recognize a provably same-identity duplicate created by an interrupted durable
move and close it safely, or the platform protocol must prove that form cannot
occur. A Windows release build by itself is not behavioral durability
evidence.

### [P2-1] A typed artifact-root policy does not by itself prove same-mount atomic rename

**Where:** RemPlan-3 lines 48-76, 284-306, 400-402.

**Defect.** `artifact_root/.gwz/checked-artifacts` and
`retained_git_directory/gwz/checked-artifacts` express ownership, but their
path relationship does not prove that the opened managed parent and opened
private parent support an atomic cross-directory rename. A workspace may
contain a nested mount at the managed parent; `.gwz` itself may be a mount
point; and on Unix a distinct/bind mount can defeat rename even when a simple
device-number comparison appears compatible. No-follow traversal prevents
symlink substitution, not mount crossing. Discovering `EXDEV` only at source
detach is too late because the plan has already published authority and a
staged goal, contrary to line 65's pre-residue promise.

**Required correction.** Make the policy constructor retain the actual
artifact parent and quarantine handles and perform a platform-specific
same-rename-domain proof before authority or family residue publication. Use
mount/volume identity where that is sufficient, and a harmless private probe
or a typed unsupported result where it is not. Bind the proved rename-domain
identity into the authority. Test a managed parent below a nested mount as
well as linked-worktree and separate-Git-dir placement; the mandatory test on
every host must assert the identity of the two opened rename domains, not just
their lexical roots. An `EXDEV` or equivalent remains no-copy/no-delete and
must occur before family state is published.

### [P2-2] The authority namespace lacks one unambiguous, filesystem-equivalent discovery key for a managed leaf

**Where:** RemPlan-3 lines 84-103, 114-119, 129-139, 152-169, 392-395.

**Defect.** The proposed `family_key` binds operation, expected value, and
goal, making it action-specific. The next sentence nevertheless says that a
different parent-bound key cannot hide residue, even though retained-parent
identity is stated to live only in the authority. The plan also requires a
“canonical relative path” but defines only lossless component encoding, not
filesystem path equivalence. On case-insensitive or normalization-insensitive
Windows/macOS volumes, two losslessly different component sequences can name
the same managed leaf.

As written, an implementation can scan only the current action key and fail
to see an authority/source/goal for the same leaf under a different operation,
expected/goal tuple, parent-bound suffix, case spelling, or normalization.
That is the same class of hidden-residue error that allowed a replacement
parent to adopt old staging in the settled P1 finding.

**Required correction.** Freeze two different concepts:

- a stable **artifact-family key**, derived from the retained artifact-root
  identity plus the platform-equivalent canonical managed path, which owns
  every authority/residue for that leaf; and
- an **action key**, derived from that family plus operation and exact
  expected/goal facts.

Every acquire/classify/execute call must enumerate the complete artifact
family before publishing an action authority. Exactly one matching authority
and its bound residues may proceed; another action, duplicate, malformed
entry, or parent/source mismatch is `Ambiguous`. Define how the canonical path
is obtained on case-sensitive, case-insensitive, and normalization-insensitive
filesystems rather than equating raw lossless encoding with canonical
identity. Add case-alias/normalization tests where the host filesystem supports
them and an injectable path-equivalence seam elsewhere.

### [P3-1] The private recovery protocol and inert scratch lifetime need explicit ownership rules

**Where:** RemPlan-3 lines 80-119, 121-139, 363-377.

`CheckedArtifactAuthorityV1` is not a public v1 wire change, but it is still a
durable on-disk recovery protocol that can survive a process restart and a GWZ
upgrade. Freeze a bounded canonical encoding, schema/unknown-version behavior,
and maximum sizes. Unsupported or malformed family records should remain
`Ambiguous`; they must not be silently treated as another schema.

The plan also says unpublished scratch files are never deleted by a later
action. That is the safest immediate rule, but it leaks complete goal bytes
and authority data without bound after repeated crashes. State explicitly
that correctness never depends on scratch GC, then either define a separate
lease-held, identity-checked GC protocol for recognizable scratch files or
record the storage/privacy leak as deferred pre-A1 work. Foreign private-root
entries must never be collected merely because their names resemble scratch.

### [P3-2] Exact conditional cleanup needs a stated linearization/concurrency boundary

**Where:** RemPlan-3 lines 177-190 and the proposed
`checked_artifact/platform.rs` owner.

The promise “not `remove_file(name)`” is an outcome, not yet an implementable
primitive. A pathname identity check followed by ordinary unlink has a
substitution window. Define whether the private namespace is exclusively
mutable by the workspace lease and protected as GWZ-private state, or specify
the handle-relative rename/verify/restore/delete sequence that gives cleanup
its linearization point. Add a deterministic hook immediately before that
point and substitute a same-byte, different-identity entry. The operation must
retain every name and return `Ambiguous`. The design should not imply an
impossible atomic compare-and-unlink against arbitrary uncooperative writers.

## Wire-model assessment

No v1 wire change is required for this remediation. The existing durable
action already supplies the operation and exact expected/goal facts; root
policy is deterministic from the action's artifact owner; physical phase can
be inferred from the immutable authority and closed residue set. The private
authority therefore can remain below the wire model.

That conclusion depends on the revised design making the private authority a
versioned restart protocol and deriving all of its state from the existing
action plus live handle facts. If implementation needs to persist a new
lifecycle decision that cannot be reconstructed from those inputs, R1's stop
condition should apply and the wire design must be revisited.

## Exit decision

Fold P1-1 through P2-2 into sections 3.1-3.4, update the Windows/platform stop
condition and evidence requirements, and repeat this interface review before
editing production code. P3-1/P3-2 should be resolved or explicitly assigned
to the pre-A1 backlog. With those changes, the proposed authority/staging
architecture is implementable without a v1 wire change and is a sound basis
for R1/R2.
