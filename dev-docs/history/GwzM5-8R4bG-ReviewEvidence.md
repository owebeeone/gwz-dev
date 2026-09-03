# R4b-G gate — EVIDENCE-axis review (round 1)

Date: 2026-08-24. Axis: **Evidence** (adversarial; round 1 of 2; program-level
dual gate, peer-blind — the Correctness-axis report was not read, sought, or
referenced; its existence alongside this report is what §8 confirms).
Object: **the amended evidence record `dev-docs/GwzM5-8R4bG-Evidence.md`
(as amended through gwz-dev `d378db9`: the executed inventory, the F-row
closures, the signed F-7 ruling, the §12 M4 enumeration) against the four-repo
tuple — gwz-core `78badbc`, gwz-cli `3cca145`, gwz-py `929efb0`, taut
`f008419`.** Contract set read end to end: `GwzM5-8R4bG-EvidenceInventory.md`
(with its bracketed corrections), `GwzM5-8ThinA1Amendment.md` (controlling),
`GwzM5-8R2DSettledTuple.md` (including the lifted perf-pricing row),
`CurrentProgramCheckpoint.md` RESUMED records (the R2-D SETTLED record, the
R4b-G evidence record with the J-1 adjudication, the run-22 dispatch line),
`GwzWindowsMatrix-Classification.md`, `GwzM5-8R0Inventory.md` §4/§5.2,
`GwzM5-8I2CompatibilityContract.md`, `GwzM5-8Refactor.md` §checkpoint-8/O1-O9,
`AgentProcessRules.md` §A.5/L1-19, `GwzM5-8ChangeBudget.md:505-556`,
`GwzM5-8M5bNoFfDesign.md:970-995`, `GwzM5-8OperatorEscapeDesign.md:751-754`,
`GwzM5-8R2DSettled-Review{Code,State}.md`, `GwzM5-8D3Impl-Review{Code,State}.md`,
`GwzM5-8ThinA1Amendment-ReviewSafety.md`, `GwzM5-8ProgressReviewF5.md`.

All commands ran on this host (Darwin 25.5.0 / arm64, Python 3.13, rustup
1.95.0) at the tuple pins with
`CARGO_TARGET_DIR=<scratchpad>/r4bg-state-target` (fresh — every cargo result
below is a from-scratch build, not the lane's cache). Tuple verified:
gwz-core HEAD `78badbc` = origin/main, clean; gwz-cli `3cca145`, gwz-py
`929efb0`, taut `f008419`, all clean. `git diff b91bdeb..78badbc` is exactly
the five `scripts/checks/` files, +582/-0, zero `src/` lines — so every gate
executed at `b91bdeb` is valid at `78badbc`, and I additionally re-executed
the train at `78badbc` itself. No git write operation was performed; this
file is the only file written.

---

## 1. Verdict

**NO-GO (round 1 of 2) — one P2, six P3.** The single P2 is a text-class
record-integrity defect in the J-1 lane-owner adjudication (an evidentiary
claim the underlying reviews do not contain), remediable in one checkpoint
edit plus re-verdict; under L1-19 the gate is NO-GO while it is open. **No
finding here touches production code, and no mechanical claim in the evidence
record failed reproduction.** Everything I re-executed reproduced exactly:
the full 1569/0/1 lib suite in four partitions (including `root_fault_matrix`
release, green at 352.48 s on this host), 50/0 integration, 111/0 g23, 400/0
checked-artifact, 254/0 v1_lifecycle, 75/0 record_wire, boundary checker
`ok (15 visible entries, 5 classified modules)`, the F-2 probe suite 5/5
(73.0 s), all four F-3 fail-closed tests (compiler probe included), the F-4
driver verbatim (markers intact, REVIEW never green), F-5 in both directions
(old form exit 101 with the same 7 sites in the same order; both corrected
lines exit 0), `regen.py --check` OK, `d0_roundtrip_check.py` all passed,
gwz-cli 139/0, gwz-py cross-driver 19/19 (14+5, parametrized — the counts are
collected-test counts), fmt clean, clippy clean. Every CI id in §3.5
verified by API: boundary `32620718426` and retained-readers `32620718427`
at `b91bdeb` success; run 22 = Windows matrix `32672188425` at `b91bdeb`
success with Platform sibling `32672189557` success (arm 1569/0/1, macos
1580/0/1); runs at `d45458d` success; run 21 pair at `514f8e6`; the
boundary/retained RED pair at `514f8e6` (`32616865440`/`429`) corroborated.
**The push lane is additionally green at `78badbc` itself**
(`32676581759`/`32676581803`), which runs the F-3 guard and its four
fail-closed tests at the exact review object.

Where the record errs, it errs in **both directions**: one signed
adjudication overstates its evidence (P2-1), while the §12 enumeration
**understates** the frozen compatibility contract's categorical disposition
of the unbound scenarios and consequently over-grades its own residual
(P3-1). The verdict-relevant negative results (13/39 bound; the FAIL rows
left FAIL) are honestly stated and arithmetically exact.

---

## 2. Findings, most severe first

### [P2-1] J-1 acceptance record, leg 4: "the settled dual's Code axis re-verified the M5b surfaces byte-identical" is not verifiable on the record

`CurrentProgramCheckpoint.md:643-657` (LANE-OWNER ADJUDICATION OF J-1, "on
record for the dual") rests ACCEPTED WITH RECORD on four legs. Legs 1-3
verify: **leg 1** — M5b prod net 0 for both commits, independently measured
(`GwzM5-8R2DSettledTuple.md:375-376`, rows 3-4: `3e60529` 0/+1398, `8c1624a`
0/+47), and `3e60529`/`8c1624a` confirmed ancestors of `b91bdeb` by
merge-base; **leg 2** — M5b ACCEPTED 2026-08-22 GO/GO round 1 on both axes,
each axis's single P2 closed in the acceptance-train dispositions
(checkpoint :95-118); **leg 3** — the F-3 masked-token scan exists at
`78badbc` in `check_checked_artifact_boundaries.py`, carries the
load-bearing-for-J-1 comment, runs per push in
`checked-artifact-boundary.yml` (green at `78badbc`), and its four
fail-closed tests pass here. **Leg 4 does not verify**: the strings `M5b`,
`no_ff`/`no-ff`, `3e60529`, and `8c1624a` appear **zero times** in
`GwzM5-8R2DSettled-ReviewCode.md` and `GwzM5-8R2DSettled-ReviewState.md`
(rounds 1+2, whole files); `v1_lifecycle` appears zero times in the Code
report. Neither settled report performed or records any byte-identity
verification of M5b surfaces. The nearest true facts are indirect: the Code
axis re-ran the full lib suite (M5b's suites inside it) and the boundary
checker whose `PROTECTED_SOURCE_TREE_DIGESTS` pin spans the `v1_lifecycle`
tree — an inference chain, not the stated verification.

**Ruling on J-1 itself: the deviation is fairly stated (REAL, sequencing
artifact of the 2026-08-22 acceptance train) and is NOT disqualifying** —
legs 1-3 stand independently, reason (b) of the must-wait clause is defused
by the measured zero-production surface, and reason (c) is answered by the
standing F-3 guard. The M5b-IMPL-settled-review-before-A1 remedy is **not**
triggered by this axis. What must happen before the gate closes: **restate
leg 4 against evidence that exists** (tuple LOC rows 3-4 + the settled
dual's full-suite re-execution + the v1_lifecycle tree-digest pin) **or
strike it**, and complete J-1's own bookkeeping asks on ratification
(dated annotation on `GwzM5-8M5bNoFfDesign.md:976-986`; strike or annotate
the live "IMPL merge waits for R4b-G" sentence, now at
`CurrentProgramCheckpoint.md:1118`).

**What breaks if unaddressed:** the A1 activation review consumes a signed
deviation-acceptance whose fourth evidentiary leg cites a verification no
report contains; if the acceptance is later challenged, the record looks
fabricated at exactly the point where the program's largest ordering
deviation was excused. A gate that closes on evidence cannot carry it.

### [P3-1] R-1 re-graded P3, not P2: §12's UNBOUND disposition omits the frozen contract's categorical clause, and the residual is registry/record work, not an unproven behavior

§12.6 hands the dual "R-1 (P2 candidate): 22 named M4 scenarios have no
registry row". The measurement is correct (verified below, §4), but the
grading frame omits `GwzM5-8I2CompatibilityContract.md:117-125` and
`:159-165` (frozen, accepted): "**A1 deliberately whitelists only seven**
one-member-workspace, `Finalizing`, normal-mode shapes … Marker/lock-only
prefixes, multi-member workspaces, selected root, born root, drift, pending
actions, recovery, preservation, rollback, and **terminal rows are not A1
migration rules**"; "**Zero whitelist matches is not an error.** Open
read-only status leaves bytes unchanged … Completed and aborted v0 records
remain v0 and use byte-preserving archival. Archived v0 uses only the
archive decoder/projection". Every one of the 22 UNBOUND rows falls in a
named exclusion class of that clause (A-* pre-acceptance; F-BASELINE/
MARKER/LOCK = marker/lock-only prefixes; G-VERIFYING outside the seven;
H-* preservation; I-EVIDENCE-ROLLBACK rollback; K/M terminal; the ten
archive shapes under the archive-projection clause), and 18 of the 22 carry
the v0 byte/restart pins §12.3 itself names, executed green here inside g23
111/0. So O8's substance for the unbound space is: contractually
dispositioned fail-closed by class + byte-pinned per shape; what is missing
is the **per-scenario record** (registry rows or an O8 acceptance note
citing the contract clause per row) so the disposition is machine-visible
where `check_merge_compatibility_predicates.py` looks. That is P3 under
L1-19 (bounded coverage/record defect, concrete consequence), not P2.
**One row needs a real answer rather than a class cite:**
`J-NO-PUBLICATION-UNBORN` — whether the whitelisted no-publication
descriptor matches the unborn-root twin is untested per-case; it is the
only UNBOUND row whose class membership is ambiguous. Row 2.3b/O8 stays
FAIL as the record states; the fix is disposition-recording, and the
operator may equally route it with the acceptance debt to the A1 activation
review — but by a named clause, not silently.

**What breaks if unaddressed:** O8's acceptance basis stays unrecordable
per scenario; a future adaptation (or descriptor widening) of an unbound
shape has no registry row to collide with, and the A1 review inherits a
"P2 candidate" label that overstates the frozen contract's actual posture.

### [P3-2] R-2 ratified at P3: four scenarios have no durable fixture and cannot be dispositioned either way

`B-NOT-STARTED`, `B-PREPARING-EMPTY` (R0 §4 row B's own recorded gap,
verified still true — no `NotStarted` injection exists in g23 outside
`fixtures.rs`), `AC-NOPUB-UNBORN`, `AP-PRESERVED` (live-workspace GC
evidence verified present at `characterization_preservation_v0.rs:442` and
`gc.rs:46`, but no archive-projection fixture, exactly as R0 §5.2 records).
Fixture them or route them, named, with the acceptance debt.
**What breaks:** the four shapes stay outside every matrix forever, and
nobody can say whether v1-era code even preserves their bytes.

### [P3-3] The F-7 ruling's signature is mis-dated and its lift left four now-false statements standing in the record

The RULING line ("**SIGNED by the lane owner, 2026-08-23**") and the lifted
tuple row ("PRICED-NOT-OPTIMIZED, ruling signed **2026-08-23**",
`GwzM5-8R2DSettledTuple.md:666-671`) both carry a date on which neither
existed: both landed in gwz-dev `d378db9` (2026-08-24 10:24), and the ruling
cites the driver's 327.6 s corroboration, measured 2026-08-24. The row also
sits inside the block headed "Added at the settled-dual round-2 remediation
(2026-08-23)". The same commit leaves standing, un-annotated: §9.2 F-7
("The RULING line is **unsigned** and the tuple row is **not inserted**"),
§13.4 ("Sign no ruling"), and §8 item 4 ("5 bullets … **25 items** … no
later commit touches the file" — now 6 bullets, 26 items, and `d378db9`
touches the file). Additionally the "~343 s release" expectation F-7 exists
to correct is attributed to "the A1 brief" with no in-corpus citation — the
one unsourced authority reference in §8; name the brief or mark it
out-of-band. On the substance the ruling is **honest and now
corroborated**: 318.71 s (release, `b91bdeb`) as the citable price, 576.03 s
retained as historical — my independent release run at `78badbc` measured
**352.48 s green**, inside the 319-353 band and decisively not 576; and the
routing is licensed (`GwzM5-8ThinA1Amendment.md:43-55` routes the
acceptance debt with the D3 named residual to the A1 activation review;
`:242-244` restates it; L1-19 at `AgentProcessRules.md:392-401` makes
branch (b) exact — "priced-not-optimized under branch (a)" is a disposition
that clause structure supports the lane owner signing).
**What breaks:** the A1 review consumes a register row whose provenance
date git contradicts, and a reader of §9.2/§13.4 alone concludes the ruling
is still open.

### [P3-4] Ledger and self-description inconsistencies inside the remediation record

(i) Two stale line counts: the driver is **150** lines and the map checker
**144** (both `wc -l` and the `78badbc` diff), but §9.2 F-4 and inventory W5
say "139 lines" and §6 W4 says "133 lines"; §13.1's table (144/138/76+74/150,
summing to the verified 582) is the correct one. (ii) Charging convention:
pass 1 declared "this document spent 0 of all five" for an 851-line evidence
document; pass 2 charged 595 doc lines for its own additions to the same
document (`d378db9` numstat: evidence +552/-25, inventory +73/-5 — net 595,
verified). Under uniform charging the package's test/tool/doc column reads
~2,028 > the ≤1,500 ceiling; under the program's review-artifact precedent
(review reports and the settled tuple are never charged) it reads 582; the
recorded 1,177/78% is neither convention. The §7.5 step-8 column-order
correction itself is **verified right** (`ChangeBudget.md:507`), and the
0-production / 0-moved / 0-production-file columns are diff-verified.
**What breaks:** the ledger row the operator ratifies is arithmetic nobody
can reproduce without guessing the convention.

### [P3-5] The F-4 driver's fault battery under-drives row 2.1's own evidence basis

Row 2.1's PASS rests on the full four-partition train including
`checked_artifact::` 400/0 (the 165-key fault census) and the 914-test
remainder; the driver's `fault` battery runs only the two v1_lifecycle
commands (254 + 1). A future solo driver run prints a green `fault` battery
while executing ~16% of the tests row 2.1 counts as the aggregate fault
gate. The record executed everything by hand (§3/§13, and I reproduced it),
so this is a standing-tooling scope gap, not an evidence gap today. Extend
the battery (or state the scope in the driver header and the W5 row).
**What breaks:** the next operator of the "one-command driver" under-verifies
the fault gate without knowing it.

### [P3-6] Bookkeeping cluster (one sweep)

(i) The checkpoint's §"Exact tuple" gwz-core row (`:817-823`) was corrected
to `b91bdeb` at `d5967c9` and is already one commit stale again — origin/main
is `78badbc`; the J-6(i) drift class recurs the same day it was closed.
(ii) §7.2's "current" anchors for the boundary checker are stale at the
review tuple because F-3's own +43-line insertion moved them again:
`source_tree_digest` `:832`, `calls()` `:851`, forbid-verification `:902`
at `78badbc` (row 2.6a cites :789/:808/:845). (iii) §5's bonus-evidence
claim "the only callers are …tests_provider.rs:168,180 and
…bootstrap/tests.rs:147,194" undercounts — `bootstrap/tests.rs` also calls
`recover_or_create` at `:213` and `:236`; the property (no production
caller) still holds. (iv) The F-6 closure record is silent on
`linux-identity-probe` (named inside F-6's row, last run `f36d20d`); the
workflow is dispatch/path-triggered only, so staleness is a dispatch
decision — state its disposition (R5-class native evidence, post-A1) in one
line rather than leaving it silently uncovered. (v) "Run 22" is the
program's ledger numbering; the GitHub run_numbers are 32 (Windows) and 13
(Platform) — one clarifying line in the classification doc prevents a
future auditor concluding the id is wrong.
**What breaks:** each item is exactly the mislead-the-A1-review class the
evidence doc's own J-6 exists to prevent.

---

## 3. Mandate 1 — verdict-row verification

**PASS rows: 22/22 examined, 17 by re-execution, the rest by cite+CI.**
Sampled deeply (re-run or byte-verified): 2.1 (four partitions re-executed
green here: 400/0 + 914/0/1 + 254/0 + root_fault_matrix 1/0 release; sum
1569/0/1 = the record, over the verified 1570 listed), 2.2 (both scripts +
both suites green via the driver; registry content 7/7/6 with the exact
case ids; `include_str!` at `compatibility_v0.rs:10`; adapter consumption
at `open_v0/adapter.rs:115-117`; retained-readers green at `b91bdeb` and
`78badbc`), 2.3a (all seven fixture-corpus ids present verbatim; the
`:451` byte-assert `accepted.lock.exact_yaml == candidate.lock_yaml` cite
exact; g23 111/0 re-run), 2.4 (75/0 re-run; `matrix.rs:18`, `drift.rs:16`,
`store.rs:142` all exact), 2.5a (`transition/mod.rs:20`, `authority.rs`
:28/:158, `store/rewrite.rs` all-`pub(super)` verified; checker ok; the
65→69 suite green in CI at `78badbc`), 2.6a (clippy.toml **19** entries with
the three gwz-core writers at :21-23; clippy `-p gwz-core` + CONF_DIR clean
from a fresh target), 2.8 (3/9/7 counts verified in the list; green inside
the 254 partition), 2.9a (12/3/7/1/29 counts verified; gwz-py 19/19
executed), O1, O2, O3 (all model cites exact, `Serialize` strictly
cfg(test)-gated: 11 `cfg_attr`, 0 unconditional), O5, O6, O9 (all named
tests exist at the cited lines and ran green), O7 (`v0_common_view` 0 hits;
`build_v1_acceptance` at `v1.rs:52`; `finalize_dispatch.rs:20` exact),
runbook steps 1, 2, 4, 6 (regen OK, d0 OK, gwz-cli 139/0 re-run), 8 (with
P3-4's caveat), 9. The bonus evidence verified: `merge/mod.rs:23-24` is
`#[cfg(test)] mod v1_lifecycle;` (with a compiler-root sentinel), and
`recover_or_create` has no production caller (P3-6(iii) count nit).

**RE-FRAMED rows: 6/6 verified licensed by the amendment's actual clauses.**
2.7a/F1 → §2.2 (`:73-78`) + §2.3 (`:79-82`) supersede the R6 wait verbatim;
the thin-A1 equivalent exists (R2-D settled GO/GO at `b91bdeb`, checkpoint
:606-631). F2 → §2.4 (`:83-91`) replaced the SCOPE CORRECTION in the
amendment's checkpoint commit. F3 → §1 (`:29-34`) + §4 (`:236-239`); both
substituted preconditions verified satisfied (checkpoint :629-631), and the
whole-criteria-1-14 release is licensed by §2.1's "gated by the R2-D
settled gate alone" (criteria 12/13/14 verified to be R3/R5/R6 deliverables
at `RemPlan-4:1231-1244`). F4 → §1; the realized-R2-D-leg reading is
consistent with the re-baselined counts I verified. F6 is a factual
re-frame, re-verified here (clean tree, checker ok, 1570 listed). No
re-framing exceeds what the amendment licenses; W3/F5's citation set
(:73-78/:79-82/:113-119) is slightly imprecise — the direct clause for
"R3 is post-A1" is §1 `:31-32`/§2.1 `:69-72` — but the supersession is
licensed either way (not a finding).

**DEFERRED rows: 2/2 citations verified.** 2.9b → amendment `:43-55` and
`:242-244`, both quoted faithfully; the §8 item-1 record exists
(`ProgressReviewF5.md:56-59` verbatim, arithmetic 3,545 checks;
ReviewSafety P1-1 at :120ff, RESOLVED at :382ff). W3 →
`RemPlan-4:1087-1101`/criterion 12; `RollbackPrefixIssuer` 0 hits in src;
the current seam cite `rollback_prefix.rs:240` is exact; J-5's
recorded-DEFERRED disposition is the correct reading, confirmed.

## 4. Mandate 3 — F-1's negative result

The enumeration is **faithful and complete against the defining document**:
R0 §4 names exactly 29 progress shapes across rows A-M (5+4+1+1+1+4+2+3+2+
2+2+1+1) and §5.2 exactly 10 archive ids; §12 carries all 39, name-for-name,
no invention, no omission. Sampled 11 scenarios end-to-end: B-NO-PUBLICATION
-ROW, B-VALIDATING, C, D, E, F-BOUNDARY (the seven-window array at
`characterization_v0.rs:24-102` with the exact window names),
J-NO-PUBLICATION-BORN (the eighth `FinalizationFault` variant driven at
`:467` — and §7.1's eight-not-seven correction verified at
`fixtures.rs:15-24` vs the seven-element array at `finalization.rs:4-13`),
G-COMPLETE-PRE-ARCHIVE (shares `terminal/completed`, reason verbatim in the
registry), I-PARTICIPANT-ROLLBACK (`rollback/participant`, reason verbatim),
K-COMPLETED-NOPUB-OPEN (`characterization_publication_v0.rs:250` — the
closes-R0-row-K claim is real), B-NOT-STARTED (NO FIXTURE verified — no
injection exists), AP-PRESERVED (both live-GC evidence tests exist; archive
projection absent as claimed). The 13-bound arithmetic is exact: 7
fixture_corpus rows bind 7 shapes 1:1; 6 valid_unlisted rows bind 6 shapes
via the stated 2:1 (`terminal/completed`) and 1:2 (`L-RECOVERY-OVERLAY`)
mappings. `check_m4_scenario_map.py` is genuinely fail-closed (absent map
exits 1; markers required; tests existence-checked against `--list`;
registry ids claimed bidirectionally) and reports
`ok (39 scenario rows, 38 named tests, 13 registry rows all claimed)` here.
**Grading: R-1 → P3 (finding P3-1, with the contract-clause reasoning and
the J-NO-PUBLICATION-UNBORN carve-out); R-2 → P3 (finding P3-2).** The
record's refusal to flip 2.3b/O8 to PASS is correct and is the honest
reading of a measurement that came back negative.

## 5. Mandate 2 — the F-row closures

**F-2 (W1): closed as required, and slightly stronger.** The probe suite is
what the battery text asks (unnameability from the most-privileged outside
position implies it everywhere — `v1_lifecycle` is private to `merge`,
verified), asserts E0603 + the specific private module, seals all three
perimeters separately, proves the raw writer sealed against the rest of the
lifecycle too, and carries the positive control that makes the negatives
non-vacuous. 5/5 in 73.0 s here. The E0603 module-privacy assertion covers
every token in `authority`, so the single-token sample is not a coverage
hole. J-4's spent-hedge reading is sound.

**F-3 (W2): closed as required, stronger than the inventory asked.** Seam
derived from `merge/mod.rs`'s two `use store::{…}` re-exports (9 names,
verified at :50/:81), 6-name floor fails the derivation closed, masked scan
over all `v1_lifecycle/**/*.rs` including the `#[path]`-mounted test files,
the ten `enter_finalizing` string literals correctly masked to zero, four
fail-closed tests including the compiles-then-rejected probe — all four
re-executed green here — and it inherits per-push CI (green at `78badbc`).
The not-subsumed-by-digest argument is correct. One theoretical residue for
the Correctness axis, not graded here: an `as`-aliased re-export would drop
the alias name from the derived seam (`split()[0]`).

**F-4 (W5): closed; scope note filed as P3-5.** Seven batteries = the seven
named gates; markers enforced (a wrong count fails a zero-exit command);
the seventh is never counted green; PARTIAL semantics verified by
execution here.

**F-5: closed exactly as recorded.** Old single line reproduces exit 101
with the identical 7 sites in the identical order; both corrected lines
exit 0 from a fresh target; no `allow`/`expect` and no gwz-cli or root
`clippy.toml` (re-verified); the correction is recorded in place in the
inventory with the reason. J-3's cheap answer is taken and recorded, and
the perimeter-extension option is genuinely left open.

**F-6: closed for what it names about the matrices; one silence (P3-6 iv).**
Run 22 (`32672188425`) verified at `b91bdeb`, success; 1530/0/1 per the
ledger (`48ee7fe`, classification doc); Platform sibling (`32672189557`)
verified success at `b91bdeb`. The tuple's round-2 [P3-4] ledger duty is
discharged. `linux-identity-probe` remains at `f36d20d` with no recorded
disposition.

**F-7: the signing's substance is right; its provenance is defective
(P3-3).** 318.71 s-citable / 576.03 s-historical is the honest reading —
independently corroborated here at 352.48 s release, `78badbc` — and the
priced-not-optimized routing to the A1 activation review is licensed by
amendment §1 (`:43-55`, the D3 named residual inside the acceptance debt)
with `:242-244` and L1-19 making the branch structure exact. The signature
date and the un-swept "unsigned" statements are P3-3.

## 6. Mandate 4 — J-1

Ruled in §2 [P2-1]: deviation REAL and fairly stated; legs 1-3 verified on
the record; leg 4 unverifiable as worded; **not disqualifying** — the
before-A1 remedy is not triggered; leg-4 restatement plus the two J-1
bookkeeping annotations are conditions of gate close.

## 7. Mandate 5 — A1-input completeness

**Item 1 (acceptance debt):** PRESENT — routing clauses verified verbatim;
figure source verified verbatim with checking arithmetic; L1-19 verified;
the checkpoint remains pointer-only (§8's own "gap, minor" is accurate and
J-6(iv) already owns it). **Item 2 (D3/D4 disposition):** PRESENT as an
obligation; the live-code check verified exactly at `78badbc`
(`stash_complete` :283, `reset_complete` :310, surviving dirty-equality at
:305-306/:332-333, third site at :498, `:277-345` still bracketing);
the record's own caveat that the Safety review's per-line cites predate D3
is correct and correctly disclosed. **Item 3 (perf pricing):** now PRESENT —
drafted, signed, and lifted into `GwzM5-8R2DSettledTuple.md:666-671` where
the A1 review will look, with the P3-3 provenance defect. **Item 4
(register):** PRESENT and current at the lift; the evidence's §8-item-4
counts (25 items, 5 bullets, "no later commit") are stale post-lift —
P3-3's sweep. **Acceptance-object statement:** both unblock claims verify
against their frozen sources — (a) M5b-IMPL settled review:
`M5bNoFfDesign.md:976-989` verbatim and checkpoint `:1116-1118` live;
(b) escape implementation packages: `OperatorEscapeDesign.md:751-753`
verbatim and checkpoint `:1016`. The (c) A1 claim matches checkpoint
`:629-631`. What R4b-G does **not** accept (§1.3) matches the amendment's
§4 unwaived list exactly.

## 8. Mandate 6 — the gate's closing condition

The condition is "two independent settled-tree reviews with no open
P0/P1/P2" (`ChangeBudget.md:553-556`) in the full-tree form
(`AgentProcessRules.md:2006`). **This report is the Evidence-axis member of
that pair, round 1 of 2, filed against the full R4b object at the tuple
above; with the Correctness-axis report (confirmed by existence only —
peer-blind) the pair is constituted.** Open P-levels from the lane reviews
of record, read against the R4b object: **D3-impl dual** — GO/GO, both Code
P2s closed at round 2 ("GO — final and unconditional"), nothing open at
P0-P2; **M5b dual** — GO/GO round 1, both P2s closed at the acceptance
train, P3s file-and-continue; **R2-D settled dual** — GO/GO round 2, zero
open P0/P1/P2, named P3 residue (of which [P3-4] is discharged by run 22
and the L2-05 CI-wiring item remains open in the tuple register). The
**accepted-real R4b D3/D4 P2 classes remain open** against the
`v1_lifecycle` tree but are routed off R4b-G to the A1 activation review by
amendment §1/§4 as a named exception — they are not open against the R4b-G
object as re-framed, and this review does not re-open them. This round adds
**1 P2 (P2-1) + 6 P3 (P3-1..P3-6)**; the gate closes when P2-1's
remediation lands and both axes' rounds complete with nothing open at
P0-P2.

---

## 9. Round-1 disposition

NO-GO on P2-1 alone. The remediation is document-class and narrow: restate
or strike J-1 leg 4 with the two J-1 annotations; sweep P3-3's provenance
corrections in the same pass if convenient. P3-1/P3-2 are the dual's
recorded grading of R-1/R-2 and require only the lane's acknowledgment (or
the operator's routing) — they do not block under L1-19. On a focused
re-verdict after that remediation, nothing this axis has seen prevents GO.

---

# ROUND-2 FOCUSED RE-VERDICT — 2026-08-24 (round 2 of 2, final)

Object of the re-verdict: the merged round-2 remediation — gwz-core
`78badbc` → **`1bd885f`** ("Harden the R4b-G gate tooling per the dual's
round-2 prescriptions"; verified: exactly three `scripts/checks/` files,
+114/−17, zero `src/` lines, zero `PROTECTED_*` pin lines in the diff,
HEAD = origin/main, tree clean) and the record remediation at gwz-dev
`aeb4411`. Everything mechanical below was re-executed on this host at
`1bd885f` under the same fresh scratch `CARGO_TARGET_DIR`.

## VERDICT: **GO** — final for this axis. P2-1 CLOSED; all six P3s closed
or validly dispositioned; no new finding at any P-level.

**One-paragraph justification.** The single blocking finding is closed at
its root: J-1 leg 4 is restated at both annotation sites to its true source
— the **D3 dual's round-2 re-verdicts**, whose cites I verified resolve and
carry exactly the missing fact (`GwzM5-8D3Impl-ReviewCode.md:447-448`: "all
five M5b files byte-identical vs `0b059e8`; `store/tests.rs` exactly
+2/−0"; `GwzM5-8D3Impl-ReviewState.md:550-552`: "M5b four-filter 37/0 with
all five M5b files `git status`-clean (byte-identical)") — with the
mis-attribution bracketed on the record ("the fact was true, its citation
was not"), and both J-1 bookkeeping asks landed (the frozen design's
`:976-986` annotation; the checkpoint's `:1147-1157` retained-not-struck
annotation preserving the owed-pre-A1 M5b-IMPL settled review). The
reconciled C-1/C-2 record (§12.7, mirrored into the settled tuple's §11.1
as a new dated block) is evidence-sound: it credits and quotes the
categorical contract clause this axis found, names the four live-residue
rows with finder attribution (my `J-NO-PUBLICATION-UNBORN` plus the
Correctness axis's three finalizing-state `F-*` shapes, whose
adaptation-disposition hazard is a genuinely sharper argument than my
class-cite carve-out), grades the residue **P2 OPEN BLOCKING-FOR-A1 per
L1-19**, and routes it under the accepted amendment's own debt clause
(`:43-55`/`:242-244`) with pre-A1 exposure mechanically ZERO — all three
`cfg(test)` gates verified in code by me (`merge/mod.rs:23-24`,
`record_wire/mod.rs:13-14`, `merge/mod.rs:44-50`). Decisively, §12.8 puts
the **PARTIAL** statement of the byte-equivalence battery inside the
acceptance record itself, so the gate closes telling the truth about O8
(13/39; row 2.3b/O8 stays FAIL) rather than over it — the named-exception
structure the accepted amendment already established for the acceptance
debt, not a silent waiver, and therefore not an open P0-P2 against the
R4b-G object. Every remediation artifact re-executed green here at
`1bd885f`: boundary checker `ok` (same line, no pin moves), M4 map `ok`
with the full-count marker the driver now pins (C-4 closed both halves —
malformed-token guard + whole-line marker), the hardened privacy suite
**8/8 in 142.1 s** including the three new root-binding probes that close
the two-edit re-export channel with the `as`-rename residual honestly
pinned to the tree digest (C-3), and the driver's fault battery now
carrying all four disjoint partitions (P3-5/C-5) — `fault:3` (**400
passed**, the checked-artifact census) and `fault:4` (**914 passed**, the
remainder) executed through the driver itself with PARTIAL semantics
intact. Zero production lines, zero pin moves, tooling-only.

## Round-1 findings, graded

| Finding | Grade | Basis |
| --- | --- | --- |
| **P2-1** (J-1 leg 4) | **CLOSED** | Restated to the D3 dual round-2 sources at both sites; cites resolve and carry per-file byte-identity; mis-attribution bracketed; design + checkpoint annotations landed; **J-1 RATIFIED by both axes, the before-A1 remedy NOT triggered, and the M5b-IMPL settled review recorded as owed pre-A1** in the checkpoint and the tuple register. |
| **P3-1** (R-1 grading frame) | **CLOSED AS RECONCILED** | §12.7 supersedes §12.6's grading with the contract clause quoted, the four live rows named per finder, C-1 at P2 bound BLOCKING-FOR-A1 on the A1 input register; §12.8 states the battery PARTIAL in the acceptance record; named cheap closures recorded for A1 to inherit (registry refusal rows; one parametric `adapt_open` → `ValidUnlisted` test; the archive-equivalence mechanism decision). The re-homing is licensed: same clause structure as the amendment's own acceptance-debt routing, explicit, graded, and register-carried. |
| **P3-2** (R-2) | **CLOSED** | C-2 recorded P3, both axes concurring, owner = the A1 activation review's input register, alongside C-1. |
| **P3-3** (F-7 provenance) | **CLOSED** | RULING re-dated **2026-08-24** in place with the correction bracketed and this axis's 352.48 s corroboration cited; "the A1 brief" corrected at all three sites to its true source (lane owner's session notes, out-of-band — "no A1 input should cite it"); §13.4's sign-no-ruling bullet SUPERSEDED-annotated; §8 item 4 re-counted (29 items; the post-`5f090eb` commit list stated); the tuple row carries the corrected date. |
| **P3-4** (ledger) | **CLOSED, one residue + one OPEN row properly owned** | (i) W4/W5 line counts corrected in place, pinned to `78badbc`, matching my `wc`/diff measurements; residue: `GwzM5-8R4bG-EvidenceInventory.md:743` still reads "139 lines" — checklist-class, one line, next docs pass. (ii) The charging convention is filed **OPEN on the operator's ChangeBudget row with both reproducible figures (2,028 uniform / 582 review-artifact), the recorded 1,177 marked superseded, and pass-1's "spent 0" corrected** — I judge this filing ACCEPTED: the axis required the rule be named, not chosen; the rule's owner is the operator; both figures reproduce from diffs I verified; and a ledger-convention question is not a P0-P2 class finding, so it does not hold the gate — but the operator must close it at ratification, knowing the uniform reading is over ceiling. |
| **P3-5** (driver scope) | **CLOSED** | Four-partition fault battery at `1bd885f`, executed green through the driver here (`fault:3` 400, `fault:4` 914); scope reasoning recorded in the driver's own docstring with dual attribution (C-5 + P3-5). |
| **P3-6** (bookkeeping) | **CLOSED with one named close-condition** | §7.2 anchors swept; run-numbering header note landed in the classification doc ("ledger run 22 = GH run_number 32"); the tuple's perf row carries the `78badbc` corroboration. On the row-bump question the coordinator posed: **no interim bump is owed** — the register rows cite the trees their measurements were taken at, which stays true at `1bd885f` (scripts-only delta, verified); the drift class is subsumed **iff the gate-close checkpoint record restates the final tuple literally (gwz-core `1bd885f` + the three member pins) per the checkpoint's own ReviewCode-3 P3-5 convention**. If the close record does that, no register edit is needed; if it does not, the J-6(i) class recurs a third time. |

Also verified absorbed, from the Correctness axis's round-1 (read only as
reconciled into the record, not from their report): **C-6**'s
forbid-islands correction (the crate root's
`#![allow(clippy::disallowed_methods, …)]` at `gwz-core/src/lib.rs:4-7` vs
the 8 checker-verified forbid islands — the evidence §3.3/2.6a clause now
states the true mechanism), and **C-7.x**'s record sweeps where they
overlapped mine.

## Gate-closing statement (this axis)

The two conditions this axis can certify are met: (1) no P0/P1/P2 of this
axis remains open **against the R4b-G object** — C-1's P2 is open
BLOCKING-FOR-A1 on the A1 activation review's input register, where the
accepted amendment routes that debt class by name, stated PARTIAL inside
the acceptance record itself; (2) the evidence record as amended is, on
every claim this axis tested across both rounds, reproducible — and where
it was not, it now says so itself, in place, dated. Named at close, none
blocking this verdict: the `1bd885f` push-lane runs were `in_progress` at
re-verdict time (scripts-only delta; the identical suite is green at
`78badbc` and everything changed was re-executed green locally — the close
record should note the lane's completion); the close record restates the
final tuple literally; the operator closes the OPEN ledger-convention row
at ratification; the inventory's `:743` one-line count residue rides the
next docs pass. **Final: GO.** With the Correctness axis's round-2
concurrence (by existence and its own text — its round-1 GO-conditional
named exactly the §12.8 statement and the C-1/C-2 register binding now in
the record), the two independent full-tree R4b reviews stand at no open
P0/P1/P2 and R4b-G closes; per the acceptance object, that unblocks the
M5b-IMPL settled review (owed pre-A1, tier as frozen) and the
operator-escape implementation package(s), and puts the A1 activation
review — with its register now carrying C-1 (P2, blocking), C-2, the D3/D4
named residual, the acceptance debt, and the signed perf-pricing row —
last in line before activation.
