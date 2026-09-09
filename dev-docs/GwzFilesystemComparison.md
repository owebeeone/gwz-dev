# Pi local-clone comparison: XFS versus ext4

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

On 2026-09-07, XFS used approximately 96% less incremental filesystem space
than ext4 for ten GWZ local clones. Across two rounds each, XFS took 24.79 seconds
for 20 clones versus ext4's 37.99 seconds (35% less elapsed time). Median clone
latency was 1.150 seconds versus 1.265 seconds (9% less).

Those timings were measured on a debug build and are superseded. The same
benchmark re-run on the released `gwz 1.0.8` binary reproduced every space
figure and cut XFS elapsed time by 43.9%, widening the gap to 62.7%. See
[Release-build verification](#release-build-verification-2026-09-09).

| Measurement | XFS | ext4 |
| --- | ---: | ---: |
| Median clone, 20 observations | 1.150 s | 1.265 s |
| First ten-clone batch | 11.59 s | 19.93 s |
| Second ten-clone batch | 13.20 s | 18.06 s |
| Additional filesystem usage, ten clones | 20.50 MiB / 20.50 MiB | 537.19 MiB / 537.15 MiB |
| Additional backing-image allocation, ten clones | 0.23 MiB / 0.49 MiB | 537.26 MiB / 537.23 MiB |

## Method

- Raspberry Pi 8 GB, ARM64, Linux 6.12.62+rpt-rpi-2712. Both filesystems used a
  fresh sparse 10 GiB loop image on the same ext4 host storage.
- Round order: XFS, ext4, ext4, XFS. Ten sequential clones per round, retained
  together until measurements completed; forty successful clones total.
- Each round copied the same existing `gwz-pi-acceptance-20260907` workspace into
  the mounted filesystem before timing. This source has no active local family.
  Both source and destinations reside on the tested filesystem.
- Each clone contains eight repositories, approximately 46.5 MB of logical file
  content, 3,107 files and 515 directories. The source occupies about 54 MiB.
- Timed operation: `gwz --root <source> local clone benchN <laneN> --json`, including
  family bookkeeping and destination Git object verification. The first clone
  founds the family. Source copying, formatting and teardown are excluded.
- All XFS first-clone files were reported natively copied; ext4 reported ordinary
  copying. The preceding helper tests independently confirmed XFS reflink support
  and ext4 reflink refusal.
- Space deltas subtract the populated-source baseline from the ten-clone state.
  Measurements flush the mounted filesystem and backing image through helper
  `status`. Timer completion itself does not add a separate explicit flush.
- No cache dropping or other workload changes. These are normal cached runs on
  a shared machine, not controlled cold-cache storage benchmarks. Both filesystems
  had occasional timing spikes; the median improvement is smaller than the total.

The native ARM64 GWZ executable is the previously accepted development build at
`/home/gianni/git/gwz-pi-build-20260907/debug/gwz`, SHA-256
`9784c943a60d9f4e6415cb3a650907084ea6f9fd3c884ff24593e41dff859c4f`.
These numbers do not measure an optimized release build or large build artifacts.

## Interpretation

XFS's clear benefit here is shared clone storage. Clone time also includes object
verification (around 0.7 seconds in the first-clone logs), limiting how much faster
CoW copying makes the whole command on this small source.

The backing-image allocation delta is not the complete clone space cost: XFS can
consume space already allocated in the image, including metadata space. Use the
20.50 MiB guest filesystem delta as well as the physical image delta; do not
interpret 0.23 MiB as the total storage required for ten clones. Empty-filesystem
layout and allocation differ between XFS and ext4, so these are incremental
measurements rather than comparisons of format overhead.

This exercise measures clone creation and retained size, not subsequent edit
amplification, merge speed, or raw native XFS partition performance.

## Evidence and cleanup

[Raw measurements](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/filesystem-comparison/runs/fs-comparison-20260907/results.json),
[exact Pi benchmark script](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/filesystem-comparison/runs/fs-comparison-20260907/benchmark.py), and forty
per-clone JSON responses are retained alongside this report. The script is a
record of this run with fixed Pi paths; its output-directory exclusivity prevents
accidental reruns over existing evidence.

The Pi also retains evidence at `/home/gianni/git/gwz-fs-comparison-20260907`.
Each round destroyed only its disposable benchmark image. Final helper status
confirmed no image and no mount. The original source workspace was only read.

## Single-clone follow-up

A separate run on the same day measured exactly one clone per fresh image, again
in XFS/ext4/ext4/XFS order. Each clone founded a new family. Source preparation,
flushes, binary and mount settings were unchanged.

| Measurement | XFS | ext4 |
| --- | ---: | ---: |
| Additional filesystem space, first run | 2.098 MiB | 53.723 MiB |
| Additional filesystem space, second run | 2.098 MiB | 53.723 MiB |
| Clone time, first run | 1.213 s | 1.396 s |
| Clone time, second run | 1.268 s | 1.223 s |

One clone therefore used 96.1% less additional filesystem space on XFS. Timing
overlaps: these two observations per filesystem do not establish a meaningful
single-clone speed advantage. The practical result for this approximately 54 MiB
source is about 2.1 MiB extra on XFS versus 53.7 MiB extra on ext4, with either
clone completing in roughly 1.2–1.4 seconds.

Backing-image growth was 0.297/0.027 MiB on XFS versus 53.734 MiB on ext4;
the earlier caution about consuming previously allocated image space applies.
All four disposable images were destroyed after measurement.
[Single-clone raw results](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/filesystem-comparison/runs/fs-single-comparison-20260907/results.json)
and the exact script and per-clone responses are retained beside them.

## Release-build verification (2026-09-09)

The 2026-09-07 numbers above were measured on a debug build, which that method
note flags. On 2026-09-09 the whole benchmark was re-run on the same Pi against
the released binary: `gwz 1.0.8`, installed from the README one-liner to
`/home/gianni/.cargo/bin/gwz`, SHA-256
`4d23cda364cfaa50fdb4c74192d1956f24b5407dd909f49107d1a4856406e729`. Source
workspace, helper, round order, clone count and timing method were unchanged;
only the binary path and the output directory differ from the original script.

**The space results reproduce; the timing results do not.** Every space figure
landed on the published value, and the reflink behaviour was identical — all
twenty XFS clones reported 3,107 files copied natively and zero ordinarily, all
twenty ext4 clones the reverse. The elapsed-time figures are superseded: the
release build is much faster on XFS and barely faster on ext4.

| Measurement | XFS | ext4 |
| --- | ---: | ---: |
| Median clone, 20 observations | 0.687 s | 0.811 s |
| First ten-clone batch | 6.95 s | 18.13 s |
| Second ten-clone batch | 6.94 s | 19.09 s |
| Additional filesystem usage, ten clones | 20.50 MiB / 20.50 MiB | 537.15 MiB / 537.19 MiB |
| Additional backing-image allocation, ten clones | 0.24 MiB / 0.23 MiB | 537.23 MiB / 537.27 MiB |

On the release binary XFS took 13.90 seconds for 20 clones against ext4's 37.21
seconds, **62.7% less elapsed time** rather than the 35% measured on the debug
build. Median clone latency was 0.687 s against 0.811 s, 15.3% less. Additional
filesystem usage was 96.18% less, matching the published 96%.

| Change, debug → release | XFS | ext4 |
| --- | ---: | ---: |
| 20-clone elapsed | 24.79 s → 13.90 s (43.9% faster) | 37.99 s → 37.21 s (2.1% faster) |
| Median clone | 1.150 s → 0.687 s | 1.265 s → 0.811 s |

The asymmetry is the point. A reflink clone moves metadata, so optimizing the
binary removes most of what it was spending; an ext4 clone writes 46.5 MB per
invocation, so the same optimization is absorbed by the storage. ext4's median
improved while its total did not, because its cost has moved into a heavy tail:
six of twenty ext4 clones exceeded two seconds (worst 7.30 s) against a 0.74 s
floor, while no XFS clone left the 0.67–0.74 s band. Quote the totals rather
than the medians for ext4; the median now understates it.

### Why the 0.23 MiB image figure is not the storage cost

The original report cautioned against reading the backing-image delta as the
space ten clones need. A separate flush-sensitivity round confirms that caution
and supplies the mechanism. Measuring XFS image allocation immediately after the
tenth clone gave 0.23 MiB; an explicit `sync` left it at 0.23 MiB; fifteen
seconds later it was **21.53 MiB**. ext4 read 537.20 MiB at all three points.

XFS therefore defers this allocation past an explicit flush, so the method note
above — that helper `status` flushes the mounted filesystem and backing image —
holds for ext4 but not for XFS metadata. The settled XFS figure converges on the
20.50 MiB guest-filesystem delta, which is the number to quote. Raw values are in
[flushcheck.json](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/filesystem-comparison/runs/fs-comparison-20260909/flushcheck.json).

Forty per-clone JSON responses, [raw measurements](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/filesystem-comparison/runs/fs-comparison-20260909/results.json),
the [re-run script](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/filesystem-comparison/runs/fs-comparison-20260909/benchmark.py) and the
[flush-check script](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/filesystem-comparison/runs/fs-comparison-20260909/flushcheck.py) are retained.
Both disposable images were destroyed; final helper status confirmed no image and
no mount, and the source workspace was again only read.
