# Shared SSH worker — merged remediation 1

Date: 2026-09-21. Baseline core `59ac059c8bfe4052d8a5a16d821f314a3985bcc8`.
Both axes returned NO-GO on independent P2 findings; no blind convergence.

| Finding | Disposition | Closure evidence |
|---|---|---|
| Code P2-1 | Accept: retain pinned libgit2 bracketed SCP authority and root-path forms, with existing redaction/local-path/refusal rules. | Table vectors against native accepted grammar, Route admission before effects, malformed and Windows cases. |
| State P2-1 | Accept: distinguish queued request expiry from worker shutdown. | Deterministic receiver expiry at/past deadline, permit release, live stop flag and a following serviced request. |

One merged correction, then focused fixture rerun and exact-tuple re-verdict by
the same reviewer for each finding. No interface/ownership/activation change.
Native credential setup and production call sites remain outside this checkpoint;
platform/source qualification remains the operator-deferred batch. Test scope
refinement retains only these closure cases and excludes additional credential,
platform and selected-source tests; test ceiling becomes 1,050 lines for the
complete checkpoint. Production ceilings remain unchanged.
