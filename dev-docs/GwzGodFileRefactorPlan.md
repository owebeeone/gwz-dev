# GWZ God-File Refactor Plan

Date: 2026-07-24

Status: **implemented; complete exit gate passed; awaiting the coordinated
`gwz` commit required before M2c implementation**

## 1. Objective

Reduce the current high-risk source-file concentration before explicit
workspace-root merge participation adds root planning, recovery, finalization,
and rollback behavior.

This is a structural refactor only. It must not change protocol values, public
behavior, error codes, event ordering, merge state transitions, Git semantics,
CLI syntax, rendered output, or test expectations.

The mechanical Rust work uses the installed `rust-split 0.1.1` from
`/Users/owebeeone/limbo/rust-split`. Hand cut-and-paste is not an acceptable
substitute where `rust-split` can perform the move.

## 2. Baseline

The refactor starts from the clean post-M2b workspace:

- root: `f9856ba8209ba66f10dc39739eeff1c8228ebb49`;
- `gwz-core`: `9eec97f2f53034d4e8177b8e60868711e6da7c28`;
- `gwz-cli`: `394f4de6aa32d94b6e850b04126c36ab675fe8d0`;
- `gwz-py`: `020a6fa3004f83640c2edca3ab8170d55a696add`;
- 691 Rust test executions: 690 passed and one ignored;
- 315 Python/native tests; and
- formatting, strict Clippy, protocol freshness, cross-driver parity, and diff
  hygiene passing.

M2c feature work must not be mixed into this refactor.

## 3. Audit findings

### Confirmed god files

| File | Evidence | Primary concern |
| --- | --- | --- |
| `gwz-core/src/git/gitbackend.rs` | 3,898 lines; 65 trait methods, 62 implementation methods, and 56 trailing helpers | Contract, Git2 implementation, merge recovery, scoped evidence, stash, transport, status, and diff behavior share one file. |
| `gwz-core/src/workspace_ops/merge/status.rs` | 1,089 production lines plus 904 test lines | Pending-action reconciliation, repository observation, participant classification, and operation drift are coupled in one module. |
| `gwz-cli/src/clirequest.rs` | 1,060 production lines | Argument structures, shared CLI errors, selection/policy lowering, and every command-family request conversion share one module. |
| `gwz-cli/src/append_branch_summary.rs` | 1,007 production lines and 48 functions | Human, porcelain, JSON, and JSONL rendering for unrelated response families share one module. |
| `gwz-cli/src/globalargs.rs` | 913 production lines | Parser schema, invocation construction, execution dispatch, open-merge gating, rendering, and exit codes share one module. |
| `gwz-core/src/workspace_ops/tests/g23.rs` | 2,175 lines | Start, continue, finalization, drift, abort, crash recovery, and driver-gate scenarios share one test module. |

### Immediate pressure points

- `merge/abort.rs` has 787 production lines and already combines root evidence
  rollback with participant rollback. M2c would add root-participant rollback.
- `merge/start.rs` has 749 production lines but reaches 2,654 lines with its
  embedded tests. M2c would add root planning and root-last execution.
- `gwz-py/src/gwz/cli_render.py` has 50 rendering functions across multiple
  response families. M2c would add root lifecycle rendering.
- `gwz-py/src/gwz/client.py` is 979 lines and 51 methods, but remains a thin
  public API facade. It is monitored rather than split in this wave.

`merge/finalize.rs`, `merge/publication.rs`, and `merge/mod.rs` remain
acceptably cohesive. Generated sources are excluded from the audit.

## 4. `rust-split` trial results

Every trial wrote only to `/tmp`. No generated layout was applied to GWZ.

For `gitbackend.rs`, `status.rs`, and `abort.rs`, concatenating the ordered
`explode` chunks reproduced the original source byte-for-byte.

| Input | Trial output with `--module --max-loc 500` |
| --- | --- |
| `merge/status.rs` | 11 Rust files; maximum 499 LOC |
| `merge/abort.rs` | 14 Rust files; maximum 492 LOC |
| `merge/start.rs` | 16 Rust files; maximum 497 LOC |
| `workspace_ops/tests/g23.rs` | 14 Rust files; maximum 499 LOC |
| `clirequest.rs` | 8 Rust files; maximum 493 LOC |
| `append_branch_summary.rs` | 8 Rust files; maximum 496 LOC |
| `globalargs.rs` | 7 Rust files; maximum 499 LOC |

The automatic names and clusters are not the final architecture. Generated
names such as `clirequest_.rs`, and clusters named after a single large
function, must be replaced through deliberate `manifest.toml` grouping before
application.

### Git backend limitation

`rust-split` correctly reports two indivisible oversized top-level items in
`gitbackend.rs`:

- the 549-line `GitBackend` trait; and
- the 1,741-line `impl GitBackend for Git2Backend`.

The tool can extract surrounding types and helpers, but it cannot split inside
those items. This plan does not redesign the public backend into capability
traits. Instead, the existing trait remains stable and the Git2 methods become
thin delegators to semantic implementation modules. A capability-trait redesign
would require its own design and compatibility review.

## 5. Required `rust-split` workflow

For each Rust source selected for splitting:

1. Run `rust-split explode <file> --out <temporary-directory>`.
2. Concatenate `chunk-*.rs` in numeric order and use `cmp` to prove byte
   identity with the original.
3. Review `manifest.toml` and edit grouping there. Do not manually re-cluster
   source text.
4. Run `rust-split split <file> --module --max-loc 500 --out
   <temporary-directory>`.
5. Review module names, imports, re-exports, visibility changes, attributes,
   comments, and test-module gates.
6. Apply the reviewed output deliberately with patches. Never run the first
   split in place and never copy an unreviewed generated tree into the repo.
7. Let the compiler identify required import and visibility corrections.
   Do not predict and broaden visibility pre-emptively.
8. Preserve a pure-move diff until the split compiles and the focused tests
   pass. Do not run a formatter during this phase.
9. Run formatting as a separately reviewable follow-up after the pure-move
   result is proven.

Registration macros, an oversized leaf item, an unexpectedly widened public
surface, or a generated cluster without a clear semantic name stops that work
package for lead review.

## 6. Work packages

### G0 — Lead-owned interface and ownership checkpoint

The lead:

- records the baseline above;
- freezes all public and crate-visible behavioral contracts;
- chooses final semantic module names;
- creates the module roots and re-export boundaries;
- assigns disjoint ownership before parallel work begins; and
- confirms no M2c code is present in the refactor diff.

No specialist independently edits a shared module root or chooses a new public
visibility.

### G1 — Core merge lifecycle split

Split these plain file modules using `rust-split`:

- `merge/status.rs` into orchestration, pending-action reconciliation, live
  observation, classification, drift helpers, and external test modules;
- `merge/abort.rs` into orchestration, preflight, evidence rollback,
  participant rollback/reconciliation, and external test modules;
- `merge/start.rs` into orchestration, durable execution, prepared-action
  execution, response conversion, and external test modules; and
- `workspace_ops/tests/g23.rs` into start, continue, finalization,
  abort/recovery, drift, and open-operation-gate scenario modules.

The existing `merge/root.rs` name remains reserved for M2c. This refactor must
not pre-implement root participation.

### G2 — Git backend decomposition

Use `rust-split` for the lossless top-level inventory and for movable types and
helpers. Hand-finish only the indivisible trait/implementation limitation.

Target semantic areas:

- backend contract and result types;
- ordinary repository/ref/status operations;
- merge analysis and prepared execution;
- merge recovery and resolution validation;
- scoped GWZ evidence commit/verification/rollback;
- stash and preservation primitives;
- fetch, push, credentials, and remote callbacks; and
- diff and comparison helpers.

`GitBackend` method signatures and the `crate::git` re-export surface remain
unchanged. The single Git2 trait implementation may remain in one contract file
only as thin forwarding methods. No implementation body may remain as a second
hidden god file.

The normal 500-LOC ceiling applies to implementation modules. A documented
exception of at most 650 LOC is allowed only for the stable backend contract
file containing the existing 549-line trait. Exceeding that requires a separate
capability-trait design review.

### G3 — Rust CLI decomposition

Use `rust-split`, with semantic manifest grouping, to separate:

- common invocation, error, selection, policy, and request metadata;
- request lowering by command family;
- parser declarations from execution dispatch and open-operation gating;
- human status, branch, stash, listing, and merge rendering; and
- machine envelope, event, result, and error serialization.

Existing CLI syntax, Clap help, exit codes, JSON keys, JSONL ordering, and
fixture shapes remain exact.

### G4 — Python renderer alignment

`rust-split` does not apply to Python. Split `cli_render.py` manually by the
same semantic response boundaries used by the Rust CLI:

- common/error rendering;
- status and file-change rendering;
- merge lifecycle rendering;
- branch/stash/listing rendering; and
- machine JSON conversion.

Keep `Client` as the stable public facade in this wave. Extract it only if the
split reveals non-facade behavior or it exceeds 1,100 lines.

Rust and Python merge rendering must continue to share canonical fixtures and
cross-driver tests.

### G5 — Integration and independent review

The lead integrates only after every owned subtree compiles independently.
Integration fixes imports, visibility, and module wiring; it does not redesign
behavior.

An independent reviewer checks:

- no logic was dropped, duplicated, or reordered;
- comments and attributes remained attached to their items;
- `#[cfg(test)]` gates still protect extracted test modules;
- public and crate-visible surfaces did not expand accidentally;
- backend defaults and mock implementations still compile;
- human and machine driver parity remains exact; and
- M2c feature behavior did not enter the refactor.

## 7. Parallel execution

After G0 freezes module roots and ownership:

- G1 status/start work and G1 abort/test work may run in parallel only when
  their destination subtrees do not overlap;
- G2 is independently owned because backend re-exports are high fan-out;
- G3 and G4 may run in parallel after the core contract is confirmed unchanged;
  and
- the lead alone edits shared `mod.rs`, crate re-exports, milestone documents,
  and integration fixtures.

Agents must use `rust-split` themselves for their owned Rust files and include
the explode byte-identity result in their hand-off.

## 8. Exit gate

The refactor is complete only when:

1. every Rust input has recorded byte-identical `explode` evidence;
2. no non-generated implementation or test source produced by this wave
   exceeds 500 LOC, except the documented backend contract exception;
3. no unreviewed generated filename or catch-all module remains;
4. `cargo fmt --all -- --check` passes after the separate formatting pass;
5. `cargo clippy --workspace --all-targets -- -D warnings` passes;
6. `cargo test --workspace` passes with at least the 691 baseline executions
   and the same intentional ignore disposition;
7. `gwz-py/.venv/bin/python run_tests.py` passes with at least the 315 baseline
   tests and a freshly built native extension;
8. generated protocol and CLI-reference freshness checks pass;
9. Rust/Python cross-driver parity and canonical fixtures pass;
10. `gwz forall @root gwz-core gwz-cli gwz-py -- git diff --check` passes;
11. `gwz status --json` reports only the intended structural files; and
12. independent review reports no P0/P1/P2/P3 defect or accidental behavioral
    change.

After this gate passes and the structural work is committed through installed
`gwz`, M2c-A may begin.

## 9. Completion evidence

Implementation and verification completed on 2026-07-24. The diff contains
only the planned structural decomposition and milestone-document updates; no
M2c root-participation behavior is present.

### Lossless Rust split evidence

For every Rust input, ordered `rust-split explode` chunks compared
byte-for-byte equal to the committed source:

| Input | Chunks | Original and reconstructed SHA-256 |
| --- | ---: | --- |
| `git/gitbackend.rs` | 121 | `c5db09bdac0e49f1cfd4a853f380056271ff6dcc6beacfa6eaf973bead947713` |
| `merge/status.rs` | 36 | `688ac2f6dbce4e0efa070dd041af54c69fb7a4b46e950959acc119572c8f36a7` |
| `merge/abort.rs` | 31 | `e18b503d85295b30a027642893bcc07d28798581363434b40d05391bbe07e8c9` |
| `merge/start.rs` | 44 | `9508dc99fb273f8a388340ae47585dc75b4a87c2df198b32bc1505affec4e932` |
| `workspace_ops/tests/g23.rs` | 61 | `4d458c3354c9d70f542093a558e72cce6e8dab3849397cfaa92de03e07b71b5a` |
| `clirequest.rs` | 45 | `fac3856641d2bfb13ab1e102540ccff29a063f0c24f0af7ec726fea51aff953a` |
| `append_branch_summary.rs` | 51 | `766efd848b88bb31630385454e1fc2bbce882e730ebad22aab48eac4f70377d2` |
| `globalargs.rs` | 26 | `951e4f607b4b299efec478fbb304aa39343d09521f89faa5bd3df216c1e29ea9` |

### Final module bounds

| Area | Largest ordinary produced file | LOC |
| --- | --- | ---: |
| Git backend | `merge_prepared.rs` | 397 |
| Merge status | `tests/pending.rs` | 480 |
| Merge abort | `tests/support.rs` | 324 |
| Merge start | `tests/prepared_recovery.rs` | 474 |
| G23 scenarios | `fixtures.rs` | 471 |
| CLI request lowering | `repo.rs` | 330 |
| CLI rendering | `response_listing.rs` | 230 |
| CLI parser/dispatch | `parser.rs` | 470 |
| Python rendering | `status.py` | 278 |

The sole exception is `gitbackend/contract.rs` at 557 LOC, within the
documented 650-LOC ceiling for the unchanged stable trait. No generated chunk
name or catch-all module remains. The Python facade remains 89 LOC and retains
its four stable exports.

### Verification record

- `cargo fmt --all -- --check`: passed after the separate formatting pass.
- `cargo clippy --workspace --all-targets -- -D warnings`: passed.
- `cargo test --workspace`: 690 passed and the same one intentional test was
  ignored, for 691 test executions.
- `gwz-py/.venv/bin/python run_tests.py`: 315 passed after rebuilding the
  native extension.
- Generated protocol freshness, generated CLI-reference freshness, canonical
  fixture coverage, and Rust/Python cross-driver parity passed in those suites.
- `gwz forall @root gwz-core gwz-cli gwz-py -- git diff --check`: passed.
- `gwz status --json` reported only the intended structural source files and
  the two milestone documents.
- Independent review reported no P0/P1/P2/P3 finding. It verified all 62 Git2
  delegations, the unchanged public and crate-visible backend surfaces,
  preserved split-source test inventories and `#[cfg(test)]` gates, all 50
  Python renderer function bodies, and the absence of M2c behavior.

The structural changes remain unstaged and uncommitted. M2c-A becomes
actionable after they are committed through the installed `gwz`.
