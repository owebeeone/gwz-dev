# Step 2.1 carrier — Code-axis re-verdict, round 2

**Verdict: GO.** All 5 of my round-1 P2s are closed on this tree, and so are 3 of my 4 P3s. The fourth, C-P3-3, closes when your 2.0 §3.3 amendment lands; it does not block. I found one new P3, and it is not architectural.

**Object:** gwz-core `d8e7478d..49e79493`, evidence `dcf5e013` (run `2026-10-25-p2-1-rem1`). Read against the RemPlan.

## Evidence base

- **Tuple:** start and end were the same (gwz-core `49e79493`, evidence `dcf5e013`), and gwz-core is clean.
- **Tested tree = committed tree:** I took `d8e7478d`, applied the archived `gwz-core-tracked.patch` and the untracked tar, and compared it with `git archive 49e79493`. `diff -r` reports them identical.
- **What I ran:**
  - The two small-crate suites (load 14): `gwz-session-channel` 46 passed, `gwz-session-contract` 14 passed, 0 failed. The target directory is deleted.
  - `check_carrier_peer_rule.py`: ok (14 files). Its unit tests: OK.
- **What I relied on and did not re-run:** the carrier rows themselves rest on the Linux matrices a and b and the Windows red/green pair (`p2-1-rem1-20261025-b`).

## Prior-finding closure

| ID | Round-1 counterexample | Now (file:line at `49e79493`) | Status |
| --- | --- | --- | --- |
| **P2-1** (architectural) | One FIFO: a not-ready binding A held back binding B and a later tag-1 cancel. | Takes are now per scope: `State::pop(Scope::{Any, Registry, Binding})` (`stream/state.rs:203-220`). A frame's charge is released only when its own consumer takes it (`:223-248`). Registry and per-binding wakers are separate (`:251-264`). The carrier-wide 128 MiB pool is gone; the registry has its own byte cap (`stream/inbound.rs:63-71`, `mod.rs:165,175`). Rows in `class_rows`; mutants `class-takes-are-global` and `binding-take-is-global` are killed. | **Closed** |
| **P2-2** | An undecodable envelope for a live binding ended the session. | The frame is attributed by a shallow walk for `session_id` before the full decode (`wire.rs:142-169`, `stream/inbound.rs:91-109`). A frame for a binding that isn't open, or with no or non-text `session_id`, is discarded and counted. A live binding whose frame fails the full decode is faulted as `Invalid`, that binding only (`:125-136`). The session ends only for a non-map body or broken request-name framing. The dead `stream_id < 0` branch is gone. Mutant `decode-failure-ends-session` is killed. | **Closed** |
| **P2-3** | `{1, 2, 5: {}, 6: true}` was refused, and no `SessionOpen` could pass both the validator and the schema. | Keys 1, 2, 5 and 6 are required, 4 is optional (`wire.rs:193-233`). Key 5 is checked to be a map with at most `transport_off = false` (`0xf4`), and key 6 to be `0xf5` (`:236-251`). Anything else is refused. On receive the end writes exactly one `SessionError` and then shuts the stream (`stream/inbound.rs:207-216`). That `SessionError` matches the schema: `call_id` (1) and `error` (2), with error being `GwzError { code (1), message (2) }`, and `invalid_request` is 1 (`protocol/gwz.taut.py:672`, `:1180-1183`). | **Closed** |
| **P2-4** | macOS used `_CS_DARWIN_USER_TEMP_DIR` + `gwz/` with a `/tmp` fallback; Linux ignored `XDG_RUNTIME_DIR` and `TMPDIR`. | The resolver is now pure (`unix.rs:125-150`). macOS: `_CS_DARWIN_USER_DIR` + `gwz-<uid>`, refused when `confstr` fails, no fallback. Linux: the frozen order, with the environment values captured once in `transport_host/runtime_env.rs`. Parents that are group- or other-writable and not sticky are refused (`:178-194`). Rows in `directory_rows`; mutants are killed. | **Closed** |
| **P2-5** | On Windows, `CancelIoEx` alone left the handle open, the peer was never told, and a reader between reads could block again. | `Pipe::abort` sets an `aborted` flag that `enter` refuses new requests on (`windows.rs:507-518`). It cancels repeatedly until no request is in the kernel (bounded at 5 s), disconnects a server instance, and closes the handle (`:609-645`). The two `ending_rows` abort rows (abort while the caller holds the carrier, and abort taken inside the router) fail in the red run and pass in green receipt `-b`. | **Closed** |
| **P3-1** | Refusals were a silent drop. | Accept refusal sends one `SessionError` with `server_peer_refused`, then EOF: Unix `turn_away` (`unix.rs:344-359`), Windows (`windows.rs:690-721`). The `SessionOpen` refusal is as in P2-3. The implementer's deviation 2 (the listener writes the frame instead of returning the open connection) is acceptable: the bytes on the wire are what the frozen text requires. | **Closed** (but see N-P3-1) |
| **P3-2** | Assigning `policy.own` passed the source check. | `own` is now private (`peer.rs:106`). The test constructor `from_identity` exists only under `cfg_if!(test)` (`peer.rs:13-23`). The check also flags `.own` and `from_identity` outside `peer.rs`. The compiler now refuses the round-1 bypass. | **Closed** |
| **P3-3** | The close announcement was not in the frozen text. | Disposition is "text moves" (the RemPlan, and your 2.0 §3.3 clerical amendment after this verdict). The code is unchanged. | **Closes when the amendment lands** (non-blocking) |
| **P3-4** | The seam docs were false: `CarrierId` "shared by both ends", and a single FIFO. | `binding.rs:6-9` now says the id is per end and that the handshake field belongs to 2.2c. `mod.rs:14-30` now says order differs by end ("oldest unblocked" control frame) and that `admits` only checks an arrival came in on its own end. | **Closed** |

The implementer's three items that were not done as written:
- **Port reason (S-P3-3)** and **post-announcement discard without a deterministic row (S-P3-4):** both are State-axis findings; I did not assess them.
- **Accept-path refusal written in the listener:** assessed under P3-1 above. Acceptable.

## New finding

**N-P3-1 (not architectural). On Unix, the accept-path refusal can block the listener for hours.**
- **Root cause:** `turn_away` drains the refused client with a 200 ms timeout *per read*, up to 64 KiB, and has no total deadline (`unix.rs:349-357`). It runs synchronously inside `LocalListener::accept` (`:549-554`).
- **Reproduction:** a refused peer writes one byte every 190 ms. Every read succeeds, so the loop runs for about 65,536 × 0.19 s ≈ 3.5 h. During that time `accept` cannot serve the next client.
- **Impact today is small:** the Unix directory is mode 0700, so the only peers that can be refused today are same-user processes, which already have the user's authority. It matters once 2.3 adds the sandbox rule, because sandboxed same-user processes are the class it refuses.
- **Windows is already bounded:** its version stops at a 1-second total (`windows.rs:696`).
- **Correction:** give the Unix drain a total deadline of 1 s, the same as Windows. Alternatively, turn the peer away off the accept thread.
- **Closure test:** a refused client that trickles bytes; `accept` returns within about 1.2 s, and the next client is accepted.

## Risks to carry forward (not findings)

- **Provisional error code:** `SERVER_PEER_REFUSED = 82` is hard-coded and marked provisional (`wire.rs:256`). 2.0 §1 says 2.2b assigns the numbers at the schema's tail, so 2.2b must reconcile this value or the wire disagrees.
- **Surviving mutant:** `peer-pin-not-confirmed` (macOS `proc_pidinfo`) survives. It is the State axis's (S-P2-5), noted only because pinning is now claimed.
- **Clippy backlog:** the lib-test clippy backlog grew from 118 to 124 errors. It is not a gate, but the six new ones are not analysed.
- **Deferred items** stay recorded in the plan: the sandbox rule's platform checks, the address walk / `SessionHello` / bounded connect, `ws-<key>` naming, memory amplification and profile-3 windows, and the operator's decision on module versus small crate.

**Next action:** fix N-P3-1 whenever convenient; it does not block. Land the 2.0 §3.3 amendment, which closes P3-3. From the Code axis, nothing stops the trunk merge.
