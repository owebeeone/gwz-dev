# GWZ M5-8 R4b-F finalization architecture review

Date: 2026-08-10

Status: **GO; P0: 0, P1: 0, P2: 0, P3: 0**

## Scope and verdict

This was an independent architecture and interface review of the exact current
R4b-F tree. I reviewed acceptance-consuming finalization, candidate and
evidence construction, checked-store/proof/lease ownership, physical-action
rechecks, selected and unselected root semantics, publication-prefix and raw
index classification, v0/v1 isolation, responsibility ownership, and the
settled package accounting.

R4b-X, R4b-P, R4b-G, production dispatch, and A1 activation remain out of
scope. No production code was modified by this review.

**GO.** No P0, P1, P2, or P3 finding remains. The required R4b-F interface is
safe for its declared dependants, subject to their own reviews. The original
950-line ceiling and the intermediate 1,700-line ceiling were both incomplete
estimates of the required exact observer. The now-reviewed 2,100-line,
18-production-path, and 1,800-test-line ceilings are justified and the current
conservative charge is within them.

## Findings and remediation audit

All issues found during this review were corrected before this verdict:

- Accepted inputs are now re-observed exactly before candidate authority.
  Root HEAD/attachment, native state, manifest and lock worktree bytes and
  regular-file types, raw stage-0 index blobs/modes/stages/flags, and selected
  participant results are all checked. A persisted acceptance can no longer
  authorize completion after owned metadata drift.
- After evidence creation, non-root participant results and the frozen
  manifest remain checked. Semantic drift becomes the representable
  `RecoveryRequired` state with literal `Finalizing` origin; operational I/O
  errors are propagated rather than misclassified as ambiguity.
- Detached born roots can complete the unchanged no-publication path without
  invented publication authority. Attached-unborn roots are covered for both
  no-publication and publication-required finalization.
- Candidate validation now checks the complete marker meaning, not merely
  mutually consistent hashes: creation metadata, actor, root input, selected
  and committed targets, members, merge identity, participant before/source/
  result evidence, target kinds and branches, and selected-root evidence.
  Coherent marker substitution cannot receive publication authority.
- Publication-prefix classification closes every filesystem/index combination.
  Mixed raw-index forms, noncanonical index flags, staged-candidate worktree
  divergence, nonregular files, and tampered owned prefixes fail closed without
  another write.
- Marker and boundary parent chains are checked with `symlink_metadata` before
  publication use. Publication rejects an external-write symlink before a ref
  or filesystem mutation, while no-publication paths do not reject or replace
  an unused marker-parent symlink.
- The temporary attempt to add the workspace manifest to parentless evidence
  was rejected as a v0 compatibility and version-isolation violation. The
  settled implementation retains the released exact two-file candidate
  evidence shape, uses `parent = None`, leaves manifest worktree bytes alone,
  and verifies the exact first commit.

## Architecture and interface ownership

### Checked record, proof, store, and lease

The service first uses an unlocked read only for response/rejection fast
paths. Before any mutating lifecycle work, it acquires `V1MutationLease` and
reloads the exact open record under that retained workspace-mutator lock.
Every durable rewrite is produced by the closed transition kernel and
committed through `CheckedV1Store`; the lifecycle has no generic v1 writer,
mutable-record escape hatch, or v0 persistence call.

Observation proofs are bound to the checked record and requested observation.
The dispatcher resolves only those opaque proofs into closed transitions or a
physical action. After a physical action, `reload_unchanged` requires the
record location, bytes, typed value, raw value, unknown-field manifest, and
digest to remain identical before the action can be reconciled.

The same retained lease and checked record reach the physical executor.
Immediately before each Git or filesystem mutation, the executor calls the
exact finalization action verifier again. The verifier must derive the same
single action from current repository, filesystem, and raw-index state; a
stale action therefore cannot cross the mutation boundary.

### Acceptance, candidate, and evidence

The finalizer consumes only persisted `AcceptedWorkspaceV1`. It never rebuilds
acceptance from a mutable v0 view. Candidate construction is separately gated
on `Finalizing / PreparingCandidate / no candidate`, exact accepted root HEAD,
and the persisted publication branch.

The candidate freezes exact accepted lock bytes, exact baseline and candidate
boundary bytes, a complete additive marker, candidate digests, marker path,
actor, and evidence base. It is durably persisted before evidence intent.
Evidence intent is durably persisted before `commit_gwz_paths_checked` can run.
That Git primitive uses an isolated index, exact parent or parentless base,
exact message and candidate files, checked attached-ref transaction, and
post-publication verification; it does not adopt the real index or unrelated
worktree content.

Evidence result records the exact commit, tree, ordered path hashes, and
selected-root result. Restart verifies the same parent, message, tree, files,
hashes, branch and ref before proceeding. Evidence creation, marker write,
lock write, boundary write, and index stage each have a persisted owner before
mutation.

### Root semantics and user-state preservation

- A selected root is accepted only as `BornAttached` at its exact successful
  result and target branch. Manifest and lock bytes come from that result
  commit, and the composition evidence commit uses the result as its parent.
- An unselected root freezes the exact operation checkout as `BornAttached`,
  `BornDetached`, or `UnbornAttached`. Detached is legal only when the frozen
  result requires no publication. Attached-unborn publication uses the exact
  symbolic branch, no fabricated parent, and the released parentless two-file
  evidence shape.
- Unrelated staged, dirty, untracked, index, worktree, and exclude-file state
  is not imported into the evidence commit and is not overwritten. Raw-index
  checks deliberately constrain only GWZ-owned paths; independent filesystem
  checks constrain their worktree bytes and types.

### Publication prefix and shared abstractions

The publication observer separates three domains:

1. exact accepted participant/root inputs;
2. exact recorded evidence; and
3. the live filesystem/raw-index publication prefix.

The shared publication classifier recognizes only baseline, marker, lock, and
boundary prefixes. A separate pure resolver maps each legal prefix/index form
to exactly one successor, including lock-equal and boundary-equal degenerate
cases. The raw-index primitive rejects duplicate entries, non-stage-0 entries,
non-regular modes, wrong blobs, assume-valid/extended flags, and unexpected
owned entries without consulting worktree bytes. Worktree alignment is checked
separately for the staged terminal form.

The shared marker adapter retains v0's released fail-closed state and drift
checks. The v1 acceptance reconstruction is lifecycle-state-independent only
because candidate creation separately proves the `Finalizing` predecessor and
later validation must reconstruct the same frozen historical marker after the
record advances. No v1-to-v0 owned projection or v0 mutator is introduced.

## Version and activation isolation

`merge::v1_lifecycle`, `model::v1`, `acceptance::v1`, and the v1 candidate
adapter remain behind `#[cfg(test)]`. A normal library build does not compile
the v1 lifecycle, writer, upgrader, or dispatch path. Production-only
`cargo check -p gwz-core --lib` passes.

Searches of the lifecycle and candidate owners find no `v0_common_view`,
`persist_merge_record`, `persist_operation_transition`, `write_v1`,
`persist_v1`, or `update_v1`. The narrow shared changes are generic marker,
candidate classification, raw-index, boundary, and module adapters; the v0
marker and v0 characterization suites remain green.

## Ownership and budget

The final ceiling revision is justified. The 950-line estimate treated F as a
thin candidate consumer. The 1,700-line revision added the exact live observer
but still omitted accepted-root/index verification, complete candidate/marker
semantics, operational-error separation, parent-chain safety, the complete
prefix matrix, and the third real-Git test owner. Those are mandatory pieces
of the accepted interface, not new feature scope. Reusing the mutable v0
finalizer would break isolation, while combining them would cross the
sub-500-line responsibility limit.

The ledger arithmetic is reproducible and conservative:

| Measure | Current charge | Ceiling | Margin |
| --- | ---: | ---: | ---: |
| Production-bearing Rust | 1,989 | 2,100 | 111 |
| Focused test/tool/doc Rust | 1,681 | 1,800 | 119 |
| Production paths | 17 | 18 | 1 |

Production is `1,425 + 395 + 169 = 1,989`: 1,425 current lines in the six
dedicated owners (`65 + 64 + 463 + 457 + 170 + 206`), the previously charged
395-line shared adapter surface, and a conservative 169-line charge for the
remediated candidate semantic validator. The latter whole-function charge is
conservative where some validation existed before remediation.

The 17 production paths charge all shared files to F: the six dedicated
owners; marker; publication classifier/validator and acceptance module; three
Git backend paths; boundary helper; lifecycle module; authority observer;
authority re-export; and merge-module wiring. This closes the earlier
13-versus-actual-path accounting gap.

Focused tests are `1,254 + 239 + 65 + 77 + 46 = 1,681`: three real-Git owners,
local candidate/live/observer tests, raw-index regression, publication
validator regression, and acceptance fixtures. The three primary test owners
are 423, 439, and 392 lines.

Every new or policy-bearing responsibility owner remains below 500 lines. The
tightest current owners are publication observation at 477, finalization
observation at 463, marker at 459, publication classification/validation at
434, and finalization-input tests at 439. Existing `GitBackend` contract wiring
is the permitted narrow addition to a pre-existing owner above 500 lines; it
does not acquire lifecycle policy.

These margins are review ceilings, not room for R4b-X/P/G work. Any dependent
package that adds to these paths must charge its own checkpoint and preserve
the accepted owner boundaries.

## Verification

The exact reviewed tree passed:

- v1 lifecycle: 101/101;
- v1 publication validator: 5/5;
- scoped Git/raw-index G13: 7/7;
- v0 marker adapter: 3/3;
- exact v0 changed-window characterization;
- full `gwz-core` library: 907 passed, 1 ignored;
- integration targets: 10/10, 27/27, 4/4, and 2/2;
- production-only `cargo check -p gwz-core --lib`;
- strict `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`;
- `cargo fmt --all -- --check`;
- root and `gwz-core` `git diff --check`;
- merge-document consistency: 8 sources and 87 assertions; and
- compatibility predicates: seven migration rules and seven runtime bindings.

The lead also reports `cargo doc -p gwz-core --no-deps` passing with four
unrelated pre-existing Rustdoc warnings and the merge checker unit suite
passing 17/17.

## Final disposition

**GO. P0: 0; P1: 0; P2: 0; P3: 0.** R4b-F now has one checked,
acceptance-consuming finalization architecture; exact proof/store/lease and
physical-recheck boundaries; closed born/detached/unborn and publication-prefix
semantics; preserved v0 behavior; no normal-build activation; and reviewed,
within-ceiling ownership.
