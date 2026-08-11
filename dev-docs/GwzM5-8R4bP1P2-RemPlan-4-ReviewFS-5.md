# R4b-P fourth remediation filesystem-interface re-review 5

Date: 2026-08-12

Reviewed independently:

- fifth-amended `GwzM5-8R4bP1P2-RemPlan-4.md` at SHA-256
  `9112674ba3a3b2b10576b8e633fcbbaafd391ca8baaca76c21881e0110dcba0f`;
- the open P1-1 and P2-1 findings in
  `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS-4.md`; and
- the earlier filesystem findings only as needed to check that the amendment
  did not reopen a P0/P1/P2 durability, identity, boundedness, or restart gap.

I re-read the complete amended plan. I did not read or coordinate with the
state-machine reviewer. This was a read-only design review; I did not modify
production or test code.

## Verdict

**GO for the filesystem/capacity design gate.** I found no open P0, P1, or P2
defect in the reviewed interfaces.

ReviewFS-4 P1-1 is closed by making multi-bootstrap ownership explicit. One
admitted action binds one ordered plan with `K <= 8` bootstrap specs and
`N <= 8` missing components in total; every admitted spec contributes at least
one component. The exact immutable-intent demand is therefore `K + 2N <= 24`,
and global plan/component/generation ordinals prevent two specs from reusing a
retained slot.

ReviewFS-4 P2-1 is closed by classifying admission staging as a prospective
action directory. Recovery applies the final action directory's count, name,
metadata, kind, and child-grammar bounds before lookup, decode, adoption, or
publication. Exact-limit, limit-plus-one, wrong-kind, and extra-foreign-child
rows are mandatory.

The related cleanup-capacity gap is also closed. One immutable worklist has at
most the three source, goal, and authority rows. Their destination slots are
pre-reserved, the worklist is published before the first retirement, and
progress is derived from exact source/destination facts rather than a mutable
cursor. Crashes cannot allocate a successor name, and neither successful
recovery nor terminal action-directory retirement depends on deletion or GC.

## ReviewFS-4 finding closure

| ReviewFS-4 finding | Fifth-review result |
| --- | --- |
| P1-1 seventeen generations assume one managed-parent spec | **Closed.** The admitted plan now has literal `K <= N <= 8`; global generation demand is `K + 2N <= 24`, with non-overlapping ranges bound to the schedule digest. |
| P2-1 admission staging children lack an action bound | **Closed.** Staging is enumerated as a prospective action directory under the exact 261-entry/name/metadata/no-follow grammar before any child lookup or adoption. |

## Capacity proof

The literal slot arithmetic is internally consistent:

```text
base slots                         13
64 barrier triples               192
24 active/retired intent pairs    48
8 marker-retirement slots          8
                                  ---
MAX_ACTION_SLOTS                  261
```

The corresponding aggregate limits are correct:

- `261 * 255 = 66,555` action-name bytes;
- `261 * 256 = 66,816` action metadata bytes;
- `64 + 10 = 74` root entries;
- `74 * 255 = 18,870` root-name bytes;
- `74 * 256 = 18,944` root metadata bytes;
- `64 * 255 = 16,320` retired-root name bytes; and
- `64 * 256 = 16,384` retired-root metadata bytes.

The ten root infrastructure forms are exhaustive for the stated grammar,
including the permanent catalog-bootstrap retirement record and the admission
active/scratch/staging names. Staging and final action names consume one action
directory credit because the durable no-replace rename cannot make them two
directories. The fixed staging root name remains conservatively included in
the infrastructure budget while present.

For managed-parent work, each of `K` plans contributes:

- one initial immutable intent generation;
- one successor per installed component; and
- one successor per retired marker.

With `N` total components this is exactly `K + 2N`. Because every admitted
plan has at least one component, `K <= N <= 8`; the maximum is the eight-plan,
eight-component layout, which consumes all 24 active/retired pairs. The
one-plan, eight-component layout exercises the deepest successor chain. Both
extrema, all eight global marker ordinals, and all 64 barrier triples are
included in the no-deletion fault gate.

## Admission and staging ownership

The admission handoff is restart-closed:

1. durable `Preparing` owns the fixed staging and deterministic final names
   before either directory is created;
2. staging is bounded as a prospective action directory and may contain only
   the matching missing/partial/exact resident reservation;
3. its resident reservation takes ownership when the directory is published
   no-replace to the final action name;
4. only that exact handoff permits the admission record to return to `Idle`;
   and
5. authority or managed mutation begins only after durable `Idle` is observed.

The matrix covers old/new admission values, partial/exact scratch,
missing/exact/other staging and final names, missing/exact/other reservation,
every aggregate staging limit, and a foreign child. A crash reuses the same
infrastructure and directory names, so repeated admission failures have
constant cardinality.

## Immutable cleanup worklist

Cleanup has a complete fixed capacity and ownership transfer:

- `RecordScratch` is the one reusable prepublication scratch;
- `CleanupWorklist` is the one immutable published owner;
- its maximum three rows bind the source, goal, and authority source
  identities to their schedule ordinals and the already reserved
  `RetiredSourceAlias`, `RetiredGoalAlias`, and `RetiredAuthorityAlias` names;
- the resident schedule digest prevents adoption under another action or slot
  assignment;
- `exact source + missing destination` performs one checked retirement;
- `missing source + exact destination` proves that row complete; and
- every other physical combination is ambiguity with the worklist retained.

The worklist is published before any source moves. Retry rescans from ordinal
zero and derives progress from durable physical facts, so it needs no mutable
cursor, progress successor, deletion, or reclaimed retirement slot. Indexed
roaming-anchor aliases and bootstrap markers remain owned by their respective
intent protocols and cannot inflate the three-row list. Maximum-encoding tests
pin all three rows below the independent 16 KiB decoder limit.

## Collision/bootstrap confirmation

The exceptional workspace collision domain remains closed. It contains the
entire `.gwz/checked-artifacts` prefix and the exact external catalog-bootstrap
scratch, active, and staging forms. Exact, parent-prefix, platform-equivalent,
index-only, conflict-stage, skip-worktree, and tracked-worktree collisions all
reject before those names, the private root, or a managed path is created.

After successful first-catalog publication, the external active bootstrap
record moves write-through and no-replace to the one fixed
`catalog-bootstrap-retired-v1` infrastructure destination. That permanent
record consumes one of the ten root infrastructure entries and requires no GC.
External scratch/staging states remain fixed-cardinality forms in the bootstrap
restart table.

## Prior-interface regression check

No prior filesystem P0/P1/P2 was reopened:

- action scratch, admission scratch, intent scratch, barrier ordinals, marker
  ordinals, alias retirement, and terminal directory retirement are all fixed
  before mutation and reused after a fault;
- retirement capacity accounts for every admitted active directory before the
  retired root can fill, so failed GC may block a new admission but cannot
  strand an admitted action;
- `WorkspaceRuntimeBootstrapV1` remains capability-neutral, fixed-name, and
  explicitly non-durable, with the Git-directory guard retained until all
  contenders converge on the one final lease object;
- Linux durable identity still comes from the retained no-follow descriptor via
  empty-path `name_to_handle_at(..., AT_EMPTY_PATH)`, with pathname relookup and
  unsupported providers forbidden;
- component-wise path equivalence, lossless pre-catalog collision ordering,
  bounded/fallible protocol observation, streamed payloads, and exact Windows
  flush rights remain intact; and
- Windows publication, retirement, catalog-anchor, barrier-intent,
  roaming-anchor, managed-bootstrap, and duplicate recovery retain their
  write-through and exact reobservation requirements.

The Linux R0-L ext4 remount/provider evidence remains a mandatory execution
gate before R1. Its pending execution is not a design finding.

## Exit decision

The filesystem/capacity reviewer approves the frozen design for implementation.
R1 may begin only after the separately required Linux R0-L evidence and the
independent state-machine reviewer also report their required GO. The later
settled-tree implementation tests and two R6 code re-reviews remain mandatory;
this design verdict does not substitute for them.
