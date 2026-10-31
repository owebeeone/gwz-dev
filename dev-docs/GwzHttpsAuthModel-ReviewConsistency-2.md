# HTTPS authentication model (DRAFT r3) — Consistency-AXIS RE-VERDICT, round 2
**Review object:** dev-docs/GwzHttpsAuthModel-DRAFT.md revision 3 (593 lines) at root 5de7d71b, with its change map and the same commit's restatement of `GwzPhase1-RemPlan.md` R3/R4. Code is at gwz-core a7b965c5 and gwz-transport 0a253639.
**Baseline:**
- I read every root document with `git show 5de7d71b:<path>`, and code with `git -C <member> show <sha>:<path>`.
- `git diff 5de7d71b HEAD -- dev-docs/GwzHttpsAuthModel-DRAFT.md` was empty at start and at end (HEAD was 5de7d71b both times).
- `GwzConnectionApiContract.md`, `GwzTransportReleaseConvergencePlan.md` and `GwzSplitCommandChannel.md` are byte-identical between 0a0545a3 and 5de7d71b, so r3's line citations against "root HEAD" hold.
- New input this round: `GwzHttpsAuthModel-RemPlan.md` (the merged round-1 remediation plan).
- I made no writes, ran no builds and used no model.
**Date:** 2026-10-31
**Axis:** Consistency, round 2: r3 against its controlling graph and against the round-1 Consistency findings. Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: NO-GO** — 0 P0, 0 P1, 1 P2 (new), 9 P3 (new). Every round-1 Consistency ID is closed; three have residuals, tracked as new P3s. I pre-commit to GO on a revision that:
- resolves N-P2-1 by adopting U7's proposal, with its row;
- dispositions N-P3-1 to N-P3-9 (fixed in text, or recorded with an owner and a closure test).

## 0. Prior-finding closure table

| Round-1 ID | r3 location | Check performed | Status |
|---|---|---|---|
| P2-1 (`Effect` of the re-authentication GET) | §3.1 "Effect"; A7; table (d) RA2-RA7; rows H3b-3, H3c-3 | A7 is anchored on the §9 paragraph L660-666. It states that `Effect` describes the exchange's own request, that the POST is handed to the engine only after the GET is drained, that a re-authentication failure is `Effect::None`, and that the stale-idle replacement does not apply. P6 keeps `PossiblySent` after a POST was actually sent. A11 lists §9. | **Closed** |
| P2-2 (rejection wipe) | §3(b) "Rejection" and "Zeroization"; A3; G6, P2; rows H1a-4..6, H1c-2..5 | Eager under the lock. Headers are built under the lock and returned as `SecretHeader`, so no `Arc<Secret>` leaves the lock. H1a-4 asserts zero before anything else runs. Scoped per origin; HMAC fingerprint. `getrandom` and `sha2` are existing gwz-core dependencies (`Cargo.toml:102`, `:104`). A3's "wipes the credential at once" now matches the design. | **Closed** |
| P2-3 (overflow silently drops) | §3(b) bounds table; G14, P8; H1d-1..5; A3, A8 | The advertisement ends with the budget code before any body byte, and no POST follows. Rows assert this. 16 KiB per credential, 256 KiB total. The bounds are in §10 (A8). The choice of code has a residual: N-P3-6. | **Closed** (residual N-P3-6) |
| P3-1 (superseded-clause list) | A1-A12 | (a) A5 wording as required. (b) A6 names L563-565 "requested by the existing protocol profile/policy", and §2 reconciles the probe POST. (c) A11 marks L881's POST-rule and "Open for Phase 3" sentences. (d) A12 replaces plan L724-728. (e) A8 adds the §10 rows, and A12 adds the plan §10 entry D-V. (f) A2 adds the not-a-framework sentence. (g) A1 restores "opaque" and says "own or retain". | **Closed** (wording residual N-P3-8) |
| P3-2 (anchors, citations) | §4.0; §3(a); §1-3 | Anchors are paragraph-based. A3's anchor is the closing words "...generic reuse is allowed." (L559), which I verified. The order is stated (A3, then A4). The key is defined via `destination.request(service)` (`https_destination.rs:116-134`, `pop_if_empty` at `:134`, verified). `exchange.rs:123`, L882 and "the only request of a different service" are all corrected. | **Closed** |
| P3-3 (file lists, pin order) | §5 table and Order; §3.2 item 2; RemPlan R3/R4 | Verified: H1b lists `auth.rs`; `prior` stays (W4); `connection_bound` goes through a `Pipe` flag, with no `Settle` change; H2b lists `exchange.rs` and leaves `Instance::settle` unchanged (`LeaseId::connection()` exists, `pool/mod.rs:303-310`); pin order is stated. Residual gaps: H1e misses `admission.rs`/`binding.rs`; H3a misses its `mod` line; R3's landed row lacks `https_adapter/auth.rs`; the pin order conflicts with the 2.3 plan. | **Closed** (residuals N-P3-1, N-P3-2) |
| P3-4 (hint recorded at settlement) | §3.1 "Placement"; A10; H2a-5..7, H2b-1..3, H3a-3 | Affinity is sampled at the first delivered byte and again in `Exchange::finish`. `prefer_wait_ms` is set only when the instance's `settling` list holds the connection. A busy connection is not waited for. Rows include the no-wait-for-idle order, 100 of 100. The gwz-git order stays a labelled hypothesis (§6 Risks). | **Closed** |
| P3-5 (`Need` transitions) | §3(d) | One table, G1-G15, P1-P8, RA1-RA7. Row H1b-5 is fixed (it is now H1b-5 and H1c-2). Residuals: `Exhausted` is not in the table (N-P3-5); RA2 contradicts the table's own rule (N-P2-1). | **Closed** (residuals N-P2-1, N-P3-5) |
| P3-6 (`closed` flag, clearing `Drop`) | §3(b) "Lifetime and holders" | Both removed, with the reason. The holder table matches the code: `binding.rs:175`, `instance.rs:76-77`, `ports.rs:142-143`; HTTPS `connect` consumes the command (`worker.rs:205-235`); `NativeRetained` probes hold a supervisor and a record id only (`native/sspi.rs:147-155`); `NativeCaller` is `Clone` (`sspi.rs:6-7`). | **Closed** |
| P3-7 (scope provenance, review structure) | Task boundary; §5 Review; RemPlan R4 restated | R4 is now the H1a-H3d group, and the review follows the RemPlan (this document's dual, plus round 2). The draft's own authority text is stale against the same commit. | **Closed** (residual N-P3-3) |
| P3-8 (fallback counter) | §3.1 "Counter"; H3d-4 | A saturating counter with a row. Its route to the sweep is U5, and it is unrecorded in the plan. | **Closed** in step (residual N-P3-9) |
| P3-9 (H1b size) | §5 | Re-estimated from the real rows: 31-61 lines each, about 40 on average. All eleven sub-steps are 500 lines or fewer; my recomputation gives 3,620 added and 1,070 product. Split contingency for H1a and H1b. | **Closed** |
| P3-10 ((a) per-capture premise, (b) served `cli`, (c) gwz-py) | §3(b) single admission and H1e; §3(f); A12 | (a) becomes mechanical through H1e. (b) admission coverage and `Opened.facts` are stated. (c) one record per call, and the tainting, are stated. The A12 wording over-constrains 3.1: N-P3-7. | **Closed** (residual N-P3-7) |
| P3-11 (deadline) | §3.1 "Deadline"; H3c-4 | Minted like `auth.rs:48-52`, refusing at 0. §10 L682 is identified as setup-only. | **Closed** |

## 1. Changed-range analysis (r2, 399 lines, to r3, 593 lines; +690/−354)

- **Header and change map (L1-43).** New. The map rows trace to RemPlan rows. The task boundary quotes the pre-amendment R4 text, which the same commit replaced (N-P3-3).
- **§1-§2 (L47-116).** Citations corrected. The probe-POST paragraph is new and is consistent with A6.
- **§3(a)-(h) (L118-285).**
  - Rewritten: the key, `Need` with `Unretained`, the bounds table, the charge formula, single admission, the holder table, eager rejection, the transition table, scope-record rules W1-W4, and placements.
  - Introduced here: N-P2-1 (RA2), N-P3-4 (single-admission wording, in A8 against 3(b)), N-P3-5 (`Exhausted`) and N-P3-6 (the budget code).
- **§3.1 (L287-352).** Learning extended (G2). The `Pipe` flag, the placement wait, the re-authentication conditions 1-5, the effect, the deadline, the scope after re-authentication, and the counter are new. Consistent with A4, A7 and A10, apart from the wording of N-P3-8.
- **§3.2 (L354-368).** R3 is named the single owner of the caller move; the caller identity goes into the scope. The landed R3 row omits one file (N-P3-1).
- **§4 (L370-443).** A1-A13 replace 4.1-4.7. The anchors verify. N-P3-4, N-P3-6, N-P3-7 and N-P3-8 come from here.
- **§5 (L445-525).** File lists by edit, sizes, order and pin, 47 rows. N-P3-1 and N-P3-2 come from here.
- **§6 (L527-593).** Decisions unchanged. U1-U7 are new; U1, U2 and U6 are stale against the commit (N-P3-3).

**NEW architectural root cause: none.**
- H1e's single-admission claim adds a gwz-connection concept: an opt-in claim on `AuthContext`, plus `AdmissionError::InvalidInput`, which is missing at 0a253639 (`operation.rs:410-430`, `#[non_exhaustive]`, so the addition is non-breaking).
- `Request.prefer` and `prefer_wait_ms` are additive public fields. gwz-core builds `Request` only through `Request::new` (`https_pool.rs:255`), so no struct literal breaks.
- All of these are bounded additions behind existing ports. None is the root cause of a defect.

## 2. Findings

### N-P2-1 — RA2 turns an unchallenged 200 into a terminal `Rejected`, which contradicts the table's own "no verdict" rule and breaks a working public-repository case

- **Location:** draft §3(d) L218-219 ("a 403, 404, 5xx or 407 is no verdict on authentication") against row RA2 L247 ("re-authentication GET gets 200 unchallenged | `ConnectionBound` | `Rejected` | ends `Authentication`"). Also G2 L224, §3.1 "Learning" L294-298, A4 L417-426, and U7 L589-593.
- **Root cause:** G2 learns `ConnectionBound` from an unchallenged GET on a `LogonSession` connection. An authenticated connection is never challenged, so G2 cannot tell a public key from a private one. RA2 then treats the one piece of evidence that separates them, a 200 without a challenge on an unauthenticated connection, as a failed login.
- **Violated invariants:**
  - The table's header rule: only a 401 to a credentialed request is a verdict.
  - A3's definition: rejection follows "a 401 to a request that carried a credential".
  - The rationale for terminal `Rejected` (L254: "repeated failed logins can lock an account"). A 200 is no login attempt.
  - The draft's own principle (3(a), from p1c Code P2-1): a request the server serves to anyone must not depend on authentication.
  - Plan §1: Windows parity.
- **Sequence:**
  1. A Windows operation fetches private `/a` and public `/pub` from one NTLM origin.
  2. `/a` authenticates c1 (G8). `/pub`'s GET lands on c1, unchallenged, and G2 records `ConnectionBound`.
  3. `/pub`'s upload-pack POST misses the hint (c1 is busy) and lands on c2 (P5).
  4. The re-authentication GET gets 200 without a challenge, so RA2 marks the key `Rejected` and the exchange ends `Authentication`.
  5. Every later exchange of `/pub` in the operation fails before any byte (P7, G11).
- **Impact:**
  - Today's legacy route keeps the lease from discovery to POST, and the host route sends `/pub`'s POST anonymously; both work.
  - r3 fails the fetch of a public member on a hint miss.
  - A fail-closed regression on Windows, on the very path decision 2 targets.
- **Required correction:** adopt U7.
  - `ConnectionBound` keeps the evidence of how it was learned (`challenged: bool`).
  - RA2 on an entry learned unchallenged sends this POST anonymously, as an unrecorded key (P3), and leaves the entry unchanged. A 401 to that anonymous POST ends `Authentication` with `PossiblySent`, exactly as P3.
  - RA2 on an entry learned through a challenge stays `Rejected`, as Safety asked.
  - Update A4's sentence "A re-authentication that does not authenticate the connection ends the exchange before any POST byte" to match.
- **Closure test:** a row on the H3 NTLM fixture with `/pub` served anonymously:
  - `/a` authenticates c1, `/pub`'s GET lands on c1, and `/pub`'s POST is forced onto c2;
  - one re-authentication GET (200, unchallenged), then one anonymous POST that succeeds;
  - the entry is unchanged, and the next `/pub` exchange works.
  - H3b-4 is re-scoped to a challenged-evidence key.

### N-P3-1 — Residual file-ownership gaps

- **Location:** §5 table rows H1e and H3a; §3.2 item 2 against the landed RemPlan R3 row.
- **Root cause:** a few edits are not traced to their files.
- **Items:**
  - **(a) H1e.** Says the "claim travels admission, permit, binding beside the `ChargeGuard`" and lists `crates/gwz-connection/src/{operation,host}.rs`. The `ChargeGuard` is carried by `Admission`/`LocalAdmission`/`LocalPermit` in `admission.rs` (`:41`, `:70`, `:79`) and by `BindingShared._charge` in `binding.rs:188`. Freeing the claim on a dropped admission happens in `admission.rs`. Neither file is listed.
  - **(b) H3a.** Creates `connection_host/https_bound_tests.rs` but omits the `mod` line in `transport_host/connection_host.rs` (`:364-368`), which H1b does list for its own new file.
  - **(c) R3.** §3.2 item 2 says R3 "switch[es] `auth.rs:42` to the context's caller", and A13 adds `https_adapter/{exchange,auth}.rs` to R3. The landed R3 row adds only `https_adapter.rs`/`https_adapter/exchange.rs` and `https_auth.rs`. `https_adapter/auth.rs` is therefore owned by R4 (`https_adapter*`), and R3 must edit it: the round-1 P3-3(d) double ownership survives for exactly this file.
- **Closure test:** H1e lists `admission.rs` and `binding.rs`; H3a lists `connection_host.rs` (mod line); the RemPlan R3 row lists `https_adapter/auth.rs`, as A13 states.

### N-P3-2 — The pin order conflicts with the step 2.3 plan and omits R1b

- **Location:** draft §5 L470-471 ("pin order: R1 first, then R2b where it moves it, then this pin, then 2.3b"), against `GwzServerCore-Step2.3-Plan.md` L94-97 (2.3b owns `.github/gwz-transport.commit` and `process_globals_allowlist_gwz_transport.json` `reconciled_commit`) and L231 (2.3b is in wave 1). Compare RemPlan L68-73 (2.3a runs beside 1.8 wave 1).
- **Root cause:** the order was written for 1.8 alone, not against the 2.3 plan's waves.
- **Impact:** read literally, it holds 2.3b, a wave-1 step of 2.3, behind 1.8's wave 4, a dependency recorded in neither plan. It also omits R1b, the gwz-transport `src/pool/*` step that H2a follows, and the allowlist `reconciled_commit` that a pin bump must move ("the file's bump rule").
- **Closure test:** state the rule, not a fixed order: each lane that moves the pin rebases on the latest pushed gwz-transport commit and moves the pin and `reconciled_commit` together. Or record the 2.3b wait in the 2.3 plan. Name R1b in the order.

### N-P3-3 — The draft's authority text and its U1, U2 and U6 are stale against the commit that carries them

- **Location:**
  - Task boundary L37-41. It quotes R4's old Owns text and says the (+) items "are proposals until the lane owner amends R4".
  - U1 (L575-576), U2 (L577-581, "the RemPlan took neither") and U6 (L588).
  - All of these against the R4 and R3 rows landed at 5de7d71b, which already list H1e, H2a, H2b, gwz-connection and the R3 caller files.
- **Second defect:** the landed R4 row lists the contract sections as "§4.1/§7.2/§9/§10/§15", but A1 and A2 edit §7 (the table and L483-487), A10 edits §8 and A9 edits §11.
- **Impact:** the reviewers' controlling record says the scope is unresolved in one file and resolved in the other. U2 still reads as an open operator question, while R4 has adopted H1e.
- **Closure test:**
  - The task boundary cites the amended R4 row, and the (+) markers are removed or re-pointed.
  - U1 and U6 are marked closed by 5de7d71b. U2 records the choice (H1e) and its authority.
  - The R4 row lists §7, §8 and §11.

### N-P3-4 — A8's single-admission sentence contradicts 3(b) and would put a fallible check inside an infallible `register`

- **Location:** A8 L384 ("once one admission registered, a second admission of any clone ... is refused `InvalidInput` before registration") against §3(b) L169-171 ("`admit` takes [the claim], frees if the admission never registers ... consumes for good once it registers") and contract §5 L223-224 and L238 (`register` "is synchronous, non-cancellable ... performs no fallible allocation"; "The infallible register signature is retained").
- **Root cause:** the contract text keys the refusal on registration; the design keys it on `admit`.
- **Sequence:** two clones are admitted concurrently, and neither has registered. Under A8's wording both pass `admit`, and only `register` could refuse the second, which §5 forbids. Under 3(b), the second `admit` is refused at once.
- **Impact:** an implementer following the contract text either breaks §5 or admits a second binding sharing the record.
- **Closure test:** A8 reads "while one admission holds or has consumed the claim, `admit` of any clone is refused `InvalidInput`", and H1e gains a row with two concurrent admissions, neither registered.

### N-P3-5 — `Exhausted` is outside the "one transition table"

- **Location:** §3(b) bounds table L159 ("the record becomes `Exhausted` ... a poisoned lock does the same after wiping and clearing"), and L201 (randomness failure gives `Exhausted` from the start), against §3(d) L216-252, which has no `Exhausted` state or row. Rows H1a-7 and H1d-5 test it.
- **Root cause:** a record-level state was added to the bounds table, not to the transition table.
- **Impact:** the table does not say what an `Exhausted` (or poisoned-lock) record does to keys already `Basic` (P1: still served from the record? "wiping and clearing" suggests no) or `ConnectionBound` (P4, P5). The behaviour under randomness failure is undefined in the same way.
- **Closure test:** §3(d) gains `Exhausted` rows for G4, G5, P1, P4 and P5, with what is sent, and H1d-5 asserts one of them.

### N-P3-6 — The budget refusal's surface is stated three ways, and the chosen code already means something else

- **Location:**
  - §3(b) L161-163: "`ErrorCode::Capacity` (wire 5) ... reported in the contract's `InputBudget` class (§11 L726); `stream_error` ... gets an arm".
  - A9 L385: "reaches an opening or a stream as `InputBudget`".
- **Code facts:**
  - `Capacity` is wire 5 (`protocol.rs:261-266`), verified.
  - Locally, an adapter failure reaches the client only as `StreamError::Io(io::Error)` with a kind chosen by `stream_error` (`https_adapter.rs:359-370`; `exchange.rs:532-548`). Neither `StreamError` nor `OpenFailure` has an `InputBudget` variant (`operation.rs:459-482`).
  - Under `cli` the same failure is `StreamError::Failed { code: Capacity }`.
  - gwz-core already gives a detail-less `Capacity` a meaning: "A `Capacity` failure without one (the pool's own limit) carries none" (`setup_retry/limit/capacity.rs:181-183`).
- **Root cause:** the budget refusal is not specified end to end.
- **Impact:**
  - A9 promises a contract-visible `InputBudget` that no file list delivers.
  - The `io::ErrorKind` of the new arm is unnamed.
  - The user may read the record overflow as the pool's connection limit. That is better than `Authentication`, but it is still the wrong diagnosis.
- **Closure test:**
  - Name the `io::ErrorKind` (for example `QuotaExceeded`) and the `cli` form.
  - Either give the `Capacity` failure a detail cause that names the credential record (an addition to `CapacityCause` or `FailureDetail`), or state in A9 that a detail-less `Capacity` from the HTTPS adapter means the operation's credential record.
  - Row H1d-1 asserts the kind and the absence of a pool-limit wording.

### N-P3-7 — A12's 3.1 text over-constrains plan 3.1

- **Location:** A12 L439-441 ("the composition captures per command, admits each capture once ... and drops it with the operation"), against plan 3.1 L721-723 ("builds one `HostOwner` per command", silent on operations per command) and contract §7 L470 (capture "once on the original entry", per operation).
- **Root cause:** "per command" merges the capture unit with the command unit.
- **Impact:** together with single admission, the text forces 3.1 to one operation per command, a 3.1 design decision made here without an operator decision. A command that needs two operations cannot satisfy both "per command" and "admits each capture once".
- **Closure test:** A12 reads "captures once per admitted operation, on the original entry, admits that capture once and drops it with the operation".

### N-P3-8 — A6 and A10 wording

- **Location:** A6 L382 and A10 L386.
- **Root cause:** each amendment's wording was not checked against its own section's terms.
- **(a) A6.** "Except the re-authentication ... which the adapter makes on its own; this crate guesses Git discovery idempotence nowhere else" attributes the re-authentication to "this crate" (gwz-connection, contract §3). It is made by the gwz-core HTTPS adapter. Say "except the gwz-core HTTPS adapter's re-authentication of a connection-bound key".
- **(b) A10's §8 sentence.** It omits "the owner not full", which §3.1 L315 and `allocation.rs:19` (`owner_full`) require, so the contract permits leasing past an operation's limit.
- **Closure test:** both sentences are corrected on re-read.

### N-P3-9 — The busy-wait trigger still has no production reader

- **Location:** §3.1 "Counter" L350-352 and §6 item 2 L541-542 ("its trigger is the re-authentication count") against U5 (L587, "3.1/3.4 composition work, not this step's") and A12, which records nothing for 3.4.
- **Root cause:** the counter's consumer is deferred to steps whose plan text this step does not amend.
- **Impact:** `HttpsAdapter::counters()` is reachable only by a test holding the adapter's handle. The `dyn EndpointAdapter` inside the instance exposes no counters. Without a plan-recorded route, the 3.4 sweep cannot fire the trigger, which was round-1 P3-8's concern.
- **Closure test:** A12 adds a 3.4 sentence: "the sweep reads the HTTPS re-authentication count (route: `<owner>`)". Or the counter is exposed the way `scopes()` is, with a composition row.

## 3. Judgement on the §6 "Unresolved" items and the RemPlan amendment

- **U1, R4's size.** Resolved by the R4 row landed at 5de7d71b: the eleven-sub-step group, each 500 lines or fewer, which matches RemPlan L55 per sub-step. Close U1 in the text (N-P3-3).
- **U2, single admission.** H1e is the right choice:
  - it makes A8 and 3(b) mechanical;
  - its surface is additive (`#[non_exhaustive]` `AdmissionError`, an opt-in claim);
  - option (B) leaves R4's isolation to discipline, and the Weak-guard alternative re-plumbs every holder.
  - Its authority is now the R4 row, which the operator's "fix per the reviewed draft" (RemPlan L35) covers once this document is accepted.
  - Fix A8's wording (N-P3-4) and H1e's file list (N-P3-1).
- **U3, clones and captures.** Agree. A12 must say "per admitted operation" (N-P3-7). Plan 3.2's "never caches a capture across calls" is right and matches gwz-py today.
- **U4, H3c-2 wording.** Agree with r3. After P6 the first POST's body has been sent (`PossiblySent`), so the zero-byte assertion belongs to the next POST of the key. The RemPlan's closure text should be read that way. No change.
- **U5, the counter's route.** The deferral is acceptable, but the plan must record it, or the busy-wait trigger is unmeasurable (N-P3-9).
- **U6, R3's row.** Done at 5de7d71b, except `https_adapter/auth.rs` (N-P3-1).
- **U7, a public key learned on a bound connection.** Adopt it now. It is N-P2-1. A 200 without a challenge is not an authentication verdict under the draft's own rule, and the fix is small (one flag, a row, about +30 lines).
- **RemPlan R3/R4 amendment.**
  - The R3 single owner and the caller identity in the scope match §3.2.
  - R4's files match §5, except the contract-section list (§7, §8 and §11 missing) and `connection_host.rs` mod lines (outside "connection_host/https* tests").
  - Dependencies: R4 depends on R3 and the re-verdict. The draft's further dependencies on R2a and R2b are implied by the waves. Good enough.

## 4. Risks

- **Highest.** N-P2-1 ships a Windows-only, fail-closed regression on exactly the path decision 2 exists for. It shows up only on a hint miss, so fixture rows that let the hint hit will not catch it.
- **Two documents in disagreement.** The draft and the RemPlan now disagree about what is decided (N-P3-3) and who owns `https_adapter/auth.rs` (N-P3-1). Lanes cut from either will diverge.
- **Pin and allowlist.** A pin bump without `reconciled_commit` fails the process-globals check, and the stated order stalls 2.3b (N-P3-2).
- **Still labelled hypotheses, unchanged from round 1:**
  - gwz-git's GET-finish-then-POST order (no production caller until 2.5);
  - server acceptance of a header-less POST on an NTLM-authenticated connection;
  - the Git behaviours in §2.
  r3 labels them correctly. H2b-2 and H3a-3 cover the order the hint needs, not the order gwz-git will use.
- **Next action:** r4 adopts U7, applies the nine P3 text corrections, and aligns the R3/R4 rows (lane owner). Then a round-3 re-read on this axis is limited to those locations.
