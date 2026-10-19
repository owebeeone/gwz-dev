# GWZ Credential: API boundary and independent-test design

Date: 2026-10-19. Revision: **0 — DRAFT, unreviewed**.

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

## 4. Public API sketch

The following is Rust-shaped design vocabulary, not compiling signatures or an
accepted interface. Bounds, object safety and async execution details must be
settled by review and a small proof before implementation.

```rust,ignore
pub fn build(config: CredentialConfig, providers: ProviderSet)
    -> Result<(CredentialOwner, CredentialHandle), BuildError>;

impl CredentialHandle {
    pub fn prepare(&self, input: CredentialInput)
        -> Result<PreparedCredential, PrepareError>;
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
    pub fn finish(self, result: PeerAuthResult) -> AuthFinishing;
    pub fn cancel(self) -> AuthClosing;
}
impl CredentialOwner {
    pub fn close(self) -> ProviderClosing;
}
```

No `sign_arbitrary_bytes`, export-key, global lookup, ambient `Default` provider
or post-construction attach method is proposed. SSH authentication adapters may
need a signing callback: it is endpoint-local, restricted to the selected key
and active authenticated-handshake/proof context, and bounded by the session.
If existing SSH bindings require key material rather than a callback, that is
a concrete feasibility question. Do not invent a public export API to hide it.

| Value/capability | Required meaning |
| --- | --- |
| CredentialOwner / CredentialHandle | Explicit provider resource ownership versus submission; operations cannot shut down shared providers |
| CredentialInput | Selected endpoint scope, explicit source/policy, original caller capability, finite input limits; no live environment discovery |
| PreparedCredential | One operation's admitted selection; preparation never treats a path or account name as proof of possession |
| PeerContext | Protocol, exact target/origin, verified peer identity, physical connection generation and channel binding where needed |
| AuthControl | Existing absolute deadline, cancellation and resource reservation supplied by composition; no restarted timeout per challenge |
| AuthChallenge / AuthStep | Typed, bounded protocol input/output; request-bound output distinguished from connection-bound state |
| PeerAuthResult | Actual peer acceptance/rejection/unknown supplied by the network adapter; token production alone never means authenticated |
| AuthFailure | Sanitized reason, whether material was offered, scope/disposal requirements and pending native cleanup where applicable |

Credentials and output tokens use private, bounded, non-serializable secret
storage, redacted formatting and best-effort overwrite on release. Avoid
unnecessary copies; do not promise erasure of OS/provider memory. Actual SSH
signatures and HTTP authentication tokens necessarily travel to the Git server
over the selected authenticated transport. The prohibition is disclosure through
the application/control channel, logs or unrelated operations, not sending the
protocol's required response to that server.

## 5. Lifetimes and necessary coordination

1. The composition root owns providers and captures caller context at the
   original entry. Worker threads do not recapture ambient Windows identity.
2. Each operation obtains an admitted credential selection before authentication
   work starts. Provider objects and retained secret bytes have finite budgets.
3. Connection supplies the actual target and verified peer/generation. Credential
   rejects mismatched scope; a plain path/digest/string is not trusted evidence
   against arbitrary malicious adapters. Both sides are trusted implementations.
4. HTTP Basic/helper material is request-scoped. NTLM/Negotiate conversations
   and established security context remain bound to caller, origin and physical
   generation. A small opaque scope decision goes to Connection; Credential does
   not decide which idle physical connection to lease.
5. SSH credential checks and signed reuse proofs are distinct from fresh SSH
   login. Both obey explicit identity selection; verification and trust evidence
   have named owners. Existing possession/revalidation policy is preserved.
6. The absolute deadline never renews on a challenge, provider switch or worker
   round. Existing qualified interaction/deadline exceptions remain policy inputs;
   this draft does not change timeout-zero semantics.
7. Cancellation seals the session and prevents delivery of later material.
   Native work may remain pending; its cleanup reservation stays held until the
   provider confirms disposal. Dropping a future does not abandon that ownership.
8. Provider-specific native cleanup is retained through the existing supervisor;
   Connection retains physical-connection capacity separately. Compose receipts
   once without adding duplicate domain budgets, orphan ledgers or supervisors.
9. Connection-bound auth failure must tell the network adapter when its physical
   connection is unsafe to reuse. Credential failure does not itself replay Git
   work, close another operation or claim that a server forgot an earlier token.

The precise close/drop and resource-transfer rules are unresolved API questions,
not solved by this sketch. Review must trace cancellation before/after an offer,
an accepted session with pending cleanup and provider-owner shutdown. The public
shape may change when that trace shows a simpler way to preserve ownership.

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

## 7. Bounded assessment before accepting the API

Deliver one responsibility/consumer map and two traces: SSH setup plus reuse
proof, and HTTPS helper plus native challenge authentication. Include remote
endpoint placement and cancellation. Compare this proposed library with leaving
the implementation behind the existing Connection ports.

For representative small credential and pooling changes, list the necessary
source edits, compilation dependencies and tests under each option. This makes
reduced test coupling observable, rather than claiming that a new repository
automatically solves it.

Acceptance requires:

- Provider behavior can be tested without constructing a Connection host.
- Pool behavior can be tested without real credential providers.
- No credential provider owns a socket/pool, Git backend or workspace operation.
- Necessary connection-bound security and cleanup coordination are explicit.
- Real production callers are mapped to this API; publishing an unused package
  is not adoption. Replaced credential paths are retired with the migration.

Do not expand into a general identity service, arbitrary authentication matrix,
new secret store, endpoint permission system or dependency upgrade. Existing
approved providers and refusals remain unchanged; older gh-only requirements
must be reconciled with later approved helper/native policy, not silently revived.

## 8. Review and adoption boundary

This draft does not amend Connection's accepted §3 dependency rules or §7 port
contracts. The assessment must identify whether existing ports suffice, or name
the exact clauses needing a bounded amendment and review. It must not duplicate
host admission, cancellation, cleanup accounting or the virtual-stream protocol.

Next handoff: the bounded mapping/traces and API review, followed by explicit
authorization for repository creation and the smallest independently testable
implementation. No source experiments, agents, builds or review loop were run
merely by writing this document. Existing oversized-document debt remains
separate; this document uses the operator-requested new draft slot and stays
within the active file-count and per-document size limits.
