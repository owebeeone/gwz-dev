# R4b Linux durable-identity capability spike

Date: 2026-08-12

Status: **interface decision accepted; execution workflow authored; native
evidence required before R1**.

## Decision

The first Linux implementation admits checked mutation only on a local ext4
filesystem when every capability below succeeds at runtime. It does not infer
support from the distribution name, `statfs`, `st_dev`, `f_fsid`, a successful
build, or `name_to_handle_at` alone.

The admitted tuple is:

```text
LinuxDurableIdentityV1 {
    provider: FsIocGetFsUuid,
    filesystem: Ext4,
    filesystem_uuid: nonzero 16-byte external UUID,
    handle_type: positive i32 returned by
                 name_to_handle_at(retained_fd, "", AT_EMPTY_PATH),
    persistent_file_handle: 1..=128 bytes,
}
```

The runtime must provide `FS_IOC_GETFSUUID`. In the upstream kernel this means
Linux 6.9 or later; a distribution backport is admitted by the successful
capability call rather than by a `uname` exception. The ioctl result is the
external filesystem UUID, requires `fsuuid2.len == 16`, and requires all 16 UUID
bytes to be nonzero as a set. The provider then calls
`name_to_handle_at(retained_fd, "", ..., AT_EMPTY_PATH)` on the already opened
no-follow object handle and requires a positive handle type and a handle no
larger than 128 bytes. `AT_SYMLINK_FOLLOW`, `AT_HANDLE_FID`, and any nonempty
path/path-relookup fallback are forbidden.

The filesystem type is obtained from the retained mount, not from the path
string. The first support table contains ext4 only. XFS, Btrfs, F2FS, overlay,
network, FUSE, tmpfs, and every unknown filesystem are unsupported until each
has its own reviewed UUID, persistent-handle, path-equivalence, remount, and
fault evidence. A successful syscall on an unlisted filesystem does not admit
it.

This deliberately narrows the first safe provider. It is preferable to a
compatibility fallback whose reboot identity is inferred from mount-local
numbers. A pre-6.9 compatibility provider may be added later, but it must be a
separate provider with its own external-UUID derivation and evidence; it may
not fall back to `f_fsid` or mount ID.

## Path equivalence

For every retained ext4 parent, the provider reads `FS_IOC_GETFLAGS` and maps
`FS_CASEFOLD_FL` to `AsciiCaseFold`; absence maps to `Sensitive`. Failure to
read the flag is unsupported. Checked protocol components are ASCII, so this
does not claim to reproduce ext4's Unicode lookup algorithm.

The mode is captured component by component and persisted with the durable
parent identity. A mode change, parent replacement, filesystem UUID change,
handle change, or unsupported flag result is ambiguity or unsupported before
mutation as specified by the caller's stage; it never selects a new family.

## Typed failure mapping

Before private hierarchy creation, authority publication, or managed mutation:

- unavailable `FS_IOC_GETFSUUID`, non-ext4, overlay, network, FUSE, tmpfs,
  zero UUID, unsupported handle, handle overflow, or unavailable casefold
  query returns `UnsupportedOperation`;
- substitution or mismatch against already persisted identity/path-mode facts
  returns the checked protocol's ambiguity/recovery-required result; and
- an ordinary I/O failure that does not prove unsupported capability remains
  `IoError`.

No unsupported row may publish residue in order to remember that it was
unsupported.

## Required pre-R1 evidence

R1 cannot begin until both Linux release architectures have a fail-not-skip
native probe on the exact implementation tuple. The probe creates an ext4
filesystem with a fixed external UUID, mounts it, creates sensitive and
casefolded parent cases where supported, and records:

- kernel release and architecture;
- filesystem type and `FS_IOC_GETFSUUID` result;
- object UUID, handle type, and handle length;
- per-parent casefold result; and
- the ordinary mount ID as non-authoritative diagnostic evidence.

It persists the expected tuple outside the mounted filesystem, unmounts and
remounts the same filesystem, and must reacquire the identical UUID, handle
type, handle bytes, and component modes while allowing the mount ID to differ.
The test then performs the checked observer's identity/path-mode comparison.
The workflow fails rather than skips if loop mounting, ext4 creation, the
ioctl, the handle, or casefold setup needed by a declared row is unavailable.

A separate negative table must prove typed pre-mutation rejection for overlay,
tmpfs, a synthetic network provider, zero/no UUID, handle overflow, unknown
handle provider, and an injected mode-query failure. The generated provider
support table and these expected rows are compared for exact equality.

The evidence artifact records the exact core commit, workflow run, probe source
digest, provider-table digest, both architecture tuples, and the remount
results. A release build is not this evidence.

The executable gate lives in `gwz-core/scripts/linux_identity_probe/` and
`.github/workflows/linux-identity-probe.yml`. The workflow runs the portable
contract first, then performs privileged fail-not-skip native probes on
`ubuntu-24.04` (`linux-x86_64`) and `ubuntu-24.04-arm` (`linux-aarch64`). Its
aggregate job accepts exactly those two rows with one commit/run/source/table
binding and publishes `linux-durable-identity-evidence`. Record the successful
run and downloaded aggregate digest here before assigning R1.

The same-invocation provider test retains object `A`, renames/replaces its old
pathname with object `B`, and then performs the empty-path descriptor query. It
must still return `A`'s handle. Negative rows cover missing `AT_EMPTY_PATH`,
permission denial, unsupported empty-path lookup, `AT_SYMLINK_FOLLOW`,
`AT_HANDLE_FID`, non-16-byte UUID length, and any attempted pathname fallback.

## Activation consequence

Until another provider is reviewed, a Linux workspace outside the admitted
ext4 capability receives a clear pre-mutation `UnsupportedOperation` for the
affected checked merge transition. Documentation at activation must state the
runtime capability, not merely say "Linux supported". The v1 decoder and
dispatcher remain disabled while this remediation is under construction.

Primary interface references:

- Linux UAPI `FS_IOC_GETFSUUID`: <https://github.com/torvalds/linux/blob/master/include/uapi/linux/fs.h>
- Linux exportable-filehandle model: <https://docs.kernel.org/filesystems/nfs/exporting.html>
- `name_to_handle_at(2)` limitations: <https://man7.org/linux/man-pages/man2/open_by_handle_at.2.html>
