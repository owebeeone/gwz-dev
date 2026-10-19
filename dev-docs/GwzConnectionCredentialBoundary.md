# Connection–Credential boundary contract

Date: 2026-10-19. Revision: **0 — accepted boundary/interface design** at root
`0570a27ececdc62b83b2bd31065aaf20559914aa`, after independent
[Consistency](GwzConnectionCredentialBoundary-ReviewConsistency.md) and
[Safety](GwzConnectionCredentialBoundary-ReviewSafety.md) reviews both returned GO.
See the [merged verdict](GwzConnectionCredentialBoundary-Verdict.md).
Acceptance covers documentation only, not repository creation or implementation.

The operator requested this boundary as a document and review before Credential
implementation. This contract makes the seam between the accepted
[Connection API](GwzConnectionApiContract.md) and
[Credential API](GwzCredentialApiDesign.md) explicit. It creates no repository,
crate, source implementation or new authentication mechanism.

## 1. Scope and authority

Editable now: this root document and its review records only. Both accepted APIs,
all implementation repositories and dependencies are reference-only. A required
change to either accepted API must be named as a proposed amendment; this document
cannot silently reinterpret it. Python result-retention work is paused, and the
builder's closeout and source lanes are outside this task.

The outcome is two independently testable libraries with one small composition
bridge. It is not another shared host, supervisor, protocol or identity framework.
Current root baseline is `9e972de5`; reference member heads are unchanged from
the accepted joint Credential review at `c0fe3cf8`.

## 2. Dependency direction and placement

| Component | Owns | Must not depend on / absorb |
| --- | --- | --- |
| gwz-transport | Discrete-message sequencing, streams and pure pool mechanics | Physical I/O or credential providers |
| gwz-connection | Reusable physical-resource ownership through injected adapters, operation isolation, admission and lease settlement | Credential library, Git, core, native credential storage |
| gwz-credential | Captured credential policy, selected sources and independent authentication conversations | Connection, transport, Git, core, sockets or pools |
| gwz-sspi | Qualified Windows worker supervision/native authentication | Connection pool or workspace policy |
| Endpoint composition bridge | Translation of the two APIs and their accounting/observations at the selected endpoint | Git behavior, another domain, hidden provider/pool or credentials forwarded to core |

The initial bridge is a narrow core endpoint-composition adapter, as selected in
the accepted Credential design. It may import both libraries. Neither library
imports it. An optional Credential Windows provider may depend on gwz-sspi.
There is no new common contract crate and no Connection-to-Credential dependency.

At core-local placement the bridge executes locally. At driver placement the
endpoint-side bridge and both sets of endpoint resources execute at the driver.
Core holds the selected route and virtual stream, not a CredentialContext for the
driver's keys or an agent-signing proxy. The driver endpoint's operation handler
owns its OperationCredentials; core operation cancellation reaches it through
the existing request/stream lifecycle. An operation owner in a design means the
owner at the executing endpoint, not permission to serialize secret capabilities.

## 3. Construction: no fifth Connection host port

Connection HostPorts remain exactly Clock, OwnerSeed, Progress and EndpointFactory.
Authentication is not another global host port. EndpointFactory constructs the
qualified network endpoint adapter; that adapter invokes the local bridge through
the authentication-provider seams already required by Connection §7.

The executing composition root:

1. Supplies the existing cleanup domain/retention and progress service.
2. Builds the explicit CredentialOwner/Handle with selected providers and the
   Credential projection of that same service. No separate cleanup ledger appears.
3. Builds Connection with its domain and EndpointFactory. The factory is bound to
   the local bridge; it cannot select an ambient credential owner later.
4. Captures immutable operation inputs and original caller identity before
   fan-out or Python detach, using the executing endpoint's rules.
5. Creates OperationCredentials and retains its unique owner in the endpoint's
   operation guard. Only its charged selection-only CredentialContext goes into
   that operation's opaque Connection AuthContext.
6. Uses each operation's own close/cancel authorities at settlement. Dropping an
   exchange cannot close CredentialOwner, Connection HostOwner or the domain.

Construction failures release reservations or retain already-started cleanup as
specified by the accepted APIs. There is no default private provider/pool or
late attach step. In remote placement core does not capture the driver's Windows
caller or resolve driver paths on its filesystem.

## 4. Exact handoffs at the seam

This table specifies mappings, not additional public signatures. The Rust-shaped
methods and value names in the two accepted APIs remain the implementation inputs.

| Handoff | Producer → consumer | Required content and restriction |
| --- | --- | --- |
| Operation authority | Endpoint guard → bridge/Connection AuthContext | Bounded CredentialContext view; unique OperationCredentials owner stays in guard; no environment/OS handle in Connection's public values |
| Per-open selection | Endpoint adapter → CredentialContext.select | Exact origin/user/source choice within captured policy; independent one-use selection for each conversation |
| Peer facts | Network/trust adapter → PreparedCredential.begin | Actual verified peer, final origin, physical generation and TLS CBT where required; never inferred from an earlier connection |
| Time/control | Connection operation/exchange → AuthControl | Same absolute deadline and clock origin, one-conversation cancellation and existing domain reservation; translation cannot restart a duration |
| Credential response | Credential → trusted endpoint send/key adapter | One-use purpose/peer/generation-bound capability, no application/control-channel token or general signing accessor |
| Peer result | Network adapter → AuthSession.finish | Actual Accepted, Rejected or Unknown; ProviderComplete/token generation is not peer acceptance |
| Terminal outcome | AuthEnd → network/Connection settlement | Independent peer fact, offered fact, RequestSettled/BoundScope/Retire, and Confirmed/Pending cleanup receipt |
| Continued cleanup | Existing provider supervisor/probe → shared retained record | Matching-generation disposal only; no fresh identity capture, late output publication or new capacity grant |

The bridge checks the opaque AuthContext's issuer and operation scope, rather
than guessing from an ID string or using a global map. Its retained byte charge
is the provider's admitted charge, referenced by both views, not copied secrets
or independent duplicate byte reservations. Unsupported/mismatched capability
refuses before credentials or effects.

Network trust is decided by the network/trust adapter. Credential validates that
its response is appropriate for those verified facts and the selected identity.
Both adapters are trusted code: opaque types do not prevent malicious in-process
code from forging facts or copying memory. User diagnostics expose only qualified
nonsecret outcomes and endpoint attribution, not paths/tokens/context dumps.

## 5. Authentication routes and physical ownership

### SSH

Connection's adapter owns the handshaken SSH session, host-key verification,
socket readiness and physical disposal. Credential owns source admission and
bounded credential/proof work.

- Agent login uses Credential's restricted transcript/algorithm/signature logic;
  the native adapter owns FFI allocation and the SSH session. No arbitrary sign
  request is routed from core to a driver agent.
- Explicit file login uses the accepted LocalSshKeyPort: the key snapshot stays
  Credential-owned, and the trusted local native port borrows it for the existing
  memory-auth call. The pin survives EAGAIN and native work in flight. No path
  reopen, key export, agent fallback or credential library socket is introduced.
- Connection retains the per-binding shared-proof coordinator, waiter deadlines
  and existing exact-identity/trust eligibility policy. It invokes one Credential
  proof conversation for its job; Credential adds no pool or proof scheduler.

### HTTPS

The network adapter owns TLS/trust, origin changes, request/response framing,
sends/reads, challenged-POST refusal and Git profile replay decisions. Credential
owns supported offer/selection policy and provider conversations. Preserve the
current approved helper/native policy, no downgrade after native rejection,
Digest refusal and original-entry caller provenance.

Basic/helper output is request-scoped. NTLM/Negotiate retains caller/origin/
physical-generation scope. Connection may retain an accepted BoundScope only
where current policy permits it; otherwise it retires the connection as today.
Neither a TLS pool hit nor native Complete establishes an authenticated account.

## 6. Terminal, revocation and capacity rules

Credential serializes response publication with its provider/operation/session
revocation. A selected endpoint send queue receives material only through that
checked commit boundary. Cancel before commit exposes no response; commit first
may leave material queued/sent and records that fact. Cancellation cannot unsend
bytes or infer that a Git operation had no effects.

The bridge owns translation of the terminal record even when the waiting caller
disappears. Terminal observation is separate from final native cleanup. It fences
the matching physical generation on Retire before permitting a new checkout;
there is no caller-supplied return_healthy flag.

| Event | Credential action | Connection/bridge action |
| --- | --- | --- |
| One conversation cancelled | Seal that child, retain pending native work/pins | Retire its uncertain generation; other operations/conversations stay independent |
| OperationCredentials closes | Revoke its selections/children | Settle/cancel that operation's exchanges, preserve unrelated owners |
| CredentialOwner closes/drops | Revoke all its operations and transferred scopes synchronously | Cancel affected active exchanges and retire affected physical generations; other providers are not globally shut down |
| Peer accepted; native finish pending | Preserve Accepted + Pending; no new offers | Keep native retention allowance; pool/release only after all applicable scope, framing and disposal requirements hold |
| Last public handles/futures disappear | Existing supervisor/retainer still progresses cleanup | Retain the bridge record and physical reservation until their actual matching disposal proofs |

An auth worker slot, physical connection permit and retained-byte charge are
different resources. Release each exactly once on its own evidence, never because
another resource ended. Provider limits and the shared Connection domain both
apply; replacing an owner cannot bypass occupied domain capacity. Five-second
cleanup reporting may return Pending; it does not release native work.

Scope revocation and checkout must not be a check-then-use permission cache.
Each use of a retained scope requires a still-live Credential grant for that
exact operation/peer/generation before new credential publication. If revocation
wins, no new grant/output occurs and the candidate retires. If use wins first,
already admitted work may finish under cancellation and retained cleanup rules.
No foreign/native call executes under cross-library authority locks. The bridge
must preserve this ordering without a new application-wide serialization gate.

## 7. Independent proof and actual-consumer gates

| Proof | Dependencies and oracle |
| --- | --- |
| Credential-only | Its fake sources, clock, peer facts, LocalSshKeyPort and retainer; no Connection/core/Git/socket/worker; prove selection, multiplicity, publication, limits and retained cleanup |
| Connection-only | Transport kernel and fake endpoint/auth adapter; no real Credential/native provider; prove eligible reuse, isolation, capacity and physical settlement |
| Small composition | Both library surfaces plus a test bridge and fake network/provider; prove actual type/value/receipt mappings in §§3–6, with no core/Git/native build |
| Production bridge | Focused tests of the real endpoint-composition adapter and supported caller; fake composition is not evidence that core adopted it |
| Provider/placement | Changed native provider and installed CLI/Python endpoint checks; credentials/network access confined to selected endpoint; no silent local fallback |

The small composition fixture's bridge is test code, not a replacement production
implementation or a third published library. The real core bridge may still
require a targeted core build. Parser/provider changes remain Credential-local;
pool-only changes use fake auth; scope/CBT/cleanup changes require affected joint
and provider checks. No reduction in actual test cost has yet been measured.

Mandatory joint traces:

1. One captured operation creates H1/H2 with independent identities/origins where
   allowed. Cancel H1, complete H2, then select H3; no recapture or global gate.
2. Two successive operations reuse an eligible fake connection with fresh
   Credential authorization. Change identity/origin/generation; reject incompatible
   reuse. Reuse alone does not authorize another account.
3. No-agent explicit-file login crosses the actual key-port seam, retains the
   snapshot over re-entry and cancellation, and applies replacement-key policy.
4. Revoke provider scope between candidate checkout and response commit; no new
   publication after revocation, and exact generation retirement/accounting.
5. Peer accepts, native finish remains pending; drop every public close future.
   Retainer still progresses, no slot frees early, and duplicate completion
   cannot release another generation.
6. Driver placement: core sees only existing virtual-stream controls/data and
   sanitized observations. Driver owns key/helper/trust access, original caller
   capture and native auth. Removing the driver capability refuses without core
   access. In-process fakes prove routing; real installed placement remains a
   separate gate and does not require the future byte carrier/iroh implementation.

## 8. Review and next implementation boundary

Review this seam with both accepted APIs as one object. No Connection host port,
dependency, wire schema or accepted deadline/security policy changes are selected.
Any mismatch must be reported with the precise clause and a bounded correction.
Do not re-review unrelated stream algorithms or take over the builder's closeout.

After this document passes review, the next proposed phase is Credential's
portable implementation using fakes. Starting it requires the agreed explicit
implementation scope; no source work, repository creation or native qualification
occurs merely because this document or its reviews exist.
