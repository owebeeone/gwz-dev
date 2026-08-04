# GWZ M5–M8 Refactor Change-Budget Ledger

Date: 2026-07-31

Status: **R0, R1, R2a, M5a, I1/I2, and R4a complete; R3 envelope foundation implemented and independently accepted**

This ledger implements the change-budget requirement in
`GwzM5-8Refactor.md`. Moved production lines are reported separately from
semantic additions and deletions. A package stops for scope review if it:

- exceeds any numeric ceiling below by more than 20%;
- adds a production module not listed in its ownership row;
- changes a declared wire/protocol delta; or
- crosses another package's file ownership without a lead-approved handoff.

Generated protocol and generated documentation output does not count as
handwritten LOC, but its source-schema change and generated file count remain
part of the review.

## Baseline revisions

| Component | Revision |
| --- | --- |
| workspace root | `3aa24c52914634f99b8957c04257cc7f5f4dd481` |
| `gwz-core` | `b4d8c63d2c15d0bd9d13288250c7c4303c27177c` |
| `gwz-cli` | `a3fb71e7e6a101b33d5da1ecf1e134a7af31ad55` |
| `gwz-py` | `423d08d572b4fcfd86ac2291847fe81052ecad09` |
| `taut` | `f00841966663bb0d500974ede446d30e9bfeeac4` |
| `taut-shape` | `389c867010a7cb474c03960ba3420308ed2bd64a` |
| `taut-shape-rs` | `7e50457391489d5a2628a60c0462f07f53baf021` |
| `taut-shape-py` | `7e7d5a7fc27635b152e10815503f1b456ae6a960` |

The baseline worktree was clean immediately after the synchronized refactor
proposal/design/plan commit.

## Measured merge baseline

| Area | Files | Handwritten LOC |
| --- | ---: | ---: |
| `gwz-core/src/workspace_ops/merge`, all Rust | 63 | 15,324 |
| merge production-bearing Rust, excluding `tests/` paths and `tests.rs` | 44 | 11,155 |
| merge-local dedicated test modules, including `tests.rs` | 19 | 4,169 |
| `workspace_ops/tests/g23` integration tests | 19 | 5,617 |

Current production review triggers:

| Module | LOC |
| --- | ---: |
| `merge/mod.rs` | 1,158 |
| `merge/model.rs` | 805 |
| `merge/finalize.rs` | 728 |
| `merge/continue_op/execution.rs` | 481 |
| `merge/store.rs` | 476 |
| `merge/plan.rs` | 440 |
| `merge/preserve.rs` | 418 |
| `merge/response.rs` | 377 |
| `merge/status/classify.rs` | 337 |

The 500-line threshold is a review trigger. R1 may reduce these concentrations
through behavior-preserving moves, but must not manufacture small files with
shared policy or count moved lines as semantic deletion.

## Frozen package ceilings

### R0 — characterization and compatibility infrastructure

Intentional production behavior delta: **none**.

Wire/protocol delta: **none**.

| Ownership slice | Production LOC add/delete | Moved LOC | Test/tool/doc LOC | Files touched/created |
| --- | ---: | ---: | ---: | ---: |
| retained-reader manifest/harness | 0 | 0 | ≤7,100 | ≤23 |
| document-consistency manifest/checker | 0 | 0 | ≤600 | ≤4 |
| v0 characterization fixtures | 0 runtime | 0 | ≤2,500 | ≤8 |
| baseline/state/mutation inventory and this ledger | 0 | 0 | ≤600 | ≤2 |

R0 may update test/build registration only where needed to make its offline
checks executable. It may not change merge runtime behavior, record bytes,
protocol values, or public feature availability.

The initial 500-line characterization allowance was expanded after the R0
inventory identified four independent missing proof domains: transition/action
tables, publication/recovery windows, archive-only evidence, and
preservation/rollback boundaries. Each additional slice remains capped at 500
lines and owns a separate test file; the aggregate ceiling prevents the
inventory from being treated as proof while preserving bounded review units.
The eight-file allowance includes two registration/`#[cfg(test)]` fault-seam
touches; those hooks are absent from non-test builds and add no runtime
behavior.

The retained-reader allowance was first expanded after tagged-release
inspection disproved the assumed v0.9.2 decoder generation and required three
separate concerns: artifact/manifest verification, isolated fixture capture,
and behavioral matrix execution. The two independent R0 interface reviews then
demonstrated six P2 false-positive paths in that first harness. Closing them
required a fail-closed schema evaluator, reviewed fixture contract, typed JSON
contracts, semantic mutation identities, interpreter-bound ephemeral runtime
preparation, process isolation, and a dedicated adversarial suite. The reviewed
scope decision therefore replaced the interim 3,700-line/13-file ceiling with
a 5,600-line/20-file ceiling. The final harness re-review then found two
remaining false-positive paths: checked evidence did not prove an exact result
set and complete evaluator-source freshness, and mutation evidence pinned
paths without pinning critical durable contents. Closing those paths required
three bounded modules for semantic YAML/Git observations and evidence-integrity
tests. The reviewed R0 closeout ceiling was therefore 6,600 lines/23 files; it
does not authorize production, wire, record, or merge-behavior changes. The
first exact-SHA five-platform run then exposed two additional portability
boundaries: platform-local Git administration bytes in fixture identity and a
Linux-only physical rewrite of the exact workspace boundary. Two independent
interface reviews required repository-semantic identity, object-integrity and
Git-admin fail-closed tests, and an asserted final boundary. Post-implementation
review then found that top-level classification still admitted active hooks,
legacy branch authority, ref locks, and filesystem indirection below `.git`.
The narrow post-CI allowance is therefore 7,100 lines/23 files. It remains
test/tool/doc only. The implemented retained-reader package is 6,968 lines/23
files, and every Python implementation/test module remains below 500 lines.

The document checker includes its reviewed assertion manifest in the ceiling
instead of treating those assertions as generated output.

### R1 — behavior-preserving ownership and policy extraction

Intentional production behavior delta: **none**.

Wire/protocol delta: **none**.

| Ownership slice | Net production LOC add/delete | Moved LOC | Test LOC | Production files | Test files |
| --- | ---: | ---: | ---: | ---: | ---: |
| runtime dispatch/open-gate/mutation-guard extraction | ≤120 | ≤900 | ≤250 | ≤6 | ≤4 |
| store seam/persistence extraction | ≤100 | ≤400 | ≤200 | ≤4 | ≤3 |
| participant semantic policy extraction | ≤250 | ≤500 | ≤400 | ≤6 | ≤5 |

The table above is the pre-implementation budget. The mandatory R1 scope
review was triggered after the frozen interface reviews required exhaustive
state, aggregate-precedence, rollback, missing-repository, conflict-validation,
drift-order, and root-override matrices. Two independent post-implementation
reviews found no P0/P1/P2 defect and accepted this measured adjustment:

| R1 execution slice | Reviewed production net | Reviewed moved production | Reviewed test LOC | Reviewed files |
| --- | ---: | ---: | ---: | --- |
| runtime extraction | ≤120 | ≤900 | 666 moved; ≤250 new | ≤6 production; ≤4 test |
| store extraction | ≤100 | ≤400 | 182 moved; ≤200 new | ≤4 production; ≤3 test |
| participant semantic policy | ≤500 | ≤650 | ≤1,550 | ≤6 semantic production; 14 named lifecycle call sites; ≤8 test |

The measured participant slice has 968 lines in its five semantic production
files, while the named lifecycle call sites add 139 and remove 647 lines. Its
physical net is therefore **+460 production lines**. The moved/centralized
figure is conservatively bounded by all 647 removed call-site lines rather than
using similarity heuristics to hide semantic growth. The eight focused test
files contain **1,489 lines**. No production, wire, record, event, error, or
mutation-order scope was added.

`participant_semantics/status.rs` has 518 production lines. The independent
reviews accepted a 550-line reviewed ceiling because its expected-head,
ordered-drift, normalized-fact, missing-repository, validation-overlay, and
root-override policy is one cohesive projection. Its tests live in separate
files, and every other new R1 implementation or test file remains below 500
lines. Further growth reopens the split review; the exception is not a general
increase to the 500-line target.

Allowed production ownership:

- `gwz-core/src/workspace_ops/merge/mod.rs`;
- new `merge/runtime/{mod,dispatch,open_gate,mutation_guard}.rs`;
- `merge/store.rs` and new `merge/store/{mod,persistence}.rs` as required by
  the accepted module layout;
- new `merge/participant_semantics/` pure-policy modules; and
- minimal module declarations/imports in existing merge orchestration files.

R1 must land mechanical movement separately from call-site semantic
centralization. Serialized types, event order, responses, errors, and Git or
filesystem mutation order remain byte-for-byte/behaviorally unchanged.

### R2a — v0-safe integration/message seam

Intentional production behavior delta: group the existing integration intent
and route existing pending merge actions through one typed integration seam.

Wire/protocol delta: **none**. The v0 serialized record must remain unchanged.

| Metric | Ceiling |
| --- | ---: |
| net production LOC | ≤300 |
| moved production LOC | ≤250 |
| test LOC | ≤450 |
| production files touched/created | ≤6 |
| test files touched/created | ≤5 |

Allowed scope is exact source, current target branch, before/source commits,
custom-message bytes, and current prepared-commit recovery. Checkout intent,
branch ownership, optional participation, snapshot provenance, accepted
workspace persistence, and composition policy are forbidden.

The implemented R2a seam measures **+141 net production lines**, no more than
**180 moved production lines**, and approximately **221 test lines**. It
touches four test-bearing paths. Production accounting is eight physical paths:
the six semantic live paths frozen by the checkpoint, an import-only cleanup in
the `continue_op.rs` parent, and the deleted `pending.rs` path replaced by
`integration.rs`. Seven production paths remain after the change. The
post-implementation review therefore considers an explicit eight-path ceiling;
the extra live path adds no ownership, behavior, wire value, or recovery
decision. The deleted and replacement paths are counted separately rather than
hidden as a similarity-based rename.

An independent interface review found one pre-implementation P1: contradictory
pending-action and participant resolution parents could be trusted by separate
recovery paths. The corrected seam fails closed when a present
`expected_merge_head` differs from the frozen source commit, while preserving
the released v0 absent-field fallback. Both interface reviewers approved the
amendment before implementation. Full Rust, strict Clippy, formatting,
protocol, Python parity, document-consistency, retained-reader, and Bazel build
gates are green.

The Bazel gate also refreshed the generated root `MODULE.bazel.lock` for the
already-landed Windows-only `windows-sys` dependency. This is build metadata
catch-up from the current committed Cargo manifests, not R2a production or a
new dependency introduced by R2a; it is included in the R2a scope review so it
cannot pass unnoticed.

Two independent post-implementation reviews returned **PASS** with no P0–P3
finding. They accepted the eight-physical/seven-live-path exception and the
generated Bazel-lock catch-up. R2a is therefore closed at the measured values
above.

### M5a — v0-safe custom messages

Intentional behavior delta: expose and execute user-supplied custom merge
messages while retaining the mandatory GWZ recovery identity and current v0
recovery authority.

Wire/protocol delta: only the already-designed request/message surface may be
activated; the durable v0 record shape and enum values do not change.

| Ownership slice | Production LOC | Test/doc LOC | Files touched/created |
| --- | ---: | ---: | ---: |
| core lifecycle/message construction | ≤180 | ≤350 | ≤9 unique (≤5 production; ≤5 test-bearing) |
| Rust CLI request/help/rendering | ≤100 | ≤250 | ≤5 |
| Python client/CLI/parity | ≤120 | ≤300 | ≤6 |
| public capability/generated-doc source updates | 0 runtime | ≤200 | ≤5 |

M5a must reject `--no-ff` before record creation. No path may serialize
`mode: no_ff`, advertise no-ff as available, introduce a v1 writer, or alter
the v0 record schema.

The original six-file core ceiling was stopped and reviewed when the complete
release-gate evidence reached nine unique files: five narrow production owners
and five test-bearing files, with `validate.rs` counted in both groups. The
reviewed nine-file ceiling keeps the production surface at five and permits
the distinct start/restart, validation, preservation/abort, and root
finalization proof domains without combining unrelated tests into a god file.

The implemented M5a slice is within every frozen ceiling:

| Ownership slice | Measured production net | Measured test/doc net | Files |
| --- | ---: | ---: | ---: |
| core lifecycle/message construction | +17 | +132 focused test | 9 unique: 5 production; 5 test-bearing |
| Rust CLI request/help | +5 | +5 test | 1 production; 1 test |
| Python client/CLI/parity | +2 | +155 test | 1 production; 2 test |
| public capability/generated-doc sources | 0 | +28 doc/tool | 5 |

No production line moved and no protocol, durable-record, event, response, or
enum shape changed. `MergeRequest.message` was already present on both driver
surfaces; M5a exposes it only for start, validates it through the R2a owner,
and continues to reject `no_ff` before record creation. The exact custom bytes
are proven across immediate true merge, persisted restart, conflict resolution,
interrupted preserve-abort recovery, and a root-participant true merge. Actual
Rust/Python executions match in human, JSON, and JSONL modes. Root composition
publication retains its separate message. Every touched handwritten
implementation or test file remains below the 500-line review trigger.

Formatting, full Rust tests, strict Clippy, full Python/native tests, generated
protocol/reference freshness, document consistency, retained-reader offline
checks, and the Bazel build are green. Two independent post-implementation
reviews report no remaining P0–P3 finding, including after the early-validation
and reviewed file-ceiling remediations.

### I1/I2 — v1 durability interface freeze

Intentional production behavior delta: **none**. I1/I2 freeze interfaces and
bind released v0 observations; executable v1 behavior remains in R4a/A1.

Wire/protocol delta: **allocations and projection contracts only**. No generated
protocol or runtime writer is activated in this package.

The I2 reviews required exact retained-reader coverage for six future envelope
pairs across eight lifecycle paths, three projections, and both retained
v0.10.2 drivers. The resulting 301-case/314-result matrix cannot fit the R0
closeout ceiling without making checked data unreviewable. This reviewed
amendment replaces the affected R0 ceilings only for the I2 interface proof:

| Ownership slice | Test/tool/doc LOC | Files | Additional boundary |
| --- | ---: | ---: | --- |
| retained-reader manifest/harness/evidence | ≤9,000 | ≤23 | readable templates; one checked result per line; every Python module <500 |
| I2 compatibility registry/checker | ≤850 | ≤3 | separate from the R0 document-consistency checker |
| v0 characterization plus I2 live binder | ≤2,750 aggregate | ≤9 touched/created | named owner ceilings below; all merge-path seams `#[cfg(test)]`; zero runtime delta |
| I1/I2 contracts, direction memo, and plan integration | ≤2,500 | ≤7 | design/contract text and checked registry references only |

The retained-reader expansion must remain a reviewed matrix/template, not a
generated list or a minified giant line. For this named I2 characterization
package, the amendment supersedes R0's 500-line-per-slice rule with exact owner
ceilings: `characterization_preservation_v0.rs` ≤525,
`characterization_v0.rs` ≤650, `compatibility_v0.rs` ≤650, and every other
characterization owner ≤500. The ≤2,750 aggregate is the six files
`characterization_{archive,preservation,publication_prefix,publication,v0}.rs`
plus `compatibility_v0.rs`; it excludes registration, fixture, and test-only
production seams. The compatibility binder exception keeps descriptor
normalization, live observation, canonical hashing, and bidirectional registry
inventory together; further growth reopens the split review.

The nine touched/created Rust paths are the binder, two modified
characterization owners, fixture and registration changes, and four test-only
merge seams. The latter four are outside the characterization LOC aggregate and
none is present in a non-test build.

### R4a — behavior-preserving acceptance extraction

Intentional production behavior delta: **none**.

Wire/protocol delta: **none**. R4a adds no writer, migration, schema,
discriminant, or accepted-workspace persistence.

The first implementation review stopped the aggregate change because it bound
the new next-action policy only as an assertion and left a 923-addition
finalizer-shaped review unit. The reviewed remediation made the action policy
the production dispatcher and split policy, orchestration, and support into
independently reviewable owners. A second review found two resume paths that
advanced their durable phase after mutation; the corrected dispatcher restores
the released preflight and write-before-mutation order and has nested-fault and
participant-drift regression coverage.

Physical production accounting is **994 lines added and 485 deleted** across
nine touched or created production paths. Of the deletions, 485 lines are
conservatively counted as moved or centralized rather than semantic removal;
the physical net is **+509 production lines**. Focused tests add **637 lines**
and delete 9 across the acceptance owners, shared fixtures, compatibility
binder, and recovery-order assertions. The independently testable slices
remain within the 500-line ceiling:

| R4a slice | Review size | Evidence |
| --- | ---: | --- |
| complete-lock and accepted-root policy | ≤430 production/test changed lines | 235-line pure owner, 133-line focused test owner, narrow root/finalizer adapters |
| publication-prefix and next-action policy | ≤355 production/test changed lines | 180-line pure owner, 155-line focused test owner, narrow publication adapter |
| production dispatcher binding | ≤480 changed lines | 278-line dispatcher plus compatibility binding and recovery-order assertions |
| mechanical finalization support split | ≤220 changed lines | 191-line support owner plus narrow imports/visibility wiring |
| shared fixture and module wiring | ≤165 changed lines | 145-line fixture, 3-line test root, and module registration |

No affected production file exceeds 500 lines: `finalize.rs` is 398,
`finalize_dispatch.rs` 278, `finalize_support.rs` 191, and the pure acceptance
owners are 235 and 180 lines. The focused finalization and drift test owners are
also below 500 lines. The pre-existing 594-line compatibility binder remains
within its separately approved 650-line I2 exception.

The final gate has 7 focused acceptance tests, 107 g23 merge/recovery tests,
688 full core tests (687 passed, 1 ignored), 329 Python/native tests, strict
all-target Clippy, formatting, and diff hygiene green. Two independent
interface re-reviews returned **GO** with no P0–P3 finding.

### R3 envelope and protocol foundation checkpoint

Intentional production behavior delta: **strict fail-closed durable-record
classification only**. Released v0 records retain their model, writer, rewrite,
and lifecycle behavior. Malformed envelopes, allocated-but-unsupported pairs,
and unknown valid pairs now follow the frozen I2 typed error contract before
body adaptation or mutation.

Wire/protocol delta: **the accepted I2 error slice**. Codes 46–61,
`MergeRecordRequiredWave`, and optional `GwzError.record_context` are generated
from taut and projected identically by core, Rust CLI, and Python/native
human/JSON/JSONL paths. The production writer remains v0 and no v1 record,
archive projection, or migration is published.

The checkpoint stays within the frozen per-owner ceilings:

| R3 slice | Review size | Evidence |
| --- | ---: | --- |
| mechanical v0 model split | 805 moved lines plus 25 net split/test wiring lines | lifecycle 163, plan 23, status 57, v0 wire 259, focused tests 315; no wire or behavior change |
| strict envelope decoder | 623 production lines across four responsibility owners plus wiring | decode 48, header 167, raw YAML 261, scalar 136, module 11; 331 focused test lines |
| store integration | 364-line store owner plus 157-line compatibility-error owner | one-read production-v0 dispatch, location-aware errors, fail-closed archive/retention/targeted GC; 393-line store test owner |
| protocol and driver projection | bounded generated schema plus narrow adapters | taut-owned codes/context, boxed internal context, Rust and Python/native machine parity |

`yaml-rust2` 0.11 is the one new direct dependency. It supplies the event
stream needed to reject duplicate keys at every depth, anchors, aliases,
semantic `<<` merge keys, multiple documents, and non-mapping roots in one
parse. Cargo and Bazel locks contain the same resolved parser dependency. No
affected handwritten implementation or test owner exceeds 500 lines; the
strict decoder is deliberately split by parser, scalar resolution, header
classification, and body dispatch responsibility.

The production installed-version set is v0-only. The v1 dispatch arm exists
only for header/unit tests; production exact-v1 input returns code 46 with A1
context before body decoding. V1 writing, open-v0 migration, and atomic upgrade
publication remain absent from the production call graph.

The settled-tree closeout passed 710 core library tests with 1 ignored, every
Rust integration suite, 329 Python/native tests and protocol regeneration,
strict all-target/all-feature Clippy, formatting, diff hygiene, and the Bazel
core/CLI build. Independent review returned **GO** with no P0–P3 finding. This
accepts only the bounded envelope/protocol foundation; the remaining R3 work
listed in the implementation plan is still required.

## Package reporting template

Every package handoff records:

```text
package:
baseline revision:
production LOC added/deleted:
production LOC moved:
test/tool/doc LOC added/deleted:
production files touched/created:
test/tool/doc files touched/created:
wire/protocol delta (expected / actual):
ceiling result:
scope exceptions requested:
```

The lead updates this ledger only when a reviewed scope decision changes a
ceiling. Passing under a ceiling does not substitute for behavioral tests or
interface review.
