# R2-C2 physical catalog-owner code re-review, round 3

Date: 2026-08-15

Reviewer designation: Code-3 (independent code/architecture axis; no
current-round peer report was read).

Filing note: filed verbatim by the implementation lane. The remediation of
this report's findings landed after this review on a successor commit; the
round-4 focused re-review verifies it. This report's tuple is the
pre-remediation `c436180` state.

## Verdict

**NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 1 |
| P3 | 5 |

The round-2 blocking race is closed for every regular-file publication: the
sealed primitive `publish_verified_no_replace` reopens the source no-follow
through the retained parent, re-verifies identity **and bytes** through that
handle, and renames while retaining it (exact-handle rename on Windows), and
the committed strict-window tests prove it at the tightest committed hook.
However, a deterministic disposable-export probe of exactly the round-2
shape shows the same defect class survives on the directory-source edge: a
foreign interior entry injected after the caller's last interior re-check
and before the primitive's source-capability acquisition is **published into
the reserved final destination**, contradicting amendment §4.1 ¶2's explicit
"directory interior … rejects before publication and leaves the observed
foreign fact untouched", and it leaves a permanently ambiguous catalog. That
is a P2 under L1-19. A second probe shows the structural gate accepts a raw
`platform::rename_relative` caller added inside the catalog provider itself
(retained.rs), which violates the RemPlan's binding sentence and amendment
§8.13; that P3 independently blocks under L1-19's exit-criterion clause.

## Exact reviewed tuple and worktree state

- workspace root `d2b4c772a84ed8601e32a8f8381dd8a3ad278a4c` — clean at start and end
- `gwz-core` `c436180d5909389005baa4f876982f5f941af856` — clean
- `gwz-cli` `3cca145c0b32410f250f640730ed7ca18f1da59f` — clean (unchanged since round 2)
- `gwz-py` `929efb0d440206772155cadda89748e9431f9ca8` — clean
- `taut` `f00841966663bb0d500974ede446d30e9bfeeac4` — clean
- `gwz status --json`: every member `lock_match: Matches`, dirty=false
  (including the taut-shape family). Workspace lock pins equal member HEADs.

No workspace file was written or staged by this review. All probes ran in
disposable `git archive` exports under the session scratchpad; the only
pristine-tree writes were `target/` build artifacts (permitted).

Scope reviewed: the full gwz-core delta `a02d7d6..c436180` (13 commits, 20
files, +1,023/−203), with the sealed primitive (`f532b1a`), the Windows
commits (`c2ec63c`, `e90de60`, `e07eb28`, `da58135`), the strict-window
tests (`95d292f`), and the matrix/keying/digest commit (`c436180`) read
line-by-line, plus all callers, visibility chains, gates, and the
generated/test surfaces. The `workspace_ops/merge` files bundled into
`f532b1a` (record_wire/location.rs, record_wire/mod.rs, root/abort.rs) were
read in full: they are a `#[cfg(test)]`-only rollback-test identity refactor
plus `pub(in crate::workspace_ops::merge)` visibility widenings; no
production behavior change, noted below as scope bleed. The release-line
commits (`6052a08`, `b4e83e0`, merges) touch only release.yml /
tests/publish_workflow.rs and are outside this seam.

## Prior-finding closure

### Round-2 P2 (post-verification publication race) — closed as stated for file sources; residual of the same class found on the directory source (new P2-1)

Round 2 proved: after the last `verify_named_file`, publication dropped the
verified handle and performed a pathname rename, so a hook-injected foreign
replacement was moved into the reserved active name before rejection.

Closure verified on this tuple:

- One sealed primitive exists: `capability/pre_catalog/provider/publication.rs:48-95`
  (`pub(super)`, provider-only visibility). It opens the source no-follow
  through the caller's retained parent (`platform.rs:23-76`), compares the
  opened handle's identity against the owner-observed identity
  (`publication.rs:63-70`), for regular files reads `len+1` bytes through
  the same handle and requires exact equality (`:71-85`), and renames with
  the handle retained (`:86-94`, `replace=false` hardcoded).
- Windows renames the exact checked handle via `SetFileInformationByHandle`
  (`platform.rs:98-140`); the object-binding test
  `rename_open_source_moves_the_checked_object_after_path_substitution`
  exists at `platform.rs:569-614` (`cfg(all(test, windows))`). Linux/macOS
  retain the handle across `renameat_with(NOREPLACE)` (`platform.rs:78-96`,
  `:222-246`) under the amendment's accepted same-user post-acquisition
  boundary; other Unix and unknown platforms fail closed on no-replace
  (`:248-268`, `:291-311`). No compare-and-rename claim is made anywhere.
- All six §4.1 edges route through the primitive: scratch→active
  (`mutation.rs:221`), staging→final (`directory_mutation.rs:232`),
  active→retired (`:318`), three anchor moves (`:574/:594/:609`). Count
  enforced by `interface_tests/capability_permit.rs:100-113` (exactly six)
  and the boundary script's seam check
  (`check_checked_artifact_boundaries.py:631-634`, `:785-831`).
- The round-2 counterexample shape is now a committed passing test:
  `mutation_tests.rs::replacement_before_active_rename_is_not_published`
  substitutes at `PublishBeforeRename` (after the caller's last
  verification) and asserts the replacement stays at scratch and active
  never appears. Identity is dev/inode-family only (`identity.rs`), so the
  new `in_place_byte_drift…` test genuinely exercises the primitive's byte
  branch.
- Round-2's alternative closure path — "an explicit controlling-design
  change" narrowing the claim — was taken for the Unix post-acquisition
  interval: amendment §4.1 (lines 265-296), reviewed under the amendment's
  own dual review.

**However**, round 2 explicitly listed the directory publication among "the
same unchecked interval exists in the remaining catalog publications." The
remediation moved identity binding into the primitive for all six edges,
but for the directory source (`PublicationSourceV1::Directory`,
`publication.rs:18-21, 34-36`) only identity is re-verified after
acquisition; the interior is last verified by the caller **before**
acquisition (`directory_mutation.rs:222-231`). My deterministic probe
(Finding P2-1) shows the round-2 failure shape — foreign content moved into
a reserved destination before rejection — reproduces there. The prior
finding is therefore closed for every regular-file edge and **still open in
residual form on the staging→final (and, same root cause, retire-into-final)
interior axis**.

### Audit P3 dispositions (GwzM5-8R2C2PublicationAudit.md)

- **P3-1 (fault vocabulary) — disposition sound and executed.** RemPlan §10
  filed; the §6 parent edge is keyed (`catalog_bootstrap.git_parent_create`
  at `mutation.rs:71-74` before `create_dir`, `.git_parent_reobserve` at
  `:83-86` after reopen+identify), family is 23 keys, inventory 163; both
  matrices execute (see below).
- **P3-2 (unpinned sealing interior) — disposition unsound as scoped.** The
  audit located the gap in the legacy interior (transition.rs, residue.rs,
  namespace/, coordinator/, cleanup.rs) and deferred it as a follow-up
  package "subject to reviewer concurrence". I do not concur: my Probe B
  shows the uncovered surface includes the **catalog provider tree itself**
  — a raw `platform::rename_relative` caller added to
  `provider/retained.rs`, with a routine self-service digest refresh,
  passes the boundary checker clean and is invisible to the interface test
  (which reads only mutation.rs and directory_mutation.rs). That
  contradicts the RemPlan's binding sentence "The structural gate must
  reject direct raw rename calls from the catalog provider outside the
  primitive" (RemPlan:987-988) and amendment §8.13 "direct raw provider
  renames are structurally rejected" (amendment:636-639). Filed as
  exit-criterion-violating P3-1 below; it cannot ride behind this gate.
- **P3-3 (legacy/catalog namespace coexistence) — disposition sound.**
  Confirmed at the type level: the legacy private parent literally is
  `CatalogPrivateNameV1::Final.relative_path(..)`
  (`checked_artifact/policy.rs:33-42`), and the four legacy rename edges
  (`transition.rs:276/:369`, `residue.rs:380/:455`) plus the Windows anchor
  barrier (`platform.rs:332-440`) mutate inside it without the primitive.
  Not co-reachable today: `recover_or_create`/`CatalogOwnerV1` have **zero
  production callers** (verified by exhaustive grep; only test modules call
  them), so no live catalog can coexist with the legacy writers. Must be a
  recorded R2-D acceptance criterion, as the audit says.
- **P3-4 (three strict-window tests) — disposition sound for the file
  edge.** All three exist in `mutation_tests.rs:148-226` (in-place byte
  drift with unchanged identity; destination created inside the window is
  not replaced and is left untouched; kind swap left untouched) and pass.
  They cover the primitive's byte branch, the NOREPLACE/EEXIST branch, and
  the identity branch. The anchor moves share the identical primitive
  branches (acceptable without dedicated window tests); the directory
  interior cannot be covered this way at all, which is exactly Finding
  P2-1.

## Findings

### [P2-1] Directory publication binds object identity but not interior across the pre-acquisition window; foreign interior bytes are published into the reserved final destination and the catalog wedges

- **Where:** `gwz-core/src/checked_artifact/capability/pre_catalog/provider/publication.rs:18-21, 34-36, 63-70`
  (Directory variant carries identity only); `provider/directory_mutation.rs:222-239`
  (`publish_final_directory`: fresh interior observation and Complete
  re-check at `:222-231` happen strictly before the primitive's
  source-capability acquisition at `publication.rs:56`); same root cause at
  `directory_mutation.rs:309-325` (`retire_active_record`: fresh
  final-interior re-check at `:309-317` precedes the retire rename into
  that directory).
- **Violated invariant:** Amendment §4.1 ¶2 (lines 288-291): "Drift present
  before source-capability acquisition, including a substituted …
  **directory interior** … still rejects before publication and leaves the
  observed foreign fact untouched." Also §4.1 ¶2's own narrowing sentence
  does not cover this: the drift is injected **before** the last identity
  check, not after it. Round 2's required-correction precedent: a residual
  of this class is only acceptable via an explicit controlling-design
  narrowing, which exists for post-acquisition substitution but not for
  pre-acquisition interior drift.
- **Reproduction (deterministic, disposable exact-SHA export of c436180; no
  workspace change):** add one test-only hook variant
  `FinalPublishAfterInteriorRecheck` between the Complete re-check and the
  `publish_verified_no_replace` call; inject
  `fs::write(staging/"foreign", b"sliver drift\n")` at that hook; run the
  owner. Result (exact):
  ```
  foreign interior entry was published into the reserved final destination;
  final_exists=true,
  foreign_in_final=Some([115, 108, 105, 118, 101, 114, 32, 100, 114, 105, 102, 116, 10]),
  run=Err("Ambiguous { fact: \"catalog interior\", detail: \"catalog directory contains an unowned child\" }")
  ```
  Those bytes are exactly `sliver drift\n`. The foreign fact was moved into
  `.gwz/checked-artifacts` (the reserved final name) before any rejection;
  the primitive's identity compare passes because interior mutation does
  not change the directory object's identity (identity is
  filesystem/object id only, `identity.rs:31-105`).
- **Impact:** the reserved authoritative destination is created containing
  a foreign entry; every subsequent recovery classifies read-only ambiguity
  ("catalog directory contains an unowned child"), so the catalog can never
  converge — a permanently wedged state with no operator escape (the escape
  story is itself an open pre-A1 item). No false composition occurs (the
  ten-slot grammar rejects extras), which is why this is P2 and not P0. The
  window is microseconds and requires a same-user writer, but that is
  materially identical to the round-2 P2's window, and the accepted design
  text explicitly claims this case rejects before publication.
- **Required correction (either lane, explicitly decided):** (a)
  implementation — extend `PublicationSourceV1::Directory` to carry the
  expected interior and have the primitive re-verify the interior **through
  the retained post-open capability** (bounded: ten entries plus one
  overflow probe) before the rename, so the residual genuinely falls inside
  the accepted post-acquisition boundary on both Unix and Windows; apply
  the same acquisition-window re-check to the retire edge's
  destination-interior precondition; or (b) design — an explicit amendment
  erratum removing "directory interior" from the ¶2 pre-acquisition
  rejection list and assigning interior drift after the caller's
  identity-proved handle acquisition to the same-user boundary, re-reviewed
  as a controlling-design change. Option (a) matches the primitive's
  architecture; option (b) repeats the round-2 narrowing precedent and must
  not be made silently.
- **Regression test:** commit the two hook variants
  (`FinalPublishAfterInteriorRecheck`, `ActiveRetireAfterInteriorRecheck`)
  and tests that inject interior drift there and assert: rejection before
  publication, final absent (or retired absent), and the foreign fact left
  at its original name — the exact probe above, inverted to pass.

### [P3-1] The structural gate does not reject raw provider renames outside the two current caller files — violates RemPlan §"second-review publication correction" and amendment §8.13; blocking under L1-19's exit-criterion clause

- **Where:** `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:785-831`
  (seam check: `rename_relative` substring is checked only in
  publication.rs, mutation.rs, directory_mutation.rs; the caller-set scan
  tracks only `publish_verified_no_replace`);
  `interface_tests/capability_permit.rs:100-113` (reads only the same two
  caller files); `platform.rs:7-96` raw pieces are `pub(super)` at
  `checked_artifact` scope; crate root carries
  `#![allow(clippy::disallowed_methods)]` (`lib.rs:4-7`), so the lint layer
  fires only in the `PROTECTED_COMPILER_MODULES` forbid files.
- **Violated invariant:** RemPlan:987-988 "The structural gate must reject
  direct raw rename calls from the catalog provider outside the primitive";
  amendment §8.13 "direct raw provider renames are structurally rejected".
  Per L1-19 a P3 blocks when it violates an exit criterion.
- **Reproduction (disposable export):** appended to `provider/retained.rs`
  a `probe_raw_publish` function calling
  `crate::checked_artifact::platform::rename_relative(..., false, ...)`;
  refreshed the pre_catalog tree digest (the routine self-service step any
  legitimate edit performs); ran the checker: `checked-artifact boundary:
  ok (15 visible entries, 5 classified modules)`, exit 0. The 61-test
  checker suite contains no case for a raw-rename caller in an unchecked
  provider file (the three seam unit tests added at f532b1a mutate only the
  checked files).
- **Impact:** a future caller (human or agent) can reintroduce exactly the
  round-2 defect — separate check followed by raw rename — anywhere in the
  provider tree except the two pinned files, and every committed gate stays
  green. The compiler does not seal this (pub(super) at subsystem scope).
- **Required correction:** extend the checker to a subsystem-wide caller
  inventory for `rename_relative` / `open_rename_source` /
  `rename_open_source` (exact allowlist: publication.rs ×1 each,
  platform.rs interior, and the four legacy edges until R2-D converts
  them), i.e. pull the audit's P3-2 package **into** this gate rather than
  after it.
- **Regression test:** checker unit test that plants a raw-rename caller in
  an unchecked provider file (and one in the legacy interior) and asserts
  the seam finding fires.

### [P3-2] Windows destination is rebound by derived absolute path, and the load-bearing comment claiming cap-std prevents destination-directory rename is incorrect

- **Where:** `platform.rs:110-126` (da58135): destination =
  `GetFinalPathNameByHandleW(destination_dir)` + leaf,
  `RootDirectory = null`; comment at `:121-125` states the retained cap-std
  open "prevents the target directory from being renamed or deleted while
  this operation runs."
- **Violated invariant:** §4.1 "publishes no-replace into the already-proved
  rename domain" — on Unix the destination is fd-bound to the proved
  handle; on Windows it is re-resolved by pathname at rename time. The
  comment's mechanism claim is wrong: cap-std directory handles are opened
  with the std default sharing mode including `FILE_SHARE_DELETE`, so a
  same-user process can rename the destination directory while the handle
  is held; a swap-in at the same absolute path would redirect the
  publication.
- **Reproduction:** static; requires a native Windows host to demonstrate
  dynamically (R2-F). The exploit actor is the same-user post-acquisition
  adversary the amendment already excludes, and post-publish verification
  through the retained destination handle detects the redirect after the
  fact, read-only.
- **Impact:** bounded (excluded adversary, post-hoc detection), but the
  false comment invites future reliance on a non-existent guarantee, and
  the Windows destination binding is silently weaker than the design
  narrative.
- **Required correction:** fix the comment to state the true residual
  (destination path re-resolution window, covered by the same-user boundary
  plus post-publish retained-handle verification); record the Windows
  destination residual in §4.1's Windows paragraph in the same erratum as
  P2-1's resolution.
- **Regression test:** `cfg(all(test, windows))` test that renames the
  destination directory after `windows_destination_path` derivation and
  asserts the publish either fails or the post-publish retained-handle
  verification rejects (executable at R2-F).

### [P3-3] The protected-tree digest discipline broke at 95d292f: the mandatory boundary gate was red for one intermediate commit and the c436180 refresh silently absorbed the earlier edit

- **Where:** `scripts/checks/check_checked_artifact_boundaries.py:151-158`;
  commits `95d292f` (edited `provider/mutation_tests.rs` inside the
  pre_catalog protected tree without updating the pin) and `c436180`
  (refreshed the pin two commits later).
- **Violated invariant:** the digest gate's audit function — a pin refresh
  should certify exactly the edit it accompanies. Independent recomputation
  (this review, byte-identical algorithm): at `da58135` pin 325f3804… ==
  tree; at `95d292f` tree is 13249ed9… while the pin stayed 325f3804…
  (**checker red at that commit**); at `c436180` both refreshed pins
  (dd476465…, 4cc6e99d…) match the tree exactly. `git diff --name-only`
  confirms the only protected-tree changes across da58135..c436180 are
  mutation_tests.rs (95d292f), mutation.rs and catalog/bootstrap/tests.rs
  (c436180) — all read line-by-line in this review; **nothing was
  smuggled**, so the c436180 digest update covers exactly the intended
  edits.
- **Impact:** bisect on the gate breaks at 95d292f; the deferred-refresh
  pattern is precisely the laundering channel Finding P3-1 describes (a
  stale-then-refreshed pin blesses whatever accumulated in between unless a
  reviewer recomputes, as done here). The push CI
  (`checked-artifact-boundary.yml`, no path filter) only checks branch
  heads, so a multi-commit push hides the red commit.
- **Required correction:** same-commit pin refresh discipline for protected
  trees (lane rule or a pre-push hook running the checker per commit on the
  lane).
- **Regression test:** process-level; the lane rule suffices.

### [P3-4] The sealed primitive's byte verification allocates with `Vec::with_capacity` instead of the crate's fallible-allocation discipline

- **Where:** `publication.rs:72` (`Vec::with_capacity(expected_bytes.len() + 1)`).
- **Violated invariant:** L2-15 discipline as implemented by the immediate
  neighbors: `mutation.rs:321-336` (`read_bounded`) and
  `directory_mutation.rs:654-660` use `try_reserve_exact` with a typed
  `CheckedFsError::unsupported` on allocation failure. The sealed primitive
  — the highest-trust function on this seam — is the one spot that can
  abort on allocation failure instead of returning the typed error.
- **Reproduction:** code inspection; input is bounded, so a real failure is
  implausible — this is a parity/consistency defect, not a reachable abort
  in practice.
- **Impact:** inconsistent failure contract inside the primitive; a future
  caller passing larger expected bytes inherits an abort path.
- **Required correction:** replace with `try_reserve_exact` + typed error,
  matching `read_bounded`.
- **Regression test:** extend the existing interface-shape test to assert
  `try_reserve` appears in publication.rs (or a direct unit test on the
  error type).

### [P3-5] The state authority's tuple table is stale: it pins gwz-core at `da58135` while the same document records the c436180 work as complete and the workspace lock pins `c436180`

- **Where:** `dev-docs/CurrentProgramCheckpoint.md` lines 11-17 (table row
  "gwz-core | `da58135`") versus its own "Next ordered actions" item 2
  ("**Complete 2026-08-15**": 23-key family, inventory 163, git-target
  matrix, digest update — all content of `c436180`), and the actual lock
  (`gwz status --json`: gwz-core `c436180`, Matches).
- **Violated invariant:** the document's own charter ("the single
  current-state authority … supersedes live status paragraphs elsewhere",
  rulebook §7.3). An agent or reviewer resolving the tuple from the
  authority table lands on the wrong SHA.
- **Reproduction:** read the file at root `d2b4c77`; compare with
  `gwz status --json`.
- **Impact:** wrong-tuple review or implementation resumption risk; this
  review proceeded on the prompt's explicit tuple, which matches the lock.
- **Required correction:** refresh the tuple row (and note `d2b4c77` as the
  docs-checkpoint root) in the next docs commit.
- **Regression test:** none (docs); the planned doc-gate anchor checking
  could pin tuple-table/lock agreement later.

## Invariant and interface verification

Verified properties on this tuple (methods: full-file reads, caller
enumeration, independent recomputation, gate execution, disposable-export
compile/behavior probes):

- **Sealed interface completeness (files):** for every regular-file edge
  the primitive re-establishes identity and full bytes through the retained
  post-open capability; length over/under-run is caught by the `len+1`
  read; kind swap is caught by identity; destination pre-occupation by
  NOREPLACE (dedicated tests for all three at the strictest committed
  hook). For directories, identity only — Finding P2-1.
- **Six-edge routing:** exactly six `publish_verified_no_replace` call
  sites covering §4.1's four families; enforced twice (interface test
  six-count; script per-file counts 1+5 with caller-set equality over the
  provider tree). A seventh call in a new provider file is caught by the
  caller-set equality; a raw rename in a new provider file is not —
  Finding P3-1.
- **Owner sealing (round-1/2 closures persist):** `CatalogOwnerEdgeV1` kind
  and constructors private to `catalog/bootstrap.rs`; edges consumed by
  value in `require_*`; every `execute_owner_*` calls
  `revalidate_observation()` first; `recover_or_create`/owner types are
  `pub(in crate::checked_artifact)` with **no production caller anywhere in
  the crate** — the catalog bootstrap is not production-activated.
- **Activation/hook gating:** the fault vocabulary enum is compiled in
  production but inert; `hit`, `run_next_at`, both thread-local slots, and
  every one of the 23 `catalog_bootstrap.*` injection sites are
  `#[cfg(test)]`; `record_for_test`, `revalidate_for_test`,
  `root_kind_for_test` are `#[cfg(test)]`. Nothing on this seam is
  production-reachable.
- **Fault-key vocabulary 161→163 / family 21→23 compatibility:** consumers
  are exactly three, all in-crate and consistent — the enum, the
  independent fixture (163 entries, set+count+duplicate equality both
  directions), and the two matrices. No stable key is serialized, no
  durable/wire surface, and `grep` across gwz-cli, gwz-py, and taut finds
  zero references.
- **Matrix semantics (L2-14):** both matrices install a panic at each key,
  **assert the fault point was reached**, then reacquire a fresh lease and
  prove convergence plus retained revalidation. Panic-unwind is equivalent
  to a process stop for durable state on this seam: the only relevant
  `Drop` impls (`AdvisoryLock`, `CatalogLeaseSetV1`→`release_reverse`)
  unlock without unlinking, matching crash semantics of flock.
- **Protected-tree digest change at c436180:** recomputed independently at
  three commits with a byte-identical algorithm; the two refreshed pins
  match the c436180 tree exactly; the only protected-tree file changes
  since the last green pin are the three intended files, each read in full
  (Finding P3-3 covers the one-commit staleness).
- **Windows typing:** `FILE_RENAME_INFO_0.ReplaceIfExists` is `bool` in the
  locked windows-sys 0.61.2 (registry source verified), so the assignment
  compiles; a shim crate containing the verbatim publication region of
  platform.rs (lines 1-205, model types stubbed) type-checks clean for
  `x86_64-pc-windows-msvc` against cap-std 4 + windows-sys 0.61.
  `GetFinalPathNameByHandleW` is called with growth, bounds (32,768
  units), and fallible allocation.
- **Compiler enforcement summary:** the primitive is compiler-sealed
  outside the provider (`pub(super)`); owner edges are compiler-sealed
  outside bootstrap.rs; fault hooks are compiler-excluded from production.
  Raw renames inside `checked_artifact` are **not** compiler-sealed
  (pub(super) at subsystem scope, lint allowed at crate root) — only the
  textual gates cover them, incompletely (Finding P3-1).

## Commands and exact results

All commands ran on the pristine tuple unless a disposable export is stated.

```text
git rev-parse HEAD; git -C {gwz-core,gwz-cli,gwz-py,taut} rev-parse HEAD; all status --porcelain
  PASS: exact requested tuple, all clean (start and end)
/Users/owebeeone/.cargo/bin/gwz status --json
  PASS: 7 members, all lock_match=Matches, all clean; gwz-core c436180

git -C gwz-core log --oneline a02d7d6..c436180      13 commits
git -C gwz-core diff --stat a02d7d6..c436180        20 files, +1023/-203
git diff 95d292f..c436180 -- scripts/checks/...     exactly 2 digest lines changed
git diff --name-only {da58135,95d292f}..c436180 -- <protected trees>
  mutation.rs, mutation_tests.rs, catalog/bootstrap/tests.rs only

python3.13 scripts/checks/check_checked_artifact_boundaries.py
  PASS: checked-artifact boundary: ok (15 visible entries, 5 classified modules)
python3.13 -m unittest scripts.checks.test_check_checked_artifact_boundaries
  PASS: Ran 61 tests in 374.932s — OK
cargo test --lib checked_artifact::            (gwz-core, workspace target)
  PASS: 241 passed; 0 failed (includes both fault matrices and all strict-window tests)

Independent digest recomputation (scratchpad, byte-identical algorithm, git-archive exports):
  da58135  pre_catalog pin 325f3804 == tree            PASS
  95d292f  pre_catalog tree 13249ed9 != pin 325f3804   GATE WAS RED AT THIS COMMIT (P3-3)
  95d292f  catalog pin == tree                          PASS
  c436180  both refreshed pins == tree                  PASS (all six pins verified)

Disposable exact-SHA export probe A (sliver interior drift):
  cargo test --lib probe_interior_drift_after_final_interior_recheck_is_not_published
  FAIL (counterexample): final_exists=true; foreign_in_final="sliver drift\n";
  run=Err(Ambiguous{catalog interior: "catalog directory contains an unowned child"})

Disposable export probe B (raw rename in provider/retained.rs + self-service digest refresh):
  python3.13 scripts/checks/check_checked_artifact_boundaries.py
  UNDETECTED: "checked-artifact boundary: ok", exit 0  (P3-1)

cargo check -p gwz-core --lib --target x86_64-pc-windows-msvc
  NOT A PRODUCT RESULT: host cross-C build fails in libz-sys (stdio.h not found),
  same as round 2; no full-crate Windows compile evidence obtainable on this host
Shim crate (verbatim platform.rs:1-205 publication region, model stubbed), cap-std 4 + windows-sys 0.61:
  cargo check --target x86_64-pc-windows-msvc — PASS
```

## Residual risks and unexecuted evidence

- **No full-crate Windows or native-Linux compile/run evidence exists for
  any commit after `a02d7d6`.** The three Windows commits' messages imply
  out-of-band Windows-runner iteration, but no run ID or artifact is
  recorded; the push CI workflows never build the crate for Windows
  (retained-readers' path filter excludes `src/checked_artifact/**`; the
  boundary workflow is ubuntu-only; release.yml is dispatch-only). My shim
  covers the new publication region's types only — not the pre-existing
  Windows anchor machinery, the cfg(windows) durable-write options, or any
  runtime behavior. Native execution is scheduled at R2-F; the compile gap
  could be closed cheaply by one dispatch run and should be recorded
  before R2-F.
- The `cfg(all(test, windows))` object-binding test and the Windows
  destination-swap behavior (P3-2) are unexecuted on this host.
- The matrices run in-process (panic-unwind, verified Drop-safe) on macOS
  workspace and git-directory targets; real process-kill and power-loss
  remain at R2-F per §4.1/§8.
- The reserved fault families (140 keys) remain injection-free by design
  (RemPlan §10); each future conversion package must bring its own sites
  and rows.
- Scope bleed: `f532b1a` bundled test-only `workspace_ops/merge`
  rollback-identity refactors into the publication commit — read and found
  harmless, but the closed R4b lane's files were touched without a
  checkpoint note.
- `FaultInstanceV1` and the reserved keys are compiled-in inert vocabulary;
  no consumer risk found, none executed.

## Final tuple recheck and next action

Rechecked after writing this report: root `d2b4c77` clean; `gwz-core
c436180`, `gwz-cli 3cca145`, `gwz-py 929efb0`, `taut f008419`, all clean;
scratchpad-only probe writes; no workspace file, lock, or marker modified.

**Next action:** remediate P2-1 (interior binding inside the acquisition
window, or an explicit reviewed §4.1 erratum) and P3-1 (subsystem-wide
raw-rename caller inventory in the checker — pulled into this gate), fold
P3-2's comment/design correction into the same erratum, then re-run both
matrices and the full gate set on a settled commit with same-commit digest
refresh (P3-3 discipline) and launch the round-4 re-review. The P3-4
one-liner and P3-5 docs refresh can ride the same package.
