# SSH N1 Remediation 1 — State-Axis Re-Review

**Review object:** N1 remediation 1 at `gwz-core` `5ff531cedf244e9e86d3cf33d73559b2c23bf1a9`, controlled by `dev-docs/GwzRemoteTransportSshN1.md`, status “remediation 1 implemented; retained focused re-review pending”  
**Baseline:** root `398ca8350a88723b24660b96f7f00a0fd3e02199`; evidence `e842abf855e58de3c4381855fbc1b8374485a7cd`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Changed range inspected from core `9b3d596f6f230b99ceabf9337d9c17376c2e9762` using immutable `git show`/`git diff`.  
**Date:** 2026-09-21  
**Axis:** State — termination authority, retry legality, trust-byte preservation, admission boundaries, and retained ownership. Independent, adversarial, read-only. The current other-axis review was not consulted. Filed verbatim by the lane owner.

**Verdict: GO** — zero open P0, P1, P2, or P3 State findings.

---

## 0. Evidence base

I read the merged remediation plan, my original State GO report, the complete changed range, revised N1 checkpoint, accepted production-setup boundary, and the new private evidence run.

The material code changes are confined to:

- `ssh_network.rs:39–88`: extraction of `connect_addresses`, live `Control` checks before attempts and after failures, and placement of `handshake` outside the address loop.
- `ssh_network.rs:130–145`: the unchanged pre-resolution line-size admission, which discounts one terminal CR for the documented CRLF measurement.
- `ssh_network.rs:300–324`: complete-line native parsing now preserves every CR byte; the redundant second line-cap check is removed.
- `network.rs:345–369`: LF and CRLF exact line-boundary coverage.
- `network.rs:451–480`: native comparison across no ending, LF, CR, CRLF, and repeated CRLF, with and without comments.
- `network.rs:606–683`: live-Control retries and terminal-Control precedence.

The committed production and test blobs hash to `88adf39d…` and `248254fd…`, matching remediation `inputs.json`; their sizes are exactly 350 and 764 lines. The evidence archive retains the three relevant red states, intermediate failed correction, sshd source-penalty diagnosis, final log, Cargo lock, and pinned `knownhost.c` fingerprint.

I independently ran the permitted Rust 1.95 locked/offline network binary. All 18 tests passed. The committed final evidence log records the wider 91-execution suite passing; I did not rerun that wider suite.

The tuple was verified before and after review and remained exact. Root contained only the two expressly excluded generated round-one prompts; all member repositories were clean.

## 1. Changed-range closure

| Finding under remediation | Counterexample retraced | Corrected state behavior | State status |
|---|---|---|---|
| **Code P2-1 — socket errors impersonated supervisor termination** | With a live `Control`, a first address returning `TimedOut` or `ConnectionAborted` previously ended setup, although those kinds could originate from the socket and another pre-negotiation address remained eligible. | `connect_addresses` now asks the original `Control` after every failed connection. A live control retains the socket error only as the last address failure and advances; cancelled or expired control returns its own `ConnectionAborted` or `TimedOut` before another attempt. The regressions inject both live socket error kinds, then use the opposite socket kind during cancellation/expiry to prove authority comes from `Control`. Exactly one attempt occurs after terminal control. `handshake` remains after successful address selection and outside retry. | **CONFIRMED** |
| **Code P2-2 — CR normalization changed native trust** | Removing a trailing CR could turn a native no-comment mismatch into endpoint trust, creating an unapproved compatibility expansion. | Admission still excludes a terminal CR solely when measuring the 16 KiB line limit. `load_known` passes all CR bytes to native parsing. The native/endpoint matrix compares no ending, LF, CR, CRLF, and repeated CR before LF for both bare and commented key lines. Endpoint success equals native parse-and-match success in every case. Exact LF and CRLF line limits pass at or below 16 KiB and refuse above it. | **CONFIRMED** |

The original State review had no findings. Its unchanged ownership, cancellation, trust-before-credential, and no-post-negotiation-replay conclusions remain supported.

## 2. Invariant analysis

**Termination authority is singular.** Socket error kinds no longer decide whether the whole operation is cancelled or expired. The shared `Control` does. A race after the post-error check is caught either at the next loop check or by the enclosing `Job` arbitration before publication. No terminal control state can start another address attempt.

**Retry remains pre-negotiation only.** The helper returns the first connected socket. `establish_inner` invokes `handshake` afterward and returns its result directly. Handshake, trust, and A2 authentication failures therefore cannot re-enter address selection.

**Trust bytes and admission measurements are separate.** Splitting on LF mirrors native line delivery, while retained CR bytes preserve native token behavior. The sole measurement-time CR exclusion implements the accepted CRLF size rule without changing parser input. Removing the duplicate parser cap prevents a valid exact-boundary CRLF line from being rejected after successful pre-resolution admission.

**The regressions retain causal isolation.** Live retry, terminal-control precedence, parser parity, and cap behavior have distinct deterministic tests. The failed combined CR attempt is not presented as product failure: retained native diagnostics show sshd’s per-source unauthenticated-connection penalty, and the final regression uses small independent fixture groups without retrying assertions or weakening server protection.

No changed code alters loader/resolver retained ownership, the 4 MiB/16 KiB G1 policy, credential ordering, host matching, A2/A3 handoff, public API, or wire behavior.

## 3. Risks and next action

This remains a Unix/macOS isolated N1 result. It does not certify kernel preemption, other platforms, selected-source reconstruction, N2 explicit-key authority, N3 backend attachment, production routing, or capability activation.

The next action is to combine this State GO with the independent Code re-verdict. If that axis closes its two findings, N1 is ready for acceptance and the separately scoped N2 contract can begin.
