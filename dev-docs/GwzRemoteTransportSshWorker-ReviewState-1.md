# Shared SSH worker remediation 1 — State-Axis Review

**Review object:** Shared-worker remediation 1 at root `60623f2a10895dd970595c5b818a68e5900a72a3`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportSshWorker.md` at `073395b5a265c4d2a60265470cd5ba173243cc64`, dated 2026-09-21.  
**Baseline:** root `60623f2a10895dd970595c5b818a68e5900a72a3`; gwz-core `073395b5a265c4d2a60265470cd5ba173243cc64`; gwz-core-evidence `5ea96433628b3bec8a8365525f8ee314d01cce99`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Corrected core was compared with `59ac059c8bfe4052d8a5a16d821f314a3985bcc8` using immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Request state, deadlines, cancellation, admission ownership, shutdown, routing admission, and failure isolation. Independent, adversarial, read-only. The other axis ran independently; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings. State P2-1 is closed, and the adjacent SCP correction introduces no State-axis defect.

---

## 0. Evidence base

I read the complete re-verdict prompt, merged `GwzRemoteTransportSshWorker-RemPlan-1.md`, the filed prior State report, the controlling draft’s remediation section, and the complete core diff from `59ac059c…` to `073395b…`.

Source inspection covered:

- `ssh_worker.rs` lines 79–99, 105–228, and 249–423.
- New `support/worker_queue.rs` lines 1–125 and its inclusion in both route and worker fixtures.
- The complete destination-parser, destination-test, and route-test changes.
- Private `worker-rem-1` README, inputs, red log, final log, and recorded source hashes at evidence commit `5ea9643…`.

The archived red State run reproduces the original counterexample as `BrokenPipe` instead of `TimedOut`. I reran the permitted Rust 1.95 offline locked gate with the external target directory. All 38 test executions passed; the ignored agent child remains explicitly driven by its passing parent. Recorded source hashes match the corrected committed blobs.

The exact six-repository tuple matched at both review boundaries. Final root status contained only the two explicitly excluded generated prompt files.

## 2. Invariant analysis

**Exact and overdue queued expiry now classify correctly.** The receiver-drain path first checks actual worker shutdown and returns `BrokenPipe` only when `stopping_at` is set. It separately checks `request.expired(now)` and returns `TimedOut`. Because expiry uses `now >= deadline`, the exact boundary and all later times take the timeout branch.

**The original scheduler sequence is closed.** The regression enqueues a request before starting the worker, with the injected monotonic clock exactly equal to its deadline. It then repeats with the clock past the deadline. Both return `TimedOut`, neither invokes the connector, both release the admission permit, and neither sets the shared stop flag. This removes thread-scheduling ambiguity from the original counterexample.

**Caller-timeout cancellation cannot invent endpoint shutdown.** A queued request marked cancelled also returns `TimedOut`. In production, that flag is set when the blocking caller’s timed wait ends; the result may consequently have no receiver, but `complete` still drops the permit before attempting reply publication. Disconnection and actual shutdown retain their existing stopped classification.

**Continued service is demonstrated.** After exact expiry, overdue expiry, and cancellation, the regression enqueues a live request. The worker reaches the connector exactly once, returns the connector’s ordinary error, releases its permit, and remains unstopped. Thus expired work neither poisons the worker nor consumes future admission.

**Shutdown precedence remains coherent.** Once shutdown begins, pending requests are completed as stopped, active callers disconnect, the pool shuts down, and newly drained receiver entries take the stopped branch before expiry classification. A request that is both expired and overtaken by real endpoint shutdown therefore reports the actual terminal state without reopening admission or physical ownership.

**The private clock seam preserves production semantics.** `run` now accepts a private clock closure; production supplies `elapsed(origin)` from the same `Instant` origin used to construct request deadlines. No public interface, timeout arithmetic, pool ownership, or activation path changed.

**Adjacent admission changes remain fail-closed.** The SCP parser now admits the pinned bracketed host, user, port, IPv6, and root-operand forms. Malformed brackets, invalid ports, Windows paths, and empty operands are still rejected before identity resolution or connector effects. Accepted forms retain current identity resolution on every open and preserve host, port, user, and repository operand separation.

## 3. Risks and next action

Production credential setup, callback/dependency activation, and the deferred platform/source qualification batch remain outside this checkpoint and are still accurately described as pending.

The State axis accepts remediation 1. The next action is aggregate acceptance, followed by the separately bounded production credential and activation work.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State P2-1 | Split real shutdown from queued expiry; classify exact/past deadline and caller cancellation as `TimedOut`, release admission, and preserve worker service. | Archived red run reproduces `BrokenPipe`; corrected exact, overdue, and cancelled sequences return `TimedOut`; permits reach zero; stop remains false; subsequent connector dispatch occurs. | **Closed** |
| Code P2-1 | Restore pinned native SCP authority and root-path compatibility. | Changed-range State inspection confirms accepted vectors reach normal identity/connector admission while malformed and local-path vectors remain effect-free. Code-axis closure remains independently owned by that reviewer. | **No State blocker** |

## Changed-range analysis

The core range changes six files: the controlling draft, destination parser, worker loop, new deterministic queue support, and destination/route regressions. The worker production delta is limited to supplying `elapsed(origin)` through a private closure and separating the shutdown and expiry branches. The test seam does not alter public API or ownership.

The SCP changes broaden recognized native syntax and add fail-closed route assertions; they do not change worker scheduling, pool keys beyond correctly parsed authority fields, identity lifetime, or physical connection ownership.

No change falls outside the merged dispositions, and no new architectural root cause was found.
