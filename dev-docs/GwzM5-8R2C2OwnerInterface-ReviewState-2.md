# R2-C2 owner interface state/filesystem re-review (round 2)

Date: 2026-08-15

Reviewer designation: State-2

Filing note: this report is the reviewer's verbatim text. The reviewer
replaced its original "Exact reviewed tuple" section and [T1] command block
in a transcription-repair pass (same session, no claim changed) after the
original contained struck hash-transcription attempts; the repair was
requested and verified by the implementation lane before filing.

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 2 |

Per `dev-docs/AgentProcessRules.md` L1-19, a gate is NO-GO while any
P0/P1/P2 is open; P3 blocks only where a checkpoint requires zero P0-P3.
RemPlan §9 item 12 requires no open P0/P1/P2 from both re-reviews. This
review finds none. Both P3 findings below have bounded consequences,
concrete corrections, and route to the existing RemPlan/R2-F tracking
rather than a new remediation round.

## Exact reviewed tuple and worktree state

| Repository | Verified HEAD | Status |
| --- | --- | --- |
| gwz-dev (root) | `d2b4c772a84ed8601e32a8f8381dd8a3ad278a4c` | clean |
| gwz-core | `c436180d5909389005baa4f876982f5f941af856` | clean |
| gwz-cli | `3cca145c0b32410f250f640730ed7ca18f1da59f` | clean |
| gwz-py | `929efb0d440206772155cadda89748e9431f9ca8` | clean |
| taut | `f00841966663bb0d500974ede446d30e9bfeeac4` | clean |

All five values were taken mechanically from `git rev-parse HEAD` with
empty `git status --porcelain`, identically at review start and at the
final recheck, and match the requested tuple exactly.

No implementation, design, plan, lock, or marker file was modified. No
staging, commit, or push was performed. The only writes were `target/`
build artifacts and self-cleaning temp-dir test fixtures.

Reviewed delta since the round-1 core commit `1ef95b4`: all
`src/checked_artifact` changes (31 files; the
catalog/pre_catalog/publication/platform/fault/tests set enumerated in the
Commands section) plus release-line `workspace_ops/merge` changes inherited
through the v0.10.4/v0.10.5 merges, which are covered by the v0.10.5
checkpoint reviews and are outside the catalog lane. The complete C2
catalog delta was read, not sampled.

## Prior-finding closure

Round-1 report: `dev-docs/GwzM5-8R2C2OwnerInterface-ReviewState.md`
(NO-GO; 2×P1, 2×P2).

**[Round-1 P1 #1 — owner stopped after active publication; production
adapter mapped staging/final to `Other`; `Complete` unreachable; no
physical evidence for the later edges] — CLOSED on this tuple.**
`CatalogOwnerV1::execute_one` now implements all seven decisions
(`gwz-core/src/checked_artifact/catalog/bootstrap.rs:164-224`):
CreatePrivateParent, WriteOrRewriteScratch, PublishActive,
PrepareOrRewriteStaging, PublishFinal, RetireActive, Complete, with
Ambiguous as typed read-only failure. The production aggregate now
classifies staging/final through the closed interior grammar
(`provider/aggregate.rs:85-106` →
`provider/interior.rs::directory_fact/staging_plan/completed_record`),
implementing exactly the amendment §5 ordered prefixes (empty;
retired-actions; roaming; anchor-at-B; anchor-at-A-with-B-absent;
descriptor; format), zero-length/partial next-file repair
(`classify_expected_prefix`: empty bytes classify as
PartialExpectedPrefix), the bounded ten-slot interior with overflow probe
and name-budget charging, and `ExactOwned` completed recognition binding
the final directory's durable identity into the canonical infrastructure
record (identity preservation is therefore durably enforced, not merely
observed). Physical evidence now executes to `Complete`: fresh workspace
and Git-directory convergence tests;
`every_completed_edge_survives_full_lease_reacquisition` (restart with
full lease reacquisition between every edge, exactly 10 retries asserted);
`zero_and_partial_next_files_recover_for_every_staging_file_role` (zero
and half-prefix for all four staged file slots); anchor A-to-B-to-A
restart at all three intermediate positions; unowned staging/final
read-only rejection with foreign bytes asserted untouched;
retained-catalog return with named-final and byte-identical interior
substitution rejection; and the two interruption/restart/convergence
matrices (21-key workspace, 23-key Git-directory) with set-equality
against the declared family and non-vacuity (`interrupted.is_err()`)
asserted per row. Every element of the round-1 required correction is
present and executed green here (241/241).

**[Round-1 P1 #2 — `OpaqueRetainedCatalogV1` did not retain the completed
catalog object] — CLOSED on this tuple.**
`OpaqueRetainedCatalogV1` now wraps `CompletedCatalogPermitV1` (target
witness + retained platform root + `RetainedCompletedCatalogV1`).
`retain_completed_catalog` (`provider/completed.rs:49-132`) opens the
final directory through the observed identity, retains the final `Dir`
handle plus all six authority-bearing interior handles, proves each
retained handle equals the freshly observed named object by identity and
bytes, and then runs a full `revalidate` (named-final vs retained-handle
identity equality, per-handle identity, fresh bounded interior
re-observation, exact completed-plus-retired grammar) before returning.
Substitution of the named final or a byte-identical interior file after
return is rejected by `revalidate` (both executed as tests). No raw handle
escapes; the only production escape remains
`Complete(OpaqueRetainedCatalogV1)`. The narrow C3 operations over this
capability are future scope by design; the round-1 state-safety substance
— a retained, identity-pinned exact final catalog under the still-live
target lease — is delivered.

**[Round-1 P2 #1 — pathname mutation after freshness check; truncate
before identity check] — CLOSED.**
`write_or_rewrite_scratch` opens with `truncate(false)`, verifies the
opened handle's identity and bytes against the fresh aggregate row,
re-verifies the name still binds that object, and only then truncates
through the verified handle (`provider/mutation.rs:126-151`); the same
discipline applies to interior rewrites
(`directory_mutation.rs::write_slot`). Publication routes through the
sealed primitive (`provider/publication.rs::publish_verified_no_replace`):
reopen no-follow through the retained parent, opened-identity compare,
byte compare for regular files, rename of the retained open handle
(Windows) / retained-parent-relative no-replace rename with the checked
handle held (Unix), `replace=false` hardcoded. The exact deterministic
hooks round 1 demanded exist and are exercised with read-only assertions:
`ScratchBeforeOpen`, `ScratchAfterOpen`, `PublishBeforeRename`, plus
`StagingAfterOpen`, `FinalPublishBeforeRename`, `ActiveRetireBeforeRename`,
`CompleteAfterFinalOpen` on the directory edges. The seam is enforced
twice structurally:
`interface_tests/capability_permit.rs::catalog_publication_uses_one_source_associated_seam`
(exactly six call sites, no raw `platform::rename_relative` in the
callers) and the boundary script's shape-and-caller-count pin
(`check_checked_artifact_boundaries.py:631-634, 785-831`), both green
here.

**[Round-1 P2 #2 — recovered exact scratch not flushed before durable
rename] — CLOSED.**
`publish_active_record` now reopens the scratch no-follow, verifies
identity and canonical bytes against the fresh observation, `sync_all`s
the source, and only then enters the sealed primitive (which re-verifies
again) (`provider/mutation.rs:210-228`). The demanded fault edge exists
and executes: the matrix row `catalog_bootstrap.scratch_write` interrupts
after bytes and before flush; the restart classifies, republishes with the
mandatory re-flush, and converges to `Complete` with a retained-catalog
revalidation — in both target variants.

**Audit P3 dispositions (`dev-docs/GwzM5-8R2C2PublicationAudit.md`),
assessed from state safety:**

- **P3-1 (140 reserved dormant keys; RemPlan §10 rescope).** Sound. I
  verified: the reserved families have no `fault_v1::hit` sites (only
  `mutation.rs`, `directory_mutation.rs`, `aggregate.rs` inject, all
  `catalog_bootstrap.*`); the §6 parent edge is now keyed
  (`git_parent_create`/`git_parent_reobserve`), the family is 23 keys, the
  total inventory is pinned at exactly 163
  (`interface_tests/fault_expected_keys.rs`), and both matrices execute
  all family keys with set-equality. Critically, scoping exit-gate item 9
  to `catalog_bootstrap.*` does **not** leave any implemented durable edge
  without executed interruption coverage: the legacy
  leaf/transition/residue/cleanup edges retain their own executed fault
  mechanism (`CheckedArtifactFault`, exercised by
  `checked_artifact/tests/{durability,recovery_protocol,staging_recovery,removal_recovery,exact_source}.rs`),
  and the capability-neutral runtime lock grammar is exercised through
  `RuntimeBootstrapFault` in `bootstrap/runtime/tests.rs` and
  `catalog_lease/tests.rs`. The rescope is a vocabulary-unification debt
  (R2-D+), not a coverage hole. One doc note: RemPlan §10 still says "21
  keys" and "currently unkeyed"; the keying it forecast has now landed (23
  keys), and `CurrentProgramCheckpoint.md` — the state authority — records
  this correctly, so no correction is required beyond normal supersession.
- **P3-2 (sealing perimeter unpinned interior).** Still real and still
  tracked. The seam check scans only the provider tree;
  `platform.rs::rename_relative` remains callable from the legacy interior
  (it is, in `prepare_private`/`private_barrier` and the legacy
  transition/residue paths). Fail-closed today: `recover_or_create` has no
  production caller (verified by grep — definition, sealed re-export, and
  tests only), so legacy and catalog namespaces are not co-reachable.
  Concur with the bounded-package disposition.
- **P3-3 (legacy/catalog coexistence).** Physically confirmed
  (`policy.rs::private_parent` is the catalog Final path). Direction of
  failure verified safe: any legacy `ca1-*` residue inside a would-be
  Final classifies as an unowned interior child → typed read-only
  ambiguity; no adoption, no mutation. R2-D acceptance criterion stands.
- **P3-4 (three strict-window tests).** Implemented at `95d292f` and
  judged below to test what they claim.

## Findings

### [P3-1] The Git-parent AlreadyExists arm and adopted-parent recovery never re-establish the Git-directory dirent barrier

- **Where:**
  `gwz-core/src/checked_artifact/capability/pre_catalog/provider/mutation.rs:89`
  (`Err(source) if source.kind() == AlreadyExists => Ok(())`, skipping
  `finish_private_parent_edge`), together with the Ready-adoption path
  (`provider/retained.rs::retain_optional_child` + preflight), which
  retains a present `gwz` without ever flushing the containing
  Git-directory edge.
- **Violated invariant:** amendment §6/§5 durability story. §6 specifies
  the Unix flush of the containing Git-directory edge for the create arm,
  and §5 declares "the first durable ownership declaration is the nonempty
  dynamic scratch, created write-through, flushed through its file
  handle." Under the strict portable crash-ordering model this codebase
  otherwise adopts (explicit parent-directory fsync after every namespace
  edge), the scratch's — and every later record's — durability is
  contingent on the `git-dir → gwz` dirent, which no path other than the
  create-arm success ever fsyncs.
- **State sequence:** (1) process A: Missing → missing-parent permit →
  revalidate → `create_dir` succeeds → crash before
  `finish_private_parent_edge` (this exact window is the executed matrix
  key `catalog_bootstrap.git_parent_reobserve`); (2) process B restarts:
  preflight observes `gwz` present (VFS-visible, durability unknown) →
  Ready → `create_git_private_parent` never runs again → scratch, active,
  staging, final, retired are all built and flushed with `gwz`-relative
  and file-level barriers only → `Complete` returned; (3) second power
  loss: on a filesystem honoring only explicit-parent-fsync ordering, the
  `gwz` dirent may be unpersisted, atomically unlinking the entire
  completed bootstrap; durable state returns to original Missing. The
  AlreadyExists arm is the same hole entered through the §6 no-replace
  race: the entrant's dirent durability is unknown, the arm returns Ok
  without a barrier, and full re-entry adopts the entrant as parent. Note
  the arm itself is near-unreachable by cooperating processes (the lease
  serializes them and the permit revalidation proves absence syscalls
  earlier), so its practical trigger class is the same-user
  non-cooperator plus the crash-restart adoption path.
- **Impact:** bounded. Loss is all-or-nothing at the `gwz` dirent, so
  every post-loss state is a classifiable protocol state (fresh Missing,
  or the fully surviving prefix); there is no misclassification, no
  wedged ambiguity, no partial adoption, and no foreign-fact damage;
  re-bootstrap converges. What is lost is the durability meaning of
  `Complete` on the Git-directory lane in a two-fault scenario — a
  guarantee the amendment explicitly waives for Windows ("loss of the
  empty parent is simply the original missing state") but never states
  for the Unix adopted-parent path. No production caller consumes
  `Complete` at this tuple, so there is no reachable consequence today.
- **Required correction:** re-run `finish_private_parent_edge` (the
  retained-root fsync) on the AlreadyExists arm, and issue the same
  one-time barrier when a Git-directory preflight first retains a present
  `gwz` parent; alternatively (minimum), amend §6 to state the Unix
  adopted-parent convergence rationale explicitly, mirroring the Windows
  text, so the durability claim and the implementation agree.
- **Regression test:** a Git-directory matrix variant that injects an
  entrant directory via the `catalog_bootstrap.git_parent_create` callback
  hook (driving the AlreadyExists arm for real — it is currently
  enumerated-adjacent but never executed) and asserts the edge completes
  with the parent barrier issued; if the doc-only option is taken, a
  doc-gate anchor tying §6's Unix text to this arm.

### [P3-2] Windows publication destination binding is path-witnessed, and the in-code comment overclaims handle protection

- **Where:** `gwz-core/src/checked_artifact/platform.rs:98-140`
  (`rename_open_source`, Windows arm: `GetFinalPathNameByHandleW` on the
  retained destination `Dir`, `RootDirectory = null`, absolute-path
  `FILE_RENAME_INFO`), comment at `:121-125`.
- **Violated invariant:** §4.1 destination binding parity of the sealed
  primitive. On Linux/macOS the no-replace rename is issued relative to
  the retained destination directory handle, so the destination is bound
  to the proved directory object. On Windows the destination directory is
  converted to an absolute pathname immediately before
  `SetFileInformationByHandle`; within that window a rename of the
  destination directory — or of any path ancestor, including the
  workspace root, which is not part of "GWZ's private catalog namespace"
  and therefore not cleanly inside §4.1's stated same-user exclusion —
  makes the rename resolve by stale name. The comment's claim that the
  cap-std directory open "prevents the target directory from being renamed
  or deleted while this operation runs" is not established: standard
  directory opens grant `FILE_SHARE_DELETE`, which permits renames of the
  open directory.
- **State sequence:** source opened and identity/byte-verified
  (handle-pinned, correct per §4.1) → destination absolute path derived
  from handle → [window] destination directory or ancestor renamed and a
  directory appears at the stale absolute path → the rename delivers the
  verified protocol object into a foreign directory; no-replace protects
  only the leaf name.
- **Impact:** Windows-only, syscall-window-narrow, requires a
  non-cooperating same-user actor — the threat class §3/§4.1 place outside
  the authentication boundary, but via an ancestor-rename route the
  recorded trust-boundary text does not literally cover. All six §4.1
  edges share the arm. No Windows behavior executes at this tuple (R2-F
  gate), so nothing green today overstates this; the §4.1 source-handle
  claim itself ("pathname replacement after the handle is opened cannot
  select a different object") remains true and is what the in-source
  `cfg(windows)` test exercises.
- **Required correction:** record the path-witnessed destination binding
  as an explicit R2-F acceptance criterion; correct the `platform.rs`
  comment to state what the handle actually guarantees; add the
  native-Windows window test below to the R2-F matrix. If a
  handle-anchored destination mechanism is later admitted, it must land
  inside the sealed seam (dual review per L2-06).
- **Regression test:** `cfg(all(test, windows))` companion to
  `rename_open_source_moves_the_checked_object_after_path_substitution`
  that renames the destination directory (and separately an ancestor)
  between path derivation and rename, asserting the operation fails with
  the source untouched and never delivers into the replacement directory;
  executes at R2-F native CI.

## State-machine and durability verification

The state machine and every physical mutation were reconstructed from
source, independently of the tests.

**Owner loop and ordering.** `recover_or_create` = `begin_preflight`
(witness revalidation) → per iteration: `preflight_catalog_target`
performs one complete lease-bound observation (retain
root/repository/common/private-parent with no-follow child opens, index
snapshot with private-collision rejection over
exact/ancestor/descendant/stage/flag/gitlink/equivalent spellings and the
entire reserved scratch family, bounded parent enumeration through the
closed name grammar, per-role no-follow opens with bounded byte reads and
bounded interiors) and immediately revalidates the bound target against
the lease witness (identity, invocation identity, rename domain, mode,
repository relationship, support profile, canonical paths —
`provider/filesystem/bound.rs:55-133`). Classification is the closed
seven-row decision table (`catalog/classifier.rs:210-275`); every other
tuple, including active+retired both present, scratch+active both
present, unowned directories, non-unique or foreign-target recovery
evidence (`digests.rs::select_historical`), oversize bytes, wrong kinds,
and interior aliases, is `Ambiguous` and read-only. Every
`execute_owner_*` then (a) consumes the single-use typed edge token, (b)
re-runs `revalidate_observation` — a full fresh re-observation whose
digest must equal the retained fresh digest, plus lease-root binding and
witness revalidation — and (c) re-classifies and requires the same
decision and expected record before touching the provider. Each physical
function then re-verifies each involved object by opened-handle identity
(and bytes for files) against the retained rows before mutating through
that handle, re-verifies after, and syncs. Every successful edge discards
the permit and re-enters complete preflight; only `Complete` escapes, and
only after `retain_completed_catalog`'s own re-verification and full
revalidate. Randomness is requested only under a ready permit for a fresh
attempt; the zero token is rejected; recovered attempts always reuse the
durable token/digests (executed: fresh values are never substituted,
historical survives unrelated index change, foreign-target evidence
rejects).

**Physical mutations, exhaustively:** (1) Git `gwz` create — no-replace
`create_dir` through the retained actual-Git-directory handle, reopen
no-follow, then Unix parent fsync (Windows deliberately none, per §6) —
subject to [P3-1] on the non-create arms; (2) scratch create/rewrite —
create_new XOR observed-row guard, verified-handle truncate/write, file
`sync_all` (F_FULLFSYNC-backed on macOS), written-identity re-verify by
handle and by name, `gwz` fsync; (3) scratch→active — pre-verified and
flushed source, sealed primitive, post-rename named verification, `gwz`
fsync; (4) staging create — no-replace, reopen, `gwz` fsync; (5) interior
slot writes — one slot per edge per the plan grammar, same verified-handle
discipline, file flush plus staging fsync (with
`InfrastructurePopulate`/`InfrastructureFlush` keys bracketing); (6) the
single sanctioned compound edge — anchor B→A publish/verify, A→B, verify,
B→A, verify (all through the sealed primitive, each subedge keyed),
descriptor create, staging fsync; every intermediate anchor position is an
enumerated repairable prefix and each is an executed restart case; (7)
staging→final — final-absent and exact-staging preconditions from the
fresh aggregate, staging fsync, named re-verify, a second bounded interior
re-observation through the open handle inside the strictest window, sealed
primitive on the directory (identity-preserving), post-rename identity
verification of the reopened final, parent fsync; (8) active→retired —
exact pre-retirement final and exact active-byte preconditions, fresh
interior re-check through the open final handle, sealed primitive with the
destination bound to the verified final handle, named retired
verification, final fsync then parent fsync; (9) completion — bounded
re-verification and retention of all seven objects. Cross-directory
rename tearing is excluded by the admitted profiles (local APFS with
persistent object IDs; local ext4 with `FS_IOC_GETFSUUID` +
`name_to_handle_at`; remote filesystems rejected), and were it ever
observed, the both-present tuple classifies as read-only ambiguity rather
than misrecovery.

**Substitution windows.** Identity on macOS/Linux is durable (volume UUID
+ persistent object ID / ext4 UUID + kernel file handle with generation)
plus dev+ino invocation identity, with no timestamps — so the new
byte-drift test genuinely reaches the primitive's byte-compare branch
rather than passing on an incidental identity change, and inode reuse
cannot alias a kind swap. Judgement on the three 95d292f tests: (a)
in-place byte drift with unchanged identity at `PublishBeforeRename` —
lands after the pre-open verify/flush/drop and before the primitive, is
caught only by the primitive's reopen byte compare, asserts drifted bytes
preserved and no active created: tests exactly what it claims; (b)
destination created inside the window — exercises the no-replace EEXIST
path with foreign destination and source both preserved: as claimed; (c)
kind swap inside the window — directory at the scratch name, caught by
opened-identity mismatch on the primitive's `maybe_dir` reopen, directory
left untouched: as claimed. The remaining in-window residue — a same-user
pathname swap injected after the primitive's last check on Unix — is
exactly the documented §4.1 trust boundary, and the implementation neither
widens nor misrepresents it.

**Locks.** The lease set implements the §4 two-phase protocol: bounded
batch (4,096 cap, max-plus-one iterator consumption, fallible reservation
throughout with an injectable allocation-failure hook),
canonical-location dedup with membership-witness merging (never
discarding a witness, rejecting divergent membership), allocation-free
unstable sorts (structurally asserted), phase-one per-target transient
guard acquired and released before the next target with only fixed
runtime lock grammar converged, phase-two canonical-order
open-existing/lock/revalidate with reverse-order release on any failure
or contention and on Drop, and no guard acquisition after the first final
lock. The witness re-proves the held lock slot (named inode equals locked
handle), target and related-Git identities, modes, rename domains, and
paths on every `facts()` call and before every edge. Contention and every
association failure are read-only.

## Commands and exact results

All from the exact tuple; gwz-core commands from `gwz-core/`.

```text
[T1] git rev-parse HEAD                    → d2b4c772a84ed8601e32a8f8381dd8a3ad278a4c
     git -C gwz-core rev-parse HEAD        → c436180d5909389005baa4f876982f5f941af856
     git -C gwz-cli rev-parse HEAD         → 3cca145c0b32410f250f640730ed7ca18f1da59f
     git -C gwz-py rev-parse HEAD          → 929efb0d440206772155cadda89748e9431f9ca8
     git -C taut rev-parse HEAD            → f00841966663bb0d500974ede446d30e9bfeeac4
     git status --porcelain (all five)     → empty, both at start and at final recheck
git -C gwz-core diff --stat 1ef95b4..c436180 -- src/checked_artifact scripts
                                           → 24 files, +3,845/−237 (complete C2 delta read)
cargo test --lib checked_artifact::        → ok. 241 passed; 0 failed (9.03s, macOS host)
cargo test --lib checked_artifact::catalog::bootstrap::tests -- --nocapture
                                           → ok. 12 passed; 0 failed
       (includes restart_and_substitution_matrix_covers_every_catalog_bootstrap_fault_key
        and restart_and_substitution_matrix_covers_git_directory_targets — all 23 keys
        interrupted, restarted, converged, retained catalog revalidated, non-vacuity asserted)
cargo test --lib checked_artifact::capability::pre_catalog::provider::mutation_tests -- --nocapture
                                           → ok. 6 passed; 0 failed (three round-1 hook tests
                                             + three 95d292f strict-window tests)
python3.13 scripts/checks/check_checked_artifact_boundaries.py
                                           → checked-artifact boundary: ok
                                             (15 visible entries, 5 classified modules)
cargo fmt --all -- --check                 → PASS
git diff --check / git -C gwz-core diff --check → PASS
rustup target list --installed             → x86_64-pc-windows-msvc present
cargo check --target x86_64-pc-windows-msvc --lib
                                           → FAILED in dependency libz-sys (cc-rs cannot drive
                                             the host cc for the msvc target); no Windows
                                             compile signal obtainable from this host.
                                             Environment limitation, not a product finding.
```

Structural cross-checks read and confirmed: `fault_expected_keys.rs` pins
the full inventory at exactly 163 keys with duplicate/missing/extra
reporting; the boundary script's tree digests are whole-tree (root file
plus descendant directory), so the deliberate c436180 digest updates for
`capability/pre_catalog.rs` and `catalog.rs` cover the edited
`provider/mutation.rs` and `catalog/bootstrap/tests.rs`; the publication
seam is pinned both by shape (one `publish_verified_no_replace`, one
`open_rename_source`, one `rename_open_source`, no `rename_relative` in
`publication.rs`) and by caller-count table {mutation.rs: 1,
directory_mutation.rs: 5} with provider-tree caller-set equality; the
budget literals are exactly the amendment's 4,096 / 255 / 510 / 2,088,960;
the scratch name is exactly 241 bytes and `parse` forces canonical
lowercase spelling by re-encode comparison.

## Residual risks and unexecuted evidence

- **Native Linux and Windows execution: none on this tuple.** Everything
  green here is macOS (APFS, case-fold parents — so the case-fold
  alias/budget branches are the host-executed branches; ext4
  sensitive/casefold branches and all Windows arms are unexecuted). This
  is where amendment §5/§8 and RemPlan place it (R2-F release gate), but
  it must be said plainly: the Windows write-through/no-directory-fsync
  durability model, the `SetFileInformationByHandle` arm, the
  exact-handle association test, and [P3-2]'s window are all source-only
  claims from this seat. Windows compilation could not even be
  cross-checked from this host (libz-sys toolchain limit above); the last
  Windows compile evidence is the implementation lane's own commits
  (`c2ec63c`, `e07eb28`, `da58135`) plus the known Windows backlog in
  `CurrentProgramCheckpoint.md`.
- **Matrix interruption granularity.** Each fault key is interrupted at
  its first occurrence per run (the callback slot is single-shot,
  ordinals unused). Later occurrences of repeating keys (e.g.
  `infrastructure_populate` across the five staging iterations,
  `catalog_enumerate` on every re-entry) are not separately interrupted;
  this is materially compensated by the exactly-10-restarts edge walk and
  the constructed zero/partial states for every staged file slot, but it
  is bounded, not exhaustive, executed evidence.
- **Power-loss (dirent-loss) semantics are reasoned, not executed.** The
  in-process matrix simulates process death; no row simulates a
  lost-but-previously-visible dirent (the [P3-1] double-fault). The
  all-or-nothing convergence argument for that case is analytic.
- **Destination-in-window EEXIST is executed on one edge**
  (scratch→active); the other five edges share the primitive's code path
  but have no dedicated destination-race row.
- **The AlreadyExists arm itself is enumerated-adjacent but never
  executed** (no test creates the entrant inside the revalidate→create
  window); [P3-1]'s regression test covers this.
- **Sealing perimeter and legacy coexistence** (audit P3-2/P3-3) remain
  tracked packages: the legacy interior can still name `rename_relative`,
  and the legacy private root is physically the catalog Final directory —
  currently fail-closed because catalog bootstrap has no production
  caller and foreign interiors classify as read-only ambiguity, but R2-D
  must convert before activation.
- **RemPlan §10 wording** ("21 keys", "currently unkeyed") is superseded
  by the checkpoint's 23-key record; supersession is governed by the
  checkpoint's authority note, no separate correction filed.

## Final tuple recheck and next action

Final recheck (post-review): all five repositories at the exact requested
tuple — root `d2b4c772a84ed8601e32a8f8381dd8a3ad278a4c`, gwz-core
`c436180d5909389005baa4f876982f5f941af856`, gwz-cli
`3cca145c0b32410f250f640730ed7ca18f1da59f`, gwz-py
`929efb0d440206772155cadda89748e9431f9ca8`, taut
`f00841966663bb0d500974ede446d30e9bfeeac4` — all worktrees clean. This
review made no workspace changes; the only expected root status entry
after filing is this report at
`dev-docs/GwzM5-8R2C2OwnerInterface-ReviewState-2.md`.

Verdict **GO** from the State-2 axis: both round-1 P1s and both P2s are
closed with executed evidence on this tuple; the audit's P3 dispositions
are sound; the two new findings are P3 and route to existing tracking
([P3-1] into the RemPlan §10 next matrix package or a §6 errata; [P3-2]
into the R2-F Windows acceptance criteria plus a comment fix). Next
action per `CurrentProgramCheckpoint.md`: reconcile with the paired
re-review's verdict; if it also reports no open P0/P1/P2, record R2-C2
acceptance at this checkpoint and proceed down the ordered list (Windows
compile-correction port and matrix classification before any catalog
activation, then the docs package, then R4b-G → M5b → A1), carrying
[P3-1]/[P3-2] and the P3-2/P3-3 audit packages as tracked, non-blocking
obligations.
