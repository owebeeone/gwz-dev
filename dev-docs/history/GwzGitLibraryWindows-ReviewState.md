# GwzGitLibraryWindows — State-Axis Review

**Review object:** Q4 native Windows source/binding/library qualification; core `44d27ad665796cce669083bb8f03b2f194198f42`, evidence `b9456f7a8ba328dd3a35ab7577c0e66d3464cdc9`, status “execution passed; retained review pending”  
**Baseline:** root `6a0e7661d66e28f27670be7717d23234dd107eba`; gwz-git `aa77c2ce5ad0bf6b4f64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. All reviewed sources were read from immutable objects with `git show`/`git diff`.  
**Date:** 2026-09-21  
**Axis:** Source-state admission, fixture/configuration isolation, failure retention, and source/lock/artifact attribution. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks. I pre-commit to GO on a revision that resolves P2-1 as specified and retains a corrected native Windows execution.

---

## 0. Evidence base

Reviewed the complete core range `deb0397c..44d27ad6`, including `GwzGitLibraryWindows.md`, the Q1/Q3 controlling documents, source verifier and guards, file-URL helper and four affected fixtures. Reviewed evidence policy, the 213-line Windows runner, final inputs, preparation scripts, status records, source snapshots and raw outputs for `windows-a` through `windows-e` and `mac-q4-e`.

The retained chain shows the expected progressive failures: executable-mode rejection, rewritten symlink targets, CRLF source drift, invalid Windows file URLs, then the final pass. Final logs report 13 passing Python guards plus one POSIX skip, nine native-binding tests, 13 library integration tests, seven documentation checks, and the both-format probe. No optional local test was run.

Rust 1.95’s bundled Cargo documentation confirms that Cargo loads `.cargo/config.toml` from the current directory and every parent directory before `$CARGO_HOME`.

The exact tuple matched at both start and end. Workspace noise was limited to the two allowed generated prompts.

## 1. Findings

### [P2-1] External ancestor Cargo configuration remains admitted

**Location:** `gwz-core-evidence/campaigns/git-library/runner/windows.py:76-103,169-190`; controlling requirement `GwzGitLibraryWindows.md`, “Use a fixture-owned Cargo home/config.”

The runner requires a fresh direct child of `D:/gwz-tests`, creates a fresh `CARGO_HOME`, and removes inherited `CARGO_*` variables. It does not inspect or reject `D:/gwz-tests/.cargo/config{,.toml}` or `D:/.cargo/config{,.toml}`. Cargo commands run from descendants such as `<root>/proof` and `<root>/gwz-git`, so Cargo automatically merges those external ancestor configurations.

Concrete sequence:

1. Place `D:/gwz-tests/.cargo/config.toml` before starting the fresh run, for example with an unrecorded `build.rustc-wrapper`, target runner, linker, source replacement, or injected environment.
2. Invoke the recorded `windows-e` command in a fresh child.
3. Cargo consumes that parent configuration despite the isolated `CARGO_HOME`.
4. The runner records only its environment and command line. Its source/lock snapshots can remain unchanged, and a forwarding or instrumenting wrapper can still produce passing rows.

The resulting artifacts and test execution are therefore not closed over the recorded fixture state. This breaks the explicit configuration-isolation and artifact-attribution gate; the retained evidence does not establish that such configuration was absent during `windows-e`.

**Required correction:** before the first Cargo command, enumerate Cargo’s effective search chain for every Cargo working directory and fail closed on any external ancestor `config` or `config.toml` (or admit and fingerprint an explicitly reviewed one). Record that preflight in status and raw output. Add a guard that creates a parent `.cargo/config.toml` and proves refusal. Then rerun the final native Windows rows under the corrected runner; a present-time filesystem check cannot reconstruct the configuration state of the completed run.

## 2. Invariant analysis

The other attacked boundaries held. Git bundles check out the exact Rust/C commits; admission reads immutable objects, verifies revisions and the nested gitlink, preserves bytes/types/file sets, and permits only separator-equivalent symlink reconstruction before rechecking exact targets. Windows executable bits are correctly excluded from equality while POSIX drift remains rejected. LF checkout preparation is explicit.

Runtime, checkouts, Cargo home, targets, temporary directories, HOME and fixtures are recorded beneath unique D: roots; installed tools alone remain on C:. Git global/system configuration is explicitly neutralized. Locks are retained, acquisition is separated from offline execution, the probe binary is hashed before and after execution, and its runtime PATH is empty.

Failures remain distinct and attributable across runs a–d. Public claims accurately limit Q4 to Windows source admission, the native fixture and G0 library; the remaining consumer matrix, platforms, distribution, publication and activation remain pending.

## 3. Risks and next action

Correct P2-1 and repeat the bounded final Windows qualification. No broader consumer, platform, publication, or activation conclusion follows from that rerun.
