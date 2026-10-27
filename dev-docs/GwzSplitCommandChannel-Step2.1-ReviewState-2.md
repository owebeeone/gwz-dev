# Step 2.1 carrier (gwz-core 49e79493) — STATE-AXIS RE-VERDICT, ROUND 2

**Review object:** gwz-core `d8e7478d..49e79493` and evidence `dcf5e013` (`runs/2026-10-25-p2-1-rem1/`), lane p2-carrier.
**Date:** 2026-10-26. **Axis:** State.
**Verdict: NO-GO.** One new P2 is open (N-P2-1). It is not architectural and has a bounded fix. All 11 round-1 findings are closed. I commit to GO once N-P2-1's fix and its closure row land; no further review round is needed for that.

## 0. Evidence base

- **Tuple, checked at start and end, unchanged:** root `d6bd7f15`, gwz-core `49e79493`, gwz-core-evidence `dcf5e013`. gwz-transport is now at `70516c0e` in the lane; round 1 saw `8e43e6dd`.
- **Rows I ran:** `RUSTFLAGS='--cfg gwz_transport_candidate' cargo test --lib carrier::` passed 88 of 88 at a load of about 15. I used the review target dir and deleted it afterwards. Without the cfg flag the filter matches 0 tests, because the carrier is compiled only in the candidate shape.
- **Digest audit:**
  - The sha256 of the rem1 `gwz-core-tracked.patch` is `66c61f6e…`. That equals `git diff d8e7478d 49e79493 --binary --diff-filter=M`.
  - The list of new files in the evidence equals the files the commit adds.
  - The README says no digest was taken at the start or end of the Linux runs or Windows receipt `-b`. As in round 1, that receipt carries no tree digest (observation).
- **Read:**
  - remediation plan;
  - `stream.rs` and `stream/{state,inbound,writer}.rs`;
  - `wire.rs` validator; `link.rs`; `peer.rs`;
  - `unix.rs` (directory resolution, pinning, `turn_away`);
  - `windows.rs` (`abort`, `turn_away`, `accept`);
  - `runtime_env.rs`;
  - the `session-channel` diffs;
  - the names of the new test rows;
  - the rem1 README mutation table;
  - the root plan's line recording the sandbox deferral (`GwzTransportReleaseConvergencePlan.md:598`).

## Prior-finding closure table

| ID | Status | Evidence on this tree |
| --- | --- | --- |
| P2-1: frames lost and reason replaced on a peer-side failure | **Closed** | A broken write now calls `peer_ended(PeerGone, false)` and stops only the writing side (`writer.rs:262–281`; `byte_stream.rs` `write_failed`). The reader goes on to EOF, and only then is `peer_final` set and the close reported (`inbound.rs:338–346`, `writer.rs:205–227`). An announcement stands over the stream's end. Only a local close or abort clears inbound frames. Rows: `a_peer_that_stops_listening_does_not_cost_the_frames_it_already_sent`, `…announcement_in_flight_stands…`, `a_peer_process_that_is_killed_leaves_what_it_sent_and_the_reason_peer_gone`. |
| P2-2: no stall bound | **Closed** | A `Counting` writer records progress, and a watchdog ends the carrier as `Stalled` and aborts the stream when it has frames queued and no byte moves for `no_progress` (default 60 s). `close` and `Drop` flush for at most the close bound, then abort (`writer.rs:326–382`). `stalled_for()` exposes the signal to 2.3. Rows cover a stall, a bounded close and a bounded drop. The defaults are 60 s and 10 s; the close bound is a 2.3 policy input. |
| P2-3: `SessionOpen` shape | **Closed** | The validator accepts exactly the frozen shape: keys 1, 2, 5, 6; key 4 optional; key 6 = `0xf5`; key 5 a map holding at most `transport_off` = false (`wire.rs:193–251`). A refused open gets one fixed-text `SessionError` (`invalid_request`, call ID 0), then the stream ends. `SERVER_PEER_REFUSED = 82` is provisional until 2.2b. |
| P2-4: per-user directory | **Closed** | macOS: `_CS_DARWIN_USER_DIR` + `gwz-<uid>`, refused when `confstr` fails, with no `/tmp` fallback. Linux keeps the frozen order with environment values passed in from the one named file, `runtime_env.rs`. Parents are walked: group- or other-writable refused unless sticky (`unix.rs:125–219`). Rows in `directory_rows.rs`. |
| P2-5: sandbox rule and pinning | **Closed**, partly by recorded deferral | Done in this patch: the Medium-integrity floor in both relations, the caller's self-check, and pinning (Linux pidfd plus start time; macOS `LOCAL_PEERTOKEN`; Windows a process handle plus creation time). The platform sandbox checks are deferred to 2.3, recorded at root plan :598. |
| P2-6 (architectural): one inbound queue, one byte pool | **Closed** | Takes by class and by binding (`State::pop(Scope)`). The registry has its own byte cap. Transport frames no longer count against a shared pool. Each frame's charge stays until its own consumer takes it. Rows in `class_rows.rs`. A new residual follows as N-P3-1. |
| P3-1: oldest control frame only | **Closed** | The writer now takes the oldest *unblocked* control frame (`write_queue.rs`). Row added. |
| P3-2: Windows rows overstated | **Closed** | The remote-open row now asserts the exact error code and adds a variant that tells the flag's effect apart from the DACL's. The Low-integrity listener row runs end to end. Red receipt failed as intended; green receipt `-b` passed. |
| P3-3: `CarrierLink` loses the close reason | **Closed** (implementer item 1 accepted) | `LinkEnd` records why the link stopped, `CarrierLink::ended()` exposes it, and surviving ends close with a code that is never `Normal` for a failure. The port still gets no reason because `disconnect()` takes none. The owner can read it from the link, which is acceptable. |
| P3-4: frames after the peer's close announcement | **Closed** (item 3 accepted) | `peer_announced` makes both routes discard and wipe what follows (`inbound.rs:57, 117`). |
| P3-5: source-check gap on `PeerPolicy.own` | **Closed** | The field is private. The test-only constructor sits inside a `cfg_if!` test arm in `peer.rs`. The check now flags writes to `.own` outside `peer.rs`. |

Implementer item 2, the accept-path refusal written by the listener itself, is accepted. It leads to N-P3-2.

## New findings

### N-P2-1 (not architectural): a faulted, retired or unknown binding is never reported through the per-binding take, so its consumer and a scoped `CarrierLink` pump wait forever

- **Location:**
  - `stream.rs:507–512` (`try_recv_binding`) and `state.rs:207` (`pop(Scope::Binding)` returns `None` for a binding with no queued entries, which includes a faulted one).
  - `writer.rs:205–227`: `take` returns `Ok(None)` unless `peer_final` is set.
  - `link.rs` `inbound`: on `Ok(None)` the pump returns without ending that join.
  - The only way to learn of the fault is to poll `binding_fault(id)`, which the link never does.
- **Violated invariant:** fault scope. A binding-class overrun or invalid frame "faults that binding" (2.0 §3.3; contract §6). The binding's owner has to learn that its binding has ended. Here it waits instead.
- **Reproduction:**
  1. Open bindings `a` and `b`.
  2. `CarrierLink::start_joined` with a join scoped to `a`.
  3. The peer sends data past `a`'s receive window.
  4. `fault_binding(a)` wakes `a`'s waker. `try_recv_binding("a")` returns `Ok(None)`, so the pump moves nothing and `a`'s port is never disconnected.
  5. The link never records a `LinkEnd`. The operation on `a` hangs until some higher-level timeout.
  6. The same happens for a binding that was retired, or never opened, under a scoped join.
- **Impact:** a stuck state in the per-binding seam the remediation introduced, which 2.2c is meant to build on. Today it is latent: only the in-process path calls the link in production, and it uses no binding scope.
- **Correction (bounded):**
  - Have `try_recv_binding` and `recv_binding_timeout` return a binding-level terminal for a faulted, retired or unknown binding (for example a `BindingClosed(BindingFault | Retired)` outcome).
  - Have `CarrierLink` end only that join: disconnect its port, record the reason, and leave the other joins and the carrier running.
- **Closure test:** a scoped link over bindings `a` and `b`, with `a` overrun. `a`'s port is disconnected within a bound with the fault recorded; `b`'s port keeps running.

### N-P3-1: a binding's inbound bound is now `max_streams × receive_window`, with no cap across the carrier

- **Location:** `BindingLimits::from_transport` (`mod.rs:217`) sets `max_streams` to 4096, and no carrier-wide cap remains.
- **Impact:** a peer that ignores flow control can make the reader queue up to about 4 GiB per open binding. A conforming peer's concurrency is the mux's `max_streams`, which defaults to 64. This matters where the peer is less trusted (the `ssh://` remote core). The implementer states it as a residual, and the remediation plan defers "memory amplification" to 2.2c.
- **Correction:** derive `max_streams` from the binding's concurrent-stream limit, or add a carrier-wide hard cap that faults only the binding with the largest backlog (the remediation plan's own alternative).

### N-P3-2: the Unix `turn_away` drain is bounded by bytes, not time

- **Location:** `unix.rs:344–362`. It drains up to 64 KiB, with a 200 ms timeout per read.
- **Problem:** a refused peer that sends one byte every <200 ms holds the single accept thread for about 3.6 hours.
- **Reachability is low:** the directory is 0700, so on Unix only a same-user peer that fails pinning can be refused. Windows `turn_away` is bounded at 1 s.
- **Correction:** one overall deadline for the drain.

### Observations (not findings)

- **Linux pinning:** it does not check that the process started before the connection, so a PID reused before `Hold::take` is accepted. That is harmless today, because the Linux rule rests on the kernel's `SO_PEERCRED` uid. It matters once 2.3 reads `/proc` for the sandbox rule; 2.3 should use `SO_PEERPIDFD` where available, or compare start time against connection time.
- **Surviving mutant:** `peer-pin-not-confirmed` (the macOS non-zombie check) survives, as the README records; the audit-token comparison still pins the process. The C-P2-5 Windows pipe changes have no mutant and rest on the red/green receipt pair.
- **Windows receipt `-b`:** like round 1's, it has no tree digest.

## Next action

Fix N-P2-1 as described, with its closure row. With that, this axis is GO. N-P3-1 and N-P3-2 can be carried to 2.2c and 2.3, alongside the deferrals the lane owner has already recorded.
