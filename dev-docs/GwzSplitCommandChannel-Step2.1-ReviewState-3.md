# Step 2.1 carrier (gwz-core 91b4c924) — STATE-AXIS RE-VERDICT, ROUND 3

**Review object:** gwz-core `49e79493..91b4c924` (10 files, +435/−47) and evidence `d385d3bd`, section "Remediation round 2" of `runs/2026-10-25-p2-1-rem1/`, under `raw/rem2/`.
**Date:** 2026-10-27. **Axis:** State.
**Verdict: GO.** No P0, P1 or P2 is open. My original N-P2-1 counterexample is now refuted on this tree.

## 0. Evidence base

- **Tuple, checked at start and end, unchanged:** root `d6bd7f15`, gwz-core `91b4c924`, gwz-core-evidence `d385d3bd`, gwz-transport `70516c0e`.
- **I ran the rows myself** at a load of about 5, using the review target dir, which I deleted afterwards:
  - all carrier rows in the candidate shape (`RUSTFLAGS='--cfg gwz_transport_candidate' cargo test --lib carrier::`): 94 passed, 0 failed;
  - the 17 scoped-link and class rows, 8 consecutive runs: 17 passed every time.
- **Digest audit:** the sha256 of `raw/rem2/sources/gwz-core-tracked.patch` is `4c8f336e…`. It equals `git diff 49e79493 91b4c924 --binary`, so the evidence tree is the commit.
- **Gate evidence read:**
  - mutation log `mutation-faulted-ok-none.log`: the `Ok(None)` mutant fails 3 rows (91 passed, 3 failed);
  - Linux matrix `p2-1-rem2-20261027-a`: OVERALL PASS;
  - Windows receipt `p2-1-rem2-20261027-a`: 99 passed, 0 failed, no gaps, `passed: true`.

## Prior-finding closure table

| ID | Status | Evidence |
| --- | --- | --- |
| **N-P2-1:** a faulted, retired or unknown binding never ends its consumer's take | **Closed** | See "How the counterexample is refuted" below. |
| **N-P3-2:** Unix `turn_away` bounded by bytes, not time | **Closed** | One overall deadline, `TURN_AWAY_BOUND` = 1 s. The write timeout uses that bound, and each read timeout is min(200 ms, time left); the drain stops at the deadline (`unix.rs:340–370`). Row `local_rows::unix::a_refused_client_that_trickles_bytes_holds_the_accept_thread_for_one_second_at_most`: a peer sending one byte per 100 ms for 4 s releases accept within 1.5 s, and the next client still gets its `SessionError` (82). The Windows-parity entry is added. |
| **N-P3-1:** a binding's bound is `max_streams × receive_window` with no carrier-wide cap | **Carried to 2.2c** per the remediation plan | Unchanged. Not re-reviewed. |

### How the counterexample is refuted (N-P2-1)

- **The take now reports the end.** `Core::take_binding` (`writer.rs:229–244`) checks in this order:
  1. the carrier's own close first: `TakeEnd::Carrier`. Because this comes first, a `finish` that clears the bindings is never misreported as `NotOpen`.
  2. then the binding's end: `TakeEnd::Binding(Faulted(fault) | NotOpen)`, from `State::binding_end`.
  3. only then the ordinary per-binding take.
- **Both take APIs use it:** `try_recv_binding` and `recv_binding_timeout`.
- **Retiring wakes the consumer:** `retire_binding` now wakes the binding's waker and `in_ready`.
- **The link ends only that join:** `CarrierLink` maps `TakeEnd::Binding` to a join-only end. It disconnects that port alone, records `join_ended(index)`, does not close the shared carrier end, and keeps the other joins running.
- **My round-2 interleaving now behaves correctly:** overrun `a` under a link scoped to `a`, and `a`'s port is disconnected with `Faulted(Overrun(TransportData))` recorded, while `b` and the carrier go on. Rows:
  - `seam_rows`: `a_scoped_link_ends_only_the_join_whose_binding_was_faulted`, `…_retired`, `…_never_opened`;
  - `class_rows`: `a_faulted_binding_ends_its_own_take_and_the_carrier_and_the_other_binding_go_on`, `a_retired_binding_wakes_and_ends_its_consumer_and_an_unknown_one_is_over_at_once`.
- **The old row that asserted `Ok(None)`** (the bug itself) now asserts the terminal.

## Judging the two test-only race fixes

- **(a) One 5-byte frame instead of five 1-byte frames.** Acceptable. The carrier's per-stream data bound counts bytes *queued and not yet taken*, as 2.0 §3.3 specifies. A scoped link that drains promptly can never overrun it with small frames. Enforcing the sender's window credit is the mux's job, not the carrier's. One frame larger than the window is the honest way to produce a queued overrun, so the row does not weaken the invariant.
- **(b) `eventually` around the registry frame and `b`'s frame.** Acceptable. Those frames were written after the faulting frame and can still be in the reader when the fault shows. The row still asserts that `a` reports `Faulted` and that the registry frame and `b`'s frame are delivered. That is the ordering-independent property; nothing is relaxed.

## New findings

None at P0–P3.

Observations, not findings:

- **Idle link loop.** A link whose last join has ended keeps waking every 5 ms (`PARK`) until it is stopped or dropped. That is cheap and bounded by the owner's lifetime, and it is the same idle behaviour as before.
- **Earlier Windows receipt.** `p2-1-rem2-20261026-a` reported PASS although its hosted-like run stopped before the carrier rows. The README flags it and does not use it. That is a weakness in `windows_lane_check.py` (outside this object), not in the carrier.
- **Ports learn no reason.** A disconnected join's port is still told nothing about why; the reason is available through `join_ended`. This matches the round-2 acceptance of implementer item 1.

## Verdict

**GO** on the State axis for step 2.1 at gwz-core `91b4c924`. Carried items:

- N-P3-1, to 2.2c.
- Recorded in the plan for 2.3: the sandbox rule's platform checks; the 30 s link check, lock file, stale-socket rule and `ws-<key>` naming; the handshake canary capture.
- Also for 2.3: Linux pinning that compares the process start time with the connection time.
