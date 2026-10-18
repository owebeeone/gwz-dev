# GwzTransportParallelInterfaces — CONSISTENCY-AXIS REVIEW 1

**Review object:** corrected `gwz-core/dev-docs/GwzRemoteTransportSetupFailureAmendment.md` at `479926c18265276e5a45659c4523a13a71f4a51f` and `gwz-py/dev-docs/GwzPyTransportDesign.md` at `259f73cc030c0da0bf29903bab258de0463b7d02`; draft interface package, 2026-09-23  
**Baseline:** root `00827c75afb93f5855ee77019df7cb459c12754a`; core `479926c18265276e5a45659c4523a13a71f4a51f`; Python `259f73cc030c0da0bf29903bab258de0463b7d02`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`. Committed documents were read with `git show` and compared with the prior pins using `git diff`.  
**Date:** 2026-09-23  
**Axis:** Focused consistency re-verdict on the two original findings and changed-range interactions. Independent, adversarial, read-only. The other axes run independently; nothing here relies on their current reports. Filed verbatim by the lane owner.

**Verdict: GO** — P0: 0, P1: 0, P2: 0, P3: 0.

---

## 0. Evidence base

The four required HEADs matched at both review boundaries. Reviewed the merged remediation plan and only the corrected ranges in the two draft documents. Core changed only the setup-failure amendment; Python changed only its transport design. No source, builds, tests, dirty timeout work, or current peer reports were inspected.

## Prior-finding closure

| ID | Original root classification | Verification | Status |
|---|---|---|---|
| Consistency P2-1 | Documentation/interface root: compatibility-significant enum discriminants were unfrozen | `SetupFailureCause` now fixes `stall` through `address_not_available` to values 1–7, reserves and rejects zero, rejects unknown values, forbids reassignment, and requires golden CBOR coverage for every mapping plus old-reader preservation of code, effect, and facts. The original two-valid-but-incompatible-schema counterexample is no longer permitted. | **CLOSED** |
| Consistency P2-2 | Architectural lifecycle root: close was unordered against lazy construction/admission | The Python design now freezes a Rust-owned monotonic lifecycle from Uninitialized through Closed. Close atomically enters Closing, blocks new construction/admission, joins either transition, prevents losing constructor results from publication, shuts each constructed runtime exactly once, retains cleanup facts, and permanently refuses post-close calls. Barrier tests cover both original races. | **CLOSED** |

## 1. Findings

No open findings.

## 2. Changed-range and invariant analysis

The enum correction preserves the accepted retry set, failure codes, effects, fingerprint meaning, profile v2, and old/new decoder behavior. Missing causes still fail closed for ambiguous `Timeout` and `Unavailable`.

The lifecycle correction agrees across the state description, native-flow diagram, frozen core APIs, Python APIs, and verification section. Close and cancellation return explicit cleanup values; repeat close returns the retained snapshot; operation-ID cancellation maps privately to the request-specific Rust handle; wrong or expired identities cannot affect an active request; retained cancellation state is explicitly bounded.

The added API clauses do not create a second lifecycle owner, Python retry loop, Python pool, deadline policy, physical carrier, activation claim, or release-order change. Local-only operations remain isolated from endpoint environment construction.

**New-root classification:** no new architectural or documentation root cause was found. The changed text is a bounded completion of the two original interfaces.

## 3. Risks and next action

Implementation and execution evidence remain outstanding by design. The interface package may proceed to implementation under its stated tests and later settled-tree review; this GO does not accept product code, platform qualification, activation, or publication.
