# GwzWt — Workspace Worktrees: Requirements & Design

Status: **ADOPTED 2026-09-01** — two review rounds complete
(`GwzWtDesign-Review.md`): round 1 NO-GO (2 P0 / 4 P1 / 7 P2 / 4 P3, all
folded, six as recorded design decisions), round 2 **GO WITH CONDITIONS**
(14 cured / 3 partial / 0 not-cured; fresh 2 P1 + 1 P2 + 3 P3, all folded
same day — §7). Final under the two-round cap. Next act, on the
operator's word only: the phased implementation plan.
Charter: the operator's GwzWt request; framing settled in conversation:
**worktrees are a second materialization strategy for the existing
workspace model, not a new object model.** Requirements + design only;
phasing is a separate future plan on the operator's word.

## 1. Problem

A gwz workspace coordinates N repositories, but gwz offers exactly one way
to have a second on-disk copy of one: `gwz clone` — heavy and fully
independent. Two standing pains follow:

1. **Agent lanes hand-roll linked materializations.** The live evidence is
   `gwz-m0-worktrees/{m0-a,m0-b1,m0-c1,m0-c2,m0-core-int}`: five real
   composites (root `.git` a gitfile; members worktrees of the primary's
   repos) built from bare `git worktree add`s plus convention — no
   coordinated create, health view, or teardown. The gwz-log project's
   lanes built the same shape and removed it by hand after use — the
   removal itself being the coordinated-teardown verb this design
   proposes. *(Round-1 F9: an earlier claim that those lane composites
   were still on disk was stale and is corrected here.)*
2. **Sibling clones go stale.** The 2026-08-29 estate push/sync sweep hit
   exactly this class: shared-member clones refusing non-fast-forward
   pushes because each clone's refs drifted. Linked worktrees of one
   primary cannot express that failure — one refdb, nothing to go stale
   against.

"Just create a new repo" is true per-repo and false per-workspace: a
second coherent workspace costs N clones plus remote wiring plus a
permanent sync obligation. Coordinating that is what gwz is for.

## 2. Trust envelope and non-goals

Stated first, per the standing lesson (gwz-log S1.1: an unstated envelope
turns a small step into a durability platform).

- **Envelope: cooperating same user.** GwzWt defends against ACCIDENT —
  crash, interruption, honest re-run — never against the user's own
  adversarial acts (hand `git worktree` surgery, refdb edits, deleting
  admin directories). Same-user confusion is handled by `prune`, re-run,
  and teaching refusals, not prevented.
- **Minimal durable footprint.** The only new durable artifacts are
  per-composite intent records (§5.4) — one small file each, advisory,
  reconstructible-except-intent, never a state machine. No WAL, no
  pending/finalized lifecycle, no journals. Composite EXISTENCE is
  derived from git (§5.4's enumeration authority); only INTENT is
  recorded, because intent is the one fact git cannot derive (round-1
  F2/F6).
- **Non-goals:** sharing build directories (`target/` stays
  per-materialization; the disk cost of builds is NOT solved here);
  cross-user or cross-host sharing; replacing `gwz clone`; any
  cross-materialization sync verbs (the shared refdb IS the sync);
  estate-wide migration of shared members across EXISTING workspaces
  (OQ-4, firmly out).

## 3. Concepts

- **Materialization.** An on-disk realization of a workspace. Today's
  only kind is the *primary*. GwzWt adds the **linked materialization**
  ("workspace worktree"): a sibling directory in which the root and every
  Git member are `git worktree`s of the primary's repositories — shared
  object stores and refdbs, independent HEADs/indexes/working trees.
- **Primary.** The materialization owning the real repositories. Git's
  per-repo worktree admin lives under each repo's common directory and is
  the per-repo source of truth.
- **Composite.** The set of per-repo worktrees forming one linked
  materialization. **Authority hierarchy:** git's per-repo admin is
  authoritative per repo; the manifest (read from the linked root's own
  checkout) is authoritative for what the workspace CONTAINS; the
  composite's intent record (§5.4) is authoritative for what the
  composite was ASKED to hold; enumeration of composites derives from
  `git worktree list` on the root repository (round-1 F6's authority),
  never from the intent records.
- **Self-consistency, honestly scoped (round-1 F1).** A linked root
  carries its own checkout of `gwz.conf/`, and workspace discovery is
  already gitfile-safe (`discover_workspace_root` keys on
  `gwz.conf/gwz.yml`; repository detection uses gitfile-aware
  `Repository::open`). But "existing verbs work unchanged" was FALSE at
  one production site and one shared mechanism, so verb parity is **real
  scope**, not an audit: the W-CMP rows in §4 name the code this design
  requires. The checked-artifact subsystem is the in-tree precedent that
  this is tractable — it already distinguishes `repository.path()` from
  `commondir()` throughout and exercises `Repository::worktree` in its
  tests, including a test pinning that the mutator-bootstrap guard lands
  in the linked worktree's own gitdir.

## 4. Requirements

Conventions per `gwz-core/dev-docs/GWZRequirements.md`: MUST/SHOULD/MAY;
v0 rows the eventual plan implements land with named tests.

### Command surface

- **W-CMD-1 (v0).** `gwz worktree add <path> [--branch <name> | --detach]
  [+snapshot]` MUST create a linked materialization of the WHOLE
  workspace at `<path>`: the root and every Git member as worktrees of
  the primary's repos, checked out to (precedence) the named branch, the
  snapshot's recorded state, or the lock pins. The default (no
  `--branch`) is detached at the pins — which the backend decision §5.3
  serves via CLI delegation, git2 being unable to create detached
  worktrees. **The root constituent's base (round-2 F18 — no gwz
  artifact records a root pin; lock and snapshot are members-only):**
  the primary root's HEAD commit resolved at plan time, under every
  mode — detached default, `--branch` creation base, and `+snapshot`
  alike — recorded in the composite's intent record so resume is
  deterministic. (Naming note for the plan: `TargetKind::Root` already
  exists in push; vocabulary reuse, not new machinery.) v0 is
  full-workspace-only (OQ-1).
- **W-CMD-2 (v0).** `--branch <name>` semantics, fully stated (round-1
  F5): the branch's CREATE BASE is that repository's lock pin (or the
  snapshot's recorded state when `+snapshot` is given). Tri-state
  dispatch, evaluated per repo at plan time: `<name>` exists in NO
  involved repo → create it from the base everywhere; exists in ALL →
  check out the existing branch everywhere (the resumed-lane case);
  exists in SOME → **refuse** (mixed states are not silently composed;
  the aggregated refusal lists which repos have it and which do not).
  Independently, a repo where `<name>` is already CHECKED OUT in another
  worktree → the same aggregated refusal names the repo, the branch, and
  the holding path (git's two distinct conflict errors — "branch exists"
  vs "already used by worktree" — are both absorbed at plan time and
  never surface raw). One aggregated teaching refusal covers all of the
  above, before any mutation.
- **W-CMD-3 (v0).** `gwz worktree list` MUST enumerate the primary and
  every composite (authority: `git worktree list` on the root repo) with
  per-repo health: present / missing / moved / dirty / **locked (with
  git's reason)**, and per-constituent checked-out ref. Composites with
  no intent record — the pre-existing `gwz-m0-worktrees/m0-*` shape —
  are listed as **unmanaged**: shown, never refused, never auto-managed
  (adopt-by-listing; round-1 F2).
- **W-CMD-4 (v0).** `gwz worktree remove <path> [--force]` MUST refuse
  when any constituent is dirty unless `--force`, and MUST refuse a
  LOCKED constituent even under `--force` (teaching `git worktree
  unlock`; gwz never auto-unlocks and never maps to git's `-f -f` —
  round-1 F10). Execution is per-repo tolerant (Partial with per-repo
  results) and idempotent — re-running completes a partial removal.
  `--force` never touches the primary's trees.
- **W-CMD-5 (v0).** `gwz worktree prune` MUST reconcile: per-repo
  dangling-admin cleanup via git2's native prune with `locked(false)`,
  `working_tree(false)`, `valid(false)` — it MUST NOT unlock, MUST NOT
  touch live or dirty trees — plus removal of intent records whose
  composites no longer exist.
- **W-CMD-6 (v0).** Every existing verb MUST behave inside a linked
  materialization as it does in a primary — with ONE named exception:
  structural merge refuses (W-CMP-4). This is **scope, not an audit**
  (round-1 F1): it holds only once the W-CMP rows below are
  implemented, and the parity battery then proves it verb-by-verb,
  the verb list derived from W-CMP-1's own blast-radius enumeration.
- **W-CMD-7 (v0).** Three channels: `gwz-cli` AND `gwz-py`, same flags,
  refusals, machine output, over an additive-only protocol extension
  (the L-PRO-1 discipline; drift check green in both repos).
- **W-CMD-8 (SHOULD, v0).** `gwz worktree repair` — per-repo delegation
  to `git worktree repair` (CLI-only; no libgit2 equivalent exists at
  any level).

### Compatibility scope (round-1 F1 — the new code W-CMD-6 needs)

- **W-CMP-1 (v0).** All gwz-core paths that resolve git metadata
  locations MUST be gitfile-safe: the one production `.git`-as-directory
  site (`workspace_exclude_path` traversing `root/.git/info/exclude`,
  reached from commit/branch/capture/pull/stage/repo/materialize and
  the merge evidence chain) is replaced by resolution through the
  repository handle (`repository.path()`/`commondir()`, the
  checked-artifact precedent), and a grep-enumerable audit pins that no
  other `.join(".git")` production site remains — against an enumerated
  allowlist for existence-probe sites where a gitfile answers correctly
  (round-2 P3: one such benign site exists today).
- **W-CMP-2 (v0).** **Boundary-exclude ownership (OQ-6, re-decided at
  round 2 — F19's probe showed the round-1 mechanism both unioned with
  the shared block anyway AND suppressed the user's global
  `core.excludesFile`, an accident-class regression inside this
  design's own envelope):** composites get **no exclude mechanism at
  all**. They INHERIT the primary's managed block through the shared
  `info/exclude` (git resolves it to the common dir for every worktree
  — probed). Boundary-sync verbs running inside a composite SKIP the
  exclude write (a no-op with a structured note), so composites never
  write the shared file and the merge-evidence chain is untouched.
  **Accepted residual, named:** a composite on a branch whose `gwz.yml`
  member set differs from the primary's shows the extra members as
  untracked noise in the composite root's `git status` — cosmetic, and
  the remedy is running the boundary sync from the primary (a
  per-worktree mechanism can be revisited in v2 with a
  multi-source design that does not clobber the user's global
  excludes). No `extensions.worktreeConfig` enablement exists anywhere
  in GwzWt (round-2 F20 moot).
- **W-CMP-3 (v0).** The plan-time aggregating preflight for
  `worktree add`/`remove` is NEW code (the existing materialize
  preflight is first-error; push's aggregation is the model) — W-LIF-1
  and W-CMD-2 depend on it and this row owns it.
- **W-CMP-4 (v0; added 2026-09-01 at the P0.1 plan review's F4).**
  Structural MERGE operations REFUSE inside a linked materialization
  with a typed teaching refusal ("run merges in the primary; this
  materialization shares the primary's merge-evidence surface").
  Ground: merge finalize/abort hash the shared `info/exclude` as
  evidence; inside a composite that file is the PRIMARY'S, which the
  primary's own ordinary boundary sync may rewrite at any moment — a
  cross-materialization evidence-invalidation window that cannot be
  closed without cross-materialization locking this design forbids.
  Refusal is the honest v0; W-CMD-6 carries this as its ONE named
  exception. Read-only merge inspection remains available.

### Lifecycle and crash behavior

- **W-LIF-1 (v0).** `add` MUST be plan-then-execute: every detectable
  refusal (path exists/nested, branch tri-state conflicts, checked-out
  conflicts, invalid names, unsupported member kinds, git floor) is
  collected by W-CMP-3's aggregating preflight and rejects the WHOLE
  request before any mutation.
- **W-LIF-2 (v0), rewritten (round-1 F2).** Composite completeness keys
  on INTENT, not the manifest: a composite is incomplete iff its intent
  record names a constituent that is absent on disk. Because partial
  materialization is already a first-class gwz state, **no verb ever
  refuses on incompleteness**: an interrupted `add` leaves a legal
  partial workspace; `list` marks it `incomplete (resume: gwz worktree
  add <path>)`; re-running the same `add` resumes idempotently
  (present-and-correct → skip; absent → create; conflicting → plan
  refusal). Composites without an intent record are unmanaged
  (W-CMD-3) and by definition never incomplete. The named crash window
  "root worktree exists, no members yet" is exactly this state and
  needs no special case.
- **W-LIF-3 (v0), rewritten (round-1 F6).** Execution-stage failures in
  `add` leave the partial composite IN PLACE (no rollback — resume or
  remove are the exits; this deliberately does NOT reuse the
  materialize seam's reject-partial `remove_dir_all` model, which would
  strand per-repo worktree admin and fight resume). Concurrent `add`s
  of distinct composites are safe: per-repo creation serializes on
  git's own admin locking, and intent records are per-composite files
  (§5.4) so no read-modify-write race exists at all.

### Safety and refusals

- **W-SAF-1 (v0).** All refusals are typed and teaching (name the repo,
  the conflict, the exit).
- **W-SAF-2 (v0).** GwzWt's write set, exactly: git per-repo admin
  areas; the composite's own paths; and the primary's `.gwz/` intent
  records. Nothing else — in particular, never the shared
  `info/exclude` (merge evidence), never any repo config, and never
  any tracked file.
- **W-SAF-3 (v0).** The conf-integrity gate applies per-materialization
  (each root worktree's `gwz.conf` against ITS committed state).
- **W-SAF-4 (v0), restated (round-1 F8).** Two lock layers, both by
  design: gwz's **workspace mutator lock is per-materialization** (each
  `.gwz/`; the in-tree test
  `linked_worktree_uses_its_actual_git_directory_for_the_guard` already
  pins the bootstrap guard per-worktree); the **checked-artifact
  catalog lease is per-common-directory and therefore SHARED** by a
  primary and all its composites — correct, because the catalog governs
  shared git state. Builders must not "fix" the resulting
  cross-materialization serialization on catalog-leased operations.

### Validity envelopes

- **W-ENV-1 (v0).** `<path>`: absolute or cwd-relative; MUST refuse a
  path inside ANY existing materialization of this workspace — the
  nesting check is a shared parent-walking helper that `gwz clone`
  also adopts (today clone checks only its target; round-1 F11) — and
  MUST refuse a non-empty existing directory — EXCEPT when a matching
  intent record marks the path as this composite's own resume target,
  which takes precedence (round-2 P3: the resume/refusal ordering,
  stated) — (an empty existing
  directory is acceptable and the CLI-delegated backend honors it;
  resume per W-LIF-2 re-enters existing constituent paths by design).
  Non-UTF-8 path bytes follow the recorded estate rows
  (`gwz-cli/dev-docs/GwzLogRequirements.md` L-ENV-10/L-ENV-12: OS-
  accepted, rendered lossy, machine-output lossy-flagged).
- **W-ENV-2 (v0).** Branch names: `git check-ref-format` is
  authoritative; refusals carry git's reason.
- **W-ENV-3 (v0).** Worktree strategy applies to Git members only; any
  non-Git member kind refuses `worktree add` naming member and kind
  (copy-fallback is v2 at most — OQ-3). A workspace with zero Git
  members refuses with the same row's message.
- **W-ENV-4 (v0).** Cross-filesystem composites permitted; Windows in
  scope with a named first-dispatch obligation. Detached/pinned members
  materialize as detached worktrees — served by §5.3's CLI-delegated
  `add` (git2 cannot; round-1 F3).
- **W-ENV-5 (v0).** `+snapshot`/`+lock` and all artifact reads inside a
  composite resolve against the composite root's OWN `gwz.conf`.
- **W-ENV-6 (v0), rewritten (round-1 F6).** Intent-record damage or
  loss is NEVER an error surface: enumeration derives from git
  (`git worktree list` on the root repo), so a lost record degrades a
  composite to unmanaged — visible in `list`, healed by re-running
  `add` (which re-records intent). `prune` drops orphaned records.
  Worst case is genuinely a re-scan plus one re-add.

### Observability

- **W-OBS-1 (v0).** `list` distinguishes primary / managed / unmanaged /
  incomplete, with per-repo health incl. locked-with-reason and the
  checked-out ref.
- **W-OBS-2 (v0).** `--json`/`--jsonl` schema-tagged (`gwz.worktree/v0`)
  with per-repo result records for add/remove/prune. Add emits
  started/finished per constituent only — the CLI-delegated backend has
  no progress callback (round-1 F15), stated so nobody hunts for one.

## 5. Design

### 5.1 Strategy at the existing seam — reuse, honestly bounded (round-1 F4)

The seam is real: `MaterializePlan`'s per-member strategy slot, the
selection/manifest/lock resolution, the jobs-bounded executor, and
per-member response construction are reused. What is NOT reused, stated
plainly: the first-error preflight (W-CMP-3 builds the aggregating one);
the reject-partial `remove_dir_all` failure model (W-LIF-3 keeps partial
composites); the remote-URL requirement and source-dirtiness refusal in
the existing preflight (irrelevant to worktrees — a worktree needs no
remote and the source tree's dirtiness has no bearing; round-1 F12); and
root creation, which sits outside the member seam — §5.5 creates the
root worktree first through its own path, exactly as clone creates the
root today. Host-bucketed parallelism degenerates to one local bucket
for worktree plans (no URLs); the jobs bound still applies.

### 5.2 Module home

`gwz-core/src/operation/worktree/` — a child of the `operation` seam per
the recorded 2026-08-29 operator ruling and reviewed placement
(`GwzLogPlan.md` §1.10; `GwzLog-S2.1-Review.md`). Mechanism, stated
precisely (round-1 F7): the crate root is protected by a whole-file
SHA-256 of `src/lib.rs` (plus `workspace_ops` mod files), the `#[path]`
edge set is inventoried with set equality, and `include!` is banned —
all three load routes into a new top-level module are pinned, which is
why a seam child is the sanctioned shape. `operation/commit_log/` is the
sole existing directory-shaped child (a precedent of one); the checker's
crate-wide symbol scans and the aggregate-gate test-count pins still
apply to a new child and its tests.

### 5.3 Backend — per-operation, decided now (round-1 F3)

Measured against git2 0.21.0 / libgit2 1.9.7 (the pinned versions):

| Operation | Backend | Ground |
| --- | --- | --- |
| `add` | **git CLI**, under the existing **AD1 per-primitive CLI fallback discipline** (shell out, then self-verify through libgit2 — the commit/tag/rev-list precedent, contract obligation recorded in `gitbackend/contract.rs`) | git2 cannot create a detached worktree (libgit2 rejects non-branch refs and otherwise auto-creates a branch named after the worktree); detached-at-pins is the v0 DEFAULT; git2 also exposes no checkout options and `Worktree` is neither Send nor Sync |
| `list` / health | **git2 native** (worktree enumeration, validity, lock status) | fully supported |
| `prune` | **git2 native**, flags pinned `locked(false)/working_tree(false)/valid(false)` | fully supported; the flags encode W-CMD-5's never-unlock/never-touch rules |
| `repair` | **git CLI** | no libgit2 equivalent exists |

**Git version floor: ≥ 2.30** (the CLI-delegated operations: `repair`
≥ 2.30, `add`/`remove` far older; round-2 P3 correction — the round-1
≥ 2.36 rested on `list --porcelain -z`, which this same table makes
git2-native, and the worktreeConfig component died with W-CMP-2's
re-decision), probed once per invocation that shells out, with a typed
teaching refusal naming the found and required versions. AD1 gains the small
central helper (environment sanitation + availability probe) it has so
far lacked — named here as in-scope. Self-verification after each
CLI-delegated add: open the created worktree via git2 and confirm HEAD,
per AD1. The checked-artifact subsystem's existing worktree-awareness
(path()/commondir() discipline, `Repository::worktree` in tests) is the
in-tree precedent, not greenfield.

### 5.4 Intent records (the registry, reshaped; OQ-5 decided; round-1 F2/F6)

Per-composite files: `.gwz/worktrees/<id>.yml` in the primary root, id =
a digest of the composite's canonical path; content: `{path, members:
[intended constituent ids], root_base: <commit>, created_at}` — the
root base recorded per W-CMD-1's round-2 F18 decision so resume is
deterministic. That is the entire durable
footprint, and it exists for exactly one reason: **intent is the one
fact git cannot derive** — it is what distinguishes an interrupted add
from a deliberately partial composite (F2) and defines resume.
Enumeration authority is git (`git worktree list` on the root repo);
records are advisory, per-composite (no shared-file read-modify-write —
F6's clobber is structurally impossible), prune-dropped when orphaned,
and re-created by re-add when lost.

### 5.5 The add algorithm

1. Resolve the workspace (manifest + lock) from the primary root.
2. PLAN (W-CMP-3 aggregating preflight): per constituent — target path
   (nesting + emptiness via the shared parent-walking helper), ref to
   check out (branch tri-state per W-CMD-2 | snapshot | lock pin),
   branch-exclusivity across each repo's worktrees, member-kind check,
   git floor probe. Collect EVERY refusal; refuse whole on any.
3. Write the intent record; create the ROOT worktree first (its own
   path, as clone does); then members under the jobs bound, per-repo
   results reported. No exclude configuration of any kind occurs
   (W-CMP-2's inherit rule; a stale instruction to configure
   worktree-scoped excludes survived the round-2 fold here and was
   removed at the P0.1 plan review's F2 — the add executor NEVER
   reaches the boundary-sync surface, by construction).
4. Failures leave the partial composite in place (W-LIF-3); resume =
   re-add; completeness derives from intent vs disk (W-LIF-2).

### 5.6 Clone vs worktree — the decision table (corrected per round-1 F13)

| Axis | `gwz clone` (independent) | `gwz worktree add` (linked) |
| --- | --- | --- |
| Isolation blast radius | Total — nothing shared | Shared refdb — branch surgery and `push --force` from a lane act on the primary's refs; a fetch in ANY materialization updates every materialization's remote-tracking refs (that shared-fetch behavior is §1's anti-staleness argument, named here because it cuts both ways) |
| Ref topology | Snapshotted; drifts; the estate's stale-clone class | One refdb; lane branches instantly visible; staleness inexpressible |
| Disk | Full trees; `gwz clone` clones from URLs, so git's local-clone hardlink optimization does NOT apply to it | Objects + refs shared; working trees, build dirs, and each root worktree's checkout+index of the 327 tracked `gwz.conf/markers/` files are per-copy (small disk, per-composite `git status` stat cost) |
| Create/destroy | Slower; independent teardown | Near-instant; coordinated remove/prune; locked constituents refuse removal until unlocked (removable-media protection) |
| Remotes/config | Re-wired per clone | Shared: primary's remotes, credentials, refspecs, `--local` config |
| Requirements | any git | git ≥ 2.36 floor + `extensions.worktreeConfig` enablement (§5.3, W-CMP-2) |
| Agent lanes | Workable, heavier, sync burden | The pattern lanes already hand-roll; first-class here |
| Failure isolation | A corrupted clone is self-contained | A corrupted worktree is prunable; refdb damage reaches everything |

Both stay first-class: clone when isolation is the point, worktree when
lanes and linkage are the point.

## 6. Open questions (defaults stated; blank endorses)

- **OQ-1.** v0 full-workspace-only; `--target` subsets v2 (now
  compatible with W-LIF-2, whose completeness keys on intent, not the
  manifest). Default: yes.
- **OQ-2.** `repair` in v0 as CLI delegation. Default: yes (SHOULD).
- **OQ-3.** Non-Git member kinds refuse. Default: refuse.
- **OQ-4.** Estate-wide shared-member migration: OUT, separate charter.
- **OQ-5 (was silent; round-1 F17).** Does a durable record exist at
  all? DECIDED: yes, minimal — per-composite intent records (§5.4),
  because intent alone cannot be derived; enumeration stays git's.
- **OQ-6 (was silent; round-1 F17/F1; RE-DECIDED at round 2, F19).**
  Who owns the member-exclusion block? The primary, alone. Composites
  carry NO exclude mechanism — they inherit the shared block, their
  boundary-sync skips the write, and the divergent-member-set case is
  a named cosmetic residual (W-CMP-2). The round-1 worktree-scoped
  mechanism was withdrawn on the reviewer's probe: it suppressed the
  user's global `core.excludesFile`.

## 7. Round-1 fold record (2026-09-01)

All 17 findings folded. The six design decisions: [F1→W-CMP-1/2, OQ-6]
gitfile-safe resolution + per-worktree exclude ownership, merge evidence
untouched; [F2→W-LIF-2, §5.4, OQ-5] intent-keyed completeness, no
refusal-on-incomplete, m0-* unmanaged-by-name; [F3→§5.3] CLI-delegated
add under AD1, per-operation table recorded, git ≥ 2.36 floor; [F4→§5.1,
W-LIF-3, W-CMP-3] seam reuse honestly bounded, no-rollback partials, new
aggregating preflight; [F5→W-CMD-2] branch base = lock pin/snapshot,
tri-state dispatch, refuse mixed; [F6→§5.4, W-ENV-6] per-composite
intent files, git as enumeration authority. P2/P3 corrections: F7 §5.2
mechanism; F8 W-SAF-4 two layers; F9 §1 evidence; F10 locked-worktree
rows in W-CMD-3/4/5; F11 shared nesting helper incl. clone; F12
preflight edges in §5.1; F13 table cells (hardlink claim dropped, floor
+ worktreeConfig + shared-fetch rows added); F14 markers cost in the
Disk row; F15 no-progress note in W-OBS-2; F16 non-UTF-8 cites the
recorded L-ENV rows; F17 OQ-5/OQ-6 surfaced and decided.

**Round-2 fold (2026-09-01; verdict GO WITH CONDITIONS, final under the
cap).** [F19 P1 → W-CMP-2/OQ-6 re-decided]: the worktree-scoped exclude
mechanism WITHDRAWN on the reviewer's probe (it unioned with the shared
block anyway and suppressed the user's global `core.excludesFile` — an
accident-class regression inside this design's own envelope);
composites now inherit the primary's block with a skip-the-write rule
and one named cosmetic residual — strictly simpler, and it moots
[F20 P2] (no `extensions.worktreeConfig` enablement exists) and shrinks
W-SAF-2's write set. [F18 P1 → W-CMD-1/§5.4]: the root constituent's
base defined — the primary root's HEAD at plan time, every mode,
recorded in the intent record (`root_base`); `TargetKind::Root` named
as the vocabulary. P3s: the git floor restated ≥ 2.30 with its real
justification; the W-CMP-1 audit gains an enumerated existence-probe
allowlist; the W-ENV-1 resume-vs-refusal precedence stated. The
reviewer's probe also verified W-CMD-2's tri-state fully buildable
against git's two conflict errors. Both round-1 P0 cures graded
"cured properly rather than papered over"; no settled decision was
reopened.

**Post-adoption corrections (2026-09-01, from the P0.1 PLAN review,
executed under this design's governs-rule):** [P0.1 F2] §5.5 step 3's
stale instruction to configure worktree-scoped excludes — a remnant the
round-2 F19 withdrawal missed — removed; the add executor never reaches
the boundary-sync surface. [P0.1 F4] **W-CMP-4 minted**: structural
merge REFUSES inside linked materializations (typed, teaching), closing
the cross-materialization evidence-invalidation window (a composite's
merge hashes the PRIMARY'S shared `info/exclude` as evidence, which the
primary's ordinary boundary sync may rewrite mid-flight — neither
document had named it); W-CMD-6 carries this as its one named
exception.
