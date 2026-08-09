# R4b typed-transition architecture review

Date: 2026-08-09

Reviewed scope: the settled `GwzM5-8R4bTransitionDesign.md`, the R4b section of
`GwzM5-8Refactor.md`, the R4b change budget, the amended frozen I2 record and
action-journal contracts, and the committed R3/R4a baseline at `8ff13b3`.

## Verdict

**GO.** No open P0, P1, P2, or P3 architecture finding remains. R4b-T may
proceed subject to the checkpoint gates and ceilings already recorded in the
design and change budget.

## Findings

- P0: none.
- P1: none.
- P2: none.
- P3: none.

## Accepted architecture boundaries

The settled design closes the authority and isolation risks examined in this
review:

- a retained root-bound mutation lease spans mutable open, observation,
  checked commit, physical execution, archive, and GC;
- only the kernel can construct the sibling-nameable opaque rewrite, and the
  store accepts no arbitrary v1 model or raw value;
- born-v1 and migrated-v1 installation use distinct opaque capabilities and
  exact source/absence rules;
- the dispatcher plus observation resolver form one closed restart loop, while
  action-specific proof bindings prevent cross-record/owner/action/phase replay;
- every value-bearing transition accepts only a bound opaque payload, including
  the state-preserving compound transitions added by the final state review;
- the bound execution-attempt handshake reconciles observation before handling
  executor errors, and the three closed failure/halt compounds atomically retain
  or write the exact participant facts and deterministic unattempted suffix;
- the effect/unknown-retirement interface is a one-to-one closed enum rather
  than a caller-provided path or retirement list;
- the owned serializable `v0_common_view` conversion is removed, so v1 cannot
  enter a v0 mutator through a compatibility projection;
- the physical-mutation matrix covers participant, publication, preservation,
  rollback, archive, and immutable-archive-worklist GC ownership; and
- the final checkpoint table and exact DAG require both R4b-F and R4b-X to wait
  for accepted T, S, and A interfaces, matching the budget gate.

The no-wire I2 amendments for exact-not-started abandonment and
pre-acceptance selected-root rollback are reflected in the closed transition
vocabulary, unknown retirement rules, and validator-correction scope. They do
not introduce a new durable field, phase, generic writer, or skip/partial-result
semantic.

## Implementation review focus

This GO accepts the architecture, not an implementation shortcut. Checkpoint
reviews must still reject any widened constructor, cloneable/raw authority
substitute, mutable v1 projection, unleased mutation path, consumer-local state
dispatcher, inferred unknown retirement, or production v1 reachability before
A1.
