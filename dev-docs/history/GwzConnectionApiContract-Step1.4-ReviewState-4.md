# Step 1.4 State axis — follow-up check on the P2-1 re-lease call site

**Tuple:** gwz-core `4703a4e8b48bb7d1cc3a748a14fe3eb14895bd7c`, gwz-transport `a22d43cd24830db71eeb7b0fa44f72b5bf70462c`, workspace `/Volumes/projects/limbo/gwz-dev`. I checked both at the start and the end; they did not move. gwz-core's worktree was clean. I did not re-check gwz-transport's worktree.
**Diff reviewed:** `git -C gwz-core diff 408a8b24..4703a4e8`. The production change is one line, `native/exchange.rs:483`, plus the new row.
**Date:** 2026-10-30
**Verdict: GO.** P2-1 is closed at every production pool-owner site. Nothing is reopened. Findings: none.

## Closure table

| ID | Claim | Verified | Status |
|---|---|---|---|
| P2-1 (completeness: every production pool owner keyed per request) | `lease_again` now builds its owner with `prepared.input.owner()`. | **Site inventory.** Production pool `Request` construction in gwz-core has two sites. `https_pool.rs:255` takes its owner as a parameter. `ssh_worker/runner.rs:213` uses the keyed owner when both the request and its limits are present. The only `checkout_scoped` callers are `prepare.rs:240` (`input.owner()`), `throttle.rs:339` (`recheckout`, whose owner comes from `serve.rs:87` as `self.input.owner()`) and `native/exchange.rs:481` (`prepared.input.owner()`, now fixed). The `HttpsPool::checkout` wrapper is `cfg(test)` only. The other `Owner::new` hits are test code (`held.rs:109` is inside `cfg(test)`, and `retire.rs:217,227` are in its tests). The other owner types, `native/connection.rs`'s `Owner` and the mux `Owner`, are unrelated to the pool. **Windows-only cfg.** `real_sspi` is the only Windows-only item under `https_worker`, and it builds no pool owner. `lease_again` itself is not cfg-gated (it compiles here under the candidate cfg), so there is no hidden branch to miss. **Row.** `a_re_lease_is_counted_with_its_request_s_other_checkouts_under_the_request_s_limits` passes. The whole `https_worker::native` family passes, 65/65. | **Closed.** |

## Does the one-line change reopen anything? No.

- **Self-blocking at the limit.**
  - `lease_again` first discards the lapsed lease (`prepared.lease.take()`, then `finish(Discarded)`) before it checks out again.
  - A Closing entry does not count against the owner.
  - So the re-lease is not blocked by its own predecessor even when the request is at its limit. The new row shows an open at limit 2 holds a re-lease plus one other connection, and the third is refused.
- **Limit lookup.** `Owner.operation` is unchanged, so `operation_limits(&owner.operation)` finds the same limits as the request's other checkouts.
- **Last-lessee binding.** `serves_binding` reads `owner.session`. Before the fix, a scoped re-lease used the raw mux session while the request's first lease used the keyed session, so the two were different bindings. The fix makes them the same binding. It can only make a legitimate re-lease easier to serve.
- **Other state.** The authority, the reservation accounting and the retirement are untouched by this diff.

## Findings

None.
