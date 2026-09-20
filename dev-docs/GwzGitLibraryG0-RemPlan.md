# G0 merged remediation and bounded source amendment

Date: 2026-09-21. Status: **scope accepted; corrected implementation awaiting re-verdict**.
Initial tuple: root `3a6d28f77591acec00334a64877b6411d29f8231`, core
`039eb1f315339a61d3d4f1cf5db5b79dce74c4ce`, library
`fcf5296095d230f46c8409a8d6d47b44f4ad918b`, Rust fork
`4c1caabbce7d56426c763dd94114052302b23e4c`, C
`b172e3d187a4b6866fd9f696f40a1b8e7f56d348`.
Code: two P2, one P3. State/Surface: GO. All reports filed verbatim.

| Finding | Disposition | Required closure |
| --- | --- | --- |
| Code P2-1: graft/shallow rewrites stored parent record | Accept; obtain ordered parent and tree IDs from the validated raw ODB header, retain native signature parsing. Do not alter graft/shallow files or native traversal. | SHA1/SHA256 fixtures with shallow boundary and explicit graft: native lookup is rewritten, G0 returns stored parents/tree; metadata unchanged. |
| Code P2-2: native class lost by raw_class | Accept; small git2-rs bug fix: raw_class returns its stored integer in the existing raw alias type, without enum whitelisting. raw_set_git_error uses raw_class instead of the lossy safe enum, preserving callback round-trip too. Safe class() retains its existing unknown-class mapping. | Known/new/arbitrary integer unit rows and native malformed info/grafts regression assert GIT_ERROR_GRAFTS = 36 from pinned C errors.h, not the old lossy getter as oracle. Both archive/source qualification must pass with the new patch. |
| Code P3-1: extra public error traits | Remove Clone/Eq/PartialEq derives from Error and NativeDiagnostic; keep the accepted diagnostics surface. | Negative compile checks for removed Clone/PartialEq traits; original reviewer verifies shape. |

This preserves all G0 public signatures and semantics. It amends only the G0
design's fixed Rust source selection and package file ownership to permit the
small binding correction needed to satisfy its already accepted diagnostic
contract. No new sys enums, native C changes, Cargo versions, production
dependency switch, source publication or operation API. No parsing of formatted
error strings or shadow FFI layout to evade the binding deficiency.

The old N2 tuple remains historical accepted evidence. G0's new fork commit
must descend from it and change only `git2-rs/src/error.rs`. The integrator owns
that file, source pins, proof patch, manifests/locks and commits. No separate
agent may change shared source selection. Existing C gitlink and the two
accepted per-remote binding files must remain byte-identical.

Additional paths owned by this remediation:

- `git2-rs/src/error.rs`: raw class accessor/round-trip plus contained unit tests;
  no new public signature; production growth ceiling 15 lines (expected net deletion).
- Core `tests/transport_native/{binding-pin.json,patches/git2-per-remote.patch,
  tests/local_fetch.rs,README.md}`: add error.rs baseline/patched hashes, extend
  the reproducible patch, add the malformed-grafts test and describe revised
  qualification. Reuse the unchanged proof runner/admission mechanism; no new
  files or harness. Test growth ceiling 100 lines, no core production growth.
- Existing G0 `src/{commit.rs,error.rs,lib.rs}`, `tests/native_baseline.rs` and
  README; library remains within its 600-line / 12-file ceiling. Core/root
  design status, G0 evidence and checkpoint record the superseding source tuple.

First add regressions and prove red on the original library/fork. Then correct
the library and narrow binding, regenerate patch/hashes from exact source,
commit the fork, run source proof before/after library checks and archive proof
plus Python admission guards. Check locks, versions, features and native-source
identity unchanged. Record actual results. Code re-verdict closes all its
findings; State reviews raw-header extraction and new source composition;
Surface checks revised qualification instructions. Implementation may proceed
only after the bounded scope amendment is reviewed. One remediation round;
no completed closure or new architectural root cause claimed by the owner.

Scope at root `409d86d8e1c61831dfd52d6dd57d71b386c920b4` received
[Code GO](GwzGitLibraryG0-RemPlan-ReviewCode.md) and
[State GO](GwzGitLibraryG0-RemPlan-ReviewState.md) before implementation.
Both reports are filed verbatim. Runtime findings remain open until retained
reviewers verify the corrected tuple.
