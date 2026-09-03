# GWZ M1/M2a Second-Remediation Re-review — F5-3

Date: 2026-07-22

Reviewer designation: **F5**

## Gate decision

**FAIL — M2b-A2, M2b-C, and shared M2b integration remain blocked.**

The submitted second remediation closes most of the four accepted defects, and
the complete required gate is green. It does not meet the section 9 independent
review criterion, however. One P1 recovery defect can replace a durably frozen
resolution specification with post-intent staged content and later treat that
content as GWZ-owned. One P2 event-lifecycle defect publishes terminal native
completion for pre-emitter failures without an `OperationFinished` event.

Severity counts:

- P0: 0
- P1: 1
- P2: 1
- P3: 1

## Reviewed snapshot and scope

The prompt's statement that RemPlan-2 is an uncommitted working tree is stale.
Per the review-launch correction, this report covers the current clean,
submitted `main` heads:

| Repository | Reviewed head |
| --- | --- |
| workspace root | `5080b441a8003cc39f38c21ce0354bdae37e9549` |
| `gwz-core` | `e152c205bb2e46e401306914e53d7b004f9a3108` |
| `gwz-cli` | `c335d972228e9e6b21658cc14938f756af6533bb` |
| `gwz-py` | `fae628acd91936d386101ee9266e6916ea93d9b7` |

The RemPlan-2 implementation commits are `77dd384`, `13137f8`, `33870aa`,
and `fae628a`, respectively. The later root/core/CLI commits add the unrelated
agent-bootstrap correction; they were included in the full verification and
did not change the merge findings below. `gwz status --porcelain` was empty
before review work. No implementation, test, protocol, or existing
documentation file was changed. Temporary reproductions used system temporary
directories and were removed automatically. This report is the only workspace
write.

I read the review prompt and all required inputs, inspected the RemPlan-2
production and test changes and their relevant callers, checked the current
submitted ranges against the listed RemPlan-1 base heads, ran the full section
8 gate, and independently exercised the two adverse paths described below.

## Findings

### P1 — Not-started recovery discards the frozen commit specification and adopts post-intent resolution content

Affected code:

- `gwz-core/src/workspace_ops/merge/status.rs:166-192`
- `gwz-core/src/workspace_ops/merge/continue_op.rs:135-220`
- `gwz-core/src/workspace_ops/merge/continue_op.rs:379-443`

Violated requirement and plan criteria:

- RemPlan-2 section 4.1 requires checked execution and post-crash recovery to
  consume the same expected tree and frozen signatures, and requires index,
  worktree, tree, or signature differences to be ambiguous.
- `GWZRequirements.md` REQ-089E and REQ-089H require the checked action to
  consume the frozen values and allow adoption only when tree, signatures,
  index, and worktree match durable intent.
- `GwzMergeDesign.md:1099-1107` requires the checked action to consume the
  frozen specification and makes any mismatch ambiguous.

The resolution reconciliation path proves only that the repository remains in
the expected resolved native merge with no unstaged or untracked changes. It
does not compare the current resolved index tree with
`pending_action.commit_spec.tree_oid`. It therefore returns `NotStarted` even
when the staged tree changed after the intent write. Continue then clears the
pending action, writes that weaker reconciliation, and calls
`prepare_merge_resolution_checked` again. This freezes the new index tree and
newly resolved signatures and commits them.

The same replacement occurs for a clean true merge that crashed before Git
mutation: `NotStarted` clears the original specification and continue resolves
the tree/signatures again. Repository identity, attribution, or implicit time
can therefore change the commit after the evidence that was supposed to bind
it became durable.

Concrete real-Git reproduction:

1. Start a one-member merge that conflicts.
2. Resolve and stage tree A.
3. Reconstruct the documented crash boundary by persisting a
   `resolve_conflict` pending action containing tree A and frozen signatures.
4. Before recovery, change the resolution and stage tree B.
5. Run status, then continue.

Observed output from the reproduction:

```text
tree_changed: true
status_pending: not_started
continue_eligible: true
abort_eligible: false
continued_state: continued
committed_content: resolved-B-after-intent
old_intent_rewritten: true
```

Status was read-only, but it incorrectly declared the changed index an exact
not-started point. Continue then durably replaced the original intent and
committed tree B.

Impact:

Post-intent content is silently absorbed into the coordinated merge. A later
ordinary abort treats the resulting resolution commit as GWZ-owned and resets
the participant to its recorded before commit. The post-intent content then
survives only in an unreachable commit subject to eventual pruning. This is the
same destructive ownership class that made the original different-tree
adoption a P1, occurring one step earlier than the existing candidate-commit
negative test.

Recommended correction:

Do not clear and re-prepare a commit-producing pending action merely because it
is not started. Reconstruct `GitPreparedCommit`/`GitPreparedMerge` from the
durable pending action and execute that exact specification after revalidation.
For a pending resolution, compare the current resolved index tree to the
recorded tree before returning `NotStarted`; any difference must be
`pending_action_ambiguous`, with both eligibility flags false and no record or
Git mutation. An exact control must use the original frozen author/committer
signatures even if repository configuration, request attribution, or time has
changed since the intent write.

Required regression tests:

- Persist resolution tree A, stage tree B before recovery, and assert status is
  ambiguous; rejected continue and abort must leave record, ref, index,
  worktree, and native merge state byte-for-byte unchanged.
- With the exact tree A still staged, change repository identity/time and
  assert recovery commits with the original durable signatures.
- Exercise the analogous clean true-merge `NotStarted` crash boundary and
  assert continue consumes, rather than replaces, its durable specification.

### P2 — Terminal completion is visible without `OperationFinished` for failures before the core emitter exists

Affected code:

- `gwz-core/src/workspace_ops/merge/mod.rs:95-120`
- `gwz-core/src/workspace_ops/merge/mod.rs:302-320`
- `gwz-py/native/src/dispatch/merge.rs:58-84`
- `gwz-py/native/src/operations.rs:157-168`
- `gwz-py/native/src/operations.rs:236-300`

Violated requirement and plan criteria:

- RemPlan-2 section 4.3 requires `OperationFinished` delivery before terminal
  completion becomes visible.
- RemPlan-2 section 7.3 requires one operation-start and one operation-finish
  event for every exercised success and failure.
- `GwzMergeDesign.md:1085-1097` says every invocation emits both lifecycle
  events and emits `OperationFinished` on failure.
- REQ-089H requires Python completion not to become visible before the final
  event is available.

`handle_merge_with_events` applies the merge-start gate before delegating to
the event-wrapped handler. The delegated handler also performs fallible
`OperationContext`/attribution conversion before constructing the emitter.
Either failure therefore returns with no lifecycle event. Native `run` still
calls `finish_model_error`, and `complete` immediately exposes the terminal
result to `wait_events` and result readers.

Concrete native reproduction:

I submitted `merge --status` with an invalid empty Git author name. The request
was accepted into the native operation store and completed with the original
typed `InvalidRequest`, but event subscription returned:

```text
operation_id: op_req_review_context_failure
events: []
```

An open-operation rejection of a submitted merge start follows the other
pre-emitter path at `mod.rs:99-105` and has the same ordering defect.

Impact:

Exactly-once result storage and structured error preservation work, and there
is no deadlock. However, an accepted submitted operation can become complete
without either lifecycle boundary. Python JSONL then ends with only the
structured error response for this class of failure, contrary to the event
contract. Consumers cannot rely on `OperationFinished` as the final event
before completion.

Recommended correction:

Place every failure that occurs after operation registration inside one outer
invocation event owner. Gate and request-context validation must either happen
before an operation is registered/accepted, or be wrapped so that the same
recorder receives exactly one `OperationStarted` and one `OperationFinished`
before terminal publication. Preserve the existing original `ModelError` when
that owner completes the recorder.

Required regression tests:

- Submitted and synchronous invalid-attribution requests: exact start/finish
  events, typed error, `complete=true`, and finish visible before completion.
- Submitted dry-run and real start rejected by an existing open operation:
  the same lifecycle and ordering assertions.
- An adversarial waiter blocked before failure must wake once and observe the
  finish event and terminal result together.

### P3 — The authoritative merge design still contradicts the frozen-signature contract

Affected documentation:

- `gwz-core/dev-docs/GwzMergeDesign.md:545-550`
- `gwz-core/dev-docs/GwzMergeDesign.md:1099-1107`

Violated criterion:

RemPlan-2 R2E requires the design to state that exact completed actions include
tree and frozen signature identity. Section 16 now does so, but section 10
still says author/committer identity comes from the repository at commit time
and is not a merge invariant.

Concrete failure scenario and impact:

An M2b recovery implementer following section 10 may resolve identity again
during retry, producing a different commit than the durable action intent. The
current P1 path already demonstrates why this distinction matters. The current
checked backend ordinarily uses frozen signatures, so this documentation
contradiction is not independently a runtime P2.

Recommended correction:

Replace the stale section 10 statement with the section 16 rule: resolve
request-or-repository identity before durable intent, freeze complete
signatures, and use those exact signatures for immediate and recovered
execution.

Regression check:

Add a documentation-content assertion that the obsolete “identity ... at
commit time” rule is absent and that the frozen-signature statement remains in
the authoritative design.

## Disposition of the four accepted defects

| Accepted defect | Disposition |
| --- | --- |
| Exact commit-producing intent and recovery | **Not closed.** Exact candidate commits are matched correctly, old weak commit-producing records fail conservatively, and checked primitives enforce prepared trees/signatures. P1 shows that `NotStarted` recovery discards those durable values and can replace the resolution tree/signatures. |
| One authoritative merge-start gate | **Closed for gate policy.** Dry-run and real starts use the effective request workspace; the four tested open states reject both forms, dry-run takes no lock, and direct/Rust/Python surfaces reach core policy. The P2 lifecycle omission applies to the event envelope around a gate rejection, not to the allow/block decision. |
| Exactly-once native Python completion | **Partially closed.** Success/failure retain one first result, wake waiters, preserve structured errors, and expose successful responses before completion. P2 shows terminal completion can precede the required final lifecycle event because some failures occur before an emitter exists. |
| Actual Python JSONL streaming and parity | **Closed for the implemented normal/error scenarios, with P2 inherited at the pre-emitter boundary.** Python subscribes before execution can outrun storage, flushes events live, emits one final response/error, and matches Rust across the eight required real-CLI scenarios. Cancellation does not own the operation. Pre-emitter failures still have no lifecycle records. |

## Focused and adversarial results

The focused submitted tests passed:

- `pending_conflict_and_resolved_native_state_are_distinguished`: 1 passed.
- `different_tree_or_signature_commit_is_ambiguous_and_status_is_read_only`:
  1 passed.
- `open_finalizing_blocks_dry_run_and_real_starts_from_an_explicit_root`: 1
  passed.
- Python native merge operations, CLI merge, and actual cross-driver merge
  JSONL files: **32 passed**.

The first adversarial reproduction extended the pending-resolution test one
step earlier than its existing different-commit case and reproduced P1 with
real Git, as detailed above. The second submitted-native reproduction showed a
complete structured result with zero events for a pre-context validation
failure, reproducing P2.

The existing named tests for exact candidate matching, open-state gating,
first-result preservation, response-before-completion, live JSONL delivery,
structured terminal errors, and eight-scenario cross-driver parity all passed
inside the full suites. Their green results do not cover the two reproduced
boundaries.

## Full verification gate

All required section 8 commands were run from the documented directories:

| Command | Exit | Result |
| --- | ---: | --- |
| `cargo fmt --all -- --check` | 0 | Pass |
| `cargo test --workspace --all-targets --no-fail-fast` | 0 | **649 passed, 0 failed, 1 ignored** across the reported Rust test binaries |
| `cargo clippy --workspace --all-targets --all-features -- -D warnings` | 0 | Pass |
| `cargo fmt --manifest-path gwz-py/Cargo.toml -- --check` | 0 | Pass |
| `cargo clippy --manifest-path gwz-py/Cargo.toml --all-targets --all-features -- -D warnings` | 0 | Pass |
| `gwz-py/.venv/bin/python -m maturin develop --manifest-path gwz-py/Cargo.toml` | 0 | Current native extension rebuilt and installed |
| `gwz-py: .venv/bin/python scripts/regen_protocol.py --check` | 0 | `regen_protocol: OK` |
| `gwz-py: .venv/bin/python -m pytest -q` | 0 | **286 passed** |
| `gwz-cli: python3 scripts/generate_cli_reference.py --check` | 0 | Pass |
| `gwz-cli: uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict --site-dir /tmp/gwz-m2a-review-F5-3-docs` | 0 | Strict build passed; upstream MkDocs 2.0 advisory only |
| `bazel build //gwz-core:gwz_core //gwz-cli:gwz` | 0 | 2 targets built successfully |
| Root/core/CLI/Python `git diff --check` | 0 | Pass in all four repositories |

An additional non-gate diagnostic,
`cargo test --manifest-path gwz-py/Cargo.toml --all-targets`, exited 101 while
linking the PyO3 test executable on macOS because Python C symbols were not
linked. The required native clippy/maturin/pytest path passed, and the compiled
extension reproductions ran successfully. This linker limitation did not
produce either finding, but it means the Rust-only unit tests embedded in the
extension crate are not directly executable through that cargo command in this
environment.

`gwz status --porcelain` remained empty after all required gates and
reproductions, before this report was added.

## Residual risks and deferred items

- The M2b-A2 late-drift resting-state correction remains deliberately deferred
  and must keep its acceptance item; this review does not reclassify it.
- Verification was on macOS arm64. Platform-specific power-loss, directory
  fsync, rename, and ref-lock behavior was not executed on Linux or Windows.
- Crash injection cannot prove durability on every filesystem/device, and no
  stress process continuously races raw Git against every checked primitive.
- M2b publication/finalization, M2c root participation, and M3 preservation,
  retention, and GC remain outside this snapshot.

## Independent-review criterion

The independent-review criterion in RemPlan-2 section 9 is **not satisfied**.
This review reports one P1 and one P2 defect in the corrected M1/M2a contracts.
M2b-A2 finalization, M2b-C driver/event completion, and shared M2b integration
must not resume until both defects are remediated, the focused regressions and
full gate pass, and a fresh independent review reports no P0/P1/P2 defect.
