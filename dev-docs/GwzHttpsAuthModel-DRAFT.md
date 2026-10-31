# GWZ HTTPS authentication model: the credential and its need on the operation (design, DRAFT)

**DRAFT, revision 4, 2026-10-31.** Not accepted. Design only. Revision 4 resolves the r3 re-verdicts (Safety and Consistency,
both NO-GO, one blind-convergent P2) as `GwzHttpsAuthModel-RemPlan-2.md` dispositions them: every row is "fix in r4", all text
corrections. The operator decisions of 2026-10-31 are applied unchanged (section 6). Object: contract `GwzConnectionApiContract.md` sections 7, 9, 10, 11, 15;
gwz-core `a7b965c5`; gwz-transport `0a253639`. Code facts were re-read at those SHAs with `git show`; contract and plan
lines are at root HEAD, where those documents are unchanged since `0a0545a3`. A row is named `H1a-4`: sub-step, row number of section 5.

**Revision 3 changes** (RemPlan row -> where r3 resolves it):

| Finding (RemPlan row) | Resolved in |
|---|---|
| Safety P2-1 (scope-record writes) | 3(e) rules W1-W4; 3.1 Re-authentication; rows H3c-5, H3c-6; `prior` stays (H1b) |
| Safety P2-2 (learning rule) | 3.1 Learning; rows H3a-2, H3d-1 |
| Safety P2-3, Consistency P3-6, P3-10(a) (provider lifetime, `closed`) | 3(b) Lifetime and holders; H1e; A8; `closed` and clearing `Drop` removed |
| Safety P2-4 = Consistency P2-3 (bound overflow) | 3(b) bounds table; 3(d) G14, P8; H1d; A3, A8 |
| Consistency P2-1 (`Effect` of the re-authentication) | 3.1 Re-authentication; A7; rows H3b-3, H3c-3 |
| Consistency P2-2 = Safety P3-1 (eager, scoped rejection) | 3(b) Rejection; 3(d) G6, G7, G11, P2; rows H1a-4..6, H1c-2..6 |
| Safety P3-2 = Consistency P3-5 (`Need` transitions) | 3(d) table; rows H3b-3, H3b-4, H3c-1, H3c-2 |
| Safety P3-3 (key omits scheme and username) | 3(a); row H1a-1 |
| Safety P3-4 (charge is a maximum) | 3(b) Charge; row H1a-8 |
| Safety P3-5 (residuals) | A1; 3(b) Zeroization; retiring idle per-binding connections is not built (6) |
| Safety P3-6, Consistency P3-1(b) (re-authentication conditions) | 3.1 Conditions; A6; rows H3d-2, H3d-3 |
| Consistency P3-1(a), (c)-(g) (superseded text) | A5 wording; A11 (L881); A12 (plan); A8 (bounds); A2; A1 |
| Consistency P3-2 (anchors, citations) | 4.0 anchors and order; 3(a) key; citations corrected throughout |
| Consistency P3-3 (file lists, pin order) | 5 by-edit file lists, order and pin; 3.2 caller owner |
| Consistency P3-4 (hint is late) | 3.1 Placement; H2a, H2b; rows H2b-2, H3a-3 |
| Consistency P3-7 (scope claims) | Task boundary; 5 Review; 6 Unresolved U1 |
| Consistency P3-8 (fallback counter) | 3.1 Counter; row H3d-4 |
| Consistency P3-9 (H1b size) | 5 sizes from the real rows; H1 split in four |
| Consistency P3-10(b), (c) (served `cli`, gwz-py) | 3(f) |
| Consistency P3-11 (deadline) | 3.1 Deadline; row H3c-4 |
| Safety and Consistency §3 item 4 (caller identity) | 3.2 item 3 (R3 row) |
| Consistency §2 note (no hash in gwz-connection) | 3.1 Placement (path-string key) |
| Safety §5 / r2 account selector | A12 (plan prerequisite); 6 Risks |

**Revision 4 changes** (RemPlan-2 row -> where r4 resolves it; `R2-` Safety, `N-` Consistency):

| Finding (RemPlan-2 row) | Resolved in |
|---|---|
| R2-P2-1 = N-P2-1 (U7) | 3(b) `Need` (`evidence`); 3(d) G2, G3, G8, RA2, RA2a; 3.1 Learning, Unchallenged re-authentication; A4, A11; rows H3b-4 (re-scoped), H3b-5; U7 resolved |
| R2-P3-1 | 3(d) G3, G5, G8, RA7; row H3c-3 (3xx) |
| R2-P3-2 | 3.1 Placement; 5 Order (R1's re-arm); row H2a-6 (driver parked first) |
| R2-P3-3 | 5 Order (the R4 group is adopted whole) |
| R2-P3-4, N-P3-7 | A12 (3.1, Phase 2 served path); 3(b) Lifetime; row H1e-7 |
| Safety U2 addition, N-P3-4 | 3(b) Lifetime; A8; rows H1e-1, H1e-2, H1e-4; U2 resolved |
| Safety caller-identity condition | 3.2 item 3; 6 item 4 |
| Safety risk, N-P3-6 | 3(b) bounds (the budget code); A9; row H1d-1 |
| N-P3-1 | 5 table H1e (`admission.rs`, `binding.rs`) and H3a (`connection_host.rs` mod line); R3's `auth.rs` (U6) |
| N-P3-2 | 5 Order (pin rule; R1 and R1b named) |
| N-P3-3 | Task boundary; A13; U1, U2, U6 resolved; the (+) markers removed |
| N-P3-5 | 3(d) X1-X3; row H1d-5 |
| N-P3-8 | A6, A10 |
| N-P3-9 | A12 (3.4 sentence); U5 |

**Task boundary (Consistency P3-7, N-P3-3).** Deliverable: this document. Authority: the operator-recorded RemPlan row R4 as
the lane owner restated it at `5de7d71b` and amended at `0454aa1f` (`GwzPhase1-RemPlan.md`, as it now reads): the group H1a-H1e,
H2a-H2b and H3a-H3d with the file lists of section 5; gwz-core `https_adapter*`, `https_auth*`, `connection_host/https*` tests;
gwz-transport `crates/gwz-connection` (H1e, H2b) and `src/pool` (H2a); contract §4.1/§7/§7.2/§8/§9/§10/§11/§15 and plan text
(A1-A13); `connection_host.rs` mod lines for new test files. The R3 additions of 3.2 are R3's row (`https_adapter.rs`,
`https_adapter/{exchange,auth}.rs`, `https_auth.rs`). The operator's
decisions are the four of section 6, not a file list. Reference only: `https_worker/`, gwz-sspi, gwz-git. The P2-2 factory
fix is R3, a prerequisite, not in this step's size. Citations are `file:line` at the SHAs above.

Note: contract section 7.1 is the SSH reuse authorization (pool-key sentence L509-510); HTTPS scope is 7.2 (L534-570).

## 1. Problem

Git smart HTTP makes two requests per repository: a GET advertisement (`/<repo>/info/refs?service=...`) and a POST
(`/<repo>/git-upload-pack` or `git-receive-pack`) whose body is streamed. A 401 to the GET is answered by asking the
credential helper and sending the GET again; the GET has no body. A 401 to the POST cannot be answered: its body is on
the wire and contract section 9 forbids replay (L660-666, "Connection cannot replay"). Challenged POST refusal is a
standing part of the mechanism matrix (L561-563). So a POST that needs a credential must carry it from its first byte,
and the sender must know that before the first byte.

**What the host-owned HTTPS adapter does today.** It remembers the need on the CONNECTION. Each connection has a scope
record (`https_adapter.rs:123-132`): the binding kind (`Anonymous`, `HelperToken`, `LogonSession`) and one SHA-256 digest of
the path of the last helper-authenticated exchange. A POST is pre-authenticated, with a fresh helper lookup, only when
`work.prior == HelperToken` and the digest equals the POST's own path (`exchange.rs:219-231`). Every other POST goes out
anonymously; a 401 to it is `Fail(Authentication)` (`https_policy.rs:48`) and the exchange ends `Discarded`. This fails
closed and is pinned by `a_post_on_a_connection_nothing_authenticated_is_refused_after_one_anonymous_attempt`
(`connection_host/https_auth_tests.rs:613`).

**Where it fails.** The POST needs the pool to lease it the connection its advertisement authenticated, with no other
authenticated exchange having overwritten the one slot. The pool picks the lowest idle connection id of equal key and
identity (`gwz-transport/src/pool/allocation.rs:22-28`, a `BTreeMap` scan) and knows nothing of what each holds.

- **PR5, placement (`history/GwzP1cHttps-ReviewState.md:63`, `:96-102`).** One binding holds anonymous connection 1 and
  helper-authenticated connection 2 (a public and a private repository). The private POST is given connection 1, goes
  anonymously, is challenged and refused: `PermissionDenied`, `Discarded`. Same for a POST on a fresh connection (PR2).
- **PR6 / P3-2, single slot (`history/GwzP1cHttps-ReviewState-2.md:34`, `:85-98`; Code review 2 probe 2).** One binding,
  one connection: GET `/a` authenticated, GET `/b` authenticated (the slot becomes `/b`), POST `/a` on the same connection:
  anonymous, refused. Both reviewers filed it as P3-2; the lane closed it by documenting the limit (contract L881).

**Why it matters for Phase 3.** Step 3.1 routes every CLI command through one host (plan L721-728 records this as its
prerequisite). R1 is 16 members at one origin, `fetch` then `push` then post-push reads (plan L166), up to 32 connections
per scope. A workspace with private members meets PR5 and PR6 routinely, and the refusal reads "Authentication" although the
helper had a credential. The legacy route keeps the need on the route, per operation, until it ends (`https_policy.rs:70-72`,
`:119`, `basic_install` at `https_worker/native/exchange.rs:459-462`, POST pre-authentication `https_worker/prepare.rs:144-149`).

## 2. The two authentication models

| Mechanism | What carries the authentication | Per request or per connection | In gwz today |
|---|---|---|---|
| Basic with a helper-supplied credential (GitHub and GitLab tokens, `gh`, osxkeychain, GCM) | An `Authorization` header on each request | Per request | Supported: `auth.rs:69-80`; header built only as `Basic base64(user:password)` (`https_auth/secret.rs:97-111`) |
| Bearer or other token scheme | An `Authorization` header on each request | Per request | Not supported: a challenge offering nothing but it ends `Authentication` (`auth.rs:81`); `parse_secret` reads username and password only |
| NTLM | The TCP/TLS connection, after a 3-leg conversation | Per connection | Supported on Windows through gwz-sspi, `auth.rs:41-68` |
| Negotiate (Kerberos or NTLM, Windows picks) | The connection, as implemented | Per connection in gwz | Same path |
| Digest | n/a | n/a | Refused (`native.rs:142-146`, `Offers`: not an offer) |

**Can Negotiate or Kerberos be per request here? No, not as the code stands.**

- The adapter starts a native conversation only after a 401, never with an unsolicited first token (`auth.rs:41-68`;
  `Offer::initial` refuses a challenge that already carries a token, `native.rs:134-139`).
- The channel binding is that physical connection's own final-origin TLS binding, and the request is built per connection
  (`native/connection.rs:119-122`); one context, one conversation, at most eight rounds, no native context reuse
  (`GwzSspiDesign.md:66-67`).
- Negotiate may select Kerberos or NTLM and the selection is observed only after processing; there is no per-mechanism
  knob and Kerberos-only callers "must not start it" (`GwzSspiDesign.md:72-84`). So every Negotiate result is treated as
  connection-bound, which is what `Binding::LogonSession` does. Per-request Kerberos is a possible future, not a premise.

The helper-supplied (Basic) route is request-scoped in the protocol, yet the adapter models its connections as bound
(`Binding::HelperToken` is `per_exchange()`, `https_adapter.rs:119-121`; `authorize_reuse` refuses it to other bindings,
`:308-325`). Only the logon-session route is bound by the mechanism itself.

**Probe POST versus re-authentication GET (Consistency P3-1(b)).** Git replays a buffered POST and draws the 401 of a large
one with a small probe POST; gwz has neither (contract section 9), and a probe POST is a Git-service decision for gwz-git.
The re-authentication of 3.1 is a different thing: a bodiless, safe advertisement GET of the operation's own accepted key,
which the gwz-core adapter can build because it already owns the service-to-path table (`https_adapter.rs:9-11`,
`exchange.rs:98-127`). No POST is ever repeated or probed.

**How Git itself behaves** (from my knowledge of git's `http.c`, `credential.c`, `remote-curl.c`; **not verified in this
session**; confirm against the installed Git before this text is quoted): Git asks for a credential once per URL per
process at the first 401 and keeps it for the command; `credential.useHttpPath` (default false) decides whether the path
is in the description given to helpers; after a success it calls `credential approve`, after a 401 to a credentialed
request `credential reject` and clears it; a POST below `http.postBuffer` (1 MiB) is replayable, larger ones get a probe.

## 3. The proposed model

**Rule.** "This operation learned that repository R, for this service class, needs a credential" is operation state. It
is recorded in the operation's `AuthContext` provider (`https_auth::OperationAuth`, `https_auth.rs:48`) the moment an
authenticated advertisement is accepted. For a request-scoped credential (Basic from the helper) the record holds the
credential, and every later exchange of that operation to R and class, POSTs included, carries it from its first byte on
whichever connection the pool leased. For a connection-bound mechanism (logon session) the record holds the fact only,
and the exchange is placed on, or re-authenticates, a connection (3.1). The connection's scope record keeps the binding
kind for `authorize_reuse`. This is not a new authentication framework (A2): it keeps, for one operation and in memory,
what the existing helper route already obtained. `OperationAuth` is built once per capture by `capture_operation`
(`endpoint_environment.rs:207`) and reaches every exchange as `Work.auth` (`exchange.rs:54`, `pipe.rs:130`); the placement of
the helper environment in `AuthContext` is the contract's §15 1.5 row (L882).

**(a) Key.** The key is a domain-separated SHA-256 over: the pool `Key` of the spec (scheme, username, host, port,
`gwz-transport/src/pool/mod.rs:92-97`), the repository path, and the service class (fetch-class `upload-pack`, push-class
`receive-pack`, as the legacy `RouteKey` splits them, `https_policy.rs:55-58`). The repository path is the path of
`destination.request(service)` (`https_destination.rs:116-134`, which pops a trailing empty segment, so `/repo` and
`/repo/` are one key and `/repo.git` another: PR7 fails closed) with the service tail (`service_path`, `info/refs` or
`git-<service>`) removed, so a GET and a POST of one repository get one key. The username matters once the lookup carries
`user@` (A12): `alice@host` and `bob@host` then share nothing. The class matters because a repository readable anonymously
and writable only with a credential must keep its fetches anonymous (`history/GwzP1cHttps-ReviewCode.md` P2-1). A separate
**origin** digest over (scheme, host, port) scopes rejection and credential sharing. `credential.useHttpPath` does not change
the key; the lookup passes the full URL (`https_auth/lookup.rs:22,126`), so with it false two repositories of a host get the
same credential from the helper and each learns for itself, which costs a 401 per repository, never a wrong send.

**(b) What is held: the credential itself, in operation-scoped memory (decided, operator 2026-10-31: b2).** This is Git's
behaviour and the legacy route's (`https_worker/credentials.rs:5-16`). The earlier option, a fact with a fresh lookup per
request, stays withdrawn.

- **Where.** One record, `Needs`, a field of `OperationAuth`, behind a mutex, in `https_auth/needs.rs` (it sees `Secret`'s
  `pub(super)` fields). Keys map to `Need = Basic(Arc<Secret>) | ConnectionBound { observed, evidence } | Rejected | Unretained`;
  absent means Unrecorded. `evidence` is `Challenged` (the logon session answered a challenge, G8) or `OnBound` (served unchallenged on a
  `LogonSession` connection, G2), which proves nothing about the key's need (RA2a). Equal credentials at one origin share one `Arc`; never across origins. It is in no profile, pool
  key, instance, `Scopes`, connection or adapter-shared state.
- **Bounds** (all in contract section 10, A8; the numbers are mine, section 6):

| Quantity | Bound | At the bound |
|---|---|---|
| Entries in the key table (any state) | 512 | the new key's advertisement ends with the budget code before any body byte; nothing is stored |
| Distinct credentials | 64 | the advertisement ends budget-coded as above, and the key is stored `Unretained` |
| One credential (username + password) | 16 KiB, the helper output limit (`https_auth.rs:27`) | as above (a lookup cannot return more; H1a-3 tests the unit) |
| All retained credential bytes | 256 KiB | as above, key stored `Unretained` |
| Rejection fingerprints | 64 | the record becomes `Exhausted`: every later lookup result is refused with the budget code before a byte is sent; a poisoned lock does the same after wiping and clearing |

  The budget code is `ErrorCode::Capacity` (wire 5), never `Authentication`, in the contract's `InputBudget` class (§11 L726).
  `stream_error` (`https_adapter.rs:359`) gets an arm: `io::ErrorKind::QuotaExceeded` with the message "operation credential
  record budget exceeded", which names the record and no pool limit; under `cli` it is `StreamError::Failed { code: Capacity }`
  with no `capacity` detail (`CapacityCause` has no value for the record; a new one is a wire change this step does not make), and
  A9 states what that detail-less `Capacity` means. Overflow therefore never produces an anonymous POST: the advertisement fails,
  so no POST follows. For a stored `Unretained` key a POST ends before any byte (defence in depth). `Exhausted` is rows X1-X3 of (d).
- **Charge.** The declared `AuthContext` charge (`https_auth.rs:70-77`, fixed at `context()`) adds the formula
  `256 KiB + 64 x per-credential + 512 x per-key + 64 x per-fingerprint`, each term the `size_of` values plus allocation
  slack at its maximum; H1a-8 asserts it is at least the byte sum with every bound full (about 350 KiB; 72 admissions of
  a host, 8 active and 64 waiting, stay under the 64 MiB host cap, §10 L677, L686).
- **Lifetime and holders (Safety P2-3).** The provider, and so the record, lives as long as its last `AuthContext` clone. A
  credential-retaining provider is therefore built single-admission: its `AuthContext` carries a shared claim that `admit`
  takes, frees if the admission never registers (refused, timed out, permit dropped) and consumes for good once it registers;
  while one admission holds or has consumed the claim, `admit` of any clone is refused `InvalidInput` (H1e), so `register` stays
  infallible. "Operation" and "provider" are then
  one admission, and the sentence of §10 L692-694 on shared provider objects does not apply to it (A8). The `closed` flag and
  the clearing `Drop` of r2 are removed: a straggler that could insert holds a clone, so `Drop` could not have run. A clone
  the composition keeps past the operation's end keeps the record alive until it drops (6, U3); 3.1, 3.2 and the Phase 2 served path
  drop theirs with the operation, the last building a fresh capture per bind (A12). Every holder and its bound:

| Holder | Where | Ends | Bound |
|---|---|---|---|
| The binding | `BindingShared.auth` (`binding.rs:175`) | finish, cancel, abort, host close | the operation |
| Pending and authorizing openings | `instance.rs:77` (`Pending.auth`, `Authorizing`) | served, abandoned, cancelled, past deadline | allocation deadline (30 s) |
| In-flight connect | `ConnectCommand.auth` (`ports.rs:143`) | HTTPS drops it at once (`worker.rs` connect); SSH at setup end | setup aggregate (30 s) |
| Running exchange | `Work.auth` (`exchange.rs:54`) | task ends; cancel, `Exchange::abort` and `dispose_all` abort tasks (`worker.rs:440`) | the exchange |
| Retained native cleanup | `NativeRetained` probes | hold no `AuthContext` (nor `NativeCaller`) at `a7b965c5`; after R3 a record needing the caller clones `NativeCaller` (`Clone`, `native/sspi.rs:7`), never the context | cleanup bound (5 s) |

- **Placement `cli` and the core.** The provider is built by the client's owner from the call's capture (plan L66-74), in
  the client only. The carrier carries no `AuthContext`, header, credential or record; a served core never has the provider.
- **Zeroization.** `Secret` overwrites its allocations on drop (`secret.rs:118-122`, exact size, `:173-178`); `SecretHeader`
  likewise (`:7-31`). A header is built **under the `Needs` lock** and returned as a `SecretHeader` (marked sensitive,
  `exchange.rs:123`), so no `Arc<Secret>` leaves the lock; a fresh lookup's `Secret` belongs to its exchange until it is
  recorded or dropped. Not covered, as today (Safety P3-5, A1): the HTTP engine's copy of the header value (`exchange.rs:121`)
  and TLS and kernel buffers of connections the operation authenticated until those are disposed (an idle `HelperToken`
  connection lives up to 60 s, §10 L681, and no other binding can lease it), process dumps, swap. No memory locking.
- **Never logged, formatted or sent.** `OperationAuth` has no `Debug`; `Secret` prints `REDACTED`; `AuthContext` prints
  `AuthContext(..)` (`operation.rs`); `Needs` has a hand-written `Debug` of counts; `NeedKey` is digests; errors and
  observations carry codes and counts only (H1a-9 sweeps every format path).
- **Rejection (Consistency P2-2, Safety P3-1).** A 401 (not a 407) to a request that carried a credential is handled
  **eagerly under the lock**: every entry of that origin holding that credential becomes `Rejected` at once, the `Arc`
  drops, and the bytes are wiped when no in-flight request holds them (none does: headers are built under the lock). An
  unrelated credential, and the same bytes at another origin, are unaffected. The record keeps, per rejected credential, a
  fingerprint: HMAC-SHA-256 under a random per-operation key (32 bytes from `getrandom`, a dependency; `sha2` is too; no new
  crate; if randomness fails the record is `Exhausted` from the start), never the secret. A later lookup that returns bytes
  with a rejected fingerprint at that origin is not sent. `approve` and `reject` stay uncalled (g).
- **Threat model, plan section 1 (L55-62).** The core is the less trusted side; the client holds credentials and admits only
  an allowed (scheme, host, port, path) and class with a client-chosen identity. This adds client memory only. The key
  comes from a server challenge to an admitted exchange, so a core cannot make the client attach a credential to an
  unadmitted tuple or read the record; for an admitted path the operation never learned it gets an anonymous POST. What
  changes is time in client memory: the operation, as in Git, not microseconds. A same-process memory reader is outside the model.
- **R4 (plan L169).** The credential comes from a helper run with the operation's own environment (`work.auth`'s `Config`)
  and is stored only in that provider. Operation B has its own provider even with an equal `AuthScope` (the scope only
  partitions pool identities); a changed `GH_TOKEN` or `GIT_CONFIG_GLOBAL` is a new operation with an empty record. The
  single-admission claim makes this mechanical. `authorize_reuse` is unchanged.

**(c) Advertisement GET for a recorded key.** Still sent anonymously and challenged (pre-authenticating it stays out,
section 6); the challenge is answered from the record. No later GET or POST round spawns the helper.

**(d) One transition table for `Need`** (Safety P3-2, Consistency P3-5; G = advertisement GET, P = POST, RA = re-authentication).
Columns: event, prior state, new state, what is sent or where it ends. "Ends" means before any further byte of that exchange; a POST row that ends before a POST byte has
`Effect::None` and zero bytes at the server. A record is written only after an authenticated 200 with the right content type
(as the legacy `basic_install`), never at a first 401; a 403, 404, 5xx or 407 is no verdict on authentication (`classify`).

| # | Event | Prior | New | Sent / ends |
|---|---|---|---|---|
| G1 | GET, 200 unchallenged, connection scope not `LogonSession` | any | unchanged | one anonymous GET; an anonymously served advertisement records nothing |
| G2 | GET, 200 unchallenged, connection scope `LogonSession` | none | `ConnectionBound { OnBound }` | one GET (Safety P2-2) |
| G3 | same | `Basic`, `ConnectionBound`, `Rejected`, `Unretained` | unchanged | one GET |
| G4 | GET 401 offering Basic; fresh lookup answers; 200 | none | `Basic` | GET + answer |
| G5 | GET 401 offering Basic; answered from the record; 200 | `Basic` | `Basic` | GET + answer; no helper; when the logon session is also offered and the native route is available, G8 applies |
| G6 | the answer (fresh or from the record) gets 401 | none, `Basic` | `Rejected`, wiped, fingerprint kept | GET + answer; ends `Authentication`, connection closed |
| G7 | fresh lookup returns bytes whose fingerprint is rejected at this origin | none | `Rejected` | GET only; ends `Authentication` |
| G8 | GET 401 offering Negotiate/NTLM; the logon session answers; 200 | any but `Rejected` | `ConnectionBound { Challenged }` (latest accepted wins; a replaced `Basic` entry's reference drops, and the bytes are wiped when no key of the origin holds them) | GET + tokens |
| G9 | the logon session's answer is refused (`Authentication`) | any | `Rejected` | ends `Authentication` |
| G10 | the answer fails transiently (`Io`, `Timeout`, `Unavailable`, `Cancelled`) | any | unchanged | ends with that code |
| G11 | GET 401 | `Rejected` | `Rejected` | GET only; not answered, no helper; ends `Authentication` |
| G12 | challenge offers only Digest, Bearer or another scheme | any | unchanged | ends `Authentication` (`auth.rs:81`) |
| G13 | answered GET gives 403, 404, 5xx, another status or content type | any | unchanged | `classify` / `Protocol` |
| G14 | recording would pass a bound | none | `Unretained` (or nothing when the key table is full) | ends with the budget code before any body byte |
| G15 | the lookup fails (declined, timeout, spawn) | any | unchanged | its mapped code (`https_auth.rs:155`) |
| P1 | POST, 200 | `Basic` | `Basic` | credential from the first byte, built under the lock |
| P2 | POST, 401 | `Basic` | `Rejected`, wiped | ends `Authentication`, `Discarded`, connection closed, never replayed |
| P3 | POST | none | unchanged | anonymous; a 401 ends `Authentication`, never answered |
| P4 | POST, connection scope `LogonSession` | `ConnectionBound` | unchanged | no header |
| P5 | POST, any other scope | `ConnectionBound` | see RA1-RA7 | re-authentication first (3.1) |
| P6 | POST, 401 (after P4 or RA1) | `ConnectionBound` | `Rejected` | ends `Authentication`, `PossiblySent`; the next POST of the key sends nothing |
| P7 | POST | `Rejected` | `Rejected` | nothing: `Authentication`, `Effect::None` |
| P8 | POST | `Unretained` | `Unretained` | nothing: the budget code, `Effect::None` |
| RA1 | re-authentication: GET challenged, the logon session answers, 200 | `ConnectionBound` | same; connection scope raised to `LogonSession` | GET + tokens, then the POST |
| RA2 | re-authentication GET gets 200 unchallenged | `ConnectionBound { Challenged }` | `Rejected` | ends `Authentication`, `Effect::None`, no POST byte |
| RA2a | the same | `ConnectionBound { OnBound }` | unchanged | the GET is drained, then the POST goes anonymously on that connection, as P3: a 401 ends `Authentication`, `PossiblySent`, never answered or replayed; the next exchange of the key works |
| RA3 | challenge offers no logon session (Basic only, other) | `ConnectionBound` | `Rejected` | not answered by the helper or the record; `Authentication`, `Effect::None` |
| RA4 | the logon session is refused | `ConnectionBound` | `Rejected` | `Authentication`, `Effect::None` |
| RA5 | transient: reset, `Timeout` past D, gate `Unavailable`, `Cancelled` | `ConnectionBound` | unchanged | that code, `Effect::None`; no second attempt in this exchange |
| RA6 | the 200 body passes its cap | `ConnectionBound` | unchanged | `Protocol`, `Effect::None` |
| RA7 | re-authentication final status 403, 404, 5xx, another status (3xx included) or content type | `ConnectionBound` | unchanged | `classify` / `Protocol`, `Effect::None` |
| X1 | the 64th rejection fingerprint, a failed `getrandom` at creation, or a poisoned lock | any | `Exhausted` (record-wide; credentials wiped, entries cleared) | the exchange in hand ends with the budget code |
| X2 | G4, G5: GET 401 | `Exhausted` | unchanged | no answer from the record; a fresh lookup's result is refused: the budget code, before any answer byte |
| X3 | P1, P3, P4, P5: any POST (a POST consults the record) | `Exhausted` | unchanged | nothing: the budget code, `Effect::None`, no request, no re-authentication |

`Rejected` is per operation and terminal: deterministic refusals are not retried, because repeated failed logins can lock an
account (a cost: the key is lost for the rest of the operation). A kind or evidence change (G8) replaces; concurrent equal kinds keep
the first insert, and `Rejected` wins over a late insert.

**(e) Scope record write rules (Safety P2-1).** The record (`ConnectionScope`, `https_adapter.rs:123-132`) is written by
`Scopes::bound`, after the exchange task returns and before the connection can idle (`worker.rs:405`, then `:417`).
- **W1.** Every exchange that sends a credential (from the record or a fresh lookup) or completes a native conversation,
  including the re-authentication GET, reports that binding in `Ended`; the worker writes it before the connection idles.
- **W2.** A task that ends without reporting (panic, abort) leaves the connection `Broken` (`worker.rs:408-422`).
- **W3.** The record only rises: `Anonymous` < `HelperToken` < `LogonSession`. `LogonSession` is sticky; `HelperToken` is
  replaced only by `LogonSession`. (`Scopes::bound` is last-writer-wins today, `:162-179`.)
- **W4.** `prior` (the record read at exchange start, `worker.rs:259-261`) stays; H1b removes only the repository slot
  (`repository`, `RepositoryDigest`, `Work.authenticated`, `Ended.repository`).
R4 and partitioning are unchanged: `HttpsScoped` partitions the pool by `AuthScope` (`instance.rs:567`) and `authorize_reuse`
refuses `HelperToken` and `LogonSession` connections to every binding but the last lessee (`:308-325`); the record is no
scope input.

**(f) Placements.** Served `cli`: the core sends only a typed Open for an admitted tuple and cannot assert a need or widen
a key. The re-authentication GET is covered by its open's admission (Split §5.1-5.5): same (scheme, host, port, path),
same class (a receive-pack advertisement under a push-class request, within the push allowance), same native-auth grant; it
is not a new `open`. `Opened.facts` (Split L188) describes the open's own request: a POST answered from the record reports
`credential_offered`; a re-authenticated POST carries no credential itself; the re-authentication is counted (3.1), not
reported as the open's method. gwz-py captures per call (`native/src/route/transport.rs` `capture`, reference only), so a
record lives one call: a private key costs a helper run per call, and under b2 a credentialed request marks a formerly
anonymous connection `HelperToken` (as `exchange.rs:227` does today), so mixed public/private workloads lose cross-call
reuse. The 3.4 sweep expects both.

**(g) `git credential approve` and `reject`.** Still never called: a side effect on the user's store that a transient 401
could trigger (section 6). **(h) Concurrency.** Two exchanges learning one key each answer their own challenge; the first
insert wins, the other's `Secret` is wiped (a duplicate helper spawn is harmless). The record is written **before the first
response byte**: after the content-type check (`exchange.rs:264`), before the body loop (`:269`), while the scope record
comes only after `pipe.end(Ok)` (`:290`) and the task's return. A client that has read any advertisement byte finds the record.

### 3.1 Connection-bound bindings: placement hint and re-authentication (decided: fix before step 3.1)

NTLM and Negotiate authenticate the connection, so a POST is authenticated only by being on a connection the logon session
authenticated for its binding. The legacy route kept the exchange's lease from discovery to POST (`https_worker/serve.rs:66-71`);
the host leases per exchange and the pool picks the lowest idle id (`pool/allocation.rs:22-28`). The fix is a hint
(placement) and a fallback (correctness).

**Learning (Safety P2-2, U7).** An accepted advertisement for key K records `ConnectionBound { Challenged }` when the logon
session answered its challenge (G8), **or** `ConnectionBound { OnBound }` when the connection's scope at the exchange's start was
`LogonSession` and K was served unchallenged (`work.prior`, G2). A Basic answer records `Basic` even on such a connection. An
`OnBound` entry proves nothing about K's need: K may be public, and its POST on another connection then pays one
re-authentication GET and goes anonymously (RA2a). The record keeps the advertisement's byte length (the body loop counts it; written before the exchange's
end is visible, so a client that read the advertisement finds it).

**Unchallenged re-authentication (U7, resolved).** A re-authentication GET served without a challenge on a key learned `OnBound`
shows that K needs no authentication: its POST is sent as an unrecorded key's (P3), with no credential, to the key's own path,
after one bodiless GET that the conditions below already allow, so nothing is sent that an unrecorded key does not send today. On
a key learned `Challenged` the same answer is server misbehaviour and the key is `Rejected` (RA2). RA3 and RA4 are `Rejected` for both.

**Scope of the flag.** `ExchangeIo::connection_bound(&self) -> bool` (default `false`) is true when the connection's scope
**after** the exchange is `LogonSession`, which by W3 is `prior == LogonSession` or the logon session answered. The exchange
task sets a flag in its `Pipe` at the learning point (between `exchange.rs:264` and `:269`); `HttpsExchange` reads it. No
`Settle` or `ssh_adapter.rs` change.

**Placement (Consistency P3-4, §2 note).** The affinity is recorded **at the learning point**, not at settlement:
gwz-connection samples `connection_bound()` when the exchange's first response byte is delivered (and once more in
`Exchange::finish`, before it calls `Instance::settle`, which carries no binding and stays unchanged; this also covers a
response without a body) and stores `(pool Key, spec path) -> ConnectionId` in the binding's
state, at most 256 entries, oldest evicted, paths over 1 KiB not recorded; the pool `Key` is `PartialEq` only and the crate
has no hash dependency, so it is a bounded `VecDeque` scanned linearly. `Operation::open` sets `Request.prefer` from it
before `submit`. `Instance::submit` sets `Request.prefer_wait_ms` (the cleanup bound, 5 s, §10; the crate has no local
constant, `remote/backend.rs:66` is the carrier's) only when the instance's own `settling` list holds that connection: the
previous exchange has been finished and its lease is not yet released. The pool (`schedule`, `allocation.rs:17-53`) leases
the preferred connection first when it is idle, of equal key and identity, reusable, not excluded and the owner not full;
while it is leased and `prefer_wait_ms` has not lapsed the request waits (opens and evicts nothing; `next_deadline` includes
the lapse); otherwise it decides as if none were named. A connection that is busy with a reading exchange is **not** waited for.
The lapse is enforced only if the host driver wakes at `next_deadline`, which is R1's re-arm (`GwzPhase1-RemPlan.md`, Code P2-1): H2a depends on R1.
Connection ids are never reused, so a stale hint matches nothing. The hint never crosses bindings: the table is binding
state, and the lease still passes `authorize_reuse` (`instance.rs:601-630`). Cost: pool +75 and crate +125 product lines.

**Conditions of the re-authentication (Safety P3-6, Consistency P3-1(b)).** For a POST whose key is `ConnectionBound` and
whose leased connection's scope is `LogonSession`, the POST is sent as is (P4). Otherwise, **before its first byte**, the
adapter sends the key's own advertisement GET on that connection, only if all hold:
1. the request is an `info/refs?service=` GET, safe and bodiless, for a key whose advertisement this operation already had
   accepted (the record exists), with method, path, query and headers equal to the original's (built by the same
   `request()`; any future `Git-Protocol` header is carried);
2. at most once per POST exchange; the POST is handed to the engine only after the GET's 200 body is drained;
3. answered only by the operation's own logon session (`work.auth`'s caller, 3.2), never by the helper or from a `Basic`
   record; any other challenge ends the exchange (RA3);
4. the body is drained under `max(8 MiB, 1.25 x` the advertisement length recorded for K`)`, each frame within the stall
   bound, stalled or over-cap ends `Protocol` or `Timeout` (RA5, RA6);
5. it is counted (Counter below).
It is the only request of a different service than its exchange's own that the adapter sends; challenge answers and native
rounds are adapter-originated too, but they answer the exchange's own request. It is not a replay: the POST is sent once,
after authentication, and A6 names the clause the GET is an exception to.

**Effect (Consistency P2-1).** An exchange's `Effect` describes its own request (A7). The re-authentication does not move the
contract section 9 boundary; any failure in it is `Effect::None`, the stale-idle replacement does not apply to it, and the
POST is never handed to the engine before the GET is drained (rows H3b-3, H3c-3).

**Deadline (Consistency P3-11).** D is minted once for the re-authentication conversation as today's answer mints it
(`auth.rs:48-52`: `now + connect_timeout_ms`, refusing at 0 with `UnsupportedOperation`), each request within the stall bound.
An HTTPS exchange has no aggregate budget (§10 L682 is for setup); contract L704-711 governs the native conversation. Rounds
past D end `Timeout`, `Effect::None` (row H3c-4).

**Scope after re-authentication.** The re-authentication reports `LogonSession` in `Ended` (W1), so the connection is
recorded before it can idle; a connection another binding last used anonymously becomes `Unprovable` to that binding
(rows H3c-5, H3c-6).

**Counter (Consistency P3-8).** The adapter keeps a saturating `u64` count of re-authentications, read through
`HttpsAdapter::counters()`; row H3d-4: N re-authentications read N. How it reaches `TransportObservation` is a 3.1/3.4
composition item (the crate has no observation surface, §15 L882); the trigger for any wait on a busy connection is this count.

### 3.2 Dependency on the 1.8 Code review P2-2 (R3)

P2-2 (`GwzPhase1-ReviewCode.md:160-175`): `CoreFactory::create` ignores its `EndpointSpec` (`connection_factory.rs:60-89`)
and the native caller is one per instance (`HttpsInputs.native`, `https_adapter.rs:93`; item 3, `ReviewCode.md:116-120`).
This design depends on its correction (RemPlan R3), which is not counted in section 5:
1. Adapters are built from `spec.profile.inputs()` (`EndpointInputs`).
2. **R3 is the single owner of the caller move**: remove `HttpsInputs.native` and `Common.native` (`exchange.rs:38`), add
   `OperationAuth::native()`, switch `auth.rs:42` to the context's caller. R3's row now owns `https_adapter.rs`,
   `https_adapter/{exchange,auth}.rs` and `https_auth.rs` (A13, U6). H3a-H3d only consume `native()`. Without it the connection-bound path
   is the one place an operation could authenticate as another's caller.
3. R3 folds the captured caller's identity (SID, LUID or logon session, as the capture exposes it) into the `AuthScope`
   digest (`https_auth.rs:58-78`) once the caller is in `OperationAuth`, with a row (two captures differing only in the
   caller get different scopes). Only a non-secret identifier (a digest of the SID, LUID or logon session) enters the digest; when
   the capture cannot read the caller's identity, the scope takes a value unique to that capture, never a constant, so it merges
   no callers (Safety round 2; a row). Both reviewers agree it is not a prerequisite for safety today (the affinity table is per
   binding and the logon-session refusal in `authorize_reuse` is unconditional). `NativeRetained` and `SharedProcessSafety`
   stay per adapter (a host-level gate).

## 4. Proposed contract and plan text (the complete amendment list)

**4.0 Anchors and order.** Anchors are paragraphs, quoted by their opening or closing words; the line numbers in brackets are
root HEAD's, for locating only. Apply A1..A13 in this order (A3 before A4: both go after the same paragraph, A3 first).

| # | Where | Change |
|---|---|---|
| A1 | §7 table, row "HTTP credential adapter" (L494) | replaced by the text below |
| A2 | §7, paragraph ending "...a new auth framework is not introduced." (L485-487) | append: "The operation-held credential record of 7.2 is not an authentication framework: it adds no mechanism, store or policy; it keeps, for one operation and in memory, the credential the existing helper route obtained." |
| A3 | §7.2, new paragraph after the paragraph ending "...generic reuse is allowed." (L553-559) | text below ("Credential need") |
| A4 | §7.2, new paragraph after A3's | text below ("Connection-bound") |
| A5 | §7.2, paragraph "Preserve the current approved mechanism matrix" (L561-563) | "challenged POST refusal" becomes "challenged POST refusal (a challenge to a POST is never answered; a recorded key's POST carries its credential from its first byte)" |
| A6 | same paragraph, sentence "Redirects or challenge retries are requested by the existing protocol profile/policy with effect/deadline accounting; this crate never guesses Git discovery idempotence." (L563-565) | "...with effect/deadline accounting, except the gwz-core HTTPS adapter's re-authentication of a connection-bound key (above), made under the conditions stated there; this crate guesses Git discovery idempotence nowhere else, and a probe POST is not one of its tools." |
| A7 | §9, paragraph "Before an SSH exec is submitted..." (L660-666) | append: "An exchange's `Effect` describes its own request. The re-authentication advertisement GET that the adapter sends inside a POST exchange (7.2) does not move the boundary: the POST is handed to the HTTP engine only after that GET's body is drained, a failure in the re-authentication is `Effect::None`, and the stale-idle replacement of this paragraph does not apply to it." |
| A8 | §10 table (L675-687) and the sentence "Shared provider objects use one actual ownership charge..." (L692-694) | two rows below; append to the sentence: "A provider that retains credentials is single-admission: while one admission holds or has consumed the claim, `admit` of any clone of its `AuthContext` is refused `InvalidInput` (`register` stays infallible); an admission that never registers frees the claim." |
| A9 | §11, paragraph "Admission errors are..." (L717-731) | add: "An adapter's budget refusal is `ErrorCode::Capacity` in the `InputBudget` class: locally a stream error of kind `QuotaExceeded`, under `cli` `StreamError::Failed { code: Capacity }` with no detail. A `Capacity` without a `capacity` detail that ends an exchange of the HTTPS adapter means the operation's credential record is full; the pool's own limit refuses an opening and never ends an exchange." (`InvalidInput` is already listed, L717; the crate's `AdmissionError` lacks it at `0a253639`. gwz-core's `capacity.rs:181-183` reads a detail-less `Capacity` as the pool's limit at an opening.) |
| A10 | §4.1, paragraph "The EndpointFactory returns an opaque..." (L197); §8, paragraph "Fairness is FIFO among eligible requests" (L606-610) | §4.1: "An exchange handle reports, once its response has begun, whether its connection is authenticated by a mechanism that lives on the connection (`connection_bound`, default false)." §8: "A request may name one preferred connection and, when its binding's own exchange on it is finishing, a bounded wait (at most the cleanup bound): the pool leases it when it is idle, of equal key and identity, reusable, not excluded and its owner not full; while it is leased and the wait runs the request opens and evicts nothing; otherwise the pool decides as if none were named. The hint is binding-local and never replaces `authorize_reuse`." |
| A11 | §15 (after L887), and L881 | two rows below; L881's "POST rule" sentence and its "Open for Phase 3" sentence are marked "Superseded by the HTTPS model step (row below)" in place; the 1.3c text stays as history |
| A12 | Plan 3.1 Prerequisite (L724-728); plan 3.2 (L729-735); plan §10 (after D-U, before "Open: none.", L996-1000) | text below |
| A13 | `GwzPhase1-RemPlan.md` rows R3 and R4 (lane owner; done at `5de7d71b`, amended at `0454aa1f`) | R3 owns `https_adapter.rs`, `https_adapter/{exchange,auth}.rs`, `https_auth.rs` (caller move, scope digest); R4 is H1a..H3d with the files of section 5 and contract §4.1/§7/§7.2/§8/§9/§10/§11/§15 |

**A1 row.**
> | HTTP credential adapter | Per-request origin/challenge and captured context; opaque request-scoped authorization | The host, endpoint
> instances, pools and connections own or retain no credential and inherit no account from a TLS pool; an instance carries an
> operation's `AuthContext` only for that operation's openings and exchanges. The operation's own `AuthContext` provider may
> hold the credentials it obtained for that operation (7.2): bounded (section 10), never logged, formatted, serialized or sent
> over a carrier, wiped when its context is dropped or the credential is rejected. Residuals: the HTTP engine's and TLS
> buffers of connections the operation authenticated until those are disposed (an idle one lives at most 60 s and no other
> binding can lease it), process dumps and swap. |

**A3 paragraph.**
> **Credential need and the credential are operation state.** For a request-scoped credential (Basic from a configured
> helper), whether a repository needs one, and the credential obtained for it, are recorded in the operation's `AuthContext`
> provider, not on the connection, under a key of (scheme, username, host, port, normalized repository path, service class).
> When an exchange of the operation receives an authenticated success to a challenged advertisement, the provider records the
> key and the credential before any byte of that response is delivered. Every later exchange of the operation for that key,
> a POST included, sends that credential from its first byte on whichever connection the pool leased; a challenge to a
> recorded key's advertisement is answered from the record. A key not recorded is sent without a credential, and a challenge
> to such a POST is refused as `Authentication` and never replayed. A 401 to a request that carried a credential ends it
> `Authentication`, closes its connection, marks the key and every key of that origin holding the same credential rejected for
> the rest of the operation, wipes the credential at once, and the operation does not send equal bytes to that origin again.
> The record is bounded (section 10); a key or credential it cannot hold ends the advertisement before any body byte with the
> budget refusal, never as `Authentication`. It is shared with nothing outside the operation, appears in no log, formatted
> output, observation or carrier message, and its provider is admitted once. A connection a helper token authenticated is
> still leased only to its last binding.

**A4 paragraph.**
> NTLM and Negotiate are connection-bound as a whole, whichever mechanism Windows selects. The operation records such a key as
> connection-bound, with no credential, when an advertisement is accepted on a connection the logon session authenticated (it
> answered the challenge, or the connection already was its; the record notes which). The host prefers, for an exchange of that key, the connection of
> the binding's previous exchange for the same repository. If the connection leased is not authenticated for the binding, the
> adapter, before the first byte of a POST, re-authenticates it: it repeats the key's own advertisement GET (bodiless, safe, an
> advertisement this operation already had accepted, with the same method, path, query and headers), answers it only through
> the operation's own logon session, drains the 200 body under max(8 MiB, 1.25 x the length it observed), once per exchange and
> within the deadline D, then sends the POST. A re-authentication that does not authenticate the connection ends the exchange
> before any POST byte, except that a key learned only on a bound connection whose re-authentication GET is served unchallenged
> sends its POST as an unrecorded key's. A connection's scope tag only rises: once the logon session authenticated it, no later exchange
> lowers it. This is the one repetition the adapter makes on its own; it is not a retry of the POST.

**A8 rows.**
> | HTTPS operation credential record | At most 512 keys, 64 distinct credentials, 16 KiB per credential, 256 KiB of retained credential bytes and 64 rejection fingerprints; counted in the `AuthContext` charge | Provider |
> | Connection-bound placement | Affinity table of 256 entries per binding (paths up to 1 KiB); re-authentication body max(8 MiB, 1.25 x the observed advertisement length); wait for a finishing preferred connection up to the cleanup bound | Binding/adapter/pool |

**A11 rows.**
> | §7 table; §7.2 (scope, challenged POST refusal, connection-bound); §9 (effect); §10 (bounds, shared providers); §11 | Recorded at the HTTPS model step (sub-steps `<H1a..H3d>`, gwz-core `<commits>`, evidence `<run>`, verdicts `<..>`): credential and need on the operation per (origin, username, path, class); POST pre-authenticated on any connection; single-slot record and its "Open for Phase 3" remedy superseded (PR5, PR6 green); connection-bound keys re-authenticate by advertisement before a POST, and a key learned only on a bound connection that is served unchallenged sends its POST as an unrecorded key's; the scope record only rises. Retained: no replay, `authorize_reuse`, `HttpsScoped`. Not done: `approve`/`reject`, pre-authenticated advertisements, a wait for a busy (not finishing) preferred connection, retiring a binding's idle per-exchange connections at its end. |
> | §4.1 and §8; §11 | Recorded with the same step (gwz-transport `<commits>`, pin `<commit>`): `ExchangeIo::connection_bound`, `pool::Request.prefer` and `prefer_wait_ms`, binding affinity, `AdmissionError::InvalidInput` for single admission. |

**A12 plan text.** *3.1 Prerequisite* replaced by: "**Prerequisite (recorded 2026-10-31; settled by the HTTPS model step,
RemPlan R4, contract 7.2 and §15):** credential need and the credential are operation state per (origin, repository, service
class) and are sent on any connection; NTLM/Negotiate stay connection-bound with a placement hint and a re-authentication
before a POST. Two items remain for 3.1: the lookup must pass the remote's `user@` account selector to `git credential fill`
(today `https_adapter/exchange.rs:86-96` builds the lookup URL from host, port and path only), and the composition captures once per
admitted operation, on the original entry, admits that capture once (a credential-retaining `AuthContext` is single-admission) and drops it
with the operation; a composition row asserts the provider's strong count is 0 after the operation finishes." *3.2* gains: "gwz-py
captures per call and never caches a capture across calls." *The Phase 2 served path* (plan L72-74) gains: "the client owner of a
served core builds a fresh capture (a fresh `OperationAuth`) per bind." *3.4* gains: "the sweep reads the HTTPS adapter's
re-authentication count; route: the composition's factory keeps each built adapter's counter handle, as the test factory keeps
`scopes()` (`https_tests.rs:79`), with a 3.1 composition row that N re-authentications read N." *Plan §10* gains a decision entry (the lane owner numbers it, next
free letter D-V): the operator's four decisions of 2026-10-31 (6) and the bounds of A8.

## 5. The HTTPS model step: sub-steps (each about 500 lines or fewer, rows included)

Sizes come from the real rows: the existing `https_auth_tests.rs` rows run 31-61 lines (about 40 on average, more with a
second connection or origin); pool rows in `tests/pool_fresh.rs` run about 12. The step is about **3,725 lines added and 95
removed in eleven sub-steps** (about 1,090 product, 2,635 rows and fixtures; r3 said 3,620, which the R4 row still quotes), plus about 150 lines of root text (T). Revision 2
said 1,500 in four: it assumed 30-line rows, 25 rows and none of the r3 mechanisms. Each sub-step aims at 500 lines or fewer (R4, U1).

| Sub-step | Repo and files, by edit | Product | Rows | Added |
|---|---|---|---|---|
| H1a record | gwz-core `https_auth/needs.rs` (new: `NeedKey`, `Needs`, bounds, dedupe, HMAC, `Debug`), `https_auth/needs_tests.rs` (new), `https_auth.rs` (field, accessor, `context()` charge), `https_auth/secret.rs` (test wipe hook in a `cfg_if` block) | +245 | 10 unit, ~200 | ~445 |
| H1b Basic wiring | gwz-core `https_adapter/{exchange,auth,worker,pipe}.rs` (key; header under the lock; record before the body; `answer` returns the `Secret`; `Scopes::bound` rises only; slot, `Work.authenticated`, `Ended.repository` removed, `prior` stays in `worker.rs` and `pipe.rs`), `https_adapter.rs`; `connection_host/https_needs_tests.rs` (new), `connection_host.rs` (mod line), `https_auth_tests.rs` (`:558`, `:613`) | +190 / -95 | 4 rows + fixture + edits, ~255 | ~445 |
| H1c rejection | gwz-core `https_adapter/{exchange,auth}.rs` (401 paths, fingerprint check), `https_needs_tests.rs`, a second-origin fixture | +60 | 6 rows, ~295 | ~355 |
| H1d overflow | gwz-core `https_adapter/exchange.rs`, `https_adapter.rs` (`stream_error` arm), `https_auth.rs` (test-only small bounds in `cfg_if`), `https_needs_tests.rs` | +50 | 5 rows, ~205 (+15) | ~270 |
| H1e admission | gwz-transport `crates/gwz-connection/src/{operation,host,admission,binding}.rs` (the claim travels with the `ChargeGuard`: `admission.rs` carries it and frees it on a dropped admission, `binding.rs:188`), `host_tests.rs`; gwz-core `https_auth.rs` (one call), `connection_host/https_auth_tests.rs`, `https_sspi_tests.rs` | +65 | 7 rows, ~190 | ~255 |
| H2a pool | gwz-transport `src/pool/{mod,machine,allocation,clock}.rs`, `tests/pool_prefer.rs` (new) | +75 | 9 rows, ~140 | ~215 |
| H2b affinity | gwz-transport `crates/gwz-connection/src/{ports,binding,exchange,instance}.rs` (`instance.rs`: `submit` only, for the settling check; `Instance::settle` unchanged, the fallback sample is in `Exchange::finish`), `fakes.rs`, tests | +125 | 9 rows, ~240 | ~365 |
| H3a learning | gwz-core `https_adapter/{pipe,exchange}.rs` (flag, learning), `connection_host/https_bound_tests.rs` (new; NTLM-style handler ~70), `connection_host.rs` (mod line) | +60 | 5 rows, ~250 | ~380 |
| H3b re-authentication | gwz-core `https_adapter/{auth,exchange}.rs`, `https_bound_tests.rs` | +165 | 5 rows, ~245 | ~410 |
| H3c branches | gwz-core `https_adapter/{auth,exchange}.rs`, `https_bound_tests.rs` | +25 | 6 rows, ~290 | ~315 |
| H3d conditions | gwz-core `https_adapter.rs` (counter), `exchange.rs`, `https_bound_tests.rs`, `https_sspi_tests.rs` (Windows) | +30 | 5 rows, ~240 | ~270 |
| T text | root `GwzConnectionApiContract.md`, `GwzTransportReleaseConvergencePlan.md` (A1-A12) | text | none | ~150 |

If H1a or H1b passes 500, move rows 8-10 of H1a, or row 4 of H1b, into an added sub-step. A split of H3a/H3b rows is likewise
free. **Order.** Needs R3 (native caller and `OperationAuth` shape; it precedes R4), R2b (`pipe.rs` poll ports), R2a
(`Effect`, and `exchange.rs` which H2b shares), R1 (the driver re-arm: H2a's wait lapses only if the driver wakes at
`next_deadline`) and R1b (the pool step H2a follows). Lane A, gwz-core: H1a, H1b, H1c, H1d serially. Lane B, gwz-transport: H1e and
H2a, then H2b. **No production caller, release tag or 3.1 lane consumes gwz-core between H1b and the completion of H1c, H1d
and H1e; the R4 group is adopted whole.** Then a pin commit in gwz-core moves `.github/gwz-transport.commit` to the pushed
gwz-transport commit. **Pin rule:** each lane that moves the pin rebases on the latest pushed gwz-transport commit and moves the
pin and the allowlist `reconciled_commit` (`process_globals_allowlist_gwz_transport.json`) together; no order among R1, R2b, 2.3b
and this pin is fixed here. Then lane A again:
H1e's gwz-core part, H3a, H3b, H3c, H3d (H3a needs H1b and the pin). T lands when this document is accepted, so the lanes
cite controlling text; the §15 fields are filled by R6. At most three compiling lanes. **Code rules** (standing): braced
control flow; conditional compilation only in `cfg_if` blocks (test hooks, Windows rows), never on single items; no
thread-local or global; fast checks (parity, cfg-boundary, process-globals) run. Windows receipts: H1b, H1e, H3a-H3d.
**Review.** The RemPlan's structure: this document's own dual (Consistency and Safety, same two reviewers), and one round-2
Code and State review of the settled tuple; no per-sub-step review.

Rows (all fixture-server rows run the real adapter in `connection_host`; credentials are compared by digest, no header is
printed; "red" rows fail on `68db19ae`/`a7b965c5` first):

| Row | Asserts | Closes |
|---|---|---|
| **H1a** (unit) | 1 key precision: scheme, username, host, port, path, class each separate; `/repo`=`/repo/`, `/repo.git` apart. 2 equal bytes share one allocation per origin, not across origins. 3 each bound (512th/513th key, 64th/65th credential, 16 KiB, 256 KiB) ends budget-coded, earlier entries work. 4 reject K1 with K2..K16 unused: buffers read zero before anything else runs. 5 reject at A leaves B's equal credential usable. 6 rejected fingerprint refuses equal bytes at that origin only; the record holds no secret bytes. 7 65th fingerprint and a poisoned lock give `Exhausted`. 8 declared charge >= byte sum with every bound full. 9 no format path shows credential bytes. 10 kind flip, `Rejected` terminal, late insert after `Rejected` is a no-op | Safety P3-1, P3-3, P3-4, P2-4; Consistency P2-2, P2-3 |
| **H1b** 1 | PR5 (red): anonymous c1 and authenticated c2; the private POST lands on c1 with the credential from the record; the helper spawned once in all (the challenge and three POST rounds spawn it zero times after learning) | PR5 |
| H1b 2 | PR6 (red): GET `/a`, GET `/b`, POST `/a` on one connection is authenticated | PR6 |
| H1b 3 | cross-binding no-leak: A records `/private`; B, with an equal-scope and a different-scope context, POSTs it: no `Authorization`, B's helper not started, typed refusal, A's digest on none of B's requests; red under the mutation that moves the record into adapter-shared state | R4 |
| H1b 4 | 100 iterations of "read the first advertisement byte, start the POST on another connection" all authenticated; two advertisements of one key leave one entry; red when recorded after the body | 3(h) |
| H1b 5 | `:558` and `:613` become "never learned" rows and stay green | draft |
| **H1c** 1 | `/public` stays anonymous; `/repo.git` after `/repo` (PR7); push-class does not authenticate an upload-pack POST; an anonymously served advertisement records nothing | 3(a) |
| H1c 2 | fresh-lookup 401 (G6): `Rejected`; the retry GET sends nothing credentialed; the key's POST sends no request and spawns no helper | Safety P3-1(c) |
| H1c 3 | 401 to a credentialed POST (P2): `Authentication`, `Discarded`, one POST, connection closed, host `Open`; the next exchange for the key and its origin-mates fails before a byte; an unrelated credential works; buffers zero at once | Consistency P2-2 |
| H1c 4 | a 401 at origin A (a second fixture server) leaves B's equal credential usable | Safety P3-1(b) |
| H1c 5 | K2 with equal bytes after K1's rejection: no request carries them; ends `Authentication` | Safety P3-1(d) |
| H1c 6 | G11: a GET for a `Rejected` key is not answered and spawns no helper | table |
| **H1d** 1 | with small test bounds: the new key past the key table ends its advertisement budget-coded before a body byte; no POST for it reaches the server; the stream error is `QuotaExceeded` and its message names the credential record, with no pool-limit wording | Safety P2-4, N-P3-6 |
| H1d 2 | the credential past the count and byte bounds: same, key `Unretained` | Consistency P2-3 |
| H1d 3 | a 5 KiB credential's push succeeds (header about 6.8 KiB) | Safety P2-4 |
| H1d 4 | POST for an `Unretained` key: budget code, `Effect::None`, no request | P8 |
| H1d 5 | `Exhausted` (small fingerprint bound; also a poisoned lock): later lookup results are not sent (X2), and the POST of a previously `Basic` key sends no request (X3) | 3(b), N-P3-5 |
| **H1e** 1-4 | (crate) 1 a second admission of a cloned context, after the first registered or while it is still waiting, is refused `InvalidInput`, with no registration, instance or effect. 2 a refused (`QueueFull`) or dropped first admission frees the claim and the retry registers; a plain provider is still shared as documented. 3 with a connect in flight, the provider's strong count falls to the test's own handle within the connect bound after `finish`. 4 two concurrent admissions of clones, neither registered: the second `admit` is refused at once and `register` stays infallible | Safety P2-3, U2; N-P3-4 |
| H1e 5-7 | (gwz-core) 5 `OperationAuth::context` is single-admission through `ConnectionHost`. 6 Windows: after a native exchange ends with cleanup `Pending`, the provider is not held by the retained record. 7 two binds of one served invocation, each with a fresh capture, both register | Safety P2-3; R2-P3-4 |
| **H2a** (pool) | 1 idle preferred taken over a lower id. 2 stale id as absent. 3 other key or identity, excluded, not reusable: not taken. 4 leased and no wait: as absent. 5 leased with wait: the request opens and evicts nothing and gets it on release. 6 the wait lapses at its bound (`next_deadline` includes it) and the request decides as absent, with the driver parked first (the PROBE-ALLOC shape; red on `0a253639`). 7 never past the request's own deadline. 8 `prefer_wait_ms` without `prefer` is `InvalidRequest`. 9 FIFO: an earlier non-preferring waiter may take it | Consistency P3-4 |
| **H2b** (crate) | 1 affinity stored at the first delivered byte while the exchange is open. 2 GET finish then POST open with no wait for idle: the POST lands on the advertisement's connection, 100 of 100. 3 a reading (not finishing) exchange: no wait. 4 B never gets A's hint. 5 the 257th entry evicts the oldest. 6 a path over 1 KiB is not recorded. 7 a closed connection's hint is ignored. 8 an exchange not `connection_bound` records nothing. 9 the default is false (SSH, fakes) | Consistency P3-4, §2 note |
| **H3a** 1 | NTLM-style fixture (authenticates a connection after a token and remembers it; `challenging` is per request, `https_tests.rs:118`): a logon-session answer records `ConnectionBound { Challenged }`, sets the flag, scope `LogonSession` | 3.1 |
| H3a 2 | `/a` authenticates c1, `/b` is unchallenged on c1: `/b` is learned `ConnectionBound { OnBound }`, `connection_bound()` true | Safety P2-2 |
| H3a 3 | advertisement then POST of one binding with two idle connections, the authenticated one the higher id; the POST goes on it, no second GET, `accepted == 2`; also with no wait for idle, 100 of 100 | Consistency P3-4 |
| H3a 4 | cross-binding: B never receives A's bound connection; B's POST re-authenticates on its own | R4 |
| H3a 5 | a `Basic` key gets no hint and sends no re-authentication | table |
| **H3b** 1-2 | hint miss (preferred busy; preferred gone): the POST lands elsewhere, re-authenticates (GET, challenge, token, 200), then succeeds once | 3.1 |
| H3b 3 | refused (RA4): zero POST bytes at the server, `Authentication`, `Effect::None`, connection discarded, host `Open`; the next POST of the key sends nothing and no GET | Consistency P2-1 |
| H3b 4 | re-authentication GET gets 200 unchallenged for a `Challenged` key (RA2): same shape | Safety P3-2(a) |
| H3b 5 | U7 fixture: `/priv` (challenged) and `/pub` (anonymous) on one origin; `/priv` authenticates c1, `/pub` is learned `OnBound` on c1, its POST is forced onto c2: one unchallenged re-authentication GET, then an anonymous POST that succeeds; the entry is unchanged and not `Rejected`, and the next `/pub` exchange works; red under the mutation "`OnBound` treated as `Challenged`" | R2-P2-1, N-P2-1 |
| **H3c** 1 | challenge offers Basic only (RA3): not answered by helper or record; zero POST bytes | Safety P3-2(b) |
| H3c 2 | POST 401 after a successful re-authentication (P6): that POST ends `Authentication`; the key is `Rejected`; the next POST sends nothing and no GET (the first POST's bytes were sent; the zero-byte assertion is for the next) | Safety P3-2(c) |
| H3c 3 | reset during the re-authentication body, or a 3xx answer to it (RA7): `Effect::None`, no second attempt, zero POST bytes, entry unchanged | Consistency P2-1 |
| H3c 4 | fake-port rounds past D (`connect_timeout_ms` set small): `Timeout`, `Effect::None` | Consistency P3-11 |
| H3c 5 | re-authentication then POST on a connection B last used anonymously: scope becomes `LogonSession`, B's lease of it is `Unprovable`; red under the mutation "re-authentication reports `Anonymous`" | Safety P2-1 |
| H3c 6 | a `Basic`-recorded POST on a `LogonSession` connection leaves it `LogonSession` | Safety P2-1 |
| **H3d** 1 | `/a`, `/b`: `/a` authenticates c1, `/b` unchallenged on c1, POST `/b` forced onto c2 (c1 held), re-authenticates and succeeds once | Safety P2-2 |
| H3d 2 | the re-authentication's request line and headers equal the original advertisement's | Safety P3-6 |
| H3d 3 | an advertisement over 8 MiB: the POST still succeeds after a hint miss | Safety P3-6 |
| H3d 4 | N re-authentications read a count of N | Consistency P3-8 |
| H3d 5 | Windows (`https_sspi_tests.rs`): real SSPI against the acceptor fixture, push through the host, hint hit and connection gone | draft rows 1, 3 |

Existing rows `https_auth_tests.rs:497` (`a_push_is_authenticated_on_the_connection_its_advertisement_was`) and `:438` stay green.

## 6. Decisions, risks and open questions

**Decided (operator, 2026-10-31), applied unchanged in r4** (RemPlan "Operator decisions").

| # | Decision | Where applied |
|---|---|---|
| 1 | Held data is the credential itself in operation-scoped memory, wiped at the operation's end (b2) | 3(b), A1, A3 |
| 2 | Windows NTLM/Negotiate fixed before 3.1, with a pool placement hint, inside the same step | 3.1, A4, A10, H2, H3 |
| 3 | Folded into the 1.8 remediation as "the HTTPS model step"; the lane owner numbers it | A12, A13, 5 |
| 4 | The service class is part of the key | 3(a) |
| 5 | Apply 1.8 Code review P2-2: adapters from `spec.profile.inputs()`, native caller in the `AuthContext` | 3.2 |

**Not decided by the operator; my recommendation stands unless told otherwise.**
1. `approve`/`reject` (3g) and pre-authenticated advertisements (3c): out; separate steps after the sweep.
2. A wait for a preferred connection: built only for one that is **finishing** (bounded by the cleanup bound; r2 built no
   wait); a busy (reading) one is not waited for; its trigger is the re-authentication count.
3. My numbers: 512 keys, 64 credentials, 256 KiB total, 64 fingerprints, 256 affinity entries, 1 KiB recorded path, the
   8 MiB floor and 1.25 factor, the 5 s wait.
4. Caller identity in `AuthScope`: R3 does it (RemPlan). Safety advised against (it partitions identity-free anonymous
   connections); Consistency for (cheap, fail-closed); Safety withdrew its objection in round 2, on the condition of 3.2 item 3. Safety's refusal rule, `LogonSession` never crosses bindings, is
   unconditional and now sticky (W3).
5. Lifting `authorize_reuse`'s refusal of helper-token connections between equal-scope bindings (R2): not now. W3's sticky
   `LogonSession` is its first prerequisite; the second is evidence that no supported server ties Basic to a connection.

**Risks and observations.**
- **A server must accept a POST without a header on a connection a GET authenticated.** True of IIS-style NTLM/Negotiate as
  defined (hypothesis for others); a Kerberos server that authenticates per request (Apache `mod_auth_gssapi` by default,
  hypothesis) challenges every POST: this fails closed and matches today's route. H3d-5 and the Windows receipt are the evidence.
- **Re-authentication costs a full advertisement per fallback**; it is counted (H3d-4). The hint's efficacy depends on
  gwz-git finishing the GET before it opens the POST; gwz-git has no production caller of `ExchangeSpec::git` until 2.5
  (contract L884), so no row can pin its real order yet.
- **Token expiry.** `parse_secret` ignores helper attributes beyond `username` and `password`, including
  `password_expiry_utc`; a retained OAuth token can expire in a long operation, and its 401 rejects every key of that origin
  holding it until the operation ends, where per-POST lookups refreshed it.
- **Account selection by URL.** The lookup URL has no `user@` (`exchange.rs:86-96`; the key's username never reaches
  `git credential fill`); with b2 the wrong account's credential would be retained and spread to every POST of its key. It
  is a named prerequisite of 3.1 (A12), not a conditional report; the username in the key (3(a)) keeps accounts apart once
  it is fixed.
- **Not closed:** whether `credential fill` strips the path before helpers when `useHttpPath` is false (3a); the Git
  behaviours marked unverified in section 2; a transient 401 rejects every key of its origin sharing the credential.
- **Findings outside the step (recorded, not built):** helper failures are not memoized on the host route (the legacy route
  caches them, `credentials.rs:16`), so an interactive helper can raise up to 8 concurrent prompts in a 16-member operation;
  retiring a binding's idle `HelperToken` connections when it ends (needs a crate port); an unrecorded key's POST on a
  `LogonSession` connection carries the binding's Windows identity to the same server (inherent to connection
  authentication; contract text could say so beside A4); contract §7.2 L541-542 still describes `ExchangeSpec` without
  `GitService` (pre-existing, Consistency §5).

**Unresolved.** No correction conflicts with an operator decision. U1, U2, U6 and U7 are resolved; U3, U4 and U5 stay recorded.
- **U1. R4's size (resolved).** The eleven-sub-step group, each 500 lines or fewer, is the R4 row as restated at `5de7d71b` and
  amended at `0454aa1f` (`GwzPhase1-RemPlan.md`; contract sections and `connection_host.rs` mod lines). Authority: the operator's
  "fix per the reviewed draft" and the lane owner's amendment. The row's "about 3,620" is r3's figure (5).
- **U2. Single admission (resolved).** H1e (the `AuthContext` claim, `AdmissionError::InvalidInput`) is in the R4 row, with the
  same authority. Safety and Consistency both judge it the only mechanical guarantee, against option (B) and the `Weak` guard.
  Even with the claim, a clone kept past the operation keeps the record until it drops (U3).
- **U3. Clones and captures.** Single admission refuses a second admission of a cached capture; gwz-py captures per call
  today, so plan 3.2 must not add a capture cache, and 3.1 and 3.2 must drop the capture with the operation: the record is
  wiped when the last clone drops, not at the binding's end. A12 records this, with a composition row (strong count 0 after the operation).
- **U4. The H3c-2 wording.** The RemPlan asks "zero POST bytes" for a POST 401 after re-authentication; the first such POST has
  sent its body, so the assertion holds for the next POST of the key (P6, P7). The RemPlan's closure text is read that way.
- **U5. The counter's route to `TransportObservation`** (3.1 Counter) is 3.1/3.4 composition work, not this step's; A12 records the 3.4 route.
- **U6. R3's row (resolved).** R3 is the single owner of the caller move and owns `https_adapter/{exchange,auth}.rs` and
  `https_auth.rs` (`5de7d71b`, completed at `0454aa1f`). The caller-identity condition of 3.2 item 3 is for the lane owner to carry into that row.
- **U7. A public key learned on a bound connection (resolved).** Adopted: `ConnectionBound { observed, evidence }`, RA2a, row
  H3b-5 (3(d), 3.1). Authority: Safety R2-P2-1 and Consistency N-P2-1 (blind-convergent) and `GwzHttpsAuthModel-RemPlan-2.md`.
