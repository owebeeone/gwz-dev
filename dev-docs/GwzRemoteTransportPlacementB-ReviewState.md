# GWZ Remote Transport Placement B — STATE-AXIS REVIEW

**Review object:** Phase 4 Placement B candidate aggregate, core `d8e23f63..6b9be8a2634700a3f47483beb99a582e7acd20b3`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPlacementB.md`, implemented candidate pending review, dated 2026-09-22.  
**Baseline:** root `ba32b3af11c08f4c6b0a6897b158d5a41ed2465b`; core `6b9be8a2634700a3f47483beb99a582e7acd20b3`; transport `03d3011b3ae9b8205bcf07f7f7862194af114856`; Taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `9980a5d2199df09b055e677e07123f209900ea8c`. Committed sources were read with `git show`/`git diff`.  
**Date:** 2026-09-22  
**Axis:** State—request/stream/physical ownership, preflight atomicity, cancellation, deadlines, retained cleanup, and fail-closed recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified and preserves the otherwise successful lifecycle evidence.

---

## 0. Evidence base

I read the controlling Placement design, Placement B checkpoint, embedding guide, root checkpoint, exact core range, private evidence index/final inputs, and changed host, session, endpoint, SSH worker/pump, backend binding, identity-selection, command-funnel, and test sources.

Focused tests passed using the permitted external candidate target:

- four `transport_host::fault_tests`: carrier loss during open/read, terminal-facts ordering, cancellation and late-terminal isolation;
- `cli_preflight_checks_every_selected_identity_before_any_target_runs`.

That preflight test exercises invocation-supplied identity options directly through `with_transport`; it does not exercise repository-local identities through an ordinary multi-target handler.

All nine tuple HEADs matched at both review boundaries. Member trees remained clean. Only excluded generated current-round reports/prompts and old N2b prompts appeared at root; no current peer report was opened.

## 1. Findings

### [P2-1] Multi-target fetch starts transfers before repository-local identity preflight is complete

**Location:** `gwz-core/src/workspace_ops/handle_fetch.rs:89-109,122-133`; effective repository-local identity resolution occurs later in `src/git/gitbackend/transport_support/identity.rs:283-309`.

**Violated invariant:** Placement design §5 requires every planned remote’s effective identity, including repository-local selections, to pass endpoint preflight before the first Git-host connection or mutation; a last-target failure must leave all participants unchanged.

**Counterexample:** Select root plus one member, set concurrency to one, and give each repository an SSH `origin` with `remote.origin.gwzSshIdentity`. Let the root’s endpoint file exist and the later member’s file be missing. `with_transport` has no invocation identity to check. Target construction records paths/remotes but does not call `validate_remote_identity`; `validate_transport_remotes` checks only override names. The execution loop can therefore fetch the root and update its tracking ref before the member first resolves and rejects its missing endpoint identity.

**Impact:** A condition classified as whole-operation preflight failure can instead produce network effects and partial repository state.

**Required correction:** Freeze and endpoint-check every selected target’s effective URL/identity before dry-run projection or `par_map_per_host`, preserving the frozen selection cache for execution.

**Closure test:** An ordinary two-target `handle_fetch` with valid first and missing last repository-local identities, concurrency one, must perform zero SSH connection attempts and leave both tracking refs unchanged.

**Architectural root:** The operation coordinator validates invocation identities globally but leaves repository-local identity admission inside per-target execution.

### [P2-2] Endpoint policy validation occurs after overflow-prone deadline aggregation

**Location:** `gwz-core/src/git/endpoint/placement_endpoint.rs:244-287,935-938`; codec bounds at `gwz-transport/src/codec/validate.rs:129-137`; later policy validation at `gwz-core/src/git/endpoint/ssh_worker.rs:690-699`.

**Violated invariant:** An admitted but locally unsupported Open must fail closed without killing the independently progressing supervisor or stranding blocking waiters.

**Counterexample:** Send an otherwise valid Open with `allocation_ms = i64::MAX`, `interaction_ms = i64::MAX`, and `connect_ms = 1`. Codec admission accepts those positive values. `accept_open` evaluates their signed sum in `deadline_from_open` before `Endpoint::validate_deadlines` can reject them against configured policy. Overflow checks panic the placement supervisor in test/debug builds; wrapping builds derive a profile-dependent deadline.

**Impact:** The supervisor can disappear while `Session::open` remains blocked in `Wait::get`, leaving the request and session without progress or a typed terminal.

**Required correction:** Validate endpoint deadline policy before queue insertion or arithmetic, and use checked or saturating aggregation even after validation. Unsupported values must produce the defined bounded failure without worker effects.

**Closure test:** Exercise maximum admitted integer values through a bound session; assert a bounded typed failure or documented session closure, zero connector calls, waiter release, and successful subsequent supervisor processing.

**Architectural root:** Local policy admission is deferred behind state creation and deadline derivation.

## 2. Invariant analysis

The remaining attacks held: request/operation metadata is immutable; receiver removal and shutdown invalidate bindings; cancellation wakes open/read waiters; late terminals do not close sibling requests; facts are reported before EOF/error; terminal/control queues retain reserved capacity; setup jobs and physical cleanup remain bounded and counted after logical timeout; local and CLI routes use endpoint-owned credentials without native fallback; callbacks run outside session locks; repository refusal remains typed; last-owner and port loss wake waiters without claiming peer cleanup.

## 3. Risks and next action

Real carrier qualification, platform/selected-source checks, production activation, HTTPS, publication, and release remain correctly deferred.

Correct P2-1 and P2-2 in one bounded remediation, add the two causal regressions, rerun the focused host/backend gates, and request a changed-range State re-verdict.
