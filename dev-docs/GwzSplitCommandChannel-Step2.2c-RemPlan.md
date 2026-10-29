# Step 2.2c rework — remediation plan, round 1

Date: 2026-10-30. Lane owner's merge of `GwzSplitCommandChannel-Step2.2c-ReviewCode.md` (NO-GO: 2 P2, 6 P3) and
`GwzSplitCommandChannel-Step2.2c-ReviewState.md` (NO-GO: 1 P1, 3 P2, 4 P3), both on the tuple gwz-transport `9d5e0866`,
gwz-core `05ea42b5`, root `49504e6d`, evidence `b79e563d`. Both reviewers pre-committed to GO on a revision that resolves
their blocking findings as specified. One patch, not a series; every row red first against `9d5e0866`/`05ea42b5`.

**Blind convergence.** State P1-1 and Code P2-1 found the same defect independently (cancel while opening hangs the
wait); State P3-2 and Code P3-6 found the same snapshot phase defect. Both are treated at the higher severity.

## Blocking findings

| ID | Disposition | Closure test |
|---|---|---|
| State P1-1 = Code P2-1 | Fix. `cancel_all` on `Stage::Opening` sets `outcome = Some(Err(OpenFailure::Cancelled))` at once and a distinct cancel-when-opened flag (a late `opened` is answered with `cancel`); `dropped` keeps meaning only "no handle". `poll_open` and `RemoteFinish::poll` answer a missing binding or exchange record with a terminal (`Err(OpenFailure::Cancelled)` / a discarded `ExchangeEnd`), never `Pending`; `is_ready()` agrees with `poll`. | Rows: bind, open, pump, cancel, then (a) client answers `opened`, (b) `open_failed`, (c) `carrier.end()` → wait `Ready(Err(Cancelled))` for (a)/(b), `Err(CarrierLost{..})` for (c); `is_ready()` matches; `op.finish()` resolves; the binding retires (`bindings() == 0`). Plus the matrix the Code reviewer asked for: cancel × {queued, opening, open, closing} × {pending, registered}, each resolving. |
| State P2-1 (with State P3-3) | Fix. `Shared::with` wakes only waiters whose state moved, or collects the wakers to wake before running `f`; a pending future is never woken by its own poll. `done_registration` polls with the real waker. Keep the final `Registration` in `RemoteOp` (or set the cell inside the lock) so `registration()` never answers `Pending` after `Registered`. | Row: N `Pending` polls of `registered()`, an `OpenWait` and an `ExchangeFinish` with no lane event → 0 wakes; a later `bound`/`opened` → ≥ 1 wake. Row: `registration()` after the binding's removal with the cell unset is not `Pending`. At least one row drives core and client only through real (counting) wakers and the drivers, with no explicit `settle()`. |
| State P2-2 | Fix. `Served::terminate` (used by `end_call`, `fault`, `lane_fault`) computes each running exchange's effect as `cancel_stream` does (`Possible` for a receive-pack exchange with `forwarded > 0`), through one shared helper for all three sites. | Rows: forwarding push then `end_call` → core `ExchangeEnd.failure == Some((Cancelled, Possible))`; the same with a client-side fault of the binding mid-push; a fetch keeps `None`. |
| State P2-3 | Fix. Aborting, cancelling while pending, or retiring a binding purges its unsent messages from `State::outbox`; `CarrierLane` refuses to re-open a session it retired. | Row: full lane, register, `finish()` → `NeverRegistered(Cancelled)`, retire, raise capacity → no `Bind` for that session is sent and the client registers nothing. gwz-core `remote_lane` row over the stream carrier: the carrier's binding count returns to zero. |
| Code P2-2 | Fix. `abort_all` maps `ErrorCode::Cancelled` to `OpenFailure::Cancelled`; `Fault(code)` stays only for rule breaches (`Protocol`, the table's `InvalidRequest`). | Rows: cancel and `finish()` of a pending operation with a queued opening, and cancel of a registered one → `Err(OpenFailure::Cancelled)` in every case. |

## P3 findings (all fixed in the same patch)

| ID | Disposition | Closure |
|---|---|---|
| State P3-1 | Fix: the receiver stops taking from the lane while its outbox holds `OUTBOX_ROOM` messages (backpressure). | Probe as row: a bind flood against a full lane holds at most `OUTBOX_ROOM`. |
| State P3-2 = Code P3-6 | Fix, keeping `snapshot() -> HostSnapshot` with `refused()` (both reviewers accept the shape; the operator was told): a refusal's `phase` is truthful (`Closed` once the carrier is lost), and the docs say every count is meaningless when `refused()`. | Extend `snapshot_is_refused_and_carries_no_counts` across carrier loss. |
| State P3-4 | Fix: the two `#[cfg(test)]` methods at the end of `receiver.rs` move into a `cfg_if!` block, as the rest of the object does. | grep shows only `if #[cfg(test)]` in `remote/`. |
| Code P3-1 | Fix: rewrite the §15 row with the full public change list (variants, fields, `Eq` removal, `#[non_exhaustive]`, `Registered`, `AdmitInput`, `OperationRef`, the `remote` module's public ports), the extra `ClientRefused` codes (`Protocol`, `UnsupportedVersion`, `UnsupportedOperation`), the pending-cancel and bootstrap-timeout outcomes, the exact §5 sentences amended, and the State reviewer's two notes: the 5 s/5 s admission-vs-bootstrap race and that client slot occupancy is call-scoped (item 11). | Lane owner checks the row against `lib.rs` `pub use` and `remote/mod.rs`. |
| Code P3-2 | Fix: `OperationRef::new` admits the lane's 128-byte request name rule. | Row admitting a 128-byte `request_id`. |
| Code P3-3 | Fix: `bind_rejection` refuses any failure detail (code + `Effect::None` only, as §3.4); correct `gwz-transport/README.md` (five codes, no detail) in the same wire-change commit. | `tests/mux.rs` row: `bind_rejected(capacity)` with a `CapacityDetail` is `InvalidMessage`. |
| Code P3-4 | Fix (the 2026-10-27 §3.3 amendment assigns this to 2.2c): `Carrier::bind_open` and the accept path take a `BindingLimits` derived from the binding's limits (`receive_window`) and the mux's `streams_per_binding`. | Stream-carrier row: more than one window on a remote binding's stream faults that binding at the carrier; a frame for stream `streams_per_binding + 1` faults it too. |
| Code P3-5 | Fix: `remote_lane.rs` says `Calls` has no implementor yet and that 2.3 adds the live-call query to 2.2b's `Service`. | Doc. |

## Out-of-scope observations (recorded, not in this patch)

- Code §4: 2.2b's `served/lane.rs` `encode_lane`/`decode_lane` carry `dead_code` reasons naming 2.2c, which does not use
  them (the carrier hand-encodes the same body; `wire_rows.rs` proves byte identity). Proposal for the next step that
  touches the carrier: one encoder (the carrier calls `encode_lane`/`decode_lane`) and the allows removed.
- Code §4 / State §3: idle polling (100 ms core tick, 5 ms receiver tick per call); the stream machine sends no terminal
  for a peer's cancel; `CarrierLane` serves both roles; lock order remote-state → registry for 2.3; `spawn` results
  ignored. Recorded for step 2.3.
- State §4 / item 11: §3.4 has no binding-release kind, so client slots are call-scoped and a call cannot run a second
  operation on another route. Operator decision whether 2.0 §3.4 needs a release kind or a multi-bind rule.
- Merge: the lane's gwz-core sits on `4703a4e8`; main has `58e506ee` (workflow) and `fb62344d` (SSH fixture) since.
  Merged at landing; neither touches this object's files.

## Re-verdict

The same two reviewers re-verdict their own findings on the corrected tuple (continued with context). The Windows
receipt `p2-2c-20261030-c` is replaced by a new label on the corrected gwz-core commit, after gwz-transport is pushed.
