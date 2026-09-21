# N3 remediation 1

Aggregate review at root2e5c06c/core2f12bbd/transporta6562e6: Code two P2,
State one P2. Reports are filed verbatim. One consolidated correction follows;
no independent convergent finding, no change to deferred activation boundaries.

- State P2-1: serialize lazy construction and publish success only. Inject a
  constructor to prove transient failure recovery, concurrent single publication,
  shared runtime and distinct operation observations.
- Code P2-2: derive terminal authentication rejection from the returned cause,
  never from per-key observations alone. Deterministic cancellation after a first
  rejected identity must retain cancellation; exhaustion still refuses and a
  later valid identity still succeeds.
- Code P2-1: preserve a bounded typed repository refusal through the existing
  local candidate stream boundary. Classify canonical refusal only; generic
  nonzero exit and oversized/noncanonical diagnostics remain generic errors.
  Prove native clone refusal and private-member omission/observation cleanup.

The correction must retain no extra socket/thread/resource owner, introduce no
wire fields or carrier, preserve admitted cleanup/backpressure, and stay within
the aggregate N3 bounds. Add red regressions first; run focused corrections and
the isolated SSH suite, then use the same two reviewers for closure on one new
committed tuple. Production/source/platform activation remains deferred.

## Implemented correction and evidence

The success-only runtime uses one family-shared mutex and owned constructor.
Its regression fails once with WouldBlock, then races three family/scope callers
and proves only one success plus actual pooled clone reuse with separate rows.
The two-key agent regression deterministically waits for the deadline immediately
after first refusal; the returned cause stays Net/timeout. Separate exhaustion
and existing later-key success tests preserve real refusal and success behavior.

Service diagnostics are reduced to one stream-local refusal receipt before empty
advertisement EOF. The pump requires complete, untruncated canonical stderr and
no stdout; generic/truncated/output-bearing cases stay generic. This mirrors
native empty-advertisement refusal classification without treating stderr as
cleanup or exit-status proof. A fixed marker crosses git2-rs's existing Net-class
Read adapter; only the candidate clone boundary recognizes it. A native forced
Git service produces canonical denial for an inaccessible loopback repository;
backend clone returns RemoteRejected, while an unrelated nonzero service error
returns GitCommandFailed. The materialization regression marks its member private
through repo-sync, removes the fixture checkout, and verifies quiet omission and
observation removal. No source or wire activation was introduced.

Initial red tests reproduced the lost refusal and overridden timeout. Constructor
and test-authoring compile failures are also retained. Focused corrections pass;
final gates and exact corrected tuple are recorded with the remediation evidence
and re-review prompts. No change to the aggregate bounds is required.
