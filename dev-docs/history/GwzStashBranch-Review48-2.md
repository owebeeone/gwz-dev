# Review 48-2: GwzStashBranchPlan.md (second pass)

Review target:
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranchPlan.md` (revised, responds
to `GwzStashBranch-Review48.md` and `GwzStashBranch-Review55.md`).

Reviewer: Claude Code (Opus 4.8). This is a follow-up to
`GwzStashBranch-Review48.md`; line references below are to the **revised** plan.

## Verdict

**The response is comprehensive and sound. Recommend proceeding to Wave 0.**

Every P0/P1 from both reviews is resolved with a concrete decision, a touchpoint,
a test, and (in most cases) a risk-matrix row — not deferred or hand-waved. The
revision is internally consistent and the new structure (Wave 0 scope gate, split
B4a/B4b + B5a/B5b protocol-before-handler slices, the dedicated documentation
revision pass D0-D4, mandatory Forge-Branch/Forge-Stash split, file-backed shared
mutator lock) directly tracks the findings. What remains are a few residual P2/P3
clarifications — two of them small **modeling questions introduced by the new
metadata fields** — none of which block Wave 0.

## Disposition of Review48 findings

All resolved; the strongest ones, with where the fix landed:

- **[P0] Scope gate** — resolved. Status intro now states implementation cannot
  merge until Wave 0 promotes scope (lines 20-24); Wave 0 scope/docs gate
  (750-768) + D0 (892-913) + Gate W0 "Review confirms `GWZRequirements.md` no
  longer requires branch/stash hidden/rejected" (767-768); risk row (868).
- **[P1] `.gwz/` storage** — resolved by an explicit decision: bundles are
  intentionally local-only runtime under `.gwz/stash/bundles/<id>.yaml` with the
  non-portability tradeoff stated (107-112); Wave 0 reconciles the stale design
  layout (758-759, 906-907).
- **[P1] `.gwz/` dirties the root** — resolved: "Root exclude boundary" decision
  (113-116), S1 touchpoint extends `sync_workspace_boundary` (163-166), S1 test
  (198-199), risk row (870).
- **[P1] `.yml`/list helper** — resolved: switched to `.yaml`, dedicated list
  helper required ("do not reuse artifact listing helpers that only scan `.yaml`
  by path order unless generalized for created-at descending", 174-176), test (196).
- **[P1] Cross-process lock** — resolved: file-backed cross-process mutator lock
  under `.gwz/locks/` with acquisition semantics (140-144, 160-162, 182-186),
  two-process test (197), risk row (875), and shared with branch ops (433-434).
- **[P1] `merge_upstream` merges current HEAD** — resolved: v0 is
  current-attached-branch-only, `--into` deferred (419-425); B5b asserts the
  target equals each member's current HEAD before calling `merge_upstream`
  (643-648); risk row (880).
- **[P1] `snapshot --branch <name>` resolver** — resolved: named-branch builder
  via `read_ref` writing `branch=<name>/detached=false/dirty=false` from the ref
  (528-530), routed through `handle_snapshot` (507-508), tests (542-543).
- **[P1] Mixed/detached/unborn taxonomy** — resolved: distinct typed errors
  `branch_detached_head`/`branch_unborn_head`/`branch_mixed` (411-414, 459-460),
  single-member trivially-satisfied rule, tests (539-540).
- **[P1] Create/delete TOCTOU + rollback + lock** — resolved: shared mutator lock,
  create-rollback, delete best-effort "must not claim transactional all-or-nothing"
  (433-437); B4b work + tests (587-591, 599-601); risk row (879).
- **[P1] `checkout_branch` overlap** — resolved: current-state note (62-64),
  "Existing checkout primitive" decision (438-441), B2 "factor shared
  ref/divergence checks ... do not regress materialize" (480-481) + regression
  test (499-500).
- **[P1] B4 protocol unscheduled** — resolved: split into B4a (protocol,
  scheduled Wave 2 before B4b) and B4b (handler/CLI, Wave 3); B5 likewise split
  into B5a (protocol) / B5b (handler).
- **[P1] Merge wire shape unscheduled** — resolved: B5a defines source/target/
  commit/conflict-paths/per-repo result + aggregate rules before B5b (605-627).

P2/P3 also all addressed: git2 conflict-without-markers contract (145-148,
216-218, 360-362); push-lifecycle state for crash recovery (130-134, 282-284);
`missing` explicitly non-terminal for auto-deletion (345-349); in-progress merge
(`MERGE_HEAD`) surfaced/blocked (647, 653-654, 663); delete dirty-policy corrected
(415-418); B6 target-manifest inspection/rollback (681-684, 692-693); mandatory
Forge split + ordered `clirequest.rs` edits (739-740, 745); stale `GWZDesign.md`
layout reconciliation (D0/D3); orphan manual-remediation guidance (137-138,
313-314); shared switch helper assigned to Switchboard (741); `GwzGaps.md` in the
disposition (D4, 1015-1016).

## Residual / newly-introduced items (all minor)

### [P2] `participation` and the new `push_lifecycle` axis overlap on `empty` — clarify orthogonality

Reference: Resolved stash decisions (Partial restore / Push lifecycle, 126-134);
S1 "Define ... participation, restore state, push lifecycle state" (170-171); S4
(279-284).

The revision adds `push_lifecycle: unattempted/saving/saved/empty/failed` (132)
but keeps a separate `participation` field, whose original values are
`stashed`/`empty`. So a clean member is `empty` in **both** axes, and a dirty
member's `participation=stashed` is fully derivable from `push_lifecycle ∈
{saving, saved, failed, unattempted}`. As written, `participation` is redundant
with `push_lifecycle`, and S4's "record clean members as `empty`/`noop`" (279)
spreads the clean-member state across three enums (participation, push_lifecycle,
restore_state) without pinning their relationship.

Fix: Either drop `participation` in favour of `push_lifecycle` (with `empty`
meaning "nothing to stash"), or state explicitly that `participation` is the
pre-attempt membership classification and `push_lifecycle` is the attempt outcome,
and define the exact (participation, push_lifecycle, restore_state) tuple for each
of: clean/noop, stashed-and-saved, failed-push, and post-restore states. Add a
round-trip test pinning those tuples.

### [P2] `merged`/`conflicted` action-result values are defined in B4a but the merge protocol is B5a — slice ownership is incoherent

Reference: B4a work "Include branch action result values (`created`, `exists`,
`deleted`, `switched`, `merged`, `conflicted`, `noop`, `skipped`)" (559-561); B5a
"Add merge result fields ... per-repo action result" (615-616).

B4a (the list/create/delete protocol slice, Wave 2) defines the `merged` and
`conflicted` action-result enum values, while B5a (Wave 3) defines the merge
result message that actually uses them. Appending the values early is harmless for
wire-compat, but it means the branch-command slice owns merge result vocabulary
before merge is protocol-shaped, which muddies Prisma's per-slice numbering ledger
and could lead to a double-add or a value landing in the wrong slice's corpus.

Fix: Either move `merged`/`conflicted` into B5a (so each slice owns its own result
values), or state in B4a that it intentionally establishes the **full** `BranchOp`
result enum up front and B5a adds only fields, not enum values — and have the W2
gate assert the value set is frozen so B5a appends none.

### [P3] Commit to the `.gwz/`-wipe recovery story, not just cross-clone non-portability

Reference: Registry storage decision (107-112); Reconciliation (135-139).

The decision documents non-portability across clones, but the more common local
hazard is a `.gwz/` wipe within one clone (a root `git clean -x`, a manual
`rm -rf .gwz`, or CI cache eviction): the registry vanishes while the native
stashes remain, so **every** coordinated stash degrades to N orphan native
stashes. The reconciliation path ("surface with manual remediation") covers it,
but the plan should say so explicitly for the wipe case, since it is the expected
failure mode of choosing disposable runtime storage.

Fix: Add one line to the Registry storage or Reconciliation decision: a `.gwz/`
wipe loses bundle grouping and leaves recoverable per-member orphan stashes that
`gwz stash list` surfaces with manual-remediation guidance (the v0 contract).

### [P3] Note that the shared mutator lock serializes stash *and* branch workspace-wide

Reference: Operation locking (140-144); Branch mutation locking (433-434).

Sharing one file-backed mutator lock across stash and branch means a
`gwz stash push` and a `gwz branch --merge` in two terminals are mutually
exclusive workspace-wide — a reasonable safety default, but a behavior users will
notice (one command blocks/queues on the other). Worth one sentence so it is a
documented choice rather than a surprise, and so the D3 OperationModel/Embedding
docs describe the lock's scope.

### [P3] Pin the advisory-lock semantics' filesystem caveat, and the Forge-Stash "shared helper patch" mechanism

- The lock decision relies on OS advisory locks being released on process death
  (182-186) — correct for `flock`/`fcntl`/`LockFileEx` on local filesystems, but
  POSIX advisory locks are unreliable on NFS/network mounts. Since `.gwz/` is
  local-only this is mostly fine; add NFS to the "document the stale-lock policy"
  caveat so a networked workspace root is handled deliberately.
- Forge-Stash "rebase after Forge-Branch or use Atlas-owned shared helper patch"
  (740) leaves the shared-`checkout_branch`-factoring ownership ambiguous: name
  who lands the factored helper (Forge-Branch in B2) and have Forge-Stash depend
  on that landed signature, rather than an undefined "Atlas-owned patch."

## Bottom line

No blockers remain. The two P2 items are small schema/slice-ownership
clarifications best closed while Prisma freezes naming in Wave 0; the P3 items are
one-line documentation commitments. Proceeding to Wave 0 (the authoritative-doc
scope promotion + layout reconciliation) is the right next step, and the plan now
correctly gates all implementation behind it.
