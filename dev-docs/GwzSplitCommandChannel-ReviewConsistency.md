# GwzSplitCommandChannel.md revision 1: Consistency review (with Surface section)

**Review object:** `dev-docs/GwzSplitCommandChannel.md` revision 1 (305 lines), together with the in-place amendment of `dev-docs/GwzCoreSessionDesign.md` §3 made in the same commit. Both are at lane root `f923969d67` in `/Volumes/projects/limbo/gwz-dev-p0-design`. Revision 0 is `ffebf16a`.

**Baseline:** lane root `f923969d67`; members gwz-core `0ed481688d`, gwz-transport `a27817b9c8`, gwz-cli `cea7bd4cab`, gwz-py `71d2d07249`. The controlling plan is the root workspace's `dev-docs/GwzTransportReleaseConvergencePlan.md` at root `53d889a0`: revision 5, accepted, plus the clerical record of D-N, D-O and the split of step 1.3.

**Date:** 2026-10-25

**Axis:** Consistency, with the required Surface section.

**Verdict: NO-GO.** Findings: 0 P0, 0 P1, 2 P2, 11 P3. Each blocking finding has a bounded fix in the text. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified below. The P3 findings do not block.

## 0. Evidence base

- **Tuple.** Verified at the start and again at the end with `git rev-parse HEAD` on all five lane repositories and on the root workspace (`53d889a0…`). Nothing moved between the two checks. The working copies of both object files match HEAD (`git diff --quiet HEAD` passed).
- **Object and diff.** Read with `git show f923969d67`, which touches `dev-docs/GwzSplitCommandChannel.md`, `dev-docs/GwzCoreSessionDesign.md` and gwz's own commit marker `gwz.conf/markers/01a16ea1-….yaml`. I compared it with revision 0 via `git show ffebf16a:dev-docs/GwzSplitCommandChannel.md`.
- **Controlling documents read:**
  - the root plan: header and clerical record :1–34, §1/§1.1 :35–110, Phase 2 steps 2.0–2.6 :455–626, Phase 3 :628–654, §10 :807–865, §11;
  - the parked server design (lane copy) §3 :74–222, §4 :223–330, §6 :526–628, §7 :629–687, §9 :762–778, §16 :1127–1258 (selected lines), §17 error table :1416–1455;
  - the session design (lane copy), header :1–12, §3 :91–126, §5.3, §8 and the changelog;
  - the crate map §1–§3;
  - the contract at :155, :162, :223, :239–249, :467 and :757;
  - placement design :12, :17–39, :63–65, :96–98, :132–135 and :178–186;
  - `gwz-core/docs/TransportPlacement.md` :3–9, :13–14, :25, :83–84, :139 and :150;
  - `gwz-core/dev-docs/GWZDesign.md` :96–110 and `GWZRequirements.md` :90 and :108;
  - the history/reference release plan (:52, :60–64, :565) and amendment 1 (:301, :336–337).
- **Code read as reference:**
  - `session-contract`: `frame.rs` :1–40, :85–165; `channel.rs` :1–40; `lib.rs` :1–30; `README.md` :20–40; `contract_tests/suite.rs` :66–100;
  - `session-channel`: `byte_stream.rs` :1–75, :128–140; `README.md` :45–46; `src/tests.rs`;
  - gwz-transport: `protocol/transport.taut.py` :1–20, :98–120; `src/sequenced.rs` :155–215; `src/mux/mod.rs` :658–668;
  - gwz-core: `protocol/gwz.taut.py` (`GwzErrorCode` ends at `transport_record_limit=74`); `src/transport_host/cli_placement.rs` :54–71; `src/git/url_scheme.rs` :54–67, :366–372.
- **Installed CLI.** `gwz 1.0.17`. Ran `gwz --help`, `gwz local --help`, `gwz repo --help`, `gwz hook --help`, `gwz auth --help` and `gwz help forall`.
- **Environment.** On this macOS host, `printenv SSH_AUTH_SOCK` and `launchctl getenv SSH_AUTH_SOCK` both return launchd's per-login agent socket (`/…/com.apple.launchd.<id>/Listeners`). That is the platform default: launchd exports the variable to every login session.
- **Restatement sweep.** I grepped active (non-`history/`) `dev-docs/`, `gwz-core/dev-docs/`, `gwz-core/docs/`, `gwz-core/crates/` and `gwz-py/` for the FIFO, "only ever sends", "16–31 reserved", "ordered in each direction" and gwz-py server-family claims.
- Nothing was written, built or mutated.

## Problem, requirement provenance, and scope

- **Requested outcome.** Plan step 2.0 asks for one document freezing (a) what crosses the carrier, (b) the surface and (c) the trust model. It also carries 2.4's design amendment and the §8 record. Revision 1 applies the operator's D-N ("A") and D-O ("replace"), and amends session design §3 in place.
- **Observed problem it solves.** Revision 0 stopped on two points:
  - S1: a byte stream is FIFO, so control traffic waits behind spool replies.
  - S2: gwz-py's `server` family was adopted by D-I but no step built it.
- **Component authorization.**
  - D-N text (plan :852–859) authorizes "amending `GwzCoreSessionDesign.md` §3 in place". The commit amends §3 only: the tag table at :101–103, the byte-stream bullet at :118–121, the volume bullets at :123–125, plus a header bullet at :9 and a changelog entry at :697.
  - No historical or parked file is edited, and no member repository is touched. The gwz marker is commit-tool output, not an edit to any document.
  - So the commit stays inside "Editable: root `dev-docs/` only".
- **Constraint provenance.**
  - Traced to the plan or an operator decision:
    - tags 16/17 (a 2.0(a) choice the plan left open);
    - the defined macOS directory (2.0(b));
    - CLI-equivalent locking (2.0(b), 2.2b);
    - no `auto` and no auto-start (§1.1, D-I);
    - the priority writer and reader (D-N);
    - gwz-py as a client only (D-O);
    - the closed service-class table, the push allowance, the explicit native-auth flag, the scrub list and the `ssh` vector (2.0(b)/(c), §1.1 :102);
    - the test-only launch hook (2.0(b)).
  - The object's own choices, each stated with a reason in §1: the `ws-<key>` naming, code 84, the name `--allow-native-auth`, and the prompt's limits.
  - **One constraint is the object's own extension beyond the plan:** "`gwz server start` refuses the same way" (§3.8 :136). The plan requires only `gwz server stdio` to refuse (§1.1 :102, 2.0(b), 2.3). It is defensible as a necessity from the plan's §1 threat model, but it contradicts the object's own walkthrough (P2-1).
  - No unauthorized scope expansion was found.
- **Out-of-scope discoveries.** These are follow-up observations, not findings:
  - (O1) The session design's tag table (:97–103) never listed the server design's tags 4–8. They exist only through the header's "as amended" bullet and in `frame.rs:91–95`. This predates the object.
  - (O2) Plan §1.1 :109 attributes "writes every frame in call order" to the `session-channel` README. That text is in `byte_stream.rs:65` and `:130`; the README (:45–46) says "It does not use the lane".
  - (O3) Plan §1 :69–70 says the served core's `admit → acquire → register → open` "travel over the carrier". Step 2.2c :578 and the object (§3.4 :71) keep `admit` and `acquire` core-local and send only `register` (as `bind`) and `open`. The object follows 2.2c, the more specific text.
  - (O4) `url_scheme.rs:372`, cited by object §5.2 :243 for the known-host list, is a `#[cfg(test)]` constant. The production list is the enum at :54–67.

## Surface section (read cold first): placement, names, lifecycle pairs, defaults, first-day walkthrough, two-machine walkthrough

I read the object's §4 cold before any design or plan text, then compared it with the installed 1.0.17 help.

- **Placement.**
  - The installed root help groups commands as Inspect / Change / Workspace / Members / Lanes / Other (`auth forall`). Adding `server` to "Other" (§4.1 :142) is where a user scanning for an infrastructure command would look. It is a family (`start|stop|status|list|stdio`) like `local`, `repo`, `hook` and `auth`, and bare `gwz server` printing its subcommands matches how `gwz local` behaves.
  - `--server ADDRESS` goes under "Common options". The text calls it "global, like every selector", but the installed help has a separate "Selection" group. The wording is loose; this is not a defect.
- **Names.**
  - `start`/`stop`/`status`/`list` read correctly cold.
  - `stdio` is meaningful only as the child half of `--server stdio`/`ssh://`. Nothing in §4 says a user should not run it by hand. That is acceptable for a plumbing verb, but the help text (6.4) should say so.
  - `--allow-destination` / `--allow-push` / `--allow-native-auth` form a consistent family.
  - `server_destination_refused` sits next to `server_address_refused` and is unambiguous.
- **Lifecycle pairs.**
  - `start` pairs with `stop`, and the printed address is consumed by `--server` and `GWZ_SERVER`.
  - `status` probes one server; `list` enumerates all of them.
  - **Created and never removed.** `ws-<key>.lock` persists with its record cleared (server design §6 shutdown step 5: "clears its lock record, releases its lock"). `ws-<key>.log` and its rotated `.log.1` persist. So does the per-user directory. No command removes them, and §4 never says they stay. The leftovers are bounded per workspace and the lock file is intentionally persistent, so this is not filed. 6.4's help should still state it, or a user doing "confirm nothing is left behind" has to guess.
- **Defaults.**
  - Stated: `--max-sessions` 64, `--idle-exit` off, `--log` `ws-<key>.log`, allowed set empty under `--server` (§5.6 :268), `GWZ_SERVER` is the default for `--server`, `--no-server` overrides it, an empty `GWZ_SERVER` means unset.
  - `stop --force` is inherited from server design §6 (off).
  - **Not stated:** what `server stop` and `server status` do with neither `--server` nor `GWZ_SERVER` (P3-9).
- **First-day walkthrough (§4.9 :208–225), from help alone.**
  1. `gwz --help`, then `gwz help server start`: plausible.
  2. `cd ~/ws && gwz server start` on the macOS shown (`/Users/alex`, `/var/folders/…`) **fails**. launchd exports `SSH_AUTH_SOCK` to every login session, and §3.8 :136 makes `server start` refuse with a usage error. The help described at :212 does not mention it (P2-1).
  3. `export GWZ_SERVER=…` then `gwz fetch`: in an interactive shell, §5.6 :270 prompts. The walkthrough instead shows a refusal and a return to the shell (P3-10).
  4. From a second shell, `gwz server list` gives the address and `gwz --server ADDR server status` gives status. Bare `gwz server status` inside the workspace is undefined (P3-9).
  5. `gwz server stop`: works only because `GWZ_SERVER` is still exported. After the stop, the walkthrough never runs `unset GWZ_SERVER`, so every later command in that shell fails with `server_unavailable` instead of running in-process. A note in 6.4 would cover this; it is not filed.
  6. "Confirm nothing is left behind": nothing in the text to confirm against (see Lifecycle pairs).
- **Two-machine walkthrough.**
  - The steps are clear enough: `gwz --server ssh://me@host/abs/ws --allow-destination … fetch`, with `gwz` on the remote `PATH` and no remote `server start` needed.
  - `forall` is refused; `server status` and `server stop` refuse the form (server design §6, inherited).
  - The remote `gwz server stdio` refuses if the remote account's shell start-up exports `SSH_AUTH_SOCK` (a common `.bashrc` idiom). That is deliberate but not mentioned in §4.4; see Risks.
  - §4 has no worked example of this form. 6.4 should add one.

## 1. Findings

### P2-1: The frozen first-day sequence cannot run on its own platform: `server start` refuses the agent variable that launchd sets by default

- **Location.** `GwzSplitCommandChannel.md:136` ("`gwz server start` refuses the same way, so no served core starts holding one"), against :213 (`cd ~/ws && gwz server start` on macOS, with output at :214–217) and :227 ("2.6's Surface review reads exactly this sequence cold").
- **Violated invariant.** A frozen walkthrough must be executable under the clauses it is frozen with. §3.8 and §4.9 cannot both hold in a default macOS login session, and the same is true on common Linux desktops whose keyring exports `SSH_AUTH_SOCK`, and wherever `GH_TOKEN` is exported. The background start inherits "the starting CLI's" environment (server design §6 "Starting in the background"), and the object never says the start scrubs it.
- **Reproduction.**
  1. Open a new Terminal on macOS: `SSH_AUTH_SOCK` is set to launchd's `…/Listeners` socket (observed on this host).
  2. Run `gwz server start` in a workspace. Under :136 this is a usage error naming `SSH_AUTH_SOCK`.
  3. The sequence at :213–224 stops at line 213.
- **Impact.** 2.6's Surface review reads "exactly this sequence" and must fail on the platform it shows. The R7 "first proof" (plan §1 :52) is blocked on the same start path. The object also leaves the resolution (scrub or refuse) open, and that choice changes the start's behaviour toward credentials.
- **Required correction.** Choose one and state it in §3.8 and §4.3:
  - (a) `server start` launches its background copy with the §3.8 list and `SSH_AUTH_SOCK` removed. Only `--foreground` (and `stdio`) refuse an inherited variable. Show that in §4.9.
  - (b) Keep the refusal, show the required `env -u …` step in §4.9 and in the `gwz help server start` description at :212, and name the refusal text.
- **Closure test.** Read the revised §3.8, §4.3 and §4.9 together with an environment containing `SSH_AUTH_SOCK` and `GH_TOKEN`. Every line of §4.9 either succeeds or shows the exact step that makes it succeed.

### P2-2: The D-N priority writer and per-class reader need each frame's target and class, which the adapters are forbidden to decode and the frozen `FrameSink` API does not carry

- **Location.**
  - The rule itself: `GwzSplitCommandChannel.md:64–65`, and as amended into `GwzCoreSessionDesign.md:119–120` ("except ordinary frames of its own target (same `call_id`, or same `(request_id, session_id, stream_id)`)"; "routes every frame to a per-class queue … the binding's transport `Limits`").
  - What it contradicts, all unamended and absent from §8 (:282–305):
    - the same section's Adapters bullet, `GwzCoreSessionDesign.md:112` ("carry bytes and never decode a body");
    - `GwzCoreSessionCrateMap.md:22` ("all encoding and decoding of GWZ's Taut-defined messages stays in gwz-core") and :40 (`FrameSink::send(frame, lane)`);
    - `session-contract` `lib.rs:13` and `:22–25` ("decodes no CBOR … classifying each frame's lane are core's").
  - The plan's §1.1 :104 names "crate map frame-tag rules" among the clauses this programme changes, yet the object records nothing about the crate map.
- **Violated invariant.** An amended clause must be implementable without silently breaking another clause of the same active section. The record of amended clauses must cover every clause the amendment changes.
- **State sequence.**
  1. The core writer queues `data(stream 3)` on tag 16, then `window(stream 3)` on tag 17.
  2. To honour "never overtaken by a control frame of its own target", the byte-stream writer (`session-channel`, which §8 :297 assigns to 2.1) must know that both frames name `(request_id, session_id, 3)`.
  3. Those fields are inside the deterministic-CBOR `TransportLaneFrame` body. `send(frame, lane)` passes only `lane`.
  4. So the writer must either decode the body, breaking :112, the crate map :22 and `lib.rs`, or receive a key through an API the design does not define.
  5. The reader has the same problem. Routing tags 1–3 into the control reserve on receipt requires the method, and routing lane frames per binding requires `session_id`; both are body fields.
  6. For tags 1–3 the target exception happens to be vacuous: replies take their call's lane, and a `SessionCall` is never overtaken. The problem is real for tags 16–17 and for the reader.
- **Impact.** 2.1 cannot satisfy both normative bullets of session design §3. Whatever it picks is an unreviewed public-API change to `session-contract`, whose API the crate map keeps under per-step dual review, or a violation of the no-decode rule. Neither appears in the freeze, so the 2.1 dual review starts with an undecided interface.
- **Required correction.**
  - State where class and target come from. One shape: core supplies a frame class and target key with each send (for example `send(frame, lane, key)`, or a core-side pump above an unchanged FIFO adapter). On receive, the adapter routes by tag and a core-supplied classifier port assigns tags 1–3 and per-binding queues.
  - Say which crate owns the per-class queues.
  - Add §8 rows for `GwzCoreSessionDesign.md:112`, `GwzCoreSessionCrateMap.md:22` and `:40`, and `session-contract` `lib.rs:13` and `:22–25`, or explain why each still holds.
  - This is a design-level gap, but the fix is in the text, not the architecture.
- **Closure test.** Starting from the revised §3.3 and §8 alone, a reader can write the signature of the 2.1 writer and reader entry points and name the crate that decodes `request_id`/`session_id`/`stream_id`. No clause of session design §3 or crate map §1–§2 contradicts it.

### P3-1: The session design §3 ordering guarantee and the shared conformance test still promise one order on every adapter

- **Location.** `GwzCoreSessionDesign.md:93` ("reliable and ordered in each direction while open", unamended); `session-contract/src/contract_tests/suite.rs:72–75` (`frames_arrive_in_order_on_both_lanes`, run against `Pipes` by `session-channel/src/tests.rs:54`). Neither is in §8 :297, which lists only `channel.rs:18–19`, `README.md:30` and `lib.rs:19`.
- **Invariant.** The record of amended clauses must be complete. Under D-N, frames sent alternately on the call and control lanes, which is exactly what `suite.rs:85–90` sends, arrive reordered on a byte stream.
- **Impact.** 2.1 either breaks the shared LBT conformance suite or has to amend it against an unamended authority clause (:93).
- **Correction.** Qualify :93 as "ordered per target on byte streams; in send order in-process". Add `suite.rs:72–75` to the §8 D-N row as 2.1's code.
- **Closure.** A grep for "ordered in each direction" and "in order on both lanes" finds only qualified or recorded text.

### P3-2: `closed` (a terminal kind) is placed outside the control class, against plan 2.0(a), for a reason that does not hold under D-N

- **Location.** `GwzSplitCommandChannel.md:59` ("Orderly kinds (`closed`, `close`, `end_write`) stay on tag 16 so they never overtake data of their stream").
- **Requirement.** Plan :444–446: "transport control (Window, Cancel, terminal frames) stays in `session-channel`'s control reserve". 2.1 :543: "never starves Window, Cancel or terminal frames". gwz-transport treats `Closed` as terminal (`mux/mod.rs:658–667`) and charges every non-`Data` kind to the control reserve (`sequenced.rs:192`).
- **Inconsistency.** D-N's own exception (:64) already stops a tag-17 frame from overtaking ordinary frames of the same `(request_id, session_id, stream_id)`. So the stated reason gives no protection that tag 17 would not also give. Meanwhile a client-to-core `closed` for stream X waits behind other streams' queued `data`.
- **Correction.** Either move `closed` to tag 17, or keep it on tag 16 with a reason that holds under D-N and record it as a deviation from plan 2.0(a).
- **Closure.** §3.3 and the plan's 2.0(a) and 2.1 sentences agree on which terminal kinds are control.

### P3-3: A §8 row says it is "not in plan §1.1's line list", which the controlling plan contradicts

- **Location.** `GwzSplitCommandChannel.md:289` ("**not in plan §1.1's line list, implied by its 2.0(a) choice**").
- **Evidence.** The root plan at `53d889a0`, :108, carries exactly this row ("recorded 2026-10-25 from 2.0's §8"). The object's own §2 :26 says the decisions are recorded in plan §1.1.
- **Impact.** A later auditor is told that the record is missing when it is present.
- **Correction.** Replace the bold clause with "plan §1.1 :108".
- **Closure.** The row and the plan agree.

### P3-4: The session design header says "§3 only", but §8 lists session design §5.1, §5.2 and §5.6 as amended clauses

- **Location.** `GwzCoreSessionDesign.md:9` ("for §3 only"); `GwzSplitCommandChannel.md:284` ("The session design is active and is amended in place … by this step") and :301 (session design §5.1 workspace-registry wait, §5.2 transport entry and §5.6 endpoint environment: "Change here: §3.2").
- **Inconsistency.** Either the :301 changes are amendments, in which case the active session design neither shows them nor names them in its header, or they are served-core adoption differences that do not amend the session design, in which case :301 does not belong in "Clauses this document amends" and :284's "amended in place" overstates.
- **Correction.** Pick one. Either re-label :301 as "adopted with differences for the served core (session design unchanged)", or extend the header bullet at :9.
- **Closure.** The header and §8 name the same set of sections.

### P3-5: D-O leaves gwz-py's CLI client selectors adopted but neither frozen nor replaced

- **Location.**
  - `GwzSplitCommandChannel.md:29`, :147 and :300 replace only `gwz-py server start|stop|status|list|stdio`.
  - Server design :38 ("`--server`, `--no-server` and `GWZ_SERVER` in both CLIs") and §7 :631 ("Selection, in both CLIs"), adopted under D-I, still give the `gwz-py` console script `--server ADDRESS|stdio`, `--no-server` and `GWZ_SERVER`.
  - Its `--server stdio` local form starts "gwz-py's own interpreter on `gwz.cli`" running `server stdio` (server design §6), and D-O has just removed that family.
- **Inconsistency.** The adopted set contains a gwz-py CLI form that cannot be built once D-O applies, and the object neither freezes nor records it.
- **Impact.** 3.2 and 3.5's Surface review on gwz-py have no frozen answer to "does `gwz-py --server` exist".
- **Correction.** Add a §8 row stating the disposition of server design :38 and §7's gwz-py CLI selectors: replaced (gwz-py's only route to a served core is `SocketCoreBridge`), or kept for socket addresses only, with `stdio` refused.
- **Closure.** Every gwz-py surface item from server design :13–14, :38 and §7 has a frozen or replaced disposition.

### P3-6: The record omits, or mis-cites, active GWZDesign restatements that the object replaces

- **Location.** `GwzSplitCommandChannel.md:295` cites `GWZDesign.md:104,108` for "the client supplies the session's snapshot". Plan :101 repeats the same lines.
- **Evidence.**
  - `gwz-core/dev-docs/GWZDesign.md:106` holds the snapshot sentence ("the client captures its environment and supplies the session's snapshot"). :108 has none.
  - The same active paragraphs restate replaced clauses that no §8 row lists against this file:
    - :104, "gwz-cli and gwz-py each gain a `server` family" (D-O);
    - :106, "commands on one workspace queue through its workspace registry", the must-match set, `server_environment_mismatch`, and the `auto` key (§1 :16, §3.1 :36, §4.1 :144);
    - :108, "the remote session uses … the remote account's credentials, or an agent the user's SSH configuration forwards" (§4.4 :170).
  - `gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md:25` schedules "`SocketCoreBridge` and `gwz-py server`" for 1.2.0.
- **Impact.** The active GWZ design summary contradicts the freeze in four places, and nothing points a later editor at them.
- **Correction.** Correct the citation to :104 and :106. Add the restatements above to §8 as clauses the member's owning step rewrites, since member files are reference-only here.
- **Closure.** A grep of active member docs for "gwz-py server", "queue through its workspace registry" and "agent the user's SSH configuration forwards" is matched by §8 rows.

### P3-7: `server_destination_refused` is mapped from every `invalid_request` refusal, including ones that name no destination

- **Location.** `GwzSplitCommandChannel.md:269` ("The core maps an `open_failed` or `bind_rejected` with `invalid_request`, `Effect::None` and no facts under `cli` to `server_destination_refused`") against :72, where `bind_rejected(invalid_request)` also covers an unknown, finished or already-bound `request_id` and a class that admits no network, and :233.
- **Inconsistency.**
  - A `bind` carries no destination, so the refusal text at :269 ("the core asked to fetch from `<canonical tuple>`") cannot be produced for a bind refusal.
  - A protocol misuse is reported as "destination refused".
  - `identity_check_failed`, also refused with `invalid_request` per :233, has no mapping at all.
- **Impact.** Misleading `--json` codes and messages for core-side bugs and for refused identity checks. This is a diagnosability defect.
- **Correction.** Map only `open_failed` produced by admission to code 84. Give bind refusals and `identity_check_failed` their own stated code or text. Or give admission refusals a distinct `Failure` cause the core can test.
- **Closure.** Each refusal path at :72 and :233 has exactly one stated user-facing code and text.

### P3-8: Code 84 assumes 77–83 are all appended, but `server_environment_mismatch` (79) has lost its only producer

- **Location.** `GwzSplitCommandChannel.md:22` (84 "after `server_session_closed` (83)") and :36 ("no must-match value exists any more").
- **Evidence.** Server design :772 and :1432 define 79 only for must-match and native-path differences. The native route is refused under a served core (§4.6 :191). The schema currently stops at 74 (`gwz.taut.py:837`), so 75–84 are all still to be appended.
- **Impact.** 2.2b or 2.3 must decide, outside the freeze, whether 79 is appended as a never-emitted value or dropped, which renumbers 80–84.
- **Correction.** State 79's disposition. The numbering-stable option is "appended, never emitted in this release".
- **Closure.** §1's code row lists 75–84 with a producer or an explicit "unused".

### P3-9: `server stop` and `server status` have no stated address default after `auto` is removed, and neighbouring selector cases are open

- **Location.** `GwzSplitCommandChannel.md:145` ("`stop` and `status` take `--server` or `GWZ_SERVER`"). Server design §6 :538 had "else `auto`", which :144 removes.
- **Gaps.**
  - Bare `gwz server status` or `gwz server stop` run inside the workspace: refused, or the workspace's own `ws-<key>` server, which §1 :17 makes derivable?
  - `gwz --server X server list` and `server stdio`: usage error? Only `start` is stated.
  - `gwz init --update` with `GWZ_SERVER` set: :193 says it "refuse[s] `--server`", but not whether it reads the variable. The text says the `hook` family does not (:145).
- **Impact.** The first-day "see its status from a second shell" step forces a guess, and the three cases produce different exit codes. Changing a refusal into a default later is not a compatibility break, hence P3.
- **Correction.** State the default for `stop` and `status`, the treatment of `--server` given to `list` and `stdio`, and `init --update`'s handling of `GWZ_SERVER`.
- **Closure.** Every `server` subcommand has a stated address source and a stated result when none is given.

### P3-10: The walkthrough contradicts the terminal-prompt rule

- **Location.** `GwzSplitCommandChannel.md:219–221` (an interactive `$ gwz fetch` prints a refusal and returns) against :270 (a prompt is shown "only when the client has a controlling terminal (`/dev/tty` opens …)": "`Allow this repository for this command only? [y/N]`").
- **Impact.** 2.6 reads :219–221 as the expected output and would either fail the prompt or encode the wrong behaviour.
- **Correction.** Show the prompt (and an answer) at :219, or run that line without a terminal (`</dev/null`, or in CI) and say so.
- **Closure.** §4.9's output is what §5.6 produces in the walkthrough's own conditions.

### P3-11: §1 says sessions share the host context "only for the member lock manager"; §3.2 shares the detached-worker record too

- **Location.** `GwzSplitCommandChannel.md:16` ("share the server's one host context only for the member lock manager") against :50 (the workspace registry is "kept only to record workers detached at a close bound, so a later W or push on their workspace is refused") and :53 ("host context = member lock manager and detached-worker record only").
- **Impact.** An implementer of 2.2b who follows §1 does not share the detached-worker record across sessions. A second session's write could then run beside a worker detached by the first, which is exactly what :50 forbids.
- **Correction.** Make §1 :16 name both shared members.
- **Closure.** §1 and §3.2 list the same host-context contents.

## 2. Invariant analysis (attacks that failed, with evidence)

- **D-N wording matches across three texts.** The object's §3.3 :64–65, the session design :119–120 and plan D-N :852–857 agree word for word on the writer: "tag 17, and tags 1–3 on the control lane", the same exceptions, and "per-target order and the increasing call-ID rule survive". The reader text differs only by "and" versus a comma. §3.1 :40's cancel/close bullet restates the same exceptions. The scope (socket, named pipe, stdio, `ssh`; in-process unchanged) is the same at :66 and :121.
- **Tag assignments agree everywhere** (16 data, 17 control, 18–31 reserved): §1 :14, §3.1 :35, §3.3 :59 and :66, §8 :296 and :299, session design :101–103 and :123, plan :108–109.
- **The tag split covers every message kind exactly once.** §3.3 :59 assigns all 18 `MessageKind` values (`transport.taut.py:9–13`), 8 on tag 17 and 10 on tag 16, with no kind missing or doubled.
- **The call-ID rule survives D-N.** No `SessionCall` is ever overtaken, cancel and close are themselves `SessionCall`s, and control frames among themselves go oldest first. So the increasing call-ID rule (session design :108) cannot be violated.
- **"Never queues behind an in-band spool" holds literally.** Spool replies flow core to client. Every core-to-client terminal or flow-control kind (`window`, `cancel`, `failed`) is on tag 17. P3-2 concerns only the control-reserve wording.
- **D-L shape matches plan :103 and §10 :836–845.** `remote::handle(binding)` takes the binding as a constructor argument, not as a fifth port. `admit` carries only an operation reference. `register` is a synchronous, infallible deferred commit. `NeverRegistered(CarrierLost)` has zero effects. `Operation`/`Exchange` keep no public constructor. `snapshot()` is refused. §12's served walkthrough is present (:88–102). The contract line cites are accurate: :155, :162, :223, :467, :757. The extra outcome `NeverRegistered(ClientRefused)` is recorded at :302.
- **D-O mentions are consistent.** All four (:29, :147, :195, :300) agree with plan :860–863 and step 3.2 :640–645 (gwz-py owns an in-process host and reaches a served core through `SocketCoreBridge` as `bridge=`). Nothing schedules a Python-hosted server. §4.7 :198 refuses `stdio`, `ssh://` and `auto`.
- **Code 84 is free.** `GwzErrorCode` ends at 74; session design :183 adds 75–76; server design :769–776 adds 77–83.
- **Error codes reused by the object match their sources:** `server_address_refused` (81), `server_unavailable` (77), `server_session_closed` (83) and `transport_session_full` (75).
- **Session limits match.** The limits in §3.2 :47 (8, 64, 128, 1024, 64) match session design :47–52, :226, :231 and :266.
- **Placement design.** The §6 quote matches :96–98 verbatim, and :12, :17–39, :63–65, :132–135 and the §4 attachment-slot sentence (:181–183) say what §8 claims. §3.1 :38's empty meta slots are recorded in plan §1.1 :108 and §8 :289 (but see P3-3).
- **Server design.** The citations check out: :13–14, :35–38, :77–88, :193–202 (the stdio/marker row requires `umask` and `openssl` absent, as §3.1 :37 does), :228, :799, :1170 and :1172–1178. Shutdown steps, the 185-second stop wait, `--force`, probe-first and the `list` rules are inherited without contradiction. Adapting the "Step 4 has nothing to dispose" note (:163) is consistent.
- **Session-crate citations are verbatim at the cited lines:** `frame.rs:14`, `:24–25`, `:96` and `:154`; `lib.rs:14` and `:19`; `channel.rs:18–19`; `README.md:30`; `byte_stream.rs:65` and `:130`; `session-channel README.md:45–46`; `cli_placement.rs:68`. The §5.2 host list is correct in substance (O4).
- **Commit scope.** Only root `dev-docs/` files (plus the gwz marker) changed. The parked server design and the placement design are untouched, as §8 :284 says. The session design changelog entry (:697) matches the diff.
- **Plan rows versus the object's §1.1-derived rows.** Plan :100–104 and :108–110 each have a matching row in §8 (:289, :292–302).

## 3. Risks and next action

- **Risk (observation for 2.1).** Under D-N, `operation.cancel`'s reply (control) may now reach the client before its target's own final reply (ordinary) when spool replies sit ahead of that reply. Session design :258 has the host send the cancel reply after the worker's terminal, which FIFO used to deliver in order. The bridge text (§10 :414, :499) waits on `call_id` and tolerates either order, but 2.1's bridge rows should test it.
- **Risk (two-machine form).** A remote account whose shell start-up exports `SSH_AUTH_SOCK` makes the remote `gwz server stdio` refuse every `ssh://` session (:136). The refusal is intended, but 6.4's help and 2.3's message should name the remote start-up file as the likely cause.
- **Risk (support status).** §4.4 freezes `--server ssh://` as user surface. Plan §1 :42–43 lists "two machines as a supported feature" as a later form, and §11 :869–870 adopts the stdio form "for evidence". 6.4 should say whether `ssh://` is supported or evidence-only in this release.
- **Next action.**
  - Revise the text for P2-1 (choose scrub or refuse and make §4.9 executable) and P2-2 (state the class and target interface, and record session design :112, crate map :22 and :40, and `session-contract` `lib.rs:13` and `:22–25`). Re-review on this axis. I pre-commit to GO if both are resolved as specified.
  - Apply P3-1 to P3-11 in the same revision where convenient. None of them blocks.
