# GWZ SSPI scaffold — STATE-AXIS REVIEW

**Review object:** Root `ec0dd6d..e6772cb` and initial committed `gwz-sspi` scaffold; controlling `dev-docs/GwzSspiScaffold.md` is DRAFT at `e6772cbeda821deb7f2f42d63f73d9a3581e60ec`.

**Baseline:** Root `e6772cbeda821deb7f2f42d63f73d9a3581e60ec`; gwz-sspi `bd807b0403d502fd6f25ef2f93389ce432f0c0c0`; gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`. Committed sources read with `git show`; tuple verified unchanged at start and end.

**Date:** 2026-10-03

**Axis:** Durable-state semantics, restart legality, filesystem ordering and fail-closed release behavior. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. Acceptance is confined to the repository/library/release scaffold.

---

## 0. Evidence base

Read:
- `AGENTS_GWZ.md`, canonical State prompt and review-loop skill; process rulebook opening/current execution rules, optimization amendment and checkpoint SSPI section.
- Complete controlling scaffold document and root diff, including Cargo exclusion, member/lock registration and integrity markers.
- SSPI design §§1–2,7; complete implementation plan and acceptance record; crate-map and library-boundary placement, dependency and test-tier clauses.
- Committed member `Cargo.toml`, `Cargo.lock`, toolchain, Gearu configuration, AGENTS, README, RELEASE, Architecture, Testing and Implementation documents; every ownership README.
- Complete library, worker, worker-bootstrap test, release-check script and both workflows.

Ran permitted `cargo metadata --manifest-path gwz-sspi/Cargo.toml --no-deps --offline`: one standalone member, no dependencies, empty defaults, worker and bootstrap test gated by `worker-bin`, registry publication disabled. Inspected package inclusion patterns against the source/test/document layout; no archive was rebuilt.

Read local Git configuration: no remote configured. Independently hashed committed registration files; both match the committed integrity marker. No builds, tests, writes, Git mutations, agents or current peer reports were used.

## 2. Invariant analysis

**Refusal cannot invent authentication progress.** The library exports no success-shaped placeholder. Worker execution has one fixed diagnostic followed by exit 2; it never reads or echoes supplied arguments and emits no stdout. The bootstrap test asserts exact exit/stdout/stderr, including a synthetic sensitive argument. Killing this process introduces no durable authentication state because none is created.

**Release failure stays closed.** `publish=false` provides Cargo’s independent publication refusal. The release helper checks requested version and publication permission before executing checks. Both Gearu check lists invoke that helper. The release workflow validates tag against package version and runs the guard before obtaining registry credentials. Ordinary pushes and PRs cannot initiate publication; checkout credentials are disabled, and no stored-token fallback exists.

**Repository state is explicit and consistent.** Root Cargo excludes the independently owned workspace. Registration names the exact committed member, marks it local-only, supplies no remotes and refreshes both integrity hashes together. The intended GitHub URLs are identified as future targets; absence of remote/registry setup is not represented as completion.

**Recovery and test claims remain bounded.** Gearu guidance names preparation, atomic push, GitHub Release and retry paths, retaining immutable tags. Fast library testing is separate from process bootstrap testing. Public CI requires no private evidence. Ownership directories establish future boundaries without adding executable recovery states, locks or filesystem writes.

## 3. Risks and next action

Compilation and archive verification were not rerun under this read-only mandate; local successful checks are reported evidence, and authored CI remains unexecuted remotely. No native SSPI, secret codec, supervisor crash recovery, installed worker or Windows authentication qualification is accepted.

Next action: accept this scaffold checkpoint, then implement owned caller values/private codecs and fake tests, retaining the mandatory secret-boundary review before supervision work.
