# SSH N1 Native Network and Trust Setup — Code-Axis Review

**Review object:** `gwz-core` `9b3d596f6f230b99ceabf9337d9c17376c2e9762`, including `src/git/endpoint/ssh_network.rs`, its focused tests, and `dev-docs/GwzRemoteTransportSshN1.md`; implementation checkpoint dated 2026-09-21  
**Baseline:** root `b759ced42b18efd674aca8699819a6eafdba2f7b`; evidence `16a9aef10b9216cc0988d3af7e0e483948bf03d2`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable objects with `git show`, `git diff`, and `git grep`.  
**Date:** 2026-09-21  
**Axis:** Code — implementation, ownership, native compatibility, error paths, and retained-interface composition. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block N1 acceptance. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

I read the complete canonical prompt; the accepted production-setup design and amended G1; the N1 checkpoint; the entire 348-line `ssh_network.rs`; the entire 652-line `network.rs`; retained `agent_job.rs`, `ssh_connection.rs`, `agent_auth.rs`, `ssh_setup.rs`, and the agent fixture where they directly compose; the pinned libgit2 host-key preference implementation; the Cargo-locked libssh2 known-host parser; and the private N1 README, inputs, and final log.

The archived hashes for `ssh_network.rs` and `network.rs` match the committed blobs. I also ran the permitted Rust 1.95 locked/offline focused suite: it exited 0 with 88 passed tests and one intentionally ignored child harness.

The tuple was unchanged at both boundaries. Final trees were root `9845b32caddcaae057852404b9b4ab93b341cb50`, core `fe22519614dd5dfa8e937bd9ce0b3b112d8988d4`, evidence `9f09873537ce6d7ecd283f8f3ca0b05480956963`, transport `bf275d0e746b8fc5104f7037cc33e8e685005bc1`, git2-rs `28fc6ad905653db47a7705706e1edea0f39d7054`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Member repositories were clean; root contained only the two allowed generated N1 prompts.

## 1. Findings

### [P2-1] Network error kinds are mistaken for cancellation/deadline signals

`ssh_network.rs:63-75` classifies a failed TCP attempt through `terminal()`, while `terminal()` at lines 326-330 treats every `TimedOut` or `ConnectionAborted` error as terminal. Those same kinds can originate from the socket itself: a first resolved address may return kernel `ETIMEDOUT` or `ECONNABORTED` while the shared `Control` remains live.

A resolution yielding `[first, live_second]` therefore stops on an ordinary pre-negotiation TCP failure from `first` and never attempts `live_second`. This violates the accepted sequential-address rule: another address remains eligible until SSH negotiation begins. The existing regression exercises `ConnectionRefused`, so it does not expose the kind collision.

After any TCP-connect error, query `control.check()`. Return its error if cancellation or the original deadline has fired; otherwise retain the socket error and continue to the next address regardless of its `ErrorKind`. Preserve the prohibition on retry after `handshake` starts.

Add a deterministic connect seam regression proving that live-control synthetic `TimedOut` and `ConnectionAborted` failures advance to a second address, while an expired or cancelled `Control` does not.

### [P2-2] Repeated carriage returns bypass the claimed native whitespace parity

`validate_lines()` at `ssh_network.rs:118-133` removes at most one terminal `\r` for measurement, but `load_known()` at lines 288-315 uses `trim_end_matches('\r')`, removing every trailing carriage return before native parsing.

For a no-comment trusted line ending `...\r\r\n`, N1 removes both carriage returns and supplies the canonical key to `KnownHosts::read_str`, so the host can be accepted. The pinned native file path removes only LF; its parser does not treat CR as a key delimiter, so the stored base64 token retains the carriage returns and later key comparison mismatches. Multiple CR bytes are not the documented CR/LF terminator or complete-line-length exception. This contradicts G1’s requirement that native malformed content still refuse and N1’s claim that native whitespace quirks remain preserved outside enumerated exceptions.

Normalize at most one terminal CR consistently, using the same `strip_suffix('\r')` rule in validation and loading. Add native differential regressions using a no-comment key line for LF, standard CRLF, and repeated-CR endings; any intended CRLF delta should be stated explicitly, while repeated CR must refuse.

## 2. Invariant analysis

The remaining attacks held. Trust loading and size/UTF-8/NUL/line admission precede resolution; opened-descriptor regular-file checks and `O_NONBLOCK` prevent FIFO trapping on the admitted Unix path. Late loader and resolver completion encounters `Control` before subsequent effects, and retained `Job` ownership prevents premature disposal.

Nonblocking connect and handshake use the original `Control`, bounded poll quanta, current libssh2 block directions, and socket-owning `SshConnection` destruction. Once a TCP connection enters handshake, every failure returns without address replay.

Full native known-host checking uses the logical host and effective port, including hashed names. Per-algorithm native sets reproduce the pinned preference order and its key-type-prefix behavior; the approved host-key bytes travel with the exclusive connection and A2 rechecks them before agent access. The fresh composition reaches ordered A2 authentication, two Git exchanges on one pooled connection, and bounded shutdown. No N2 authority, N3 attachment, production activation, or additional platform qualification is implied.

## 3. Risks and next action

The passing suite and archived evidence remain valid for the exercised cases, but neither blocking counterexample is covered. Apply the two bounded corrections and add their focused regressions, then request the focused Code re-verdict. Platform/source qualification and N2/N3 remain later gates.
