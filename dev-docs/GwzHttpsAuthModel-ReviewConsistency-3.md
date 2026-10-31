# HTTPS authentication model (DRAFT r4) — Consistency-AXIS RE-VERDICT, round 3
**Review object:** dev-docs/GwzHttpsAuthModel-DRAFT.md revision 4 (637 lines) at root 56186ad1, with its "Revision 4 changes" table, plus the `GwzPhase1-RemPlan.md` R3/R4 rows as amended at 0454aa1f and 56186ad1. Code is at gwz-core a7b965c5 and gwz-transport 0a253639.
**Baseline:**
- I read every root document with `git show 56186ad1:<path>`, and code with `git -C <member> show <sha>:<path>`.
- `git diff 56186ad1 HEAD -- dev-docs/GwzHttpsAuthModel-DRAFT.md` was empty at start and at end (HEAD was 56186ad1 both times).
- The contract, the plan, `GwzSplitCommandChannel.md` and the 2.3 plan are unchanged since 0a0545a3.
- New input: `GwzHttpsAuthModel-RemPlan-2.md`. I did not read the Safety-2 report.
- I made no writes to the workspace and ran no builds. A temporary copy of the draft in my session scratchpad was deleted.
**Date:** 2026-10-31
**Axis:** Consistency, round 3: r4 against the round-2 Consistency findings, the RemPlan-2 dispositions and its controlling graph. Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: GO** — 0 P0, 0 P1, 0 P2; 6 new P3s (N3-P3-1 to N3-P3-6), all text or row corrections, none architectural. N-P2-1 and N-P3-1 to N-P3-9 are closed. This GO honours my round-2 pre-commitment. The six P3s must be dispositioned (fixed, or recorded with an owner and a closure test) before the T text lands; none blocks acceptance.

## 0. Prior-finding closure table

| Round-2 ID | r4 location | Check performed | Status |
|---|---|---|---|
| N-P2-1 (U7: RA2 rejects an unchallenged 200) | §3(b) `Need`; §3(d) G2, G3, G8, RA2, RA2a; §3.1 "Learning" and "Unchallenged re-authentication"; A4; A11; rows H3b-4 and H3b-5 | `evidence` = `Challenged` or `OnBound`. RA2 applies only to `Challenged`. RA2a sends the POST anonymously, as P3; a 401 gives `PossiblySent`, and the entry is unchanged. G8 upgrades `OnBound` to `Challenged` ("a kind or evidence change replaces"); G3 never downgrades. A4's exception sentence matches RA2a, and so does A11. A7 is unaffected: RA2a is no failure, and the POST is handed over only after the drain. P6 is limited to "after P4 or RA1", so RA2a's 401 leaves the entry unchanged. H3b-5 has the fixture and the mutation | **Closed** (scope wording residual: N3-P3-1) |
| N-P3-1 (file lists) | §5 H1e and H3a; RemPlan R3 at 0454aa1f | H1e lists `admission.rs` and `binding.rs` (`admission.rs:41,70,79`, `binding.rs:188`, verified). H3a lists the `connection_host.rs` mod line. R3 owns `https_adapter/{exchange,auth}.rs` | **Closed** |
| N-P3-2 (pin order) | §5 Order | A rule replaces the fixed order: rebase on the latest pushed commit, and move the pin with the allowlist `reconciled_commit`. R1 and R1b are named, and no order is fixed among R1, R2b, 2.3b and this pin, so the 2.3 plan's waves are untouched | **Closed** |
| N-P3-3 (stale authority text) | Task boundary; A13; U1, U2, U6; R4 row | Cites the amended R4 row. The (+) markers are gone (the only "(+)" left is in the change table). R4 lists §7, §8, §11 and the mod lines | **Closed** (new currency residual: N3-P3-3) |
| N-P3-4 (A8 against infallible `register`) | §3(b) Lifetime; A8; H1e-1 and H1e-4 | "While one admission holds or has consumed the claim, `admit` of any clone is refused `InvalidInput` (`register` stays infallible)". There is a concurrent row | **Closed** |
| N-P3-5 (`Exhausted` outside the table) | §3(d) X1-X3; H1d-5 | Rows added | **Closed** (new inconsistencies inside the rows: N3-P3-2) |
| N-P3-6 (budget-refusal surface) | §3(b) bounds; A9; H1d-1 | Local `io::ErrorKind::QuotaExceeded` with a named message (stable on the pinned 1.95 toolchain). Under `cli`, `Failed { code: Capacity }` with no detail. A9 defines a detail-less `Capacity` that ends an HTTPS-adapter exchange as the record full. I checked that the pool's own `Capacity` arises at openings in this route (`https_connection.rs:412,445` are connect failures; `served.rs:513` is `open_failed`) | **Closed** |
| N-P3-7 (A12 "per command") | A12 | "Captures once per admitted operation, on the original entry" | **Closed** (anchor residual: N3-P3-4) |
| N-P3-8 (A6 "this crate"; A10 owner-full) | A6; A10 | "the gwz-core HTTPS adapter's re-authentication"; "and its owner not full" | **Closed** |
| N-P3-9 (counter route) | A12 3.4 sentence; U5 | The route is named: the factory keeps the adapter's counter handle, as `https_tests.rs:79` keeps `scopes()` (verified) | **Closed** (boundedness residual: N3-P3-5) |

## 1. Changed-range analysis (r3 to r4: +212/−… in the draft; R3/R4 rows in the RemPlan)

- **Header and change table (L1-53).** Each row maps to a RemPlan-2 row; I checked every location the table names.
- **Task boundary (L55-63).** It cites R4 "restated at 5de7d71b and amended at 0454aa1f". It omits the 56186ad1 amendment (size, R3 caller condition) made in the same commit (N3-P3-3).
- **§3(b) (L165-205).** `evidence`, the budget-code surface and the served-path lifetime: consistent with A9, A12 and H1d-1.
- **§3(d) (L246-285).** G2, G3, G5, G8, RA2, RA2a and RA7 are edited, and X1-X3 are new.
  - RA2 and RA2a agree with A4, A7 and A11.
  - X1-X3 disagree with the bounds table and with H1a-7, and the reach of X3 is unstated (N3-P3-2).
- **§3.1 (L320-383).** The "Learning" and "Unchallenged re-authentication" paragraphs are new; the R1 re-arm dependency is stated. The unchanged paragraph "Scope after re-authentication" (L381-383) is now wrong for RA2a (N3-P3-1).
- **§3.2 item 3.** The caller-identity condition matches the R3 row at 56186ad1.
- **§4.** A4, A6, A8, A9, A10, A11, A12 and A13 edited. The A9 meaning is consistent with the code. The A12 anchors are incomplete (N3-P3-4).
- **§5.** Sizes recomputed: 3,725 added and 1,090 product. My sums of the table agree, and every sub-step is 500 lines or fewer (largest: H3b, about 410). The pin rule, the R1 and R1b dependencies and the "adopted whole" sentence are new. Rows H1d-1, H1d-5, H1e-1..7, H2a-6, H3a-1..2, H3b-4..5 and H3c-3 are edited.
- **§6.** U1, U2, U6 and U7 are resolved. Some resolved text is stale against the same commit (N3-P3-3).
- **RemPlan rows.**
  - R3 owns `https_adapter/auth.rs` and carries the caller-identity condition.
  - R4 lists §7, §8 and §11 and the mod lines, with "about 3,725 ... per r4", but still cites "the file lists of ... r3 §5" (N3-P3-3).
- **New root causes.** None architectural. Each new finding below is a local wording, row or anchor defect.

## 2. Findings

### N3-P3-1 — "Scope after re-authentication" says the re-authentication always reports `LogonSession`; under RA2a it authenticates nothing

- **Location:** draft §3.1 L381-383 ("The re-authentication reports `LogonSession` in `Ended` (W1), so the connection is recorded before it can idle"). Compare W1 at L289-290 (conditional: "sends a credential ... or completes a native conversation"), "Scope of the flag" (true only if `prior == LogonSession` or the logon session answered) and RA2a.
- **Root cause:** a sentence written for RA1 was not narrowed when RA2a was added.
- **Sequence, if implemented literally:**
  1. RA2a on c2: no native conversation, and c2 is reported `LogonSession`.
  2. W3 makes that sticky, and `connection_bound()` turns true, so the affinity points `/priv` at c2.
  3. `/priv`'s POST is sent as is on c2 (P4) and gets 401 (c2 was never authenticated).
  4. P6 makes `/priv` `Rejected` for the rest of the operation.
- **Impact:** a fail-closed loss of a private key, but only for an implementer who follows the unconditional sentence over W1.
- **Closure:**
  - The sentence reads "After RA1, the re-authentication reports `LogonSession`...; after RA2a the connection keeps its prior scope".
  - H3b-5 also asserts that c2's scope stays `Anonymous`, that `connection_bound()` is false for the anonymous POST, and that no affinity entry names c2.

### N3-P3-2 — The `Exhausted` rows disagree with the bounds table and with H1a-7, and X3's reach is unstated

- **Location:** §3(d) X1 L279 ("the 64th rejection fingerprint") against H1a-7 L530 ("65th fingerprint ... give `Exhausted`"); the bounds-table row "Rejection fingerprints | 64" ("every later lookup result is refused") against X3 ("P1, P3, P4, P5: any POST ... nothing: the budget code").
- **Root cause:** X1-X3 were added without reconciling them with the bounds row and the H1a rows.
- **Items:**
  - **(a) Off by one.** Is the 64th fingerprint stored, or does it trigger `Exhausted`? X1 and H1a-7 disagree. The other bounds use the n+1 convention (513th key, 65th credential).
  - **(b) X3 covers P3.** Every unrecorded, anonymous POST then fails once the record is `Exhausted`. With X1's "a failed `getrandom` at creation", that means every HTTPS POST of the operation, public repositories included. r3 refused lookups only. This contradicts the bounds row and 3(a)'s principle (a request the server serves to anyone must not depend on authentication state), and H1d-5 tests only a previously-`Basic` key.
  - **(c) Native answers under `Exhausted`.** G2 and G8 are not covered: is a native answer still recorded into an `Exhausted` record?
- **Closure:**
  - Fix the threshold in one place.
  - Either keep the key digests on clearing (no secrets), so X3 refuses only keys that were recorded and P3 stays anonymous for never-recorded keys, or state the wider X3 in the bounds row and in A3/A8.
  - Add X rows for G2 and G8.
  - H1d-5 asserts the chosen P3 behaviour.

### N3-P3-3 — Same-commit currency: the RemPlan R4 row and the draft each describe the other's previous revision

- **Location:**
  - RemPlan R4 at 56186ad1: "with the file lists of `GwzHttpsAuthModel-DRAFT.md` r3 §5 (about 3,725 lines ... per r4)". r3 §5 lacks H1e's `admission.rs`/`binding.rs` and H3a's mod line.
  - Draft §5 ("r3 said 3,620, which the R4 row still quotes") and U1 ("The row's 'about 3,620' is r3's figure"). Both are false at 56186ad1.
  - U6 ("The caller-identity condition ... is for the lane owner to carry into that row"): already carried.
  - Task boundary: omits the 56186ad1 amendment.
- **Root cause:** the draft and the RemPlan rows were edited in one commit, and neither re-read the other's final text.
- **Impact:** a lane cut from the R4 row takes r3's file lists, and the draft's own resolution notes are wrong.
- **Closure:** R4 cites "r4 §5" (or "the accepted revision's §5"). Drop the "still quotes 3,620" and "for the lane owner to carry" clauses, and add 56186ad1 to the task boundary.

### N3-P3-4 — The A12 anchor row omits two of A12's own insertion points

- **Location:** §4.0 table row A12 L425 lists "Plan 3.1 Prerequisite (L724-728); plan 3.2 (L729-735); plan §10 ...". The A12 text then also inserts into "The Phase 2 served path (plan L72-74)" and "3.4" (L739).
- **Root cause:** the anchor table was not updated with the round-2 additions.
- **Further gap:** the served-path sentence ("a fresh capture ... per bind") lands in the plan's §1 terms paragraph. The controlling text for the served receiver is `GwzSplitCommandChannel.md` §3.4 L136 ("builds the `PreparedOperation` itself from that call's captured invocation"), which A12 does not touch.
- **Closure:** the A12 row names all five targets by paragraph. Add a sentence, or a §15 note, that Split L136's capture is per bind.

### N3-P3-5 — The counter route copies an unbounded test pattern into production

- **Location:** A12 3.4 sentence ("the composition's factory keeps each built adapter's counter handle, as the test factory keeps `scopes()` (`https_tests.rs:79`)"). The test factory pushes into a `Vec` that is never pruned (`https_tests.rs:70-80`).
- **Root cause:** the route reuses a test convenience without its bound.
- **Impact:** instances are LRU-retired and rebuilt (RemPlan R1, Code P2-3), and R6 runs 10,000 operations. A production factory that keeps every built adapter's handle grows without bound, against plan R6 and contract §6 L317-318 (no forever-growing host-wide set).
- **Closure:** the route is one shared saturating counter the factory owns, or handles pruned at instance retirement. The 3.1 composition row also asserts a bounded size after more than 16 profile changes.

### N3-P3-6 — H1e's new `AdmissionError::InvalidInput` reaches a served core as `bind_rejected(capacity)`

- **Location:** H1e (L500, L532-533) adds `AdmissionError::InvalidInput`. The client receiver maps every admission error except `Closing` to `ErrorCode::Capacity` (`gwz-connection/src/remote/receiver.rs:321-326` `rejection_code`; `remote/served.rs:236-239`).
- **Violated text:** `GwzSplitCommandChannel.md` L136-137 makes a bind for an already-bound request `bind_rejected(invalid_request)`.
- **Root cause:** the new variant was added without tracing its served mapping.
- **Impact:** the case is unlikely under H1e-7's fresh capture per bind. If it occurs, the core reports a misleading `capacity` for what is a composition fault.
- **Closure:** H1e adds `receiver.rs` and `served.rs` (neither is owned by 2.3b, whose files are `remote/{core,backend,lane}.rs`) and maps `InvalidInput` to `InvalidRequest`, with a row. Or the text states the `capacity` mapping as accepted.

## 3. Risks

- **The three remaining residuals with consequences** are N3-P3-1, N3-P3-2(b) and N3-P3-5. Each has a stated failure mode and a closure row, and each changes behaviour only where an implementer could read the text two ways. Settling them before T lands keeps the contract text and the rows from diverging.
- **Labelled hypotheses, unchanged:**
  - gwz-git's GET-finish-then-POST order;
  - server acceptance of a header-less POST on an NTLM-authenticated connection;
  - the §2 Git behaviours.
  r4 labels them correctly. H3b-5 now covers U7's regression on a hint miss.
- **Next action:** the lane owner dispositions N3-P3-1 to N3-P3-6 in r5 or in the T-text commit. No further Consistency round is needed if the dispositions are text and row edits at the named locations.
