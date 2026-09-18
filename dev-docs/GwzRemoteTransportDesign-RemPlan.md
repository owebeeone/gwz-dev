# Remote transport design — merged remediation plan

Date: 2026-09-19. Status: **remediation round 1 complete; all six findings reviewer-closed at core
`05842b38e55f109ed3663555680751811a72eb9b`**.
Review object: gwz-core `3d2cc92c4a233e40ed049c8fe79bc555d39b272f`.
The [Consistency report](GwzRemoteTransportDesign-ReviewConsistency.md) and
[Safety report](GwzRemoteTransportDesign-ReviewSafety.md) are filed verbatim.
Both returned NO-GO: six blocking findings, one P1 and five P2. No finding is
self-closed by this plan. No implementation or platform evidence is claimed.

## One patch and closure mapping

Report-qualified IDs avoid collisions between axes. Each row has one disposition
and one closure scenario group, to be retraced by the originating reviewer.
The revision is one coordinated documentation patch, not a sequence of
independently reviewed partial fixes.

| Finding | Disposition | Closure scenario group |
|---|---|---|
| Consistency P2-1 | Accept: add a taut Bind/Bound/BindRejected handshake, session-bound immutable negotiated capabilities/limits, acknowledgement before Open, invalidation on disconnect, and core-service versus endpoint-capability separation. | Retrace incompatible versions, missing/SSH-only endpoints, payload min-selection and stale session replacement; each unsupported route refuses before connection/helper calls. |
| Consistency P2-2 | Accept: enumerate SCP, all native SSH aliases, HTTPS, retained local-only HTTP/git and file/local routes; narrow the blanket all-network claim; reject unsupported nonlocal placement before effects. | Trace each spelling through route selection and canonical pool scheme; preserve local git:// behaviour and prove HTTP/git nonlocal requests cannot fall through to core sockets. |
| Safety P1-1 | Accept: validate a credential-free structured destination before any transport message/helper/network action; reject HTTPS userinfo and query/fragment forms, including redirects; never pass a raw credential-bearing URL to the endpoint. | Trace username-only and password/token URL sentinels through local/carried opens and redirects; redacted refusal, zero helper/network effects, no sentinel in generated messages. Anonymous/gh paths remain supported. |
| Safety P2-1 | Accept: require per-remote libgit2 transport callbacks with owned operation context; prohibit process-global standard/synthetic prefix registration and thread-local factory binding. Qualify/extend the safe binding rather than weakening library isolation. | Retrace unrelated standard/custom transport activity before/during/after GWZ initialization, plus concurrent GWZ identities. No global registry write or interception exists; unsupported binding refuses before effects. |
| Safety P2-2 | Accept: define API close from every active half-state as ordered data flush + EndWrite before wire Close; close begins a deadline immediately and drains reverse traffic through bounded storage solely for cleanup. Closed reports discarded unread data and is never Git success. | Trace pending writes, exhausted credit, each half-state, unread response and racing Cancel; exactly one release, final offsets ordered, timeout cancels without replay. |
| Safety P2-3 | Accept: bound framing before body allocation, bounded nonrecursive/preflight decoding, finite metadata/depth/collection budgets and accounting for simultaneous encoded/decoded copies; equivalent in-process admission. | Trace oversized/huge-truncated frames, unknown deep collections, metadata and small-frame storms; bounds apply before allocation/recursion and failure releases affected leases. |

The acceptance matrix will carry these scenario groups. At this documentation
gate, closure means proving that each original counterexample has exactly one
specified outcome in the corrected text; executable tests remain prerequisites
for implementation acceptance, not invented evidence for this review.

## Review routing and cap

Operator override (2026-09-19): **"use the old reviewers"**. This supersedes
review-loop step 5.5's fresh-context rule for this draft. The fresh pair was
stopped without a completed report or verdict; its unfinished work is not used.
Continue the original Consistency and Safety agents, preserving their context.
They both closed their original counterexamples and checked changed-range
interactions, returning full axis GO re-verdicts in
[Consistency round 2](GwzRemoteTransportDesign-ReviewConsistency-2.md) and
[Safety round 2](GwzRemoteTransportDesign-ReviewSafety-2.md). Peer-blindness still applies within this
round. Both original reports and this merged plan are legitimate prior-round
inputs. No review round is counted for an abandoned attempt without a report.

Remediation rounds used after this patch: 1 of 2. Reviewers classify any new
architectural root cause. No third architectural remediation patch is
pre-authorized; apply the skill's stop/redesign rule if the cap is reached.

## Convergence and boundary

Both axes independently noticed process-wide registration/coexistence: Safety
classified it as P2-1, while Consistency recorded it as a qualification risk.
This is corroboration of the same seam, not two separate blocking root causes.
The six findings do not reopen the operator's endpoint placement, ordinary
endpoint trust, gh-only HTTPS, pool grouping, idle timeout or batching direction.

This patch changes only the four transport documents plus review/process
records. No code, schema tags, dependency, private experiment, release, tag or
push is included.
