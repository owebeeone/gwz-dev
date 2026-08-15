# Code Review — gwz M5-8 Panic-Invariant Conversion, Packages P1-P3

**Review object:** the uncommitted delta in exactly six files of `gwz-core`, against the specification `dev-docs/GwzM5-8PanicInvariantAudit.md` §5.3/§7 packages P1-P3. Filed verbatim by the lane owner.
**Tuple:** six files at uncommitted worktree state, delta = 139 insertions / 71 deletions, delta sha256 `96a53dcb1f633f266fa6bc8f1bf4c2dab4a55359e340dcfda4720f7995374fc2`; gwz-core HEAD `43c37bcff338daf95f053eedbc7467e6c9368eff` at review start, `9939b02d2cc57db84540b9e3f903317f6c088342` at review end (one commit landed mid-review; `git diff 43c37bc..9939b02 -- <six files>` is **empty**, so the reviewed delta is byte-identical under both bases; the six files are also identical between the audit's pinned commit `a506a93` and both HEADs, so all audit line numbers map 1:1). Date 2026-08-16.

## Verdict: **GO**

All 27 audit line-items (28 individual panic-style calls) in P1-P3 are converted exactly as prescribed or better; both class-C keeps are kept; no conversion exists outside the audit's list; happy-path behavior is byte-identical everywhere statically provable, and all four gates are green with direct exit codes. Findings are P3-only.

## 1. Per-site verification (audit line = pre-delta line, verified identical at a506a93/43c37bc/9939b02)

**P1 — publication observers** (`.../observe/finalization/publication.rs`, `.../publication/live.rs`, `.../dispatcher/invocation.rs`)

| Audit site | Old form | Converted to (new location) | Verdict |
|---|---|---|---|
| publication.rs:58 | `observed.expect("exact candidate snapshot")` | let-else → `Err(reverse_handoff_error())` (new :53-55) + `require_handoff_exact` on the pinned tuple (:56-58) | OK — see hunt (d) |
| publication.rs:91 | same | same shape (new :88-96) | OK |
| publication.rs:460 | `publication.as_ref().unwrap()` in `verify_publication` | `ok_or_else(MergeRecordUnreadable, "publication verification requires publication progress")` (new :459-464) | OK |
| publication.rs:620 | same, in `recorded_evidence_is_live` | `ok_or_else(... "recorded evidence verification requires publication progress")` (new :629-634) | OK |
| live.rs:36-39 | chained double-unwrap | `let (_, candidate) = candidate_progress(current)?` (new :32); new accessor (new :37-56) with per-field MergeRecordUnreadable errors | OK — audit's exact prescription |
| live.rs:85-87 | `progress/candidate/marker_path unwrap()` ×3 in `snapshot` | `candidate_progress(current)?` + marker `ok_or_else(... "candidate publication requires the recorded marker path")` (new :99-105) | OK |
| invocation.rs:63 | BTreeMap index | `.get(member_id).ok_or_else(|| dispatch_error("preparation owner is missing from the record"))?` (new :66-70) | OK — see hunt (a) |

**P2 — reverse/rollback observers** (`.../observe/reverse/rollback.rs`)

| Audit site | Old form | Converted to | Verdict |
|---|---|---|---|
| rollback.rs:393, 449, 509 | `pending_rollback.clone().unwrap()` ×3 in Before-arms | router `observe_pending` threads `pending: &PendingRollbackActionV1` into the three observers (new :332-357); Before-arms `pending.clone()` | OK — the audit's preferred option; see hunt (b) |
| rollback.rs:464 | `next_evidence(step).unwrap()` | `.ok_or_else(|| recovery_error("evidence rollback step has no successor"))?` | OK — audit's suggested text verbatim |
| rollback.rs:524 | `next_root(step).unwrap()` | `.ok_or_else(|| recovery_error("root metadata rollback step has no successor"))?` | OK |

**P3 — v1_rollback executor blocks** (`.../merge/abort/evidence.rs`, `.../merge/root/abort.rs`)

| Audit sites | Old form | Converted to | Verdict |
|---|---|---|---|
| evidence.rs:153, 296, 394, 442 | `candidate.as_ref().unwrap()` ×4 | `candidate_v1(record)?` (new :466-473) | OK ×4 |
| evidence.rs:158, 455 | `composition_commit.as_deref().unwrap()` ×2 | `composition_commit_v1(record)?` (new :475-480) | OK ×2 |
| evidence.rs:177, 185, 331, 338, 395 | `candidate_marker_path unwrap()` ×5 | `marker_path_v1(record)?` (new :482-487) | OK ×5 |
| root/abort.rs:360, 366 | baseline `{manifest,lock}_yaml.as_deref().unwrap()` | `ok_or_else(root_metadata_error(...))` inline per arm (new :360-386), messages byte-identical to the :197-202 twin | OK — see F4 |

**Class-C keeps kept:** root/abort.rs:237 (guarded `components.last().unwrap()`); evidence.rs:64-67 (pre-existing typed check with the DISTINCT message "publication-evidence rollback has no immutable candidate" — deduping would have changed an already-typed error string; leaving it is behavior-preserving). publication.rs:699 unwrap is cfg(test). Grep: no other panic-style call remains in the six files. No out-of-list conversions; the mechanical `action`→`pending` router param rename un-shadows the `Participant{action,..}` binding; safe-listed sites correctly untouched.

## 2. The three behavior-preservation hunts

**(a) invocation.rs short-circuit split — PRESERVED under all operand orders.** The `!any` test still runs first and returns `Ok(action)` before any map access (including the member-absent-and-not-owner case, which never panicked and still doesn't error); `.get()` + `dispatch_error` fires only in the owner-present-in-list-but-absent-from-map case (exactly the old panic); remaining legs unchanged. Happy path byte-identical.

**(b) rollback.rs router threading — SAME VALUE, no aliasing/ordering change.** Sole caller `observe_cursor` (:97-98) threads a reference into `current.record().pending_rollback` — the exact field the old clone-unwrap read, on the same immutable `StoredV1Record`, cloned at the same position. No other callers exist (same-named fns elsewhere are different functions).

**(c) accessor dedupe byte-identity — VERIFIED, message AND ErrorCode.** The two pre-existing typed fetches in `v1_evidence_residue_after_selected_root_is_exact` carried exactly the strings the new accessors emit, through the same `root_error` constructor (MergeRecoveryRequired + `@root`); fetch positions preserve error ordering. root/abort.rs messages byte-identical to the :197-202 twins. `composition_commit_v1`'s message is new (no prior typed fetch), styled to siblings.

**(d) publication.rs:58/91 reachable-None path:** `snapshot()` legitimately returns `Ok(None)`; old code funneled None into `require_handoff_exact(false)` — literally `Err(reverse_handoff_error())` (:146-152) — and the new let-else returns the same error object. Byte-identical on the reachable path; arms now match the three pre-existing sibling arms exactly.

## 3. Error-idiom fidelity

All new rejections use the module-local constructors with matching precedent: inline `MergeRecordUnreadable` (matching `action_for_state` :505-509), `dispatch_error` (matching the same-file `.get()` pairing at :78-86), `recovery_error` (module helper, 5 prior uses), `root_error`/`root_metadata_error` (twins). Messages lowercase, per-site distinct (audit review-item satisfied: no collapse into a shared string).

## 4. Gates (direct exit codes)

1. `cargo check --lib --tests --quiet` → exit 0, zero warnings.
2. `cargo test --lib workspace_ops::merge::v1_lifecycle:: --quiet` → **225 passed / 0 failed** aggregate, via six disjoint foreground splits (single invocation provably exceeds the 600s shell cap; the 8 root-matrix tests alone take 569s). Partition proven complete and disjoint against the 225-name `--list` roster (110+61+54). `reverse::rollback::tests` (29/0) directly exercises the P2 conversions; zero test-file updates needed — consistent with behavior preservation.
3. `cargo test --lib checked_artifact:: --quiet` → **245 passed / 0 failed** (vs briefed 244 — see F1).
4. `cargo fmt -- --check` → exit 0.

## 5. Site recount

The audit's P1-P3 tables list **27 line-items** = **28 individual panic-style calls** (live.rs:36-39 is a chained pair). The audit §4 counts 26 convert-recommended for P1-P3 (excluding the two class-A "zero-cost cleanup" expects at publication.rs:58/91); the implementer's "25" counts the chained pair as one. The delta in fact eliminates **all 28** panic-style calls in scope. No site missed; nothing extra converted.

## 6. Findings

- **P0/P1/P2: none.**
- **F1 [P3]** — checked_artifact count drift: 245/0 observed vs 244/0 briefed; the +1 is the R2-F package's mid-review landing (43c37bc→9939b02 adds 4 checked_artifact tests), not this delta. Re-pin the expectation at land.
- **F2 [P3]** — tree drift during gating: HEAD moved mid-review; the six-file delta is proven invariant across both bases (empty base diff; delta digest pinned above). Re-validation at land rides the CI lane gate.
- **F3 [P3, cosmetic]** — `marker_path_v1` returns `&String` where `&str` (via `as_deref`) would be marginally more idiomatic; deref coercion covers every use; zero warnings.
- **F4 [P3, letter-vs-substance]** — root/abort.rs duplicates the fetch+message inline per arm rather than literally reusing the :196-202 twin fetch; constructor and messages byte-identical, evaluation stays per-arm-lazy like the old unwraps. Substance compliant.

**GO** for landing P1-P3 as reviewed, under the tuple above.
