# GWZ SSPI installed hosts4a — Code-AXIS REVIEW

**Review object:** Final focused caller cleanup, controlled by `dev-docs/GwzSspiHostsCheckpoint.md` at root `a17a7b07becb1e92da5519b64add39c41d6ee09e`. The checkpoint remains DRAFT pending final independent closure.

**Baseline and reviewed tuple:**

| Repository | Prior GO baseline | Reviewed HEAD |
|---|---|---|
| gwz-dev | `6b670283179f45f0dbbe38b1f6c300b326e50100` | `a17a7b07becb1e92da5519b64add39c41d6ee09e` |
| gwz-sspi | `14d834b311200b0984e7041e4a34419c59501119` | unchanged |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` | unchanged |
| gwz-py | `c3f5f7d6b614155e413db0036242849010ef1149` | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-core, reference | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` | unchanged |

**Date:** 2026-10-04

**Axis:** Architecture, interfaces, call graphs, compatibility and error paths. Independent, adversarial, read-only. Nothing here relies on current peer reports. Filed verbatim by the lane owner.

**Verdict: GO** — Code P3-2 is closed; all earlier Code closures remain valid. No open Code finding or new architectural root cause was found.

---

## 0. Evidence base

All five HEADs matched the canonical prompt at the start and end. SSPI, CLI and Python working trees were clean. Root and reference core contained the disclosed untracked documentation. No reviewed HEAD moved. Current peer reports/prompts were not read.

The review read the complete canonical Code-2 prompt, my prior Code report, the merged remediation plan and root settlement notes. It inspected the complete four-file Python range `c3f5f7d..ded4713`:

- `build_support/sspi_backend.py`, particularly lines 79–119 and the unchanged metadata/editable hooks.
- `scripts/build_candidate_extension.py:6–20,121–153,192–210`.
- `src/tests/test_worker_packaging.py:168–281`, including the new root-selection fixture and retained coherence/concurrency/failure cases.
- The complete `docs/HostPackaging.md` change.

Root inspection covered checkpoint/program/remediation updates and the changed Python member pin/integrity record. SSPI, CLI and reference core had no changes from the prior GO tuple.

Python and targeted root diff checks passed. Inspection used Git HEAD/status/diff checks and text reads only. No files were written, no Git state was mutated, and no builds, tests, compiler probes, helpers or native processes were executed.

The owner-recorded final gates report 53 focused-suite passes with one EILSEQ skip using the previous unchanged native image, and 52 unprovisioned-suite passes with two opt-in fixture skips. These are recorded owner results, not reviewer executions. Root notes correctly distinguish final Python recipe testing from a fresh packaged artifact or native qualification.

### Prior-finding closure

| Finding | Status | Closure basis |
|---|---|---|
| Code P2-1: interpreter-dependent target defaults | **Closed, preserved** | This cleanup does not change normalization, interpreter probing, metadata hooks or target/profile arguments. The prior single-choice call graph and regression evidence remain valid. |
| Code P2-2: lossy filesystem path | **Closed, preserved** | No native source changed. The checked path still crosses PyO3 through `OsString`; prior conversion closure and its platform execution limits remain unchanged. |
| Code P3-1: matrix receipt collisions | **Closed, preserved** | CLI and SSPI are unchanged. Qualified receipt filenames and the aggregation correction remain intact. |
| Code P3-2: ignored candidate `--target-dir` | **Closed** | Candidate selection reaches `CARGO_TARGET_DIR`; the backend now uses it as the scratch root and allocates unique private worker/extension target trees beneath it. Help and layout documentation describe that behavior. The production handoff fixture covers explicit candidate selection, the candidate default and the ordinary backend default. |

## 2. Invariant analysis

The original P3-2 counterexample no longer holds. With `--target-dir /another-volume/candidate-target`, the candidate supplies that root to the backend. `build_wheel()` resolves it at line 102 and creates a unique `gwz-sspi-target-*` directory beneath it. Worker and extension receive distinct target trees inside that private directory. Without the option, the candidate selects `DESTINATION/target`; without ambient `CARGO_TARGET_DIR`, the ordinary backend selects the wheel output directory.

The new fixture traces the actual candidate-to-backend handoff with subprocesses mocked. It checks both target paths lie beneath the selected root, differ from each other and from the root, and disappear after completion. It also checks transaction staging is removed and the resulting wheel remains coherent. This addresses the former fixture gap, which checked only the environment sent to the backend.

Scratch selection does not restore shared mutable artifact capture. Concurrent builds sharing one root receive separate temporary directories. The retained barrier tests still exercise the production handoff and bundler for identical wheel names, while fault tests preserve a previous complete destination or its absence. The added fault assertion checks disposal of private scratch.

Wheel transaction staging remains beneath the output directory. Raw wheel bytes may be copied from the independently selected scratch filesystem into owned transaction staging; provisioning, validation and final no-replace publication then use the output filesystem. The final hard link therefore remains between staged and destination files on that filesystem. No incomplete raw wheel is published at the final pathname.

### Changed-range classification

| Change | Classification |
|---|---|
| Scratch-root selection | Bounded build-time placement/interface correction for the existing P3-2 cause. It changes where private Rust target trees live, while preserving exclusive ownership and disposal. |
| Candidate help/layout | Contract correction describing the option’s effective root, default and cleanup behavior. |
| Regression additions | Production-handoff coverage for explicit/default roots and cleanup, retaining existing coherent publication tests. |
| Delegated-option documentation | Documentation clarification; no corresponding normalization or delegated-option behavior change occurs in this range. |
| Root settlement records | Updated provenance and member pin, with honest separation of previous native-image evidence from final recipe testing. |

No new architecture, authentication ownership, process owner, wire, dependency, core implementation or platform activation cause was introduced. Compiled fingerprint authority, loaded-image selection, early CLI dispatch, normalized metadata hooks, source-archive staging and atomic collision refusal remain unchanged.

## 3. Risks and next action

The final correction is supported by source inspection and recorded focused recipe tests. It is not a fresh packaged-build, cross-filesystem runtime or Windows native qualification proof. The earlier EILSEQ limitation, unexecuted Windows surrogate/runtime publication and installed Hello gates remain accurately disclosed.

HTTP/core composition 4b, platform qualification, remote authentication/EPA, publication, CI and activation remain outside this verdict.

The next action is to record this final Code GO, close Code P3-2 and merge the independent focused verdicts into the hosts4a acceptance record.
