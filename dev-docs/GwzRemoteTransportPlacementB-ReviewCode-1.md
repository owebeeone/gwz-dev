# Placement B correction 1 — CODE-AXIS REVIEW

**Review object:** Phase 4 Placement B correction 1 at gwz-core `4f06384397a67d3dcae4856a93fd032499fda5dc`, controlled by `dev-docs/GwzRemoteTransportPlacementB.md`; candidate pending retained re-review, 2026-09-22  
**Baseline:** root `93334058352828b1069b198d795c5860a395dc81`; core `4f06384397a67d3dcae4856a93fd032499fda5dc`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `a2180f71f9f4f16ecc639eecd25125b980ea54f3`. Sources were read from immutable objects with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** Code—focused original-counterexample closure and changed-range architecture, interface, call-graph, and failure-path review. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — both original P2 findings are closed; no new P0–P3 finding or architectural root was found in the correction range.

---

## 0. Evidence base

I read the merged remediation plan, my filed initial report, and the complete core correction range `6b9be8a2634700a3f47483beb99a582e7acd20b3..4f06384397a67d3dcae4856a93fd032499fda5dc`. Inspection covered the fetch handler and new real SSH regression, endpoint check lifecycle, nonblocking identity admission, deadline validation/arithmetic, bound-session regression, and updated Placement B checkpoint.

I independently ran the two permitted focused gates:

- Placement endpoint `check_tests`: 3 passed, covering blocked-job logical timeout, FIFO rejection, and unsupported deadline policy.
- `fetch_checks_later_repo_identity_before_any_tracking_ref_moves`: 1 passed against the prepared candidate backend.

The full commit/tree tuple matched at both review boundaries. All member worktrees remained clean; root contained only allowed generated review artifacts. No current peer prompt or report was read.

## 1. Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| P2-1 — fetch omitted effective per-repository identity preflight | **Closed** | `handle_fetch.rs:101-120` now validates the complete remote-name set and then calls `validate_remote_identity(path, remote, false)` for every contactable target before dry-run return, event emission, or fan-out. Targets that cannot contact a remote are correctly excluded. The new two-member real SSH regression places a valid configured identity first and a missing configured identity last, advances the server, and proves the handler returns an error with both tracking refs unchanged and no transport attempt observation. The focused test passed. |
| P2-2 — identity check could wait forever for physical disposal | **Closed** | `ssh_worker.rs:638-661` opens candidate identity files with `O_NONBLOCK` before checking the opened object’s regular-file type, so a FIFO without a writer cannot strand admission. `placement_endpoint.rs:466-549` now publishes exactly one logical timeout terminal at the endpoint deadline, cancels the job, and retains the physical job in the check ledger until disposal. Late disposal cannot publish a duplicate terminal. Both the FIFO and injected blocked-job counterexamples passed; the latter also proves pending work remains charged until release. |

## 2. Changed-range and invariant analysis

The fetch correction preserves the existing selection semantics: only targets that would actually call `fetch` undergo repository identity validation, while detached, unborn, refused, or remote-less targets retain their established `NoUpstream` or refusal behavior. Validation occurs on the scoped backend, so CLI paths remain endpoint-owned and cached resolution remains frozen for the operation. Actual Open still rechecks the selected file.

The endpoint correction cleanly separates logical completion from physical ownership. Removing the terminal request after delivery does not discard the blocked job: `checks` continues contributing to `pending_request` and cleanup reporting until `poll_disposed` succeeds. Cancellation before expiry still emits its existing `Cancelled` terminal, and the terminal-state guard prevents timeout duplication.

The adjacent deadline correction validates endpoint policy before queue ownership and duration arithmetic. Checked aggregation and checked `Instant` construction reject unrepresentable positive deadlines; later saturating derivation cannot wrap. Invalid policy produces a typed per-stream `OpenFailed(InvalidRequest)` without queued/native work, and the bound-session regression proves the same binding subsequently progresses. No public API, schema, routing default, credential ownership, or production configuration changed.

## 3. Risks and next action

Real supplied-carrier qualification, carrier construction, platform and selected-source qualification, HTTPS, production activation, publication, and release remain explicit later gates. This verdict accepts the corrected Placement B candidate only; it does not certify those deferred outcomes or claim a whole-core test pass. The next action is to record the aggregate acceptance decision and proceed to the separately gated Placement C work.
