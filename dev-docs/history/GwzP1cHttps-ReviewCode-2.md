# p1c-https (1.3c/C2(b) per-exchange HTTPS auth) — Code-AXIS RE-VERDICT, ROUND 2
**Review object:** gwz-core `68db19ae` ("HTTPS adapter: pre-authenticate a POST only for the repository its connection was authenticated for (1.3c/C2(b), round 1)"), reviewed as `git -C gwz-core diff de03168e 68db19ae` (6 files, +192/-26), with evidence gwz-core-evidence `2c7a6ad5` (`runs/2026-10-31-p1c-https/rem1/`). The original object `dabe7dc6` is an ancestor.
**Baseline:**
- lane root `11156906`, which records the round-1 reviews and `dev-docs/GwzP1cHttps-RemPlan.md`;
- gwz-core HEAD `68db19ae`, the same at start and at end, with a clean working tree;
- gwz-core-evidence HEAD `2c7a6ad5`;
- gwz-transport read-only.

How sources were read:
- the diff and the full changed files at `68db19ae`;
- the RemPlan;
- the `rem1` README section and logs. All 25 manifest digests match, and a grep of `rem1/` and `README.md` found no token or `Basic` credential.
**Date:** 2026-10-31
**Axis:** Code, round 2 (architecture, interfaces, call graphs, compatibility). Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 1 P3 (new, non-gating). Both prior findings are closed.

## 0. Evidence base

My own copy tree of the lane at `68db19ae` (rsync `-L`, targets excluded, `diff -rq` against the lane's `src`: identical). Candidate shape: `TMPDIR=/Volumes/gwztmp`, `RUSTFLAGS='--cfg gwz_transport_candidate'`, `GWZ_TEST_GIT=real`, `cargo +1.95.0 test --locked --lib`.

- **`connection_host::https`, plus my two probe rows appended to the copy's `https_tests.rs`:** 19 passed, which is the drafter's 17 plus my 2.
  - My round-1 P2-1 counterexample (upload-pack, `/private` then `/public`, a helper that declines `public`) is now green: `outcome_ok=true`, `public_posts_reaching_server=1`.
  - Probe 2 is in P3-2 below.
- **`reuse_tests`:** 2 passed. **`https_adapter`:** 1 passed.
- **Mutation: the path check removed** (`authenticated_here` forced true whenever `prior == HelperToken`). This reproduces the dabe7dc6 predicate. Both `a_post_to_a_repository_the_helper_did_not_authenticate_is_sent_without_a_credential` and my probe went red with `PermissionDenied`. I reverted it.
- **Drafter's evidence I checked:**
  - `rem1/red-1.log`: the new P2-1 row is red on the dabe7dc6 product code.
  - `rem1/red-post-401-mutation.log`: the P3-1 row is red when a POST's 401 is answered.
  - `rem1/green-1.log`: 17 / 2 / 1 passed.
- **Checks:** `check_windows_parity.py`, `check_cfg_boundaries.py` and `check_process_globals.py` all report "nothing new" at `68db19ae`.
- **Clean-up:** the copy tree and the target dir are deleted, and the copy was restored identical before deletion. Two run logs are kept, with test names and results only and no credential:
  - `/Volumes/projects/limbo/build-targets/p1c-https-review-code-run1.log`
  - `/Volumes/projects/limbo/build-targets/p1c-https-review-code-run2.log`

## Problem, requirement provenance, and scope

- **Observed problem being remediated:** my round-1 P2-1, plus the P3-1 that both axes raised. The RemPlan cites both reviews, which is legitimate input for this round.
- **The remedy is the localized fix I specified:**
  - a path digest in the adapter-local scope record;
  - pre-authentication only on a path match;
  - an anonymous POST otherwise, as before `dabe7dc6`.
- **Authorization:**
  - Every edited file is in the owned list (`https_adapter*`, `connection_host/https*`). Nothing outside it was touched.
  - No scope was widened. The deferral's outcome is unchanged: a POST placed on an unauthenticated connection is refused. It is now stated in the module doc and pinned by a row.
  - The stale test comment was corrected as State §4 asked.
- **Requirement provenance:** every change traces to a round-1 finding. The added `RepositoryDigest` type is the minimum carrier for the fix.

## 1. Prior-finding closure

| ID | Status | Evidence |
|---|---|---|
| **P2-1**: POST pre-authentication keyed on a sticky "ever helper-authenticated" record | **Closed** | `exchange.rs`: `authenticated_here = prior == HelperToken && authenticated == Some(repository_digest(&work.path))`, with the digest recorded by `Scopes::bound` from `Ended.repository`. `bound` still ignores non-per-exchange endings, so an anonymous exchange on another path never overwrites the record. The new standing row `a_post_to_a_repository_the_helper_did_not_authenticate_is_sent_without_a_credential` is equivalent to my probe: the same `/private` → `/public` → POST sequence on one connection of one binding, a helper declining `public`, and assertions that the `/public` POST succeeds, reaches the server once with no Authorization, on the same connection, with `accepted == 1`. It uses receive-pack where my probe used upload-pack; the predicate does not depend on the service. My probe is green on `68db19ae`, red on `dabe7dc6`, and red under my path-check mutation. `a_push_is_authenticated_on_the_connection_its_advertisement_was` stays green. |
| **P3-1**: no standing row pins the deferred refused-POST failure mode | **Closed** | `a_post_on_a_connection_nothing_authenticated_is_refused_after_one_anonymous_attempt`. A push POST is opened on a fresh connection (`prior == Anonymous`, so no pre-authentication) and gets a 401. The row asserts `PermissionDenied`, `Discarded`, `counts.total() == 0` (connection closed), exactly one POST, no digest on any request, and the host `Open`. It is red under the 401-answering mutation (`rem1/red-post-401-mutation.log`). |

## 2. Changed-range analysis

**Recording path.**
- `exchange::run` computes `repository_digest(&work.path)` for every ending. `Scopes::bound` stores it only with a per-exchange binding.
- A pre-authenticated POST sets `*binding = HelperToken` and so re-records its own, equal path.
- A logon-session ending records its path too. That is harmless, because pre-authentication requires `HelperToken`.
- `Worker::start` reads `binding` and `repository` from a single `Scopes::of` snapshot. Both are set in `step()` before the connection returns to `Idle`, so there is no read-before-write race.
- `pipe.rs` initializes `authenticated: None`.
- There is one construction site each for `Work`, `Ended` and `ConnectionScope`, all inside `https_adapter/`. Grep finds no other constructor or `.bound(` call in the tree, and the Windows-only `https_sspi_tests.rs` reads `binding` only.

**Secret handling.** The digest is SHA-256 of the URL path. It is not a credential, and the token flow is unchanged: a lookup per exchange, the secret dropped after the request is built, the header marked sensitive.

**Cross-binding safety is unchanged.**
- A `HelperToken` connection is still leased only to its authenticating binding: `authorize_reuse` refuses it, and the host checks the lessee.
- The added condition only narrows when a credential is attached. It cannot attach one in a new place.

**Single-digest replacement** (the drafter's note). This is a real, new limitation of the single-slot record. Its failure mode is safe and typed. It is not a new architectural root cause: it is the same "POST not pre-authenticated, so a challenge is refused" outcome the deferral already covers, reached by a new route. I record it as P3-2 below.

**`https_tests.rs` spec-path change.** `spec_for` now delegates to `spec_at(through, service, "/repo")`, and `exchange_at` is added. Every existing row still uses `/repo`, so their behaviour is unchanged. The only other test-support change is that `authorizing` now serves `/public*` anonymously. No pre-existing row uses a `/public` path, so their semantics are unchanged (confirmed by the 17 drafter rows passing).

**`changed_helper_gives_each_operation_its_own_credential`** now passes `(None, None)` as the case routes. That is correct: the instance holds no helper, and the R4 rows still go red under my round-1 instance-helper mutation, whose mechanism is unaffected by this patch.

**Repository rules.**
- All control flow is braced.
- No `#[cfg]` was added.
- No global or thread-local.
- Test code stays under `src/…/*tests*.rs`.
- The three guards report nothing new.

## 3. Findings

### P3-2 (new; limitation, not architectural) — The single repository digest is replaced by a later authenticated exchange, so a POST on its own advertisement's connection can be refused

**Root cause.** `ConnectionScope.repository` (`https_adapter.rs`, `Scopes::bound`) holds one digest, overwritten by every per-exchange binding.

**Location:**
- `src/git/endpoint/https_adapter.rs` (`ConnectionScope`, `Scopes::bound`);
- `src/git/endpoint/https_adapter/exchange.rs` (`authenticated_here`);
- the module doc at `https_adapter.rs:19-25`.

**Invariant affected.** The module doc says "an authenticated push goes through the host only when the pool places its POST on the connection its advertisement was authenticated on". That is accurate as a necessary condition. But it reads as sufficient, and it is not.

**Reproduction (run).** My probe 2: one binding, one connection, every path challenged, a helper that answers everything.
1. Receive-pack advertisement `/a` (authenticated).
2. Receive-pack advertisement `/b` (authenticated; the record becomes `/b`).
3. POST `/a` on the same connection (`accepted == 1`).

Result: the POST goes out anonymously, gets a 401, and is refused with `PermissionDenied`. The server saw one POST, unauthorized, status 401.

Interleaving that produces this in practice: one operation pushing several private repositories of one origin concurrently. Push A's advertisement ends, the idle connection is leased to push B's advertisement, and A's POST then lands on it.

**Impact.**
- The deferred Phase 3 prerequisite (push through the host) is somewhat less likely to succeed under concurrent pushes than the doc implies.
- The failure is safe: a typed `Authentication`, no replay, no credential sent, the connection discarded. P3-1's row pins this outcome.
- There is no regression against the pre-`dabe7dc6` parent. Production does not use the host before Phase 3.

**Required correction (either is sufficient):**
- (a) State in the module doc, and in the §15 1.3c/C2(b) row and the Phase 3.1 prerequisite text, that pre-authentication covers only the repository of the last helper-authenticated exchange on that connection.
- (b) Keep a small bounded set of authenticated path digests per connection: for example up to N with oldest eviction, cleared at disposal. This is safe because a `HelperToken` connection serves only its authenticating binding, and each POST still asks the helper for its own path.

**Closure test.** Probe 2 as a standing row asserting the chosen behaviour: refused and typed under (a), or authenticated and sent once under (b).

## 4. Risks

- **Windows receipt** `p1c-https-20261031-b`: it must include both new rows. The `/public` row's helper fixture uses a `case "$(cat)"` script under `sh.exe`. I ran macOS only, and I cite no Linux or Windows result.
- **Clerical work still the lane owner's at merge** (RemPlan rows; not graded here):
  - the §15 1.3c/C2(b) row text, including retiring the `reuse_tests.rs:141`/`:149` obligation;
  - the scope AND as implemented;
  - the Phase 3.1 deferral, which should also carry P3-2's qualification.
- **Phase 3:** a placement-aware design for push through the host (POST affinity to the advertisement's connection, or a bounded digest set) remains a named prerequisite. Today's failure mode is safe and pinned.

## 5. Out-of-scope observations

Unchanged from round 1 and recorded in the RemPlan for Phase 3 design. No new ones.
- The instance-level `NativeCaller` versus the per-operation caller capture (a hypothesis for shared hosts).
- The cost of a helper spawn per authenticated advertisement and per pre-authenticated POST.
