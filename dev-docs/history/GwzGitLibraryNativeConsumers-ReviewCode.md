# Git Library Q3 Native Consumers — CODE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzGitLibraryNativeConsumers.md` and Q3 implementation at core `468fd5e41fe369cc2892a330c8b2df9241023d38`; private evidence at `9c2daec1652f22b1c135e8205f794f38db514f37`; bounded native-consumer qualification dated 2026-09-21  
**Baseline:** root `9008e13262d4e24f1cbef150a3717dd79ca31a22`; core `468fd5e41fe369cc2892a330c8b2df9241023d38`; evidence `9c2daec1652f22b1c135e8205f794f38db514f37`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; taut-shape-rs `df1303656583121e043f7a925c02958571951735`. All sources and evidence were read from immutable commits.  
**Date:** 2026-09-21  
**Axis:** Code — running-artifact routing, native identity/features, object-format/fetch assertions, diagnostics, and public-claim accuracy. Independent, adversarial, read-only. The other axis ran independently; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

Inspected:

- Core diff `d3278e730db104285e0ac5fe447167a5cd52ed4c..468fd5e41fe369cc2892a330c8b2df9241023d38`.
- Public `consumer_probe.rs:1-168`, its usage record, and `GwzGitLibraryNativeConsumers.md`.
- Private `runner/native.py:1-196`, `runner/test_native.py:1-41`, frozen runner/probe copies, instrumentation maps, source snapshots, build/probe logs, artifact hashes, composition evidence, and `results.json`.
- Stock libgit2 `49e408b3208bc3093757a1c2db938d3590f3f412` versus corrected `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; the production delta is the local-fetch correction, with its native regression.

The permitted pure parser suite passed all three tests.

Start and end tuple checks matched exactly. Root, core, and evidence trees were respectively `9a8abbc31be844d09aca511076f93ef17c8b2fbb`, `f8baa5822cc1df15650cffe67439d11b9faf3a46`, and `e4319f7f78df3baddec101b4eb821bc3203be2f2`. Only the two declared generated prompts were untracked.

## 2. Invariant analysis

The probe exercises actual linked code rather than inferring identity from metadata. It requires vendored libgit2 1.9.7, records HTTPS/SSH capability, creates SHA-1 and SHA-256 repositories, and verifies payload bytes, full-width IDs, tree, parent order, signatures, timestamps, raw messages, fetch results, and owned-error diagnostics.

The noncommit-hint reproduction is specific: the receiver retains a blob ref, the source adds a new commit, and the second fetch must import that commit without disturbing the hint. The same Rust/sys composition with exact stock C 1.9.7 builds, then fails at this fetch with `InvalidSpec (-12)`. The corrected same-version C tree passes both formats for every consumer. This supports attribution to the bounded native correction rather than to a version, build, or routing difference.

Instrumentation reaches each intended artifact: root and standalone CLI entry points, standalone core and library examples, and the explicitly loaded Python extension. Every row has a distinct target and fixture path. Frozen instrumentation bytes, source/lock snapshots, artifact hashes, exact commands, exit statuses, stdout, and stderr are retained. Runtime PATH is empty and Git configuration is isolated; successful source and artifact snapshots remain unchanged after execution.

All five positive outputs contain identical deterministic IDs. The Python parser independently recomputes both Git blob hashes and rejects incorrect identity, capabilities, widths, encoding, duplicate/incomplete frames, fetch status, diagnostic marker, or coincident commit IDs. The public report matches the raw output and accurately labels the artifacts as instrumented local evidence.

No product source, manifest, lock, API, CLI option, wire field, or Q2 composition machinery changed.

## 3. Risks and next action

This run covers instrumented macOS arm64 artifacts only. It does not qualify unchanged release artifacts, other platforms, network operations, remote-only packaging, publication, production activation, or fallback removal.

Proceed to the separately reviewed remaining native platform and reproducible packaging gates before any production source-selection change.
