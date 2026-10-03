# HTTPS SSPI composition proposal — Consistency-AXIS REVIEW

**Review object:** `dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md`, lines 1–576, committed DRAFT proposal at `fe40ba9b23e31da95358441ae5214a7cadb31b31`. Documents-only review; no implementation acceptance.

**Baseline:**

| Repository | Reviewed HEAD |
|---|---|
| gwz-dev | `fe40ba9b23e31da95358441ae5214a7cadb31b31` |
| gwz-core | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-sspi | `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-transport | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |

The object was read with `git show fe40ba9b23e31da95358441ae5214a7cadb31b31:dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md`. Reference documents and source were inspected at the unchanged member HEADs. All six HEADs matched at the start and end.

**Date:** 2026-10-04

**Axis:** Consistency against the controlling document graph, retained source contracts, internal invariants and proposed evidence obligations. Independent, adversarial, read-only. The other axes run in parallel; nothing here relies on them. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks acceptance of the proposed contract. No P0, P1 or P3 findings. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

Inspection covered:

- The complete composition DRAFT, §§1–9, lines 1–576, and complete `GwzSspiHttpsCallerGuide-DRAFT.md`.
- `AGENTS_GWZ.md`, the supplied workspace scope rules, `AgentProcessRules.md` and `GwzProcessOptimization.md`.
- `GwzSspiDesign.md` revision 2, particularly §§3, 5–6 and the accepted token-limit amendment; `GwzSspiPlan.md` step 4 and qualification boundary; `GwzSspiHostsCheckpoint.md`’s remaining-composition characterization; and `GwzSspiCallerGuide-DRAFT.md`’s mechanism, deadline, original-thread and cleanup contracts.
- Core’s `GwzRemoteTransportRetryPlan.md` §§4–5 and §§7–8; the helper timing and configuration-view amendments; the accepted SSH helper-clock amendment, including its precise supersessions and regression obligations; release amendment 2 §3.19; and Windows parity §§7–8 with its accepted supersession boundary.
- The production entry and handoff paths named in §3: CLI dispatch and worker descriptor; Python `ClientHost::network`, `Network::run`, route capture/run and worker descriptor; core local/cancellable invocation, HTTPS request continuation, endpoint/retry, preparation/budget, physical connection, lease, operation dependency and route retirement.
- Transport’s authored `transport.taut.py`, policy pairing, binding capability intersection, and codec admission rules.
- SSPI’s retained public mechanism-observation contract and its implementation:
  - `src/worker/windows/conversation.rs:255–263`;
  - `src/worker/windows/observation.rs:24–64`;
  - `src/worker/session.rs:16–71`;
  - `src/protocol/profile.rs:192–222`;
  - `src/protocol/bounds_tests.rs:295–330`;
  - `src/worker/test_support.rs:20–33`.
- The committed root changes accompanying the object: program checkpoint, release readiness and the existing caller-guide checkpoint link. They continue to describe composition and activation as outstanding.

Commands were limited to inspection: `git rev-parse`, `git show`, `git diff`, `git status`, `sed`, `nl` and `rg`. No tests, builds, native execution, source probes, remote commands, edits or Git mutations occurred. Member status inspection found no tracked modifications; the known core bug-report file remained untracked and outside the review.

Current peer reports and prompts were not read.

## 1. Findings

### [P2-1] Native facts conflate authoritative mechanism selection with token completion

**Location:** Composition DRAFT §7, lines 427–438, especially:

> authoritative=true follows only native Complete

The surrounding rule describes a `Continue` observation as provisional with `authoritative=false`, while the remote-success rule uses `Selected/authoritative=true` as its native prerequisite.

**Root cause:** The proposed facts projection treats mechanism authority and conversation completion as the same fact. The retained SSPI contract represents them independently.

`GwzSspiDesign.md:72–77` specifies native mechanism observations and states that direct NTLM selects its known requested provider. The existing caller guide likewise distinguishes `TokenStatus` from `MechanismObservation`.

The reference implementation makes the difference concrete:

- Direct NTLM’s `observation()` returns `Selected { mechanism: Ntlm, authoritative: true }` independently of token completion (`gwz-sspi/src/worker/windows/conversation.rs:255–260`).
- `Active::token` independently maps native status `0x90312` to `TokenStatus::Continue`, then obtains and forwards that mechanism observation (`src/worker/session.rs:17–31, 60–64`).
- The SSPI wire accepts an authoritative selected observation on a Continue token. Its rule is that Complete **requires** authoritative selection, not that authoritative selection **requires** Complete (`src/protocol/profile.rs:208–222`).
- The existing matrix explicitly admits this combination (`src/protocol/bounds_tests.rs:295–330`). The fake provider’s default already supplies Continue plus authoritative NTLM (`src/worker/test_support.rs:20–29`).

**Violated invariant:** Native facts must truthfully preserve the retained provider-observation contract while keeping token completion and remote acceptance separate. §2 does not propose changing SSPI’s mechanism-observation semantics or wire contract.

**Credible reproduction:** Select direct NTLM with either permitted source. The first successful native step returns:

```text
TokenStatus::Continue
MechanismObservation::Selected {
    mechanism: Ntlm,
    authoritative: true
}
```

This is admitted by the retained worker bridge and SSPI codec. Core must now project facts while awaiting another HTTP challenge. Forwarding the actual observation violates the proposed §7 rule. Replacing its flag with false silently turns a known authoritative mechanism into a provisional observation. Rejecting the token instead would reject a valid retained SSPI result.

If cancellation, deadline expiry or remote rejection occurs before Complete, that discrepancy remains in the terminal facts.

**Impact:** The proposal cannot both apply its stated native-facts rule and accurately report an already supported SSPI observation. Implementations following different interpretations can either reject valid NTLM progress or publish misleading mechanism provenance. The proposed authoritative-mechanism evidence row therefore has a concrete contract counterexample.

**Required correction:** Specify the projection with mechanism authority separate from token completion. Preserve the actual `MechanismObservation.authoritative` value, including authoritative Continue observations, and require observed native Complete separately in the producer’s remote-authentication-success checks. Alternatively, explicitly name and define a different completion field and revise the claimed observation contract accordingly; do not use `authoritative` to imply both facts.

Update the affected §7 validity and success rules together. Preserve the requirements that Complete alone does not establish remote acceptance and that remote success cannot bypass native completion.

**Closure/regression test:** Through the proposed production bridge and independently authored wire vectors:

1. Admit Continue plus authoritative selected NTLM and preserve that observation in facts.
2. Cancel or expire before Complete; retain truthful mechanism authority with `authenticated=None`, and truthful `credential_offered`.
3. Receive a response before native Complete; do not publish authenticated success merely because mechanism authority is true.
4. Complete native negotiation and obtain valid remote acceptance; only then permit `authenticated=Some(true)`.
5. Retain the controls for unresolved/provisional Negotiate, remote rejection, and Complete without remote acceptance.

These are future implementation obligations; the present finding is established by inspection, without executing native qualification.

## 2. Invariant analysis

**Clock source and domain changes are explicit.** The quoted “absolute existing operation/setup deadline” agrees with `GwzSspiDesign §6`. The draft acknowledges that the existing HTTPS physical deadline ends at establishment and that allocation and active-I/O budgets cannot supply an immutable authentication deadline. It proposes a new logical anchor using the existing positive aggregate, carries it across discovery and continuation, and preserves exact-expiry rejection. It expressly identifies the tighter enclosing boundary during helper work as a compatibility change.

**Helper timing and provenance remain distinguishable.** M4/M10 allowances stay independently captured. The enclosing setup deadline does not become helper interaction or admission provenance, and helper timeout/cancellation does not trigger default-logon fallback. The accepted SSH shared-clock mechanism is confined to its installed SSH path; the proposed HTTPS deadline does not claim to replace or pause that authority.

**Clock scope does not expand retry eligibility.** The draft retains the first-request-byte boundary and the existing failed-first-fresh-connect classifier. Redirects, authentication failure and post-effect traffic gain no retry authority from the extended clock. Receive-pack authentication must finish before its existing conservative `Effect::Possible` boundary.

**Caller capture addresses the actual handoff.** The proposed capture is made at original CLI/Python entry before detach or endpoint-thread execution. Its context binding, shared operation ownership, per-Start admission ticket, launch-time identity/liveness recheck and refusal/drop rules avoid substituting endpoint identity. The synchronous capture and final-handle disposal exceptions are expressly disclosed; polling is not claimed to perform those metadata operations.

**Policy and capability amendments are explicit.** New Windows policies preserve Anonymous/CredentialsDisabled and existing Gh meanings. Native admission requires acknowledged HTTPS policy and profile 2 or permitted profile 3. The draft accurately acknowledges closed-enum rejection by old bootstrap decoders and does not promise automatic downgrade. The authored additions use unused tags in the retained schema. No further compatibility contradiction was established.

**Origin and cleanup ownership are coherent at the proposal level.** The draft binds CBT to verified final-origin TLS and physical generation, retains exclusive lease ownership through challenge rounds, and separates native cleanup ownership from connection cleanup. Pending cleanup cannot make the connection reusable or revive terminal publication. Dependency-owned TLS/HTTP copies are acknowledged rather than covered by the SSPI wipe claim.

**The proposed evidence boundary is generally satisfiable.** Portable orchestration tests can exercise clocks, ownership, capability admission and publication order without activating the Windows endpoint. Native qualification remains separate. The mechanism-facts rows require the correction in P2-1; missing future implementation or test execution was not treated as a draft defect.

## 3. Risks and next action

Timeout-zero/native-refusal disposition remains explicitly pending. This report neither decides it nor treats the missing operator outcome as a finding. The proposed API and policy shape were reviewed independently of that outcome.

Windows provider/EPA/Digest/trust/proxy/Pageant qualification, physical-wire evidence, activation and release remain deferred. Earlier host acceptance does not accept this composition object.

The next action is one bounded text correction resolving P2-1, followed by Consistency re-review of the revised exact tuple. Operator disposition of the clock proposal remains a separate prerequisite for dependent implementation.

