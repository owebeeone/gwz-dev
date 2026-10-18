# No-fallback first implementation checkpoint — CONSISTENCY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackCheckpoint.md` and `GwzNoFallbackPreparation.md` at core `d20f7e761df1f2bcf423f1285df04d9f15f61954`; draft P2 first-package planning gate dated 2026-09-20  
**Baseline:** root `d562607e27fa3d72a93a83a2b2ed8733d31caee5`; core `d20f7e761df1f2bcf423f1285df04d9f15f61954`; git2-rs `f42a01267a3042b26d30e9d8acf286c6c739bd8a`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Committed sources were read with `git show`; the working trees were not used as authority.  
**Date:** 2026-09-20  
**Axis:** Consistency against the accepted plan, preparation baseline, retained interface contracts, and pinned source trees. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks the checkpoint. I pre-commit to GO on a revision that resolves P2-1 as specified, provided the tuple and affected text remain otherwise unchanged.

---

## 0. Evidence base

I verified the complete tuple and tree objects at the start and end. Final trees were root `fa7100eaad28950b5338e2d4dcd18bf76f1fb583`, core `aebd5289962f467fa92a22057de9d81738459da0`, git2-rs `7ca5a51585eecbb1169c970abaefdb67470b0468`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, and taut `bb0694d0e31d217b82024cb73c2e1212dfa5c72f`. All member repositories were clean; root contained only the two authorized untracked current prompts. `git diff --check` was clean.

I read the complete checkpoint and preparation documents; the accepted plan through its lane contracts and gates; the relevant native-binding, adapter-foundation, core design/requirements, and root checkpoint clauses; the exact retained Rust signatures; the planned module parents; the existing native proof runner, pin file, tests, and documentation; the registered fork manifests at release base `dffaf272eb0e62ac15b74283c4e488252db9afc3` and fork HEAD; and the membership lock.

The core implementation tree under `src`, `crates`, manifests, lock, and `tests/transport_native` has no delta from the recorded source baseline `0154d36d3412d26b0a8431ece67f64abd214f5d6`. No builds or tests were run.

## 1. Findings

### [P2-1] The checkpoint substitutes the wrong dual-review axes for its shared-boundary gate

**Location:** `GwzNoFallbackCheckpoint.md:113-120`, especially line 116, and closure text at lines 231-234; controlling `GwzNoFallbackPlan.md:93-108,130-135`.

The accepted plan requires dual **Code/State** review for shared or durable boundaries, with Surface added for a changed API or user-facing freeze. This P2 object freezes shared signatures, ownership, wiring, budgets, and integration order, yet its table declares “Dual Consistency/Safety,” and its closure permits lane work after that review.

Reproduction: accept the checkpoint under its stated Consistency/Safety pair, integrate shared wiring, and begin all four packages. No Code review has then checked source-level implementability and no State review has qualified the shared boundary, despite the controlling plan making those reviews part of the pre-implementation gate.

The current review pair can supplement the required gate but cannot silently replace explicitly named axes. This makes the checkpoint capable of declaring P2 complete while omitting a controlling entry condition.

**Required correction:** state that the exact P2 shared-boundary object requires and records dual Code/State review before wiring or lane execution. Retain Consistency/Safety only as additional review, or explicitly amend the accepted controlling plan through a reviewed supersession. Surface need not be repeated for unchanged previously qualified APIs; it remains required if the implementation changes a public API or runner input.

**Closure test:** mechanically compare every checkpoint review-tier row with plan lines 130-135, then verify filed Code and State verdicts identify this exact tuple and are GO before first-package edits begin.

## 2. Invariant analysis

The remaining attacks held:

- Ownership is path-exact, assigns shared manifests, locks, wiring, and contracts to the integrator, and supplies a bounded handoff when privacy prevents reuse.
- Every first package has numeric production, move, test, tool, documentation, file-count, protocol, growth, and crossing limits. The listed files fit their ceilings.
- Recorded signatures match the pinned source. Planned child test modules are feasible at the named parents; `cfg-if` is available for compliant conditional boundaries.
- The registered lock pins fork HEAD exactly. L2-A instead branches from the resolvable qualified git2 0.21.0 release base, replaces its old path sys edge with registry `=0.18.8`, preserves the accepted native 1.9.7 identity, and expressly rejects cloned-HEAD qualification.
- The existing archive runner, hashes, compile-red evidence, seven-test fixture, and 72-line patch support the proposed bounded source-mode extension.
- Characterization precedes route or policy selection in lanes 1, 3, and 4. Production activation remains separately gated across all consumers, platforms, object formats, packaging, and native linkage.

## 3. Risks and next action

Implementation evidence remains intentionally absent, and the later route/design decisions remain open by contract. Correct P2-1 and obtain the required exact-tuple Code/State GO verdicts before shared wiring or lane packages begin.
