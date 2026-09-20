# Native Local-Fetch Correction — SURFACE-AXIS REVIEW

**Review object:** Public native-binding qualification runner and README at core `01d6f6624472620c215693243f7ac3865aeb31a4`.  
**Date:** 2026-09-20  
**Axis:** Input modes, prerequisites, defaults, qualification claims, and cleanup. Independent, adversarial, read-only; no peer report informed this verdict.

**Baseline:**

| Component | Exact SHA |
|---|---|
| root | `62c2f122c28ededdefb7af32058f4b058b018dcb` |
| core | `01d6f6624472620c215693243f7ac3865aeb31a4` |
| git2-rs | `4c1caabbce7d56426c763dd94114052302b23e4c` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |
| upstream-facing branch | `fe618d0f5de9e506b9714643afc42d2fcba6e984` |

The README was read through `git show` at the exact core revision. Pinned revisions were checked before and after execution; no movement was observed. The sibling C checkout and initialized C submodule matched. Final working trees were clean apart from the three authorized root review prompts.

**Verdict: GO** — P0: 0 · P1: 0 · P2: 0 · P3: 0. No Surface findings.

---

## 0. Evidence base

Read the complete public `tests/transport_native/README.md`, observable runner help, and supplied Surface prompt. No source, design, plan, or peer report was read.

Executed sequentially from the workspace root:

```sh
python3 gwz-core/tests/transport_native/prove.py --help

python3 gwz-core/tests/transport_native/prove.py --git2-source git2-rs

python3 gwz-core/tests/transport_native/prove.py \
  --git2-archive /Users/owebeeone/.cargo/registry/cache/index.crates.io-1949cf8c6b5b557f/git2-0.21.0.crate
```

**Both modes passed eight tests**, with no failures or ignored tests. Both selected Rust 1.95.0 and printed locked, offline commands targeting temporary copies.

Source mode reported:

```text
member_release=dffaf272eb0e62ac15b74283c4e488252db9afc3
sys_revision=6c93812dbc1c34aef6e6464a645545b4a4299807
c_revision=b172e3d187a4b6866fd9f696f40a1b8e7f56d348
```

Its dependency output showed local replacements for git2 and libgit2-sys without version changes. Archive mode showed only the local git2 replacement.

No edits, commits, publication, or network fetching were performed.

## 2. Invariant analysis

**Mode selection and defaults:** Help requires exactly one archive or source input. The README explicitly states that neither input has a default. The toolchain selector retains its purpose and stated `1.95.0` default. Extending the existing source-input mode is coherent; no new product command or API is implied.

**First-day prerequisites:** The README identifies tools, initial dependency preparation, working directories, exact Rust/sys baselines, and the required initialized C submodule. It explicitly explains that the unpublished backport is unavailable to a fresh remote-only clone and directs source qualification to the prepared workspace. This limitation is disclosed rather than presented as reproducible remote provisioning.

**Qualification claims:** Archive mode is documented as characterizing the released dependency’s known noncommit-hint failure. Source mode qualifies the correction. Accordingly, archive test success does not claim that released C contains the fix. Both documented invocations completed as described.

**Identity and isolation:** The public contract specifies matching C checkout/gitlink identity, checked source bytes and modes, rejected drift, and narrow metadata exclusions. Execution printed the expected source revisions and temporary build locations. Final source-tree status remained clean.

**Lifecycle:** Temporary-source cleanup on success or failure and retained build-output cleanup are documented. There is no installed service, credential setup, or registration requiring an inverse action. Shared build-output deletion was not exercised.

**Scope:** Documentation continues to separate local qualification from production activation, publication, platform parity, and SSH/pool activation.

## 3. Risks and next action

This Surface review does not independently verify admission internals, native correctness, complete dependency-graph comparison, or cleanup implementation. Passing archive characterization is not evidence of a corrected released dependency.

**Next action:** Record Surface GO for the exact tuple. No Surface remediation is required.
