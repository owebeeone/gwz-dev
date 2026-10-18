# SSH N2 Selected Identity Design Remediation 1 — Consistency-Axis Review

**Review object:** `gwz-core` `35df881b7075d7031082f61e0b99b838341149e1`, `dev-docs/GwzRemoteTransportSshSelectedIdentityDesign.md`, with G1 and ProductionSetup refinements; documentation-only remediation draft dated 2026-09-21  
**Baseline:** root `a9ad12dcafb51d77e7d0fbd28fac97934e070b09`; gwz-core-evidence `e842abf855e58de3c4381855fbc1b8374485a7cd`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed objects were read with `git show` and `git diff`.  
**Date:** 2026-09-21  
**Axis:** Focused Consistency re-verdict on first-fan-out token canonicalization and bounded encrypted-container refusal. Independent, adversarial, read-only. The current other-axis report was not consulted. Filed verbatim by the lane owner.

**Verdict: GO** — both remediation items close on this axis; no new P0–P3 finding.

---

## 0. Evidence base

I read the complete remediation prompt; committed `GwzRemoteTransportSshSelectedIdentityDesign-RemPlan-1.md`; the exact core diff from `120fb9f1379025c66ea00cbdfd5af7f5815859db` to `35df881b7075d7031082f61e0b99b838341149e1`; revised N2 §§1–5 and remediation annotation; revised G1 lines 145–178; the ProductionSetup explicit-identity contract; and the root checkpoint delta.

I retraced the first-fan-out sequence against pinned `gwz-transport/src/pool/allocation.rs`: idle reuse requires `Key`, `Identity`, and `reusable` equality at lines 19–24; a new entry begins nonreusable at lines 71–91; and only a successful connector proof makes it reusable at lines 167–193. Retained source context supplied the worker, setup, resource, and native-memory-authentication boundaries. No build, test, experiment, remote operation, or file write was performed. Both changed documentation ranges passed `git diff --check`.

Start and end tuple verification matched:

| Repository | Commit | Tree |
| --- | --- | --- |
| root | `a9ad12dcafb51d77e7d0fbd28fac97934e070b09` | `3020418cf328e57b4fe08fc6f3e485c0ed3097d4` |
| gwz-core | `35df881b7075d7031082f61e0b99b838341149e1` | `85e46138d5d1faaeaa23dc9bb0d6ab15392c3bd5` |
| gwz-core-evidence | `e842abf855e58de3c4381855fbc1b8374485a7cd` | `568e75ccf28e429e4024ffb72eb27788cc6bef99` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| git2-rs | `ce78628308e11b4e8901d5061602619109bce21a` | `28fc6ad905653db47a7705706e1edea0f39d7054` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |

Member repositories were clean. Root contained only the two explicitly excluded generated remediation prompts.

## 1. Prior-finding closure

| Item | Status | Closure evidence |
| --- | --- | --- |
| Initial Consistency review | N/A | The initial review reported no P0–P3 findings. Its unchanged ownership and lifecycle analysis remains valid. |
| Owner P2-O1: unique concurrent candidate tokens defeat first-fan-out reuse | **Closed** | N2 lines 26–39 now atomically intern live exact-byte, same-`Key` candidates under one token while keeping authentication, cancellation, and deadlines independent. Lines 33–35 require an idle authenticated reusable physical resource before lease. Lines 49–53 state the capacity-one first-fan-out invariant, and tests at lines 266–274 require a native barrier, one physical connection, no early lease, changed-byte isolation, and independent sharer cancellation/failure. |
| Safety P2-1: encrypted native KDF can occupy all helpers | **Closed for Consistency** | N2 lines 113–152 define a framing-only, streaming, fixed-scratch classifier before interning, checkout, DNS, credentials, or native authentication. It explicitly rejects encrypted PKCS#8, traditional encrypted PEM, encrypted OpenSSH, mixed and ambiguous forms without evaluating work factors. Revised G1 lines 159–169 records the compatibility restriction and accounting. Tests at N2 lines 254–265 require extreme-work-factor fixtures, zero native-auth calls, capacity recovery, cleanup truth, and active-stream progress without executing the KDFs. |

## 2. Changed-range invariant analysis

Canonicalizing an unproven candidate does not turn equality into proof. The token names immutable admitted bytes for one endpoint and one pool `Key`; the pool still cannot lease its opening resource, and `reusable` remains false until the connector returns successful identity proof. Each physical connection authenticates independently. A failed or cancelled sharer neither promotes nor poisons the shared entry, while another live request may authenticate the same material. Changed bytes and different keys remain incompatible.

The correction now produces the intended capacity-one sequence: A and B admit the same bytes and receive one identity token; A opens and authenticates C1; B waits while C1 is leased; after authenticated reclaim, the pool’s existing key/identity/reusable checks grant C1 to B. No eviction or second connection is required. The causal test states this observable result directly.

The classifier remains a representation gate rather than a second key parser. It accepts one exact armor block, preserves the original bytes, and examines only enough bounded framing to exclude encryption: label and header rules for traditional PEM, definite DER/version framing for unencrypted PKCS#8, and OpenSSH magic plus `none` cipher/KDF and empty options. Declared lengths are bounded before advancement; the maintained streaming decoder and reserved 256-byte scratch prohibit parameter-sized allocation or a complete decoded copy. Native parsing remains solely responsible for key usability.

G1 expressly permits the newly narrower representation set for both endpoint placements and assigns container refusal to `InvalidRequest` before effects. Its 16 MiB accounting now names classifier capacity; N2 requires the read buffer and scratch reservation before file I/O. The N2a production ceiling remains 600 lines, its test allowance rises from 800 to 900 for the adversarial matrix, and N2b/N3/platform boundaries remain unchanged.

## 3. Risks and next action

This is design acceptance only. Streaming armor/DER/OpenSSH boundary handling, atomic interning, and cancellation/promotion races remain implementation risks and must be demonstrated by the specified red/green tests; no implementation or native execution is certified here.

Proceed to the bounded N2a implementation and aggregate Code/State gate, preserving zero native calls for every encrypted or ambiguous fixture and retaining the capacity-one barrier test for N2b integration.
