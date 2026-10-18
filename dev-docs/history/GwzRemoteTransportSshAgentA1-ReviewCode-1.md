# SSH agent A1 merged remediation 1 — CODE-AXIS REVIEW

**Review object:** `gwz-core` `14409399bc7404446200192ffaf585f9969eec49`, delta from `309696601227494f60942aea96124ca4eaeebda5`; controlling checkpoint `dev-docs/GwzRemoteTransportSshAgentA1.md`, status “remediation 1 implemented; retained focused re-review pending,” dated 2026-09-21.  
**Baseline:** root `d552bbda5c5b8c14291243cdf73b1c1955443222`; gwz-core-evidence `d5605a5ad81feff445d0d712940ba050c849dec3`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Code architecture, ownership, interfaces, lifecycle races, and changed-range compatibility. Independent, adversarial, read-only. Nothing here relies on the parallel current-round review.

**Verdict: GO** — Code P2-1 and P3-1 are closed. No new P0–P3 finding was established in the remediation or its direct interactions.

---

## 0. Evidence base

I read the complete re-verdict prompt, merged remediation plan, revised A1 checkpoint, the full `3096966..1440939` source/test diff, corrected `agent_client.rs`, corrected `agent_job.rs`, and the focused portions of `agent_client.rs` and `agent_capacity.rs` tests.

I inspected the private remediation README, red/focused/full logs, and `inputs.json`. Exact-object hashes independently matched for all three production files and both focused test files. The resulting counts are 557 production lines and 742 test lines, within the amended limits.

I did not rerun the optional suite. The archived exact-source evidence records the intentional initialization failure, the 15-test focused pass, and the 54-execution full pass with 16 A1 tests and 64 printed fragmentation seeds.

The tuple was verified at start and end. Root/core trees remained `507e0fb7287991ccbf6e64b0a610299d4001c58a` and `2ea5e984299c6f1f550c4f5a36fce47eb0261664`. All member SHAs remained pinned; member worktrees were clean. Root contained only the two permitted untracked `-1` prompts.

## 1. Prior-finding closure and changed-range analysis

| Prior finding | Status | Closure evidence |
|---|---|---|
| Code P2-1 — owned-channel extraction resets irreversible protocol state | **Closed** | `agent_client.rs:12-17` retains the channel as a private field, and `into_channel` is removed. Exact-tree symbol search found no remaining extraction interface. Tests now inspect an external shared write record. `agent_client.rs:472-500` proves successful or failed enumeration emits only one request, later unsupported/failed operations emit nothing, and channel destruction occurs exactly once. The retained partial-write regression confirms accepted bytes are not replayed. Rewrapping the live channel can no longer be expressed through the production interface. |
| Code P3-1 — missing publication/claim/join cancellation boundaries | **Closed** | `agent_client.rs:502-575` holds the helper after result publication, separately cancels before thread exit and after the completion wake proving join, verifies pending state until join/disposal, exactly-once result destruction, final `ConnectionAborted`, and completed disposal. Its success case proves dropping `Job` does not destroy an owner already transferred to the caller. |

The directly adjacent supervisor change also preserves the lifecycle invariants. `agent_job.rs:131-171` publishes only a successfully constructed `Hub`; the initialization mutex and second `HUB` check serialize retries and admit one singleton. `Job::start_with` obtains that hub before incrementing `COUNT`, so an initial supervisor failure consumes no helper permit. Helper-spawn failure still drops the local permit. The concurrent retry and subsequent 64-slot exhaustion/recovery test covers this interaction.

## 2. Invariant analysis

The prior counterexamples no longer reproduce:

- Agent protocol state and channel ownership now remain inseparable until drop. Enumeration remains single-use, failure remains irreversible, and a desynchronized channel cannot be extracted and reset.
- Cancellation wins over every unclaimed success boundary examined. Publication does not transfer ownership; join does not transfer ownership; only successful claim does.
- Result destruction remains outside the arbitration lock, exactly once, and before disposal becomes observable.
- Supervisor retry publication does not create multiple reapers or leak setup capacity.
- The amended checkpoint accurately describes the changed implementation and limits its claims to the local codec/client and generic helper lifecycle.

## 3. Risks and next action

A2 must still prove native signing ownership and destruction behavior. A3 still owns pool/backend integration, endpoint refusal, and physical-capacity accounting. Native pending-connect execution, send-buffer exhaustion, platform coverage, selected-source qualification, and production activation remain expressly deferred.

Accept remediation 1 for the A1 Code axis and proceed to the separately reviewed A2 checkpoint.
