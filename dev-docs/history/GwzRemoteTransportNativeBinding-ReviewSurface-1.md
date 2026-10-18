# Native Binding Prerequisite — Surface Closure Review

**Date:** 2026-09-20  
**Scope:** Focused closure of Surface P3-1.  
**Verdict: GO**  
**Remaining findings:** P0: 0 · P1: 0 · P2: 0 · P3: 0

## Exact tuple

| Repository | Commit |
|---|---|
| root | `d798d201b4d9d0e8f02d55b415efd3c725bfee3a` |
| gwz-core | `7b03091941f047c61f8261fd12c451bca6db49d7` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` |

Verified unchanged at review start and end. All four working trees were clean at completion.

## Prior-finding closure

| Finding | Required correction | Evidence | Status |
|---|---|---|---|
| P3-1: Toolchain option lacked a stated default | Explain selector purpose, default, and relationship to qualification baseline | Help now states `rustup toolchain selector (default: 1.95.0)`. README explains overrides provide additional qualification without replacing the recorded baseline. Invocation without the option selected `cargo +1.95.0`. | **Closed** |

## Changed-range analysis

Core range `fe815856291a93fa4ecdf0ab5879984d7b5ba1ee..7b03091941f047c61f8261fd12c451bca6db49d7` changes only the public README and `prove.py`: six insertions and one deletion.

Reviewed the README and observable help output. No implementation, design, plan, or peer report was read. The revised public surface resolves the missing-default ambiguity without introducing a new lifecycle or configuration obligation.

## Evidence

Executed:

```sh
python3 gwz-core/tests/transport_native/prove.py --help

python3 gwz-core/tests/transport_native/prove.py \
  --git2-archive /Users/owebeeone/.cargo/registry/cache/index.crates.io-1949cf8c6b5b557f/git2-0.21.0.crate
```

The proof printed `cargo +1.95.0 test --offline --locked` and passed **all seven tests**. Reported archive and patch digests matched the previous review.

Also checked the changed-file summary, repository revisions, and final working-tree status. No edits or commits were made.

## Remaining risks and next action

This closes the documentation finding; it does not broaden qualification to production integration, real SSH, or other native platforms.

**Next action:** Record P3-1 as closed. No Surface findings remain open at the verified tuple.
