# GWZ lane issues

Status: open register, started 2026-09-15. It records each problem met while
running parallel agents in `gwz local clone` lanes of
`/Users/owebeeone/limbo/gwz-dev`, with its reason and the remedy used.
Observed with the installed gwz 1.0.12; gwz-core source was at
`v1.0.12-13-g5bf8f1a`. Earlier investigation:
`GwzLaneDisposalAudit-2026-09-10.md`.

| ID | Problem | Status |
| --- | --- | --- |
| L1 | Disposing a merged lane takes two gwz operations and a manual check | open |
| L2 | An untracked file in a receiving member blocks every lane merge | open |
| L3 | A verbatim lane's Python venv still points at the source workspace | open |
| L4 | Each `gwz merge` rotates the fields of every lock member row | fix started |

## L1: disposing a merged lane needs a loss waiver

Occurrences:

- 2026-09-10: three lanes refused; they were detached with `--keep` (see the
  audit).
- 2026-09-15, round 1: push-1-1, push-1-2, push-3-1 and push-3-2, forced.
- 2026-09-15, round 2: push-2-1, push-3-7 and push-3-8, forced.

**Symptom.** Every lane's work was already merged into gwz-dev. Even so,
`gwz local dispose <lane>` exits 1 after 7–10 s with `UnwaivedHazard` and
removes nothing. Round 2's push-2-1 reported the following; the other two lanes
reported the same two hazards.

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

**Remedy used (round 2).**

1. Check that no lane holds anything gwz-dev lacks. For the root and each
   member, compare the lane with gwz-dev:
   - the commits from `rev-list --all`, `reflog --all` and `stash list` that
     gwz-dev does not have (none, in all three lanes);
   - the entries in `git status --porcelain --ignored` that gwz-dev does not
     have (none).
2. `gwz local dispose <lane>` refuses as shown above; its output names the
   hazards.
3. `gwz local dispose <lane> --force dirty,unpreserved-history` succeeds in
   30–34 s.

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

## L4: each merge rotates lock member fields

**Symptom.** A lane commit shows a ~50-line diff of `gwz.conf/gwz.lock.yml`, in
which only one member's commit actually changed.

**Reason.**
- `gwz commit` writes member fields in struct order: `path, source_id,
  source_kind, commit, branch, detached, dirty, materialized`.
- `gwz merge` rewrites each selected row in `render_complete_lock`
  (`gwz-core/src/workspace_ops/merge/acceptance/v1/support.rs:187-203`) by
  removing and re-inserting each field.
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

**Remedy.** Nothing is needed per operation. The fix is `shift_remove`, with a
test that compares the rendered bytes; it starts in lane `lock-order` on
2026-09-15. The reordering continues until a gwz release with the fix is
installed.
