# Step 2.3d round 2 — State-AXIS RE-VERDICT
**Review object:** gwz-core `93baab84..84efc96b` in lane p2-3d2; evidence `rem1/` at gwz-core-evidence `c03fa5d6`. Remediation plan: `dev-docs/GwzServerCore-Step2.3d-RemPlan.md`.

**Tuple:** lane root `efe75a42`, gwz-core `84efc96b`, evidence `c03fa5d6`. Read-only git at the start and the end gave the same three values, and gwz-core was clean (0 status lines). The tuple did not move.

**Baseline:** the round-1 commit objects (`aaa7c23a`, `07053e90`) are not in this lane, so `93baab84` (the re-landed 2.3d) is my baseline, as the coordinator directed.

**Date:** 2026-10-31

**Verdict: GO.** All nine of my round-1 findings are closed. F2 is closed by a recorded disposition to 2.3g. There is one new P3. There is no P0, P1 or P2, and no new architectural root cause.

## Evidence I ran
- **Mac:** macOS 26.6.2, candidate shape. `transport_host::carrier`: **177 passed**, which matches the evidence.
- **Linux:** OrbStack container, image `p2-3d2-rev-state-img`, volume `p2-3d2-rev-state-cargo`. `transport_host::carrier`: **187 passed**, which matches the evidence.
- **My round-1 hostile rows, adapted to the new types, plus five new ones (rv13–rv16), on both platforms with one test thread.** All of them behave as the closure table below says. The one red result is in rv11, and it is my own threshold mistake: a 4056 B client string makes a 4069 B request, which is under the 4096 B cap, so it is sent, as designed.
- **Evidence I read:**
  - the rem1 README closure table;
  - `raw/mutations-mac.log`, `mutations-mac-2.log` and `mutations-linux.log`: r01–r25 red, every one `restored=True`. r24's first Mac run was green; it was explained and red on the re-run;
  - `raw/threads-{mac,linux}.log`: "before 3, after 3".
- **Plan and design text:** the plan's 2.3g carry-in (L163) and the split doc §3.1 amendment (L87–91).
- **Clean-up:** my copy, target dirs, image tag and volume are removed. My row file and logs are kept in `build-targets/review-p2-3d-state-tools/round2/`. The `p2-3d2-rev-code` directory is not mine and I left it untouched.

## Prior-finding closure table
| ID | Disposition | Evidence checked on this tree | Status |
|---|---|---|---|
| F1 — request write outside the bound | Fix: `request_bytes` runs in `dial` before the connect; over `FIRST_FRAME_MAX` gives `DialFault::Request(TooLarge)` with `invalid_request` | **rv11:** the 4 MiB client string is refused in 2.5 ms (Mac) and 1.6 ms (Linux) with code 1, before any connect. 4096 B is refused too (4108 B request); 4056 B is sent. **My check:** sizing at `supported.end()` is safe, because a lower protocol number never encodes longer. **Lane:** the lane row is green and mutations r19/r20 are red | **Closed** |
| F2 — lane frame before `SessionOpen` silently discarded | Recorded for 2.3g | **rv12** still reproduces (`Opened`, tags `[6,5,17]`), as expected for a carried item. Plan L163 carries the rule with a red row in 2.3g. I accept the deferral: no state is created, and 2.3g owns the router | **Closed by disposition** |
| F3 — connect timeout has no `server_unavailable` code | Fix: `DialFault::code()` | **rv16:** missing address and stale socket both give 79 on Mac and Linux. **rv15b:** a reply over the cap gives 79. **Lane:** the full-queue dial row asserts 79 on Linux, mutation r15 is red, and mutations r10–r14 are red | **Closed** |
| F4 — short banner reported as plain silence | Fix: `Heard::Silent(Vec<u8>)`, a progress buffer in the pump, `Cause::Silent{first}` | **rv13:** `"$ "` is shown escaped after the bound. **rv15a:** only the reply's three bytes (`"ABC"`) are shown, not the hello's, so the per-read reset works. **rv01:** the partial body is shown. **My check:** the buffer is cleared before each request and only one read is ever in flight, so there is no race. **Lane:** mutations r21/r22 are red | **Closed** |
| F5 — refusal docs overstate the wire | Fix (doc) | **Text:** `mod.rs` and `host.rs` now name the close announcement and the numeric exception; the split doc §3.1 amendment agrees. **Wire, rv06/rv07:** `[6,3,17]` and `[6,8,17]` with close body `[0,0]`, matching the text | **Closed** |
| F6 — rollback owner unstated after hook success | Fix: doc plus a row | `Hooks::open` and `accept_open` docs read. Row `a_session_opened_that_cannot_be_written_is_a_wire_fault_after_the_hook_ran` asserts the hook ran, `Err(Wire)` and the wire closed. Mutation r23 is red | **Closed** |
| F7 — `WaitNamedPipeW` could become wait-forever | Fix: `.clamp(1, u32::MAX - 1)` | Read in `windows.rs` `connect_checked`. It was type-checked for the Windows target, which is sufficient for this review-only closure | **Closed** (receipt not needed) |
| F8 — decoder accepts floats and two-byte simple values | Fix: `head` refuses major type 7 except the bytes `0xf4`/`0xf5` | **rv10:** `f9 00 14`, `f8 14` and `fb…15` are all `Malformed`; `validate_session_open` still passes them, but the decoder no longer lets them reach the hook. **Lane:** mutation r04 is red | **Closed** |
| F9 — no row for helper-thread and fd cleanup | Fix: child-process thread-count row | Child logs show 3→3 threads on Mac and Linux; mutation r24 is red on both. **rv05:** threads 2→2, fds 5→65 with the host holding 60, so no client fd leaks. **Gap:** the lane row counts threads, not fds. I accept that, because my round-1 correction asked only for a thread count | **Closed on Mac and Linux.** The Windows arm (Toolhelp32) **depends on receipt `p2-3d2-20261031-a`** |

## The six declared deviations
1. **The client takes any `SessionError` code under call_id 0 as the refusal.** This fails closed:
   - rv14 shows the client writes 0 bytes and fails in under 0.5 ms;
   - the message is escaped (`\x1b`, `\x07`);
   - a non-zero call_id and an over-4 KiB refusal both stay `NotAHello`/79.

   The cost is that a non-server listener of the same user can choose the reported code (see N1). Acceptable, with text alignment.
2. **A reply over the cap has `ClientFault::code() == None`, mapped to 79 only by `DialFault::code()`.** Acceptable, since `dial` is the client entry point. If 2.3j's stdio client calls `client_handshake` directly, it must apply the same mapping. That is an observation, not a finding.
3. **Two copies of `connect`, one in each test arm.** This is test-only, about 8 lines each, forced by `check_carrier_peer_rule.py`, and has no effect on state. Acceptable.
4. **F9 runs in a child process.** This is sound: the parent requires the child's output line, so a child that silently does nothing fails, and the parent's thread count is not polluted. Acceptable.
5. **`ClientFault::TooLarge` and `DialFault::Request`.** These are the correct shape for F1: nothing is connected, and the code is `invalid_request`. Acceptable.
6. **Windows test-only `squat` and `await_client` moved out of `accept`.** `squat` sits under the existing `cfg_if!` test block. `await_client` is the old `ConnectNamedPipe` wait moved out unchanged; only its `if !connected return` became an if/else. `raw_over` is shared by the accept path and the connect path. Its only behaviour change is that the Windows error label for a `CarrierId::random` failure is now `getrandom` instead of `spawn`. Acceptable, **pending the receipt**.

**Closures and rows that depend on the Windows receipt `p2-3d2-20261031-a`:**
- F9's Windows arm.
- The Windows run of every pump-path change that F1, F3 and F4 touch: the progress buffer over `Pipe`, the busy-pipe row after the `raw_over`/`await_client` refactor, and the dial-to-nothing row on Windows.

F1, F3, F4, F5, F6, F7 and F8 are closed on Mac and Linux evidence alone.

## New finding
**N1 (P3, not architectural) — The first-frame refusal code is whatever the listener chooses, and the code and the amendment text disagree.**
- **Location:** `handshake/client.rs` `exchange`, the tag-3 arm at `Stage::Hello`.
- **The disagreement:** the split doc §3.1 amendment (L89) says the first-frame refusal is `SessionError(call_id 0, server_peer_refused)`. The code accepts any code.
- **Reproduction:** rv14 `code80`. A squatter sends `SessionError(0, 80, …)` as its first frame. `dial` reports "the host refused (code 80)", and `DialFault::code()` is `Some(80)` (`server_version_mismatch`), although no hello and no version exchange took place.
- **Impact:** diagnosability only. It fails closed and nothing is sent. Only a listener of the same user (the peer check passed) can pick the user-facing code.
- **Correction:** either accept only 82 at `Stage::Hello` and treat any other code as `NotAHello`/79, or amend L89 to say "a `SessionError(call_id 0)`, whatever its code". The second fits if 2.3i's shutdown is to refuse new connections before the hello.
- **Closure test:** rv14 `code80` gives the chosen outcome, with the amendment text matching it.

## Note (not a finding)
`Cause::Silent`'s message prints `waited.as_secs()`, so a short bound reads "no answer in 0s". At the production bounds of 30 s and 5 s it reads correctly.

**Verdict: GO.** 0 P0, 0 P1, 0 P2. Open: N1 (P3). I pre-commit that N1 does not change this verdict whichever correction the lane owner chooses. The Windows-arm items named above stand on the pending receipt `p2-3d2-20261031-a`.
