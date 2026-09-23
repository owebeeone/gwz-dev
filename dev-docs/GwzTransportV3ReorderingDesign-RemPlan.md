# Profile-3 reordering design — remediation round 1

Date: 2026-09-23. Review object: core `f837a0007e6fe30866123734c6e509edb58492fe`, root `856ec97774915256691fd29ab3443555e6489d5a`. Both independent axes reported NO-GO. This plan covers one merged correction to the design text; it does not accept implementation or the stopped delivery amendment.

| Finding | Disposition | Closure evidence required |
| --- | --- | --- |
| Consistency P2 (local retirement); Safety P2-1 | Add a distinct locally-retired tombstone that retains original route context and phase, contains only race-legal late completion/cleanup, and never adopts facts, outcomes, or leases. Wrong context/transition remains Protocol. | Two streams share one binding; locally retire A before its first legal terminal, deliver valid late completion and Cancel in either order, observe no change to A or B and no duplicate lease release; wrong-context terminal fails closed. |
| Consistency P2 (CheckIdentity scope) | Explicitly supersede placement §6's `v2-only` wording for profile 3, while preserving profile 1 and 2 behavior. | Bound 3 followed by CheckIdentity completes; Bound 1 refuses; Bound 2 unchanged; v3-only offer to a v2 endpoint refuses before effects. |
| Safety P2-2 (stale-generation false success) | A profile-3 mux session mismatch returns a distinct non-disconnecting `WrongSession` result, never `Ok` admission; the host cannot acknowledge that Open/Check and must fail or atomically abort its original generation. | Deliver S1 opening to S2 port; S2 stays healthy with no route/action; S1 waiter wakes with failed/aborted delivery and no success acknowledgement; independent S2 work continues. |

The correction is confined to the profile-3 design and its required proof list. Reviewers must trace their original counterexamples against the corrected committed tuple. The authoritative design/requirements and code will be changed only after this design gate passes.
