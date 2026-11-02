# Step 2.3d (session handshake and bounded connect) — remediation plan, round 1

**Status (2026-10-31):** round-2 re-verdicts `GwzServerCore-Step2.3d-ReviewCode-2.md` and `-ReviewState-2.md` both report **GO** at lane p2-3d2 root `efe75a42` / gwz-core `84efc96b` / gwz-core-evidence `c03fa5d6`; WSL matrix `p2-3d2-20261031-a` PASS. Windows receipt `p2-3d2-20261031-a` PASSED (4440 passed; the 3 named host gaps), closing CODE-9, the Windows halves of CODE-1 and CODE-7, and F9's Windows arm. Blind convergence: Code NEW-1 = State N1 (any `call_id`-0 code accepted as the first-frame refusal, against the §3.1 amendment's text); disposed by amending the text (option (b)): `GwzSplitCommandChannel.md` §3.1 amendment now says a `call_id`-0 `SessionError`, this release's hosts sending `server_peer_refused`. Code NEW-2 (P3) is carried into 2.3j (`GwzServerCore-Step2.3-Plan.md`, after 2.3j). No architectural root cause in round 2.

Status: **accepted at lane p2-3d2 root `83bd4c8f` / gwz-core `84efc96b` / gwz-core-evidence `c03fa5d6` after `GwzServerCore-Step2.3d-ReviewCode-2.md` and `-ReviewState-2.md` reported GO; this accepts step 2.3d only.**

**Object:** gwz-core `aaa7c23a` (2.3d-1) and `07053e90` (2.3d-2), lane p2-3d.

**Reviews:**
- `GwzServerCore-Step2.3d-ReviewCode.md`: NO-GO, P2 CODE-1, P3 CODE-2 to CODE-10.
- `GwzServerCore-Step2.3d-ReviewState.md`: GO, P3 F1 to F9.

**Blind convergence:**
- CODE-2 = F8: the strict decoder accepts floats and two-byte simple values.
- CODE-6 = F3: a connect or handshake failure carries no `server_unavailable` code.
- CODE-5 ≈ F5: the refusal docs overstate. The version text repeats the peer's number, and the close announcement after a refusal is not mentioned.
- CODE-10 ≈ State §1 note: the 16 KiB reply cap has no recorded provenance.
- Code §4 = F2: a transport-lane frame before `SessionOpen` is discarded, not a protocol error.

**Architectural root causes on this object:** none.

**Re-landing.** Root moved under the lane. 2.3a landed, and it edits `local/unix.rs` (`check_peer`, `Hold`) and the parity inventory. So the two commits are re-applied as patches in a fresh lane p2-3d2 cloned from root. The shared parity files (`2.1.json`, `meta.json`) are regenerated, not hand-merged. The remediation is one further patch.

**Next round:** re-verdicts by the same two reviewers, with their context intact. The changes stay inside 2.3d's own new surface, plus the test-only move of 2.1's `connect`, which CODE-7 asked for.

**Design amendment:** CODE-1 resolves a conflict between design §3 and §4/§17. The lane owner records it as a clerical amendment to the split doc §3.1, in the same form as the 2.1 amendment of §3.3. The parked design is not edited. The reviewers judge the text as part of the object.

| ID | Disposition | Closure test |
|---|---|---|
| CODE-1 (P2) | **Fix.** At `Stage::Hello`, after the length check (the 4 KiB cap still applies), the client takes a tag-3 frame whose body `decode_refusal` accepts with `call_id` 0 as `ClientFault::Refused { code, message }`. The message is escaped and nothing is written. Any other non-hello first frame stays `NotAHello`. **Amendment** (split doc §3.1): "the host's first frame is `SessionHello`, or, for a peer its listener refused, exactly one `SessionError(call_id 0, server_peer_refused)` and then the close; a client takes that frame as the refusal and shows its cause (server design §4 and §17); any other first frame is `server_unavailable` (§3)". | A real-carrier row on all three platforms: a listener with `other_user_policy()`; `dial` gives `Refused { code: server_peer_refused }`, its message names the cause, and nothing is written. A scripted row: a first-frame `SessionError` with `call_id` ≠ 0 stays `NotAHello`. Dropping the tag-3 arm turns the real-carrier row red. |
| CODE-2 = F8 (P3) | **Fix.** In `body.rs` `skip` and `head`, major type 7 is accepted only as the single bytes `0xf4` and `0xf5`. | The four inputs `a1 01 f9 00 14`, `a1 01 fa 00 00 00 15`, `a1 01 f8 14` and `a1 01 81 f9 00 15` are each `Malformed` in `nested_items_are_held_to_the_same_rule`. |
| CODE-3 (P3) | **Fix.** One rule for optional booleans: an explicit `false` decodes as false, and encoding writes it absent. This matches the frozen `SessionOpen` `transport_off`, and `Control::decode` now accepts `force: false`. | A row pins `force: false` and `transport_off: false`, both accepted and re-encoded absent. |
| CODE-4 (P3) | **Fix.** `Refusal.message` becomes `Cow<'static, str>`. The doc keeps the rule that it never repeats text the peer sent. | A hook row refuses with a formatted message (a count), and the client shows it escaped. |
| CODE-5 = F5 (P3) | **Fix (doc).** `handshake/mod.rs` and `host.rs` say a refusal is one `SessionError` and then the carrier's close (its close announcement), and that a refusal never repeats text the peer sent but may name a number it sent, such as the refused protocol version. | The version row asserts the chosen text; text review. |
| CODE-6 = F3 (P3) | **Fix.** Add `DialFault::code()`. `Connect(Timeout)`, a refused or missing socket, `Handshake(Protocol)` and `Handshake(Wire)` give `server_unavailable`. `Connect(Peer)` gives `server_listener_refused`. Otherwise it gives the `ClientFault` code. | One row per mapping; the full-queue dial row also asserts `code() == Some(server_unavailable)`. |
| CODE-7 (P3) | **Fix.** 2.1's `local::connect` (rows only) is defined as `connect_stream(..)?.into_carrier(..)` and moves into the `cfg_if!` test arm of `local/mod.rs`. The client path builds its carrier only through `into_carrier`. `into_carrier` and the listener side's `carrier_over` share one helper on each platform, so the error labels agree. | The build and the 2.1 rows pass with `connect` absent from non-test builds. One helper exists per platform. |
| CODE-8 (P3) | **Fix (doc).** The `handshake` dead-code allowance names its real first callers: 2.3g (`accept_open`), and 2.3i and 2.3k (`dial`). | Text review. |
| CODE-9 (P3) | **Fix.** Add a Windows twin of the banner row: `create_first_instance`, write a banner, and `dial` fails at once with the bytes escaped. | Green on the Windows receipt. |
| CODE-10 (P3) | **Fix and record.** `REPLY_FRAME_MAX` covers the largest legal `ServerState`, with a log path of up to 32 767 UTF-16 units on Windows (up to 98 301 UTF-8 bytes): 128 KiB. The split doc §3.1 amendment records the cap and why it exists. | A row with the longest legal log path passes; a reply one byte over the cap is refused. |
| F1 (P3) | **Fix.** `dial` refuses a request whose encoded `SessionOpen` or `ServerControl` exceeds `FIRST_FRAME_MAX` before connecting, as `invalid_request`. | The reviewer's rv11 shape (a 4 MiB `client` string) returns at once with that refusal. |
| F2 = Code §4 (P3) | **Record for 2.3g,** which owns the router. Before `SessionOpened` is written, a transport-lane frame for an unopened binding ends the carrier as a protocol error (split doc §3.3 "When"). It needs a red row there. The handshake cannot see it through `recv_registry_timeout`. | Carried into the 2.3g brief. |
| F4 (P3) | **Fix.** `Heard::Silent` carries the partial bytes received before the bound, and the message shows them escaped. | A row that sends 2 bytes and goes silent: the message contains the escaped bytes. |
| F6 (P3) | **Fix.** `Hooks::open` documents that the caller owns rollback on any `Err` after the hook returned `Ok`. A scripted row whose tag-5 send fails asserts both that the hook ran and that `Err(Wire)` is returned. | That row. |
| F7 (P3) | **Fix.** `.min(u32::MAX - 1)` on the `WaitNamedPipeW` wait, so it can never become `NMPWAIT_WAIT_FOREVER`. | Review. |
| F9 (P3) | **Fix.** A row on all three platforms: 20 failed dials against a squatter leave the process's thread count where it was (`/proc/self/status` `Threads:` on Linux, `proc_pidinfo` on macOS, Toolhelp32 on Windows). | Green on Mac, Linux and the Windows receipt. |
| Code §4: `SessionLimits` ceilings | **Record for 2.3g.** Design §3 says a limit above a socket host's default is refused with `invalid_request`. 2.3g decodes `limits` and owns that rule, with a red row. | Carried into the 2.3g brief. |
| Code §4: `local_rows` flake at load about 60 | **Record** in the findings file (unconfirmed; watched). | — |
| State §4: `local::connect` future | Closed by CODE-7. | — |

**Gates:**
- the carrier rows on the Mac and on Linux (container), ordinary and candidate shapes;
- every closure mutation red, then restored;
- the source checks (peer guard, cfg, globals, windows parity); fmt; clippy; the candidate clippy ratchet;
- `run_boundary_job.py`;
- WSL and the Windows receipt `p2-3d2-20261031-a` (`--cache-from land-p2-3a-20261031-a`).
