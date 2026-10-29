# Step 1.5 profile and partition: Code-axis re-verdict, round 2

**Verdict: GO.** All three blocking (P2) findings from round 1 are closed and I found no new defects. One round-1 P3 (the contract §15 record) is still open; the remediation plan has the lane owner write it at merge, and it does not gate.

**Review object:**
- gwz-transport `bd7cf4f6..9fbe7448`: `adf808d`, merge `617afa9`, `9fbe744`. The range also merges step 1.6's `a21e972`, which I read only where it touches my findings.
- gwz-core `d8dd9625..aa7eaff5`, with the pin moved to `9fbe7448`.
- Evidence `5df539cd`, folder `runs/2026-10-25-p1-5/rem1/`.

**Tuple:** identical at start and end. gwz-transport is clean and `9fbe7448` is on `origin/main`; gwz-core is clean. The only dirty entry in the evidence repo is the same unrelated untracked alpha-run folder as in round 1.

**What I ran (load 6 to 11):**
- `cargo test -p gwz-connection` with `CARGO_TARGET_DIR=…/p1-profile-crate`: **70 passed**.
- The gwz-core candidate build `cargo +1.95.0 test --lib --locked endpoint_environment`: **18 passed**, including the new profile, encoder, trust and bypass rows.

I read the evidence README in `rem1/` and the mutation summaries.

## Prior-finding closure

| ID | Status | Evidence |
| --- | --- | --- |
| **P2-1** Retry after a lease of another scope: new non-fresh request, lost FIFO place, reset deadline, unsound bound | **Closed** | `instance.rs`: the opening's deadline is now fixed at `submit` (`Pending.deadline`, after `pool.advance(now)`). A lease of another scope goes back untouched as `Reusable` and is not retired. The opening then asks again as a `fresh` request through `request_until(…, Some(pending.deadline))`; `ask_again` does the same after a failed 1.6 proof. HTTPS requests now carry a scope-derived pool identity (`Identity::HttpsScoped("scope-N")`), so the pool's idle match never offers another scope's connection. A misfit can then only come from a broken scope record, and the cap remains as a guard. Closure rows pass: `a_misfit_keeps_the_other_scopes_connection_and_opens_its_own`, `a_misfit_opening_times_out_at_its_original_allocation_deadline`, `a_failed_reuse_proof_resubmits_under_the_original_deadline`, and `under_saturated_load_of_one_scope_another_scope_is_served_after_at_most_one_misfit`, which is my sequence run for `per_user_host + 2` cycles: never `NoFittingConnection`, at most one misfit. The remaining wait under saturated same-key load ends in `AllocationTimeout` at the original deadline. It is recorded as a limitation in the module doc and matches how the pool already treats requests with a different identity. |
| **P2-2** Dropping an opening after a misfit addressed a stale `RequestId` | **Closed** | `OpenWait` now holds only the slot. `Instance::abandon` finds the opening with `Arc::ptr_eq` on the slot and abandons that opening's current request; it also returns any lease under a running reuse proof as `Reusable`. Row `dropping_an_opening_after_a_misfit_leaves_no_request_context_or_exchange` passes: the provider's strong count is back to 2, `outstanding_for_test() == 0`, `exchanges == 0` after further pumps, and the count is 1 after `finish`. Mutation M4 is now killed (`rem1` README, round 3: 12 of 12 killed). |
| **P2-3** Windows machine-proxy `Debug` text pushed the profile over its 16 KiB bound | **Closed** | `endpoint_environment.rs`: the machine setting is encoded from its parsed inputs (`machine_canonical`, `Bypass::canonical`) in fields 12, 15 and 16, never from `Debug`. Each of the 17 fields has a maximum, and a longer value is kept as its SHA-256 with a marker byte. `MAX_PROFILE_BYTES` is raised to 64 KiB and made `pub`. `the_field_maxima_fit_the_profile_bound` holds the sum statically: about 47.1 KB including framing. The platform-neutral rows `a_long_machine_bypass_list_fits_the_profile_and_each_rule_partitions` (8192-character list) and `a_long_bypass_list_fits_and_one_host_changes_the_profile` (16 KiB `NO_PROXY`) pass. Mutations CM6 and CM8 are killed. Windows receipt `p1-5-rem1-20261025-c` passed. |
| **P3-1** Interface obligations on the trunk lane not recorded where it will read them | **Open (non-gating; scheduled at merge)** | `ports.rs` now documents the identity echo rule (`ConnectCommand.identity`) and the retention rule for `ConnectCommand.auth`. Neither the lane's nor the root's `GwzConnectionApiContract.md` has the step 1.5 row in §15 yet, and the plan has no 1.3c route-naming text. The remediation plan says the lane owner records both at merge. That record must also cover the three items in "Text the §15 row must add" below. |
| **P3-2** Auth scope applied to every scheme, so SSH was partitioned by the helper environment | **Closed** | Lane owner's ruling: the scope check applies to HTTPS only (`!setup.https \|\| …`), and the pool identity is scoped only for HTTPS. Row `ssh_connections_are_not_partitioned_by_the_auth_context` passes. SSH reuse across bindings is now gated by step 1.6's `authorize_reuse`. |

## New findings

None. These attacks failed:

- **Scope tokens.**
  - A connection whose pool identity is scoped but whose recorded setup scope is `None` would misfit for every opening of that scope and keep coming back idle. This is unreachable. The pool hands the tag back on `Connect` whenever the opening request still exists, and it removes an opening entry outright if it is cancelled before `Connect` (`lifecycle.rs:94-95`). `submit` pumps under the same lock that holds the pending opening, so the tag lookup always succeeds.
  - Pruned scope tokens are never reissued, so an orphaned connection's token matches nothing.
- **Misfit returned as `Reusable`.** The lease was taken (`take` clears the request) and nothing was started on it, so `release` accepts it. A `fresh` resubmission skips idle connections, so it cannot loop back onto the same one.
- **Proxy credentials.** `refuse_proxy_authorization` closes the remaining path (killed by mutation CM7). The profile inputs still carry no helper environment.
- **Platform and style rules.** The new `cfg_if!` blocks are `cfg(any(windows, test))` with a matching Unix else-branch (`machine_canonical` on an uninhabited type) or `cfg(test)` only. All control flow is braced. No globals or thread-locals. The test helpers live under `src/`. The large test file was split (`profile_partition.rs`, `host.rs`, `ssh_home_rows.rs`).

## Text the §15 row must add (not findings)

1. **Per-field digests.** Fields longer than their maximum are compared by SHA-256, so for those fields a SHA-256 collision would count as equality. That narrows §7's rule that "a digest collision is not equality", which until now applied only to the index digest. This extends the CA-content digest I accepted in round 1. Record it as a §7 amendment.
2. **Two kinds of scope on HTTPS identities.** The instance now gives every HTTPS request `HttpsScoped("scope-N")`. The pool's existing `scope_https` (§7.2's per-connection scope for NTLM/Negotiate) refuses a lease that is already scoped differently. The crate exposes no way for an adapter to scope a lease, so nothing is reachable today. But §15 must state how §7.2's per-connection security scope will combine with the operation scope before 1.3c brings in Negotiate.
3. **Adapter conformance.** The trunk adapters (`ssh_adapter.rs`, `connection_factory.rs`, 1.3c `https_adapter.rs`) must report `command.identity` as their `Connected` proof, or report `None`. Any other proof fails the connect as an identity mismatch.

Observation, not a finding: on OpenSSL builds every `capture_operation` call now reads and hashes the platform root store. That is a small per-entry cost which the evidence does not measure.
