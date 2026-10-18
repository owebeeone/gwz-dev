# SSH Agent A1 Remediation 1 — State-Axis Review

**Review object:** A1 merged remediation 1 at root `d552bbda5c5b8c14291243cdf73b1c1955443222`; controlling draft and corrected implementation at gwz-core `14409399bc7404446200192ffaf585f9969eec49`, reviewed against core `309696601227494f60942aea96124ca4eaeebda5`.  
**Baseline:** root `d552bbda5c5b8c14291243cdf73b1c1955443222`; gwz-core `14409399bc7404446200192ffaf585f9969eec49`; gwz-core-evidence `d5605a5ad81feff445d0d712940ba050c849dec3`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Supervisor initialization, helper capacity, publication/claim/disposal races, and recovery-state closure. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — State P2-1 is closed; no new P0–P3 findings.

---

## 0. Evidence base

I read the complete re-verdict prompt, filed prior State report, merged remediation plan, corrected A1 checkpoint, core diff `3096966…1440939`, and private `agent-a1-rem-1` evidence.

Focused source inspection covered:

- `agent_job.rs` lines 125–242, including retryable singleton construction, initialization serialization, supervisor/helper spawn dispatch, permit acquisition, publication, and registration.
- Corrected `agent_capacity.rs` lines 19–167.
- The removal of agent channel extraction and the added publication/join/claim lifetime regressions in `agent_client.rs`.
- Archived initialization red evidence, final evidence manifest, source hashes, and focused/full passing logs.

I reran the authorized Rust 1.95 offline locked gate with the external target directory. All 54 executions passed: 16 A1 tests and 38 retained transport tests. The 64 fragmentation seeds printed; the deliberately ignored agent child remained exercised by its passing parent. Current committed source hashes match the remediation manifest.

The exact six-repository tuple matched at both review boundaries. Root status contained only the two explicitly excluded remediation prompts.

## 2. Invariant analysis

**The original cached-error state is removed.** `HUB` now stores only `Hub`, rather than `Result<Hub, ErrorKind>`. If supervisor creation fails, the function returns the error without populating the singleton. A later call therefore attempts construction again.

**Concurrent recovery preserves one supervisor.** A separate static initialization mutex serializes the empty-check, construction, and successful publication. Callers check `HUB` both before and after acquiring that mutex. Consequently, only the mutex owner can spawn while the singleton is absent; all later callers reuse the published hub. The corrected regression injects one initial supervisor failure, races eight retry callers, and observes exactly one successful supervisor construction.

**Failed initialization cannot consume helper capacity.** `Hub::global` completes before `COUNT.fetch_update`. The original failing supervisor attempt therefore creates no permit, helper, job cell, or retained entry. The subsequent 64-live-helper phase demonstrates that the full configured capacity remains available after initialization recovery and helper-spawn failures.

**Failure and abandonment recovery remain intact.** Once the hub exists, helper-spawn failure drops the newly acquired permit. Live abandoned helpers remain charged after their `Job` handles are dropped, cap exhaustion refuses new work, and successful late results are destroyed only after helper exit and join. After all 64 helpers finish, the regression observes all results disposed and admits a fresh job.

**Publication, join, cancellation, and transfer remain ordered.** The new deterministic wrapper holds a helper after result publication but before thread exit. Cancellation in that state leaves the result owned until join and destroys it exactly once. A second case waits for the completion wake proving join, then cancels before claim and again observes exactly-once destruction. The success case transfers the result to the caller; dropping the `Job` does not destroy the transferred owner.

**Changed client ownership strengthens the state grammar.** Removing `Agent::into_channel` prevents a used or poisoned protocol object from yielding a resettable channel. Shared test records show repeated enumeration, failed exchanges, unsupported methods, and partial operations cannot emit a second request; channel destruction occurs once with the agent.

**Scope and evidence remain honest.** A1 is still fixture-only. Native signing and session handoff belong to A2; endpoint refusal, physical pool retention, and backend activation belong to A3. Pending-connect execution on this Mac, native send-buffer exhaustion, platform primitives, and selected-source qualification remain explicitly deferred.

## 3. Risks and next action

The concrete A2 result owner must still prove bounded, non-panicking destruction. A3 must preserve the accepted distinction between cleanup failure and disposal acknowledgment. These are declared later gates and are not gaps in this corrected A1 object.

Accept A1 remediation 1 on the State axis and proceed only to the separately reviewed A2 signing checkpoint.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State P2-1 | Publish only a successfully constructed singleton supervisor; serialize initialization and retry after spawn failure without consuming capacity. | Archived red evidence reproduces all retries receiving the cached first error. Corrected code leaves `HUB` unset on failure; eight concurrent retries publish exactly one supervisor; the subsequent helper-spawn and 64-helper exhaustion/recovery phases demonstrate no lost capacity. | **Closed** |

## Changed-range analysis

The correction changes five files. The State-relevant production change replaces cached `Result` initialization with successful-value-only publication under a mutex and widens the private spawn seam to distinguish supervisor and helper creation. The surrounding job arbitration and socket code are unchanged.

The remaining changes remove owned-channel extraction, add deterministic protocol-state and publication/join/claim regressions, update the checkpoint, and refine only the requested test ceiling. These changes introduce no new state owner, public surface, capability, or architectural root cause.
