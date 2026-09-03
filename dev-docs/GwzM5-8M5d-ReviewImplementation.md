# GwzM5-8M5d — IMPLEMENTATION-LANE REVIEW (operator-requested, after the dual GO)

**Review object:** `dev-docs/GwzM5-8M5d-Charter.md` revision 3, accepted at
SHA-256 `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`
(the working-tree file now differs only by the acceptance banner).
**Baseline:** gwz-dev `3d0740a`; gwz-core `57502e4`; gwz-cli `dccd619`;
gwz-py `da9fb7a`.
**Date:** 2026-09-03. **Reviewer:** implementation lane (Fable), the lane
that will build it. Independent of `ReviewConsistency-3` and
`ReviewSafety-3`; both were read after my own pass. Axis: can this be built
as written, at what cost, and do the new revision-3 claims hold against the
code. Read-only; no cargo run; no files changed except this one.

**Verdict: GO-WC** — 0 P0 / 0 P1 / 2 P2 / 4 P3. Design sound. Two
one-paragraph folds before the build charter; neither is architectural.
Round-1 findings F1 (refuse before floor) and F5 (strict ignored on
ordinary starts) from `GwzM5-8M5d-Review.md` are closed by revision 3.

---

## 0. Revision-3 claims verified against the tree

| Claim | Where | Result |
|---|---|---|
| Only record **create** meets a checked door on the forward path (§3 table) | `v1_lifecycle/store/rewrite.rs:63` is the sole `checked_artifact::entry::create_merge_store_record` caller in `v1_lifecycle/store/`; `commit` is raw (record-root exception, `rewrite.rs:90-`) | **holds** |
| Copy-then-abort escape re-acquires handles on the destination (§3 b) | Catalog is workspace-internal (`.gwz/catalog-final`), so an uncatalogued merge copies without one; `CheckedArtifact::acquire` (`observation.rs:82-`) computes root and parent identity live from the destination; abort takes the plain lease (`service.rs:145`); reverse doors are `root/artifact_facts.rs:28,45,74` and `preserve/checked_bundle.rs:33-99` via `entry::*` | **holds** at the door level; nothing in `.gwz/` binds to the source volume |
| `v0.13.0` is a published, installable artifact (§2 precondition) | GitHub releases marked Latest in gwz-core, gwz-cli, gwz-py (2026-09-02); remote tags present in all three | **holds today** |
| `gwz commit` / `add` / `stage` sit behind the mutation guard (§2 occupancy) | `handle_commit.rs:37`, `handle_stage.rs:27`, `handle_materialize.rs`, `handle_tag.rs`; add-existing is `RepoMutate` in `handle_create_repo.rs`; all route through `enforce_workspace_open_merge_gate` | **holds** |
| I2 line citations in §10.1 | `GwzM5-8I2CompatibilityContract.md` 36, 83–100, 102–103, 119, 160–184, 186–204, 289, 304–305 | **all quote the cited text** |
| `write_atomic_verified` may be absent from `CAPABILITY_FREE_WRITER_TOKENS` (§7 hedge) | token list at `check_checked_artifact_boundaries.py:522-528` | **absent**; the hedge is warranted |
| Warning sentence is pinned outside gwz-core | `merge_docs_manifest.json:309` (regex), `gwz-cli/src/tests/g03.rs`, `gwz-py/src/tests/test_cli_merge.py` | pins move if the sentence changes (see L-P2-1) |

## 1. Findings

### L-P2-1 — §3 "states this limit in the same diagnostic" is not achievable where the charter puts the decision

Ship (1)'s decision (`crash_recovery_decision`) runs before any lease and
emits the warning there (`v1_lifecycle/start.rs`, `warn_once` before the
lease). Handle failure is learned later, at the create door
(`rewrite.rs:63` → `entry.rs:541`). So an implementer has exactly two
readings, and the charter forbids both: a second diagnostic at raw
fallback (§3: "No second warning", "not a second unrelated warning
class"), or a start diagnostic that asserts a limit it has not measured.
The gap enum does not settle it either: on Linux `no_durable_identity`
implies the handle fallback already failed, but `remote` and `volatile`
do not (tmpfs and NFS answer `name_to_handle_at`), and those volumes must
**not** carry the limit.

**Fold:** state that the decision gains a cheap directory-level handle
probe (the create door's own probe, applied to `.gwz` before any write),
so it carries `handles_ok` alongside the gap; the one Diagnostic event
appends the reverse-door sentence only when `handles_ok` is false; the
docs-manifest regex and the cli/py echo tests move in the same landing.
Cost: ~40 LOC in `entry.rs` plus the sentence pins. Without this
paragraph the implementer invents a decision-struct change to ship (1).

### L-P2-2 — §9's single unpublished landing over-scopes the hazard it cures

Safety P2-2's walk was the production refuse and the floor: neither may
be on `main` apart. That closure is right. Parity (2) and raw create (3)
carry no such hazard: they are additive, they leave ordinary starts on
v0, and they are measurable on `--no-ff` today (0.13.0 shipped `--no-ff`
with the empty completed response). Binding them into the close makes the
dual's object: floor flip + refuse + ~16,600 deleted lines + parity + raw
create + ~9,300 lines of re-pointed suites + checker and pin amendments,
as one review, one CI train, one revert unit. A NO-GO on any part blocks
all of it, and a post-landing regression reverts parity with the floor.

**Fold:** in §9, allow (2) and (3), and the class (ii) suite re-pointing
under a cfg(test) floor override, to land on `main` as their own reviewed
steps; keep "floor + production refuse + engine deletion + pins + docs +
CapabilityFree/F-3 revisions" as the single dual-reviewed landing. Also
say plainly that the header-only envelope classifier may land in
production early; only the refuse arm may not (the current "may exist
test-only" reads as forbidding both).

### L-P3-1 — machine form cannot express the reverse-door limit

`crash_recovery.supported = false` covers "no catalog"; the reverse-door
limit is a narrower predicate. §10.2 says this charter allocates no new
wire field, which is stricter than the operator's ship (1) ruling
(request flag, response field, diagnostic event allowed; no version
bump). Either add one optional `MergeCrashRecovery` field carrying
`handles_ok`, or state that the limit is human-only. Recommend the field.

### L-P3-2 — strike the F-3 "deliberately empty / test-only seam module" option (§4, §7)

A hollow `use store::{…}` kept in production `merge/mod.rs` so a regex can
derive a floor is a ghost seam, the shape §5 forbids for suites. After the
close there is no v0 persistence for the derivation to catch. Keep only
the redefinition: floor = the neutral raw primitive's module; property =
`v1_lifecycle/` names it nowhere and only `entry.rs` calls it.

### L-P3-3 — the existing open-operation remedy must not appear for a v0 envelope

Today's gate sentence is "merge '<id>' is open; use merge status, merge
continue, or merge abort" (`merge/start.rs:159-163`,
`operation/open_merge_gate.rs`). Under 0.14 that remedy is false for a
v0 envelope. §2's "OpenOperation plus that sentence" should add: the
existing remedy text is suppressed; the §2 sentence is the whole remedy.
The start gate at `merge/start.rs:74-80` is the sixth site.

### L-P3-4 — the product sentence names a git tag

"use tag v0.13.0" is developer wording. Users install releases; "use gwz
0.13.0 (the last release before 0.14)" with the release-notes pointer
says the same thing. Wording only.

## 2. Not findings (checked and dropped)

- Third occupancy vs read-only commands: `OpenMergeGateDecision::Allow`
  rows (ls, status, diff, branch list) stay allowed; only mutators and
  `StageConflictResolution` refuse. Consistent with §2.
- `--filesystem-strict` on ordinary starts before the close: §1 now
  states it is live only with the floor. Closes my F5.
- Archive decoder retained while §8 says no lifecycle engine: consistent
  after RemPlan-1; the serde struct for the archived body stays with it
  (~30 lines in `model/v0.rs`, which is otherwise shared vocabulary and
  is renamed, not deleted).
- CapabilityFree row: seven merge rows exist; the two GC rows stay; the
  five engine rows leave; one enters. Key-set SHA moves; amendment-tier as
  the charter says.

## 3. Sizing delta against `GwzM5-8M5d-Review.md`

Revision 3 adds: the reverse-door refusal sentence and its start-time
limit (L-P2-1 probe, ~40 LOC; sentence and pins, ~80 LOC; tests ~150);
the third-occupancy refusals at six sites (already counted, +1 site);
release notes naming `v0.13.0` as a precondition of the close (doc work,
before step 4). Net: one small package more. The material change is
review shape, not volume: under §9 as written the close is a single dual
over the whole diff; under L-P2-2 it is the floor-and-deletion diff only.

## 4. Next action

Operator decides L-P2-1 and L-P2-2 (each a paragraph in the charter);
P3s fold at will. No build, no commit, no tag from this review.
