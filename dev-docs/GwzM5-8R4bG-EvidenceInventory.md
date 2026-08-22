# R4b-G evidence inventory — gate requirements vs today's evidence

> Re-frame banner (2026-08-22): this inventory was drafted against the
> pre-thin-A1 frame in which R4b-G ran "after RemPlan-4's R2-R6 chain
> settles the tree" (:8; the SCOPE CORRECTION restated at :121 is
> superseded). Under the accepted `GwzM5-8ThinA1Amendment.md`, R4b-G
> follows **R2-D settle** and M5b's bound proofs; read the gate
> obligations here against that tree. The evidence items themselves are
> unaffected.

Date: 2026-08-16

Status: **DRAFT — pending lane-owner adoption.** This document inventories
every evidence obligation the R4b-G aggregate gate will check, and maps each
to what exists on today's tree, so that executing R4b-G (after RemPlan-4's
R2-R6 chain settles the tree) is a confirmation pass, not a discovery pass.
It changes no interface, runs no gate, and accepts nothing. It is a sibling
of `GwzM5-8R2D-Plan.md` and `GwzM5-8R2F-EvidenceMap.md` (the two scoping
deliverables forecast at `CurrentProgramCheckpoint.md:118-120`).

Verified tree: gwz-core HEAD `43c37bc` ("Add platform matrix workflow; file
R2-D plan and R2-F evidence map"), one commit past the accepted run-11 tuple
`f36d20dda7af631f6d9a5657affd481022bcebed`
(`CurrentProgramCheckpoint.md:14`), **with six uncommitted in-flight
modifications** at inventory time:
`src/workspace_ops/merge/abort/evidence.rs`,
`src/workspace_ops/merge/root/abort.rs`, and four files under
`src/workspace_ops/merge/v1_lifecycle/authority/` (dispatcher/invocation.rs,
observe/finalization/publication.rs, …/publication/live.rs,
observe/reverse/rollback.rs). Test counts below were listed (not run) on this
tree via `cargo test -p gwz-core --lib -- --list`: **1350 lib tests** total.
Consequence of the dirty tree: the checked-artifact boundary checker is
locally RED (two protected-tree digests:
`workspace_ops/merge/v1_lifecycle/authority/observe.rs` and
`workspace_ops/merge/v1_lifecycle/mod.rs`) because
`source_tree_digest` (`scripts/checks/check_checked_artifact_boundaries.py:746`)
hashes each protected root *plus its whole descendant tree*, and the
uncommitted files sit under both roots. That is the mechanism working as
designed (digest refresh is the lane's literal last pre-commit step,
`CurrentProgramCheckpoint.md:200-204`), not a defect finding.

## 1. Requirement sources, verbatim

### 1.1 The controlling checkpoint definition

`GwzM5-8Refactor.md:2243-2244` (checkpoint 8 of the R4b package list):

> 8. **R4b-G** runs aggregate fault, compatibility, byte-equivalence,
>    unknown-field, privacy, call-graph, and settled-tree review gates.

### 1.2 The R4b obligations the gate confirms

`GwzM5-8Refactor.md:2246-2268`, quoted in full:

> Across those checkpoints R4b must:
>
> - add member audit entries separating integration ref, final checkout, and
>   exact lock membership/row;
> - represent born and attached-unborn accepted roots without sentinels while
>   retaining M4 checked first-commit/rollback behavior;
> - persist the complete immutable accepted workspace before classifying
>   publication need and consume only its exact lock YAML after restart;
> - represent deterministic no-publication as complete publication progress
>   with no candidate or output evidence;
> - persist a typed owner before every participant, publication, preservation,
>   or rollback mutation and accept a result only through the matching opaque
>   exact-observation proof;
> - use exact phase edges rather than ordinal publication progress or generic
>   old/new record validation;
> - keep v0 and v1 mutation services separate; remove the owned serializable
>   `v0_common_view` conversion and replace shared semantics with borrowed,
>   non-serializable inputs; and
> - prove byte-equivalent lock/candidate/root/archive output and identical
>   restart actions for every M4 and all seven adapted-v0 scenarios.
>
> No skip behavior is introduced in R4b; selected and unselected M4 handling
> must remain exact.

### 1.3 Other documents that define R4b-G content

Swept every `R4b-G` mention in `dev-docs/` outside `history/` (83 lines);
these are the ones that add gate content rather than sequencing status:

- `AgentProcessRules.md:1992-2008` (§A.5, operator sequence instruction),
  verbatim:

  > 4 Once P1 and P2 are accepted, complete R4b-G:shared router integration;
  > aggregate restart/fault matrices;
  > v0 compatibility and byte-equivalence;
  > unknown-field, privacy, and call-graph gates;
  > P3/P4 status/archive cross-driver integration.
  >
  > 5 Run the two final full-tree R4b reviews.

  This adds two named items beyond checkpoint 8: **shared router
  integration** and **P3/P4 status/archive cross-driver integration**, and
  makes explicit that the settled-tree review gate is the **two full-tree
  R4b reviews** (also `GwzM5-8ProgressReviewF5.md:426-428`: "R4b-G
  settled-tree review over the full ~25k-line tree").

- `GwzM5-8ChangeBudget.md:515`: "R4b-G — aggregate gates and wiring only |
  ≤150 | 0 | ≤1,500 | ≤3 | ≤12" — zero production lines; the gate may add
  only wiring/test/doc material. `GwzM5-8ChangeBudget.md:553-556`: "R4b-G
  includes two independent settled-tree reviews with no open P0/P1/P2
  finding. A failed review or a requirement for an unowned mutation stops
  the package; it is not covered by the ledger's ordinary 20% numeric
  tolerance."

- `GwzM5-8R4bTransitionDesign.md:1170` (packages table): "R4b-G | aggregate
  fault, equivalence, compatibility, call-graph, and settled-tree gates | no
  feature work". Its §13 (`:1246-1321`) is the fullest battery definition;
  the distinctly-G bullets are quoted in §2 below where each gate is
  inventoried. Its §15 (`:1341-1348`) is the R4b exit condition: "R4b is
  complete only when every v1 durable rewrite reachable in the test harness
  is produced by the typed kernel, committed by the checked store, and
  covered by the predecessor, footprint, unknown-field, restart, and fault
  matrices. All finalization and reverse paths must consume the persisted
  accepted workspace and exact lock bytes. The production v1 writer and R3
  migration dispatch must still be unreachable."

- `GwzM5-8R4bP1P2-RemPlan-4.md:1140-1145` (R6) and `:1238`: "R4b-G resumes
  only when both [settled-tree re-reviews] report no open P0/P1/P2" /
  "Only then does step 4 of the controlling six-step sequence—R4b-G—begin."
  Note the scope distinction: RemPlan-4's R6 is the **checked-artifact**
  settled-tree re-review pair; R4b-G's own review gate is the **full-tree**
  pair (§1.3 first bullet). Both exist; they are not the same review.

- `CurrentProgramCheckpoint.md:108-111` (scope correction 2026-08-16): "the
  gate to R4b-G is **RemPlan-4's R2-R6 chain** (`GwzM5-8R4bP1P2-RemPlan-4.md`
  §4), not merely two tracked items".

Not gate content, but sequencing neighbors: `GwzM5-8OperatorEscapeDesign.md:752`
places the operator-escape implementation package "after R4b-G, before or at
A1"; it adds nothing R4b-G must check.

## 2. Named-gate inventory

Buckets: **IMPLEMENTED+TESTED** (property implemented, named suite exercises
it), **IMPLEMENTED+UNVERIFIED** (implemented, no dedicated evidence found),
**GAP** (not implemented or not verifiable → pre-G work item, §7).
Counts are from the 1350-test `--list` on the verified tree.

### 2.1 Aggregate fault gate ("aggregate restart/fault matrices")

Design battery (`GwzM5-8R4bTransitionDesign.md:1304-1307`): "fault injection
immediately before and after every durable owner, physical mutation, result,
publication-prefix, terminal, and archive boundary; restart tests from every
persisted publication, rollback, preservation, and recovery phase".

What exists today, per checkpoint (suite → count):

- v1 lifecycle: `workspace_ops::merge::v1_lifecycle::tests` — **225 tests**
  total, of which the fault/restart-bearing suites are `store` (16, incl.
  `fault_before_publish_keeps_source_and_cleans_temporary`,
  `fault_after_publish_is_recoverable_by_reopen_and_stale_retry_is_rejected`),
  `forward` (16), `dispatcher`/`dispatcher_matrix`/`dispatcher_attempt_matrix`/
  `dispatcher_reconciliation` (6+3+3+6), `effect`/`effect_retry` (8+2),
  `publication_attempt_sequence` (4), `prefixed_preservation` (3),
  `reverse::preservation` (32, incl. `root_fault_matrix.rs`,
  `root_ambiguity_matrix.rs`), `reverse::rollback` (29),
  `finalization::{tests,root_tests,input_tests}` (7+6+6, incl.
  `restart_reconciles_every_owned_publication_mutation_prefix`),
  `archive::{archive_tests,gc_tests}` (3+6), `service` (9), `status` (12).
- v0 lifecycle (g23): `abort_recovery` (7), `root_recovery` (3),
  `continue_merge` (6), `finalization` (5, driving the seven-member
  `FinalizationFault` enumeration at
  `src/workspace_ops/tests/g23/finalization.rs:4-13`), `gc` (5).
- checked-artifact: `checked_artifact::` — **244 tests**, including the
  24-key interruption/recovery matrix executed over both workspace and
  Git-directory target variants
  (`restart_and_substitution_matrix_covers_git_directory_targets`;
  `CurrentProgramCheckpoint.md:163-173, 222-234` — family 24 keys,
  inventory 164).
- v0 transition matrix: `transition_matrix_v0` (4) and `acceptance_v0` (7)
  under `workspace_ops::merge::tests`.

Status: **IMPLEMENTED+TESTED per checkpoint; GAP for the aggregate wiring.**
No single aggregate artifact (script, meta-suite, or recorded checklist)
exists that names the full fault-matrix set and runs it as one gate — which
is exactly the "wiring only" material the ChangeBudget row (≤150 test/tool
lines) reserves for G. Running the gate at G = executing every suite above
on the settled tree in one recorded pass (§5), against re-baselined counts
(§6: R2-D/R4 will change them).

### 2.2 Compatibility gate ("v0 compatibility")

What exists:

- The frozen predicate registry
  `gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` — 7
  `migration_whitelist` rules, 7 `fixture_corpus` cases, 6
  `valid_unlisted_corpus` cases — is `include_str!`-bound into the test
  binary at `src/workspace_ops/tests/g23/compatibility_v0.rs:10` and
  consumed by production dispatch at
  `src/workspace_ops/merge/record_wire/open_v0/adapter.rs:115-117`.
- Mechanical check, run green at inventory time:
  `python3 scripts/checks/check_merge_compatibility_predicates.py
  dev-docs/GwzM5-8I2CompatibilityPredicates.json --core .` → "validated 7
  migration rules and 7 runtime bindings".
- Suites: `workspace_ops::tests::g23` — **108 tests** (compatibility_v0 2,
  compatibility_v0_edges under `compatibility_v0` module family,
  characterization_* 22, atomic_upgrade assertions driven per case, plus the
  scenario modules); `record_wire::` — **65 tests** (archive 24,
  unknown_fields 23, decode/tests 18). The seven fixture-corpus cases are
  driven through `assert_i2_compatibility_fixture`
  (`g23/compatibility_v0.rs:376-470`) from the characterization scenario
  tests (`g23/characterization_v0.rs:237,493` et al.).
- CI: `.github/workflows/retained-readers.yml` validates predicates +
  retained-reader harness on ubuntu/windows and runs the platform matrix on
  five runners (linux-x86_64, windows-x86_64, linux-aarch64, macos-x86_64,
  macos-aarch64).
- Doc gate, run green at inventory time:
  `python3 scripts/checks/check_merge_docs.py` → "ok (11 sources, 147
  assertions)".

Status: **IMPLEMENTED+TESTED.** Running the gate at G = the two scripts, the
g23 + record_wire suites, and a retained-readers workflow run on the settled
tree.

### 2.3 Byte-equivalence gate

Obligation (`GwzM5-8Refactor.md:2264-2265`): "prove byte-equivalent
lock/candidate/root/archive output and identical restart actions for every
M4 and all seven adapted-v0 scenarios." Battery form
(`GwzM5-8R4bTransitionDesign.md:1311-1312`): "byte-equivalent accepted lock,
candidate, root output, and archive results across all M4 and all seven R3
adapted-v0 scenarios".

What exists:

- **Seven adapted-v0 scenarios: IMPLEMENTED+TESTED.** Each registry case
  (`changed/finalizing-before-publication-record`,
  `changed/validating-before-candidate`, `changed/candidate-persisted`,
  `changed/evidence-unrecorded`, `changed/evidence-recorded`,
  `changed/prefix-boundary`, `unchanged/no-publication-finalizing`) runs
  through `assert_upgrade_fixture`
  (`src/workspace_ops/tests/g23/atomic_upgrade_v0.rs:21-57`), which asserts
  the published upgrade bytes, the exact v1 restart action
  (`assert_v1_restart`, `atomic_upgrade_v0.rs:341`), repository observation
  equality, a per-case fault matrix, unknown-field survival + verifier, and
  the accepted-lock extension; plus the accepted-workspace byte assertions
  at `g23/compatibility_v0.rs:444-458`
  (`accepted.lock.exact_yaml == candidate.lock_yaml`). Historical executed
  evidence: "G23 compatibility/recovery suite, including all seven
  adapted-v0 registry cases: 107/107"
  (`GwzM5-8R4bAcceptance-ReviewState.md:135-136`).
- **v0 byte pinning across the refactor: IMPLEMENTED+TESTED.** The g23
  characterization suites (22 tests: characterization_v0 6,
  …publication_v0 5, …preservation_v0 5, …archive_v0 5,
  …publication_prefix_v0 1) pin exact durable shapes, e.g.
  `v0_changed_merge_windows_have_named_exact_durable_shapes`,
  `v0_terminal_completed_before_archive_is_read_only_and_closes_byte_exactly`
  (`g23/characterization_v0.rs:114,260`).
- **"Every M4 scenario": IMPLEMENTED+UNVERIFIED as an enumeration.** No
  document or suite on the tree enumerates the M4 scenario set by name and
  maps it to equivalence evidence. The candidates are the g23 scenario
  modules (start 11, continue_merge 6, preserve 13, root_start 9, root_abort
  6, root_recovery 3, root_stage/status/drift 2+2+2, drift 5, gc 5,
  finalization 5, m4_matrix 3 — mixed selected/unselected member+root) plus
  g24 (6) / g25 (5) pull-path merges. Until the mapping is written down and
  confirmed against the M4 acceptance record, "every M4" is not checkable —
  pre-G work item **W4** (§7).

Status: **split — adapted-v0 TESTED; every-M4 UNVERIFIED (enumeration gap).**

### 2.4 Unknown-field gate

Battery form (`GwzM5-8R4bTransitionDesign.md:1303`): "unknown survivor and
retirement-manifest checks after every transition".

What exists: **37 tests** whose names contain "unknown", anchored by
`v1_lifecycle::tests::store::matrix::every_transition_effect_commits_its_exact_unknown_manifest`
(the per-transition exhaustive check), the store implementation at
`src/workspace_ops/merge/v1_lifecycle/store/unknown.rs` and
`store/rewrite.rs`, `record_wire::unknown_fields` (23 tests),
`record_wire::tests::decode`, drift-survivor tests
(`store::drift::clearing_either_duplicate_drift_preserves_the_survivors_own_unknowns`),
g23 `assert_unknown_fields_and_verifier` (`atomic_upgrade_v0.rs:52`), and
first-write extension preservation
(`store::first_acceptance_write_preserves_derived_lock_member_extensions`).

Status: **IMPLEMENTED+TESTED.**

### 2.5 Privacy gate

Battery form (`GwzM5-8R4bTransitionDesign.md:1313-1314`): "a privacy/compile
test proving lifecycle code cannot construct `PreparedV1Rewrite`, proof
tokens, or call a raw v1 writer".

What mechanically enforces privacy today:

- **Rust visibility.** `PreparedV1Rewrite` is `pub(super)`
  (`src/workspace_ops/merge/v1_lifecycle/transition/mod.rs:20`); the proof
  token/authority types are `pub(super)` throughout
  (`v1_lifecycle/authority.rs:28-158`); the store commit path is
  `pub(super)` (`v1_lifecycle/store/rewrite.rs:15-23`). Construction outside
  the owning modules does not compile.
- **The boundary checker.**
  `scripts/checks/check_checked_artifact_boundaries.py` pins
  `workspace_ops/merge/v1_lifecycle/authority/observe.rs` as a protected
  compiler module (`:34`) and holds whole-tree digests over
  `v1_lifecycle/authority/observe.rs` and `v1_lifecycle/mod.rs`
  (`PROTECTED_SOURCE_TREE_DIGESTS`, `:156-157`); its unittest suite
  (`scripts/checks/test_check_checked_artifact_boundaries.py`) runs
  adversarial **compiler probes** (temp-tree cargo builds) proving e.g. an
  outside observer caller cannot hide
  (`test_approved_outside_source_target_cannot_hide_an_observer_caller`) and
  the authority-observer tree rejects nested writer helpers/new files. Wired
  in CI: `.github/workflows/checked-artifact-boundary.yml` ("Check complete
  entry and call graph") plus the per-commit lane gate
  (`scripts/checks/check_lane_commits.sh`).

What is missing vs the battery: **no dedicated privacy/compile test targets
`PreparedV1Rewrite`, the proof tokens, or a raw v1 writer** (grep: the only
occurrence of "cannot construct" is a comment, `v1_lifecycle/authority.rs:710`;
no `compile_fail`/trybuild harness exists in gwz-core). Visibility gives the
property; nothing *proves* it fails closed the way the checker's probes do
for the checked-artifact boundary. Pre-G work item **W1**.

Also arriving before G, not yet on the tree: the reverse-prefix privacy gate
from RemPlan-4's architecture — "Rust visibility and a compile-fail gate
reject imports or calls to `RollbackPrefixIssuer`/the prefix constructor
outside the reverse observer" (`GwzM5-8R4bP1P2-RemPlan-4.md`, §3 tail,
directly above §4; exit criterion 12 `:1231-1233`). `RollbackPrefixIssuer`
does not exist in `src/` today (the current seam is `pub(super) fn
issue_verified_rollback_prefix`,
`v1_lifecycle/authority/observe/reverse/rollback_prefix.rs:240`); RemPlan-4
R3 step 6 (`:1087-1101`) owns installing it. R4b-G verifies it; it is R3's
deliverable, tracked here as **W3**.

Status: **IMPLEMENTED (visibility) + partially TESTED (boundary probes for
the observe tree); GAP for the dedicated PreparedV1Rewrite/proof-token/raw
writer probe and for the not-yet-landed R3 rollback-prefix gate.**

### 2.6 Call-graph gate

Battery form (`GwzM5-8R4bTransitionDesign.md:1315-1316`): "a
source/call-graph check proving v1 lifecycle modules contain no raw v0
persistence call".

What mechanically enforces call-graph discipline today:

- The boundary checker's structural call-graph inventory (function-call
  scan `calls()`, `check_checked_artifact_boundaries.py:765`; raw-rename
  caller inventory with exact allowlist incl. alias/fn-pointer/bare-
  identifier counting, `CurrentProgramCheckpoint.md:206-210`).
- The compiler-resolved writer boundary: `clippy.toml` `disallowed-methods`
  (21 entries: `std::fs::write/rename/remove_file/create_dir*`,
  `File::create`, `OpenOptions::open`, `io::Write::write*`, `cap_std` raw
  ops, `git2::Index::write`, `git2::Reference::delete`, and the three
  gwz-core fall-through writers `artifact::write_atomic`,
  `stash::write_bundle`, `sync_workspace_boundary`), enforced by
  `#![forbid(clippy::disallowed_methods)]` whose presence the checker itself
  verifies (`check_checked_artifact_boundaries.py:802`), and by
  `cargo clippy --all-targets --all-features -- -D warnings` in the boundary
  workflow.
- CI: `checked-artifact-boundary.yml` on every push/PR, plus the per-commit
  lane gate over pushed ranges.

What is missing vs the battery: **no check asserts the specific "v1
lifecycle modules contain no raw v0 persistence call" property.** It is
structurally true today — zero references to `MergeStore`/`FileMergeStore`
in `src/workspace_ops/merge/v1_lifecycle/` outside test code (grep at
inventory time), while the v0 finalizer (`finalize_dispatch.rs:20`,
`fn finalize<B: GitBackend, S: MergeStore>`) lives outside the v1 tree — but
nothing fails closed if that changes. Pre-G work item **W2**.

Running the gate at G = boundary checker + its unittest probes +
`test_release_boundary.py` + clippy `-D warnings` on the settled tree, plus
the W2 check once it exists.

Status: **IMPLEMENTED+TESTED for the checked-artifact/writer boundary; GAP
for the v1-lifecycle→v0-persistence assertion.**

### 2.7 Settled-tree review gate

Two layers, both review-only:

- **R6 of RemPlan-4** (`GwzM5-8R4bP1P2-RemPlan-4.md:1140-1145`): the two
  independent checked-artifact settled-tree re-reviews at the exact settled
  tuple; "R4b-G resumes only when both report no open P0/P1/P2". This is a
  *precondition* of G, owned by the R2-R6 chain — cross-reference only.
- **R4b-G's own reviews**: "Run the two final full-tree R4b reviews"
  (`AgentProcessRules.md:2006`), i.e. dual independent review over the full
  R4b tree (~25k lines per `GwzM5-8ProgressReviewF5.md:426-428`), no open
  P0/P1/P2 (`GwzM5-8ChangeBudget.md:553-554`), tier per the process
  authority (dual, cross-model where available;
  `CurrentProgramCheckpoint.md:34-42`).

Status: **pending by design** — nothing to implement; execution happens at G.

### 2.8 Shared router integration (AgentProcessRules item)

Evidence today: `v1_lifecycle::tests::reverse_router` (3),
`reverse_entry` (9), `c7_matrix::request_dispatch` (2 of the 7 c7 tests),
stable reverse router landed with R4b-P P0 (`GwzM5-8Refactor.md:2228-2233`).
Status: **IMPLEMENTED+TESTED**; at G this is re-execution plus the aggregate
wiring pass.

### 2.9 P3/P4 status/archive cross-driver integration (AgentProcessRules item)

Evidence today: `v1_lifecycle::status::tests` (12),
`v1_lifecycle::archive::{archive_tests,gc_tests}` (3+6),
`archive_result` (1), archive projection tests in `record_wire::archive`
(24), and the Python driver side
`gwz-py/src/tests/test_merge_cli_cross_driver.py` (4 tests). The I2
field-10 cross-driver projection is frozen ("the already frozen I2 field-10
cross-driver projection", `GwzM5-8Refactor.md:2231-2232`).
Status: **IMPLEMENTED+TESTED**, with the caveat that P2-P4 *acceptance* is
recorded review debt (`GwzM5-8ProgressReviewF5.md:58-60,426`) that the
settled-tree reviews absorb.

## 3. Obligation-by-obligation inventory

Each heading quotes `GwzM5-8Refactor.md:2248-2268` verbatim (see §1.2 for
the uncut list).

### O1 — "add member audit entries separating integration ref, final checkout, and exact lock membership/row"

**IMPLEMENTED+TESTED.** `MemberAcceptanceV1::Selected { integration:
AcceptedIntegrationRefV1, final_checkout: AcceptedAttachedCheckoutV1,
lock_member: AcceptedLockMemberV1 }`
(`src/workspace_ops/merge/model/v1/acceptance.rs:46-56`), with
`UnselectedPresent`/`Absent` rows. Suites:
`model::v1::validate::acceptance_tests` (7, incl.
`unselected_present_and_manifest_only_absent_rows_are_distinct`),
`v1_lifecycle::finalization::input_tests` (6), builder tests
(`v1_lifecycle::tests::acceptance`, 4, incl.
`builder_accounts_for_unselected_present_and_intentionally_absent_members`),
store extension preservation
(`first_acceptance_write_preserves_derived_lock_member_extensions`).

### O2 — "represent born and attached-unborn accepted roots without sentinels while retaining M4 checked first-commit/rollback behavior"

**IMPLEMENTED+TESTED.** `AcceptedRootBaseV1 { BornAttached, BornDetached,
UnbornAttached }` — a closed enum, no sentinel values
(`model/v1/acceptance.rs:99-112`). Suites: g23 `root_start` (9),
`root_abort` (6), `root_recovery` (3), characterization
`v0_no_publication_completion_preserves_born_and_unborn_root_inputs`
(`g23/characterization_v0.rs:397`); v1 `finalization::root_tests` (6),
`reverse::rollback` (29).

### O3 — "persist the complete immutable accepted workspace before classifying publication need and consume only its exact lock YAML after restart"

**IMPLEMENTED+TESTED.** `AcceptedWorkspaceV1`/`AcceptedLockV1 { exact_yaml,
sha256 }` (`model/v1/acceptance.rs:10-41`; production-serializable only for
reading — `Serialize` is `cfg(test)`-gated). Suites:
`finalization::tests::concrete_finalizer_freezes_acceptance_and_publishes_exact_candidate`
(asserts published candidate lock == `accepted.lock.exact_yaml`,
`v1_lifecycle/tests/finalization.rs:15,45-47`),
`restart_reconciles_every_owned_publication_mutation_prefix` (`:157`),
adapted-record assertions `g23/compatibility_v0.rs:444-458`
(`accepted_workspace.is_some()`, `accepted.lock.exact_yaml ==
candidate.lock_yaml`).

### O4 — "represent deterministic no-publication as complete publication progress with no candidate or output evidence"

**IMPLEMENTED+TESTED.** Validator: `model/v1/validate/publication.rs` with
`validate::publication_tests` (5) and `canonical_tests`; lifecycle:
`no_change_finalization_freezes_acceptance_without_physical_publication`
(`v1_lifecycle/tests/finalization.rs:94`); v0 characterization:
`v0_no_publication_completion_preserves_born_and_unborn_root_inputs`,
`v0_no_publication_complete_before_terminal_write_is_read_only`
(`g23/characterization_v0.rs:397,457`); registry cases
`unchanged/no-publication-finalizing` (fixture corpus) and
`recovery/no-publication` (valid-unlisted corpus).

### O5 — "persist a typed owner before every participant, publication, preservation, or rollback mutation and accept a result only through the matching opaque exact-observation proof"

**IMPLEMENTED+TESTED.** Suites: `v1_lifecycle::tests::forward` (16),
`effect`/`effect_retry` (8+2), `authority` (7, e.g.
`bound_payload_rejects_stale_record_and_value_tampering`,
`every_authority_binding_rejects_an_identical_record_from_another_root`),
`authority::observe` (5), the four dispatcher suites (18),
`service`/`service_sequence` (9), store owner/contention checks
(`contention_and_wrong_root_are_rejected_before_mutation`), plus
`reverse::preservation` (32) and `reverse::rollback` (29) for the reverse
owners.

### O6 — "use exact phase edges rather than ordinal publication progress or generic old/new record validation"

**IMPLEMENTED+TESTED.** `predecessor_matrix` (3 exhaustive table-driven
tests: `every_operation_transition_accepts_only_its_listed_predecessor_states`,
`every_transition_variant_executes_its_declared_footprint`,
`publication_physical_action_phase_matrix_is_closed`);
`model::v1::validate::{lifecycle_tests,journal_tests,canonical_tests}`;
vocabulary closure tests (`journal_vocabulary` 2, `vocabulary` 1, plus the
spelling-closure tests under `model::v1::tests`).

### O7 — "keep v0 and v1 mutation services separate; remove the owned serializable `v0_common_view` conversion and replace shared semantics with borrowed, non-serializable inputs"

**IMPLEMENTED+TESTED (removal verified).** `v0_common_view`: zero references
in `src/` today; last touched by gwz-core `3be3485` ("Implement typed v1
merge lifecycle through R4b-X"), where it was removed. Replacement is the
borrowed acceptance input seam: `build_v1_acceptance`
(`workspace_ops/merge/acceptance/v1.rs:52`) over `RecordView<'a>` with
`&'a` fields (`acceptance/v1.rs:20-21,248-253`). Service separation: the v0
finalizer (`finalize_dispatch.rs:20`, generic over `MergeStore`) is disjoint
from `v1_lifecycle/` (no `MergeStore` references there outside tests). The
*mechanical guard* for that disjointness is the W2 gap (§2.6).

### O8 — "prove byte-equivalent lock/candidate/root/archive output and identical restart actions for every M4 and all seven adapted-v0 scenarios"

**Split** — see §2.3: seven adapted-v0 IMPLEMENTED+TESTED;
"every M4" IMPLEMENTED+UNVERIFIED pending the scenario enumeration (W4).
Restart-action identity for adapted cases: `assert_v1_restart`
(`g23/atomic_upgrade_v0.rs:341`); for v0 scenarios: `abort_recovery` (7),
`root_recovery` (3), `continue_merge` (6), characterization
recovery-overlay tests.

### O9 — "No skip behavior is introduced in R4b; selected and unselected M4 handling must remain exact."

**IMPLEMENTED+TESTED.** `m4_matrix` (3 mixed selected/unselected tests,
`g23/m4_matrix.rs:51,119,155`), model rows
(`MemberAcceptanceV1::UnselectedPresent`/`Absent`), builder coverage
(`builder_accounts_for_unselected_present_and_intentionally_absent_members`),
archive retention
(`record_wire::archive::tests::v0::unavailable_projection_retains_unselected_rows_and_still_rejects_their_mutation`),
and phase-skip rejection under O6 (`predecessor_matrix`).

## 4. Bucket summary

| Item | Bucket |
| --- | --- |
| O1 member audit | IMPLEMENTED+TESTED |
| O2 born/unborn roots, no sentinels | IMPLEMENTED+TESTED |
| O3 accepted-workspace freeze + exact lock YAML | IMPLEMENTED+TESTED |
| O4 no-publication as complete progress | IMPLEMENTED+TESTED |
| O5 typed owner + opaque proofs | IMPLEMENTED+TESTED |
| O6 exact phase edges | IMPLEMENTED+TESTED |
| O7 v0/v1 separation, `v0_common_view` removed | IMPLEMENTED+TESTED (guard = W2) |
| O8 byte-equivalence + restart identity | adapted-v0 TESTED; every-M4 UNVERIFIED (W4) |
| O9 no skip behavior | IMPLEMENTED+TESTED |
| Aggregate fault gate | TESTED per checkpoint; wiring GAP (W5) |
| Compatibility gate | IMPLEMENTED+TESTED (scripts green today) |
| Byte-equivalence gate | split (see O8) |
| Unknown-field gate | IMPLEMENTED+TESTED |
| Privacy gate | visibility + observe-tree probes; GAP W1, W3 |
| Call-graph gate | writer/observer boundary TESTED; GAP W2 |
| Settled-tree review gate | pending by design (R6 + two full-tree reviews) |
| Shared router integration | IMPLEMENTED+TESTED |
| P3/P4 cross-driver integration | IMPLEMENTED+TESTED (acceptance debt → reviews) |

Counts: **13 IMPLEMENTED+TESTED**, **1 IMPLEMENTED+UNVERIFIED** (every-M4
enumeration), **5 GAP work items (W1-W5)** of which W3 is owned by the
in-flight R3 lane, **1 pending-by-design** (reviews).

## 5. G-day runbook sketch

Preconditions: RemPlan-4 exit criteria 1-14 closed
(`GwzM5-8R4bP1P2-RemPlan-4.md:1185-1238`), `CurrentProgramCheckpoint.md`
tuple settled, clean tree, boundary digests refreshed, W1/W2/W4/W5 landed
(W3 via R3). The R5 settled gate (`RemPlan-4:1114-1138`) will already have
run once; G re-runs the aggregate on the *final* settled tree and adds the
R4b-G-only items. Wall-clock figures are rough, single-host macOS unless
noted.

1. **Mechanical doc/compat gates** (~5 min):
   `check_merge_docs.py` (147 assertions);
   `check_merge_compatibility_predicates.py … --core .` (7 rules / 7
   bindings). Both green at inventory time.
2. **Boundary + call-graph gates** (~5-10 min, Python ≥3.11):
   `check_checked_artifact_boundaries.py`;
   `python -m unittest scripts/checks/test_check_checked_artifact_boundaries.py`
   (compiler probes dominate the time);
   `python -m unittest scripts/checks/test_release_boundary.py`; the W2
   v1→v0 persistence check.
3. **Privacy probes** (~2-5 min): W1 compile probe
   (PreparedV1Rewrite/proof-token/raw-writer) and the R3-installed
   rollback-prefix compile-fail + semantic call-graph assertion (W3).
4. **Full test battery** (~20-45 min local): `cargo test --all-targets
   --no-fail-fast` across the workspace (gwz-core lib alone lists 1350
   today); record per-gate suite results for g23 (108), v1_lifecycle (225),
   checked_artifact (244), record_wire (65), model::v1 (56), acceptance_v0
   (7) + transition_matrix_v0 (4) against the re-baselined post-R2-R6
   counts.
5. **Format/lint** (~5-10 min): `cargo fmt --all -- --check`;
   `CLIPPY_CONF_DIR=$PWD cargo clippy --workspace --all-targets
   --all-features -- -D warnings`.
6. **Protocol + parity** (~10-20 min): `protocol/regen.py` diff-clean;
   `protocol/d0_roundtrip_check.py`; gwz-py suite (33 test files, incl.
   `test_merge_cli_cross_driver.py`); gwz-cli tests.
7. **CI platform evidence** (~1-3 h wall, parallel):
   `checked-artifact-boundary.yml` (incl. per-commit lane gate over the
   final range); `retained-readers.yml` (5-platform);
   `windows-matrix.yml` re-dispatch on the settled tree (run-11 form:
   1306/0/1 + 29/0); `linux-identity-probe.yml`; the R5 exact-tree
   macOS/Linux-x86/Linux-ARM64 release-evidence jobs with ext4
   remount reproduction (scope per `GwzM5-8R2F-EvidenceMap.md`).
8. **Ledger/LOC audit** (~1-2 h, manual): reconcile against
   `GwzM5-8ChangeBudget.md` (R4b-G row: ≤150 test/tool lines, 0 production,
   ≤3 unique production-bearing files).
9. **Aggregate gate record** (~1 h): one recorded pass mapping every §2 gate
   and §3 obligation to its executed evidence — this document is the
   checklist skeleton (W5).
10. **Two independent full-tree R4b settled-tree reviews** (dominates:
    expect days-to-weeks; every prior settled review of this program
    spawned a multi-round remediation cascade,
    `GwzM5-8ProgressReviewF5.md:426-428`). No open P0/P1/P2 → R4b-G closes;
    update `CurrentProgramCheckpoint.md`; proceed M5b per the resume order
    (`GwzMergeCheckpoint-v0.10.5.md:128-130`).

Machine time ≈ 2-4 h; the review pair is the long pole.

## 6. Churn risk — what R2-R6 can invalidate before G

Per-lane exposure of today's evidence (lanes per `RemPlan-4` §4 and
`CurrentProgramCheckpoint.md:108-120`):

- **R2-D (in flight now — the six dirty files above sit exactly here/nearby):
  HIGH churn.** Managed recovery on `CheckedDurablePathV1`, legacy interior
  digest-pinning/conversion. Invalidates: `checked_artifact::` count (244),
  boundary-checker digests/allowlists (already red locally), and — because
  the dirty set reaches `v1_lifecycle/authority/observe/**` and the v0
  abort path (`abort/evidence.rs`, `root/abort.rs`) — the reverse/rollback,
  publication live-observation, and abort/recovery suites. Everything in
  §2.1 re-runs at G regardless.
- **R3 (complete checkout + aggregate handoff): HIGH churn for privacy/
  call-graph and reverse suites.** Installs `RollbackPrefixIssuer` + the
  privacy/call-graph gates (W3), rebinds rollback-prefix authority, adds
  gitlink/status observation. Invalidates: `reverse::rollback` (29),
  `authority` (7) shape/count, and the *content* of the privacy gate itself
  — do not build W1 against the pre-R3 seam without checking R3's design
  first.
- **R4 (executable C7): HIGH churn for matrix evidence.** Re-keys C7 into
  stage-typed executed keys and *deletes* tuple-count/synthetic-authority
  tests once subsumed (`RemPlan-4:1111-1112`). Invalidates: `c7_matrix` (7),
  the dispatcher matrices (12), and any count cited in §2.1.
- **R5 (settled gate): regenerates all platform evidence.** The run-11 green
  Windows matrix is the baseline, but R5 requires exact-tree three-platform
  release jobs + Windows native execution on the settled tree
  (`RemPlan-4:1131-1138`; gap map in `GwzM5-8R2F-EvidenceMap.md`), so no
  platform result collected before R5 survives as G evidence.
- **R6: review-only.** No code churn unless a blocker is found (then
  consolidation → re-run everything).

Stable across R2-R6 (collect once, expect to hold):

- The I2 contracts and the frozen predicate registry
  (`GwzM5-8I2CompatibilityPredicates.json`: 7+7+6 entries; `include_str!`
  binding makes drift a compile/test failure), and the doc gate's 11
  sources / 147 assertions — stable absent a contract amendment.
- The g23 v0 characterization *pins* (v0 semantics are frozen; the suites
  must re-execute, but their expected bytes should not move).
- `model::v1::validate` suites (56) — the no-wire validators frozen since
  R4b-TI.
- The retained-readers corpus/harness and `clippy.toml` writer boundary.
- The requirement texts themselves (§1) — all frozen documents.

Practical rule for G: **all executed evidence re-runs at G on the settled
tree; what this inventory buys is knowing the checklist is complete and the
only *missing* artifacts are W1-W5.**

## 7. Pre-G work items (the GAP list)

- **W1 — privacy compile probe.** Add a fail-closed probe (natural home:
  the boundary checker's compiler-probe harness) proving code outside the
  owning modules cannot construct `PreparedV1Rewrite`
  (`transition/mod.rs:20`), any proof token (`authority.rs`), or reach a
  raw v1 writer (`store/rewrite.rs`). Battery source:
  `GwzM5-8R4bTransitionDesign.md:1313-1314`. Sequence after R3 (see churn
  note). Budget: fits R4b-G's ≤1,500 test/tool lines.
- **W2 — v1→v0 call-graph check.** Mechanically assert
  `src/workspace_ops/merge/v1_lifecycle/` production code contains no
  v0 persistence call (`MergeStore`/`FileMergeStore`/v0 writer seams) —
  today true by grep, unguarded. Battery source: `TransitionDesign.md:1315-1316`.
- **W3 — rollback-prefix privacy/call-graph gate.** Owned by RemPlan-4 R3
  step 6 (`RemPlan-4:1087-1101`, exit criterion 12): `RollbackPrefixIssuer`
  visibility + compile-fail gate + the semantic call-graph assertion for the
  `require -> ()` executor seam. G verifies; not G work — tracked so its
  absence today is not misread as a G-time surprise.
- **W4 — "every M4 scenario" enumeration.** Write the M4-scenario →
  equivalence-evidence map (candidate set: the g23 scenario modules + g24 +
  g25; confirm against the M4 acceptance record and
  `GwzM5-8Refactor.md` §15.3.3 archive rows) so O8's "every M4" clause is
  checkable. Doc-only.
- **W5 — aggregate gate wiring.** The ≤150-line "aggregate gates and wiring
  only" artifact (`ChangeBudget.md:515`): a recorded checklist/driver that
  names every §2 gate, its command, and its expected count on the settled
  tree. §5 of this document is the seed.

---

DRAFT — pending lane-owner adoption. Nothing here accepts, rescopes, or
executes any gate; adoption and any tier recording belong to the lane owner
and operator per `AgentProcessRules.md` and `GwzProcessOptimization.md`.
