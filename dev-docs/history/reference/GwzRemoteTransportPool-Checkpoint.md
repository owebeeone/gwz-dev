# Endpoint connection pool checkpoint — 2026-09-19

Status: **accepted at the tuple below after original Code and State reviewers
reported GO; this accepts the in-memory connection pool checkpoint only**.

| Review object | Commit |
|---|---|
| gwz-transport | `e8b9a1c5408cc9ea9528939b3a602acbeb697814` |
| gwz-core controlling checkpoint/design/plan | `5122031244bdf62eda0105aef9f7e98a0a5ce8dc` |
| workspace review inputs and merged remediation | `dd15b7c99a625fc84241c191caba603df4ccae02` |

## Scope

The independent crate now has a deterministic connection pool and an async
facade with exclusive leases. Clones share endpoint capacity. Defaults are eight
physical connections per user/host across ports, eight total per host across
users/ports/schemes, 256 per endpoint and 1,024 outstanding requests. Idle expiry
is 60 seconds after healthy release. Opening and closing consume capacity until
the host acknowledges actual completion or disposal.

The pool handles identity eligibility, fair eligible allocation, bounded waits,
network/helper/cleanup clocks, cancellation, late connection completion,
spontaneous idle loss and shutdown. Structured session/operation ownership
prevents stale cancellation crossing sessions. The host owns physical resources,
clock service and cleanup proof. The stream/lease test demonstrates explicit
release only after backend cleanup. No wire, socket, carrier framing, timer task
or CLI/core interface was added. The accepted stream runtime and taut schema
remain unchanged.

## Review history

The original [Code](GwzRemoteTransportPool-ReviewCode.md) and
[State](GwzRemoteTransportPool-ReviewState.md) reviews returned NO-GO with three
P2 findings: spontaneous idle loss had no callback; cancellation conflated
session and operation ownership; the first capacity ceiling was per reuse key
rather than per user/host across ports. These were three distinct roots with no
blind convergence, found before integration or release.

One [merged remediation](GwzRemoteTransportPool-RemPlan.md) implements all three
corrections and their regressions. The original reviewers were reused at the
operator's request. The first re-review attempt ended at an account usage limit
without verdicts; it is not an additional completed review round. The resumed
review uses exactly the same committed tuple. Both the
[Code re-verdict](GwzRemoteTransportPool-ReviewCode-1.md) and
[State re-verdict](GwzRemoteTransportPool-ReviewState-1.md) report GO, verify
original counterexamples and find no new issues. Reports are filed verbatim.
Two completed review rounds, one of two permitted remediation rounds used; no
open findings or observed production escapes. Acceptance is limited to this
unintegrated pool implementation. Subsequent documentation commits record
acceptance without changing the reviewed transport source.

## Executed local evidence

All commands below ran from `gwz-transport` unless a workspace-relative path is
shown. These are concise verification records; no private raw campaign artifact
was created. Tests and replay runners are public product tests.

| Gate | Result |
|---|---|
| `cargo +1.95.0 test --locked` | 66 tests pass, including fixed 3,000-case stream and 2,000-case pool schedules; two extended campaigns intentionally ignored by default |
| `cargo fmt -- --check` | Pass |
| `cargo clippy --locked --all-targets -- -D warnings` | Pass on Rust 1.96 |
| `gwz-core/protocol/.regen-venv/bin/python gwz-transport/scripts/regen.py --check` from workspace root | Four generated artifacts match |
| `GWZ_POOL_MC_CASE_SEED=0x1234 cargo +1.95.0 test --locked --test pool_random seeded_pool_lifecycles -- --exact --nocapture` | Direct replay passes |
| `GWZ_POOL_MC_SEED=0x202609195eed GWZ_POOL_MC_CASES=50000 cargo test --locked --release --test pool_random extended_pool_lifecycles -- --ignored --exact --nocapture` | 50,000 v2 cases pass on Rust 1.96 |
| `cargo package --locked --allow-dirty` | Standalone package builds; only unrelated workspace work was excluded |
| `cargo +1.95.0 test --locked --manifest-path target/package/gwz-transport-0.1.0/Cargo.toml --test pool_regressions --test pool_async` | Twelve extracted-package regression/async tests pass |

The fixed v2 extended run exercised 1,220,602 connects, 13,332 reuses, 357,050
leases, 192,899 connector cancellations, 204,345 late successes, 396,006 closes,
623,407 aborts, 1,768,074 blocked allocations, 214,931 helper interactions,
985,681 timeouts, 412,385 spontaneous idle disposals and 682,598 session
cancellations. These are one run's event counts, not unique-path coverage.

Generator `gwz-transport-pool-v2` uses independently derived case seeds, prints
failure configuration, clock, step, recent trace and an exact replay command,
and repeats every sixteenth case to compare trace digest and coverage. The
fake host maintains its own resource ledger and checks capacity including
unfinished cleanup, identity, exclusive allocation and full teardown. Preserve
the source revision with any seed because generator changes alter its meaning.

Both reviewers independently passed the 66-test suite, fmt, Clippy and direct
replay on Rust 1.96. Their separate 10,000-case v2 campaigns also passed: Code
seed `0xc0de1d1e`, State seed `0x51a7e20260919`. See their reports for event
counts and exact source/line evidence. No native Windows or physical network
execution was performed.

## Remaining scope

This is an in-memory pool implementation checkpoint, not an API/schema freeze,
Phase 1/2 completion or production transport activation. The pre-existing taut
external-type generator prototype and test-only core consumer remain excluded
unfinished work. No SSH/HTTPS adapters, real carrier, remote provisioning,
publication, network speed measurement or native Windows qualification is claimed.
Next integration work must finish the exported-schema/shared-type proof and
complete the planned runtime/pool interface gate before dependent adapters.
