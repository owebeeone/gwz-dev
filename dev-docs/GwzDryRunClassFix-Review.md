# GwzDryRunClassFix — Single-Axis Review (behaviour + seam)

- **Date:** 2026-09-02
- **Candidate:** gwz-core `fix/dry-run-class` @ `22f388d` (parent `9c808ff`); gwz-cli `fix/dry-run-class` @ `d26ce35` (parent `4791eb6`)
- **Axis:** does the claimed dry-run class actually stop mutating, and does the seam hold?
- **Method:** reading + five scripted end-to-end probes through the candidate binary and
  the pre-fix binary. No test suites, no clippy, no full builds (lane owner's landing
  verification runs those once).
- **Binaries:** candidate `…/scratchpad/fix-target/debug/gwz` (`gwz 0.2.0-dev`, mtime
  Sep 2 22:49); pre-fix `/Users/owebeeone/limbo/gwz-dev/target/debug/gwz` (`gwz 0.2.0-dev`,
  mtime Sep 2 22:14).
- **Verdict:** **GO-WITH-CONDITIONS** (four conditions, all text/notes, no code behaviour change).

---

## A. The five probes

Fixture per probe: a fresh source repo with one commit, `gwz init file://<src>` into an
empty dir, giving a root + one member. Every probe was run in **both** flag positions —
`gwz --dry-run <verb>` and `gwz <verb> … --dry-run`. Scripts:
`…/scratchpad/probe.sh`, `…/scratchpad/probe2.sh`.

Two false starts are recorded for honesty: the first pass used the wrong stash-id prefix
(`st_` vs `stash_`) and `stash push --target @all` (rejected — "selected command does not
support @root"), so probes (a)/(b) aborted at `InvalidRequest` and proved nothing. Both
were corrected and re-run; only the corrected transcripts are cited below.

### (a) `--dry-run stash drop <id>`

| | pre-fix | candidate |
|---|---|---|
| native `git stash list` before | `stash@{0}: On main: gwz:stash_…: probe` | same |
| native after | **`[]` — DESTROYED** | `stash@{0}: … probe` — intact |
| `gwz stash list` rows before/after | 1 → **0** | 1 → 1 |
| bundle-file digest before/after | n/a (gone) | unchanged |
| status line | `status: Ok` (real drop) | `status: Accepted` + member row `Planned` |
| control: real drop afterwards | `IoError: No such file or directory` | rows 1 → 0, works |

Identical in both flag positions. **PASS.** The audit's DR-1 reproduction is confirmed on
the pre-fix binary and closed on the candidate; note the pre-fix control shows the damage
was not merely "mutated" but left the workspace unable to perform the real drop.

### (b) `--dry-run stash apply` / `pop`

| | pre-fix | candidate |
|---|---|---|
| worktree before apply | clean, `f.txt` = `hello` | same |
| worktree after `--dry-run apply` | **` M f.txt`, `hello\nchanged` — RESTORED** | clean, `hello` |
| `--dry-run pop` | `StashIncomplete: stash bundle has no eligible members` (state already consumed by the prior dry-run apply) | `Accepted` + `Planned`; tree and native stash unchanged |
| control: real apply afterwards | `StashIncomplete` — broken | ` M f.txt` — works |
| `--dry-run stash list` | `Ok` + rows | `Ok` + rows — **still works** |

Identical in both flag positions. **PASS.** DR-2 confirmed and closed. `list` remains
ungated under dry-run, as required.

### (c) `--dry-run forall -- sh -c 'touch PROBE_X'`

- pre-fix: prints `=== <member> ===`, **`PROBE_X` files created: 1** — the command ran. Both positions.
- candidate: `dry run: would run \`sh -c touch PROBE_X\` in 1 target(s); nothing was executed`
  followed by the target list; **`PROBE_X` files created: 0**. Both positions.

**PASS.** DR-3 confirmed and closed. The plan names the command and the targets in order.

### (d) `--dry-run init`, and `init --update`

- pre-fix, empty dir: `status: Ok`, `ls -A` → `.claude .git .gwz AGENTS.md AGENTS_GWZ.md gwz.conf …`
  — **the whole workspace was created**. Both positions.
- candidate, empty dir: `status: Accepted`, `would create workspace 'ws_default' at <path>`,
  **`ls -A` empty**. Both positions.
- `init --update` under dry-run (control-backed): a *real* `init --update` after deleting
  `AGENTS_GWZ.md` recreates it (`present after REAL update? YES`); under `--dry-run` in
  **both** positions it is **not** recreated. So the update shape honours dry-run — but see
  **P3-3** on what it *prints*.

**PASS.** DR-4 confirmed and closed.

### (e) `gwz --all add|commit` vs `-A` / `-a`

| invocation | pre-fix | candidate |
|---|---|---|
| `gwz --all commit -m x`, unstaged tracked mod | member HEAD **moved**, tree clean → acted as `-a` | HEAD **unchanged**, `M f.txt` still dirty, `status: Ok`, `@root . Ok` |
| `gwz commit --all -m x` (post-position) | (same collision) | HEAD unchanged, still dirty, **exit 0, `status: Ok`** |
| `gwz commit -a -m y` | — | HEAD moves, tree clean |
| `gwz commit -a --all -m y` | — | HEAD moves — the two compose |
| `gwz --all add` (untracked in root + member, no pathspec) | **exit 0**, `status: Ok`, staged `member_new.txt` + `root_new.txt` | **exit 1**, `InvalidRequest: nothing specified to stage; pass pathspecs or -A` |
| `gwz add --all` (old muscle memory) | exit 0, staged everything | **exit 1**, same error |
| `gwz add -A --target @all` | — | `status: Ok`, member staged |

**PASS on the fix; see P2-1 on the contract.** Answer to the posed question: `gwz --all add`
with no pathspec is now **an error** (exit 1, `InvalidRequest`), not a no-op and not a
stage-all. It is documented — `docs/commands/add.md:19` records `--all` as the selector and
`-A` as the staging flag, and the hint text itself was retargeted to `-A`
(`gwz-core/src/workspace_ops/stage_routing.rs:49`).

---

## B. The seam

`gwz-core/src/workspace_ops/merge/runtime/mutation_guard.rs` — `acquire_workspace_mutation_guard`
now takes `dry_run` and returns `WorkspaceMutationAccess { Mutating(g) | PlanOnly(g) }`.
`reconcile_authority(guard, dry_run) = guard.filter(|_| !dry_run)`
(`workspace_bootstrap/conf_gate.rs:72-77`), so `reconcile_authority(Some(&g), dry_run)`
and `access.writes()` are **the same function of the same input** at every migrated site,
provided the same `dry_run` expression is passed to `acquire`. Read at each site — it is.
Every caller checked:

| caller | before | after | equivalent? |
|---|---|---|---|
| `handle_commit.rs:37` | `reconcile_authority(Some(&_guard), dry_run)` | `access.writes()` | yes |
| `handle_materialize.rs:27` (snapshot) | same | `access.writes()` | yes |
| `handle_materialize.rs:101` (capture) | same | `access.writes()` | yes |
| `handle_tag.rs:32` | `List` → no guard, no gate; mutate → `reconcile_authority(_guard.as_ref(), dry_run)` | `if let Some(access) = _access.as_ref() { … access.writes() }` | yes; `List` arm untouched |
| `handle_stage.rs:27` | `_guard`, root only, no conf gate | `_access`, root only, no conf gate | yes (rename only) |
| `conf_gate.rs:296` (test helper) | guard | `…, false).into_guard().expect(…)` | yes |
| `guarded_workspace_root` (`mutation_guard.rs:120`) | guard | `…, false).into_guard().expect(…)` | yes; dry-run still early-returns `(None, root)` **before** the lock |
| gwz-cli `globalargs/dispatch.rs:26` (forall) | no dry-run | `meta.dry_run.unwrap_or(false)` | new, correct |

The merge lifecycle's own guards are untouched (grep over `src/workspace_ops/merge/` shows
no changed call site besides `mutation_guard.rs` and its test).

`handle_stash.rs:37-70` gates every arm correctly: `List` → `resolve_workspace_root`, no
guard, no conf gate (inspectable when damaged); every other op → `guarded_workspace_root(…,
dry_run)`, which under dry-run takes **no** mutator lock and yields `None` authority, and the
conf gate still runs (gated on the op, not the guard) so a hand edit is still refused.
Push's dry-run early return is pre-existing (`handle_stash/commands.rs:58`); apply/pop/drop's
is the new one (`commands.rs:243`), placed **after** bundle resolution, selection checking and
`restore_plans` — so a dry run still reports a dirty destination or a missing native payload
as an error rather than a rosy plan. Correct placement.

**Can a caller reach a write with `PlanOnly`? Yes — see P3-1.** The type gates the
*reconcile* authority, not filesystem writes.

---

## C. The `--all` decision

**Clap panic claim: NOT reproduced (would need a build); read and judged plausible.** The
builder quotes `Command add: Long option names must be unique for each argument, but '--all'
is in use by both 'stage_all' and 'all'` at `clap_builder-4.6.0 debug_asserts.rs:110`. That
is consistent with clap's global propagation: a `global = true` root arg is cloned into every
subcommand, and once the ids differ the two `--all` longs coexist in one `Command` and trip
the uniqueness assert. The evidence the builder points at is real and checkable: the
regenerated `docs/CLI.md` now renders the global `--all` **under** `add` and `commit`
(hunks at CLI.md:206-211 and 631-636), which only happens once propagation is no longer
suppressed by a shared id. I accept the claim. Marked **UNVERIFIED by execution**.

**Contract change.** `--all` under `add`/`commit` flipped from "stage everything" to
"select all workspace targets". Documented in all three places:
`docs/commands/add.md:19`, `docs/commands/commit.md:33`, and the regenerated `docs/CLI.md`
(`-A` / `-a` help strings now carry "`--all` is the target selector"). `generate_cli_reference.py
--check` re-run by me: **exit 0**.

**Can the old meaning be typed by accident?** Yes, and the two verbs differ sharply:

- `add --all` / `--all add` → **exit 1** with `InvalidRequest: nothing specified to stage;
  pass pathspecs or -A`. Loud. Low risk (**P3**).
- `commit --all -m x` / `--all commit -m x` → **exit 0, `status: Ok`**, and it commits
  *whatever is already staged across all targets* while silently leaving tracked
  modifications uncommitted. No error, no warning, no deprecation notice. **This is the
  risk (P2-1).**

**Rating: P2.** It is a silent semantic change in a committing verb, reachable by muscle
memory, that can leave a user believing work is committed when it is not — and, when
something *is* staged, produces a partially-correct commit rather than a clean failure.
**The release notes must call it out**, naming `commit --all` explicitly and giving `-a` as
the replacement. (`docs/Releases.md` and `RELEASE.md` exist; the note belongs with them.)

---

## D. The collision test

`gwz-cli/src/tests/g01.rs` — `no_subcommand_argument_reuses_a_global_argument_id`.

- **Recursive: yes.** It seeds a `pending` stack from `command.get_subcommands()` and, for
  each popped node, pushes `nested.get_subcommands()` with a dotted path — so `repo add`,
  `stash push`, `merge …` and any depth below are walked.
- **Allow-list: empty** (`const ALLOWED: &[(&str, &str)] = &[]`), with a comment saying it
  must stay empty.
- **Non-vacuous:** it first asserts the global id set actually contains `all`, so renaming
  the global out from under the test fails loudly instead of passing trivially.
- **Skips propagated globals** via `arg.is_global_set()`, so only subcommand-declared args
  are compared — the right predicate.
- **Would it have caught the original?** Yes. `CommitArgs.all` and `StageArgs.all` were
  subcommand-declared, non-global, with derived id `all`, matching the global `all` →
  reported as `commit.all` / `add.all`.

One scope note (informational, not a finding): the test compares **ids**, not long/short
spellings. A future arg declaring `long = "all"` under a distinct id is caught only by clap's
own `debug_assert`, which runs in debug builds — which is where the suite runs, so it is
covered in practice, but the class the test extinguishes is id-reuse specifically.

---

## E. Tests

**Behavioural over concrete state — yes.** `g20.rs` asserts native `stash_list` length and
the stash **object id**, `bundle_path(...).exists()`, and the bundle's
`restore_state == Pending` / `push_lifecycle == Saved`, plus `README.md` bytes via
`assert_text_eq`. `dry_run_stash_drop_destroys_nothing` additionally performs the **real**
drop afterwards and asserts both the native stash and the bundle file are gone — so the plan
is proved not to have poisoned state. `g02.rs::dry_run_create_workspace_creates_nothing`
asserts the directory is empty. `mutation_guard.rs` test asserts `is_dry_run()`,
`writes().is_none()`, and `root()` on the plan-only arm. None of these assert response shape
in place of state.

**Both flag positions — yes, verified by reading the loops** in
`gwz-cli/tests/local_workflows.rs`: each test iterates `("global-first", …)` /
`("verb-last", …)` and in the verb-last branch appends `--dry-run` *after* the subcommand and
its operand (`command.args([op, stash_id]); if position == "verb-last" { command.arg("--dry-run") }`).
Genuinely both. The e2e stash test also compares the `stash_bundles` JSON before/after each op.

**Weaker-than-claimed checks:** `assert_success` is exit-status only, so the "Accepted"
status and `Planned` member rows are asserted only in the core tests, not in the e2e ones;
the forall e2e test compensates with a `contains("dry run:")` assertion on stdout. Acceptable
— the e2e layer's job here is absence-of-mutation, which it asserts over real state. No test
found that claims more than it checks.

---

## F. Pins

| pin | result |
|---|---|
| `check_checked_artifact_boundaries.py` (run by me, core worktree) | **exit 0** — `checked-artifact boundary: ok (18 visible entries, 8 classified modules)` |
| raw-writer primitive added/removed in an annotated file | **no.** `handle_stash/commands.rs` adds an early `return Ok(stash_response(…))` only; `handle_stage.rs` is a binding rename; `handle_create_repo.rs` adds an early return *above* `ensure_workspace_git_repo` and touches no writer. Boundary checker green corroborates. |
| two `mod.rs` compiler-root digests | **untouched** — zero `mod.rs` paths in the core commit's stat |
| `docs/CLI.md` regenerated | `generate_cli_reference.py --check` **exit 0** (run by me); the 15-line diff is exactly the two help-string edits plus the global `--all` block appearing under `add` and `commit` — sane |
| gwz-cli `Cargo.lock` | **only** `gwz-core version = "0.12.0"` → `"0.12.1"`. Full `+`/`-` line dump is those two lines and nothing else |
| trailers | **none** on either commit — no `Co-Authored-By`, no `noreply@anthropic`, no "Generated with" |
| worktrees byte-clean | `git status --porcelain` empty in both at review start and at review end; I made no writes in either |

---

## Findings

### P2-1 — `commit --all` silently changes meaning, with no runtime signal
**Site:** `gwz-cli/src/clirequest/repo.rs:141-150` (`commit_all`), `src/clirequest/invocation.rs:52`.
`gwz commit --all -m x` and `gwz --all commit -m x` previously staged tracked modifications
(`git commit -a`); they now select `@all` and commit only what is already staged, returning
exit 0 / `status: Ok`. Probe (e) transcript above: HEAD unchanged, `M f.txt` still dirty,
success reported. The `add` half of the same change fails loudly (exit 1) and is P3; the
`commit` half is silent. Correctly documented in `add.md`, `commit.md` and `CLI.md`, but
documentation does not reach a user retyping a habit.
**Disposition:** the change itself is right — it restores the contract `globalargs/parser.rs`
and every `CLI.md` Global Options block already promised, and closes audit §4 row 6 / §4b
item 18. **Condition: the release notes must name `commit --all` explicitly and point at `-a`.**

### P3-1 — the seam's doc-comment claims more than the type enforces
**Site:** `gwz-core/src/workspace_ops/merge/runtime/mutation_guard.rs:25-31` ("a plan-only run
gets a root to read and nothing to write with") and the commit message's "write authority is
only reachable through `writes()`/`into_guard()`".
`WorkspaceMutationAccess::root()` returns the path in **both** arms, and every real
filesystem/git write in the handlers goes through that root via the backend, not through the
guard. `writes()` feeds exactly one thing: `assert_conf_unmodified_for`'s reconcile-authority
parameter. `handle_stage.rs:27` is the live demonstration — it binds `_access`, never calls
`writes()`, and behaves identically under dry-run. The accurate claim is the commit message's
*other* one: a handler **cannot compile without stating** dry-run. That is real and valuable;
"nothing to write with" is not.
**Condition: soften the doc comment to what the type enforces.**

### P3-2 — false comment about the mutator lock in the forall guard
**Site:** `gwz-cli/src/globalargs/dispatch.rs:21-23` (added): "the guard is taken with the
request's dry-run answer, **so a planned run never takes the mutator lock**".
It does take it. `acquire_workspace_mutation_guard` acquires `lock` *before* branching and
stores it in the guard held by **both** arms — deliberately, per `mutation_guard.rs:25-31`.
Only `guarded_workspace_root` skips the lock under dry-run, and `forall` does not go through
it. Behaviour is fine (lock held, nothing spawned); the comment misleads about lock
semantics, which is precisely the thing a future reader must not be wrong about. The same
sentence appears in the commit message.
**Condition: correct the comment.**

### P3-3 — `init --update` under `--dry-run` reports a write it did not make
**Site:** the `init --update` path (`workspace_bootstrap.rs`), not touched by this change.
Under `--dry-run`, in both flag positions, it prints `status: Ok` and
`created workspace agent bootstrap files` while creating nothing (control: a real
`init --update` does recreate `AGENTS_GWZ.md`; the dry run does not). **Pre-existing —
byte-identical on the pre-fix binary — and outside DR-4's scope**, so not a regression and
not a blocker. But it is a false success report in the same verb DR-4 just fixed, and it
contradicts the new `gwz --dry-run init`, which correctly answers `Accepted` +
"would create workspace…".
**Condition: file it as a new audit row (DR-11) alongside DR-6..DR-10.**

### P3-4 — unreachable `expect` introduced (accepted)
`mutation_guard.rs:120-122` and `conf_gate.rs:298-300` use
`.into_guard().expect("a non-dry-run acquisition always yields the mutating arm")`. Sound by
construction (the literal `false` is two lines above), and the message is honest. Noted, not
actionable.

---

## What I did NOT verify

- **Test suites, clippy, full builds.** Out of scope by instruction. I was **not given the
  builder's exit codes** to cite, so the claim "they were green at this byte-identical tree"
  is **UNVERIFIED** here and must be confirmed by the lane owner's landing run.
- **The clap panic** (§C) — read and judged plausible from the builder's quoted panic with
  file:line plus the corroborating `CLI.md` propagation evidence, but **not reproduced**.
- **`docs/CLI.md` in full** — I read the 15-line diff and re-ran `--check` (exit 0); I did not
  proofread the whole regenerated document.
- **DR-6..DR-10** — explicitly out of this change's scope; not probed.
- **Multi-member and multi-target fixtures** — every probe used one root + one member.
  Conflict/partial-selection paths under dry-run are unprobed.
- **Windows / non-`sh` forall paths** — probed on macOS with `sh -c` only.

---

## Verdict

**GO-WITH-CONDITIONS** — all five DR items reproduce as broken on the pre-fix binary and are
closed on the candidate, in both flag positions, with working controls; the seam is
behaviour-preserving at every migrated caller and the pins are green. Conditions for the lane
owner to fold at landing: (1) release notes must call out the `commit --all` contract change
naming `-a` as the replacement (P2-1); (2) correct the "never takes the mutator lock" comment
in `dispatch.rs` (P3-2); (3) soften the `mutation_guard.rs` "nothing to write with" claim to
what the type enforces (P3-1); (4) file the `init --update` dry-run false report as a new
audit row (P3-3). None require a code-behaviour change.
