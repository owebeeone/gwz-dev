# Filesystem capability gate fix — 2026-09-10

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

Windows implementation and validation are complete. No release or installation
was performed. Existing workspace changes were preserved.

## Admission and compatibility

`gwz-core/src/filesystem/native/facts/windows.rs::facts` now queries volume
capability flags independently of the optional diagnostic name. It requires
`FILE_SUPPORTS_OPEN_BY_FILE_ID`, a successful nonzero 128-bit FileIdInfo query
and a local volume GUID. Case-mode and checked-artifact handle checks remain.
The admission helper propagates unsupported and operational I/O errors distinctly.
Ordinary merges retain warning behavior; filesystem-strict retains refusal.
The WindowsNtfsFileId128V1 and WindowsNtfs stored representations are unchanged.

## Audit and remaining work

- Windows facts: removed `require_ntfs`; filesystem_name is called only by
  describe_volume, which discards diagnostic failure. No ReFS allowlist.
- Linux facts: `classify_volume` feeds describe_volume, then
  `checked_artifact/capability/pre_catalog/provider/platform.rs::describe_volume`,
  then `checked_artifact/entry.rs::crash_recovery_decision_in`. The latter returns
  Supported before consulting remote/volatile labels. These name-derived labels
  select warning wording only, not support or fallback.
- **Remaining admission violation:**
  `gwz-core/src/filesystem/native/facts/linux.rs::identity` calls
  `refuse_volatile_filesystem` (lines 231–241), which rejects TMPFS_MAGIC and
  RAMFS_MAGIC. This pre-existing type gate remains. Removing it without a
  replacement durability contract could admit volatile storage that returns a
  per-mount UUID and persistent handles. Designing that replacement is separate
  work; the whole-codebase capability-only policy is not complete.
- macOS facts use operation probes and MNT_LOCAL, not a filesystem name gate.
  Descriptive names and the test asserting the host's APFS/HFS label are not
  production admission decisions.
- Checked capability code maps OS support profiles to historical wire labels;
  durable_identity validates values, not reported filesystem names.
- `gwz-core/crates/refcopy/src/native.rs`: macOS clonefile and Linux FICLONE
  fallbacks depend on operation results; Windows checks
  FILE_SUPPORTS_BLOCK_REFCOUNTING and duplicate-extents results. Filesystem
  names in comments/messages do not control the outcome. Arithmetic unchanged.
- Existing standalone conditional attributes elsewhere were not migrated.
  This patch adds no conditional imports or unbraced conditional declarations.
  Rust parsing/format checks inspect the edited Windows source even on macOS;
  broader existing-code scope-rule migration is not claimed.

## Validation

All native Windows fixtures are under D:/gwz-tests/capability-gate or
E:/gwz-tests/capability-gate. No C: fixtures were used. Source was streamed to
Dabeest without target directories; builds were on D:. On the Mac only a cached
cargo check was run; no local clone or full build was created.

- Red: the new admission test failed against the old implementation with the
  expected missing admission helper. The old name-gate defect was also directly
  confirmed by source inspection.
- D: NTFS and E: ReFS: 3/3 Windows fact tests passed on each, covering the flag,
  zero identity, unsupported and I/O probe errors, native reopen, rename,
  replacement identity and directory case query. The name is absent from the
  admission helper's inputs, including when diagnostics are unavailable.
- D: and E:: all 99 checked-artifact capability tests passed on each, including
  injected interruption/restart convergence and stable-slot fault matrices.
- D: and E:: all 85 g23 merge/recovery tests passed on each. Initial runs caught
  one obsolete NTFS wording assertion; it was updated and both suites rerun.
- D: and E:: strict CLI clean merge, conflict-paused process exit followed by
  continue in a new process, and abort in a new process passed. The runner checks
  supported recovery, merge ancestry, final file bytes and no open merge record.
  These are process-restart checks, not forced process kills or power-loss tests.
- Windows core test build passed with --locked. The outer CLI snapshot's existing
  lockfile did not match its path dependency graph; cargo build --offline updated
  only that disposable copy's lockfile and passed. Workspace lockfiles on the Mac
  were not changed by this fix.
- macOS: cargo check -p gwz-core --lib --locked passed using the existing cache.
- rustfmt parse/format checks, diff whitespace checks and the existing filesystem
  source-boundary check passed. Linux native compilation was not run; WSL is
  available but has no discovered Rust toolchain.

Raw logs and the reusable CLI runner are in
[evidence/capability-gate-20260910](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/filesystem-capability/runs/capability-gate-20260910).
Windows build/source/fixture directories remain on Dabeest for review.
