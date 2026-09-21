# SSH N2 Selected Identity Design — SAFETY-AXIS REVIEW

**Review object:** `gwz-core` `120fb9f1379025c66ea00cbdfd5af7f5815859db`, `dev-docs/GwzRemoteTransportSshSelectedIdentityDesign.md`, DRAFT dated 2026-09-21, including the companion G1 and ProductionSetup refinements  
**Baseline:** root `6cb03c3049aae85b66a254208a6084e41d04a027`; gwz-core `120fb9f1379025c66ea00cbdfd5af7f5815859db`; gwz-core-evidence `e842abf855e58de3c4381855fbc1b8374485a7cd`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed files were read with `git show`; pinned dependency sources were inspected read-only.  
**Date:** 2026-09-21  
**Axis:** Safety: degraded paths, disclosure and resource scale, stuck states, cleanup, and irreversible-effect preconditions. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I read the complete generated Safety prompt; the N2 draft §§1–5; `GwzRemoteTransportRequirements.md` G1–G3; the explicit-identity and sequence sections of `GwzRemoteTransportSshProductionSetup.md`; and `CurrentProgramCheckpoint.md`’s N2/N1 status.

For feasibility and lifecycle grounding I inspected pinned `ssh_endpoint.rs`, `ssh_worker.rs`, `ssh_setup.rs`, `ssh_shutdown.rs`, `ssh_pool.rs`, `agent_job.rs`, `agent_auth.rs`, `ssh_network.rs`, `ssh_connection.rs`, and gwz-transport pool identity/request state. I also traced `Session::userauth_pubkey_memory` through locked `libssh2-sys` 0.3.3: `userauth.c` and `openssl.c`, including the memory/file key readers and native parser entry.

No build, test, experiment, mutation, or report write was performed. Start and end tuple checks matched exactly. Root contained only the two permitted untracked generated review prompts; member repositories were clean.

## 1. Findings

### [P2-1] Unsupported encrypted containers can monopolize every shared helper inside an uninterruptible native KDF

**Location:** `GwzRemoteTransportSshSelectedIdentityDesign.md` §4, lines 157–168, and §5 test 2, lines 197–199.

The design sends every admitted UTF-8/NUL-free candidate to `Session::userauth_pubkey_memory(..., None)` and only checks cancellation at native-call boundaries. It acknowledges non-preemptible parsing, but its encrypted-key gate requires only an ordinary nonempty-passphrase fixture to fail.

The pinned path first calls `PEM_read_bio_PrivateKey` (`libssh2/src/openssl.c`, `_libssh2_pub_priv_keyfilememory`) with libssh2’s null passphrase normalized to an empty password. Encrypted PKCS#8 PBES2/PBKDF2 inputs are therefore decrypted using attacker-controlled algorithm parameters before bad-password rejection; an empty password cannot be rejected structurally because it is itself a valid PKCS#8 password. A sub-1-MiB `BEGIN ENCRYPTED PRIVATE KEY` container can encode an extreme valid iteration count.

Concrete sequence: supply such a selected file, connect through a trusted controlled server, and enter native authentication. The setup Job cannot observe its deadline or cancellation while OpenSSL performs the KDF. Repeat across 64 requests/endpoints: all process-wide A1 helpers are occupied, later admission and network setup refuse, CPU is saturated, and shutdown retains all Jobs without reaching cleanup-complete until the native calls return. The file-size, snapshot-byte, and thread-count caps bound memory and thread count, but not the native work or retention duration. The normal encrypted fixture does not exercise this state.

The design must either reject every unsupported encrypted container through a bounded pre-native classifier, including encrypted PKCS#8, traditional encrypted PEM, and encrypted OpenSSH envelopes, or put native parsing in an actually terminable resource boundary with enforced CPU/time limits. Classification need only establish “encrypted/unsupported”; native parsing must remain the authority for accepted unencrypted key usability.

Closure requires adversarial fixtures with extreme KDF work factors and an observer proving refusal before native authentication. Saturate the admission path with those fixtures and prove bounded refusal, helper recovery, truthful cleanup completion, and progress of an existing active stream.

## 2. Invariant analysis

The authority boundary otherwise held. A candidate is only admitted by a fresh bounded read; exact bytes plus the same pool `Key` may reuse only a live native-proven token. Fresh and concurrent unproven candidates receive distinct nonrecycled tokens, and promotion occurs only after a joined, live, authenticated handoff. Explicit tokens remain distinct from ambient identity.

Ownership is complete across admission, pool wait, connecting, idle, active, disposal, panic, and cleanup overrun. Slots and actual buffer capacity remain charged through helper join and destruction; weak registry entries cannot retain secrets or resurrect tokens. Full capacity refuses before file I/O, and the endpoint cleanup owner covers both admission Jobs and physical resources.

The single absolute deadline, exact-deadline arbitration, late-result suppression, host-key recheck, immutable snapshot use, no pathname reopen, and no agent/Git/alternate-file fallback close the examined confused-authority and cancellation races. The G1 size/encoding changes are explicit and testable. N2a/N2b retain bounded implementation gates and do not claim N3 attachment or activation.

## 3. Risks and next action

Snapshot lifetime still enlarges the interval during which raw private-key bytes exist in process memory, but storage, copying, diagnostics, and ownership are explicitly bounded; this does not independently cross the finding threshold at this design gate.

Revise §4 and the causal gates to bound unsupported encrypted-container rejection before an uninterruptible KDF, then perform a focused Safety re-verdict. N2 implementation, N3 attachment, platform/source qualification, and activation remain separate later gates.
