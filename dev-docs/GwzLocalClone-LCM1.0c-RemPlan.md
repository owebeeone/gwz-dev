# GwzLocalClone-LCM1.0c — remediation plan (round 1 of at most 2)

**Date:** 2026-09-05. **Lane owner:** F51 (main session). **Status:** CLOSED —
patch landed as gwz-core `a8eae4e` (`LCM1.0c-rem1`); round-2 re-verdicts
`GwzLocalClone-LCM1.0c-ReviewCode-2.md` and `-ReviewState-2.md` both GO on
that revision; every blocking finding verified closed on its original
counterexample; four new P3s carried into lane briefs (record status block).

**Input reviews (filed verbatim):** `GwzLocalClone-LCM1.0c-ReviewCode.md`
(Fable 5.1, medium — NO-GO, 1 P2 / 5 P3, pre-commit to GO on P2-1) and
`GwzLocalClone-LCM1.0c-ReviewState.md` (Opus 5 — NO-GO, 2 P2 / 3 P3,
pre-commit to GO on P2-1 + P2-2). Prompt: `GwzLocalClone-LCM1.0cReviewPrompt.md`.

**Reviewed tuple:** gwz-dev `fc8b18966f24c14f422147d16f61a0dd19159670`;
gwz-core `52468cac728b7b5a877071187de2cb04196821ec`; gwz-cli
`86840f67e23a9ef04312df0dd7c8f781cc476e04`; gwz-py
`afcd5a396ccaf5e64884f8f1cec2f66273ac08bb`.

## 1. Merged verdict

NO-GO. Three independent blocking root causes, none in skeleton behaviour,
ordering or gates: one transport-admission defect (Code P2-1) and two false
or missing sentences in frozen contracts (State P2-1, State P2-2). Both
reviewers pre-committed to GO on a revision that resolves their blockers as
specified.

**Blind convergence (highest-confidence class):**
- The anonymous-fetch contract's `FETCH_HEAD` clause contradicts the
  implementation and nothing asserts the truth — State **P2-1** and Code
  **P3-4**, reached from opposite directions (durable receiver state vs.
  contract/document agreement). Same root cause, one disposition.
- The family-store conformance suite never exercises the pointer/marker
  operations or `StoreError::Partial` against a real store — State **P3-1**
  and Code **P3-3**. Same root cause, one disposition.

## 2. Dispositions — every finding, one disposition, one closure test

IDs are prefixed by axis: `C-` = ReviewCode, `S-` = ReviewState.

| ID | Root cause (short) | Disposition | Closure test |
|---|---|---|---|
| **C-P2-1** | `admitted_local_peer` passes a bare path to libgit2; on unix a path containing `:` selects the SSH transport (`transport.c` checks `:` before `isdir`) | **FIX.** After admission, canonicalise and hand libgit2 `url::Url::from_file_path(..)` (`file://…`), which the transport table matches before the `:` heuristic. Keep the caller-facing refusal of `file://` *inputs*. Change confined to `src/git/gitbackend/transport.rs` (unpinned); docs `GitBackend.md:58-60` say why. | `#[cfg(unix)]` case in `src/local_clone/tests/transport.rs`: source and bare hub under a path component containing `:`; `fetch_anonymous` creates the import ref with the source commit; `push_anonymous` updates the hub; `remotes()` stays empty. |
| **S-P2-1** = **C-P3-4** | `contract.rs:746-747` promises "does not update `FETCH_HEAD`"; the test comment says libgit2 wrote it; `git2 0.21` clears `UPDATE_FETCHHEAD` on `update_fetchhead(false)`, so one statement is wrong | **FIX by measurement.** Assert the truth in the existing Tier B fetch test (record `.git/FETCH_HEAD` presence + bytes/mtime before, assert after) and align all four texts to the asserted behaviour: `contract.rs:742-748`, `docs/GitBackend.md:64-68`, `dev-docs/GWZDesign.md:500`, the `tests/transport.rs:39-42` comment. If `FETCH_HEAD` is written, the contract says so and excludes it from the promise; if not, the false comment goes. | The assertion in `fetch_anonymous_imports_an_explicit_refspec_without_persisting_a_remote`; a libgit2 bump that flips the behaviour fails Tier B. |
| **S-P2-2** | `family-store-contract/src/lib.rs:252-254` "call order … in any order" licenses `apply(RemoveRow)` before `remove_pointer`, producing an orphan pointer no interface operation can remove | **FIX.** Replace the clause with the ordering the design and the fake already enforce: pointer/marker install only after the `creating` row exists; pointer/marker removal strictly before `RemoveRow`/`Disband`; a pointer whose row is gone is unreachable through this interface and must not be produced. Lane C may additionally make the reference fake refuse `RemoveRow` while a pointer it installed is still present (stronger, optional). | New `run_all` case `removing_the_row_before_the_pointer_is_refused_and_leaves_no_orphan` (found a family, install a pointer, `apply(RemoveRow{Keep})`, assert `remove_pointer` refuses and the fixture still observes the pointer), so the store never silently accepts the orphaning order. |
| **S-P3-1** = **C-P3-3** | `run_all` has six index/lock cases and no pointer/marker or `Partial` case; `StoreFixture` cannot inject a write failure | **FIX (ride-along, contract crate).** Add `fail_next(&mut self, root, StoreOperation)` to `StoreFixture`; extend `run_all` with: marker-before-pointer ordering; `Partial { completed: [MarkerWritten] }` when the pointer write fails; repeatable `remove_pointer` (second call reports no effects); `ConflictingMetadata` for a destination holding an index; `PointerTargetInvalid` for a pointer at another family. All green against `InMemoryFamilyStore`. **Do not commit a red real-store test**: wiring `run_all` through a real temp fixture is lane S's first failing test, recorded in the record §8 brief. | `cargo test -p gwz-family-store-contract --lib` green with the new cases; the §8 brief for lane S names the wiring. |
| **C-P3-2** | Copy conformance never exercises a non-empty partial report | **FIX (ride-along, contract crate).** Add a countdown-`Cancellation` case and a mid-tree `DestinationUnwritable` case (unix) asserting `partial` counts equal the destination's actual contents and the source is untouched. | `cargo test -p gwz-copy-contract --lib` green; the suite fails against a deliberately inaccurate fake (add that negative as a unit test inside the crate). |
| **C-P3-1** | `validate_family_merge` admits requests the engine's Start validation refuses (whitespace message, `policy.partial`); once lane X wires the import, a malformed start would fetch before refusing | **FIX (ride-along, unpinned).** Expose `pub(crate) fn validate_merge_start_shape(&MergeRequest)` in `merge/validate.rs` (the Start arm's `validate_common_meta` + `validate_custom_commit_message` + `reject_present` set) and call it from `validate_family_merge` on the projected request (selector cleared, placeholder import ref) before returning the selector. | `local_clone::request::tests`: both counterexamples refuse with the engine's codes; `local_clone::tests::request`: a family start with a whitespace-only message refuses and `refs/gwz/local-imports/*` stays empty. |
| **S-P3-2** | Both probe harnesses `copytree` `crates/` without an ignore filter, copying git-ignored per-crate `target/` | **FIX (ride-along, one line each).** `shutil.copytree(…, ignore=shutil.ignore_patterns("target", "Cargo.lock"))` in `test_v1_lifecycle_privacy_probe.py` and `test_check_checked_artifact_boundaries.py`. | Unit test over the copy helper: a `crates/x/target/marker` in a scratch source is absent from the copy. |
| **S-P3-3** | CI Tier A runs `--manifest-path` unlocked; once any crate declares a third-party dep (lane I: `git2`) its resolution may diverge from the product lock — false green | **DEFER to operator item §7.6 (workspace layout), with a fail-closed guard now.** Add a rule to `check_local_clone_boundaries.py`: refuse if any classified crate manifest *declares* a third-party dependency while the CI Tier A step is unlocked (detect the `--locked`-less loop in the workflow, or read a flag in the inventory). Retirement condition: Option A lands (gwz-core as its own workspace, excluded from the root as `taut-shape-rs` is) or per-crate locks are committed and `--locked` restored. | Negative fixture: a crate with a declared third-party dep + unlocked CI step ⇒ gate fails; current tree ⇒ gate passes. |
| **C-P3-5** | Plan 1.0c's hook-path preflight tests are absent and not recorded as deferred; lane I has no §8 brief | **FIX (record amendment).** Record §6: explicit deferral naming lane I with lane T's fixtures and the `LayoutHazard` outcome per fixture (`EscapingConfig` / admitted internal relative path / `UnresolvableConfig`, incl. bare and push-hook working directories); record §8: the lane I brief. | The text; lane I's TDD starts from it. |

**Record corrections in the same patch** (lane C owns the record):
§1 root row — gwz-dev is now committed at `fc8b189` (closes §7.7);
§2.5 heading — gwz-py SHA is `afcd5a3`, not `571ca97`;
§5.2 — `gwz-history-check`'s closure is `gwz-repo-contract` only;
§3 — add the round-1 review files and this plan to the evidence list.

**Residual risks (not findings) to carry into the lane briefs, no code now:**
CR must switch `gwz-cli/src/globalargs/dispatch.rs:198` to
`handle_merge_with_local_family` together with `--remote` parsing; the resolver
table needs a ready **bare** row when lane B lands; the store must canonicalise
paths before `validate_transition` (design §3.1); `local_clone/transport.rs`
`map_error` conflates every `invalid_request` into `NotLocal`; gwz-py async
`submit` accepts before shape validation (parity with push/merge); CI does not
clippy `crates/` — **optional ride-along:** add `cargo clippy -p <crate>
--all-targets -- -D warnings` to the Tier A CI loop; the `pending` inventory
state has no retirement condition (LBT-012); `.gwz/family-root` stores an
absolute `root_path` (documented same-user limitation; lane D's
`PathMismatch` must catch a moved root); `TrackingBackend`'s anonymous ports
skip peer/refspec admission (fidelity note for lane X's tests).

## 3. One patch, not a series

Lane C lands all FIX rows as **one** gwz-core commit (`LCM1.0c-rem1: …`),
plus the record edits in gwz-dev (uncommitted; the lane owner commits the
root checkpoint). No gwz-cli/gwz-py change is required. No pinned file is
touched (every change is in `transport.rs`, `merge/validate.rs`,
`local_clone/*`, `crates/*`, `scripts/checks/*`, docs, the workflow); if a
pinned file must move, stop and report. Best-effort scope is unchanged.

Gates on the patch (from `gwz-core`): fmt; `CLIPPY_CONF_DIR="$PWD" cargo
clippy --all-targets --all-features -- -D warnings`; `check_checked_artifact_boundaries.py`;
`check_local_clone_boundaries.py` + its unittest; `protocol/regen.py --check`
(unchanged protocol — must still pass); `PYTHON=python3.13
scripts/checks/check_lane_commits.sh 87207c2 HEAD`; from gwz-dev:
`cargo test -p gwz-core --lib local_clone`, `… --lib workspace_ops::merge::validate`,
`… --lib workspace_ops::tests::g01::tracking_backend`, `cargo test -p
gwz-copy-contract --lib`, `cargo test -p gwz-family-store-contract --lib`,
`cargo test -p gwz-family-store --lib`; the `--list` census (new test rows
move the remainder pin: re-pin `run_r4bg_aggregate_gates.py` darwin
executed / linux derived in the same commit, with the dated reason).
Whole-suite and 16-minute checker runs are **not** required for this patch.

## 4. Re-verdict

Single-axis re-verdicts, each axis re-checking its own counterexamples on
the corrected tuple: Code axis verifies C-P2-1 (the `:` path case) and the
ride-alongs it named; State axis verifies S-P2-1 and S-P2-2 (the
`FETCH_HEAD` assertion and the ordering clause + `run_all` case). Reports
file as `GwzLocalClone-LCM1.0c-ReviewCode-2.md` and
`GwzLocalClone-LCM1.0c-ReviewState-2.md` with the prior-finding closure
table and changed-range analysis. **Deviation noted:** this session cannot
message the round-1 reviewer agents, so the re-verdicts run as fresh
agents with the round-1 report, this plan and the diff as inputs
(legitimate round-2 inputs). GO from both on the same revision accepts
LCM1.0c and opens the W1 feature lanes. Round count after this patch: 1 of 2.
