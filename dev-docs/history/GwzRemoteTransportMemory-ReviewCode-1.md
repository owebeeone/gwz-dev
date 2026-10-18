# GWZ REMOTE TRANSPORT IN-MEMORY TYPED-MESSAGE STREAM — CODE-AXIS RE-VERDICT

**Review object:** Corrected initial gwz-transport implementation at `aa9ecae65d6c0d568c5f4d738f9930d49f684f56`, including committed source, schema, scripts, tests and README; remediation round 1; controlling checkpoint `gwz-core/dev-docs/GwzRemoteTransportMemoryImplementation.md` at `6a82c646fa9cff5555a182366bc3dd41707bd076`; implementation checkpoint, no interface/schema freeze, dated 2026-09-19.  
**Baseline:** gwz-transport `aa9ecae65d6c0d568c5f4d738f9930d49f684f56`; gwz-core `6a82c646fa9cff5555a182366bc3dd41707bd076`; workspace root `a93224a7d8b21a49534174073b7fa55ee62ce4ce`. Committed sources were read with `git show PIN:path`; remediation was examined with `git diff 89e9ebefee2598ababbbb227366ecbbd924b5cb5..aa9ecae65d6c0d568c5f4d738f9930d49f684f56`.  
**Date:** 2026-09-19  
**Axis:** Code — architecture, interfaces, call graphs and compatibility reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, zero P1, zero P2 and zero P3 findings. All four prior Code findings are verified closed, including both original P2 counterexample families, and no new architectural root cause was found in the remediation interactions.

---

## Prior-finding closure

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Code P2-1 | Carry directional negotiated limits into active streams; enforce typed ingress and validate locally supplied close facts before retention | `Config` now owns receiver and peer limits and validates construction against both (`src/stream/mod.rs:22–103`). Ingress uses `admit_limited` with receiver limits (`src/stream/incoming.rs:17–25`). `complete_close` builds and admits the candidate `Closed` under peer limits before storing it (`src/stream/machine.rs:224–253`). The original 257-byte and 16,385-byte fingerprint cases are rejected without terminalizing or queuing; 256 bytes succeeds. Direct typed, typed admission, limited encoding and limited decoding agree at the boundary (`tests/stream_limits.rs:27–95`). | **Closed** |
| Code P2-2 | Central Scheme × AuthPolicy × IdentityMode relation; reject unusable capability sets | The single relation in `src/policy.rs:4–39` defines the four executable triples. Open admission calls it at `src/codec/validate.rs:83–99`; binding creation filters intersections through it and refuses an empty executable set at `src/binding.rs:96–157`; Bound verification first applies the same codec validation at `src/binding.rs:160–188`. The full 2×4×3 Open matrix and the original SSH+gh impossible binding/Bound cases pass with effect-free rejection (`tests/policy.rs:51–136`). | **Closed** |
| Code P2-3 | Retain peer failure code/effect and restrict Cancel to representable reasons | `Error::PeerFailed { code, effect }` preserves both fields (`src/stream/mod.rs:107–123`). `Closed.failure` and `Failed` retain them through terminal state (`src/stream/incoming.rs:112–153`). Cancel admission accepts only `Cancelled` and `Timeout` (`src/codec/validate.rs:142–147`). Tests cover five failure codes, both effects, failed Closed, an accepted byte prefix, later reads/writes, both supported cancel reasons, an invalid cancel reason and asynchronous wake/error propagation (`tests/stream.rs:151–229`; `tests/async_stream.rs:207–238`). | **Closed** |
| Code P3-1 | Make typed admission enforce depth and match serialized accounting | The generated visitor now passes explicit root-relative depth, charges map keys, nodes, exact integer/string/container headers and absent optional values (`scripts/admission_codegen.py:5–47`; generated `src/admission.rs`). `Budget` enforces depth and entries while matching preflight’s canonical encoded-size and conservative allocation charges (`src/budget.rs:3–99`). Boundary sweeps compare typed admission, limited encode and limited decode for depth, entries, frame size and allocation (`tests/protocol.rs:95–140`). Regeneration reproduced all four committed artifacts. | **Closed** |

## Changed-range analysis

The remediation changes 19 files: 972 insertions and 269 deletions. The authored taut schema and its tags and fields are unchanged.

The directional limit correction is internally consistent. Receiver policy governs `receive`; peer policy governs locally emitted caller-controlled metadata. Construction rejects receive windows, peer windows, payloads and send buffers that exceed their respective negotiated budgets, then admits a maximum-offset, maximum-payload Data probe under both policies (`src/stream/mod.rs:56–103`). This establishes that generated Data fits negotiated frame, depth, entry and allocation limits. Other generated controls are bounded by the minimum usable policy; the only variable caller-supplied terminal metadata, `Facts`, receives its own pre-retention check. Rejected close facts leave the endpoint live and retryable.

The exact typed-admission accounting matches serialized preflight’s model: root depth is zero; only decoded value nodes consume collection entries; map keys consume encoded bytes and key allocation; each decoded node receives the same conservative CBOR-tree charge; text/byte payloads receive the same four-times-length charge; and final encoded bytes are included in the allocation ceiling. Integer head lengths include negative values correctly. The boundary tests and regeneration check found no path-dependent acceptance.

The authentication change has one source of truth. Flat capability lists now mean compatible pairs under that relation, and every advertised scheme and policy must have a partner. Intersection filtering cannot leave a dangling member, Open admission checks the complete triple, and Bound verification rejects a false capability product before installation.

Structured peer errors remain stable after buffered prefixes and across synchronous and asynchronous entry points. Unsupported Cancel reasons fail protocol admission instead of being collapsed into `Cancelled`.

The dispatcher-capacity correction does not compete with application waiters. Registrations are classified in `src/stream/asynchronous.rs:14–22`; application waiters use `Config.max_waiters`, while exactly one dispatcher registration has separate capacity at lines 51–139 and 239–255. Both registration orders, application overflow, Data wakeup, Cancel wakeup and cleanup pass at application capacity one (`tests/async_stream.rs:156–205`).

The bounded ancillary changes are coherent: `Cargo.toml:9` packages authored schema/IR, scripts and tests through narrow patterns that exclude Python caches; Monte Carlo construction and replay failures now print reproducible seed information. The crate still has no dependencies or physical-I/O implementation. No new architectural root cause was found.

## 0. Evidence base

The exact tuple resolved unchanged at both start and end:

| Repository | Commit | Tree |
|---|---|---|
| gwz-transport | `aa9ecae65d6c0d568c5f4d738f9930d49f684f56` | `f60b9fc47f24d66f7e75e40c36336c53dd571380` |
| gwz-core | `6a82c646fa9cff5555a182366bc3dd41707bd076` | `0fa0cfdfbb5f97607e442ef17ba3c54641395e6c` |
| workspace root | `a93224a7d8b21a49534174073b7fa55ee62ce4ce` | `a962437fa4435066d7204fc647c1c500eadc6ee2` |

gwz-transport had no tracked or untracked changes. The root contained only the generated current-round prompts, and gwz-core contained the explicitly excluded uncommitted `tests/transport_consumer`; neither changed the reviewed commits.

The review read the complete focused prompt, root instructions, process authorities, merged remediation plan and original Code report. It rechecked the memory checkpoint, design §§5–6 and relevant requirements/plan boundaries at the core pin. It inspected every changed source/test range and the generated admission output, plus the unchanged codec preflight needed to compare accounting. No current-round peer report was read.

Independent commands and results:

- `cargo test --locked`: 38 ordinary/default tests passed, including the fixed 3,000-case randomized walk; one extended test remained intentionally ignored.
- `cargo clippy --locked --all-targets -- -D warnings`: passed.
- Pinned regeneration check: `4 generated artifacts verified`.
- Direct replay with `GWZ_TRANSPORT_MC_CASE_SEED=0x1234`: passed.
- Fixed 50,000-case release campaign with run seed `0x202609195eed`: passed in 49.48 seconds. Coverage was 9,071,148 blocked writes, 19,583,722 blocked reads, 2,523,054 partial writes, 719,444 flushes, 3,030 tiny-window cases, 193 empty streams, 710,829 zero operations, 4,386,561 delayed deliveries and 6,247,412 Data messages.
- Local independent execution used Rust 1.96. The lane owner additionally reported the same tuple passing its Rust 1.95 suite and standalone package build; those two commands were not rerun because the focused prompt’s command allowlist did not include packaging.

## 2. Invariant analysis

The attacks that support this GO held:

- **Negotiated admission:** Active streams retain directional receiver/peer policies. Direct typed ingress and serialized ingress agree at the original metadata counterexample boundary. Locally supplied facts are rejected before storage or cloning, and invalid construction cannot expand negotiated windows, payloads, frame size or queue capacity.
- **Capability reality:** Every admitted Open has one executable scheme/authentication/identity triple. Binding cannot advertise a scheme or policy with no compatible partner, and an impossible Bound cannot be installed.
- **Failure fidelity:** Authentication, trust, I/O, protocol and invalid-request failures preserve exact effect information after accepted data and through asynchronous wakeups. Cancel has no silently erased reason.
- **Admission parity:** Typed walking enforces depth, total nodes, canonical encoded length and conservative allocation without encoding or constructing a CBOR tree. Its accounting agrees with preflight around all four negotiated boundaries exercised.
- **Control progress:** Application waiter saturation cannot occupy the dispatcher slot. State changes wake the dispatcher, and cancellation removes all registrations.
- **No-wire boundary:** The remediation adds no socket, pipe, framing, filesystem, background executor, core/CLI service or external dependency. Typed generated Envelopes remain the in-memory boundary.
- **Schema ownership:** The authored schema is unchanged, generated outputs reproduce exactly, and package inclusion remains restricted to authored and generated public artifacts.
- **Deterministic evidence:** Direct case replay and the fixed 50,000-case run reproduce and retain substantial pressure across both byte directions, blocking, partial writes, flushing, tiny windows and delayed delivery.

## 3. Risks and next action

This GO accepts only the corrected in-memory typed-message stream checkpoint. Physical delivery, aggregate host scheduling, external-type composition, the excluded core consumer, pools, SSH/HTTPS adapters, real core/CLI integration, platform qualification and release remain deferred. The schema and public API remain draft-stage and unfrozen.

The next action is to record this Code GO alongside the independent current-round verdict and, if that gate also passes, accept the bounded memory-stream checkpoint before proceeding to its later integration gates.
