# SSH N2a evidence-only remediation 2 — CODE-AXIS REVIEW

**Review object:** N2a evidence-only remediation 2 at `gwz-core` `3fa6a23a2a05732ed9368e77048ba0caaf7e508d`; controlling draft `dev-docs/GwzRemoteTransportSshN2a.md`; 2026-09-21.  
**Baseline:** root `06c74a31fc61f5a70bb02b01786f099c6ba198f9`; core `3fa6a23a2a05732ed9368e77048ba0caaf7e508d`; evidence `8af0f7ce002148ed31d16c52e830308faaf9e5de`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from committed objects with `git show` and immutable diffs.  
**Date:** 2026-09-21  
**Axis:** Focused Code endorsement of the corrected scanner test and evidence. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on its current-round output. Filed verbatim by the lane owner.

**Verdict: GO** — the evidence defect is closed, production remains identical to the accepted remediation 1 object, and no new finding was identified.

---

## 0. Evidence base

I read the canonical prompt, RemPlan-2, legitimate remediation 1 reports, the complete core diff `dbc7616f..3fa6a23a`, and the new evidence archive. The core delta contains only:

- `tests/transport_ssh/tests/key_container.rs`, a 16-line net test correction;
- `dev-docs/GwzRemoteTransportSshN2a.md`, recording that correction.

No production source changed. The committed test blob hashes to `6f5cd634db4cfa60b79769dec0d1c2aff663b05cf8bfccaca9b8928ec099b8de`, exactly matching remediation 2 `sources.json`.

The archived causal red shows the old `true` predicate returning `Ok(())` where `ConnectionAborted` is required. The corrected archive shows all six container tests passing. I independently ran the permitted focused Rust 1.95 locked/offline test; `checked_scan_observes_cancellation_after_first_chunk` passed.

The six-repository tuple and tree objects were identical at review start and end. Member repositories were clean. Root contained only the two permitted untracked remediation 2 prompts.

## 1. Findings

No findings.

### Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| Code P2-1 — native terminal errors misclassified as authentication failure | **Remains closed** | Accepted remediation 1 production and its real disconnect regression are byte-identical in this object. |
| Code P2-2 — malformed surrogates for hostile encrypted containers | **Remains closed** | Accepted remediation 1 classifier, guarded native-dispatch test, and valid encrypted fixtures are unchanged. |
| State P3-2 — scanner test terminated through its predicate | **Closed on Code inspection** | `key_container.rs:164-199` now returns `false` after the cancellation barrier, sends `scan_for_test`’s own result to the parent, asserts that result is `ConnectionAborted`, and retains the exact count of 128 predicate calls. The archived old-predicate red demonstrates the original counterexample. |

## 2. Invariant analysis

The previous test could stop at byte 128 by returning `true`, so a later explicit control check manufactured the observed cancellation. That counterexample no longer applies.

After byte 128, the corrected predicate releases the barrier and returns `false`. The scanner must therefore continue. Its production loop checks `Control` before entering the next 128-byte chunk, returns `ConnectionAborted` before invoking the predicate for byte 129, and sends that direct result through the separate `scanned` channel. The parent independently verifies both the scanner result and the job result, then verifies exactly 128 predicate invocations.

Removing or bypassing the between-chunk production check would now cause the scanner to continue beyond byte 128 or return `Ok`, failing the direct result or count assertion. The test therefore protects the claimed boundary causally.

Production remains at 598 lines. The two tests total 876 lines, within the accepted 900-line bound. The prior full-suite result remains valid as the unchanged production baseline; the focused correction did not require a redundant full rerun.

## 3. Risks and next action

This endorsement adds no N2b worker integration, N3 attachment, platform or selected-source qualification, or production activation claim.

The next action is to accept the final N2a checkpoint and proceed to the separately reviewed N2b integration gate.
