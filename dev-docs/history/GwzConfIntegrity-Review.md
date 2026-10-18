# conf-integrity — interior review

**Train** `b23a68e` → `3aa48ab` → `0523f0e` on base `8c59521` (gwz-core, standalone lane)
**Reviewer stance** peer-blind, single-axis; evidence is the code, the commits, gates I ran, and my own probes. Builder's report not read.
**Worktree** `/private/tmp/claude-501/-Users-owebeeone-limbo/3a4f2794-661b-40df-b586-1f52221017f3/scratchpad/conf-review-worktree`, returned DETACHED at `0523f0e`, clean, no commits/pushes/tags.

---

## Verdict

**GO-WITH-CONDITIONS.**

The redesign is sound in its core judgement. The two decisions that carry it — *gate only at structural-mutation call sites*, and *refuse only on `mismatch AND uncommitted`* — are the right pair, and the merge lane is provably untouched (`src/workspace_ops/merge/` byte-identical; the pinned v1_lifecycle partition holds at exactly 256). The teaching refusal is excellent, and the acceptance path works end to end. What is wrong is at the edges the redesign created: the reconcile branch is a **write**, and it fires in places that must not write. Two conditions before landing; the rest is adjudication and record.

**Blocking conditions**

1. **[P1-1]** Suppress the reconcile write on `--dry-run` (and hold the mutation guard when it does write).
2. **[P1-2]** `gwz init --update --force` must validate `gwz.lock.yml` before blessing it, not only `gwz.yml`.

**Recommended before landing (lane owner's call)**

3. **[P2-1]** Decide what happens to the root worktree the reconcile dirties.
4. **[P2-2]** Correct the `Edit(gwz.conf/**)` anchoring rationale — the stated reason is backwards.
5. **[P2-3]** Make the settings-JSON round trip verify itself.

**Pin reconciliation** — darwin, measured at `0523f0e`: `checked_artifact::` **446** (pin 446, unchanged) · lib remainder **965 passed + 1 ignored** (pin 935 → **needs 965**) · `workspace_ops::merge::v1_lifecycle::` skip `root_fault_matrix` **256** (pin 256, unchanged). Pins themselves are untouched on this branch.

---

## Findings

| # | Grade | Axis | Title |
|---|---|---|---|
| P1-1 | **P1** | C | The gate's silent reconcile **writes the marker during a `--dry-run`**, without the mutation guard |
| P1-2 | **P1** | D | `gwz init --update --force` **enshrines an unparseable `gwz.lock.yml`** |
| P2-1 | P2 | B/C | The reconcile leaves the root worktree dirty; a later root merge then refuses `DirtyMember` |
| P2-2 | P2 | G | `Edit(gwz.conf/**)` is cwd-anchored — the stated rationale for preferring it is wrong |
| P2-3 | P2 | G | The settings JSON round trip is lossy for numbers, and "unparseable ⇒ untouched" is YAML's test, not JSON's |
| P2-4 | P2 | B/J | The gated set is a strict subset of `Block` **by convention**; `Forall` and `MergeStart` are ungated and no test proves call-site coverage |
| P2-5 | P2 | C/D | A hand edit that breaks the YAML never reaches the teaching refusal |
| P2-6 | P2 | J | `editing_the_marker_alone_refuses_both_directions` asserts a property the shipped gate does not have |
| P2-7 | P2 | C | Three clean bypasses / inert cases, all accepted residuals but none documented as such |
| P3-1 | P3 | F | `pull_head_member_preflight.rs:444` writes the lock (hence the marker) and stages only the lock |
| P3-2 | P3 | E | `conf_integrity.rs` module doc still describes the abandoned workspace-load gate |
| P3-3 | P3 | E | `ConfIntegrityVerdict::warning()`'s `Mismatch` arm is dead in production |
| P3-4 | P3 | J | `b23a68e`'s message repeats its "Gates at this tree" paragraph |
| P3-5 | P3 | D | `--force` conflates AGENTS_GWZ.md overwrite with conf acceptance |
| P3-6 | P3 | G | `.claude/settings.json` is staged even when gwz declined to touch it |
| P3-7 | P3 | I | Remainder-partition pin needs 935 → 965 |

---

## A. Boundary compliance — PASS

`git diff --stat 8c59521..0523f0e -- src/checked_artifact/ src/workspace_ops/merge/` → **empty**, exit 0. `scripts/` empty. `dev-docs/` empty. `git diff --name-only | grep -v '^src/'` → no matches (grep exit 1).

Digest-pinned files byte-identical to base:

| file | base blob | tip blob |
|---|---|---|
| `src/lib.rs` | `00a26b3b4196…` | `00a26b3b4196…` |
| `src/workspace_ops/mod.rs` | `aced6f8b5c48…` | `aced6f8b5c48…` |

`check_checked_artifact_boundaries.py` → `checked-artifact boundary: ok (15 visible entries, 5 classified modules)`, exit 0. Its suite: **69 tests, OK**, exit 0.

The digest pin on `src/workspace_ops/mod.rs` is exactly why `conf_gate.rs` and `claude_settings.rs` are submodules of `workspace_bootstrap` rather than new `workspace_ops` modules — a correct structural accommodation, not a workaround.

## B. Gate-set soundness — SOUND MECHANISM, CONVENTIONAL COVERAGE

**Conf-writing paths, all gated correctly.** Every production write of `gwz.yml`/`gwz.lock.yml` goes through `artifact::write_manifest` / `write_lock` / `write_manifest_and_lock`, and every one of those call sites sits downstream of an `assert_conf_unmodified_for` — `handle_create_repo` (3), `handle_repo_lifecycle` (3), `handle_commit`, `handle_branch`, `handle_materialize` (3), `pull_head_member_preflight` (3). The two ungated writers are correct: `handle_create_workspace` creates a new workspace, and `handle_init_from_sources` against an existing workspace only *plans* (`handle_init_from_sources.rs:43-58` returns `Accepted` without writing). No conf-writing path is ungated. **Direction (i) is clean.**

**Direction (ii) is clean too.** No gated path is one a merge/recovery/clone flow needs. The proof is two-layer: every gated call site sits *after* `guarded_workspace_root` / `acquire_workspace_mutation_guard`, and both enforce `enforce_open_merge_gate` first (`merge/runtime/mutation_guard.rs:56-72`, including the dry-run branch at line 63). So during an open merge a Block command gets `OpenOperation` before the conf gate can speak — **the merge lane's own errors are never displaced.** The gate's own `gate_decision() != Block → Ok` filter is belt-and-braces on top.

**The coupling property, and its gap.** `conf_gate.rs:8` claims the gated set is "Only those whose `OpenMergeCommand` decision is `Block`". That is true as a *filter* but not as *coverage*: the set is `{call sites} ∩ {Block}`, and the call sites are hand-added. Two Block commands have no call site:

- **`Forall`** — `grep -rn "OpenMergeCommand::Forall" src/` returns nothing outside the enum and test tables. `forall` is implemented entirely CLI-side (`gwz-cli/src/forall.rs`), where the crate-private gate cannot reach. `gwz forall` does not write conf, so it cannot enshrine an edit — but it will resolve and act on a fabricated member set.
- **`MergeStart`** — Block, dispatched at `merge/runtime/dispatch.rs:273`. Deliberately ungated (gating it means touching `merge/`), but the doc's equivalence claim over-reads.

The right property would be "every command that can reach a conf write", enforced *by construction* at the single CLI-side pre-dispatch gate that already classifies every command. As delivered it is convention, and a future Block command inherits no protection. **[P2-4]**

**Merge-lane partitions, run at the tip:**

```
cargo test --lib workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix
  → test result: ok. 256 passed; 0 failed; 0 ignored; 1413 filtered out; 399.54s   DIRECT_EXIT=0
cargo test --lib -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::
  → test result: ok. 965 passed; 0 failed; 1 ignored; 703 filtered out; 60.64s     DIRECT_EXIT=0
```

**Important correction for the lane owner.** My first full `cargo test --lib` at the tip reported `1664 passed; 4 failed` — all four in `merge::v1_lifecycle::reverse` (`root_ambiguity_matrix`, `root_fault_matrix`, `root_successor_matrix`, `rollback::service_fault_matrix`). **These were ENOSPC artifacts, not regressions.** The volume was at 165 MiB free; a probe build failed in the same window with `No space left on device`. Re-run with disk available, all three non-`root_fault_matrix` tests pass individually and inside the 256-count partition. Do not carry that first result forward.

## C. Refusal-rule soundness — CORRECT BIAS, ONE REAL DEFECT

State matrix, verified against `conf_integrity.rs:115-154` and `conf_gate.rs:29-76`, and probed end to end:

| # | marker | file vs marker | git state | verdict | gate | charter-correct? |
|---|---|---|---|---|---|---|
| 1 | absent | — | — | `NotEnrolled` | pass | ✅ grandfather |
| 2 | unreadable / conflict-marked | — | — | `MarkerUnreadable` | pass + warn | ✅ |
| 3 | future schema | — | — | `MarkerUnreadable` | pass | ✅ |
| 4 | present | match | any | `Verified` | pass | ✅ |
| 5 | present | differ | clean (== HEAD) | `Mismatch` | **reconcile** | ✅ for git; ⚠️ enshrines a *committed* hand edit |
| 6 | present | differ | ` M` worktree | `Mismatch` | **refuse** | ✅ |
| 7 | present | differ | `M ` staged | `Mismatch` | **refuse** | ✅ (probed) |
| 8 | present | differ | `UU` conflicted | `Mismatch` | refuse | ⚠️ misleading text; `read_manifest` errors first anyway |
| 9 | present | differ | `??` untracked | `Mismatch` | refuse | ✅ |
| 10 | present | differ | root not a git repo | `Mismatch` | **reconcile** | ✅ by charter; layer 2 inert |
| 11 | present | differ | `gwz.conf` gitignored | `Mismatch` | **reconcile** | ⚠️ layer 2 inert |
| 12 | rewritten consistently | match | — | `Verified` | pass | ⚠️ trivial bypass |
| 13 | tampered alone | differ (both) | files clean | `Mismatch` | **reconcile → repairs** | ✅ outcome, ❌ vs the unit test's claim |
| 14 | partial (manifest only) | — | — | per-file | pass | ✅ |

Every ambiguous cell resolves warn-or-adopt; only positive evidence refuses. **The charter's bias is faithfully implemented.**

**[P1-1] The reconcile writes during a dry run.** `assert_conf_unmodified_for` calls `artifact::refresh_conf_integrity_marker(root)` (`conf_gate.rs:49`), which is a real `write_atomic`. In `handle_create_repo`, `handle_repo_lifecycle`, `handle_materialize`, `push_member`, `pull_head_member_preflight` the gate is called *before* the `if dry_run { return … }` early return (`handle_create_repo.rs:97` vs the return at ~133). And `guarded_workspace_root`'s dry-run branch returns `None` for the guard — so the write happens **without the workspace mutation lock**. Probed through the real handler:

```
verdict     : Mismatch(["gwz.conf/gwz.lock.yml"])
porcelain   : []
DRY RUN     : Ok
marker rewritten by the dry run: YES
porcelain after dry run: [M gwz.conf/markers/conf-integrity.yml]
```

This crate already codifies the norm — `g02.rs:470 repo_sync_dry_run_plans_without_mutating_manifest` — and the builder states it verbatim one function away, at `workspace_bootstrap.rs:207-211`: *"making read-only commands … or a dry run mutate the tree would dirty a workspace nobody asked to change."* The gate is the one place that does it.
**Cure:** thread `dry_run` into `assert_conf_unmodified_for`; still refuse on positive mismatch, skip the refresh.

**[P2-1] The reconcile dirties the root.** Confirmed at git level. A merge composition commit rewrites `gwz.lock.yml` and the merge marker but *not* the conf-integrity marker (`publication.rs:243-264` — `candidate_files` is `[LOCK_PATH, candidate_marker_path]`; `commit_gwz_paths_checked` builds its tree from parent + candidates only). So after every root merge the marker is stale-but-clean → cell 5 → reconcile → the marker becomes a modified tracked file:

```
post-merge  : Mismatch: gwz.conf/gwz.lock.yml   porcelain=[]
after reconcile: Verified  porcelain=[ M gwz.conf/markers/conf-integrity.yml]
```

`merge/root/planning.rs:37` then refuses a root merge with `DirtyMember` ("has index or worktree changes"). Most gated commands call `sync_workspace_boundary` afterwards, which stages `gwz.conf` wholesale — staged is *still* dirt, but that class of dirt pre-exists this train. The genuinely new dirt is (a) every dry run, and (b) gated commands that never write conf and never sync — `gwz push` is the clear one (`push_member.rs` appears in no `sync_workspace_boundary` call site).

**[P2-5] Broken YAML bypasses the teaching.** `read_manifest` runs *before* the gate at every call site. Probed:

```
error: ManifestInvalid: failed to parse artifact YAML: invalid type: string "<<<<<<< HEAD schema", expected struct ManifestArtifact
```

The most likely agent breakage — a botched edit or a conflict-marked file — yields a raw parse error, never the teaching refusal. The teaching only fires for edits that keep the document valid.

**[P2-7] Residual holes**, all defensible, none documented as accepted:
- *hand edit + `git commit`* → silently enshrined (probed: verdict `Mismatch`, porcelain clean, gate reconciles).
- *hand edit + recomputed marker* → `Verified`; the format is plain `sha256:<hex>` of file bytes, so any agent that can run `shasum` bypasses layer 2 entirely.
- *`gwz.conf` gitignored, or root not a git repo* → `uncommitted_among` yields the empty set and the gate reconciles. Layer 2 is a **no-op** for such workspaces. (The real `gwz-dev` workspace tracks `gwz.conf`, so it is protected — but nothing enforces that.)

**TOCTOU:** negligible. The gate digests the file, then the handler acts on a manifest it read one line *earlier*; both under the mutation guard for non-dry-run paths. Nothing worth hardening. Index states behave correctly: `status_with_options` sets `include_untracked(true)`, `include_ignored(false)`, so `??` counts as uncommitted and ignored files produce no entry. Detached HEAD is irrelevant — `status` works regardless.

## D. Teaching refusal — EXCELLENT, WITH ONE DEFECT

Triggered through the real handler in a temp workspace. Verbatim:

> `PermissionDenied: hand edits detected in gwz.conf/gwz.yml: the bytes no longer match the digest gwz recorded when it last wrote the file. gwz.conf/ is machine-managed — gwz.conf/gwz.yml and gwz.conf/gwz.lock.yml must stay synchronized with the filesystem, so gwz writes them and nothing else may. Structural changes go through `gwz repo <add|clone|create|detach|attach|sync>`; there is no rename or move verb, so relocate a member with `gwz repo detach` and then re-add it at the new path. Recovery: revert the hand edit (`git checkout -- gwz.conf`), or accept the current on-disk state with `gwz init --update --force`.`

Names the exact file ✅ · machine-managedness ✅ · only real verbs ✅ (all six confirmed against `gwz-cli/src/clirequest/repo.rs`: Add, Clone, Create, Detach, Attach, Sync — no rename, no move) · both recoveries ✅ · one sanctioned acceptance path ✅. Typed `ModelError::new(ErrorCode::PermissionDenied, …)`, no panic, and the same code as the existing analogous AGENTS_GWZ.md refusal (`workspace_bootstrap.rs:290`) — idiomatic. The gate names only the *uncommitted* subset, not all drifted files, which is the honest set.

**Acceptance path, driven end to end:**

```
--- refusal ---  (as above, on `gwz repo create` after a hand "rename")
workspace unchanged? porcelain=[M gwz.conf/gwz.yml]
acceptance -> Ok: "workspace agent bootstrap files already current; accepted the current on-disk gwz.conf state as the gwz-written baseline"
verdict after : Verified
command after : Ok
member path now: repos/renamed
```

Blocked command produced nothing; acceptance unblocks; marker re-records; the hand-edited state becomes the new baseline. ✅

**[P1-2] `--force` validates only half of what it blesses.** `workspace_bootstrap.rs:68-70` claims *"checks the on-disk state still parses (so `--force` can never enshrine bytes gwz cannot read)"*, and the commit message repeats it. The path reads `artifact::read_manifest(&root)?` and **never reads the lock**. Probed:

```
manifest is garbage    -> force Err ManifestInvalid …      verdict Mismatch  (correctly refused)
manifest wrong schema  -> force Err SchemaUnsupported …    verdict Mismatch  (correctly refused)
LOCK is garbage        -> force Ok  ("accepted the current on-disk gwz.conf state as the gwz-written baseline")
                          verdict now Verified; lock parses: FALSE
```

The workspace is left "Verified" with an unreadable lock: the integrity layer reports everything is fine while every subsequent command dies on a raw YAML error. **Cure:** add `artifact::read_lock(&root)?` (guarded on the file existing) beside the manifest read.

**[P3-5]** `--force` here is `policy.destructive == Allow` — the same flag that authorises the AGENTS_GWZ.md overwrite. Anyone forcing for that reason silently accepts conf drift too. Worth a distinct flag or at least a louder note. Related: `assert_workspace_id` runs *after* the force-refresh, so a `--force` that then fails on workspace-id still leaves the marker re-blessed.

## E. Banner emission — CORRECT, WITH A VERIFIED GAP

`gwz.yml` carries the three-line banner via `ManifestArtifact::to_yaml` (`artifact/mod.rs:48-52`) — serialization is the single seam every manifest write reaches, so it rides `write_manifest` and `write_manifest_and_lock` automatically. Idempotent by construction: `from_yaml` drops comments, `to_yaml` re-adds exactly one. Pinned in `MANIFEST_GOLDEN`, and `g02.rs:790` asserts `starts_with(CONF_BANNER)` plus `read_manifest`/`read_lock` round trip on the real emitted files.

No alternate reader is harmed: `yaml_rust2` appears only in `merge/record_wire/{scalar,raw_yaml}.rs` (merge records, not conf), and the merge lane carries the manifest as `manifest_exact_yaml` verbatim (`merge/acceptance/v1.rs:100,177`) and hashes bytes read off disk (`merge/plan.rs:37`) — self-consistent either way.

**`gwz.lock.yml` deliberately carries no banner.** I verified the stated obstacle rather than taking it: `merge/acceptance/v1/support.rs:112-122 render_complete_lock` parses the accepted lock through `serde_yaml::from_str` into a `Value` and re-emits — comments cannot survive — and the result is then compared byte-wise against the on-disk baseline (`merge/abort/evidence.rs:321 lock_noop: candidate.lock_yaml == candidate.baseline_lock_yaml`; `:766` bytes-vs-baseline; `merge/finalize.rs:328` sha256-vs-`record.baseline.lock_sha256`). A banner on the lock breaks that for every no-op root merge. **The obstacle is real and correctly characterised; the gap is correctly deferred to the merge lane.** Graded: acceptable, and the lock is still digest-protected — only the human-visible warning is missing from it.

**[P3-2]** `conf_integrity.rs:7` still says the module *"refuses a workspace load whose conf bytes no longer match"* — stale from the abandoned design. **[P3-3]** `ConfIntegrityVerdict::warning()`'s `Mismatch` arm is never reached in production (`adopt_conf_integrity` only calls it for `MarkerUnreadable`).

## F. Marker design — SOUND, PROVEN AT GIT LEVEL

`gwz.conf/markers/conf-integrity.yml`; schema `gwz.conf-integrity/v0`; `files: {path: sha256:<hex>}`; two-line comment banner.

**Exclusion from commit-marker parsing is proven, not asserted:** `list_artifacts` filters `extension() == Some("yaml")` (`artifact/mod.rs:429`) and the marker is `.yml`. Test `the_marker_is_not_read_as_a_commit_marker` asserts `list_markers(root)` is empty in an enrolled workspace.

**Staging:** `sync_workspace_boundary` → `stage_workspace_git_metadata` stages the whole `gwz.conf` directory, and it follows every production conf write. `adopt_conf_integrity` stages the marker explicitly. `g02.rs:825-836` asserts index status `A` for both the marker and the settings file after create.

**End-to-end git behaviour, scripted with real git in temp repos** (not unit tests):

```
origin              : Verified
fresh clone         : Verified   porcelain=[]
after pull (feature): Verified   porcelain=[]
after branch switch : Verified   porcelain=[]
```

The core design claim holds: when git moves all three together, the digests agree and nothing refuses. Clone, pull and checkout all pass cleanly.

**[P3-1]** One staging-list gap: `pull_head_member_preflight.rs:444` calls `artifact::write_lock` (which now also writes the marker) and then stages **only** `artifact::LOCK_PATH` via `stage_paths_allowing_other_conflicts` — no `sync_workspace_boundary`. The sibling function at `:472` does sync. That leaves the marker unstaged inside a conflicted-pull path.

## G. settings.json — SYNTAX VERIFIED, ANCHORING RATIONALE WRONG, EMITTER LOSSY

**Rule syntax — I fetched the live docs and the claims check out.** From `code.claude.com/docs/en/permissions`, "Read and Edit": *"Claude Code checks file permissions against `Edit(path)` and `Read(path)` rules only. If you write a path rule for `Write`, `NotebookEdit`, `Glob`, or the legacy `MultiEdit` tool instead, Claude Code accepts the rule but never consults it, and warns at startup."* And *"`Edit` rules apply to all built-in tools that edit files."* So `Edit(gwz.conf/**)` is correct and sufficient; a `Write` rule would be inert. Omitting a `Read` deny is right — a `Read` deny would also block Edit/Write and stop agents reading state they must read. From `settings`: *"Settings files are strict JSON: a `//` comment or a trailing comma is a syntax error."* Structure (`permissions.deny` array of `Tool(specifier)` strings) confirmed.

**[P2-2] The anchoring rationale is backwards.** `claude_settings.rs:38-42` argues the bare form "beats the settings-source-anchored `/gwz.conf/**` form here, because a session may be started in a member subdirectory of the workspace." The docs say a bare relative pattern is *"Path relative to current directory"*, and for deny rules a single-segment pattern *"matches a directory named `secrets` at any depth **under** the current directory"* — with the explicit counter-case *"Does not block: `.env` in a parent directory."* A session started in `<workspace>/repos/app` has the workspace's `gwz.conf` in a **parent**, so the bare form does not match it either. Both forms fail that case; the bare form is not better for it. The rule is still a reasonable choice (it also covers nested workspaces), but the justification is wrong and, more importantly, **layer 3's protection evaporates for sessions started inside a member repo** — a routine workflow in a multi-repo workspace. Worth stating honestly, and worth considering a `//`-absolute or `~/`-anchored companion rule.

**Merge behaviour — verified by fixture.** Fresh create writes exactly `{"permissions":{"deny":["Edit(gwz.conf/**)"]}}`; re-run is `Unchanged` with no rewrite; merging into an existing file preserves every unrelated key and entry order (including tab-indented input); unparseable and wrong-shape files are left byte-identical with a `Skipped` warning routed to `response.meta.message` — and the builder correctly wired that channel through `handle_create_workspace` and `handle_init_from_sources`, which previously had no message to carry.

**[P2-3] The JSON handling has a real cluster.** I replicated `merge_deny_rules`/`push_json`/`push_json_string` verbatim against serde_yaml 0.9.34 and drove 31 fixtures:

*Value corruption:*
- `{"n": 1e400}` → emitted as the **string** `"1e400"` (libyaml does not resolve it as a float). A JSON number becomes a JSON string.
- `{"n": 1e22}` → emitted as `10000000000000000000000`, which serde_yaml then **refuses to re-parse** (`invalid type: integer … as u128`). gwz writes a file its own reader can no longer read — every later `gwz init --update` would classify its own output as unparseable and skip it forever.
- `{"n": 1.0}` → `1` (float → integer).

*The "untouched" contract is YAML's, not JSON's* — these are all invalid JSON that Claude Code would reject, yet gwz **rewrites** them: trailing comma `[],}` (accepted, silently fixed — directly contradicting the doc comment at `claude_settings.rs:123`, which names trailing commas as the unparseable case); a leading `#` comment (accepted, comment silently deleted); a `---` document separator; YAML anchors/aliases (`&x`/`*x` silently expanded); `!!str 1` → `"1"`. The `Value::Tagged` guard is largely dead, since libyaml resolves standard tags before they reach it.

*Correctly conservative:* duplicate keys (both levels) → skipped; integers > u64 → skipped; non-string keys → skipped; control chars → skipped; BOM stripped (benign); non-ASCII and emoji round trip cleanly; `deny` holding an object preserved.

**Cure (≈6 lines, closes every case):** after `push_json`, re-parse the emitted text and require it to equal the input value plus the appended rule; on any mismatch, fall back to `Skipped` with the warning. That turns a lossy transform into a verified one without adding `serde_json`.

**[P3-6]** `workspace_bootstrap.rs:205` stages `.claude/settings.json` whenever the file exists — including a pre-existing file gwz declined to touch. Ignored files are safe (`IndexAddOption::DEFAULT`), but a deliberately-untracked settings file gets staged.

## H. Template — PASS

One coherent **Workspace Integrity** section, not two (full file read; the operator's draft was merged into the existing section, not appended beside it). Honest verb list `gwz repo <add|clone|create|detach|attach|sync>` ✅ · explicit "There is no rename or move verb" with the detach-then-re-add composition ✅ · the why ("stay synchronized with the actual filesystem state") ✅ · recovery line naming both routes ✅ · detection claim — *"refuses any structural command that finds an uncommitted hand edit"* — which is **precisely true** of the delivered behaviour, including the `uncommitted` qualifier. That is unusually careful.

Consumers: no fixture pins the template body — `g00.rs:72`, `g02.rs:28/91/182` all compare against `managed_agents_gwz_contents()`, and `workspace_bootstrap.rs:238` derives it from the `include_str!` template with a digest header. So the prose edit needed no fixture change, exactly as the commit claims. `g02.rs:816-822` asserts the emitted brief carries the new text and contains neither `gwz repo rename` nor `gwz repo move`.

## I. Per-commit green — PASS

| commit | `cargo fmt --check` | `cargo check --all-targets` | `clippy -D warnings` | `cargo test --lib` |
|---|---|---|---|---|
| `b23a68e` | **0** | **0** | **0** | **0** — 1655 passed, 0 failed, 1 ignored (856s) |
| `3aa48ab` | **0** | **0** | **0** | **0** — 1663 passed, 0 failed, 1 ignored (1283s) |
| `0523f0e` | **0** | **0** | **0** | partitions below, all **0** |

Tip partition counts for pin reconciliation (darwin):

| partition | measured | current pin | action |
|---|---|---|---|
| `checked_artifact::` | **446 passed** | 446 | unchanged |
| lib remainder | **965 passed + 1 ignored** | 935 | **bump to 965** |
| `v1_lifecycle::` skip `root_fault_matrix` | **256 passed** | 256 | unchanged |

446 + 965 + 256 = 1667, plus `root_fault_matrix` (the release-profile gate, 1) = 1668 + 1 ignored — consistent across all three commits' arithmetic. Every per-commit count matches the corresponding commit message exactly. Pins verified untouched (`git diff --stat 8c59521..0523f0e -- scripts/` empty). `root_fault_matrix` was not run in release profile — it is a separate pinned gate and the volume could not hold a release target dir during this review.

## J. Test quality and hygiene — MIXED

**Good:** `g02.rs` drives all four layers through the *real handlers*, not module seams — create emits banner + marker + deny rule and stages both; a hand edit refuses `gwz repo create` with the teaching error and `--force` unblocks it; a marker-less workspace is adopted; settings merge and skip both exercised. The ambiguous-case dispositions in `conf_integrity.rs` are covered cell by cell (no marker, unreadable, future schema, absent file, partial init). `conf_gate.rs` covers uncommitted-refuses, committed-reconciles, no-marker, and not-a-git-repo.

**Gaps:**
- **[P2-4]** `only_commands_an_open_merge_blocks_are_gated` tests the *filter*, not call-site coverage, and its hand-written list enumerates 25 of the enum's 27 variants — **`MergeGc` and `MergeStart` are omitted**, so it is not exhaustive even over what it does test. Nothing proves every Block command's handler calls the gate; that is the load-bearing property.
- **[P2-6]** `editing_the_marker_alone_refuses_both_directions` comments: *"must refuse too — otherwise rewriting the marker is a silent bypass."* At the *verdict* level that is true; at the **gate** level it is false. Probed: `verdict Mismatch(["gwz.conf/gwz.yml","gwz.conf/gwz.lock.yml"])`, `gate: Ok`, `verdict after: Verified`. The gate silently repairs — a fine outcome, but the test asserts a safety property the shipped system does not have, which is worse than no test.
- The git-rewrite reconciliation is proven only by a synthetic `commit_all` in a unit test. **No test drives a real clone, pull, or branch switch** — the flows the whole redesign exists to protect. My own scripted git probes pass, so the behaviour is right; the regression net is not there.
- No test covers dry-run purity of the gate, or the `--force`/lock validation hole.

**Hygiene: clean.** Author and committer are Gianni on all three; **no `Co-Authored-By` or any AI-attribution trailer anywhere**. Per-commit diffs contain exactly what the messages claim (b23a68e: layers 1-2 + gate call sites; 3aa48ab: layer 3 + message plumbing; 0523f0e: template + battery). No stray files; nothing outside `src/`. Messages are house-style, detailed, and honest about what was measured — including that an earlier load-path design "broke 11 merge tests" and a `write_atomic` hook "broke 4 more", which is exactly the disclosure the lane wanted. **[P3-4]** `b23a68e` repeats its "Gates at this tree, direct exits: fmt/check/clippy -D warnings 0." paragraph — once truncated, once complete.

---

## On the deviation itself

Reviewed fresh, the redesign is **the better design**, not a retreat. Gating on workspace load would have put a refusal in `artifact::read_manifest` — the seam the merge lane reads through mid-rewrite — and displaced that lane's own errors; the builder measured 11 failures and backed out rather than papering over them. The `mismatch AND uncommitted` rule is a genuinely good insight: it converts "did git do this?" into a question git can answer locally, with no new state and no schema change, and my end-to-end probes confirm clone, pull and branch-switch all pass silently.

The cost is that the reconcile is a **write**, and the design has not fully absorbed what that implies. Every finding above except the settings-JSON cluster traces to that one fact: writes in dry runs, writes without the mutation guard, writes that dirty the root the merge lane requires clean. Fix the write discipline (P1-1, P2-1) and the design holds. The detection *strength* is deliberately modest — a committed hand edit, a recomputed marker, or an untracked `gwz.conf` all defeat it — and that is the correct trade for a defence whose false positive bricks a workspace, but it should be written down as an accepted residual rather than left implicit.

---

## Appendix — every command, direct exit status

| # | command | exit |
|---|---|---|
| 1 | `git log --oneline -6` / `git rev-parse HEAD` / `git status --porcelain` / `git worktree list` | 0 |
| 2 | `git diff --stat 8c59521..0523f0e -- src/checked_artifact/ src/workspace_ops/merge/` (empty) | 0 |
| 3 | `git diff --stat 8c59521..0523f0e` | 0 |
| 4 | `git diff --stat … -- scripts/` (empty) | 0 |
| 5 | `git diff --stat … -- dev-docs/` (empty) | 0 |
| 6 | `git diff --name-only … \| grep -v '^src/'` | grep 1 (no matches) |
| 7 | `git rev-parse 8c59521:src/lib.rs` / `0523f0e:src/lib.rs` | 0 |
| 8 | `git rev-parse 8c59521:src/workspace_ops/mod.rs` / tip | 0 |
| 9 | `python3.13 scripts/checks/check_checked_artifact_boundaries.py` → `ok (15 visible, 5 classified)` | **0** |
| 10 | `python3.13 -m pytest scripts/checks/test_check_checked_artifact_boundaries.py` | 1 (no pytest module) |
| 11 | `python3.13 scripts/checks/test_check_checked_artifact_boundaries.py` → `Ran 69 tests … OK` | **0** |
| 12 | `git show --stat` ×3 (per-commit diffstat) | 0 |
| 13 | `git log --format=… 8c59521..0523f0e` (messages, authorship) | 0 |
| 14 | numerous `grep -rn` / `sed -n` source reads | 0 |
| 15 | `cargo test --lib` at tip (**first run, disk at 165 MiB — ENOSPC artifact**) → 1664 passed; 4 failed | 101 |
| 16 | `cargo test --lib -- root_ambiguity_matrix root_successor_matrix service_fault_matrix root_fault_matrix --test-threads=1` → `root_ambiguity_matrix … ok` (killed during `root_fault_matrix`) | 0 (killed) |
| 17 | **`cargo test --lib workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix`** → `ok. 256 passed; 0 failed; 1413 filtered out; 399.54s` | **0** |
| 18 | `cargo fmt --check` @ `0523f0e` | **0** |
| 19 | `cargo check --all-targets` @ `0523f0e` | **0** |
| 20 | `CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets -- -D warnings` @ `0523f0e` | **0** |
| 21 | `cargo test --lib checked_artifact::` → `ok. 446 passed; 1223 filtered out` | **0** |
| 22 | `cargo test --lib -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::` → `ok. 965 passed; 1 ignored; 703 filtered out` | **0** |
| 23 | `git checkout --detach b23a68e` | 0 |
| 24 | `cargo fmt --check` @ `b23a68e` | **0** |
| 25 | `cargo check --all-targets` @ `b23a68e` | **0** |
| 26 | `clippy -D warnings` @ `b23a68e` | **0** |
| 27 | `cargo test --lib` @ `b23a68e` → `ok. 1655 passed; 0 failed; 1 ignored; 856.26s` | **0** |
| 28 | `git checkout --detach 3aa48ab` | 0 |
| 29 | `cargo fmt --check` @ `3aa48ab` | **0** |
| 30 | `cargo check --all-targets` @ `3aa48ab` | **0** |
| 31 | `clippy -D warnings` @ `3aa48ab` | **0** |
| 32 | `cargo test --lib` @ `3aa48ab` → `ok. 1663 passed; 0 failed; 1 ignored; 1283.03s` | **0** |
| 33 | `git checkout --detach 0523f0e` (restore) → `0523f0e9f081…`, worktree clean | **0** |
| 34 | `WebFetch code.claude.com/docs/en/permissions` (rule syntax, Read-and-Edit, pattern tables) | 0 |
| 35 | `WebFetch code.claude.com/docs/en/settings` (strict JSON, permissions shape, precedence) | 0 |
| 36 | **JSON probe** — verbatim `merge_deny_rules`/`push_json` over 31 fixtures on serde_yaml 0.9.34 | **0** |
| 37 | **git probe** — clone / pull / branch-switch / composition-commit / committed-hand-edit / gitignored-conf | **0** |
| 38 | **handler probe** — refusal text, acceptance end-to-end, `--force` validation ×3, broken-YAML error, dry-run purity, marker-tampered-alone, non-git root | **0** |
| 39 | `git rev-parse HEAD` + `git status --porcelain` final → `0523f0e…`, clean, 32 tags unchanged | **0** |

Notes on environment: the volume hit 100% capacity twice during this review (unrelated to the train — other lanes' build trees). Run #15 is invalid for that reason and its four failures must not be attributed to this change; every merge test in it passes when re-run with disk available. Probe crates and their target dirs were created under the session scratchpad and removed; the reviewed worktree was never edited, committed, pushed, or tagged.
