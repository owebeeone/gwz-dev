# Git library design — merged remediation

Date: 2026-09-21. Reviewed core `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf`,
root `37d5038f820f9ecd3934f37545029f80d3d45b58`.
Round 1: Code NO-GO (one P2); State GO; Surface GO. Reports filed verbatim.

| Finding | Disposition | Closure |
| --- | --- | --- |
| Code P2-1: undecided Send surface | Accept. Freeze Repository as Send, !Sync, !Clone in both design and API guide; ownership can move, concurrent sharing cannot. | Original Code reviewer re-traces the two formerly permissible wrappers and confirms only Send now conforms. G0 must include positive Send assertion, negative Sync/Clone compile checks and sequential cross-worker read/drop. |

This changes only the explicit transfer contract and its tests. State and
Surface recheck that changed boundary on the corrected tuple; no runtime
implementation or broader operation API is added. No findings from those axes
need remediation. The SHA backend concern was withdrawn after inspecting the
corrected pinned C implementation; add independent-handle concurrent SHA-256
reads to G0's baseline regression, without expanding the native patch.

One merged remediation round. No blind convergence or new architectural root
cause established; original reviewer determines closure, not the lane owner.
