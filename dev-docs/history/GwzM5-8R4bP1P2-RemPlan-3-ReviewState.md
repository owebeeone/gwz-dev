# R4b-P P1/P2 third remediation-plan state review

Date: 2026-08-12

Reviewed:

- `GwzM5-8R4bP1P2-RemPlan-3.md`;
- the exact settled findings in `GwzM5-8R4bP1P2-ReviewArch-3.md` and
  `GwzM5-8R4bP1P2-ReviewFaults-3.md`;
- the frozen I2 action-journal, R4b transition, reverse-lifecycle, and
  interface-amendment contracts; and
- the settled rollback dispatcher, observers, recovery verifier, executor,
  selected-root helpers, preservation compatibility projection, and current
  matrix evidence at workspace/core commits
  `1c6bbba13586cb8be50576301072b38f0e0a3463` /
  `7d1a3e6a33cf2e4b5d882db1f7196f16ea7cc87d`.

This was a read-only design review. I did not modify the remediation plan or
production/test code and did not coordinate findings with the other plan
reviewer.

## Verdict

**NO-GO for implementation.**

There is no P0 or P1 finding. Sections 3.1-3.4 appear to answer the settled
checked-artifact findings at an implementable level, but sections 3.5-3.6 have
five P2 design blockers. The proposed rollback prefix does not cover the
complete rollback owner sequence or define shared-root supersession; its
recovery rule is illegal in action-free `RollingBack`; and the proposed C7
spec omits accepted legal handoff, request, no-mutation, and causal-durability
rows. The sketched prefix-token API also collapses pure physical rechecking
with authority issuance in a way that cannot satisfy the frozen replay-binding
and privacy rules as written.

These are plan defects rather than a reason to change the wire model. Each can
be corrected while retaining the existing v1 journal and transition
vocabulary.

## Findings

### [P2-1] The live rollback prefix omits completed publication evidence and has no shared-root supersession table

**Where:** RemPlan-3 §3.5 lines 192-230 and exit criterion 6; frozen transition
design §6.6 lines 625-665; I2 action-journal contract §3 lines 184-220;
`model/v1/validate/journal.rs:200-260`;
`authority/observe/reverse/rollback.rs:75-124`; and
`authority/observe.rs:93-122`.

**Problem.** The proposed verifier checks selected participants already
recorded `Aborted`/`RolledBack` and vaguely adds selected-root
“baseline/result artifacts where that owner requires them.” That is not the
complete rollback prefix. The frozen cursor is:

1. publication evidence;
2. participants in reverse durable order, including no-mutation rows; and
3. selected-root metadata.

Once `evidence_rolled_back` is durable, the evidence result is retained
history just like a terminal participant row. It must remain live-exact before
a later participant is observed or mutated. The plan does not require any
live evidence-result proof at those boundaries or at exhaustion.

The missing rule cannot be repaired by blindly re-running every completed
owner's local `After` classifier. Publication evidence and the selected-root
participant share root HEAD/ref/index/worktree facts; selected-root metadata
then supersedes some root file facts. An evidence `After` form that was exact
before selected-root participant rollback is intentionally no longer the
whole live form after that participant moves HEAD. The aggregate verifier
therefore needs an explicit, cursor-relative last-writer/supersession table
that assigns every live root fact to exactly one current expected projection.

The selected-root exclusion rule is consequently also under-specified. Using
the candidate path set as a blanket exclusion leaves an excluded marker/index
path unchecked after its last owning rollback step. Exact manifest/lock
baseline proof does not prove the marker, index, boundary, HEAD, or ref. An
excluded path is legal only while another exact observer proves it; after the
owning overlay has retired, the final complete checkout must include it or a
separate exact terminal artifact projection must prove it.

**Failure scenario.** Complete publication-evidence rollback, persist
`evidence_rolled_back`, then drift a baseline marker/index fact or the root
HEAD before a later non-root participant. The participant-only prefix in the
plan can still pass and permit that later participant mutation. With no
selected `@root`, the same omission can reach `AbortOperation`; with selected
root, a candidate-path drift can remain hidden by the proposed exclusion while
manifest/lock baseline checks still pass.

**Required correction.** Define a closed `RollbackAggregatePosition` table for
entry, every evidence phase/result, every participant cursor, selected-root
metadata phases, and exhaustion. For each position, declare:

- every completed/no-op owner that contributes live authority;
- the exact expected repository/ref/native/checkout/artifact projection;
- which later owner supersedes each shared-root fact;
- every temporarily excluded path and the exact alternate observer proving
  it; and
- the terminal projection in which no authority-bearing root fact is left
  unproved.

The prefix verifier must consume that table, not only terminal participant
states. Add service rows that drift each non-superseded evidence/root fact
after durable evidence completion and after selected-root participant
completion, asserting no later physical call and no false terminal abort.

### [P2-2] Prefix mismatch cannot always persist the proposed recovery overlay, and recovery resume is missing from the verifier call sites

**Where:** RemPlan-3 §3.5 lines 212-224; frozen I2 action-journal contract
§2 lines 129-144; transition design §6.5 lines 592-623; reverse-lifecycle
interface §8; and
`authority/observe/reverse/rolling_back_recovery.rs:6-70`.

**Problem.** The plan says a prefix mismatch may persist only a typed
`RecoveryRequired` state/context overlay and then describes repeated requests
from that recovery state. That is legal only while the exact pending rollback
journal remains present. The frozen wire contract deliberately forbids
`RecoveryRequired(origin=RollingBack)` between actions, because no resume
action exists. A prefix mismatch discovered before the next `Begin*`, at
post-clear exhaustion, or during entry therefore cannot use that overlay.

The proposed four call sites also omit rolling-back recovery-origin
verification. If prefix drift under a later pending action enters recovery,
`ResumeFromRecovery` must prove both the current pending action and the entire
completed prefix. The current verifier checks only the pending action. Without
the new prefix check, a request can commit `RecoveryRequired -> RollingBack`
while the completed prefix remains wrong, then immediately re-enter recovery;
that violates the plan's byte-stable repeated-request claim and needlessly
churns durable state even if a later mutation remains blocked.

**Required correction.** Split behavior by representability:

- reverse-entry preflight failure: typed rejection, no rewrite;
- action-free `RollingBack` prefix mismatch, including exhaustion: typed
  rollback/recovery error, no rewrite;
- prefix mismatch with a matching pending rollback action: bound ambiguity may
  enter `RecoveryRequired`, retaining that exact action byte-for-byte;
- physical-executor mismatch: return only the diagnostic; the fresh
  post-attempt observer performs the representable ambiguity decision; and
- rolling-back recovery resume: require the exact completed-prefix proof and
  exact current-action before/after classification before issuing
  `VerifiedRecoveryOrigin`.

Add pending-action and between-action rows for both requests, including fixed
and still-drifted retries, and assert exact record bytes: no rewrite for the
unrepresentable rows; one overlay write followed by byte stability for the
representable rows.

### [P2-3] The declared C7 legal domain omits accepted handoff, request, and rollback rows

**Where:** RemPlan-3 §3.6 lines 232-267 and R4 lines 322-334; amendment 1
§2.4 lines 139-175; amendment 2 §3.3 lines 186-200; transition design §7;
`authority/dispatcher.rs:255-296`; and
`tests/reverse_entry.rs:230-260`.

**Problem.** The plan calls five present handoff/index forms complete, but the
accepted compatibility matrix has six legal candidate forms. In addition to
`Baseline/Pre`, `Marker/Pre`, `Lock/Pre`, `Boundary/Pre`, and
`Boundary/Staged`, `Marker/Staged` is legal when candidate lock and boundary
bytes both equal their baselines. That conditional degenerate row is exactly
the kind of semantic form an enum-cardinality check cannot discover.

“Handoff absence” also collapses two distinct durable operation forms:
`NoCandidate` and `EvidencePending`. Both produce no root candidate field on a
pending action, but they have different record/publication prerequisites and
must receive separate legality decisions.

The plan further names only two incoming requests. The frozen dispatcher
routes every non-`Status` request through an active `Preserving` cursor, and
the accepted reverse-entry matrix treats every non-`Status` request there as
the exhausted-rollback route. `RollingBack` is likewise dispatched before
ordinary request legality. If C7 deliberately limits service coverage to
`Abort` and `Preserve`, it does not close the frozen request/state matrix.

Finally, “both participant rollback kinds” omits the legal
`RecordNoMutationAbort` participant position. That action-free terminalization
is part of the rollback cursor and completed-prefix problem even though it has
no physical Git action.

**Required correction.** Make `matrix_spec` declare, without deriving rows
from executed fixtures:

- `NoCandidate`, `EvidencePending`, all four `Pre` candidate forms, conditional
  degenerate `Marker/Staged`, and `Boundary/Staged`, with their exact
  publication/equality guards;
- all non-`Status` requests that the frozen dispatcher admits in
  `Preserving`/`RollingBack`, or an explicit reviewed change to that frozen
  policy;
- no-mutation participant terminalization as a verified no-op row; and
- all action-free begin, finish, and exhaustion positions in addition to the
  physical action enums.

Use exhaustive enum matches plus explicit semantic guard rows and independent
per-category cardinalities. A raw count over enum variants cannot prove the
conditional `Marker/Staged` case or distinguish the two absent-candidate
forms.

### [P2-4] The C7 cross-product omits `AfterNeedsDurability` and requires impossible observations for no-op/terminal rows

**Where:** RemPlan-3 §3.6 lines 243-256; amendment 2 §3.4 lines 226-261,
§5.4 lines 495-599, and §8; transition design §6.7 lines 703-710 and §8 lines
946-979.

**Problem.** The plan says every legal base row is crossed with `Before`,
`After`, and `Ambiguous`. The frozen root classifier has a fourth causal class:
`AfterNeedsDurability` / `PreservationDurabilityPending` for a required empty
marker parent. It is not ordinary `After`; a matching successful attempt is
required before the phase may advance. This is absent from the proposed
matrix.

Conversely, several declared crosses cannot exist. A source-equals-goal
managed object is goal-first `After` and has no legal `Before`; optional or
already-established parent phases are proof-only `After`; stash/reset
`Complete` positions are proof-only terminal states; and operational read
errors are errors, not synthetic `Ambiguous` facts. Generating a rectangular
`Before x After x Ambiguous` product will either invent physically impossible
fixtures or silently special-case them outside the purported declarative
spec.

The checked-artifact staging/durability boundaries listed by the plan do not
substitute for the root-parent causal protocol. That protocol also requires
the Unix parent-sync boundary, both Windows round-trip rename boundaries,
matching success/failure, restart before the phase rewrite, and stale,
mismatched, or second execution attempts.

**Required correction.** Give each legal matrix row an explicit observation
alphabet and attempt/fault alphabet. At minimum distinguish:

- physical `Before -> After` rows;
- structural `AfterNeedsDurability -> matching-attempt completion` rows;
- source-equals-goal and optional/established proof-only `After` rows;
- action-free `Complete`/exhaustion rows;
- real action-specific `Ambiguous` rows; and
- operational error rows.

Generate only legal crosses, assert independent cardinalities for each class,
and pin total/per-action execution counts plus exact successor and retained
artifact bytes for every interruption. Include the Unix and Windows causal
boundaries as frozen platform evidence rather than classifying them as generic
checked-artifact faults.

### [P2-5] The sketched prefix API conflates an authority proof with the executor's pure live recheck

**Where:** RemPlan-3 §3.5 lines 194-218 and path manifest lines 379-382;
transition design §4 lines 118-173 and §8 lines 946-1014; and
`reverse/execute/rollback.rs:17-74`.

**Problem.** The proposed API takes `backend, root, record, cursor` and returns
`VerifiedRollbackPrefix`, then the plan calls it from the physical executor.
As written, that value lacks the mandatory `StoredV1Record` source digest,
canonical open-record location, workspace/merge/operation identity, action,
phase, and payload binding. Adding those fields inside the function would make
an executor call issue an observer authority token, contrary to the frozen
proof-constructor/privacy boundary. Entry is also based on an anticipated
reverse-entry model, while observation/execution/exhaustion use a checked
persisted model; one unqualified raw-record signature cannot bind both
lineages honestly.

**Required correction.** Split the seam:

1. a pure, non-authoritative rollback aggregate classifier/require function
   consumes validated model facts plus canonical root and returns a closed
   physical fact/error;
2. the authority observer binds that fact to `StoredV1Record`, exact cursor,
   owner/action/phase, and payload hash and alone issues
   `VerifiedRollbackPrefix` or bound ambiguity;
3. reverse-entry preflight evaluates the anticipated model only through the
   existing sealed preview visitor and folds the result into
   `VerifiedRollbackEntryPreflight`; and
4. the executor, under the retained lease and exact persisted action, calls
   only the pure require/classifier and discards its non-authoritative fact.

Add a privacy/call-graph gate proving the executor cannot construct or receive
the opaque prefix proof and that no raw `MergeOperationRecordV1` plus path can
issue replayable authority. This is an internal interface correction; it does
not require a wire/model change.

## Acceptance conditions for the revised plan

The state-machine side of R0 is GO only when the plan itself:

1. contains the closed cursor-relative aggregate projection for publication
   evidence, every participant/no-op row, selected-root metadata, and terminal
   exhaustion, including exact shared-root supersession;
2. distinguishes representable pending-action recovery from action-free
   no-rewrite errors and adds prefix proof to recovery-origin verification;
3. declares all six legal candidate forms, both no-candidate durable forms,
   the full frozen request domain, no-mutation rows, and action-free terminal
   rows;
4. models row-specific observation/attempt domains including
   `AfterNeedsDurability` and the platform causal boundaries; and
5. separates pure executor rechecking from replay-bound authority issuance and
   names the privacy/call-graph evidence enforcing that split.

After those changes, sections 3.5-3.6 appear implementable without a new v1
wire field, durable phase, or production decoder/dispatcher activation.

## Checks performed

- installed `gwz status --json` confirmed the requested workspace/core/CLI
  settled commit tuple before this memo was written;
- read-only source searches confirmed the current rollback cursor skips
  terminal participants, rolling-back recovery checks only the current
  pending action, and the executor has no aggregate-prefix seam; and
- no test suite was run because this checkpoint reviews a proposed interface,
  not an implementation.
