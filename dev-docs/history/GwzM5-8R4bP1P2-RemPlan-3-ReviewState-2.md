# R4b-P P1/P2 third remediation-plan focused state re-review

Date: 2026-08-12

Reviewed:

- revised `GwzM5-8R4bP1P2-RemPlan-3.md`;
- `GwzM5-8R4bP1P2-RemPlan-3-ReviewState.md`;
- the exact settled `ReviewArch-3` and `ReviewFaults-3` findings;
- the frozen I2 action-journal, R4b transition, reverse-lifecycle, and both
  interface-amendment contracts; and
- the settled rollback cursor, dispatcher, observers, recovery verifier,
  executor, selected-root helpers, and current matrix evidence at workspace /
  core commits `1c6bbba13586cb8be50576301072b38f0e0a3463` /
  `7d1a3e6a33cf2e4b5d882db1f7196f16ea7cc87d`.

This was a read-only focused re-review. I did not modify the plan or any
production/test code.

## Verdict

**NO-GO for implementation.**

All five P2 findings from my first state review are materially closed in the
revised architecture, and the cursor-relative projection is implementable
without a v1 wire or durable-phase change. One new P2 matrix omission remains:
ordinary physical rows still lack the frozen executor-attempt cross. The plan
now declares observation alphabets and exact aggregate/call assertions, but it
does not generate `Success` and `Failed` attempts against fresh exact-before,
exact-after, and ambiguous observations for every physical preservation and
rollback action. That executor-result non-authority contract is part of the
accepted R4b service matrix and was one of C7's reasons for requiring generated
rather than fixture-derived evidence.

There is no P0 or P1 finding and no separate P3 finding.

## Prior-finding closure audit

| Prior finding | Result | Evidence in the revision |
| --- | --- | --- |
| P2-1 complete aggregate and shared-root supersession | **Closed** | §3.5 now declares a closed aggregate position vocabulary, retains publication evidence as live prefix authority, assigns root ref/HEAD, checkout, boundary/marker, metadata, and index facts by cursor position, and explicitly transfers shared-root authority from evidence to selected-root participant to metadata. Domain-specific overlays are legal only when another exact aggregate projector proves the excluded fact; reserved trees are not erased. |
| P2-2 representable versus action-free recovery | **Closed** | §3.5 distinguishes entry/action-free no-rewrite errors from pending-action recovery, leaves executor failure diagnostic-only, and adds the complete aggregate plus current action to rolling-back recovery-origin verification. This matches the frozen rule that `RecoveryRequired(origin=RollingBack)` retains an exact pending journal. |
| P2-3 legal handoff/request/no-op/action-free rows | **Closed** | §3.6 separately declares `NoCandidate` and `EvidencePending`, all six candidate forms including conditional `Marker/Staged`, all five admitted non-Status requests, `RecordNoMutationAbort`, every evidence/root step, and action-free begin/finish/cursor/recovery/exhaustion positions. Independent semantic cardinalities cover the conditional form rather than relying only on enum size. |
| P2-4 row-specific and causal durability alphabets | **Partially closed; superseded by the finding below** | §3.6 now separates physical, causal-parent, proof-only, and action-free rows; includes `AfterNeedsDurability`, Unix sync, both Windows round-trip boundaries, matching/stale/failed attempts, and avoids impossible no-op `Before` facts. The remaining omission is the ordinary physical executor-attempt dimension. |
| P2-5 pure recheck versus replay-bound authority | **Closed** | §3.5 splits non-authoritative aggregate classification/require from observer-private issuance, binds the proof to checked lineage and exact action position, keeps anticipated entry inside the sealed visitor, and prevents the executor from naming or receiving the proof. §3.6/R3 add the matching privacy and call-graph gate. |

## Finding

### [P2-1] Ordinary physical rows omit the executor diagnostic × fresh observation matrix

**Where:** revised RemPlan-3 §3.6 lines 423-468, R4 lines 550-563, and exit
criterion 8; frozen transition design §7 service-loop resolution and §13
required matrices; reverse-lifecycle interface §14.

**Violated contract.** The frozen reverse-lifecycle evidence rule requires
every physical action to cross executor `Success` and `Failed` with fresh
`NotStarted`, `Completed`, and `Ambiguous` observations. The executor diagnostic
alone must never advance a journal; a late failure followed by exact completion
must advance, success followed by exact-before must not fabricate completion,
and either diagnostic followed by ambiguity must follow the representable
recovery/no-rewrite rule. One invocation must not execute the same action
again.

**Evidence.** The revised plan says every row declares its legal observation
and attempt alphabets, but `PhysicalTransition` contains only:

```text
Before, After, real Ambiguous, operational error
```

Those are fresh observation classes, not executor-attempt outcomes. Unlike
`CausalParentTransition`, the ordinary physical declaration has no matching
success, matching failure, stale/mismatched attempt, or second-attempt cases.
Later assertions of total/per-action calls and restart do not create the
missing cross. Checked-artifact fault boundaries also do not substitute for
the service's `BoundExecutionAttempt` resolution matrix.

**Failure scenario.** A participant reset, native conflict abort, evidence
step, selected-root metadata write, backup-ref creation, stash, bundle write,
or ordinary root managed-object mutation returns a diagnostic failure after
actually completing. A fresh observer sees exact `After`. An implementation
that treats the diagnostic as authoritative can leave the journal pending;
one that treats `Success` as authoritative can advance when the fresh state is
still `Before` or ambiguous. The declared C7 cases do not require either
regression to fail for every action.

**Impact.** C7 can satisfy its row, cardinality, byte, and aggregate call-count
assertions while omitting the service boundary that proves executor results are
non-authoritative. This leaves D4 incomplete and permits a future action-specific
resolver or runtime regression to escape the generated gate.

**Required correction.** Give ordinary physical rows a separate attempt
dimension. At minimum generate, for every physical preservation and rollback
action:

- no attempt + fresh `Before` -> exactly one execution;
- no attempt + fresh `After` -> durable successor with zero executions;
- matching `Success` and matching `Failed`, each crossed with fresh `Before`,
  `After`, and real `Ambiguous`;
- post-attempt operational read error with the owner retained and no successor;
  and
- stale/mismatched/consumed-second attempt rejection where the shared attempt
  matrix does not already prove the exact same action binding.

Pin the expected transition/disposition, exact successor or retained record
bytes, representable recovery behavior, and total/per-action execution count.
`Failed + After` must advance from live proof; `Success + Before` must not
advance or retry within the invocation; either diagnostic + `Ambiguous` must
not produce an outcome. Apply the same explicit fact × attempt relation to the
causal-parent alphabet rather than listing its facts and attempts as an
uncrossed set.

## Projection and wire-feasibility conclusion

The revised rollback aggregate itself is feasible without a wire change:

- `RollbackAggregatePosition` is derived from the existing durable cursor,
  pending journal, participant terminals, publication evidence bit, and
  selected-root membership;
- the shared-root table is a read-only cursor-relative projection and does not
  persist a new completion bit;
- checkout overlays are internal exact-observation facts, with each overlaid
  domain proved by an existing evidence or metadata authority source;
- action-free mismatch remains the already frozen typed no-rewrite error;
- pending-action ambiguity uses the already legal retained-journal recovery
  shape; and
- `VerifiedRollbackPrefix` and the pure executor check are internal opaque
  capabilities/facts rather than model fields.

I found no remaining contradiction in the evidence -> selected-root
participant -> selected-root metadata ownership transfer. The table correctly
avoids rerunning a whole evidence `After` classifier after a later owner has
superseded its HEAD/ref/index facts, while retaining exact evidence authority
for the unsuperseded boundary/marker projection.

## Exit decision

Add the ordinary physical fact × executor-attempt cross to §3.6, R4, and exit
criterion 8. With that correction, this state-machine interface is GO for
implementation; no v1 wire/model amendment is indicated.

## Checks performed

- read-only comparison of every first-review P2 against revised §§3.5-3.6,
  R3/R4, the path manifest, and exit criteria;
- read-only comparison with the frozen recovery legality, proof privacy,
  request dispatch, root-handoff compatibility, causal-parent, and physical
  attempt contracts; and
- no tests were run because this remains an interface checkpoint rather than
  settled implementation.
