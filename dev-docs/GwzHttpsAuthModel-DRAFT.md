# GWZ HTTPS authentication model: credential need on the operation (design, DRAFT)

**DRAFT, 2026-10-31.** Not reviewed, not accepted. Design only (one step is sketched in section 5). Object:
contract `GwzConnectionApiContract.md` section 7; gwz-core `a7b965c5` (`https_adapter*` at `68db19ae`);
gwz-transport `0a25363`.

**Task boundary (recorded before drafting).** Deliverable: this document. Components a later step may change:
gwz-core `src/git/endpoint/https_adapter*`, `https_auth*`, `transport_host/connection_host/https*_tests.rs`, and the
root contract and plan text. Reference only, no change proposed: gwz-transport (`gwz-connection`, `pool`),
`https_worker/` (legacy route and native), gwz-sspi, gwz-git. Citations are `file:line` in the trees above.

Note: contract section 7.1 is the SSH reuse authorization (the pool-key sentence is at L509-510 inside it); the
HTTPS scope text is section 7.2 (L534-570).

## 1. Problem

Git smart HTTP makes two requests per repository: a GET advertisement (`/<repo>/info/refs?service=...`) and a
POST (`/<repo>/git-upload-pack` or `git-receive-pack`) whose body is streamed. A 401 to the GET is answered by
asking the credential helper and sending the GET again; the GET has no body. A 401 to the POST cannot be answered:
its body is on the wire and contract section 9 forbids replay (contract L663-666, "Connection cannot replay").
Challenged POST refusal is a standing part of the mechanism matrix (L561-563). So a POST that needs a credential
must carry it from its first byte, and the sender must know that before the first byte.

**What the host-owned HTTPS adapter does today.** It remembers the need on the CONNECTION. Each connection has a
scope record (`https_adapter.rs:123-132`): the binding kind (`Anonymous`, `HelperToken`, `LogonSession`) and one
SHA-256 digest of the path of the last helper-authenticated exchange. A POST is pre-authenticated, with a fresh
helper lookup, only when `work.prior == HelperToken` and the digest equals the POST's own path
(`exchange.rs:219-231`). Every other POST goes out anonymously; a 401 to it is classified `Fail(Authentication)`
(`https_policy.rs:48`) and the exchange ends `Discarded`. This fails closed and is pinned by
`a_post_on_a_connection_nothing_authenticated_is_refused_after_one_anonymous_attempt`
(`connection_host/https_auth_tests.rs:613`).

**Where it fails.** The POST needs three things to line up that the adapter does not control: the pool must lease
it the connection its advertisement authenticated, no other authenticated exchange may have overwritten the one
slot, and the exchange must be the same binding. The pool picks the lowest idle connection id of equal key and
identity (`gwz-transport/src/pool/allocation.rs:22-24`, a `BTreeMap` scan), with no notion of which connection
holds what.

- **PR5, placement (State review 1, `GwzP1cHttps-ReviewState.md:63`, `:96-102`).** One binding holds anonymous
  connection 1 and helper-authenticated connection 2 (two overlapping advertisements: a public and a private
  repository). The private POST is given connection 1, goes anonymously, is challenged and refused:
  `PermissionDenied`, `Discarded`. Same outcome for a POST on a fresh connection (PR2).
- **PR6 / P3-2, single slot (State review 2, `GwzP1cHttps-ReviewState-2.md:34`, `:85-98`; Code review 2 probe 2).**
  One binding, one connection: GET `/a` authenticated, GET `/b` authenticated (the slot becomes `/b`), POST `/a`
  on the same connection: anonymous, refused. Both reviewers filed it as P3-2; the lane closed it by documenting
  the limit (contract L881, "a single SHA-256 slot").

**Why it matters for Phase 3.** Step 3.1 routes every CLI command through one host (plan L721-728). R1 is 16
members at one origin, `fetch` then `push` then post-push reads (plan L166), with up to `per_user_host` (32)
connections per scope. Each member's advertisement and POST are separate leases. A workspace with private
members will meet PR5 and PR6 routinely, and the refusal reads "Authentication" although the helper had a
credential. The plan already records this as the step 3.1 prerequisite (plan L724-728). The legacy route does not
have the problem: it keeps the need on the route, per operation, until the operation ends
(`https_policy.rs:70-72`, `:119`, `basic_install` at `https_worker/native/exchange.rs:459-462`, POST pre-authentication
`https_worker/prepare.rs:144-149`, `:165`).

## 2. The two authentication models

| Mechanism | What carries the authentication | Per request or per connection | In gwz today |
|---|---|---|---|
| Basic with a helper-supplied credential (GitHub and GitLab tokens, `gh`, osxkeychain, GCM) | An `Authorization` header on each request | Per request | Supported: `auth.rs:69-80`; header built only as `Basic base64(user:password)` (`https_auth/secret.rs:97-113`) |
| Bearer or other token scheme | An `Authorization` header on each request | Per request | Not supported: a challenge offering nothing but it ends `Authentication` (`auth.rs:81`); `parse_secret` reads username and password only |
| NTLM | The TCP/TLS connection, after a 3-leg conversation | Per connection | Supported on Windows through gwz-sspi, `auth.rs:41-68` |
| Negotiate (Kerberos or NTLM, Windows picks) | The connection, as implemented | Per connection in gwz | Same path |
| Digest | n/a | n/a | Refused (`native.rs:142-146`, `Offers`: not an offer) |

**Can Negotiate or Kerberos be per request here? No, not as the code stands.** I checked what would have to be
true and found it is not:

- The adapter starts a native conversation only after a 401, never with an unsolicited first token
  (`auth.rs:41-68`; `Offer::initial` refuses a challenge that already carries a token, `native.rs:134-139`).
- The channel binding is that physical connection's own final-origin TLS binding, and the request is built per
  connection (`native/connection.rs:119-122`); one context, one conversation, at most eight rounds, no native
  context reuse (`GwzSspiDesign.md:66-67`).
- Negotiate may select Kerberos or NTLM and the selection is observed only after processing; there is no
  per-mechanism policy knob and Kerberos-only callers "must not start it" (`GwzSspiDesign.md:72-84`). The adapter
  cannot know in advance that a Negotiate exchange will end in a per-request-capable mechanism, so it must treat
  every Negotiate result as connection-bound. That is what `Binding::LogonSession` does.
- Per-request Kerberos would need an optimistic first token, a Kerberos-only knob and a willing server; none
  exists here. It is a possible future, not a premise.

Consequence for the design: the helper-supplied (Basic) route is request-scoped in the protocol but the adapter
still models its connections as bound (`Binding::HelperToken` is `per_exchange()`, `https_adapter.rs:119-121`,
and `authorize_reuse` refuses it to other bindings, `:308-325`). Only the logon-session route is bound by the
mechanism itself.

**How Git itself behaves** (from my knowledge of git's `http.c`, `credential.c` and `remote-curl.c`; **not
verified in this session**; confirm against the installed Git before this text is quoted anywhere):

- Git asks for a credential once per URL per process, when the first 401 arrives (`credential_fill`), and keeps
  it in `http_auth` for the rest of the command. libcurl then sends it on later requests to that host without a
  new challenge once the method is settled, so a streamed POST carries it.
- `credential.useHttpPath` (default false) decides whether the path is part of the credential description given
  to helpers: false gives one credential per host, true gives one per repository path. Matching of
  `credential.<url>.*` sections uses the full URL either way.
- After a request that used a filled credential succeeds Git calls `credential approve` (helpers that store on
  `store` learn the credential). After a 401 to a request that carried one it calls `credential reject` and clears
  it, so the next fill asks again.
- A POST is replayable in Git when its body is buffered (smaller than `http.postBuffer`, default 1 MiB). For
  larger bodies `remote-curl.c` sends a small probe POST first to draw the 401. gwz has neither: contract section 9
  forbids replay, and a probe POST is a Git-service decision that belongs in gwz-git, not in the generic adapter.

## 3. The proposed model

**Rule.** "This operation learned that repository R, for this service class, needs a credential" is operation
state. It is recorded on the operation's `AuthContext` provider (`https_auth::OperationAuth`, `https_auth.rs:48`)
the moment an authenticated advertisement is accepted. Every later exchange of that operation to R and class, POSTs
included, carries a credential from its first byte on whichever connection the pool leased. The scope record keeps
only the binding kind, which `authorize_reuse` reads for connection-bound mechanisms, unchanged.

Where the state lives: `OperationAuth` is built once per operation by `capture_operation`
(`endpoint_environment.rs:207`), reaches every exchange as `Work.auth` (`exchange.rs:54`, `pipe.rs:130`), and dies
with the operation's binding. It is already where a helper environment lives (contract L483-485). Interior mutability is needed, and the declared `charge`
(`operation.rs:171`, contract L686) must cover the bound below.

**(a) Repository key.** Options: (1) SHA-256 of the exact `work.path` as today; (2) SHA-256 over the `Destination`
the request is built from (`exchange.rs:206`, host, port, path as `url::Url` normalizes it: dot segments removed,
userinfo and query absent, trailing empty segment popped), plus the service class; (3) fold `/repo` and
`/repo.git`, case, or trailing slashes. Option 3 guesses server behaviour and is rejected: PR7 shows an unfolded
spelling fails closed, which is the safe direction. Option 1 omits the origin and the class, both needed once
the key lives beyond one connection. **Recommend option 2**: key = domain-separated SHA-256 of (host, port,
normalized path, class) where class is fetch-class (`upload-pack`) or push-class (`receive-pack`), as the legacy
`RouteKey` already splits (`https_policy.rs:55-58`). The class matters: a repository readable anonymously and
writable only with a credential must keep its fetches anonymous (the Code P2-1 finding,
`GwzP1cHttps-ReviewCode.md` P2-1: a request the server serves to anyone must not depend on a helper).
`credential.useHttpPath` does not change the key. The lookup passes the full URL to `git credential fill`
(`https_auth/lookup.rs:22,126`), so Git itself drops the path before helpers when `useHttpPath` is false. The
key is finer than Git's per-host credential in that case: each repository learns for itself, and two repositories
on one host then get the same credential from the helper. That costs a 401 per repository, never a wrong send.

**(b) What is held.** Options: (b1) only the fact, with a fresh helper lookup per credentialed request (the
current contract: "No token cache in host", L494; C2(b) closed on "a fresh per-exchange helper lookup, nothing
kept", contract L881); (b2) the credential itself in the operation's provider, zeroized at operation end (what
Git and the legacy route do: `https_worker/credentials.rs:5-16`, `Answers` per route, `Secret` overwritten on
drop, `secret.rs:118`).

| | (b1) fact only | (b2) credential in operation memory |
|---|---|---|
| Secret exposure in the client process | Microseconds per request, wiped (`secret.rs`, `wiped.rs`) | Operation lifetime; same as Git and legacy; swap and core-dump window |
| R4 and R7 | Nothing to leak across operations or to the core | Per-operation provider, so no cross-operation reach; core never sees it (f); needs a cross-binding no-leak row |
| Helper cost | One `git credential fill` per credentialed request: at least 2 per private member (challenged GET, POST), more per v0 negotiation round | One per repository, single-flight (h) |
| Interactive helpers | May prompt or re-prompt per request; without `approve`, a prompted credential is never stored (hypothesis, not measured) | One prompt per repository per operation |
| Consistency | GET and POST could get different accounts if the helper changes its answer mid-operation | One credential per repository per operation |
| Contract change | None to the "no token cache" rule | Amend L494: the operation's own provider may hold what it obtained, never the host, instance or pool |
| Size | Smallest | Adds single-flight cells, a bounded secret store, wipe-on-cancel rows |

**Recommend (b1) for the first landing**, with the record shaped so (b2) is additive (`Needed` becomes
`Needed { credential: Option<Secret> }`). Reasons: it is sufficient for the defect (PR5, PR6); it keeps the
accepted "nothing kept" rule and the C2(b) rows green; optional improvements must not become prerequisites for 3.1
(user standing rule). The trigger for (b2) is a measurement, not a preference: in the Phase 3 sweep (R1, 16
private members), if helper spawns per command exceed a bound the operator sets, or any supported helper
re-prompts, build (b2). This is the threat-model trade the operator should see: (b2) buys fewer spawns and
prompts at the price of a longer secret lifetime on the less exposed side (the client).

**(c) Pre-authenticate the advertisement GET once known.** It would save one 401 round trip per repeat GET of the
same key in an operation (for example a post-push read of a repository the fetch learned). A GET can be sent
anonymously on helper failure, so a failed pre-auth could degrade. **Recommend not in this step.** The POST
defect does not need it; it adds a degrade-on-decline path and its rows; and the first advertisement of each key is
unknown anyway. Revisit with the sweep's 401 count. The record supports it without change.

**(d) Failure handling.** A pre-authenticated request whose response is 401 (expired or wrong credential):
the exchange ends `Authentication`, `Discarded`, never replayed, the connection closed (as now, `exchange.rs:257`
with `https_policy.rs:48`). New: the key is marked `Rejected` on the operation. Later exchanges of the operation
for that key fail with `Authentication` before any helper spawn and before any byte (`Effect::None`), as the legacy
route's `rejected` flag does (`credentials.rs:55`). The same marking applies to a helper credential answered
at the advertisement and rejected again. `Rejected` is operation-scoped and not sticky across operations, so a
transient 401 costs one operation. A key becomes `Needed` only after an authenticated 200 with the right content
type (as the legacy `basic_install` does after validation), never at the first 401. If the helper fails at a
POST's lookup, the POST ends before any byte with the helper's mapped code (`AuthError::code`,
`https_auth.rs:155`); that differs from the P2-1 case because this key was learned, not assumed.

**(e) R4 and scope partitioning.** Unchanged: `HttpsScoped` still partitions the pool by `AuthScope`
(`instance.rs:567`), and `authorize_reuse` still returns false for `HelperToken` and `LogonSession` connections to
any binding but the last lessee (`https_adapter.rs:308-325`). The new record is not a scope input and is not in a
profile, pool key, instance or `Scopes`. Two operations with equal scopes have two providers and two records: B
never inherits A's learning, even on a connection A authenticated (which B cannot lease anyway) or on an anonymous
one. Because the context is immutable after capture, a changed `GH_TOKEN` or `GIT_CONFIG_GLOBAL` is a new
operation with an empty record. The mutation that would break this (the record moved into adapter-shared state)
is the cross-binding row in section 5.

**(f) The served `cli` placement.** The provider, the record and every lookup are in the client's process; the
core never sees a credential, a header, or the record (plan threat model L55-62; R7). The core sends only a typed
Open mapped to an admitted (scheme, host, port, path, service class); the key is derived on the client from that
admitted tuple and the server's own challenge. The core cannot assert a need, cannot widen the key, and a
POST for an unlearned key goes out anonymously. The error the core reads is the existing typed `Authentication`.

**(g) `git credential approve` and `reject`** (never called; recorded limitation, `GwzP1cHttps-RemPlan.md` last
row, State review 1 section 4). **Recommend out of scope here, kept as a named follow-up.** Reasons: it is a new
side effect on the user's credential store; `reject` lets a transient or hostile 401 erase a good stored
credential; it needs the secret alive at response time (b1 drops it after the request is built,
`exchange.rs:239-240`); and PR5 and PR6 do not need it. Its absence is real: a stale stored token fails every gwz
command until the user rejects it by other means. `Rejected` bounds the damage to one failed request per key.

**(h) Concurrency.** Two exchanges of one operation racing to learn the same key: under (b1) both answer their
own challenge and insert the same key; the insert is idempotent and the map holds one entry. `Rejected` wins over
`Needed` (a late `Needed` insert is a no-op on a rejected key). The ordering that matters is different: gwz-git
sends a key's POST after it has read the advertisement, possibly on another connection. Today the connection's
scope record is written only after the exchange task returns (`worker.rs:395-406`), after `pipe.end(Ok)`
(`exchange.rs:290`), so it is not visible at that moment. The operation record must be written **before the first
response byte is delivered to the client**: in `exchange`, after the content-type check (`exchange.rs:264`) and
before the body loop (`:269`). Then any client that has read a byte of the advertisement is guaranteed to find
the fact. Under (b2) single-flight per key (a cell, as legacy `Answers`) would be added.

**Connection-bound mechanisms (NTLM, Negotiate) keep the current rule.** A POST is authenticated by being on the
connection the logon session authenticated; `authorize_reuse` keeps that connection with its binding. The same
placement hazard as PR5 exists there (the pool may hand the POST an unauthenticated connection) and this design
does not remove it. The legacy route avoided it by keeping the exchange's lease from discovery to POST
(`https_worker/serve.rs:66-71`, `native_route`); the host model has separate leases. Options: (U1) a placement
hint to the pool so a binding's POST prefers its own authenticated connection (gwz-transport change); (U2) before
a POST on an unauthenticated connection to a key learned as connection-bound, re-authenticate that connection with
a bodiless GET (a new request, not a replay of the POST, but an adapter-initiated request that contract
L563-565 reserves to the protocol profile, so it needs contract text); (U3) accept the refusal and record it.
**Recommend U3 for this step**, and raise U1 or U2 as a Windows-parity decision (section 6, item 1).

## 4. Proposed contract text

**4.1 Contract section 7 table, HTTP credential adapter row (L494), last cell.** Replace "No token cache in host or
account inherited from a TLS pool" with:

> No token cache in host, endpoint instance or pool, and no account inherited from a TLS pool. The operation's own
> `AuthContext` provider holds one fact per (origin, repository path, service class): that a credential was needed.
> It holds no credential and is dropped with the operation.

**4.2 Contract section 7.2, new paragraph after L557 ("...as the current native contract requires.").**

> **Credential need is operation state.** For a request-scoped credential (Basic from a configured helper), whether
> a repository needs a credential is recorded on the operation, not on the connection. When an exchange of an
> operation receives an authenticated success to an advertisement that was challenged, the adapter records, in that
> operation's `AuthContext` provider, the key (origin, normalized repository path, service class) as needed, before
> delivering any byte of that response. Every later exchange of the same operation for that key, a POST included,
> asks the operation's helper for a credential for that exchange and sends it from its first byte, on whichever
> connection the pool leased. A key the operation has not recorded is sent without a credential; a challenge to such
> a POST is refused as `Authentication` and never replayed. A 401 to a request that carried a credential ends it
> `Authentication`, closes its connection, and marks the key rejected for the rest of the operation: later
> exchanges for it fail before any helper lookup and before any byte. If the helper fails for a recorded key, the
> exchange ends before any byte with the helper's mapped code. The record is bounded (512 keys per operation,
> charged in the context); when full, further keys are not recorded and their POSTs are refused as above. The
> record is never shared with another operation, instance, profile, pool or connection, and holds digests only.
> Connection scope for these exchanges is unchanged: a connection a helper token authenticated is leased only to
> the binding that last used it.

**4.3 Contract section 7.2, replace L561-563's "challenged POST refusal" with** "challenged POST refusal for a key the
operation has not recorded".

**4.4 Contract section 7.2, add at the end of the paragraph "NTLM/Negotiate must retain...":**

> NTLM and Negotiate are connection-bound as a whole, whichever mechanism Windows selects. A POST is authenticated
> only by being on a connection the logon session authenticated for its binding; a POST the pool places on any other
> connection is refused as `Authentication`. This is a recorded limitation until placement or re-authentication is
> decided (section 6 of this draft).

**4.5 Section 15, new row** (appended; the 1.3c row's "POST rule" sentence is replaced by "see the next row"):

| Existing authority | Replacement / retained rule |
|---|---|
| Section 7 (HTTP credential adapter row); section 7.2 (HTTPS authentication scope, challenged POST refusal) | Recorded at step 3.0c (gwz-core `<commit>`; evidence `<run>`; Code and State `<verdicts>`): credential need is recorded on the operation per (origin, path, class) and a POST for a recorded key carries a fresh helper credential on any connection; the connection scope record no longer holds a repository digest (PR5 and PR6 turn green). Retained: no replay, no token cache outside the operation's provider (nothing is cached at all in 3.0c), `authorize_reuse` unchanged, NTLM/Negotiate connection-bound with the placement limitation. Not done: `credential approve`/`reject`; pre-authenticating advertisements; (b2) credential retention. |

**4.6 Plan.** Step 3.1's "Prerequisite" paragraph becomes "Prerequisite: step 3.0c". Add after 3.0b: "**3.0c HTTPS
credential need on the operation** (this document, section 5). ~450 including rows."

## 5. Implementation step sketch (proposed step 3.0c, lands before 3.1)

Repositories: gwz-core, plus the root contract and plan text. No gwz-transport or public API change (the provider
is crate-private), so no Surface review.

| File (gwz-core `src/`) | Change | Lines |
|---|---|---|
| `git/endpoint/https_auth/needs.rs` (new) | `NeedKey` (domain-separated SHA-256 of host, port, path, class), `Needs` (`Mutex<BTreeMap<NeedKey, Need>>`, `Need = Needed \| Rejected`, `MAX_NEEDS = 512`, `learn`, `is_needed`, `reject`, overflow flag) | +110 |
| `git/endpoint/https_auth.rs` | `OperationAuth` owns `Needs`; accessor; `context()` adds `MAX_NEEDS * 40` bytes to the declared charge | +15 |
| `git/endpoint/https_adapter/exchange.rs` | Compute the key from `destination` and `work.service`; replace `authenticated_here` (`:219-220`) by the record; record `Needed` after `content_matches`, before the body loop; mark `Rejected` on a 401 to a credentialed request; fail fast on `Rejected`; release the secret after the response head | +45 / -20 |
| `git/endpoint/https_adapter.rs` | Remove `RepositoryDigest`, `repository_digest`, `ConnectionScope.repository`, the `bound` parameter; rewrite module doc lines 15-24 | +10 / -40 |
| `git/endpoint/https_adapter/{worker,pipe}.rs` | Remove `prior`/`authenticated` plumbing (`worker.rs:258-261, :395-405`, `pipe.rs:131-132`); `Ended.repository` goes. Dead code removed in the same change | -15 |
| `transport_host/connection_host/https_needs_tests.rs` (new) | The rows below | +270 |
| `transport_host/connection_host/https_auth_tests.rs` | Re-comment the two POST rows as the "never learned" cases | +10 / -10 |

Estimate: about 460 added, 85 removed. If the rows pass 300 lines, split: 3.0c-A (record, rows 1-4, 7), 3.0c-B
(`Rejected`, bound, helper failure: rows 5, 6, 8, 9).

**Rows (all fixture-server rows in `connection_host`, digests compared, no header printed):**

1. **PR5 placement, red on `68db19ae`.** One binding, anonymous connection 1 and authenticated connection 2; the
   private POST is leased connection 1; expect success, one POST carrying the binding's own credential (digest).
2. **PR6 single slot, red on `68db19ae`.** GET `/a`, GET `/b`, POST `/a` on one connection; expect the POST
   authenticated and successful.
3. **Cross-binding no-leak.** Binding A learns `/private`. Binding B, once with an equal-scope context and once
   with a different one, POSTs `/private` without having learned it: no `Authorization`, B's helper not started
   for that exchange, typed refusal, A's credential digest on no request of B. Red under the mutation that moves
   the record into adapter-shared state.
4. **Key precision.** `/public` after `/private` stays anonymous (the existing public-POST row stays green);
   `/repo.git` after `/repo` stays anonymous (PR7); push-class learning does not pre-authenticate an upload-pack
   POST of the same repository; an anonymously served advertisement records nothing.
5. **Learn only on authenticated success.** A helper credential rejected at the advertisement does not create a
   `Needed` key; the POST fails fast with zero helper spawns and zero requests.
6. **401 to a credentialed POST.** `Authentication`, `Discarded`, exactly one POST (digest present), the connection
   closed, the host `Open`; the next exchange for that key fails before a helper spawn; another repository on the
   same operation is unaffected.
7. **Ordering and concurrency.** 100 iterations: the client reads the advertisement's first byte, then starts the
   POST on another connection at once; every POST is authenticated. Two advertisements of one key in flight leave
   one entry. Red under the mutation that records after the body.
8. **Bound.** The 513th key is not recorded; its POST is refused typed; keys 1-512 still work.
9. **Helper failure at a recorded key.** The POST ends before a byte reaches the server (`Effect::None`).

The existing push row and the retention row (`https_auth_tests.rs:497`, `:438`) stay green unchanged.
A Windows receipt is required (the worker and `LogonSession` records compile on both platforms). Fast checks:
parity, cfg-boundary and globals scripts; braced control flow; no thread-local; the record is a provider field.

**Review tier: dual** (secrets handling plus a normative clause make it a per-step dual). Document object
(sections 4.1-4.6): Consistency and Safety. Code object: Code and State. Two reviewers per object, remediation
capped at two rounds.

## 6. Risks, open questions, decisions

**Decisions for the operator or the 1.8 phase review** (each with my recommendation):

1. **Windows logon-session POST placement.** The host model can refuse an NTLM/Negotiate push that the legacy route
   served by lease continuity. Moving the CLI to the host in 3.1 is then a Windows regression for those setups,
   against the goal that Windows is a full peer (plan L48). Choose: U3 (record; add a standing Windows
   "known refused" row) or schedule U1 (pool placement hint, gwz-transport) or U2 (bodiless re-authentication GET)
   before the Windows exit of Phase 6. Recommend U3 now, a design step for U1 before 6.2. I did not run Windows.
2. **(b1) or (b2)** (section 3b). Recommend (b1), measured trigger for (b2) in the 3.4 sweep; the operator sets the
   helper-spawn bound.
3. **Land before or after the 1.8 review.** Before: the 1.8 object includes the new model and the PR5/PR6 limit never
   reaches the reviewers as an open item. After: the 1.8 object stays as the closed 1.3c rows describe it. Recommend
   after 1.8, before 3.1.
4. **Service class in the key** (section 3a). Recommend yes.
5. **`approve`/`reject`** (section 3g) out of 3.0c. Recommend a separate step, decided after the sweep shows
   whether prompts or stale tokens are a real user problem.
6. **Pre-authenticated advertisements** (section 3c) out. Recommend revisit after the sweep.

**Risks and observations.**

- **Spawn cost under (b1) is an estimate, not a measurement:** at least two `git credential fill` per private
  member per service, on 8 helper slots and the spawn gate (`https_auth.rs:34,184`). Step 3.4 should record
  spawns per command and wall time.
- **Helper-token connections are still not shared across bindings** (`authorize_reuse` refuses them). With the need
  on the operation, a Basic connection carries no authentication state that a later binding could inherit, so the
  restriction could be lifted for equal scopes and R2 (a Python client keeping connections across calls) would then
  reuse private-repository connections. That is a separate Safety decision, not a prerequisite; I did not attempt
  to prove no server ties Basic to a connection.
- **Account selection by URL (observation, outside this design).** The host adapter builds the lookup URL from the
  key's host and port and the spec's path only (`exchange.rs:86-96`); no userinfo reaches `git credential fill`,
  whereas the legacy route looks up with the user's URL. Whether a `user@` in a member's URL must select a helper
  account on the host route is not checked here; report it if multi-account setups matter for 3.1.
- **The 512-key bound** is a chosen number with a typed, fail-closed overflow; recheck it against the largest
  workspace member count. `Rejected` can turn a transient 401 into a whole-operation failure for that repository.

**Not closed from the code and docs.** (1) Whether any supported helper (GCM, osxkeychain, `gh auth
git-credential`) prompts on every `fill` when `approve` is never called. (2) Whether `credential fill` strips the
path before helpers when `useHttpPath` is false, as 3a states. (3) The Git behaviours marked unverified in section 2.
