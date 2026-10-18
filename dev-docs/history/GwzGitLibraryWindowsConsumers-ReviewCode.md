# Q5 Windows Four-Consumer Qualification — Code-Axis Review

**Review object:** Q5 Windows four-consumer qualification at core `54a04a381272806d9f6bddd2c319294ff63bcc79`, controlled by `gwz-core/dev-docs/GwzGitLibraryWindowsConsumers.md`; native execution passed, review pending, 2026-09-21.  
**Baseline:** root `97b01e752a6ac7709970f49daa24f017e762aebf`; core `54a04a381272806d9f6bddd2c319294ff63bcc79`; evidence `0c1c34b85064bff2d8e75536ce3351063f74316b`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable commits with `git show` and `git diff`.  
**Date:** 2026-09-21  
**Axis:** Code architecture, graph and artifact identity, executable routing, compatibility, and claim accuracy. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings.

---

## 0. Evidence base

The complete tuple and tree objects matched before and after review. Core, evidence, gwz-git, git2-rs, and libgit2 were clean. Root contained only the two explicitly excluded generated prompts.

I inspected:

- `GwzGitLibraryWindowsConsumers.md:1-106`, the Q1 pointer, Q3 contract, evidence policy, and root checkpoint.
- `runner/windows_consumers.py:1-191`, its 59-line guard suite, the reused Q2 composition, Q3 native-result, and corrected Q4 Windows helpers.
- Q5’s frozen preparation scripts, path guards, exact adaptation record, composition results, locks, source manifests, and instrumentation bytes.
- The native Windows state, commands, environments, metadata, build logs, probe output, before/after snapshots, helper hashes, artifact hashes, and owner cross-check.
- The permitted pure guards: four consumer-runner tests passed, the Cargo-configuration test passed, and all preparation path counterexamples passed.

No build or remote campaign was rerun.

## 1. Invariant analysis

Each consumer has an independent admitted source root, candidate lock, Cargo home, target directory, and native build. The retained Windows metadata for workspace CLI, standalone core, standalone CLI, and Python resolves exactly one local git2 `0.21.0`, one local libgit2-sys `0.18.8+1.9.7`, one local gwz-git `0.1.0`, and one `links = "git2"` provider. All four graphs include vendoring, SHA-256, SSH, and HTTPS features. Their lock records contain the same unique local chain, while Q2’s normalization proves that resolution changed only the intended git2/sys provenance and gwz-git edge.

Build logs identify the corresponding row-local Rust/sys/library/core sources. Fresh targets prevent stale artifact selection. The two CLI executables, core example, and Python DLL were taken from their respective targets and hashed before execution. Python was loaded directly through `ExtensionFileLoader`, checked `health() == "ok"`, and invoked the campaign probe from that loaded module. Every artifact retained the same hash after its probe.

All four probes executed with an empty PATH and reported vendored libgit2 1.9.7 with SSH/HTTPS enabled. The unchanged Q3 validator independently checks framing, object-format widths, canonical blob hashes, distinct commits, corrected noncommit-hint fetch, and raw diagnostic class. Every row’s SHA-1 and SHA-256 observations exactly match the accepted Q3 macOS observations. Thus the evidence connects the selected graph to executed native artifacts rather than relying on lock or version strings alone.

Composition exported pinned Git objects, admitted the exact Rust/C/library sources, and recorded the current committed public probe. The instrumentation mechanism and CLI/Python entry-point bytes match Q3; the probe bytes incorporate Q4’s accepted Windows file-URL correction. The transfer manifest covers 53,653 files and 20 real symlinks across the four roots. Extraction rejects unsafe, colliding, reserved, escaping, or type-mismatched paths, hashes every regular file, recreates exact symlink targets, and verifies each complete snapshot. Native before/after records are byte-identical for every row.

The runner uses fresh D: roots and separate source, result, build, cache, home, temp, and fixture paths. Before and after all twelve Cargo acquisition, metadata, and build invocations, the corrected Q4 helper checked both Cargo configuration names through the working-directory ancestry and Cargo home. All recorded searches were nonempty, unchanged, and found no configuration. Registry acquisition was explicit; metadata and builds were locked and offline. Git and Python configuration was fixture-owned. Frozen helper and runner hashes match the executed records.

The public claims match this evidence. Q5 qualifies four instrumented Windows consumer artifacts. Together with Q4 it covers five instrumented consumer shapes, while ordinary command behavior, installed Python/wheel behavior, other platforms, independent source distribution, publication, activation, and fallback removal remain expressly open.

## 2. Risks and next action

These are instrumented debug artifacts built from external admitted copies; they are evidence for native composition and probe behavior, not unchanged release packages. The remaining package, platform, distribution, and activation gates therefore remain material but are accurately deferred.

If the independent parallel review also returns GO, accept this bounded Q5 checkpoint without promoting any deferred production or release-readiness claim.
