# Connection–Credential boundary contract — CONSISTENCY-AXIS REVIEW

**Review object:** `dev-docs/GwzConnectionCredentialBoundary.md`, revision 0, DRAFT, composed with both accepted API documents, at root `0570a27ececdc62b83b2bd31065aaf20559914aa`.

**Baseline:**

| Repository | Pinned HEAD |
| --- | --- |
| `.` | `0570a27ececdc62b83b2bd31065aaf20559914aa` |
| `gwz-core` | `9e8edb80e730e86e44894d79d18cef551f70e6e5` |
| `gwz-py` | `5b6b66579867d96d21d5d5d003aaa69f50960b4e` |
| `gwz-cli` | `5193db962e672c54cd20ab2ef8448d904129a559` |
| `gwz-git` | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| `gwz-transport` | `a27817b9c86fd60a07a9e77d62b1f5e94a5cf252` |
| `gwz-sspi` | `a2978a8f3f6ce7a4b387a2f9ebcad555a7c8888e` |
| `git2-rs` | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| `libgit2` | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All nine HEADs matched at start and end. Substantive inspection used pinned Git objects.

**Date:** 2026-10-19.

**Axis:** Consistency, including Surface: joint construction, dependencies, endpoint authority, handoffs, lifecycle and test-isolation claims. Independent, adversarial, read-only. Nothing here relies on the other axis’s current-round report. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings. The boundary is consistent with the accepted APIs and makes their composition obligations explicit. This accepts documentation only.

---

## 0. Evidence base

Inspected:

- The complete 215-line boundary contract.
- Accepted Credential public API, ownership, terminal grammar, retention, defaults and Connection clause mapping.
- Accepted Connection public API, exact four HostPorts, AuthContext/provider ports, shared SSH proofs, security scope and deadline rules.
- Credential’s accepted joint verdict and retained Connection acceptance evidence.
- A pinned diff confirming neither accepted API’s substantive body changed; Credential gained its acceptance header.
- Core `GwzRemoteTransportRequirements.md:244–272`, placement P1–P9.
- Core `src/git/endpoint/https_worker/native/sspi.rs:141–223`, confirming the existing captured-caller Port/Session and cleanup-probe route.
- Relevant source/authority evidence retained from the preceding accepted reviews: SSH agent callback and explicit memory-key authentication, retained native owners, Python original-entry capture, gwz-sspi supervision and current mechanism policy.

The boundary/public surfaces were inspected before targeted source checks. No nonexistent CLI help was run.

Commands were read-only `git rev-parse`, pinned `git show`, `git diff` and text inspection. No writes, builds, tests, network access, repository changes or further agents occurred. No current-round peer report or excluded draft was accessed.

## Problem, requirement provenance, and scope

The operator explicitly requested this boundary as a document and review before Credential implementation. The outcome is independently testable libraries with one explicit composition bridge, preserving endpoint-local authority.

The documented remedy addresses the established coupling without extracting whole connection-owning functions or creating another host/supervisor. Existing native Port/Session, cleanup probes, constrained agent signing and explicit memory-authentication primitives support the proposed route. No dependency upgrade, new carrier or provider rewrite is established as necessary.

The editable scope is confined to the boundary and review records. Both accepted APIs and all implementation dependencies remain reference-only. The document preserves that limit, excludes Python/builder work and requires separate implementation authorization.

No unsupported prerequisite or unauthorized expansion blocks this gate. Native process safety, installed placement and measured test-cost reduction remain execution obligations rather than accepted results.

## 1. Findings

No findings.

**NEW ARCHITECTURAL ROOTS: zero.** The prior accepted Connection and joint Credential roots remain closed; no unrelated ordering algorithm was reopened.

## 2. Invariant analysis

### Construction and dependency graph

Boundary lines 47–72 preserve exactly Clock, OwnerSeed, Progress and EndpointFactory as Connection HostPorts. Authentication is reached through the factory-created endpoint adapter’s existing §7 seams. Binding that factory to an explicitly constructed local bridge does not add a fifth port or permit ambient provider lookup.

The dependency graph remains acyclic: the bridge imports both APIs; neither library imports the bridge or the other library. Credential’s optional Windows provider may delegate to gwz-sspi. No third contract library, executor or cleanup domain is introduced.

The shared service supplies distinct projections for Credential and Connection. Their views reference admitted charges and retained records rather than copying secrets or minting independent capacity.

### Endpoint ownership and remote placement

Lines 39–45 distinguish the executing endpoint from the core-facing route/virtual stream. Under driver placement, the driver retains OperationCredentials, key/helper access, trust and original caller provenance. Core receives neither a driver CredentialContext nor a signing proxy.

This agrees with P4, P6 and P8: authentication executes at the selected endpoint, paths resolve there, and credentials do not cross the application/control channel. The scope/delivery metadata is not promoted into secret authority.

The remote-placement trace expressly distinguishes in-process routing evidence from installed endpoint qualification. It does not claim a new physical carrier exists.

### Handoffs and authentication responsibilities

The handoff table consistently maps:

- operation-owned CredentialContext into opaque AuthContext;
- per-open selection into independent conversations;
- verified peer/origin/generation facts into authentication;
- existing clocks, deadlines and reservations into AuthControl;
- actual peer acceptance separately from provider completion;
- terminal disposition and cleanup evidence into Connection settlement.

Network trust, socket readiness, physical disposal and HTTP framing remain network-adapter responsibilities.

Agent signing stays restricted to authentication/proof contexts. Explicit file login uses LocalSshKeyPort and the existing memory-authentication route, with snapshot retention across re-entry/cancellation. The boundary does not replace Connection’s reread-before-lookup, exact-identity or shared-proof policy.

HTTPS remains request-scoped for Basic/helper output and caller/origin/generation scoped for native authentication. BoundScope retention is conditional on current policy. No pool hit or native Complete becomes account authentication or replay permission.

### Revocation and retained cleanup

Lines 134–167 preserve the publication race outcomes: cancellation before commit prevents response publication; commit first may leave material queued or sent. Cancellation cannot establish no effect.

Retire fences the matching physical generation before another checkout. Retained-scope use requires live operation/peer/generation authority; revocation cannot be handled as a stale permission-cache check. Already admitted work remains subject to cancellation and retained cleanup.

Peer acceptance, terminal observation and native disposal remain independent. Losing public handles or futures does not abandon the bridge record. Native slots, physical permits and byte charges release separately on matching evidence. Five-second Pending reporting is not disposal.

### Surface walkthrough

The combined documents support a complete first use:

1. Supply the shared retention/progress service.
2. Build Credential explicitly.
3. Build Connection with the bridge-bound EndpointFactory.
4. Capture endpoint-local inputs and original caller provenance.
5. Retain OperationCredentials in the operation guard and pass only its charged context view.
6. Select/begin independent exchanges; supply verified peer facts.
7. Finish with actual peer results or cancel the child/operation.
8. Close provider, connection and domain owners using their accepted lifecycle rules.

No new default or flag is introduced by this seam. Accepted limits, refusal behavior and close/drop semantics remain inherited. No additional caller guess is required about who owns credentials, physical resources or pending cleanup.

### Test-isolation claims

The proof layers are accurately separated:

- Credential-only tests use fake sources/key ports/retention.
- Connection-only tests use fake endpoint/authentication adapters.
- Small composition checks both API surfaces through test code without core/native builds.
- The real production bridge has its own focused tests and may require a targeted core build.
- Installed provider/placement qualification remains separate.

A fake bridge is expressly not evidence of production adoption. The document also states that test-cost reduction has not been measured. These qualifications prevent the proposed test boundary from becoming a false integration claim.

## 3. Risks and next action

Implementation must still prove grant/publication revocation races, key-pin lifetime, shared cleanup ownership and actual production adapter consumption. The document defines those obligations without claiming their execution.

The next action is to merge both independent verdicts for this exact boundary revision. Any subsequent source work, repository creation, Python work or builder work requires its separately agreed scope and authorization.

