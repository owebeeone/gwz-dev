# Step 2.2b operation service (gwz-core 08ce3de2): Code-axis review

**Review object:** gwz-core `9a9a659e..08ce3de273` (41 files, +7037/−33: 26 tracked files changed by +1488/−33, plus 15 new files of 5,549 lines), and evidence run `gwz-core-evidence 17789b74d0` `campaigns/transport-qualification/runs/2026-10-25-p2-2b`.
**Baseline:** gwz-core `9a9a659e`. Lane `/Volumes/projects/limbo/gwz-dev-p2-opsvc` (root `a32ae33f60`).
**Date:** 2026-10-25.
**Axis:** Code (architecture, interfaces, call graphs, wire and compatibility reality).
**Verdict:** **NO-GO.** P0 0, P1 0, P2 4, P3 7. All four P2 findings have bounded remedies, and I commit in advance to GO once they close as §3 states.

## 0. Evidence base

- **Tuple at start and end (identical):**
  - Lane HEADs: gwz-core `08ce3de273`, gwz-core-evidence `17789b74d0`, gwz-transport `22b53dce5d`.
  - Other members: gwz-git `e3bc2a41f6`, gwz-py `22e4f133a4`, gwz-cli `9ecc2b7c1b`, gwz-credential `6d78c593a4`, gwz-sspi `a2978a8f3f`, taut `913e0dc35c`, taut-shape `89916c4d01`, taut-shape-py `da79bfb743`, taut-shape-rs `959f0f048a`, git2-rs `d13951f7e0`, libgit2 `b172e3d187`.
  - The only dirty state is pre-existing and not mine: the inherited `2026-09-22-alpha-setup-timeout/` directory, and the root's staged gwz.conf markers and untracked dev-docs.
- **Read:**
  - The full diff of the crate (`service.rs`, `service/{table,workers,events}.rs`, `lane.rs`, `detached.rs`, `lib.rs`, README, Cargo.toml).
  - Core `served.rs`, `served/{dispatch,methods,spool,lane}.rs`, `served/lane/tests.rs` and the red-row tests in `served/tests.rs`.
  - The schema diff, the `check_log_additive.py` diff, `convert.rs`, `error_code.rs`, `ErrorCatalog.md`, `GWZRequirements.md:116`, `GWZDesign.md:108` and the switch inventory.
  - Controlling text: 2.0 §1, §2, §3.1–§3.4, §4.5 and §8; session design §3, §4.1–§4.3, §5.1–§5.6, §6–§8, §10 and §13; crate map §1–§3; plan 2.1, 2.2, 2.2b, 2.2c and 2.3; the lane brief.
  - For convergence only, read-only: lane p2-carrier gwz-core `d8e7478d` plus its working tree (`src/transport_host/carrier/stream.rs` module doc, `wire.rs:1–80`), and lane p2-unpark gwz-core `df31c221` (`protocol/gwz.taut.py`).
- **Commands run:**

  | Command | Load | Result |
  | --- | --- | --- |
  | `python3.13 protocol/regen.py --check` | — | OK, artifacts current |
  | `protocol/.regen-venv/bin/python protocol/check_log_additive.py protocol/gwz.taut.py` | — | OK, `sha256:4d377a49…` reproduced |
  | `cargo test -p gwz-session-host` (warm ordinary dir) | ~32 | 54 + 1 + 4 passed |
  | `TAUT_PYTHON=python3.13 cargo test --test protocol` (warm) | ~25 | 48 passed |
  | **Decisive:** `CARGO_TARGET_DIR=…/review-p2-2b-both RUSTFLAGS='--cfg gwz_transport_candidate --cfg gwz_session_candidate' cargo check --lib` (cold) | 6.5 | **exit 101**: `src/transport_host/cli_placement.rs:125:20: error[E0063]: missing field 'cancellation' in initializer of 'protocol::generated::TransportCapabilitiesResponse'`. This was the only error; `served/lane.rs` type-checks. Directory deleted afterwards; log copied to `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/review-2b-both-check.log`. |
  | A Python probe of `check_appended_codes` (module loaded with `-I`, nothing written) | — | See P3-3 |

- **Evidence run:**
  - README, `raw/mac/*` (red1 is a compile failure of `tests/protocol.rs`; there is a red on the old schema for the additive check; mutation reds exist for rows 4 and 5 only), and `raw/remote/final/summary.txt` (OVERALL PASS).
  - Receipts a FAIL and b PASS. Both built only the ordinary and transport shapes.
  - Hosted CI: `.github/workflows/transport-candidate.yml:35–39` builds a `transport-and-session` leg with both switches. `.github/workflows/checked-artifact-boundary.yml:217–227` runs `cargo test -p`, `--doc` and `clippy --all-targets -D warnings` for every present library, including `gwz-session-host`.

## Problem, requirement provenance, and scope

- **Requested outcome** (plan 2.2b, with 2.0 §3.2 and §3.3 and session design §4–§5 as adopted):
  - an operation service over `session-contract` frames;
  - the §13 schema additions and codes 77–86 at the schema's tail, with a collision check;
  - records, admission, direct workers, classes N and W, limits, cancel, events, results, release and close;
  - core-side dispatch;
  - the five red rows.
- **Component authorization:** gwz-core and the evidence member only. The brief does not allow edits to `session-channel`'s byte stream, `session-contract`'s `FrameSink`/`FrameRouter`, `cli_placement.rs`/`placement_endpoint.rs` (2.2) or `gwz-connection::remote` (2.2c). The diff respects those files. However, the brief asked 2.2b to code *against* the §3.3 shapes using "a local fake router in your tests". 2.2b instead ships a production router (P2-2).
- **Scope-changing constraints:** I found none that lacks provenance. Plan 2.2b's text ("the transport lane … for core-to-client envelopes during a call") does overlap 2.1's ("framing … so a saturated data queue never starves Window, Cancel or terminal frames") on who routes tags 16 and 17. That plan-level ambiguity is the origin of P2-2.
- **The implementer's reported limitations, assessed:**

  | Limitation | Assessment |
  | --- | --- |
  | `served/lane.rs` never compiled | Its own code compiles. The shape it belongs to does not compile because of a field this step added: **defect P2-1**, not a limitation. |
  | §5.1 member lock manager not built | Recorded, but adopted by 2.0 §3.2 and named in 2.0 §1. It needs an owning step: **P3-7**. |
  | `cargo test -p gwz-session-host` run by no gate | Inaccurate. Hosted `checked-artifact-boundary` runs it, plus doctests and `clippy --all-targets -D warnings`. It is the local and remote lane matrix that skips it. Risk only: crate clippy was not verified (§3). |
  | `allow(dead_code)` on `serve_in_process`, `ServedChannel`, `CoreDispatch::new`, `ServedSession.lanes`, `lane::envelope` | Each has a reason naming 2.3 or 2.2c. Acceptable under the release-scoped rule. |
  | `cancellation` never true | Consistent: `transport_capabilities::handle` uses `..Default::default()` and leaves it `None`. Recorded limitation. Related to P3-4. |
  | `diff.output`/`log.output` reads are probes | Wire-visible deviation from §4.2, which 2.0 §3.2 adopts "unchanged". Recorded only in the evidence README. It needs an acceptance line in the plan or design (folded into §3, not a separate ID). |
  | Cancel of a spent read, result or control call answers `operation_expired`, not `invalid_request` | Same class as the previous row: a recorded deviation that needs an acceptance line. |
  | Handshake messages out of schema | Scope: 2.3. |
  | Size ~2,675 + ~1,190 production lines vs ~650 planned; `tests/protocol.rs` 2,183 lines | Operator structural decisions (deferred). |
  | gwz-py IR drift | Out of scope; observation. |

## Convergence with 2.1 and 2.2 (who owns what; duplicates; the port plan)

**Who implements what today, across lanes:**

| Rule (2.0 §3.3) | 2.2b (this object) | 2.1 (p2-carrier, gwz-core `src/transport_host/carrier/stream*`, `wire.rs`) |
| --- | --- | --- |
| Tag 16/17 by kind | `served/lane.rs:17–22` `control()`; crate `lane.rs:29–42` | `wire.rs` `tag_of(Lane::of(kind))`, `Tag::TransportData/Control` |
| Kind under the wrong tag faults its binding | `lane.rs:203–207` | `stream.rs` module doc :29 |
| `data` bounded by the receive window | `lane.rs:230–238` | `BindingLimits` (:25–27) |
| `window` coalesced to the highest offset | `lane.rs:239–252` | :26 |
| One slot per other kind | `lane.rs:253–263` (per kind per stream) | "a few slots per stream" (:27) |
| Fault confined to the binding | `lane.rs:211–214`, **per call** (`request_id`, `:140`, `:153`) | **per binding** (`open_binding`) |
| Frame for no live target is discarded, no state | `lane.rs:196–202` (unknown `session_id` under a live `request_id` *creates* state at `:231`) | "A frame for a binding that is not open is discarded and creates no state" |
| Undecodable body that names a binding | Ends the session (`served/lane.rs:56–57` → `lane.rs:381`) | Faults that binding only |
| Writer key | `ReplySink { target: u64 \| &StreamTarget, control: bool }` (`service.rs:463–519`) | `FrameKey`/`Target` in session-contract |
| Body encoding | Taut `TransportLaneFrame {1: request_id, 2: Envelope}` (2.0 §3.3's frozen form) | One-byte name length + name + gwz-transport CBOR envelope (`wire.rs:11–12`) |

- **Ownership.** 2.0 §3.3 "Reader interface" puts every per-class and per-binding state in core's `FrameRouter`, which 2.1 implements with a binding concept. The brief told 2.2b to use only a test fake. Therefore 2.1 owns lane routing and per-binding state, and 2.2b's `Lanes`/`route_frame`/`CallLane` admission are a second live implementation (P2-2).
- **Body encoding (observation, outside my scope to judge 2.1).** 2.1's encoding differs from 2.0's frozen `TransportLaneFrame`. One of the two must change, and the change must be traced to 2.0 or amended there.
- **Port plan:**
  1. 2.2 lands first.
  2. Re-apply 2.2b's schema delta to 2.2's composed `gwz.taut.py`: the session messages, methods, codes 77–86 and `cancellation` (8), with `TransportLaneFrame` now *ordinary*, because `Envelope` is composed directly.
  3. Give `check_log_additive` its own strip block.
  4. Delete the candidate overlay.
  5. Add `cancellation` to `cli_placement.rs`'s literal. It becomes ordinary in 2.2, so without this the merge breaks the ordinary build (P2-1).
  6. When 2.1 merges: replace `ReplySink` (and its blanket impl, `service.rs:495–519`, which drops key and class) with `FrameSink::send(frame, lane, FrameKey)`. Keep the close report ordinary-keyed. Make `Service::receive` the registry-class consumer behind 2.1's router. Delete the crate's `Lanes` admission state and `route_frame`, or 2.1's, by one lane-owner decision.

## 1. Findings

### P2-1: The both-switch shape (hosted CI's `transport-and-session` leg) does not compile

- **Location:** `protocol/gwz.taut.py:1165` adds `cancellation`. The exhaustive literal at `src/transport_host/cli_placement.rs:125` (under `gwz_transport_candidate, gwz_session_candidate`) was not updated.
- **Invariant:** plan amendment 2 §3.13 rule (a): both candidate shapes stay green. Lane rule: red-first plus green on the gate shapes.
- **Reproduction:** `RUSTFLAGS='--cfg gwz_transport_candidate --cfg gwz_session_candidate' cargo check --lib` in gwz-core gives E0063, missing field `cancellation`.
- **Impact:**
  - Pushing 08ce3de2 turns `Transport candidate / transport-and-session` red.
  - After 2.2 makes `cli_placement.rs` ordinary, the same omission breaks the *ordinary* build at the port.
  - The limitation "never compiled" hid a concrete break. The lane matrix and both receipts only build the ordinary and transport shapes.
  - The fix lies in a file the brief assigns to 2.2: an ownership conflict the lane owner must settle.
- **Correction:** add `cancellation: None` at `cli_placement.rs:125`, either with the lane owner's authorization in 2.2b or in the port onto 2.2.
- **Closure test:** both-switch `cargo check --lib --profile test` exits 0, and the hosted candidate leg is green on the ported commit.

### P2-2 (ARCHITECTURAL): a second production implementation of 2.0 §3.3's core-side transport-lane routing, diverging from 2.1's

- **Location:** `crates/session-host/src/lane.rs:135–386` (`Lanes`, `admit`, `CallLane`, `route_frame`, `TRANSPORT_*`, `lane_tag`, all exported at `lib.rs:47–50`); `service.rs:463–519` (`ReplySink` as a parallel `FrameKey`); `src/session_host/served/lane.rs:17–80`. Duplicates 2.1's `transport_host/carrier/stream/{inbound,state}.rs` and `wire.rs`.
- **Invariant:**
  - one live design, no duplicates (R9);
  - 2.0 §3.3 "Reader interface" and "Fault scope" (faults confined to the *binding*; a frame for a binding that is not open creates no state);
  - the brief's instruction to use a local fake router in tests.
- **Reproduction:** the table in the convergence section. In 2.2b, two bindings under one `request_id` share one `faulted` flag (`lane.rs:153`, `:211–214`). A wrong-tag frame on binding A faults binding B. An envelope that decodes under the call but fails `Envelope` decode ends the whole session (`served/lane.rs:56–57`). An unknown `session_id` is queued (`lane.rs:231`). A zero-payload `data` frame is never charged against the window (`:230–238`), so its queue is unbounded.
- **Impact:** after the merges, the composition has two routers with different fault granularity and body encodings. Whichever 2.3 wires, the other's rules and tests are dead or contradictory. Red rows 1 and 4 are proven against the router that does not own bindings.
- **Correction:** the lane owner records one owner, 2.1's carrier by 2.0 §3.3 and the brief. 2.2b keeps `Service::receive` and the call registry hand-off, deletes `Lanes` admission state and `route_frame`'s lane branch at the port, and re-points rows 1 and 4 at 2.1's router. Record the body-encoding choice against 2.0 §3.3.
- **Closure test:** after the port, a source check that exactly one type implements per-binding transport state. Rows 1 and 4 pass through 2.1's `FrameRouter`, with two bindings under one call where faulting one leaves the other delivering.

### P2-3: The close report and channel closure can overtake a terminal reply that settled before them

- **Location:** `crates/session-host/src/service/workers.rs:83–92`. `settle` runs under the table lock, then `inner.flush` runs *after* unlock on the worker thread. Against that, `run_close` at `:236–301` wakes on `settled`, and steps 5–8 send the close report and `close_channel()` without waiting for in-flight flushes. The same unlocked-flush pattern is at `table.rs:788–792` (admission) and `service.rs:676–679` (reader).
- **Invariant:** 2.0 §3.3 "Close report": it is written only after every frame the host queued before it, and step 9 closes after drain. Session design §8 steps 6–9. This is the class Safety S-P2-3 closed in 2.0.
- **Reproduction:** in the crate harness (`service/tests.rs`), use a test `Sink` whose `send_reply` sleeps about 200 ms for the unary call's own reply. Run a unary `fetch` that completes while `session.close` is waiting in step 4. The sink records the close report and `closed` before the fetch's reply. With the in-process end, that reply returns `SendError::Closed` and is lost.
- **Impact:** a completed unary call (a push, for example) loses its reply. Its record was already discarded (`table.rs:745–748`), so it cannot be recovered. The client fails the call with a closed-session error although the call succeeded. Rare in timing, real under load.
- **Correction:** make sends ordered with settlement. Either send under the table lock (sinks never block), or count unflushed outgoing under the lock and have step 8 wait for zero before the report and step 9.
- **Closure test:** the delayed-sink row above. The fetch reply precedes the close report, and `closed` comes last.

### P2-4: The detached-worker record outlives the detached worker for a submitted operation

- **Location:** `workers.rs:41` stores `record.alive = Some(alive.clone())`. Nothing clears it: `settle` (`table.rs:667–755`) and every other site leave it. `detached.rs:57–69` treats the worker as alive while any strong reference exists. A submitted and accepted record is retained after close step 5 (`table.rs:701–702`, `:750`).
- **Invariant:** `detached.rs:8` and `:57` ("until the worker's thread ends"); 2.0 §1 and §3.2 (refuse writes and pushes only *beside* a detached worker).
- **Reproduction:** change `close_settles_cancels_detaches_and_reports_and_the_detached_record_refuses_writes` (`service/tests.rs:731`) so the stubborn `commit` is submitted (`submit = true`). After `release`, `detached.running_on("/ws")` stays true while `host` is alive, and the final loop times out.
- **Impact:** every session of the host refuses W and push on the served workspace (with `unsupported_operation`) after the worker has ended. This lasts until the closed session's `Service` is dropped. `ServedChannel` bundles that `Service` with the client end, so holding the client end holds the refusal. Recovery needs a server restart.
- **Correction:** clear `record.alive` in `settle`, after step 5 has recorded it, or hold only the worker's own clone.
- **Closure test:** the submit variant of the row above passes.

### P3-1: The 64 open-log limit (§5.4) is not enforced

- **Location:** `limits.rs:50`/`:76` `open_logs` is never read by the service or by `served`. `dispatch.rs:109–127` registers without a bound.
- **Invariant:** §5.4: at most 64 open logs, release the oldest sealed log, otherwise `transport_session_full`.
- **Impact:** `docs/ErrorCatalog.md:36` documents "the open logs" under 77 for a refusal that is never produced. A session can accumulate unbounded spools until it ends.
- **Correction:** enforce the limit at registration, or record it as a deviation and remove the catalog clause.
- **Closure test:** the 65th `diff` with no releasable log gets 77.

### P3-2: Merge delivery is marked by either view

- **Location:** `table.rs:518` and `:698–700` set `delivered` on any view.
- **Invariant:** §5.4: a merge is delivered only once both its result *and* its response have been delivered.
- **Impact:** a merge whose result was read can be evicted before `operation.response`, which then answers 78.
- **Closure test:** fill the table, read only the merge's result, admit another operation, and check the merge record survives.

### P3-3: The tail check refuses every future code append

- **Location:** `protocol/check_log_additive.py:266–267` computes the expected tail from `max(earlier)`.
- **Reproduction:** the probe that adds `some_future_code=87` gets "session codes must be the consecutive tail after 87: expected [88…97]".
- **Impact:** the next appending step meets a misleading refusal that tells it to renumber published codes.
- **Correction:** anchor on `capacity` (76), with 77–86 consecutive.
- **Closure test:** the probe passes with 87 added, and still refuses a taken 77.

### P3-4: Production dispatch never gives the handler its token, so red row 2 is proven only by a scripted handler

- **Location:** `served/dispatch.rs:77–197` passes no token. `served.rs:240` discards gate refusals. `served/tests.rs:46–80` replaces `fetch` with a script that polls the token.
- **Invariant:** session design §5.2: "The worker also passes the token to the handler's context."
- **Impact:** with `CoreDispatch`, a cancel of any running handler is answered only after the handler ends naturally. "Cancel reaches the handler" holds for no production handler.
- **Correction:** pass `worker.token()` into the handler context, or record the row as plumbing-only, with the owning step (2.2c or 2.3).
- **Closure test:** a `CoreDispatch` handler observes cancellation at a gated step.

### P3-5: Red rows 1–3 have no red evidence, and row 1 is not over the in-process carrier

- **Location:** evidence `raw/mac/red1-protocol.log` is a schema compile failure. Mutation reds exist only for rows 4 and 5. Row 1 (`service/tests.rs:869`) uses a fake sink and string bodies, not `Envelope`, and not the in-process carrier the plan names.
- **Invariant:** the common brief's "TDD, red first, logs kept"; plan 2.2b's row 1 wording.
- **Correction:** add mutation reds for rows 1–3 (for example, delay the lane frame until after the reply; stop cancelling the token; stop appending events). Record row 1's dependence on 2.1.

### P3-6: Schema comments say "absent" where the decoder requires the key

- **Location:** `gwz.taut.py:2713` ("Absent means unary") and `:2716` ("absent means read"), for fields with `optional=True`. `generated.rs` `SessionCall` decodes with `c.try_get(3)?` and `c.try_get(5)?`.
- **Impact:** a client that follows the comment and omits the key gets a protocol error that ends the session.
- **Correction:** say "null means …", or use `MISSING_OK` with a design note.
- **Closure test:** the catalog text matches the decoder, or a `SessionCall` without key 3 decodes.

### P3-7: The adopted §5.1 member lock manager has no owning step

- **Location:** `context.rs` holds only `DetachedWorkers`. 2.0 §1 and §3.2 adopt the member lock manager in the host context.
- **Impact:** class N admits fetch beside fetch or push on the one served workspace (`table.rs:72`), so two calls in one session can update one member's refs at once and fail spuriously on Git's lock files. That is new concurrency for this workspace, compared with one CLI process.
- **Correction:** assign the build to a named step before 2.3 exposes sessions, or build it now.

## 2. Invariant analysis (attacks that failed, with evidence)

- **Codes 77–86:** in 2.0 §1's name order after `capacity=76` (`gwz.taut.py:875–915`). The collision check refuses a taken number (probe) and a missing or misplaced slot. Stripping exactly the session surface reproduces the pin `4d377a49…` (run).
- **§13 shapes:** match the design: `SessionCall`/`SessionReply`/`SessionError`, the request messages, `CleanupReport`, `OperationReleaseResponse`, and `cancellation` (8, `MISSING_OK`, encoded as key 8 null, which `tests/protocol.rs:333–346` pins). `u64`/`u32` map to taut `INT`, with `call_id < 1` refused (`served.rs:251`).
- **`convert.rs` mapping:** 77 and 78 are not a behaviour change for any existing caller. No producer of model `TransportSessionFull`/`OperationExpired` exists outside `served` in gwz-core, gwz-py or gwz-cli (grep). taut decoders keep unknown fields (`taut/docs/CodecContract.md` §6).
- **Crate boundary:** `gwz-session-host` depends only on `gwz-session-contract`, decodes no body (core's `HostPorts`/`decode_lane`), and keeps no statics or thread-locals.
- **Class table:** fetch beside fetch and push; at most one push; a write beside nothing but direct calls (`table.rs:70–76`); unknown methods are W (`methods.rs:47–53`); `remote_identity` is W; FIFO head-of-line per workspace (`table.rs:929–932`).
- **Limits:** 8/64/128/1024/64 (`limits.rs:71–78`). Eviction takes the oldest delivered retained record (`table.rs:235–247`). Control calls are outside the 1024 limit.
- **Cancel, records and admission:**
  - Records and tokens are created at receipt under the reader's lock (`table.rs:381–426`).
  - Admission runs on one thread in receipt order, and a cancelled Received call settles `Cancelled` before resolution or after it (`table.rs:774`, `:810–821`).
  - An unseen target gets `operation_not_found` (`:559`).
  - A queued or waiting call settles at once under the table lock (`:600–609`).
- **`session.close`:** step 1 refusal "session closing" (`table.rs:288–294`); the report is ordinary-keyed on the control lane (`table.rs:366–377`); repeated closes get the same report (`:349–351`).
- **`configure_transport_runtime`:** it does not mutate other sessions. Once a backend exists, `transport_support.rs:49–52` refuses a different value.
- **Resolution:** refuses another root and the four creating methods with `invalid_request` naming the served root (`served.rs:285–299`).
- **Detached-record refusal code:** matches the existing try-lock's (`unsupported_operation`, `workspace_mutator_lock.rs:19`).
- **Code rules:** `cfg_if!` only; braced control flow; test modules gated with `#![cfg(test)]` as in the crate's precedent; nothing under `tests/` is production code; `table.rs` is 944 lines, under the 1000 split review.
- **`served/lane.rs`:** type-checks in the both-switch shape. Its tests were not compiled.

## 3. Risks and next action

- **Precommitted GO:**
  1. P2-1 fixed, with the both-switch check green.
  2. P2-3 and P2-4 fixed, with their closure rows.
  3. P2-2 closed by a recorded lane-owner ownership decision plus the deletion-and-re-point items written into the port plan (the code may land in the reviewed port).
  4. The P3s fixed, or recorded with owners.
  5. The two recorded §4.2 deviations (probe reads; spent cancel → 78) accepted in writing as amendments.
- **Unverified risk:** hosted `checked-artifact-boundary` runs `cargo clippy -p gwz-session-host --all-targets -D warnings` and `cargo test -p gwz-session-host --doc`, which no local gate ran. Run them before any push.
- **2.3 seam:** `route_frame` refuses every frame before `open()`. 2.3 must call `Service::open()` *before* writing `SessionOpened`, or a client's first call races into a protocol error. The doc at `service.rs:649` ("has answered") invites the wrong order.
- **Observations outside scope:** 2.1's tag-16/17 body encoding is not 2.0 §3.3's `TransportLaneFrame`. gwz-py's packaged IR drifts. The lane gate matrix omits the both-switch shape that CI requires.
