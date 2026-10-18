# Q4 Native Windows Qualification — Code-Axis Review

**Review object:** Q4 native Windows source/binding/library qualification at core `44d27ad665796cce669083bb8f03b2f194198f42`, controlled by `gwz-core/dev-docs/GwzGitLibraryWindows.md`; execution passed, retained review pending, 2026-09-21.  
**Baseline:** root `6a0e7661d66e28f27670be7717d23234dd107eba`; core `44d27ad665796cce669083bb8f03b2f194198f42`; evidence `b9456f7a8ba328dd3a35ab7577c0e66d3464cdc9`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable commits with `git show`/`git diff`.  
**Date:** 2026-09-21  
**Axis:** Code architecture, interfaces, call graphs, compatibility, and claim accuracy. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings.

---

## 0. Evidence base

I verified the complete tuple and tree objects before and after review. Core, evidence, gwz-git, git2-rs, and libgit2 were clean. Root contained only the two explicitly excluded generated review prompts.

I inspected:

- Core diff `deb0397c..44d27ad`, including `prove.py:127-172`, `test_prove.py:39-137`, the test-only `file_url` helpers, all four changed file-transport call sites, fixture README, and Q4/qualification documents.
- Evidence runner `campaigns/git-library/runner/windows.py:17-213`; frozen windows-a through windows-e records; mac-q4-e records; command statuses, environments, source snapshots, inputs, probe validation, and raw test output.
- Q3 boundary documents and the root checkpoint only for retained contracts and claim limits.
- The optional public guard suite: 14 tests passed on this host.

I independently confirmed that windows-e’s frozen runner equals the maintained runner; input fixture hashes match core `5f6cc919`; source-before/source-after differ only by the documented injected example; all 12,020 admitted git2-rs paths are otherwise identical.

## 1. Invariant analysis

The executable-mode correction is properly platform-bounded. On Windows it suppresses only comparison of suffix-derived executable bits. Exact file set, regular-file versus symlink type, bytes, link targets, revisions, and the nested C gitlink remain mandatory. POSIX still compares executable state in both directions. Tests cover Windows suffix disagreement, byte drift, file-to-link substitution, and POSIX executable drift. Native Windows admission exercised real source symlinks and executable Git entries.

Git-for-Windows preparation remains fail-closed. The runner accepts link rewriting only when the checked-out target differs solely by slash direction, then recreates a real symlink from the immutable Git blob. Any missing link, plain-file substitution, or other target difference fails. The retained b/c runs demonstrate exact-link and CRLF refusals; windows-e pins both `core.autocrlf=false` and `core.eol=lf` and then passes unchanged-byte admission.

The file-URL correction is confined to test fixtures. It derives the path from libgit2’s repository handle, emits a proper `file:///D:/...` form for the qualified Windows root, and percent-encodes unsafe UTF-8 path bytes. Explicit native file transport remains exercised before, during, and after the custom transport. Windows native proof and both-format consumer probe pass; the focused macOS proof and probe also pass. No production or library API changed.

The final runner enforces a fresh immediate child of `D:/gwz-tests`, fixture-owned HOME, Cargo home, Git configuration, temp and target directories, deliberate dependency acquisition followed by locked offline execution, empty-tools PATH for the probe, exact source pins, and before/after admission. The retained binary hash is checked across execution. Raw windows-a/b/c/d failures support the documented progression rather than being overwritten.

Claims remain bounded to Windows source admission, the nine-test native fixture, the existing G0 library suite, and the instrumented probe. The records support 13 passing Python guards plus one POSIX-only skip, nine native tests, 13 library integration tests, seven documentation checks, matching SHA-1/SHA-256 observations, and macOS regression. Complete Windows consumer qualification, other platforms, remote-only source reconstruction, publication, activation, and fallback removal remain explicitly deferred.

## 2. Risks and next action

The URL helper assumes the controlled fixture path is Unicode and is evidence code rather than a general filesystem API; all recorded roots satisfy that bound. The remaining consumer/platform and distribution gates are accurately left open.

The next action is to accept Q4 only if the independent parallel review also returns GO, then update the checkpoint without promoting any deferred activation or platform claim.
