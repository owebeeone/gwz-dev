# GWZ core session host — implementation plan

Date: 2026-09-26. Status: **draft; not implementation authority**. As of 2026-09-27, the [transport release plan](../gwz-core/dev-docs/GwzTransportReleasePlan.md) carries this plan's work in the transport release. Its TR1.4a and TR1.4b revise this plan before any step starts, including every statement here that this work follows 1.1.0.

This is the phased plan that item 2 of [the proposals §9](GwzClientCoreTransportProposals.md) calls for. It implements the [core session contract](GwzCoreSessionDesign.md) in a release after 1.1.0, in six phases and 51 steps, one of them conditional. It cites the contract by section number; sections keep their numbers across revisions 2, 3 and 4. It maps the findings that [Verdict-2](GwzCoreSessionDesign-Verdict-2.md), [Verdict-3](GwzCoreSessionDesign-Verdict-3.md) and [RemPlan-2](GwzCoreSessionDesign-RemPlan-2.md) carry onto named steps (§5).

The plan authorizes nothing by existing: no implementation, commit, tag, push or publish. Code facts were read at root `9a653065`, gwz-core `b13bbadb`, gwz-cli `ebbea902`, gwz-py `4ad2b077` and gwz-transport `a7a36aec`, called the planning tuple below.

## 1. Purpose and scope

### 1.1 What 1.1.0 already does

The [1.1.0 plan](../gwz-core/dev-docs/GwzV110Plan.md) is amended by [GwzV110PlanAmendment.md](../gwz-core/dev-docs/GwzV110PlanAmendment.md), accepted on 2026-09-26 at SHA-256 `cb4ae166…` ([its verdict](../gwz-core/dev-docs/GwzV110PlanAmendment-Verdict-2.md)), so that 1.1.0 ships only a minimal unification. Its Phase 1 first revises gwz-py's transport design to the per-operation model (S1.1, reviewed in S1.2). Its Phase 6 then builds:
- **1.1.0 S6.1:** a variant of gwz-core's `transport_host::with_local_transport` that takes a caller-supplied cancellation token. It is the token part of the contract's §5.2 session variant. The endpoint-environment and timeout inputs are not in 1.1.0.
- **1.1.0 S6.2:** gwz-py backs out its long-lived `TransportSession` (`gwz-py/native/src/transport_session.rs`) and runs each network operation through that entry, one runtime per operation, as gwz-cli does per command. A Client runs at most 8 network operations at once. The public Python API is unchanged.
- **1.1.0 S6.3:** tests, including two overlapping Python operations on one Client, each on its own runtime, with transport-route assertions on macOS, Linux and Windows.

1.1.0 also turns the transport's construction sites into `cfg_if` unix and windows arms (S4.5). S7.1 removes the candidate switch from gwz-core, gwz-cli and gwz-py, so the normal build ships the transport on macOS ARM64, Linux x86-64 and Windows x86-64. S7.3 asserts the transport route on those normal builds before any tag. This plan starts from that tree and repeats none of it.

### 1.2 In scope

Everything else in the contract:
- the schema additions and the model-to-wire error mapping (§4, §13);
- the channel and its two adapters (§3);
- the host context, the session context and operation gates (§5.6), and the removal of the process-global state they replace: every `debt` entry of §5.7's allowlists that lies on the session path;
- admission, workers and the shared dispatch (§5.1, §5.2). The dispatch moves gwz-cli's `execute_invocation` (`gwz-cli/src/globalargs/dispatch.rs`) into gwz-core as a dispatch over protocol requests. Today that function is CLI-specific: it chooses the event sink, holds a guard across `forall`, and applies the open-merge pre-gate. gwz-core has no such dispatch. gwz-py's native `dispatch/` module, about 1,800 lines, is a second routing table that the move retires;
- cancellation, retention, events and results, and closure (§5.3, §5.4, §6, §8);
- ordinary-build behaviour, including core's own `git credential fill` (§5.8);
- the gwz-py extension's channel and the thin `NativeCoreBridge` (§9, §10);
- the wire proof (§12);
- gwz-cli onto the session host (§11).

The proposals' outline maps onto the phases: its item (1) is Phase 2, (2) is Phase 3, (3) is Phase 4, (4) is Phase 5 and (5) is Phase 6. Phase 1 freezes the interfaces they share.

### 1.3 Out of scope

- 1.1.0 S6.1, S6.2 and S6.3 (§1.1).
- Client placement, item (6) of the proposals' outline. The contract excludes it (§1) and reserves only frame tags 16–31 (§3). It needs a contract amendment when a remote core is scheduled.
- The socket server of [GwzCoreServerDesign.md](GwzCoreServerDesign.md), which is unreviewed. It assumes the contract is implemented as far as gwz-cli's move, so it can start after Phase 6. It is a follow-on, not a phase.
- Connection reuse across operations (§1, §16).
- Any change to gwz-transport, the virtual-stream protocol or the public Python API (§1).
- The half-landed crate rename that breaks the transport candidate build (§2.4).
- Migrating existing code, outside the files a step touches, to the conditional-compilation rule. CS1.7 inventories that code; it does not migrate it.

## 2. Prerequisites and gates

### 2.1 G0 — an accepted contract revision

No step starts until a filed verdict accepts, on both axes, the contract revision the step implements. On 2026-09-26:
- revision 2 is accepted ([Verdict-2](GwzCoreSessionDesign-Verdict-2.md));
- revision 3 is NO-GO on one blocking root, B11, and revision 2's acceptance stands ([Verdict-3](GwzCoreSessionDesign-Verdict-3.md));
- [RemPlan-2](GwzCoreSessionDesign-RemPlan-2.md) proposes revision 4, which is not applied.

RemPlan-2 §4 leaves three decisions to the operator. This plan works on any of three paths (decision D1):
1. **Revision 4 carries RemPlan-2 §1 and §2 and is accepted (recommended).** Steps follow revision 4. The findings RemPlan-2 corrects are contract text, and their closure tests are §15 rows.
2. **Revision 4 carries only B11 and is accepted** (RemPlan-2 §4, decision 2, second option). Steps follow revision 4. RemPlan-2 §2's ten findings become obligations of the steps that §5.2 names, each with its closure test. A step that follows one of them where the accepted text differs records the finding it follows.
3. **Revision 4 is not applied.** Revision 3 cannot be implemented. Starting on revision 2 needs the operator's explicit direction, recorded in the program checkpoint. Verdict-2's ten findings (as revision 3 worded their corrections), B11 (through CS1.8) and RemPlan-2 §2's ten findings then all become step obligations. This path is not recommended: revision 2 lacks corrections that both axes have already made.

On any path, the paired DRAFT paragraphs in gwz-core's [GWZDesign](../gwz-core/dev-docs/GWZDesign.md) and [GWZRequirements](../gwz-core/dev-docs/GWZRequirements.md) are flipped to the accepted revision (Verdict-2, next action 1) before CS1.1, because gwz-core's `AGENTS.md` requires them before core behaviour expands. A revision accepted after this plan triggers the re-check in §7.

### 2.2 G1 — this plan's review

Dual peer-blind review of this document, with GO on both axes, before any step starts (§7).

### 2.3 G2 — the 1.1.0 steps this plan builds on

- **1.1.0 S6.1.** CS3.7 extends that token-taking entry with the endpoint environment and the session's timeouts. If S6.1's final signature differs from this plan's reading, CS3.7 adapts to it.
- **1.1.0 S6.2.** gwz-py's post-1.1.0 native layer is Phase 4's baseline. By then `TransportSession` and its `CURRENT_SESSION` thread-local, a `debt` entry in gwz-py's allowlist, are gone. The 8-operation Client limit is the one CS4.7 retires.
- **1.1.0 S6.3.** Its overlapping-operation and route tests are regression tests that must pass through the session host (CS4.7).
- **1.1.0 S4.5 and S7.1.** The transport compiles on Windows and ships in the normal build. Phase 3's route proofs run against that build.
- **Merge timing (decision D9).** Product code from this plan merges to the product repositories' main branches after the 1.1.0 tags exist. Interface drafts and freeze reviews may start earlier.

### 2.4 Execution prerequisites

These are not planned here.
- **A transport build.** Until 1.1.0 S7.1 puts the transport in the normal build, transport tests need `gwz-core/tests/transport_backend/prepare.py`. A half-landed crate rename breaks it: its `[patch.crates-io]` entries still name `git2` and `libgit2-sys`, while gwz-core now depends on the packages `gwz-git2` and `gwz-libgit2-sys`. Phase 3's transport tests cannot run until the rename is finished or S7.1 has landed.
- **Python 3.11 or later** for gwz-core's `scripts/run_tests.py`, whose crate-version check imports `tomllib`.
- **The dabeest Windows host** for Windows transport evidence, under the 1.1.0 plan §2's rules (mingw bash, a work root on the E: drive). GitHub's windows-2022 runners cover builds and every test that needs no network fixture.
- **Evidence handling.** Raw runs go to the private gwz-core-evidence member ([EVIDENCE.md](../EVIDENCE.md)). Public reports redact agent-socket paths, known_hosts bodies, and `gh` tokens and headers, as the 1.1.0 plan §2 requires.

## 3. Phases and steps

### 3.0 Step format, standing rules and review tiers

**Format.** Each step names:
- its repository and files, which are also its ownership manifest: two steps that name the same file never run at once (L1-06);
- the contract sections it implements;
- **test-first:** the §15 rows and carried findings it closes, written as failing tests before the change (gwz-core `AGENTS.md`, L1-12). §15 rows are cited as "§15.n (short description)", because bullets can move between revisions;
- its dependencies;
- its review tier;
- a budget: aspirational hand-written production lines. Tests, generated code and moved code are reported separately (GwzProcessOptimization §2.1).

**Standing rules.**
- gwz-core stays independent of gwz-cli. The session host, the dispatch and the test bridge live in gwz-core; rendering stays in gwz-cli.
- Payloads are Taut messages (L2-02, L3-06). Frames carry Taut-encoded bodies. No step adds a Rust-only or Python-only wire type.
- Conditional compilation sits in `cfg_if` blocks or enclosing platform modules, never as a bare `#[cfg]` on an import or another unbraced declaration. Every control-flow body is braced. A step that moves or deletes a declaration moves or deletes its attributes and owning scope with it. CS1.7's check runs in every step's gate.
- Every step builds and passes on macOS, Linux and Windows CI. Steps on the transport path also pass on dabeest.
- The process-global ratchet moves in the same commit as the code. A step that removes state removes its allowlist entry. A step that adds state lists it as `debt` and names the step that removes it. A spawn entry flips from `debt` to `permanent` only with a test that proves `env_clear()` plus the snapshot, since the checker cannot see `env_clear` (Consistency-3's residual on "the lists only shrink").
- The Rust quality gate of L2-01 applies: format, Clippy with warnings denied across targets, tests.
- New files stay under 500 lines. A step that would take an existing file past about 1,000 lines splits it first in a movement-only commit (L1-11, L1-23), using the rust-split tool.
- Documentation changes land in the step that changes the behaviour (L1-24).
- Legacy entry points keep today's behaviour until their driver moves (CS3.1).

**Review tiers** follow GwzProcessOptimization §4.2 and the review-loop rules, cross-model where available (§4.3):
- **Dual:** peer-blind Consistency and Safety. Used at the four interface freezes (CS1.1 schema; CS1.2 channel contract; CS1.4 with CS1.5, the gate and context API; CS1.6 dispatch signature), at CS3.4, and at the exits of Phases 3, 4 and 6, which change behaviour for users. Freezes that settle together may be reviewed as one interface checkpoint (AgentProcessRules §6.1), still dual.
- **Surface:** added at the Phase 4 exit (the Python API and gwz-py's `gwz` console script) and at the Phase 6 exit (gwz-cli help and output).
- **Single-axis:** every interior step behind a frozen interface. The step names the first axis, and axes alternate. Any P0, P1 or P2, or a reviewer's request, escalates to the second axis. Remediation re-reviews are single-axis by default.
- **Activation steps** (CS4.7, CS6.4) are reviewed by their phase exit's review on the settled tree (L1-32).
- The tiers are recorded in `CurrentProgramCheckpoint.md` when this plan is accepted (§7).

### Phase 1 — Frozen foundations (milestone: the schema additions and the frozen interfaces are merged and tested; nothing calls them)

- **CS1.1 — Schema additions and error mapping** *(gwz-core and gwz-py; < 250 lines, generated code excluded)*.
  - Files: gwz-core `protocol/gwz.taut.py`, the bindings and corpus that `protocol/regen.py` regenerates, `src/protocol/convert.rs`, `docs/MessageCatalog.md`, `docs/ErrorCatalog.md`, `docs/Protocol.md`; gwz-py `src/gwz/protocol/generated/`, through `scripts/regen_protocol.py` and `scripts/check_protocol_drift.py`.
  - Implements §13 in full, §4.1, and §4.2's error codes, `convert.rs` mapping and `cancelled` comment.
  - Test-first: §15.2 (a `SessionError` with each named code round-trips through the Rust and Python projections; each §13 body round-trips in both languages). Verdict-2 residual: the Taut `CleanupReport` coincides with `gwz_core::transport_host::CleanupReport`, and the crate re-exports generated types at its root, so the step renames the Rust type to keep one meaning per name; §13 fixes the Taut name. `OperationCancelRequest`'s comment states that zero or two targets is `invalid_request` (CS2.7 tests it). The new messages stay out of `gwz.__all__`.
  - Depends on G0 and G1. Review: dual, the schema freeze.

- **CS1.2 — Frames, the channel contract and the in-process adapter** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/frame.rs` and `src/session_host/channel.rs`, registered in `src/lib.rs`.
  - Implements §3: the carrier guarantees; a tag byte and a deterministic-CBOR body; tags 1–3 and the reserved 16–31; the frame-level protocol errors (any other tag, an undecodable body, an oversize frame); the in-process adapter's two bounded queues, each holding the outstanding-call limit plus a 64-frame control reserve; a `send` that never blocks and fails with `transport_session_full`; control calls on the reserve; a `recv` that blocks until a frame arrives or the session ends; closure reported to both ends. Also O6.
  - Test-first: §15.10 (`send` on a full queue refuses and never blocks or drops; a frame over 64 MiB ends the session); a reserved-lane tag ends the session; both ends observe closure.
  - Depends on CS1.1 for merge order; the queues can be built beside it. Review: dual, the channel-contract freeze.

- **CS1.3 — The byte-stream adapter** *(gwz-core; < 250 lines)*.
  - Files: new `src/session_host/byte_stream.rs`.
  - Implements §3's byte-stream adapter: a little-endian `u32` length before each frame, as taut-shape's interop tool frames it; end of stream is closure.
  - Test-first: frames round-trip against the framing of `taut-shape-rs/crates/taut-shape-tool`; a length prefix over 64 MiB ends the session before anything is allocated; §15.9 (the byte-stream host closes its channel), adapter half.
  - Depends on CS1.2. Review: single-axis, Safety first.

- **CS1.4 — Contexts, gate, token and `open`** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/context.rs`, `limits.rs` and `gate.rs`; `docs/RustApi.md`.
  - Implements:
    - §5.6's host context and session context as types, whose members later steps fill;
    - O7, and O8 with the gate's three states: live, cancelled, revoked;
    - §1's limits, and `open`'s validation of them, including that the table holds at least the running plus the queued operations;
    - §9's `open(options)`: limits, snapshot and host context in, the client end of an in-process channel out;
    - how a handler reaches its gate: through the handler's context, which gains the token (§5.2, §16).
  - Test-first: limit validation; after cancellation the gate refuses effectful requests and log appends with `Cancelled`; after revocation it also ignores events and terminals; nothing but the token cancels; dropping a host context ends its supervisor thread once its jobs finish.
  - Depends on G0 and G1. Review: dual, the gate and context freeze, together with CS1.5.

- **CS1.5 — The endpoint environment snapshot** *(gwz-core; < 300 lines)*.
  - Files: new `src/session_host/environment.rs`.
  - Implements §5.6's endpoint environment: captured by the driver as byte-string pairs; secret-bearing, so values have no `Debug`, `Display` or serialization; dropped with the session. Also its spawn helper, `env_clear()` plus the snapshot, and O9.
  - Test-first: non-UTF-8 values survive on POSIX; on Windows, WTF-8 pairs with an unpaired surrogate decode to the same `OsString` (Verdict-3 residual, Rust half); lookups follow the platform's name rules, case-insensitive on Windows (C7); no value appears in formatted output or in an error (§15.8, an invalid proxy or CA error carries no environment value, core half); a child spawned by the helper sees exactly the snapshot.
  - Depends on G0 and G1. Review: dual, with CS1.4.

- **CS1.6 — The shared dispatch signature and method registry** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/dispatch/mod.rs`; the request inventory below, filed with the step's review package.
  - Implements §5.2's shared dispatch as a signature over the method name, the request bytes and the operation's context; §5.1's class table (R, N, W; a method missing from it is W); §4.2's method kinds. For each method the registry records its request and response messages, class, transport scope, open-merge command (ported from gwz-cli's `open_merge_gate_request`) and declared views: the result, and for `merge` the response too.
  - Obligations:
    - The eight direct methods are listed: `status`, `ls`, `resolve_forall_targets`, `list_snapshots`, `diff`, `log`, `transport_capabilities` and `configure_transport_runtime` (Verdict-2 residual).
    - There is one transport-scope predicate. Today gwz-cli's `transport_meta` includes `repo_sync` and gwz-py's `network_meta` includes `attach_repo_member`, and each lacks the other's. The registry takes the union unless a test shows that a method does no network I/O (D5). A method left out would run on libgit2's native backend in a transport build.
    - A method the service does not declare is refused before any effect (C4).
    - The inventory maps every gwz-cli `CliRequest` and every gwz-py native route to a protocol method, or to a named CLI-local exception: `forall`'s command execution with its DR-3 workspace guard, `init --update` (no protocol method exists), and `claude-code setup` (C2, D3).
  - Test-first: §15.4 (a method missing from the table is treated as W); each method's class, transport scope and open-merge command equal gwz-cli's and gwz-py's current answers, or the difference is a recorded decision.
  - Depends on CS1.1 and CS1.4. Review: dual, the dispatch-signature freeze.

- **CS1.7 — Conditional-compilation boundary check** *(gwz-core tooling; < 350 lines)*.
  - Files: new `scripts/checks/check_cfg_boundaries.py`, with its tests and allowlist; `scripts/run_tests.py`. The check covers gwz-core with its `crates/`, gwz-cli and gwz-py's native crate.
  - Implements the root `AGENTS.md` rule on explicit scope. Like `check_process_globals.py`, it is a lexical scan that inspects every platform arm without compiling any. It flags a `#[cfg(...)]`, or a conditional `#[cfg_attr(...)]`, placed directly on a `use` item or another unbraced declaration. The existing occurrences are listed, about 90 at the planning tuple and almost all `cfg(test)` imports, and the list only shrinks.
  - Test-first: a bare `#[cfg(windows)] use …;` in a disabled arm fails; the same import inside a `cfg_if` block passes; a listed occurrence that disappears fails.
  - Closes part (e) of the 1.1.0 amendment round's Safety P2-1, as handed to this plan: every cfg-site edit is checked in its disabled arm.
  - No dependencies. Review: single-axis, Consistency first.

- **CS1.8 — gwz-transport's process-global check in CI** *(gwz-core tooling; < 200 lines; only if revision 4 has not applied RemPlan-2 §1)*.
  - Files: `scripts/run_tests.py`, `scripts/checks/process_globals_allowlist_gwz_transport.json`, `.github/workflows/checked-artifact-boundary.yml`, `release.yml`, `platform-matrix.yml` and `windows-matrix.yml`, and a test beside `scripts/checks/test_run_tests_filesystem_mode.py`.
  - Implements parts 1 and 2 of RemPlan-2 §1: fail closed locally, run the check pinned in gwz-core's CI, and the bump rule. Part 3, the text, stays with the contract.
  - Test-first: RemPlan-2 §1's closure tests.
  - No dependencies. Review: single-axis, Safety, the axis that raised B11.

**Exit.** CS1.1–CS1.7, and CS1.8 where it applies, are merged. The freezes have GO. gwz-core and gwz-py CI is green on macOS, Linux and Windows. No driver calls the new code, so there is no further review.

### Phase 2 — The session host with local operations (milestone: core serves every local operation over the in-process channel; no driver uses it)

The host serves operation methods through the shared dispatch, driven by the core test bridge. In a build with the transport, a transport-scope request is refused before any effect, with `unsupported_operation`, until CS3.10. It never runs on the default backend in such a build, which would be a silent native route.

Lanes: A, the host (CS2.1–CS2.7, CS2.9, CS2.12); L, the logs (CS2.10, CS2.11); B, the routes (CS2.8, CS2.13–CS2.16).

- **CS2.1 — The core test bridge and test hooks** *(gwz-core, test code; < 350 lines)*.
  - Files: new `src/session_host/tests/`; a test-only switch for the hooks that gwz-py's and the host binary's tests also need, which the process-globals checker treats as test code; `src/workspace_ops/merge/preserve/artifacts.rs`.
  - Implements §2's test bridge, which chooses call IDs and registers each waiter before `send` (O7). The hooks are latch methods in each class (R, N, W), a handler that panics, a fault injected into finish, and a latch on resolution. Coverage claim (GwzProcessOptimization §5.2): no existing harness drives framed calls. The step also gates `V1_PRESERVATION_IMAGE_CAPTURES` behind `cfg(test)`, as §5.7's test-hooks row requires, and removes its `debt` entry.
  - Test-first: the bridge's own tests over a loopback channel; a workflow-text test shows that no release workflow enables the hook switch (L2-12).
  - Depends on CS1.1 and CS1.2. Review: single-axis, Consistency first.

- **CS2.2 — The reading thread and receipt records** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/host/mod.rs` and `host/reader.rs`.
  - Implements O1, O2 and O7: the record, token, gate and `operation_id` are created when the frame is read (§4.3). Also §3's call-ID protocol error; §5.1's rule that control frames and reads never wait for admission (cancel, close and log reads are handled without blocking, and a held read is parked as a waiter); and the outstanding-call limits of 1024 ordinary calls and 64 control calls.
  - Test-first: §15.1 (exactly one reply per call; a duplicate outstanding call ID and a lower one each end the session, and the original call fails with the closed-session error, core half); §15.10 (a frame over 64 MiB ends the session at the host); §15.5 (a call ID above the highest received gets `operation_not_found`). Verdict-3 residual: a call ID below the highest that the session never saw gets `operation_expired` (C5).
  - Depends on CS2.1 and CS1.4. Review: single-axis, Safety first.

- **CS2.3 — Admission** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/host/admission.rs`.
  - Implements §5.1: resolution then admission, on the admission thread, in receipt order and outside the table lock; refusal before any effect; the classes; one FIFO queue of 64; a running limit of 8; queue order per workspace; unique `request_id`s among live operations (§4.3); `transport_session_full` refusals.
  - Test-first: §15.4 (with 64 operations queued the next request is refused with no effect; a W excludes N and W on its workspace while R calls run; a second live operation with the same `request_id` gets `invalid_request`).
  - Depends on CS2.2 and CS1.6. Review: single-axis, Safety first.

- **CS2.4 — Workers and terminals** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/host/worker.rs`.
  - Implements §5.2: a worker thread per running operation; a spawn failure settles `Failed` with `internal_error` before start; 8 direct workers; the worker runs the dispatch and reports exactly one terminal through its gate; panics are caught. Also O3, §4.2's reply kinds and §6's projection of terminals into `OperationResult`.
  - Test-first: §15.6 (a handler panic yields `Failed` with `internal_error`, and the next call succeeds); a hook error carrying `member_id`, `target_kind`, `record_context` and `ResponseMeta` arrives unchanged in the `SessionError`.
  - Depends on CS2.3. Review: single-axis, Safety first.

- **CS2.5 — Event logs and results** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/events.rs`; `src/operation/push_event.rs`.
  - Implements §6: appends through the gate; any number of readers, each with its own cursor; nothing pushed. Also §5.4's event-log ring of 4096 events: on overflow older incremental events are dropped, a reset event is kept, and the result is kept separately. That brings `push_event`, which clears the whole buffer today, to the rule, as §5.4 requires for the V0 `OperationRuntime`. Also `events.subscribe` reads with taut-shape's `LogReadRequest` and `LogReadResponse`, and `operation.result` and `operation.response`, held until the operation is terminal. The V0 `submit`, `subscribe` and `wait` API is marked deprecated (§14; D10).
  - Test-first: §15.7 (an overflowing event log keeps its reset event and later events, and the result stays readable); `OperationRuntime`'s own overflow test follows the rule.
  - Depends on CS2.2 and CS1.4. Review: single-axis, Consistency first.

- **CS2.6 — Operation table, delivery and release** *(gwz-core; < 350 lines)*.
  - Files: new `src/session_host/host/table.rs`.
  - Implements §5.4's operation table: 128 entries; a record is delivered when every declared view is delivered, or when it is released; the oldest delivered terminal record is evicted; with none, the request is refused; a unary record is discarded after its reply. Also `operation.release` (§4.2): a live operation gets `open_operation`; a repeat or an evicted record gets `operation_expired`.
  - Test-first: §15.7 (72 submitted operations all stay readable; a table full of unread terminal records refuses before any effect and admits again after reads or releases; release refuses live operations with `open_operation`; a record whose method declares a response view keeps its response under a full table, core half of the merge-handle row).
  - Depends on CS2.3 and CS2.5. Review: single-axis, Safety first.

- **CS2.7 — Cancellation** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/host/cancel.rs`.
  - Implements §5.3 and §4.2's `operation.cancel`: a target named by `call_id` or `operation_id`, exactly one of them; unadmitted calls, queued operations and direct calls waiting for a worker settle `Cancelled`; queued starts and queued cancels share one table lock; the reply to a running target waits for its terminal; a terminal target is answered at once with its retained report; `operation_not_found`, `operation_expired` and `invalid_request` as §4.2 assigns them. Also §7's cancel before the accepted reply.
  - Test-first, §15.5, core halves: a submit cancelled by its call ID before its accepted reply settles `Cancelled`; a queued operation becomes `Cancelled` with no effect; success racing a cancel stays `Completed`; a local handler past its last gate crossing reports its own outcome; a cancelled queued submit shows `failed` with `errors[0].code == cancelled`; with resolution latched, a cancel settles that submit and its handler never runs, while a cancel of another, running operation completes within its bound; a queued unary call and a waiting direct call each yield `SessionError{cancelled}`.
  - Also test-first: §15.2 (a cancel racing its unary reply gets `operation_expired`, core half). Verdict-2 residuals: zero or two targets get `invalid_request`; a target that never touched the network reports `(0, false)`, which is today's `CleanupReport::default()` and §8's close rule (C3).
  - Depends on CS2.4 and CS2.6. Review: single-axis, Safety first.

- **CS2.8 — Gate crossings in core's lock paths** *(gwz-core; < 350 lines)*.
  - Files: `src/operation/workspace_mutator_lock.rs`, `src/workspace_ops/merge/runtime/mutation_guard.rs`, `src/operation_context.rs`, `src/git/gitbackend/backend.rs`.
  - Implements §5.6 (a worker reaches the workspace mutator lock only through its gate), §5.3 (every gate crossing is a cancellation point, local handlers included) and O8. A legacy caller passes no gate and keeps today's behaviour.
  - Test-first: a W handler cancelled before its lock acquisition settles `Cancelled` with no effect; after revocation the next lock request fails with `Cancelled` (§15.9, workspace-lock half); §15.4 (no R method's handler acquires the workspace mutator lock), for all eight direct methods, called with a gate that records lock requests (Verdict-2 residual).
  - Depends on CS1.4 and CS1.6. Review: single-axis, Safety first.

- **CS2.9 — The workspace registry** *(gwz-core; < 300 lines)*.
  - Files: new `src/session_host/registry.rs`.
  - Implements §5.1's registry, a member of the host context (§5.6). It records each workspace on which any session sharing the host context runs a W operation or a push, and each detached worker. A W or a push on a recorded workspace waits in its session's queue and stays cancellable. A refusal names the lock's possible holders.
  - Test-first: §15.4 (two sessions sharing a host context submit W operations on one workspace, and each completes in turn, core half). Verdict-3 residual: check-and-record is one atomic step, so of two sessions racing a W on one workspace exactly one starts. Verdict-2 residual: a worker's registration is a guard its thread drops on exit or unwind, and that drop is the "worker has ended" signal.
  - Depends on CS2.3. Review: single-axis, Safety first.

- **CS2.10 — Parked log reads** *(gwz-core; < 400 lines)*.
  - Files: `src/diff/log_service.rs`; `src/operation/commit_log/handler.rs` (`CommitLogOutputRegistry`).
  - Implements §4.2's read verb, with a wait of at most 30 seconds and at most 1 MiB per read, and §5.1's parked reads. Today `DiffLog` blocks one thread per held read on a condvar and degrades bounded waits to probes. A held read becomes a parked waiter, completed by the next gated append, seal or close, by the read timer, or by session end (Verdict-3 residual).
  - Test-first: §15.10 (1024 reads parked on idle logs, plus a cancel and a close, complete with no host thread blocked and no reply lost); a read with a 1-second wait returns an empty batch after 1 second, not at once.
  - Depends on CS2.2. Review: single-axis, Safety first.

- **CS2.11 — Session-owned diff and log outputs** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/logs.rs`.
  - Implements §4.2's end-stream verb and §5.4's `diff.output` and `log.output` logs: members of the session context; at most 64 open; the four ways a log is released; the make-room release; `operation_expired` after release; `operation_not_found` for another session. Also §5.6: the registry writes each spool, and a producer never holds a spool's handle. On paths 1 and 2 of §2.1 the make-room rule is RemPlan-2's: the oldest log sealed or closed at least 30 seconds earlier, with no reader stream open, and `operation_expired` after any release.
  - Test-first: §15.7 (the open-log limit; session end leaves the spool directory empty; another session's read gets `operation_not_found`; 65 sequential diffs read to EOF; 64 streams left open refuse the next open until they end; 65 byte-format diffs never read all succeed; a log with an open reader is never released to make room; a read after release gets `operation_expired`). RemPlan-2's closure tests: 64 closed, unread logs, and the next open releases one; a log sealed within the last second is kept and its first read succeeds; a read after each way of release gets `operation_expired`.
  - Depends on CS2.10 and CS1.4. Review: single-axis, Safety first.

- **CS2.12 — Closure** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/host/close.rs`; `docs/Embedding.md` and `docs/OperationModel.md`, which now describe the session host.
  - Implements §8: `session.close` steps 1–9; channel closure without close, steps 2–5 and 7; the close bound; revocation; the detached marker (§6); detached workers recorded in the registry; the close report; repeated close. Also O4 and O5.
  - Test-first, §15.9: with a latched handler, close answers within the bound with `pending_local_work == 1` and unconfirmed cleanup; after the latch is released, events and the terminal are ignored, the next lock request fails with `Cancelled`, and session state is unchanged; the detached operation's result carries the marker; a detached log producer writes nothing to its spool and leaves no registry entry; after a close with a latched W, a new session sharing the host context queues its W until the latch is released, while one with another host context gets a refusal naming the lock's possible holders; repeated close returns the same report; dropping the channel performs the same shutdown.
  - Also test-first: §15.2 (a queued operation cancelled by close reads `errors[0].code == cancelled`). Verdict-3 residual: a session's live records survive its close as detached records. Consistency-2 residual: an admission thread stuck in resolution at the bound is not counted in the report, since resolution has no effect, and it starts nothing when it returns.
  - Depends on CS2.7, CS2.9 and CS2.11. Review: single-axis, Safety first.

- **CS2.13 — Dispatch routes: reads and settings** *(gwz-core; < 300 lines moved, < 150 new)*.
  - Files: new `src/session_host/dispatch/read.rs`, moved from the direct-method half of gwz-py's `native/src/dispatch/read.rs`; the open-merge pre-gate in `dispatch/mod.rs`.
  - Implements §5.2 for `status`, `ls`, `resolve_forall_targets`, `list_snapshots`, `transport_capabilities` and `configure_transport_runtime`, whose process-wide behaviour stays until CS3.8. The pre-gate moves out of gwz-cli's driver into core, so it applies to every driver (D4).
  - Test-first: each route's encoded response equals gwz-py's native dispatch on the same fixture workspace; each method's pre-gate outcome equals gwz-cli's today, and the outcomes that change for Python are listed for the Phase 4 Surface review; §15.4 (`status` and `transport_capabilities` answer while eight long operations run).
  - Depends on CS1.6 for the code and parity tests, and on CS2.4 for the rows that run through the host. Review: single-axis, Consistency first.

- **CS2.14 — Dispatch routes: workspace and repository lifecycle** *(gwz-core; < 450 lines moved)*.
  - Files: new `src/session_host/dispatch/workspace.rs`, moved from gwz-py's `native/src/dispatch/read.rs` (its W methods), `materialize.rs` and `local_family.rs`.
  - Implements §5.2 for `remote_identity`, `create_workspace`, `init_from_sources`, `clone_workspace`, `add_existing_repo`, `create_repo`, `repo_sync`, `clone_repo_member`, `detach_repo_member`, `attach_repo_member`, `materialize`, `clone_local_workspace` and `local_family`, with events through the gate. In a transport build the network-scoped ones stay refused until CS3.10.
  - Test-first: parity with gwz-py's native dispatch on fixtures; §15.4 (`remote_identity get`, submitted while a `materialize` runs, waits and then answers; `remote_identity set` queues behind a running W; no W fails with "already held" because of a `remote_identity` call).
  - Depends on CS1.6, and on CS2.4 for the rows that run through the host. Review: single-axis, Consistency first.

- **CS2.15 — Dispatch routes: Git mutations and merge** *(gwz-core; < 450 lines moved)*.
  - Files: new `src/session_host/dispatch/mutation.rs`, moved from gwz-py's `native/src/dispatch/git_mutation.rs`, `branch_stash.rs` and `merge.rs`, and the `snapshot`, `tag` and `capture` routes of `materialize.rs`.
  - Implements §5.2 for `snapshot`, `tag`, `capture`, `commit`, `stage`, `pull_head`, `pull_snapshot`, `push`, `fetch`, `stash`, `branch` and `merge`, with `merge`'s response view for `operation.response`.
  - Test-first: parity on fixtures; `merge`'s response is readable through `operation.response` after its result; §15.3, core half (a member-scoped model error and a merge-record error through the dispatch carry `member_id`, `target_kind`, `record_context` and `response_meta.request_id` equal to today's in-process values).
  - Depends on CS1.6, and on CS2.4 for the rows that run through the host. Review: single-axis, Consistency first.

- **CS2.16 — Dispatch routes: diff and log** *(gwz-core; < 250 lines)*.
  - Files: new `src/session_host/dispatch/output.rs`, moved from gwz-py's `native/src/dispatch/diff.rs` and `log.rs`.
  - Implements §5.2, whose dispatch absorbs the diff and log paths, and §4.2: `diff` and `log` are direct calls whose producers write session logs through the gate. As `handle_diff` does today, a producer runs to completion before its call replies.
  - Test-first: parity with gwz-py's native diff and log reads on fixtures; a `diff` read to EOF yields the bytes gwz-cli's `diff_exec` yields today.
  - Depends on CS2.11 and CS1.6. Review: single-axis, Consistency first.

**Exit.** Every §15 row assigned to Phase 2 passes in gwz-core CI on macOS, Linux and Windows. The settled tuple is recorded in the program checkpoint (L1-17, L1-31). Review: single-axis, Safety first, on the settled tree, attacking the interleavings of admission, cancellation, release and closure across CS2.2–CS2.12. Users see no change.

### Phase 3 — Network operations and session isolation (milestone: network operations run in the host, each on its own runtime built from the session's environment and timeouts)

The session path reads no environment variable and no process-global mutable state, except `permanent` entries and the legacy adapter's `debt` entries. The transport route is proven on the three 1.1.0 platforms, and the cost of a runtime per operation is measured.

CS3.1–CS3.6 need only Phase 1, so lane C can run beside Phase 2.

- **CS3.1 — The legacy adapter** *(gwz-core; < 200 lines)*.
  - Files: new `src/session_host/legacy.rs`; its callers `src/transport_host/local_command.rs` and 1.1.0 S6.1's token-taking entry.
  - Implements the transition rule. Until its driver moves (CS4.7, CS6.4), a legacy entry builds a per-call context equal to today's process state: the live environment, read at the entry; one process-wide legacy host context; the process-wide timeout. Nothing on the session path uses it. Its `debt` entries replace the module statics that CS3.5 and CS3.6 remove, so the allowlist shrinks, and CS6.5 removes them.
  - Test-first: characterization (L1-03): gwz-cli's and gwz-py's suites pass unchanged through the adapter.
  - Depends on CS1.4, CS1.5 and CS1.6. Review: single-axis, Consistency first.

- **CS3.2 — Endpoint configuration from the snapshot** *(gwz-core; < 450 lines)*.
  - Files: `src/transport_host/mod.rs` (`SshEndpointConfig::from_environment`), `src/transport_host/local_command.rs` (`environment_config`, `tls_config`), `src/git/gitbackend/transport_binding.rs` (the runtime factory's `HOME` and `SSH_AUTH_SOCK`), `src/git/gitbackend/transport_support/identity.rs` (`~/` identities), `crates/repo-inspect/src/environment.rs` (`Environment::Process`).
  - Implements §5.6: each network operation derives its endpoint configuration from the snapshot (SSH home and agent socket, TLS roots, proxies, the `gh` environment); an invalid proxy or CA setting refuses only network operations; a CA file is read when each network operation derives its configuration. Also §5.7's environment-read rows and §5.8's scoping of libgit2's own reads.
  - Test-first: §15.8 (after `open`, changing `HOME`, `SSH_AUTH_SOCK` or `GIT_SSL_CAINFO` leaves a later operation's endpoint configuration equal to the captured one, on an `ssh://` or `https://` remote, with the documented exception asserted for an `http://` remote, per RemPlan-2's Consistency P3-29; with an invalid proxy in the snapshot, local-only operations succeed and a network operation is refused; the error carries no environment value). The five environment-read `debt` entries leave the allowlist or move into the legacy adapter (§5.4).
  - Depends on CS3.1. Review: single-axis, Safety first.

- **CS3.3 — Session-path child processes** *(gwz-core; < 300 lines)*.
  - Files: `src/git/gitbackend/refs.rs`, `repository.rs` and `transport.rs`; `src/operation/commit_log/mod.rs`.
  - Implements §5.6's child processes and §5.7's spawn row: `git tag` and `git tag -d`, `git commit`, the local-import `git fetch` fallback and the commit-log `git rev-list` are spawned with `env_clear()` plus the snapshot.
  - Test-first: §15.8 (on POSIX, a value containing a byte that is not valid UTF-8 reaches a session-path child unchanged; after `open`, changing `GIT_CONFIG_GLOBAL` leaves a later commit's committer identity unchanged). The four spawn entries flip from `debt` to `permanent`, each citing its test.
  - Depends on CS3.1. Review: single-axis, Safety first.

- **CS3.4 — Core's own `git credential fill`** *(gwz-core; < 450 lines)*.
  - Files: `src/git/gitbackend/transport_support.rs` (the credential callback); new `src/git/gitbackend/credential_fill.rs`; `scripts/checks/check_process_globals.py` and its tests.
  - Implements §5.8's credential-helper rule and §5.7's `Cred::credential_helper` row. On paths 1 and 2 of §2.1 the mechanism is RemPlan-2's (Safety P3-29): `env_clear()` plus the snapshot without `GIT_ASKPASS` and `SSH_ASKPASS`, with `GIT_TERMINAL_PROMPT=0` and `-c credential.interactive=false`; killed on drop, bounded like the `gh` helper, and killed when the token is cancelled or the gate revoked; output treated as secret, with only `username` and `password` read, other lines tolerated and never logged; `approve` and `reject` never called. A missing `git` executable reads `external_tool_missing`.
  - Test-first: §15.8 (in both builds, the helper a later HTTP fetch runs is the one the snapshot's configuration names, and it sees the snapshot, even after `HOME` changes; a new `Cred::credential_helper` call fails gwz-core's check). RemPlan-2's closure tests: with `GIT_ASKPASS` naming a recorder and no helper configured, an HTTPS fetch fails with an authentication error and the recorder never runs; a helper that sleeps is killed when the operation is cancelled; a fault-injected `CredentialHelper::new(url).execute()` is flagged as `Cred::credential_helper` (Safety P3-31). The `debt` entry goes, and the new `git` spawn is listed `permanent`.
  - Depends on CS3.1 and CS1.5. Review: dual. It changes secret handling, and it changes credential lookup for both existing drivers.

- **CS3.5 — The SSH setup supervisor in the host context** *(gwz-core; < 450 lines)*.
  - Files: `src/git/endpoint/agent_job.rs` (769 lines: split first if the change would pass 1,000).
  - Implements §5.6's host-context member: the SSH setup supervisor with its helper and cleanup budgets (`HUB`, `INIT`, `COUNT`, `CLEANUPS`), stopped when its host context drops and its jobs have finished. Also §5.7's supervisor row and §14's change from per-process to per-host-context scope.
  - Test-first: §15.9, core half (two sessions sharing a host context share one supervisor and one helper budget; two host contexts have two); the supervisor thread ends after its host context drops. The four `debt` entries go.
  - Depends on CS3.1 and CS1.4. Review: single-axis, Safety first.

- **CS3.6 — HTTPS helper slots and `AuthOwner` cleanup** *(gwz-core; < 400 lines)*.
  - Files: `src/git/endpoint/https_auth.rs` (934 lines: split first, in a movement-only commit).
  - Implements §5.6's last paragraph and §5.7's HTTPS row: the slot budget (`SLOTS`) moves into the host context; the endpoint's `AuthOwner` owns helper cleanup, replacing `ORPHANS` and `ORPHAN_REAPING`.
  - Test-first: two sessions sharing a host context share its 8 HTTPS helper slots; an orphaned helper is reaped by its owner, with no process-wide registry. The three `debt` entries go.
  - Depends on CS3.1 and CS1.4. Review: single-axis, Safety first.

- **CS3.7 — The session transport entry** *(gwz-core; < 400 lines)*.
  - Files: new `src/transport_host/session_entry.rs` (`local_command.rs` stays the legacy entry); `src/transport_host/request.rs`.
  - Implements §5.2's transport entry. Through its gate, a worker builds the operation's own runtime from the session's endpoint environment and timeouts, registers the single request with the token through 1.1.0 S6.1's constructor, runs the handler, finishes and shuts down. Registration fails with `Cancelled` when the token is already cancelled; the token is the only cancellation authority. Finish and shutdown run under their own `catch_unwind`, and the entry never finishes from `Drop` while unwinding, as today's `Command` in `local_command.rs` does. Also §5.3 for network I/O and §16's core API changes. The entry's arms sit in `cfg_if` blocks for unix and windows; whichever of this step and any 1.1.0 cfg-site change lands second adapts, checked by CS1.7 and on dabeest.
  - Test-first: §15.5 (an operation blocked opening a stream to a latch-held endpoint is cancelled through its token, and the cancel returns within the admission deadline with a cleanup report while the handler's I/O fails with `Cancelled`; a token cancelled while the runtime is being built fails registration, runs no handler and settles `Cancelled`); §15.6 (a fault-injected panic in finish after a handler panic yields `Failed`, and the next call succeeds with the process alive).
  - Depends on CS3.2, CS1.4 and 1.1.0 S6.1. Review: single-axis, Safety first.

- **CS3.8 — Session timeouts** *(gwz-core; < 300 lines)*.
  - Files: `src/session_host/context.rs` (the timeouts), `src/session_host/dispatch/read.rs` (`configure_transport_runtime`).
  - Implements §5.5, §5.6's timeouts paragraph, and §4.2's per-session `configure_transport_runtime` in transport builds: it applies to operations admitted after its reply and never touches another session or a running operation. Ordinary builds keep today's process-wide behaviour, and `TIMEOUT_STATE` stays `permanent` (§5.8).
  - Test-first: §15.8 (two sessions: A sets a zero timeout, and B's next operation keeps B's own deadline). The 1.1.0 amendment round's Safety P3-6, as handed to this plan: a session opened without `configure_transport_runtime` carries the accepted clocks, the 9-second stall and 30-second aggregate the CLI uses at the 1.1.0 tag, asserted on the session context (D8); 1.1.0 S3.3's production-graph stall regression, one idle stage expiring with reason `stall` while the aggregate is ahead, passes through the session entry.
  - Depends on CS3.7. Review: single-axis, Consistency first.

- **CS3.9 — Member locks and push serialization** *(gwz-core; < 400 lines)*.
  - Files: `src/operation/membermutationguard.rs`, `src/workspace_ops/handle_fetch.rs`, `src/workspace_ops/push_member.rs`, `src/session_host/host/admission.rs`.
  - Implements §5.1's serialization and §16's handler change. The member lock manager moves into the host context; it blocks instead of refusing (today's `MemberLockManager::try_lock` refuses), recovers from poisoning, and wakes a wait on cancellation, since the wait is a gate crossing. Each fetch or push member step takes it. A push also takes the workspace mutator lock, and at most one push runs per workspace.
  - Test-first: §15.4 (two pushes on one workspace in one session complete one after the other with no `UnsupportedOperation`, while a fetch and a push run concurrently and so do two fetches; two fetches touching one member serialize that member's step); §15.9 (after revocation the next member-lock request fails with `Cancelled`, member half).
  - Depends on CS2.8, CS2.9 and CS1.4. Review: single-axis, Safety first.

- **CS3.10 — Network operations through the host** *(gwz-core; < 300 lines)*.
  - Files: `src/session_host/host/worker.rs`, `src/session_host/dispatch/read.rs`.
  - Implements §5.2's choice of entry: transport-scope requests use the session entry, and Phase 2's refusal goes. Otherwise the handler runs with the default backend. Also §5.8's ordinary-build behaviour, where a cancel waits for completion, and §13's `cancellation` field in `transport_capabilities`.
  - Test-first: §15.4 (eight overlapping fetch and push operations complete independently, and a ninth waits in the queue and then runs, core half); §15.5 (a cancelled running fetch shows `failed` with `errors[0].code == cancelled`, core half); §15.15, in the build configuration D7 settles.
  - Depends on the Phase 2 exit and CS3.5–CS3.9. Review: single-axis, Safety first.

- **CS3.11 — Route proof and runtime cost** *(evidence; < 300 lines of fixtures)*.
  - Files: gwz-core test fixtures; raw runs in gwz-core-evidence, with a redacted public report.
  - Implements the route evidence for the session entry. On macOS ARM64, Linux x86-64 and dabeest, SSH and HTTPS clone and fetch run through the session host against the 1.1.0 disposable fixtures. A typed assertion shows the transport route: `transport_capabilities.cancellation` and the transport observation the result carries. A `gh` failure and an unsupported proxy still refuse. It also measures runtime construction cost and connection counts for 1, 2 and 8 overlapping operations (§16: up to 8 × 32 connections to one host).
  - Test-first: a build with the session entry's Windows arm removed fails the route test instead of passing natively. This closes the 1.1.0 amendment round's A3 for the session entry and its Safety P3-4.
  - Depends on CS3.10. Review: part of the Phase 3 exit.

**Exit.** Every §15 row assigned to Phase 3 passes: in CI on macOS, Linux and Windows for rows without a network fixture, and on macOS, Linux and dabeest for transport rows. gwz-core's allowlist holds no `debt` entry outside the legacy adapter. Review: dual, because CS3.4 and the `cancellation` field change what existing CLI and Python users see.

### Phase 4 — gwz-py on the session host (milestone: `NativeCoreBridge` is the thin client of §10 and the public API is unchanged)

gwz-py's suite passes through the session on CPython 3.10–3.13 on macOS, Linux and Windows, and 1.1.0 S6.3's overlapping-operation and route tests pass through it on the three platforms. gwz-py's process-global debt is gone.

CS4.1 and CS4.3–CS4.6 work against a fake channel, so lane D can start after Phase 1.

- **CS4.1 — Assertions that change** *(gwz-py; 0 product lines)*.
  - Implements §12's requirement that this plan list the assertions that change. The list below was taken at gwz-py `4ad2b077`. 1.1.0 S6.2 and S6.3 rewrite the transport-session tests, so this step retakes the list against the 1.1.0 tag and files it before any test changes. Known changes:
    1. A cancel naming a foreign or unknown operation: `InvalidRequest` becomes `operation_not_found` (`test_cancel_uses_public_operation_id_and_rejects_foreign_identity`). The contract names this one.
    2. A cancel naming an expired or older completed operation: `InvalidRequest` becomes `operation_expired`, and the "latest completed cancellation snapshot" becomes each terminal record's retained report (GwzPyTransportDesign §4, superseded by the contract's §14).
    3. Capacity refusals read `transport_session_full` (§5.1) instead of the 1.1.0 limiter's code (`test_typed_capacity_refusal_reaches_python_without_message_parsing`, or its 1.1.0 successor).
    4. The host assigns operation IDs at receipt (§4.3), and `reserve_operation` leaves the bridge's path (§9). Identity tests (`test_public_identity_is_issued_once_before_native_work` and the identity tests in `test_transport_session_native.py`) instead assert that the accepted response's `operation_id` is the one events and results carry.
    5. A cancel before the worker starts becomes a cancel by `call_id` before the accepted reply (§15.5; `test_cancel_before_default_executor_starts_worker_reaches_reserved_operation`).
    6. The serialization tests (`test_native_bridge_does_not_serialize_independent_network_calls`, `test_legacy_module_keeps_its_network_call_serialization`), and GwzPyTransportDesign §5's physical-session-reuse, construction-barrier, admission-barrier, overlapping-direct-native-call and Python-lock tests, are replaced by §15.4's rows.
    7. Event waits (`test_native_bridge_uses_wait_events_when_available`) become `events.subscribe` reads against a fake channel.
    8. `test_result_wait_releases_the_gil_for_a_failing_worker` becomes: `recv()` releases the GIL, and a failing worker, now a core thread, needs no GIL.
    9. An undeclared method (`test_native_bridge_routes_unsupported_methods_explicitly`) raises a `GwzBridgeError` with the code C4 settles, instead of the protocol error "unsupported gwz-core method".
    10. Tests that call the module-level native functions (`call`, `submit`, `wait_events`, `operation_result`, `try_operation_result`, `merge_operation_response`, `diff_log_read`, `log_output_read` and the rest) move to the bridge, or go with those functions (D6).
    11. `test_process_globals.py` expects an allowlist with no `debt` entry (CS4.8).
  - Unchanged in meaning, with a new mechanism: close and repeated close return the retained report; repeated task cancellation waits for cleanup; the caller's directory travels in each request (`test_native_submit_keeps_serialized_caller_context_after_cwd_changes`); an unknown operation reads `operation_not_found`; 1.1.0 S6.3's tests.
  - Depends on G2 (the 1.1.0 tag). Review: with CS4.6.

- **CS4.2 — The extension's channel** *(gwz-py; < 350 lines)*.
  - Files: new `native/src/session.rs`; `native/src/lib.rs`.
  - Implements §9: the `HostContext()` constructor; `open(options)` with limits, the snapshot as byte-string pairs, and the host context; a non-blocking `send` that fails with `transport_session_full` on a full queue and fails once the session has ended; a `recv` that blocks with the GIL released and returns `None` once the session has ended; `close()` and drop close the channel. Nothing else crosses, and the extension reads neither the environment nor the working directory.
  - Test-first: §15.10 (`send` on a full queue raises `transport_session_full` and never blocks or drops, Python half); another Python thread runs while `recv()` blocks. The step adds no allowlist entry.
  - Depends on CS1.2 and CS1.4; its integration tests need CS2.4 and CS2.13. Review: single-axis, Safety first.

- **CS4.3 — The bridge's call table and pump** *(gwz-py; < 450 lines)*.
  - Files: `src/gwz/bridge.py`, and a new private module for the pump and the call table (`bridge.py` is 643 lines).
  - Implements §10's Calls, The pump, and Waiting and bounds:
    - call ID allocation, waiter registration and `send` under one lock;
    - one daemon pump thread per session, completing each reply on its issuing loop with `call_soon_threadsafe`;
    - a reply for a closed loop is dropped, except a result or response reply, which the bridge keeps in a view bounded by the operation-table size, oldest dropped first;
    - on paths 1 and 2 of §2.1, a kept result leaves the view on release, or when `operation.release` reports `operation_expired` (RemPlan-2's Safety P3-30), not when a late cancel does;
    - an undecodable payload fails only its call; `recv()` returning `None` fails every outstanding call with the closed-session error; any pump exception closes the channel, marks the bridge closed and fails every outstanding call;
    - two thread-safe counters, for ordinary calls and control calls, with waits on the caller's own loop.
  - Test-first: §15.11 (an injected undecodable frame fails every outstanding call and later sends; one undecodable payload fails only its call); §15.12 (calls from different loops complete on their own loops; with the issuing loop closed before its result reply, `operation_result` from another loop returns the result; 1000 cancelled result waits with no release leave at most the table size in the view); §15.1 (a call whose client stops waiting still completes and its reply is dropped, except a result or response reply, per RemPlan-2's Consistency P3-28); RemPlan-2's Safety P3-30 test (after a cancelled result wait, the host's eviction and a `cancel_operation` reporting `operation_expired`, `operation_result` still returns the result).
  - Depends on CS1.1 and CS1.2. Review: single-axis, Safety first.

- **CS4.4 — Bridge methods and error mapping** *(gwz-py; < 400 lines)*.
  - Files: `src/gwz/bridge.py`, `src/gwz/errors.py`.
  - Implements §10's mapping table and its error rule: a `SessionError` becomes a `GwzBridgeError` with code, member ID, member path, target kind, detail and record context from `GwzError`, `machine_message` from `GwzError.message`, and `response_meta` from `ResponseMeta`. Every request carries the caller's directory, as it already does.
  - Test-first: §15.2 (each code arrives as the same member in Python; a queue-full refusal reads `transport_session_full`); §15.3 through `NativeCoreBridge`.
  - Depends on CS4.3. Review: single-axis, Consistency first.

- **CS4.5 — Task cancellation, exit, host context and environment** *(gwz-py; < 400 lines)*.
  - Files: `src/gwz/bridge.py` and the pump module; `src/gwz/client.py` (stream helpers only).
  - Implements:
    - §10's task cancellation: cancelling a unary call sends `operation.cancel` for its `call_id`, waits shielded for the reply, then propagates, and `operation_expired` raises nothing; cancelling a read stops only that read; the Client's stream helpers deliver the result or release the record in `finally`;
    - interpreter exit: a finalizer closes the channel and joins the pump before finalization, waiting at most the close bound;
    - the per-process host context, created once under a process-wide lock at the first open; `NativeCoreBridge` gains an optional host context to share, the one Python-visible addition, and `Client` is unchanged;
    - the environment captured at `open`: `os.environb` on POSIX, and `os.environ` encoded as WTF-8 on Windows.
  - Test-first: §15.12 (a cancelled unary call cancels and joins); §15.2 (a cancel racing its unary reply raises nothing); §15.7 (a cancelled result wait followed by a retry returns the result; 200 streams abandoned early do not exhaust admission); §15.9 (32 Clients opened at once from 32 threads share one host context); §15.10 (1024 outstanding unary calls whose tasks are all cancelled: every cancel is delivered and answered); §15.11 (a script that exits without close exits 0 without aborting on CPython 3.10–3.13, on Linux, macOS and, beyond the contract, Windows, C6). Verdict-3 residuals: on Windows, a value with an unpaired surrogate reaches a session-path child unchanged; after `fork` the bridge drops its default host context in the child, through `os.register_at_fork`, so a Client opened there creates its own (Linux and macOS).
  - Depends on CS4.3 and CS4.4. Review: single-axis, Safety first.

- **CS4.6 — Bridge-internal tests against a fake channel** *(gwz-py; test code < 500 lines)*.
  - Files: `src/tests/test_bridge_transport.py`, `test_native_bridge.py`, `test_native_result_wait.py` and `test_native_operations.py`, and the 1.1.0 successors of `test_transport_session_api.py` and `test_transport_session_native.py`.
  - Implements §12's rule that bridge-internal tests run against a fake channel, with CS4.1's assertion changes.
  - Test-first: each row of CS4.1's retaken list has a rewritten test.
  - Depends on CS4.1 and CS4.3–CS4.5. Review: single-axis, Consistency first.

- **CS4.7 — Activation: `NativeCoreBridge` on the session host** *(gwz-py; < 200 lines)*.
  - Files: `src/gwz/bridge.py`; `src/gwz/client.py` (the default bridge only); the 1.1.0 per-Client limit of 8 network operations, which the session's running limit and queue replace.
  - Implements §10 with G11 held, and makes the supersessions of §14 for gwz-py take effect.
  - Test-first: §15.12 (gwz-py's Client-level and native-integration tests pass through `NativeCoreBridge`); §15.4 (eight overlapping fetch and push operations on one Client complete independently and a ninth queues; two Clients in one process run W operations on one workspace, and pushes on another, each pair in turn with no `UnsupportedOperation`); §15.5 (a cancelled queued submit and a cancelled running fetch show `failed` with `cancelled`; a queued unary call and a waiting direct call yield `SessionError{cancelled}`); §15.9 (two Clients in one process share the SSH helper budget); 1.1.0 S6.3's tests on macOS, Linux and dabeest.
  - Depends on CS4.2, CS4.6 and the Phase 3 exit. Review: the Phase 4 exit review.

- **CS4.8 — Legacy removal and gwz-py's debt** *(gwz-py; mostly deletions, < 300 lines added)*.
  - Files: `native/src/operations.rs`, `shims.rs`, `diff_logs.rs`, `log_outputs.rs` and `dispatch/`, removed or kept off the session path per D6; `native/src/lib.rs`; `scripts/process_globals_allowlist.json`; `dev-docs/GwzPyDesign.md` (the bridge-contract statements §14 replaces); `RELEASE.md`. The status edits to `GwzPyTransportDesign.md` belong to the 1.1.0 amendment, not this step.
  - Implements §9 (the legacy entry points leave the bridge's path) and §5.7's gwz-py rows: the diff and log registries, the legacy operation store and its thread-local scoping, the thread-local backend and operation ID, and the `GWZ_PY_TEST_EVENT_DELAY_MS` hook, which moves behind `cfg(test)`.
  - Test-first: §15.8 (`check_process_globals.py` passes over gwz-py with no `debt` entry). The release notes state the behaviour changes: the cancel codes, `transport_session_full` and `operation_expired`, and an interpreter exit that waits up to the close bound.
  - Depends on CS4.7. Review: the Phase 4 exit review.

**Exit.** Review: dual plus Surface, on the settled tree. Surface reads the Python API, its docstrings, gwz-py's `gwz` console script help and the release notes, and the pre-gate outcomes CS2.13 listed.

### Phase 5 — The wire proof (milestone: CI runs gwz-py's tests through both bridges, and any difference is a defect)

- **CS5.1 — The host binary** *(gwz-core; < 200 lines)*.
  - Files: new `examples/session_host.rs`.
  - Implements §12's host binary: it serves one session over stdin and stdout through the byte-stream adapter, captures its own environment once at start, and creates one host context. gwz-core's `Cargo.toml` `include` list already leaves `examples/` out of the published crate, so no crate installs it (G10).
  - Test-first: §15.9 (the byte-stream host closes its channel); `cargo package --list` contains no example (the 1.1.0 amendment round's residual on the example binary).
  - Depends on CS1.3 and CS2.4. Review: single-axis, Consistency first.

- **CS5.2 — `StreamCoreBridge`** *(gwz-py tests; < 350 lines)*.
  - Files: new `src/tests/stream_bridge.py`, a test bridge that is not packaged.
  - Implements §12's test bridge: CS4.3's call table over asyncio subprocess pipes, with an asyncio reader task in place of the pump thread. It starts the binary with the snapshot the native bridge would pass to `open`.
  - Test-first: §15.1 (a duplicate outstanding call ID, and a lower one, each end the session, and the original call fails with the closed-session error, never `invalid_request`).
  - Depends on CS4.3 and CS5.1. Review: single-axis, Consistency first.

- **CS5.3 — Two-bridge CI and the evidence rule** *(gwz-py; < 200 lines)*.
  - Files: `.github/workflows/package-smoke.yml`, which builds the example from the checked-out gwz-core; `.github/workflows/publish.yml`; `run_tests.py`, for bridge selection and the marker naming the Client-level and native-integration tests.
  - Implements §12's CI on macOS, Linux and Windows.
  - Test-first: §15.13 (the same tests pass through `StreamCoreBridge`), which also gives the stream half of every §15 row that says "on both bridges". The 1.1.0 amendment round's Safety P3-3: the release-evidence run is the one against the registry-pinned gwz-core at the release tag, with the host binary built from the same tag; path-pinned runs are development evidence; retained evidence, including fixture logs and the host binary's stderr, passes the secret scan the 1.1.0 plan uses, and its record names the gwz-core version and the binary's source revision. Its Safety P3-6: 1.1.0 S3.3's stall regression passes on both bridges.
  - Depends on CS4.7 and CS5.2. Review: the Phase 5 exit review.

**Exit.** Review: single-axis, Safety first, on evidence, secrets and the identity of release evidence. Users see no change.

### Phase 6 — gwz-cli on the session host (milestone: the CLI sends every protocol request through an in-process session, and what users see is unchanged)

After this phase the CLI's copy of the dispatch and the O7 legacy exceptions are gone, and O9 holds: no allowlist holds a `debt` entry on the session path. Lane F can start after the Phase 2 exit.

- **CS6.1 — The CLI's session driver** *(gwz-cli; < 450 lines)*.
  - Files: new `src/session_driver.rs`; `src/lib.rs`.
  - Implements §11: one host context created at startup; an in-process session opened with it and the environment captured at startup; a submit followed by event reads whenever the renderer consumes events (JSONL output, and the progress line in human mode on a terminal), and a unary call otherwise; the final response read with `operation.response`, which §4.2 generalizes to every operation method; replies rendered as today. The main thread blocks on `recv()`.
  - Test-first: gwz-cli's local-command tests pass through the driver; on fixtures, the JSONL events read through `events.subscribe` equal today's sink output.
  - Depends on the Phase 2 exit and CS1.1. Review: single-axis, Consistency first.

- **CS6.2 — Diff, log and hook paths** *(gwz-cli; < 450 lines)*.
  - Files: `src/diff_exec.rs`, `src/log_exec.rs`, `src/hook/family.rs`.
  - Implements §5.2, whose dispatch absorbs the CLI's diff, log and hook paths, and §4.2's log reads and end stream. The hook's family verbs (`local_family`, `ls`, `clone_local_workspace`) go through the session; the hook's own filesystem probes stay in the CLI.
  - Test-first: gwz-cli's diff, log and hook tests pass through the session; a pager quit ends the reader stream and the exit codes are unchanged.
  - Depends on CS6.1 and CS2.16. Review: single-axis, Consistency first.

- **CS6.3 — CLI-local exceptions** *(gwz-cli; < 250 lines)*.
  - Files: `src/forall.rs`, `src/globalargs/dispatch.rs` (`UpdateBootstrap`), `src/hook/setup.rs`.
  - Implements D3's disposition of the requests with no protocol method (C2). On the recommended option, `forall` stays CLI-local, as GWZDesign's CLI driver design already says: it resolves members through `resolve_forall_targets` and keeps its DR-3 guard by taking the cross-process workspace lock itself, so the session host sees another process, as today. `claude-code setup` stays CLI-local. If D3 gives `init --update` a protocol method, a contract amendment and a schema step, reviewed dual like CS1.1, come first.
  - Test-first: `forall`'s dry-run and real-run tests, and `init --update`'s tests, pass unchanged.
  - Depends on CS6.1 and D3. Review: single-axis, Safety first.

- **CS6.4 — Activation: every request through the session** *(gwz-cli; < 400 lines, deletions excluded)*.
  - Files: `src/lib.rs`; `src/globalargs/dispatch.rs` loses `execute_invocation`, `execute_with_backend` and `transport_meta`; its callers in `src/tests/m2c.rs`, `g12.rs`, `g13.rs` and `g01/commands.rs` are re-pointed at the session driver.
  - Implements §11 and completes the dispatch move: the 1.1.0 amendment round's A2, as handed to this plan. The pending-cleanup notice comes from the close report.
  - Test-first: §15.14 (the suite passes through the in-process session; a pseudo-terminal test, a new harness on macOS and Linux, shows the progress line); the CLI reference check passes with no change to help, and the machine-output fixtures are unchanged. Route evidence for the CLI entry: SSH and HTTPS clone and fetch through the session on macOS, Linux and dabeest, with the typed route assertion of CS3.11 (A3 for the CLI entry).
  - Depends on CS6.2, CS6.3 and the Phase 3 exit. Review: the Phase 6 exit review.

- **CS6.5 — Legacy removal and the CLI boundary** *(gwz-core and gwz-cli; < 250 lines added)*.
  - Files: gwz-core `src/transport_host/local_command.rs` (the legacy `with_local_transport`, once no caller remains), `src/transport_host/request.rs` (`cancellation_handle()` and `TransportCancellation`), `src/session_host/legacy.rs` with its allowlist entries; gwz-cli, a new source test.
  - Implements §16's removal of O7's two legacy exceptions, O9 in full, and §5.7's end state. The gwz-cli test fails on any direct call of a gwz-core handler outside the session driver and the named CLI-local exceptions (L2-06, G2).
  - Test-first: §15.8 (`check_process_globals.py` passes over gwz-core, gwz-transport and gwz-py with no `debt` entry on the session path); an injected direct handler call fails the gwz-cli test.
  - Depends on CS6.4 and CS4.8. If the operator ships Phase 6 before Phase 4 (D2), the CLI-only removals go here and the shared ones wait for CS4.8.
  - Review: the Phase 6 exit review.

**Exit.** Review: dual plus Surface, on the settled tree. Surface compares gwz-cli's help at every level and its human, JSON and JSONL output with the release before. Recording the contract as implemented, which Verdict-2 asked to wait for these gates, is then an operator decision.

## 4. Dependency sketch and parallel lanes

```text
G0 ── G1 (§7) ── Phase 1
1.1.0 S6.1 ──────────────────────────────────────────────── CS3.7
1.1.0 tags (D9) ─────────────────────────────────────────── first merge of product code

Phase 1
CS1.1 ──┬── CS1.2 ── CS1.3
        └──────────────────────── CS1.6
CS1.4 ──┬──────────────────────── CS1.6
CS1.5 ──┘   (CS1.4 and CS1.5 freeze together)
CS1.7       (independent)
CS1.8       (only if revision 4 has not applied B11)

Phase 2
A   CS2.1 ── CS2.2 ── CS2.3 ── CS2.4 ── CS2.6 ── CS2.7 ──┐
               │        └── CS2.9 ───────────────────────┼── CS2.12
               ├── CS2.5 ── CS2.6                        │
L              └── CS2.10 ── CS2.11 ─────────────────────┘
B   CS1.6 ──┬── CS2.8
            ├── CS2.13 ─┐
            ├── CS2.14 ─┼── (through-host rows after CS2.4)
            └── CS2.15 ─┘
    CS2.11 ── CS2.16
    all CS2.x ── Phase 2 exit

Phase 3
C   CS3.1 ──┬── CS3.2 ── CS3.7 ── CS3.8 ──┐
            ├── CS3.3                     │
            ├── CS3.4                     │
            ├── CS3.5 ────────────────────┤
            └── CS3.6 ────────────────────┤
    CS2.8 + CS2.9 ── CS3.9 ───────────────┤
    Phase 2 exit ─────────────────────────┴── CS3.10 ── CS3.11 ── Phase 3 exit

Phase 4
D   CS4.1 ──────────────────────────────────┐
    CS4.3 ── CS4.4 ── CS4.5 ── CS4.6 ───────┤
    CS2.4 + CS2.13 ── CS4.2 ────────────────┼── CS4.7 ── CS4.8 ── Phase 4 exit
    Phase 3 exit ───────────────────────────┘

Phase 5
E   CS1.3 + CS2.4 ── CS5.1 ──┐
    CS4.3 ────────── CS5.2 ──┴── CS5.3 (after CS4.7) ── Phase 5 exit

Phase 6
F   Phase 2 exit ── CS6.1 ── CS6.2 ── CS6.3 (D3) ── CS6.4 (after Phase 3 exit) ── CS6.5 (after CS4.8) ── Phase 6 exit
```

What can run at once:
- **After G0 and G1:** CS1.1, CS1.2's queues, CS1.4, CS1.5, CS1.7 and CS1.8. CS1.3 follows CS1.2; CS1.6 follows CS1.1 and CS1.4.
- **After the Phase 1 freezes**, per step dependencies:
  - lane A, the host: CS2.1 → CS2.2 → CS2.3 → CS2.4 → CS2.6 → CS2.7 → CS2.12, with CS2.5 after CS2.2 and CS2.9 after CS2.3;
  - lane L, the logs: CS2.10 → CS2.11;
  - lane B, the routes: CS2.8, CS2.13, CS2.14 and CS2.15 at once, and CS2.16 after CS2.11;
  - lane C, isolation: CS3.1, then CS3.2–CS3.6 at once, then CS3.7 → CS3.8; CS3.9 after CS2.8 and CS2.9;
  - lane D, the Python bridge: CS4.1 (after the 1.1.0 tag) and CS4.3 → CS4.4 → CS4.5 → CS4.6, all against a fake channel;
  - lane T, tooling: CS1.7 and CS1.8, if not already done.
- **After the Phase 2 exit:** CS3.10, CS4.2, CS5.1, and lane F's CS6.1 → CS6.2 → CS6.3.
- **Serial tail:** CS3.11 → Phase 3 exit → CS4.7 → CS4.8 → Phase 4 exit → CS5.3 → Phase 5 exit; CS6.4 after the Phase 3 exit; CS6.5 after CS4.8.

Files that more than one step names are ordered by the sketch: `dispatch/mod.rs` (CS1.6, CS2.13), `host/admission.rs` (CS2.3, CS3.9), `host/worker.rs` (CS2.4, CS3.10), `dispatch/read.rs` (CS2.13, CS3.8, CS3.10), `transport_host/local_command.rs` (CS3.1, CS3.2, CS6.5) and `src/gwz/bridge.py` (CS4.3–CS4.5, CS4.7). No other file is shared.

## 5. Carried findings mapped to steps

On path 1 of §2.1 the findings below are contract text, and each step implements and tests the corrected text. On paths 2 and 3, the findings marked "RemPlan-2 §2" are step obligations with the closure tests shown. On path 3, Verdict-2's findings are obligations too.

### 5.1 Verdict-2 (revision 2's acceptance)

| Finding | Steps | Closure test |
| --- | --- | --- |
| Consistency P3-23: one environment capture rule, byte-string pairs | CS1.5, CS4.5 | §15.8 (a non-UTF-8 byte reaches a child unchanged) |
| Consistency P3-24: held reads parked, never serviced on the reading thread | CS2.2, CS2.10 | §15.10 (1024 parked reads, a cancel and a close) |
| Consistency P3-25 and Safety P3-26: result replies kept, bounded | CS4.3 | §15.12 (closed loop keeps the result; 1000 waits leave at most the table size) |
| Consistency P3-26: "never received" gets `operation_not_found` | CS2.2, CS2.7 | §15.5 (call ID above the highest received) |
| Consistency P3-27: unpinned gwz-core checkout | superseded by revision 3's move; see B11 | — |
| Safety P3-23: W across sessions sharing a host context | CS2.9 | §15.4 (two Clients, W operations on one workspace) |
| Safety P3-24: libgit2's credential helper sees the live environment | CS3.4 | §15.8 (the helper named by the snapshot, even after `HOME` changes) |
| Safety P3-25: unread diffs exhaust the open-log limit | CS2.11 | §15.7 (65 unread byte-format diffs) |
| Safety P3-27: host-context creation discipline | CS4.5 | §15.9 (32 Clients from 32 threads) |
| Residual: eight direct methods | CS1.6, CS2.8 | no R handler takes the workspace lock, all eight |
| Residual: the cleanup report of a direct-call cancel | CS2.7 | `(0, false)` for any target that never touched the network (C3) |
| Residual: the signal that a detached worker has ended | CS2.9 | the registration guard drops on thread exit or unwind |
| Residual: zero or two cancel targets | CS1.1, CS2.7 | `invalid_request` |
| Residual: a detached worker that never returns keeps its lock | — | disclosed risk R6 |
| Residual: the `CleanupReport` name | CS1.1 | the Rust type is renamed |
| Residual: an admission thread stuck past the close bound | CS2.12 | not counted in the report; starts nothing when it returns |

### 5.2 Verdict-3 and RemPlan-2

| Finding | Steps | Closure test |
| --- | --- | --- |
| B11 (Safety P2-9, Consistency P3-31): the gwz-transport check runs in no CI | revision 4, or CS1.8 | RemPlan-2 §1's closure tests |
| RemPlan-2 §2, Consistency P3-28: §15.1's dropped reply | CS4.3 | §15.12's closed-loop result test; §15.1 for a cancelled unary call only |
| RemPlan-2 §2, Consistency P3-29: scope of libgit2's own reads | CS3.2 | §15.8 on an `ssh://` or `https://` remote; the exception asserted for `http://` |
| RemPlan-2 §2, Consistency P3-30, P3-32, P3-33 and Safety P3-28: the make-room release | CS2.11 | 64 closed unread logs release one; a log sealed within a second is kept; `operation_expired` after every release way |
| RemPlan-2 §2, Safety P3-29: how core runs `git credential fill` | CS3.4 | the `GIT_ASKPASS` recorder never runs; a sleeping helper is killed on cancel |
| RemPlan-2 §2, Safety P3-30: a late cancel must not drop the kept result | CS4.3 | the result survives a cancel that reports `operation_expired` |
| RemPlan-2 §2, Safety P3-31: the checker's spelling of the helper spawn | CS3.4 | `CredentialHelper::new(url).execute()` is flagged |
| Residual: atomic check-and-record in the registry | CS2.9 | two racing sessions, one start |
| Residual: live records survive a close as detached records | CS2.12 | §15.9 (a new session queues behind a latched W) |
| Residual: the registry does not record fetches | — | disclosed risk R7 |
| Residual: `fork` inherits the host context | CS4.5 | a Client opened after `fork` has its own host context |
| Residual: non-contiguous call IDs | CS2.2 | `operation_expired` for an unseen ID below the highest (C5) |
| Residual: no test for Windows WTF-8 capture | CS1.5, CS4.5 | an unpaired surrogate reaches a child unchanged on Windows |
| Residual: `DiffLog` parks a blocked thread per read | CS2.10 | §15.10 (1024 parked reads) |

### 5.3 The 1.1.0 amendment round

The amendment's first-round [verdict](../gwz-core/dev-docs/GwzV110PlanAmendment-Verdict.md) raised these findings against a draft that put the whole contract into 1.1.0. The operator's decision of 2026-09-26, recorded in the amendment's [remediation plan](../gwz-core/dev-docs/GwzV110PlanAmendment-RemPlan.md), kept 1.1.0 to the minimal unification. The accepted amendment resolves each finding for 1.1.0's own steps. Each also applies to the parts of the contract this plan builds, and the mapping below is for those parts. The accepted amendment's later rounds hand nothing else to this plan: their carried items go to 1.1.0's S1.1 revision.

| Finding | Steps | Closure test |
| --- | --- | --- |
| A2 (Consistency P2-2): no owner for the gwz-cli dispatch move | CS1.6, CS2.13–CS2.16, CS6.4 | `execute_invocation` and its test callers are gone; gwz-cli's boundary test (CS6.5) passes |
| A3 (Safety P2-1): no route proof after the entries move | CS1.7, CS3.11, CS6.4 | on dabeest, a session fetch reports the transport route; a build without the Windows arm fails that test |
| Consistency P3-4: owners of the core rows and the byte-stream adapter | CS1.3, Phases 2 and 3 for core rows, Phase 4 for gwz-py rows, CS5.1 for the host binary | each §15 row's step lives in the repository its test lives in (§5.5) |
| Safety P3-3: redaction and identity of release evidence | CS5.3 | the secret scan passes; the record names the core version and the binary's revision |
| Safety P3-4: cost and fan-out of a runtime per operation | CS3.11 | measurements for 1, 2 and 8 operations |
| Safety P3-6: the session variant's clocks | CS3.8, CS5.3 | default clocks on the session context; S3.3's stall regression on both bridges |
| Consistency P3-5 and Safety P3-2: the implementation plan's own gate | §7 | this plan's dual GO precedes CS1.1 |
| Residual: file sizes in `transport_host` | CS3.5, CS3.6, CS3.7 | new files under 500 lines; splits before growth |
| Residual: the example host binary is test-only | CS5.1 | `cargo package --list` has no example |

### 5.4 The §5.7 debt entries

gwz-core's allowlist has 30 entries at the planning tuple, 18 of them `debt`. gwz-py's has 8, all `debt`; 1.1.0 S6.2 removes `CURRENT_SESSION`. gwz-transport's has one `permanent` entry.

| Entry | Step |
| --- | --- |
| `agent_job.rs`: `HUB`, `INIT`, `COUNT`, `CLEANUPS` | CS3.5 |
| `https_auth.rs`: `ORPHANS`, `ORPHAN_REAPING`, `SLOTS` | CS3.6 |
| `transport_binding.rs` `env::var_os`; `identity.rs` `env::home_dir`; `repo-inspect` `env::var_os` | CS3.2 |
| `local_command.rs` `env::vars_os`; `transport_host/mod.rs` `env::var_os` | CS3.2 moves them into the legacy adapter; CS6.5 removes them |
| `Command::new("git")` in `refs.rs`, `repository.rs`, `transport.rs`, `commit_log/mod.rs` | CS3.3, flipped to `permanent` with tests |
| `transport_support.rs` `Cred::credential_helper` | CS3.4 |
| `V1_PRESERVATION_IMAGE_CAPTURES` | CS2.1 |
| gwz-py: `diff_logs.rs` and `log_outputs.rs` `REGISTRY`; `operations.rs` `STORE`, `SCOPED_STORE` and the `env::var` test hook; `shims.rs` `SCOPED_BACKEND` and `SCOPED_OPERATION_ID` | CS4.8 |
| the legacy adapter's own entries | CS3.1 adds them; CS6.5 removes them |

§5.7's row on gwz-py's working-directory reads is already closed at the planning tuple: gwz-py takes the caller's directory from each request, and its allowlist has no `current_dir` entry. `TIMEOUT_STATE`, libgit2's timeout options, the `gh` spawn and the ID counters stay `permanent`.

### 5.5 Contract §15 coverage

| §15 item | Steps |
| --- | --- |
| 1. One reply per call | CS2.2; CS4.3; CS5.2 |
| 2. Error codes | CS1.1; CS2.7, CS2.12; CS4.4, CS4.5; CS5.3 |
| 3. Structured errors | CS2.4, CS2.15; CS4.4; CS5.3 |
| 4. Concurrency and admission | CS1.6, CS2.3, CS2.8, CS2.9, CS2.13, CS2.14; CS3.9, CS3.10; CS4.7 |
| 5. Cancellation | CS2.2, CS2.7; CS3.7, CS3.10; CS4.7; CS5.3 |
| 6. Panics | CS2.4; CS3.7 |
| 7. Retention | CS2.5, CS2.6, CS2.11; CS4.5 |
| 8. Session context | CS1.5; CS3.2, CS3.3, CS3.4, CS3.8; CS4.8; CS6.5; CS1.8 where it applies |
| 9. Closure | CS1.3, CS2.8, CS2.12; CS3.5, CS3.9; CS4.5, CS4.7; CS5.1 |
| 10. Channel | CS1.2, CS2.2, CS2.10; CS4.2, CS4.5 |
| 11. Pump | CS4.3, CS4.5 |
| 12. Python mapping | CS4.3, CS4.5, CS4.7 |
| 13. Wire proof | CS5.3 |
| 14. gwz-cli | CS6.4 |
| 15. Ordinary builds | CS3.10, in the configuration D7 settles |

## 6. Risks and open decisions

### 6.1 Risks

- **R1. A moving baseline.** The 1.1.0 amendment is accepted, but its S1.1 revision of gwz-py's transport design is not yet written. 1.1.0 S6.1's entry, S6.2's native layer and S6.3's tests may still change. CS3.7 and CS4.1 re-baseline against the 1.1.0 tag.
- **R2. Size and review load.** 51 steps, with eight dual reviews (four freezes, CS3.4, and three phase exits), two Surface reviews and about forty single-axis reviews. The pipeline rule of GwzProcessOptimization §4.4 applies: the implementer starts the next step behind a frozen interface while reviewers hold the last one.
- **R3. Three dispatch tables at once.** From Phase 2 until CS4.8 and CS6.4, core's dispatch, gwz-py's native dispatch and gwz-cli's `execute_invocation` all exist. CS2.13–CS2.16's parity tests guard against drift. A handler change in that window updates every copy or waits.
- **R4. A runtime per operation.** Eight operations can open up to 8 × 32 connections to one host, and each builds its own runtime threads (§16). If CS3.11's measurements are unacceptable, the proposals' decision 1, one runtime per session, reopens as a contract amendment; this plan does not choose it.
- **R5. Direct-worker saturation.** `diff` and `log` produce to completion before they reply, so eight large ones occupy every direct worker and `status` waits. §5.2 promises only that `status` never waits behind operations.
- **R6. A detached worker that never returns** keeps its thread, any cross-process lock it holds and its registry entry until the process exits, and process exit can tear a local write it has in progress (§16).
- **R7. Fetches are not recorded in the registry.** A fetch member step in one session can run beside a W in another session sharing the host context, although the pair is excluded within one session. This matches the cross-process status quo.
- **R8. User-visible changes.** CS3.4 changes how HTTP credentials are found for both existing drivers, and that path now needs `git` on `PATH`. Phase 4 changes Python's cancel codes, adds `transport_session_full` and `operation_expired`, applies the open-merge pre-gate to Python (D4), and makes interpreter exit wait up to the close bound, 60 seconds by default. The release notes and the Surface reviews carry these.
- **R9. gwz-core's published crate API changes:** the new `session_host` module, the §16 handler changes and the deprecations. No compatibility rule for the crate API is written down.
- **R10. Windows.** Transport proofs depend on dabeest. The contract names only Linux and macOS for the exit test and no platform for the pseudo-terminal test (C6). Environment names on Windows are case-insensitive and values are WTF-8 (CS1.5, CS4.5).
- **R11. Large files.** `transport_host/session.rs` is already 1,009 lines, `https_auth.rs` 934 and `agent_job.rs` 769. Steps that touch them split first.

### 6.2 Decisions for the operator

- **D1. The contract revision path** (§2.1). Recommended: path 1, revision 4 with RemPlan-2 §1 and §2.
- **D2. Which release carries which phase.** Recommended: Phases 1–3 may ship in any release after 1.1.0, since only CS3.4 and the `cancellation` field are visible to users; Phase 4 ships only with Phase 5, whose two-bridge run is gwz-py's release evidence; Phase 6 ships with Phase 4 or later.
- **D3. CLI requests with no protocol method** (C2). Recommended: `forall` and `claude-code setup` stay CLI-local named exceptions, and `forall` takes the cross-process workspace lock itself, as CS6.3 describes. `init --update` gets an append-only protocol method through a contract amendment, because GWZDesign routes structured workspace operations through the message path. The alternative is a third named exception.
- **D4. The open-merge pre-gate for Python.** Moving it into core's dispatch applies it to Python requests, which skip it today; core's handlers already guard mutations, so the change is an earlier refusal for some calls. Recommended: yes, for cross-driver parity (L2-03), with the changed outcomes in the Phase 4 Surface review.
- **D5. One transport-scope predicate** (CS1.6). Recommended: the union of gwz-cli's and gwz-py's, narrowed only by a test that shows a method does no network I/O.
- **D6. gwz-py's legacy module-level entry points** (§9 leaves this to implementation). Recommended: remove them, so CS4.8 clears every gwz-py `debt` entry. Keeping them off the session path leaves their entries in the allowlist, and O9's test would then need a session-path marker there.
- **D7. The ordinary build after 1.1.0** (C1). Recommended: keep a CI build configuration without the transport, so §5.8 and §15.15 stay testable. The alternative is a contract amendment that confines §5.8 to libgit2's native remotes.
- **D8. A wire field for the default clocks.** The 1.1.0 amendment round's Safety P3-6 asks for the defaults "in a typed field"; §13 has none. Recommended: CS3.8's assertion on the session context, and an amendment only if the operator wants the field.
- **D9. Merge timing.** Recommended: product code merges after the 1.1.0 tags exist.
- **D10. The V0 `OperationRuntime` API.** Recommended: deprecate `submit`, `subscribe` and `wait` in CS2.5 and remove them at a later major version. Removing them now breaks the published crate's API.

### 6.3 Contract ambiguities found while planning

Each has a working default in the step named, and each is a candidate for the contract's next revision.
- **C1 (§5.8, §13, §15.15, §16).** These sections predate 1.1.0's activation; §16 still says the transport "stays candidate-only". After S7.1 the normal build contains the transport on every platform GWZ builds, so "ordinary builds" may describe no product build (D7). In a transport build `transport_capabilities.cancellation` is true, yet libgit2's native remotes (`git://`, `http://`, `file://`) cannot be cancelled promptly. §5.8 states the timeout exception for those remotes but not the cancellation one.
- **C2 (§5.2, §11).** The CLI "sends its request", but three CLI requests have no protocol method: `init --update`, `forall`'s command execution with its workspace guard, and `claude-code setup`. O6 forbids sharing the `forall` guard across the channel (D3).
- **C3 (§4.2, §8).** The cleanup report for a cancel whose target never touched the network is unstated. §8's close rule uses `(0, false)` when no peer cleanup occurred, while Verdict-2's residual assumed `(0, true)` for a direct call. CS2.7 uses `(0, false)`, which is also today's `CleanupReport::default()`.
- **C4 (§4.2, §5.1).** The reply to a call whose `method` the service does not declare is unstated. The rule that a method missing from the class table is W governs class, not routing. CS1.6 uses `invalid_request` before any effect.
- **C5 (§4.2).** A call ID below the highest received that the session never saw, which non-contiguous IDs allow, is neither "above the highest" nor a spent call. CS2.2 answers `operation_expired`.
- **C6 (§15.11, §15.14).** The exit test names Linux and macOS although the extension ships on Windows; the pseudo-terminal test names no platform. CS4.5 adds Windows to the exit test; CS6.4 runs the pseudo-terminal test on macOS and Linux.
- **C7 (§5.6).** The snapshot "fixes environment values and the paths they name", but the contract does not say that lookups follow the platform's name rules. Windows names are case-insensitive. CS1.5 follows the platform.

## 7. Review of this plan

- **Object.** This file, identified by its SHA-256 while uncommitted, as the 1.1.0 plan and its amendment were, and by its commit once committed.
- **Tier.** Dual peer-blind Consistency and Safety, with GO on both axes before any step starts (G1). The 1.1.0 amendment round's Consistency P3-5 and Safety P3-2 ask for exactly this gate. No Surface review: the plan fixes no user-facing surface; its Phase 4 and Phase 6 exits carry Surface.
- **Consistency attacks** agreement with the contract at the accepted revision: every section in §1.2 has an owning step, and every §15 row has an owning step in the repository where its test lives. Also agreement with the proposals' §9 outline, and with the 1.1.0 plan as its accepted amendment amends it: no step repeats 1.1.0 S6.1–S6.3, and every obligation the amendment hands over appears in §5.3. Also the completeness of §5, the tiers against GwzProcessOptimization §4.2, and the standing rules.
- **Safety attacks** what the plan permits to go wrong: a silent native route during the transition (Phase 2's refusal, the transport-scope predicate, the cfg arms); drift between the three dispatch tables; behaviour changes through the legacy adapter; activation order; Windows gaps; secrets in evidence; gaming the ratchet by flipping `debt` to `permanent`; parallel lanes that share files; a contract revision that changes mid-program.
- **Remediation.** GwzProcessOptimization §4.1's cap: two rounds, and a third only for non-architectural corrections.
- **On GO.** A status-only edit under AgentProcessRules §7.2: "Status: accepted at <SHA-256> after <review files> reported GO; this accepts the plan text only". The program checkpoint records the acceptance and each step's review tier (GwzProcessOptimization §4.2). The plan becomes implementation authority only together with G0 and the operator's direction to start.
- **Re-review triggers.** A contract revision accepted after this plan: a focused Consistency re-check of §2.1, §5 and the affected steps. A later change to the 1.1.0 amendment that moves an obligation: a focused re-check of §1.1, §2.3 and §5.3. A step that must cross another step's files: a handoff under L1-06.
- This plan authorizes no implementation, commit, tag, push or publish.

## Changelog

- 2026-09-27: status notes that the transport release carries this plan, pending TR1.4a and TR1.4b of [`GwzTransportReleasePlan.md`](../gwz-core/dev-docs/GwzTransportReleasePlan.md).
