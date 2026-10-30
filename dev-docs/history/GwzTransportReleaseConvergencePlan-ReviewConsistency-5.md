**GO.** No P0–P2 is open. Three P3s remain; none blocks.

# Transport execution plan (revision 5, sha256 b225a24b…) — CONSISTENCY FINAL CLOSURE CHECK

**Review object:** `dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 5, uncommitted, 896 lines, sha256 `b225a24bda6ee05be718513fdae02650b547a94f719abacc2074e89e520e1f8a`.
**Date:** 2026-10-25.
**Axis:** Consistency. Read-only; the Safety axis runs in parallel.

## 1. Closure table

| Round 4 | Status | Evidence in revision 5 |
|---|---|---|
| R4-P2-1 D-L amendment incomplete, no step | **Closed** | §1.1 row (line 100) now covers contract §4, §4.1, §5, §7 and §12:<br>• `HostHandle` has two backings; the carrier-backed one is built by `gwz-connection::remote::handle(binding)`, a constructor argument, "not a fifth `HostPort`";<br>• `admit` carries an operation reference, and the client builds `PreparedOperation`, so §7 holds at the client;<br>• core-side `register` is a deferred commit, and carrier loss is a third outcome, terminal `NeverRegistered` with zero effects;<br>• new step 2.2c (dual, after 1.2 and 2.1, ~400 + 400) builds the handle and the client receiver, with rows for remote reuse, a refused core-supplied profile, a refused unmapped `ExchangeSpec`, and a pending register dropped by the carrier;<br>• 2.3 now runs after 2.2c, and D-L lists all five clauses. |
| R4-P2-2 R1 rule unable to fail under CAP 32 | **Closed** | R1 rule (line 273): "creations ≤ min(CAP, members) … the push and the three reads create none (idle-closed replacements excepted)". |
| R4-P3-1 §9 totals | **Closed** | Rows add to 130–160, which matches the stated total. Phase headers match §9. |
| R4-P3-2 §3.1 inconsistencies | **Closed** | Workload C runs at one member, with a 16-member variant (line 202). The physical/protocol agreement rule applies from 3.0 on, and Phase 1–2 rows rest on physical counts (line 228). Step 3.0b fixes the Windows SSH fixture. |
| R4-P3-3 leftover text | **Not closed** (now R5-P3-1) | §12's column header is fixed. In 1.2 the wrong sentence was removed: line 382 still reads "The host owns pools and endpoint sessions through `EndpointFactory`", and the correct "The host owns endpoint instances and pools" is gone. The next sentence still says the opposite ("only endpoint instances and pools are host-wide"). |

## 2. Defects in the new text

### R5-P3-1 — The 1.2 contradiction is still there
Line 382 says the host owns endpoint sessions; line 384 says both mux sessions are per binding. Replace "pools and endpoint sessions" with "endpoint instances and pools".

### R5-P3-2 — The parallelism diagram and the file ownership miss the new steps
- The Phase 2 line of the diagram has no 2.2c.
- The Phase 3 line has no 3.0 or 3.0b.
- §4's file ownership does not assign `gwz-connection`'s `remote` module. 2.2c (Phase 2 lane) therefore edits the crate the Phase 1 trunk owns while 1.3–1.7 are still in progress there. The plan's own rule ("asks that lane") makes this workable, but the owner should be named.

### R5-P3-3 — Stale headings
- §12's heading still reads "(revisions 1 → 4)".
- The Phase 0 diagram still says "closure check pending".

No P0, P1 or P2 is introduced. D-L, as now recorded, matches contract §3 (edges unchanged), 1.2, 2.0(a), 2.2c and 5.4. The three P3s are text corrections that can be made without another review round.

## 3. Object check at end

`shasum -a 256 dev-docs/GwzTransportReleaseConvergencePlan.md` gives `b225a24bda6ee05be718513fdae02650b547a94f719abacc2074e89e520e1f8a`, 896 lines. This matches the pin given at the start. Nothing was modified.
