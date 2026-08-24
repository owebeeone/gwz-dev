# Windows matrix classification — GREEN (runs 11, 13, 18–22); macOS GREEN at run 13; ARM GREEN at the run-18 sibling

*(Run numbers in this ledger are the ledger's own sequence; GitHub
`run_number` diverges — e.g. ledger run 22 = GH `run_number` 32 for the
Windows matrix, and 13 for its Platform sibling. The workflow **run ids**
quoted per entry, e.g. `32672188425`, are the unambiguous handles. Note added
2026-08-24 per the R4b-G Evidence axis [P3-6](v), so a future auditor does not
conclude the id is wrong.)*

**Run 22 — 32672188425 on `b91bdeb` (THE R2-D SETTLED OBJECT):
1530 passed / 0 failed / 1 ignored.** The settled object's own full
matrix record, closing the R4b-G evidence inventory's F-6 and
discharging the settled-dual Code axis's [P3-4] ledger duty: the ten
multi-component writer rows and the 5.1/round-2 deltas executed
natively. Platform sibling (32672189557): ubuntu-24.04-arm 1569/0/1,
macos-14 1580/0/1. Five consecutive fully-green dispatched trees
(`c2d2f15`, `9c454ce`, `51a9cba`, `514f8e6`, `b91bdeb`).

**Run 21 — 32616954369 on `514f8e6` (Step 4.3, Phase 4 settled):
1528 passed / 0 failed / 1 ignored.** The racing-contention row's
retirement branch executed natively (discharging the closure doc's
run-21 expectation), alongside the seam-level refusal row and the
narrowed-allow tree. Platform sibling (32616955672):
ubuntu-24.04-arm 1567/0/1, macos-14 1578/0/1. **Four consecutive
fully-green trees** (`c2d2f15`, `9c454ce`, `51a9cba`, `514f8e6`) —
the settled tuple's platform record.

**Run 20 — 32613728318 on `51a9cba` (Step 4.2, the anchor
retirement): 1526 passed / 0 failed / 1 ignored.** The full native
validation of the retirement beyond the probe slice: the
ordinal-indexed durable retirement, the deterministic staging names
(anchor and authority family), all ten protocol boundaries including
both retirement boundaries, and the seven legacy AnchoredPrivateArea
suites — green first-execution at full population. Platform sibling
(32613729322): ubuntu-24.04-arm 1565/0/1, macos-14 1576/0/1. Three
consecutive fully-green trees (`c2d2f15`, `9c454ce`, `51a9cba`)
spanning two production write-path rewrites and the anchor
retirement.

**Run 19 — 32607411300 on `9c454ce` (Phase 3 complete + Step 4.1):
1506 passed / 0 failed / 1 ignored.** The native Windows debut of the
Step 3.1b intent matrix, the 3.2 writer/purpose matrices, the 3.3
coordinator suite, and 4.1's leaf_publication substitution rows — all
green first-execution, discharging run 17's pre-attribution. The
platform sibling (32607412448) is green both legs (ubuntu-24.04-arm
1544/0/1, macos-14 1555/0/1), and `retained-readers` ran green at the
same sha INCLUDING its network matrix job (triggered by the push's
scripts/checks/** change), completing L2-04's CI verification of the
4.1 leaf-publication conversion. Two consecutive trees (`c2d2f15`,
`9c454ce`) now fully green across a production write-path change.

**Run 18 — 32573548540 on `c2d2f15`: 1472 passed / 0 failed / 1
ignored.** The first fully green Windows run since the R2-D fault
families began landing. Discharges the anchor package's TRACKED
ACCEPTANCE ITEM (review [P2-3]): the seven legacy
`AnchoredPrivateArea` call-site suites (cleanup/residue/transition
paths) ran green under the classed barrier, and the run-16/17
namespace fault-matrix four are extinct at `6b8b76e` exactly as the
trimmed probe (32569434565) predicted. Tree note: `c2d2f15` includes
Step 3.1 but neither 3.1b nor 3.2 — the intent and writer matrices
make their Windows debut in the next full run, pre-attributed here as
expected-green (they drive the same classed barrier and the managed
matrix has been green natively since run 17).

**Platform run 32573547362 on `c2d2f15` (sibling of run 18):
ubuntu-24.04-arm 1510/0/1 — THE FIRST GREEN ARM RUN IN THE PROGRAM —
and macos-14 1521/0/1.** The 46-row template-mode class is extinct on
its first post-fix execution, exactly as the fixture package's
red-green under the simulated runner HOME predicted (29 g15 + 17 v1,
name for name). With run 18 green the same hour, the ENTIRE platform
matrix is green simultaneously at one tree for the first time since
the fault families began landing.

**Class-membership note (M5b-IMPL ledger obligation, design §9 Q7,
recorded 2026-08-22):** a v1 record row carrying `mode: no_ff` makes
the commit-arm materialization reachable for otherwise-fast-forwardable
participants; both the ff path and the commit path were already members
of the CRLF/materialization class covered by the un-pinned CRLF matrix
sentinel (D1 package), so this is bookkeeping, not new exposure.

**[Dated update, 2026-08-25 — A1 landing (activation record L11).]**
The note above was written while the v1 writer was `cfg(test)`-only;
at the A1 landing commit that clause changes truth-value: the v1
writer is production and `--no-ff` creates v1 records with
`mode: no_ff` in ordinary operation (ordinary/custom starts remain v0
under the `ACTIVE_WRITER_FLOOR = V0` named residual, owner M5c). The
class membership recorded above is now a production fact, still
inside the same CRLF/materialization class — bookkeeping then,
production bookkeeping now. Pre-attributed EXPECTED-GREEN at the
landing-commit dispatch: the activation package's +11 tests (census
1573 → 1584, incl. the L5 abandonment witness) and the C-1 closure's
3 `compatibility_residue_v0` tests (in-base since `26f48f5`, first
native execution at this dispatch).

**Run 17 — 32568481700 on `8b83a2c` (Steps 2.3+2.4 and D3 landed):
1457/4/1.** The four failures are byte-for-byte run 16's namespace
fault-matrix class — same tests, same baseline refusal — now
root-caused and FIXED at `6b8b76e` (P5 gets its writer class;
E10/E14's `ExactInterior` arm is a documented no-op; freeze §4.3
E10/E14 activation annotation; probe run `32569434565` on
`probe/namespace-anchor-win` was GREEN 22/0 before landing).
**Correction to run 16's pre-attribution:** the managed matrix did
NOT join the class — `tests_managed_matrix`, `tests_managed`, and
both 2.4 suites all ran natively green here at the un-fixed base, so
the class was always scoped to the namespace matrix's baseline E14
barrier call; the probe's managed-matrix rows confirm the fix keeps
them green rather than rescuing them. Population 1404 → 1462
(+58: the 2.3/2.4/D3 test mass). **TRACKED ACCEPTANCE ITEM (review
[P2-3]):** the trimmed probe exercised none of the seven legacy
`AnchoredPrivateArea` call sites, so the anchor repair's acceptance
evidence is run 18 — the next full Windows matrix run must be green
across the legacy private-area suites (cleanup/residue/transition
paths) as well as the four namespace-matrix tests. Expected run 18:
1462/0/1, the first fully green Windows run since the R2-D fault
families began landing.

**Platform run 32568483432 on `8b83a2c` (sibling of run 17):
ubuntu-24.04-arm 1463/46/1.** `exact_source` CLEARED on its first
post-fix execution (47 → 46, exactly the diagnosis's prediction);
the surviving 46 are the one template-mode class
(29 g15 + 17 v1_lifecycle; probe `probe/g15-gate-dump` proved
`.git/info/exclude` mode 0755 from the runner image's git template
tree; fixture-side mode pinning in flight). Expected next platform
run after that package lands: ARM green for the first time in the
program.

**Run 16 — 32559979514 on `6c7c8f3` (the share-delete fix):
1399/4/1.** The run-15 class is EXTINCT: all 14 admission tests green
on their first post-fix execution (the behavioral detector fired
clean), zero os-32 anywhere. The 4 failures are the namespace fault
matrix (`checked_artifact::namespace::tests_fault_matrix`, both
matrices × both target variants) on ITS first native execution —
every row dies at BASELINE, before any fault injection, with one
typed message: `Ambiguous { fact: "action namespace barrier",
detail: "checked action namespace barrier: private durability anchor
is not ready" }` (`tests_fault_matrix.rs:376`). One cause, four
symptoms: a deterministic Windows arm gap in Step 2.2's barrier
anchor-readiness (E9-family: the readiness evidence the barrier
demands is presumably unobtainable through the Windows no-op flush
arm). Production fails typed and closed — correct posture, wrong
availability. Diagnosis dispatched on the clean post-landing tree;
the managed matrix landed with Step 2.3 (`c2564ba`) drives the same
barrier and is EXPECTED to join this class in run 17 — same cause,
pre-attributed here, do not count it as a new class.

**Run 15 — 32555573059 on `ae4e143` (Phase 1 settled + ARM64 train):
1359/14/1.** One cause, fourteen symptoms, all in the new admission
kernel on its first native execution: the sealed publication's
retained rename-source handle on the admission STAGING DIRECTORY
(a child of the catalog root) carries DELETE access, and the same
edge's `AdmissionCatalogInterior` destination recheck re-enumerates
that root through `interior.rs`'s plain `open_dir_nofollow`, which
omits FILE_SHARE_DELETE → `ERROR_SHARING_VIOLATION` (os 32) at
"open catalog interior directory". Novel composition: Phase 1's
staging publish is the first in the tree whose rename-source is a
directory child of the very root its destination recheck walks
(PreRetirementFinal publishes a regular file from a different
parent, which is why Windows stayed green until now). Ordering was
barred in principle — the retained handle IS the seam's identity
proof — so the fix is the pre-written house recipe
`platform::open_dir_share_delete` (one hunk, +24/−2, non-Windows
arm byte-identical), landed at `6c7c8f3` with the pre_catalog pin;
the 14 admission tests are the behavioral detector on the next run.

**Platform run 32555574473 on `ae4e143` (Phase 1 settled + the ARM64
EBADF package): the repair verified — ubuntu-24.04-arm went
1094/266 → 1372/49**, clearing the 143 direct EBADF panics and their
cascades at all three repaired sites, and **macos-14 is 1410/0/1**
(matching the Phase-1 reviewer's pristine full-lib count). The 49
residue is ONE cause, named in advance by `GwzArm64EbadfDiagnosis.md`:
the untouched preservation-root twin (`preservation_root/parent.rs`
`sync_dir` dup+fsync) — 29 direct in `g15::root_preservation`, 20 as
`v1_lifecycle` finalization/reverse cascades (raw "Bad file descriptor
(os error 9)" in the reverse rows; RecoveryRequired downstream). Fixed
the same hour by applying the identical review-verified reopen recipe
to `sync_dir` (Linux arm; other-unix dup+fsync retained); g15 45/0
locally, clippy/fmt clean. Re-dispatched for verification.

**Run 14 — 32545131198 on `c40e712` (the R2-D Phase 0 freeze train):
1333/1/1.** The single failure is the new seam pin
`r2d_seam_freeze::the_track_p_publication_spike_is_test_only` on its
first native execution (the predicted first-dispatch class):
`include_str!` receives working-tree bytes, so on a CRLF checkout the
pin's hard-coded `\n` needle misses — fixed the same hour by
normalizing `\r\n` before matching (test-only file, no pin owed).
Both Track-P spike cases ran GREEN natively inside this run,
discharging the freeze acceptance's tracked item ("both spike cases
green on the next full Windows matrix run"). The trimmed probe run
32542469665 on `probe/track-p-spike` (2/2 in 0.04s) remains the
recorded Windows arm of the Step C.1 spike.

**Run 13 — 31921194567 on `90d3f8a` (run-12 residue fixes):
1322/0/1 — GREEN.** Both run-12 first-dispatch failures cleared on
native re-execution: the destination-window ancestor test now asserts
the OS-level denial (raw os error 5/32, source untouched) instead of
unwrapping the rename, and the LFS fixture's repo-local hermeticity
pins survive the runners' system git-lfs `filter.lfs.required=true`.
Same test universe as run 12 (1322 executed), both conversions to
passes, no count drift.

**Platform run 31921196151 on `90d3f8a` (sibling of run 13):**
the **macos-14 leg is 1359/0/1 — the first macOS green on record**
(run 12's single macOS failure was the same LFS test, cleared by the
same hermeticity fix; 752.70s). The ubuntu-24.04-arm leg is 1094/266/1
in 10.30s — failure count byte-identical to run 12, as predicted: this
train did not touch the EBADF substrate class, which remains the
entire ARM64 delta (tracked for its own package; cheapest lead is the
`query_error` errno allowlist at linux.rs:173-181). Three of four
platform legs (Windows, ubuntu ext4 probe, macOS) are now green.

**Run 12 — 31911445128 on `d70bd57` (the day's full landing set):
1320/2/1.** Both failures were brand-new tests on first native
execution (the predicted first-dispatch class): the destination-window
ancestor test unwrapped an ancestor rename Windows denies at OS level
(error 5 — the denial IS the property; fixed to assert it, mirroring
the retained-directory guarantee test), and the LFS-allowlist positive
failed because CI runners ship system git-lfs with
`filter.lfs.required=true`, hard-failing the fixture's porcelain
checkout on fake pointer content (fixed with repo-local hermeticity
pins; the gate's allowlist keys on the attribute NAME and
short-circuits before any config probe, so the pins cannot change the
gate's decision — verified at recovery_support.rs:75-77, and the fix
was proven under a reproduced hostile system-lfs config locally).

**Platform-leg attribution CORRECTION (per L1-16 — the earlier record
inverted the legs):** in runs 31901724907 and 31911446334, the
near-green leg is **macos-14** (1358/1/1 in run 12's sibling — its
single failure was the same LFS test, now fixed) and the mass-fail leg
is **ubuntu-24.04-arm** (1094/266/1 in 9.92s). The ARM failure is ONE
environmental substrate fault, not 266 bugs: 143 direct
`Bad file descriptor (os error 9)` panics across exactly three sites —
`sync_parent` fsync (observation.rs:44 ×79, transition.rs:413 ×13) and
the ext4-UUID ioctl (linux.rs:126 ×50) — with ~123 downstream
cascades. Diagnostic signature: `fstatfs` succeeds on the same
descriptor while `fsync`/`ioctl` return EBADF — O_PATH-class directory
descriptor semantics on the runner's default mount, a substrate the
suite was never evidenced against (the identity probe runs under sudo
on a purpose-made ext4 remount). Cheapest lead, tracked for the ARM
package: `query_error` (linux.rs:173-181) maps only
EOPNOTSUPP/ENOSYS/ENOTTY/EINVAL to graceful `unsupported` — EBADF
falls through to a hard Io error instead of a capability downgrade.
The earlier "macOS burn-down" framing collapses to: macOS is green
after the LFS fix; the real platform work is the ARM64 substrate
class.

**Green state: revalidated at run 13 (1322/0/1 on `90d3f8a`).
First green: run 11 (`31897404688` on `f36d20d`, 2026-08-16):
1306 passed / 0 failed / 1 ignored + integration 29/0 — the first
fully green Windows matrix on record. Eleven-run burn-down:
126 → 118 → 95 → 93 → 64 → 52 → 24 → 9 → 34 (recorded regression) →
1 → 0. The platform matrix is ACCEPTED for the purposes of
`GwzMergeCheckpoint-v0.10.5.md` item 5.** Green does NOT close the
standing residual tripwire (see that section below): real-Windows
exact-evidence satisfiability on filter-materialized worktrees and the
`stash_save` filtered reset remain reviewed-open decisions; CI is
structurally blind to both by fixture hermeticity.

# Windows matrix classification — run 31880022755

Date: 2026-08-15
Evidence: workflow `windows-matrix.yml` (dispatch-only, non-publishing,
contents:read), run `31880022755` on `74041ac`, windows-2022,
rustc 1.95.0. Totals: lib suite **1193 passed / 124 failed / 1 ignored**;
integration `29`-suite 27/2; all other suites green. 126 unique failures.
This supersedes the C1-era "98 failures" record in
`GwzMergeCheckpoint-v0.10.5.md`: the tree grew (C2 + gates) and two former
classes died — **zero `os error 87` occurrences** (the `d84a30d`/`a350746`
corrections, verified incorporated at `f532b1a`, hold on a real runner)
and no compile failures (the tree builds clean on Windows for the first
time on record).

## Classes, by root cause

| # | Class | ~Count | Sample | Kind | Disposition |
| --- | --- | ---: | --- | --- | --- |
| W1 | Git index-path containment: "index path is not inside the actual Git directory" | 28 | pre_catalog preflight/catalog tests | product (path canonicalization: verbatim/`\\?\`, 8.3, case) | R2-F product package: canonicalize both sides with the existing no-follow identity helpers before containment compare |
| W2 | `refs/heads/main` not found | 19 | v1_lifecycle reverse/rollback, g15 | test fixtures (runner lacks `init.defaultBranch`; libgit2 defaults to `master`) | test-only package: fixtures create the branch explicitly |
| W3 | Access denied (os error 5) on preservation artifacts/markers | ~28 | g15 root preservation, reverse_preservation | product (Windows ACL/read-only attribute vs reopen-for-DELETE pattern) | R2-F product package: open disposition audit on artifact reopens |
| W4 | Sharing violation (os error 32) | ~13 | catalog publish/final, durability suites | product (retained handles vs rename/delete sharing semantics) | R2-F product package: share-mode audit where retained handles overlap renames |
| W5 | Exact assertions + NotFound + residue | ~36 | mixed; incl. 10 `left == right`, 5 path-NotFound, cascades of W1-W4 | mixed (path spelling in expectations; cascade) | reclassify after W1-W4 land; most are expected to collapse |
| W6 | Protocol currency (2) | 2 | `generated_*_protocol_is_current` | infra (CRLF/generator env on Windows) | normalize line endings in the currency comparison or gate the test to Unix with the pinned-CI variant kept |

## Consequences

- The **resume-order classification item is complete**; per
  `GwzMergeCheckpoint-v0.10.5.md` item 5, R4b-G remains gated until the
  platform matrix is *accepted*, i.e. W1-W4 product packages and the W2
  fixture package land and a re-run is green (or a reviewed rescope says
  otherwise). W2 is mechanical; W1/W3/W4 are real Windows-semantics work
  in the checked-artifact/preservation layer and belong to the R2-F lane.
- The per-commit lane gate and boundary/retained-readers push workflows
  ran on the same push; their outcomes are recorded in the checkpoint.
- The dispatched workflow is retained for re-runs: each fix package ends
  with a fresh dispatch and an updated classification here.

## Run 2 — 31881596373 on `39b66aa` (post W2 + Linux-lint fixes)

Totals: **1201 passed / 116 failed / 1 ignored** + integration 27/2 = 118
unique (−8 net). `refs/heads/main` occurrences fell 20 → **1** (one
straggler with a non-`new_git` fixture path — first task of the next
W-package). The net drop is smaller than the 19 W2 clears because
formerly-W2 tests now progress into the W3/W4 walls: `os error 5`
panics rose to 37, confirming the classification's cascade prediction
(W5 collapse direction). Also on this push: the boundary workflow's
**first recorded green run** (`31880974224`, after the Linux
`redundant_guards` fold at `6edb9cb`) — the standing red since 2026-08-14
is resolved and the per-commit lane gate has its first successful CI
execution. Remaining matrix work: W1 (index-path containment, 28-class),
W3 (access-denied, now ~37), W4 (sharing violations), W6 (2 protocol
currency) — the R2-F product packages.

## Run 3 — 31882396053 on `4297c17`: W1 CLOSED

Totals: **1224 passed / 93 failed / 1 ignored** + integration 27/2 = 95
unique (−23 net). `index path is not inside` occurrences: **0** — the W1
class is fully cleared by the canonicalized-parent comparison. Remaining
leaders: W3 error-5 (preservation artifact 16, bundle 9, plus workspace/
test-artifact residue) and W4 error-32 (12 bare + 10 at "publish final
catalog" — the catalog seam's directory rename meeting a held handle
without delete-sharing on Windows; the failing edge is already named by
its label, so W4's catalog slice needs sharing-mode reasoning at that
edge, not more instrumentation). W5 assertion residue: 12.

## Run 4 — 31882850251 on `94fc8ca`: W4 catalog slice CLOSED, W3 named

Totals: 1224/93 (class shift at constant count). "publish final catalog"
error-32: **0** (was 10) — the caller's staging capability now drops
before the rename edge. The 31-site instrumentation named every surviving
error-5 in one run: **all 35 are `sync family entry`** (rebarrier_exact:
read-only reopen + `sync_all`, which Windows' FlushFileBuffers forbids).

## Run 5 — 31883193051 on `cf1ccaa`: W3 CLOSED

Totals: **1255 passed / 62 failed** + 27/2 = 64 unique (−31).
`os error 5`: **0** — the family-entry re-sync is Unix-only per the
write-through doctrine, with the anchor barrier unchanged. Remaining:
error-32 sharing (13 bare + 10 at "reopen publication source directory" —
the primitive's second capability lacking DELETE-share against the
rename handle, fixed next commit via `open_dir_share_delete` + 5 other),
7 assertion residue, 3 NotFound, 2 matrix-row escapes to diagnose.

## Run 6 — 31883655778 on `a506a93`: publication share-delete CLOSED

Totals: **1267 passed / 50 failed** + 27/2 = 52 unique (−12). The
"reopen publication source directory" class is gone. Six-run burn-down:
**126 → 118 → 95 → 93 → 64 → 52**, with W1, W2, W3, and both W4 catalog
slices extinct at zero occurrences each.

Run 6b — 31885262853 on `117117e` (workflow-only: rust-cache step; no
tree change): **1267/50/1 + 27/2, identical to run 6** — the 52-failure
tail is deterministic across runs, no flake component. The run also
exposed that `rust-cache`'s save step skips on job failure, and this job
is red by design until the tail clears; `cache-on-failure: true` rides
the next train. Duration recalibration across runs 1-6b: every matrix
run completes in 6-10 minutes (not the ~20 assumed while planning), so
the cache is a minor win and batching fix packages per dispatch is the
dominant accelerator.

Remaining tail, for the next session (per-site work, not batch classes):
(a) ~20 error-32 sharing failures scattered one-per-site across
fault-injection tests (`g15/root_preservation/{faults.rs:153,221,384,
mutation.rs:192}`, `reverse_preservation/{root_fault_matrix.rs:118,
root_durability.rs:236}`, and bare test-helper unwraps) — each callback's
substitution strategy conflicts with a handle production legitimately
holds at that hook on Windows; per-site redesign of the injection
(substitute-by-handle or drop-then-substitute) rather than a shared fix.
(b) **Two production-side findings needing real scrutiny** — DIAGNOSED,
see `GwzWindowsMatrix-ExactEvidenceDiagnosis.md`. Corrected counts:
**4×** RecoveryEvidenceMismatch (class A: CRLF smudge at production
rollback/recovery checkout edges vs the raw-byte exact-state compare —
model-consistency amendment required) and 2× PreservationEvidenceMismatch
(class B: the Windows-only `.gwz/checked-artifacts` durability anchor is
excluded from the guard image but not from capture/decode/stash sweep —
genuine production bug, latent on any OS with crash residue). The
`Stopped(RecoveryRequired) != Terminal` family (~7 tests) is the same two
roots surfacing through the service loop. Both fixes change production
semantics → reviewed amendment package; label-only confirmation
instrumentation dispatched on probe branch
`probe/exact-evidence-diagnosis`. (c) 6 assertion spellings, 3 NotFound, W6 taut generator
(1-2), and 2 matrix-row catch_unwind escapes.

Pre-run-7 disposition of tail (c) (second diagnosis pass + probe branch;
patches ride the run-7 train unless noted):
- **"2 catch_unwind escapes": misclassification — zero exist.** Both
  harnesses caught their injected panics (the classifier counted default
  panic-hook prints); the tests fail downstream in the (b) cascades.
- **Probe branch `probe/exact-evidence-diagnosis` (run 31886400209,
  ~30s)**: windows-2022 ships system `core.autocrlf=true`
  (`C:/Program Files/Git/etc/gitconfig`, git 2.55.0.windows.3) — class
  A's environmental precondition is proven.
- **W6 is not CRLF**: `ModuleNotFoundError: No module named 'taut'` —
  the matrix workflow never installed the pinned generator release.yml
  uses; fixed by mirroring `taut-proto==0.8.1` install. `.gitattributes`
  gains `protocol/** text eol=lf` (also fixes the `include_str!`
  assertion A1).
- **os-87 twin found**: `preservation_root/parent.rs::rename_windows`
  still passed a non-NULL `RootDirectory` — the d84a30d/a350746 class,
  missed at its twin site; fixed by porting platform.rs's absolute-path
  + null-RootDirectory recipe (NEEDS-WINDOWS-VERIFICATION via run 7).
- Fixture hermeticity: g15 `seeded_repo` pins `core.autocrlf=false`;
  root_preservation `support.rs` pins `core.longpaths=true` +
  `core.autocrlf=false` (MAX_PATH: staged `ca1-*.source` names ≈173
  chars breach it — product ticket tracked for the private-area
  relocation option). `recovery_protocol.rs` residue test forks on
  Windows to expect the fail-closed anchor-missing rejection.
- Deferred to the exact-evidence amendment lane: `capture_inner`
  exclusion, `ensure_clean_recovery_state` anchor-dirt (rollback
  preflight permanently blocked on Windows — production), CRLF checkout
  semantics, resolver Publication-arm diagnosability (tracked).

## Run 7 — 31886821459 on `f2fceaf`: sharing + small tails CLOSED

Totals: **1279 passed / 24 failed / 1 ignored** + integration **29/0**
(was 27/2) = 24 unique (−28). Seven-run burn-down:
**126 → 118 → 95 → 93 → 64 → 52 → 24.** Every train element verified on
first dispatch: zero `os error 32` (parent.rs publish-handle release +
15 gates hold), zero `os error 87` (the parent.rs rename twin fix —
NEEDS-WINDOWS-VERIFICATION discharged), zero path-too-long (longpaths
pin), zero `include_str!`/assertion residue (`.gitattributes` +
fixture pins), W6 integration currency green (taut-proto install), the
positive guarantee test `retained_directory_blocks_substitution_rename_windows`
passed, and the `cfg(windows)` additions compiled clean. rust-cache
saved on the red run (`cache-on-failure`) — run 8+ starts warm.

**Standing residual tripwire (amendment F6/Code P2 debt).** Two
real-Windows exposures are deliberately NOT closed by the exact-evidence
amendment and have no failing CI sentinel (the run-7 fixture
`core.autocrlf=false` pins make CI structurally blind to them):
(1) libgit2 `stash_save`'s internal filtered reset of swept tracked
files (no checkout-options lever exists in git2 0.21/libgit2 1.9.6);
(2) unrewritten smudged files — recovery checkouts rewrite deltas only,
so filter-materialized paths unchanged between the two commits stay
CRLF and the full-tree raw-byte observers classify Ambiguous on
ordinary Windows-CRLF worktrees (availability loss, fail-closed, never
wrong evidence). Both are recorded in
`GwzM5-8ExactEvidencePlatformAmendment.md` (Clause A scope limits);
this entry is the live tracking record. **Update 2026-08-16 (D1/D2
landed):** exposure (2)'s gwz-born half is CLOSED by construction —
`create_repo` pins filter neutralization at creation and the clone
funnel materializes filters-off — and the adopted-worktree half now has
a live sentinel
(`doctrine_sentinel_adopted_crlf_worktree_classifies_ambiguous_in_the_reverse_observer`,
Windows-gated, g12): it asserts today's fail-closed Ambiguous doctrine
and going green means the doctrine changed and the frozen texts must
move with it. Exposure (1) (`stash_save` filtered reset) remains
sentinel-less and open — its fix is a new mutation needing its own
review. Do not read matrix-green as real-Windows exact-evidence closure
for ADOPTED worktrees; the post-A1 `renormalize` command is the tracked
follow-up.

The 24 survivors are **exactly the predicted (b) cluster**, nothing
else: 4 × g15/root_preservation (stash ×2 + mutation = class B anchor
contamination; observation/marker_presence = the anchor-dirt preflight
finding), 13 × reverse::preservation and 6 × reverse::rollback service
tests (class A CRLF + anchor cascades per the diagnosis blast-radius
list), and finalization_root's masked-resolver failure (same family
suspected; resolver diagnosability tracked if it survives the
amendment). The matrix is one reviewed package — the exact-evidence
amendment train — from expected green.

## Run 8 — 31889856607 on `731fcb2`: exact-evidence amendment verified

Totals: **1297 passed / 9 failed / 1 ignored** + integration 29/0 = 9
unique (−15). Eight-run burn-down: **126 → 118 → 95 → 93 → 64 → 52 →
24 → 9.** The amendment's classes cleared on first dispatch: every g15
root-preservation survivor (anchor class B + the anchor-dirt preflight
finding B′) and the root-side reverse cascades (phases, entry,
root_ambiguity, root_durability ×2, half the successor matrix,
publication_root, native-conflict abort) are green on Windows.

The 9 survivors are one mechanism plus one wildcard: eight are
member-level service loops (reverse preservation faults/real_git/
recovery/root_fault_matrix/successor short-graphs, rollback
service_durability/service_fault_matrix) whose fixtures were never
autocrlf-pinned — the runner's machine-level filter smudges tracked
files through libgit2 `stash_save`'s internal reset (the documented
no-lever residual), producing exact-evidence Ambiguous downstream; plus
`finalization_root` (cause still masked by the resolver's
Publication-arm error discard — diagnosability edit tracked). Fix:
fixture hermeticity at the shared `commit_file` config site
(`workspace_ops/tests/g02.rs` — one line covering every v1 fixture;
the deliberate autocrlf=true repro sets its filter after fixture
commits and is unaffected). This closes the TEST exposure only; the
real-user exposure remains the standing residual tripwire above.

## Run 10 — 31894795103 on `a55d20b`: correction verified, ONE failure left

Totals: **1305 passed / 1 failed / 1 ignored** + integration 29/0.
Ten-run burn-down: **126 → 118 → 95 → 93 → 64 → 52 → 24 → 9 → 34
(regression, below) → 1.** The creation-time fixture pins cleared all
eight member-loop survivors AND returned g23/g25 to green. Runtime
note: runs 8-10 take ~35 min (vs 6-10 min for runs 1-7) because the
formerly fast-failing reverse service suites now run their full
real-git loops.

Sole survivor on the entire matrix:
`finalization::root_tests::unborn_publication_uses_the_exact_checked_first_commit_candidate`
— its cause still masked by the resolver's non-participant arm
(`authority/resolver/execution.rs:16-21` discards code/message/detail).
Diagnosability probe dispatched on branch `probe/finalization-diagnosis`
(propagates the discarded error into the reject message; single-test
trimmed workflow). **Probe run 31896698402 unmasked it in one 0.35s
test execution: `config value 'user.name' was not found`** — the
unborn-root scenario drives a PRODUCTION first-commit, and this is the
only fixture whose root never passes through `commit_file` (unborn = no
fixture commits), so it never inherits the identity config; runners
carry no machine-level git identity, developer Macs do — a
host-dependent test visible only on CI. Production behavior is correct
(git refuses identity-less commits). Fix: `pin_fixture_identity` at the
fixture root + the resolver's non-participant arm now propagates the
discarded cause permanently (the bare label cost a run-10→probe
diagnosis cycle). Run 11 is the expected full-green run.

## Run 9 — 31892216186 on `2e04f58`: REGRESSION (recorded per L1-16), corrected

Totals: **1272 passed / 34 failed / 1 ignored** (9 → 34). The eight
targeted fixture-smudge survivors cleared, but the `2e04f58` fix was
wrongly scoped: pinning `core.autocrlf=false` inside the shared
`commit_file` flipped the key **mid-life** in every fixture repo,
including the clone-based g23/g25 families — files a filtered
clone/checkout had already materialized as CRLF became permanent
manufactured dirt against their LF blobs once the clean filter turned
off ("cannot switch a dirty worktree to a different commit",
`g23/fixtures.rs:159`), breaking ~29 previously-green v0
characterization/abort/continue/pull tests. Mechanism lesson recorded:
**the pin is safe only at repo creation, before any filtered
materialization; clone-based fixtures must stay consistently
filter-aware.** Correction: the global pin is reverted; a documented
`pin_fixture_autocrlf` helper (g02.rs) states the precondition and is
applied at the ten creation sites inside the three v1 reverse/
finalization fixture files only (created empty, never cloned).
`finalization_root` remained failing in run 9 (masked cause;
resolver-diagnosability probe next if it survives run 10).

## W1 fix (landed at `4297c17`)

W1 root cause confirmed at `provider/index.rs`: raw `PathBuf` equality
between GWZ's recorded git-directory spelling and libgit2's reported index
path — guaranteed to diverge under Windows 8.3 short names
(`RUNNER~1`-class temp paths). Fixed by comparing canonicalized *parents*
(the always-present anchor; the index file itself may not exist) plus an
exact `index` file-name check. W3 instrumentation plan for the next
session: the error-5 opens route through `CheckedArtifact::acquire`'s
read-only observation chain (`observation.rs` acquire → ambient root Dir →
`durable_identity`/`canonical_path_identity` → traverse), so the failing
syscall is not identifiable from the current error text; add the failing
operation name to the io-error labels in that chain (labels only, no
behavior), push, re-dispatch, then fix the identified Windows semantics
(candidates: directory opens without backup semantics in the identity
helpers; read-only-attribute interactions in replace/quarantine paths).
