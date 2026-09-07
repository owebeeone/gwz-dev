# Pi local-clone comparison: XFS versus ext4

On 2026-09-07, XFS used approximately 96% less incremental filesystem space
than ext4 for ten GWZ local clones. Across two rounds each, XFS took 24.79 seconds
for 20 clones versus ext4's 37.99 seconds (35% less elapsed time). Median clone
latency was 1.150 seconds versus 1.265 seconds (9% less).

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

[Raw measurements](evidence/fs-comparison-20260907/results.json),
[exact Pi benchmark script](evidence/fs-comparison-20260907/benchmark.py), and forty
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
[Single-clone raw results](evidence/fs-single-comparison-20260907/results.json)
and the exact script and per-clone responses are retained beside them.
