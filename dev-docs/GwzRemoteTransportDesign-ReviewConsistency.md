# GWZ Remote Transport four-document draft — CONSISTENCY-AXIS REVIEW

**Review object:** The four-document change from gwz-core `c2ca71b7595a0ce332f6f7f4c47db9d0c62ac230` to `3d2cc92c4a233e40ed049c8fe79bc555d39b272f`: `dev-docs/GwzRemoteTransportRequirements.md`, `dev-docs/GwzRemoteTransportDesign.md`, `dev-docs/GWZRequirements.md`, and `dev-docs/GWZDesign.md`; draft-stage design, implementation pending, dated 2026-09-19.  
**Baseline:** Root `7971d6f447692a70d79c9cc09f0bf5f3c5a9d4fe`; gwz-core `3d2cc92c4a233e40ed049c8fe79bc555d39b272f`; gwz-cli `07ad6620f2e26a2b8d70ae0cb8808dd5584058e1`; gwz-py `9c9ac1b5c5bbd800e8454817b99f107cc74d25d2`; taut-shape `74f375c9d3521f3e98110862dcf89ec64a3b6d6c`; taut-shape-rs `df1303656583121e043f7a925c02958571951735`. Repository sources were read only with `git show PIN:path` and searched with `git grep PIN`; pinned git2/libgit2 sources were read from the installed versions named by gwz-core’s pinned `Cargo.lock`.  
**Date:** 2026-09-19  
**Axis:** Consistency — internal coherence, agreement with controlling contracts, supersession exactness, satisfiable evidence, and unstated graph impacts. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — zero P0, zero P1, two P2, and zero P3 findings. The two missing protocol dispositions leave capability negotiation and existing network URL handling underdetermined. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

The start and end checks resolved every requested pin to the same immutable commit and tree:

| Repository | Commit | Tree |
|---|---|---|
| gwz-dev root | `7971d6f447692a70d79c9cc09f0bf5f3c5a9d4fe` | `aecec17e3d22529930afd7ac38f80206a03bf9db` |
| gwz-core | `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` | `a2f7e28b2d9fbc0a7b61f54675c4835dc03d6f17` |
| gwz-cli | `07ad6620f2e26a2b8d70ae0cb8808dd5584058e1` | `91690f211085c46afdbca4876d48cb9f964bc986` |
| gwz-py | `9c9ac1b5c5bbd800e8454817b99f107cc74d25d2` | `9dcfd015bc9ec5a447b439c6fb93c4a8a9eb73a1` |
| taut-shape | `74f375c9d3521f3e98110862dcf89ec64a3b6d6c` | `a094d35624c46307f5441376eb2053503dc451b9` |
| taut-shape-rs | `df1303656583121e043f7a925c02958571951735` | `fe39f42326b572b761058d47f8a185ded8acdc7c` |

All six current HEADs equalled their pins at the start. Other lanes advanced four HEADs during this review: root ended at `be5278a0c297a070d30ebda8e1f99cbfdafe3a3c`, gwz-core at `d6503d95e4092cdf378ff7d07f45f7d0f251e6c9`, gwz-cli at `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`, and gwz-py at `d07d55dacb1725d9306be9c04d157ac29a78e000`; taut-shape and taut-shape-rs remained at their pins. This movement did not alter or replace the immutable review objects, which re-resolved to the same commits and trees at the end.

The review read:

- The complete canonical Consistency prompt, review-loop skill and canonical template; root `AGENTS_GWZ.md`; pinned gwz-core `AGENTS.md`; and the relevant authority in root `CurrentProgramCheckpoint.md`, `AgentProcessRules.md` (especially L1-07 through L1-09, L1-16 through L1-20, L1-24, L2-03 through L2-05, L3-05 through L3-06, and §§7–9), and `GwzProcessOptimization.md`.
- The full pinned transport requirements, lines 1–293, and transport design, lines 1–536.
- The changed authoritative amendments in `GWZRequirements.md`, lines 1–38, plus REQ-011 through REQ-013 at lines 189–212 and REQ-124 at lines 994–997.
- The changed amendment and retained transport clauses in `GWZDesign.md`, lines 1–40 and 139–241.
- The full prior version of `GwzRemoteTransportRequirements.md` at `c2ca71b7`, to verify which operation-lifetime, lending, helper, placement, and identity-path clauses were replaced.
- Root `GwzRemoteAuthProposal.md`, including the no-Git-CLI ruling at lines 8–10 and §6 at lines 275–278; root `GwzUrlSchemePushPlan.md` transport dependencies; and gwz-cli `GwzFetchPlan.md` Phase 3 at lines 356–371.
- The full pinned taut stream decision. Its accepted contract is a bounded lossy ring with slow-reader drop and late-join loss (`TautShapeStreamDecision.md`, lines 8–18 and 20–52), agreeing with the transport design’s refusal to reinterpret that shape (`GwzRemoteTransportDesign.md`, lines 162–170).
- The pinned GWZ schema around `OperationAttribution`, `OperationPolicy`, existing transport capabilities, observations, transport options, invocation context, and request/response metadata (`protocol/gwz.taut.py`, lines 994–1136), plus the existing capabilities handler (`src/protocol/transport_capabilities.rs`, lines 1–37).
- Current native transport selection, authentication, timeout, observations, operation entry points, and host scheduler at the gwz-core pin, including `transport_support.rs`, `transport_support/identity.rs`, `transport_observations.rs`, `backend.rs`, `transport.rs`, `par_map_per_host.rs`, and `resolve_per_host.rs`.
- Pinned git2 0.21.0 `src/transport.rs`, including process-wide registration at lines 103–121, stateful/RPC smart transports at lines 124–188, stream continuation at lines 232–280, and `write_all` at lines 336–355.
- Pinned libgit2 1.9.7 transport lookup and scheme definitions, `libgit2-sys` backend feature selection, SSH session lifecycle/trust/authentication, and HTTP keepalive implementation.

The exact four-document diff was confirmed as 808 insertions and 244 deletions, touching only the four named files. No tests or builds were run, as required for this docs-only gate. No peer prompt or current-round peer report was read.

## 1. Findings

### [P2-1] The carried endpoint has no defined pre-effect capability-binding exchange

**Location:** `GwzRemoteTransportRequirements.md` G6 at lines 153–156, P1 at lines 193–195, D8 and D13 at lines 274 and 279, and the assignment of message inventory to the design at lines 283–289; `GwzRemoteTransportDesign.md` §3 at lines 63–81, §4 at lines 113–140, §10 at lines 424–436, and the compatibility/reporting matrix row at line 482.

**Violated invariant:** A selected nonlocal endpoint must be bound to its carrier session and its exact protocol, placement, transport, authentication-policy, and limit capabilities must be known before any network effect. All public payloads must be taut-defined, and an unavailable or unsupported explicit route must refuse without fallback.

**Reproduction/state sequence:** Start a new core and a gwz-cli endpoint on a real driver–core channel. Let the CLI endpoint support only SSH or an older conversation version, then submit an explicit CLI-placement HTTPS request. The mux must decide before `Open` whether the selected endpoint supports the route. The design’s complete conversation inventory contains only `Open`, `Opened`, `OpenFailed`, data/flow-control, and terminal messages. `Open` contains the requested operation but no endpoint capability advertisement, while `Opened` is sent only after lease acquisition and service setup succeed. The existing pinned `TransportCapabilitiesResponse` is a core-side response containing only `file_identity` and `exact_agent_identity`; it neither advertises a driver endpoint nor binds advertised facts to a carrier session. Section 10 says to extend and negotiate capabilities, but does not define who sends endpoint capabilities, when, how they attach to the session/binding, or what invalidates them after reconnect.

An implementation must therefore choose among three incompatible behaviours: send `Open` speculatively and discover refusal after setup, invent an unlisted out-of-band handshake, or trust unbound driver claims. The first can violate the before-effects rule; the second violates the single taut contract/message-inventory rule; the third cannot satisfy session isolation or old/new negotiation. The same omission makes `Opened`’s “negotiated maximum payload” underdetermined because the endpoint has no specified pre-open source for the initiator’s supported limit beyond its receive-window value.

**Impact:** Separate-process implementations can disagree while each follows parts of the draft. Explicit placement can cause effects before unsupported status is known, bind stale capabilities after a carrier reconnect, or fall back to local execution contrary to D8. The old/new capability acceptance row has no single protocol trace that a test can assert.

**Required correction:** Add a normative endpoint-binding and capability exchange to the design’s taut inventory, or explicitly define an equivalent taut-defined binding record installed before requests. Specify direction, carrier-session association, supported conversation versions, placements/transports/authentication policy, relevant stream limits, successful acknowledgement, invalidation on disconnect/reconnect, and the exact pre-`Open` refusal rule. Reconcile that exchange with the external `transport_capabilities` service so core capability and selected-endpoint capability cannot be confused.

**Closure/regression test:** A real separate-process carrier matrix must cover compatible and incompatible conversation versions; SSH-only versus HTTPS requests; missing driver binding; stale capabilities after carrier-session replacement; a deliberately unsupported explicit route with a fake network adapter proving zero connect/auth calls; negotiated payload-limit selection; and an ordinary old-driver/new-core local request. Each case must assert the same typed result in the in-process and serialized forms and prove that no local fallback occurs.

### [P2-2] The “all Git network work” contract has no disposition for existing network schemes and SSH aliases

**Location:** `GwzRemoteTransportRequirements.md` purpose at lines 12–18, G1 at lines 136–140, and G4 at lines 147–150; `GwzRemoteTransportDesign.md` outcome at lines 13–22, SSH integration at lines 340–388, HTTPS integration at lines 390–420, and acceptance matrix at lines 469–484. The authoritative core amendment is narrower: `GWZRequirements.md` lines 12–16 requires endpoint access for SSH/HTTPS.

**Violated invariant:** Every currently valid network URL must either route through the selected endpoint under the promised mux contract or have an explicit compatibility disposition. A selected driver route must never silently execute through core’s native transport, and G1’s preserved outcomes may be narrowed only by a named intentional change.

**Reproduction/state sequence:** The pinned native backend explicitly treats `git://`, `http://`, `https://`, `ssh://`, `ssh+git://`, and `git+ssh://` as smart network transports (`src/git/gitbackend/transport.rs`, lines 591–613), and its tests execute a real `git://` push (`src/git/tests/g08.rs`, lines 223–242). Pinned libgit2 likewise has separate built-in definitions for `git`, `http`, `https`, `ssh`, `ssh+git`, and `git+ssh`; custom transport lookup is prefix-specific, with SCP-style URLs re-resolved through the `ssh` registration.

The draft says all Git network work passes through the mux, but defines only SSH and HTTPS endpoint adapters and gives no exemption, refusal, or retained-native rule for `git://` and `http://`. It also does not enumerate the separate `ssh+git` and `git+ssh` registrations or test them. Following the document permits incompatible outcomes:

1. Leave `git://`/`http://` or an SSH alias on libgit2’s built-in path, bypassing mux placement, endpoint trust/traffic ownership, pooling, observations, and explicit-route pinning.
2. Reject those URLs, breaking current outcomes without listing that break among G1’s intentional changes.
3. Invent unreviewed endpoint adapters and authentication rules.
4. Register only `ssh`, causing SCP and `ssh://` to use the endpoint while `ssh+git://` and `git+ssh://` silently remain in core.

For an explicit CLI endpoint request using an uncovered SSH alias, outcome 1 is especially contradictory: traffic and ambient authority run at core even though P1/D8 require the selected endpoint to remain pinned with no fallback.

**Impact:** Existing repositories can silently bypass the new endpoint or suffer an undocumented compatibility break. Pool counts, endpoint observations, trust ownership, and driver-only reachability become URL-spelling dependent. The acceptance matrix can pass for `ssh://` while the same SSH operation under another libgit2-supported spelling violates the architecture.

**Required correction:** Add an exhaustive route table for every currently admitted smart-network family. At minimum, define handling for SCP syntax, `ssh://`, `ssh+git://`, `git+ssh://`, `https://`, `http://`, and `git://`, while retaining the explicit file/local exclusion. Canonicalize all SSH spellings to the SSH pool scheme where intended. For `http://` and `git://`, either define endpoint adapters or state the preserved local-only/unsupported-placement behaviour and narrow the “all network work” claim accordingly. State the exact typed refusal for an explicit placement that a retained-native scheme cannot satisfy. Add the corresponding prefix-registration and bypass-proof cases to the acceptance matrix.

**Closure/regression test:** For each admitted spelling, use a fake registered transport plus connection counters to assert the selected endpoint, physical-connect count, pool key, observation endpoint, and absence of built-in fallback. Include a separate-process CLI-placement case for each SSH spelling, a retained current `git://` outcome if that scheme remains supported, and unsupported-placement cases proving refusal before any core-side socket or credential access.

## 2. Invariant analysis

The following attacks held:

- **D1–D15 alignment:** Endpoint-owned lifetime, 60-second active reaping, one active exchange per connection, pool grouping without repository identity, explicit-key compatibility, local default, route pinning, endpoint-owned trust and `gh`, and the stated peer/signing/config deferrals agree across the requirements, transport design, and two authoritative amendments.
- **Supersession:** The amendments identify the three material changes to the retained baseline: cross-operation authenticated connection reuse, endpoint-local path interpretation under remote placement, and the `gh`-only HTTPS restriction. The older operation-scoped observation and selected-identity isolation clauses remain compatible because the design copies only proven connection facts with explicit reuse context, not an earlier operation’s observation row or credential material.
- **Byte and ordering model:** `Data` offsets, absolute byte windows, independent direction credit, finite byte/message budgets, first-byte timer, full-buffer eligibility, explicit flush barriers, `EndWrite`, and no automatic replay form a coherent bounded model. The design correctly notes that git2 calls `write_all` rather than `flush`, requires independent timers, and makes a subsequent blocking read force pending writes eligible.
- **Half-close and teardown:** `EndWrite` preserves the reverse direction; `Close`, `Cancel`, `Failed`, final-handle drop, carrier loss, allocation races, and bounded cleanup are distinguished. Carrier loss is correctly treated as a runtime event rather than a peer message.
- **Taut stream boundary:** The draft does not falsely claim that the accepted taut `stream.oracle/v1` shape is reliable. It preserves the frozen slow-reader-drop and late-join-loss contract and requires a separate reliable conversation adapter with flow control.
- **Pool mechanics:** Opening, idle, allocated, and closing resources count against capacity; connection establishment happens outside the state lock; incompatible idle entries can be retired; quiet allocated leases are not reaped; timer generations prevent stale expiry; operation fan-out remains distinct from endpoint capacity.
- **Identity and trust:** Current precedence is preserved while endpoint-local resolution is separated from selection. An unavailable or changed explicit file cannot be masked by a cached session, ambient requests cannot borrow explicit-override sessions, and host-key decisions stay at the executing endpoint.
- **libgit2 integration claims:** Installed pinned sources confirm custom registrations precede built-ins; SCP-style URLs consult the `ssh` registration; `rpc=false` preserves the advertisement stream into the natural upload/receive-pack progression; the write callback invokes `write_all`; current SSH streams own and free their session; and the proposed global factory needs synchronization. The document properly labels thread-local factory capture, concrete SSH/HTTP libraries, and platform/cancellation behaviour as qualification work rather than accepted facts.
- **Observability:** New versus reused sessions, endpoint/connection/stream correlation, and offered versus authenticated credential facts are additive and operation-associated. The legacy nullable `authenticated` value is not forced where proof is absent.
- **Acceptance scope:** The matrix meaningfully covers buffering, flow control, lifecycle races, carrier loss, pool capacity, idle expiry, path bases, trust, HTTPS parity, Git request varieties, context isolation, old/new combinations, platforms, and performance. Apart from the two missing protocol dispositions above, its entries are satisfiable as future tests rather than falsely reported execution.

## 3. Risks and next action

Concrete SSH/HTTP library qualification, numeric payload/window/queue/deadline choices, Windows agent parity, TLS/proxy/redirect behaviour, and the scoped thread-local factory mechanism remain declared delivery risks, not defects in this draft-stage review. In particular, process-wide registration can collide with or intercept unrelated git2 use in an embedding process; §8 and §12 acknowledge the global binding and require qualification, but the implementation plan should include an unbound/foreign-caller coexistence test before adopting that mechanism.

The next action is one bounded document revision: define the taut endpoint-binding/capability exchange and add the exhaustive current network-scheme route table with acceptance cases. Then return the same immutable four-document object for a focused Consistency re-verdict against P2-1 and P2-2.
