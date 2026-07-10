# GWZ User Documentation Plan

Status: planning only. No implementation in this change.

Date: 2026-07-09

## Verdict

Yes — the current docs are usable once you already know GWZ, but they are a poor
first contact. We need a single **StartHere** path for humans, a clear pointer
from `gwz-cli` into the full **`gwz-dev`** development workspace via
`gwz clone`, and a real **GitHub Pages** site (astichi-style MkDocs) published
from release, not a raw GitHub tree link.

This plan supersedes the *user-facing* portion of the historical
`dev-docs/history/GwzDocPlan.md` wave. Keep that history for inventory and
generation/drift ideas; do not treat it as the current onboarding design.

## Problem

What exists today (mostly under `gwz-cli/docs/`):

- A docs index that already says "Start Here", then fans out into Install,
  QuickStart, Concepts, CLI.md, Workflows, RootWorkspace, Troubleshooting, etc.
- `QuickStart.md` covers create/clone/status/add/commit/snapshot/tag/forall —
  but it is long, generic (`org/workspace.git`), and does not lead with the
  real development bootstrap story.
- Hosted docs are currently a GitHub tree URL
  (`https://github.com/owebeeone/gwz-cli/tree/main/docs`), not a docs site.
- `RootWorkspace.md` talks about `gwz-dev`, but newcomers never get a crisp
  "clone this to hack on GWZ" instruction from the CLI README / install path.
- Command pages and generated `CLI.md` are valuable reference, not a learning
  path. Diff is especially easy to miss if it is only buried in reference.

Result: a new user sees many doors and no obvious first room.

## Goals

1. **One onboarding document** (`StartHere.md`) that a beginner can follow
   end-to-end without reading Concepts or CLI.md first.
2. **Two explicit entry paths** after install:
   - create/grow your own GWZ workspace;
   - clone an existing GWZ workspace (including `gwz-dev` for full devel).
3. **Daily loop** shown early: `status` → `diff` → `add`/`commit`, plus
   `forall` as the multi-repo escape hatch.
4. **GitHub Pages** user site for `gwz-cli`, built and published on release
   (astichi pattern), with README / AGENTS_GWZ / install text pointing at the
   Pages URL instead of the raw tree.
5. Keep reference docs; stop asking newcomers to discover them first.

Non-goals for this wave:

- Rewriting `gwz-core` / taut API docs.
- Redesigning Clap help or regenerating `CLI.md` (keep existing drift checks).
- Building a separate marketing site; Pages is the product docs site.

## Proposed Information Architecture

Publish from `gwz-cli` (user product). Source of truth remains markdown under
`gwz-cli/docs/` (or a thin `docs/` MkDocs root that includes those pages).

Suggested nav (MkDocs Material, modeled on astichi):

```text
Home                 → docs/README.md (short map + "read StartHere")
Start here           → docs/StartHere.md          ← primary onboarding
Install              → docs/Install.md
Guide
  Create a workspace → (section of StartHere or thin CreateWorkspace.md)
  Clone a workspace  → (section of StartHere or thin CloneWorkspace.md)
  Daily workflow     → status / diff / commit / forall
  Grow members       → repo add / repo create
  Develop GWZ        → clone gwz-dev
Concepts             → docs/Concepts.md
Workflows            → docs/Workflows.md (trim overlap with StartHere)
Commands             → docs/commands/*.md + CLI.md
Machine output       → docs/MachineOutput.md
Troubleshooting      → docs/Troubleshooting.md
Releases             → docs/Releases.md
Agents               → docs/AgentBootstrap.md
```

Disposition of existing pages:

| Page | Action |
|------|--------|
| `QuickStart.md` | Fold into / replace with `StartHere.md`. Keep a redirect stub or delete after Pages ships. |
| `README.md` (docs index) | Become the site home: short purpose + "Start here" CTA + link table. |
| `RootWorkspace.md` | Keep for dogfooding detail; StartHere links to it for "hacking on GWZ". |
| `Workflows.md` | Keep advanced recipes; remove duplicate beginner flows once StartHere owns them. |
| `CLI.md` + `commands/` | Keep as reference; site nav under Commands. |
| `Install.md` / `Releases.md` | Keep; Install is step 0 of StartHere. |

## StartHere Content Outline

Audience: someone who just installed `gwz` and has never used a GWZ workspace.

### 0. Install (pointer)

One short block + link to `Install.md`. Do not duplicate installer matrices.

### 1. What GWZ is (one screen)

- A GWZ workspace is a **root Git repo** that owns `gwz.conf/` and coordinates
  **member** Git repos.
- You work from the root (or inside a member); GWZ finds the workspace.
- Mental model only — no artifact deep-dive (that stays in Concepts).

### 2. Path A — Create a new workspace

```sh
mkdir my-workspace && cd my-workspace
gwz init
```

Then grow it:

```sh
# new empty member repo inside the workspace
gwz repo create services/api

# or register an existing local git checkout as a member
gwz repo add libs/shared
```

Show the commit of workspace metadata:

```sh
gwz status
gwz add -A
gwz commit -m "Add api and shared members"
```

Optional: `gwz init <url> <url>` as the "bootstrap from existing remotes"
variant, with a link to `commands/init.md`.

### 3. Path B — Clone an existing workspace

```sh
gwz clone <workspace-git-url> [directory]
cd <directory>
gwz status
```

Note the plain-git fallback: `git clone` root → `gwz materialize --lock`.

### 4. Develop GWZ itself (first-class, not a footnote)

`gwz-cli` docs and README should explicitly say: to get a full development
environment, clone the `gwz-dev` root workspace:

```sh
gwz clone git@github.com:owebeeone/gwz-dev.git
cd gwz-dev
gwz ls --local
gwz status
```

That materializes `gwz-cli`, `gwz-core`, `taut`, `gwz-py`, etc. from the lock.
Point to root `README.md` / `RootWorkspace.md` for cargo workflows
(`cargo run -q -p gwz -- …`).

This is the missing bridge between "I installed the binary" and "I can hack
the project."

### 5. Daily loop (status / diff / commit)

Keep this early and concrete:

```sh
gwz status
gwz diff                 # or whatever the shipped surface is; document real flags
gwz add -A               # or pathspecs
gwz commit -m "…"
```

Call out:

- status before mutating;
- diff before commit;
- commit spans members + root when staged;
- `--porcelain` / `--json` only as a one-liner pointer to MachineOutput.

If `diff` docs are thin or unfinished relative to the product, StartHere should
still show the intended user command and link the command page; do not hide
diff until Concepts.

### 6. forall

Show `forall` as "run the same command in selected members":

```sh
gwz forall -- git status --short
gwz forall gwz-cli gwz-core -- cargo test
gwz forall -c 'printf "%s\n" "$GWZ_MEMBER_PATH"'
```

One sentence on `GWZ_MEMBER_ID` / `GWZ_MEMBER_PATH`. Link `commands/forall.md`
for `--partial`, banners, selection rules.

### 7. Where next

Short table only:

- Concepts → vocabulary
- Workflows → snapshots / materialize / pull / push / tag
- Commands → full reference
- Troubleshooting → when something fails
- Root workspaces → dogfooding `gwz-dev`

Hard rule: StartHere stays short enough to finish in one sitting. No tag
matrix, no protocol, no agent template digests.

## gwz-cli Surface Pointers

Update these entrypoints so they all agree:

1. **`gwz-cli/README.md`**
   - Hosted docs → Pages URL (not GitHub tree).
   - After Install: "New to GWZ? Read StartHere."
   - Explicit "Full development checkout" block with `gwz clone …/gwz-dev.git`.

2. **`gwz-cli/docs/README.md`**
   - Lead with StartHere; demote the long link salad.

3. **`AGENTS_GWZ.md` template / AgentBootstrap**
   - Keep brief.
   - Point agents at Pages StartHere (or raw StartHere.md on the release tag)
     instead of only the docs tree.
   - Optionally mention `gwz-dev` clone for contributors; keep agent file short.

4. **Terminal help** (optional, small)
   - Root `--help` epilog or `gwz help` blurb: docs URL + "start with StartHere".
   - Do not dump the tutorial into Clap.

## GitHub Pages (astichi pattern)

Reference implementation:
`/Users/owebeeone/limbo/grip-pyrolyze-dev/astichi`
(`mkdocs.yml`, `docs-requirements.txt`, `.github/workflows/docs.yml`).

### Site

- **Repo:** `owebeeone/gwz-cli` (user-facing product).
- **URL:** `https://owebeeone.github.io/gwz-cli/` (confirm Pages project settings).
- **Tooling:** MkDocs + Material theme; `mkdocs build --strict`.
- **Content root:** existing `gwz-cli/docs/` markdown, plus `mkdocs.yml` at
  `gwz-cli` root (same layout as astichi).

### When to publish

User request: **build on release**.

Recommended:

| Trigger | Behavior |
|---------|----------|
| `release: types: [published]` on `gwz-cli` | Build MkDocs from the release tag; deploy to GitHub Pages. Docs match the shipped CLI. |
| `workflow_dispatch` | Manual rebuild/repair for a tag or `main`. |
| Optional: push to `main` touching `docs/**` / `mkdocs.yml` | Preview or staging only — **not** required for v1. Prefer release-tagged docs so Install/StartHere never disagree with the binary users just installed. |

Astichi currently deploys docs on `main` pushes. GWZ should intentionally
differ: **release-coupled docs** fit a versioned CLI better. If we later want
main-branch preview docs, add a separate Pages environment or a `docs-dev`
workflow; do not conflate with the release site.

Implementation sketch (do not land in this plan change):

- Add `gwz-cli/mkdocs.yml`, `gwz-cli/docs-requirements.txt`.
- Add `.github/workflows/docs.yml` with `pages: write` / `id-token: write`,
  build job → `upload-pages-artifact`, deploy job → `deploy-pages`.
- Checkout the **release tag** (same discipline as `release.yml`), not
  floating `main`, when the trigger is a published release.
- Enable GitHub Pages (GitHub Actions source) on `gwz-cli`.

### Versioning note

Pages is a single live site. On each `gwz-cli` release, overwrite the site with
that tag's docs. Call that out in Releases.md ("docs on the site match the
latest release"). Multi-version Mike/version switcher is out of scope unless
we later need it.

## Work Phases

### Phase 0 — Decisions (short)

- Confirm Pages lives on `gwz-cli` (recommended) vs a docs-only repo.
- Confirm release-only deploy vs also-on-main.
- Confirm `StartHere.md` name (vs renaming QuickStart in place). Prefer a new
  `StartHere.md` so links and mental model match the product ask.

### Phase 1 — StartHere + entrypoint rewrites

- Author `StartHere.md` with the outline above.
- Retarget `docs/README.md`, `gwz-cli/README.md`, and AgentBootstrap links.
- Add the `gwz-dev` clone block everywhere a human would look for "how do I
  develop this."
- Trim QuickStart/Workflows duplication.

### Phase 2 — MkDocs site skeleton

- `mkdocs.yml` nav matching the IA above.
- Local `mkdocs serve` / `mkdocs build --strict`.
- Fix relative links that break under MkDocs (command pages ↔ CLI.md).

### Phase 3 — Release publish workflow

- `docs.yml` on `release: published` (+ `workflow_dispatch`).
- Wire Pages; verify one dry-run via workflow_dispatch.
- Update Install / Releases / README hosted-docs URLs to
  `https://owebeeone.github.io/gwz-cli/` (and StartHere deep link).

### Phase 4 — Polish (optional follow-ons)

- Diff command page + StartHere examples aligned with current behavior.
- Homepage one-liner in `gwz --help`.
- Consider publishing a docs tarball or Pages URL in release notes.
- Later: versioned docs if multiple major lines must stay online.

## Success Criteria

A new user can:

1. Install `gwz` from the README.
2. Open StartHere on Pages.
3. Either `gwz init` + `repo create`/`repo add`, or `gwz clone` an existing
   workspace.
4. Run `status`, `diff`, `commit`, and a simple `forall`.
5. Clone `gwz-dev` with `gwz clone` and recognize they have a full devel env.

Maintainers can:

1. Cut a `gwz-cli` release and get an updated Pages site without a separate
   docs push ritual.
2. Keep editing markdown in-repo; reference/`CLI.md` generation stays as-is.

## Explicit Non-Actions In This Change

This file is the only deliverable for now:

- No new `StartHere.md`.
- No MkDocs / workflow files.
- No README edits.
- No commits or releases.

## Open Questions

1. Exact `gwz-dev` clone URL form in docs: SSH (`git@github.com:…`) vs HTTPS
   for copy-paste friendliness on Pages.
2. Whether `diff` should be in the StartHere daily loop before the diff feature
   docs are fully settled — recommendation: **yes**, with a link to the command
   page and honest "see `gwz help diff`" if flags are still moving.
3. Whether root `gwz-dev/README.md` should mirror the "clone me with gwz"
   blurb for people who land on the super-workspace repo first (recommended:
   yes, small pointer, still owned by a later edit pass).
