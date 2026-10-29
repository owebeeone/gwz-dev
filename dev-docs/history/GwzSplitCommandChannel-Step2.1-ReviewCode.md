# Step 2.1 carrier (gwz-core d8e7478d) — CODE-AXIS REVIEW

**Review object:** gwz-core `0ed48168..d8e7478d81` (step 2.1: `src/transport_host/carrier/`, `crates/session-contract`, `crates/session-channel`, `session_host/context.rs`, `scripts/checks/check_carrier_peer_rule.py` and its test, `windows_parity/2.1.json`, the `run_tests.py`, workflow and `check_checked_artifact_boundaries.py` wiring, and the `#[path]` declaration in `session/local_link.rs`), plus the evidence run `gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-25-p2-1/` with `mod-rs-trunk-patch.diff`.
**Baseline:** gwz-core `0ed48168`. Controlling text: `GwzSplitCommandChannel.md` revision 3 (accepted).
**Date:** 2026-10-25. **Axis:** Code.
**Verdict:** **NO-GO.** P0 0, P1 0, P2 5, P3 4. I commit in advance to GO once P2-1 to P2-5 are closed by the corrections and closure tests stated below. Each one has a bounded remedy and none needs a dependency or scope change.

## 0. Evidence base

- **Tuple at the start (06:39):**
  - gwz-core `d8e7478d81d3…`, clean.
  - gwz-core-evidence `0293b6d18bdf…`. Its only untracked entry is the inherited `2026-09-22-alpha-setup-timeout/`.
  - Lane root `d6bd7f155c`.
  - The other members: bazel-gwz-dev `1f2bded38e`, git2-rs `d13951f7e0`, gwz-cli `cea7bd4cab`, gwz-credential `6d78c593a4`, gwz-git `a9d7ee09cc`, gwz-py `71d2d07249`, gwz-sspi `a2978a8f3f`, libgit2 `b172e3d187`, taut-shape `89916c4d01`, taut-shape-py `da79bfb743`, taut-shape-rs `959f0f048a`, taut `913e0dc35c`, gwz-transport `8e43e6ddde`.
- **Tuple at the end (06:51):**
  - gwz-core and gwz-core-evidence are unchanged.
  - **gwz-transport moved** to `70516c0e48` at 06:40:29 (reflog: "gwz fast-forward", made by the lane owner; I made no change). The move touches only `crates/gwz-connection`, and `git diff --quiet 8e43e6dd 70516c0e -- src/` is empty. So every gwz-transport file this report cites (`binding.rs`, `mux/mod.rs`, `mux/routing.rs`, `stream/`, `codec.rs`, `codec/validate.rs`, `sequenced.rs`) is byte-identical at both commits.
  - The object tuple did not move, so I did not stop.
  - Every other member is unchanged.
- **Read in full:**
  - The commit stat.
  - In carrier: `carrier/{mod,binding,in_process,link,queue,stream,wire}.rs` and `carrier/local/{mod,peer,unix,windows}.rs`.
  - In session-channel: `byte_stream.rs`, `write_queue.rs` and `test_router.rs`, plus the diffs of `in_process.rs` and `lib.rs`.
  - In session-contract: the diffs of `channel.rs`, `frame.rs`, `lib.rs` and the suite.
  - The check script and the wiring diffs.
  - Test rows: `stream_rows.rs` and the relevant parts of `local_rows.rs`; the test names of `windows_rows.rs`, `write_queue_tests.rs` and `priority_tests.rs`.
  - The evidence README and the trunk patch.
  - Controlling text: 2.0 §1 and §3.1–§4.3; parked server design §3 (schema) and §4; contract §6; plan §4 step 2.1, §10 and §12.
- **Commands run:**
  - `python3.13 scripts/checks/check_carrier_peer_rule.py` printed `ok (11 files)`; `python3.13 -m unittest scripts/checks/test_check_carrier_peer_rule.py` ran 10 tests, OK.
  - A bypass probe of the source check, run on a scratch copy of `carrier/` (P3-2).
  - A `patch --dry-run` of the trunk patch against copies of the trunk's files, with default fuzz and with `-F0`.
- **Cargo was not run.** The 1-minute load was 98, 68, 69 and 56 at the four checks, all above the limit of 40. No `review-p2-1-code` target directory was created, so there is none to delete. The behavioural claims below rest on the code read, quoted with file:line, and on gwz-transport's own rules.

## Problem, requirement provenance, and scope

- **Requested outcome (plan 2.1):**
  - `LocalLink`'s pump behind a `Carrier` seam, with the in-process carrier as the default.
  - A same-user local socket and named pipe (D-D), with peer checks in both directions.
  - `session-channel` framing with D-N: a priority writer, a non-blocking reader, per-class and per-binding bounds, and binding-only faults.
  - A `SessionOpen` of the `cli` placement that carries no environment.
  - A source check that no test-only override of the peer rule exists.
  - Red rows on three platforms.
- **Authorization:** the change set touches only gwz-core and gwz-core-evidence. Each edit outside the step's named files is necessary, not an expansion:
  - `session_host/context.rs`: the `FrameSink::send` signature change breaks it otherwise.
  - `run_tests.py` and the workflow: they wire in the required source check.
  - `check_checked_artifact_boundaries.py`: it is forced by the `#[path]` edge.
  - The `#[path]` declaration: `transport_host/mod.rs` is the trunk's file. This is temporary, and the trunk patch removes it.
- **Not delivered against the plan's row text (observations):**
  - The plan's red rows name "the CLI client and `SocketCoreBridge`". Neither exists in 2.1: the CLI's `--server` is 2.3, and gwz-py is outside this step's components. The rows exist at the carrier level only.
  - 2.0 §3.4's walkthrough attributes `connect_verified` (the address walk, the listener check and `SessionHello`) to 2.1. Only the listener check is here.
- **Requirement provenance of what was left out (follow-ups, not findings, per the brief):**
  - Process-id pinning and the sandbox rule are not required by 2.1's rows.
  - They are release requirements: 2.0 §4.3 says the "sandbox rule and listener verification apply in both directions, unchanged", and plan §12 says the "server design §4 rules [are] normative" at 2.1.
  - The sandbox rule covers Linux seccomp and namespaces, macOS `sandbox_check`, and Windows integrity "below medium" as an absolute refusal. Pinning covers `SO_PEERPIDFD`, `LOCAL_PEERTOKEN`, and the creation time of a Windows process handle.
  - The lane owner must name the step that owns them.
- **Size (observation):**
  - The plan estimated about 800 lines (350 + 450). The commit is +7,250 / −388.
  - Breakdown:

    | Area | Added | Removed |
    | --- | --- | --- |
    | carrier production | 3,344 | 0 |
    | carrier tests | 1,953 | 0 |
    | session-channel production | 400 | 125 |
    | session-channel tests | 721 | 101 |
    | session-contract production | 123 | 27 |
    | session-contract tests | 68 | 32 |
    | scripts | 602 | 0 |
    | other | 39 | 103 |

  - `stream.rs` is 993 lines, 7 under the review threshold.
- **Structural (for the operator's decision, not a finding):** the carrier is a 3,300-line module of gwz-core rather than a small crate that builds and tests alone. The `#[path]` workaround and the trunk patch exist only because of that placement.

## Deviations from the frozen 2.0 text: disposition per item (code moves, text moves, or defect)

1. **`FrameRouter::ended(Closed)`.** The text moves (2.0 §3.3 "Reader interface"; the crate map's `FrameRouter` row). It is necessary: an end that implements `FrameSink` only has no other way to report closure. The contract is met:
   - It is called once (`byte_stream.rs:194-211`, first reason only).
   - It comes after the last `route`: the delivery fence at `byte_stream.rs:257-260` and `206`.
   - No defect.
2. **Provisional lane body** (`[len][request name][transport CBOR]`, `wire.rs:53-119`). The code moves in 2.2b. For now this is convergence risk, not a defect. When 2.2b adopts `TransportLaneFrame` it must replace:
   - `encode_attachment` and `decode_attachment_any_class`;
   - `MAX_TRANSPORT_FRAME` (`wire.rs:30`), which counts the provisional overhead (1 + 128 + 1) and not a taut map's;
   - `is_close`;
   - the rows in `wire_rows.rs`.

   Until then no peer built to the frozen text, gwz-py's bridge included, can interoperate with this carrier. `key_of`'s inputs are unchanged by the move.
3. **Close announcement as a tag-17 frame with a zero-length name.** A defect at P3, recorded as P3-3. The text says tag 17 carries "exactly" the transport kinds. Either the text moves (it defines the carrier announcement) or 2.2b removes the announcement.
4. **Four slots per kind instead of one.** No defect. The stated reason is wrong: a compliant sender has at most one `flush` outstanding per stream. gwz-transport's sender keeps one pending flush (`stream/machine.rs:114-134`) and its receiver refuses a second while one is pending (`stream/incoming.rs:69-80`); `flushed` answers one barrier at a time. Four slots is a looser bound that the sender still controls. Either the text records "4" or the code returns to 1; either is acceptable.
5. **`byte_stream` returns `io::Result`.** The text moves, because the reader thread's spawn can fail. No defect.
6. **The `SessionOpen` validator drops keys 5 and 6.** A defect: the code moves. This is P2-3.

## 1. Findings

**P2-1 — ARCHITECTURAL. Inbound delivery is one FIFO across every class and binding, so a consumer that is not ready holds back every binding and every call.**
- **Root cause:** `StreamCarrier` keeps per-class and per-binding *accounting* but has one *queue* (`inn: BTreeMap<seq, Entry>`, `stream.rs:129`). Every take pops its head: `Core::take` → `state.inn.pop_first()` (`stream.rs:608`), reached from `try_recv` (`:964-973`), `recv_timeout` (`:336`) and `recv_frame_timeout`. The last of these even returns transport frames "in its place" (`:324-333`). There is no per-class or per-binding take, and there is one waker per end (`:975-977`).
- **Violated invariant:**
  - 2.0 §3.3 "Byte-stream reader (D-N)": the reader "routes every frame to a per-class queue".
  - 2.0 §3.3 "Transport-class bound": "a consumer that is not ready holds back that binding only, never the reader".
  - 2.0 §3.2: "control frames and reads never wait".
- **Reproduction (state sequence):**
  1. Open bindings A and B.
  2. The peer sends A's data within credit, then a `window` for B, then a tag-1 `operation.cancel` for A's operation.
  3. A's mux is full (`WouldBlock`) because A's consumer (Git) is not reading.
  4. To reach B's window or the cancel, the composition must first take A's frames. Taking them releases their charge (`release`, `:475-501`), so the carrier's per-stream bound no longer covers them. The composition must then keep them in a buffer that nothing bounds.
  5. If it does not take them, B and the cancel wait behind A. The cancel that would unblock A never arrives, until carrier loss (2.0 §4.3, 60 s).
- **Impact:**
  - The binding-only fault scope and "a consumer that is not ready holds back that binding only" cannot be met through this seam.
  - 2.2b and 2.2c would have to re-implement the per-binding bound outside the carrier, or accept head-of-line blocking that can deadlock a cancel.
  - The existing rows always drain everything (`the_reader_does_not_wait_for_a_consumer` and the overrun row at `stream_rows.rs:479`), so they do not show it.
- **Correction:** keep `inn` as the arrival record, and add takes by class and by binding:
  - `recv_registry` (tags 1–8);
  - `try_recv_binding(id)` and `recv_binding_timeout(id, …)`;
  - per-binding wakers.

  A frame's charge must stay in the carrier until its own consumer takes it. The global `try_recv` stays for the in-process carrier.
- **Closure test:**
  1. Binding A has its whole receive window queued and untaken.
  2. A tag-1 frame and one frame of binding B arrive after it.
  3. Both are taken while A's frames stay queued, A's `data_bytes` is unchanged, and no fault is raised.
  4. A's frames then drain in order.

**P2-2. A malformed envelope for a live binding ends the whole session instead of faulting that binding.**
- **Root cause:** `route_transport` decodes the full envelope before it knows the binding. Any `codec::decode` error goes to `session_fault(…)`, which aborts the carrier (`stream.rs:676-682`; `wire.rs:117`). `decode` runs `validate::envelope` (gwz-transport `codec.rs:71-79`). So semantic invalidity, an unknown `MessageKind`, a kind whose body does not match, or a `message_seq` that breaks the rule (`codec/validate.rs:66-86`) each end the session.
- **Supporting evidence:** the binding fault for `stream_id < 0` (`stream.rs:776-781`) is unreachable. `validate` already rejects any kind above 3 whose `stream_id` is ≤ 0, and any kind of 3 or less whose `stream_id` is non-zero. Only the wrong-tag case faults a binding.
- **Violated invariant:**
  - Contract §6: "Current-binding invalid messages fault that binding only".
  - 2.0 §3.3 Scope and Fault scope.
- **Reproduction:** with binding A open, the peer sends a tag-16 frame `[1]["r"][CBOR envelope {session_id: A, kind: data, stream_id: 0, …}]` (or a kind byte the receiver does not know). The carrier ends with `Protocol(Message)`, and every other binding and call is lost.
- **Impact:**
  - Fault scope is wrong.
  - Version skew is a session-ending error: 2.0 §3.1 checks only the protocol range, so a client and a served core of different gwz-transport versions are allowed, and a newer kind kills the session rather than one operation.
- **Correction:**
  - Read `session_id` (envelope key 2) with a shallow CBOR walk, as `validate_session_open` already walks CBOR.
  - Not a live binding: discard the frame.
  - Live binding whose full decode fails: `fault_binding(id, BindingFault::Invalid)`.
  - Session fault only when the body is not a CBOR map or the request-name framing is broken.
  - Remove the dead `stream_id < 0` branch.
- **Closure test:** with A and B open, an undecodable envelope naming A faults A only. B completes, and the carrier stays open. An envelope with an unknown kind naming a closed binding is counted as discarded.

**P2-3. The client-placement `SessionOpen` shape contradicts the frozen text and the schema, so no conformant `SessionOpen` can pass.**
- **Root cause:** `validate_session_open` allows only keys 1, 2 and 4 (`wire.rs:144`). Key 5 (`attributes`) and key 6 (`host_environment`) give `ForbiddenField` (`:163-165`). It runs on send (`stream.rs:421-429`) and on receive (`:847-854`).
- **Violated invariant:**
  - 2.0 §3.1: "`environment` absent, `host_environment` true, `attributes` with `umask` and `openssl` absent and `transport_off` absent or false. Every host … accepts exactly that combination".
  - The parked schema marks `attributes: ProcessAttributes (5)` required (`GwzCoreServerDesign.md:157`).
- **Reproduction:**
  - `{1: 1, 2: "gwz 1.1.0", 5: {}, 6: true}` → `Err(ForbiddenField(5))`.
  - `{1: 1, 2: "gwz 1.1.0"}` is accepted by the validator but has no required key 5, so a taut decoder of the schema, which 2.2b writes, refuses it.
- **Impact:**
  - The 2.1 carrier and the frozen operation channel cannot agree on any handshake: a wire-compatibility block for 2.2b and 2.3.
  - The validator's own reason ("facts about the client's process that the core must not adopt", `wire.rs:16-18`) is already met by the frozen rule, because `umask` and `openssl` are absent.
- **Correction (the code moves):**
  - Accept exactly: keys 1 and 2; optional 4; 5 present as a map containing at most key 2 (`transport_off`) with value false; 6 present with value `true` (CBOR `0xf5`).
  - Refuse 3, any other key, any other value of 5 or 6, and a missing 5 or 6.
  - The alternative, amending the text, needs a reviewed 2.0 amendment and also a schema change.
- **Closure test:** rows for each accepted and refused combination. The frozen combination is accepted on send and on receive. Each refused combination is refused before a byte is written, and wiped on receive. The canary row still holds.

**P2-4. The per-user socket directory is not the one 2.0 revision 3 froze. On macOS it is the location the Safety review rejected, with a `/tmp` fallback.**
- **Root cause:** `user_parent` uses `confstr(_CS_DARWIN_USER_TEMP_DIR)` (`unix.rs:96-110`). `for_current_user` appends `gwz/`, and when `confstr` fails it falls back to `/tmp/gwz-<uid>/` on macOS too (`unix.rs:123-130`; documented at `local/mod.rs:9-13`). On Linux it hard-codes `/run/user/<uid>/gwz/`, then `/tmp/gwz-<uid>/` (`unix.rs:113-116`).
- **Violated invariant (2.0 §1 macOS row and §4.2):**
  - macOS: "`confstr(_CS_DARWIN_USER_DIR)` followed by `gwz-<uid>/` … if `confstr` fails the server refuses to start (`server_address_refused`) … not the per-user temporary directory, whose old files dirhelper removes (S-P3-6). A `/tmp` fallback would put the socket in a shared sticky directory."
  - Linux: "keeps the server design's order (`$XDG_RUNTIME_DIR/gwz/`, else `$TMPDIR/gwz-<uid>/`, else `/tmp/gwz-<uid>/`)".
- **Reproduction:**
  - On macOS, `RuntimeDir::for_current_user()` returns `/var/folders/…/T/gwz`.
  - With `confstr` failing, it returns `/tmp/gwz-<uid>`.
  - On Linux with `XDG_RUNTIME_DIR` set elsewhere, that value is ignored.
- **Impact:**
  - On macOS, dirhelper's cleanup can unlink a long-lived server's socket and lock, which is the S-P3-6 failure.
  - The shared-sticky `/tmp` fallback is reintroduced on macOS.
  - 2.3's `stop`, `status` and `list` addresses (`<per-user directory>/ws-<key>`) would be computed from a directory the text does not define.
- **Correction:**
  - macOS: `_CS_DARWIN_USER_DIR` + `gwz-<uid>/`, refuse when `confstr` fails, and no `/tmp` fallback.
  - Linux: the frozen order.
  - The 0700 check stays on the directory gwz creates, plus the parent-writable walk of §1.
  - Keep the `sun_path` rule. 2.3 will also need a named bind for `ws-<key>` (19 bytes).
- **Closure test:**
  - macOS: the resolved path is under `confstr(_CS_DARWIN_USER_DIR)` and ends in `gwz-<uid>`; a simulated `confstr` failure refuses.
  - Linux: `XDG_RUNTIME_DIR` and `TMPDIR` are honoured in order.

**P2-5. On Windows, an aborting end neither closes nor disconnects its pipe, and its one-shot cancel can be lost, so the peer is not told.**
- **Root cause:**
  - The Windows `abort` closure is `killer.cancel()` = `CancelIoEx` (`windows.rs:457-460`, `:486-490`).
  - The `killer: Arc<Pipe>` lives in `Core.abort` for as long as the `StreamCarrier` exists.
  - The handle closes only when every `Arc<Pipe>` drops, and `DisconnectNamedPipe` is called only on refusal (`:580`).
  - `Pipe::run` has no aborted flag (`:391-416`), so a reader that is between reads when `CancelIoEx` runs issues its next `ReadFile` and blocks.
  - Unix's `shutdown(Both)` (`unix.rs:264-266`) is persistent and the peer sees EOF at once.
- **Violated invariant:**
  - `StreamCarrier::abort` promises "the stream is shut and the peer reads the stream's end" (`stream.rs:371`).
  - The module doc says "Nothing waits on a timeout to learn that the other end has gone" (`mod.rs:28`).
  - Windows parity.
- **Reproduction (state sequence; inferred from the code, and no Windows row exercises it):**
  1. The core end routes a frame that makes `session_fault` fire (a registry overrun, a bad close code, an oversized transport frame) or calls `abort()`.
  2. `finish(…, Abort)` (`stream.rs:551-563`) calls `CancelIoEx`, but no `ReadFile` is outstanding because the reader thread is the caller.
  3. The writer exits, with no announcement on the Abort path.
  4. The session holds the `StreamCarrier`, so the handle stays open.
  5. The client's reader stays blocked with no EOF until the core drops the carrier. If the core's reader re-entered `ReadFile` after the cancel, its thread also keeps the handle alive after the drop, until the client writes or closes.
- **Impact:**
  - Silent hangs and leaked reader threads on Windows only.
  - The client gets no carrier reason.
  - An idle client is not covered by 2.0 §4.3's 60-second rule, which needs frames queued.
- **Correction:**
  - Give `Pipe` an `aborted` flag that is checked before each `ReadFile` and `WriteFile` and after each completion.
  - On abort, set the flag, call `CancelIoEx`, then `DisconnectNamedPipe` on the server side.
  - On the client side, close the handle once both halves have returned: hold it in a `Mutex<Option<Owned>>` that the last I/O user releases.
- **Closure test (Windows rows):**
  - Abort one end while the caller still holds it; the peer reads `PeerGone` within a short bound.
  - The same with the abort taken while the reader thread is inside `route` (a forced `session_fault`).
  - Both reader threads exit.

**P3-1. Host-side refusals are silent connection drops, so the frozen refusal frames can never be sent.**
- **Root cause:** the carrier refuses and tears down before the host can answer:
  - The received `SessionOpen` refusal is `session_fault` → Abort (`stream.rs:847-854`).
  - The peer refusal shuts the connection "with nothing written" (`unix.rs:235-249`; `windows.rs:578-582`).
  - `Accepted` returns no connection a host could answer on.
- **Violated invariant:**
  - 2.0 §3.1: refuse "with `invalid_request` before `SessionOpened`".
  - Parked §4, adopted unchanged by 2.0 §4.3: "A refused peer gets one `SessionError` (`call_id` 0, `server_peer_refused`) naming the cause".
- **Impact:** diagnosability. A non-conforming or old client, such as a parked-design bridge that sends an environment, and a refused peer both see end of stream (`server_unavailable`) instead of the named code. A conforming gwz client refuses on send first.
- **Correction:** return the refusal together with the still-open connection (or a sink limited to one `SessionError`), so that 2.2b and 2.3 can write the frozen frame and then close. Keep "nothing read" before the peer check.
- **Closure test:** an other-user client, and a client that sends an environment, each receive exactly one `SessionError` with call_id 0 and the code, then EOF.

**P3-2. The peer-rule source check can be bypassed by assigning the policy's field.**
- **Root cause:** `PeerPolicy.own` is `pub(in super::super)` (`peer.rs:92-95`). That makes it visible to every production file in `carrier/`. The check scans only for struct literals, names and `verify` definitions (`check_carrier_peer_rule.py:113-116`).
- **Reproduction:** on a scratch copy, inserting `let mut policy = policy.clone(); policy.own = peer.clone();` before `policy.verify(…)` in `unix.rs:247` still gives `check_carrier_peer_rule: ok (11 files)`, exit 0.
- **Impact:** the guard the plan requires ("No test-only override of the peer check exists in production code (source check)") misses a configurable override written this way. An always-on bypass would still fail the runtime rows.
- **Correction:** make `own` private, and give the tests a constructor inside `peer.rs` under a `cfg_if!` test arm that the check allows, or have the check refuse `.own` outside `peer.rs`.
- **Closure test:** a unit case in `test_check_carrier_peer_rule.py` with the assignment above must fail.

**P3-3. The close announcement is a wire element the frozen format does not define.**
- **Root cause:** the end-of-carrier code travels as tag 17, `[0, code]` (`wire.rs:121-139`). It is detected by first byte 0 (`stream.rs:839-845`) and written after the drain (`:920-926`).
- **Violated invariant:** 2.0 §3.3 "Tag alignment": tag 17 carries exactly the transport kinds.
- **Impact:**
  - A peer built to the frozen text (gwz-py's bridge, or 2.2b's `TransportLaneFrame` decoder) treats it as a malformed lane frame, which is a protocol error.
  - The announcement also adds a 2-second `LINGER` (`stream.rs:45`).
- **Correction:** either amend 2.0 §3.3 to define a carrier announcement (its tag, body, codes and linger), or remove it in 2.2b in favour of end of stream plus the session's close report. This review accepts either, provided it is recorded.
- **Closure test:** a 2.0 amendment row, or the announcement's removal with the drop and close rows still passing.

**P3-4. The seam's documentation states invariants the stream carrier does not have.**
- **Root cause:** the docs were written for the in-process pair:
  - `CarrierId` is "shared by its two ends" (`binding.rs:6-8`), but each stream or local end draws its own (`unix.rs:260`; `windows.rs:485`).
  - The seam is "one FIFO … A control message does not overtake the data queued before it" (`mod.rs:14-18`), but the stream writer puts control frames ahead of other targets' data.
  - Consequently `admits` in the pump compares an end's binding with itself (`link.rs:50`, `:117`), which is always true.
- **Impact:** 2.2c may design connection identity or ordering on false statements. The implementer's README note (5) confirms the first.
- **Correction:** state the per-end id and the D-N ordering in the seam docs; name the handshake field 2.2c needs.
- **Closure test:** a documentation diff, reviewed with 2.2c.

## 2. Invariant analysis (attacks that failed, with evidence)

- **D-N writer rule** (`write_queue.rs:70-86`). The candidate is the first control entry. It is blocked if any earlier entry is tag 1 or has its target; when blocked, the oldest frame goes.
  - Control frames stay FIFO among themselves.
  - A blocked control frame leaves right after its last blocker.
  - Ordinary frames stay FIFO.
  - Starvation is bounded by the control reserve (16).
  - Head-of-line blocking of a later control frame behind a control frame that is waiting on its own stream is what the text's "oldest control frame" rule means, and its delay is bounded by already-credited data that drains to a reader that never blocks.
- **Close report keyed ordinary and drain before close:** `Ending::Flush` writes everything queued before the announcement (`stream.rs:880-927`), and `admit` refuses after `finish` (`:524-526`).
- **No body decoding in the two crates:** neither manifest has a CBOR or taut dependency, and neither crate's source contains `cbor` or `decode` outside comments. `in_process.rs` ignores the key (`let _ = key`). The suite's fixture wraps a byte-stream end through `RoutedEnd`/`QueueRouter` (`test_router.rs:93-119`). `frames_arrive_in_order_on_both_lanes` uses one target, so it is per target.
- **Reader:**
  - The length is checked before allocation, then the tag, then a single allocation, all wiped by the guard (`byte_stream.rs:373-393`).
  - `route` never blocks: there are no waits under the state lock in `stream.rs:646-744`.
  - No re-entrant deadlock through `delivery`: neither `route`, `finish`, the waker paths nor `ended` reach `Shared::end`.
- **Tag split:** `Lane::of` (`mod.rs:68-73`) equals the mux's own rule (gwz-transport `mux/mod.rs:638-641`). A wrong tag faults the binding (`stream.rs:768-770`).
- **Data bound:** per stream, by `receive_window`, with a charge of at least one byte (`stream.rs:795-806`). A sender that keeps within credit cannot overrun it. Window coalescing keeps the highest offset in its place (`:707-721`).
- **Peer rule:**
  - The check is pure, has one constructor, runs in both directions, and refuses when it cannot be made (`peer.rs:107-131`).
  - On Windows: first instance, reject-remote, the `D:P(A;;GA;;;<sid>)` DACL, and a client opened with `SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION` (`windows.rs:54-58`, `:209-211`, `:611-618`).
  - The listener accepts a client at its own integrity or above; the client wants exactly its own.
- **`sun_path`:** the field less the NUL, refused before anything is created (`unix.rs:26-50`).
- **Platform parity rules:**
  - Conditional compilation is `cfg_if!` only, apart from `#[cfg(unix)] mod unix { … }` in `local_rows.rs:415`, an enclosing braced module, and the crate's existing `#![cfg(test)]` file pattern.
  - No `static`, `thread_local!`, `OnceLock` or `lazy_static` in the diff.
  - No production code under `tests/`.
  - Control flow is braced.
- **Trunk patch (`mod-rs-trunk-patch.diff`):**
  - It is conceptually correct against trunk `fb26d4dc`. The `LocalLink` sites are unchanged (`mod.rs:247`, `:390`; `cli_driver_tests.rs:22`, `:41`), the `session.rs` module list matches, and the trunk's `local_link.rs` is byte-identical to the lane's baseline.
  - `mod.rs` hunk 1 applies only with fuzz (it fails at `-F0`), because 1.3a inserted `composition`, `connection_factory` and `connection_host`.
  - The `check_checked_artifact_boundaries` hunk applies only after the lane merge, by design.
- **Global byte bound (`InboundLimits.bytes`, 128 MiB), attempted and only partly successful (a risk, not a finding):**
  - It is not a quantity the sender controls.
  - It faults whichever binding's frame arrives when it is exceeded (`stream.rs:697`, `:776-781`), and it can end the session on a registry frame (`:652-656`).
  - It is unreachable under the session limit of 8 running operations at 1 MiB per stream.

## 3. Risks and next action

- **Risks:**
  - **Memory amplification:** a frame is charged one byte against the window, but its real cost is about 150–200 bytes: a `Queued` entry, a `String` copy of the binding name, a `Vec` and a B-tree node. A hostile or remote (`ssh://`) core can hold about 5× the 128 MiB global bound in memory.
  - **Sequenced (v3) windows** are neither coalesced nor slot-bounded (`stream.rs:812-822`). The global bound is their only limit.
  - **`connect` blocks without a bound** (`unix.rs:357`); the parked §3 requires a non-blocking connect within the handshake bound. The address walk and `SessionHello` are not built yet.
  - **2.2b must replace** `wire.rs`'s body, `MAX_TRANSPORT_FRAME` and `is_close` together.
  - `stream.rs` is 7 lines under the split threshold. Its writer, router and binding state split cleanly; that is the operator's decision.
- **Next action:**
  1. The lane owner remediates P2-1 to P2-5 with the closure tests above, all inside gwz-core and the evidence member.
  2. For P2-3 and P3-3, record whether the code or the text moves.
  3. Assign process-id pinning, the sandbox rule, the address walk and bounded connect, and the CLI and `SocketCoreBridge` rows to named steps.
  4. Re-review on this axis with Windows rows for P2-5 and a cargo run when the load allows.
