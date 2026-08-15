# STATE-AXIS REVIEW — "v0 forged-action resume gate" package (M5b-IF Code F-1 disposition)

**Tuple:** five files at uncommitted working-tree state on gwz-core HEAD `9939b02d2cc57db84540b9e3f903317f6c088342` ("Land the filter-policy (D1+D2) and R2-F missing-tests packages"), reviewed 2026-08-16. Filed verbatim by the lane owner.
**Review object confirmed:** `src/workspace_ops/merge/continue_op/execution.rs` (+80, the gate), `continue_op/coordinator.rs` (+1 call site), `continue_op.rs` (imports `MergeExecutionMode`, `PendingMergeExpectedResult`), `src/workspace_ops/tests/g23.rs` (+1 mod), `src/workspace_ops/tests/g23/continue_v0_gate.rs` (new, 319 lines, T-6 names frozen-exact). Other-package dirt (`abort/evidence.rs`, `root/abort.rs`, four `v1_lifecycle` files — unwrap→typed-error hardening in v1 rollback/publication paths) inspected and excluded; it intersects none of the gate's lanes.

**VERDICT: GO.** No P0–P2 findings. Three P3 observations, none blocking.

## Q1 — Placement completeness: HOLDS, program-wide

**Continue lane.** `handle_continue` (coordinator.rs:5-133) order: `WorkspaceMutatorLock::acquire` (advisory OS lock on `.gwz/locks/workspace-mutator.lock`, no record/ref/worktree effect — workspace_mutator_lock.rs:14-30) → `store.discover_open` (read/decode/validate only; store/mod.rs:81,230-259 — no writes) → `validate_open_merge_id` (pure compare, validate.rs:37-45) → **gate at coordinator.rs:18** → state match. Every side effect postdates the gate: the `Finalizing|Completed → finalize` branch (:20-34), `reconcile_pending_actions` and its adopted-outcome persist (:42; reconciliation.rs:61-66 — and Ambiguous errors out during the read-only collect phase before any adoption persists), `preflight` (:43), both `persist_operation_transition` calls (:44,98), `set_pending_action`+persist (:55-56), execution and per-member persists (:58-107), and the tail finalize (:120-131). Notably the gate also precedes the Finalizing/Completed finalize branch, so even a forged record parked in a finalizing state refuses first.

**Program-wide sweep of every production entry** (dispatch.rs:153-199 — Start, Status, Resume, Abort, Gc are the complete set):
- **Start** refuses on any open record before consuming it (start.rs:70-72, `open_operation_error`); the dry-run early return plans only, persists nothing.
- **Status** is read-only end-to-end: zero `persist_merge_record`/`write_open`/`persist_operation_transition` calls anywhere under `status/`; `reconcile_pending_action` is documented and implemented as non-writing (pending.rs:24-26). Not gating status is contract-*required* (CompatibilityContract:160-161 open read-only status projects v0; :96-98 archived `no_ff` must decode for status/GC), so the placement — continue, not the store/decoder — is the only correct one.
- **Abort/preserve** consume the forged pending action only to classify it through the same read-only observation machinery, then either refuse (Ambiguous → MergeDrift, abort/reconciliation.rs:65-81; staged-resolution ineligibility :33-47; preserve refuses pending rows outright, preserve/plan.rs:12-17) or clear/adopt and roll **back** (apply_pending_reconciliations rewrites toward Conflicted/Merged then abort restores `before`). No forward-execution call exists on the abort lane — the frozen two-parent commit cannot be executed there. See P3-1 for the literal-contract-text tension and why refusal there would be worse.
- **GC** refuses while any open record exists (gc.rs:170-175) and refuses open records by id (:214-218).
- **Finalize** is reachable only from continue (post-gate, coordinator.rs:21,123) and start's own fresh-record flow (start.rs:104); marker conversion refuses any record carrying a pending action (marker.rs:115-122).

Conclusion: no production entry can execute or forward-persist the forged shape ahead of the gate. "Before resume/mutation" holds on the resume lane exactly as adjudicated; the abort lane mutates only in the reverse/destructive direction (P3-1).

## Q2 — Kind-row soundness: HOLDS

`merge_analysis` (git/gitbackend/merge_prepared.rs:339-362) resolves the **branch ref tip** (not HEAD), classifies via `classify_merge` (merge_support.rs:4-30: UpToDate ⟺ tip==source or tip descends source; FastForward ⟺ source strictly descends tip; TrueMerge ⟺ merge base exists; error otherwise), and is read-only (pinned by g12 `merge_analysis_classifies_without_mutating_the_repository`). The executable NotStarted resume lane (pending.rs:117-133 → `validate_prepared_merge_upstream_in_repo`, merge_support.rs:79-153) requires tip==before (`validate_checked_merge_head` with expected=before, plus `live_commit==before`) and the same `classify_merge` verdict — so the gate's refusal condition (tip==before ∧ FastForward) is exactly necessary for that lane, and it refuses a strict superset of the executable shapes (detached-HEAD/dirty/tree-mismatch variants included), every element of which is forgery-only. No legitimate v0 writer can produce the durable (TrueMerge, Commit) shape over an ff-able pair: freezing requires TrueMerge classification at the same tip==before, and ancestry between fixed OIDs is immutable.

Interleavings, each verified in code with fail direction and code:
- **Tip moved after forge:** gate silent → reconciliation `TrueMergeCommit` arm fails both NotStarted and adoption checks → Ambiguous → `MergeRecoveryRequired` ("pending merge action is ambiguous: live repository does not exactly match a pending-action recovery point"), erroring in the collect phase before any persist. (Sub-case — forger moved tip to a hand-crafted matching commit: field-wise adoption marks Merged; pre-existing F-6a semantics, no gwz Git mutation, out of package scope.)
- **Tip==before, analysis errors** (repo missing / source missing / no merge base): gate's `let Ok … else continue` skips → reconciliation refuses — repo missing → Ambiguous "recorded participant repository is missing" (pending.rs:33-39); missing objects → `clean` false → Ambiguous; validate-arm errors → `.is_ok()` false → Ambiguous. All `MergeRecoveryRequired`, all pre-write.
- **Triple ≠ participant row:** gate deliberately skips (execution.rs:53-57) → `decode_for_participant` (integration.rs:175-191) rejects the full intent quad (branch, before, source, message) with `INTENT_MISMATCH` → Ambiguous → `MergeRecoveryRequired`. Verified.
- **(UpToDate, Commit)** not gated: falls to merge_support.rs:142-144 `prepared_merge_mismatch` ("up-to-date result class changed"), fail-closed — matches the implementer's claim.
- **Error-code stability for legitimate rows:** the kind row cannot fire on any writer-producible shape (argument above); the positive control `durable_two_parent_action_over_divergent_pair_still_resumes_and_merges` proves the legal divergent durable lane executes end-to-end (two-parent commit with asserted parents/tree, lib fast-forwarded), and g23 111/0 confirms no pre-existing expectation moved.

## Q3 — Mode-row completeness: HOLDS

Full-crate grep: production consumers of v0 `record.mode` are exactly plan-time (`enforce_mode`, plan.rs:224-242, ff-only refusal at plan) and the new gate (execution.rs:23). The v1 validator's mode read (model/v1/validate/action.rs:62) is the v1 record surface, not this lane. Request-surface NoFf is refused at validate.rs:12-14 (`MergePhaseUnsupported`) before any record exists — so "no supported v0 writer produces `mode: no_ff`" is a tree fact. **FfOnly passing the gate is contract-correct:** CompatibilityContract:145 (migration eligibility excludes only `no_ff`), writer matrix :71, and validate.rs accepts ff_only requests. Nothing reads mode between record load and the gate.

## Q4 — V1 non-inheritance: VERIFIED

Grep confirmed independently: zero `continue_op` references anywhere under `v1_lifecycle/`; zero callers of `handle_continue` outside `continue_op.rs`/`dispatch.rs`; `reject_unsupported_legacy_v0` is `pub(super)`, referenced only from the coordinator. V1 executes via `v1_lifecycle/forward/execute.rs` (present), and the production store decodes v0 only pre-A1. Ran the suite: `v1_lifecycle::tests::forward` **16/16 green**, including both no-ff e2e rows.

## Q5 — Error-code fidelity: HOLDS

Sole prior constructor is the cfg-test adapter (record_wire/open_v0/adapter.rs:88-96, gated by record_wire/mod.rs:12-13); the gate's two constructors use the identical `ModelError::new(ErrorCode::UnsupportedLegacyMode, …)` pattern — mode row carries the merge id like the adapter's message; kind row adds member context via `.with_member`. Protocol mapping intact and untouched: convert.rs:58, generated.rs:1389/:1454 (`=> 47`)/:1518 (`47 =>`). No new code allocated. Both messages name the found shape and the refusal ground. The gate's doc-comment citations resolve correctly under the amendment banners with the operative rows unchanged.

## Q6 — T-6 discharge shape: HOLDS

Both renamed tests match the design §6 frozen names exactly and pin refusal-before-mutation concretely: `bytes_before` captured post-`write_open`, re-read and asserted **byte-identical** after the refusal; both members' head commits asserted at `before`/`lib_before` (refs unmoved, including the second untouched participant); `source.txt` asserted absent in the target worktree — meaningful because the fixture commits it only on `feature/source`, so any executed ff/two-parent checkout would materialize it. Error code, member attribution, and message fragments asserted. Both negatives are genuine tripwires: with the gate removed, test 1's fixture rides the exact F-1 lane to a completed two-parent commit, and test 2's plain retry rows fast-forward to completion — either would fail loudly. Residual not pinned: a dangling ODB object with refs unmoved — unobservable here and impossible anyway since the gate precedes every backend prepare/execute call.

## Q7 — Gates (all foreground, direct exits)

| Gate | Result |
|---|---|
| `cargo check --lib --quiet` | exit 0 |
| `--list` proof of the three exact renamed names | all three listed |
| the three gate tests | **3/3 green** |
| `workspace_ops::tests::g23::` | **111 passed / 0 failed** |
| `v1_lifecycle::tests::forward` | **16 passed / 0 failed**, both no-ff e2e included |
| `continue_op::` | 5/0 |
| `start::` | 37/0 |

Zero failures anywhere; no dirt attribution needed.

## Findings

- **P3-1 (observation).** Read maximally literally, CompatibilityContract:164-165's "before resume/**mutation**" is not enforced on the abort lane: abort classifies a forged row, rewrites the record toward Aborted, and restores refs without `UnsupportedLegacyMode`. Non-blocking because (i) the adjudicated §3.5a disposition scopes the discharge to "v0 continue preflight" and was ratified at round-2; (ii) abort's mutations are exclusively reverse/destructive — no forward-execution call exists on that lane, so no-ff semantics remain unreachable; (iii) gating abort would strand forged records permanently (gc refuses open records) — a strictly worse failure mode. If literalism ever matters, the artifact to touch is the contract sentence, not this code.
- **P3-2 (observation).** Inherent TOCTOU between the gate's `merge_analysis` (T1) and reconciliation's re-validation (T2): an actively racing local adversary parking the tip off `before` at T1 and restoring it before T2 slips the static gate. Unexploitable for anything they cannot already do — they hold every frozen commit input and can create the identical commit with raw git; the mutator lock does not bind raw git. F-1's actual repro (static forged record) is closed.
- **P3-3 (observation).** The gate widens `UnsupportedLegacyMode` over forged sub-shapes previously refused as `MergeRecoveryRequired` (message-mismatch, `commit_spec: None`, tree≠source, detached-HEAD-at-before — when tip==before ∧ ff-able). All affected shapes are forgery-only, both codes fail closed pre-mutation, and the new code is the contract-designated one. Recorded so the code shift is not later mistaken for drift.

Considered and dismissed: forged `ff_only` + TrueMerge/Commit over a divergent pair executes a normal-mode-shaped true merge — not no-ff semantics, producible by supported v0 anyway, ff-only enforcement is plan-time by design; forger-crafted twin-commit adoption — pre-existing F-6a field-wise adoption, no gwz Git mutation, out of scope.

**GO for landing.**
