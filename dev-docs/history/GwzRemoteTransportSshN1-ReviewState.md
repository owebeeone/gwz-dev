# SSH N1 Native Network and Trust Setup — State-Axis Review

**Review object:** N1 implementation at `gwz-core` `9b3d596f6f230b99ceabf9337d9c17376c2e9762`, controlled by `dev-docs/GwzRemoteTransportSshN1.md`, status “implementation review pending”  
**Baseline:** root `b759ced42b18efd674aca8699819a6eafdba2f7b`; evidence `16a9aef10b9216cc0988d3af7e0e483948bf03d2`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show`; native libssh2 behavior was read from the Cargo-locked `libssh2-sys 0.3.3` source.  
**Date:** 2026-09-21  
**Axis:** State — lifecycle, ownership, cancellation, fail-closed transitions, replay legality, and retained-resource disposal. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings.

---

## 0. Evidence base

I read:

- `src/git/endpoint/ssh_network.rs`, especially setup ordering and address state at lines 31–76; trust admission and resolution at 78–140; nonblocking connect/handshake and polling at 143–263; host-key preference and complete-line native parsing at 265–324; and error classification at 326–339.
- `tests/transport_ssh/tests/network.rs`: fresh setup and reuse at 72–143; trust refusal at 145–218; cancellation and host-key selection at 229–328; exact store/line/native-reader boundaries at 345–447; late loader/resolver ownership at 449–516; address replay exclusions at 518–611; and pre-resolution input refusal at 614–650.
- The accepted production-setup design and amended G1, the N1 checkpoint, CurrentProgramCheckpoint, and unchanged A1–A3 ownership/authentication components where they interact directly.
- Pinned libgit2 `ssh_libssh2.c:434–590` and Cargo-locked libssh2 `knownhost.c`, including `hostline`, `libssh2_knownhost_readline`, and the 4,092-byte read buffer.
- Evidence run README, `inputs.json`, retained red/green logs, and final logs. The committed production, test, manifest, and lockfile hashes match `inputs.json`.

I independently ran the sole permitted Rust 1.95 locked/offline focused suite. It passed all 88 executed tests; the one ignored test is the documented child-only fixture. Build output remained under `/tmp`.

The exact tuple was verified before and after review. It did not move. Core, evidence, transport, git2-rs, and libgit2 remained clean. Root contained only the two expressly excluded generated N1 prompt files.

## 2. Invariant analysis

**Admission precedes external effects.** `establish_inner` validates the destination, loads and validates the complete trust store, and checks cancellation before invoking resolution. The actual loader caps reads at 4 MiB plus one detection byte, requires an opened regular descriptor, validates UTF-8, rejects NUL, and enforces the 16 KiB physical-line bound. Tests drive invalid UTF-8, NUL, oversized store, and oversized line through the real loader and prove zero resolver calls.

**Blocking OS work remains owned.** File loading and resolution execute inside the existing bounded `Job`. They are not claimed to be kernel-preemptible. A cancellation arriving while either injected operation is blocked prevents every subsequent resolver/connect effect after it returns; the job remains charged until the worker joins and disposes its result. The late-loader and late-resolver sequences exercise both branches and distinguish an overdue cleanup report from a disposal acknowledgement.

**The address state machine does not replay negotiated work.** At most 32 resolved addresses are retained. Pre-negotiation TCP failures may advance sequentially. Once a socket enters `handshake`, its result is returned directly, so handshake, trust, or later authentication failure cannot revisit the address loop. Tests prove dead-address fallback before negotiation, no second address after a failed handshake, and no second address after terminal authentication failure.

**Network waits preserve one deadline and one owner.** Connect and libssh2 handshake use nonblocking sockets, the shared `Control`, correct inbound/outbound poll directions, and waits capped at 20 ms. Cancellation and expiry are checked around native calls. Failed or discarded results drop the exclusive `SshConnection`; its owner shuts down the socket before the native session destructor runs. Stalled-handshake tests observe the requested error class and peer-side socket closure.

**Trust state remains fail-closed.** Complete-line parsing implements the accepted G1 exception exactly: 4 MiB store, 16 KiB line, UTF-8, NUL-free, with 4,092-byte lines intentionally diverging from the pinned 4,091-byte native chunk reader. The differential fixtures cover padded comments and host lists at 4,090/4,091/4,092 bytes and the restrictive 5 MiB case. Space/tab handling and native key-type-prefix recognition follow the pinned parser; retained failed red runs show both parity mistakes were detected before review.

Host matching, effective-port handling, hashed hosts, raw approved-key comparison, and libgit2 host-key preference order remain native. Unknown, missing, malformed, and mismatched trust cannot open the agent or offer a key. All temporary native known-host owners end before the connection and approved host-key bytes transfer together to A2, which rechecks the host key before agent access.

**A2/A3 composition preserves state attribution.** The native fixture demonstrates rejected-first/accepted-second identity progression within one negotiated session, one physical connection across two Git exchanges, and reuse without reporting another credential offer. Cancellation, failure, or late completion cannot publish a connection after the supervisor has classified the job terminal.

## 3. Risks and next action

This verdict covers the Unix/macOS N1 implementation and its isolated composition only. It does not certify kernel cancellation of DNS or filesystem calls, Windows/Linux behavior, production routing, selected-source reconstruction, N2 explicit-key snapshot authority, N3 backend attachment, or capability activation.

The next action is to accept this N1 checkpoint and proceed to the separately reviewed N2 contract. N3 and the deferred platform/source qualification remain mandatory before activation.
