# GwzPyTransportSessionV4FoundationDesign — CONSISTENCY-AXIS RE-REVIEW

**Review object:** The first remediation of the DRAFT v4 foundation design package, dated 2026-09-24. It covers:
- `dev-docs/GwzPyTransportSessionV4FoundationDesign.md` at root `11358352a237a673ecf0fd82347b45ca4aca8950`. Its status reads "DRAFT v4 foundation design …, first remediation; focused re-review … required".
- The v4 paragraphs in `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md` at gwz-core `d43b1474566eb607e2fb6d9d2da1b7419149f1dd`.
- The `request_id_consumed` note, refusal table and retry example in `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`, plus the new "Draft foundation pointer" in `gwz-py/dev-docs/GwzPyTransportDesign.md`, both at gwz-py `0ffc4cb7bbdeff8244ee4c5c78c4b3d1f91f6753`.

**Baseline:**
- Revised tuple:
  - root `11358352a237a673ecf0fd82347b45ca4aca8950`;
  - gwz-core `d43b1474566eb607e2fb6d9d2da1b7419149f1dd`;
  - gwz-py `0ffc4cb7bbdeff8244ee4c5c78c4b3d1f91f6753`.
- Mux source is gwz-transport `36ae2b13d7beaf289c72143e2f451c76489110ed`, which the lock still pins.
- Reviewed revision: root `fbee49c`, gwz-core `a1f2102`, gwz-py `0ca424f`.

All text was read from commits with `git show <sha>:<path>`, and working-tree noise was ignored. The three HEADs matched the revised tuple at the start and at the end. `git status --short` showed no change to any reviewed path.

**Date:** 2026-09-24

**Axis:** Consistency: the remediated V4 package against its controlling graph, its own round-1 findings and current source. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — all five round-1 P2s and all eleven round-1 P3s are closed against their original counterexamples. One new P2 (R2-C-P2-1) blocks, and there are four new P3s. The new P2 is a bounded contract/text correction; no finding is a new architectural root cause. I pre-commit to GO on a revision that resolves R2-C-P2-1 as specified.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| P2-1 | Each owner clock is the sum of the core deadlines that run in sequence, plus 1 s. Finish, registered cleanup, lease finish and shutdown get 11 s (§3.5 lines 80–82; §7.5 lines 348–355). | I re-ran the round-1 sequence: the driver retires at about 4.5 s and the endpoint returns core's report at about 8.5 s. Core's worst case for `TransportRequest::finish()` is two sequential CLEANUP windows, about 10 s (`request.rs:328-339`; `session.rs:30`, `766-808`; `driver.rs:470-517`), which is inside 11 s. The operation settles C4 with core's report, the generation check finds the generation open, and nothing is faulted (§12 line 462). | Closed |
| P2-2 | The capability preflight becomes the constructing owner (§3.6 line 86; §7.6 rule 1, line 363). | Fresh Client with an identity-option fetch. The preflight used to install an unbootstrapped runtime (`transport_session.rs:469-483`). It now constructs under `Construction`, bootstraps and installs. The claim then finds a Ready driver mux, and GwzPyTransportDesign §4's same-generation rule is kept. | Closed. A new caller-visible masking on this path is R2-C-P3-1. |
| P2-3 | The ledger upgrade moves to T2 `claim` (S5/S6; §7.1 lines 311–315), and `accept` becomes infallible. | Seven retained 8 MiB results plus one running operation: the new claim refuses at S6 with TransportSessionFull/NotRegistered before any core work. After release, the same ID is admitted. The §11 preamble (line 409) records this as v2 §5/§6 behavior. | Closed |
| P2-4 | GWZRequirements is restated; V4 §3.5 line 82 and §3.6 line 88 list the retained core closes; §12 line 465 adds a row. | The sibling cleanup-expiry close (`driver.rs:498-501`) is now permitted: "MAY close a generation … MUST reach callers as errors". The caller-clock rule is scoped to the candidate path. Legacy `request()` is unchanged apart from waking waiters on exit. Test 11 covers it. | Closed |
| P2-5 | The reserved ID becomes `bootstrap-<32 hex>`, a random token; the extra registration applies only to session runtimes. | `bootstrap-1` can no longer collide (§6.1 line 224). Legacy constructors keep 256 and refuse bootstrap (line 206; GWZRequirements). | Closed |
| P3-1 | The example is rewritten and the exception hierarchy is stated once. | I traced every path: a refusal that consumed the ID, a refusal that did not, a second refusal, success, and task cancellation at the first `accepted()`, the retried `accepted()` and `result()` (guide lines 68–85). Each handle is released once, cancellation is re-raised, and no retry follows a cancellation. | Closed |
| P3-2 | "Mark close-owed" applies only to network Admitting, Accepted and Finishing records. | §3.4 line 70 and §9 line 399 agree, and local records are excluded (line 74). | Closed. Test 7's setup cannot be built; see R2-C-P3-4. |
| P3-3 | Test 4 stalls the mux clock, there is a separate exit test, and hardening completes results. | The original cases (a)–(c) are now reachable. With the clock stalled, the readiness clock wins. Bound arriving before 6 s resolves Ready. The exit test matches §6.3's final retirement pass (line 277). | Closed. The new stalled-clock clauses lack setup; see R2-C-P3-3. |
| P3-4 | §12 is regenerated with citations; C-rows are disjoint; one drop-path kind; new A6 rows. | Each original instance is fixed: (i) S4 and S7 now cover faulted only (line 183); (ii) the A14 row closes the generation and sets faulted (line 456); (iii) C6, C7, §5.3 and §11 item 5 agree; (iv) C4 and C5 are disjoint with a stated precedence; (v) line 446 lists the pair and authority failures. | Closed. New citation defects are R2-C-P3-4. |
| P3-5 | A refused T1 or T2 settles on the sender's behalf (R2 note at line 13; §3.3 line 59; I1; I3). | A ninth claim from an Attempting record: S6 settles TransportSessionFull. `abandon_attempt` is then a no-op that returns that disposition, so the typed code is kept (§8 line 389). | Closed |
| P3-6 | Expiry does not apply to `Attempting` (§3.4 line 72; §11 item 11). | An attempt stuck behind a blocked executor for more than 15 minutes keeps its record. Test 7 has the timer clause. | Closed |
| P3-7 | `ProgressHandle(Arc<AtomicU8>)` is obtained before phase 2; §1 is corrected; bootstrap is split in two. | The handle is shared with the `Claim` and read after `catch_unwind` on the owner's thread (lines 264–275). `bootstrap_ready` has a 6 s clock and `BootstrapLease::finish` an 11 s clock. | Closed |
| P3-8 | Phase 1 returns a typed `AlreadyRegistered`, which reports `true`. | A phase-1 duplicate now retains `true` (A5; §5.1 line 121; guide line 63). | Closed. The synchronous live-claim variant is R2-C-P3-2. |
| P3-9 | The generation fault rule, A6 and `TransportGenerationBusy`. | After a post-mutation cancel (A7), the generation check sets `faulted`, and the next attempt refuses TransportGenerationBusy. A close nobody observed is caught at phase 1 (A6). | Closed. It is masked for identity-option callers; see R2-C-P3-1. |
| P3-10 | A `threading.Lock` and a `concurrent.futures.Future`, awaited through `wrap_future` (§8 line 393). | No asyncio primitive is bound across loops any more, and test 2 adds a two-loop clause. | Closed |
| P3-11 | V4 §1 (line 17) names the supersession of GwzPyTransportDesign §§2–3, and a pointer is added. | Both documents now carry the trail (pointer lines 294–302), and the §4 same-generation rule is kept. | Closed |

## Changed-range analysis

**Commits between the two tuples.**
- Root commits between the tuples:
  - `9d846c2` records the round-1 reports, the verdict and the remediation plan. It is not part of the object.
  - `1135835` rewrites the V4 design (510 lines, up from 393) and updates the lock pins.
- gwz-core `d43b147` changes one paragraph each in GWZDesign and GWZRequirements.
- gwz-py `0ffc4cb` rewrites the guide's request-ID section, table and example, and adds the pointer to GwzPyTransportDesign.
- No source file changed in any member. gwz-transport stays at `36ae2b1`. The V3 and v2 documents are unchanged.

**Mapping to the remediation plan.** I mapped every change in the V4 diff to a plan row: B1–B7, the nonblocking rows, and the adopted Safety and Consistency residuals.

One item goes slightly beyond the plan's text: `cancel()` shielding its native join against a repeated task cancellation (§8 line 393; §11 item 9; guide line 38). It directly supports P3-1's "cancelled and joined before release" disposition, so I do not treat it as outside the plan.

**New defects.** The new material that carries them is:
- the local-submitted-operation lifecycle (§7.6 rule 3, L-rows, §11 item 10) — R2-C-P2-1;
- the preflight's new refusals — R2-C-P3-1;
- the guide's refusal table — R2-C-P3-2;
- the stalled-clock clauses of test 4 — R2-C-P3-3;
- the regenerated row IDs and tests — R2-C-P3-4.

**No new architectural root cause.** The ownership ladder, transfers, intents, owner clocks and generation fault rule all held. The local lifecycle itself is fully owned (§3.2 line 51; L1–L6). Its defect is the way it is filed against v2 and the caller guide.

## 0. Evidence base

**Commands.** All read-only:
- `git rev-parse HEAD` in the root, gwz-core, gwz-py and gwz-transport, at start and end. They returned the tuple above, with gwz-transport at `36ae2b1`.
- Path-limited `git status --short` over the reviewed documents, `gwz-core/src`, `gwz-py/native/src`, `gwz-py/src/gwz` and `gwz-transport/src/mux` returned nothing.
- `git log --oneline fbee49c..1135835`, and `git show --stat` of `9d846c2` and `1135835`.
- Member logs and `git diff --stat` across the member ranges showed documentation-only changes.
- Full `git diff` of the core paragraphs, the guide and the pointer.
- `git show 11358352:gwz.conf/gwz.lock.yml`.

**Documents read.**
- The revised V4, lines 1–510, in full.
- The round-1 V4 at `fbee49c`, for comparison.
- `-Verdict.md` and `-RemPlan.md` at `1135835`.
- The v2 contract, lines 1–61.
- The revised guide, lines 1–110.
- GwzPyTransportDesign lines 1–13, 49–234 and 266–308.
- The core v2 and v4 paragraphs.

**Source re-read for the changed claims.** gwz-core `d43b147`:
- `session/driver.rs` 140–242, including the `reply.get()` and `result.get()` waits, and 470–517.
- `session.rs` 449–679 and 766–898.
- `request.rs` 328–367.
- `transport_host/mod.rs` 47–51 and 185–269.
- `local_command.rs` 33–46.
- `model/mod.rs`, the `ErrorCode` enum.
- `protocol/convert.rs` 8–91: bridge-only codes map to `IoError`, so adding `TransportGenerationBusy` leaves Taut untouched.

gwz-py `0ffc4cb`:
- `native/src/dispatch/mod.rs` 101–175, whose submitted local methods are `merge` and `clone_local_workspace`.
- `src/gwz/client.py` 349–391, 540–548, 968–1039 and 1325–1346.
- `bridge.py` 385–416.

## 1. Findings

### [R2-C-P2-1] Local submitted operations narrow v2's close, cancel and bound guarantees under a "clarification", while the caller guide still promises the v2 behavior

**Location.**
- V4 §7.6 rule 3 (line 365), §3.4 line 74, L1–L6 (lines 176–181), §9 line 399, and §11 item 10 (line 420) together with the §11 preamble (line 409): "states behavior that v2 leaves unstated in items 9–11. Nothing else in v2 §§2–8 changes".
- §14 line 510: "a local handler may still be writing the workspace after `close()` returns".
- v2 §5 line 35: at most 8 top-level operations and 8 top-level native workers per session.
- v2 §6 line 43: the session owns "merge-response" records.
- v2 §6 line 45: close "cancels live operations, joins their finish paths", and gives summaries "for every operation live when close began".
- Guide line 108: close "cancels and joins every active operation".
- Source: `client.py:998-1039` and `1325-1346`. `merge_stream` returns a `MergeOperationHandle` that has `events()` and `result()` but no `release()`.

**Violated invariant.** The exactness of §11, and agreement between the caller guide and V4.

**Reproduction.**
1. Call `h = await client.merge_stream(...)` on a native Client.
2. While the merge handler is still writing, call `await client.close()`.
3. Under V4 §7.6 and §9, close neither cancels nor joins the merge and lists no summary for it. Close returns while the handler may still be writing — V4 §14 says exactly this.
4. The guide (line 108) and v2 §6 promise the opposite.

A caller who removes or reuses the workspace after close, as the guide allows, races the merge.

The same new model also departs from v2 in four other places:
- A local submitted operation takes no slot and spawns its own native worker, so a Client can exceed v2 §5's limit of eight top-level operations and workers.
- `cancel_operation(id)` on its ID returns `UnsupportedOperation`, which v2 §6 does not provide for an issued record.
- Because the merge helpers "release the record after reading its final response", a second `MergeOperationHandle.result()`, or `events()` after `result()`, now fails with `OperationExpired`.
- A handle whose result is never read holds one of the 64 records it shares with network admission, for 15 minutes.

**Impact.** Guarantees in the accepted contract are narrowed for session records without an amendment, and the caller guide gives a false promise of quiescence together with a workspace hazard that V4 itself names. V4 does count these operations toward v2 §6's 64-record bound, which shows they are treated as v2 session operations.

**Remedy.** Choose one of two options:
- Move item 10 into the amendments, citing the exact v2 §5 and §6 clauses it narrows (the bound, close cancel and join, summaries, and cancel semantics), with the rationale. Then update the guide's close paragraph and the merge API description.
- Or make close wait for local submitted operations to settle.

Either way, define `MergeOperationHandle`'s record lifecycle.

**Closure test.**
- Submit a merge that blocks, then call close. Close either waits, or returns with the merge still running and the guide says so.
- `cancel_operation` and a repeated `result()` behave as documented.
- 64 unread merge handles followed by a network admission behave as documented.

**Classification.** Bounded contract/text correction; not a new architectural root cause.

### [R2-C-P3-1] The client's capability-preflight wrapper re-types V4's new preflight refusals

**Location.** V4 §7.6 rule 1 (line 363), §3.6 line 90, §11 item 7 (line 417) and I8 (line 101). Guide line 100 and the refusal table (lines 57–65). Source: `client.py:349-372`, which V4 leaves unchanged; §1 lists `bridge.py` only.

**What goes wrong.**
- `_require_transport_capability` turns any `GwzBridgeError` raised by the preflight into `UnsupportedOperation` with the message "update the core".
- Under V4 the preflight refuses with `TransportGenerationBusy` on a faulted Client, and returns typed construction errors — including an `IoError` for a bootstrap failure that §12 line 442 says succeeds "on a later construction".
- Identity-option network calls therefore see "update the core". The guide instead promises that every later network call is refused with `TransportGenerationBusy`.
- The guide's table says an `UnsupportedOperation` fails the same way when retried unchanged, so a transient failure reads as permanent.

**Remedy.** State in §7.6 or §8 that these refusals propagate unchanged, and narrow the wrapper to genuine absence of the capability.

**Closure.** A faulted Client with an identity-option fetch refuses `TransportGenerationBusy`. An injected preflight bootstrap failure surfaces as `IoError`, and a later call is admitted.

### [R2-C-P3-2] The guide's refusal table misstates the slot-full and live-claim refusals

**Location.** Guide lines 60 and 63. V4 §12 line 434 (S6: "after a slot or ledger space frees"), S2 line 140, §5.1 line 129 and §11 item 2.

**What goes wrong.**
- **(a) Slot-full.** The table clears `TransportSessionFull` only by releasing records or waiting for expiry. A ninth operation refused because all eight slots are held clears only when a live operation finishes. Live records refuse release with `OpenOperation` and do not expire.
- **(b) Live claim.** V4 never defines `request_id_consumed` for S2's synchronous live-claim refusal, which has no record. The table gives `True` and "never in this generation". But a live holder that is cancelled while still `Issued` and then released never registered the ID, and the ID is reusable. Applied to that holder, the §5.1 definition gives `false`.

**Remedy.** Add the slot case, define S2's value in V4, and align the table with it.

**Closure.** Each table row cites the §12 or S2 row it restates, with matching values.

### [R2-C-P3-3] Owner-clock wins are attributed to a "dead" supervisor, and test 4 omits the setup that reaches them

**Location.** V4 §3.5 line 80 ("an owner clock wins only when the supervisor is dead"); §12 lines 463 (C6, "dead supervisor"), 466 and 450 (A8, "stalled"); §14 line 508; §6.3 line 277; §13 test 4 (line 488). Source: `driver.rs:196-199` (`reply.get()` has no clock; its deadline is enforced by `drive()`), `session.rs:449-583`, `766-808` and `746-757`.

**What goes wrong.** With §6.3's hardening, an exited supervisor closes its session and completes sealed registrations, so finish returns first with C4 or C5 (line 466). Owner clocks win only against a supervisor that is stalled without holding core's mutex — the state test 4 creates — yet §3.5, C6 and §14 call it "dead".

Test 4's clause "With the clock still stalled, the phase-1 clock wins at 6 s and the finish clock at 11 s" also needs three unstated steps:
- **(i)** Stall after construction. The lease's `finish()` needs `drive()`, so construction cannot succeed while the clock is stalled.
- **(ii)** Hold an admission or capacity gate. An uncontended phase 1 at equal capacity completes on its first poll.
- **(iii)** Have the handler return without core I/O, for example after a cancel, which completes reply waiters (`session.rs:746-757`). Otherwise an accepted network handler blocks forever in `reply.get()`.

**Remedy.** Name the state "stalled (not driving, not holding core's mutex)" in §3.5, §12 and §14, and add steps (i)–(iii) to test 4.

**Closure.** Run as written, test 4 reaches both owner-clock branches.

### [R2-C-P3-4] Mechanical defects in the regenerated §5.2, §12 and §13

**Instances.**
- **Colliding IDs.** The §5.2 row IDs T1–T3 (lines 173–175) reuse the transfer names T1–T3 from §3.3. §12 cites "T2" and "T3" for its release rows (lines 470–471), while its C1 row reads "T3 send fails" (line 457), and S5's event is "T2 `claim`".
- **Incomplete error types.** A3 (line 152) types construction failures as "IoError or InternalError", yet §12 line 441 cites A3 for `InvalidRequest`. That is the HOME-unset case: `SshEndpointConfig::from_environment` returns `invalid(...)` (`mod.rs:47-51`).
- **Unbuildable test.** Test 7 (line 491) closes "a Client holding 64 issued handles and 8 live operations". That is 72 records against the 64-record limit (S1/S2). It copies the wording of my own round-1 P3-2; the intended case is 56 issued and 8 live.

**Impact.** Citations are ambiguous, and one closure test cannot be set up.

**Remedy.** Rename the terminal rows (for example R1–R3). Type A3 as "typed construction error". Use 56 + 8 in test 7.

**Closure.** A citation check passes, and test 7 builds within 64 records.

## 2. Invariant analysis

**Ownership.**
- Every §5.2 row is one of: an owner step; T1–T4; a refused transfer settled on the sender's behalf (§3.3 line 59); an intent; or an L-row mint whose record is born owned (§3.2 line 51).
- Every §12 row cites its §5.2 row, apart from the R2-C-P3-4 defects.
- The `Construction` value covers the construction interval, including preflight construction (§3.6). Its drop guard gives waiting claims and close a bounded exit (§9 line 399).
- The generation fault rule gives every close other than `close()` one explicit caller signal. A6 catches closes nobody observed.

**Source claims that held.**
- The sequential finish and shutdown compositions (`request.rs:328-339`; `mod.rs:263-266`).
- The retained core closes (`driver.rs:498-501`; mux `mod.rs:536-540`, `542-595`).
- Capacity failures that close the generation (`session.rs:622-626`, `663-672`).
- The mux bound of 1–4096, which allows 257 (mux `182-183`).
- `generation_open()` is computable from `is_closed()` and the mux phase.
- Uncontended admission completes synchronously.

**Paired core paragraphs.** They now agree with V4 §6 and with each other: two-step bootstrap, a random reserved ID, 257 registrations only for session runtimes, `AdmitRefusal`, `ProgressHandle`, the retained deadlines, the caller-clock scope and the hardening. The v2 paragraphs remain compatible.

**§11 exactness.**
- Items 1–8 each amend real v2 text.
- Items 9 and 11 are genuine clarifications.
- Item 10 is not a clarification (R2-C-P2-1).

**Caller guide.** Its definition of `request_id_consumed`, the `GwzOperationCancelled` hierarchy and attributes, the observer rule, the implicit-release rule, and the `TransportGenerationBusy` action all match V4. The exceptions are R2-C-P3-1, R2-C-P3-2 and R2-C-P2-1.

**Pointer.** It is consistent with V4 §1.

## 3. Risks and next action

**Residual risks below the bar.**
- §11 item 9 and §8's observer rule are not scoped to handles. v2 §2 makes cancelling a stream helper's task cancel and join the operation, so `OperationStream.result()` needs an explicit rule in the handle stage.
- §14's construction bound of "up to 6 s plus 11 s" leaves out the failure-path shutdown, which can bring it to 28 s.
- §3.6 says "four ways" but lists three.
- The owner's deliberate generation close in A11 and A14 has no core API declared in §6.
- Core's `ErrorCode::Cancelled` is documented as "cancelled before core registration", yet A12 uses it after registration.
- If the first of several concurrent `accepted()` callers is cancelled, the shared future must be completed with the terminal, not with `GwzOperationCancelled`. Otherwise co-waiters that were not cancelled receive a `CancelledError`.

**Next action.** Make one bounded revision resolving R2-C-P2-1, with the four P3s alongside, committed as a new exact tuple for a focused re-review. That is remediation round 2 of 2. No finding here triggers the §4.1 stop rule.
