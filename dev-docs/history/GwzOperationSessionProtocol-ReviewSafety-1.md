# GWZ operation-session protocol — independent Safety re-review

Date: 2026-09-23  
Axis: Safety  
Verdict: **NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 1 |
| P1 | 0 |
| P2 | 3 |
| P3 | 0 |

## Exact reviewed tuple and scope

| Repository | Commit |
| --- | --- |
| gwz-dev | `ee11f44efa5a0796a71d874ffaa4bc570a611a4e` |
| gwz-core | `8756fd6b32443b0ac63287ee5b4a3335e8cf0894` |
| gwz-py | `45bcd7b3ea102ca927935cee1b41b43934d68140` |
| gwz-transport | `46e65a9a888fbd4a5bbeace946996581dcf23333` |

All four commits matched at the start and end of this read-only review. The object is the committed corrected protocol design, caller guide, and paired core capacity amendment. I used the committed round-1 reports and remediation plan, process rules, accepted transport and Python designs, Taut schema, and selected committed source to check feasibility. I did not inspect current-round peer material, modify files, or run builds or tests. Wire delivery, platform qualification, implementation acceptance, and release remain outside this verdict.

## Changed-range and prior-finding analysis

Relative to root `852fd94a082f82b8d4e9d777edf7d20acdf12576` and core `26b30ca673f7fbc13aed89f29341a2a9a8a78f5b`, the root correction adds an execution scope for local work, a combined terminal record and byte limits, separate transport IDs, receiver/session limits, route-loss close, and detailed admission rules. The caller guide is substantially expanded. The core capacity amendment is a new 88-line draft specifying equal/lower admission and atomic capacity epochs. The relevant regression scope is therefore local cancellation, terminal fallback, legacy/version-1 isolation, capability preflight, and capacity admission; the unchanged member commits remain feasibility evidence.

| Round-1 Safety finding | Re-traced counterexample and disposition |
| --- | --- |
| P2-1, local-only cancel and join ownership | The design now gives every accepted operation an execution scope and says close joins local handlers (`GwzOperationSessionProtocolDesign.md:51–58, 75–77`). The false-completed-close counterexample is closed in the contract. A blocked local handler can still pin a closing session without a recovery bound; see P2-1 below. |
| P2-2, caller ID reuse versus mux tombstones | Caller `request_id`, opaque operation ID, and generation-local transport ID are now separate (`GwzOperationSessionProtocolDesign.md:183–194, 264–274`). The original reuse counterexample is closed at the design level, subject to the stated rollover implementation proof. |
| P2-3, unbounded terminal bytes | Per-record and aggregate byte limits and a reserved fallback now exist (`GwzOperationSessionProtocolDesign.md:97–107, 289–311`). The fallback cannot meet its own attribution contract for an unbounded caller ID; see P2-2 below. |
| P2-4, unlimited sessions and abandoned routes | Session limits, idle expiry, route-loss close, and close-report expiry are now specified (`GwzOperationSessionProtocolDesign.md:162–168, 283–305`). The original unlimited-open counterexample is closed. Abandoned active local work can still keep all receiver slots charged indefinitely; see P2-1 below. |

The antecedent Python concurrency Safety P0-1 also remains material: the correction protects version-1 records but expressly retains the version-0 global lookup. See P0-1.

## Findings

### [P0-1] Preserving the legacy global lookup preserves cross-client result disclosure

- **Location and root cause:** `dev-docs/GwzOperationSessionProtocolDesign.md:109–113, 156–160` retains version-0 scalar event/result methods and module-level Python compatibility functions, requiring only isolation *from version-1 records*. In committed `gwz-py`, `native/src/operations.rs:10–50, 81–115` stores and reads legacy records process-wide by operation ID alone; `native/src/shims.rs:42–43` derives that ID from the caller ID; `src/gwz/client.py:1267–1275` exposes the scalar reads without an owner argument.
- **Violated invariant:** One client must not read another client’s operation events or result, and two clients must not compose their outcomes into one record.
- **Reproduction:** On the retained version-0 path, client A submits with `request_id="x"`. Client B calls `operation_result("op_x")` or `events_subscribe("op_x")`; the global lookup checks no client owner. If B also submits with `"x"`, `OperationStore::begin` returns the same record to both submissions. Isolating version-1 records does not change either version-0 sequence.
- **Impact:** Active cross-client disclosure and false result composition remain available through a supported compatibility path. This is the antecedent Python Safety P0 root cause, not a newly discovered architectural root cause.
- **Required correction:** Specify owner binding and checks for retained legacy reads and writes using the implicit client/route context. Module-level lookups that have no owner context must be confined to a trusted compatibility boundary or replaced with a safe equivalent; namespace separation alone is insufficient.
- **Closure test:** Run two version-0 clients with the same explicit caller ID and opposite completion orders, then attempt foreign event, result, and merge-response reads through every retained compatibility entry point. Verify independent outcomes and refusal of foreign access, alongside version-1 isolation.

### [P2-1] Route-loss close can retain every receiver session slot indefinitely

- **Location and root cause:** `dev-docs/GwzOperationSessionProtocolDesign.md:296–305` keeps a closing session charged until all local work joins, while explicitly allowing local work without bounded cancellation checkpoints to leave close pending. The current local clone path in `gwz-py/native/src/dispatch/local_family.rs:70–99` has no transport request or cancellation argument.
- **Violated invariant:** Receiver-wide session limits and route-loss cleanup must provide a recovery path for abandoned active work without falsely reporting that mutation has stopped.
- **Reproduction:** Accept a local clone whose handler blocks in filesystem work, then drop its owner or lose its route. Five seconds later close reports `ClosePending`, but the handler has not joined, so the session remains charged. Repeat from 32 sessions across routes. The receiver then refuses every new session with `SessionCapacityFull`; the lost routes cannot reattach to finish or release their sessions. Neither the idle TTL nor the 60-second completed-close TTL applies.
- **Impact:** The bounded session count limits memory growth but converts abandoned blocked local work into receiver-wide admission starvation with no stated recovery bound. This is an incomplete closure of round-1 P2-4, tied to the local-work concern in P2-1; it is not a new architectural root cause.
- **Required correction:** Make bounded cancellation or safe termination of admitted local handlers an activation precondition, or define a recovery mechanism that can retire the session while preserving the no-post-close-mutation guarantee. A bounded *wait* alone does not bound cleanup.
- **Closure test:** Block local mutation after acceptance, trigger owner loss and route loss, exhaust the declared receiver session limit, and prove every orphan eventually retires by the specified mechanism. Also prove no mutation occurs after a successful close and repeat close reports remain truthful.

### [P2-2] An unbounded caller ID can exceed the reserved terminal fallback

- **Location and root cause:** `dev-docs/GwzOperationSessionProtocolDesign.md:97–105, 143–146, 307–311` reserves 4 KiB for a `ResultLimitExceeded` terminal that must include the caller `request_id`. Neither its admission rules nor `RequestMeta.request_id` in `gwz-core/protocol/gwz.taut.py:1108–1121` gives that string a length bound. The proposed guide describes it simply as an optional caller correlation string (`GwzOperationSessionCallerGuideDraft.md:7–10`).
- **Violated invariant:** Every accepted operation must retain one readable, correctly attributed terminal outcome within its reserved charge.
- **Reproduction:** Submit a local operation with an 8 KiB caller ID and produce an action response above the 16 MiB per-record limit. The full response cannot be retained. The required fallback cannot encode even its mandatory caller ID inside the reserved 4 KiB, before accounting for the operation ID, action, effect, error, and CBOR overhead.
- **Impact:** The receiver must exceed its advertised reservation, omit required attribution, or fail to publish the promised terminal record. This is an incomplete closure of round-1 P2-3, not a new architectural root cause.
- **Required correction:** Bound and validate all fallback-carried caller metadata before `Accepted`, or reserve a proven worst-case fallback size based on an explicit metadata bound. Include encoding overhead in that proof.
- **Closure test:** Exercise IDs at the accepted byte boundary and one byte beyond it, then force oversized successful and failed responses. The latter must still yield one attributable terminal within the reserved and aggregate byte limits; an oversized ID must refuse before acceptance.

### [P2-3] The permitted capability handshake can construct an endpoint before a placement refusal

- **Location and root cause:** `dev-docs/GwzOperationSessionProtocolDesign.md:70, 113–116, 172–179` allows the existing `transport_capabilities` method as the version handshake, yet only makes `operation_session.open` cheap. The guide promises an unbound `cli` request raises `PlacementUnavailable` before credentials are read (`GwzOperationSessionCallerGuideDraft.md:13–15`). In committed source, `gwz-py/native/src/transport_session.rs:285–296` calls `runtime()` for `transport_capabilities`; `runtime()` constructs `TransportRuntime::from_environment()` (`:225–249`), whose configuration copies the process environment and parses TLS settings (`gwz-core/src/transport_host/local_command.rs:33–45`).
- **Violated invariant:** An unsupported placement must be refused before endpoint construction or credential-environment access, and must return the specified placement error.
- **Reproduction:** With no bound CLI endpoint, request version-1 `cli` placement on a fresh Client. If negotiation uses the expressly permitted existing capability method, it constructs the local endpoint and copies environment values before placement admission. An invalid CA/proxy setting can make negotiation fail before the caller receives `PlacementUnavailable`.
- **Impact:** Refused placement has an unintended endpoint/environment side effect and can report an unrelated configuration error. This is a new, bounded admission-order root cause, not a new architectural root cause.
- **Required correction:** Require a cheap version/placement capability path that does not call endpoint construction or inspect credential-bearing environment before unsupported-placement refusal; or move that refusal ahead of the existing capability path. State the ordering in the contract.
- **Closure test:** On a fresh session with no CLI endpoint and poisoned TLS/proxy settings, request `cli` placement. Assert `PlacementUnavailable`, zero endpoint constructions, zero environment/credential helper accesses, zero Opens, and continued success of local-only operations.

## Invariant assessment and next action

The paired capacity amendment now gives an atomic pre-acceptance capacity epoch and explicitly handles equal/lower requests, later transitions, waiters, and bound CLI refusal. The separate internal transport ID closes the caller-ID reuse counterexample in the design. Those corrections need implementation proofs, but I found no additional capacity-transition defect in the reviewed text.

**NO-GO** follows from the retained P0 disclosure path and three P2 defects above. The correction needs a new committed tuple and independent re-review. No new architectural root cause was found in this round; the P0 and two resource findings continue earlier roots, while the capability finding has a bounded contract and code remedy.
