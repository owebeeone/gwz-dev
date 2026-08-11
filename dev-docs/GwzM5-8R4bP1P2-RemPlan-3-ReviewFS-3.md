# R4b-P third remediation filesystem-interface confirmation review 3

Date: 2026-08-12

Reviewed narrowly:

- the latest `GwzM5-8R4bP1P2-RemPlan-3.md` sections 3.1-3.4, R1/R2, and
  exit criteria 1-5; and
- ReviewFS-2 P1-1, P2-1, and P2-2 only, plus any new P0-P2 contradiction
  introduced while closing them.

This was a read-only confirmation review. I did not modify the plan or code.

## Verdict

**NO-GO, with one narrow new P2 table ambiguity.**

All three ReviewFS-2 findings are now closed. The revised identity, family,
durability-anchor, duplicate, and typed-unsupported contracts are implementable
without a v1 wire change. The existing-source replacement table is not yet
disjoint when source and goal are equal, however, so the promise that the three
operation tables are literal and exhaustive is not true as written.

There is no P0 or P1 finding.

## Finding

### [P2-1] Existing-source replacement has overlapping `Before` and `After` rows when source equals goal

**Where:** RemPlan-3 lines 282-294 and the source-equals-goal `ProofOnly` rule
at lines 527-533.

**Defect.** The existing-source replacement table does not state the
precondition `expected != goal`. With no authority or residue and a managed
leaf whose exact bytes equal both values, the first row classifies the same
physical state as `Before` while the second classifies it as `After`. With an
authority, `BeforeBound` can similarly overlap the exact-goal completion row.
The later C7 section correctly declares source-equals-goal to be `ProofOnly`,
but the checked-artifact table says it is implemented literally and unit tests
reject every unlisted cross-product. Those contracts cannot all be satisfied.

Choosing `Before` is not harmless: it permits authority creation, staging,
detach, and publication for a physical no-op and can replace the original file
identity. Choosing `After` by undocumented row precedence makes the supposedly
exhaustive table nonliteral and leaves residue handling unspecified.

**Required correction.** Either:

1. state that the existing-source replacement table applies only when exact
   source and goal differ, and add a separate source-equals-goal proof-only
   table/rule; or
2. add an explicit highest-priority disjoint row for source-equals-goal.

The proof-only rule should require durable exact observation, perform zero
authority/staging/detach/cleanup mutations, and classify any pre-existing
family authority/residue for that otherwise no-op invocation explicitly
rather than relying on row order. Add cardinality and transition tests proving
zero physical calls and unchanged object identity.

## ReviewFS-2 closure audit

| ReviewFS-2 item | Confirmation result |
| --- | --- |
| P1-1 parent-dependent family lookup | **Closed.** `artifact_family_key` now uses durable root identity plus a platform-equivalent canonical root-relative component sequence and explicitly excludes parent object identity. Parent identity remains authority evidence found only after stable family lookup. Original/alias reacquisition after parent replacement is required evidence. |
| P2-1 durable identity versus invocation rename-domain proof | **Closed.** Reboot-stable `DurableObjectIdentity` is persisted; `InvocationObjectIdentity` and `RenameDomainProof` are not. Linux persistent file handle plus durable filesystem ID, macOS persistent object ID plus volume UUID, and Windows volume GUID plus full 128-bit `FileIdInfo` are explicit. Filesystems lacking those promises fail typed and pre-mutation rather than becoming post-reboot ambiguity. |
| P2-2 duplicate and Windows barrier states | **Closed.** Same-full-identity crash aliases are distinguished from foreign duplicates and have named operation-specific rows. `DurabilityAnchor` is separately owned outside every family prefix, has a closed final/round-trip/alias/missing/error table, persists rather than requiring retirement, and has independent fault/reobservation coverage. |

## Platform and wire assessment

The platform identity contract is implementable with the stated typed
unsupported fallback:

- Linux can use a persistent filesystem file handle plus a durable filesystem
  identifier while retaining mount ID only as the current-invocation rename
  proof;
- macOS and Windows have corresponding persistent volume/object identifiers;
  Windows retains the complete `FileIdInfo` width; and
- unsupported filesystems are rejected before authority, anchor-dependent
  action state, or managed mutation.

This intentionally narrows supported filesystems, but it does not make an
impossible portability promise. R1's stop conditions and exact-tree Windows
behavioral gate prevent a build-only claim from substituting for semantics.

No v1 wire change is needed. Family/action keys, durable identities, anchor
state, and physical transition classification remain a bounded private
recovery protocol derived from the existing action and live filesystem facts.

## Exit decision

Make the source-equals-goal path disjoint and explicitly proof-only, then this
filesystem design is **GO for R1/R2**. No further change is required for the
three ReviewFS-2 findings.
