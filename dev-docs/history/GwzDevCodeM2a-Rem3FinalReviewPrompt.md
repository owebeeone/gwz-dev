# GWZ M1/M2a Third-Remediation Final Re-review Prompt

You are an independent code-review agent. Review the final M1/M2a
third-remediation working tree as safety-critical local Git tooling. Do not
modify implementation, tests, generated files, or existing documentation.
Your only permitted workspace write is the report requested below.

Use your reviewer designation as `XX` and write:

```text
/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDevCodeM2a-ReviewXX-6.md
```

Do not read another `GwzDevCodeM2a-Review*-6.md` report before completing your
own analysis.

## Snapshot and inputs

The remediation is intentionally uncommitted on these submitted heads:

```text
gwz-dev:  5080b441a8003cc39f38c21ce0354bdae37e9549
gwz-core: e152c205bb2e46e401306914e53d7b004f9a3108
gwz-cli:  c335d972228e9e6b21658cc14938f756af6533bb
gwz-py:   fae628acd91936d386101ee9266e6916ea93d9b7
```

Read:

- `AGENTS.md` and `AGENTS_GWZ.md`;
- `dev-docs/GwzDevCodeM2a-RemPlan-3.md`;
- `dev-docs/GwzDevCodeM2a-ReviewF5-5.md`;
- the earlier `*-4.md` third-remediation reviews;
- `gwz-core/dev-docs/GWZRequirements.md`;
- `gwz-core/dev-docs/GwzMergeDesign.md`;
- `gwz-core/dev-docs/GwzMergePlan.md`; and
- current merge and machine-output user documentation.

Review the complete uncommitted root/core/Rust CLI/Python diffs and relevant
callers. Do not trust the remediation claims or green tests without checking
the invariants.

## Primary final recovery gate

Independently verify both clean true-merge and conflict-resolution durable
actions:

- preparation freezes the exact result/tree/signatures before execution;
- preparation, status, and checked execution require the exact durable target
  branch, before/source commits, native state, index/worktree, tree object and
  contents, author, committer, and timezone;
- status and checked execution use the same read-only definitions;
- checked execution validates under the durable target-ref lock before any
  object or ref write and commits from the existing recorded tree;
- missing or malformed local fixture evidence, changed staged content, or a
  switch to another branch at the same commit is fail-closed, leaves the
  durable pending action intact, and makes no post-boundary Git/record/event or
  accepted-metadata mutation;
- existing durable actions never call preparation again; and
- valid recovered execution consumes the frozen tree and signatures despite
  later request identity, repository configuration, or clock changes.

Use the existing focused tests and an independent temporary local-repository
check. Keep temporary artifacts small and outside the workspace.

## Whole-remediation gate

Also verify:

1. The original tree-A/tree-B replacement and pre-emitter lifecycle defects
   remain closed.
2. Every accepted merge invocation has one outer lifecycle pair, and native
   completion appears only after the final event.
3. The four open states × dry-run/real matrix covers direct core, native, Rust
   CLI, Python client, and Python CLI, including explicit root from unrelated
   working directories and byte-level non-mutation.
4. Native synchronous/submitted backend and store failures retain typed errors,
   exact completion, waiters, replay, and cancellation behavior.
5. Rust/Python JSONL parity and generated artifacts remain current.
6. Requirements, design, plan, and implementation agree on frozen signatures,
   durable pending actions, exact target branch, and lifecycle ownership.

M2b finalization/publication, the M2b-C test-delay cleanup, M2c root
participation, M3 preservation/history/GC, and later strategies remain
intentionally deferred unless this snapshot violates their handoff.

## Verification and decision

Run section 7 of `dev-docs/GwzDevCodeM2a-RemPlan-3.md`. The lead baseline is
**666 Rust pass executions with 1 ignored** and **314 Python tests passed**.
Record blockers and explain any count difference.

Lead with:

- `PASS` only when no P0/P1/P2 defect remains; or
- `FAIL`, with the highest severity and why M2b remains blocked.

Every finding needs file/line, violated criterion, concrete scenario, impact,
recommended correction, and regression test. End with severity counts,
dispositions for all previously reproduced defects, focused/full-gate results,
residual risks, and an explicit statement whether this report satisfies one of
the two passing-review requirements and whether shared M2b integration may
resume.
