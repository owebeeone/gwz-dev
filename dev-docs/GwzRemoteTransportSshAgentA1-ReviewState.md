# SSH Agent A1 — State-Axis Review

**Review object:** A1 implementation checkpoint at root `6da3ce07e0ccbca36eec4bbf23b9315add645f98`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportSshAgentA1.md` and implementation at core `309696601227494f60942aea96124ca4eaeebda5`, dated 2026-09-21.  
**Baseline:** root `6da3ce07e0ccbca36eec4bbf23b9315add645f98`; gwz-core `309696601227494f60942aea96124ca4eaeebda5`; gwz-core-evidence `0792238da91b2466ca8ce9dc752e9dc901c0bad4`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Helper, socket, and codec state machines under timeout, cancellation, abandonment, failure, and recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 recovery finding blocks A1 acceptance. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I read the complete prompt, accepted helper design, A1 checkpoint document, and all new source and focused tests:

- `agent_job.rs` lines 1–288.
- `agent_client.rs` lines 1–205.
- `agent_socket.rs` lines 1–57.
- `agent_capacity.rs` lines 1–106 and `agent_client.rs` tests lines 1–454.
- Fixture manifest/lock changes and private `agent-a1` README, inputs, development logs, and final source hashes.

I reran the authorized Rust 1.95 offline locked fixture using the external target directory. All 52 executions passed: 14 A1 tests and 38 retained transport tests. The 64 fragmentation seeds printed, and the existing ignored agent child was executed by its passing parent. Committed source hashes match the evidence manifest.

The exact tuple matched at both review boundaries. Root status contained only the two explicitly excluded generated prompts.

## 1. Findings

### [P2-1] A transient first supervisor-spawn failure permanently disables agent setup

**Location:** `src/git/endpoint/agent_job.rs` lines 125–163, especially `static HUB: OnceLock<Result<Hub, io::ErrorKind>>` and `HUB.get_or_init(...)`.

The process-global initializer stores both successful `Hub` values and initialization errors. Therefore an error spawning the first `gwz-setup-reaper` thread is permanent:

1. The first `Job::start` calls `Hub::global`.
2. Reaper-thread creation fails transiently, for example because the process or host temporarily cannot create another thread.
3. `OnceLock` stores `Err(error.kind())`.
4. The resource condition clears.
5. Every later `Job::start`, including from recreated endpoints, reads the cached error. No new reaper-spawn attempt occurs, and no helper can ever start until process restart.

This creates a process-lifetime stuck state from a recoverable setup error. It contradicts the design’s thread-creation-failure contract: failure must release reservations and return a setup error, rather than irreversibly poisoning future setup. It also makes endpoint recreation ineffective for a reason unrelated to a live or quarantined helper.

The existing injected spawn-failure test does not cover this path. `start_with` calls `Hub::global()` before invoking its injected helper-thread spawner, so the test proves helper permit recovery only after the supervisor already exists.

**Required correction:** publish only a successfully created global supervisor. A failed supervisor initialization must leave initialization retryable while preserving the single-supervisor invariant under concurrent callers. It must not increment helper capacity or retain a job record.

**Closure test:** add a deterministic supervisor-construction seam that fails the first reaper spawn and succeeds on the next attempt. Assert that the first job returns the injected error with capacity unchanged, a later job starts, completes, joins, and releases its permit, and concurrent retries cannot create multiple supervisors.

## 2. Invariant analysis

**Job arbitration otherwise holds.** Deadline, cancellation, publication, claim, and disposal share the state lock. Cancellation is monotonic; exact deadline sets `TimedOut`; cancellation before deadline sets `ConnectionAborted`. A result cannot be claimed until the helper has finished and the supervisor has joined it.

**Abandonment and cleanup ownership hold.** Dropping a job cancels it but leaves the supervisor’s entry, JoinHandle, completion cell, and permit intact. Late success is destroyed outside the arbitration lock. Cleanup overrun returns an error without acknowledging disposal. The 64-helper test proves abandoned work remains charged until exit, join, and result destruction, after which fresh capacity becomes available.

**Helper-thread spawn failure holds.** Once the supervisor exists, injected helper-spawn failure drops the local permit, starts no work, and does not reduce later capacity. This does not close P2-1’s earlier global-initialization path.

**Codec state holds.** Frames are length-checked before body allocation; identities, key blobs, comments, signing inputs, methods, and signatures have finite bounds. Parsing rejects truncation, trailing data, wrong message types, algorithm mismatch, empty signatures, and oversized inputs. After any exchange failure the agent is poisoned, preventing replay. Errors do not include reply payloads or socket paths.

**Socket state holds within claimed scope.** The Unix socket is nonblocking before connect. Connect, partial reads, and partial writes repeatedly check the absolute deadline and cancellation, with waits bounded to 20 ms. Native peers observed EOF after cancellation. The document accurately excludes an executed pending-connect and native send-buffer-exhaustion proof on this host.

**Scope holds.** These modules remain fixture-only. A2 signing/session transfer, A3 pool and endpoint integration, Windows/Linux primitive qualification, selected-source reconstruction, and production activation are not claimed.

## 3. Risks and next action

A2 must still prove bounded non-panicking destruction for its concrete native result owner; A3 must retain physical capacity and make cleanup failure observable. Those declared later gates do not block this isolated review.

Correct P2-1 and add the supervisor-initialization retry regression. The focused re-verdict can then inspect that initialization transition and its interaction with the existing helper permit and global-cap tests.
