# L2-A Binding Member Port — SURFACE-AXIS REVIEW

**Review object:** Source-mode qualification input at core `f9794d55a7c9e483fbf5c503a5e48dd1117b8507`; public native-binding README and runner help.  
**Date:** 2026-09-20  
**Axis:** Public input selection, prerequisites, defaults, ownership and cleanup. Independent, adversarial, read-only; no peer report informed this verdict.

**Baseline:**

| Repository | Exact SHA |
|---|---|
| root | `bc4cddcfe9da7a44494d3c6632b41e3a87190165` |
| core | `f9794d55a7c9e483fbf5c503a5e48dd1117b8507` |
| git2-rs | `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` |

Read the README from its immutable core commit. All five HEADs matched at start and end. The reviewed fixture had no tracked working changes; `git2-rs` remained clean. Concurrent work outside the frozen review scope was excluded.

**Verdict: GO** — P0: 0 · P1: 0 · P2: 0 · P3: 0. No Surface findings.

---

## 0. Evidence base

Read the entire committed `tests/transport_native/README.md`, including “Proposed safe API” and “Reproduce,” and the supplied Surface prompt. Examined observable help only; no implementation, internal design, plan, or peer report was read.

Executed from the workspace root:

```sh
python3 gwz-core/tests/transport_native/prove.py --help

python3 gwz-core/tests/transport_native/prove.py --git2-source git2-rs

python3 gwz-core/tests/transport_native/prove.py \
  --git2-archive /Users/owebeeone/.cargo/registry/cache/index.crates.io-1949cf8c6b5b557f/git2-0.21.0.crate
```

**Results:** Both qualification modes passed all seven native tests, with no failures or ignored tests. Both printed `cargo +1.95.0 test --offline --locked` against temporary copied sources.

Source mode identified release:

```text
dffaf272eb0e62ac15b74283c4e488252db9afc3
```

Both modes reported matching provenance:

```text
reference_archive_sha256=ddddbf932745a6be37109b6112d3ee09696106f848449069d3a57bba937ab82e
patch_sha256=51586ea398130dbb36e3c9f59a3117b5c19b3a5c223d5bb62e266077a8d6c3e6
```

Git revision, scoped diff and status checks confirmed the settled review inputs. No edits, commits, or network fetching were performed.

## 2. Invariant analysis

**Input placement and defaults:** `--git2-source` belongs alongside `--git2-archive` in the existing qualification runner. Help presents a required, mutually exclusive choice. The README explicitly says neither input has a default. The toolchain selector’s purpose and `1.95.0` default remain visible.

**First-day walkthrough:** Prerequisites, dependency preparation, working directories, archive selection, multiple-registry ambiguity, and the alternative member-checkout command are documented. Both documented input paths completed successfully using existing cached dependencies.

**Source admission contract:** The README identifies the exact release commit, the two pinned binding files, and the sole permitted manifest-edge change. It explicitly describes rejection of unrelated, extra, missing, and substituted files, and enumerates excluded directories. No hidden assumption about qualifying the fork’s auxiliary workspace is imposed.

**Isolation and compatibility:** Both commands visibly compiled temporary paths. Source qualification left the member clean, and archive qualification retained its working public interface. Runtime logs showed the registry-to-local git2 replacement. These observations support the public isolation contract without constituting an implementation audit.

**Lifecycle:** Temporary sources are documented as removed on success or failure. Retained build products and their cleanup directory are identified. There is no installed service or registration needing an inverse command. Cleanup deletion was not exercised against shared build outputs.

**Scope honesty:** The README distinguishes qualification from production activation, publication, SSH/pool integration, and platform parity.

## 3. Risks and next action

Adversarial drift rejection, complete dependency-graph validation, cache immutability, and cleanup internals were not independently inspected or mutation-tested on this restricted Surface axis. Successful public commands alone do not prove those implementation properties.

**Next action:** Record Surface GO for the exact tuple. No Surface remediation is required.
