# Placement B candidate aggregate — CODE-AXIS REVIEW

**Review object:** Phase 4 Placement B candidate aggregate at gwz-core `6b9be8a2634700a3f47483beb99a582e7acd20b3`, controlled by `dev-docs/GwzRemoteTransportPlacementB.md`; implemented candidate pending acceptance, 2026-09-22  
**Baseline:** root `ba32b3af11c08f4c6b0a6897b158d5a41ed2465b`; core `6b9be8a2634700a3f47483beb99a582e7acd20b3`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `9980a5d2199df09b055e677e07123f209900ea8c`. Sources were read from immutable objects with `git show`/`git diff`.  
**Date:** 2026-09-22  
**Axis:** Code—architecture, interfaces, call graphs, compatibility, failure paths, and claims versus implementation. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified, absent a directly introduced regression.

---

## 0. Evidence base

I inspected the complete core range `d8e23f63..6b9be8a`, especially `src/transport_host/{mod,request,session,session/driver}.rs`, `src/git/endpoint/{placement_endpoint,ssh_worker,ssh_pump,stream_io,agent_job}.rs`, backend transport binding and identity resolution, every changed workspace network-funnel call site, and the host, command, fault, placement-endpoint, preparation, and guide fixtures. I checked the controlling Placement Design/B documents, `docs/TransportPlacement.md`, and the current checkpoint.

The facade ownership, binding, route isolation, typed refusal, pool reuse, cancellation, and ordinary-handler paths were traced source-to-source. No test command was needed or run; recorded passing gates were treated as owner evidence rather than independently claimed execution.

The complete commit/tree tuple matched at both boundaries. Member worktrees remained clean; root had only generated current-round/previously allowed review artifacts. No current peer report was read.

## 1. Findings

### [P2-1] Fetch validates override names but skips effective per-repository identities before parallel network work

**Location:** `src/workspace_ops/handle_fetch.rs:89-109,369-424`; `src/git/gitbackend.rs:261-284`; `src/git/gitbackend/transport_support/identity.rs:215-296`.

The claimed invariant is that every selected identity is checked before any target performs network work or mutates remote-tracking refs. Fetch builds every target and calls `validate_transport_remotes`, but that method only rejects unused invocation override names. Unlike pull, tag, push, and materialize, fetch never calls `validate_remote_identity` for each target. Repository-local `remote.<name>.gwzSshIdentity` is therefore discovered only inside the target’s eventual `fetch`.

Concrete counterexample: select two materialized repositories under CLI placement with concurrency one. Give the first a valid changing SSH remote and the second an absolute, missing repository-configured identity. Invocation identity options are empty, so `with_transport` has nothing to check. The name-only preflight succeeds; the first fetch updates its tracking ref; the second then fails when `for_remote` checks its configured key. The operation has already produced the partial mutation that whole-operation preflight promises to prevent.

Before dry-run return or worker dispatch, fetch must call `validate_remote_identity(path, remote, false)` for every contactable target after validating the complete remote-name set. Add a handler regression with the invalid configured identity on the final target and serialized execution; assert zero endpoint opens and unchanged tracking refs for all targets.

### [P2-2] Endpoint identity checking can wait forever after its advertised deadline

**Location:** `src/git/endpoint/ssh_worker.rs:629-646`; `src/git/endpoint/placement_endpoint.rs:448-506`; `src/git/endpoint/agent_job.rs:272-291`.

`start_identity_file_check` uses blocking `File::open` before determining that the object is a regular file. A FIFO with no writer therefore blocks the setup thread. When the check deadline expires, `finish_checks` switches to `poll_disposed`, but emits a timeout only after disposal returns `Ok`. An overdue still-blocked job returns `Err`; that branch is converted back to “no result,” so no `IdentityCheckFailed` is sent. `Session::check`, and consequently handler preflight, can remain blocked indefinitely despite the documented 120-second bound.

Make special-file admission nonblocking, and separate logical deadline completion from physical disposal: emit the one timeout terminal at the deadline while the cleanup owner retains any uninterruptible job. Add a FIFO-without-writer regression and an injected non-returning check job; each must produce a single bounded failure, while cleanup truthfully accounts for retained physical work.

## 2. Invariant analysis

The remaining attacks held. Candidate code and generated-schema selection remain isolated under the Unix candidate boundary. Request metadata and operation identity are checked at changed public handler funnels, and explicit CLI placement cannot fall back to the local native route. Endpoint-relative identities remain opaque in core and are resolved at the endpoint. Bind registration, last-port drop, cancellation, and cleanup ownership are bounded and do not claim peer cleanup. Open/stream queues and setup fan-out are bounded. The worker retains physical SSH ownership while the endpoint bridge carries typed envelopes, and canonical repository refusal crosses that bridge with facts and maps back to the retained Git classification. Default production routing remains outside the candidate path.

## 3. Risks and next action

Carrier construction, split-process/platform qualification, selected-source qualification, HTTPS, production activation, and release remain explicit later gates. The next action is one bounded correction adding fetch-wide effective-identity preflight and deadline-independent identity-check completion, followed by the two counterexample regressions and focused changed-range re-review.
