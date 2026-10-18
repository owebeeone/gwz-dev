# GWZ SSPI scaffold — CODE-AXIS REVIEW

**Review object:** Root `ec0dd6d..e6772cb` and initial committed `gwz-sspi` scaffold; controlling DRAFT `dev-docs/GwzSspiScaffold.md` at `e6772cbeda821deb7f2f42d63f73d9a3581e60ec`.

**Baseline:** Root `e6772cbeda821deb7f2f42d63f73d9a3581e60ec`; gwz-sspi `bd807b0403d502fd6f25ef2f93389ce432f0c0c0`; gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Sources inspected through committed `git show` and read-only source inspection. Exact tuple verified unchanged at start and end.

**Date:** 2026-10-03

**Axis:** Architecture, interfaces, dependency isolation, package contents and release compatibility. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0/P1/P2 findings; one nonblocking P3 documentation defect.

---

## 0. Evidence base

Read root `AGENTS_GWZ.md`, canonical Code prompt, process authority and review-loop instructions; root diff; controlling scaffold document; SSPI Design revision 2, Plan, Acceptance, crate-map SSPI amendment and library-boundary policy.

Inspected member Cargo manifest/lock/toolchain, AGENTS, README, RELEASE, Architecture, Testing, Implementation, all ownership notes, library and worker source, bootstrap test, release checker, Gearu configuration and both workflows.

`cargo metadata --manifest-path gwz-sspi/Cargo.toml --no-deps --offline` succeeded: exactly one workspace package, zero dependencies, an independent workspace root, empty default features, feature-gated worker/test and publication disabled.

No builds, tests, release commands, writes or Git mutations were performed. Local build/test claims in the DRAFT were inspected as claims, not independently rerun.

## 1. Findings

### [P3-1] Packaged README links to an excluded implementation checkpoint

**Location:** `gwz-sspi/Cargo.toml:12`; `gwz-sspi/README.md:46`.

The explicit package include list contains `/docs/**` but excludes `/dev-docs/**`, while the shipped README links to `dev-docs/Implementation.md`. The invariant is that documentation linked as part of the standalone artifact remains accessible in that artifact.

**Reproduction:** Inspect the include whitelist and README link, then extract a generated crate archive: README is included, but its implementation-status target is absent. Consumers following that link cannot obtain the referenced checkpoint.

**Impact:** Bounded documentation/navigation failure; scaffold status remains stated directly in README, so this does not undermine the publication guard.

**Correction:** Move the public checkpoint into `docs/` and update the link, explicitly include this checkpoint, or replace the artifact link with an appropriate repository link.

**Closure test:** Inspect an extracted Cargo archive and check that every local README documentation link resolves within it. Non-architectural finding.

## 2. Invariant analysis

Dependency isolation holds: no production, development, optional, target or build dependencies exist; Cargo metadata confirms standalone membership. Root Cargo explicitly excludes the member. GWZ registration identifies the exact member commit, with `local_only: true` and no remotes.

The worker reads no arguments, emits fixed stderr, produces no protocol output and exits 2. Its test checks the exit code and exact stdout/stderr, including a synthetic sensitive argument. Process execution is isolated from the library tier.

Publication fails closed before check execution or authentication: Cargo has `publish=false`; both Gearu check lists invoke the guard; the publication workflow checks release tag against Cargo version and invokes that guard before Trusted Publishing. Permissions, trigger and authentication match the documented scaffold shape.

There are no caller APIs, conditional declarations, native implementations, secret codecs or success-shaped authentication placeholders. Owned boundaries and later review stops remain explicit. Scaffold compilation is not presented as Windows authentication qualification.

## 3. Risks and next action

Actual CI/platform execution, registry setup, worker composition and authentication qualification remain deferred. GO accepts this scaffold only. Address P3-1 during the next documentation edit, then proceed to the planned caller-value/secret-codec implementation boundary and its required review.
