# R4b-P0 settled-tree code review — state, restart, and authority

Date: 2026-08-10

Reviewer: independent state-machine code review

## Verdict

**NO-GO.**

P0 is not yet a safe shared prerequisite for P1-P4. Three P1 defects remain in
the frozen state/authority surface, and two P2 findings remain in its executable
evidence and controlling manifest. No P0 or P3 finding was identified.

The implementation-discovered addition of
`transition/reduce/{mod,participant}.rs` is accepted. The three visibility-only
changes keep `record_outcome` and `abandon` inside the transition owner and let
the preview call the reducer's existing predecessor helpers rather than copy
their policy. The corrected 25-production-path P0 manifest and
72-charged/61-unique R4b-P production accounting are coherent; these paths were
already present in R4b-TR, so aggregate unique production remains 111.

## Review basis

This review inspected the current uncommitted implementation against:

- `GwzM5-8R4bReverseLifecycleInterface.md`;
- `GwzM5-8R4bTransitionDesign.md`;
- both accepted `ReviewArch-2` and `ReviewState-2` interface reviews; and
- the P0 ownership, path, and LOC stop gates.

The review traced exact predecessor selection, preview/handoff/preflight/entry
binding, publication reconciliation, recovery routing, physical-action closure,
archive provenance, intervening-write rejection, and focused test coverage.

## P1 findings

### P1-1 — Reverse-entry authority omits the lifecycle request and admits illegal direct predecessors

`transition/reverse_entry.rs:26-34` does not retain the
`V1LifecycleRequest` used to create `PreparedReverseEntryView`.
`authority.rs:108-119` similarly binds only `ReverseEntryKind` and the
anticipated-model digest in `ReverseEntryAuthorityPayload`. F's handoff, the
P1/P2 preflights, and the entry constructors consequently cannot prove the
exact request required by the accepted interface.

This is observable for an exhausted preservation entry: the same source,
anticipated model, and `ExhaustedRollback` kind can currently be authorized
under one non-Status request and replayed as if produced under another. This
finding does **not** change the accepted dispatcher matrix. A `Preserving`
record continues its exact preservation cursor for every non-Status request and
may enter rollback after fresh preservation-exhaustion proof. The defect is the
missing identity of the accepted request, not the set of accepted requests.

There is a second defect in `entry_kind` plus the action-free path at
`transition/reverse_entry.rs:118-149`: outside `Preserving`, `Abort` and
`Preserve` are mapped to direct entry kinds without first restricting the
operation state. An action-free `Completed`, `Aborted`, `RollingBack`, or
`RecoveryRequired` record can therefore obtain a preview and F handoff even
though the reducer later rejects that state as a direct predecessor. Authority
must not be issued first and left to the final reducer to discover that the
predecessor never existed.

Required correction:

- add the minimal `request: V1LifecycleRequest` field to
  `PreparedReverseEntryView` and `ReverseEntryAuthorityPayload` (or one small
  request/kind/anticipated-digest identity shared by them);
- carry and compare that value through the visitor, F handoff, P1/P2 preflight,
  and production entry constructors; the prepared entry may bind it through
  the retained matching handoff/preflight authorities;
- enforce the reducer's exact direct state set before issuing a preview:
  `Executing`, `AwaitingResolution`, `Halted`, or `Finalizing`; allow
  `Preserving` only through the exhausted form and the accepted non-Status
  request matrix; reject terminal, rolling-back, and recovery states; and
- keep using transition-owned reducer helpers/predicates so this check does not
  become a second reducer policy.

The existing source digest, workspace/merge/operation lineage, kind, and
anticipated-model comparisons are otherwise correctly layered. Their missing
negative tests are covered under P2-1.

### P1-2 — F does not bind the exact publication handoff fact

`authority/observe/finalization/handoff.rs:61-73` correctly returns an exact
live `EvidenceResult` for durable recording before any reverse entry. For all
other finalizing observations it discards the result of `publication::observe`,
performs a second `recovery_origin_is_exact` read that returns only a Boolean,
and issues the same `{ kind, anticipated_model_sha256 }` handoff.

That loses the distinction frozen in the transition design at lines 575-582:
an exact no-effect/pre-publication handoff and each exact partial candidate
file/index prefix are not interchangeable. The partial prefix must be bound so
the preservation or rollback journal can own precisely that state. The current
record digest binds durable publication fields, but it does not bind mutable
root files/index/worktree observed by F. Two successful reads can also observe
different live shapes while issuing one indistinguishable token.

Required correction:

- have F perform one exact handoff classification that returns a closed fact,
  at minimum exact no-effect versus the exact candidate file/index prefix;
- retain `RecordEvidence` as the sole result when exact unrecorded evidence is
  present, and propagate operational errors or reject mixed/ambiguous forms;
- include the closed publication fact in the sealed handoff payload consumed by
  the P1/P2 preflight and entry construction, so a different exact prefix cannot
  reuse it; and
- require the later lane classifier to revalidate the bound fact before any
  reverse physical mutation, preserving normal intervening-write rejection.

### P1-3 — `ValidatedArchivedMerge` can combine unrelated bytes, projection, and cleanup authority

`v1_lifecycle/archive_result.rs:6-18` declares a second cleanup-ref/worklist
model. Its `ValidatedArchivedMerge::new` at lines 29-44 is available to every
`v1_lifecycle` sibling and accepts destination bytes, projection, and cleanup as
three unrelated caller values. It computes a byte hash but proves neither
canonical no-follow destination acquisition nor that R3 decoded those exact
bytes into the supplied projection/worklist.

The focused test at `tests/archive_result.rs:11-28` demonstrates the defect: it
uses incomplete hand-written archive bytes and independently fabricated
projection and cleanup values. Private result fields prevent later mutation;
they do not establish validation provenance. This boundary will become P4's
backup-ref deletion authority and P3's archive projection source.

The existing R3 object that P0 must consume is
`record_wire::archive::ValidatedArchivedRecord`, defined in
`record_wire/archive/mod.rs:19-23` and returned by
`decode_archived_for_r3_tests` at lines 25-44. It already keeps the R3-derived
`ArchivedMergeProjection` and
`record_wire::archive::cleanup::ArchivedCleanupWorklist` together; the latter
is defined in `record_wire/archive/cleanup.rs:6-18`.

Required correction:

- remove the shadow cleanup types from `archive_result.rs`;
- make construction consume a non-cloneable, P4-owned canonical-destination
  acquisition capability that bundles the final destination bytes with the one
  `ValidatedArchivedRecord` decoded from those bytes;
- make the capability's fields and acquisition constructor private to the P4
  archive module, so P3 and other lifecycle siblings cannot fabricate it;
- derive the source version and SHA-256 internally and retain/re-expose only
  read-only views of the R3 projection/worklist; and
- test with real valid archived v0 and v1 bytes through R3 decode and canonical
  acquisition, including rejection/unrepresentability of cross-byte pairing.

If the R3 decoder's test-oriented entry-point name must become a production
entry point, that ownership/path change must be explicitly charged rather than
copying the decoder or cleanup model into P0.

## P2 findings

### P2-1 — Focused tests do not execute the frozen negative matrices

The focused tests prove several success paths but leave the authority boundary
under-specified:

- `preview_rejects_stale_lineage_and_cross_kind_reuse` tests only a changed
  source record; it never attempts cross-kind reuse;
- there is no cross-request replay, wrong anticipated digest, stale handoff,
  stale preflight, stale prepared entry, or operation-state × request ×
  predecessor table;
- F is not exercised against real finalizing live evidence and every exact
  candidate prefix, including evidence-first recording followed by a fresh
  digest/handoff and rejection of mixed or changed prefixes;
- router tests omit `PreservationCursor`, `RollbackEntry`, forward recovery
  origins, `Finalizing` recovery, missing recovery context, unsupported forward
  observations, and wrong-request participant routing;
- physical-router tests reject only `Archive`, not `Participant`, `Publication`,
  or a wrong-location lease, and do not prove that no forbidden lane ran; and
- archive-result coverage fabricates the handoff instead of starting from R3
  decoded v0/v1 archive bytes.

The production router match is otherwise state-exact in this review: reverse
recovery origins delegate to their own lane, forward and `Finalizing` origins
delegate to the accepted forward observer, and physical execution admits only
preservation/rollback after the lease-location check. The missing rows are
required executable closure, not a request to change that match.

### P2-2 — The exact P0 test/tool/doc manifest and remaining ceilings are not reconciled

The interface charges eight P0 test/tool/doc paths. In addition, the current P0
implementation modifies six existing test paths:

- `v1_lifecycle/tests/{mod,authority,dispatcher_attempt_matrix}.rs`;
- `v1_lifecycle/tests/{dispatcher_reconciliation,predecessor_matrix,reducer}.rs`.

The two required P0 code-review memos are also distinct from the two aggregate
final settled-tree reviews. The honest P0 charge is therefore 16 paths, not 8,
and the R4b-P test/tool/doc charged/unique total becomes 65 rather than 57.
The parent aggregate unique total must be recomputed for overlap with earlier
R4b-TR test owners; it cannot be inferred by adding eight unique paths blindly.

Before this memo, the five interface/review documents (1,842 lines), three new
tests (563 lines), six modified tests (net +41), and architecture code-review
memo (201 lines) already consume 2,647 lines of the 3,000-line P0
test/tool/doc ceiling. This memo and the required negative tests remain to be
charged. Production is currently net +855/900, leaving 45 lines before the P1
corrections. Reconcile both ceilings on the remediation diff before proceeding;
if the exact fix does not fit, revise the reviewed ledger rather than trimming
the required state matrix.

## P0 and P3 assessment

No P0 correctness defect or P3 quality-only finding was identified. The three
P1 findings are blocking because P1-P4 would otherwise have to widen or replace
P0-owned shared authorities after parallel work begins.

## Verification evidence

- `cargo test -p gwz-core workspace_ops::merge::v1_lifecycle::tests -- --nocapture`:
  **94 passed, 0 failed**.
- `cargo test -p gwz-core`: library **932 passed, 1 ignored**, plus integration
  groups of **10, 27, 4, and 2 passed**, with no failure.
- `cargo fmt --all --check`: passed.
- `cargo clippy -p gwz-core --all-targets -- -D warnings`: passed.
- Current production delta: net **+855 / 900** across the corrected **25 / 25**
  P0 production paths.

The future aggregate docs-manifest checker named by the interface is not yet in
the tree, so no automated exact-manifest gate was available. The green Rust
checks establish build and regression health; they do not close the missing
authority facts or negative matrices.

## GO gate

P1-P4 must not start. P0 may be accepted only after P1-1 through P1-3 are
corrected, P2-1 is executable, P2-2 and both LOC ceilings are reconciled, the
full checks pass on the settled remediation tree, and both independent code
reviewers report no open P0/P1/P2 finding.
