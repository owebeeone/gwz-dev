# Sentinel review: GwzStashBranchPlan.md

Review target:
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranchPlan.md`

Reviewer: Sentinel (`Bohr`, sub-agent `019efc93-c7a6-76d3-b748-efdfa9cb0d2a`)

## Findings

[P0] Scope gate contradicts authoritative docs before implementation can start

Reference: `Status` intro, "No design or requirements scope change is required";
`Wave 0 - planning lock`

`GWZRequirements.md` still lists selection-wide branch/merge and coordinated
stash as deferred v0 operations. The plan schedules implementation without an
explicit scope promotion or post-v0 framing. That conflicts with the repo rule
that behavior beyond `GWZDesign.md`/`GWZRequirements.md` must update those docs
first.

Fix: Add a Wave 0 scope gate: either update `GWZDesign.md`/`GWZRequirements.md`
to accept these features, or explicitly mark all phases as post-v0/future work
that cannot merge until the authoritative docs are updated.

[P1] B4 protocol work is assigned but not scheduled

Reference: `Multi-agent breakdown schedule`, `Wave 3 - branch commands plus
stash push/list`

Prisma owns "B4 protocol slice" and Curator must wait for it, but no wave
schedules Prisma to add `ActionKind.branch`, `BranchOp`, `BranchRequest`, or
`BranchResponse`. Wave 3 jumps straight to Curator implementing B4.

Fix: Split B4 into `B4a protocol` and `B4b handler/CLI`; schedule Prisma's B4a
before Curator and Lens, with protocol corpus gates.

[P1] Branch merge protocol/result shape is under-specified

Reference: `Resolved branch decisions`, `B5 - Branch merge`,
`Risk and test matrix`

The plan says merge reports `conflicted` and "aggregate `conflicted` or
`partial` as specified by protocol," but no phase defines merge response fields,
conflict path payloads, per-member statuses, or error mapping. B4's protocol
slice only names branch summaries.

Fix: Define merge operation/status/conflict fields in taut before B5, including
aggregate rules for mixed clean/conflicted results.

[P1] Stash pending/partial schema is internally inconsistent

Reference: `Resolved stash decisions`, `S1`, `S4`

The plan defines restore states as `pending/applied/popped/dropped/noop/missing`,
but S4 requires metadata for `completed`, `failed`, and `unattempted` members
after mid-operation failure. There is no schema state for failed push attempts or
planned-but-not-yet-stashed members.

Fix: Add explicit bundle/member lifecycle fields for push intent and failure
recovery, or define how existing fields represent failed/unattempted states.

[P1] Operation locking is not concrete enough for concurrent CLI processes

Reference: `Resolved stash decisions`, `S1`, `Risk and test matrix`

"workspace-level operation lock" may mean the existing in-process operation
runtime, which would not protect two separate `gwz` invocations mutating native
stash stacks and `.gwz/stash`.

Fix: Specify a file-backed lock path, acquisition/release semantics,
stale-lock behavior, and cross-process tests.

[P1] Delete branch dirty-worktree policy is contradictory/untested

Reference: `Resolved branch decisions`, `B4`, `Risk and test matrix`

The decision says branch delete requires a clean worktree, but B4 tests only
reject deleting a checked-out branch. Deleting a non-current ref does not mutate
the worktree, so this is either unnecessary scope or an untested safety policy.

Fix: Either remove delete from the clean-worktree requirement or add explicit
dirty-delete rejection tests and rationale.

[P1] `--force` delete is exposed without semantics

Reference: `B2`, `B4`

B4 exposes `gwz branch --delete <name> [--force]`, while B2 says deleting the
current branch is refused unless a future force policy exists. This invites
CLI/API behavior that the backend refuses or cannot define.

Fix: Defer `--force` from the CLI/protocol, or define exact force semantics and
tests now.

[P2] Multi-agent ownership is still too coarse around shared files

Reference: `Multi-agent breakdown schedule`

Forge owns both S2 and B2 in the same shared file, while Prisma, Curator, Vault,
and Lens all depend on generated protocol churn. The table says ownership is
disjoint, but the wave plan allows overlapping edits in `gitbackend.rs`,
generated protocol, and CLI dispatch files.

Fix: Split shared-file work into serialized sub-slices with branch/commit order,
especially `Forge-Branch` vs `Forge-Stash` and protocol-before-CLI gates.

[P2] Wave gates are not exact enough to execute reliably

Reference: `Gate W1` through `Gate W5`

Several gates say "focused tests pass" or "workflow tests remain green" without
naming commands, test modules, or expected new test files. This weakens the plan
as a multi-agent coordination artifact.

Fix: Add concrete commands per wave, for example protocol check, targeted
`cargo test` module paths, CLI parser test targets, and final full-suite
commands.
