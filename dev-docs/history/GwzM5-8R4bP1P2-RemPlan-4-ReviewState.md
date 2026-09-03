# R4b-P fourth remediation state-machine design review

Date: 2026-08-12

Reviewed:

- `GwzM5-8R4bP1P2-RemPlan-4.md` in full;
- the frozen R4b reverse-lifecycle and action-journal contracts; and
- the settled-tree findings in `GwzM5-8R4bP1P2-ReviewArch-4.md`.

I did not read or coordinate with the filesystem reviewer. This is a design
review only; no production or test source was changed.

## Verdict

**NO-GO for implementation.**

RemPlan-4 is the right kind of redesign: it replaces weak interfaces instead
of adding another patch layer, gives the checked-artifact protocol cohesive
owners, and makes R4b-G remain blocked until exact-tree review. Sections 3.9
and 3.11 correctly close the preservation-diagnostic and rollback-prefix
authority findings at the design level. The terminal-row rule and global
catalog direction are also sound.

Four P2 design gaps remain. The most important is that C7 currently promises
an impossible single-stage test model: stale/mismatched and consumed-second
attempt classes cannot enter the same production service relation as matching
attempts. The selected-root and complete-checkout interfaces also assign
incompatible meanings to overlays and the global dirty summary. Finally, the
new prerequisite that a managed parent already exist delegates missing-parent
creation to a durable preflight action that the architecture never defines.

## Findings

### [P2-1] C7 needs stage-typed cases; not all 20 attempt/fact cells can enter through `service::run`

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:326-371`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:456-464`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:555-557`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/service.rs:53-117`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver.rs:51-82`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver.rs:261-267`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/dispatcher/invocation.rs:108-135`.

**Violated design goal.** Section 3.10 says every legal key, including all 20
attempt/fresh-fact cells, passes through one generated service executor and the
production service relation. Exit criterion 9 likewise requires every C7 key
to execute through production service and restart.

**Evidence.** `service::run` can produce only a matching success or matching
failure attempt: it receives an authorized physical action, executes it,
records the resulting sealed attempt against the unchanged stored record, and
passes that attempt to the immediately following observation. The resolver
rejects a stale or mismatched attempt at its binding gate before
`attempt_class`; `attempt_class` itself has only `None`, matching success, and
matching failure outcomes. A consumed-second attempt is stopped still earlier
by `V1Invocation::before_execute`; no second sealed `BoundExecutionAttempt`
exists to enter the resolver.

These early gates are correct production architecture. A test that makes all
five conceptual attempt classes enter only through `service::run` must either
weaken those gates, forge sealed authority, or falsely count a pure relation.
That is the same evidence error C7 is intended to replace.

**Required correction.** Make `CaseKey` a row- and stage-typed sum, for example:

```text
ServiceObservationCase {
    row, request,
    attempt: None | MatchingSuccess | MatchingFailed,
    fresh_fact,
    restart/platform form
}

BindingRejectionCase {
    row, request,
    rejection: StaleRecord | MismatchedRequestOrAction,
    fresh_fact
}

InvocationRejectionCase {
    row, request,
    rejection: ConsumedSecondExecution,
    first_diagnostic
}

ProofOnlyCase | ActionFreeCase | PlatformBoundaryCase
```

Add a named production-stage harness that can stop after production observer
issuance, after physical-action issuance, after attempt recording, and at the
invocation second-execution guard. It must obtain every request, observation,
action, and attempt through production constructors; it may interpose at a
stage, but may not construct a sealed token directly. Matching rows continue
through `service::run` and restart. Stale/mismatched rows assert rejection at
the binding stage, and consumed-second rows assert rejection at the invocation
stage, with exact unchanged bytes and zero additional execution.

The independent expected-key set must name the expected owning stage and
rejection stage. The conceptual five-by-four cardinality may remain as a
coverage audit, but exit criterion 9 must say every key executes through its
own production stage—not that every cell reaches the same service relation or
terminal exhaustion.

### [P2-2] `SelectedRootAfterPending` requires and forbids an overlay at the same time

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:262-279`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:292-309`; and
- `GwzM5-8R4bP1P2-RemPlan-4.md:445-454`.

**Violated contract.** The frozen aggregate ownership rule assigns every
shared-root fact to exactly one last unsuperseded owner. An overlay is valid
only for an exact path/domain fact simultaneously proved by that alternate
owner.

**Evidence.** The proposed `SelectedRootAfterPending` projection requires an
“exact terminal commit index and non-overlaid checkout” while also saying that
evidence still owns lock/marker worktree facts. Those evidence-result facts can
legitimately differ from the selected-root `before_commit` worktree. A
non-overlaid checkout compares them with commit bytes and rejects the valid
handoff; ignoring them makes the evidence ownership claim false.

The projection does correctly identify the missing physical-`After` window,
but it does not define which owner controls each worktree and index domain
during that window. Saying that all listed facts “therefore” have one owner
does not resolve the conflicting comparison.

**Required correction.** Freeze the handoff as a domain table rather than one
compound phrase. If the physical participant reset establishes the terminal
index, the expected shape is:

- selected-root participant: target ref, attached HEAD, native state, every
  index entry, manifest worktree, and all other non-overlaid checkout paths;
- publication evidence: exact boundary and absent marker, plus the exact
  lock/marker worktree paths it still owns; and
- complete checkout: terminal commit with only those named evidence-owned
  **worktree** paths overlaid, and no index overlay.

If the real physical action instead leaves candidate index entries in the
evidence form, the table must say so and overlay those exact index paths until
the durable successor. Pick one form from the physical reset contract; do not
permit both. State the ownership before physical execution, in the pending
`After` window, after `FinishParticipant`, and during each root-metadata step.
Add a positive handoff case where evidence-owned lock/marker bytes differ from
the terminal commit, plus one drift case for every worktree/index domain.

### [P2-3] A global dirty summary cannot coexist correctly with path-specific overlays

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:262-282`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:292-313`;
- `gwz-core/src/git/gitbackend/preservation_image.rs:53-143`; and
- `gwz-core/src/git/gitbackend/preservation_image.rs:188-193`.

**Violated contract.** Section 3.7 requires the complete comparator both to
consume dirty information and to accept exact alternate-owner overlays. The
frozen contract requires every accepted overlay fact to be path- and
domain-specific.

**Evidence.** The current dirty summary contains only three global booleans:
staged, unstaged, and untracked. Consuming those booleans after an exact
evidence-owned path is overlaid rejects a valid selected-root handoff merely
because the lock/marker is intentionally different from the terminal commit.
Discarding or clearing a global boolean to admit that overlay can hide an
unrelated dirty path or moved gitlink. No implementation using this summary
shape can distinguish the two cases.

**Required correction.** Replace “consume the dirty summary” with an explicit
path-keyed checkout observation. Each raw path must retain its index stages and
semantic flags, worktree fact, Git status flags, and—when it is a gitlink—the
nested HEAD/status proof. Apply `CheckoutOverlay` separately to the worktree
and index/status facts of exactly named paths only after the alternate-owner
proof succeeds. Then derive any display summary from the remaining facts; do
not use global summary booleans as authority.

Tests must include an exact overlaid lock/marker drift that is accepted, the
same overlay plus unrelated staged/unstaged/untracked drift that rejects, and
the same overlay plus moved/dirty nested gitlink that rejects. This interface
should be frozen at the R3 lead checkpoint before comparator and aggregate
work proceed in parallel.

### [P2-4] The required durable managed-parent preflight has no owner, interface, or restart model

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:82-86`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:252-260`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:414-443`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:496-510`; and
- `gwz-core/src/workspace_ops/merge/store/mod.rs:135`.

**Violated contract.** A checked mutation may issue authority only after its
managed parent has a stable component-wise path profile and durable identity.
The plan also requires every namespace edge and restart form to be represented
before implementation.

**Evidence.** Section 3.1 newly forbids checked-artifact code from issuing
mutation authority when the managed parent is missing and delegates creation
of a GWZ-owned parent to a “separate durable preflight action.” Section 3.6
defines bootstrap only for the private hierarchy and explicitly says that
protocol does not create managed parents. R1 freezes no managed-parent
preflight interface, R2 has no conversion step for it, the cohesion manifest
assigns it no owner, and C7 names only the private directory bootstrap.

This is a live cross-interface requirement, not hypothetical cleanup. The
merge store currently prepares its managed parent before acquiring the checked
record artifact. Leaving that preparation outside the new namespace and fault
model can recreate the exact gap the redesign is meant to remove, especially
on Windows where a visible new directory is not yet the promised durable
namespace fact.

**Required correction.** Add a frozen `ManagedParentBootstrap` (or explicitly
named equivalent) to R1. It must:

- begin at the deepest retained existing parent and create only a declared
  ASCII GWZ-owned component sequence;
- use the platform namespace protocol to make each new directory entry
  durable, reopen it no-follow, and record its full identity/path mode before
  descending;
- treat a user-managed missing parent as unsupported and a collision,
  substitution, mode change, or nonempty foreign partial as ambiguous;
- define closed before/after-create, barrier, reopen, and successor fault
  forms without publishing checked-action authority; and
- be idempotent across a crash so no new durable lifecycle phase is required.

Route every consumer that can create `.gwz`/merge/preservation parents through
that interface before checked acquisition. Add the owner to the cohesion
manifest, its boundaries to C7/platform keys, an R2 conversion item, and an
exit criterion proving no legacy parent-preparation path remains.

### [P3-1] “Reverse-order suffix” is ambiguous at the exact invariant it is meant to freeze

**Where:** `GwzM5-8R4bP1P2-RemPlan-4.md:284-290`.

The intended rule appears sound, but the phrase “one reverse-order suffix of
`selected_targets`” can be read in opposite directions. Freeze it with an
example and indices: for stored `selected_targets = [a, b, c]`, rollback order
is `[c, b, a]`; after one completed owner the only legal terminal set is
`{c}`, after two it is `{c, b}`. Thus terminal rows are a suffix of the stored
forward list and a prefix of its reverse traversal. Bind the pending/current
cursor immediately before that suffix/prefix. The live aggregate must still
inspect every terminal row even after validation succeeds.

This is a specification-precision issue rather than a separate present safety
defect because section 3.8 already requires both validation and all-terminal
live observation.

## Prior-finding closure audit

| Settled-tree finding | Design result |
| --- | --- |
| Arch P2-1 path equivalence/Linux durability | **Closed in design** by component-wise capabilities, ASCII restriction, supported-filesystem identity providers, and global catalog visibility. Filesystem feasibility remains the separate reviewer's responsibility. |
| Arch P2-2 payload cap/unbounded reads | **Closed in design** by separate bounded protocol reads and streamed exact payload proof. |
| Arch P2-3 tracked private-prefix erasure | **Closed in direction, interface amendment required.** The eraser is removed, but P2-3 above requires a path-keyed status image. |
| Arch P2-4 live gitlink omission | **Closed in direction, interface amendment required.** Live nested HEAD/status is required, but it must participate in the same path-keyed observation. |
| Arch P2-5 omitted terminal rows | **Closed in design**, subject to the P3 wording clarification. Validation and the aggregate independently cover all terminal rows. |
| Arch P2-6 selected-root ownership window | **Still open** because `non-overlaid checkout` conflicts with evidence-owned worktree facts (P2-2). |
| Arch P2-7 diagnostic precedence | **Closed in design.** The common relation owns matching success/failure plus fresh ambiguity. |
| Arch P2-8 executable C7 | **Still open** because the single service-stage promise is not executable for stale/mismatched/consumed attempts (P2-1). |
| Arch P3-1 prefix binding/privacy | **Closed in design.** The request/owner/action/phase payload and observer-private constructor are feasible. Use Rust visibility/compile-fail proof for constructor privacy, with a separate static call-graph gate; do not rely on source-text grep alone. |

The checked durability and Windows namespace corrections in sections 3.3-3.6
are coherent with the state-machine interfaces, but the unspecified managed-
parent bootstrap in P2-4 must be added before those interfaces can be frozen.

## Required design changes before GO

1. Replace the flat C7 key/single-service claim with stage-typed keys and a
   named production-stage harness for binding and invocation rejection paths.
2. Replace `SelectedRootAfterPending`'s non-overlaid checkout with an explicit
   worktree/index ownership and overlay table tied to the physical reset form.
3. Replace the global dirty-summary authority with path-keyed checkout/status/
   gitlink facts and overlay them only under an exact alternate-owner proof.
4. Define and own the durable managed-parent bootstrap through implementation,
   fault/restart coverage, C7, and exit criteria.
5. Clarify the terminal suffix orientation with one normative example.

After those changes, the architecture is ready for another independent design
confirmation. It should not enter R1 while any of the four P2 interface gaps
remains.

