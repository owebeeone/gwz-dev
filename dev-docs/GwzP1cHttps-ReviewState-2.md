# p1c-https (1.3c/C2(b) per-exchange HTTPS auth): State-Axis Re-Verdict, Round 2
**Review object:** gwz-core `68db19ae` ("HTTPS adapter: pre-authenticate a POST only for the repository its connection was authenticated for (1.3c/C2(b), round 1)"), on lane merge `de03168e`; the original object `dabe7dc6` is an ancestor. Evidence: gwz-core-evidence `2c7a6ad5`, `runs/2026-10-31-p1c-https/rem1/`.
**Baseline:**
- **HEADs, unchanged from start to end of this round:**
  - gwz-core `68db19ae`.
  - gwz-core-evidence `2c7a6ad5`.
  - Lane root `11156906`. Since `5bc56837` it adds only the filed reviews, `GwzP1cHttps-RemPlan.md` and gwz.conf records.
- **Changed range:** `git -C gwz-core diff de03168e 68db19ae` (6 files, +192/-26), read in full.
- **Probes:** run on an `rsync -aL` copy of gwz-core at `build-targets/p1c-https-review-state-tree`, with siblings symlinked read-only and its own target dir `build-targets/p1c-https-review-state`. Both have been deleted.

**Date:** 2026-10-31
**Axis:** State, round 2. Credential and connection state: lifetime, interleavings, fail-closed direction, cancellation and disclosure. Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: GO.** 0 P0, 0 P1, 0 P2, 1 P3 (new P3-2, non-blocking). Prior State P3-1 is closed except for its doc sentence, which P3-2 replaces.

## 0. Evidence base
- **Checks on `68db19ae`** (python3.13): `check_windows_parity.py`, `check_cfg_boundaries.py` and `check_process_globals.py` each report "nothing new". The changed range has no `#[cfg]`, and every control-flow body is braced.
- **Focused runs on the candidate shape** (`TMPDIR=/Volumes/gwztmp`, `RUSTFLAGS='--cfg gwz_transport_candidate'`, `GWZ_TEST_GIT=real`, `cargo +1.95.0 test --locked --lib`, macOS arm64):
  - `connection_host::https`: 23 passed. That is the object's 17 rows plus 6 reviewer probes.
  - `reuse_tests`: 2 passed.
  - `https_adapter`: 1 passed.
- **Evidence:**
  - The manifest covers 25 files and every SHA-256 matches.
  - `rem1/red-1.log` (`dabe7dc6` product code, new rows): `a_post_to_a_repository_the_helper_did_not_authenticate_is_sent_without_a_credential` is FAILED. The refused-POST row is green there, as the README says, because it pins existing behaviour.
  - `rem1/red-post-401-mutation.log`: the refused-POST row is FAILED under "answer a POST's 401".
  - Searching the whole run directory for `fixture-token`, `token-of-binding`, `password=`, `Basic <b64>` and the base64 of `fixture:` finds nothing.
- **Probes** (test-only, in the copy). They print booleans, connection ids and token labels resolved by digest, never a value.
  - **PR1 (R4 to the POST).** op-a (token a) runs an advertisement and a POST on connection 1. op-b (token b) runs both on connection 2, the POST carrying B's credential. Connection 1 carried only A's. Unchanged from round 1.
  - **PR3 (two contexts concurrently, 3 rounds of GET+GET then POST+POST).** Connection 1 carried A=6, B=0; connection 2 carried A=0, B=6. Unchanged.
  - **PR4 (POST abandoned while its pre-authentication lookup hangs).** 0 POSTs reached the wire; `close()` returned true in 1 ms, so the helper was reaped. Unchanged.
  - **PR5 (the deferred case inside one binding).** The binding holds anonymous connection 1 and helper-authenticated connection 2; the pool gives its POST connection 1.
    - The POST goes without a credential: `PermissionDenied`, `Discarded`.
    - Connection 1 closes, connection 2 stays idle, host `Open`.
    - Unchanged, and now pinned by the lane's standing row.
  - **PR6 (new, the single-slot record).** One binding, one connection. GET `/a` is authenticated, then GET `/b` is authenticated on the same connection, then POST `/a` is sent there.
    - POST `/a` went without a credential: `PermissionDenied`, `Discarded`.
    - A fresh advertisement of `/b` and POST `/b` on a new connection then succeeded, pre-authenticated with A's own credential. Host `Open`.
  - **PR7 (new).**
    - POST `/repo.git` after an authenticated GET `/repo` went anonymously and was refused: the digests differ, so it fails closed.
    - POST `/public-x` on that binding went anonymously and succeeded.

## Problem, requirement provenance, and scope
- **Inputs.** Code P2-1 and both P3-1s, merged in `GwzP1cHttps-RemPlan.md` (legitimate round-2 input).
- **Code P2-1** (from dabe7dc6's "ever helper-authenticated" pre-authentication rule): an anonymous POST to a repository the server serves to anyone failed when the helper declined it. The behaviour before dabe7dc6, that such a POST goes out anonymously, is the requirement. The fix, a path-bound pre-authentication, addresses it directly, and `rem1/red-1.log` shows the row red on dabe7dc6.
- **Scope:**
  - The change stays inside the editable list (`https_adapter*`, `connection_host/https*`).
  - Nothing outside it changed; there is no edit to `transport_host/mod.rs` this round.
  - Neither the repository digest nor `exchange_at` is a scope expansion; both are the minimal state and test support for the remedy.
- **Alternatives:** keeping a bounded set of authenticated path digests per connection would have met P2-1 without the regression in P3-2. It was not considered in the plan.

## Prior-finding closure table
| ID | Status | Evidence |
|---|---|---|
| State P3-1 (row) | **Closed** | `a_post_on_a_connection_nothing_authenticated_is_refused_after_one_anonymous_attempt`. It asserts `PermissionDenied`, `Discarded`, exactly one POST, no credential on any request, `total()==0` and host `Open`. Red under the POST-401 mutation (`rem1/red-post-401-mutation.log`). PR5 independently reproduces the same shape inside one binding. |
| State P3-1 (doc) | **Partly closed; superseded by P3-2** | `https_adapter.rs:19-25` now qualifies the claim and names the deferral a Phase 3 prerequisite. Its sentence "an authenticated push goes through the host only when the pool places its POST on the connection its advertisement was authenticated on" reads as a sufficient condition, which PR6 refutes. |
| State §4 stale comment | **Closed** | `changed_helper_gives_each_operation_its_own_credential` now passes `(None, None)` and says the instance holds no helper. |
| State §4 no `approve`/`reject` | Recorded by the lane owner as a limitation (RemPlan) | Not in this patch's scope. |

## Changed-range analysis
- **Lifetime of `ConnectionScope.repository`:**
  - It is set only by `Scopes::bound`, which runs only for a per-exchange binding, in `worker::step` before `settle.end(Reusable)`.
  - It is removed with the record in `Scopes::forget` when the connection is disposed. It lives no longer than the connection.
  - `start()` reads `prior` and `authenticated` from one `scopes.of()` snapshot, so the pair is consistent. The connection is exclusively leased while the exchange runs.
- **Replacement:**
  - An anonymous exchange never updates the record, so a public GET or POST does not erase `/private` (the lane's new row, and PR7).
  - A later helper-authenticated exchange on another path *replaces* it. PR6 shows the earlier path's POST on the same connection is then sent anonymously and refused. **This is new state introduced by the remediation:** a single-slot representation. It is not an architectural root cause; see P3-2.
- **Wrong path or wrong binding.** Pre-authentication requires `prior == HelperToken` and an equal SHA-256 of the exact request path. The lookup is still made for the POST's own destination with the POST's own context's helper.
  - A different path, including a different spelling (PR7), goes anonymously.
  - A helper-token connection is never leased to another binding (`authorize_reuse`, unchanged), and another scope never selects it (per-scope pool identity, PR1 and PR3).
  - No interleaving tried pre-authenticated a POST on the wrong path or on another binding's connection. Every mismatch fails in the anonymous direction.
- **Disclosure:**
  - The digest is of the URL path, not a secret. It is held only in adapter memory.
  - `ConnectionScope` derives `Debug`, but it is printed only by test assertions (`Scopes::all` is `cfg(test)`).
  - No log or message in the changed range formats it, a token, a header or helper output.
  - The new rows compare Authorization headers by digest.
- **Fail-closed and cancellation.** These are unchanged and re-verified by PR4 (no POST on the wire, helper reaped) and the refused-POST row (no credential, connection closed).

## 1. Findings

### P3-2 (new, introduced by `68db19ae`): the scope record keeps only the last authenticated path, so a POST on the very connection its advertisement authenticated can be refused, and the module doc states the opposite
- **Location:**
  - `gwz-core/src/git/endpoint/https_adapter.rs:174-177`: `Scopes::bound` overwrites `repository: Some(repository)`.
  - `https_adapter/exchange.rs:219-220`: `authenticated_here` compares against that single slot.
  - Doc `https_adapter.rs:19-25`.
- **Violated invariant.** The recorded deferral boundary: a POST is refused only when the pool places it on a connection other than the one its advertisement was authenticated on. The doc sentence and the commit message state this boundary.
- **Reproduction (PR6).** Within one binding and on one connection:
  1. GET `/a` is challenged and answered.
  2. GET `/b` is challenged and answered on the same connection, replacing the record with `/b`.
  3. POST `/a` is placed on that connection. It goes out anonymously, gets a 401, and is refused with `PermissionDenied`/`Discarded`.

  In a gwz operation fetching or pushing several private repositories on one host through one binding, the advertisement of one member can land between another member's advertisement and POST. Under dabe7dc6 that POST was pre-authenticated and succeeded.
- **Impact:**
  - Fail-closed: no credential is sent to a wrong path or binding, the refusal is typed, and nothing is replayed.
  - No production caller until Phase 3.
  - The Phase 3 prerequisite the lane owner will record (§15 1.3c/C2(b) row, plan 3.1) inherits an inaccurate boundary. It would be under-scoped by the case "same connection, another path re-authenticated in between". The refusals it causes look identical to the deferred case.
- **Required correction (either):**
  - **(a)** Keep the set of path digests authenticated on the connection. All of them belong to the one binding the connection serves. Bound the set, for example to 64, and send anonymously on overflow. A POST is pre-authenticated when its path is in the set.
  - **(b)** Keep the single slot, and correct the doc, the commit's carried text and the §15/plan wording to: "only when the POST's path is the last path a helper authenticated on that connection".
- **Closure test.** PR6 as a standing row. Under (a), POST `/a` carries A's credential (by digest) on the same connection and succeeds. Under (b), it is refused exactly as the doc says. Either way the existing public-POST and refused-POST rows stay green.

## 2. Risks
- The Windows receipt `p1c-https-20261031-b` and the Linux/remote matrix were not given to me, so SSPI/Negotiate behaviour on the corrected tree is unverified by this review. The changed range does not touch the native route: a `LogonSession` record still never triggers pre-authentication.
- The helper-spawn cost per POST is unchanged; the lane owner records it for Phase 3.

## 3. Out-of-scope observations
- Path matching is by exact bytes (PR7). Git uses one remote URL for both the advertisement and the POST, so this is not a defect today. A Phase 3 caller that canonicalises paths differently between the two would only see more anonymous POSTs, which is fail-closed.
- `rem1/README.md` says the red run's test file "still held a no-op environment entry in the helper fixture, removed before the green run". The red-to-green delta therefore includes a test-fixture edit. It is harmless as described, but the red log is not of the exact committed test text.
