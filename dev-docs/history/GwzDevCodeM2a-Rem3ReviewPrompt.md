# GWZ M1/M2a Third-Remediation Re-review Prompt

You are an independent code-review agent. Review the final integrated M1/M2a
third-remediation working tree as safety-critical Git tooling. Do not modify
implementation, tests, generated files, or existing documentation. Your only
permitted workspace write is the review report requested below.

Use your reviewer designation as `XX` and write the report to:

```text
/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDevCodeM2a-ReviewXX-4.md
```

Do not read another `GwzDevCodeM2a-Review*-4.md` report before completing your
own analysis.

## Snapshot and inputs

The third remediation is intentionally uncommitted on top of these submitted
heads:

```text
gwz-dev:  5080b441a8003cc39f38c21ce0354bdae37e9549
gwz-core: e152c205bb2e46e401306914e53d7b004f9a3108
gwz-cli:  c335d972228e9e6b21658cc14938f756af6533bb
gwz-py:   fae628acd91936d386101ee9266e6916ea93d9b7
```

Read these sources of truth and review inputs before judging the code:

- `AGENTS.md` and `AGENTS_GWZ.md`;
- `dev-docs/GwzDevCodeM2a-RemPlan-3.md`;
- `dev-docs/GwzDevCodeM2a-ReviewF5-3.md`;
- `dev-docs/GwzDevCodeM2a-Review56-3.md`;
- `gwz-core/dev-docs/GWZRequirements.md`;
- `gwz-core/dev-docs/GwzMergeDesign.md`;
- `gwz-core/dev-docs/GwzMergePlan.md`;
- `gwz-cli/docs/MachineOutput.md`; and
- `gwz-cli/docs/commands/merge.md`.

Review the complete uncommitted diffs in the root, `gwz-core`, `gwz-cli`, and
`gwz-py` repositories. Follow all relevant callers outside the diff. Do not
trust the remediation plan's claims or passing tests without independently
checking the underlying invariants.

The M2b-A2 late-drift resting-state correction, M2b publication/finalization,
M2c root participation, M3 preservation/history/GC, and new merge strategies
remain intentionally deferred. Report them only if this snapshot makes the
later phase unsafe, contradicts its handoff, or exposes it prematurely.

## Required review focus

### A. Exact durable pending-action recovery

Independently verify that:

- a `NotStarted` pending action is retained and decoded into its exact durable
  action rather than cleared or re-prepared;
- true-merge and conflict-resolution retries use the frozen result class,
  tree, parents, message, author, and committer;
- status and checked execution use the same exact resolved-index definition;
- changed index content or mode, unresolved entries, unstaged/untracked work,
  foreign native state, missing objects, ref/HEAD drift, or source drift is
  ambiguous and makes both continue and abort ineligible;
- ambiguity causes zero Git, operation-record, event, and accepted-metadata
  mutation;
- the real backend compares the resolved index to the recorded tree without an
  observation-time `write_tree`; and
- an execution-time race preserves the pending intent and original structured
  error without persisting a false `failed` outcome or leaving an unmatched
  member lifecycle.

Reproduce the F5 tree-A/tree-B case independently. Add a spy or fault
reproduction if needed to prove that an existing durable action never calls a
preparation method.

### B. One outer lifecycle-event owner

Independently verify that:

- the event owner is constructed non-fallibly from protocol-safe request
  fields before attribution conversion, root/recovery resolution,
  open-operation gating, validation, and dispatch;
- every accepted merge invocation emits exactly one `OperationStarted` and one
  final `OperationFinished`, including invalid attribution, all four open
  states in dry-run and real modes, backend failure, and store failure;
- success and member failure retain one monotonically increasing sequence and
  finish after all inner events;
- the original `ModelError` and structured member/target/detail fields survive
  unchanged;
- direct core, Rust CLI, native Python, Python client, and Python CLI paths all
  pass through the same owner; and
- no Rust or Python driver performs an earlier merge-specific open-operation
  rejection that bypasses that owner.

### C. Native completion and public surfaces

Independently verify that:

- synchronous and submitted merge execution publish terminal completion
  exactly once and only after `OperationFinished`;
- response and event waiters all wake, iterator closure/cancellation does not
  cancel the operation, and resubscription/replay returns buffered events;
- invalid attribution, every open state × dry/real mode, backend/store failure,
  and malformed-record failure retain typed errors and byte-level
  non-mutation;
- explicit-root and unrelated-current-directory calls behave identically
  across core, Rust CLI, native Python, Python client, and Python CLI; and
- the retained eight-scenario Rust/Python JSONL parity matrix still emits live
  ordered events followed by exactly one terminal response or error.

Treat the production-visible `GWZ_PY_TEST_EVENT_DELAY_MS` hook as the documented
M2b-C residual unless you find that it changes production semantics beyond the
already accepted test-delay behavior.

### D. Documentation and phase boundary

Verify that the requirements, design, plan, and code consistently say:

- identity is resolved before durable intent and complete signatures are
  frozen;
- `NotStarted` consumes the existing durable specification;
- every accepted invocation has the one outer lifecycle owner; and
- M2b-A2/M2b-C and shared M2b integration remain blocked until both independent
  third-remediation reviews contain no P0/P1/P2 finding.

## Verification

Run the complete gate in section 7 of
`dev-docs/GwzDevCodeM2a-RemPlan-3.md`. Record every command that did not pass
and the precise blocker. Record exact Rust and Python counts. Use `gwz status`
and `gwz diff` for workspace-wide state; the plan's direct `git diff --check`
commands are only the specified read-only whitespace checks.

The lead's recorded baseline is 662 Rust tests passed with 1 ignored and 308
Python tests passed. A different count is not automatically a finding, but
must be explained.

## Report format and gate decision

Lead with:

- `PASS` only if no P0/P1/P2 defect remains in the corrected M1/M2a contracts;
  or
- `FAIL`, naming the highest severity and why M2b remains blocked.

List findings in descending severity. Every finding must include the affected
file and line, violated requirement or plan criterion, concrete failure
scenario, impact, recommended correction, and regression test. Separate
verified defects from P3 proof gaps, questions, and optional improvements.

End with:

- counts by severity;
- a disposition for the accepted F5 P1 and P2 defects;
- focused/adversarial and full-gate results;
- residual risks and deferred items; and
- an explicit statement whether this review satisfies one of the two
  independent-review requirements and whether M2b-A2/M2b-C shared integration
  may resume.
