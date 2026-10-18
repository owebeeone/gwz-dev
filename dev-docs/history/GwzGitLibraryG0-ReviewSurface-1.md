# GWZ Git Library G0 — SURFACE-AXIS RE-REVIEW

**Review object:** Focused G0 remediation re-verdict: revised public qualification instructions and source identity.  
**Date:** 2026-09-21  
**Axis:** Public setup, dependency qualification, defaults, and cleanup. Independent, adversarial, read-only; no peer report informed this verdict.

**Baseline:**

| Repository | Exact SHA |
|---|---|
| root | `699c584a93185ef5e73dc96318603e25354018e2` |
| gwz-core | `2a5bd773df04450148c7630e01913edba2bbedb8` |
| gwz-git | `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c` |
| git2-rs | `ce78628308e11b4e8901d5061602619109bce21a` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All five revisions matched at start and end. The nested C checkout also matched the pinned C revision. Workspace status showed only the three authorized untracked review prompts.

**Verdict: GO** — P0: 0 · P1: 0 · P2: 0 · P3: 0.

---

## 0. Evidence base

Read immutable versions of the complete:

- `gwz-git/README.md`.
- `gwz-core/dev-docs/GwzGitLibraryApi.md`.
- `gwz-core/tests/transport_native/README.md`.

Also read the supplied Surface prompt. No implementation, test source, design, checkpoint, remediation plan, or peer report was read.

Ran the documented library test command from `gwz-git`, with fetching disabled and outputs outside source:

```sh
CARGO_TARGET_DIR=/tmp/gwz-git-g0-review-surface-1 \
CARGO_NET_OFFLINE=true \
rustup run 1.95.0 cargo test --locked
```

**Result:** 13 integration tests and seven documentation checks passed. Integration output included:

- `stored_parents_ignore_shallow_and_explicit_graft_rewrites`
- `malformed_grafts_preserve_the_native_error_class`

Documentation checks comprised one compile-only example and six compile-fail cases. The `no_run` example was not executed.

Removed the isolated target directory afterward. No source edits, fetching, commits, or production activation occurred.

### Prior-finding closure

| Prior Surface result | Current disposition |
|---|---|
| GO with no findings | No inherited Surface finding requires closure. Revised qualification instructions and identity were reassessed. |

This report does not close findings owned by other review axes.

## 2. Changed-range and invariant analysis

**Consistent source identity:** The library README now requires Rust fork `ce78628308e11b4e8901d5061602619109bce21a`, matching the settled checkout and native proof README. The C revision remains unchanged and matches both the sibling repository and nested checkout.

**Qualification scope is explained:** The native README identifies the additional raw-error-class correction, including callback replay, and updates source admission from two to three pinned binding files. It distinguishes that correction from the unchanged per-remote binding files and C identity.

**Inputs and defaults remain explicit:** Exactly one archive/source input is required; neither has a default. Rust 1.95.0 remains the stated toolchain default. Archive mode characterizes the released C dependency’s known fetch failure; source mode qualifies the pinned correction. These outcomes are not conflated.

**Bootstrap limits remain honest:** Source qualification requires the prepared workspace and initialized unpublished C checkout. Publication and clean remote-only reproduction remain later gates. The library’s source-proof command uses the stated core working directory and corresponding sibling path.

**Lifecycle remains complete:** Temporary proof sources and retained build products have documented cleanup behavior. The library walkthrough uses an isolated target directory; its test and cleanup steps completed successfully here. No service or registration needs reversal.

**User API remains unchanged:** The guide retains the accepted opening, ID, record, error, and Send/!Sync/!Clone contracts. Revised dependency qualification does not introduce new product behavior or authorize production activation.

No new Surface defect was identified.

## 3. Risks and next action

The native source/archive runner was not rerun in this focused review; its revised composition was assessed from public instructions and identity checks. Passing library tests supports the executable walkthrough but does not independently audit regression completeness or close other axes’ findings.

**Next action:** Record Surface GO for this exact tuple, retaining the source-distribution, production-activation, and platform gates.
