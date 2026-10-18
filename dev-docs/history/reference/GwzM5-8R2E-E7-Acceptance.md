# GwzM5-8 R2-E E7 Acceptance — the settled gate

Status: **CLOSED — R2-E ACCEPTED, 2026-08-29.** Both dual-#2 axes filed
GO-WITH-CONDITIONS in round 1 (peer-blind; zero escalating findings between
them); every condition is a record act executed in this document, the plan,
and the freeze (§5), plus the one sanctioned gwz-core driver commit (§2).
Conditions folded by the lane owner are folds, not a remediation round — the
two-round cap was never engaged. Charter: `GwzM5-8R2E-Plan.md` §3 Phase E7;
the operator's "proceed with E7" (2026-08-29).

Object: gwz-core main `be693bd` ("chore(release): gwz-core 0.11.1", tag
v0.11.1, released 2026-08-29 across all three channels). The settled tree is
the release tree; no commits have landed after it.

**Two framing statements (State axis, completeness sweep — both required for
"R2-E accepted" to be true as written):** (i) R2-E closes against its
**§1.1-amended object**: the plan §0 object statement ("the §10 conversion
table, executed") is deliberately not met, by the operator's ruling "a"
(2026-08-27, verbatim), and §1.1 is the authority that re-scopes the close —
*[and a SECOND re-scope, 2026-09-02: rows `:275`–`:279`'s capability-free
arms are a dated exception per `GwzM5-8R2E-CapabilityFreeAmendment.md`; see the O1 row's bracket]* —
O1/O2's re-owned form below IS the sanctioned close of that object. (ii) The
E6 milestone annotation (review F-7) means the review-debt ledger empties
**at this record's own acts** — each act in §5 below is therefore part of
the acceptance, not commentary on it.

---

## 1. E7.1 — dual #2, the full-tree settled review

Peer-blind Code + State axes (Fable tier per the program's dual-gate
policy), launched 2026-08-29 against `be693bd` and the record set as of the
same date, carrying the E1-E6 escalation second-axis scrutiny and the
accumulated E7-dual queue (anchor nit 1 with the platform.rs:219-234
bounded-read template; [R2-P3-1]; [R2-P3-2]; [R2-P3-3]; the K/J(ii)
determination; the tier-2 comparable-sub-surface question; E3 F7; the
second-axis items E3 F1/F2/F3, E2 [P2-1], E5 [P1-1]).

- **State axis: GO-WITH-CONDITIONS** (`GwzM5-8R2E-E7-ReviewState.md`,
  2026-08-29) — 0 P0 · 0 P1 · 1 P2 · 7 P3. "Every prescribed ledger close
  form matches reality as of `be693bd` — no O-row's close form fails."
  Every condition is a record act performed in this acceptance: the [P2 F1]
  plan annotation and the [P3 F2]-[P3 F8] acts, all executed at §5 below.
- **Code axis: GO-WITH-CONDITIONS** (`GwzM5-8R2E-E7-ReviewCode.md`,
  2026-08-29) — 0 P0 · 0 P1 · 0 P2 · 4 P3, all record-class. "The tree is
  sound as landed: all 38 re-reserved keys are bound with the semantics the
  freeze and the controlling amendment pair record … and all five
  machine-enforced inventories match the tree and fail closed under my own
  perturbation probes." Its verification record includes: all gates
  direct-exit green in its own worktree (447 / 979+1 / 256 matching the
  darwin pins; fmt/check/clippy 0; boundary ok 15/5; compat 7/7/10 + suite
  27/27); all five inventories perturbation-probed RED and reverted; digest
  re-pin archaeology across `8597d32..be693bd` (E6's two re-pins each moved
  exactly one digest, both platform.rs); the E3 [P1 F1] cure ratified with
  an independent at-the-bound probe (64 rows recover, the 65th refuses
  typed); the E2 [P2-1] wedge grammar re-derived from `slots.rs` first
  principles; O9's 8-byte window arithmetic re-derived and machine-held.
  Conditions C1 (the O12 anchor-nit-1 close carries the Q1 shape,
  re-owned with a named carrier) and C2 (the freeze §3.5 barrier record
  gains the dated named-exception sentence) — both executed at §5.

**Round status: both axes complete in round 1; the two-round cap was not
engaged. The peer-blind axes converged independently on [R2-P3-2] (named
exception, no 17th key) and on the E4-consumer-contract framing of the
settled≠barriered / converged≠flushed pair.**

## 2. E7.2 — three-platform matrix acceptance at the settled tree

No new dispatches were needed: the settled tree is the release tree, and the
release train executed the matrix at it.

- **Linux — per-partition, EXECUTED at the settled tree.** The Release
  workflow run **33196576270** (event `release`, head
  `be693bdebbecd8208ffc61f3343f8185c06f7184`, conclusion success; the
  checkpoint's release record notates the pair "33196574973 → 33196576270";
  the run and both verify jobs were re-verified via `gh` 2026-08-29), job
  **Verify (ubuntu-24.04) = 98935133025**, ran the fault battery against the
  pinned counts: `checked_artifact::` **457**, lib remainder **980**,
  `v1_lifecycle::` **256**, root_fault_matrix (release profile) **1** — all
  four per-partition linux values MEASURED, discharging every
  FIRST-DISPATCH-EXPECTED linux marker in the driver by direct execution
  (the E5 CI closure's owed item). Per the State axis (Pins §): the
  conversion is **sound and digest-safe** — the driver rewrite is a
  scripts-only doc edit (no protected source-tree digest covers
  `scripts/checks/`), touches only the linux provenance wording in
  `_fault_count`, and leaves the FIRST-DISPATCH-EXPECTED convention text
  intact for future moves. Landed as this acceptance's one gwz-core commit:
  **`8e18403`** (scripts-only, +18 lines, pushed `be693bd..8e18403`;
  gates proportionate per the `8597d32` micro-train precedent —
  py_compile 0, driver `--list` 0, boundary checker ok 15/5 exit 0; the
  push legs fire at `8e18403` routinely).
- **Windows — EXECUTED at the settled tree.** The same Release run's job
  **Verify (windows-2022) = 98935132771**, success; additionally the Windows
  matrix **33188777003** at `cc7c625` (the settled tree's parent — `be693bd`
  is the version-bump commit) and the era's four-leg green chains at
  `afbc25d` (33175382330) and the `9a64ce9` first-dispatch red fixed forward
  at `cc7c625` (fixture-only os-error-267 class, checkpoint record).
- **Darwin — EXECUTED at the settled tree.** The release script's full local
  gate in a clean worktree at `be693bd` (2026-08-29): fmt, check, clippy
  `-D warnings`, partitions 447 / 979 + 1 ignored / 256, root_fault_matrix
  release-profile 1, boundary checker, g23 marker 124. The boundary checker
  was re-executed directly at `be693bd` for this acceptance (2026-08-29):
  "checked-artifact boundary: ok (15 visible entries, 5 classified
  modules)", direct exit 0.
- **Sum identities** (E5/E6 CI closures, Platform matrix full-lib runs):
  darwin 1641 passed + 1 ignored = 447 + 937 + 256 + 1 at afbc25d, and
  447 + 979 + 256 + 1 = 1683 + 1 ignored at the settled tree's local gate;
  linux 457 + 980 + 256 + 1 = 1694 with 1 ignored at the release verify.
  The darwin/linux remainder offset is the pre-R2-E baseline platform delta
  plus the cfg(unix) O9 pair, per the driver docstring's dated blocks (the
  one bare-`#[ignore]` test premise-checked at the E5 CI closure).

**E7.2's matrix acceptance is met by citation of executed runs; the current
pin set has per-OS measured status on all three platforms.**

## 3. E7.2 — the 481-item frozen-surface reconciliation (O12's settle rider), EXECUTED 2026-08-29

**Method** — the Phase 4 settle-item-7 measures (`GwzM5-8R2DPhase4Closure.md`
§5, "All eleven module-level allow(dead_code) attributes … were deleted and
the tree recompiled" / "a --force-warn dead_code sweep over the whole
subsystem"), reproduced on `be693bd` in a detached throwaway worktree,
against **both denominators** (plan §1.1: "the 481-item reconciliation stays
E7.2's, against both denominators"):

- **Denominator 1 (blanket-hidden subset).** The Phase-4 eleven module-level
  allows survive on today's tree as **twelve heir attributes** (seven kept in
  `checked_artifact/mod.rs`; three narrowed into `catalog.rs`'s children;
  two in `coordinator/mod.rs` — admission's and catalog_names' were deleted
  at Phase 4 with zero items). All twelve deleted, `cargo check --lib`:
  **337 unique dead items across 54 files** (every one under
  `checked_artifact/`). Baseline (2026-08-23): **481 / 50**.
- **Denominator 2 (global sweep).** `RUSTFLAGS="--force-warn dead_code"
  cargo check --lib` on the pristine tree: within `checked_artifact/`
  **998 warnings across 82 files** against the baseline **1657 spans / 85
  files** (the baseline's "whole subsystem" scope is `checked_artifact/`,
  evidenced by its file count); the crate-wide figure, recorded as new
  context, is 1154 / 141.

**Movement:** −144 blanket-hidden items (481 → 337) and −659 subsystem spans
(1657 → 998) since the Phase 4 settle — the measured consumption effect of
E1-E3 binding all 38 re-reserved keys (with E5/E6's additions inside the
same window). Per-module blanket-hidden movement (baseline → settle):
protocol ~144 → 66; namespace 51 → 16; catalog 54 → 30; bootstrap 82 → 67;
entry 14 → **0** (fully consumed); fault_v1 7 → 4; leaf 4 → 4; capability
~90 → 93; coordinator 31 → 47; admission 0 → 10. The three risers are
R2-E-era additions and cascade re-attribution, not regressions: rustc treats
allow(dead_code)-covered items as live roots, so deleting the covers
cascades warnings into items (admission's ten among them) whose only
consumers are themselves frozen — the same convention the baseline was
measured under.

**Disposition:** the residue (337 blanket-hidden / 998 subsystem spans) is
the R1/R2-frozen interface whose first consumer is **Phase E4**; the
expectation of the count falling to (near) zero **transfers to the E4
resumption** under O1/O2's named carrier, since E4 converted nothing in
R2-E (State axis, O12 row). Phase 4's zero-orphans audit is not re-litigated
here — this reconciliation records measured movement at the settle, per O12.
Counting convention: one `dead_code` diagnostic = one item; files counted by
primary span.

## 4. E7.2 — the ledger's row-by-row close (O1-O13, and E6.3)

Close forms per plan §1.1 (the operator's ruling "a", 2026-08-27, and the E7
close form set). The State axis's ledger-close table verdict: **all fourteen
rows CAN close exactly as prescribed at `be693bd`**; its named conditions
are folded into the rows below. *Final on the Code axis's GO.*

| Row | Close | Carrier / citation |
| --- | --- | --- |
| O1 | **RE-OWNED with a named carrier** (not DISCHARGED) | R2-F's quarantine/relocation package → this lane's E4 resumption; the close carries §10 row `:280` via O13; `ConsumerCheckpoint:272-280` (nine rows); plan §1.1 cross-lane row + §7.8 fallback (iv) with the dated RULED line *[2026-09-02: O1's eventual close additionally cites GwzM5-8R2E-RecordRootAmendment.md — row `:280` carries one recorded exception (the record root) alongside O13/O14.]* *[2026-09-02: the named carrier's E4 resumption executed E4.1/E4.2; O1's obligation text is now RE-SCOPED a second time (plan :46) and closes DISCHARGED at E4.7 citing O13, `GwzM5-8R2E-CapabilityFreeAmendment.md`, the record-root amendment, and O14 as RE-OWNED to DR-1.]* *[2026-09-02, PHASE E4 CLOSED: O1 CLOSES per that line — `GwzM5-8R2E-E4-Close.md` §2 is the residual register (the record root; the twenty carved writers; the three `execute.rs` forward arms, `:45` on the directional-residue ground by operator ruling (a)); nothing of row `:280`'s obligation is left silent.]* |
| O2 | **RE-OWNED with a named carrier** | same carrier (settled tuple §11.3 item 1, the A1 coexistence gate — verified live by the State axis; no production `recover_or_create` caller appeared since E0.1's zero-caller trace) |
| O3 | **RE-OWNED to R2-F's quarantine/relocation package, which discharges O3 directly on landing** (§7.6.2's single-owner fact: `policy.rs:33-42` defines the legacy private parent, `observation.rs:93` its only consumer); E4.7 remains this lane's stronger full-retirement mechanism at the E4 resumption; OPEN-R1 rides with the package (blocking for its owner). *[Tense per State [P3 F8]: nothing has discharged O3 yet — the package discharges it on landing.]* *[DISCHARGED 2026-09-01: the landing happened — R2-F R1.1 at gwz-core `027da5b`; this row quoted verbatim with the full discharge record at the R2-E plan's O3 row (R1.3). Single-owner scope note ([P3-7]): the fact is the parent COMPOSER's location-ownership and stays true; the name family separately counts THREE production spellings (legacy variant leaf bytes, bootstrap marker stem, scratch stem `catalog/scratch.rs:6`). OPEN-R1: resolved by design — see the §5 routing row's dated note.]* | addendum §7.6.2 + §7.8(a); plan §1.1 O3 + OPEN-R1 rows *[2026-09-02: "E4.7 remains this lane's stronger full-retirement mechanism" — that retirement is RE-OWNED to DR-1 as O14's outcome (`GwzM5-8R2E-CapabilityFreeAmendment.md` §4); O3's own discharge (R1.3, 2026-09-01) stands unchanged.]* |
| O4 | **DISCHARGED** — cleanup.* 11/11 executed, matrix-green both variants | E1 landing `4a0b01a`, review GO (`GwzM5-8R2E-E1-Review.md`); freeze cleanup activation record; E1-tip CI 4/4 green |
| O5 | **DISCHARGED** — barrier.* 16/16 executed; the caller-supplied-restatement class extinct | E2 landing `c11c5ef` (squashed per the minted lesson), round-2 GO; nine→ten inventory move in place; first native Windows execution green (WINDOWS-ARM-OWED discharged, 33044834867) |
| O6 | **DISCHARGED** — mint-side + read-side observe-or-refuse, production-unreachable at the settle; wire zero-diff re-verified by the State axis over the whole span (94da3e5..be693bd, both generated.rs files) | E0.2b §5; E2's O6 witness (read-side refusals split into named rows beside a positive control) |
| O7 | **DISCHARGED** — terminal.* 10/11 PartiallyExecuted with the T-D determination; T1 widening live at exactly three gates; announced-vs-performed integrity verified at all four homes (State axis, Determinations note) | E3 landing `1d50e59`, round-2 GO; freeze §4.3 E7 annotation "ten activated + one determined" |
| O8 | **Split close**: the 9+1 registry partition (9 valid_unlisted rows + G-VERIFYING's clause-cited disposition) + eight tier-1 byte-preservation rows DISCHARGED; two PENDING-FIXTURE rows carrier **R2-F**; tier 2 RE-OWNED — **carrier E4.4, under the two-part encumbrance: R2-F-gated AND sub-surface-by-amendment-first** (amendment §6.3's tier 2 as written is NOT SATISFIABLE; the comparable sub-surface must be minted by amendment with dual review before E4.4 executes it — State [P3 F7]). **Denominator authority: Evidence §12.9(e)** (State [P3 F3] — the addendum §8 "10 registry rows" figure is superseded there, right as a shape count, off by one as a registry-row count). This close does not cite byte-equivalence as green: §12.8 stays PARTIAL. | E5 landings `0a17e48`/`fc0bb22`; `GwzM5-8R2E-E5-Review.md` adjudications A and G(iii); Evidence §12.6 E5 bracket, Tables A/B *[2026-09-02: tier-2's carrier E4.4 dissolved — tier 2 is EXPLICITLY DEFERRED to DR-1, which mints the comparable sub-surface BY AMENDMENT WITH ITS OWN DUAL and names the execution carrier; the `gc_archived` route re-owns to DR-1 conditional on (C) — `GwzM5-8R2E-CapabilityFreeAmendment.md` §5.]* |
| O9 | **DISCHARGED** — the composed-path upgrade-failure fallback executed (genuine ENAMETOOLONG at the upgrade's own open(2) via the 8-byte staging-name window; control arm + isolation guard; cfg(unix) pair) — both halves MET on the E6 review's own ledger (E6-1, E6-2), including the moved-pin coverage restoration | E6.1 `d73abe8`, landed `afbc25d`, review GO; A1 record §14 |
| O10 | **DISCHARGED**, close wording per E6 review F-5, verbatim: "cfg(test)-gated variants — a third shape, strictly stronger than both named options (production compiles no constructor at all; negative compile probe verified twice)" | E6.2 `e1e043f` |
| O11 | **CLOSED NEGATIVE at E0.1** (no v0.11.0 production path reaches the identity probe); the capability-refusal UX folded into E4.1's seven-item precondition set (5.3's five incl. the `unsupported.rs` claim-sweep as item 5, + item 0 the coexistence gate, + item 6 refusal-ordering/restart legality) — travels with E4's carrier | `GwzM5-8R2E-E01ReachTraces.md` §(a); plan §1.1 |
| O12 | **DISCHARGED at this settle, WITH this record's own acts** (State axis's O12 condition): three riders consumed at E0.2 — rider 2 (A-1 re-framed) **with its E4.7 reopen condition traveling to the E4 resumption**, the `unsupported.rs` nit riding E4.1's preconditions; the abort rewrite-set bound guard tie executed at E6.2 (E6 review E6-4 MET); anchor nit 2 executed at `a593dbd` (E6-5); anchor nit 1 **RE-OWNED with a named carrier per dual #2 condition C1** — the next production train touching `checked_artifact/` (the E4 resumption's first train, or the R2-F lane if one lands there first) implements the Code axis's Q1 shape rather than re-deciding it: bound the shared reader `observe_leaf_exact` itself (signature unchanged; cap = the already-identity-checked `opened.len()` fstat; `try_reserve_exact` + `take(len+1)` per the platform.rs:220-235 template; allocation failure a typed refusal; overrun caught by the existing len-mismatch → Invalid arm; the stale ":394" terms are retired — this shape supersedes them); [R2-P3-3]'s one-word wording fix and the F3 stat-level family-gate reorder ride the same train as one class; the 481-item reconciliation executed at §3 against both denominators *[nit-1 carrier deviation 2026-09-01: the R2-F alternative fired without carrying (R1.1 `027da5b`, un-routed by the R2-F charter — lane owner's miss); the obligation binds E4.1 — full note at the carrier-table row below]* | plan §1.1 O12 re-route; E0.2 §7.1/§7.2/§7.4; Code axis Q1/F2/F3 *[2026-09-02: the A-1 rider's E4.7 reopen condition SURVIVES E4.7's re-scope and stays E4.7's to check — `GwzM5-8R2E-CapabilityFreeAmendment.md` §4.]* |
| O13 | **Pin half DISCHARGED** (`V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` landed `8597d32`: exactly {archive.rs, store/archive.rs, store/rewrite.rs}, set-equality both directions, probes executed RED, shrink-only retirement protocol — verified live by the State axis); **substantive half RE-OWNED** with E4.2/E4.3 under E4's carrier, incl. the §7.7 duties (door disposition + workspace-root binding); the dated accepted-residual record (A1 → E4.2/E4.3 interval) STANDS at both homes and expires with those landings | plan §1.1 O13 row + E4.2/E4.3 entries; addendum §7.6.1 *[2026-09-02: the pin's retire-on-conversion expectation is SUPERSEDED for `store/rewrite.rs` — the record's rewrite path is carved out permanently by GwzM5-8R2E-RecordRootAmendment.md §2 (P-1 keeps the row fail-closed both directions); the archive rows keep retire-on-conversion until E4.4.]* *[2026-09-02: the archive rows are CARVED OUT too (`GwzM5-8R2E-CapabilityFreeAmendment.md` §3) — the inventory is permanent-documented in all three rows and never empties; "retire-on-conversion until E4.4" is superseded.]* *[2026-09-02, PHASE E4 CLOSED: the substantive half's carrier (E4.2/E4.3) is spent — E4.2 landed the record's door, E4.3 became the record-root carve-out; O13 stays the v1_lifecycle SCAN (three rows, three permanent exceptions), the twenty-row `CAPABILITY_FREE_RAW_WRITER_INVENTORY` is the data layer; the `gc_archived` family KEPT permanent-pending-DR-1 (its measured extent in `GwzM5-8R2E-E4-Close.md` §3 and `v1_lifecycle/archive.rs`). Close record: `GwzM5-8R2E-E4-Close.md`.]* |

**E6.3 — dated no-work record (2026-08-29, closing per plan §1.1 and §3;
content per the State axis's four-ingredient specification).** E6.3 is VOID
with both branches empty: (i) the reach branch — O11's question was answered
NEGATIVE at E0.1 (`GwzM5-8R2E-E01ReachTraces.md` §(a): no v0.11.0 production
path executes the `name_to_handle_at` probe; `recover_or_create` has zero
production callers; the 22.04 failure was fixture-only), so the "if reach is
real" branch never opened; (ii) the UX branch — the capability-refusal UX
moved into **E4.1's precondition set** (the seven items enumerated at O11
above; plan §1.1 O11 row), so no UX work was E6.3's; (iii) this record is
dated and cites plan §1.1's "E6.3 — VOID" row and E0.2b [P3-5]; (iv) the E6
review's **E6-10 row** is the executed emptiness check — nothing in the E6
diff touches UX/runbook/notes surfaces. No work was performed under E6.3,
none was owed, and nothing is silently dropped: the UX obligation travels
with E4's named carrier.

## 5. Record acts performed at this acceptance (the State axis's conditions, executed)

1. **[P2 F1] — the [R2-P3-1] scope clause is now written where E4 will read
   it**: dated E7.2 annotation at the plan's §3 E4 gate note (2026-08-29,
   beside the E2.2-ordering sentence): *a settled barrier ordinal does not
   imply its target parent's dirents were ever ordered; E4 consumers must
   not rely on the implication.* Contract-fork disposition recorded in the
   same annotation: **state the non-implication explicitly** — re-barriering
   a converged alias would be an E0.2b-class semantics move on the frozen
   16-key record, for a property no current consumer needs (State
   determination; *Code-axis concurrence to be noted on filing*).
2. **[P3 F2] — [R2-P3-3] closed as a named accepted residual**: the
   `platform.rs:489` cost-claim wording ("two `symlink_metadata` calls" —
   imprecise; `leaf_is_resident` is a full bounded observation) missed its
   assigned fold window (E6.2b moved the platform.rs tree without it).
   Carrier restated: **the next production train that moves the platform.rs
   protected tree** (the E4 resumption or the R2-F lane) folds the one-word
   fix with its digest re-pin. Deliberately NOT executed at E7.2 — a
   comment-only digest move at the acceptance is a worse trade than the
   residual.
3. **[P3 F4] — the four OPEN-B register rows closed by citation**:
   **OPEN-B2** answered "no widening — PASS" (E2 review; expiry condition
   attached to the caller-count no-move confirmation). **OPEN-B3** answered
   "PASS on placement", honest limit filed as E2 [P3-1]. **OPEN-B7**
   discharged by the first native Windows execution of the roaming arm
   (Windows matrix 33044834867 green at c11c5ef; WINDOWS-ARM-OWED
   discharged). **OPEN-B8** answered positive on the conjunction (the dated
   B-4 bracket at addendum §1.5, corrected 2026-08-27, E2-review-ratified).
   The addendum's §10/Appendix B register is superseded by these citations
   under the corpus's resolution-index discipline.
4. **[P3 F5] — the ten-writer-rows native-execution ledger entry, written
   here in the Step-4.2 record's form (freeze `:836-838`), discharging E0.2
   §7.3's "E7.2 re-verifies" clause**: *the ten §11.1 row-d writer rows
   executed natively at the E1 tip — CI 4/4 green: Windows matrix
   33039208104, Platform matrix 33039209950, boundary 33039190896, retained
   readers 33039190893 (success on the known kill-race flake's rerun) —
   re-confirmed at `1d50e59` (33040773113 / 33040774620) and `c11c5ef`
   (33044834867 / 33044836558).* The settled tuple's "ledger entry owed at
   the next matrix dispatch" row (`:665` as of 2026-08-29) is **resolved by
   this citation**.
5. **[P3 F6] — the two E1-review residuals ride by name**: (a) **E1 F1**,
   the 16 KiB alias-retirement bound's prose-only coupling
   (`namespace_mutation.rs:983` names `CleanupRetirementDestination::
   source_bound` in a doc comment; no parameter threading landed E2-E6) —
   carried on §6's residual register **with the E4 resumption** (the bound
   is separately an E4 input per the E1 landing record); (b) **E1 F2**, the
   corrected cohesion ranking ("fifth largest", not third) — closed by
   citing the E1 review's §2.1 correction; the landing record repeats no
   false ranking.
6. **[P3 F3]/[P3 F7]/[P3 F8]** — executed inside §4's O8 and O3 rows, as
   marked there.
7. **Code C2 — the freeze §3.5 barrier activation record gained the dated
   named-exception annotation** (2026-08-29): the P5 roaming recovery
   return rename named as the deliberate, announced-by-no-key exception to
   "every durable barrier edge is announced", with the
   idempotence/atomicity ground and the census-neutrality statement (the
   17th key refused on substance). Both peer-blind axes converged on this
   branch. The Code axis's *optional* in-code qualifying clause at
   `barrier_mutation.rs:19` is **declined at E7.2** (a comment-only edit
   in a digest-pinned tree would force a third deliberate re-pin for zero
   semantic content) and rides the C1 carrier train instead, recorded in
   the freeze annotation itself.
8. **Code F4 — the terminal sibling clause written beside [R2-P3-1]'s** in
   the plan's E4 gate-note annotation (same dated block): *converged does
   not imply flushed* — the terminal converged-restart return does not
   imply key #8's flush or key #9's barrier ran on that drive; E4's first
   consumers must not assume either implication; corpus-wide re-flush
   tightening is a hardening-lane (R2-F class) decision.
9. **The driver's linux pins rewritten MEASURED** — the one sanctioned
   gwz-core commit of this acceptance (§2; State Pins § soundness +
   digest-safety finding; plan §3 E7.2's own instruction).

## 6. Census statements at the settle

All verified against `be693bd` by the State axis (Pins & provenance §):

- The fault-key census holds at **165** (`EXPECTED_KEY_COUNT`, equal to the
  tuple §4.8 arithmetic); all **38 re-reserved keys** accounted: cleanup
  `Executed` (E1) 11 + barrier `Executed` (E2) 16 + terminal
  `PartiallyExecuted` 10 with the eleventh carried by DECISION T-D's
  determination.
- The activation vocabulary's `Reserved` arm is **unconstructed** for the
  first time, retained as frozen vocabulary with the dated allow-reason
  block per the map's carried re-reservation clause.
- **The five machine-enforced inventories of addendum §6.2's stated scope**
  — the inventories the converging E1-E3 commits must move
  (FAULT_INJECTION_SOURCES; CATALOG_PUBLICATION_CALL_COUNTS with the
  fifth-dated-extension comment; PROTECTED_SOURCE_TREE_DIGESTS pre_catalog +
  platform; the catalog.rs digest; the capability_permit Rust twin) — all
  green at the settle. The R2-E-era machine-pin family is larger (the O13
  raw-writer inventory, the driver pins, the g23/compat/map markers), per
  the State axis's scoping note. `PROTECTED_SOURCE_TREE_DIGESTS` was
  deliberately re-pinned twice in R2-E (E2 precedent; E6), exactly one
  digest moving each time, flagged to and accepted by the interior reviews.
- The freeze §3.5 inventory addendum counts **ten** (nine → ten moved in
  place at E2 with dated provenance; the len()==10 machine pin real, inside
  the completeness anchor that set-equalizes declared-vs-scanned sources).
- Wire surface: `git diff 94da3e5..be693bd` over both
  `protocol/generated.rs` files is **empty** — plan §4's non-goal honoured
  through E1-E6 + conf-integrity + the release. `ACTIVE_WRITER_FLOOR`
  untouched.
- Lib accounting at the settle: darwin 1683 = 447 + 979 + 256 + 1 (+1
  ignored); linux 1694 = 457 + 980 + 256 + 1 (+1 ignored); g23 marker 124.

## 7. Residuals and carriers leaving R2-E

| Residual | Carrier |
| --- | --- |
| Quarantine/relocation package (with OPEN-R1: move resident residue or relocate future writes only — blocking for that package's owner) *[EXECUTED 2026-09-01: the package landed — R1.1 `027da5b` + R1.2 tripwire `bb52dc0` on gwz-core main, plan ADOPTED at gwz-dev `d2e5636` after three rounds. OPEN-R1 RESOLVED BY DESIGN: "neither — the relocation relocates the CATALOG'S name; residue is not a category this package creates" (plan §2 verbatim, quoted in full at the R2-E plan's OPEN-R1 row); operator veto open in the adoption record.]* | **R2-F** (operator ruling "a") |
| E4 — the §10 conversion (O1/O2, O13-substantive incl. §7.7 duties, O3's stronger E4.7 form, E4.1's seven preconditions incl. the capability-refusal UX and the `unsupported.rs` sweep; the A-1 rider's E4.7 reopen condition; the 481-surface consumption expectation of §3) | **this lane, after R2-F's relocation lands** *[2026-09-02: the §10 conversion re-scoped — checked-feature writes go through the boundary, capability-free arms carved (`GwzM5-8R2E-CapabilityFreeAmendment.md`); O3's "stronger E4.7 form" re-owned to DR-1.]* *[2026-09-02, EXECUTED and CLOSED at gwz-core `9c808ff`: conversions E4.1 (catalog activation) and E4.2 (the first merge record) only; the A-1 rider's E4.7 reopen condition CHECKED and NOT MET (`cleanup.rs:146-200`, DECISION A-1 STANDS); the allowance class dispositioned (6 expired / 16 re-reasoned permanent-pending-DR-1 / 2 kept); the 481-surface expectation of §3 is unchanged by a docs-and-allowances close. Record: `GwzM5-8R2E-E4-Close.md`.]* |
| PENDING-FIXTURE archive rows (AC-NOPUB-UNBORN, AP-PRESERVED) | **R2-F** |
| Tier-2 archive equivalence — two-part encumbrance: R2-F-gated AND comparable sub-surface by amendment with dual review first | **E4.4** *[2026-09-02: carrier E4.4 → DR-1 (mints the sub-surface by amendment with its own dual) — `GwzM5-8R2E-CapabilityFreeAmendment.md` §5.]* |
| E1 F1 — the 16 KiB alias-retirement bound's prose-only coupling (`namespace_mutation.rs:983`) | **E4 resumption** (the bound is an E4 input) |
| [R2-P3-3] — the `platform.rs:489` cost-claim one-word fix + digest re-pin | **next production train moving the platform.rs tree** (E4 resumption or R2-F lane) |
| C-2's four fixtures, T-5 regeneration, multi-repo CI checkout, legacy-path removal, MAX_PATH, native power-loss evidence | **R2-F** (plan §1, "explicitly NOT R2-E") |
| Ordinary/custom-message merge starts writing v1 | **M5c** |
| conf-integrity backlog (NF-3, NF-4, lock banner, marker in merge candidate_files, CLI pre-dispatch gate, distinct --force) | recorded at the checkpoint's conf-integrity records (standalone lane — resident on the settled tree, correctly registered, not R2-E ledger rows) |
| Anchor nit 1 — bound `observe_leaf_exact` per the Code axis's Q1 shape (recorded verbatim in §4's O12 row; supersedes the stale ":394" terms), WITH [R2-P3-3]'s wording fix and F3's stat-level family-gate reorder as one class | **the next production train touching `checked_artifact/`** (E4 resumption's first train, or the R2-F lane if one lands there first) — the carrier implements, not re-decides *[DEVIATION RECORDED 2026-09-01 (R2-F R1.3): the R2-F alternative FIRED and did not carry — R1.1 (gwz-core `027da5b`) was a production train touching `checked_artifact/` and landed without the Q1 shape, because the adopted R2-F charter (drafted after this acceptance) failed to route it: the lane owner's plan-drafting miss, recorded rather than papered over. The obligation now binds the PRIMARY carrier — the E4 resumption's first production train (E4.1) — and is written into the R2-E plan's E4 gate note so the train cannot open without seeing it. The shape is unchanged: implement, don't re-decide.]* |

**Completeness citations (State axis's sweep — present, needing only
citation):** the E3 landing range's four permanently-red intermediates with
the minted per-commit-green/squash lesson (ritual 7's arm, validated at the
E2 squash and reused at conf-integrity); E5's `a6ce8a8` latent-red J-7 catch
fixed at `fc0bb22`; the conf_gate Windows first-dispatch red fixed forward
at `cc7c625` (fixture-only); the operator agents-template commit `8c59521`
riding the E6 train per standing ruling; v0.11.1 complete across all three
channels with the PyPI runbook gap closed at source (`11bca66`).

## 8. The E7-dual queue — dispositions

All queue items are dispositioned; where both axes spoke, they converged
peer-blind:

- **Anchor nit 1** — Code Q1: **bound the shared reader itself**; closed
  re-owned with the named carrier and the full shape (§4 O12 row, §5.9's
  companion, §7's residual row). The State axis's requirement (take the
  decision or re-own with updated dated terms; never revive ":394") is met.
- **[R2-P3-2] / 17th key** — **both axes independently chose the
  named-exception branch**; Code refused the 17th key on substance (one
  idempotent self-converging rename; a key moves the frozen census for zero
  new crash-window evidence); the dated freeze annotation is executed
  (§5.7). Closed.
- **[R2-P3-1]** — Code Q3 confirmed zero reliants on the settled tree
  (compile-only shape aside); the clause now lives at the plan's E4 gate
  note (§5.1). Closed as an E4 scope clause.
- **E3 F7 / the convergence idiom** — Code Q4: acceptable as landed
  (converged-by-observation is the corpus-wide semantic; occupancy
  re-observed per drive); the terminal sibling clause written at E4's gate
  note (§5.8); any re-flush tightening is a corpus-wide hardening-lane
  decision. State's out-of-scope line concurs. Closed for R2-E.
- **[R2-P3-3]** — accepted residual, carrier restated (§5.2); rides the C1
  train (Code F2 concurs: fold into the train that edits the function the
  sentence mis-describes).
- **K/J(ii)** — State: **SUSTAINED** on second-axis scrutiny, no surviving
  corpus contradiction. Closed.
- **Tier-2 sub-surface** — both axes confirm deliberately unminted, rightly
  (amendment-with-dual required; §6.3's own rejected-alternative warning);
  two-part encumbrance stated at O8. Closed for R2-E, carried at §7.
- **Citation-drift rule** — State: PASS on spot-check. **E6 F-3's
  authorization reading** — State: sustained, no further act owed.
- **Code F1** (the announcement universal's weak reading) — cured by §5.7's
  freeze sentence; the optional in-code clause declined there and carried.
- **Code F3** (residue's family gate charges after the read) — recorded as
  one class with Q1's cure; the stat-level reorder rides the C1 carrier
  (§7).

## 9. Acceptance

**R2-E IS ACCEPTED — 2026-08-29.** Both dual-#2 axes returned
GO-WITH-CONDITIONS in round 1 with zero P0/P1 findings between them (one
State P2, folded at §5.1; four Code P3 + seven State P3, every one
dispositioned above); all conditions were lane-owner folds executed in this
record, the plan, and the freeze, plus the driver commit of §2 — the
two-round remediation cap was never engaged. The milestone "R2-E accepted"
(plan §3 Phase E7) holds against the §1.1-amended object, with the ledger
closed as §4 records, this record's acts §5 executed, and the residual set
carried as §7 names. **The review-debt ledger empties at this close** (plan
Phase E6 annotation, F-7): every riding item is either executed above or
re-owned with a named carrier and recorded terms.
