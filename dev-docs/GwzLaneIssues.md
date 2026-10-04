# GWZ lane issues

Status: open register, started 2026-09-15. It records each problem met while
running parallel agents in `gwz local clone` lanes of
`/Users/owebeeone/limbo/gwz-dev`, with its reason and the remedy used.
Observed with the installed gwz 1.0.12 through round 9; gwz-core source was at
`v1.0.12-13-g5bf8f1a` when the register opened. gwz 1.0.13 was released and
installed on 2026-09-16: it carries the L4 fix and changes nothing for L1 to
L3. Earlier investigation: `GwzLaneDisposalAudit-2026-09-10.md`. gwz 1.0.14, released and installed 2026-09-18, carries the lane clean-up's Phase 1 steps S1.1 to S1.4 (gwz-core `757ed57a` and after): the `unpreserved-history` half of L1 is expected to clear for lanes made from then on; the `dirty` half remains until Phase 1's S1.5 to S1.8 and Phase 2 land.

| ID | Problem | Status |
| --- | --- | --- |
| L1 | Disposing a merged lane takes two gwz operations and a manual check | fixed on gwz-core main (clean-up Phases 1 and 2, `c46fe4c7`): a merged verbatim lane disposes with no waiver, built in or not; released in gwz 1.0.17 on 2026-09-18 |
| L2 | An untracked file in a receiving member blocks every lane merge | open |
| L3 | A verbatim lane's Python venv still points at the source workspace | open |
| L4 | Each `gwz merge` rotates the fields of every lock member row | fixed (gwz-core `44b24ee`), released in gwz 1.0.13 |
| L5 | gwz-py's test runner prefers a stale `gwz-cli/target/debug/gwz` copied into the lane | fixed (gwz-py `4adc636`, released in gwz 1.0.14); the stale directory in gwz-dev is deleted |

Round 10 (2026-09-17, gwz 1.0.13): three lanes (`r20-wait`, `clean-fixes`,
`claude-hooks`) implementing GwzLaneCleanFixes R20 to R22, its Phase 1, and
the Claude Code hook; created in 97 to 99 s each, merged in that order with
no textual conflict, forced disposal 34 to 45 s each. Phase 1 of the clean-up
(gwz-core `757ed57a` and after) is now merged but not released: the
`unpreserved-history` half of L1 is expected to clear once it is in the
installed gwz.

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
- 2026-09-17, round 10: r20-wait, clean-fixes and claude-hooks, forced
  after the ancestor check in every repository. Forced took 34–45 s.

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

**Found in round 10 (lane r20-wait).** The consequence is not only reads.
`maturin develop` installs into the environment `gwz.pth` names, so a lane
whose `.pth` still points at gwz-dev writes its `_gwz_core.abi3.so` **into
the source workspace**. Repoint `gwz.pth` before running anything that
builds. `run_tests.py`'s `PYTHONPATH` masks the import side only.

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

## L5: gwz-py's runner prefers a stale `gwz-cli/target/debug/gwz`

**Symptom.** 2026-09-17, round 10, lane r20-wait, and again on gwz-dev
itself after the merge: `gwz-py/run_tests.py` reports `10 failed, 866
passed`, with `unexpected argument '--verbose'` and a provenance mismatch
(extension `core 1.0.13`, CLI `core 1.0.0 revision=17e13ed2 dirty=true`).

**Reason.** `run_tests.py` prefers `gwz-cli/target/debug/gwz` and falls back
to the workspace's `target/debug/gwz` only when the former is missing. In
this workspace cargo builds gwz-cli into the root `target/` (gwz-cli is a
workspace member), so `gwz-cli/target/debug/gwz` is a standalone build from
8 September that nothing refreshes, and every verbatim lane copies it. The
runner then silently tests against a months-old CLI. This is R6 in
`GwzLaneCleanFixes.md` (`gwz-cli/target`, untagged) seen from the other
side.

**Remedy used.** `GWZ_RUST_BIN=/Users/owebeeone/limbo/gwz-dev/target/debug/gwz`
(or the lane's own root `target/debug/gwz`) pinned explicitly; the
provenance assertion in the suite is what catches the mismatch. Not yet
done: either delete `gwz-cli/target` in gwz-dev, or change the runner's
preference to the workspace binary when gwz-cli is a workspace member.

## L6: a dispose refusal renders a protected ref in Rust debug form

**Symptom.** 2026-09-18, the 1.0.17 docs Surface review (gwz-cli
`dev-docs/history/GwzRelease1017Docs-ReviewSurface.md`, P3-4). A lane holding a
unique commit is refused with `unique to the lane 1: ... Head <oid>, Ref {
name: "refs/heads/main" } <oid>`. The `Ref { name: ... }` part is an internal
type's `{:?}` formatting, not a ref name a user can paste into git.

**Reason.** gwz-core's protected-root rendering in `src/local_clone/dispose.rs`
formats the root's source with `Debug`. Released behaviour since 1.0.16; the
docs in 1.0.17 describe the form as it prints and say it is not a path.

**Remedy used.** None yet. Fix: print `refs/heads/main` (and `HEAD`) in the
refusal and update the sample in gwz-cli `docs/LocalClones.md`; check the
gwz-py test that asserts the refusal text. Not a docs change, so it was
deferred out of the 1.0.17 documentation lane.

## L7: a merge that exposes an ignored directory goes to `recovery-required`

**Symptom.** 2026-10-02, merging the TR2.15 lane into gwz-dev with gwz 1.0.17. The merge deletes the `tests/transport_ssh` crate, including its `.gitignore`. In the receiving workspace that left the crate's 2.1 GB build cache, `tests/transport_ssh/target/`, as an untracked directory. `gwz merge --remote` made gwz-core's merge commit, then stopped in `recovery-required` before the root participant ran. Continue and abort were both blocked. The drifts it reported were `worktree-modified`, `head-advanced`, `target-ref-changed` and `pending-action-ambiguous`. The TR2.15 agent met the same defect inside its merge lane. There it also hit a rename/delete conflict whose path was deleted on both sides, so no file existed at that conflict path. `gwz add` then refused the path as "not a conflicted participant".

**Reason.** gwz-core's post-merge conflict check (`merge_conflict_snapshot`) requires a worktree with no untracked files and a real file at every conflict path. A merge that turns ignored files into untracked ones, or leaves a path deleted on both sides, fails that check after the merge action has already run. The recovery planner then cannot match the live repository to a recovery point, and blocks both directions.

**Remedy used.**
- Park the receiving workspace's untracked drafts outside every workspace, with checksums.
- Remove the exposed build cache, which is build output of a crate the merge deleted.
- `gwz merge --status` then reports the pending true-merge as "completed exactly", with continue eligible. `gwz merge --continue` finished the root participant.
- Restore the drafts and verify the checksums.
- In the conflicted case, the agent restored the conflicted files from their index stages using only read-only `git show`, after which gwz accepted the conflict.

**Fix needed in gwz-core:**
- the post-merge check should ignore untracked files the merge did not create, or say which file blocks it;
- it should accept a conflict whose path no longer exists on either side;
- it should never block both continue and abort for a merge whose action completed exactly.

## L8: `gwz merge --continue` refuses any change outside the conflict paths

**Symptom.** 2026-10-02, the TR2.15 merge lane. After resolving the conflicts, two test modules had to be renamed in `src/git/endpoint/mod.rs` to match code from the other side. `merge --continue` refused the edit ("merge index contains changes outside the expected conflict paths"). The merge commit therefore left the modules out of `mod.rs`, and its test build has 18 warnings. A follow-up commit put them back.

**Reason.** The continue step accepts only the conflict paths' resolutions. A resolution that needs a semantic fix elsewhere cannot go in the merge commit.

**Remedy used.** A separate commit right after the merge commit. The merge commit's library still builds clean.

**Fix needed:** let `--continue` accept staged changes outside the conflict paths, perhaps behind a flag, and list them.

## L9: during an open merge, `gwz add` needs an explicit `--target`

**Symptom.** 2026-10-02, the TR2.15 merge lane. With a coordinated merge open, `gwz add <path>` with the default target selection was refused. `gwz --target gwz-core add <path>` worked.

**Fix needed:** while a merge is open, select the participant that owns the path, as `gwz add` does outside a merge.

## L10: a moved workspace retains a catalog bound to its previous target

**Symptom.** 2026-10-02, the first TR2.1 family merge after moving the workspace to another volume. GWZ 1.0.17 refused twice before coordinated merge registration: "checked merge start parents rejected catalog recovery target: durable recovery evidence belongs to another catalog target". `merge --status` reported idle and every selected HEAD was unchanged. Import refs were retained.

**Reason.** The catalog target digest includes the canonical path and durable identities of the workspace, repository, common Git directory and private parent. The retired bootstrap record in `.gwz/catalog-final` held a target digest different from the current checkout's. The move is the likely cause; the old digest was not independently reconstructed.

**Remedy used, on explicit operator approval.** Preserve the complete old catalog outside the workspace, verify every file's SHA-256, then retry through `gwz merge --remote`. GWZ created a fresh catalog and the merge completed. No catalog record was edited, no Git merge was substituted, and the old catalog remains available. This is a recovery-state workaround, not a general instruction to remove catalogs when a merge fails.

**Fix needed:** a supported relocation/reinitialization operation that checks for pending actions and preserves the previous catalog, or a refusal that explains the relocation case and a supported recovery route.

## L11: a member registered after a lane was cloned blocks every merge from it

**Symptom.** 2026-10-04, gwz-dev with gwz 1.0.17. gwz-sspi (`mem_gwz_sspi`) was registered in gwz-dev after the lane `tr1-8-win` was cloned, so gwz-dev's lock records it and the lane's does not. `gwz --target @root --target gwz-core --target gwz-core-evidence merge --remote tr1-8-win` refused before any fetch: `PairingMismatch: … import pairing is incomplete; unpaired: mem_gwz_sspi; no import ref was created; nothing was written`. The `--target` set leaves gwz-sspi out, and the refusal came anyway; a default merge refuses the same way.

**Reason.** gwz-core's import library (`gwz_local_import::pair_participants`) required the two workspaces' member sets to match "whatever this verb selected", so a member only the receiving lock records was reported unpaired even when the selection left it out. Under the default selection the family wrapper also selected it for the import, and the delegated merge would have planned it and looked in it for an import ref the lane cannot supply.

**Ruling.** The operator: "this is a bug, it should allow merging".

**Reproduction.** gwz-core `src/local_clone/tests/family_merge/receiver_only.rs`: the root registers `extra` (`mem_extra`) after its lane `A` was cloned, and merges from `A` with the default selection, with `--target @root --target mem_app`, with targets naming `extra`, and through a `README` conflict with `--abort` and `--continue`. All four tests failed on gwz-core `21f9e15e` with `PairingMismatch` (`unpaired: mem_extra`).

**Fix.** gwz-core `dca4a162`, with docs in gwz-cli `220c89a` (lane `fix-family-pairing`):
- a member only the receiving lock records is left out of the import and of the merge when only the default or `@all` reached it. It gets no import ref; its HEAD, worktree and lock row are unchanged; it is never a participant of the merge record, so status, continue, abort and gc never visit it. The merge's summary message reports it as `<path> (<id>): not in source lane; unchanged`;
- a selection that names it, by id or path, refuses `PairingMismatch` before any fetch, saying the source lane has no such member;
- a member only the lane has, and a member recorded at different paths or with a different `source_id`, still refuse as before.

The rule is the 2026-10-04 amendment in `GwzLocalCloneDesign.md` §6 and in gwz-core `GWZDesign.md` and `GWZRequirements.md`.

**Limits.** gwz's human output prints the engine's participant table and not the summary message, so the "not in source lane; unchanged" line is visible in `--json` output (`meta.message`) and through the Python API, and the member is simply absent from the human table. Not yet released: an installed 1.0.17 still refuses; use a build with the fix.
