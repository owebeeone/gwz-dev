# GWZ v0.11.0 release checkpoint

Status: RC EVIDENCE COMPLETE AT `8008bf6`, 2026-08-25 — awaiting the
operator's release-script runs (which push members and mint the tag). This document is the
release record per the v0.10.5 convention; the tag line below is
filled when the operator publishes.

## The release candidate

| Repo | Commit | State |
| --- | --- | --- |
| gwz-core | `8008bf6` | pushed to main; version `0.11.0`; taut-proto pin 0.9.1 |
| gwz-cli | `6b7e75a` | local main — OPERATOR PUSHES |
| gwz-py | `6e1d52f` | local main — OPERATOR PUSHES; taut-proto pin 0.9.1 + regenerated IR |
| taut | `5cd26a1` | snapped at the taut v0.9.1 release tag (operator-directed; was `f008419`, an ancestor — the old pin was behind the release) |

Tag: ⟨OPERATOR: `v0.11.0` at gwz-core `8008bf6`; the GitHub release
body is `GwzReleaseNotes-v0.11.0.md`; member release-branch commits
per the R3 proposal (version + path→git-tag re-point) at the tag.⟩

The train (all exact-ref pushed, each behind its filed review):
`1a31851` A1 activation + `8e40fa8` corrective → `a6ef094` R1 →
`bed072a` R2 → `07e1ac1` R3 → `a6ce8a8` precision micro-train → `8008bf6` taut-proto
0.9.1 bump (operator-directed, tested-first: wire byte-identical;
currency harness 29/29 under 0.9.1).
gwz-cli: `cf0d16d` docs truth fix → `6b7e75a` R4 recovery guide.
Reviews: `GwzM5-8A1ReleaseR{1,2}-ReviewState.md`,
`GwzM5-8A1ReleaseR3-Review.md`, `GwzM5-8A1ReleaseR4-Review.md` — all
GO (R3/R4 after their condition rounds, applied and re-executed).

## Gate ledger at the RC

| Gate | Status | Evidence |
| --- | --- | --- |
| G1 foreign-filter refusal (A′) | **DISCHARGED** | landed `9939b02`, first filed review GO; condition tests + doctrine sentinel at `a6ef094`; forbidden ordering cleared |
| G2 CRLF birth pins (B) | **DISCHARGED incl. G2-c1** | landed `9939b02`; gap package `bed072a`; RC evidence below |
| G3 retained-reader truth | **DISCHARGED** | the v0.11.0 generation + migration clause + the register hole closed (`07e1ac1`); tuples 24; L2-04 86 |
| G4 release-lane gates | **DISCHARGED** | batteries Linux-leg + CRLF count-pin Windows-leg wired topology-audited (`07e1ac1`); both un-wireable checkers recorded with the R2-F cure |
| G5 native evidence at RC | **DISCHARGED** | the four RC-head runs below |
| G6 release-truth docs/notes | **DISCHARGED** | docs gate `ok (12 sources, 155 assertions)` with the recovery guide pinned (`a6ce8a8`); notes final |

**Plan correction (G5, recorded here):** the plan's G5 row named "the
T-5 candidate-pair regeneration discharged on its §11.2 carrier" —
over-eager. v0.11.0 regenerates no retained-reader evidence, so T-5
remains OPEN on its R2-F native-evidence carrier exactly as the
settled-tuple row states. Not a gate for this release.

## RC evidence (2026-08-25, at `a6ce8a8`, verbatim)

All four RC-head runs GREEN: **Platform matrix** `32802563914`,
**Windows matrix** `32802562170`, **Checked-artifact boundary**
`32802495545` (with the wired batteries), **Retained merge readers**
`32802495544` — re-established at the taut-bump head `8008bf6`:
Windows `32846951548`, Platform `32846954270`, boundary
`32846765849`, retained-readers `32846765913` — ALL GREEN at
`8008bf6` (retained-readers on rerun: its windows harness leg first
failed `test_timeout_kills_descendant_process`, a process-tree kill
race in a file the bump's five-file diff provably does not touch;
the rerun of the identical tree passed in 606s — environment flake,
recorded, hardening chip filed). The a6ce8a8 evidence stands for
the unchanged tree content; the re-run covers the workflow-pin
change. Push CI green at every train commit
(`a6ef094`/`bed072a`/`07e1ac1` boundary+readers all success).

**G2-c1, discharged with both required tails** (windows run
`32802562170`, `crlf-sentinel` job, under the hostile un-pinned
host):

```
Pinned lane:      test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1545 filtered out; finished in 0.27s
Un-pinned:        test git::tests::g01::crlf_sentinel_unpinned_worktree_materializes_blob_exact - should panic ... ok
                  test result: ok. 1 passed; 0 failed; ...
```

**First-dispatch confirmations, both held:** the Windows count pin at
exactly `6 passed` (the 5-unconditional + 1-`cfg(windows)` derivation
confirmed on the runner), and the stash proof's first Windows
execution —
`git::tests::g11::stash_round_trip_on_a_born_repo_is_a_filter_no_op
... ok` in the full Windows suite — which CLOSES the C-iii evidence
caveat ("proven non-vacuously off-Windows"): the born-repo stash
closure is now proven ON Windows as well. The `#[should_panic]`
sentinel also rode the full Windows suite green, as designed.

## What v0.11.0 carries, named

Shipped: the v1 writer + `--no-ff` (A1); envelope-dispatched v0/v1
reads; the seven-shape resume migration; the A′ pre-mutation
foreign-filter refusal; the CRLF birth-time pins; the merge recovery
guide (`docs/MergeRecovery.md`); the release-lane batteries + CRLF
lane; the retained-reader generation set.

Named residuals riding, each with owner: `ACTIVE_WRITER_FLOOR = V0`
(M5c); the A′ target-side-coverage residual (register entry +
doctrine sentinel; R6 renormalize); adopted-worktree CRLF
availability (renormalize); attribute-driven smudge (permanent
fail-closed doctrine); C-2/18-UNBOUND/archive-equivalence (R2-E
archive/GC sub-package); T-5 pair (R2-F carrier); L2-05 + m4-map CI
wiring (R2-F multi-repo checkout); the eligible-row upgrade-failure
fallback test ([P3-R2-2], R2-E).

## Resume order after the tag

R2-E (the 67 re-reserved keys + the binding-obligation ledger) —
M5c — R2-F (native evidence, T-5, the multi-repo CI checkout) — the
escape implementation packages (second lane, still blocked on
operator handoff) — R6 renormalize (shares A′'s predicate; owns two
residuals above). Program state authority remains
`CurrentProgramCheckpoint.md`.
