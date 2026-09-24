# GwzPyTransportSessionV4FoundationDesign — CONSISTENCY-AXIS REVIEW

**Review object:** The committed DRAFT v4 foundation design package, dated 2026-09-24, status "DRAFT v4 foundation design … design review pending". It has four parts:
- `dev-docs/GwzPyTransportSessionV4FoundationDesign.md` at root `fbee49ce45e0ee906ea0de45b6ac5c3db990e766`.
- The v4 paragraphs in `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` at gwz-core `a1f2102dd6da1bf96538b4972de7995255a53b48`.
- The `request_id_consumed` note and retry example in `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md` at gwz-py `0ca424f36a2872a0896abaea89641513352fdf47`.
- The WITHDRAWN status line of `dev-docs/GwzPyTransportSessionV3FoundationDesign.md` at root `fbee49c`.

**Baseline:**
- Root `fbee49ce45e0ee906ea0de45b6ac5c3db990e766`.
- gwz-core `a1f2102dd6da1bf96538b4972de7995255a53b48`.
- gwz-py `0ca424f36a2872a0896abaea89641513352fdf47`.
- The mux source came from gwz-transport `36ae2b13d7beaf289c72143e2f451c76489110ed`. That is the member pin in `gwz.conf/gwz.lock.yml` at `fbee49c`; the member's working tree differs only in `Cargo.toml`.

All text was read from commits with `git show <sha>:<path>`. Working-tree noise was ignored. The three HEADs matched the tuple at the start and at the end. `git status --short` showed no change to any reviewed path.

**Date:** 2026-09-24

**Axis:** Consistency: the V4 package against its controlling graph. That covers internal coherence, the exactness of the v2 amendments, the predecessor dispositions, claims about current source, the paired core paragraphs, the caller note and the status line. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — five P2 and eleven P3 findings; the five P2s block. Every P2 is a bounded contract or text correction; none is a new architectural root cause. I pre-commit to GO on a revision that resolves P2-1, P2-2, P2-3, P2-4 and P2-5 as specified.

---

## 0. Evidence base

**Commands.** All were read-only. No builds, tests or formatters were run.
- Tuple checks at start and end:
  - `git rev-parse HEAD`, `git -C gwz-core rev-parse HEAD` and `git -C gwz-py rev-parse HEAD` returned the tuple both times.
  - `git -C gwz-transport rev-parse HEAD` equals the lock pin.
  - `git status --short --` returned nothing for:
    - the V4, V3 and v2 documents (root);
    - `GWZDesign.md`, `GWZRequirements.md` and `src/transport_host` (gwz-core);
    - the guide, `native/src` and `src/gwz` (gwz-py);
    - `src/mux` (gwz-transport).
- `git show --stat` and full `git show` of `fbee49c`, gwz-core `a1f2102` and gwz-py `0ca424f`.
- `git show fbee49c:gwz.conf/gwz.lock.yml`, to get the gwz-transport pin.
- `nl -ba | sed -n`, `grep -n` and `git grep -n` over the paths below.

**Documents read.**
- The V4 design, lines 1–393, in full.
- V3 design lines 1–260: the status line and §§1–11.
- The filed V3 evidence:
  - `-Verdict.md`, `-Verdict-1.md` and `-RemPlan.md`;
  - `-ReviewConsistency.md` and `-ReviewConsistency-1.md`;
  - `-ReviewSafety.md` and `-ReviewSafety-1.md`;
  - `-ReviewSurface.md` and `-ReviewSurface-1.md`.
- `GwzPyTransportSessionV2Foundation-Verdict-3.md`.
- The v2 contract `GwzPyTransportSessionV2Design.md`, lines 1–61 (§§1–8).
- `gwz-py/dev-docs/GwzPyTransportDesign.md`: the status header and §§2–4 (lines 1–234).
- The caller guide at `0ca424f`, lines 1–101.
- Core documents:
  - `GWZDesign.md` and `GWZRequirements.md`, lines 1–66: the v2 and v4 paragraphs and the remote-transport amendment.
  - `GwzRemoteTransportDesign.md`, lines 118–128 and 425–440.
  - `GwzRemoteTransportRetryPlan.md`: a grep for bind and registration ordering.
  - `gwz-core/AGENTS.md`.
- Process documents:
  - `AgentProcessRules.md`: L1-07, L1-08, L1-33, L2-15 and §7.1–7.3.
  - `GwzProcessOptimization.md` §§1–4.
  - The status line of `GwzPyTransportConcurrencyDesign-1.md`, as precedent.

**Source read. These describe the starting point, not an implementation of V4.**
- gwz-core `a1f2102`:
  - `transport_host/mod.rs` 1–370.
  - `session.rs` 1–1009.
  - `request.rs` 1–464.
  - `session/driver.rs` 1–140 and 240–525.
  - `tests.rs` 1–60, 186–240 and 325–355.
  - `git/endpoint/placement_endpoint.rs` 150–300 and 780–930.
  - `git/gitbackend/transport_support.rs` 277–283.
- gwz-transport `36ae2b1`: `src/mux/mod.rs` 1–664, `asynchronous.rs` 1–227 and `routing.rs` 1–200.
- gwz-py `0ca424f`:
  - `native/src/transport_session.rs` 1–1512.
  - `dispatch/mod.rs` 1–140 and 380–548.
  - The function inventories of `operations.rs`, `shims.rs` and `lib.rs`.
  - `src/gwz/bridge.py` 1–480, `client.py` 320–420 and `errors.py`.

## 1. Findings

### [P2-1] The 6-second finish clock is shorter than core's sequential finish bound, so a slow but healthy cleanup faults the Client

**Location.**
- V4 §3.5 (line 73), §7.5 (line 272: "registered cleanup and request finish 6 s"), §5.2 (line 145), I8 (line 88) and §14 (lines 389 and 392).
- Source: `gwz-core/src/transport_host/request.rs:328-339`, `session.rs:30` and `766-808`, and `session/driver.rs:470-517`.

**Violated invariant.** §3.5's own sizing rule is "core's own deadline for that step plus a one-second margin, so a healthy supervisor's driven deadline fires first". §14 says owner clocks "only matter when the supervisor is dead".

**Evidence.**
- `TransportRequest::finish()` awaits the driver `Session::finish` first. Only after that does it await `ClientRequest::finish` for the endpoint registration (request.rs:328-339).
- Each `Session::finish` seals its own registration. It completes only when `drive()` retires that registration, or when `at.elapsed() >= CLEANUP` measured from that registration's own seal instant (session.rs:766-808; driver.rs:470-517). CLEANUP is 5 s (session.rs:30).
- So the endpoint window opens only after the driver window has closed. Core's own bound for "request finish" is two sequential 5-second windows, about 10 s, not 5 s.
- `RegisteredCleanup::finish()` has the same shape if it finishes the two registrations in turn. V4 does not say which it does.

**Reproduction.** Keep the supervisor live and cancel an accepted push.
1. The endpoint adapter is blocked in a read under the default 9-second transport timeout (`transport_support.rs:279-281`), so the stream terminal arrives 4.5 s after the Cancel.
2. Physical teardown then takes a further 4 s.
3. The driver registration retires at about 4.5 s.
4. `ClientRequest::finish` then seals the endpoint registration and waits for `pending_request` to clear. Core returns its report at about 8.5 s.
5. The owner's 6-second clock wins first. The finish future is dropped, and the record settles Cancelled, possible effect, cleanup unconfirmed.
6. §5.2 line 145 then sets `faulted`.

**Impact.** A healthy but slow cancellation permanently disables network admission for the Client: I8 applies, and per §14 nothing clears `faulted` before rollover. The report core would have produced is thrown away. The owner clock stops being a backstop for a dead supervisor.

**Remedy.** Derive each bound from what core actually does. Either set request finish and registered cleanup to at least 2 × CLEANUP + 1 s (11 s), or specify a candidate-path finish that seals both registrations together so the two windows overlap. The second option must leave legacy `request()` timing unchanged. Correct §3.5, §7.5 and §14 to match.

**Closure test.** With a live supervisor, hold the endpoint terminal for about 4.5 s and physical teardown for about 4 s after cancel. Assert that finish returns before the owner's clock, that the Cancelled terminal carries core's report, that the session is not faulted, and that the next admission succeeds.

**Classification.** Bounded contract/text correction.

### [P2-2] The capability preflight builds and installs a generation outside the claim-owned bootstrap step

**Location.**
- V4 §3.6 (line 77), §7.1 (line 242: runtime "installed only after bootstrap()"), §7.2 (lines 256–258), §6.2 (line 194) and §12 (line 344).
- Source: `gwz-py/native/src/transport_session.rs:469-483` and `373-457`; `gwz-py/src/gwz/client.py:349-391`; `bridge.py:385-406`.
- Controlling text: `gwz-py/dev-docs/GwzPyTransportDesign.md` lines 200–202 ("the capability preflight and dispatch must use the same live core receiver/runtime generation"). v2 §1 does not supersede this.

**Violated invariant.** §3.6 says "Generation-level decisions follow the same rules … the first admitting claim that finds no runtime becomes the constructing owner". §7.1 says a runtime is installed only after bootstrap.

**Reproduction.**
1. Create a new native Client and run `client.fetch(..., transport=TransportOptions(default_identity=...))`.
2. `_require_transport_capability` calls `bridge.call("transport_capabilities")` first (client.py:349-376, and 390 for streams). The bridge routes it to `session.call` with no issued ID (bridge.py:385-406).
3. Native `call_inner` answers it through `self.runtime()`. That builds the runtime with `from_environment()` and installs `state.runtime` without any bootstrap (transport_session.rs:469-483 and 373-457).
4. V4 §7.2 specifies only the claimed network entry, so this path stays unchanged.
5. The fetch's claim then finds a runtime already installed and skips construction. Phase 1's driver-mux `Ready` check refuses with `IoError`/NotRegistered (§6.2; §12 line 344).
6. Nothing in V4 bootstraps a runtime that is already installed, and rollover is deferred. Every later network call from that Client is refused the same way.

If the preflight is instead changed to bootstrap, that construction has no owner, clock or failure rule anywhere in V4.

**Impact.** As written, a Client that uses explicit SSH identity options cannot admit network work. The alternative fix would reopen an unowned construction interval.

**Remedy.** Specify the non-claimed `call` path. The accepted design requires the preflight to use the dispatch generation. So any caller that finds no runtime — a claim or the capability preflight — should become the constructing owner under §3.6's `constructing` flag, clock, bootstrap and failure rules. There is no record for it to settle. Keep §7.1's invariant.

**Closure test.**
- On a fresh Client, run the capability preflight and then `call` and `submit` network operations. All must be admitted on the same generation.
- The driver mux must be `Ready` before the first admission.
- A bootstrap failure during the preflight must leave the session `Open`, and a later claim must be able to construct.

**Classification.** Bounded contract correction. It extends the existing constructing-owner rule; it is not a new architectural root cause.

### [P2-3] The deferred 8 MiB ledger upgrade sits at an infallible, post-registration `Claim::accept`

**Location.**
- V4 §7.1 line 250: the ledger stage "attaches at `reserve` (4 KiB), `Claim::accept` (upgrade to 8 MiB and the primary reader cursor, before the gate opens) and `settle`".
- §1 line 15: deferred stages "attach at the three points named in §7.1".
- §5.3 line 157 gives `accept(TransportRequest) -> AcceptedOperation`, which has no refusal. §5.2 line 139 has no accept-failure row.
- §7.2 line 258 places accept after `Ready`, checkpoint 2 and the worker spawn.
- v2 §5 line 35: "A top-level slot or result-budget refusal is `TransportSessionFull` before acceptance and effects".
- v2 §6 line 47: "Aggregate pressure refuses operations before acceptance".
- v2 §2 line 19: "A pre-registration capacity/session/placement refusal does not consume that request ID".
- Caller guide lines 53 and 91.

**Violated invariant.** v2's session-full refusal happens before registration and does not consume the request ID. §11 also claims "Nothing else in v2 §§2–8 changes".

**Reproduction.**
1. Seven completed operations each retain about 8 MiB of results, and one running operation holds its 8 MiB reservation.
2. A new operation with an explicit request ID claims a free slot and passes phase 1.
3. Phase 2 returns `Ready`, so the ID is now registered.
4. At `Claim::accept` the 8 MiB upgrade does not fit.
5. V4 gives accept no way to refuse. Any refusal the ledger stage later adds at this point is Refused/Registered with `request_id_consumed=true` and a cleanup finish.

**Impact.** A v2 session refusal becomes one that consumes the request ID. That contradicts v2 and the guide's advice to release records and retry the same ID. The only alternative is for the ledger stage to move the attach point, which restructures V4's admission sequence — exactly what V4 §1 promises will not happen. The ledger itself is deferred, but the attach point is the shape V4 fixes now.

**Remedy.** Take the 4 KiB → 8 MiB upgrade at T2 `claim`, together with the top-level slot and before any core work. A refusal there settles TransportSessionFull/NotRegistered. Bind only the already-reserved primary cursor at `accept`. Update §5.2, §7.1 and §12.

**Closure test.** This belongs to the ledger stage but should be specified now. With the ledger within 8 MiB of full, admission refuses with TransportSessionFull, `request_id_consumed=false`, and no core registration. After records are released, the same ID is admitted.

**Classification.** Bounded contract correction.

### [P2-4] The paired core requirement forbids the expiry closes that V4 keeps and relies on

**Location.**
- `gwz-core/dev-docs/GWZRequirements.md` line 11 says: "Core MUST NOT own any timer, callback or thread with authority to decide an operation's outcome or close a generation on expiry; every core wait MUST be bounded by its caller's own clock". The same paragraph says "The existing `TransportRuntime::request()` path, including … error/cleanup timing, MUST remain unchanged."
- `GWZDesign.md` line 11 says only that "Core gains no timer or callback".
- V4 §3.5 line 73: "nothing but an owner acts on an operation or a generation" and "a healthy supervisor's driven deadline fires first".
- V4 §3.6 line 77 lists only protocol errors and supervisor exit as tolerated core closes.
- V4 §6.1 line 179: bootstrap is "bounded by the mux's own 5-second deadline".
- Source:
  - the supervisor's cleanup-expiry close, `session/driver.rs:498-501`;
  - the mux bootstrap-deadline disconnect, `mux/mod.rs:536-540`, and route-deadline terminals, `542-595`, both advanced by the supervisor (`driver.rs:245-249`);
  - legacy waits with no caller clock, `transport_host/mod.rs:216-224` and `234`, and `request.rs:328-339`.

**Violated invariant.** The core baseline must be satisfiable, and the paired requirement, paired design and V4 must agree.

**Reproduction.**
1. Operation A is cancelled while its endpoint stays silent.
2. A's sealed driver registration is not retired within CLEANUP.
3. On the next `drive()`, the placement supervisor calls `close_state` (driver.rs:498-501). That closes the generation underneath accepted operation B.

This behavior is unchanged by V4. It is a core thread closing a generation on expiry, which the MUST NOT forbids. Removing it, and the mux deadlines, in order to comply would change legacy `request()` timing — which the same paragraph requires to stay unchanged. It would also remove the "core deadline fires first" premise that sizes every V4 owner clock.

**Impact.** The requirement cannot be met as written. V4 §3.5 and §3.6 misstate which parties can close a generation. §12 has no row for a generation closed by a sibling operation's cleanup expiry.

**Remedy.** Restate the requirement the way GWZDesign already states it:
- Core gains no new timer, callback or thread with authority over native records.
- The existing supervisor-driven mux and cleanup deadlines remain. They fire before owner clocks and reach owners as faults.
- The caller-clock rule applies only to the Python-session candidate path.

Add cleanup-expiry and mux-deadline closes to §3.6's list, and add a matching §12 row.

**Closure test.** A documentation check that the requirement names the core deadlines it keeps. A native test in which a sibling's cleanup expiry closes the generation under an accepted operation: B settles exactly once through its owner (Failed, possible effect), with no second terminal.

**Classification.** Bounded text correction.

### [P2-5] The reserved bootstrap ID shares the caller request-ID namespace

**Location.**
- V4 §6.1 line 179: the ID is `bootstrap-<generation-serial>`, "never a caller request ID".
- §11 item 6, line 316: "is not a caller request ID, is never reported".
- §14 line 390, and line 11 of both GWZRequirements and GWZDesign.
- v2 §2 lines 13 and 19: the grammar is the only issuance constraint, and an ID is refused only if "already used in the current core generation of the same Client".
- Source: the same identifier grammar in `request.rs:390-392` and `mux/mod.rs:650-652`; duplicate checks against `used` in `session.rs:481-485` and `685-688` and in `mux/mod.rs:247-253`.

**Violated invariant.** §11 item 6, and v2 §2's duplicate rule.

**Reproduction.**
1. On a fresh Client, the first network operation uses the caller request ID `bootstrap-1`. It passes the grammar, and it is the reserved form for the first generation.
2. Construction has already registered `bootstrap-1` on both sessions.
3. Phase 1's duplicate pre-check refuses it with InvalidRequest/NotRegistered. §12 line 345 describes that case as "consumed earlier".

The same thing happens in every generation.

**Impact.** A valid caller ID that was never used is refused. The reserved registration becomes visible to callers, which is exactly the case §14 line 390 says would need correcting.

**Remedy.** Put a per-generation random token, inside the existing grammar, into the reserved ID. Nothing else should expose that token; in particular, do not reuse the session nonce, which appears in operation IDs. Alternatively, amend v2 §2 explicitly with a reservation that callers can see.

**Closure test.** The deterministic reserved forms are admitted as caller request IDs in the first generation. The bootstrap ID never appears in results, events or descriptors.

**Classification.** Bounded contract correction.

### [P3-1] The completed retry example still mishandles task cancellation (V3 Surface P3-2 is not closed)

**Location.**
- Guide lines 55–79: the example, and line 79's claim that "Every handle the example creates reaches release … whether it is refused, cancelled, fails or completes".
- Guide line 95: `GwzOperationCancelled` is "a subclass of `asyncio.CancelledError`".
- Guide line 53 lists it separately from `GwzBridgeError`.
- V4 §2 line 29 records the disposition. V4 §11 item 2, line 312, says "`GwzBridgeError` …, including `GwzOperationCancelled` before acceptance".
- v2 §2 lines 15 and 17.

**What goes wrong.**
- Cancel the task while it awaits the first `handle.accepted()` or the retried one. Under the guide's hierarchy, `except GwzBridgeError` does not catch the exception, so neither handle is released. Its Cancelled record stays until expiry, contrary to line 79.
- Under V4 item 2's "including", the first `except` would instead catch the caller's cancellation. When `request_id_consumed` is False, it would then start a new operation the caller asked to cancel.
- The `finally` comment claims that cancelling `handle.result()` joins the operation. v2 and the guide define that behavior only for `accepted()` and for helper tasks. If it does not hold, `release()` raises OpenOperation and hides the cancellation.

**Impact.** The example's lifecycle guarantee is false on the cancellation paths, and the exception hierarchy is described two different ways.

**Remedy.** State the hierarchy once; v2 says it is a CancelledError subclass. Give each handle its own try/finally that cancels on BaseException before it releases. Do not rely on undefined `result()` cancellation behavior.

**Closure.** Walk through cancellation at the first `accepted()`, at the retried `accepted()` and at `result()`. Each handle is released exactly once, the cancellation is re-raised, and no retry starts.

### [P3-2] The intent table marks Issued and Attempting records close-owed

**Location.** V4 §3.4 line 65 (the close row, first column: "mark close-owed"), against §9 line 301 (only Admitting, Accepted and Finishing records; at most eight) and §10 line 307. v2 §6 lines 45 and 47: summaries only for operations live at close, at most eight; the summary charge is reserved only inside the 8 MiB admission allowance.

**What goes wrong.** Close a Client holding 64 issued handles and 8 live operations. Following §3.4, `settle` copies 64 extra summaries.

**Impact.** The close report exceeds v2's limit of eight. The 4 KiB reservations for unstarted records carry no summary charge. The design gives two different instructions for one flag.

**Remedy.** Remove "mark close-owed" from that column.

**Closure.** Closing with 64 issued and 8 live records yields exactly eight summaries.

### [P3-3] §13 test 4 cannot reach its owner-clock cases, and its supervisor case contradicts §6.3

**Location.**
- V4 §13 line 377, §3.5 line 73 and §6.3 line 219.
- mux `mod.rs:285-286` and `536-540`.
- core `session.rs:784-789` and `809-839`, `driver.rs:498-516`, and `tests.rs:327-335`.

**The three cases.**
- **(a) Holding Bound.** With a live supervisor, the mux's own 5-second bootstrap deadline disconnects first, so the owner's 6-second clock never wins. §3.5 itself says this.
- **(b) Bound just before the clock.** Bound "just before" the owner's clock would arrive after the mux had already closed at 5 s. The race this case names cannot happen.
- **(c) "Stop the placement supervisor … the owner's finish clock wins … `faulted`".** This depends on what "stop" means:
  - If it means the exit or unwind that §6.3 hardens, the session is closed. A finish started afterwards returns immediately, because `seal()` completes results on a closed session (session.rs:784-789; tests.rs:327-335). There is no clock win and no `faulted`.
  - If the supervisor stops while a finish is in progress, `close()` alone completes no registration result (session.rs:809-839); only `drive()` does (driver.rs:498-516). The waiter then sits until the owner's clock, which contradicts §6.3's "wake … promptly instead of at their owner's clock".

**Impact.** The required "one arbiter" proof can pass without ever exercising the owner clock. One of its expectations fails against the hardening V4 itself specifies.

**Remedy.** Have cases (a)–(c) stall the supervisor-driven mux clock — hang it, not exit it. Add a separate hardening test. State that hardening completes pending registration results rather than only closing and signalling.

**Closure.** Run as written, the revised tests reach the owner-clock branch in (a)–(c) and the prompt-wake branch in the hardening test.

### [P3-4] The transition table, fault matrix and §11 item 5 disagree

**Instances.**
- **(i) Unreachable refusal rows.** §5.2 lines 123 and 126 and §12 line 333 refuse Closing and Closed at `begin_attempt` and `claim`. But close settles every Issued and Attempting record inside its own intent (§3.4 line 65; §9 line 301) and also refuses `reserve`. A later call therefore reads the retained Cancelled terminal. §13 test 5 cannot observe Refused/InvalidRequest in those states; only `faulted` reaches the row.
- **(ii) Missing effects on the spawn–accept panic row.** §5.2 line 138 says a dropped Claim with Registered provenance closes the generation and sets `faulted`. §12 line 354 (panic between spawn and accept, Registered) omits both.
- **(iii) Two different drop-path terminals.** §11 item 5, line 315, publishes "Failed (or Cancelled if cancellation had been signalled)". §5.2 line 146, §5.3 line 158 and §12 line 360 always settle Failed.
- **(iv) Overlapping guards.** §5.2 lines 143 and 144 overlap. A cancelled operation whose handler returned Err satisfies both "handler Err → Failed" and "cancellation signalled, no Completed output → Cancelled". §12 line 355 expects code 73.
- **(v) Missing post-mutation rows.** Failures after pool mutation other than the retirement timeout also close the generation:
  - `install_capacity_pair` failing after the guard is armed (session.rs:622-626);
  - an authority failure (669-672).

  §12 has no row for either.

**Impact.** The typed-field closure tests (§13) get contradictory expectations, and an implementation may choose either code.

**Remedy.**
- Regenerate §12 from §5.2, with disjoint guards and an explicit precedence for cancellation.
- Drop Closing and Closed from the T1/T2 refusal rows.
- Choose one drop-path kind across §11, §5.2 and §5.3.
- Add the missing rows.

**Closure.** Each §12 row cites the §5.2 row it instantiates, and all guards are disjoint.

### [P3-5] Refused T1/T2 transfers settle records outside the ownership rules

**Location.** V4 R2 (line 10), §3.2 (line 42), T2 (line 52), I1 (line 81) and I3 (line 83); §5.2 lines 123, 125 and 126; §7.2 line 256; §8 line 288.

**What goes wrong.**
- §5.2 has `claim` settle an Attempting record (slot full, or session closed or faulted), and `begin_attempt` settle an Issued record.
- The Attempting owner is the Python attempt. §3.2 lists its exits as T2, `abandon_attempt`, cancel and close. I3 says the terminal is written "by the record's owner at that moment". R2 leaves a failed transfer with the sender.
- Following R2, the ninth-slot refusal would be recorded through `abandon_attempt(_typed_pre_entry_failure(exc))`. That mapping (IoError, InvalidRequest, else InternalError) turns TransportSessionFull into InternalError.
- Following §5.2 and §7.2, the claimant records TransportSessionFull.

**Impact.** The claims "every change of owner is T1–T4" and "the owner writes the terminal" are false for these rows, and the two rules retain different codes.

**Remedy.** Define a refused T1/T2 as one atomic decision that settles on the sender's behalf. List it in §3.2, R2 and I3.

**Closure.** Refused claims from an Attempting record retain TransportSessionFull or InvalidRequest, and `abandon_attempt` returns that disposition.

### [P3-6] Expiry discards an actively owned `Attempting` record

**Location.** V4 §3.4 line 67: Issued and Attempting "discard after 15 minutes", but Admitting is "not applicable (owner is active)". Also §3.2 line 42, §8 line 293, and v2 §6 line 49.

**What goes wrong.** v2 expires only completed, refused and unstarted records. An Attempting record — one whose `accepted()` has been called — is none of these, and it is owned by a running attempt. If its native call waits more than 15 minutes from issuance on a blocked executor, the record is discarded. The late `claim` and `abandon_attempt` then find nothing, and the raised error cannot match a retained terminal (§8 line 293).

**Impact.** This is an unlisted change to v2 §6. It is also a timer acting on an owned record, contrary to R1 and R3.

**Remedy.** Treat Attempting like Admitting: expiry is not applicable.

**Closure.** In the timer stage, an Attempting record older than 15 minutes survives, and its late claim settles normally.

### [P3-7] The §6 API sketches cannot carry the owner's observation points

**Location.** V4 §1 line 15; §6.1 lines 170–176 and 179; §6.3 lines 210–213 and 217; §7.2 line 258; §7.5 line 272. V3 design lines 124–127 and 145.

**Two problems.**
- **(a) Progress.** `Admission::progress(&self) -> Progress`, annotated `NotEntered | MayHaveRegistered | Registered`, is read before the consuming call `register_and_open(self)`. A snapshot taken then is always NotEntered. The unwind path therefore needs a shared record written inside phase 2. That is v3's `RegistrationWitness(Arc<AtomicU8>)` — the mechanism §1 says V4 "withdraws".
- **(b) Bootstrap.** `bootstrap()` is a single future, yet V4 says it is awaited "in two clocked steps" with separate 6-second bounds. One future can carry only one timeout.

**Impact.** A literal implementation either classifies an unwind after insertion as NotRegistered, or bounds Ready at 12 s. §1's list of withdrawn mechanisms is also inaccurate.

**Remedy.** Declare a shared progress handle and correct §1. Split bootstrap into a Ready step and a finish step, or state one combined bound.

**Closure.** The §13 test 3 unwind reads MayHaveRegistered through the handle. A held Bound fails at the Ready bound.

### [P3-8] `request_id_consumed` is false on a duplicate refusal

**Location.** V4 §12 line 345 ("Refused / NotRegistered … no (consumed earlier)"), §5.1 line 107, §11 item 2 line 312. Guide line 53 ("Do not work this out from the error code") and example lines 60–65.

**What goes wrong.** A duplicate is refused NotRegistered, so the field reads False. Yet the ID is consumed in this generation, and the cause of the refusal never clears. A caller who has been told not to read the error code cannot tell the difference. The guide's own example retries such an ID.

**Impact.** In a common case, the field's name contradicts its value, and callers waste retries.

**Remedy.** Report a duplicate as consumed: core's pre-check proves prior use and can return a typed variant. Otherwise, rename or qualify the field.

**Closure.** A duplicate refusal carries `True`, or the guide states this exception in a way callers can act on without reading the error code.

### [P3-9] §14 promises `faulted` for closed generations that never set it

**Location.** V4 §14 line 389, §3.6 line 77, §5.2 lines 135–136, §12 lines 341, 343 and 344, I8 line 88; core `driver.rs:498-501`.

**What goes wrong.**
- Three kinds of close leave the runtime installed and the session `Open` and not faulted:
  - a phase-1 cancel or owner-clock win after pool mutation;
  - a retirement timeout;
  - a core cleanup-expiry close.
- Every later claim runs phase 1 against the closed generation. It retains IoError/NotRegistered with `request_id_consumed=false`, which §12 labels "yes on a later generation".
- No later generation exists before rollover.

**Impact.** A permanent loss of network admission is reported as a series of transient, retryable refusals.

**Remedy.** Set `faulted`, or return a typed generation-closed refusal, whenever an installed generation closes. Otherwise correct §14.

**Closure.** After a post-mutation cancel, the next admission is refused with the faulted or closed disposition.

### [P3-10] A per-handle `asyncio.Lock` cannot serialize cross-loop `accepted()` calls

**Location.** V4 §8 line 295; v2 §8 line 61 ("cross-loop use"); guide line 101.

**What goes wrong.** An `asyncio.Lock` binds to one event loop when it is first contended, and it is not thread-safe. The guide allows handle methods to run on different loops in different threads. Two concurrent `accepted()` calls on different loops either raise RuntimeError or depend on a cross-thread future wake-up. When neither happens, the second call reaches T1's InvalidRequest instead of "the first's outcome" that v2 §2 promises.

**Remedy.** Serialize through the native record — wait on its Accepted or terminal state — or use a thread-safe primitive.

**Closure.** In the handle stage, concurrent `accepted()` calls on two loops return the same handle or the same retained refusal.

### [P3-11] V4 supersedes frozen text in the accepted Python transport design without naming it

**Location.** `gwz-py/dev-docs/GwzPyTransportDesign.md` (status: accepted), lines 153–164: the frozen core additions; "Existing `TransportRuntime::request(meta, operation_id)` … remain the operation/cleanup APIs. No extra registration … is needed". Also lines 107–116, the per-call `runtime.request` flow. v2 §1 (line 9) supersedes neither. V4 §1 and §11 do not cite the document. AgentProcessRules L1-08 applies.

**What goes wrong.** For the Python session, V4 replaces `request()` with `bootstrap`, `admit_local` and `register_and_open`, and adds a reserved registration. The operator permitted internal API change, but L1-08 still requires the superseded frozen text to be named.

**Impact.** Two controlling documents conflict, and there is no precedence trail between them.

**Remedy.** List the supersession of GwzPyTransportDesign §§2–3 in V4 §1.

**Closure.** Both documents carry the precedence trail.

## 2. Invariant analysis

**Findings from the stop verdict, re-traced against V4 §2's dispositions.**

| Finding | V4 disposition | Re-trace | Status |
| --- | --- | --- | --- |
| Safety P2-1: unowned pre-entry interval | §3.2 Attempting owner; T1 and T2; `_admit` | If the executor is shut down, the `to_thread` task raises; `except BaseException` then calls `abandon_attempt(IoError)`, which settles Attempting. An encoding failure takes the same path. When a native claim races, the first transition wins and the other only reads (§3.3). | Closed |
| Safety P2-2: watchdog versus `Ready` | Watchdog deleted; phase 2 synchronous; owner timeouts | No core callback can fire after `Ready`. The mux bootstrap deadline is cleared at Ready (`routing.rs:176`) and only acts in Binding or Rejecting (`mod.rs:536-540`). Owner timers are dropped on completion. | Closed for the original sequence. The core expiry closes that remain are P2-4. |
| Consistency R2-C-P2-1 | §11 item 5 | Finish unwind is now an explicit v2 §5 supersession, and a caught handler panic keeps finish-first. | Closed. A residual kind mismatch is P3-4(iii). |
| Consistency R2-C-P2-2 | Split rows in §12 | Pre-mutation timeouts are "yes"; the retirement timeout is "no". This matches `session.rs:522-580` and `584-668`. | Closed for timeouts. Other post-mutation failures are P3-4(v). |
| Surface P3-2 | The completed example | The refusal branches now release their handles; the cancellation branches still do not. | **Not closed** (P3-1) |

**Source claims in §6 and §7 that held.**
- **First-request bootstrap today.** The endpoint and driver registrations come first. Then `begin()` enqueues the Bind offer carrying that request ID and moves the mux from Unbound to Binding with a 5-second deadline, and `ready()` awaits Bound (`mod.rs:209-236`; mux `270-293`; `asynchronous.rs:96-104`).
- **Cancel while Binding.** Cancelling the offer's request while Binding disconnects the mux (mux `464-469`). After Ready, a cancel only seals (`470-478`), and finish leaves a tombstone (`516-526`).
- **Endpoint Bind admission.** The endpoint requires the Bind's request to be registered and live, and it replays registrations into the new endpoint mux (`session.rs:864-898`; `routing.rs:110-148`). Dropping a pending bootstrap closes the generation (`tests.rs:191-204`).
- **`Session::register` ordering** (`680-705`). It runs the checks, then the mux insert, then the core insert, all under one lock. So an Err proves nothing was inserted, and §6.3's Progress points can be placed.
- **Limits.** Core allows 256 (`483`, `685`). The mux default is also 256 (`mod.rs:49`, `256`), and its validation accepts 1–4096 (`182-183`), so raising both by one stays within bounds.
- **`admit_client_request`, `install_capacity` and `CapacityMutation`** (`449-679`, `240-250`). There is one arrival deadline. Waits happen before the guard is armed, and retirement after. The guard closes the generation on drop or failure.
- **Phase 2 is synchronous.** Every step is a synchronous call: `request.rs:18-31` and `351-358`; `begin` at `session.rs:706-714`, which returns Ok on Ready (mux `289`); and backend attach at `mod.rs:235`.
- **Executor.** It is multi-threaded with one worker and `enable_all()` (`transport_session.rs:165-169`).

**Exactness of §11.** Each of the six items maps to real v2 text:
- Item 1 maps to v2 lines 19 and 39.
- Item 2 is a new field.
- Items 3 to 5 map to line 37.
- Item 6 maps to line 39.

The checks against v2 found these unlisted changes: P2-3, P2-5, P3-6 and P3-10. Outside v2, P3-11 is another.

These v2 rules were checked and held: the unstarted-cancel rule, one-shot `accepted()`, CLI refusal before construction, capacity, slot and worker ceilings, spawn-failure consumption, ledger isolation, close join, post-close reads, repeat cancel, release refusal, and typed codes and effects.

**Ownership attacks that held.**
- Every phase in §3.2 has exactly one owner row.
- The atomicity of T3 follows from `send` returning the value when it fails.
- §7.4's session-then-record lock order agrees with §9 (cancel after both locks are released) and with §10 (settle).
- The finish-first rule in I7 agrees with §5.2 lines 141–144 and with §7.2.
- A cancel that races acceptance is covered by checkpoint 2 and by the pending-cancel note taken at accept.

**Paired core paragraphs.** Both agree with V4 §6 on the following points:
- bootstrap;
- the two phases, with phase 2 synchronous;
- the `Admitted` variants and Progress;
- supervisor hardening;
- legacy `request()`;
- generation pinning.

GWZDesign's "gains no timer" matches V4 §3.5. GWZRequirements' stronger MUST NOT does not (P2-4). The v2 paragraphs that are still present stay compatible, subject to V4's listed amendments.

**Caller note.** The meaning it gives `request_id_consumed` matches V4 §5.1 and §11 item 2, including the qualifier that the generation must still be open. Its defects are P3-1 and P3-8.

**V3 WITHDRAWN line.**
- L1-33 holds: V3 correctly stays in live dev-docs, because V4 cites it and its reviews are that object's latest rounds.
- L1-08 holds: V3 points to V4 and to the stop verdict, and V4 points back.
- L1-07 holds: no freeze word is misused.

§7.2's exact pattern is not used, as recorded under §3.

## 3. Risks and next action

**Residual risks below the finding bar.**
- **Legacy limit.** Raising the core and mux per-generation limits "by exactly one" everywhere gives legacy runtimes that never bootstrap 257 caller IDs. No existing test pins 256, but "`request()` unchanged" is only strictly true if the increase is tied to bootstrap.
- **Legacy timing on panic.** Supervisor hardening changes how legacy `request()` behaves when `drive()` panics: waiters wake instead of hanging. That is an improvement, but it is not "unchanged".
- **For the Safety axis.** §7.2 wraps only `from_environment()` in `catch_unwind`. An unwind inside `bootstrap()` must still clear `constructing`; otherwise close (§9) and waiting claims wait forever.
- **Two refusal points for duplicates.** A request ID that is still live is refused at `reserve`, that is, at `start_*`. One whose operation has completed is refused at `accepted()`. The guide describes only the second.
- **V3 status line.** It uses "WITHDRAWN" rather than §7.2's exact supersession pattern: there is no scope clause, no "as of" clause, no evidence-validity clause and no changelog. Its historical tail still says "this amendment is a new exact tuple for re-review". This follows the program's own precedent ("REJECTED" in `GwzPyTransportConcurrencyDesign-1.md`), and the current authority remains identifiable.
- **Naming.** V4 calls itself a §7.1 transition design but does not carry the `TransitionDesign` name.

**Next action.** Make one bounded revision covering the V4 design, the paired core paragraphs and the caller note. It should resolve P2-1 to P2-5, fixing the P3s alongside. Commit it as a new exact tuple for re-review. No finding here is a new architectural root cause.
