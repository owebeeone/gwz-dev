# GwzGitLibraryEvidence C1 — State-Axis Review

**Review object:** C1 implementation at `gwz-core` `deba48c93a04e6aaf0bab36066b12d1547d5469e`: identity/date/message characterization, braced test-module wiring, and `GwzNoFallbackCommitTagDesign.md` evidence appendix  
**Baseline:** root `9fc664de8389ea334f36bc41135cd59448893e05`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`  
**Date:** 2026-09-21  
**Axis:** C1 state isolation, stored-object observations, rejection typing, and failure aftermath. Independent, adversarial, read-only; no current peer output was consulted.

**Verdict: GO** — no P0–P2 findings; one bounded P3 evidence-precision finding.

---

## 0. Evidence base

Read the generated prompt, accepted NextPackages scope, immutable core diff `65867683…deba48c9`, all 394 lines of `commit_tag_identity_characterization.rs`, the one-line `cfg_if!` wiring change, and the C1 evidence appendix.

Ran the permitted Rust 1.95 locked offline focused command:

```text
cargo +1.95.0 test --locked --offline --lib characterization -- --nocapture
```

Result: 17 passed, 0 failed, 0 ignored. Each C1 parent observed exactly one successful exact-name child execution. Start and end tuple checks matched; only the two allowed generated prompts were untracked.

## 1. Findings

### [P3-1] C-EMPTY overattributes and does not independently snapshot the second raw-index transition

**Location:** `src/git/gitbackend/commit_tag_identity_characterization.rs:369-389`; `dev-docs/GwzNoFallbackCommitTagDesign.md:177-185`.

The test snapshots raw index bytes once, runs empty and whitespace-message failures sequentially, and checks raw bytes only after the first failure. It proves that the first failure changes raw index bytes while logical path/mode/OID entries remain stable. It does not parse the changed extension, so the stronger documentation claim that Git specifically “refreshes the index TREE cache” is inferred rather than asserted. Because the whitespace case begins from the first case’s already-modified index and empty `COMMIT_EDITMSG`, it also cannot independently characterize that case’s administrative-file transition.

The typed rejection and protected state are still proved for both cases, so this does not block C1. Before this evidence is used to freeze mutation parity, either:

- use a fresh staged repository and before/after raw snapshots for each message variant, parsing or otherwise proving the TREE-extension change; or
- describe only the observed fact that raw index bytes changed on the first rejection and leave the exact extension and per-variant transition pending.

A closure regression should fail if either variant changes refs, reflog, logical index entries, staged blob, or worktree, while recording its own raw-index and `COMMIT_EDITMSG` before/after state.

## 2. Invariant analysis

The isolation attack held. Each child clears ambient environment, supplies fixture-owned global/system configuration, disables prompting, signing, hooks and auto-GC effects, and changes neither process-global environment nor cwd. Exact-name output counting prevents a zero-test success.

Identity precedence is observed from stored commit/tag signatures: repository over global, author and committer environment overrides independently, tagger follows committer identity, and global fallback is fixture-owned. Fixed author/committer offsets and seconds survive into the commit; annotated tagger date and lightweight object kind are separately asserted.

Message evidence reads the raw commit object after its header separator, preserving the leading blank line that the parsed accessor omitted. Default, strip and verbatim cleanup results are asserted byte-for-byte.

Both empty-message variants return `GitCommandFailed`. HEAD, branch ref, HEAD reflog, logical staged entry, staged blob and tracked worktree bytes remain unchanged; the final empty `COMMIT_EDITMSG` and first raw-index-byte change are positively observed. No rollback guarantee or product behavior change is claimed.

The source stays within its 400-line ceiling, and its declaration is inside the existing braced conditional boundary. No runtime, API, dependency, lock, H1, or Q1 behavior entered the State-owned change.

## 3. Risks and next action

macOS arm64 is the only executed platform, and signer, hook, filter, SHA-256 mutation, interruption and concurrent-ref rows remain explicitly deferred. Accept C1 at this State gate and carry P3-1 into the next commit/tag evidence or mutation-design checkpoint before treating administrative failure side effects as frozen parity.
