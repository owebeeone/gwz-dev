# R5 / R2-F platform-evidence map — required evidence vs the green Windows matrix

Date: 2026-08-16

Status: **DRAFT — pending lane-owner adoption.** This document is the
platform-evidence gap map announced in `CurrentProgramCheckpoint.md` ("Scoping
in flight: … `GwzM5-8R2F-EvidenceMap.md` (R5/R2-F platform-evidence gap map vs
the green matrix)", CurrentProgramCheckpoint.md:119-120). It inventories every
platform-evidence obligation named by RemPlan-4 R5 and by the R2-F release
gate across current dev-docs, and maps each to its executed-evidence status as
of gwz-core `f36d20dda7af631f6d9a5657affd481022bcebed` (the run-11 GREEN
Windows matrix train). It proposes actions; it adopts nothing. Any rescope it
recommends is a *reviewed* rescope per program rules — never silent.

Baseline evidence event: Windows matrix run 11, workflow `windows-matrix.yml`,
run `31897404688` on `f36d20d` (ledger date 2026-08-16; run created
2026-08-15T17:07Z), **1306 passed / 0 failed / 1 ignored lib + integration
29/0** after the eleven-run burn-down 126 → 118 → 95 → 93 → 64 → 52 → 24 → 9 →
34 → 1 → 0 (`GwzWindowsMatrix-Classification.md:1-13`). Matrix green does NOT
close the standing residual tripwire (ledger "Standing residual tripwire"
section, GwzWindowsMatrix-Classification.md:182-197); see §5.4.

## 1. Controlling requirements, verbatim

### 1.1 RemPlan-4 R5 platform paragraph (the release gate this map serves)

`GwzM5-8R4bP1P2-RemPlan-4.md:1131-1138`:

> Exact-tree macOS, Linux x86, and Linux ARM64 release jobs must execute their
> real identity/equivalence tests. Linux jobs must also reproduce the ext4
> provider-table and unmount/remount evidence from R0-L. Windows must execute
> NTFS case-sensitive and case-insensitive directory identity, non-ASCII
> rejection, read/write handle and ACL/read-only flush failures, write-through
> publish/retire, concurrent live runtime bootstrap, catalog/barrier intent,
> roaming-anchor, managed-parent bootstrap, duplicate, and complete fault/restart
> tests. A compile or release build alone is not platform evidence.

Related R2 step (context for the Windows recovery items,
GwzM5-8R4bP1P2-RemPlan-4.md:1071-1072): "Replace Windows deletion with durable
retirement and implement closed catalog-anchor/barrier-intent/roaming-anchor
recovery."

### 1.2 R2-F obligations named elsewhere (grep `R2-F` over dev-docs, deduped)

1. **Gate definition.** "**R2-F — closure.** Remove legacy successful paths
   and run complete local and native-platform fault/restart gates."
   (`GwzM5-8R4bR2ConsumerCheckpoint.md:304-305`.)
2. **Fault/identity evidence is the gate, builds are not.** "Linux and Windows
   native fault evidence remains an R2-F release gate."
   (`GwzM5-8R2CCatalogBootstrapAmendment.md:667`.) "Native Windows and Linux
   power-loss evidence remains mandatory in R2-F; this macOS implementation
   checkpoint does not substitute for it."
   (`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:953-954`.) "Linux
   x86-64/AArch64 and Windows native execution remain mandatory R2-F evidence;
   a macOS cross-check does not substitute for those runtime rows."
   (`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:649-651`.)
3. **Windows destination-window test.** "A native Windows destination-window
   test executes at the R2-F platform gate."
   (`GwzM5-8R2CCatalogBootstrapAmendment.md:315-316`; reinforced by the §4.1
   erratum and `platform.rs:121-130`; regression shape specified at
   `GwzM5-8R2C2OwnerInterface-ReviewState-2.md:291-296` — "renames the
   destination directory (and separately an ancestor) between path derivation
   and rename" — and `GwzM5-8R2C2OwnerInterface-ReviewCode-3.md:300-303`.)
4. **Windows object-binding test.** "the object-binding and destination-window
   tests remain unexecuted until R2-F"
   (`GwzM5-8R2C2OwnerInterface-ReviewCode-4.md:146`, written pre-campaign);
   checkpoint line: "Windows destination-window + object-binding native tests
   at R2-F" (`CurrentProgramCheckpoint.md:261`).
5. **Native Windows power-loss/fault matrix.** "R2-F still requires the native
   Windows power-loss/fault matrix before release."
   (`GwzM5-8R2CCatalogBootstrapAmendment.md:488-489`, §5's closing sentence.)
   The dirent-barrier review pins the same item: the Windows durability claim
   "rests on §5's write-through model plus NTFS metadata journal ordering …
   already owed at the standing R2-F native Windows power-loss/fault matrix
   (§5's closing sentence; the checkpoint's R2-F line). Recommendation, not a
   finding: fold the adopted/nonempty-parent power-loss case explicitly into
   that existing R2-F matrix item."
   (`GwzM5-8R2C2DirentBarrier-ReviewState.md:293-299`.)
6. **Resume-window residual routed alongside the power-loss item.**
   DirentBarrier [P3-1]: "Resume paths that re-enter after the scratch bytes
   are complete skip the root barrier; a narrower strict-model window to a
   non-durable containing dirent at Complete remains"
   (`GwzM5-8R2C2DirentBarrier-ReviewState.md:126-127`); required correction
   "(a) issue the same idempotent root barrier once in the common Ready-edge
   mutation prologue … or (b) a bounded §6-style erratum … Route with the
   original finding's tracking (next matrix package / R2-F power-loss
   semantics)" (:173-181); verdict routing "routes to the next
   matrix/durability package or a bounded §6-style erratum, alongside the
   R2-F power-loss item" (:394-395). Residual risk restated: "Power-loss
   (dirent-loss) semantics remain reasoned, not executed" (:360-361).
7. **Checkpoint R2-F candidates.** "MAX_PATH product exposure (~173-char
   `ca1-*` names; the private-area relocation option under `.git/` would
   retire it — candidate for R2-F scope)" (`CurrentProgramCheckpoint.md:144-147`;
   ledger fixture note GwzWindowsMatrix-Classification.md:158-161). Also from
   checkpoint item 4: native verification of the two incorporated Windows
   compile corrections "is owed at the R2-F gate or the next Windows dispatch
   run" (`CurrentProgramCheckpoint.md:283-284`).
8. **Source-level bypass inventory at closure.** "Actual source inspection
   confirms the corresponding legacy creators still exist, so R2-F must retain
   a source-level bypass inventory/gate rather than relying only on functional
   tests." (`GwzM5-8R4bR2ConsumerCheckpoint-ReviewFS.md:153-156`.)
9. **Strict-window/publication tests' native runs.** "native Linux/Windows
   runs stay at the R2-F gate where §5/§8 already put them"
   (`GwzM5-8R2C2PublicationAudit.md:69-70`).
10. **Executed-matrix native reruns.** "Native Linux/Windows execution remains
    at the R2-F gate." (`CurrentProgramCheckpoint.md:173`, closing the local
    23/24-key matrix item.)

## 2. Consolidated inventory and status map

Buckets: **PROVEN** (a specific test exists and executed green natively on the
right platform, cited), **EXISTS-UNEXECUTED** (test exists; not yet executed
natively where required), **MISSING** (no test exists), **BLOCKED** (requires
machinery or a prior program stage that does not exist yet). Split rows state
both halves.

### 2.1 Non-Windows platform rows (R5 sentence 1-2)

| ID | Required item | Status | Evidence / gap |
| --- | --- | --- | --- |
| P-1 | macOS release job executes real identity/equivalence tests | **EXISTS-UNEXECUTED (no CI job)** | Tests exist and run continuously on the macOS dev host (e.g. `native_macos_provider_issues_the_persistent_identity_profile`, `production_tests.rs:502-521`; full `checked_artifact::` suite 244/0 on Darwin 25.5.0/APFS, `GwzM5-8R2C2DirentBarrier-ReviewState.md:329`). But **no workflow runs `cargo test` on any macOS runner** — `release.yml` matrix is `ubuntu-22.04` + `windows-2022` only (release.yml:26-28). Runner availability is proven in-repo: `retained-readers.yml` matrix executes on `macos-14` (arm64) and `macos-15-intel` (x86_64), green on `f36d20d` (run `31897404838`). |
| P-2 | Linux x86 release job executes real identity/equivalence tests (+ the full native fault/restart suite, §1.2 items 2/10) | **EXISTS-UNEXECUTED** | The job exists (`release.yml` `ubuntu-22.04`, `cargo test --locked`), but **no green run exists on the current dev tree**: the 2026-08-14 dispatches failed — the ubuntu leg at the "Checked-artifact boundary" step (run `31818093061`), the exact `clippy::redundant_guards`/probe-lockfile class since fixed at gwz-core `6edb9cb` (boundary workflow first green push run `31880974224`; latest green `31897404801` on `f36d20d`) — and the windows leg on the pre-campaign red suite. Last green release verify is the v0.10.5 tag run `31820283530` on the *released* line, not this tree. One dispatch on `f36d20d` is expected green. |
| P-3 | Linux ARM64 release job executes real identity/equivalence tests | **EXISTS-UNEXECUTED (no CI job)** | No workflow runs `cargo test` on ARM64 anywhere. Tests are the same platform-neutral suites as P-2. Hosted ARM runners are available and proven in this repo: `linux-identity-probe.yml` native job runs on `ubuntu-24.04-arm` (green runs `31536272593`, `31537368340`) and `retained-readers.yml` runs its `linux-aarch64` leg on `ubuntu-24.04-arm` (green on `f36d20d`, run `31897404838`). |
| P-4 | Linux jobs reproduce the ext4 provider-table and unmount/remount evidence from R0-L | **PROVEN (R0-L tree) / re-execution owed on the settled tree** | Machinery exists and is dispatch-able: `linux-identity-probe.yml` runs the fail-not-skip ext4 remount probe on both `ubuntu-24.04` and `ubuntu-24.04-arm`, covering fixed-UUID mkfs/remount, `FS_IOC_GETFSUUID`, `AT_EMPTY_PATH` handle stability across unmount/remount, pathname-replacement immunity, and "the exact 15-row negative provider table, including real overlay and tmpfs mounts" (`scripts/linux_identity_probe/README.md`). Accepted evidence: runs `31536272593` (core `11d0817`, both architectures, `GwzM5-8R4bLinuxIdentityEvidence.json`) and hardened `31537368340`; acceptance "GO: accept R0-L and begin R1" (`GwzM5-8R4bLinuxIdentityEvidence-Review-2.md:25`). R5 demands reproduction on the exact settled tree — a dispatch (sub-minute job) at R5 time; one can be run this week on `f36d20d` to prove currency. |

### 2.2 Windows rows (R5 sentence 3)

All "run 11" citations below are test names extracted from the run
`31897404688` log (lib suite 1306/0/1 on `windows-2022`, default
case-insensitive NTFS, rustc 1.95.0, tree `f36d20d`).

| ID | Required item | Status | Evidence / gap |
| --- | --- | --- | --- |
| W-1 | NTFS case-insensitive directory identity | **PROVEN** | Run 11: `provider::production_tests::platform_equivalent_index_spelling_collides`, `platform_equivalent_workspace_parent_spelling_rejects`, `exact_ancestor_descendant_stage_flag_and_gitlink_collisions_are_read_only`, plus the real-provider lease/preflight path (`production_provider_observation_is_derived_from_the_workspace_lease`, `catalog_tests::preflight::ready_permit_revalidation_rejects_namespace_drift`). Per-directory mode read is production code: `provider/platform/windows.rs:35-53` (`FILE_CASE_SENSITIVE_INFO` → `AsciiCaseFold`/`Sensitive`). Optional hardening: a Windows twin of the macOS native-profile assertion (~15 LOC) — see G-6. |
| W-2 | NTFS case-**sensitive** directory identity | **MISSING** | No test flips `FILE_CS_FLAG_CASE_SENSITIVE_DIR`; the `PathComponentMode::Sensitive` arm of `windows.rs:50-53` has never executed natively (grep: no `fsutil`/`setCaseSensitiveInfo`/`FileCaseSensitiveInfo`-write anywhere in tests). Action G-2. |
| W-3 | Non-ASCII rejection | **PROVEN (classifier/alias level); one physical test EXISTS-UNEXECUTED on Windows** | Run 11: `interface_tests::catalog_classifier::non_ascii_native_names_fail_closed_only_on_case_fold_parents`, `provider::production_tests::case_fold_parent_alias_scan_rejects_non_ascii_names_before_callback`, `catalog_lease::tests::bounds::case_fold_alias_scan_rejects_non_ascii_names_after_charging_them`. Gap: the physical on-disk variant `physical_case_fold_parent_rejects_non_ascii_equivalent_fixed_and_scratch_names` is `#[cfg(target_os = "macos")]` (`provider/catalog_tests/grammar.rs:99-101`) and did not run on Windows, although the scenario (case-fold parent) applies there. Action G-3 (cfg extension). |
| W-4 | Read/write handle and ACL/read-only flush failures | **PROVEN (as a campaign-executed class)** | The class was executed natively and adversely before being closed: W3 = "Access denied (os error 5) on preservation artifacts/markers … Windows ACL/read-only attribute vs reopen-for-DELETE pattern" (ledger :34), instrumented to "all 35 are `sync family entry` (rebarrier_exact: read-only reopen + `sync_all`, which Windows' FlushFileBuffers forbids)" (ledger :84-86; production site `residue.rs:508`/`:527`), closed at run 5 (`31883193051`, `os error 5`: 0, "family-entry re-sync is Unix-only per the write-through doctrine" ledger :88-92); W4 sharing-mode class closed runs 4-7 (`open_dir_share_delete`, publish-handle release; zero `os error 32` at run 7, ledger :167-179). The g15/preservation suites that exercise these artifacts are green in run 11. Note: the read-only-flush behavior is now *avoided by doctrine* on Windows rather than asserted by a dedicated negative test; optional hardening in G-6. |
| W-5 | Write-through publish/retire | **PROVEN** | Run 11: `mutation_tests::{replacement_before_active_rename_is_not_published, destination_created_inside_the_window_is_not_replaced, in_place_byte_drift_with_unchanged_identity_is_rejected_before_publication, kind_swap_inside_the_window_is_left_untouched, replacement_before/after_rewrite_open_*}`, `directory_mutation_tests::{every_anchor_namespace_prefix_converges_after_restart, changed_staging_contents_before_final_publish_are_not_published, changed_final_contents_before_retirement_leave_active_in_place, substituted_active_before_retirement_is_not_moved_into_final, interior_drift_after_final_recheck_is_rejected_inside_the_primitive, destination_drift_after_retire_recheck_is_rejected_inside_the_primitive}`, plus the two positive Windows guarantees: `checked_artifact::tests::retained_directory_blocks_substitution_rename_windows` (`tests.rs:147`) and the object-binding test (F-2). |
| W-6 | Concurrent live runtime bootstrap | **PROVEN** | Run 11: `operation::workspace_mutator_lock::tests::child_process_observes_lock_contention` (real second process, `workspace_mutator_lock.rs:280`, self-invoking at :270) plus in-process contention rows `catalog_lease::tests::{workspace_batch_target_contends_on_the_existing_compatibility_lock, later_target_contention_releases_every_earlier_final_lock, linked_worktrees_resolving_to_one_git_target_share_one_final_lock}`. Note: the three runtime substitution-fault tests are `#[cfg(unix)]` (`bootstrap/runtime/tests.rs:10,33,65`) — consistent with the recorded doctrine that the substitution race is unproducible against retained handles on Windows (`production_tests.rs:544-545` comment), with `retained_directory_blocks_substitution_rename_windows` as the positive counterpart. |
| W-7 | Catalog/barrier intent | **SPLIT: catalog intent PROVEN; barrier-intent physical BLOCKED (R2-D)** | Catalog: the complete 24-key `catalog_bootstrap.*` fault family executed natively both target variants — run 11 `catalog::bootstrap::tests::restart_and_substitution_matrix_covers_every_catalog_bootstrap_fault_key` and `restart_and_substitution_matrix_covers_git_directory_targets` (intent generations/markers included in the schedule). Barrier-intent: the `barrier.*` family (16 keys, `fault_v1.rs:131-145`) is one of the reserved injection-free families (140 keys total: admission 19, barrier 16, cleanup 11, durable_leaf 11, managed_bootstrap 30, namespace 11, record 13, runtime 18, terminal 11) awaiting R2-D+ conversion — "each future conversion package must bring its own sites and rows" (`GwzM5-8R2C2OwnerInterface-ReviewCode-3.md:497-499`; `GwzM5-8R2C2PublicationAudit.md` P3-1 disposition). Record-schema level did run natively (`interface_tests::schedule_records::barrier_intent_id_binds_every_persisted_field`). |
| W-8 | Roaming-anchor | **PROVEN (catalog lane)** | The `RoamingAnchorHome` staging slot and its recovery plans (`provider/interior.rs:25-171`, write/reobserve/missing-later logic) are exercised by the catalog bootstrap suite and both executed fault matrices (slot fixtures `catalog/bootstrap/tests.rs:159,274`), all green natively in run 11; the Windows durability-anchor machinery itself also ran (`platform::windows_tests::anchor_roundtrip_name_remains_native`; class-B anchor fix verified on the runner at run 8, ledger :209-217). |
| W-9 | Managed-parent bootstrap | **BLOCKED (R2-D)** | The named `ManagedParentBootstrap` route exists as frozen contracts only: `bootstrap/managed.rs` — "R1 freezes the execution owner before R2 production conversion" (`managed.rs:6-9`); the `managed_bootstrap.*` fault family (30 keys) is reserved/injection-free. Legacy managed-parent durability behavior did execute natively (run 11: `checked_artifact::tests::durability::removal_recovers_without_a_managed_parent_tombstone`, `existing_replacement_recovers_after_detach_and_obtains_durability`), but that is not the converted route R5 names. Evidence becomes writable only after the R2-D conversion package lands (see `GwzM5-8R2D-Plan.md`, in flight). |
| W-10 | Duplicate | **SPLIT: dedup/lease level PROVEN; converted duplicate-repair route BLOCKED (R2-D/E)** | Run 11: `catalog_lease::tests::duplicate_targets_are_deduplicated_and_held_in_canonical_order`, `association::{duplicate_location_with_changed_identity_rejects_before_preparation_in_both_orders, duplicate_location_requires_exact_live_target_and_repository_bindings}`, `interface_tests::catalog_classifier::malformed_duplicate_and_equivalent_reserved_names_are_ambiguous`. The R2 step-6 rerouting of *duplicate repair* "through the durable leaf/namespace interfaces" (RemPlan-4:1069-1070) is an R2-D/E conversion; its Windows evidence rides the post-conversion matrix like W-7b/W-9. |
| W-11 | Complete fault/restart tests | **SPLIT: catalog family PROVEN; remaining 140 reserved keys BLOCKED (R2-D+)** | Catalog: both matrices (24 keys, workspace + Git-directory targets, interrupt+restart+converge per key) executed natively green in run 11 — the first native-Windows execution of the complete catalog fault matrix. Legacy fault machinery also ran (50 fault-named tests green). The other nine families are reserved by design until their conversions land (citations as W-7). The standing rule this map proposes (§4): **every R2-D/R2-E conversion package ends with a `windows-matrix.yml` dispatch plus a Linux full-suite run**, so the family-by-family evidence accretes with the conversions instead of ballooning at R2-F. |

### 2.3 R2-F items named outside the R5 list

*[Re-owned 2026-09-01 (R2-F R1.3): the quarantine/relocation package this
section anticipated has LANDED — R1.1 `027da5b` + R1.2 tripwire `bb52dc0` on
gwz-core main, plan `GwzM5-8R2F-RelocationPlan.md` (ADOPTED, three rounds).
F-5's relocation half executed without retiring MAX_PATH (see its row's dated
note). F-4 is unchanged and rides the next matrix train (G-4). Every other
row keeps its stated owner and status.]*

| ID | Required item | Status | Evidence / gap |
| --- | --- | --- | --- |
| F-1 | Native Windows destination-window test (§1.2 item 3) | **MISSING** | No such test exists (grep: `platform.rs` `windows_tests` module contains exactly `anchor_roundtrip_name_remains_native` and the object-binding test; no `destination_window` anywhere). The production window is `platform.rs:110-142` (`windows_destination_path` derivation → comment block :121-130 stating the residual and "native window test executes at R2-F" → `SetFileInformationByHandle`), and it has **no in-window hook**, so the prescribed rename-in-window injection needs a `cfg(test)` hook first. Action G-1. |
| F-2 | Native Windows object-binding test (§1.2 item 4) | **PROVEN** | `checked_artifact::platform::windows_tests::rename_open_source_moves_the_checked_object_after_path_substitution` (`platform.rs:612-657`, `cfg(all(test, windows))`) executed green in run 11 (log 2026-08-15T17:09:15Z, "... ok"). ReviewCode-4:146's "remain unexecuted until R2-F" is discharged for this half; the destination-window half remains (F-1). |
| F-3 | Native Windows power-loss/fault matrix (§1.2 items 5-6) | **SPLIT: process-fault half PROVEN; power-loss (dirent-loss) half BLOCKED** | Process-death fault/restart: executed natively (W-11 catalog matrices, run 11). Power loss: no machinery exists on any platform — the matrix "simulates process death; no row simulates a lost-but-previously-visible dirent" (`GwzM5-8R2C2OwnerInterface-ReviewState-2.md:474-476`) and "Power-loss (dirent-loss) semantics remain reasoned, not executed" (`GwzM5-8R2C2DirentBarrier-ReviewState.md:360-361`). Honest feasibility assessment in §3/G-7: (i) constructed post-loss-state rows are executable everywhere now; (ii) true writeback-order evidence is feasible on Linux hosted runners (dm-flakey/dm-log-writes; sudo+loop devices proven by the ext4 probe); (iii) true power-cut evidence on hosted **Windows** runners is not feasible — recommend a reviewed rescope (§5.3). |
| F-4 | Resume-window residual (DirentBarrier [P3-1], routed alongside F-3) | **MISSING (correction not yet landed)** | The review offers correction (a) — idempotent root barrier in the common Ready-edge mutation prologue ("six idempotent fsyncs at bootstrap frequency, negligible") — or (b) a bounded §6-style erratum (:173-181). Correction (a) plus its prescribed regression row (arm the barrier key on the resume drive and assert the slot is consumed, :182-188) is small and rides the next matrix train. Action G-4. |
| F-5 | MAX_PATH / private-area relocation candidate (§1.2 item 7) | **MISSING (evidence) + open product decision** — *relocation half executed 2026-09-01 WITHOUT retiring the exposure* | Real product exposure: staged `ca1-*.source` names ≈173 chars breach MAX_PATH on default Windows; currently *test-masked* by the run-7 `core.longpaths=true` fixture pin (ledger :158-161), so CI is green while an ordinary user without the registry/git opt-in is not covered. Interim evidence action G-5 (fail-closed negative test without longpaths); the relocation itself ("under `.git/`") is a product package needing design + dual review — rescope-class routing in §5.3. *[R1.3 note 2026-09-01: the relocation package landed (R2-F R1.1, gwz-core `027da5b`) and FALSIFIES the "would retire it" premise — trace §5.2: +4 chars under `.git/`, 160 of 173 chars are the hex triple, and the landed design keeps the `ca1-*`-bearing legacy area at `.gwz/checked-artifacts` (only the catalog moved, to `catalog-final`). `max_path.rs` re-measured at R1.1, naming WHICH git directory each arm measures (commondir for the catalog; per-worktree `repo.path()` for the legacy git-dir arm). The exposure stays a live product item, owned by the name-shortening class; the open product decision stands, sharpened.]* |
| F-6 | Linux native fault evidence (§1.2 item 2) | **EXISTS-UNEXECUTED** | Same suites as W-11 exist and are platform-neutral; they have executed natively on macOS (host) and Windows (run 11) but not on Linux CI for the current tree (see P-2 — the ubuntu release leg never reached its Test step on the failed 2026-08-14 dispatches). Folds into the P-2/P-3 dispatches. |
| F-7 | R2-F closure's legacy half: remove legacy successful paths + retain a source-level bypass inventory/gate (§1.2 items 1/8) | **MISSING (gate tooling); conversions are R2-D/E lane work** | The boundary checker now inventories raw-rename callers subsystem-wide with an exact allowlist (landed with the round-3 remediation; hardened at `641f03c`), but the *legacy-creator* bypass inventory demanded by ReviewFS:153-156 (conversion-table creators: runtime lock, merge store/archive, stash bundles, root markers, workspace lock/marker, `.git/info/exclude`) does not exist as a check. Action G-8. |
| F-8 | Standing residual tripwire + amendment OPEN DECISIONS | **BLOCKED (reviewed decision required; not test-closable as-is)** | "Green does NOT close the standing residual tripwire … real-Windows exact-evidence satisfiability on filter-materialized worktrees and the `stash_save` filtered reset remain reviewed-open decisions; CI is structurally blind to both by fixture hermeticity" (ledger :8-13, detail :182-197). The amendment records both: "The two OPEN DECISIONS in Clause A's scope limits (foreign-filter policy; real-Windows raw-byte satisfiability follow-up) remain tracked review debts — acceptance does not close them" (`GwzM5-8ExactEvidencePlatformAmendment.md:11-13`). Candidate designs already named: recovery-grade full re-materialization at rollback entry, filters-off at member materialization, or a clean-side comparison mode (ledger :195-197). Routing in §5.3. |
| F-9 | Native verification of the two incorporated Windows compile corrections (§1.2 item 7, second half) | **PROVEN (discharged)** | Owed "at the R2-F gate or the next Windows dispatch run" (checkpoint :283-284); the dispatch runs happened: run 1 (`31880022755`) already showed "zero `os error 87` occurrences (the `d84a30d`/`a350746` corrections … hold on a real runner)" (ledger :22-25), the missed twin site (`preservation_root/parent.rs::rename_windows`) was found and fixed, and its NEEDS-WINDOWS-VERIFICATION was discharged at run 7 (ledger :152-155, :167-175); run 11 green on the containing tree. |

## 3. Gap actions (every MISSING / EXISTS-UNEXECUTED / BLOCKED row)

Effort figures are aspirational estimates in the <500-LOC step discipline;
none is a hard limit. "Matrix train" = one batched commit set + one
`windows-matrix.yml` dispatch (the ledger's recorded lesson: batching fix
packages per dispatch is the dominant accelerator, runs complete in 6-40 min).

- **G-1 (F-1) Windows destination-window test.** Add a `cfg(test)` single-shot
  hook inside `rename_open_source` between `windows_destination_path`
  derivation and `SetFileInformationByHandle` (`platform.rs:110-135`; the
  `fault_v1` hit pattern is the precedent), then a
  `cfg(all(test, windows))` companion to the object-binding test that, in the
  hook, (v1) renames the destination directory away and creates a replacement
  at the same absolute path, and (v2) renames an *ancestor* — asserting per
  the ReviewState-2 spec that the operation "fails with the source untouched
  and never delivers into the replacement directory", or that the post-publish
  retained-handle verification rejects. The hook touches the sealed primitive
  file: keep it observation-only/`cfg(test)` and expect the protected-tree
  digest refresh in the same commit; if the reviewers read it as a seam
  change, it takes the dual-review route per L2-06. ~15 LOC production hook +
  ~100-140 LOC test. Rides a matrix train.
- **G-2 (W-2) NTFS case-sensitive directory identity.** New
  `cfg(all(test, windows))` test: create a temp directory, enable per-directory
  case sensitivity (shell `fsutil.exe file setCaseSensitiveInfo <dir> enable`
  — hosted runners execute elevated — or set `FILE_CASE_SENSITIVE_INFO`
  directly via `SetFileInformationByHandle`), assert
  `parent_mode == PathComponentMode::Sensitive` (`windows.rs:50-53`), then
  assert case-variant names get distinct component identities and the
  case-fold alias/rejection branches do NOT fire (mirror of the existing
  fold-mode tests). Fail-not-skip if the flag cannot be enabled, mirroring the
  R0-L probe doctrine — but de-risk first with a 2-minute probe dispatch on a
  throwaway branch (same pattern as `probe/finalization-diagnosis`) to confirm
  the windows-2022 image accepts the flag; if the image refuses it, that fact
  goes to §5.3 as a reviewed decision instead of a silent skip. ~80-150 LOC.
  Rides the same matrix train as G-1.
- **G-3 (W-3 physical half).** Extend
  `physical_case_fold_parent_rejects_non_ascii_equivalent_fixed_and_scratch_names`
  from `#[cfg(target_os = "macos")]` to
  `#[cfg(any(target_os = "macos", windows))]` (`grammar.rs:99`) — the test is
  written against fold-mode semantics with an existence guard, not APFS
  specifics. ~2 LOC + verify on the train. If it trips on NTFS's non-ASCII
  upcase behavior, that is itself evidence and gets a Windows-specific
  variant (~40 LOC).
- **G-4 (F-4) Resume-window correction.** Implement DirentBarrier [P3-1]
  correction (a): the idempotent `finish_private_parent_edge(retained.root())`
  barrier in the common Ready-edge mutation prologue (owner dispatch,
  `catalog/bootstrap.rs:164-224` layer), keeping preflight read-only, plus the
  prescribed regression row (fault at `catalog_bootstrap.scratch_write`,
  converge on restart, assert the barrier key is consumed on the resume
  drive). Also apply the [P3-2] comment/label pass (cite §3/§6 not §5;
  generalize the Windows-arm rationale and error label, `mutation.rs:183`,
  `:375`, `:379-383`) in the same commit. ~80-140 LOC. Rides the train; the
  new/changed keys re-execute natively on the same dispatch.
- **G-5 (F-5) MAX_PATH interim evidence.** A `cfg(windows)` negative test that
  constructs a workspace path long enough that a staged `ca1-*` name breaches
  MAX_PATH *without* `core.longpaths`, and asserts the typed fail-closed
  rejection (no partial mutation, classifiable state) rather than an OS panic.
  ~60-90 LOC. Rides the train. The *relocation* decision stays a reviewed
  product package (§5.3).
- **G-6 (W-1/W-4 hardening, optional).** (a) Windows twin of the macOS
  native-profile test asserting the provider issues the Windows NTFS profile
  on the runner (~15 LOC). (b) A directed `cfg(windows)` negative test
  asserting the read-only-reopen flush path is structurally unreachable /
  typed on Windows (today enforced by the Unix-only cfg at
  `residue.rs:508-527`) (~40-60 LOC). Non-blocking; take them on a train with
  spare room.
- **G-7 (F-3 power-loss half).** Three-tier plan, honest about what each
  proves:
  1. **Constructed dirent-loss rows (portable, in-process).** After a
     converged bootstrap (or a mid-schedule captured state), artificially
     revert the containing dirent (remove `.gwz`/`gwz` subtree or the specific
     parent entry), restart, and assert classification lands on a legal row
     (fresh Missing or surviving prefix) and re-converges — turning the
     review's *analytic* all-or-nothing argument into executed evidence on
     macOS, Linux, and Windows. Extends the existing matrix machinery;
     ~150-250 LOC. This does NOT prove kernel writeback ordering; it proves
     the state machine over the modeled post-loss states.
  2. **Linux true writeback-order evidence.** dm-log-writes (or dm-flakey)
     harness over a loop-mounted ext4: run a bootstrap under log-writes,
     replay prefixes at every flush boundary, run classification on each
     replayed image, assert convergence. Hosted `ubuntu-24.04`/`-arm` runners
     provide sudo, loop devices, and device-mapper (the ext4 probe already
     uses sudo/mkfs/mount). New workflow + scripts, own reviewed package;
     ~400-700 LOC. This is real "Linux native fault evidence" beyond
     process-kill and satisfies the Linux half of §1.2 item 5.
  3. **Windows true power-cut: not feasible on hosted runners.** No
     writeback-fault-injection facility, no VM power-cut control on
     `windows-2022` hosted images. Realistic options: a documented manual
     procedure on a local Hyper-V/VM (operator-run, evidence JSON checked in),
     or a reviewed rescope accepting tiers 1+2 plus the §5 write-through/NTFS
     journal doctrine, recorded via the §6-style erratum form DirentBarrier
     correction (b) already sanctions. Recommendation in §5.3.
- **G-8 (F-7) Legacy-creator bypass inventory.** Extend
  `scripts/checks/check_checked_artifact_boundaries.py` (or a sibling check)
  with an explicit inventory of the conversion-table legacy creators and an
  allowlist that R2-D/E packages shrink to zero; wire into
  `checked-artifact-boundary.yml` beside the existing per-commit lane gate.
  ~100-200 LOC script + unit tests. This is the tooling half of R2-F closure;
  the conversions themselves are the R2-D/E lane (see `GwzM5-8R2D-Plan.md`).
- **G-9 (P-1/P-2/P-3/F-6) Release-matrix completion + dispatches.** Add
  `macos-14` (arm64; plus `macos-15-intel` if the release line ships x86_64
  macOS) and `ubuntu-24.04-arm` rows to `release.yml`'s verify matrix
  (test-only on the new rows; Format/boundary/Clippy stay on the Linux x86
  leg), then dispatch on `f36d20d`. ~10-20 LOC YAML. Known small risk: the
  pinned `taut-proto==0.8.1` pip install on linux-aarch64/macOS runners (wheel
  availability); fallback is sdist install or gating the two
  protocol-currency tests to platforms with the generator, keeping the pinned
  x86 leg authoritative (the W6 precedent, ledger :148-151).
- **G-10 (P-4) ext4 probe currency.** Dispatch `linux-identity-probe.yml` on
  `f36d20d` now (sub-minute), and again on the exact settled tree at R5. 0 LOC.

## 4. Standing process rule proposed for adoption

Because 140 of 164 fault keys are reserved for R2-D+ conversions (W-7/W-9/
W-10/W-11), the only way R5's "complete fault/restart" bullet stays affordable
is accretive evidence: **every R2-D/R2-E conversion package that adds fault
sites/rows ends with (a) a `windows-matrix.yml` dispatch and (b) a Linux
full-suite run on the same commit, recorded in the package's closure note.**
The dispatch machinery is retained for exactly this (ledger :49-50). Adopting
this rule is a lane-owner decision; this map only proposes it.

## 5. Prioritized closure plan

### 5.1 THIS WEEK — rides existing machinery (windows-matrix / probe / release dispatch)

Ordered; items 3-8 batch onto one or two matrix trains per the batching
lesson.

1. **G-9 dispatch half:** dispatch `release.yml` on `f36d20d` as-is → first
   green Linux-x86 full-suite (identity/equivalence + catalog fault matrices
   native on Linux) and a green Windows release-verify. Expected green: the
   ubuntu blocker (`6edb9cb`) and the Windows suite (run 11) are both fixed.
   0 LOC.
2. **G-9 matrix half:** add `ubuntu-24.04-arm` + `macos-14` (± `macos-15-intel`)
   rows; dispatch again → P-1/P-3 native evidence. ~10-20 LOC.
3. **G-10:** dispatch the ext4 probe on `f36d20d`. 0 LOC.
4. **G-1:** destination-window hook + test (~120-155 LOC) — clears the single
   most-cited named R2-F test debt.
5. **G-2:** NTFS case-sensitive probe dispatch, then the test (~80-150 LOC).
6. **G-3:** cfg-extend the physical non-ASCII test (~2 LOC).
7. **G-4:** resume-window correction (a) + regression row + comment/label pass
   (~80-140 LOC).
8. **G-5:** MAX_PATH fail-closed negative test (~60-90 LOC).

Completing 1-8 moves every Windows R5 bullet except the R2-D-gated ones
(W-7b/W-9/W-10b/W-11b) and the power-loss half of F-3 to PROVEN, and gives all
four non-Windows rows current native runs.

### 5.2 NEW INFRASTRUCTURE — own packages, normal review

1. **G-7 tier 1:** constructed dirent-loss rows in the fault matrix
   (~150-250 LOC; portable; executes on the same trains thereafter).
2. **G-7 tier 2:** Linux dm-log-writes writeback-replay harness + workflow
   (~400-700 LOC; the substantive new machinery; satisfies the Linux half of
   the power-loss gate with real reordering evidence).
3. **G-8:** legacy-creator bypass inventory/gate (~100-200 LOC; the R2-F
   closure tooling half).
4. Optional: a small `workflow_call` aggregator so R5's exact-tree evidence
   (release matrix + probe + windows-matrix) is one dispatch at settlement
   (~50 LOC YAML).

### 5.3 REVIEWED RESCOPES recommended (never silent; each needs its own recorded decision)

1. **Windows true power-cut evidence (F-3).** Physically infeasible on hosted
   CI. Propose: accept G-7 tiers 1+2 plus the §5 write-through/NTFS-journal
   doctrine as the Windows power-loss evidence basis, recorded as a bounded
   §6-style erratum (the form DirentBarrier [P3-1] correction (b) already
   sanctions), with an optional operator-run VM power-cut runbook as
   strengthening. Disproportionate to force further: the alternative is
   self-hosted infrastructure for one evidence row.
2. **Exact-evidence tripwire + foreign-filter OPEN DECISIONS (F-8).** These
   are product-semantics decisions among the three named candidates, not test
   gaps; CI is structurally blind by design of the fixture pins. Propose a
   dedicated decision package in the amendment lane; until it lands, the
   ledger tripwire language stays controlling ("do not read matrix-green as
   real-Windows exact-evidence closure").
3. **MAX_PATH private-area relocation (F-5).** The relocation under `.git/` is
   a product/design package (R2-D-adjacent), not an R2-F evidence item; keep
   G-5 as the R2-F evidence and route the relocation to its own reviewed
   design.
4. **Conditional:** if the G-2 probe shows hosted `windows-2022` images refuse
   per-directory case sensitivity, decide between a container/self-hosted
   variant and a recorded acceptance that the Sensitive arm is
   doctrine-covered — by review, not by a skipped test.

### 5.4 Explicitly NOT closed by any of the above

The standing residual tripwire and both amendment OPEN DECISIONS (F-8) remain
open until their own reviewed decisions land, regardless of how green the
platform matrix is; and the R2-D/E-gated evidence rows (W-7b, W-9, W-10b,
W-11b — 140 reserved fault keys) become writable only as their conversion
packages land, under the §4 accretive-evidence rule.

## 6. Summary counts

Deduped inventory: 20 rows (4 non-Windows, 11 Windows incl. splits, 9
cross-doc R2-F items, minus overlaps).

- **PROVEN (native, cited):** 9 — W-1, W-3 (classifier/alias), W-4, W-5, W-6,
  W-8, F-2, F-9, plus the catalog halves of W-7/W-10/W-11 and the
  process-fault half of F-3; P-4 additionally PROVEN on the R0-L tree
  (currency re-run owed).
- **EXISTS-UNEXECUTED:** 5 — P-1, P-2, P-3 (+F-6 folded in), P-4 re-run,
  W-3 physical half. All five clear with G-9/G-10/G-3: two workflow edits and
  three dispatches.
- **MISSING:** 5 — F-1 (destination-window), W-2 (NTFS case-sensitive), F-4
  (resume-window correction), F-5 (MAX_PATH interim test), F-7/G-8 (bypass
  inventory gate). Four of five ride this week's matrix trains.
- **BLOCKED:** 3 clusters — F-3 power-loss half (machinery: G-7 tiers, Windows
  leg rescope-recommended), the R2-D/E-gated cluster (W-7b, W-9, W-10b, W-11b),
  and F-8 (reviewed product decisions).

DRAFT — pending lane-owner adoption. Nothing here changes gate status by
itself; adoption, package ordering, and any rescope decisions belong to the
implementation lane and reviewers per `AgentProcessRules.md`.
