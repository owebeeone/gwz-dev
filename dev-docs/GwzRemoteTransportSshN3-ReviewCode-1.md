# SSH N3 aggregate backend attachment — Code-AXIS REVIEW

**Review object:** N3 remediation 1 at core `c79c7f13aebfcf582d0df75cff469d452e3477f1`, controlled by `gwz-core/dev-docs/GwzRemoteTransportSshN3.md`; corrected draft candidate checkpoint, 2026-09-22.  
**Baseline:** Root `7f0a844b1bb851eedd3792eb13c0194b2231a190`; core `c79c7f13aebfcf582d0df75cff469d452e3477f1`; transport `a6562e654b52705b72ef1f793ae2045c320cee47`; evidence `36d29397faae5205e1e16812f9f573a122665b7f`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Corrected range: core `2f12bbd63f748d836cd3ff858c87603e745788c9..c79c7f13aebfcf582d0df75cff469d452e3477f1`; transport unchanged. Sources were read from immutable objects with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** Code architecture, interfaces, call graphs, compatibility, and failure semantics. Independent, adversarial, read-only. The current other-axis re-review was not read or used. Filed verbatim by the lane owner.

**Verdict: GO** — both original Code P2 findings and the consolidated State P2 finding are closed. No new P0–P3 finding or architectural root cause was found in the changed range.

---

## Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| Code P2-1: SSH service refusal lost through `SshPump` | **CLOSED** | `ssh_pump.rs:276-318,334-357` recognizes only complete, untruncated canonical stderr with no stdout and publishes a stream-local boolean before response EOF. `ssh_worker.rs:613-623`, `stream_io.rs:18-66`, `transport_binding.rs:106-110`, and `transport.rs:772-795` carry only the fixed internal marker to the candidate clone boundary. Real inaccessible-repository clone now returns `RemoteRejected`; noncanonical failure remains `GitCommandFailed`; private materialization quietly omits the member and removes its observation. |
| Code P2-2: per-key facts overrode later timeout/cancellation | **CLOSED** | `ssh_endpoint.rs:71-97` now requires both `authenticated == Some(false)` and the returned pool cause `ConnectFailed { code: Authentication, .. }` before producing `AuthenticationRejected`. The deterministic two-key regression preserves Net/timeout after first-key rejection; exhaustion remains Auth/Ssh, and retained later-key-success coverage passes. |
| State P2-1: transient endpoint construction permanently poisoned the backend family | **CLOSED** | `transport_binding.rs:14-71` replaces the cached `Result` with a family-shared `Mutex<Option<Endpoint>>`, serializes construction, and stores only successful endpoints. The originating caller receives the transient error; later callers retry. The regression proves one initial `WouldBlock`, one shared successful construction across concurrent family/scope callers, real pooled reuse, and separate operation observations. |

## Changed-range analysis

The correction changes 11 core files with 565 insertions and 41 deletions; transport, git2-rs, libgit2, and public protocol objects are unchanged.

The refusal receipt is scoped to one attached stream. It contains one `AtomicBool`, carries no remote text or authority, and is installed before the exchange enters the active set. Matching waits for stdout and stderr EOF, rejects any observed stdout or truncation, and accepts only the existing canonical repository-refusal forms. Normal nonzero exit still invalidates the channel, so the receipt cannot manufacture cleanup completion or pool reuse. Generic, truncated, and output-bearing negative cases are covered.

Authentication facts remain observational: a rejected key may leave `authenticated=false`, but terminal classification now also examines the typed pool failure. Timeout, cancellation, and nonauthentication connector failures therefore retain their actual disposition. Explicitly exhausted refusals still map to the retained Auth/Ssh contract.

Success-only runtime publication retains one shared endpoint for the backend family. Holding the mutex through construction prevents duplicate successful owners; failures leave the slot empty. Default construction performs endpoint-owner setup only and preserves the existing deferred claims around preflight boundedness and production activation.

No new physical owner, carrier, wire field, public API, dependency switch, or platform claim was introduced. Candidate cfg containment and the accepted aggregate line bounds remain intact.

## 0. Evidence base

I read the revised N3 draft, merged remediation plan, both prior reports, the full corrected diff, and the pinned final implementations and tests for:

- `transport_binding`, `transport`, candidate backend tests, and command-driver materialization;
- `ssh_endpoint`, `ssh_pump`, `ssh_worker`, `stream_io`, authentication reporting, and cleanup capacity;
- the remediation evidence archive `2026-09-22-backend-n3-rem1`, including source hashes and the actual red/green logs.

The archived reds reproduce the two Code counterexamples: timeout became Auth/Ssh, and canonical remote service refusal became `GitCommandFailed`. Final archived results are backend 7/7, default backend 8/8, and isolated SSH 126 passed with one extended campaign ignored. The unchanged prior transport result is 94 passed with two ignored. The evidence archive’s root source pin predates final report/evidence archival; its reviewed core and member source pins match this tuple.

I independently ran the permitted focused gates:

- candidate backend: 7 passed, 0 failed;
- `local_endpoint`: 10 passed, 0 failed;
- `pump`: 10 passed, 0 failed;
- `cleanup_capacity`: 1 passed, 0 failed.

`git diff --check` passed. The six commit and tree identities were verified unchanged at both review boundaries. The core, transport, evidence, git2-rs, and libgit2 members were clean; root contained only the explicitly excluded generated prompts.

## 2. Invariant analysis

The original counterexamples no longer reproduce. Repository refusal parity now reaches the only clone boundary that needs it, without widening classification to arbitrary stderr. Private-member omission retains its required observation cleanup. Per-key rejection can no longer erase a later timeout, while genuine authentication exhaustion remains distinguishable.

The concurrent runtime attack also held: failed construction creates no durable state, successful construction is published once, family clones share it, and operation observation scopes remain separate. The correction preserves existing backpressure, cleanup authority, local/native coexistence, and fresh per-remote lifecycle behavior.

## 3. Risks and next action

The fixed refusal marker is deliberately local to the candidate adapter. Future CLI-hosted placement still needs an admitted terminal-disposition mapping before activation. Platform, selected-source, publication, production activation, HTTPS, and CLI placement remain separate gates.

Accept N3 remediation 1 as the corrected local candidate checkpoint and proceed only through those deferred qualification gates.
