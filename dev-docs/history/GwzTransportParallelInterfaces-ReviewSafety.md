# GWZ Transport Parallel Interfaces — SAFETY-AXIS REVIEW

**Review object:** Draft `gwz-core/dev-docs/GwzRemoteTransportSetupFailureAmendment.md` at `f3640463bd1c69d29322de9d92902eff54af7b8e` and draft `gwz-py/dev-docs/GwzPyTransportDesign.md` at `4a884174a0a1a3a9f8d9aa756017d1b8c6034813`.

**Baseline:** root `4ad1aa3d00c7f3ac5b725cc271dc94d28b96a326`; core `f3640463bd1c69d29322de9d92902eff54af7b8e`; Python `4a884174a0a1a3a9f8d9aa756017d1b8c6034813`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`. Verified unchanged at start and end. Sources were read from committed objects with `git show`; dirty timeout work was excluded.

**Date:** 2026-09-23

**Axis:** Safety—mixed-version behavior, replay boundaries, ownership, cancellation, shutdown and credential containment. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

Read both draft documents in full; the accepted retry-plan classifier and Closed-state clauses; transport taut `Failure`, generated CBOR decoder and ingress behavior; current core `TransportRuntime`/`TransportRequest`; and committed Python bridge, client, native shim and operation ownership APIs. No tests, builds, writes, dirty source or peer reports were used.

## 1. Findings

### [P2-1] `SetupFailureCause` has no frozen wire discriminants

**Location:** Setup Failure Amendment lines 21–25 and 64–72.

The draft freezes `Failure.setup_cause` as key 4 and names seven enum members, but does not assign their integer wire values. Taut enums require explicit numeric assignments, and mixed-version safety depends on those values remaining immutable.

A writer assigning `allocation = 4` and a later/reordered reader assigning `connection_refused = 4` would reinterpret a non-retriable allocation failure as retriable. Unknown-value rejection does not protect against a known integer with changed meaning.

Specify immutable values for every member, such as 1 through 7 in the listed order, with reserved-value policy. Closure requires golden CBOR tests for all seven causes, missing key 4, an unknown discriminant, and old-reader re-encoding that drops key 4 while preserving code/effect/facts.

### [P2-2] Python `close()` lacks a terminal, race-safe lifecycle contract

**Location:** Python Design §2 lines 46–71; §3 lines 100–127; §4 lines 133–143.

The design makes host construction lazy and makes `close()` idempotent, but never states that closing is a monotonic terminal state or defines its race with construction/admission. Consequently, close may observe no installed host while another thread is constructing one, return, and allow that thread to publish a live host afterward. Likewise, a later network call may interpret “no host” as permission to construct a second runtime after close. Either sequence permits work and changed environment credentials after shutdown was reported complete.

Define a serialized native lifecycle covering uninitialized, constructing, ready/active, closing and closed. Closing must prevent publication of an in-flight construction, refuse all later network admission with a typed closed error, and return the same retained cleanup facts on repeated close. Specify whether local-only calls remain usable, but they must never reconstruct transport.

Closure tests need barriers for close-during-first-construction and close-versus-admission, plus close-before-first-network, network-after-close, repeated-close cleanup identity, and proof that no second runtime/helper/credential access occurs.

## 2. Invariant analysis

The typed cause otherwise fails closed: absent causes cannot retry Timeout or Unavailable; interaction/allocation remain non-retriable; body failures are protected by `before_reusable`; code/effect/fingerprint remain separate. Old readers ignore key 4 and new readers accept absence.

The Python design correctly chooses one core-owned host, rejects overlapping admitted operations in Rust, keeps local-only work independent of network configuration, preserves request-specific cancellation, and avoids Python-visible credentials or raw transport objects. Those protections do not close the two interface gaps above.

## 3. Risks and next action

Implementation, platform qualification, activation and publication remain deferred. Amend the two documents with fixed enum values and the terminal native-session lifecycle, then run a focused documentation re-verdict before either implementation lane begins.
