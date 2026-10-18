# GWZ SSPI installed hosts4a — State-AXIS REVIEW

**Review object:** Final narrow caller cleanup at root `a17a7b07becb1e92da5519b64add39c41d6ee09e`, controlled by `dev-docs/GwzSspiHostsCheckpoint.md`. The checkpoint remains DRAFT pending final acceptance.

**Baseline:**

| Repository | Reviewed HEAD |
|---|---|
| gwz-dev | `a17a7b07becb1e92da5519b64add39c41d6ee09e` |
| gwz-sspi | `14d834b311200b0984e7041e4a34419c59501119` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-core, unchanged reference | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |

Sources were read through inspection-only Git commands and clean member files. All five HEADs matched at the start and end. SSPI, CLI and Python members were clean. Untracked prompts and unrelated root/core files were excluded.

**Date:** 2026-10-04

**Axis:** State — build-artifact ownership, capture races, publication ordering and recovery. Independent, adversarial, read-only. Nothing here relies on current peer reports. Filed verbatim by the lane owner.

**Verdict: GO** — both original State P2 findings remain closed. No new finding or architectural root cause was established.

---

## 0. Evidence base

This review inspected the complete four-file Python range `c3f5f7d..ded4713`:

- `build_support/sspi_backend.py`, particularly lines 79–119.
- `scripts/build_candidate_extension.py`, its changed help/documentation and existing production handoff at lines 121–153.
- `src/tests/test_worker_packaging.py`, particularly synthetic handoff and coherence checks at lines 168–207, the new root-selection fixture at lines 209–239, concurrent publication at lines 241–259 and fault preservation at lines 261–281.
- `docs/HostPackaging.md`, including scratch-root ownership, delegated option defaults and publication behavior.

The canonical focused prompt, prior State reviews, merged remediation plan and root settlement notes were legitimate inputs. Root changes to the checkpoint, program status, member lock and integrity marker were inspected. Current peer prompts/reports remained unread.

Git inspection confirmed SSPI and CLI have no changes from the previously reviewed remediation-1 HEADs. Core remains at the same reference HEAD. The four-file Python cleanup changes no compiled Rust source, dependency, lockfile or protocol.

Root records distinguish the final recipe tests from the previously built, unchanged native image: they report 53 passing focused cases and one EILSEQ skip, without claiming a fresh packaged build or native qualification. Those are owner-recorded gates, not reviewer-executed tests.

No writes, builds, test execution, compiler probes, native execution or Git mutations were performed.

## 1. Prior-finding closure

| Original State finding | Final status | Evidence |
|---|---|---|
| **P2-1 — shared, already-published wheel postprocessing** | **Remains closed** | Rust output moves into a unique directory beneath the selected scratch root. Raw-wheel capture, bundling and validated no-replace publication retain their private transaction boundary beneath the output directory. Concurrent and fault-preservation regressions remain present. |
| **P2-2 — omitted `CARGO_BUILD_RUSTFLAGS`** | **Remains closed** | SSPI producer and sensitivity tests are unchanged from the independently reviewed correction. Selecting a scratch root does not remove or alter recorded compiler-option inputs. |

No new State finding was established.

## 2. Invariant analysis

### Complete changed-range classification

| Change | Classification |
|---|---|
| Selected scratch root with unique worker/extension directories | **Material build-output placement correction, preserving ownership.** The supplied root is honored without becoming a shared mutable target tree. No runtime ownership or architectural change. |
| Candidate help and layout description | **Caller interface clarification.** Explicit `--target-dir` and default `DESTINATION/target` now describe the production behavior. |
| Root-selection and cleanup regressions | **Focused coverage addition.** Tests traverse the production candidate/backend handoff with synthetic subprocess artifacts and the actual bundler. |
| Delegated option/default documentation | **Documentation-only clarification.** No option parser, metadata normalization or build-policy behavior changes in this range. |
| Root settlement records | **Administrative bookkeeping.** They preserve qualification limits and distinguish unchanged compiled-image evidence from final recipe tests. |

There is no new architectural root cause, supervisor/process owner, wire change, core change, platform activation or installed-descriptor change.

### Scratch selection preserves isolation

`build_wheel()` resolves the supplied `CARGO_TARGET_DIR` as a scratch root, defaulting to the wheel output directory when absent. It creates a unique `gwz-sspi-target-*` directory beneath that root.

The extension and worker commands receive distinct private target paths beneath this unique directory. Two simultaneous builds using the same supplied root therefore cannot overwrite each other’s compiler outputs. The caller’s environment is still copied rather than mutated.

The new regression exercises explicit candidate selection, the candidate default and the ordinary backend default. It checks that both target paths belong beneath the selected root, differ from each other and are removed after the build. It also checks disposal of both private staging directories and coherence of the completed wheel.

### Separate filesystems do not weaken publication

The wheel transaction directory remains beneath the final output directory. The raw wheel is captured from the private compiler output through `shutil.copyfile()`, then provisioned and validated in that transaction directory.

Consequently, selecting scratch on another filesystem does not turn the final publication into a cross-filesystem hard link: the link still joins staged wheel and destination beneath the output filesystem. The fixture named `another-volume` establishes selection through the handoff; it is not claimed as an actual cross-volume runtime test.

Raw-wheel containment remains checked against the private extension target. There is no fallback to a pre-existing wheel in the caller-selected root.

### Failure and race states remain closed

The original publication counterexample cannot recur:

- Compiler outputs and raw wheels remain private to one build.
- The bundler retains its unique temporary file.
- Capture and provisioning do not write the final filename.
- Publication retains atomic no-replace semantics.

The concurrent regression still requires one coherent winner for coincident destination names and two coherent outputs for separate destinations. Fault cases preserve the prior completed wheel or leave the final filename absent, and now also verify scratch cleanup after handled capture/provisioning failures.

An abrupt kill can leave private scratch. Before publication it exposes no raw final wheel; after publication it can leave a complete final wheel and private residue. No stale lock or shared writable published inode is introduced. Existing documented collision refusal and fresh-output recovery remain applicable.

### Prior boundaries remain intact

The cleanup does not change fingerprint production, metadata normalization, compiled runtime authority, OS-loaded-image selection, missing/symlink refusal or Hello-before-Begin verification. The independently reviewed remediation-1 conclusions remain applicable to those unchanged components.

## 3. Risks and next action

This focused GO establishes that scratch-root selection preserves the closed capture/publication invariants. It does not add Windows filesystem execution, installed Hello, Linux qualification, HTTP composition, publishing availability or power-loss durability proof.

The next action is to record this final **State GO** for the exact tuple in the owner’s acceptance merge, subject to the independently obtained remaining verdicts. Plan step 4 and the deferred runtime/release gates remain open.
