# Cross-lane cleanup — CONSISTENCY-AXIS REVIEW (round 2)

**Review object:** bundle `…/scratchpad/review-cleanup/r2/` — `manifest.txt`, 142 entries, SHA-256 `b6a5834a10778f78203c20fab62ad0f6f5c434256ecaa75d6c4787f9522d3442`; `root.diff` (139 lines), `gwz-core.diff` (7,979), `gwz-cli.diff` (352), `gwz-py.diff` (empty); `delta.diff` (71 files, 5,662 lines, SHA-256 `c63bff886445a80a3bfd52f3e219542c0828ccbed89bb192a4d87f63f041a4ca`). Base HEADs unchanged: root `1ccb5c1`, gwz-core `2514dc19`, gwz-cli `ebbea90`, gwz-py `4f9b2bb`.

**Tuple verification:** `python3.13 …/r2/verify_manifest.py` at the start and at the end: both "142 entries checked, 0 discrepancies", manifest sha256 as above, all four HEADs at base; `shasum -a 256` of `delta.diff` matches. Bundle integrity: every manifest entry whose digest changed since round 1, or is new (71), appears in the delta, and nothing else does. Read-only: no workspace file was edited, created or deleted, no mutating git command was run; probes, logs and the candidate runs lived in `…/review-cleanup/consistency/`, the coordinator's prepared tree and its `ctarget`, and temp directories the probe harness creates itself.

**Date:** 2026-09-29 (run into 2026-09-30). **Axis:** consistency.

Filed verbatim by the lane owner.

## Verdict: NO-GO

P0: 0. P1: 0. P2: 1. P3: 4.

The one P2 is a one-line count in the process-globals checker's own unit suite, which the `SLOTS` removal left at 25 while the allowlist now holds 24; CI runs that suite. Everything else on this axis holds, including all five round-1 closures.

## Closure of round-1 findings

| ID | Status | Evidence |
|---|---|---|
| C-P3-1 | CONFIRMED | Checkpoint "Plan text for the next revision" now names the plan's §2.4 bullets, CS1.1's file list, the C8 record (line 1495), the contract's §13 sentence (`GwzCoreSessionDesign.md:534`) and line 109's stale taut/owner pins with what `0b7fdf19` moved them to. All four line numbers re-checked against the unchanged files. |
| C-P3-2 | CONFIRMED | Checkpoint "For the taut lane: `taut/dev-docs/TautOptions.md:475`". |
| C-P3-3 | CONFIRMED | Line 8 "declared as a dependency in `gwz-cli/BUILD.bazel`"; line 11 "The ordinary build doesn't use them; the candidate protocol calls `try_get_opt`" (13 calls in `candidate_generated.rs`, 0 in `generated.rs`); line 19's split 6 reads + 9 spawns + 1 cache + 1 clap = 17 matches the allowlist entry by entry. |
| C-P3-4 | CONFIRMED | Checkpoint line 16 records the packaging consequence; `GwzCratesIoPlan.md` D4 has the dated status note; `include` list unchanged (`/src/**`, not `/protocol/candidate/`). |
| C-P3-5 | CONFIRMED | `GwzLibgit2Gaps.md:198-201` says S2.3's `git`-absent condition was met by the §4 stub run, not by removing Git. |

## Findings

### C2-P2-1 — The checker's own unit suite fails: the allowlist count was not updated when `SLOTS` left

- **Location:** gwz-core `scripts/checks/test_check_process_globals.py:720-721` (`Dispositions.test_the_temp_name_counters_are_gone`: `self.assertEqual(len(core), 25)`, comment "26 -> 25 (2026-09-29): the local-import Git fallback's spawn left"); `scripts/checks/process_globals_allowlist.json` (24 entries after the delta removed `SLOTS`).
- **Root cause:** round 2 deleted the `SLOTS` entry (delta `process_globals_allowlist.json`) and added four `TestsDirectory` tests, but left the round-1 count.
- **Violated invariant:** the object's own gate is green. `python3.13 -m unittest scripts/checks/test_check_process_globals.py` → "Ran 45 tests … FAILED (failures=1): AssertionError: 24 != 25". gwz-core's CI runs exactly this suite (`.github/workflows/checked-artifact-boundary.yml:99`), so the checked-artifact-boundary workflow fails on push. `scripts/run_tests.py` does not run the unit suite (0 mentions), which is how it slipped past the local gate; the checker itself reports "24 allowlisted items (20 debt, 4 permanent); nothing new".
- **Impact:** bounded: one assertion, no product behaviour; but the round-2 checkpoint says the checker changes were tested, and CI would say otherwise.
- **Required correction:** `25` → `24`, with the comment extended ("25 -> 24: the HTTPS helper `SLOTS` semaphore left, CS6.5 pulled forward").
- **Closure test:** the unit suite runs green; the checker's count line still reads 24.

### C2-P3-1 — The checkpoint's probe count does not match what `boundary_probes.py` reports, and the harness cannot run its cargo probes

- **Location:** root `dev-docs/CurrentProgramCheckpoint.md`, the "lost protections" bullet ("14 of the gate's 15 manual defeat probes pass it. Among them are `cfg_attr` path rerouting …"; "the 15 probes become its tests"); gwz-core `scripts/manual_tests/boundary_probes.py`.
- **Root cause:** the file holds 75 probes, not 15. Run twice against today's gate (`python3.13 scripts/manual_tests/boundary_probes.py -v`, 11 and 18 minutes): 41 ok, 34 failed. Of the 34: 17 are gate-accepted defeats (`AssertionError: 0 == 0`), 14 cannot run at all because every cargo-driven probe copies gwz-core into a temp directory without `../git2-rs`, so cargo fails on the `gwz-git2` path dependency `26b30ca6` introduced ("failed to load manifest … git2-rs/Cargo.toml: No such file"), 2 fail on stale expectations (one expects the removed "protected source tree changed" wording although the gate still rejects that probe on the observer-caller set; one patches a source pattern that no longer exists), 1 expects `V1Runtime` in cargo output that never ran. The named example "`cfg_attr` path rerouting" (`test_cfg_attr_path_cannot_hide_a_concrete_observer_caller`) is one of the cargo probes: it neither passes nor fails the gate here. The round-2 change to the harness (copy `tests/`) does not touch the `../git2-rs` gap.
- **Violated invariant:** a count in the checkpoint should be reproducible from the tool it names; the operator's decision text ("the 15 probes become its tests") rests on it.
- **Impact:** text and the manual harness only; the decision (structural checks next) stands either way.
- **Required correction:** state the population and the split (75 probes; 17 accepted by the gate; 14 not runnable since `26b30ca6`; 2 stale), or name the subset that "15" counts; note the `../git2-rs` gap for the structural-checks step, which will inherit the harness.
- **Closure test:** the checkpoint's numbers match a fresh `-v` run's tally.

### C2-P3-2 — The checkpoint's account of the HTTPS test changes omits one budget test and one new test

- **Location:** root `dev-docs/CurrentProgramCheckpoint.md`, "macOS XProtect" ("no deadline or assertion changed") and "The rest of the candidate tests" ("two HTTPS budget tests have 390 ms of scheduling slack instead of 25 ms"); gwz-core `src/git/endpoint/https_budget_tests.rs:19-52` (delta 3659-3733).
- **Root cause:** a third budget test changed its deadlines and name: `delayed_helper_is_not_capped_by_one_millisecond_allocation` → `delayed_helper_is_charged_to_interaction_not_allocation`, allocation 1 → 250 ms, interaction 1 → 5 s, helper delay 20 → 500 ms, and a new `a_stale_supervisor_tick_never_expires_a_fresh_one_millisecond_allowance` keeps the 1 ms case. Neither is listed; "no deadline changed" reads as covering the HTTPS tests, and the XProtect warm-up is not what changed here. The "190 tests" result count is 191 by `--list` on the prepared tree (116 `git::endpoint::` + 75 `transport_host::`).
- **Violated invariant:** "Round 2 fixes each at its cause, with no serialized, retried or skipped test" is meant to be checkable against a complete list.
- **Impact:** record only; the changed tests pass (run by name, below) and the widened allowances keep the property under test, as the new comment explains.
- **Required correction:** add the third test and the new one to the list, scope "no deadline or assertion changed" to the warm-up, and correct 190 → 191 or say when 190 was counted.
- **Closure test:** every test the delta renames, re-times or adds under `https_*` appears in the bullet.

### C2-P3-3 — The S-1 comments state the hazard more broadly than the checkpoint's narrowing

- **Location:** gwz-core `Cargo.toml:83-87` ("links a system libgit2 in [1.9.7, 1.10.0) whenever pkg-config finds one"); `tests/native_libgit2.rs:5-8` (same); root checkpoint, "S-1" ("with `unstable-sha256`, only an experimental-sha256 system libgit2 could have been picked up").
- **Root cause:** `git2-rs/libgit2-sys/build.rs:12-60, 113` probes `libgit2-experimental` in that range, and only one whose `experimental.h` enables `GIT_EXPERIMENTAL_SHA256`, whenever `unstable-sha256` is on; the two code comments omit the qualifier the checkpoint adds.
- **Impact:** text; the fix itself is right (`vendored-libgit2` is requested; `production_links_the_forks_vendored_libgit2` passes and asserts `version.vendored()`, and the round-1 clippy log already showed the probe failing and the vendored build running).
- **Required correction:** "a system `libgit2-experimental` (SHA-256 build) in [1.9.7, 1.10.0)" in both comments.
- **Closure test:** the three texts name the same library.

### C2-P3-4 — The `SLOTS` pull-forward leaves accepted plan and design text stale, and the plan-text bullet does not cover it

- **Location:** root `dev-docs/GwzCoreSessionPlan.md:383` ("the `gh` slots (`SLOTS`) stay process-wide there … until the legacy path goes (CS4.8, CS6.5)"), `:426-427` (CS3.6: "The legacy path keeps today's process-wide `SLOTS` until it goes … `SLOTS` stays listed `debt`, for the legacy path only, and names CS6.5"), `:636` (CS6.5's files: `https_auth.rs`, "the legacy path's process-wide budgets"), `:1251` (§5.4 row), `:1536`; root `dev-docs/GwzCoreSessionDesign.md:350` (the `SLOTS` row); the checkpoint's "Plan text for the next revision".
- **Root cause:** `SLOTS` is gone now, the legacy `with_local_transport` creates its own `HelperSlots` per command, and gwz-py's candidate `TransportSession`, whose per-process budgets line 383 protects, was deleted at root `621f671`. The bullet lists the candidate paths, line 109, CS1.1 and CS6.1, but not this.
- **Impact:** text; same class as C-P3-1.
- **Required correction:** add one sub-bullet naming CS3.6, CS6.5's file list, §5.4's `SLOTS` row, line 383 and the design's line 350 as done early.
- **Closure test:** the bullet names every accepted-text line that still describes `SLOTS` as live.

## The round-2 changes on this axis

**Consistent with the code, verified:**

- **Regenerated content:** `candidate-regenerator.py --check` passes from `gwz-core/` and from the workspace root; `test_candidate_regen.py` → 7 passed from both (the three parametrized runs incl. `RUSTUP_TOOLCHAIN=stable`, plus the two stale-corpus refusals). The rustfmt pin equals `rustfmt +1.95.0 --version` (`59807616e1 2026-04-14`) and `rust-toolchain.toml` says `1.95.0`; the two new `taut-source-sha256` rows are the modules the regenerator now imports (`corpus/kit.py`, `corpus/synth.py`); rustfmt is used only for that provenance pin (taut never formats), so "outputs byte-identical" is exact: `candidate_generated.rs`, `candidate_generated.py` and the retained reader have the same digests in both manifests. `golden.json` holds 175 vectors, as the checkpoint says. `corpus_byte_parity` passes on the production lib and on the prepared candidate.
- **Docs and notes:** `GwzLibgit2Gaps.md`, `GwzNoFallbackPlan.md` §4 ("no test path … bounds the tested paths"), `GwzCratesIoPlan.md` D4, `GwzRustSplitPlan.md` (`PROTECTED_SOURCE_DIGESTS` "never compared since 107aca7a": `git show 107aca7a` removes exactly the three digest loops, the `source_tree_digest` call and the three `ENTRY_*` equality checks; `107aca7a` is 2026-09-08, "…streamline validation and release tests"). The §10.1 implementation note sits inside §10.1 (`:763`, before `### 10.2`) and every clause matches `git_turns.rs`/`ssh_pump.rs`: `Idle` only when `clients_turn()` and both buffers are empty; the four covered waits; the conservative stops (v2 via `version 2` and lengths 1–3, out-of-place lines, malformed/oversized lengths, lines over `PREFIX` = 1024, `no-done`, a bare `ACK`, stderr in the client's turn, half-close); push `Idle` only before the client's first byte; "libgit2 never asks for v2" holds for the fork's `transports/` (no `version=` request). The HTTPS `Backpressure` wait is in `https_worker.rs:914,984,992`, as recorded.
- **Citations:** `GwzCoreSessionDesign.md:65` lists the HTTPS helper slots in the host context "created by the driver and shared by the sessions it opens (§5.6)", `:350` says the slot budget moves there, and §5.6 is "The session context, the host context and operation gates": `https_auth.rs`'s `HelperSlots` doc and the checkpoint cite correctly. CS6.5 (`:635`) is the step that would have removed `SLOTS`, so "pulled forward from CS6.5" is right. `SshOpenFailure` entered at `1ac248ca`; gwz-transport `14f0d09` adds `setup_cause` and `connect_timeout_ms: 30_000`, `36ae2b1` "Align pool timeout tests with 30 second setup budget"; the consumer's `4_030_000` follows.
- **Gate comments after the table removal:** each `PROTECTED_COMPILER_MODULES` file carries `#![forbid(clippy::disallowed_methods)]` (checked all seven) and the gate enforces it (`:1144`); `ENTRY_REFERENCES` exists and is compared (`:1448-1497`); the "Digest coverage: NONE since 107aca7a", "OPEN now" and entry.rs-skip comments say what the code does; `privacy_probes.py`, `record_root_exception.rs` and `merge/validate.rs` say the same. The gate passes ("ok, 24 visible entries, 9 classified modules"); `check_cfg_boundaries.py` passes with the `protocol_corpus` row removed, and its suite is OK; the new approved edge `("lib.rs", "../protocol/candidate/corpus/rust/vectors.rs")` matches `src/lib.rs`'s `cfg_if!`.
- **Checker docstring vs code:** `INCL`/`TESTS` on unreadable includes, the refused `include` entry, `cfg_attr` routes and the default-file rule all match `_path_attributes`, `_cfg_attr_paths`, `module_routes` and `check`; the checker reports "nothing new" on gwz-core (24), gwz-cli and gwz-py.
- **`driver.rs`'s new comment** ("`send` answers `InvalidRequest` only for a route it no longer has; its other rejections are `Protocol`"): the mux's `send` returns `InvalidRequest` for a missing route or a request/session/version mismatch, `validate_transition` returns `Protocol`, `admit_limited` maps to `Protocol`; the driver guards the session id, and the pump only forwards its own request's streams at version 2, so the comment holds for the messages that can reach it.
- **Tests run:** production `corpus_byte_parity` and both `native_libgit2` tests pass; on the prepared candidate: the 12 `git_turns_tests`, 3 `ssh_pump_clock_tests`, the `HelperSlots` tests (`one_hosts_live_helpers…`, `endpoints_of_one_host_share…`, `owner_cancellation…`), both new fault tests, both new CLI driver tests, the four re-timed HTTPS tests → all pass (39 + 4 targeted, 0 failed); `tests/protocol.rs` on the candidate → 41 of 41 (was 37). `cargo fmt --check` clean in gwz-core and gwz-cli; `cargo clippy --all-targets --all-features -D warnings` clean; `cargo metadata --locked` consistent in gwz-core and at the root; `test_prepare.py` and `test_package_proof.py` → 18 passed; `test_candidate.py` → 8.
- **gwz-cli:** the only delta is the S-2 docstring, which agrees with the checkpoint's pushing order.

**Not verified:** the 6-of-6 and 10-of-10 parallel-run claims and the XProtect timings (measurements not archived); whether the mux ever hands the pump a stream whose route belongs to another request (Safety's axis).
