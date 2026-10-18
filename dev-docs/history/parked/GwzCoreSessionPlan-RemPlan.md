# GWZ core session plan, revision 1 (TR1.4a) — first remediation plan

Date: 2026-09-27. Status: **remediation plan for [the first verdict](GwzCoreSessionPlan-Verdict.md); applied as revision 2 of `GwzCoreSessionPlan.md` by its drafter.**

Every finding of the two reports gets exactly one disposition. The findings are [Consistency](GwzCoreSessionPlan-ReviewConsistency.md) C-P3-1 to C-P3-5, and [Safety](GwzCoreSessionPlan-ReviewSafety.md) S-P2-1, S-P2-2 and S-P3-1 to S-P3-5.
- All findings are accepted.
- Two corrections differ in form from the reviewer's remedy, with the reason given: S-P2-1's legacy context, and S-P3-5's marker.
- Every correction is to the plan's text. None moves a phase or changes TR1.4a's and TR1.4b's ownership.
- The revision also takes the reviewers' residual notes where they are cheap (§3).

## 1. Blocking findings

| ID | Disposition | Where | Closure test |
| --- | --- | --- | --- |
| S-P2-1 | (i) CS3.1's transition rule covers every driver entry that reaches a handler before its driver moves, in both builds: gwz-cli's `execute_with_backend` (`src/globalargs/dispatch.rs`), gwz-py's backend scope (`native/src/shims.rs`), and gwz-core's public handler API. Each builds a **per-call legacy context**: the live environment read at the entry, a fresh host context whose member lock manager serves that call only, and the process-wide timeout. (ii) The legacy environment read is listed `debt` where it lives: gwz-py's allowlist, and gwz-core's for the crate default. CS4.8 and CS6.5 are its removers. gwz-cli's read is named in CS3.1, and CS6.1 removes it. (iii) CS3.3 is marked **Ordinary path**. (iv) CS3.3's four spawn entries and CS3.4's new `git` spawn stay `debt` until CS6.5 removes the legacy context. (v) CS3.9 states that a direct caller's handler takes its per-call context's manager. **Form differs:** the reviewer named a process-wide legacy host context. A per-call context keeps today's lock behaviour exactly, since no handler takes a member lock today. **Refined after drafting:** the per-call part covers the environment and the member locks only. The setup-job cap, cleanup permits and `gh` slots stay process-wide on the legacy path, as today's statics are, and stay listed `debt` until the legacy path goes (CS4.8, CS6.5). Otherwise each legacy call, and each gwz-py `TransportSession` in the candidate build, would get budgets of its own, which weakens today's process-wide bounds. | CS3.1, CS3.3, CS3.4, CS3.9, §5.4 | gwz-cli's and gwz-py's ordinary-build suites pass unchanged through CS3.3, CS3.4 and CS3.9, with a configured credential helper and `GIT_CONFIG_GLOBAL` set. After CS3.4, `check_process_globals.py` over gwz-core and gwz-py lists the legacy environment reads and the spawn entries as `debt`, each naming its remover. After CS6.5 and CS4.8 it shows no `debt` on the session path. |
| S-P2-2 | CS1.1's files name `tests/transport_consumer/candidate/candidate_generated.rs`. CS1.1 adds the §13 additions to it by hand, beside the production file, and adds a lexical test: the two files' `GwzErrorCode`, `TransportCapabilitiesResponse` and §13 messages must match, except for the recorded candidate-only delta (`ResponseMeta.transport_message`). §2.4 gains a rule that holds until activation: every schema step keeps both generated files in step, and builds the candidate before it merges. The Phase 1 exit adds "the candidate build compiles". | CS1.1, §2.4, Phase 1 exit | After CS1.1 and after CS1.2, `cargo test` under `RUSTFLAGS='--cfg gwz_transport_candidate'` on a `prepare.py` tree compiles, and the protocol corpus vectors pass under both cfgs. The lexical test fails when the two files disagree on any member, field or code number of the §13 additions. |

## 2. Nonblocking findings

| ID | Disposition | Where | Closure test |
| --- | --- | --- | --- |
| S-P3-1 | CS3.1, CS3.5, CS3.6 and CS6.5 gain "and TR3.1 (G2)" as a dependency, and so does §4's lane C text. CS3.1's characterization row runs on the candidate build. | CS3.1, CS3.5, CS3.6, CS6.5, §4 | §4 and each step's dependency line agree with G2. CS3.1's review package shows a candidate-build run. |
| S-P3-2 | The §5.3 release-pins row names the four artifacts that implement today's git-tag pin: gwz-py's `scripts/release.py`, the pin assertions in `.github/workflows/publish.yml`, `RELEASE.md`, and `test_native_module_reports_compiled_core_provenance`. It states that the `=1.1.0` registry form replaces the git-tag form in all four. | §5.3 | The row's test, plus: `publish.yml` refuses a git-tag pin and accepts `=1.1.0`, and `release.py`'s unit test writes the registry form. |
| S-P3-3, C-P3-4 | Blind convergence. R8 and the Phase 4 exit's Surface list gain the change: from CS2.10, a bounded wait on the bridge's `diff_log_read` and `log_output_read` waits instead of probing. CS2.10 updates the docstrings in `diff_logs.rs` and `log_service.rs`. | CS2.10, R8, Phase 4 exit | The Phase 4 Surface package lists the `timeout_ms` semantics. A gwz-py test shows that a one-second bounded read on the native bridge returns after about one second. |
| S-P3-4, C-P3-5 | Blind convergence. C1 lists the three adopted native routes beside libgit2's native remotes: the off switch (TR1.5), non-gh HTTPS (TR1.6) and unsignable agent keys (TR2.8). For each, the contract's next revision must state what `cancellation` means, and C1 points to those designs' Surface reviews. D7 names the off switch as a way to exercise §5.8's rows on a transport build. | §6.3 C1, §6.2 D7 | C1 and D7 name the routes and the switch. |
| S-P3-5 | CS1.4's host-context members, CS2.12's close report, CS6.1 and CS6.4's notice each gain "TR1.4b extends this (reuse design §7, §13)": the host context gains the endpoint registry and a bounded `shutdown`, and the close report sums binding reports. §2.2 states that such a step may merge on TR1.4a's GO, and that TR1.4b's extension is reviewed as an additive re-freeze (L1-09). **Form differs:** the reviewer offered a wait for TR1.4b as the alternative. Waiting would hold Phase 1's host context, and so most of Phase 2, behind TR1.3 and TR1.4b. The reviewer rated the finding as having no release or data hazard, so a planned additive re-freeze is the cheaper safe path. | CS1.4, CS2.12, CS6.1, CS6.4, §2.2 | Every step whose contract section the reuse design's §13 "On GO" list amends carries the marker, and §2.2's rule names the re-freeze. |
| C-P3-1 | CS4.7 and CS4.8 gain `**Ordinary path:**` markers, with R8's reasons. §3.0's "TR1.4b marks" sentence is kept only for steps TR1.4b adds. Adding the markers does not revise the steps' content, which stays TR1.4b's. | CS4.7, CS4.8, §3.0 | Every step whose text or R8 names a change for an existing gwz, gwz-py or crate user carries the marker, and §3.0's list equals the set of marked steps. |
| C-P3-2 | The Phase 3 exit's reason for a dual review becomes CS3.4 alone, which is sufficient. The `cancellation` field's route to users belongs to CS3.10, which is TR1.4b's. | Phase 3 exit | The exit no longer claims the field is user-visible at Phase 3. |
| C-P3-3 | §1.3 cites the contract's §1 for "any change to gwz-transport", and notes that §13's envelope clause stands, as the accepted reuse design's §13 says. The transport plan's TR1.2 (line 182) has the same slip. It is recorded for the lane owner to correct in the plan's next status edit, when the reuse design's "On GO" list is applied. | §1.3 | The cited section is the one the reuse design amends. |

## 3. Residual notes taken

- **§4's shared-file list** is completed with the files it omits (Consistency): `session_host/context.rs` (CS1.4, CS3.8), `legacy.rs` (CS3.1, CS6.5), `transport_host/request.rs` (CS3.7, CS6.5), gwz-py's `native/src/lib.rs` (CS4.2, CS4.8) and `client.py` (CS4.5, CS4.7), and gwz-cli's `dispatch.rs` (CS6.3, CS6.4) and `lib.rs` (CS6.1, CS6.4). It also notes that the gwz-core allowlist JSON is ordered by the ratchet rule.
- **R1** says the reuse design is accepted as a design (its Verdict-1), and that TR1.2 closes when its "On GO" list is applied (Consistency).
- **CS1.8's title** drops "only if revision 4 has not applied RemPlan-2 §1" (Consistency).
- **G0's open obligation** names its actor: the lane owner flips the paired GWZDesign and GWZRequirements paragraphs in a status-only edit on TR1.4a's GO, before CS1.1 (Safety).
- **CS1.8's first CI run** has an observer: the Phase 1 exit's "gwz-core CI is green" includes the boundary job's first run on GitHub (Safety).

Not taken, as outside the object: `EVIDENCE.md`'s Windows path (Safety residual), which the lane owner records separately; and the other residuals, which the reports carry for TR1.4b.

## 4. Re-review

- The same two reviewers give a focused re-verdict on revision 2's SHA-256. Each gets this plan and the diff from revision 1.
- Acceptance needs GO from both axes on revision 2.
- The revision changes no phase, gate or step ownership, so no fresh round is needed.
- If a reviewer classifies a finding in the re-verdict as a new architectural root cause, the lane stops for the operator.
