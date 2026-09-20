# Q5 Windows Four-Consumer Qualification — State-Axis Review

**Review object:** Q5 Windows four-consumer qualification at root `97b01e752a6ac7709970f49daa24f017e762aebf`, controlling draft `gwz-core/dev-docs/GwzGitLibraryWindowsConsumers.md` at `54a04a381272806d9f6bddd2c319294ff63bcc79`; native execution passed, review pending.  
**Baseline:** root `97b01e752a6ac7709970f49daa24f017e762aebf`; gwz-core `54a04a381272806d9f6bddd2c319294ff63bcc79` (production-source baseline `95ad5b6cca0b2692598cbf8ae3d0381567658603`); gwz-core-evidence `0c1c34b85064bff2d8e75536ce3351063f74316b`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show <SHA>:<path>`.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics, source admission, isolation, failure retention, and fail-closed behavior. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0, P1, or P2 findings; one bounded P3 failure-attribution defect.

---

## 0. Evidence base

I read the complete Q5 prompt; `GwzGitLibraryWindowsConsumers.md` lines 8–106; the Q1/Q3 qualification boundaries; `EVIDENCE.md`, evidence-member instructions, and the current checkpoint; the evidence commit and its complete changed-file inventory; `runner/windows_consumers.py` lines 1–191; `test_windows_consumers.py` lines 1–59; unchanged `compose.py`, `native.py`, and `windows.py`; both source-preparation scripts and path guards; composition inputs/results/manifests; and the Q5 Windows README, `state.json`, command outputs, before/after snapshots, helper hashes, and owner-validation record.

Direct inspection confirmed four independent rows—workspace CLI, core example, standalone CLI, and Python extension—with distinct locks, source roots, Cargo homes, targets, runtime homes, and artifacts. Each recorded locked acquisition, offline metadata/build, exact graph/provider/features, native probe output, stable artifact hash, and unchanged source/link inventory. Python loaded the recorded `_gwz_core.dll` explicitly and checked `health() == "ok"`.

The permitted pure guards passed: four Q5 manifest/graph tests, the Q4 Cargo-ancestor configuration regression, and all source-preparation path counterexamples.

The exact tuple was verified at both review boundaries. Final workspace status contained only the two explicitly excluded owner-generated prompt files.

## 1. Findings

### [P3-1] A post-command configuration rejection can discard the command result

**Location:** `gwz-core-evidence/campaigns/git-library/runner/windows_consumers.py` lines 120–149, especially lines 139–146; contract at `GwzGitLibraryWindowsConsumers.md` lines 38–40 and 48–51.

The runner performs the post-command Cargo configuration check in a `finally` block before writing the completed subprocess’s stdout, stderr, return code, or final status. A concrete sequence is:

1. The pre-command configuration check passes and the record is saved as `running`.
2. Cargo completes and returns output and a return code.
3. A `.cargo/config`, `.cargo/config.toml`, or link at a checked Cargo-home/cwd-ancestor location is present at the post-command check.
4. `windows.cargo_configuration()` raises at line 141.
5. Execution skips lines 143–147. The outer handler correctly fails the row, but the command remains recorded as `running`; its output and return code are absent. A timeout can likewise be masked by the post-check exception, although its byte streams were already written.

This is fail-closed and cannot turn a contaminated run into a pass, so it does not invalidate the successful Q5 evidence. It does violate the stated failure-attribution and retained-attempt contract by losing whether the completed command itself succeeded and its diagnostics.

**Required correction:** persist subprocess output, return code, and terminal command state before the post-command check; record any post-check rejection separately; guarantee a final state save; then propagate the configuration error.

**Closure test:** mock a completed Cargo subprocess and make only the post-check raise. Assert that stdout, stderr, return code, completed-command status, and the separate configuration rejection all remain recorded. Cover the timeout-plus-post-check-error case as well.

## 2. Invariant analysis

Source admission held. Preparation rejects absolute, traversal, reserved, case-colliding, type-conflicting, and escaping-link paths. The Windows extraction reconstructs regular files and real links, then compares the full byte/type/link manifest. Every native row’s before/after snapshot matched its admitted manifest.

Graph and artifact independence held. Each row used its own resolved lock, source root, Cargo home, target, and artifact. Metadata selected the pinned gwz-git/git2-rs/libgit2 chain, sole native provider, and required vendored/SHA256/SSH/HTTPS features. Artifact hashes were unchanged by execution.

Build and runtime isolation held for the recorded run. Inherited Git/Cargo/Rust/libgit2/PyO3 variables were removed or replaced; HOME, Git, temporary, Cargo, target, and Python settings were fixture-owned; Cargo configuration absence was recorded around every Cargo invocation; and probe PATH was empty.

Native identity held. Both CLIs, the core example, and the explicitly loaded Python cdylib produced the expected SHA-1/SHA-256 identities and blob hashes, distinct commits, corrected local fetch result, and raw native error-class evidence. Helper and instrumentation hashes tie the Windows run to the admitted Q2/Q3/Q4 machinery. Public claims remain within instrumented native qualification and do not claim wheel behavior, ordinary command parity, publication, activation, or other platforms.

## 3. Risks and next action

The accepted evidence is a successful-run qualification and does not exercise crash recovery or the P3-1 post-check failure path. Repair P3-1 and add its focused regression before reusing this runner where failed-attempt diagnostics are part of a release decision. No State-axis blocker prevents accepting Q5 within its documented bounds.
