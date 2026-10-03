# GWZ SSPI native worker remediation 1 — Code-AXIS REVIEW

**Review object:** Remediation 1, controlled by `dev-docs/GwzSspiNativeCheckpoint.md` and `GwzSspiNative-RemPlan.md` at root `bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e`, dated 2026-10-03, pending reviewer closure. Member range `610964282663c3b7844c9620d063e40d9ee76258..425e13dc011c42e94fdea31779a8e5967aedc82b`; root range `40fd121fbc2d9e6e727ed1a1b2501275200a1797..bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e`.

**Baseline:**

| Repository | Corrected HEAD |
|---|---|
| gwz-dev | `bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e` |
| gwz-sspi | `425e13dc011c42e94fdea31779a8e5967aedc82b` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-core-evidence | `9adf06beab7a15d1e5c22ec1cecf8e35966e1d7c` |

All four HEADs and statuses were verified at start and end and remained unchanged. Member status was clean. The identified unrelated untracked root, reference-core and evidence material remained outside scope. Controlling root documents were read with `git show HEAD:`; tracked member sources were read directly and compared with the original reviewed commit using `git diff`.

**Date:** 2026-10-03

**Axis:** Architecture, interface contracts, call graphs, ownership, compatibility and error paths. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — Both original Code findings are closed. No new P0, P1, P2 or P3 finding was established. No new architectural root cause was found.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Code P3-1 | Correct stale unconditional worker-refusal statements in Testing. | The former `docs/Testing.md:93–96` statements are replaced by implemented local Negotiate/NTLM behavior, native-fixture links and conditional refusal. The complete guide preserves the separate qualification limits. | **Closed** |
| Code P3-2 | Exercise real initial Negotiate observation and checked query-allocation release in an opt-in native fixture. | The new native owner fixture calls `Conversation::observation`, which reaches production `QueryContextAttributesW`. Its receipt records `query_status=0`, `allocation_returned=true`, `release_success=true`, selected NTLM. A separate production Supervisor fixture passes initial Negotiate and normal Finish. Both native commands exited 0 against the corrected member SHA. | **Closed** |

## Changed-range analysis

The entire 13-file member range was inspected.

- **Documentation:** CallerValues, Testing and NativeFixtures correct current status, describe the strengthened containment evidence and add environment restoration and guarded teardown to the fixture recipe.
- **Test ownership:** A test-only cleanup guard owns helper and scratch cleanup across failures. Windows fixtures add suspended-child Job closure, a no-kill negative control and helper-failure cases. These modules remain enclosed by `#[cfg(test)]`; they introduce no production lifecycle owner.
- **Private production call graph:** `create_owned` still creates and configures the production kill-on-close Job, then transfers it into extracted `create_in_job`. The extraction retains the existing creation body, handle ownership, creation-time Job attachment and membership check. Searches found only the configured production caller and the test-only negative-control caller. No public Job-policy choice was added.
- **Private observation audit:** Conversation gains a private audit field and passes it to the existing negotiation query. Production Audit is an empty struct with a no-op method. The Arc/Mutex receipt exists only in the enclosing test module. Native query, interpretation and checked release retain their prior ordering and error behavior.

These changes fall within the merged dispositions. They do not materially change authority, authentication policy, public API, wire protocol, dependencies or the original production ownership proofs. A new numbered architectural review is not required for this bounded patch.

## 0. Evidence base

This focused re-verdict continues the original Code review and its accepted-design analysis. It independently retraced the two original counterexamples.

Read:

- Generated `GwzSspiNative-PromptCode-1.md`, committed remediation plan and corrected checkpoint.
- Full member diff from the original reviewed commit to corrected HEAD.
- Complete `docs/Testing.md`, changed CallerValues and NativeFixtures text, and `docs/WorkerEntry.md`.
- `src/supervisor/fixture_cleanup.rs:1–131`, its enclosing test-module declaration, `windows/fixture_support.rs:1–181`, `windows/containment_tests.rs:1–140`, and changed native-test includes.
- The complete launch extraction and its call sites.
- Conversation audit-field changes; `windows/observation.rs:1–101`; `native_owner_tests.rs:55–111`; and the added production Negotiate fixture in `tests/native/windows_worker.rs`.
- Corrected private campaign README, source receipt, native-owner and native-Supervisor receipts and outputs, final inventory output, recipe-adaptation description and successful recipe-restoration output.

The native receipts identify member `425e13dc011c42e94fdea31779a8e5967aedc82b`, pinned Rust 1.95.0, synthetic build metadata and external runtime/build paths. The recorded native-owner and Supervisor commands exited 0. The query receipt demonstrates an actual returned allocation and successful checked release, rather than merely allowing a no-allocation outcome. Final inventory reports zero owned-path processes, Windows build 26200 and ReFS.

Executed only the permitted prebuilt pure test command:

```text
/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/native/debug/deps/gwz_sspi-e95571679a8a0127 supervisor::fixture_cleanup
```

It exited 0; both selected cleanup tests passed and no ignored tests ran. This verifies the prebuilt portable guard tests, not an independent rebuild or Windows execution.

No writes, Git mutations, builds, compiler probes, native campaigns or process fixtures were executed by this reviewer. No current peer report or prompt was read.

## 2. Invariant analysis

**Documentation agreement now holds.** Testing no longer claims that the production worker always refuses. Its updated text agrees with the shared-entry contract and keeps missing/malformed metadata, malformed bootstrap and unavailable Digest explicit. It distinguishes local native work from installed composition and complete qualification.

**The original missing native query is now exercised.** The native fixture uses Package::Negotiate and calls the same Conversation observation method as production. The receipt is recorded after `Allocation::release`, on both query-success and query-error paths. Recording does not dereference freed allocation memory. The executed result reports a successful query, a returned allocation and successful release. The separate Supervisor fixture verifies this path through production worker IPC.

**Observation policy is preserved.** The patch adds evidence collection without changing mechanism-name matching, authoritative-state interpretation or release-error propagation. Complete publication still requires authoritative selection in the unchanged serial bridge. An authoritative mechanism observation accompanying Continue is not presented as completed authentication.

**Production containment policy is preserved.** The extracted creation body receives an owned Job. Every production call still obtains that Job from the unchanged kill-on-close setup. The no-kill Job is constructed only by a test fixture. The revised containment tests leave children suspended, preventing bootstrap or clean EOF from explaining their exit.

**Fixture ownership is bounded and separate.** Helper ownership transfers into its guard immediately after spawn. Cleanup observes the held process before consuming Child::wait, attempts scratch disposal even when helper cleanup fails, and records failed confirmation explicitly. Scratch creation claims only a new file. Test-only ownership and native waits do not enter parent production control paths.

**Earlier production proofs remain applicable.** The patch leaves protocol projections, serial phase handling, cap admission, identity/CBT storage, provider-output ownership, credential/context disposal, bootstrap metadata policy and parent terminal arbitration unchanged.

## 3. Risks and next action

The new Negotiate evidence covers local initial selection and query-allocation disposal. It does not qualify completed remote Kerberos/NTLM, final authentication, TLS/EPA, blocked providers, descendants, Digest or installed provenance.

The corrected containment evidence is stronger than the original resumed-child receipts because the child cannot execute EOF handling while suspended. It still supplies no worst-case OS cancellation or physical-erasure guarantee.

**Next action:** Record closure of Code P3-1 and P3-2 and this Code-axis GO at the corrected tuple. The lane owner should complete the remaining recorded reviewer gates before accepting the bounded checkpoint; activation, publication and deferred qualification remain outside this verdict.
