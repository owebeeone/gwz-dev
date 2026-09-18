# GWZ Remote Transport draft — SAFETY-AXIS REVIEW

**Review object:** Four-document change in gwz-core from `c2ca71b7595a0ce332f6f7f4c47db9d0c62ac230` to `3d2cc92c4a233e40ed049c8fe79bc555d39b272f`: `dev-docs/GwzRemoteTransportRequirements.md`, `dev-docs/GwzRemoteTransportDesign.md`, `dev-docs/GWZRequirements.md`, and `dev-docs/GWZDesign.md`; draft-stage transport design, dated 2026-09-19, with implementation pending.  
**Baseline:** root `7971d6f447692a70d79c9cc09f0bf5f3c5a9d4fe`; gwz-core `3d2cc92c4a233e40ed049c8fe79bc555d39b272f`; gwz-cli `07ad6620f2e26a2b8d70ae0cb8808dd5584058e1`; gwz-py `9c9ac1b5c5bbd800e8454817b99f107cc74d25d2`; taut-shape `74f375c9d3521f3e98110862dcf89ec64a3b6d6c`; taut-shape-rs `df1303656583121e043f7a925c02958571951735`. Repository sources were read from immutable objects with `git show PIN:path` and `git grep PIN`; installed git2 0.21.0 source was read directly.  
**Date:** 2026-09-19  
**Axis:** Safety — degraded and mixed-version paths, irreversible and uncertain effects, disclosure, resource bounds, stuck states, and process-wide blast radius. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P1 and three P2 findings block. I pre-commit to GO on a revision that resolves P1-1 and P2-1 through P2-3 as specified.

---

## 0. Evidence base

The exact tuple was verified at both the beginning and end with `git -C REPO rev-parse PIN^{commit} PIN^{tree}`. Both checks returned these unchanged pairs:

| Repository | Commit | Tree |
|---|---|---|
| gwz-dev | `7971d6f447692a70d79c9cc09f0bf5f3c5a9d4fe` | `aecec17e3d22529930afd7ac38f80206a03bf9db` |
| gwz-core | `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` | `a2f7e28b2d9fbc0a7b61f54675c4835dc03d6f17` |
| gwz-cli | `07ad6620f2e26a2b8d70ae0cb8808dd5584058e1` | `91690f211085c46afdbca4876d48cb9f964bc986` |
| gwz-py | `9c9ac1b5c5bbd800e8454817b99f107cc74d25d2` | `9dcfd015bc9ec5a447b439c6fb93c4a8a9eb73a1` |
| taut-shape | `74f375c9d3521f3e98110862dcf89ec64a3b6d6c` | `a094d35624c46307f5441376eb2053503dc451b9` |
| taut-shape-rs | `df1303656583121e043f7a925c02958571951735` | `fe39f42326b572b761058d47f8a185ded8acdc7c` |

Current HEADs were recorded separately and were not substituted for the pins. Samples taken during the review and at its end showed no movement: root `be5278a0c297a070d30ebda8e1f99cbfdafe3a3c`, gwz-core `d6503d95e4092cdf378ff7d07f45f7d0f251e6c9`, gwz-cli `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`, gwz-py `d07d55dacb1725d9306be9c04d157ac29a78e000`, taut-shape `74f375c9d3521f3e98110862dcf89ec64a3b6d6c`, and taut-shape-rs `df1303656583121e043f7a925c02958571951735`. These later HEADs were outside scope.

The review read:

- The canonical Safety prompt, review-loop skill/template, pinned root `AGENTS_GWZ.md`, gwz-core `AGENTS.md`, and the relevant independent-review/severity rules in `AgentProcessRules.md:350-455`, plus `GwzProcessOptimization.md`.
- The complete pinned transport requirements, `GwzRemoteTransportRequirements.md:1-293`, including G1-G6, C1-C8, P1-P9, S1-S8, D1-D15, and the declared deferrals.
- The complete controlling transport design, `GwzRemoteTransportDesign.md:1-536`, with adversarial traces through endpoint binding, messages, flow control, lifecycle, pool allocation/reaping, SSH, HTTPS, compatibility, deadlines, and its acceptance matrix.
- The complete four-document diff via `git diff c2ca71b... 3d2cc92... -- dev-docs/...`.
- The amendments and relevant older clauses in `GWZRequirements.md:1-69,90-220,970-1020,1146-1163` and `GWZDesign.md:1-40,130-241,943-1036,2229-2246`.
- Root `GwzRemoteAuthProposal.md:1-129`, particularly its wrong-account sequence and explicit-identity boundary.
- Pinned `TautShapeStreamDecision.md:1-60`, confirming that the shipped Taut stream is deliberately lossy and cannot itself supply this contract.
- Pinned GWZ protocol and transport evidence: `protocol/gwz.taut.py:1-30,985-1110`; `transport_support.rs:1-263`; `transport_observations.rs:1-140`; `url_scheme.rs:90-180,210-285,390-455`; and `cbor.rs:455-610`.
- Installed pinned git2 0.21.0 `src/transport.rs:1-355`, including the process-wide unsafe `register`, factory invocation, stateful smart transport, and `write_all` callback behavior.

No tests or builds were run because the canonical prompt forbids them for this documentation-only gate. No working-tree file, index, ref, or review artifact was modified. No peer prompt, report, or current-round output was read.

## 1. Findings

### [P1-1] Credential-bearing HTTPS URLs can bypass the `gh`-only boundary and cross the carrier

**Location.** `GwzRemoteTransportDesign.md:127-151` makes `Open` carry the captured destination/effective URL and says only diagnostics redact credential-bearing URLs. `GwzRemoteTransportDesign.md:390-420` constructs endpoint HTTP requests from that descriptor and requires `gh`, but never rejects or removes URL userinfo. This conflicts with the accepted `gh`-only rule in `GwzRemoteTransportRequirements.md:147-150,212-213`. Supporting pinned source confirms that HTTPS URLs explicitly admit `[user[:password]@]` (`src/git/url_scheme.rs:233-240`), that an already selected form is retained verbatim (`:130-152`), and that `https://user@HOST/...` is tested as unchanged (`:390-417`).

**Violated invariant.** Authenticated HTTPS must use only endpoint-local `gh`; credential material must not enter generic transport control messages or be accepted from another provider.

**Reproduction.**

1. A manifest or captured remote URL is `https://user:token@github.com/o/r.git`.
2. Existing URL resolution recognizes it as HTTPS and preserves the effective URL.
3. Core places that effective URL in `Open`, including over the driver carrier.
4. The endpoint derives its HTTP request from the descriptor. The draft does not require rejection or stripping of userinfo before the request reaches the HTTP library.
5. A conforming implementation may therefore use URL Basic credentials, or at minimum transport the token through the generic message runtime, while still claiming the `gh`-only policy. Redacting later diagnostics does not remove the credential from `Open`.

**Impact.** The contract permits credential exposure outside the HTTP authentication adapter and permits a non-`gh` credential to authenticate a request. Redirect and error paths increase the disclosure surface. This defeats the accepted authentication boundary rather than merely leaving a library choice open.

**Required correction.** Define a credential-free transport descriptor. Before any `Open`, helper invocation, carrier write, or network effect, reject HTTPS destinations containing userinfo/password/token material with a typed redacted error. The endpoint must construct requests only from separately validated scheme, host, effective port, and path fields, and must never infer authentication from URL userinfo. Apply the same rule to redirected destinations before resolving endpoint-local `gh` authority.

**Closure test.** Cover password-bearing and username-bearing HTTPS URLs through local and carried endpoints, including redirect and error rendering. Assert refusal before network/helper activity, assert encoded messages and diagnostics contain no sentinel secret, and assert ordinary anonymous and endpoint-local `gh` cases still work.

### [P2-1] Process-wide transport registration has no coexistence or external-synchronization boundary

**Location.** `GwzRemoteTransportDesign.md:360-370` proposes process-wide transport registration, protects only runtime initialization, stores operation context in a thread-local, and explicitly makes unknown or unbound factory calls fail. Pinned git2 0.21.0 documents `transport::register` as unsafe because registration must be externally synchronized with other transport creation (`transport.rs:103-121`); the registered global factory is invoked for matching remotes (`:199-229`). The overall requirements still require core to remain independent of a particular application runtime and usable as a standalone library (`GWZRequirements.md:90-99,189-205`).

**Violated invariant.** Installing GWZ transport support must not corrupt, race, or commandeer unrelated Git activity in the embedding process.

**Reproduction.**

1. An embedding application already uses libgit2 directly, or has installed another custom SSH/HTTPS transport.
2. A thread begins creating an unrelated transport while GWZ initializes its endpoint and calls process-wide `register`. GWZ's internal initialization lock cannot externally synchronize the other caller, so it cannot satisfy the unsafe API's precondition.
3. After registration, an unrelated matching remote creation reaches the GWZ factory without GWZ's thread-local context.
4. The proposed rule deliberately fails that unbound call. Depending on prefix/collision behavior, an earlier custom registration may instead be rejected or displaced.

**Impact.** Enabling GWZ can break unrelated network operations for the whole host process and permits a race at an unsafe process-global registry. Documenting the impact for embedders does not provide an ownership or recovery rule.

**Required correction.** Choose and specify an isolation mechanism before implementation. Prefer per-remote binding or GWZ-only synthetic schemes that never capture unrelated standard URLs. If standard process-wide prefixes are unavoidable, the design must define singleton ownership, existing-registration detection, external synchronization requirements, registration lifetime/restoration, and delegation for unbound calls; it must refuse initialization safely when coexistence cannot be proved. “Unbound calls fail” cannot be the ordinary coexistence behavior.

**Closure test.** In one process, run a pre-existing custom transport and ordinary non-GWZ libgit2 SSH/HTTPS operations before, concurrently with, and after GWZ runtime construction and teardown. Verify no race, replacement, rerouting, or failure. Concurrent GWZ operations with different endpoint/identity contexts must remain isolated.

### [P2-2] `Close` is legal from `Active` even though it neither requires nor performs `EndWrite`

**Location.** The message inventory separates `EndWrite` from `Close` (`GwzRemoteTransportDesign.md:127-139`). Flush explicitly does not end an HTTP request body; only `EndWrite` does (`:210-217`). Nevertheless, the lifecycle table permits graceful `Close` from `Active/half-closed` and specifies only flushing pending writes before cleanup (`:225-235`). The HTTP mapping again requires `EndWrite` to terminate the request body (`:398-404`). Requirements S4-S5 require distinct, coherent end-write and graceful-close semantics (`GwzRemoteTransportRequirements.md:231-237`).

**Violated invariant.** Every legal graceful-close transition must have enough information to finish both protocol directions without an avoidable cleanup timeout, truncation, or implicit reinterpretation of another lifecycle message.

**Reproduction.**

1. An HTTP exchange is `Active`; request-body bytes are buffered or have been delivered, but no `EndWrite` has been sent.
2. The initiator sends `Close`, which is legal from `Active`.
3. The endpoint flushes the bytes and enters cleanup.
4. Per the draft's own rule, flush does not terminate the HTTP body. The server can keep waiting for body completion while the endpoint waits for cleanup, until the close deadline discards the connection and reports failure.
5. If an implementation instead treats `Close` as implicit EOF, it collapses the separately specified `EndWrite` transition and leaves its final-offset ordering unstated.

The same ambiguity exists when `Close` is sent while reverse traffic remains active: the draft does not state whether response data must be delivered, drained only for reuse, or abandoned.

**Impact.** A message sequence explicitly admitted by the state table cannot complete gracefully. Implementations can diverge between deterministic failure, implicit half-close, or response truncation.

**Required correction.** Make one rule normative: either reject graceful `Close` until the initiator has sent `EndWrite` and the required reverse-direction completion condition holds, or define `Close` to enqueue an ordered `EndWrite(final_offset)` before cleanup. Specify how unread reverse data is handled and when `Closed` may be reported. Early abandonment remains `Cancel`.

**Closure test.** Exercise `Close` from active, local-half-closed, remote-half-closed, and fully ended states, with pending partial data, exhausted credit, unread reverse data, and racing `Cancel`. Each case must have one typed outcome, release the lease exactly once, preserve final offsets, and never wait beyond the bounded cleanup deadline.

### [P2-3] Payload and aggregate limits are enforceable only after an unbounded serialized message has already been accepted and decoded

**Location.** The draft says oversized messages fail the conversation (`GwzRemoteTransportDesign.md:153-170`) and bounds outgoing buffers, receive windows, message counts, and aggregate carrier storage (`:172-208`). It does not define an encoded-frame limit or require size/depth checks before allocation and generic Taut decoding. Supporting pinned GWZ codec behavior shows why that boundary matters: CBOR `BYTES` and text are copied into new allocations (`src/cbor.rs:541-552`), while arrays/maps recursively allocate before generated typed-field validation (`:554-584`).

**Violated invariant.** Bounded backpressure must bound memory at the first carrier ingress, including malformed and oversized messages, rather than only after a `Data` value exists.

**Reproduction.**

1. A broken or adversarial carried endpoint sends one syntactically valid envelope whose encoded `Data` payload exceeds the negotiated maximum, or sends deeply nested/large unknown CBOR collections.
2. Without a pre-frame bound, the carrier first accepts or buffers the entire encoded message.
3. Generic decoding then clones byte/text data and recursively allocates collections.
4. Only after that work can the typed conversation layer classify the message as oversized or malformed.
5. The nominal payload/window/queue limits therefore do not bound peak memory; one frame can exceed them and can exhaust the process before the specified typed failure occurs.

The same gap applies to unbounded `Open` metadata and diagnostic strings, not only `Data`.

**Impact.** A carrier peer can exhaust memory or stack and terminate the endpoint/core process, taking unrelated streams with it. This defeats the design's bounded-buffer and per-conversation-failure claims.

**Required correction.** Specify a maximum encoded frame size, bounded metadata/string/collection sizes, and a decoder depth limit. Enforce the frame bound before reading or allocating the complete frame, and charge simultaneous encoded and decoded storage to the aggregate carrier budget. The in-process adapter must enforce equivalent limits. An oversized frame must produce a bounded typed stream/carrier failure without first allocating its declared content.

**Closure test.** Feed a valid `Data` frame just over the negotiated limit, a huge declared-but-truncated frame, oversized metadata, deeply nested unknown values, and a storm of minimum-size messages. Measure/assert bounded peak storage and recursion, deterministic typed failure, waiter wake-up, and cleanup of every affected lease while unrelated conversations retain their specified behavior.

## 2. Invariant analysis

The following adversarial attacks held:

- **No silent route fallback.** Binding and capability checks occur before open; missing explicit placement refuses; the session and endpoint remain pinned through terminal (`GwzRemoteTransportDesign.md:61-81`). Old/new capability rules treat missing fields as unsupported and explicitly refuse a driver endpoint request that cannot be honored (`:422-436`).
- **No transparent replay after a possible remote effect.** Idle-stale replacement is confined to the interval before the Git service request could reach the host. After that boundary, failures surface without replay; lost push responses remain uncertain and `Closed`/`Flushed` are explicitly not publication proof (`:264-268,412-420`).
- **Cancellation and carrier loss release ownership.** Opening cancellation removes queue/reservation state, the single lease is released exactly once across races, terminal streams cannot reopen, and carrier death wakes blocked callers and tears down owned allocations (`:225-262`). Session identifiers prevent old frames from releasing a new stream.
- **Backpressure after successful decode is coherent.** Both directions have independent absolute byte credit; credit returns only after consumption or transfer into another bounded sink; message count/overhead is included; and control capacity is reserved so cancellation and window updates are not blocked behind data (`:172-217`). The failure is specifically the earlier framing/decode boundary in P2-3.
- **Publication acknowledgements are not overstated.** Flush acknowledges delivery only to the receiving sink, and `Closed` is a transport result rather than a Git result (`:127-151,210-217,264-268`).
- **Pool accounting is fail closed.** Opening, idle, allocated, and closing connections all consume capacity; connection creation occurs outside the pool lock under a reservation; setup failure/cancellation releases it; incompatible idle entries can be retired; active quiet leases are not reaped (`:297-325`). Idle generations prevent stale expiry from closing a checked-out connection, and shutdown is bounded (`:327-338`).
- **Explicit identity reuse does not silently substitute another key.** Explicit selections are resolved before reuse, must match proven authentication identity, and changed/unavailable selected files refuse even if an older session remains cached. Ambient requests cannot borrow a connection created for an unrelated explicit override (`:270-295`). This respects D4 without adding a repository component.
- **HTTPS replay and redirect hazards are recognized after credentials are validly acquired.** The design requires different-origin redirect isolation, re-resolution of authentication, streaming bodies, and no automatic replay once publication may have begun (`:390-420`). P1-1 is the missing ingress rule for credential-bearing URLs.
- **The shipped lossy Taut stream is not falsely claimed as reliable.** The draft identifies its slow-reader-drop and late-join semantics and requires a separate reliable conversation adapter with flow control and loss-as-terminal conformance (`:153-170`), matching pinned `TautShapeStreamDecision.md:8-18,20-52`.
- **Deferred work remains bounded.** Concrete SSH/HTTP library qualification, numeric payload/window/deadline tuning, peer networking, endpoint permissions, forwarded signing, portable identity naming, multi-channel SSH, and OpenSSH configuration parsing are clearly deferred without being presented as implemented evidence.

The failed attacks are P1-1 and P2-1 through P2-3: the credential boundary can be bypassed before endpoint authentication policy applies; process-global registration can affect unrelated operations; one admitted close transition lacks a completion rule; and wire bounds begin too late to guarantee bounded memory.

## 3. Risks and next action

Library qualification, platform parity, and numeric tuning remain legitimate draft-stage risks rather than findings. The implementation plan should also retain the existing private-member clone suppression invariant in `GWZRequirements.md:1146-1163` when adding endpoint/connection observation fields; the current core has an explicit suppression hook, so this review did not classify absence of a new transport-specific matrix row as a separate blocking root cause.

The next action is one bounded documentation revision that:

1. makes HTTPS destinations credential free before `Open`;
2. replaces or fully governs process-global transport registration;
3. closes the `EndWrite`/`Close` state-machine hole; and
4. moves serialized-frame/resource enforcement to pre-decode ingress.

Add the named closure cases to the acceptance matrix, commit the revised four-document object, verify a new exact tuple, and run focused Safety and Consistency re-verdicts before producing the TDD implementation plan.
