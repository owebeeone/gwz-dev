# GWZ HTTPS authentication model: the credential and its need on the operation (design, DRAFT)

**DRAFT, revision 2, 2026-10-31.** Not reviewed, not accepted. Design only (the HTTPS model step, in four
sub-steps, is sketched in section 5). Revision 2 applies the operator's decisions of 2026-10-31 (section 6) and the
1.8 Code review P2-2 (section 3.2). Object: contract `GwzConnectionApiContract.md` section 7; gwz-core `a7b965c5`
(`https_adapter*` at `68db19ae`); gwz-transport `0a25363`; root `dev-docs/GwzPhase1-ReviewCode.md` P2-2.

**Task boundary.** Deliverable: this document. Components the HTTPS model step may change (operator, 2026-10-31):
gwz-core `src/git/endpoint/https_adapter*`, `https_auth*`, `transport_host/connection_host/https*_tests.rs`;
gwz-transport `src/pool` and `crates/gwz-connection` (placement hint, section 3.1); the root contract and plan text.
Reference only: `https_worker/`, gwz-sspi, gwz-git. The P2-2 factory fix is a separate 1.8 remediation prerequisite,
not in this step's size. Citations are `file:line` in the trees above.

Note: contract section 7.1 is the SSH reuse authorization (pool-key sentence L509-510); HTTPS scope is 7.2 (L534-570).

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

**Where it fails.** The POST needs the pool to lease it the connection its advertisement authenticated, and no other
authenticated exchange to have overwritten the one slot. The pool picks the lowest idle connection id of equal key
and identity (`gwz-transport/src/pool/allocation.rs:22-24`, a `BTreeMap` scan), with no notion of what each holds.

- **PR5, placement (State review 1, `GwzP1cHttps-ReviewState.md:63`, `:96-102`).** One binding holds anonymous
  connection 1 and helper-authenticated connection 2 (two overlapping advertisements: a public and a private
  repository). The private POST is given connection 1, goes anonymously, is challenged and refused:
  `PermissionDenied`, `Discarded`. Same outcome for a POST on a fresh connection (PR2).
- **PR6 / P3-2, single slot (State review 2, `GwzP1cHttps-ReviewState-2.md:34`, `:85-98`; Code review 2 probe 2).**
  One binding, one connection: GET `/a` authenticated, GET `/b` authenticated (the slot becomes `/b`), POST `/a`
  on the same connection: anonymous, refused. Both reviewers filed it as P3-2; the lane closed it by documenting
  the limit (contract L881, "a single SHA-256 slot").

**Why it matters for Phase 3.** Step 3.1 routes every CLI command through one host (plan L721-728, which records this
as its prerequisite). R1 is 16 members at one origin, `fetch` then `push` then post-push reads (plan L166), with up
to 32 connections per scope and a separate lease per advertisement and POST. A workspace with private members will
meet PR5 and PR6 routinely, and the refusal reads "Authentication" although the helper had a credential. The legacy
route does not have the problem: it keeps the need on the route, per operation, until the operation ends
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
- `credential.useHttpPath` (default false) decides whether the path is in the credential description given to
  helpers: false gives one credential per host, true one per repository path. `credential.<url>.*` matching uses
  the full URL either way.
- After a request with a filled credential succeeds Git calls `credential approve`; after a 401 to one that carried
  it, `credential reject`, and it clears the credential so the next fill asks again.
- A POST is replayable in Git when its body is buffered (below `http.postBuffer`, default 1 MiB); larger bodies get
  a small probe POST first to draw the 401. gwz has neither: contract section 9 forbids replay, and a probe POST is
  a Git-service decision for gwz-git, not the generic adapter.

## 3. The proposed model

**Rule.** "This operation learned that repository R, for this service class, needs a credential" is operation
state. It is recorded on the operation's `AuthContext` provider (`https_auth::OperationAuth`, `https_auth.rs:48`)
the moment an authenticated advertisement is accepted. For a request-scoped credential (Basic from the helper) the
record holds the credential itself, and every later exchange of that operation to R and class, POSTs included,
carries it from its first byte on whichever connection the pool leased. For a connection-bound mechanism (logon
session) the record holds the fact only, and the exchange is placed on, or re-authenticates, a connection
(section 3.1). The scope record keeps the binding kind for `authorize_reuse`.

Where the state lives: `OperationAuth` is built once per operation by `capture_operation`
(`endpoint_environment.rs:207`), reaches every exchange as `Work.auth` (`exchange.rs:54`, `pipe.rs:130`), and dies
with the operation's binding. It is already where a helper environment lives (contract L483-485). The record needs
interior mutability, and the context's declared `charge` (`operation.rs:171`, contract L686) must cover its bounds.

**(a) Repository key.** Options: (1) SHA-256 of the exact `work.path` as today; (2) SHA-256 over the `Destination`
the request is built from (`exchange.rs:206`, host, port, path as `url::Url` normalizes it: dot segments removed,
userinfo and query absent, trailing empty segment popped), plus the service class; (3) fold `/repo` and
`/repo.git`, case, or trailing slashes. Option 3 guesses server behaviour and is rejected: PR7 shows an unfolded
spelling fails closed, the safe direction. Option 1 omits the origin and the class, both needed once the key
outlives one connection. **Decided: option 2** (operator, 2026-10-31, service class included): key =
domain-separated SHA-256 of (host, port, normalized path, class) where class is fetch-class (`upload-pack`) or push-class (`receive-pack`), as the legacy
`RouteKey` already splits (`https_policy.rs:55-58`). The class matters: a repository readable anonymously and
writable only with a credential must keep its fetches anonymous (the Code P2-1 finding,
`GwzP1cHttps-ReviewCode.md` P2-1: a request the server serves to anyone must not depend on a helper).
`credential.useHttpPath` does not change the key. The lookup passes the full URL to `git credential fill`
(`https_auth/lookup.rs:22,126`), so Git itself drops the path before helpers when `useHttpPath` is false. The
key is finer than Git's per-host credential in that case: each repository learns for itself, and two repositories
on one host then get the same credential from the helper. That costs a 401 per repository, never a wrong send.

**(b) What is held: the credential itself, in operation-scoped memory (decided, operator 2026-10-31: b2).** This is
Git's behaviour and the legacy route's (`https_worker/credentials.rs:5-16`, `Answers` per route, `Secret` wiped on
drop). The earlier option, a fact with a fresh helper lookup per request, is withdrawn. Specification:

- **Where it lives.** One record, `Needs`, a field of `OperationAuth` (the provider of the operation's
  `AuthContext`, `https_auth.rs:48`), behind a mutex, in a new `https_auth/needs.rs` (it must see `Secret`'s private
  fields). `NeedKey -> Need`, where `Need = Basic(Arc<Secret>) | ConnectionBound | Rejected`. Equal credentials
  (same username and password) share one `Arc`. It is in no profile, pool key, instance, `Scopes`, connection or
  adapter-shared state. Bounds: 512 keys, 64 distinct credentials, each at most 4 KiB (username plus password;
  the helper's output limit is 16 KiB, `https_auth.rs:27`, and a Basic header over 8 KiB is refused by common
  servers). Declared `charge` about 280 KiB (512 x 48 B + 64 x 4 KiB), counted at admission (contract L686).
  A credential over 4 KiB or a 65th distinct one is not retained: the key is not recorded, and its POST is
  refused as an unrecorded key (typed, fail closed). These numbers are mine; section 6 asks for confirmation.
- **Lifetime.** Created empty with the context on the original entry (`capture_operation`). Entries are added only
  by exchanges whose `Work.auth` is this provider. It ends when the last holder of the context drops: the binding's
  own reference at operation finish, cancel, abort or host close, and each exchange task's clone when the task
  ends. Cancel (`OperationCancel`), `Exchange::abort` and host close (`dispose_all`) abort the tasks
  (`worker.rs:440`), and retained cleanup records hold no `AuthContext` (contract L502-503), so the
  last drop is bounded. `OperationAuth` also gets a `Drop` that clears the map and a `closed` flag that makes a
  straggling exchange's late insert a no-op. No hook is added to the host.
- **Placement `cli` and the core.** The provider is built by the client's owner from the client's own capture
  (plan L66-74), in the client process only. The carrier carries no `AuthContext`, header, credential or record; a
  served core never has the provider. A persistent client still ends the record with the operation.
- **Zeroization.** `Secret` overwrites its username and password allocations on drop (`secret.rs:118-122`, through
  `session_host::environment::overwrite`, whole allocation, volatile writes). They are allocated at exact size
  (`secret.rs:173-178`). `SecretHeader` and `SecretBuffer` likewise (`secret.rs:7-31`). The header is built per
  request from the held `Secret`, dropped right after the request is built (`exchange.rs:239-240`) and marked
  sensitive (`exchange.rs:122`). The map's entries drop at end, on `Rejected` and on poison. Not covered, as today: Hyper's copy of the header value (`exchange.rs:121`), TLS and kernel buffers, swap; no
  memory locking is added.
- **Never logged, formatted or sent.** `OperationAuth` has no `Debug`; `Secret` prints `REDACTED`
  (`secret.rs:113`); `AuthContext` prints `AuthContext(..)` (`operation.rs:188-192`); `Needs` gets a hand-written
  `Debug` that prints counts; `NeedKey` is a digest; errors and observations carry typed codes and counts only. The
  core side (gwz-git, the carrier-backed handle) gets streams and typed errors, never the context.
- **What invalidates it.** (1) A 401 to a request that carried the credential: the key becomes `Rejected` and the
  credential is poisoned; every key sharing that `Arc` becomes `Rejected` at its next use without a request, and the
  secret is wiped when its last key drops it. (2) The operation's end. (3) Nothing else: a 403 or 404 is no verdict on
  authentication (`classify` unchanged), and the bounds never evict a live credential.
- **Threat model, plan section 1 (L55-62).** The core is the less trusted side; the client holds credentials and
  admits only an allowed (scheme, host, port, path) and service class, with a client-chosen identity. This adds client
  memory only. The key comes from a server challenge to an admitted exchange, so a core cannot make the client attach a
  retained credential to an unadmitted host, path or class, cannot read the record, and gets an anonymous POST for an
  admitted path the operation never learned. What changes is the time a credential sits in client memory: the
  operation, as in Git, instead of microseconds. A same-process memory reader is outside the model.
- **R4 (plan L169).** The credential comes from a helper run with the operation's own environment (`work.auth`'s
  `Config`) and is stored only in that operation's provider. Operation B has its own provider even with an equal
  `AuthScope` (the scope only partitions pool identities); a changed `GH_TOKEN` or `GIT_CONFIG_GLOBAL` is a new
  operation with an empty record. No path from A's provider to B's exists: `AuthContext` is cloned only for its
  binding's own connects and exchanges. `authorize_reuse` is unchanged, so a helper-token connection still serves
  only its binding.

**(c) Advertisement GET for a recorded key.** Still sent anonymously and challenged (pre-authenticating it stays out,
section 6); the challenge is answered from the record. No later GET or POST round spawns the helper.

**(d) Failure handling.** A 401 to a credentialed request (a GET answered from the record, or a POST): the exchange
ends `Authentication`, `Discarded`, never replayed, the connection closed (`exchange.rs:257`, `https_policy.rs:48`);
the key becomes `Rejected`, with the poisoning above. Later exchanges of the operation for it fail with
`Authentication` before any byte (`Effect::None`), as the legacy `rejected` flag does (`credentials.rs:55`).
`Rejected` is per operation. A key is recorded only after an authenticated 200 with the right content type (as the
legacy `basic_install`), never at the first 401. Lookup failures keep their mapped code (`https_auth.rs:155`).

**(e) R4 and scope partitioning.** `HttpsScoped` still partitions the pool by `AuthScope` (`instance.rs:567`) and
`authorize_reuse` still refuses `HelperToken` and `LogonSession` connections to any binding but the last lessee
(`https_adapter.rs:308-325`). The record is not a scope input. **(f) Served `cli`:** see (b); the core sends only a
typed Open for an admitted tuple and cannot assert a need or widen a key.

**(g) `git credential approve` and `reject`.** Still never called. With the credential in hand they are additive, but
they are a side effect on the user's store that a transient 401 could trigger; kept out of this step (section 6).

**(h) Concurrency.** Two exchanges learning the same key both answer their own challenge; the first insert wins, the
other's `Secret` serves its own request and is wiped (a duplicate helper spawn is harmless). `Rejected` wins over a
late insert. gwz-git sends a key's POST after reading the advertisement, possibly on another connection, but the scope
record is written after the exchange task returns (`worker.rs:395-406`), after `pipe.end(Ok)` (`exchange.rs:290`). So
the operation record is written **before the first response byte is delivered**: after the content-type check
(`exchange.rs:264`), before the body loop (`:269`). A client that has read any advertisement byte finds the record.

### 3.1 Connection-bound bindings: placement hint and re-authentication (decided: fix before step 3.1)

NTLM and Negotiate authenticate the connection, so a POST is authenticated only by being on a connection the logon
session authenticated for its binding. The legacy route got this by keeping the exchange's lease from discovery to POST
(`https_worker/serve.rs:66-71`); the host leases per exchange and the pool picks the lowest idle id
(`pool/allocation.rs:22-24`). The fix has a hint (placement) and a fallback (correctness).

**Learning.** When the logon session answers an advertisement and the response is accepted (`auth.rs:67`), the
operation records `ConnectionBound` for the key (no credential: the authentication lives on the connection).

**API (named and sized).**
- gwz-transport `pool::Request.prefer: Option<ConnectionId>` (`pool/mod.rs:214`; validated with the other fields
  `:266`). Schedule (`allocation.rs:22`): if the preferred entry is idle, of equal key and identity, reusable and not
  excluded, lease it; otherwise behave as if absent. It is a soft preference. Connection ids are never reused, so a
  stale hint matches nothing. About +35 product, +70 tests.
- gwz-connection `ExchangeIo::connection_bound(&self) -> bool`, default `false` (`ports.rs:206`): after a `Reusable`
  end, whether this exchange left its connection authenticated by a mechanism that lives on the connection. The
  HTTPS adapter returns true when its binding was `LogonSession`. About +10.
- gwz-connection binding affinity table: `(Key, SHA-256 of spec path) -> ConnectionId`, at most 256 entries, oldest
  evicted, held in the operation's binding state, never in the instance. `Instance::settle` records after a `Reusable`
  end with `connection_bound()`; `Operation::open` sets `Request.prefer` from it before `submit`
  (`instance.rs:349`). About +85 product, +130 tests. Neither port carries a secret or a Git concept beyond the
  path the spec already has.

**Why soft.** A hard preference would make a POST wait on a connection a long exchange holds, or fail if it is gone;
the fallback makes any connection correct, so the hint only saves its cost.

**Adapter check and fallback** (gwz-core, about +130). For a POST whose key is `ConnectionBound`: if the leased
connection's scope is `LogonSession` (only this binding can hold it, `https_adapter.rs:308-325`), send the POST as is.
Otherwise, **before the POST's first byte**, re-authenticate that
connection: send the key's own advertisement GET (same service class, same path) on it, answer the challenge through
the operation's native caller (section 3.2), read and discard the 200 body (stall-bounded, capped at 8 MiB, my
number), then send the POST on the now-authenticated connection. It runs inside the POST exchange's budget: the
native deadline D and rounds are minted once from the remaining budget, never reset (contract L704-711). It happens
at most once per exchange. If it fails, the exchange ends before any POST byte (`Effect::None`), the connection is
discarded, and the client reads the typed code. A POST to a key not recorded as `ConnectionBound` is sent as today.

**Why it is not a replay.** The POST is sent once, after authentication. The GET is a new request that repeats an
advertisement this operation already issued and had accepted; it is the one request the adapter sends that the
operation did not ask for, an exception to contract L565 that 4.4 states and Safety reviews.

**Rows (NTLM-style fixture: a new handler that authenticates a connection after a token and remembers it, as a real
NTLM server does; the existing `challenging` is per request, `https_tests.rs:118`).**
1. Hint hit: advertisement then POST of one binding with two idle connections, the authenticated one with the higher
   id; the POST goes on the advertisement's connection, no second GET, `accepted == 2` (red before: PR5 for logon
   session).
2. Hint miss, busy: the preferred connection is leased to another exchange; the POST lands elsewhere, re-authenticates
   (GET, challenge, token, 200), then the POST succeeds once.
3. Hint miss, gone: the advertisement's connection was closed; fresh connection; same outcome.
4. Re-authentication refused by the server: no POST byte reaches the server, typed `Authentication`, connection
   discarded, host `Open`.
5. Cross-binding: B never receives A's bound connection (hint or not); B's POST re-authenticates on its own connection.
6. Windows only (`https_sspi_tests.rs`): real SSPI against the acceptor fixture, push through the host, rows 1 and 3.

### 3.2 Dependency on the 1.8 Code review P2-2

P2-2 (`GwzPhase1-ReviewCode.md:160-175`): `CoreFactory::create` ignores its `EndpointSpec` (`connection_factory.rs:
60-89`), so every adapter takes its configuration from the runtime that built the host, and the native caller is one
per instance (`HttpsInputs.native`, `https_adapter.rs:93`, item 3 at `GwzPhase1-ReviewCode.md:116-120`). This design
depends on its correction, which is a prerequisite in the 1.8 remediation and not counted in section 5:
1. Adapters are built from `spec.profile.inputs()` (`EndpointInputs`).
2. `HttpsInputs.native` and `Common.native` go. The `NativeCaller`, captured on the original entry by
   `capture_operation`, lives in the operation's `OperationAuth` (accessor `native()`). The logon-session answer
   (`auth.rs:41-68`) and the re-authentication GET take the caller from `work.auth`. Without this the
   connection-bound path is the one place an operation could authenticate as another operation's caller.
3. Whether the caller's identity (SID, LUID, session) enters `AuthScope` is the P2-2 fix's call. `NativeRetained` and
   `SharedProcessSafety` stay per adapter (a host-level gate, not an identity).

## 4. Proposed contract text

**4.1 Section 7 table, HTTP credential adapter row (L494).** The clause now reads: "| HTTP credential adapter |
Per-request origin/challenge and captured context; opaque request-scoped authorization | No token cache in host or
account inherited from a TLS pool |". Replace with:

> | HTTP credential adapter | Per-request origin/challenge and captured context; request-scoped authorization | The
> host, endpoint instances, pools and connections keep no credential and inherit no account from a TLS pool. The
> operation's own `AuthContext` provider may hold the credentials it obtained for that operation (section 7.2):
> bounded, never logged, formatted, serialized or sent over a carrier, wiped when the operation ends or the
> credential is rejected. |

**4.2 Section 7.2, new paragraph after L557.**

> **Credential need and the credential are operation state.** For a request-scoped credential (Basic from a
> configured helper), whether a repository needs one, and the credential obtained for it, are recorded in the
> operation's `AuthContext` provider, not on the connection. When an exchange of an operation receives an authenticated
> success to a challenged advertisement, the provider records the key (origin, normalized repository path, service
> class) and the credential, before any byte of that response is delivered. Every later exchange of the operation for
> that key, a POST included, sends that credential from its first byte on whichever connection the pool leased; a
> challenge to a recorded key's advertisement is answered from the record. A key not recorded is sent without a
> credential, and a challenge to such a POST is refused as `Authentication` and never replayed. A 401 to a request
> that carried a credential ends it `Authentication`, closes its connection, marks the key and every key sharing that
> credential rejected for the rest of the operation, and wipes the credential; later exchanges for them fail before
> any byte. The record is bounded (512 keys, 64 credentials of at most 4 KiB, charged in the context); what does not
> fit is not recorded. It is shared with nothing outside the operation, is wiped when the operation's context is
> dropped, and appears in no log, formatted output, observation or carrier message. A connection a helper token
> authenticated is still leased only to its last binding.

**4.3 Section 7.2, L561-563.** "challenged POST refusal" becomes "challenged POST refusal for an unrecorded key".

**4.4 Section 7.2, after "NTLM/Negotiate must retain..." (L553-557).**

> NTLM and Negotiate are connection-bound as a whole, whichever mechanism Windows selects. The operation records such a
> key as connection-bound (no credential). The host prefers, for an exchange of that key, the connection the
> binding's previous exchange for the same repository authenticated. If the connection leased is not authenticated
> for the binding, the adapter, before the first byte of a POST, re-authenticates it by repeating the key's own
> advertisement request, through the operation's own native caller, within the exchange's budget and once. This is
> the only request the adapter sends that the operation did not make, and it is not a retry of the POST. If it
> fails, the POST ends before any byte is sent.

**4.5 Sections 4.1 and 8.** Add `ExchangeIo::connection_bound` (default `false`) to the ports, and to section 8: "A
request may name one preferred idle connection; the pool leases it when it is idle, of equal key and identity and not
excluded, and otherwise decides as if none were named."

**4.6 Section 15, new rows** (and the 1.3c row's POST-rule sentence is marked superseded):

| Existing authority | Replacement / retained rule |
|---|---|
| Section 7 table; section 7.2 (scope, challenged POST refusal, connection-bound) | Recorded at the HTTPS model step (sub-steps `<H1a..H3>`, gwz-core `<commits>`, evidence `<run>`, verdicts `<..>`): credential and need on the operation per (origin, path, class); POST pre-authenticated on any connection; single-slot record removed (PR5, PR6 green); connection-bound keys re-authenticate by advertisement before a POST. Retained: no replay, `authorize_reuse`, `HttpsScoped`. Not done: `approve`/`reject`, pre-authenticated advertisements, a wait on a busy preferred connection. |
| Sections 4.1 and 8 | Recorded with the same step (gwz-transport `<commit>`): `ExchangeIo::connection_bound`, `pool::Request.prefer`, binding affinity. |

**4.7 Plan.** Step 3.1's "Prerequisite" names "the HTTPS model step (1.8 remediation, after P2-2)"; the lane owner numbers it.

## 5. The HTTPS model step: sub-steps (each 500 lines or fewer, rows included)

Whole step: about 1,500 lines added, 95 removed (revision 1 estimated 460 for the model alone; credential retention,
the hint, the fallback and their rows are the growth), so four sub-steps, each under 500.

| Sub-step | Repository and files | Product | Rows | Added |
|---|---|---|---|---|
| H1a record | gwz-core `https_auth/needs.rs` (new, `NeedKey`, `Needs`, `Need`, bounds, dedupe, poison, wipe, `Debug`), `https_auth.rs` (field, accessor, charge) | +190 | +190 (bounds, dedupe, wipe probe, `Debug` and error-format sweep, closed flag) | ~380 |
| H1b wiring | gwz-core `https_adapter/exchange.rs` (key, record before body, POST and challenge from record, 401 to `Rejected`), `https_adapter.rs`, `worker.rs`, `pipe.rs` (slot, `prior`, `authenticated` removed: -95); `connection_host/https_needs_tests.rs` (new) | +90 | +260 | ~350 |
| H2 hint | gwz-transport `pool/{mod,allocation}.rs`; `gwz-connection` `ports.rs`, `binding.rs`/`instance.rs`/`operation.rs` | +130 | +200 | ~330 |
| H3 connection-bound | gwz-core `https_adapter/{auth,exchange}.rs` (learn, `connection_bound`, re-authentication, caller from context), `connection_host/` fixture and rows | +155 | +290 | ~445 |

Order: P2-2 first (1.8 remediation). H1a then H1b; H2 in a gwz-transport lane in parallel with H1; H3 after H1b and
P2-2, with H2 for rows 1 and 5 (its fallback rows need only H1b). The gwz-core pin of gwz-transport moves with H2.
If H3 passes 500, split rows 4-6 into H3b.

**Rows of H1b (all fixture-server rows in `connection_host`, credentials compared by digest, no header printed):**
1. PR5 placement, red on `68db19ae`: anonymous connection 1, authenticated connection 2, the private POST on 1;
   it succeeds, carrying the credential from the record, with no helper spawn.
2. PR6 single slot, red on `68db19ae`: GET `/a`, GET `/b`, POST `/a` on one connection; authenticated.
3. Cross-binding no-leak: A records `/private`; B, with an equal-scope and with a different-scope context, POSTs
   `/private`: no `Authorization`, B's helper not started, typed refusal, A's digest on none of B's requests. Red under
   the mutation that moves the record into adapter-shared state.
4. Key precision: `/public` stays anonymous (existing row); `/repo.git` after `/repo` too (PR7); push-class does not
   authenticate an upload-pack POST; an anonymously served advertisement records nothing.
5. A credential rejected at the advertisement records nothing; the POST fails fast with no helper spawn, no request.
6. 401 to a credentialed POST: `Authentication`, `Discarded`, one POST, connection closed, host `Open`; the next
   exchange for the key and for every key sharing the credential fails before a byte; an unrelated credential is
   unaffected; the secret buffers read zero afterwards.
7. Ordering and concurrency: 100 iterations of "read the first advertisement byte, start the POST on another
   connection" all authenticated; two advertisements of one key leave one entry. Red when recorded after the body.
8. Bounds: the 513th key and the 65th credential are not recorded and their POSTs refused typed; earlier ones work.
9. After learning, a GET challenge and three POST rounds spawn the helper zero times.

Existing rows `https_auth_tests.rs:497` and `:438` stay green; `:558` and `:613` become the "never learned" cases.
Windows receipts for H1b and H3. Fast checks: parity, cfg-boundary and globals scripts; no thread-local.

**Review.** Three objects, two reviewers each: contract text (4.1-4.7), Consistency and Safety; credential retention
(H1a, H1b) and placement (H2, H3), each Code and State. No Surface (no public API beyond the two crate ports).

## 6. Decisions, risks and open questions

**Decided (operator, 2026-10-31).**

| # | Decision | Where applied |
|---|---|---|
| 1 | Held data is the credential itself in operation-scoped memory, wiped at the operation's end (b2) | 3(b), 4.1, 4.2 |
| 2 | Windows NTLM/Negotiate fixed before 3.1, with a pool placement hint, inside the same step | 3.1, 4.4, 4.5, H2, H3 |
| 3 | Folded into the 1.8 remediation as "the HTTPS model step"; the lane owner numbers it | 4.7, 5 |
| 4 | The service class is part of the key | 3(a) |
| 5 | Apply 1.8 Code review P2-2: adapters from `spec.profile.inputs()`, native caller in the `AuthContext` | 3.2 |

**Not decided by the operator; my recommendation stands unless told otherwise.**
1. `approve`/`reject` (3g) and pre-authenticated advertisements (3c): out; separate steps after the sweep.
2. A bounded wait for a busy preferred connection: not built; trigger is the fallback count in the 3.4 sweep.
3. My numbers: 512 keys, 64 credentials, 4 KiB each, 256 affinity entries, 8 MiB re-authentication body cap.
4. Whether the caller's identity enters `AuthScope` (P2-2 fix owner; State to review).
5. Lifting `authorize_reuse`'s refusal of helper-token connections between equal-scope bindings (R2: private-repository
   connection reuse across Python calls). A Basic connection now carries nothing a later binding could inherit, but
   I did not prove that no server ties Basic to a connection. A separate Safety decision, not a prerequisite.

**Risks and observations.**
- **A server must accept a POST without a header on a connection a GET authenticated.** True of NTLM/Negotiate as
  defined and of IIS-style servers; no gwz fixture has run it on Windows. Row 6 and the receipt are the evidence.
- **Re-authentication costs a full advertisement per fallback** (the 8 MiB cap fails closed); the sweep should count
  fallbacks. It also widens "never guesses Git discovery idempotence" (contract L565) by one case: Safety must agree
  with 4.4.
- **Account selection by URL (observation).** The host adapter builds the lookup URL from the key's host and port and
  the spec's path (`exchange.rs:86-96`); no `user@` reaches `git credential fill`, unlike the legacy route. Whether
  the key's username is set for HTTPS is unchecked; report it before 3.1 if multi-account setups matter.
- **Not closed:** whether `credential fill` strips the path before helpers when `useHttpPath` is false (3a); the Git
  behaviours marked unverified in section 2. A transient 401 poisons every key sharing the credential until the
  operation ends.
