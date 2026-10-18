# GwzRemoteTransport HTTPS H1 Correction 2 — Code Re-verdict

**Date:** 2026-09-22  
**Axis:** Code  
**Verdict:** **GO**

## Review object

The exact tuple was verified unchanged at the start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `63ef26308979b6ce2e2925d71a96f42afcde2645` | `ff7b0dac901d3da92caa1b7cd237d9593003c1a3` |
| gwz-core | `e29e799ee65fb9794ac2fad7972d94707262b4cb` | `60a315d2ea1ccd492be2ac51725645bd7ac8fb67` |
| gwz-transport | `aa40936d0805e8cb60f8027615abe20d4f2045e4` | `e2c13ebdad077f447cb142c3143fdd3ed1d1a5a2` |
| taut | `bcf98b64d465fc54841121b6d1a2d46940f81a3c` | `583da1f46c3eb56cab1908997e41d5b35e8a37ff` |
| gwz-cli | `7db07bbdefd2897c07fd0f9e550bf032bd8b1314` | `cdcfb401e8ba5ae67e015e175d4caceb742d52b9` |
| gwz-py | `d07d55dacb1725d9306be9c04d157ac29a78e000` | `6d6ab339a70bdd72907c35b6e6b309c772e66358` |
| git2-rs | `ce78628308e11b4e8901d5061602619109bce21a` | `28fc6ad905653db47a7705706e1edea0f39d7054` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |
| gwz-core-evidence | `fc1caa478c1fcd9539b2c061c51be17b64924d7c` | `6f68a5ef9477cfe250d84396ba0e914ef41d8464` |

I reviewed RemPlan-2, the correction-one Code verdict, the complete two-file source change, updated H1 report, and the `2026-09-22-h1-rem2` red/green evidence. The committed `https_auth.rs` and `https_worker.rs` hashes match the evidence manifest.

The archived full endpoint gate records **66 passed**. I independently ran the permitted existing binary’s focused authentication tests: **9 passed, 0 failed**. No build was performed.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Code P2-1 | Operation routes remain until explicit whole-operation sealing and final dependent release. | Operation, route, local-RPC, and preparation code are unchanged from correction one; correction two does not touch the dependency or sealing paths. The accepted A/B/C counterexample remains covered by the passing full gate. | **Closed** |
| Code P2-2 | Allocation, interaction, connect, active-I/O, and cleanup budgets remain independent, including zero/custom I/O and Open shortening. | Budget and pool implementation are unchanged. The only worker change is shutdown accounting after cancellation. All budget regressions remain passing in the 66-test gate. | **Closed** |
| Code P2-3 | Hyper parse failures map to `Protocol`; transport loss maps to `Io`. | Classifier and both discovery/POST call sites are unchanged; malformed-header and network-loss mux regressions remain passing. | **Closed** |
| Code P3-1 | Conditional test code has an explicit `cfg_if` boundary. | `shared_reservation.rs` is unchanged, and the correction-two conditional-boundary check passes. | **Closed** |
| Code P3-2 | Post-start cancellation and task abortion retain helper ownership until actual reap. | The existing post-start cancellation and abort tests remain passing. Correction two strengthens the same ownership path during reaping itself. | **Closed** |
| Owner P2-1 | Guard in-flight reap batches, count concurrent arrivals, preserve orphan ownership, and order shutdown snapshots to prevent false zero. | Both correction-one counterexamples are retained red: aborting the reaper reported zero while releasing its child/permit, and a child arriving during a wait was omitted from the returned count. On the corrected source, both tests pass. Source tracing confirms `ReapBatch` and `OrphanReapBatch` return every unreaped child and its permit to the original registry on future cancellation; in-flight counts remain observable; return counts include queued arrivals. Shutdown reads active preparations before the final retained-helper count, matching the transfer order. | **Closed** |

## Changed-range analysis

Only `https_auth.rs` and `https_worker.rs` changed in runtime source.

`AuthOwner` now tracks children temporarily owned by an active reaper. `reap_pending` moves queued children into an RAII `ReapBatch`, increments the in-flight count before releasing the registry lock, and removes each count only after a successful child wait. If the future is aborted or the deadline expires, batch drop starts termination again, returns each child and its still-held admission permit to the same endpoint owner, then removes the in-flight count while holding the registry lock. An observer therefore sees either in-flight ownership or queued ownership, never an unowned interval.

The compatibility orphan registry uses the same guarded-batch pattern and retains each original owner ID. This avoids creating a different cleanup authority or attributing orphan work to an unrelated endpoint.

`Endpoint::shutdown` now completes its reap and physical shutdown attempts, then snapshots active preparations before retained helpers. A helper transition records the retained child before releasing the preparation slot, so this order may conservatively overcount a concurrent completion but cannot falsely return zero during the handoff.

The two causal tests exercise the exact missed states rather than only ordinary helper cancellation. The first aborts a reaper after it has taken ownership and verifies both retained count and unavailable permit before successful reap. The second inserts a new retained child while another child is being awaited and verifies the returned count equals the owner’s observable pending count.

This is a **bounded continuation of the existing helper child/permit ownership invariant**, not a new architectural root. It corrects cancellation safety and observation linearization inside the endpoint-scoped owner introduced in correction one. It adds no public interface, protocol field, dependency, cross-component ownership boundary, or activation path.

No new P0–P3 finding was found in the changed range.

## Verdict and pre-commit conditions

Owner P2-1 is closed, all prior Code closures remain valid, and the correction-two H1 candidate is **GO**.

There are no additional Code-axis pre-commit conditions. This verdict retains all stated H2, shared-authority injection, platform/source qualification, production activation, physical transport, release, and performance deferrals.
