# p1c-https (1.3c/C2(b) per-exchange HTTPS auth): State-Axis Review
**Review object:** gwz-core dabe7dc6 (+ evidence 9e9db70a), 2026-10-31
**Baseline:**
- Lane root 5bc56837.
- gwz-core HEAD de03168e (the object is dabe7dc6, parent 34cdf51a).
- gwz-core-evidence HEAD 85bb605c. This is a merge of 9e9db70a with the p1c-crate evidence (319c3b42). `git diff 9e9db70a HEAD -- runs/2026-10-31-p1c-https` is empty, so the run directory is byte-identical to the object.
- gwz-transport 0a253639 was read for reference only.
- The same values were checked at the start and at the end of the review; nothing moved.
- Sources were read with `git show`/`git diff` and in place. `git diff dabe7dc6 de03168e` over the object's files is empty, so the object holds unchanged on the merge.
- Probes ran on an `rsync -aL` copy of gwz-core at `build-targets/p1c-https-review-state-tree`, with siblings symlinked read-only, and their own target dir `build-targets/p1c-https-review-state`. Both have been deleted.

**Date:** 2026-10-31
**Axis:** State: credential and connection state, meaning lifetime, races, fail-closed direction, cancellation and disclosure. Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: GO.** 0 P0, 0 P1, 0 P2, 1 P3 (P3-1, non-blocking).

## 0. Evidence base
- **Diff read in full:** `git -C gwz-core diff 34cdf51a dabe7dc6` (9 files, +521/-75), plus the unchanged code it depends on:
  - gwz-core:
    - `https_adapter/{worker,exchange,auth,pipe}.rs`
    - `https_auth.rs` (`OperationAuth`, `Config` has no `Debug`)
    - `endpoint_environment.rs` (`capture_operation`: every production operation gets an `OperationAuth` context over its full snapshot)
    - the legacy `https_worker.rs` and `transport_host/https_endpoint.rs` (operation naming)
  - gwz-connection:
    - `instance.rs:560-660` (scope fit, cross-binding `authorize_reuse`, `serve`)
    - `operation.rs:164-192` (`AuthContext` Debug prints `AuthContext(..)`)
  - gwz-transport `pool/allocation.rs` (idle selection: the lowest `ConnectionId` with equal key and identity)
- **Controlling text read:**
  - Contract §7, §7.2 and §15 rows 1.3b, 1.3c, 1.5, 1.6 and integration.
  - Plan §3 R4 rows, §4 1.8 text, the "1.8 preparation" note and the 1.8 Exit.
  - `history/…Step1.5-RemPlan.md:23` (C-P3-1: "the host-owned HTTPS adapter names routes uniquely per binding, never by the caller's operation name or `Input.session`").
  - `Step1.5-ReviewCode-2.md:25-40`, `Phase1Integration-RemPlan.md:22-28`, `Phase1Integration-ReviewState-2.md:40-50`.
  - The lane brief.
- **Checks** (gwz-core at de03168e, python3.13):
  - `check_windows_parity.py`: nothing new (513 files).
  - `check_cfg_boundaries.py`: nothing new (1729 files).
  - `check_process_globals.py`: nothing new (1514 files).
  - Braces and cfg in the diff: compliant. The only cfg is a pre-existing `cfg_if!`.
- **Evidence:**
  - `manifest.json` SHA-256 matches all 20 files.
  - `red-2.log` shows 4 of 9 red before the change, on the two R4 rows, the retention row and the push row. `green-final.log` shows 17 passed.
  - The mutation logs read as the README says.
  - Searching every evidence file for `fixture-token`, `token-of-binding`, `password=`, `Basic <b64>` and the base64 of `fixture:` finds nothing.
- **Focused runs on the candidate shape** (`TMPDIR=/Volumes/gwztmp`, `RUSTFLAGS='--cfg gwz_transport_candidate'`, `GWZ_TEST_GIT=real`, `cargo +1.95.0 test --locked --lib`, macOS arm64):
  - `connection_host::https`: 19 passed, which is the object's 15 rows plus 4 reviewer probes.
  - 1 further probe passed.
  - `reuse_tests`: 2 passed, 0 ignored.
  - `https_adapter`: 1 passed.
- **Reviewer probes.** Test-only and in the copy only. They print booleans, connection ids and token labels resolved by digest, never a value.
  - **PR1 (R4 carried to the POST).** op-a (token a) runs an advertisement and a POST, then finishes. op-b (token b) does the same on the same host while op-a's authenticated connection idles.
    - op-b's GET and POST both ran on connection 2 with B's credential. The POST was pre-authenticated.
    - Connection 1 carried only A's credential.
    - Accepted connections: 2.
    - The per-scope pool identity (`HttpsScoped`) keeps the idle A connection out of B's selection.
  - **PR2 (POST on a fresh, unauthenticated connection at a challenging server).**
    - Client read `Io(PermissionDenied)`, end `Discarded`.
    - 1 request, 0 with a credential.
    - No connection kept (`total()==0`), host `Open`.
  - **PR3 (two contexts in flight at once, 3 rounds of GET+GET then POST+POST).** Connection 1 carried A=6 and B=0; connection 2 carried A=0 and B=6. No connection ever carried both.
  - **PR4 (cancellation mid-pre-authentication).** The helper answers once, then hangs (`sleep 60`). The POST is abandoned while its lookup hangs.
    - 0 POSTs reached the wire.
    - The lease cleared at once; the connection went to closing.
    - `case.close()` returned true in 354 µs, so the helper was reaped.
  - **PR5 (the deferred case inside one binding).** The server answers its first request anonymously and challenges every later unauthenticated one. A's two advertisements overlap, so A holds anonymous connection 1 and helper-authenticated connection 2.
    - A's POST was given connection 1, the lowest idle connection of its own binding. It went without a credential and was refused: `Io(PermissionDenied)`, `Discarded`.
    - Connection 1 closed; connection 2 stayed idle; host `Open`.

## Problem, requirement provenance, and scope
- **Observed problem.** Before the change, `start_exchange` ignored its `AuthContext` and answered challenges with `HttpsInputs::auth`, the instance's helper. A second operation on the same host therefore sent the first operation's credential. This is the contract §15 1.3c row, "Open, owned by C2(b)". `red-2.log` confirms it ("B's request did not carry B's own credential").
- **Requirement provenance.** It traces to contract §7 (the helper environment lives in the per-operation `AuthContext`), plan §3 R4 ("a helper never runs with another operation's environment"), the 1.8 "carried by name" list, and the operator's 2026-10-31 go for 1.8.
- **The remedy addresses that problem directly:**
  - `Work` carries the operation's context.
  - `HttpsInputs` and `Common` lose the helper.
  - `auth::answer` takes the helper from `OperationAuth::of(&work.auth)`.
- **The POST pre-authentication is within scope.** The brief asks for "an assessment of the POST challenge, implemented if small". It is small (about 25 product lines in `exchange.rs`). It mirrors the legacy route, which already sends a route-cached credential on POSTs (`https_worker/serve.rs:41-45`). It complies with contract §7.2's "challenged POST refusal": nothing is replayed, and a 401 to a POST is still a failure.
- **The `transport_host/mod.rs` one-line deletion is necessary.** `https_factory_inputs` initialised the removed `HttpsInputs::auth` field; it cannot compile otherwise. `HttpsEndpointConfig::auth` stays live for the legacy endpoint (`https_endpoint.rs:175`), so no dead code is left. Authorized by demonstrated necessity.
- **The deletion of the legacy `#[ignore]`d row is correct, not hiding a defect:**
  1. The recorded obligation (Step1.5-RemPlan C-P3-1) is on the *host-owned* HTTPS adapter. That adapter has no route table; it takes each exchange's helper from the operation's context. PR1 and PR3 show no cross-context credential.
  2. In the legacy worker, the routes and their cached answers are per operation name. Each request's name is freshly minted (`session::unique()`, `https_endpoint.rs:338`). Each legacy `Endpoint` holds exactly one helper, the runtime's own capture (`Client::auth`; `endpoint_config_native`).
  3. Two bindings under one operation name exist only in that rig. A cached answer can only be the same operation's own helper's.
  4. The retained row still witnesses the connection leg, and it now compares digests instead of printing Authorization values (the old `assert_eq!` on `Option<String>` would have printed them).

  The brief's "un-ignore the row" was the lane owner's preference, not a requirement. The controlling obligation is met.
- **Scope note.** The object does not state how §7.2's per-connection logon-session scope combines with the operation scope (owed to C2(b) by the 1.5 §15 row: "two scopes on HTTPS identities"). That text belongs in the §15 row the lane owner writes; I did not see the lane report. In behaviour it is closed:
  - A logon-session connection is per-exchange scoped and serves only the binding that authenticated it (`authorize_reuse`).
  - An operation of another scope never selects it, because the pool identity is per scope.

## 1. Findings

### P3-1: The deferred "POST on a connection other than an authenticated one of its binding" limitation is neither recorded nor witnessed in the object's durable artifacts, and the module doc overclaims
- **Location:**
  - `gwz-core/src/git/endpoint/https_adapter.rs:19-21`: "…so an authenticated push can go through the host", with no qualification.
  - The dabe7dc6 commit message, same sentence.
  - The evidence README: "see the lane report".
  - No row in `connection_host/https_auth_tests.rs` covers the refusal.
- **Violated invariant.** A recorded limitation must be stated where later readers look (AgentProcessRules; Surface structural debt). A deferral's failure mode must have a regression witness.
- **Reproduction (PR5).**
  1. Within one binding, two overlapping advertisements leave the binding holding an anonymous connection with a lower id than its helper-authenticated one. In the real world this is a public and a private repository on one host, fetched concurrently by one gwz operation.
  2. The pool gives the next lease to the lowest idle connection with equal key and identity (`pool/allocation.rs:22`).
  3. The POST lands on the anonymous connection, `prior == Anonymous`, so it is sent without a credential.
  4. The server answers 401 and the POST is refused.

  PR2 shows the same outcome for a POST on a fresh connection. The sequential R4 shape is not affected (PR1).
- **Impact.**
  - The failure mode is safe:
    - It fails closed: no other context's credential, and no credential at all.
    - It is typed: `PermissionDenied`/`Authentication`.
    - The connection is discarded, and the authenticated sibling and the host are untouched.
    - Nothing is replayed.
  - The host has no production caller until Phase 3, so the deferral is honestly bounded to Phase 3 as the drafter says.
  - The consequence is diagnosability and planning. A Phase 3 or 1.8 hosted-like run with mixed public/private members on one host can fail a fetch or push with "Authentication" although no credential was offered. A reader of the code, commit or README would believe authenticated push through the host is done.
- **Required correction.**
  1. Qualify the module doc at `https_adapter.rs:19-21`: a POST is pre-authenticated only on a connection a helper token authenticated for its own binding; a POST that the pool places elsewhere is refused (Phase 3 prerequisite).
  2. Put the same sentence in the §15 1.3c/C2(b) row text the lane owner proposes.
  3. Add a row pinning the refusal shape, in the shape of PR5 or PR2. It asserts:
     - a typed `PermissionDenied` read and `Discarded` end;
     - zero requests with a credential on the POST;
     - the POST's connection closed and the host `Open`.
- **Closure test.** The new row is green, and it turns red if the POST path is changed to send a credential on an unauthenticated connection, or to keep the connection.

## 2. Invariant analysis
- **Credential lifetime.** The `AuthContext` clone lives only in `Work`: from `pipe::exchange` into `Command::Start`, then into the exchange task. It is dropped on every path:
  - The task returns (success or `Denied`).
  - `end_exchange` aborts the task (close, dispose).
  - `Command::refuse` or a failed `start()` drops `work` after `work.fail`.
  - The worker runtime drops at the end of `run`.
  - A failed `send` refuses the command.

  Nothing else clones it. `https_auth` lookups borrow `&Config` (`runner.rs:9`, `lookup.rs`). Neither `Work` nor `Config` has `Debug`; `AuthContext`'s `Debug` prints `AuthContext(..)`. The `Secret` and `SecretHeader` are dropped right after the request is built, and the header value is `set_sensitive(true)`.
- **`prior` cannot authenticate a later operation:**
  - `prior` is a copy of the scope record read when `start()` takes the lease. The record is updated in `step` before `settle.end(Reusable)`, so it is current whenever the host can lease again.
  - `prior == HelperToken` only causes a fresh lookup with *this* exchange's own helper. With no helper, it is `Err(Authentication)` before anything is sent.
  - A helper-token connection is never leased across bindings: `authorize_reuse` returns false for `per_exchange()` (red-retention-mutation kills the inverse). A different scope never selects it, because the pool identity is per scope.
  - So a token of operation A cannot land on a request of operation B under the interleavings tried:
    - two contexts in flight on one host (PR3);
    - sequential with A's connection idle (PR1);
    - an operation with no helper (the object's row, plus red-nohelper-mutation);
    - a cancel mid-lookup (PR4: nothing on the wire).
- **`authorize_reuse` timing.** It is called by the host after the pool lease and before `serve`, and it reads the same records `start()` reads. No exchange runs on a connection while it is leased for a proof.
- **Fail-closed.**
  - A refusing helper gives a typed denial, keeps no connection and sends no credential (the object's row, and red-denied-mutation).
  - A hanging helper is cancelled by the client's abandonment (PR4) or bounded by the interaction and allocation clocks.
  - A garbage or crashing helper maps through `AuthError::code` to `Authentication`/`Unavailable`, never to a send.
  - A POST-path lookup failure sets no binding, and the connection is discarded.
- **Stuck states.** None found:
  - Every new early return yields `Ok(Discarded)` or `Err`, both of which end `settle`.
  - The `select!` on `pipe.interrupted()` covers the new lookup.
- **Disclosure.**
  - The new and changed rows compare SHA-256 digests. `panic!` and `assert!` messages print booleans or connection ids only.
  - No log, Debug or format of a token or header was found in the diff.
  - The evidence logs contain no token.
- **Legacy deletion.** Correct, as argued under scope above.

## 3. Risks and next action
- **Risks:**
  - The host's per-POST helper lookup spawns `git credential fill` for every POST, and for every challenged GET, with no `approve`. That follows the contract ("no token cache in host"). An interactive helper may therefore prompt more often than Git does; this should be measured when Phase 3 adopts the host.
  - The Windows (SSPI/Negotiate) and Linux behaviour was not run by me. It is covered by the lane owner's receipt `p1c-https-20261031-a` and matrix, which I was not given.
- **Next action:**
  1. Land P3-1's doc qualifier and refusal row with the §15 text.
  2. Record the deferred case as a named Phase 3 prerequisite in the plan.

## 4. Out-of-scope observations
- `https_auth_tests.rs`, in `changed_helper_gives_each_operation_its_own_credential`, still says "the instance's helper (while it has one) is the first operation's" and passes `Some(first)` to `Case::with`. Since dabe7dc6 that only seeds `Case::helper`, which this row does not use. The comment is stale but has no behavioural consequence.
- No adapter path calls `git credential approve/reject`, and neither does the legacy route. Helpers that store on approve (GCM) never learn of a success. This is a limitation, not a defect of this object.
