# M5b-IMPL settled review (round 1 of 2)

Date: 2026-08-24. Review: **M5b-IMPL SETTLED** — the review the frozen
dependency statement owes before A1 (`GwzM5-8M5bNoFfDesign.md:976-1003`,
including the 2026-08-24 annotation), unblocked by the R4b-G acceptance and
carried as the owed-pre-A1 row in the pre-A1 queue
(`CurrentProgramCheckpoint.md:702-708`). Round 1 of 2. This review's GO is
an explicit input to the A1 activation review.

Object: **the M5b no-ff semantics installation as it stands on the accepted
tree — gwz-core `1bd885f` (the R4b-G accepted tuple's gwz-core member,
restated literally at close: gwz-core `1bd885f` / gwz-cli `3cca145` /
gwz-py `929efb0` / taut `f008419`)**: the five M5b surfaces named by the D3
duals (`v1_lifecycle/tests/{forward.rs, mod.rs, no_ff_determinism.rs,
no_ff_wire.rs, reverse_no_ff.rs}`) plus the sixth package file
(`merge/store/tests.rs`) and the acceptance-train scan/CI additions; their
bound proofs; the §8.3 zero-production-line ceiling with tripwires T-1..T-6
(T-6 = the §3.5a "v0 forged-action resume gate" package's landed gate); the
clean-tree re-cut; and the no_ff mode's reachability status (v1 remains
`cfg(test)` until the A1 activation review).

Governing terms: design §8.1 item 3 (`GwzM5-8M5bNoFfDesign.md:950-955` —
suites green, ledger row reconciled, T-1..T-6 in the gate with T-6 bound to
the landed §3.5a package, checkpoint updated, freeze tuple re-cut against a
clean committed tree) and the frozen dependency statement §8.2 (:976-1003).
Contract set read: `GwzM5-8M5bNoFfDesign.md` (§2, §3.5a, §5.4, §6, §7, §8,
§9, dated annotations), `CurrentProgramCheckpoint.md` (M5b park/acceptance
records :74-118, J-1 adjudication + leg-4 restatement :643-664, R4b-G dual
and close records :666-714, M5b-IF freeze record + 2026-08-24 annotation
:1164-1188, ARM fixture package record :320-338),
`GwzM5-8R2DSettledTuple.md` (§5.1 rows 3-4, §11.1),
`GwzM5-8D3Impl-Review{Code,State}.md` (five-surface byte-identity records
:447-448 / :550-552), `GwzM5-8M5bImpl-Review{Code,State}.md` (routing
rulings), `GwzM5-8V0ForgedActionGate-ReviewState.md`,
`GwzM5-8ChangeBudget.md` (M5b row :828-852; section census),
`GwzM5-8I2CompatibilityContract.md` §8/§9 (:260-280),
`GwzWindowsMatrix-Classification.md` (:74-79).

Instruments and host: Darwin 25.5.0 / arm64, rustc+rustfmt 1.95.0 (the CI
pin), Python 3.13. All cargo results below were produced twice: first in
the live checkout, then — after the working tree went dirty mid-review
(finding [P3-2]) — re-derived from a **pristine read-only extraction**
(`git archive 1bd885f | tar -x`, no git write) with a fresh
`CARGO_TARGET_DIR` under this session's scratchpad
(`.../scratchpad/m5b-settled-target-pristine`). The pristine numbers are
the evidence of record. gwz-core HEAD = `1bd885f` throughout; no git write
operation was performed; this report is the review's only file write.

---

## 1. Object identity and tree condition

- gwz-core HEAD is exactly the accepted tuple pin `1bd885f`
  (`1bd885fcdf36183fcf5500cb3823858a8edcd574`); at review start the
  working tree was clean. **Mid-review it went dirty** with an in-flight
  lane's uncommitted work (the C-1 closure package's shape: `M` on
  `scripts/checks/run_r4bg_aggregate_gates.py`,
  `src/workspace_ops/tests/g23.rs`, `g23/atomic_upgrade_v0.rs`,
  `g23/compatibility_v0.rs`, +63/−4; untracked new
  `g23/compatibility_residue_v0.rs`). The committed object is untouched
  (HEAD unchanged); all committed-blob diffs below are unaffected, and
  every executed gate was re-run from the pristine extraction. See [P3-2].
- Rust-tree identity to the standing full-suite evidence:
  `git diff b91bdeb 1bd885f -- '*.rs'` and
  `git diff 78badbc 1bd885f -- '*.rs'` are both **empty** — the tuple's
  Rust tree is byte-identical to run 22's object (`b91bdeb`, 1530/0/1)
  and to the clippy-green tooling train `78badbc`; `78badbc → 1bd885f` is
  exactly three `scripts/checks/*.py` files (+114/−17), as the R4b-G close
  record states.

## 2. Mandate 1 — the five surfaces vs their accepted state

Baseline: the acceptance shas on record, `3e60529` (park: +1,425/−27
across 7 files) and `8c1624a` (acceptance train: +58/−1 across 3 files —
the T-3 second scan in `no_ff_wire.rs` +47, checker +8, workflow step +4).
Diff of every package file, `8c1624a → 1bd885f`:

| file | delta | attribution |
| --- | --- | --- |
| `v1_lifecycle/tests/mod.rs` | byte-identical | — |
| `v1_lifecycle/tests/no_ff_determinism.rs` | byte-identical | — |
| `v1_lifecycle/tests/no_ff_wire.rs` | byte-identical | — |
| `v1_lifecycle/tests/reverse_no_ff.rs` | byte-identical | — |
| `merge/store/tests.rs` | **+2/−0** | `8b83a2c` (D3 landing): `noop_commit: None, reset_commit: None` at :475-476 — exactly the D3-forced round-1 marker lines already on record (`GwzM5-8D3Impl-ReviewCode.md:447-448`, site list :73-78). ACCOUNTED. |
| `v1_lifecycle/tests/forward.rs` | **+6/−0** | `c2d2f15` (ARM fixture package, landed 2026-08-22 direct-with-record per the fixture-class precedent, `CurrentProgramCheckpoint.md:320-338`): one `pin_fixture_boundary_mode(&root.path)` call + comment inside the `fixture()` helper — runner-class environment pin, no semantics. Landed **after** the D3 byte-identity check (consistent with D3's "all five byte-identical" record) and validated by the program's first green ARM run (32573547362, 1510/0/1). ACCOUNTED. |
| `.github/workflows/checked-artifact-boundary.yml` | byte-identical | the M5b push-lane tripwire step intact |
| `scripts/checks/check_checked_artifact_boundaries.py` | +131/−18 | shared instrument, not an M5b surface: eleven recorded trains (`558f834` … `78badbc`), including the R4b-G tooling that ADDS the F-3 call-graph guard M5b leans on (§4 below). |

No other commit in `8c1624a..1bd885f` touches any package file. **Every
delta is attributed to a recorded train; nothing unaccounted.** The four
untouched surfaces plus the two attributed test-only deltas mean the M5b
semantics installation is byte-wise the accepted one.

## 3. Mandate 2 — bound proofs re-run and the ceiling re-verified

**Bound proofs on the pristine `1bd885f` extraction (fresh target dir):**

- The four M5b filters (`v1_lifecycle::tests::{forward,
  no_ff_determinism, no_ff_wire, reverse_no_ff}`): **37 passed / 0
  failed** — the population is exactly the D3-cited 37 (22+4+6+5 per
  file); it has **not** grown across the six landing trains.
- T-6 (`workspace_ops::tests::g23::continue_v0_gate`): **3/0** — both
  named suites (`v0_resume_rejects_forged_two_parent_action_over_fast_forwardable_pair`,
  `v0_resume_rejects_forged_no_ff_mode_row`) plus the divergent-pair
  positive control.
- T-1 pin (`custom_messages_validate_while_no_ff_remains_reserved`) and
  T-2's no-ff body variant
  (`installed_but_disabled_v1_no_ff_body_reports_a1_before_any_row_is_read`):
  both green. Combined single run: **42 passed / 0 failed** (1,570-test
  lib population).
- Composition: merge-namespace partition **565/0** (984.5 s, includes the
  debug fault matrix) on this host; full-suite and clippy evidence carries
  by the zero-`.rs`-delta identity to run 22 (`b91bdeb`, 1530/0/1) and to
  `78badbc` (§1). Retained-reader lane green at `1bd885f` per the R4b-G
  close record. `cargo fmt --check` on the pristine tree: **clean**.
- Boundary checker (Python 3.13): `ok (15 visible entries, 5 classified
  modules)` + its unittest suite OK — includes the **F-3 v1_lifecycle →
  v0-persistence-seam call-graph guard**, which the checker marks
  "LOAD-BEARING FOR JUDGMENT CALL J-1" and derives from
  `merge/mod.rs`'s own re-exports, failing closed via
  `V0_PERSISTENCE_SEAM_FLOOR`.

**Zero-production-line ceiling on `1bd885f`, by the acceptance's own
instruments:**

- Non-test dep-info (pristine `cargo check --lib`): the production lib
  references **81** `workspace_ops/merge` source files with **zero**
  `v1_lifecycle` paths, **zero** `no_ff`-named files, **zero**
  `store/tests.rs` — the entire M5b surface is compiled out of the
  production binary (the test binary's dep-info mounts all 130
  `v1_lifecycle` refs, confirming the instrument discriminates).
- The §6 gate anchors verified byte-level on this tree: `#[cfg(test)] mod
  v1_lifecycle;` + compile sentinel (`merge/mod.rs:23-26`); the
  module-local dead-code allowance naming A1 (`v1_lifecycle/mod.rs:1-5`);
  `PRODUCTION_R3` + the `unreachable!` v1 arm
  (`record_wire/decode.rs:82-86`) with `open_v0`/`unknown_fields` under
  `cfg(test)` (`record_wire/mod.rs:13-23`); the `--no-ff` start rejection
  (`validate.rs:12-16`, pinned at :201); v1 `Serialize` only under
  `cfg_attr(test, ...)` (`model/v1/record.rs:19`).
- T-4's structural scan
  (`force_merge_commit_construction_sites_stay_v1_lifecycle_only`) and
  both T-3 scans green in the 37; the T-3 second scan's **16-file pinned
  NoFf surface held unchanged across all six trains** — no new file in
  the tree mentions the mode.
- The settled-tuple audit's independent measurement stands: M5b prod net
  **0** on both commits (`GwzM5-8R2DSettledTuple.md:375-376`, rows 3-4).

**Reachability status: the v1 no_ff mode remains `cfg(test)`-only on
`1bd885f`; the A1 activation gate holds.** The one production-compiled
no-ff-adjacent surface remains exactly the §3.5a-acknowledged consumption
lane, and it is CLOSED by the landed production gate
(`continue_op/execution.rs:6-18`, `reject_unsupported_legacy_v0`) — see
T-6 below.

## 4. Tripwires T-1..T-6 in the gate

- **T-1** start-rejection: present and pinned; green (§3).
- **T-2** production decoder rejects v1 with typed `required_wave: A1`
  context, no-ff body variant included: green
  (`store/tests.rs:142`, run §3).
- **T-3** writer/positive-fixture scan + the 16-file mention pin: both
  green on the tuple; the g23 negative-fixture allowlist intact; runs in
  the push lane via the workflow's M5b step (byte-identical since
  acceptance).
- **T-4** construction-site structural scan: green; CI-executed on every
  push (same workflow step); companioned since `78badbc` by the checker's
  F-3 call-graph guard (§3), which is the standing form of the R4b-G gate
  that reason (c) leans on.
- **T-5** retained-reader lane: the lane is in CI and green at `1bd885f`;
  the v0.10.2 envelope matrix rejects the v1 pair on both distributed
  surfaces with exact evidence. **However, the tripwire's named fixture —
  "the v1 no-ff envelope pair" — does not exist in the harness**: the
  `future-v1` open+archived pair is generated by rewriting the envelope
  of a normal-mode fixture (`generate_retained_reader_fixtures.py:
  340-370`), and it predates M5b entirely (`16c0f5b`, the I2 freeze). No
  fixture addition to the harness has occurred since. Substance vs
  letter: rejection is envelope-first per the frozen matrix
  (`GwzM5-8I2CompatibilityContract.md:266-273`), so a no-ff body cannot
  change the retained readers' verdict, and this tree's own reader proves
  the header-before-body pattern (T-2's test name says it: "…before any
  row is read"); the gap cannot manifest before A1 activates a v1
  writer. It becomes load-bearing exactly then. Finding **[P2-1]**.
- **T-6** the §3.5a package: landed `d70bd57` (2026-08-16, before the
  M5b park — the binding's "lands before M5b settled acceptance" is
  satisfied with margin), focused State review GO on record
  (`GwzM5-8V0ForgedActionGate-ReviewState.md`: 0 P0-P2, 3 P3
  observations); the production gate present at
  `continue_op/execution.rs:6-18` ahead of every side effect on the
  continue lane; both named suites + positive control re-run green on
  the pristine tuple (§3). **T-6 HELD.** One bookkeeping defect found in
  its paper trail: the Q8-ratified "its own budget row"
  (`GwzM5-8M5bNoFfDesign.md:1061-1065`) was never filed in
  `GwzM5-8ChangeBudget.md` (section census: R0…R4b-P0.2, M5b — no gate
  package row). The no-absorption substance holds — M5b's row charges
  0/0 production and the measurement confirms it; the gate's production
  lines are separately attributable to `d70bd57` and its GO review — but
  the ledger is incomplete against the ratified text. Finding **[P3-1]**.

## 5. Ledger obligations

- **ChangeBudget M5b row** (`GwzM5-8ChangeBudget.md:828-852`):
  reconciled. 0/0 production, 6 files, 1,424 (+~45 acceptance-train)
  test/tool lines vs the 1,450 ceiling raised and ratified on record;
  wire/protocol none/none; the two post-acceptance deltas on M5b files
  (§2) are other packages' recorded lines (D3 row 10; the ARM fixture
  package), not M5b charges. Nothing to restate.
- **Class-membership note (mandate 3; design §9 Q7 / §5.4)**: recorded
  verbatim in `GwzWindowsMatrix-Classification.md:74-79`, and **true on
  this tree**: `mode: no_ff` → `ForceMergeCommit` → prepared `Commit`
  whose execution materializes the worktree at the frozen tree
  (`merge_prepared.rs:328` `checkout_tree`) — reachable for
  otherwise-ff-able participants, `cfg(test)`-only pre-A1; both paths
  were already CRLF/materialization class members; the class-level
  instrument the note leans on exists and is live — the D1 creation-time
  filter neutralization in production
  (`git/gitbackend/repository_support.rs:28-33`) and the un-pinned CRLF
  doctrine sentinel (`git/tests/g12.rs:1034`, `#[cfg(windows)]` by
  design, unchanged since run 21's green Windows object `514f8e6`).
  Bookkeeping, not new exposure — exactly as recorded.
- **Checkpoint updated**: the acceptance record, the J-1 adjudication
  with its 2026-08-24 leg-4 restatement, the R4b-G close naming this
  review as unblocked-and-owed, and the §11.1 owed-pre-A1 row are all in
  place.
- **Clean-tree re-cut**: at acceptance, both reviewers' pristine
  extraction runs at `3e60529` (on record); at settle, this review's own
  pristine extraction of the clean committed `1bd885f` (§1) — HEAD is
  the tuple pin, and the re-cut discipline was re-executed rather than
  assumed, which the mid-review tree contamination made necessary.

## 6. Mandate 4 — the sequencing deviation's residue, judged from here

J-1 accepted the merge-ahead-of-gates with this review as the
remedy-in-standing. From this review's own vantage:

- **Reason (a)** (frozen resume order violated): the recorded violation's
  named remedy — this settled review running before A1 — is hereby
  delivered pre-A1. No further residue is possible from (a) beyond the
  standing record.
- **Reason (b)** (moving target / finding-ownership aliasing for R4b-G's
  settled-tree, call-graph, and byte-equivalence gates): closed empty on
  re-verification. M5b moved zero production bytes (prod net 0 on both
  commits, re-proven here by non-test dep-info); the five surfaces were
  byte-stable through D3 and to today except the two attributed test-only
  deltas (§2); R4b-G's PARTIAL byte-equivalence state (C-1, 13/39) is an
  M4-scenario-adaptation matter with no row touching an M5b surface — no
  finding aliasing observed in either R4b-G report.
- **Reason (c)** (M5b's unreachability argument leaned on R4b-G's
  call-graph gate, which was absent at merge time): the true exposure
  window was 2026-08-22 → 2026-08-23 (merge to `78badbc`). It closed
  empty: the F-3 guard now stands in the checker with an explicit
  "LOAD-BEARING FOR JUDGMENT CALL J-1 … must not be weakened without the
  R4b-G lane owner's ruling" covenant, holds at 0 hits including test
  code on the tuple, and the surfaces it protects did not change inside
  the window (§2). T-4's own scan ran green throughout via the push-lane
  step that has existed since `8c1624a`.

**Ruling: nothing in the M5b object needed a protection of R4b-G's that
it did not, in the end, receive.** The deviation's whole residue is the
record itself plus the one gate element this review found unevidenced in
its named form — T-5's fixture ([P2-1]) — which predates and is
independent of the deviation (assigned to the retained-reader lane at
acceptance, simply not yet delivered).

## 7. Findings

- **[P2-1] T-5's named fixture is absent**: the retained-reader harness
  has no `mode: no_ff` v1 envelope pair — the `future-v1` pair rewrites a
  normal-mode body and predates M5b (`16c0f5b`). Substance is proven
  mode-blind at the envelope layer on the green lane; the letter gap
  becomes load-bearing when A1 activates a v1 writer. Remedy (cheap,
  mechanical): add the `mode: no_ff` variant pair to
  `generate_retained_reader_fixtures.py` + `cases.json` (or record a
  lane-owner narrowing that the envelope pair subsumes it). **Bind to the
  A1 activation review's input register — recommended BLOCKING-FOR-A1
  alongside C-1** per the same L1-19 pattern. Not blocking this settled
  acceptance: the protected property cannot be violated on any pre-A1
  tree.
- **[P3-1] The "v0 forged-action resume gate" package has no
  ChangeBudget row**, contrary to the Q8-ratified "its own budget row"
  (`GwzM5-8M5bNoFfDesign.md:1061-1065`). No absorption occurred (M5b's
  0/0 row holds by measurement; the gate's lines are separately
  attributable to `d70bd57` + its GO review). Remedy: file the row from
  the `d70bd57` stat and `GwzM5-8V0ForgedActionGate-ReviewState.md`.
- **[P3-2] Tree condition**: the gwz-core working tree went dirty
  mid-review with an in-flight lane's uncommitted work (§1); this
  review's evidence was re-derived from a pristine extraction, and the
  committed object is unaffected (pristine `cargo fmt --check` clean —
  the fmt drift observed live sits only in the lane's uncommitted
  `g23/compatibility_residue_v0.rs`, 6 sites). Forward notices for that
  lane, non-gating here: the fmt gate lives only in `release.yml`, so
  unformatted landing bites at the next release cut; two of its edited
  files sit on the T-3 16-file pin allowlist (mentions must stay within
  the pin — its new file currently has zero `NoFf` mentions, so no trip
  as it stands); and reviews in this parallel-execution mode should
  pristine-extract by default, as this one did.
- **[P3-3] Attribution record (informational)**: the only
  post-acceptance deltas on the six M5b package files are
  `store/tests.rs` +2 (`8b83a2c`, D3's forced marker lines) and
  `forward.rs` +6 (`c2d2f15`, ARM fixture package boundary-mode pin);
  the other four surfaces are byte-identical to `8c1624a`. Recorded so
  the A1 activation review inherits the accounting instead of
  re-deriving it.

## 8. Verdict

**GO — the M5b-IMPL settled acceptance stands on `1bd885f`.**
0 P0 / 0 P1 / 1 P2 / 3 P3. Every §8.1-item-3 criterion is met on the
accepted tuple — suites green (37/0 bound proofs + T-6 3/0 + T-1 + T-2 on
a pristine extraction; composition by run-22 identity and the 565/0
partition), ledger row reconciled, tripwires in the gate (T-5 by its lane
with the letter gap filed as [P2-1] → A1 register; all others by direct
re-execution), checkpoint updated, clean committed tree re-cut — the
zero-production-line ceiling is re-proven by the acceptance's own three
instruments, the five surfaces are byte-accounted to the accepted state,
the Q7 ledger note stands true, and the J-1 deviation leaves no untreated
residue. The frozen dependency statement's owed review is discharged;
**A1 may take this GO as its input, carrying [P2-1] onto the A1 input
register beside C-1/C-2.**
