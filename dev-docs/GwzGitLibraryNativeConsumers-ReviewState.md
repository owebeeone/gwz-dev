# GwzGitLibraryNativeConsumers — State-Axis Review

**Review object:** Q3 instrumented native-consumer qualification at core `468fd5e41fe369cc2892a330c8b2df9241023d38` and evidence `9c2daec1652f22b1c135e8205f794f38db514f37`  
**Baseline:** root `9008e13262d4e24f1cbef150a3717dd79ca31a22`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; taut-shape-rs `df1303656583121e043f7a925c02958571951735`  
**Date:** 2026-09-21  
**Axis:** Running-artifact identity, fixture isolation, negative-control attribution, and evidence provenance. Independent, adversarial, read-only; no current peer output was consulted.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

Read the complete public Q3 scope/report, probe documentation and all 168 Rust lines; private 196-line native runner and 41-line parser suite; native-a README, frozen inputs, composition result, instrumentation maps, source snapshots, command/environment records, artifact hashes, positive outputs, stock-control stderr, and final results.

The retained data shows five distinct target directories and artifacts. Each successful output reports vendored libgit2 1.9.7, the required capability profile, and complete SHA-1/SHA-256 frames. Their deterministic IDs agree. The stock executable built and exited 101 at the intended second-fetch panic, with retained stderr identifying `InvalidSpec (-12)`, rather than at build, startup, version, or fixture creation.

Ran only the permitted pure parser suite:

```text
python3.14 -B -m unittest discover \
  -s gwz-core-evidence/campaigns/git-library/runner -p test_native.py
```

Result: 3 passed. No campaign or build was rerun. Start and end tuple checks matched; only the two allowed generated prompts were untracked.

## 1. Invariant analysis

Artifact routing is closed. The runner starts from Q2’s independently resolved copies, adds recorded instrumentation, builds each row into its own fresh target directory, executes the exact resulting path, hashes that artifact before execution, and confirms both source and artifact hashes afterward. Python explicitly loads the recorded dylib path and invokes its campaign-only function. CLI entry-point instrumentation is guarded by an exact original-source assertion and the campaign environment variable.

The probe itself makes no Git subprocess or network call. Runtime PATH is an empty directory; HOME, XDG and Git global/system configuration are isolated. Every probe receives a distinct newly-created fixture root. Bare repositories, objects, refs and malformed graft files therefore remain confined to disposable external state.

The native observations are substantive. Runtime code asserts version 1.9.7 and vendoring, reports compiled HTTPS/SSH capabilities, creates and reads both object formats, preserves raw message and identity data through `gwz-git`, independently checks the known blob hash in Python, and requires the corrected second local fetch while preserving the noncommit hint ref. Malformed grafts must retain owned error kind plus native class 36/code -1.

Control attribution is credible. The control begins as the admitted minimal-library copy, replaces only its C subtree from immutable stock commit `49e408b…`, retains the same Rust binding, library, lock, instrumentation and build route, and uses a separate target. Its exact panic marker proves arrival at the intended fetch boundary; raw stderr supplies the reported class/code. The patched same-version rows then pass the identical probe for both formats.

The runner is fail-closed: unexpected control success or failure elsewhere rejects the control, any positive nonzero exit or parse error fails its row, missing/duplicate frames are rejected, source or artifact drift fails, and the final result requires all six scheduled rows. The original fork passes admission after execution. Failed or partial records would remain explicit in the incrementally saved JSON.

Public claims match the raw record and clearly distinguish instrumented evidence from unchanged production binaries. Parser results are not presented as native execution.

## 2. Risks and next action

This closes only the macOS arm64 instrumented-consumer checkpoint. Other native targets, unchanged release artifacts, broader Git/network operations, remote-only source reconstruction, publication, activation and fallback removal remain accurately pending. Continue with those separate qualification gates; Q3 alone does not authorize production source or runtime changes.
