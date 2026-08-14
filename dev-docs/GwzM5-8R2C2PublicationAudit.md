# R2-C2 sealed publication primitive — §4.1 conformance audit

Date: 2026-08-15
Auditor: Claude (F5), implementation-lane owner. Method: first-hand read of
the primitive and platform layer; delegated breadth sweep (edge routing,
sealing, provenance, fault coverage, substitution tests) by an independent
agent; static only — no test execution, no mutations. This is the
`CurrentProgramCheckpoint.md` next-action-1 deliverable and an input to the
R2-C2 settled re-reviews; it is not itself an acceptance review.

## Verdict

**The implementation satisfies amendment §4.1 as written.** No P0/P1/P2
findings. Four P3 items, all conversion-debt or coverage-scope, none a
defect in the primitive.

Verified, with the load-bearing citations:

- One sealed primitive (`provider/publication.rs:48`
  `publish_verified_no_replace`): no-follow open through a retained parent
  Dir, opened-identity compare, byte compare for regular files, rename of
  the retained open handle, `replace=false` hardcoded (`:90`).
- All six §4.1 edges route through it — scratch→active
  (`mutation.rs:213`), staging→final (`directory_mutation.rs:232`),
  active→retired (`:318`), three anchor moves (`:574/:594/:609`) — and the
  count is enforced twice: `interface_tests/capability_permit.rs:110`
  asserts exactly six call sites, and the boundary script's seam check pins
  caller counts per file (`check_checked_artifact_boundaries.py:631-634`,
  run green during this audit).
- Windows renames the exact identity-checked handle
  (`SetFileInformationByHandle`, `platform.rs:99-140`), no-follow +
  reparse-point open with `DELETE` access (`:49-76`); path substitution
  after open cannot retarget the source. The exact-handle association test
  exists in source (`platform.rs:569-614`, `cfg(all(test, windows))`).
- Unix: `renameat_with(NOREPLACE)` relative to retained parents for
  linux/macos (`:223-246`); other Unix targets and unknown platforms fail
  closed on no-replace (`:249-268`, `:292-307`). No compare-and-rename
  claim anywhere, matching §4.1's explicit trust boundary.
- Provenance: every call site's Dir capabilities trace to the
  permit-retained platform root (`provider/retained.rs:185-258`,
  `pre_catalog.rs:83`) directly or through one identity-proved no-follow
  hop (`open_observed_directory`); `revalidate_observation()` runs
  immediately before every owner edge. No ambient-path Dir reaches the
  primitive.
- Fault coverage: all six rename edges have a pre-edge injection point in
  the strictest window and before/after power-loss keys; the 21-key
  `catalog_bootstrap.*` family is executed as an
  interruption+restart+convergence matrix with set-equality against the
  declared family (`catalog/bootstrap/tests.rs:326-383`), not an inventory.
- Substitution tests cover object, identity, bytes/interior, kind, name and
  case-fold aliases, destination pre-occupation, target, and lock slot,
  including left-untouched assertions for foreign facts.

## Findings

- **[P3-1] Declared-vs-executed fault vocabulary gap.** 140 of 161
  `fault_v1.rs` keys have no injection sites — the whole `namespace.*`
  (including `pre_publish_reobserve`/`publish_no_replace`), `record.*`,
  `admission.*`, `barrier.*`, `managed_bootstrap.*`, `cleanup.*`,
  `terminal.*`, `durable_leaf.*`, `runtime.*` families are reserved for
  edges not yet converted (R2-D+). The catalog edges use
  `catalog_bootstrap.*` instead. Additionally the §6 Git private-parent
  creation edge has no fault key (`mutation.rs:66-87`), and the executed
  matrix runs workspace targets only, in-process, host-platform only.
  §8.9's "every declared fault edge" reads wider than what executes.
  Disposition: rescope note now (declare the reserved families explicitly
  reserved-for-R2-D in the amendment errata or RemPlan), key the
  parent-creation edge or record why not, extend the matrix to
  Git-directory targets; native Linux/Windows runs stay at the R2-F gate
  where §5/§8 already put them.
- **[P3-2] Sealing perimeter has an unpinned interior.** The raw pieces
  (`open_rename_source`/`rename_open_source`/`rename_relative`) are
  `pub(super)` at `checked_artifact` scope; the structural test and seam
  check scan only the provider tree, and digests pin only `platform.rs`,
  `mod.rs`, and the pre_catalog/catalog trees. A new raw-rename caller in
  the legacy interior (transition.rs, residue.rs, namespace/, coordinator/,
  cleanup.rs) would pass every gate. Disposition: extend the boundary
  script's caller inventory to the whole subsystem (bounded gate change,
  own package, dual review per L2-06).
- **[P3-3] Legacy/catalog namespace coexistence.** The legacy leaf writers'
  private root is physically the catalog Final directory
  (`policy.rs:34-42` → `.gwz/checked-artifacts`); four legacy rename edges
  (`transition.rs:276/:369`, `residue.rs:380/:455`) and the Windows
  durability-anchor barrier (`platform.rs:332-440`, verify-by-path then
  unassociated rename) mutate inside it without the sealed primitive. Not
  co-reachable with a live catalog today (bootstrap not
  production-activated); becomes interior ambiguity for the ten-slot
  grammar the moment a catalog completes. Disposition: R2-D conversion
  scope; record as an explicit R2-D acceptance criterion.
- **[P3-4] Three strict-window substitution scenarios untested** (mechanism
  present, scenario absent): (a) in-place byte drift with unchanged
  identity at `PublishBeforeRename` — the primitive's byte-compare branch
  (`publication.rs:71-85`) has no direct test; (b) destination appearing
  inside the window (no-replace EEXIST path) has no dedicated test; (c)
  kind swap at the window covered only implicitly. Disposition: three
  focused tests; small, test-only; next coding task on this lane.

## Remainder before resume-order item 2 is complete

1. Execute the existing 21-key matrix green on this tree (it was not run in
   this static audit).
2. Add the three P3-4 tests; extend the matrix to Git-directory targets;
   key or rescope the parent-creation edge.
3. File the reserved-families rescope note (P3-1) so §8.9's claim matches
   reality.
4. Then commit the settled tree and launch the two R2-C2 settled-tree
   re-reviews (dual — mandated tier; cross-model where available). P3-2 and
   P3-3 are tracked as separate bounded packages, not blockers for the
   re-reviews, subject to reviewer concurrence.
