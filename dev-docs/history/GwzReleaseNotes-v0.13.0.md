# GWZ v0.13.0 — release notes

Status: FINAL 2026-09-02, assembled by the implementation lane from the landed tree (gwz-core `5ae6df7`, gwz-cli `69f2723`), `GwzOverClaimingCommitDiagnosis.md`, `GwzParameterPlumbingAudit.md`, `GwzDryRunClassFix-Review.md` and `GwzM5-8R2E-E4-Close.md`. Published as the gwz-core GitHub release body at the tag.

A roll-up of the merge lane's Phase E4 and the workspace-command fixes. Every
release up to and including 0.12.1 is affected by the defects under "Fixed";
upgrade before relying on `--target` or `--dry-run` with `gwz add`,
`gwz commit`, `gwz stash`, `gwz forall` or `gwz init`.

## Changed: `--no-ff` merges run on the checked artifact boundary

- **The checked artifact catalog is activated on a `--no-ff` merge start.**
  The first merge record (`gwz merge <source> --no-ff`) is now written through
  the checked artifact boundary: an authority record is pinned to the leaf's
  durable filesystem identity and the publication is verified against it. An
  interrupted `--no-ff` start therefore recovers or refuses on exact evidence
  instead of on file names.
- **This needs a filesystem with persistent file identity.** On Linux the
  catalog admits **ext4 only** for now; on macOS local APFS or HFS+; on Windows
  NTFS. On any other filesystem a `--no-ff` merge start refuses with a typed
  error naming the remedy; ordinary merges, `commit`, `repo create`,
  `init-from-sources`, `abort` and `--gc` do not depend on the filesystem and
  keep working. An open merge can be cleared with `gwz merge --abort`, which
  needs no such filesystem unless it must re-verify checked artifacts (a
  preservation bundle, a selected root's manifest and lock, or the merge's
  published evidence). A new merge can be started without `--no-ff`.
- **An adaptation that would need the catalog is declined, not wedged.** When
  an interrupted-finalization upgrade cannot activate the catalog, the merge
  completes on the v0 lifecycle instead of stranding.
- The remaining raw writers of the merge lifecycle (the merge record's rewrite
  path and the finalization's marker, lock and boundary writes) stay on plain
  atomic renames, documented and pinned in the tree; their conversion is
  deferred to a design round.


## Fixed

- **`gwz add -A --target <member>` and `gwz commit -a --target <member>` no
  longer select every repository.** The `-A`/`-a` flags collided with the
  global `--all` selector inside the argument parser, so an explicit `--target`
  was silently widened to `@all` — the cause of whole-workspace staging sweeps
  that pulled untracked root files into member commits.
- **`gwz commit --target <member>` no longer creates a root commit.** Since
  v0.8.0 every marker-enabled commit (the default) also committed the workspace
  root over its whole index, unannounced. The root is now committed only when it
  is selected (the default selection still includes it); member-scoped commits
  keep the GWZ trailers, write no marker artifact, and leave the refreshed lock
  staged in the root.
- **Root commits are reported.** When the root is committed it appears as an
  `@root` row in both human and `--json` output, including root-only commits.
- **`gwz add <pathspec> --target <t>` honours the selection.** A pathspec whose
  owning repository lies outside the selection is refused; fan-out into
  unselected repositories is dropped.
- **`--dry-run` mutates nothing on `add`, `commit`, `snapshot`, `capture` and
  `tag`.** These five verbs parsed and delivered the flag but never read it;
  `gwz --dry-run commit` performed a real commit. They now validate and plan,
  then return before the first write. The same fix reaches the Python client's
  `dry_run=True`.

## Notes

- `gwz merge --gc` now collects merges archived under the v1 record envelope
  (every `--no-ff` merge, and any ordinary merge an interrupted-finalization
  upgrade adapted to v1), which previously could never be collected. Ordinary
  retention applies to them too: a workspace holding more than 20 archives
  begins retiring its oldest v1 archives on the next merge that archives. No
  backup ref, stash or bundle is deleted; an unreadable archive is never
  deleted.
- `gwz merge --abort` needs no special filesystem when it touches no checked
  artifact; an abort that must re-verify checked artifacts (preservation
  bundles, a selected root's manifest and lock, or the merge's published
  evidence) needs persistent file handles and a mount identity.

- **`--dry-run` on `stash apply|pop|drop`, `forall` and `init` mutates
  nothing.** Found by the 2026-09-02 parameter-plumbing audit
  (`GwzParameterPlumbingAudit.md`): `gwz --dry-run stash drop` deleted the
  stash, `--dry-run stash apply` restored the tree, `--dry-run forall` ran the
  command, and `--dry-run init` created the whole workspace (`.git`, `.gwz`,
  `gwz.conf/`, the agent bootstrap files). `--dry-run forall` now prints the
  command and the targets it would run in. The guard seam that let
  handlers ignore the flag now carries it by type.
- **`--all` means the same thing under every verb: select all workspace
  targets.** Under `add` and `commit` the long spelling `--all` used to be
  double-booked with the git-style flags, so `gwz --all commit -m …` silently
  behaved as `commit -a` and `gwz --all add` as `add -A`. One spelling now has
  one owner: `--all` (in either position) is the `@all` selector, and the
  git-style flags are the short forms only — **`gwz add -A`** stages
  everything in the selected targets, **`gwz commit -a`** stages tracked
  modifications first. **Scripts that used `gwz add --all` or
  `gwz commit --all` for the git-style meaning must switch to `-A` / `-a`.**
  In human output, `gwz commit --all` prints a one-line note on stderr pointing
  at `-a`, so the changed meaning is never silent. A test now asserts that no
  subcommand argument reuses a global argument id, so this class of collision
  cannot recur.

## Known, deferred (recorded in `GwzParameterPlumbingAudit.md`)

- `--partial`, `--force` and `--sync` are accepted by many verbs that do not
  read them; `materialize --branch` ignores `--force` where `materialize --lock`
  honours it; `tag`'s protocol `all` field is unused; the Python client accepts
  `--ssh-timeout` without transmitting it and still widens `--all` on `add`.
  These become explicit refusals or honoured flags in a following release.
