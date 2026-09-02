# GWZ parameter plumbing audit — has the over-claiming failure mode been overlooked elsewhere?

- **Date:** 2026-09-02
- **Trees audited:** `gwz-dev` workspace at root `688a8df`; **gwz-core `9c808ff`**,
  **gwz-cli `4791eb6`**, gwz-py `94378b9`. All line citations are at those commits.
- **Mode:** read-only audit. No source changed. The only binary used was the
  pre-existing `target/debug/gwz` (reports `gwz 0.2.0-dev`); every probe ran in a
  throwaway workspace under `/tmp/gwzaudit`. No test suite was run.
- **Prompt:** the follow-up to `dev-docs/GwzOverClaimingCommitDiagnosis.md` —
  the five defects there are fixed, but the *pattern* (a parameter accepted at
  the CLI and then collided, discarded, or never read) was never swept for
  systematically.

**Headline:** yes, the failure mode was overlooked elsewhere. Four reproduced
user-facing defects, one of them destroying data:

| # | Defect | Class | Severity |
|---|---|---|---|
| DR-1 | `--dry-run stash drop` **permanently destroys the stash** | D5 (never read) | **Critical** |
| DR-2 | `--dry-run stash apply` / `stash pop` perform the real restore | D5 (never read) | **High** |
| DR-3 | `--dry-run forall -- <cmd>` runs the command for real | D5 (never read) | **High** |
| DR-4 | `--dry-run init` **creates the workspace for real** | D5 (never read) | **High** |
| DR-5 | `gwz --all commit -m …` silently means `git commit -a` | D1 (id collision) | **Medium-High** |
| DR-6 | `materialize --branch` never consults `--force`, unlike `materialize --lock` | D2 (read-but-partial) | Medium |
| DR-7 | `--partial`, `--force` and `--sync` are accepted by ~20 verbs that never read them | D5 (never read) | Medium |
| DR-8 | `TagRequest.all` is a protocol field no handler reads and no CLI path sets | dead parameter | Low |
| DR-9 | gwz-py accepts `--ssh-timeout` and never sends it (no binding exists) | D5 (never read) | Medium |
| DR-10 | gwz-py has no F1 fix, so `gwz-py --all add` still widens the selection | D1 (id collision) | Medium |

A second finding, orthogonal to the code: **the generated `docs/CLI.md` promises
every global flag under every verb**, including the three cases the CLI or core
actively reject. See §4.

---

## 1. Method

Mechanical, not impressionistic. Four independent sweeps, each producing
citations rather than judgements:

1. **Enumeration.** Every clap argument in gwz-cli was extracted by parsing the
   `#[derive(…Args/Parser)]` structs under `gwz-cli/src/` (excluding `tests/`)
   and recording, per field: the clap **id** (the field name unless an explicit
   `id = …` is present — `long = "…"` does *not* change the id), the long name,
   the short name, and whether `global = true`. 34 structs, 132 arguments.
2. **Collision detection.** Set intersection of the `GlobalArgs` id set against
   each subcommand struct's id set, and of the long-name sets, and duplicate
   shorts within each struct. Clap merges same-id arguments across the
   global/subcommand boundary, so an id intersection *is* the D1 defect class.
3. **Delivery.** Every read of `self.global.*` outside the parser was grepped
   (`gwz-cli/src`, 24 sites) and traced to the field of `RequestMeta` /
   `OperationPolicy` / the per-verb request struct it lands in.
4. **Consumption.** For each parameter, every non-test reference in
   `gwz-core/src` was grepped and read in place, then classified per verb as
   `honoured` / `read-but-partial` / `NEVER READ` / `rejected` / `n/a`.
5. **Probes.** Fourteen end-to-end probes (P1–P14) against `target/debug/gwz` in
   a throwaway workspace with one member `m1`, checking real state (git index,
   HEAD, stash list, file existence) before and after. Every defect below is
   backed by a probe that was actually run, or is explicitly marked
   UNREPRODUCED.

Time-boxed to 45 minutes; anything not reached is marked **UNVERIFIED** rather
than guessed.

### The parameter surface

`GlobalArgs` (`gwz-cli/src/globalargs/parser.rs:31-201`), all `global = true`:

| clap id | long | type | delivered to |
|---|---|---|---|
| `root` | `--root` | `Option<String>` | `meta.workspace.root` (`clirequest/invocation.rs:36-42`) |
| `targets` | `--target` | `Vec<String>` | `meta.selection.targets` (`:55`) |
| `exclude_targets` | `--no-target` | `Vec<String>` | `meta.selection.exclude_targets` (`:59`) |
| `members` | `--member` | `Vec<String>` | folded into `selection.targets` (`:56`) |
| `exclude_members` | `--no-member` | `Vec<String>` | folded into `exclude_targets` (`:60`) |
| `paths` | `--member-path` | `Vec<String>` | folded into `selection.targets` (`:57`) |
| `exclude_paths` | `--no-member-path` | `Vec<String>` | folded into `exclude_targets` (`:61`) |
| **`all`** | `--all` | `bool` | `"@all"` token in `selection.targets` (`:52-54`) — **collides**, see §2 |
| `dry_run` | `--dry-run` | `bool` | `meta.dry_run` (`:45`) |
| `partial` | `--partial` | `bool` | `policy.partial` (`:87-90`) + forall `continue_on_fail` (`:142`) |
| `force` | `--force` | `bool` | `policy.destructive` (`:91-93`) |
| `sync` | `--sync` | `Option<SyncArg>` | `policy.sync` (`:94`) |
| `remote` | `--remote` | `Option<String>` | `policy.remote` (`:94`), plus `TagRequest.remote` (`:177`) and `PushRequest.remote` (`:191`) |
| `jobs` | `--jobs` | `Option<i64>` | `policy.concurrency` (`:95`) |
| `max_per_host` | `--max-per-host` | `Option<i64>` | `policy.max_connections_per_host` (`:96`) |
| `progress_interval` | `--progress-interval` | `Option<i64>` | `policy.progress_min_interval_ms` (`:97-101`, **always** `Some`, defaulted) |
| `json` / `jsonl` | `--json` / `--jsonl` | `bool` | `OutputMode` only (`:20-27`); never leaves the CLI |
| `ssh_timeout` | `--ssh-timeout` | `Option<i64>` | `gwz-cli/src/lib.rs:146`, a libgit2 knob; never reaches `RequestMeta` |

Per-subcommand arguments were enumerated in full (34 structs); they are cited
below only where a finding depends on them.

---

## 2. Arg-id collisions (D1 class)

**Result: exactly one id collides, and it is the known one.** The mechanical
intersection of the `GlobalArgs` id set with every subcommand struct's id set
yields:

```
COLLISION CommitArgs  ['all']   gwz-cli/src/clirequest/repo.rs:141  (-a/--all)
COLLISION StageArgs   ['all']   gwz-cli/src/clirequest/repo.rs:170  (-A/--all)
```

Nothing else collides. `force`, `remote`, `partial`, `sync`, `jobs`, `root`,
`json` appear nowhere in a subcommand struct. The long-name intersection is the
same single row (`--all` × 2). No duplicate short within any one struct
(`-m` recurs across `commit`/`merge`/`tag`/`stash push`, `-a` across
`commit`/`stash push`, but never inside one command's arg set, so clap is
unambiguous). Near-misses that are *not* collisions: `RepoDetachArgs.member`
(id `member`) vs global id `members`; `MaterializeArgs.tag` vs the `tag`
subcommand name; `RepoCommandArgs::Sync` vs global `--sync`.

### What the user-typed forms actually deliver

The F1 fix (`clirequest/invocation.rs:52`, `:74-77`) suppresses the `@all`
*selector* for `add`/`commit`, so the direction `-A → global.all → "@all"` is
dead: `global.all` is read at exactly one site (`invocation.rs:52`) and nowhere
else in gwz-cli. **The opposite direction was left live**, exactly as
`GwzOverClaimingCommitDiagnosis.md` §6 follow-up 3 predicted — and it is worse
than "a quirk".

**Probe P2** (`/tmp/gwzaudit/ws`, member `m1` with two untracked files):

```
$ gwz add --target mem_m1
gwz: InvalidRequest: nothing specified to stage; pass pathspecs or --all
$ gwz --all add --target mem_m1
status: Ok
$ git -C m1 status --porcelain
A  f1.txt
A  f2.txt
```

The global `--all` — documented as *"Select all workspace targets (`@all`)"*
(`parser.rs:95-100`) — became the git-style `-A` and staged. It also contributed
*no* target selection, so `--all` now means two unrelated things depending on
the verb.

**Probe P3 (defect DR-5)** — same workspace, `m1/f1.txt` modified and **not**
staged:

```
$ git -C m1 status --porcelain
 M f1.txt
$ gwz --all commit -m probe --target mem_m1
status: Ok
mem_m1 m1 Ok
$ git -C m1 show --stat --oneline HEAD
f5445a4 probe
 f1.txt | 1 +
$ git -C m1 status --porcelain          # clean — the unstaged change was committed
```

**Control (P3b)**, identical setup, `--all` omitted:

```
$ gwz commit -m ctl --target mem_m1
status: Ok
$ git -C m1 status --porcelain
 M f1.txt                                # correctly NOT committed
$ git -C m1 log --oneline -1
f5445a4 probe                            # HEAD unmoved
```

So `gwz --all commit -m …` silently applies `git commit -a` semantics. This is
the same shape as the incident that started the whole investigation — a global
selector flag turning into an index-sweeping verb flag — merely in the mirror
direction, and it is still shipping.

---

## 3. Verb × parameter matrix

Legend: **H** honoured (read and acted on) · **P** read-but-partial (some
branches only) · **N** NEVER READ (delivered, zero references in the reachable
handler code) · **R** rejected (explicit refusal) · **–** n/a (read-only verb or
parameter cannot reach it).

| verb | `dry_run` | `selection` | `partial` | `force` | `sync` | `remote` | `jobs`/`max-per-host` | `progress-interval` |
|---|---|---|---|---|---|---|---|---|
| `add` (stage) | **H** `handle_stage.rs:90,95,121,164,179` | **H** (F2) `handle_stage.rs:47`, `stage_routing.rs` | **N** | **N** | – | – | – | **N** |
| `commit` | **H** `handle_commit.rs:47,76,139` | **H** (F3) | **N** | **N** | – | – | – | **N** |
| `branch` | **H** `handle_branch.rs` (7 refs) · probe P7 | **H** | **N** | **N** | – | – | – | **N** |
| `tag` | **H** `handle_tag.rs:25,45,78,104,167,199` · probe P8 | **H** | **N** | **N** | – | **H** `invocation.rs:177` | **N** | **N** |
| `snapshot` | **H** probe P9 | **H** | **N** | **N** | – | – | – | **N** |
| `capture` | **H** probe P10 | – | **N** | **N** | – | – | – | **N** |
| `materialize` (`--lock`/`--snapshot`/`--tag`) | **H** `handle_materialize.rs:161,167,175` | **H** `:234-236` | **N** | **H** `:238-250` → `materialize_preflight.rs:44` | **N** | **N** | **H** `:289-290` | **H** `:190,:499` |
| `materialize --branch` | **H** `handle_materialize.rs:430` | **H** `:428` | **N** | **N** — dispatched at `:171-173` *before* `destructive_allowed` is computed (**DR-6**) | **N** | **N** | **N** | **N** |
| `pull` | **H** `pull_head_member_preflight.rs:44,55,86,120` | **H** | **P** `:850` — the `SyncBehavior::Merge` arm only; `Rebase` `:868-884`, `Reset` `:885`, `FetchOnly`/`FfOnly`/`DriverSelected` `:791-793` ignore it | **P** `:647` — the `Reset` arm of `pull_dirty_guard` only; `Merge`/`Rebase`/`FfOnly` hit the unconditional refusal `:653-656` | **H** `:73,232,592,714` | **H** `:378,383` | **H** `:509-510` | **H** `:143` |
| `push` | **H** `push_member.rs` (7 refs) — UNREPRODUCED | **H** | **N** | **N** (no `force` token in `push_member.rs`) | – | **H** `:379,409` | **H** `:133,140` | **H** `:122` |
| `repo create/clone/add/attach/detach/sync` | **H** `handle_repo_lifecycle.rs` (12), `handle_create_repo.rs` (17) · probes P11–P13 | **H** | **N** | **N** | – | – | UNVERIFIED | **H** `handle_repo_lifecycle.rs:86` |
| `init --update` (bootstrap) | **H** `workspace_bootstrap.rs:64-97` · probe P14 | – | **N** | **H** `workspace_bootstrap.rs:261` | – | – | – | – |
| `init` (create workspace) | **N** — `handle_create_repo.rs:19-60` · probe P15 (**DR-4**) | – | **N** | **H** `:27,56` | – | – | – | – |
| `init <url>…` (from sources) | **H** `handle_init_from_sources.rs:80` | – | **N** | **H** `:43` | – | – | **H** `:124,131` | **H** `:102` |
| `clone` | **R** `clirequest/invocation.rs:13-15` | – | **N** | **N** | UNVERIFIED | UNVERIFIED | UNVERIFIED | UNVERIFIED |
| `merge` | **P** — start only; **R** for every other op `merge/validate.rs:59-60`; `start.rs:68`, `plan.rs:213` | **H** | **R** `validate.rs:63` | **R** `:66` | **R** `:71` | **R** `:72` | **R** `:74,80` | **R** `:76` |
| `stash push` | **H** `handle_stash.rs:47`, `handle_stash/commands.rs:58` | **H** | **N** | **N** | – | – | – | **N** |
| `stash apply` / `pop` / `drop` | **N** — see DR-1/DR-2 · probes P4, P5 | **H** | **N** | **N** | – | – | – | **N** |
| `stash list` | – | **H** | – | – | – | – | – | – |
| `forall` | **N** — see DR-3 · probe P6 | **H** `forall.rs:52-57` | **H** `invocation.rs:142` | **N** | – | – | **N** | **N** |
| `status` / `ls` / `log` / `diff` | – | **H** | – | – | – | – | – | – |

Two rows deserve to be read as findings rather than data:

- **The `partial` column is almost entirely `N`.** `PartialBehavior` has exactly
  two non-test readers in all of gwz-core:
  `workspace_ops/pull_head_member_preflight.rs:851` and
  `workspace_ops/merge/validate.rs:63` (a rejection). Every other verb — stage,
  commit, branch, tag, snapshot, capture, materialize, push, all six `repo`
  operations, all five `stash` operations, init — accepts `--partial` and never
  looks at it. gwz-cli's `forall` is the only other consumer, via
  `continue_on_fail` (`invocation.rs:142`).
- **The `force` column is almost entirely `N`.** `DestructiveBehavior` has four
  non-test readers: `handle_materialize.rs:243`, `workspace_bootstrap.rs:262`,
  `pull_head_member_preflight.rs:648`, `merge/validate.rs:66` (a rejection).
  Notably `push_member.rs`, `handle_branch.rs` and `handle_tag.rs` contain **no
  occurrence of the string `force` at all** — so `gwz --force push`,
  `gwz --force branch --delete x` and `gwz --force tag …` are accepted and inert.

`merge` is the one verb in the tree that gets this right: it enumerates every
policy field it cannot honour and refuses the request
(`workspace_ops/merge/validate.rs:57-84`). That is the template for §8.

---

## 4. Docs promises not held

| # | Promise | Location | Verdict |
|---|---|---|---|
| 1 | "`--dry-run` plans an operation without mutating workspace metadata or member repositories. It is not supported for `gwz clone`." | `gwz-cli/docs/Concepts.md:101-102` | **NOT HELD** — the only stated exception is `clone`; `stash apply/pop/drop` and `forall` mutate anyway (DR-1/2/3) |
| 2 | Per-verb help block *"Plan the operation without mutating workspace metadata or member repositories"* rendered under **`gwz forall`** | `gwz-cli/docs/CLI.md:946`; source `globalargs/parser.rs:103-108` | **NOT HELD** (DR-3) |
| 3 | Same block under **`gwz stash apply`** | `gwz-cli/docs/CLI.md:3007` | **NOT HELD** (DR-2) |
| 4 | Same block under **`gwz stash pop`** | `gwz-cli/docs/CLI.md:3098` | **NOT HELD** (DR-2) |
| 5 | Same block under **`gwz stash drop`** | `gwz-cli/docs/CLI.md:3189` | **NOT HELD** (DR-1) |
| 6 | "Select all workspace targets (`@all`). May be combined with target exclusions." | `globalargs/parser.rs:95-100`, echoed in every `CLI.md` help block | **NOT HELD for `add`/`commit`** — there `--all` is the git-style flag and selects nothing (DR-5, probes P2/P3) |
| 7 | "`--partial` allows operations to complete for members that can proceed even when another selected member fails." | `gwz-cli/docs/Concepts.md:104-106` | **NOT HELD** for every verb except `pull`, `merge` (rejects) and `forall` — §3 |
| 8 | "`--force` allows destructive behavior when an operation requires explicit confirmation." | `gwz-cli/docs/Concepts.md:108-109` | **NOT HELD** for `push`, `branch`, `tag`, `stash`, `commit`, `add`, `repo *` — §3 |
| 9 | "Use `--dry-run` for materialize, pull, push, tag, add, and other mutations when the effect is not obvious." | `gwz-cli/docs/RootWorkspace.md:147-148` | **HELD** for the five named verbs; **"and other mutations" is NOT HELD** — this sentence is what would send a user to `--dry-run stash drop` |
| 10 | "`dry_run` — Plan without mutation **where supported**." | `gwz-core/docs/OperationModel.md:18` | **UNRESOLVED HEDGE** — flagged as debt in the diagnosis §6.5 and still unresolved; §58-64 says "Handlers that support dry-run…" without ever naming them. This audit's §3 column 1 *is* that missing matrix |
| 11 | `gwz merge` usage line advertises `[--dry-run]` for start | `gwz-cli/docs/CLI.md:1495`, `parser.rs:377-381` | **HELD** — start honours it (`merge/start.rs:68`), all other merge ops reject it explicitly (`merge/validate.rs:59-60`) |

### 4a. The generated help is the largest single promise defect

`gwz-cli/docs/CLI.md` is `@generated by python scripts/generate_cli_reference.py`
(CLI.md:1) and reproduces the **entire** Global Options block (CLI.md:66-137)
verbatim under all 33 command sections — the `--dry-run` copies alone sit at
lines 96, 208, 327, 423, 526, 630, 836, 946, 1063, 1163, 1320, 1437, 1563, 1662,
1761, 1868, 1977, 2088, 2195, 2296, 2397, 2497, 2610, 2720, 2826, 2916, 3007,
3098, 3189, 3296, 3422. Because the generator emits the block unconditionally, it
promises flags that are **actively rejected**:

| Rejected in code | Promised anyway by the generated help |
|---|---|
| `--dry-run` on `clone` (`clirequest/invocation.rs:13-15`; `clone.md:44`, `Concepts.md:102`) | CLI.md:526 |
| `--json`/`--jsonl` on `forall` (`clirequest/invocation.rs:121`; `forall.md:94`, `MachineOutput.md:489`) | CLI.md:978-982 |
| `--sync`, `--remote`, `--jobs`, `--max-per-host`, `--progress-interval`, `--partial`, `--force` on `merge` (`gwz-core/src/workspace_ops/merge/validate.rs:57-84`; `merge.md:386-389`) | CLI.md:1566-1593 |

This is the mirror image of DR-1/2/3/4: there the code under-delivers on a
promise; here the doc generator manufactures promises the code was careful to
refuse. Both are curable by the same artefact — a per-verb capability
declaration that the generator reads (§8).

### 4b. Further doc defects surfaced by the sweep

| # | Finding | Location |
|---|---|---|
| 12 | **`RootWorkspace.md` has no dry-run matrix.** The file is 149 lines, contains no table and no per-verb support list; the only support statement is the hedged prose at `:147-148`. The matrix the follow-up assumed exists does not | `gwz-cli/docs/RootWorkspace.md` |
| 13 | **`--path` is documented but does not exist.** `add.md:22` and `stage_long.rs:9` name the selection triple as `--target`/`--member`/`--path`; the real global flag is `--member-path` (`parser.rs:77-82`). `--path` exists only as `gwz init --path` (`parser.rs:317-322`) | `gwz-cli/docs/commands/add.md:22` |
| 14 | **`unsupported_member` is a documented policy with no CLI flag.** `OperationModel.md:52` and `ErrorCatalog.md:21` describe fail-vs-skip behaviour; `clirequest/invocation.rs:83-101` never constructs it | `gwz-core/docs/OperationModel.md:52` |
| 15 | `merge --gc` while a merge is open: `merge.md:345-346` says it refuses; `MergeRecovery.md:395-397` says it runs | doc-vs-doc |
| 16 | Root as a stash participant: `stash.md:11` says never; `merge.md:324-330` shows `gwz --target @root stash drop` | doc-vs-doc |
| 17 | `commit --no-commit-marker`: `commit.md:34` promises trailer suppression too; clap help (`clirequest/repo.rs:158`) promises only the marker | doc-vs-help |
| 18 | `commit.md:18` lists `--all` as a way to select the root, which F1 made false (`invocation.rs:74-77`) — the doc was not updated with the fix | doc-vs-code, DR-5 adjacent |
| 19 | `merge.md:105-106` says bare `--all` "retain[s] merge's member-only default", but `invocation.rs:74-77` exempts only `Add`/`Commit`, so merge *does* receive `@all` | doc-vs-code, **UNVERIFIED** by probe |
| 20 | `tag --push`/`--fetch`: docs say "all tags"; clap says "all **gwz** tags" (`nameargs.rs:54,58`) | doc-vs-help |
| 21 | `--all` composability: `OperationModel.md:36` says `all=true` is "rejected if filters are also present" and `Concepts.md:93` forbids combining with `--member`, yet `CLI.md:93-94`, `RootWorkspace.md:119`, `push.md:22` and `forall.md:54` all promote `gwz --all --no-target @root` as canonical | doc-vs-doc |
| 22 | `log --tagged` (`logargs.rs:84`) is entirely absent from `log.md`; ~15 `diff` flags (`--numstat`, `--shortstat`, `--summary`, `--raw`, `-z`, `-M`, `-U`, `-w`, `-b`, prefix flags…) are absent from `diff.md` | help-only promises |
| 23 | `forall` default selection: `forall.md:10-13` says materialized configured members with the root excluded unless selected; the clap `[projects]` help (CLI.md:901) says "empty = all" | doc-vs-help |

Items 15-23 are **UNVERIFIED against behaviour** — they are doc-vs-doc and
doc-vs-help-string inconsistencies found by reading, not probes. They are
included because every one of them is the same failure mode at the
documentation layer: a promise emitted without anything checking it.

Docs subagent sweep of the remaining per-verb `docs/commands/*.md` files:
**UNVERIFIED** at the deadline (`forall.md` and `stash.md` contain no `dry`
occurrence at all, so the promise for those verbs lives only in `Concepts.md`
and the generated `CLI.md`).

---

## 5. Test-coverage gaps, and gwz-py

### 5a. Where the tests are

All gwz-core behavioural coverage lives in `gwz-core/src/**/tests/`
(`workspace_ops/tests/g00`–`g25`, `operation/`, `merge/*/tests/`);
`gwz-core/tests/*.rs` holds **no** flag/behaviour tests (`protocol.rs` is CBOR
round-trip only). End-to-end CLI coverage that actually spawns the binary is two
files: `gwz-cli/tests/local_workflows.rs` and `gwz-cli/tests/diff_workflows.rs`.
`gwz-cli/src/tests/g00`–`g11` are in-process **parser** tests — plumbing-only by
construction, since they assert `request.meta.*`.

### 5b. Honoured but with zero test at any layer

1. **`--progress-interval`.** `should_emit_progress` (`operation/push_event.rs:507-518`)
   is wired into init, materialize, repo-lifecycle, push and pull and has **no
   test at all**. It is also the only global with a CLI-injected default
   (`clirequest/common.rs:5` = 100 ms) that is never asserted end to end.
2. **`--max-per-host`.** `resolve_per_host(policy.max_connections_per_host)` at
   four call sites; no test sets the policy field. The per-host primitive is
   tested in isolation only (`operation/operation_runtime.rs:124-147`).
3. **`--jobs` for pull/push/init/materialize.** Only `log` has behavioural jobs
   tests (`operation/commit_log/tests.rs:1852-1885`, `:2113-2125`).
   `workspace_ops/tests/g01.rs:280,319` test *default* parallelism, not `--jobs`.
4. **`--ssh-timeout`.** Parsed and clamped at `gwz-cli/src/lib.rs:144-147`; no
   test at any layer, not even a parser test.
5. **`--dry-run` for `push`** (`push_member.rs:64,247,326`) and **for `pull`**
   (`pull_head_member_preflight.rs:120`) — the two network verbs, both untested.
6. **`--dry-run` for `repo add` / `repo create` / `repo attach`**
   (`handle_create_repo.rs:139,312,406`, `handle_repo_lifecycle.rs:67,210`) —
   only `detach` (`g22.rs:276`) and `sync` (`g02.rs:470`) are covered.
7. **`--force` on the pull dirty guard** (`pull_head_member_preflight.rs:645-651`,
   the `Reset` + `Allow` branch that discards a dirty worktree).
   `DestructiveBehavior` appears in three core tests (`g02.rs:1503`,
   `g19.rs:200`, `g21.rs:316`) and none of them is on that branch.

### 5c. Plumbing-only tests that read as coverage

- `gwz-cli/tests/local_workflows.rs:674 add_create_and_dry_run_commands_work` —
  despite the name it never dry-runs `add`; its one dry-run (`:702-715`) asserts
  `aggregate_status == "Accepted"` and **no state at all**.
- `gwz-cli/src/tests/g01.rs:259 parses_global_selection_policy_and_output_flags`
  (`:288-306`) is the only test that exercises `--partial --force --sync
  --remote --jobs` together, and it asserts `request.meta` fields exclusively.
- `merge/validate.rs:267 unrelated_policy_errors_name_the_field_and_cli_option`
  asserts the error text, not that no state moved.
- The F1 tests `stage_all_flag_does_not_widen_an_explicit_target` /
  `commit_all_flag_does_not_widen_an_explicit_target` (`gwz-cli/src/tests/g01.rs:381,397`)
  are parser-level assertions on the built request, not effects.

### 5d. Flag position

**Every** e2e global-flag use in `gwz-cli/tests/` is *before* the subcommand,
with exactly two exceptions (`push --remote origin`,
`local_workflows.rs:363,653`). `--dry-run` after a subcommand is exercised
nowhere e2e (only in-process, `gwz-cli/src/tests/g01/commands.rs:242`). Only
`g01.rs:413 global_all_still_selects_every_target_for_other_verbs` tests both
positions, and only for `--all status` at parser level. **`--target` itself
appears in no e2e test at all** — only `--member` and `--no-target` do.

That last fact is the direct descendant of the diagnosis's observation that "the
only CLI selection test placed `--target` before the subcommand": the flag the
incident was about is still not exercised through the binary.

### 5e. gwz-py — two real divergences and one drop

Package `gwz-py/src/gwz/` (`client.py`, `cli_shared.py`, `cli_mutation.py`,
`cli_local.py`); bindings `gwz-py/native/src/`. `Client.meta()`
(`client.py:164-249`) is the single funnel and every verb method forwards
`**meta` verbatim, so all seven policy globals are exposed. Renames are
cosmetic and consistent (`--jobs`→`concurrency`, `--force`→`destructive`,
`--all`→`all_members`, `--max-per-host`→`max_connections_per_host`,
`--progress-interval`→`progress_min_interval_ms`). Three findings are not
cosmetic:

- **DR-9 — `--ssh-timeout` is accepted and silently discarded.** Declared
  `cli_shared.py:307-314`, normalized `:35`, asserted as *parsed* by
  `src/tests/test_cli_parser.py:81,113` — and **absent from `meta_kwargs()`
  (`cli_shared.py:420-446`) and from `Client.meta()`**. There is no
  `set_server_timeout_ms` binding in `native/src/lib.rs:175-188`. So
  `gwz-py --ssh-timeout 3 pull` keeps libssh2's no-timeout default while
  `gwz --ssh-timeout 3 pull` does not. The parity fixture
  (`src/tests/fixtures/cli_parity/parser_cases.json:6,60`) lists the flag as
  accepted on both sides — **parity is asserted at the parse level, which is
  exactly the plumbing-only trap the diagnosis identified.**
- **DR-10 — gwz-py reopens the D1 widening that F1 closed.** F1 lives entirely
  in `gwz-cli` (`clirequest/invocation.rs:74-77`). gwz-py's `--all` sets
  `Selection.all = True` *and* injects `"@all"` into `targets`
  (`client.py:190-191,201`), with no add/commit exemption; its argparse
  `conflict_handler="resolve"` (`cli_shared.py:155-157`) only suppresses the
  collision when `--all` follows the verb. So `gwz-py --all add --target mem_x`
  reproduces the original incident's widening. **UNREPRODUCED** — no gwz-py test
  was found covering that ordering, and running the Python client did not fit
  the time box.
- **`--remote` is stripped from the policy for `push` and `tag`.**
  `cli_mutation.py:150,207-214` call `_meta_without(meta, "remote")`, so
  `policy.remote` is never set; the Rust CLI sets both the request-level field
  and `policy.remote` (`invocation.rs:94,177,191`). Since `push_member.rs:379,409`
  reads `policy.remote` as its fallback, this is a live behaviour difference for
  `push`. Lower-stakes companion: gwz-py leaves `progress_min_interval_ms`
  unset, so core defaults to 0 (emit every update) where the Rust CLI coalesces
  at 100 ms.

Also confirmed: gwz-py exposes `dry_run` for **every** verb
(`cli_shared.py:238-244,430-431`), guarding only `clone`
(`cli_local.py:98-99`) — so DR-1, DR-2, DR-3 and DR-4 are all reachable through
the Python surface too, exactly as D5 was.

**UNVERIFIED:** whether `StageRequest.all` / `CommitRequest.all` have core
behavioural tests; the bodies of `g16.rs:127`, `g22.rs:276`, `g00.rs:326`,
`g02.rs:470/687/792` were classified behavioural from their assertion-bearing
names and the `byte-identical` / `without mutating` idiom, not read line by
line.

## 6. Seam findings

The dry-run seam split named as D5's structural cause is **still exactly as
described, and still has occupants that ignore dry-run**:

- `guarded_workspace_root(start, workspace, command, dry_run)`
  (`gwz-core/src/workspace_ops/merge/runtime/mutation_guard.rs:56-71`) — takes
  `dry_run` by parameter and short-circuits the mutator lock.
  Callers: `handle_branch.rs:33`, `handle_create_repo.rs:91,245,459`,
  `handle_materialize.rs:157`, `handle_repo_lifecycle.rs:49,193,269`,
  `handle_stash.rs:43`, `merge/runtime/dispatch.rs:270`,
  `pull_head_member_preflight.rs:45`, `push_member.rs:36`,
  `workspace_bootstrap.rs:60`.
- `acquire_workspace_mutation_guard(start, workspace, command)`
  (`mutation_guard.rs:25-51`) — **no `dry_run` parameter**, so a caller cannot be
  reminded by the type system. Callers: `handle_commit.rs:37`,
  `handle_stage.rs:27`, `handle_materialize.rs:27,100`, `handle_tag.rs:32`,
  `workspace_bootstrap/conf_gate.rs:296`, **and — outside gwz-core entirely —
  `gwz-cli/src/globalargs/dispatch.rs:21-31` for `forall`**.

The 2026-09-01 remediation cured the *handlers* on the second seam by adding
manual `if dry_run { return … }` gates inside each one. It did not change the
seam. Consequently:

1. The seam still admits new ignorers by construction; `forall` is one, and it
   is in a different repository, so a gwz-core-only sweep would never see it.
2. `handle_stash.rs:43-48` sits on the *first* seam but passes
   `request.op == StashOp::Push && dry_run` — i.e. it hard-codes the truth that
   only `push` honours dry-run, and `apply`/`pop`/`drop` therefore take the real
   mutating guard and never re-check the flag (`handle_stash/commands.rs:58` is
   the push-only gate; `handle_stash_restore` has no `dry_run` reference). This
   is a **read-but-partial (D2) shape wearing a honoured-seam costume**: the
   parameter *is* threaded, and is deliberately ANDed away for three of five ops.
   `mutation_guard.rs:69` already notes the asymmetry in prose
   (`conf_gate.rs:69-70`: "`guarded_workspace_root` already yields `None` for a
   dry run, but `acquire_workspace_mutation_guard` does not").

A second seam split, smaller: **`Selection` has two encodings of "all"** — the
protocol field `Selection.all: Option<bool>` (`protocol/generated.rs:1963`) and
the `"@all"` string token in `Selection.targets`. gwz-core reads the field at
`target_selection.rs:90,248`, `handle_stage.rs:308`,
`handle_stash/shared.rs:312`, `handle_repo_lifecycle.rs:565`; **gwz-cli never
sets it** (`invocation.rs:63-68` constructs `Selection { targets,
exclude_targets, ..Default::default() }`). The F1 fix therefore lives entirely
in gwz-cli: any non-CLI caller (gwz-py, a direct core embedder) that sets
`selection.all = true` on a stage or commit request still gets the full
workspace widening that F1 was written to prevent. **Confirmed**: gwz-py sets
`Selection.all = True` and injects `"@all"` with no add/commit exemption
(`gwz-py/src/gwz/client.py:190-191,201`) — see DR-10 in §5e.

## 7. Defects

Ranked by user harm.

### DR-1 — `--dry-run stash drop` permanently destroys the stash — **Critical**

Irreversible data loss under a flag whose entire contract is "changes nothing".
`handle_stash.rs:47` passes dry-run to the guard only when
`request.op == StashOp::Push`; `handle_stash_restore` never references
`meta.dry_run`; the push-only gate is `handle_stash/commands.rs:58`.

Reproduction (probe P5, actually run, `/tmp/gwzaudit/ws`):

```
$ gwz stash push -m probe
status: Ok
stash_unix_ms_1788351657365 unix-ms:1788351657365 (1 member)
$ git -C m1 stash list
stash@{0}: On main: gwz:stash_unix_ms_1788351657365: probe

$ gwz --dry-run stash drop stash_unix_ms_1788351657365
status: Ok
stash_unix_ms_1788351657365 unix-ms:1788351657365 (1 member)
mem_m1 m1 Ok

$ git -C m1 stash list
                      # EMPTY — the stash is gone
$ gwz stash list
status: Ok            # no rows — the coordinated bundle is gone too
```

Aggravator: the response reports `Ok` with a member row, which reads exactly
like a successful *plan*. There is nothing in the output to distinguish "planned
a drop" from "dropped".

### DR-2 — `--dry-run stash apply` / `stash pop` perform the real restore — **High**

Same code path as DR-1. `apply` overwrites the working tree; `pop` additionally
drops the stash.

Reproduction (probe P4, actually run) — `m1` clean before, stash held:

```
$ git -C m1 status --porcelain      # (empty)
$ gwz --dry-run stash apply
status: Ok
mem_m1 m1 Ok
$ git -C m1 status --porcelain
 M f1.txt                            # the working tree was really restored
```

`stash pop` was **UNREPRODUCED** — it is the identical dispatch arm
(`handle_stash.rs:68-70` routes `Apply | Pop | Drop` to the same
`handle_stash_restore`), so it is asserted from the shared code path, not from a
probe.

### DR-3 — `--dry-run forall -- <cmd>` runs the command for real — **High**

`gwz-cli/src/globalargs/dispatch.rs:21-31` takes
`acquire_workspace_mutation_guard` unconditionally for `forall`, and
`forall.rs::execute_forall` never reads `meta.dry_run`. The command is arbitrary,
so the blast radius is arbitrary — `gwz --dry-run forall -- git reset --hard` is
a working way to lose every member's work while believing nothing will happen.

Reproduction (probe P6, actually run):

```
$ gwz --dry-run forall -- sh -c 'echo FORALL_RAN > FORALL_RAN.txt'
=== m1 ===
$ ls m1/FORALL_RAN.txt
m1/FORALL_RAN.txt                    # created
```

### DR-4 — `gwz --dry-run init` creates the workspace for real — **High**

`handle_create_workspace` (`gwz-core/src/workspace_ops/handle_create_repo.rs:19-60`)
contains **no reference to `meta.dry_run`**. It is the one mutating handler that
neither sits on either guard seam nor gates itself.

Reproduction (probe P15, actually run, in an empty directory):

```
$ ls -a                 # . ..  — empty
$ gwz --dry-run init
status: Ok
$ ls -a
.  ..  .claude  .git  AGENTS.md  AGENTS_GWZ.md  gwz.conf
$ ls gwz.conf
gwz.lock.yml  gwz.yml  markers
```

A git repository, a workspace manifest, a lock, a marker directory and two
agent-bootstrap files were created. Contrast `init --update`, which honours the
flag correctly (probe P14 returned `status: Noop`, nothing written) — the two
`init` shapes route to different handlers and only one of them was ever gated.
`gwz-cli/docs/commands/init.md:31-33` is the strongest availability claim for
`--dry-run` anywhere in the doc set: it lists the flag as "also available" for
`init` with no qualification.

### DR-5 — `gwz --all commit` silently means `git commit -a`; `gwz --all add` means `add -A` — **Medium-High**

The surviving half of the D1 id collision (diagnosis §6 follow-up 3). Reproduced
above as probes P2/P3 with control P3b. Harm: an unrequested index sweep of
every tracked modification in the selected members, from a flag documented as a
*selector*. It is the same category of surprise as the incident commit, with a
smaller radius (tracked modifications only, not untracked files, and only within
the selection now that F2/F3 landed).

### DR-6 — `materialize --branch` ignores `--force` while `materialize --lock` honours it — **Medium**

A D2-class split *inside one verb*. `handle_materialize.rs:171-173` dispatches
the `--branch` shape to `handle_materialize_branch` **before**
`prepare_materialize_execution` computes `destructive_allowed`
(`handle_materialize.rs:238-250`). The branch path's dirty gate is
`preflight_branch_switch` (`workspace_ops/branch_switch.rs:7-38`), which takes
no policy argument and returns `DirtyMember` at `:34-37` with no escape hatch.
So `gwz --force materialize --tag v1` proceeds on a dirty member and
`gwz --force materialize --branch x` does not.

**UNREPRODUCED** — needs a dirty-member fixture plus a second branch; it did not
fit the time box. The asymmetry is legible from the dispatch order alone.

Nuance worth preserving: the *refusal* here may be deliberate — both
`gwz-cli/docs/commands/materialize.md:62-65` and
`gwz-core/docs/OperationModel.md:170-174` state that a dirty switch to a
different commit is rejected "even under force policy". If so the defect is that
the same sentence is not true of `branch --create --switch`
(`gwz-cli/docs/commands/branch.md:69-73` makes the identical promise) and that
nothing in the code records the intent — the parameter simply never arrives.

### DR-7 — `--partial`, `--force` and `--sync` accepted-and-ignored by most verbs — **Medium**

Not a state-corruption bug, a false-assurance bug: `gwz --force push` reads as
"I have authorised the dangerous thing" and changes nothing; `gwz --partial add`
reads as "let the other members proceed" and changes nothing. Enumerated in §3;
the evidence is the total non-test reader set for `PartialBehavior` (2 sites) and
`DestructiveBehavior` (4 sites) in all of gwz-core. **UNREPRODUCED** as an
end-to-end probe — demonstrating an *absence* of effect needs a multi-member
failure fixture that did not fit the time box. The static evidence (zero
references in the reachable handler code) is the same evidence that established
D5 in the original diagnosis.

### DR-8 — `TagRequest.all` is a dead parameter — **Low**

`protocol/generated.rs:4244` declares `TagRequest.all: Option<bool>`;
`gwz-cli/src/clirequest/invocation.rs:180` hard-codes `all: None`;
`workspace_ops/handle_tag.rs` contains zero references to `request.all`. A
protocol field that no producer sets and no consumer reads. Same shape, lower
stakes: `PushRequest.refspec` is hard-coded `None` at `invocation.rs:190` but is
at least read by `push_member.rs:243,322` via `resolve_push_refspec`, so it is
reachable by other embedders.

### DR-9 / DR-10 — gwz-py

Both are written up in §5e with citations: gwz-py accepts `--ssh-timeout` and
never transmits it (**Medium**, no binding exists — `cli_shared.py:307-314` vs
`meta_kwargs()` `:420-446`), and gwz-py carries no equivalent of the F1 fix, so
`gwz-py --all add --target mem_x` still injects `@all`
(**Medium**, `client.py:190-201`). Both **UNREPRODUCED** — the Python surface was
not executed inside the time box.

### Non-defect observations

- `gwz commit -m ctl --target mem_m1` with nothing staged returns `status: Ok`
  and prints **no member row at all** (control P3b). Not wrong, but the silent
  no-op is the same reporting shape that hid D4.
- `merge`'s policy rejection block (`merge/validate.rs:57-84`) is correct and is
  the only place in the tree that closes this class by construction.
- **Not a defect, checked and cleared:** `handle_tag.rs` resolves the remote only
  from `request.remote` (`:107,134,165,197`) and never falls back to
  `policy.remote`, unlike `push` (`push_member.rs:379,409`). Via the CLI this is
  harmless because `clirequest/invocation.rs:177` copies the global `--remote`
  into `TagRequest.remote`, so `gwz --remote upstream tag --push v1` does reach
  the right remote. It is a latent inconsistency for non-CLI embedders only: two
  parallel channels carry the same parameter and `tag` reads one, `push` reads
  both. `PushRequest.refspec` (`invocation.rs:190`, hard-coded `None`) is the
  same shape.

## 8. Recommended fix shape, per class

**Class D5 (delivered, never read) — make the seam carry the parameter by type.**
The 2026-09-01 remediation fixed the five handlers but left the seam that
produced them. Change `acquire_workspace_mutation_guard` to take `dry_run: bool`
and return `Option<WorkspaceMutationGuard>` — i.e. collapse it into
`guarded_workspace_root` — so that *no* caller can acquire a mutation guard
without having stated an answer for dry-run. Every current caller
(`handle_commit.rs:37`, `handle_stage.rs:27`, `handle_materialize.rs:27,100`,
`handle_tag.rs:32`, `conf_gate.rs:296`, `gwz-cli dispatch.rs:23`) then fails to
compile until it decides. Stronger still: make the guard's *type* differ —
`MutationGuard` vs `PlanOnlyGuard` — and require the mutating git calls to take
`&MutationGuard`, so a dry-run path cannot reach a write at all. `handle_stash`
should stop ANDing on the op (`handle_stash.rs:47`) and gate each arm.

**Class "cannot honour it" — reject, don't absorb.** Adopt merge's pattern
(`merge/validate.rs:57-84`) tree-wide: a per-verb declaration of the policy
fields it honours, and a shared validator that returns `InvalidRequest` for any
other field that is `Some`. `clone` already does this for dry-run
(`invocation.rs:13-15`) but by hand, in the CLI, for one verb. This turns DR-7
from a silent lie into an error message, and turns §3's `N` cells into either
`H` or `R` with no third option.

**Class D1 (id collisions) — assert at build time.** The collision set is
computable from `Cli::command()`: walk the command tree, and for each
subcommand compare its own argument ids against the global argument ids. A
`#[test]` in gwz-cli that asserts the intersection is empty (with an explicit
allow-list, initially containing `all` with a dated exemption) makes the class
extinct and makes DR-5's eventual fix — renaming `StageArgs.all`/`CommitArgs.all`
to `stage_all`/`commit_all` with `long = "all"` preserved — checkable. The rename
is the actual fix for DR-5; diagnosis §6 deferred it as "requires renaming a long
option", but only the *id* needs to change, not the long name.

**Class "docs promise, code silent" — generate the matrix, don't write it.**
`gwz-core/docs/OperationModel.md:18`'s "where supported" should be replaced by a
generated table: a per-verb capability declaration in core (the same declaration
the rejection validator uses), emitted into the docs and asserted against by a
test. `scripts/generate_cli_reference.py` should read that same declaration and
stop emitting the Global Options block for flags the verb rejects — that one
change closes §4a's three rows (clone/forall/merge) at the source rather than by
adding prose exceptions to `Concepts.md`. Then `Concepts.md:101` can stop enumerating exceptions by hand and
`RootWorkspace.md:147`'s "and other mutations" can name them.

**Test shape.** Every `H` cell in §3 needs (a) a core test asserting the effect,
and for dry-run a **negative** assertion over concrete state (stash list, index
digest, HEAD, file existence), and (b) a CLI test through the parser in **both**
flag positions. The g01 tests added by F1 cover one direction of the `all`
collision; the mirror direction (DR-5) has none.
