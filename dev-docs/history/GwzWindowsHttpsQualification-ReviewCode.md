# GwzWindowsHttpsQualification — Code-AXIS REVIEW

**Review object:** Bounded Windows worker/provider qualification fixtures and supporting campaign evidence. This is separate from accepted HTTPS composition and does not qualify integrated Windows HTTPS, activation or release.

**Baseline:**

| Repository | Reviewed HEAD |
|---|---|
| root | `c1db8d490bbce380c726ac4793493aec87053a00` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-core-evidence | `1930542b7264bcbc5d9b10c67887c0f350798cb1` |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` |

The SSPI review range is `c88fa0e174b185a43e0d0d0c91660cb0957e380a..582ec001bd2972076ea65a7db87d81d988c6f2e7`. It adds 401 lines across three public test/documentation files. Production sources, dependencies, public API and wire format are unchanged.

All four HEADs matched at the start and end. Final tracked-diff checks returned no changes. Sources and evidence were inspected using read-only commands; native execution was not replayed.

**Date:** 2026-10-04

**Axis:** Code — architecture, interfaces, call graphs, compatibility, native verifier safety and fidelity of implementation claims. Independent, adversarial, read-only. Another axis runs separately; nothing here relies on its report. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1 or P2 finding. One P3 finding invalidates the separate final process-inventory receipt. The recorded native fixture passes remain supported by their assertions and source provenance. Full integrated Windows HTTPS qualification and release remain NO-GO.

---

## 0. Evidence base

Authority inspected:

- Root `AGENTS_GWZ.md`, `EVIDENCE.md` and SSPI member instructions.
- `AgentProcessRules.md` and its controlling process amendment.
- `GwzWindowsHttpsQualificationCheckpoint.md`.
- Relevant identity, native ownership, disposal and qualification clauses of `GwzSspiDesign.md`.
- Composition design §9 and implementation acceptance.
- SSPI Architecture, Testing and NativeFixtures documentation.

Principal sources inspected:

| Area | Sources |
|---|---|
| New verifier and exchanges | `tests/native/completion.rs:1–377` |
| Existing shared fixture helpers | `tests/native/windows_worker.rs:1–125` |
| Windows enclosing boundary | `tests/native_worker.rs:1–8` |
| Existing admission and cleanup API | `src/supervisor/api.rs:69–155`, `180–205` |
| Existing worker native ownership | `src/worker/windows/conversation.rs`, relevant credential, initialization and disposal paths |
| Campaign invocation and provenance | New campaign’s `collect.py`, `remote.py`, baseline/input hashes, source updates/readback, manifest and recorded receipts |
| Portability prerequisite | Existing core/CLI/Python Unix guards, helper Unix imports and endpoint-environment non-Unix compile error |

Only the new private campaign was inspected:

`gwz-core-evidence/campaigns/https-integration/runs/2026-10-04-windows-sspi-qualification`

Hash verification established:

- All **39** manifest entries match their recorded SHA-256 values.
- The archived final fixture equals the reviewed public fixture byte-for-byte.
- Its SHA-256 is `2fcc4b14e70b474cca6fca781fc24a87c3d8a4a75784c04c3e6f10d8115358bc`, matching final source readback.
- All **87** recorded production `src/` input hashes match the reviewed SSPI production tree.
- The later SSPI commit following the recorded test-source settlement changes only NativeFixtures documentation.

The recorded final-v3 invocation returned exit 0, with **8 passed**, no failures or ignored tests, in 2.50 seconds. It reports no campaign timeout and a reaped command child. Six tests are the additions under review; two are existing production-worker fixtures.

I inspected the retained completion-v1 failures and the v1→v2 source difference. The correction replaces the fixed two-round assumption with a bounded exchange loop and permits verifier error output. It preserves positive verifier success, separate worker Complete, authoritative NTLM selection and the exact wrong-binding rejection requirement.

Executed independently:

```sh
CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/qualification-review-code \
cargo +1.95.0 check --manifest-path gwz-sspi/Cargo.toml \
--all-features --tests --target x86_64-pc-windows-msvc --locked --offline
```

Result: exit 0; Windows branches checked successfully.

```sh
rustup run 1.95.0 rustfmt --check --edition 2024 \
gwz-sspi/tests/native/completion.rs
```

Result: exit 0.

These are source checks, not independent Windows execution. I did not run remote commands, replay the campaign, modify files or inspect the current peer report.

## 1. Findings

### [P3-1] Final inventory compares Windows executable paths against an unnormalized slash prefix

**Location:** Private campaign [collect.py:46](/Volumes/projects/limbo/gwz-dev/gwz-core-evidence/campaigns/https-integration/runs/2026-10-04-windows-sspi-qualification/collect.py:46), using the `WINDOWS` value at line 16; root checkpoint’s final owned-path process-inventory claim.

**Violated invariant:** A claimed zero-process inventory must recognize processes executing beneath the owned Windows fixture directory.

**Counterexample:** The inventory sets:

```text
$root = 'E:/gwz-tests/https-sspi-qual-20261004-a'
```

It then applies a case-insensitive string `StartsWith` comparison to `Win32_Process.ExecutablePath`, without normalizing separators. A live owned worker with this native executable path fails the predicate:

```text
E:\gwz-tests\https-sspi-qual-20261004-a\target\debug\gwz-sspi-worker.exe
```

Case-insensitive comparison does not equate `/` with `\`. The retained native command output itself uses backslash paths beneath this directory. The inventory can therefore report `ownedRemaining: 0` and exit successfully while such an owned process remains alive.

**Impact:** The recorded inventory result does not independently establish zero remaining owned-path processes. This is a bounded evidence defect: the production Finish, cleanup-status and shutdown assertions in the native tests remain separate proof obligations and are not bypassed by this collector.

**Required correction:** Normalize the root and candidate executable paths using Windows path semantics before comparison. Include a directory boundary so a similarly named sibling root is not counted. Correct the checkpoint’s inventory attribution until a refreshed receipt exists.

**Closure test:** While a known fixture-owned process remains alive beneath the runtime root, execute the actual inventory and require a nonzero count and refusal of the zero-process gate. Cover native backslash paths, accepted slash spelling and a similarly prefixed sibling directory. After disposing the held process and observing its exit, rerun the inventory and retain the final zero receipt.

## 2. Invariant analysis

**Verifier buffers and handles have explicit owners.** The binding allocation is initialized, aligned and fixed: 88 bytes accommodate the 32-byte header and 53-byte application data. The descriptor exposes the intended 85-byte binding. Input tokens are copied into initialized zeroizing storage; output storage is initialized and bounded to 65,536 bytes.

The verifier does not request provider-allocated output or missing-binding acceptance. After the synchronous call, it checks the output pointer and length before copying into SecretBytes. Input and output storage are explicitly wiped and remain zeroizing owners on assertion unwind.

Credential and context handles begin invalid. Live returned handles are retained before subsequent assertions. Normal completion checks DeleteSecurityContext and FreeCredentialsHandle statuses; Drop provides cleanup attempts on fixture failure. QuerySecurityContextToken is followed immediately by checked handle closure without reading or logging identity metadata. I found no concrete dangling-buffer, unchecked output-length or double-disposal defect in these paths.

**Positive completion requires independent results from both sides.** The exchange is bounded to eight rounds. Positive rows require native verifier status zero, worker TokenStatus::Complete and authoritative NTLM selection. A continuing exchange must have a verifier token; empty client output requires an already completed verifier. The test cannot qualify a positive exchange merely from worker token availability.

The Negotiate result specifically selects NTLM. It does not establish Kerberos. The wrong-binding row requires `SEC_E_BAD_BINDINGS`, rather than accepting an arbitrary native failure.

**Binding evidence remains synthetic and local.** Worker and verifier construct matching synthetic bindings independently. The negative verifier changes an application-data byte. This exercises native match/mismatch behavior; it supplies no certificate provenance, verified TLS connection or EPA-required HTTP-server evidence.

**Token availability is not identity equality.** The accepted verifier context yields an OS token whose handle is closed. The fixture does not inspect or compare its account, SID, authentication LUID or session. The checkpoint and campaign explicitly disclose that limit. I found no claim that this token proves the authenticated peer equals a separately expected identity.

**Origin handoff checks the retained original thread.** The live-origin fixture keeps its originating thread alive while a different thread starts with the captured capability. The retired-origin case joins that thread before attempting Start and requires IdentityMismatch.

The impersonation fixture captures before ImpersonateSelf, requires new capture to refuse while impersonating, and attempts launch with the earlier capability while the origin remains impersonating. Its restoration owner calls RevertToSelf on that same fresh thread; a subsequent actual capture checks restoration before thread exit. No differing-account or primary-token-change result is claimed.

**Idle cleanup uses the original deadline.** The expiry case passes one fixed two-second deadline to Start, obtains the initial token, waits beyond that deadline and requires Timeout from Finish. The cancellation case observes the receipt’s record until Confirmed within its separate observation bound. Both cases require empty shutdown outstanding records and one lifetime confirmation.

These tests exercise idle worker IPC and actual Supervisor cleanup. They do not establish cancellation of a blocked synchronous provider call, physical erasure after forced termination or external LSASS cancellation. Those limits are explicit.

**Recorded fixture corrections preserve the qualification bar.** The initial failed assumptions concerned native round count and error output. The corrected loop accommodates those observations while retaining both completion requirements and exact negative status. The original failures, deployed source versions and final source readback remain preserved.

**No production or activation expansion occurred.** The new fixtures sit inside the existing enclosing Windows module and remain ignored by default. They use the existing worker, Supervisor and dependency set. The production source hashes match the campaign inputs.

The recorded portability prerequisite is supported by actual source: core transport and installed caller selection retain Unix candidate guards; endpoint environment has an explicit non-Unix compile error; helper code retains Unix APIs. These local provider fixtures do not close that prerequisite or make Windows endpoint selection available.

## 3. Risks and next action

The bounded local provider, origin-handoff and idle-cleanup results are supported. The separate zero-process inventory needs correction under P3-1 before it is used as evidence.

Full TLS/EPA-required HTTPS, integrated core/Git/CLI/Python execution, explicit-password authentication, Kerberos, Digest, differing-account identity and blocked-provider cancellation remain open and explicitly unclaimed. Activation and release remain separate gates.

The next action is to correct and characterize the inventory predicate, then retain a refreshed native inventory receipt. This review requires no production modification and does not authorize activation or release.
