# R2-C2 dirent-barrier remediation state/filesystem review

Date: 2026-08-15

Reviewer designation: State-DirentBarrier

Review tier: focused single-axis (State) review of a bounded production
change behind the accepted R2-C2 interface, per `GwzProcessOptimization.md`
§4.2 and the tier recorded in advance in `CurrentProgramCheckpoint.md`
("Open — State-2 [P3-1] ... production-behavior change → focused State-axis
review after"). Read-only: no file in any repository was modified; the only
writes were `target/` build artifacts and self-cleaning temp-dir fixtures.

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 2 |

Per `dev-docs/AgentProcessRules.md` L1-19, a gate is NO-GO while any
P0/P1/P2 is open; none is found here. **Second-axis escalation is not
warranted**: §4.2 escalates on any P0, P1, or P2, or on reviewer request; I
find none and do not request escalation. Both new findings are P3 with
bounded consequences and concrete corrections, and route to existing
tracking (the same package family as the original State-2 [P3-1] residue
and the standing R2-F native-matrix item).

## Exact reviewed tuple

| Repository | Verified HEAD | Status |
| --- | --- | --- |
| gwz-dev (root) | `e0124b25e28ae23030c2d4e720a09e8bf39b22b3` | clean |
| gwz-core | `660f46c4cd4c580f3ffc922c877538058294fc85` | clean |
| gwz-cli | `3cca145c0b32410f250f640730ed7ca18f1da59f` | clean |
| gwz-py | `929efb0d440206772155cadda89748e9431f9ca8` | clean |
| taut | `f00841966663bb0d500974ede446d30e9bfeeac4` | clean |

All five taken mechanically from `git rev-parse HEAD` with empty
`git status --porcelain`, identically at review start and final recheck.
gwz-cli/gwz-py/taut match the `CurrentProgramCheckpoint.md` table
unchanged. The root commit `e0124b2` is the coordinated pin/journal commit
for the member commit (same `GWZ-Commit-ID` trailer; touches only
`gwz.conf/gwz.lock.yml` and the commit-journal YAML). gwz-core is the
accepted `0d8382e` plus three checkpoint-recorded gate/docs commits
(`641f03c`, `ca520e4`, `89b414a`) plus the review object.

**Review object:** the single commit
`660f46c4cd4c580f3ffc922c877538058294fc85` "Anchor the Git-root dirent
barrier to the scratch edge" (parent verified `89b414a`, the
checkpoint-recorded lane-gate commit). Complete diff read, every hunk: 5
files, +38/−5 (`scripts/checks/check_checked_artifact_boundaries.py` — two
protected-tree digest lines;
`src/checked_artifact/capability/pre_catalog/provider/mutation.rs`;
`src/checked_artifact/catalog/bootstrap/tests.rs`;
`src/checked_artifact/fault_v1.rs`;
`src/checked_artifact/interface_tests/fault_expected_keys.rs`). Host:
macOS (Darwin 25.5.0, APFS), cargo/rustc 1.95.0.

## Finding closure (State-2 [P3-1])

Round-2 report: `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewState-2.md`,
finding [P3-1]: the Git-parent AlreadyExists arm and adopted-parent
recovery never re-established the Git-directory dirent barrier, so a
two-fault sequence (crash in the parent-create window, restart-and-complete
by adoption, then power loss) could atomically unlink a completed
bootstrap's containing dirent under the strict portable crash-ordering
model.

**CLOSED on this tuple.** The original counterexample and both of its
variants are closed, verified independently from source and by executed
evidence:

1. **Create-arm crash → adopt** (the finding's exact state sequence;
   executed windows `catalog_bootstrap.git_parent_create` /
   `.git_parent_reobserve`). The restarting process observes a present
   parent and nothing else; classification is WriteOrRewriteScratch
   (`catalog/classifier.rs:218-225`); `write_or_rewrite_scratch` now cannot
   return success without `finish_private_parent_edge(retained.root()
   .handle())` succeeding (`provider/mutation.rs:188`, `?`-propagated on a
   linear path). The containing-root dirent is therefore durable before any
   later durable role exists. Executed: both git-matrix rows interrupt,
   restart, and converge with retained-catalog revalidation.
2. **AlreadyExists entrant → adopt.** The arm now runs
   `finish_private_parent_edge(parent)` before returning Ok
   (`mutation.rs:89-94`) — the same idempotent barrier as the create arm,
   issued before adoption-by-retry, without opening or trusting the entrant
   (the arm only fsyncs the retained root's dirent list; entrant
   kind/alias vetting remains preflight's job per §6). Executed for real by
   the new regression `entrant_git_parent_inside_the_creation_window_
   converges` (analysis below confirms it drives the AlreadyExists arm,
   not the create arm).
3. **Pre-existing adopted parent.** First preflight returns Ready; the
   first mutation on a fresh-but-adopted target is necessarily the scratch
   edge (no other role can exist), whose tail issues the root barrier
   before success.

**Barrier ordering against the §5/§3/§6 durability claim.** On the create
and AlreadyExists paths the root dirent is durable **before** the first
durable ownership declaration exists. On adoption paths it is durable
**with** it: within the same owner edge, ordered bottom-up (scratch file
`sync_all` → gwz dirent-list fsync → containing-root fsync), all before
the edge reports success. On every path on which the scratch edge
*completes*, `Complete` is unreachable with a non-durable containing
dirent. The correction implemented differs from the finding's literally
prescribed correction (barrier at first preflight retention of a present
parent) — the implementer chose a mutation-edge anchor, correctly
preserving the read-only preflight (see State-machine verification) — and
per L1-21 what closes a finding is the original counterexample, which is
closed. The delta between the prescribed and implemented anchors is a
strictly narrower, higher-fault-order residual window, filed as new
finding [P3-1] below rather than held against closure.

The implementation matches the checkpoint's pre-recorded package shape
exactly (AlreadyExists-arm barrier + scratch-edge root flush + one new
matrix key + entrant-arm regression). Windows disposition: axis answer in
the State-machine section; summary — the §6 no-op stance satisfies the
finding as filed, no new R2-F item required.

## Findings (new only)

### [P3-1] Resume paths that re-enter after the scratch bytes are complete skip the root barrier; a narrower strict-model window to a non-durable containing dirent at Complete remains

- **Where:** the anchor at
  `gwz-core/src/checked_artifact/capability/pre_catalog/provider/mutation.rs:188`
  combined with `catalog/classifier.rs:234-241` (exact scratch →
  PublishActive, skipping the scratch edge) and
  `catalog/bootstrap.rs:164-224` (the owner dispatch). No edge after the
  scratch edge ever syncs `retained.root()`: `publish_active_record` ends
  at `sync_published_namespace(parent)` (gwz only, `mutation.rs:260`), and
  every `sync_directory_edge` call in `directory_mutation.rs` (lines 100,
  166, 214, 275, 360-361, 501) targets staging, final, or the gwz parent —
  verified exhaustively by grep; `finish_private_parent_edge` has exactly
  the three call sites in `mutation.rs`.
- **Violated invariant:** the same §5/§3/§6 durability story as State-2
  [P3-1], plus the commit's own rationale scope (the lane's claim that the
  scratch-edge anchor closes the window "on every path since every
  bootstrap passes through the scratch edge after adoption" — every
  bootstrap does not: a restart whose observation is an exact scratch, or
  any later state, re-enters after the anchor point).
- **State sequence (workspace lane — no non-cooperator required):**
  runtime bootstrap creates `.gwz` with no workspace-root flush anywhere
  (`bootstrap/runtime/paths.rs:133-150` — `ensure_child_directory` has no
  parent fsync; no sync call exists in the runtime bootstrap path) →
  catalog scratch edge crashes at `catalog_bootstrap.scratch_write` or
  `.scratch_flush` (both executed matrix keys: bytes VFS-complete, root
  barrier at line 188 not yet reached) → restart classifies Exact →
  PublishActive → staging → final → retire → `Complete`, with no root
  fsync on the entire resume path → power loss; on a strict
  explicit-parent-fsync-ordering model the `.gwz` dirent may revert,
  atomically unlinking the completed bootstrap (and the runtime lock
  tree). Two faults: one crash in a one-edge sub-window plus one power
  loss. Git-directory lane: identical shape, but additionally requires the
  parent to be adopted rather than owner-created (entrant pre-creation
  before first preflight, or a prior create-window crash raising it to
  three faults). A non-crash variant exists: an fsync *failure* (Err) at
  line 188 leaves the same resumable exact-scratch state, and every retry
  thereafter skips the barrier permanently.
- **Impact:** bounded; the identical class State-2 rated P3. Loss is
  all-or-nothing at the parent dirent; every post-loss state is a
  classifiable protocol state (fresh Missing or the surviving prefix); no
  misclassification, no partial adoption, no foreign damage; re-bootstrap
  converges. No production caller consumes `Complete` at this tuple. The
  commit strictly improves both lanes — pre-commit the workspace lane had
  *no* explicit workspace-root barrier at all (a zero-fault gap State-2's
  git-scoped finding did not name; the lane-neutral anchor was the right
  choice) and the git entrant inlets were one-fault — so this residual is
  one fault order narrower everywhere than what it replaces.
- **Required correction (either):** (a) issue the same idempotent root
  barrier once in the common Ready-edge mutation prologue (the
  `execute_owner_*` dispatch layer, keeping preflight read-only), or
  equivalently at the head/tail of each namespace-mutating edge — six
  idempotent fsyncs at bootstrap frequency, negligible; or (b) a bounded
  §6-style erratum recording the strict-model residual and its fault-order
  calculus as accepted, mirroring the existing Windows waiver text. Route
  with the original finding's tracking (next matrix package / R2-F
  power-loss semantics).
- **Regression test:** fixture with a pre-planted parent (entrant before
  first preflight, or plain workspace), panic armed at
  `catalog_bootstrap.scratch_write`; converge on restart; assert the root
  barrier is issued on the resume drive before `Complete` — mechanically:
  arm a non-panicking callback at the barrier's key on the resume drive
  and assert the slot is consumed (today it demonstrably is not, since
  `scratch_root_flush` is unreachable on that drive).

### [P3-2] The barrier helper's rationale text and error label were not generalized for the new call-site set; the §5 miscitation is propagated

- **Where:** (a) `mutation.rs:183` — the new comment cites "amendment §5"
  for "the first durable ownership declaration is the nonempty dynamic
  scratch"; in `dev-docs/GwzM5-8R2CCatalogBootstrapAmendment.md` that
  sentence is §6 (line 555), with the variant in §3 (line 155); §5 (lines
  318-511) does not contain it. The miscite is inherited verbatim from
  State-2's own finding text and the checkpoint's plan line ("where §5's
  durability claim begins") — three documents now propagate it. (b)
  `mutation.rs:379-383` — the Windows `finish_private_parent_edge` comment
  ("This edge publishes no durable authority") is now false for the
  scratch-edge caller, where the edge publishes the *first* durable
  authority; the no-op remains behaviorally correct there, but for the §5
  write-through/NTFS-metadata-ordering reason (as stated on
  `sync_created_file_namespace`), not the stated empty-parent reason. (c)
  `mutation.rs:375` — a failure of the new call surfaces as "flush created
  Git GWZ parent" on workspace-target scratch edges, where nothing Git and
  nothing created is involved.
- **Violated invariant:** L1-16-adjacent documentation accuracy: rationale
  comments on a durability barrier must let a reader verify the claim
  against the controlling text and the actual call context.
- **Impact:** bounded documentation/diagnosability defect: future
  verification is pointed at the wrong amendment section, and a rare
  fsync-failure diagnostic misdescribes its context. No behavioral
  consequence.
- **Required correction:** one comment/label pass on the helper: cite
  §3/§6, generalize or parameterize the Windows-arm rationale and the
  error label for the enlarged caller set; optionally note the citation
  correction where the next document touches the checkpoint plan line. No
  behavior change; no re-review needed beyond the next routine gate.
- **Regression test:** none (comment/label only); the Phase-2
  section-anchor doc-gate tripwire already tracked in the checkpoint is
  the right mechanical home.

## State-machine verification

Reconstructed from source at the exact tuple, independently of the tests.

**Claim-by-claim verification of the remediation.**
(1) AlreadyExists arm: `mutation.rs:89-94` — confirmed; the arm now calls
`finish_private_parent_edge(parent)` where `parent` is
`retained.root().handle()` (line 69), the same handle the create arm
flushes; Unix = `platform::sync_parent`, Windows = deliberate no-op.
(2) Scratch-edge anchor: `mutation.rs:182-193` — order is `write_all` →
`ScratchWrite` hit → `sync_all` → `ScratchFlush` hit → written-identity
re-verify by handle and by name → `sync_created_file_namespace(parent)`
(gwz) → `finish_private_parent_edge(retained.root().handle())` →
`ScratchRootFlush` hit → Ok. Bottom-up flush order is correct.
`retained.root()` is verified to be the directory holding the private
parent's dirent in both variants: `retain_workspace` binds `root` to the
canonical worktree root and retains `.gwz` as its child
(`retained.rs:203-208`); `retain_git_directory` requires `root_path ==
repository.path()` and retains `gwz` as its optional child
(`retained.rs:226-246`). The barrier is handle-based (no pathname
resolution; no TOCTOU), on a per-edge-revalidated retained handle.
(3) Fault key: `fault_v1.rs:58` in family order; family counted at exactly
24 `catalog_bootstrap.*` keys; `EXPECTED_KEY_COUNT` 164 with the
duplicate/missing/extra set comparison executed green.
(4) Matrices: workspace `mapped` = 22 keys executed, plus set-equality of
22+2 git-only against the full family; git-directory `mapped` = 24 keys
executed with the same set-equality. Non-vacuity per row: the armed
callback panics, `catch_unwind` must return Err ("fault point was not
reached" otherwise), then a fresh lease acquisition must converge to
`Complete` with retained-catalog revalidation. The `scratch_root_flush`
row therefore *asserts* that the barrier line executed and succeeded (the
hit is only reachable through the `?` on line 188) on both target
variants, and that the post-barrier crash state classifies (Exact →
PublishActive) and converges from every restart path.

**The entrant test drives the AlreadyExists arm, not the create arm.**
`run_next_catalog_fault` is a direct alias of `fault_v1::run_next_at`
(tests.rs:12-14): it arms a single-shot callback and injects no error. The
`CatalogBootstrapGitParentCreate` hit sits *before* `create_dir`
(`mutation.rs:71-75`), so during the one convergence drive the callback
creates `commondir/gwz` inside the revalidate→create window, `create_dir`
then fails AlreadyExists, and the arm — including its new barrier — runs
for real before full re-entry adopts the entrant and converges. Vacuity is
excluded from both directions: if the hit were deleted, the git matrix row
fails its "fault point was not reached" assertion; if the hit migrated
after `create_dir`, the callback's `fs::create_dir(...).unwrap()` panics
on AlreadyExists and the test fails loudly.

**Side-effect safety (workspace targets).** The barrier now fsyncs the
workspace root once per scratch edge. This is not merely defense-in-depth:
it is the first explicit workspace-root→`.gwz` dirent flush anywhere in
the catalog stack (runtime bootstrap's `ensure_child_directory` performs
none), so the workspace lane gains the durability property rather than
merely paying an fsync. No classification impact (nothing is written; a
barrier failure is a typed io error → fail-closed retry). Cost is one
directory fsync per bootstrap-scratch edge — bootstrap-frequency only.
**Read-only-path confirmation:** the barrier lives only in the two
mutation edges; grep of `preflight.rs`, `retained.rs`, and `aggregate.rs`
finds zero `sync` calls of any kind, and the commit touches no
preflight/observation file. Preflight remains strictly read-only.

**Windows (axis 4, explicit).** State-2 [P3-1]'s required correction had
no Windows component: the finding itself recorded the amendment's Windows
waiver ("loss of the empty parent is simply the original missing state")
as already given, and demanded the barrier (or errata) for Unix only. The
commit's deliberate Windows no-op therefore **satisfies the finding as
filed via the §6 doctrine; the R2-F item is not newly required by this
finding's Windows aspect.** Precision, though: §6's waiver sentence covers
the *empty* parent; once the scratch exists, the gwz dirent's power-loss
durability on Windows rests on §5's write-through model plus NTFS metadata
journal ordering (as encoded in the `sync_created_file_namespace` Windows
comment) — a source-only claim at this tuple, already owed at the standing
R2-F native Windows power-loss/fault matrix (§5's closing sentence; the
checkpoint's R2-F line). Recommendation, not a finding: fold the
adopted/nonempty-parent power-loss case explicitly into that existing
R2-F matrix item. Also note the `ScratchRootFlush` hit is outside the
`cfg(not(windows))` split, so both matrix rows remain non-vacuous on a
future native Windows run.

**New-defect sweep of every changed line.** Boundary script: exactly the
two protected-tree digest lines for the two edited trees changed, verified
by executing the checker (ok, exit 0) — the same-commit digest-refresh
lane rule is honored. `mutation.rs`: arms and anchor as analyzed; the
added `?` on `sync_created_file_namespace` preserves prior semantics; the
AlreadyExists arm's new fsync can only convert a prior silent Ok into a
typed error (fail-closed); the arm does not open, adopt, or trust the
entrant. `tests.rs`: the two matrix list insertions keep set-equality
honest; helper reuse is sound. `fault_v1.rs` / `fault_expected_keys.rs`:
key naming, ordering, and the 163→164 pin are consistent and executed.
Scope: the diff stays within the catalog lane's files and matches the
checkpoint's pre-recorded package shape ("one new matrix key,
entrant-arm regression"); the new key is test-only vocabulary (`hit` is
`cfg(test)`), no wire change. Defects found by the sweep are exactly the
two findings above.

## Commands and exact results

All from the exact tuple; gwz-core commands from `gwz-core/`. Toolchain:
cargo 1.95.0 / rustc 1.95.0, macOS host (Darwin 25.5.0, APFS).

```text
git rev-parse HEAD (all five) + git status --porcelain (all five)
    → tuple table above; all clean; identical at start and final recheck
git -C gwz-core rev-parse 660f46c^        → 89b414aa4c360e7824aa12b342c767bd7f23fca8
git -C gwz-core show 660f46c --stat       → 5 files, +38/−5 (full diff read)
cargo test --lib checked_artifact::       → ok. 244 passed; 0 failed (8.89s)
                                            (prior tuple 243; +1 = entrant test)
cargo test --lib checked_artifact::catalog::bootstrap::tests
                                          → ok. 13 passed; 0 failed
    (includes restart_and_substitution_matrix_covers_every_catalog_bootstrap_fault_key
     — 22 keys interrupted+restarted+converged, set-equality 22+2 vs family 24 —
     and restart_and_substitution_matrix_covers_git_directory_targets
     — all 24 keys interrupted+restarted+converged, incl. scratch_root_flush)
cargo test --lib checked_artifact::catalog::bootstrap::tests::entrant_git_parent_inside_the_creation_window_converges -- --exact
                                          → ok. 1 passed; 0 failed
cargo test --lib checked_artifact::interface_tests::fault_expected_keys
                                          → ok. 2 passed; 0 failed (164-key pin)
python3.13 scripts/checks/check_checked_artifact_boundaries.py; echo exit=$?
                                          → checked-artifact boundary: ok
                                            (15 visible entries, 5 classified modules); exit=0
cargo fmt --all -- --check                → PASS
git diff --check (gwz-core and root)      → PASS
```

Structural cross-checks read and confirmed: `fault_v1.rs` family count
(24 `catalog_bootstrap.*` of 164 total); both matrices' set-equality
blocks; the three `finish_private_parent_edge` call sites are the only
root-directory fsyncs in the catalog bootstrap; `directory_mutation.rs`
sync targets never include the root; amendment section placement of the
durability sentence (§3:155, §6:555; absent from §5).

## Residual risks

- **The [P3-1] resume-window above** — the one substantive residual of
  this package; strictly narrower than what it replaces on every path
  (workspace: crash-in-one-sub-window + power loss; git: additionally an
  entrant or a second crash). Power-loss (dirent-loss) semantics remain
  reasoned, not executed, exactly as State-2 recorded.
- **No fault key inside the gwz-fsync→root-barrier sub-window.**
  `scratch_flush` interrupts before the gwz fsync and `scratch_root_flush`
  after a *successful* barrier; the few-instruction interval between the
  two flushes is uninstrumented. The neighboring rows cover the same
  restart classification (Exact → PublishActive), so this is granularity,
  not a coverage hole in classification.
- **Barrier reached-ness on ordinary convergence drives** is implied by
  the linear `?` chain rather than asserted per row; only the
  `scratch_root_flush` rows assert reaching it. Acceptable, worth knowing.
- **Native Linux/Windows execution: none on this tuple** (inherited). The
  Windows write-through/NTFS-ordering doctrine backing the no-op stance is
  source-only until the R2-F native matrix; fold the nonempty-parent
  power-loss case into that existing item.
- **Matrix first-occurrence granularity** and the State-2 sealing/
  coexistence packages (audit P3-2/P3-3, R2-D) remain tracked, unchanged
  by this commit.

## Final tuple recheck and next action

Final recheck (post-review): all five repositories exactly as the tuple
table above — root `e0124b2`, gwz-core `660f46c`, gwz-cli `3cca145`,
gwz-py `929efb0`, taut `f008419` — all worktrees clean
(`git status --porcelain` empty). This review made no workspace changes;
the only expected root status entry after filing is this report at
`dev-docs/GwzM5-8R2C2DirentBarrier-ReviewState.md`.

Verdict **GO** from the State-DirentBarrier axis. State-2 [P3-1] is CLOSED
on this tuple: all three adoption paths of the original counterexample now
issue the containing-root dirent barrier before or with the first durable
ownership declaration, with executed matrix and entrant-arm evidence, and
the Windows aspect is satisfied by the §6 doctrine as the finding itself
framed it. The two new findings are P3 and non-blocking: [P3-1]
(resume-window residual) routes to the next matrix/durability package or a
bounded §6-style erratum, alongside the R2-F power-loss item; [P3-2]
(helper rationale/label and the propagated §5 miscitation) is a
comment-pass plus the queued doc-gate anchor tripwire. Second-axis
escalation: not warranted (no P0/P1/P2). Next action per
`CurrentProgramCheckpoint.md`: record this package's closure against the
"Open — State-2 [P3-1]" obligation, carry the two P3s as tracked
non-blocking items, and proceed down the ordered list (item 5 docs
package, then R4b-G → M5b → A1), with R2-D and R2-F obligations
unchanged.
