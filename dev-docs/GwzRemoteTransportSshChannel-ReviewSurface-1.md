# SSH Channel — Surface Re-review

**Date:** 2026-09-20  
**Axis:** Surface  
**Object:** Phase 3c changed ownership and disposal API, remediation round 1.

**Verdict: GO**  
**Open findings:** P0: 0 · P1: 0 · P2: 0 · P3: 0

## Exact tuple

| Repository | Commit |
|---|---|
| root | `6076c6153f2b4fb74da5179b0ec6ffd2765d81ad` |
| gwz-core | `f03f5f79bae73d378e575273af0b9ed2a87c052d` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` |

All commits matched at start and end. Member working trees remained clean. Root contained only three authorized current-round prompts; peer contents were not read.

## Prior-finding closure

| Prior Surface result | Re-review disposition |
|---|---|
| GO with no findings at core `b77f4fef5dbb6958789dd8160fbb74eb67a3f47e` | No inherited Surface finding to close. Changed API independently reassessed below. |

This report does not adjudicate other reviewers’ findings.

## Evidence

Read the current [public README](/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/transport_ssh/README.md), its diff over correction range `b77f4fef5dbb6958789dd8160fbb74eb67a3f47e..f03f5f79bae73d378e575273af0b9ed2a87c052d`, the supplied review prompt, and my previous Surface report.

No implementation, test source, internal remediation plan/design, or peer report was read.

Executed:

```sh
cargo +1.95.0 test \
  --manifest-path gwz-core/tests/transport_ssh/Cargo.toml \
  --offline --locked --tests -- --nocapture
```

**All five tests passed; none failed or were ignored:**

- `upload_pack_advertisement_and_abort_keep_cleanup_explicit`
- `receive_pack_reuses_one_authenticated_connection_and_quotes_repository`
- `poll_dispose_completes_or_waits_without_releasing_session`
- `flush_preserves_buffered_upload_pack_advertisement`
- `stalled_disposal_retains_then_forces_owned_connection`

No edits or commits were made.

## Changed-range and API analysis

**Construction now retains shutdown ownership.** The README replaces direct Session transfer with `SshConnection::new(TcpStream)`. It explains setup access, trust-before-authentication, switching both socket and Session to nonblocking mode, and transferring the complete owner. Prohibitions on retained native clones, channels, and listeners make the exclusivity obligation explicit.

**Disposal now has an observable acknowledgment.** The documented stalled-peer sequence is: abort ordinary work; retry `poll_dispose`; retain ownership across `WouldBlock`; use readiness information; force disposal at the host’s deadline; reclaim pool capacity once after success. A shutdown error retains ownership and does not acknowledge disposal. The named stalled-disposal regression passed, re-exercising this failure scenario without treating documentation alone as proof.

**Healthy extraction preserves the complete owner.** `into_session` explicitly returns the whole `SshConnection` only after successful finish; unsuccessful extraction retains the channel owner. Native failure prevents reuse.

**Flush preserves incoming data.** The documented `Write::flush` checks active state and otherwise does nothing because accepted writes have reached the native channel. It does not discard input. The native advertisement-preservation regression passed.

**Fixture prerequisites and effects are disclosed.** The README adds `ps` and `kill`, describes pausing only the fixture’s server processes, and states that cleanup resumes them. Process-tree internals were not independently inspected.

## Findings and invariants

No new Surface finding or architectural root cause was identified.

The public contract distinguishes EOF, readiness waits, and native errors; leaves deadlines with the host; documents path limits; and supplies construction, finish/extraction, abort/disposal, and fallback-drop lifecycle steps. Disposal idempotence, post-disposal readiness, and exactly-once capacity reclamation are explicit. Early Drop is a fallback rather than an observable disposal acknowledgment.

## Residual risks and next action

Passing the five tests supports this public-contract review; it is not an independent native-code or cleanup-guard audit. Production authentication, trust resolution, pooling, message-pump activation, publication, and platform parity remain deferred.

**Next action:** Record Surface GO at this exact tuple. No Surface remediation is required.
