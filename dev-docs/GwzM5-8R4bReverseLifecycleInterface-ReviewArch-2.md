# R4b-P reverse-lifecycle interface checkpoint — architecture re-review

Date: 2026-08-10

## Review result

**Verdict: GO.**

The remediated interface closes every P1/P2 finding from
`GwzM5-8R4bReverseLifecycleInterface-ReviewArch.md`. This re-review finds no
open P0, P1, P2, or P3 issue in the architecture, ownership, dependency,
protocol, version-isolation, archive/GC-authority, or package-budget boundary.

This is GO for the documented sequence, not permission to bypass it. The
lead-owned P0 implementation must land, pass its focused tests, and receive the
two required code/interface re-reviews before P1-P4 begin. After that P0 gate,
P1-P4 are safe to proceed concurrently within their fixed manifests and sealed
interfaces.

## Review basis

The current revision was checked against:

- `GwzM5-8R4bTransitionDesign.md`;
- `GwzM5-8I2ActionJournalContract.md`;
- `GwzM5-8I2ProtocolContract.md`;
- `GwzM5-8Refactor.md`;
- `GwzM5-8ChangeBudget.md`;
- the first architecture review; and
- the current transition authority/resolver, F/X observers, checked service
  and store, R3 archive projection/worklist, v0 status/GC/retention, protocol
  generation, and Rust/Python rendering seams.

The workspace was inspected with `gwz status`; this review changes only this
memo.

## Closure of the first review

### Reverse-entry authority and dependency closure — resolved

The revised P0 surface now provides the missing production authority chain:

1. `preview_reverse_entry(current, request, predecessor)` is transition-owned
   and applies the exact predecessor effect in memory. Its opaque result binds
   source digest, predecessor identity, request-specific entry kind, and the
   canonical anticipated action-free model digest.
2. F owns `observe_reverse_publication_handoff`. It reuses F's accepted
   participant, candidate, evidence, prefix, selected-root, and
   no-publication checks and returns either the existing evidence result to be
   recorded first or a sealed `VerifiedPublicationHandoff`.
3. P1 and P2 issue distinct sealed, digest-bound global-preflight authorities.
   The preservation and rollback preflights evaluate their complete
   all-member/root/artifact contracts against the model held by the opaque
   preview. No boolean or caller-constructed payload is accepted.
4. P0's three entry constructors consume the matching preview, handoff, and
   lane-specific preflight authority. Rollback from preserving additionally
   consumes `VerifiedPreservationExhausted`; its direct and exhausted forms
   cannot be interchanged.
5. The reducer remains the final verifier of the same anticipated-model
   digest. P1/P2 neither construct a replacement record nor duplicate the
   predecessor transition.

This removes the former circular choice between private reach-in, duplicated
F policy, and consumer-side reducer prediction. It also preserves the accepted
resolver behavior in which a discovered evidence result is durably recorded
before fresh entry observation.

The constructibility bridge is closed rather than implicit. The transition
owner lends the anticipated model only during
`visit_reverse_entry(ReverseEntryInspectionPermit, ...)`; the permit is
non-cloneable and constructible only by `AuthorityIssuer`, and the visitor
trait is sealed to the F/P1/P2 authority-observer modules. The borrow cannot
escape and the only return is a sealed handoff/preflight authority. Runtime,
service, and physical modules can neither acquire the permit nor obtain a
record/projection. This gives the real preflight observers the facts they need
without weakening preview opacity.

### P0 router and parallel ownership — resolved

P0 now owns sealed `PreservationLane`, `RollbackLane`, and `ArchiveLane`
interfaces plus the stable exhaustive router. It can compile and be tested
before the lanes because its tests use fake sealed implementations and it
predeclares compiling lane skeletons. Ownership of each skeleton is then
transferred once to its named lane after P0 review.

The router's only additional dispatcher dependency is a crate-v1-visible,
read-only accessor for the lifecycle request already sealed inside
`BoundObservationRequest`. Charging `authority/dispatcher.rs` to P0 is exact:
the accessor exposes no constructor or mutable authority, and lets the router
select Abort versus Preserve without duplicating request/state policy.

The central match, `v1_lifecycle/mod.rs`, and aggregate dispatch paths remain
lead-owned. P1/P2 implement only their respective lane traits. P3 does not
import the reverse runtime. P4 implements only the archive lane and cannot map
a protocol response. No concurrent lane owns the same module-declaration or
router edit.

The archive-to-protocol boundary is also explicit. P4 returns only
`ValidatedArchivedMerge`, constructed after canonical no-follow destination
acquisition and R3 decode. It carries immutable destination bytes/digest,
source version, archive projection, and cleanup worklist. P3 alone maps that
projection to field 10. Source-only and both-copy archive requests obtain the
bound `Archive` observation and call S's exact-byte primitive; destination-only
recovery performs only destination acquisition and decode.

The resulting parallel-start rule is coherent:

- P0 lands and is accepted first;
- P1 preservation/preserving recovery and P2 rollback/rolling-back recovery
  may then proceed independently;
- P3 status/protocol and P4 archive/GC may proceed beside them through the
  immutable archived-result handoff; and
- only the named lead-owned integration paths are merged after lane files
  settle.

### Field-10 ownership and A1 sequencing — resolved

P3 unambiguously owns the accepted I2 append before A1:

- Taut field 10 and the frozen nested types/discriminants;
- generated Rust/Python bindings and protocol conversion;
- one semantic projection owner for open v0, open v1, archived v0, and
  archived v1;
- response population for start after durable creation, open/archived status,
  continue, preserve-abort, abort, and id-qualified GC successes;
- omission for dry-run, idle status, pre-record failure, unqualified GC, and
  top-level failures; and
- Rust, Python/native, JSON, JSONL, and human rendering plus corpus/parity
  fixtures.

This closes the former open-v0 gap as well as the R4b-P/A1 ambiguity. The
parent refactor now makes the complete field-10 matrix an A1 prerequisite, and
A1 remains only the small writer/migration/no-ff dispatch activation.

The change-budget ledger correctly declares the append-only wire delta and its
normal-build durable-v0 response effect. It no longer claims that R4b has no
protocol delta.

### Git observation seam — resolved

The preservation seam now preserves the repository's error and identity
types. Its methods return `ModelResult`; preimage digests use `Sha256Digest`;
stash identity and HEAD use `GitObjectId`; and stash evidence includes the
decoded `GitPreservationImage`, including the dirty summary needed to rebuild
and verify canonical bundle rows after restart.

The Git implementation owns exact raw/non-UTF-8-safe byte acquisition and
forbidden-index-form rejection. Authority observers own semantic
before/after/ambiguous classification. Operational acquisition errors remain
typed errors and cannot become false mismatches.

### Archive, retention, and GC authority — resolved

The archive/GC design has one immutable authority source and no history
rewrite:

- archive acquisition proves canonical real parent directories, a validated
  merge-id leaf, and a regular non-symlink file before decode;
- R3 decoding produces the immutable projection and cleanup worklist;
- targeted GC holds the workspace mutator lock and rejects any open merge;
- every path/name/owner/target and every repository is fully preflighted before
  the first ref deletion;
- absent refs are restart-complete, exact targets alone authorize checked
  deletion, and mismatch/unavailable repositories retain the archive;
- final deletion requires a no-follow, byte-identical reread, identical
  redecoding, and observation that every owned ref is absent; and
- native stashes/bundles survive, unreadable and unsupported-future archives
  are retained, and ordinary retention exempts ref-owning archives.

The ledger now declares these normal-build v0 no-follow/worklist/retention
changes, SHA-256 support, and stash/bundle-only rows rather than describing
field 10 as the sole production behavior delta. Archived-v1 projection and GC
are compiled and tested, but no production-created v1 record can reach them
before A1.

### Status and version isolation — resolved

Status remains read-only and lease-free. Its open-source snapshot is bound by
an unchanged-source reread after live observation, preventing one response
from combining different durable digests. Its open/archive arbitration matrix
covers absent, terminal, identical-both-copy, malformed/different,
nonterminal-plus-destination, and noncanonical/symlink forms. Archive-only
status uses no live repository state.

The normal build still has no reachable v1 creator, migration dispatcher,
mutable v1 lifecycle route, generic v1 writer, or v1-to-v0 owned-record
conversion. R4b completes semantics and read/projection support behind the
disabled mutation boundary; A1 alone changes reachability.

## Budget and manifest verification

The original 1,450/1,600/2,600 ceiling is withdrawn. The replacement is
consistent across the interface and `GwzM5-8ChangeBudget.md`:

| Charge | Production LOC | Moved LOC | Test/tool/doc LOC | Production paths | Test/tool/doc paths |
| --- | ---: | ---: | ---: | ---: | ---: |
| P0 | 900 | 0 | 3,000 | 23 | 8 |
| P1 | 950 | 1,050 | 1,800 | 9 | 8 |
| P2 | 900 | 650 | 1,800 | 9 | 8 |
| P3 | 900 | 250 | 2,500 | 18 | 13 |
| P4 | 750 | 100 | 1,500 | 8 | 8 |
| aggregate | 100 | 0 | 1,500 | 3 | 12 |
| **Total** | **4,500** | **2,050** | **12,100** | **70 charged** | **57 charged** |

The production manifest expands to those exact charged counts. Accounting for
the explicit P0-to-lane skeleton handoffs and lead-owned integration overlap
produces the stated 59 unique production paths. The exact test/tool/doc list
expands to 57 paths: 8 P0, 8 P1, 8 P2, 13 P3, 8 P4, and 12 aggregate. It names
the interface and both review rounds, protocol corpus/parity files, focused
lane matrices, current-v0 characterization, integration/fault owners, parent
document amendments, and document-consistency checks.

The ceilings are materially credible for the frozen scope: they are based on
the current 1,276-line preservation owners and measured accepted F/X packages;
they separately charge code movement; generated LOC is excluded while the
schema and generated paths remain charged; and every owner must stop for lead
and independent review rather than silently add a path or exceed a slice.

The aggregate parent-ledger ceilings were correspondingly rebased to 12,400
production-bearing and 23,800 test/tool/doc lines, 111 unique production paths,
and 113 unique test/tool/doc paths. The arithmetic and replacement of the old
P allowance are consistent.

## Remaining gates, not findings

The following are mandatory implementation gates already present in the
accepted interface; none is deferred architecture work:

- implement P0 exactly within its sealed surface and obtain both P0 code
  re-reviews before opening P1-P4;
- stop if a lane needs a new durable field/phase/transition, generic writer,
  proof constructor, store mutation entry point, production v1 mutation route,
  or another production path;
- execute the full preservation, rollback, recovery, status/protocol,
  archive/restart, and partial-GC matrices within the enumerated manifests;
- retain source/call-graph gates proving the disabled v1 writer/migration
  boundary; and
- perform aggregate settled-tree review before R4b-G and A1.

Subject to those existing gates, the remediated checkpoint is
**architecture/interface GO with no P0-P3 finding**.
