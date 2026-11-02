# Step 2.3d (session handshake and bounded connect) — State-AXIS REVIEW
**Review object:** gwz-core aaa7c23a and 07053e90 in lane p2-3d; evidence 2026-10-31-p2-3d1 and -p2-3d2
**Baseline:** I read the sources at lane gwz-core `07053e90`, which was clean at both start and end with 0 status lines. I read `git diff aaa7c23a^ 07053e90` (19 files, +3247/−30) and each commit alone, against the evidence at gwz-core-evidence `54cf45e5` and `d269fb3d`. I ran builds and tests in an `rsync -aL` copy at `build-targets/review-p2-3d-state-src` with my own target dirs, on the Mac and in an OrbStack container (`review-p2-3d-state-img` / `review-p2-3d-state-cargo`). The copy, target dirs, image tag and volume are all removed. My rows file and logs are kept in `/Volumes/projects/limbo/build-targets/review-p2-3d-state-tools/`.
**Date:** 2026-10-31
**Axis:** State: adversity, bounds, faults and fail-closed direction of the handshake, the raw-stream wire and the bounded connect.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 9 P3. Every bound held against hostile peers on macOS and Linux. Every refusal was one `SessionError` followed by the carrier's close. No thread or fd leaked, and no byte was lost at the handover. The P3s are robustness, diagnosability, documentation and coverage items. None blocks the release.

## 0. Evidence base
- **Tuple:** lane gwz-core HEAD `07053e906eb1…` at start and end. The diff of the two commits matches the commit stats. The evidence heads are `54cf45e5` and `d269fb3d`.
- **Mac rows:** macOS 26.6.2 arm64, rustc 1.95.0, candidate shape (`RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real TMPDIR=/Volumes/gwztmp`). `cargo test --lib transport_host::carrier`: 146 passed, 0 failed. That equals the drafter's count.
- **Linux rows:** OrbStack aarch64, privileged, same shape. The carrier rows: all 147 lane rows passed. The 2 failures in that run were my own first-draft rows, which wrongly assumed no trailing close frame; corrected below.
- **My hostile-peer rows:** `review_state_rows.rs`, 14 rows, run with `--test-threads=1` on Mac and Linux. All pass. Some are observational (they print what happened rather than assert). Results:

| Row | Attack | Result on Mac and Linux |
|---|---|---|
| rv01 | A host that sends a valid 52-byte hello one byte every 20 ms; open bound 300 ms | `Unavailable{Hello, Silent}` at 305/302 ms. The bound is not reset by partial progress |
| rv02 | Hello sent after 150 ms, then `SessionOpened` one byte at a time | `Unavailable{Reply}` at 304/302 ms, counted from the start of the dial |
| rv03 | A first-frame head only: length 4097, 0, or 0xFFFFFFFF, then silence | Fails in under 7 ms with the five bytes escaped. The client does not read on |
| rv04/b/c | `SessionOpened` and a `SessionReply` in one write; the same pipelined with the hello before the open; the same byte by byte | The client's carrier receives the `SessionReply` each time. No byte lost |
| rv05 | 60 failed dials to a squatter that holds every accepted stream open | Threads 2→2. Fds 5→65, of which 60 are held by the squatter, so the client leaked none |
| rv06 | A real listener and host, raw client sending: an open with an environment canary; a non-canonical protocol head; protocol 9; a hook refusal | Wire tags `[6,3]` (the carrier's own refusal) or `[6,3,17]`. The 17 is the close announcement, body `[0,0]`. The canary never comes back. The hook does not run on validation refusals |
| rv07 | `ServerControl` stop/force | `[6,8,17]`; the open hook does not run |
| rv08 | A client that dribbles its `SessionOpen`; host bound 300 ms | `HostFault::Expired` at 304 ms; tags `[6,17]` |
| rv10 | `limits` given as a half-float or a two-byte simple value | Passes both checks (F8) |
| rv11 | 4 MiB `client` string sent to a host that sends its hello and never reads | No result within 3 s against a 300 ms bound (F1) |
| rv12 | A tag-16 lane frame, then a valid `SessionOpen` | The frame is discarded and the session opens (F2) |

- **Owner gates I did not rerun:** WSL `p2-3d-remote-20261031-a` and Windows receipt `p2-3d-20261031-a`, both given as PASS in the brief.

## Problem, requirement provenance, and scope
- **Requested outcome.** Plan 2.3d: C3, R6 and the server design §3 order, plus a bounded connect, the C11 hook before `SessionOpened`, and the listed red rows. The authority is the operator's 2026-10-31 go.
- **Mechanisms checked against provenance:**
  - 4 KiB first-frame cap, the 30 s and 5 s bounds from the connect, and the single `SessionError(0)`: design §3 and split doc §3.1 / §3.3.
  - Non-blocking connect retried on EAGAIN, `WaitNamedPipeW` for the time left, and macOS ECONNREFUSED treated as refused: design §3 "Every client-side connect is non-blocking".
  - Raw stream before any carrier: needed so a banner is judged as bytes, not frames.
  - The helper thread: needed because a pipe read has no timeout.
- **Within the owned files and functions:** `connect_once`, `connect_until`, `connect_checked` and `connect_stream` stay inside the `connect` region of `unix.rs` and `windows.rs`. The `windows_parity` JSON edits are gate-required.
- **Drafter's notes, judged:**
  - *16 KiB cap on later host frames:* accepted. It bounds the client's allocation before trust. `SessionOpened` strings are already bounded by the 4 KiB hello, and the refusal text is fixed. A `ServerState.log` path on Windows could in theory exceed 16 KiB (32 767 UTF-16 units). The cap should be recorded in the plan's C3 row as a design addition.
  - *Helper thread detached on failure:* accepted. rv05 confirms it ends on Mac and Linux. On Windows it depends on `CancelIoEx`, which the Windows silent-host row exercises indirectly (F10).
  - *`connect` keeps its signature with a 30 s bound; `Bounds` is an ordinary parameter; the 30 s and 5 s bounds are tested on an injected clock:* accepted. No test-only switch exists.
  - *macOS ECONNREFUSED not retried:* matches the design, which says "inferred". It is now observed.
  - *`BUSY_WAIT_MS` / `BUSY_ATTEMPTS` removed:* correct. They were dead after the change.
  - *`put_head` made `pub(super)`:* harmless.
  - *`wait_closed`:* out of scope, as stated.
- **Plan gap:** split doc §3.3 "When" says a lane frame before `SessionOpened` is a protocol error that ends the carrier. No 2.3 step carries that rule (F2).

## 1. Findings

**F1 (P3) — The client's request write is outside the handshake bound.**
- **Root cause:** `StreamWire::send` does a blocking `write_all` on the caller's thread (`handshake/dial.rs`; called from `client.rs` `exchange`). The request has no size cap.
- **Violated invariant:** "every wait counts from the start of the connect" (design §3).
- **Reproduction:** rv11. A 4 MiB `client` string sent to a host that sends a valid hello and never reads gave no result within 3 s against a 300 ms bound, on Mac and Linux.
- **Impact:** unreachable with real requests. The product's requests are about 60 B, far below the macOS 8 KiB AF_UNIX send buffer. The bound holds only while the request fits the send buffer.
- **Correction:** refuse a request over `FIRST_FRAME_MAX` before connecting, or bound the write by doing it on the helper thread and aborting at the deadline.
- **Closure test:** rv11 returns within the bound plus 100 ms.

**F2 (P3) — A transport-lane frame before `SessionOpen` is discarded and the open is then accepted.**
- **Root cause:** `host.rs` `converse` reads only the registry class through `CarrierWire::recv`. A tag-16/17 frame for an unopened binding is discarded by the carrier, counted in `discarded_frames()`, and never seen by the host.
- **Violated invariant:** split doc §3.3 "When" ("a lane frame before it is a protocol error ending the carrier") and design §3 "any other order is a protocol error".
- **Reproduction:** rv12. Host result `Ok(Opened)`, wire `[6,5,17]`.
- **Impact:** no state is created, but ordering is not enforced and a client bug is hidden.
- **Provenance:** the 2.3d red rows do not list this rule and no other step carries it.
- **Correction:** before writing `SessionOpened`, have the host close with a protocol fault if the carrier's discard count has grown; `discarded_frames()` is already `pub(crate)`. Alternatively, the lane owner assigns the rule to 2.3g with a red row.
- **Closure test:** rv12 gives a host fault, no tag 5, and the hook does not run.

**F3 (P3) — A connect that never completes is not reported as `server_unavailable`.**
- **Root cause:** `dial` returns `DialFault::Connect(LocalRefusal::Timeout)`, and `DialFault` has no `code()` (`handshake/dial.rs`).
- **Violated invariant:** design §3 "A connect that never completes is reported as a silent listener is."
- **Reproduction:** the lane's own row `a_dial_against_a_full_queue_fails_at_the_handshakes_bound` asserts `Connect(Timeout)`.
- **Impact:** the caller has to know to map it. Nothing is lost; the lock-file details belong to 2.3f.
- **Correction:** add `DialFault::code()` mapping `Timeout` to 79, or record the mapping as a carried item for 2.3k.
- **Closure test:** that row also asserts `code() == Some(SERVER_UNAVAILABLE)`.

**F4 (P3) — A banner shorter than five bytes, followed by silence, is reported as "no answer".**
- **Root cause:** `Heard::Silent` carries no bytes (`handshake/mod.rs`), so partial bytes are dropped at the bound in both `StreamWire` and `Hearing::bytes`.
- **Violated invariant:** design §3 and §17, "shows its first bytes with control characters escaped".
- **Reproduction:** by code reading. A squatter sends `"$ "` and holds the stream; at 30 s the message says "no answer in 30s while waiting for the host's SessionHello".
- **Impact:** a wrong diagnosis on a misdirected address.
- **Correction:** carry the partial bytes in `Silent` (after the abort, the pump's `Ended(partial)` can be collected with a short wait) and show them escaped.
- **Closure test:** a row that sends 2 bytes and goes silent; the message contains the escaped bytes.

**F5 (P3) — The refusal docs overstate what goes on the wire.**
- **Location:** `handshake/mod.rs` "Refusals" says "The host never writes after it" and "a fixed message that repeats nothing the peer sent". `host.rs` says "never repeating anything the peer sent".
- **Reproduction:** rv06 and rv07 show the carrier's close announcement (tag 17) after every refusal and after `ServerState`. That close is correct under the split doc's 2026-10-27 amendment, but the text does not say so. The version-mismatch text includes the peer's `protocol` number.
- **Impact:** a reader of the docs could build a strict client that treats tag 17 as a violation.
- **Correction:** amend the text to "then the carrier's close (its close announcement)" and note the numeric exception.
- **Closure test:** text check.

**F6 (P3) — On `SessionOpened` write failure after a successful hook, the C11 contract is unstated.**
- **Root cause:** `host.rs` `open` returns `HostFault::Wire` after `hooks.open` returned `Ok`, with nothing to show that a session was opened.
- **Violated invariant:** C11's exposure to 2.3g, "a hook that runs before `SessionOpened` is written", leaves the rollback owner undefined.
- **Reproduction:** a `FrameWire` whose `send` of tag 5 fails gives `Err(Wire)`, with the hook's session left in the composition.
- **Impact:** 2.3g could leak a counted session.
- **Correction:** document on `Hooks::open` that the caller owns rollback on any `Err` after `Ok`, or return a variant that carries the `OpenRequest`.
- **Closure test:** a scripted row where tag-5 send fails asserts the outcome reports that the hook ran.

**F7 (P3) — The Windows busy-pipe wait has a latent wait-forever value.**
- **Location:** `windows.rs` `connect_checked`, `u32::try_from(left.as_millis()).unwrap_or(u32::MAX)`. `u32::MAX` is `NMPWAIT_WAIT_FOREVER`.
- **Impact:** only reachable with a deadline more than 49.7 days away, so unreachable at the 30 s and 5 s bounds. The `.max(1)` guard against 0 (the default wait) is correct.
- **Correction:** add `.min(u32::MAX - 1)`.
- **Closure test:** review only.

**F8 (P3) — The strict body decoder accepts some floats and invalid simple values.**
- **Root cause:** `body.rs` `Strict::head` exempts major 7 from the shortest-form check, and `skip` accepts `7 if (20..=21)` whatever the additional info. So `f9 00 14`, `f8 14` and `fb …15` all pass.
- **Violated invariant:** the module doc says "no tags or floats … refuses every non-canonical spelling". `validate_session_open` skips key 4 loosely.
- **Reproduction:** rv10. `validate` returns `Ok` and `OpenRequest::decode` returns `Ok`, carrying the raw float as `limits` into the hook.
- **Impact:** a non-canonical `limits` reaches the composition; 2.3g's `SessionLimits` decode is the remaining check.
- **Correction:** accept only the single bytes `0xf4` and `0xf5` for major 7.
- **Closure test:** rv10's three inputs give `Malformed`.

**F9 (P3) — No row asserts helper-thread and fd cleanup after failed dials.**
- **Location:** `handshake_local_rows.rs`.
- **Reproduction:** rv05 passes on Mac and Linux. Windows (`CancelIoEx` with a 5 s give-up) is covered only indirectly.
- **Correction:** add an rv05-like row on all three platforms (thread count via `/proc`, `proc_pidinfo`, Toolhelp).
- **Closure test:** that row is green on the Windows receipt.

## 2. Invariant analysis
- **Bounds counted from the start of the connect** — holds. `dial` takes `started` before `connect_stream`, which gets `started + wait`, and the handshake keeps a single deadline. A control dial uses 5 s for the connect too (mutation n04). Dribbled bytes do not reset the bound (rv01, rv02). A connect that uses the whole bound reads nothing (injected-clock row).
- **First-frame cap judged before the body** — holds (rv03). Escaping is printable ASCII only, capped at 32 bytes, so no terminal escapes and no unbounded output. Refusal text is capped at 256 bytes.
- **Nothing sent before a well-formed hello** — holds by control flow. On a byte stream the host cannot see whether a client sent early, so the plan's row is correctly placed on the client. The version check happens before any send.
- **One `SessionError(0)` then close** — holds (rv06), with the carrier's close announcement as the close. `ServerControl` never runs the open hook (rv07). `SessionOpened` is written only after the hook returns `Ok` (rows plus rv06's hook case).
- **Fail-closed** — every client failure aborts the stream. `ECONNREFUSED`, `EINPROGRESS` and other errors fail rather than retry. A Unix-domain non-blocking connect completes at once or returns `EAGAIN`, so no `SO_ERROR` check is needed. Exceptions: F2 (an out-of-order lane frame is tolerated) and F8 (decoder).
- **No byte lost at the handover** — holds. The pump reads exact counts on unbuffered halves, and the read half goes back through `Ask::Return` (rv04/b/c).
- **Helper-thread lifetime** — holds on Mac and Linux (rv05). Windows abort may hold the caller up to 5 s if the kernel does not return a cancelled read; that is bounded and a limitation.
- **Unbounded paths** — only F1, and F7 (latent).
- **Repo rules** — conditional compilation is in `cfg_if!` or platform modules; no globals or thread-locals; rows live under `src/…/tests` behind `cfg(test)`; Rust enforces braces.

## 3. Risks and next action
- **Risk:** the Windows client paths (`connect_stream`, the pump over `Pipe`) are verified only by the owner's receipt and its four real-carrier rows; I could not check them locally.
- **Next actions:**
  - The lane owner decides F2's owner (2.3d or 2.3g) and records F3's mapping for 2.3k.
  - F1, F4, F5, F6, F7, F8 and F9 can be fixed in one small follow-up or carried. None blocks 2.3g.
  - Record the 16 KiB reply cap in the C3 row.

## 4. Out-of-scope observations
- `local::connect` (production, 30 s bound, no handshake) now has only test callers (`local_rows.rs`, `windows_rows.rs`). That was already true of the 2.1 code, but its future should be decided rather than left as production code used only by rows.
- `RawConnection::into_carrier` duplicates about 10 lines of `carrier_over`, as the drafter noted. This is left until the 2.3a remediation lands.
- The `StreamCarrier::wait_closed` behaviour after a peer's normal close (it stays `None` until a consumer takes the end) is in `stream.rs`. The drafter's probe matches what I read in the code. It is relevant to 2.3g and 2.3i.
