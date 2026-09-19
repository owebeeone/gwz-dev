# GWZ REMOTE TRANSPORT ENDPOINT CONNECTION POOL — STATE-AXIS FOCUSED RE-VERDICT

**Review object:** Endpoint pool implementation at gwz-transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`, covering the immutable range from accepted stream baseline `aa9ecae65d6c0d568c5f4d738f9930d49f684f56`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPoolImplementation.md` at `5122031244bdf62eda0105aef9f7e98a0a5ce8dc`; remediation round 1, dated 2026-09-19, with no API freeze or physical transport.  
**Baseline:** gwz-transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`; gwz-core `5122031244bdf62eda0105aef9f7e98a0a5ce8dc`; workspace root `dd15b7c99a625fc84241c191caba603df4ccae02`. Sources were read from immutable objects using `git show PIN:path` and `git diff 554384f...e8b9a1c`.  
**Date:** 2026-09-19  
**Axis:** State — state machines, concurrency, failure and recovery, including conservative ownership under adverse host timing. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — the original State P2 finding is closed, and the correction introduces no new P0, P1, P2 or P3 finding.

---

## Prior-finding closure

| ID | Disposition | Original counterexample verification | Status |
|---|---|---|---|
| State P2-1 | `per_key` is replaced by `per_user_host`; the capacity group compares exact configured host and SSH username across ports, while the complete scheme/username/host/port key remains the reuse key. Creation and eviction both use the new group. | With `per_user_host=1`, `per_host=4`, `total=4`, `git@host:22` prevents a second Connect for `git@host:2222` while the first resource is Opening, Leased, Idle pending retirement, or Closing. A different username can allocate. The alternate-port request allocates only after actual disposal acknowledgement. | **CLOSED** |

## Changed-range analysis

- **Cross-port capacity:** `Config.per_user_host` is defined at `src/pool/mod.rs:14-38`; `Key::same_user_host` groups exact host and username independently of port at `:64-103`; `counts_for_user_host` counts every physical state at `src/pool/machine.rs:173-210`; creation and eviction enforce the same group at `src/pool/allocation.rs:47-57,95-129`. The four-state regression is at `tests/pool_regressions.rs:15-75`.
- **Spontaneous idle disposal:** `idle_closed` accepts only Idle or already-Closing state, removes the entry only on an acknowledgement of actual disposal, reschedules waiters, and rejects Opening or Leased state at `src/pool/lifecycle.rs:142-170`. The async facade performs the same mutation under the shared mutex at `src/pool/asynchronous.rs:242-256`. Deterministic tests cover loss-before-checkout, checkout-before-loss, eviction/loss overlap, foreign and duplicate callbacks, capacity wakeup and replacement at `tests/pool_regressions.rs:77-132` and `tests/pool_async.rs:196-238`.
- **Cancellation scope:** requests carry structured `Owner { session, operation }` at `src/pool/mod.rs:128-181`. `cancel_operation` compares the complete owner, while `cancel_session` matches only its session; both cancel queued/opening/ready work through `fail_request`, close matching taken leases, retain ownerless Idle entries, and reschedule at `src/pool/lifecycle.rs:24-110`. Tests cover delayed cancellation from S1/op-1 against S2/op-1, all request states, active leases, retained idle resources, other sessions and invalid owner fields at `tests/pool_regressions.rs:134-211`.
- **Combined behavior:** all three changes mutate the same machine under one serialization boundary. Removed Idle/Closing entries immediately participate in rescheduling; cancellation leaves physical reservations counted until connector settlement or actual disposal; user/host accounting includes those retained states. No fix creates a second capacity ledger, tombstone registry, retry path or false resource-disposal claim.
- The v2 randomized ledger independently aggregates resources by host and username across ports and adds idle-loss and session-cancellation events at `tests/support/pool_case.rs:24-73,336-451`.

**NEW ARCHITECTURAL ROOT CAUSES: none.**

## 0. Evidence base

The exact tuple was verified at the beginning and end of the review. Both checks returned:

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `dd15b7c99a625fc84241c191caba603df4ccae02` | `299129f377240b143171315b8471f3770a49785e` |
| gwz-core | `5122031244bdf62eda0105aef9f7e98a0a5ce8dc` | `2e561d680a6e553b775e5e1d3469ef2aa56f34a4` |
| gwz-transport | `e8b9a1c5408cc9ea9528939b3a602acbeb697814` | `6aedc6f4e2de3e228b03d4a66e792f94183ec429` |

No reviewed tracked file changed. `gwz status` showed only the authorized prompt outputs and explicitly excluded taut-generator/core-consumer work. Build output remained ignored. No current-round peer prompt or report was read.

The review read:

- The complete focused State prompt, merged remediation plan and prior-round Code and State reports.
- The exact transport correction diff `554384f5808264009b3af016452290cacefab855..e8b9a1c5408cc9ea9528939b3a602acbeb697814`.
- All changed pool source, README, deterministic, async, stream-seam, regression and randomized-test files.
- The corrected implementation checkpoint, design §§3 and 7, requirements C1–C8, Phase 2 plan and current root checkpoint.
- Unchanged machine/clock/allocation interactions where needed to retrace capacity retention, scheduling, timeout and shutdown behavior.

Commands run independently on the pinned transport tree with Rust `1.96.0`:

- `cargo test --locked`: all 66 executed tests passed; the two extended campaigns remained intentionally ignored.
- `cargo fmt -- --check`: passed.
- `cargo clippy --locked --all-targets -- -D warnings`: passed.
- Direct randomized replay with `GWZ_POOL_MC_CASE_SEED=0x1234`: passed.
- Independent 10,000-case release campaign with seed `0x51a7e20260919`: passed. Coverage was 242,967 connects, 2,557 reuses, 70,815 leases, 38,641 cancellations, 41,001 late successes, 78,945 closes, 124,335 aborts, 354,023 blocked observations, 42,786 interaction transitions, 197,653 timeouts, 81,709 spontaneous idle disposals and 136,618 session cancellations.
- `git diff --check 554384f...e8b9a1c`: passed.

The lane-owner evidence at this tuple additionally records a Rust 1.95 66-test suite, package build and twelve extracted-package tests, artifact regeneration, direct replay, and a 50,000-case v2 campaign using seed `0x202609195eed`. Those are corroborating inputs rather than substitutes for this review’s attacks.

## 2. Invariant analysis

- **User/host capacity:** The original port-bypass sequence no longer creates a second reservation. Opening, Idle, Leased and Closing entries share the same user/host bucket across ports. Disposal acknowledgement is the event that releases capacity.
- **Reuse versus capacity identity:** Reuse still requires the complete key and compatible identity. The broader capacity group does not allow cross-port reuse or merge SSH users; HTTPS occupies its no-username host bucket.
- **Eviction:** When the user/host domain is full, only an Idle entry from that domain can release it. A host-wide block similarly selects the blocked host, while endpoint-total pressure may retire any Idle victim. Each waiter still tracks at most one live eviction victim.
- **Idle-loss race:** Loss winning while Idle removes the dead resource before reuse and schedules replacement. Checkout winning changes the state to Leased atomically; a delayed `idle_closed` then returns `WrongState` and cannot steal the lease. If eviction or expiry has already changed Idle to Closing, an acknowledgement that the physical object is already disposed may remove it without a redundant Close.
- **Cancellation isolation:** A delayed session or operation cancellation from S1 cannot match S2 with the same operation name. Session cancellation spans that session’s waiting, opening, ready and taken-lease work while preserving Idle resources and other sessions. The host’s stop-admission-before-cancel rule prevents post-cancellation work from re-entering a dead session without requiring an unbounded tombstone set.
- **Capacity during cancellation:** Dispatched connectors remain Opening with cleanup state; active leases become Closing. Cancel, Close and Abort commands do not themselves free capacity. Late success cannot become a lease and retains the existing cleanup deadline.
- **Async atomicity:** State checks, `idle_closed`, cancellation, scheduling and waker collection occur under the shared mutex. Wakers are invoked after releasing it. Idle disposal changes the machine revision, waking both capacity waiters and the driver when replacement work appears.
- **Token safety:** Foreign and duplicate connection identifiers remain `Stale`; callbacks against incompatible live states return `WrongState`. Lease generations continue to prevent an old lease from releasing a later allocation.
- **Shutdown and recovery grammar:** Shutdown and driver loss still invalidate callers and transition owned resources toward cleanup without claiming physical disposal. `idle_closed` adds a bounded transition from Idle/Closing to absent state; it does not create a new stuck state.
- **Durability boundary:** This object owns no filesystem or other durable state. Crash and kill adversity reduces to conservative in-memory ownership: the pool never invents reusable health or physical disposal after host/process loss.

## 3. Risks and next action

The remaining risks are the documented host obligations: supply fresh session identities, keep an operation identifier unique through its live lifecycle and pending cleanup, stop admission before session cancellation, service clock deadlines independently, and call disposal acknowledgements only after the physical resource is gone. Physical adapters, process-loss qualification, native platforms, credentials, external integration and network behavior remain deferred.

The next action is to combine this GO with the independent Code-axis re-verdict. If both axes pass, record acceptance of this corrected draft pool checkpoint and proceed to the next authorized integration stage. This verdict does not freeze the pool API or accept a physical transport.
