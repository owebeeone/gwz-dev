# SSH N1 Remediation 1 — Code-Axis Re-verdict

**Review object:** `gwz-core` `5ff531cedf244e9e86d3cf33d73559b2c23bf1a9`, remediation of `src/git/endpoint/ssh_network.rs`, `tests/transport_ssh/tests/network.rs`, and the N1 checkpoint documents; dated 2026-09-21  
**Baseline:** root `398ca8350a88723b24660b96f7f00a0fd3e02199`; evidence `e842abf855e58de3c4381855fbc1b8374485a7cd`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable objects with `git show` and `git diff`.  
**Date:** 2026-09-21  
**Axis:** Code — focused closure of original P2-1/P2-2 and changed-range interactions. Independent, adversarial, read-only. The current peer axis was not consulted. Filed verbatim by the lane owner.

**Verdict: GO** — both original P2 findings are closed; no new findings.

---

## 0. Evidence base

I read the complete remediation prompt, the filed original Code report, `GwzRemoteTransportSshN1-RemPlan-1.md`, the exact core diff `9b3d596f6f230b99ceabf9337d9c17376c2e9762..5ff531cedf244e9e86d3cf33d73559b2c23bf1a9`, the corrected production source and focused regressions, the revised N1 and production-setup documents, and the private `network-n1-rem-1` README, inputs, red logs, and final result.

The committed source hashes match the evidence inputs:

- `ssh_network.rs`: `88adf39d93be75f7464b2ffd023a84fad0f59cef507489a423a1583b6c7977a4`
- `network.rs`: `248254fd05000dbb5cb2f99ea854201751a3b070183467b9dab3c200efa28a71`

The archived full Rust 1.95 locked/offline gate records 91 passing executions. I independently ran the permitted focused network binary at the reviewed tuple: all 18 tests passed.

The tuple was unchanged at both boundaries. Final trees were root `13c6fe0c4b2bb7272276eb993834880a3e1a3547`, core `730851b4902264312c70d98f329bc4aefe2dc688`, evidence `568e75ccf28e429e4024ffb72eb27788cc6bef99`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, git2-rs `28fc6ad905653db47a7705706e1edea0f39d7054`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Member repositories were clean; root contained only the two permitted generated remediation prompts.

## 1. Prior-finding closure

| Original finding | Status | Closure evidence |
|---|---|---|
| P2-1 — network error kinds mistaken for Control termination | **Closed** | `ssh_network.rs:66-88` now checks the original `Control` after every failed pre-handshake connection attempt. A live Control retains the socket error and advances; cancellation or deadline returns the Control’s own error. `establish_inner` calls `handshake` only after `connect_addresses` succeeds, so SSH negotiation remains outside the retry loop. Tests at `network.rs:606-683` cover live-Control `TimedOut` and `ConnectionAborted`, cancellation and expiry precedence, exactly one terminal attempt, and second-address success. |
| P2-2 — trailing CR normalization broadened native trust acceptance | **Closed** | `load_known` at `ssh_network.rs:300-324` no longer strips any CR bytes before `KnownHosts::read_str`. The sole `strip_suffix('\r')` remains in pre-resolution line-size measurement, matching the documented exclusion without changing native token bytes. The redundant parse-time cap was removed safely because every call reaches parsing only after immutable-text validation. `network.rs:451-480` compares native and endpoint outcomes for no-comment and commented lines with no ending, LF, CR, CRLF, and repeated CR; the original `\r\r\n` counterexample now refuses exactly when native trust refuses. |

## 2. Changed-range and invariant analysis

The extracted connector loop preserves the original address cap and order. It checks `Control` before each attempt and after each failure. When the last live-Control attempt fails, it returns that socket error; when the supervisor becomes terminal, the post-failure check takes precedence. Concrete production `connect` still checks the same Control around socket creation, nonblocking setup, polling, and completion.

No connection retry was introduced around handshake, trust validation, A2 authentication, or Git exchange. The existing invalid-peer and terminal-authentication regressions still prove that negotiation or authentication failure cannot move to another resolved address.

Trust admission still validates the immutable loaded string for total size, UTF-8, NUL, and physical-line bounds before DNS. Removing the duplicate cap inside `load_known` therefore creates no unchecked production path. Preserving CR bytes restores the pinned native parser’s behavior while retaining the already accepted complete-line and input-cap exceptions. The CRLF boundary tests confirm that terminator exclusion is applied once for measurement without normalizing parser input.

The new connector seam remains private to the module and is re-exported only under the existing test boundary. It adds no public, wire, pool, or routing interface. Production remains exactly 350 lines; the 764-line focused test file is within the revised 770-line ceiling. N2, N3, activation, and platform/source qualification remain excluded.

## 3. Risks and next action

The native evidence remains macOS/Unix-specific, as explicitly scoped. Kernel DNS/file preemption, selected-key admission, backend attachment, production routing, and deferred platform/source qualification are not certified here.

Accept the corrected N1 implementation checkpoint and proceed to the separately gated next package.
