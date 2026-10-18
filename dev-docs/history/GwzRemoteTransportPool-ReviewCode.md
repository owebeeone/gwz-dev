# GWZ REMOTE TRANSPORT ENDPOINT CONNECTION POOL — CODE-AXIS REVIEW

**Review object:** Endpoint pool implementation at gwz-transport `554384f5808264009b3af016452290cacefab855`, covering the change from accepted stream baseline `aa9ecae65d6c0d568c5f4d738f9930d49f684f56`, including README and tests; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPoolImplementation.md` at `3522a1cc11991d81f9376c6ff134900233d3b4c1`; implementation checkpoint, no API/schema freeze, dated 2026-09-19.  
**Baseline:** gwz-transport `554384f5808264009b3af016452290cacefab855`; gwz-core `3522a1cc11991d81f9376c6ff134900233d3b4c1`; workspace root `49d6f76d005e1898e253fc16243f0f20291c05af`. Committed sources were read with `git show PIN:path` and the pool range with `git diff aa9ecae65d6c0d568c5f4d738f9930d49f684f56..554384f5808264009b3af016452290cacefab855`.  
**Date:** 2026-09-19  
**Axis:** Code — architecture, interfaces, call graphs and compatibility reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — zero P0, zero P1 and two P2 findings block the pool checkpoint. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

The exact tuple resolved unchanged at both start and end:

| Repository | Commit | Tree |
|---|---|---|
| gwz-transport | `554384f5808264009b3af016452290cacefab855` | `298e33e4e22a83bfdf603c79fc85be8410f74aee` |
| gwz-core | `3522a1cc11991d81f9376c6ff134900233d3b4c1` | `23a960f58c67bc6a648acc6c1ca14efc61e9d601` |
| workspace root | `49d6f76d005e1898e253fc16243f0f20291c05af` | `8e26944cc0cb08c7d364cefde44fb6eb8240ecbf` |

gwz-transport remained clean. The authorized root prompts, excluded core consumer, and pre-existing excluded taut generator work were present but outside the reviewed object. All reviewed content came from the immutable commits. No current-round peer report was read.

The review read:

- The complete generated Code prompt, workspace instructions, applicable process rules and current checkpoint.
- The complete pool checkpoint, design §7 plus its session-staleness and failure rules, applicable requirements C1–C8, and Phase 2 plan.
- Every new pool source module: public types, machine state, allocation, lifecycle, clocks and asynchronous facade.
- The complete README pool contract and every deterministic, asynchronous, stream-integration and randomized pool test.
- The independent fake-resource ledger and randomized scheduler.
- The full 2,565-insertion pool diff. A targeted diff check confirmed no changes to the taut schema, generated protocol/admission code, binding, codec or accepted stream runtime.

Commands run independently:

- `cargo test --locked`: 59 ordinary/default tests passed, including the 2,000-case pool walk, 3,000-case stream walk and the pool/stream ownership case; two long campaigns remained intentionally ignored.
- `cargo clippy --locked --all-targets -- -D warnings`: passed.
- `cargo fmt -- --check`: passed.
- Direct pool replay with `GWZ_POOL_MC_CASE_SEED=0x1234`: passed.
- An independent 10,000-case release walk with run seed `0x5eedc0de`: passed. Coverage was 246,015 connects, 1,138 reuses, 44,335 leases, 53,559 cancellations, 51,821 late successes, 98,278 closes, 161,576 aborts, 358,528 blocked observations, 41,956 interaction transitions and 76,709 timeouts.
- The lane owner additionally reported a Rust 1.95 suite, Rust 1.96 package/clippy checks, four verified generated artifacts and a passing 50,000-case pool campaign at seed `0x202609195eed`. Those reports were supporting evidence, not substituted for the attacks above.

## 1. Findings

### [P2-1] The host cannot report a physical connection that dies while idle, so the pool re-leases a resource it knows nothing about

**Location:** Design §7.2 requires allocation to remove expired or broken idle entries before reserving an eligible connection (`GwzRemoteTransportDesign.md:500–505`). The host owns the actual connection objects and acknowledgements under the pool checkpoint. In code, `PoolMachine::closed` accepts only `State::Closing` after a pool-generated Close or Abort (`src/pool/lifecycle.rs:133–146`). There is no loss/broken callback for an Idle entry. Allocation treats every matching Idle entry as live and immediately assigns it (`src/pool/allocation.rs:15–45`).

**Violated invariant:** Once the physical-resource owner has observed that an idle connection is closed or unusable, that resource must leave the pool before it can satisfy another checkout. The pool and host resource ledgers must have a complete lifecycle pair for both pool-initiated and externally observed closure.

**Reproduction/state sequence:**

1. Connect successfully, take the lease, and release it as `Reusable`. The entry becomes Idle.
2. While idle, the remote peer closes the physical SSH/HTTPS connection, or the host’s liveness mechanism otherwise proves it unusable. The host disposes the physical object.
3. Calling the only disposal acknowledgement, `closed(connection)`, returns `Error::WrongState` because the entry is Idle. `connected`, `release` and owner cancellation cannot remove this idle entry either.
4. Submit a compatible request before idle expiry. `schedule` matches the Idle key, identity and reusable flag and marks it Leased.
5. `take` or the async Checkout returns a live lease for the same `ConnectionId`, although the host has no corresponding physical resource.

The host can recover only by adding an undocumented caller-level liveness/retry loop: accept the dead lease, discard it, consume the pool-generated Close for an already closed resource, acknowledge that close, and issue another checkout. Until then the dead entry also consumes key, host and endpoint capacity.

**Impact:** The public host boundary cannot keep pool state synchronized with physical reality. A checkout can resolve to a connection already known dead, contrary to the design’s broken-idle removal step. Adapters must invent a retry protocol outside the pool, and capacity can remain occupied until reuse, eviction or the 60-second idle timer.

**Required correction:** Add an explicit host callback for spontaneous resource loss, or broaden a clearly named completion verb to handle an already disposed Idle resource. It must validate the pool-scoped token, remove an Idle entry atomically, reschedule/wake waiters, and define races with concurrent checkout: loss winning before assignment prevents reuse; assignment winning transfers responsibility to the exclusive lease and must not let a stale callback steal it.

**Closure/regression test:** Release a proven connection to Idle, report physical loss, and assert that capacity is freed and the next compatible request emits a new Connect rather than receiving the old ID. Cover waiting-at-capacity wakeup, foreign/duplicate loss callbacks, and both orderings of idle-loss versus checkout.

### [P2-2] An unscoped string conflates carrier and operation ownership, allowing stale cancellation to close a new session’s lease

**Location:** The design requires a fresh carrier session identifier and states that late activity from an old session cannot release another stream’s lease (`GwzRemoteTransportDesign.md:79–82`). `Request.owner` is instead an arbitrary bounded `String`, documented as either “Operation/carrier ownership” (`src/pool/mod.rs:124–156`). That text is copied into opening and reused entries (`src/pool/allocation.rs:29–37,69–92`). `cancel_owner(&str)` cancels every request and active lease with equal text (`src/pool/lifecycle.rs:79–101`; async exposure at `src/pool/asynchronous.rs:107–113`). The README likewise says the verb cancels one “operation/carrier” without defining a unique namespace (`README.md:126–128`).

**Violated invariant:** Cancellation from one carrier session must be scoped so it cannot affect a stream created by a later session. Operation identifiers are only required to be unique within their carrier session; carrier and operation cancellation are distinct ownership scopes.

**Reproduction/state sequence:**

1. Carrier session S1 opens operation `"op-1"` and supplies `"op-1"` as the documented operation owner.
2. S1 terminates. Its carrier-loss cancellation is delayed.
3. Fresh carrier session S2 legally reuses operation identifier `"op-1"` and obtains an active lease from the same endpoint pool.
4. The delayed S1 cleanup calls `cancel_owner("op-1")`.
5. The equality scans match S2’s request/entry, invalidate its lease, and initiate Close, even though all pool-generated request, connection and lease IDs are otherwise instance-scoped.

Using the carrier-session string instead avoids this particular collision, but the API and documentation expressly permit operation ownership and provide no structured way to distinguish the two scopes.

**Impact:** A stale carrier or operation cleanup can interrupt an unrelated later exchange, including an active Git exchange whose remote effect may already be uncertain. The pool’s scoped token protection is bypassed at its group-cancellation boundary.

**Required correction:** Give ownership an unambiguous session scope. Suitable shapes include a structured owner containing the fresh carrier session plus operation identifier, separate `cancel_session` and operation-cancellation verbs, or pool-minted opaque owner generations. Do not use one freely reusable string for both scopes. The public contract must state which lifecycle event each verb cancels.

**Closure/regression test:** Create two fresh session owners with the same operation identifier, acquire a lease for the second, then deliver delayed cancellation for the first. The second request and lease must remain live. Also prove that carrier cancellation still removes every request/lease belonging to that carrier while preserving idle resources and other sessions.

## 2. Invariant analysis

The following attacks held:

- **Capacity accounting:** Opening reservations, Idle, Leased and Closing entries all count under endpoint, host and key ceilings. Slots are not released merely because Cancel, Close or Abort was issued.
- **Exclusive leases:** Pool-scoped connection IDs and generation-bearing lease IDs prevent duplicate, stale and foreign lease operations. Ready-but-unclaimed results retain both request and connection capacity.
- **Identity eligibility:** SSH ambient and explicit proofs are separated; exact proof mismatch fails closed; unproven successful connectors are single-use. HTTPS reuse carries no authenticated-account claim.
- **Fair allocation:** Compatible idle reuse precedes creation and eviction. FIFO-compatible waiters progress even when an earlier waiter requires a different identity. Each blocked waiter tracks at most one eviction victim.
- **Cancellation and late completion:** Unsent reservations cancel without host work. Sent connectors retain capacity through cancellation; late success enters bounded cleanup and cannot become a lease. Reported connect failures preserve code/effect and are not retried.
- **Deadline separation:** Allocation time runs until Connect dispatch. Connect-network time pauses during helper interaction; repeated interactions share one interaction allowance. Cleanup deadlines survive late connector success. Idle time begins only at healthy release and never expires a leased connection.
- **Shutdown and driver loss:** New requests refuse, pending callers wake, active/idle resources enter cleanup, and shutdown remains incomplete until physical disposal is acknowledged. Abort does not pretend disposal already happened.
- **Async ownership:** Pool clones share one endpoint state; Checkout drop abandons pending or ready results; Lease drop discards; the final Pool owner initiates shutdown. Driver wake capacity is independent of request capacity, and wake callbacks execute outside the mutex.
- **Stream boundary:** The pool neither infers stream success nor returns a lease to Idle automatically. The fake integration releases only after endpoint stream cleanup. No schema, carrier, socket, credential resolution, CLI/core API or physical adapter was introduced.
- **Randomized evidence:** The independent fake-resource ledger checks exclusive allocation, resource counts, key/host ceilings, late success, cancellation, interaction timing and complete cooperative shutdown under deterministic replay.

The two failed attacks concern missing boundary shapes rather than deferred physical implementation: the host needs a way to report an idle resource’s real closure, and carrier/operation cancellation needs an explicit session scope.

## 3. Risks and next action

Physical SSH/HTTPS behavior, liveness detection mechanics, credentials, carrier integration, native-platform qualification, publication and performance remain deferred. The findings do not require implementing those components; they require complete and correctly scoped callbacks so those components can integrate without inventing contradictory lifecycle rules. The pool API remains draft-stage and unfrozen.

The next action is one bounded pool remediation: add the externally observed idle-loss transition and replace the ambiguous owner string/cancellation verb with a session-scoped ownership contract. Add the four race/isolation tests described above, rerun the existing deterministic and seeded campaigns, and return the revised immutable tuple for a focused Code re-verdict.
