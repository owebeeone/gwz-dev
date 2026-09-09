# ReFS sharing check — 2026-09-09

## Finding

On Dabeest's E: ReFS volume (4,096-byte clusters), released GWZ v1.0.8 and
Windows `CopyFileW` shared the same logical byte ranges in all eight tested
file sizes. Complete clusters were shared; a trailing partial cluster was
private. There is no evidence from this check that refcopy's finishing steps
undo sharing. Small-file reference counts of one do not establish that a
native clone failed.

This is a measured result for this Windows/ReFS configuration, not a promise
about every filesystem, sparse file, integrity-stream setting, or file size.
The largest test file was 1 MiB + 3 bytes; the multi-gigabyte chunk boundary was
not exercised.

## Method and results

Created a fresh, independent GWZ root with two members through the pinned pilot
fixture builder. Added eight random-content payload files and flushed them to
disk. Ran `gwz local clone sharing <destination>` using the released executable
at `E:\gwz-agent-eval\tools\gwz.exe`. No subsequent GWZ operation ran on that
lane before querying it. The clone reported 114 native files, zero ordinary.

Queried each source and destination using:

```powershell
fsutil file queryExtentsAndRefCounts "E:\path\to\file"
```

Then copied the same source files using Windows `CopyFileW`, and queried those
destinations. Checked both reference counts and source/destination logical
cluster mappings; matching contents alone were not the sharing test.

| File length, bytes | GWZ shared logical bytes | Windows copy shared logical bytes |
| ---: | ---: | ---: |
| 1 | 0 | 0 |
| 4,095 | 0 | 0 |
| 4,096 | 4,096 | 4,096 |
| 4,097 | 4,096 | 4,096 |
| 65,536 | 65,536 | 65,536 |
| 65,537 | 65,536 | 65,536 |
| 1,048,576 | 1,048,576 | 1,048,576 |
| 1,048,579 | 1,048,576 | 1,048,576 |

Shared logical bytes exclude padding beyond EOF. Private logical tail bytes
can consume an entire allocation cluster. Windows' one-byte copy reported no
allocated extent; GWZ's one-byte destination reported one private cluster.
Thus equal shared-byte results do not imply identical small-file allocation.

All GWZ and Windows-copy contents matched their sources before the write test.
Immediately after GWZ cloning, shared extents had reference count 2. Windows
copy increased those counts to 3 where it shared the same source clusters.

## Finishing-step investigation

The current Windows path in `gwz-core/crates/refcopy/src/native.rs` pre-sizes
the destination to the source's logical length, then submits cluster-rounded
`FSCTL_DUPLICATE_EXTENTS_TO_FILE` ranges. It does not truncate the file after
duplication. `ordinary.rs::finish_clone` applies permissions and renames it.

A separate direct-API probe reproduced pre-sizing and rounded duplication for
six sizes, then queried extents before close, after close, after clearing the
read-only attribute, after rename, and after hashing. Sharing of complete
clusters survived every stage. The partial tail was already private before
close; it did not become private because of the later metadata operations.
Some before-close results included allocation beyond EOF; the analysis clips
logical ranges to file length and uses after-close measurements for comparison.

Changed one byte in the GWZ lane's 1 MiB payload and flushed the write. The
first cluster became private (reference count 1); the remaining clusters stayed
shared (reference count 3). The source's SHA-256 remained unchanged. Before
flushing, the extent query could still show the old sharing state, so a durable
write/flush is necessary when checking this transition.

## Consequences

- Do not change block-copy arithmetic based on the earlier small-file samples.
- Native-copy counts describe the mechanism used, not files that retain shared
  allocation. Storage savings should be measured in shared bytes/extents.
- The NTFS-only persistent-identity admission gate and the CRLF-related
  `MergeDrift` are separate outstanding issues; this check fixes neither.

## Evidence

Remote directory:
`E:\gwz-agent-eval\qualification\sharing-20260909-163152`.
It retains the fixture, Windows/direct-API controls, `gwz-clone.txt`, stage-by-stage
`results.json`, and `summary.json` including the flushed write observation.
Probe script: `E:\gwz-agent-eval\qualification\sharing-probe.py`;
local copy: `scratch/refs-sharing/probe.py`. Raw output stays outside version control.
The allocation/mapping summary and final flush check were additional read/check
steps after that probe; their results are saved in `summary.json`.
