# Windows HTTPS WH1 — remediation round 3

2026-10-04.
- Limited WH1 remains **NO-GO**; see the [round-2 verdict](GwzWindowsHttpsIntegrationImplementation-Verdict-2.md).
- This is the third and final round. It is confined to non-architectural corrections, as GwzProcessOptimization §4.1 and the review-loop cap permit. An architectural root cause found here stops the lane for redesign.
- Apply one patch in lane `wh1-rem2`.

| Finding | Disposition | Closure evidence |
|---|---|---|
| State-2 P2-4: a stale admitted action for a retired HTTPS stream closes an HTTPS-only endpoint Session | **Correct.** See the disposition notes below. | **State-2's two cases**, run on a portable HTTPS-only Session (`ssh: None`) through the real pump and mux. Each must close the session on `f76cf4cc` and keep it open after the fix. See the case list below. **Controls:** the Unix SSH-present control stays green, and so do the round-2 Session tests and the round-1 regressions. |
| Code-2 P3-1: atomicity of `send_if` untested (non-blocking, folded into this patch) | **Add the deterministic transport test Code-2 specifies.** See the disposition notes below. | The test passes on the committed body. A recorded mutation run, with a split body, fails it. |

**State-2 P2-4, the disposition:**
- On an endpoint Session without an SSH engine, route a non-Open action to the HTTPS endpoint. Its `accept` already treats an unknown or retired key as no-work, as `PlacementEndpoint::accept` does for SSH (`placement_endpoint/admission.rs:33-39`).
- Opens stay routed by scheme.
- No dummy SSH owner.
- Genuinely invalid input stays fatal. The change must not turn an action that a live HTTPS stream cannot accept into silent success.

**State-2 P2-4, the closure cases:**
- **(a) Refusal with a racing Cancel.**
  1. The native Opened is refused under the lock.
  2. A full control queue holds the converted OpenFailed.
  3. The initiator's Cancel for that stream is admitted before the enqueue.
  4. A later pass removes the retired entry.

  Then assert:
  - the session stays open;
  - at most one terminal for the stream is published;
  - the physical charge is held until real disposal;
  - a second request on the same Session opens and completes.
- **(b) An endpoint-side cancel or request drop.** The request's HTTPS entry is idle on a live route: a pending native Opened, or a published POST awaiting its first Data. The session stays open.

**Code-2 P3-1, the disposition:**
- **The probe.** Before `send_if(.., || true)`, register a waiter by polling `next_message()` once on the client port, with a probing waker. On its post-release wake, the probe records whether the Opened is already queued.
- **What it shows.** With one critical section, the probe observes the Opened. With a body split into admit-only and send acquisitions, it does not.

**Native refresh, required before acceptance.** At the round-3 tuple, on the operator's Windows host, using round 1's procedure and new labels:
- the exact-source MSVC build and the qualification runner;
- the provisioned CLI and the installed Python wheel clone, fetch and push normal paths, with the refusal cases.

The receipts are archived in gwz-core-evidence, as round 1's were.

**Scope.**
- The round-2 disposition stands: 55 source, test and build files plus three inventories, and 2,600 gross added lines.
- No API, owner, dependency, schema, wire or activation change. Exceeding a ceiling or any structural change stops the patch.

**Review.**
- The round-2 reviewers re-verdict with their context intact. State checks P2-4's closure and that P2-3 stays closed. Code checks P3-1's closure and the routing change's call graph.
- No finding is closed by its implementer.
