# GWZ SSPI installed hosts4a — State-AXIS REVIEW

**Review object:** Installed hosts4a at the exact tuple below, controlled by `dev-docs/GwzSspiHostsCheckpoint.md` at root `8cb284391dc5864c9ba6f41324cdd3c5807cf765`. The checkpoint is DRAFT and not accepted.

**Baseline:**

| Repository | Reviewed HEAD |
|---|---|
| gwz-dev | `8cb284391dc5864c9ba6f41324cdd3c5807cf765` |
| gwz-sspi | `e31b17e95defd3468140e9b5f73fef8591766599` |
| gwz-cli | `061385fc009fd173488dd07e18d5b2d5882de05d` |
| gwz-py | `e9e228c0006d7f8b3d44c9657403dec5905c6e90` |
| gwz-core, unchanged reference | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

Sources were read through read-only Git inspection and clean member files. All five HEADs matched at both the start and end. SSPI, CLI and Python members were clean. Untracked root prompts/reports and unrelated root/core files were excluded.

**Date:** 2026-10-04

**Axis:** State — durable-state semantics, failure ordering, concurrent artifact production, installed selection and fail-closed recovery. Independent, adversarial, read-only. Nothing here relies on the parallel axis. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves **P2-1 and P2-2** as specified, with no additional material change.

---

## 0. Evidence base

The reviewed ranges were SSPI `84266f4..e31b17e`, CLI `f925e11..061385f`, Python `5aeff4b..e9e228c`, and root `3066630..8cb2843`. This is a new installed-host acceptance review, not a re-verdict on native acceptance.

Authority read and applied:

- Root/member instructions and the review process documents.
- Accepted `GwzSspiDesign.md`, particularly §§2 and 4–6.
- `GwzSspiPlan.md`, step 4.
- `GwzSspiNativeAcceptance.md`, as prior authority only.
- `GwzSspiHostsCheckpoint.md`, including its bounded implementation, file-ceiling disposition, verification claims and explicit remaining qualifications.

Principal implementation and test evidence:

| Area | Sources inspected |
|---|---|
| Artifact identity | `gwz-sspi/scripts/artifact_set.py:13–81`; `tests/schema/test_artifact_set.py:8–51` |
| Wheel mutation and RECORD | `artifact_set.py:86–111`; `test_artifact_set.py:52–70` |
| Shared installed descriptor | `gwz-sspi/src/packaging.rs:7–54`; `tests/packaging.rs` |
| CLI producer and early entry | `gwz-cli/scripts/build_sspi.py:9–47`; `build.rs`; `src/worker_host.rs:4–59`; early call in `src/lib.rs` |
| Python backend | `gwz-py/build_support/sspi_backend.py:15–130` |
| Python loaded-image selection | `gwz-py/native/src/worker_host.rs:7–96`; descriptor registration in `native/src/lib.rs` |
| Python fault/race fixtures | `gwz-py/src/tests/test_worker_packaging.py:18–133` |
| Actual build entry points | CLI release workflow and release reconciliation; Python publish workflow, candidate staging, package smoke and release reconciliation |
| Caller contracts | Member `HostPackaging.md` files; SSPI `WorkerEntry.md`; Python release instructions |
| Existing Hello boundary | `gwz-sspi/src/protocol/supervision.rs:114–145`, especially the build/schema comparison at lines 129–134 |

I inspected the complete changed host diff, including manifests, dependency/lock changes, workflows and public tests. No wire change, core implementation change or new supervisor owner appears in these ranges.

Additional read-only evidence:

- The owner-recorded `hosts/owner-tests.log` under `/Volumes/projects/limbo/evidence-build-cache/gwz-sspi/hosts`.
- External installed receipt and normal artifact paths, including ordinary and extracted-sdist wheel outputs.
- The installed Maturin Python backend’s wheel handoff implementation.
- Local Rust 1.96 Cargo documentation, `reference/config.html:693–717` and `reference/environment-variables.html:281`, establishing `CARGO_BUILD_RUSTFLAGS` as a conventional Cargo compiler input.

The checkpoint records broader owner/drafter gates. Those claims were assessed against source and available recorded evidence; they are not reviewer-executed tests. I performed no writes, builds, compiler probes, native execution, test execution or Git mutation. Current peer prompts and reports remained unread.

## 1. Findings

### [P2-1] Wheel postprocessing uses a shared, already-published filename

**Location:** `gwz-py/build_support/sspi_backend.py:61–88`, particularly the direct copy to `wheel_directory` at lines 67–68 and subsequent mutation at line 87; `gwz-sspi/scripts/artifact_set.py:89–111`, particularly the deterministic `.tmp` filename at line 105.

**Violated invariant:** A successful packaging operation must publish one complete extension/worker/receipt/RECORD set. Its private writes must not mutate a different operation’s published artifact. A failed replacement must not leave an incomplete wheel under the completed artifact’s ordinary filename.

The worker executable has build-owned output, but the wheel transaction does not. Maturin’s raw wheel is copied directly to the caller-visible final filename before provisioning. `bundle()` then reads that shared wheel and writes a temporary file whose name is derived solely from the final wheel name. There is no lock, collision refusal or build-specific staging boundary.

**Credible interleaving:** Run two backend builds A and B with the same package version, ABI and target, writing to the same permitted `--out` directory. Their filenames coincide even when their artifact fingerprints differ.

1. A copies its raw wheel and reads it into `bundle()`’s private `records`.
2. B copies its raw wheel over the same final filename and reads its own records.
3. A writes and closes the common `.tmp`, then pauses before `replace()`.
4. B opens that same `.tmp` in write mode, truncating it, and pauses while its ZIP is incomplete.
5. On POSIX, A replaces `.tmp` onto the final wheel and returns success.
6. B’s still-open file descriptor now refers to A’s published wheel. B can continue changing it after A’s success, or die leaving an incomplete ZIP there. B’s eventual `replace()` also lacks its original pathname.

The sequence follows directly from the shared path, ZIP write mode and rename ordering. It does not require a worker-output collision or native execution.

A simpler failure state exists without concurrency: a previously complete wheel is overwritten by the raw Maturin copy; provisioning then fails while writing the temporary ZIP. The final filename remains a valid-looking wheel containing the extension without the required worker and receipt. No rollback restores the prior complete artifact.

**Impact:** A successful build can expose a wheel that another operation subsequently corrupts. Failed replacement can destroy a previously complete wheel and leave an unprovisioned wheel at its normal output name. Later wheel selection, copying or installation can consume that artifact. These are concrete artifact correctness and recovery defects; runtime Hello refusal does not repair the wheel transaction.

The existing race fixture at `test_worker_packaging.py:79–103` cannot refute this sequence: it replaces `maturin_wheel()` with distinct thread-specific filenames and replaces `bundle()` with a worker-byte assertion. It establishes worker ownership only.

**Required correction:** Stage the raw wheel and all provisioning writes in a build-owned location. Use a unique temporary filename, finish and validate the complete wheel there, then publish it atomically. Define and enforce concurrent publication semantics for the same destination filename, through serialization or explicit collision refusal. Ensure a failed build preserves any prior complete destination and publishes no raw wheel as the final artifact. The shared extension-cache handoff must also remain protected through capture of the extension used by that build.

**Closure/regression test:** Exercise the real backend handoff and real `bundle()` using synthetic wheels, distinct fingerprints and controlled barriers. Cover coincident wheel filenames, interruption during ZIP writing, copy/provisioning failure, and a pre-existing complete destination. Successful outputs must always be complete coherent sets with valid RECORD hashes; failed replacement must preserve the previous complete wheel or leave no final artifact when none existed. Also exercise distinct frontend output directories with a shared extension cache, without replacing the handoff with distinct mocked filenames.

### [P2-2] The fingerprint ignores Cargo’s build-level rustflags environment input

**Location:** `gwz-sspi/scripts/artifact_set.py:73–80`.

**Violated invariant:** Conventional effective compiler options must distinguish artifact sets, or be refused before provisioning. Different effective compiler configurations must not receive the same trusted identifier merely because one supported configuration channel was omitted.

The producer records `CARGO_PROFILE_*`, `CARGO_TARGET_*`, selected compiler/native-tool variables, `CARGO_ENCODED_RUSTFLAGS` and `RUSTFLAGS`. It neither records nor refuses **`CARGO_BUILD_RUSTFLAGS`**.

Cargo’s local reference documentation identifies that variable as the environment form of `build.rustflags`. In the absence of higher-priority rustflags sources, Cargo passes it to the target compiler. This is a conventional Cargo build input, within the documented target/profile/rustflags coverage; it is not an arbitrary compiler plugin or external tool input.

**Credible reproduction:**

1. Hold package sources, locks, manifests, target, profile, features, compiler and configuration files constant.
2. Leave `RUSTFLAGS`, `CARGO_ENCODED_RUSTFLAGS` and overriding target rustflags unset.
3. Identify/build artifact set A with `CARGO_BUILD_RUSTFLAGS=-C target-cpu=generic`.
4. Identify/build artifact set B with `CARGO_BUILD_RUSTFLAGS=-C target-cpu=native`.

The producer’s environment projection excludes the changed key and its `rustflags` field remains empty. Consequently, the fingerprint inputs and identifier are identical, although Cargo compiles with different target CPU options. A similar collision can use a behavior-changing `--cfg` setting.

**Impact:** Host/worker replacement across these effective configurations cannot be distinguished by the compiled fingerprint. The existing Hello comparison accepts equal identifiers, so it cannot detect this configuration mismatch. For target CPU variants, the wrongly selected worker can also have incompatible instruction requirements. This does not claim a hash of every binary byte; it identifies a missing compiler-option channel inside the promised artifact-set inputs.

**Required correction:** Record this conventional rustflags channel, or reject it explicitly before identification and building. Audit the supported compiler-option channels as a coherent set so the declared coverage matches what Cargo actually consumes. Conservative hashing of all supported supplied rustflags channels is acceptable even when precedence means some are inactive.

**Closure/regression test:** With fixed synthetic Cargo resolution, compiler output and configuration, change only `CARGO_BUILD_RUSTFLAGS` and assert either distinct fingerprints or explicit refusal. Include unset/set cases and precedence combinations with `RUSTFLAGS`, encoded rustflags and target-specific rustflags. Retain the existing compiler-wrapper refusal tests.

## 2. Invariant analysis

**Compiled metadata remains the runtime authority.** The shared decoder requires exactly 64 hexadecimal characters. Missing metadata returns `WorkerUnavailable`; malformed metadata returns `WorkerMismatch`. Runtime variables and receipt contents are not consulted by installed descriptor construction. The Python fixture explicitly changes runtime fingerprint variables and the receipt without changing selection. That attack failed.

**Installed selection has no search fallback.** Python obtains its extension image from an extension-local function address and OS loader APIs. Mutable `module.__file__`, cwd and PATH do not supply its location. Relative loader paths refuse. The shared selector uses a fixed adjacent worker name and rejects missing, nonregular and final-component symlink workers. The missing-worker fixture supports the stated refusal. Selection does not authenticate executable contents; that remains the existing Hello boundary.

**Replacement and upgrade fail closed at the protocol boundary, subject to P2-2.** A retained loaded extension continues to supply its own compiled expected bytes. A newly installed worker with a distinct identifier is rejected by the existing Hello comparison before Begin. Unix canonicalization can make deleted or redirected image paths unavailable; it introduces no fallback. The reviewed source supports refusal, not a promise that a running old image survives arbitrary filesystem replacement without losing availability.

**CLI bootstrap precedes ordinary application effects.** `worker_host::early()` is called before ordinary parsing and build-info handling. Private malformed bootstrap and absent/malformed compiled metadata return a fixed failure exit, without falling through. The installed descriptor uses `current_exe()` and compiled metadata. No additional supervisor or process owner is introduced.

**Copied environments and worker ownership hold within their tested boundary.** The backend copies process environments. Its worker target directory is fresh and owned by one build. The failure and concurrency fixtures support these properties. Their scope ends before the unprotected wheel transaction identified in P2-1.

**Normal wheel RECORD construction is coherent.** On the uncontended success path, the producer inserts the worker and receipt adjacent to the extension, assigns executable worker permissions, and regenerates hashes and sizes. The standard RECORD contains the added files, providing ordinary installer removal ownership. Candidate extraction preserves the adjacent layout and worker mode. P2-1 concerns publication ordering and contention, rather than the normal RECORD calculation.

**Actual packaging entry points are connected.** Python wheel, candidate, package-smoke and publish paths invoke the new backend; source distributions carry that backend and discover the Cargo-resolved producer after extraction. CLI local provisioning and cargo-dist matrix production invoke the producer. Registry reconciliation requires the exact SSPI pin. Ordinary Cargo and editable builds remain explicitly unprovisioned. Publishing availability and the disclosed package prerequisites were not treated as findings.

**Scope and qualification remain bounded.** The changes add packaging and installed descriptor behavior without adding wire messages, core composition, runtime activation or another supervisor owner. Portable tests and Darwin artifacts do not qualify Windows installed Hello execution or native authentication. Existing native acceptance was not reattributed to these host changes.

## 3. Risks and next action

This review does not establish Windows installed execution, Linux installed-image qualification, HTTP/core composition, remote authentication or release availability. Those remain the explicit deferred outcomes. The fingerprint is an identifier for selected build inputs, not a signature or finished-binary hash.

The next action is one bounded remediation of **P2-1 and P2-2**, with deterministic publication/fault tests and conventional rustflags sensitivity or refusal tests, followed by settlement and re-review of the revised tuple. No native campaign or deferred HTTP implementation is needed to close these findings.
