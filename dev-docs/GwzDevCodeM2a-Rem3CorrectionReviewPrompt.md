# GWZ M1/M2a Third-Remediation Correction Re-review Prompt

You are an independent code-review agent. Review the final corrected M1/M2a
third-remediation working tree as safety-critical Git tooling. Do not modify
implementation, tests, generated files, or existing documentation. Your only
permitted workspace write is the review report requested below.

Use your reviewer designation as `XX` and write the report to:

```text
/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDevCodeM2a-ReviewXX-5.md
```

Do not read another `GwzDevCodeM2a-Review*-5.md` report before completing your
own analysis.

## Snapshot and inputs

The correction is intentionally uncommitted on top of these submitted heads:

```text
gwz-dev:  5080b441a8003cc39f38c21ce0354bdae37e9549
gwz-core: e152c205bb2e46e401306914e53d7b004f9a3108
gwz-cli:  c335d972228e9e6b21658cc14938f756af6533bb
gwz-py:   fae628acd91936d386101ee9266e6916ea93d9b7
```

Read these inputs before reviewing:

- `AGENTS.md` and `AGENTS_GWZ.md`;
- `dev-docs/GwzDevCodeM2a-RemPlan-3.md`;
- `dev-docs/GwzDevCodeM2a-ReviewF5-4.md`;
- `dev-docs/GwzDevCodeM2a-Review56-4.md`;
- `gwz-core/dev-docs/GWZRequirements.md`;
- `gwz-core/dev-docs/GwzMergeDesign.md`;
- `gwz-core/dev-docs/GwzMergePlan.md`;
- `gwz-cli/docs/MachineOutput.md`; and
- `gwz-cli/docs/commands/merge.md`.

Review the complete uncommitted root, core, Rust CLI, and Python diffs and all
relevant callers. Do not trust the remediation claims or green suite without
independently checking the invariants.

## Primary correction gate

Independently reproduce and verify the `*-4.md` missing-tree/specification
finding:

- a clean not-started durable true merge validates exact branch/ref/HEAD,
  before/source objects, result class, tree OID syntax and tree existence/type,
  exact in-memory merge-index equality, author, committer, and timezone;
- status and checked execution share one read-only definition;
- validation creates no object and changes no ref, HEAD, index, worktree,
  native Git state, record, event, manifest, or accepted lock;
- checked execution validates before every object-writing primitive and
  commits from the already recorded tree rather than recreating it;
- missing tree, malformed tree OID, invalid author, invalid committer, invalid
  timezone, tree mismatch, or result-class drift is
  `PendingActionAmbiguous`, with continue and abort both ineligible;
- rejected direct execution, continue, and abort make no mutation and never
  call a preparation method; and
- the exact valid control still consumes the frozen tree and signatures
  regardless of later request identity, repository configuration, or clock.

Repeat an independent real-Git missing-tree reproduction rather than relying
only on the permanent regression.

## Whole-remediation regression gate

Also verify:

1. A persisted resolution for tree A cannot be replaced by later staged tree B
   and exact true-merge/resolution retries never re-prepare durable intent.
2. Every accepted merge invocation has exactly one outer lifecycle owner
   around context conversion, open-operation gating, validation, and dispatch;
   native completion becomes visible only after `OperationFinished`.
3. All four open states × dry-run/real are pinned through direct core, native,
   Rust CLI, Python client, and Python CLI, with public drivers using explicit
   root from an unrelated cwd and byte-level non-mutation.
4. Native backend and malformed-store failures are pinned in synchronous and
   submitted modes with original typed errors and exact lifecycle/completion.
5. Multiple waiters, iterator close/cancellation, replay, and the retained
   eight-scenario Rust/Python JSONL parity matrix remain correct.
6. Requirements/design/plan consistently freeze identity before durable intent
   and forbid retry-time re-resolution.

The M2b-A2 late-drift resting-state correction, M2b publication/finalization,
M2c root participation, M3 preservation/history/GC, new merge strategies, and
the documented `GWZ_PY_TEST_EVENT_DELAY_MS` M2b-C cleanup remain intentionally
deferred. Report them only if this snapshot violates their handoff or exposes
them prematurely.

## Verification

Run the complete gate in section 7 of
`dev-docs/GwzDevCodeM2a-RemPlan-3.md`. Record every command that does not pass,
the precise blocker, and exact Rust/Python counts. Use `gwz status` and
`gwz diff` for workspace-wide state. The corrected lead baseline is **663 Rust
tests passed with 1 ignored** and **314 Python tests passed**.

## Report format and decision

Lead with:

- `PASS` only if no P0/P1/P2 defect remains in the corrected M1/M2a contracts;
  or
- `FAIL`, naming the highest severity and why M2b remains blocked.

List findings in descending severity. Every finding must include the affected
file and line, violated criterion, concrete failure scenario, impact,
recommended correction, and regression test. Separate verified defects from
P3 proof gaps, questions, and optional improvements.

End with:

- counts by severity;
- explicit dispositions for the missing-tree/specification defect, original
  tree-A/tree-B defect, and original pre-emitter lifecycle defect;
- focused/adversarial and full-gate results;
- residual risks and deferred items; and
- an explicit statement whether this report satisfies one of the two required
  passing corrected-snapshot reviews and whether M2b-A2/M2b-C shared
  integration may resume.
