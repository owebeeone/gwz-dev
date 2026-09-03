# R4b-P third remediation filesystem-interface final confirmation review 4

Date: 2026-08-12

Reviewed narrowly:

- the current `GwzM5-8R4bP1P2-RemPlan-3.md` source-equals-goal contract in
  section 3.4;
- its R1 transition-test requirement, C7 `ProofOnly`/cardinality rules, and
  exit criterion 8; and
- ReviewFS-3 P2-1 only, plus any P0/P1/P2 contradiction introduced by its
  correction.

This was a read-only confirmation review. I did not modify the plan or code.

## Verdict

**GO for the filesystem-interface R0 gate and for R1/R2 implementation.**

ReviewFS-3 P2-1 is fully closed. No new P0, P1, or P2 finding was introduced
by the correction.

## Closure confirmation

The revised source-equals-goal contract is now complete and disjoint:

1. **Highest-priority operation.** Section 3.4 evaluates source-equals-goal as
   a separate operation before ordinary existing-source replacement and states
   that it never enters that transition. The ordinary table is explicitly
   restricted to `source != goal`, eliminating the former overlapping
   `Before`/`After` rows.
2. **Exact durable authority.** Success requires authority absent, residue
   absent, and an exact durable managed source/goal. Missing or changed content
   is `Ambiguous`; unreadable content remains an operational observer error.
   Visible bytes alone are not elevated to proof.
3. **Zero transition mutation.** The successful proof-only row performs no
   authority publication, staging, detach, goal publication, cleanup, or
   alias retirement. Dispatch returns success without authority/residue
   retirement.
4. **Managed identity preservation.** The contract explicitly preserves the
   managed object's identity, rather than replacing equal bytes through a
   physical transition.
5. **Pre-existing family state.** Any authority or family residue is
   `Ambiguous`, retains every name, and requires explicit recovery. It is not
   adopted or cleaned by the otherwise no-op invocation.
6. **Executable evidence.** R1 requires focused source-equals-goal zero-call
   identity-preservation tests. C7 gives the operation a distinct `ProofOnly`
   row/cardinality, admits its real family-residue ambiguity and observer
   error, and asserts zero physical calls plus unchanged managed identity and
   family state. Exit criterion 8 repeats those exact gates.

The correction is consistent with the three operation-specific tables and
does not weaken the previously accepted family-key, durable-identity,
`DurabilityAnchor`, duplicate, or cleanup contracts.

## Platform and wire confirmation

The operation uses the already frozen platform durable-observation seam. A
platform/filesystem unable to provide the required durable identity or
durability guarantee takes the typed pre-mutation unsupported path; it does
not fall back to a mutating equal-byte replacement. This remains implementable
under the plan's explicit supported-filesystem contract.

No v1 wire field or lifecycle phase is required. The proof-only decision is
derived from the existing exact source/goal action, the stable artifact-family
lookup, and live durable filesystem facts.

## Exit decision

The filesystem-interface portion of the independent R0 checkpoint is
accepted. Production implementation may proceed through R1/R2, subject to the
plan's stop conditions and the later settled-tree acceptance gate.
