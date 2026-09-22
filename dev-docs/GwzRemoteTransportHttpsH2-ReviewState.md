# HTTPS H2 AGGREGATE — STATE-AXIS REVIEW

**Review object:** H2 diff `gwz-core c28d5439ac62526628a07045c4bd8807b337e680..1d3a759acdcc0c977e24199e92f98cf8ca59a129`, including `gwz-core/dev-docs/GwzRemoteTransportHttpsH2.md`; status **implementation in progress, not accepted or activated**, 2026-09-22.  
**Baseline:** root `a4b3ca210a1ce7872a4f7c44ebc5ce37cbded466`; core `1d3a759acdcc0c977e24199e92f98cf8ca59a129`; transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `73b15cc87c1f34667687a9bc815a95aec148b5f9`. Sources were read from these exact committed heads using `git show`/`git diff` and the matching clean working-tree bytes.  
**Date:** 2026-09-22  
**Axis:** State—state machines, races, fail-closed behavior, cancellation, cleanup and recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified. Both are bounded H2 implementation roots; the accepted design already states the required behavior.

---

## 0. Evidence base

I inspected the complete H2 range and controlling H2 document, with focused reads of:

- `src/transport_host/https_endpoint.rs:22-420`, `request.rs:12-158`, `session.rs:226-484`, and `session/driver.rs:74-459`;
- `src/transport_host/mod.rs:88-305`, HTTPS policy/host/embedding/compatibility tests, and `cleanup_tests.rs`;
- `src/git/endpoint/{https_worker,https_remote,shared_reservation,ssh_local}.rs`;
- `src/git/gitbackend/{transport_binding,transport_observations,transport_support/identity}.rs`;
- HTTPS design §§3–8 and §10, transport lifecycle §§4–6, Placement C’s open cleanup finding, and the current checkpoint;
- private H2 evidence README, baseline/final fingerprints, final gate logs and retained failed-attempt logs.

All 20 recorded final source fingerprints match the reviewed bytes. Archived evidence reports host45, endpoint68, observation3, binding2, identity6 and the ordinary core check passing. I did not rerun a binary or build. `git diff --check` was clean.

The nine heads were exact at both review boundaries. Member trees were clean; root contained only the two current H2 prompts and four expressly excluded old N2b prompts.

## 1. Findings

### [P2-1] Cancellation can cross a queued `Opened` and convert a canceled opening into a stream

**Location:** `transport_host/https_endpoint.rs:119-126,221-310,328-365`; `session/driver.rs:218-272,287-298,331-371`. The existing cancellation test at `https_policy_tests.rs:258-319` cancels while headers remain pending and does not cover this boundary.

**Violated invariant:** Transport design `:402-409` requires cancellation while Opening/queued to clean raced allocation and report failure. `Opened` is the transition into Active. A cancellation that wins before the receipt is handed to the mux must not subsequently publish `Opened` or a successful connection observation.

**Counterexample:** A GET preparation completes and `HttpsEndpoint::step` creates `entry.output = Opened`, starts `serve`, and leaves the receipt unsent. In the same driver turn, HTTPS stepping occurs before inbound mux actions. An already queued `Cancel` is therefore accepted afterward. `accept` only cancels the token; it does not invalidate the queued `Opened`. `take_outbound` prioritizes `entry.output`, so it sends `Opened`, and the canceled serving task emits `Failed` later. On the initiator, request cancellation has already completed the blocking waiter with `Cancelled`, but receipt handling still sets `entry.opened = true` and invokes the observation callback before the completed waiter rejects the second result. The equivalent race exists when `Opened` has moved into `state.pending` under mux backpressure.

**Impact:** A canceled opening invents an active-stream transition and connection observation, then terminates it. This contradicts the closed lifecycle grammar and can misstate endpoint/reuse/authentication observations after cancellation.

**Required correction:** Linearize `Opened` publication with cancellation across both the endpoint output slot and driver pending slot. Cancellation before successful mux handoff must suppress/replace `Opened` with the correct `OpenFailed(Cancelled)` and retain cleanup ownership. Cancellation after accepted handoff may use stream `Failed`.

**Closure test:** Place barriers after successful preparation and after `Opened` enters driver pending state. Deliver `Cancel` before handoff, including a `WouldBlock` retry, and require no `Opened` or opened callback, exactly one opening terminal, zero final cleanup work, and an unaffected sibling request.

### [P2-2] The two-Open authentication retry has no causal attempt identity

**Location:** `transport_host/https_endpoint.rs:23-36,150-204,287-305,415-420`; `request.rs:75-157`. The sequential policy test at `https_policy_tests.rs:30-107` does not exercise concurrent transitions.

**Violated invariant:** HTTPS design `:117-133` and H2 `:20-22` require each anonymous 401/404→Gh transition to retain its first typed receipt and use that attempt’s remaining cumulative budget without refill.

**Counterexample:** Two concurrent same-request, same-destination/service anonymous discoveries A and B use the identical string `retry_key`. Suppose A consumes nearly all network allowance before 401 while B consumes little. Each failure inserts its `Budget` into the same `BTreeMap<String, Budget>` slot, so the later completion overwrites the earlier. Their independently scheduled Gh Opens then remove whichever budget remains; the other receives a fresh `budget_for_open`. One retry can borrow its sibling’s allowance and the other can regain a full allowance. A cached or explicit Gh Open can likewise consume an orphaned entry. Concurrent same-URL remotes are expressly supported by the design.

The same missing causal state loses the first typed receipt: `RequestContext` retains only `anonymous_status`, discarding the first failure, stream identity and attempt policy required by the design.

**Impact:** Helper/network effects can continue after the initiating attempt’s budget is exhausted, and diagnostics cannot reconstruct the mandated two-stream transition.

**Required correction:** Bind the remaining budget and complete first receipt to the exact anonymous attempt and its Gh successor. Equivalent canonical routes must not collide or permit another Gh Open to steal the transition. A private host-side serialization of each canonical automatic transition is acceptable if it preserves two real mux Opens and requires no schema change.

**Closure test:** Interleave two same-key transitions with asymmetric consumed budgets and reversed failure/retry ordering. Each Gh attempt must receive only its own remainder; exhausted allowance must not refill, explicit/cached Gh must not steal it, cancellation must retire it, and the complete first typed receipt must remain associated with the final result.

## 2. Invariant analysis

Other attacks held. HTTPS capability mismatch refuses before socket creation. Local and carried Rust/Python paths use existing request IDs, mux receipts and byte streams without a shadow carrier. Scheme dispatch is bounded and alternated. Request cancellation wakes blocked callers, and request/endpoint shutdown retains preparation, serving, helper and physical owners until disposal or an honestly nonzero report.

SSH and HTTPS receive clones of one `Authority`; reservations are acquired before physical connect and released only after acknowledged disposal. Abnormal resource drop leaks capacity fail-closed rather than allowing excess allocation. The added cleanup fixture observes nonzero retained physical work before release and eventual zero, closing Placement C P3-1 at the asserted boundary.

Private-refusal classification, native HTTP/git compatibility, dynamic endpoint observations, command funnels and final evidence limits are represented consistently. The archived intermediate failures are retained without being mislabeled as exact replay or passing evidence. Deferred platform/source, activation, physical-wire and release work is not claimed.

## 3. Risks and next action

Platform trust/provider behavior and selected-source qualification remain deferred as declared and do not affect this verdict. The evidence does not constitute production activation or whole-core qualification.

Correct P2-1’s opening/cancellation handoff and P2-2’s causal retry state, add the exact barrier regressions above, refresh focused H2 evidence, and return the changed range for retained State closure.
