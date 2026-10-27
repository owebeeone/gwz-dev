# Step 2.1 carrier (gwz-core d8e7478d) — STATE-AXIS REVIEW

**Review object:** gwz-core `0ed48168..d8e7478d81` (step 2.1: `src/transport_host/carrier/`, `crates/session-contract`, `crates/session-channel`, `session_host/context.rs`, `scripts/checks/check_carrier_peer_rule.py`, `windows_parity/2.1.json`, wiring), plus the evidence run `gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-25-p2-1/`.
**Baseline:** gwz-core `0ed48168`. The controlling documents are the root copies of `GwzSplitCommandChannel.md` rev 3, `GwzCoreSessionDesign.md` §3/§8, the parked `GwzCoreServerDesign.md` §4/§6, and plan §4 2.1 and §10 D-D/D-N.
**Date:** 2026-10-25. **Axis:** State.
**Verdict: NO-GO.** P0 0, P1 0, P2 6, P3 5. I commit to GO once bounded remedies for P2-1…P2-6 land (§3).

## 0. Evidence base

- **Tuple, checked at start and end, unchanged.**
  - Lane root `d6bd7f15`, gwz-core `d8e7478d81`, gwz-core-evidence `0293b6d18b`.
  - Other members: gwz-transport `8e43e6dd`, gwz-git `a9d7ee09`, gwz-cli `cea7bd4c`, gwz-py `71d2d072`, gwz-sspi `a2978a8f`, gwz-credential `6d78c593`, git2-rs `d13951f7`, libgit2 `b172e3d1`, taut `913e0dc3`, taut-shape `89916c4d`.
  - As expected, this gwz-core has no gwz-git dependency and no `composition.rs`.
  - The lane root has staged `gwz.conf` changes. They are not part of the object.
- **Read in full:**
  - carrier: `mod.rs`, `stream.rs`, `wire.rs`, `link.rs`, `in_process.rs`, `queue.rs`, `local/{mod,peer,unix,windows}.rs`;
  - `session-channel`: `byte_stream.rs`, `write_queue.rs`;
  - the `session-contract` `channel.rs` diff;
  - the `local_link.rs` and `context.rs` diffs;
  - the source check;
  - the test rows I cite;
  - gwz-transport `binding::default_limits`, the mux `max_streams`, `stream/machine.rs` flush, `sequenced.rs`.
- **Ran:** `cargo test -p gwz-session-channel` → 43 passed, run at a 1-minute load of 36 into the review target dir, which I deleted afterwards. I did not run `cargo test --lib carrier::`, because the 1-minute load was 39.5–91 and the 5-minute load stayed above 60.
- **Evidence:** README, manifest, digests, the mutation summaries, the Windows receipt `final-a-receipt.json`, `windows-rows-nocapture.log`, the carrier lines of the hosted-like log, and the Linux `summary.txt` and `candidate-test.log` (50 carrier rows ok).
- **Digest audit (my own recomputation):**
  - The sha256 of `raw/sources/gwz-core-tracked.patch` is `59cbeb47…`. That equals `final.txt`, and it equals `git diff 0ed48168 d8e7478d --binary --diff-filter=M`.
  - All 40 files in `gwz-core-untracked.tar` are byte-identical to the files the commit adds. Their list hash `c0807706…` equals `final.txt`.
  - The inherited dev-docs hashes match the lane's files today.
  - The re-entered `start.txt` claims empty diffs at the same HEADs. That is consistent but cannot be independently re-proven. Since the end state equals the commit, the change set is fully attributable.

## Problem, requirement provenance, and scope

- **Requested outcome:** plan 2.1, with its red rows, under D-D and D-N and the frozen 2.0 shapes.
- **Components touched:** gwz-core and the evidence member only, which is authorized.
- **Edits outside the named files** are wiring for the plan-required source check, inside gwz-core, and demonstrably necessary:
  - `.github/workflows/checked-artifact-boundary.yml`;
  - `scripts/run_tests.py`;
  - `scripts/checks/test_check_carrier_peer_rule.py`;
  - one approved `#[path]` edge in `check_checked_artifact_boundaries.py`. It declares the carrier from `session/local_link.rs` because `mod.rs` is the trunk's file; `mod-rs-trunk-patch.diff` removes it. This is a temporary seam, recorded.
- **Scope-changing constraints without provenance:**
  - (a) `check_carrier_peer_rule.py` forbids every environment read in `carrier/`. That is self-imposed, and it makes 2.0 §1's Linux order (`$XDG_RUNTIME_DIR`, `$TMPDIR`) impossible inside the module that resolves the directory (P2-4).
  - (b) The README says the sandbox rule is "not in 2.1's text". But 2.0 §4.3 binds the sandbox rule to the peer check, "(2.1 source check)", and no step owns it (P2-5).
- **Not implemented here, and owned by 2.3 per 2.0 §4.3:** the 30 s link check, drain-before-exit, the lock file, the stale-socket rule and `ws-<key>` naming. The README does not record this deferral (§3).

## Deviations from the frozen 2.0 text: disposition per item

1. **`FrameRouter::ended(Closed)`: text moves.** It is additive. It is called once, after the last `route`, under the `delivery` mutex (`byte_stream.rs:206–208, 257–267`).
2. **Provisional CBOR lane body: code moves at 2.2b.** Acceptable while nothing calls it in production. Routing keys on `session_id` alone, so 2.0 §3.3's "valid only on the … request that bound it" is not enforced yet. 2.2b/2.2c must add that.
3. **Close announcement = tag 17 with a zero-length name: text moves.** It is sound as a discriminator. Its state laxity is P3-4.
4. **Four slots per kind: text moves.** It is harmless and bounded. A profile-1/2 sender cannot exceed one: `start_flush` returns the existing ticket (`stream/machine.rs:114–134`), and the terminal kinds occur once each. The implementer's "flush barriers repeat" reason is weaker than needed.
5. **`byte_stream` returns `io::Result`: text moves.** Thread spawn can fail.
6. **`SessionOpen` drops keys 5 and 6: defect, code moves or a recorded decision moves the text.** See P2-3.

The implementer did not list these deviations; they are findings: the macOS and Linux directory (P2-4), the sandbox rule and pinning (P2-5), a refused peer gets nothing instead of the parked §4 single `SessionError` (2.3, observation), and random `s-` socket names (2.3, observation).

## 1. Findings

### P2-1. A stream failure after the peer's last frame discards the frames already received and replaces the close reason

- **Location:**
  - `stream.rs:540–563`: `Core::finish` clears `inn` at :547.
  - It is reached from `CarrierRouter::ended(Closed::Stream)` at :859–872 and from the writer's error path at :902–914.
  - `byte_stream.rs:259`: once `ended` is set, the reader stops routing frames it has already read.
- **Violated invariant:**
  - The `Carrier::try_recv` contract: "After the peer's close, the messages it sent before still arrive, then the closure" (`mod.rs:325–326`).
  - Plan row: "carrier dropped → both sessions end with the carrier's reason".
  - 2.0 §3.3 Close report: the client "delivers every reply that arrives to its waiter".
- **Reproduction:**
  - (a) Deterministic on Linux, half-closed socket. A raw peer on a `UnixStream` pair writes N valid tag-16 frames for an open binding, then calls `shutdown(Read)`. The carrier `try_send`s one data frame. The writer gets EPIPE, `shared.end(Stream(BrokenPipe))` makes `finish(…, Abort)` clear `inn`, and `try_recv` returns `Err(Stream(BrokenPipe))` with zero arrivals.
  - (b) Process kill. The core writes its final reply and frames, then is killed. The client's mux writes a `window` before the client's consumer has taken the queued frames. The frames are lost, and the reason is `Stream(BrokenPipe)` instead of `PeerGone`. Which outcome happens depends on which thread notices first.
  - (c) Orderly close under load. The peer's `LINGER` (2 s) expires before this end reads the announcement. The peer shuts down, this end's active writer gets EPIPE, and `Peer(code)` and the replies are lost.
- **Impact:**
  - Progress is lost but never invented: a received success reply or terminal frame is reported as carrier loss.
  - The close reason is nondeterministic across thread timing and platform.
  - No row covers a separate-process kill, which the plan's process-kill qualification requires.
- **Correction:**
  - Treat a failure on the peer's side, whether the writer sees it (EPIPE, ECONNRESET, `ERROR_NO_DATA`) or the reader does, like `peer_ended`: stop writing, keep reading to EOF, deliver what is queued, then report one deterministic reason (`PeerGone`).
  - Only a local close or abort clears inbound frames.
- **Closure test:** a Unix half-close row, plus a child-process-kill row on all three platforms, each asserting N arrivals followed by `PeerGone`.

### P2-2. No stall bound: closing or dropping a carrier whose peer has stopped reading never completes

- **Location:**
  - `stream.rs:876–934`: `write_loop` blocks in `end.send` and has no deadline.
  - `close` (:979) and `Drop` (:990) use `Ending::Flush` with no escalation.
  - Only `StreamCarrier::abort` calls the platform abort.
  - `unix.rs` and `windows.rs` set no write timeout.
  - `mod.rs:28`: "Nothing waits on a timeout".
- **Violated invariant:**
  - 2.0 §4.3 Stuck peers: the 60 s no-byte rule, in both directions. It has no mechanism and no recorded owner.
  - The README Limitations claim "a wedged peer holds the writer thread that long [2 s]" is false: `LINGER` applies only after the queue and the announcement are written.
- **Reproduction:** connect a local pair. The peer stops reading (SIGSTOP, or a raw stream that never reads). This end `try_send`s data until the socket buffer fills, and the writer blocks inside `write_frame`. Then drop this end. The writer thread, the reader thread and both descriptors live forever, and consumers see `Ok(None)` forever.
  - The tests mask this: the `against_raw` and `io::pipe` harness unblocks the writer only because the test drops the reading half.
- **Impact:**
  - A stuck state with a leak per connection.
  - 2.3 cannot build "accepts no byte for 60 s" on top: `queued_out()` does not move while one frame of up to 64 MiB is mid-write, so there is no progress signal.
- **Correction:**
  - In the carrier: a last-byte-written instant, and a timed writer or watchdog. When frames are queued and no byte has moved for the bound, finish with a stall reason and abort.
  - Bound the Flush on `close` and `Drop` by the close bound, then abort.
  - Correct the README.
  - If the lane owner places the 60 s policy in 2.3, record that. The bounded close and the progress hook are still 2.1's.
- **Closure test:** a peer that never reads. Assert that `close` and `Drop` release both threads within the bound, and that both ends report the stall reason.

### P2-3. The `SessionOpen` validator refuses the only shape 2.0 §3.1 accepts (deviation 6)

- **Location:** `wire.rs:143–176` (`ALLOWED = [1, 2, 4]`); `stream.rs` router tag-4 arm, which ends the session.
- **Violated invariant:** 2.0 §3.1 says `environment` absent, `host_environment` true, `attributes` without `umask`/`openssl`, `transport_off` absent or false. Every host accepts exactly that and refuses anything else with `invalid_request` before `SessionOpened`.
- **Reproduction:**
  - `{1, 2, 5: {}, 6: true}`, the frozen shape, gives `ForbiddenField(5)`, and the carrier aborts with no reply.
  - `{1, 2}`, which 2.0 refuses, is accepted.
- **Impact:**
  - Any 2.2b or 2.3 code written to the frozen text cannot open a session.
  - The refusal is a stream abort, not `invalid_request`.
  - The disclosure invariant (no environment crosses) holds either way.
- **Correction:**
  - The lane owner decides, and the decision is recorded.
  - Either the validator accepts exactly 2.0's shape (key 6 = true required; key 5 a map without `umask`/`openssl`, `transport_off` absent or false), or 2.0 §3.1 and plan §1.1 are amended.
  - The decision also states the refusal mode: carrier abort, or a host reply.
- **Closure test:** wire and stream rows showing the agreed shape accepted and every variant refused, on send and on receive.

### P2-4. The per-user directory is not the accepted one, and the deviation is undisclosed

- **Location:**
  - `unix.rs:96–130`: macOS uses `_CS_DARWIN_USER_TEMP_DIR` + `gwz/`; Linux hard-codes `/run/user/<uid>`, else `/tmp/gwz-<uid>`.
  - `unix.rs:65–84`: only the leaf is checked; parents are never walked.
- **Violated invariant:** 2.0 §1 and §4.2 rev 3:
  - macOS: `_CS_DARWIN_USER_DIR` + `gwz-<uid>/`, chosen because dirhelper removes old files in the temp directory (S-P3-6). The walk refuses a parent writable by group or other.
  - Linux: `$XDG_RUNTIME_DIR/gwz/`, else `$TMPDIR/gwz-<uid>/`, else `/tmp/gwz-<uid>/`.
- **Impact:**
  - On macOS, a long-running server's socket, and in 2.3 its lock and log in the same directory, can be swept. That leaves an unreachable server holding a lock, and no link check exists to notice.
  - On Linux, the order differs from the frozen text. This module cannot read the environment at all, because of the self-imposed source check.
- **Correction:**
  - Implement the macOS `confstr` rule exactly, with the parent walk.
  - Move environment-dependent Linux resolution to the caller (2.3, `RuntimeDir::at`), or let the check permit it in one named file. Either choice is a recorded decision.
- **Closure test:**
  - A row showing the macOS path comes from `_CS_DARWIN_USER_DIR` and ends in `gwz-<uid>`.
  - A parent with group-write is refused.
  - Linux order rows at the layer that owns the environment.

### P2-5. The peer rule omits the adopted sandbox rule and process-ID pinning, and no step owns them

- **Location:** `peer.rs:108–131`, `unix.rs:190–233`, `windows.rs` `process_identity` and `accept`/`connect`.
- **Violated invariant:** 2.0 §4.3 Peers: "the server design's §4 peer check, sandbox rule and listener verification apply in both directions, unchanged … (2.1 source check)". The parked §4 requires:
  - Windows: refusal below Medium integrity, and AppContainer refusal;
  - Linux: seccomp, user/mount namespace checks;
  - macOS: `sandbox_check` via the audit token;
  - the caller's self-check;
  - pinning (pidfd or start time, `LOCAL_PEERTOKEN`, or a process handle plus creation time).
- **Reproduction:**
  - `verify(own = Low, theirs = Low, either relation)` returns `Ok`, so two Low-integrity processes of one user reach each other.
  - A seccomp'd or user-namespaced Linux peer is accepted.
  - The pid is read, then `OpenProcess`/`/proc` is opened later with no hold on the process.
- **Impact:** once 2.3 ships, a sandboxed same-user process gains the user's full authority through a served core. The design calls this "the one case where a peer would gain something".
- **Correction:** either implement it (bounded: the Windows below-Medium refusal is one comparison in `verify`; Linux `/proc` reads; macOS `LOCAL_PEERTOKEN`), or the lane owner records the owning step and the deferral in the plan before 2.3.
- **Closure test:**
  - Pure rows for Low and Untrusted in both relations.
  - A Linux child in a user namespace, refused.
  - A Windows Low-integrity child, refused.
  - A peer that exits before the check, refused.

### P2-6 (ARCHITECTURAL). One arrival-ordered inbound queue and one carrier-wide byte pool shared by the registry and transport classes

- **Location:**
  - `stream.rs`: `State.inn` (`BTreeMap` by sequence number); `Core::take` at :604 (`pop_first`).
  - `route_registry` at :646–653 and `route_transport` at :697 both charge `inn_bytes` against `InboundLimits.bytes` = 128 MiB (`mod.rs:139–153`).
  - `link.rs:103–111`: a pending `give` stops `try_recv` for the whole end.
- **Violated invariant:** 2.0 §3.3:
  - "a consumer that is not ready holds back that binding only, never the reader";
  - "a sender that sends data only within Window credit cannot overrun it";
  - Fault scope: only call-class and session-class overruns end the session.
- **Interleaving:**
  1. Binding A's consumer is not ready.
  2. The peer keeps sending within its windows on bindings A, B and C. At defaults that is up to 64 streams × 1 MiB per mux, and `max_streams` can be configured up to 1024.
  3. Queued bytes pass 128 MiB.
  4. The next transport frame faults the binding it happens to belong to (`global_over`), which may be the conforming B.
  5. The next registry frame (`operation.cancel`, a reply) ends the whole session with `Overrun(Registry)`, though the registry class holds a handful of frames.
  6. Separately, inbound registry control frames can never be taken ahead of earlier transport frames.
- **Impact:**
  - Fail-closed but misattributed. This reopens S-P2-2 on the receive side.
  - Reachability: it needs a stalled consumer and more than 128 MiB of window credit in flight. Today's single-stream operations do not reach that; 2.2c pooling or larger configured limits make it reachable.
- **Correction:**
  - Give the registry its own byte cap.
  - Bound transport by the windows the carrier itself grants, or fault the binding with the largest backlog.
  - Give 2.2c a per-binding take (for example `recv_binding(id)`) beside the registry take.
- **Closure test:**
  - Three bindings each fill their window with no consumer: no binding is faulted, and a registry frame is still taken.
  - One binding over its window: only that binding is faulted.

### P3-1. The writer considers only the oldest control frame

- **Location:** `write_queue.rs:70–86`.
- **Problem:** a control frame blocked by data of its own target holds every later control frame of other targets behind that data, up to `QUEUE_BYTES` (8 MiB). Example: core sends push data on stream A and a `window` for A's return direction; a cancel for B then waits behind all of A's queued data.
- **Status:** literally conforms to D-N.
- **Correction:** pick the oldest *unblocked* control frame. Each candidate's own-target and `SessionCall` blockers are still checked, and same-target control frames block each other.
- **Closure test:** the two-stream row described above.

### P3-2. The Windows evidence overstates two rows

- **SMB-loopback row:**
  - It asserts only `is_err()` (`windows_rows.rs:65–91`), so codes 53 and 67 would also pass it.
  - The README attributes `Err(5)` to `PIPE_REJECT_REMOTE_CLIENTS` on the grounds that "a remote open arrives as the same user". That is unshown for a key-authenticated (S4U) SSH logon, where loopback SMB may authenticate as anonymous and the DACL alone denies access.
  - The receipt's own inference caveat is the honest reading; the row's conclusion is not proven.
- **Lower-integrity row:** it is called a host limit, but a single-user host can run it (`DuplicateTokenEx` + `SetTokenInformation(TokenIntegrityLevel, Low)` + `CreateProcessAsUserW`).
- **Correction:**
  - A discriminating variant: the flag set with a DACL that also grants Everyone and ANONYMOUS LOGON is still refused, and with the flag cleared the open behaves differently.
  - Run the Low-integrity listener row end to end.

### P3-3. `CarrierLink` loses the close reason

- **Location:** `link.rs:79, 92, 109, 123` (`Err(())`); `:180` (`close(CloseCode::Normal)` on every surviving end).
- **Problem:**
  - A local port failure is announced to a remote stream peer as an orderly close.
  - The "both sessions end with the carrier's reason" row is proven at carrier ends only, not at ports.
  - R5 risk: if 2.2c reads `Peer(Normal)` as a clean end, it could understate a possible remote effect.
- **Correction:** carry the reason to the port, and close with a code that reflects it.

### P3-4. Frames after the peer's close announcement are still routed and delivered

- **Location:** `peer_ended` (:585) does not stop `route_registry` or `route_transport`, which check only `closed`.
- **Problem:** a misbehaving peer's post-close frames are delivered before the closure.
- **Correction:** after the announcement, discard (and wipe) further frames, or fault.

### P3-5. A gap in the source check

- **Location:** `PeerPolicy.own` is `pub(in super::super)` (`peer.rs:94`).
- **Problem:** production carrier code can assign `policy.own = …` without a `PeerPolicy {` literal, and `check_carrier_peer_rule.py` does not flag that.
- **Correction:** make the field private and give tests their own constructor, or have the check flag `.own` writes outside `peer.rs`.

## 2. Invariant analysis (attacks that failed, with evidence)

- **Oversized length refused before allocation.** `read_frame` checks the size, then the tag, before `vec![0; size-1]` (`byte_stream.rs:380–388`). The row sends only 4 prefix bytes for `u32::MAX`, 64 MiB+1 and 0. A transport frame over 128 KiB is refused after an allocation of at most 64 MiB, one frame at a time, which is bounded.
- **Frames for an unknown, retired or faulted binding.** The router counts and discards them before any stream entry is created. Fault accounting stays consistent: `drop_entries` subtracts `binding.bytes`, and coalescing adjusts both totals.
- **A kind under the wrong tag** faults only its binding. That is the first check in `admit_transport`.
- **Close ordering.**
  - Flush writes the queue in priority order, then the announcement.
  - The single reader routes the peer's announcement after all its earlier frames.
  - In `write_loop`, `end.close()` (:929) precedes `abort`, so `ended(Local)` stands. Windows `CancelIoEx`'s `ERROR_OPERATION_ABORTED` never reaches the router, and `Peer(code)` survives on an orderly close.
- **The reader never blocks.** `route` takes only the state mutex, and no holder keeps it across I/O. `linger` uses `wait_timeout`.
- **`SessionOpen` disclosure.**
  - The sender's refusal happens before admission: nothing is queued, and the canary row reads the raw wire to its end.
  - The receiver refuses before routing and wipes the body. Allocation is exact-size and the wipe is `fill(0)`, though best-effort (`black_box`).
  - `SessionOpenRefusal` carries key numbers only, and nothing is logged.
- **Peer checks in both directions.**
  - On Unix, the client checks the directory, the socket's owner and mode, and the peer credentials before any write. The listener checks before a carrier exists. A Unix row counts the squatter's bytes as zero.
  - On Windows: a DACL for the user's SID only; SQOS identification; the server-pid token check before any write.
- **`/tmp` fallback.** It is created without following a leaf link and checked with `lstat`. A pre-existing directory that is another user's or is a link is refused. Sticky `/tmp` prevents replacing our own directory. `sun_path` overflow is refused before anything is created.
- **R5 (no false success).** The carrier never fabricates a frame. Coalescing keeps the higher offset of windows the peer sent. Every failure path drops or ends; none invents. The one residual is P3-3's `Normal` code.
- **Windows pipe-name lapse** after a same-user-induced `ConnectNamedPipe` error: later instances drop `FIRST_PIPE_INSTANCE`, but the client's server-pid check still refuses a squatter. The defence holds.
- **Mutations.** All 8 kill at least one row, and they cover the rules they target. Close and linger, failure-path delivery (P2-1), the stall (P2-2) and the shared byte pool (P2-6) have no mutation, which matches where the findings are.

## 3. Risks and next action

- **Bounded remedies, which together earn GO:**
  - P2-1: about 100 LOC plus the kill and half-close rows.
  - P2-2: a bounded close plus a progress deadline, with rows and the README fix.
  - P2-3: a recorded decision plus about 30 LOC.
  - P2-4: the macOS `confstr` fix, the parent walk, and a recorded Linux owner.
  - P2-5: implement it, or record the owner, plus the Windows below-Medium comparison.
  - P2-6: a separate registry pool and a per-binding take.
- **Carry-forward observations, not findings:**
  - No handshake exists in 2.1. The canary row covers the send-side and raw-peer refusals only; the full-handshake capture must run at 2.2b/2.3.
  - 2.3 must replace `LocalListener::bind`: random `s-` names, no lock, crash-left sockets, and `Drop` removing the socket by path. It must also add the 30 s link check and drain-before-exit.
  - On Linux, `connect` blocks indefinitely when the listener's accept backlog is full. The handshake bound is 2.3's.
  - Profile-3 sequenced windows are not coalesced, so they are bounded only by the carrier-wide byte pool. That feature is unstable.
  - On Windows, a writer-thread spawn failure leaks the reader thread.
  - The Windows receipt binds to `0ed48168` plus "worktree" with no tree digest. The Mac and Linux digests do match `d8e7478d`.
  - The size overrun is deferred, as instructed.
