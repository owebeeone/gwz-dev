# GWZ Repository Home README Alignment Plan

Status: planning; `gwz-core/docs/WhyGwz.md` is the one content anchor added
while establishing this plan.

Date: 2026-07-10

## Verdict

The GWZ repositories should use a consistent **information flow**, not shared
boilerplate. A first-time visitor should be able to answer these questions from
the first screen of every README:

1. What is GWZ, in terms of the problem it solves?
2. What part of GWZ is this repository?
3. Is this the repository I should install, embed, contribute to, or merely
   inspect?
4. What is the shortest useful next step?
5. Where are the canonical motivation, QuickStart, and detailed docs?

Implementation inventory, parity notes, maturity labels, build prerequisites,
and release mechanics belong later. They are useful after the reader knows why
the repository matters.

## Canonical Message

Every home README should express the same product story in repository-specific
language:

> GWZ coordinates multiple ordinary Git repositories as one reproducible,
> inspectable workspace. The workspace root records composition and exact
> state; the member repositories remain normal Git repositories.

The repositories then identify their role:

| Repository | Role | Primary visitor |
|------------|------|-----------------|
| `gwz-cli` | Primary, recommended, most thoroughly tested terminal interface | Someone who wants to install and use GWZ |
| `gwz-core` | Embeddable Rust engine and typed operation/protocol contract | Tool, UI, agent, bridge, or library author |
| `gwz-py` | Python bindings to `gwz-core` plus an equivalent Python CLI that exercises and supports the bindings | Python application author or Python-only integrator |
| `gwz-dev` | Complete development super-workspace containing the coordinated GWZ repositories | Contributor who wants to build or change GWZ itself |

Taut has its own product and README. It should be linked from the `gwz-dev`
repository map but is not forced into this GWZ README template.

## Canonical Destinations

- **Why GWZ:** `gwz-core/docs/WhyGwz.md` explains the problem, fit, and product
  boundaries.
- **QuickStart:** `gwz-cli/docs/QuickStart.md` remains the canonical first-use
  path. Do not create `StartHere.md`.
- **User documentation:** `gwz-cli/docs/` and, once published, the `gwz-cli`
  Pages site.
- **Core embedding documentation:** `gwz-core/docs/README.md`.
- **Full development checkout:** the `gwz-dev` root README.

Links must work when each repository is viewed independently on GitHub. Use a
same-repository relative link where possible and a stable GitHub/Pages URL for
cross-repository destinations.

## Shared README Shape

The exact headings may vary, but use this order:

1. Repository name.
2. Two or three plain-language paragraphs:
   - outcome/problem;
   - this repository's role;
   - recommended next action or important product-positioning distinction.
3. A small next-step block: install, QuickStart, embed, or clone for
   development.
4. Repository-specific usage or API example.
5. What the repository contains.
6. Development and verification.
7. Status/maturity and platform caveats.
8. Release and license details.

Avoid opening with:

- package-shape or bridge inventory;
- exhaustive command lists;
- argv/protocol parity claims;
- generated-code details;
- release-wheel implementation mechanics;
- alpha disclaimers before the reader knows the purpose.

## Repository Revisions

### `gwz-cli/README.md`

Position this as the normal installation choice:

> `gwz` is the primary command-line interface for coordinating multiple Git
> repositories as one reproducible GWZ workspace.

The first screen should contain:

- the product outcome;
- “install this if you want to use GWZ from a terminal”;
- the release installer;
- a direct QuickStart link;
- a short “develop GWZ itself” link to `gwz-dev`.

Move parser/request/renderer architecture below the user entrypoint. Keep
generated CLI reference and development checks, but do not make them the
introduction.

### `gwz-core/README.md`

The current explanation of the embeddable engine is useful, but it starts one
level below the product motivation. Add, near the top:

- one sentence describing the multi-repository problem;
- a link to `docs/WhyGwz.md`;
- a direct link to the user QuickStart for readers who actually want the CLI;
- an explicit choice: “use `gwz-cli` for terminal workflows; embed this crate
  for typed workspace operations.”

Keep the Rust example and core documentation map. Put release mechanics after
the embedding path.

### `gwz-py/README.md`

Replace the current implementation-status opening with this message:

> `gwz-py` provides Python bindings to `gwz-core` and a Python implementation
> of the GWZ CLI. The CLI keeps the Python bindings exercised across real
> user-facing workflows, so Python applications can rely on the same workspace
> operations exposed by the core engine.
>
> The `gwz-py` CLI is intended to be functional and compatible with the GWZ
> command model. For general terminal use, the Rust `gwz` CLI is the primary
> and more thoroughly tested implementation. Use `gwz-py` when Python API
> integration or a Python-distributed CLI is the requirement.

Then show, in this order:

1. `pip install gwz`.
2. A minimal Python `Client` example.
3. A minimal `gwz-py` CLI example and a link to QuickStart.
4. The relationship to the shared native `gwz-core` extension.
5. Development, native build, protocol regeneration, and package smoke tests.
6. Platform prerequisites and alpha status near the bottom.

Remove the opening enumeration of `add`, `clone`, `create`, `detach`, `attach`,
`sync`, id overrides, and dry-run details. Those belong in generated command
reference, lifecycle docs, and tests. Retain only a concise statement that the
Python CLI follows the same command model.

Do not imply that the Python package shells out to the Rust executable. Both
the Python API and Python CLI call the native `gwz-core` extension.

### `gwz-dev/README.md`

Lead with visitor outcome rather than “development super-workspace” inventory:

> `gwz-dev` is the complete checkout for building, testing, and changing GWZ.
> Clone this workspace when you want the Rust CLI, core engine, Python
> bindings, Taut tooling, and their coordinated revisions together.

The next block should be immediately actionable:

```sh
gwz clone git@github.com:owebeeone/gwz-dev.git
cd gwz-dev
gwz status
cargo test --workspace
```

Then link:

- Why GWZ for the product rationale;
- QuickStart for using the product;
- repository map for contributors;
- root-workspace development workflow.

Move the repository inventory below the clone/build path. Retain the useful
cross-repository commands, but present them as contributor workflow rather than
as the reason the repository exists.

## QuickStart Integration

Keep `QuickStart.md`; rewrite it in place under the separate user documentation
plan. At minimum, the following home READMEs link to it in their first screen:

- `gwz-cli/README.md` — primary action after install;
- `gwz-core/README.md` — route terminal users away from embedding docs;
- `gwz-py/README.md` — shared command/workspace model;
- `gwz-dev/README.md` — route contributors who first need the product model.

The QuickStart owns beginner commands. READMEs should not each grow a separate,
soon-divergent tutorial.

## Tone And Editorial Rules

- Lead with user outcome, not internal architecture.
- Prefer concrete nouns: workspace root, member repository, manifest, lock.
- Say which implementation is primary where that affects a user decision.
- Do not claim absolute parity when “same command model” is sufficient.
- Keep maturity honest but place it after the useful path.
- Use “GWZ” for the product, `gwz` for the Rust CLI, `gwz-core` for the engine,
  and `gwz-py` for the Python package/CLI.
- Do not copy the same introductory paragraph verbatim across repositories.

## Work Phases

### Phase 0 — Anchors

- Add `gwz-core/docs/WhyGwz.md`. **Complete.**
- Retain `gwz-cli/docs/QuickStart.md` as the canonical beginner path.
- Agree on stable cross-repository URLs before publishing README changes.

### Phase 1 — First-screen rewrites

- Rewrite the opening and next-step block in `gwz-cli/README.md`.
- Rewrite the opening and content order in `gwz-py/README.md`.
- Rewrite `gwz-dev/README.md` around the clone/build contributor outcome.
- Add Why GWZ / QuickStart routing to `gwz-core/README.md`.

Do this as one coordinated change so the role statements can be reviewed side
by side.

### Phase 2 — Content ordering

- Move implementation inventories, platform prerequisites, release mechanics,
  and status sections below the usage path.
- Deduplicate tutorials in favor of QuickStart.
- Keep repository-specific API and contributor examples.

### Phase 3 — Link and rendering verification

- Check every relative link from each repository root.
- Check cross-repository links in standalone GitHub rendering.
- When Pages exists, replace raw GitHub user-doc links with the released Pages
  destinations.
- Render the first screen at common desktop and narrow widths; the purpose and
  next action must appear before a long code sample or contents list.

## Acceptance Criteria

- A new visitor can identify the purpose and role of every GWZ repository from
  its first three paragraphs.
- `gwz-cli` is clearly the recommended and most thoroughly tested terminal
  implementation.
- `gwz-py` clearly provides both supported core bindings and a functional
  Python CLI without leading with parity noise.
- Alpha status remains visible in `gwz-py`, but does not displace its purpose.
- `gwz-dev` immediately tells a contributor how and why to clone the complete
  workspace.
- `gwz-dev`, `gwz-core`, `gwz-cli`, and `gwz-py` all route beginners to the
  same QuickStart.
- Product motivation has one canonical explanation in `WhyGwz.md`.
- No README duplicates the full repository lifecycle or command reference.
- All local and cross-repository documentation links pass verification.

## Non-goals

- Making all README files visually or textually identical.
- Moving detailed command reference out of `gwz-cli/docs/`.
- Describing every protocol or native bridge capability on a repository home
  page.
- Repositioning Taut; its documentation has a separate audience and product
  story.
- Changing package maturity or claiming the Python CLI has the same test depth
  as the Rust CLI.

