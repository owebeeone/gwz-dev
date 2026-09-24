# GWZ operation-session protocol — independent Safety review

Date: 2026-09-23  
Axis: Safety  
Verdict: **NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 4 |
| P3 | 0 |

## Exact reviewed tuple and scope

| Repository | Commit |
| --- | --- |
| gwz-dev | `852fd94a082f82b8d4e9d777edf7d20acdf12576` |
| gwz-core | `26b30ca673f7fbc13aed89f29341a2a9a8a78f5b` |
| gwz-py | `45bcd7b3ea102ca927935cee1b41b43934d68140` |
| gwz-transport | `46e65a9a888fbd4a5bbeace946996581dcf23333` |

The tuple matched at the start and end. I reviewed the committed protocol design and caller guide, the prior Python concurrency Safety report and remediation plan, the controlling transport, retry and Python designs, the Taut schema, and committed host and Python source for feasibility. I did not use working-tree material, inspect current-round peer reports, modify files, or run builds or tests. Wire carrier implementation, platform qualification and release remain outside this verdict.

## Prior-finding closure

| Prior Safety finding | Design-text disposition |
| --- | --- |
| P0-1, cross-client result attribution | Addressed by session-owned records, opaque operation IDs and owner checks on reads and controls. Implementation remains a later gate. |
| P2-1, capacity-transition ownership | Addressed by a leader, target and waiter rules for every transition, subject to the separate required capacity amendment. |
| P2-2, CLI pre-Open capacity admission | Addressed by requiring the bound endpoint to check or refusing CLI placement before dispatch. |
| P2-3, lifetime mux IDs | The rollover rule addresses exhaustion, but leaves caller request-ID reuse unresolved; see P2-2 below. |
| P2-4, unbounded completed retention | Record counts and event bytes are bounded, but terminal-result bytes are not; see P2-3 below. |
| P2-5, unbounded worker admission | Addressed by a worker budget independent of `jobs`, fallible launch and terminalization on failure. |

These are assessments of the proposed contract, not claims that implementation tests have passed.

## Findings

### [P2-1] Local-only operations lack the cancellation and join owner promised by close

- **Where:** `dev-docs/GwzOperationSessionProtocolDesign.md` §§2–3, especially lines 58, 62–64 and 83–94. The accepted Python design §2 keeps local-only handlers on their current backend path. In committed source, `gwz-py/native/src/transport_session.rs::call_inner` dispatches a local call without creating a `TransportRequest`; `native/src/dispatch/local_family.rs::submit` can start a local clone operation.
- **Violated invariant:** Every accepted operation must have one terminal result, and session close must cancel and join every admitted operation before reporting completion.
- **Reproduction/state sequence:** Submit a local-only `clone_local_workspace` operation, then cancel its handle or close the session while its handler runs. The draft defines cancellation as joining that operation’s `TransportRequest.finish()`, but this local path has no transport request. It does not define a separate handler completion or cancellation owner.
- **Impact:** An implementation following the stated rule either cannot support the promised local operation, or can report cancellation/close while local workspace mutation continues.
- **Required correction:** Define one operation-level worker and completion scope for *all* admitted operations. State how local handlers receive cancellation, how close joins them, and how their terminal result and cleanup report are published. Transport finish is an additional step only for operations that have a transport request.
- **Regression test:** Block a local-only submitted handler after admission, cancel it and race session close. Verify one terminal result, no mutation after close returns, and identical retained cleanup reports for repeat calls.

### [P2-2] Reusing a retired caller request ID conflicts with transport-generation tombstones

- **Where:** Protocol design §3 lines 97–99 permits `request_id` reuse after retirement; §4 lines 161–171 describes rollover at the 256-ID ceiling and isolates *different* generations. In committed `gwz-core/src/transport_host/request.rs`, `ClientRequest` and `RequestContext` register `meta.request_id`; `session.rs::register` rejects any ID already in its lifetime `used` set.
- **Violated invariant:** A retired caller correlation ID may be reused without either refusing otherwise admissible work or attaching delayed transport messages to the new operation.
- **Reproduction/state sequence:** On one session, finish operation A with `request_id="x"` and submit B with `"x"` before the generation approaches 256 IDs. The design says B may be admitted, while the current host refuses the registration. Simply removing that tombstone would let a delayed message for A address B within the same generation; the stated rollover rule only isolates messages across generations.
- **Impact:** The advertised request-ID reuse fails, or an unsafe implementation risks cross-operation transport effects.
- **Required correction:** Give every admitted operation an internal, generation-unique transport request ID distinct from the reusable caller `request_id`, or require a safe generation transition before reuse. Keep both IDs explicitly attributed and specify behavior if transition capacity is unavailable.
- **Regression test:** Submit A and B sequentially with the same caller request ID well below the rollover threshold. Both must complete under different opaque operation and transport IDs; inject a delayed A message and prove it cannot affect B.

### [P2-3] A terminal-result “slot” does not bound retained-result memory

- **Where:** Protocol design §4 lines 173–182 budgets retained terminal *records* and event-log bytes, but gives no byte or serialization bound for terminal results. The committed Taut `OperationResult` in `gwz-core/protocol/gwz.taut.py` has lists of member responses, errors and transport observations. The current Python store also retains a separate merge response.
- **Violated invariant:** Finite advertised session budgets must bound retained memory while preserving exactly one readable terminal outcome per accepted operation.
- **Reproduction/state sequence:** Admit operations up to the retained-record count on a large workspace, each producing a large member/error result. Event logs obey their byte limit, yet all terminal payloads remain retained until release or expiry. A single exceptionally large result can also exceed a future carrier’s message limit after the result slot has been reserved.
- **Impact:** Memory can grow far beyond the published budget, or `result_v1` can fail to deliver an accepted operation’s final outcome.
- **Required correction:** Publish and enforce a terminal-result byte budget, including any separately retained operation-specific response. Define an immutable bounded representation or typed oversized-result outcome that remains retrievable when the full payload cannot be retained or encoded.
- **Regression test:** Produce many large terminal results and one result above the per-result limit. Assert bounded peak retained bytes and exactly one readable, correctly attributed terminal outcome for every accepted ID.

### [P2-4] Session creation and abandoned-route lifetime have no receiver-wide bound

- **Where:** Protocol design §2 lines 57 and 77–79 makes `operation_session.open` create a logical session and says an old receiver owns close/expiry after reconnect. Section 4 lines 173–182 specifies budgets only *within each session*. No limit on open sessions, idle-session expiry, lost-owner action, or close-record lifetime is given.
- **Violated invariant:** A bounded operation store and physical-resource owner must remain bounded when callers abandon sessions or lose a route.
- **Reproduction/state sequence:** An authorized route repeatedly opens sessions and leaves them idle; each has finite per-session budgets, but the receiver accumulates session records without a stated aggregate cap. Alternatively, a route starts work, disconnects, and reconnects. Reattachment is prohibited, while no trigger or deadline requires the old session to close; it can retain its host, results and active work indefinitely.
- **Impact:** Resource exhaustion and stranded work remain possible despite every per-session limit. The reconnecting caller has no defined way to observe or stop the old operation.
- **Required correction:** Define receiver-wide and per-route session admission limits, a typed refusal, idle and orphaned-session expiry, and the event that starts close after owner loss. Specify how active operations are cancelled and joined, and bound retention of close/expiry markers. This is an interface contract; it need not implement a wire carrier now.
- **Regression test:** Open sessions to the declared limit and verify the next open refuses without allocating a host. Abandon idle and active sessions, simulate route loss and reconnect, and verify old work closes within the declared bound while a new route cannot read or control it.

## Invariant and matrix assessment

The revised text protects cross-client reads and controls by checking the session and route together. It also gives compatible overlapping operations a capacity rule, keeps CLI placement under its physical owner, and separates worker slots from caller-controlled `jobs`. The four findings above are independent gaps in operation ownership, ID lifetime and resource accounting. None requires changing the deferred wire carrier to correct the contract.

## Commands and exact results

Read-only inspection used `git show HEAD:<path>` and `git -C <member> show HEAD:<path>` for committed documents, schema and source; `rg`, `sed` and line numbering narrowed the cited sections. `git rev-parse HEAD` for the root and three members returned the four exact SHAs above on both checks. No build, test or mutation command was run.

## Residual risks and next action

This is a design verdict. Generated schema checks, implementation races, numeric budget selection, capacity-amendment acceptance and platform evidence remain unexecuted gates. **NO-GO** remains until P2-1 through P2-4 are corrected on a new committed tuple. I pre-commit to **GO** on a revision that resolves these four IDs as specified, provided it introduces no new blocking defect.
