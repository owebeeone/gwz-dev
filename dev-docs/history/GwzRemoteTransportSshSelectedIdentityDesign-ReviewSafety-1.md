# SSH N2 Selected Identity Design Remediation 1 — SAFETY-AXIS REVIEW

**Review object:** `gwz-core` `35df881b7075d7031082f61e0b99b838341149e1`, `dev-docs/GwzRemoteTransportSshSelectedIdentityDesign.md`, DRAFT remediation 1 dated 2026-09-21, with revised G1 and ProductionSetup text  
**Baseline:** root `a9ad12dcafb51d77e7d0fbd28fac97934e070b09`; gwz-core `35df881b7075d7031082f61e0b99b838341149e1`; gwz-core-evidence `e842abf855e58de3c4381855fbc1b8374485a7cd`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed content was read with `git show`/`git diff`.  
**Date:** 2026-09-21  
**Axis:** Safety: focused closure of encrypted native-work admission and changed first-fan-out authority sharing. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — Safety P2-1 and owner P2-O1 are closed; no new P0–P3 findings.

---

## 0. Evidence base

I read the complete remediation prompt, the filed initial Safety report, merged `GwzRemoteTransportSshSelectedIdentityDesign-RemPlan-1.md`, and the exact core diff from `120fb9f1379025c66ea00cbdfd5af7f5815859db` to `35df881b7075d7031082f61e0b99b838341149e1`. I inspected the revised N2 draft §§1–5, Requirements G1, ProductionSetup’s N2 refinement, the root checkpoint change, and the unchanged transport pool allocation/lifecycle state machine.

The encrypted-container counterexample was retraced against the pinned native path established in the initial review: generic PEM memory loading reaches OpenSSL before bad-password reporting, while the Rust/native deadline cannot preempt work inside that call.

No build, test, native experiment, mutation, or report write was performed, as required for this documentation gate. Start and end tuple checks matched exactly. Root contained only the two permitted untracked remediation prompts; all member repositories were clean.

## 1. Prior-finding closure

| Finding | Status | Closure evidence |
| --- | --- | --- |
| Safety P2-1 — unsupported encrypted containers can monopolize shared helpers inside an uninterruptible KDF | **CLOSED** | N2 §2 lines 76–80 reserves the fixed classifier scratch with the bounded read; lines 108–152 require preflight before interning, checkout, DNS, or native authentication. Encrypted PKCS#8 is rejected by label without decoding parameters; traditional encrypted PEM is excluded by rejecting auxiliary headers; OpenSSH admission requires exact `none` cipher/KDF and empty options while ignoring work-factor values. Streaming decode has fixed 256-byte scratch, bounded field lengths, no parameter-sized allocation, and cancellation checks between bounded chunks. Test gate 2, lines 254–265, requires extreme PBKDF2/bcrypt fixtures, zero native-auth calls, saturation recovery, truthful cleanup, and active-stream progress without executing the KDFs. G1 records the resulting representation incompatibility explicitly. |
| Owner P2-O1 — distinct concurrent candidate tokens defeat first-fan-out reuse | **CLOSED** | N2 §1 lines 26–39 atomically interns exact-byte, same-Key live candidates under one token while retaining independent request/connection ownership, cancellation, and deadlines. Lines 33–35 make an authenticated reusable physical resource—not token equality or the registry Proven bit—the lease precondition. This agrees with the pinned pool: allocation only selects an idle matching entry whose `reusable` flag is true, and that flag is set only by a successful connector proof. Lines 49–53 state the capacity-one fan-out invariant; test gate 3, lines 266–274, requires a pre-authentication barrier batch, one physical connection for all successful exchanges, no early lease, changed-byte isolation, and independent failure/cancellation behavior. |

## 2. Invariant analysis

The encrypted-input correction is placed before every authority shortcut. Even bytes equal to a live entry must pass the classifier again, so an old token cannot bypass current representation admission. Exactly one case-sensitive armor block is allowed; mismatched or multiple blocks, non-whitespace exterior data, auxiliary headers, unknown labels, truncation, declared-length overflow, and encrypted or ambiguous framing all fail before native work. The classifier establishes only an unencrypted container shape. Native authentication remains responsible for key mathematics, usability, signing, and authenticated state.

The first-fan-out correction shares compatibility without sharing request state. Exact bytes and the same complete pool `Key` are necessary for canonicalization. Different bytes, different Keys, ambient identity, caller-supplied tokens, and dead entries remain isolated. Each new physical connection still authenticates independently. A failed or cancelled sharer cannot promote, cancel, or poison another request; strong handles preserve the common immutable entry while any independent request or physical resource still owns it. A connecting, failed, disposing, or merely registry-proven entry cannot issue a lease.

The existing deadline, late-result, cleanup-retention, capacity-accounting, no-reopen, host-trust, and no-fallback rules remain intact. The increased N2a test allowance is tied to the explicit adversarial classifier matrix; production and module limits are unchanged. N3 attachment, platform/source qualification, and capability activation remain excluded.

## 3. Risks and next action

Native parse/sign work for admitted unencrypted keys can still be non-preemptible. The revised text states that limit accurately and retains bounded helper count and truthful cleanup ownership rather than claiming hard CPU termination.

Proceed to the bounded N2a implementation gate. Its acceptance must demonstrate the specified zero-native-call encrypted fixtures and fixed-memory classifier, followed by N2b’s capacity-one fan-out and independent cancellation tests. This verdict accepts the corrected design only; it does not certify implementation or activation.
