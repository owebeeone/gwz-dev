# DRAFT SSPI caller API — SURFACE-AXIS REVIEW

**Review object:** `dev-docs/GwzSspiCallerGuide-DRAFT.md`, DRAFT revision 2, at root `f2029e4b1739c0214138675dfb16abdb44f6a0d7`, dated 2026-10-03.

**Baseline:** root `f2029e4b1739c0214138675dfb16abdb44f6a0d7`; gwz-core `d78a664e3c5a325c6f12be409eb7645c1c1b51d0`; evidence `1beb1d204c824701ddbd033c7f89df9a3561f5e5`. Committed bytes read with `git show`; caller-guide changes inspected against root `377c5e2`. All three HEADs matched the required tuple at start and end.

**Date:** 2026-10-03

**Axis:** Surface: cold caller API walkthrough and remediation-counterexample retrace. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — both original findings closed; no new findings.

---

## 0. Evidence base

Read the revised committed caller guide in full, lines 1–93, and its complete diff:

- Lines 8–29: installation, API signatures, worker startup and lifecycle.
- Lines 31–54: request inputs, revised Digest validation and mechanism observation.
- Lines 56–66: revised deadline semantics and failures.
- Lines 68–93: first-use walkthrough, cancellation, retained ownership and cleanup.

Commands performed:

- `git rev-parse HEAD`
- `git -C gwz-core rev-parse HEAD`
- `git -C gwz-core-evidence rev-parse HEAD`
- `git show f2029e4b1739c0214138675dfb16abdb44f6a0d7:dev-docs/GwzSspiCallerGuide-DRAFT.md | nl -ba`
- `git diff 377c5e2..f2029e4 -- dev-docs/GwzSspiCallerGuide-DRAFT.md`

The three HEAD checks were repeated at the end with unchanged results. No design, plan, implementation or current peer report was read. No files were written, state mutated, builds run or tests executed.

## 1. Prior-finding closure

| Finding | Counterexample retraced | Evidence and disposition |
|---|---|---|
| P2-1: missing actual mechanism identity | Negotiate selects Kerberos, selects NTLM, or remains unresolved during an intermediate step; host requires Kerberos only. | Lines 45–54 expose typed Unresolved/Selected observation and distinguish provisional from authoritative identity. Selecting Negotiate explicitly permits both mechanisms before the first offer. Kerberos-only callers must reject the unsupported request before start and publish nothing. Intermediate unresolved publication is permitted under that permissive choice; Complete requires authoritative native identity or failure before a publishable token returns. Lines 73–75 incorporate the observation check into the walkthrough. **Closed**, under the previously agreed scope clarification, without introducing a restriction knob. |
| P3-1: undocumented active deadline shortening | Pending step started under D1; enclosing operation changes to earlier D2 or later D3. | Lines 58–60 make the active deadline immutable. Earlier D2 uses the supplied Cancellation signal at D2; later D3 cannot replace D1. The caller no longer needs to invent a deadline-mutation operation. **Closed.** |

No new architectural root cause reached the finding threshold.

## 2. Invariant analysis

The changed ranges withstand the following attacks:

- **Mechanism identity:** Requested Package and raw attributes explicitly do not prove Negotiate selection. Native identity observation is available without token parsing. An unresolved Complete cannot escape as a publishable result.
- **Deadline behavior:** Earlier enclosing deadlines have a documented cancellation path; extension and active mutation are explicitly unavailable. Launch, native work and delivery continue to consume the original absolute deadline.
- **Digest request shape:** Lines 34–40 require explicit credentials, actual method, percent-encoded URI and a nonempty initial challenge. Digest-only fields on other packages, missing inputs and oversized inputs fail before native work. Begin transports the initial inputs on `step(None)` in owned zeroizing storage. Qualification remains deferred.
- **Pre-step cleanup:** Line 25 expressly permits finish immediately after start without initializing native handles, during negotiation, and after Complete. This supplies the normal cleanup path even when authentication never begins.
- **Installation and first use:** The host installs matching library/worker components, supplies an absolute trusted worker path, constructs connection-bound inputs, starts, checks mechanism observation, exchanges tokens, then finishes or cancels. Upgrade/removal retain the application-owned worker lifecycle.
- **Cancellation and retained ownership:** Pending cleanup remains distinct from confirmation. Dropped operations revoke the conversation while supervision retains storage. Quarantined workers retain capacity until all named exit conditions are confirmed.
- **Completion and shutdown:** Native Complete remains distinct from remote success. Shutdown closes admission, returns outstanding IDs at its explicit deadline and preserves subsequent reaping. Bounded tombstone eviction yields Unknown rather than false confirmation.
- **Defaults and diagnostics:** The worker limit retains its explicit default and range; authentication and shutdown require caller deadlines. Failures exclude secret and identity text.

These are successful attacks against the documented contract, not implementation or native-provider qualification results.

## 3. Risks and next action

The package remains unimplemented. Native mechanism observation, containment, zeroization and provider behavior still require implementation verification and the deferred qualification work.

Accept this caller-guide revision for the Surface axis.
