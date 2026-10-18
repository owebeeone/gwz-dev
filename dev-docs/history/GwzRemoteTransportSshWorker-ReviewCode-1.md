# Shared SSH Worker Remediation 1 — Code-Axis Re-verdict

**Review object:** remediation diff `59ac059c8bfe4052d8a5a16d821f314a3985bcc8..073395b5a265c4d2a60265470cd5ba173243cc64`; controlling DRAFT `gwz-core/dev-docs/GwzRemoteTransportSshWorker.md`; merged `dev-docs/GwzRemoteTransportSshWorker-RemPlan-1.md`  
**Baseline:** root `60623f2a10895dd970595c5b818a68e5900a72a3`; core `073395b5a265c4d2a60265470cd5ba173243cc64`; evidence `5ea96433628b3bec8a8365525f8ee314d01cce99`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Focused Code re-verdict on the original SCP compatibility finding and changed-range interactions. Independent, adversarial, read-only. Nothing here relies on the current parallel review. Filed verbatim by the lane owner.

**Verdict: GO** — Code P2-1 is closed; no new P0–P3 finding arose from the correction.

---

## 0. Evidence base

I read the merged remediation plan, complete six-file diff, revised DRAFT, corrected parser and worker, destination/Route regressions, deterministic queue support, private worker-rem-1 record, and pinned libgit2 SCP vectors.

All 24 evidence SHA-256 entries match the committed core blobs. I reran the permitted Rust 1.95 offline locked suite: all 38 executions passed, with the ignored fake-agent child invoked by its passing parent. This included the SCP closure vectors, Route side-effect checks, deterministic queue-expiry test in both fixture crates, and unchanged native composition tests.

The tuple was stable at review end. Trees were root `ef5b2d121583d56da82db2eda854625bb0bb8c9d`, core `71d10073441a5994ee02dedbb7fd2c01256df93c`, evidence `da2e77bc072bd7cbd6758a5df5eba12a85aab118`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, git2-rs `28fc6ad905653db47a7705706e1edea0f39d7054`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Only the two permitted root re-verdict prompts were untracked.

## 2. Invariant analysis

`scp_colon` now tracks balanced bracket depth, finds only the path-separating colon, and leaves local paths containing an earlier slash or backslash on their existing route. `scp_group` distinguishes an outer authority group from an actual bracketed IPv6 literal. `host_port_parts` consequently accepts the pinned bracketed hostname, IPv4, IPv6, username, and effective-port forms while retaining numeric range checks and fixed redacted failures.

Root operands now remain data. Windows drive paths remain local. Malformed brackets, empty operands, invalid ports, password-bearing URLs, control characters, and unsupported schemes still refuse before identity resolution or connector activity.

The worker clock seam changes only the source of monotonic milliseconds supplied to the existing loop; production still captures one `Instant`. The queued branch now separates actual shutdown from expiry/caller cancellation. Permit release, subsequent worker liveness, connector dispatch, and exact/past deadline behavior are covered deterministically. No pool, pump, lease, public API, wire, dependency-activation, or native credential boundary changed.

## 3. Risks and next action

Production credentials, callback activation, platform parity, and selected-source qualification remain outside this correction. The next action is to accept the remediated shared-worker checkpoint and proceed to the separately scoped credential/setup boundary.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| Code P2-1 | Retain pinned libgit2 bracketed SCP authority, port, and root-path forms without weakening local-path or malformed-input refusal. | Re-traced `[example.com]:/resource`, `[example.com:42]:/resource`, `[user@example.com:42]:/resource`, `[192.168.99.88]:/resource`, IPv6 variants, and `example.com:/`; all produce the expected canonical key/path. Route tests prove accepted forms reach identity/connector processing while malformed and Windows-local forms do not. | **Closed** |

## Changed-range analysis

The correction modifies only the destination parser, worker expiry classification/clock injection, their tests, and checkpoint text. Parser expansion remains bounded to pinned native forms and does not add fallback or raw-URL diagnostics. The worker correction preserves scheduling and ownership while making queued timeout classification accurate. The new support file is test-only and introduces no production dependency. No change falls outside the merged dispositions, and no new architectural root cause was found.
