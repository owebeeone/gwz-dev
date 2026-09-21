# SSH N2a — merged remediation 1

Baseline root c33005d576e59ef46222e465285ae1eccf7ceedd, core
42f90094dae33eef1cd4828d2c97cadda2761f43, evidence
b5375aceb57773cf79ba25b443cafa2ada38bc08; remaining pins in reports.
Code NO-GO (two P2s); State GO (one P3). No dual-axis blind convergence.
All findings are addressed in one correction and returned to their retained
originating reviewers. No N2b/N3 scope or activation is added.

| Finding | Disposition | Closure |
| --- | --- | --- |
| Code P2-1 | Accept. Reserve PermissionDenied for explicit unambiguous native authentication failure; ambiguous PUBLICKEY_UNVERIFIED and other terminal errors become sanitized Other. EAGAIN remains the sole retry. | Disconnect a real handshaken native connection during paused selected-key auth; actual SetupConnector/Resource emits Io, never Authentication, disposes the attempt and never invokes another setup. Explicit rejected-key ambiguity expectations follow the native contract. |
| Code P2-2 | Accept. Replace broad evidence claim with actual well-formed encrypted PKCS#8 PBKDF2, traditional encrypted PEM and OpenSSH bcrypt fixtures. Construct high-work-factor fixtures by modifying parameters of cheaply generated containers; do not execute extreme KDFs. | Exact test wrapper around native dispatch counts entries and vetoes unexpected encrypted native entry before any KDF. Every encrypted input returns InvalidInput, zero dispatches and zero remaining charge. An unencrypted positive control reaches real native auth through the same wrapper. |
| State P3-1 | Accept as bounded correction now. Use checked fixed-size scanning for NUL, leading whitespace and newline discovery as well as base64/trailing text. | Deterministically cancel after scanning the first chunk of a near-cap input; prove cancellation precedes scanning later chunks. |

Maintain 600 production/900 test scope bounds. Rerun focused regressions then the
full locked/offline isolated suite once. Archive red and green results separately,
retain old evidence unchanged, settle the corrected tuple, and seek focused
Code/State re-verdicts. At most two merged remediation rounds remain the gate rule.
