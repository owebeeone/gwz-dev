# Python concurrent transport session v2 — Safety re-review 3

**Review object:** `dev-docs/GwzPyTransportSessionV2Design.md` and `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`  
**Baseline:** root `5b39c6f360506844695cbd658a20a57f8bda430a`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `53075fbf56856e51cc1aac3f146ab7f7c84cdfc5`  
**Date:** 2026-09-24  
**Axis:** Safety. Focused, independent, read-only re-verdict.

**Verdict: NO-GO.** The 256-byte request-ID limit closes the unbounded-string counterexample, but Safety P2-1 remains open for required descriptor and close-summary charges under a full ledger. This is incomplete closure of the same quota-accounting root, **not a new architectural root cause**.

## Prior-finding closure table

| Prior finding | Re-trace | Status |
| --- | --- | --- |
| Safety-2 P2-1 — unbounded retained caller IDs | A native `start_*` factory now rejects an ID over 256 UTF-8 bytes before issuing a record or constructing an endpoint. Sixty-four distinct retained IDs therefore contribute at most 16 KiB of string bytes before copies. Root design lines 13 and 47; caller guide lines 53 and 63. | **Partially closed.** The unbounded-ID sequence is closed; required copies still lack guaranteed reserved capacity. |
| Safety-1 P2-1 — first stream reader unavailable after acceptance | The primary reader remains inside the admission reservation and is claimed before the stream worker gate opens. | **Closed in design text.** |
| Safety-1 P2-2 — abandoned push outcome erased | Dropped streams remain discoverable through the Client-owned ledger. The proof now separates a retained stream’s `aclose()` result from discovery after every stream reference is dropped. | **Closed in design text.** |

## Changed-range analysis

Relative to root `59c13d6184c82789d9dc558ca436b3a43be8bdcd` and gwz-py `ffa184bbb50bf56ff49ebee65a79c228e0bf6489`, the correction limits request IDs to 256 UTF-8 bytes, charges identity and descriptor copies to the 64 MiB ledger, narrows native-host claims to `NativeCoreBridge`, preserves custom-bridge close behavior, separates the two stream-abandonment proof cases, and documents omitted push options. Core is unchanged.

The new accounting sentence exposes an admission-order gap: descriptor and close-summary charges are now mandatory, while the specified 8 MiB per-operation reservation does not include or separately reserve them.

## 0. Evidence base

I read `GwzPyTransportSessionV2-RemPlan-2.md`, my filed Safety-2 report, both revised review objects and their old-to-new diffs. I re-traced the prior request-ID sequence through record creation, `recent_operations()`, close and expiry. No file was changed; no build, test, source check or wire proof ran. The tuple matched at the start and final recheck.

## 1. Findings

### [P2-1] Required recovery summaries have no reserved charge at full admission

**Location and invariant:** Root design lines 45 and 47; caller guide lines 69–71. `recent_operations()` must remain able to reveal an abandoned operation’s ID and effect, and native close must retain summaries for every operation live when close began. Line 47 newly charges record identity, `recent_operations()` descriptors and close summaries to the 64 MiB ledger. Its pre-acceptance 8 MiB reservation lists events, result, failure slot and primary reader, but assigns no reserved space to those required descriptors or summaries.

**Credible sequence:** Admit eight operations at the documented 8 MiB reservation each, filling the 64 MiB ledger. Drop a push stream after the remote may have accepted it, then call `recent_operations()` to recover its ID. Producing a charged descriptor needs ledger capacity that is not reserved. Close while the operations are live; its eight charged summaries face the same gap, including if their results consume their retained allowances. The text neither guarantees these reads from a prior reservation nor specifies an attributed fallback if the charge cannot fit. Charging record identity separately also makes eight full 8 MiB reservations arithmetically exceed 64 MiB unless that identity charge is folded into each reservation.

**Impact:** Aggregate pressure can make the very discovery and close-report paths used to recover possible-effect pushes unavailable after effects. It also leaves the stated eight-operation full-reservation proof unsatisfiable as written.

**Required correction:** Assign a bounded maximum for record identity, one required discovery descriptor and a possible close summary to capacity reserved before acceptance, or give required recovery metadata a separately bounded reserve. State precisely how charges transfer at completion and expiry. Optional caller-requested copies may refuse; the required discovery and close paths must remain available after acceptance.

**Closure test:** Use maximum-length IDs and fill admission to the stated eight-operation, 64 MiB boundary. Drop one possibly effective push stream, recover its ID/status/effect with `recent_operations()`, then close while all eight are live and obtain all eight summaries. Repeat with retained results near their per-operation limits. Assert every required charge stays within the declared budget and no post-effect capacity refusal hides an outcome.

This remains **Safety-2 P2-1**, because the correction bounds ID strings but has not completed the requested accounting for their recovery and close-report copies.

## 2. Invariant analysis

The changed custom-bridge language preserves `Client.close() -> TransportCleanup | None` and refuses native-only handle and ledger APIs before effects when hooks are absent. It does not add a credential or cross-Client record path. The separated stream proof cases now distinguish an `aclose()` result held by a caller from by-ID recovery after the object is dropped. Primary-reader reservation, generation-pinned cancellation, capacity-installation serialization and possible-effect terminal classification are unchanged by this correction.

## 3. Risks and next action

Keep the design gate **NO-GO**. Complete the recovery-metadata reservation rule and re-trace this one full-ledger sequence on a pinned tuple. The fix can remain bounded to quota accounting; this review found no new architectural root cause. Implementation and platform evidence remain separate gates.

The tuple matched at the final check: root `5b39c6f360506844695cbd658a20a57f8bda430a`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `53075fbf56856e51cc1aac3f146ab7f7c84cdfc5`.
