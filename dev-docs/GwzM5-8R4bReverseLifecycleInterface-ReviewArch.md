# R4b-P reverse-lifecycle interface checkpoint — architecture review

## Review result

**Verdict: NO-GO.**

The checkpoint has no P0 finding and no independent P3 finding, but it has
three P1 findings and two P2 findings. P1–P4 are **not safe to start in
parallel** from the interface as
written. The archive/GC semantics themselves are substantially closed, but the
reverse-entry authority path, the field-10 ownership/sequencing decision, and
the shared-lane handoffs are not.

This review covered the proposed checkpoint against:

- `GwzM5-8R4bTransitionDesign.md`;
- `GwzM5-8I2ActionJournalContract.md`;
- `GwzM5-8I2ProtocolContract.md`;
- `GwzM5-8Refactor.md`;
- `GwzM5-8ChangeBudget.md`; and
- the current R4b-S/F/X, protocol, status, archive, GC, and Git-backend
  implementation seams.

The current tree was also checked with `gwz status` before and after review.

## Findings

### P1 — reverse-entry authority has no production construction path

The proposal says P consumes the accepted F interface and leaves the closed
dispatcher/reducer unchanged. The implementation does not presently provide
the interfaces necessary to do that:

- `VerifiedPublicationHandoff` has no production issuer.
- `PreparedPreservationEntry` and `PreparedRollbackEntry` have only
  `#[cfg(test)]` constructors.
- Those entries bind the hash of the anticipated post-outcome/action-free
  model. P has no pure production API that obtains that exact anticipated
  model from the same transition implementation that the reducer will use.
- `observe_finalization` accepts only `ParticipantsComplete`, `Acceptance`,
  and `Publication`; it rejects `PreservationEntry` and `RollbackEntry`.
- The accepted resolver nevertheless expects those entry observations to
  return either publication evidence that must first be recorded or a bound
  prepared entry.

Consequently P1/P2 cannot implement a conforming observer by consuming F as it
exists. They would have to reach into private authority internals, duplicate
F's publication/evidence policy, or independently reproduce transition logic
to predict the anticipated model. All three violate the accepted ownership
boundary and make the entry proof circular or forgeable.

**Required correction:** add and freeze an authority-owned production entry
interface before assigning P1/P2. It must include all of the following:

1. A pure, non-serializable reverse-entry preview produced by the transition
   owner from one of the exact predecessor forms: current action-free state,
   a verified participant outcome, or a verified participant-not-started
   proof. The preview must carry or expose the canonical anticipated-model
   digest used by the reducer; P must not rebuild the transition.
2. An F-owned request-neutral publication-handoff observer. It must either
   return the existing verified evidence result that the resolver records
   first, or return a bound `VerifiedPublicationHandoff`. It must reuse F's
   accepted evidence, prefix, candidate, and selected-root checks.
3. Authority-owned production constructors for preservation entry, direct
   rollback entry, and rollback-from-preserving entry. The final form must
   require `VerifiedPreservationExhausted`. Constructors must bind current
   checked bytes, request/observation identity, the transition-owned preview,
   and the F-owned handoff.

One suitable conceptual surface is:

```text
preview_reverse_entry(current, verified_predecessor)
    -> ModelResult<PreparedReverseEntryView>

observe_reverse_publication_handoff(backend, context, current, preview)
    -> ModelResult<RecordEvidenceOr<VerifiedPublicationHandoff>>

prepare_preservation_entry(current, preview, handoff)
prepare_direct_rollback_entry(current, preview, handoff)
prepare_exhausted_rollback_entry(current, preview, handoff,
                                 VerifiedPreservationExhausted)
```

The names are not normative; issuer ownership and binding are. This interface
must be implemented or accepted as a lead-owned prerequisite, not invented
independently by P1 and P2.

### P1 — field 10 belongs to R4b-P, but the proposal leaves it undecided and the budget forbids it

Section 8 leaves `MergeResponse` field 10 as a choice between R4b-P and A1.
That is not an implementation-ready interface decision. It also conflicts
with both accepted protocol ordering and the frozen R4b ledger:

- I2 already allocates optional field 10 `record` and freezes the nested
  projection, enum values, presence matrix, and Rust/Python/JSON/JSONL/human
  parity.
- `GwzM5-8Refactor.md` requires every v0/v1 archive fixture to project
  identically through core, CLI, Python/native, JSON, JSONL, retention, and GC
  before A1.
- Transition Design keeps A1 small: it activates writer/migration/dispatch
  reachability; it does not invent or finish lifecycle semantics.
- Change Budget says R4b has no normal-build behavior delta and no wire/protocol
  delta.
- The current Taut schema and generated Rust/Python types stop at field 9.
  Generated Rust encodes every optional key with `Null` when absent, so merely
  adding optional field 10 changes the normal CBOR response map, even before a
  record is populated.

The only sequencing consistent with those constraints is to implement and
verify field 10 in R4b-P (or in a separately named, reviewed checkpoint before
A1). It cannot be deferred as unreviewed work inside A1.

**Required correction:** make the direct decision that P3 owns the I2 field-10
schema, generated bindings, constructors/projector, and every driver renderer
and fixture. Amend the R4b budget/ledger to record this intentional append-only
wire and behavior delta. A1 must then do only reachability activation.

The corrected P3 contract must enumerate the entire I2 presence matrix rather
than only status:

- include `record` for start after record creation, open or archived status,
  continue, preserve-abort, abort, and id-qualified GC successes;
- omit it for dry-run, idle, pre-record failure, and unqualified GC;
- project open v0, open v1, archived v0, and archived v1 through one semantic
  projection owner;
- preserve exact source version, acceptance, recovery origin, terminal
  outcome, participant rows, preservation rows, and publication data; and
- render every frozen key in JSON/JSONL, including explicit `null` and empty
  arrays, with Rust/Python/native/human parity.

The present draft describes open-v1 and archived-v0/v1 status, but does not
close open-v0 projection and response population for the other durable v0
successes. That omission must be corrected explicitly. The existing central
v0 `MergeOperationRecord::to_response` call path is the natural adapter; P3
must own it rather than leaving each command to synthesize field 10.

### P1 — the proposed parallel lanes share unfrozen implementation choke points

The draft designates one `ReverseRuntime` as the sole P implementation of the
observer/executor traits, yet P1 owns preservation, P2 owns rollback/recovery,
and P4 needs archive observation. All three therefore need the same router and
authority exports. The draft says P1/P2 may share only lead-frozen helpers, but
does not actually define their signatures or assign their implementation.

There is a second collision between P3 and P4. P4 returns an archived response
derived from the reopened destination, while P3 owns protocol projection. No
frozen type says where validated archive decoding ends and response mapping
begins. P4 also claims to consume only archive bytes after archival even though
source-only and both-copy paths must first obtain the service's `Archive`
observation before invoking S's primitive.

**Required correction:** land or freeze a lead-owned integration skeleton
before parallel work:

- The shared dispatch/router file contains only stable routing and delegates
  to exact lane-owned free functions. P1, P2, and P4 must not independently
  edit the same dispatch match. Separating `ReverseRuntime` and
  `ArchiveRuntime` is acceptable and likely clearer.
- The authority exports from the first finding are lead-owned prerequisites.
- P4 returns a protocol-neutral validated archive result containing the exact
  destination bytes/digest plus the already validated immutable projection
  and cleanup worklist. P3 alone maps that value to
  `MergeRecordProjection`/`MergeResponse`.
- P4's archive wrapper explicitly obtains and resolves the `Archive`
  observation for source-only/both-copy cases, calls S's exact-byte archive
  primitive, and then reopens and validates the destination. Destination-only
  status/response performs only the final reopen/decode step.
- Each shared file has one owner and one merge point listed in the plan.

Until these boundaries exist, P1–P4 are not independently implementable and
parallel work would create ownership conflicts rather than merely integration
work.

### P2 — the frozen R4b-P package budget is not credible for the frozen scope

The 1,450 net production / 2,600 test-tool-doc / 22 production-path / 18
test-tool-doc-path ceiling predates the concrete field-10 and reverse-entry
closure exposed by this review. It is inconsistent with measured neighboring
work and the number of mandatory owners:

- accepted R4b-F used 1,989 production-bearing lines across 17 paths and 1,681
  test lines for finalization alone;
- accepted R4b-X used 987 production-bearing lines across 14 paths and 1,426
  test lines;
- the v1 lifecycle module, v1 model/archive projection, and v0/v1 archive
  decoder/worklist are currently test-compiled, so P must pay for their
  intended production interface and wiring rather than treating those owners
  as already production-integrated;
- the existing preservation implementation is already roughly 1,276 lines
  across its three principal owners before extraction, v1 observer/executor,
  recovery, status, archive, GC, or protocol work;
- field 10 necessarily reaches the Taut source, generated Rust and Python
  bindings, golden/corpus fixtures, core response construction, Rust CLI, and
  Python/native renderers; and
- generated LOC is excluded, but `GwzM5-8ChangeBudget.md` explicitly counts
  the schema change and generated file paths. Shared files are charged to each
  checkpoint that changes them.

The current path ceilings alone leave no defensible room for the named P
modules, narrow v0 extractions, authority/handoff closure, Git-backend facts,
archive/status adapters, schema/codegen, both driver stacks, fixtures, and the
required fault/restart matrices. The 2,600 test allowance is only about 1.55x
F's focused test count even though P covers four large physical/projection
domains and a cross-language protocol matrix.

**Required correction:** unfreeze the P ceiling and perform a measured
rebaseline after the entry APIs and field-10 file manifest are fixed. Add a
named protocol/projection slice to the ledger. The revised ledger must list
every production and test/tool/doc path, count schema and generated paths,
charge shared paths to each modifying slice, and separate semantic additions
from v0 movement. If the lead wants to retain the present numeric ceiling, it
must first provide a path-level implementation manifest showing how all frozen
contracts fit; the present allocation is not such a manifest.

No replacement number is approved by this review: a speculative larger
ceiling would hide the same uncertainty. The stop gate should be a measured
lead rebaseline followed by independent review before implementation lanes
start.

### P2 — the proposed Git fact seam erases the repository's error and identity types

The preservation fact vocabulary is directionally correct and appears capable
of supporting the I2 raw-worktree and stash grammar. Its conceptual signatures
are nevertheless weaker than the existing `GitBackend` contract: they omit
`ModelResult` and use a bare string for object identity.

That ambiguity matters because a semantic mismatch (for example, image bytes
do not match the journal) must remain distinguishable from a backend/read
failure; the latter cannot be collapsed into drift or a false observation.
Likewise a stash object is a typed Git object identity, not arbitrary display
text.

**Required correction:** freeze the new backend methods as fallible
`ModelResult<...>` operations, preserve the existing operational error codes,
and use the repository's canonical typed object-id/digest representation (or
an explicitly validated algorithm-plus-hex value). State that the observer,
not the backend, owns comparison with journal semantics, while the backend owns
exact byte/object acquisition.

## Accepted portions and non-findings

The following parts are suitable foundations for the corrected checkpoint:

- Status remains read-only and lease-free; mutating archive and GC retain the
  workspace-mutator lease boundary.
- S remains the sole exact terminal-byte archive primitive. P owns request and
  state dispatch into it, destination reopen, and response/GC consumption.
- Archive validation yields an immutable projection plus immutable cleanup
  worklist. GC rereads the same archive before final deletion and never rewrites
  it.
- Stashes and bundles are retained. Only validated merge-owned backup refs are
  candidates for deletion.
- Source-only, both-copy, destination-only, mismatch, nonterminal, symlink,
  partial-crash, unreadable, and unsupported-future cases are present in the
  proposed test obligations.
- The proposal's rule that targeted GC must hold the mutator lock and reject an
  open merge matches current behavior. `OpenMergeGate` allowing GC dispatch is
  not contrary evidence: `merge/gc.rs` performs the no-open check after taking
  the lock. There is no finding on this point.
- Keeping the v1 decoder, writer, migration, lifecycle dispatch, and mutable v1
  record view unreachable in a normal production build until A1 is correct.

## Required amended dependency and ownership order

The checkpoint may return to implementation only after a lead revision closes
the three P1 findings and the budget is re-reviewed. A safe sequence is:

1. **Lead-owned interface prerequisite (not an implementation lane):**
   freeze/land reverse preview, F handoff, production entry constructors,
   router skeleton, P4-to-P3 archive result, and the complete owner/path
   manifest.
2. **Protocol decision and ledger amendment:** assign field 10 to P3, record the
   append-only protocol delta, enumerate the v0/v1 presence matrix, regenerate
   a mechanical spike if necessary, and approve a measured P budget.
3. **P1 and P2:** preservation and rollback/recovery may proceed in parallel
   once they consume, rather than widen, the shared prerequisite APIs.
4. **P3:** status and protocol projection may proceed after the field-10
   amendment. Its projection type remains independent of the reverse runtime.
5. **P4:** archive/GC may proceed after the archive-observation router and
   protocol-neutral reopened-destination handoff are frozen.
6. **Integration:** merge through the single owned router, run the full
   cross-driver protocol and archive/GC matrices, then perform settled-tree
   review before R4b-G.

This does not require A1 to move earlier. It preserves the intended boundary:
R4b-P completes the semantics and protocol projection behind the disabled v1
writer/migration boundary; A1 later changes reachability only.

## Closure criteria for re-review

The next architecture review can return GO only when the revised checkpoint:

- names production issuers for every reverse-entry authority token and shows
  that anticipated-model binding comes from the transition owner;
- names the F-owned evidence-or-handoff observer consumed by P;
- resolves field 10 to P (or to a separately accepted pre-A1 checkpoint),
  amends the no-wire/no-behavior ledger language, and closes every I2
  projection/presence case including open v0;
- assigns one owner to the router and one protocol-neutral handoff between P4
  and P3;
- publishes an enumerated, measured budget/path manifest that fits all frozen
  obligations; and
- keeps all writer/migration/v1 mutation reachability disabled until A1.

Until then, the exact parallelism decision is: **P1–P4 NO-GO; do not start the
four implementation lanes.**
