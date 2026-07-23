# GWZ M1/M2a Second-Remediation Re-review — Reviewer 56

Date: 2026-07-22

Reviewer designation: **56**

## Gate decision

**PASS. No P0, P1, or P2 defect remains in the corrected M1/M2a contracts.**

The four accepted RemPlan-2 defects are corrected in the submitted snapshot:
commit-producing intent and reconciliation are content/signature exact, every
merge start uses the effective-workspace gate, native Python completion has one
terminal owner, and Python JSONL is a real live event stream with a single final
response or structured error.

Severity counts:

- P0: 0
- P1: 0
- P2: 0
- P3: 3

The P3 findings below are documentation and test-matrix completeness gaps. They
do not expose a current unsafe mutation, deadlock, output-parity defect, or data
loss path and do not block M2b-A2/M2b-C integration.

## Reviewed snapshot and method

The prompt's statement that RemPlan-2 is an uncommitted working tree is stale.
I reviewed the current clean, pushed `main` snapshot:

| Repository | RemPlan-1 base | Reviewed head |
| --- | --- | --- |
| workspace root | `d48c9ddda697430d8af63fb83b8b52ff3053e49c` | `5080b441a8003cc39f38c21ce0354bdae37e9549` |
| `gwz-core` | `9c6e85104c997ab17940642f963f839aea3ad2bc` | `e152c205bb2e46e401306914e53d7b004f9a3108` |
| `gwz-cli` | `c36bf1bf0426a9ce43204dc0816e76297a00d491` | `c335d972228e9e6b21658cc14938f756af6533bb` |
| `gwz-py` | `7f40afb20f4fa2c4c9d73f694edadea89c7b668a` | `fae628acd91936d386101ee9266e6916ea93d9b7` |

The RemPlan-2 implementation is committed as root `77dd384`, core `13137f8`,
CLI `33870aa`, and Python `fae628a`. The later root/core/CLI bootstrap commit
adds the `AGENTS.md`/`AGENTS_GWZ.md` discovery rule and does not alter merge
runtime behavior.

I read all required review inputs, inspected every RemPlan-2 implementation and
test diff from the listed bases, followed the corrected values through their
callers outside the diff, and ran the complete section 8 gate. I did not
coordinate findings with the other reviewer and used no subagents. No
implementation, test, protocol, or documentation file was changed; this report
is the only review write.

## Findings

### P3-1 — An earlier design passage still contradicts frozen-signature recovery

**Affected file and lines:**
`gwz-core/dev-docs/GwzMergeDesign.md:545-550` says author and committer identity
come from the repository at commit time and are not merge invariants. The
corrected contract later in the same document (`:1118-1129`), in
`GWZRequirements.md` REQ-089E/H, and in the implementation freezes and verifies
the fully resolved signatures before durable intent.

**Violated criterion:** RemPlan-2 R2E task 2 requires the authoritative design
to state that completed actions include exact tree and frozen-signature
identity. The later text does, but the directly conflicting finalization text
was not removed.

**Concrete failure scenario:** an M2b contributor follows section 10 while
implementing a finalization retry and re-resolves repository identity or time
after intent persistence. That behavior would disagree with the checked
start/continue primitives and make an otherwise exact recovery candidate
ambiguous.

**Impact:** documentation-driven regression risk; the current implementation
is correct and tests reject signature drift.

**Recommended correction:** replace lines 549-550 with the frozen attribution
rule used by REQ-089E/H and section 16, explicitly covering immediate,
retried, and resolution commits.

**Regression test:** add a lightweight authoritative-doc consistency check
which asserts that the obsolete "identity come[s] from the member repository
at commit time" rule is absent and that `GwzMergeDesign.md` names frozen author
and committer signatures in the commit-intent section.

### P3-2 — The required open-start driver/state cross-product is not pinned

**Affected files and lines:**

- `gwz-core/src/workspace_ops/tests/g23.rs:422-487` covers
  `awaiting_resolution`, `finalizing`, `halted`, and `recovery_required`, but
  only through the direct core handler;
- `gwz-cli/tests/local_workflows.rs:430-444` covers Rust CLI dry-run and real
  starts only against `finalizing`, from the workspace; and
- `gwz-py/src/tests/test_native_branch_stash.py:66-110` covers the Python
  client/native path from an unrelated cwd only against
  `awaiting_resolution`; its Python CLI assertions exercise only the real
  start.

**Violated criterion:** RemPlan-2 section 7.2 requires all four open states,
dry-run and real starts, workspace and unrelated cwd/effective-root resolution,
and direct core, Rust CLI, Python client, Python CLI, and native surfaces. It
also says every focused matrix row must be named and green.

**Concrete failure scenario:** a later Rust advisory-gate or Python dispatch
change special-cases `halted` or `recovery_required`; core-only tests remain
green, while one public driver returns a plan or a different error for that
state.

**Impact:** latent parity-regression risk. Current inspection confirms both
drivers reach the authoritative core rule, and the covered state/surface
combinations pass.

**Recommended correction:** add one table-driven real-workspace test harness
which runs the complete state × dry-run/real × driver matrix, including an
explicit root from an unrelated cwd, and checks byte-for-byte zero mutation.

**Regression test:** the table above is the regression test; it should name
each state and surface in parameter ids so section 8 evidence can enumerate
every row directly.

### P3-3 — Native completion coverage omits several required failure/cancellation rows

**Affected files and lines:**
`gwz-py/src/tests/test_native_merge_operations.py:55-191` covers successful
status, validation failure in synchronous/submitted forms, duplicate ids, and
a submitted second-member preflight failure. It does not exercise a
recovery/drift rejection in both forms, backend failure, store failure after
recorder creation, prompt completion of multiple event iterators on each
failure, or subscriber cancellation. No merge-stream cancellation test exists
in the Python suite.

**Violated criterion:** RemPlan-2 section 7.3 enumerates those failure rows and
requires completion, structured error, terminal events, and waiter wake-up for
each. Required focus 4 also requires proof that subscriber cancellation cannot
cancel or corrupt the operation.

**Concrete failure scenario:** a future store-error return is added between
core event delivery and `finish_model_error`, or an event iterator cleanup path
starts owning operation cancellation. The current validation/preflight tests
would not enter those boundaries, so the operation could remain incomplete or
be cancelled without failing the focused suite.

**Impact:** missing fault-boundary evidence, not a reproduced defect. Current
code has one shared `run`/completion owner, completion and retained response are
published under one lock, all current waiters are notified, and event
subscription is observational only.

**Recommended correction:** add narrow injected native fault seams for backend
and post-recorder store failures, parameterize synchronous/submitted execution,
start multiple event/result readers for every failure row, and add an
`aclose()`/task-cancellation test that subsequently obtains the intact final
result and complete event history.

**Regression test:** the expanded section 7.3 parameterized matrix plus the
cancel-then-resubscribe/result test.

## Accepted-defect dispositions

### 1. Exact commit-producing intent and recovery — fixed

- `PendingMergeAction` now carries an additive result discriminator and an
  exact commit specification containing tree OID plus author/committer name,
  email, timestamp, and timezone.
- Clean true-merge preparation uses an in-memory merge tree; resolution
  preparation freezes the resolved index tree. Both resolve identities before
  the pending intent is written and leave refs, HEAD, on-disk index, worktree,
  and native state unchanged (apart from the explicitly allowed unattached
  tree object).
- Checked execution recomputes and compares the tree under the checked ref
  boundary, uses only the frozen signatures, validates parents/message/tree/
  signatures on the created commit, and moves the ref only afterward.
- Reconciliation requires exact branch/ref, clean repository/index/worktree,
  ordered parents, byte-exact message, tree, author, and committer. Tree or
  signature variants are `pending_action_ambiguous`, with continue and abort
  both false.
- Old commit-producing records lacking the new specification are ambiguous;
  legacy up-to-date and fast-forward intents remain conservatively
  classifiable. Nested unknown fields round-trip.

Focused evidence includes
`prepared_clean_merge_freezes_exact_content_without_observable_mutation`,
`prepared_conflict_prediction_does_not_enter_native_merge_state`,
`checked_resolution_binds_parents_and_rejects_unsafe_index_states`,
`different_tree_or_signature_commit_is_ambiguous_and_status_is_read_only`,
`resolution_candidate_with_different_tree_is_never_adopted_or_rollback_eligible`,
and `old_commit_producing_pending_record_is_ambiguous_but_old_fast_forward_is_classifiable`.

### 2. One authoritative merge-start gate — fixed

Every `MergeOp::Start` now enters `guarded_workspace_root` with the actual
dry-run flag. Real starts retain the workspace mutator guard; dry-runs run the
same effective-root open-operation decision without taking that lock. The
public event and non-event core entries share this path, so Rust, native
Python, Python client/CLI, and direct public core calls cannot bypass it.
Core tests cover all four required lifecycle states and assert no workspace
byte changes; real Rust/Python surface tests confirm the authoritative rule is
reached. P3-2 records the remaining cross-product coverage debt.

### 3. Exactly-once native Python completion — fixed

Synchronous and submitted merge dispatch both use
`native/src/dispatch/merge.rs::run`, one exclusive recorder, and the same
terminal completion methods. A model error is converted directly to the
generated structured error without string flattening. `OperationFinished` is
delivered by core before `run` completes the recorder; the successful typed
`MergeResponse` and terminal result are then published atomically under the
same lock. Duplicate completion is rejected without overwriting the first
result, and the condition variable wakes every result/event reader. P3-3 is
missing fault/cancellation coverage rather than an observed lifecycle defect.

### 4. Actual Python JSONL streaming and parity — fixed

Native submission creates the recorder before returning the operation id and
buffers events from sequence zero, so subscription cannot race past early
events. `NativeCoreBridge` advances a sequence cursor, and Python CLI flushes
each event immediately, then emits exactly one final response or structured
error. `--json` remains synchronous and human output remains on the old path.
Dropping an async subscriber only stops its polling task; it sends no
cancellation into the native operation. The actual Rust/Python CLI test covers
all eight required dry-run, clean, conflict, status, continue, recovery
rejection, abort, and preflight-failure scenarios with equal normalized
semantic streams. The subprocess live-delivery test reads `OperationStarted`
while the operation is deliberately still incomplete.

## Verification results

All commands were run against the reviewed submitted heads. Exit status was 0
for every command.

| Command | Result |
| --- | --- |
| `gwz status --json` | clean root and all members; reviewed heads match the table above |
| `cargo fmt --all -- --check` | pass |
| `cargo test --workspace --all-targets --no-fail-fast` | pass — **649 passed, 0 failed, 1 ignored** across all reported Rust test binaries |
| `cargo clippy --workspace --all-targets --all-features -- -D warnings` | pass |
| `cargo fmt --manifest-path gwz-py/Cargo.toml -- --check` | pass |
| `cargo clippy --manifest-path gwz-py/Cargo.toml --all-targets --all-features -- -D warnings` | pass |
| `gwz-py/.venv/bin/python -m maturin develop --manifest-path gwz-py/Cargo.toml` | pass; current native extension rebuilt and installed editable |
| `cd gwz-py && .venv/bin/python scripts/regen_protocol.py --check` | pass; `regen_protocol: OK` |
| `cd gwz-py && .venv/bin/python -m pytest -q` | pass — **286 passed** |
| `cd gwz-cli && python3 scripts/generate_cli_reference.py --check` | pass |
| `cd gwz-cli && uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict --site-dir /tmp/gwz-m2a-review-56-3-docs` | pass; upstream MkDocs 2.0 advisory only |
| `bazel build //gwz-core:gwz_core //gwz-cli:gwz` | pass; both targets built |
| root/core/CLI/Python `git diff --check` | pass for all four repositories |
| final `gwz status --porcelain` | empty; verification left the submitted workspace clean |

The focused exact-intent, gate, native-completion, live-delivery, and all eight
cross-driver JSONL tests all ran inside these green suites.

## Residual risks and deferred items

- P3-1 should be corrected before the stale wording is reused by M2b work.
- P3-2 and P3-3 should be filled while their test harnesses are already active;
  they are proof gaps, not known implementation failures.
- `GWZ_PY_TEST_EVENT_DELAY_MS` is a production-visible test hook in
  `native/src/operations.rs:409-417`. It is useful for the subprocess liveness
  proof but should eventually be replaced by an injected or explicitly
  test-build seam so an inherited environment variable cannot delay normal
  merge starts.
- The prior macOS/arm64, power-loss filesystem, and adversarial raw-Git race
  limitations remain. Linux/Windows CI and the later finalization fault matrix
  are still required.
- M2b final publication, M2c explicit-root participation, and M3 preservation,
  history, retention, and GC remain deliberately deferred and were not treated
  as defects in this M1/M2a review.
- The late-drift resting-state correction remains an explicit M2b-A2
  acceptance item, as required by the plan.

## Independent-review criterion

**Satisfied.** This independent re-review found no P0/P1/P2 defect in the
corrected M1/M2a contracts. M2b-A2 finalization, M2b-C driver/event completion,
and shared M2b integration may resume. The three P3 findings should be carried
as non-blocking documentation/test debt and must not be allowed to weaken the
frozen recovery, gate, completion, or streaming interfaces.
