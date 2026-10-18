# GWZ-GIT G0 FOCUSED REMEDIATION — STATE-AXIS REVIEW

**Review object:** Focused G0 remediation at `gwz-git` `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`, git2-rs `ce78628308e11b4e8901d5061602619109bce21a`, and supporting proof/checkpoint changes at `gwz-core` `2a5bd773df04450148c7630e01913edba2bbedb8`; local implementation candidate, unpublished and not production-activated  
**Baseline:** root `699c584a93185ef5e73dc96318603e25354018e2`; `gwz-core` `2a5bd773df04450148c7630e01913edba2bbedb8`; `gwz-git` `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Changed ranges were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics and adversity at raw commit extraction, graft/shallow isolation, native error-class preservation, and exact-source composition. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on its current report. Filed verbatim by the lane owner.

**Verdict: GO** — zero new P0, P1, P2, or P3 findings. The State recheck passes for all three prior findings; formal closure ownership for the original Code findings remains with the Code reviewer.

---

## 0. Evidence base

I read:

- The accepted `GwzGitLibraryG0-RemPlan.md`.
- Complete library diff `fcf5296095d230f46c8409a8d6d47b44f4ad918b..aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`, especially `src/commit.rs`, `src/error.rs`, `src/lib.rs`, `tests/native_baseline.rs`, and `README.md`.
- Complete fork diff `4c1caabbce7d56426c763dd94114052302b23e4c..ce78628308e11b4e8901d5061602619109bce21a`.
- Core proof changes in `binding-pin.json`, `patches/git2-per-remote.patch`, `tests/local_fetch.rs`, the proof README, and `GwzGitLibraryG0.md`.
- Pinned libgit2 commit parsing, graft application, raw ODB verification, and native error definitions.

The corrected Rust 1.95 suite passed with an external target directory:

- Four foundation tests.
- Nine native-baseline tests.
- One compiling usage doctest.
- Six compile-fail ownership/trait doctests.
- No ignored or failed tests.

`cargo metadata --locked --offline` resolved one local git2 0.21.0 and one local libgit2-sys 0.18.8+1.9.7, selecting only `unstable-sha256` and vendored features.

Source-composition checks established:

- `ce786283...` descends from `4c1caabb...`.
- Its only changed path is `src/error.rs`.
- `src/remote_callbacks.rs`, `src/transport.rs`, and the entire `libgit2-sys` subtree are byte-identical to the prior fork.
- The nested C gitlink remains `b172e3d...`.
- Immutable stock and corrected `src/error.rs` hashes match `binding-pin.json`.
- The committed patch and proof-lock hashes match their pins.
- Root lock state selects the reviewed core, library, fork, and C commits.

I did not duplicate the full native source/archive proof. The runner is unchanged; I inspected its exact-tree/hash admission and verified the newly pinned bytes. The checkpoint records nine source-mode tests before and after library checks, nine archive-mode tests, ten admission guards, and the isolated raw-class callback-replay unit.

Start and end tuple checks matched:

| Repository | Commit | Tree |
|---|---|---|
| root | `699c584a93185ef5e73dc96318603e25354018e2` | `70a9a49ef8209314d9115c560703cc3641b7ab5a` |
| gwz-core | `2a5bd773df04450148c7630e01913edba2bbedb8` | `a2a954cdbd909bcdefc645703685a3d442c651f7` |
| gwz-git | `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c` | `a32df35d6ba9c27019ae2a4928325d25bef41a29` |
| git2-rs | `ce78628308e11b4e8901d5061602619109bce21a` | `28fc6ad905653db47a7705706e1edea0f39d7054` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |

All members remained clean. Root contained only the three permitted owner-generated review prompts.

## 2. Invariant analysis

| Prior finding | Corrected evidence | State status |
|---|---|---|
| Code P2-1: graft/shallow traversal state replaced stored parents | Native lookup still validates the commit, while tree and contiguous ordered parent headers are parsed from the strictly verified raw ODB object. SHA-1 and SHA-256 tests cover shallow and reversed explicit-graft rewrites and assert metadata bytes remain unchanged. | **State boundary closed** |
| Code P2-2: unknown native error classes collapsed to zero | `raw_class()` returns the stored class integer through the existing raw alias; callback replay uses that raw value; safe enum mapping remains unchanged. Malformed grafts produce class 36 in G0, and proof regressions cover known/new/arbitrary classes independently of the old getter. | **State boundary closed** |
| Code P3-1: extra public error traits | Extra derives were removed, and six ownership/trait compile-fail doctests passed. | **No State regression** |

**Stored content is isolated from traversal metadata.** Pinned libgit2 requires the tree header first and contiguous parent headers next, matching the new raw parser. Native parsing still validates signatures and commit grammar before extraction. Graft or shallow state may alter native traversal, but cannot alter the returned stored tree/parent sequence. The library neither edits nor suppresses those metadata files.

**Failure direction remains closed.** Raw IDs are parsed using the repository’s SHA format and any extraction failure becomes `ObjectRead`; no partial/default record is returned. If the ODB read fails after native validation, the operation fails. External metadata changes during a read may affect native traversal validation, but returned tree/parents derive only from the content-addressed raw object.

**Native diagnostics are lossless.** The binding stores native classes as `c_int`; the corrected getter preserves all bits through the platform’s existing raw alias and converts back to `c_int` for callback replay. Classes 36, 12345, and `-1` survive observation/replay while safe `class()` continues mapping unknown values to `None`. G0 copies that raw class into owned diagnostics before native error state is cleared.

**Source proof covers the expanded fork delta.** The proof now pins stock and corrected `src/error.rs` alongside the two established binding files. Archive mode applies the three-file patch; source mode admits the exact member bytes plus unchanged sys/C composition. The path dependency remains explicitly non-immutable, but tuple checks, full-tree admission, file hashes, nested gitlink verification, lock comparison, and runtime vendored-version assertion close substitution paths at this gate.

No mutation, process, network, credential, lifetime, handle-sharing, or public G0 signature boundary changed.

## 3. Risks and next action

Native execution remains macOS arm64 only. Other required platforms, clean remote-only reconstruction, publication, consumer integration, and production activation remain explicit gates. G0 still provides no multi-read snapshot against external repository mutation.

The next action is to accept the corrected local G0 candidate once the independent Code and Surface re-verdicts also pass. Later operation APIs and production activation remain separately reviewed work.
