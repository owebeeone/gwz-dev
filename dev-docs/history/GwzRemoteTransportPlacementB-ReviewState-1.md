# GWZ Remote Transport Placement B remediation 1 — STATE-AXIS REVIEW

**Review object:** Phase 4 Placement B correction 1, core range `6b9be8a2634700a3f47483beb99a582e7acd20b3..4f06384397a67d3dcae4856a93fd032499fda5dc`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPlacementB.md`; implemented candidate pending re-review, dated 2026-09-22.  
**Baseline:** root `93334058352828b1069b198d795c5860a395dc81`; core `4f06384397a67d3dcae4856a93fd032499fda5dc`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; Taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `a2180f71f9f4f16ecc639eecd25125b980ea54f3`. Committed sources were read with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** State—preflight atomicity, deadline admission, request/physical ownership, timeout and retained cleanup. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — both prior P2 findings are closed. No new P0–P3 finding or architectural root was found in the changed range.

---

## 0. Evidence base

I read the merged remediation plan, my initial State report, the exact core diff, corrected Placement B checkpoint, and the affected fetch, identity-resolution, placement-endpoint, SSH-worker, session-fault, and endpoint-check sources. Key locations were:

- `src/workspace_ops/handle_fetch.rs:101-120` and `src/git/gitbackend/transport_support/identity.rs:215-248`;
- `src/git/endpoint/placement_endpoint.rs:244-269,304-355,466-555,982-989`;
- `src/git/endpoint/ssh_worker.rs:612-660,669-724`;
- `src/transport_host/fetch_preflight_tests.rs:1-145`;
- `src/transport_host/fault_tests.rs:396-486`;
- `tests/transport_ssh/support/placement_checks.rs:45-195`.

I also checked the correction evidence README, input hashes, causal red logs, and final green logs at evidence commit `a2180f71`.

Permitted focused tests passed:

- ordinary multi-target fetch preflight regression: 1 passed;
- oversized deadlines through a bound session: 1 passed;
- isolated endpoint checks for retained physical cleanup, FIFO admission, and unsupported deadline policy: 3 passed.

All nine tuple revisions matched at both review boundaries. Member trees remained clean. Root contained only the excluded generated prompts/reports and four old N2b prompts; no current peer report was read.

## 1. Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| P2-1 — multi-target fetch could mutate an early repository before rejecting a later repository-local identity | **Closed** | `handle_fetch` now validates every non-refused target having both remote and branch before dry-run projection or fan-out. Resolution uses the repository/remote/URL-keyed cache, and execution rechecks the selected endpoint file without rereading a changed repository selection. The causal ordinary-handler test advances the upstream, configures a valid first key and missing last key, then proves rejection with both tracking refs unchanged and no transport attempt observation. |
| P2-2 — deadline aggregation could overflow before endpoint policy validation | **Closed** | `accept_open` now invokes endpoint policy validation before request insertion, queue ownership, or deadline derivation. Unsupported values produce typed `OpenFailed(InvalidRequest, Effect::None)`. Aggregation uses checked arithmetic and `Instant::checked_add`, with saturating defensive derivation after validation. The bound-session regression submits codec-admitted maximum values, proves waiter release and supervisor survival, then completes a subsequent request. The direct test proves no open job, queued open, or native pending request was created. |

## 2. Invariant analysis

The corrected fetch path establishes whole-operation identity admission before any contact or tracking-ref mutation. Refused and no-upstream rows remain noncontactable and are excluded consistently with `fetch_one`; every target that can reach `contact` is preflighted.

Invalid deadline policy now fails in the logical admission layer before physical ownership. The terminal is removable through the normal outbound path, while valid deadlines retain bounded queue semantics.

The adjacent identity-check correction preserves the ownership split: logical timeout publishes one terminal immediately, marks the request terminal, cancels the job, and retains the `CheckJob` until physical disposal. Pending accounting continues to include that job after terminal delivery; later completion cannot emit a duplicate. Nonblocking open followed by descriptor metadata rejects FIFOs without allowing filesystem admission to strand the supervisor. The three isolated endpoint checks directly exercise these interactions.

## 3. Risks and next action

Real supplied-carrier qualification, platform and selected-source qualification, production activation, HTTPS, publication, and release remain explicitly deferred and are not implied by this GO.

Accept Placement B correction 1 for the State axis and proceed to the separately gated deferred work.
