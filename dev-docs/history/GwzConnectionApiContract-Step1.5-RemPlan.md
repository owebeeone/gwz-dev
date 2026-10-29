# Step 1.5 profile and partition: remediation plan (round 1)

Date: 2026-10-25. Lane owner's plan after [Code](GwzConnectionApiContract-Step1.5-ReviewCode.md) (NO-GO: 3 P2, 2 P3) and [State](GwzConnectionApiContract-Step1.5-ReviewState.md) (NO-GO: 3 P2, 5 P3) on gwz-transport `bd7cf4f6` / gwz-core `aaf6efb4` (lane p1-profile). Both pre-committed to GO on bounded remedies. One patch; the same two reviewers re-verdict. No architectural root cause.

Convergences (blind): C-P2-1 = S-P2-1 (the misfit re-request); C-P2-3 = S-P2-2 (the machine-proxy `Debug` encoding exceeds the profile bound); C-P3-1 = S-P3-3 (the §15 record).

## Blocking (five root causes)

| ID | Disposition (lane owner) | Where | Closure rows |
| --- | --- | --- | --- |
| C-P2-1 = S-P2-1 | Record the opening's absolute allocation deadline at `submit`. On a scope misfit: release the untouched lease as `Reusable` (never retire another scope's idle connection; this also closes the implementer's finding 1), then resubmit ONCE with `fresh: true` through `request_until(template, Some(original_deadline))`; a fresh request opens its own connection, so at most one misfit per opening; the `per_user_host + 1` cap stays as a pure guard. Correct the module comments (`instance.rs:14–15`, `operation.rs` `NoFittingConnection`). | `gwz-connection/src/instance.rs:343–372`, `exchange.rs` | Fakes: A, B (other scope), A → B opens its own connection, `closes() == 0`, A's next exchange `reused: true`; with the fake clock a misfit opening times out at its ORIGINAL deadline; saturated scope-a load + waiting scope-b → B served within its deadline, never `NoFittingConnection`; Code's 34-cycle sequence ends with a fitting exchange after at most one misfit. The existing bound row still passes. |
| C-P2-2 | `abandon` identifies the pending by its slot (`Arc::ptr_eq`) or its tag and calls `pool.abandon` with that pending's CURRENT request; `OpenWait` passes the slot alone. | `instance.rs:235–240, 367`, `exchange.rs:383` | Code's sequence: misfit then `drop(wait)` → provider `strong_count == 2`, no live pool request, `world.exchanges == 0` after further pumps, `finish` does not say Complete while work is live; rerun M4: it must now be killed. |
| C-P2-3 = S-P2-2 | Canonical bounded encoding of field 12 (machine proxy) from its bounded inputs: access type, proxy string (≤ 4096), the bypass list text normalized as the rules parse it (≤ 8192), own addresses as octets; field 11 (`no_proxy`) likewise bounded; a static check (test) that the sum of the field maxima is under `MAX_PROFILE_BYTES`, raising `MAX_PROFILE_BYTES` in the crate if needed (record the number); any remaining refusal names the field. | `endpoint_environment.rs:267–269, :187–189`; `operation.rs:93` | Platform-neutral row: `MachineProxy::Named` with an 8192-character bypass list and a 16 KiB `NO_PROXY` → `capture_operation` succeeds; two lists differing in one rule → unequal profiles. |
| S-P2-3 | On OpenSSL builds, resolve the verify paths at capture, read the certificate bytes once (same reader as `verify_paths`), put SHA-256(content) in the canonical bytes beside the paths, and carry the captured certificates in the profile inputs so the instance trusts exactly what was captured. | `endpoint_environment.rs:347–365`, `https_tls.rs` | Linux row (runs on the remote matrix): same `SSL_CERT_FILE` path, changed content → new instance, `reused: false` then `true`; restoring the content reuses the first instance. |

## Non-blocking (same patch)

| ID | Disposition | Where |
| --- | --- | --- |
| C-P3-2 | **Lane owner's ruling:** the auth scope applies to HTTPS keys only; SSH connections are not partitioned by the helper environment (SSH reuse authorization is 1.6's possession proof, contract §7.1). Row: SSH with a changed `GH_TOKEN` reuses. Recorded for the operator in the findings list. | `instance.rs` scope check keyed by scheme; `endpoint_environment_tests.rs` R4 row |
| S-P3-1 | `capture_operation` refuses when `tls.proxy.authorization.is_some()` (reuse design §3). | `endpoint_environment.rs` + unit row |
| S-P3-2 | Document the adapter retention rule on `ConnectCommand.auth` (drop when the connect completes or is cancelled; never used for later exchanges) and add a fake-adapter conformance row (after the connect completes, `strong_count` returns to the binding's count). | `ports.rs:127–137`, `fakes.rs`, tests |
| C-P3-1 = S-P3-3 | Lane owner records contract §15 row at merge (Code's draft text, amended by the rulings above) and the 1.3c obligation: the host-owned HTTPS adapter names routes uniquely per binding, never by the caller's operation name or `Input.session`. | contract §15; plan 1.3c text (lane owner) |
| S-P3-4 | Windows receipt of the `profile_partition` and encoder rows after the C-P2-3 fix (label `p1-5-rem1-20261025-a`); lane owner carries R4's fixture/hosted-like rows into 1.8 by name (plan clerical). | receipt; plan |
| S-P3-5 | README records the committed tuple (`aaf6efb4`/`bd7cf4f6`, two-commit range) and the pin difference; maps "no stale event" to `operation_tests::a_late_event_of_binding_g_is_rejected_by_g_plus_one…` (1.2) and names the missing observation surface. | evidence README |

## Process

One patch in lane p1-profile; gates via the remote runner (`p1-5-rem1-20261025-a`) plus the Windows receipt above; evidence appended to run `2026-10-25-p1-5` (or a `-rem1` run); the lane owner commits (gwz-transport first and pushes it, pin moved in the gwz-core commit); re-verdict by the same two reviewers (`-ReviewCode-2.md`, `-ReviewState-2.md`). Round count after this patch: 1 of 2.
