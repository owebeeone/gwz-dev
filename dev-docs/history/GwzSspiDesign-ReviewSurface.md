# DRAFT SSPI caller API — SURFACE-AXIS REVIEW

**Review object:** `dev-docs/GwzSspiCallerGuide-DRAFT.md` at root `377c5e29882e353e42dbe75031b175374b213f7f`; DRAFT API contract dated 2026-10-03, without implementation.

**Baseline:** root `377c5e29882e353e42dbe75031b175374b213f7f`; gwz-core `d78a664e3c5a325c6f12be409eb7645c1c1b51d0`; evidence `1beb1d204c824701ddbd033c7f89df9a3561f5e5`. Caller-guide bytes read using `git show` at the exact root SHA. All three HEADs matched at both start and end.

**Date:** 2026-10-03

**Axis:** Surface: cold caller API walkthrough, including installation, authentication, cancellation and cleanup. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks; one P3 finding also remains. I pre-commit to GO on a revision that resolves P2-1 and P3-1 as specified.

---

## 0. Evidence base

Read the complete committed caller guide, lines 1–74:

- Lines 8–13: application-owned installation, worker discovery, upgrade and removal.
- Lines 15–29: ownership and public API signatures.
- Lines 31–38: package selection, identity, targets, Digest inputs and channel binding.
- Lines 40–48: deadlines, cancellation and failure classification.
- Lines 50–63: first-use walkthrough.
- Lines 65–74: dropped futures, retained supervision, capacity accounting and platform behavior.

Inspection commands:

- `git rev-parse HEAD`
- `git -C gwz-core rev-parse HEAD`
- `git -C gwz-core-evidence rev-parse HEAD`
- `git show 377c5e29882e353e42dbe75031b175374b213f7f:dev-docs/GwzSspiCallerGuide-DRAFT.md`
- The same committed document piped through `nl -ba` for exact locations.

The three HEAD checks were repeated at the end and returned the same tuple.

No design, plan, implementation, other review or experimental evidence was read. No files were written, commands mutated state, or builds/tests were run. Because this package is unimplemented and the review is expressly limited to its caller guide, the walkthrough examines the documented API contract rather than executable help or behavior.

## 1. Findings

### [P2-1] TokenStep does not expose the actual negotiated mechanism

**Location:** Caller guide line 24; package input at lines 31–32; token-publication walkthrough at lines 55–58.

**Architectural root cause:** The public result contract specifies token-generation status, attributes and token bytes, but omits an explicit actual-mechanism identity. The requested package and the mechanism that actually generated a token are different facts when the request selects Negotiate.

**Violated invariant:** A caller must be able to identify the actual authentication mechanism and enforce its mechanism gate before publishing a token. Mechanism identity is expressly in scope even while native provider qualification remains deferred.

**Reproduction:** A host constructs an `AuthRequest` with `Package::Negotiate` and calls `step(None)`. Consider two provider outcomes: negotiation selects Kerberos, or negotiation selects NTLM. In both cases the documented result offers Continue/Complete, unspecified “checked attributes,” and secret token bytes. The guide names no actual-mechanism field, query or guarantee that the requested package uniquely determines the mechanism. A host that permits one mechanism but excludes the other cannot implement its publication decision using the stated caller API. The walkthrough instead proceeds directly to sending the token.

**Impact:** The guide permits callers to treat a Negotiate request as sufficient identity for the authentication mechanism. A mechanism-sensitive host must guess, independently decode opaque tokens, or publish before applying its policy. None is a reliable public API contract.

**Required correction:** Specify an owned, typed actual-mechanism result or explicit query, including when its value becomes authoritative. Define behavior while the mechanism is unresolved: the caller must be able to withhold publication, or the library must enforce the allowed-mechanism policy before returning a publishable token. Update the first-use walkthrough to perform this gate. Do not substitute requested Package, native completion status or unnamed attributes for actual mechanism identity.

**Closure/regression test:** Using only the revised guide, trace Negotiate selecting Kerberos, selecting NTLM, and remaining unresolved at an intermediate step. For a host allowing Kerberos only, identify the exact API value or enforced request policy that permits Kerberos publication and prevents NTLM or unresolved publication. No token decoding or undocumented provider knowledge should be necessary.

### [P3-1] Deadline shortening is promised without a caller operation that can perform it

**Location:** Caller guide lines 23–25 and 40–43.

**Architectural root cause:** The deadline contract describes a mutable lifecycle property, but the public API only supplies a deadline to `start`; subsequent conversation operations expose no deadline update.

**Violated invariant:** Every supported caller action needs an explicit public operation and usable ownership contract. A cold caller must not invent how to exercise the promise that a request “may shorten” its initial deadline.

**Reproduction:** Start a conversation with absolute deadline D1. After receiving the first token, the enclosing operation acquires an earlier deadline D2. The next `step` accepts only challenge bytes, and `finish` explicitly uses the original deadline. The guide does not state that `Deadline` is a shared observable object, nor provide a setter or shortening method. The caller cannot locate the promised action or determine whether D2 affects an outstanding native call.

**Impact:** Integrators may build an external cancellation timer, leave the original deadline active, or assume undocumented shared mutation. These choices have different behavior and defeat a consistent deadline contract.

**Required correction:** Document the concrete shortening operation, its ownership and its effect on queued and outstanding work, including rejection of extension. Alternatively, explicitly constrain the statement to choosing a shorter deadline before `start` and state that an active conversation’s deadline is immutable.

**Closure/regression test:** Trace D1 → earlier D2 while a step is pending and an attempted D1 → later D3. The guide must identify the operation and outcome in both cases, or explicitly rule out active shortening and give the supported cancellation path.

## 2. Invariant analysis

The following attacks did not produce findings:

- **Installation and removal:** The guide assigns the library and matching worker to the application’s installation lifecycle. Upgrade replaces both; removal removes both. Dedicated-wheel and internal self-exec arrangements are named, and worker discovery uses an absolute installed path with no PATH or shell fallback.
- **Capacity and bounded ownership:** `max_workers` has an explicit default and range. Quarantined workers retain slots until process exit, Job emptiness and IPC/launch-thread completion are confirmed. Saturation does not imply unbounded launches. Cleanup history is bounded, with explicit Unknown after eviction.
- **Cancellation versus completion:** `cancel` revokes publication immediately but returns a receipt distinguishing Pending from Confirmed cleanup. Timeout similarly stops publication without claiming all provider activity has stopped.
- **Dropped operations:** Dropping start, step, finish or Conversation cancels the conversation; registration determines whether a waiter is removed or supervised storage remains owned. Dropping Supervisor retains supervision for pending cleanup.
- **Lifecycle pairs:** Start has finish/cancel; host lifetime has shutdown; application installation has application removal. Names and placement make these pairs discoverable in the guide.
- **Deadlines:** An explicit absolute monotonic deadline covers launch, native work and delivery without per-round reset. Shutdown independently requires an explicit cleanup deadline. Neither silently acquires a default timeout.
- **Identity and connection binding:** Supervisor captures the primary Windows identity and refuses impersonation. AuthRequest distinguishes current-logon and explicit credentials. The guide requires binding from the actual verified final-origin TLS connection and prohibits moving a conversation between connections.
- **Secret ownership and diagnostic output:** Secret storage is owned and zeroizing, lacks Debug/Clone, and is excluded from failure text. No raw native handles or pointers escape through the caller API. Forced exit does not promise physical secret erasure.
- **Startup and failure:** Missing or mismatched workers fail explicitly. Malformed IPC and provider failures are terminal, without automatic fallback or retry.
- **Native versus remote completion:** Complete means native token generation completed, rather than server acceptance. The walkthrough preserves that distinction.

These are findings about the stated contract, not verification of an implementation. In particular, the phrase “checked attributes” does not independently establish their exact schema or enforcement behavior.

## 3. Risks and next action

Native provider behavior, Digest qualification and platform enforcement remain outside this review’s implementation evidence. Their deferral does not resolve the public mechanism-identity gap.

The next action is to revise the caller guide to expose the actual-mechanism publication gate and make deadline shortening operationally explicit, then repeat these two bounded contract walkthroughs against the settled revision.
