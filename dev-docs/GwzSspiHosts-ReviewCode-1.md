# GWZ SSPI installed hosts4a — Code-AXIS REVIEW

**Review object:** Installed hosts4a remediation 1, controlled by `dev-docs/GwzSspiHostsCheckpoint.md` at root `6b670283179f45f0dbbe38b1f6c300b326e50100`. The checkpoint remains DRAFT pending independent closure.

**Baseline and reviewed tuple:**

| Repository | Initial reviewed HEAD | Remediation HEAD |
|---|---|---|
| gwz-dev | `8cb284391dc5864c9ba6f41324cdd3c5807cf765` | `6b670283179f45f0dbbe38b1f6c300b326e50100` |
| gwz-sspi | `e31b17e95defd3468140e9b5f73fef8591766599` | `14d834b311200b0984e7041e4a34419c59501119` |
| gwz-cli | `061385fc009fd173488dd07e18d5b2d5882de05d` | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `e9e228c0006d7f8b3d44c9657403dec5905c6e90` | `c3f5f7d6b614155e413db0036242849010ef1149` |
| gwz-core, unchanged reference | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

**Date:** 2026-10-04

**Axis:** Architecture, interfaces, call graphs, compatibility and error paths. Independent, adversarial, read-only. Nothing here relies on current parallel reports. Filed verbatim by the lane owner.

**Verdict: GO** — original Code P2-1, P2-2 and P3-1 are closed. No blocking finding remains. One new bounded P3 interface defect remains open.

---

## 0. Evidence base

All five HEADs matched the canonical remediation prompt at the start and end. SSPI, CLI and Python working trees were clean. Root and reference core contained the disclosed untracked documentation. No reviewed HEAD moved. Current peer prompts and reports were not opened.

The review read the complete canonical Code remediation prompt, my original report, the merged remediation plan, the controlling checkpoint and the complete remediation diffs in all three changed members. Root inspection covered the checkpoint/program updates, managed member pins and changed-file inventory. The original Code report’s authority and initial-range analysis remain the baseline; this review independently retraced its counterexamples through the corrected call sites.

Principal source and regression evidence:

- Python `build_support/sspi_backend.py:28–170`: normalization, interpreter probing, metadata/editable hooks, private wheel/source staging and publication.
- Python `native/src/lib.rs:16–28` and `native/src/worker_host.rs:98–138`: the exported OS-string conversion and Unix/Windows conversion fixtures.
- Python `src/tests/test_worker_packaging.py:48–83,144–329`: retained installed-image fixture, frontend defaults, complete handoff/bundler concurrency, failure preservation, normalized command/input agreement and flattened matrix aggregation.
- Retained candidate `scripts/build_candidate_extension.py:6–18,119–150,190–208` and its command fixture at `src/tests/test_build_candidate_extension.py:101–112`.
- SSPI `scripts/artifact_set.py:56–81,86–134`, its complete changed producer tests and HostPackaging additions.
- CLI `scripts/build_sspi.py:20–50`, the changed workflow upload glob and retained flattened-download paths.
- Complete member HostPackaging documentation changes and retained Maturin interpreter/default implementation.

Read-only artifact inspection covered the corrected installed and extracted receipts, extracted backend/configuration, metadata WHEEL file and installed layout under `evidence-build-cache/gwz-sspi/hosts-remediation-1`. Both inspected receipts record the normalized interpreter, target and profile; the extracted source carries the corrected staging backend.

The owner-recorded portable gates in `GwzSspiHosts-RemPlan.md` report 50 passing installed-wheel focused tests with one filesystem skip, passing producer/release tests and conditional-boundary checks. These are recorded owner results, not reviewer executions. The Darwin non-UTF-8 directory fixture was skipped with EILSEQ; Windows surrogate/runtime publication remains unexecuted.

Inspection used Git HEAD/status/diff/show checks and text/file reads. Member remediation diff checks passed. Root diff checking reported the intentional Markdown hard-break whitespace in the filed original Code report. No writes, Git mutations, builds, tests, compiler probes, helpers or native execution were performed.

### Prior-finding closure

| Original finding | Status | Independent closure basis |
|---|---|---|
| Code P2-1: interpreter-dependent target defaults | **Closed** | `settings()` resolves the frontend or explicit interpreter, derives the i686 default for 32-bit Python on AMD64 Windows, and makes target/interpreter/profile explicit. Metadata, editable and wheel hooks use that normalization. Tests at lines 144–155 and 251–298 check defaults, overrides, producer inputs and actual subprocess argv. The candidate launches the backend under its selected `--python`, which becomes the default explicit interpreter. |
| Code P2-2: lossy filesystem path | **Closed** | The exported function returns `OsString`; the shared conversion copies the checked path without `to_string_lossy()`. Tests exercise that same conversion through PyO3, checking Unix `fsencode` byte identity and Windows surrogate round-trip. The loader selection remains unchanged. Closure is source/conversion closure, without claiming an executed Windows loader proof. |
| Code P3-1: matrix receipt collisions | **Closed** | Matrix receipt filenames include a SHA256 digest of the sorted distinct target set; the workflow uploads the qualified filenames. The actual CLI wrapper aggregation fixture combines two disjoint target sets and checks all three records/fingerprints survive. Single local builds retain their fixed receipt filename. |

## 1. Findings

### [P3-2] Candidate `--target-dir` remains advertised but is now ignored

**Location:** [build_candidate_extension.py](/Volumes/projects/limbo/gwz-dev/gwz-py/scripts/build_candidate_extension.py:195), lines 15–17,119–127,142–150 and 195–207; [sspi_backend.py](/Volumes/projects/limbo/gwz-dev/gwz-py/build_support/sspi_backend.py:102), lines 102–110.

**Violated invariant:** A supported option must have the behavior advertised by its help and default. Removing shared-cache capture must also reconcile callers that expose that location as an explicit option.

**Reproduction/state sequence:** Invoke the candidate recipe with `--target-dir /another-volume/candidate-target`. Its help promises that Cargo target directory, and `build_environment()` passes it as `CARGO_TARGET_DIR`. The corrected provisioned backend then unconditionally replaces that value with private extension and worker directories under `prepared.wheels`.

The candidate therefore accepts the option but builds on the destination wheel filesystem instead. Its default claim of `DESTINATION/target` is also stale. The existing candidate test checks the environment passed to the backend, so it does not detect the backend discarding the value.

**Impact:** A developer cannot use the advertised option to select build scratch placement or disk capacity. The recipe can consume space on the destination volume despite an explicit request for another build volume. Artifact coherence remains protected, so this is nonblocking.

**Required correction:** Reconcile the candidate contract with the new ownership policy. Either preserve scratch-root selection using unique build-owned directories, or explicitly retire/refuse the option and correct its help, introductory layout and tests. Do not leave it silently accepted without effect.

**Closure/regression test:** Trace the candidate option through the real backend handoff with subprocesses mocked. Assert that the selected scratch root controls build-owned target placement, or that the retired option refuses clearly before preparation/build work. Also assert the documented default matches the resulting layout.

**Classification:** A new bounded caller-interface defect exposed by the remediation’s intentional cache-policy change. It is not a new authentication architecture, wire, runtime ownership or native-platform root cause.

## 2. Invariant analysis

### Changed-range materiality

| Change | Classification and assessment |
|---|---|
| Private wheel/extension/worker staging | Material build-time ownership change addressing the existing publication/capture cause. Each build owns both Rust output trees and raw wheel capture; no shared mutable extension cache remains in provisioned builds. No new authentication process owner is introduced. |
| Atomic no-replace publication | Material packaging interface and filesystem primitive change. Complete artifacts are published with a same-filesystem hard link; existing destinations refuse rather than being replaced. The behavior and unsupported-filesystem failure are documented. No stale lock or indefinite wait is introduced. |
| Normalized metadata hooks | Material packaging/default interface correction addressing Code P2-1. One normalization path supplies explicit interpreter, target and profile to metadata and builds. No HTTP or runtime selection policy changes. |
| Python OS-string return | Bounded interface representation correction addressing Code P2-2. The Python result remains a filesystem string while retaining path identity. The OS loader algorithms and worker-selection rules are unchanged. |
| Rustflags identity inputs | Conservative fingerprint-input correction addressing the merged plan’s existing omission. Supplied encoded, ordinary, build-level and target-specific channels are recorded, including shadowed inputs. This is not a binary-attestation expansion. |
| Qualified matrix receipts | Bounded diagnostic naming/workflow correction addressing Code P3-1. Runtime authority remains compiled metadata. |
| Documentation and root settlement | Options, defaults, staging limitations and corrected member pins are recorded. No member dependency, lock, wire or reference-core implementation delta was introduced by remediation. |

No new material authentication architecture, session facade, process owner, carrier or platform activation cause was found. P3-2 is the remaining interface reconciliation consequence of the build-staging change.

The private publication path withstands the relevant failure sequences in source. Raw wheel capture occurs inside owned staging. Provisioning rewrites into a unique temporary file, validates the complete ZIP/RECORD and receipt, then replaces only the staged raw wheel. Final publication happens afterward. Capture or provisioning failure cannot expose a raw wheel at the final pathname or replace a prior complete destination.

Two builds targeting the same final name have separate staging trees. The first successful link establishes one complete destination; the other receives the documented collision refusal. Separate output directories remain independent even when callers supply the same ambient target cache. The new tests use the production handoff and bundler with barriers, synthetic fingerprints and failure injection, rather than merely checking temporary-directory names.

Source archives likewise adapt their backend privately, avoid retaining duplicate backend entries, verify the inserted backend bytes and publish only after completion. Unsupported hard links propagate failure without replacing an existing artifact. Platform/filesystem execution qualification remains separate.

The frontend-default attack now fails: the original 32-bit Windows case selects i686 before identification and uses the same selection in metadata and both builds. Explicit interpreter and target overrides remain visible in normalized arguments and recorded inputs. Candidate `--python` reaches that same frontend selection.

The path-conversion attack now fails: checked OS paths cross PyO3 through `OsString`, using its filesystem conversion. No replacement-character conversion remains. Fixed error classification and actual loaded-image provenance are retained.

The matrix-collision attack now fails for the actual disjoint cargo-dist target sets: qualified receipt names survive flattened aggregation. Receipts remain diagnostic and cannot provision or redirect a runtime host.

The retained SSPI boundary still holds. CLI early dispatch remains before ordinary initialization; descriptors create no Supervisor or process; Python selection still uses the actual loaded image and fixed adjacent filename; absent, malformed, missing and nonregular inputs refuse. Compiled Hello verification remains the mismatch boundary. Ordinary Cargo/editable installations remain unprovisioned. Modified Rust conditional sections retain enclosing boundaries and braced control-flow bodies.

## 3. Risks and next action

This GO closes the original Code blockers on the corrected committed tuple. It does not establish Windows installed Hello execution, Windows surrogate/runtime publication, complete Windows qualification, Linux installed-image qualification, HTTP/core composition, remote authentication/EPA, publication or CI success. The Darwin EILSEQ skip is accurately disclosed; conversion coverage is not presented as a native loader campaign.

Private target trees trade cache reuse for artifact coherence, and no-replace publication requires callers to use a fresh output directory for replacement builds. Those are explicit packaging contracts. P3-2 is the remaining inconsistency in their candidate caller.

The next action is to record this Code GO with P3-2 open and reconcile the candidate scratch-location option in a bounded follow-up. Aggregate hosts4a acceptance still requires the independent remaining verdicts; it does not close Plan step 4b or authorize activation or release.
