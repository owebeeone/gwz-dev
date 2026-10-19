# Credential + Connection joint design: remediation round 1

Date: 2026-10-19. Status: **proposed correction; awaiting independent re-verdicts**.

Reviewed root: `158a940b961941ebe0bd5c47c771dd9658156a04`.
All eight reference member heads remain as recorded in the initial review package.
Both reports returned NO-GO, with two P2 findings each and no other severities.

## Merged disposition

| Finding | One disposition | Closure evidence to retrace |
| --- | --- | --- |
| Consistency P2-1 | Correct lifecycle/accounting grammar | Credential §§4–5: operation owner/context split, close/drop table, terminal outcomes, retained bridge/probe, inherited domain and explicit finite defaults; §7.4 unpolled close/late completion traces |
| Safety P2-1 | Same lifecycle correction | Provider close seals all children immediately, operation close affects only its children, finish preserves peer fact while cleanup stays pending, physical disposition/ScopeLease revocation and generation-qualified exactly-once disposal |
| Consistency P2-2 | Select concrete operation/provider mapping | One operation issues multiple one-use child conversations; core owns the narrow two-library bridge; SSH/HTTP source map, shared proof coordinator and source-based small-change test comparison in §7 |
| Safety P2-2 | Same provider-mapping correction, explicit-key branch | Credential §4.1 LocalSshKeyPort scoped snapshot borrow at executing endpoint; existing Unix/Windows memory-auth calls stay native; no agent fallback/key reopen; cancellation retains key pin until call/disposal completes; §7.2 trace |

The two reviewers independently identified two architectural roots: (1) terminal
ownership/accounting, and (2) the operation/provider responsibility boundary.
The explicit-file-key handoff is part of root 2, not a third independent root.
The lane owner does not self-close these findings. Reviewers must verify both
their original counterexamples and the combined correction.

## One bounded correction

Only Credential design and joint review records change. The accepted Connection
contract and every source/dependency repository remain unchanged. §8 maps the
composition onto exact existing Connection clauses rather than assuming an
unrecorded dependency or port amendment.

The selected design uses existing native Port/Session, retained Starting/Finishing
probes, SSH agent callback and explicit memory-auth primitives. It does not add
a supervisor, network stack, generalized signing service, carrier or dependency
upgrade. The mandatory retainer/progress ports project into the existing shared
domain; distinct native and physical resources retain distinct reservations.

The same two reviewers continue under the operator's standing instruction to use
old reviewers. This overrides the skill's usual fresh replacement for material
interface changes; it does not add agents or waive counterexample retracing.
Each reviewer retains its axis, and remains blind to the other's current round.
Prior reports and this merged remediation are legitimate shared inputs now.

One of two remediation rounds is used. Both axes explicitly classify any new
architectural root. A third new architectural root on this joint object stops
further remediation for the operator's redesign/acceptance decision. Earlier
roots on the separately accepted Connection contract are not newly attributed
to this object, but regressions remain blocking.

## Verification and limitations

This is design-only trace closure. No portable library, native provider, Windows
test, actual endpoint placement or build/test cost reduction has been executed.
The chosen API must receive joint re-review before repository creation or source
work is separately authorized. Current process safety and provider qualification
remain gates; no reduced platform release is authorized.
