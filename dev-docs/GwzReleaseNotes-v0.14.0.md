# GWZ v0.14.0 — release notes (DRAFT)

Status: **DRAFT 2026-09-03**, assembled by the implementation lane from the
DR-1 ship (1) charter (`GwzM5-8DR1-WarnOrRefuse-Charter.md`) and the landed W1,
W2 and W5 branches. **No release is cut by this document. No tag of any kind is
created by this document.** The charter's standing instruction is "Cut no tag of
any kind, including 0.13.1, without asking"; this draft exists so the release
body is written from the tree rather than from memory when the operator asks
for one. Wording tied to W3 (the core decision point and its remedy string) and
W4 (the driver flag, help text and JSON) is reconciled by the lane owner at
landing; the operator's warning sentence and the flag's name and semantics are
fixed.

One theme: **a `gwz merge` now runs on every filesystem GWZ can write a merge
record on.** Crash recovery became a capability of the volume instead of a gate
in front of the command.

## Changed: crash recovery is a capability, not a gate

- **`gwz merge --no-ff` no longer refuses because of the filesystem.** In
  0.13.0 a `--no-ff` merge start refused outright on any volume the checked
  artifact catalog would not admit — on Linux that was ext4 and nothing else,
  so Fedora's btrfs and RHEL's xfs both refused. The merge now starts, runs and
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
- **What is actually absent is the crash-recovery evidence, not merge safety.**
  Below the bar the merge runs with the same participants, the same order, the
  same verification and the same published composition evidence. What is not
  recorded is the catalog evidence an *interrupted start* would have used to
  prove on restart which artifacts it had created. Ctrl-C, `gwz merge --abort`
  and `gwz merge --continue` of an open merge never needed it and behave
  identically.
- **`--filesystem-strict` restores the refusal.** Add it where an
  unrecoverable interrupted merge is unacceptable: the merge is refused before
  any lease, record, or Git mutation. It is a start-only flag — supplying it
  with `--continue`, `--abort`, `--status` or `--gc` is rejected as an invalid
  request. There is no environment variable and no configuration key; the flag
  is the whole control surface.

## Changed: the bar is identity, not a filesystem name

- **xfs and f2fs now get crash recovery.** The Linux gate asks the volume for a
  nonzero 16-byte UUID through `FS_IOC_GETFSUUID` and a persistent handle
  through `name_to_handle_at`, and admits whatever answers. ext4, xfs and f2fs
  answer alike. The 0.13.0 superblock magic-number test for ext4 is gone, and
  `--filesystem-strict` does **not** bring it back.
- **btrfs does not, and says so.** btrfs never publishes its filesystem UUID to
  the kernel's VFS, so the ioctl answers `ENOTTY` and the volume is below the
  bar. The gap reported is `no durable filesystem identity`.
- **Kernels before 6.9 have no such call at all**, so every Linux volume is
  below the bar there. The warning still names the filesystem correctly.
- **tmpfs and ramfs are refused as volatile.** They do answer both syscalls —
  tmpfs publishes a random per-mount UUID — but their contents do not survive
  power loss, so recovery evidence written on them cannot be trusted after a
  crash. A merge on tmpfs still runs; the gap reported is
  `volatile filesystem`.
- **Network mounts are named as remote.** NFS, SMB/CIFS, SSHFS and other FUSE
  mounts fail the identity probe; the gap is `remote filesystem`. This is a
  *reason*, not a hidden list of filesystem names.
- **macOS and Windows are unchanged in behaviour**: a local APFS or HFS+ volume
  is above the bar on macOS, NTFS on Windows. Replacing the Windows name test
  with the `FILE_SUPPORTS_OPEN_BY_FILE_ID` capability flag is a named follow-up
  and is not in this release.

## Changed: the machine surface

- **`merge.crash_recovery`** is a new optional object on the merge response,
  carrying `supported`, the `filesystem` name when it can be named, and the
  `gap` when it is not supported (`NoDurableIdentity`, `RemoteFilesystem`,
  `VolatileFilesystem`). It is the machine truth for the decision: `--json`,
  porcelain and `--jsonl` consumers read it there and never parse stderr. The
  object is absent on any response that made no such decision — `--abort`,
  `--status`, `--gc`, dry run, and every start that writes a v0 record. Present
  means a decision was made, not that crash recovery is available; read
  `supported` for that. A `false` decision is a success response, not a failure.
- **A new `Diagnostic` event kind** carries the same warning text with
  `severity: "Warn"` on the `--jsonl` stream. Human output prints it once per
  invocation; JSON and porcelain print nothing, because the response carries it.
- GWZ is pre-1.0: consumers that already tolerate additive keys need no change.

## Known limit: `--no-ff` still needs persistent file handles

A `--no-ff` merge record is published through the checked artifact boundary,
which needs persistent file handles even though it needs no volume UUID. A
filesystem that cannot supply them at all — **overlayfs without `nfs_export`,
sshfs and other FUSE mounts without export support** — still refuses a
`--no-ff` merge when the record is written, with the boundary's own message
rather than the warning above. That set is much smaller than 0.13.0's refusal
set, but it is not empty, and this release does not claim otherwise. Ordinary
and `--ff-only` merges are unaffected and write v0 records as before. Lifting
this limit is chartered separately.

## Unchanged

- The catalog's on-disk format, the v1 merge record contract, and the checked
  artifact boundary and its protected sources.
- Ordinary and `--ff-only` merges: they write v0 records and never reach this
  door.
- `gwz merge --abort` still needs no special filesystem when it touches no
  checked artifact. An abort that must re-verify checked artifacts —
  preservation bundles, a selected root's manifest and lock, or the merge's
  published evidence — still needs persistent file handles and a mount
  identity, as in 0.13.0.
- `gwz repo create`, `init-from-sources`, `gwz merge --status`, GC, and the
  workspace mutation guard never reach the catalog.
- No protocol version bump: the request flag, the response object and the event
  kind are additive.

## Notes

- **The decision is not recorded.** A workspace moved between volumes while a
  merge is open re-decides at `--continue`, on the volume it finds. Same
  volume, same answer. Moved onto an above-bar volume a catalog is bootstrapped
  mid-attempt, which is harmless; moved onto a below-bar volume the continue
  proceeds without one and warns.
- **The warning is per invocation.** A workspace on btrfs sees one line per
  start and one per continue. Suppressing it across invocations would need
  persistent state or configuration, and neither is in this release.
- **The Linux identity release gate (R0-L) was rebased on the same contract.**
  It no longer admits ext4 by name; it records the filesystem and tests the
  evidence, keeps its 15-row negative table with every verdict intact — tmpfs
  and overlay now refused on real mounts by real syscall answers — and
  additionally builds xfs on a loop device and reproduces its identity across a
  remount, on both release architectures.
