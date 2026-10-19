# GWZ Credential: API boundary and independent-test design

Accepted acquisition amendment: [GwzCredentialAcquisitionAmendment.md](GwzCredentialAcquisitionAmendment.md)
at root 5dd59da1 controls the exact acquisition clauses listed in its §7.
Its one directly awaited acquisition replaces the old public selection chain;
prior acceptance below remains historical for its recorded tuple.

Date: 2026-10-19. Revision: **1 — accepted as a joint Credential/Connection
interface design** at root `c0fe3cf8185087a61780b8a61e347c95efc83c4f`, after
[Consistency-1](GwzCredentialApiDesign-ReviewConsistency-1.md) and
[Safety-1](GwzCredentialApiDesign-ReviewSafety-1.md) both reported GO. See the
[merged verdict](GwzCredentialApiDesign-Verdict.md). Acceptance covers design only;
repository creation, implementation and release require separate authorization.

The operator requested this API definition after identifying responsibility and
test coupling between connection management and authentication. This document
proposes a separately testable `gwz-credential` library in its own repository.
It does not create that repository or authorize implementation, migration,
dependency changes, publication or amendments to an accepted contract.

## 1. Outcome, scope and evidence

Credential selection and authentication must be testable without GWZ core, Git,
the Connection runtime, physical sockets or a complete platform release build.
Connection lifecycle and pool tests must likewise work with a fake credential
provider, without running native authentication.

Editable for this task: this root development document only. The reviewed
[Connection API contract](GwzConnectionApiContract.md), existing credential
adapters, `gwz-sspi`, Git bindings and other remediation plans are reference-only.
No accepted interface is silently changed by this proposal.

Observed examples at root `78b85d24` and its recorded member tuple:

- `gwz-core/src/git/endpoint/https_worker/native/sspi.rs`: NativeHost combines
  SSPI supervision, a spawn gate, caller capture and Git2Backend construction;
  NativeCaller also carries machine-proxy data.
- `gwz-core/src/git/endpoint/ssh_worker/open_request.rs`: an open request carries
  identity authority alongside Git service/path, pool permit, setup state and
  cancellation. Coexistence in one request is not itself a defect; ownership and
  test dependencies must show whether separation helps.
- `gwz-py/native/src/route/transport.rs`: caller/environment capture, identity
  resolution and per-operation transport construction are composed together.
- Connection contract §7 already separates provider ports from communication
  ownership. New API work must refine those ports, not invent a second host.

These source observations establish coupling candidates, not a quantitative
claim that authentication caused all churn. Windows containment, native launch
safety and stream ordering remain distinct concerns.

## 2. Responsibility boundary

| Responsibility | Owner |
| --- | --- |
| Workspace selection, Git configuration precedence and operation input capture | Core/driver composition and gwz-git as appropriate |
| Selected endpoint, physical connections, pool eligibility, lease and exchange lifetime | GWZ Connection and network adapters |
| Credential-source selection within explicit policy, key/agent/helper access, authentication conversation and proof | GWZ Credential and its providers |
| DNS, sockets, SSH handshake transport, HTTP requests, redirects, TLS and peer trust verification | Connection's network/trust adapters |
| Containment of blocking Windows authentication calls and native worker cleanup | gwz-sspi, called by a Credential provider |
| Native process launch safety | Qualified native launcher; separate residual anti-pattern task |
| Git service selection, repository paths, fetch/push success and replay semantics | gwz-git |
| Application results and request/response routing | Core/CLI/Python application APIs |

Credential consumes verified peer/channel facts and returns authentication
material or decisions to a trusted endpoint-local adapter. It owns neither a
connection pool nor a second network implementation. It must not construct Git
backends, capture proxy settings or select endpoint placement.

Server trust is distinct from possession of client credentials. This proposal
does not move all certificate/trust management into Credential or select another
trust repository. The network adapter verifies the server and supplies narrowly
scoped evidence before Credential permits a secret-bearing response.

## 3. Placement and dependency rules

Proposed package/repository name: `gwz-credential`. There is no dependency on
gwz-core, gwz-git, gwz-connection, gwz-transport or their application protocols.
Its public values do not contain types from those libraries. An optional Windows
provider may depend on gwz-sspi; portable tests do not build or execute it.
Use explicit platform modules/cfg_if boundaries under existing repository rules.

Composition supplies concrete providers through the existing Connection ports.
That adapter may depend on both APIs, but neither pure library imports the
other. This preserves Connection's accepted dependency rule and avoids a cycle.
Whether that small adapter ultimately lives with Credential or the application
is settled by the mapping exercise, not by introducing another contract crate.

Providers execute at the selected endpoint. Under remote driver placement:

- Keys, passphrases, agent access and helper tokens remain at that endpoint.
- SSH signing and authentication traffic happen there too. Core receives no
  general signing capability, private key or HTTPS authorization token.
- Paths are endpoint-local; core does not resolve or test a driver key path.
- No fallback to another endpoint or identity occurs when the selected source
  is unavailable. No automatic login workflow is introduced.

Only existing, separately negotiated transport messages cross the CLI/core
boundary. This library adds no serialized protocol, new wire carrier, credential
reference namespace or remote secret service. Current in-process deployment does
not permit wider secret access merely because both roles share an address space.

## 4. Selected API and operation/conversation ownership

The following Rust-shaped surface fixes ownership and outcomes for review; it is
not implemented code. ProviderSet and CredentialPorts are supplied at build.
Ports are a clock, progress service and mandatory bounded retainer. They contain
no Connection types. The composition root adapts its existing progress/retention
service rather than constructing a second executor, supervisor or cleanup domain.

```rust,ignore
pub fn build(config: CredentialConfig, providers: ProviderSet,
             ports: CredentialPorts)
    -> Result<(CredentialOwner, CredentialHandle), BuildError>;

impl CredentialHandle {
    pub fn prepare(&self, input: CredentialInput)
        -> Result<OperationCredentials, PrepareError>;
}
impl OperationCredentials {
    pub fn context(&self) -> CredentialContext;
    pub fn close(self) -> OperationClosing;
}
impl CredentialContext {
    pub fn select(&self, selection: ExchangeSelection)
        -> Result<PreparedCredential, SelectError>;
}
impl PreparedCredential {
    pub fn begin(self, peer: PeerContext, control: AuthControl)
        -> AuthOpening;
}
impl AuthOpening {
    pub async fn wait(self) -> Result<AuthSession, AuthFailure>;
}
impl AuthSession {
    pub async fn respond(&mut self, challenge: AuthChallenge)
        -> Result<AuthStep, AuthFailure>;
    pub fn use_ssh_key(&mut self, port: &mut dyn LocalSshKeyPort)
        -> Result<SshProgress, AuthFailure>;
    pub fn finish(self, result: PeerAuthResult) -> AuthFinishing;
    pub fn cancel(self) -> AuthClosing;
}
impl CredentialOwner {
    pub fn close(self) -> ProviderClosing;
}
```

OperationCredentials is Send, not Clone, and owns cancellation of that
operation's credential work. Its cloneable CredentialContext has selection
rights only; dropping a context does not close the operation. Context references
are bounded/charged views of one record, not fresh admission. CredentialHandle
is cloneable submission only. CredentialOwner is unique shutdown authority.
All other owned stages are non-Clone. There are no ambient/default owners.

One original-entry caller capture and immutable input create one
OperationCredentials. It may issue multiple independent one-use
PreparedCredentials through select, including different origins or per-open
identity choices allowed by the captured policy. A child cannot widen that
policy, recapture the caller or switch endpoints. Selection does not reserve a
physical connection. begin reserves one bounded conversation record and returns
AuthOpening; a rejected reservation is a terminal failed opening with no effect.
An operation closed between select and begin returns OperationClosed without
provider work. Closing one conversation leaves sibling conversations usable;
closing OperationCredentials cancels all its children, not another operation.

The composition's existing Connection AuthContext holds a charged
CredentialContext, not the OperationCredentials owner. The application operation
scope retains that owner and closes it when the operation settles. This is the
specific bridge owner: a small core endpoint-composition adapter implementing
Connection's existing authentication ports and depending on both libraries.
It constructs neither a Git backend nor a hidden provider/pool.

| Input/output | Selected meaning |
| --- | --- |
| CredentialInput | Endpoint scope, original caller capability, immutable source/policy views and inherited input charge |
| ExchangeSelection | Exact origin/SSH user and explicit or ambient source; constrained by CredentialInput |
| PeerContext | Exact protocol/target, verified server identity, physical generation and final TLS CBT when applicable |
| AuthControl | Absolute deadline, one-conversation cancellation and a reservation into existing composition accounting |
| AuthChallenge | Bounded typed HTTP offer/continuation, SSH authentication transcript or domain-separated possession proof |
| AuthStep | AwaitPeer, ProviderComplete, or a one-use endpoint-local response capability; never a peer-success assertion |
| PeerAuthResult | Accepted, Rejected or Unknown for the exact peer/generation; supplied by the trusted network adapter |
| AuthEnd | Terminal reason, offered/publication facts, peer result, physical disposition, cleanup receipt and optional accepted scope lease |

### 4.1 Endpoint-local output and explicit-key SSH handoff

No general key-export, arbitrary-signing or application-level secret accessor is
provided. There are two deliberately different SSH provider routes:

- **Agent:** the network adapter owns the handshaken SSH session and FFI callback.
  Credential validates the bounded transcript's user/key/method/session binding,
  selects the supported algorithm and signs through its local agent provider.
  The adapter keeps native allocation and socket-readiness handling. This factors
  the existing agent_auth.rs callback, not the entire connection-owning function.
- **Explicit key file:** retain the existing native memory-authentication route.
  Credential owns the bounded admitted key snapshot and its exact-byte identity.
  use_ssh_key calls the trusted LocalSshKeyPort with a scoped, non-Clone borrowed
  key view and immutable selected user/peer facts. The port owns the SSH session
  and invokes the existing Unix userauth_pubkey_memory or admitted Windows
  libssh2_userauth_publickey_frommemory call. It returns NeedNetwork, Accepted or
  Rejected; readiness waits remain outside Credential. A later native step uses
  the SAME admitted snapshot, never a path reopen or an agent fallback.

LocalSshKeyPort is a privileged native-provider port, not a capability handed to
Git/application callers or sent in messages. Its borrowed secret view has no
serialization/formatting/ownership-transfer operation and cannot outlive the
call. As with today's Entry::text(), trusted native code necessarily sees key
bytes: this is not protection against a malicious in-process adapter copying
memory. The boundary prevents accidental application/control-channel export.
The portable fake port checks selection, pin lifetime and native outcome mapping
without ssh2, sockets or private keys.

Before each local call, the session validates the physical peer/generation and
reserves an in-flight-use pin under its publication guard, then releases all
state locks before calling the port. Cancellation prevents new calls; a call
already admitted may finish and its pin remains charged. Retain the snapshot
until that native call and any provider cleanup are confirmed complete. If
native code retains a reference, its completion must stay pending and retain the
pin. Existing native rejection destroys the connection before releasing the key
pin. Preserve unsupported-key-form refusals before entering blocking native code.
Do not replace libssh2 or introduce a new key signer as an extraction prerequisite.

A response capability similarly binds session, peer/generation and purpose. Its
bounded synchronous commit into the trusted endpoint send queue checks the same
publication guard; it never runs network I/O under that guard. It contains no
public token getter. Cancellation before commit rejects it; cancellation after
commit cannot retract already queued/sent protocol material. The queue owns and
charges that material until sent or discarded, then wipes it. Drop before commit
wipes it and settles its reservation. A stale capability never authorizes a new
origin, generation or operation.

Private keys, helper tokens and actual protocol responses use bounded,
non-serializable secret storage and redacted diagnostics, with best-effort
wipe-on-release rather than a promise to erase OS memory. Responses necessarily
reach the selected Git server; they do not traverse core's application/control
channel under driver placement.

## 5. Terminal grammar, retention and composition

Every operation/child has one authority record. Selection, begin registration,
response publication and all terminal transitions serialize against its owning
provider/operation revocation. One terminal reason wins; later native completions
may update only cleanup evidence, never republish a revoked result. Native calls,
helper execution and foreign callbacks run outside authority locks.

| Owned stage | Close/cancel/drop behavior | Observable result / retained owner |
| --- | --- | --- |
| CredentialHandle / CredentialContext | Drop loses only that view | No provider/operation shutdown |
| OperationCredentials | close construction or Drop synchronously seals selection and cancels children | OperationClosing yields OperationEnd; existing retained records survive |
| PreparedCredential | Drop releases selection pin; begin after owner revocation fails before effects | No native work; exactly-once selection charge release |
| AuthOpening, including successful-but-unconsumed output | Drop seals its child and cancels any registered/late session | AuthFailure or hidden terminal AuthEnd retained by the bridge; late session is disposed, not published |
| respond future | Drop cancels that conversation and revokes publication, even if native step already started | The AuthSession remains terminal; sibling conversations unaffected; in-flight work remains retained |
| AuthSession | cancel construction or Drop synchronously seals and cancels it | AuthClosing yields AuthEnd; native task/pin remains owned until verified disposal |
| finish(PeerAuthResult) | Construction records the peer result and seals further offers; starts native finish | AuthFinishing yields AuthEnd, distinct from cleanup completion |
| AuthFinishing / AuthClosing | Drop abandons observation only, never cancellation/retention progress | Bridge retains the terminal record and cleanup receipt without caller polling |
| CredentialOwner | close construction or Drop seals prepare/select/begin, cancels all operations and revokes provider scopes | ProviderClosing yields ProviderEnd; existing supervisor/retainer owns pending work |
| OperationClosing / ProviderClosing | Drop abandons observation only | Retainer keeps pending records/progress and accounting alive |

AuthOpening::wait consumes opening observation. Returning a live AuthSession is
an atomic transfer; dropping the completed opening before transfer disposes that
session. Respond success is publication of a capability, not server acceptance.
ProviderComplete means the native mechanism finished producing tokens, not that
the remote peer accepted them.

AuthEnd contains peer Accepted/Rejected/Unknown independently of
cleanup Confirmed/Pending. Its physical disposition is one of:

- **RequestSettled:** no native connection-bound scope; the network adapter must
  still settle HTTP framing/body and Connection's other lease requirements.
- **BoundScope:** an accepted, generation/caller/origin-bound ScopeLease. Only
  eligible where current policy permits retaining that scope; current native
  dispose-after-exchange behavior is preserved where required.
- **Retire:** rejection, unknown/uncertain native state, cancellation or provider
  revocation makes that generation ineligible. Close it; never return it idle.

Cancellation does not retroactively change a recorded peer acceptance to failure;
it changes publication and physical disposition. For request-scoped auth a
committed output can remain offered even if cancelled; no later replay is implied.
Provider-owner shutdown revokes every BoundScope. The bridge cancels affected
active exchanges and schedules physical retirement; no new checkout/output is
allowed. Already committed transport output cannot be unsent. Ordinary operation
close does not revoke another operation or a scope explicitly transferred to a
healthy physical connection. A provider epoch is not reused.

### 5.1 One accounting bridge, no replacement supervisor

Before native effects, the bridge reserves the existing Connection domain's
provider-retention allowance AND obtains the provider's own native slot where
required. These are different resources: a Connection physical permit is never
released because an SSPI worker slot was released. A retained key/output byte
allocation has one reservation object, referenced by the relevant host/provider
views, not duplicated byte accounting or cloned independent capacity.

Credential's required RetentionPort reserves and retains generic authentication
records; composition implements it over the SAME existing retention service
specified by Connection. Portable tests inject a stepped fake service. No effects
are permitted without a retainer reservation. Records retain the registered
native Start/Session/Finish, cleanup probe, key/output pins and generation, plus
an immutable terminal slot visible to the bridge. This is the provider projection
of an existing retained record, not a second orphan registry or cleanup domain.

The retained Progress service runs terminalization and disposal even if all
public futures, handles and CredentialOwner disappear. Native providers use the
existing gwz-sspi supervisor and probes; helper/agent providers use their existing
bounded work owner. They may not introduce a detached untracked job. Providers
unable to retain and progress their work are refused before effects. Connection's
retainer owns the bridge record; provider-specific pending work remains owned by
its qualified supervisor. Removing either public owner does not drop that record.

A terminal decision is immediately visible through a cloneable observation
receipt in the bridge. The bridge fences the exact physical generation before
releasing operation observation; it need not await cleanup to make Retire known.
Connection retains its physical reservation through closure, and its retained
native-cleanup allowance through native disposal. Confirmed probe completion
releases only its matching resource once. Duplicate/stale completions cannot free
another generation. No final caller polling is required to make progress.

Close reports return a bounded report within the existing five-second cleanup
report bound, with explicit Pending receipts if disposal is not confirmed. A
report timeout is not release. No second timer extends the exchange deadline;
the existing deadline and supported interaction/timeout-zero exceptions remain
inputs. Original caller identity is captured once before fan-out/detach, and
current ProcessSafety qualification remains mandatory for native effects.

### 5.2 Selected finite defaults and refusal

These are proposed design defaults, not new externally mandated prerequisites:

| Resource | Bound / authority |
| --- | --- |
| Live operation selections | 72 per Credential owner (the accepted 8 active + 64 waiting host default); configurable finite, validated before construction |
| Conversation records, including pending cleanup and retained scopes | 64 per owner; Capacity refusal before work allocation, no internal waiting queue |
| Secret/context bytes | At most 16 MiB per operation and 64 MiB per owner, including selections, outputs and retained cleanup; same charged object in Connection AuthContext |
| Challenge/output | Existing 64 KiB native token/header envelope limits; enforce protocol-specific tighter bounds before copies |
| HTTP native rounds | Existing eight-round bound; no new mechanism fallback |
| Workers/helper/agent native slots | Existing qualified provider limits and Connection domain allowances; extraction does not raise them |
| Close reporting | Existing five-second bound; pending resources remain charged |

Aggregate bridge admission enforces the minimum of provider and Connection
allowances before effects. Separate providers may not bypass a domain limit by
creating new owners. Charges for terminal observations are fixed-size and finite;
consumed records retire once native cleanup and transferred scope/output pins
are gone. There is no history of all completed sessions. No global ID source is
introduced; provider, operation and physical generations are explicitly issued.

## 6. Independent tests and change impact

| Test layer | Builds/runs | Required cases |
| --- | --- | --- |
| Portable Credential contract | This crate and fake sources/clock/peer; no core, Git, Connection, socket or helper | Selection precedence inputs, challenge transitions, peer rejection, offer versus success, fixed deadlines, cancellation, finite retention, secret redaction, owner teardown |
| Provider qualification | Only the changed provider and required platform facility | Agent/key removal, helper refusal and output limits, native caller/CBT binding, worker stall and cleanup |
| Small composition suite | Credential + Connection ports with fake physical adapter | Reuse with compatible/incompatible authority, identity change, cancel A while B continues, pending native cleanup versus physical capacity |
| Actual placement/client qualification | Installed CLI/Python and real SSH/HTTPS endpoint | Selected endpoint alone has credentials/network access, correct trust owner, no secret control messages, actionable refusal without local fallback |

A small internal parser/state-machine change should use portable tests and the
affected provider tests. Contract, scope, deadline, cleanup or platform-assumption
changes additionally require affected composition tests. Final release testing
still qualifies the settled application; cheap tests do not waive required gates.

Seeded schedule tests must print seed, limits, trace and source tuple. Vary
challenge/cancellation/cleanup ordering without confusing arbitrary lost messages
with the supported transport contract. Inspect both record count and retained
bytes; moving an unbounded cache to an explicit owner is not a bound.

## 7. Concrete consumer map and joint traces

### 7.1 Production bridge and portable code

The core endpoint-composition bridge alone translates between the two libraries.
The planned extraction is limited to these responsibilities, not whole files:

| Existing seam | Credential extraction | Stays outside Credential |
| --- | --- | --- |
| agent_auth.rs validation/sign callback; agent key/provider routines | Bounded transcript validation, algorithm policy, agent source access and signature verification | Native FFI buffer allocation, ssh2 session and socket readiness |
| ssh_key_snapshot.rs and ssh_key_auth.rs | Admitted key source/snapshot/pin and explicit LocalSshKeyPort use | Trusted native memory-auth call and connection-before-pin disposal |
| https_worker/native.rs | Bounded offer parsing, supported mechanism selection and token policy | HTTP framing, request method/idempotence and response handling |
| native/sspi.rs Port/Session; native/owners.rs | Original caller capability and provider Start/Session/Finish projection | Composition launch-safety capability and physical lease owner |
| https_auth/view/helpers.rs and helper work owners | Explicit helper policy/input/output admission and credential retrieval | Git config precedence captured upstream; no arbitrary helper discovery |
| Python route capture | Receives one original-entry capture through explicit input | GIL handling, application operation/result storage and endpoint route selection |

Machine proxy, network trust and backend construction stay out. The bridge is a
small explicit consumer of both APIs. Review of this map does not authorize
moving code or rewriting providers, and native internal modules are not accepted
merely by naming them in the table.

### 7.2 Explicit file-key SSH setup and reuse, no agent

1. At selected endpoint E, capture operation A with key path K resolved against
   E's explicit base. Admit exact bounded bytes X and refuse unsupported forms.
2. A selects an explicit-key conversation. Connection owns the handshaken socket
   and verifies E-local host trust before peer facts reach Credential.
3. LocalSshKeyPort uses X in the supported Unix/Windows memory-auth call, retaining
   the key pin over EAGAIN/re-entry. Connection waits for readiness, not Credential.
4. Native accepted plus actual peer authenticated yields a receipt for that user,
   key and physical generation. No agent fallback or key-path reopen occurs.
5. Another operation reads K anew: Y != X means incompatible reuse and fresh
   setup under existing policy. Equality alone does not replace required trust/
   possession checks. Ambient reused connections use the recorded agent key's
   domain-separated proof under the accepted policy; explicit routes preserve
   their exact-key and existing proof rules, without inventing agent dependence.
6. Cancel during a native call: revoke further calls/output; keep X pinned until
   call completion and destroy the rejected/uncertain connection before pin
   release. A late Accepted does not publish success into a cancelled operation.

Fake LocalSshKeyPort traces the same step/pin behavior without SSH or secrets.
Real platform calls remain later provider qualification, including the existing
Windows unsupported-form refusals.

### 7.3 Multiplicity, proof sharing and HTTPS

One OperationCredentials issues conversations H1 and H2 for two allowed origins.
Both retain the SAME original caller capture and operation accounting, but own
separate challenge, cancellation, deadline and output state. Cancel H1; H2 may
finish, and a later select is still legal. Closing the operation cancels both.
A separate operation B using the same provider is unaffected by A's cancellation.

For shared SSH proofs, Connection's existing per-binding proof coordinator owns
waiter deduplication. It starts one credential proof conversation per accepted
proof job, supplying its policy-derived deadline. Each waiter retains its own
existing deadline; one waiter leaving does not cancel the proof for others.
Binding cancellation cancels the job. Credential does not add another proof
cache or waiter scheduler, or change Connection's latest-waiting-request rule.

For HTTPS, the network adapter supplies offers, final origin, actual TLS CBT and
physical generation. Credential's portable policy selects current allowed
mechanisms: Windows logon Negotiate/NTLM ahead of helper Basic, no downgrade after
rejected native identity, and the existing Digest refusal. Basic material is
request-scoped; native auth uses the captured caller and a generation-bound
conversation through gwz-sspi. Connection/HTTP adapter owns sends, reads,
redirects, framing and challenged-POST refusal. ProviderComplete is not remote
acceptance. Every follow-up uses the SAME absolute deadline; redirect/new-origin
work requires newly scoped selection under existing policy, not old-token reuse.

The bridge maps AuthEnd into Connection's existing authorization receipt,
security-scope tag, failure and native-cleanup probe. A pending peer-accepted
native finish may report Accepted + Pending + BoundScope (only where allowed)
or the existing retire policy; it never reports a free physical slot merely
because the peer accepted a token.

### 7.4 Lifecycle counterexamples and test oracles

- Owner closes before begin: prepared tickets yield ProviderClosed, zero effects,
  selection charges released exactly once.
- A is opening/responding, B active; operation A closes: A's publication is
  fenced, B proceeds. Closing the provider intentionally revokes BOTH operations.
- Drop the unpolled ProviderClosing immediately: synchronous revocation already
  happened; late native completion writes cleanup only. Retainer progresses with
  all ordinary handles gone; physical Retire and pending reservations remain.
- Peer accepted, then drop AuthFinishing while native cleanup waits: terminal
  peer fact survives, no late offers, scope/physical disposition is explicit,
  and provider/physical permits release only on their corresponding proof.
- Fill all 64 records with pending cleanups: next begin refuses before native
  work, even after replacing a public owner; shared domain accounting still holds.
- Output commit races cancellation: cancellation first exposes nothing; commit
  first records offered and may reach the peer, but never becomes inferred peer
  acceptance or permits replay.

### 7.5 Change-impact comparison

| Representative change | With current core-owned implementation | With selected boundary |
| --- | --- | --- |
| HTTP offer parser or credential-selection policy | native.rs and its enclosing core test/build context | Credential portable parser/policy tests; affected provider checks; small bridge check if output meaning changes |
| Explicit-key snapshot/input validation | Core key snapshot and native-auth test context | Credential portable admission + fake key-port tests; affected native provider tests for format assumptions |
| Pool idle/eligibility transition without auth-policy change | Core host/endpoint composition plus kernel tests | Connection + gwz-transport tests with fake credential port; no real helpers, SSPI or Git build |
| Caller scope, cleanup or CBT contract change | Cross-layer qualification | Affected portable, provider AND joint composition tests; no claim that extraction makes it local |

This is source-based feasibility, not executed speed/build evidence. Existing
ports alone could also host these extracted policies; the separate repository
makes the selected dependency and standalone test boundary explicit. It is not
a requirement to replace native providers. Acceptance concerns this design;
production migration and measured small-change test isolation remain proof gates.

## 8. Connection composition and exact adoption consequences

The accepted Connection contract stays unchanged in this documentation package.
No algorithm, wire envelope, endpoint route or cleanup-domain amendment is
selected. On adoption, document these precise provider refinements in the same
reviewed migration package:

| Accepted Connection clause | Credential composition / preserved rule |
| --- | --- |
| §3 first-party dependencies and initially core-hosted native adapters | Unchanged: Connection does not depend on Credential. Its small core adapter may depend on both; no reverse dependency or new contract crate. Credential's Windows provider alone may use gwz-sspi. |
| §7 PreparedOperation AuthContext, bounded provider ownership | AuthContext references one charged CredentialContext; operation scope retains OperationCredentials. Per-open select derives independent one-use conversations; no recapture. |
| §7 SSH identity/trust port, key material stays in endpoint adapter | LocalSshKeyPort permits a scoped native borrow at that SAME endpoint using existing memory-auth; application/core remote roles gain no key-export/signing API. |
| §7.1 shared proofs and exact identity | Connection retains proof job/waiters. Credential supplies bounded proof work; explicit file-key route preserves exact admitted bytes and existing rules. |
| §7.2 request-bound versus connection-bound auth | Map AuthEnd/ScopeLease to existing receipt/security tag and qualified cleanup probe; retain current native disposal and no-downgrade policy. |
| §§4–5 retained domain; §7.3 ProcessSafety; §10 clocks | Same retention/progress authority and fixed deadline. Native worker resources and physical permits settle separately; extraction supplies no new qualification or timeout exception. |

The leaf boundary is architectural: Credential has no dependency on core, Git,
Connection or transport; an optional native provider can depend on gwz-sspi.
A separate repository is selected as the proposal, not created by this review.

Next handoff: re-review this complete joint composition and closure traces.
Only after design acceptance and separate authorization create the repository
and implement the smallest portable provider proof. No source, native tests,
new supervisors, dependency upgrades or source migration were performed here.
