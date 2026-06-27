# Review 48: GwzStashBranchPlan.md

Review target:
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranchPlan.md`

Reviewer: Claude Code (Opus 4.8). Method: code-grounded review via four parallel
sub-reviewers (code-accuracy, stash soundness, branch soundness,
design-coherence/process), each verifying the plan's factual claims against the
actual `gwz-core`/`gwz-cli` source and the authoritative docs.

This review **complements** `GwzStashBranch-Review55.md` (Sentinel), which focused
on internal consistency. The value added here is grounding: every finding cites
`file:line`. Where a finding confirms a Review55 item it is marked
**(confirms R55)** and adds the code evidence R55 lacked.

## Verdict

The plan is unusually well-grounded — nearly every touchpoint file, primitive,
and test target I checked exists and is characterized correctly, and the central
designs (native-stash-oid identity, routing `--switch` through
`MaterializeTargetKind.branch`, conflict-as-normal-state) are implementable on
the real `git2 0.21` API and the existing lock-rewrite machinery. It is **not yet
executable as a multi-agent plan**, for five reasons that recur across the
findings: (1) a false scope gate against `GWZRequirements.md`; (2) several
"reuse" claims that are actually **new infrastructure** (cross-process lock,
`.gwz/` boundary handling, a `.yml`/newest-first list helper, a named-ref→artifact
builder); (3) a backend mismatch on `merge_upstream` that constrains B5; (4) an
overlap with the existing `checkout_branch` primitive; and (5) protocol slices
(B4, merge wire shape) that are owned but unscheduled. None are fatal; all are
specification gaps to close before the corresponding phase lands.

## Findings

### [P0] The scope-gate claim is false — `GWZRequirements.md` still mandates these ops be rejected/hidden in v0 **(confirms R55)**

Reference: Status intro, lines 19-23 ("No design or requirements scope change is
required ... both selection-wide branch/merge and coordinated stash are already
deferred/future operations").

"Deferred" in `GWZRequirements.md` means **out of v0 scope and gated**, not
"pre-approved to build." REQ-002 lists `selection-wide branch`, `selection-wide
merge`, and `coordinated stash bundles` as deferred, and mandates: "Deferred
operations MUST either be rejected with typed unsupported-operation errors or
hidden from the v0 action surface" (`GWZRequirements.md:104-119`). REQ-088/REQ-089
say branch/merge "SHOULD be supported **after** v0" (`GWZRequirements.md:556,564`).
`GWZDesign.md` Design Deferrals (`:1502-1511`) still lists selection-wide branch
and merge. So the authoritative baseline currently *requires* these to be
rejected; building them is a scope change that must update the docs first.

Fix: Add a Wave-0 scope-promotion gate that edits `GWZRequirements.md` (move these
out of REQ-002 deferred into a scoped post-v0 section with REQ ids and
test-traceability per REQ-153) and the `GWZDesign.md` Design Deferrals list,
**before** any Wave-1 source edit. Alternatively mark every phase
post-v0/cannot-merge-until-docs-updated.

### [P1] Stash bundles under `.gwz/` contradict the codebase's actual artifact convention (`gwz.conf/`), and the versioned/unversioned split is undesigned

Reference: S1, lines 116-118 ("Store bundle files under
`.gwz/stash/bundles/<stash_id>.yml`, not under `gwz.conf/`").

Every versioned workspace artifact the code actually writes lives under
`gwz.conf/`: lock (`artifact/mod.rs:13` `LOCK_PATH="gwz.conf/gwz.lock.yml"`),
snapshots (`artifact/mod.rs:14` `SNAPSHOT_DIR="gwz.conf/snapshots"`), tags, and
the manifest (`workspace/mod.rs:7`). **Nothing in the current code ever writes to
`.gwz/`** — `RUNTIME_DIR=".gwz"` (`workspace/mod.rs:8`) exists only as a
reserved-path check (`workspace/mod.rs:41`). The plan justifies `.gwz/` by citing
`GWZDesign.md`, but that doc's layout section is itself **stale**: it places
snapshots under `.gwz/snapshots/` (`GWZDesign.md:207,356`) and the manifest under
`workspace/` (`:354`), both of which the code already overrode in favour of
`gwz.conf/`. So stash bundles would be the *first* real `.gwz/` artifact and would
be unversioned while structurally-parallel snapshots are versioned — a real
design decision (is stash metadata committed/shared?) made implicitly by file
placement, with no rationale. (Independently flagged by three of four reviewers.)

Fix: State explicitly whether stash bundles are versioned workspace metadata
(then put them under `gwz.conf/stash/` alongside snapshots/tags) or intentionally
local-only/disposable (then keep `.gwz/` and document non-portability + how
reconciliation survives a `.gwz/` wipe). Either way, add an erratum that
`GWZDesign.md`'s directory layout predates the `workspace/`→`gwz.conf/` rename and
must be reconciled before being cited as authority.

### [P1] `.gwz/` is never added to the root's managed `.git/info/exclude`, so every `gwz stash push` dirties the workspace root the plan claims not to touch

Reference: S1 storage; Resolved stash decisions / Root repo participation.

The root git boundary is maintained by `ensure_workspace_exclude`, which writes
only `/gwz.conf/.tmp/` and each member path into `.git/info/exclude`
(`sync_workspace_boundary.rs:33-40`). `.gwz/` is **not** in that managed block and
is excluded nowhere. So writing `<root>/.gwz/stash/bundles/...` makes the **root**
repo report `.gwz/` as untracked. Because several flows in this same plan reject
on, or surface, a dirty root, a stash operation will visibly dirty the root it
claims to leave untouched, and can self-block subsequent root-sensitive
operations. (This also already applies latently to any future `.gwz/` runtime
writes.)

Fix: In S1, add `/.gwz/` (or `/{RUNTIME_DIR}/`) to the managed exclude block (or a
dedicated runtime-dir boundary step) and add a test asserting the root stays clean
after `gwz stash push`. If the storage moves to `gwz.conf/stash/` per the prior
finding, this is moot — another reason to prefer `gwz.conf/`.

### [P1] S1's "reuse artifact helpers" breaks on the `.yml` extension and on ordering

Reference: S1 ("Use atomic write helpers/patterns from `artifact/mod.rs`"; "List
returns newest first by `created_at`"); S5 ("Load registry bundles first").

The only existing list helper, `list_artifacts`, filters strictly for the
`.yaml` extension and sorts by **file path ascending** (`artifact/mod.rs:295-310`).
The plan files bundles as `<stash_id>.yml` and wants **newest-first by
`created_at`**. Reusing `list_artifacts` would silently return zero bundles (wrong
extension) and, even fixed, produce oldest-first lexical order — the opposite of
the requirement. Existing artifacts use `.yaml` (`artifact/mod.rs:401`), so `.yml`
is also inconsistent with convention.

Fix: Standardize bundles on `.yaml` to match the convention, or add a dedicated
stash-list helper that accepts `.yml` and sorts by `created_at` descending. Add a
test that a `.yml` bundle is actually discovered and ordered by `created_at`, not
filename.

### [P1] No cross-process lock primitive exists to reuse — the "workspace-level operation lock" is new infrastructure **(confirms R55)**

Reference: Resolved stash decisions / Operation locking; S1 touchpoint ("Add a
workspace-level operation lock in `src/operation/`"); Risk matrix.

The plan points S1's lock at `src/operation/` as if a reusable workspace lock
exists. The only lock there is `MemberLockManager`, a
`std::sync::Mutex<HashSet<String>>` guarding member ids **within one process**
(`operation/membermutationguard.rs`); the other locks (`par_map_per_host`,
`operation_runtime`) are also in-process `std::Mutex`. There is **no
file-backed/advisory/flock primitive anywhere** in core (no `fs2`/`flock` in
`Cargo.toml`). Two separate `gwz stash` OS processes would mutate the same native
stash stack and `.gwz/stash` registry with zero mutual exclusion, and the S1 test
("Concurrent stash mutations cannot both acquire the workspace-level lock") is
satisfiable in-process — giving false confidence. (Independently flagged by three
reviewers.)

Fix: Specify a **new** file-backed lock (exclusive lock file under
`gwz.conf/`/`.gwz/`, with acquire/release + stale-lock policy) and a real
cross-process test (two processes contending on the lockfile). Make S1 clear this
is new infrastructure, not an extension of `MemberLockManager`. **Also decide
whether branch mutations need the same lock** (see branch TOCTOU finding) — the
plan currently gives stash a lock but branch ops none.

### [P1] `merge_upstream` merges into the **current** worktree HEAD — B5's "merge into the current attached target branch" is only safe when `target == HEAD`, and `--into` cannot be honored as written

Reference: B5 - Branch merge; Resolved branch decisions (Merge conflicts).

The existing primitive `merge_upstream(path, branch, upstream_ref)` operates on
the repo's currently-checked-out worktree: the clean path commits onto `HEAD` and
self-verifies `head(path) == merge_oid`, the conflict path writes `MERGE_HEAD` and
leaves the live index conflicted, and the `branch` argument is used **only** for
the fast-forward ref update and the commit message — it does **not** check out
`branch` first (`gitbackend.rs:406-481`, FF at `:388-398`, conflict at `:429-448`).
So B5's preflight option "explicit target branch exists" (a target that need not
be the current HEAD) cannot be honored, and `--into <target>` requires a prior
switch the plan defers.

Fix: Restrict v0 B5 to merging into the **current attached branch only** (assert
each member's observed `head.branch == target` before calling `merge_upstream`;
reject detached/mismatched). Defer `--into` and the "explicit target branch
exists" preflight to the later step that first runs B3 `switch_branch`. Document
that `merge_upstream`'s `branch` arg must equal current HEAD.

### [P1] `snapshot --branch <name>` cannot reuse `observed_member_map` — B3 omits the named-ref→artifact builder

Reference: B3; Snapshot-from-branch semantics.

The existing snapshot handler builds members via `observed_member_map`, which
reads `backend.head()`/`backend.status()` of the **live worktree**
(`handle_materialize.rs:401-434`) — i.e. the current branch/commit/dirty, not a
named ref. There is no helper that resolves `refs/heads/<name>` into a
`ResolvedMemberArtifact` (`branch=<name>`, `commit=read_ref(...)`,
`detached=false`, `dirty=false`). B3's touchpoints list only
`handle_materialize.rs`/`materialize_preflight.rs`/CLI files and never route the
named-branch source through `handle_snapshot`. The named-branch artifact must also
**not** carry the worktree's dirty flag.

Fix: Add an explicit work item/touchpoint: a named-branch member builder using
`backend.read_ref(refs/heads/<name>)` (`detached=false`, `dirty=false`), wired
through `handle_snapshot`. Add a test that the named-branch snapshot ignores
worktree dirtiness and records the ref commit.

### [P1] "Reject mixed branch names" is undefined for detached/unborn HEAD and has no error taxonomy

Reference: Snapshot-from-branch semantics; B3 tests; Risk matrix.

The decision conflates two distinct rejection reasons — detached/unattached HEAD
vs attached-but-differing names — into one phrase, with no precedence or error
code for each, and says nothing about a materialized-but-**unborn** member (HEAD
on a branch, `commit=None`). `GitHeadState { branch, commit, is_detached }`
(`gitbackend.rs:253-258`) distinguishes these states, but the plan never maps them
to typed errors, so Switchboard and Lens will implement divergent rejection logic.
"Mixed" is also ambiguous for a single-member selection.

Fix: Define typed errors in B1/B3 — e.g. `DetachedHead`/`UnbornHead` distinct from
`MixedBranches` — checked in preflight across all selected members; define the
single-member case as "one shared branch name = trivially satisfied." Add tests
for unborn-HEAD and detached-among-attached.

### [P1] Branch create/delete "all-or-nothing" has a real cross-repo TOCTOU and **no rollback** — weaker than materialize's documented guarantee

Reference: Branch create/delete all-or-nothing preflight; B2; B4; Risk matrix.

`materialize` survives partial failure because its only mutation for failed repos
is a fresh clone it can delete, writing no lock (`handle_materialize.rs:257-265`).
Branch create/delete mutate refs **in place** across N repos. Between preflight
and the mutation loop, a concurrent process can change a ref, so the loop can fail
on repo K after 1..K-1 were already created/deleted — and the plan gives **no
rollback** (created branches aren't deleted on later failure; deleted branches
aren't restored). So "all-or-nothing" is asserted without backing, and is strictly
weaker than materialize's reject-partial. Branch ops also get no workspace lock
while stash does. **(Partly confirms R55's ownership/atomicity concerns.)**

Fix: Specify create rollback (delete branches created earlier in the batch on
later failure); treat delete as best-effort-after-full-preflight and report a
partial aggregate honestly rather than claiming all-or-nothing; decide whether
branch ops share the stash workspace lock to close the TOCTOU.

### [P1] `checkout_branch` already implements the proposed switch/create semantics — the "no switch-existing-branch primitive" claim is wrong, risking duplicate/contradictory primitives and a materialize regression

Reference: Current implementation state, line 37; B2 (`switch_branch`,
`branch_create`).

`GitBackend::checkout_branch(path, branch)` already exists and already does almost
exactly what B2 proposes: create-if-missing, checkout-if-present, **refuse
(`DivergedMember`) if the branch exists at a different commit**, and self-verify
HEAD is attached (`gitbackend.rs:64-74`). `handle_materialize` depends on this
behaviour for branch restore (`handle_materialize.rs:202-226`). B2's `switch_branch`
("must not create") and `branch_create` ("reject different-commit") re-specify
overlapping-but-subtly-different contracts as **new** methods, so Forge could add
redundant primitives that drift from — or refactor and regress — `checkout_branch`.

Fix: Have B2 state explicitly how `branch_list/create/delete/switch_branch` relate
to `checkout_branch`: build the new methods by factoring/splitting it (single
source of truth for the divergence rule), or document why a create-free switch is
needed and that `checkout_branch` is left untouched. Add a regression gate that
existing materialize branch-restore tests stay green when this file is edited.

### [P1] B4 protocol (`ActionKind.branch`, `BranchOp`, `BranchRequest/Response`) is owned by Prisma but scheduled in no wave **(confirms R55)**

Reference: Multi-agent table (Prisma "B4 protocol slice"; Curator "Must wait for
B4 protocol slice"); Wave 3.

`ActionKind` ends at `forall=15` and there is no branch action/op/message in the
schema (`gwz.taut.py:79-95`). B4 requires adding all of them. The table assigns
the "B4 protocol slice" to Prisma and makes Curator/Lens depend on it, but the
wave schedule never gives Prisma a B4-protocol task (Wave 1 = B1; Wave 2 = S3;
Wave 3 = Curator implements the handler). Curator cannot build `handle_branch.rs`
against a non-existent `BranchRequest`.

Fix: Split B4 into **B4a** (Prisma: `ActionKind.branch`, `BranchOp`,
`BranchRequest/Response`, branch repo summary, corpus + protocol-test gate) and
**B4b** (Curator handler / Lens CLI). Schedule B4a at the end of Wave 2 / start of
Wave 3 with a `regen.py --check` + `cargo test --test protocol` gate before B4b.

### [P1] The B5 merge **wire shape** is unscheduled in any protocol slice **(confirms R55)**

Reference: B5 touchpoints (lines 465-467); Resolved branch decisions; Risk matrix.

B5 needs aggregate + per-member conflict-path response fields, but B5's touchpoints
list only handler/backend/renderer — **not** `protocol/gwz.taut.py` — and no other
slice (B1, S3, B4) defines a merge result message. The backend already returns
conflict paths (`GitIntegrateResult.conflicts`, `gitbackend.rs:174-179`) and
`MemberStatus.conflicted`/`AggregateStatus.conflicted` already exist
(`gwz.taut.py:127,137`), but there is no wire field to carry **per-member conflict
paths** to the driver, and the aggregate rule (`all-conflicted` vs
`some-merged-one-conflicted`) is undefined.

Fix: Add a protocol sub-slice that defines the merge result wire shape (per-member
conflict paths + an explicit aggregate-status rule) and schedule it **before** B5.

## P2 findings

### [P2] libgit2 stash apply aborts on conflict **without** porcelain markers — the conflict contract needs to be non-porcelain

`git2 0.21` `stash_apply`/`stash_pop` wrap `git_stash_apply`, which on conflict
returns `GIT_EMERGECONFLICT` and **aborts without leaving conflict markers** in
the worktree — unlike porcelain `git stash apply`, which leaves conflicts on disk.
(Contrast `merge_upstream`, which writes `MERGE_HEAD` and asserts conflicts
persisted, `gitbackend.rs:431-448` — that pattern is not available here.) So on
`stash_conflict` the member is left with neither the stash applied nor markers; the
user must re-run manually. S6 never states this. Fix: document that on
`stash_conflict` the member worktree is **unchanged**, keep the member `pending`,
and surface guidance; test that a real conflict returns `stash_conflict` **and**
the worktree is unmodified.

### [P2] Restore-state enum still cannot encode push-side failure **(confirms R55)**

S4 requires recording `completed`/`failed`/`unattempted` members after a
mid-operation push failure, but the only per-member fields are participation
(`stashed`/`empty`) and `restore_state`
(`pending`/`applied`/`popped`/`dropped`/`noop`/`missing`) — none encodes "save was
attempted and failed" or "selected but not yet attempted." Concrete trigger: a
selected member with an **unborn HEAD** errors on `git stash`, and the bundle can't
distinguish that from a clean `empty` member, so reconciliation can't tell a clean
member from a failed-push one. Fix: add a push-lifecycle dimension (e.g.
`attempt_state: unattempted/saving/saved/failed` + error code) distinct from
`restore_state`; test a member whose `stash_save` fails.

### [P2] Bundle-deletion "all members terminal" is ambiguous on `missing` and collides with crash recovery

S6 leaves the terminal set conditional ("`popped, dropped, noop, or missing if
that policy is chosen`"). A member can be `missing` because the user manually
`git stash drop`ed it **or** because a crash lost the payload. If `missing` is
terminal, a bundle that crashed mid-push can be auto-deleted, destroying the
recoverable metadata the crash-recovery story depends on; if it isn't, a
legitimately-removed payload can never be cleaned up except via drop. Fix: decide
and document; if terminal, gate deletion on the bundle not being in a
pending-push/partial state. Test a bundle with mixed `missing`+`pending` members.

### [P2] Conflict-as-normal-state recovery is asserted but not tested for resumability; `capture` records a mid-merge member as dirty/pre-merge

The model leaves a conflicted member in git merge state (`MERGE_HEAD` present) and
relies on the developer running `gwz capture`/`gwz commit`. But `capture` writes
the **observed** head/commit/dirty via `observed_member_map`
(`handle_materialize.rs:65-95,401-434`); a mid-merge member reads as **dirty +
pre-merge commit**, so the lock would mix merged (clean) and unmerged (dirty)
members with no recorded "merge in progress." There is no `--continue`/`--abort`
verb, and the plan doesn't say an in-progress merge blocks subsequent branch ops.
Fix: surface `MERGE_HEAD` in status and block re-running `branch --merge` on that
member; define the aggregate status enum; test that after resolve+commit the lock
converges; scope or explicitly defer a `--continue`/`--abort` verb.

### [P2] `branch_create`'s noop/reject rule duplicates `checkout_branch`'s `DivergedMember` logic

B2's `branch_create` contract restates the orphan-safety rule already inside
`checkout_branch` (`gitbackend.rs:595-643`). A second primitive re-deriving it can
drift, and create-and-switch composing `branch_create` then `switch_branch` could
double-check or skip the divergence test. Fix: factor `branch_create` out of the
existing `checkout_branch` ref logic, or require it to self-verify the same rule;
make create-and-switch reuse `checkout_branch`.

### [P2] Delete's clean-worktree requirement is unmotivated against backend reality **(confirms R55)**

Resolved decisions list `delete` among ops requiring clean worktrees, but B2/B4
enforce delete safety purely by refusing to delete a **checked-out (current)**
branch. Deleting a non-current ref does not touch the worktree at all, so the
clean-worktree-for-delete gate is unmotivated and untested. Fix: drop delete from
the clean-worktree requirement; keep only the current-branch refusal, and state
explicitly that non-current delete ignores worktree dirtiness.

### [P2] B6 root-switch re-resolution against a changed member set is a partial-state hazard with no rollback

B6's order (preflight, switch root first, reload `gwz.yml`+`gwz.lock.yml`,
re-resolve selection, then switch members) is the right shape, but after the root
switch the manifest may **drop/rename/repath** originally-selected members.
`resolve_locked_selection` errors `LockNotFound` for members missing from the new
lock (`handle_materialize.rs:384-399`), aborting the op **after the root was
already switched** — leaving the root on the new branch with members un-switched
and no rollback. Fix: define behaviour for dropped/renamed members (error class +
whether to roll the root back), and make the root switch reversible (record prior
root branch) or fully dry-preflight member resolvability against the target root's
`gwz.conf` before mutating the root.

### [P2] "Disjoint ownership" is contradicted by single-file chokepoints **(confirms R55)**

`GitBackend` is one trait+impl file; Wave 1 has **Forge** adding both S2 stash and
B2 branch primitives to `gitbackend.rs` (the plan flags this and offers an
*optional* `Forge-Branch`/`Forge-Stash` split). **Lens** owns `clirequest.rs`,
which holds both the `CliRequest` enum (`:130`) and the central `CommandArgs→request`
match (`:310-375`); every new stash/branch subcommand edits these same two spots,
so S7, B3-CLI, B4-CLI, B5-CLI all serialize through one file. Fix: make the
Forge split mandatory with commit ordering, and define a fixed integration order
for `clirequest.rs` edits.

## P3 findings

- **`GWZDesign.md` is stale on the directory layout it's cited for.** It names
  `workspace/gwz.yml`, `workspace/gwz.lock.yml`, `.gwz/snapshots`
  (`GWZDesign.md:207,354,356`) where the code uses `gwz.conf/...`
  (`artifact/mod.rs:13-15`, `workspace/mod.rs:7`). Add an erratum before citing it
  as authority for the stash location (see P1).
- **Reconciliation "surface but don't adopt" leaves an undeletable orphan.** v0
  surfaces gwz-prefixed orphan stashes (the S4 crash test creates exactly one) but
  provides no command to adopt/pop/drop them — `drop` operates on bundles. Provide
  a minimal escape (`gwz stash drop --orphan <ref>`) or print the manual
  remediation in list output.
- **`materialize --switch` preflight is new logic, not reuse.** `materialize_preflight`
  only rejects dirty members (`materialize_preflight.rs:43-50`) and
  `materialize_target_members` has no branch arm (`handle_materialize.rs:473-543`);
  B3 must add a branch-target arm + a branch-existence preflight. The sequencing
  rationale's "mostly reuses" understates this — the lock-rewrite/boundary-sync is
  reused, but target resolution and preflight are genuinely new.
- **create-and-switch crosses agent ownership.** Switch lives behind
  `MaterializeTargetKind.branch` (Switchboard, `handle_materialize.rs`) but
  create-and-switch lives in `BranchRequest` (Curator, `handle_branch.rs`). Extract
  a shared `switch+observe+lock-rewrite+boundary-sync` core helper with explicit
  ownership so Curator depends on a stable signature instead of duplicating it.
- **Doc disposition misses `GwzGaps.md`.** It keeps `GwzGaps.md` active but moves
  `GwzStashPlan.md`/`GwzBranchPlan.md` to history, while `GwzGaps.md:15-16` still
  points readers at `GwzStashPlan.md`. Add `GwzGaps.md` to the disposition list.

## Strengths (verified, not assumed)

- **Touchpoints are overwhelmingly accurate.** `src/protocol/convert.rs` (the
  correct home for model→generated conversions), `src/status/branch_groups_and_differences.rs`,
  `materialize_preflight.rs`/`push_member.rs`, `artifact/mod.rs` atomic writers, and
  the CLI help modules (`materialize_long/after`, `snapshot_long/after`,
  `append_branch_summary`) all exist. Verification commands are real:
  `tests/rename.rs` and `tests/protocol.rs` exist; `protocol/regen.py --check` is
  genuinely implemented (`regen.py:152,187,231`).
- **The GitBackend inventory and `MaterializeTargetKind` claim are correct**: no
  `stash_*`/`branch_list/create/delete` methods exist, and the enum is exactly
  `Lock/Head/Snapshot/Tag/Commit` (`generated.rs:201`) with no `Branch`.
- **Native-stash-oid identity + pre-mutation index re-resolution is sound and
  implementable** on `git2 0.21` (`stash_foreach` exposes `(index, message, &Oid)`),
  and the cross-bundle index-shift risk only exists within a single member's stash
  stack — exactly what the S2 "older bundle after newer stash" test targets.
- **Atomic-write reuse is well-founded** (`artifact/mod.rs` already does
  stage-durably-then-rename with fsync of file and parent dir).
- **The protocol append-only discipline matches reality** (`ActionKind.forall=15`,
  `GwzErrorCode` tail `internal_error=29`; `GWZDesign.md:1443-1449` makes the enum
  the registry), and the plan correctly names the real shared chokepoints.
- **Routing `--switch` through `MaterializeTargetKind.branch` maps onto proven
  code**: `handle_materialize` already re-observes post-mutation and rewrites the
  lock from observed state, then runs `sync_workspace_boundary`
  (`handle_materialize.rs:267-281`). The conflict-as-normal-state model matches
  `merge_upstream`'s actual conflict-preserving behaviour
  (`gitbackend.rs:431-448`), and `MemberStatus.conflicted`/`AggregateStatus.conflicted`
  already exist on the wire.

## Recommended gating actions (priority order)

1. **Resolve the P0 scope gate** in Wave 0 — promote the ops in
   `GWZRequirements.md`/`GWZDesign.md`, or mark all phases cannot-merge-until-docs.
2. **Decide stash storage** (`gwz.conf/stash/` vs `.gwz/`) and, if `.gwz/`, add the
   root-exclude entry + a stash-specific `.yml`/newest-first list helper; add an
   errata for the stale `GWZDesign.md` layout.
3. **Specify the file-backed cross-process lock** (and decide whether branch ops
   share it), with a real two-process test.
4. **Reconcile B2 against `checkout_branch`** before Forge touches `gitbackend.rs`,
   with a materialize-regression gate.
5. **Schedule the missing protocol slices** — B4a (branch protocol) and the B5
   merge wire shape — each before its consumer, behind `regen.py --check` +
   `cargo test --test protocol`.
6. **Pin B5 to current-branch-only merge** until switch-before-merge and the
   aggregate-status enum are defined; add the named-branch snapshot resolver and the
   detached/unborn/mixed error taxonomy as explicit B3 work items with tests.
