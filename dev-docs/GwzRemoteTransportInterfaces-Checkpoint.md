# Remote transport Phase 1/2 interface checkpoint

Status: **accepted; Phase 1 schema/admission/message-handoff and Phase 2 runtime interfaces frozen**.
Date: 2026-09-19.

## Acceptance

Accepted at root review inputs `9d0dc7ef5c616d64d52c296ea2fa34d83d21d73e`,
core `ace269896ad80aee923e2e8fd31e565c43de57ed`, transport
`28f5afb3938a2aa8af0e1e8d5b07779add6ab776` and taut
`733e8a78897a90f017f4726e4331aed95e8cb977`, after
[Code](GwzRemoteTransportInterfaces-ReviewCode-1.md),
[State](GwzRemoteTransportInterfaces-ReviewState-1.md) and
[Surface](GwzRemoteTransportInterfaces-ReviewSurface-1.md) all reported GO.
All four findings are closed by their originating reviewers; none are deferred
or self-closed. Reports are filed verbatim. Acceptance records and core plan/gate
status updates are documentary descendants of the reviewed implementation.

Current core documentation descendant: `9303eb86914aa5770b4f951613270b14b2108f73`; its diff
from the reviewed core contains only the plan and interface-gate status records.

This freezes the named Phase 1 schema/types, negotiated admission and message
handoff contract and Phase 2 stream/pool runtime API only. It does not advertise
SSH/HTTPS support, a production host, native platforms, registry availability or
remote CI. Remaining programme work begins with Phase 3 safe per-remote git2
callback qualification and the host SSH adapter; production placement and HTTPS
follow under their existing gates. No wire carrier belongs in gwz-transport.

Metrics: two completed review rounds, one merged remediation, one P2 and three
P3 findings discovered at interface review, all closed before acceptance; zero
new findings at re-review, no blind convergence on the blocking defect, no known
production escapes. This acceptance completed in the resumed 2026-09-19 task;
exact end-to-end wall time/session count across prior interruptions was not
instrumented and is not estimated. Local evidence is 89 owner tests, 18 isolated
consumer tests, normal seeded replay, generation/format/tooling checks and clean
archive verification. The re-reviewers independently repeated focused gates;
Surface reproduced setup in its own fresh environment. Remote/platform outcomes
remain explicitly unqualified.

## Initial review disposition (historical)

The initial exact-tuple review returned Code **NO-GO** (P2-1 negotiated
Open admission, P3-1 host timeout-policy evidence), State **GO** (P3-1 clock-only
wake amplification), and Surface **GO** (P3-1 Python setup documentation).
Reports are filed verbatim in the adjacent ReviewCode/ReviewState/ReviewSurface
files. The merged `GwzRemoteTransportInterfaces-RemPlan.md` accepts all four
findings as one bounded correction. No schema or public signature changes.
At this initial checkpoint, one review round was completed and remediation was pending; no blind convergence
on the blocking root. No production escapes are known. The candidate evidence
below describes the initial tuple and must not be read as acceptance or as
closure of the review findings. Revised-tuple qualification and acceptance are recorded above and below.

## Remediation 1 qualification

Revised transport: `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`.
Revised core: `ace269896ad80aee923e2e8fd31e565c43de57ed`.
Taut remains `733e8a78897a90f017f4726e4331aed95e8cb977`.
The generated re-review prompts pin the root commit containing this record.
Owner archive SHA-256:
`986033108eab2967028dc52c69f94e859ed6cbb78384648f03e88d9703383191`.

Binding now enforces installed receiver limits. The consumer's single fake host
effect boundary validates negotiated typed or encoded admission, ownership and
captured connect/I/O policy first. Its metadata matrix covers three 256/257-byte
field boundaries, and its native-policy matrix independently rejects disabled
or lengthened finite connect/I/O requests. Accepted values feed exact pool and
stream inputs, with capped cumulative helper remainder. The encoded receiver
actually runs the bounded decoder; no default-cap rejection proxy is claimed.
Host policy remains test-only; no production dispatcher or identity resolver is
implemented. Public signatures and generated schema artifacts are unchanged.

Clock-only pool ticks, helper pause/resume, stream classification and peer-byte
progress no longer wake unrelated callers. Terminal/data readiness still wakes;
cleanup abort work becomes runnable at its exact deadline. Host timer snapshots
and recomputation duties stay unchanged. The consumer README now supplies fresh
Python setup, the exact package and formatter/toolchain selection.

Executed final correction checks:

- Rust 1.95 complete owner suite: **89 passed, 0 failed, 2 opt-in campaigns
  ignored**, including normal 3,000 stream / 2,000 pool seeded cases.
- Focused admission, binding, wake, timeout and async tests: passed.
- Owner generation: four artifacts match; consumer exact-source generation:
  passed in a newly created temporary environment containing taut-proto 0.9.1.
- Consumer tooling tests: **17 passed**. Owner/consumer formatting and whitespace
  checks passed. No new conditional-compilation declarations were introduced.
- Clean committed owner package built and verified on Rust 1.95.
- **18 isolated consumer tests passed** against that exact archive, offline and
  locked, using the fresh Python environment and Rust 1.96 selection documented
  in the README. Its source revision and digest were checked by the proof runner.

Reproduction (from workspace root, using the README prerequisites):

```sh
gwz-core/protocol/.regen-venv/bin/python \
  gwz-core/tests/transport_consumer/package_proof.py \
  --archive gwz-transport/target/package/gwz-transport-0.1.0.crate \
  --archive-sha256 986033108eab2967028dc52c69f94e859ed6cbb78384648f03e88d9703383191 \
  --source-revision 28f5afb3938a2aa8af0e1e8d5b07779add6ab776
```

Correction size: transport production +28/-10 across three source files;
owner tests +232 across two files; consumer tests +349/-17 in one cohesive
admission fixture (688 total lines). Documentation changes are separate.
All fit the remplan ceilings. Red/green and evidence-only disclosures are in
the remplan. Initial reports remain verbatim evidence; the old qualification
and metrics below describe that original candidate, not the revised acceptance
status. No remote CI, native adapter, publication or new large campaign is claimed.

## Object and authority

This checkpoint seeks the two named interface freezes in
`gwz-core/dev-docs/GwzRemoteTransportPlan.md`: Phase 1 schema/types, bounded
admission and message-handoff contract; Phase 2 stream/pool runtime API.
Controlling detail is the existing design with its bounded §10.1 active-I/O
addition and `GwzRemoteTransportPool-InterfaceGate.md`. No new transport schema,
physical carrier, CLI/core service method, adapter or credential flow is added.

The operator authorized the small design addition, implementation and planned
interface review. The review-loop skill applies. Original Code and State
reviewers are retained as requested; Surface is a separate public-document-only
axis. All three must return GO on the settled tuple before either freeze is
recorded. Prior stream/pool/integration GO reports are baseline evidence, not
acceptance of this object.

Baseline: root `d6aefddbf3093d14a346896c7fff8de6bfe0d756`, core
`435e936b593476f24fad4cc4e70f5d06b784ed7d`, transport
`e8b9a1c5408cc9ea9528939b3a602acbeb697814`, taut
`733e8a78897a90f017f4726e4331aed95e8cb977`.
Transport implementation: `163feebe439edd5d1fbaf11e163882e80d4f2257`.
The source archive SHA-256 is
`8c7d91d54f0e176109f286beec65c24544f35f23506380939fdb081e6cef87a6`.
Core implementation: `ce3bb967f9696b1791fcbefb65b6ccdd0c8ab618`.
Taut remains `733e8a78897a90f017f4726e4331aed95e8cb977`. The root review-input
commit is recorded in the generated prompts and verified by reviewers; it
contains this evidence record and the managed member locks.

## Contract completed here

The endpoint host explicitly classifies Idle / Network / Backpressure /
Interaction, records actual peer-byte progress, initializes and advances a
single monotonic origin, and runs its own independent timer service. Network
progress resets only network allowance; pauses preserve it and helper time is
cumulative. Connect and active-stream helpers share one Open allowance through
host accounting. Exact-deadline expiry wins, close takes over its own budget,
first terminal cause and received prefixes survive, and disposal acknowledgment
precedes reclaiming a timed-out lease's physical capacity.

Typed and encoded consumer fixtures exercise binding, Open admission before
fake endpoint effects, the full envelope inventory and stream conversation
contracts using the generated test wrapper. This is a trusted
wrapper fixture, not a production outer decoder or physical delivery layer.

Native disabled and maximum positive timeout settings are represented without
changing schema tags/types. `Connect.network_deadline` is optional and preserves
the distinction between an undispatched connect and a started, untimed connect.
Only network domains can be disabled; helper and disposal remain bounded. This
corrects a compatibility gap in the pre-freeze prototype under the existing
native-policy preservation requirement.

The owner generator now checks its exact formatter build, and a standalone
workflow declares generation-drift, formatting, MSRV tests and packaging. The
consumer archive unit tests use self-contained fixtures; actual archive
identity/revision/digest is qualified by the explicit isolated proof runner.

## Qualification and limits

Executed on 2026-09-19:

- `cargo +1.95.0 test --manifest-path gwz-transport/Cargo.toml --locked --quiet`:
  passed, including the fixed 3,000-case stream and 2,000-case pool replay suites.
  The two opt-in extended campaigns were not rerun; targeted new clock tests
  qualify the new behavior while the normal random suites protect interaction
  with the existing stream/pool machinery.
- Both owner and consumer `cargo fmt --all -- --check`: passed.
- Owner regeneration `--check`: all four artifacts match; exact formatter guard
  unit tests: two passed.
- Explicit source-pinned consumer regeneration: matched the checked artifact.
- Consumer regeneration and archive tooling pytest suite: 17 passed.
- `cargo +1.95.0 package --manifest-path gwz-transport/Cargo.toml --locked`:
  clean archive built and verified. Cargo warns only that this unpublished
  package has no documentation/homepage/repository metadata.
- Isolated consumer archive proof: all 16 integration tests passed, using only
  the verified archive and copied consumer, offline and locked. No sibling source
  patch is retained in a manifest.
- Owner workflow YAML parsed locally; source diffs pass whitespace checks.
  Remote workflow execution is not claimed.

The exact archive proof command is:

```sh
gwz-core/protocol/.regen-venv/bin/python \
  gwz-core/tests/transport_consumer/package_proof.py \
  --archive gwz-transport/target/package/gwz-transport-0.1.0.crate \
  --archive-sha256 8c7d91d54f0e176109f286beec65c24544f35f23506380939fdb081e6cef87a6 \
  --source-revision 163feebe439edd5d1fbaf11e163882e80d4f2257
```

Transport change from the baseline: 263 production additions / 35 deletions
across ten source files; 719 test additions / two deletions across seven test
files; 22 lines for the formatter guard test module, plus the small regeneration/CI and
public-documentation changes. No production code moved to a new owner. This
fits the recorded initial clock and bounded native-compatibility scopes.
Core Rust proof tests add 643 / remove 22 lines across three files, within
the expanded 700-line ceiling. The compatibility test file is 264 lines, within the 20% allowance on its 250-line
ceiling. Test counts here are observations, never inventory pins or replacement
pass/fail gates.

Remote CI is not executed: transport has no remote and the extended taut source
revision is not established as remotely available. The owner workflow is ready
for its own checkout; cross-repository consumer CI is not activated. Local
reproducible generation and archive tests remain the evidence. No file-exists
skip or absent remote input is presented as successful CI.

Native platforms, real host dispatch/timers, physical message delivery bounds,
SSH/HTTPS behavior, registry resolution and performance remain later integration
or release evidence. No physical framing belongs in gwz-transport. No push,
publication or remote provisioning is part of this checkpoint.

## Review metrics

Implementation and local gates complete. Review rounds: 0. Accepted-through remains the
prior shared-schema checkpoint. New production escapes: none known. Implementation-contact corrections included
preventing clock rearming after Close, retaining transition-only batching wakes,
accounting the final budget before terminal stop, and strengthening fake-host
admission/timeout fixtures so their effect/disposal assertions exercise real
ordering. These are draft corrections, not independent-review findings. The
new async timer tests passed on Rust 1.95, including exact-boundary wakeups,
first-cause retention, independent batching and no repeated wakes under credit
exhaustion. Local gates now pass; reviewer verdicts remain pending.

## Pre-freeze compatibility contact

The native startup configuration accepts zero (disable network timeouts) and
positive milliseconds through `i32::MAX`; the proposed positive-only 24-hour
runtime bound cannot represent that contract. This is an implementation-contact
finding before reviewer dispatch, not a user request to change native policy.
Design §10.2 now records the mapping; its correction is under implementation
and must pass the same interface gate.
No silent fallback, clamping to 24 hours, or use of application Idle state to
simulate a disabled network clock is acceptable.

Process disclosure: the clock drafter ran the existing suite first, then added
clock tests after drafting code; it did not execute a genuine pre-implementation
red phase. The checkpoint does not claim that it did. The formatter guard did
have a missing-function red phase followed by green tests. New behavioral
corrections must demonstrate their failing regression before production edits.

The focused codec regression initially failed on typed `connect_ms = 0`
(`InvalidMessage`), then passed after separating network and other deadlines.
It tests typed and bounded encoded ingress for zero, negative, maximum and
excessive network values. The consumer's generated-wrapper regression also
failed on zero before that production change. The pool/stream correction first produced four expected `u64`/`Option<u64>`
compile failures in its new fixtures, then passed focused and complete suites.
Seven native-timeout tests cover disabled and maximum deadlines, helper resume /
expiry and acknowledged cancellation/shutdown disposal. The schema IR and generated types need no regeneration change.

The local verification matrix maps obligations to public tests:

| Obligation | Evidence source |
|---|---|
| Native schema ownership and all message variants | consumer `identity.rs`, `admission.rs`; owner `protocol.rs` |
| Bind/Open rejection before fake endpoint effects; negotiated caps | consumer `admission.rs`; owner `binding.rs`, `policy.rs` |
| Typed/encoded data, credit, flush, half-close, reverse drain, terminal and deadline propagation | consumer `messages.rs` (each scenario in both handoff modes) |
| Finite I/O budget, pauses, cumulative helper, prefix, closing and backward clocks | owner `io_clock.rs` |
| Async timer wake, first cause, separate batching and no repeat wake storm | owner `io_clock_async.rs` |
| Helper allowance transfer and discard before physical capacity release | owner `pool_io_clock.rs` |
| Disabled/native-maximum timing, tightening, bounded helper and cleanup | owner `network_timeouts.rs`, `policy.rs`; consumer Open fixture |
| Resource/stream lifecycle adversity and deterministic random replay | existing owner `stream*`, `async_stream`, `pool*`, `monte_carlo` suites |
| Generation/package independence and provenance | owner regen/unit checks and consumer source-pinned regeneration, tooling tests and isolated archive proof |

The matrix identifies tests, not a claim that remote adapters, real outer-message
allocation or native hosts were exercised. Fixture queues are bounded and owner
admission checks run in both input modes; actual host aggregate enforcement
remains a supplied-layer duty.
