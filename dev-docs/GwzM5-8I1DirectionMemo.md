# GWZ I1 v1 Direction Memo

Date: 2026-08-04

Status: **accepted; selected-root metadata-base correction independently
re-reviewed with no remaining P0–P3 finding**

This memo freezes only the M6 checkout-evidence and M8 accepted-lock
directions that v1 must respect. It does not freeze a durable schema, protocol
field, enum discriminant, product flag, or later-wave policy. I2 may define v1
only within these boundaries.

The behavioral authority remains `GwzM5-8Refactor.md`. This memo makes its I1
boundary independently reviewable before I2 freezes record and protocol
shapes.

## 1. Closed v1 scope

V1 contains only the lifecycle semantics that A1 will execute:

- current direct-ref integration, including deterministic `--no-ff`;
- a persisted, complete accepted workspace;
- born/unborn root acceptance;
- publication-required and no-publication consumption; and
- the already released continue, abort, preservation, rollback, status, GC,
  and finalization behavior needed to complete those operations.

V1 contains no dormant representation of M6 branch lifecycle, M7 snapshot
source provenance, or M8 optional/partial participation. Recognizing a future
concept is not permission to serialize a placeholder for it.

| Dimension | V1/A1 boundary | Later owner |
| --- | --- | --- |
| integration checkout | existing checkout only | I6/v2 branch lifecycle |
| branch mutations | integration ref movement plus checked reset of that same existing ref during released preservation/rollback | I6/v2 for checkout/ownership lifecycle |
| source provenance | direct ref resolved to an exact commit | I7/v3 |
| participant authorization | required | I8/v4 |
| participant outcome | participated | I8/v4 |
| accepted lock | complete deterministic workspace | extended, not reinterpreted, by I8/v4 |

## 2. M6 checkout-evidence direction

Acceptance keeps the integration ref and the final checkout as separate
logical facts, even though v1 permits only their existing-checkout form.
Collapsing them would force v2 either to reinterpret a v1 field or to infer the
original checkout after a switch.

For every v1 selected repository participant:

- integration uses the same existing attached, born local branch frozen before
  the first mutation;
- the frozen before commit, source commit, integration mode, resulting commit,
  and exact integration ref remain the execution and rollback authority;
- the accepted final checkout is that same attached branch at the verified
  result; and
- selected-detached integration is unsupported before record creation.

V1 must not encode, execute, or claim ownership of branch creation, checkout
switch/restoration, deletion, or branch cleanup. The released preservation and
rollback behavior may journal a checked reset of the same existing attached
integration ref and exact restoration of root publication files/index after
that reset; this is ref rollback, not M6 checkout lifecycle. V1 has no
operation-created branch, branch ownership token, ABA proof, checkout cursor,
or policy for leaving a different branch checked out. A request requiring any
such behavior is unsupported before record creation.

For an unselected metadata-base-present member, acceptance carries the authoritative
unchanged complete lock row, whether materialized or not, including only the
checkout evidence actually present in that row. An audit-domain member with no
metadata-base lock row receives no invented checkout or lock row. Neither case is
treated as a selected no-op or given integration evidence.

V2 may add explicit original checkout, integration checkout, final checkout,
branch ownership, and forward/reverse branch actions. Its adapter must map the
closed v1 existing-checkout case without changing v1 meaning. Full detached
HEAD, creation, restoration, cleanup, concurrency, ABA, rollback-order, and
root `--into` policy remains open until I6.

## 3. M8 accepted-lock direction

V1 acceptance is complete-workspace evidence, not a selected-result map. It
separates the operation baseline used for rollback from the accepted metadata
base used for composition. When `@root` participates, that metadata base is
the exact manifest and lock read from its verified result commit; otherwise it
is the exact frozen baseline manifest and lock. Publication output is never a
metadata-base source.

Its audit domain accounts deterministically for the union of:

- members represented by the accepted metadata-base lock;
- active members in the accepted metadata-base manifest;
- selected members; and
- members deliberately absent from the accepted lock.

The exact v1 audit-domain and membership construction is frozen at I2, but it
must preserve these directional rules:

- every selected v1 participant is required and has a participated outcome;
- a successful selected member is always `Present` with its one verified final
  checkout and complete accepted lock row;
- unselected metadata-base-present members remain `Present` with their
  authoritative unchanged metadata-base rows and remain distinguishable from selected
  participants;
- every member present in the accepted audit has an explicit `Present` or
  `Absent` lock-membership decision;
- each `Present` audit row equals the complete parsed lock row; and
- each `Absent` audit member is absent from the complete lock.

The complete lock's member keys equal exactly the audit members classified
`Present`. `Absent` is never a v1 skip or partial-success outcome and carries
no fabricated checkout evidence.

V1 has no optional authorization, skipped outcome, skip reason, partial
success, post-attempt skip, or root-skip representation. It does not reserve
dormant values for them. Continue, abort, preservation, rollback, exit status,
and reporting therefore use the existing all-selected-participants lifecycle.

I8/v4 owns the closed skip causes, authorization and outcome types, decision
points, partial exit status, exact participated/skipped/unselected membership
rules, root policy, and mutation ownership. V4 must extend the v1 complete-lock
domain through an explicit adapter; it must not reinterpret an absent or
unselected v1 row as a historical skip.

## 4. Root input and publication output

The workspace root is not a `LockArtifact` member. Its accepted input remains
separate from any commit or filesystem output created during publication.

The v1 accepted root checkout is exactly one of:

- `BornAttached`, with the accepted root commit and exact attached symbolic
  branch;
- `BornDetached`, with the exact detached commit; or
- `UnbornAttached`, with the exact attached symbolic branch and no fabricated
  object identifier.

When `@root` participates successfully, the input is `BornAttached` at its
verified integration result. Selected-root integration cannot be detached or
unborn. Otherwise the input preserves the frozen baseline root checkout,
including the already legal born-detached case.

The exact accepted metadata-base manifest and lock are immutable acceptance
inputs alongside the root checkout. For selected `@root` they come from the
verified root result commit, not the initial operation baseline or the later
composition commit. This lets restart reconstruct the audit, candidate, and
boundary without reading live Git while rollback authority remains in the
separate operation baseline.

Publication is permitted only from an attached accepted root. Its publication
branch is required and equals that accepted branch. `BornDetached` is legal
only when the complete accepted workspace deterministically requires no
publication; it persists and archives acceptance without fabricating a branch.
Evidence and composition commits, candidate artifacts, and publication
progress are outputs. They never replace or regenerate the accepted input. A
previously accepted unborn root that becomes born before its checked first
publication commit is drift, not an adoptable new input.

This preserves the released checked first-commit and rollback model and leaves
any future root checkout/`--into` design to I6/v2.

## 5. Preservation and publication ownership seams

Accepted workspace evidence is frozen before deciding whether publication is
required. Finalization, restart, status, preservation, abort, and rollback
consume the same immutable acceptance and exact accepted lock bytes; none may
regenerate it from live repositories or a later serializer.

V1 preservation owns only evidence for mutations executable under v1:

- direct integration and its checked rollback;
- current backup/stash preservation;
- root evidence and composition output; and
- candidate publication progress and its reverse restoration.

I2 supplies the typed pending rollback/preservation journals and recovery
origin required to own those current-lifecycle mutations; they are v1 safety
mechanisms, not dormant M6–M8 policy.

V1 publication owns applying and verifying the exact accepted workspace. Its
progress remains distinct from acceptance. A successful no-publication result,
including all-up-to-date work, still persists and archives the same complete
acceptance.

Later versions may add typed preservation or publication evidence only for
their new executable actions and outcomes. They must use a new semantic
version, explicit adapters, and version-specific archive projections. V1 does
not predeclare a generic action bag, optional participant policy, branch
ownership field, or future projection discriminant.

## 6. I2 constraints and I1 exit criteria

I2 may begin only if interface review confirms all of the following:

1. Integration ref and final checkout are separate logical facts, with v1
   closed to their existing-checkout case.
2. No v1 request, record, action, response, or archive shape can express M6,
   M7, or M8 executable semantics.
3. V1 acceptance covers the complete metadata-base/audit/lock domain while
   preserving the separate operation baseline, and is closed to
   required/participated selected members.
4. Selected, unselected, present, and intentionally absent members cannot be
   conflated.
5. Root attached-born, detached-born, and attached-unborn input is distinct
   from publication-created output, and detached-born acceptance cannot
   publish.
6. Preservation and publication can be extended by later versions without
   changing any v1 field's meaning.
7. Full M6 and M8 product decisions remain explicitly deferred to I6 and I8,
   where the I1 directions receive mandatory re-review before v2 or v4 is
   frozen.

If I2 needs a branch action, branch ownership claim, optional/skipped outcome,
partial-lock rule, fabricated root commit, or generic future-variant container,
I1 has failed and the v1 checkpoint must stop for redesign.
