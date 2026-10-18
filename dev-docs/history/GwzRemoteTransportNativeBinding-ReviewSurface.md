# Native Binding Prerequisite — Surface Review

**Date:** 2026-09-20  
**Axis:** Surface  
**Object:** Phase 3a per-Remote git2 binding qualification prerequisite.

**Verdict: GO**  
**Open findings:** P0: 0 · P1: 0 · P2: 0 · P3: 1

The public contract clearly describes ownership, explicit transport mode, retained-Remote behavior, errors, and teardown. The documented qualification command passed. One nonblocking help defect remains.

## Exact tuple

| Repository | Commit |
|---|---|
| root | `d7b1b04d35acd73dd85ec553a68d4e498c120f21` |
| gwz-core | `fe815856291a93fa4ecdf0ab5879984d7b5ba1ee` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` |

All four commits matched at both start and end. Member working trees remained clean. At completion, root contained only three authorized untracked reviewer prompts; peer contents were not read.

## Evidence base

Read:

- `AGENTS_GWZ.md` and the supplied Surface prompt for process instructions.
- [Public native-binding README](/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/transport_native/README.md), in full.
- `python3 gwz-core/tests/transport_native/prove.py --help`.

No implementation, design, plan, checkpoint, or peer report was read.

Executed:

```sh
# From gwz-core/tests/transport_native:
cargo +1.95.0 fetch --locked

# From workspace root:
python3 gwz-core/tests/transport_native/prove.py \
  --git2-archive /Users/owebeeone/.cargo/registry/cache/index.crates.io-1949cf8c6b5b557f/git2-0.21.0.crate
```

Both commands succeeded. The proof reported **seven passing tests**, covering registry coexistence, errors/panics, retained Remote behavior, distinct contexts, upload/receive ownership, networkless routes, and native file transport.

The runner reported:

```text
archive_sha256=ddddbf932745a6be37109b6112d3ee09696106f848449069d3a57bba937ab82e
patch_sha256=51586ea398130dbb36e3c9f59a3117b5c19b3a5c223d5bb62e266077a8d6c3e6
```

Repository revisions and status were checked before and after. No files were edited or committed.

## Finding P3-1 — Toolchain option has no stated default

**Location:** Public `prove.py --help`, the `--toolchain TOOLCHAIN` option; [README reproduction instructions](/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/transport_native/README.md:41).

**Root cause:** The help exposes `--toolchain` without a description or default. The README requires Rust 1.95.0 but does not explain this override or explicitly state what omission selects.

**Violated invariant:** Every optional public setting must state its default, particularly when it controls a reproducibility check.

**Reproduction:** Run `prove.py --help`. It displays:

```text
--toolchain TOOLCHAIN
```

Neither its meaning nor default appears. Running the documented proof without that option subsequently reveals `cargo +1.95.0 test` in the execution log.

**Impact:** Before executing the qualification, a help-only reader must guess whether the runner uses a fixed version, the active rustup default, or another selection mechanism. This is a bounded documentation and diagnosability defect, without an API compatibility consequence.

**Required correction:** Describe `--toolchain` as the rustup toolchain selector and state its default explicitly. Explain its relationship to the qualified baseline in the README if overrides are intended for additional qualification.

**Closure test:** Verify that `prove.py --help` states the selector’s purpose and default, and that an invocation omitting it prints a matching Cargo toolchain.

## First-day walkthrough and invariant analysis

- **Setup and use:** The README states prerequisites, working directories, locked dependency fetch, archive selection, and the multiple-registry fallback. The documented sequence succeeded. It explicitly warns that direct stock-manifest tests lack the proposed method.
- **Placement and naming:** A dedicated qualification runner is appropriate for this unpublished test package. Its summary accurately separates qualification from production dependency changes.
- **Mode:** `rpc` is mandatory; false/stateful and true/stateless meanings are explicit. No implicit API mode must be inferred.
- **Ownership:** Borrowed Remote access ends after construction; returned transports and streams must own their state. Ownership transfer to libgit2 is stated.
- **Retained Remote:** Disconnect is explicitly insufficient to replace transport context. A fresh Remote is required to change route/context; dropping the Remote frees retained ownership.
- **Errors:** Message/code/class preservation and panic handling across the C boundary are documented.
- **Cleanup:** Temporary-source cleanup and the retained `target/qualified` directory are identified. Build-output deletion was not exercised during this shared, read-only review.
- **Scope honesty:** The README disclaims production activation, SSH/authentication/pooling, and native-platform parity.

## Residual risks and next action

This review assesses the public surface and executable walkthrough, not implementation correctness. Passing local tests does not establish production integration or platform parity.

**Next action:** Correct P3-1 and recheck help output. No blocking Surface finding remains at the verified tuple.
