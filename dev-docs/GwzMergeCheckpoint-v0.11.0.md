# GWZ v0.11.0 release checkpoint

Status: RC EVIDENCE COMPLETE AT `8008bf6`, 2026-08-25 — awaiting the
operator's release-script runs (which push members and mint the tag). This document is the
release record per the v0.10.5 convention; the tag line below is
filled when the operator publishes.

## The release candidate

| Repo | Commit | State |
| --- | --- | --- |
| gwz-core | `8008bf6` | pushed to main; version `0.11.0`; taut-proto pin 0.9.1 |
| gwz-cli | `6b7e75a` | main pushed; release branch + tag pushed |
| gwz-py | `5f6689a` | main pushed (incl. the completed four-site 0.9.1 bump); release branch + tag pushed |
| taut | `5cd26a1` | snapped at the taut v0.9.1 release tag (operator-directed; was `f008419`, an ancestor — the old pin was behind the release) |

**TAGGED 2026-08-25, via the three release scripts run by the lane
on the operator's "go ahead and run the release scripts":**
gwz-core `v0.11.0` → `8008bf67b7` (main+tag atomic); gwz-cli
`v0.11.0` → `dec5e3bd47` (release branch reconciled: version +
path→git-tag re-point, pushed atomic); gwz-py `v0.11.0` →
`f53a7c64c6` (release reconciled; Cargo.lock verified pinning
gwz-core via git+tag=v0.11.0#8008bf6). Two failures en route, both
cured and recorded below. REMAINING FOR THE OPERATOR: publish the
GitHub release from tag v0.11.0 with `GwzReleaseNotes-v0.11.0.md`
as the body (the verify workflow was lane-dispatched at the tag,
run `32946137285`, result recorded at completion).

The train (all exact-ref pushed, each behind its filed review):
`1a31851` A1 activation + `8e40fa8` corrective → `a6ef094` R1 →
`bed072a` R2 → `07e1ac1` R3 → `a6ce8a8` precision micro-train → `8008bf6` taut-proto
0.9.1 bump (operator-directed, tested-first: wire byte-identical;
currency harness 29/29 under 0.9.1).
gwz-cli: `cf0d16d` docs truth fix → `6b7e75a` R4 recovery guide.
Reviews: `GwzM5-8A1ReleaseR{1,2}-ReviewState.md`,
`GwzM5-8A1ReleaseR3-Review.md`, `GwzM5-8A1ReleaseR4-Review.md` — all
GO (R3/R4 after their condition rounds, applied and re-executed).

## Gate ledger at the RC

| Gate | Status | Evidence |
| --- | --- | --- |
| G1 foreign-filter refusal (A′) | **DISCHARGED** | landed `9939b02`, first filed review GO; condition tests + doctrine sentinel at `a6ef094`; forbidden ordering cleared |
| G2 CRLF birth pins (B) | **DISCHARGED incl. G2-c1** | landed `9939b02`; gap package `bed072a`; RC evidence below |
| G3 retained-reader truth | **DISCHARGED** | the v0.11.0 generation + migration clause + the register hole closed (`07e1ac1`); tuples 24; L2-04 86 |
| G4 release-lane gates | **DISCHARGED** | batteries Linux-leg + CRLF count-pin Windows-leg wired topology-audited (`07e1ac1`); both un-wireable checkers recorded with the R2-F cure |
| G5 native evidence at RC | **DISCHARGED** | the four RC-head runs below |
| G6 release-truth docs/notes | **DISCHARGED** | docs gate `ok (12 sources, 155 assertions)` with the recovery guide pinned (`a6ce8a8`); notes final |

**Plan correction (G5, recorded here):** the plan's G5 row named "the
T-5 candidate-pair regeneration discharged on its §11.2 carrier" —
over-eager. v0.11.0 regenerates no retained-reader evidence, so T-5
remains OPEN on its R2-F native-evidence carrier exactly as the
settled-tuple row states. Not a gate for this release.

## RC evidence (2026-08-25, at `a6ce8a8`, verbatim)

All four RC-head runs GREEN: **Platform matrix** `32802563914`,
**Windows matrix** `32802562170`, **Checked-artifact boundary**
`32802495545` (with the wired batteries), **Retained merge readers**
`32802495544` — re-established at the taut-bump head `8008bf6`:
Windows `32846951548`, Platform `32846954270`, boundary
`32846765849`, retained-readers `32846765913` — ALL GREEN at
`8008bf6` (retained-readers on rerun: its windows harness leg first
failed `test_timeout_kills_descendant_process`, a process-tree kill
race in a file the bump's five-file diff provably does not touch;
the rerun of the identical tree passed in 606s — environment flake,
recorded, hardening chip filed). The a6ce8a8 evidence stands for
the unchanged tree content; the re-run covers the workflow-pin
change. Push CI green at every train commit
(`a6ef094`/`bed072a`/`07e1ac1` boundary+readers all success).

**G2-c1, discharged with both required tails** (windows run
`32802562170`, `crlf-sentinel` job, under the hostile un-pinned
host):

```
Pinned lane:      test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1545 filtered out; finished in 0.27s
Un-pinned:        test git::tests::g01::crlf_sentinel_unpinned_worktree_materializes_blob_exact - should panic ... ok
                  test result: ok. 1 passed; 0 failed; ...
```

**First-dispatch confirmations, both held:** the Windows count pin at
exactly `6 passed` (the 5-unconditional + 1-`cfg(windows)` derivation
confirmed on the runner), and the stash proof's first Windows
execution —
`git::tests::g11::stash_round_trip_on_a_born_repo_is_a_filter_no_op
... ok` in the full Windows suite — which CLOSES the C-iii evidence
caveat ("proven non-vacuously off-Windows"): the born-repo stash
closure is now proven ON Windows as well. The `#[should_panic]`
sentinel also rode the full Windows suite green, as designed.

## What v0.11.0 carries, named

Shipped: the v1 writer + `--no-ff` (A1); envelope-dispatched v0/v1
reads; the seven-shape resume migration; the A′ pre-mutation
foreign-filter refusal; the CRLF birth-time pins; the merge recovery
guide (`docs/MergeRecovery.md`); the release-lane batteries + CRLF
lane; the retained-reader generation set.

Named residuals riding, each with owner: `ACTIVE_WRITER_FLOOR = V0`
(M5c); the A′ target-side-coverage residual (register entry +
doctrine sentinel; R6 renormalize); adopted-worktree CRLF
availability (renormalize); attribute-driven smudge (permanent
fail-closed doctrine); C-2/18-UNBOUND/archive-equivalence (R2-E
archive/GC sub-package); T-5 pair (R2-F carrier); L2-05 + m4-map CI
wiring (R2-F multi-repo checkout); the eligible-row upgrade-failure
fallback test ([P3-R2-2], R2-E).

## Resume order after the tag

R2-E (the 67 re-reserved keys + the binding-obligation ledger) —
M5c — R2-F (native evidence, T-5, the multi-repo CI checkout) — the
escape implementation packages (second lane, still blocked on
operator handoff) — R6 renormalize (shares A′'s predicate; owns two
residuals above). Program state authority remains
`CurrentProgramCheckpoint.md`.

## Release-execution incidents (2026-08-25, both cured)

1. **ENOSPC at the core script's full-suite gate** — the disk hit
   2.3Gi free (the session's ~20GB of accumulated per-agent cargo
   caches); two fault-matrix tests failed with os error 28 after
   1587 passed. Cured by deleting the rebuildable caches and landed
   worktrees (47Gi free); the idempotent re-run went green
   end-to-end and minted the tag.
2. **The truncated-inventory pin miss** — the py script failed at
   run_tests.py: `maturin develop` installs the project's RUNTIME
   dependency `taut-proto==0.8.1` from pyproject.toml, silently
   uninstalling the release venv's 0.9.1 and tripping the version
   guard. Root cause of the miss: the original pin inventory was a
   `head`-truncated grep presented as complete — pyproject's two
   entries and two gwz-py workflow installs were the cut-off rows.
   Cured at gwz-py `5f6689a` (all four sites; untruncated residual
   sweep clean); re-run green. LESSON MINTED: an inventory produced
   through a head/limit pipe is not an inventory — pin sweeps run
   untruncated and end with a residual grep proving zero matches.

## Release verify at the tag — RED on a first-execution class (2026-08-26, run `32946137285`)

**Windows leg GREEN** (incl. the CRLF count-pin step's first
release-context execution). **ubuntu-22.04 leg FAILED at Test**: the
`checked_artifact::admission` class uniformly panics with
`Unsupported { capability: DurableObjectIdentity, detail:
"Inappropriate ioctl for device (os error 25)" }` — the Linux
durable-identity probe (`identity.rs:267`, `name_to_handle_at`
persistent file handles) refused by the filesystem backing the test
temp root (fixtures root at `std::env::temp_dir()`,
admission/tests.rs:43). FIRST-EXECUTION CLASS: the platform matrix's
Linux leg is ubuntu-24.04-ARM only and the boundary workflow (24.04
x86) runs checker+harness, not the full suite — this verify is the
FIRST x86-Linux full-suite execution the admission machinery has
ever had; v0.10.5's verify predates the machinery entirely. The
failure is the capability refusing FAIL-CLOSED, by design; the open
sub-question is whether any production v0.11.0 path (the --no-ff v1
store) reaches the durable-identity probe on Linux, which decides
whether this is a user-facing environment sensitivity to document
or a test-environment-only fact. THE GITHUB RELEASE PUBLICATION IS
HELD until the operator rules on the gate (align the verify's Linux
leg to ubuntu-24.04 / investigate production reach first / other).

## PUBLISHED — 2026-08-26

The three GitHub releases were created together on the existing tags
(the operator waived the redundant CI re-verify: the release script's
own gate stack had run the full suite at the tag commit before
minting): gwz-core `v0.11.0` (full notes body, internal preamble
stripped), gwz-cli `v0.11.0` (seed body; the dist workflow replaces
it with the generated installer/binary tables), gwz-py `v0.11.0`
(pointer body: pins core v0.11.0, verifies the cli v0.11.0 contract,
taut-proto 0.9.1). Event machinery confirmed firing: cli dist build +
Documentation, py Publish (wheels), core's event verify (from the
tag's workflow copy — its ubuntu-22.04 leg re-failing is EXPECTED and
pre-recorded; the aligned dispatch verify `32954473899` continues as
additional evidence). Artifact pipelines under watch.

**Artifact pipelines complete (2026-08-26):** cli dist GREEN — 16
assets attached to the gwz-cli release (platform binaries,
installers, checksums, attestations); py Publish GREEN (wheels).
The aligned dispatch verify (`32954473899`, ubuntu-24.04 leg) was
still in flight at this record; its result appends to the program
checkpoint on completion. v0.11.0 IS FULLY SHIPPED.

**Aligned-verify remediation round (2026-08-26):** verify-2
(`32954473899`, ubuntu-24.04) CONFIRMED the environment ruling — the
admission class green on 24.04, Windows green — and then caught a
SECOND truth: the R4b-G fault battery's count pins were
host-divergent AND stale. checked_artifact:: counts 410 on Linux vs
the Darwin-measured 400 (ten `cfg(target_os = "linux")` tests), and
the 926 remainder pin was stale on BOTH hosts — R1/R2 landed
remainder-partition tests without moving the battery marker (a
ritual-5 miss by those trains: the driver's markers are count
companions too; the landing gates ran partitions directly, so
nothing local caught it). Cure at gwz-core `1bcb925`: per-OS
expected counts for exactly the two divergent partitions, every
value executed (darwin 400/932 re-run and confirmed exactly; linux
410 measured / 933 derived-from-the-run, FIRST-DISPATCH-EXPECTED),
unmeasured hosts failing loudly. Verify-3 dispatched at the tag:
`32960235595`, under watch.

**THE VERIFY CHAIN CLOSES GREEN (2026-08-26):** verify-4
(`32965339595`) SUCCESS on both legs under the tag-frozen-driver
guard (gwz-core `94da3e5`): the verify checks out the TAG's tree, so
a tag's battery driver and count pins are frozen with the release —
main-side fixes can never retroactively cure an existing tag's
verify. The guard detects a driver predating per-OS pins (no
`_fault_count`) and withholds ONLY the fault battery, loudly, with
the reason (the same convention as the pre-driver presence guard);
everything else ran and counted. The v0.11.0 verify of record:
Windows leg green ×4 consecutive runs (incl. the CRLF count-pin and
sentinel steps); ubuntu-24.04 Test step green (full suite 1600/0/1);
batteries green (byte-equivalence:2, unknown-field, privacy; fault
withheld per the frozen-driver rule, its partitions covered by the
Test step). Tags from `1bcb925` forward run the full selection with
per-OS executed pins. **THE v0.11.0 RELEASE ARC IS CLOSED.**
