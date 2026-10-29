# GwzSplitCommandChannel.md revision 2: Consistency re-verdict (with Surface section)

**Review object:** `dev-docs/GwzSplitCommandChannel.md` revision 2 (368 lines) and the in-place amendment of `dev-docs/GwzCoreSessionDesign.md` (§3, plus a renumbering in §4.2 and §13), both at lane root `2f3b7cb7` in `/Volumes/projects/limbo/gwz-dev-p0-design`. Changes reviewed: `git diff f923969d67 2f3b7cb7` on both files.

**Baseline:** lane root `2f3b7cb7a3`; members gwz-core `0ed481688d`, gwz-transport `a27817b9c8`, gwz-cli `cea7bd4cab`, gwz-py `71d2d07249`. The controlling plan is the root workspace's `dev-docs/GwzTransportReleaseConvergencePlan.md` at root `7ce0b749` (revision 5 plus the clerical record and three clerical corrections). The remediation plan is `dev-docs/GwzSplitCommandChannel-RemPlan.md` at the lane root.

**Date:** 2026-10-25

**Axis:** Consistency, with the required Surface section. This round re-verdicts my own round-1 findings (P2-1, P2-2, P3-1 to P3-11).

**Verdict: GO.** All 13 round-1 findings are closed: 2 P2 and 11 P3. The new text adds 3 P3 findings (N-P3-1 to N-P3-3) and no P0, P1 or P2. None of the new findings is architectural. The P3 findings do not block.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| P2-1 | `server start` scrubs and does not refuse; only `server stdio` refuses (§3.8, §4.3, §4.9) | **Rule:** §3.8 :179 removes the scrub list and `SSH_AUTH_SOCK` from the background server; a `--foreground` server removes them from itself before any thread starts. §3.8 :178 says `stdio` "is the only command that refuses (plan 2.3)". §4.3 :202 matches. No remaining text says `start` refuses (grep for "refuses the same way" finds nothing). **Re-run of the counterexample:** §4.9 :266 is now annotated with `SSH_AUTH_SOCK` (launchd's) and `GH_TOKEN` set, which is this host's default (`launchctl getenv SSH_AUTH_SOCK` is set). The start succeeds under §3.8. **Help:** the `gwz help server start` note at :264–265 states the scrub. **Authority:** the scrub is narrower than the plan (plan 2.3 requires only `stdio` to refuse), so nothing needs new authority. | Closed |
| P2-2 | Core keys each frame (`send(frame, lane, key)`) and routes each received frame through a core `FrameRouter`; the adapters decode nothing; §8 rows added | **Writer:** §3.3 :99, `FrameSink::send(frame, lane, key)`, where `FrameKey { class, target }` is a `session-contract` type with no GWZ or Taut type; the adapter only compares `target` for equality. **Reader:** §3.3 :102, `FrameRouter::route(frame) -> Result<(), Closed>`, given to `byte_stream(read, write, router)`. gwz-core decodes `call_id`, `request_id`, `session_id` and `stream_id` at both ends. **Same text in the session design:** :112, :114 and the new byte-stream bullets. **My closure test passes:** from §3.3 and §8 alone I can write the 2.1 writer and reader entry points and name the decoding crate. Session design :112, crate map :22 and `lib.rs:13`/`:22–25` still hold because no adapter reads a body. Crate map :40 is recorded as amended in §8 :361. **Authority:** this is a public-API change to `session-contract`, assigned to 2.1 under its per-step dual review, and traceable to D-N as a demonstrated necessity. A residual record gap is filed as N-P3-2. | Closed |
| P3-1 | Qualify session design :93; add `suite.rs:72–75` to §8 | Session design :93 now reads "in send order in-process; per target on byte streams, under the priority writer below". §8 :358 lists `contract_tests/suite.rs:72–75` as 2.1's code against the qualified :93. | Closed |
| P3-2 | Tag 17 = gwz-transport's control-reserve kinds | §3.3 :93 and :105 put every kind except `data`, `open` and `check_identity` on tag 17. That matches `gwz-transport/src/mux/mod.rs:638–641` exactly (`control = !matches!(kind, Data \| Open \| CheckIdentity)`). `closed` and the orderly kinds are now control, as plan 2.0(a) requires. Every kind is assigned exactly once, and the own-target exception keeps per-stream order. | Closed |
| P3-3 | Cite plan §1.1's row | §8 :345 cites "plan §1.1 :109 at root `7ce0b749` (:108 at `53d889a0`)". Confirmed: the row is at :109 of the root plan now. | Closed |
| P3-4 | §8 preamble "§3 only"; the §5 row relabelled as adoption differences | §8 :364 now reads "adopted with differences for the served core (session design unchanged beyond §3)", and :340 says that rows naming other session design sections record adoption differences. The §5.x contradiction is gone. The renumbering added in this revision reintroduces a "§3 only" mismatch for §4.2 and §13; that is a new root cause, filed as N-P3-1. | Closed |
| P3-5 | gwz-py's console script gains no `--server`, `--no-server` or `GWZ_SERVER`; §8 row added | §4.7 :249 and §8 :363 (server design :38, §7 :631 replaced for gwz-py). This is consistent with D-O ("gwz-py is a client only") and plan 3.2. | Closed |
| P3-6 | Correct the GWZDesign citation; add restatement rows | §3.1 :71 and §8 :351 now cite `GWZDesign.md:104, :106`. :106 holds "supplies the session's snapshot", verified. New §8 rows :352–355 cover GWZDesign :104, :106, :108 and `GwzPyPerOperationTransportDesign.md:25`, each naming the step that rewrites it. GWZDesign :108 also says "error codes 77 to 83"; that belongs to the renumbering and is filed as N-P3-1. | Closed |
| P3-7 | Only an admission-produced `open_failed` maps to the destination code; bind and identity refusals keep existing codes | §5.6 :325: a `bind_rejected(invalid_request)` is a binding fault reported as `invalid_request` (1) with the existing text at `session.rs:105–108`, and `identity_check_failed` keeps `opening.rs:394–396`. Both cited code sites hold those strings. §3.4 :113 points to §5.6. Every refusal path at §3.4 and §5.1 now has one stated code. | Closed |
| P3-8 | `server_environment_mismatch` appended but never emitted; numbering stated | §1 :30 lists 77–86, each with its producer or "never emitted" for 81. **Correction to my round-1 evidence:** I said the schema ends at 74. It does not: it ends at `capacity=76`, with `credential_helper_timeout=75` at `gwz.taut.py:840` and `capacity=76` at `:847`. The lane owner's renumbering is right. | Closed |
| P3-9 | `stop`/`status` default to the workspace's own server; `--server` with `list`/`stdio` is a usage error; `init --update` never reads `GWZ_SERVER` | §4.1 :189 and §4.6 :244 state all three. Order of precedence: `--server`, else `GWZ_SERVER`, else the workspace's server inside a workspace, else a usage error. | Closed |
| P3-10 | §4.9 shows the prompt | :272–276 show the refusal followed by `Allow this repository for this command only? [y/N] y` in a terminal. That matches §5.6 :326 and §1's limits on what a prompt can add (one exact tuple on port 443). | Closed |
| P3-11 | §1 names both shared members | §1 :24 says "exactly two members: the member lock manager … and the detached-worker record (§3.2)", which agrees with §3.2 :84 and :87. | Closed |

## Changed-range analysis

- **What changed since `f923969d67`:**
  - In the object, the header, §1, §2 ("Revision 2 changes"), §3.1, §3.3, §3.4, §3.6, §3.8, §4.1–§4.7, §4.9, §5.1, §5.3, §5.6, §7 and §8 changed.
  - In the session design: the header bullet :9, §3 (:93, the new paragraph after the protocol-error list, :112, :114, the byte-stream bullets :124–132), §4.2 :191, §13 :546 and the changelog :705.
  - The commit also adds a gwz commit marker, which is tool output.
- **Inside the dispositions:** every hunk maps to a RemPlan row (C-*, S-*, and the two carried notes) or to the lane owner's renumbering. Each of my rows was re-checked above. The Safety rows (S-P2-1 to S-P2-3, S-P3-2 to S-P3-7) belong to the other axis. I checked them only for consistency with my axis:
  - The plan class (§4.5 :226 and :233–237, §5.3 :306) is consistent with §3.4 :113 ("admits no `bind`") and §5.6.
  - The transport-class bound and fault scope (§3.3 :103–104) agree with the session design paragraph that follows its protocol-error list.
  - The close-report exemption (§3.3 :100) is consistent with the D-N writer rule. Keying the report as ordinary keeps it behind earlier ordinary frames, and earlier control frames go first anyway.
  - The macOS directory switch to `_CS_DARWIN_USER_DIR` (§1 :23) is consistent with §4.9's paths (`…/0/gwz-501/…`). On this host `getconf DARWIN_USER_DIR` is 49 bytes, so the socket path is 76 bytes, under the 103-byte `sun_path` limit.
- **Outside the RemPlan: the error-code renumbering (lane owner).**
  - **Necessity is demonstrated:** the schema already uses 75 and 76. The new numbers 77–86 are consistent across §1 :30, §8 :356, session design :191 and :546, and the changelog.
  - **Authority:** D-N authorizes amending session design §3 only. Changing §4.2 and §13 in place is justified as a correction of a factual collision in numbers no code has used yet, and the names stay the identity ("2.2b assigns the numbers at the schema's tail").
  - **Recording is incomplete:** see N-P3-1.
- **D-N's writer and reader text is still verbatim** in §3.3 :98 and :101 and in the session design. The new interface bullets add mechanism without changing the decided rule.
- **No new architectural root cause.** The push-model `FrameRouter` and the core-computed `FrameKey` stay inside the existing crate boundaries: bytes-only crates, with core doing all decoding and classification.

## Surface section: re-walk of §4.9 where it changed

- **Steps 1–2, `gwz --help` then `gwz help server start`:** the help now says the server runs without the caller's agent or credential variables (:264–265). No guess needed.
- **Step 3, `cd ~/ws && gwz server start` with launchd's `SSH_AUTH_SOCK` and `GH_TOKEN` set:** succeeds under §3.8 :179 and §4.3 :202. The printed address uses the new per-user directory.
- **Steps 4–5, `export GWZ_SERVER=…` then `gwz fetch` in a terminal:** refusal, prompt, `y`, and the fetch runs. This matches §5.6, and the next line with `--allow-destination` runs without a prompt.
- **Second shell, status:** `cd ~/ws && gwz server status` now reaches the workspace's own server with no address (§4.1 :189). `gwz server list` is unchanged.
- **Stop:** `gwz server stop` works through the exported `GWZ_SERVER`, or inside the workspace without it.
- **Still undocumented, not filed:**
  - the walkthrough never runs `unset GWZ_SERVER` after the stop, so later commands in that shell fail;
  - the cleared `ws-<key>.lock` and the `ws-<key>.log` stay in the per-user directory.

  Both are left to 6.4's help.
- **Two-machine form:** §4.4 :215 adds `SetEnv`, keepalive and the allow-listed environment. A user can follow it without guessing. Its release status is filed as N-P3-3.

## 0. Evidence base

- **Tuple:** verified at the start and the end, unchanged both times: lane root `2f3b7cb7a3`; members `0ed481688d`, `a27817b9c8`, `cea7bd4cab`, `71d2d07249`; root `7ce0b74992`. `git diff --quiet HEAD` is clean for both object files. `git log f923969d67..HEAD` shows `2694c595` and `3e308fa5` (the filed reviews and the RemPlan) and `2f3b7cb7`.
- **Read:**
  - the RemPlan (50 lines), in full;
  - the object at `2f3b7cb7`, all 368 lines;
  - the session design diff and its current lines 1–12, 90–135, 191, 530–546 and 705;
  - the word-level diff of the object;
  - `git -C /Volumes/projects/limbo/gwz-dev show 7ce0b749`;
  - the root plan's header :18–23, row :109 and Phase 6 :719–730.
- **Code checked as reference:**
  - `gwz-core/protocol/gwz.taut.py:830–847` (the error-code tail) and `:1148–1151` (`dry_run`);
  - `gwz-core/src/transport_host/session.rs:100–110`;
  - `gwz-core/src/transport_host/session/driver/opening.rs:390–398`;
  - `gwz-transport/src/mux/mod.rs:620–656`;
  - `gwz-transport/src/sequenced.rs:192` (from round 1);
  - `GwzCoreSessionCrateMap.md:40–41`;
  - `session-contract/src/contract_tests/suite.rs` (17 `recv` uses);
  - `gwz-core/dev-docs/GWZRequirements.md:116` and `GWZDesign.md:108`;
  - `gwz-core/docs/ErrorCatalog.md:33–34`.
- **Grep:** active root and member docs, for stale 75/76/77–83 code numbers.
- Nothing was written, built or mutated.

## Problem, requirement provenance, and scope

- **Requested outcome:** close round 1 of step 2.0 within D-N and D-O, without reopening their outcomes. Editable: root `dev-docs/` only. Members are reference-only.
- **Scope held:** revision 2 edits only the two root documents, plus the gwz marker.
- **New constraint provenance:**
  - `FrameKey` and `FrameRouter`: a demonstrated necessity from D-N. The writer cannot apply its own-target exception without a key, and the crate map forbids decoding in the adapter.
  - Scrub instead of refuse: narrower than the plan.
  - Plan-class narrowing: traced to D-H (b), "service class of its own command".
  - Tag alignment: traced to plan 2.0(a) ("terminal frames stay in the control reserve").
  - Renumbering: demonstrated necessity (a schema collision).
  - The `ssh://` status sentence: from my own round-1 risk note, but it does not match plan 6.3 (N-P3-3).
- **No unauthorized expansion.**
- **Out-of-scope observations (not findings):**
  - (O5) The root plan's clerical-record paragraph (:18–23) still ends "Nothing else changed", although `7ce0b749` added three inline corrections, each marked "corrected 2026-10-25" where it appears.
  - (O6) `gwz-transport` classifies differently in two layers. `sequenced.rs:192` treats every non-`data` kind as control; `mux/mod.rs:638–641` excludes `open` and `check_identity`. The object follows `mux`, which is the attachment layer the lane carries, and that is correct. 2.1 should not cite `sequenced.rs` for the tag split.
  - (O7) A plan-class refusal (§5.3 :306) goes through §5.6's refusal text "followed by the flag that would allow it", but no flag can allow an `open` under a dry run. This is reached only when a core misbehaves; the Safety axis should judge the message.

## 1. Findings

### N-P3-1: The renumbering is recorded incompletely, and two "§3 only" statements now contradict it

- **Location.**
  - `GwzSplitCommandChannel.md:340`: "The session design is … amended in place for §3 only."
  - `:356`: "the session design's §4.2 numbers are amended in place (clerical)". This row lists session design §4.2 :183 and server design §9 and §17 only.
  - `GwzCoreSessionDesign.md:9`: the header says "for §3 only", then names §4.2's renumbering but not §13.
  - The commit also changed session design §13 :546 (`transport_session_full` (77), `operation_expired` (78)).
  - Unrecorded restatements of the old numbers in active member docs: `gwz-core/dev-docs/GWZRequirements.md:116` ("error codes 77 to 83 after `operation_expired` (76)") and `GWZDesign.md:108` ("error codes 77 to 83, `server_unavailable` to `server_session_closed`").
- **Invariant:** §8 is the complete record, and the "amended for" statements name every section edited in place.
- **Impact:** an auditor reading :340 or the header does not learn that §13 changed. Step 2.2b and the member rewrite steps get no pointer to two active documents that carry the old numbers.
- **Correction:**
  - Name §4.2 and §13 (clerical renumbering) next to §3 in :340 and in the session design header :9.
  - Add `GWZRequirements.md:116` and `GWZDesign.md:108` (the code range) to the renumbering row :356, with the steps that rewrite them.
- **Closure test:** a grep of active docs for "77 to 83", "(75)" in a session-code context and "(76)" in a session-code context finds only text that is recorded or corrected. The header and :340 list every session design section edited in place.

### N-P3-2: §8 records the `send` change but not the matching receive-side API change

- **Location.** `GwzSplitCommandChannel.md:361` records crate map §2 :40 (`FrameSink::send(frame, lane)` becomes `send(frame, lane, key)`). It does not record:
  - crate map :41 (`session-channel`'s `byte_stream(read, write)`, which §3.3 :102 replaces with `byte_stream(read, write, router)`);
  - whether a byte-stream end still implements `FrameSource::recv()`, given that every received frame now goes to the router.
- **Invariant:** the record of amended public-API clauses is complete.
- **Impact:** the shared conformance suite drives every adapter through `ChannelFixture::pair()` and `recv()` (`suite.rs`, 17 `recv` calls; `session-channel/src/tests.rs:54` runs it on `Pipes`). Step 2.1 must restructure the suite's byte-stream fixture, or keep `recv` beside the router, without a recorded decision. §8 :358 covers only `suite.rs:72–75`.
- **Correction:**
  - Add crate map :41 to the :361 row.
  - State in §3.3 "Reader interface" whether byte-stream ends implement `FrameSource`. If they do not, say how the conformance suite drives them (for example, a test router that queues frames).
- **Closure test:** from §3.3 and §8 a reader can say which `session-contract` traits a byte-stream end implements, and how `run_all` exercises it.

### N-P3-3: The text leaves open a support status that plan 6.3 has already decided

- **Location.** `GwzSplitCommandChannel.md:215`: `ssh://` "is the evidence form in this release (plan §11, 6.3); its support status is a Phase 6 decision (6.4)". Plan :719–723 (step 6.3) says: "Evidence, not a supported feature in this release." Step 6.4 :724–730 covers help and notes, not a support decision.
- **Invariant:** the object follows the plan's decided clause.
- **Impact:** 6.4 could document `--server ssh://` as supported, against an accepted plan clause.
- **Correction:** replace the sentence with "It is the evidence form in this release, not a supported feature (plan 6.3); 6.4's help says so."
- **Closure test:** §4.4 and plan 6.3 agree word for word on the release status.

## 2. Invariant analysis (attacks that failed, with evidence)

- **The adapters still decode nothing.** `FrameKey` holds no GWZ or Taut type and `target` is compared for equality only (:99). `FrameRouter::route` takes the checked `(tag, body)` and gwz-core decodes it (:102). Session design :112 says the same in place. The rule at crate map :22 holds.
- **Per-target order and the call-ID rule still hold.**
  - A `SessionCall` is never overtaken.
  - The close report is keyed ordinary, so it never passes the replies queued before it.
  - Control frames keep oldest-first order among themselves.
  - Tag 17 frames never pass data of their own `(request_id, session_id, stream_id)`.
- **Tag split matches its stated source.** It is the exact complement of `mux/mod.rs:638–641`. All 18 `MessageKind` values are assigned once, 15 on tag 17 and 3 on tag 16.
- **Fault scope is consistent everywhere.**
  - §3.3 :93 and :95 say a wrong-tag or invalid lane frame faults its binding.
  - :104 says a transport-class overrun faults its binding.
  - The new session design paragraph says a tag 16–17 frame breaking its live binding's rules is not a session protocol error.
  - Frames before `SessionOpened` (:94) and tags 18–31 remain session protocol errors.
  - D-N's verbatim reader phrase "protocol error of that class" reads consistently as binding-scoped for the transport class.
- **Code numbering:**
  - 79–85 keep the server design's order.
  - 81 is appended but never emitted.
  - 86 applies only to an admission-produced `open_failed`, with the other refusal paths assigned their existing codes.
  - This agrees across §1 :30, §5.6 :325, §8 :356 and session design :191 and :546.
- **Address precedence** for `stop` and `status` (§4.1 :189) does not conflict with `start` ignoring `GWZ_SERVER`, or with `hook` and `init --update` never reading it.
- **D-O:** every gwz-py mention (:37, :191, :249, :352, :355, :362, :363) says client only, with `SocketCoreBridge` as its only route. No Python server or Python command-line route is scheduled.
- **Citations checked in this round:**
  - `gwz.taut.py:840`, `:847`, `:1150–1151`;
  - `session.rs:105–108`;
  - `opening.rs:394–396`;
  - `mux/mod.rs:627–650`, `:638–641`;
  - plan :109 at `7ce0b749`;
  - `GWZDesign.md:104` and `:106`.

## 3. Risks and next action

- **Risks:**
  - (O6) If 2.1 takes its control classification from `sequenced.rs` instead of `mux`, it will put `open` and `check_identity` on the wrong tag.
  - The push-model reader (`FrameRouter`) has to run on a thread or pump that the adapter owns. 2.1's design should name who drives it (this ties to N-P3-2).
  - The `-o SetEnv=GWZ_NONE=1` precedence relies on OpenSSH treating `SetEnv` as first-match. That is the Safety axis's claim to check; I did not verify it here.
- **Next action:** accept revision 2 on this axis (GO). Apply N-P3-1 to N-P3-3 at the next clerical touch, or in 2.1, 2.2b and 6.4 as their owning steps. None of them blocks.
