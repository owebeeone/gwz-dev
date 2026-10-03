# HTTPS SSPI composition proposal — Safety-AXIS REVIEW

**Review object:** `dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md` at `721e07d65aa78a8bd79d41dae86ad99629a7aefc`. Documents-only DRAFT proposal dated 2026-10-04; implementation acceptance is outside this verdict.

**Baseline:**

| Repository | Reviewed HEAD |
|---|---|
| gwz-dev | `721e07d65aa78a8bd79d41dae86ad99629a7aefc` |
| gwz-core | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-sspi | `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-transport | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |

Committed documents were read with `git show`; reference source was inspected at the verified, unchanged member HEADs.

**Date:** 2026-10-04

**Axis:** Safety — degraded and mixed-version behavior, disclosure, irreversible effects, concurrency, retained cleanup and scope. Independent, adversarial, read-only. Other axes run separately; nothing here relies on their current reports. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2 or P3 findings. This verdict accepts the proposed contract within its stated scope. It does not accept an implementation or authorize Windows activation.

---

## 0. Evidence base

This is the first completed Safety review of this composition proposal. The previous interrupted attempt produced no report or verdict and supplies no acceptance. The corrected whole proposal was reviewed, rather than treating another axis’s remediation as sufficient.

The exact six HEADs were checked at the start and end using `git rev-parse HEAD` and `git -C <member> rev-parse HEAD`. Every result matched the baseline table. Inspection used `git show`, `git status`, `git diff`, `rg`, `sed` and numbered source reads. No files were modified, and no builds, tests, compiler probes, native execution, remote commands or Git mutations were performed.

Evidence inspected:

- The complete controlling composition DRAFT, §§1–9, including its current timeout-zero disposition, caller capture contract, native facts validity, compatibility rules and proposed implementation gates.
- The complete proposed `GwzSspiHttpsCallerGuide-DRAFT.md`, and the existing `GwzSspiCallerGuide-DRAFT.md`.
- `GwzSspiDesign.md` revision 2, particularly identity, deadline, publication and cleanup contracts; `GwzSspiPlan.md`, including step 4; and `GwzSspiHostsCheckpoint.md`, including the boundary between installed packaging and future HTTP composition.
- Core retry policy sections defining first-request-byte eligibility, aggregate/stall timeouts and zero behavior.
- Credential-helper timing and configuration-view amendments, including M4/M10 provenance, configuration filtering, secret ownership and retained child cleanup; SSH helper-clock rules.
- Windows parity design §§7–9, covering source selection, native negotiation, final-origin CBT, exclusive connection ownership and operation/route authentication scope.
- Relevant release-amendment status and operator decisions, including OD10 and OD16. The entire release amendment was not treated as newly reviewed implementation evidence.
- `AgentProcessRules.md` review, severity and lifecycle provisions, and `GwzProcessOptimization.md`, including the adopted review-granularity amendment.
- The permitted prior-round composition remediation plan. No current Consistency or Surface report or prompt was inspected.

Reference source reads included:

- Core request admission and `open_https_recording`; HTTPS preparation, budget, credential ownership and service handling; connection establishment and final-origin TLS handshake; pool ownership; operation dependencies; and the closed first-connect retry classifier.
- SSPI supervisor API and Windows identity capture/verification; `values.rs:160–218`; `worker/session.rs:1–85`; `worker/windows/observation.rs:1–110`; and `worker/windows/conversation.rs:130–290`.
- Existing transport policy/method enums, facts shape, profiles, capability validation and reused-credential routing.
- Actual CLI/Python entry and platform-guard boundaries cited by the proposal.

Untracked SSH prompts, route-mapping draft, core bug report, timeout evidence and generated review prompts were excluded. Source inspection establishes the current integration seams and feasibility constraints; it supplies no executed proof of the future bridge.

## 2. Invariant analysis

**One finite native deadline, without borrowing another allowance.**  
I attacked the proposal with helper work crossing the deadline, reused leases, discovery redirects, capacity waiting, eight native rounds, and a token or HTTP response arriving exactly at expiry. §§1 and 4 require the positive logical-Open deadline to be captured before first checkout/adoption and retained through those transitions. Neither a 401, a new Start, nor helper work grants another native allowance. Publication checks use current monotonic time, so an earlier completed read is insufficient to publish after expiry.

The delegated timeout-zero disposition is concrete: native selection refuses before native credentials or Authorization publication when no finite setup deadline exists. It does not substitute the allocation deadline, helper allowance or a fresh 30-second timer. Anonymous, existing Basic and SSH zero behavior remains unchanged. Existing active HTTP I/O accounting remains separate; the new deadline does not disable its observer or erase its accounting. Helper timeout provenance is preserved rather than assigning every deadline failure a helper cause.

The proposal openly amends the positive setup-clock domain. It does not claim that today’s physical-connect timer already bounds authentication. Eligible pre-byte first-connect retries remain governed by the existing classifier and retry policy; authentication failure cannot create another eligible attempt.

**Originating identity survives fanout without executor recapture.**  
I considered Python submission, CLI runtime construction, concurrent member Opens, dropping the caller capability before Start completes, foreign Supervisor use, original-thread exit and shutdown racing capture. §6 places capture on the original entry before handoff and retains it with the operation. Each Start takes an independent private reference; it does not duplicate identity from the endpoint thread or consume the operation’s sole capture.

Context binding rejects a foreign capture before registration or credential effects. Launch rechecks the retained original thread, impersonation state and primary identity. Capture does not reserve worker capacity or keep admission open. Dropping it cannot revoke an already owned reference, release another conversation’s permit or reopen a closed Supervisor. The synchronous metadata and last-handle disposal exceptions are explicitly bounded in scope, not represented as hard-time-bound operations.

The API has complete disposal paths through Drop, cancel, finish, cleanup observation and shutdown. Its owned future requirement prevents borrowed host state from becoming a hidden lifetime dependency.

**Native unavailability does not break ordinary transport paths.**  
Missing packaging metadata, unavailable workers, fingerprint mismatch or caller-capture failure is retained as an operation-owned native refusal. §§6–7 do not require successful native setup before anonymous, Basic or SSH work. Once native authentication is selected, refusal precedes effects; no PATH lookup, endpoint-thread recapture or native-Git fallback repairs the failure by changing the trust boundary.

Source selection is fixed after selection. Helper timeout/cancellation and explicit rejection are terminal. Digest remains a pre-provider refusal rather than silently selecting Basic. These rules prevent degraded operation from turning into an additional credential offer.

**Mechanism authority, native completion and remote acceptance remain distinct.**  
The corrected §7 survives a direct source counterexample. `worker/windows/conversation.rs` reports direct NTLM as Selected and authoritative; `worker/session.rs` independently permits a Continue token status. Negotiate authority is obtained from the native negotiation observation, not inferred from the token status.

The proposed facts therefore preserve an authoritative Continue without falsely labelling negotiation complete. Core must separately retain native Complete. A successful HTTP response before that completion cannot satisfy authenticated success. Cancellation or expiry preserves the actual mechanism observation and leaves remote authentication unknown unless an actual rejection was observed. Native failure alone cannot manufacture rejection; Complete alone cannot manufacture success. `credential_offered` requires actual Authorization publication.

The corresponding planned cases cover authoritative NTLM Continue, cancellation/expiry, early remote response, unresolved/provisional Negotiate and rejection. Those are implementation obligations, not executed results of this review.

**Mixed-version admission fails before native effects.**  
I considered profile 1, an old closed-enum decoder, old-only policy offers, a binding that omits the selected native policy, and a peer that advertises capability but cannot consume native facts. §7 requires HTTPS, the exact acknowledged native policy and profile 2 or 3 before native admission. Profile 1 filters native policies; native Open without acknowledgement refuses before effects.

Old-only offers retain old Bound and facts shapes. An old decoder may reject new enum tags during bootstrap; the document accurately describes this as protocol refusal rather than promising automatic downgrade. A lying peer causes binding closure without replay or fallback. Facts remain bounded nonsecret enums and cannot carry identities, targets, CBT or tokens as diagnostics.

**CBT and native state remain attached to one physical origin.**  
I attacked proxy TLS confusion, equal CBT bytes on a replacement socket, authenticated redirects and concurrent socket use. §§5 and 8 require CBT from the verified final-origin TLS connection before type erasure, bound to final origin and physical generation. Proxy TLS cannot supply that binding. Equal CBT bytes do not establish lease identity.

All challenge rounds retain one exclusive lease and one conversation. Replacement requires new CBT and a fresh conversation, and is permitted only where existing retry eligibility allows it. Authenticated redirects refuse. The inherited operation/route authentication scope prevents a completed credential-bearing connection from becoming an anonymous or cross-operation pool resource.

**Terminal revocation prevents reuse and post-effect replay.**  
I considered cancellation winning before a late token, late HTTP success after route retirement, Finish timing out after remote acceptance, and connection cleanup outliving native cleanup. §8 revokes publication, discards the lease and retires the authenticated scope on failure, cancellation or expiry. Both cleanup owners remain retained and the operation dependency persists until both are accounted for.

Remote acceptance may be reported separately from cleanup, but Pending or failed Finish cannot publish a reusable socket or confirmed cleanup. Late results cannot overwrite the saved terminal outcome. Existing supervision retains pending native capacity; the composition does not add a second process owner.

Receive-pack retains `Effect::Possible` before POST submission. A subsequent 401 cannot trigger authentication replay, alternate credentials or another scheme. Fetch/body failures remain outside setup retry. The new clock’s name does not widen the retry classifier.

**Disclosure and activation scope remain bounded.**  
Core/library-owned credential, challenge, CBT and Authorization storage must be initialized, bounded and wiped. The proposal explicitly distinguishes those guarantees from Hyper/native-tls dependency copies and makes no whole-stack erasure claim. Diagnostics remain nonsecret.

§9 keeps production guards unchanged and allows only private, explicitly enclosed test visibility. Source checks cannot stand in for Windows authentication evidence. The implementation ceiling, structural stop triggers and later qualification gates do not authorize dependencies, new process owners or Windows activation.

## 3. Risks and next action

The remaining risks require implementation and platform evidence: correct deadline/publication arbitration, actual host capture handoff, truthful facts production, final-origin CBT extraction, dependency-buffer lifetime and retained cleanup across both owners. The DRAFT names these obligations and meaningful failure schedules. Their future execution is not presumed by this GO.

OD16’s unrestricted current-logon policy remains an explicitly accepted policy hazard; this review does not reinterpret it as zone-restricted behavior. Provider/EPA, Digest, proxy/trust, installed Windows and physical-wire qualification remain deferred.

The next action is to file this completed Safety verdict against the exact tuple and apply the required independent design gate before beginning the bounded core HTTPS implementation. Windows activation remains separately unauthorized.
