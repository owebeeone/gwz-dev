# ARM/Linux preservation-handoff class — diagnosis

Package scope: the 47-row failure mass remaining on the
`ubuntu-24.04-arm` platform-matrix leg after EBADF extinction —
29 × `git::tests::g15::root_preservation`, 17 ×
`workspace_ops::merge::v1_lifecycle`, 1 ×
`checked_artifact::tests::exact_source` — recorded at run 32556291055
(gwz-core `0b059e8`, 1374/47/1) and re-confirmed by this lane's A/B
dispatch at run 32559626688 (`6c7c8f3`, 1404/47/1). All line numbers
verified against `d2cf755` (identical to `0b059e8` in every module
this diagnosis touches; `6c7c8f3` adds only a Windows-only commit).

## Verdicts

**(a) 29 × g15::root_preservation + (downstream) 17 × v1_lifecycle:**
a single Linux-runner-only class, **not ARM-specific, not
git-version-specific, and not reproducible off the GitHub runners** —
including in a byte-parity arm64 Linux rig built for this diagnosis.
The mechanism was narrowed to the preservation-root-only observation
gates operating on CLI-git-built fixture state, but the divergent
field could not be observed without instrumentation on a real runner.
No fix is applied for (a); a one-test probe branch is prescribed
below. Production-side repair is NOT indicated by any evidence
gathered here; every gate implementation was proven correct against
byte-identical inputs on Linux.

**(b) 1 × exact_source: FIXTURE-side. Fixed in this package — loudly:
the fixture, not production, was wrong.** The production refusal it
tests held on every failing CI run; only the fixture's inode-inequality
precondition is false on inode-recycling filesystems. Reproduced
locally at will; fix verified on macOS and on the reproducing Linux
rig.

## Evidence base (runs and logs)

| Run | Tree | Leg | git | Result | Note |
|---|---|---|---|---|---|
| 31820283530 (release verify) | `4bcf13c` 2026-08-14 | ubuntu-22.04 x64 | 2.54.0 | GREEN | last green Linux; **g15 suite does not exist in this tree** |
| 31901705323 (release verify) | `f36d20d` 2026-08-15 | ubuntu-22.04 x64 | 2.54.0 | red | **the same 29 g15 rows with the same prepare message** (log-verified), inside the larger EBADF-era mass; exact_source PASSED here |
| 32555574473 (platform) | `ae4e143` | ubuntu-24.04-arm | 2.55.0 | red | 29+17 plus 3 reverse-preservation EBADF rows (see fixture-selectivity below) |
| 32556291055 (platform) | `0b059e8` | ubuntu-24.04-arm | 2.55.0 | 1374/47/1 | EBADF extinct; the 47 under diagnosis |
| 32559626688 (platform, dispatched by this lane) | `6c7c8f3` | ubuntu-24.04-arm | 2.55.0 | 1404/47/1 | A/B against the local parity rig on the same tree; macos-14 leg (git 2.54.0) green |

Key derived facts:

- **The g15 suite has never passed on any Linux.** It landed in the
  `4bcf13c..f36d20d` window (Windows-campaign train `f2fceaf`/`731fcb2`);
  the last green Linux run predates it. "First-Linux-execution class"
  (checkpoint L1-16) is confirmed and sharpened: x64/22.04/git-2.54 and
  arm64/24.04/git-2.55 fail the identical 29 rows with the identical
  message. ARM is merely the only Linux leg currently dispatched.
- All 29 die at `src/git/tests/g15/root_preservation/support.rs:424`
  (the shared fixture `prepare()`), with
  `PreservationEvidenceMismatch: "root preservation preparation
  requires the exact durable handoff"`. The five g15 tests that pass on
  CI are exactly the rejection tests (they do not need a successful
  prepare). The one non-prepare row
  (`mutation::reset_partials_reject_without_changing_refs_index_or_worktree`,
  mutation.rs:408) dies at its first checked step observation with
  "ambiguous physical evidence" — the same gate family reporting
  through `observe_managed`'s Ambiguous arm.

## Mechanism analysis for (a)

`prepare_root_preservation_stash`
(`src/git/gitbackend/preservation_root.rs:11-33`) fails with this
message iff all inner calls return `Ok` and at least one of four gates
is false: `exact_head`, `repository_state == Clean`,
`full_form_matches(handoff_form)` (marker-worktree, lock-worktree,
raw-index, marker-parent), `files::observe_boundary`. Every inner
*error* path (index parse errors, unsupported extensions, identity
`UnsupportedOperation`, io errors) carries a different message and is
therefore excluded by the panic payloads.

Cross-suite elimination on the same failing runs:

- `head`/`read_ref`/`repository_state` are exercised green by the
  g01–g14 backend suites (hundreds of ARM-green rows) — libgit2 against
  CLI-built repos works on the runner.
- The workspace-policy checked observe substrate
  (`checked_artifact::observation/identity`) is exercised green by the
  271-row `checked_artifact::` suites on the same leg.
- The remaining candidates are the preservation-root-only observers:
  the raw index chain (`index_format::parse` + `index::observe`), the
  marker-parent dirent chain (`parent::observe`), the boundary observe
  (the sole consumer of the `git_directory` policy), and `exact_head`'s
  specific composition.

**Fixture-selectivity, proven on-runner:** at `ae4e143` (pre-twin-fix)
three `v1_lifecycle::reverse::preservation` rows died with EBADF *at
the marker-parent durability sync* — i.e. those fixtures' step
observations returned `Before`, which requires the same gate family
(`repository_state`, `exact_head`, `observe_boundary`,
`guard_matches`, parent state machine) to evaluate TRUE on the very
runner where every g15 fixture's gates evaluate false. The
discriminating difference: the v1 fixtures build repository state via
libgit2 (`create_repo`/`commit_file`), the g15 fixture via the system
`git` CLI (`init`/`add`/`commit`). The runner-only condition is
therefore selective on CLI-git-produced repository state.

**Byte-parity rig (all green — this is what makes (a) runner-only):**
an arm64 Linux reproduction rig was built from the `d2cf755` archive
(ubuntu:24.04 container on a native arm64 Linux VM; loop-mounted ext4
TMPDIR; rustc 1.95.0; `--locked`):

| Rig configuration | g15 result |
|---|---|
| ext4, stock git 2.43.0, root | 34/34 green |
| ext4, PPA git 2.55.0 (runner's build), root, system LFS config + safe.directory | 34/34 green |
| ext4, PPA git 2.55.0, uid-1001 user (runner parity) | 34/34 green |
| ext4, full `--lib` suite at 4 threads (concurrency parity) | g15 rows all green |
| ext4, git 2.55.0 — `v1_lifecycle` suite | 246/246 green |
| overlayfs (negative control) | 29 failures but with `UnsupportedOperation (os error 95)` — a different message; rules overlayfs out as the CI filesystem |

Deeper parity checks, all equal between macOS and the Linux rig:

- The fixture's `.git/index` is byte-identical (244 bytes, version 2,
  single TREE extension, mode 100644, same object ids, same flags)
  under git 2.43, 2.52 (macOS host), and 2.55 — a byte-faithful
  re-implementation of `index_format::parse` + `observe_fact` passes on
  both OSes, and a verbatim compile of the real
  `observation.rs`/`identity.rs`/`parent.rs` code (cap-std 4.0.2,
  rustix 1.1.4 pins) passes every gate on Linux/ext4 with healthy
  durable identity (`name_to_handle_at` handles + `f_fsid`).
- Runner git config seeding was replicated exactly (git-core PPA
  build; `/etc/gitconfig` `safe.directory` per the runner image's
  `install-git.sh`; `git lfs install --system`; checkout's global
  safe.directory). No other git config or `GIT_*`/CI env influence
  exists (`src/` contains a single `env::var` read, unrelated).

**Residual (what could not be proven without a runner):** with code,
inputs, git binary, distro userspace, filesystem type, user, and
concurrency all matched and green, the divergent field must involve a
property unique to the GitHub runner environment (Azure kernel builds
6.5/6.8, the aged root-ext4 superblock and its mount context — the
`exact_source` inode numbers 6029447/6029448 prove CI fixtures live on
the large root ext4 — or the runner service's process context)
interacting with CLI-git-produced state. Which of the four gates is
false, and on which field, is exactly one instrumented run away.

### Prescribed probe (one dispatch, no production change)

Precedent: the `probe/finalization-diagnosis` single-test pattern.
On a probe branch:

1. Add a temporary Linux diagnostic test beside
   `src/git/tests/g15/root_preservation/support.rs` that builds the
   standard fixture and prints, without asserting: each prepare gate's
   boolean (`exact_head`, `repository_state`, each of the four
   `object_matches` legs, `observe_boundary`), a hexdump of
   `.git/index`, dirent listings of `gwz.conf` and `gwz.conf/markers`,
   `.git/HEAD` and `refs/heads/main` bytes, and
   `statfs`(f_type/f_fsid/flags) + `uname` for the fixture root.
2. Trim platform-matrix to
   `cargo test --lib g15::root_preservation -- --nocapture` on
   `ubuntu-24.04-arm` only.
3. Exact reproducing test to keep in the filter:
   `git::tests::g15::root_preservation::observation::real_sha256_repository_prepares_exact_handoff`.

The gate booleans identify the false gate; the accompanying dumps
identify the divergent field in the same run. Only then does a fix
(production or fixture) have proof to stand on.

## Cluster disposition: the 17 v1_lifecycle rows

**Same class, downstream — not independent.** Production wiring: the
lifecycle's preservation phase calls
`prepare_root_preservation_stash`
(`src/workspace_ops/merge/preserve/artifacts.rs:692`) and executes
root steps through `execute_root_preservation_step_checked`
(`v1_lifecycle/reverse/execute/preservation.rs:87,125`). The traced
shapes are recovery-entry, not assertion drift:
`concrete_finalizer_freezes_acceptance_and_publishes_exact_candidate`
(tests/finalization.rs:40) gets `RecoveryRequired` where `Completed`
is expected; the `crashed.is_err()` rows and "EvidenceCommit fault was
not reached" show injected crash faults never being crossed because
the run diverts into recovery before the fault site. All 246
`v1_lifecycle` rows are green in the parity rig; three members of this
family reached the marker-parent sync on the runner at `ae4e143`
(the EBADF rows), placing them on the same substrate with
fixture-dependent gate outcomes. They are expected to clear with (a)
and need no separate diagnosis unless the probe says otherwise.

## (b) exact_source — mechanism, fix, verification

`same_byte_new_inode_after_final_proof_is_not_accepted_as_the_source`
(`src/checked_artifact/tests/exact_source.rs`) tampered at the
`AfterFinalProof` fault by `remove_file` + `write` of the same bytes.
ext4-class allocators recycle the freed inode number: on the runners
the surviving inode equalled the original (6029447 at `0b059e8`,
6029448 at `6c7c8f3`), and the same failure reproduces at will on the
local loop-ext4 rig. Two load-bearing observations:

- **The production guarantee held.** The refusal assertion
  (`MergeRecoveryRequired`, line 45) passed on every failing run —
  Linux durable identity (`name_to_handle_at` handles carry the inode
  generation) correctly distinguishes a recycled-inode object even
  when `(dev, ino)` and the bytes all match. The failure was only the
  fixture's `assert_ne!(surviving_inode, original_inode)` precondition
  — a proxy for "a new object exists" that is false under recycling.
- **Fix (applied):** stage the same-byte replacement under a sibling
  name while the original still exists — two live files cannot share
  an inode — then `rename` over the source. The new-inode precondition
  becomes a guaranteed invariant on every POSIX filesystem, the
  recorded foreign inode survives the rename unchanged (line 48 intact),
  and the adversary is strengthened to the realistic rename-replacement
  form. No production code, no other tests, and no observer semantics
  are touched.

Verification: macOS `exact_source` 3/3; Linux/ext4 rig 3/3 post-fix
where the pre-fix test failed on the same rig; scoped rustfmt clean;
`check_checked_artifact_boundaries.py` ok (15 visible entries, 5
classified modules); `cargo clippy --all-targets --all-features -- -D
warnings` clean on the fixed tree; g15 unaffected (33/33 on the macOS
archive tree).

## Masking-safety

- The (b) fix cannot mask (a): different suite, different fixture,
  and (a)'s rows are prepare-time deaths unrelated to inode identity.
- The (b) fix cannot mask a production regression: it *tightens* the
  fixture (the distinct-object precondition now always holds, so the
  refusal path is exercised on every platform every run, instead of
  vacuously "passing" only where allocators do not recycle).
- No change was made inside `src/git/gitbackend/preservation_root/**`,
  `src/git/tests/g15/**`, or any workspace_ops/checked_artifact
  production module; (a) remains fully observable for the probe.

## Expected next platform-matrix outcome

With only this package landed, on the `6c7c8f3`-equivalent tree:
`ubuntu-24.04-arm` = **1405 passed / 46 failed / 1 ignored** — the
exact_source row turns green; the 29 g15 + 17 v1 rows persist by
design until the probe run identifies the divergent gate field.
macOS unchanged (green).

## Reproduction tooling (session scratchpad, disposable)

- Gate-replica probe (Python, byte-faithful `index_format::parse`):
  `scratchpad/linux-repro/probe.py`.
- Real-substrate probe crate (verbatim `observation.rs`/`identity.rs`/
  `parent.rs`, pinned deps): `scratchpad/linux-repro/g15probe/`.
- Parity rig: ubuntu:24.04 arm64 container + loop ext4 + PPA git +
  rustup 1.95.0 over the `d2cf755` archive; commands recorded in the
  session transcript.

Diagnosis lane, 2026-08-22. Non-gating (thin A1). This memo dispatched
one platform-matrix run (32559626688) for the A/B; no branches, tags,
commits, or pushes were created.
