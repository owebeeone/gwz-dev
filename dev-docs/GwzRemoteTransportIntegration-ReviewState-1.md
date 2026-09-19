# Shared-schema transport integration — State-AXIS REVIEW

**Review object:** Corrected shared-schema integration checkpoint at workspace `23617273932031a346c6fd772e1df99fd68e2706`, comprising taut `7a5f616c3a9f72e143b6e20dab41ffa6e20e240a..733e8a78897a90f017f4726e4331aed95e8cb977`, gwz-core `e21250ce0d6e9b5fce02154595db5ba2e451ae4c..435e936b593476f24fad4cc4e70f5d06b784ed7d`, the workspace integration checkpoint, and the draft `gwz-core/dev-docs/GwzRemoteTransportPool-InterfaceGate.md`; gwz-transport remains unchanged at accepted revision `e8b9a1c5408cc9ea9528939b3a602acbeb697814`. This is an implementation checkpoint, not a Phase 1/2 interface freeze.  
**Baseline:** workspace `23617273932031a346c6fd772e1df99fd68e2706`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`; gwz-core `435e936b593476f24fad4cc4e70f5d06b784ed7d`; gwz-transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`. Committed sources were read using immutable `git show PIN:path` and pinned diffs.  
**Date:** 2026-09-19  
**Axis:** State — state machines, ownership, failure and recovery, filesystem/provenance ordering, bounds and fail-closed behavior. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — all three prior P2 findings are closed; zero new P0, P1, P2 or P3 findings.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 | Require the canonical Git checkout `src`, bind imported modules to it, reject cached modules and use a fresh interpreter for positive regeneration. | The original `taut/docs` counterexample now refuses with the resolved expected and supplied paths. `_generate` rejects every preloaded `taut` module, diagnoses foreign origins, and verifies the imported `taut`, scaffold and external-type modules beneath the canonical source. Tests cover noncanonical input, same-version foreign cache, canonical cache and fresh-process positive regeneration; the pinned artifact reproduces. | Closed |
| P2-2 | Require clock initialization before checkout, one monotonic origin, independent ticking or mutation-driven re-query with bounded fallback, and define `next_deadline` as a snapshot while `next_action` supplies no timer. | The draft now states each duty explicitly. The consumer fixture initializes at `4_000_000`, obtains deadline `4_010_000`, remains pending at `4_009_999`, and times out exactly at `4_010_000`. A second fixture leaves `next_action` pending, introduces an independently held short waiter, observes the deadline move from `40_000` to `10_025`, and expires it through an independent tick. | Closed |
| P2-3 | Require retention of a Pool clone for endpoint lifetime and define final Pool drop as shutdown even while driver, checkout or lease state remains. | The draft distinguishes Pool ownership from driver-loss duties. The consumer fixture proves that dropping a nonfinal Pool preserves the live lease, dropping the retained final owner makes the lease stale and emits `Close`, and the retained driver drains the close acknowledgement to completion. The accepted transport regression is cited. | Closed |

## Changed-range analysis

The correction binds regeneration verification to execution. `tests/transport_consumer/protocol/regen.py:82-164` resolves the Git top level, requires its exact canonical `src`, checks the pinned revision, clean source and extension hashes, rejects preloaded modules, and validates imported module origins. The positive test now launches the regeneration script in a fresh interpreter. The old path-substitution and `sys.modules` routes therefore fail before schema loading or output generation.

The adjacent taut correction rejects a nonempty Rust external-type map combined with runtime vendoring at `src/taut/gen/scaffold.py:623-637`, before output directories are created. Its Rust-only and mixed-language regressions assert that no partial output exists. This is compatible with the consumer’s valid path: it omits runtime vendoring and explicitly shares `gwz_transport::cbor`. The generated consumer artifact did not change.

The draft contract adds the missing lifecycle and clock obligations at `GwzRemoteTransportPool-InterfaceGate.md:43-48,76-92`, and its evidence table names both the accepted transport regression and the three consumer host fixtures at `:136-137`. The fixtures exercise the unchanged public pool API from the package consumer, including the two adverse sequences from the original report. They introduce no production dispatcher, timer, socket, carrier or protocol field.

The correction changes no transport source and does not broaden the checkpoint into the deferred Phase 1 schema or Phase 2 runtime/pool freezes.

**NEW ARCHITECTURAL ROOT CAUSES: none.**

## 0. Evidence base

The exact tuple was verified at the beginning and end of the review. Both checks returned:

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `23617273932031a346c6fd772e1df99fd68e2706` | `9074eb4f23a186aa98c6a471d8fd1410e1065076` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |
| gwz-core | `435e936b593476f24fad4cc4e70f5d06b784ed7d` | `6a1a9b27860dbeffc1c0614a15d9bad5c5425a78` |
| gwz-transport | `e8b9a1c5408cc9ea9528939b3a602acbeb697814` | `6aedc6f4e2de3e228b03d4a66e792f94183ec429` |

All member repositories remained clean. Root contained only the two authorized untracked round-2 prompts. The current Code prompt and report were not read.

The review read:

- The focused State prompt, merged remediation plan and prior State report.
- The complete taut correction diff, especially `README.md`, `src/taut/gen/scaffold.py` and `src/tests/test_rust_external_types.py`.
- The complete core correction diff, especially consumer regeneration code/tests, generator pins, README, draft interface gate and `tests/transport_consumer/tests/pool_host.rs`.
- The workspace current checkpoint and integration checkpoint.
- Applicable plan, design and requirements sections governing schema ownership, pooling, clocks, shutdown and deferred carrier work.
- Unchanged transport implementation only at the affected seams: `PoolMachine` clock initialization/deadlines, async Pool owner counting/final-drop shutdown, transport README contract and the accepted final-owner regression.

Targeted verification produced:

- The combined taut and consumer-tooling command passed all 40 tests.
- Explicit consumer regeneration reported `consumer regen verified`.
- Consumer formatting passed.
- The isolated offline archive proof verified transport source revision `e8b9a1c5408cc9ea9528939b3a602acbeb697814`, package `gwz-transport` version `0.1.0`, and archive SHA-256 `24c9d7a839b1a23ae1f188541ac87092550dcae99bd9cf6e14df4c900b1a7dd9`; all nine consumer tests passed, including the three new pool-host regressions.
- Both correction diffs passed `git diff --check`.
- The original explicit `_verify_taut_source(taut/docs, pin)` counterexample now refused: `expected .../taut/src, got .../taut/docs`.
- The unchanged transport’s 66-test suite was accepted lane-owner evidence and was not rerun in this focused review. No large campaign or implementation acceptance is claimed.

## 2. Invariant analysis

The provenance attack now fails closed. A path elsewhere in the verified repository cannot stand in for the package source, and a cached same-version installation cannot bypass `sys.path`: foreign cached modules fail their origin check, canonical cached modules still require a fresh interpreter, and imported generator modules are checked after loading. The positive subprocess reproduces the checked artifact from the pinned source.

The single-clock-origin invariant now appears in both contract and executable consumer evidence. No request is created at internal time zero before switching to a large external epoch. The exact-deadline assertions also establish elapsed-budget behavior rather than merely checking a reported timestamp.

The snapshot/timer attack now fails. The contract says that `next_deadline` is not a subscription and a pending `next_action` supplies no timer. The short-waiter fixture demonstrates that an earlier deadline can arise while the action future remains pending and that the host must cancel or suspend that wait to service its independent tick.

The endpoint ownership grammar is now complete. Only Pool clones count as endpoint owners; driver, checkout and lease references do not. A retained Pool preserves the endpoint, final owner drop stops it and invalidates the lease, and the driver remains responsible for draining the resulting cleanup action. This is distinct from driver loss.

Shared native type and codec ownership remain intact. The consumer continues to re-export owner transport types and the owner CBOR runtime. The new taut option guard prevents generation of a second runtime when external owner types are present, and the corrected regeneration path still emits the unchanged checked artifact.

Failure and write ordering remain bounded. Invalid generator option combinations refuse before output creation. Regeneration performs provenance, schema, codec and rustfmt checks before replacing the checked output, while check mode only compares. Archive verification and extraction remain temporary, isolated and offline. No corrected path invents accepted product state after failure.

The scope claims remain accurate: no production core/CLI service surface, optional protocol field, physical framing, carrier, credential flow or transport implementation changed. Active-I/O clocks, full phase-exit matrices, platform qualification, production dispatcher wiring and publication remain stated obligations.

## 3. Risks and next action

The checkpoint still does not qualify a physical carrier, native platforms, package publication, production host scheduling, complete encoded-contract matrices or active stream-I/O timeout semantics. Those are recorded deferrals and do not weaken this focused verdict. The host timing regressions are deterministic fixtures; executor integration remains future evidence.

The next action is to combine this GO with the independent Code-axis re-verdict. Accept only this shared-schema implementation checkpoint and draft contract if that axis also reports GO; do not treat acceptance as a schema/API freeze or authorization for adapter integration.
