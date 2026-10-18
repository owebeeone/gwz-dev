# Windows HTTPS WH1 — remediation round 2

2026-10-04. Limited WH1 implementation remains **NO-GO**.

The reviewed round-1 tuple:
- root `0d2db2c23afd83d496ca9eb55d8264bf8366314e`;
- core `261eaca55dca4067548027e8976ff0249a34d2f3`;
- evidence `053121cc97664e46539c07d77cdad4effb481955`.

[Code-1](GwzWindowsHttpsIntegrationImplementation-ReviewCode-1.md) reported GO and [State-1](GwzWindowsHttpsIntegrationImplementation-ReviewState-1.md) NO-GO. The [merged verdict](GwzWindowsHttpsIntegrationImplementation-Verdict-1.md) keeps State P2-3 open. State classifies P2-3 as an incomplete correction of the existing publication-arbitration root, not a new architectural root. This is the second and last remediation round under the cap.

| Finding | Disposition | Closure evidence |
|---|---|---|
| State P2-3: final native publication check precedes the mux publication lock | **Correct:** see the steps below the table. | See the closure-evidence list below. |

**The steps for State P2-3:**
1. **gwz-transport.** Add one neutral method to `mux::asynchronous::Owner`: `send_if(request, message, admit: impl FnOnce() -> bool) -> Result<bool, Error>`.
   - It runs `admit()` while it holds `Shared.inner`, immediately before `Mux::send`.
   - When `admit()` refuses, it queues nothing and returns `Ok(false)`.
   - The transport gains no notion of time, authentication or policy.
   - `admit` must be short and must not call back into the owner. The lock order is the mux mutex, then whatever `admit` reads.
2. **Core: which messages use it.** The pump sends an Opened whose entry carries a native publication deadline through `send_if`. The admit closure re-reads monotonic time and the entry's cancellation inside the lock, and equality counts as expired.
3. **Core: what a refusal does.** A refusal takes the existing failure path, which publishes the single Timeout (or Cancelled) outcome. That path preserves observed facts, revokes the authenticated route, discards prepared ownership and keeps the physical and native cleanup charges until disposal.
4. **Core: what stays.** The pre-lock check in `before_handoff` stays as an early exit. Every other message, and nonnative timing, keeps the plain `send`.
5. **The test seam.** A clock seam in the HTTPS endpoint lets a test choose the reading the pre-lock check sees and the reading the in-lock check sees. Production reads the same monotonic clock as before.

**Closure evidence for State P2-3.**
- **The new regression** goes through the actual asynchronous Owner/Session publication path, not a direct Mux:
  - preparation completes before D;
  - the pre-lock check reads D−ε and the in-lock check reads a time at or after D, modelling a suspension or contention between the guard and the lock;
  - it asserts no Opened, exactly one Timeout, preserved facts, the route revoked, and the physical charge kept until real disposal.
- **Fail-before:** the new regression fails on core `261eaca`, publishing Opened.
- **Controls:**
  - a pre-D success, with both readings before D, publishes Opened;
  - an equality case, with the in-lock reading exactly D, is expired.
- **gwz-transport unit tests:** `send_if` refusing queues nothing and leaves the route's state unchanged; `send_if` admitting equals `send`; `admit` runs once, while the lock is held.
- **Existing tests:** the collection, backpressure, nonnative, capacity, constructor and cleanup regressions and the portable runners stay green.

**Scope, from the operator's disposition of 2026-10-04 ("Approve send_if"):**
- The one new shared interface is `Owner::send_if`, and gwz-transport joins WH1's changed members. No other API, owner, dependency, schema, wire, helper support or ordinary activation is added.
- The file and line allowance stays at 55 source, test and build files plus the three switch inventories, and 2,600 gross added lines. The [budget disposition](GwzWindowsHttpsIntegrationBudgetDisposition.md) records it before the edits.
- Exceeding either ceiling, or any further interface, owner or platform change, stops the patch for a new disposition.

**Review.** A shared interface changes, so the round-1 proofs no longer cover the call graph. **Fresh** Code and State reviewers review round 2 on one settled tuple, with the round-1 reports, the verdict and this plan as inputs. They also re-trace State P2-3's original counterexample. No finding is closed by its implementer.

**Process.** TDD. One patch. Commit with gwz from the lane root, with `--no-commit-marker`. Builds stay outside the evidence member. No push, tag, publication or OS policy change.
