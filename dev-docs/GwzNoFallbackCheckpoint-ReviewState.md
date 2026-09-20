# No-fallback first implementation checkpoint — STATE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackCheckpoint.md` with baseline `GwzNoFallbackPreparation.md` at core `dd47810ece5980cfa35017ae0dfd7a8f33701e80`; corrected draft P2 first-package gate; 2026-09-20  
**Baseline:** root `d98922e03b837d030477f1d9a696fef2464b8b17`; git2-rs `f42a01267a3042b26d30e9d8acf286c6c739bd8a`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Committed sources were read with `git show`, `git diff`, and `git ls-tree`.  
**Date:** 2026-09-20  
**Axis:** State — durable-state transitions, partial effects, restart legality, fail-closed behavior, and activation reachability. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — **P0 0 · P1 0 · P2 0 · P3 0**. This State verdict is one half of the required exact-tuple Code/State entry gate.

---

## 0. Evidence base

I read the corrected checkpoint, preparation baseline, remediation plan, original P2-1 report, prior Safety evidence, and the complete correction range `d20f7e761df1f2bcf423f1285df04d9f15f61954..dd47810ece5980cfa35017ae0dfd7a8f33701e80`.

I reused prior immutable-source inspection of the retained backend/local-import/history signatures, binding patch and hashes, archive proof runner, locked native graph, registered-member lock, release source `dffaf272eb0e62ac15b74283c4e488252db9afc3`, and `libgit2-sys` native selection. A fresh comparison confirmed that core source, manifests, locks, and `tests/transport_native` remain unchanged from source baseline `0154d36d3412d26b0a8431ece67f64abd214f5d6`.

No tests, builds, writes, network operations, or Git mutations were performed. `git diff --check` passed.

The tuple was unchanged at start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `d98922e03b837d030477f1d9a696fef2464b8b17` | `017d75c514463dd1b4a7abcac01008c094679c5d` |
| core | `dd47810ece5980cfa35017ae0dfd7a8f33701e80` | `3563b18a3196c55572f3dd77fe27ea2c4d070cf4` |
| git2-rs | `f42a01267a3042b26d30e9d8acf286c6c739bd8a` | `7ca5a51585eecbb1169c970abaefdb67470b0468` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

Member repositories were clean. Root contained only the two authorized Code/State prompts.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Consistency P2-1 | Require exact-tuple Code/State GO before wiring or lane execution | Checkpoint line 116 now names dual Code/State; lines 231–233 require both GO verdicts on this exact shared-boundary object before wiring, fixtures, or code and state that Consistency/Safety do not substitute | **Closed** |

## Changed-range analysis

The correction changes only the P2 review tier and closure condition. It adds no source, API, ownership, budget, qualification, or activation change.

The original bypass is gone: accepting the earlier Consistency/Safety reports cannot transition the program into wiring or lane execution. The next legal state requires both filed Code and State GO verdicts on core `dd47810e` and the corresponding exact tuple.

No new architectural root cause was introduced.

## 2. Invariant analysis

**Entry-state grammar:** The legal progression is now closed: corrected draft → exact-tuple Code/State GO → test-only wiring and bounded first packages → individual package review → separately designed and budgeted replacement packages. No text permits skipping from document acceptance to implementation.

**Member source alignment:** The registered fork HEAD remains preserved. L2-A creates a separate local branch from the qualified release commit, changes only the declared root manifest edge, and leaves GWZ production resolution unchanged. Interruption after branch creation, manifest alignment, or either binding-file edit produces visible member state that the source-admission checks reject unless it exactly matches the release tree plus declared changes.

**Fail-closed qualification:** Source mode works on an isolated copy, checks expected hashes and native selection, retains locked-graph checks, rejects unrelated drift, and must prove its rejection path. Partial patching or unexpected files therefore lose progress or fail qualification; they cannot become a qualified candidate.

**Shared wiring:** Wiring is test-only, runtime-neutral, integrator-owned, and checked with the affected lane. A missing child module or incomplete test package fails compilation visibly and cannot change production state. Privacy failure stops for a bounded handoff instead of widening an interface or duplicating state ownership.

**Repository mutation characterization:** L1-A must observe object/ref/FETCH_HEAD state on success and failure and characterize multi-refspec partial publication before any route decision. L3-A must record HEAD/index/ref and hook/signing aftermath under isolated configuration and child environments. These packages describe existing failure states; they cannot install replacements.

**Read-only history state:** L4-A cannot write repositories, add dependencies, alter routing/schema, or introduce lazy fetch. It only freezes observed ordering and traversal behavior for a later design gate.

**Activation reachability:** The member candidate remains unpublished and absent from production manifests. All-consumer, native-link, platform, object-format, packaging, and separate activation reviews remain mandatory before any production dependency transition.

## 3. Risks and next action

Execution evidence remains intentionally absent. Package reports must still record actual native linkage, executed and pending platform/object-format rows, partial-state observations, and budget actuals. Pending evidence cannot advance a later activation state.

Record this State GO together with Code GO on the same tuple. Only then may the integrator add the listed test-only wiring and begin L1-A, L2-A, L3-A, and L4-A; no replacement or production activation is authorized.
