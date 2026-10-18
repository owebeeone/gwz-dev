# Windows HTTPS WH1 — merged round-3 verdict and native refresh

2026-10-04. **Reviewed round-3 tuple:**

| Repository | Revision |
|---|---|
| root | `c8ebae9a5dee0e876a96484607af3b92ae0298d3` |
| core | `21f9e15ed4360c031f6d2c803224b173f82b3111` |
| transport | `cd007b6868905543caa212155ad8ec99b3a4052c` |
| evidence | `1add0746a1ffc50cd4f6cbe479c630b9cc2babeb` |
| CLI | `6ab16d4` |
| Python | `5df1576` |
| sspi | `582ec00` |

**The reports:**
- [Code-3](GwzWindowsHttpsIntegrationImplementation-ReviewCode-3.md): **GO**, no findings. It closes Code-2 P3-1, re-running the split-body counterexample independently.
- [State-3](GwzWindowsHttpsIntegrationImplementation-ReviewState-3.md): **GO**, no findings. It closes State-2 P2-4, re-tracing all three original interleavings.
- Both keep State-1 P2-3 and the round-1 findings closed.

**Merged gate: GO.**
- Every finding across the three rounds is closed: Code-1 P2-1 and P2-2, State-1 P2-1, P2-2 and P2-3, State-2 P2-4, and Code-2 P3-1.
- There is no new architectural root cause.
- Round 3 was confined to non-architectural corrections under the cap.

**Native refresh at the round-3 sources,** required by remediation plan 3 and Code-2's risk 1. It was run on the operator's Windows host (Windows 11, MSVC 1.95, E: ReFS) and archived at evidence `566db869313cb072beeefe12670f1cab77d07d99`, under `raw/wh1-rem3-*`.
- **Source readback:** 14,442 inputs and 1,143 candidate files, with zero mismatches.
- **The MSVC qualification runner:** 5 of 5.
- **Library checks:** the ordinary and candidate-only checks compile with 0 warnings.
- **The provisioned CLI** passes clone, fetch and push with `--max-per-host 1`.
  - Checkout and refs were verified independently, with no Git on the client PATH.
  - It refuses a wrong CBT, an untrusted chain and a wrong hostname.
- **The installed wheel** passes `Client(max_connections_per_host=1)` clone, fetch and push. It also passes ordered streaming, a call completing while application events stay unconsumed, and close with `pending_local_work=0`.
- **Process hygiene:** every native context was disposed, and the fixtures were reaped. No OS, trust or policy state changed.
- **Artifact hashes:**

  | Artifact | SHA-256 prefix |
  |---|---|
  | CLI | `ca787e18…` |
  | wheel | `e416b2f1…` |
  | PYD | `0029a7a6…` |
  | worker | `6afd0ea0…` |

- **One nonzero exit:** the final readback found a Python bytecode cache that round 1's unchanged runner writes during the CLI build. It is a harness by-product, census-documented and retained.
- **Round 2's native receipts** are archived too. Its qualification build passed 5 of 5.

**Discovery record:**
- **Defects found and closed before acceptance:**
  - three in the initial implementation (round 1);
  - one residual publication schedule (State-1 P2-3, round 2);
  - one pre-existing routing omission (State-2 P2-4, round 3).
- **No release or activation took place,** so no escape is claimed.
