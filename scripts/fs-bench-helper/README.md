# GWZ filesystem benchmark helper

A Linux C++20 utility for one disposable **10 GiB** XFS/ext4 image. It does the
privileged formatting, mounting and teardown. The benchmark source, N local
clones, and file edits run as the ordinary user inside the mounted filesystem.

The Pi build is restricted to real UID/GID **1000/1000** (`gianni`). Root may also
invoke it, but mounted filesystem ownership always goes to that compiled-in
user. Other real UIDs are refused, including callers with effective UID zero.

## Commands

```text
gwz-fs-bench create xfs
gwz-fs-bench create ext4
gwz-fs-bench status
gwz-fs-bench destroy
```

There are no path, device, size, mount-option, or extra formatter arguments.
`create` only formats a newly created image; it refuses an existing image or
mount. **`destroy` discards every file inside this benchmark image.** It unmounts,
detaches the matching loop device, verifies detachment, and removes the image.
Busy mounts are retained; there is no force/lazy-unmount option.

For UID 1000 the fixed paths are:

- Root-owned image: `/var/lib/gwz-fs-bench-1000/image.img`, mode 0600.
- User-owned mounted filesystem: `/var/lib/gwz-fs-bench-1000/mnt`, mode 0700.
- Root-owned parent and lock remain after destruction; the 10 GiB image does not.

XFS is formatted with `reflink=1`. Both filesystem types are mounted
`nodev,nosuid,noexec,noatime`. Keep the GWZ executable outside the mount.
The source workspace must be **inside the mounted filesystem**, alongside its
local clones, to exercise same-filesystem CoW.

## Build and inspect

Requires Linux 5.9+, a C++20 compiler, `xfsprogs`, and `e2fsprogs`.
The Pi already has these dependencies after the integration run.

```sh
./build.sh "$(id -u)" "$(id -g)" ./gwz-fs-bench
./gwz-fs-bench --help
sha256sum ./gwz-fs-bench
```

`build.sh` enables strict warnings, PIE, stack protection, fortified libc calls,
and full RELRO. It never installs or sets privilege bits.

## Bless the tested Pi binary

The tested source and ordinary executable are on the Pi at
`/home/gianni/git/gwz-fs-bench-helper-20260907/`. The reviewed binary was installed on 2026-09-07 as root:gianni mode 4750,
and its status command was verified without sudo. To install a reviewed rebuild,
run this **on the Pi**:

```sh
sudo install -o root -g gianni -m 4750 \
  /home/gianni/git/gwz-fs-bench-helper-20260907/gwz-fs-bench \
  /usr/local/sbin/gwz-fs-bench
```

This sets root ownership and the setuid bit together; it does not set setgid.
Keep the installed file and its parent directory unwritable by ordinary users.
Simply applying `chmod +s` to a user-owned executable does not make it root.

Then, as `gianni`, without sudo:

```sh
/usr/local/sbin/gwz-fs-bench create xfs
# Populate /var/lib/gwz-fs-bench-1000/mnt and run N ordinary-user local clones.
/usr/local/sbin/gwz-fs-bench status
cd ~
/usr/local/sbin/gwz-fs-bench destroy
/usr/local/sbin/gwz-fs-bench create ext4
# Repeat with the same source, clone count and edits.
/usr/local/sbin/gwz-fs-bench status
cd ~
/usr/local/sbin/gwz-fs-bench destroy
```

Close shells/processes using the mount before `destroy`. A failed/interrupted
create can leave an image; `destroy` handles ordinary incomplete setup too.
Unexpected ownership, links, loop associations or geometry are refused and
retained for administrator inspection. The helper does not recursively delete
host paths. It can also be invoked with sudo without setting the setuid bit.

## Measurements

`status` emits JSON with filesystem `used_bytes` and `available_bytes`, plus
`image_bytes` and `image_allocated_bytes` for the sparse backing file.
It calls `syncfs` for this filesystem and flushes the image before measuring.
Subtract the empty formatted baseline to measure growth from N clones and edits.
Guest filesystem allocation and actual allocated blocks in the sparse image
are different measurements; retain both. Do not sum `du` per clone to estimate
unique shared extents. Time the GWZ clone command separately from `status`'s
flush. The helper does not drop the host's caches or alter other workloads.

## Privileged scope

- Fixed paths beneath root-owned, non-writable parent directories; no symlink
  traversal for managed files/directories; raw image and lock must be root-owned
  private regular files with one hard link.
- One nonblocking exclusive lock serializes helper operations. The caller's
  environment and extra file descriptors are discarded; core dumps disabled.
- Only two fixed root-owned system formatter executables are invoked, without
  a shell and with fixed arguments/environment. The formatter inherits pinned
  loop/lock descriptors so parent termination cannot release/reassign its device.
- `LOOP_CONFIGURE` attaches only the newly created image with a fixed size limit
  and autoclear. Teardown checks device/inode identity, geometry, flags and the
  mounted device; it never operates on a caller-supplied block device.
- Host mount/user/PID namespaces only. Mount IDs distinguish an unmounted
  directory from a mount, including same-device bind mounts.
- Ordinary unmount and confirmed detachment precede unlinking. A loop still
  referenced elsewhere leaves its image intact.

API references: [Linux loop device API](https://man7.org/linux/man-pages/man4/loop.4.html),
[mount flags](https://man7.org/linux/man-pages/man2/mount.2.html), and
[ordinary unmount](https://man7.org/linux/man-pages/man2/umount.2.html).

## Integration checks

Run as the authorized user with working sudo, before installing setuid:

```sh
python3 test_helper.py --disposable-image-tests "$PWD/gwz-fs-bench"
```

This is an explicit privileged test: it creates/formats/destroys this helper's
fixed disposable image. It refuses to start if an image or mount already exists.
It does not enable setuid. Data writes and copies run as the ordinary user;
`setpriv` exercises authorized and unauthorized real/effective UID combinations.

The Pi passed **20 checks** on 2026-09-07: both filesystem mounts and teardown,
XFS reflink success/ext4 reflink refusal, independent edits, busy-unmount
retention, existing-image refusal, environment isolation, linked-image refusal,
metadata permissions, mountpoint symlink/nonempty-directory refusal, incomplete-image
cleanup and caller restrictions. The test images were removed.
These are helper correctness checks, not the N-clone performance benchmark.
The local log is `/tmp/gwz-fs-bench-tests.log`.

## Review follow-up

The September 7 review prompted three hardening changes:

- Directory emptiness is checked through the already validated directory FD,
  including hidden entries. Mount targets use that pinned FD through procfs.
- Unmount resolves `mnt` relative to the pinned root-owned parent directory and
  uses `UMOUNT_NOFOLLOW`. The mounted-directory FD is closed first so it does not
  itself prevent ordinary unmount. This still relies on the root-only parent:
  it is not intended to defend against a concurrent privileged administrator
  replacing mounts or directory entries.
- Formatter children restore default INT/TERM/HUP/QUIT/PIPE dispositions before
  exec. The parent retains its existing signal policy while waiting for mkfs.
  The integration suite exercises normal formatting; it does not fault-inject
  termination during formatter execution.

Three other findings do not establish the claimed vulnerabilities in this design:

- Ownership of files inside the mounted filesystem does not grant raw-image
  access. The backing file remains root-owned 0600 under a root-only writable
  parent; new images are created exclusively, formatted by the trusted system
  formatter, and never reused by `create`. Ordinary raw-image access/removal is
  tested. Kernel filesystem bugs remain a host-kernel risk, as with other local
  writable filesystems; this utility is not a sandbox for hostile users or disk
  images. A VM is appropriate if that is the required threat model.
- The formatter deliberately inherits the loop FD and lock. These pin the device
  association and serialize operations if the parent dies. Passing only a
  `/dev/loopN` name would lose that protection. The fixed root formatter is in
  the trusted computing base regardless of these descriptors.
- Loop allocation uses atomic `LOOP_CONFIGURE`, bounded collision retries, RAII
  descriptor ownership and `LO_FLAGS_AUTOCLEAR`. Contention can cause a safe
  refusal. Unmounted associations clear when their last reference closes;
  mounted or externally held devices can remain busy. Failed creation retains
  the fixed image for explicit `destroy`, rather than unlinking possibly busy
  backing storage. Cleanup of an incomplete zero-length image is tested.

The review's introductory claim about dropping capabilities is also inaccurate:
this helper normalizes its identity to root and runs a trusted root formatter.
The restrictions above constrain its interface; they do not remove its privilege.
