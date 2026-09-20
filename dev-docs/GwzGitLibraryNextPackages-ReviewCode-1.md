# gwz-git Next-Packages Q1 Scope Correction — CODE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzGitLibraryNextPackages.md` at `6586768396886fe1aeb1371bbd3064377cfa70ec`; DRAFT scope correction awaiting State closure; 2026-09-21  
**Baseline:** root `3589aa05092abb7c38d90747384954565510301f`; gwz-core `6586768396886fe1aeb1371bbd3064377cfa70ec`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. All sources were read from immutable commits.  
**Date:** 2026-09-21  
**Axis:** Code — focused verification of Q1 consumer enumeration, pinning, and qualification gates. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — the prior omitted-consumer defect is closed. No open P0–P3 findings.

---

## 0. Evidence base

Reviewed the prior State finding, merged remediation plan, and core change from `5b05768c98f246a4f7ac8c00d5961ba19ebc5310` to `6586768396886fe1aeb1371bbd3064377cfa70ec`.

Inspected:

- `GwzGitLibraryNextPackages.md:118-146`, including the revised Q1 scope.
- Root `gwz.conf/gwz.lock.yml:15-18,24-27,51-54`.
- Pinned `gwz-py` `Cargo.toml` and `Cargo.lock` at `d07d55dacb1725d9306be9c04d157ac29a78e000`.
- Pinned standalone CLI `Cargo.toml` and `Cargo.lock` at `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`.

No build or test execution was required or performed.

## 2. Invariant analysis

| Prior finding | Status | Closure evidence |
|---|---|---|
| State P2-1: Q1 omitted the active indirect Python consumer and its independent dependency graph | **CLOSED** | Q1 now pins `gwz-py`, identifies its independent manifest and lock, records its `git2 0.21.0` / `libgit2-sys 0.18.5+1.9.4` resolution, and requires independent provider, platform, and package qualification. |
| Standalone CLI graph could be conflated with root-workspace resolution | **CLOSED** | Q1 separately pins the CLI revision, records its standalone `libgit2-sys 0.18.5+1.9.4` lock result, and requires distinct inspection. |
| Another active consumer could remain silently omitted | **CLOSED at scope level** | Q1 requires enumeration of every active GWZ member, direct and indirect dependency inspection, and documentation of nonconsumers. Any differing or unqualified consumer keeps activation pending. |

The pinned source confirms both Python and standalone CLI are core consumers with independent locks resolving the recorded older native provider. The correction therefore captures the concrete compatibility risk instead of implying qualification. It preserves the separate all-consumer, all-feature, native-platform, publication, and activation gate. C1 and H1 are unchanged.

Start and end tuple checks matched exactly, including trees: root `5bd77d8a82626f7aaf5c8015244cc0ce10a642b6`, core `0d68ebe795e741adfd76f14fa3eff39f67818e8a`, gwz-git `a32df35d6ba9c27019ae2a4928325d25bef41a29`, git2-rs `28fc6ad905653db47a7705706e1edea0f39d7054`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Only the two declared generated root prompts were untracked.

## 3. Risks and next action

Actual cross-consumer qualification remains future Q1 work; this review accepts only its corrected scope. Proceed with the gated packages, retaining activation as pending until every enumerated consumer passes its independent lock, provider, platform, and package checks.
