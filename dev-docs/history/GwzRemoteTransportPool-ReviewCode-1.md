# GWZ REMOTE TRANSPORT ENDPOINT CONNECTION POOL — CODE-AXIS REVIEW

**Review object:** Endpoint pool implementation at gwz-transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`, focused remediation diff from `554384f5808264009b3af016452290cacefab855`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPoolImplementation.md` at `5122031244bdf62eda0105aef9f7e98a0a5ce8dc`; remediation round 1, no API/schema freeze, dated 2026-09-19.  
**Baseline:** gwz-transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`; gwz-core `5122031244bdf62eda0105aef9f7e98a0a5ce8dc`; workspace root `dd15b7c99a625fc84241c191caba603df4ccae02`. Committed sources were read with `git show PIN:path`; the exact correction was read with `git diff 554384f5808264009b3af016452290cacefab855..e8b9a1c5408cc9ea9528939b3a602acbeb697814`.  
**Date:** 2026-09-19  
**Axis:** Code — architecture, interfaces, call graphs and compatibility reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — both prior Code P2 findings are closed, the merged cross-axis capacity correction is consistent with the Code boundary, and no new P0–P3 finding was established.

---

## Prior-finding closure

| ID | Disposition | Original counterexample verification | Status |
|---|---|---|---|
| Code P2-1 | Added `PoolMachine::idle_closed` and `PoolDriver::idle_closed` for actual host-observed disposal. It accepts Idle or already Closing state, removes the scoped entry and reschedules; Opening or Leased returns `WrongState`. | Releasing a connection to Idle and then reporting loss removes it, rejects duplicate/foreign IDs, and forces a new Connect. When checkout wins first, both ready-unclaimed and taken leases remain live and the callback returns `WrongState`. When eviction wins concurrently, actual disposal removes the Closing reservation and wakes the waiter without requiring a redundant Close. | **Closed** |
| Code P2-2 | Replaced the ambiguous string with validated `Owner { session, operation }`; added exact `cancel_operation` and session-wide `cancel_session`. | S1/op-1 cancellation delivered after S2/op-1 acquires the reused connection leaves S2 live. Session cancellation covers queued, Opening, Ready and Leased work from S1 while preserving Idle entries and S2; empty/control-bearing fields are refused. | **Closed** |
| State P2-1 | Replaced `per_key` with `per_user_host`; capacity counts exact configured host plus SSH username across ports, while reuse still requires the full scheme/username/host/port key. | With `per_user_host = 1`, a second port cannot create while the first connection is Opening, Leased, Idle or Closing. Idle state is retired, Closing continues to count until actual disposal, and another username remains independently eligible subject to the host ceiling. | **Closed for Code-side changed-range verification**; the independent State-axis verdict remains separate. |

## Changed-range analysis

The correction is confined to the draft pool API, implementation, README and tests. No protocol schema, generated protocol/admission code, binding implementation or accepted stream runtime changed. The draft API intentionally changes `Config::per_key` to `per_user_host`, changes `Request::new` to require `Owner`, and replaces `cancel_owner` with two scoped cancellation verbs. A committed-source search found no legacy pool call sites outside the updated transport package; the excluded uncommitted core consumer is not part of this object.

The capacity and reuse domains remain distinct. `Key::same_user_host` compares exact host and username while ignoring port; creation and eviction both use that grouping. Reuse still compares the complete key and identity. Opening, Idle, Leased and Closing entries are all included in the capacity counts, so the rename does not introduce early release or a second accounting path.

`idle_closed` is an acknowledgement of disposal already completed by the host. It therefore may remove an Idle entry or an entry concurrently moved to Closing. It cannot remove Opening or Leased state. If an old idle observation arrives after checkout but before the checkout is taken, the state is already Leased and the callback returns `WrongState`; responsibility stays with that exclusive lease. If the lease is subsequently abandoned, cancelled or discarded and becomes Closing, accepting the disposal acknowledgement cannot steal live ownership because that lease is already invalidated.

Cancellation scans remain atomic under the pool machine. Request cancellation handles Waiting, Opening and Ready states; the entry scan closes taken Leased state. `start_closing` clears the owner, Idle entries already have no owner, and exact owner comparison includes both session and operation. The documented host precondition—stop admitting a lost session before `cancel_session`, and never reuse its binding session ID—removes the delayed-cancellation collision without an unbounded tombstone registry.

**NEW ARCHITECTURAL root cause:** none.

## 0. Evidence base

The exact tuple resolved unchanged at both start and end:

| Repository | Commit | Tree |
|---|---|---|
| gwz-transport | `e8b9a1c5408cc9ea9528939b3a602acbeb697814` | `6aedc6f4e2de3e228b03d4a66e792f94183ec429` |
| gwz-core | `5122031244bdf62eda0105aef9f7e98a0a5ce8dc` | `2e561d680a6e553b775e5e1d3469ef2aa56f34a4` |
| workspace root | `dd15b7c99a625fc84241c191caba603df4ccae02` | `299129f377240b143171315b8471f3770a49785e` |

gwz-transport remained clean. The authorized root prompts, excluded core consumer and pre-existing excluded taut generator work remained outside the reviewed object. All object content came from the immutable commits. No current-round peer report was read.

The focused review read:

- The complete generated Code prompt, workspace instructions, merged remediation plan and original Code report.
- The complete correction diff and controlling pool checkpoint.
- Design §7, applicable requirements C1–C8, Phase 2 plan and the current checkpoint.
- Corrected public pool types and validation in `src/pool/mod.rs:14–180`.
- Capacity grouping and accounting in `src/pool/machine.rs:173–210`, creation/reuse/eviction in `src/pool/allocation.rs:4–131`, disposal and cancellation in `src/pool/lifecycle.rs:66–171`, and async exposure in `src/pool/asynchronous.rs:74–117, 213–276`.
- Focused regressions in `tests/pool_regressions.rs:15–211`, async wake behavior in `tests/pool_async.rs:196–238`, and the v2 independent ledger/scheduler in `tests/support/pool_case.rs:336–469` and `tests/pool_random.rs:1–73`.
- A committed-source search for `per_key`, `cancel_owner`, old owner fields and pool consumers. No retained legacy pool caller was found.
- A path-limited baseline diff confirming no protocol, binding, generated-schema or stream-runtime change.

Commands run independently on the corrected tuple:

- `cargo test --locked`: passed all 66 ordinary/default tests, including the fixed 3,000-case stream suite, fixed 2,000-case pool suite, six focused pool regressions and the async idle-disposal wake test. The two long campaigns remained intentionally ignored.
- `cargo clippy --locked --all-targets -- -D warnings`: passed.
- `cargo fmt -- --check`: passed.
- Direct v2 pool replay with case seed `0x1234`: passed deterministically.
- Independent 10,000-case release walk with seed `0xc0de1d1e`: passed. Coverage was 244,570 connects, 2,576 reuses, 71,510 leases, 38,490 request cancellations, 40,965 late successes, 79,431 closes, 124,744 aborts, 356,405 blocked observations, 42,797 interaction transitions, 195,476 timeouts, 82,599 spontaneous idle disposals and 136,197 session cancellations.
- The lane owner additionally reported a Rust 1.95 66-test suite, package and extracted-regression tests, generated-artifact verification, direct replay and a passing 50,000-case v2 campaign at seed `0x202609195eed`. Those reports were supporting evidence, not substitutes for the attacks above.

## 2. Invariant analysis

The original dead-idle sequence no longer produces a lease for an entry the host has already disposed. The new lifecycle pair distinguishes pool-commanded cleanup (`closed`) from host-observed idle disposal (`idle_closed`), validates pool-scoped IDs, frees capacity only upon actual disposal, and preserves checkout-wins exclusivity.

The original stale-owner sequence no longer crosses sessions. Operation cancellation matches the complete structured owner, while carrier loss uses the session verb. All request phases and taken leases are covered; Idle entries and unrelated sessions remain available.

The cross-port capacity attack failed in every physical state. Creation checks the user/host aggregate, eviction selects an Idle victim from the saturated aggregate, and Closing state retains the slot until `closed` or actual `idle_closed`. Full-key reuse, identity proof checks and aggregate host limits remain unchanged.

Unchanged invariants also held: exclusive generation-bearing leases; foreign/stale token rejection; bounded request results; late connector completion cleanup; separate allocation, network, interaction, cleanup and idle clocks; compatible FIFO reuse; driver wake capacity independent of request capacity; shutdown waiting for acknowledged disposal; and explicit stream cleanup before reusable release.

The v2 randomized scheduler now includes idle loss and session cancellation, generates multiple ports and fresh sessions, and independently checks user/host and aggregate host resource counts. Its fake ledger does not replace physical-adapter qualification, but it exercises the corrected machine boundary and complete cooperative teardown.

## 3. Risks and next action

Physical SSH/HTTPS liveness detection, resource-map integration, carrier admission shutdown, credentials, native platforms and performance remain deferred. A host must call `idle_closed` only after disposing the physical resource, must stop admission before session cancellation, must mint non-reused binding session IDs, and must continue servicing pool deadlines. These obligations are explicit in the public documentation and are suitable for later adapter integration tests; they are not missing pool mechanisms.

Record this Code GO alongside the independent State re-verdict. The result accepts this corrected fake-resource pool checkpoint on the Code axis only; it does not freeze the API or accept physical transport, core/CLI integration, platform behavior or release readiness.
