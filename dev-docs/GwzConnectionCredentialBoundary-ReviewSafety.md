# Connection–Credential boundary contract — SAFETY-AXIS REVIEW

**Review object:** `dev-docs/GwzConnectionCredentialBoundary.md`, revision 0, DRAFT dated 2026-10-19, composed with both accepted API documents at root `0570a27ececdc62b83b2bd31065aaf20559914aa`. Joint seam/design gate only.

**Baseline:**

| Repository | SHA |
| --- | --- |
| root | `0570a27ececdc62b83b2bd31065aaf20559914aa` |
| gwz-core | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| gwz-py | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| gwz-cli | `5193db962e672c54cd20ab2ef8448d904129a559` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| gwz-transport | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| gwz-sspi | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

Pinned documents were read using `git show`; accepted-document continuity was checked with `git diff`.

**Date:** 2026-10-19  
**Axis:** Safety — endpoint authority, credential publication, scope revocation, explicit-key use, retained cleanup and truthful composition evidence. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on its current-round report. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. This accepts the seam design only.

---

## 0. Evidence base

Inspected:

- The complete boundary contract, lines 1–215.
- Accepted Credential lifecycle, local key/output capabilities, accounting, scope and composition clauses, particularly current lines 142–332.
- The accepted Credential merged verdict.
- Both accepted APIs and their prior Safety evidence retained from the completed joint reviews.
- The diff from the accepted joint tuple `c0fe3cf8185087a61780b8a61e347c95efc83c4f`: Connection is unchanged; Credential differs only in its acceptance header and links.

Retained pinned source evidence includes core’s explicit-key memory-authentication route, agent-signing callback, native Port/Session cleanup probes, original-entry caller capture, and SSPI supervision documentation. These support the existing adapter route; they do not establish implementation of the proposed seam.

All nine heads matched the required tuple at the start and end. Inspection was read-only. No files, builds, tests, network operations, mutations or further agents were created. No current-round peer report or excluded draft was read.

## Problem, requirement provenance, and scope

The operator requested this boundary as a document and review before Credential implementation. The purpose is explicit responsibility and test separation between the two accepted APIs, while preserving endpoint-local authority.

The document addresses that purpose with one endpoint composition bridge. It retains the four Connection HostPorts, places authentication behind the existing endpoint seams, and identifies which operation owner and capability live at the executing endpoint.

Existing native and credential adapters provide a supported route for this design. Explicit keys already use memory-authentication calls; native authentication already exposes conversation and cleanup-probe seams. No dependency upgrade, replacement supervisor or new network layer is established as necessary.

Only this boundary document and its review records are editable. Both accepted APIs, member sources and dependencies remain reference-only. Python and builder work remain outside scope. No silent API amendment or unsupported prerequisite was found.

The separate libraries and bridge remain design obligations. Their creation, migration, actual test-cost reduction and production adoption are not claimed.

## 1. Findings

No findings.

**NEW ARCHITECTURAL roots identified: zero.** The previously closed joint roots remain closed.

## 2. Invariant analysis

**Remote placement does not lend driver credentials to core.**  
Lines 39–45 place the bridge and credential resources at the driver endpoint. Its operation handler owns OperationCredentials; core holds routing and virtual-stream authority. Construction and handoff clauses exclude driver CredentialContext, signing proxies, local path checks and worker-thread identity recapture in core.

Removing the driver capability must therefore refuse rather than trigger local authentication. The mandatory placement trace preserves that distinction without requiring a future physical carrier.

**Construction does not introduce another host or ambient provider.**  
Lines 49–67 retain Clock, OwnerSeed, Progress and EndpointFactory as the complete Connection HostPorts. EndpointFactory binds the local bridge during construction. The operation guard retains the unique credential owner; Connection receives only the charged selection view. Exchange teardown cannot close shared owners or the domain.

**Explicit-file SSH retains the supported handoff.**  
Lines 106–119 preserve network ownership of the SSH session and host trust, while Credential owns bounded source/proof work. LocalSshKeyPort borrows the admitted snapshot for the existing memory-authentication route. The pin survives re-entry and in-flight native work. No agent fallback, path reopen, general key export or Credential-owned socket is introduced.

The accepted Credential rules continue to require cancellation fencing and connection-before-pin disposal for rejected or uncertain work.

**Publication and cancellation have an explicit race outcome.**  
Lines 136–140 require response commitment through Credential’s publication guard. Cancellation winning exposes no response; commitment winning may leave material queued or sent. Neither outcome implies peer acceptance or absence of Git effects.

The terminal bridge survives loss of the waiting caller. Retire fences the exact physical generation before another checkout, and application callers cannot assert healthy return.

**Scope revocation cannot become a cached permission check.**  
Lines 161–167 require a live grant for the exact operation, peer and generation, with revocation ordered against use and publication. If revocation wins, no new grant/output is permitted and the candidate retires. If use wins, admitted work remains subject to cancellation and retained cleanup.

This is consistent with accepted provider shutdown revoking transferred scopes. It does not treat an old TLS hit or native Complete as current account authority.

**Native and physical cleanup remain distinct.**  
Peer acceptance with pending native finish preserves both facts independently. A native worker slot ending cannot free a physical permit, and physical closure cannot invent native disposal. Matching-generation proof releases each resource once; duplicate completion cannot settle another generation.

Lost public futures and five-second reporting timeout leave the retained bridge and provider supervisor responsible. Replacing an owner cannot bypass occupied domain capacity.

**Different operations and identities remain isolated.**  
Per-open selection stays within captured policy. One conversation’s cancellation does not close siblings or unrelated operations. Origin, caller and generation remain explicit for connection-bound authentication, while Basic/helper output remains request-scoped. Existing no-downgrade, trust, CBT and challenged-POST rules are retained.

**Test claims distinguish fake composition from real adoption.**  
Section 7 separates Credential-only, Connection-only, small fake composition, production bridge and installed placement evidence. Lines 179–183 expressly acknowledge that the real bridge may require a targeted core build and that no test-cost reduction has been measured.

A fake bridge is therefore not presented as proof that core adopted the seam. No third published contract library is introduced, and affected scope/CBT/cleanup changes still require joint and provider checks.

## 3. Risks and next action

Implementation must still prove grant/revocation ordering, secret-pin lifetime, retained progress, accounting and installed endpoint placement. Native ProcessSafety qualification and real production-bridge evidence remain required.

**Next action:** merge the two seam-review verdicts and record acceptance at this exact tuple if both axes are GO. Repository creation and portable implementation require separate authorization.

