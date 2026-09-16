# GWZ lane issues

Status: open register, started 2026-09-15. It records each problem met while
running parallel agents in `gwz local clone` lanes of
`/Users/owebeeone/limbo/gwz-dev`, with its reason and the remedy used.
Observed with the installed gwz 1.0.12 through round 9; gwz-core source was at
`v1.0.12-13-g5bf8f1a` when the register opened. gwz 1.0.13 was released and
installed on 2026-09-16: it carries the L4 fix and changes nothing for L1 to
L3. Earlier investigation: `GwzLaneDisposalAudit-2026-09-10.md`.

| ID | Problem | Status |
| --- | --- | --- |
| L1 | Disposing a merged lane takes two gwz operations and a manual check | open; requirements draft in gwz-core `dev-docs/GwzLaneCleanFixes.md` |
| L2 | An untracked file in a receiving member blocks every lane merge | open |
| L3 | A verbatim lane's Python venv still points at the source workspace | open |
| L4 | Each `gwz merge` rotates the fields of every lock member row | fixed (gwz-core `44b24ee`), released in gwz 1.0.13 |

## L1: disposing a merged lane needs a loss waiver

Occurrences:

- 2026-09-10: three lanes refused; they were detached with `--keep` (see the
  audit).
- 2026-09-15, round 1: push-1-1, push-1-2, push-3-1 and push-3-2, forced.
- 2026-09-15, round 2: push-2-1, push-3-7 and push-3-8, forced.
- 2026-09-15, round 3: lock-order, push-2-2 and push-3-3, forced.
- 2026-09-15, round 4: push-3-4, forced.
- 2026-09-15, round 5: push-3-5, forced.
- 2026-09-16, round 6: push-3-6, push-4-1 and push-e2e, forced.
- 2026-09-16, round 7: fix-pushurl and fix-symlink-root, forced.
- 2026-09-16, round 8: fix-review, split-ops, split-ca, split-git and
  split-crates, forced. Same six dirty and four unpreserved-history members
  each. Plain refused in 5–16 s; forced took 28–44 s, except split-ca at
  238 s.
- 2026-09-16, round 9: fix-refcopy, fix-g12 and fix-locks, forced. Same
  counts. Plain refused in 6–15 s; forced took 27–33 s.

**Symptom.** Every lane's work was already merged into gwz-dev. Even so,
`gwz local dispose <lane>` exits 1 after 5–10 s with `UnwaivedHazard` and
removes nothing. Round 2's push-2-1 reported the counts below, and the other two
round-2 lanes reported the same two hazards. Every lane in rounds 3 to 7
reported exactly these counts.

| Repository | `dirty` entries | `unpreserved-history` roots |
| --- | --- | --- |
| root | 20 | 3 |
| gwz-cli | 3 | 12 |
| gwz-core | 12 | 32 |
| gwz-py | 14 | 5 |
| taut | 10 | 0 |
| taut-shape-rs | 1 | 0 |

**Reason.** Both hazards come from the lane being a verbatim copy, not from
anything the lane did.

- `dirty`: every ignored entry counts as user work, and a verbatim lane copies
  all of them. Examples:
  - build and cache directories: `target/`, `.venv/`, `.pytest_cache/`,
    `.ruff_cache/` and `__pycache__/`;
  - agent and editor state: `.claude/`, including `.claude/.cc-writes/` and
    `.claude/settings.local.json`;
  - native stash entries.

  See `gwz-core/crates/work-detector/src/lib.rs` ("ignored does not mean
  disposable") and `gwz-core/src/local_clone/adapters/disposal.rs`.
- `unpreserved-history`: stash entries and reflog-only commits are protected
  roots. A surviving repository preserves a root only through a durable ref,
  `HEAD` or a tag. Its own identical reflog and stash entries do not count
  (`is_eligible_witness_root` in `gwz-core/crates/history-check/src/lib.rs`).
  A lane copies gwz-dev's stashes and reflogs, so a lane of any repository that
  has either one refuses.

**Remedy used (rounds 2 to 7).**

1. Check that no lane holds anything gwz-dev lacks. For the root and each
   member, compare the lane with gwz-dev:
   - the commits from `rev-list --all`, `reflog --all` and `stash list` that
     gwz-dev does not have (none, in all three lanes);
   - the entries in `git status --porcelain --ignored` that gwz-dev does not
     have (none).
2. `gwz local dispose <lane>` refuses as shown above; its output names the
   hazards.
3. `gwz local dispose <lane> --force dirty,unpreserved-history` succeeds in
   23–34 s.

Removing a lane is therefore two gwz operations, plus a check that gwz does not
make itself.

**Fix candidates** (not yet designed):

- **Copy-aware disposal.** Record at clone time what was copied: reflog and
  stash entries, and fingerprints of ignored entries. At dispose time, treat
  anything unchanged since then as not the lane's work.
- **Caches.** Treat a directory with a valid `CACHEDIR.TAG` as a cache.
  Every `target/`, `.venv/`, `.pytest_cache/` and `.ruff_cache/` here has one;
  `__pycache__/` could be added.
- **Clean lanes.** Enable `gwz local clone --clean`, which the parser accepts
  but this build refuses.
- **Built-in check.** Have dispose run step 1 itself, so the history waiver is
  not needed when the check passes.

## L2: an untracked file in a receiving member blocks merges

**Symptom.** 2026-09-15, round 1: `gwz --target @all merge --remote <lane>`
refused with `DirtyMember` for gwz-core. The cause was another session's
untracked drafts.

**Reason.** Merge refuses when a member it writes to has any untracked file,
and there is no override.

**Remedy used.** Wait for the drafts' owner to commit them; the four merges then
succeeded. Never leave an edit uncommitted in gwz-dev while a lane merge is
pending.

## L3: a verbatim lane's venv points at the source workspace

**Symptom.** 2026-09-15, round 2, lane push-3-8: gwz-py tests run in the lane
imported gwz-dev's source.

**Reason.** The copy keeps absolute paths. `gwz.pth` in `gwz-py/.venv`,
`.venv/bin/activate` and the console scripts all still name
`/Users/owebeeone/limbo/gwz-dev`.

**Remedy used.**
- The agent repointed `gwz.pth` at the lane, and ran tools with an explicit
  `VIRTUAL_ENV` and `.venv/bin/python -m …`.
- Merge a gwz-py change into gwz-dev only when no other lane is running gwz-py
  tests.

**Found in round 6 (lane push-e2e).**
- **Absolute paths.** 18 text files in the venv name the source workspace, and
  all of them need rewriting:
  - `gwz.pth`;
  - five `activate*` scripts;
  - ten console-script shebangs;
  - `direct_url.json` and the SBOM.
- **Native extension.** It is copied from the source workspace's build. Rebuild
  it with `maturin develop`, passing `VIRTUAL_ENV` explicitly.
- **Stale caches.** Copied `__pycache__` and `.pytest_cache` directories point
  at the source workspace's test files. Remove them.
- **`VIRTUAL_ENV`.** `run_tests.py` sets `VIRTUAL_ENV` only if it is unset. A
  shell that inherited gwz-dev's `VIRTUAL_ENV` makes `maturin develop` install
  into gwz-dev's venv, so always set it explicitly.
- **Proof.** A lane tests its own code when all of these hold:
  - `gwz.__file__` points into the lane;
  - the native module's path points into the lane;
  - the module's recorded revision matches the lane's gwz-core HEAD.

## L4: each merge rotates lock member fields

**Symptom.** A lane commit shows a ~50-line diff of `gwz.conf/gwz.lock.yml`, in
which only one member's commit actually changed.

**Reason.**
- `gwz commit` writes member fields in struct order: `path, source_id,
  source_kind, commit, branch, detached, upstream, dirty, materialized`, leaving
  out fields that are not set.
- `gwz merge` rewrites each selected row in `render_complete_lock`
  (`gwz-core/src/workspace_ops/merge/acceptance/v1/support.rs:187-203` at
  `5bf8f1a`) by removing and re-inserting each field.
- serde_yaml 0.9's `Mapping::remove` is `swap_remove`: it moves the row's last
  field into the gap. So each merge rotates the first seven fields by one place,
  and `materialized` stays last.
- The root history matches this exactly:
  - `367fe86` is in commit order;
  - `c9f11d8`, `64944c5` and `c5d970c` each rotate it once more;
  - round 1's four merges ended at `4e7c8a3`'s
    `commit branch detached dirty path …`.

**Impact.** Noise only: the merge checks that the rendered YAML parses back to
the same typed lock.

**Remedy.** Nothing is needed per operation.

**Fix.** gwz-core `44b24ee` (lane `lock-order`, merged on 2026-09-15) rebuilds
each selected row instead of editing it in place:

- Known fields come first, in the commit writer's order, then any fields gwz
  does not know, in their original order. `shift_remove` would have put the
  unknown fields first.
- A new member row goes before the first row with a larger id.
- Four tests check that:
  - eight repeated renders stay byte-identical;
  - the output equals the commit writer's bytes when rows are added;
  - unknown fields follow the known ones;
  - the renderer's field list matches `ResolvedMemberArtifact`.

The merge renderer was the only lock writer with its own field order. Every
other writer serializes the typed lock.

**Limits.**

- Rows that an earlier merge already rotated stay rotated until a later merge
  selects them, or until a `gwz commit` that commits a member rewrites the lock.
- Lanes merged with gwz 1.0.12 (rounds 1 to 9) kept rotating rows. gwz 1.0.13,
  installed 2026-09-16, carries the fix; merges from then on write the commit
  writer's order.
