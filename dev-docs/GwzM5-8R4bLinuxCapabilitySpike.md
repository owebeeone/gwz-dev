# R4b Linux durable-identity capability spike

Date: 2026-08-12

Status: **R0-L evidence remediation implemented; authoritative native rerun
and independent acceptance are pending**.

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
binding and publishes `linux-durable-identity-evidence`. The successful run and
downloaded aggregate digest are recorded below.

## Superseded first execution

GitHub Actions run
[`31536272593`](https://github.com/owebeeone/gwz-core/actions/runs/31536272593)
completed successfully against core commit
`808e64a21a36d5e0bbe5b4400cd5975eef5cff97`.

The downloaded aggregate is retained byte-for-byte as
`GwzM5-8R4bLinuxIdentityEvidence-Run31536272593.json` with SHA-256
`46357d09608205eac0b0868181d0bc9c6d643c2e5dbd0d7c8fedd51648184b57`.
It binds:

- probe source SHA-256
  `04a5cc39cf6437783a350fdff19375cafe045c346ebabddd045397c01b40a9f7`;
- provider-table SHA-256
  `5eb3679744e641b4f718cb86b7ea92f71238ed4a69802d1f058d7496d8017aaa`;
- `linux-x86_64` and `linux-aarch64` on kernel `6.17.0-1020-azure`;
- the fixed external UUID `718c918e3cc343c9ae9e27f5cecc8a17`;
- positive handle type `1` and eight-byte persistent handles on both
  architectures;
- identical UUID, handle type/bytes, and sensitive/casefold modes across each
  unmount/remount cycle;
- descriptor-bound substitution, missing-`AT_EMPTY_PATH`, and forbidden
  flag/path-fallback checks; and
- exact `UnsupportedOperation` results for all eight negative support rows,
  including real overlay and tmpfs mounts.

Mount IDs happened to remain stable within both runs, but the artifact records
them only as non-authoritative diagnostics and the equality gate excludes them.
An independent review found that the native positive observations were
credible, but the aggregate validator did not reject deleted or falsified
tuple, query, substitution, and mount-authority facts. It also found that the
executed negative table omitted permission denial, unsupported empty-path
lookup, and a malformed nonzero UUID length. This run is therefore useful
positive evidence but is not the R0-L gate and does not permit R1 to begin.

The remediated gate binds both rows to the aggregate job's exact commit and run,
recomputes source and provider-table digests, checks the native machine for each
declared architecture, validates every field in a closed schema, and rejects
missing, unknown, or false evidence. Its exact 15-row negative table now
executes the omitted typed query outcomes and malformed UUID case. The two
native architectures must be rerun and the resulting artifact independently
accepted before this status changes to complete.

## Remediated execution awaiting independent acceptance

GitHub Actions run
[`31537368340`](https://github.com/owebeeone/gwz-core/actions/runs/31537368340)
completed successfully against the hardened gate at core commit
`11d0817657939cdc995a3696b6606b7f3e7e6614`. The contract, native x86-64,
native AArch64, and aggregate jobs all passed.

The downloaded canonical artifact is retained byte-for-byte as
`GwzM5-8R4bLinuxIdentityEvidence.json` with SHA-256
`5ebddb1141576816609c2fe1e8e65b163d75ba9a3e4001852e0a6ed7baff313c`.
It binds probe-source SHA-256
`fbf2de6426dc728af8db8c6444eeeff7c1bbc4bde61b0ee4d576a43492a2311d`
and provider-table SHA-256
`5eb3679744e641b4f718cb86b7ea92f71238ed4a69802d1f058d7496d8017aaa`
to the exact commit, workflow run, and declared/native architecture pairs.

Both native rows record the fixed external UUID, positive type-1 eight-byte
handles, complete sensitive/casefold path modes, descriptor-bound substitution,
and equal durable tuples across remount. They also record the exact closed query
contract and all 15 negative outcomes, including `IoError` for injected
permission denial and `UnsupportedOperation` for injected unsupported empty-path
lookup. Mount IDs remain diagnostic and non-authoritative. Independent review
must confirm that this remediated run closes P1-1 and P2-1 before R0-L is marked
complete or R1 begins.

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
