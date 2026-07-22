# GWZ M1/M2a Second-Remediation Re-review Prompt

You are an independent code-review agent. Review the final integrated M1/M2a
second-remediation working tree. Do not modify implementation or documentation
files. Your only permitted write is the review report requested below.

Use your reviewer designation as `XX` and write the report to:

```text
/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDevCodeM2a-ReviewXX-3.md
```

## Snapshot and inputs

The remediation is intentionally uncommitted on top of these submitted
RemPlan-1 heads:

```text
gwz-dev:  d48c9ddda697430d8af63fb83b8b52ff3053e49c
gwz-core: 9c6e85104c997ab17940642f963f839aea3ad2bc
gwz-cli:  c36bf1bf0426a9ce43204dc0816e76297a00d491
gwz-py:   7f40afb20f4fa2c4c9d73f694edadea89c7b668a
```

Read these inputs before reviewing:

- `dev-docs/GwzDevCodeM2a-RemPlan-2.md`
- `dev-docs/GwzDevCodeM2a-Review56-2.md`
- `dev-docs/GwzDevCodeM2a-ReviewF5-2.md`
- `gwz-core/dev-docs/GWZRequirements.md`
- `gwz-core/dev-docs/GwzMergeDesign.md`
- `gwz-core/dev-docs/GwzMergePlan.md`
- `gwz-cli/docs/MachineOutput.md`

Review the complete uncommitted diffs in the root, `gwz-core`, `gwz-cli`, and
`gwz-py` repositories. Do not limit review to the changed tests or trust the
remediation plan's stated results.

## Required review focus

Independently verify all four accepted defects and their surrounding safety
contracts:

1. **Exact commit-producing intent and recovery**
   - preparation freezes the exact tree, ordered parents, byte-exact message,
     and fully resolved author/committer signatures before durable intent;
   - preparation does not move refs or change HEAD, index, worktree, or native
     repository state;
   - checked execution consumes the frozen values and rejects drift before ref
     publication;
   - reconciliation adopts only the exact commit and treats tree, signature,
     parent, message, ref, index, worktree, and repository-state differences as
     ambiguous and mutation-ineligible; and
   - old commit-producing records without the new specification fail
     conservatively while legacy fast-forward/up-to-date records remain safe.

2. **One authoritative merge-start gate**
   - both dry-run and real starts use the effective request workspace;
   - awaiting-resolution, halted, recovery-required, and finalizing operations
     reject a second start with `OpenOperation`;
   - dry-run does not take the mutator lock or mutate workspace bytes; and
   - direct core, Rust CLI, native bridge, Python client, and Python CLI cannot
     bypass the same policy from an unrelated current directory.

3. **Exactly-once native Python completion**
   - synchronous and submitted merge execution share one recorder and one
     terminal-completion owner;
   - every success and failure stores its result once, wakes all waiters, and
     never overwrites the first result;
   - `OperationFinished` is delivered before completion becomes visible;
   - successful completion exposes the complete retained `MergeResponse`; and
   - failures retain original code, message, member id/path, target kind, and
     detail without string flattening or deadlock.

4. **Actual Python JSONL streaming and cross-driver parity**
   - Python subscribes before completion can race past event delivery;
   - events are flushed live, once, and in sequence order;
   - one final response or structured error follows all events;
   - `--json` remains one non-streaming response and human output is unchanged;
   - subscriber cancellation cannot cancel or corrupt the operation; and
   - actual Rust/Python CLI scenarios cover dry-run, clean success, conflict,
     status, continue, recovery rejection, abort, and top-level preflight
     failure with equivalent semantic output.

Also check for regressions in status read-only behavior, continue/abort
eligibility, rollback safety, durable write/event ordering, unknown-field
round-tripping, generated protocol compatibility, and the deferred M2b-A2
late-drift resting-state handoff.

## Verification

Run the complete gate in section 8 of
`dev-docs/GwzDevCodeM2a-RemPlan-2.md`. Independently inspect and, where useful,
extend the adversarial reproductions rather than relying only on the existing
green tests. Record every command, exit status, and exact Rust/Python test count
in the report.

## Report format and decision

Lead with a clear gate decision:

- `PASS` only if no P0/P1/P2 defect remains in the corrected M1/M2a contracts;
- otherwise `FAIL`, with the highest severity and why M2b remains blocked.

List findings in descending severity as P0, P1, P2, then P3. Every finding must
include the affected file and line, violated requirement or plan criterion, a
concrete failure scenario, impact, recommended correction, and a regression
test. Distinguish verified defects from questions or optional improvements.

End with:

- counts by severity;
- a disposition for each of the four accepted defects;
- focused/adversarial and full-gate results;
- residual risks and deferred items; and
- an explicit statement whether the independent-review criterion in section 9
  is satisfied and M2b-A2/M2b-C shared integration may resume.
