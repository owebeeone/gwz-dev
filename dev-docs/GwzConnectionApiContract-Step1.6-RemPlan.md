# Step 1.6 cross-binding reuse authorization: remediation plan (round 1)

Date: 2026-10-25. Lane owner's plan after [Code](GwzConnectionApiContract-Step1.6-ReviewCode.md) (NO-GO: 1 P2, 5 P3) and [State](GwzConnectionApiContract-Step1.6-ReviewState.md) (NO-GO: 2 P2, 3 P3) on gwz-transport `9f8f091` / gwz-core `726e9062` (lane p1-reuse). Both pre-committed to GO on bounded remedies. One patch; the same two reviewers re-verdict.

**Architectural root-cause count on this object: 1.** C-P2-1 = S-P2-1, found blind by both axes: `Completion::Reuse` carries no binding or generation and there is no cancel, so a late verdict from one binding's proof can serve another binding's lease on the same connection.

Convergences: C-P2-1 = S-P2-1; C-P3-2 = S-P3-1 (RSA sizes sent to the agent); C-P3-3 = S-P3-3 (the helper-token probe not in the evidence); C-P3-5 = S-P3-2 (the shared job cancelled with its last waiter).

## Lane owner's decisions

1. **Authorize a narrow gwz-transport pool change for S-P2-2** (the lane owns the gwz-transport member): a `release` variant that returns a bounced lease to idle WITHOUT re-stamping its idle `since`, and a request-level exclusion (or last-lessee-preferring idle selection) so a non-fresh retry is not handed the same connection again. No other pool semantics change; the pool stays bytes-only and lessee-agnostic except for the exclusion the request itself names.
2. The crate's port grows a proof token: `ReuseCommand { connection, binding, proof: ProofId }` echoed in `Completion::Reuse { connection, proof, verdict }`; `reused()` matches `(connection, proof)`; `EndpointAdapter::cancel_reuse(proof)` is called from `abandon` and `fail_in_flight` (contract §4.1 `cancel_work`). Record in contract §15 (lane owner).
3. The crate now sits on gwz-transport `9fbe7448` (1.5's remediation: HTTPS requests carry `Identity::HttpsScoped`, the misfit path releases `Reusable` and resubmits once fresh under the original deadline, `ask_again` keeps the original deadline). Build on it; reconcile `ask_again` with your P2-2 fix.

## Blocking

| ID | Disposition | Where | Closure |
| --- | --- | --- | --- |
| C-P2-1 = S-P2-1 (architectural) | Decision 2: proof token in command and completion; `reused` matches `(connection, proof)` and drops a completion whose proof is no longer the one in `authorizing`; `cancel_reuse` on abandon and seal; fakes end proofs by command. | `crates/gwz-connection/src/ports.rs`, `instance.rs:265–289, 512–520`, `fakes.rs` | Crate row: B holds and is dropped, C opens and holds; ending B's proof `Authorized` leaves C unserved; ending C's `Revoked` gives C a new connection; `cancel_reuse` issued on the dropped opening. Mutation: ignore the proof in `reused` → row fails. |
| S-P2-2 | Decision 1: a bounced (unprovable or no-evidence) lease goes back to idle without re-stamping `since`, and the opening's fresh/non-fresh retry excludes that connection so the binding's OWN idle connection is offered; the idle-age bound (60 s from the last real release) and the 10-minute maximum age are observable. Apply in both paths (crate `instance.rs:446, 497, 528`; gwz-core `ssh_worker/runner.rs:512–513, 693`, `held.rs:57`). | gwz-transport `src/pool/{lifecycle,allocation}.rs` (narrow), crate `instance.rs`, gwz-core | Crate and gwz-core rows: binding B's second open reuses B's own idle connection (`reused: true`, one connect, `connects() == 2` not 3); the unprovable connection expires 60 s after its last real release despite B's opens (fake clock). |

## Non-blocking (same patch)

| ID | Disposition | Where |
| --- | --- | --- |
| C-P3-1 | An `ssh-rsa` reply to a `rsa-sha2-512` request is `Proof::Unsupported` → `Unprovable` (never `Refused`); row with the downgrading agent asserts `Unprovable` and A's idle connections survive. | `agent_keys.rs:240`, `agent_auth.rs:406` |
| C-P3-2 = S-P3-1 | `provable(blob)` reads the RSA modulus size and refuses outside 2048–8192 before any agent request; row: RSA-1024 proof sends zero sign requests; README corrected. | `agent_keys.rs:219–221` |
| C-P3-3 = S-P3-3 | The helper-token row asserts the current behaviour as a named known defect (`OPEN: routes keyed by operation name; owner 1.3c/C2(b)`), plus an `#[ignore]` row for the desired behaviour; `--nocapture` output filed; README worded "connection leg only; route leg = O1". | `https_worker/reuse_tests.rs:126–137`, evidence |
| C-P3-4 | Two crate rows: a misfit lease last used by binding A is released with no `authorize_reuse` call (`reuses()` empty); the host sealed while a proof is held → opening `Closing`, lease discarded, late verdict ignored. | `reuse_tests.rs` |
| C-P3-5 = S-P3-2 | A shared proof job with no waiters is kept (under its deadline/supervisor) until it ends; a passing verdict is recorded into `Proofs` for the binding; only binding cancellation cancels it; row: the proof outlives its waiter, passes, and the next lease by the same binding asks the agent nothing (`signs()` +1 total). | `ssh_worker/runner.rs:611–616` |
| P-521 | Recorded (contract §15 row, lane owner): excluded by lane-owner decision, departing from reuse design C9's list, because ring lacks P-521. | doc |

## Observations carried

Two last-lessee records (crate `Setup.lessee` by `BindingId` vs gwz-core `PoolHost.lessee` by bridge `session_id`): one owner when the 1.3b/1.3c adapters wrap the worker (1.3c/Phase 1 review); binding identity in gwz-core is the bridge `session_id` (T3 forbids client-chosen ids: 2.2c); no maximum connection age anywhere (reuse design C3; contract §10 10 min: Phase 1 review); certificate and security-key proof paths have no rows (add if cheap, else record); `runner.rs` 354 → 725 lines shared with 1.3b (operator: split `ssh_worker/reuse.rs`); ring's duplicate dependencies (getrandom 0.2, windows-sys 0.52).

## Process

One patch in lane p1-reuse on gwz-transport `9fbe7448`; gates: crate tests + clippy 1.96, remote matrix (`p1-6-rem1-20261025-a`), Windows receipt `p1-6-rem1-20261025-a` (the real-SSPI and Windows agent rows); evidence appended to `2026-10-25-p1-6`; the lane owner commits (gwz-transport first and pushes, pin moved in gwz-core); re-verdict (`-ReviewCode-2.md`, `-ReviewState-2.md`). Round count after this patch: 1 of 2.
