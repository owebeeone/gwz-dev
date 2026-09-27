# GWZ core session contract — remediation plan 2 (revision 4)

Date: 2026-09-26. Status: **remediation plan; applied as revision 4 on 2026-09-27, which [Verdict-4](GwzCoreSessionDesign-Verdict-4.md) accepted**.

Input: [Verdict-3](GwzCoreSessionDesign-Verdict-3.md), which merged [ReviewConsistency-3](GwzCoreSessionDesign-ReviewConsistency-3.md) (GO) and [ReviewSafety-3](GwzCoreSessionDesign-ReviewSafety-3.md) (NO-GO) on revision 3: root `5a5d6cf`, gwz-core `e8509c32`, gwz-py `851c2e2`, gwz-transport `a7a36ae`.

This plan maps the one blocking root, B11, and the ten new nonblocking findings to one patch, revision 4. Both axes numbered their new findings from P3-28, so every ID below carries its axis name. No finding in any round has been architectural. This is the fourth review pass on the object, confined to non-architectural corrections, which [GwzProcessOptimization §4.1](GwzProcessOptimization.md) permits.

## 1. B11 — the gwz-transport check runs in no CI

Findings: Safety P2-9, converging with Consistency P3-31. Safety pre-committed to GO on a revision that resolves P2-9 as specified. Revision 4 applies all three parts of its correction and keeps the operator's dependency direction: gwz-core is the consumer, so gwz-core's CI checks out gwz-transport, never the reverse.

1. **Fail closed locally.** `scripts/run_tests.py` finds the gwz-transport checkout from `GWZ_TRANSPORT_CHECKOUT`, or else the sibling beside gwz-core.
   - With neither, the run fails and names both ways.
   - `--skip-transport-globals` skips the check and prints `SKIPPED GATE` with the reason. Only CI jobs that have no gwz-transport checkout pass it.
2. **A pinned CI run on gwz-core's side.**
   - `process_globals_allowlist_gwz_transport.json` gains `reconciled_commit`: the gwz-transport commit its entries were last reconciled against. That commit must already be on gwz-transport's GitHub `main`.
   - gwz-core's boundary job (`checked-artifact-boundary.yml`) checks out `owebeeone/gwz-transport` at exactly that commit, beside gwz-core, and runs the check over it. It cannot skip it.
   - The jobs that run `run_tests.py` on a single checkout (`release.yml` twice, `platform-matrix.yml`, `windows-matrix.yml`) pass `--skip-transport-globals`.
   - **Bump rule:** a gwz-core commit that changes the gwz-transport allowlist sets `reconciled_commit` in the same commit, to a gwz-transport commit that is already pushed. Upstream goes first.
   - **Initial value:** `46e65a9a888f…`, gwz-transport's GitHub `main` on 2026-09-26. The checker passes against it with today's allowlist: 25 files, one `permanent` item, nothing new. This was verified by running the checker over a `git archive` export of that commit. The local gwz-transport is four commits ahead of it, all unpushed, and passes too.
3. **Text.** O9, §5.7 and §15.8, and the paired GWZRequirements and GWZDesign paragraphs, state that:
   - gwz-core's boundary CI job enforces the gwz-transport check at the recorded commit;
   - `run_tests.py` enforces it locally and fails closed;
   - gwz-transport's own CI carries no O9 gate;
   - the bump rule above applies.

**Closure tests:**
- A `run_tests.py` unit test beside `test_run_tests_filesystem_mode.py`:
  - with no sibling and no `GWZ_TRANSPORT_CHECKOUT`, the run exits non-zero and names both ways;
  - the skip flag exits zero and prints `SKIPPED GATE`;
  - `GWZ_TRANSPORT_CHECKOUT` is honoured.
- A unit test asserts that `reconciled_commit` is a full 40-character SHA and that the boundary job's gwz-transport checkout takes its ref from it.
- The checker's existing `--repo` tests cover detection in a foreign repository.

## 2. Nonblocking findings

At the operator's direction after round 2, revision 3 applied every carried P3 to the contract instead of the implementation plan. This plan proposes the same for the ten new ones. Doing so is the operator's choice (§4).

| Finding | Correction in revision 4 | Closure test |
| --- | --- | --- |
| Consistency P3-28 | §15.1: "… and its reply is dropped, except a result or response reply, which the bridge keeps (§10)." | §15.12's closed-loop result test; §15.1 for a cancelled unary call only. |
| Consistency P3-29 | O9, §5.6, §16 and GWZRequirements scope the libgit2 own-reads exception as §5.8 and GWZDesign do: "in ordinary builds, and for the remotes libgit2 handles natively in transport builds". §15.8's `HOME` test names remotes gwz-transport handles. | §15.8 on an `ssh://` or `https://` remote, plus an assertion for an `http://` remote that records the exception. |
| Consistency P3-30, P3-32, P3-33 and Safety P3-28 | One rewrite of §5.4's make-room bullet: "the oldest log whose producer sealed or closed it at least the fixed read wait (30 seconds) earlier, and that has no reader stream open, is released first; with none, the open is refused with `transport_session_full`". Then: "A later read of a log this session has released, in any way, gets `operation_expired`." §15.7's refusal test names logs none of which is releasable. | 64 closed, unread logs; the next open releases one. A log sealed within the last second is kept, and its first read succeeds. A read after each release way gets `operation_expired`. |
| Safety P3-29 | §5.8 states the mechanism for core's own `git credential fill`. The spawn has `env_clear()` plus the snapshot without `GIT_ASKPASS` and `SSH_ASKPASS`, plus `GIT_TERMINAL_PROMPT=0` and `-c credential.interactive=false` (older gits ignore the key). It is killed on drop, bounded like the `gh` helper, and killed when the token is cancelled or the gate revoked. Its output is secret-bearing under §5.6: only `username` and `password` are read, other lines are tolerated and never logged, and `approve` and `reject` are not called, as libgit2's lookup does not call them today. | With `GIT_ASKPASS` naming a recorder and no helper configured, an HTTPS fetch fails with an authentication error and the recorder never runs. A helper that sleeps is killed when the operation is cancelled. |
| Safety P3-30 | §10: a kept result leaves the view when the operation is released, or when `operation.release` reports `operation_expired`. Another call reporting `operation_expired` leaves it. The size bound is unchanged. | Cancelled result wait; the host evicts the record; `cancel_operation` reports `operation_expired`; `operation_result` still returns the result from the view. |
| Safety P3-31 | The checker flags `CredentialHelper::new` as the same `process` occurrence, `Cred::credential_helper`. §5.7 says both spellings count. | A fault-injected `CredentialHelper::new(url).execute()` is flagged as `process` `Cred::credential_helper`. |

## 3. Recorded, not patched

These residual risks from Verdict-3 go to the implementation plan with the P3s' regression tests. They are not text defects of the contract.
- The registry's check-and-record as one atomic step, and live records surviving a close as detached records.
- Fetches not recorded in the registry.
- The `fork` inheritance of the per-process host context.
- Non-contiguous `call_id`s.
- The Windows WTF-8 capture test.
- The parked-waiter completion mechanism for today's `DiffLog`.
- Verdict-2's residuals.

## 4. Operator decisions

1. **Apply this plan.** Revision 4 would change the contract, GWZDesign, GWZRequirements and four gwz-core workflows, plus `run_tests.py`, the checker, the gwz-transport allowlist and their tests.
2. **The ten P3 corrections.** Apply them in revision 4 (recommended), or carry them into the implementation plan and apply only B11.
3. **The initial `reconciled_commit`, `46e65a9…`.** It is the only pushed commit, so the pin stays behind local gwz-transport until gwz-transport is pushed.

## 5. Re-verdict

The same two reviewers continue, with this plan and the revision 4 diff:
- Safety re-verdicts B11 and its four P3s.
- Consistency checks its five P3s, the make-room rewrite, and the consistency of all text revision 4 touches.

Reports are filed as `-ReviewConsistency-4.md` and `-ReviewSafety-4.md` and merged into `-Verdict-4.md`. An architectural root cause in that pass stops the lane.
