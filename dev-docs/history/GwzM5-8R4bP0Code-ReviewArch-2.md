# R4b-P0 settled-tree code re-review — architecture and ownership

Date: 2026-08-10

## Verdict

**GO.**

The remediated P0 closes P1-1 through P1-5 and P2-1 from
`GwzM5-8R4bP0Code-ReviewArch.md`. This re-review finds **no P0, P1, P2, or P3
finding** in the current architecture, ownership boundary, authority chain,
router, archive provenance, path manifest, or stop ceiling.

P1–P4 may start after the independently required state/restart code re-review
also returns GO. This memo does not waive that second-review gate.

## Review basis

The current uncommitted P0 tree was checked directly against:

- `GwzM5-8R4bReverseLifecycleInterface.md`;
- `GwzM5-8R4bTransitionDesign.md`;
- both accepted interface `-2` reviews;
- `GwzM5-8R4bP0Code-ReviewArch.md`;
- the exact 29-production/21-test-tool-doc manifest; and
- the current transition reducer, authority observer, F publication
  classifier, fixed reverse runtime, R3 decoder/cleanup owner, and focused
  negative tests.

The first review's Abort-only recommendation for a `Preserving` record was
over-narrow and is superseded by the accepted contract. The controlling rule
is preserved here: every non-`Status` request continues an existing
preservation sequence, and transition to rollback is possible only after a
fresh exact preservation-exhaustion proof.

## Prior-finding closure

### P1-1 — exact request/state authority: closed

`PreparedReverseEntryView` now stores the exact `V1LifecycleRequest`, lends it
through the visitor, and includes it in `ReverseEntryAuthorityPayload` beside
the entry kind and anticipated-model digest. The handoff, lane preflight, and
prepared entry all carry the same payload; the production constructors reject
any request, kind, source, or digest mismatch before an entry can reach the
reducer.

The transition-owned state matrix is closed:

- direct Preserve and Abort are available only from `Executing`,
  `AwaitingResolution`, `Halted`, and `Finalizing`;
- terminal, `RollingBack`, and `RecoveryRequired` states issue no direct-entry
  preview;
- `Preserving` maps every non-`Status` request to `ExhaustedRollback`; and
- the exhausted constructor still requires a source-bound
  `VerifiedPreservationExhausted`.

The preview continues to use the real participant reducer helpers
`record_outcome` and `abandon`; no anticipated-model policy is duplicated.
The focused matrix crosses all six requests and all operation states. A
production-constructor test proves that an Abort handoff/preflight cannot be
replayed against a Continue preview, while a consistently Continue-bound
entry succeeds only with fresh exhaustion. The existing dispatcher matrix
also proves that all five mutating/resume requests finish preservation before
rollback.

### P1-2 — exact F publication handoff: closed

F now owns one closed reverse-handoff classifier,
`publication::observe_reverse_handoff`. It returns exactly one of:

- `RecordEvidence`, when an exact live composition result must be durably
  recorded first;
- `NoCandidate` for an exact no-publication/pre-candidate form;
- `EvidencePending` for the exact not-started evidence form; or
- `Candidate { prefix, index }`, carrying the exact Baseline/Marker/Lock/
  Boundary file prefix and Pre/Staged index form.

The classifier reuses F's accepted-input, post-evidence, candidate snapshot,
index, and publication-resolution checks. Mixed or unrepresentable forms
return a typed recovery error rather than a token. There is no former Boolean
`recovery_origin_is_exact` handoff and no second classification whose result
is discarded.

`VerifiedPublicationHandoff` binds the closed fact together with source bytes,
request, kind, and anticipated digest. P1/P2 can invoke the same F observer
during their global preflight and issue their existing approved token from the
reobserved payload without widening a P0 file. Entry construction requires
handoff and preflight payload equality, so a change from Marker to Lock (or any
other exact class) rejects instead of silently transferring a different live
prefix. Focused tests cover evidence-first ordering, every durable publication
phase, all physical prefixes, staged completion, exact-prefix replay
rejection, and mixed-state rejection.

### P1-3 — visitor and result sealing: closed

The visitor seal now lives inside `authority::observe`, outside the reach of
runtime, service, physical, dispatcher, and transition consumers. The
non-cloneable inspection permit remains issuer-created and source-bound.

`SealedReverseEntryVisitor` has both:

- a private observer-subtree `Visitor` supertrait; and
- an associated result bounded by a second private `AuthorityResult` marker.

The result marker is implemented only for
`RecordEvidenceOr<VerifiedPublicationHandoff>`,
`VerifiedPreservationEntryPreflight`, and
`VerifiedRollbackEntryPreflight`. A model, projection, or caller-selected
payload cannot satisfy the current trait. A source gate permits visitor/marker
implementations only in the F handoff and the two declared P1/P2 preflight
files. Thus the later lanes can fill their predeclared observer skeletons
without exposing the anticipated record or editing the shared bridge.

### P1-4 — stable non-replaceable router: closed

The replaceable generic lane traits are gone. `ReverseRuntime<'a, B>` has no
consumer-supplied delegate type or constructor argument. Its exhaustive match
calls only the predeclared concrete module functions:

- `reverse::preservation::{observe}` and
  `reverse::execute::preservation::{execute}`;
- `reverse::rollback::{observe}` and
  `reverse::execute::rollback::{execute}`; and
- `archive::observe_open`.

P1, P2, and P4 can implement those owned skeleton functions without editing
the router or `v1_lifecycle/mod.rs`. The pure route selector keeps P0 routing
testable without production fake-lane authority. Its latest matrix rejects
wrong-request PreservationEntry, RollbackEntry, Archive, and `Status` cursor
combinations, delegates forward/finalizing recovery to X/F, and retains the
accepted all-non-`Status` cursor behavior for already-started preservation and
rollback.

### P1-5 — immutable R3 archive provenance: closed

The duplicate P0 cleanup model and arbitrary-data constructor are gone.
`ValidatedArchivedMerge` is constructible only from
`CanonicalArchiveAcquisition`, a non-cloneable P4-owned capability whose
fields and constructor authority remain private to `archive.rs`. The result
consumes that capability and retains its single `ValidatedArchivedRecord`;
projection and cleanup are borrowed through read-only getters from that R3
value.

R3's production-bearing decoder now owns the only projection/worklist
construction. `ValidatedArchivedRecord` fields and
`ArchivedCleanupWorklist`/owner fields are private, with read-only getters for
P3/P4. No lifecycle sibling can fabricate the acquisition or combine bytes
with an independently supplied projection/worklist. The focused v0/v1 test
constructs the handoff only by decoding the same exact bytes and verifies the
result digest, version, projection, and R3 cleanup view.

### P2-1 — manifest and ceilings: closed

The settled production set is the exact revised **29-path** manifest. The
delta is 1,205 additions and 25 deletions, net **+1,180 / 1,200**, with no
moved production LOC. The four additions beyond the accepted 25-path
intermediate manifest are F's existing publication classifier and R3's parent
module, archive decoder, and cleanup owner. They hold the policy/provenance
that P0 must reuse; they do not duplicate it.

The test/tool/doc manifest is also exact: 12 Rust evidence paths plus nine
interface/review documents when both code `-2` reviews are present, for
**21 paths**. This architecture memo is the eighth currently present document;
the companion state/restart `-2` memo is the sole reserved ninth document and
the only not-yet-present path. Before the two `-2` memos, the settled evidence
uses 3,511 added and 54 deleted test/tool/doc lines; the 4,200 ceiling leaves
ample reviewed capacity for both concise re-reviews. The package arithmetic is
consistent at 76 charged/63 unique production paths and 70 charged/69 unique
test-tool-doc paths.

## Verification

- reverse-entry request/state/lineage/visitor suite: **9 passed**;
- fixed router suite: **3 passed**;
- immutable archived-result suite: **1 passed**;
- F reverse-handoff evidence/prefix suite: **2 passed**;
- R3 archived projection/cleanup suite: **17 passed**;
- full `gwz-core` library suite: **940 passed, 1 ignored, 0 failed**;
- `cargo fmt --all -- --check`: passed;
- `cargo clippy -p gwz-core --lib --tests -- -D warnings`: passed; and
- `git diff --check` over the P0 source/doc tree: passed.

## Parallel-start decision

**GO from the architecture/ownership re-review.** Once the independent
state/restart `-2` review also reports GO, P1 preservation, P2 rollback, P3
status/protocol, and P4 archive/GC may start concurrently within their fixed
manifests. None requires a shared P0 interface edit to begin.
