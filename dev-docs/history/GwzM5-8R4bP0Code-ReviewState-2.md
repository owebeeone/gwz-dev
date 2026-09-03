# R4b-P0 settled-tree code re-review — state, restart, and authority

Date: 2026-08-10

Reviewer: independent state-machine code re-review

## Verdict

**GO.**

The remediated P0 closes P1-1 through P1-3 and P2-1 through P2-2 from
`GwzM5-8R4bP0Code-ReviewState.md`. This re-review finds **no open P0, P1, P2,
or P3 finding** in the current state/restart behavior, authority chain,
executable matrices, archive provenance, manifest, or stop ceilings.

P1 preservation, P2 rollback, P3 status/protocol, and P4 archive/GC may start
within the accepted interface and fixed manifests.

## Review basis

The current uncommitted P0 tree was checked directly against the accepted
reverse-lifecycle interface, transition design, both accepted interface `-2`
reviews, the first state/restart code review, and the revised budget ledger.
The review traced preview through handoff/preflight/entry, finalization
publication classification, router and physical closure, and R3 archive
provenance. It also rechecked every changed P0 path and the three amended
aggregate parent documents.

## Prior-finding closure

### P1-1 — exact request, state, and predecessor authority: closed

`PreparedReverseEntryView` and `ReverseEntryAuthorityPayload` retain the exact
request beside kind and anticipated-model digest. F handoff, lane preflight,
and prepared entry carry and compare that identity; production constructors
reject cross-request, cross-kind, wrong-digest, stale-handoff, stale-preflight,
and stale-prepared-entry reuse.

Direct Preserve/Abort previews are limited to `Executing`,
`AwaitingResolution`, `Halted`, and `Finalizing`. `Preserving` retains every
non-`Status` request as `ExhaustedRollback`, still requiring fresh exact
preservation-exhaustion proof. Terminal, rolling-back, recovery, and retained
forward-owner action-free forms reject before authority is issued. The preview
continues to use the transition-owned participant predecessor helpers.

### P1-2 — one exact F publication handoff: closed

F now performs one `observe_reverse_handoff` classification and returns only
exact `RecordEvidence`, `NoCandidate`, `EvidencePending`, or
`Candidate { prefix, index }`. The candidate fact distinguishes Baseline,
Marker, Lock, and Boundary with Pre/Staged index form and is part of the sealed
payload consumed by preflight and entry construction.

The executable evidence covers every durable publication phase, evidence-first
ordering, candidate-bearing preparation, all ordinary prefixes, Boundary/
Staged completion, and the degenerate terminal Marker/Staged form. A changed
exact prefix cannot be paired with an earlier handoff, and mixed or
unrepresentable file/index forms reject.

### P1-3 — same-byte R3 archive provenance: closed

`ValidatedArchivedMerge` consumes only non-cloneable
`CanonicalArchiveAcquisition`. The P4-owned capability keeps canonical
destination bytes with the single `ValidatedArchivedRecord` decoded from those
same bytes; its fields and construction authority remain private. The result
derives its version and digest internally and exposes only read-only R3
projection and cleanup views. No shadow cleanup model or unrelated-value
constructor remains. Real valid v0/v1 fixtures prove the same-byte path.

### P2-1 — executable negative and closed matrices: closed

The reverse-entry suite now executes all nine operation states, six requests,
and all three predecessor forms, including `ParticipantNotStarted`, and proves
actual cross-kind constructor rejection. It separately covers cross-request,
wrong-digest, stale-authority, retained-owner, and visitor-source-gate cases.

The concrete observation router covers participant, preservation entry/cursor,
rollback entry/cursor, every forward and reverse recovery origin, archive,
missing recovery context, wrong request, and both `Status` cursor rejections.
The physical router admits only preservation and rollback after exact lease
location and rejects participant, publication, archive, and wrong-location
execution. The R3 archive tests and F tests close the remaining provenance and
publication rows.

### P2-2 — manifest and ceilings: closed

The production set is the exact **29-path** manifest: 1,205 additions and 25
deletions, net **+1,180 / 1,200**, with no moved production LOC. The exact P0
test/tool/doc charge is **21 paths** when this memo is present. Before this
memo, the 21-path evidence plus the three conservatively counted aggregate
parent amendments uses 4,008 added lines. This 112-line memo brings the final
total to **4,120 / 4,200**. The package totals remain 76 charged/63 unique
production paths and 70 charged/69 unique test/tool/doc paths.

## Verification

- reverse-entry authority/matrix suite: **9 passed**;
- exact F reverse-handoff suite: **2 passed**;
- closed reverse router: **3 passed**;
- archived-result provenance: **1 passed**;
- R3 archive decoder/projection/cleanup: **17 passed**;
- serialized full `gwz-core`: library **940 passed, 1 ignored**, plus
  integration groups of **10, 27, 4, and 2 passed**;
- `cargo fmt --all -- --check`: passed;
- strict all-target/all-feature Clippy with warnings denied: passed; and
- root and `gwz-core` diff hygiene: passed.

## Parallel-start decision

**GO from the state/restart/authority re-review.** Together with the companion
architecture/ownership GO, the P0 two-review gate is satisfied. No shared P0
interface widening is required before P1-P4 begin.
