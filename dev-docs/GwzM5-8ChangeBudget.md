# GWZ M5–M8 Refactor Change-Budget Ledger

Date: 2026-07-31

Status: **R0, R1, R2a, M5a, I1/I2, R4a, and production-disabled R3
complete; R4b-TI/R4b-TR and production-disabled R4b-S/R4b-A are
independently accepted; the R4b-F implementation and its revised scope ceiling
are independently accepted; R4b-X and its revised scope ceiling are
independently accepted; R4b-P P0/P0.1 are independently accepted and P2-P4 are
implemented; both amendment 2 interface reviews are GO through §13 and the
production-disabled P0.2 implementation and bounded settled-code remediation
are accepted by both corrected-code reviews with no open P0-P3 finding; the
Windows portability remediation is code-GO; both §14 settled-implementation
reviews and the exact-SHA native release-platform run are GO; P1-WR1 is closed
and P1 is implemented with local tests and static analysis green, pending
settled-tree review**

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

Current production concentrations at the measured baseline:

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

The general god-file review threshold is 1,000 lines. Reaching or modestly
crossing it prompts a cohesion review rather than an automatic split. When a
split is warranted, the resulting responsibility owners should each be below
500 lines. Size is only a backstop: a smaller file that is accumulating
unrelated concepts or becoming a policy dumping ground is split earlier. The
package plan should define those ownership boundaries before implementation so
the split is architectural rather than cleanup churn. R1 may reduce these
concentrations through behavior-preserving moves, but must not manufacture
small files with shared policy or count moved lines as semantic deletion.

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

The next R3 wire-shape slice added 327 test-only production-shaped lines across
four owners (accepted workspace 120, action journals 149, complete record 48,
module wiring 10), plus a sub-500-line golden test owner and a narrow test-only
strict decoder entry point. The following validated-model checkpoint adds
strict baseline, participant/action/lifecycle, acceptance, publication,
rollback, and preservation cross-field validation plus an opaque canonical
adapter. Responsibilities are split across sub-500-line source and test
owners. Focused v1 validation passed 39 tests, record-wire passed 17, the full
core library passed 760 with 1 ignored plus every integration suite, and the
settled-tree independent review returned **GO** with no P0–P3 finding.

Both checkpoints compile no v1 body or serializer into a normal library build
and introduce no production writer or migration call path. The next reviewed
slice adds the identity-aware v0/v1 unknown-field manifest, semantic
sequence/action/error identities, collision-checked survivor overlay, all four
v0 top-level collision rejects, and location-correct archived contradiction
projection. It is split across sub-500-line source and test owners; record-wire
passed 38 tests, the full core library passed 783, strict Clippy/format/diff
hygiene passed, and independent re-review returned GO with no P0–P3 finding.
The following open-v0 adapter checkpoint is also implemented and independently
accepted. Its test-only owners are 349 adapter, 165 baseline-recovery, 474
descriptor, and 352 structural-validation lines, each below the 500-line R3
ceiling. The 641-line compatibility binder remains inside its reviewed
650-line exception; a separate 193-line edge owner covers legacy action,
baseline recovery, valid-unlisted, and no-ff precedence cases. The adapter
binds typed and raw v0 evidence, recovers missing baseline bytes only from the
closed I2 sources, verifies live evidence and invariant-specific errors, maps
unknown fields losslessly, and produces a fully validated canonical v1 model.
All code remains behind `cfg(test)`; production still has no v1 writer or
migration path. G23 passed 107 tests, the full core library passed 782 with 1
ignored plus every integration suite, strict Clippy/format/diff gates passed,
and independent re-review returned GO with no P0–P3 finding.

The archive-projection slice is also implemented and independently accepted.
It adds only test-compiled model/projection, v0/v1 archive validation, and an
immutable cleanup-worklist seam. The responsibility owners are 290 lines for
the projection model, 211 for archive dispatch, 263 for v0 classification, 278
for member audit, 420 for candidate/root evidence, 161 for v1 projection, and
126 for cleanup ownership; every owner remains below 500 lines. Focused archive
tests passed 17/17. The final full core gate passed 801 tests with 1 ignored,
plus integration groups 10/10, 27/27, 4/4, and 2/2; strict Clippy, formatting,
diff hygiene, and the production-only check passed. Two independent
settled-tree reviews returned **GO** with no P0–P3 finding. Production store,
status, retention, GC, protocol, CLI, Python/native, v1 writing, and migration
remain unchanged and unreachable from this seam.

The final test-only atomic-upgrade slice is implemented and independently
accepted. Its separate owners are 106 lines for pure v0-to-v1 preparation, 187
for atomic store publication, and 411 for the real-Git/fault test package. The
compatibility binder is 644/650 lines. The harness requires one sole named open
v0 record, preserves every valid-unlisted or rejected source byte-exactly,
stages all seven eligible rules through semantic and unknown-manifest
verification, fsyncs before atomic replacement, and re-reads the published
hash and model. Four fault points prove pre-rename v0 survival, stale-temp
discovery exclusion, and post-rename v1 restart. A focused accepted-lock
extension case proves exact authoritative lock bytes and the derived audit-row
extension survive together.

G23 passed 107/107. The final full core gate passed 801 tests with 1 ignored,
plus integration groups 10/10, 27/27, 4/4, and 2/2; strict all-target/all-
feature Clippy, formatting, and diff hygiene passed. Both independent
settled-tree re-reviews returned **GO** with no P0–P3 finding. No v1 decoder,
serializer, writer, upgrade entry point, or migration dispatch enters a normal
production build. R3 is complete at its disabled-writer boundary; R4b is next.

### R4b — typed v1 lifecycle and persisted-acceptance consumption

Intentional production behavior delta: **the already frozen I2 durable-record
projection and archive-worklist semantics**. R4b keeps v1 lifecycle mutation
test-reachable behind the existing production-disabled boundary, but P appends
optional field 10 `MergeResponse.record` for existing durable-v0 successes,
changes existing v0 archive acquisition/retention/targeted-GC to the canonical
no-follow immutable-worklist rules, and adds the frozen SHA-256 and
stash/bundle-only rows. Archived-v1 decode/projection/GC is compiled and tested
but cannot be reached from a production-created v1 record before A1. P does not
activate v1 creation, migration, or lifecycle dispatch.

Wire/protocol delta: **the accepted I2 field-10 append and its frozen nested
projection/discriminants**. P0.1 adds one production-disabled v1-only durable
`preservation_publication_handoff` field and its journal binding; v0 bytes and
the user-facing protocol remain unchanged. P0.2's reviewed amendment replaces
the two compound root phases with exact per-object phases, including parent
phases before clean-marker creation and before handoff-marker restoration,
without adding a field, action, protocol shape, or production reachability. No
other durable
field, phase, action, or observation is authorized without another design
review.

`GwzM5-8R4bTransitionDesign.md` is the normative architecture. These are stop
ceilings, not implementation targets:

| Checkpoint | Net production-bearing Rust | Moved production Rust | Test/tool/doc LOC | Production files | Test/tool/doc files |
| --- | ---: | ---: | ---: | ---: | ---: |
| R4b-TI — no-wire validator corrections, v1/v0 isolation, checked types, sealed proof composition, closed transition/effect vocabulary and executable exact-effect verification | ≤2,100 | ≤150 | ≤1,100 | ≤22 | ≤14 |
| R4b-TR — reducers, next-action/observation dispatch, bound execution attempts, exact retirement and exhaustive predecessor/phase/cursor/footprint matrices | ≤2,750 | 0 | ≤4,400 | ≤12 | ≤16 |
| R4b-S — checked store, unknown overlay, lineage, exact archive | ≤1,050 | ≤250 | ≤1,000 | ≤15 | ≤8 |
| R4b-A — shared acceptance builder and publication classification input | ≤650 | ≤400 | ≤900 | ≤10 | ≤8 |
| R4b-F — acceptance-consuming finalization | ≤2,100 | ≤1,100 | ≤1,800 | ≤18 | ≤16 |
| R4b-X — participant/continue/recovery service | ≤1,050 | ≤1,100 | ≤1,500 | ≤14 | ≤12 |
| R4b-P — lead reverse-entry closure; exact root-preservation correction; preservation/rollback/recovery/status/archive/GC consumers; frozen I2 field-10 projection | ≤14,000 | ≤2,950 | ≤31,400 | ≤179 charged / ≤109 unique | ≤160 charged / ≤107 unique |
| R4b-G — aggregate gates and wiring only | ≤150 | 0 | ≤1,500 | ≤3 | ≤12 |

The aggregate unique-file ceiling is conservatively 150 production-bearing
paths and 150 test/tool/doc paths. The aggregate semantic-addition ceiling is
19,250 production-bearing lines and 38,300 test/tool/doc lines; the per-checkpoint
ceilings remain controlling even when the aggregate has room. Shared files are
counted in every checkpoint that changes them and once in the aggregate.

The general source-file threshold is 1,000 lines. Reaching or modestly crossing
it triggers a cohesion review, not an automatic split; when splitting is the
right architectural choice, the resulting responsibility owners should each
be below 500 lines. Explicit lower per-owner ceilings frozen for an individual
checkpoint remain controlling for that checkpoint. Moving v0 code into a v1
module does not count as semantic deletion and may not weaken the
version-isolation boundary. Independent concepts are separated earlier when
the planned ownership or dependency direction calls for it, regardless of LOC.

Allowed ownership is limited to:

- new `merge/v1_lifecycle/` checked-record, transition, store, observer,
  service, and focused test modules;
- the existing `model/v1`, `record_wire`, `acceptance`, and R4a pure semantic
  modules for narrow interface extraction or shared-builder delegation;
- minimal dispatch/module declarations required for a test-only v1 harness;
- existing participant, finalization, preservation, rollback, status, and
  archive modules only for extraction to typed adapters or wiring, not for a
  second v1 policy implementation; and
- this plan, its checker/manifest, and focused compatibility/fault fixtures.

The lead owns the `V1Transition` vocabulary, `PreparedV1Rewrite` visibility,
checked-store entry point, proof-token constructors, and acceptance-builder
interface. Parallel checkpoint owners cannot widen those interfaces, add a
transition variant, introduce a raw v1 writer, or change a phase graph without
a lead-owned interface checkpoint and independent review.

R4b-TI receives two independent architecture/interface reviews before R4b-TR
implementation begins. R4b-TR then receives two independent reducer/state-
machine reviews before R4b-S or R4b-A implementation begins. R4b-S and R4b-A each receive interface review
before R4b-F/R4b-X consumers start. R4b-G includes two independent settled-tree
reviews with no open P0/P1/P2 finding. A failed review or a requirement for an
unowned mutation stops the package; it is not covered by the ledger's ordinary
20% numeric tolerance.

The R4b-TR ceilings were revised after both required TR reviews independently
rejected the first implementation: its open resolver let observers supply a
transition, omitted the request from the proof binding, and lacked the required
cross-product matrices. Closing that authority requires separate observation,
resolution, and exact-execution owners, plus readable request/state, exact-
action/attempt, and prefixed-cursor matrices, rather than dense packing in one file.
The revised ceilings add no behavior, wire shape, transition, or production
dispatch; they cover only that reviewed authority split and its matrices. Both
TR re-reviews must accept the revised accounting before R4b-S or R4b-A starts.

The second TR reviews found that the first revision still omitted proof-bearing
publication and preservation action observations, request-sensitive handoff
and abandonment cases, exact post-attempt action binding, and the review memos
the gate itself requires. The final TR ceiling records those closed-boundary
and executable-matrix costs explicitly; it does not authorize another durable
field, phase, transition, runtime writer, or production dispatch path.
The settled implementation measures 2,723 production-bearing lines on the same
12 paths and 4,277 focused Rust test lines before the final review memos. The
2,750/4,400 scope-reviewed ceilings preserve readable proof types and the
mandatory operation-predecessor, publication-action, attempt, rollback, stash,
and reset matrices. The final evidence increment is test-only apart from a
two-line `#[cfg(test)]` effect-capture hook: it executes all 53 declared
footprints, crosses ambiguous observations with both executor outcomes for
every physical owner, rejects every wrong recovery origin, and rejects every
unlisted no-prefix stash/reset successor. The ceilings are not a tolerance for
further feature work.

R4b-A is implemented at the production-disabled boundary and independently
accepted. It adds 730 lines across two new sub-500-line builder owners,
removes 118 net lines from the duplicated R3 adapter builder, and adds eight
lines of module wiring, for **+620 net production-bearing lines**. Its focused
acceptance suite contains 202 lines. A therefore remains within 650 production
lines, 400 moved lines, and 900 test lines.

The first R4b-S interface review rejected the original ceiling as incomplete:
the checked-store boundary had not carried I2's full duplicate-aware
participant-drift identity, the mandatory 53-effect unknown-manifest matrix
was absent, and exact archive publication used a hard-link/unlink interval
instead of an atomic no-replace rename. Closing those frozen-contract defects
requires typed occurrence authority and survivor rebasing across transition,
record-wire, and store owners, plus a release-platform atomic rename primitive;
it adds no transition, wire field, runtime reachability, or user behavior.
The revised ceiling is a scope correction requiring re-review, not numeric
tolerance for feature work. The four dedicated store owners now contain 598
lines; the focused store suite contains 979 lines across four sub-500-line
owners and executes every one of the 53 effects through
`CheckedV1Store::commit`. Cross-boundary identity and archive support brings S
to approximately +970 net production-bearing lines with about 145 moved
lines, within the revised 1,050/250 ceilings. Independent remediation
re-review accepted R4b-S with no P0-P3 finding. Its 15 production paths are
the nine paths
from the first review plus the atomic no-replace helper, record-wire unknown
manifest owner, drift authority owner and wiring, reducer wiring, and
test-only prepared-rewrite seam; the prior 14-path entry omitted that last
shared path. Both S and A keep the wire/protocol delta at none and do not make
a v1 writer, upgrader, or lifecycle dispatch reachable in a normal build.

The first concrete R4b-F implementation exposed that the original 950-line
semantic ceiling counted the finalizer as a thin candidate consumer but did
not allocate the exact live observer required by the accepted transition
architecture. F must separately own participant/root acceptance observation,
evidence reconciliation, filesystem-prefix classification, raw-index
classification, physical execution, and the service adapter. Combining those
responsibilities would violate the sub-500-line owner rule; routing them
through the v0 mutable finalizer would violate version isolation.

Post-remediation review found that the 1,700/14/1,500 entry still described the
first candidate rather than the exact reviewed package. It omitted the
accepted-root byte/index verifier, complete marker semantic reconstruction,
all candidate-equality rows, operational-error classification, owned-parent
symlink rejection, and the third focused real-Git test owner. Keeping those
requirements in the five original owners would either cross 500 lines or
conflate filesystem, Git, and durable-state authority.

The settled review ceiling is therefore **2,100 production-bearing lines,
1,800 focused test/tool/doc lines, and 18 production paths**. The current
conservative F charge is **1,989 production-bearing lines across 17 paths**:
1,425 exact current lines in the six dedicated owners
(`65 + 64 + 463 + 457 + 170 + 206`), the previously charged 395-line shared
adapter surface, and 169 lines of candidate semantic validation added by the
review remediation. The 17-path count deliberately charges all shared wiring:
the six dedicated owners, marker, publication classifier/validator and its
module, three Git backend paths, the boundary helper, lifecycle module,
authority observer module, authority re-export, and merge-module wiring.
This is conservative where earlier checkpoints also touched a shared file.
Two independent R4b-F reviews reconciled this accounting and accepted the
finalization interface, state machine, recovery behavior, and scope with no
P0-P3 findings.

The focused F test charge is **1,681 lines**: 1,254 lines in the three
sub-500-line real-Git owners; 239 local candidate/live/observer test lines; 65
raw-index regression lines; 77 publication-validator regression lines; and 46
acceptance-fixture lines. It covers born-attached, born-detached, and
attached-unborn roots; the exact released parentless two-file unborn evidence
shape; selected-root evidence parentage; no-publication preservation; exact
restart suffixes; all candidate-equality rows; mixed and staged/tampered index
forms; noncanonical index flags; owned-parent symlinks; operational-error
propagation; and fail-closed drift/recovery without overwrite. Every owner is
below 500 lines. This reviewed scope correction adds no field, phase,
transition, wire shape, normal-build reachability, or user-visible behavior.
The required independent F interface and scope re-acceptance is complete.
Dependent packages may consume it only in the declared R4b DAG.

The concrete R4b-X implementation exposed that the original 900-line ceiling
did not include the complete exact-recovery verifier, the version-neutral
prepared-merge Git seam needed for deterministic no-ff, symlink-safe durable
participant path validation, or the repeated-failure footprint correction.
Those are required parts of the accepted transition architecture rather than
new wire shape or activation scope.

Two independent settled-tree reviews accepted a revised **1,050 net
production-bearing line ceiling** while retaining the original **1,100 moved,
1,500 test/tool/doc, 14 production-path, and 12 test/tool/doc-path ceilings**.
Their accounting methods produced 987 lines/13 production paths and 958
lines/14 production paths respectively; the ledger records the conservative
union of **987 production-bearing lines across 14 paths**. The conservative
focused Rust test charge is **976 lines across four paths**. The two review
memos add 370 lines, and the closeout plus general ownership-policy document
deltas add at most 80 lines, for a conservative **1,426 test/tool/doc lines
across nine paths**. Both reviews found no P0-P3 issue.

X freezes mode only during read-only preparation; the resulting durable
prepared variant is the sole authority for validation and execution. It
provides write-ahead participant execution, exact conflict path/hash evidence,
pre-owner continue classification, direct up-to-date adoption, checked
fast-forward and deterministic two-parent no-ff execution, and origin-specific
recovery that verifies every selected participant. Finalizing recovery
delegates to F; preservation and rollback recovery remain P-owned and are
rejected by X. The 610-line exact observer and 826-line focused forward test
owner are cohesive and below the general 1,000-line judgment trigger; the
reviews found no concept-driven reason to split them. X adds no field, durable
phase, transition variant, v0 mutation route, normal-build reachability, or
user-visible behavior.

The first R4b-P ceiling was withdrawn after both independent interface reviews
returned NO-GO. It omitted every production reverse-entry issuer, left the
transition-owned anticipated-model preview and F-owned publication handoff
undefined, assigned all reverse recovery to one lane despite preservation
classifier dependency, and treated I2 field 10 as an optional A1 sequencing
choice. It also lacked canonical no-follow archive deletion authority,
optimistic status lineage, and restart-complete decoded stash-image evidence.

The revised P ceiling is based on the enumerated path manifest in
`GwzM5-8R4bReverseLifecycleInterface.md`: 76 charged/63 unique production paths
and 70 charged/69 unique test-tool-doc paths. It allocates 1,200/4,200
production/test lines for the lead prerequisite, 950/1,800 for preservation,
900/1,800 for rollback, 900/2,500 for status/protocol, 750/1,500 for archive/GC,
and 100/1,500 for integration and documents. Movement is capped separately at
2,050 lines. Generated Rust/Python LOC is excluded while its three output paths
and the Taut source are counted.

The first P0 code reviews found that the 900/3,000 ceiling could not contain the
required exact-request authority, closed publication-prefix handoff, R3 archive
provenance, negative matrices, and retained review evidence. The corrected P0
manifest adds F's existing publication classifier plus R3's existing archive
decoder/cleanup owner and parent module; the P4 archive paths remain shared and
charged to both owners. The earlier two-path reducer correction remains
visibility-only and duplicates no policy. P0 must pass the same two reviewers'
code re-review before P1-P4 start. Both re-reviews now report GO with no open
P0-P3 finding, so that gate is satisfied. This is a scope correction for frozen
I2 behavior, not permission for a new durable phase, transition, record field,
or production v1 mutation route.

The remediated P0 evidence reuses F's focused finalization owner for the exact
publication-prefix/evidence-first matrix and R3's archive test module plus v0/v1
fixture for same-byte decoder provenance. Those three paths are charged to P0;
the archive fixture remains shared with P4. This raises only the P checkpoint's
charged/unique path accounting: all three paths already belong to earlier F/R3
work in the aggregate program union, so the aggregate 117-path ceiling is
unchanged.

The first P1-P4 consumption then exposed a missing durable publication index
handoff, closed authority observer entry points, a normal-build canonical
archived-v0 acquisition path, complete borrowed status inputs, and exact
named-ref observation for GC. The reviewed proposal is frozen in
`GwzM5-8R4bInterfaceAmendment-1.md`; it is a P0.1 interface correction, not a
new lifecycle phase or production v1 activation.

P0.1 reserves 1,400 production and 2,200 test/tool/doc lines across exactly 60
and 46 charged paths. Measured lane work also requires P1 950→1,350, P2
900→2,200, and P3 900→1,250 production corrections; P4 remains 750. P3 gains
four exact constructor/parity test paths but remains within 2,500 test lines.
The revised R4b-P package ceilings are therefore 8,250 production, 2,050 moved,
15,500 test/tool/doc, 136 charged production paths, and 120 charged
test/tool/doc paths. Conservative unique ceilings are 93 and 92. The prior
4,800/2,050/13,300 and 76/70 R4b-P row is superseded only for this package;
earlier aggregate packages and their accepted evidence are unchanged. The
two added common-view constructor paths raise only the P package union because
accepted R3 already charges both in the 137-path program union. The
`acceptance/mod.rs` parent re-export likewise raises only the P package union
because accepted R4b-A already charges it. The
`authority/binding.rs` location binding and existing authority test likewise
raise only the P package unions because accepted R4b-TI/P0 already charge them.
The
new P0.1 charges for `record_wire/unknown_fields/tests/{v0,nulls}.rs` and
`model/v1/validate/common_tests.rs` raise the P package union but not the
136-path program union because accepted R3/TI already charge those physical
paths.

At the current P0.1 start point, P1 is 508/1,350 production and 100/1,800 tests;
the 36/40 direct-ref increment is conservatively also charged to the shared
P0.1 slice. P2's
conservative HEAD delta is 1,941/2,200 and 842/1,800; P3 is 1,096/1,250 and
801/2,500; P4 is 508/750 and 532/1,500. P0.1 has 36/1,400 implementation lines
and 1,818/2,200 test/tool/doc lines: 1,778 final pre-implementation document
lines plus the shared 40-line direct-ref test increment.
Both independent interface re-reviews returned GO with no open P0-P3 finding,
so implementation may resume within the corrected manifests.

### R4b-P0.2 exact root-preservation correction

P1 implementation then proved that the accepted compound `NormalizeRoot` and
`RestoreRoot` phases could not represent separate marker, lock, and raw-index
durability boundaries. `GwzM5-8R4bInterfaceAmendment-2.md` replaces them with
the reviewed physical phase graphs, exact commit-derived `C0`/`C1` forms,
authority-derived handoff `H`, invariant-boundary classification, durable
marker-parent preparation/restoration, checked preimage/stash/reset execution, and a
closed fault/restart matrix.

The exact P0.2 allocation is:

| Component | Net production | Moved production | Test/tool/doc | Production paths | Evidence paths |
| --- | ---: | ---: | ---: | ---: | ---: |
| shared Git implementation/testing | 2,200 | 450 | 3,800 | 11 | 9 |
| model/transition integration | 450 | 0 | 1,100 | 3 | 7 |
| lifecycle causal seam | 450 | 0 | 1,000 | 9 | 3 |
| documents/reviews/control/platform gate | 0 | 0 | 5,200 | 0 | 8 |
| **P0.2 total** | **3,100** | **450** | **11,100** | **23** | **27** |

The shared Git and lifecycle rows are also charged to P1. They revise P1 to
4,000 net production, 1,500 moved, 6,600 test/tool/doc, and 29/20 charged
production/evidence paths.

The complete revised R4b-P charged ceiling is 14,000 production, 2,950 moved,
31,400 test/tool/doc, and 179/160 production/evidence path charges.
Conservative unique P-package ceilings are 109/107. At the program level the
shared P1 charge is counted once, producing 19,250 production lines, 38,300
test/tool/doc lines, and 150/150 paths because the resolver and lifecycle
evidence owners already belong to earlier checkpoints while the focused parent
and unknown-field owners are new.

The five additional P1 production paths are the lead-approved cohesion split
of the retained preservation observer into `cursor`, `entry`, `phase`,
`phase/evidence`, and `phase/steps` owners. Every resulting owner is below 500
lines. The six dedicated `reverse_preservation` evidence owners were already
reserved in the P1 manifest, so evidence path and line ceilings do not change.

The settled-code recount charges the causal seam's four existing parent export
paths to both P0.2 and P1. Those eight charged appearances add no unique path
because P0 already owns all four. The full Python parity gate also exposed the
one-line `MergeResponse.record` fixture omission in
`gwz-py/src/tests/test_client.py`; P3 evidence therefore rises from 17 to 18
charged paths and the P/program evidence unions each rise by one. No line
ceiling changes.

All twenty-three production and twenty-seven evidence owners are frozen in
amendment 2, including the one-path non-publishing Cargo Dist platform gate
allocated by §14. New cohesion owners remain below 500 lines; `contract.rs` remains below 950 and the
post-extraction `preservation.rs` below 850. Both protocol and owner-manifest
interface reviews are GO through §13. The first settled-code architecture
review found index-only provenance, manifest, evidence, and stale-control
defects; their bounded remediation is accepted by both corrected-code
re-reviews. With the allocated workflow implementation present, the measured actual
is 2,738/3,100 semantic production lines, 450/450 moved production lines, and
10,648/11,100 evidence lines across 23/27 implemented paths within the 23/27
manifest. Both §14 settled-implementation reviews and the exact-SHA run are GO.
P0.2 is accepted locally; P1-WR1 is closed. P1 is implemented against the
frozen seam with local tests and static analysis green and awaits settled-tree
review. This does not activate
production v1 writing or dispatch.

The document/review/control component measured 1,753 lines at its first
interface GO. Its 5,200-line ceiling reserves the §13 correction, retained
code-review, and bounded remediation/re-review appendices in the same two memo
paths; it cannot fund test or code scope and adds no path.

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
