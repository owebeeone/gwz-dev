# HTTPS authentication model (DRAFT r2): Safety-axis review

**Review object:** dev-docs/GwzHttpsAuthModel-DRAFT.md at root 0a0545a3 (code at gwz-core a7b965c5, gwz-transport 0a253639)

**Baseline:** Every root document was read with `git show 0a0545a3:<path>`. Code was read with `git -C gwz-core show a7b965c5:<path>` and `git -C gwz-transport show 0a253639:<path>`. `git diff 0a0545a3 HEAD -- dev-docs/GwzHttpsAuthModel-DRAFT.md` was empty at the start and at the end; root HEAD moved to `8d0837bc` during the review, which does not change the object. Nothing was written, built or run. No model was needed, so `build-targets/review-https-model-safety/` was never created.

**Date:** 2026-10-31

**Axis:** Safety: what the text permits to go wrong in credential retention (b2), in the connection-bound placement hint and its re-authentication GET, and on degraded, stuck and fail-closed paths. Independent, adversarial, read-only. Filed verbatim by the lane owner.

**Verdict: NO-GO.** 0 P0, 0 P1, 4 P2, 6 P3.
- I pre-commit to **GO** on a revision that:
  - resolves P2-1, P2-2, P2-3 and P2-4 by one of the corrections each offers;
  - dispositions P3-1 to P3-6 (fixed, or recorded with their consequence);
  - introduces no new P2 or higher.

## 0. Evidence base

**Object.** The whole draft (399 lines at 0a0545a3): sections 1-6, the amendments 4.1-4.7, the sub-steps H1a, H1b, H2 and H3, and the rows.

**Controlling text read at 0a0545a3:**
- `GwzConnectionApiContract.md`:
  - §5 L210-283;
  - §6 L285-330;
  - §7 L470-505;
  - §7.1 and §7.2 L507-570;
  - §8 L592-630;
  - §9 L632-666;
  - §10 L668-713;
  - §11 L715-742;
  - §15 rows 1.3c/C2(b) L881, 1.5 L882, 1.6 L883 and the integration row L884.
- `GwzTransportReleaseConvergencePlan.md`:
  - §1 and the threat model L40-79;
  - §3 R1 to R9 L164-181;
  - §4 step 3.1 and its Prerequisite L721-728.
- `GwzPhase1-RemPlan.md`: the rows for P2-2, P2-4 and "HTTPS model"; steps R3 and R4; the waves.
- `GwzPhase1-ReviewCode.md`: P2-2, and judged item 3 (native caller per instance).
- `history/GwzP1cHttps-ReviewState.md` (PR5, P3-1) and `history/GwzP1cHttps-ReviewState-2.md` (PR6, PR7, P3-2).

**Code facts checked at the pinned SHAs:**

*Scope record, POST pre-authentication and reuse*
- `https_adapter.rs`:
  - :117-121: `per_exchange` is "not Anonymous".
  - :123-132: the record has one `repository` slot.
  - :162-179: `Scopes::bound` returns early for `Anonymous` and otherwise **overwrites** the whole record: last writer wins, so `HelperToken` can replace `LogonSession`.
  - :308-325: `authorize_reuse` authorizes only a non-per-exchange (anonymous) connection.
- `https_adapter/exchange.rs`:
  - :183: each exchange's `binding` starts `Anonymous`.
  - :206: destination.
  - :219-231: POST pre-authentication by `prior == HelperToken` plus a digest match.
  - :239-240: header and secret dropped.
  - :123: `set_sensitive`.
  - :246-256: only a GET's 401 is answered.
  - :257, :264, :269, :290: classify, content type, body loop, `pipe.end(Ok)`.
- `https_adapter/auth.rs`:
  - :37-38: a challenge that closes its connection is refused.
  - :41-67: the logon session is preferred whenever it is offered and a caller exists; while the gate is unqualified it refuses `Unavailable` and does not downgrade to Basic.
  - :69-80: helper Basic.
  - :81: anything else is `Authentication`.
- `https_adapter/worker.rs`:
  - :258-261: `prior` and `authenticated` are read from the record at start.
  - :395-406: `scopes.bound` runs only after the task returns. A panicked or aborted task does not reach it, and its connection goes `Broken`.
  - :440-446: `end_exchange` aborts.
- `https_adapter/pipe.rs:125-135`: `Work.auth` is a clone of the context.

*Secrets, helper lookup and legacy route*
- `https_auth.rs`:
  - :27: `OUTPUT_LIMIT` is 16 KiB.
  - :48-87: `OperationAuth` has no `Debug`. Its scope is the SHA-256 of the helper executable and environment, and `context()` makes a fresh `Arc` per call.
  - :155-175: lookup errors map to codes.
- `https_auth/secret.rs`:
  - :5-34: `SecretBuffer` and `SecretHeader` are wiped on drop.
  - :97-111: header construction.
  - :113-123: `Debug` prints `REDACTED`; drop overwrites.
  - :125-178: `parse_secret` reads only `username` and `password`; other helper attributes are ignored.
- `https_auth/lookup.rs:22,126`: `url=<destination>` is the full URL, with no `username`.
- `https_destination.rs:116-125`: the request URL is built from the same `Url` and userinfo is cleared, so a key over `Destination` matches the bytes on the wire.
- `https_policy.rs:41-53,55-67,119`: classify; legacy `RouteKey`; `finish`.
- `https_worker/credentials.rs:5-16,53-65`: the legacy route caches `Result<Arc<Credential>, Failure>` per route, so a helper **failure** is memoized too, and the `rejected` flag is checked before sending.

*gwz-transport: context, pool and instance*
- `crates/gwz-connection/src/operation.rs:163-192`: `AuthContext` is `#[derive(Clone]`, holds `Arc<dyn Any>`, and its `Debug` is opaque.
- `binding.rs:164-189`: `BindingShared.auth`.
- `exchange.rs`:
  - :52-95: `Operation` holds `Arc<BindingShared>`, and `cancel_handle` is **weak**.
  - :145: `finish(mut self)` consumes the operation.
  - :320-336: `Live` guards.
- `instance.rs`:
  - :77, :512: pending openings hold an `AuthContext` clone.
  - :554-566: scope fit.
  - :601-630: the cross-binding `authorize_reuse` runs **after** the pool lease, so a pool-level hint passes through it.
- `ports.rs:143`: `ConnectCommand.auth: Option<AuthContext>`.
- `ports.rs:206-215`: `ExchangeIo`.
- `src/pool/allocation.rs:17-53`: lowest idle id of equal key and identity, not excluded.
- `src/pool/mod.rs`:
  - :92-97: `Key { scheme, username, host, port }`.
  - :214-268: `Request` and its validation.

*Test rows*
- `connection_host/https_auth_tests.rs`:
  - :438 an idle connection reused only by an equal context;
  - :497 push authenticated on the advertisement's connection;
  - :558 a POST to an unauthenticated repository is anonymous;
  - :613 a POST on an unauthenticated connection is refused.
- The draft's citations of these four are correct.

**Citation accuracy.** Every code citation I checked resolves to what the draft says, with at most an off-by-one (`exchange.rs:122` vs `:123`). The Git behaviour in §2 and server acceptance of a header-less POST on a GET-authenticated connection are hypotheses, and the draft labels them so. Where I rely on my own unverified knowledge (hyper 1.x buffer reuse, Apache Negotiate defaults, token sizes, server 401 conventions), I mark it **hypothesis**.

## Problem, requirement provenance, and scope

**Stated problem.** Three cases (draft §1 L34-53):
- PR5: the pool places a private repository's POST on an anonymous connection of the same binding.
- PR6/P3-2: a single-slot record is overwritten by a second authenticated repository.
- Phase 3: concurrent private fetches and pushes through one host (plan L724-728).

**Does the design solve it?**
- **Basic/helper: yes.** The POST carries the operation-held credential on any connection of its binding. That closes PR5 and PR6 by construction (rows H1b 1-2).
- **Connection-bound (NTLM/Negotiate): only for the one-repository shape.** A second repository served on an already-authenticated connection is never learned, so PR5 recurs for logon-session pushes with two or more private repositories on one host. That is R1's shape (**P2-2**).

**Mechanism provenance.**

| Mechanism | Traces to | Necessary? |
|---|---|---|
| `Needs` record holding the credential | Operator decision 1 (b2); PR5/PR6 | Yes |
| Service class in the key | Operator decision 4; Code P2-1 of p1c-https | Yes |
| Poisoning on 401 | Git behaviour; the axis requirement "never sent after a 401" | Yes (shape: P3-1) |
| Dedupe ("equal credentials share one `Arc`") | Memory bound only | **No.** 512 x 4 KiB = 2 MiB still fits §10's 16 MiB per admission. It creates the cross-key and cross-host poisoning coupling of P3-1. |
| `closed` flag | None | **No.** It has no reachable effect (P2-3). |
| `Request.prefer`, `connection_bound`, affinity | Operator decision 2 | Yes |
| Re-authentication GET | Decision 2. It is the correctness fallback of a soft hint. | Yes (conditions: P3-6) |
| 8 MiB re-auth cap, 512/64/4 KiB/256 bounds | Drafter's numbers (§6 item 3) | Bounds are required (contract §7 L499-501); the values are judged in §3 |
| Native caller in `AuthContext` | Decision 5; Code P2-2 | Yes |

**Scope.** The task boundary (L8-12) is respected. The amendments touch only contract §4.1, §7, §8 and §15 and the plan's 3.1 Prerequisite, all named. Nothing outside the authorized components is proposed.

## 1. Findings

### P2-1: The scope record's write rule is left unspecified for every new path that authenticates a connection, and the existing rule is last-writer-wins (not monotone)

- **Root cause.** The design changes who authenticates connections:
  - credentialed POSTs from the record, on any connection;
  - GET challenges answered from the record;
  - the re-authentication GET.

  It says only "The scope record keeps the binding kind for `authorize_reuse`" (draft §3 L108). The record is still written by `Scopes::bound` from the exchange's own `binding` variable. That variable starts `Anonymous` (`exchange.rs:183`) and is ignored when `Anonymous` (`https_adapter.rs:169-171`). Otherwise `bound` overwrites the whole record (`:172-177`).
- **Locations:**
  - draft §3 L108;
  - §3.1 "Adapter check and fallback" L232-240;
  - §5 H1b L335 ("`prior`, `authenticated` removed"), although H3's "if the leased connection's scope is `LogonSession`" (L233) needs `prior`;
  - code `https_adapter.rs:162-179`, `exchange.rs:183`.
- **Violated invariant:**
  - Contract §7.2 L554-559: an authenticated NTLM/Negotiate connection "is not a generic account-free TLS lease … No cross-account downgrade to generic reuse is allowed."
  - §15 1.6 L883: logon-session and helper-token connections "are never reused across bindings".
- **Concrete sequences:**
  1. **Re-authentication recorded as anonymous (P0 class if built as the text permits).** Binding A's POST for a `ConnectionBound` key is leased connection c7, which binding B last used anonymously. `authorize_reuse` authorizes it. The adapter runs the re-authentication GET (native three-leg) on c7, then the POST. Nothing in §3.1 makes the POST exchange's `Ended.binding` `LogonSession`: the re-auth is not `auth::answer` on the exchange's own advertisement path. So `bound()` sees `Anonymous`, returns early, and c7 stays recorded `Anonymous` while the server holds it authenticated as A's logon identity. B's next exchange is leased c7, `authorize_reuse` authorizes it (`:313-316`), and B runs as A's Windows identity.
  2. **Downgrade by b2.** Binding A holds `LogonSession` connection c1. A POST for a Basic-recorded key on the same host is leased c1 and carries Basic from its first byte, so `binding = HelperToken` and `bound()` overwrites `LogonSession` with `HelperToken`. Consequences:
     - H3's check "scope is `LogonSession`" is now false for an NTLM-authenticated connection, which costs a needless re-auth.
     - If §6 item 5 (lift the `HelperToken` refusal for equal-scope bindings) is ever taken, a connection the server still holds NTLM-authenticated is handed to another binding.
  3. H1b deletes `prior`, which H3 needs, so H3 must reintroduce the read with no stated rule.
- **Impact.** No leak with today's `authorize_reuse` unless sequence 1 is implemented as the text permits. With it, there is cross-operation identity inheritance on Windows. The record that every cross-binding decision rests on becomes untrustworthy.
- **Required text correction** (in §3, stated as rules):
  - "Every exchange that sends a credential (from the record or a fresh lookup) or completes a native conversation on a connection, including the re-authentication GET, writes that connection's scope record before the connection can return to idle; a task that ends without reporting leaves the connection `Broken` (as `worker.rs:408-422`)."
  - "`LogonSession` is sticky: no later exchange replaces it. `HelperToken` may be replaced only by `LogonSession`."
  - "`prior` (the record read at exchange start) stays; H1b removes only the repository slot."
- **Closure rows:**
  - (a) Re-auth then POST on a connection another binding last used anonymously. Afterwards the scope is `LogonSession`, and binding B's lease of it is `Unprovable` (B gets another connection). The mutation "re-auth path reports `Anonymous`" turns the row red.
  - (b) A Basic-recorded POST on a `LogonSession` connection leaves the scope `LogonSession`.

### P2-2: `ConnectionBound` is learned only when the logon session answers a challenge, so a repository served on an already-authenticated connection is never learned and its POST is refused

- **Root cause.** The learning rule is keyed on "the logon session answered this exchange's 401" (L212-213, `auth.rs:67`). It should be keyed on "an advertisement for K was accepted on a connection the logon session authenticated". The same choice is in `connection_bound()`, which "returns true when its binding was `LogonSession`" (L220-222). "Its binding" is the exchange's own variable, which stays `Anonymous` when no challenge came (`exchange.rs:183`).
- **Location:** draft §3.1 "Learning" L212-213, API bullet L220-222, and "A POST to a key not recorded as `ConnectionBound` is sent as today" (L240).
- **Violated invariant:**
  - operator decision 2 (Windows logon-session push placement fixed before 3.1);
  - RemPlan row "HTTPS model" (an NTLM-style connection-bound push succeeding);
  - the draft's own problem statement (§1 L47-51, R1 shape).
- **Sequence** (one binding, IIS-style NTLM server, two private repositories /a and /b on one host):
  1. GET /a on c1: 401, NTLM, 200. /a is recorded `ConnectionBound`, c1 is `LogonSession`, and affinity (/a to c1) is recorded.
  2. GET /b is leased c1 (lowest idle id). The connection is already authenticated, so there is no 401 and the 200 comes directly. Nothing is recorded for /b, and `connection_bound()` is false, so there is no affinity entry.
  3. POST /a and POST /b run concurrently. POST /a takes c1 by hint. POST /b gets c2 (fresh or anonymous). /b is unrecorded, so it is "sent as today": anonymous, with its pack body on the wire. The server answers 401, and the result is `Authentication`, `Discarded`, `Effect::PossiblySent`.
- **Impact.** PR5 recurs for logon-session pushes with two or more private members per host: the R1 workload. The refusal reads "Authentication" although the logon session had authenticated the binding. Rows 1-6 of §3.1 use one repository and do not cover it.
- **Required text correction:**
  - "Learning: an accepted advertisement for K records `ConnectionBound` when the logon session answered its challenge **or** the connection's scope at the exchange's start was `LogonSession`."
  - "`connection_bound()` is true when the connection's scope after the exchange is `LogonSession`."
  - Trade-off to state: K may in fact be anonymously served, and its POST on a non-`LogonSession` connection then pays a re-authentication. That is acceptable only because native authentication already succeeded for this binding and origin. Alternatively, record a separate `ObservedOnBound` that sets the hint but does not force re-auth, and state that a miss is refused.
- **Closure row.** NTLM-style fixture with /a and /b:
  1. GET /a authenticates c1.
  2. GET /b on c1 is unchallenged.
  3. POST /b is forced onto c2 (c1 held by a long exchange), re-authenticates, and succeeds once.

  The row is red on the text as written.

### P2-3: The record lives as long as the provider `Arc`, not the operation; one provider per admission is asserted but not enforced, and the `closed` flag can never take effect

- **Root cause.** `Needs` is a field of `OperationAuth`, the provider behind a `Clone` `AuthContext` (`operation.rs:163-187`). Its life is the life of the last clone. The draft equates this with "the operation" (L110-112, L143-149) and asserts "Operation B has its own provider even with an equal `AuthScope`" (L174-175). Nothing makes a provider single-admission:
  - the contract itself contemplates "Shared provider objects … while retained, not zero because an Arc was cloned" (contract §10 L692-694);
  - capture happens "once on the original entry thread, before fan-out" (contract L470-471);
  - `OperationCapture.auth` is a plain clonable value held by the caller (`endpoint_environment.rs:148-156`).
- **Location:** draft §3 L110-113; §3(b) "Lifetime" L143-149; R4 bullet L173-178; 4.1 L283 and 4.2 L298 ("wiped when the operation's context is dropped", "shared with nothing outside the operation").
- **Violated invariant:**
  - operator decision 1 ("wiped at the operation's end");
  - 4.2's "shared with nothing outside the operation";
  - plan R4 "no stale event reaches the new operation".
- **Sequences:**
  1. 3.1's composition captures once per command and admits one operation per member, each with a clone of one `AuthContext`. The contract permits this. Then all members share one `Needs`:
     - member 1's 401 makes member 2's key `Rejected` before any byte;
     - cancelling member 3 wipes nothing;
     - the record lives until the last member operation ends.
  2. gwz-py (plan 3.2, R2) caches a capture on a `Client` across calls. Credentials then persist for the Client's life, across Python calls, which is outside the decided lifetime.
  3. `closed` flag: `Drop` of `OperationAuth` runs only when no clone exists, so no straggler can observe `closed = true` afterwards. Nothing else sets it, and the draft says "No hook is added to the host". The flag is a mechanism with no reachable effect, presented as a safety property.
  4. Holders the "Lifetime" bullet does not list:
     - the instance's pending openings (`instance.rs:77`);
     - in-flight `ConnectCommand.auth` in the SSH and HTTPS adapters (`ports.rs:143`), bounded by the setup aggregate;
     - after P2-2 moves the native caller into `OperationAuth`, any native retained-cleanup record that reaches the caller through `work.auth`.
- **Impact.** Retention time and `Rejected` state cross admission boundaries. The operator's decision is not mechanically guaranteed; its validity depends on 3.1 and 3.2 choices the draft does not constrain.
- **Required text correction** (choose one):
  - **(A) Bind the record to one admission.** `OperationAuth` carries a one-shot claim taken by gwz-core's client-side admit path. A second admission with the same provider is refused `InvalidInput` before registration. The contract's "shared provider objects" sentence is then stated not to apply to providers that retain credentials.
  - **(B) Define the unit as one capture.** State that the record is per capture. Require 3.1 and 3.2 to capture per command or call, never cache a capture across calls, and amend 4.1/4.2 ("wiped when the capture's last admission ends").

  In both cases:
  - delete the `closed` flag, or give it a real setter (e.g. a guard returned by `capture_operation` whose drop clears the map, with exchanges holding a `Weak`);
  - enumerate every holder with its bound;
  - state that retained native cleanup holds a `NativeCaller` clone, never the `AuthContext`.
- **Closure rows:**
  - two admissions of one cloned context: second refused (A), or shared exactly as documented (B);
  - after `Operation::finish` with an SSH connect of the same operation still in flight, the provider's strong count reaches 0 within the connect bound;
  - on Windows, after a native exchange ends with cleanup `Pending`, the provider is not held by the retained record.

### P2-4: When the retention bounds overflow, the key falls back to the unrecorded-key path, so a credential the server accepts produces an anonymous POST and a misreported `Authentication`

- **Root cause.** "A credential over 4 KiB or a 65th distinct one is not retained: the key is not recorded, and its POST is refused as an unrecorded key" (L141-142; 4.2 L297 "what does not fit is not recorded"). An unrecorded key's POST is sent anonymously with its body and refused only on the server's 401. Overflow should instead fail before any byte with a typed bound code.
- **Location:** draft §3(b) L138-142; 4.2 L296-297; §6 item 3 L382.
- **Violated invariant:**
  - contract §10 L691-692: charge exhaustion "does not silently omit environment/credential inputs";
  - §11 L726: `InputBudget` is an opening/stream error;
  - the draft's own §1 diagnosability complaint (L50): "the refusal reads 'Authentication' although the helper had a credential";
  - parity with 68db19ae, where a large credential's POST on the advertisement's connection succeeds through a fresh lookup.
- **Sequence:**
  1. The helper returns a 5 KiB password, for example an OAuth access token. That size is a **hypothesis** for some Entra tokens.
  2. The header is about 6.8 KiB, under the 8 KiB server limit the draft cites, so the GET answer gets a 200.
  3. The credential is not retained.
  4. The POST goes anonymously with the full pack, gets a 401 and ends `Authentication`, `Effect::PossiblySent`.

  The 65th distinct credential behaves the same.
- **Impact.** A regression against the current tree for large tokens, even in the sequential, correctly placed case. The pack upload is wasted, and the outcome is misreported.
- **Required text correction:**
  - Overflow records K as `Unretained` (no secret). A POST for an `Unretained` key ends before any byte with `InputBudget` and `Effect::None`.
  - Replace the per-credential 4 KiB limit with the helper output limit (16 KiB, `https_auth.rs:27`), or at least about 6 KiB (an 8 KiB header). Add a total retained-secret bound, for example 256 KiB.
  - Recompute the charge from these bounds.
- **Closure rows:**
  - a 5 KiB credential's push succeeds;
  - at the bound, the POST ends `InputBudget`, `Effect::None`, with zero POST bytes at the server.

### P3-1: Rejection is defined over shared-`Arc` membership, which is neither eager nor scoped to the origin that rejected

- **Root cause.** "Every key sharing that `Arc` becomes `Rejected` at its next use … the secret is wiped when its last key drops it" (L163-165). This is combined with deduplication across all keys, and so across hosts (L136-137).
- **Location:** §3(b) L136-137 and L163-166; §3(d) L183-188; 4.1 L283 ("wiped when … the credential is rejected"); 4.2 L294-297 ("every key sharing that credential rejected"); H1b row 5 L352.
- **Invariant:** 4.1's wipe-on-reject claim, and the axis requirement that a credential is not sent after a rejection.
- **Sequences:**
  - (a) **Lazy wipe.** A rejected credential stays resident until every sharing key is touched again or the operation ends, which contradicts 4.1.
  - (b) **Cross-host poisoning.** Equal bytes for hosts A and B share one `Arc`. A 401 at A rejects B's keys, where the bytes are valid. This fails closed but needlessly.
  - (c) **401 to a fresh lookup is unspecified.** A 401 to a GET answered by a fresh lookup for an unrecorded key: neither whether K becomes `Rejected` nor whether recorded keys holding equal bytes are poisoned is stated. Row 5 says both "records nothing" and "the POST fails fast with no request", which contradict each other.
  - (d) **Re-sending known-rejected bytes.** After K1 is rejected, an unrecorded K2 on the same origin is challenged. The helper returns the same bytes (gwz never calls `reject`), and they are sent again. On directory-backed Basic servers each send counts as a failed login (**hypothesis**). 4.2's "every key sharing that credential" reads as covering K2 but does not.
- **Required correction:**
  - poison eagerly under the lock: every entry sharing the credential becomes `Rejected` at once;
  - scope rejection to (scheme, host, port);
  - retain, per rejected origin, a keyed fingerprint (HMAC-SHA-256 under a random per-operation key; 32 bytes, not the secret) so that a later lookup returning equal bytes for that origin is refused before sending;
  - state the fresh-lookup 401 rule and fix row 5;
  - say whether "on poison" (L158) means credential poisoning or mutex poisoning. It should mean both, and a poisoned mutex should clear the map.
- **Closure rows:**
  - after a 401 the secret buffers read zero at once (not at the next use);
  - a 401 at host A leaves host B's equal credential usable;
  - K2 with equal bytes after K1's rejection: no request carries them.

### P3-2: The connection-bound fallback's branches are not enumerated, so "fails closed" is not stated at every new branch

- **Location:** §3.1 L232-240; 4.4 L306-312.
- **Root cause.** The fallback is specified only on its success path, plus "if it fails".
- **Unspecified branches:**
  - (a) The re-authentication GET returns 200 without a challenge (a backend with another configuration). The connection is not authenticated, and as written the POST is sent anonymously with its body. It must end before any POST byte: `Authentication`, `Effect::None`.
  - (b) The re-auth GET's challenge offers only Basic, or Basic as well as Negotiate on a path where the native route is unavailable. The re-auth must be answered only by the logon session, never by the helper and never from a Basic record. Otherwise it ends.
  - (c) The POST gets a 401 after a successful re-auth (the server does not honour connection authentication for POSTs; §6 L389-390 is unverified). The key's state is unstated. Without `Rejected`, every later POST of the key repeats GET plus NTLM plus a lost body.
  - (d) A key seen both ways (`ConnectionBound`, then a Basic-only challenge, or the reverse): the `Need` transition is unstated, and "first insert wins" is stated only for equal kinds.
- **Required correction.** A transition table for `Need` (`Unrecorded`, `Basic`, `ConnectionBound`, `Rejected`, and `Unretained` from P2-4) over these events, each ending before any POST byte where authentication is not established.
- **Closure rows:** (a), (b) and (c), each asserting zero POST bytes at the server and the typed code.

### P3-3: The key omits the pool `Key`'s scheme and username

- **Location:** §3(a) L120-121, "domain-separated SHA-256 of (host, port, normalized path, class)". `pool::Key` is `{ scheme, username, host, port }` (`gwz-transport/src/pool/mod.rs:92-97`).
- **Root cause.** The record key is coarser than the identity that selects connections and, once fixed, the credential.
- **Sequence.** The draft's own observation (L394-396) is that no `user@` reaches `git credential fill`. When that is fixed before 3.1, `https://alice@host/x` and `https://bob@host/x` in one operation share a key, and alice's retained credential is sent on bob's exchange. A future adapter that sends Basic without TLS would also match an HTTPS key, since the scheme is omitted.
- **Required correction.** The key is the domain-separated digest over the full pool `Key` (scheme, username, host, port), the normalized path and the class.
- **Closure row.** Two keys differing only in username, same path: neither receives the other's credential.

### P3-4: The declared charge is an estimate, not a maximum

- **Location:** §3(b) L140, "Declared `charge` about 280 KiB (512 x 48 B + 64 x 4 KiB)". Contract §7 L499-501: "the provider must report a maximum retained charge".
- **Root cause.** The figure leaves out:
  - map node overhead;
  - `Arc` headers;
  - two `Vec`s per `Secret` (the username is allocated at len+1, `secret.rs:173`);
  - the `Need` tag;
  - P3-1's fingerprints and P2-4's `Unretained` entries.
- **Required correction.** A stated formula that is an upper bound at the configured limits, or byte accounting at insert that refuses beyond the declared charge.
- **Closure.** A unit test asserts the declared charge is at least the sum of `size_of` values and capacities with every bound full.

### P3-5: The proposed 4.1 text claims absolute non-retention that the mechanism does not provide

- **Location:** 4.1 L280-283: "pools and connections keep no credential … wiped when the operation ends". Compare §3(b) "Not covered" L157-158.
- **Root cause.** The residuals stated in §3(b) are dropped from the normative text, and two are missing.
- **Residuals the text omits:**
  - **Hyper's buffer.** Hyper's per-connection header write buffer is reused and not zeroed (**hypothesis**, from hyper 1.x). It keeps the last base64 `Authorization` bytes on an idle connection until that connection is disposed. A `HelperToken` or `LogonSession` connection cannot be leased by any other binding after its binding ends (`authorize_reuse`), yet it stays idle for up to 60 s (contract §10 L681), holding the residue and a pool slot.
  - **Crash dumps.** A crash dump of the client (Linux core, Windows WER) during an operation now holds up to 64 credentials for the whole operation, instead of one lookup's lifetime.
- **Required correction:**
  - 4.1 names these residuals (Hyper and TLS buffers of connections the operation authenticated, until disposed; process dumps; swap).
  - Optionally: "a per-exchange-scoped idle connection is retired when its binding ends", since no other binding can use it.
- **Closure.** The sentence in 4.1. Optionally a row: after `finish`, the binding's `HelperToken` connections are closed.

### P3-6: The re-authentication GET exception is not stated as conditions, and its body cap fails closed on large advertisements

- **Location:** 4.4 L306-312 and §3.1 L236-244. The exception is to contract §7.2 L564-565.
- **Root cause.** The safety basis and the conditions under which the adapter may repeat an advertisement are prose in §3.1 L242-244 and are absent from 4.4.
- **Gaps:**
  - 4.4 also overclaims ("the only request the adapter sends that the operation did not make"): challenge answers and native rounds are also adapter-originated.
  - The 8 MiB cap refuses keys whose advertisement is larger, such as repositories with very many refs, every time the hint misses, although the operation already read that advertisement in full.
- **Required correction.** 4.4 states:
  - (i) the basis: an `info/refs?service=` GET is a safe, read-only request in Git smart HTTP;
  - (ii) it is sent only for a key whose own advertisement this operation already had accepted, with identical method, path, query and headers (including any future `Git-Protocol` header);
  - (iii) at most once per POST exchange, inside that exchange's budget, before any POST byte;
  - (iv) its body is discarded under a cap. Recommend `max(8 MiB, 1.25 x` the advertisement length the operation observed for K, stored non-secret in the `ConnectionBound` record`)`;
  - (v) each one is counted in the operation's observations.
- **Closure.** A row in which the re-auth GET's request line and headers equal the original advertisement's. A row with an advertisement larger than 8 MiB whose POST succeeds after a hint miss.

## 2. Invariant analysis

| Invariant (source) | Holds in the design? | Evidence / finding |
|---|---|---|
| Credential reaches only the key that challenged and accepted it (contract §7.2; plan §1) | **Yes**, for Basic. The key is computed from the same `Destination` the request is built from (`https_destination.rs:116-125`). Spelling variants fail closed (PR7). | P3-3 narrows it further |
| No credential on an unchallenged GET (§3(c)) | Yes. Advertisements stay anonymous first. | — |
| No credential to another binding or operation | Yes for the record: the provider is per context, and `authorize_reuse` is unchanged. **Not guaranteed per admission.** | P2-3 |
| The core never receives the provider, a header or the record (cli) | Yes. The carrier carries a typed Open only (plan L75-77). The hint and affinity are client-local, and `prefer` is not a carrier field; the text should say so in 4.5. | — |
| Hint cannot bypass cross-binding authorization | Yes. The pool lease precedes the instance's `authorize_reuse` (`instance.rs:601-630`), and H2 places `prefer` in `schedule`. | Keep row 5 |
| Logon-session connection never serves another binding | Yes today; **at risk** on the new write paths | P2-1 |
| Credential wiped at operation end | Wiped when the provider's last clone drops | P2-3, P3-5 |
| Never sent after a rejection | For keys sharing the `Arc`, yes (lazily). For later keys with equal bytes, no. | P3-1 |
| Never written to logs, `Debug` or panics | Yes. `Secret` prints REDACTED, `AuthContext(..)`, counts-only `Needs`, digests-only `NeedKey`; `Destination` redacts the username. No path formats a header. | — |
| Fails closed at every new branch | Basic: yes. Overflow: closed but after the body is sent. Connection-bound fallback: branches unstated. | P2-4, P3-2 |
| No replay (contract §9) | Yes. The POST is sent once; the re-auth GET precedes its first byte. | P3-6 |
| Degraded native route does not downgrade | Yes. An unqualified gate gives `Unavailable` before any credential (`auth.rs:45-47`). The logon session is preferred whenever offered, never Basic in its place. | — |
| Bounded retention (contract §7, §10) | Bounded, but the charge is not a maximum | P3-4 |

**Disclosure scale.**
- Per operation: up to 64 credentials (draft bound) for the operation's duration.
- Per host: 8 active admissions (contract §10 L677) x 64 = 512 resident credentials at worst. Waiting admissions have empty records.
- This is acceptable against the plan's threat model, which places same-process readers outside it, provided P3-5's residual text is added.

**Stuck states:**
- **Affinity table full:** oldest evicted, a miss, then re-auth. Bounded.
- **Credential bound reached:** P2-4.
- **Helper hang:** at most 8 lookups per adapter and 8 slots per host, bounded by allocation plus a 120 s interaction cap, reaped on cancel. b2 reduces spawns to one per unrecorded key.
- **Busy-wait:** none is built.

## 3. Judgement on the draft's open items (§6), each with a recommendation

1. **`approve`/`reject` and pre-authenticated advertisements: keep out (agree).**
   - `reject` erases the user's stored credential on what may be a transient 401: a destructive side effect on the user's store.
   - Pre-authenticated advertisements would send a credential to a GET the server may serve to anyone.
   - With P3-1's fingerprint, the in-operation re-send gap that `reject` would otherwise close is closed without touching the store.
2. **A bounded wait for a busy preferred connection: do not build (agree).** The fallback is correct by construction once P3-2 is closed. Count fallbacks and re-auth bytes in the 3.4 sweep.
   - Risk (§4): the affinity entry is written at `Instance::settle`, after the GET's full settlement. If gwz-git opens the POST before the GET exchange has settled, the hint misses systematically in the sequential case. A row should pin gwz-git's actual ordering.
3. **The numbers:**
   - **512 keys: accept.** 16 members x 2 classes is 32.
   - **64 distinct credentials: accept.**
   - **4 KiB per credential: reject.** Use 16 KiB, or at least about 6 KiB, plus a total-bytes bound, and fail typed before sending (P2-4).
   - **256 affinity entries: accept.**
   - **8 MiB re-auth cap: accept as a floor, scaled by the observed advertisement length (P3-6).**
   - **Charge: state a maximum (P3-4).**
4. **Caller identity in `AuthScope`: do not add it.**
   - `AuthScope` partitions pool identities. Adding the caller would partition anonymous connections that carry no identity.
   - Safety does not need it while `LogonSession` connections are refused to every other binding. State that refusal as unconditional, independent of scope, and make it sticky (P2-1).
   - If a later decision ever shares `LogonSession` connections, the caller's identity (SID, LUID, logon session) must then enter the scope record of that connection, not `AuthScope`.
5. **Sharing helper-token connections across equal-scope bindings: not now.**
   - Prerequisites: P2-1's sticky `LogonSession`, so that no NTLM-authenticated connection can carry a `HelperToken` record, and evidence that no supported server ties Basic to a connection.
   - After those, lifting it for equal `AuthScope` with `HelperToken`-only history is defensible: the same helper environment yields the same credential, and nothing credential-bearing persists in the adapter for the connection. It remains a separate Safety decision, as the draft says.
6. **The re-authentication GET as an exception to contract L565: agree, conditionally.** Agreement is subject to the five conditions of P3-6 in 4.4 and the branch table of P3-2.
   - It causes no server-side effect beyond a repeated read: rate-limit and audit counts only (**hypothesis** for exotic hooks).
   - It is not a replay of the POST.
   - It sends the native token only in answer to a challenge, on the same origin, over a TLS-bound conversation.

## 4. Risks and next action

**Risks (not findings):**
- **Server acceptance of a header-less POST** on a connection the GET authenticated. This holds for IIS-style NTLM. **Hypothesis:** for Kerberos servers that authenticate per request (Apache `mod_auth_gssapi` by default), every POST is challenged and refused. This fails closed and matches today's route, but §6 L389-390's "as defined" overclaims. Label it, and add the Windows receipt as the evidence.
- **Token expiry mid-operation.** `parse_secret` ignores helper attributes beyond `username` and `password` (`secret.rs:125-178`), including Git's `password_expiry_utc`. A retained OAuth token can expire inside a long operation. That poisons every key sharing it for the rest of the operation, where per-POST lookups would have refreshed it.
- **Hint efficacy depends on gwz-git's GET finish preceding the POST's submit** (item 2 above).
- **Cross-key poisoning on servers that answer "authenticated but no access to this repository" with 401** (**hypothesis**) fails a whole multi-member command. P3-1's origin scoping limits the damage, but not within one origin.

**Next action.** Revision 3 of the draft should:
- fix P2-1 to P2-4, each by one offered correction;
- disposition P3-1 to P3-6;
- add the named closure rows to H1b (P2-3, P2-4, P3-1, P3-3) and H3 (P2-1, P2-2, P3-2, P3-6).

A re-review need only re-run the sequences above. The pre-commitment in the header stands.

## 5. Out-of-scope observations

- **Account selection by URL (pre-existing; the draft's own L394-396).** The host adapter's lookup sends `url=` with no username, so a `user@` in a remote never reaches `git credential fill`. With b2 the wrong account's credential would be retained and spread to every POST of the key. Recommend recording it as a named prerequisite of 3.1, not a conditional report. It interacts with P3-3.
- **Helper failures are not memoized on the host route (pre-existing).** The legacy route caches a lookup's `Failure` per route (`credentials.rs:16`). The host adapter re-spawns the helper at every challenge to an unrecorded key. With an interactive helper, a 16-member operation can raise up to 8 concurrent prompts (the slot cap) before the first success. Impact: user experience and robustness only.
- **Unrecorded keys on a `LogonSession` connection (inherent to connection authentication, unchanged by the draft).** A POST for an unrecorded or publicly served key placed on such a connection carries the binding's Windows identity to the same server. This is no new disclosure, but the contract text could state it beside 4.4.
