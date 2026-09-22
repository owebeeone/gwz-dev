# HTTPS H2 host and command integration — CODE-AXIS REVIEW

**Review object:** H2 aggregate diff `gwz-core` `c28d5439ac62526628a07045c4bd8807b337e680..1d3a759acdcc0c977e24199e92f98cf8ca59a129`, including `dev-docs/GwzRemoteTransportHttpsH2.md`; implementation candidate, not activated, reviewed 2026-09-22  
**Baseline:** root `a4b3ca210a1ce7872a4f7c44ebc5ce37cbded466`; `gwz-core` `1d3a759acdcc0c977e24199e92f98cf8ca59a129`; `gwz-transport` `aa40936d0805e8cb60f8027615abe20d4f2045e4`; `taut` `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; `gwz-cli` `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; `gwz-py` `d07d55dacb1725d9306be9c04d157ac29a78e000`; `git2-rs` `ce78628308e11b4e8901d5061602619109bce21a`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `73b15cc87c1f34667687a9bc815a95aec148b5f9`. Committed objects were read with `git show`; all nine HEADs matched at both review boundaries.  
**Date:** 2026-09-22  
**Axis:** Code — architecture, interfaces, call graphs, ownership, compatibility, and failure reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 correctness findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified, with focused regressions.

---

## 0. Evidence base

I read the complete H2 diff and controlling H2 document; HTTPS design §§4, 6–8; the retained Placement C and H1 integration contracts; and the exact committed implementations in:

- `transport_host/{mod,request,session,session/driver,https_endpoint}.rs`
- `git/gitbackend/{transport_binding,transport_observations,transport_support/identity}.rs`
- `git/endpoint/{https_remote,https_worker,shared_reservation,ssh_local}.rs`
- the new host, policy, compatibility, binding, observation, embedding, and cleanup tests.

I also inspected the pinned `gwz-transport` mux and binding admission paths and the private H2 evidence README, source fingerprints, and final logs. The archived results record endpoint 68, host 45, observation 3, binding 2, identity 6, the default check, conditional-boundary inspection, and archive verification passing. No command or build was run during this review.

## 1. Findings

### [P2-1] Retry budgets are correlated by destination, not by the anonymous attempt that consumed them

`src/transport_host/https_endpoint.rs:35-39,189-198,287-305,415-420` stores one `Budget` in `Operation.retries` under only `(service, host, port, path)`. The stream ID and callback/attempt owner are absent. `RequestContext` is cloneable, H2 supports concurrent streams under one registered request, and per-remote callbacks can issue the same discovery URL concurrently.

Reproduction: two same-request routes A and B read the initial anonymous policy and open the same advertisement. Both receive 401. During one endpoint step their distinct remaining budgets overwrite the same map entry. The first delivered failure starts its Gh Open and removes whichever budget was stored last; the other retry then receives a fresh budget through `unwrap_or_else`. Thus one retry can inherit another attempt’s shorter allowance while the other replenishes allocation/helper/connect/network/cleanup allowances.

This violates `GwzRemoteTransportHttpsDesign.md:277-280` and H2’s claim that the two real mux Opens share the originating attempt’s remaining budget. It can cause a spurious timeout or allow authentication work beyond the original operation budget.

Correlate each retained budget with the exact anonymous failure and its continuation. A bounded per-key serialization spanning anonymous failure through Gh continuation is sufficient; an equivalent attempt-owned correlation is also acceptable. Add a regression with two overlapping same-request, same-destination anonymous discoveries whose first attempts consume distinguishable budgets; each Gh continuation must receive its own remainder, and neither may receive a fresh allowance.

### [P2-2] Local cancellation publishes `Effect::Possible` before HTTPS has established any possible publication effect

`src/transport_host/session.rs:354-390` immediately completes every not-yet-open stream waiter with `Cancelled / Effect::Possible`. The `Entry` created in `src/transport_host/session/driver.rs:76-149` retains neither scheme nor service, so cancellation cannot apply HTTPS’s defined effect boundary. This local result wins the `Wait`; the later endpoint-owned terminal cannot correct it.

A request canceled while `open_https` is waiting on anonymous discovery, helper lookup, connection setup, or headers therefore returns a `HttpsOpenFailure` carrying `Effect::Possible`. Discovery and upload-pack must always be `None`; even receive-pack preparation remains `None` until the POST is handed to the network. The existing cancellation test checks only that an error occurs, so it accepts the false effect.

This violates the HTTPS design’s failure contract and can send callers into ambiguous-publication recovery despite no push bytes having been offered.

Retain sufficient scheme/service/phase information to classify local pre-Opened cancellation, or preserve the endpoint’s authoritative terminal while still guaranteeing a bounded wake. Add focused tests that cancel a pending discovery Open and a receive-pack Open blocked before send, asserting `Cancelled`, `Effect::None`, and zero POST request bytes.

## 2. Invariant analysis

The following attacks held:

- HTTPS uses the existing registration, mux, request attachment, and negotiated binding. Unsupported HTTPS is rejected by binding before socket activity.
- Endpoint `Opened` receipts and both stream halves use the negotiated limits.
- Scheme and per-stream dispatch are bounded and round-robin; HTTPS does not introduce a shadow carrier.
- SSH and HTTPS physical creation use the same reservation authority, retained through actual disposal.
- HTTPS interception is per callback, malformed HTTPS cannot fall through to native libgit2, and native HTTP/git compatibility remains intentionally separate.
- POST is never placed into the discovery retry path, and receive-pack marks possible effect before network handoff.
- Final repository-refusal suppression requires the scheme-specific receipt; helper, authentication, malformed-response, and ambiguous POST failures remain visible.
- Request cancellation retains task and physical cleanup ownership; Rust/Python carried-message tests exercise the same mux path.

The remaining-budget and pre-Opened effect invariants fail for the concrete interleavings above.

## 3. Risks and next action

Platform and selected-source qualification, public constructor activation, external accounts, system TLS/proxy parity, physical carrier work, and release remain valid deferrals; this verdict does not expand those gates.

Correct P2-1 and P2-2 in one bounded patch, add the two focused counterexample regressions, refresh the H2 evidence tuple, and request a changed-range Code re-verdict.
