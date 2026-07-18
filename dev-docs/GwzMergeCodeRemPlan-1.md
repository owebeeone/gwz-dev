# GWZ Merge M0 Code Review Remediation Plan 1

Date: 2026-07-18  
Input review: `dev-docs/GwzMergeCode-ReviewF5.md`

## 1. Assessment

Remediation is warranted, but the review does not justify redesigning M0.
There are no P0 or P1 findings. The M0 start/dry-run architecture, checked Git
mutation, partial-lock model, and driver surfaces remain sound.

The work should be completed before M1 begins because it closes one material
diagnostic regression, removes an avoidable preflight-to-execution failure,
and freezes the machine-output contract that M1 will extend.

This plan does **not** add durable merge records, status, coordinated continue
or abort, the open-operation gate, root participation, or rollback. Those
remain M1/M2 work.

## 2. Finding dispositions

| Finding | Disposition | Remediation |
| --- | --- | --- |
| P2-1 memberless preflight errors | Fix | Preserve backend error codes while adding member id/path context and a member-prefixed message to every per-member preflight backend failure. |
| P3-2 lock advancement after halt | Keep and specify | Retain advancement for verified clean outcomes, including outcomes before an unexpected halt. Document and test it. |
| P3-3 M0 commit-message divergence | Specify phase boundary | Keep the legacy M0 message. State that quoting and GWZ merge/operation trailers begin with the durable M1 record. |
| P3-4 no real-repository halt test | Fix | Add deterministic real-Git execution/failure injection and assert stop order, response, and partial lock state. |
| P3-5 one-sided parity fixture | Fix | Make one canonical fixture and require both Rust and Python serializers to match it. |
| P3-6 omitted future protocol fields | Fix now | Serialize the complete existing `MergeResponse` shape in both drivers, including non-empty synthetic coverage. |
| P3-7 unrelated-history behavior | Keep and specify | Retain porcelain-faithful rejection; pin both first-class merge and `pull --sync merge`, and document the compatibility change. |
| P3-8 late identity rejection | Fix | Reject values that libgit2 cannot represent during request validation, before any member mutation. |
| P3-9 global-option diagnostics | Fix diagnostics/docs | Return a field-specific core error for each unrelated policy field and explain generated global-option visibility. |
| P3-10 module-wide dead-code allow | Narrow | Scope allowances only to frozen, not-yet-consumed lifecycle seams and remove the module-wide exemption. |

## 3. Contract decisions to freeze first

The lead records these decisions in `GwzMergeDesign.md` before parallel code
work begins:

1. **Halted M0 batches:** the lock records every verified clean outcome even
   when a later participant fails unexpectedly. This is more truthful than
   retaining a known-stale lock and is the intended M0 behavior. M1 will
   deliberately replace it with the documented baseline-lock freeze.
2. **M0 merge messages:** M0 retains `Merge <source> into <branch>` for legacy
   compatibility. The quoted form and `GWZ-Merge-ID`/`GWZ-Operation-ID`
   trailers begin when M1 can persist the merge id and recorded message before
   mutation.
3. **Unrelated histories:** both first-class merge and pull-merge reject them.
   GWZ will not add an implicit equivalent of Git's
   `--allow-unrelated-histories`.
4. **Machine output:** JSON and JSONL expose the complete existing protocol
   response shape now. Fields reserved for later phases are emitted with their
   current empty/null values and are populated without another key-set change.
5. **Preflight error context:** `ModelError` may carry optional member id/path
   context. This is an internal additive change; it does not change the Taut
   schema because `GwzError` already has those fields.

Update the M0 sections of the design, `gwz-cli/docs/commands/merge.md`,
`gwz-cli/docs/MachineOutput.md`, and `gwz-cli/docs/Releases.md` as applicable.

## 4. Work packages

After section 3 is frozen, R1, R2, and R3 may run in parallel. Each package is
independently testable and has non-overlapping primary ownership.

### R1 — Core preflight correctness and diagnostics

Owner: core/model agent. Suggested budget: at most 300 handwritten changed
lines.

Primary files:

- `gwz-core/src/model/mod.rs`
- `gwz-core/src/protocol/convert.rs`
- `gwz-core/src/workspace_ops/merge/plan.rs`
- focused model, protocol-conversion, and merge-planning tests

Tasks:

1. Extend `ModelError` with optional member id/path context and a small builder
   or constructor that keeps existing call sites source-compatible.
2. Map that context into the existing `GwzError.member_id`, `member_path`, and
   member `target_kind` fields.
3. In `preflight_member`, attach member context to every fallible backend call,
   not only `merge_state` and `merge_analysis`. Preserve the original
   `ErrorCode`; prefix the human message with the member id/path without
   exposing backend-only formatting as a stable API.
4. Extend `GitObjectIdentity::validate` to reject every value that
   `git2::Signature::new` cannot represent. Characterize the exact invalid set
   with tests first; at minimum cover angle brackets and embedded NUL/control
   separators. Do not wait until a true-merge participant executes.
5. Prove `OperationRequest::context` rejects an invalid supplied author or
   committer before workspace locking, planning, or Git mutation.

Required tests:

- a two-member merge where only the second member lacks the source ref;
- a two-member merge where the second member has foreign integration state;
- both errors include structured member id/path and a member-identifying
  message while preserving the backend code;
- invalid author and invalid committer table tests;
- a mixed fast-forward/true-merge request with invalid identity rejects before
  the fast-forward member moves.

### R2 — Halt semantics and Git compatibility tests

Owner: core integration/Git agent. Suggested budget: at most 350 handwritten
changed lines.

Primary files:

- `gwz-core/src/workspace_ops/merge/start.rs` tests
- `gwz-core/src/workspace_ops/tests/g23.rs` or a new focused test module
- `gwz-core/src/git/tests/g12.rs`
- pull workflow tests

Tasks:

1. Add a test-only execution wrapper that delegates to the real
   `Git2Backend` but deterministically moves a selected later participant's
   branch between frozen planning and execution inspection. Keep the injection
   seam test-only; do not add a production callback or weaken the checked
   backend contract.
2. Exercise a real two- or three-member batch in which:
   - an earlier member merges cleanly;
   - the injected later member fails with `MergeDrift`;
   - remaining members are unattempted;
   - the response is `Halted`/`Failed` with accurate rows and unknown live
     commits for failed/unattempted participants;
   - the lock advances for the verified earlier outcome only.
3. Add real-repository unrelated-history tests for:
   - `gwz merge <source>`;
   - the checked backend primitive;
   - the existing `pull --sync merge` path.
4. Assert all unrelated-history failures leave the target ref, index,
   worktree, native integration state, and lock unchanged.

This package pins existing policy; it must not add rollback or an
allow-unrelated-histories option.

### R3 — Complete and shared machine-output parity

Owner: driver/parity agent. Suggested budget: at most 350 handwritten changed
lines, excluding the canonical fixture.

Primary files:

- `gwz-cli/src/merge_render.rs` and Rust renderer tests
- `gwz-py/src/gwz/cli_render.py` and Python renderer tests
- one canonical merge-response fixture consumed by both repositories
- `gwz-cli/docs/MachineOutput.md`

Tasks:

1. Move or replace the Python-only fixture with one canonical fixture at a
   workspace location both test suites can consume. Do not maintain two copied
   fixtures.
2. Construct the same synthetic `MergeResponse` in Rust and Python and compare
   each parsed JSON value with the canonical fixture. Canonical byte equality
   may also be used after key ordering is normalized; semantic JSON equality
   is the required contract.
3. Emit every existing merge response field in both drivers:
   - all participant counts, including `continued`, `aborted`, and
     `rolled_back`;
   - `operation_drift`;
   - `preservation`;
   - `publication_step`.
4. Implement real serializers for these fields rather than hard-coding empty
   values. The synthetic fixture must contain non-empty drift/preservation and
   non-zero reserved counts so future M1 data cannot be silently dropped.
5. Pin both JSON and JSONL paths and preserve the established enum labels,
   error shape, action, and exit codes.

The output change is additive but user-visible. Record it in `Releases.md` and
state that strict consumers must tolerate additive keys while the protocol is
pre-1.0.

### R4 — Validation diagnostics and lint cleanup

Owner: lead integration. Suggested budget: at most 180 handwritten changed
lines.

Primary files:

- `gwz-core/src/workspace_ops/merge/validate.rs`
- `gwz-core/src/workspace_ops/merge/mod.rs`
- focused validation tests
- merge command/reference documentation notes

Tasks:

1. Replace the combined unrelated-policy rejection with field-specific errors
   for `sync`, `remote`, `concurrency/jobs`, progress interval, and per-host
   connection limit. Keep core as the validation authority.
2. Explain on the merge command page that generated global options are
   syntactically available to every command but merge rejects the listed
   unrelated policies. Do not hand-edit generated `CLI.md`; change generator
   inputs or linked command documentation and regenerate normally.
3. Remove `#![allow(dead_code)]` from the live merge module. Apply narrowly
   scoped allowances only to the frozen M1+ store/model/response seams that
   remain intentionally unused, each with a phase-removal comment.
4. Run clippy after removing the blanket allowance and remove genuinely dead
   M0 code rather than suppressing it.

Required tests assert that every rejected global policy names the offending
field and retains the expected typed error code.

## 5. Integration order

1. Lead completes and commits the section 3 design decisions.
2. Run R1, R2, and R3 in parallel from that shared baseline.
3. Review each package independently before integration; findings must include
   file/line evidence and a reproducer.
4. Integrate R1 first because it establishes error context and early identity
   rejection used by later tests.
5. Integrate R2 and R3, resolving only fixture/test wiring conflicts.
6. Complete R4 against the integrated tree.
7. Run the full gate in section 6.
8. Use `gwz capture`, explicit `gwz add`, and a coordinated `gwz commit` for
   the member heads and root lock. Do not push unless separately requested.

## 6. Verification gate

From the workspace root:

```text
cargo fmt --all -- --check
cargo test -p gwz-core -p gwz
cargo clippy --workspace --all-targets -- -D warnings

cd gwz-core
../gwz-py/.venv/bin/python protocol/regen.py --check

cd ../gwz-cli
../gwz-py/.venv/bin/python scripts/generate_cli_reference.py --check
uvx --with 'mkdocs-material>=9.5,<10' mkdocs build --strict

cd ../gwz-py
.venv/bin/python -m maturin develop
.venv/bin/python run_tests.py

cd ..
bazel build //gwz-core/... //gwz-cli/...
git diff --check
```

The repository currently has Bazel build targets but no Bazel test targets;
`bazel test` reporting "No test targets were found" is not a successful test
gate and should be recorded separately.

## 7. Exit criteria

Remediation is complete only when:

- every per-member preflight failure identifies the member in both human text
  and structured machine error fields;
- invalid request identities cannot cause a mid-batch halt;
- the real-Git halt test proves stop order and the chosen partial-lock rule;
- unrelated-history rejection is pinned for merge and pull and documented;
- Rust and Python consume the same canonical parity fixture;
- both serializers emit the complete current `MergeResponse` field set and
  pass non-empty future-field tests;
- every rejected global policy names its field;
- M0/M1 message and lock transitions are explicitly documented;
- the module-wide dead-code allowance is gone;
- all commands in section 6 pass with generated artifacts clean;
- the two unrelated HN draft files remain untouched.

