# GwzSplitCommandChannel.md revision 1: Safety-axis review

**Review object:** `dev-docs/GwzSplitCommandChannel.md` revision 1 (305 lines, SHA-256 prefix `5817dd28…`) at lane root `f923969d67`, together with the in-place amendment of `dev-docs/GwzCoreSessionDesign.md` §3 in the same commit. The prior revision is lane commit `ffebf16a`.
**Baseline:** lane workspace `/Volumes/projects/limbo/gwz-dev-p0-design`: root `f923969d67`, gwz-core `0ed481688d`, gwz-transport `a27817b9c8`, gwz-cli `cea7bd4cab`, gwz-py `71d2d07249`. Controlling plan: the root workspace's `dev-docs/GwzTransportReleaseConvergencePlan.md` at root `53d889a0` (revision 5 plus the clerical record of D-N and D-O).
**Date:** 2026-10-25 **Axis:** Safety (what the text permits to go wrong)
**Verdict: NO-GO.** The review found 0 P0, 0 P1, 4 P2 and 7 P3 findings. Every blocking finding has a bounded fix in the text. I pre-commit to GO on a revision that resolves P2-1, P2-2, P2-3 and P2-4 as specified below.

## 0. Evidence base

- **Tuple.** I checked it at the start and again at the end with `git rev-parse HEAD` in each repository, and `git -C /Volumes/projects/limbo/gwz-dev rev-parse HEAD` gave `53d889a099a0…`. Nothing moved.
- **The object.**
  - I read `git show f923969d67:dev-docs/GwzSplitCommandChannel.md` in full.
  - I read `git show f923969d67`. It changes only `dev-docs/GwzSplitCommandChannel.md`, `dev-docs/GwzCoreSessionDesign.md` (header line 9, §3 lines 100–126, one changelog entry) and the tool-generated `gwz.conf/markers/01a16ea1-….yaml`.
  - I read `git diff ffebf16a f923969d67` for the object.
- **Controlling text.**
  - The plan: §1 (goal and threat model), §1.1, §3 rows R5 and R7, §4 Phase 2 steps 2.0–2.6, and §10 D-A to D-O.
  - The parked server design, `history/parked/GwzCoreServerDesign.md`: §3 (lines 148–222), §4 (lines 223–330), and the §6/§17 force rules (lines 533, 589, 1398).
  - The server design's §16 (lines 1150–1185).
  - The session design at `f923969d67`: §3, §8 (lines 416–433) and §10 (lines 456–506).
  - The Connection API contract: §6 (lines 285–341) and §7 (lines 461–506).
- **Reference code, read only to test the text's claims.**
  - gwz-transport `src/mux/asynchronous.rs:155–158`: `Port::deliver` waits on `WouldBlock`, which is backpressure.
  - gwz-transport `src/mux/mod.rs:89–97` and `:627–650`: the inbound and outbound queue bounds. Every kind except `Data`, `Open` and `CheckIdentity` is charged as control (`:638`).
  - gwz-transport `src/mux/routing.rs:4–16`: a `Protocol` result disconnects the binding.
  - gwz-transport `src/mux/routing.rs:87–91`: a frame for an unknown stream above `highest_id` is a `Protocol` error.
  - gwz-transport `src/stream/outgoing.rs:69–72`: a data frame is sent when the batch deadline expires, so small frames are possible.
  - gwz-transport `src/binding.rs:4–18`: the defaults are `queued_frames` 64, `control_reserve_frames` 8 and `receive_window` 1 MiB.
  - gwz-core `crates/session-contract/src/frame.rs`: the body is opaque and no adapter decodes it.
  - gwz-core `crates/session-contract/src/channel.rs:18–19`: in-process lanes "share one order".
  - gwz-core `crates/session-channel/src/byte_stream.rs:129–131`: `send(frame, lane)` carries no target.
  - gwz-core `protocol/gwz.taut.py:1151` (`RequestMeta.dry_run`) and `:1993` (`PushRequest`).
  - gwz-cli `src/tests/g02/push_check_remotes.rs:90–94`: `gwz --dry-run push` builds a `PushRequest` with `meta.dry_run = Some(true)`.
  - gwz-core `src/git/endpoint/ssh_destination_tests.rs:241–245`: the client endpoint quotes the exec command with sq-quoting.
- **Local read-only probes on macOS, OpenSSH_10.3p1.**
  - Command: `ssh -G -F /dev/stdin -a -x -T -o ForwardAgent=no -o ClearAllForwardings=yes x` against a config holding `ForwardAgent yes`, `SendEnv GH_TOKEN AWS_*`, `SetEnv FOO=bar`, `LocalForward` and `ControlMaster auto`.
    - Result: `forwardagent no`, `forwardx11 no`, `clearallforwardings yes`; `sendenv GH_TOKEN`, `sendenv AWS_*` and `setenv FOO=bar` survive, and so does `controlmaster auto`.
  - `-o 'SendEnv=-*'` on the command line does not clear SendEnv patterns that come from the config.
  - `-o SetEnv=GWZ_NONE=1` on the command line does replace the config's `SetEnv`, because the first value obtained wins.
  - `launchctl print gui/<uid>/com.openssh.ssh-agent`: the system LaunchAgent `/System/Library/LaunchAgents/com.openssh.ssh-agent.plist` is running and exports `SSH_AUTH_SOCK => /var/run/com.apple.launchd.<random>/Listeners` to every login session.
  - `/System/Library/LaunchDaemons/com.apple.bsd.dirhelper.plist`: `RunAtLoad`, daily at 03:35, `CLEAN_FILES_OLDER_THAN_DAYS=3`.

## Problem, requirement provenance, and scope

- **Requested outcome.** Plan step 2.0 asks for one root dev-docs document that fixes three things:
  - (a) what crosses the carrier, including the D-L served-core composition and the lane for core-to-client envelopes;
  - (b) the surface;
  - (c) the D-H (b) trust model.

  It also asks the document to carry 2.4's HTTPS amendment and its §8 amendment record. D-N authorizes amending session design §3 in place.
- **Observed problem it solves.** A core with no credentials must reach Git hosts only through a client that admits each request against the client's own invocation, over a same-user local carrier (D-D) or `ssh … gwz server stdio`.
- **Component authorization.**
  - Only root `dev-docs/` changes.
  - The session design amendment is limited to §3, its header entry and its changelog, which D-N authorizes.
  - No parked or historical file is edited. The parked server design and the placement design appear only in §8's record.
  - The marker YAML is gwz's own commit record, not an authored change.
- **Provenance of the scope-changing constraints.**
  - The transport lane on tags 16 and 17 traces to the plan's 2.0(a) choice.
  - The priority writer and non-blocking reader trace to D-N; their wording is verbatim.
  - gwz-py as a client only traces to D-O.
  - "No `auto`, no auto-start" traces to the plan's 2.0(b) and §1.1.
  - The macOS directory traces to "a defined macOS location" in 2.0(b).
  - One workspace per server traces to 2.2b.
  - The 60-second stall close traces to 2.3's red row, "a dead client without EOF does not wedge the server".
  - `--allow-native-auth` traces to 2.0(c)'s "explicit flag".
  - Error code 84 traces to the "named refusal" in 2.0(c).
  - Refusing a port that does not match the allowance traces to D-H (b)'s "(scheme, host, port, path) tuples". The plan's 2.0(c) text says "the port is not refused" (plan line 524), which contradicts its own threat model. The object follows D-H (b), the stricter reading. This is recorded as a plan-text observation, not a finding.
  - The limits on what the prompt may add narrow the plan's text; they do not widen it.
- **Untraced constraint.** `gwz server start` refusing an inherited `SSH_AUTH_SOCK` or listed variable (object L136) is not in the plan. The plan places that refusal on `gwz server stdio` only (2.0(c), 2.3). It is a preference written as a requirement, and it blocks the documented first-day flow; see P2-4.
- **Out-of-scope observations** (not findings against this object):
  - the plan's "the port is not refused" wording (plan line 524);
  - the residual exposure when a user forwards an agent to the core machine by other means (§2, item 9).

## 1. Findings

### P2-1: A `--dry-run` push or tag push is push-class, so the client admits a real receive-pack exchange for an invocation that authorized no remote change

- **Location.** `GwzSplitCommandChannel.md:180` (the §4.5 push row), `:175` ("its class fixes what admission can grant") and `:248–249` (§5.3).
- **Violated invariant.** Threat model (plan §1) and D-H (b): admission uses "the service class of its own command". A client must not spend a push allowance its own invocation did not authorize.
- **Reproduction.**
  1. `gwz.server.allow-push <addr> https://github.com/OWNER/` is set in the client's global Git configuration. §5.6 makes such grants persist per address, and on Linux and macOS the address survives restarts (L17).
  2. The user runs `gwz --server <addr> --dry-run push --check-remotes`. gwz-cli builds a `PushRequest` with `meta.dry_run = Some(true)` (gwz-cli `push_check_remotes.rs:90–94`).
  3. The client classes the request as push from its type alone (L180).
  4. A buggy core, or a hostile one, which is in scope as the less-trusted side, binds the request and sends `Open(receive_pack_exchange)` for a push-allowed tuple.
  5. The client admits it (§5.3 conditions: push allowance and push-class request) and runs a real receive-pack with the client's credentials.
- **Impact.** A remote mutation the user explicitly asked not to make. The core chooses the refs, including force or delete. It cannot be undone from gwz.
- **Required correction.** The class is a function of the request type and of `meta.dry_run`.
  - A request with `dry_run = true` admits no `receive_pack_exchange`, ever.
  - Name what a dry-run push may admit: nothing (local), or `receive_pack_advertisement`/upload-pack for allowed tuples if the planned rows need remote state.
  - Apply the same rule to the `TagRequest` push and delete rows.
  - State that a request field which turns an operation into a plan without mutation always narrows the class.
- **Closure test.** A 2.3 red row: a `--dry-run push` with a push allowance in configuration; a core-sent `receive_pack_exchange` gets `open_failed(invalid_request)` with `Effect::None`, and the fixture remote is unchanged. Repeat for `--dry-run tag push` and `--dry-run tag delete`.

### P2-2: The byte-stream reader's transport-class bound can be overrun by a peer that obeys every rule it can observe, and what an overrun does is undefined

- **Location.**
  - `GwzSplitCommandChannel.md:62` (Bounds: `queued_frames`, `queued_bytes`, `receive_window`, `control_reserve_*`; "the sender sends `data` only within `Window` credit").
  - `:65` (the reader: "a per-class queue whose bound the peer must respect … an overrun is a protocol error of that class").
  - `:59` (the tag split).
  - The amended session design §3, line 120, together with line 105 ("Protocol errors end the session").
- **Violated invariant.** An overrun must be attributable to the peer that misbehaved. The contract §6 says "Current-binding invalid messages fault that binding only". Today a slow consumer is handled by backpressure: `Port::deliver` waits on `WouldBlock` (`asynchronous.rs:155–158`). D-N forbids the byte-stream reader from waiting, and the text puts nothing the peer can observe in place of that backpressure.
- **Interleaving A (frame count versus byte credit).**
  1. The client endpoint relays a slow GitHub upload-pack stream (sideband progress) to the core. gwz-transport flushes small data frames on its batch deadline (`outgoing.rs:69–72`).
  2. The core's consumer of that binding, the Git worker, pauses for about a second, for example on an fsync or an antivirus scan.
  3. More than 56 small data frames arrive. That is `queued_frames − control_reserve_frames` with the defaults (`binding.rs:4–18`). Every one is within the 1 MiB byte credit.
  4. The core's per-binding queue exceeds its frame bound, and the reader must declare a protocol error against a client that obeyed the protocol. Window credit is in bytes only (`max_offset`), so no peer can respect a frame-count bound.
- **Interleaving B (tag split versus gwz-transport's control reserve).**
  1. gwz-transport charges `flush`, `flushed`, `end_write`, `close`, `closed`, `opened` and `bind` against the control reserve (`mux/mod.rs:638`: everything but `Data`, `Open` and `CheckIdentity`).
  2. The object puts exactly those kinds on tag 16, the ordinary class (L59).
  3. A sender that queued 56 data frames and then a `flush` from its reserve puts 57 tag-16 frames on the wire.
  4. Whichever bound the tag-16 class gets (the data share, or the whole of `queued_frames` plus a separate tag-17 reserve), either the receiver faults a conforming peer or the combined bound exceeds the binding's negotiated `Limits`.
- **Consequence ambiguity.** "A protocol error of that class" is defined nowhere for the transport class. Two readings conflict:
  - session design line 105: end the session, which aborts every operation including a concurrent push mid receive-pack, with `Effect::possible`;
  - contract §6: fault the binding only.
- **Impact.** Under ordinary load, large fetches and pushes over socket, stdio and ssh fail at random, depending on disk and scheduler timing. The blame lands on the well-behaved side. Under the "ends the session" reading, unrelated concurrent operations are aborted too. This makes R5 ("truthful outcome") unprovable.
- **Required correction (within D-N's outcome).** State the transport-class bound in quantities a sender controls, and fix the consequence. For example:
  - The reader keeps per-stream state, not a frame queue:
    - `window` coalesces to the highest `max_offset`, which is monotone and idempotent (`incoming.rs` already ignores a lower limit);
    - `data` appends to a byte buffer bounded by `receive_window`;
    - each terminal or orderly kind (`flush`, `flushed`, `end_write`, `close`, `closed`, `cancel`, `failed`) holds at most one slot per stream, as gwz-transport's state machine already allows.

    Or define an explicit sender-side frame credit, which would require a schema change the object rules out.
  - Map gwz-transport's control/non-control charging onto tags 16 and 17 explicitly.
  - State that an overrun of a binding's bound faults that binding only (contract §6), and that only call-class and session-class overruns end the session.
- **Closure test.** 2.1 red row: a conforming sender emits its whole receive window as minimum-size data frames plus one `window` per consumed frame, while the receiver's consumer is paused until every frame is on the wire. The binding is not faulted. A deliberate overrun faults only its own binding while a second binding on the same carrier completes.

### P2-3: The priority writer lets `session.close`'s report overtake the step-6 replies it summarizes, so result replies are lost and outstanding calls are misreported as closed

- **Location.**
  - `GwzSplitCommandChannel.md:64` (writer) and `:40` (cancel and close).
  - `:201` (Python `close()`: "sends `session.close`, waits up to the close bound, disconnects and returns the close report").
  - The amended session design §3, line 119.
  - Session design §8, lines 419–427 (step 6 answers every outstanding call; step 8 sends the close report; step 9 closes the channel).
- **Violated invariant.**
  - Session design §10, line 482: "A result or response reply is never dropped".
  - R5: carrier and close paths end "with a truthful outcome".
  - In-process lanes "share one order" (`channel.rs:18–19`), so this reordering is new with D-N and the two adapters now differ in a way the bridge can observe.
- **Interleaving.**
  1. A gwz-py program has `operation_result(push_op)` outstanding, and another task calls `await client.close()`.
  2. The host runs §8. Step 6 queues the push's terminal result as an ordinary call-lane `SessionReply`, along with held event reads holding their last records.
  3. Step 8 queues the close report. It is a reply on the control lane ("A reply goes on the lane of the call it answers").
  4. The writer writes "the oldest control frame ahead of ordinary frames". The close report's target (its own `call_id`) differs from the step-6 replies, so no exception applies, and it is written first.
  5. The bridge receives the close report and disconnects (L201). Step 9 closes the channel, and nothing says the writer drains first.
  6. The push's terminal result, which may carry `Effect::possible` or cleanup facts, never arrives. `operation_result` fails with `server_session_closed`.
- **Impact.** A completed or partly completed push is reported as a closed session. The caller cannot tell success from interruption. This contradicts §10's never-drop rule and R5.
- **Required correction.** Either of these:
  - (i) Exempt the close report from priority: it is written only after every frame the host queued before it.
  - (ii) The host's writer drains every queued frame before step 9's close, and `close()` keeps its reader running until EOF or the close bound, delivering late result replies to the operation view before failing what remains.

  State the chosen rule in both §3.3 and the amended session design §3.
- **Closure test.** 2.1/2.2b red row: with result and read replies held in the writer behind a slow reader, `session.close()` delivers every step-6 reply before or with the close report, and `operation_result` returns the push's terminal, not `server_session_closed`.

### P2-4: `gwz server start` refuses whenever `SSH_AUTH_SOCK` is set, which is every default macOS login and most Linux desktop sessions; the constraint is unauthorized and protects nothing on the same-user form

- **Location.**
  - `GwzSplitCommandChannel.md:136` ("`gwz server start` refuses the same way, so no served core starts holding one").
  - `:158` (Start).
  - `:213–216` (the §4.9 first-day sequence, `cd ~/ws && gwz server start` succeeding on macOS).
- **Violated invariant.**
  - Scope provenance: the plan places the refusal only on `gwz server stdio` (2.0(c), 2.3 "`gwz server stdio` refuses, in `cli` placement, an inherited `SSH_AUTH_SOCK`").
  - Parity (plan §1, "Windows is a full peer").
  - The documented surface that 2.6's Surface review "reads exactly".
- **Reproduction.** On stock macOS, `com.openssh.ssh-agent` (a system LaunchAgent) exports `SSH_AUTH_SOCK` to every session; this was verified locally. `cd ~/ws && gwz server start` therefore exits with the usage error L136 prescribes, and §4.9's sequence fails at its first command. GNOME and KDE desktops set it too. On Windows the OpenSSH agent is a fixed pipe with no variable, so `start` succeeds there: an asymmetry.
- **Why it buys no safety.** The served core is the same user on the same machine. On macOS it can recover the agent address with `launchctl getenv SSH_AUTH_SOCK`, or read `~/.ssh` directly. The driver-only builder (L158) already holds no endpoint and no agent. The plan's actual protection is the `stdio` refusal on a separate account or machine.
- **Impact.** The local socket form, D-D's first carrier, does not start out of the box on two of the three platforms. Users are pushed into ad-hoc `env -u` workarounds the help does not document.
- **Required correction.** Either of these:
  - `server start` launches its background server, and a `--foreground` server sanitizes itself before any thread starts, with the §3.8 list and `SSH_AUTH_SOCK` removed, mirroring the local stdio child (L169); it does not refuse.
  - Remove the clause and keep the plan's `stdio`-only refusal.

  In both cases §4.9 must stay true as written.
- **Closure test.** On the macOS and Linux CI runners, with the default session `SSH_AUTH_SOCK` and a canary `GH_TOKEN` present, `gwz server start` succeeds. The served process's environment, read from `/proc/<pid>/environ` on Linux and through a test hook on macOS, holds neither. The §4.9 sequence runs verbatim.

### P3-1: The writer's own-target exception needs each frame's target, which lives in the opaque body; nothing gives it to the body-blind adapter

- **Location.** `GwzSplitCommandChannel.md:64`; the amended session design §3, line 119; `session-channel/src/byte_stream.rs:129–131` (`send(frame, lane)`); `session-contract/src/frame.rs` (adapters never decode a body).
- **Consequence.** An implementation that can see only `(tag, lane)` must either drop the exception or drop the priority.
  - Dropping the exception lets a tag-17 `cancel` or `failed` for stream X overtake that stream's tag-16 `open` or `data`. gwz-transport then rejects a frame for an unknown stream above `highest_id` as `Protocol` (`routing.rs:87–91`) and disconnects the binding (`routing.rs:4–16`).
  - Dropping the priority loses D-N's guarantee.
- **Correction.** State the interface: core passes each frame's target key with `send`, either `call_id` or `(request_id, session_id, stream_id)`. Alternatively, define it as a header outside the body. Name which side computes it.
- **Closure test.** A 2.1 row: a `cancel` queued after `open` and `data` of the same stream is received after them, and the binding is not faulted.

### P3-2: §3.6's classification of what crosses omits `Bound`'s client-configuration fields

- **Location.** `GwzSplitCommandChannel.md:117–126`. Compare `:256` ("the client advertises that set per binding in `bound.policies`") and gwz-transport `protocol/transport.taut.py:53–54` (`Bound` carries `schemes`, `policies` and `receive_limits`).
- **Consequence.** The core learns which authentication policies the client's configuration permits (for example `gh`, or a Windows native policy), which helps it choose a policy. The plan's 2.0(a) asks for the classification. R7's carrier-capture assertions (2.5) are written from this table, so an unlisted field is never checked.
- **Correction.** Add the `Bound` fields as sensitive metadata (client configuration, client to core), with the reason they are acceptable.
- **Closure test.** The R7 capture inventory names every field of every client-to-core envelope.

### P3-3: The credential scrub is a closed deny-list; `SendEnv` and `SetEnv` from the user's ssh configuration carry every other variable to the core machine

- **Location.** `GwzSplitCommandChannel.md:134–136`, `:170`.
- **Evidence.**
  - `ssh -G` shows that `SendEnv` patterns from configuration survive the argument vector, and that `-o SendEnv=-*` cannot clear them.
  - A command-line `-o SetEnv=…` does replace the configuration's `SetEnv`, yet the text relies only on the server-side refusal, which covers only listed names.
  - The ssh child inherits the client's whole environment minus the list.
- **Consequence.** Proxy URLs with user info (`https_proxy=http://user:pass@…`; contract §7 forbids "Credentials … smuggled into proxy URLs"), other services' tokens, and so on reach the core machine whenever the user's `SendEnv` matches and the remote `AcceptEnv` permits. That contradicts the heading "Credential variables never leave the client" for any credential not on the list.
- **Correction.** Either of these:
  - Start `ssh` with an allow-listed environment (what `ssh` needs: `PATH`, `HOME`, `USER`/`LOGNAME`, `SSH_AUTH_SOCK`, locale and terminal variables) and add `-o SetEnv=<inert>` so the configuration's `SetEnv` cannot apply.
  - State the residual explicitly in §3.8 and 6.4.
- **Closure test.** The §7 probe (1), run with `SendEnv *` in the client's configuration and a canary `https_proxy` carrying user info, finds no canary on the remote side.

### P3-4: There is no client-side stall bound against a silent core

- **Location.** `GwzSplitCommandChannel.md:161` (the 60-second rule is host-side only), `:170` (no `ServerAliveInterval` or similar in the vector), `:200` (Python follows session design §10, which has no stall timer).
- **Consequence.**
  - A `SIGSTOP`ped local core, or an `ssh://` path that goes silent (sleep, network partition, no EOF), leaves a CLI call waiting until the user presses Ctrl-C twice.
  - A gwz-py task-cancel waits "shielded" for a cancel reply that cannot be written, so only `close()` recovers.
  - R5 ("bounded … across the carrier") is not stated for this direction.
- **Correction.** A symmetric rule: the client closes the carrier as lost when it has frames queued and the core accepts no byte for the same bound. Also state the ssh keepalive, or rely on that rule.
- **Closure test.** A 2.5 R5 row: a core stopped mid-fetch, then the client ends within the bound with `server_session_closed` and its bindings revoked.

### P3-5: Python carrier loss during a push does not report the possible remote effect the CLI reports

- **Location.** `GwzSplitCommandChannel.md:162` ("the client reports a push interrupted after its receive-pack exchange began as a possible remote effect") versus `:200` ("outstanding calls fail `server_session_closed`").
- **Consequence.** A gwz-py caller cannot tell an interrupted push from one that never started, although the client's own binding revocation knows `Effect::possible` (L87). This conflicts with R5's truthful outcome.
- **Correction.** State that the bridge attaches the revoked bindings' effect to the failed call: `server_session_closed` with `possible_remote_effect` when a `receive_pack_exchange` had begun.
- **Closure test.** Drop the carrier mid-push through `SocketCoreBridge`; the raised error carries the possible effect.

### P3-6: The per-user directory can be removed by the system under a running server, and the text does not say what happens

- **Location.** `GwzSplitCommandChannel.md:15` and `:152–153`, the object's own macOS choice of `_CS_DARWIN_USER_TEMP_DIR`. Linux's `$XDG_RUNTIME_DIR` is adopted unchanged from server design §4.
- **Evidence.** dirhelper's launch daemon runs at load and daily, with `CLEAN_FILES_OLDER_THAN_DAYS=3` over the user temporary directories. Connecting to a socket does not refresh its times. That dirhelper deletes sockets and lock files is inferred from its configuration and needs a probe. On Linux, logind removes `/run/user/<uid>` at last logout unless lingering is enabled.
- **Consequence.**
  - The server's socket and `ws-<key>.lock` vanish while it runs. Clients get `server_unavailable`, `stop` cannot reach it, and `list` does not show it.
  - A new `start` takes a fresh lock while the orphan holds a lock on the unlinked inode. That defeats server design §4's "the lock file stays", so two servers exist for one workspace, and one of them is unreachable with idle exit off by default.
  - The workspace try-lock prevents concurrent writes, so the impact is recovery and diagnosability, not corruption.
- **Correction.** Do at least one of these: choose `_CS_DARWIN_USER_DIR` (per-user, 0700, not swept as temporary files), have the server periodically verify that its socket and lock inode are still linked and exit or warn if not, and state the logind case.
- **Closure test.** A probe on macOS: age the directory's entries and run dirhelper; or simulate the unlink; the server's behavior matches the text.

### P3-7: The trust model does not say that admission is not confinement on the same-user forms

- **Location.** `GwzSplitCommandChannel.md:8` (Terms: the core "holds no credentials"), `:135` and `:169` (removing `SSH_AUTH_SOCK` from the local stdio child), and §5.
- **Consequence.**
  - A served core on the same user and machine (socket, `--server stdio`) has the user's full authority. It can read `~/.ssh`, find the agent (`launchctl getenv SSH_AUTH_SOCK`), open Pageant's pipe, or edit `~/.gitconfig` to add a `gwz.server.allow-push` grant for itself.
  - Admission there prevents accidents; it does not stop a hostile core. The protection the operator's threat model asks for holds only for `ssh://` and the separate-user launch.
  - 6.4's help text and R7's claim are written from this text and can overstate the protection.
- **Correction.** One sentence in §5.1 or §3.8: for which forms admission is a security boundary, and for which it only prevents misdirected requests. Server design §4's "A server acts with its user's full authority" still holds there.
- **Closure test.** 6.4's help text and the R7 README state the per-form boundary.

## 2. Invariant analysis (attacks that failed, with evidence)

1. **Fetch turned into push.** The class comes from the client's own request (L175, L248), and receive-pack needs both a push allowance and a push-class request (L249). This holds except for `dry_run`, which is P2-1.
2. **A different host, port or scheme.**
   - Every `bind`, `open` and `check_identity`, and every redirect hop, is admitted at the client before any credential, helper or connect (L233, L263).
   - The canonical form refuses IDNA tricks, non-LDH labels, zones, `..` and encoded separators (L240–242).
   - The port must match, and no prompt grants a non-default port (L258, L19).
   - `http://`, `git://` and `file` are refused (L192).
   - A host rewritten by the core's `insteadOf` is still only an `Open` (L111).
3. **Into a credential helper or native authentication.**
   - A non-empty `auth`, extra headers and generic HTTP are refused on both sides (L84).
   - The policy must be in the client's set (L256).
   - NTLM and Negotiate answer only for `--allow-native-auth` hosts that are also allowed, including on redirect (L262–263).
   - The core cannot supply a profile, `AuthContext` or policy (L72).
4. **Choosing an identity or username.** A core-named key path is refused, `ambient` is refused when the invocation named a key, and a core-supplied username is admitted only when absent or equal (L254–255). `CheckIdentity` is asked only for paths the client named (L121).
5. **Command injection on the Git host through the path.** The path rules permit `'`, but the client endpoint builds the exec command from the typed destination with sq-quoting (`ssh_destination_tests.rs:241–245`). The `ssh://` server form uses `--` and fixed remote text (server design §16).
6. **Prompt spoofing.** Hosts are shown in punycode and control characters are removed (L269). Bidirectional or homoglyph paths can only name repositories that do not exist on hosts with ASCII repository names, and the grant is an exact fetch tuple on the default port for one invocation (L19).
7. **`forall` on core-reported paths.** It is refused over `ssh://` and the separate-user launch (L189). On same-user forms the core already has the user's authority (P3-7), so nothing is gained.
8. **Agent forwarding through `~/.ssh/config`.** `-a` and `-o ForwardAgent=no` override `ForwardAgent yes` (verified with `ssh -G`). `ClearAllForwardings=yes` clears configured forwards. `-x` overrides `ForwardX11`.
9. **Riding an existing ControlMaster.** The multiplexed session requests agent and X11 forwarding only when the mux client asks, and gwz's client does not. An agent forwarded by the user's other `ssh -A` sessions to the core machine is the user's own exposure; the text cannot close it. This is recorded as a residual, not a finding.
10. **Increasing call IDs under reordering.** A `SessionCall` is never overtaken, and control frames keep FIFO order among themselves (L64), so `call_id` order on the wire equals enqueue order.
11. **Starvation.**
    - Ordinary frames can starve only while control frames keep arriving.
    - Tag-17 output is produced in response to the peer's own traffic, or to this end's cancels, which are bounded by the 64-call control reserve. A peer can therefore starve only its own operations.
    - A `SessionCall` waits only for control frames older than it.
12. **Stuck `Window`.** The reader never blocks, so the peer's writer always drains. The resulting risk is the overrun in P2-2, not a deadlock.
13. **Lifecycle.**
    - Stale socket: removed only with ownership, a refused connect and a matching record (L164, server design §4).
    - Two starts: serialized by the lock.
    - A moved workspace: requests naming another root are refused (L47).
    - `stop --force`: the cost is stated in the server design (lines 1398, 589).
    - Client killed: carrier loss cancels operations at their gates, revokes bindings and reports a possible effect (L162).
    - Core killed mid-write: Git's own lock files, the same as a killed in-process CLI, so nothing new.
14. **Squatting the Windows pipe.** The name is random per start, created with `FILE_FLAG_FIRST_PIPE_INSTANCE`, `PIPE_REJECT_REMOTE_CLIENTS`, a DACL for the user's SID only, and listener verification of SID and integrity (L153, server design §4). A lower-integrity process of the same user that reads an old name from configuration cannot pass listener verification.
15. **Mixed versions.** The handshake fails closed:
    - a non-gwz or older remote produces no well-formed `SessionHello` within 30 seconds;
    - a core without `cli` and a common version is refused before any `bind` (L206);
    - tags 18–31 are a protocol error.

    Observation: the text does not say that assigning a tag in 18–31 later requires a protocol-version bump, which would turn a mid-session protocol error into a `server_version_mismatch` at the handshake. No such peer exists in this release.
16. **Environment crossing in `SessionOpen`.** Every host requires `environment` absent with the marker set, and refuses anything else before `SessionOpened` (L37).

## 3. Risks and next action

- **Blocking.** P2-1 (dry-run push admits receive-pack), P2-2 (reader bound and overrun consequence), P2-3 (close report reordering) and P2-4 (unauthorized `server start` refusal). Each has a bounded fix in the text and a named closure test. None needs a code change in this step, a change to gwz-transport's schema, or a reopening of D-N's outcome:
  - P2-2's remedy defines the bound in quantities the sender controls;
  - P2-3's remedy pins the close report's order.
- **Nonblocking.** P3-1 to P3-7 should be fixed in the same revision, because 2.1, 2.3, 2.5 and 6.4 write tests and help text directly from these clauses.
- **Residuals to record in 6.4.**
  - Same-user forms are not confinement (P3-7).
  - An agent forwarded to the core machine by the user's other sessions.
  - A host-only or prefix push grant lets the core push to any repository under it (D-H (b), decided).
- **Next action.** The lane owner prepares revision 2 resolving P2-1 to P2-4, then a re-review on this axis limited to those findings and to any text the fixes touch. Under that condition I pre-commit to GO.
