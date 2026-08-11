# R4b-P fourth remediation filesystem-interface re-review 3

Date: 2026-08-12

Reviewed independently:

- third-amended `GwzM5-8R4bP1P2-RemPlan-4.md` at SHA-256
  `332e63f30bff59aadca359d400ba057a0cb1869ce5423ba5cc9a0ee546c76277`;
- `GwzM5-8R4bLinuxCapabilitySpike.md` at SHA-256
  `daca9bb82800590cb2a9d3188dde79ea0a3d58ca6cc9a0a591040f801f17f095`;
- the two open findings in
  `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS-2.md`; and
- the deterministic-capacity, live runtime-bootstrap, Linux empty-path, and
  pre-catalog collision contracts requested for this confirmation.

I did not read or coordinate with the state-machine reviewer. This was a
read-only design review; I did not modify production or test code.

## Verdict

**NO-GO for R1/R2 implementation.** There is no P0 finding. One P1 capacity
closure defect and one P2 collision-domain defect remain.

The two ReviewFS-2 directions are now substantially implemented in the design.
Retry scratch is deterministic, admission owns one reusable global scratch,
each admitted action reserves its active and retired directory capacity, and
the repeated-crash test is required to cross the nominal capacity without
cardinality growth or GC. `WorkspaceRuntimeBootstrapV1` is now explicitly a
live, non-durable protocol with one Git-directory guard and one final lease
object; it does not call the checked durability provider or either catalog
anchor. That closes ReviewFS-2 P2-1.

The literal action-slot table does not yet prove the capacity promise for the
managed-parent bootstrap it admits. The plan supports a bounded component
list, publishes one marker per component, and later retires all markers, but it
defines neither a literal maximum component count nor indexed per-component
slots. Its single `RetiredBootstrapMarker` slot cannot, as written, hold two
retired markers when deletion/GC is unavailable. This can strand an already
admitted action.

The pre-catalog order is also correct but its collision domain is incomplete.
`CatalogBootstrapV1` creates fixed scratch, active-record, and staging names in
the workspace `.gwz` parent, while the lossless Git collision scan is frozen
only for names below `.gwz/checked-artifacts`. An index-only entry at an
exceptional bootstrap name therefore passes the scan and can be materialized
by GWZ before the tracked collision is recognized.

## Findings

### [P1-1] The fixed action-slot arithmetic omits the managed component multiplier

**Where:** RemPlan-4 lines 136-145, 158-211, 230-237, and 496-537.

**Contradiction.** Admission promises worst-case capacity for every bootstrap
successor, alias, cleanup, and retirement, and promises that an admitted action
can terminate without GC. The only literal table has sixteen single fixed
names, including one `BootstrapIntent`, one `IntentSuccessorScratch`, and one
`RetiredBootstrapMarker`. `ManagedParentBootstrap`, however, accepts a full
bounded component list, creates and retains an ownership marker for each
component, publishes an immutable successor before each descent, and retires
the markers only after all components are installed. No
`MAX_MANAGED_PARENT_COMPONENTS`, indexed marker/successor slot family, or
formula connects that multiplicity to `MAX_ACTION_SLOTS`, action-name bytes,
and retired capacity.

There is a second location ambiguity in the same arithmetic: the catalog table
says intents live in the scanned private root, while the fixed action slot
table also places barrier/bootstrap intent names inside the action directory.
If intents are root entries, the `MAX_ROOT_ENTRIES` equation does not reserve
their worst-case count. If they are action-directory entries, the root grammar
must say so explicitly.

**Failure scenario.** A declared spec has two missing GWZ-owned components.
Both are durably installed with their ownership markers. Cleanup write-through
retires the first marker into the sole `RetiredBootstrapMarker` name and then
crashes or encounters a permitted GC failure. The second marker cannot be
moved no-replace into that occupied slot. Reusing, replacing, or deleting the
first would contradict the fixed-slot/no-GC contract, so the admitted action
cannot reach terminal catalog closure.

The same-boundary repeated-crash test proves that one scratch name does not
multiply; it does not prove bounded forward progress through the maximum
number of component successors and marker retirements.

**Required correction.** Freeze one of these interfaces:

1. define a literal `MAX_MANAGED_PARENT_COMPONENTS`, deterministic indexed
   staging/marker-retirement and intent-generation slots, and generated
   arithmetic that multiplies those slot families into every entry/name/
   metadata/retired limit; or
2. constrain every admitted v1 `ManagedParentSpec` to one missing component,
   prove that restriction against the complete caller inventory, and reject a
   longer missing suffix before reservation or mutation.

Also state whether intents are direct root entries or action-directory slots
and make `MAX_ROOT_ENTRIES` mechanically equal to that chosen grammar. Add a
maximum-component restart table that fills all non-reserved retirement
capacity, crashes after every component/successor/marker edge, disables GC,
and still reaches terminal closure with constant cardinality.

### [P2-1] The lossless collision scan excludes workspace catalog-bootstrap names

**Where:** RemPlan-4 lines 102-119 and 435-445.

**Gap.** The scan covers every control name or prefix *below*
`.gwz/checked-artifacts`. The exceptional first-catalog protocol necessarily
uses two record names and a staging name in the already retained workspace
`.gwz` parent, outside that prefix. Those names are part of the checked control
protocol but are absent from the stated Git index/tracked-path scan.

**Failure scenario.** The Git index contains one fixed `CatalogBootstrapV1`
scratch or staging path, but its worktree path is absent. The below-prefix scan
passes. Catalog bootstrap then creates the fixed worktree entry no-replace;
there was no existing worktree name to make creation fail. GWZ has now mutated
a tracked/index-visible path before detecting the collision, contrary to the
pre-mutation guard and exact-checkout ownership rules.

**Required correction.** Either place the entire workspace
`CatalogBootstrapV1` exceptional grammar in a proved non-worktree Git-directory
namespace, or add every fixed active/scratch/staging name and prefix used in
`.gwz` to the same lossless pre-catalog Git collision scan. Pin index-only,
stage-conflict, tracked-worktree, case-equivalent, and prefix-collision rows for
each exceptional form and prove zero bootstrap/private/managed mutation. This
finding does not require the live `.gwz/locks` substrate to make a durability
claim; it concerns the exceptional names that publish durable checked-catalog
state.

## Focused confirmation

| Requested item | Result |
| --- | --- |
| Deterministic retry scratch | **Closed for repeated faults at one boundary.** One action/kind name is rewritten in place; retries do not allocate a nonce. |
| Literal admission limits | **NO-GO (P1-1).** Constants and an enum are present, but no literal managed-component bound or multiplier makes the worst-case equation complete. |
| Reserved retirement capacity | **NO-GO (P1-1).** The action and retired-directory destinations are reserved, but one marker-retirement slot does not cover the plan's plural component markers. |
| Repeated-crash cardinality proof | **Closed for same-boundary scratch; incomplete for maximum-component forward progress.** The required test crosses nominal capacity and proves constant count without GC, but it does not cross all successor/marker retirements. |
| `WorkspaceRuntimeBootstrapV1` | **Closed.** It is explicitly capability-neutral and non-durable, uses fixed names, retains the Git-dir guard until the one final lease lock is acquired, revalidates the winning object, and has live/process-exit rows. No catalog or checked identity is needed. |
| Linux `AT_EMPTY_PATH` provider | **Closed in design; execution evidence still pending by rule.** The handle is queried from the retained no-follow descriptor with an empty path and `AT_EMPTY_PATH`; pathname relookup, `AT_SYMLINK_FOLLOW`, and `AT_HANDLE_FID` are forbidden. The same-invocation substitution row now proves descriptor rather than old-path identity. `name_to_handle_at(2)` documents this descriptor form; the capability requirement applies to `open_by_handle_at`, which this identity comparison does not use. |
| Pre-catalog ordering | **Correct order, incomplete collision domain (P2-1).** Live lease, read-only capability/collision checks, catalog bootstrap, capacity/bootstrap, and authority issuance are ordered correctly. The exceptional workspace bootstrap names must join the collision set. |

## ReviewFS-2 closure

| ReviewFS-2 finding | Third-review result |
| --- | --- |
| P1-1 valid scratch can exhaust the catalog | **Partly closed, still NO-GO as P1-1 above.** Deterministic scratch and action/retired admission remove repeated-fault growth, but the literal reservation does not cover plural managed-component cleanup. |
| P2-1 runtime bootstrap has no non-circular Windows protocol | **Closed.** The design deliberately narrows the promise to live cooperative exclusion, uses no durable barrier, and retains the fixed Git-dir bootstrap guard until acquisition of the one final lock object. |

## Exit decision

Do not begin R1/R2 production edits. Complete the literal action-capacity
equation (including managed-component successor and marker retirement), and
include the exceptional workspace catalog-bootstrap grammar in the lossless
pre-catalog collision domain or move it to a non-worktree root. Then repeat the
focused confirmation. The Linux R0-L native evidence remains a required
execution gate after the design reaches GO; its absence is not an additional
design finding.
