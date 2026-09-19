# Gwz remote transport Phase 1/2 interface candidate — Code review

## Review object

- **Date:** 2026-09-19
- **Axis:** Code
- **Object:** Phase 1/2 schema, admission, message-handoff, stream, and pool interface-freeze candidate
- **Root:** `9bb74ace2fca56f770af34b6ec1bdf6bab4e695e`
  - Tree: `6fadda10c4e45a049164e3a74ebf4cbd471e773b`
- **gwz-core:** `ce3bb967f9696b1791fcbefb65b6ccdd0c8ab618`
  - Tree: `b0fe75ea55d7d4e85719acb4c10ca4586485828d`
- **gwz-transport:** `163feebe439edd5d1fbaf11e163882e80d4f2257`
  - Tree: `4cebefa1058d0b87c838ecdb70e0a0ecc70d527c`
- **taut:** `733e8a78897a90f017f4726e4331aed95e8cb977`
  - Tree: `bb0694d0e31d217b82024cb73c2e1212dfa5c72f`

The tuple and trees matched at both the beginning and end of review. The three member repositories were clean. Root contained only the permitted untracked current-round prompts and Surface report.

## Verdict

**NO-GO**

| Severity | Count |
|---|---:|
| P0 | 0 |
| P1 | 0 |
| P2 | 1 |
| P3 | 1 |

`P2-1` blocks the requested interface freeze. The runtime and schema work otherwise remained within the declared scope: no physical carrier, production dispatcher, adapter, credential flow, or CLI/core service was introduced.

I pre-commit to **GO** on a revision that resolves `P2-1` as specified, provided focused changed-range inspection finds no regression. `P3-1` should also be corrected so the final checkpoint does not overstate its timeout-policy evidence.

## Evidence base

I inspected the pinned root checkpoint; the core requirements, design, plan, and interface gate; the complete transport and core changed ranges; the generated protocol and admission code; stream and pool clock machinery; binding and policy paths; async interfaces; public documentation; workflow/regeneration declarations; and the isolated consumer fixtures.

Focused execution produced:

- Rust 1.95 transport tests passed: 21 tests across `io_clock`, `io_clock_async`, `pool_io_clock`, `network_timeouts`, and `policy`.
- Owner regeneration check passed using the pinned core regeneration environment: four artifacts verified.
- Source-pinned consumer regeneration check passed.
- Isolated archive proof passed all 16 consumer tests using archive SHA-256 `8c7d91d54f0e176109f286beec65c24544f35f23506380939fdb081e6cef87a6` and source revision `163feebe439edd5d1fbaf11e163882e80d4f2257`.
- Both reviewed source ranges passed `git diff --check`.
- An initial owner-regeneration attempt with unqualified system `python3` failed because that interpreter lacked the `taut` module. The documented pinned environment then passed; this was an environment-selection failure, not a generated-artifact mismatch.
- No full suite, extended randomized campaign, native-platform qualification, remote CI, or physical-transport test was run or inferred.

No current peer report was opened or used. During a repository-wide command search, one generic `COMMANDS` line from the State prompt was emitted accidentally; it duplicated the command authorization already present in the Code prompt and exposed no State analysis, finding, or verdict.

## Findings

### P2-1 — `Binding::check_open` and the retained consumer handoff revert negotiated ingress admission to default limits

**Root cause**

The mandatory pre-effect Open path does not apply the receiver’s negotiated binding limits. `Binding::check_open` invokes default-limit admission, and the retained consumer proof independently admits every incoming envelope under the same defaults.

**Locations**

- `gwz-transport/src/binding.rs:64-85`
  - `Binding::limits()` exposes the negotiated receiver limits.
  - `Binding::check_open()` nevertheless calls `codec::admit(message)`, which uses defaults.
- `gwz-transport/src/codec.rs:32-41`
  - `admit()` delegates to `admit_limited()` with `binding::default_limits()`.
- `gwz-transport/src/admission.rs:151-169`
  - Open metadata, including `operation_id`, is charged through the limited budget.
- `gwz-transport/src/budget.rs:81-91`
  - Metadata is rejected when its length exceeds `limits.metadata_bytes`.
- `gwz-core/tests/transport_consumer/tests/admission.rs:9-15`
  - `handoff()` always uses `binding::default_limits()`.
- `gwz-core/tests/transport_consumer/tests/admission.rs:39-50`
  - `dispatch_open()` increments its endpoint-effect counter after only that handoff and `Binding::check_open()`.
- `gwz-core/tests/transport_consumer/tests/admission.rs:242-300`
  - The fake endpoint narrows only `data_payload`; it never challenges negotiated metadata or envelope budgets.
- `dev-docs/GwzRemoteTransportInterfaces-Checkpoint.md:45-48,154-165`
  - The checkpoint claims the paired fixtures prove bounded Open admission, negotiated caps, and pre-effect rejection.
- `gwz-transport/README.md:226-237`
  - The public contract requires limited typed or encoded admission before effects.

**Violated invariant**

An Open must be admitted under the bound receiver policy before credentials, lease allocation, or network work. Typed and encoded handoff must reject the same message under the same negotiated limits. A narrowed negotiated cap cannot silently revert to the package default.

**Reproduction**

1. Build an endpoint configuration whose otherwise-default limits set `metadata_bytes = 256`, the minimum accepted by `binding::usable`.
2. Accept a normal Bind. The resulting `Binding::limits().metadata_bytes` is 256.
3. Construct an otherwise-valid Open for that binding, set its `receive_limits` to `binding.limits()`, and give `operation_id` 257 bytes.
4. Send it through the consumer’s typed or encoded `handoff()` and then `dispatch_open()`.
5. `handoff()` admits under the default 16 KiB metadata limit. `Binding::check_open()` repeats default admission, and its ownership/capability/limit comparisons pass. `dispatch_open()` increments the effect counter.
6. `codec::admit_limited(&message, binding.limits())` rejects the same Open with `Bounds`.

The retained proof therefore permits a message over the negotiated receiver cap to reach its modeled endpoint effect.

**Impact**

The frozen typed-message contract would leave negotiated ingress limits dependent on an extra caller check that the canonical consumer call site omits. This creates typed/encoded policy divergence once a carrier or dispatcher correctly applies negotiated limits and invalidates the checkpoint’s pre-effect admission evidence. The hard package defaults still bound the message, so this is a P2 contract and parity defect rather than an unbounded-allocation P1.

**Required correction**

Make the pre-effect Open path enforce the installed binding’s receiver policy. The direct correction is for `Binding::check_open()` to use `codec::admit_limited(message, &self.bound.receive_limits)`. If admission remains a separate host responsibility, the public API and retained fake host must instead make that step explicit and unavoidable before `check_open()` and before effects; the current split is demonstrably unsafe at the canonical call site.

Update the consumer handoff/dispatch fixture to use the negotiated receiver limits rather than defaults.

**Closure test**

With a binding whose `metadata_bytes` is 256, exercise both typed and bounded encoded delivery:

- 256-byte Open metadata is accepted.
- 257-byte `operation_id`, destination metadata, or identity metadata is rejected.
- The endpoint-effect counter remains zero on rejection.
- Default-limit admission alone is insufficient to make the rejected Open executable.
- Existing ownership, capability, and receive-limit narrowing tests continue to pass.

### P3-1 — The claimed native timeout-tightening proof stops before the host-policy check

**Root cause**

The design intentionally leaves native timeout policy in the endpoint host, but the cited fake-host evidence has no captured endpoint policy and counts an effect immediately after schema/binding admission.

**Locations**

- `gwz-transport/README.md:108-122`
  - Defines zero/positive network timeout semantics and the tightening relation.
- `gwz-transport/README.md:216-234`
  - Explicitly says the host checks deadline tightening before effects and that `Binding` does not contain native timeout settings.
- `gwz-transport/src/binding.rs:42-50,68-85`
  - `EndpointConfig` and `Binding::check_open()` contain no native timeout policy, as intended.
- `gwz-transport/src/stream/mod.rs:39-58,79-110`
  - A stream configuration accepts `io_timeout_ms = 0`, which correctly represents an already-authorized disabled timeout but cannot establish whether an Open was allowed to request it.
- `gwz-transport/src/pool/mod.rs:170-191`
  - Pool requests enforce connect-timeout tightening against pool policy; there is no equivalent host-composition proof for Open `io_ms`.
- `gwz-core/tests/transport_consumer/tests/admission.rs:39-50,287-300`
  - The fake dispatcher accepts `(connect_ms, io_ms) = (0, 0)` and immediately increments its effect counter without supplying or checking endpoint policy.
- `dev-docs/GwzRemoteTransportInterfaces-Checkpoint.md:154-165`
  - The evidence matrix attributes disabled/native-maximum timeout tightening to `network_timeouts.rs`, `policy.rs`, and the consumer Open fixture.

**Violated invariant**

A zero Open network timeout is admissible only when the corresponding endpoint policy is already disabled. A positive request may impose a finite bound on disabled policy or shorten finite policy, but cannot lengthen it. That relation must be checked before the first endpoint effect.

**Reproduction**

1. Assume a captured endpoint I/O policy of 3,000 ms.
2. Send the consumer fixture an otherwise-valid Open with `io_ms = 0`.
3. Schema and binding admission accept zero because zero is a valid wire representation.
4. `dispatch_open()` increments its effect counter without comparing the request with 3,000 ms.
5. Copying the admitted value into `StreamConfig::io_timeout_ms` constructs a valid stream with no network deadline, as `tests/network_timeouts.rs:28-42` demonstrates.

This does not prove a production-host defect because that dispatcher is deferred and the README assigns the check to it. It does prove that the cited fixture does not qualify the documented relation.

**Impact**

The evidence record overstates coverage of an interface-critical host obligation. A later dispatcher can reverse a finite native policy into an untimed operation while still resembling the retained fake-host sequence. The required values are representable by the public pool and stream APIs, so this is a bounded evidence and integration-fixture defect rather than a current runtime-interface blocker.

**Required correction**

Extend the fake endpoint boundary with captured connect and I/O policies and resolve Open deadlines before incrementing effects or constructing pool/stream inputs. The fixture may use a small explicit resolver rather than adding timeout fields to `Binding`.

**Closure test**

For both connect and active I/O policy, prove before effects that:

- finite policy rejects zero and values above the configured maximum;
- finite policy accepts positive equal or shorter values;
- disabled policy accepts zero and positive finite bounds;
- `i32::MAX` is preserved when allowed;
- rejected requests leave the effect counter at zero;
- accepted requests produce the exact pool request and stream configuration values;
- helper time transferred from connect/auth remains cumulative and independently bounded.

## Invariant analysis

- **Schema and nominal ownership:** The transport schema, generated Rust types, consumer wrapper, and taut integration retain the established ownership and identity. Regeneration checks found no drift.
- **Bootstrap and capabilities:** Bind/Bound ownership, capability intersections, policy/identity compatibility, and effect-free rejection remain coherent. Bootstrap continues to use the fixed hard ingress limits required before a binding exists.
- **Negotiated admission:** Failed because the canonical Open path falls back to defaults instead of the installed binding limits (`P2-1`).
- **Active-I/O clock:** Focused source inspection and tests support exact-deadline precedence, pause/resume without refill, positive peer-byte progress, disabled/native-maximum timing, cumulative helper allowance, close takeover, prefix preservation, first-cause retention, and async wake behavior.
- **Pool/runtime lifecycle:** Optional disabled connect timing, request tightening, allocation/connect/helper/cleanup separation, lease discard, and disposal acknowledgment remain represented by the public API and focused tests.
- **Native timeout composition:** The public values can represent the required mapping, and pool connect overrides enforce their relation. The retained fake host does not qualify the corresponding Open-to-stream host check (`P3-1`).
- **Scope control:** No physical framing, production carrier, dispatcher, SSH/HTTPS adapter, credential transport, retry policy, or CLI/core service surface entered the candidate.
- **Evidence honesty:** Regeneration and archive proof were reproducible locally. Remote CI, native platforms, real adapters, physical ingress, and large campaign reruns remain correctly unclaimed.

## Residual risks and next action

Physical carrier bounds, aggregate host queues, real timer service, native adapters, credentials, registry publication, and cross-repository CI remain deferred and are not accepted by this review.

Apply one bounded correction set:

1. enforce negotiated receiver admission on the pre-effect Open path and add typed/encoded one-over-bound regressions;
2. add the missing fake-host native timeout-policy composition proof and narrow the checkpoint’s evidence claims if any part remains deferred.

Then rerun the focused admission, policy, timeout, regeneration, and isolated archive checks and perform a changed-range Code re-verdict on the corrected immutable tuple.
