# Gwz over-claiming commit — diagnosis and resolution

- **Date:** 2026-09-01
- **Trigger incident:** workspace `datascad`, gwz 0.12.1 (`~/.cargo/bin/gwz`).
  `gwz commit --target mem_garns -m "<msg>"` committed the member correctly AND
  created a root-repo commit (207ef16, since rewritten away) in the same second
  with the same message, the same `GWZ-Commit-ID`/`GWZ-Workspace-ID` trailers,
  and 1,464 files — ~1,341 of which were untracked and never staged by any
  command (`-a` was not passed; `gwz add -A --target @root` ran only later).
- **Status:** root-caused (5 distinct defects), fixed on gwz-core/gwz-cli main
  working trees (see §5), unreleased. **0.12.1 and every prior release remain
  affected** — see the per-defect history in §4.

## 1. Reproduction

Reproduced deterministically in throwaway workspaces (root repo with 3 staged +
25 untracked files; zero-commit member `garns` with 8 files), against both the
installed 0.12.1 release binary and a source build of gwz-dev main — identical
behavior. Minimal sequence:

```sh
gwz repo add garns                    # auto-stages gwz.yml/gwz.lock.yml/conf-integrity.yml in root
gwz add -A --target mem_garns         # BUG: also stages all 25 untracked ROOT files
gwz commit --target mem_garns -m msg  # BUG: also commits the ROOT (whole index), silently
```

Variant matrix (0.12.1):

| Variant | Root committed? | Member committed? | Untracked root files swept? |
|---|---|---|---|
| `add -A --target mem_garns` + `commit --target mem_garns` | YES | YES | **YES (25/25)** |
| `commit` (no target, member staged via plain git) | YES | YES | no |
| `commit --target @root` | YES | no (correct) | no |
| root index empty, `commit --target mem_garns` | YES (lock+marker) | YES | no |
| `--dry-run commit --target mem_garns` | **YES (real commit)** | **YES (real commit)** | no |

Key discriminating facts:

- `gwz commit` itself never stages anything — every root commit contained
  exactly the pre-existing index. The untracked-file sweep was performed by
  `gwz add`, which staged workspace-wide despite `--target mem_garns`.
- The root commit is **never announced**: human output lists only member rows,
  `--json` omits the root entirely (`workspace_git_status: null`).
- A probe build showed the CLI delivering
  `Selection { targets: ["@all", "mem_garns"] }` for `add -A --target mem_garns`
  — the `"@all"` token is injected by the CLI, not typed by the user.

## 2. Answers to the incident questions

**(a) Does `--target <member>` scope `commit` at all?** It scopes the *member*
set (`--target @root` correctly excludes members) but the root is committed
unconditionally whenever commit markers are enabled — which is the default
(`handle_commit.rs:69`). The gate was
`if marker.is_some() || root_has_changes` (`handle_commit.rs:219`).

**(b) How did untracked root files enter the commit?** They were staged —
silently — by the preceding `gwz add -A --target mem_garns`, because a clap
argument-id collision turned `-A` into the global `@all` selector (§3, D1).
`commit` then faithfully committed the poisoned index. No commit path stages
untracked files implicitly.

**(c) Intended or regression?** Mixed, per defect — see §4. The unconditional
root commit was introduced deliberately with commit markers in v0.8.0 and is
documented as intentional in `gwz-core/dev-docs/GwzCommitMarker.md:276-279`,
but it shipped against a contradictory user-facing contract
(`gwz-cli/docs/commands/commit.md` — "Commit only selected members"), is
unannounced in output, and combines with `repo add`'s auto-staging to guarantee
surprise root commits. The workspace owner has ruled the semantics: **a commit
whose selection excludes the root must not create a root commit.**

## 3. The five defects

### D1 — CLI: `-A`/`-a` collide with the global `--all` selector (the sweep's first cause)

The global selector `--all` (`gwz-cli/src/globalargs/parser.rs`, id `all`,
long-only, `global = true`) shares its clap **id** with `StageArgs.all`
(`-A/--all`, `clirequest/repo.rs`) and `CommitArgs.all` (`-a/--all`). Clap
merges same-id args across the global/subcommand boundary, so any
`gwz add -A …` or `gwz commit -a …` also reads as `global.all = true`, and
`Cli::selection()` (`clirequest/invocation.rs`) injects an `"@all"` token into
`Selection.targets`. `@all` expands to root + every member
(`target_selection.rs`), swallowing the explicit `--target`. The core `-A`
narrowing branch (`handle_stage.rs:47`) is correct and covered by a passing
test (`g15.rs: all_with_member_selection_scopes_to_selected_member`) — it was
simply never given a clean selection by the CLI when `-A` was used.

### D2 — core: pathspec staging ignores the selection entirely

`resolve_stage_targets` (`stage_routing.rs:28-34`) takes no selection parameter
and never has; only the `-A` branch of `handle_stage` consults the selector.
`gwz add docs --target mem_x` stages `docs` in the root; a root-territory
pathspec additionally fans out into every member (`stage_routing.rs:58-69`).
The selector is accepted and silently discarded.

### D3 — core: `commit` commits the root unconditionally (markers force it)

`handle_commit.rs:216-221`: marker-enabled commits (the default) force a root
commit so the marker artifact "is not left pending". The root commit is
`git commit -m` over the **whole index** — not pathspec-scoped to the metadata
— so it absorbs whatever the index holds. Aggravators: `gwz repo add`
auto-stages `gwz.conf` (so the root index is essentially never clean before the
next commit), and the root commit reuses the member's message + `GWZ-Commit-ID`.
Note: `gwz log` coalescing does **not** need the root commit — it groups purely
on the `GWZ-Commit-ID` message trailer (`commit_log/coalesce.rs:66-75`,
`:203-233`), and `GwzCommitMarker.md:198-204` states markers are not the
coalescing authority.

### D4 — core: the root commit is structurally unreportable

`CommitResponse` rows are a projection of `lock.members`
(`locked_member_responses`), which has no root entry; the root
`GitCommitResult` was discarded at the call site; a root-only commit returned
zero rows. `TargetKind::Root` exists and is already emitted by `gwz push`
(`push_member.rs:347-358`) — the drop was in core, not the protocol or
renderer.

### D5 — core: `--dry-run` parsed, delivered, never read (stage + commit)

`handle_stage` had zero references to `dry_run`; `handle_commit` exactly one
(the conf-integrity reconcile). Structural cause: two guard seams —
`guarded_workspace_root(…, dry_run)` (13/13 callers honor dry-run) vs
`acquire_workspace_mutation_guard` (no dry-run parameter; all 5 callers ignore
it: **stage, commit, snapshot, capture, tag**). `gwz --dry-run add`/`commit`
performed the real mutation. The docs promised otherwise
(`docs/commands/add.md`: "--dry-run plans the staging operation without
changing indexes"; global help text; `docs/RootWorkspace.md`). The same silent
no-op is exposed through gwz-py (`client.stage(..., dry_run=True)` /
`client.commit(..., dry_run=True)`).

## 4. History (regression vs longstanding, per defect)

| Defect | Verdict | Evidence |
|---|---|---|
| D1 `-A`/`-a` id collision | Longstanding — since `add` existed | `e844a02` 2026-06-23 introduced `add`; collision inherent in the arg declarations |
| D2 pathspec routing ignores selection | Longstanding — never worked | `resolve_stage_targets` has no selection param at every tag v0.9.2→v0.12.1; scoping recorded as deferred in `gwz-core/dev-docs/history/GWZAddPlan.md:8` |
| D3 unconditional root commit | **Regression in v0.8.0** | flip commit `f88150b` 2026-07-06 ("Added commit markers") replaced the `commit_root_selected` gate; last correct release **v0.7.5** — so 0.10.x/0.11.x were already affected |
| D4 root commit unannounced | Longstanding | members-only response since `60467c8` 2026-06-22 |
| D5 dry-run ignored | Longstanding — never honored in any release | `handle_commit` (`60467c8`) and `handle_stage` (`e844a02`) born ungated; global flag predates both (`2d7524a` 2026-06-16) |

Test-coverage gaps that let all five through: no test combined `--target` with
`commit` and asserted anything about the root; no test asserted any path was
*absent* from a commit tree; no test combined pathspecs with a selection; no
behavioral dry-run test existed for stage/commit (they existed for every other
verb); the only CLI selection test placed `--target` before the subcommand and
asserted plumbing only.

## 5. Resolution (landed on main working trees, this change)

Owner-ruled contract: **`gwz add`/`gwz commit` act only on the selected
targets; the root is a target like any other; no verb mutates on `--dry-run`.**

- **F1 (CLI):** `Cli::selection()` no longer treats the collided `all` id as
  the `@all` selector for `add`/`commit` — `-A`/`-a` are purely the git-style
  flags there. Other verbs keep `--all` as the selector.
- **F2 (core stage):** an explicit target selection now constrains pathspec
  routing: an explicitly-named pathspec whose owning repo is outside the
  selection is a hard `InvalidRequest` error; fan-out-derived targets outside
  the selection are silently dropped.
- **F3 (core commit):** the root is committed only when selected. Member-scoped
  commits keep the message trailers (coalescing intact), keep the lock rewrite
  + `gwz.conf` staging (capture-style, so pins stay true), but write **no
  marker artifact** and create **no root commit**. Default selection (which
  includes the root) is unchanged. This supersedes the
  `GwzCommitMarker.md:276-279` "member-only selection still requires a root
  metadata commit" design statement — see the amendment note in that doc.
- **F4 (core commit):** the root commit, when it happens, is reported as a
  `TargetKind::Root` row in the response (human + `--json`), including
  root-only commits.
- **F5 (core stage+commit):** `--dry-run` returns after validation/planning
  with zero mutation (no staging, no exclude-file write, no commits, no lock
  rewrite, no marker).

Out of scope, filed as follow-ups (§6): dry-run for snapshot/capture/tag; the
residual `--all`-before-`add`/`commit` quirk (still sets the local flag via the
id merge); marker retention/GC.

## 6. Follow-ups

1. **`--dry-run` still ignored by `snapshot`, `capture`, `tag`** — same guard
   seam (`acquire_workspace_mutation_guard` has no dry-run parameter). Same fix
   shape as F5. **Update:** landed by a parallel session on 2026-09-01
   (`handle_materialize.rs` snapshot/capture gates, `handle_tag.rs`, tests in
   g02/g16) — in that session's working-tree change, not this one.
2. **Marker accumulation:** `gwz.conf/markers/` grows one tracked YAML per
   commit with no retention/GC (334 files in gwz-dev at time of writing — 98%
   of tracked workspace metadata). Needs a retention design (the merge lane
   already designed bounded retention for its records).
3. **`gwz --all add …` / `gwz --all commit …`** still set the subcommand's
   local `-A`/`-a` flag (clap id merge is position-independent). Untangling
   requires renaming a long option; deferred.
4. **`repo add` auto-staging** of `gwz.conf` is undocumented in
   `docs/commands/`; document or narrow (`repository.rs` staged-count also
   reports 0 for directory pathspecs).
5. **Docs debt:** `docs/RootWorkspace.md` dry-run matrix; "where supported"
   hedge in `gwz-core/docs/OperationModel.md` never resolved into a support
   matrix.

## 7. Verification (2026-09-01, all green)

**Tests added (12):** gwz-core g13 —
`member_selected_commit_leaves_the_root_uncommitted`,
`root_selected_commit_skips_members_and_reports_a_root_row`,
`member_selected_commit_does_not_sweep_the_root_index`,
`dry_run_commit_mutates_nothing`, root-row asserts on the two pre-existing
fan-out tests; gwz-core g15 —
`pathspec_routing_outside_the_selection_is_rejected`,
`pathspec_routing_into_the_selected_member_stages_there_only`,
`dot_fan_out_drops_members_outside_a_root_only_selection`,
`dry_run_stage_with_all_mutates_no_index_and_no_exclude`; gwz-cli g01 —
`stage_all_flag_does_not_widen_an_explicit_target`,
`commit_all_flag_does_not_widen_an_explicit_target`,
`global_all_still_selects_every_target_for_other_verbs`.

**Suites:** gwz-core `cargo test --workspace` 1821 passed / 0 failed
(+ auxiliary targets, 0 failures); gwz-cli `cargo test` all targets 0 failures;
`cargo build -p gwz` clean; gwz-py 576 passed (run with the repo venv +
`GWZ_RUST_BIN` pointing at the fixed binary; this host's `python3.13` lacks
maturin). `cargo fmt --check` clean on every file this change touched (two
pre-existing diffs remain in the parallel session's `tests/g02.rs`).

**End-to-end (fresh fixture, independently re-run against `target/debug/gwz`):**
`add -A --target mem_garns` → root index byte-identical (6 staged / 25
untracked), member 8/8 staged. `commit --target mem_garns` → root HEAD unmoved,
member commit carries both GWZ trailers, zero marker files, lock refreshed and
staged in root. `add docs --target mem_garns` → hard error
("pathspec routes to @root which is outside the selected targets [mem_garns]").
`--dry-run add -A` + `--dry-run commit` → root/member HEAD and index md5s
unchanged. Default `commit` → root committed and reported as `@root . Ok`.

The gwz skill reference (`~/.claude/skills/gwz/SKILL.md`) was updated: the
commit WARNING now records the root cause, the 0.12.1-still-affected guidance,
and the fixed semantics.
