# ARM64 EBADF substrate class — diagnosis and repair

Package scope: the `ubuntu-24.04-arm` platform-matrix leg's 1094/266/1
failure mass, recorded in `GwzWindowsMatrix-Classification.md`
("Platform-leg attribution CORRECTION"). Diagnosed and repaired at
gwz-core `c13f773`; all cited line numbers re-verified against that
tree (the record's numbers were drift-free). Native validation lands
on the next platform-matrix dispatch (lane owner).

## Verdict

The class is real, but its recorded attribution is wrong in one
load-bearing way: the failing descriptors are **`O_PATH` by
construction in cap-std 4.0.2 on every Linux system**, not a property
of "the runner's default mount", and not ARM-specific. The
`ubuntu-24.04-arm` leg is simply the only leg anywhere in CI that runs
the full Rust suite on Linux (platform-matrix.yml runs macos-14 +
ubuntu-24.04-arm; release.yml verifies ubuntu-22.04-x86, but no
released tree contains this substrate — `v0.10.5` has neither cap-std
in its lockfile nor `src/checked_artifact/platform.rs`). The suite's
Linux fsync/ioctl-on-directory seams had **never executed on any Linux
host** before this leg ran, and they fail there deterministically,
independent of kernel version, filesystem, or architecture. The repair
is therefore not an errno reclassification but a descriptor repair:
obtain a real descriptor from the capability at the exact seams that
need one. No allowlist changes; EBADF remains a hard error everywhere.

## Substrate mechanism

Two independent facts compose into the recorded signature.

**1. cap-std 4.0.2 opens every directory capability `O_PATH` on
Linux.** Verified in the vendored sources this workspace builds
against (`cap-primitives-4.0.2`):

- `open_ambient_dir_impl` (rustix/fs/dir_utils.rs:105-119) hard-codes
  `O_DIRECTORY | target_o_path()`, and `target_o_path()` is `O_PATH`
  on Linux (dir_utils.rs:121-152).
- Checked opens (`Dir::open_dir`, `open_dir_nofollow`) route through
  `compute_oflags` (rustix/fs/oflags.rs:42-51), which adds `O_PATH`
  for every `dir_required` open that is not `readdir_required` and not
  writable — i.e. every plain directory-capability open. Both the
  `openat2` fast path and the `manually::open` fallback consume the
  same flags (linux/fs/open_impl.rs `open_beneath`), so the property
  holds regardless of `openat2` availability, seccomp, or kernel.

**2. The kernel's descriptor lookup discriminates by operation
class.** Syscalls that *consume* the file (fsync, ioctl, read/write)
resolve the descriptor through the lookup that excludes `FMODE_PATH`
files, so they report `EBADF` before any filesystem code runs.
Traversal-class syscalls (fstatfs, fstat/statx, `name_to_handle_at`
with `AT_EMPTY_PATH`, `openat`-family with the fd as anchor, dup,
fcntl) accept `O_PATH` descriptors by design. That is the recorded
diagnostic signature verbatim: `fstatfs` succeeds (so `require_ext4`
passes and reports genuine ext4) on the very descriptor whose
`FS_IOC_GETFSUUID` ioctl and whose dup'd `fsync` report `EBADF`. The
`EBADF` here is produced at descriptor-table lookup and reveals
nothing about the mount; on a real (non-`O_PATH`) descriptor the same
kernel and mount carry both operations — which is why the raw-syscall
Linux identity probe (`linux-identity-probe.yml`) is green on the same
ubuntu-24.04-arm runner class.

Why the other legs never showed it: macOS cap-std uses no `O_PATH`
(real descriptors; run-13 macOS is 1359/0/1), and Windows
`sync_parent` is a documented no-op under the write-through doctrine.

## Per-site disposition (all: O_PATH by construction, not environment)

| Site (at `c13f773`) | Operation | Count | Descriptor origin |
| --- | --- | ---: | --- |
| `src/checked_artifact/observation.rs:44` | `sync_parent` → fsync of dup | ×79 | `Dir::open_ambient_dir` root + `open_dir_nofollow` components (both `O_PATH`) |
| `src/checked_artifact/transition.rs:409-413` | `sync_parent` → fsync of dup ("sync managed parent directory") | ×13 | managed `Dir` capabilities (checked opens, `O_PATH`) |
| `provider/platform/linux.rs:126` | `FS_IOC_GETFSUUID` ioctl in `filesystem_uuid` via `dir_identity` | ×50 | `Dir` capabilities (`O_PATH`); `file_identity` takes a real `File` descriptor and was never affected |
| `provider/platform/linux.rs:54-58` (latent) | `FS_IOC_GETFLAGS` ioctl in `parent_mode` | — | same class; plausibly the record's 143rd direct panic (itemized counts sum to 142); fixed in the same stroke |

The remaining ~123 recorded failures are downstream cascades of these.

**Same-class twin, deliberately untouched:**
`src/git/gitbackend/preservation_root/parent.rs:250-252` (`sync_dir`:
`dir.try_clone()...sync_all()`, `#[cfg(unix)]`). It did not appear as
a direct site in the ARM classification — consistent with its callers
dying earlier inside the checked-artifact seams — but once those
clear, preservation-root publication flows will reach it and it will
report the same `EBADF` on Linux. It belongs to the preservation-root
lane's subsystem and is not in this package's blast radius; the recipe
is identical to `sync_parent` below (reopen `.` through the
capability, fsync the reopened descriptor).

## The fix (platform-scoped, no new primitive, no record change)

Principle: the substrate *supports* every one of these operations on a
real descriptor for the same directory object; only the descriptor
kind is wrong. So the fix hands the kernel the right descriptor
instead of teaching error paths to forgive the wrong one.

- `src/checked_artifact/platform.rs` — `sync_parent` gains a
  Linux-specific variant: `openat(dir, ".",
  O_RDONLY|O_DIRECTORY|O_CLOEXEC)` through the capability, then
  `fsync` the reopened descriptor. Non-Linux unix keeps the dup+
  `sync_all` body; Windows stays the documented no-op. The durability
  barrier now *executes* on Linux — it is repaired, not downgraded.
- `provider/platform/linux.rs` — new private `descriptor_query_fd`
  helper (same reopen), used by `dir_identity` and `parent_mode`
  before their descriptor-consuming ioctls. `rename_domain` and the
  metadata read stay on the capability descriptor (traversal-class,
  `O_PATH`-safe); `require_ext4` and `persistent_handle` run on the
  reopened descriptor via their callers' shared query path
  (behaviorally neutral — corrected per the package review [P3-1]).
  `file_identity` is untouched. `query_error` (linux.rs:173-181) is
  **byte-identical**: the EOPNOTSUPP/ENOSYS/ENOTTY/EINVAL downgrade
  allowlist did not grow.

The reopen resolves `.` through the descriptor itself — no stored
path, no re-resolution, no sandbox escape surface — so it names the
same directory object even under concurrent renames (pinned by a
dev/ino-equality test). On kernels ≥ 6.8 with ext4 the UUID ioctl now
*succeeds* (durable identity becomes real on the ARM leg for the first
time); on older kernels it reports `ENOTTY` on the real descriptor and
takes the pre-existing, documented `unsupported` downgrade.

## Masking analysis (why this shape, and why not the recorded lead)

**Blanket-adding EBADF to the `query_error` allowlist is rejected.**
(a) A closed or recycled descriptor — a genuine lifecycle defect —
also reports `EBADF`; the allowlist cannot distinguish it from the
`O_PATH` condition, so the downgrade would classify real bugs as
graceful capability absence. (b) It would not even fix two of the
three sites: `sync_parent` does not route through `query_error`, and
an analogous downgrade there would silently no-op the crash-durability
barrier on every Linux mount — the worst available masking. (c) It
downgrades capabilities the substrate demonstrably has.

**The brief's alternative shape ("verify fd validity via fstatfs
before classifying EBADF as unsupported") is also rejected**: it still
cannot make `fsync` execute on an `O_PATH` capability, so it converts
the durability barrier into a verified no-op, and it adds an
errno-interpretation branch where none is needed.

**The chosen shape cannot mask:** there is no errno interpretation
anywhere in it. A dead descriptor fails the reopen itself with a hard
`EBADF` I/O error; a descriptor number recycled onto a non-directory
fails the `O_DIRECTORY` reopen with a hard `ENOTDIR`; a directory
without read permission fails hard with `EACCES` (fail-closed; read
permission is intrinsic to holding a real directory descriptor, and
before this fix the same call hard-failed with `EBADF` on all of
Linux, so no previously-working input regresses). The
recycled-onto-another-directory corner is unreachable through the
typed seams — a live `&Dir`/`&File` owns its descriptor and Rust's I/O
safety forbids it closing mid-borrow — and both raw-seam behaviors are
pinned by tests. Ambiguity is nowhere downgraded; the `unsupported`
path remains reserved for substrates that genuinely lack a capability.

## Test inventory (all Linux-compiled only; first native run = next dispatch)

`provider/platform/linux.rs` (in-gate: `checked_artifact::capability::pre_catalog`):

1. `capability_directory_descriptors_are_traversal_only_on_linux` —
   substrate sentinel: the capability descriptor carries `O_PATH`
   (F_GETFL). Flips only if cap-std changes doctrine; revisit the seam
   with it.
2. `descriptor_query_fd_names_the_same_directory_object` — dev/ino of
   the reopened descriptor equals the capability's (anti-TOCTOU pin).
3. `descriptor_query_fd_carries_descriptor_consuming_operations` —
   fsync succeeds on the reopened descriptor.
4. `a_dead_descriptor_still_fails_closed_with_ebadf` — a
   race-free-unallocatable descriptor number (≥ RLIMIT_NOFILE) reports
   hard `Io`/`EBADF`, never `Unsupported`.
5. `a_recycled_non_directory_descriptor_still_fails_closed` — a file
   descriptor through the seam reports hard `Io`/`ENOTDIR`.
6. `query_error_downgrades_the_documented_capability_refusals` —
   `ENOTTY` → `Unsupported` (the pre-6.8-kernel real-descriptor path).
7. `query_error_keeps_bad_descriptors_as_hard_io_errors` — `EBADF` →
   hard `Io`: the allowlist provably did not grow.
8. `dir_identity_never_reports_ebadf_for_a_live_capability` — the ×50
   site's regression pin, substrate-independent.
9. `parent_mode_never_reports_ebadf_for_a_live_capability` — the
   latent fourth surface's pin.

`src/checked_artifact/platform.rs` (`checked_artifact::platform::linux_tests`,
outside the pre_catalog gate filter, runs under full `cargo test`):

10. `sync_parent_flushes_a_live_linux_directory_capability` — the ×79
    and ×13 sites' regression pin.

## Local evidence and its limits (macOS host)

- Native macOS: scoped rustfmt clean; clippy clean for this package's
  files under `-D warnings`; `cargo test --lib
  checked_artifact::capability::pre_catalog` green (73 = 54 baseline +
  19 concurrent leaf-lane additions at run time; this package adds
  none that compile on macOS).
- Linux compile validation: the full-crate
  `--target x86_64-unknown-linux-gnu` check is blocked by C-native
  deps (libz-sys/libgit2-sys need a Linux sysroot); the
  `aarch64-unknown-linux-gnu` toolchain is not installed on this host.
  Instead, a seam-mirror crate (scratchpad `linux-seam-check`)
  compiles the exact new Linux-only constructs — `sync_parent`,
  `descriptor_query_fd`, `query_error`, and the test module except
  tests 8-9 (not mirrored; residual compile risk carried by the
  ubuntu clippy lane at landing — review [P3-2]) —
  against the locked versions (cap-std 4.0.2, cap-fs-ext 4.0.2, rustix
  1.1.4, libc 0.2.189) for `x86_64-unknown-linux-gnu`, `--all-targets
  --offline`: clean. Every construct used is arch-generic across
  x86_64/aarch64 Linux (no arch-specific libc structs or ioctl
  encodings introduced).
- The `O_PATH`/`fget` kernel semantics and the cap-std flag
  computation are verified from sources (vendored cap-primitives;
  kernel descriptor-lookup rules), and the green ARM identity probe is
  existing native evidence that real descriptors on that exact runner
  class carry these operations.

## Expected ARM matrix outcome

- The 143 recorded direct `EBADF` panics clear: ×79 observation.rs,
  ×13 transition.rs, ×50 linux.rs:126, plus the latent
  `parent_mode` surface. The ~123 cascades should clear with them.
- Ten new Linux-only tests execute natively for the first time
  (first-dispatch class per program history; test 1 and 3-5 are
  kernel-semantics pins and low-risk, tests 8-9 are
  substrate-conditional by design).
- Named residue risks, in expected order of likelihood:
  1. `preservation_root/parent.rs:250` — the untouched same-class twin
     becomes reachable once upstream seams stop failing; any
     preservation-root publication test that reaches its unix barrier
     on Linux will report the old signature there. Recipe as above;
     preservation-root lane.
  2. First-true-execution effects: with `FS_IOC_GETFSUUID` now
     *succeeding* on the runners' ext4 + 6.8 kernel, durable-identity
     and rename-domain code paths run for real on Linux CI for the
     first time; downstream assertions in that territory have never
     been natively evidenced on Linux and may surface ordinary
     first-dispatch findings unrelated to this class.
- Not affected by this package: the two standing Windows tripwires,
  and all green legs (the fix is `target_os = "linux"`-scoped;
  macOS/Windows binaries are byte-identical in behavior).
