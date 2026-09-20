# GwzGitLibraryCandidate — State-Axis Review

**Review object:** Q2 verifier correction, isolated consumer-composition campaign, and public candidate report at core `ea059ba89b1b61201b75c26708a99c7ca780d2a9` and evidence `c82e38394947611b3848c9e73701ac378ae2917c`  
**Baseline:** root `7c017a5f1c3db1a743e5e1a0a62413fa5130755b`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; taut-shape-rs `df1303656583121e043f7a925c02958571951735`  
**Date:** 2026-09-21  
**Axis:** Source admission, isolation, lock drift, and retained-evidence integrity. Independent, adversarial, read-only; no current peer output was consulted.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

Read the canonical prompt; complete `GwzGitLibraryCandidate.md` and qualification map; core diff `f70f4a74…ea059ba8`; corrected `prove.py` and all guard tests; private campaign policy, maintained 231-line runner, five run records, frozen runner copies, result JSON, locks, source fingerprints, raw logs, and manual negative guard record.

The recorded runner/proof/pin hashes match the committed frozen files. Raw result statuses and commands support the public table: `local-b` supplies successful root/core/CLI/library build evidence while retaining Python failure; `python-d` supplies the successful maturin/import row; `metadata-e` uses the final runner and records five successful metadata rows. Earlier failed attempts remain failed.

Ran only the permitted guard command:

```text
PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover \
  -s gwz-core/tests/transport_native -p test_prove.py
```

Result: 12 passed. No campaign or build was rerun. Start and end tuple checks matched; only the two allowed generated prompts were untracked.

## 1. Invariant analysis

The path-flavour attack failed. `verify_copy` now derives comparison and exclusion keys with `Path.as_posix()` while retaining native paths for filesystem access. The Windows-flavour regression exercises nested directory and file-form `.git` exclusions, unexpected and missing files, content drift, and symlink substitution. Its POSIX fixture preserves a literal backslash-bearing filename, avoiding the corruption that a textual backslash replacement would introduce. File type, executable-mode parity, content, symlink target, and nested C gitlink checks remain in the admission path.

Source admission is fail-closed. Immutable Git trees and blobs define expected content; export-ignore attributes cannot hide tracked files. The selected Rust/sys/C revisions, live nested gitlink and C checkout are checked. Extra, missing, substituted, mode-changed, or content-changed inputs fail. Admission runs before each candidate row and again after the run, while source snapshots ensure builds do not change admitted copies.

The five Cargo roots are isolated in fresh external directories with independent locks and offline resolution. Root workspace, standalone core, standalone CLI, Python, and library metadata each assert the actual workspace root; selected package manifests must point at the admitted paths. Each graph has exactly one git2 package and one native `links = "git2"` provider. Recorded feature unions show vendored SHA-256 everywhere and HTTPS/SSH where required.

Lock normalization permits only the stated additions and source/version transitions. It removes the candidate-only gwz-git edge and admitted provenance differences before exact structured comparison; duplicate providers, foreign packages, extra library dependencies, wrong native versions, and the observed tempfile/getrandom edge change are rejected.

Evidence separation is sound. Runtime sources, targets and wheels remained outside both product and evidence repositories. Raw failures, outputs, commands, lock pairs and hashes are retained privately; the public report summarizes them without making the private archive a build dependency. Build/metadata success is not presented as runtime native-identity, remote-distribution, Windows, all-platform, or activation proof.

## 2. Risks and next action

The candidate proves local macOS arm64 source and graph composition only. Actual linked-native identity, SHA-1/SHA-256 operation rows for every consumer, native Windows and remaining platform execution, remote-only packaging, publication, activation and fallback removal remain accurately pending. Proceed to those separately reviewed qualification gates without changing production manifests or locks on Q2 evidence alone.
