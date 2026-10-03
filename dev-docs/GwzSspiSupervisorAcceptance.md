# SSPI parent supervision acceptance

2026-10-03. **Accepted at root 255d05e09c0433e27dd7afeaa9f9fe04699cad05,
gwz-sspi a75485cbdd03607909d11637c07f97548dd7902c, unchanged reference core
8cb3a3f01d79699a5ad07b6ec7cfc78321224d31 after independent Code/State/Surface
GO. This accepts parent supervision only.**

Verbatim verification reports:

- [Code](GwzSspiSupervisor-ReviewCode-1.md)
- [State](GwzSspiSupervisor-ReviewState-1.md)
- [Surface](GwzSspiSupervisor-ReviewSurface-1.md)

Implemented: owned async Supervisor/Conversation API and support values,
immutable deadline/cancellation and first-terminal arbitration, strict private
phase/round bridge, FIFO admission, charged dispatch/launch/read/write/reaping,
retained quarantine, checked IDs and bounded cleanup tombstones, Windows primary
and originating-thread identity, creation-time Job/handle-list containment and
anonymous pipes. Existing taut wire/schema/IR/fingerprints remain unchanged.
No dependency on core, Git, transport, CLI, Python or an async runtime.

One merged [remediation](GwzSspiSupervisor-RemPlan.md) closed three blocking P2s:
receiver-lifetime capture of owned futures, terminal error projection after record
reaping, and retained completed-step challenge bytes. The original reviewers
independently checked their original counterexamples on corrected bytes. Code
and State also converged on missing production ownership-bridge coverage; new
fake-port/owned-completion tests exercise the same bounded iterations used by
native drivers. The synchronous pre-registration captured-handle disposal
exception and trusted host fingerprint provisioning are now explicit.

## Evidence and its limits

Owner Darwin gates pass: full Rust tests (60 unit, three integrations, three
compiled examples and sixteen negative trait doctests), strict all-target and
all-feature Clippy, Cargo and all-source formatting, 61-file disabled-branch
scope scan, pinned 16-artifact generation check, standalone crate verification
and extracted-archive full Rust tests. Drafter also passed 13 Python schema tests.
Windows MSVC and GNU all-target/all-feature strict Clippy pass by cross-compilation.
Counts describe observed runs; no expected-pass census gate was introduced.
Build caches remain outside repositories. Public CI is authored, not remotely run.

Deterministic tests reproduce the failure cases; seeded kernel and production
reaper schedules record their seed and action trace. Fake OS/IPC/completion ports
establish parent decisions, not Windows runtime behavior. Actual native identity
changes, inheritance/Job behavior, stalled-I/O cancellation, handle cleanup and
provider disposal remain unqualified.

Final member aec1b9c65b75ad53dd3ae780fc1e04af18de795c changes only the packaged
implementation-status document after acceptance. Reviewed executable/API/wire
bytes are exactly those of a75485cbdd03607909d11637c07f97548dd7902c. Final root
landing files reports, prompts and acceptance/status records; it changes no
reviewed member behavior.

## Review ledger and next phase

Recorded tier: dual peer-blind Code/State plus cold Surface. Initial verdicts:
Code NO-GO, State NO-GO, Surface GO. One remediation round, then all three GO;
zero open P0–P3. Settled review discovered three distinct P2s and four P3 records
representing three distinct P3 causes (ownership-bridge coverage converged).
Implementation contact corrected five recorded concern groups: notification
ownership, failed-start retention, fresh clock arbitration, caller provenance,
and blocking dispatch/resource retirement. Reviewers found no new architectural
root cause or material contract change. Escaped defects observed: zero. Work was
one continuous checkpoint across context resumptions; no reliable wall-time
measurement was retained.

Next is plan step 3: native SSPI and worker bootstrap, native credential/context
and UTF-16/provider-buffer ownership, Windows fixtures, then the mandatory dual
secret-disposal review. CLI/Python/core composition and aggregate Windows
qualification follow. The production worker still refuses. Publishing remains
disabled; full Windows release remains NO-GO. No push, tag, registry release or
product activation occurred.
