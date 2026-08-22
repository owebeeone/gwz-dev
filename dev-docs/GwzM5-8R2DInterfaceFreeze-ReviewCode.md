# R2-D Phase 0 interface freeze — CODE-axis review (adversarial, round 1)

Date: 2026-08-22
Axis: CODE (adversarial; round 1 of the two-round cap, `GwzProcessOptimization.md` §4.1)
Reviewer basis: peer-blind (the State-axis report was not read or searched for).

Object under review:

- gwz-core parked commit `d32b2c9cb67a0d25ddb2dd047e1c5d1a70d0c39b`
  ("Park the R2-D Phase 0 freeze package pre-review (pause point)"),
  diff `90d3f8a..d32b2c9`: 10 files, +767/−29.
- `dev-docs/GwzM5-8R2DInterfaceFreeze.md` (DRAFT, 692 lines).
- The dated append-only block at the end of §10 in
  `dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` (:1039-1068).
- Workspace root HEAD at review time: `a59c5d0` ("Record the pause checkpoint").

Tree caution honored: during this review the gwz-core working tree gained an
uncommitted, non-compiling modification to
`src/workspace_ops/merge/v1_lifecycle/tests/forward.rs` (+132/−25, another
lane's draft, outside the object). All file evidence below was read from the
committed object (`git show d32b2c9:<path>`), and all gates were re-executed
against a pristine extraction of the d32b2c9 tree (`git archive` into the
session scratchpad, assembled with the workspace root manifests and the
`taut-shape-rs` patch target) where the dirty tree could otherwise contaminate
the result. No repository file was modified; this report is the review's only
write.

## 0. Verdict

**NO-GO (round 1).** Findings: **0 × P0, 2 × P1, 0 × P2, 8 × P3.**

Both P1s have small, well-bounded remediations (one is dischargeable by a
recorded operator decision alone). Everything else in the package — the trait
delta, the admission skeleton, the six seam pins, the Track-P spike mechanics,
the checker interaction, the §7 drift corrections, and every gate the memo
claims — verified correct, most of it to the exact line.

## 1. Gates re-executed

| Gate | Memo §10 claim | Re-run result (committed object) |
| --- | --- | --- |
| `cargo test --lib checked_artifact::` | 256 passed, 0 failed, 1115 filtered | **256 passed, 0 failed, 1115 filtered** (10.45 s) — exact match |
| Spike standalone (`...provider::tests_admission_spike`) | 2 passed | **2 passed** (0.02 s) |
| `cargo fmt --all -- --check` | clean | **clean** on the extracted d32b2c9 workspace copy (`OBJECT-FMT-CLEAN`); the in-repo run flags only the out-of-object dirty `forward.rs` |
| `cargo clippy --all-targets --all-features -- -D warnings` | Finished, exit 0 | **exit 0** on the extracted d32b2c9 workspace copy; the in-repo run fails only inside the dirty `forward.rs` (3 × E0716) |
| `python3.13 scripts/checks/check_checked_artifact_boundaries.py` | two expected digest findings pre-refresh; refreshed in-commit | **GREEN at d32b2c9**: "checked-artifact boundary: ok (15 visible entries, 5 classified modules)", exit 0. The commit's checker delta is exactly the two digest refreshes the memo names (`checked_artifact/mod.rs` protected digest; `checked_artifact/capability/pre_catalog.rs` tree digest) and nothing else |

The in-repo checker failure ("protected source tree changed:
workspace_ops/merge/v1_lifecycle/mod.rs") is caused solely by the uncommitted
`forward.rs` inside that digested subtree — attributable to the other lane,
not to this object.

## 2. Findings

### [P1-1] The mandated per-platform Track-P spike was not executed on Windows, but the trait delta was implemented anyway, and the memo claims the condition discharged

Controlling text, in escalating specificity:

- `GwzProcessOptimization.md` §3.1 (:76-80): Track P requires "a mandatory
  implementation spike on each target platform *before* the contract freezes,
  proving the admitted primitive exists and behaves."
- `GwzFasterProposal.md` §2 (:65-72): "Before `GwzM5-8R2DInterfaceFreeze.md`
  goes to dual review, spike the admission publish/retire path on macOS and
  Windows against the already-sealed `publish_verified_no_replace` family. …
  Policy is already in force; apply it to this freeze." §3 Step C.1 (:139-143)
  repeats it as the first ordered step.
- The memo's own §1 (:35-36) concedes the reading: the operator instruction
  "requires Track-P evidence per platform *before* this freeze is reviewed."

What the package did:

- Executed the two spike cases on macOS only. Memo :401-402: "**Windows arm
  status:** these two cases have not been executed on Windows by this
  package — no native Windows runner is available in this lane…".
- Implemented the trait delta (defaults → required) rather than
  paper-freezing it (`namespace/backend.rs`, §3.2).
- Asserted the operator condition satisfied. Memo :62-67: "…forbids freezing
  the four managed defaults into required methods 'until each new physical
  edge names an admitted primitive per platform'. **§4 discharges that
  condition**…".

Why that is a defect and not just a judgment call: the quoted discharge
addresses only the *naming* leg (§4.3 does name an admitted primitive per
edge per platform — verified, see §3.3 below). §3.1's policy and Step C.1's
instruction have a second leg — *execute* the spike on macOS **and Windows**
before the freeze goes to review — and that leg is unmet. The memo discloses
the gap honestly and offers substitute evidence (run-13 Windows matrix,
1322/0/1 GREEN at `90d3f8a`, which does exercise `publish_verified_no_replace`'s
Windows arms across the six production moves including a directory
staging→final publish and a cross-parent retirement — I verified that
enumeration against amendment §4.1 :569-575 and
`CATALOG_PUBLICATION_CALL_COUNTS` 1+5). But the substitution is nowhere
surfaced as a contingency for this review to confirm — §2 lists only C-1 and
C-2 — so accepting it silently would have the review, not the operator,
waiving an explicit operator precondition. The standing rule the memo leans on
(amendment :667, "Linux and Windows native fault evidence remains an R2-F
release gate"; plan §5 item 4) predates the operator's freeze-specific
instruction, which says it applies "to this freeze" on top of policy already
in force.

Mitigating facts, recorded for the disposition: the spike's two cases add no
platform-sensitive code beyond what run 13 already executes on Windows (their
deltas — indexed naming, `DestinationRecheckV1::None`, cross-parent file
retirement — are caller-level parameters of the same sealed function), and the
package will run them on Windows automatically at the next matrix run.

Minimal remediation, any one of:

1. **Operator disposition recorded** (one dated line in the memo §4.2 and/or
   `CurrentProgramCheckpoint.md`): run-13 accepted as the Windows arm of the
   Step C.1 spike for E3/E7, with the two spike cases confirmed on the next
   matrix run. This is the cheapest path and fully discharges the finding.
2. Execute `cargo test --lib checked_artifact::capability::pre_catalog::provider::tests_admission_spike`
   on any Windows host at d32b2c9 and append the result to memo §4.2/§10.
3. Failing both, the memo's own §2 contingency applies as written: revert the
   trait delta to paper-freeze (the two-file revert the memo pre-arranged) and
   re-spike in Phase 1.

### [P1-2] The frozen record of Seam 5's injection-site inventory is false on this tree, and the D3 pin does not deliver the guarantee the package records in three places

The claim, made three times:

- Memo :364-367: "The only `CheckedArtifactFaultKeyV1` injection sites on the
  tree today are `catalog_bootstrap.*` in
  `capability/pre_catalog/provider/mutation.rs` and
  `capability/pre_catalog/provider/directory_mutation.rs`; every reserved
  family has zero, by design, which the fixture now asserts (§8, D3)."
- `interface_tests/fault_expected_keys.rs` :244-247 (doc on
  `FAULT_INJECTION_SOURCES`, :248): "The complete set of production sources
  that hold `CheckedArtifactFaultKeyV1` injection sites today."
- RemPlan §10 annotation :1063-1068: "The map is now machine-checked: …
  fails if a family changes size, **gains an injection site while still
  declared reserved**, or changes activation state without the converting
  package's deliberate edit." Memo :605-607 makes the same claim of test
  `:320`.

The contradicting evidence:
`capability/pre_catalog/provider/aggregate.rs:26-29` (committed at d32b2c9,
unchanged by this package) is a third production source holding an injection
site, in the identical pattern as the declared two:

```rust
#[cfg(test)]
crate::checked_artifact::fault_v1::hit(
    crate::checked_artifact::fault_v1::CheckedArtifactFaultKeyV1::CatalogBootstrapCatalogEnumerate,
);
```

Full-crate sweep at d32b2c9 (`git grep 'CheckedArtifactFaultKeyV1::'`,
production files): exactly three files hold variant-qualified references —
aggregate.rs (1), directory_mutation.rs (15), mutation.rs (9); all
`CatalogBootstrap*`. So the *design* claim is intact — reserved families do
have zero sites anywhere — but the recorded inventory is wrong, and the
mechanism is porous: `reserved_fault_families_have_no_injection_sites_before_their_package`
(:320) scans only the two `include_str!` sources in `FAULT_INJECTION_SOURCES`.
A Phase-1+ injection site for a reserved family added in aggregate.rs — a
file that demonstrably *is* where injection sites live — or in any other
production file would not trip the pin, its activation row could stay
`Reserved`, and the "machine-checked" sentence in the frozen RemPlan
annotation would be false in exactly the way that matters. Neither the
boundary checker (no fault-key rule; verified by inspection of
`scripts/checks/check_checked_artifact_boundaries.py`) nor any other test
anchors the inventory's completeness.

Because Seam 5 is one of the five frozen seams (§2 item 5) and this is both a
false factual statement in the freeze record and an undelivered enforcement
property of the Step 0.2 scaffolding, it is P1, not P3.

Minimal remediation:

1. Add the aggregate.rs row to `FAULT_INJECTION_SOURCES` (two lines) and
   correct its doc comment.
2. Correct memo :364-367 ("three production sources"), and append a dated
   correction line to the RemPlan §10 annotation (append-only discipline
   maintained) qualifying "machine-checked" to the declared source set — or,
   better, add a completeness anchor so the qualification is unnecessary:
   e.g. a checker rule or fixture test pinning the set of production files
   that contain `CheckedArtifactFaultKeyV1::` to the declared inventory
   (the same closed-inventory pattern pin 2 of `r2d_seam_freeze.rs` already
   uses for backend impls).

### P3 findings (informational; file-and-continue)

- **[P3-1]** Memo :580 says the backend.rs delta is "net -1 line". Actual:
  `git diff --numstat 90d3f8a..d32b2c9` gives +19/−27 = **net −8**.
- **[P3-2]** Memo :589 says `r2d_seam_freeze.rs` is "239 lines" (actual
  **240**); memo :610 says `tests_admission_spike.rs` is "192 lines" (actual
  **191**).
- **[P3-3]** Memo :692 says the full `--lib` run partitions "all 1 367
  tests". Not reproducible: the d32b2c9 lib totals **1371** (256+1115 and
  2+1369 in my runs), and the pause checkpoint itself records 1370/1371.
  The gate-bearing numbers (256/245, 2, fmt, clippy, checker) all reproduce;
  only this aside is off.
- **[P3-4]** Spike doc comment cites "RemPlan-4 §4 R2 stop clause
  (:1082-1085)". In `GwzM5-8R4bP1P2-RemPlan-4.md` as of the parked commit the
  clause sits at **:1089** (shifted by the thin-A1 supersession banners,
  commit `6a458aa`). The citation was inherited verbatim from
  `GwzFasterProposal.md` :160 and was already stale at commit time. Content
  is accurate (the clause does forbid fresh retry scratch names, matching the
  spike's no-nonce discipline).
- **[P3-5]** Memo :356-357 cites "the separate `RuntimeBootstrapFault` enum
  (`bootstrap/runtime/fault.rs:19`)". The enum is at **:4-11**; :19 is
  `run_next_at`, the injection entry point. The companion test cites
  (:21/:37/:73) are exact.
- **[P3-6]** Memo :379 (§4.1 P5 row) attributes "catalog root-flush callers
  at `mutation.rs:396/:412/:426` and `directory_mutation.rs:729`" to the
  dirent-barrier family. Those four sites call
  `platform::sync_parent` — P2's mechanism (:395-398/:411-414/:425-428 and
  directory_mutation :727-731 all call `sync_parent`). `private_barrier`'s
  actual production callers today are the legacy `cleanup.rs:89/:129/:159`,
  `residue.rs:407/:500/:529`, `transition.rs:427`, plus the Windows anchor
  internals (`platform.rs:410`). The Windows anchor cell of the same row is
  exact to the line; the per-edge table (§4.3) is unaffected because
  E10/E14/E22's P5 mechanisms exist per platform as cited.
- **[P3-7]** Memo §3.2's table mixes line-number baselines: the
  `RecordingNamespaceBackend` row (:219) cites 90d3f8a positions
  (:412-554, ops :471/:488/:508/:525 — all verified exact **pre**-package;
  post-package they are :465, :524/:541/:561/:578), while the adjacent
  blanket-adapter "After" cell cites post-package positions (:243-248,
  verified exact **post**). Defensible under the header's declared
  convention (verified at 90d3f8a), but a future reader diffing against
  d32b2c9 will trip on it; one baseline per table would be kinder.
- **[P3-8]** Environment, not the object: the gwz-core working tree currently
  carries the other lane's uncommitted, non-compiling
  `workspace_ops/merge/v1_lifecycle/tests/forward.rs` (breaks tree-level
  fmt/clippy/checker/`cargo test --lib`). Whoever runs the GO/GO push train
  should do so on a clean checkout of d32b2c9, or that train will fail for
  reasons unrelated to this package.

## 3. Mandate dispositions (what was checked and found correct)

### 3.1 Trait delta (Seam 2) — VERIFIED

- The diff touches exactly the four managed operations and the sentinel
  helper; the rest of `RawNamespaceBackend` (:375-457 at d32b2c9) is
  hunk-for-hunk untouched and matches the memo §3.2 transcription verbatim
  (associated types, `provider_binding`, `revalidate_action_directory`,
  `publish_no_replace`, `retire_exact`, `barrier`, then
  `SealedActionNamespace` at :459).
- Complete impl inventory at d32b2c9 (crate grep): exactly three —
  `provider_compile.rs:153` (`ProductionShapedBackend`, ops at
  :204/:219/:239/:254, pre-existing), `test_support.rs:465`
  (`RecordingNamespaceBackend`, ops at :524/:541/:561/:578, pre-existing),
  and the blanket legacy adapter `test_support.rs:137`, which this package
  extended with the four ops (:185/:199/:206/:220).
- Behavior byte-identical: the moved helper (`test_support.rs:243-248`)
  reproduces the deleted `backend.rs` helper exactly —
  `CheckedFsError::ambiguous("managed namespace operation", "provider does
  not implement exact post-observation")` — same variant, fact, and detail
  strings, compared character-for-character across the two diff hunks.
- Genuinely test-only: `namespace/mod.rs:13-14` is
  `#[cfg(test)] pub(super) mod test_support;`, so non-test builds contain
  neither the adapter nor the refusal; with the defaults gone, the compiler
  itself now forces every production impl to state the four ops. The
  ConsumerCheckpoint §8 :230-231 sentence is discharged structurally, as
  claimed. The only implementor of the legacy protocol is
  `contracts.rs:236` (`RecordingNamespace`), as the memo says; the
  production `NamespaceProtocol` (namespace/mod.rs:304-305,
  `cfg(not(test))`) and the :348-349 test re-export check out.
- Pin robustness: pin 1's four assertion groups (exact count 2 of each
  `;`-terminated declaration string, absence of both `{`-terminated
  defaulted forms, absence of the sentinel token, presence of all four fn
  names) mean a reintroduced default under the enforced rustfmt style must
  flip at least one of them — a body flips the `{` check, a where-clause or
  reshaping flips the count-2 check, a move flips the trait-presence check.
  Pin 2's per-file counts (1/2 impls; each op ×1/×2) verified against the
  tree. Pin 2 cannot see a brand-new impl in a *new* file, but with the
  methods required that case is compiler-enforced, so no gap remains.

### 3.2 Admission skeleton (Seam 1) — VERIFIED

- `admission/mod.rs` compiles to exactly the memo §3.1 frozen signature
  (struct :28-30, `const fn from_retained_catalog`, `resume_or_admit`
  taking `&ActionCapacityReservationV1` and returning
  `Result<AdmittedActionV1, CheckedFsError>`; refusal helper :59-64 in the
  house idiom). Exactly three `pub(in crate::checked_artifact)` items, as
  pin 3 counts.
- Consumes `OpaqueRetainedCatalogV1` only; that type's only construction
  site is `catalog/bootstrap.rs:210-214` inside the sealed owner (private
  field `permit`, struct :128-130), sole production entry
  `recover_or_create` :145-149 via `begin_preflight` :155 — all verified.
  `AdmittedActionV1` (:281-294 of `protocol/admission.rs`) is accessor-only
  over `{reservation, directory_identity}`; the R1 classifier
  (`protocol/admission/owner.rs` :82/:110) keys on the same expectation
  type. No filesystem logic, no `cap_std`, no paths, no handles in the
  module; declared at `checked_artifact/mod.rs:8-12` with the dead-code
  reason. Checker module rules pass over it unchanged (checker GREEN, "5
  classified modules"). The module matches the amendment §7 owner list
  (:575) and its doc mirrors ConsumerCheckpoint §7 step 9 verbatim.

### 3.3 Track-P spike — MECHANICALLY VERIFIED (execution scope is P1-1)

- End-to-end and genuine: test 1 stages via write+`sync_all`, publishes
  no-replace through the real sealed 7-argument
  `publish_verified_no_replace` (:82-183; identity compare :98-105 plus
  full byte verification :107-128 before the retained-handle rename
  :174-182), reobserves through the same retained parent (staging name
  consumed; identity and bytes equal), proves the no-replace collision
  (second publish rejects; destination object identity unchanged), then
  retires cross-parent with the same reobservation. Test 2 proves
  identity-compare (not byte-compare) rejection: same name, same bytes,
  different object → rejects pre-edge with zero namespace mutation, exactly
  the amendment §4.1 drift-rejection paragraph.
- No shortcut and no flake exposure: `encode_identity` (retained.rs:417-427)
  encodes the durable identity — ext4 UUID + persistent handle (generation-
  bearing) on Linux (linux.rs:90-102, `require_ext4`), `ATTR_CMN_OBJPERMANENTID`
  + volume UUID on macOS, 128-bit `FILE_ID_INFO` on Windows — plus dev+ino,
  so unlink/recreate inode reuse cannot alias identities on any admitted
  platform. The fixture pattern (env::temp_dir + PID + counter) is identical
  to the four pre-existing provider test files, so no new environmental
  assumption.
- Test-only pin sound and the naming deviation genuinely forced:
  `production_rust_files` (checker :663-670) excludes only paths with a
  `tests` component, an `interface_tests` component, or a `tests`-prefixed
  file name; `publication_callers` (:817-824) sweeps everything else under
  `provider/` for the primitive name, and the seam rule (:834-848) fails on
  any set drift from `CATALOG_PUBLICATION_CALL_COUNTS` (:648-651,
  mutation.rs=1 + directory_mutation.rs=5). A sibling-style
  `admission_spike_tests.rs` would therefore join the caller set and fail
  the six-move rule exactly as memo §4.2 and the spike's own header claim.
  Pin 6 additionally locks the `#[cfg(test)]` module edge (provider.rs
  :199-200) and bans the three raw-rename tokens in the spike.

### 3.4 Scaffolding — VERIFIED EXCEPT AS IN P1-2

- All 6 pins in `r2d_seam_freeze.rs` pass at d32b2c9 and each breaks on its
  claimed violation (pins 1-2 §3.1 above; pin 3's nine forbidden tokens are
  absent and its exposure count is exact; pins 4-5's required/forbidden
  strings verified present/absent in `leaf.rs` and
  `bootstrap/managed/owner.rs`; pin 6 §3.3 above).
- `fault_expected_keys.rs`: `EXPECTED_KEY_COUNT = 165` at :172 (list ends
  :171), enum-anchored by the pre-existing
  `fault_vocabulary_exactly_matches_the_independent_stable_key_fixture`
  (:412+, over `CheckedArtifactFaultKeyV1::all()`); the activation map's
  ten rows (18+25+19+11+11+13+30+11+16+11) sum to 165 and match the memo
  §3.5 table and the RemPlan annotation exactly; the three new tests sit at
  :280/:320/:349 as cited; the runtime carve-out (:335-343) matches the
  separate `RuntimeBootstrapFault` mechanism (injection entry
  `run_next_at` fault.rs:19; test cites :21/:37/:73 exact). The 165/25
  claims are therefore true; the two-file injection-source inventory is not
  (P1-2).
- `cargo test --lib checked_artifact::` = 256 = 245 + 6 + 3 + 2, exactly as
  the memo decomposes it.

### 3.5 Memo accuracy — VERIFIED except the P1/P3 items above

Spot-verified exact (committed object): `publish_verified_no_replace` at
publication.rs:82 (fn spans :82-183); `LeafObserver` has **zero**
implementations (crate grep: trait decl leaf.rs:74 plus the pin's string
literal only — the §7 material correction is true, and `contracts.rs:183/:236`
are indeed `ExpectedLeafContent for Bytes` / `NamespaceProtocol for
RecordingNamespace`); Windows anchor block :379-604 with `ANCHOR_BYTES` :380,
`ANCHOR_PREFIX` :383, `prepare_private` :386, random scratch :420,
`private_barrier` :458-459, `anchor_state` :517, `anchor_roundtrip_name`
:575, `verify_anchor` :582 (all exact); `rename_relative` arms :276/:324;
legacy edges transition.rs:275/:368 and residue.rs:393/:486; six-site pin
capability_permit.rs:110; `MAX_INTERIOR_ENTRIES = 10` interior.rs:27 and
`interior::observe` :50; enumeration budgets :8-10; schema gate
durable_path_schema.rs:14; `sync_parent` :367-370/:372-377;
`open_rename_source` :23-46/:48-76; `rename_open_source` :78-96/:98-157;
`open_dir_share_delete` :159-163/:165-194; `windows_destination_path`
:196-259. The §7 drift table's "plan said" column quotes the plan faithfully
(plan §2.4/§3 really did say 164 keys/:171, 24 keys, 244 passed,
publication.rs:48, contracts.rs:183/:236; plan §9 pre-declared the
platform.rs drift). §5's six decisions transcribe plan §9 verbatim. The D4
annotation is 30 inserted lines, zero deletions, at the end of §10 —
append-only as claimed. The tier table matches `GwzFasterProposal.md` §2 as
quoted in the checkpoint. Run-13 (1322/0/1, GREEN at 90d3f8a) is in the
checkpoint ledger as cited.

### 3.6 C-2 contingency — CONFIRMED code-true

`DirectoryInteriorRecheckV1` (publication.rs:34-37) types its `expected` to
`&CatalogBootstrapRecordV1`; the only destination arm is
`DestinationRecheckV1::PreRetirementFinal` (:41-47), enforced at :157-173 via
`interior::completed_record` + `InfrastructureSlotV1::CatalogBootstrapRetired`;
the directory source arm re-verifies through `interior::staging_plan ==
Complete` (:145-154). An admission staging interior is a resident
`ActionCapacityReservationV1`, so E3's directory form and E7's destination
recheck do need new admission arms on those two enums, inside
`publication.rs`, exactly as §4.4 describes. Both enums are `pub(super)`,
in-memory, non-serialized — the §6 no-new-durable-records analysis holds.
C-2 is a faithfully described, bounded, in-seam Phase-1 obligation: **accept**.

On C-1: the §4.3 table's contents are correct as far as this review could
drive them — all 22 edges name admitted P1-P5 arms whose code exists per
platform at the cited lines (every spot-check exact; one caller-attribution
imprecision in the §4.1 P5 row filed as P3-6). C-1's *evidence* leg fails
only on the Windows execution question, which is P1-1's subject, not a table
defect.

## 4. Round-2 scope

Re-review needs only: the P1-1 disposition (operator line, Windows run, or
paper-freeze revert), the P1-2 fixture/memo/annotation correction (with or
without a completeness anchor), and optionally the P3 corrections batched
into the same edit. No architectural root cause was found in the frozen
seams themselves; under the two-round cap this object should close in round 2.

---

# Focused re-verdict (round 2)

Date: 2026-08-22
Axis: CODE (round 2 of the two-round cap; focused on this report's round-1
findings only, per the coordinator's instruction — no whole-object re-review).
Peer-blind maintained: the State-axis report was not read; State finding IDs
appearing below are quoted from the revised memo, which is the object.

Object of the re-verdict:

- gwz-core `2ed2435` ("Apply the Phase 0 freeze round-2 remediation (merged,
  all 14 findings)"; code delta vs `d32b2c9`: `tests_admission_spike.rs`
  1+/1−, `fault_expected_keys.rs` +87/−1 — nothing else).
- Workspace root `7ecc1a7` (same-named docs commit; revised memo now 1062
  lines; RemPlan §10 correction block; RemPlan-4 supersession banner).

Method: all reads via `git show 2ed2435:<path>` / `git show 7ecc1a7:<path>`
(the working tree now carries further uncommitted other-lane files, including
red-by-design `checked_artifact/admission/tests.rs` Phase-1 tests). Gates were
re-run on a pristine `git archive` extraction of 2ed2435 in the session
scratchpad. The Windows run was verified first-hand via `gh run view`
(metadata + job log), not taken from the memo.

## Per-finding disposition

**[P1-1] — RESOLVED (by execution, my remediation option 2).**
Verified first-hand: GitHub Actions run **32542469665**, workflow "Windows
matrix", `workflow_dispatch`, branch `probe/track-p-spike`, conclusion
`success`, headSha `e448d7f1c989548e57316da11d76321f0a4a2cf1`. Local commit
`e448d7f` is exactly `d32b2c9` plus a 2+/2− trim of
`.github/workflows/windows-matrix.yml` (the suite step narrowed to the spike
filter) — **no `src/` change**, so the code that executed on Windows is
byte-identical to the reviewed object's spike. The job log shows both named
spike tests `ok` and `test result: ok. 2 passed; 0 failed; … 1332 filtered
out` on a native Windows runner. (Caveat noted, not blocking: that workflow's
suite step uses `|| true` with a FAILED-grep gate, which would not catch a
compile failure — but the log lines themselves are direct evidence the two
tests ran and passed.) The memo now records the two-leg discharge (§2), the
per-arm execution table (§4.2), the §10.3 run record, explicitly **withdraws
run-13 as discharging evidence** (retained as corroboration only), corrects
its round-1 misreading of amendment `:667` (fault evidence ≠ spike execution),
and carries the tracked acceptance item "both spike cases green on the next
full Windows matrix run" in §4.2 and §10.3. No waiver was needed and none was
claimed.

**[P1-2] — RESOLVED.**
- `FAULT_INJECTION_SOURCES` now declares all three sources including
  `capability/pre_catalog/provider/aggregate.rs` (fault_expected_keys.rs
  :266-269 at 2ed2435), with a corrected doc ("three files, all holding
  `catalog_bootstrap.*` sites") that states the pin/anchor split explicitly.
- The new completeness anchor
  `the_declared_injection_sources_are_every_production_source_holding_sites`
  (:418) is sound: it rescans `src/checked_artifact` at runtime with a filter
  that faithfully mirrors the checker's `production_rust_files` (skip dirs
  named `tests`/`interface_tests`; skip `tests`-prefixed and non-`.rs` files),
  collects every file containing `CheckedArtifactFaultKeyV1::`, and asserts
  **set-equality** with the declared list — so it fails on an unregistered
  file gaining sites *and* on a declared file losing them, naming the
  difference in the assert output. Its scope justification is code-true:
  `pub(super) enum CheckedArtifactFaultKeyV1` at `fault_v1.rs:10` under
  `mod fault_v1;` at `checked_artifact/mod.rs:51`, so no injection site can
  exist outside the scanned subtree; and `fault_v1.rs` itself contains zero
  `CheckedArtifactFaultKeyV1::` tokens (macro-generated, `Self::` internally),
  so the anchor passes exactly on the three declared files. Empirically
  confirmed: `cargo test --lib checked_artifact::` at the 2ed2435 extraction
  → **257 passed, 0 failed** (256 + the anchor), and the scoped fmt check on
  both changed files is clean.
- All three claim sites corrected: memo §3.5 now tables the three sources
  with per-file counts (aggregate.rs `:26-29`,
  `CatalogBootstrapCatalogEnumerate`, 1 site) and names the round-1
  porousness; the fixture doc is rewritten; and the RemPlan §10 correction is
  a **+27/−0 append-only** dated block that restates the three-source truth,
  the anchor, and the tree-wide validity of the "machine-checked" sentence,
  plus a DRAFT-status clarification. Append-only discipline maintained.
- Residual (note only, no severity): both the reserved-family scan and the
  anchor key on the literal `CheckedArtifactFaultKeyV1::` token, so a
  deliberate alias import (`use …::CheckedArtifactFaultKeyV1 as K`) could
  still evade both. That is the same token convention the boundary checker
  itself uses, it applies uniformly inside already-declared files, and
  defeating it requires deliberate circumvention that review would catch.

**[P3-1] — RESOLVED.** Memo §8 now reads "+19/−27 = net −8 lines" and tags
the round-1 error.

**[P3-2] — RESOLVED.** Memo §8 now says **240** lines (r2d_seam_freeze.rs)
and **191** lines (tests_admission_spike.rs), both tagged.

**[P3-3] — RESOLVED.** §10.1 corrects the full-suite figure to **1 371** lib
tests at d32b2c9 and acknowledges the round-1 self-contradiction. Consistent
with my measurements (1371 at d32b2c9; 1372 at 2ed2435 with the anchor).

**[P3-4] — RESOLVED, with a correction to my own round-1 attribution.**
The spike comment at :84 now cites ":1089-1092", which is correct at
`7ecc1a7` (verified: "R2 stops" at `GwzM5-8R4bP1P2-RemPlan-4.md:1089` there).
But the round-1 timeline claim — mine, repeated by memo §4.2 — that the
original ":1082-1085" citation "was already stale when the package was
parked" is **wrong**: at the park-time root (`a59c5d0`) the clause sat at
**:1082** (verified via `git show a59c5d0:…`), so the original citation was
accurate at commit. The staleness was created mid-round-1 by the thin-A1
supersession-banner commit `6a458aa` (+7 lines at the file top; **not** an
ancestor of `a59c5d0` — it landed between `a59c5d0` and `7ecc1a7`), which my
round-1 working-tree grep then observed. The fix as applied is correct
against the current head; the memo §4.2 sentence repeating the "already
stale at park" attribution (and blaming inheritance from
`GwzFasterProposal.md:160`, which was likewise accurate when written) is a
one-line historical nit for the next memo touch, not a blocker. Also noted:
§4.2 still says the spike one-liner is "owed to the lane owner at the
acceptance commit", while 2ed2435 already applied it — narrative staleness
with a real consequence recorded as P1-3 below.

**[P3-5] — RESOLVED.** Memo §3.5 now cites the enum at
`bootstrap/runtime/fault.rs:4-11` and separately names `run_next_at` as the
injection entry.

**[P3-6] — RESOLVED.** The §4.1 P2 row now owns the `sync_parent`
root-flush callers (`mutation.rs:396/:412/:426`, `directory_mutation.rs:729`)
with an explicit tag, and the P5 row lists `private_barrier`'s true
production callers (`cleanup.rs:89/:129/:159`, `residue.rs:407/:500/:529`,
`transition.rs:427`, `platform.rs:410`) — exactly matching my round-1 grep.

**[P3-7] — RESOLVED at cause.** The memo is re-baselined: §3.2 declares
"All positions in this table are at `d32b2c9`" and the Recording-backend row
now cites `:465-607` with ops `:524/:541/:561/:578` (the very numbers I
computed in round 1), keeping the 90d3f8a positions as labeled history.
Spot-checks elsewhere hold (spike spans `:72-152`/`:157-191` verified against
the file).

**[P3-8] — RESOLVED as far as the object can.** §10.2 attributes the dirty
tree (now including the other lane's red-by-design
`checked_artifact/admission/tests.rs` and `workspace_ops` edits), scopes each
gate accordingly, and gives the clippy rationale. Standing operational note
(unchanged): the acceptance/push train must run on a clean checkout.

## New finding (round 2)

### [P1-3] NEW — the boundary checker is RED at the committed 2ed2435: the P3-4 spike edit landed without the mandated same-commit digest refresh

Evidence: on a pristine extraction of 2ed2435,
`python3.13 scripts/checks/check_checked_artifact_boundaries.py` →
"checked-artifact boundary: **failed** — protected source tree changed:
`checked_artifact/capability/pre_catalog.rs`", exit 1. The round-1 extraction
of d32b2c9 was GREEN, and the only delta under that digest root is the spike
comment line. The causal mechanism is unambiguous: `source_tree_digest`
(checker :746-762) hashes the root file plus **every** file under
`capability/pre_catalog/` via unfiltered `rglob("*")` — tests-prefixed files
included (which is precisely why round 1's addition of the spike file forced
this same digest refresh). 2ed2435 changed `tests_admission_spike.rs` but not
the pinned digest value, violating the adopted same-commit refresh lane rule
the memo itself cites, and falsifying §10.2's "This revision owes **no**
digest refresh" for the *merged* commit (the sentence was written for the
three-file docs edit; the merge then also applied the spike one-liner §4.2
had recorded as "owed to the lane owner").

Not an architectural root cause: this is a mechanical gate omission in the
remediation application, so it does not trigger the two-round cap's
redesign-or-accept clause.

Minimal remediation: refresh
`PROTECTED_SOURCE_TREE_DIGESTS["checked_artifact/capability/pre_catalog.rs"]`
in `scripts/checks/check_checked_artifact_boundaries.py` in the same commit
train as the spike edit (or equivalently drop the spike edit back out to the
acceptance commit as §4.2 planned), re-run the checker to GREEN on the
committed object, and correct the one §10.2 sentence.

## Lane-owner additions (judged as in-axis code claims)

- **§3.1 persisted-home pin — ACCEPTED, code-true.** Every cite verified at
  the committed object: the `ActionAdmission{Active,Scratch,Staging}` triad in
  the enum (`protocol/slots.rs:47-58`), in `ALL` (:61-72), named at :83-85;
  issued into the infrastructure record at
  `infrastructure_record.rs:102-104` and re-checked on decode at :181-193
  with the ":191" refusal; `ActionDirectoryAdmissionV1::preparing` naming the
  staging slot at `admission.rs:153-155` and the final action name at :156.
  The pin strengthens §6's zero-new-records argument without widening any
  seam.
- **C-3 (observer/slot-grammar class, §2 + §4.4 Class 2) — ACCEPTED,
  code-true and correctly bounded.** All three cited facts verified at
  `interior.rs`: `exact_slot` walks `InfrastructureSlotV1::ALL` and refuses
  any other child ("catalog directory contains an unowned child",
  :309-329); `staging_plan` returns `Other` (:127-138) and
  `completed_record` returns `None` (:261-272) when any `ActionAdmission*`
  slot is present; `MAX_INTERIOR_ENTRIES = 10` = `|ALL|` is enforced with the
  ten-slot bound error (:64-69), zero headroom. `RootEntryNameV1`
  (`slots.rs:366-377`) already carries the `ActiveAction` arm the observer
  does not yet admit, and `RootEntryNameV1::parse` has no production caller.
  The consequence chain to `retain_completed_catalog`
  (`completed.rs:49`, `completed_record(..).is_none()` at :61) holds, so a
  first admission genuinely breaks the next `recover_or_create` reobservation
  until Phase 1 extends the observer's reading. Classifying that as an
  in-seam, Phase-1-owned grammar extension (no new slot/record/name — the
  vocabulary is already frozen per the §3.1 pin) is consistent with the code.
  Recording C-3 tightens the freeze; it does not widen it. (The reshape of
  C-2 into an extension *class* originates from the State axis and is outside
  this focused re-verdict; nothing in it contradicts this report's round-1
  C-2 code verification.)

## Verdict (round 2)

**NO-GO — on [P1-3] alone.** All ten round-1 findings from this report are
**resolved** (P1-1 by direct Windows execution, P1-2 by inventory + anchored
completeness + three corrected claim sites, P3-1..P3-8 as dispositioned
above), and both lane-owner additions are accepted as code-true. The sole
blocker is the red boundary checker at the committed 2ed2435 — a one-value
digest refresh plus one corrected sentence, after which (checker GREEN on the
committed object re-confirmed) this axis has no remaining objection to GO.

---

## [P1-3] confirmation and final verdict — 2026-08-22 (single-item follow-up)

Object: gwz-core `c40e712` ("Refresh the pre_catalog tree pin for the spike
citation fix (ReviewCode round-2 P1-3)"; delta vs `2ed2435` is exactly one
line in `scripts/checks/check_checked_artifact_boundaries.py`) and root
`85f9fe6` (memo §10.2 correction; the other changed root files are gwz tool
bookkeeping).

Verified:

- The `checked_artifact/capability/pre_catalog.rs` tree pin at c40e712 (:154)
  is `877e9d1867de17c8b6778ed2506888ead515cde75947e2d6ccfcb8c2b3a3e3c1`, and —
  decisively — the checker run on a **pristine extraction of c40e712** returns
  "checked-artifact boundary: ok (15 visible entries, 5 classified modules)",
  exit 0, which is what validates that pin value against the committed tree.
- Memo §10.2 at 85f9fe6 now records the cause (the P3-4 one-liner inside the
  pre_catalog **tree** digest), owns the same-commit-refresh miss, names the
  refreshed value, and keeps the sole remaining working-tree finding
  (`workspace_ops/merge/v1_lifecycle/mod.rs`) attributed to the other lane's
  uncommitted files.

**[P1-3] — RESOLVED.**

**FINAL VERDICT: GO.** Every finding this axis raised across both rounds is
resolved; zero P0/P1 remain. Standing operational note only (from P3-8): run
the acceptance/push train on a clean checkout of the accepted commits — the
shared working tree still carries other lanes' uncommitted files.
