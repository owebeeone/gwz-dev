# GwzCoreSessionDesign — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzCoreSessionDesign.md` at root `e4b8d4332ccc87712a946a34c2d8355e22a8b633` (DRAFT contract design, dated 2026-09-24, "review required; no implementation or activation authority"); its paired DRAFT sections at gwz-core `31036fdde1cc7d87fcaa7a977527acfe0f526d1d` (`dev-docs/GWZDesign.md` lines 9–11 "Core session host (2026-09-24; DRAFT, design review pending)"; `dev-docs/GWZRequirements.md` lines 9–11 "Core session host amendment (2026-09-24; DRAFT, design review pending)"); the pointers at gwz-py `5f938a05fd505a83ab93c74c09533afe6fe6f3b2` (`dev-docs/GwzPyDesign.md` lines 315–320 "Draft session pointer"; `dev-docs/GwzPyTransportDesign.md` line 3). Controlling DRAFT: `dev-docs/GwzClientCoreTransportProposals.md` at root `e4b8d43` (§2 G1–G11, §4, §5, §8).
**Baseline:** root `e4b8d433…`, gwz-core `31036fdd…`, gwz-py `5f938a05…`; gwz-cli working tree at `e926b2b5…` for `src/globalargs/dispatch.rs`. All committed content read with `git show <sha>:<path>` / `git -C <member> show <sha>:<path>`; working-tree reads only for gwz-cli, taut and taut-shape-py sources. Tuple verified identical at start (17:41 AEST) and at end of the review. No writes, builds, tests or network.
**Date:** 2026-09-24
**Axis:** Consistency — the document against its controlling graph: internal contradictions; agreement with every cited contract/design (quotes verified at source); exactness of superseded-clause lists; satisfiability of its own verification section; unstated impacts on uncited documents. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — 0 P0, 0 P1, 4 P2, 13 P3. Every finding is a bounded text-or-schema correction; none is a new architectural root cause. I pre-commit to GO on a revision that resolves P2-1, P2-2, P2-3 and P2-4 as specified below.

---

## 0. Evidence base

Object and controlling documents (committed content, full text unless noted):
- `dev-docs/GwzCoreSessionDesign.md` @ e4b8d43, lines 1–363 (all).
- `dev-docs/GwzClientCoreTransportProposals.md` @ e4b8d43, lines 1–256 (all); §2 table lines 26–38, §4 lines 72–78, §5 lines 80–122, §8 lines 185–214, §9 line 218.
- gwz-core `dev-docs/GWZDesign.md` @ 31036fdd: lines 5–11 (retired note, DRAFT paragraph), 1566–1617 ("Operation Runtime"), 1618–1745 ("CLI Driver Design"); commit diff of 31036fdd.
- gwz-core `dev-docs/GWZRequirements.md` @ 31036fdd: lines 9–11 (DRAFT amendment), 17–66 ("Remote transport amendment"), 216–226 (REQ-010/011), 931–954 (REQ-097, REQ-100–103).
- gwz-core `README.md` lines 7–10, 87; `AGENTS.md` lines 6–8; `docs/TransportPlacement.md` lines 1–80; `docs/Protocol.md` lines 1–60, 150–200; `docs/Reference.md` 60–85; `docs/Embedding.md` 80–110; `docs/MessageCatalog.md` (method rows 48–53); `docs/ErrorCatalog.md` (grep for codes); `dev-docs/GwzRemoteTransportDesign.md` lines 250–304 (§4.1, §4.1.1) and §2 table row 58; `dev-docs/GwzTransportSequencedStreamDesign.md` lines 1–15, 23–43; `dev-docs/GwzV110Plan.md` lines 62–78, 296–301, 414–446.
- gwz-py `dev-docs/GwzPyDesign.md` @ 5f938a0 lines 1–421 (all); `dev-docs/GwzPyTransportDesign.md` @ 5f938a0 lines 1–280 (all); `README.md` lines 10–20; commit diff of 5f938a0.
- gwz-core `protocol/gwz.taut.py` @ 31036fdd: service lines 8–167; `AggregateStatus` 532–540; `EventKind` 648–659; `GwzErrorCode` 669–836; `TransportRuntime*`/`TransportCapabilities*` 1065–1086; `RequestMeta` 1115–1127; `ResponseMeta` 1130–1142; `GwzError` 1153–1163; `OperationResult` 1731–1743; response wrappers 2188–2235; `DiffOutputLogRef`/`LogOutputLogRef` 2449–2584.
- taut `dev-docs/TautDecisions.md` lines 119–134 (D17–D20); taut-shape-py `src/taut_shape/tool/framing.py` lines 1–120; `src/taut_shape/log/_generated.py` (`LogReadRequest` line 87, `LogReadResponse` line 109).

Source facts (committed unless noted):
- gwz-cli `src/globalargs/dispatch.rs` (working tree @ e926b2b): lines 4–19 (`execute_invocation`), 21–60 (`execute_with_backend`, sinks), 300–351 (diff/hook/log `unreachable!` arms), 353–370 (`transport_meta`).
- gwz-core `src/transport_host/local_command.rs` lines 1–145; `src/transport_host/mod.rs` lines 21–160, 240–270; `src/transport_host/request.rs` lines 1–60, 120–160, 280–345; `src/operation/push_event.rs` lines 283–380, 620–655; `src/operation/operation_runtime.rs` lines 1–40; `src/operation/membermutationguard.rs` lines 1–38; `src/operation/workspace_mutator_lock.rs` lines 1–60; `src/model/mod.rs` lines 13–149 (`ErrorCode`); `src/protocol/convert.rs` lines 70–91; `src/protocol/generated.rs` lines 1593–1594, 1671–1672, 1748–1749; `src/git/gitbackend/transport_support.rs` lines 277–283.
- gwz-py `src/gwz/bridge.py` lines 1–643 (all); `src/gwz/client.py` lines 131–236, 349–400, 1162–1320, 1322–1347 and the method index; `native/src/error.rs` 1–70; `native/src/transport_session.rs` 181–240, 960–1043, 1176–1466, 1466–1510; `native/src/diff_logs.rs` 1–30; `native/src/lib.rs` and `native/src/dispatch/*` (function index); tests: `src/tests/test_transport_session_api.py` 55–80, 225–310; `test_client.py` 255–275; `test_transport_session_native.py` (direct native usages); grep counts of `NativeCoreBridge(native=` across `src/tests`.

Commands: `git rev-parse HEAD` (×3 repos, start and end); `git show`, `git -C … show`, `git … show --stat`, `git … grep`, `git … ls-tree`; `grep`, `sed -n`, `awk`, `nl -ba` over the above.

## 1. Findings

### [P2-1] Two of the seven "existing" error codes do not exist in the protocol, and the model-to-wire conversion collapses three of them to `io_error`
- **Location:** contract §4.2 line 131 ("Error codes: all reused, none added: `InvalidRequest`, `OperationNotFound`, `OperationExpired`, `OpenOperation`, `TransportSessionFull`, `InternalError`, and `cancelled` (73)"); §4.1 line 94 (`SessionError { … code: GwzErrorCode (2) … }`); §13 line 301 ("No new error codes"); §15.2 line 329 and §15.6 line 341 (typed-code assertions); §5.1 lines 160–161; §4.2 lines 127–128.
- **Violated invariant:** a frame field typed `GwzErrorCode` must be able to carry every code the contract mandates; §15 line 326 "Assertions check typed fields and codes only"; G8/§13 "no new error codes" must be true as written.
- **Evidence:** the Taut `GwzErrorCode` enum (`protocol/gwz.taut.py` 669–836) ends `url_scheme_unavailable=72`, `cancelled=73`, `transport_record_limit=74`; a whole-schema grep finds no `transport_session_full`, `operation_expired` or `transport_capacity_conflict`. They exist only in the Rust model (`src/model/mod.rs` 141–148, documented there as native-Python-session bridge errors). `src/protocol/convert.rs` 84–88: `// Native-session bridge errors do not enter an OperationResult.` then `TransportCapacityConflict => Self::IoError`, `TransportSessionFull => Self::IoError`, `OperationExpired => Self::IoError`, `Cancelled => Self::IoError`. The generated Rust enum has a `Cancelled` wire member (generated.rs 1594/1672) but the model's `Cancelled` never reaches it through the conversion.
- **Reproduction:** (1) queue holds 64 entries; the 65th request arrives (§5.1); host builds `SessionError{code: TransportSessionFull}` — no Taut member exists; via the existing conversion it encodes as `io_error`. (2) `operation.cancel` on an evicted record must reply `OperationExpired` (§4.2); same failure. (3) a queued operation cancelled by close gets a `Cancelled` terminal (§5.3/§8 step 2) built from the model code; its `OperationResult.errors[0].code` reads `io_error` on the wire.
- **Impact:** the client cannot distinguish "queue full, retry" from an I/O failure; the bridge rule "`OperationExpired`; the bridge treats that as 'already finished'" (§4.2 line 127, §10) is unimplementable; §15.2, §15.4 and §15.6 are unsatisfiable as written; §13's G8 statement is false.
- **Required correction:** append `transport_session_full` and `operation_expired` (decide `transport_capacity_conflict`) to the Taut enum after 74 (append-only, G8-compatible); change `convert.rs` so `Cancelled`, `TransportSessionFull` and `OperationExpired` map to their own wire members; rewrite §4.2/§13 to "two codes appended, none redefined".
- **Closure test:** round-trip a `SessionError` carrying each of the seven codes through the generated Rust and Python projections and assert the same member; a queued operation cancelled by close yields `OperationResult.errors[0].code == cancelled (73)` on both bridges; §15.2/§15.6 pass with typed codes.
- **Classification:** bounded correction (schema append + conversion + text).

### [P2-2] The cancellation path is specified through `with_local_transport`, which exposes no cancellation handle
- **Location:** §5.2 lines 166–168 ("it runs `with_local_transport(meta, operation_id, handler)`. That builds the operation's own runtime, registers its single request, runs the handler, finishes the request and shuts the runtime down."); O4 line 55; §5.3 line 178 ("its worker's transport cancellation signalled, which fails its network I/O"); §8 step 3 line 223; §15.4 line 336.
- **Violated invariant:** proposals §8.1 line 191: the host "sends a cancel message to that worker's transport cancellation"; contract O4/§5.3.
- **Evidence:** `local_command.rs` 12–16: `pub fn with_local_transport<T>(meta: RequestMeta, operation: String, action: impl FnOnce(&Git2Backend) -> T) -> ModelResult<(T, CleanupReport)>`; the `TransportRequest` lives in the module-private `Command` (47–51) and is never returned; the handler receives only `&Git2Backend` (line 29). The only handle constructor is `TransportRequest::cancellation_handle()` (`request.rs` 321–327), which the caller of `with_local_transport` can never reach. (GwzPyTransportDesign §3 lines 153–160 froze exactly that handle for this purpose.)
- **Reproduction:** worker enters `with_local_transport` per §5.2; a `fetch` blocks in an SSH open against a non-answering host; client sends `operation.cancel(op)`; the host holds no `TransportCancellation` for `op`; the cancel reply "waits for the worker's terminal" (§5.3), which arrives only when core's own admission/stall deadlines expire. `session.close()` step 3 has nothing to signal.
- **Impact:** §5.3 running-operation cancellation, §8 step 3 and §15.4 ("cancelled promptly") are unimplementable as written; close of a session with live network work waits for the network deadlines.
- **Required correction:** specify the shared dispatch's transport entry as a variant of `with_local_transport` that hands the session host the request's `cancellation_handle()` before the handler runs (returned handle or a pre-run callback), and list this core change in §5.2 and §16 beside the fetch/push member-lock change.
- **Closure test:** core unit test: an operation blocked in a transport open against a latch-held endpoint; `operation.cancel` returns within the admission deadline with a cleanup report; the handler's I/O fails with `Cancelled`.
- **Classification:** bounded correction.

### [P2-3] The error frame cannot carry the structured error context the public errors expose today (G11)
- **Location:** §4.1 line 94 (`SessionError { call_id, code, message, meta: bytes (4, optional) }` — `meta` undefined); §10 line 257 ("`Client` and `CoreBridge` keep their shape (G11)"); §12 (wire proof); §15.8.
- **Violated invariant:** G11 "the errors" keep their shape; G3/§12 — the wire must deliver what the in-process path delivers.
- **Evidence:** today's native error attaches `code`, `member_id`, `member_path`, `target_kind`, `detail`, `machine_message`, `record_context{merge_id, schema, record_schema_version, required_wave, legacy_mode}` and `response_meta_cbor` (`native/src/error.rs` 13–67); `bridge.py` `_native_bridge_error` 629–643 projects all of them onto `GwzBridgeError`. The Taut `GwzError` message (schema 1153–1163) already carries code, message, member_id, member_path, detail, target_kind and record_context; `ResponseMeta` (1130–1142) is separate. The contract's frame carries a code, a string and undefined bytes.
- **Reproduction:** a merge compatibility refusal with `record_context` crosses as `SessionError`; the bridge can fill only `code` and the message; `GwzBridgeError.record_context`, `.member_id`, `.target_kind`, `.response_meta` become `None`. Both bridges lose it equally, so the §12 two-run comparison cannot detect the regression.
- **Impact:** diagnosability regression for every model error crossing the boundary; a silent G11 shape change (attributes present today, absent after).
- **Required correction:** define the error frame body as the Taut `GwzError` plus optional `ResponseMeta` (for example `SessionError { call_id, error: GwzError, response_meta: ResponseMeta optional }`), and state the mapping to `GwzBridgeError` fields (`machine_message` ← `GwzError.message`).
- **Closure test:** trigger a member-scoped model error and a merge-record error through `NativeCoreBridge` and `StreamCoreBridge`; assert `member_id`, `target_kind`, `record_context` and `response_meta.request_id` equal the in-process values.
- **Classification:** bounded correction.

### [P2-4] Session close waits without bound, contradicting §8.1 of the controlling recommendation
- **Location:** §8 line 224 (step 4 "waits for every worker to report its terminal"), line 231 ("The cost is that a handler which never returns keeps close waiting"), §15.7 line 345, §16 line 358; paired GWZRequirements line 11 ("Closing a session MUST cancel its operations and wait for their workers").
- **Violated invariant:** proposals §8.1 line 192: "when the session closes, cancels every worker and waits for them within a bound;" — a fixed part of the recommendation, not one of the four open decisions the contract's §1 lists as adopted/overturnable. Proposals §5 line 115: "The session host never blocks on an operation."
- **Reproduction:** a handler blocked in a native call that never returns (the proposals' own "native hang" cost); client sends `session.close()`; steps 4–7 never run; over the byte stream the host binary never closes the channel (the §12 CI run hangs rather than fails); in-process the pump thread stays in `recv()` and every outstanding call (results, held reads beyond their 30 s) waits forever because step 5 never runs.
- **Impact:** no bounded recovery at session end; the only bounded wait in the design (reads, §5.5) is undermined by an unbounded close; §15.7 as written enshrines the deviation.
- **Required correction:** either implement §8.1 exactly — add a close wait bound to §1's limits, define the post-bound state (close answers with `peer_cleanup_confirmed=false` and the count of unfinished workers; step 5 runs; unfinished workers are detached and their terminals discarded; the byte-stream host closes the channel) — or record the unbounded wait explicitly in §1 as an overturned recommendation with the reason, and align the GWZRequirements MUST.
- **Closure test:** a handler that blocks on a test latch; `session.close()` answers within the bound with unconfirmed cleanup; releasing the latch later has no effect on session state; repeated close returns the same report.
- **Classification:** bounded correction.

### [P3-1] §14's supersession list for the Python transport design is inexact
- **Location:** §14 lines 316–321 ("superseded in: §2's per-call runtime flow; the single-active-operation rule; the Python network lock. §4's capability rule survives as an ordinary `transport_capabilities` call").
- **Violated invariant:** exactness of superseded-clause lists; GwzPyTransportDesign line 5 remains "accepted for implementation".
- **Evidence:** (a) GwzPyTransportDesign §2 (lines 49–124) specifies one long-lived runtime per session with per-call `runtime.request()`, and lines 120–122 state "The current `with_local_transport` helper creates and shuts down a runtime per command, so the Python bridge must not use it for each call." The contract does the opposite (§5.2) yet names the superseded clause "per-call runtime flow", inverting §2's content and hiding the overridden MUST NOT. (b) §2 lines 62–64 "changing process environment mid-session does not silently change its credentials or trust context" is contradicted by per-operation `environment_config()` (`local_command.rs` 34 captures `std::env::vars_os()` on every call) and is not listed. (c) §4 lines 202–203 "the capability preflight and dispatch must use the same live core receiver/runtime generation" cannot survive per-operation runtimes; only the file-identity check (`client.py` 349–372) survives. (d) §4 lines 187–190: cancel of "an unknown, foreign, older completed or expired ID raises typed `GwzBridgeError(code="InvalidRequest")`" is replaced by §4.2's `OperationNotFound`/`OperationExpired` and not listed. (e) §5 lines 247–250's physical-session-reuse proof is abolished by §16 line 359 and not listed.
- **Impact:** an S6 implementer holding the accepted design and the contract receives contradictory MUSTs; the accepted design's tests assert behaviour the contract abolishes.
- **Required correction:** rewrite the list to name each superseded clause exactly (single long-lived runtime and the with_local_transport prohibition; environment stability; single-active refusal and asyncio lock; same-generation capability clause; cancel error codes; session-reuse test) and what survives (file-identity preflight, `TransportCleanup` shape, `meta(max_retries)`, gh-only HTTPS, sanitisation).
- **Closure test:** documentary: every must/MUST in GwzPyTransportDesign §§1–5 is either unaffected or named in §14.
- **Classification:** bounded correction.

### [P3-2] The event-log rule is called "adopted unchanged" but differs from GWZDesign and the code in unit and protection
- **Location:** §1 table ("event log per operation | 2 MiB"); §5.4 line 186 ("the oldest incremental events are dropped and a reset marker is kept … The terminal event and the result are never dropped. This is GWZDesign's existing event-buffer rule."); §14 line 310 ("Its event-buffer rule is adopted unchanged"); §15.6 line 343.
- **Evidence:** GWZDesign 1603–1607: "V0 uses a bounded ring buffer. If the buffer overflows, the runtime drops older buffered incremental events, records overflow state, and keeps a reset event plus later events … The final `OperationResult` is retained separately and must not be dropped" — counted in events (`OperationRuntime::new(event_capacity: usize)`, push_event.rs 284), protecting the result only. push_event.rs 628–655 clears the whole buffer on overflow (`state.events.clear()`) and pushes one `reset`.
- **Impact:** §1's byte limit is not GWZDesign's rule; "terminal event never dropped" is new; an implementer reusing `OperationRuntime` cannot satisfy §1; §15.6 tests a rule its cited source does not contain.
- **Required correction:** either adopt the record-count rule (put the count in §1) or state the amendment (bytes + protected terminal event) and amend the GWZDesign "Operation Runtime" paragraph per gwz-core AGENTS.md line 7.
- **Closure test:** an overflow test verifies the stated unit and the protected terminal event.
- **Classification:** bounded correction.

### [P3-3] §8's step 1 contradicts the repeated-close rule
- **Location:** §8 line 221 (step 1 "refuses every later call with `InvalidRequest` ("session closing")") versus line 229 ("Repeated `session.close()` calls, accepted until step 7, receive the same report") and §15.7 line 346; §10 line 270 does not say how `close()` answers after step 7 (today `_close_result` is retained, bridge.py 273–299).
- **Impact:** a literal step-1 implementation refuses the second close and fails §15.7; the post-closure bridge behaviour is unspecified.
- **Required correction:** "refuses every later call other than `session.close`"; state that after channel closure the bridge returns its retained report.
- **Closure test:** §15.7's repeated-close case, including one close issued after the channel has closed.
- **Classification:** bounded correction.

### [P3-4] The `Cancelled` terminal has no typed representation in `OperationResult`
- **Location:** §5.2 lines 169–173, §5.3 line 177, §8 step 2, §15.4 lines 335–338.
- **Evidence:** `OperationResult.aggregate_status` is `AggregateStatus {accepted, ok, noop, rejected, partial, failed, dirty, conflicted}` (schema 532–540, 1731–1743); no cancelled status exists; the contract does not say which status and which `errors[]` entry represent `Cancelled`, nor how `Failed (InternalError)` for thread-creation failure or a panic is projected, nor what `members` holds for a queued operation that never planned.
- **Impact:** two implementers (or the two bridges' test expectations) may project the terminal differently; §15.4's "typed fields and codes" assertions have no defined target.
- **Required correction:** specify `aggregate_status = failed`, `errors = [GwzError{code: cancelled}]`, member rows as planned/skipped; likewise for `InternalError` terminals.
- **Closure test:** cancel a queued submit and a running fetch; assert the specified fields on both bridges.
- **Classification:** bounded correction.

### [P3-5] Role-out call bodies are undefined and §13's promised log-read messages shadow taut-shape's log contract
- **Location:** §4.1 line 98 ("`request` and `response` are the Taut-encoded messages that the called method declares"); §4.2 lines 114–118; §13 line 299 ("Request and reply messages for log reads, stream end and release").
- **Evidence:** `events.subscribe`, `operation.result`, `diff.output` and `log.output` declare scalar params (`operation_id=STR`, `log_id=STR`; schema 134–140, 155–157, 165–167), not request messages. The schema's own comments (152–154): "Cursor/tail/EOF/close/backpressure/retention are the shape_log contract — NOT fields here". taut-shape generates `LogReadRequest`/`LogReadResponse` (taut-shape-py `log/_generated.py` 87, 109); gwz-core re-exports them (`gwz_core::diff::{LogReadRequest, LogReadResponse, LogReadState}`, used by `native/src/diff_logs.rs`), and `diff.output` reads carry a `stream_id` (bridge.py 63–70, 108–121). gwz-core AGENTS.md line 8: "do not create a shadow protocol".
- **Impact:** an implementer cannot tell what bytes a `SessionCall(events.subscribe)` carries; GWZ-local read messages duplicate the shape_log messages and diverge from `diff.output`'s existing stream/cursor semantics.
- **Required correction:** specify that read calls carry taut-shape's `LogReadRequest`/`LogReadResponse` (append type as payload, handle field explicit) for all three logs; add GWZ messages only for `operation.result`, stream end and release where taut-shape has none.
- **Closure test:** documentary plus a codec round-trip of each read call body on both language projections.
- **Classification:** bounded correction.

### [P3-6] `configure_transport_runtime` is specified as session-wide but the setting it names is process-global
- **Location:** §4.2 line 106 ("changes session-wide transport settings for operations admitted after its reply and leaves running operations unaffected").
- **Evidence:** `transport_support.rs` 279–281: `server_timeout_ms()` reads a static `TIMEOUT_STATE` mutex; `SshEndpointConfig::from_environment()` reads it (`transport_host/mod.rs` 55); `with_local_transport` builds the runtime from `environment_config()` with no per-session input (`local_command.rs` 21–22).
- **Impact:** two sessions in one process (two `Client`s, or gwz-py's CLI and API) share the setting; the isolation the contract claims requires a session-scoped override plumbed through the shared dispatch, which the contract does not require and §15 does not test.
- **Required correction:** either state "process-wide, existing behaviour" or specify the session-scoped setting, its plumbing into per-operation runtime construction, and a two-session test.
- **Closure test:** two sessions in one process; configuring one leaves the other's next operation's deadline unchanged.
- **Classification:** bounded correction.

### [P3-7] gwz-cli §11 drops human-mode live progress while claiming unchanged user-visible behaviour
- **Location:** §11 lines 284–288 (unary call, or submit + reads only "for `--jsonl` progress"; "User-visible behaviour does not change").
- **Evidence:** `dispatch.rs` 28–36: Human mode renders a live progress line to stderr through `StderrProgressSink` (TTY-gated); only JSONL uses `JsonlSink`. A unary call yields nothing until the reply.
- **Impact:** a TTY user loses the live progress line, or the CLI must use submit+reads for human mode too, contradicting §11; §15.10's suite run would not detect a TTY-gated stderr change.
- **Required correction:** "as a submit followed by event reads whenever the renderer consumes events (JSONL, and TTY human progress); as a unary call otherwise."
- **Closure test:** a CLI test with a pseudo-TTY asserts the progress line still appears under the session path.
- **Classification:** bounded correction.

### [P3-8] The GWZRequirements DRAFT amendment carries a MUST the contract does not make and that contradicts the baseline's direct-embedding statements
- **Location:** GWZRequirements line 11: "Clients MUST reach core's operations only through the session protocol's frames".
- **Evidence:** the contract scopes the session to gwz-py and gwz-cli (§1, §11 "In a later phase") and never forbids direct embedding. Baseline: REQ-010/011 (lines 218–225: standalone library; "All core operations MUST be callable in-process"); README lines 7–8 ("A local adapter can call it directly"); `docs/Reference.md` 72–74 (synchronous `handle_*` "simpler entrypoints for direct embedding and tests"); `docs/Protocol.md` 157–159.
- **Impact:** an extra MUST not in the contract (fails "same claims, no extra MUSTs"); accepted as written it makes gwz-core's documented direct embedding and its own test harness non-compliant.
- **Required correction:** "gwz-cli and gwz-py MUST reach core's operations only through the session protocol's frames; direct in-process embedding of the library remains supported (REQ-010/011)."
- **Closure test:** documentary cross-check of the paired paragraph against §1/§11.
- **Classification:** bounded correction.

### [P3-9] The in-process queues are "bounded" with no bound and no overflow behaviour
- **Location:** §3 lines 76–78 ("two bounded queues … `send(frame)` never blocks"), line 81; §9 line 242 ("non-blocking; fails once the session has ended"); §1 table (no queue capacity).
- **Violated invariant:** an option without a default; proposals §5 lines 115–118 (host runs the channel without blocking); O2 (one reply per call).
- **Impact:** if capacity is below the 1024 outstanding calls plus close, either the host blocks on a full reply queue or must drop a reply; the client's `send` on a full call queue has no defined outcome.
- **Required correction:** add both capacities to §1 (at least outstanding-call limit + 1 each way) and define overflow (host side cannot occur by construction; client-side `send` returns a typed full error mapped to `TransportSessionFull`).
- **Closure test:** 1024 held reads plus a cancel and a close complete without host blocking or reply loss.
- **Classification:** bounded correction.

### [P3-10] §15.8's "whole existing suite passes through `NativeCoreBridge`" is unsatisfiable as written
- **Location:** §15.8 line 349; §12 line 294 ("CI runs gwz-py's whole test suite twice"); §9 lines 248–253; §10 line 257.
- **Evidence:** the suite constructs `NativeCoreBridge(native=…)` with fakes of the old `NativeModule`/`TransportSession` protocol 24 times (`test_transport_session_api.py` ×12, `test_bridge_transport.py` ×6, `test_native_bridge.py` ×2, `test_transport_session_native.py` ×2, `native_helpers.py`, `test_client_log.py`); `test_transport_session_native.py` drives `native_module.TransportSession()` directly (lines 43–160); `test_transport_session_api.py` 296–306 asserts `cancel_operation("op_foreign")` raises `InvalidRequest` "not owned", which §4.2 changes to `OperationNotFound`.
- **Impact:** the thin bridge cannot drive those fakes; the criterion can only be met by rewriting part of the suite, which §15 does not say, so it will be reinterpreted silently; the "twice" CI run has no meaning for tests that never touch a bridge.
- **Required correction:** scope §15.8/§12 to Client-level and native-integration tests; state that bridge-internal tests are rewritten against the four-operation channel with a fake channel, and list the assertions that change.
- **Closure test:** the CI job lists the two runs' test sets and they are identical and non-empty for bridge-crossing tests.
- **Classification:** bounded correction.

### [P3-11] `diff.output`/`log.output` logs have no retention or closure rule
- **Location:** §2 (host owns "retention"); §5.4 (submitted records and event logs only); §1 table (no `log_id` limit); §8 (closure steps do not end or release logs); §4.2 line 118.
- **Evidence:** today's native extension keeps process-global registries (`diff_logs.rs` 1–30 "process-global store", `log_outputs.rs`), and `log_output_release` "Idempotently release a commit-log output and its temporary spool" (bridge.py 132–133; `client.py` 1266–1310 releases on EOF/cancel).
- **Impact:** a dropped iterator or an ended session leaves logs and temporary spools unbounded and unreleased; §3's "Volume is bounded on both sides" does not cover them.
- **Required correction:** add a per-session limit on open `log_id` logs, the rule that session end releases them (spools deleted), and the error code for reads on a released log.
- **Closure test:** open N logs without release, close the session, assert the spool directory is empty and the N+1th open refuses with the stated code.
- **Classification:** bounded correction.

### [P3-12] The shared model's `request_id` uniqueness rule is silently dropped
- **Location:** §4.3 line 137 ("no uniqueness rule applies across operations beyond core's existing validation").
- **Evidence:** proposals §5 line 105: "The client chooses each request's `request_id`, unique within the session. Core returns the `operation_id`." Not among §1's adopted/overturned decisions. `ResponseMeta.request_id`/`OperationEvent.request_id` are echoed on every response and event (schema 1116, 1131); `docs/Protocol.md` 182 asks bridges to preserve `RequestMeta.request_id`.
- **Impact:** an unrecorded deviation from the controlling model; with concurrent operations sharing a `request_id`, request-id-keyed diagnostics and `GwzBridgeError.request_id` correlation become ambiguous.
- **Required correction:** keep §5's rule (refuse a duplicate `request_id` among live operations with `InvalidRequest`) or record the deviation in §1 with its reason.
- **Closure test:** two live operations with one `request_id`; the second is refused (or, if overturned, the deviation is documented and the test asserts independent completion).
- **Classification:** bounded correction.

### [P3-13] GwzV110Plan S6.3's pool-reuse premise is abolished but the plan is neither cited nor amended
- **Location:** §16 line 359 ("Connections are not reused across operations"); §14 (no mention of the 1.1.0 plan); G9's source is "the 1.1.0 Phase 6 NO-GO … 1.1.0 plan S6.3".
- **Evidence:** `GwzV110Plan.md` 298–301: "S6.3: focused tests … One pool serves two Python operations against a disposable SSH or HTTPS fixture"; line 446: "S6.3 and the Phase 6/7 NO-GO remain open."
- **Impact:** the open release gate specifies a test the contract's design cannot pass; the contract meant to close that NO-GO does not say how S6.3 changes.
- **Required correction:** add S6.3 to §14 with its amended test ("two overlapping Python operations complete independently, each on its own runtime"), or state that the amendment is filed separately.
- **Closure test:** documentary: S6.3 text and the contract agree.
- **Classification:** bounded correction.

## 2. Invariant analysis

Attacks that failed (evidence the invariant held):
- **Every existing service method classified exactly once.** The service declares 37 methods (schema 8–167). Direct: `status`, `ls`, `resolve_forall_targets` (line 66, a real method), `list_snapshots`, `diff`, `log`, `transport_capabilities`, `remote_identity`, `configure_transport_runtime` (9). Log reads: `events.subscribe`, `diff.output`, `log.output` (3, all `role="out", shape="log"`). Result: `operation.result` (1, `role="out"`). "Every other existing method" = the remaining 24 `role="in"` methods. 9+3+1+24 = 37, no overlap.
- **Protocol facts cited.** `AggregateStatus.accepted = 0` (532–533); `ResponseMeta.operation_id` tag 5 optional (1138); `cancelled = 73` (833); `InvalidRequest`, `OperationNotFound`, `OpenOperation`, `InternalError` exist in both model and Taut enum (model 15/39/43/55; wire 1/25/29/41). Frame layout matches taut-shape `framing.py` 7–16 (u32-LE length over tag+body, tag byte, deterministic CBOR); the contract applies the length prefix only to the byte-stream adapter and defines its own tag registry, which it never claims taut-shape supplies. D17/D18's generic `call(method, in)` floor is compatible with `SessionCall{method, request bytes}`.
- **`with_local_transport` description.** Verified at `local_command.rs` 17–31: current-thread executor, `TransportRuntime::with_https`, `runtime.request(meta, operation)`, action, then `request.finish()` + `runtime.shutdown()`; panic unwinding runs `Command::drop` → `finish()` (70–74). Only the cancellation-handle exposure fails (P2-2).
- **`execute_invocation`.** `dispatch.rs` 4–19 confirms the transport path is gated by `transport_meta` and otherwise `Git2Backend::new()`. Note (risk, not finding): `diff`, `log` and the hook commands are dispatched before it (`unreachable!` arms 314–331), so the "shared dispatch" must absorb those paths too.
- **`OperationRuntime`.** `push_event.rs` 283–380: registry keyed by `operation_id`, `submit` spawns a thread and returns `accepted`, `subscribe`, `try_result`, `wait`; no cancel, release or transport integration — proposals §1 and contract §14 are accurate here.
- **Locks.** `MemberLockManager::try_lock(member_id)` (membermutationguard.rs 12–22) and the cross-process `WorkspaceMutatorLock` under `.gwz/locks/workspace-mutator.lock` (workspace_mutator_lock.rs 6, 47–58) match GWZDesign 1573–1585 and contract §5.1 "The existing cross-process workspace mutator lock still applies". The contract's per-workspace-and-member lock is a stated extension (§16 line 361).
- **G1, G4, G5, G10.** README 7–10 (no server or daemon); REQ-011; gwz-py README 13–16 (no shell-out, in-process bridge, remote adapter designed for). The contract adds no daemon; the test host binary is test-only.
- **G2, G7.** §11 mirrors GWZDesign 1624–1632; every waiting `CoreBridge` method is async (`subscribe_events` returns an `AsyncIterator`).
- **Ownership rules O1–O6 versus §5.2/§5.3/§8.** O3's "terminal written once" is honoured by §5.2/§5.3/§8 steps 2–4; O2 is honoured by §8 step 5 and the closure-stands-in rule; O6 is honoured by §9 (only frames cross). The one contradiction is P3-3.
- **§1 limits versus §3/§5.** 8 running / 64 queued / 64 retained / 1024 outstanding / 30 s wait are used consistently in §4.2, §5.1, §5.4, §10 (semaphore 1024). The 30 s matches `_EVENT_WAIT_TIMEOUT_MS = 30_000` (bridge.py 15); the native `TransportSessionFull` message today says "eight live operations" (transport_session.rs 195–199); `OpenOperation` for release of a live record matches `release_inner` 1029–1032.
- **Paired core paragraphs.** The GWZDesign paragraph restates the contract's claims without adding MUSTs (its schema-addition list omits the log-read messages of §13 — minor). The GWZRequirements paragraph's MUSTs match the contract except the one extra MUST (P3-8) and the unbounded-close MUST it shares with the contract (P2-4).
- **gwz-py pointers.** GwzPyDesign 315–320 and GwzPyTransportDesign line 3 accurately describe the draft and keep the existing sections authoritative; proposals §9 line 218 names the draft.
- **§10 mapping versus the bridge.** `call`/`submit`/`subscribe_events`/`operation_result`/`merge_operation_response`/`cancel_operation`/`release_operation`/`diff_log_*`/`log_output_*`/`close` are all covered; the cancel-and-join rule matches `_run_native` 366–383; `close()` returning the same report matches 273–299; `MergeOperationHandle.result` (client.py 1333–1346) tolerates any error code from `operation.response`, so the unspecified failure reply of `operation.response` is not a public-API break (it is covered by P3-4/P3-5's completeness remedies).
- **Remote transport, placement and sequenced-stream designs "unchanged".** GwzRemoteTransportDesign §4.1.1's prohibition on new service methods and length prefixes binds the transport programme, and names the CLI–core communication layer as "supplied elsewhere" — the session contract is that layer, so no conflict. The reserved transport lane (tags 16–31) is unused, so the sequenced-stream design's profile-3 carrier rule is untouched. TransportPlacement.md's local default is unchanged; its cli-placement lifecycle is deferred (OUTCOMES).

## 3. Risks and next action

Residual risks below the finding bar:
- The shared dispatch must also absorb gwz-cli's `diff`/`log`/hook paths (dispatch.rs 314–331) and the process-global diff/log registries of the native extension; §5.2 names only `execute_invocation`.
- §12's "never shipped" test host binary: a `[[bin]]` target in the published `gwz-core` crate is published with the crate source; name the mechanism (an `examples/` or dev-only target).
- Workspace-root resolution before admission (§5.1) performs filesystem discovery on the host thread, against proposals §5's "never blocks" spirit; bounded, but unstated.
- Reads wait for a running slot (§5.1), so eight long network operations delay `status`; a reserved read slot would remove the starvation without changing the contract's shape.
- With one runtime per operation the placement guide's step-1 premise ("Keep the returned runtime owner alive across operations to reuse connections") will need amendment when client placement is scheduled.
- Unstated documentation impacts once accepted: gwz-core `docs/MessageCatalog.md` (four new methods), `docs/ErrorCatalog.md` and `docs/Protocol.md` (codes, §Transport), `docs/Reference.md` line 72 (`OperationRuntime` guidance), gwz-cli `docs/MachineOutput.md` (unchanged output, changed path), gwz-py `RELEASE.md`/release gates.

Next action: the lane owner returns the object for one bounded revision resolving P2-1 through P2-4 (schema append plus conversion fix; cancellation-handle exposure; error-frame body; close bound or recorded deviation), folding in the P3 text corrections, then re-submits the same tuple shape for re-verdict. I pre-commit to GO on a revision that resolves P2-1, P2-2, P2-3 and P2-4 as specified.
