# No-fallback first implementation checkpoint — CODE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackCheckpoint.md` with baseline `GwzNoFallbackPreparation.md` at core `dd47810ece5980cfa35017ae0dfd7a8f33701e80`; corrected draft P2 first-package gate dated 2026-09-20  
**Baseline:** root `d98922e03b837d030477f1d9a696fef2464b8b17`; core `dd47810ece5980cfa35017ae0dfd7a8f33701e80`; git2-rs `f42a01267a3042b26d30e9d8acf286c6c739bd8a`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Sources were read from immutable commits with `git show`, `git diff`, `git log`, and `git ls-tree`.  
**Date:** 2026-09-20  
**Axis:** Code architecture, interfaces, call graphs, ownership, and compatibility feasibility. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0, P1, P2, or P3 findings.

---

## 0. Evidence base

The complete tuple and tree objects were verified at start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `d98922e03b837d030477f1d9a696fef2464b8b17` | `017d75c514463dd1b4a7abcac01008c094679c5d` |
| core | `dd47810ece5980cfa35017ae0dfd7a8f33701e80` | `3563b18a3196c55572f3dd77fe27ea2c4d070cf4` |
| git2-rs | `f42a01267a3042b26d30e9d8acf286c6c739bd8a` | `7ca5a51585eecbb1169c970abaefdb67470b0468` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

Member repositories were clean; root contained only the two authorized current prompts. Working-tree `git diff --check` was clean.

I read the corrected checkpoint, preparation baseline, accepted plan, and merged remediation. I reused the prior immutable-source inspection and targeted the actual commit/tag, fetch-fallback, and path-history call sites; retained trait signatures; module parents; native proof patch, manifest, runner and seven tests; git2 release and fork manifests; and release ancestry. No tests, builds, writes, network access, or mutations were performed.

## 1. Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| Consistency P2-1 — wrong dual-review axes allowed implementation without the plan-required Code/State gate | **Closed** | Checkpoint line 116 now requires dual Code/State before wiring or execution. Lines 231-233 require both reviewers to report GO on the exact P2 object before any wiring, fixture, or code edit and state that earlier document reviews do not substitute. The remediation plan maps exactly this correction. |

## 2. Changed-range analysis

Core range `d20f7e761df1f2bcf423f1285df04d9f15f61954..dd47810ece5980cfa35017ae0dfd7a8f33701e80` changes only `GwzNoFallbackCheckpoint.md`: four inserted and two removed lines. The edit changes the review-entry requirement only. It does not alter APIs, ownership, budgets, lane scope, source pins, qualification obligations, or production code. Core source, crates, manifests, lock, and native fixture remain identical to baseline `0154d36d3412d26b0a8431ece67f64abd214f5d6`.

## 3. Invariant analysis

The Code attacks held:

- The five recorded subprocess sites exist at the named baseline paths: ordinary commit; tag create and delete; anonymous-fetch fallback; and path-filtered `rev-list`. Their current call shapes match the proposed characterization packages.
- The recorded `GitBackend`, `LocalTransport`, and `RemoteCallbacks::smart_transport` signatures match the retained contracts. No first package changes their visibility, result types, error types, CLI surface, or protocol.
- The named module parents and test targets exist. The proposed test-only wiring is feasible within 24 lines; core already depends on `cfg-if`, permitting compliant conditional boundaries without manifest changes.
- Qualified release commit `dffaf272eb0e62ac15b74283c4e488252db9afc3` is resolvable in the registered fork’s ancestry. Its two binding source files match the cached qualified git2 0.21.0 archive. The accepted patch changes only those files and adds 72 lines, within the 150-line ceiling.
- The release manifest’s path dependency is `libgit2-sys` 0.18.4, while cloned HEAD uses path version 0.18.7. The checkpoint’s sole alignment edit—remove the path and require registry `=0.18.8`—therefore preserves the qualified 1.9.7 native identity and avoids treating cloned HEAD as qualified.
- Exact ownership separates the integrator’s branch/manifest and shared-wiring work from lane-owned files. Privacy failure triggers a bounded handoff rather than interface widening or duplicated harnesses.
- The source-mode runner must retain archive mode, validate exact source drift, work in an isolated copy, preserve the locked graph, and exercise all seven existing native tests.
- First packages remain characterization or an unpublished binding port. Production dependency activation still requires separate all-consumer, platform, object-format, packaging, and native-link qualification.

## 4. Risks and next action

This gate establishes implementability, not implementation acceptance. Runtime evidence, actual budget use, and deferred route decisions remain for their named packages.

Record the independent State verdict on this exact tuple. If it is also GO, the integrator may add the bounded test-only wiring and begin the first packages under the checkpoint’s ownership and stop rules.
