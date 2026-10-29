# Step 1.5 profile and partition: State-axis re-verdict, round 2

**Verdict: GO.** All three of my round-1 P2 findings are closed. One new P3 (N-P3-1). Two P3 items are carried to the merge as the remediation plan records.

**Tuple:** gwz-transport `9fbe7448`, gwz-core `aa7eaff5`, evidence `5df539cd`. It matched at both start and end, and all three trees are clean. The evidence member still has the untracked `runs/2026-09-22-alpha-setup-timeout/` directory that was there before this review. gwz-core `d8dd9625` has the same tree as `aaf6efb4`; only the commit message was amended.

**Runs at load 5:**
- `cargo test -p gwz-connection`: 70 passed.
- gwz-core candidate filter `endpoint_environment`: 18 passed.
- Linux run d: the trust row passes there.
- CM9 (platform roots left out of the profile) is killed on Linux (`raw/remote/m9`). It survives on macOS, which does not compile that branch.
- Windows receipt `-c`: PASS.

## Prior-finding closure

| ID | Status | Evidence |
|---|---|---|
| **S-P2-1** (misfit retires the other scope's connection and resubmits) | **Closed**, with a recorded limitation | **(a) Retire with free capacity:** HTTPS requests now go to the pool as `Identity::HttpsScoped("scope-N")` (`instance.rs` `pool_identity`), so the pool only offers an opening idle connections of its own scope. Tokens are never reused and are pruned once nothing uses them. SSH is not partitioned by scope; the lane owner ruled that, and cross-binding SSH reuse waits for 1.6's `authorize_reuse`, whose default is to refuse. Row `a_misfit_keeps_the_other_scopes_connection_and_opens_its_own`; mutant M8 killed (`round4b`).<br>**(b) Deadline and FIFO:** the deadline is fixed at `submit`. A misfit hands the lease back as `Reusable`, then asks again once with `fresh` through `request_until(…, Some(deadline))`. `ask_again` after a failed reuse proof does the same. Rows `a_misfit_opening_times_out_at_its_original_allocation_deadline` and `a_failed_reuse_proof_resubmits_under_the_original_deadline`.<br>**(c) Spurious failure:** row `under_saturated_load_…_after_at_most_one_misfit` runs `per_user_host + 2` cycles with no `NoFittingConnection`.<br>**Limitation:** sustained one-scope saturation can starve another scope until its original deadline, which then ends as `AllocationTimeout`. That is the pool's existing rule for requests whose identity does not match, it is bounded and typed, and it is documented in `instance.rs`. It should go on the operator's decision list rather than stay only in the module doc. |
| **S-P2-2** (profile bound refuses long bypass lists) | **Closed** | Every field now has a maximum; a longer value is kept as its SHA-256 behind a marker byte. The machine setting is encoded from its parsed inputs (`machine_canonical`, `Bypass::canonical`), which are length-prefixed and contain no `Debug` text. Rows: `the_field_maxima_fit_the_profile_bound`, a 16 KiB `NO_PROXY`, and an 8192-character machine bypass list that is captured and partitions on each rule. CM6 and CM8 killed. |
| **S-P2-3** (OpenSSL platform-root content not in the profile) | **Closed** | `platform_trust` reads the roots once at capture. Field 17 is a SHA-256 over their DER, each length-prefixed, and the loader returns exactly the captured certificates. Row `the_trust_content_behind_ssl_cert_file_is_in_the_profile`: same path with new content gives a new instance, and restoring the content reuses the first one. It passes on Linux in run d, and CM9 is killed there. |
| S-P3-1 (proxy authorization) | Closed | `refuse_proxy_authorization` refuses it in `capture_operation`, with a row; CM7 killed. |
| S-P3-2 (adapter retention rule) | Closed | The retention rule is on `ConnectCommand.auth` (`ports.rs`), plus row `an_adapter_holds_the_connect_context_only_until_the_connect_completes`. |
| S-P3-3 (§15 record) | Open, scheduled | The remediation plan has the lane owner record it at merge. It is not yet in the root contract. It does not block. |
| S-P3-4 (Windows and fixture rows) | Partly closed | Windows receipt `p1-5-rem1-20261025-c` PASS. The plan's 1.8 carry of R4's fixture and hosted-like rows also happens at merge; it is not yet in the root plan. |
| S-P3-5 (tuple, pin, "no stale event") | Closed | The run README now records the committed tuple, the pin difference, and the mapping of "no stale event" to `a_late_event_of_binding_g…`. |

## New finding

**N-P3-1: a lease with no setup record is handed back `Reusable`, which revives a connection the adapter has already disposed.**

- **Location:** `instance.rs`, `openings()`. `fits = setup.is_some_and(…)` treats a missing `Setup` as a misfit, then calls `release(lease, Disposition::Reusable)`.
- **Interleaving:**
  1. The adapter reports `IdleClosed` for a connection the pool has just leased. The port doc covers this case: "the lease keeps it and its exchange fails; that lease's release and close end it."
  2. The pool refuses the `IdleClosed`, but the pump still removes the `Setup`.
  3. `openings()` takes the lease, finds no setup, and hands the dead connection back to idle.
- **Impact:** fail-closed — no context is exposed and no exchange starts on it. But a disposed connection holds a capacity slot until idle expiry, and any opening of the same scope that is offered it pays a misfit plus a fresh request. Round 1 discarded it here.
- **Correction:** discard when there is no setup record; give the lease back `Reusable` only when the setup record shows a known other scope.
- **Closure test:** with the fakes, deliver `IdleClosed` for a connection that has just been leased, then pump. Expect `closes()` to increase by one and the next same-scope opening to have no misfit.

**Next action:** at merge, fold in N-P3-1, the §15 row and the 1.8 carry, and put the saturation limitation in front of the operator as a decision.
