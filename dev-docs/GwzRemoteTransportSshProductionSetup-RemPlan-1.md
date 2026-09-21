# SSH production setup — merged remediation 1

Status: implemented in controlling documents; retained re-review pending.

| Finding | Disposition | Closure gate |
|---|---|---|
| Consistency P2-1 | Accept. G1 explicitly lists regular UTF-8/NUL-free 4 MiB/16 KiB trust admission for both placements; size/encoding maps to InvalidRequest before effects. | N1 native 5 MiB characterization and below/at/above input limits. |
| Safety P2-1 | Accept. G1 intentionally permits complete physical-line parsing up to16 KiB instead of native4091-byte chunks; matching host/key/port policy unchanged. | N1 differential padded-comment and host-list cases below/at/above4091; explicit expected deltas. |
| Consistency P2-2 | Accept. Prohibit address/connection replay after negotiation, expressly preserve accepted A2 ordered per-key attempts and error-specific advancement. | N1 rejected-first/accepted-second success plus no next address after handshake/terminal auth failure. |

One merged documentation patch. Trust-input mismatch is independent blind
convergence on the same compatibility boundary, with stricter-refusal and broader
admission counterexamples. Concrete code/tests remain provisional outside the
workspace until this design gate passes. No implementer self-closure claimed.
