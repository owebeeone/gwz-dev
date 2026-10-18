# SSH N3 aggregate backend attachment — Code-AXIS REVIEW

**Review object:** Aggregate N3 implementation at core `2f12bbd63f748d836cd3ff858c87603e745788c9`, transport `a6562e654b52705b72ef1f793ae2045c320cee47`, controlled by `gwz-core/dev-docs/GwzRemoteTransportSshN3.md`; draft candidate checkpoint, 2026-09-22.  
**Baseline:** Root `2e5c06c38bf161a852afefc7c0307226b6862212`; core `2f12bbd63f748d836cd3ff858c87603e745788c9`; transport `a6562e654b52705b72ef1f793ae2045c320cee47`; evidence `3dad43c66544486786a866d6f5b1bc6f5f788077`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from these immutable objects with `git show`/`git diff`. Commit and tree identities were verified unchanged at the start and end.  
**Date:** 2026-09-22  
**Axis:** Code architecture, interfaces, call graphs, compatibility, and failure semantics. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 correctness/parity findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified, provided the focused closure tests pass without changing the accepted scope.

---

## 0. Evidence base

I inspected the controlling N3 draft and retained production-setup, selected-identity, transport-design, and transport-plan contracts; the complete core and transport review ranges; and the pinned implementations of:

- backend runtime binding, transport callback construction, clone/fetch/tag/pull/advertisement/manifest/push funnels, nested driver reporting, and candidate tests;
- `ssh_endpoint`, `ssh_local`, `ssh_worker`, `ssh_setup`, `ssh_pump`, `stream_io`, `ssh_remote`, `agent_auth`, and selected-key authentication;
- request-correlated pool progress and `Checkout::opening_connection()`;
- private-member materialization and `clone_error` refusal classification;
- candidate harness containment and private evidence run `2026-09-22-backend-n3`.

The isolated candidate backend gate passed 5/5. The focused transport `pool_async` gate passed 7/7. Archived evidence records backend 5, ordinary backend 8, isolated SSH 123 with one ignored, and transport 94 with two ignored; it correctly limits those claims to local candidate composition.

## 1. Findings

### [P2-1] The pump discards remote service diagnostics required for SSH refusal parity

**Location:** `gwz-core/src/git/endpoint/ssh_pump.rs:304-338,383-389`; `ssh_worker.rs:632-662`; `stream_io.rs:53-68`; `ssh_remote.rs:79-87`; `gitbackend/transport.rs:769-797`; `workspace_ops/handle_materialize/apply.rs:68-84`.

`SshPump` retains bounded stderr, but a nonzero remote command status becomes only `PumpError::Invariant("backend service failed")`. The worker then drops the `PumpError`, disconnects both stream ends, and the Git-facing adapter receives a generic stream/network failure. The retained stderr and exit status never reach `clone_error`.

Concrete sequence:

1. SSH authentication succeeds.
2. `git-upload-pack` refuses a missing or inaccessible repository, writes a canonical diagnostic such as `ERROR: Repository not found.` to stderr, and exits nonzero.
3. The pump drains that diagnostic, then replaces it with the generic invariant and disconnects.
4. The resulting libgit2 error lacks the SSH class/message recognized by `clone_error`, so it becomes `GitCommandFailed` rather than `RemoteRejected`.
5. An explicitly private workspace member therefore aborts materialization instead of being quietly omitted and having its observation forgotten.

This violates the draft’s failure-classification/parity requirement and changes retained private-member behavior.

Carry a bounded, sanitized service-terminal diagnostic or an equivalent typed refusal through the local candidate stream boundary. Canonical SSH repository refusals must reach the clone boundary as SSH-class refusal information; unrelated nonzero statuses must remain generic failures. This correction need not add a new physical carrier or broaden activation.

Closure requires a real candidate-backend clone to an inaccessible or nonexistent loopback repository that produces `RemoteRejected`, plus a private-member materialization test proving omission and observation removal. A noncanonical nonzero service failure must still produce `GitCommandFailed`, and diagnostic retention must remain bounded.

### [P2-2] Per-key observation state is incorrectly used as the terminal operation error

**Location:** `gwz-core/src/git/endpoint/agent_auth.rs:49-55,79-134`; `ssh_local.rs:44-52,64-71`; `ssh_endpoint.rs:71-84`; `agent_job.rs:32-59`.

Every explicit `AUTHENTICATION_FAILED` response immediately writes `facts.authenticated = Some(false)`. `Route::open` later converts *any* error into `AuthenticationRejected` whenever that observational field remains false.

Concrete sequence:

1. The agent supplies at least two identities.
2. The server rejects the first key, setting the shared fact to false.
3. Cancellation or the absolute deadline fires at the following `control.check()`—either immediately after the native call or before the next key’s `offered()` resets the fact.
4. Authentication correctly returns `ConnectionAborted` or `TimedOut`.
5. `Route::open` discards that terminal cause and returns `AuthenticationRejected`; `RemoteTransport` consequently reports libgit2 Auth/Ssh instead of cancellation or timeout.

Per-key facts may record that a refusal occurred, but they cannot serve as the terminal-cause oracle. Produce a typed terminal authentication-exhaustion result, or otherwise classify from the returned cause. Only exhausted explicit refusals should become `AuthenticationRejected`; timeout, cancellation, signer, agent-I/O, and transport failures must retain their own disposition.

Closure requires a deterministic two-key case where key one is rejected and cancellation or deadline occurs before key two is offered: the result must remain cancellation/timeout and must not be Auth/Ssh. Exhausting explicit key refusals must still map to authentication rejection, and a later successful key must still publish authentication success.

## 2. Invariant analysis

The attacks on endpoint and operation ownership otherwise held. Backend-family clones share one lazy endpoint; `with_transport` creates fresh identity and observation scopes; nested driver responses merge without duplicate rows. Changed network funnels construct fresh remotes and add transport callbacks without dropping progress or push callbacks. Local and non-SSH routing remains native.

Request correlation also held: `opening_connection()` exposes an identifier only while Opening and grants no resource capability; waiting, ready, failed, and consumed states return none. Setup facts are associated before connection work, successful reuse suppresses a new credential-offer claim, and published operation rows are copied rather than remaining mutable by late helpers. The focused pool test confirmed these state boundaries.

Candidate compilation remains explicitly contained, ordinary dependency routing is unchanged, and the evidence does not claim production activation, platform/source qualification, or a full-core pass. The blocking defects are confined to terminal failure propagation and classification.

## 3. Risks and next action

Platform, selected-source, publication, production activation, HTTPS, and CLI placement remain valid deferred work. Existing evidence establishes local candidate composition but does not exercise the two counterexamples above.

The next action is one bounded remediation that preserves remote service refusal information through the candidate adapter and separates terminal authentication cause from per-attempt facts, followed by the specified focused regressions and a changed-range re-verdict.
