# GWZ ENDPOINT CONNECTION POOL IMPLEMENTATION — STATE-AXIS REVIEW

**Review object:** Endpoint pool implementation at gwz-transport `554384f5808264009b3af016452290cacefab855`, covering the immutable diff from accepted stream baseline `aa9ecae65d6c0d568c5f4d738f9930d49f684f56`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPoolImplementation.md` at `3522a1cc11991d81f9376c6ff134900233d3b4c1`; implemented checkpoint awaiting review, dated 2026-09-19, with no API freeze or physical transport.  
**Baseline:** gwz-transport `554384f5808264009b3af016452290cacefab855`; gwz-core `3522a1cc11991d81f9376c6ff134900233d3b4c1`; workspace root `49d6f76d005e1898e253fc16243f0f20291c05af`. Sources were read from immutable objects using `git show PIN:path` and `git diff aa9ecae...554384f`.  
**Date:** 2026-09-19  
**Axis:** State — state machines, concurrency, failure and recovery, including conservative ownership under adverse host timing. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

The exact tuple was verified at the beginning and end of the review. Both checks returned:

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `49d6f76d005e1898e253fc16243f0f20291c05af` | `8e26944cc0cb08c7d364cefde44fb6eb8240ecbf` |
| gwz-core | `3522a1cc11991d81f9376c6ff134900233d3b4c1` | `23a960f58c67bc6a648acc6c1ca14efc61e9d601` |
| gwz-transport | `554384f5808264009b3af016452290cacefab855` | `298e33e4e22a83bfdf603c79fc85be8410f74aee` |

`gwz status` reported only the prompt outputs and the explicitly excluded taut generator/core-consumer work named by the dispatch. No reviewed tracked file changed during this review. Build output remained ignored. No current-round peer report was read.

The review read:

- Root `CurrentProgramCheckpoint.md`, `AgentProcessRules.md`, and `GwzProcessOptimization.md`.
- The complete `GwzRemoteTransportPoolImplementation.md:1-91`.
- Pool authority in `GwzRemoteTransportDesign.md:458-526`, requirements C1-C8 in `GwzRemoteTransportRequirements.md:180-211`, and Phase 2 in `GwzRemoteTransportPlan.md:161-202`.
- The complete transport diff from `aa9ecae65d6c0d568c5f4d738f9930d49f684f56` to `554384f5808264009b3af016452290cacefab855`.
- Every pool implementation module: `src/pool/mod.rs`, `machine.rs`, `allocation.rs`, `lifecycle.rs`, `clock.rs`, and `asynchronous.rs`.
- Pool documentation and all pool tests, including the fake-resource ledger and stream/lease ownership seam. Existing stream code was inspected only where needed to assess that seam.

Commands run independently on the pinned transport tree with Rust `1.96.0`:

- `cargo test --locked`: passed all 59 executed tests; the two extended randomized campaigns remained intentionally ignored.
- `cargo fmt -- --check`: passed.
- `cargo clippy --locked --all-targets -- -D warnings`: passed.
- Direct replay with `GWZ_POOL_MC_CASE_SEED=0x1234`: passed.
- `GWZ_POOL_MC_SEED=0x57a7e20260919 GWZ_POOL_MC_CASES=10000 cargo test --locked --release --test pool_random seeded_pool_lifecycles -- --exact --nocapture`: passed 10,000 cases. Coverage was 245,853 connects, 1,195 reuses, 44,593 leases, 53,924 cancellations, 52,139 late successes, 98,152 closes, 161,928 aborts, 361,948 blocked allocations, 41,468 interactions, and 76,187 timeouts.
- `git diff --check aa9ecae...554384f`: passed.

The lane-owner evidence at the same tuple additionally records a Rust `1.95` full suite, Rust `1.96` clippy/package validation, pinned regeneration of four artifacts, and a passing 50,000-case campaign with seed `0x202609195eed`. Those are corroborating inputs rather than this review’s independently executed evidence.

## 1. Findings

### [P2-1] The per-user/host ceiling is enforced per full reuse key, so a port change bypasses it

**Location.** The requirements mandate connections “bounded per user/host,” separately from the aggregate per-host ceiling (`GwzRemoteTransportRequirements.md:195-199`). The design and Phase 2 plan likewise define the first starting ceiling as eight per user/host and the second as eight per host across users, ports, and schemes (`GwzRemoteTransportDesign.md:485-498`; `GwzRemoteTransportPlan.md:170-183`).

The implementation replaces the first domain with `Config.per_key` (`src/pool/mod.rs:14-38`). A `Key` contains scheme, username, host, and port (`:63-69`), `counts_for_key` compares that entire value (`src/pool/machine.rs:176-181`), and creation and eviction enforce `per_key` through that full comparison (`src/pool/allocation.rs:47-57,95-128`). The implementation checkpoint and README repeat “per key” (`GwzRemoteTransportPoolImplementation.md:44-50`; `README.md:105-111`), but no requirements/design amendment authorizes that change.

**Violated invariant.** The user/host ceiling must constrain one SSH username at one configured host across effective ports. Port and scheme distinctions belong to reuse eligibility and the separate aggregate host domain; they must not create extra buckets under the user/host ceiling.

**Reproduction.**

1. Construct a pool with `per_key = 1`, `per_host = 4`, and `total = 4`.
2. Submit an ambient SSH request for username `git`, host `example.test`, port 22.
3. Submit another ambient SSH request for the same username and host at port 2222.
4. The first reservation has full key `(ssh, git, example.test, 22)` and the second has `(ssh, git, example.test, 2222)`.
5. `counts_for_key` reports zero for the second full key. Host count is one, below four, so `schedule` creates a second opening reservation and emits a second `Connect`.
6. Two physical connections for the same user/host are therefore opening concurrently despite the configured first-domain ceiling of one.

The same bypass persists across opening, idle, leased, and closing states because all four are counted under separate full keys.

**Impact.** When an endpoint intentionally configures a larger aggregate host allowance than its per-user allowance, one username can consume multiple port buckets up to the host-wide or endpoint-wide ceiling. The implementation remains finitely bounded, but it does not enforce the specified capacity policy and can crowd out other users while reporting that both configured domains hold.

**Required correction.** Keep the full `(scheme, username, host, port)` key for reuse compatibility, but introduce the specified capacity grouping for the first ceiling. For SSH, count the same username and exact configured host together across ports. Rename the public setting to describe that domain, or amend and re-review the requirements/design if a full-key ceiling is actually intended. Eviction must select a victim that releases the blocked user/host domain when that ceiling is full.

**Closure test.** With first-domain limit 1, host limit 4, and total 4:

1. Open `git@example.test:22`.
2. Request `git@example.test:2222`; assert that it waits and produces no second `Connect`.
3. Request a different username at the same host; assert that it may allocate under the host-wide allowance.
4. Repeat while the first connection is idle, leased, and closing, proving all physical states count against the same user/host ceiling.
5. After acknowledged closure, assert that the waiting alternate-port request can allocate.

## 2. Invariant analysis

Apart from P2-1, the attacked state invariants held:

- **Exclusive allocation.** Each successful checkout receives a fresh lease generation. A connection cannot be assigned while opening, leased, or closing. Retained stale lease tokens cannot release a later lease.
- **Host-wide and endpoint-wide capacity.** Opening reservations, idle connections, leases, and closing resources remain in `entries` and count until actual completion acknowledgement. The separate host and total ceilings held throughout deterministic and randomized schedules.
- **Cancellation before dispatch.** Cancelling an opening reservation before `Connect` is taken removes it without host work and immediately releases capacity.
- **Cancellation after dispatch.** Once `Connect` is transferred to the host, cancellation retains the reservation, emits `CancelConnect`, and later emits `AbortConnect` at the cleanup deadline. It never assumes that either command disposed the resource.
- **Late connector completion.** Late failure releases the reservation only under the documented acknowledgement that the host has disposed partial state. Late success after cancellation becomes closing state and cannot become a lease; an already-expired cleanup deadline produces immediate abort work.
- **Connection failure semantics.** A live connector failure preserves its exact `ErrorCode` and `Effect` and does not retry the request.
- **Identity isolation.** Ambient, explicit-proof, and HTTPS eligibility are distinct. A mismatched proof fails the request and closes the resource. An unproven resource can be leased once but cannot enter the reusable idle pool.
- **Fair eligible scheduling.** Waiting IDs are traversed in order. Compatible idle reuse precedes creation and eviction, while an incompatible head cannot consume an idle resource eligible for a later waiter.
- **Bounded request results.** Waiting, opening, ready, and failed-but-unclaimed results all occupy the configured request bound. Taking or abandoning a result releases that slot.
- **Idle expiry and checkout atomicity.** Healthy release records the current monotonic time. Checkout removes idle state under the same machine mutation. A stale clock tick sees the current leased or newly released state and cannot reclaim an active lease.
- **Independent timeout domains.** Allocation time ends when `Connect` is taken. Network time pauses during helper interaction, repeated interactions share the original interaction allowance, and cleanup has its own deadline.
- **Owner loss.** `cancel_owner` cancels that owner’s requests and active leases while retaining idle entries and other owners’ leases.
- **Shutdown.** Shutdown refuses new requests, fails pending results, invalidates leases, and transitions physical resources to cleanup. Close/abort commands do not free capacity; `shutdown_complete` remains false until disposal acknowledgement.
- **Async registration.** Request state checks and waker installation occur under one mutex. Driver wake capacity is separate from request capacity. Replaced wakers, wake callbacks, and destructor callbacks execute outside the lock. Dropping an unclaimed checkout cancels it; dropping a lease discards its connection; dropping the final `Pool` owner initiates shutdown.
- **Driver loss.** Pending callers wake with `DriverLost`, active leases become stale, and the machine makes no false claim that physical resources were disposed. The documented host obligation to tear down its resources before dropping the driver is necessary and explicit.
- **Stream ownership seam.** The fake integration retains the lease until stream/backend cleanup is proven, and reuse receives a new lease generation only after explicit healthy release.
- **Closed recovery grammar.** Foreign, duplicate, and late request, connection, and lease tokens return typed stale/wrong-state errors without reopening or releasing another resource.

This object has no durable filesystem state. Crash persistence and write-order attacks reduce to conservative in-memory ownership: after driver/process loss, the machine invalidates callers and never invents physical disposal or reusable health.

## 3. Risks and next action

The pool depends on the host executing transferred actions, servicing clock deadlines independently, resolving explicit identity proofs before checkout, and acknowledging disposal only after the physical object is gone. Those responsibilities are explicit at this checkpoint and the implementation remains conservative when acknowledgements are absent.

The randomized ledger exercises all ten declared event classes and complete cooperative teardown, but physical connector, helper, socket, and process-kill behavior remains deferred. No network, native-platform, API-freeze, or production-integration acceptance follows from this review.

The next action is one bounded capacity-domain correction for P2-1, with the cross-port user/host regression above, followed by a focused State re-verdict on the corrected tuple.
