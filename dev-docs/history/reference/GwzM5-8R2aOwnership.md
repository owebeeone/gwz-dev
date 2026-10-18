# GWZ M5–M8 R2a Integration/Message Interface Checkpoint

Date: 2026-08-04

Status: **Complete; amended interface and implementation approved by both
independent reviewers with no P0–P3 findings**

Baseline:

- workspace root: `9f6704c993df51a63be2419134428916f99d21fe`;
- `gwz-core`: `6ba97da8afa1d608e187b555bcc679471a38f4c1`; and
- installed `gwz status`: clean on `main`.

This checkpoint implements only R2a from `GwzM5-8Refactor.md`. It freezes the
internal integration/message seam that M5a will use. It does not expose custom
messages, activate `--no-ff`, or change a serialized record, protocol value,
Git mutation, event, error, or recovery decision.

## 1. Frozen scope

R2a groups exactly four existing immutable values:

```rust
struct IntegrationIntent {
    target_branch: String,
    before_commit: String,
    source_commit: String,
    commit_message: String,
}
```

The values continue to originate from the already-complete selection-wide
preflight and the pre-mutation message freeze. `source_commit`, not a fresh
resolution of `source_ref`, remains execution and recovery authority. A
conflicted participant's optional `expected_merge_head` is corroborating native
merge-state evidence, not a second authority: when present it must equal this
same exact `source_commit`.

R2a represents only the five legal prepared integration states:

```rust
enum PreparedIntegrationAction {
    VerifyUpToDate,
    FastForward,
    TrueMergeExpectedConflict,
    TrueMergeCommit(GitPreparedCommit),
    ResolveConflict(GitPreparedCommit),
}

struct PreparedIntegration {
    intent: IntegrationIntent,
    action: PreparedIntegrationAction,
}
```

The concrete Rust names may vary to avoid a collision, but this data and these
variants are frozen. There is no catch-all or `Other` variant. Construction
from Git analysis/preparation is fallible and rejects mismatched combinations,
so an up-to-date or fast-forward action cannot carry a commit specification
and a commit-producing action cannot omit one.

The following remain forbidden in R2a:

- checkout/original-branch intent;
- branch creation, switching, restoration, deletion, or ownership;
- merge strategy or writable `no_ff` intent;
- source provenance beyond the existing exact commit;
- required/optional participation or skip outcomes;
- accepted-workspace, lock-composition, or root-publication policy; and
- a new generic workflow/action framework.

## 2. Unchanged v0 adapter

`PendingMergeAction` remains the serialized v0 type, with its existing field
names, omissions, enum spellings, extension flattening, and compatibility
behavior. R2a adds internal encode/decode adapters; it does not move the wire
type or nest it in a new envelope.

| Existing v0 durable shape | Internal prepared action |
| --- | --- |
| `verify_up_to_date` + absent/`unchanged` result + no spec | `VerifyUpToDate` |
| `fast_forward` + absent/`fast_forward` result + no spec | `FastForward` |
| `true_merge` + `expected_conflict` + no spec | `TrueMergeExpectedConflict` |
| `true_merge` + `commit` + complete spec | `TrueMergeCommit(spec)` |
| `resolve_conflict` + `commit` + complete spec | `ResolveConflict(spec)` |

Every other combination remains `MergeRecoveryRequired`/ambiguous through the
current caller-specific mapping. In particular, the adapter must not repair,
default, or environmentally reconstruct a weak commit-producing record.

Encoding a current action always emits the same v0 bytes and values produced
today: an explicit `expected_result`, an exact commit specification only for a
commit-producing action, and an empty flattened extension map. Decoding retains
the legacy allowance for an absent expected result on only the up-to-date and
fast-forward no-commit shapes.

The adapter reconstructs exact author and committer names, emails, timestamps,
timezone offsets, and tree object from the record. No live repository identity,
clock, environment variable, source-ref lookup, or current Git state supplies
missing commit inputs.

Before status or continue trusts a decoded action:

- its four intent values must equal the participant record's four frozen
  values; and
- any present `participant.expected_merge_head` must equal the decoded
  `IntegrationIntent.source_commit`.

The absent `expected_merge_head` fallback remains compatible with released v0
records. Any repeated-intent or present merge-head mismatch is an ambiguous
pending-action recovery point and cannot be retried. Status reports the
existing pending-action ambiguity; continue independently repeats this check
and returns `MergeRecoveryRequired` before Git, even if a future status-policy
change accidentally makes the row eligible.

## 3. Exact commit-message contract

R2a becomes the only owner of final per-participant merge-message bytes. The
root publication message remains separately owned by publication and is not
changed by `-m`.

The final member message is:

```text
<normalized body>

GWZ-Merge-ID: <merge-id>
GWZ-Operation-ID: <operation-id>
```

There is no trailing newline after the operation-id value.

When no custom message is supplied, the body is exactly:

```text
Merge '<source-ref>' into '<target-branch>'
```

This preserves the released M4 bytes exactly.

For a future custom body supplied by M5a, normalization is frozen as follows:

1. reject a body containing NUL;
2. convert CRLF and bare CR line endings to LF;
3. remove trailing LF characters from the body;
4. reject a body that is empty or Unicode-whitespace-only after that
   normalization;
5. otherwise preserve every character, including leading whitespace, trailing
   spaces on non-empty lines, interior blank lines, and text resembling a Git
   trailer; and
6. append exactly two LF characters followed by the two mandatory GWZ lines in
   the order shown above.

The final two lines are the authoritative mandatory identity block. User text
is never parsed as or substituted for that block. These rules make the same
request deterministic across Unix and Windows without applying Git's
configuration-dependent cleanup modes.

Message construction completes for every selected participant before the
initial merge record is written. The exact resulting string is copied into the
participant record and every pending integration action. Start, restart,
continue retry, conflict resolution, and prepared-commit reconciliation use
that recorded string without rebuilding or renormalizing it.

R2a tests the custom-body constructor but start continues to reject every
present `MergeRequest.message` as `MergePhaseUnsupported`. M5a, not R2a, will
change validation and pass the request value to this constructor.

## 4. Call-site ownership

The R2a semantic production change is limited to six live files:

| File | R2a responsibility |
| --- | --- |
| `merge/integration.rs` | sole intent/action, v0 adapter, exact message owner |
| `merge/mod.rs` | private module declaration only |
| `merge/start/record.rs` | call message owner and integration encoder |
| `merge/continue_op/execution.rs` | consume the decoded typed action for restart/retry classification |
| `merge/continue_op/reconciliation.rs` | encode continue/resolution actions through the same adapter |
| `merge/status/pending.rs` | validate intent and reconcile through the decoded typed action |

The existing `merge/pending.rs` implementation is moved into
`merge/integration.rs`; it does not survive as a second adapter or message
owner. `start/prepared.rs` and backend traits remain unchanged in R2a: they
already produce the current exhaustive `GitMergeAnalysisKind` and
`GitPreparedMerge` inputs, which the integration constructor validates at the
durable boundary.

Implementation also requires import-only cleanup in `merge/continue_op.rs`
after the duplicate pending-action encoder is removed. Physical file accounting
therefore reports eight production paths: six semantic live paths, that
import-only parent, and the deleted `merge/pending.rs` path. Seven production
paths remain after the change. This variance reopens the file-count budget for
explicit post-implementation scope review; it does not authorize another
semantic owner or broader behavior. Focused tests may touch at most five files
under the ledger.

## 5. Required proof before implementation acceptance

R2a must prove:

1. every legal current start/continue prepared shape round-trips through the
   internal action and serializes to the existing v0 YAML shape;
2. every illegal kind/result/spec cross-product fails closed;
3. exact commit specifications reconstruct without environmental input;
4. pending/participant intent mismatch, including a contradictory present
   `expected_merge_head`, is rejected before retry or status adoption, and the
   resolution-parent mismatch test proves zero continue mutation;
5. default messages remain byte-for-byte identical;
6. custom-message normalization covers LF, CRLF, bare CR, trailing newlines,
   interior blank lines, Unicode, whitespace-only input, NUL, trailer-like user
   text, and no-final-newline output;
7. start, interrupted start, continue retry, conflict resolution, and status
   reconciliation use the identical recorded message;
8. the retained v0 fixture/reader gates and record golden tests remain green;
9. `message: Some(_)` and `mode: no_ff` remain rejected before record creation;
10. full Rust tests, strict Clippy, formatting, protocol freshness, Python
    parity, and the R0 compatibility/document checks remain green; and
11. measured R2a changes remain within 300 net production LOC, 250 moved
    production LOC, 450 test LOC, the reviewed eight-physical-path production
    allowance in §4, and five test files.

Implementation stops for review if any proof requires a wire change, a new
mutation, or a field forbidden by §1.

## 6. Interface review questions

The compatibility reviewer must answer:

- Does the mapping preserve every released v0 decode/recovery behavior,
  including absent expected results and weak-record rejection?
- Can any action be retried or adopted with intent differing from the
  participant record, including a contradictory resolution
  `expected_merge_head`?
- Does the proposed call-site boundary cover start, continue, and status
  without changing mutation/event/persistence order?

The message/recovery reviewer must answer:

- Are the normalization and exact-byte rules complete, deterministic, and safe
  for Git commit creation and prepared-commit comparison?
- Is there still any competing member-message constructor or recovery path?
- Can M5a expose `-m` using this seam without making `no_ff` or another future
  durable field writable?

Both reviews require an explicit **GO** or **NO-GO**. A GO may list P3
follow-ups only when none weakens the frozen v0, durability, exact-message, or
scope boundaries.

## 7. Interface review disposition

The first message/recovery review returned **GO**. The first compatibility
review returned **NO-GO** with one P1: a resolution pending action could carry a
`source_commit` contradicting the participant's present
`expected_merge_head`, while different paths trusted different copies. This
checkpoint was amended to require equality when that corroborating field is
present and to preserve the released absent-field fallback. Both independent
reviewers then returned **GO** on the amended interface. No P0–P3 finding
remained before implementation.

## 8. Measured implementation

Measured against the committed R1 baseline:

| Metric | Measured | Reviewed ceiling | Result |
| --- | ---: | ---: | --- |
| net production LOC | +141 | 300 | within ceiling |
| moved production LOC | ≤180 | 250 | within ceiling |
| test LOC | approximately 221 | 450 | within ceiling |
| production paths touched/created/deleted | 8 physical; 7 live | 8 reviewed | scope review required |
| test-bearing paths | 4 | 5 | within ceiling |

The moved-line figure conservatively includes the 59-line former pending
decoder plus the existing encoder, commit-spec conversion, and message
construction relocated behind the seam. The test figure is the new co-located
integration tests plus the net additions in three focused test modules.

The full Rust, strict Clippy, formatting, generated-protocol, Python parity,
document-consistency, retained-reader, and Bazel build gates are green. The
Bazel gate refreshed `MODULE.bazel.lock` for the already-landed Windows-only
`windows-sys` dependency; that generated root metadata change is not R2a
production LOC and receives explicit post-implementation scope review.

Both post-implementation reviewers returned **PASS** with no P0–P3 finding.
They accepted the eight-physical/seven-live-path scope exception, confirmed the
v0 decode and recovery matrix remained exact, and verified that the generated
Bazel-lock delta contains only input-digest refreshes plus the already-landed
Windows dependency alias and conditional mapping.
