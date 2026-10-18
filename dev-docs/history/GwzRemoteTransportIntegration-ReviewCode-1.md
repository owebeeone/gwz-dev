# Shared-schema transport integration — Code-AXIS REVIEW

**Review object:** Corrected taut external Rust type generation at `733e8a78897a90f017f4726e4331aed95e8cb977`, the test-only core consumer and draft `gwz-core/dev-docs/GwzRemoteTransportPool-InterfaceGate.md` at `435e936b593476f24fad4cc4e70f5d06b784ed7d`, and workspace integration checkpoint `23617273932031a346c6fd772e1df99fd68e2706`; implementation checkpoint, no interface freeze, dated 2026-09-19.  
**Baseline:** workspace root `23617273932031a346c6fd772e1df99fd68e2706`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`; gwz-core `435e936b593476f24fad4cc4e70f5d06b784ed7d`; unchanged gwz-transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`. Committed sources were read using pinned `git show` and `git diff` operations.  
**Date:** 2026-09-19  
**Axis:** Code — architecture, interfaces, call graphs, compatibility, error paths and scope reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. Original Code P2-1 is closed, and the corrected ranges introduce no new architectural root.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Code P2-1: external dependency types and a separately vendored Rust runtime were accepted together | Reject a nonempty Rust external-type mapping combined with runtime vendoring before writing any target output; cover Rust-only and mixed-language requests while retaining valid shared-runtime generation | `taut/src/taut/gen/scaffold.py:623–637` validates the mapping and languages, then rejects `runtime and rust_external_types`; output creation does not begin until lines 657–660. `src/tests/test_rust_external_types.py:64–77` reruns the incompatible option combination for `["rust"]` and `["rust", "python"]`, verifies the ownership diagnostic, and proves the output directory was never created. The valid owner-runtime/external-type compilation remains covered at lines 17–45. The targeted suite passed all 40 tests. | **CLOSED** |

## Changed-range analysis

The correction enforces the ownership rule at the common `emit` boundary used by the library and CLI. A nonempty external Rust mapping with `runtime=True` now raises a clear error stating that the owner crate must provide the shared runtime. The check precedes service selection and every `mkdir` or file write, so a mixed-language request cannot leave Python or Rust partial output. An empty map remains equivalent to no externally owned type and may use a vendored runtime; an external map without runtime vendoring retains the intended shared-runtime path.

The README now records the rejected combination beside the requirement to re-export the owner runtime. Existing tests still compile a composed consumer whose generated wrapper uses the owner’s nominal `Cbor`, `DecodeError` and dependency types, establishing that the correction did not disable the valid mode or change default generation.

Core updates pin the corrected taut revision and generator hashes. Regeneration still requests external Rust types without runtime vendoring, so the new rejection does not alter its checked artifact. The revised regeneration path requires the canonical checkout `src`, rejects preloaded modules, checks imported module origins and reproduces the checked output in a fresh interpreter. These provenance changes preserve the single-runtime ownership arrangement.

The three added consumer host tests exercise the documented nonzero clock origin, an independently serviced earlier deadline, and shutdown on final `Pool` owner loss through the unchanged transport API. They add test-only evidence and draft duties without changing transport source, production core/CLI interfaces, schema fields, service methods or physical delivery.

No NEW ARCHITECTURAL root was found in either corrected range.

## 0. Evidence base

The exact tuple resolved unchanged at both the start and end of review:

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `23617273932031a346c6fd772e1df99fd68e2706` | `9074eb4f23a186aa98c6a471d8fd1410e1065076` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |
| gwz-core | `435e936b593476f24fad4cc4e70f5d06b784ed7d` | `6a1a9b27860dbeffc1c0614a15d9bad5c5425a78` |
| gwz-transport | `e8b9a1c5408cc9ea9528939b3a602acbeb697814` | `6aedc6f4e2de3e228b03d4a66e792f94183ec429` |

Taut, core and transport remained clean. The workspace root contained only the two permitted untracked round-two prompts. No current-round peer report or prompt was read.

The review read:

- The complete focused Code prompt, workspace instructions, original Code report and merged remediation plan.
- Taut correction range `3b8436508ecc6b74b5efa0f1e642d0698b708bf7..733e8a78897a90f017f4726e4331aed95e8cb977`, including `src/taut/gen/scaffold.py:581–698`, `src/tests/test_rust_external_types.py:1–77` and the external-type README contract.
- Core correction range `3b4b632dfba5b0f2f154f50b45d9056a21a9ce99..435e936b593476f24fad4cc4e70f5d06b784ed7d`, including regeneration and pin changes, all regeneration tests, consumer documentation and `tests/transport_consumer/tests/pool_host.rs:1–144`.
- The corrected draft `GwzRemoteTransportPool-InterfaceGate.md`, especially shared ownership at lines 14–29, Pool lifetime at 31–48, clock duties at 68–100, evidence at 122–165 and explicit deferrals at 168–201.
- The root checkpoint/current-state records and workspace lock entries tying the corrected member revisions to the reviewed root.

Targeted verification produced:

- Combined taut and consumer tooling suite: **40 passed**.
- Explicit consumer regeneration against the pinned owner schema and canonical taut source: passed.
- Isolated offline archive proof against SHA-256 `24c9d7a839b1a23ae1f188541ac87092550dcae99bd9cf6e14df4c900b1a7dd9` and transport revision `e8b9a1c5408cc9ea9528939b3a602acbeb697814`: **nine consumer tests passed**, including the three host-contract regressions.
- Consumer `cargo fmt --check`: passed.
- Taut and core correction ranges: `git diff --check` passed.
- No large transport campaign was repeated because transport source is unchanged.

## 2. Invariant analysis

- **One Rust runtime identity:** The incompatible generator request now fails before output. The valid consumer path re-exports the owner runtime and compiles native dependency types through the generated wrapper without conversion.
- **Atomic rejection:** Rust-only and mixed-language requests create no output directory when the external-type/runtime ownership modes conflict.
- **Compatibility:** Runtime-only generation, external-type generation without vendoring, default generation and existing CLI/scaffold behavior remain covered by the passing targeted suite.
- **Exact regeneration provenance:** The command verifies the canonical taut source, exact revision, clean `src`, extension hashes, imported module origins, owner package/schema, codec and rustfmt version before comparing output. The checked artifact reproduced.
- **Package independence:** The isolated consumer builds and passes offline from the pinned archive without a sibling transport checkout or schema fetch.
- **Draft host contract:** New clock and ownership tests match the unchanged pool API and substantiate the draft’s duties without presenting them as a production dispatcher or interface freeze.
- **Scope containment:** The changed ranges add generator validation, proof hardening, tests and draft clarification. They do not alter production core/CLI service methods, the transport implementation, physical carrier behavior or protocol fields.
- **Claim accuracy:** Evidence counts reproduced, and the documents continue to identify Phase 1/2 completion, active-I/O semantics, production integration, publication, adapters and platform qualification as open work.

## 3. Risks and next action

The checkpoint still does not establish the full Phase 1 message/admission matrix, Phase 2 active-I/O behavior, production dispatch, physical carrier bounds, native-platform qualification, publication or adapter integration. Those remain explicit later gates and are unaffected by this GO.

Record Code P2-1 as closed and combine this verdict with the independently produced current-round verdict. If the dual gate accepts the checkpoint, update its acceptance record while preserving the stated Phase 1/2 deferrals and the absence of any API/schema freeze or adapter authorization.
