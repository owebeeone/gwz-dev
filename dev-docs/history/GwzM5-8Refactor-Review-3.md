# GWZ M5–M8 Merge-Lifecycle Refactor Proposal — Re-review 3

Date: 2026-07-28

Reviewed:

- `dev-docs/GwzM5-8Refactor.md`, revision dated 2026-07-28 with SHA-256
  `9be3c65fb74f5553f1615a29405b95bc944b4bad9dff0ea2d98b9cd0566f8725`;
- `dev-docs/GwzM5-8Refactor-Review.md`;
- `dev-docs/GwzM5-8Refactor-Review-2.md`;
- the accepted M2b/M4 behavior in `gwz-core/dev-docs/GwzMergePlan.md`; and
- the current merge baseline, finalization, root-finalization, and record-store
  implementation.

## Verdict

**The immediate approval remains valid:** proceed with R0, R1, R2a, and M5.
The revision incorporates all four findings from the second review.

**More issues remain at the broader record/finalization boundary.** One P1
finding blocks I2/R4 because the proposed accepted-root type cannot represent
an already released M4 finalization case. One P2 finding closes a
no-publication path around the new accepted-workspace invariant. The P3
compatibility clarification should be added to the R3 tests.

None of these findings requires speculative M6/M8 fields in R2a or delays M5.

## Disposition of review 2

| Review-2 finding | Re-review result |
| --- | --- |
| P2-1: additive v0 lacked an old-reader safety gate | **Resolved.** §15.1 now requires actual supported previous-release executables, enumerates behavioral equivalence, and mandates a version change on any semantic difference. |
| P2-2: persisted versus derived `AcceptedWorkspace` was undecided | **Resolved for publication-bearing paths.** §9 now makes the exact accepted lock bytes/hash and member audit immutable persisted evidence before publication. The separate no-publication path is addressed below. |
| P3-1: post-rename migration verification was ambiguous | **Resolved.** §15.5 identifies the validated staged file plus atomic rename/directory fsync as the durable boundary and makes the hash explicitly ephemeral. |
| P3-2: snapshot ID was not immutable provenance | **Resolved.** §§6 and 12 freeze the exact artifact-byte hash, schema, workspace identity, snapshot ID, and per-participant execution commits. |

## Findings

### [P1-1] `RootPublicationInput` cannot represent an unborn accepted root

**Where:** §9 lines 638–642; R4; §22.3.

The illustrative accepted-root shape requires:

```rust
struct RootPublicationInput {
    accepted_integration_base: String,
    publication_branch: String,
    baseline_artifact_hashes: RootArtifactHashes,
}
```

That requires every publication to have a parent commit. Released M4
deliberately supports a different valid state: the workspace root is attached
to a symbolic branch but has no commit yet.

This is not a hypothetical M6 root-`--into` case:

- `MergeBaseline.root_head` is `Option<String>`;
- current member-only finalization obtains an
  `evidence_parent: Option<String>`;
- `commit_gwz_paths_checked` accepts `expected_head = None` and creates the
  root's first evidence commit; and
- the accepted M2b gate explicitly covers changed member merges, interruption,
  evidence rollback, and finalization on both born and unborn roots.

For a member-only merge in an unborn workspace root,
`accepted_integration_base` must therefore be “unborn,” not a fabricated commit
string. Fabricating a value would break the checked root-parent precondition;
rejecting the state would regress released M4 behavior.

**Required correction before I2/R4:**

Represent root base state explicitly, for example:

```rust
enum AcceptedRootBase {
    Born { commit: String },
    Unborn { symbolic_branch: String },
}

struct RootPublicationInput {
    base: AcceptedRootBase,
    publication_branch: String,
    baseline_artifact_hashes: RootArtifactHashes,
}
```

The exact shape can differ, but it must not encode unborn state with an empty,
sentinel, or synthetic object ID.

The accepted-workspace validator must prove:

1. `Born.commit` equals the durable accepted root result when `@root`
   participated, or the frozen baseline root head when it did not;
2. `Unborn` is legal only when the frozen baseline and accepted live
   precondition both identify the exact attached unborn branch;
3. an explicitly selected `@root` cannot become an unborn publication base
   after a successful integration;
4. the publication branch agrees with the accepted symbolic branch; and
5. restart and abort preserve the existing checked first-commit behavior.

Add real-Git coverage for:

- a changed member finalized from an unborn root;
- a fault immediately after `AcceptedWorkspace` persistence on an unborn root;
- restart through first evidence-commit creation and recording;
- abort before and after that evidence record; and
- drift where the formerly unborn root becomes born before publication.

This finding does not affect M5 if R2a continues to use the current M4
finalization model unchanged. It blocks freezing the broader accepted-workspace
record until corrected.

### [P2-1] The accepted-workspace freeze must also precede terminal no-publication completion

**Where:** §4.3 lines 175–181; §9 lines 601–603 and 685–700; R4; §22.4.

The proposal strongly says finalization consumes the persisted
`AcceptedWorkspace`, but most concrete timing rules only require persistence
“before the first candidate or publication mutation.”

Current M4 has an intentional path that performs no candidate or publication
mutation: after participant verification, an all-up-to-date operation marks
publication complete and archives immediately. M8 can add another important
case in which publication may be unnecessary: every optional selected target
is validly skipped and the complete accepted lock is unchanged.

If R4 preserves the current short circuit literally, the “before publication”
rule is vacuous and the operation can complete without ever freezing the
accepted workspace. That would create two finalization contracts:

- changed operations have immutable accepted lock/root evidence; and
- no-change operations infer success directly from participant state.

The latter contradicts the stated containment boundary and makes complete lock
and audit validation optional precisely on all-no-op or all-skipped results.

**Required correction before R4:**

- freeze `AcceptedWorkspace` after complete participant/outcome validation and
  before deciding whether candidate publication is necessary;
- require every successfully completed operation, including all-up-to-date and
  all-validly-skipped operations, to archive the exact accepted workspace; and
- make “publication not required” a derived decision from that accepted
  workspace, not a bypass around it.

Add pure and real-Git tests for all-up-to-date member-only, explicit-root,
mixed, born-root, and unborn-root operations. When M8 lands, add the equivalent
all-skipped and mixed skipped/up-to-date cases. Fault injection should cover
the accepted-workspace record write followed by the no-publication terminal
transition.

### [P3-1] Preserve migrated unknown fields through subsequent current-version rewrites

**Where:** §§15.2–15.5; R3; §18.

The proposal now specifies lossless unknown-field transfer during v0-to-v1
migration and says the decoder retains raw/unknown representation needed for a
lossless rewrite. It does not explicitly test the next step: the migrated v1
record being rewritten repeatedly by continue, abort, preservation, or
publication progress.

The current v0 store carries surviving unknown nested fields from the old raw
record into every replacement write. Migration must not preserve an extension
only during the initial version change and then silently drop it on the first
ordinary v1 lifecycle update.

Add an explicit R3 invariant:

- every rewrite of a current-version record recursively preserves unknown
  fields whose containing semantic object survives;
- this applies both to extensions migrated from v0 and fields written by a
  future/newer writer; and
- when a lifecycle transition legitimately removes the containing object, the
  compatibility contract states whether its nested extensions disappear with
  it or require another durable destination.

Test one migrated open record with unknown fields in the operation, baseline,
participant, pending action, prepared commit, preservation, publication,
candidate, and drift containers. Drive it through multiple legal v1 rewrites
and assert the unknown-field manifest after each write, not only immediately
after migration.

## Additional confirmation

The rest of the revised proposal remains coherent:

- M5 is isolated from checkout, participation, and snapshot dimensions.
- The no-delete branch default remains the safe ownership policy.
- Forward, reverse, preservation, and publication mutations have distinct
  durable owners.
- Participation authorization and runtime outcome remain separate.
- Exact accepted lock YAML is authoritative and is not regenerated after
  restart.
- Root integration input remains separate from publication-created output.
- Envelope-first decoding, schema-pair dispatch, and old-reader gating are
  suitable I2 prerequisites.
- Snapshot execution uses frozen commits while provenance records the exact
  artifact identity/hash.
- The module split is responsibility-based and does not require mechanical
  fragmentation.

## Recommended disposition

1. Keep the existing approval for R0, R1, R2a, and M5.
2. Correct P1-1 in the illustrative accepted-workspace model and I2 criteria
   before the root contract is frozen.
3. Add P2-1 to R4's ordering and acceptance tests.
4. Add P3-1 to the R3 compatibility invariant and fixture matrix.
5. Re-review the actual I1/I2 frozen contracts, as the proposal already
   requires, before R3 begins.

The proposal's phased architecture remains sound. These findings close current
M4 edge cases at the future durable boundary rather than changing the approved
next implementation package.
