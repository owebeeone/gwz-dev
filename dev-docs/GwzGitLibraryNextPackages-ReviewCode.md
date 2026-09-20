# gwz-git Next Evidence Packages — CODE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzGitLibraryNextPackages.md` at core `5b05768c98f246a4f7ac8c00d5961ba19ebc5310`; status **scope proposal awaiting Code/State review**, dated 2026-09-21.  
**Baseline:** root `74fa1855fcc5ae336ba9edc4541e164b5c6fc45a`; core `5b05768c98f246a4f7ac8c00d5961ba19ebc5310`; library `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Scope feasibility, ownership, isolation, source readiness, and compatibility with actual call paths. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings. C1, H1, and Q1 are bounded and executable without a runtime, dependency, or public-API delta.

---

## 0. Evidence base

I read the complete scope, accepted G0/design/no-fallback records, L3-A/L4-A reports, and relevant immutable source:

- current commit/tag subprocess routes and test wiring;
- existing commit/tag characterization module;
- path-history routing, cursor, fixtures, native oracle, and existing characterization;
- core, repo-inspect, local-testrepo, CLI, proof-package, and `gwz-git` manifests and locks;
- source-admission runner and its guards.

The root lock pins the additionally inspected CLI at `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`. No build or experiment was run.

The exact tuple was unchanged at both boundaries. End trees were root `2a54b587a0deb596014b9caf7d1b064caef6ee72`, core `c3942b3ecd68599e202a0d04049df8d52553818c`, library `a32df35d6ba9c27019ae2a4928325d25bef41a29`, fork `28fc6ad905653db47a7705706e1edea0f39d7054`, and C `af52de522aacf3593250c45159a038801153c6b5`. Worktrees remained clean except for the two allowed owner-generated prompts.

## 2. Invariant analysis

**Ownership:** The three packages have disjoint lane files and explicit ceilings. C1 adds one test file and one declaration inside the existing braced `cfg_if!` test boundary. H1 extends its already-wired private test module without changing helper visibility or adding wiring. Q1 owns one future document. Stop conditions prohibit production bytes, dependencies, locks, options, public types, process-global mutation, and unsupported parity claims.

**C1 feasibility:** The actual backend invokes `git commit` and `git tag` through inherited child state, while git2 can inspect stored signatures, timestamps, messages, refs, index entries, and objects independently. A self-reexecuted exact test can receive fixture-owned global config and case-specific identity/date variables without mutating the parent environment. Fresh staged edits distinguish message rejection from empty-tree rejection. The required snapshots cover HEAD, index/blob, worktree, object aftermath, and any message-file side effect. The scope correctly excludes already-characterized hooks/signing and operation-provided identity.

**H1 feasibility:** Existing private helpers already construct attribute histories, manifests, native ordered-ID oracles, and current cursor IDs. Routing preserves the complete magic prefix while rerooting a direct `member/path` payload; workspace fan-out separately synthesizes `"."`, and the scope records that unresolved distinction. Bare repositories are accepted by the current opening path, and fixture-local bare clones plus `info/attributes` can exercise positive and unspecified forms without network access. Exact-test child execution, deterministic IDs/results, error observations, and post-read snapshots are explicitly required.

**Q1 readiness:** The proposed inventory corresponds to real divergence that must be qualified rather than assumed away: core currently selects git2 0.21 with HTTPS/SSH/SHA-256 and registry sys 0.18.8+1.9.7; repo-inspect/local-testrepo select reduced SHA-256 features; CLI has a default-feature dev dependency and currently locks sys 0.18.5+1.9.4; `gwz-git` selects the local vendored fork/C tuple. Q1 requires exact-source proof, lock/provider uniqueness, runtime version checks, isolated consumers, native platforms/object formats, packaging, licensing, and remote-only reconstruction before any activation. It also names the Windows proof-path normalization defect as a required regression/correction rather than inferring parity.

The scope does not claim that omitted evidence, publication, fallback removal, or activation is complete. Its per-package review order preserves those gates.

## 3. Risks and next action

The 400-line C1/H1 limits require compact fixture reuse, and Q1 will expose rather than immediately resolve the current consumer/source differences. Those are deliberate package constraints, not feasibility defects.

Accept this scope, then implement C1, H1, and Q1 within their stated ownership and submit each committed result to its named focused review gate.
