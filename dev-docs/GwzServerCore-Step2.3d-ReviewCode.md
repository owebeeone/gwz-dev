# Step 2.3d (session handshake and bounded connect) — Code-AXIS REVIEW
**Review object:** gwz-core aaa7c23a and 07053e90 in lane p2-3d; evidence 2026-10-31-p2-3d1 and -p2-3d2
**Baseline:** I read the sources at gwz-core `07053e90` in `/Volumes/projects/limbo/gwz-dev-p2-3d`, from `git diff aaa7c23a^ 07053e90` and each commit alone. The working tree was clean, and the tuple was the same at the start and the end. Evidence is at `d269fb3d` and `54cf45e5`. Builds and tests ran in an `rsync -aL` copy, now deleted. I wrote probes in that copy only.
**Date:** 2026-10-31
**Axis:** Code: architecture, interfaces, call graphs, and the reality of the wire format and compatibility.
**Verdict: NO-GO** — 0 P0, 0 P1, 1 P2, 9 P3. I commit to GO on a revision that closes CODE-1 with its closure test. Each P3 should be either closed or recorded by the lane owner as a decision. No P3 blocks.

## 0. Evidence base
- **Mac** (macOS 26.6.2, rustc 1.95.0, candidate shape, `TMPDIR=/Volumes/gwztmp`). `cargo test --lib transport_host::carrier` passed 146 rows, the drafter's count.
- **Linux** (OrbStack, image `review-p2-3d-code-img` tagged from `p2-3a-linux-img`, volume `review-p2-3d-code-cargo`, privileged). After my sources were confirmed identical to the lane, three runs in a row passed 147 rows, the drafter's count.
  - One earlier run is invalid. A mutation of mine was being applied to the same copy while it compiled.
  - One other run failed one 2.1 row, `local_rows::a_frame_set_round_trips_over_a_real_local_carrier_between_two_threads`, with the host load average near 60. It did not fail again in three reruns at load 9 to 20 (see §4).
- **Mutations.** I ran the drafter's drivers, with paths changed to point at my copy. All ten were red on their named rows.
  - Mac: `m06`, `m22`, `m31`, `m33`, `m34`, `n02`, `n03`.
  - Linux: `n05`, `n06`, `n10`.
  - The logs are in `/Volumes/projects/limbo/build-targets/review-p2-3d-code-tools/mut/` and `mut2/`.
- **Source checks** in the copy: `check_cfg_boundaries.py --skip-repo gwz-cli --skip-repo gwz-py`, `check_windows_parity.py`, `check_process_globals.py` and `check_carrier_peer_rule.py` all exited 0.
  - The diff adds no `thread_local`, `static` state, or `#[cfg]` outside `cfg_if!`.
- **Reviewer probes.** The source is `/Volumes/projects/limbo/build-targets/review-p2-3d-code-tools/review_probe_rows.rs`.
  - R1: a peer refusal arriving as the first frame, both scripted and over a real listener.
  - R2: floats and long-form simple values inside `limits`.
  - R3: an explicit `false` in the optional booleans.
  - R4: the text of the version refusal.
- **Gates.** I took the lane owner's WSL OVERALL PASS and Windows receipt PASS as given. I confirmed their summary lines in `p2-3d-logs/remote-a.log` and `win-p2-3d-20261031-a.log`.

## Problem, requirement provenance, and scope
- **Requested outcome.** Plan §3 step 2.3d: C3 and R6. The server design's §3 handshake on the existing carrier: bodies as deterministic CBOR, the host and client machines, the 30 s and 5 s bounds, the 4 KiB first-frame cap, the version range, and `server_unavailable` with escaped bytes. Also the C11 hook for 2.3g and a bounded `connect` on all three platforms. The operator's go of 2026-10-31 authorizes it.
- **Scope.** The plan's Owns list names `handshake/{mod,body,host,client}.rs` and `handshake_rows.rs`. The lane also created `handshake/dial.rs`, `RawConnection` in `local/mod.rs`, and four further row files.
  - `dial.rs` is not in the Owns list. It is needed, though: a bounded read of raw bytes before any carrier exists is the only way to judge a banner by its length and to time out a pipe read. It follows the 2.3d-1/2.3d-2 split the lane owner accepted.
  - The `local/*.rs` edits stay in the connect region, as the plan allows.
- **Drafter's notes, judged.**
  - 16 KiB later-frame cap: this is an addition not found in the design or the plan (CODE-10). It does not contradict the design, which caps frames at 64 MiB.
  - Detached read helper thread: accepted. The abort frees it on every platform (`shutdown(Both)` on a duplicated descriptor on Unix; a `CancelIoEx` loop in `Pipe::abort` on Windows), so it is a bounded limitation, not a leak.
  - `connect` keeps its signature with a 30 s bound, and `Bounds` is an ordinary parameter: accepted. `Bounds` is a real configuration value, not a test seam. But `connect` itself is now only a seam (CODE-7).
  - macOS `ECONNREFUSED` not retried: accepted. It matches design §3 ("refused at once"), and a row observes it.
  - `BUSY_WAIT_MS`/`BUSY_ATTEMPTS` removed: accepted. Nothing references them, so the dead-code rule is met.
  - `put_head` became `pub(super)`: accepted. This is a minimal change to `wire.rs`.
  - `validate_session_open` is called, not changed: confirmed.
  - `wait_closed` stays `None` after a normal peer close: accepted as the drafter recorded it, for 2.3g and 2.3i. It is outside this step's files.

## 1. Findings

### CODE-1 (P2) The client turns the host's peer refusal into `server_unavailable` and discards its code and cause
- **Location:** `handshake/client.rs:254-258` (`exchange`: any first frame whose tag is not 6 goes to `not_a_hello`).
- **Violated invariant:** The server design says the host answers a refused peer with exactly one `SessionError` and nothing else:
  - §4 (`GwzCoreServerDesign.md:256`): "A refused peer gets one `SessionError` (`call_id` 0, `server_peer_refused`) naming the cause, and then the connection closes."
  - The split doc's §3.3 amendment of 2026-10-27 says the same.
  - §17 gives the client's message as `the server at <address> refused this process: <cause>`.
  - The 2.1 listener sends exactly this frame (`turn_away`) in place of the hello. A client following §17 must therefore recognise it as the first frame.
- **Reproduction** (probe R1, real listener): bind `LocalListener` with `other_user_policy()` and `dial` as the current user.
  - Result: `DialFault::Handshake(Unavailable { stage: Hello, cause: NotAHello { first: "2\x00\x00\x00\x03" } })`, code 79, text `server unavailable: the first frame is not a SessionHello; it began "2\x00\x00\x00\x03" (process 44222)`.
  - The host logged `Peer(OtherPrincipal)`. The scripted form of the probe gives the same result.
- **Impact:** These clients are told "server unavailable" with five hex bytes instead of the refusal cause: a client of another user, one in a Linux user namespace or under a seccomp filter (containers, Flatpak), one in the macOS sandbox, or one at low integrity on Windows. Code 82 is lost for good, because the body is never read, so 2.3k cannot recover it.
- **Design tension, to record:** §3 and the plan's red row say literally that a first frame with a tag other than 6 fails `server_unavailable`. §4 and §17 require the exception above. The code follows §3 alone.
- **Correction:** At `Stage::Hello`, after the length check (the 4 KiB cap still applies), take a tag-3 frame whose body `decode_refusal` accepts (`call_id` 0). Return `ClientFault::Refused { code, message: escape(..) }` and write nothing. Everything else stays `NotAHello`. The lane owner records a clerical amendment to §3 for the exception.
- **Closure test:**
  - A real-carrier row: a listener with `other_user_policy()`; `dial` gives `Refused { code: 82 }`, its message contains "another user", and nothing is written.
  - A scripted row for a first-frame `SessionError` with `call_id` ≠ 0, which stays `NotAHello`.
  - Dropping the tag-3 arm turns the first row red.

### CODE-2 (P3) The strict decoder accepts floats and two-byte simple values whose argument is 20 or 21
- **Location:** `handshake/body.rs:68-69` (major type 7 is exempt from the shortest-form check) and `:99` (`7 if (20..=21).contains(&argument)`).
- **Violated invariant:** The module doc at `body.rs:4-6` says a float is refused and that decoding and re-encoding gives the same bytes.
- **Reproduction** (probe R2): a `SessionOpen` with `limits` = `a1 01 f9 00 14` (half float), `a1 01 fa 00 00 00 15` (single float), `a1 01 f8 14` (two-byte simple value), or `a1 01 81 f9 00 15`.
  - Each passes `validate_session_open` and `OpenRequest::decode`.
  - `accept_open` returns `Opened`, and the hook receives those bytes.
- **Impact:** Bounded. Typed booleans go through `as_bool`, which checks the exact bytes, so only skipped values are affected. Today that means key 4, which reaches the 2.3g hook raw. The existing row uses `f9 00 00`, so it misses this case.
- **Correction:** In `skip`, accept major 7 only for the single bytes `0xf4` and `0xf5`.
- **Closure test:** Add the four inputs above to `nested_items_are_held_to_the_same_rule`, each expected `Malformed`.

### CODE-3 (P3) Explicit `false` in an optional boolean is refused in one body and accepted in the other
- **Location:** `body.rs:341-345` (`Control::decode` refuses `force: false` with `Range(2)`). Compare `wire.rs` `check_attributes`, which admits `transport_off: false`.
- **Violated invariant:** The design's shape is `force: bool (2, optional; absent means false)`. Refusing a spelled-out `false` is a drafter rule with no provenance in the design or the 2.1 amendment. It also differs from the frozen `SessionOpen` form, where an explicit `false` is legal.
- **Reproduction** (probe R3): `a2 01 00 02 f4` gives `Err(Range(2))`. `SessionOpen` with key 5 = `a1 02 f4` validates and decodes.
- **Impact:** Another encoder could write `force: false` and get `invalid_request` from `server status`, for example a hand-written or generated gwz-py encoder.
- **Correction:** Either decode an explicit `false` as false (still encoding it absent), or record a single rule for optional booleans in the design and apply it to both bodies.
- **Closure test:** A row that pins the chosen rule for both `force` and `transport_off`.

### CODE-4 (P3) The C11 hook cannot carry a refusal message that holds a value
- **Location:** `handshake/host.rs:32-37`: `Refusal { message: &'static str }`.
- **Violated invariant:** Design §17 gives `transport_session_full` the message "…has <N> open sessions, its --max-sessions". N is known only on the host. 2.3g's red row is `transport_session_full`, but 2.3g does not own `host.rs`.
- **Impact:** 2.3g would have to edit a file it does not own, leak a string, or send a less specific message.
- **Correction:** Make `message` a `Cow<'static, str>` (or a `String`). Keep the rule that it never repeats what the peer sent.
- **Closure test:** A hook row that refuses with a formatted message, and the client shows it escaped.

### CODE-5 (P3) The host's version refusal repeats the peer's value, against the module's own contract
- **Location:** `host.rs:141-147` (`format!("protocol {} is outside…", request.protocol, …)`) against `mod.rs:25-26` ("a fixed message that repeats nothing the peer sent").
- **Reproduction** (probe R4): the text is `protocol 4000000000 is outside the 1..=2 this host serves`.
- **Impact:** A number is not a secret, so the harm is small. But the documented invariant is false, and the row pins the echo.
- **Correction:** Give only the host's range, or change the doc to say a number may be repeated.
- **Closure test:** The row `a_protocol_outside_the_hosts_range_is_server_version_mismatch` asserts the chosen text.

### CODE-6 (P3) Handshake and connect failures carry no error code where design §17 gives `server_unavailable`
- **Location:**
  - `client.rs:97-104`: `code()` returns `None` for `Protocol` and `Wire`.
  - `dial.rs:180-186`: `DialFault` has no code at all.
- **Violated invariant:** §17 defines `server_unavailable` as covering a server that "did not act as a gwz server" and a connection "closed before the session opened". Design §3 says a connect that never completes "is reported as a silent listener is".
- **Impact:** These failures surface with no code: `LocalRefusal::Timeout`, a refused or missing socket, `EPIPE` while sending the open, and a reply out of order. Every caller (2.3k, `SocketCoreBridge`) has to rebuild the mapping.
- **Correction:** Add `DialFault::code()`:
  - `Connect(Timeout | Os{ConnectionRefused | NotFound})` and `Handshake(Protocol | Wire)` give 79;
  - `Connect(Peer)` gives `server_listener_refused`;
  - otherwise the `ClientFault` code.
- **Closure test:** A row per mapping.

### CODE-7 (P3) `connect` stays a production entry point that builds a carrier without the handshake, and duplicates `carrier_over`
- **Location:** `local/unix.rs` `connect` (about lines 651-660) and `local/windows.rs` `connect` (about 880-890). Only rows call them.
  - `connect_stream` and `RawConnection::into_carrier` repeat about ten lines of `carrier_over` on each platform.
  - The error labels already differ: `getrandom` from `into_carrier` and Unix, `spawn` from Windows `carrier_over`.
- **Violated invariant:** Design §3: "Every byte stream uses the handshake." A test convenience sits in production code.
- **Correction:**
  - Move `connect` to the `cfg_if!` test arm of `local/mod.rs`, or define it as `connect_stream(..)?.into_carrier(..)`.
  - Build the client path's carrier only through `into_carrier`.
  - The drafter deferred this until the 2.3a remediation lands; the lane owner should schedule it, not park it.
- **Closure test:** The build and the 2.1 rows pass with `connect` absent from non-test builds.

### CODE-8 (P3) The dead-code allowance on `handshake` gives a reason that is now stale
- **Location:** `carrier/mod.rs:46-52`: "first production callers: plan steps 2.3d-2 and 2.3g".
- **Problem:** 2.3d-2 has landed, yet `dial` still has no production caller; its first are 2.3i and 2.3k. `accept_open`'s first caller is 2.3g.
- **Correction:** Update the reason to name the real first callers.

### CODE-9 (P3) Parity: Windows has no row for a squatter's first frame
- **Location:** `handshake_local_rows.rs:225-259`: the banner row is in the `unix` arm only. `2.3d.json` pairs `mod unix` with the Windows arm, which holds only the busy-pipe row.
- **Problem:** The same threat exists on Windows: a process of the same user that creates the pipe name first is a listener that is not a gwz server. The non-hello path over `ReadHalf` is not exercised there.
- **Correction:** Add a Windows twin: `create_first_instance`, write a banner, and `dial` fails at once with the bytes escaped.
- **Closure test:** The twin passes on the Windows receipt.

### CODE-10 (P3) The 16 KiB reply cap has no recorded provenance or bound
- **Location:** `mod.rs:56` (`REPLY_FRAME_MAX`) and `client.rs:287`.
- **Problem:** Neither the design nor the plan has this cap; it is tighter than the 64 MiB frame limit only for handshake replies. `ServerState.log` is an unbounded `str`. On Windows a long `--log` path (up to 32767 UTF-16 units, so up to about 96 KiB of UTF-8) could exceed the cap and break `server status`.
- **Correction:** Record the cap in a clerical amendment to design §3. Derive it from the largest legal `ServerState`, or bound the length of `--log`.
- **Closure test:** A row with the longest legal log path.

## 2. Invariant analysis
- **Encoding:**
  - Maps are definite, with ascending unsigned keys and shortest heads (`put_head`, `encode` sorts its entries).
  - The golden-byte and re-encode rows hold.
  - Decoding refuses duplicate, out-of-order and unknown top-level keys, trailing bytes, tags, indefinite lengths, non-UTF-8 text and values outside the u32 range. Nested maps get the order and duplicate checks but not the unknown-key check, which is right for raw `limits`.
  - Exception: CODE-2.
- **`SessionOpen` shape:**
  - `validate_session_open` is the gate on both sides.
  - The client's `OpenRequest` has no field for an environment, `umask`, `openssl` or `transport_off`, so the canary cannot leave the client.
  - Key 6 must be `f5`, and key 5 may hold at most `{2: false}`. Confirmed by `m22`, `m33` and the canary row.
  - The client checks only the shape of caller-supplied `limits`, not its canonical form (validate only). The host refuses a non-canonical form with `invalid_request`, so the failure is safe.
- **Version range:**
  - The client picks `min(host_max, own_high)`, refuses `server_version_mismatch` when that is below `max(host_min, own_low)`, and sends nothing.
  - The host refuses a `SessionOpen.protocol` outside its range with code 80.
  - A hello with `min = 0` or `max < min` is malformed (`server_unavailable`). That is acceptable.
- **`transport_capabilities`:** It is a call after `SessionOpened` (split doc §4.8), not part of the handshake. None is expected here, and none is done.
- **Dispatch:**
  - Tag 4 runs the hook and answers `SessionOpened`. Tag 7 answers `ServerState` and closes, without running the open hook. Any other tag closes with no reply.
  - Over a real carrier, a malformed tag 4 is refused by `inbound.rs` before delivery. The texts are worded differently but keep the same meaning.
- **C11:** `accept_open(wire, hello, bounds, hooks)` runs `Hooks::open` after the shape, canonical-form and range checks and before `SessionOpened` is written (`m31` is red). 2.3g needs only `CarrierWire`, `Hooks`, `OpenRequest` and `HostOutcome`; there is no reaching into internals. Two limits:
  - CODE-4.
  - `limits` arrives as raw CBOR, and no public decoder exists for it (see §4).
- **Bounds:**
  - Both bounds count from the start of `dial`, and the deadline is shared by `connect_stream` and the handshake (`m19`, `n05`).
  - Linux retries `EAGAIN` with a backoff capped at 50 ms. macOS fails at once on `ECONNREFUSED`. Windows waits with `WaitNamedPipeW` for the time left; `.max(1)` avoids the zero-means-default case, and a 10 ms sleep prevents spinning.
  - The mapping differs by design: `Timeout` on Linux and Windows, `Os(ConnectionRefused)` on macOS.
- **No byte lost:** The helper thread reads into a buffer of exactly the count asked for, so it never reads past the handshake. The real-carrier row with the frame written right after `SessionOpened` passes.
- **Plan row "a client that sends before a well-formed hello is a protocol error":** It is implemented as the client's discipline, sending nothing before a well-formed hello (`m11`). The host cannot tell a frame sent before its hello from one sent after, since it writes the hello at once. I accept this interpretation and record it here, not as a finding.
- **Repository rules:**
  - Braces on every body: rustfmt; no unbraced forms.
  - Conditional compilation only inside `cfg_if!`: checked.
  - No globals or thread-locals: none.
  - No production code under `tests/`: none.
  - Parity: see CODE-9.

## 3. Risks and next action
- **Risk:** Without CODE-1, the most common failure for sandboxed users (Flatpak, containers, the macOS sandbox) is reported as a dead server. The 2.3k surface would bake that in.
- **Next action:**
  1. The drafter fixes CODE-1 with its rows.
  2. The lane owner records the §3/§4 amendment and decides each P3 (close now, or record with an owner step). CODE-4, CODE-6 and CODE-7 are cheapest before 2.3g and 2.3k start.
  3. Re-verify on the Mac, Linux and the Windows receipt. CODE-1's real-carrier row runs on all three platforms.

## 4. Out-of-scope observations
- **`SessionLimits` ceilings have no owner.** Design §3 says a limit above a socket host's default is refused with `invalid_request`. No 2.3 step names this; the drafter assigns it to 2.3g, whose text does not mention it. No `SessionLimits` decoder exists in gwz-core. **Hypothesis of a plan gap; the lane owner should assign it.**
- **Transport frames before `SessionOpened` are not a protocol error.** The 2.1 carrier routes tags 16 and 17 before the open (`stream/inbound.rs`). Split doc §3.3 makes such a frame a protocol error, and the handshake cannot see it through `recv_registry_timeout`. No state is created, since no binding exists yet. **This is a limitation of the 2.1 carrier and the 2.3g router, not a defect of 2.3d.**
- **A possible flake in a 2.1 row.** `local_rows::a_frame_set_round_trips_over_a_real_local_carrier_between_two_threads` failed once on Linux at load average about 60. It passed in three reruns at load 9 to 20, and I did not keep the panic text. **Unconfirmed; worth a watch on loaded hosts.**
- **Cleanup.** My copy, both target directories, the image tag and the volume are removed. Only `/Volumes/projects/limbo/build-targets/review-p2-3d-code-tools/` remains: the adapted drivers, the mutation logs, the probe source and the baseline logs.
