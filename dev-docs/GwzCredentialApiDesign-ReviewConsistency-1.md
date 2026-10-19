# GWZ Credential API design — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzCredentialApiDesign.md`, revision 1, DRAFT, and `dev-docs/GwzCredentialApiDesign-RemPlan-1.md`, composed with the unchanged accepted Connection contract, at root `c0fe3cf8185087a61780b8a61e347c95efc83c4f`.

**Baseline:**

| Repository | Pinned HEAD |
| --- | --- |
| `.` | `c0fe3cf8185087a61780b8a61e347c95efc83c4f` |
| `gwz-core` | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| `gwz-py` | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| `gwz-cli` | `5193db962e672c54cd20ab2ef8448d904129a559` |
| `gwz-git` | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| `gwz-transport` | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| `gwz-sspi` | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| `git2-rs` | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| `libgit2` | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All nine HEADs matched at the beginning and end. Substantive inspection used pinned Git objects.

**Date:** 2026-10-19.

**Axis:** Consistency, including API Surface: the composed Credential/Connection boundary, lifecycle, provider mapping, compatibility, scope and independent-test feasibility. Independent, adversarial, read-only. Nothing here relies on the other axis’s current-round report. Filed verbatim by the lane owner.

**Verdict: GO** — both original Consistency P2 findings close at the design gate. No new P0–P3 findings. This accepts the reviewed design and composition obligations, not implementation, native qualification, measured test-cost reduction or release.

---

## Prior-finding closure table

| Prior finding | Result | Independent retrace |
| --- | --- | --- |
| **Consistency P2-1 — undefined terminal and retained-resource contract** | **Closed at the design gate** | §§4–5 now specify operation/context ownership, stage-specific close/drop behavior, AuthEnd’s independent peer/cleanup facts and physical disposition, synchronous revocation, bounded reporting and the retained bridge record. The original A-pending/B-active and final-owner-loss cases have explicit outcomes in §7.4. Native slots and physical permits settle separately; lost public futures do not abandon progress. |
| **Consistency P2-2 — operation/provider composition remained undecided** | **Closed at the design gate** | OperationCredentials retains one captured operation and issues independent one-use conversations through CredentialContext::select. The core bridge is named. §§4.1 and 7 map agent signing, explicit memory-key authentication, HTTPS policy, native sessions, shared proofs and original-entry capture. §7.5 supplies the requested source-based small-change comparison. |

The explicit-file-key correction identified in the merged remediation was also inspected. Its scoped LocalSshKeyPort preserves the existing memory-authentication route and assigns native session/readiness handling outside Credential.

No implementer disposition was assumed closed merely because it appeared in the remediation table. These closures follow the revised text and counterexample retraces; no executed proof is claimed.

**Architectural-root accounting:** No NEW ARCHITECTURAL root is identified. The joint object’s two previously identified roots are resolved in this revision’s design. The third-root lane-stop rule is not triggered.

## Changed-range analysis

The entire Credential diff from `158a940b961941ebe0bd5c47c771dd9658156a04` to `c0fe3cf8185087a61780b8a61e347c95efc83c4f` was read.

The revision replaces the preliminary sketch with five concrete decisions:

1. **Operation versus conversation authority:** prepare creates OperationCredentials; cloneable contexts issue bounded, independent children without recapture or fresh operation admission.
2. **Endpoint-local output:** agent responses use constrained authentication/proof contexts; explicit keys use a scoped native borrow through LocalSshKeyPort; response publication has a revocation check and defined queue ownership.
3. **Terminal grammar:** each owned stage has close/drop behavior; peer acceptance, provider completion, cleanup and physical disposition remain separate facts.
4. **Retention and limits:** the composition projects Credential records into the existing retention service, preserves qualified provider supervision and states finite defaults/refusals.
5. **Production and test mapping:** the revised document identifies which responsibilities move, which remain native/network/application responsibilities, and which tests a representative small change should require.

The Connection contract has no diff at this checkpoint. Its accepted ordering, domain, registration and effect rules remain controlling.

The same two reviewers continue under the recorded operator instruction to use the old reviewers. No additional reviewer or current-round peer material was used.

## 0. Evidence base

This round inspected:

- The complete regenerated Consistency prompt.
- `GwzCredentialApiDesign-RemPlan-1.md:1–55`.
- The complete Credential revision 0 → revision 1 diff.
- Revised Credential:
  - API and ownership, lines 92–171;
  - output/key handoff, lines 173–224;
  - terminal grammar and accounting, lines 226–332;
  - test layers, lines 334–351;
  - consumer map, joint traces and change-impact comparison, lines 353–459;
  - Connection composition/adoption consequences, lines 461–484.
- Unchanged accepted Connection:
  - domain/retention, lines 167–190;
  - operation inputs and authentication ports, lines 458–500;
  - SSH exact identity and shared proofs, lines 502–520;
  - request-bound/native scope, lines 548–560;
  - timeout behavior, lines 693–706.
- Pinned `gwz-core/src/git/endpoint/ssh_key_auth.rs:1–127`, including:
  - connection-before-key-pin ownership;
  - exact admitted snapshot use;
  - EAGAIN/readiness handling;
  - Unix memory authentication;
  - the existing Windows memory-authentication call and unsupported-form protection.
- Earlier unchanged source evidence retained from the original review:
  - native Port/Session and original caller capture;
  - retained Starting/Finishing probes;
  - bounded agent-authentication callback;
  - SSH OpenRequest;
  - Python route capture;
  - gwz-sspi supervision/caller/cleanup API;
  - current placement and mechanism policy.

Commands were inspection-only: `git rev-parse`, `git show`, `git diff`, `rg`, `nl`, `sed` and `cat`. No files were written, builds/tests run, network accessed, dependencies changed or additional agents launched. No current-round peer report or excluded draft was read.

## Problem, requirement provenance, and scope

The operator’s requested outcome remains reduced conflation between communication ownership and credential behavior, with independently cheap tests for small changes. A separate Credential repository is a selected proposal; repository creation, source extraction, provider rewrites and release remain unauthorized.

The observed coupling remains supported by source. NativeHost currently includes SSPI supervision, spawn-gate composition and backend construction; NativeCaller also contains proxy data. SSH requests combine credential authority with connection/setup/Git fields. Python captures inputs and enters an operation transport route together.

The revised remedy now follows demonstrated supported seams rather than treating those entire structures as extraction units. Native Port/Session, retained cleanup probes, agent signing and explicit memory-key authentication already exist. The new map preserves physical ownership, native session handling, readiness, trust, proxy capture and application routing outside Credential.

The explicit-key source check confirms a supported localized route: memory authentication already operates on admitted snapshot bytes on both admitted platform paths. There is no demonstrated need to replace libssh2, introduce a new signer or upgrade dependencies.

The scope stays within the Credential document and joint records. The accepted Connection contract and all source repositories remain reference-only. Section 8 records how the new provider composition uses existing Connection clauses and carries refinements into a later reviewed migration package. It introduces no wire, ordering, endpoint-route or cleanup-domain amendment.

The source-based test-impact comparison is sufficient design evidence for this gate. It does not establish measured build speed or runtime behavior, and the document now says so explicitly. No unsupported infrastructure prerequisite blocks the proposed small proof.

## 1. Findings

No new findings.

Both original Consistency P2 findings are closed in the revised design. No open Consistency P0, P1, P2 or P3 remains.

## 2. Invariant analysis

### Surface and first-use lifecycle

The revised public recipe is now coherent:

- Build the provider owner/handle with explicit providers and clock/progress/retention ports.
- Prepare one original-entry capture into OperationCredentials.
- Place a charged CredentialContext in Connection’s AuthContext while the application retains the operation owner.
- Select one-use child conversations for individual origins or identities.
- Begin, respond or use the privileged local key port.
- Finish with actual peer facts, or cancel the child.
- Close the operation and finally the provider owner.

The authority differences are explicit. Dropping a context does not cancel its operation; cancelling one child does not cancel siblings; closing OperationCredentials cancels its children; provider-owner close intentionally revokes all its operations and transferred provider scopes.

Closing values have observable end/report semantics. Cleanup reporting has the existing five-second bound and Pending does not release resources. The new finite provider limits are identified as proposed design choices rather than existing externally mandated constants.

There is no CLI help for these unimplemented APIs, and none was required or run.

### Original terminal/retention counterexample

The original A-pending/B-active case now has a specified path:

1. A and B hold separate operation authority under one provider.
2. A’s opening or response is cancelled or dropped.
3. A’s publication is fenced; B remains usable.
4. Registered native work, pins and cleanup probes remain in the shared retained bridge record.
5. Closing/dropping the provider seals all remaining submissions and revokes affected scopes immediately.
6. Losing public closing futures abandons observation only.
7. The existing progress/supervision route continues cleanup, and each resource releases only on its matching proof.

An accepted peer result remains a peer fact even when cleanup is pending or later revocation forces physical retirement. Native completion cannot manufacture peer acceptance. Duplicate/stale completion cannot release another generation.

The provider/native slot and Connection physical permit are explicitly different resources. One retained byte allocation has one reservation referenced through the relevant views, not independent cloned capacity.

### Multiple conversations from one operation

The consuming begin method now applies to a child selection, not the entire operation authority.

H1 and H2 can be derived from the same captured operation with separate challenge, deadline, cancellation and output state. Cancelling H1 leaves H2 and later permitted selections usable. Closing the operation cancels all its children without affecting operation B.

Selected children cannot widen captured policy, switch endpoints or recapture caller identity. A selection held across owner revocation fails before provider work. This resolves the original multiplicity ambiguity.

### SSH agent and explicit-key routes

The map no longer proposes moving the connection-owning agent-authentication function wholesale.

Credential owns bounded transcript/algorithm/signature policy and local agent work. The network adapter retains the SSH session, FFI allocation and readiness handling. Authentication and reuse-proof purposes remain constrained; no general signing service is introduced.

For explicit files, Credential owns the admitted snapshot and exact-byte identity. The privileged LocalSshKeyPort invokes the existing memory-authentication route at the executing endpoint. Re-entry uses the same snapshot; it does not reopen a path or fall back to an agent.

Calls reserve an in-flight pin before releasing state locks. Cancellation prevents new calls but cannot pretend an admitted native call disappeared. Uncertain/rejected connection disposal precedes key-pin release. The revised trace and pinned source agree on this ownership requirement.

Connection’s existing reread-before-lookup, trust and exact-identity rules remain controlling. The extraction does not replace them with operation-start path equality.

### Proof sharing and HTTPS policy

Connection retains its per-binding proof coordinator, waiter deduplication, individual waiter deadlines and accepted latest-waiting-request rule. Credential supplies one bounded proof conversation for that job rather than another cache or scheduler.

For HTTPS, portable offer/mechanism policy is separated from network sends, reads, redirects, framing and challenged-POST refusal. The selected policy preserves Windows logon Negotiate/NTLM ahead of helper Basic, no downgrade after native rejection and existing Digest refusal.

Final origin, actual TLS binding and physical generation come from the network adapter. AuthEnd maps to the existing authorization receipt, security-scope tag and cleanup probe. BoundScope is conditional on current policy; the design does not turn every native-authenticated connection into an account-free reusable lease.

### Publication and scope revocation

Response capabilities are session/peer/generation/purpose bound. Cancellation before commit prevents publication; commit before cancellation may leave material queued or sent and records that conservative outcome. No peer acceptance or replay permission is inferred.

Queue ownership includes the material’s charge and disposal/wiping obligation. A stale capability cannot authorize another operation, origin or generation.

Provider shutdown revokes transferred provider scopes and causes physical retirement through the bridge. Ordinary operation close does not cancel another operation or revoke a scope already transferred under permitted policy.

### Dependency and test isolation

The libraries remain independent leaves with no reverse core/Git/Connection/transport dependency. The named core bridge consumes both APIs. Proxy, trust, backend construction and application results remain outside Credential.

The change-impact comparison distinguishes genuinely local parser/policy changes from scope/cleanup/CBT changes that still require composition and provider checks. Pool-only work uses fake credential ports. This supports the requested cheap-test structure without pretending that all authentication changes become local or that repository separation alone proves a speed improvement.

The accepted Connection stream ordering and application-before-Applied invariants are unchanged and were not unnecessarily re-reviewed.

## 3. Risks and next action

Implementation must prove the selected retention bridge, cancellation/publication races, native key-pin lifetime, shared-proof deadline projection and original caller scope. Native process safety and installed endpoint/client behavior remain qualification gates.

The smallest later proof should exercise the revised fake lifecycle and multi-conversation traces, LocalSshKeyPort re-entry/cancellation, independent native/physical disposal, and the stated parser-versus-pool test isolation. This review supplies no executed result for those cases.

The next action is to merge the two independent re-verdicts for this exact revision. This GO authorizes no repository creation, source migration, dependency change, native qualification, publication or release.

