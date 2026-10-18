# gwz-git G0 Focused Remediation — CODE-AXIS REVIEW

**Review object:** Focused G0 correction at library `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`, Rust fork `ce78628308e11b4e8901d5061602619109bce21a`, and core checkpoint `dev-docs/GwzGitLibraryG0.md` at `2a5bd773df04450148c7630e01913edba2bbedb8`; status **corrected implementation awaiting re-verdict**, dated 2026-09-21.  
**Baseline:** root `699c584a93185ef5e73dc96318603e25354018e2`; core `2a5bd773df04450148c7630e01913edba2bbedb8`; library `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects after exact-commit verification.  
**Date:** 2026-09-21  
**Axis:** Architecture, interfaces, native call paths, ownership, error preservation, and changed-range compatibility. Independent, adversarial, read-only. The other axes run independently; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: GO** — original P2-1, P2-2, and P3-1 are closed; no new P0–P3 finding arose from the bounded correction.

---

## 0. Evidence base

I retraced the three original counterexamples and inspected only their correction and changed-boundary interactions:

- library diffs from `fcf5296095d230f46c8409a8d6d47b44f4ad918b`, especially `src/commit.rs:25-62,90-96`, `src/error.rs:12-24`, `src/lib.rs:29-49`, and the two new native regressions;
- fork diff from `4c1caabbce7d56426c763dd94114052302b23e4c`, limited to `src/error.rs:305-325,370-388`;
- pinned libgit2 commit parsing and graft application in `commit.c:394-566` and exact OID-header validation in `object.c:644-670`;
- core pin manifest, three-file patch, malformed-grafts regression, qualification README, design source-pin update, and checkpoint remediation record at lines 85–124;
- root lock composition and accepted remediation scope.

The fork descends from the previous pin and changes only `src/error.rs`. The two accepted transport files and C gitlink are byte-identical. I independently matched the committed patch digest and upstream/patched `src/error.rs` hashes to the pin manifest.

I did not repeat the suites. The committed checkpoint records red demonstrations on the old tuple, Rust 1.95 fmt/check/test/clippy, 13 integration tests, seven documentation checks, source proof before and after library checks, archive proof, ten admission guards, and the isolated raw-class replay unit test.

The tuple was exact and unchanged at both boundaries. End trees were root `70a9a49ef8209314d9115c560703cc3641b7ab5a`, core `a2a954cdbd909bcdefc645703685a3d442c651f7`, library `a32df35d6ba9c27019ae2a4928325d25bef41a29`, fork `28fc6ad905653db47a7705706e1edea0f39d7054`, and C `af52de522aacf3593250c45159a038801153c6b5`. Worktrees remained clean apart from the three allowed owner-generated review prompts.

## 2. Invariant analysis

| Prior finding | Status | Closure evidence |
| --- | --- | --- |
| P2-1: graft/shallow rewrites stored parent record | **Closed** | `read_commit` retains native `find_commit` validation and signature parsing, but now obtains the tree and ordered parents from the raw ODB header. Pinned native parsing requires the tree first and full-width repeated parent IDs before applying grafts, so the library parser does not weaken admission. SHA-1 and SHA-256 regressions demonstrate both shallow and explicit-graft rewrites in native traversal while G0 returns the original tree and `[first, second]` parent order. Each test rereads the metadata bytes unchanged. |
| P2-2: native class lost by `raw_class()` | **Closed** | The getter now returns `self.klass` through the existing raw integer alias, and callback replay passes that value to `git_error_set_str`; the safe `class()` mapping remains unchanged. Unit rows cover known, newly added, arbitrary, and negative values, including replay. Both G0 and proof-package malformed-grafts tests use the independent pinned-C value `GIT_ERROR_GRAFTS = 36`, closing the original class-zero counterexample. Archive and exact-source qualification include the corrected file through the existing admission mechanism. |
| P3-1: extra public error traits | **Closed** | `Error` and `NativeDiagnostic` now derive only `Debug`; `Clone`, `Eq`, and `PartialEq` are absent. Separate compile-fail checks cover cloning and equality for both public types. No accepted error accessor or signature changed. |

The raw-header parser’s use of an OID routine that can parse abbreviated text does not admit abbreviated stored fields: native `find_commit` runs first, and pinned libgit2 requires the exact repository-format width and terminating newline for every tree and parent header. Missing references remain representable because native validation parses IDs without resolving the referenced objects.

On non-MSVC targets the raw class alias is unsigned, while the stored C field is signed; the explicit cast back to `i32` preserves the bit pattern. The recorded `-1` observation-and-replay row exercises that boundary. MSVC uses the signed alias directly.

Cargo manifests, locks, versions, features, native C, operation APIs, and production activation are unchanged. The updated workspace lock selects precisely the reviewed core, library, and fork commits.

## 3. Risks and next action

Unexecuted native platforms, publication, clean remote-only reconstruction, and production activation remain the previously declared gates. They do not block this local G0 Code verdict.

File this GO and combine it with the independent current-axis verdicts. No further Code remediation is required for P2-1, P2-2, or P3-1.
