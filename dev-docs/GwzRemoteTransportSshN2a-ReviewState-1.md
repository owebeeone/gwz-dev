# SSH N2a remediation 1 — State-axis review

**Review object:** N2a merged remediation 1 at `gwz-core` `dbc7616f7163de4d2bd78b839a50f7a2bc3c92a9`, compared with initial N2a commit `42f90094dae33eef1cd4828d2c97cadda2761f43`. No N2b/N3 attachment or production activation.  
**Baseline:** root `f92a34d9e097b2f947e8d8c544c3bf748fd2d068`; `gwz-core` `dbc7616f7163de4d2bd78b839a50f7a2bc3c92a9`; `gwz-core-evidence` `f90ce95e83fd17e87be18bf18937f5e470472fc4`; `gwz-transport` `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Immutable Git objects were used. Start and end tuple checks matched; only the two permitted generated remediation prompts were untracked.  
**Date:** 2026-09-21  
**Axis:** Focused State closure: cancellation checkpoints, failure disposition, ownership, and evidence causality. Independent, adversarial, read-only. The current other-axis report was not consulted.

**Verdict: GO** — the original production P3-1 is closed and no P0–P2 finding remains. One new evidence-only P3 finding remains.

---

## 0. Evidence base

I read the initial State report, merged `GwzRemoteTransportSshN2a-RemPlan-1.md`, the complete `42f90094..dbc7616f` core delta, corrected N2a checkpoint, and remediation evidence at `gwz-core-evidence` `f90ce95e`.

Focused inspection covered:

- `ssh_key_container.rs:26-63,227-289`
- `ssh_key_auth.rs:53-84`
- `key_container.rs:147-192`
- `selected_key.rs:349-672`
- setup failure mapping at `ssh_setup.rs:106-132,293-310`
- evidence README, raw focused/full logs, and `sources.json`.

All seven evidence hashes match the corrected committed blobs. The archived full suite records 110 passing executions and one intentional child-only ignore. I reran the permitted locked/offline Rust 1.95 pair: all six container and thirteen selected-key tests passed.

## 1. Prior-finding closure

| Prior finding | Status | Evidence |
| --- | --- | --- |
| State P3-1 — maximum-size armor lines could run between Control checks | **Closed in production code** | `checked_scan` checks Control before every 128-byte chunk. NUL detection, leading whitespace, newline discovery, and trailing text now use it; base64 already used the same cadence. A near-cap single-line body can no longer cross a chunk boundary without a check. |

## 2. Findings

### [P3-2] The deterministic scanner regression stops through its predicate, not cancellation

**Location:** `tests/transport_ssh/tests/key_container.rs:164-192`, particularly lines 172-181; remediation evidence README’s claim that this test proves cancellation terminates the scanner.

At predicate call 128, the test signals the parent, waits for cancellation, and then returns `true`. That return causes `checked_scan` to return `Some(127)` immediately. The separate `control.check()` at line 181 reports cancellation afterward.

Consequently, the observed count of 128 would remain true even if the production scanner’s check before the second chunk were removed: the predicate itself prevents entry into that chunk. The production implementation is correct by source inspection, but this regression does not causally protect the corrected boundary and the archived evidence overstates what it proves.

After the barrier, return `false` and require `scan_for_test` itself to return `ConnectionAborted` before invoking the predicate for byte 129. Retain the exact `observed == 128` assertion. The corrected test must fail if the between-chunk `control.check()` is removed.

## 3. Invariant analysis

The changed scanner preserves fixed scratch and performs no input-sized allocation. Every formerly unchecked classifier scan now has a 128-byte Control cadence. Cancellation/deadline arbitration still precedes interning, and the unchanged snapshot reservation remains charged until joined disposal.

The authentication mapping now retries only EAGAIN, reserves `PermissionDenied` for explicit `LIBSSH2_ERROR_AUTHENTICATION_FAILED`, and maps ambiguous terminal native errors to sanitized `Other`. The real disconnected-session test passes through `SetupConnector` and produces `ErrorCode::Io`, makes one setup attempt, leaves the candidate unproven, and releases its reservation. No alternate identity, address, agent, file, or Git path is attempted.

The replacement encrypted-container evidence uses structurally valid encrypted PKCS#8, traditional PEM, and OpenSSH containers with modified extreme work factors. Each refuses during admission with zero native dispatch and zero retained quota; the same dispatch wrapper reaches native authentication for the unencrypted control. This preserves the fail-closed KDF boundary.

No changed range weakens exact-byte interning, monotonic tokens, connection-before-pin disposal, or live-handoff promotion.

## 4. Risks and next action

N2b lifecycle integration, N3 attachment, platform/source qualification, and activation remain explicitly deferred.

Correct P3-2’s predicate and refresh the focused evidence before treating the scanner regression as a causal guard. The production N2a State boundary itself is fit to proceed.
