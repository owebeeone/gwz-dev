# GWZ Remote Transport Four-Document Draft — Consistency Round-2 Review

**Axis:** Consistency  
**Review object:** gwz-core `05842b38e55f109ed3663555680751811a72eb9b`, compared with original reviewed object `3d2cc92c4a233e40ed049c8fe79bc555d39b272f`.  
**Prior-round inputs:** root `3a0b8fa975bc013673ee919b087f69da9f3853ff`.  
**Scope:** The four transport requirements/design documents, with changed-range interaction analysis against the previously reviewed unchanged text.

**Verdict: GO for the draft-stage Consistency axis.** The two original P2 findings are closed. The other remediation changes remain internally coherent and introduce no new concrete Consistency finding or architectural root. This verdict does not assert implementation acceptance, schema freeze, library qualification, or executed tests.

## Prior-finding closure

| Finding | Corrected locations and counterexample result | Status |
|---|---|---|
| **P2-1 — Missing pre-effect endpoint capability binding** | `GwzRemoteTransportRequirements.md:161–167` requires session-bound Bind/Bound before Open and separates endpoint negotiation from core capabilities. `GwzRemoteTransportDesign.md:86–118` defines the exchange, version/scheme/policy intersections, limit negotiation, acknowledgement, session invalidation and pre-allocation checks; lines `206–224` bind Open to that session and endpoint. Incompatible versions, SSH-only endpoints, missing bindings and stale sessions now refuse before lease, credential, helper or host effects. Payload limits select bounded minima. The acceptance case is at line `641`. | **CLOSED** |
| **P2-2 — Missing disposition for current schemes and SSH aliases** | `GwzRemoteTransportRequirements.md:12–20,138–145,208–212` narrows the programme to SSH/HTTPS, retains local HTTP/git behavior and prohibits unsupported-route fallthrough. `GwzRemoteTransportDesign.md:14–16,150–173` covers SCP, `ssh://`, `ssh+git://`, `git+ssh://`, HTTPS, HTTP, git, file/local and unknown schemes. SSH aliases canonicalize to `ssh`; explicit nonlocal HTTP/git requests return `UnsupportedOperation` before core socket/helper access. The acceptance case is at line `642`. | **CLOSED** |

The filed original Consistency report at root `3a0b8fa975bc013673ee919b087f69da9f3853ff` matches my returned testimony; no transcription discrepancy was found.

## Changed-range consistency analysis

| Changed area | Analysis | Result |
|---|---|---|
| Credential-free destinations | Requirements G1/G4 at `GwzRemoteTransportRequirements.md:138–158` name HTTPS userinfo/query/fragment refusal as an intentional compatibility change. Design §§3.2–3.3 at `150–195` validates the effective route before Open and defines a structured destination with no HTTP credential, query, fragment or raw-URL field. HTTPS adapter rules at `548–578` obtain authority only through endpoint-local `gh` and apply the same validation to redirects. This agrees with the route table, no-fallback policy and authoritative amendments. | Coherent |
| Per-remote native binding | G7 and D3 at `GwzRemoteTransportRequirements.md:169–171,295` prohibit process-global interception. `GwzRemoteTransportDesign.md:507–528` consistently uses the per-remote callback, owns operation context, preserves foreign transports, and treats the missing safe git2 binding as a delivery prerequisite. Lines `521–523` keep the old implementation as the baseline until that prerequisite exists, matching the rule against advertising unqualified support. The route table’s native local HTTP/git exceptions remain possible because endpoint callbacks are installed only for the applicable GWZ remote operation. | Coherent |
| Graceful close | S4/S5 at `GwzRemoteTransportRequirements.md:248–257`, the message inventory at `GwzRemoteTransportDesign.md:212–224`, and lifecycle rules at `350–401` all require pending Data, one final EndWrite, then Close with the same offset. Close begins its deadline immediately, handles exhausted credit deterministically, drains unread reverse traffic within bounded storage, reports discarded data, and never establishes Git success. Cancel retains distinct abandonment semantics. These rules agree with HTTP body completion, half-close, connection reuse and no-replay requirements. | Coherent |
| Framing and decode bounds | S9 at `GwzRemoteTransportRequirements.md:267–271` and design §4.2 at `GwzRemoteTransportDesign.md:257–295` place limits before body allocation and recursive decoding. Bootstrap has fixed pre-negotiation caps; negotiated stream values cannot enlarge local hard limits. Encoded, scratch, decoded and queued storage share the aggregate budget, with in-process admission parity. Stream-local violations and malformed carrier envelopes have distinct failure scopes consistent with the existing lifecycle. | Coherent |
| Authority and acceptance coverage | `GWZRequirements.md:40–50` and `GWZDesign.md:42–52` carry all remediation outcomes into the authoritative documents while leaving detailed transitions in the companion design. The expanded matrix at `GwzRemoteTransportDesign.md:628–645` covers binding, routing, credentials, callback isolation, lifecycle and ingress bounds without claiming execution. No amended clause conflicts with retained D1–D15, endpoint trust, observation isolation, pooling, or the frozen lossy taut stream boundary. | Coherent |

## Findings

No new P0, P1, P2 or P3 Consistency findings. No new architectural root was identified.

## Immutable evidence verification

| Object | Commit | Tree |
|---|---|---|
| Root remediation inputs | `3a0b8fa975bc013673ee919b087f69da9f3853ff` | `445c63ee7e6a692b8453b297ea94265437dbbddc` |
| Original gwz-core object | `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` | `a2f7e28b2d9fbc0a7b61f54675c4835dc03d6f17` |
| Revised gwz-core object | `05842b38e55f109ed3663555680751811a72eb9b` | `226102463b82fd1d334663245df78fe1a00cd963` |

The pins resolved identically before and after review. Root HEAD equalled the remediation-input pin and gwz-core HEAD equalled the revised-object pin at the final check. The bounded diff contained 244 insertions and 30 deletions across only the four named documents.

No current-round Safety prompt or report was read. No build, schema generation, experiment, or implementation test was run or claimed.
