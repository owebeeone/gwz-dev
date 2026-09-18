# In-memory transport — merged remediation 1

The Code and State reports both return NO-GO on transport `89e9ebefee2598ababbbb227366ecbbd924b5cb5`, core `6a82c646fa9cff5555a182366bc3dd41707bd076`, root `3976ef2c046b333bcf0765db916004f8387d8259`.
This is one patch and the first remediation round. Original reviewers re-verdict
their findings; the lane owner does not self-close them. Blind convergence:
both axes independently found loss of terminal Failure code/effect.

| Finding | Disposition | Correction and closure evidence |
|---|---|---|
| Code P2-1 | Accept | Carry local and peer negotiated Limits in Config, enforce receive admission and validate caller-provided Closed facts before retaining them; validate construction bounds so generated controls fit. Compare exact metadata boundary/excess on typed and codec paths, including directional policies and retry of rejected local facts. |
| Code P2-2 | Accept | Central scheme/policy/identity relation; capability sets denote supported compatible pairs, not an unrestricted Cartesian product. Reject dangling capabilities/no executable pair; exhaustive 2×4×3 Open matrix and effect-free binding rejection tests. |
| Code P2-3 / State P2-1 | Accept, one root | Retain exact peer code/effect in the runtime error. Restrict Cancel to the two supported reasons (Cancelled, Timeout). Test failures after byte prefixes, failed Closed and async wake/error parity. Explicit close still discards unread reverse data by design; failed-close tests read the prefix before requesting close. |
| State P2-2 | Accept | Reserve a separate outgoing-dispatcher waiter slot; application cap excludes it. Test both registration orders at cap 1, application overflow, data and cancellation wakeups, and registration cleanup. |
| Code P3-1 | Accept | Generate depth-aware typed admission, including containers, scalar/absent optional nodes and list elements. Compare typed admission/encoding/decoding around actual depth and collection boundaries. |

Lane-owner validation also found that the broad Cargo protocol include pattern
admits Python cache files after regeneration. Narrow package includes to authored
schema and exported JSON files, and include the public tests and regeneration
scripts without cache files; verify package listing and standalone package
build. Strengthen Monte Carlo failure diagnostics around case construction and
sampled replay failures so every case failure prints its seed. These are bounded
packaging/diagnostic corrections within the same patch, not a scope expansion.

Run the deterministic/default suite, clippy, regeneration check, minimum Rust
version and the fixed 50,000-case campaign again after corrections. Record the
new committed tuple and ask the original reviewers to verify the counterexamples
and changed-range interactions. No physical transport or production integration
is introduced and no interface freeze is claimed.

The depth correction also aligns typed admission's root/collection count,
canonical encoded length and conservative live-allocation charge with preflight.
The regression sweeps all four boundaries, since carrying negotiated limits into
the stream makes consistent accounting necessary. It walks typed fields directly
without serialization or allocating a CBOR tree.
