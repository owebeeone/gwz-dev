# GWZ Credential API design with accepted Connection — SAFETY-AXIS REVIEW, remediation round 1

**Review object:** `dev-docs/GwzCredentialApiDesign.md`, revision 1, and `dev-docs/GwzCredentialApiDesign-RemPlan-1.md`, composed with the accepted `dev-docs/GwzConnectionApiContract.md`, at root `c0fe3cf8185087a61780b8a61e347c95efc83c4f`. DRAFT dated 2026-10-19. API/design gate only.

**Baseline:**

| Repository | SHA |
| --- | --- |
| root | `c0fe3cf8185087a61780b8a61e347c95efc83c4f` |
| gwz-core | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| gwz-py | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| gwz-cli | `5193db962e672c54cd20ab2ef8448d904129a559` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| gwz-transport | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| gwz-sspi | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

Pinned documents were read with `git show`. The complete Credential delta from root `158a940b961941ebe0bd5c47c771dd9658156a04` was inspected with `git diff`.

**Date:** 2026-10-19  
**Axis:** Safety — authentication state, selected endpoint, secret boundaries, cancellation, retained cleanup, physical settlement and scope. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on its current-round report. Filed verbatim by the lane owner.

**Verdict: GO** — both prior Safety P2 findings are closed at the design gate. Zero open P0, P1, P2 or P3 findings. No implementation or native qualification is accepted.

---

## Prior-finding closure

| Finding | Result | Independent closure evidence |
| --- | --- | --- |
| Safety P2-1 — missing joint shutdown and terminal-settlement grammar | **Closed** | Lines 228–271 define serialized revocation, synchronous close/drop behavior, terminal outputs, accepted-peer facts and physical dispositions. Lines 273–311 define retained bridge ownership, immediate terminal observation, separate native/physical permits and bounded reporting without caller polling. |
| Safety P2-1 — provider owner closes while a native session survives | **Closed** | Provider close construction/drop seals admission, cancels all operations and revokes scopes. Dropping ProviderClosing abandons observation only. Late native completion updates cleanup rather than publication; the bridge marks the affected physical generation Retire. |
| Safety P2-1 — peer accepted, AuthFinishing dropped, native cleanup pending | **Closed** | The recorded peer result survives independently of Confirmed/Pending cleanup. Retention continues after lost futures; BoundScope is permitted only under existing policy, otherwise physical retirement applies. Corresponding permits release only on their own proof. |
| Safety P2-2 — no selected explicit-file-key handoff | **Closed** | Lines 183–209 select LocalSshKeyPort over the existing memory-authentication calls, with a scoped secret borrow and stable admitted snapshot. Lines 374–395 trace no-agent setup, changed key bytes, EAGAIN/re-entry and cancellation. |

These are design-level closures, not executed tests.

**NEW ARCHITECTURAL roots identified in this round: zero.** The correction resolves the previously identified terminal/accounting and provider-mapping roots. It does not reopen the separately accepted Connection roots.

## Changed-range analysis

The entire Credential diff was reviewed.

- Section 4 adds an operation owner/context split. One captured operation can issue independent one-use child conversations without recapture or endpoint switching.
- Section 4.1 selects separate agent and explicit-file-key routes. The latter preserves the supported memory-authentication primitive rather than requiring a new signer or binding.
- Section 5 replaces unresolved lifecycle questions with explicit close/drop, publication, terminal and physical-disposition rules.
- Section 5.1 maps retention onto the existing composition service and provider supervisors.
- Section 5.2 states finite defaults and refusal before effects.
- Section 7 supplies consumer mapping, joint traces, closure oracles and a source-based test-impact comparison.
- Section 8 maps the proposal to exact accepted Connection clauses.

A direct diff confirmed that the accepted Connection contract is unchanged. Its ordering, ownership, qualification and cleanup requirements remain controlling.

The same reviewer continues under the operator’s instruction to use the old reviewers. No additional reviewer was created.

## 0. Evidence base

This round inspected:

- The regenerated canonical Safety prompt.
- The complete Credential diff from `158a940b961941ebe0bd5c47c771dd9658156a04` to `c0fe3cf8185087a61780b8a61e347c95efc83c4f`.
- Revision-1 Credential lines 92–332 and 350–484 with line numbers.
- The complete merged remediation plan.
- The empty Connection-contract diff between those roots.
- The prior Safety findings and retained source/reference evidence from the initial joint review.

That retained evidence includes:

- Accepted Connection §§4–5 and §7: revocation, retention, authentication ports, security scopes and native safety.
- Core `ssh_key_auth.rs:1–127`, `ssh_key_snapshot.rs` ownership/view clauses and `agent_auth.rs:45–135`: supported memory-key and agent-callback routes.
- Core `https_worker/native/sspi.rs:1–224` and `native.rs:1–175`: original caller capture, current provider seam, bounded token handling and cleanup probes.
- SSPI public supervision/caller-value documentation: immutable deadlines, publication revocation, pending ownership and native Complete versus peer acceptance.
- Transport placement requirements P1–P9 and the accepted Connection remediation authority.

All nine repository heads matched the required tuple at the start and end.

Only read-only inspection was performed. No files, builds, tests, network operations, mutations or further agents were created. No current-round peer report or excluded draft was read.

## Problem, requirement provenance, and scope

The operator requested separation of conflated connection/credential responsibilities, independently cheap tests, a proposed separate Credential repository, and review of the composed Credential/Connection boundary.

The source establishes real coupling candidates: current native composition combines caller capture, supervision, gate and backend concerns, while SSH authentication combines network-owned native sessions with credential material. The proposed extraction now maps those responsibilities to concrete existing seams. It does not claim that authentication caused every prior churn incident.

The existing provider-port alternative was considered explicitly. Section 7.5 acknowledges that those ports could also host extracted policy and distinguishes source-based feasibility from measured build/test improvement. Repository separation remains the selected proposal, not a demonstrated prerequisite for replacing native providers.

The remediation stays within the authorized documentation boundary. Credential design and joint review records change; Connection and source/dependency repositories remain reference-only. No new supervisor, network stack, carrier, secret store or dependency upgrade is selected.

The explicit-key correction uses the supported native memory-authentication route. The lifecycle correction projects onto existing supervision and Connection retention. Neither requires broader infrastructure work.

Section 8 identifies the exact Connection clauses being refined by composition while preserving their requirements. No unsupported prerequisite or unauthorized scope expansion was established.

## 1. Findings

No open findings.

## 2. Invariant analysis

**Provider close cannot leave a surviving session with undefined publication authority.**  
Re-running the original trace: A is responding, B is active, and the provider owner closes. Construction of close synchronously seals admission and revokes both operations. Dropping its future does not undo revocation or stop retained progress. A late result can contribute cleanup evidence but cannot publish new material. The bridge observes the terminal decision and fences the matching physical generation without awaiting final cleanup.

Operation A’s ordinary close has narrower scope: it cancels A’s children while B remains usable. Provider shutdown intentionally affects all provider-owned operations and scopes.

**Lost futures do not orphan pending resources.**  
Opening drop covers successful-but-unconsumed sessions. Respond-future drop terminalizes that conversation. Finish/cancel and owner-close futures lose only their observation on drop. The retained bridge record keeps native work, probes, pins and terminal state alive through the existing service. No final application poll is needed to drive disposal.

**Peer acceptance is preserved without fabricating cleanup.**  
A peer-accepted finish can remain Pending. The peer fact does not free an SSPI slot, physical permit or cleanup allowance. RequestSettled still requires network framing/body settlement. BoundScope remains conditional on current policy, including required native disposal. Rejection, uncertainty, cancellation and provider revocation select Retire.

**Explicit-key SSH works without an agent fallback.**  
The network adapter retains the handshaken SSH session and native call. Credential retains the exact admitted snapshot. LocalSshKeyPort receives a scoped local borrow and returns native progress while readiness waits stay outside Credential. EAGAIN/re-entry uses the same bytes; changed file contents in a later operation produce a new identity decision.

Cancellation blocks further calls and publication. An admitted native call may finish, but its pin remains charged. An uncertain/rejected connection is destroyed before releasing the key pin. Existing unsupported Windows key-form refusals remain before the native call.

**Secret views do not become application or remote authority.**  
The privileged key port exists only at the executing endpoint. Its view cannot be serialized or transferred as an owned application value. The design honestly treats native adapters as trusted code rather than claiming protection against malicious in-process copying.

Response capabilities bind session, peer/generation and purpose. Cancellation before commit rejects publication; after commit it cannot unsend protocol material. The queue retains charge until send/discard and wiping. Neither outcome implies peer authentication or replay permission.

**One operation can support multiple independent exchanges.**  
OperationCredentials supplies a charged CredentialContext to Connection’s AuthContext. Per-open selection derives child conversations without widening captured policy or recapturing caller identity. Cancelling H1 does not cancel H2. Connection retains the existing shared-proof coordinator, waiter deadlines and cancellation semantics; Credential adds no competing proof scheduler.

**Account, origin and generation boundaries remain intact.**  
Basic output remains request-scoped. Native conversations and accepted scopes retain caller/origin/physical-generation binding. Redirected/new-origin work requires a new scoped selection under existing policy. Final TLS CBT remains a network fact; ProviderComplete remains distinct from peer acceptance. Existing no-downgrade and challenged-POST refusals are preserved.

**Bounds and deadlines remain meaningful across teardown.**  
Conversation records include pending cleanup and retained scopes. Exhaustion refuses before work allocation; replacing public owners cannot bypass shared domain accounting. Byte allocations have one shared reservation object rather than cloned independent capacity. Reporting timeout does not release resources or renew the exchange deadline. Existing interaction, timeout-zero and ProcessSafety rules remain controlling.

**Accepted Connection protections remain preserved.**  
The patch does not alter reserved-permit revocation, atomic stream application, binding-local lifetime limits, stale-generation rejection, physical-fault scope or conservative Git effect/replay rules.

## 3. Risks and next action

Implementation must still prove the scoped key borrow and pin lifetime, publication arbitration, retained progress after owner destruction, exact resource accounting and actual selected-endpoint behavior. Native qualification and measured test isolation remain future gates.

**Next action:** merge the two review verdicts and record design acceptance at this exact tuple if both axes are GO. Repository creation and the smallest portable implementation proof require separate authorization.

