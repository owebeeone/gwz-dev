# Cross-lane cleanup — remediation plan after round 1

Date: 2026-09-29. This is the lane owner's merge of round 1, which reviewed the object at manifest `43fcf974…`:
- [Consistency](GwzCrossLaneCleanup-ReviewConsistency.md): GO, with five P3s.
- [Safety](GwzCrossLaneCleanup-ReviewSafety.md): NO-GO, with one P2 and three P3s.

The merged verdict is NO-GO, on S-1. Round 2 is the last round under the two-round cap.

## Dispositions

| ID | Disposition |
|---|---|
| S-1 (P2) | Accepted, correction (a). gwz-core's `gwz-git2` dependency gains `vendored-libgit2`, so every build links the fork's corrected libgit2 and never picks up a system library through pkg-config. A test asserts that the linked libgit2 is the vendored one. `prepare.py` stops adding the feature itself. |
| S-2 (P3) | Accepted as a recorded coupling. gwz-cli's job runs the checker from gwz-core's `main` unpinned, as gwz-py's already does. The checkpoint entry and gwz-cli's `test_process_globals.py` say so, and gwz-core reaches GitHub before or with gwz-cli. |
| S-3 (P3) | Accepted. The checker fails closed: a production `include!` it can't read is an error that no allowlist entry waives, and it reads `cfg_attr(<predicate>, path = "…")` as a path attribute. The reviewer's fixtures A and B join the `TestsDirectory` tests. |
| S-4 (P3) | Accepted. The regenerator runs rustfmt under gwz-core's pinned toolchain whatever the working directory, the rustfmt pin moves to that toolchain's rustfmt, and the outputs are regenerated with it. The regeneration test passes from gwz-core and from the workspace root. |
| C-P3-1 | Accepted. The checkpoint's plan-text bullet also names the design's §13 sentence and the plan's C8 record. It also names line 109's taut and owner pins, stale since `0b7fdf19`. |
| C-P3-2 | Accepted. The checkpoint records `taut/dev-docs/TautOptions.md:475` as the taut lane's follow-up. |
| C-P3-3 | Accepted: the three wordings. |
| C-P3-4 | Accepted. The checkpoint, and a dated note under gwz-core `GwzCratesIoPlan.md` D4, say that `src/protocol/candidate_generated.rs` ships in the package, inert without the cfg. |
| C-P3-5 | Accepted. `GwzLibgit2Gaps.md`'s Phase 2 note says the stub run in the no-fallback plan's §4 met S2.3's `git`-absent condition. |
| Safety residual | `GwzNoFallbackPlan.md` §4 says that no test path reached the fallback, not that nothing did. |

## Added by the operator after round 1

On 2026-09-29 the operator folded the candidate suites' pre-existing failures into this cleanup:
- **The candidate lib:**
  - the corpus byte parity;
  - three driver and fault tests that expect older gwz-transport `PeerFailed` codes;
  - the HTTPS tests that fail only in parallel, and the test that hangs in parallel.
- **`tests/protocol.rs`:**
  - three taut lookups in a prepared tree;
  - the `MergeRequest` byte pin under the candidate's `RequestMeta`.
- **The consumer crate:** `admission`, `messages` and `pool_host` against gwz-transport's `setup_cause` (`14f0d09`).

Each is fixed at its cause. None of these fixes serializes tests, adds a retry or skips a test to go green. gwz-transport stays read-only.

## Round 2

Each reviewer reviews the delta from round 1's object to round 2's:
- that their own findings are closed;
- the fold-in changes, on their axis;
- anything the corrections broke.

Any P0 to P2 left after round 2 goes to the operator.
