# SSH N2a evidence-only remediation 2 — State-axis review

**Review object:** Evidence-only N2a correction at `gwz-core` `3fa6a23a2a05732ed9368e77048ba0caaf7e508d`, compared with remediation-1 commit `dbc7616f7163de4d2bd78b839a50f7a2bc3c92a9`. The delta is confined to the scanner regression and checkpoint documentation; production blobs are unchanged.  
**Baseline:** root `06c74a31fc61f5a70bb02b01786f099c6ba198f9`; `gwz-core` `3fa6a23a2a05732ed9368e77048ba0caaf7e508d`; `gwz-core-evidence` `8af0f7ce002148ed31d16c52e830308faaf9e5de`; `gwz-transport` `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Start and end tuple checks matched; only the two permitted generated remediation-2 prompts were untracked.  
**Date:** 2026-09-21  
**Axis:** Focused State closure of scanner cancellation evidence. Independent, adversarial, read-only. The current other-axis report was not consulted.

**Verdict: GO** — P3-2 is closed; no open findings.

---

## 0. Evidence base

I inspected:

- `GwzRemoteTransportSshN2a-RemPlan-2.md`.
- The complete `dbc7616f..3fa6a23a` core delta: 12 additions and four removals in `tests/transport_ssh/tests/key_container.rs`, plus documentation.
- The corrected test at `key_container.rs:164-200`.
- Remediation-2 evidence README, source fingerprint, causal-red log, and six-test green log at evidence commit `8af0f7ce`.
- Production blob identities for all three `ssh_key_*.rs` files; each is byte-identical to the accepted remediation-1 revision.
- The permitted focused Rust 1.95 locked/offline test, which passed.

The committed test SHA-256 is `6f5cd634db4cfa60b79769dec0d1c2aff663b05cf8bfccaca9b8928ec099b8de`, matching `sources.json`.

## 1. Prior-finding closure

| Prior finding | Status | Closure evidence |
| --- | --- | --- |
| State P3-1 — unchecked maximum-line scans | **Closed, unchanged** | The accepted production scanner still checks Control before every 128-byte chunk. Its blob is unchanged from remediation 1. |
| State P3-2 — regression stopped through its predicate rather than cancellation | **Closed** | After the byte-128 barrier, the predicate now returns `false`. The scanner must attempt the next chunk, where its own Control check returns `ConnectionAborted`. The scanner result is sent separately to the parent and asserted independently of Job arbitration; the predicate count remains exactly 128. |

## 2. Invariant analysis

The revised sequence is causal:

1. The scanner processes exactly the first 128 bytes.
2. The predicate blocks at the barrier.
3. The parent cancels the Job and releases the barrier.
4. The predicate returns `false`, so it cannot terminate scanning.
5. Before processing byte 129, `checked_scan` performs its next Control check and returns `ConnectionAborted`.
6. The worker sends that direct scanner result to the parent before returning it to Job arbitration.

The archived causal red uses the old `true` predicate with the new independent assertion. It fails because the scanner reports `Ok(())` rather than `Err(ConnectionAborted)`. The corrected version passes all six container tests, and my focused rerun also passed. Removing the between-chunk Control check would now make the predicate count exceed 128 and the direct result cease to be `ConnectionAborted`, so the regression protects the intended boundary.

No production source, authentication mapping, reservation accounting, ownership order, or routing state changed. The remediation-1 full-suite result remains the applicable production baseline.

## 3. Risks and next action

N2b lifecycle integration, N3 attachment, platform/source qualification, and activation remain deferred exactly as recorded.

Accept N2a State closure and proceed to the separately gated N2b work.
