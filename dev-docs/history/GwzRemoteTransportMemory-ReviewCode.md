# GWZ REMOTE TRANSPORT IN-MEMORY TYPED-MESSAGE STREAM — CODE-AXIS REVIEW

**Review object:** Initial gwz-transport implementation at `89e9ebefee2598ababbbb227366ecbbd924b5cb5`, including all committed source, schema, scripts, tests and README; controlling checkpoint `gwz-core/dev-docs/GwzRemoteTransportMemoryImplementation.md` at `6a82c646fa9cff5555a182366bc3dd41707bd076`; implementation checkpoint, no interface/schema freeze, dated 2026-09-19.  
**Baseline:** gwz-transport `89e9ebefee2598ababbbb227366ecbbd924b5cb5`; gwz-core `6a82c646fa9cff5555a182366bc3dd41707bd076`; workspace root `3976ef2c046b333bcf0765db916004f8387d8259`. Committed sources were read with `git show PIN:path`; generated-artifact identity was checked through the pinned regeneration command.  
**Date:** 2026-09-19  
**Axis:** Code — architecture, interfaces, call graphs and compatibility reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — zero P0, zero P1, three P2 and one P3 findings. P2-1 through P2-3 block this implementation checkpoint. I pre-commit to GO on a revision that resolves P2-1, P2-2 and P2-3 as specified; P3-1 is bounded and may be closed in the same revision.

---

## 0. Evidence base

The start and end checks resolved the exact tuple unchanged:

| Repository | Commit | Tree |
|---|---|---|
| gwz-transport | `89e9ebefee2598ababbbb227366ecbbd924b5cb5` | `a2ae33738300ba1894920a9b2cd88cfbb51245e5` |
| gwz-core | `6a82c646fa9cff5555a182366bc3dd41707bd076` | `0fa0cfdfbb5f97607e442ef17ba3c54641395e6c` |
| workspace root | `3976ef2c046b333bcf0765db916004f8387d8259` | `87e5238dc3b3df61a33dc8cde1aab459657c4f36` |

All three HEADs equalled their pins at both checks. gwz-transport had no tracked worktree changes after validation.

The review read:

- The complete generated Code prompt, root agent instructions and the relevant process authority in `CurrentProgramCheckpoint.md`, `AgentProcessRules.md` and `GwzProcessOptimization.md`.
- The complete memory checkpoint; transport requirements §5.4; design §§4–6; and implementation-plan ownership, supplied-message-interface, Phase 1 and Phase 2 sections at the gwz-core pin.
- Every authored gwz-transport source module: admission, binding, budgets, codecs, stream state machine, incoming/outgoing transitions, asynchronous facade and crate exports.
- The canonical taut schema, generator pins and regeneration scripts; relevant generated protocol and CBOR paths; and the exported IR. Regeneration verified all four generated artifacts.
- The complete README and all deterministic, asynchronous, binding, protocol and Monte Carlo tests.
- Searches for physical I/O/dependencies, unsafe code, conditional compilation and unbraced control-flow hazards. The crate has no dependency, socket, pipe, filesystem, thread or executor implementation.

Commands run independently:

- `cargo test --manifest-path gwz-transport/Cargo.toml --locked`: 28 deterministic/default tests passed; the default 3,000-case Monte Carlo test passed; the 50,000-case campaign remained intentionally ignored.
- `cargo clippy --manifest-path gwz-transport/Cargo.toml --all-targets -- -D warnings`: passed.
- `gwz-core/protocol/.regen-venv/bin/python gwz-transport/scripts/regen.py --check`: four generated artifacts verified.
- Direct replay with `GWZ_TRANSPORT_MC_CASE_SEED=0x1234`: passed and reproduced.
- An additional 10,000-case walk with run seed `0x5eedc0de`: passed in 23.62 seconds with substantial blocked-write/read, partial-write, flush, tiny-window, delayed-delivery and data-message coverage.

The lane owner additionally reported a passing Rust 1.95 full suite, the pinned 50,000-case release campaign with seed `0x202609195eed`, direct replay, regeneration and clippy. The verdict does not rely on the excluded uncommitted core consumer. No current-round peer report was read.

## 1. Findings

### [P2-1] Active streams discard the negotiated admission policy and can emit or accept messages above it

**Location:** `src/stream/mod.rs:22–33` exposes only buffer, window and payload values in `Config`; `src/stream/incoming.rs:23–24` admits incoming messages through default `codec::admit`; `src/stream/machine.rs:224–249` accepts and clones arbitrary host-supplied `Facts` into a pending close; `src/stream/outgoing.rs:44–52` emits that value without admission. The limited entry point exists at `src/codec.rs:37–40` but is not connected to an active stream. This conflicts with the checkpoint’s claim at `GwzRemoteTransportMemoryImplementation.md:25–44` that negotiated limits are supplied at construction and that local delivery observes the same admitted Envelope contract.

**Violated invariant:** Every active stream must enforce its effective negotiated limits in both directions. Local typed delivery must accept and reject the same values as limited serialized delivery, and locally constructed messages must be checked before the crate copies or queues them.

**Reproduction/state sequence:**

1. Negotiate otherwise usable limits with `metadata_bytes = 256`.
2. Construct the initiator and endpoint streams. `Config` has no field carrying that metadata limit.
3. Complete the normal EndWrite/Close exchange until the endpoint may call `complete_close`.
4. Call `complete_close` with `Facts.key_fingerprint` containing 257 bytes.
5. `complete_close` accepts and clones the value. `next_message` emits the `Closed` Envelope.
6. The in-memory peer calls `deliver`; `receive` applies the default 16 KiB metadata limit and accepts the message. The same Envelope through `decode_limited` under the negotiated 256-byte limit is rejected.

A value above the default 16 KiB limit is still copied and emitted by the endpoint, then rejected only by its peer.

**Impact:** In-process and serialized forms disagree on accepted messages. Negotiated metadata/collection/allocation bounds do not govern the active typed stream, and a host-supplied terminal fact can cause avoidable copies above the agreed budget. A later integration can pass all binding checks yet violate the resulting session contract.

**Required correction:** Carry the effective admission policy or complete negotiated `Limits` in `Config`/`StreamMachine`. Use it for every incoming Envelope and admit every locally constructed Envelope containing caller-supplied data before committing state or copying it. In particular, validate `Facts` before storing `close_pending`. Retain the separately configured stream buffer/window values only where they intentionally narrow those negotiated maxima.

**Closure/regression test:** Establish a low but usable metadata limit and run the same valid and one-byte-excess `Closed` cases through direct typed delivery and codec round-trip delivery. Both paths must accept the boundary value, reject the excess before state completion, remain nonterminal for a locally rejected construction where appropriate, and perform no above-limit queued copy.

### [P2-2] Authentication capabilities and identity mode are admitted as independent fields even when their combination is impossible

**Location:** `protocol/transport.taut.py:8–13,25–30,38–41` defines flat scheme/policy sets plus a separate identity mode. `src/codec/validate.rs:90–112` rejects only part of the invalid cross-product. `src/binding.rs:110–126` intersects schemes and policies independently and treats two nonempty lists as a usable binding; `Binding::check_open` at lines 68–85 repeats independent membership checks.

**Violated invariant:** A successful binding must advertise at least one executable scheme/authentication combination, and an admitted Open must express one unambiguous authority policy before credentials or network effects.

**Reproduction/state sequence:**

1. A normal binding advertises HTTPS and `gh`.
2. Construct an HTTPS Open with `policy = Gh` and `identity.mode = CredentialsDisabled`.
3. `codec::admit` accepts it because HTTPS rejects only `ExplicitKey`; `Binding::check_open` also accepts it because HTTPS and `gh` are independently present.
4. A future adapter must either invoke `gh`, violating credentials-disabled intent, or avoid it, violating the selected `Gh` policy.

Independently, an `EndpointConfig` containing `schemes = [Ssh]` and `policies = [Gh]` successfully returns `Bound`, although no Open using that advertised capability set can pass the existing Open validator.

**Impact:** Binding can report readiness for an unusable capability set, and a credential-disabled request can cross the pre-effect admission boundary with an instruction to use credentials. Implementations must invent precedence between contradictory fields.

**Required correction:** Define one central compatibility relation covering scheme, authentication policy and identity mode. Apply it to EndpointConfig validation, Bound validation and Open admission. Reject a binding with no executable combination. Either represent supported pairs directly or ensure the flat lists cannot advertise a false Cartesian product.

**Closure/regression test:** Exhaustively test the Scheme × AuthPolicy × IdentityMode matrix. Include successful SSH ambient, SSH explicit-key, HTTPS anonymous and HTTPS `gh` cases, plus every cross-family or credentials-disabled contradiction. An endpoint with no valid combination must reject Bind with `Effect::None`.

### [P2-3] The stream API erases typed peer failure code and effect at the delivery boundary

**Location:** The schema retains `Failure { code, effect }` at `protocol/transport.taut.py:31,54,60–67`. The public stream error at `src/stream/mod.rs:71–84` has only `PeerFailed`. `src/stream/incoming.rs:111–140` maps every `Closed.failure` and every `Failed` body to that single value, and maps all non-timeout Cancel reasons to `Cancelled`. No terminal accessor preserves the original protocol failure.

**Violated invariant:** Typed terminal failures must remain available to the operation adapter. In particular, authentication/trust/protocol failures and `Effect::None` versus `Effect::Possible` cannot become indistinguishable after a valid typed delivery.

**Reproduction/state sequence:**

1. Deliver `Failed { code: Authentication, effect: None }` to one active stream.
2. Deliver `Failed { code: Io, effect: Possible }` to an otherwise identical stream.
3. Both deliveries succeed, and every pending or later stream operation reports only `Error::PeerFailed`.
4. The original code and effect are unrecoverable through `Stream` or `MessageEndpoint`. A failed `Closed` has the same outcome.

**Impact:** Core cannot produce the actionable error reporting required by the transport contract or retain the uncertainty distinction needed after a possibly effective Git request. A host would have to duplicate stream semantic parsing outside the state machine before calling `deliver`, contrary to the runtime ownership boundary.

**Required correction:** Preserve the complete received `Failure` in terminal state and expose it through the file-like error or a terminal-result accessor. Preserve the defined Cancel reason as well, or restrict Cancel admission to reasons the local API can represent. Do not require the host router to duplicate this semantic state.

**Closure/regression test:** Deliver distinct authentication, trust, I/O and protocol failures with both effect values, plus failed Closed and supported Cancel reasons. Assert that blocked and later operations expose the exact code/effect consistently after any already accepted byte prefix.

### [P3-1] Local limited admission does not enforce its advertised nesting limit

**Location:** `codec::admit_limited` at `src/codec.rs:37–40` delegates to the generated typed visitor. `src/budget.rs:3–65` tracks encoded size, allocation and collection entries but has no depth state. `scripts/admission_codegen.py:13–39` generates recursive visitors without charging or checking depth. Serialized preflight does enforce depth at `src/codec/preflight.rs:73–80`.

**Violated invariant:** A function accepting `Limits` must enforce every applicable limit, and local typed admission must match serialized preflight.

**Reproduction:** Start with default limits, set `nesting = 1`, and pass a valid nested Envelope to `admit_limited`. `validate_limits` accepts the positive value and the generated local visitor ignores it. The same value encoded and checked under that limit is rejected by preflight once it descends through Envelope into its body.

**Impact:** The public limited-admission API has path-dependent semantics. The current binding’s separate `usable` rule requires nesting of at least ten, so the shipped active binding path is not presently exposed to this particular low-depth counterexample; custom callers and future schema growth remain vulnerable.

**Required correction:** Generate depth-aware local visitors, or reject negotiated nesting below the maximum depth required by the generated schema and document that interpretation.

**Closure/regression test:** For each nesting value around the generated schema’s actual maximum depth, assert parity among `admit_limited`, `encode_limited` and `decode_limited`.

## 2. Invariant analysis

The following attacks held:

- **No-wire boundary:** The crate contains no physical framing, socket, pipe, filesystem, background thread, executor or CLI/core service surface. Typed `Envelope` values are the stream boundary; codecs are optional utilities.
- **Schema ownership:** One authored taut schema generates the checked-in protocol, CBOR runtime, admission visitor and exported IR. The regeneration check reproduced all four artifacts. Ordinary builds require no Python, sibling checkout or network discovery.
- **Byte and ordering behavior:** Data offsets, exact byte reconstruction, independent directional credit, bounded partial writes and receive buffers, first-byte batching, full-buffer eligibility and read-triggered emission are coherent. The deterministic and randomized tests exercised these paths heavily.
- **Flush semantics:** Initiator-to-endpoint flush waits for sink consumption; endpoint-to-initiator flush acknowledges bounded read-adapter admission without granting credit. Barriers remain ordered before EndWrite.
- **Lifecycle:** Half-close preserves reverse traffic. Initiator close starts its deadline before credit wait, discards unread reverse bytes under credit, emits Data/EndWrite/Close in order, and cannot turn a failure into clean EOF. Endpoint completion remains host-controlled and requires both ends, empty request storage and completed barriers.
- **Async facade:** Futures are executor-independent; wakers are bounded and removed on cancellation; state changes wake blocked peers without spinning; clones share one exchange; final-owner drop cancels; endpoint loss wakes callers with CarrierLost.
- **Host boundary:** Per-stream delivery ordering, bounded aggregate queues, fair scheduling, monotonic clock service, routing and backend cleanup are explicitly assigned to the host. Their absence inside this crate is not a defect.
- **Deterministic replay:** SplitMix64 generation, per-case seeds, bounded steps, direct replay, event digests and repeated cases are deterministic. The independent source, offset and credit ledgers detect byte corruption, premature EOF and excess emission rather than merely comparing two implementations of the state machine.
- **Protocol basics:** Envelope body cardinality, version, stream domain, session/stream routing, offsets, window monotonicity and hard payload/metadata bounds fail closed. Binding checks session identity, version intersection and non-increasing numeric limits before Open.

## 3. Risks and next action

Physical delivery, external-type composition, core/CLI integration, pools, SSH/HTTPS adapters, platform qualification and production performance remain explicitly deferred. Their absence is not part of this verdict. The public generated CBOR runtime still exposes legacy infallible helpers, but the reviewed untrusted-data path uses bounded preflight and fallible decode; integrations must continue to enter through `codec`.

The next action is one bounded remediation in gwz-transport: attach effective admission limits to active streams and validate outbound host-supplied fields, centralize the authentication compatibility matrix, and preserve typed terminal failures. Add the four focused regression groups above, rerun the existing deterministic/default/extended checks, and return the exact revised tuple for a focused Code re-verdict.
