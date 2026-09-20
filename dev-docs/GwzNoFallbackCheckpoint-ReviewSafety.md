# No-fallback first implementation checkpoint — SAFETY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackCheckpoint.md` with baseline `GwzNoFallbackPreparation.md` at core `d20f7e761df1f2bcf423f1285df04d9f15f61954`; draft P2 first-package gate; 2026-09-20  
**Baseline:** root `d562607e27fa3d72a93a83a2b2ed8733d31caee5`; git2-rs `f42a01267a3042b26d30e9d8acf286c6c739bd8a`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Sources were read from immutable commits with `git show`, `git log`, and `git ls-tree`.  
**Date:** 2026-09-20  
**Axis:** Safety — whether the first-package checkpoint permits unsafe implementation, partial-state concealment, or premature activation. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — **P0 0 · P1 0 · P2 0 · P3 0**. This authorizes only the bounded first packages and their test-only wiring.

---

## 0. Evidence base

I read the complete checkpoint and preparation baseline; the accepted plan’s P2, lane-2, integration, and completion clauses; the native-binding and adapter-foundation qualification boundaries; and the current root checkpoint.

Targeted source inspection verified:

- The retained backend, local-import, history, and binding signatures.
- The accepted two-file binding patch and its original/patched hashes.
- The current archive proof runner, lock-graph guard, runner tests, and native fixture manifest.
- Release-source commit `dffaf272eb0e62ac15b74283c4e488252db9afc3`, including its root manifest and matching binding-file hashes.
- `libgit2-sys` `0.18.8+1.9.7` native-selection behavior.
- GWZ’s registered git2-rs member identity and lock entry.

No tests, builds, writes, network operations, or Git mutations were performed.

The exact tuple was unchanged at start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `d562607e27fa3d72a93a83a2b2ed8733d31caee5` | `fa7100eaad28950b5338e2d4dcd18bf76f1fb583` |
| core | `d20f7e761df1f2bcf423f1285df04d9f15f61954` | `aebd5289962f467fa92a22057de9d81738459da0` |
| git2-rs | `f42a01267a3042b26d30e9d8acf286c6c739bd8a` | `7ca5a51585eecbb1169c970abaefdb67470b0468` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

Member repositories were clean. Root contained only the two authorized untracked review prompts. All permitted `git diff --check` invocations passed.

## 2. Invariant analysis

**First-package containment:** Checkpoint lines 9–31 restrict lanes 1, 3, and 4 to characterization/design and lane 2 to the previously accepted binding port. Later replacement packages require separate designs, grounded budgets, and reviews. No first package may change a protocol or add a runtime owner.

**Ownership and scope:** Lines 78–129 enumerate every writable path, make unlisted paths read-only, reserve manifests, locks, pins, shared contracts, and shared wiring to the integrator, and require a handoff rather than copied helpers or widened visibility. Numeric ceilings and the standing owner/protocol/file-crossing stop rules bound each package.

**Member-source isolation:** L2-A starts from release commit `dffaf272`, whose two binding files match the accepted archive hashes. Only the declared two-file patch and root manifest alignment are admitted. Source mode must work on an isolated copy, preserve archive mode, reject unrelated checkout drift, leave Cargo caches untouched, and prove both positive admission and rejection.

**Native identity:** The member manifest is aligned to registry `libgit2-sys =0.18.8`, while the locked proof graph identifies `0.18.8+1.9.7`. The preparation baseline requires recording whether the actual native library is system-selected or bundled; the checkpoint requires checking native dependency selection and reporting observed and pending platform/object-format rows. This first package cannot convert host-local evidence into release or activation evidence.

**Test wiring:** The three shared source paths permit test-module declarations only, with zero runtime change. Privacy failure causes a bounded integrator handoff or documentation-only result; it cannot justify a parallel harness or public API widening.

**Failure and partial-state evidence:** L1-A must distinguish explicit wants from negotiation hints, observe objects and refs on success and failure, and characterize multi-refspec partial publication before choosing a route. L3-A records HEAD, index, ref, hook, signing, and interruption aftermath using isolated repository configuration and child environments, without personal keys or process-wide environment mutation. Existing subprocesses remain until later replacements pass their full gates.

**Read-only history work:** L4-A may characterize existing ordering, range, pathspec, merge-simplification, and routing behavior, but cannot add a dependency, change schema or CLI surface, write repositories, or claim a replacement.

**Activation separation:** The registered fork and source candidate do not affect production resolution. Publication, packaging, all-consumer/platform qualification, production dependency switching, and SSH/message/pool routing remain separate reviewed packages.

## 3. Risks and next action

The actual native linkage choice and unsupported platform/object-format rows remain execution evidence, not assumptions. Package closure must record them precisely; pending rows cannot support later activation.

Accept this P2 checkpoint. Proceed only with the listed test wiring and L1-A/L2-A/L3-A/L4-A packages, then review each exact result before designing or budgeting any replacement package.
