# GWZ User Documentation Plan

Status: implemented and deployed. GitHub Pages was enabled with GitHub Actions
as its source, and the v0.9.1 documentation was deployed successfully.

Date: 2026-07-10

## Verdict

The documentation now has a single **QuickStart** path for humans, a clear
pointer from `gwz-cli` into the full **`gwz-dev`** development workspace via
`gwz clone`, and an astichi-style **GitHub Pages** site configuration that
publishes released documentation instead of exposing a raw GitHub tree link.

This plan supersedes the *user-facing* portion of the historical
`dev-docs/history/GwzDocPlan.md` wave. Keep that history for inventory and
generation/drift ideas; do not treat it as the current onboarding design.

## Original Problem

Before this implementation, the following existed mostly under
`gwz-cli/docs/`:

- A docs index that already says "Start Here", then fans out into Install,
  QuickStart, Concepts, CLI.md, Workflows, RootWorkspace, Troubleshooting, etc.
- `QuickStart.md` covers create/clone/status/add/commit/snapshot/tag/forall —
  but it is long, generic (`org/workspace.git`), and does not lead with the
  real development bootstrap story.
- `RepoLifecycle.md` now explains `repo clone`, `repo detach`, `repo attach`,
  evidence-backed re-add, and repository identity checks, but the beginner
  path does not yet introduce or route readers to that lifecycle.
- Hosted docs are currently a GitHub tree URL
  (`https://github.com/owebeeone/gwz-cli/tree/main/docs`), not a docs site.
- `RootWorkspace.md` talks about `gwz-dev`, but newcomers never get a crisp
  "clone this to hack on GWZ" instruction from the CLI README / install path.
- Command pages and generated `CLI.md` are valuable reference, not a learning
  path. Diff is especially easy to miss if it is only buried in reference.

Result: a new user sees many doors and no obvious first room.

## Goals

1. **One onboarding document** (`QuickStart.md`) that a beginner can follow
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
- Redesigning the broader Clap help model.
- Building a separate marketing site; Pages is the product docs site.

## Proposed Information Architecture

Publish from `gwz-cli` (user product). Source of truth remains markdown under
`gwz-cli/docs/` (or a thin `docs/` MkDocs root that includes those pages).

Suggested nav (MkDocs Material, modeled on astichi):

```text
Home                 → docs/README.md (short map + "read QuickStart")
Quick start          → docs/QuickStart.md         ← primary onboarding
Install              → docs/Install.md
Guide
  Create a workspace → (section of QuickStart or thin CreateWorkspace.md)
  Clone a workspace  → (section of QuickStart or thin CloneWorkspace.md)
  Daily workflow     → status / diff / commit / forall
  Grow members       → repo create / repo clone / repo add
  Member lifecycle   → repo detach / repo attach / evidence-backed re-add
  Refresh metadata   → repo sync
  Develop GWZ        → clone gwz-dev
Concepts             → docs/Concepts.md
Repository lifecycle → docs/RepoLifecycle.md
Workflows            → docs/Workflows.md (trim overlap with QuickStart)
Commands             → docs/commands/*.md + CLI.md
Machine output       → docs/MachineOutput.md
Troubleshooting      → docs/Troubleshooting.md
Releases             → docs/Releases.md
Agents               → docs/AgentBootstrap.md
```

Disposition of existing pages:

| Page | Action |
|------|--------|
| `QuickStart.md` | Keep the filename and rewrite it in place as the primary onboarding path. Existing links remain valid. |
| `README.md` (docs index) | Become the site home: short purpose + "Start here" CTA + link table. |
| `RootWorkspace.md` | Keep for dogfooding detail; QuickStart links to it for "hacking on GWZ". |
| `RepoLifecycle.md` | Keep as the canonical detailed guide to member creation, clone/add, detach/attach, evidence verification, and replacement. QuickStart demonstrates the common path and links here. |
| `Workflows.md` | Keep advanced recipes; remove duplicate beginner flows once QuickStart owns them. |
| `CLI.md` + `commands/` | Keep as reference; site nav under Commands. |
| `Install.md` / `Releases.md` | Keep; Install is step 0 of QuickStart. |

## QuickStart Content Outline

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

# clone a remote as a new member of this existing workspace
gwz repo clone git@github.com:org/shared.git libs/shared

# or register a Git checkout that already exists on disk
gwz repo add tools/local-tool
```

Give newcomers two explicit command distinctions before continuing:

| Command | Meaning |
|---------|---------|
| `gwz clone` | Clone a **workspace root** and materialize its locked members |
| `gwz repo clone` | Clone one new member into the current workspace |
| `gwz add` | Stage file changes across the workspace |
| `gwz repo add` | Register a repository checkout as a workspace member |

Repository membership commands stage their `gwz.conf/` metadata. Show the
review and commit without a blanket content-staging step:

```sh
gwz status
gwz commit -m "Add workspace members"
```

Do not teach `gwz add -A` merely to record a membership change: it can also
stage unrelated content in active member repositories. Keep `gwz add -A` in
the later daily editing loop, where staging all content is intentional.

Optional: `gwz init <url> <url>` as the "bootstrap from existing remotes"
variant, with a link to `commands/init.md`.

### 2a. Detach and attach a member

Demonstrate the reversible lifecycle without reproducing the full identity
specification:

```sh
gwz repo detach mem_shared
gwz repo attach mem_shared
```

Explain only the essentials here:

- detach removes the member from the current composition but retains its
  designation and checkout;
- attach requires the historical `mem_...` id and the checkout at its recorded
  path;
- `repo add` may reattach one historical designation only when local
  snapshot/marker commit evidence identifies it unambiguously;
- detailed source/member identity, `SourceIdentityMismatch`, empty-evidence
  warnings, and replacement flows live in `RepoLifecycle.md` and
  `Troubleshooting.md`.

### 2b. Publish a locally created member later

Make the `repo create` lifecycle explicit. A created member is a local Git
repository; GWZ does not create a repository on a hosting service. When an
empty remote is created later, the normal flow is:

```sh
gwz repo create services/api --member-id mem_api
# Create the empty org/api repository on the chosen host.
git -C services/api remote add origin git@github.com:org/api.git
gwz repo sync services/api

printf '# API service\n' > services/api/README.md
gwz add services/api/README.md gwz.conf
gwz commit -m "Create API service"
gwz --member mem_api push
```

Explain the boundary between the commands:

- `repo create` creates and registers the local repository;
- the hosting service or its own CLI creates the empty remote repository;
- `git remote add origin` changes the member's local Git configuration;
- `repo sync` records the observed remote and desired branch in the manifest;
- `repo sync` does not create/fetch/push the remote or rewrite the lock;
- a member branch needs a commit before `gwz push` can publish it;
- if the remote already has history, use `repo clone` instead of this flow.

Resolved implementation detail: unlike create/add/clone/detach/attach,
`repo sync` currently writes manifest metadata without staging it. The
implemented documentation therefore retains the explicit `gwz add gwz.conf`
step in every publish-later example. Making core stage this metadata remains a
separate behavior change, not a documentation prerequisite.

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

If `diff` docs are thin or unfinished relative to the product, QuickStart should
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
- Repository lifecycle → create / clone / add / detach / attach / sync
- Workflows → snapshots / materialize / pull / push / tag
- Commands → full reference
- Troubleshooting → when something fails
- Root workspaces → dogfooding `gwz-dev`

Hard rule: QuickStart stays short enough to finish in one sitting. No tag
matrix, no protocol, no agent template digests.

## gwz-cli Surface Pointers

Update these entrypoints so they all agree:

1. **`gwz-cli/README.md`**
   - Hosted docs → Pages URL (not GitHub tree).
   - After Install: "New to GWZ? Read QuickStart."
   - Explicit "Full development checkout" block with `gwz clone …/gwz-dev.git`.

2. **`gwz-cli/docs/README.md`**
   - Lead with QuickStart; demote the long link salad.

3. **`AGENTS_GWZ.md` template / AgentBootstrap**
   - Keep brief.
   - Point agents at Pages QuickStart (or raw QuickStart.md on the release tag)
     instead of only the docs tree.
   - Optionally mention `gwz-dev` clone for contributors; keep agent file short.

4. **Terminal help** (optional, small)
   - Root `--help` epilog or `gwz help` blurb: docs URL + "start with QuickStart".
   - Do not dump the tutorial into Clap.

5. **`gwz-dev/README.md` and `gwz-core/README.md`**
   - Link QuickStart in the first-screen next-step block.
   - Route product-motivation questions to `gwz-core/docs/WhyGwz.md`.

6. **`gwz-py/README.md`**
   - Link the same QuickStart instead of restating the command inventory.
   - Keep Python API/CLI positioning consistent with the separate
     `HomeReadmesPlan.md`.

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
| Optional: push to `main` touching `docs/**` / `mkdocs.yml` | Preview or staging only — **not** required for v1. Prefer release-tagged docs so Install/QuickStart never disagree with the binary users just installed. |

Astichi currently deploys docs on `main` pushes. GWZ should intentionally
differ: **release-coupled docs** fit a versioned CLI better. If we later want
main-branch preview docs, add a separate Pages environment or a `docs-dev`
workflow; do not conflate with the release site.

Implemented release publishing:

- `gwz-cli/mkdocs.yml` and `gwz-cli/docs-requirements.txt` define a strict
  MkDocs Material build.
- `.github/workflows/docs.yml` has `pages: write` / `id-token: write`, a
  build job → `upload-pages-artifact`, deploy job → `deploy-pages`.
- The workflow checks out the **release tag** (same discipline as
  `release.yml`), not
  floating `main`, when the trigger is a published release.
- GitHub Pages must be enabled with GitHub Actions as its source after the
  workflow lands; this is repository configuration rather than a source-file
  change.

### Versioning note

Pages is a single live site. On each `gwz-cli` release, overwrite the site with
that tag's docs. Call that out in Releases.md ("docs on the site match the
latest release"). Multi-version Mike/version switcher is out of scope unless
we later need it.

## Work Phases

### Phase 0 — Decisions (short)

- **Complete:** Pages lives on `gwz-cli`.
- **Complete:** release-only deploy plus tagged manual repair; there is no
  production deploy from `main` pushes.
- **Resolved:** retain `QuickStart.md` as the canonical onboarding filename and
  rewrite it in place.

### Phase 1 — QuickStart + entrypoint rewrites

- **Complete:** rewrote `QuickStart.md` with the outline above.
- **Complete:** retargeted `docs/README.md`, `gwz-cli/README.md`, generated
  terminal help, and AgentBootstrap links.
- **Complete:** linked QuickStart directly from all four GWZ root READMEs.
- **Complete:** integrated `RepoLifecycle.md` into the nav and showed the short
  create/clone/add/detach/attach flow in QuickStart.
- **Complete:** documented the publish-later lifecycle for `repo create`:
  create remote, add
  `origin`, `repo sync`, initial commit, and selected member push.
- **Resolved:** the current implementation leaves `repo sync` metadata
  unstaged, and every documented publish-later workflow explicitly stages
  `gwz.conf`.
- **Complete:** added the `gwz clone` vs `gwz repo clone` and `gwz add` vs
  `gwz repo add` distinction boxes.
- **Complete:** added the `gwz-dev` clone block everywhere a human would look
  for "how do I develop this."
- **Complete:** removed beginner create/clone/lifecycle duplication from
  `Workflows.md`.

### Phase 2 — MkDocs site skeleton

- **Complete:** `mkdocs.yml` nav matches the implemented IA.
- **Complete:** local `mkdocs build --strict` succeeds.
- **Complete:** command-page and generated-reference links resolve.

### Phase 3 — Release publish workflow

- **Complete:** `docs.yml` runs on `release: published` and tagged
  `workflow_dispatch`, and passes `actionlint`.
- **Complete:** Pages uses GitHub Actions as the publishing source, the
  `github-pages` environment permits `v*` release tags, and v0.9.1 completed the
  first deployment.
- **Complete:** updated Install / Releases / README hosted-docs URLs to
  `https://owebeeone.github.io/gwz-cli/` (and QuickStart deep link).

### Phase 4 — Polish

- **Complete:** added the diff command page and aligned QuickStart examples
  with current behavior.
- **Complete:** root help points to the hosted documentation and the generated
  `CLI.md` was refreshed.
- Consider publishing a docs tarball or Pages URL in release notes.
- Later: versioned docs if multiple major lines must stay online.

## Success Criteria

A new user can:

1. Install `gwz` from the README.
2. Open QuickStart on Pages.
3. Either `gwz init` + `repo create`/`repo clone`/`repo add`, or `gwz clone` an
   existing workspace.
4. Distinguish workspace clone/member clone and file staging/repository
   registration.
5. Detach and reattach a historical member designation without losing its
   identity.
6. Associate a locally created member with a later-created remote, synchronize
   its manifest metadata, and push its first commit.
7. Run `status`, `diff`, `commit`, and a simple `forall`.
8. Clone `gwz-dev` with `gwz clone` and recognize they have a full devel env.

Maintainers can:

1. Cut a `gwz-cli` release and get an updated Pages site without a separate
   docs push ritual.
2. Keep editing markdown in-repo; reference/`CLI.md` generation stays as-is.

## Resolved Decisions And Deployment State

- Documentation uses HTTPS clone URLs for copy-paste friendliness.
- `diff` is part of the QuickStart daily loop and has a dedicated command page.
- The `gwz-dev` root README leads with the complete-workspace clone path.
- The Pages site is release-coupled and represents the newest published CLI.
- GitHub Pages is enabled with workflow builds. The `github-pages` environment
  permits `main` and `v*` tags, allowing release-triggered deployments.
- The v0.9.1 workflow successfully deployed the first site. Manual dispatch can
  rebuild any release tag that contains the documentation site files.

## Local Validation

- `mkdocs build --strict` succeeds with the dedicated docs requirements.
- `python scripts/generate_cli_reference.py --check` confirms `CLI.md` matches
  the current Clap surface.
- `actionlint` accepts the release-coupled Pages workflow.
- Every relative Markdown link target in the active CLI and core docs and the
  four home entrypoints exists.
- CLI tests, managed-bootstrap tests, and the complete `gwz-py` test runner pass.
