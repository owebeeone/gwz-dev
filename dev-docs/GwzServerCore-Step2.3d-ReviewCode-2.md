# Step 2.3d (session handshake and bounded connect) — Code-AXIS RE-VERDICT, round 2
**Review object:** the gwz-core remediation `git diff 93baab84 84efc96b`, in lane `/Volumes/projects/limbo/gwz-dev-p2-3d2`. Evidence is `2026-10-31-p2-3d2/rem1/`, and the remediation plan is `dev-docs/GwzServerCore-Step2.3d-RemPlan.md`.
**Tuple:** root `efe75a42`, gwz-core `84efc96b` (working tree clean), gwz-core-evidence `c03fa5d6`. I checked it at the start, again after the host app restarted, and at the end. It did not move.
**Date:** 2026-10-31
**Axis:** Code: architecture, interfaces, wire format and compatibility.
**Verdict: GO.** All ten round-1 findings are closed on this tree (CODE-1, the P2, and CODE-2 to CODE-10). Two new P3 findings come from the implementer's own deviations; neither blocks and neither is architectural. CODE-9, and the Windows half of CODE-1's and CODE-7's closures, depend on the Windows receipt `p2-3d2-20261031-a`, which is still running and which I have not seen.

## 0. Evidence base
- **Copy.** I made an `rsync -aL` copy of the lane under `build-targets/p2-3d2-rev-code`, with its own target directories. Before each run and at the end I compared the copy with the lane file by file and found no differences.
  - The host restart killed one of my mutation runs, which left `dial.rs` mutated in the copy. I restored it from the lane and re-ran that mutation.
  - The copy, the image tag `p2-3d2-rev-code-img` and the volume `p2-3d2-rev-code-cargo` are all removed.
  - The probes and mutation logs are kept in `/Volumes/projects/limbo/build-targets/review-p2-3d-code-tools/round2/` and `review2_probe_rows.rs` beside it.
- **Mac** (candidate shape): `transport_host::carrier`, 177 passed.
- **Linux** (OrbStack container, candidate shape): 187 passed. The thread-count row `failed_dials_leave_no_helper_thread_behind` passes on its own.
- **WSL matrix** `p2-3d2-20261031-a`: OVERALL PASS. I read `remote-p2-3d2-20261031-a/summary.txt`.
- **Closure mutations** (the implementer's driver, with paths changed to my copy). Every one was red and every file was restored:
  - Mac: `r01` (both refusal rows), `r04`, `r13`, `r17` (three rows), `r19`, `r21`.
    - My first `r19` run failed to link, an artifact of the interrupted build. On re-run it was red.
  - Linux: `r15`, `r24` (the parent and child thread rows).
- **Source checks**, run read-only on the lane with `python3.13 -B`: `check_cfg_boundaries`, `check_windows_parity`, `check_process_globals` and `check_carrier_peer_rule` all exit 0.
- **Text.** I read the split doc's §3.1 amendment of 2026-10-31.

## 1. Prior-finding closure table

| ID | Disposition | Evidence I checked on this tree | Status |
|---|---|---|---|
| CODE-1 (P2): the listener's refusal came back as `server_unavailable` | Fix: a tag-3 first frame that `decode_refusal` accepts with `call_id` 0 becomes `Refused`, escaped, with nothing written. Plus the §3.1 amendment. | The round-1 counterexample (probe R1, a real listener with `other_user_policy()`) now gives `Handshake(Refused { code: 82, message: "peer refused: the peer is another user" })`, and `DialFault::code()` is `Some(82)`. Before, it gave `NotAHello`, code 79. The real-carrier row and `r01` are red on the Mac. The 4 KiB cap is still checked first (`client.rs`, before the tag-3 arm). | **Closed** (Mac and Linux). The Windows real-pipe run is in the receipt. |
| CODE-2 (P3): floats and two-byte simple values accepted | Fix: `head` refuses major type 7 except `0xf4` and `0xf5` | Probe R2: `f9 00 14`, `fa 00 00 00 15`, `f8 14` and `81 f9 00 15` inside `limits` are each `Malformed`, and the host refuses each with `invalid_request`. `a1 01 f5` is still accepted. `r04` is red. | **Closed** |
| CODE-3 (P3): optional booleans had two rules | Fix: an explicit `false` decodes as false and is encoded absent | Probe R3: `a2 01 00 02 f4` gives `Ok(Control { force: false })`, which re-encodes as `a1 01 00`. This matches `transport_off`. | **Closed** |
| CODE-4 (P3): the hook's refusal text was `&'static str` | Fix: `Cow<'static, str>` | Probe R4b: a hook's `format!("has {} open sessions", 64)` reaches the peer as one `SessionError` with code 77. | **Closed** |
| CODE-5 (P3): the docs promised a fixed refusal text | Fix (doc) | The docs of `handshake/mod.rs` and `host.rs`, and the §3.1 amendment, now say a refusal may name a number the peer sent. Probe R4 shows the text is still `protocol 4000000000 is outside the 1..=2 this host serves`, which now matches the docs. | **Closed** |
| CODE-6 (P3): dial and handshake failures had no code | Fix: `DialFault::code()` | I read the mapping: `Timeout`, `ConnectionRefused`/`NotFound`, `Protocol` and `Wire` give 79; `Peer` gives 84; `Request` gives 1; anything else takes the `ClientFault` code. Probe R6: a reply over the cap through `DialFault` gives 79. `r13` (Mac) and `r15` (Linux full queue) are red. | **Closed.** The residual is NEW-2. |
| CODE-7 (P3): `connect` was a production path with no handshake | Fix: `raw_over` on each platform; `connect` = `connect_stream(..)?.into_carrier(..)`, test arms only | `fn connect(` appears only at `local/mod.rs:90` (`all(test, unix)`) and `:108` (`all(test, windows)`). Both platforms' `carrier_over` and `connect_stream` go through one `raw_over`, so the error labels agree. The 2.1 rows pass on the Mac and Linux. | **Closed.** The Windows `raw_over` is run only by the receipt. |
| CODE-8 (P3): stale dead-code reason | Fix (doc) | `carrier/mod.rs` now names 2.3g (`accept_open`) and 2.3i and 2.3k (`dial`). | **Closed** |
| CODE-9 (P3): no Windows squatter row | Fix: the banner row is shared across platforms; Windows uses the test-only `squat` | The row is outside every platform gate (`handshake_local_rows.rs:501`), and the Windows `squatted` helper exists (`:419`). It is green on the Mac and Linux. On Windows it was only type-checked. | **Closed pending the Windows receipt** |
| CODE-10 (P3): the 16 KiB reply cap had no provenance | Fix and record: 128 KiB, with the reason in the doc and the §3.1 amendment | The arithmetic holds: 32 767 UTF-16 units come to at most 98 301 UTF-8 bytes, and the other fields add a few dozen. The amendment records it. `r17` (the cap set back to 16 KiB) turns three rows red. | **Closed** |

The round-1 §4 observations (transport frames before the open, `SessionLimits` ceilings, the `local_rows` flake) are recorded for 2.3g and in the findings file, as the remediation plan says. I accept that. They were outside the object.

## 2. Declared deviations, judged
1. **Any code under `call_id` 0 is taken as the refusal.** Probe R1b: a first-frame `SessionError(call_id 0, code 79)` is shown as `Refused { 79 }`, with nothing written. The amendment's text names only `server_peer_refused`, and says any other first frame is `server_unavailable`. The harm is small: the listener's identity check passed before the first byte, and nothing is sent. But the code and the text disagree, so this is **NEW-1**.
2. **A reply over the cap has no code from `ClientFault::code()`; only `DialFault::code()` maps it.** Probe R6 confirms both halves: `client_handshake` alone gives `Protocol(..)` with code `None`, and through `DialFault` it is 79. This is **NEW-2**.
3. **Two copies of `connect`, one per test arm.** Accepted. Each is five lines inside a test-only `cfg_if!` arm, and the peer-rule checker forbids a plain `cfg(test)` arm there. Whether to widen the checker is the lane owner's call. It is not needed for closure.
4. **The thread-count row runs in a child process.** Accepted. It follows the precedent of `ending_rows.rs`. The parent requires the child's output line, so a child that matched no test cannot pass. `r24` turns both the parent and the child row red on Linux.
5. **New `ClientFault::TooLarge` and `DialFault::Request`.** Accepted. The request is sized before anything is connected, using the client's highest protocol, which can only make the request longer, never shorter. `r19` is red.
6. **The Windows test-only `squat` helper, and `await_client` moved out of `accept`.** I read the diff. The `ConnectNamedPipe` wait was moved unchanged, and `accept` returns the same error as before. This is production code on Windows that has only been type-checked. **CODE-9, and the Windows halves of CODE-1 and CODE-7, depend on receipt `p2-3d2-20261031-a`.** If that receipt fails a pipe row, this GO turns back into a re-check of those three.

## 3. New findings
- **NEW-1 (P3; not architectural). The client and the §3.1 amendment disagree about which first-frame refusal they accept.**
  - Location: `client.rs`, the tag-3 arm in `exchange` / `refusal`.
  - The amendment says a first frame that is not `SessionHello` is `server_unavailable`, except `SessionError(call_id 0, server_peer_refused)`. The code accepts any code.
  - Reproduction: probe R1b.
  - Correction, either:
    - (a) in the code, take a first-frame refusal only when the code is 82, and treat anything else as `NotAHello`; or
    - (b) in the text, say "a `SessionError` for `call_id` 0 (this release's hosts send `server_peer_refused`)".
  - Closure test: a scripted row with a first-frame `call_id`-0 refusal of code 79 that pins whichever rule is chosen.
- **NEW-2 (P3; not architectural). The `server_unavailable` mapping lives in `DialFault` and not in `ClientFault`.**
  - Location: `client.rs` `ClientFault::code()` returns `None` for `Protocol` and `Wire`; `dial.rs` maps them.
  - The amendment says a reply over the cap is `server_unavailable`. The stdio local form and the SSH form (design §15 and §16, step 2.3j) will call `client_handshake` over their own `ByteWire`, not `dial`, and so get no code.
  - Every `Protocol` and `Wire` fault is a failure before `SessionOpened`, which is §17's own definition of `server_unavailable`.
  - Reproduction: probe R6.
  - Correction: return 79 from `ClientFault::code()` for `Protocol` and `Wire`, and let `DialFault::Handshake` delegate to it.
  - Closure test: a scripted row asserting `client_handshake`'s code for a reply over the cap and for a reply out of order.

## 4. Verdict
**GO.** 0 P0, 0 P1, 0 P2. NEW-1 and NEW-2 are P3, both non-blocking. Each should either be fixed in this step or recorded by the lane owner for 2.3j, whose stdio and SSH clients are the first callers of `client_handshake` without `dial`. The GO assumes a green Windows receipt for CODE-9 and for the Windows halves of CODE-1 and CODE-7.
