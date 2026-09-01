# GwzWt — Workspace Worktrees: Design Review

- **Object under review:** `gwz-dev/dev-docs/GwzWtDesign.md` (status DRAFT,
  "GwzWt — Workspace Worktrees: Requirements & Design").
- **Date:** 2026-09-01.
- **Tier / mode:** single-axis, peer-blind, Opus. No second axis. Gates whether
  the design proceeds to planning.
- **Supporting documents read:** `gwz-core/dev-docs/GWZRequirements.md` (row
  conventions), `gwz-cli/dev-docs/GwzLogRequirements.md` (envelope-row idiom),
  `gwz-cli/dev-docs/GwzStashSpec.md` (coordinated-verb style),
  `gwz-cli/dev-docs/GwzLog-S0.1-Review.md` (review bar and finding format),
  `gwz-cli/dev-docs/GwzLog-S2.1-Review.md` + `GwzLogPlan.md` §1.10 (placement
  record).
- **Tree read (read-only), gwz-core at HEAD:**
  `src/workspace_ops/handle_materialize.rs`,
  `src/workspace_ops/materialize_preflight.rs`,
  `src/workspace_ops/sync_workspace_boundary.rs`, `src/workspace/mod.rs`,
  `src/git/gitbackend/repository.rs`, `src/operation/workspace_mutator_lock.rs`,
  `src/operation/mod.rs`, `src/status/member_not_materialized.rs`,
  `src/checked_artifact/bootstrap/runtime/paths.rs`,
  `src/checked_artifact/bootstrap/runtime/catalog_lease.rs` and
  `catalog_lease/{association,target}.rs`,
  `scripts/checks/check_checked_artifact_boundaries.py`, plus the vendored
  `git2-0.21.0` / `libgit2-sys-0.18.8+1.9.7` sources.
- **Probes run (throwaway fixtures only, under scratch):** `git worktree`
  add/lock/remove/list envelope probes on a scratch repo (git 2.52.0);
  `info/exclude` sharing and `ENOTDIR` reproduction; a YAML registry
  read-modify-write race simulation. Read-only inspection of the real
  `gwz-m0-worktrees/m0-*` composites and `git worktree list` in `gwz-dev` and
  `gwz-dev/gwz-core`.
- **Nothing was built, mutated, committed, pushed or tagged.** No real repository
  was written to. The only file this review creates is itself.

---

## Verdict

# NO-GO

**Narrow and bounded.** The charter is right and the central bet is sound:
worktrees genuinely are a second materialization strategy for the existing
workspace model, the pain is real and evidenced on disk, and §1, §2, §5.2, §5.4
and §5.6 survive with edits. Three things must be rewritten before planning:

1. **§3 self-consistency / W-CMD-6.** The claim that existing verbs already work
   unchanged inside a linked root is **false in the tree**, at a single
   production call site that nearly every mutating verb reaches. It is new code,
   not an audit obligation. **[P0 F1]**
2. **W-LIF-2 / §5.5 derived completeness.** The completeness derivation is not
   well-founded: it cannot distinguish an interrupted `add` from an ordinarily
   partial workspace, a state gwz supports as first-class. As written it would
   make every gwz verb refuse inside `gwz-m0-worktrees/m0-a` — one of the two
   artifacts §1 cites as its motivating evidence. **[P0 F2]**
3. **§5.3 backend.** The claim is **inverted**. `git2` cannot create a detached
   worktree at all — and detached/pinned is the v0 *default* under W-CMD-1 and
   W-ENV-4. Meanwhile `prune`, named as thin, is the best-supported operation in
   the crate. **[P1 F3]**

The design's own §5.1 instruction — "the reviewer should verify this seam claim
against the code rather than trusting it" — was the right instinct and is
honoured below. The seam is real; its *failure model* is the opposite of what
the rows assume. **[P1 F4]**

**Finding counts:** P0 × 2, P1 × 4, P2 × 7, P3 × 4 (17 total).

**Re-review scope on revision:** §3, §5.3, §5.5, W-CMD-2, W-CMD-4, W-CMD-6,
W-LIF-1..3, W-ENV-1, W-ENV-6, W-SAF-2, W-SAF-4. Sections §1, §2, §5.2, §5.4 and
§5.6 need only the edits named in P2/P3 and should not be re-reviewed whole.

---

## Claim-verification table

The five claims the review was chartered to check, each graded against the tree
or against a probe.

| # | Claim | Grade | Evidence |
| --- | --- | --- | --- |
| 1 | **§5.1 seam** — `gwz clone`/`gwz materialize` share member-creation machinery a worktree strategy can slot into | **OVERSIMPLIFIED** | The slot is real: `MaterializePlan.clone_url: Option<String>` (`materialize_preflight.rs:12`) is the strategy discriminator, consumed at `handle_materialize.rs:282-286`; `clone_workspace_with_emitter` already re-enters the same machinery with a different root (`handle_materialize.rs:545-560`). But it is an ad-hoc `Option<String>`, not a strategy abstraction; the plan carries no *source repository* (clone's source is a URL, a worktree's is a path in the primary — a second root the machinery does not carry); the root is created outside the seam entirely (`handle_materialize.rs:509`); and the failure model is reject-partial-with-`remove_dir_all`, not the push model the rows assume. See **[P1 F4]**, **[P2 F12]**. |
| 2 | **§5.3 backend** — git2 solid for add/list, thin for prune/repair | **WRONG (inverted)** | `list` native (`repo.rs:3356,3370`); `prune` native with `valid/locked/working_tree` flags (`worktree.rs:124,134,217-231`) — *not* thin. `add` **cannot produce a detached worktree**: `WorktreeAddOptions::reference` rejects any non-branch ref (`libgit2/src/libgit2/worktree.c:331-346`), and omitting it silently creates a branch named after the worktree (`worktree.c:342-345`). `checkout_options` is unreachable in git2 0.21.0 (no setter; `raw()` is `*const`). `repair`: zero occurrences of the string in git2 `src/` or libgit2's public headers — genuinely absent. See **[P1 F3]**. |
| 3 | **§5.2 module home** — crate-root manifest + source-loading-edge inventory pinned; `operation/<child>/` sanctioned | **ACCURATE in conclusion, OVERSIMPLIFIED in mechanism** | The *conclusion* is backed by a dated operator ruling (`GwzLogPlan.md` §1.10, 2026-08-29) and a reviewed GO (`GwzLog-S2.1-Review.md:36-44,162`); the placement commit `c5438a5` is in **gwz-cli**, docs-only. But there is no "module manifest": `PROTECTED_COMPILER_ROOT_DIGESTS` (`check_checked_artifact_boundaries.py:20-24`) is a whole-file SHA-256 of `src/lib.rs`, which cannot tell a new module from a comment fix. `APPROVED_RUST_PATH_EDGES` (`:71-151`, set equality at `:1122`) is a literal inventory but covers only `#[path]`, not ordinary `mod`. See **[P2 F7]**. |
| 4 | **§3 self-consistency** — a root worktree carries `gwz.conf` and existing verbs resolve against it | **WRONG** | Discovery itself is safe (`discover_workspace_root` keys on `gwz.conf/gwz.yml`, `workspace/mod.rs:88`; `is_repository` uses `git2::Repository::open`, gitfile-aware, `repository.rs:8`). But `workspace_exclude_path(root) = root.join(".git").join("info").join("exclude")` (`sync_workspace_boundary.rs:89`) traverses `.git` **as a directory**. In a linked worktree `.git` is a regular file — probe returns `ENOTDIR (errno 20)`, which the `NotFound`-only handler at `:75-79` does not absorb. It is the single production `.git`-as-directory assumption in the crate, and it is on the finishing path of nearly every mutating verb. See **[P0 F1]**. |
| 5 | **Evidence** — `gwz-log-worktrees/` and `gwz-m0-worktrees/` on disk; `gwz.conf/` tracked, `.gwz/` untracked | **PARTLY WRONG (half stale) / registry claim ACCURATE** | `gwz-m0-worktrees/{m0-a,m0-b1,m0-c1,m0-c2,m0-core-int}` exist and are exactly the described shape (root `.git` is a 59-byte gitfile; members are worktrees of the primary's per-member repos). **`gwz-log-worktrees/` does not exist anywhere under `/Users/owebeeone/limbo`.** Registry claim holds: `git ls-files gwz.conf` = 332 tracked (327 under `markers/`); `git ls-files .gwz` = 0, and `.gwz/` holds only `locks/workspace-mutator.lock`. See **[P2 F9]**, **[P3 F14]**. |

---

## Findings

### [P0 F1] §3 and W-CMD-6 are refuted by the tree: the workspace boundary write traverses `.git` as a directory

W-CMD-6 asserts every existing verb works inside a linked materialization
"identically to a primary" and that the row "is an audit-and-battery obligation,
**not new code**". §3 asserts "There is no 'linked mode' in the verbs." Both are
false at one call site.

`gwz-core/src/workspace_ops/sync_workspace_boundary.rs:88-90`:

```rust
pub(crate) fn workspace_exclude_path(root: &Path) -> std::path::PathBuf {
    root.join(".git").join("info").join("exclude")
}
```

In a linked worktree `<root>/.git` is a **regular file**. Probe (scratch fixture,
git 2.52.0):

```
$ ls -la linked/.git
-rw-r--r--  1 owebeeone  wheel  141 ... linked/.git
$ cat linked/.git
gitdir: .../wtprobe/primary/.git/worktrees/linked
$ cat linked/.git/info/exclude
cat: linked/.git/info/exclude: Not a directory
$ python3 -c "open('linked/.git/info/exclude').read()"
errno 20 ENOTDIR NotADirectoryError(20, 'Not a directory')
```

The reader at `sync_workspace_boundary.rs:75-79` special-cases only
`ErrorKind::NotFound`; `ENOTDIR` falls through to `return Err(io_error(error))`.
`write_atomic` at `:42` fails likewise. This is a hard error, not a soft skip.

**Blast radius.** `sync_workspace_boundary` / `ensure_workspace_exclude` /
`workspace_exclude_path` are reached from: `handle_init_from_sources.rs:203`;
`handle_materialize.rs:116,392,444` (capture, materialize, branch-materialize);
`handle_create_repo.rs:51,211,336,426`;
`pull_head_member_preflight.rs:186,484`;
`handle_repo_lifecycle.rs:153,229,342`; `handle_stage.rs:84`;
`handle_branch.rs:234`; `handle_commit.rs:209,213`; and the merge chain
`merge/finalize.rs:139,251,283`, `merge/publication.rs:40`,
`merge/abort/evidence.rs:335,683,767`. That is commit, branch, capture, pull,
stage, repo, materialize and merge — most of W-CMD-6's own enumerated list.

**The second half is semantic and survives the obvious fix.** `info/exclude`
lives in the **common** directory and is shared by every worktree — repointing
the path at `--git-common-dir` does not give a composite its own file:

```
$ git -C linked check-ignore -v taut/f
.../primary/.git/info/exclude:2:/taut/  taut/f
$ git -C linked rev-parse --git-dir --git-common-dir
.../primary/.git/worktrees/linked
.../primary/.git
```

So primary and every composite would rewrite one managed block. Because a
composite may sit on a different branch with a different `gwz.yml`, the block's
member set differs and the writers ping-pong. Worse, the **merge machinery hashes
that exact file as evidence** (`merge/publication.rs:40`,
`merge/abort/evidence.rs:767`, `merge/finalize.rs:283`) — a composite's boundary
sync can invalidate an in-flight merge's evidence in the primary. That also
contradicts **W-SAF-2**, whose declared write set is "git per-repo admin areas,
the composite's paths, and the primary's `.gwz/`": writing the primary's shared
`info/exclude` is a write to the primary, read back as merge evidence.

**Required:** §3 and W-CMD-6 must stop claiming zero new code. The design must
name (a) gitfile-safe resolution of the git directory, (b) an explicit rule for
which materialization owns the managed exclude block — per-worktree
`info/exclude` does not exist in git, so the honest options are *primary-only
writes it*, or *`extensions.worktreeConfig` plus a per-worktree mechanism*, or
*derive the boundary without a shared file* — and (c) the merge-evidence
interaction. This is a design decision, not a battery.

---

### [P0 F2] W-LIF-2's derived completeness is not well-founded, and bricks the composites §1 cites as evidence

W-LIF-2: "a materialization is complete iff every manifest entry has its worktree
present at the composite path… Any OTHER gwz verb invoked at a derived-incomplete
root MUST refuse."

**Partial materialization is already a first-class, supported state in gwz.**
`observed_member_map` explicitly carries lock state for absent members rather
than failing — `handle_materialize.rs:660-663`: "A member that isn't materialized
can't be observed, so carry its existing lock state (AD3 b) — the
capture/snapshot stays complete and restorable rather than failing."
`src/status/member_not_materialized.rs` exists precisely to render such a member
as an ordinary response. So "not every manifest member is on disk" is a normal
workspace today, in primaries and composites alike.

**The derivation cannot tell the two apart.** An interrupted `add` and a
deliberately partial workspace produce byte-identical on-disk states. W-LIF-2
reads that one state as "incomplete → refuse everything".

**Proof by the design's own evidence.** `gwz-m0-worktrees/m0-a` — cited in §1 —
has its own `gwz.conf/gwz.yml` listing **seven** members (`mem_gwz_cli`,
`mem_gwz_core`, `mem_taut`, `mem_gwz_py`, `mem_taut_shape`, `mem_taut_shape_rs`,
`mem_taut_shape_py`) and its own `gwz.lock.yml` records `materialized: true` for
all seven. On disk it has **three**: `gwz-cli`, `gwz-core`, `taut-shape-rs`. Under
W-LIF-2 that is derived-incomplete, so after GwzWt ships every gwz verb inside
`m0-a` must refuse — a composite that works today — and the teaching exit it
names (`gwz worktree add <same path>`) would then create worktrees for `taut`,
`gwz-py`, `taut-shape` and `taut-shape-py` that the lane deliberately never
materialized. All five `m0-*` composites are in this shape.

This also collides with **OQ-1**: once `--target` subsets exist (v2), *every*
subset composite is permanently derived-incomplete. The rule cannot survive its
own roadmap.

**Required:** replace derivation-from-manifest with a signal that actually
distinguishes intent from interruption. The cheapest well-founded option is to
record the *intended constituent set* for the composite (the registry already has
a row; one field), making "incomplete" mean "intended-but-absent" rather than
"manifest-but-absent" — which also resolves the adoption question for
pre-existing composites (no row ⇒ no intended set ⇒ no refusal, `list` marks
them unmanaged). Whatever is chosen, W-LIF-2's refusal must never fire on an
ordinarily partial workspace, and the design must say what happens at the
`m0-*` composites by name.

---

### [P1 F3] §5.3 is inverted: git2 cannot create the v0 default (detached/pinned) worktree; prune is the well-supported one

Pinned: `git2 0.21.0`, `libgit2-sys 0.18.8+1.9.7` (bundled libgit2 1.9.7),
identical in `gwz-dev/Cargo.lock:406-409` and `gwz-core/Cargo.lock:291-294`;
declared `gwz-core/Cargo.toml:15`.

| Operation | §5.3 says | Reality |
| --- | --- | --- |
| `add`, branch case | solid | partial — works, with gaps below |
| `add`, detached/pinned case | solid | **impossible**; requires shell-out |
| `list` | solid | native (`repo.rs:3356,3370`; `worktree.rs:59,72,82,107`) |
| `prune` | thin | **native**, with `valid/locked/working_tree` (`worktree.rs:124,134,217-231`); only bulk-prune and `--expire` are absent |
| `repair` | thin | **absent** — zero hits for "repair" in git2 `src/` or libgit2 public headers |

The detached gap is the load-bearing one. `WorktreeAddOptions::reference` accepts
`Option<&Reference>`, and libgit2 hard-errors "reference is not a branch" for
anything that is not `refs/heads/*` (`worktree.c:331-346`); with no reference set
it takes `HEAD`'s commit and **creates a local branch named after the worktree**
(`worktree.c:342-345`), then writes HEAD as a symref to it (`worktree.c:406`).
There is no code path to a detached HEAD.

W-CMD-1's checkout precedence is "the named branch, **the snapshot's recorded
state, or the lock pins**" — the latter two are commits. W-ENV-4 says
"Detached/pinned members materialize as detached worktrees." So the *default*
`gwz worktree add <path>` with no `--branch`, and every `+snapshot`, is exactly
the case git2 cannot serve. The observed lane practice agrees this is the common
case: all five `m0-*` **roots** are `(detached HEAD)` per
`git -C gwz-dev worktree list`.

Three further gaps a builder will hit:

- **No `checkout_options` setter** in git2 0.21.0 (the C struct has the field;
  the crate exposes no setter and `raw()` is `*const`). Checkout is always
  `GIT_CHECKOUT_SAFE`: no force, and **no progress callback** — the existing seam
  emits `member_progress` through `clone_repo_with_progress`
  (`handle_materialize.rs:283-285`), which has no worktree analogue. See
  **[P3 F15]**.
- **Target directory must not exist** — `git_futils_mkdir(..., GIT_MKDIR_EXCL)`
  (`worktree.c:368`) — whereas the CLI accepts an existing *empty* directory.
  Probe: `git worktree add ../emptydir -b lane-empty` → succeeds;
  `../fulldir` (non-empty) → `fatal: '../fulldir' already exists`. W-ENV-1 refuses
  only "a non-empty existing directory", i.e. it matches the **CLI**, not git2.
  This also collides with W-LIF-2's resume (on resume the composite directory
  exists).
- **`Worktree` is neither `Send` nor `Sync`**, while execution runs through
  `par_map_per_host` (`handle_materialize.rs:274`).

**Note in the design's favour:** shell-out is *not* a new capability. gwz-core
already shells out under a named "AD1 per-primitive CLI fallback" discipline —
shell out, then self-verify through libgit2 — at `git/gitbackend/repository.rs:297-300`
(commit), `git/gitbackend/refs.rs:171-172,233` (tag), `operation/commit_log/mod.rs:320`
(rev-list), with the contract obligation at `git/gitbackend/contract.rs:812-816`.
What is missing is a central helper, environment sanitation and a `git`
availability probe. §5.3 should adopt AD1 by name rather than invent a new
"stated shell-out policy", and should record the per-operation decision **now**
(the section says the decision "is recorded in this section as the design
settles" — it is not recorded, and it is the one thing §5.3 exists to settle).

---

### [P1 F4] §5.1's seam is real, but its failure model is the opposite of what the rows assume

Verified accurate: `MaterializePlan { member_id, state, clone_url:
Option<String>, response }` (`materialize_preflight.rs:9-14`) is a genuine
per-member strategy slot, consumed at `handle_materialize.rs:282-286`; selection,
manifest/lock resolution, jobs-bounded execution (`par_map_per_host`,
`:274`) and per-member response construction are all reusable; and
`clone_workspace_with_emitter` already demonstrates re-entering the machinery
against a *different* root (`:545-560`). The design's architectural bet is sound.

What the rows get wrong:

1. **W-LIF-1 / W-CMD-2 assume an aggregating planner. `materialize_preflight` is
   first-error.** `materialize_preflight.rs:44-49` does `return Err(...)` on the
   first dirty member; it never collects. W-LIF-1 demands "collect EVERY refusal"
   and W-CMD-2 demands "ONE aggregated teaching refusal naming every conflicting
   repo and the path holding it". That aggregation is push's machinery, not this
   seam's — it is new code, and §5.1's "per-member planning… are the existing
   machinery, reused" oversells it.
2. **W-CMD-4 cites "the push model"; materialize is reject-partial with
   `rm -rf`.** `apply_materialize_plans` collapses every outcome to `first_error`
   (`handle_materialize.rs:353-368`) and then, per the F2/Q6 reject-partial rule,
   **deletes** the paths this operation created (`:370-378`,
   `std::fs::remove_dir_all`). Applied to worktrees this both contradicts
   W-LIF-2's "interrupted `add` leaves a derived-incomplete composite; re-running
   resumes" *and* leaves dangling per-repo worktree admin behind (a
   `remove_dir_all`'d worktree is prunable, not gone). The design must state
   which model `worktree add` uses; it currently cites both, one row apart.
3. **The root is created outside the seam.** `apply_materialize_plans` handles
   manifest members only; the root repository is cloned by a bespoke call at
   `handle_materialize.rs:509` before the manifest is even readable. For
   `worktree add` the root worktree must exist first (it carries `gwz.conf`,
   per §3), so §5.5 step 2's "for every constituent (root + Git members)" spans
   two different code paths, not one.
4. **`par_map_per_host` buckets on `git_host(clone_url)`** (`:277`). Worktree
   plans have no URL; the design should say the host bucketing degenerates (it is
   harmless — the jobs bound still applies — but it is a silent behaviour change
   the row set never mentions).

---

### [P1 F5] W-CMD-2's `--branch` has an unstated grammar

"`--branch <name>` MUST create-or-checkout `<name>` in the root and every member
worktree" leaves three things undefined:

- **What base does a *created* branch start from?** Not stated anywhere. W-CMD-1
  makes `--branch` outrank the lock pins, so the pins cannot be assumed as the
  base by implication. For the agent-lane pattern the answer almost certainly
  must be "the lock pin (or snapshot state) for that member", but a builder
  cannot infer it.
- **Create and checkout are two different git invocations with two different
  conflict errors,** and the row names only one. Probe:
  `git worktree add ../dup -b lane1` → `fatal: a branch named 'lane1' already
  exists`; `git worktree add ../dup2 lane1` → `fatal: 'lane1' is already used by
  worktree at '…/linked'`. The row's refusal condition ("already has `<name>`
  checked out in another worktree") is the second error only; the first is a
  different, non-fatal condition that `create-or-checkout` is supposed to absorb.
- **Per-repo divergence is undefined.** If `<name>` exists in `gwz-core` but not
  in `gwz-cli`, one repo checks out an existing branch at whatever commit it
  points to (possibly stale, possibly unrelated) while the other creates a fresh
  one. The composite silently lands on mixed states. The design should either
  require uniformity (refuse mixed) or define the mixed outcome.

Worth noting the cited evidence did **not** use one uniform branch: the `m0-*`
roots are detached while `gwz-core` members sit on `codex/m0-a-backend`,
`codex/m0-b1-planning`, `codex/m0-core-integration` or detached
(`git -C gwz-dev/gwz-core worktree list`). The design's uniform-branch model is a
change from the practice it presents as its evidence, and should say so.

---

### [P1 F6] W-LIF-3 and W-ENV-6 contradict each other; "last-writer-wins" on a YAML file destroys rows

W-LIF-3: "the registry tolerates concurrent appends with last-writer-wins because
it is advisory and prune-healable (W-ENV-6)". W-ENV-6: "Worst-case registry loss
costs a re-scan, nothing else."

A whole-file YAML read-modify-write does not merge — the loser's row is
**destroyed**, not superseded. Probe (two threads, each read → sleep → append own
row → write whole file):

```
worktrees:
- path: /B
  created_at: t
rows surviving: 1
```

Two successful `add`s, one surviving row. `prune` cannot heal this: prune's job
per W-CMD-5 is to *drop* rows whose composites no longer exist, and the lost row's
composite exists and is live. So "prune-healable" is false as stated.

Whether this is harmful depends entirely on a thing the design never states:
**what derivation enumerates the composite path set when the registry is
incomplete.** The answer is available and good — `git worktree list` on the
**root** repository enumerates exactly the root worktrees, i.e. the composite
paths. Verified against the real estate:

```
$ git -C /Users/owebeeone/limbo/gwz-dev worktree list
/Users/owebeeone/limbo/gwz-dev                       7c24141 [main]
/Users/owebeeone/limbo/gwz-m0-worktrees/m0-a         d3dac00 (detached HEAD)
/Users/owebeeone/limbo/gwz-m0-worktrees/m0-b1        d3dac00 (detached HEAD)
/Users/owebeeone/limbo/gwz-m0-worktrees/m0-c1        d3dac00 (detached HEAD)
/Users/owebeeone/limbo/gwz-m0-worktrees/m0-c2        d3dac00 (detached HEAD)
/Users/owebeeone/limbo/gwz-m0-worktrees/m0-core-int  d3dac00 (detached HEAD)
```

Name that as the enumeration authority and W-ENV-6's "worst case is a re-scan"
becomes true and W-LIF-3's clobber becomes genuinely benign. Leave it unstated
and both rows are unsupported.

**This raises a question the design should face rather than assume away:** if the
root repo's own worktree admin enumerates composites and `created_at` is the only
other field, §5.4's registry may not be needed at all — which would make GwzWt's
durable footprint *zero*, strengthening §2's "no new durable lifecycle
machinery". If the registry is kept (e.g. to carry the intended constituent set
that **[P0 F2]** requires), say what it holds that git does not. Either way this
belongs in the open questions. **See [P2 F17].**

---

### [P2 F7] §5.2 describes the wrong artifact (conclusion right, mechanism wrong)

There is no crate-root "module manifest". `PROTECTED_COMPILER_ROOT_DIGESTS`
(`gwz-core/scripts/checks/check_checked_artifact_boundaries.py:20-24`) is a
whole-file SHA-256 of `src/lib.rs`, `src/workspace_ops/mod.rs` and
`src/workspace_ops/merge/mod.rs`, compared flat at `:974-981`; it parses nothing
and cannot distinguish a new `pub mod` from a comment fix. The
source-loading-edge inventory is real and literal (`APPROVED_RUST_PATH_EDGES`,
`:71-151`, set equality at `:1122`) but covers only `#[path]` attributes, not
ordinary `mod` declarations. A new top-level module is out of bounds because all
three *load routes* into it are pinned (`lib.rs` digest, `#[path]` set equality,
`include!` ban at `:586-588`), not because top-level placement is itself checked.

The conclusion is well-supported: operator ruling of 2026-08-29 recorded in
`GwzLogPlan.md` §1.10 with both refusal strings quoted verbatim (byte-exact
matches for the checker's `:981` and `:1128`), reviewed GO at
`GwzLog-S2.1-Review.md:162`. Note `c5438a5` is a **gwz-cli, docs-only** commit
(162+39+16 lines across three dev-docs); it moved no code — the code home was set
by core commit `affaa69`.

Two things a planner needs that §5.2 omits: `operation/commit_log/` is the
**only** directory-shaped child of `operation/` (the "existing seam" is a
precedent of one), and the crate-wide scans at `:1213-1273` still constrain a new
child's symbol names. Also `run_r4bg_aggregate_gates.py` pins per-command test
counts, so adding tests can trip a gate marker.

---

### [P2 F8] W-SAF-4's "locks are per-materialization" is wrong for the checked-artifact catalog lease

W-SAF-4 states flatly that "Locks are per-materialization (each has its own
`.gwz/`)" and that no cross-materialization lock exists — "stated so nobody
builds a cross-composite lock". Half true.

- The workspace mutator lock **is** per-materialization: `.gwz/locks/workspace-mutator.lock`
  (`operation/workspace_mutator_lock.rs:6,56`), and the bootstrap guard is
  per-worktree — there is an existing test asserting exactly that,
  `linked_worktree_uses_its_actual_git_directory_for_the_guard`
  (`workspace_mutator_lock.rs:189-204`), which checks the guard lands in the
  linked worktree's own gitdir and **not** in the main repo's.
- The **checked-artifact catalog lease is anchored on the common directory**:
  `retain_repository_common_git_directory` →
  `Self::retain_git_directory(association.common_directory_path())`
  (`checked_artifact/bootstrap/runtime/catalog_lease/target.rs:126-131`), where
  `common_directory_path()` is `repository.commondir()`
  (`catalog_lease/association.rs:60-63,89-91`). One common dir is shared by the
  primary and every one of its linked worktrees, so a primary and all its
  composites contend on **one** catalog lease.

That is arguably the correct design (the catalog governs shared git state), but
W-SAF-4 as written gives a builder the wrong model and will make the resulting
serialisation look like a bug. Restate as: gwz's *workspace mutator* lock is
per-materialization; the *checked-artifact catalog* lease is per-common-directory
and therefore shared — by construction, not by omission.

Encouraging for the charter: this subsystem is already fully worktree-aware. It
distinguishes `repository.path()` from `repository.commondir()` throughout
(`catalog_lease/association.rs:56-63`,
`capability/pre_catalog/provider/retained.rs:201-253,388`) and already exercises
`Repository::worktree(...)` in tests
(`bootstrap/runtime/tests.rs:72`, `workspace_mutator_lock.rs:194`,
`catalog_lease/tests/association.rs:185,219,249,270`). §5.3 should cite this as
existing precedent instead of treating worktree support as greenfield.

---

### [P2 F9] Half of §1's evidence does not exist

§1 asserts in the present tense: "The evidence is on this machine:
`gwz-log-worktrees/s*/…` **and** `gwz-m0-worktrees/m0-*` are per-lane workspace
copies built by hand."

`gwz-m0-worktrees/` exists and is exactly as described. **`gwz-log-worktrees/`
does not exist** —
`find /Users/owebeeone/limbo -maxdepth 2 -type d -name "*worktree*"` returns only
`/Users/owebeeone/limbo/gwz-m0-worktrees`.

This is precisely the failure class this review was chartered against (the
gwz-log S0.1 P0: a doc status claim taken on trust). The surviving evidence is
strong enough on its own — five real composites, dated 2026-07-17, with root
gitfiles and per-member worktrees — so the fix is to correct the sentence, not to
weaken the argument. If the gwz-log lane's composites were removed after use,
that is itself a point *for* the charter (coordinated teardown) and worth saying.

---

### [P2 F10] Locked worktrees are ignored, and they are a real refusal surface

`git worktree lock` is exactly the removable-media/network-volume mechanism, and
it changes the behaviour of two v0 verbs. Probe:

```
$ git -C primary worktree lock ../linked --reason "removable media"
$ git -C primary worktree remove ../linked
fatal: cannot remove a locked working tree, lock reason: removable media
use 'remove -f -f' to override or unlock first
$ git -C primary worktree list
…/linked   95f1266 [lane1] locked
```

Consequences the design must state: **W-CMD-4** — `--force` is *not* enough for a
locked worktree (git needs `-f -f`), so `remove --force` on a partially locked
composite gives a partial failure the row does not anticipate; **W-CMD-5** —
`prune` must not silently unlock (git2's `WorktreePruneOptions::locked(true)`
would do exactly that, `worktree.rs:224`); **W-CMD-3/W-OBS-1** — `locked` with its
reason is per-repo health that `list` should show. Ignoring locks is defensible
for v0 *if stated* (refuse and teach `git worktree unlock`); ignoring them
silently is not.

---

### [P2 F11] `gwz clone` inside a linked materialization is not refused, and W-ENV-1's nesting rule is inconsistent with it

W-ENV-1 requires `worktree add` to refuse a path inside any existing
materialization. But gwz's two creation verbs already disagree about nesting:

- `init` walks parents and refuses (`preflight_create_workspace`,
  `workspace/mod.rs:112-121`, `ErrorCode::NestedWorkspace`).
- `clone` checks the **target only** (`handle_materialize.rs:476-481`) — it does
  not walk parents. So `gwz clone <url> ./sub` run inside a linked
  materialization creates a nested workspace unnoticed.

That is pre-existing behaviour, not caused by GwzWt, but W-CMD-6 claims every
verb "works… identically to a primary", which is true here in the unhelpful
sense: it is equally wrong in both. The design should state whether `gwz clone`
inside a composite is supported, refused, or out of scope, and whether W-ENV-1's
nesting check is a new shared helper both verbs adopt.

---

### [P2 F12] The reused preflight refuses members a worktree could serve

`materialize_preflight.rs:52` calls `first_remote_url(member)?` for any member
whose directory is absent, which errors when a member has no fetch remote. A
worktree needs no remote at all — the objects are already local. Reusing the
preflight unchanged would therefore refuse a purely local member that
`worktree add` can serve perfectly. Conversely `:41-50` refuses on a **dirty**
member, which for worktree creation is irrelevant (the source tree's dirtiness
has no bearing on a new worktree). Both are small, but they are exactly the
"reuse the existing machinery" assumption in §5.1 failing at the edges, and they
belong in the design rather than being discovered mid-build.

---

### [P2 F13] The §5.6 decision table is fair in direction but has one wrong cell and three omissions

Direction and honesty are good — the "Shared refdb" and "refdb damage reaches
everything" rows argue against the author's own preference, and the two-minds
framing is intact.

- **Wrong cell.** "Disk — Full trees; **objects hardlinked on same-FS local
  clones** (cheaper than reputed)". `gwz clone` clones members from a **URL**
  (`first_remote_url`, `materialize_preflight.rs:52`;
  `backend.clone_repo_with_progress(url, …)`, `handle_materialize.rs:283`), never
  from a local path, so git's local-clone hardlink optimisation never applies to
  `gwz clone`. The cell overstates clone's disk position in clone's favour.
- **Missing: git version floor.** Probed host is git 2.52.0, but the shell-out
  policy of **[P1 F3]** makes a floor load-bearing. `git worktree repair` needs
  ≥ 2.30; `worktree list --porcelain -z` needs ≥ 2.36; `worktree remove`
  ≥ 2.17. State the floor and the probe.
- **Missing: `extensions.worktreeConfig`.** Not set in a fresh repo
  (`git config --get extensions.worktreeConfig` → exit 1) and it is the only
  mechanism for per-worktree config. It is directly relevant to **[P0 F1]**'s
  boundary-ownership decision. Note also that config is otherwise **shared**:
  probe shows a `--local` value set in the primary is read in the linked worktree,
  and `origin` added in the primary appears in the linked worktree's
  `git remote -v`. That makes the table's "Remotes — Inherited from the primary,
  one config" accurate, and worth extending: push/pull from a composite uses the
  primary's remotes, credentials and refspecs, and writes remote-tracking refs
  into the **shared** refdb — so a fetch in one composite silently updates every
  other materialization's `origin/*`. That is the *desired* behaviour (§1's
  anti-staleness argument) but it should be named, because it is also how a
  `push --force` from a lane reaches the primary's refs.
- **Missing: locked worktrees** as a create/destroy asymmetry (**[P2 F10]**).

---

### [P3 F14] Tracked `gwz.conf/markers/` duplicates into every root worktree

`git ls-files gwz.conf` = 332 tracked files, **327** of them under
`gwz.conf/markers/` (the review charter said 317; recording the measured figure).
Every root worktree checkout materialises all of them in its working tree and
records them in its own per-worktree index. The object store is shared so the
*disk* cost is small, but the index and stat cost is per-composite and grows
monotonically with commit history, and each composite's `git status` walks them.
Not a correctness issue and not a reason to change anything; worth one sentence
in §5.6's Disk row so it is not discovered as a surprise.

---

### [P3 F15] Worktree add has no progress channel

The seam emits `member_progress` via `clone_repo_with_progress`
(`handle_materialize.rs:283-285`), and `EventEmitter` is threaded through the
whole apply path. git2 0.21.0 exposes no `checkout_options` setter on
`WorktreeAddOptions`, so a git2-backed worktree add can emit only
started/finished, never progress. Cheap to accept (worktree add is near-instant,
per §5.6) but W-OBS-2 and the protocol extension should say so rather than leave
a builder hunting for the callback.

---

### [P3 F16] W-ENV-1's non-UTF-8 clause is the only envelope edge stated as a convention rather than a behaviour

"Non-UTF-8 path bytes follow the estate convention: accepted where the OS accepts
them, rendered lossy." Every other envelope row names a testable behaviour; this
one names a convention and cites no source. Either point at the convention's
document or restate it as a row with a named test (Windows is already carrying a
first-dispatch obligation in W-ENV-4, and that is the platform where this bites).

---

### [P3 F17] Two questions were decided silently and belong in §6

The four stated OQs are the right ones and their defaults are defensible
(subset-deferral, `repair` as SHOULD, fail-closed on non-Git kinds, estate
migration out of scope — the last is correctly and firmly fenced). Two more
decisions were made in the prose without being surfaced:

- **OQ-5: does the registry exist at all?** §5.4 asserts it as settled. Given
  that the root repo's own worktree admin enumerates composites (**[P1 F6]**),
  its necessity is a genuine open question, and answering it "no" would make
  GwzWt's durable footprint zero — a strictly better outcome against §2's own
  stated goal. If **[P0 F2]** is resolved by recording an intended constituent
  set, the answer flips to "yes, and here is what it holds".
- **OQ-6: who owns the shared `info/exclude` managed block?** Decided by silence
  today (§3 assumes it just works). It cannot just work (**[P0 F1]**) and there
  is no per-worktree `info/exclude` in git, so this is a real fork:
  primary-only, `extensions.worktreeConfig`, or derive-without-a-shared-file.

---

## W-row buildability

Buildable = testable as written, with envelope edges defined, no unstated
grammar, no undefined degenerate input.

| Row | Buildable? | Note |
| --- | --- | --- |
| W-CMD-1 | **No** | Checkout precedence names snapshot/lock pins, i.e. commits — the case git2 cannot serve **[F3]**. Backend decision must land first. |
| W-CMD-2 | **No** | Unstated base for a created branch; create-vs-checkout dispatch and its two distinct git errors unnamed; per-repo divergence undefined **[F5]**. Aggregated refusal is new code **[F4]**. |
| W-CMD-3 | Partial | Health vocabulary (present/missing/moved/dirty) is testable, but `locked` is missing **[F10]** and the enumeration source is unnamed **[F6]**. Behaviour at pre-existing `m0-*` composites undefined **[F2]**. |
| W-CMD-4 | Partial | Dirty-refuse and idempotent re-run are testable. `--force` is insufficient for locked worktrees (`-f -f`) **[F10]**; "the push model" conflicts with the seam's reject-partial reality **[F4]**. |
| W-CMD-5 | Yes | git2-native (`is_prunable`/`prune`) once bulk iteration is written; "MUST NOT touch any live or dirty tree" maps to leaving `working_tree(false)` and `valid(false)`. Add: must not unlock. |
| W-CMD-6 | **No** | Refuted by the tree; it is new code, not an audit **[F1]**. Testable only once the boundary-ownership decision exists. |
| W-CMD-7 | Yes | Additive-only protocol extension with a drift check in both repos is the established L-PRO-1 discipline; testable as written. |
| W-CMD-8 | Yes | Thin `git worktree repair` delegation. Requires the git version floor **[F13]** since no native equivalent exists at any level **[F3]**. |
| W-LIF-1 | Partial | "Reject the whole request before any mutation" is testable; "collect EVERY refusal" is not the seam's behaviour and must be built **[F4]**. |
| W-LIF-2 | **No** | Completeness derivation is not well-founded and misfires on ordinary partial workspaces **[F2]**. Resume also collides with git2's must-not-exist target **[F3]**. |
| W-LIF-3 | **No** | "Last-writer-wins" destroys rows, and "prune-healable" is false for a live composite **[F6]**. Becomes buildable once the enumeration authority is named. |
| W-SAF-1 | Yes | House style; testable per refusal. |
| W-SAF-2 | **No** | Write set is violated by the shared `info/exclude` **[F1]**; restate once ownership is decided. |
| W-SAF-3 | Yes | Per-materialization conf-integrity follows from `assert_conf_unmodified_for` taking the resolved root; discovery is gitfile-safe (`workspace/mod.rs:88`). |
| W-SAF-4 | Partial | True for the workspace mutator lock, false for the checked-artifact catalog lease **[F8]**. Restate, do not drop. |
| W-ENV-1 | Partial | Matches the git CLI, not git2 (existing *empty* directory) **[F3]**; nesting check needs the composite path set **[F6]** and is inconsistent with `clone` **[F11]**; non-UTF-8 clause is a convention, not a behaviour **[F16]**. |
| W-ENV-2 | Yes | `git check-ref-format` as authority with git's reason carried is clean and testable. |
| W-ENV-3 | Yes | Fail-closed refusal naming member and kind; degenerate input (workspace with zero Git members) should be named but the rule is unambiguous. |
| W-ENV-4 | **No** | "Detached/pinned members materialize as detached worktrees" is unimplementable on the stated backend **[F3]**. Cross-filesystem and Windows halves are fine. |
| W-ENV-5 | Yes | Follows from §3 once **[F1]** is resolved; testable by running `+snapshot` in a composite whose `gwz.conf` differs from the primary's. |
| W-ENV-6 | Partial | Sound *if* the enumeration authority is named; unsupported as written **[F6]**. |
| W-OBS-1 | Partial | Needs `locked` **[F10]** and a defined rendering for unmanaged pre-existing composites **[F2]**. |
| W-OBS-2 | Yes | Schema tag `gwz.worktree/v0` and per-repo result records follow the house convention; note the absent progress channel **[F15]**. |

Totals: 8 buildable as written, 8 partial, 7 not buildable.

---

## Mandate item dispositions

- **Requirements quality.** Envelope rows are the document's strongest section
  and the S0.2 "mint at birth" lesson is visibly applied. Failure classes found:
  **unstated grammar** in W-CMD-2 (branch base, create-vs-checkout, per-repo
  divergence) **[F5]**; **lifecycle ambiguity** in W-LIF-2 **[F2]** and in
  W-CMD-4's conflicting failure models **[F4]**; **untestable MUST** in W-CMD-6
  (it asserts an absence of work that does not hold) **[F1]** and in W-ENV-4
  (asserts an unimplementable outcome) **[F3]**. **No trust-envelope leak found**
  — §2 is stated first, is genuinely same-user-cooperating, and no row quietly
  requires defending against the user's own adversarial acts. W-SAF-2's write-set
  claim is violated by accident, not by adversary **[F1]**.
- **Lifecycle soundness / crash windows.** Enumerated against §5.5: *before
  registry append* — no artifacts, nothing to heal, clean; *after append, before
  any worktree* — registry ghost, covered by W-CMD-5 prune and W-ENV-6; *mid-member*
  — **not covered**, this is where W-LIF-2's derived-incomplete state is
  indistinguishable from an ordinarily partial workspace **[F2]**, and where the
  reused seam's `remove_dir_all` rollback actively fights the resume rule
  **[F4]**; *after last member* — complete, clean. One further window the
  algorithm does not name: crash *between* the root worktree and the first
  member, which leaves a root carrying `gwz.conf` and no members — the state most
  likely to be mistaken for an intentional workspace.
- **Concurrent-add races.** Not safe as written: last-writer-wins on YAML
  destroys rows and prune cannot heal a live composite **[F6]**. W-ENV-6's
  "worst case is a re-scan" is *achievable* but currently unsupported — it needs
  the root repo's worktree admin named as the enumeration authority. Grade: the
  two rows contradict each other today; both become sound with one added
  sentence.
- **Locked worktrees.** Not fine to ignore silently; refusing and teaching
  `git worktree unlock` is an acceptable v0, but must be stated **[F10]**.
- **`gwz clone` inside a linked materialization.** Currently not refused (clone's
  guard is target-only, unlike init's parent-walking check) **[F11]**.
- **`gwz worktree list` at pre-existing `m0-*` composites.** Worse than confuse:
  under W-LIF-2 they become refusal sites for every verb **[F2]**. `list` itself
  would find them (git's admin enumerates them) but has no vocabulary for
  "unmanaged". Recommend an explicit *adopt-by-listing, never auto-manage* rule.
- **Push/pull from a linked materialization.** Config, remotes and credentials
  are shared (probed), and remote-tracking refs land in the shared refdb — so a
  fetch in one composite updates every materialization's `origin/*`. Desirable
  and on-charter, but it should be named in §5.6, together with the fact that a
  force-push from a lane reaches the primary's refs **[F13]**.
- **Root worktrees × tracked `gwz.conf/markers/`.** 327 tracked marker files
  duplicated into every root worktree's checkout and index — a per-composite
  index/stat cost, not a correctness issue **[F14]**.
- **§5.6 decision table.** Fair in direction; one cell wrong (hardlinked local
  clones do not apply to `gwz clone`), three omissions (git version floor,
  `extensions.worktreeConfig`, locked worktrees) **[F13]**.
- **Open questions.** The four are the right four with defensible defaults; two
  further decisions were made silently and should be surfaced as OQ-5 (does the
  registry need to exist?) and OQ-6 (who owns the shared `info/exclude` block?)
  **[F17]**.

---

## What the design gets right (for the record)

Recorded so remediation does not overcorrect:

- The **framing** — a second materialization strategy rather than a new object
  model — is correct, and the tree supports it: the strategy slot really is at
  `MaterializePlan.clone_url`, and `clone_workspace_with_emitter` already proves
  the machinery can be driven against a different root.
- **§2's trust envelope, stated first**, with an explicit no-new-durable-machinery
  commitment, is exactly the S1.1 lesson applied. It holds throughout; no row
  smuggles in a durability platform.
- **§5.2's conclusion** is right and is the best-evidenced claim in the document.
- **§3's authority hierarchy** (git per-repo admin authoritative per repo,
  manifest authoritative for content, registry advisory) is the correct
  decomposition, and it is what makes the **[P0 F2]** and **[P1 F6]** fixes small
  rather than structural.
- **§5.1's instruction to verify the seam against the code** is the discipline
  that caught three of this review's findings. It should become house practice
  for design docs making architectural claims.
- The **estate-migration fence (OQ-4)** is correctly and firmly placed.

---

## Conditions for GO

1. Rewrite §3 and W-CMD-6 to state the gitfile-safe git-directory resolution and
   the `info/exclude` ownership decision as **real scope**; add the
   merge-evidence interaction. **[F1]**
2. Replace W-LIF-2's completeness derivation with one that distinguishes intent
   from interruption, and state by name what happens at the existing
   `gwz-m0-worktrees/m0-*` composites. **[F2]**
3. Rewrite §5.3 with the measured per-operation table, adopt the existing AD1
   CLI-fallback discipline by name, and **record the per-operation decision in
   the document** rather than deferring it. **[F3]**
4. Reconcile W-LIF-1/W-CMD-2/W-CMD-4's failure model with the seam's actual
   reject-partial behaviour, and state which model `worktree add` and
   `worktree remove` each use. **[F4]**
5. State `--branch`'s base, its create-vs-checkout dispatch, and the per-repo
   divergence rule. **[F5]**
6. Name the registry-free enumeration authority (`git worktree list` on the root
   repo), which repairs W-ENV-6 and W-LIF-3 together. **[F6]**
7. Apply the P2 corrections: §5.2 mechanism, W-SAF-4 restatement, §1 evidence,
   locked worktrees, clone-nesting, preflight edges, §5.6 cell and omissions.
8. Add OQ-5 and OQ-6. **[F17]**

Conditions 1–3 are the ones a builder cannot work around; 4–6 are what stop a
builder building the wrong thing.

---

# Round 2

- **Object:** `gwz-dev/dev-docs/GwzWtDesign.md`, status "DRAFT round 2 —
  remediated 2026-09-01", §7 fold record present.
- **Date:** 2026-09-01. Same reviewer, single-axis, Opus. Final round under the
  two-round cap.
- **Scope:** per round 1's own re-review list — §3, §5.3, §5.5, W-CMD-2,
  W-CMD-4, W-CMD-6→W-CMP-*, W-LIF-1..3, W-ENV-1, W-ENV-6, W-SAF-2, W-SAF-4 read
  in full; §1, §2, §5.2, §5.4, §5.6 read only for the named edits.
- **Probes run (fresh scratch fixtures, git 2.52.0; no real repo written):**
  `extensions.worktreeConfig` honoring at `core.repositoryformatversion=0`;
  worktree-scoped `core.excludesFile` isolation, union behaviour, and
  interaction with a user global excludes file; the W-CMD-2 tri-state against
  both git conflict errors and its plan-time detectability. Read-only checks of
  `gwz-core/src/artifact/mod.rs`, `push_member.rs`,
  `gwz-cli/dev-docs/GwzLogRequirements.md`, and the four gwz repos'
  `core.bare`/`core.worktree`/`core.excludesFile` config.

## Round-2 verdict

# GO WITH CONDITIONS

Both round-1 P0s are **cured**, and cured properly rather than papered over.
W-LIF-2's intent-keyed completeness with "no verb ever refuses on
incompleteness" plus `m0-*`-unmanaged-by-name is a better answer than the one
round 1 asked for; per-composite intent files make F6's clobber structurally
impossible rather than merely unlikely; §5.1 and §5.3 are now honest about what
is reused and what is new. 14 of 17 findings are fully cured.

The remaining gate is one **P1 in the remediation itself**: W-CMP-2's chosen
mechanism works for the property it was chosen for (composites never write the
shared `info/exclude` — verified) but carries two behaviours the row does not
know about, one of which is a user-visible regression inside GwzWt's own
accident envelope. **[P1 F19]**. Second P1 is a residue: the root constituent
still has no defined checkout base, because no gwz artifact records a root pin.
**[P1 F18]**. Neither blocks planning; both must be edited before a builder
starts.

**Fresh finding counts:** P0 × 0, P1 × 2, P2 × 1, P3 × 3 (6 total).
**Round-1 dispositions:** CURED × 14, PARTIALLY CURED × 3, NOT CURED × 0.

## Round-1 finding dispositions

| F | Round-1 subject | Disposition | Note |
| --- | --- | --- | --- |
| F1 (P0) | `.git`-as-directory traversal; shared `info/exclude` | **PARTIALLY CURED** | W-CMP-1 cures the traversal correctly (resolution through the repository handle) and verb parity is now real scope, stated as such in §3 and W-CMD-6. W-CMP-2's mechanism has two unknown behaviours — **[F19]** — and over-reaches on enablement — **[F20]**. |
| F2 (P0) | Derived completeness not well-founded | **CURED** | W-LIF-2 keys on recorded intent; "no verb ever refuses on incompleteness" removes the refusal surface entirely; W-CMD-3 names `m0-*` as unmanaged (listed, never refused, never auto-managed). §2 states the reason precisely: intent is the one fact git cannot derive. OQ-1's subset case is explicitly reconciled. |
| F3 (P1) | Backend claim inverted | **CURED** | §5.3's table matches the measured reality per operation, with the grounds stated. Floor ground is mis-attributed — **[F21]**, cosmetic. |
| F4 (P1) | Seam reuse / failure model | **CURED** | §5.1 names each non-reused piece; W-CMP-3 owns the new aggregating preflight; W-LIF-3 explicitly rejects the `remove_dir_all` model and says why; root-outside-the-seam and host-bucket degeneration both stated. |
| F5 (P1) | `--branch` unstated grammar | **PARTIALLY CURED** | The tri-state is fully stated and probe-verified buildable (below). The stated create base does not exist for the root constituent — **[F18]**. |
| F6 (P1) | Registry race / enumeration authority | **CURED** | Per-composite files remove the race by construction; `git worktree list` on the root repo is named as the enumeration authority in §3, §5.4 and W-ENV-6. W-ENV-6's "worst case is a re-scan plus one re-add" is now true. |
| F7 (P2) | §5.2 mechanism misdescribed | **CURED** | All three load routes named correctly (lib.rs whole-file digest, `#[path]` set equality, `include!` ban), plus precedent-of-one and the symbol-scan / aggregate-gate notes. |
| F8 (P2) | W-SAF-4 lock layers | **CURED** | Two-layer restatement is accurate, cites the in-tree per-worktree guard test by name, and adds the builder instruction not to "fix" the shared catalog serialization. |
| F9 (P2) | Stale evidence | **CURED** | §1 corrected to the five `m0-*` composites, with the correction flagged in-line; the log lanes' teardown is now cited *for* the charter, which is the right reading. |
| F10 (P2) | Locked worktrees | **CURED** | W-CMD-4 refuses locked even under `--force` and never maps to `-f -f`; W-CMD-5 pins `locked(false)`; W-CMD-3/W-OBS-1 show locked-with-reason. Matches the probed git behaviour exactly. |
| F11 (P2) | Clone nesting inconsistency | **CURED** | W-ENV-1 makes the parent-walking check a shared helper that `gwz clone` adopts. |
| F12 (P2) | Preflight edges | **CURED** | §5.1 names both the remote-URL requirement and the source-dirtiness refusal as inapplicable, with the reason. |
| F13 (P2) | §5.6 table | **CURED** | Hardlink cell dropped with the URL reason given; floor + `worktreeConfig` row added; shared-fetch and `push --force` consequences named in the blast-radius row; locked-removal in create/destroy. |
| F14 (P3) | Markers cost | **CURED** | Folded into the Disk row with the measured 327 and the per-composite stat cost named. |
| F15 (P3) | No progress channel | **CURED** | W-OBS-2 states started/finished only, with the reason. |
| F16 (P3) | Non-UTF-8 convention | **CURED** | Cites `L-ENV-10`/`L-ENV-12` by document; both rows exist and say what is claimed (`GwzLogRequirements.md:556,571` — lossy render, never panic; machine output carries `"lossy": true`). |
| F17 (P3) | Silent decisions | **CURED** | OQ-5 and OQ-6 surfaced and decided, each with its reason. |

## Fresh findings

### [P1 F18] The root constituent has no lock pin or snapshot state, so W-CMD-1's precedence and W-CMD-2's create base are undefined for it

W-CMD-1 makes the root a constituent ("the root and every Git member as
worktrees") and gives the checkout precedence as "the named branch, the
snapshot's recorded state, or the lock pins". W-CMD-2 gives the create base as
"that repository's lock pin (or the snapshot's recorded state when `+snapshot`
is given)".

**No gwz artifact records a root state.** `LockArtifact`
(`gwz-core/src/artifact/mod.rs:159-164`) is `{schema, workspace_id,
manifest_schema, members}`; `SnapshotArtifact` (`:234-242`) is `{…,
selected_members, members}`. `gwz-dev/gwz.conf/gwz.lock.yml` contains only
`mem_*` keys. `grep -rn "root_commit\|root_head\|root_state" src/artifact/`
returns nothing. The root is not a manifest member either — `MemberPath::parse`
rejects empty and reserved paths (`workspace/mod.rs:14-50`).

So for the **default** invocation — `gwz worktree add <path>` with no
`--branch`, which W-CMD-1 defines as "detached at the pins" — there is no pin
for the root, and for `--branch` there is no base for the root. Both of the
document's two creation modes are undefined for the one constituent that must be
created first (§5.5 step 3).

This also reaches §5.4: the intent record's `members: [intended constituent
ids]` has no id to record for the root, so "incomplete iff its intent record
names a constituent that is absent" cannot express a missing root.

The protocol already has the vocabulary — `TargetKind::Root` is used throughout
push (`push_member.rs:341,356,504,511`) — so this is a naming decision, not new
machinery. Observed practice suggests the intended answer: all five `m0-*` roots
are detached at a single hand-chosen commit (`d3dac00`). State the root's base
explicitly (root HEAD at plan time is the obvious default), give it a reserved
constituent id for the intent record, and say what `+snapshot` means for a root
the snapshot never captured.

### [P1 F19] W-CMP-2's mechanism has two behaviours the row does not account for; one is a user-visible regression

The property W-CMP-2 was chosen for **holds** — verified. Enabling
`extensions.worktreeConfig` is honored even at `core.repositoryformatversion=0`
(git special-cases it), per-worktree config lands in
`<gitdir>/worktrees/<id>/config.worktree`, and the composite's own exclusion
patterns take effect while the shared `info/exclude` is left byte-untouched:

```
$ git -C primary config extensions.worktreeConfig true   # repositoryformatversion stays 0
$ git -C comp config --worktree core.excludesFile .../worktrees/comp/gwz-exclude
$ git -C primary check-ignore -v gwz-cli/f taut/f
.git/info/exclude:2:/taut/      taut/f          # primary unaffected by the composite
$ git -C primary status --porcelain
?? gwz-cli/                                     # composite's block does NOT leak into the primary
```

Two behaviours the row asserts away:

**(a) The composite's effective exclusion is the UNION with the primary's block,
not "ITS member set".** `core.excludesFile` is an *additional* ignore source, not
a replacement for `info/exclude`, and `info/exclude` resolves to the **common**
directory for every worktree (round-1 probe). Both fire inside the composite:

```
$ git -C comp check-ignore -v gwz-cli/f taut/f
.../worktrees/comp/gwz-exclude:2:/gwz-cli/      gwz-cli/f     # composite's own
.../primary/.git/info/exclude:2:/taut/          taut/f        # primary's, still applying
```

W-CMP-2's "a composite on a branch whose `gwz.yml` differs excludes ITS member
set" is therefore wrong in one direction: the composite excludes its set **plus**
the primary's. A path that is a member in the primary but ordinary untracked
content on the composite's branch is silently hidden from the composite's `git
status`. Git offers no per-worktree `info/exclude` and no way to suppress it, so
this cannot be engineered away — only stated.

**(b) It suppresses the user's own global `core.excludesFile`.**
`core.excludesFile` is single-valued and worktree config has the highest
precedence, so gwz's value replaces the user's personal global ignore file
inside every composite:

```
$ git config -f <global> core.excludesFile <user's ~/.gitignore_global>   # ignores /secretstuff/
$ git -C primary check-ignore -v secretstuff/f
<user's global>:1:/secretstuff/     secretstuff/f        # honored in the primary
$ git -C comp check-ignore -v secretstuff/f
(NOT ignored)                                            # suppressed in the composite
```

That is an accident-class, user-visible regression — squarely inside the
envelope §2 promises to defend — introduced by the remediation itself. The user
has this file unset today (`git config --global --get core.excludesFile` →
unset), which is why it would not be caught by a battery run on this machine.

**Cheapest sound resolution, offered because it is strictly simpler than what is
written:** composites may need **no exclusion mechanism of their own**. The
composite already inherits the primary's managed block through the shared
`info/exclude` (probe (a) above), and member paths are identical across branches
in the overwhelmingly common case — so the shared block already does the job it
exists to do. Dropping `core.excludesFile` removes the regression, removes the
extension enablement and its migration caveat (**[F20]**), and leaves one stated
degradation: a composite whose branch adds a member the primary's block does not
list shows that member as untracked in the composite's root `git status`. If
W-CMP-2 keeps the config mechanism instead, it must state behaviour (a) and
handle (b) — reading the effective inherited `core.excludesFile` at add time and
composing it is the only correct form, and it goes stale when the user edits
their global config.

### [P2 F20] The extension is enabled in every involved repo, but only the workspace root repo has a boundary exclude

W-SAF-2 licenses "the one-time `extensions.worktreeConfig` enablement in **each
involved repo's** shared config", and §5.5 step 3 says "configure **each
constituent's** worktree-scoped exclude". But the managed exclude block is
written only for the workspace **root** repository: `ensure_workspace_exclude` /
`workspace_exclude_path` take `root` and only `root`
(`sync_workspace_boundary.rs:34-45,88-90`); member repositories never get one.

So the design permanently changes the shared config of N repositories to solve a
problem that exists in one. Each such write is a repository-format extension,
not a preference. Git also documents a migration caveat for enabling it —
`core.bare` and `core.worktree` in the common config must be considered — and
all four gwz repos carry an explicit `core.bare=false` in shared config
(`gwz-dev`, `gwz-core`, `gwz-cli`, `gwz-py`; `core.worktree` unset everywhere).
Benign in this estate, but gwz cannot know that for an arbitrary member, and the
design should say what it does when it finds `core.worktree` set.

Narrow the row to the root repository, or drop the mechanism per **[F19]**.

### [P3 F21] §5.3's git floor is justified by an operation the same table says is not shelled out

The floor is given as "**≥ 2.36** (worktree `list --porcelain -z`)". But the row
above it makes `list` / health **git2 native**, so no CLI `worktree list` is
invoked and `-z` is never parsed. The grounds that actually bind are
`extensions.worktreeConfig` (≥ 2.20) and `repair` (≥ 2.30), both already
parenthesised in the same sentence. A conservative floor is harmless, but a
builder reading the stated ground will write a `--porcelain -z` parser the design
does not want. Restate the ground; keep or lower the number as preferred.

### [P3 F22] W-CMP-1's "no other `.join(\".git\")`" audit has one benign occurrence that will fail it

The production tree has exactly two `.join(".git")` sites:
`sync_workspace_boundary.rs:89` (the one W-CMP-1 removes) and
`handle_create_repo.rs:980`, `if root.join(".git").exists()`. The second is
**correct for both shapes** — `.exists()` is true for a gitfile as well as a
directory — so a grep-enumerable "no other site remains" audit will trip on a
line that needs no change. Name it as an allowlisted exception, or scope the
audit to `.join(".git").join(...)` traversal.

### [P3 F23] W-ENV-1's non-empty refusal and W-LIF-2's resume need a stated precedence

W-ENV-1 refuses "a non-empty existing directory"; W-LIF-2's resume re-enters a
composite whose root worktree already exists and is therefore non-empty. The row
gestures at this ("resume per W-LIF-2 re-enters existing constituent paths by
design") without giving the order. It is cleanly decidable now that intent
records exist — a `<path>` with an intent record is a resume, everything else is
a create — so one sentence closes it. Confirmed the underlying git behaviour
matches the row: `git worktree add` accepts an existing **empty** directory and
refuses a non-empty one (`fatal: '../fulldir' already exists`).

## Probe results for the two decisions the coordinator asked to be verified

**(a) Per-worktree exclusion via `extensions.worktreeConfig` + worktree-scoped
`core.excludesFile`.** Works for its stated purpose — composites get their own
patterns and never write the shared `info/exclude`; the primary is provably
unaffected. Honored at `core.repositoryformatversion=0`, so no format bump is
needed. **Two unstated behaviours: the composite's exclusion is a union with the
primary's block, and the user's global `core.excludesFile` is suppressed inside
composites.** See **[F19]**.

**(b) W-CMD-2 tri-state against git's two conflict errors.** Fully verified and
buildable as written. Both errors reproduce exactly as the row describes —
`worktree add <path> <name>` on a branch held elsewhere gives `fatal: '<name>' is
already used by worktree at '<path>'`, and `worktree add -b <name>` on an
existing branch gives `fatal: a branch named '<name>' already exists`. Both
states are cheaply detectable at plan time with no mutation
(`rev-parse --verify -q refs/heads/<name>` for existence;
`worktree list --porcelain` for the holder), so the row's "absorbed at plan time
and never surface raw" is achievable, and the "exists in ALL → check out
everywhere" path works (`worktree add ../a1 lane-exists` → succeeds when not held
elsewhere). The only gap is the root's base — **[F18]**.

## Conditions for the round-2 GO

1. **W-CMP-2:** either drop the composite-side exclude mechanism (recommended —
   the shared block already reaches composites, and this also closes **[F20]**),
   or state the union behaviour and handle the global-`core.excludesFile`
   suppression. **[F19]**
2. **W-CMD-1 / W-CMD-2 / §5.4:** define the root constituent's checkout base and
   its intent-record id. **[F18]**
3. **W-SAF-2 / §5.5:** narrow the extension enablement to the repositories that
   need it, and say what happens when `core.worktree` is already set. **[F20]**
4. Cosmetic, no re-review needed: **[F21]** floor ground, **[F22]** audit
   exception, **[F23]** resume precedence.

Conditions 1–3 are doc edits, not redesigns; none reopens a settled decision.
Under the two-round cap this closes the review — the lane owner merges the
verdict and the conditions ride into planning as edits.
