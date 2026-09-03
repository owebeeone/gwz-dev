# R4b-P0 settled-tree code review — architecture and ownership

Date: 2026-08-10

## Verdict

**NO-GO.**

P0 is not yet a safe shared prerequisite for P1–P4. There are five P1
interface/authority findings and one P2 manifest finding. P1–P4 would have to
edit P0-owned shared interfaces to correct them, so the parallel lanes must not
start.

No P0 or P3 finding was identified. The implementation-discovered addition of
`transition/reduce/{mod,participant}.rs` to the P0 production manifest is
accepted: the three changed visibility lines expose the existing
`record_outcome` and `abandon` reducer helpers only to the transition owner,
and `transition/reverse_entry.rs` calls those exact helpers rather than copying
their policy. The corrected 25-path, 72-charged/61-unique production accounting
is coherent and adds no aggregate-unique path because both files were already
owned by TR.

## P1 findings

### P1-1 — Reverse-entry authority does not bind the exact request/state

`transition/reverse_entry.rs:26-34` stores the source identity, entry kind,
anticipated digest, and model, but not the lifecycle request. The corresponding
`ReverseEntryAuthorityPayload` at `authority.rs:108-119` likewise contains only
the kind and anticipated digest. More immediately,
`transition/reverse_entry.rs:118-134` maps a `Preserving` record to
`ExhaustedRollback` for **every** request except `Status`. A `ResumeStart`,
`Continue`, `Preserve`, or `Archive` preview can therefore mint the same
preview/handoff/preflight shape as `Abort`; none of the entry constructors can
distinguish their provenance.

The action-free path also accepts `Abort`/`Preserve` previews in operation
states that are not legal predecessors of `BeginRollback`/`BeginPreservation`.
The reducer will eventually reject the transition, but the transition-owned
preview and lane preflights have already issued authority for a non-predecessor.
That is weaker than the accepted request-specific, exact-predecessor contract.

Required correction:

- bind `V1LifecycleRequest` in the preview and every reverse-entry authority;
- enforce the closed state/request matrix in `preview_reverse_entry`: Preserve
  only from `Executing`, `AwaitingResolution`, `Halted`, or `Finalizing`;
  direct Abort from those same states; exhausted Abort only from `Preserving`;
  reject every other pair; and
- add all-request negative tests for `Preserving`, illegal-state tests, and a
  cross-request replay test that reaches the production entry constructor.

### P1-2 — The F handoff proves a Boolean, not the publication fact it hands off

`authority/observe/finalization/handoff.rs:61-73` preserves the required
evidence ordering for one case: an exact unrecorded `EvidenceResult` is returned
for durable recording before entry. For every other publication observation,
however, it discards `publication::observe`'s result, calls
`recovery_origin_is_exact` for a Boolean, and issues the same token.
`handoff.rs:77-92` and `authority.rs:108-119` bind no publication classification
at all.

Consequently, baseline, a not-started evidence action, every exact
`PublishingCandidate` prefix/index form, and publication-complete state all
produce indistinguishable handoffs. In particular, the accepted transition
design requires the exact partial candidate prefix to be bound so the
preservation/rollback journal can own it. A change from one exact prefix to
another between observation, handoff, preflight, and entry is not detectable
from this token. The second read in `recovery_origin_is_exact` also means the
fact used for `publication::observe` need not be the fact for which the token is
issued.

Required correction: make F return and bind a closed publication-handoff fact
(at minimum exact no-effect/pre-publication versus the exact candidate
prefix/index and any other supported completed shape), while retaining
`RecordEvidence` as the only result when evidence must first be persisted.
The prepared entry must validate that bound fact rather than merely equating
two `{kind, anticipated_digest}` payloads. Add focused finalizing tests for
every durable publication phase, each partial prefix, evidence-first ordering,
mixed shapes, and stale/different exact-prefix replay.

### P1-3 — The anticipated-model visitor bridge is not sealed to approved outputs

The accepted bridge is safe only if F/P1/P2 observers are the sole implementers
and the result is one of the approved sealed authority types. The code does not
enforce either half:

- `authority.rs:194-196` exposes the marker to the whole `authority` subtree,
  not just the F, P1, and P2 observer implementations; every authority child
  can also reach `AuthorityIssuer::for_observer` and the private permit issuer;
  and
- `authority.rs:198-207` leaves `type SealedAuthority` unconstrained. An allowed
  implementer can select an arbitrary owned result, including a clone or other
  projection of the anticipated record lent to `inspect`.

This makes the accepted claim that the borrow can return only a sealed
handoff/preflight authority a convention rather than a compiler-enforced
boundary.

Required correction: centralize/narrow marker implementations to the named
F/P1/P2 visitor types and bound the associated result by a private result
marker implemented only for `RecordEvidenceOr<VerifiedPublicationHandoff>`,
`VerifiedPreservationEntryPreflight`, and
`VerifiedRollbackEntryPreflight` (or an equivalently closed result enum). Add a
compile/source gate proving runtime, dispatcher, service, and arbitrary
authority modules cannot implement the bridge or return a model/projection.

### P1-4 — The stable router lane traits are not sealed

`reverse.rs:16-60` declares ordinary `pub(super)` traits with no private
supertrait or capability witness. Any `v1_lifecycle` sibling can implement any
of `PreservationLane`, `RollbackLane`, or `ArchiveLane`. The direct fake
implementations in `tests/reverse_router.rs` demonstrate that no seal exists.
This does not provide the compiler-enforced P1/P2/P4 ownership on which the
accepted parallel-start decision depends.

Required correction: install production-only private seals (or P0-owned
concrete wrappers) whose production implementations are pre-authorized only
for the declared preservation, rollback, and archive lane types. A test-only
fake hook may remain under `cfg(test)`. P1, P2, and P4 must be able to fill their
existing skeletons without editing `reverse.rs`, `v1_lifecycle/mod.rs`, or
another lane.

### P1-5 — `ValidatedArchivedMerge` is freely fabricated and loses R3 provenance

`archive_result.rs:29-44` exposes `ValidatedArchivedMerge::new` to every
`v1_lifecycle` sibling and accepts unrelated caller-supplied bytes, projection,
and cleanup values. It performs no canonical no-follow destination acquisition
and no R3 decode. `archive_result.rs:6-18` also defines a second cleanup model
instead of consuming R3's existing decoder-produced
`ValidatedArchivedRecord`/`ArchivedCleanupWorklist`. The test at
`tests/archive_result.rs:12-28` constructs the result from hand-built values,
which proves getter immutability but disproves the required validation
provenance.

As written, P3 can fabricate the object it is supposed only to map, and P4 can
combine bytes with a projection/worklist derived from different bytes. The
private fields prevent mutation after construction but do not make the value a
validated archive handoff.

Required correction: construct the result only from a P4-owned opaque
canonical-destination acquisition plus the R3 decoder's single validated
result for those exact bytes. Preserve the R3 cleanup type rather than copying
it into an independently constructible model. The constructor capability must
be unavailable to P3 and other lifecycle siblings. Tests must start with real
archive bytes, canonical acquisition evidence, and R3 decode, and must reject
cross-byte/projection/worklist composition.

## P2 finding

### P2-1 — P0 changes six test paths outside its exact manifest

The corrected production manifest is exact: 25 production paths are touched.
The exact P0 test/tool/doc manifest is not. In addition to the five interface
documents and three named new tests, the implementation modifies these six
existing test paths:

- `v1_lifecycle/tests/mod.rs`;
- `v1_lifecycle/tests/authority.rs`;
- `v1_lifecycle/tests/dispatcher_attempt_matrix.rs`;
- `v1_lifecycle/tests/dispatcher_reconciliation.rs`;
- `v1_lifecycle/tests/predecessor_matrix.rs`; and
- `v1_lifecycle/tests/reducer.rs`.

P0 therefore touches 14 test/tool/doc paths, not the frozen 8. The LOC ceiling
still fits: the five interface documents plus three new tests total 2,405
lines, and the six existing tests add 89/delete 48 lines (2,494 additions in
all, below 3,000). Path charging is nevertheless a controlling stop gate and
shared paths are charged to every slice that changes them.

Required correction: either avoid those six edits while retaining compiling
coverage, or explicitly review and reconcile the P0 and R4b-P test path
manifest/totals before acceptance. `tests/mod.rs` is intrinsically needed to
compile the three new modules, so a documented correction is likely the honest
resolution.

## Budget and verification evidence

- Production delta: 864 additions, 9 deletions, net **+855 / 900**; no moved
  production LOC.
- Production paths: **25 / 25** under the corrected manifest.
- The reducer visibility correction is narrow and architecture-preserving.
- P0 focused tests: **7 passed** (4 reverse-entry, 2 reverse-router, 1 archived
  result).
- Full `gwz-core` library suite: **933 passed, 0 failed**.
- `cargo fmt --all -- --check`: passed.
- `cargo clippy -p gwz-core --lib --tests -- -D warnings`: passed.

Those green checks establish build/regression health, but they do not close the
authority and provenance findings above; several focused tests currently
exercise the overly broad construction surfaces rather than proving they are
unavailable.

## Parallel-start decision

**NO-GO for P1–P4.** P0 must first correct P1-1 through P1-5, reconcile P2-1,
extend the focused negative evidence, rerun the full gates, and receive both
required settled-tree re-reviews. Until then, preservation/rollback preflight,
publication-prefix ownership, lane implementation, status/archive projection,
and archive orchestration would all depend on later edits to P0-owned shared
interfaces.
