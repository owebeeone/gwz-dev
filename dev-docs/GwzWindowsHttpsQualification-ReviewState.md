# GwzWindowsHttpsQualification — State-AXIS REVIEW

**Review object:** Bounded Windows worker/provider qualification preparation, consisting of three changed SSPI test/documentation files, the root qualification checkpoint and the new private campaign. Full integrated Windows HTTPS qualification remains unclaimed.

**Baseline:**

| Repository | Reviewed HEAD | Comparison baseline |
|---|---|---|
| root | `c1db8d490bbce380c726ac4793493aec87053a00` | Qualification checkpoint at this revision |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` | `c88fa0e174b185a43e0d0d0c91660cb0957e380a` |
| gwz-core-evidence | `1930542b7264bcbc5d9b10c67887c0f350798cb1` | New named qualification campaign only |
| gwz-core | `28f564a674574eaefa43266d3137be9a4ddc38b8` | Unchanged |

Sources were inspected through Git diffs, numbered files and retained receipts. All four HEADs matched at start and end; tracked `git diff --stat HEAD` was empty in each repository.

**Date:** 2026-10-04

**Axis:** Native ownership, unwind, cancellation, originating-thread restoration and the scope of cleanup proof. Independent, adversarial, read-only. The other axis runs separately; nothing here relies on its report. Filed verbatim by the lane owner.

**Verdict: GO** — zero open findings for this bounded qualification object. This verdict does not qualify integrated Windows HTTPS, activation or release.

---

## 0. Evidence base

### Authority and scope

I read root `AGENTS_GWZ.md`, `EVIDENCE.md`, SSPI member instructions, architecture/testing documents, the qualification brief and [qualification checkpoint](/Volumes/projects/limbo/gwz-dev/dev-docs/GwzWindowsHttpsQualificationCheckpoint.md).

Controlling contract inspection included root SSPI design §§5–6, composition design §9, implementation acceptance and [NativeFixtures.md](/Volumes/projects/limbo/gwz-dev/gwz-sspi/docs/NativeFixtures.md). Process authority remains the review contract established by AgentProcessRules and GwzProcessOptimization.

No current peer report was read. Unrelated archives and drafts were not inspected.

### Sources inspected

- [completion.rs](/Volumes/projects/limbo/gwz-dev/gwz-sspi/tests/native/completion.rs): verifier ownership and release, lines 15–184; bounded exchange and independent completion checks, lines 186–257; live/retired origin handoff, lines 258–300; self-impersonation and restoration, lines 302–341; idle cancellation/deadline cleanup, lines 343–377.
- [native_worker.rs](/Volumes/projects/limbo/gwz-dev/gwz-sspi/tests/native_worker.rs): enclosing Windows module and fixture inclusion.
- Existing [windows_worker.rs](/Volumes/projects/limbo/gwz-dev/gwz-sspi/tests/native/windows_worker.rs): shared waiter, request, production executable selection, deadlines and existing fixtures.
- Production SSPI originating-thread checks in `supervisor/windows/identity.rs:109–171`; Conversation cancellation/Drop and Finish behavior; context snapshot/reaping and confirmed accounting; kernel Finish legality.
- Complete SSPI comparison diff: three public files, 401 additions. Production `src`, Cargo manifests/lock and protocol diff is empty.
- The new private campaign’s README, collectors, source updates, input hashes, manifest, final source readback and native receipts.

### Commands actually run

```text
CARGO_TARGET_DIR=/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/qualification-review-state cargo +1.95.0 check --manifest-path gwz-sspi/Cargo.toml --all-features --tests --target x86_64-pc-windows-msvc --locked --offline
```

Result: exit 0; Windows-target all-feature test compilation passed in 1.56 seconds.

```text
rustup run 1.95.0 rustfmt --check --edition 2024 gwz-sspi/tests/native/completion.rs
```

Result: exit 0.

Read-only hashing verified all **39 campaign manifest entries**. Of 133 provisioned inputs, only the subsequently updated completion fixture and public fixture documentation differ from the current tree. Provisioned production bytes match.

The final public completion fixture is byte-equal to archived `completion-final.rs`, with SHA-256:

```text
2fcc4b14e70b474cca6fca781fc24a87c3d8a4a75784c04c3e6f10d8115358bc
```

The retained remote readback records the same hash and byte equality. The intermediate settled-source receipt names test commit `e5f2843`; reviewed SSPI HEAD `582ec00` adds the documentation disposition.

### Recorded native execution

I inspected, but did not replay, these Windows receipts:

- Completion-v1: exit 101, one pass and two failed fixture assertions.
- Completion-v2: exit 0, all three completion/binding rows passed.
- Origin-v1: exit 0, one handoff test passed.
- Origin-v3: exit 0, both handoff/impersonation tests passed.
- Final-v3: exit 0, **eight tests passed**, zero failed or ignored, in 2.50 seconds; runner reported no timeout and its child reaped.
- Final inventory: exit 0, Windows build 26200, E: ReFS, zero processes whose executable paths fall beneath the owned runtime root.

Original failure outputs and deployed fixture versions are preserved. Their corrections concern assumed Negotiate round count and verifier error-token shape; production code remained unchanged.

Windows execution, ordinary suite and strict-Clippy claims remain owner-recorded evidence. My independently executed Windows-target check is source evidence, not native execution. No remote commands, credentials, writes or Git mutations were used.

## 2. Invariant analysis

**Verifier ownership:** The verifier owns its credential/context handles and fixed, initialized, aligned binding storage before native calls. Incoming and outgoing token buffers have initialized wiping owners before copies. Each synchronous call completes before those buffers can be released. The fixture checks returned output location and length before copying into SecretBytes.

Normal verifier teardown checks DeleteSecurityContext and FreeCredentialsHandle results. Assertion unwinding invokes best-effort cleanup through Drop, with wiping storage retained through disposal attempts. The accepted-context token is queried separately and its CloseHandle result is checked. No token contents or identity metadata are recorded. These paths do not claim native cleanup success when an assertion fails.

**Worker cleanup:** Exchange calls Finish before interpreting verifier acceptance, and separately requires empty Supervisor shutdown. Successful Finish uses the existing production completion proof, rather than merely receiving an acknowledgment or observing EOF. The cancellation row observes the receipt’s own record becoming Confirmed. Both idle-cancellation and expiry branches require an empty shutdown report and exactly one lifetime confirmation.

The expiry row retains the original two-second deadline through Start, the initial token and Finish. Its later shutdown deadline is a separate cleanup bound. This tests idle worker/IPC disposal after the first leg; it does not manufacture evidence of cancelling a blocked provider call.

**Origin lifetime:** The live handoff keeps the originating thread parked until captured Start and cleanup complete. The retired case joins that thread before Start and requires IdentityMismatch. Production checks use the retained original thread handle, inspect its liveness/impersonation and compare current primary metadata. The fixture therefore attacks executor recapture without claiming differing-account identity proof.

**Impersonation restoration:** A fresh originating thread captures first, then self-impersonates. Both a new capture and launch through the earlier capture must refuse. A thread-local RAII owner calls RevertToSelf; the normal path drops it explicitly and performs an actual subsequent capture before joining. Channel disconnection also causes that owner to unwind on the originating thread. No other thread or account is modified.

**Completion and binding truthfulness:** Positive exchanges require native worker Complete, authoritative NTLM selection, native verifier success and an accepted context token independently. The bounded round loop does not infer client Complete from verifier acceptance. The wrong-binding row requires the exact native bad-binding status after worker cleanup. Matching and mismatching synthetic bindings establish this local verifier behavior; they do not establish certificate provenance, HTTP EPA enforcement or token/SID equality.

**Adversity and containment:** Conversation/Start failure paths retain the existing Supervisor ownership semantics. Verifier calls are synchronous and can block; the public documentation states the need for an external campaign timeout. The retained runner distinguishes timeout from success and the collector labels unknown cleanup on collector timeout. Final executable-path inventory supports the stated owned-path observation, not universal process, LSASS or provider disposal.

**Boundary preservation:** All additions reside in the existing enclosing Windows test module. Tests remain opt-in/ignored by ordinary fast execution. Dependencies, public API, wire protocol, production ownership and activation guards are unchanged. The checkpoint accurately identifies the remaining Unix-only integration and portability prerequisite.

No durable product format, restart-adoption rule or new recovery state is introduced.

## 3. Risks and next action

Unwind cleanup is best effort, while normal successful fixture paths check native release results. Synchronous verifier calls still require campaign containment. Neither process termination nor zero owned-path inventory proves physical erasure or cancellation of external provider work.

These receipts qualify local worker/provider completion, synthetic binding rejection, originating-thread lifetime/impersonation refusal and idle cleanup on the recorded Windows host. TLS/EPA-required HTTP, differing identities, explicit-password completion, Kerberos, blocked-provider cancellation, Git and installed CLI/Python routes remain deferred.

The next action is to record this bounded preparation result alongside the other independent verdict, then develop and review the identified Windows qualification-entry/portability package before integrated HTTPS qualification. Normal Windows endpoint activation and full release remain NO-GO.
