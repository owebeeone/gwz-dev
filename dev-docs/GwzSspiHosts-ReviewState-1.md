# GWZ SSPI installed hosts4a — State-AXIS REVIEW

**Review object:** Installed hosts4a remediation 1, controlled by `dev-docs/GwzSspiHostsCheckpoint.md` at root `6b670283179f45f0dbbe38b1f6c300b326e50100`. The checkpoint remains DRAFT pending independent acceptance.

**Baseline:**

| Repository | Reviewed HEAD |
|---|---|
| gwz-dev | `6b670283179f45f0dbbe38b1f6c300b326e50100` |
| gwz-sspi | `14d834b311200b0984e7041e4a34419c59501119` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `c3f5f7d6b614155e413db0036242849010ef1149` |
| gwz-core, unchanged reference | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

Sources were read through read-only Git inspection and clean member files. All five HEADs matched at the start and end. SSPI, CLI and Python members were clean. Untracked prompts and unrelated root/core files were excluded.

**Date:** 2026-10-04

**Axis:** State — failure ordering, artifact ownership, publication races, recovery states and fail-closed installed selection. Independent, adversarial, read-only. Nothing here relies on a current parallel-axis report. Filed verbatim by the lane owner.

**Verdict: GO** — both original State P2 findings are closed; no new P0, P1, P2 or P3 finding was established. Material changes beyond the original State fixes were independently inspected and classified below.

---

## 0. Evidence base

The focused remediation ranges were:

- SSPI `e31b17e..14d834b`.
- CLI `061385f..0c7dfaf`.
- Python `e9e228c..c3f5f7d`.
- Root changes through `6b67028`, within the cumulative `3066630..6b67028` acceptance object.

I read the canonical remediation prompt, my original `GwzSspiHosts-ReviewState.md`, the merged `GwzSspiHosts-RemPlan.md`, the revised controlling checkpoint and program checkpoint. Accepted design revision 2 §§2 and 4–6, plan step 4 and prior native acceptance remain controlling authority.

The complete eleven-file member remediation was inspected:

| Repository | Files and principal evidence |
|---|---|
| SSPI | `scripts/artifact_set.py:56–134`; `tests/schema/test_artifact_set.py:38–83`; `docs/HostPackaging.md` |
| CLI | `scripts/build_sspi.py:20–50`; release workflow receipt glob; `docs/HostPackaging.md` |
| Python | `build_support/sspi_backend.py:28–170`; `native/src/lib.rs` descriptor conversion; `native/src/worker_host.rs` conversion fixtures; `src/tests/test_worker_packaging.py:18–329`; `docs/HostPackaging.md` |

Root implementation/status documentation and member-lock settlement changes were inspected. Filed peer reports and peer prompts were not read. Their presence in Git path inventories supplied no verdict evidence.

Additional dependency source inspection covered:

- Maturin’s installed metadata and source-distribution hooks, including its explicit source-distribution output directory.
- PyO3 0.28.3 `conversions/std/osstr.rs:23–73`, `90–148` and `197–207`, establishing the filesystem-string conversion used by the changed public descriptor.

The checkpoint and merged plan record owner portable gates, including the focused installed-wheel suites, producer/release tests and source-boundary checks. They also record Darwin wheel, sdist and extracted-sdist builds, the filesystem skip and unexecuted Windows rows. These were assessed as recorded evidence, not reviewer-executed tests. Initial-round external artifacts were not treated as proof of this corrected tuple.

No writes, builds, tests, compiler probes, native execution or Git mutations were performed.

## 1. Prior-finding closure

| Original State finding | Result | Independently inspected closure |
|---|---|---|
| **P2-1 — shared, already-published wheel postprocessing** | **Closed** | Backend-owned extension and worker outputs; private raw-wheel capture; unique bundler temporary file; complete RECORD validation; atomic no-replace publication. Real handoff/bundler fixtures cover coincident names, separate output directories, capture/provisioning failures and publication refusal. |
| **P2-2 — omitted `CARGO_BUILD_RUSTFLAGS`** | **Closed** | The producer records build-level, ordinary and encoded rustflags, alongside target-specific environment channels and configuration digests. Fixed-input tests distinguish unset/set and differing build-level flags with higher-priority channels present. Compiler-wrapper refusal remains intact. |

### P2-1 retrace

`build_wheel()` now creates a unique staging directory beneath the resolved output directory. It overrides the copied extension environment’s `CARGO_TARGET_DIR` with build-owned storage and retains a separate worker target directory. `maturin_wheel()` refuses a provisioned raw wheel outside that owned extension output.

The raw wheel is copied into private staging. `bundle()` uses `mkstemp()` instead of the old deterministic `.tmp` path, closes the ZIP writer, validates the resulting archive and RECORD, and replaces only the private staged wheel. The caller-visible filename is created afterward by `os.link()`.

The original interleaving no longer has a shared writable inode:

- A and B have separate compiler outputs, raw wheels and bundler temporary files.
- Neither writes the final wheel during capture or provisioning.
- At publication, one same-name link succeeds; the other receives an explicit collision.
- No writer continues writing the published inode. Staging cleanup unlinks its private names rather than changing the completed file.

The regression at `test_worker_packaging.py:209–227` uses the actual handoff and actual bundler, with synthetic subprocess artifacts and different thread fingerprints. It requires one coherent success for coincident output names and two coherent successes for separate output directories despite the same ambient cache setting. This addresses the original mocked-handoff limitation.

The failure cases at lines 229–248 partially write capture/provisioning output and then raise. They verify preservation of a prior completed destination or absence of a final wheel when none existed. Publication refusal/interruption cases at lines 300–308 verify no final artifact and cleanup for handled failures.

A process killed before publication can leave private scratch, but cannot expose a raw wheel under the final filename. A kill after the link can leave a complete final wheel plus scratch. Retrying that filename refuses explicitly; the documented recovery is a fresh output directory. No stale lock state or indefinite wait was added.

### P2-2 retrace

`artifact_set.py:73–80` now includes `CARGO_BUILD_RUSTFLAGS` in recorded build environment inputs and records all three supplied general rustflags channels separately. Target-specific channels remain covered by the `CARGO_TARGET_` projection; conventional Cargo configuration files remain digested.

Changing only `CARGO_BUILD_RUSTFLAGS` therefore changes the fingerprint input. The original generic/native target-CPU collision is removed. Conservative recording of shadowed channels is permitted by the original remedy and explicitly documented.

The fixed-resolution/compiler regression at `test_artifact_set.py:52–64` covers absent versus supplied build-level flags and differing values with ordinary, encoded and target-specific flags present. Existing compiler executable/wrapper refusal tests remain.

No new findings were established.

## 2. Invariant analysis

### Changed-range classification

The remediation contains material build and conversion changes; closure was not inferred solely from the initial conditional pre-commit.

| Change | Classification and State assessment |
|---|---|
| Private extension, worker and raw-wheel staging | **Material build-artifact ownership correction.** Removes shared mutable capture. It introduces no authentication process owner, supervisor or runtime architecture. |
| Atomic no-replace hard-link publication | **Material publication/interface and filesystem requirement.** Same-name output now refuses, and unsupported filesystems propagate failure. These semantics and fresh-directory recovery are documented. No overwrite fallback weakens the invariant. |
| Unique bundler temporary writes and RECORD validation | **Material artifact integrity boundary.** The final publication follows closed, validated writes. Validation checks archive names, RECORD membership, hashes/sizes, receipt identifier and packaged worker presence. |
| Private source-archive adaptation | **Material build publication ordering.** Maturin receives the private sdist directory; backend replacement occurs privately and is checked before final no-replace publication. No extracted-source layout or runtime owner is added. |
| Shared frontend interpreter/target normalization | **Material packaging interface/platform correction.** Metadata, wheel and editable hooks use the same normalization. Explicit interpreter/target choices remain explicit; the selected 32-bit Python on AMD64 Windows produces the i686 default before fingerprinting. |
| `OsString` descriptor conversion | **Material platform-path correctness correction.** Python still receives a filesystem string; the implementation preserves path identity through PyO3 instead of returning a lossy changed pathname. OS image selection itself is unchanged. |
| Target-set-qualified matrix receipts | **Material diagnostic artifact naming correction.** Disjoint matrix target sets survive flattened aggregation. Receipts remain diagnostic and do not become runtime authority. |
| Wrapper validation/default documentation | **Bounded interface clarification and refusal tightening.** Local/matrix modes, profile defaults, target forms, outputs and collision behavior agree with the inspected wrappers. |
| Root settlement/status records | **Administrative acceptance bookkeeping.** No core, wire, dependency, runtime-owner or activation change. |

No new architectural root cause was established in this range.

### Publication and recovery

The final wheel states are now a closed set: absent, previously complete, or newly complete. Partial capture, partial ZIP writing and validation failure remain private. Existing destinations cannot be replaced by publication, including a symlink or other same-name entry.

This is a process-failure and concurrency assessment. The implementation does not establish power-loss durability through an fsync protocol, and this review does not attribute such a guarantee to it.

### Metadata and platform choices

`settings()` selects one interpreter and target before identification. The producer records the normalized arguments; worker and extension commands receive the same target/profile. The metadata hook consumes the same normalized arguments.

The process-free regressions cover the AMD64/32-bit frontend case and explicit overrides, and compare metadata arguments, producer inputs and actual generated build command arguments. Candidate launching continues to make its selected Python the frontend interpreter. These tests establish command coherence, not Windows runtime qualification.

### Filesystem path identity

The public conversion now retains the descriptor’s `OsString`. PyO3 uses filesystem decoding on Unix and wide-character conversion on Windows. The new Unix fixture round-trips the public conversion through `os.fsencode`; the Windows fixture expresses the corresponding unpaired-surrogate round trip.

Darwin’s refusal to create the actual non-UTF8 fixture directory is recorded as a skip. It is not silently counted as an installed-loader success. Windows runtime conversion remains unexecuted. Source inspection supports the conversion correction without inflating either qualification.

### Preserved runtime boundaries

The remediation does not change compiled fingerprint authority, OS-loaded-image selection, fixed adjacent worker naming, missing/nonregular/symlink refusal, CLI early bootstrap ordering or the existing Hello-before-Begin comparison.

Ordinary Cargo and editable installations remain unprovisioned. No runtime environment, receipt, cwd or PATH fallback was added. No new supervisor/process owner, core adapter, wire message or HTTP activation appears in the changed range.

The receipt and fingerprint changes affect future complete artifact sets together. Retained old images encountering a differently provisioned worker still meet the existing mismatch boundary rather than acquiring a fallback.

## 3. Risks and next action

Windows hard-link publication, surrogate conversion, complete installed host execution and installed Hello mismatch remain unexecuted qualification rows. Linux installed-image qualification, HTTP/core composition, remote authentication and publishing availability remain deferred. The Darwin filesystem skip is explicit.

These limits do not leave either original State finding open. The next action is to record this **State GO** in the owner’s acceptance merge for the exact corrected tuple, subject to the independently obtained remaining verdicts. This report does not close plan step 4, authorize publication or activate Windows authentication.
