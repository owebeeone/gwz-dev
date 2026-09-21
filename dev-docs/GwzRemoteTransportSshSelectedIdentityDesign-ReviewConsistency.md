# SSH N2 Selected Identity Design — Consistency-Axis Review

**Review object:** `gwz-core` `120fb9f1379025c66ea00cbdfd5af7f5815859db`, `dev-docs/GwzRemoteTransportSshSelectedIdentityDesign.md`, with companion G1 and ProductionSetup refinements; documentation-only draft dated 2026-09-21  
**Baseline:** root `6cb03c3049aae85b66a254208a6084e41d04a027`; gwz-core-evidence `e842abf855e58de3c4381855fbc1b8374485a7cd`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed objects were read with `git show`/`git diff`.  
**Date:** 2026-09-21  
**Axis:** Consistency against the retained requirements, ProductionSetup contract, accepted A1–A3/N1 lifecycle, and current implementation seams. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings.

---

## 0. Evidence base

I read the complete generated review prompt; the complete N2 design (`GwzRemoteTransportSshSelectedIdentityDesign.md:1-224`); the changed G1 text (`GwzRemoteTransportRequirements.md:145-174`); the ProductionSetup explicit-identity contract and N2 refinement (`GwzRemoteTransportSshProductionSetup.md:74-120` and its final N2 section); accepted N1 scope/results; and the root checkpoint delta.

For feasibility grounding, I inspected the pinned transport pool `Key`, `Identity`, request, checkout and resource contracts; core identity selection and endpoint-local path resolution; `ssh_endpoint`, `ssh_worker`, `ssh_setup`, `ssh_shutdown`, `agent_job`, `agent_auth`, and `ssh_connection`; and the locked `ssh2` 0.9.6 / `libssh2-sys` 0.3.3 memory-key call path. No builds, tests, experiments, remote operations, or file writes were performed. The documentation diff passed `git diff --check`.

Start and end verification matched:

| Repository | Commit | Tree |
| --- | --- | --- |
| root | `6cb03c3049aae85b66a254208a6084e41d04a027` | `2bde69a48ce980a0a4990b93f8047af935356470` |
| gwz-core | `120fb9f1379025c66ea00cbdfd5af7f5815859db` | `2125e5a951add72789ea84096fb8bebdc58e9033` |
| gwz-core-evidence | `e842abf855e58de3c4381855fbc1b8374485a7cd` | `568e75ccf28e429e4024ffb72eb27788cc6bef99` |
| gwz-transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| git2-rs | `ce78628308e11b4e8901d5061602619109bce21a` | `28fc6ad905653db47a7705706e1edea0f39d7054` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |

Member repositories were clean. Root contained only the two explicitly excluded generated review prompts.

## 1. Invariant analysis

The admitted-versus-proven split is internally complete. Every selected open rereads before pool lookup; exact bytes may recover only a live, same-`Key`, previously native-proven token; unmatched and concurrent unproven candidates receive unique nonrecycled tokens; and physical connections still authenticate independently. This refines ProductionSetup’s earlier “validated snapshot” wording without allowing path, metadata, digest, or caller tokens to establish authority. Ambient and explicit identities remain isolated.

The resource ledger closes across every stated state. Slot and maximum-buffer reservations precede file I/O; the 16 MiB byte bound independently limits large concurrent reads even though 64 slots exist; matching cannot release its reservation until the new buffer is discarded and the existing entry is pinned. Strong handles cover admitted requests, setup Jobs, idle and active resources, reclaim, disposal, and retained cleanup. Weak registry ownership cannot resurrect expired entries, and monotonic endpoint-local tokens cannot acquire new meaning.

The worker sequence agrees with the accepted A1/A3 model: pending-request admission and one absolute deadline precede the selected-file Job; checkout occurs only after joined admission arbitration; cancellation and shutdown retain helper ownership until physical completion; and the retained endpoint owner covers both admission Jobs and `PoolHost`. Bounded per-tick admission work, deadline-before-result arbitration, sticky cleanup failure, and admission-only reaping close the no-pool-entry and panic cases.

The native-authentication shape is feasible against the pinned APIs. UTF-8/NUL-free admission supplies the `&str` required by `Session::userauth_pubkey_memory`; the wrapper makes a bounded temporary `CString`, while libssh2 preserves its nonblocking state across EAGAIN and reuses the currently supplied identical private bytes. Host-key recheck precedes credential work, terminal failure destroys the connection, and proof promotion requires joined authenticated handoff. No agent, alternate file, path reopen, prompt, Git, or old-transport fallback remains available.

G1 now states the exact intentional compatibility exceptions, limits, placement parity, failure ordering, and native differential requirement. The N2a/N2b packages have distinct ownership, activation boundaries, line budgets, and aggregate Code/State gates. N3 and platform/source qualification remain clearly deferred without being silently claimed.

## 2. Risks and next action

This verdict accepts the draft contract, not implementation, native-format coverage, platform qualification, backend attachment, capability advertisement, or production activation. The principal remaining risk is implementation drift at the promotion/cancellation linearization point and during transfer of reservation ownership; the required causal tests expressly cover those interleavings.

Proceed to N2a on a settled implementation tuple, enforcing the documented slot/byte ledger, exact-byte authority rules, native authentication handoff, and differential fixtures before N2b worker integration.
