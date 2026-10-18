# In-memory transport acceptance — 2026-09-19

Status: **accepted at the tuple below after Code and State GO; this accepts
the in-memory typed-message stream checkpoint only**.

| Reviewed object | Commit |
|---|---|
| gwz-transport | `aa9ecae65d6c0d568c5f4d738f9930d49f684f56` |
| gwz-core controlling checkpoint/design/plan | `6a82c646fa9cff5555a182366bc3dd41707bd076` |
| workspace review inputs and merged remediation | `a93224a7d8b21a49534174073b7fa55ee62ce4ce` |

The subsequent documentation commit only files verdicts and annotates
acceptance. It does not change the accepted transport source.

## Review outcome

The original reviewers were reused as explicitly requested. The initial
[Code](GwzRemoteTransportMemory-ReviewCode.md) and
[State](GwzRemoteTransportMemory-ReviewState.md) reviews returned NO-GO:
five P2 findings representing four distinct roots, plus one P3. Both axes
independently found loss of peer failure code/effect. All were found during
implementation review, before integration or release.

One [merged remediation](GwzRemoteTransportMemory-RemPlan.md) addressed all
findings. [Code re-verdict](GwzRemoteTransportMemory-ReviewCode-1.md) and
[State re-verdict](GwzRemoteTransportMemory-ReviewState-1.md) both report GO,
verify original counterexamples and find no new findings. Reports are filed
verbatim. Two completed review rounds; one of two permitted remediation rounds
used. No production escape is claimed or observed; this is unintegrated code.

## Implemented result

The independent crate exports taut message types and a file-like async stream
facade over a deterministic state machine. Messages move directly between
in-memory endpoints. It implements partial reads/writes, byte credit,
first-byte coalescing, flush barriers, half-close, bounded graceful close,
explicit discard reporting, structured failures, cancellation and ownership
cleanup. Application waiters cannot consume the dispatcher wake slot.
Negotiated admission applies in both directions, and authentication settings
are validated before any future endpoint effects.

The runtime contains no physical I/O, framing, socket, pipe, timer task or
executor. The host supplies ordered bounded delivery and clock notifications.
Optional schema codecs are separate from the message-stream runtime and tests.

## Executed validation

| Gate | Result |
|---|---|
| Full suite on minimum Rust 1.95 | 38 executed tests pass, including the fixed 3,000-case walk; extended campaign separately ignored by default |
| Clippy, all targets, warnings denied | Pass on Rust 1.96 |
| Regeneration, pinned taut 0.9.1 | Four generated artifacts match |
| Fixed extended campaign | 50,000 cases pass on Rust 1.96 release build; seed `0x202609195eed` |
| Direct case replay | Case `0x1234` passes, including a Rust 1.95 replay at the accepted transport commit |
| Exported package | Standalone package builds; 33 focused tests pass from extracted package; caches excluded and public tests/scripts included |
| Original reviewers' checks | Both independently pass the full suite/clippy; Code repeats the fixed 50,000-case run; State passes a separate 10,000-case seed |

Toolchain attribution above is the precise lane-owner execution record: the
minimum-version full suite and direct replay used Rust 1.95; clippy, release
campaign and package verification used Rust 1.96. Python schema regeneration
used the pinned taut environment. No native Windows execution or network
performance measurement is claimed.

The fixed extended run exercised 6,247,412 Data messages, 9,071,148 blocked
writes, 19,583,722 blocked reads and 719,444 flushes. Random writes, reads,
windows, buffers, message caps and delivery delays reconstruct the exact source
bytes in both directions. These counts describe one reproducible run, not
independent coverage multiplied by rerunning it.

## Replaying a failure

The test approach follows `sdax-wz/sdax-rs`: fixed suite seed, independent case
seeds, explicit failure context and direct replay. Generator version is
`gwz-transport-stream-v1`; retain the source commit with any saved seed.
Failures include the run seed, case index/seed, configuration, inputs, step,
recent trace and exact command. Every sixteenth case repeats and checks its
trace digest and coverage. Default coverage floors guard against losing adverse
cases as the test evolves.

From `gwz-transport`:

```sh
cargo test --locked --test monte_carlo
GWZ_TRANSPORT_MC_CASE_SEED=0x1234 cargo test --locked --test monte_carlo seeded_message_streams -- --exact --nocapture
GWZ_TRANSPORT_MC_SEED=0x202609195eed cargo test --locked --release --test monte_carlo extended_message_streams -- --ignored --exact --nocapture
```

## Remaining scope

Phase 1 and Phase 2 are not declared complete. The taut external-type generator
prototype and test-only core consumer remain separate uncommitted work, outside
this acceptance. Their focused tests currently pass, including a typed async
message handoff, but their generation/pinning and integration gate remain open.
No production CLI/core surface changed.

Next: finish and qualify the exported-schema/shared-type integration, then
the planned pool and endpoint integration. Pools, SSH/HTTPS adapters, remote
repository provisioning, package publication, real network delivery and an
API/schema freeze are not accepted by this checkpoint. Physical delivery stays
outside gwz-transport under the operator's explicit scope clarification.
