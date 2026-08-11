# R4b-P fourth remediation fifth state/authority confirmation

Date: 2026-08-12

Reviewed artifact:

- `GwzM5-8R4bP1P2-RemPlan-4.md` in full at SHA-256
  `9112674ba3a3b2b10576b8e633fcbbaafd391ca8baaca76c21881e0110dcba0f`;
  and
- every finding and required correction in
  `GwzM5-8R4bP1P2-RemPlan-4-ReviewState-4.md`.

This was a read-only design review. I did not change production or test source.

## Verdict

**GO — no open P0/P1/P2 state/authority design defect.**

All three P2 findings and the P3 ordering issue from `ReviewState-4` are closed.
The catalog admission, prospective-directory bounds, multi-bootstrap schedule,
cleanup worklist, and pre/post catalog scans now form one bounded, restart-closed
model. I found no cross-interface contradiction that reopens the previously
accepted Linux identity, runtime bootstrap, checkout, selected-root aggregate,
reconciliation, C7, or rollback-prefix authority contracts.

This GO is the state/authority R0 confirmation only. It does not waive the
independent filesystem review, the fail-not-skip Linux R0-L evidence, or any
other explicit pre-R1 gate in the plan.

## ReviewState-4 closure

| Prior finding | Result |
| --- | --- |
| P2-1 singular managed-parent schedule | **Closed.** One action binds an ordered plan with `K <= 8` bootstraps and `N <= 8` missing components in total; every plan is nonempty, so `K <= N`. The planner assigns stable bootstrap, global generation, and global component ordinals before admission. |
| P2-2 admission staging child bounds | **Closed.** While `Preparing`, staging is a prospective action directory governed before lookup by the exact final `MAX_ACTION_*` entry/name/metadata and child grammar. Staging and final consume one directory credit. Extra children and every limit-plus-one form are explicit ambiguity tests. |
| P2-3 mutable single cleanup cursor | **Closed.** `CleanupWorklist` is an immutable, at-most-three-row record. It is published before cleanup, never records progress, and retry derives progress only from exact source/destination facts. No successor, deletion, or GC is required. |
| P3-1 catalog enumeration order | **Closed.** The frozen order now performs a full bounded scan, global-ambiguity rejection, infrastructure recovery, and existing-family selection before admission/managed bootstrap, followed by a fresh scan before authority issuance. |

## Arithmetic and physical-layout confirmation

### Managed bootstrap schedule

For `K` nonempty bootstrap plans and `N` total missing components:

```text
initial generations     = K
installation successors = N
marker successors       = N
total generations       = K + 2N
```

Because `K <= N <= 8`, the maximum is `8 + (2 * 8) = 24`. The two named
extremes exercise different risks:

- `K = 1, N = 8` gives the deepest single successor chain: 17 generations;
- `K = 8, N = 8` gives the largest initial-generation overhead: 24
  generations.

The plan tests both. Non-overlapping global generation ranges and global marker
ordinals prevent a later bootstrap from colliding with an earlier bootstrap's
no-replace retired slots. Records bind local and global ordinals to the resident
schedule digest, so retries cannot silently consume a new ordinal.

### Action-directory slots

The literal action bound is arithmetically consistent:

```text
base slots                         13
64 barrier triples                192
24 bootstrap active/retired pairs  48
8 retired bootstrap markers         8
total                             261
```

The thirteen base slots include the single reusable barrier scratch, bootstrap
scratch, and `RecordScratch`, plus the immutable `CleanupWorklist`; each
indexed history has its own non-replace active/retired/alias destination. The
64-barrier and 24-generation maxima can therefore coexist without deleting or
reusing a retired destination.

The derived bounds are also correct:

```text
261 * 255 = 66,555 action-name bytes
261 * 256 = 66,816 action-metadata bytes
74  * 255 = 18,870 root-name bytes
74  * 256 = 18,944 root-metadata bytes
64  * 255 = 16,320 retired-root name bytes
64  * 256 = 16,384 retired-root metadata bytes
```

Admission staging is charged against that same 261-entry action credit from
its first appearance. Its root name is already included among the ten
infrastructure entries, and rename to the final action name exchanges one root
entry for one root entry. There is no staging/final double charge or unbounded
pre-publication child namespace.

### Cleanup closure

The worklist contains only source, goal, and authority alias rows. Barrier
anchor aliases and managed-parent markers remain owned by their indexed intent
protocols, so three is a complete rather than heuristic maximum. For each row:

- exact source plus missing destination authorizes one checked retirement;
- missing source plus exact expected destination proves completion; and
- every other combination is ambiguity with the immutable worklist retained.

Restart begins at ordinal zero and skips only physically proved completed rows.
The list is schedule-bound, published before its first mutation, and retained
until the whole action directory is durably retired. `RecordScratch` is needed
only for the one no-replace publication; it is not a hidden mutable cursor.
The 16 KiB decoder limit is paired with a generated maximum-three-row encoding
test.

## Prior contracts remain closed

- The persistent `Idle | Preparing` infrastructure record owns admission before
  directory creation and hands off only to an exact resident reservation.
- The exceptional catalog-bootstrap record has one permanent infrastructure
  retirement destination and does not consume an action-local slot.
- Every logical roaming barrier has a stable indexed active/retired/anchor-alias
  triple.
- Source and goal payloads physically occupy reserved action-directory slots
  but are streamed rather than materialized by catalog decoding.
- Protocol records, catalog entry counts, aggregate name bytes, and normalized
  metadata all have literal pre-allocation bounds.
- Retained-fd Linux identity still requires `AT_EMPTY_PATH`; capability and
  collision rejection precede private mutation.
- `WorkspaceRuntimeBootstrapV1` remains capability-neutral and separate from
  checked durable authority.
- Complete checkout retains separate index/worktree authority, the selected-root
  handoff has one owner for every fact, and every stored terminal row is
  observed.
- Fresh ambiguity continues to outrank executor diagnostics; C7 remains
  stage-typed and production-executed rather than tuple-counted.
- Rollback-prefix issuance remains private, request/owner/action/phase-bound,
  and absent from executors.

## State/authority disposition

No additional design amendment is required by this review. The plan may proceed
through its remaining R0 gates and then to the lead-owned R1 interface
checkpoint exactly as written.
