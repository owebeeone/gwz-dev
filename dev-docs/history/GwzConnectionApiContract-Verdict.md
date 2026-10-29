# GWZ Connection API contract — merged verdict

Date: 2026-10-19. **GO — revision 2, interface/design scope only.**

## Exact reviewed object

Contract: `dev-docs/GwzConnectionApiContract.md`, 879 lines at root
`f4aff9b0b6524711b5065eb0d088e41ef43ecabe`.
Git blob: `593ce55e73f9a33ffd62f5b27407d4471f38cdc2`.
SHA-256: `70f361a38e07b4d97fc9616485601c9f081e88a9819625cb2b99d813a36dc225`.

The [review package](GwzConnectionApiContract-ReviewPackage.md) pins the full
nine-repository tuple, original operator brief and generated independent prompts.
All implementation dependencies remained reference-only. The original request
was for a concrete API contract and review addressing the recurring churn.

## Final reviews and closures

| Axis | Final report | Verdict | Open P0 / P1 / P2 / P3 |
| --- | --- | --- | --- |
| Consistency + API Surface | [Round 2](GwzConnectionApiContract-ReviewConsistency-2.md) | GO | 0 / 0 / 0 / 0 |
| Safety | [Round 2](GwzConnectionApiContract-ReviewSafety-2.md) | GO | 0 / 0 / 0 / 0 |

Two peer-blind reviewers examined the same committed revisions, verified all heads
at start/end and returned complete reports filed verbatim. The same two continued
under the operator's earlier explicit direction to use the old reviewers. No third
axis, extra closure agent or implementation agent was dispatched. Cross-family
model diversity is not claimed; only the harness's offered family was available.

| Finding | Correction | Originating reviewer closure |
| --- | --- | --- |
| Consistency P2-1: generic exchange cannot enter Git-specific sequence/byte interface | Typed neutral start plus one factored order core, Git profile wrapper and version-free ordered byte entry | Consistency round 1, rechecked round 2 |
| Safety P2-1: held permit can register after host revocation | Same authority transition covers Reserved/Registered; revoked permit yields terminal NeverRegistered and zero effects | Safety round 1, rechecked round 2 |
| Safety P2-2: Applied receipt precedes actual byte mutation through owned handoff | Remove handoff; synchronous borrowed candidate callback performs mutation before sequence advance/Applied; no-effect WouldBlock and cancel share lane guard | Safety round 2 |
| Consistency P3-1: missing final cleanup-domain lifecycle | Owner/handle construction and bounded close/drop with retained supervisor and two-host recipe | Consistency round 1, rechecked round 2 |
| Consistency P3-2: InputBudget at wrong error boundary | AdmissionError::InputBudget; BuildError::InvalidConfig(InputBudget) for invalid configuration | Consistency round 1, rechecked round 2 |

No findings were self-closed by the drafter. There was no blind convergence on an
identical new root; the complementary axes found different problems. They did
independently confirm the existing lifetime/composition defect and the need for
actual consumer proof rather than treating internal pool tests as Python reuse.

## Bounded loop accounting

Initial review: both NO-GO. Remediation 1: Consistency GO, Safety NO-GO on a new
application-commit boundary defect. Remediation 2: both GO. Three review revisions,
two remediation rounds used, three P2 and two P3 findings closed, zero open.
Two architectural roots were identified across the object, both resolved in the
contract. No third root was identified; the lane-stop rule was not triggered.

Round 1's selected neutral seam initially split eligibility from application;
round 2 corrected that introduced defect by preserving the current kernel's
atomic callback boundary. This matters for prevention: do not reintroduce an
owned post-Applied queue or make scheduler order carry the stream guarantee.

## What this accepts, and why it addresses churn

- One explicit persistent owner; operation handles cannot close shared resources.
- Mandatory injected dependencies and frozen operation inputs, without private
  pool defaults, late context repair or ambient recapture.
- Separate finite per-operation bindings instead of a host-wide mux rollover.
- Bounded admission before worker creation, truthful retained cleanup and explicit
  final domain lifetime; application result retention remains caller work.
- Generic in-process SSH/HTTP semantics over a neutral ordering seam with one
  algorithm; current Git taut envelopes remain a profile, not generic API types.
- Atomic application-before-receipt, conservative effect/replay boundaries,
  explicit auth/trust ports and qualification-gated native safety.
- A separate crate in the existing gwz-transport repository as the selected design
  placement, not a newly created repository or implemented Cargo change.
- Independent portable tests, focused native conformance and real gwz-git/core/
  CLI/Python production-consumer proof as distinct required gates.

These are accepted design obligations, not evidence that the repaired code exists.
No source implementation, kernel feature activation, dependency upgrade, member
change, native qualification, physical carrier, push/tag/publish or release occurs
under this acceptance. The other remediation drafts remain outside this gate.

Only the contract's acceptance header/links change after the reviewed tuple; its
substantive body is unchanged. This verdict/package is the per-object checkpoint,
without enlarging the already oversized historical aggregate checkpoint.

## Next bounded handoff

When implementation is authorized, start the portable owner proof and the named
pure-kernel neutral/atomic seam with deterministic fake connectors, clock, progress
and retainer. Required tests include two-operation reuse, 10,000 operations,
register/revocation race orders, two-host retained cleanup, input-budget refusal,
non-Git SSH/HTTP opening, reordered data/terminals, no-effect WouldBlock and cancel
before/after byte commit, plus the existing profile-3 vectors.

That proof must compile without core/git2 or native I/O. Native adapter safety,
caller migration and the promised useful release remain explicit later work;
no outcome is silently deferred or claimed closed by this document GO.
