# GWZ Credential API design with accepted Connection — SAFETY-AXIS REVIEW

**Review object:** `dev-docs/GwzCredentialApiDesign.md`, revision 0, DRAFT dated 2026-10-19, composed with the accepted `dev-docs/GwzConnectionApiContract.md`, at root `158a940b961941ebe0bd5c47c771dd9658156a04`. API/design acceptance gate only.

**Baseline:**

| Repository | SHA |
| --- | --- |
| root | `158a940b961941ebe0bd5c47c771dd9658156a04` |
| gwz-core | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| gwz-py | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| gwz-cli | `5193db962e672c54cd20ab2ef8448d904129a559` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| gwz-transport | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| gwz-sspi | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

Documents and substantive source were inspected using pinned `git show`.

**Date:** 2026-10-19  
**Axis:** Safety — authentication state, selected endpoint, secret boundaries, cancellation, retained cleanup, physical settlement and scope. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block; zero P0, P1 or P3 findings. I pre-commit to GO on a revision resolving P2-1 and P2-2 as specified.

---

## 0. Evidence base

Inspected:

- The complete Credential draft, lines 1–241.
- Accepted Connection contract: acceptance/boundary, §§4–5 ownership and cleanup, §7 authentication and placement, §§8–10 physical settlement, effects and deadlines, and implementation/qualification gates.
- Connection’s accepted merged verdict and the retained evidence from its completed Safety review. Its accepted permit-revocation, retained-domain and atomic stream-application invariants were preserved.
- Connection remediation plan §§3–4.
- Core transport requirements P1–P9.
- Core `src/git/endpoint/https_worker/native/sspi.rs:1–224`: current caller capture, supervisor/gate composition, native conversation and cleanup-probe adapter.
- Core `src/git/endpoint/https_worker/native.rs:1–175`: bounded native tokens, wiping owners and approved challenge mechanisms.
- Core `src/git/endpoint/ssh_worker/open_request.rs:1–165`: current identity/setup ownership and sanitized outcomes.
- Core `src/git/endpoint/ssh_key_auth.rs:1–127`: explicit-key authentication from retained key bytes, cancellation checks, offered/authenticated distinctions and platform-specific native calls.
- Core `src/git/endpoint/agent_auth.rs:45–135`: existing endpoint-local agent-signing callback.
- Core `src/git/endpoint/ssh_key_snapshot.rs:30–60,195–211`: charged secret ownership and the current local key view.
- SSPI `src/lib.rs:1–31`, `docs/Supervision.md:1–220`, and relevant `docs/CallerValues.md` clauses: captured caller provenance, bounded conversations, immutable deadlines, publication revocation, retained cleanup and native Complete versus peer acceptance.

The applicable root instructions, process rules and review-loop skill were retained from the preceding review work.

All nine heads matched the required tuple at the start and end. Inspection was read-only. No files, builds, tests, network operations, mutations or further agents were created. No current-round peer report or excluded draft was read.

## Problem, requirement provenance, and scope

The operator requested fewer conflated connection/credential responsibilities and independently cheap tests, selected a separate Credential repository as reasonable, requested its API document, and requested review with the accepted Connection library as one composed boundary.

The source establishes concrete coupling candidates. NativeHost currently combines supervisor/gate composition with backend construction, and NativeCaller includes machine-proxy data. SSH authentication already combines a network-owned session with endpoint-local credential material and setup control. These observations justify defining narrower responsibilities; they do not establish that authentication caused every earlier churn incident.

The existing provider-port alternative remains viable. Connection already admits opaque bounded authentication context and narrow endpoint adapters. Current native code already projects pending SSPI cleanup through a small probe, and SSH has mechanism-specific local authentication seams. Those routes support a localized design without a new supervisor, network stack or dependency upgrade.

The separate repository is an operator-selected proposal, not a prerequisite demonstrated by this review. Creation, source migration and publication remain unauthorized. The editable boundary is the Credential design and joint review/remediation records. Connection and all source repositories remain reference-only.

The two blockers concern missing decisions needed to compose the proposed API with that accepted boundary. They can be corrected in the design. Where the chosen correction changes an accepted Connection obligation, the draft must identify the exact proposed amendment rather than silently treating it as adopted.

## 1. Findings

### [P2-1] Credential shutdown and terminal operations lack a joint ownership and settlement grammar

**Location:** Credential `98–121`, `131–140`, `150–182`; composed with Connection `167–203` and `225–272`.

**Violated invariant:** Every authentication terminal path must revoke publication, retain native ownership until confirmed cleanup, and give the physical adapter enough information to settle the correct lease exactly once.

Credential defines `AuthOpening`, `AuthFinishing`, `AuthClosing` and `ProviderClosing`, but does not define their terminal outputs, sealing points, drop behavior or transfer into Connection’s cleanup accounting. Lines 179–182 explicitly leave close/drop and resource-transfer rules unresolved.

A concrete joint trace is:

1. Operations A and B share a Credential provider owner and a Connection host, with separate physical leases and caller authority.
2. A has an AuthSession with native response work pending.
3. The composition calls `CredentialOwner::close()` and drops the returned ProviderClosing without polling it.
4. A’s AuthSession and Connection lease remain alive.
5. Native work completes later.

The contract does not determine whether step 3 synchronously seals admission and revokes A’s publication, whether A may still produce material, or how Connection receives a terminal/disposal decision. The general statement that cancellation seals a session does not define whether owner close/drop performs that cancellation. Nor does “dropping a future does not abandon ownership” define who continues progress and reports the outcome.

A second point on the same lifecycle boundary occurs when a peer-accepted session is consumed by `finish(PeerAuthResult)` and AuthFinishing is dropped while native cleanup remains pending. The API does not specify the returned cleanup proof, its retained owner, or how it composes with the physical connection’s reservation and connection-bound security scope.

An implementation could invent safe rules, but the proposed API does not yet prescribe them. This is consequential at a joint interface gate.

**Impact:** Different implementations can disagree about late material publication, physical retirement, pending capacity and final teardown. Connection may be left waiting without a defined terminal route, or receive an inadequate settlement result.

**Required correction:** Define one explicit lifecycle table covering preparation, begin, active response, peer result, finish/cancel and owner close/drop. Specify:

- Synchronous sealing and first-terminal/publication arbitration.
- Drop behavior for each future and session, including completed-but-unconsumed output.
- Terminal result and cleanup receipt ownership.
- How the existing provider supervisor and Connection domain retain distinct resources without duplicate budgets.
- How Connection learns the matching generation’s disposal requirement and settles its lease.
- Whether established connection-bound scope survives provider shutdown or requires physical retirement.

Retain the existing supervisors and accepted Connection rules. No replacement framework is required.

**Closure tests:** Specify a two-operation trace covering owner close/drop during begin and respond, unpolled/dropped terminal futures, late native completion, and peer acceptance with pending native cleanup. Assert no late publication after the selected terminal point, an explicit physical disposition, continued isolation of unrelated work, retained permits after all ordinary handles disappear, and exactly-once release after verified disposal.

**NEW ARCHITECTURAL ROOT: YES.** The proposed owner/session boundary lacks its revocation and resource-transfer protocol. This is a new Credential composition root, not a reopening of the accepted Connection findings.

### [P2-2] The explicit-key SSH route has no selected credential-to-authentication handoff

**Location:** Credential `47–62`, `124–129`, `163–165` and `203–220`; composed with Connection `488–500` and `504–510`.

**Violated invariant:** Existing explicit SSH identity must remain endpoint-local, fail closed, and have an implementable route through the selected boundary without exporting general signing authority or moving network ownership into Credential.

The draft proposes a possible restricted signing callback and explicitly leaves open what happens if the existing SSH binding requires key material instead. The pinned source answers that question for the current explicit-key route:

- `ssh_key_auth.rs:19–69` owns a network connection and a charged key-snapshot pin, then authenticates using `entry.text()`.
- Unix calls `Session::userauth_pubkey_memory` at lines 88–93.
- Windows calls `libssh2_userauth_publickey_frommemory` with the key bytes at lines 95–119.
- The separate agent route already has a signing callback; it does not establish the explicit-file-key route.

The credible use trace is an explicit file-key operation at the selected endpoint with no agent available. After the network adapter verifies the server:

1. Credential has admitted the selected key/source.
2. The network adapter owns the SSH session.
3. The current native call requires the selected key’s bytes.
4. Credential’s public sketch supplies no selected way to perform that call: no bounded local key-use capability is defined, no explicit-key signing route is selected, and Credential must not acquire network ownership.

Falling back to an agent violates explicit identity behavior. Reopening the path independently in the network adapter bypasses the proposed credential admission/ownership decision. Replacing the binding or inventing a new signer is not an established prerequisite.

**Impact:** A currently required SSH authentication path cannot yet be traced through the proposed public boundary. Accepting the design would leave its principal responsibility split to implementation discovery.

**Required correction:** Select and document the endpoint-local explicit-key handoff using the supported route. A localized option is to retain the actual memory-authentication operation in the existing trusted endpoint/provider adapter and expose a narrowly scoped admitted capability with explicit key-pin, cancellation and cleanup lifetime. Another route is acceptable if its feasibility is demonstrated.

State how the capability fits Credential’s no-network rule and Connection’s existing secret-placement/authorization port. Do not introduce a general key-export or arbitrary-signing API. Identify any exact Connection amendment needed; do not edit or assume amendment of the accepted contract in this checkpoint.

**Closure test:** Provide the missing explicit-file-key setup and reuse trace, including no agent, key replacement between operations, cancellation during authentication, selected endpoint-local paths and failure without fallback. Map the real Unix and admitted Windows memory-authentication calls to the chosen capability and its fake portable counterpart. Native execution remains a later qualification gate.

**NEW ARCHITECTURAL ROOT: YES.** This is an unresolved mechanism boundary between credential ownership and network-owned SSH authentication, independent of the terminal-lifecycle root.

## 2. Invariant analysis

**Selected endpoint and secret location are correctly constrained.**  
Credential `78–90` preserves endpoint-local keys, helper tokens, signing and path resolution. It forbids fallback and new secret/control protocols. This agrees with placement P1–P9. Sharing an address space does not authorize transferring driver secrets or signing capability to core.

**Two callers must not inherit one authenticated account.**  
Credential `159–162` retains caller/origin/physical-generation scope for NTLM/Negotiate. Accepted Connection `548–554` prevents treating such a connection as generic account-free TLS. Basic/helper output remains request-scoped. These constraints defeat reuse based solely on equal endpoint configuration.

**Token production is not peer authentication.**  
PeerAuthResult is supplied by the trusted network adapter, and the draft explicitly rejects treating token production as authentication. SSPI’s native Complete has the same restriction. The missing terminal grammar must preserve this distinction rather than replace it with a provider-only success bit.

**Trust, final origin and CBT remain network facts.**  
Credential consumes verified peer/channel facts and rejects mismatched scope. Network adapters retain TLS and server-trust ownership. The draft does not authorize synthetic or prior-generation channel binding, credential-driven endpoint selection, or redirected reuse of another origin’s material.

**Deadlines and native qualification are retained obligations.**  
AuthControl uses the existing absolute deadline, and challenge/provider rounds cannot renew it. Existing timeout-zero and interaction exceptions remain controlling. The accepted Connection native ProcessSafety gate still applies; extracting a provider cannot qualify an otherwise unsafe launcher.

**Independent tests are useful but do not waive composition.**  
Portable provider tests and fake-credential pool tests can reduce unrelated build work. The draft correctly retains affected provider, composition, placement and final-release checks. Repository separation alone is not executed evidence of reduced test coupling.

**Accepted Connection invariants remain intact.**  
This review does not reopen unrelated stream algorithms. Reserved-permit revocation, retained domain ownership, atomic application-before-Applied, finite binding state, conservative physical settlement and no ambiguous Git replay remain required in the composition.

## 3. Risks and next action

Native provider correctness, process safety, secret-copy behavior and actual installed placement remain later qualification risks. No current code defect or native qualification result is accepted by this document review.

**Next action:** revise the Credential design with the joint terminal/cleanup grammar and the explicit-key SSH handoff, including their concrete traces and any precisely named proposed Connection amendments. Re-review that committed documentation before authorizing repository creation or implementation.

