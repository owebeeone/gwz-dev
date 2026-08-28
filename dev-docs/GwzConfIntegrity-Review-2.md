# Verification pass — conf-integrity remediation round 1 (`cf4f308`, `2508343`)

**Scope.** Round 2 of the two-round cap. Verified the round-1 GO-WITH-CONDITIONS items as discharged or not; not a fresh full review. Worktree `conf-review-worktree`, detached at each commit in turn, read-and-run only. Left at `2508343`, clean. No edits, commits, pushes, or tags.

### Per-item verdicts

| # | Condition | Verdict |
|---|---|---|
| 1 | P1-1 dry run + guard discipline | **PARTIAL** — guard half discharged, dry-run half **NOT DISCHARGED** (NF-1) |
| 2 | P1-2 garbage lock under `--force` | **DISCHARGED** |
| 3 | P2-1/P3-1 reconcile stages what it writes | **DISCHARGED** |
| 4 | P2-2 anchored deny rule + doc citation | **DISCHARGED** |
| 5 | P2-3 JSON round trip | **DISCHARGED**, residual real (NF-2) |
| 6 | P2-4 coverage test | **DISCHARGED** (NF-3 on the strength of the claim) |
| 7 | P2-5 conflict-marked manifest | **DISCHARGED** |
| 8 | P2-6 marker-tamper tests | **DISCHARGED** |
| 9 | P2-7/P3-2/P3-3 docs + dead arm | **DISCHARGED** |
| 10 | P3-6 settings staged only when written | **DISCHARGED** |
| 11 | Gates at both commits | **DISCHARGED** |

**1 [P1-1].** The type does prevent an unguarded write. `WorkspaceMutationGuard` has fully private fields (`root`, `_lock: WorkspaceMutatorLock`) and no constructor outside `src/workspace_ops/merge/runtime/mutation_guard.rs`; the only producers are `acquire_workspace_mutation_guard` (which takes the real lock) and `guarded_workspace_root`. `Some(&guard)` is therefore unforgeable elsewhere in the crate, and the lock lives for the reference's lifetime. `reconcile_authority` is `guard.filter(|_| !dry_run)`. Across 16 production call sites, 13 pass `_guard.as_ref()` and 3 pass `Some(&_guard)` where `_guard` is the non-optional return of `acquire_workspace_mutation_guard` and `root` is `_guard.root()` — no site passes a guard it does not hold.

The dry-run clause fails. Probe: `commit --dry-run` over a real hand edit returns `PermissionDenied`, marker byte-identical; porcelain gains only `?? .gwz/locks/workspace-mutator.lock` (the mutator lock, not a conf write). But `branch --create --dry-run` and `stash push --dry-run` return **Ok — no refusal**. Real runs of both refuse correctly. See NF-1.

**2 [P1-2].** `handle_update_workspace_bootstrap` now reads both documents before blessing: `if root.join(LOCK_PATH).exists() { artifact::read_lock(&root)?; }` precedes `refresh_conf_integrity_marker`. Probe: garbage lock + `--force` → `ManifestInvalid`, verdict stays `Mismatch(["gwz.conf/gwz.yml", "gwz.conf/gwz.lock.yml"])`, marker not refreshed. Formally discharged.

**3 [P2-1/P3-1].** `stage_marker` added, no-op on a non-repo. `a_committed_git_side_rewrite_is_reconciled_and_the_marker_is_staged` asserts the index column. Probe: reconcile during a real commit takes `Mismatch → Verified` with porcelain `""`. `pull_head_member_preflight.rs:450` conflicted-pull stage list is now `&[artifact::LOCK_PATH, artifact::CONF_INTEGRITY_MARKER_PATH]`.

**4 [P2-2].** Emitted rule is `Edit(/gwz.conf/**)`. I checked the citation against the live page rather than the builder's paraphrase; every clause is supported verbatim: the `/path` row ("Path relative to the settings source", `Edit(/src/**/*.ts)` → `<primary working directory>/src/**/*.ts` in project settings); the comparison row `Edit(/src/**) in any rule type | Yes | No` plus "match only at their anchored location"; and "Claude Code checks file permissions against `Edit(path)` and `Read(path)` rules only… accepts the rule but never consults it, and warns at startup" for `Write`/`MultiEdit`/`NotebookEdit`. The bare-form correction is supported too ("as a deny or ask rule, matches a `src` directory at any depth under the current directory"). The comment states layer 3 covers workspace-root sessions only.

**5 [P2-3].** Probe over the fixture cluster: `1e22` SKIPPED, `1.0` SKIPPED, `1e5` SKIPPED, and trailing-comma / leading-comment / anchors WRITTEN, re-reading correctly and idempotent on a second pass. The builder's flag is confirmed: unquoted `1e400` is **WRITTEN** as `"n": "1e400"` — number to string. `serde_yaml` resolves the overflowing literal to a string at parse time, so `root` already holds the string and the round-trip compare is trivially satisfied. **Accepted residual on behaviour** (only unquoted literals beyond f64 range, ~1.8e308+; `1e5` and `1e22` are both caught), but the claim around it is wrong — see NF-2.

**6 [P2-4].** Wildcard-free 27-arm match, `assert_eq!(commands.len(), 27)`, chain ends `MergeGc → MergeStart → None`; both are included. `conf_gate` module doc opens with "filter, not a coverage guarantee" and reasons both exclusions.

**7 [P2-5].** All 15 conf-reading gated functions are gate-before-read (the 16th call site, `handle_stash`, does not read before the gate either). Probe: conflict-marked manifest yields `PermissionDenied` with the teaching text in both real and dry runs, not `ManifestInvalid`.

**8 [P2-6].** Renamed to `a_tampered_marker_alone_is_repaired_because_the_files_match_head`, asserting the shipped repair as an accepted residual; `a_tampered_marker_with_an_uncommitted_file_still_refuses` holds. The `conf_integrity` mirror test no longer asserts `refuses()` as a safety property.

**9 [P2-7/P3-2/P3-3].** Docs describe the classify-vs-decide split. Accepted residuals names all six: hand-edit-then-commit, recomputed marker, untracked/gitignored conf and non-git roots, Forall/MergeStart, staged-`gwz.conf` dirt. Dead arm gone: `Verified | NotEnrolled | Mismatch(_) => None`.

**10 [P3-6].** `if settings.changed() && root.join(CLAUDE_SETTINGS_PATH).exists()`; `changed()` is `Created | Updated`.

### Gate table (direct exits)

| commit | fmt | check --all-targets | clippy -D warnings | cargo test --lib |
|---|---|---|---|---|
| `cf4f308` | 0 | 0 | 0 | 0 — **1677 passed, 0 failed, 1 ignored** (865.96s) |
| `2508343` | 0 | 0 | 0 | 0 — **1679 passed, 0 failed, 1 ignored** (893.79s) |

Partitions at `2508343` by `--list`: `checked_artifact::` = **446** (pin held), `workspace_ops::merge::v1_lifecycle::` less `root_fault_matrix` = **256** (pin held), `root_fault_matrix` = 1, whole lib = 1680 → remainder 977 (976 passed + 1 ignored). The builder's `446 / 976+1 / 256` is exact; the apparent off-by-one against 1679 is `root_fault_matrix`, which the partitioned scheme runs separately in release. `cf4f308` is consistent the same way.

Boundary checker: `ok (15 visible entries, 5 classified modules)`, exit 0; its unittest suite exit 0. Cumulative boundary diff `0523f0e..2508343` over `src/checked_artifact/ src/workspace_ops/merge/ scripts/ dev-docs/` is **0 bytes**, and neither commit touched those trees individually. No pin files modified. No AI-attribution trailers.

### New findings

**NF-1 — P2 — dry-run gate bypass in `handle_branch` and `handle_stash`.** Both wrap the gate in a pre-existing `if _guard.is_some()`. `guarded_workspace_root` returns `None` on a dry run, so for `BranchMutate` and `StashOp::Push` the gate never runs: `branch --create --dry-run` and `stash push --dry-run` return Ok over a real hand edit. This contradicts P1-1 as written and the shipped `conf_gate` doc ("A dry run still *refuses* on positive evidence"). `handle_tag` has the same wrapper but is safe — its else-branch uses `acquire_workspace_mutation_guard`, which is always `Some`. No corruption is possible (dry runs write nothing, real runs refuse); the harm is a false all-clear to anything that probes with `--dry-run` first. The remediation moved `read_manifest` past the gate on these exact lines without noticing. Fix is two lines, but **not** by deleting the wrapper: the command argument is hardcoded to the *Mutate* variant even on the List path, so an unconditional gate would refuse `branch --list` / `stash list` on a damaged workspace, which the design deliberately protects. Condition on the op instead — `if request.op != BranchOp::List` / `if request.op != StashOp::List`.

**NF-2 — P3 — P2-3's claim overstates the check.** `merge_deny_rules`' comment says the round-trip check turns "an unresolvable exponent arrives as a string" into a `Skipped`. It does not — that case is written, silently retyping the value. The test `every_lossy_json_shape_ends_skipped_or_verified_never_corrupted` carries the `1e400` fixture but its written branch asserts only that the file re-reads, carries the rule, and is idempotent — never that the scalar kept its type, so it passes through the corruption its own name disclaims. Either preserve the literal, or correct the comment and add a type-preservation assertion.

**NF-3 — P3 — "exhaustive by construction" is weaker than stated.** A new `OpenMergeCommand` variant forces a match arm (genuine compile error), but not chain membership: `NewVariant => None` compiles, the walk never reaches it, `len()` stays 27, and the assertion passes. The compiler forces classification, not coverage.

**NF-4 — P3, advisory — the guard is a token, not a same-root proof.** `assert_conf_unmodified_for` checks only `reconcile.is_some()`, never `guard.root() == root`. Unreachable today (every site derives `root` from the same call that produced the guard), but `root()` is available and the assertion is free.

### Verdict

**CONDITIONS DISCHARGED EXCEPT P1-1's DRY-RUN CLAUSE.** Both P1 conditions are met on their guard/lock substance, and all nine P2/P3 conditions are discharged; gates, pins, and boundary are clean at both commits. NF-1 leaves two gated commands unrefused on a dry run, which is exactly what P1-1 asserts. Because it is pre-existing and cannot corrupt a workspace, this is landable with a fix-forward if the lane owner prefers — but landing as-is means shipping a module doc that states the opposite of the behaviour. Recommendation: take the two-line op-conditioned fix (and the NF-2 comment correction) before landing; NF-3 and NF-4 are backlog.
