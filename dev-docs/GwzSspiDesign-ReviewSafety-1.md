# DRAFT GWZ SSPI process boundary — SAFETY-AXIS REVIEW

**Review object:** DRAFT revision 2 of `dev-docs/GwzSspiDesign.md`, revised caller guide and implementation plan, remediation round 1, at root `f2029e4b1739c0214138675dfb16abdb44f6a0d7`.

**Baseline:** root `f2029e4b1739c0214138675dfb16abdb44f6a0d7`; gwz-core `d78a664e3c5a325c6f12be409eb7645c1c1b51d0`; evidence `1beb1d204c824701ddbd033c7f89df9a3561f5e5`. Read committed bytes using explicit-SHA `git show` and the complete package diff from `377c5e2` to `f2029e4`. All three HEADs matched at start and end.

**Date:** 2026-10-03

**Axis:** Safety: authentication inputs, secret ownership, containment, cancellation, deadlines, cleanup and host composition. Independent, adversarial, read-only. Nothing here relies on current peer reports. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0, P1, P2 or P3 findings from this reviewer. Both full-Safety findings are closed. This accepts the bounded mechanism/API design only.

---

## 0. Evidence base

Read the corrected full-Safety remediation prompt, the complete revised:

- `dev-docs/GwzSspiDesign.md`, §§1–7.
- `dev-docs/GwzSspiCallerGuide-DRAFT.md`, public signatures, request/result contracts, deadline behavior and host walkthrough.
- `dev-docs/GwzSspiPlan.md`, all implementation steps and review stops.
- `dev-docs/GwzSspiDesign-RemPlan-1.md`, including dispositions and proposed regression obligations.

Inspected the complete changed range for those three package documents. Also checked the range for `GwzCoreSessionCrateMap.md`, `GwzLocalCloneLibraryBoundaries.md`, `AgentProcessRules.md` and `GwzProcessOptimization.md`: no changes.

The core/evidence revisions remain identical to the full initial Safety review. Its controlling-graph and native-evidence inspection therefore remains applicable: Windows parity’s explicit amendment and authentication/route rules; accepted helper timing, SSH clock and configuration-view boundaries; core requirements/design; native alternative, process-worker and creation-containment reports; and the two native campaigns’ committed receipts and raw results. No new native proof is claimed.

Checked the added negotiation observation against [Microsoft’s negotiation-info contract](https://learn.microsoft.com/en-us/windows/win32/api/sspi/ns-sspi-secpkgcontext_negotiationinfoa), which distinguishes a chosen package from negotiation completion and permits intermediate queries.

Commands included explicit-SHA `git show`, bounded `sed`/`nl` reads, the package/authority `git diff`, primary-source browsing, and all three `git rev-parse HEAD` checks at start and end. No writes, builds, tests, repository mutations or current-peer-report reads occurred. Closure below is document-contract closure; executable regressions remain implementation obligations.

## 1. Prior-finding closure

| Finding | Original counterexample retraced | Disposition |
|---|---|---|
| **Safety P2-3:** Begin omitted Digest’s initial challenge | Construct Digest with explicit identity and challenge C; verify Hello; call required `step(None)`. Revised design §4, lines 144–154, expressly carries `initial_challenge/method/URI` in Begin and supplies C to the first Initialize. All three fields are Digest-only; challenge is nonempty, bounded and zeroizing. Invalid combinations refuse before native work. The caller guide states the same path. No invented message, extra round or token parser is needed. | **Closed.** Exact-first-input and malformed/wrong-package cases are explicitly required fake-protocol regressions in §7. Existing cancellation and owned-storage rules cover cancellation before Begin. |
| **Safety P3-1:** Cooperative worker budget lacked a carrier | Delay Begin and move the enclosing deadline earlier. Revised §6, lines 216–226, removes worker-budget transmission entirely. The parent retains the immutable original deadline; earlier enclosing expiry uses supplied Cancellation. Delayed delivery creates no new worker allowance or clock-domain conversion. | **Closed.** Parent enforcement and publication revocation remain authoritative during native calls. |
| **Initial limited-scope P2-1:** Actual mechanism unavailable | The full initial review withdrew the Kerberos-only counterexample because that policy was not promised. Revision 2 additionally makes permissive Negotiate explicit, exposes provisional versus authoritative mechanism observations, and requires unsupported restrictive callers to refuse before start. | **Withdrawal retained.** No new mechanism-policy knob is required by this Safety verdict. |
| **Initial limited-scope P2-2:** Queued ownership unbounded | The full initial review distinguished caller-owned pending futures from charged native records and withdrew the alleged unbounded native-cleanup sequence. Remediation retains charged launch/I/O ownership, quarantine and waiter-drop rules. | **Withdrawal retained.** Implementation must verify waiter bookkeeping; no queue redesign is inferred. |

No new architectural root cause was identified.

## 2. Changed-range invariant analysis

**Digest input and secret codec.** The added field completes the closed first-message shape while preserving Hello verification before secret transmission. Its token/header/frame bounds and zeroizing ownership are explicit. Digest requires Explicit identity, preserving the inherited helper-only rule. Provider qualification remains gated; the revision does not claim successful Digest authentication.

**Mechanism observation.** Requesting Negotiate explicitly accepts Kerberos or NTLM before any offer. Unresolved/provisional Continue results cannot serve as proof of Kerberos-only use. Token Complete requires authoritative known selection or fails before publication. Native provider strings remain inside the worker and query storage has cleanup ownership. This defeated the attempted misuse of requested package or provisional attributes as final identity. Actual native observation remains a required production qualification test.

**Finish lifecycle.** Finish now covers verified Hello before Begin, negotiation and Complete. Pre-Begin finish initializes no native credential/context handles. Every successful finish still requires confirmed process, Job and thread cleanup; an expired finish can report Pending. This does not weaken permit release or introduce a publication path after cancellation.

**Deadline changes.** Removing the cooperative budget promise introduces no worker-clock reset. Launch, Hello, native processing and delivery still consume the parent deadline. An earlier enclosing D2 cancels; a later D3 cannot extend the original. Existing first-terminal arbitration and late-frame rejection remain applicable.

**Review stops.** The plan now explicitly stops after codec/API secret handling and after native secret ownership/disposal, in addition to the supervision-kernel review. Its cohesive sequence does not bypass the mandatory secret-boundary reviews.

**Retained architecture.** The changed range preserves creation-time Job attachment, handle allowlisting, identity verification before Begin, bounded private framing, charged quarantined records, confirmed cleanup before slot release, route/CBT isolation, terminal authentication rejection and prohibition on credential-driven POST replay. It adds no helper provenance, environment snapshot, fallback executable, network carrier or application protocol.

## 3. Risks and next action

The design is unimplemented. Native observation, secret-codec disposal, installed CLI/Python worker behavior and deterministic cancellation races still require executable acceptance. Real blocked providers, interactive SSO, Digest/EPA, TLS binding lifetime, trust, proxy and Pageant remain deferred qualification gates. Full Windows remains separately NO-GO; this verdict authorizes neither guard removal nor release.

The next action is for the lane owner to combine the required verdicts on this exact tuple. If all pass, proceed through the plan’s standalone implementation and explicit secret-boundary review stops.
