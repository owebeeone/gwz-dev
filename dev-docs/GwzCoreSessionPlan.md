# GWZ core session host — implementation plan

Date: 2026-09-26; revisions 1 and 2, 2026-09-27; revision 3, 2026-09-28. Status: **accepted at SHA-256 `01adc2f0b1647c987f903c4704ced759c6f12599582048e7fb945e51012e0255` after [Consistency-2](GwzCoreSessionPlan-ReviewConsistency-2.md) and [Safety-2](GwzCoreSessionPlan-ReviewSafety-2.md) reported GO; this accepts the plan text only**. As of 2026-09-27, the [transport release plan](../gwz-core/dev-docs/GwzTransportReleasePlan.md) carries this plan's work in the transport release.
- Revision 2 was accepted for TR1.4a's scope at SHA-256 `58ab341a92c68a0b08f6383b60b23dc6e299d428fb4259409e7547eb556ea0f2` after [Consistency-1](GwzCoreSessionPlan-ReviewConsistency-1.md) and [Safety-1](GwzCoreSessionPlan-ReviewSafety-1.md) reported GO. Its status sentence and corrections were added after that GO, and [Verdict-1](GwzCoreSessionPlan-Verdict-1.md) records them. Revision 3's acceptance, for TR1.4b, completes the plan's G1 (§2.2).
- Revision 3 revises the steps revision 2 left to TR1.4b, adds the extension steps, two addenda and the reuse and server phases, and corrects the text that Verdict-1 and the contract's [Verdict-5](GwzCoreSessionDesign-Verdict-5.md) recorded. The changelog lists every change. This status sentence was added after revision 3's GO. So were the post-GO corrections its reviewers cleared and then confirmed ([Consistency-2a](GwzCoreSessionPlan-ReviewConsistency-2a.md), [Safety-2a](GwzCoreSessionPlan-ReviewSafety-2a.md)), and the lane owner's last text corrections from those confirmations. [Verdict-2](GwzCoreSessionPlan-Verdict-2.md) records them.
- The acceptance authorizes no implementation, activation or release.

This is the phased plan that item 2 of [the proposals §9](GwzClientCoreTransportProposals.md) calls for. It implements the [core session contract](GwzCoreSessionDesign.md), at revision 5 as the [connection reuse design](GwzConnectionReuseDesign.md) and the [server design](GwzCoreServerDesign.md) amend it, in the transport release. Its Phases 1 to 6 are that plan's Phase 5, and its Phases 7 and 8 are that plan's Phases 6 and 7. It has eight phases and 116 steps, one of which, CS1.8, revision 4's application has already done. It cites the contract by section number; sections keep their numbers across revisions 2 to 5. It cites the reuse design as "reuse §n" and the server design as "server §n". It maps the findings that [Verdict-2](GwzCoreSessionDesign-Verdict-2.md), [Verdict-3](GwzCoreSessionDesign-Verdict-3.md) and [RemPlan-2](GwzCoreSessionDesign-RemPlan-2.md) carry, the obligations the transport release plan moves here, and the two designs' steps and rows, onto named steps (§5).

The plan authorizes nothing by existing: no implementation, commit, tag, push or publish. Code facts were read at root `9a653065`, gwz-core `b13bbadb`, gwz-cli `ebbea902`, gwz-py `4ad2b077` and gwz-transport `a7a36aec`, called the planning tuple below. Revision 1's new code facts were read at root `4bf52e00`, gwz-core `bd538656`, gwz-cli `ebbea902`, gwz-py `0b535dc5` and gwz-transport `a7a36aec`, called revision 1's tuple. Revision 3's new code facts were read at revision 1's tuple, which was unchanged on 2026-09-28; it is also called revision 3's tuple.

## 1. Purpose and scope

### 1.1 The transport release

The [transport release plan](../gwz-core/dev-docs/GwzTransportReleasePlan.md) replaces the [1.1.0 plan](../gwz-core/dev-docs/GwzV110Plan.md). It was accepted on 2026-09-27 and amended the same day by [GwzTransportReleasePlanAmendment.md](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment.md). Its release, expected to be v1.1.0, ships in the normal builds on macOS ARM64, Linux x86-64 and Windows x86-64:
- the transport, in the `local` placement;
- this plan's session host, with gwz-cli and gwz-py as its clients;
- `gwz server` and `gwz-py server`, with the server's stdio mode and its SSH remote form (TR1.3; OD12 is yes);
- connection reuse across operations (TR1.2).

Its Phase 5 is this plan's Phases 1 to 6, as TR1.4a and TR1.4b revise them, and its Phases 6 and 7 are this plan's Phases 7 and 8, which TR1.4b adds. Its other parts frame them:
- **No part of the contract ships earlier.** Under OD1, gwz-py goes straight to the session host. The transport plan retires the 1.1.0 amendment's S1.1, S1.2 and S6.1–S6.3 as steps, and moves S6.1–S6.3's obligations here (§5.3).
- **The tree this plan starts from** (transport plan §3). The transport builds only under the `gwz_transport_candidate` cfg, on Unix, and a half-landed crate rename breaks that build (§2.4). gwz-py's candidate `TransportSession` (`native/src/transport_session.rs`) is still in the tree, with its `CURRENT_SESSION` thread-local.
- **Built beside this plan.** TR3.1 finishes the rename, and 1.1.0 S4.5 gives the candidate's construction sites `cfg_if` unix and windows arms (G2). The transport plan's Phase 2 finishes the transport and reviews its code.
- **After this plan.** Activation, 1.1.0 S7.1 in the transport plan's Phase 9, removes the switch from every site, the session host's transport entry and its arms, and the server surfaces the switch gates (the transport plan's §6(a)), included. Only then does the normal build ship the transport and the server. 1.1.0 S7.3 then asserts the transport route on the normal builds, before the tags of the transport plan's Phase 10. Until activation, this plan's transport rows run on the candidate build.

### 1.2 In scope

The whole contract, including the parts that 1.1.0 S6.1 and S6.2 were to build (§5.3):
- the schema additions and the model-to-wire error mapping (§4, §13);
- the channel and its two adapters (§3);
- the host context, the session context and operation gates (§5.6), and the removal of the process-global state they replace: every `debt` entry of §5.7's allowlists that lies on the session path;
- admission, workers and the shared dispatch (§5.1, §5.2). The dispatch moves gwz-cli's `execute_invocation` (`gwz-cli/src/globalargs/dispatch.rs`) into gwz-core as a dispatch over protocol requests. Today that function is CLI-specific: it chooses the event sink, holds a guard across `forall`, and applies the open-merge pre-gate. gwz-core has no such dispatch. gwz-py's native `dispatch/` module, about 1,800 lines, is a second routing table that the move retires;
- cancellation, retention, events and results, and closure (§5.3, §5.4, §6, §8);
- ordinary-build behaviour, including core's own `git credential fill` (§5.8);
- the gwz-py extension's channel and the thin `NativeCoreBridge` (§9, §10);
- the wire proof (§12);
- gwz-cli onto the session host (§11);
- connection reuse across operations, with the contract as reuse §13 amends it: the host context's endpoint registry, a binding per operation, revalidation at each cross-operation lease, and the gwz-transport pool changes it names (reuse §2 to §11, §14);
- the server, with the contract as server §8 amends it: the byte-stream handshake, the socket host, the stdio mode and its SSH remote form, the client forms of both CLIs, and `SocketCoreBridge` (server §2 to §17).

The proposals' outline maps onto the phases: its item (1) is Phase 2, (2) is Phase 3, (3) is Phase 4, (4) is Phase 5 and (5) is Phase 6. Phase 1 freezes the interfaces they share. Phase 7 builds the reuse design (TR1.2) and Phase 8 the server design (TR1.3). The outline has neither, and its item (6) stays out of scope (§1.3).

### 1.3 Out of scope

- Client placement, item (6) of the proposals' outline. The contract excludes it (§1) and reserves only frame tags 16–31 (§3). It needs a contract amendment when a remote core is scheduled.
- Any change to the virtual-stream protocol or to gwz-transport's envelopes (§1, §13; reuse §13).
- A server for another user, any TCP listener, a server on another machine other than the SSH remote form, and a gwz-py library client for the stdio mode or the SSH remote form (server §1).
- The half-landed crate rename that breaks the transport candidate build (§2.4).
- Migrating existing code, outside the files a step touches, to the conditional-compilation rule. CS1.7 inventories that code; it does not migrate it.

Revisions 1 and 3 remove five exclusions (TR1.4a, TR1.4b). Where each now lives:
- **1.1.0 S6.1–S6.3.** The transport plan retires them as steps (its §4, OD1). §5.3 maps their obligations.
- **The socket server** of [GwzCoreServerDesign.md](GwzCoreServerDesign.md). It ships in the transport release, with the stdio mode and, since OD12 is yes, the SSH remote form. TR1.3 revised its design, which is accepted. Revision 3 places its steps in Phase 8, the transport plan's Phase 7: `gwz server` after this plan's Phase 6, `gwz-py server` and `SocketCoreBridge` after its Phase 4, the Windows primitives after the transport plan's Phase 4, and reuse through a server after Phase 7. The core pieces that earlier steps need come earlier: the schema (CS1.10) and the host side of the handshake (CS5.4).
- **Connection reuse across operations.** The accepted reuse design takes it off the contract's §1 list of exclusions (reuse §13). Revision 3 places its steps in Phase 7, the transport plan's Phase 6.
- **Changes to gwz-transport.** TR1.2 names those that reuse needs (its question 12). The accepted reuse design amends the contract's §1 exclusion, "any change to gwz-transport", for its pool changes, and §13's clause on envelopes and the virtual-stream protocol stands (reuse §13). The transport plan's changelog of 2026-09-28 corrects its TR1.2, which had cited §13 for this clause. Revision 3 places the changes in Phase 7 (CS7.2–CS7.6, CS7.10), and they ship in gwz-transport's Phase 10 release.
- **Changes to the public Python API.** It gains only `NativeCoreBridge`'s optional host context (CS4.5) and `SocketCoreBridge` (CS8.27), as the transport plan's §2 scope row states.

## 2. Prerequisites and gates

### 2.1 G0 — an accepted contract revision

No step starts until a filed verdict accepts, on both axes, the contract revision the step implements.
- **Revision 5, as the reuse and server designs amend it, is that revision.** [Verdict-5](GwzCoreSessionDesign-Verdict-5.md) accepts revision 5 on both axes: revision 4 with `cancellation` at field 8 (C8). [Verdict-4](GwzCoreSessionDesign-Verdict-4.md), recorded at root `3480a76`, accepted revision 4 and closed B11; revision 4 is committed at root `fa3f44c`, with its B11 tooling at gwz-core `4bd92285` (CS1.8). The reuse design's [Verdict-1](GwzConnectionReuseDesign-Verdict-1.md) and the server design's [Verdict-1](GwzCoreServerDesign-Verdict-1.md) accept the contract text each amends, as TR1.2's and TR1.3's closures require, and the contract's status names both amendments.
- **Path 1 below holds** (D1). Revision 4 carries RemPlan-2 §1 and §2 in full, as the transport release plan's OD3 recommended and the operator adopted.
- **G0 is discharged.** The paired paragraphs in gwz-core's [GWZDesign](../gwz-core/dev-docs/GWZDesign.md) and [GWZRequirements](../gwz-core/dev-docs/GWZRequirements.md), which gwz-core's `AGENTS.md` requires before core behaviour expands, are no longer marked DRAFT and name revision 5. They were flipped on 2026-09-28 with the reuse design's "On GO" edits (the contract's changelog), and the server design's "On GO" edits added paired "Core server" paragraphs. Revision 2 said the flip was to revision 4; Verdict-5's third recorded item corrects that to revision 5. At revision 3's tuple these edits are in gwz-core's working tree, uncommitted.

Revision 5 and both designs' amendments were accepted after revision 2's acceptance, which triggers §7's re-check. Revision 3 is that re-check: §5.9 and §5.10 map each clause the two designs amend to the step that implements it, and a step revision 2 accepted gains an extension step or an addendum where an amended clause reaches it (§2.2).

The record from before revision 4's acceptance stays readable. On 2026-09-26:
- revision 2 is accepted ([Verdict-2](GwzCoreSessionDesign-Verdict-2.md));
- revision 3 is NO-GO on one blocking root, B11, and revision 2's acceptance stands ([Verdict-3](GwzCoreSessionDesign-Verdict-3.md));
- [RemPlan-2](GwzCoreSessionDesign-RemPlan-2.md) proposes revision 4, which is not applied.

RemPlan-2 §4 left three decisions to the operator, and revision 0 of this plan worked on any of three paths (decision D1):
1. **Revision 4 carries RemPlan-2 §1 and §2 and is accepted (recommended).** Steps follow revision 4. The findings RemPlan-2 corrects are contract text, and their closure tests are §15 rows.
2. **Revision 4 carries only B11 and is accepted** (RemPlan-2 §4, decision 2, second option). Steps follow revision 4. RemPlan-2 §2's ten findings become obligations of the steps that §5.2 names, each with its closure test. A step that follows one of them where the accepted text differs records the finding it follows.
3. **Revision 4 is not applied.** Revision 3 cannot be implemented. Starting on revision 2 needs the operator's explicit direction, recorded in the program checkpoint. Verdict-2's ten findings (as revision 3 worded their corrections), B11 (through CS1.8) and RemPlan-2 §2's ten findings then all become step obligations. This path is not recommended: revision 2 lacks corrections that both axes have already made.

### 2.2 G1 — this plan's review

Dual peer-blind review of this document, with GO on both axes, before any step it gates starts (§7). The transport plan's Phase 5 splits it in two:
- TR1.4a's review of revisions 1 and 2 is G1 for every section and step they revise, which revision 2 left unmarked.
- TR1.4b's review of revision 3 is G1 for the rest: the steps revision 2 marked "TR1.4b revises this", which revision 3 revises and unmarks; the extension steps and addenda below; Phases 7 and 8; and every other section the changelog's revision-3 entry lists.
- **Extensions.** The accepted reuse design extends six steps revisions 1 and 2 revise, which revision 2 marked "TR1.4b extends this": CS1.1, CS1.4, CS2.12, CS3.6, CS6.1 and CS6.4. Revision 3 gives each extension its own additive step (§5.11), and each extended step points to it. An extended step may merge on TR1.4a's GO, and its extension is reviewed as an additive re-freeze (L1-09). A re-freeze is a freeze, so its review is dual (§3.0).
- **Addenda.** Where a clause the server design amends reaches a step revision 2 accepted and did not mark, revision 3 adds a marked addendum to that step: CS2.3 (§5.1) and CS3.4 (§5.6). A step that merges before revision 3 is accepted takes its addendum as a follow-up, reviewed at the step's own tier. The server design's amendment of §5.6 also reaches CS1.4 and CS1.5, whose implementation began on TR1.4a's GO (the program checkpoint), so their share goes to the extension step CS1.9 instead.

### 2.3 G2 — build prerequisites for transport rows

A transport row is a test that needs the candidate build: a §15 row on the transport path, a route proof, or a test of candidate-only code. That code is `transport_host` and `git/endpoint` in gwz-core, and `native/src/transport_session.rs` in gwz-py. Phase 7's rows, and Phase 8's rows that run a network operation or a surface behind the switch, are transport rows too.
- **TR3.1** (transport plan Phase 3) finishes the crate rename, so the candidate build compiles (§2.4). Every transport row waits on it, and so does every step that changes candidate-only code, CS1.1's rename and CS1.10's regeneration included.
- **1.1.0 S4.5** (transport plan Phase 4) turns the candidate's construction sites into `cfg_if` unix and windows arms. Every Windows transport row, on dabeest, waits on it, and so does every phase exit that names dabeest. Phase 8's Windows steps also wait on the rest of the transport plan's Phase 4 (1.1.0 S4.1–S4.5), as its Phase 7 orders.
- **No tag precedes any phase.** The release's tags come in the transport plan's Phase 10, after this plan's phases and activation.
- **Merge timing.** D9 is superseded (§6.2). A step's code merges to a product repository's main once its review passes, under the transport plan's §6 rules (a)–(d), as amended:
  - under (a), `gwz_transport_candidate` gates the transport and the other surfaces that rule lists, until activation;
  - under (b), once a step marked **Ordinary path** (§3.0) merges to a product repository's main, no 1.0.x release is cut from that main, and a 1.0.x patch comes from the last 1.0.x tag on a release lineage;
  - under (c) and (d), a change to a documented contract is released only with Surface review, and at each patch decision the program checkpoint lists what main carries.

### 2.4 Execution prerequisites

These are not planned here.
- **A candidate build.** Until activation (§1.1), transport tests need `gwz-core/tests/transport_backend/prepare.py` and `RUSTFLAGS='--cfg gwz_transport_candidate'`. A half-landed crate rename breaks the harness: its `[patch.crates-io]` entries still name `git2` and `libgit2-sys`, while gwz-core depends on the packages `gwz-git2` and `gwz-libgit2-sys`. TR3.1 finishes the rename (G2).
- **Python 3.11 or later** for gwz-core's `scripts/run_tests.py`, whose crate-version check imports `tomllib`.
- **The dabeest Windows host** for Windows transport evidence, under the transport plan's §2 rules: mingw bash, the `/e/gwz-tests/<name>` work root, the historical `D:` trees untouched, and the Cargo config isolation. GitHub's windows-2022 runners cover builds and every test that needs no network fixture. Windows transport rows also wait on 1.1.0 S4.5 (G2).
- **Evidence handling.** Raw runs go to the private gwz-core-evidence member ([EVIDENCE.md](../EVIDENCE.md)). Retained evidence and public reports redact agent-socket paths, known_hosts bodies, `gh` tokens and headers, private members' repository names, and agent key comments and fingerprints, as the transport plan's §2 requires as amended. Python-visible errors and server logs follow the same rule. A secret in filed evidence fails its step.
- **Live accounts.** Every run against a real account needs the operator's go (transport plan §2).
- **Both protocol builds, until activation.** Every schema step keeps both generated protocol files in step, the production `src/protocol/generated.rs` and the candidate's `tests/transport_consumer/candidate/candidate_generated.rs` (CS1.1). Once TR3.1 has landed, every step, schema step or not, builds the candidate before it merges, and its review package records the build.
- **Regenerating the candidate protocol** with `tests/transport_consumer/protocol/candidate-regenerator.py` needs the inputs that `candidate-generator.json` beside it pins, among them: a clean taut checkout whose `src/` is at the pinned revision `bcf98b64…`; taut-proto `0.9.1`; rustfmt `1.9.0-stable`; and the gwz-transport owner schema (`gwz-transport` `0.1.0`) at its pinned digest. The file also pins the core schema its retained old reader was generated from, gwz-core `54618449`'s (`551fe930…`), and the owner schema at `10179189…`. Both pins are older than HEAD by design, and a schema step that regenerates moves both deliberately (CS1.1).
- **Linux runners for server rows** (server §12, §14; its Verdict-1). Every Linux row that runs a server, the stdio local form included, runs where the test's own processes have a `Seccomp` status of 0 and the initial user namespace, such as a hosted virtual-machine runner, and never in a `container:` job; each such job's first step asserts both. At revision 3's tuple no test job in gwz-core, gwz-cli or gwz-py runs in a container. gwz-cli's `release.yml` build job may (cargo-dist's matrix), and it runs no server row.
- **Phase 7's and Phase 8's fixtures,** all disposable and needing no live account: fixture agents that count sign requests and can deny them, a local TLS smart-Git server and a fake `gh` that records its environment (reuse §15); a loopback `sshd` with the binary under test first on its `PATH`, on Linux and macOS, and the Windows OpenSSH client on dabeest (server §16); for the walk, a Linux runner with `sudo` for autofs and systemd automount fixtures, and a disposable macOS runner with `/net` and a direct map enabled; and dabeest for named pipes, integrity levels and AppContainer (server §12).

## 3. Phases and steps

### 3.0 Step format, standing rules and review tiers

**Format.** Each step names:
- its repository and files, which are also its ownership manifest: two steps that name the same file never run at once (L1-06);
- the contract sections it implements;
- **test-first:** the §15 rows and carried findings it closes, written as failing tests before the change (gwz-core `AGENTS.md`, L1-12). §15 rows are cited as "§15.n (short description)", because bullets can move between revisions. The reuse design's §15 items, which the contract takes as §15.16, are cited as "reuse §15.n (short description)", and the server design's §12 rows, which are not numbered, as "server §12, <group> (short description)";
- its dependencies;
- its review tier;
- a budget: aspirational hand-written production lines. Tests, generated code and moved code are reported separately (GwzProcessOptimization §2.1);
- the **Ordinary path** marker and its reason, when the step changes ordinary-build behaviour (below). Each step revision 3 adds also says so when it is not.

**Standing rules.**
- gwz-core stays independent of gwz-cli. The session host, the dispatch and the test bridge live in gwz-core; rendering stays in gwz-cli.
- Payloads are Taut messages (L2-02, L3-06). Frames carry Taut-encoded bodies. No step adds a Rust-only or Python-only wire type.
- Conditional compilation sits in `cfg_if` blocks or enclosing platform modules, never as a bare `#[cfg]` on an import or another unbraced declaration. Every control-flow body is braced. A step that moves or deletes a declaration moves or deletes its attributes and owning scope with it. CS1.7's check runs in every step's gate. Its scope is items and unbraced statements: at revision 3's tuple it lists 437 occurrences under 432 keys, 264 items and 173 statements. It excludes, and counts, the positions that no explicit boundary can hold: 31 fields and variants, 7 struct-expression fields, 8 match arms, 2 parameters, 2 call arguments and 4 tail expressions ([CS1.7's Verdict-1](GwzCoreSessionCS1.7-Verdict-1.md)).
- Every step builds and passes on macOS, Linux and Windows CI. Steps on the transport path also pass on dabeest; until 1.1.0 S4.5 lands, those rows wait for it (G2).
- The process-global ratchet moves in the same commit as the code. A step that removes state removes its allowlist entry. A step that adds state that carries session-relevant content lists it as `debt` and names the step that removes it. State that carries none may be listed `permanent`, with its reason, under review, only when it is a marker, flag or counter whose type holds no payload, as CS1.4's `thread_local CROSSING` is ([CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md)); where a test can prove what the reason claims, the step adds it, as the driver-level exception requires. Anything else a step adds is `debt`, with a named remover. A spawn entry flips from `debt` to `permanent` only with a test that proves `env_clear()` plus the snapshot, since the checker cannot see `env_clear` (Consistency-3's residual on "the lists only shrink").
- One exception, from revision 3: a step may add a `permanent` entry for process-wide state or a spawn that an accepted design puts outside the session path, with no session state in it: the server launcher's spawns (CS8.13, CS8.18), the serving process's signal handling (CS8.10, CS8.26) and macOS's automount hold (CS8.7). The entry cites the design's section, and a test proves what its reason claims; for a spawn, which descriptors, environment and working directory the child gets. Such entries cannot be `debt`, because the server's release gate fails on any `env` or `process` `debt` (server §5; CS8.4).
- The Rust quality gate of L2-01 applies: format, Clippy with warnings denied across targets, tests.
- New files stay under 500 lines. A step that would take an existing file past about 1,000 lines splits it first in a movement-only commit (L1-11, L1-23), using the rust-split tool. At revision 3's tuple, five product files that steps here grow are past that already: `src/git/endpoint/ssh_worker.rs` (1,279 lines), `placement_endpoint.rs` (1,064), `https_worker.rs` (1,056) and `src/transport_host/session.rs` (1,009), which CS7.1 splits before Phase 7 grows them, and gwz-py's `src/gwz/client.py` (1,380), which a step that grows it first reviews for cohesion (L1-23; R11).
- Documentation changes land in the step that changes the behaviour (L1-24).
- Legacy entry points keep today's behaviour until their driver moves (CS3.1), except where a step marked **Ordinary path** says otherwise.

**Ordinary-path steps** (transport plan §6(b)).
- A step marked **Ordinary path** changes, once merged, what a build without `gwz_transport_candidate` does for an existing user of gwz, gwz-py or the gwz-core crate: a result, output, lock, requirement, protocol text or public API that user relies on, or the path an existing request takes through core.
- From the first marked step that merges to a product repository's main, no 1.0.x release is cut from that main (§2.3).
- A step is not marked when it only adds code that no ordinary-build caller reaches, changes only candidate-only code (G2), or keeps legacy callers' behaviour under the rule above. The program checkpoint still lists it under the transport plan's §6(d).
- The marked steps are CS1.1, CS1.10, CS2.5, CS2.10, CS3.3, CS3.4, CS3.9, CS4.7, CS4.8, CS6.2, CS6.3 and CS6.4. Of the steps revision 3 adds, only CS1.10 changes the ordinary build. Revision 3 also decides CS3.10, which it revises, and leaves it unmarked, with the reason in the step. No step of Phase 7 or Phase 8 is marked: their code is candidate-only, or sits behind the switch (their preambles).

**Review tiers** follow GwzProcessOptimization §4.2 and the review-loop rules, cross-model where available (§4.3):
- **Dual:** peer-blind Consistency and Safety. Used at the four interface freezes (CS1.1 schema; CS1.2 channel contract; CS1.4 with CS1.5, the gate and context API; CS1.6 dispatch signature), at CS3.4, and at the exits of Phases 3, 4 and 6, which change behaviour for users. Freezes that settle together may be reviewed as one interface checkpoint (AgentProcessRules §6.1), still dual.
  - Revision 3 adds its freezes and re-freezes: CS1.9 and CS1.10 (re-freezes of CS1.4 with CS1.5 and of CS1.1, and the server's frames); CS3.2 (the endpoint configuration); CS3.7 (the binding seam, and CS1.4's re-freeze); CS5.4 (`serve_session`); CS6.6 with CS6.7 (re-freezes of CS6.1 and CS6.4, one checkpoint); CS7.2–CS7.6 (the pool, one checkpoint); CS7.9 (the instance and its bindings); CS7.16 (CS3.6's re-freeze); CS7.18 and CS7.19 (revalidation); CS8.1 (the must-match set); CS8.5 (the address grammar); CS8.8 (the sandbox rule); CS8.14 (the client channel); CS8.16 (the SSH remote form); and, from [TR1.4b's Verdict-2](GwzCoreSessionPlan-Verdict-2.md) (S-P3-2), CS8.9 with CS8.10 (the socket host's file, record and stale-socket rules and its accept path, one checkpoint) and CS8.17–CS8.19 (the Windows re-freezes of CS8.5, CS8.8 and CS8.14, one checkpoint).
  - It also adds the exits of Phases 7 and 8, settled-tree gates, the first of which is CS7.26's activation review.
- **Surface:** added at the Phase 4 exit (the Python API, gwz-py's `gwz-py` console script, and R8's and CS4.5's changes, transport plan TR1.7), at CS4.9 (reuse across a Python process's operations, TR1.7's remaining subject), and at the Phase 6 exit (gwz-cli help and output). The server's surfaces stay behind the switch until activation, where S7.5's Surface review covers them (transport plan Phase 9).
- **Single-axis:** every interior step behind a frozen interface. The step names the first axis, and axes alternate. Any P0, P1 or P2, or a reviewer's request, escalates to the second axis. Remediation re-reviews are single-axis by default.
- **Activation steps** (CS4.7, CS6.4, CS7.26) are reviewed by their phase exit's review on the settled tree (L1-32).
- The tiers are recorded in `CurrentProgramCheckpoint.md` when this plan is accepted (§7).

### Phase 1 — Frozen foundations (milestone: the schema additions and the frozen interfaces are merged and tested; nothing calls them)

- **CS1.1 — Schema additions and error mapping** *(gwz-core and gwz-py; < 250 lines, generated code excluded)*.
  - **Ordinary path:** the ordinary build's generated protocol and catalogs change; `TransportCapabilitiesResponse` gains a field and `GwzErrorCode` two members, both re-exported at the crate root.
  - **Extended by CS1.10** (reuse §13): the ErrorCatalog comment for `transport_session_full` names two host-context causes. The extension is a dual re-freeze (§2.2).
  - Files: gwz-core `protocol/gwz.taut.py`, the bindings and corpus that `protocol/regen.py` regenerates, `src/protocol/convert.rs`, `docs/MessageCatalog.md`, `docs/ErrorCatalog.md`, `docs/Protocol.md`; gwz-py `src/gwz/protocol/generated/`, through `scripts/regen_protocol.py` and `scripts/check_protocol_drift.py`. For the rename below, the users of `transport_host::CleanupReport`: gwz-core `src/transport_host/`, and gwz-py's candidate `native/src/transport_session.rs` until Phase 4 removes it (§5.3). The candidate protocol (§2.4): gwz-core `tests/transport_consumer/candidate/candidate_generated.rs` and `candidate_generated.py`, which `tests/transport_consumer/protocol/candidate-regenerator.py` writes from `candidate.taut.py`, the production schema plus a placement projection; and `tests/transport_consumer/protocol/candidate-generator.json`, whose pins it moves.
  - Implements §13 in full, §4.1, and §4.2's error codes, `convert.rs` mapping and `cancelled` comment.
  - Test-first: §15.2 (a `SessionError` with each named code round-trips through the Rust and Python projections; each §13 body round-trips in both languages). Verdict-2 residual: the Taut `CleanupReport` coincides with `gwz_core::transport_host::CleanupReport`, and the crate re-exports generated types at its root, so the step renames the Rust type to keep one meaning per name; §13 fixes the Taut name. `OperationCancelRequest`'s comment states that zero or two targets is `invalid_request` (CS2.7 tests it). The new messages stay out of `gwz.__all__`.
  - The candidate protocol, kept in step until activation:
    - CS1.1 brings the §13 additions into the candidate, beside the production file, by regenerating it; a hand edit would leave the generator's output stale. The candidate is pinned to an older schema by design: `candidate-generator.json` pins the retained old reader to gwz-core `54618449`'s schema (SHA-256 `551fe930…`) and the gwz-transport owner schema to `10179189…`, and the regenerator refuses any other, so the candidate lacks `GwzErrorCode` members 73 and 74. CS1.1 moves both pins deliberately when it regenerates (Verdict-1; contract Verdict-5).
    - The regenerator's check and the lexical test below run in gwz-core's `scripts/run_tests.py` or its CI, so the composition's refusal of a colliding tag runs on every change, not only when CS1.1 regenerates (contract Verdict-5).
    - A lexical test compares the two files' `GwzErrorCode`, `TransportCapabilitiesResponse` and §13 messages. They must match, except for the projection's recorded additions in `candidate.taut.py`, among them `ResponseMeta.transport_message` and `TransportCapabilitiesResponse` fields 3 to 7. The test fails when the files disagree on any member, field or code number of the §13 additions.
    - The candidate build compiles, and the protocol corpus vectors pass under both cfgs.
    - The projection already gives `TransportCapabilitiesResponse` field 3 to `message_versions`, and its composition refuses a colliding tag, so revision 4's `cancellation` (3) cannot enter the candidate. The operator's decision (C8) gives it field 8 in the contract's revision 5, and CS1.1 implements that number, optional and allowed to be absent (Taut `optional=MISSING_OK`, §13).
  - Also test-first, from revision 5's §15.2: a `TransportCapabilitiesResponse` with `cancellation` set survives a round trip on both projections at tag 8, and a reader built from a schema without the field ignores it; one encoded without key 8, and one with key 8 null, decode on both projections with `cancellation` unset.
  - Depends on G0 and G1; on TR3.1 (G2), for its rename in candidate-only code and its candidate build; and on contract revision 5, which [Verdict-5](GwzCoreSessionDesign-Verdict-5.md) accepts (C8). Review: dual, the schema freeze.

- **CS1.2 — Frames, the channel contract and the in-process adapter** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/frame.rs` and `src/session_host/channel.rs`, registered in `src/session_host/mod.rs`.
  - Implements §3: the carrier guarantees; a tag byte and a deterministic-CBOR body; tags 1–3 and the reserved 16–31; the frame-level protocol errors (any other tag, an undecodable body, an oversize frame); the in-process adapter's two bounded queues, each holding the outstanding-call limit plus a 64-frame control reserve; a `send` that never blocks and fails with `transport_session_full`; control calls on the reserve; a `recv` that blocks until a frame arrives or the session ends; closure reported to both ends. Also O6.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): the queues are bounded by counters, never pre-allocated with `with_capacity(limit)`.
  - Test-first: §15.10 (`send` on a full queue refuses and never blocks or drops; a frame over 64 MiB ends the session); a reserved-lane tag ends the session; both ends observe closure.
  - Depends on CS1.1 for merge order, and on CS1.4, which creates `src/session_host/mod.rs` (§4); the queues can be built beside CS1.1. Review: dual, the channel-contract freeze.

- **CS1.3 — The byte-stream adapter** *(gwz-core; < 250 lines)*.
  - Files: new `src/session_host/byte_stream.rs`.
  - Implements §3's byte-stream adapter: a little-endian `u32` length before each frame, as taut-shape's interop tool frames it; end of stream is closure.
  - Test-first: frames round-trip against the framing of `taut-shape-rs/crates/taut-shape-tool`; a length prefix over 64 MiB ends the session before anything is allocated; §15.9 (the byte-stream host closes its channel), adapter half.
  - Depends on CS1.2. Review: single-axis, Safety first.

- **CS1.4 — Contexts, gate, token and `open`** *(gwz-core; < 450 lines)*.
  - **Extended by CS1.9 and CS3.7** (reuse §7, §13): CS1.9 adds the host context's bounded `shutdown`, and CS3.7 fills its endpoint-registry member. Each extension is a dual re-freeze (§2.2).
  - Files: new `src/session_host/mod.rs`, `context.rs`, `limits.rs` and `gate.rs`, with their tests in `gate/tests.rs` and `context/tests.rs`; `src/lib.rs`; `docs/RustApi.md`; `scripts/checks/process_globals_allowlist.json` (its accepted object).
  - Implements:
    - §5.6's host context and session context as types, whose members later steps fill;
    - O7, and O8 with the gate's three states: live, cancelled, revoked;
    - §1's limits, and `open`'s validation of them, including that the table holds at least the running plus the queued operations;
    - §9's `open(options)`: limits, snapshot and host context in, the client end of an in-process channel out;
    - how a handler reaches its gate: through the handler's context, which gains the token (§5.2, §16).
  - Test-first: limit validation; after cancellation the gate refuses effectful requests and log appends with `Cancelled`; after revocation it also ignores events and terminals; nothing but the token cancels; dropping a host context ends its supervisor thread once its jobs finish.
  - Depends on G0 and G1. Review: dual, the gate and context freeze, together with CS1.5.
  - Measured at its acceptance ([CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md)): 505 production lines against the aspirational < 450. The excess is its two Safety remediations.

- **CS1.5 — The endpoint environment snapshot** *(gwz-core; < 300 lines)*.
  - Files: new `src/session_host/environment.rs`, with its tests in `environment/tests.rs`; gwz-core's `Cargo.toml`, for the `windows-sys` feature.
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
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): the step records whether handlers get the token through `OperationServices` or through `HandlerContext`.
  - Test-first: §15.4 (a method missing from the table is treated as W); each method's class, transport scope and open-merge command equal gwz-cli's and gwz-py's current answers, or the difference is a recorded decision.
  - Depends on CS1.1 and CS1.4. Review: dual, the dispatch-signature freeze.

- **CS1.7 — Conditional-compilation boundary check** *(gwz-core tooling; < 350 lines)*.
  - Files: new `scripts/checks/check_cfg_boundaries.py`, with its tests and allowlist (`test_check_cfg_boundaries.py`, `cfg_boundaries_allowlist.json`); `scripts/run_tests.py`; and, as [its Verdict-1](GwzCoreSessionCS1.7-Verdict-1.md) accepted them, `scripts/release.py`, `scripts/checks/test_release_boundary.py` and the workflows `release.yml`, `platform-matrix.yml`, `windows-matrix.yml` and `checked-artifact-boundary.yml`. The check covers gwz-core with its `crates/`, gwz-cli and gwz-py's native crate.
  - Implements the root `AGENTS.md` rule on explicit scope. Like `check_process_globals.py`, it is a lexical scan that inspects every platform arm without compiling any. It flags a `#[cfg(...)]`, or a conditional `#[cfg_attr(...)]`, placed directly on a `use` item, another unbraced item or an unbraced statement; §3.0 gives its scope and its counted exclusions. The existing occurrences are listed, and the list only shrinks: about 90 at the planning tuple, almost all `cfg(test)` imports, before statements joined the scope, and 437 under 432 keys at revision 3's tuple.
  - Test-first: a bare `#[cfg(windows)] use …;` in a disabled arm fails; the same import inside a `cfg_if` block passes; a listed occurrence that disappears fails.
  - Closes part (e) of the 1.1.0 amendment round's Safety P2-1, as handed to this plan: every cfg-site edit is checked in its disabled arm.
  - **Recorded at its acceptance** ([its Verdict-1](GwzCoreSessionCS1.7-Verdict-1.md)): its coverage of gwz-cli and gwz-py is local-only. The check covers them only when it runs with gwz-core beside them, until their own CI runs it; wiring that is a follow-up task once CS1.7 is on gwz-core's `main`.
  - No dependencies. Review: single-axis, Consistency first.

- **CS1.8 — gwz-transport's process-global check in CI** *(gwz-core tooling; < 200 lines)*.
  - **Done** with revision 4's application, at root `fa3f44c` and gwz-core `4bd92285`, whose tooling files match the digests Verdict-4 accepts. Revision 4 applied RemPlan-2 §1, and that commit already carries this step's work:
    - `scripts/run_tests.py` fails closed without a gwz-transport checkout, honours `GWZ_TRANSPORT_CHECKOUT`, and prints `SKIPPED GATE` under `--skip-transport-globals`;
    - the gwz-transport allowlist records `reconciled_commit` `46e65a9…`, and `checked-artifact-boundary.yml` checks gwz-transport out at that commit and runs the check, with no skip;
    - `release.yml`, `platform-matrix.yml` and `windows-matrix.yml` pass `--skip-transport-globals`;
    - RemPlan-2 §1's closure tests are `test_run_tests_transport_globals.py` and the `TransportPin` tests in `test_check_process_globals.py`, and they pass at revision 1's tuple.
  - **What remains:** no code. Verdict-4 did not run the GitHub workflows, and in this workspace gwz-core's remote-tracking `origin/main` does not contain `4bd92285`, so the boundary job's first run is still to be observed. The Phase 1 exit observes it. Verdict-4's residual risks stay recorded there.
  - Files: `scripts/run_tests.py`, `scripts/checks/process_globals_allowlist_gwz_transport.json`, `.github/workflows/checked-artifact-boundary.yml`, `release.yml`, `platform-matrix.yml` and `windows-matrix.yml`, and a test beside `scripts/checks/test_run_tests_filesystem_mode.py`.
  - Implements parts 1 and 2 of RemPlan-2 §1: fail closed locally, run the check pinned in gwz-core's CI, and the bump rule. Part 3, the text, stays with the contract.
  - Test-first: RemPlan-2 §1's closure tests.
  - No dependencies. Review: single-axis, Safety, the axis that raised B11.

- **CS1.9 — Host-context shutdown, the off switch's attribute and the snapshot's zeroization** *(gwz-core; < 250 lines)*.
  - The additive re-freeze of CS1.4 with CS1.5 (§2.2). Reuse §7 and §13 extend CS1.4's host context, and server §8 amends the contract's §5.6, which CS1.4 and CS1.5 implement.
  - Files: `src/session_host/context.rs`, `src/session_host/environment.rs`, `docs/RustApi.md`.
  - Implements:
    - the host context's bounded `shutdown`, which disposes what its members hold within one cleanup bound and returns a cleanup report of what remains; dropping a host context without it disposes the same way and reports nothing (reuse §7; §5.6 as amended). Its pending report counts the supervisor's quarantined jobs, which hold their resources for the host context's lifetime (carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md)). The endpoint registry is what it will dispose: CS3.7 fills that member, and from CS7.26 instances outlive their bindings;
    - `open(options)` also takes the off switch's resolved value, `transport_off`, into the session context. The snapshot never carries it, and nothing derives it from the snapshot or from the host process's own environment or configuration (§5.6 as amended; server §5);
    - the snapshot is zeroized when its session ends (§5.6 as amended; server §4).
  - Test-first: `shutdown` returns within its bound, and a second call returns the same report; a quarantined job appears in its pending report; a drop after `shutdown` disposes nothing more; a session opened with `transport_off` reads it from the options, and no snapshot entry of any name sets it; after a session ends, the memory that held its snapshot's values is overwritten, shown through the zeroizing type's own test.
  - Depends on CS1.4 and CS1.5. Review: dual, the additive re-freeze of the gate and context freeze. Not **Ordinary path**: a legacy caller's per-call context (CS3.1) sees none of the three changes.

- **CS1.10 — The server design's schema additions, with CS1.1's extension** *(gwz-core and gwz-py; < 250 lines, generated code excluded)*.
  - **Ordinary path:** the ordinary build's generated protocol and catalogs change: the handshake and control messages, `SessionLimits`, and seven `GwzErrorCode` members, all re-exported at the crate root.
  - The additive re-freeze of CS1.1 (§2.2), and the schema freeze for the server's byte-stream frames.
  - Files: CS1.1's schema, bindings, corpus, `convert.rs`, catalogs and candidate protocol files, `candidate-generator.json` included; not the `CleanupReport` users, which CS1.1's rename alone touches.
  - Implements server §9 and §3: `SessionHello`, `SessionOpen`, `SessionOpened`, `ServerControl`, `ServerState`, `ServerAction`, `EnvironmentEntry`, `ProcessAttributes` and `SessionLimits`, and frame tags 4 to 8, which byte streams use and the in-process adapter does not (§3 as amended, so CS1.2's registry stands); the codes `server_unavailable` (77) to `server_session_closed` (83), each with its schema comment, `convert.rs` mapping and ErrorCatalog entry (§4.2 and §13 as amended).
  - CS1.1's extension (reuse §13): the schema comment and ErrorCatalog entry for `transport_session_full` name its two host-context causes, every endpoint instance bound at the 16-instance bound and the cleanup-owner budget spent, and say that the message names the bound and its value.
  - Test-first: §15.2 (each of the seven codes round-trips through the Rust and Python projections as the same member; each new body round-trips in both languages); CS1.1's lexical test covers the new messages and codes in both protocol files; the candidate build compiles, and the corpus vectors pass under both cfgs. The regeneration moves the candidate's core pin again. Its gwz-core half merges first and its gwz-py half second, core before the driver that pins it (L3-10).
  - Depends on CS1.1, which names the same files, and so on TR3.1 (G2). Review: dual, a schema freeze and CS1.1's re-freeze.

**Exit.** CS1.1–CS1.7, CS1.9 and CS1.10 are merged, and CS1.8 is done (§2.1). The freezes have GO. gwz-core and gwz-py CI is green on macOS, Linux and Windows, and gwz-core's includes the boundary job's first run on GitHub (CS1.8). CS1.5's Windows-only tests run there for the first time, and `case_variants_of_a_name_are_one_entry_for_get_apply_to_and_the_child` shows any disagreement between `CompareStringOrdinal` and std (carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md)). CS1.7's coverage of gwz-cli and gwz-py is still local-only (CS1.7). The candidate build compiles. No driver calls the new code, so there is no further review.

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
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a session context never drops with a live, uncancelled token.
  - Test-first: §15.1 (exactly one reply per call; a duplicate outstanding call ID and a lower one each end the session, and the original call fails with the closed-session error, core half); §15.10 (a frame over 64 MiB ends the session at the host); §15.5 (a call ID above the highest received gets `operation_not_found`). Verdict-3 residual: a call ID below the highest that the session never saw gets `operation_expired` (C5).
  - Depends on CS2.1 and CS1.4. Review: single-axis, Safety first.

- **CS2.3 — Admission** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/host/admission.rs`.
  - Implements §5.1: resolution then admission, on the admission thread, in receipt order and outside the table lock; refusal before any effect; the classes; one FIFO queue of 64; a running limit of 8; queue order per workspace; unique `request_id`s among live operations (§4.3); `transport_session_full` refusals.
  - Test-first: §15.4 (with 64 operations queued the next request is refused with no effect; a W excludes N and W on its workspace while R calls run; a second live operation with the same `request_id` gets `invalid_request`).
  - **Addendum** (revision 3; the server design's amendment of §5.1; §2.2): resolution uses only the request's `InvocationContext` and workspace reference, through core's `caller_directory`. A request that carries `RequestMeta` without `InvocationContext` is refused with `invalid_request` before any effect, and core's `invocation_start` fallback to a supplied start directory serves only legacy direct callers. Test-first: server §12, gwz-core `serve_session` (such a request is refused with `invalid_request`), the in-process half; CS5.4 runs the byte-stream half.
  - Depends on CS2.2 and CS1.6. Review: single-axis, Safety first.

- **CS2.4 — Workers and terminals** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/host/worker.rs`.
  - Implements §5.2: a worker thread per running operation, created when it starts, so a queued operation holds none; a spawn failure settles `Failed` with `internal_error` before start; 8 direct workers; the worker runs the dispatch and reports exactly one terminal through its gate; a handler's panic is caught first and reported `Failed` with cleanup unconfirmed. Also O3, §4.2's reply kinds and §6's projection of terminals into `OperationResult`.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a guard whose `Drop` could cross a gate stays in the worker's frame, outside every closure, since a nesting panic raised from a destructor during unwinding aborts the process; the gate's module doc gains that sentence when this step adds such a guard.
  - Test-first: §15.6 (a handler panic yields `Failed` with `internal_error`, and the next call succeeds); a hook error carrying `member_id`, `target_kind`, `record_context` and `ResponseMeta` arrives unchanged in the `SessionError`. Two obligations moved from 1.1.0 (§5.3): §15.6's row also closes the worker's half of 1.1.0 S6.1's library-safety rule; and, for the first item the 1.1.0 amendment's Verdict-2 carried, with eight latched operations running and 64 queued, the host has started exactly eight operation workers, and a direct call waiting for a direct worker holds no thread.
  - Depends on CS2.3. Review: single-axis, Safety first.

- **CS2.5 — Event logs and results** *(gwz-core; < 400 lines)*.
  - **Ordinary path:** the published `OperationRuntime` changes its overflow rule, and its V0 API is deprecated.
  - Files: new `src/session_host/events.rs`; `src/operation/push_event.rs`.
  - Implements §6: appends through the gate; any number of readers, each with its own cursor; nothing pushed. Also §5.4's event-log ring of 4096 events: on overflow older incremental events are dropped, a reset event is kept, and the result is kept separately. That brings `push_event`, which clears the whole buffer today, to the rule, as §5.4 requires for the V0 `OperationRuntime`. Also `events.subscribe` reads with taut-shape's `LogReadRequest` and `LogReadResponse`, and `operation.result` and `operation.response`, held until the operation is terminal. The V0 `submit`, `subscribe` and `wait` API is marked deprecated (§14; D10).
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a guard whose `Drop` could cross a gate stays in the worker's frame, outside every closure, since a nesting panic raised from a destructor during unwinding aborts the process; the gate's module doc gains that sentence when this step adds such a guard.
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
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a guard whose `Drop` could cross a gate stays in the worker's frame, outside every closure, since a nesting panic raised from a destructor during unwinding aborts the process; the gate's module doc gains that sentence when this step adds such a guard.
  - Test-first: §15.4 (two sessions sharing a host context submit W operations on one workspace, and each completes in turn, core half). Verdict-3 residual: check-and-record is one atomic step, so of two sessions racing a W on one workspace exactly one starts. Verdict-2 residual: a worker's registration is a guard its thread drops on exit or unwind, and that drop is the "worker has ended" signal.
  - Depends on CS2.3. Review: single-axis, Safety first.

- **CS2.10 — Parked log reads** *(gwz-core and gwz-py; < 400 lines)*.
  - **Ordinary path:** both drivers' diff reads use `DiffLog` today, and it now parks held reads and honours a bounded wait instead of probing.
  - Files: gwz-core `src/diff/log_service.rs`; `src/operation/commit_log/handler.rs` (`CommitLogOutputRegistry`); gwz-py `native/src/diff_logs.rs`, for its docstring.
  - Implements §4.2's read verb, with a wait of at most 30 seconds and at most 1 MiB per read, and §5.1's parked reads. Today `DiffLog` blocks one thread per held read on a condvar and degrades bounded waits to probes. A held read becomes a parked waiter, completed by the next gated append, seal or close, by the read timer, or by session end (Verdict-3 residual). The docstrings in `log_service.rs` and gwz-py's `diff_logs.rs` that say a bounded wait degrades to a probe change with it (R8).
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): each record's encoded size counts against `read_bytes`, which is what makes the half-frame bound sufficient.
  - Test-first: §15.10 (1024 reads parked on idle logs, plus a cancel and a close, complete with no host thread blocked and no reply lost); a read with a 1-second wait returns an empty batch after 1 second, not at once. A gwz-py test shows that a one-second bounded read through the native bridge's `diff_log_read` returns after about one second.
  - Depends on CS2.2. Review: single-axis, Safety first.

- **CS2.11 — Session-owned diff and log outputs** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/logs.rs`.
  - Implements §4.2's end-stream verb and §5.4's `diff.output` and `log.output` logs: members of the session context; at most 64 open; the four ways a log is released; the make-room release; `operation_expired` after release; `operation_not_found` for another session. Also §5.6: the registry writes each spool, and a producer never holds a spool's handle. On paths 1 and 2 of §2.1 the make-room rule is RemPlan-2's: the oldest log sealed or closed at least 30 seconds earlier, with no reader stream open, and `operation_expired` after any release.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a closure or cancel callback never crosses or revokes any gate, and a wait happens after its crossing, waking on the token. A closure must not block on another thread's crossing either, which the gate's thread-local rule cannot detect.
  - Test-first: §15.7 (the open-log limit; session end leaves the spool directory empty; another session's read gets `operation_not_found`; 65 sequential diffs read to EOF; 64 streams left open refuse the next open until they end; 65 byte-format diffs never read all succeed; a log with an open reader is never released to make room; a read after release gets `operation_expired`). RemPlan-2's closure tests: 64 closed, unread logs, and the next open releases one; a log sealed within the last second is kept and its first read succeeds; a read after each way of release gets `operation_expired`.
  - Depends on CS2.10 and CS1.4. Review: single-axis, Safety first.

- **CS2.12 — Closure** *(gwz-core; < 450 lines)*.
  - **Extended by CS7.26** (reuse §7, §8, §13): once instances outlive their bindings, an operation's report is its binding's alone, so the close report sums binding reports, and instance disposal is reported through the host context's `shutdown`. The extension is a dual re-freeze (§2.2).
  - Files: new `src/session_host/host/close.rs`; `docs/Embedding.md` and `docs/OperationModel.md`, which now describe the session host.
  - Implements §8: `session.close` steps 1–9; channel closure without close, steps 2–5 and 7; the close bound; revocation; the detached marker (§6); detached workers recorded in the registry; the close report; repeated close. Also O4 and O5.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a session context never drops with a live, uncancelled token; and close latency is measured while a worker emits events in a loop, since the gate's mutex is not fair.
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

**Exit.** Every §15 row assigned to Phase 2 passes in gwz-core CI on macOS, Linux and Windows. The settled tuple is recorded in the program checkpoint (L1-17, L1-31). Review: single-axis, Safety first, on the settled tree, attacking the interleavings of admission, cancellation, release and closure across CS2.2–CS2.12. Users of gwz and of gwz-py's `Client` see no change; CS2.5 and CS2.10 are on the ordinary path (§3.0).

### Phase 3 — Network operations and session isolation (milestone: network operations run in the host, each on its own binding, which the host context's endpoint registry gives it for the endpoint configuration derived from the session's environment and timeouts)

The session path reads no environment variable and no process-global mutable state, except `permanent` entries and the legacy adapter's `debt` entries. The transport route is proven on the three platforms. The model is the reuse design's, which amends §1 and §5.2: one binding per operation, over endpoint instances the host context shares. Until CS7.26 turns sharing on, the registry gives each binding a private instance and disposes it when the binding finishes. So in this phase an operation costs what today's per-operation runtime costs, and no operation uses another's connection before revalidation, per-owner accounting and fault isolation exist (reuse §4, §5, §8). Phase 7 changes the registry behind CS3.7's seam, not the entry.

CS3.1 and CS3.3–CS3.6 need only Phase 1 and TR3.1 (G2), so lane C can run beside Phase 2. CS3.2, CS3.7 and CS3.8 also wait on revision 3's GO (TR1.4b).

- **CS3.1 — The legacy adapter** *(gwz-core, gwz-cli and gwz-py; < 200 lines)*.
  - Files: new `src/session_host/legacy.rs`, which also builds the crate's default context for gwz-core's public handler API; the allowlists that list the entries' environment reads, gwz-core's `scripts/checks/process_globals_allowlist.json` and gwz-py's `scripts/process_globals_allowlist.json`; and its callers, the legacy entries:
    - the two candidate-only transport entries: `with_local_transport` in `src/transport_host/local_command.rs`, which gwz-cli calls, and `TransportRuntime::from_environment` in `src/transport_host/mod.rs`, which gwz-py's candidate `TransportSession` calls until Phase 4 removes it (§5.3);
    - in both builds, gwz-cli's `execute_with_backend` in `src/globalargs/dispatch.rs`, and gwz-py's backend scope in `native/src/shims.rs`.
  - Implements the transition rule, in both builds. Until its driver moves (CS4.7, CS6.4), every driver entry that reaches a handler builds a **per-call legacy context**. Its per-call part covers the environment and the member locks only: the live environment, read at the entry, and a member lock manager that serves that call only. The process-wide timeout applies as today. The entries are those above and gwz-core's public handler API, for which the crate builds the context by default. Nothing on the session path uses it. No handler takes a member lock today, so a direct caller's locking is unchanged.
  - **Budgets stay process-wide on the legacy path.** The setup-job cap with its supervisor (`HUB`, `INIT`, `COUNT`), the cleanup permits (`CLEANUPS`) and the `gh` slots (`SLOTS`) stay process-wide there, as today's statics are, and stay listed `debt` until the legacy path goes (CS4.8, CS6.5). The session host's host context owns its own budgets (CS3.5, CS3.6). This keeps gwz-py's candidate `TransportSession` on per-process budgets, as today: per-call budgets would give each legacy call, and each `TransportSession`, bounds of its own.
  - The environment read at each entry is `debt` where it lives: in gwz-py's allowlist for its backend scope, which CS4.8 removes, and in gwz-core's for the crate default, which CS6.5 removes with the adapter's other entries. gwz-cli carries no process-global check, so its read at `execute_with_backend` is named here, and CS6.1 removes it.
  - Test-first: characterization (L1-03): gwz-cli's and gwz-py's suites pass unchanged through the adapter, in the ordinary build and on the candidate build.
  - Depends on CS1.4, CS1.5 and CS1.6, and TR3.1 (G2). Review: single-axis, Consistency first.

- **CS3.2 — Endpoint configuration from the snapshot** *(gwz-core; < 450 lines)*.
  - Files: new `src/transport_host/endpoint_config.rs`; `src/transport_host/mod.rs` (`SshEndpointConfig::from_environment`) and `src/transport_host/local_command.rs` (`environment_config`, `tls_config`), whose reads move into the legacy adapter (CS3.1); `src/git/gitbackend/transport_support/identity.rs` (`~/` identities); `crates/repo-inspect/src/environment.rs` (`Environment::Process`). `src/git/gitbackend/transport_binding.rs` is not converted: CS7.24 retires its lazy endpoint instead (reuse §11).
  - Implements §5.6: each network operation derives its endpoint configuration from the snapshot and the session's timeouts; an invalid proxy or CA setting refuses only network operations, before any instance is selected; a CA file is read when the configuration is derived. Also §5.7's environment-read rows, which reuse §3's derivation and §11's retirement close, and §5.8's scoping of libgit2's own reads. The configuration is reuse §3's (reuse C1):
    - its fields, in canonical order: the format version and core build; the SSH home (`HOME`, absolute; on Windows `USERPROFILE` when `HOME` is unset); the known_hosts path; the agent source (`SSH_AUTH_SOCK` when non-empty, its absence a value; on Windows, once 1.1.0 S4.3's agent channel and S4.5's arms have landed, the session's one agent source as kind and address, server §5); the CA certificates, at most 1 MiB, reduced to their `CERTIFICATE` blocks; the proxy's host, port and TLS flag; the no-proxy list, lower-cased, sorted and deduplicated; the stall and aggregate, the aggregate 0 when the stall is 0;
    - left out: the `gh` environment, applied per request (CS7.15); capacity, per operation (CS7.10); identity and HTTPS policy, which travel in each Open; the pool's constants;
    - no secrets: a proxy URL with credentials, and a configuration carrying proxy authorization, are refused; only certificate blocks are kept; the configuration is never logged, serialized or observed (reuse §3, §12);
    - equality: the deterministic-CBOR encoding of the fields, compared byte for byte, absent distinct from empty, with no path normalization; an index by its SHA-256 compares the full encoding on a hit;
    - a conversion to the inputs today's runtime is built from, which CS3.7's private instances use.
  - Not **Ordinary path**: a direct caller's `~/` identities and repo-inspect reads take its per-call legacy context's live environment (CS3.1), as today, and the rest is candidate-only.
  - Test-first: §15.8 (after `open`, changing `HOME`, `SSH_AUTH_SOCK` or `GIT_SSL_CAINFO` leaves a later operation's endpoint configuration equal to the captured one, on an `ssh://` or `https://` remote, with the documented exception asserted for an `http://` remote, per RemPlan-2's Consistency P3-29; with an invalid proxy in the snapshot, local-only operations succeed and a network operation is refused; the error carries no environment value); reuse §15.4, the configuration half (a differing HOME, agent, CA content under one path, proxy or timeout gives unequal configurations; no-proxy lists that differ only in order and case give equal ones; a configuration with proxy authorization is refused; a CA file holding a private-key block keeps only its certificates). The `identity.rs` and repo-inspect `debt` entries leave the allowlist, and the `local_command.rs` and `mod.rs` entries move into the legacy adapter (§5.4).
  - Depends on CS3.1. Review: dual, the endpoint-configuration freeze: its encoding decides which operations may share a connection.

- **CS3.3 — Session-path child processes** *(gwz-core; < 300 lines)*.
  - **Ordinary path:** for both drivers, `git tag`, `git commit`, the local-import `git fetch` fallback and `git rev-list` run with `env_clear()` plus the environment captured at the entry, instead of inheriting the live one.
  - Files: `src/git/gitbackend/refs.rs`, `repository.rs` and `transport.rs`; `src/operation/commit_log/mod.rs`.
  - Implements §5.6's child processes and §5.7's spawn row: `git tag` and `git tag -d`, `git commit`, the local-import `git fetch` fallback and the commit-log `git rev-list` are spawned with `env_clear()` plus the snapshot. A direct caller's children get its per-call legacy context's environment (CS3.1).
  - Test-first: §15.8 (on POSIX, a value containing a byte that is not valid UTF-8 reaches a session-path child unchanged; after `open`, changing `GIT_CONFIG_GLOBAL` leaves a later commit's committer identity unchanged). Characterization (L1-03): gwz-cli's and gwz-py's ordinary-build suites pass unchanged, with a configured credential helper and `GIT_CONFIG_GLOBAL` set. The four spawn entries stay `debt` and name CS6.5, which flips them to `permanent`, each citing its test, once the legacy context is gone.
  - Depends on CS3.1. Review: single-axis, Safety first.

- **CS3.4 — Core's own `git credential fill`** *(gwz-core; < 450 lines)*.
  - **Ordinary path:** both drivers' HTTP credential lookup runs core's own `git credential fill`, which needs `git` on `PATH` (R8).
  - Files: `src/git/gitbackend/transport_support.rs` (the credential callback); new `src/git/gitbackend/credential_fill.rs`; `scripts/checks/check_process_globals.py` and its tests.
  - Implements §5.8's credential-helper rule and §5.7's `Cred::credential_helper` row. On paths 1 and 2 of §2.1 the mechanism is RemPlan-2's (Safety P3-29): `env_clear()` plus the snapshot without `GIT_ASKPASS` and `SSH_ASKPASS`, with `GIT_TERMINAL_PROMPT=0` and `-c credential.interactive=false`; killed on drop, bounded in time and killed at the bound like the `gh` helper, and killed when the token is cancelled or the gate revoked; output treated as secret, with only `username` and `password` read, other lines tolerated and never logged; `approve` and `reject` never called. A missing `git` executable reads `external_tool_missing`. The spawn takes no `gh` slot. For a direct caller, the spawn uses its per-call legacy context's environment (CS3.1).
  - Test-first: §15.8 (in both builds, the helper a later HTTP fetch runs is the one the snapshot's configuration names, and it sees the snapshot, even after `HOME` changes; a new `Cred::credential_helper` call fails gwz-core's check). RemPlan-2's closure tests: with `GIT_ASKPASS` naming a recorder and no helper configured, an HTTPS fetch fails with an authentication error and the recorder never runs; a helper that sleeps is killed when the operation is cancelled; a fault-injected `CredentialHelper::new(url).execute()` is flagged as `Cred::credential_helper` (Safety P3-31). Characterization (L1-03): gwz-cli's and gwz-py's ordinary-build suites pass unchanged, with a configured credential helper and `GIT_CONFIG_GLOBAL` set. The `Cred::credential_helper` `debt` entry goes. The new `git` spawn is listed `debt` and names CS6.5, which flips it to `permanent` once the legacy context is gone. After this step, `check_process_globals.py` over gwz-core and gwz-py lists the legacy environment reads and the spawn entries as `debt`, each naming its remover.
  - **Addendum** (revision 3; the server design's amendment of §5.6; §2.2): the `git credential fill` spawn gets an explicit working directory, `/` on POSIX and the system drive's root on Windows, as the `gh` helper's is, so it never inherits the host process's directory and, like today's `git2::Config::open_default()` lookup, reads no repository's local configuration (C10). CS3.3's spawns already set theirs to the repository they act on (`current_dir` in `repository.rs`, `refs.rs`, `transport.rs` and `commit_log/mod.rs`). Test-first: with the process's own directory inside a repository whose local configuration names a recording helper, the helper that runs records the root as its directory, and the recording helper never runs.
  - **Working rule** (C11; [TR1.4b's Verdict-2](GwzCoreSessionPlan-Verdict-2.md), S-P3-6): the spawn's environment also drops `GIT_DIR`, `GIT_COMMON_DIR` and `GIT_WORK_TREE`, as it drops the prompt hooks. `git credential fill` honours an explicit `GIT_DIR` from any directory, so without the drop a snapshot's `GIT_DIR` would make it read that repository's local configuration, which today's lookup never reads. Test-first: with the snapshot's `GIT_DIR` naming a repository whose local configuration names a recording helper, and no helper in the global configuration, an HTTPS fetch fails with an authentication error and the recorder never runs; the §15.8 `GIT_CONFIG_GLOBAL` row still passes.
  - Depends on CS3.1 and CS1.5. Review: dual. It changes secret handling, and it changes credential lookup for both existing drivers.

- **CS3.5 — The SSH setup supervisor in the host context** *(gwz-core; < 450 lines)*.
  - Files: `src/git/endpoint/agent_job.rs` (769 lines: split first if the change would pass 1,000).
  - Implements §5.6's host-context member: the session host's host context owns its own SSH setup supervisor, with its helper and cleanup budgets, stopped when the host context drops and its jobs have finished. Also §5.7's supervisor row and §14's change from per-process to per-host-context scope. The legacy path keeps today's process-wide supervisor and budgets (`HUB`, `INIT`, `COUNT`, `CLEANUPS`) until it goes (CS3.1), so gwz-py's candidate `TransportSession` stays on per-process budgets, as today.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a supervised job releases its permit in `Drop`, not only in `poll`, so the release when the supervisor ends is real.
  - Test-first: §15.9, core half (two sessions sharing a host context share one supervisor and one helper budget; two host contexts have two); the supervisor thread ends after its host context drops. The four statics stay listed `debt`, for the legacy path only, and name CS6.5, which removes them after CS4.8.
  - Depends on CS3.1 and CS1.4, and TR3.1 (G2). Review: single-axis, Safety first.

- **CS3.6 — HTTPS helper slots and `AuthOwner` cleanup** *(gwz-core; < 400 lines)*.
  - **Extended by CS7.16** (reuse §6, §13): a request awaits one of the host context's helper slots within its own helper budget, cancellably, and helper children belong to the instance's `AuthOwner`. The extension is a dual re-freeze (§2.2).
  - Files: `src/git/endpoint/https_auth.rs` (934 lines: split first, in a movement-only commit).
  - Implements §5.6's last paragraph and §5.7's HTTPS row: the session host's host context owns its own slot budget; the endpoint's `AuthOwner` owns helper cleanup, replacing `ORPHANS` and `ORPHAN_REAPING`. The legacy path keeps today's process-wide `SLOTS` until it goes (CS3.1), so gwz-py's candidate `TransportSession` stays on per-process slots, as today.
  - Test-first: two sessions sharing a host context share its 8 HTTPS helper slots; an orphaned helper is reaped by its owner, with no process-wide registry. The `ORPHANS` and `ORPHAN_REAPING` `debt` entries go. `SLOTS` stays listed `debt`, for the legacy path only, and names CS6.5, which removes it after CS4.8.
  - Depends on CS3.1 and CS1.4, and TR3.1 (G2). Review: single-axis, Safety first.

- **CS3.7 — The session transport entry and the binding seam** *(gwz-core; < 450 lines)*.
  - The re-freeze of CS1.4 that fills the host context's endpoint-registry member (§2.2), and the seam that Phase 7 builds behind.
  - Files: new `src/transport_host/session_entry.rs` (`local_command.rs` stays the legacy entry); new `src/transport_host/registry.rs`, the registry's first form; `src/session_host/context.rs`, whose transport member it fills inside the `cfg_if` boundary that gates `transport_host`, beside the ordinary build's counterpart in the same block (reuse §14); `src/transport_host/request.rs`.
  - Implements §5.2's transport entry, and §5.5 and O5, as the reuse design amends them, with §16's core API changes. Through its gate, a worker derives the operation's endpoint configuration (CS3.2); obtains a binding with the operation's limits from the host context's registry, within the admission deadline; registers the operation's single request with its token, through a new request constructor that takes the caller's token; runs the handler; finishes the request; and releases the binding. The gate reaches the registry for binding creation (§5.6 as amended). Registration fails with `Cancelled` when the token is already cancelled, including while the binding is being obtained, and the token is the only cancellation authority. The request's transport deadlines act inside its binding (O5 as amended). Also §5.3 for network I/O.
  - **The registry's first form.** Each binding gets a private instance, built from its configuration as today's runtime is built, and the registry disposes it when the binding finishes. The operation's cleanup report is the binding's report with that disposal, as `Command::finish` combines them today, and the private instance's `gh` helper takes the session's snapshot through the gate. The registry shares nothing until CS7.26, so this form costs what today's per-operation runtime costs.
  - **The panic path** (1.1.0 S6.1's library-safety rule, the entry's half). The entry catches the handler's panic first. It then finishes the request and releases the binding under their own `catch_unwind`, and returns the failure with cleanup unconfirmed. It never finishes from `Drop` while unwinding: a guard dropped during an unwind only cancels its request and marks its binding abandoned, and the registry disposes it outside the unwind, at its next call or at the host context's `shutdown` or drop. So in a host context that makes no further network call, an abandoned binding's private instance keeps its threads and connections until that `shutdown` or drop, bounded by the host context's life. Today's `Command` in `local_command.rs` is not the model, because its `Drop` (lines 70–74) calls `finish`, which blocks on the request's finish and the runtime's shutdown with no check for unwinding. A handler panic there finishes from `Drop` during the unwind, and a second panic aborts the process (transport plan TR1.4b). That legacy entry serves only gwz-cli's candidate process until CS6.5 removes it, and never the session path.
  - The entry's arms sit in `cfg_if` blocks for unix and windows; whichever of this step and any 1.1.0 cfg-site change lands second adapts, checked by CS1.7 and on dabeest.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): cancel callbacks are non-blocking and cannot report through a gate: the request registration signals the transport without a crossing, and the worker reports the cancel at its next crossing. As for CS2.11, a closure or cancel callback never crosses or revokes any gate, and a wait happens after its crossing, waking on the token. A closure must not block on another thread's crossing either, which the gate's thread-local rule cannot detect.
  - Test-first: §15.5 (an operation blocked opening a stream to a latch-held endpoint is cancelled through its token, and the cancel returns within the admission deadline with a cleanup report while the handler's I/O fails with `Cancelled`; a token cancelled while its binding is being obtained fails registration, runs no handler and settles `Cancelled`); §15.6 (a fault-injected panic in finish after a handler panic yields `Failed`, and the next call succeeds with the process alive). 1.1.0 S6.1's unit tests (§5.3): a cancel before the start and a cancel while running each return their cleanup report. A guard dropped during an unwind never calls finish: a test finish that fails when it runs under `std::thread::panicking()` is never reached. The registry's next call disposes an abandoned binding's private instance, and its threads end. Two overlapping operations with equal configurations get two private instances and share no connection.
  - Depends on CS3.2, CS1.4 and CS1.9, and on CS3.5 and CS3.6, whose host-context budgets its private instances draw on. 1.1.0 S6.1 is retired as a step (§5.3), so this step builds its token-taking constructor. Review: dual, the binding seam and CS1.4's re-freeze. Not **Ordinary path**: candidate-only code.

- **CS3.8 — Session timeouts** *(gwz-core; < 300 lines)*.
  - Files: `src/session_host/context.rs` (the timeouts), `src/session_host/dispatch/read.rs` (`configure_transport_runtime`).
  - Implements §5.5 as the reuse design amends it, §5.6's timeouts paragraph, and §4.2's per-session `configure_transport_runtime` in transport builds: it applies to operations admitted after its reply and never touches another session or a running operation. The session's timeouts enter the endpoint configuration (CS3.2), so sessions with different timeouts never share an instance, and a session's timeouts never govern another's connection (reuse §3; its §16.1 decision 7). Ordinary builds keep today's process-wide behaviour, and `TIMEOUT_STATE` stays `permanent` (§5.8).
  - Test-first: §15.8 (two sessions: A sets a zero timeout, and B's next operation keeps B's own deadline, and the two sessions' operations derive unequal configurations). The 1.1.0 amendment round's Safety P3-6, as handed to this plan: a session opened without `configure_transport_runtime` carries the accepted clocks, the 9-second stall and 30-second aggregate the retry plan sets as the CLI's defaults (its §1), asserted on the session context (D8); 1.1.0 S3.3's production-graph stall regression, one idle stage expiring with reason `stall` while the aggregate is ahead, passes through the session entry.
  - Depends on CS3.7, and on CS2.13, which creates `dispatch/read.rs` (§4). Review: single-axis, Consistency first. Not **Ordinary path**: the ordinary build keeps its process-wide timeout.

- **CS3.9 — Member locks and push serialization** *(gwz-core; < 400 lines)*.
  - **Ordinary path:** the fetch and push handlers that both drivers run take member locks.
  - Files: `src/operation/membermutationguard.rs`, `src/workspace_ops/handle_fetch.rs`, `src/workspace_ops/push_member.rs`, `src/session_host/host/admission.rs`.
  - Implements §5.1's serialization and §16's handler change. The member lock manager moves into the host context; it blocks instead of refusing (today's `MemberLockManager::try_lock` refuses), recovers from poisoning, and wakes a wait on cancellation, since the wait is a gate crossing. Each fetch or push member step takes it. A push also takes the workspace mutator lock, and at most one push runs per workspace. A direct caller's handler takes the member lock manager of its per-call legacy context (CS3.1), which serves that call only.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a closure or cancel callback never crosses or revokes any gate, and a wait happens after its crossing, waking on the token. A closure must not block on another thread's crossing either, which the gate's thread-local rule cannot detect.
  - Test-first: §15.4 (two pushes on one workspace in one session complete one after the other with no `UnsupportedOperation`, while a fetch and a push run concurrently and so do two fetches; two fetches touching one member serialize that member's step); §15.9 (after revocation the next member-lock request fails with `Cancelled`, member half). Characterization (L1-03): gwz-cli's and gwz-py's ordinary-build suites pass unchanged, with a configured credential helper and `GIT_CONFIG_GLOBAL` set.
  - Depends on CS2.8, CS2.9 and CS1.4. Review: single-axis, Safety first.

- **CS3.10 — Network operations through the host** *(gwz-core; < 300 lines)*.
  - Files: `src/session_host/host/worker.rs`, `src/session_host/dispatch/read.rs`.
  - Implements §5.2's choice of entry: transport-scope requests use the session entry (CS3.7), and Phase 2's refusal goes. Otherwise the handler runs with the default backend. A session whose `transport_off` is on (CS1.9) runs every transport-scope request with the default backend, the native path (server §5); the switch's forms, precedence, notice and reporting are TR1.5's and TR2.5's. Also §5.8's ordinary-build behaviour, where a cancel waits for completion, and §13's `cancellation` field: in a build with the transport, the session route's `transport_capabilities` reply sets it true, and an ordinary build leaves it absent, which reads as unsupported (§5.8, §13).
  - Not **Ordinary path**: the ordinary build's reply is unchanged, and the field is set only on the session route, which no existing user reaches before CS4.7 on the candidate build and activation on the normal builds; gwz-cli has no `transport_capabilities` command. C1 records what the field cannot yet say for the native routes.
  - Test-first: §15.4 (eight overlapping fetch and push operations complete independently, and a ninth waits in the queue and then runs, core half); §15.5 (a cancelled running fetch shows `failed` with `errors[0].code == cancelled`, core half); §15.2 (the session route's reply carries `cancellation` at tag 8 in a transport build, and no key 8 in an ordinary build); §15.15, in the build configuration D7 settles; a session opened with `transport_off` sends a fetch down the native path that the same session without it sends through the transport, as the result's transport observations show.
  - Depends on the Phase 2 exit, CS3.5–CS3.9 and CS1.9. Review: single-axis, Safety first.

- **CS3.11 — Route proof** *(evidence; < 300 lines of fixtures)*.
  - Files: gwz-core test fixtures; raw runs in gwz-core-evidence, with a redacted public report.
  - Implements the route evidence for the session entry. On macOS ARM64, Linux x86-64 and dabeest, SSH and HTTPS clone and fetch run through the session host against the 1.1.0 disposable fixtures. A typed assertion shows the transport route: `transport_capabilities.cancellation` and the transport observation the result carries. A `gh` failure and an unsupported proxy still refuse. The cost of a binding per operation, and connection counts with sharing, are Phase 7's to measure (CS7.27), since before CS7.26 each binding's private instance costs what today's runtime costs.
  - Test-first: a build with the session entry's Windows arm removed fails the route test instead of passing natively. This closes the 1.1.0 amendment round's A3 for the session entry; its Safety P3-4 goes to CS7.27.
  - Depends on CS3.10. Review: part of the Phase 3 exit. Not **Ordinary path**: fixtures and evidence.

**Exit.** Every §15 row assigned to Phase 3 passes: in CI on macOS, Linux and Windows for rows without a network fixture, and on macOS, Linux and dabeest for transport rows. gwz-core's allowlist holds no `debt` entry outside the legacy path: the adapter's own entries, the spawn entries CS3.3 and CS3.4 keep, and the process-wide budgets the legacy path keeps, all of which CS6.5 removes; and the lazy endpoint's environment read in `transport_binding.rs`, which the session path reaches only if the transport-scope predicate misses a method (CS1.6), and which CS7.24 retires. Review: dual, because CS3.4 changes what existing CLI and Python users see. The `cancellation` field reaches no existing user in this phase (CS3.10).

### Phase 4 — gwz-py on the session host (milestone: `NativeCoreBridge` is the thin client of §10 and the public API is unchanged)

gwz-py's suite passes through the session on CPython 3.10–3.13 on macOS, Linux and Windows, and the transport plan's restated S6.3 rows pass through it on the three platforms (§5.3): CS4.7 runs those that need no reuse, and CS4.9 the rest once Phase 7 has turned sharing on. gwz-py's process-global debt is gone, and `TransportSession` with it.

CS4.1 and CS4.3–CS4.6 work against a fake channel, so lane D can start after Phase 1, once CS1.10 gives CS4.3 the closed-session code. The phase's exit follows CS4.8, as in revision 2, so that no step of Phases 5, 6 and 8 waits on Phase 7 through CS4.7 and CS4.8. CS4.9 follows both the exit and the Phase 7 exit, and carries the phase's Surface review of reuse (transport plan TR1.7).

- **CS4.1 — Assertions that change** *(gwz-py; 0 product lines)*.
  - Files: none in the product or its tests; the retaken list is filed with the step's review package, and CS4.6 rewrites the tests it names.
  - Implements §12's requirement that this plan list the assertions that change. The list below was taken at gwz-py `4ad2b077`. 1.1.0 S6.2 and S6.3 are retired and never rewrote the transport-session tests, and no 1.1.0 tag precedes this plan's phases (§2.3). So this step retakes the list against gwz-py's main when it starts, and files it before any test changes; CS4.6 retakes it if those tests change in between. Known changes:
    1. A cancel naming a foreign or unknown operation: `InvalidRequest` becomes `operation_not_found` (`test_cancel_uses_public_operation_id_and_rejects_foreign_identity`). The contract names this one.
    2. A cancel naming an expired or older completed operation: `InvalidRequest` becomes `operation_expired`, and the "latest completed cancellation snapshot" becomes each terminal record's retained report (GwzPyTransportDesign §4, superseded by the contract's §14).
    3. Capacity refusals read `transport_session_full` (§5.1) instead of the candidate session's `TransportCapacityConflict`, which the session host never produces (§4.2 as the reuse design amends it; CS7.10) (`test_typed_capacity_refusal_reaches_python_without_message_parsing`).
    4. The host assigns operation IDs at receipt (§4.3), and `reserve_operation` leaves the bridge's path (§9). Identity tests (`test_public_identity_is_issued_once_before_native_work` and the identity tests in `test_transport_session_native.py`) instead assert that the accepted response's `operation_id` is the one events and results carry.
    5. A cancel before the worker starts becomes a cancel by `call_id` before the accepted reply (§15.5; `test_cancel_before_default_executor_starts_worker_reaches_reserved_operation`).
    6. The serialization tests (`test_native_bridge_does_not_serialize_independent_network_calls`, `test_legacy_module_keeps_its_network_call_serialization`), and GwzPyTransportDesign §5's construction-barrier, admission-barrier, overlapping-direct-native-call and Python-lock tests, are replaced by §15.4's rows. Its physical-session-reuse test is replaced by reuse §15.3's row through gwz-py (CS4.9), as reuse §13 amends the contract's §14.
    7. Event waits (`test_native_bridge_uses_wait_events_when_available`) become `events.subscribe` reads against a fake channel.
    8. `test_result_wait_releases_the_gil_for_a_failing_worker` becomes: `recv()` releases the GIL, and a failing worker, now a core thread, needs no GIL.
    9. An undeclared method (`test_native_bridge_routes_unsupported_methods_explicitly`) raises a `GwzBridgeError` with the code C4 settles, instead of the protocol error "unsupported gwz-core method".
    10. Tests that call the module-level native functions (`call`, `submit`, `wait_events`, `operation_result`, `try_operation_result`, `merge_operation_response`, `diff_log_read`, `log_output_read` and the rest) move to the bridge, or go with those functions (D6).
    11. `test_process_globals.py` expects an allowlist with no `debt` entry (CS4.8).
    12. The typed closed-session error, which every outstanding call gets when the session ends, has the code `server_session_closed` (83) on `NativeCoreBridge` (§10 as the server design amends it); a test that asserts another code for it changes.
  - Unchanged in meaning, with a new mechanism: close and repeated close return the retained report; repeated task cancellation waits for cleanup; the caller's directory travels in each request (`test_native_submit_keeps_serialized_caller_context_after_cwd_changes`); an unknown operation reads `operation_not_found`; the restated S6.3 rows (§5.3).
  - Test-first: none of its own. The list is its output, and CS4.6 writes each row's test, failing first.
  - Depends on G1 alone; CS4.6 retakes the list if the tests change first. Review: with CS4.6. Not **Ordinary path**: it changes no file.

- **CS4.2 — The extension's channel** *(gwz-py; < 350 lines)*.
  - Files: new `native/src/session.rs`; `native/src/lib.rs`.
  - Implements §9: the `HostContext()` constructor, whose host context holds the endpoint registry in a transport build (CS3.7); `open(options)` with limits, the snapshot as byte-string pairs, the host context and the off switch's resolved value (`transport_off`, CS1.9); a non-blocking `send` that fails with `transport_session_full` on a full queue and fails once the session has ended; a `recv` that blocks with the GIL released and returns `None` once the session has ended; `close()` and drop close the channel. Nothing else crosses, and the extension reads neither the environment nor the working directory.
  - Test-first: §15.10 (`send` on a full queue raises `transport_session_full` and never blocks or drops, Python half); another Python thread runs while `recv()` blocks. The step adds no allowlist entry.
  - Depends on CS1.2, CS1.4 and CS1.9; its integration tests need CS2.4 and CS2.13. Review: single-axis, Safety first. Not **Ordinary path**: nothing reaches the channel before CS4.7.

- **CS4.3 — The bridge's call table and pump** *(gwz-py; < 450 lines)*.
  - Files: `src/gwz/bridge.py`, and a new private module for the pump and the call table (`bridge.py` is 643 lines).
  - Implements §10's Calls, The pump, and Waiting and bounds:
    - call ID allocation, waiter registration and `send` under one lock;
    - one daemon pump thread per session, completing each reply on its issuing loop with `call_soon_threadsafe`;
    - a reply for a closed loop is dropped, except a result or response reply, which the bridge keeps in a view bounded by the operation-table size, oldest dropped first;
    - on paths 1 and 2 of §2.1, a kept result leaves the view on release, or when `operation.release` reports `operation_expired` (RemPlan-2's Safety P3-30), not when a late cancel does;
    - an undecodable payload fails only its call; `recv()` returning `None` fails every outstanding call with the closed-session error; any pump exception closes the channel, marks the bridge closed and fails every outstanding call. The closed-session error's code is `server_session_closed` (83), on this bridge as on the others (§10 as the server design amends it);
    - two thread-safe counters, for ordinary calls and control calls, with waits on the caller's own loop.
  - Test-first: §15.11 (an injected undecodable frame fails every outstanding call and later sends; one undecodable payload fails only its call); §15.12 (calls from different loops complete on their own loops; with the issuing loop closed before its result reply, `operation_result` from another loop returns the result; 1000 cancelled result waits with no release leave at most the table size in the view); §15.1 (a call whose client stops waiting still completes and its reply is dropped, except a result or response reply, per RemPlan-2's Consistency P3-28); RemPlan-2's Safety P3-30 test (after a cancelled result wait, the host's eviction and a `cancel_operation` reporting `operation_expired`, `operation_result` still returns the result). The bridge's side of the 1.1.0 amendment's Verdict-2 first carried item (§5.3): with the ordinary-call counter at its limit, a further call waits on its own event loop, holding no thread, and proceeds when a reply frees a slot, while a control call never takes an ordinary slot.
  - Depends on CS1.1, CS1.2 and CS1.10. Review: single-axis, Safety first. Not **Ordinary path**: the bridge's public path switches only at CS4.7.

- **CS4.4 — Bridge methods and error mapping** *(gwz-py; < 400 lines)*.
  - Files: `src/gwz/bridge.py`, `src/gwz/errors.py`.
  - Implements §10's mapping table and its error rule: a `SessionError` becomes a `GwzBridgeError` with code, member ID, member path, target kind, detail and record context from `GwzError`, `machine_message` from `GwzError.message`, and `response_meta` from `ResponseMeta`. Every request carries the caller's directory, as it already does.
  - Test-first: §15.2 (each code arrives as the same member in Python, the server design's seven included; a queue-full refusal reads `transport_session_full`); §15.3 through `NativeCoreBridge`.
  - Depends on CS4.3. Review: single-axis, Consistency first. Not **Ordinary path**, for CS4.3's reason.

- **CS4.5 — Task cancellation, exit, host context and environment** *(gwz-py; < 400 lines)*.
  - Files: `src/gwz/bridge.py` and the pump module; `src/gwz/client.py` (stream helpers only; a step that grows it reviews it for cohesion first, §3.0).
  - Implements:
    - §10's task cancellation: cancelling a unary call sends `operation.cancel` for its `call_id`, waits shielded for the reply, then propagates, and `operation_expired` raises nothing; cancelling a read stops only that read; the Client's stream helpers deliver the result or release the record in `finally`;
    - **the exit path** (the reuse design's Verdict-1 carries it here). One process-wide finalizer, registered at the first open, closes every open session's channel at once and joins their pumps before finalization, so exit waits at most one close bound in all, however many Clients are open (C9). It then releases the bridge's reference to its default host context, whose drop disposes the host context's endpoint instances within the cleanup bound and reports nothing (reuse §7). A host context that another object still holds is left to process exit, which ends its threads and closes its sockets. Nothing at exit joins a detached worker, or setup work left under the supervisor, which process exit ends (§16). Exit therefore waits at most the close bound plus the cleanup bound. The close bound is the session host's own timer (O5, §5.5), which `configure_transport_runtime` cannot change, so the bound holds when a session has turned its transport deadlines off (the 1.1.0 amendment's Verdict-2, second carried item);
    - the per-process host context, created once under a process-wide lock at the first open; `NativeCoreBridge` gains an optional host context to share, the one Python-visible addition, and `Client` is unchanged;
    - **fork.** After `fork`, the child forgets the inherited default host context and every inherited session, through `os.register_at_fork`. It retains them, unreachable from the bridge, and never closes or drops them: their Rust owners are leaked (`mem::forget`), since dropping would join a thread the child lacks and shut down a socket the parent shares (reuse §7, which corrects this step's "drops"). A Client opened there creates its own host context;
    - the environment captured at `open`: `os.environb` on POSIX, and `os.environ` encoded as WTF-8 on Windows, copied in one step under the GIL, so that a concurrent change to `os.environ` can neither tear the snapshot nor fail the open. The extension reads no environment (CS4.2), so no C-level read races Python's `putenv` on the session path; CS4.8 removes the legacy path's live read (the 1.1.0 amendment's Verdict-2, third carried item);
    - the off switch's resolved value, passed to `open` as `transport_off`: false until TR2.5 resolves it for a library client as TR1.5 designs it.
  - Its documented changes, for the Phase 4 Surface review (TR1.7): exit waits up to the close bound plus the cleanup bound; the environment is captured when a Client opens its session, and a change made while it opens may or may not be seen; a forked child's Client has its own host context; `NativeCoreBridge`'s optional host context.
  - Test-first: §15.12 (a cancelled unary call cancels and joins); §15.2 (a cancel racing its unary reply raises nothing); §15.7 (a cancelled result wait followed by a retry returns the result; 200 streams abandoned early do not exhaust admission); §15.9 (32 Clients opened at once from 32 threads share one host context); §15.10 (1024 outstanding unary calls whose tasks are all cancelled: every cancel is delivered and answered); §15.11 (a script that exits without close exits 0 without aborting on CPython 3.10–3.13, on Linux, macOS and, beyond the contract, Windows, C6). Verdict-3 residuals: on Windows, a value with an unpaired surrogate reaches a session-path child unchanged; after `fork` the child forgets its default host context, so a Client opened there creates its own, and a child that opens a Client and exits leaves the parent's session working, neither joining a missing thread nor shutting down a parent's socket (Linux and macOS; reuse §15.13's pooled-connection half is CS4.9's).
  - Also test-first, the Verdict-2 carried items: with a close bound of 2 seconds set at open and `configure_transport_runtime` setting a zero timeout, two Clients each run a network operation that a fixture holds without answering, and a script that exits without close exits 0 within the close bound plus the cleanup bound, in the ordinary build and on the candidate build; and while one thread sets and deletes a variable in a loop, another opens 1,000 sessions, every open succeeds, and each snapshot holds the variable either absent or with a value the loop wrote.
  - Depends on CS4.3 and CS4.4. Review: single-axis, Safety first. Not **Ordinary path**, for CS4.3's reason.

- **CS4.6 — Bridge-internal tests against a fake channel** *(gwz-py; test code < 500 lines)*.
  - Files: `src/tests/test_bridge_transport.py`, `test_native_bridge.py`, `test_native_result_wait.py` and `test_native_operations.py`, and `test_transport_session_api.py` and `test_transport_session_native.py`, whose rows CS4.1's list maps: each is rewritten against the fake channel, or goes with `TransportSession` in CS4.8.
  - Implements §12's rule that bridge-internal tests run against a fake channel, with CS4.1's assertion changes.
  - Test-first: each row of CS4.1's retaken list has a rewritten test, after the list is retaken if the tests changed since CS4.1.
  - Depends on CS4.1 and CS4.3–CS4.5. Review: single-axis, Consistency first. Not **Ordinary path**: test code.

- **CS4.7 — Activation: `NativeCoreBridge` on the session host** *(gwz-py; < 200 lines)*.
  - **Ordinary path:** Python requests run through the session host, so R8's changes take effect: the cancel codes, `transport_session_full` and `operation_expired`, the open-merge pre-gate on Python (D4), and an interpreter exit that waits up to the close bound.
  - Files: `src/gwz/bridge.py`; `src/gwz/client.py` (the default bridge only). No per-Client limit exists to replace, since 1.1.0 S6.2 never landed; the session's running limit and queue bound a Client (§5.1).
  - Implements §10 with G11 held, and makes the supersessions of §14 for gwz-py take effect.
  - **Two budgets for a while** (Verdict-1; R12). Between this step and CS4.8, a gwz-py process that uses the session host and the legacy module functions at once holds two SSH supervisors and two sets of caps, at most twice today's. The window is development only: after this step no 1.0.x release is cut from gwz-py's main (§2.3), and the transport release follows CS6.5.
  - Test-first: §15.12 (gwz-py's Client-level and native-integration tests pass through `NativeCoreBridge`); §15.4 (eight overlapping fetch and push operations on one Client complete independently and a ninth queues; two Clients in one process run W operations on one workspace, and pushes on another, each pair in turn with no `UnsupportedOperation`); §15.5 (a cancelled queued submit and a cancelled running fetch show `failed` with `cancelled`; a queued unary call and a waiting direct call yield `SessionError{cancelled}`); §15.9 (two Clients in one process share the SSH helper budget); the restated S6.3 rows that need no reuse, on macOS, Linux and dabeest (§5.3). The first of them, two overlapping Python operations on one Client completing independently, closes the NO-GO of 2026-09-23 once it passes on all three platforms (transport plan §4).
  - Depends on CS4.2, CS4.6 and the Phase 3 exit. Review: the Phase 4 exit review.

- **CS4.8 — Legacy removal and gwz-py's debt** *(gwz-py; mostly deletions, < 300 lines added)*.
  - **Ordinary path:** gwz-py's legacy native entry points and their behaviour leave (D6), and the release notes state R8's changes.
  - Files: `native/src/operations.rs`, `shims.rs`, `diff_logs.rs`, `log_outputs.rs` and `dispatch/`, removed or kept off the session path per D6; the candidate `native/src/transport_session.rs` and the bridge's use of it, removed, with `src/tests/test_transport_session_api.py` and `test_transport_session_native.py` if CS4.6 left them; `native/src/lib.rs`; `scripts/process_globals_allowlist.json`; `dev-docs/GwzPyDesign.md` (the bridge-contract statements §14 replaces); `RELEASE.md`. The status edits to `GwzPyTransportDesign.md` are TR1.7's, which lands with TR1.4b's acceptance (transport plan TR1.7), not this step's.
  - Implements §9 (the legacy entry points leave the bridge's path) and §5.7's gwz-py rows: the diff and log registries, the legacy operation store and its thread-local scoping, the thread-local backend and operation ID, the current-session scoping, and the `GWZ_PY_TEST_EVENT_DELAY_MS` hook, which moves behind `cfg(test)`. Also 1.1.0 S6.2's removal of `TransportSession` and its `CURRENT_SESSION` `debt` entry (§5.3). With the legacy backend scope goes the live environment read CS3.1 adds there, the last native environment read on gwz-py's paths, which could race Python's `putenv` (the Verdict-2 third carried item, §5.3).
  - Test-first: §15.8 (`check_process_globals.py` passes over gwz-py with no `debt` entry, so none names `CURRENT_SESSION`); no `TransportSession` remains in gwz-py outside `dev-docs`, and the candidate build compiles. The release notes state the behaviour changes: the cancel codes, `transport_session_full` and `operation_expired`, and an interpreter exit that waits up to the close bound.
  - Depends on CS4.7, and on CS5.2, which also changes `native/src/lib.rs` (§4). Review: the Phase 4 exit review.

**Exit.** After CS4.8. Review: dual plus Surface, on the settled tree. Surface reads the Python API, its docstrings, gwz-py's `gwz-py` console script help and the release notes; the pre-gate outcomes CS2.13 listed; the `timeout_ms` semantics of the bridge's `diff_log_read`, whose bounded wait CS2.10 changes (R8); and CS4.5's documented changes (TR1.7). Reuse across a Python process's operations is CS4.9's Surface review, after the Phase 7 exit. `SocketCoreBridge` stays behind the switch until activation, where S7.5's Surface review covers it.

**After the exit.** CS4.9 closes the phase once the Phase 7 exit has passed (TR1.7).

- **CS4.9 — Reuse through gwz-py, and the restated S6.3 measurement** *(gwz-py tests and evidence; < 250 lines)*.
  - Files: new `src/tests/test_native_reuse.py`; raw runs in gwz-core-evidence, with a redacted public report; and a draft release note on reuse in a Python process, filed with the review package for S7.2's notes to carry at activation, since reuse is candidate-only until then.
  - Implements the gwz-py half of reuse and the restated S6.3 rows that need it: a Python process reuses connections across its operations and across Clients whose configurations are equal (reuse §11), which is TR1.7's Surface subject.
  - Test-first: reuse §15.3 through `NativeCoreBridge` (two Clients in one process run sequential fetches over one physical connection: the second row has `reused = true` and the first row's `connection_id`, and the fixture's accept count does not change); reuse §15.13 (a Client opened in a forked child has its own host context, and the parent's pooled connection stays usable after the child exits, on Linux and macOS); S6.3's measurement row: for 1, 2 and 8 overlapping operations, the construction cost of each operation's binding, the part of the transport TR1.2 names as the operation's own, and the connection counts, on macOS, Linux and dabeest, for S7.2's notes.
  - Depends on the Phase 4 exit, which merges CS4.7, and the Phase 7 exit. Review: single-axis, Safety first, plus Surface (TR1.7): reuse across a Python process's operations, its lifetime and its revalidation prompts, as the draft note and R8 state them. Not **Ordinary path**: tests, evidence and a draft note.

### Phase 5 — The wire proof (milestone: CI runs gwz-py's tests through both bridges, and any difference is a defect)

The wire proof's host is the contract's stdio test host, which speaks the server design's handshake (§12 as the server design amends it). CS5.4 builds the handshake's host side in core, and Phase 8's socket and stdio hosts reuse it; so the proof exercises the stdio mode's core path (server §15).

- **CS5.4 — `serve_session` and the byte-stream handshake, host side** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/serve.rs` and `src/session_host/handshake.rs`.
  - Implements server §3's handshake on the host side, which every byte stream uses (§3 as amended):
    - the host speaks first, with `SessionHello`, and waits at most 30 seconds for the client's one frame;
    - it checks a `SessionOpen` against the host-environment table: a stdio host takes the marker only without a snapshot, a socket host refuses the marker and a missing snapshot, and on POSIX a snapshot comes with `umask` and `openssl`; it validates `SessionLimits` as `open` does, and a socket host also refuses a limit above its §1 default;
    - it calls the same `open` the in-process path uses, with its host context and, as `transport_off` (CS1.9), the `SessionOpen`'s `ProcessAttributes.transport_off`, absent meaning false (server §5), and answers `SessionOpened`; a refusal is a `SessionError` with `call_id` 0, legal only before `SessionOpened`; each frame comes at most once, in order;
    - `serve_session` then runs the contract's reading thread, and a writer thread that drains the replies to the stream;
    - the process attributes a driver sends: its file-creation mask and, on Linux and macOS, the OpenSSL library core loaded (server §3, §5).
  - A stdio host refuses `ServerControl` with `invalid_request`. The socket host's `ServerControl` comes with CS8.10, and the must-match comparison with CS8.1. Until CS8.1, a stdio host applies the table's presence rules only; the only stdio host before Phase 8 is the unpublished test host, whose environment is the snapshot it is sent (CS5.1).
  - Test-first: server §12, gwz-core `serve_session` (the host's `SessionHello` first, then the client's one frame, answered by `SessionOpened`; a second client frame before `SessionOpened`, and a `SessionOpened` whose identity differs from the `SessionHello`'s, are protocol errors; `call_id` 0 before and after `SessionOpened`; `SessionLimits` at a socket host and at a stdio host; §3's table, including the transport plan's Phase 7 exit row that a socket host refuses a `SessionOpen` with no snapshot, and one with the marker, with `invalid_request` before `SessionOpened`; a stdio host refusing `ServerControl`; a snapshot that appears in no frame after `SessionOpen`; zeroization at session end, with CS1.9; a request with `RequestMeta` but no `InvocationContext` refused with `invalid_request` over the stream, the byte-stream half of CS2.3's addendum). Also: a `SessionOpen` whose `ProcessAttributes.transport_off` is true opens a session whose context reads it true, and one without the field opens it false.
  - Depends on CS1.3, CS1.9, CS1.10, CS2.3 and CS2.4. Review: dual, the `serve_session` freeze, which the test host, the socket host and the stdio host share. Not **Ordinary path**: before activation it has no caller but the unpublished test host (G10).

- **CS5.1 — The host binary** *(gwz-core; < 200 lines)*.
  - Files: new `examples/session_host.rs`.
  - Implements §12's host binary, which the server design makes the contract's stdio test host: it serves one session over stdin and stdout through the byte-stream adapter and `serve_session` (CS5.4), speaking first with `SessionHello`; it captures its own environment and process attributes once at start, and creates one host context. gwz-core's `Cargo.toml` `include` list already leaves `examples/` out of the published crate, so no crate installs it (G10).
  - Test-first: §15.9 (the byte-stream host closes its channel); `cargo package --list` contains no example (the 1.1.0 amendment round's residual on the example binary); server §12, gwz-py, host half (the test host refuses a `SessionOpen` that lacks `umask` or `openssl` on POSIX).
  - Depends on CS1.3, CS2.4 and CS5.4. Review: single-axis, Consistency first. Not **Ordinary path**: unpublished.

- **CS5.2 — `StreamCoreBridge`** *(gwz-py tests; < 350 lines)*.
  - Files: new `src/tests/stream_bridge.py`, a test bridge that is not packaged; `native/src/lib.rs`, which exposes core's process attributes (CS5.4) to Python.
  - Implements §12's test bridge: CS4.3's call table over asyncio subprocess pipes, with an asyncio reader task in place of the pump thread. It starts the binary with the snapshot the native bridge would pass to `open`. It speaks the client side of the handshake (server §3): it reads the host's `SessionHello`, refusing a first frame over 4 KiB or with another tag; it sends one `SessionOpen` with the snapshot, the limits and the process attributes, `umask` and `openssl` taken through the extension as `SocketCoreBridge` takes them; and it waits at most 30 seconds for `SessionOpened`. Its closed-session error has the code `server_session_closed` (83), and a stream that closes before `SessionOpened` fails the open with `server_unavailable` (77) (§10 as the server design amends it).
  - Test-first: §15.1 (a duplicate outstanding call ID, and a lower one, each end the session, and the original call fails with the closed-session error, `server_session_closed`, never `invalid_request`); server §12, gwz-py (`StreamCoreBridge` sends `umask` and `openssl`, and the test host refuses a `SessionOpen` without them; a channel closed before `SessionOpened` fails the open with `server_unavailable`).
  - Depends on CS4.3 and CS5.1, and on CS4.2, which first changes `native/src/lib.rs` (§4). Review: single-axis, Consistency first. Not **Ordinary path**: test code, and an extension function no public path calls.

- **CS5.3 — Two-bridge CI, the evidence rule and the release pins** *(gwz-py; < 250 lines)*.
  - Files: `.github/workflows/package-smoke.yml`, which builds the example from the checked-out gwz-core; `.github/workflows/publish.yml`, with its pin assertions; `run_tests.py`, for bridge selection and the marker naming the Client-level and native-integration tests; and, for 1.1.0 S6.2's release pins (§5.3), `scripts/release.py`, `RELEASE.md` and `test_native_module_reports_compiled_core_provenance` in `src/tests/test_native_bridge.py`.
  - Implements §12's CI on macOS, Linux and Windows, and S6.2's release pins: `RELEASE.md` and the publish workflow name every native dependency pin, and gwz-core on the release branch is `=1.1.0` from crates.io, which replaces `GwzCratesIoPlan.md` D7's git-tag pin in all four artifacts.
  - Test-first: §15.13 (the same tests pass through `StreamCoreBridge`; server §12, gwz-py: contract §15.13 passes with both bridges), which also gives the stream half of every §15 row that says "on both bridges". The 1.1.0 amendment round's Safety P3-3: the release-evidence run is the one against the registry-pinned gwz-core at the release tag, with the host binary built from the same tag; path-pinned runs are development evidence; retained evidence, including fixture logs and the host binary's stderr, passes the secret scan the 1.1.0 plan uses, and its record names the gwz-core version and the binary's source revision. Its Safety P3-6: 1.1.0 S3.3's stall regression passes on both bridges. §5.3's release-pin rows: on the release branch, a check finds gwz-core pinned `=1.1.0` from crates.io with no git-tag pin, and `RELEASE.md` and the publish workflow name every native dependency pin; `publish.yml` refuses a git-tag pin and accepts `=1.1.0`; a unit test of `release.py` shows it writes the registry form.
  - **The hook rows.** A row that needs CS2.1's test hooks runs in the development runs, with the hook switch on. The release-evidence run, which no workflow may give the switch (CS2.1, L2-12), runs every row that needs no hook, and its record names the development run, at the same source revision, that carried the hook rows (the round-1 Safety residual on CS2.1's switch).
  - Depends on CS4.7, CS4.8 (which edits `RELEASE.md` first, §4) and CS5.2. Review: the Phase 5 exit review. Not **Ordinary path**: it changes CI and release tooling, not what a build does, and its pins apply on the release branch, after CS4.7 has ended 1.0.x releases from gwz-py's main (§2.3).

**Exit.** Review: single-axis, Safety first, on evidence, secrets, the identity of release evidence and the release pins. Users see no change: the test host is unpublished, and `serve_session` has no other caller before Phase 8.

### Phase 6 — gwz-cli on the session host (milestone: the CLI sends every protocol request through an in-process session, and what users see is unchanged)

After this phase the CLI's copy of the dispatch and the O7 legacy exceptions are gone, and O9 holds, except for one entry: gwz-core's `debt` entry for the lazy endpoint's read in `transport_binding.rs`, which the session path reaches only if the transport-scope predicate misses a method (the Phase 3 exit), and which CS7.24 retires. Lane F can start after the Phase 2 exit.

- **CS6.1 — The CLI's session driver** *(gwz-cli; < 450 lines)*.
  - **Extended by CS6.6** (reuse §7, §13): the CLI calls its host context's bounded `shutdown` at command end. The extension is a dual re-freeze (§2.2).
  - Files: new `src/session_driver.rs`; `src/lib.rs`; `src/globalargs/dispatch.rs`, for the legacy read below.
  - Implements §11: one host context created at startup; an in-process session opened with it and the environment captured at startup; a submit followed by event reads whenever the renderer consumes events (JSONL output, and the progress line in human mode on a terminal), and a unary call otherwise; the final response read with `operation.response`, which §4.2 generalizes to every operation method; replies rendered as today. The main thread blocks on `recv()`.
  - It also removes the live-environment read that CS3.1 adds at `execute_with_backend`: that legacy entry takes the environment captured at startup.
  - Test-first: gwz-cli's local-command tests pass through the driver; on fixtures, the JSONL events read through `events.subscribe` equal today's sink output.
  - Depends on the Phase 2 exit, CS1.1, and CS3.1, whose gwz-cli read it removes. Review: single-axis, Consistency first.

- **CS6.2 — Diff, log and hook paths** *(gwz-cli; < 450 lines)*.
  - **Ordinary path:** gwz's diff, log and hook family paths run through the session host.
  - Files: `src/diff_exec.rs`, `src/log_exec.rs`, `src/hook/family.rs`.
  - Implements §5.2, whose dispatch absorbs the CLI's diff, log and hook paths, and §4.2's log reads and end stream. The hook's family verbs (`local_family`, `ls`, `clone_local_workspace`) go through the session; the hook's own filesystem probes stay in the CLI.
  - Test-first: gwz-cli's diff, log and hook tests pass through the session; a pager quit ends the reader stream and the exit codes are unchanged.
  - Depends on CS6.1 and CS2.16. Review: single-axis, Consistency first.

- **CS6.3 — CLI-local exceptions** *(gwz-cli; < 250 lines)*.
  - **Ordinary path:** `forall` resolves its members through the session and takes the workspace lock itself.
  - Files: `src/forall.rs`, `src/globalargs/dispatch.rs` (`UpdateBootstrap`), `src/hook/setup.rs`.
  - Implements D3's disposition of the requests with no protocol method (C2). On the recommended option, `forall` stays CLI-local, as GWZDesign's CLI driver design already says: it resolves members through `resolve_forall_targets` and keeps its DR-3 guard by taking the cross-process workspace lock itself, so the session host sees another process, as today. `claude-code setup` stays CLI-local. If D3 gives `init --update` a protocol method, a contract amendment and a schema step, reviewed dual like CS1.1, come first.
  - Test-first: `forall`'s dry-run and real-run tests, and `init --update`'s tests, pass unchanged.
  - Depends on CS6.1 and D3. Review: single-axis, Safety first.

- **CS6.4 — Activation: every request through the session** *(gwz-cli; < 400 lines, deletions excluded)*.
  - **Ordinary path:** every gwz request runs through the session host.
  - **Extended by CS6.7** (reuse §7, §13): the pending-cleanup notice also counts the host context's instance disposal. The extension is a dual re-freeze (§2.2).
  - Files: `src/lib.rs`; `src/globalargs/dispatch.rs` loses `execute_invocation`, `execute_with_backend` and `transport_meta`; its callers in `src/tests/m2c.rs`, `g12.rs`, `g13.rs` and `g01/commands.rs` are re-pointed at the session driver.
  - Implements §11 and completes the dispatch move: the 1.1.0 amendment round's A2, as handed to this plan. The pending-cleanup notice comes from the close report.
  - Test-first: §15.14 (the suite passes through the in-process session; a pseudo-terminal test, a new harness on macOS and Linux, shows the progress line); the CLI reference check passes with no change to help, and the machine-output fixtures are unchanged. Route evidence for the CLI entry: SSH and HTTPS clone and fetch through the session on macOS, Linux and dabeest, with the typed route assertion of CS3.11 (A3 for the CLI entry).
  - Depends on CS6.2, CS6.3 and the Phase 3 exit. Review: the Phase 6 exit review.

- **CS6.5 — Legacy removal and the CLI boundary** *(gwz-core and gwz-cli; < 250 lines added)*.
  - Files: gwz-core `src/transport_host/local_command.rs` (the legacy `with_local_transport`, once no caller remains), `src/transport_host/request.rs` (`cancellation_handle()` and `TransportCancellation`), `src/session_host/legacy.rs` with its allowlist entries, and `src/git/endpoint/agent_job.rs` and `https_auth.rs` (the legacy path's process-wide budgets), which it shares with CS7.13, CS7.15 and CS7.16 under an L1-06 handoff (§4); gwz-cli, a new source test.
  - Implements §16's removal of O7's two legacy exceptions, O9 except for the lazy endpoint's read, which CS7.24 retires, and §5.7's end state. The gwz-cli test fails on any direct call of a gwz-core handler outside the session driver and the named CLI-local exceptions (L2-06, G2). With the legacy context goes its environment read for the crate default, and CS3.3's four spawn entries and CS3.4's `git` spawn flip from `debt` to `permanent`, each citing its test. The legacy path's process-wide budgets go too: `HUB`, `INIT`, `COUNT`, `CLEANUPS` and `SLOTS` (CS3.1). Once the crate default goes, gwz-core's public handler API takes an explicit context, a crate API change R9 discloses.
  - Test-first: §15.8, as far as this step reaches it: `check_process_globals.py` over gwz-core lists no `debt` entry except `transport_binding.rs`'s `env::var_os`, whose reason names CS7.24, and that one only until CS7.24 merges; over gwz-transport and gwz-py it lists none. An injected direct handler call fails the gwz-cli test.
  - Depends on CS6.4 and CS4.8, and TR3.1 (G2).
  - Review: the Phase 6 exit review.

- **CS6.6 — The CLI's host-context shutdown and its off switch at open** *(gwz-cli; < 150 lines)*.
  - The additive re-freeze of CS6.1 (§2.2).
  - Files: `src/session_driver.rs`.
  - Implements reuse §7's in-process rule: the CLI has one host context per command, and calls its bounded `shutdown` (CS1.9) at command end, so the command's cleanup report is its session's close report plus that disposal, as runtime shutdown is part of the report today (`local_command.rs`). Also §5.6 as the server design amends it: the driver passes its resolved off switch to `open` as `transport_off`, false until TR2.5 resolves it.
  - Test-first: gwz-cli's local-command tests pass with `shutdown` at command end; a pending disposal, injected with CS2.1's hooks, appears in the command's cleanup report; `open` receives the resolved `transport_off`.
  - Depends on CS6.1 and CS1.9. Review: dual, with CS6.7 as one checkpoint. Not **Ordinary path**: an ordinary build's host context has no endpoint registry, so `shutdown` disposes nothing, and without the transport the switch changes no route.

- **CS6.7 — The pending-cleanup notice counts instance disposal** *(gwz-cli; < 100 lines)*.
  - The additive re-freeze of CS6.4 (§2.2).
  - Files: `src/lib.rs`, where CS6.4 builds the notice.
  - Implements reuse §7: the notice comes from the close report and the host context's `shutdown` report together.
  - Test-first: reuse §15.11's CLI row (a CLI test shows a pending instance disposal in the command's notice), with the disposal injected until instances outlive their bindings (CS7.26), and end to end at whichever of the Phase 6 and Phase 7 exits comes second.
  - Depends on CS6.4 and CS6.6. Review: dual, with CS6.6. Not **Ordinary path**: an ordinary build has no instances, so its notice is unchanged.

**Exit.** After CS6.5, CS6.6 and CS6.7. If the Phase 7 exit has passed, CS6.7's notice row runs end to end here. Review: dual plus Surface, on the settled tree. Surface compares gwz-cli's help at every level and its human, JSON and JSONL output with the release before. Recording the contract as implemented, which Verdict-2 asked to wait for these gates, is then an operator decision.

### Phase 7 — Connection reuse (the transport plan's Phase 6; milestone: operations in one host context reuse matching idle connections, and nothing else)

The reuse design's steps (reuse §14; §5.9 maps each), after this plan's network phase, as the transport plan's Phase 6 orders: every step here depends on the Phase 3 exit. Reuse C1 is CS3.2, and C4's entry is CS3.7.
- **Sharing comes last.** The registry shares nothing until CS7.26. Until then each binding keeps the private instance Phase 3 gives it, and the steps below exercise several bindings on one instance only through a registry option that only tests can set: it exists under `cfg(test)` or CS2.1's hook switch, and a workflow-text test shows that no release workflow enables it (L2-12; CS7.11). So no operation uses another's connection before revalidation, per-owner accounting and fault isolation are in (reuse §4, §5, §8).
- **The candidate build compiles at every step.** gwz-core stops calling a pool function before gwz-transport removes it (CS7.10), and every step builds the candidate before it merges (§2.4).
- **gwz-transport.** CS7.2–CS7.6 and CS7.10's second half are gwz-transport commits, tested in its CI and in gwz-core's candidate build against the sibling checkout. They add no process-global state; a change to gwz-transport's allowlist follows §5.7's bump rule. They ship in gwz-transport's Phase 10 release (transport plan Phase 10, step 2).
- **Large files.** CS7.1 splits the four endpoint files past 1,000 lines, movement only, before any step here grows them (§3.0).
- **Conditional compilation.** The registry and instance types are transport-build members of the host context, grouped as one member inside the `cfg_if` boundary that gates `transport_host`, with the ordinary build's counterpart in the same block. A step that edits `src/git/gitbackend/backend.rs` moves its `#[cfg(test)] use super::*;` into a `cfg_if` boundary (reuse §14).
- **Shared files.** The transport plan's Phase 2 changes files here too: TR2.1 builds the retry machine CS7.23 moves, and TR2.8 changes `agent_auth.rs` (CS7.17, CS7.18). A step that must cross one hands off under L1-06 (R1).
- No step here is **Ordinary path**: its code is candidate-only, or in gwz-transport, on which no ordinary build depends.

Lanes: RT, the pool (CS7.2–CS7.6); RC, the core (CS7.1, CS7.7–CS7.14, CS7.20–CS7.22, CS7.24); RA, authentication (CS7.15–CS7.19, CS7.23); CS7.25 in gwz-cli; then CS7.26 and CS7.27.

- **CS7.1 — Split the four large endpoint files** *(gwz-core; movement only)*.
  - Files: `src/git/endpoint/ssh_worker.rs` (1,279 lines), `placement_endpoint.rs` (1,064) and `https_worker.rs` (1,056), and `src/transport_host/session.rs` (1,009), each into owners of under about 500 lines, with the rust-split tool (L1-11, L1-23).
  - Implements nothing new. It lets this phase's steps change these files without taking one past 1,000 lines (§3.0).
  - Test-first: characterization (L1-03): gwz-core's ordinary and candidate-build tests pass unchanged, and the diff is movement only.
  - Depends on the Phase 3 exit and TR3.1 (G2), with a handoff under L1-06 for any transport plan Phase 2 step that still edits these files. Review: single-axis, Consistency first. Not **Ordinary path**: candidate-only code.

- **CS7.2 — Per-owner accounting in the pool** (reuse T1) *(gwz-transport; < 400 lines)*.
  - Files: `src/pool/mod.rs`, `machine.rs`, `allocation.rs`, `lifecycle.rs`.
  - Implements reuse §5 and T1: `Request` gains the operation's limits (`Capacity`), through an additive or defaulted API, so that gwz-core's candidate build compiles unchanged until CS7.10 plumbs the limits; the machine counts opening and leased entries per `Owner.session`, and admits a lease or a new connection only below the owner's `per_user_host`, `per_host` and `total`; an owner's outstanding requests stay below its `max_requests`; idle entries belong to no owner, which is the rule T3 and T4 follow.
  - Test-first: reuse §15.7, pool half (owners with `per_host` 4 and 32 are both admitted while a lease is non-idle, and the first never exceeds 4 opening or leased per host); retry S1.3's pool rows still pass.
  - Depends on the Phase 3 exit. Review: dual, with CS7.3–CS7.6 as one interface checkpoint, the pool's. Not **Ordinary path**.

- **CS7.3 — Ceilings by the max rule** (reuse T2) *(gwz-transport; < 300 lines)*.
  - Files: `src/pool/mod.rs` (`Config`), `machine.rs`, `asynchronous.rs`.
  - Implements T2's first half: `Config`'s `total`, `per_host` and `max_requests` become instance ceilings, applied as max(ceiling, the request's own limit), so a lone operation's limits stay exactly as today (reuse §5). The install path, `install_capacity`, `install_capacity_pair`, `can_install_capacity` and their use of `ActiveOperation`, stays until CS7.10 stops gwz-core calling it; CS7.10 removes it.
  - Test-first: reuse §15.7, pool half (with test ceilings of 8, a lone owner at 16 gets 16, and two owners at 8 share those 8).
  - Depends on CS7.2. Review: dual, in the pool checkpoint. Not **Ordinary path**.

- **CS7.4 — Cross-binding leases** (reuse T3) *(gwz-transport; < 250 lines)*.
  - Files: `src/pool/lifecycle.rs`, `allocation.rs`.
  - Implements T3: an idle entry's `Owner` stays `None`; the pool records its last lessee in a separate field that `cancel_operation`, `cancel_session`, eviction and per-owner accounting never read; a lease reports whether the last lessee's binding (`Owner.session`) differs from the new lessee's, without comparing the per-request serial.
  - Test-first: reuse §15.6's gwz-transport rows (after a release, the entry's `Owner` is `None` and its last lessee is set; `cancel_session` and `cancel_operation` naming the last lessee leave the idle entry untouched; a later lease by another binding reports "differs", and one by the same binding "same").
  - Depends on CS7.2, which names the same files. Review: dual, in the pool checkpoint. Not **Ordinary path**.

- **CS7.5 — Host hooks** (reuse T4) *(gwz-transport; < 250 lines)*.
  - Files: `src/pool/lifecycle.rs`, `mod.rs`, `allocation.rs`.
  - Implements T4: `retire_idle(connection, reason)` closes a named idle entry with a new `CloseReason::Revoked`, and is a reported no-op on an entry that is no longer idle; a `fresh` request takes no idle entry; `release` gains a `Revoked` disposition beside `Reusable`, `Unproven` and `Discarded`.
  - Test-first: `retire_idle` closes an idle entry `Revoked` and reports a no-op on a leased one; a `fresh` request opens anew while an eligible idle entry waits; a lease released as `Revoked` closes through its release path.
  - Depends on CS7.3 and CS7.4, which name the same files. Review: dual, in the pool checkpoint. Not **Ordinary path**.

- **CS7.6 — Reserve before connect** (reuse T5) *(gwz-transport; < 300 lines)*.
  - Files: `src/pool/machine.rs`, `asynchronous.rs`, `clock.rs`.
  - Implements T5: the machine asks the host for a physical reservation before Connect, and connects once it is granted; while it waits, the entry counts as opening, with no connect clock, under the allocation deadline.
  - Test-first: a withheld reservation defers the connect without failing it or starting its clock, and at the allocation deadline the request fails as an allocation timeout, never an immediate Capacity (reuse §5; §15.10, pool half).
  - Depends on CS7.3, which names the same files. Review: dual, in the pool checkpoint. Not **Ordinary path**.

- **CS7.7 — Placement tables per binding** (reuse C2 a) *(gwz-core; < 400 lines)*.
  - Files: the owners CS7.1 splits from `src/git/endpoint/placement_endpoint.rs`.
  - Implements C2(a) and reuse §2: `PlacementEndpoint`'s tables, keyed by request and stream, are keyed by binding as well, and its bridge limits (64 requests, 16 queued inputs, 64 outbound, 8 open jobs) are per binding, so an operation gets what its own runtime gives it today; two bindings may carry one `request_id` (§4.3 stands).
  - Test-first: two bindings register one `request_id` and each completes; a binding's limits equal today's runtime's; an exhausted outbound bound on one binding leaves the other's requests running.
  - Depends on CS7.1. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.8 — HTTPS endpoint state per binding** (reuse C2 b) *(gwz-core; < 450 lines)*.
  - Files: `src/transport_host/https_endpoint.rs`; the owners CS7.1 splits from `src/git/endpoint/https_worker.rs`.
  - Implements C2(b): `HttpsEndpoint`'s entries, operations, 64 request slots and 64 routes are per binding, over one shared client; the challenge carry, the Gh pin and the route pins stay in the binding (reuse §6); routes keyed by binding retire with their request (reuse §10).
  - Test-first: a binding's slot and route limits equal today's; two bindings' routes never cross, and each retires with its own request.
  - Depends on CS7.1. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.9 — The instance and its bindings** (reuse C2 c) *(gwz-core; < 500 lines)*.
  - Files: the owners CS7.1 splits from `src/transport_host/session.rs`; `src/transport_host/session/driver.rs`.
  - Implements C2(c) and reuse §8's binding failures: the endpoint `Session` splits into an instance, which keeps the shared engine, pools and authority, and its bindings; a binding's Bind creates only its own endpoint mux, with the instance's `EndpointConfig`, so every binding names the instance's endpoint ID; a mux protocol error, an exhausted waiter or outbound bound, or a panicked request task closes only its binding and discards its leases; the Q6 retirement record lives in the binding's registration, and its close at the cleanup deadline closes that binding only; no binding lock is held while another binding or the instance's physical owners are stepped (reuse §2).
  - Test-first: reuse §15.6 (a binding fault in A, an injected mux protocol error or an exhausted outbound bound, and a handler panic in A, leave B and the instance usable; Q6: backdated cleanup ages on A's binding close neither B nor the instance; and its cancellation row: A's token is cancelled while B holds a lease on the same instance, and B completes, A's lease closes, the idle connections survive, and B's next request reuses one), with two bindings attached to one instance directly.
  - Depends on CS7.7 and CS7.8. Review: dual, the instance and binding seam, the main shared-state change (reuse §17). Not **Ordinary path**.

- **CS7.10 — Capacity per operation, and the install path's removal** (reuse C5, T2) *(gwz-core and gwz-transport; < 350 lines)*.
  - Files: gwz-core `src/transport_host/mod.rs`, the owners of `src/transport_host/session.rs` and of `ssh_worker.rs`, and `src/git/endpoint/shared_reservation.rs`; gwz-transport's install path in `src/pool/` (`mod.rs`, `machine.rs` and `asynchronous.rs`) and its `README.md`.
  - Implements C5 and T2's second half (reuse §5): the binding carries the operation's limits, `per_user_host` and `per_host` from `--max-per-host`, `total` = max(256, `--jobs`) and `max_requests` = max(1024, `--jobs`), with every pool request; `admit_client_request`'s comparison and its `TransportCapacityConflict`, `install_capacity` and `CapacityMutation`, and `Authority::install_capacity` go; the SSH worker's `set_request_capacity` becomes an instance constant; `Owner.session` is the binding's session ID for SSH and HTTPS. Then gwz-transport removes `install_capacity`, `install_capacity_pair`, `can_install_capacity` and their use of `ActiveOperation`, and its README's ceilings paragraph takes reuse §13's text. So the session host never produces `TransportCapacityConflict`, and its mapping stays (§4.2, as the reuse design amends it).
  - Test-first: reuse §15.7 (operations with `--max-per-host` 4 and 32 are both admitted while a lease is non-idle; the first never exceeds 4 opening or leased per host; neither closes the other's connections; no `TransportCapacityConflict` occurs; with test ceilings of 8, a lone operation at 16 gets 16, and two at 8 share those 8; retry S1.3's pool rows still pass). These rows replace `src/transport_host/tests.rs:12-54`, `55-80` and `128-164`, and `src/transport_host/driver_tests.rs:123-146` generalizes to differing capacities (reuse §15).
  - Depends on CS7.2, CS7.3 and CS7.9, and on CS7.5 and CS7.6, which change the same pool files first (§4). Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.11 — The registry: selection, construction and the instance pump** (reuse C3) *(gwz-core; < 450 lines)*.
  - Files: `src/transport_host/registry.rs`; new `src/transport_host/instance.rs`.
  - Implements reuse §3's selection and §7's lifetime, C3's first half: the registry selects a usable instance whose configuration is equal, by digest and then in full, or builds one; construction does no network, trust, key or agent I/O, and takes one of the host context's cleanup permits; operations with one configuration wait for one construction, within the admission deadline, and a failed construction fails them before any effect and leaves no instance; selection, attach and the decision to dispose are made under one registry lock; an instance with no binding and no connection is disposed; the instance pump steps the shared engine, pools and authority, and detaches finished bindings, while a finishing operation only marks its binding, so no binding is detached twice (reuse §2); binding session IDs come from a counter in the registry and never repeat within the host context's life (reuse T3). Before CS7.26, only tests reach sharing, through the test-only registry option (the phase's preamble); CS7.26 makes it the registry's behaviour.
  - Test-first: reuse §15.4, the instance half (a differing configuration gives separate instances; with the option on, two sessions with equal configurations and different explicit identities share no connection); §15.5 as amended (a token cancelled while its operation waits for another operation's construction of the same instance fails registration, runs no handler and settles `Cancelled`); a failed construction fails every waiter before any effect and leaves no instance; with the option on, two sessions with equal configurations attach to one instance, and an instance with no binding and no connection is disposed; a workflow-text test shows that no release workflow enables the option (L2-12).
  - Depends on CS7.9. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.12 — Instance bounds, faults and the host context's shutdown** (reuse C3) *(gwz-core; < 400 lines)*.
  - Files: `src/transport_host/registry.rs`, `instance.rs`; `src/session_host/context.rs`.
  - Implements C3's second half (reuse §7, §8): at most 16 instances per host context, disposing the least-recently-used unbound instance beyond that; with every instance bound, an operation waits within its admission deadline, holding no registry lock, and is then refused before any effect with `transport_session_full`, naming the instance bound and its value; a faulted instance is no longer selected; with the cleanup-owner budget spent, a construction is refused the same way, naming that budget; each SSH instance holds a cleanup permit until its disposal completes, and for good if disposal overruns; the host context's `shutdown` (CS1.9) disposes every instance concurrently within one cleanup bound and reports what remains, and a drop disposes them without a report.
  - Test-first: reuse §15.11 (an instance with no binding is disposed after its last idle connection expires, with an injected clock, and its SSH worker, HTTPS thread and instance pump end and its permit returns; a 17th configuration disposes the least-recently-used unbound instance; with 16 bound, the 17th is refused only after its admission deadline, with `transport_session_full` naming the bound, and proceeds if an instance loses its last binding inside the deadline; `shutdown` disposes every instance within its bound, and its report names what remains); reuse §15.6 (with the cleanup-owner budget spent, a later operation is refused before any effect with `transport_session_full`, naming that budget).
  - Depends on CS7.11. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.13 — The instance clock and the maximum age** (reuse C3) *(gwz-core; < 350 lines)*.
  - Files: `src/transport_host/instance.rs`; the owners of `ssh_worker.rs` (its `now`) and of `transport_host/session.rs` (the admission deadline's conversion); `src/git/endpoint/agent_job.rs` (the setup helpers' `Control` deadlines); `https_pool.rs` (its epoch).
  - Implements reuse §7's clocks, C3's last part, and O5 as amended: each instance has one clock origin, which the SSH worker's `now`, the admission deadline's conversion, the setup helpers' `Control` deadlines and the HTTPS pool's epoch all read; where the platform has a clock that counts suspended time, the origin is that clock, and where it has none, the origin stays monotonic and the host discards every idle entry when the monotonic and wall clocks disagree by more than the idle timeout; a connection older than 10 minutes is closed at release instead of returning to idle (reuse §16.1 decision 5).
  - Test-first: reuse §15.11 (with the injected clock, a jump larger than the idle timeout expires every idle entry before the next lease; a jump larger than the aggregate during a setup fails that setup with `Timeout` on wake, and the setup helper's `Control` reports its expiry at the same instant as the pool); reuse §15.16 (a connection reused every 30 seconds is closed at its first release after 10 minutes, and the next operation opens anew).
  - Depends on CS7.11, and on CS7.12 and CS7.20, which change `instance.rs`, `https_pool.rs` and the owners of `ssh_worker.rs` first (§4). Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.14 — The public runtime over a private host context** (reuse C4) *(gwz-core; < 250 lines)*.
  - Files: `src/transport_host/mod.rs` (`TransportRuntime`'s public candidate constructors), `docs/TransportPlacement.md`.
  - Implements the rest of C4: `TransportRuntime`'s public candidate constructors become a wrapper over a private host context and its registry, since the placement guide documents them for embedders, and the guide takes reuse §13's text. CS3.7 built the session entry.
  - Test-first: through the wrapper, sequential SSH requests on one `TransportRuntime` share a connection, and sequential HTTPS fetches open no second TLS connection (`src/transport_host/driver_tests.rs:83-121` and `https_tests.rs:316-373`, reuse §1).
  - Depends on CS7.12, and on CS7.10, which changes `transport_host/mod.rs` first (§4). Review: single-axis, Consistency first. Not **Ordinary path**: a candidate-only API.

- **CS7.15 — The `gh` environment per request** (reuse C6) *(gwz-core; < 300 lines)*.
  - Files: `src/git/endpoint/https_auth.rs` (as CS3.6 splits it), the owners of `https_worker.rs`, `src/transport_host/https_endpoint.rs`.
  - Implements reuse §6's per-request rule: a Gh request's lookup uses its own binding's `gh` configuration, the executable `gh` found through the snapshot's `PATH` and the session's snapshot, reached through the gate; the helper is spawned as today, with `env_clear()` plus the snapshot, `GH_PROMPT_DISABLED=1`, `GIT_TERMINAL_PROMPT=0` and working directory `/`; the instance holds no `gh` environment, and CS3.7's private instances stop taking one; close cancels tokens before it revokes gates, so a session's snapshot can be dropped, and zeroized, when the session ends. This supersedes the HTTPS design's construction-time snapshot.
  - Test-first: reuse §15.1 (two sessions on one host context, with different `GH_TOKEN` and different `SSH_AUTH_SOCK`, run SSH and HTTPS operations: every `gh` invocation sees its own session's token, each new SSH connection's sign requests reach its own session's agent, no `connection_id` appears in both sessions' rows, and the registry holds two instances; with only `GH_TOKEN` different, the sessions share TLS connections, and each request's `Authorization` carries its own session's token), with the registry option on.
  - Depends on CS7.8 and CS7.11. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.16 — `gh` slots awaited in the host context** (reuse C7; extends CS3.6) *(gwz-core; < 250 lines)*.
  - The additive re-freeze of CS3.6 (§2.2).
  - Files: `src/git/endpoint/https_auth.rs`, the owners of `https_worker.rs`.
  - Implements reuse §6's slots and cleanup, and §5.7's `SLOTS` row as amended: a request awaits one of the host context's 8 helper slots within its own helper budget, and the wait is cancellable; that replaces the per-endpoint permit and the process-wide try-acquire that fails at once with Capacity; helper children belong to the instance's `AuthOwner`, and a retained child keeps its process and its slot, not the snapshot. The legacy path's process-wide `SLOTS` stays `debt` until CS6.5 (CS3.6).
  - Test-first: reuse §15.12 (nine concurrent lookups across two sessions: the ninth waits and completes within its helper budget, and one whose budget expires while it waits fails `Timeout`); a waiting lookup whose token is cancelled leaves the wait and holds no slot.
  - Depends on CS3.6 and CS7.15. Review: dual, CS3.6's re-freeze. Not **Ordinary path**.

- **CS7.17 — Keys kept on a pooled connection** (reuse C8) *(gwz-core; < 300 lines)*.
  - Files: `src/git/endpoint/ssh_setup.rs`, `agent_auth.rs`, `ssh_local.rs`, `ssh_key_snapshot.rs`.
  - Implements C8 and reuse §16.1 decision 10: the authenticated connection records its authenticating public key (for the agent, the accepted identity; for an explicit key, its public key) and its trusted host key; after authentication a pooled connection keeps only its token, a SHA-256 of the key bytes, in memory only and never in errors, and the public key, and private key text lives only with requests that are setting up a connection (reuse §12). This amends N2's registry.
  - Test-first: reuse §15.17 (after an explicit-identity setup, the entry a pooled connection pins holds the token, the digest and the public key only); after an agent setup, the connection records the accepted public key and the host key.
  - Depends on CS7.1, with a handoff under L1-06 for TR2.8's change to `agent_auth.rs` (R1). Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.18 — The possession proof** (reuse C9) *(gwz-core; < 450 lines)*.
  - Files: new `src/git/endpoint/ssh_proof.rs`; `agent_client.rs`; gwz-core's `Cargo.toml`, for the signature verifier.
  - Implements reuse §4's proof for an ambient SSH identity: a fresh 32-byte nonce; a sign request, to the instance's agent address, for the OpenSSH SSHSIG signed-data blob (the `SSHSIG` preamble, a fixed gwz namespace in the `name@domain` form, an empty reserved field, `sha512`, and the SHA-512 of the nonce), with `rsa-sha2-512` for RSA keys; verification against the recorded key, for Ed25519, RSA SHA-2 and ECDSA P-256, P-384 and P-521, with a verifier crate chosen under the release's pin rules (reuse §16.2); the proof runs as a supervised setup job under the host context's cap and the request's stall clock and absolute deadline, and a prompt does not pause the clock; nonces and signatures are never logged, stored, or placed in rows or errors.
  - Test-first: for each key type, a signature from the fixture agent verifies, and an altered one fails; an agent that refuses, lacks the key or is unreachable fails the proof; the blob's preamble and namespace keep it from parsing as SSH user-authentication data; a proof that outlasts the request's stall fails `Timeout`.
  - Depends on CS7.17, with a handoff for TR2.8's ECDSA admission (R1). Review: dual, a new verifier on the trust path. Not **Ordinary path**.

- **CS7.19 — Revalidation at each cross-operation lease** (reuse C9) *(gwz-core; < 450 lines)*.
  - Files: new `src/git/endpoint/ssh_revalidation.rs`; the owners of `transport_host/session.rs` and `ssh_worker.rs` that lease connections; `src/git/endpoint/ssh_network.rs`, for the known_hosts re-check.
  - Implements reuse §4: when a lease takes an idle connection whose last lessee was a different binding (CS7.4), the instance checks it for the requesting binding before the exchange starts, and holds it as that binding's lease meanwhile. For an ambient SSH identity, the proof (CS7.18); for an explicit one, the re-read key's public key equals the connection's authenticating key; for either, the retained host key is checked again against known_hosts, read now, a regular file of at most 4 MiB; for HTTPS, nothing beyond configuration equality. A binding remembers a passed proof per key, and a passed trust check per host, port and host key; concurrent leases that need one proof share one job, under the latest waiting request's deadline, which only the binding's token cancels; a waiter whose own deadline passes fails alone, and a passed proof is recorded for the binding even when the request that started it has failed. When no setup job can start, the connection returns to idle and the request fails with Capacity, as a setup that cannot start does. On a failure the connection closes through its lease as `Revoked` (CS7.5), every idle connection of the instance with the same key, or the same host and host key, is retired, and the request continues with a `fresh` checkout; on cancellation or a passed deadline the host releases the lease as reusable before it cancels the binding's pool work; an abandoned binding's lease closes `Cancelled` through `cancel_session`.
  - Test-first: reuse §15.2 (a: an agent behind a repointed path that lacks the key gives no reuse and retires the connection, and one that holds the key gives reuse after one sign request; b: a confirm-required key asks once per operation, a denial retires the connection and its replacement asks again, and several leases that need one proof see one prompt); §15.5 (a key removed from the agent or past its lifetime, a changed explicit key file, or a changed or removed known_hosts entry, between commands, gives no reuse and retirement, and a restored entry reuses again); §15.6 (cancelling B while its proof is pending returns the connection to idle, and A's next request reuses it; abandoning B closes it `Cancelled` while every other idle entry survives).
  - Depends on CS7.4, CS7.5, CS7.11 and CS7.18, and on CS7.22, the last of the steps that change the owners of `ssh_worker.rs` before it (§4). Review: dual, the trust decision at each cross-operation lease (reuse §4, §12). Not **Ordinary path**.

- **CS7.20 — Stale idle replacement** (reuse C10) *(gwz-core; < 300 lines)*.
  - Files: `src/git/endpoint/ssh_channel.rs`, the owners of `ssh_worker.rs`, `https_pool.rs`.
  - Implements reuse §7's replacement before send: on a reused SSH connection `Opened` waits for the open channel, and a failure while the channel opens discards the connection and continues with another idle entry or a new connection within the same deadlines; an HTTPS checkout of an idle connection that is no longer reusable discards it and checks out again within the allocation budget, and the host may call `idle_closed` when an idle connection's driver ends; a new connection is never replaced, so the loop is bounded; the observation reports the connection actually used.
  - Test-first: reuse §15.9 (the server drops an idle SSH connection, and the next operation's channel open fails and completes on another connection within its deadlines, reporting that connection; the same for HTTPS, with a closed TLS socket at checkout; a failure after the request reaches Hyper is not replaced).
  - Depends on CS7.9, and on CS7.10, which changes the owners of `ssh_worker.rs` first (§4). Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.21 — The shared authority** (reuse C11) *(gwz-core; < 350 lines)*.
  - Files: `src/git/endpoint/shared_reservation.rs`.
  - Implements reuse §5's authority: the pool reserves before Connect (CS7.6), and a full authority defers the connect without failing it; it asks the pool holding the oldest idle entry of either scheme in the constrained group to retire it, chooses again on a reported no-op, and frees slots only on disposal; with nothing idle, the request waits until its allocation deadline and then fails as an allocation timeout; the ceilings are 256 and 256 until S5.4 chooses them (transport plan Phase 8).
  - Test-first: reuse §15.10 (idle SSH connections that fill the authority do not stop an HTTPS open: the oldest idle SSH connection is retired and the open proceeds, and the same the other way; when the chosen entry is leased between the choice and its retirement, the waiting request is served by the next idle entry within its deadline).
  - Depends on CS7.6 and CS7.10, which names the same file. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.22 — Instance fault scope** (reuse C12) *(gwz-core; < 300 lines)*.
  - Files: the owners of `ssh_worker.rs` and `https_worker.rs`; `src/git/endpoint/ssh_shutdown.rs`.
  - Implements reuse §8's instance faults: a sticky cleanup failure or a key-admission failure closes the instance's admission and lets active exchanges run to their terminal, and the instance is disposed when its last binding detaches; the SSH worker or the HTTPS thread ending, or a panic caught at an instance entry point, stops the instance and fails each active exchange with its typed failure, as today, and it is disposed under the cleanup owner. This reads the A3 slice's "A worker cleanup error closes endpoint admission" as closing admission without an immediate stop, the reading reuse §13 records.
  - Test-first: reuse §15.6 (an injected sticky-cleanup fault stops selection, B's active exchange completes, and a later operation builds a fresh instance; an injected panic in the SSH worker loop, and separately a forced HTTPS thread exit, fail B's active exchange with a typed failure, no further frame arrives on that connection, the host context stays usable, and a later operation of B builds a fresh instance and completes).
  - Depends on CS7.12, and on CS7.13 and CS7.16, which change the owners of `ssh_worker.rs` and `https_worker.rs` first (§4). Review: single-axis, Safety first. Not **Ordinary path**.

- **CS7.23 — The retry machine per binding** (reuse C13) *(gwz-core; < 250 lines)*.
  - Files: the retry machine that TR2.1 adds, and the binding that owns it.
  - Implements reuse §9: the retry plan's per-key machine lives in the binding, keyed by pool key, so one operation's authentication failure closes a key for that operation only; leasing an idle connection is not a setup and uses no attempt; while a key is Cold, the operation's members may lease eligible idle connections, and only new setups wait for the one probe; a Closed key stops reuse too, for that operation; a failed revalidation and a stale replacement are not attempts; a reused SSH connection whose channel opens, or a reused HTTPS connection that returns a response, marks the key Healthy for the operation (reuse §16.1 decision 12).
  - Test-first: reuse §15.8 (A's authentication failure closes the key for A only, and B on the same instance still leases and opens; revalidation failures and replacements consume no attempt); reuse §15.18 (on a Cold key, a reused connection whose channel opens marks the key Healthy for that operation, and a following member opens without waiting for the probe); the retry plan's own rows still pass.
  - Depends on TR2.1, CS7.19 and CS7.20. Review: single-axis, Consistency first. Not **Ordinary path**.

- **CS7.24 — The lazy endpoint retires** (reuse C14) *(gwz-core; < 200 lines)*.
  - Files: `src/git/gitbackend/transport_binding.rs`, `scripts/checks/process_globals_allowlist.json`.
  - Implements reuse §11: first an audit of gwz-cli's `transport_meta` and the session dispatch's transport-scope predicate (CS1.6) against every handler that reaches `transport_binding::configure`, filed with the review package; then the backend-local SSH endpoint and its environment read go, with the read's `debt` entry, so every transport-build network operation's endpoint comes from the registry, and a transport-build SSH open without a binding is refused before any effect. HTTPS without a host context is decided in 1.1.0 S7.1's ledger work (reuse §16.1 decision 11).
  - Test-first: reuse §15.15 (a transport-build SSH open without a binding is refused before any effect, and the checker's allowlist holds no `transport_binding` environment entry); once CS6.5 has also merged, `check_process_globals.py` over gwz-core lists no `debt` entry (§15.8, O9).
  - Depends on CS7.14 and CS3.10. Review: single-axis, Safety first. Not **Ordinary path**: candidate-only code.

- **CS7.25 — The `--verbose` connection summary** (reuse §10) *(gwz-cli; < 200 lines)*.
  - Files: `src/response_meta_json.rs` and the human `--verbose` renderer.
  - Implements reuse §10: one summary line per operation, derived from its transport rows: physical connections (distinct `connection_id`s), new ones (those with a `reused = false` row), inherited ones, and exchanges. No row field is added (reuse §16.1 decision 13).
  - Test-first: reuse §15.14 (`--verbose` prints physical, new and inherited connections and exchanges; B's rows hold no sentinel member path, remote or URL from A, and no configuration value; the contract's §15.8 timeout test passes unchanged).
  - Depends on the Phase 3 exit; its rows with reuse run with CS7.26. Review: single-axis, Consistency first; S7.5's Surface review covers the line (reuse §16.1 decision 14). Not **Ordinary path**: an ordinary build's operations have no transport rows, so the line never appears.

- **CS7.26 — Sharing on, and reports over shared instances** (extends CS2.12) *(gwz-core; < 150 lines)*.
  - The activation of reuse, and the additive re-freeze of CS2.12 (§2.2).
  - Files: `src/transport_host/registry.rs`, `src/transport_host/session_entry.rs`, `docs/Embedding.md`, `docs/OperationModel.md`.
  - Implements reuse §3 and §7 in full: the registry selects a usable instance of equal configuration for every binding, and instances outlive their bindings under CS7.11's and CS7.12's rules. An operation's cleanup report is then its binding's alone: a session's close report sums its operations' binding reports, idle connections belong to no operation and are not in it, and the host context's owner reports instance disposal through `shutdown` (reuse §8; §8 as amended). A session with no network operation still reports `(0, false)`.
  - Test-first: reuse §15.3 (two sessions with equal configurations run sequential operations over one physical connection, for SSH and for HTTPS: the second row has `reused = true` and the first row's `connection_id`, and the fixtures' accept counts do not change); a close after network operations whose connections stay idle reports only binding work, and the host context's `shutdown` reports the instances' disposal; every Phase 7 row passes with the default setting.
  - Depends on every other Phase 7 step except CS7.25 and CS7.27. Review: the Phase 7 exit review (L1-32), which is dual, as CS2.12's re-freeze requires. Not **Ordinary path**: candidate-only code.

- **CS7.27 — Reuse measurements** *(evidence; < 300 lines of fixtures)*.
  - Files: gwz-core test fixtures; raw runs in gwz-core-evidence, with a redacted public report.
  - Implements the measurement the per-operation runtime owed (the 1.1.0 amendment round's Safety P3-4), for the shared model: for 1, 2 and 8 overlapping operations through the session host, on macOS, Linux and dabeest, each binding's construction cost (a link thread and a driver session), the instance threads, and the physical-connection and exchange counts, with sharing on (reuse §13's amendment of the contract's §16; R4).
  - Test-first: none beyond its fixtures. The record names the source revision, and follows §2.4's redaction rule.
  - Depends on CS7.26. Review: part of the Phase 7 exit. Not **Ordinary path**: fixtures and evidence.

**Exit.** Every row of the contract's §15.16, reuse §15 items 1 to 18, passes, except the rows whose owning steps follow this exit: item 1 across a server's clients, item 3 through gwz-py and through a server, item 11's server-shutdown half and item 13's pooled-connection half. Those run at CS4.9 and CS8.32, whose reviews carry them ([TR1.4b's Verdict-2](GwzCoreSessionPlan-Verdict-2.md), S-P3-4). The operator adopted every §16.1 decision, so items 16 to 18 apply. The rows run in gwz-core's CI on macOS and Linux, and transport rows on dabeest once S4.5 has landed (G2); gwz-transport's CI is green. The transport plan's Phase 6 exit tests pass, mapped as reuse §15 maps them (items 3, 1, 4, 2a, 2b, 5, 6, 7, 7 and 14). If the Phase 6 exit has passed, CS6.7's notice row runs end to end here; otherwise the Phase 6 exit runs it. The reuse cells' evidence (reuse §15's cell table) is filed for S5.6 (transport plan Phase 8). Review: dual, on the settled tree, attacking shared state across bindings: selection, attach and disposal under the registry lock; revalidation at every cross-operation lease; cancellation and fault isolation; per-owner accounting and eviction; and CS7.26's activation (L1-32). No Surface: the idle default stays 60 seconds (reuse §16.1 decision 1), and S7.5's Surface review covers the `--verbose` line (decision 14). CS4.9's Surface review covers reuse in a Python process (TR1.7).

### Phase 8 — The server (the transport plan's Phase 7; milestone: both CLIs host and use a server on all three platforms, with reuse through it)

The server design's steps, in the order the transport plan's Phase 7 sets: `gwz server` after this plan's Phase 6 (CS8.20–CS8.25 depend on the Phase 6 exit); `gwz-py server` and `SocketCoreBridge` after its Phase 4 (CS8.26–CS8.30 depend on the Phase 4 exit); the Windows primitives after the transport plan's Phase 4 (CS8.17–CS8.19); and reuse through a server after Phase 7 (CS8.32). Core's server modules, CS8.1–CS8.16, need only the steps each names, so lane SC runs beside Phases 3 to 6. The code lives where server §2 puts it: the security-critical code once, in core, which both CLIs call; gwz-py's copies of itself, and its `ssh`, are spawned by the extension through core's launcher.
- **Behind the switch until activation** (transport plan §6(a)). The CLIs' `server` family, `--server`, `--no-server` and `GWZ_SERVER`, the stdio local form and the SSH remote form, and the extension's server bindings that `SocketCoreBridge` and `gwz-py server` need, sit in the switch's `cfg_if` arms and fail closed without it. Core's modules build in both builds, and no ordinary-build caller reaches them. So no step here is **Ordinary path**.
- **No environment reads in core.** Core's server code reads no environment variable itself: the serving process's own values and a client's snapshot come from the driver's capture at its edge (server §5, "How the check runs"). Its process-wide effects are `permanent` entries under §3.0's rule for driver-level state, each in the step that adds it.
- **Linux runners** follow §2.4 for every row that runs a server, or a caller's own sandbox check: CS8.8 adds the first-step assertion to gwz-core's CI, CS8.25 to gwz-cli's and CS8.30 to gwz-py's.
- **Windows and other Unix arms.** Until CS8.17–CS8.19 land, each server module's Windows arm refuses with `server_unavailable`. On a Unix that is neither Linux nor macOS, which the crate also builds for, the walk, the sandbox rule, the socket host and the client refuse with `server_unavailable` before any connect or bind ([TR1.4b's Verdict-2](GwzCoreSessionPlan-Verdict-2.md)).
- **The release gate** (server §5, §12). Neither CLI releases `server start` while gwz-core's allowlist holds an `env` or `process` `debt` entry, and gwz-py does not release `gwz-py server` while its own allowlist holds one. CS8.4 adds the checker's mode, and the gate passes once CS6.5 and CS7.24 have cleared gwz-core's `env` and `process` debt, and CS4.8 gwz-py's. After the Phase 8 exit the mode stays on in `run_tests.py`. The transport plan's Phase 7 exit names only gwz-core's checker, so this plan's exit names both (server design Verdict-1).

Lanes: SC, core (CS8.1–CS8.16); SW, Windows (CS8.17–CS8.19); SL, gwz-cli (CS8.20–CS8.25); SP, gwz-py (CS8.26–CS8.30); then CS8.31 and CS8.32.

- **CS8.1 — The must-match set** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/server/must_match.rs`; `src/session_host/serve.rs`.
  - Implements server §5, and the contract's §5.6 and §5.8 as it amends them: the must-match list per platform, core's one copy beside the §5.8 disclosure (libgit2's start, the mask on POSIX, OpenSSL and the loader on Linux and macOS, the libgit2 network timeout, and the native row, `SSH_AUTH_SOCK` and on Windows the logon session); the comparison (byte strings, WTF-8 on Windows; names exact on POSIX and case-insensitive on Windows; unset distinct from empty; on Linux, `SSL_CERT_FILE` and `SSL_CERT_DIR` as openssl-probe resolves them, a host resolving its own once after its git2 start); the refusal of a relative path in a path variable or a list element, an empty value counting as relative on POSIX except `SSH_AUTH_SOCK` and `SSLKEYLOGFILE`; the host's own values, which the serving driver captures once at start; the check at `SessionOpen` in `serve_session`, for socket and stdio hosts, of the every-session rows, and of the native row too when the switch is on or the build is ordinary; a client whose real and effective user IDs differ refusing before it connects. A refusal uses `server_environment_mismatch`, naming the value and never its content.
  - Test-first: server §12, gwz-core `serve_session` (an empty `XDG_CONFIG_HOME` and a relative `OPENSSL_CONF` refused at open by a socket and a stdio host, absolute values passing, and a socket host whose own `XDG_CONFIG_HOME` is empty refusing to start; an empty `SSH_AUTH_SOCK` and an empty `SSLKEYLOGFILE` passing, and a non-empty relative value of either refused; must-match refusals at `SessionOpen` naming the value and never its content; `SSL_CERT_FILE` and `SSL_CERT_DIR` compared as resolved, with snapshots taken before and after git2's start both accepted, and a bundle created after a host's start not letting a client pass; unset and empty distinct, and names case-insensitive on Windows; a differing `openssl` attribute, and a differing `LD_PRELOAD` on Linux or `DYLD_INSERT_LIBRARIES` on macOS, refused at `SessionOpen`; a client with differing real and effective user IDs refusing before it connects, with injected IDs; the off switch taken from `transport_off`, never from the snapshot or the host's environment).
  - Depends on CS5.4. Review: dual, the must-match freeze: what a server refuses rather than serve with its own value. Not **Ordinary path**.

- **CS8.2 — OpenSSL's configuration, files read once and the `auto` key** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/server/openssl_scan.rs` and `auto_key.rs`.
  - Implements server §5's scan and "Files read once", and §4's key. The scan reads the configuration the process's OpenSSL loaded, follows `.include` and `.pragma includedir`, and collects every `$ENV::` name into the OpenSSL row. It opens files close-on-exec and without blocking, and fails closed, with `server_unavailable` naming the file and the reason, on an unreadable or non-regular file, a file over 1 MiB, more than 32 files, nesting deeper than 8, or an unparsable line. It records each file's identity (device, inode, size and modification time) and, on Linux, the CA bundle libgit2 loaded; a changed identity refuses a session at `SessionOpen`, or a native HTTPS operation when it routes, naming the file and `gwz server stop --server <address>`. The `auto` key is the first 16 hexadecimal digits of the SHA-256 of a deterministic-CBOR list of the core version and build and every must-match name and value in the comparison's form, whatever the off switch, with the scanned names and values, the files' identities and, on Windows, the logon session. The stdio mode skips the scan (server §15).
  - Test-first: server §12, OpenSSL's configuration, the rows without a server (an included `$ENV::` name joins the set; an unreadable include, nesting deeper than 8, a FIFO or a file over 1 MiB fail the scan, naming the file; a changed identity is detected); the socket host's `auto` key rows (equal environments give equal keys; each must-match row changes it; an in-place edit of a scanned file changes it; the off switch does not). CS8.10 runs the rows that start a server.
  - Depends on CS8.1. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.3 — Native routes through a server** *(gwz-core; < 250 lines)*.
  - Files: `src/transport_host/session_entry.rs` (the route decision's check), `src/session_host/server/must_match.rs`.
  - Implements server §5's routing rule: in a session a socket or stdio host serves, an operation that takes the native path with the switch off, by TR1.6's route for private HTTPS whose helper is not `gh`, by OD11's route for an SSH remote whose agent holds a key the transport cannot sign with (TR2.8), or as a `git://` or `http://` remote, is checked against the native row and the timeout row when it routes, before any connection opens. The route decision uses only the session's own snapshot. A mismatch refuses that operation with `server_environment_mismatch`, naming the route's cause and the variable or the logon session, never a value.
  - Test-first: server §12, native routes through a server (transport plan Phase 7 exit): at an explicit address, a server and a client with different `SSH_AUTH_SOCK`, the client's agent holding a key TR2.8 routes native: the client's SSH operation is refused before any connection opens, and the server's agent records zero signature requests; on Linux, different `SSL_CERT_FILE` values with a private HTTPS member on TR1.6's route: refused at `SessionOpen`, and the TLS fixture records no handshake. Server §12, gwz-core `serve_session`: a session whose timeout differs from the server's runs its transport remotes, and a `git://` or routed operation is refused before any connection opens, whether the session called `configure_transport_runtime` or kept the default.
  - Depends on CS8.1, CS8.10 and CS3.10, and shares `session_entry.rs` with CS7.26 under an L1-06 handoff (§4); its TR2.8 row on TR2.8, and its TR1.6 row on TR2.2's route after TR1.6. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.4 — The server's checks: vendored reads and the release-gate mode** *(gwz-core tooling; < 450 lines)*.
  - Files: new `scripts/checks/check_native_env_reads.py`, with its tests and its exclusion table; `scripts/checks/check_process_globals.py` and its tests; `scripts/checks/process_globals_allowlist.json` (the credential-helper entry's wording); `scripts/run_tests.py`.
  - Implements server §5, and §5.7 and §5.8 as server §8 amends them: a source-level scan of the vendored libgit2, libssh2, openssl-probe and git2's `lib.rs` for environment reads, failing on a name that is neither in core's must-match list (CS8.1) nor in the exclusion table with its reason; the checker's mode that fails on `debt` entries of named kinds, which releasing `server start` needs to pass for `env` and `process` over gwz-core's allowlist, and releasing `gwz-py server` over gwz-py's too; and the allowlist entry that names the git2 crate as the credential helper's spawner. It wires the mode into gwz-core's `run_tests.py` over gwz-core's and gwz-transport's allowlists, off until the Phase 8 exit's gate first passes and on from then, so a later `env` or `process` `debt` entry fails every gwz-core run; CS8.30 does the same for gwz-py's allowlist in gwz-py's test run. openssl-probe's write of `SSL_CERT_FILE` and `SSL_CERT_DIR` is the contract's disclosed §5.7 row, which no allowlist holds.
  - Test-first: server §12, native routes (a source-level test asserts that the must-match list holds every name in server §5's tables, and the scan fails on a planted read of an unlisted name in a fixture copy); server §12, gwz-core (the checker's new mode fails on an `env` or `process` `debt` entry, and passes over a fixture allowlist without one); with the mode switched on, `run_tests.py` fails over a fixture allowlist that gains an `env` `debt` entry.
  - Depends on CS8.1, and on CS3.4 and CS1.7, which change `check_process_globals.py` and gwz-core's `run_tests.py` first (§4). Review: single-axis, Safety first. Not **Ordinary path**: tooling.

- **CS8.5 — The address grammar** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/server/address.rs`.
  - Implements server §3's addresses and §16 question 1's parser: one `ServerAddress` type, `auto`, a socket path, a pipe name, `stdio` or an SSH destination, which every connector, binder and launcher takes, so no code path opens an unparsed string; the forms per platform and per source (`--server`, `GWZ_SERVER`, `SocketCoreBridge`), with their length limits and macOS's refused prefixes; the SSH form, `ssh://[user@]host[:port]/absolute/remote/path`, from `--server` only (OD12 is yes), with question 2's allowlist (a host of ASCII letters, digits, `.`, `-` and `_`, or a bracketed IPv6 literal without a zone; a user likewise; neither empty nor starting with `-`; a port from 1 to 65535; a remote path that starts with `/` and is valid UTF-8 without NUL, `?` or `#`, taken without percent-decoding); everything else refused with `server_address_refused` before any file-system, pipe or network call, naming the source and the reason, with the address's control characters escaped; an empty `GWZ_SERVER` counting as unset, and an empty `--server` refused. CS8.19 adds the Windows checks after the grammar.
  - Test-first: server §12, the address parser (transport plan Phase 7 exit), on every platform, with a connector double that records no call: the Windows corpus of network forms, `\\?\pipe\x`, `\??\pipe\x`, `\\.\PIPE\x`, `\\.\pipe\x.`, `\\.\pipe\x `, `\\.\pipe\a\b`, an empty rest, a NUL, U+FF3C and an over-length name; on Linux an abstract name, an empty, `.` or `..` component, a trailing `/` and a 108-byte path; on macOS `/.vol/1/2/s`, `/.nofollow/…`, `/.resolve/4/…` and a 104-byte path; on every platform `stdio` and `ssh://` from `SocketCoreBridge`, `ssh://` from `GWZ_SERVER`, and both given to `server start`, `stop` and `status`. Server §16 question 8, core half: a user or host that starts with `-`, or holds a character outside the allowlist, a backtick and `$(` included, is refused before `ssh` runs.
  - Depends on CS1.10, which adds `server_address_refused`. Review: dual, the address-grammar freeze: nothing opens an address it refuses. Not **Ordinary path**.

- **CS8.6 — The walk on Linux** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/server/walk/linux.rs`, in a `cfg_if` arm.
  - Implements server §3's Linux walk: from `open("/", O_PATH | O_DIRECTORY | O_CLOEXEC)`, each single component is opened with `openat(parent, name, O_PATH | O_NOFOLLOW | O_CLOEXEC)`; `statx` on the descriptor refuses `STATX_ATTR_AUTOMOUNT`; `fstatfs` refuses the listed types, autofs and every FUSE file system among them; a procfs link is refused, and any other link is read and its target walked from `/`, at most 8 links in all; a missing component ends the walk without a refusal. The walk ends holding an `O_PATH` descriptor of the socket, through which the client connects and the host binds (`/proc/self/fd`); without `/proc` no client connects and no host binds, as server §3 and §14 say. (The server design's erratum of 2026-09-28 corrected §12's row to match: without `/proc` the client refuses before any connect.) A refusal uses `server_address_refused`, naming the component and its type.
  - Test-first: server §12, the walk on Linux (transport plan Phase 7 exit), on a runner with `sudo`: an autofs program map whose key log stays empty; a systemd transient automount that stays waiting, with no automount request in the journal; tracefs refused for `STATX_ATTR_AUTOMOUNT`; loopback NFS, SMB and sshfs mounts refused by type, and a fake `statfs` layer for the rarer types; a procfs link, and a ninth link, refused; the connect through `/proc/self/fd` reaching the walked socket after its directory is swapped for a link; the host refusing to create its per-user directory under a refused path.
  - Depends on CS8.5. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.7 — The walk on macOS** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/server/walk/macos.rs`, in a `cfg_if` arm.
  - Implements server §3's macOS walk: from `open("/", O_SEARCH | O_DIRECTORY | O_CLOEXEC)`, each component is probed first with `getattrlistat(…, FSOPT_NOFOLLOW)`, refusing `DIR_MNTSTATUS_TRIGGER`; only a link is read, with `readlinkat`; only a directory that is not a trigger is opened, with `openat(parent, name, O_SEARCH | O_NOFOLLOW | O_CLOEXEC)`, and its `fstatfs` and `fgetattrlist` must show the probe's IDs; a mount is refused when it is not `MNT_LOCAL`, is `MNT_AUTOMOUNTED` or has a listed type name; links are walked from `/`, at most 8. A CLI holds `/dev/autofs_notrigger` from the walk's start until `connect` returns, or sets `setiopolicy_np` at process scope where the device cannot be opened, a `permanent` entry under §3.0's rule; `SocketCoreBridge` holds neither.
  - Test-first: server §12, the walk on macOS: on every macOS runner, `/home/gwz.invalid/s` refused at auto_home's root with no lookup below the map (transport plan Phase 7 exit); on a disposable runner with `/net` enabled, `/net/gwz-fixture.invalid/s` and a local link to it refused with no lookup below `/net` (Phase 7 exit); on the same runner, a direct map's trigger showing `DIR_MNTSTATUS_TRIGGER`, an address below it refused with no mount, and a control `open` showing that the amendment's former probe mounts it; a loopback `nfsd` mount refused by type; a CLI holding `/dev/autofs_notrigger` across its walk and connect, and `SocketCoreBridge` not; the host refusing to create its per-user directory under a refused path.
  - Depends on CS8.5. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.8 — The sandbox rule and pinned process checks** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/server/sandbox.rs`, with Linux and macOS arms in `cfg_if` blocks; the Windows arm is CS8.18's. Also gwz-core's workflows whose Linux jobs run its test suite, `.github/workflows/platform-matrix.yml` among them: each such job asserts §2.4's runner rule as its first step, before any of this phase's rows runs there.
  - Implements server §4's rule: on Linux, a `Seccomp` status other than 0, or a user namespace other than the initial one (the `/proc/<pid>/ns/user` inode `0xEFFFFFFD`), marks a process sandboxed, and a peer in another mount or user namespace is refused; on macOS, `sandbox_check_by_audit_token` on the audit token, else `sandbox_check` on the process ID; a check that cannot be made refuses; a process is read only through a hold taken after the connection (`SO_PEERPIDFD`, or a start time that precedes the connection; `LOCAL_PEERTOKEN`'s audit token), and one that began after the connection is refused; the caller's own check runs before any connect or start, and refuses a sandboxed caller with `server_peer_refused`, except in the SSH remote form.
  - Test-first: server §12: process IDs (a peer or listener that began after the connection, with an injected start time, is refused; on Linux 6.5 and later the check holds `SO_PEERPIDFD`'s descriptor, and on macOS it reads the audit token); listener verification (on Linux, with two seccomp-only sandboxes of equal filter count in one namespace, a client in either refuses itself before any connect, and `server start` in either refuses to start; a process in the initial user namespace is not flagged, and one under `unshare -U` is). A workflow-text test shows that every gwz-core Linux job that runs the test suite asserts the runner rule first, and that none runs in a `container:`.
  - Depends on CS1.10, which adds `server_peer_refused` and `server_listener_refused`, and on CS1.7, which changes the same workflows first (§4). Review: dual, the sandbox rule, the trust boundary a server keeps. Not **Ordinary path**.

- **CS8.9 — The socket host's files: directory, lock, record and stale socket** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/server/host_files.rs`.
  - Implements server §4's files: the per-user directory, created with mode 0700 relative to the walked parent without following a link, and refused with another owner, group or other bits, or as a link; the file rule for every file the host opens (relative to the walked directory's descriptor, with `O_NOFOLLOW` and `O_CLOEXEC` and `O_NONBLOCK` during the open; an existing entry used only when it is the user's regular file with no group or other bits; new files created 0600; the log only appended to); the lock and its record (created only after the walk and the directory checks; the previous holder's record read under the file rule; the host's record written through the lock's descriptor after the stale-socket check passes and before it binds; a clean exit removing the socket, then clearing the record, then releasing the lock; the lock file kept); and the stale-socket rule (an entry removed only when it is the user's socket, a non-blocking connection to it within 5 seconds is refused, and the record beside it names exactly that address). Refusals use `server_address_refused` or `server_unavailable`, with server §17's text.
  - Test-first: server §12, the socket host (the directory, owner and permission refusals; the file rule: a link, and separately a FIFO, at the log and at the lock before a start, and a link named by `--log`, refuse the start within 5 seconds, the link's target unchanged, and a lock or log with a group bit or another owner is refused; the lock and its record; the record's trust, with a forged record and a FIFO at the lock's path; the stale-socket rows, a foreign program's socket with a full accept queue on macOS included).
  - Depends on CS8.6 and CS8.7. Review: dual, with CS8.10 as one socket-host checkpoint (AgentProcessRules §6.1): its file, record and stale-socket rules close the server design's first-round P1 and P2 findings (server §18; [TR1.4b's Verdict-2](GwzCoreSessionPlan-Verdict-2.md), S-P3-2). Not **Ordinary path**.

- **CS8.10 — The socket host: accepting, control and shutdown** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/server/socket_host.rs`.
  - Implements server §6's serving and §4's peer check: the host binds at the walked address (through `/proc/self/fd` on Linux, holding `/dev/autofs_notrigger` on macOS) and accepts on one thread; each connection gets the peer check before any byte is read, another user or a sandboxed peer refused with `server_peer_refused` naming the cause, then the host's `SessionHello`, then a thread of its own that reads the client's one frame; a `SessionOpen` runs `serve_session` (CS5.4) with the server's one host context and the must-match check (CS8.1); a `ServerControl` is answered with `ServerState` however many sessions are open; a `SessionOpen` beyond `--max-sessions` gets `transport_session_full`. Shutdown, on a stop, idle exit, `SIGTERM` or `SIGINT`, runs server §6's five steps, the host context's `shutdown` (CS1.9, CS7.12) at step 4, while the draining server refuses `SessionOpen` with `server_unavailable`, naming the stop, and still answers `ServerControl`; the signal handling is a `permanent` entry under §3.0's rule. Idle exit counts only time with no open session, and a server that refused because a file read once changed exits at its next idle point.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): a snapshot's `insert` is quadratic in its entries, and the server design bounds a `SessionOpen` only by its 64 MiB frame. The host keeps that bound, and a test shows that `open` of a snapshot at the bound returns within the client's 30-second handshake bound. The test's snapshot is the worst case: the most minimum-size entries a 64 MiB frame can carry.
  - Test-first: server §12, gwz-core (`ServerControl`'s `status` and `stop` answered by a socket host, at `--max-sessions` too; shutdown runs its five steps, calling the host context's `shutdown` at step 4, and logs pending counts without configuration values, with a pending disposal injected until CS8.32 runs the row with real instances); the socket host (the peer check; the sandbox refusal of a client double that skips its own check, under `unshare` or bubblewrap or a seccomp filter on Linux and `sandbox-exec` on macOS, with the host's own namespaces and filters varied through a server double; `server start` inside a sandbox refused on all three platforms); the OpenSSL configuration rows that start a server (at an explicit address, a configuration changed after the start refuses the next session, and the server exits when its last session closes; under `auto`, an in-place edit starts a new server; a client whose scan cannot read an include refuses `auto` before any connect; on Linux, a changed CA bundle refuses a native HTTPS operation and not a transport one).
  - Depends on CS8.1, CS8.2, CS8.8 and CS8.9. Review: dual, in the socket-host checkpoint with CS8.9: its accept path is where the peer check must come before any byte is read (S-P3-2). Not **Ordinary path**.

- **CS8.11 — The server's log** *(gwz-core; < 250 lines)*.
  - Files: new `src/session_host/server/log.rs`.
  - Implements server §6's logging: one line per event (a connection accepted or refused, with the peer's process ID and the reason; a session opened or closed; a status or stop request; an operation's method, ID, outcome and duration; shutdown's pending counts); never an environment value, request body, credential, remote URL, repository name or item of the plan's redaction list, and a refused agent pipe only by the rule it failed; at 10 MiB, the log renamed to `<log>.1` through the directory's descriptor, replacing any previous one, and a new file created under the file rule; a server that cannot rotate says so and shuts down at its next idle point.
  - Test-first: server §12, the socket host (a log past 10 MiB is renamed, and a link planted at `<log>.1` is replaced, never followed); the log-redaction row, core half (a scan of the log against the plan's redaction list finds nothing).
  - Depends on CS8.9 and CS8.10. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.12 — The stdio host** *(gwz-core; < 300 lines)*.
  - Files: new `src/session_host/server/stdio_host.rs`.
  - Implements server §15's host: it moves the channel off the standard descriptors before anything else runs (on POSIX, `F_DUPFD_CLOEXEC`, then descriptors 0 and 1 onto the null device; on Windows, non-inheritable duplicates, `SetStdHandle` to `NUL`, and the originals' inherit flag cleared); it sends its `SessionHello` at once and serves one session through `serve_session`, with the must-match check against its own values, captured at start, and no configuration scan; it refuses `ServerControl`; it applies the session's libgit2 timeout as in-process does; after the session ends it calls the host context's `shutdown` and exits, 0 after `session.close` and 1 otherwise, without waiting for detached workers; it opens no socket and takes no lock; it refuses to start when its standard input or output is a terminal.
  - Test-first: server §12, the stdio host (transport plan Phase 7 exit): end of input cancels live work, and the process exits with no helper left; a child that writes to its standard output or reads its standard input cannot touch the stream; it sends `SessionHello` first and refuses `ServerControl` with `invalid_request`.
  - Depends on CS5.4 and CS8.1. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.13 — The launcher** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/server/launcher.rs`, with its POSIX arms; the Windows arms are CS8.18's.
  - Implements server §2's launcher, with §6's, §15's and §16's rules: it starts a copy of a CLI in the background (a new session; standard output and error on the log the starter opened under the file rule; standard input on the null device), or as a stdio child (pipes created close-on-exec; a new session), and it starts `ssh` (its resolved program, with `SIGINT` and `SIGQUIT` ignored); only the standard streams cross (`close_range(3, ~0U, CLOSE_RANGE_CLOEXEC)` on Linux, falling back to the descriptors `/proc/self/fd` lists, and `POSIX_SPAWN_CLOEXEC_DEFAULT` on macOS); the child's working directory is the one the driver names, never the caller's; the program, its arguments and its environment come from the driver. A program that cannot start fails with `server_unavailable`, naming it and the operating system's error. Its spawns are `permanent` entries under §3.0's rule, with the row below as their test.
  - Test-first: server §12, the socket host's descriptors row (a starter that holds an `flock`ed descriptor and a pipe's write end starts a server, and once the start returns another process can take the lock and the pipe's reader sees end of file; the same for the stdio child), run over a test program that the launcher starts in the server's place, so that the `permanent` entries land proven at this step.
  - Depends on CS1.10, which adds `server_unavailable`. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.14 — The socket client channel** *(gwz-core; < 450 lines)*.
  - Files: new `src/session_host/server/client.rs`.
  - Implements server §3's client and §4's listener verification, at an explicit address: the channel parses the address (CS8.5), applies the caller's own sandbox check (CS8.8), walks it (CS8.6, CS8.7), and checks the socket's directory and file; it connects without blocking within the handshake's bound, retrying `EAGAIN` on Linux and reporting a refusal on macOS as nothing answering; it then checks the peer's credentials and the listener's process under the sandbox rule, refusing with `server_listener_refused` and sending no frame; it reads the host's first frame, refusing one over 4 KiB or with another tag with `server_unavailable` and its first bytes escaped; on the session path it refuses a protocol range it does not speak, and a core of another version or build, with `server_version_mismatch`, before the snapshot crosses; it sends one `SessionOpen`, or `ServerControl`, and waits at most 30 seconds for `SessionOpened`, or 5 for `ServerState`, counting from the connect; at the bound it disconnects and fails with `server_unavailable`, naming the listener's process from the kernel or, marked as such, from its lock file. The channel implements `open`, `send` and `recv` (§3 as amended).
  - Test-first: server §12, the handshake's bound, on all three platforms (a host double that accepts and never answers fails a client command within 30 seconds, and `status` and `stop` within 5, naming its process, lock file and log, with no frame after the connect; one that sends `SessionHello` and never answers `SessionOpen` fails within 30 seconds; echo and silent doubles at an explicit address, and one whose `SessionHello` gives another core, receive no environment byte; a first frame over 4 KiB or with another tag fails at once; a listener that never accepts, its queue filled, fails within the bounds on Linux and reports nothing answering on macOS); listener verification at an explicit address (a listener owned by another user, and one of the same user inside a sandbox, receives zero bytes, transport plan Phase 7 exit; a listener double in either of two seccomp-only sandboxes is refused by an unsandboxed client; a per-user directory with another owner, a group bit or a link is refused before any connect).
  - Depends on CS1.3 and CS1.10, whose framing and frames it speaks, and on CS8.8 and CS8.9. Review: dual, the client channel: the snapshot crosses only after it has verified the listener. Not **Ordinary path**.

- **CS8.15 — `auto`, auto-start and the byte-stream client channel** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/server/auto.rs` and `stream_client.rs`.
  - Implements server §4's default address and §6's auto-start: the per-user directory from the driver's captured `XDG_RUNTIME_DIR` or `TMPDIR`, walked, else `/tmp/gwz-<uid>/`; `server-<key>` on POSIX, and on Windows the pipe the lock file records; when nothing answers, the client starts one server through the launcher (CS8.13) and verifies it within 5 seconds. Also server §15's client end of the stdio local form: the byte-stream channel over a child's standard streams, which reads the child's `SessionHello` and opens the session within the handshake's bound, and after `session.close` waits up to 10 seconds for the child before killing it.
  - Test-first: listener verification at the `auto` path (a listener owned by another user, and a sandboxed one of the same user, receives zero bytes; transport plan Phase 7 exit); server §12, gwz-cli's lifecycle, core half (two auto-starts racing leave one server; a different `HOME` under `auto` starts a second server rather than being refused); the stdio rows, client half (end of input cancels live work, and the child exits with no helper left).
  - Depends on CS8.2, CS8.10, CS8.13 and CS8.14; its auto-start rows start a test program built on CS8.10's host, and the CLIs' own starts come with CS8.22 and CS8.29. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.16 — The SSH remote form** *(gwz-core; < 400 lines)*.
  - Files: new `src/session_host/server/ssh_form.rs`.
  - Implements server §16's questions 2, 3, 7 and 9: `ssh` is resolved to an absolute path through the driver's captured `PATH` alone, skipping empty and relative elements, and on Windows as `ssh.exe` alone, with no `PATHEXT`, current directory or App Paths; it runs directly, never through a shell, with the vector `-T`, `-o BatchMode=yes` when the client has no terminal, `-p` for a given port, `--`, the destination and the fixed remote command, `gwz server stdio` or `gwz-py server stdio`; the client sends no environment, so its `SessionOpen` carries `host_environment` true and no snapshot, `umask` or `openssl`, and the off switch travels as `transport_off`; it checks only the protocol range; with no terminal it waits at most 30 seconds from the spawn to `SessionOpened`, then kills `ssh` and waits for it; with a terminal it sets no bound before `SessionOpened`, and the first interrupt kills `ssh`; question 7's errors, the status-127 and first-output messages included.
  - Test-first: server §16 question 8, core half (a spawn double records zero spawns for a refused user or host; with a planted `ssh` in the caller's directory, or `.` absent from `PATH`, the form runs the recording `PATH` program; with no terminal, an unknown host key fails within the bound and leaves no `ssh` process; a banner from the remote shell fails within the bound with the start-up output message; status 127 gives its message; no environment entry leaves the client, and the `SessionOpen` carries the marker).
  - Depends on CS8.5, CS8.13 and CS8.15. Review: dual, the SSH remote form: its host, user and program checks stand between a `--server` value and a command on another machine (server §16; CVE-2023-51385, CVE-2017-1000117). Not **Ordinary path**.

- **CS8.17 — Windows: the pipe host, its directory and its lock** *(gwz-core; < 450 lines)*.
  - Files: the Windows arms of `src/session_host/server/host_files.rs` and `socket_host.rs`.
  - Implements server §11's host primitives: the `auto` pipe name, `\\.\pipe\gwz-` and 32 hexadecimal digits from the system's cryptographic generator, drawn at each start, created with `FILE_FLAG_FIRST_PIPE_INSTANCE`, drawn again at most three times, and recorded in the lock file only once the pipe exists; `PIPE_REJECT_REMOTE_CLIENTS`, and a security descriptor that grants only the user's SID; the per-user directory `gwz\` under `SHGetKnownFolderPath(FOLDERID_LocalAppData)`, on a local fixed drive and never a UNC or device path, with an ACL for the user, SYSTEM and Administrators; the file rule with `FILE_FLAG_OPEN_REPARSE_POINT`, a reparse point refused, and the user as owner; `LockFileEx` over a byte range past the record; `--log` on a local fixed drive.
  - Test-first: server §12, the socket host on Windows (a per-user directory whose ACL grants another user refused; a `LOCALAPPDATA` naming a UNC path not used; a junction and a symbolic link at the log path refused; a pipe another user creates first under any name does not stop the user's `auto` server, a pipe created later under a name seen in the listing is never used, and with such a stale record a start takes a new name and a status finds it; the name comes from the cryptographic generator, and a pipe created first under the first drawn name makes the host draw another).
  - Depends on the transport plan's Phase 4 (1.1.0 S4.1–S4.5), CS8.9 and CS8.10. Review: dual, in one Windows interface checkpoint with CS8.18 and CS8.19 (AgentProcessRules §6.1): together they re-freeze CS8.5, CS8.8 and CS8.14 on Windows (S-P3-2), and the checkpoint's review re-runs those steps' counterexamples on Windows. Not **Ordinary path**.

- **CS8.18 — Windows: verification, the sandbox rule and the launcher** *(gwz-core; < 450 lines)*.
  - Files: the Windows arms of `sandbox.rs`, `socket_host.rs`, `client.rs` and `launcher.rs`.
  - Implements server §4's and §11's Windows checks: the host's peer check through `GetNamedPipeClientProcessId`, a handle whose process began before the connection, and the user SID in its token, refusing an AppContainer client and one below medium integrity or below the host's own; the client's open with `SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION`, then `GetNamedPipeServerProcessId`, whose process token must name the caller's SID, must not be an AppContainer's and must have the client's own integrity, and which fails closed if it does not work on a client's handle, server §14's fallback then being designed and tested; a busy pipe waited for with `WaitNamedPipeW` within the bound; the logon session read from the client's token at the peer check, for the native row; the launcher's arms (for the background copy, `DETACHED_PROCESS` and `CREATE_NEW_PROCESS_GROUP` with only the log and null-device handles; for the stdio child, `CREATE_NEW_PROCESS_GROUP`, which also ignores `CTRL_BREAK_EVENT`, with the pipe ends and standard error; both through `PROC_THREAD_ATTRIBUTE_HANDLE_LIST`, leaving the caller's job where the job allows it).
  - Test-first: server §12, Windows on dabeest (`GetNamedPipeServerProcessId` works on a client's handle, or verification fails closed and the fallback is tested; a medium-integrity pipe server passes, and low-integrity and AppContainer servers are refused; an elevated client against a medium server is refused before any frame, and a medium client against an elevated server by the server); process IDs on Windows; the socket host on Windows (a remote client, an AppContainer client, a low-integrity client and a medium client of an elevated server refused); Windows' logon session (a client in another logon session has its transport operations run and a routed operation refused before any connection opens, naming the logon session); a background start inside a job object.
  - Depends on CS8.13, CS8.14 and CS8.17. Review: dual, in the Windows interface checkpoint with CS8.17 and CS8.19. Not **Ordinary path**.

- **CS8.19 — Windows: name rules for addresses and agents** *(gwz-core; < 300 lines)*.
  - Files: the Windows arm of `src/session_host/server/address.rs`; the Windows module of `src/git/endpoint/agent_socket.rs`, the agent channel 1.1.0 S4.3 adds.
  - Implements server §3's checks after the grammar (`GetFullPathNameW` returns the name unchanged; `QueryDosDeviceW(L"PIPE")` returns `\Device\NamedPipe`) and server §5's agent rules, on top of S4.3: one agent source per session, the snapshot's `SSH_AUTH_SOCK` or, when it is unset or empty, `\\.\pipe\openssh-ssh-agent`; Pageant only when `SSH_AUTH_SOCK` names its pipe; the local-pipe rule before any open, and for another form the local-path rule; a refusal with `permission_denied` whose message, log line and Python-visible error name `SSH_AUTH_SOCK` and the rule, never the value, and end with the next step (server §17).
  - Test-first: server §12, the address parser and Windows agent pipes (every name the grammar accepts comes back unchanged from `GetFullPathNameW`, on dabeest; a Pageant-style name with a space and a non-ASCII letter passes the agent rule and fails the address rule; a remote agent pipe, and a UNC path in another form, are refused before any open with `permission_denied`, holding no substring of the refused name); an error-text row for each value of each rule's `<rule>`, in-process and through a server.
  - Depends on the transport plan's Phase 4 (1.1.0 S4.1–S4.5, S4.3 among them), CS3.2 and CS8.5. Review: dual, in the Windows interface checkpoint with CS8.17 and CS8.18. Not **Ordinary path**.

- **CS8.20 — gwz-cli: server selection and the client forms** *(gwz-cli; < 450 lines)*.
  - Files: `src/globalargs/parser.rs`; `src/session_driver.rs`; new `src/server_client.rs`.
  - Implements server §7's selection: `--server`, `--no-server` and `GWZ_SERVER` are parsed at startup, before anything else runs; with none of them the CLI runs in-process exactly as before; `--server auto`, an address, `stdio` or the SSH remote form selects the channel (CS8.14–CS8.16), and a server asked for but unusable fails the command, naming the cause and `--no-server`, with no fall-back to in-process; `--no-server` runs in-process without parsing `GWZ_SERVER` and cannot be combined with `--server`; the `hook` family never reads `GWZ_SERVER` and refuses `--server` as a usage error; the session driver (CS6.1) opens its session over the chosen channel, with its resolved off switch as `ProcessAttributes.transport_off` (CS6.6), and renders as in-process; `--ssh-timeout` travels as `configure_transport_runtime`; the stdio local form starts the CLI's own executable with `server stdio` through core's launcher. Every surface sits in the switch's `cfg_if` arms.
  - Test-first: server §12, gwz-cli (in every mode, a working directory that is not valid Unicode is refused before anything is sent, and relative `--root` values and operands arrive at the server as the absolute paths an in-process run resolves; `GWZ_SERVER` holding each refused form is refused before any open, and `--server` with `--no-server` is a usage error; the `hook` family runs in-process with `GWZ_SERVER` set and refuses `--server`).
  - Depends on the Phase 6 exit and CS8.14–CS8.16. Review: single-axis, Safety first. Not **Ordinary path**: behind the switch.

- **CS8.21 — gwz-cli: the interrupt protocol and the SSH remote form's commands** *(gwz-cli; < 300 lines)*.
  - Files: `src/session_driver.rs`, `src/diff_exec.rs`, `src/log_exec.rs`, `src/forall.rs`, `src/hook_exec.rs`.
  - Implements server §7's interrupt protocol (the first interrupt sends `operation.cancel` and waits up to the close bound, and a second disconnects at once) and §16's question 5: `forall`, `init --update` while it has no protocol method (D3), and the `hook` family are refused under the SSH remote form before `ssh` runs, as usage errors with server §17's messages; `diff` and `log` compute their workspace-relative directory from the remote directory and `--root`, as text; the pager is chosen before the session.
  - Test-first: server §12, the stdio rows (the terminal's interrupt reaches only the client: a pseudo-terminal test on Linux and macOS, and a console test on Windows, show the first interrupt cancelling by protocol while the child keeps its stream); server §16 question 8 (`forall`, and each input question 5 refuses, are refused before `ssh` runs).
  - Depends on CS8.20. Review: single-axis, Consistency first. Not **Ordinary path**: behind the switch.

- **CS8.22 — gwz-cli: `server start` and `server stop`** *(gwz-cli; < 450 lines)*.
  - Files: new `src/server_cmd/mod.rs`, `start.rs` and `stop.rs`.
  - Implements server §6's `start` and `stop`. `start` probes first, with each outcome server §6 lists; it starts a copy of itself with `server start --foreground`, the same options, `--log` resolved and `--ssh-timeout`, through core's launcher, and returns once the address answers a well-formed `SessionHello` and passes listener verification within 5 seconds. `--foreground` serves through core's socket host (CS8.10), after the CLI captures its own environment and mask, sets the libgit2 timeout from `--ssh-timeout`, records its OpenSSL library and scan, resolves its certificate variables on Linux and reads its logon session on Windows (server §5). `stop` sends `ServerControl`'s stop and waits for the process through a handle taken while the connection is open, up to 185 seconds; `--force` ends the drain, with server §17's warning. Outputs, exit codes and JSON records are server §17's.
  - Test-first: server §12, gwz-cli's lifecycle (start and stop are idempotent; `start` with options A, then B, is refused; `stop` with an open session: the bound, the message at the bound, and `--force`'s behaviour and warning; a client arriving during a drain gets `server_unavailable`, never `server_session_closed`, and the old process exits within its bound; two auto-starts racing leave one server; idle exit, which a `status` loop does not delay; an explicit address refusing a different `umask`; `--max-sessions` and its message at the cap; shutdown with a latched worker; `--foreground --log PATH` logging to the file, with standard error quiet; a server and a client both with no options run a native-path operation unrefused).
  - Depends on CS8.20, CS8.10, CS8.11 and CS8.13, and on CS8.3, so no server a CLI starts serves a native route before those rows run through a server. Review: single-axis, Consistency first. Not **Ordinary path**: behind the switch.

- **CS8.23 — gwz-cli: `server status`, `list` and `stdio`** *(gwz-cli; < 400 lines)*.
  - Files: new `src/server_cmd/status.rs`, `list.rs` and `stdio.rs`.
  - Implements server §6's `status`, which exits 1 when nothing answers; `list`, which reads each lock file in the per-user directory under the file rule, parses and walks each recorded address, probes it within 5 seconds, marks the `auto` server, and lists not-answering, refused and protocol-range rows; and `stdio`, core's stdio host, which refuses every option, `--server` included, and a terminal on its standard input or output with server §17's message. Outputs and JSON records are server §17's.
  - Test-first: server §12, gwz-cli's lifecycle (`status` exits 1 when nothing answers, and reports A after the A-then-B start; two servers at two keys are listed, and each stops by its address; a server double of another core is listed and stopped, while a command through it is refused with `server_version_mismatch` and no environment byte is sent; a double whose protocol range excludes the client is listed as refused, and `status` and `stop` fail with `server_version_mismatch`); the stdio rows (`server stdio` refuses every option and a terminal; with `GWZ_SERVER` set to `stdio`, to an `ssh://` form and to a refused path in the child's environment, the session opens each time).
  - Depends on CS8.22 and CS8.12. Review: single-axis, Consistency first. Not **Ordinary path**: behind the switch.

- **CS8.24 — gwz-cli: help, messages and `--verbose`** *(gwz-cli; < 400 lines)*.
  - Files: `src/help.rs`, `src/cli_reference.rs`, `src/response_meta_json.rs`, and the error renderer.
  - Implements server §17's surface for gwz-cli: the frozen help strings, inside the switch's arms, so the ordinary build's help is unchanged; bare `gwz server` printing the subcommands; each subcommand refusing another's options and `--dry-run` as usage errors, and `--jsonl` giving `--json`'s records; every error's template and hint, per class and per kind; `--verbose`'s server line in human mode, and `meta.server` with `--json` or `--jsonl`.
  - Test-first: server §12, gwz-cli's command surface (the subcommand listing; help snapshots of `gwz --help`, `gwz help server` and each subcommand's `--help`; `--ssh-timeout`'s default of 9 in `gwz server start --help` and in another command's help; the per-user directory and the grammar in `gwz help server`, checked against the parser; the `already running` line with its `--idle-exit`; `server stdio` on a terminal; the error text for each class of `server_address_refused`, the four kinds of `server_environment_mismatch` at open, a `server start` failure with no `--no-server` hint and a pointer to the log, and the file-read-once message; `meta.server` present with `--verbose --server auto --json` and absent without `--verbose`; an error-text row for each command the SSH remote form refuses, since OD12 is yes); the ordinary build's help and CLI reference are unchanged.
  - Depends on CS8.22 and CS8.23, and on CS7.25, which changes `response_meta_json.rs` first (§4). Review: single-axis, Consistency first. Not **Ordinary path**: behind the switch.

- **CS8.25 — gwz-cli: the server CI** *(gwz-cli CI and tests; < 400 lines)*.
  - Files: `.github/workflows/ci.yml` and `platform-gate.yml`; the test harness that starts a server per test.
  - Implements server §12's gwz-cli CI: the suite runs twice, in-process and with `GWZ_SERVER` set to a server the harness starts per test from the binary under test, with `server start --foreground` and that test's environment, and any difference between the runs is a defect; it runs once more through the stdio local form on Linux; Linux server jobs run on runners §2.4 allows, each asserting it first.
  - Test-first: server §12, gwz-cli (the two runs and the stdio run; the squatted-listener refusal, a listener owned by another user at the `auto` path and at an explicit address receiving zero bytes; a sandboxed caller with `GWZ_SERVER` set refused on all three platforms, at the `auto` path, at an explicit address and with `stdio`, naming the cause and `--no-server`, with no server started; the soak, many client processes against one server, with its memory recorded for TR8.3; log redaction against the plan's list).
  - Depends on CS8.24. Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.26 — gwz-py: the extension's server bindings** *(gwz-py; < 450 lines)*.
  - Files: new `native/src/server.rs`; `native/src/lib.rs`.
  - Implements §9 as server §8 amends it: the extension exposes core's socket host for `gwz-py server`, serving with the GIL released and handling `SIGTERM` and `SIGINT` itself, a `permanent` entry under §3.0's rule; the stdio host; the client socket channel, for `SocketCoreBridge`; the byte-stream client channel, for the CLI's private stdio client; and core's launcher, which spawns the copies Python composes, and `ssh`. All of it sits in the switch's `cfg_if` arms and fails closed without it (transport plan §6(a)).
  - Test-first: the socket channel refuses `stdio` and `ssh://` with `server_address_refused` before any open; a copy spawned through the extension gets only its standard streams; with the switch off, each binding refuses.
  - Depends on the Phase 4 exit, CS8.10, CS8.12, CS8.13, CS8.14 and CS8.15. Review: single-axis, Safety first. Not **Ordinary path**: behind the switch.

- **CS8.27 — `SocketCoreBridge`** *(gwz-py; < 400 lines)*.
  - Files: new `src/gwz/socket_bridge.py`; `src/gwz/errors.py`; `src/gwz/__init__.py`.
  - Implements server §7's library client, and §10 as amended: `NativeCoreBridge`'s call table and pump over the extension's socket channel; `address` required, `auto`, a socket path or a pipe name, as `str` or `os.PathLike[str]`; `GWZ_SERVER` never read; `stdio` and `ssh://` refused; the environment captured at `open`, as `NativeCoreBridge` does; the mask read from `/proc/self/status` on Linux, and elsewhere set to `0o077` and restored; the OpenSSL library the extension loaded; the off switch's resolved value as `ProcessAttributes.transport_off`, as CS4.5 resolves it; no timeout; with `auto`, a server started as gwz-py's CLI starts one, running `sys.executable` on `gwz.cli` with the interpreter's isolation flags, from the directory that holds the `gwz` package; the closed-session code `server_session_closed`, and `server_unavailable` for a channel that closes before `SessionOpened`.
  - Test-first: server §12, gwz-py (its Client-level tests through `SocketCoreBridge`, beside the `NativeCoreBridge` and `StreamCoreBridge` runs; every bridge refuses a working directory that holds a lone surrogate; `SocketCoreBridge` refuses `stdio` and `ssh://`, and never reads `GWZ_SERVER`; a copy never imports a `gwz` package planted in the caller's directory; the mask capture never exposes a more permissive mask to another thread; under `auto`, a session that configures 30 seconds gets the library's timeout message on a `git://` remote; the closed-session code on each of the three bridges; an interpreter that `sys.executable` names and cannot run gives the documented message).
  - Depends on CS8.26 and CS4.7. Review: single-axis, Safety first. Not **Ordinary path**: it fails closed without the switch.

- **CS8.28 — gwz-py: selection, the client forms and the private stdio client** *(gwz-py; < 400 lines)*.
  - Files: `src/gwz/cli.py`, `src/gwz/cli_shared.py`; new `src/gwz/_stdio_client.py`.
  - Implements server §7's selection in gwz-py's CLI (`--server`, `--no-server` and `GWZ_SERVER`, parsed at startup, with gwz-cli's rules) and server §15's local client form: a private client, `NativeCoreBridge`'s call table and pump over the extension's byte-stream client channel, neither exported nor documented, which starts gwz-py's own interpreter on `gwz.cli` with `server stdio` through the extension's launcher. Every form carries the CLI's resolved off switch as `ProcessAttributes.transport_off`. The SSH remote form's remote command is `gwz-py server stdio`.
  - Test-first: server §12, gwz-py (its CLI suite's stdio local-form rows; the refused forms from `GWZ_SERVER`; a copy never imports a planted `gwz` package).
  - Depends on CS8.26 and CS8.27. Review: single-axis, Consistency first. Not **Ordinary path**: behind the switch.

- **CS8.29 — gwz-py: `gwz-py server`** *(gwz-py; < 450 lines)*.
  - Files: new `src/gwz/cli_server.py`; `src/gwz/cli.py`.
  - Implements server §6 and §17 for gwz-py: the `server` family, `start`, `stop`, `status`, `list` and `stdio`, behaving as gwz-cli's through the extension (CS8.26), with every frozen string naming `gwz-py` where gwz-cli's names `gwz`; the command list gains `server`, with server §17's summary; a background start composes the copy's argument vector in Python, and the extension spawns it.
  - Test-first: server §12, gwz-py (its lifecycle and command-surface rows with gwz-py's strings; help-snapshot and error-text rows assert `gwz-py`, never `gwz`, in the `server` summary, the `server stdio` terminal refusal, the different-options refusal, the stop-bound message and the error prefix).
  - Depends on CS8.28, and on CS8.3, so no server gwz-py starts serves a native route before those rows run through a server. Review: single-axis, Consistency first. Not **Ordinary path**: behind the switch.

- **CS8.30 — gwz-py: the server CI** *(gwz-py CI and tests; < 300 lines)*.
  - Files: `.github/workflows/package-smoke.yml`; `run_tests.py`.
  - Implements server §12's gwz-py CI: its CLI suite runs twice, in-process and against `gwz-py server start --foreground`; its stdio local-form rows run; its Client-level tests run through `SocketCoreBridge`; Linux server jobs run on runners §2.4 allows, each asserting it first; and gwz-py's test run checks gwz-py's allowlist in CS8.4's no-debt mode, off until the Phase 8 exit's gate first passes and on from then.
  - Test-first: the runs themselves; any difference between a suite's two runs is a defect; with the mode on, a fixture `env` `debt` entry in gwz-py's allowlist fails gwz-py's run.
  - Depends on CS8.29, and on CS5.3, which changes `package-smoke.yml` and `run_tests.py` first (§4). Review: single-axis, Safety first. Not **Ordinary path**.

- **CS8.31 — The SSH remote form, end to end** *(both CLIs' suites; < 300 lines of fixtures)*.
  - Files: gwz-cli's and gwz-py's test suites; a disposable loopback `sshd` fixture.
  - Implements the rows of server §16 question 8, which OD12's "If yes" texts add to the transport plan's Phase 7 exit, with no live account: against a disposable `sshd` on Linux and macOS, with the binary under test first on its `PATH`, and with the Windows OpenSSH client on dabeest.
  - Test-first: server §16 question 8 (one network operation with the remote account's credentials; no environment entry leaves the client, and its `SessionOpen` carries the marker; a socket host refuses the marker; `forall`, and each input the form refuses, are refused; closing the client ends the remote session and leaves no helper; the allowlist rows with zero spawns; the planted-`ssh` rows; `GWZ_SERVER` holding the form refused before `ssh` runs; with no terminal, an unknown host key fails within the bound and leaves no `ssh` process; after `SessionOpened`, the first interrupt cancels by protocol and `ssh` survives it; a banner's start-up output message; a remote host without gwz on its non-interactive `PATH` gives the status-127 message; help and `--verbose` rows for the form).
  - Depends on CS8.21 and CS8.28. Review: part of the Phase 8 exit. Not **Ordinary path**: tests of surfaces behind the switch.

- **CS8.32 — Reuse and the off switch through a server** *(both CLIs' suites; < 250 lines of fixtures)*.
  - Files: gwz-cli's and gwz-py's server suites.
  - Implements the transport plan's Phase 7 exit rows that need reuse or the off switch.
  - Test-first: server §12, reuse through a server (a second command through a server reuses the first command's connection: its row has `reused = true` and the first row's `connection_id`, and the fixture's accept count does not change), with reuse §15.1 across two clients of one server; server §12, gwz-core (shutdown disposes every endpoint instance within the close bound, reuse §15.11's server half); server §12, the off switch (a server started with one `SSH_AUTH_SOCK`, and a client with another and the off switch on, run once with the switch as a flag and once as an environment variable; both runs are refused at `SessionOpen`, and the server's agent records zero signature requests).
  - Depends on the Phase 7 exit, TR2.5, CS8.25 and CS8.30. Review: part of the Phase 8 exit. Not **Ordinary path**: tests of surfaces behind the switch.

**Exit.** The server design's §12 verification passes on all three platforms: gwz-core's rows in its CI, Linux on the runners §2.4 allows; gwz-cli's and gwz-py's suites each run in-process and through a server, and gwz-cli's once more through the stdio local form on Linux; the stdio rows in both CLIs; the Windows rows on Windows CI and on dabeest; the SSH remote form's rows (CS8.31); and the soak. That covers the transport plan's Phase 7 exit rows, as its amendment's §3.6 and OD12's "If yes" texts extend them: the squatted-listener refusal, the off switch as a flag and as a variable, the sandboxed caller, reuse through a server (CS8.32), refused addresses, sandboxed listeners, native routes, the handshake and the stdio mode. The workspace run, in which each CLI is the client of the other's server, is recorded. gwz-core's checker, in its new mode, reports no `env` or `process` `debt` over gwz-core's allowlist, and none over gwz-py's: the server design's release gate, as this phase's preamble states it (server §5, §12). The exit's settled tree then switches the mode on for good, in gwz-core's `run_tests.py` (CS8.4) and in gwz-py's test run (CS8.30). The server cells' evidence (server §12's cell table) is filed for S5.6 (transport plan Phase 8). Review: dual, on the settled tree. No Surface: every server surface stays behind the switch until activation, where S7.5's Surface review covers it (transport plan Phase 9).

## 4. Dependency sketch and parallel lanes

```text
G0 (discharged, §2.1) ── G1, TR1.4a's review (§2.2) ── the steps revisions 1 and 2 revise, from Phase 1
TR1.2 + TR1.3 (closed) ── G1, TR1.4b's review ──────── the steps revision 3 revises or adds
TR3.1 (G2) ─────────────── transport rows, and steps that change candidate-only code (CS1.1 and CS1.10 among them)
1.1.0 S4.5 (G2) ────────── Windows transport rows;  transport plan Phase 4 (1.1.0 S4.1–S4.5) ── CS8.17, CS8.18, CS8.19
contract revision 5 (C8), accepted ── CS1.1
TR2.1 ── CS7.23;  TR2.8 ── CS8.3's TR2.8 row;  TR1.6 + TR2.2 ── CS8.3's TR1.6 row;  TR2.5 ── CS8.32

Phase 1
CS1.1 ──┬── CS1.2 ── CS1.3
        ├── CS1.10
        └──────────────────────── CS1.6
CS1.4 ──┬──────────────────────── CS1.6
CS1.5 ──┴── CS1.9  (CS1.4 and CS1.5 freeze together; CS1.9 re-freezes them)
CS1.7       (independent; CS8.4 and CS8.8 follow it)
CS1.8       (done: revision 4 applied B11, §2.1)

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
C   CS3.1 ──┬── CS3.2 ── CS3.7 (after CS1.9, CS3.5, CS3.6) ── CS3.8 (after CS2.13) ──┐
            ├── CS3.3                                                                │
            ├── CS3.4                                                                │
            ├── CS3.5 ───────────────────────────────────────────────────────────────┤
            └── CS3.6 ───────────────────────────────────────────────────────────────┤
    CS2.8 + CS2.9 ── CS3.9 ──────────────────────────────────────────────────────────┤
    Phase 2 exit + CS1.9 ────────────────────────────────────────────────────────────┴── CS3.10 ── CS3.11 ── Phase 3 exit

Phase 4
D   CS4.1 ────────────────────────────────────────┐
    CS1.10 ── CS4.3 ── CS4.4 ── CS4.5 ── CS4.6 ───┤
    CS2.4 + CS2.13 + CS1.9 ── CS4.2 ──────────────┼── CS4.7 ── CS4.8 (after CS5.2) ── Phase 4 exit
    Phase 3 exit ─────────────────────────────────┘
    Phase 4 exit + Phase 7 exit ── CS4.9 (single-axis plus Surface, TR1.7)

Phase 5
E   CS1.3 + CS1.9 + CS1.10 + CS2.3 + CS2.4 ── CS5.4 ── CS5.1 ──┐
    CS4.2 + CS4.3 ──────────────────────────────────── CS5.2 ──┴── CS5.3 (after CS4.7, CS4.8) ── Phase 5 exit

Phase 6
F   Phase 2 exit + CS3.1 ── CS6.1 ── CS6.2 ── CS6.3 (D3) ── CS6.4 (after Phase 3 exit) ── CS6.5 (after CS4.8) ── Phase 6 exit
    CS6.1 + CS1.9 ── CS6.6;  CS6.4 + CS6.6 ── CS6.7 ── Phase 6 exit

Phase 7 (every step after the Phase 3 exit)
RT  CS7.2 ──┬── CS7.3 ──┬── CS7.6
            └── CS7.4 ──┴── CS7.5
RC  CS7.1 ──┬── CS7.7 ──┐
            └── CS7.8 ──┴── CS7.9 ──┬── CS7.11 ── CS7.12
                                    └── CS7.10 (after CS7.2–CS7.6) ──┬── CS7.21 (after CS7.6)
                                                                     ├── CS7.14 (after CS7.12) ── CS7.24 (after CS3.10)
                                                                     └── CS7.20 ── CS7.13 (after CS7.12) ── CS7.22 (after CS7.16)
RA  CS7.8 + CS7.11 ── CS7.15 ── CS7.16 (after CS3.6)
    CS7.1 ── CS7.17 ── CS7.18 ──┐
    CS7.4 + CS7.5 + CS7.11 ─────┴── CS7.19 (after CS7.22) ── CS7.23 (after TR2.1, CS7.20)
CLI Phase 3 exit ── CS7.25
    all other Phase 7 steps ── CS7.26 ── CS7.27 ── Phase 7 exit (CS7.25 included)

Phase 8
SC  CS5.4 ── CS8.1 ──┬── CS8.2
                     └── CS8.4 (after CS1.7, CS3.4)
    CS1.10 ──┬── CS8.5 ──┬── CS8.6 ──┐
             │           └── CS8.7 ──┴── CS8.9 ──┐
             ├── CS8.8 (after CS1.7) ────────────┤
             └── CS8.13                          │
    CS8.1 + CS8.2 ───────────────────────────────┴── CS8.10 ──┬── CS8.11
                                                              └── CS8.3 (after CS3.10; handoff with CS7.26)
    CS5.4 + CS8.1 ── CS8.12
    CS1.3 + CS1.10 + CS8.8 + CS8.9 ── CS8.14 ── CS8.15 (after CS8.2, CS8.10, CS8.13) ── CS8.16 (after CS8.5)
SW  transport plan Phase 4 + CS8.9 + CS8.10 ── CS8.17 ── CS8.18 (after CS8.13, CS8.14);  transport plan Phase 4 + CS3.2 + CS8.5 ── CS8.19
SL  Phase 6 exit + CS8.14–CS8.16 ── CS8.20 ──┬── CS8.21
                                              └── CS8.22 (after CS8.3, CS8.10, CS8.11, CS8.13) ── CS8.23 (after CS8.12) ── CS8.24 (after CS7.25) ── CS8.25
SP  Phase 4 exit + CS8.10 + CS8.12–CS8.15 ── CS8.26 ── CS8.27 (after CS4.7) ── CS8.28 ── CS8.29 (after CS8.3) ── CS8.30 (after CS5.3)
    CS8.21 + CS8.28 ── CS8.31;  Phase 7 exit + TR2.5 + CS8.25 + CS8.30 ── CS8.32
    all CS8.x ── Phase 8 exit
```

What can run at once:
- **After G0 and G1:** CS1.1 once TR3.1 has landed (G2), CS1.2's queues, CS1.4, CS1.5 and CS1.7; contract revision 5 is accepted (C8). CS1.3 follows CS1.2; CS1.6 follows CS1.1 and CS1.4. CS1.8 is done. Once revision 3 has GO, CS1.9 follows CS1.4 and CS1.5, and CS1.10 follows CS1.1.
- **After the Phase 1 freezes**, per step dependencies:
  - lane A, the host: CS2.1 → CS2.2 → CS2.3 → CS2.4 → CS2.6 → CS2.7 → CS2.12, with CS2.5 after CS2.2 and CS2.9 after CS2.3;
  - lane L, the logs: CS2.10 → CS2.11;
  - lane B, the routes: CS2.8, CS2.13, CS2.14 and CS2.15 at once, and CS2.16 after CS2.11;
  - lane C, isolation, once TR3.1 has landed (G2): CS3.1, then CS3.3–CS3.6 at once; once revision 3 has GO, CS3.2 → CS3.7 (after CS1.9, CS3.5 and CS3.6) → CS3.8 (after CS2.13); CS3.9 after CS2.8 and CS2.9;
  - lane D, the Python bridge, once revision 3 has GO: CS4.1, and CS4.3 (after CS1.10) → CS4.4 → CS4.5 → CS4.6, all against a fake channel;
  - lane SC, the server's core, once CS1.10 has merged, and CS1.7 for CS8.8: CS8.5, CS8.8 and CS8.13 at once, then CS8.6 and CS8.7 after CS8.5;
  - lane T, tooling: CS1.7, if not already done.
- **Beside Phase 2, once revision 3 has GO**, per step dependencies: CS4.2 once CS1.9, CS2.4 and CS2.13 have merged; lane E's CS5.4 once CS1.3, CS1.9, CS1.10, CS2.3 and CS2.4 have, then CS5.1, and CS5.2 once CS4.2 and CS4.3 have too; and lane SC's CS8.1 onwards after CS5.4.
- **After the Phase 2 exit:** lane F's CS6.1 (after CS3.1) → CS6.2 → CS6.3, with CS6.6 after CS6.1 and CS1.9; and CS3.10, after CS3.5–CS3.9 and CS1.9.
- **After the Phase 3 exit:** Phase 7's lanes RT, RC and RA, and CS7.25; CS4.7 → CS4.8 (after CS5.2); CS6.4 → CS6.7, and CS6.5 after CS4.8, never at the same time as CS7.13, CS7.15 or CS7.16 (the handoff below).
- **After the transport plan's Phase 4:** lane SW.
- **After the Phase 6 exit:** lane SL. **After the Phase 4 exit:** lane SP.
- **Serial tail:** CS3.11 → Phase 3 exit → CS4.7 → CS4.8 → Phase 4 exit → CS5.3 → Phase 5 exit, and lane SP; CS6.4 → CS6.5 (after the Phase 4 exit) → Phase 6 exit → lane SL; Phase 3 exit → Phase 7 → CS7.26 → Phase 7 exit → CS4.9; the Phase 7 exit and TR2.5 → CS8.32 → Phase 8 exit. Within Phase 7 the longest chain is CS7.9 → CS7.10 → CS7.20 → CS7.13 → CS7.22 → CS7.19 → CS7.23 → CS7.26, the order the owners of `ssh_worker.rs` impose.

Files that more than one step names. Every pair of steps that name one file is ordered by a path in the sketch, or the list names an L1-06 handoff for it: the two never change the file at once, and the lead hands it from one to the other ([TR1.4b's Verdict-2](GwzCoreSessionPlan-Verdict-2.md), S-P3-1).
- in gwz-core: `session_host/mod.rs` (CS1.4, CS1.2), `dispatch/mod.rs` (CS1.6, CS2.13), `host/admission.rs` (CS2.3, CS3.9), `host/worker.rs` (CS2.4, CS3.10), `dispatch/read.rs` (CS2.13, CS3.8, CS3.10), `session_host/context.rs` (CS1.4, CS1.9, CS3.7, CS3.8, CS7.12), `session_host/environment.rs` (CS1.5, CS1.9), `docs/RustApi.md` (CS1.4, CS1.9), `Cargo.toml` (CS1.5, CS7.18), `legacy.rs` (CS3.1, CS6.5), `transport_host/local_command.rs` (CS3.1, CS3.2, CS6.5), `transport_host/mod.rs` (CS1.1, CS3.1, CS3.2, CS7.10, CS7.14), `transport_host/request.rs` (CS3.7, CS6.5), `transport_host/session_entry.rs` (CS3.7, CS7.26, CS8.3), `transport_host/registry.rs` (CS3.7, CS7.11, CS7.12, CS7.26), `transport_host/instance.rs` (CS7.11, CS7.12, CS7.13), `transport_host/https_endpoint.rs` (CS7.8, CS7.15), the owners of `transport_host/session.rs` (CS7.1, CS7.9, CS7.10, CS7.13, CS7.19), of `ssh_worker.rs` (CS7.1, CS7.10, CS7.20, CS7.13, CS7.22, CS7.19, in that order) and of `https_worker.rs` (CS7.1, CS7.8, CS7.15, CS7.16, CS7.22), `git/endpoint/agent_job.rs` (CS3.5, then CS6.5 and CS7.13 under a handoff), `git/endpoint/https_auth.rs` (CS3.6, then CS7.15 and CS7.16, with CS6.5 under a handoff), `shared_reservation.rs` (CS7.10, CS7.21), `https_pool.rs` (CS7.20, CS7.13), `agent_auth.rs` (CS7.17), `agent_client.rs` (CS7.18), `docs/Embedding.md` and `docs/OperationModel.md` (CS2.12, CS7.26), `session_host/serve.rs` (CS5.4, CS8.1), `session_host/server/must_match.rs` (CS8.1, CS8.3), `host_files.rs` (CS8.9, CS8.17), `socket_host.rs` (CS8.10, CS8.17, CS8.18), `sandbox.rs` (CS8.8, CS8.18), `client.rs` (CS8.14, CS8.18), `launcher.rs` (CS8.13, CS8.18), `address.rs` (CS8.5, CS8.19), `scripts/checks/check_process_globals.py` (CS3.4, CS8.4), `scripts/run_tests.py` (CS1.7, CS8.4; CS1.8's change is done), and the workflows whose Linux jobs CS8.8 changes, `platform-matrix.yml` among them (CS1.7, CS8.8);
- in gwz-transport: `src/pool/mod.rs` (CS7.2, CS7.3, CS7.5, CS7.10), `machine.rs` (CS7.2, CS7.3, CS7.6, CS7.10), `asynchronous.rs` (CS7.3, CS7.6, CS7.10), `allocation.rs` and `lifecycle.rs` (CS7.2, CS7.4, CS7.5), `clock.rs` (CS7.6), `README.md` (CS7.10);
- in gwz-py: `src/gwz/bridge.py` (CS4.3–CS4.5, CS4.7), `client.py` (CS4.5, CS4.7), `errors.py` (CS4.4, CS8.27), `cli.py` (CS8.28, CS8.29), `native/src/lib.rs` (CS4.2, CS5.2, CS4.8, CS8.26, in that order), `native/src/shims.rs` (CS3.1, CS4.8), `native/src/diff_logs.rs` (CS2.10, CS4.8), `native/src/transport_session.rs` (CS1.1, CS4.8), `src/tests/test_native_bridge.py` (CS4.6, CS5.3), `test_transport_session_api.py` and `test_transport_session_native.py` (CS4.6, CS4.8), `RELEASE.md` (CS4.8, CS5.3), `.github/workflows/package-smoke.yml` and `run_tests.py` (CS5.3, CS8.30);
- in gwz-cli: `src/globalargs/dispatch.rs` (CS3.1, then CS6.1, CS6.3 and CS6.4), `src/lib.rs` (CS6.1, CS6.4, CS6.7), `src/session_driver.rs` (CS6.1, CS6.6, CS8.20, CS8.21), `src/response_meta_json.rs` (CS7.25, CS8.24), `src/diff_exec.rs` and `src/log_exec.rs` (CS6.2, CS8.21), `src/forall.rs` (CS6.3, CS8.21).

The handoffs are CS6.5's with CS7.13 on `agent_job.rs`, and with CS7.15 and CS7.16 on `https_auth.rs`. An order edge there would hold Phase 6 behind Phase 7, or Phase 7 behind the gwz-py phase, which CS6.5 follows. CS7.26 and CS8.3 share `session_entry.rs` under a handoff too: an edge would hold CS8.3, and with it CS8.22 and CS8.29, behind reuse's activation. A file named only as the source of a move, such as gwz-py's `native/src/dispatch/` files that CS2.13–CS2.16 copy from, is read, not changed, until CS4.8 removes it, so it is not shared.

The four endpoint files CS7.1 splits appear here, and in the steps, as "the owners of" each. When CS7.1 merges, its split owners replace those names in the steps' file lists, and the sketch is redrawn for them; the program checkpoint records both.

Revision 1 added gwz-core's `src/transport_host/`, where CS1.1's rename precedes CS3.1, CS3.2, CS3.7 and CS6.5, and gwz-py's `RELEASE.md`, where CS4.8 precedes CS5.3's release pins (§5.3). The allowlist JSONs, which every step that adds or removes an entry touches, are the one standing exception: their entries move with the code (§3.0), and two steps that change one at once hand it over under L1-06, the second rebasing its entries onto the first's. R1 lists files that the transport plan's own steps also change.

## 5. Carried findings mapped to steps

On path 1 of §2.1 the findings below are contract text, and each step implements and tests the corrected text. On paths 2 and 3, the findings marked "RemPlan-2 §2" are step obligations with the closure tests shown. On path 3, Verdict-2's findings are obligations too. Path 1 holds (§2.1). §5.3's rows are step obligations on every path. §5.9 to §5.12 map what revision 3 adds: the reuse design, the server design, the extensions, and each clause of TR1.4b with every item carried to it. The numbers §5.6 to §5.8 are left unused, so that no section of this plan shares a number with the contract's §5.6 to §5.8, which the steps cite.

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
| B11 (Safety P2-9, Consistency P3-31): the gwz-transport check runs in no CI | closed by revision 4 (Verdict-4); its tooling is CS1.8's work, done at gwz-core `4bd92285` | RemPlan-2 §1's closure tests, which pass at revision 1's tuple |
| RemPlan-2 §2, Consistency P3-28: §15.1's dropped reply | CS4.3 | §15.12's closed-loop result test; §15.1 for a cancelled unary call only |
| RemPlan-2 §2, Consistency P3-29: scope of libgit2's own reads | CS3.2 | §15.8 on an `ssh://` or `https://` remote; the exception asserted for `http://` |
| RemPlan-2 §2, Consistency P3-30, P3-32, P3-33 and Safety P3-28: the make-room release | CS2.11 | 64 closed unread logs release one; a log sealed within a second is kept; `operation_expired` after every release way |
| RemPlan-2 §2, Safety P3-29: how core runs `git credential fill` | CS3.4 | the `GIT_ASKPASS` recorder never runs; a sleeping helper is killed on cancel |
| RemPlan-2 §2, Safety P3-30: a late cancel must not drop the kept result | CS4.3 | the result survives a cancel that reports `operation_expired` |
| RemPlan-2 §2, Safety P3-31: the checker's spelling of the helper spawn | CS3.4 | `CredentialHelper::new(url).execute()` is flagged |
| Residual: atomic check-and-record in the registry | CS2.9 | two racing sessions, one start |
| Residual: live records survive a close as detached records | CS2.12 | §15.9 (a new session queues behind a latched W) |
| Residual: the registry does not record fetches | — | disclosed risk R7 |
| Residual: `fork` inherits the host context | CS4.5; CS4.9 | a Client opened after `fork` has its own host context, and the child forgets the inherited one, never dropping it (reuse §7); reuse §15.13 (CS4.9) |
| Residual: non-contiguous call IDs | CS2.2 | `operation_expired` for an unseen ID below the highest (C5) |
| Residual: no test for Windows WTF-8 capture | CS1.5, CS4.5 | an unpaired surrogate reaches a child unchanged on Windows |
| Residual: `DiffLog` parks a blocked thread per read | CS2.10 | §15.10 (1024 parked reads) |

### 5.3 The 1.1.0 amendment and its retired steps

The [1.1.0 plan amendment](../gwz-core/dev-docs/GwzV110PlanAmendment.md) hands this plan three sets of obligations.

**Its first-round findings.** The amendment's first-round [verdict](../gwz-core/dev-docs/GwzV110PlanAmendment-Verdict.md) raised these findings against a draft that put the whole contract into 1.1.0. The operator's decision of 2026-09-26, recorded in the amendment's [remediation plan](../gwz-core/dev-docs/GwzV110PlanAmendment-RemPlan.md), kept 1.1.0 to the minimal unification. The accepted amendment resolved each finding for 1.1.0's own steps. Each also applies to the parts of the contract this plan builds, and the mapping below is for those parts.

| Finding | Steps | Closure test |
| --- | --- | --- |
| A2 (Consistency P2-2): no owner for the gwz-cli dispatch move | CS1.6, CS2.13–CS2.16, CS6.4 | `execute_invocation` and its test callers are gone; gwz-cli's boundary test (CS6.5) passes |
| A3 (Safety P2-1): no route proof after the entries move | CS1.7, CS3.11, CS6.4 | on dabeest, a session fetch reports the transport route; a build without the Windows arm fails that test |
| Consistency P3-4: owners of the core rows and the byte-stream adapter | CS1.3, Phases 2 and 3 for core rows, Phase 4 for gwz-py rows, CS5.1 for the host binary | each §15 row's step lives in the repository its test lives in (§5.5) |
| Safety P3-3: redaction and identity of release evidence | CS5.3 | the secret scan passes; the record names the core version and the binary's revision |
| Safety P3-4: cost and fan-out of a runtime per operation | CS7.27, since the model is now a binding per operation over shared instances (R4); CS4.9 through gwz-py | measurements for 1, 2 and 8 operations: each binding's construction cost, the instance threads and the connection counts |
| Safety P3-6: the session variant's clocks | CS3.8, CS5.3 | default clocks on the session context; S3.3's stall regression on both bridges |
| Consistency P3-5 and Safety P3-2: the implementation plan's own gate | §7 | this plan's dual GO precedes CS1.1 |
| Residual: file sizes in `transport_host` | CS3.5, CS3.6, CS3.7, CS7.1 | new files under 500 lines; splits before growth |
| Residual: the example host binary is test-only | CS5.1 | `cargo package --list` has no example |

**The retired steps' obligations.** The transport release plan retires the amendment's S6.1–S6.3 as steps, and moves their obligations here (its §4, OD1). Revision 1 mapped those that land outside CS3.7 and Phase 4, and revision 3 maps the rest (TR1.4b).

| Obligation | Steps | Closure test |
| --- | --- | --- |
| S6.1's transport entry, which takes the operation's cancellation token | CS3.7, which builds the token-taking request constructor (§16), over a binding | §15.5: a token cancelled while its binding is being obtained fails registration, runs no handler and settles `Cancelled`; S6.1's unit tests below |
| S6.1's library-safety rule: catch the operation's panic first, run finish and shutdown under their own guard, report failure with cleanup unconfirmed, and never finish from `Drop` while unwinding | CS2.4, the worker's half: it catches the handler's panic first and reports `Failed` with cleanup unconfirmed. CS3.7, the entry's half: it catches the handler's panic first, finishes the request and releases the binding under their own `catch_unwind`, and never finishes from `Drop` while unwinding, unlike today's `Command` (`local_command.rs:70-74`) | CS2.4's §15.6 row: a handler panic yields `Failed` (`internal_error`), and the next call succeeds. CS3.7's §15.6 row: a fault-injected panic in finish after a handler panic yields `Failed`, the next call succeeds and the process stays alive; and a guard dropped during an unwind never calls finish |
| S6.1's unit tests: a cancel before the start, a cancel while running, the cleanup report each returns, and a fault-injected panic in finish after a panic in the operation | CS3.7 | each cancel returns its cleanup report; the double panic leaves the process alive and the next operation succeeding |
| S6.2's removal of `TransportSession` and its `CURRENT_SESSION` `debt` entry | CS4.8 | gwz-py's allowlist holds no `CURRENT_SESSION` entry, no `TransportSession` remains outside `dev-docs`, and the candidate build compiles |
| S6.2's release pins: `gwz-py/RELEASE.md` and the publish workflow name every native dependency pin; gwz-core on the release branch is `=1.1.0` from crates.io; `GwzCratesIoPlan.md` D7's git-tag-only core pin is not the release form | CS5.3, whose files and tests revision 3 extends with the row: its release-evidence run is already the one against the registry-pinned gwz-core (Safety P3-3), and four gwz-py artifacts implement today's git-tag pin, `scripts/release.py`, the pin assertions in `.github/workflows/publish.yml`, `RELEASE.md`, and `test_native_module_reports_compiled_core_provenance` (`src/tests/test_native_bridge.py`). The `=1.1.0` registry form replaces the git-tag form in all four | on the release branch, a check finds gwz-core pinned `=1.1.0` from crates.io with no git-tag pin, and `RELEASE.md` and the publish workflow name every native dependency pin; `publish.yml` refuses a git-tag pin and accepts `=1.1.0`; a unit test of `release.py` shows it writes the registry form |
| S6.3's test rows, restated for the session host and reuse | CS4.7 runs the eight rows that need no reuse, with CS4.5 for the close, exit and capture rows; CS4.9 runs the measurement row once Phase 7 has turned sharing on | the rows as the transport plan's §4 restates them, on macOS, Linux and dabeest. The first, two overlapping Python operations on one `Client` completing independently, passing on all three platforms closes the NO-GO of 2026-09-23 (transport plan §4) |

**Its Verdict-2's carried items.** The amendment's [Verdict-2](../gwz-core/dev-docs/GwzV110PlanAmendment-Verdict-2.md), not the contract's (§5.1), carried five items below the finding bar to S1.1's revision. The transport plan retires S1.1 and moves them (its §4).

| Item | Steps | Closure test |
| --- | --- | --- |
| Whether a waiting operation holds a native thread, and a bound on the waiting set | CS2.3 and CS2.4. The bridge's side, a call that waits for a slot on its own event loop, is CS4.3's | with eight latched operations running and 64 queued, the host has started exactly eight operation workers (CS2.4), and the next request is refused with `transport_session_full` before any effect (CS2.3, §15.4); a direct call waiting for a direct worker holds no thread (CS2.4); with the ordinary-call counter at its limit, a further call waits on its own event loop holding no thread, and a control call never takes an ordinary slot (CS4.3) |
| The interpreter-exit bound when `configure_transport_runtime` disables deadlines | CS4.5: the bound is the close bound, the session host's own timer (O5, §5.5), which the call cannot change, plus the cleanup bound of the host context's drop; one finalizer closes every session at once (C9) | with a 2-second close bound and a zero transport timeout, two Clients whose network operations a fixture holds exit 0 within the close bound plus the cleanup bound, in the ordinary build and on the candidate build |
| The environment capture's race with `os.environ` mutation | CS4.5, whose capture is one copy under the GIL, while the extension reads no environment (CS4.2); CS4.8, which removes the legacy path's native read | while one thread sets and deletes a variable, 1,000 opens all succeed, each snapshot holding the variable absent or with a written value; after CS4.8, gwz-py's allowlist holds no environment read |
| Which reading of "paths" S7.2's notes use for the native branch | transport plan Phase 9, S7.2 | — |
| The fixtures S7.3's Linux run needs on the CI host, now including a server | transport plan Phase 9, S7.3; its server needs the runner §2.4 names | — |

### 5.4 The §5.7 debt entries

gwz-core's allowlist has 30 entries at the planning tuple, 18 of them `debt`. gwz-py's has 8, all `debt`, `CURRENT_SESSION` among them. gwz-transport's has one `permanent` entry. The counts are the same at revision 1's tuple, which is revision 3's. CS1.4 adds one `permanent` entry, `thread_local CROSSING` in `gate.rs`, which carries no session-relevant state, so gwz-core's then holds 31: 18 `debt` and 13 `permanent` ([CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md)).

| Entry | Step |
| --- | --- |
| `agent_job.rs`: `HUB`, `INIT`, `COUNT`, `CLEANUPS` | CS3.5 gives the session host's host context its own; the statics stay `debt` for the legacy path, and CS6.5 removes them after CS4.8 |
| `https_auth.rs`: `ORPHANS`, `ORPHAN_REAPING` | CS3.6 |
| `https_auth.rs`: `SLOTS` | CS3.6 gives the session host's host context its own, and CS7.16 makes a request await one within its helper budget; the static stays `debt` for the legacy path, and CS6.5 removes it after CS4.8 |
| `transport_binding.rs` `env::var_os` | CS7.24 retires the lazy endpoint, and this entry with it (reuse §11) |
| `identity.rs` `env::home_dir`; `repo-inspect` `env::var_os` | CS3.2 |
| `local_command.rs` `env::vars_os`; `transport_host/mod.rs` `env::var_os` | CS3.2 moves them into the legacy adapter; CS6.5 removes them |
| `Command::new("git")` in `refs.rs`, `repository.rs`, `transport.rs`, `commit_log/mod.rs` | CS3.3 keeps them `debt`; CS6.5 flips them to `permanent` with tests, once the legacy context is gone |
| `transport_support.rs` `Cred::credential_helper` | CS3.4 removes it; its new `git` spawn stays `debt` until CS6.5 flips it |
| `V1_PRESERVATION_IMAGE_CAPTURES` | CS2.1 |
| gwz-py: `diff_logs.rs` and `log_outputs.rs` `REGISTRY`; `operations.rs` `STORE`, `SCOPED_STORE` and the `env::var` test hook; `shims.rs` `SCOPED_BACKEND` and `SCOPED_OPERATION_ID` | CS4.8 |
| gwz-py: `transport_session.rs` `CURRENT_SESSION` | CS4.8, with `TransportSession` (§5.3) |
| gwz-py: the legacy environment read at `shims.rs`'s backend scope | CS3.1 adds it; CS4.8 removes it |
| the legacy adapter's own entries, the crate default's environment read among them | CS3.1 adds them; CS6.5 removes them |
| new process-wide state and spawns outside the session path, from Phase 8: the launcher's spawns, the serving process's signal handling, and macOS's automount hold | `permanent` from the step that adds each (CS8.7, CS8.10, CS8.13, CS8.18, CS8.26), under §3.0's rule for driver-level state |

§5.7's row on gwz-py's working-directory reads is already closed at the planning tuple: gwz-py takes the caller's directory from each request, and its allowlist has no `current_dir` entry. `TIMEOUT_STATE`, libgit2's timeout options, the `gh` spawn, the ID counters and CS1.4's `thread_local CROSSING` (`gate.rs`) stay `permanent`. Phase 7 adds no static: the endpoint registry, its binding-ID counter and the instances live in the host context. The server's release gate (CS8.4) passes once CS6.5 and CS7.24 have cleared gwz-core's `env` and `process` debt, and CS4.8 gwz-py's. openssl-probe's write of `SSL_CERT_FILE` and `SSL_CERT_DIR` is the contract's disclosed §5.7 row, which no allowlist can hold (server §5).

### 5.5 Contract §15 coverage

| §15 item | Steps |
| --- | --- |
| 1. One reply per call | CS2.2; CS4.3; CS5.2, whose closed-session code is CS1.10's `server_session_closed` |
| 2. Error codes | CS1.1, CS1.10 (the server design's seven codes); CS2.7, CS2.12; CS3.10 (the field at tag 8); CS4.4, CS4.5; CS5.3 |
| 3. Structured errors | CS2.4, CS2.15; CS4.4; CS5.3 |
| 4. Concurrency and admission | CS1.6, CS2.3, CS2.8, CS2.9, CS2.13, CS2.14; CS3.9, CS3.10; CS4.7 |
| 5. Cancellation | CS2.2, CS2.7; CS3.7, CS3.10, CS7.11 (a wait for another operation's construction); CS4.7; CS5.3 |
| 6. Panics | CS2.4; CS3.7 |
| 7. Retention | CS2.5, CS2.6, CS2.11; CS4.5 |
| 8. Session context | CS1.5; CS3.2, CS3.3, CS3.4, CS3.8; CS4.8; CS6.5; CS7.24, for the last `debt` entry; CS1.8, done at gwz-core `4bd92285`, for the `run_tests.py` and `reconciled_commit` rows |
| 9. Closure | CS1.3, CS2.8, CS2.12; CS3.5, CS3.9; CS4.5, CS4.7; CS5.1; CS7.26, for reports over shared instances |
| 10. Channel | CS1.2, CS2.2, CS2.10; CS4.2, CS4.5 |
| 11. Pump | CS4.3, CS4.5 |
| 12. Python mapping | CS4.3, CS4.5, CS4.7 |
| 13. Wire proof | CS5.3, with CS5.1, CS5.2 and CS5.4 |
| 14. gwz-cli | CS6.4 |
| 15. Ordinary builds | CS3.10, in the configuration D7 settles |
| 16. Connection reuse (reuse §15, items 1 to 18) | Phase 7, item by item in §5.9; CS4.9 through gwz-py; CS8.32 through a server |

The server design's §12 rows are not contract §15 items, except its amendments of §15.1 and §15.2 above; §5.10 maps them.

### 5.9 The reuse design

The accepted [reuse design](GwzConnectionReuseDesign.md) names its steps for TR1.4b in its §14, each aimed at under 500 lines, and its verification in §15. Revision 3 places them as follows.

| Reuse §14 step | Plan step |
| --- | --- |
| T1 per-owner accounting | CS7.2 |
| T2 ceilings by the max rule | CS7.3; its removal of the install path is CS7.10's, after gwz-core stops calling it |
| T3 cross-binding leases | CS7.4 |
| T4 host hooks | CS7.5 |
| T5 reserve before connect | CS7.6 |
| C1 endpoint configuration | CS3.2 |
| C2 (a), (b), (c) several bindings per instance | CS7.7, CS7.8, CS7.9 |
| C3 registry | CS3.7 (its first form), CS7.11 (selection, construction, the pump), CS7.12 (bounds, faults, shutdown), CS7.13 (clock, maximum age); CS7.26 turns sharing on |
| C4 transport entry | CS3.7 (the entry, its panic path, and the seam); CS7.14 (`TransportRuntime`'s constructors and the embedding guide) |
| C5 capacity plumbing | CS7.10 |
| C6 the per-request `gh` environment | CS7.15 |
| C7 `gh` slots awaited | CS7.16, CS3.6's extension |
| C8 retained keys | CS7.17 |
| C9 revalidation | CS7.18 (the proof), CS7.19 (leases, proofs, host trust, retirement) |
| C10 stale replacement | CS7.20 |
| C11 authority | CS7.21 |
| C12 instance fault scope | CS7.22 |
| C13 the retry machine per binding | CS7.23 |
| C14 the lazy endpoint retires | CS7.24 |
| gwz-cli: §10's `--verbose` line | CS7.25 |
| gwz-py: CS4.5's at-fork handler forgets | CS4.5 |
| Prerequisites CS3.5 and CS3.6 | Phase 3, whose exit every Phase 7 step follows |
| The conditional-compilation rule | Phase 7's preamble; CS3.7 |

| Reuse §15 item | Steps |
| --- | --- |
| 1. Configuration ownership | CS7.15; CS3.2's configuration half; CS8.32 across a server's clients |
| 2. Agent identity and per-use authorization (a, b) | CS7.19 |
| 3. Sharing | CS7.26; CS4.9 through gwz-py; CS8.32 through a server |
| 4. Partition | CS3.2 (configurations), CS7.11 (instances; different explicit identities share no connection) |
| 5. Withdrawn authority | CS7.19 |
| 6. Isolation | CS7.4 (the pool's rows), CS7.9 (binding faults, Q6, and cancelling A while B holds a lease), CS7.12 (the cleanup-owner budget), CS7.19 (a pending proof), CS7.22 (instance faults) |
| 7. Capacity | CS7.2, CS7.3 (pool halves), CS7.10 |
| 8. Retry | CS7.23 |
| 9. Stale idle replacement | CS7.20 |
| 10. Cross-scheme eviction | CS7.6 (pool half), CS7.21 |
| 11. Bounds and lifetime | CS7.12, CS7.13; CS8.10 and CS8.32 (server shutdown); CS6.7 (the CLI's notice) |
| 12. `gh` slots | CS7.16 |
| 13. Fork | CS4.5 (forgetting), CS4.9 (the row) |
| 14. Observations | CS7.25 |
| 15. Lazy endpoint | CS7.24 |
| 16. Maximum age | CS7.13 |
| 17. Key text | CS7.17 |
| 18. A Cold key and reuse | CS7.23 |
| The Phase 6 exit rows | the Phase 7 exit, mapped as reuse §15 maps them |
| The cells | the Phase 7 exit files their evidence for S5.6 |

Reuse §13's contract amendments reach steps as follows: §1's two exclusions, Phase 7 with its gwz-transport steps (§1.3); §1's binding per operation, §2's registry and binding, §5.2 and O5, CS3.7; §5.5 and §15.5, CS3.7, and CS7.11 for a wait for another operation's construction; §4.2's `TransportCapacityConflict`, CS7.10, and its two host-context causes of `transport_session_full`, CS1.10 and CS7.12; §5.6's registry member, CS3.7, its bounded `shutdown`, CS1.9 and CS7.12, and the `gh` environment per request, CS7.15; §5.7's environment rows, CS3.2 and CS7.24, and its `SLOTS` row, CS7.16; §8's close report, CS7.26; §14's and §16's physical reuse and cost, CS4.9 and CS7.27; §7's sequence changes only a word, which needs no step; §4.3 and §13 stand. Its amendments of other documents land with the steps that change the behaviour: the embedding guide with CS7.14, gwz-transport's README with CS7.10, the A3 slice's reading with CS7.22, and N2's registry with CS7.17.

Its §13 lists the sentences of this plan that rest on the per-operation runtime, by revision 0's line numbers, and its Verdict-1 carries them here. Each is now:
- :15 and :16, 1.1.0 S6.2's per-operation runtime and S6.3's "each on its own runtime": gone from §1.1 in revision 1; §5.3's S6.2 and S6.3 rows;
- :280, the Phase 3 heading: "each on its own binding";
- :282, the Phase 3 preamble: the binding model and the registry's first form;
- :293, CS3.2's `transport_binding.rs` row: CS3.2 leaves it, and CS7.24 retires the lazy endpoint;
- :324 and :325, CS3.7's entry and its test: a binding from the registry, "while its binding is being obtained";
- :348, CS3.11's measurement: CS7.27 and CS4.9;
- :607, §5.3's Safety P3-4 row: CS7.27;
- :658, R4: rewritten;
- CS4.5's at-fork "drops": "forgets".

### 5.10 The server design

The accepted [server design](GwzCoreServerDesign.md)'s "On GO" list says TR1.4b places its steps in the transport plan's Phase 7. Revision 3 places them in Phase 8, with two foundations earlier: the schema (CS1.10) and the host side of the handshake (CS5.4).

| Server §2 piece | Steps |
| --- | --- |
| The session host, channels, the handshake and `serve_session`; the client ends of the socket and byte-stream channels | CS5.4; CS8.14, CS8.15 |
| The address parser and its walk | CS8.5, CS8.6, CS8.7; CS8.19 (Windows checks) |
| Listener verification and the sandbox rule | CS8.8, CS8.14; CS8.18 (Windows) |
| The must-match set and the `auto` key | CS8.1, CS8.2, CS8.3, CS8.4 |
| The socket host | CS8.9, CS8.10, CS8.11; CS8.17, CS8.18 (Windows) |
| The stdio host | CS8.12 |
| The launcher | CS8.13; CS8.18 (Windows) |
| The SSH remote form's parser, `ssh` resolution and argument vector | CS8.5, CS8.16 |
| gwz-cli: `gwz server`, the options, starting copies and `ssh` through core's launcher | CS8.20–CS8.25 |
| gwz-py: `gwz-py server`, the options, `SocketCoreBridge`, the private stdio client, the extension bindings, copies and `ssh` spawned by the extension | CS8.26–CS8.30 |

Server §8's contract amendments reach steps as follows: §1, the SSH remote form, CS8.16 and §1.3; §3, tags 4 to 8, `call_id` 0 and the client interface, CS1.10, CS5.4 and CS8.14; §4.1, Unicode paths, already in the tree (gwz-cli `49d784d`, gwz-py `3778394`, gwz-core `5ca45a66`) and asserted by CS8.20 and CS8.27; §4.2 and §13, the seven codes and their uses, CS1.10, CS5.4 and CS8.10; §5.1, CS2.3's addendum; §5.6, the snapshot's crossing, CS5.4 and CS8.14, its zeroization and `transport_off`, CS1.9, explicit working directories, CS3.4's addendum, and the must-match set, CS8.1; §5.7, CS8.4; §5.8, CS8.1 and CS8.4; §9, CS8.26; §10, CS4.3, CS5.2 and CS8.27; §11, CS8.20–CS8.25; §12, CS5.1, CS5.2, CS5.4, CS8.25 and CS8.30; §15, CS1.10 and CS5.2; §16, the transport plan's Phase 4.

| Server §12 rows | Steps |
| --- | --- |
| gwz-core, `serve_session` | CS5.4; CS8.1; CS8.3; CS2.3's addendum |
| The handshake's bound | CS8.14 (client); CS5.4 (the host's 30 seconds) |
| The address parser | CS8.5; CS8.19 |
| The walk on Linux; on macOS | CS8.6; CS8.7 |
| Windows, on dabeest | CS8.18, CS8.19 |
| Process IDs | CS8.8; CS8.18 |
| Listener verification | CS8.8, CS8.14, CS8.15; CS8.18 |
| The socket host | CS8.9, CS8.10, CS8.11, CS8.13; CS8.17, CS8.18 |
| The stdio host | CS8.12 |
| Native routes through a server | CS8.3, CS8.4 |
| OpenSSL's configuration | CS8.2, CS8.10 |
| Windows' logon session | CS8.18 |
| Shutdown disposes every instance | CS8.10; CS8.32, with instances |
| The checker's new mode | CS8.4 |
| gwz-cli: two runs, the stdio run, the client side, the soak | CS8.25; CS8.20 (paths, refused forms, `hook`) |
| gwz-cli: lifecycle; command surface | CS8.22, CS8.23; CS8.24 |
| gwz-cli: the off switch; reuse through a server | CS8.32 |
| gwz-cli and gwz-py: the stdio rows | CS8.12, CS8.15, CS8.21, CS8.23, CS8.28 |
| gwz-py | CS5.2 (`StreamCoreBridge`), CS8.27, CS8.28, CS8.29, CS8.30 |
| Windows, every repository | CS8.17–CS8.19, on Windows CI and dabeest |
| The workspace run | the Phase 8 exit |
| The release gate | CS8.4; the Phase 8 exit |
| The cells | the Phase 8 exit files their evidence for S5.6 |
| §16 question 8 | CS8.5 and CS8.16 (core halves), CS8.21, CS8.31 |

### 5.11 The extensions and addenda

| Extended step | Extension | Its step | Review |
| --- | --- | --- | --- |
| CS1.1 | the schema comment and ErrorCatalog entry for `transport_session_full` name its two host-context causes (reuse §13) | CS1.10 | dual, CS1.1's re-freeze, with the server's schema |
| CS1.4 | the host context's bounded `shutdown` (reuse §7), and its endpoint registry (reuse §2, §13) | CS1.9 (`shutdown`); CS3.7 (the registry member) | dual, each |
| CS2.12 | the close report sums binding reports, and instance disposal is the host context owner's to report (reuse §8, §13) | CS7.26 | dual, the Phase 7 exit |
| CS3.6 | slots awaited within the helper budget, cancellably; helper children belong to the instance's `AuthOwner` (reuse §6, §13) | CS7.16 | dual |
| CS6.1 | the CLI calls `shutdown` at command end (reuse §7) | CS6.6 | dual, with CS6.7 |
| CS6.4 | the notice counts instance disposal (reuse §7, §15.11) | CS6.7 | dual, with CS6.6 |

The server design's amendments reach four steps revision 2 accepted and did not mark: CS1.4 and CS1.5 (`transport_off` and zeroization, §5.6), through CS1.9; CS2.3 (§5.1), by an addendum; and CS3.4 (explicit working directories, §5.6), by an addendum (§2.2).

### 5.12 TR1.4b's closure map

Each clause of the transport plan's TR1.4b, and each item carried to TR1.4b, with the place that answers it.

| Source | Clause or item | Answered in |
| --- | --- | --- |
| Transport plan TR1.4b | Revises CS3.7, the session plan's Phase 4, and every other step that depends on the runtime model, reuse or the server | CS3.2, CS3.7, CS3.8, CS3.10, CS3.11 and the Phase 3 text; CS4.1–CS4.8, the Phase 4 text and exit; CS5.1–CS5.3 and the Phase 5 exit; R4; the extensions and addenda (§5.11); every "TR1.4b revises this" marker is gone |
| TR1.4b | Replaces "each on its own runtime" with TR1.2's model: one binding per operation, over endpoint instances the host context shares | the Phase 3 heading and preamble; CS3.7 (a binding from the host context's registry); CS7.26 (sharing on); §1.1, §1.2, §1.3; R4; §5.9's list of sentences |
| TR1.4b | Rewrites the 1.1.0 dependencies of CS3.7 and CS4.1 | CS3.7 no longer depends on 1.1.0 S6.1, a retired step, and builds its constructor; CS4.1 no longer waits for a 1.1.0 tag, and retakes its list when it starts |
| TR1.4b | The panic model: "as today's `Command` in `local_command.rs` does" does not stand | CS3.7's panic path, which names `Command`'s `Drop` as the pattern it must not follow; §5.3's library-safety row |
| TR1.4b | Maps §4's remaining moved obligations | §5.3: S6.1's entry, the entry's half of its safety rule and its unit tests, CS3.7; S6.2's removal, CS4.8; S6.2's pins, CS5.3; S6.3's rows, CS4.7 and CS4.9 |
| TR1.4b | Maps Verdict-2's second and third carried items | CS4.5 (the exit bound; the capture), with CS4.8 (the legacy read); §5.3 |
| TR1.4b | Adds the reuse steps (its Phase 6) and the server steps (its Phase 7), each under 500 lines, foundational-first and parallel-friendly | Phase 7 (CS7.1–CS7.27); Phase 8 (CS8.1–CS8.32), with CS1.10 and CS5.4; §4's lanes; §5.9, §5.10 |
| TR1.4b | Review: dual; G1 for the rest | §2.2; §7 |
| Transport plan Phase 5 | TR1.4b gates CS3.7, Phase 4 and the other runtime, reuse and server steps | §2.2; §4 |
| Transport plan Phases 6 and 7 | Reuse after the network phase; `gwz server` after the gwz-cli phase, `gwz-py server` and `SocketCoreBridge` after the gwz-py phase, Windows after its Phase 4, reuse through a server after its Phase 6; the Phase 7 exit rows | the preambles of Phases 7 and 8; the Phase 8 exit |
| Transport plan TR1.7 | The gwz-py phase's Surface review covers R8's and CS4.5's changes and reuse | CS4.5's documented changes and the Phase 4 exit's Surface review; CS4.9's Surface review of reuse, after the Phase 7 exit |
| Reuse design §13 and its Verdict-1 | The session-plan sentences on the runtime model; CS4.5's exit path | §5.9's list; CS4.5 |
| Reuse design Consistency-1 residuals | Nothing says the process-wide host context drops at exit; A3 is read, not amended | CS4.5's exit path; CS7.22 |
| Reuse design §14 and §15 | Its T- and C-steps, and its verification | Phase 7; §5.9 |
| Server design "On GO" | TR1.4b places its steps in the transport plan's Phase 7 | Phase 8; §5.10 |
| Server design Verdict-1, recorded for TR1.4b | Linux CI rows that run a server need a runner outside a seccomp-profiled container | §2.4; the preamble of Phase 8; CS8.8, CS8.25, CS8.30 |
| Server design Verdict-1 | The session plan names gwz-py's console script `gwz`, where it is `gwz-py` | §3.0; the Phase 4 exit |
| Server design Verdict-1 | The plan's Phase 7 gate names only gwz-core's checker, where the design's gate also covers gwz-py's allowlist | the preamble and exit of Phase 8 |
| Server design Consistency-1, R4 | Linux server rows in `container:` jobs must move | §2.4: none exist at revision 3's tuple |
| Server design remediation plan, C-P3-9 | gwz-py's copies and `ssh` have an owner | CS8.13, CS8.26, CS8.29 |
| Session plan Verdict-1 | CS1.1's candidate pins; "stale" | CS1.1; §2.4 |
| Session plan Verdict-1 | The transitional double budget between CS4.7 and CS4.8 | CS4.7; R12 |
| Session plan Consistency-1 residual | TR1.4b marks a step it revises if that step changes the ordinary build (CS3.10) | CS3.10, not marked, with the reason |
| Session plan Safety round-1 residual | Whether the release-evidence run can run the hook rows without CS2.1's switch | CS5.3 |
| Session plan Consistency round-1 residual | CS4.8's status edits to `GwzPyTransportDesign.md` belong to TR1.7 | CS4.8 |
| Contract Verdict-5, recorded | The owner-schema pin; the harness check in `run_tests.py` or CI | CS1.1; §2.4 |
| Contract Verdict-5, recorded | The DRAFT flip names revision 5 | §2.1 (G0 discharged) |
| Contract Verdict-5, recorded | C1 stays open | §6.3, C1 |
| Contract §15.2 at revision 5 | `cancellation` at tag 8, absence-tolerant | CS1.1; CS3.10 |
| [TR1.4b's Verdict-2](GwzCoreSessionPlan-Verdict-2.md) | Its post-GO corrections, the residual notes taken with them, and the plan text carried from the CS1.4 and CS1.5 and the CS1.7 verdicts | The changelog's post-GO entry lists each by finding ID |

## 6. Risks and open decisions

### 6.1 Risks

- **R1. A moving baseline.**
  - The reuse design ([its Verdict-1](GwzConnectionReuseDesign-Verdict-1.md)) and the server design ([its Verdict-1](GwzCoreServerDesign-Verdict-1.md)) are accepted, TR1.2 and TR1.3 are closed, and the contract is at revision 5 as both amend it. Revision 3 places their steps. Two designs this plan runs beside have no GO yet: the off switch (TR1.5) and non-gh HTTPS credentials (TR1.6). CS3.10 carries the switch's attribute, and CS8.3 checks whatever routes those designs define. Each one's acceptance triggers §7's re-check of CS3.10, CS8.3 and C1.
  - An accepted design can still reach steps through §7's contract-revision trigger. Revision 3 did that for both designs (§5.9, §5.10). The server design's tags 4 to 8 reach CS1.10 and CS5.4, not CS1.2's in-process registry or CS1.3's framing; its zeroization reaches CS1.9; its refusal of `RequestMeta` without `InvocationContext` reaches CS2.3; and its explicit working directories reach CS3.4, while CS3.3's spawns already set theirs (C10).
  - The transport plan's Phases 2 and 4 change code that steps here also change. TR2.7 changes the CA-file path through `tls_config` in `local_command.rs`, which CS3.2 also changes. 1.1.0 S4.5 turns the candidate sites in `transport_binding.rs` (CS7.24), `identity.rs` (CS3.2), `transport_support.rs` (CS3.4) and `backend.rs` (CS2.8) into unix and windows arms. TR2.1 builds the retry machine CS7.23 moves into the binding; TR2.8 changes `agent_auth.rs`, which CS7.17 and CS7.18 also change; 1.1.0 S4.3 builds the Windows agent source CS8.19 extends. A step that must cross one of these files hands off under L1-06.
- **R2. Size and review load.** 116 steps. Twenty-eight dual reviews: the four freezes of Phase 1, CS3.4, and the exits of Phases 3, 4 and 6; and revision 3's CS1.9, CS1.10, CS3.2, CS3.7, CS5.4, CS6.6 with CS6.7, the pool checkpoint CS7.2–CS7.6, CS7.9, CS7.16, CS7.18, CS7.19, CS8.1, CS8.5, CS8.8, the socket-host checkpoint CS8.9 with CS8.10, CS8.14, CS8.16, the Windows checkpoint CS8.17–CS8.19, and the exits of Phases 7 and 8. Three Surface reviews, at the exits of Phases 4 and 6 and at CS4.9, and seventy-four single-axis reviews, the exits of Phases 2 and 5 and CS4.9 among them; CS1.8 is done and needs none. The pipeline rule of GwzProcessOptimization §4.4 applies: the implementer starts the next step behind a frozen interface while reviewers hold the last one.
- **R3. Three dispatch tables at once.** From Phase 2 until CS4.8 and CS6.4, core's dispatch, gwz-py's native dispatch and gwz-cli's `execute_invocation` all exist. CS2.13–CS2.16's parity tests guard against drift. A handler change in that window updates every copy or waits.
- **R4. A binding per operation, over shared instances.** Each network operation builds its own binding, a link thread and a driver session, and each endpoint instance runs three threads, the SSH worker, the HTTPS thread and the instance pump, with at most 16 instances per host context (reuse §2, §7). Before CS7.26, each binding's private instance costs what a per-operation runtime costs today, and eight operations can open up to 8 × 32 connections to one host. After it, overlapping operations each open up to their own limits, bounded by the instance ceilings, 256 and 256 until S5.4 chooses them (reuse §5; §16 as amended). CS7.27 measures a binding's cost and the connection counts for 1, 2 and 8 overlapping operations, and CS4.9 does so through gwz-py. If the measurements miss the transport plan's TR8.1 targets, S5.4 retunes within the ceilings first, and OD8 follows (transport plan Phase 8).
- **R5. Direct-worker saturation.** `diff` and `log` produce to completion before they reply, so eight large ones occupy every direct worker and `status` waits. §5.2 promises only that `status` never waits behind operations.
- **R6. A detached worker that never returns** keeps its thread, any cross-process lock it holds and its registry entry until the process exits, and process exit can tear a local write it has in progress (§16).
- **R7. Fetches are not recorded in the registry.** A fetch member step in one session can run beside a W in another session sharing the host context, although the pair is excluded within one session. This matches the cross-process status quo.
- **R8. User-visible changes.** CS3.4 changes how HTTP credentials are found for both existing drivers, and that path now needs `git` on `PATH`. From CS2.10, a bounded wait on the bridge's `diff_log_read` (`timeout_ms` above 0) waits instead of probing; `log_output_read` takes no wait. Phase 4 changes Python's cancel codes, adds `transport_session_full` and `operation_expired`, applies the open-merge pre-gate to Python (D4), and makes interpreter exit wait up to the close bound, 60 seconds by default, plus the cleanup bound (CS4.5). From CS7.26 a Python process reuses connections across its operations, and an agent key that asks for confirmation asks again once in each operation that reuses a connection it authenticated (reuse §4). The release notes and the Surface reviews carry these. Phase 7's `--verbose` summary line and every server surface reach users only at activation, under S7.5's Surface review.
- **R9. gwz-core's published crate API changes:** the new `session_host` module, the §16 handler changes and the deprecations. No compatibility rule for the crate API is written down.
- **R10. Windows.** Transport proofs depend on dabeest. The contract names only Linux and macOS for the exit test and no platform for the pseudo-terminal test (C6). Environment names on Windows are case-insensitive and values are WTF-8 (CS1.5, CS4.5). Phase 8's Windows primitives are new code on the least-tested platform, several of whose rows only dabeest can run (transport plan §8).
- **R11. Large files.** At revision 3's tuple, `ssh_worker.rs` is 1,279 lines, `placement_endpoint.rs` 1,064, `https_worker.rs` 1,056, `transport_host/session.rs` 1,009, `https_auth.rs` 934 and `agent_job.rs` 769; gwz-py's `client.py` is 1,380 and its candidate `transport_session.rs` 1,507, which CS4.8 removes. CS7.1 splits the first four before Phase 7 grows them; CS3.6 splits `https_auth.rs` first, and CS3.5 splits `agent_job.rs` if its change would pass 1,000 lines. A step that grows `client.py` reviews it for cohesion first (L1-23).
- **R12. Two budgets in one gwz-py process** ([Verdict-1](GwzCoreSessionPlan-Verdict-1.md)). Between CS4.7 and CS4.8, a gwz-py process that uses the session host and the legacy module functions at once holds two SSH supervisors and two sets of caps, at most twice today's. This is a development window only: no 1.0.x release is cut from gwz-py's main after CS4.7 (§2.3), and the transport release follows CS6.5.

### 6.2 Decisions for the operator

- **D1. The contract revision path** (§2.1). Decided: path 1. Revision 4 applies RemPlan-2 §1 and §2 and is accepted (Verdict-4), under the transport release plan's OD3, which the operator adopted on 2026-09-27. The recommendation, kept for the record: path 1, revision 4 with RemPlan-2 §1 and §2.
- **D2. Which release carries which phase.** Superseded by TR1.4a in revision 1: every phase ships in the transport release, whose activation and tags follow this plan's phases (§1.1, §2.3). The superseded recommendation, kept for the record: Phases 1–3 may ship in any release after 1.1.0, since only CS3.4 and the `cancellation` field are visible to users; Phase 4 ships only with Phase 5, whose two-bridge run is gwz-py's release evidence; Phase 6 ships with Phase 4 or later.
- **D3. CLI requests with no protocol method** (C2). Recommended: `forall` and `claude-code setup` stay CLI-local named exceptions, and `forall` takes the cross-process workspace lock itself, as CS6.3 describes. `init --update` gets an append-only protocol method through a contract amendment, because GWZDesign routes structured workspace operations through the message path. The alternative is a third named exception.
- **D4. The open-merge pre-gate for Python.** Moving it into core's dispatch applies it to Python requests, which skip it today; core's handlers already guard mutations, so the change is an earlier refusal for some calls. Recommended: yes, for cross-driver parity (L2-03), with the changed outcomes in the Phase 4 Surface review.
- **D5. One transport-scope predicate** (CS1.6). Recommended: the union of gwz-cli's and gwz-py's, narrowed only by a test that shows a method does no network I/O.
- **D6. gwz-py's legacy module-level entry points** (§9 leaves this to implementation). Recommended: remove them, so CS4.8 clears every gwz-py `debt` entry. Keeping them off the session path leaves their entries in the allowlist, and O9's test would then need a session-path marker there.
- **D7. The ordinary build after activation** (C1; 1.1.0 S7.1 in the transport plan's Phase 9). Recommended: keep a CI build configuration without the transport, so §5.8 and §15.15 stay testable. The alternative is a contract amendment that confines §5.8 to libgit2's native remotes. The off switch (TR1.5, TR2.5) is a second way to exercise §5.8's rows, on a transport build.
- **D8. A wire field for the default clocks.** The 1.1.0 amendment round's Safety P3-6 asks for the defaults "in a typed field"; §13 has none. Recommended: CS3.8's assertion on the session context, and an amendment only if the operator wants the field.
- **D9. Merge timing.** Superseded by TR1.4a in revision 1: a step's code merges to a product repository's main once its review passes, under the transport plan's §6 rules (§2.3). The superseded recommendation, kept for the record: product code merges after the 1.1.0 tags exist.
- **D10. The V0 `OperationRuntime` API.** Recommended: deprecate `submit`, `subscribe` and `wait` in CS2.5 and remove them at a later major version. Removing them now breaks the published crate's API.
- **The `cancellation` field's number** (C8, §6.3). Decided by the operator on 2026-09-27: the contract's revision 5 gives it field 8, and the candidate projection's tags 3–7 stay. Revision 5 is accepted (Verdict-5), so CS1.1 waits only on TR3.1.

### 6.3 Contract ambiguities found while planning

Each has a working default in the step named, except C8, which the operator has decided, and each is a candidate for the contract's next revision.
- **C1 (§5.8, §13, §15.15, §16).** These sections predate 1.1.0's activation; §16 still says the transport "stays candidate-only". After S7.1 the normal build contains the transport on every platform GWZ builds, so "ordinary builds" may describe no product build (D7). In a transport build `transport_capabilities.cancellation` is true, yet libgit2's native remotes (`git://`, `http://`, `file://`) cannot be cancelled promptly. §5.8 states the timeout exception for those remotes but not the cancellation one. Three adopted routes also send work down the native path inside a transport build: the off switch (TR1.5), non-gh HTTPS (TR1.6), and SSH remotes whose agent holds a key the transport cannot sign with (TR2.8). For each, the contract's next revision must state what `cancellation` means. TR1.5's and TR1.6's Surface reviews carry it, and for TR2.8's route, which has no design review of its own, S7.5's Surface review of the `--verbose` transport rows does. Revision 5 does not settle it ([Verdict-5](GwzCoreSessionDesign-Verdict-5.md)), and neither design that amends it does. CS3.10's working default sets the field true in every transport build, as §13 says.
- **C2 (§5.2, §11).** The CLI "sends its request", but three CLI requests have no protocol method: `init --update`, `forall`'s command execution with its workspace guard, and `claude-code setup`. O6 forbids sharing the `forall` guard across the channel (D3).
- **C3 (§4.2, §8).** The cleanup report for a cancel whose target never touched the network is unstated. §8's close rule uses `(0, false)` when no peer cleanup occurred, while Verdict-2's residual assumed `(0, true)` for a direct call. CS2.7 uses `(0, false)`, which is also today's `CleanupReport::default()`.
- **C4 (§4.2, §5.1).** The reply to a call whose `method` the service does not declare is unstated. The rule that a method missing from the class table is W governs class, not routing. CS1.6 uses `invalid_request` before any effect.
- **C5 (§4.2).** A call ID below the highest received that the session never saw, which non-contiguous IDs allow, is neither "above the highest" nor a spent call. CS2.2 answers `operation_expired`.
- **C6 (§15.11, §15.14).** The exit test names Linux and macOS although the extension ships on Windows; the pseudo-terminal test names no platform. CS4.5 adds Windows to the exit test; CS6.4 runs the pseudo-terminal test on macOS and Linux.
- **C7 (§5.6).** The snapshot "fixes environment values and the paths they name", but the contract does not say that lookups follow the platform's name rules. Windows names are case-insensitive. CS1.5 follows the platform.
- **C8 (§13).** Revision 4's §13 appends `cancellation` to `TransportCapabilitiesResponse` as field 3. The candidate protocol's placement projection (`tests/transport_consumer/protocol/candidate.taut.py`) already gives field 3 to `message_versions`, with fields 4 to 7 after it, and its composition refuses a colliding tag. One of the two had to move before CS1.1 could keep both protocol files in step.
  - **Decided by the operator on 2026-09-27:** the contract's revision 5 gives `cancellation` field 8, and the projection's tags 3–7 stay.
  - Revision 5 was accepted at SHA-256 `6d12f03e…` ([Verdict-5](GwzCoreSessionDesign-Verdict-5.md)), with `cancellation` optional and allowed to be absent. CS1.1 implements it (§6.2).
- **C9 (§10).** The finalizer that "closes the channel and joins the pump before interpreter finalization, waiting at most the close bound" is stated per bridge. With several Clients open, finalizers that run one after another could make exit wait one close bound per Client, which no text bounds. CS4.5 registers one process-wide finalizer that closes every open session at once, so exit waits at most one close bound, plus the cleanup bound of the host context's drop.
- **C10 (§5.6, as the server design amends it; §5.8).** Every session-path child gets "an explicit working directory, the repository it acts on, or `/` for the `gh` helper". Core's own `git credential fill` (§5.8) acts on a URL, not a repository, and run inside a repository it would also read that repository's local configuration, which today's lookup (`git2::Config::open_default()` in `transport_support.rs`) never reads. CS3.4's addendum uses `/`, as the `gh` helper does.
- **C11 (§5.8; §5.6, as the server design amends it).** §5.8 names only the prompt hooks, `GIT_ASKPASS` and `SSH_ASKPASS`, as dropped from the credential spawn's environment. `git credential fill` also honours `GIT_DIR`, `GIT_COMMON_DIR` and `GIT_WORK_TREE` from any directory, so a snapshot that carries them, as a git hook's environment does, would make it read a repository's local configuration, which today's lookup never reads. CS3.4's working rule drops them too, and the contract's next revision states it ([TR1.4b's Verdict-2](GwzCoreSessionPlan-Verdict-2.md), S-P3-6).

## 7. Review of this plan

- **Object.** This file, identified by its SHA-256 while uncommitted, as the 1.1.0 plan and its amendment were, and by its commit once committed.
- **Tier.** Dual peer-blind Consistency and Safety, with GO on both axes before any step it gates starts (G1, in two parts, §2.2). The 1.1.0 amendment round's Consistency P3-5 and Safety P3-2 ask for exactly this gate. No Surface review: the plan fixes no user-facing surface; its Phase 4 and Phase 6 exits, and CS4.9, carry Surface.
- **Consistency attacks** agreement with the contract at the accepted revision, as the reuse and server designs amend it: every section in §1.2 has an owning step, and every §15 row has an owning step in the repository where its test lives. Also agreement with the reuse and server designs: every step of reuse §14 and item of its §15, and every piece of server §2 and row of its §12, has an owning step in the repository where its test lives (§5.9, §5.10). Also agreement with the proposals' §9 outline, and with the transport release plan as its amendment amends it: every obligation its §4 moves here is mapped in §5.3, TR1.4b's every clause and carried item is answered (§5.12), and each step's gate is the one its Phases 5 to 7 set. Also the completeness of §5, the tiers against GwzProcessOptimization §4.2, and the standing rules.
- **Safety attacks** what the plan permits to go wrong: a silent native route during the transition (Phase 2's refusal, the transport-scope predicate, the cfg arms); drift between the three dispatch tables; behaviour changes through the legacy adapter; an ordinary-path step left unmarked (§3.0); activation order, CS7.26's position among the reuse steps included; Windows gaps; secrets in evidence, and the snapshot's crossing to a server; gaming the ratchet by flipping `debt` to `permanent`, or by the driver-level `permanent` entries §3.0 now allows; the release gate's two allowlists; parallel lanes that share files; a contract revision that changes mid-program.
- **Remediation.** GwzProcessOptimization §4.1's cap: two rounds, and a third only for non-architectural corrections.
- **On GO.** After each part's GO, a status-only edit under AgentProcessRules §7.2: "Status: accepted at <SHA-256> after <review files> reported GO; this accepts <scope> only". After TR1.4a's GO the scope is the plan text revisions 1 and 2 revise; after TR1.4b's, the plan text. TR1.7's status edit to `GwzPyTransportDesign.md` lands with TR1.4b's acceptance (transport plan TR1.7). The program checkpoint records the acceptance, each step's review tier (GwzProcessOptimization §4.2) and the new edges for the transport plan's §6 sketch (§4). The plan becomes implementation authority only together with G0 and the operator's direction to start.
- **Re-review triggers.** A contract revision accepted after this plan, or a revision of the reuse or server design: a focused Consistency re-check of §2.1, §5 and the affected steps. The acceptance of TR1.5's or TR1.6's design: a focused re-check of CS3.10, CS8.3 and C1. A later amendment to the transport release plan that moves an obligation or a gate: a focused re-check of §1.1, §2.3 and §5.3. A step that must cross another step's files, or code that a transport plan step changes (R1): a handoff under L1-06.
- This plan authorizes no implementation, commit, tag, push or publish.

## Changelog

- 2026-09-27: status notes that the transport release carries this plan, pending TR1.4a and TR1.4b of [`GwzTransportReleasePlan.md`](../gwz-core/dev-docs/GwzTransportReleasePlan.md).
- 2026-09-27: revision 1 applies TR1.4a. Its dual review is pending.
  - D2 and D9 are superseded, and D7 names activation instead of 1.1.0.
  - §1.1 describes the transport release. §1.3 drops its exclusions of 1.1.0 S6.1–S6.3, the server, gwz-transport changes and public API changes, and says where each now lives.
  - G1 is split in two, and G2 becomes the build prerequisites for transport rows. §2.4, R1 and CS3.1's callers are rewritten.
  - §3.0 defines the **Ordinary path** marker, and eight steps carry it.
  - §5.3 maps the retired 1.1.0 S6.1–S6.3 obligations and the 1.1.0 amendment's Verdict-2 carried items; CS2.4 takes two of them. CS1.1 names every user of the type it renames, and waits on TR3.1 for them.
  - §4 and §7 follow the new gates. Each section and step that TR1.4b revises is marked.
  - §2.1 records revision 4's acceptance (Verdict-4) and the one open G0 obligation, the DRAFT flip before CS1.1. D1 is decided: path 1. CS1.8 is marked done at root `fa3f44c` and gwz-core `4bd92285`, with its first CI run still to be observed. The intro, the Phase 1 exit, §4, §5's intro, §5.2's B11 row and §5.5 follow.
- 2026-09-27: revision 2 applies the [first remediation plan](GwzCoreSessionPlan-RemPlan.md) after the [first verdict](GwzCoreSessionPlan-Verdict.md), NO-GO at SHA-256 `51cea8b6…`. Its focused re-review is pending.
  - S-P2-1: CS3.1's per-call legacy context covers every driver entry, in both builds. Its environment reads are `debt`, which CS4.8, CS6.1 and CS6.5 remove. CS3.3 is marked **Ordinary path**, and its spawn entries and CS3.4's stay `debt` until CS6.5.
  - S-P2-2: CS1.1 keeps the candidate protocol in step, by regeneration, with a lexical test; §2.4 gains the rule for both protocol builds; the Phase 1 exit requires the candidate build. C8 records the field-3 collision that blocks CS1.1.
  - S-P3-1: CS3.1, CS3.5, CS3.6, CS6.5 and lane C wait on TR3.1.
  - S-P3-2: §5.3's release-pins row names the four artifacts of today's git-tag pin.
  - S-P3-3 and C-P3-4: R8, CS2.10 and the Phase 4 exit carry `diff_log_read`'s bounded wait.
  - S-P3-4 and C-P3-5: C1 and D7 name the three native routes and the off switch.
  - S-P3-5: six steps carry "TR1.4b extends this", and §2.2 states the additive re-freeze.
  - C-P3-1: CS4.7 and CS4.8 carry the **Ordinary path** marker.
  - C-P3-2: the Phase 3 exit's dual review rests on CS3.4 alone.
  - C-P3-3: §1.3 cites the contract's §1 for gwz-transport changes.
  - Residual notes: §4's shared-file list, R1, CS1.8's title, G0's actor, and CS1.8's first CI run.
  - S-P2-1, refined after drafting: the per-call legacy context covers the environment and the member locks only. The legacy path keeps today's process-wide setup-job cap with its supervisor, cleanup permits and `gh` slots, listed `debt` until CS6.5 removes them after CS4.8, so gwz-py's candidate `TransportSession` stays on per-process budgets. CS3.1, CS3.5, CS3.6, CS6.5, the Phase 3 exit, §4 and §5.4 follow.
- 2026-09-27: revision 2 accepted for TR1.4a's scope at SHA-256 `58ab341a…` ([Verdict-1](GwzCoreSessionPlan-Verdict-1.md)). The status names the acceptance, and the corrections Verdict-1 records follow the GO:
  - C8 records the operator's decision: the contract's revision 5 gives `cancellation` field 8, and the candidate projection's tags 3–7 stay. §6.2 cross-references it; CS1.1 and §4 wait on revision 5's acceptance.
  - §2.4: once TR3.1 has landed, every step builds the candidate before it merges and records the build; the regenerator's pinned inputs are listed.
  - CS3.4's credential spawn has a time and kill bound, not a `gh` slot. CS6.5 says the public handler API takes an explicit context once the crate default goes (R9).
  - §2.2: a re-freeze is dual. §2.2 and §7 name TR1.4a's scope as revisions 1 and 2.
- 2026-09-28: revision 3 applies TR1.4b, after TR1.2's and TR1.3's GO. Its dual review is pending; it is the plan's G1 for every part it revises or adds (§2.2).
  - The per-operation runtime becomes TR1.2's model, one binding per operation over endpoint instances the host context shares: the Phase 3 heading and preamble, CS3.2 (reuse §3's endpoint configuration), CS3.7 (the entry, the binding seam and the registry's first form), CS3.8, CS3.10, CS3.11 and R4. Every sentence reuse §13 lists is replaced (§5.9).
  - CS3.7's panic path names `Command`'s `Drop` as the pattern it must not follow, and CS3.7 and CS4.1 lose their 1.1.0 dependencies. Phase 4 and Phase 5 are revised: CS4.5's exit path, fork rule and capture; CS4.7's double-budget note; CS4.8's removal of `TransportSession`; the new CS4.9; the new CS5.4, which gives the wire proof's test host the server's handshake; and CS5.3's release pins and hook rows. The Phase 4 exit names the `gwz-py` console script, and CS4.9 carries TR1.7's Surface review of reuse after the Phase 7 exit.
  - §5.3 maps the remaining moved obligations and the Verdict-2 carried items. §5.4 and §5.5 follow, and §5.9 to §5.12 map the reuse design, the server design, the extensions and TR1.4b's closure.
  - The six extensions get their own steps, each a dual re-freeze (§5.11): CS1.10 for CS1.1, CS1.9 and CS3.7 for CS1.4, CS7.26 for CS2.12, CS7.16 for CS3.6, CS6.6 for CS6.1 and CS6.7 for CS6.4. Each extended step's marker now points to its step, and every "TR1.4b revises this" marker is gone. CS2.3 and CS3.4 gain addenda for the server design's amendments.
  - New phases: Phase 7, connection reuse (CS7.1–CS7.27), and Phase 8, the server (CS8.1–CS8.32), the transport plan's Phases 6 and 7.
  - G0 is discharged, and names revision 5. CS1.1's candidate is pinned to an older schema by design, and CS1.1 moves both pins; `candidate-generator.json` joins its files. §2.4 adds the Linux runner rule for server rows, and the two phases' fixtures. §3.0 adds the citation forms, the rule for driver-level `permanent` entries, the large files, CS1.10's marker, CS3.10's reason for none, and the new dual reviews.
  - §1 describes the release and the scope with reuse and the server. R1, R2, R8, R10 and R11 are updated, and R12 records the transitional double budget. C1 stays open; C8 records revision 5's acceptance; C9 and C10 are new. §4 and §7 follow.
- 2026-09-28: post-GO corrections after TR1.4b's GO ([Verdict-2](GwzCoreSessionPlan-Verdict-2.md)), applied by the drafter in one patch; each reviewer confirms its own items. The status line is unchanged.
  - C-P3-1, S-P3-3: the Phase 6 preamble and CS6.5 say that O9 holds except for the lazy endpoint's read in `transport_binding.rs`, which CS7.24 retires, and CS6.5's check asserts exactly that set; CS7.24's test adds an allowlist with no `debt` entry once CS6.5 has merged. §5.4 and the Phase 8 preamble say the release gate passes once CS6.5 and CS7.24 have cleared gwz-core's `env` and `process` debt, and CS4.8 gwz-py's. §5.5's item 8 names CS7.24.
  - C-P3-2, S-P3-5: CS7.9 gains reuse §15.6's cancellation row, and CS7.11 §15.4's explicit-identity row; §5.9's items 4 and 6 name them.
  - C-P3-3: CS8.5, CS8.8 and CS8.13 depend on CS1.10, and CS8.14 on CS1.3, CS1.10, CS8.8 and CS8.9. §4's sketch and lanes follow.
  - C-P3-4: CS8.6 cites the server design's erratum of 2026-09-28.
  - C-P3-5: CS4.5's fork rule names the mechanism: the inherited Rust owners are leaked (`mem::forget`).
  - S-P3-1: §4 orders every pair of steps that name one file, or names an L1-06 handoff, which it does for CS6.5 with CS7.13, CS7.15 and CS7.16. New edges: CS7.10 → CS7.20 → CS7.13 → CS7.22 → CS7.19; CS7.12 → CS7.13; CS7.10 → CS7.14; CS7.5 and CS7.6 → CS7.10; CS7.16 → CS7.22; CS2.13 → CS3.8; CS3.4 → CS8.4; CS1.7 → CS8.4 and CS8.8; CS4.2 → CS5.2 → CS4.8; CS5.3 → CS8.30; CS7.25 → CS8.24; CS7.26 → CS8.3. Each step's dependency line names its new edge. The list gains `errors.py`, gwz-core's `scripts/run_tests.py`, `docs/RustApi.md` and `Cargo.toml`, the workflows CS1.7 and CS8.8 share, and four gwz-py files. After CS7.1 its split owners replace "the owners of", and the sketch is redrawn, recorded in the program checkpoint. CS7.10's file list names its pool files.
  - S-P3-2: CS8.9 with CS8.10 is one dual socket-host checkpoint, and CS8.17–CS8.19 one dual Windows checkpoint that re-freezes CS8.5, CS8.8 and CS8.14. §3.0's dual list and R2 follow: 28 dual, 74 single-axis and 3 Surface reviews.
  - S-P3-4: the Phase 7 exit names the rows whose owning steps follow it, which CS4.9 and CS8.32 carry.
  - S-P3-6: C11 records the credential spawn's `GIT_DIR`, `GIT_COMMON_DIR` and `GIT_WORK_TREE`, and CS3.4's working rule drops them, with its test.
  - Residual notes: CS1.10 states its merge order; CS7.2's `Request` API is additive or defaulted until CS7.10; CS8.13's descriptor row runs over a test program; the registry option is test-only, with a workflow-text test (CS7.11); the server's arms on another Unix refuse with `server_unavailable`; CS8.4's mode stays on after the Phase 8 exit; CS3.7 bounds an abandoned binding's life and tests its disposal at the next call.
  - Carried from [CS1.4 and CS1.5's Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md): the two steps' file lists; `thread_local CROSSING` among §5.4's `permanent` entries, with gwz-core's allowlist at 31 entries, 18 `debt` and 13 `permanent`; §3.0's ratchet sentence; CS1.4's measured 505 production lines; and one clause each in CS1.2, CS1.6, CS2.2, CS2.4, CS2.5, CS2.9–CS2.12, CS3.5, CS3.7, CS3.9, CS1.9's `shutdown`, CS8.10 (the socket host) and the Phase 1 exit.
  - Carried from [CS1.7's Verdict-1](GwzCoreSessionCS1.7-Verdict-1.md): §3.0 records the check's scope and its counted exclusions; CS1.7 and the Phase 1 exit record that its coverage of gwz-cli and gwz-py is local-only; CS1.7's file list matches its accepted object.
- 2026-09-28: accepted for TR1.4b's scope at `01adc2f0…` ([Verdict-2](GwzCoreSessionPlan-Verdict-2.md)). Both reviewers confirmed the post-GO corrections ([Consistency-2a](GwzCoreSessionPlan-ReviewConsistency-2a.md), [Safety-2a](GwzCoreSessionPlan-ReviewSafety-2a.md)). The lane owner then applied their last notes:
  - the status sentence takes §7's form;
  - §3.0's route to `permanent` is only for state whose type holds no payload (a marker, flag or counter), with a test where one can prove the reason (Safety-2a P3-7);
  - CS8.4's standing no-debt mode covers gwz-core's and gwz-transport's allowlists in gwz-core's `run_tests.py`, and CS8.30 adds it for gwz-py's allowlist in gwz-py's test run; the Phase 8 exit switches both on (Consistency-2a);
  - CS8.22 and CS8.29 depend on CS8.3, and CS8.3 shares `session_entry.rs` with CS7.26 under a handoff (Safety-2a);
  - CS8.10's snapshot test uses the worst case, the most minimum-size entries a frame can carry; the Windows checkpoint re-runs CS8.5's, CS8.8's and CS8.14's counterexamples on Windows (Safety-2a);
  - CS1.4's file list matches its accepted object; CS1.2 registers its modules in `session_host/mod.rs`, after CS1.4 (Consistency-2a).
