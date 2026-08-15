# GwzM5-8 Panic-Invariant Audit (pre-A1 checklist item 5)

- **Date:** 2026-08-15
- **Audited commit:** `a506a93c68b60504d5634cc9d564546a521785d2` (2026-08-15 22:03:56 +1000, "Share delete on the publication directory recheck")
- **Repo:** `gwz-core`
- **Task:** `GwzM5-8ProgressReviewF5.md` §9 item 5 — audit (inventory + classification + conversion plan) of panic-style invariants on A1-destined production paths. This document is the audit only; no code was changed.
- **Snapshot caveat:** other agents were concurrently editing test files under `v1_lifecycle/tests/` (g15 root_preservation / reverse_preservation). Those are outside this production-only scope, but line numbers here are pinned to the HEAD above.

## 1. What "A1-destined" physically means at this commit

The v1 merge lifecycle is not merely *unreached* in production — it is **compiled out**. The entire v1 implementation sits behind `#[cfg(test)]` on module declarations, and A1 is the change that removes those gates. Any panic site inside these modules is inert today and becomes a latent production panic the day A1 lands:

| Gate | Location |
|---|---|
| `#[cfg(test)] mod v1_lifecycle;` | `src/workspace_ops/merge/mod.rs:23-24` (whole v1 kernel: dispatcher, reducers, observers, resolver, v1 store, service) |
| `#[cfg(test)] pub(crate) mod v1;` | `src/workspace_ops/merge/model/mod.rs:7-8` (v1 record model + validators) |
| `#[cfg(test)] mod v1; mod v1_candidate;` | `src/workspace_ops/merge/acceptance/mod.rs:2-5` |
| `#[cfg(test)] mod open_v0; mod unknown_fields;` | `src/workspace_ops/merge/record_wire/mod.rs:13-16` (v0→v1 upgrade adapter, unknown-field preservation) |
| `#[cfg(test)] mod v1;` | `src/workspace_ops/merge/record_wire/archive/mod.rs:18-19` |
| `#[cfg(test)] mod atomic_upgrade;` | `src/workspace_ops/merge/store/mod.rs:14-15` |
| `#[cfg(test)] mod checked_bundle;` | `src/workspace_ops/merge/preserve.rs:17-18` |
| `#[cfg(test)] pub(super) mod artifact_facts;` | `src/workspace_ops/merge/root.rs:2-3` |
| `#[cfg(test)] mod v1_rollback { … }` ×4 | `abort/evidence.rs:27`, `abort/participants.rs:83`, `abort/preflight.rs:30`, `root/abort.rs:99` (v1 rollback executors/observers) |
| `#[cfg(test)] fn decode_v1_for_r3_tests` etc. | `record_wire/decode.rs:109-110`, `record_wire/archive/mod.rs:69-70` |

The audit therefore covers two populations: **(P) production-now** code (compiled into today's binary, still on the path after A1) and **(T1) A1-gated** code (cfg(test) today, production at A1). Test-only code (`tests/` trees, `*_tests.rs`, in-file `#[cfg(test)] mod tests`, fixtures, fault injectors) is excluded, with exclusions noted in §8.

## 2. The two checklist tie-ins verified

### 2.1 `decode.rs:86`

Still present, verbatim, at this commit:

```rust
// src/workspace_ops/merge/record_wire/decode.rs:77-87 (decode_production_v0)
match classify_merge_record_header(&header, InstalledMergeRecordVersions::PRODUCTION_R3)
    .map_err(RecordDecodeError::Header)?
{
    MergeRecordDispatch::V0 => {}
    MergeRecordDispatch::V1 => unreachable!("the R3 production decoder does not install v1"),
}
```

Class **A today**: `PRODUCTION_R3` (`record_wire/header.rs:25`) installs v0 only, so the classifier cannot return `V1` — it rejects v1 headers with a typed header error instead. This site *is* the A1 gate: the A1 change must delete this arm and route `V1` into the real decoder (currently `decode_v1_for_r3_tests`, `decode.rs:110`). The archived twin has the same shape but is already typed: `archive/mod.rs:61-66` maps `V1` to a typed `unsupported(..., MergeRecordRequiredWave::A1)` rejection — that is the pattern `decode.rs:86` should have had, and the trivial pre-A1 hardening is to convert the `unreachable!` arm to the same `unsupported(...)` error so a future dispatch-table edit cannot turn it into a panic.

### 2.2 `*_for_r3_tests` / `*_for_test` inventory

**No `*_for_test`/`*_for_r3_tests`/`*_for_i2` function is compiled into the production binary today.** All 47 definitions found in non-test files across both scopes carry `#[cfg(test)]` directly or sit inside cfg(test) regions (verified per definition). "Production-visible" callers exist only *inside the T1 (A1-gated) module set*, which is why they compile. These are the names the A1 change must rename (or absorb) because they become the production implementations:

- `decode_v1_for_r3_tests` (`record_wire/decode.rs:110`) — called from `v1_lifecycle/checked.rs:73` (`StoredV1Record::from_open_bytes`, the sole production v1 record constructor), `record_wire/open_v0/upgrade.rs:5,33`, `record_wire/checked_owner.rs:166,199`.
- `decode_archived_for_r3_tests` (`record_wire/archive/mod.rs:70`) — called from `v1_lifecycle/status.rs:87`, `v1_lifecycle/authority/observe/archive.rs:87`, `record_wire/checked_owner.rs:171`.
- `decode_v0_for_r3_tests` (`record_wire/mod.rs:58`) — a thin alias of the production `decode_production_v0`; test-only naming.
- `adapt_open_v0_for_r3_tests` (`record_wire/open_v0/adapter.rs:80`) — the v0→v1 upgrade classifier, called from `prepare_upgrade` (`upgrade.rs:59`).
- `upgrade_open_v0_for_r3_tests` (`store/atomic_upgrade.rs`, re-exported `merge/mod.rs:50`).

Everything else with a `_for_test` suffix is genuinely test-only (fixture builders, fault injectors like `record_wire/location.rs:445-462`, `abort/evidence.rs:501 classify_v1_evidence_shape_for_test`, accessors in `checked_artifact`) and must **stay** cfg(test)-gated when A1 un-gates the surrounding modules — worth an explicit check in the A1 review, since four of them live *inside* the `v1_rollback` blocks and `catalog_lease` that A1 will touch.

### 2.3 Status of the sites named by review P2 item 3

| Review claim (2026-08-11 WIP) | Status at this commit |
|---|---|
| Six `let … else { unreachable!() }` + chained double-unwrap in the preservation observer | **Resolved.** `observe/reverse/preservation/**` contains zero panic-style sites today. |
| Naked unwraps in `observe/finalization/publication.rs` (459, 616, 681) | **Two remain** (now lines 460, 620); "681" is now inside `#[cfg(test)] mod tests` (669-688). Sites 58/91 are require-pinned expects (safe-local). |
| `publication/live.rs` unwraps | **Present**: 36/39 (chained double-unwrap), 85, 86, 87. |
| `observe/reverse/rollback.rs` 364-495 | **Present**: 393, 449, 464, 509, 524. |
| `.position(...).unwrap()` in `v1_write_bundle_checked` | **Resolved.** `preserve/checked_bundle.rs:114-127` `owner_index` now returns `ModelResult` via `ok_or_else(PreservationEvidenceMismatch)`. |

## 3. House error idioms (conversion targets)

Typed-rejection helpers already in the codebase, used as conversion targets below:

- v0 kernel: `unreadable(msg)` → `MergeRecordUnreadable`, `recovery(msg)` → `MergeRecoveryRequired`, `root_drift(msg)` → `MergeDrift`, and the typed accessors `progress(record)?` / `progress_mut(record)?` / `candidate(record)?` (`finalize_support.rs:150-191`); `participant_mut(record, id)?` (`continue_op/execution.rs:366`).
- v1 reducers: `rejected()` → `MergeRecoveryRequired` "v1 transition predecessor or authority mismatch" (`transition/reduce/mod.rs:491-496`).
- v1 dispatcher/observers: `dispatch_error(detail)` (`authority/dispatcher.rs:409-414`), `ambiguity(current)`, `prefix_error(...)` (`observe/reverse/rollback_prefix.rs:59`).
- v1_rollback blocks: `publication_v1(record)?` typed accessor (`abort/evidence.rs:479`).
- checked_artifact: `CheckedFsError::ambiguous(op, detail)` (e.g. `provider/mutation.rs:205-210`), typed `from_live(...)?` propagation (`provider/digests.rs:42`).

Note review P2 item 4 (error opacity): new conversions should carry **per-site messages**, not another 25 sites collapsing into `rejected()`'s single string.

## 4. Counts

Panic-style sites (unwrap/expect/panic!/unreachable!/assert!, production-classified; excludes safe-verified indexing, which is tabulated separately in §6):

| Module group | A (proven) | of which convert-recommended | B (convert) | C (local) | debug_assert (keep) | Total |
|---|---|---|---|---|---|---|
| P: merge kernel v0/shared (`workspace_ops/merge` prod) | 11 | 3 | 0 | 7 | 1 | 19 |
| P: `checked_artifact` | 31 | 0 | 2 | 3 | 8 | 44 |
| T1: `v1_lifecycle` (+ `model/v1`) | 23 | 13 | 0 | 0 | 0 | 23 |
| T1: `record_wire/open_v0` | 4 | 3 | 0 | 0 | 0 | 4 |
| T1: `abort/*`+`root/abort` `v1_rollback` blocks | 13 | 13 | 0 | 1 | 0 | 14 |
| **Totals** | **82** | **32** | **2** | **11** | **9** | **104** |

Also: 3 compile-time `const _: () = assert!(...)` (`protocol/bounds.rs:69-71`) — zero runtime risk, excluded; 2 sites in cfg(test)-gated `bootstrap/runtime/fault.rs` — test-only, excluded.

Evidence rule: every class-A row below cites its guarantor. Only two sites could not be proven from named code and default to B.

The "convert-recommended" column counts the sites in mandatory packages P1-P5 of §7. A few further A-sites (`dispatcher.rs:294,309`, `validate/lifecycle.rs:57`, `decode.rs:86`, `reduce/preservation.rs:126`) are additionally marked `A→convert` in §5 but sit in the optional hardening package P6 and are not counted in that column.

## 5. Site inventory

Legend: **A** proven-unreachable (guarantor cited) · **A→convert** proven, but the proof spans modules / duplicates an existing typed twin, so conversion is cheaper than the standing proof obligation · **B** convert (reachable or unprovable) · **C** just-checked local · **dbg** debug_assert, no release panic.

### 5.1 Production-now: merge kernel (v0/shared)

| Site | Snippet | Class | Justification / proposed conversion |
|---|---|---|---|
| `finalize.rs:368` | `record.publication.as_mut().expect("publication exists during finalization")` | **A→convert** | Sole caller `ensure_composition_commit` already ran `candidate(record)?` (:167) and `progress(record)?` (:169) before calling `record_composition` (:221); record not mutated in between. Convert: use `progress_mut(record)?` (`finalize_support.rs:157`) — the exact typed helper exists; one-line change, fn becomes `ModelResult`. |
| `plan.rs:357` | `head.branch.expect("checked above")` | **C** | Guard 7 lines up (:350 `if head.is_detached \|\| head.branch.is_none() { return Err(BranchDetachedHead) }`). Optional zero-cost rewrite: `let Some(branch) = head.branch else { return Err(...) }` folds guard and bind. |
| `abort/participants.rs:62` | `record.participants.get_mut(&target_id).unwrap().state = …` | **C** | Same key fetched with typed `ok_or_else(MergeRecordUnreadable)` at :23-28; map not mutated between (only git side effects). NLL re-borrow dance. Conversion = repeat the `ok_or_else` (2 lines, matches :23). |
| `start/execution.rs:56` | `.get_mut(&participant.target_id).expect("participant was validated before execution")` | **A** | `record.participants` is built from the same `plan.participants` list this loop iterates (`start/record.rs:46-53`); in-memory, same request. Typed `participant_mut`-style lookup is a cheap optional alignment. |
| `status/observe.rs:85` | `participant.pending_action.as_ref().expect("pending observation requires durable pending action")` | **A→convert** | Sole caller guards `if participant.pending_action.is_some()` (`observe.rs:27-29`). Cheaper than the proof: change the fn to take the checked `&MergePendingAction` (single `pub(super)` caller), deleting the expect. |
| `status/snapshot.rs:102` | `live.expect("open status acquired live facts")` | **C** | `live` is `Some` exactly for the `Open` arm, set 10 lines up (:85-92) on the same immutable `acquired.record`. |
| `status/snapshot.rs:137` | `locations.open().exact().expect("open source exists")` | **A** | `select_canonical_status_source` (:57-69) returns `Open` only when `locations.open().exact()` is `Some`; `exact()` is a pure accessor over the already-acquired snapshot (`record_wire/location.rs:74-83`), no re-read, so the second call is deterministic. |
| `status/snapshot.rs:140` | `open.expect("open source decoded")` | **A** | `open` (:118-122) is `Some` iff `locations.open().exact()` is `Some` (mapped decode); same guarantor as :137. |
| `status/snapshot.rs:144` | `locations.archived().exact().expect(…)` | **A** | Mirror of :137 for the `Archived` arm. |
| `status/snapshot.rs:148` | `archived.expect("archive source decoded")` | **A** | Mirror of :140. |
| `status/snapshot.rs:274` | `debug_assert!(!observation.interrupted_root_rollback \|\| …contains_key("@root"))` | **dbg** | Release no-op. Keep. |
| `status/snapshot.rs:327` | `prefix.expect("candidate prefix was checked")` | **C** | Same expression: `prefix.is_none() \|\|` short-circuits before the expect. |
| `continue_op/execution.rs:394` | `record.participants.get_mut(&action.target_id).unwrap()` | **A→convert** | Actions are derived from this record's own participants during continue planning; map keys never removed. But `participant_mut(record, id)?` is used 28 lines up (:366) in the same file — the unwrap is a style straggler. Convert to `participant_mut` (one line). |
| `acceptance/workspace.rs:175` | `head.branch.clone().expect("observed attached branch")` | **C** | `.filter(\|head\| … && head.branch.is_some())` (:163-168) guards the same chain. |
| `root/finalization.rs:74` | `accepted_root.publication_branch().expect("selected root is attached")` | **A** | In the `Some(participant)` arm, `accepted_root_checkout_with_observation` returned `BornAttached{target_branch}` (`acceptance/workspace.rs:145-157`), and `publication_branch()` is `Some` for `BornAttached`/`UnbornAttached` (`workspace.rs:123-131`). Conversion (`ok_or_else(unreadable)`) is one line if desired. |
| `root/finalization.rs:78` | `unselected_root_head.expect("unselected root was observed")` | **C** | `unselected_root_head = Some(…) iff root_participant.is_none()` (:39-43); this is the `else` arm of the same `root_participant` branch. |
| `root/planning.rs:32` | `head.branch.expect("checked above")` | **C** | Guard at :26-31, same shape as `plan.rs:357`. |
| `participant_semantics/status.rs:199` | `HeadRelation::Equal => unreachable!()` | **A** | Whole match sits under `if live.head_relation != HeadRelation::Equal` (:191); arm exists for exhaustiveness only. Keep. |
| `record_wire/decode.rs:86` | `MergeRecordDispatch::V1 => unreachable!(…)` | **A / A1 gate** | See §2.1. Pre-A1: convert to the archive twin's typed `unsupported(..., RequiredWave::A1)`; at A1: replaced by real v1 dispatch. |

### 5.2 Production-now: `checked_artifact`

Constant-input `expect`s (parse/validate of fixed `&'static` names and paths — input is a compile-time constant, parser is total on it; keep; a one-time unit test over the constant tables discharges them permanently):

| Sites | Pattern | Class |
|---|---|---|
| `catalog_names.rs:59`; `capability/pre_catalog/provider/completed.rs:352`; `provider/directory_mutation.rs:721`; `provider/mutation.rs:215` | `from_utf8(<fixed catalog name>)` — names are fixed byte-string constants (`catalog_names.rs:31-42`) | **A** ×4 |
| `bootstrap/managed.rs:86,113` (fixed purpose sets into validating ctor), `managed.rs:332` (fixed purpose paths) | `try_from_purposes([...consts]).expect` / `AsciiComponent::parse(const)` | **A** ×3 |
| `capability/collision.rs:242,247` | `GitPathBytes::new(<fixed name>)` | **A** ×2 |
| `namespace/mod.rs:295` | `AsciiComponent::parse(slot.name(action))` — generated from fixed grammar + hex digest, ASCII by construction (`protocol/slots.rs:188-201`) | **A** |
| `protocol/catalog_bootstrap_record.rs:81,87,292`; `protocol/infrastructure_record.rs:244`; `protocol/managed_bootstrap_record.rs:509` | `AsciiComponent::parse(<fixed slot name>)` / `decode_ascii(b"gwz-bootstrap-owner-v1")` | **A** ×5 |
| `protocol/slots.rs:93,390,425` | `.name().strip_suffix(PROTOCOL_VERSION_SUFFIX).expect` — every name in the fixed table (:75-86) carries the `-v1` suffix | **A** ×3 |

Logic/closed-enum/numeric sites:

| Site | Snippet | Class | Justification |
|---|---|---|---|
| `identity.rs:65` | `Sha256::digest(…)[..16].try_into().expect` | **A** | SHA-256 output is 32 bytes; slice+convert infallible. |
| `observation.rs:128` | `ParentState::Open{..} => unreachable!()` | **A** | Inside the `let … else` negative branch of the same pattern (:124); exhaustiveness artifact. |
| `residue.rs:232`, `residue.rs:333` | `self.open_private(true)?.expect("private directory created")` | **A** ×2 | `open_private`'s only `Ok(None)` return is guarded on `!create` (`residue.rs:69`); with `create=true`, NotFound falls through to a typed `Err`. Optional ergonomics: an `open_private_created()` wrapper. |
| `transition.rs:269` | `leaf.identity.expect("exact existing source has identity")` | **A** | :255-259 returns typed error when `identity.as_ref().is_none_or(mismatch)` — `None` cannot reach :269. |
| `bootstrap/runtime/catalog_lease.rs:253` | `requests.next().expect("prepared target has a request")` | **A** | Every `PreparedCatalogTargetV1` is built with exactly one pushed request (`catalog_lease.rs:93-99`); dedupe only merges request vectors (:229-247), never empties. |
| `bootstrap/runtime/paths.rs:251` | `unreachable!("bounded runtime file reopen loop returns on its final attempt")` | **A** | On the final attempt (`attempt + 1 < MAX_WINNER_REOPENS` false) both `Err` arms return; `Ok` returns. Loop cannot fall through while the const > 0. Keep. |
| `bootstrap/runtime/catalog_lease/target.rs:384,385` | `_runtime_dir/_locks_dir.as_ref().expect("workspace … retained")` | **A** ×2 | `acquire_final` (:308-331) constructs `(Some, Some, …)` exactly for `Workspace` root_kind and `(None, None, …)` for `GitDirectory`; `revalidate_held` matches the same `root_kind`. |
| `capability/pre_catalog/provider/interior.rs:500` | `ALL.iter().position(…).expect("slot belongs to the closed infrastructure grammar")` | **A** | `InfrastructureSlotV1::ALL` enumerates the closed enum; every variant has a position. |
| `provider/namespace.rs:140` | `u64::try_from(limit + 1).expect` | **A** | usize→u64 infallible on supported targets; `limit` is a small protocol const. |
| `protocol/slots.rs:194` | `.dynamic_parts().expect("every non-base slot has a dynamic grammar")` | **A** | Reached only via the non-`Base` catch-all (:190-191); `dynamic_parts` (:204-226) returns `Some` for every non-`Base` variant (closed enum, total). |
| `protocol/slots.rs:244`, `:408` | `from_utf8(value).expect("ASCII was checked")` | **C** ×2 | `!value.is_ascii()` returned 1-3 lines above in the same fn. |
| `protocol/managed_bootstrap_record/validation.rs:101` | `component.installed_path.as_ref().expect("complete installed tuple was checked")` | **C** | `installed == true` only from the match arm that destructured `Some(installed_path)` (:66-89). Cleaner rewrite: bind the path in the arm instead of a bool. |
| `validation.rs:105` | `path.components().last().expect("canonical path is nonempty")` | **A** | `DurablePathV1::try_new` rejects empty component vectors (`capability/path/durable.rs:110-115`); all constructors funnel through it. |

Class-B sites (the only two hard converts in production-now code):

| Site | Snippet | Class | Trigger / conversion |
|---|---|---|---|
| `bootstrap/managed/owner.rs:385` | `DurablePathV1::from_live(path).expect("canonical paths are validated against the durable envelope when constructed")` | **B** | `from_live→try_new` fails on real environmental inputs: durable encoding > 4 KiB (very deep workspace paths) or mixed filesystem support profiles across components (`durable.rs:110-133`). The claim that every `CanonicalPathIdentityV1` reaching this digest helper already round-tripped the envelope is a whole-flow property with no single guarantor to cite — and the sibling `provider/digests.rs:42` does the *same call with `?`*. Convert: propagate `CheckedFsError` (make `durable_path_bytes` and its two callers at `owner.rs:336,365` fallible). |
| `capability/pre_catalog/provider/digests.rs:116` | `retained.private_parent().expect("ready digest requires retained mutation parent")` | **B** | Depends on the provider being in the "ready" retention state — a flow property, not a cited construction invariant. The same accessor is typed 100 lines away: `provider/mutation.rs:205-210` uses `ok_or_else(CheckedFsError::ambiguous(...))`. Convert to match that twin. |

Debug-only (keep, release no-ops): `catalog/scratch.rs:39` (name-length arithmetic), `provider/retained.rs:361` (fixed names ASCII), `provider/snapshot.rs:169` (sortedness of already-built list), `namespace/backend.rs:249,289,308,344,362` (provider-binding wiring checks). 8 sites.

Compile-time (excluded from runtime inventory): `protocol/bounds.rs:69-71` `const _: () = assert!(…)` ×3.

### 5.3 A1-gated (cfg(test) today; production the day A1 lands)

This is the population review P2 item 3 targeted. All sites operate on `StoredV1Record`, whose only constructors run `validate_v1_record` (`v1_lifecycle/checked.rs:65-89` for `from_open_bytes`; the `for_test` ctor also validates). Class-A proofs below therefore chain **decoder → validator → dispatcher routing → observer**, on an immutable in-memory record. The proofs are real, but they are exactly the "invariant established in a different function" pattern the module's own philosophy converts — hence the heavy `A→convert` column.

v1_lifecycle kernel:

| Site | Snippet | Class | Justification / proposed conversion |
|---|---|---|---|
| `v1_lifecycle/status.rs:97` | `open.expect("open source decoded")` | **A** | Same `select_canonical_status_source` guarantee as §5.1 snapshot.rs:137-140 (`status/snapshot.rs:57-69` + pure `exact()`). |
| `transition/reduce/preservation.rs:126` | `Action::Backup => unreachable!()` | **A** | `require(match action { … Action::Backup => false })?` at :111-122 typed-rejects `Backup` before this match. Optional: fold name derivation into that earlier match. |
| `authority/dispatcher.rs:294` | `Completed \| Aborted \| RecoveryRequired => unreachable!("handled before state dispatch")` | **A→convert** | `next_action` early-returns for all three states at :229-241 before the state match at :255. Convert to `Err(dispatch_error("terminal or recovery state reached state dispatch"))` — zero cost, survives future reordering. |
| `authority/dispatcher.rs:309` | `V1LifecycleRequest::Status => unreachable!()` | **A→convert** | `Status` returns at :226-227; `executing` has exactly one caller (:256). Convert to `reject(…)`, same as the adjacent arms. |
| `authority/resolver/reconciliation.rs:77` | `(_, _, F::Ambiguous \| F::OperationalError) => unreachable!()` | **A** | Both facts early-return at :50-55 in the same pure table function. Keep. |
| `forward/execute.rs:52` | `PreparedIntegrationAction::ResolveConflict(_) => unreachable!()` | **A** | Outer match arm :28 captures every `ResolveConflict`; the `value` binder cannot hold one. Keep (or map inside the first match). |
| `store/rewrite.rs:85` | `path.parent().expect("open record has a parent")` | **A** | `from_open_bytes` rejects any record whose `path.parent() != Some(root/.gwz/merge)` (`checked.rs:66-72`); the store rewrites only that path. |
| `model/v1/validate/lifecycle.rs:57` | `OperationState::RecoveryRequired => unreachable!("recovery state was resolved above")` | **A** | `effective_state` (:134-148) never returns `RecoveryRequired`: non-recovery states pass through, recovery maps through `RecoveryOriginStateV1` (six non-recovery values). Optional hardening: `return Err(lifecycle_error(record))`. |

Reverse observers (the review's core list — all convert):

| Site | Snippet | Class | Justification / proposed conversion |
|---|---|---|---|
| `observe/reverse/rollback.rs:393,449,509` | `current.record().pending_rollback.clone().unwrap()` ×3 | **A→convert** | Reached only via the rollback router, which matched `Some(action)` on the same immutable record (`rollback.rs:97`; dispatcher gate `dispatcher.rs:245-246`). Convert with the sibling's exact idiom: `rollback_prefix.rs:56-59` does the same derivation with `.ok_or_else(\|\| prefix_error("rollback recovery has no exact pending action"))`. Better: pass the matched action down from the router. |
| `rollback.rs:464` | `next_evidence(step).unwrap()` | **A→convert** | `step == Complete` is captured by the arm at :451; `next_evidence` (:633-642) is `Some` for every non-`Complete` step. Convert: `ok_or_else(recovery-style "evidence rollback step has no successor")` — one line. |
| `rollback.rs:524` | `next_root(step).unwrap()` | **A→convert** | Same shape: :511 captures `Complete`; `next_root` (:644-650) total on the rest. |
| `observe/finalization/publication.rs:58, 91` | `observed.expect("exact candidate snapshot")` ×2 | **A** | `require_handoff_exact(observed == Some((Baseline, Pre)))?` one line above pins `observed`. Zero-cost cleanup: use the pinned constant (or `let Some(...)` after the require). |
| `publication.rs:460` | `current.record().publication.as_ref().unwrap()` (verify_publication) | **A→convert** | Sole callers :205-206 sit inside `match progress.step` where `progress` was destructured `Some` at :181. Convert: `ok_or_else` with a per-site message (align with P2 item 4). |
| `publication.rs:620` | same, in `recorded_evidence_is_live` | **A→convert** | All callers are in candidate-bearing flows below the :181 destructure; for validated records, steps ≥ `CommittingEvidence` force `candidate.is_some()` (`validate/publication.rs:34-64` — candidate-absent records only admit steps `NotStarted/ValidatingResults/PreparingCandidate/Complete`). Convert as :460. |
| `publication/live.rs:36-39` | `.publication.as_ref().unwrap().candidate.as_ref().unwrap()` | **A→convert** | The review's chained double-unwrap. Callers (`publication.rs:100,123,258,268,437,466,513`) all sit in step arms where the validator forces candidate presence (same citation as :620). Convert to a small typed accessor (`candidate_progress(current)?`). |
| `live.rs:85,86,87` | `publication…unwrap()`, `candidate…unwrap()`, `candidate_marker_path…unwrap()` (snapshot) | **A→convert** ×3 | publication/candidate: as above. marker path: for any validated record with a candidate, `validate_candidate_progress` → `validate_candidate_semantics_for_v1` (`acceptance/publication.rs:174-199`) requires `candidate_marker_path` to equal the derived marker path exactly. 8 call sites inherit the proof — convert once in `snapshot()`. |
| `authority/dispatcher/invocation.rs:63` | `current.record().participants[member_id]` (BTreeMap index) | **A→convert** | `member_id` originates from `selected_targets` (dispatcher :262-272) and the v1 validator enforces participants ≡ selected targets (`validate/common.rs:67-71, 85-93`). Still the harshest panic form on disk-derived data: convert to `.get(member_id).ok_or_else(dispatch_error)?`. |

open_v0 upgrade adapter (A1-destined):

| Site | Snippet | Class | Justification / proposed conversion |
|---|---|---|---|
| `open_v0/adapter.rs:56` | `.get(&selected_targets[0]).expect("v0 structural validation requires every selected participant")` | **A→convert** | `adapt_open_v0_for_r3_tests` runs `validate_v0_structure` (:97) before `classify_open_v0` (:98); structural runs `validate_common_v0_view` (`structural.rs:16`) which enforces every selected target has a participant (`validate/common.rs:67-71`). Foreign-bytes path — convert to `ok_or_else(unreadable)` anyway (one line). |
| `open_v0/descriptor.rs:139` | `&record.participants[&record.selected_targets[0]]` (map index) | **A→convert** | `verified_v0_descriptor` runs `validate_record_evidence` (:61) before `validate_live_evidence` (:62); the former fetches the same key typed (`.ok_or_else(unreadable "selected participant is missing")`, :83-86). Convert: mirror the precursor. |
| `open_v0/descriptor.rs:140` | `participant.resulting_commit.as_deref().unwrap()` | **A→convert** | Same precursor binds `result` typed at :100-103 (`acceptance_drift "selected participant result is missing"`). Convert: mirror it. |
| `open_v0/descriptor.rs:464` | `serde_yaml::to_value(input).expect("compatibility descriptor must serialize")` | **A** | Inputs are plain structs/string-keyed maps built in this file; `to_value` on them cannot fail. Keep. |

v1_rollback blocks (cfg(test) executors/observers that A1 un-gates):

| Site | Snippet | Class | Justification / proposed conversion |
|---|---|---|---|
| `abort/evidence.rs:153, 296, 394, 442` | `publication.candidate.as_ref().unwrap()` ×4 | **A→convert** | Any validated record carrying a `PublicationEvidence` pending rollback has candidate, composition commit/tree, and hashes present (`validate/journal.rs:296-326`). `publication_v1(record)?` already exists in this block (:479) — add sibling typed accessors (`candidate_v1`, `composition_commit_v1`, `marker_path_v1`) and use them at all eleven sites. |
| `abort/evidence.rs:158, 455` | `publication.composition_commit.as_deref().unwrap()` ×2 | **A→convert** | Same guarantor (`journal.rs:303-307`). |
| `abort/evidence.rs:177, 185, 331, 338, 395` | `publication.candidate_marker_path.as_deref().unwrap()` ×5 | **A→convert** | Candidate present ⟹ marker path exact (`acceptance/publication.rs:174-199`). |
| `root/abort.rs:237` | `components.last().unwrap()` | **C** | Same fn rejects empty/absolute/non-Normal paths at :218-227; callers pass fixed `WORKSPACE_MANIFEST`/`LOCK_PATH`. |
| `root/abort.rs:360, 366` | `record.baseline.{manifest,lock}_yaml.as_deref().unwrap()` ×2 | **A→convert** | `SelectedRootMetadata` pending rollback requires both baseline blobs present **and** digest-matching (`validate/journal.rs:344-353`). The same file already fetches these typed 160 lines up (`root/abort.rs:196-202` `ok_or_else(root_metadata_error)`) — reuse that. |

`abort/participants.rs` and `abort/preflight.rs` v1_rollback blocks: **zero panic-style sites** (already fully typed).

## 6. Load-bearing indexing/slicing review

All flagged indexing/slicing on production or A1-destined paths was checked; none is panic-capable under its cited guard:

| Site(s) | Why safe |
|---|---|
| `record_wire/scalar.rs:122` `value[1..]` | `value.len() > 1 && value.starts_with('0')` guard; byte 0 is single-byte, boundary valid. |
| `record_wire/archive/v0_evidence.rs:191`; `provider/snapshot.rs:172-173`; `bootstrap/managed.rs:142` | `windows(2)` yields exactly-2 slices. |
| `open_v0/adapter.rs:47,55`, `descriptor.rs:75,85,221`; `bootstrap/managed.rs:164` | `[0]` behind short-circuit `len != 1 \|\| …` (or post-`len==1` flow). |
| `start/prepared.rs:63`, `start/execution.rs:27,63`, `continue_op/coordinator.rs:94`, `reduce/participant.rs:287`, `invocation.rs:73`, `observe/forward.rs:579`, `resolver/execution.rs:160`, `preservation/cursor.rs:53`, `store/unknown.rs:200` | `[i+1..]` / `[start..]` where the index comes from `enumerate()`/`position()` (< len) or `map_or(len)`; `v[n..]` is valid for n ≤ len. |
| `reduce/preservation.rs:263` `rows[0]` | `require(rows.len() == 1)?` at :262. |
| `model/v1/validate/preservation.rs:273` | `(rows.len() == 1).then(\|\| &rows[0])`. |
| `preserve/artifacts.rs:53` | `.get(1).ok_or_else(…)?` at :41-43 proves len ≥ 2. |
| `preserve/artifacts.rs:393` | is_empty→push then `len != 1 → Err` at :379-392. |
| `preserve/checked_bundle.rs:26,27,71,72` | `owner_index` is typed (`ok_or_else`, :114-127) and `position() < plans.len()` bounds `[..=index]`. |
| `record_wire/unknown_fields/overlay.rs:117` | `matched_index` recorded during enumerate. |
| `checked_artifact/authority.rs:281` | `take(1)` returns an exactly-1 slice (`split_at_checked`). |
| `checked_artifact/identity.rs:63, 289` | 32-byte SHA output; `handle_length ≤ MAX_HANDLE_BYTES` checked :278-284 against a `MAX_HANDLE_BYTES` buffer. |
| `bootstrap/managed/owner.rs:122, 270, 276-281` | `retained_existing_parent_count > spec.components().len()` rejected at :103-108 before all slices. |
| `coordinator/identity.rs:508` | `starts_with(prefix)` guard; prefix boundary is a char boundary. |
| `catalog/enumeration.rs:303, 335, 354` | fixed non-empty prefix const; `bytes.len() > buf.len() → None` guard; `len` maintained by ctor. |
| `catalog/scratch.rs:29-38` fixed-buffer writes | offsets are compile-time-consistent with `CATALOG_SCRATCH_NAME_BYTES_V1` (debug_assert_eq at :39 double-checks). |

## 7. Conversion plan (ranked, commit-sized packages; <500 LOC aspirational each)

Ordering: packages 1-3 are the pre-A1 blockers (they de-panic the code A1 activates); 4-5 are cheap alignment in already-production code; 6 is optional hardening. Each package should use **per-site error messages** (review P2 item 4), not the shared `rejected()` string.

1. **P1 — v1 finalization/publication observers** (~120 LOC + test updates).
   `publication.rs:460,620` → `ok_or_else`; `live.rs:36-39,85-87` → one typed `candidate_progress(current)?` accessor used by `publication_resolution`/`snapshot`; `publication.rs:58,91` → use the require-pinned value; `invocation.rs:63` → `.get().ok_or_else(dispatch_error)`. Tests: dispatcher/publication matrices may assert specific errors; expect small updates only.
2. **P2 — v1 reverse/rollback observers** (~80 LOC).
   `rollback.rs:393,449,509` → router passes the matched `PendingRollbackActionV1` down (or `ok_or_else` per `rollback_prefix.rs:56-59`); `rollback.rs:464,524` → `ok_or_else` on `next_evidence`/`next_root`.
3. **P3 — v1_rollback executor blocks** (~100 LOC).
   Add typed siblings to `publication_v1` in `abort/evidence.rs` (`candidate_v1`, `composition_commit_v1`, `marker_path_v1`) and use them at the 11 evidence sites; `root/abort.rs:360,366` → reuse the typed fetch at `root/abort.rs:196-202`. Blocks are cfg(test) today, so this lands with zero production risk and de-panics them before A1 un-gates.
4. **P4 — v0 kernel idiom alignment** (~60 LOC; production-now, low risk).
   `finalize.rs:368` → `progress_mut(record)?`; `continue_op/execution.rs:394` → `participant_mut(...)?`; `status/observe.rs:85` → pass checked value; `start/execution.rs:56` → typed lookup; `open_v0/adapter.rs:56`, `descriptor.rs:139-140` → mirror their typed precursors.
5. **P5 — checked_artifact class-B pair** (~30 LOC).
   `bootstrap/managed/owner.rs:385` → propagate `from_live` error (make `durable_path_bytes` fallible; two callers); `provider/digests.rs:116` → `ok_or_else(ambiguous)` per the `mutation.rs:205` twin.
6. **P6 (optional) — exhaustiveness-arm hardening** (~20 LOC).
   `dispatcher.rs:294,309` → `dispatch_error`/`reject`; `validate/lifecycle.rs:57` → `lifecycle_error(record)`; `decode.rs:86` → archive-style typed `unsupported(..., A1)` until A1 replaces it. Also fold `reduce/preservation.rs:126` into its :111-122 require.
7. **A1 change itself (not this plan, but tied by checklist):** remove the §1 cfg gates; rename `decode_v1_for_r3_tests` / `decode_archived_for_r3_tests` / `decode_v0_for_r3_tests` / `adapt_open_v0_for_r3_tests` / `upgrade_open_v0_for_r3_tests` to production names; verify every genuinely-test-only `*_for_test` helper (fault injectors, fixtures — including the four inside `v1_rollback`/`catalog_lease`) retains its own `#[cfg(test)]` once the surrounding module gate disappears; delete `decode.rs:86` in favour of real v1 dispatch. Packages 1-3 should land **before** this flip.

Everything not named in packages 1-6 is recommended **keep as-is**: 25 constant-input expects, the closed-enum/numeric/loop proofs, the 10 class-C just-checked sites, and all 9 debug_asserts (release no-ops used correctly).

## 8. What this audit did NOT cover

- **Out-of-scope modules:** `src/workspace_ops` outside `merge/` (handle_*, pull_head_*, push_member, stage_*, branch_switch, workspace_bootstrap, …), `src/git`, `src/stash`, `src/artifact`, `src/runtime`, `src/protocol`, `src/operation`, `src/model` root, `src/status`, `src/workspace`, `src/durable_fs`, `src/cbor`, `src/diff`. Several are on user-facing production paths today but are not A1-destined merge-lifecycle code; a follow-up sweep there is cheap with the same grep set.
- **Test code**, per scope: `tests/` trees, `*_tests.rs`, in-file `#[cfg(test)] mod tests`, fixtures and fault injectors (`bootstrap/runtime/fault.rs`, `record_wire/location.rs` fault hooks, `marker.rs:322+`, `abort/evidence.rs:501-535 classify_v1_evidence_shape_for_test`, `identity.rs:663-676`), and the cfg(test)-only unknown-field extraction arms inside `decode.rs`.
- **Panic classes not systematically swept:** arithmetic overflow/underflow (debug-profile panics), allocation failure aborts, stack exhaustion via recursion, `RefCell` borrows, explicit `[expr]` indexing beyond the load-bearing set in §6 (the sweep targeted literal-index/slice/position patterns; a clippy `indexing_slicing` lint run would close the tail), and `format!`/serde serialization infallibility beyond the sites listed.
- **Proof depth:** class-A citations were verified by reading the cited guarantor and the call sites named, not by exhaustive whole-program caller enumeration. The riskiest chains (StoredV1Record validator invariants) were verified at all three links (constructor `checked.rs:65-89`, validators `validate/{common,publication,journal}.rs`, acceptance marker equality `acceptance/publication.rs:174-199`), but any *future* caller of `snapshot()`/`publication_resolution()`/the rollback observers inherits those proof obligations — which is precisely why packages 1-3 convert them.
- `authority/resolver.rs` execution bodies and the forward-observer beyond the sites listed were grep-swept, not line-by-line read (they contained no panic-style hits).
