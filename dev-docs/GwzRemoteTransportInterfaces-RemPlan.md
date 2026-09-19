# Transport interface candidate — merged remediation 1

Date: 2026-09-19. Status: implementation authorized; closure pending reviewers.
Baseline: root `9bb74ace2fca56f770af34b6ec1bdf6bab4e695e`, core
`ce3bb967f9696b1791fcbefb65b6ccdd0c8ab618`, transport
`163feebe439edd5d1fbaf11e163882e80d4f2257`, taut unchanged
`733e8a78897a90f017f4726e4331aed95e8cb977`.

Code returned NO-GO (one P2, one P3); State and Surface returned GO (one P3
each). Reports are filed verbatim. No blind convergence on the blocking root.
One merged patch addresses all four findings; no schema, public signature,
physical carrier, production dispatcher, native adapter or credential change.

| Finding | Disposition | Closure test / reviewer |
|---|---|---|
| Code P2-1 negotiated Open admission | Accept: Binding::check_open uses installed limits; canonical consumer handoff uses the same limits and returns rejection before effects. | Owner regression plus typed/encoded consumer metadata 256/257 boundary, operation/destination/identity fields, zero effects for rejection and existing capability/ownership tests; Code verifies. |
| Code P3-1 native timeout composition evidence | Accept: fake host captures endpoint connect/I/O policy, resolves deadlines before effects, builds exact pool/stream inputs, and transfers remaining helper allowance. Native policy stays host-owned. | Finite rejects zero/longer, accepts equal/shorter; disabled accepts zero/positive; maximum preserved; rejected effect count zero; cumulative helper proof; Code verifies. |
| State P3-1 clock-only wake amplification | Accept as bounded ride-along: separate readiness changes from clock bookkeeping without new notification API. | Counting-waker tests: quiet pool ticks and stream progress/classification do not wake; exact expiry/cleanup and genuine readiness still wake; State verifies. |
| Surface P3-1 undocumented Python environment | Accept as documentation ride-along: give environment creation/dependency/toolchain prerequisites, preserving pinned source verification. | Fresh temporary environment follows setup and passes consumer regeneration/archive proof; Surface verifies. |

Ownership: retained economical integration drafter owns only consumer admission
fixtures (up to 400 new test LOC, with cohesive helper if needed); retained
runtime drafter owns clock notifications and counting-waker tests (up to 100
production / 250 test LOC). Lane owner owns binding correction/regression,
documentation, evidence and commits (up to 10 production / 150 test LOC).
Existing async ownership and timeout semantics remain fixed; no new production
owner or framework. Readiness changes are required to retain wakeups.

TDD: each behavioral correction starts with a genuine failing regression against
the candidate. Record executed failures, not reconstructed claims. Run focused
checks, complete owner suite, regeneration and isolated archive proof after the
merged patch. Package only the committed owner source and record its new digest.

Re-review: same original Code and State reviewers, plus the same Surface
reviewer for its docs-only closure; exact revised root/member tuple, changed
range and closure tables. User override to reuse original reviewers applies.
This is the first remediation round for the interface object; two-round cap
unchanged. Do not declare acceptance until the blocking reviewer verifies closure.

## Implementation evidence

Binding regression `open_binding_enforces_negotiated_metadata_before_effects`
failed before the production edit: operation metadata length 257 was accepted
under a 256-byte binding. After changing `check_open` to limited admission,
all owner binding/policy tests pass. Consumer typed and actual bounded encoded
receiver paths now share negotiated policy and reject before the single fake
host effects increment. The broad schema fixtures explicitly capture disabled
network policy; a separate matrix drives the same host boundary with finite
and disabled policies, checking connect and I/O independently and carrying the
policy-capped helper remainder into stream configuration. Identity resolution
uses an explicitly fake proof, never the supplied key path as a reuse proof.

The runtime drafter recorded genuine failing wake regressions before its edits:
pool wakes increased 1 to 2 on a quiet tick; stream bookkeeping increased wakes
0 to 4. A parent extension also failed before correction: pool helper begin/end
increased wakes 1 to 3 despite no readiness change. The corrected runtime leaves
clock-only bookkeeping quiet, while expiry, cleanup actions and real data still
make progress. Cleanup abort/abort-connect boundary tests pass. A pending
PoolDriver receiver borrows it mutably, so a public host cancels that receiver
before ticking; no impossible simultaneous receiver/advance borrow is claimed.
Stream read/dispatcher use independent counting wakers. The existing async
expiry tests continue to verify terminal wakeups.

The host-policy proof adds test-only sequencing/coverage; its first run against
the existing runtime was green. No pre-implementation runtime red is claimed
for that evidence-only change. The fresh Python setup was reproduced in a
new temporary environment with only taut-proto==0.9.1 installed; source-pinned
consumer regeneration passed. Final clean-archive qualification and reviewer
closure are recorded in the interface checkpoint.
