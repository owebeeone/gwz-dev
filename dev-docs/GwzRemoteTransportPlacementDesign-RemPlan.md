# Endpoint placement — consolidated design remediation 1

Date: 2026-09-22. Original root cf34abc7b739c82af9f466665e0e514e166f5e35,
core 5be22a1931160c2d124e0705cd516a4b88fdb098. Design-only correction;
no executable tests or implementation claims. One consolidated patch.

| Finding | Disposition | Closure trace / later regression |
|---|---|---|
| Consistency P2-1 | Closed.facts alone supplies failed Closed facts; nested Failure.facts must be absent/null, even equal duplicates are invalid | Trace decoder/adaptor/projector rule; later absent/null admitted, equal/conflicting nested values rejected |
| Consistency P2-2 + Safety P2-1 | Serialize Bound installation vs abandonment; installation wins -> preserve binding; cancellation wins -> retire generation/port, wake all waiters, propagate endpoint closure, fresh binding only | Trace before/after readiness/install, late/duplicate Bound, multiple waiters, shutdown; later deterministic barrier matrix |
| Safety P1-1 | Bind capability admission to exact receiving core/runtime/channel generation, pin through dispatch, invalidate on replacement, prohibit hidden reroute/replay; unavailable without host affinity | Trace new probe then old receiver swap and unaffined host; later zero-send/zero-effect fixture |
| Surface P2-1 | Publish concrete proposed runtime, endpoint, registration, port, cancellation and shutdown signatures plus configure/use/undo example | Walk example against declarations; compile fixture after implementation, not claimed compiled today |
| Surface P3-1 | Name gwz auth identity and --remote-identity accurately | Existing help paths resolve |

Both design axes independently found the bootstrap race. The four distinct
blocking root causes were discovered at initial design review, not in released
code. No findings are self-closed: retained reviewers must verify their original
counterexamples on the corrected committed tuple.

Owner integration clarification: candidate regeneration stays in the existing
isolated harness until dependency activation; normal production artifacts/Cargo
remain unchanged. Later production regeneration uses identical pinned inputs.
Explicit local endpoint_path_base rejects any non-null value. The concrete guide
also defines client request registration, port cancellation/closure, local cleanup
accounting and the distinction between cleanup and Git success. These are direct
Surface lifecycle corrections, and their safety interactions are in re-review.

Checks: package local links and whitespace; document-only review. The broader
check_merge_docs.py has an unrelated unchanged CLI Releases.md missing
releases_unreleased_compatibility statement; do not claim the full gate green.

Same reviewers, focused re-verdict with closure table and changed-range analysis.
This is remediation round 1 of at most 2. Platform/selected-source work remains
one deferred batch; no carrier, new service, CLI command, push or activation.
