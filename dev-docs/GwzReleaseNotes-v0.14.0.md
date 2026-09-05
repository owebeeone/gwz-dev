# GWZ v0.14.0 — release notes

Status: **FINAL 2026-09-05**, assembled by the implementation lane from the
released tree: gwz-core `c67e0d4` (tag `v0.14.0`), gwz-cli `79b53f6`
(tag `v0.14.0`, pins gwz-core `v0.14.0`), gwz-py `5bc4a03` (tag `v0.14.0`,
pins gwz-core `v0.14.0`). Published as the gwz-core GitHub release body at
the tag.

Two things to know before reading further. First, GWZ now has **one merge
implementation**: every way of starting a merge writes the same record and
follows the same path. Second, a merge now **runs on every filesystem GWZ can
write a file on**, warning where it cannot also record crash-recovery
evidence, instead of refusing.

The single implementation has an upgrade cost, and it is the first section
below. Read it before you install 0.14.0.

## Before you upgrade: close any open merge

**Finish or abort any in-progress merge while you are still on 0.13.x.** On
0.13.x, run `gwz merge --status`; if a merge is open, close it with
`gwz merge --continue` or undo it with `gwz merge --abort`. Only then install
0.14.0.

0.14.0 cannot act on a merge record written by 0.13.x or earlier. If one is
still open when you upgrade, every merge verb — and every command a merge
blocks, including `gwz commit`, `gwz add` and `gwz stage` — refuses with one
sentence and changes nothing:

```text
gwz: OpenOperation: this is a pre-0.14 merge; use gwz 0.13.0 (the last release before 0.14) to continue or abort
```

`--status`, `--continue` and `--abort` all refuse alike, so the usual
open-merge advice does not apply and is deliberately not printed. Nothing is
damaged and nothing needs repairing. There is no conversion step and no
in-place upgrade for an open record.

**If you have already upgraded with a merge open:** install GWZ 0.13.0 — the
last release before 0.14.0, and the version the refusal names — close the merge
with it, then return to 0.14.0. Releases are published at
<https://github.com/owebeeone/gwz-cli/releases>; install a pinned version by
substituting the tag for `latest` in the installer URL, for example
`https://github.com/owebeeone/gwz-cli/releases/download/v0.13.0/gwz-installer.sh`
(`.ps1` on Windows). Do not delete the record, and do not use plain
`git merge --abort` as a substitute — neither rolls back what the coordinated
merge changed.

**Merges you already finished are unaffected.** Archived records still project
read-only through `gwz merge --status <merge-id>`, and `gwz merge --gc` still
never deletes an archive it cannot read.

## Changed: one merge implementation

- **Ordinary, `--ff-only`, custom-message (`-m`) and `--no-ff` merges all write
  the same record and take the same path.** Until now `--no-ff` used one record
  format and everything else used another. For you this is mostly consistency:
  `gwz merge --status`, `--continue`, `--abort`, `gwz stage` and recovery
  behave identically however the merge was started, and the crash-recovery
  behaviour below now applies to every merge rather than to `--no-ff` alone.
- **The older format's engine is no longer in the binary.** It is not selected,
  not written, and not read for an open merge — which is why the upgrade step
  above exists. The reader for *archived* records of the old format stays, so
  history written before 0.14.0 is still readable.
- **There is no migration between formats.** A merge interrupted while it was
  finishing could previously be switched from one format to the other in
  flight. That step is gone along with the second format: there is only one
  format to be in.

## Changed: crash recovery is a capability of the volume, not a gate

- **A merge no longer refuses because of the filesystem.** In 0.13.0 a
  `--no-ff` merge start refused outright on any volume that could not record
  crash-recovery evidence — on Linux that was ext4 and nothing else, so
  Fedora's btrfs and RHEL's xfs both refused. The merge now starts, runs and
  completes there.
- **One warning says what you lose.** GWZ probes the workspace volume once, at
  merge start, before it takes any lease or writes anything. Where the volume
  cannot prove a durable filesystem identity it prints one line on stderr and
  continues:

  ```text
  warning: crash recovery is unsupported on btrfs (no durable filesystem identity). Merge will continue. Use --filesystem-strict to refuse.
  ```

  The filesystem is named, or `unknown` when it cannot be named. The
  parenthetical is exactly one of `no durable filesystem identity`,
  `remote filesystem`, or `volatile filesystem`. The same rule applies on
  Linux, macOS and Windows.
- **What is absent is the crash-recovery evidence, not merge safety.** Below
  the bar — that is, on a volume that could not prove durable identity — the
  merge runs with the same participants, the same order, the same
  verification and the same published evidence. What is not recorded is the
  evidence an *interrupted start* would have used to prove on restart which
  artifacts it had created. Ctrl-C, `gwz merge --abort` and
  `gwz merge --continue` of an open merge never needed it and behave
  identically. Below the bar crash recovery is **absent, not degraded**: a
  power loss mid-merge there is manual cleanup, because nothing on that volume
  can prove after a reboot what the interrupted attempt created.
- **`--filesystem-strict` restores the refusal.** Add it where an
  unrecoverable interrupted merge is unacceptable: the merge is refused before
  any lease, record, or Git mutation. It is a start-only flag — supplying it
  with `--continue`, `--abort`, `--status` or `--gc` is rejected as an invalid
  request. There is no environment variable and no configuration key; the flag
  is the whole control surface.

## Changed: the bar is durable identity, not a filesystem name

- **xfs and f2fs now get crash recovery.** The Linux check asks the volume for a
  nonzero 16-byte UUID through `FS_IOC_GETFSUUID` and a persistent handle
  through `name_to_handle_at`, and admits whatever answers. ext4, xfs and f2fs
  answer alike. The 0.13.0 superblock magic-number test for ext4 is gone, and
  `--filesystem-strict` does **not** bring it back.
- **btrfs does not, and says so.** btrfs never publishes its filesystem UUID to
  the kernel's VFS, so the call answers `ENOTTY` and the volume is below the
  bar. The gap reported is `no durable filesystem identity`.
- **Kernels before 6.9 have no such call at all**, so every Linux volume is
  below the bar there. The warning still names the filesystem correctly.
- **tmpfs and ramfs are below the bar as volatile.** They do answer both
  syscalls — tmpfs publishes a random per-mount UUID — but their contents do
  not survive power loss, so recovery evidence written on them cannot be
  trusted after a crash. A merge on tmpfs still runs; the gap reported is
  `volatile filesystem`.
- **Network mounts are named as remote.** NFS, SMB/CIFS, SSHFS and other FUSE
  mounts fail the identity probe; the gap is `remote filesystem`. This is a
  *reason*, not a hidden list of filesystem names.
- **macOS and Windows are unchanged in behaviour**: a local APFS or HFS+ volume
  is above the bar on macOS, NTFS on Windows.

## Changed: merges run on volumes without persistent file handles

A few volumes are below the bar and additionally cannot identify a file across
a rename — **overlay filesystems mounted without `nfs_export`, and some FUSE
mounts**. In 0.13.0 a `--no-ff` merge refused on those, as it did on every
volume that could not record crash-recovery evidence. It now completes: the
record is written by a different durable path — still staged, renamed, flushed
and read back to verify its bytes, and still refusing to overwrite an existing
record — and no crash-recovery evidence is recorded, exactly as on any other
volume below the bar.

There is no second warning. The one above gains a clause:

```text
warning: crash recovery is unsupported on overlay (no durable filesystem identity). Merge will continue. Use --filesystem-strict to refuse. Selected-root and --preserve abort may refuse until the workspace is on a handle-capable volume.
```

That clause is the remaining limit, and it falls on **abort**, not on start. A
plain abort still clears the record, because it re-verifies nothing. An abort
that must re-verify something — a selected root's manifest and lock, a
preservation bundle under `--preserve`, or the merge's published evidence —
refuses on such a volume. One escape works: copy the whole workspace onto a
volume that proves handles (a local APFS or HFS+ volume on macOS; ext4, xfs or
f2fs on Linux; NTFS on Windows) and run `gwz merge --abort` there, adding
`--preserve` if that was the door that refused. Do not delete the record in
place, and do not treat `git merge --abort` as a stand-in — neither rolls back
the root, the bundle or the published evidence.

## Fixed

Every defect below affected merges started with `--no-ff`, and only those.
On 0.13.x `--no-ff` was the one mode that used the newer record path; every
other way of starting a merge took the older one and was unaffected. If you
never passed `--no-ff`, none of these reached you.

- **A conflicted `--no-ff` merge no longer makes the workspace unusable.** In
  0.13.0, once a `--no-ff` merge was open, `commit`, `add`, `push`, `pull`,
  `capture`, `materialize`, `snapshot`, `stash`, `tag`, `branch`, `forall`,
  repository mutation and a second `merge` all failed with
  `UnsupportedRecordVersion: … use a compatible newer GWZ` — advice the user
  had already taken, since 0.13.0 was the newest release and was what wrote the
  record. `add` could stage nothing, so `merge --continue` then refused with
  `conflict resolution is not ready`, and the only way out was
  `merge --abort`, which threw the merge away. Ordinary merges were unaffected
  throughout, which is what made this hard to recognise. Every one of those
  commands now reads an open record of either format, and a conflicted
  `--no-ff` merge resolves and continues to an integration commit.
- **A refused `gwz merge --continue` now changes nothing.** In a multi-repository
  merge, `--continue` checked each repository's staged resolution as it reached
  it, so a repository whose staged work was wrong could be discovered only after
  an earlier repository had already been committed and its merge state cleared —
  a "refused" continue that had half applied itself. The whole selected set is
  now checked before any of it is applied: if one repository is not ready, the
  continue refuses naming that repository, no ref moves, no native merge state
  is cleared, and the record does not advance.
- **`gwz merge --abort --preserve` no longer discards an edited conflict.**
  Where a conflict had been re-created while resuming a merge, GWZ recorded the
  file's *current* bytes as its original conflict snapshot. A later
  `--abort --preserve` then compared the file against a snapshot derived from
  that same edited file, concluded the conflict was untouched, rolled it back
  and reported success — with the user's edit gone and no preservation evidence
  written for that repository at all. GWZ now records "original unavailable" in
  that case and refuses the preserving abort instead, leaving the file alone.
- **`gwz merge --abort --preserve` no longer refuses over an up-to-date
  repository.** A merge whose selection included a repository that was already
  up to date failed the preserving abort with
  `successful participant is missing from the preservation owner set`. An
  already-up-to-date repository changed nothing and is now treated as such.
- **A merge that failed while assembling the workspace root can be aborted
  again.** When a merge stopped because the merged root's manifest or lock was
  invalid — a redefined member identity, an unsupported schema, a path escaping
  the workspace — the failure was reported as a generic "accepted manifest
  bytes are invalid" with no repository named, and the resulting record then
  blocked `gwz merge --abort` outright with `operation drift prevents
  coordinated rollback entry`. The three causes are told apart again and name
  the offending target, and the merge stays abortable, with the record still
  saying why it stopped. Separately, a merge that failed *before* the workspace
  root was accepted refused every abort with `publication state is not an exact
  reversible handoff`; such a merge now aborts.
- **A completed `--no-ff` merge reports its work.** It printed
  `participants: total 0` and emitted no per-repository outcome on `--jsonl`.
  Completed merges now carry the per-repository rows, counts, publication step
  and preservation summary that ordinary merges always did.

## Changed: the machine surface

- **`merge.crash_recovery`** is a new optional object on the merge response,
  carrying `supported`, the `filesystem` name, the `gap` when identity could
  not be proved (`NoDurableIdentity`, `RemoteFilesystem`, `VolatileFilesystem`),
  and `handles_ok`. It is the machine truth for the decision: `--json`,
  porcelain and `--jsonl` consumers read it there and never parse stderr.
  Present means a decision was made, not that crash recovery is available; read
  `supported` for that. A `false` decision is a success response, not a failure.
- **`handles_ok`** reports whether the volume proves the persistent file handles
  the previous section describes: `false` on a below-bar volume whose handles do
  not work, so a consumer can tell that a selected-root or `--preserve` abort
  may refuse there; `true` on a below-bar volume whose handles do work; `null`
  above the bar. It is an additive field and there is no protocol version bump.
- **How absence renders.** `filesystem`, `gap` and `handles_ok` are always
  present as keys and carry an explicit `null` when the decision has no value
  for them. Only the `crash_recovery` object as a whole is omitted, on a
  response that made no such decision: `--abort`, `--status`, `--gc` and dry
  run.
- **A `Diagnostic` event kind** carries the warning text with
  `severity: "Warn"` on the `--jsonl` stream. Human output prints it once per
  invocation; JSON and porcelain print nothing, because the response carries it.
- **`source_version` is `V1` for every merge this build writes**, so `V0` now
  appears only on an archived record written before 0.14.0. An open pre-0.14
  record is refused rather than projected, so no response carries it.
- **Some structured error codes for ordinary merges moved** to match what
  `--no-ff` already returned, now that both take the same path. Where a
  recovery or preservation check fails, `MergeDrift` is replaced by the specific
  code for the check that fired — `RecoveryEvidenceMismatch`,
  `MergeRecoveryRequired` or `PreservationEvidenceMismatch`; an unreadable
  *archived* record answers `ArchivedRecordUnreadable` rather than
  `MergeRecordUnreadable`; and a `--preserve` abort blocked by a staged
  resolution answers `DirtyMember` rather than `MergeRecoveryRequired`.
  Consumers that branch on merge error codes should re-check those branches.
- GWZ is pre-1.0: consumers that already tolerate additive keys need no change.

## Notes

- **The crash-recovery decision is not recorded.** A workspace moved between
  volumes while a merge is open re-decides at `--continue`, on the volume it
  finds. Same volume, same answer. Moved onto a volume above the bar, recording
  begins mid-attempt, which is harmless; moved onto one below the bar, the
  continue proceeds without it and warns.
- **The warning is per invocation.** A workspace on btrfs sees one line per
  start and one per continue. Suppressing it across invocations would need
  persistent state or configuration, and neither is in this release.
- **The Python client matches the CLI.** `gwz-py` refuses a pre-0.14 open
  record with the same sentence and exposes `handles_ok` on the same object.
- **`gwz merge --gc` now says what survives it.** Collecting an archived merge
  reports the per-repository rows and the preservation summary, so the backup
  refs and stashes that outlive the collected archive are named rather than
  merely left behind. Which archives are retained, and the refusal to delete an
  archive it cannot read, are unchanged.

## Known, deferred

- **A selected-root or `--preserve` abort on a volume without persistent file
  handles refuses**, with the escape given above. Plain abort, and starting and
  completing a merge, work there.
- **Crash recovery is unavailable below the bar and is not partially
  reconstructed.** There is no graded evidence and no per-attempt reporting of
  how much was recorded: it is recorded or it is not.
- **Windows admits NTFS by name** rather than by asking the volume whether it
  can open a file by id.
