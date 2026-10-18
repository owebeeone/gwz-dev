# GWZ Git Library G0 — SURFACE-AXIS REVIEW

**Review object:** Implemented G0 candidate in `gwz-git`, reviewed through its README, accepted API guide, and documented commands.  
**Date:** 2026-09-21  
**Axis:** Public setup, API usage, lifetimes, errors, defaults, testing, and cleanup. Independent, adversarial, read-only; no peer report informed this verdict.

**Baseline:**

| Repository | Exact SHA |
|---|---|
| root | `3a6d28f77591acec00334a64877b6411d29f8231` |
| gwz-core | `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce` |
| gwz-git | `fcf5296095d230f46c8409a8d6d47b44f4ad918b` |
| git2-rs | `4c1caabbce7d56426c763dd94114052302b23e4c` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All five revisions matched at start and end. Workspace status showed only the three authorized untracked review prompts.

**Verdict: GO** — P0: 0 · P1: 0 · P2: 0 · P3: 0. No Surface findings.

---

## 0. Evidence base

Read immutable versions of:

- `gwz-git/README.md`, in full.
- `gwz-core/dev-docs/GwzGitLibraryApi.md`, in full, including opening, ownership, IDs, records, and errors.
- The supplied Surface review prompt.

No implementation, tests, internal design, plan, checkpoint, or peer report was read.

From `gwz-git`, ran the documented commands with these environment settings:

```sh
CARGO_TARGET_DIR=/tmp/gwz-git-g0-review-surface
CARGO_NET_OFFLINE=true
```

| Command | Result |
|---|---|
| `rustup run 1.95.0 cargo fmt --check` | Passed |
| `rustup run 1.95.0 cargo test --locked` | 11 integration tests passed; three documentation checks passed |
| `rustup run 1.95.0 cargo clippy --locked --all-targets -- -D warnings` | Passed |

Documentation checks comprised one compile-only example and two compile-fail cases. The example is marked `no_run`; executing its body was not demonstrated.

Integration output included exact opening, linked worktrees, format-specific IDs, SHA-256 concurrent reads, cross-worker ownership, owned diagnostics, raw commit fields, malformed/missing objects, unchanged repository/process state, and operation without Git on PATH.

Removed the isolated target directory afterward with `rm -r`. No source edits, dependency fetching, commits, or production activation occurred.

## 2. Invariant analysis

**First-use setup:** The README clearly identifies an unpublished, prepared-workspace-only package, the qualified sibling dependencies and exact revisions, the working directory, toolchain, and isolated output directory. The documented formatting, testing, linting, and cleanup sequence completed without changing source state.

**API placement and inputs:** `Repository::open_exact`, `ObjectId::parse_hex`, and `read_commit` describe their jobs directly. Repository location and object format are explicit inputs. Full hexadecimal IDs exclude revision syntax and abbreviations. No implicit format or undocumented optional setting was exposed.

**Ownership and cleanup:** Repository owns one native handle, is Send but not Sync or Clone, and permits sequential cross-worker ownership transfer. Independent handles may operate concurrently. Drop releases resources; returned records own their memory, while path accessors borrow from Repository. The public lifecycle needs no additional close operation.

**Read and error semantics:** The guide distinguishes invalid ID representation, format mismatch, repository opening, and object-read failures. Native code/class/message remain diagnostic data rather than stable message-text match keys. Commit reads do not peel tags, resolve refs, or manufacture empty records. Missing referenced parents/trees do not invalidate returning their IDs.

**Read-only boundary:** Exact opening does not search parents, create repositories, or claim path-security guarantees. Reads are not snapshots. The public contract excludes subprocesses, network access, lazy fetch, credential prompts, and ref/index/worktree or process-state mutation. Named integration tests supporting these claims passed; their internal coverage was not inspected.

**Record fidelity and limits:** The guide defines raw bytes, parent order, time fields, encoding behavior, and omitted serialization details. It does not promise unlimited object size or recoverable allocation failure.

**Scope honesty:** The package remains disconnected from production core/CLI. Future operations, source distribution, dependency activation, and platform qualification remain separate gates. The README does not present successful local tests as completion of those outcomes.

## 3. Risks and next action

This review establishes a working documented qualification path and a coherent public contract, not an independent implementation audit. Test names and passing results do not establish coverage completeness. The `no_run` example compiled but was not executed as a standalone consumer.

**Next action:** Record Surface GO for this exact G0 tuple, retaining the source-distribution, production-activation, and platform gates.
