**GO: every Safety finding is closed, and the new text introduces nothing at P0–P2.**

# Transport execution plan (revision 5, sha256 b225a24b…) — SAFETY-AXIS FINAL CLOSURE CHECK

**Object:** `/Volumes/projects/limbo/gwz-dev/dev-docs/GwzTransportReleaseConvergencePlan.md`, revision 5, 896 lines, sha256 `b225a24bda6ee05be718513fdae02650b547a94f719abacc2074e89e520e1f8a`. It is an uncommitted draft (`??`).
**Date:** 2026-10-25.
**Axis:** Safety. The check is read-only and peer-blind. It is filed verbatim as `GwzTransportReleaseConvergencePlan-ReviewSafety-5.md`.

## 1. Closure table

| Finding | Revision 5 text | Status |
|---|---|---|
| N-1: core-supplied `PreparedOperation` | §1 terms :68–70; §1.1 contract row :100; 2.0(a) :453; 2.2c :557–565. The carrier-backed `admit` carries only an operation reference. The client's owner builds the profile, authentication context and policy from its own invocation and refuses any such field sent by the core. Red row in 2.2c. | Closed |
| N-2: generic HTTP from core-side gwz-git | §1 :71–73; 2.0(a) :457; 2.2c; 2.4 :587–588. Only the typed Git-service Open crosses the carrier. The core-side handle maps `ExchangeSpec` through a closed table of smart-HTTP service paths and refuses anything else. The client still refuses generic HTTP. Rows in 2.2c and 2.4. | Closed |
| P3-a: where admission runs; `snapshot()` | §1 :73–75; 2.0(a) :457–458; 2.2c. Admission runs only at the client's owner, on receipt. `snapshot()` on the core's handle is refused, with a row. | Closed |
| P3-b: consequences of the default of 32 | 6.4 :705–709 records: an sshd with a low `MaxStartups`; `--max-retries 0`; agent keys that require confirmation; HTTPS measured only to 16, so 32 is stated as a chosen default until 3.4's sweep extends it. | Closed |
| P3-c: SSH username default | 2.0(b) :488: `user@` in the allowance, else the client's SSH configuration for that host, else `git`. | Closed |
| P3-d: launch hook placement | 2.0(b) :485: the hook lives in the test harness only, never in the client binary, under 2.1's source check. | Closed |

## 2. New text

No defect at P0–P2.

- **The core-side refusals are defence in depth, not the security control.** The refusals in 2.2c (an unmapped `ExchangeSpec`, a `snapshot()` call) run on the less-trusted side. The control that matters is the client's admission plus its refusal of generic HTTP, and both are stated (§1 :73–74, 2.4 :588). This is consistent with the threat model.
- **One wording point for implementers (not a finding).** §1 :71–72 describes the typed Open as carrying "the identity the client selected". Read that as: the client substitutes its own identity, and refuses any non-empty identity field the core sends. 2.0(c) and the core-named-identity row in 2.3 already enforce this.
- **The deferred-commit `register` adds no new effect path.** It ends as `NeverRegistered` with zero effects when the carrier drops while it is pending, and as a revocation for live bindings (2.2c :561–565). Under the placement design's registration rule, an Open that arrives before registration is accepted cannot create state at the client.

## 3. Object check

`shasum -a 256` at start and end gives `b225a24bda6ee05be718513fdae02650b547a94f719abacc2074e89e520e1f8a`, 896 lines, status `??`, unchanged. Nothing was modified during the review.
