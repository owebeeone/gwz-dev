# GwzGitLibraryWindows Remediation 1 — State-Axis Review

**Review object:** Focused Q4 State P2-1 closure; core `f1029847cd964a012ccec12495f897f444acdcd1`, evidence `040e3ab4db7871350650382d01d4f7c5f62d7615`  
**Baseline:** root `5d11a6c3444d4a58c774fc083db1dbbd4e621fd8`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable objects with `git show` and `git diff`.  
**Date:** 2026-09-21  
**Axis:** Focused configuration-isolation, failure-state, and evidence-attribution closure. Independent, adversarial, read-only. Nothing here relies on the concurrent axis.

**Verdict: GO** — State P2-1 is closed; no new P0–P3 findings.

---

## 0. Evidence base

Reviewed the committed remediation plan; core documentation diff `44d27ad6..f1029847`; evidence diff `b9456f7a..040e3ab4`; corrected 237-line `runner/windows.py`; 37-line `runner/test_windows.py`; and the complete retained `windows-f` README, inputs, status, configuration-guard output, native proof, library tests, probe output, source snapshots, and validation record.

Ran the permitted focused guard:

```text
python3.14 -B -m unittest discover \
  -s gwz-core-evidence/campaigns/git-library/runner -p test_windows.py
```

Result: one test passed.

The tuple matched at review start and end. Workspace noise was limited to the allowed generated State re-review prompt.

## 1. Prior-finding closure

| Finding | Original counterexample | Closure status |
|---|---|---|
| P2-1 — external ancestor Cargo configuration remained admitted | A `config` or `config.toml` under `D:/gwz-tests/.cargo`, the drive-root `.cargo`, a Cargo working directory, or the fixture Cargo home could alter compilation or test execution without appearing in source/lock snapshots. | **Closed.** `cargo_configuration` enumerates both names at the Cargo home and at the working directory plus every ancestor through the drive root. `lstat` makes regular files, directories, symlinks, and dangling symlinks fail closed; permission and unexpected filesystem errors propagate. The native and local guards inject current-directory, ancestor, home, and dangling-link cases and observe refusal. |

## 2. Changed-range and invariant analysis

Every scheduled Cargo-bearing step is wrapped by the check: proof fetch, native proof, library fetch, library tests, and probe build. Absence is recorded before and after each step. A pre-existing configuration prevents subprocess launch; a configuration remaining after execution converts the row to failure. There is no bypass option.

For the nested native proof, the retained record covers the shared external ancestors `D:\gwz-tests` and `D:\`, the fixture Cargo home, and the admitted outer proof source. Its additional temporary child directories are freshly created beneath the checked shared parent and populated only from that admitted fixture. This closes the original external-parent counterexample without weakening source admission.

`windows-f` uses the same input-manifest hash as `windows-e`, with core fixture `5f6cc919…` and unchanged gwz-git/Rust/C pins. The run uses a new `D:/gwz-tests/gwz-git-q4-20260921-f` root and fresh Cargo cache. Status records all five before/after path sets and reports every row passed. Raw output confirms 13 Python guards plus one POSIX skip, nine native-binding tests, 13 library integration tests, seven documentation checks, and the SHA-1/SHA-256 probe. The probe artifact remained stable and ran with the empty tools PATH.

No prior evidence was rewritten. The core delta is documentation-only, and its claims match the retained run. Source, lock, executable-mode, symlink, line-ending, file-URL, D: containment, and failure-retention boundaries from the initial review remain unchanged.

## 3. Risks and next action

This accepts only the executed Windows source-admission, native fixture, and G0 library rows. The remaining Windows consumers, other platforms, broader operations, independent source distribution, publication, production activation, and fallback removal remain separate gates. Q4 may now be recorded as accepted at this tuple.
