# DRAFT SSPI caller API — SAFETY-AXIS REVIEW

**Review object:** `dev-docs/GwzSspiCallerGuide-DRAFT.md` at root `377c5e29882e353e42dbe75031b175374b213f7f`. DRAFT contract, without implementation; reviewed 2026-10-03.

**Baseline:** root `377c5e29882e353e42dbe75031b175374b213f7f`; gwz-core `d78a664e3c5a325c6f12be409eb7645c1c1b51d0`; evidence `1beb1d204c824701ddbd033c7f89df9a3561f5e5`. The review object was read with `git show` using the exact root SHA. All three HEADs matched at the beginning and end.

**Date:** 2026-10-03

**Axis:** Safety: what a cold caller walkthrough permits to go wrong, including mechanism identity, bounded ownership, cancellation, cleanup, containment and lifecycle. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

Read:

- Workspace instructions in `AGENTS_GWZ.md`.
- Canonical review brief in `dev-docs/GwzSspiDesign-PromptSafety.md`.
- Committed `dev-docs/GwzSspiCallerGuide-DRAFT.md`, lines 1–74, including installation, the public API table, authentication inputs, deadlines, the host sequence, cancellation and quarantine semantics.

Commands:

- `git rev-parse HEAD`
- `git -C gwz-core rev-parse HEAD`
- `git -C gwz-core-evidence rev-parse HEAD`

Each was run at review start and end, producing the exact baseline tuple above.

- `git show 377c5e29882e353e42dbe75031b175374b213f7f:dev-docs/GwzSspiCallerGuide-DRAFT.md`
- The same committed content numbered with `nl -ba` for locations.

The Surface restriction was applied: no design, plan, source, native reports or current peer report was read. No files were written, no builds or tests were run, and no repository state was changed. Findings concern behavior permitted by the draft contract, not missing implementation.

## 1. Findings

### [P2-1] Negotiate completion does not expose the actual authentication mechanism

**Location:** Caller guide lines 24 and 31–38; host sequence lines 55–58.

**Architectural root cause:** The public result contract conflates the requested negotiation package with the mechanism actually selected. `AuthRequest` offers `Negotiate`, `Ntlm` or `Digest`. `TokenStep` exposes status, checked attributes and token bytes, but specifies no actual negotiated mechanism or mechanism-policy result.

**Violated invariant:** A caller must be able to distinguish successful token generation from selection of an authentication mechanism that its security policy permits. Ordinary native context attributes do not establish that a Negotiate conversation selected Kerberos rather than NTLM.

**Reproduction/state sequence:**

1. A host has a policy requiring Kerberos for an origin.
2. It requests `Package::Negotiate`; the guide exposes no direct Kerberos package or mechanism restriction.
3. Negotiation selects NTLM and reaches `Complete`.
4. The host receives the documented status, attributes and opaque token.
5. It cannot establish the selected mechanism through the documented API before publishing the token. Its choices are to reject every Negotiate result, inspect opaque authentication tokens itself, or accept without enforcing its policy.

No Windows provider qualification outcome is assumed here. The defect is the caller interface’s inability to represent the outcome, which remains in scope.

**Impact:** A conforming integration cannot reliably enforce mechanism restrictions. Treating the requested package as the actual mechanism permits authentication-policy downgrade; parsing tokens independently creates an undocumented authentication dependency.

**Required correction:** Define actual mechanism identity explicitly in the public result contract, including its availability during negotiation and at completion. Also define how callers restrict permitted mechanisms and when a disallowed or indeterminate selection stops token publication. If mechanism restrictions are intentionally unsupported, state that limitation explicitly and provide a documented way to obtain the actual mechanism before accepting the completed conversation.

**Closure/regression test:** A caller walkthrough requesting Negotiate must distinguish a Kerberos completion from an NTLM completion through documented fields. A Kerberos-only policy must reject the latter without parsing token bytes, and the contract must specify whether any token can be published before that policy decision.

### [P2-2] Worker capacity does not bound queued request ownership

**Location:** Caller guide lines 22–23, 31–33 and 65–72.

**Architectural root cause:** Admission bounds running and quarantined workers but leaves the waiting population unbounded. `start` owns an `AuthRequest`, waits for worker capacity, and has no documented queue limit, immediate saturation rejection option or finite pending-request budget.

**Violated invariant:** A resource-containment contract must bound resources retained outside its contained workers as well as resources inside them. A worker-count bound cannot establish bounded parent ownership when arbitrary numbers of credential-bearing requests may wait.

**Reproduction/state sequence:**

1. Eight workers occupy the default capacity.
2. Cancellation quarantines them; the guide explicitly allows cleanup duration to remain unbounded.
3. The host continues starting operations with valid, distant absolute deadlines.
4. Every additional request is permitted to wait for capacity while retaining its owned identity/password and cancellation/deadline state.
5. The guide supplies no admission threshold that stops this accumulation. Eventual deadline expiry does not provide a finite bound on simultaneous retained requests.

This does not require a provider defect to be accepted as normal behavior: the guide explicitly reserves capacity until cleanup is confirmed and expressly declines to bound cleanup time.

**Impact:** The containment model permits unbounded parent memory and credential retention during prolonged quarantine. A host following the minimal sequence has no documented admission control with which to prevent this accumulation.

**Required correction:** Specify a finite pending-admission limit with a default and accepted range, or require immediate refusal when worker capacity is unavailable. Define the rejection kind, ownership disposal and cancellation/deadline behavior for rejected and queued requests. If admission bounding belongs to the host, make that a mandatory precondition and give the public API a documented way to implement it without retaining an unbounded queue.

**Closure/regression test:** Hold all worker slots in pending cleanup and submit requests beyond the stated admission bound. Demonstrate from the contract that retained requests remain bounded, excess requests receive a defined result, and dropping or cancelling a queued request releases its secret ownership without consuming a worker slot.

## 2. Invariant analysis

The following attacks did not establish additional findings:

- **Mixed-version fallback:** Lines 8–13 require a matching installed worker and make missing workers and protocol/build mismatches errors. The text does not permit silent fallback to another executable.
- **Executable discovery:** The contract uses an absolute installed path, excludes PATH lookup and shell launch, and requires a trusted path in the host sequence.
- **Authentication versus remote acceptance:** Lines 5–6 and 55–58 expressly separate token generation from server acceptance. `Complete` does not falsely compose the two.
- **Connection rebinding:** Lines 36–38 require binding from the actual verified final-origin TLS connection. Lines 59–61 require cancellation and lease closure on connection loss or redirect and prohibit moving a conversation between connections.
- **Deadline resets:** Lines 40–43 prohibit extending the original deadline and count launch, native calls and delivery against it. The contract does not grant additional time per challenge.
- **Cancellation versus completion:** Cancellation immediately revokes publication but reports cleanup separately. Timeout is explicitly not evidence that an OS call or external provider stopped.
- **Premature capacity reuse:** Lines 70–72 retain a quarantined worker’s slot until process exit, Job emptiness and IPC/launch-thread completion are confirmed.
- **Dropped futures and supervisor ownership:** Lines 65–68 retain supervision after launch registration and cancel on drop. The documented ownership does not disappear merely because the caller stops awaiting.
- **Cleanup record eviction:** Line 27 returns `Unknown` for evicted IDs rather than manufacturing confirmation.
- **Error disclosure:** Lines 44–48 restrict failures to fixed kinds, optional numeric status and cleanup identifiers, excluding secrets, identity and native error text. Secret containers lack Debug/Clone and use zeroizing storage.
- **Installation lifecycle:** Lines 8–13 pair worker installation, upgrade and removal with its owning application. No independently orphaned worker installation is prescribed.
- **Deferred provider outcomes:** Digest qualification remains gated. Full Windows provider, SSO, EPA, trust, proxy and Pageant qualification were not treated as defects merely because they are deferred.

The guide’s “private bootstrap pipe handles” and “contained process” descriptions are high-level contracts. This Surface-only review cannot verify their internal realization and does not infer an implementation defect from that limitation.

## 3. Risks and next action

The package is a draft without implementation. These conclusions establish defects in the caller contract, not observed runtime credential exposure or resource exhaustion.

The next action is to amend the caller guide with an enforceable actual-mechanism contract and bounded pending admission, then repeat the cold caller walkthrough against those two changes.
