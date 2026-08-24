# A1 activation review — Completeness axis (round 1 of 2)

Date: 2026-08-24. Axis: **Completeness** (peer-blind to the Safety axis).
Review: **A1 ACTIVATION** — the final gate: whether every obligation the
program routed to this review is delivered, judged, and recorded, such that
activation is a closure and not a leak.

Object: **the A1 input register and its delivery state** at the tuple
**gwz-core `26f48f5` / gwz-cli `3cca145` / gwz-py `929efb0` / taut
`f008419`** (all four member HEADs verified equal to the tuple at review
start; gwz-core working tree clean throughout; the root gwz-dev tree carries
one untracked non-register draft, disclosed in §7).

Governing terms: `GwzM5-8ThinA1Amendment.md` §1/§4 (the register's charter —
the acceptance-debt exception with its (a)/(b) branch, the D3/D4 residuals,
the L1-19 routing); `GwzM5-8R2DSettledTuple.md` §11 (the register itself, as
amended through 2026-08-24); `CurrentProgramCheckpoint.md`'s RESUMED records
(through "THE PRE-A1 QUEUE IS COMPLETE — 2026-08-24").

Instruments and discipline: Darwin 25.5.0 / arm64; every cargo invocation
under a scratch `CARGO_TARGET_DIR` outside every repository; no git write
operation was performed; this report is the review's only file write.
Executed this review: `cargo test --lib workspace_ops::tests::g23::` →
**114 passed / 0 failed** (1,459 filtered; 114 + 1,459 = **1,573**, matching
the driver's "1573 listed" pin); `check_m4_scenario_map.py` → **ok (39
scenario rows, 41 named tests, 13 registry rows all claimed)**; `cargo fmt
--check` → **clean**; plus the corpus sweep of §1.

---

## 1. Mandate 1 — the corpus sweep and the obligation inventory

Method: the entire live `dev-docs/` corpus (248 files; `history/` consulted
for provenance only) grepped for every routing phrase — "A1 activation
review", "activation review's input", "A1 input register", "BLOCKING-FOR-A1",
"before A1", "pre-A1", "at A1", "until A1", "A1 gate", "A1 window", "A1
diff", "A1 checklist", "rides to", "owed pre-A1" — **364 matching lines**,
each read and classified. Every obligation-bearing line lands in exactly one
bucket below or is a finding.

### 1.1 Bucket (a) — DELIVERED, with the record cited (17 items)

1. **C-1 closure** — gwz-core `26f48f5`; record `GwzM5-8R4bG-Evidence.md`
   §12.9. Independently re-verified at the tuple this review: the three
   tests exist by name in `src/workspace_ops/tests/g23/
   compatibility_residue_v0.rs` (:201/:344/:410); g23 **114/0** executed;
   map checker **39/41/13** executed; driver pins ("114 passed", "917
   passed", 1573) present in `run_r4bg_aggregate_gates.py`; the J-twin
   label correction (§12.9(b): the ADAPTED case is the UNBORN twin) is in
   both the commit message and the record; the driver partition off-by-one
   (1572 vs 1573) is reported, not silently fixed.
2. **M5b-IMPL settled review** — GO (0 P0/P1, 1 P2, 3 P3),
   `GwzM5-8M5bImplSettled-Review.md`; the owed-pre-A1 row (tuple §11.1) is
   discharged on its frozen tier; J-1's remedy-in-standing delivered.
3. **The signed perf pricing** — RULING SIGNED 2026-08-24 (gwz-dev
   `d378db9`): **318.71 s (release, `b91bdeb`)** is the citable price;
   record `GwzM5-8R4bG-Evidence.md` §8 item 3 addendum + tuple §11.1 row;
   corroborated 327.6 s (driver wall) and 352.48 s (Evidence axis at
   `78badbc`); the "~343 s" figure marked out-of-band (lane-owner session
   notes), "no A1 input should cite it".
4. **R2-D settled** — GO/GO at `b91bdeb` (dual #3), the first thin-A1
   precondition; **M5b bound proofs green on the settled tree** — the
   second. Both stand at this tuple (Rust tree `b91bdeb → 26f48f5` is
   exactly four g23 test files, +504/−0, zero production lines — verified
   by diff this review).
5. **R4b-G accepted** — both axes GO round 2 at gwz-core `1bd885f` tuple;
   zero open P0/P1/P2 against the gate object.
6. **M5b settled [P3-1] remedy** — the F-1 forged-action-gate ChangeBudget
   row FILED 2026-08-24 (`GwzM5-8ChangeBudget.md:875-891`),
   existence-first, numbers withheld under the OPEN convention — exactly
   as the checkpoint records.
7. **T-6 / the v0 forged-action resume gate** — landed `d70bd57`, focused
   State review GO; both named suites + positive control re-run green at
   the accepted tuple by the M5b settled review (3/0).
8. **The operator-escape wire** — ACCEPTED 2026-08-22 GO/GO (fourth train
   on the I2 contracts); second-lane, non-gating per thin A1.
9. **The wedge runbook** — DRAFTED (`GwzMergeWedgeRunbook-v0.md`),
   explicitly "owed to current users **independent of A1**" (its own
   header and Q9); reproductions second-lane.
10. **D3 durable preservation cursor** — decided (packet D3), wire
    amendment accepted, implementation landed `8b83a2c`, dual GO/GO round 2.
11. **D1 pins live in production** — creation-time filter neutralization
    (`git/gitbackend/repository_support.rs:28-33`) and the un-pinned CRLF
    sentinel (`git/tests/g12.rs:1034`), verified live by the M5b settled
    review §5 at the accepted tuple.
12. **The coexistence decision** — MADE AND RECORDED
    (`GwzM5-8R2DPhase4Closure.md` §2.7, quarantine/relocation; execution
    pinned to R2-F); the A1 catalog gate fail-closed in code
    (`recover_or_create` production-caller-free, re-verified at the tuple
    per tuple §11.3(1)).
13. **The R2-E re-reservation records, all standing** (mandate 5): the
    census closes — **165 = 107 per-key-tabled + 2 determinations
    (preflight/plan_complete, never-gain-boundaries, filed at the Phase 3
    settle) + 38 re-reserved with records (cleanup 11 / barrier 16 /
    terminal 11; freeze §3.5 non-activation records read at
    `GwzM5-8R2DInterfaceFreeze.md:707-733/:763-790/:791+`) + 18 runtime
    restated** (freeze `:433-479`, the settled dual's ruled restatement:
    "a family-level claim on a disjoint mechanism … no per-key evidence").
14. **Matrix evidence** — three consecutive fully-green three-arm trees
    (runs 19-21) + run 22 at `b91bdeb`; carries to `1bd885f` and to this
    tuple's production code by the zero-`.rs`-delta identity (item 4).
15. **The PARTIAL byte-equivalence statement** — stated in the acceptance
    record (`GwzM5-8R4bG-Evidence.md` §12.8, quoted form), restated after
    the C-1 closure (§12.9(d): "stays PARTIAL and must still not be cited
    as green"). Swept: **no live document cites the battery as green**
    (remaining corpus hits are gate names and design references).
16. **Second-lane blocked status** — "BLOCKED ON OPERATOR HANDOFF" stated
    consistently at all five live checkpoint sites (:704, :820, :879,
    :1003, :1118); see §5.
17. **The M5b [P3-2] forward notice heeded** — `cargo fmt --check` clean at
    `26f48f5` (the C-1 closure landed formatted); the T-3 16-file NoFf pin
    untripped (g23 additions carry no NoFf mentions; suite green).

### 1.2 Bucket (b) — ON THE REGISTER, judgment owed by THIS review (12 items)

Named for the verdict; the §2 judgments below discharge this axis's half:

1. The ~3.5k acceptance-debt named exception — branch (a)/(b).
2. The D3/D4 named-residual disposition — the operator-signed output the
   branch-(a) record must carry.
3. The signed perf pricing — consumed (§2.3).
4. The C-1 closure — judged (§2.4), with its inherited residue: the 18
   UNBOUND rows' per-scenario record debt, the archive-equivalence
   mechanism decision, and cheap closures (i)/(ii) of §12.7.
5. C-2 [P3] — post-A1 ownership (§2.5).
6. The T-5 narrowing — judged (§2.6) — [P2-1] of this review.
7. The M5b settled GO — consumed with its P3 accountings (§2.7).
8. The PARTIAL statement's ride-through into the activation record.
9. The L2-05 + three-tools CI-wiring item (J-7: "with owners") — §4.
10. The Q3 escape-implementation schedule reconciliation — §5, [P3-1].
11. The service-level abandonment executable witness
    (`GwzM5-8M5bImpl-ReviewState.md:465-472`) — caught leak, [P3-2].
12. The C-1 closure tests' native Windows/ARM execution ledger entry —
    joins the register's existing Code [P3-4] expected-green pattern.

### 1.3 Bucket (c) — explicitly post-A1 or release-gated by the amendment
(14 streams)

R2-E consumer conversion (the 38 re-reserved keys; the §10 conversion
table; the **BINDING** `BarrierIntentV1::issue` observe-or-refuse
obligation; the `authority_name` self-checking-name weigh; the anchor
nits; the 481-item frozen surface; first production catalog activation —
gated by §11.3); the six R2-F rows of tuple §11.2 (quarantine/relocation
execution, MAX_PATH, legacy-path removal, native power-loss, native fault
evidence as release gate, twin-seam equivalence); the R5 exact-tree
three-platform release jobs (`ThinA1:236-239` — "off the A1 gate, not off
the release gate"); R3-R6 hardening chain; **D2 foreign-filter policy —
release-gated** ("must land before the next release cut, which is the
first to carry the amendment's `disable_filters` code"; the packet's "one
ordering that must not happen"); D1's renormalize command ("with/after
A1"); the escape implementation packages and runbook reproductions
(second lane, §5); the panic-conversion packages (second lane, operator
decision); the ARM executable-template operator policy question
(recorded "for post-A1"); probe-branch cleanup (operator's call,
standing); F5 §9 item 9 ("Before I6"); the multi-component writer rows'
native ledger entry (owed at the next matrix dispatch, register row
standing); the I6/I7/I8 version-ladder items (`GwzM5-8Refactor.md`).

### 1.4 LEAKED — routed here, previously in no bucket (2, both now placed)

1. **The service-level abandonment executable witness.**
   `GwzM5-8M5bImpl-ReviewState.md` [P3-3] (:465-472) routes it verbatim:
   "the A1 activation review should inherit an executable witness: commit
   a small probe pinning the current service-level refusal shape for a
   NotStarted action in both Normal and NoFf modes (equal outcomes), **as
   an A1-checklist input**." It appears on **no register row** (tuple §11
   does not carry it; the checkpoint's M5b acceptance P3 list names the
   other four State P3s and omits this one), and no such probe is
   committed (searched: the abandonment suites at the tuple are
   authority/reducer-level, exactly as the routing ruling described;
   nothing drives the service level across modes). Finding **[P3-2]**.
2. **The escape-implementation schedule sentence.** The adopted Q3 says
   "implementation lands … after R4b-G, **at or before A1**" (design
   :750-753; restated in the accepted amendment :41-42/:89/:2054/:2226 and
   checkpoint :1101), while the packages remain BLOCKED ON OPERATOR
   HANDOFF. Activating past that sentence silently would contradict an
   adopted operator decision — the J-1 class. The reconciliation exists in
   the corpus but is registered nowhere: `GwzM5-8A1DecisionPacket.md` §4
   step 3 schedules "**With/after A1**: … the escape implementation
   packages per the escape design's placement." Finding **[P3-1]**: the
   activation record must cite packet §4 step 3 as the controlling
   schedule (or the operator reconciles the two sentences); one line.

**Counts: delivered 17 · judged-here 12 · post-A1/release-gated 14 ·
LEAKED 2 (both caught by this sweep and placed into bucket (b) with named
remedies; zero remain unplaced).**

---

## 2. Mandate 2 — the register's own items, judged

### 2.1 The ~3.5k acceptance-debt named exception

**Terms verified at source.** `GwzM5-8ProgressReviewF5.md:56-59`: P2
(rollback, ~1,941) + P3 (status/protocol, ~1,096) + P4 (archive/GC, ~508)
= 3,545 — the arithmetic checks. The routing clause (`ThinA1:43-55`,
binding restatement `:242-244`): this review must either **(a) subsume a
settled-tree acceptance of the `v1_lifecycle` tree with an operator-signed
named-residual disposition for the D3/D4 classes, or (b) treat them as
blocking per L1-19**.

**What consuming it AT this review means:** the dual (this axis +
Safety) is the "§4.2 activation tier" review; branch (a) is a two-part
act — the dual's settled-tree acceptance PLUS the operator's signature on
the named-residual disposition. Neither half exists yet; both are the
activation train's designed outputs, not leaks. Branch (b) is exact
(L1-19, `AgentProcessRules.md:392-401`).

**Is the exception's scope still what the amendment's dual accepted?
YES — verified, not assumed.** Every `v1_lifecycle` delta since the F5
figure carries its own accepted review: the R4b P1/P2 remediation chain
(eleventh-round GO/GO), D3 (+2,192, dual GO/GO round 2 — independently
accepted, so it does not enlarge the *unaccepted* debt), M5b (0 production
lines, dual GO/GO + settled GO, six-train byte-accounting in
`GwzM5-8M5bImplSettled-Review.md` §2), the ARM fixture package (+6
test-only, direct-with-record), and the C-1 closure (+504 test-only,
measured by this review). **Zero unattributed production drift since the
settled object** (`git diff b91bdeb 26f48f5 -- '*.rs'` = four g23 test
files). The exception has not silently widened. For scale: the
`v1_lifecycle` tree at the tuple is 36,513 `.rs` lines of which 22,690
are test-path — consistent with the ~25k-line R4b acceptance object plus
its evidence weight.

**Named members the branch-(a) disposition must carry** (this axis's
enumeration, so the record is complete when signed): the D3/D4 classes
(§2.2); the **service-level abandonment hole** — routed to this exact
surface by the checkpoint (:88-93) and ruled "at A1 it must either be
fixed or block" (`M5bImpl-ReviewState.md:332-333`) — with its executable
witness ([P3-2]) or its disposition; the perf pricing (consumed, §2.3);
and the C-2 fixtures' assignment (§2.5).

### 2.2 The D3/D4 named-residual dispositions — found; neither dispositions nor deferrals

**They do not exist yet — and nothing pretends they do.** What exists is
the obligation with a traced record: the controlling clause
(`ThinA1:47-55`, "dirty-boolean recovery predicates;
`preservation/cursor.rs:277-345`"); the only site-by-site mechanism trace
(`ThinA1Amendment-ReviewSafety.md:133-164`); the scheduling provenance
(RemPlan-4 `:1100` R3 item 3, `:1111` R4 item 1); and the R4b-G Evidence
axis's live-code check (§8 item 2): the predicates are **still live** at
the tuple — `stash_complete` (`cursor.rs:283-308`) and `reset_complete`
(`:310-335`) open with D3 durable-marker fast paths and **fall through to
the surviving dirty-equality checks** (`:305-306`, `:332-333`);
companions `preservation/phase.rs:187,198`, `observe/finalization.rs:217`;
and **a third dirty-equality site at `cursor.rs:498`, outside the
amendment's cited range**. The Safety review's per-line cites predate the
D3 landing and no longer resolve one-to-one. (These cites carry to
`26f48f5` unchanged — zero `.rs` drift since `b91bdeb`.)

**Completeness requirement:** the operator-signed disposition must name
the **current** site set (including `cursor.rs:498`), not the stale
`:277-345` range, and must state per class: disposition (accepted
residual with its closure owner — R3/R4 of the post-A1 chain — and the
fail-direction argument) — a **disposition**, not a bare deferral. The
register is complete on this item (obligation + records present); the
signature is the activation train's output.

### 2.3 The signed perf pricing — CONSUMED

This axis consumes the ruling as filed: **318.71 s (release, `b91bdeb`)**
is the citable price of `root_fault_matrix`; PRICED-NOT-OPTIMIZED; the
cost is the D3 amendment §8 crash matrix's evidence weight; "no
optimization is owed pre-A1"; 576.03 s stands as the historical
D3-review-tree figure; the "~343 s" figure is out-of-band and must not be
cited. Record-complete: signed, dated (corrected 2026-08-24), register
row inserted (`d378db9`), provenance of the corrected figure stated,
independently corroborated inside the 319-353 band. Nothing further owed
here.

### 2.4 C-1 closed at `26f48f5` — record-completeness VERIFIED

§12.9's four dispositions are derived from the frozen contract with the
clauses quoted per row (`:117-121`, `:123-125`, `:159-160`, `:162-163`,
`:167-169`), tested through the real `decode → adapt_open → atomic
upgrade` path with the fully-staged `F-BOUNDARY` positive control keeping
discrimination non-vacuous in both directions; the shared-durable-record
measurement ("all four F shapes share one durable record differing only
in live observation") converts C-1's feared coincidence from assumption
to measurement; the **J-twin label correction is pinned in a dedicated
executed test** so "the A1 review cannot misread it" — and this review
did not: the whitelist has no born-root rule, the ADAPTED case is the
UNBORN twin, counts unchanged (7-ADAPTED / 13-bound stands). Executed
claims re-verified here: map 39/41/13 · g23 114/0 · driver pins · registry
mechanism argument (§12.9(c)) sound — the corpus closure assertion is
load-bearing and correctly not widened. The off-by-one is reported, not
fixed — honest.

**What the closure leaves, correctly stated by its own §12.9(d) and
judged here:** 18 UNBOUND rows keep only the class-cite record; the
archive-equivalence **mechanism decision** remains undone A1-shaped work;
cheap closures (i)/(ii) remain inherited. The battery **stays PARTIAL**.
This axis accepts the closure as complete for the four live rows and
rules the inheritance must be **done-or-owned at activation**: each of
the three inherited items gets either execution in the activation train
or a named post-A1 owner in the activation record ([P3-3]).

### 2.5 C-2 [P3] — stays open post-A1 ONLY with an owner it does not have

The four unfixtured scenarios (`B-NOT-STARTED`, `B-PREPARING-EMPTY`,
`AC-NOPUB-UNBORN`, `AP-PRESERVED`) are undispositionable either way until
fixtures exist; both R4b-G axes concur at P3; owner today = "the A1
activation review's input register". **That owner dissolves at
activation.** No successor is named anywhere. Ruling: the activation
record must assign one (the R2-F fixtures/native-evidence lane is the
natural home, matching where fixture work already lives); otherwise the
register's dissolution is itself the leak. Part of [P3-3].

### 2.6 The T-5 narrowing — substance ACCEPTED; record DEFECTIVE — [P2-1]

**Substance, verified in code at the tuple:** the register row's cites
resolve and say what the ruling claims — `retained_reader_matrix.py`'s
`ENVELOPE_FIXTURES` keys fixtures **exclusively** on `(schema,
record_schema_version)` (~:47-52), fixture names derive from that key
alone (~:83-88), the envelope-case validator admits only known
(schema, version) pairs (~:177-183), and the harness's
supported/unsupported groups are (schema, version) pairs (~:93-95). The
record body — where `mode` lives — is structurally incapable of
participating in envelope classification. The mode-blindness property is
mechanized in the tooling; the closure is per the M5b finding's own
offered alternative remedy ("or record a lane-owner narrowing that the
envelope pair subsumes it"). This axis **accepts the narrowing on
substance**.

**Record, defective in two enforceability legs:**
1. **The evidence artifact is session-ephemeral.** The row cites
   `scratchpad/insurance-t5/CANDIDATE-generator-t5-pair.NOT-APPLIED.patch`
   with pair digests `e4ea14de…` / `a1cb103c…`. That path resolves to the
   **session-scoped `/private/tmp` scratchpad** (verified present there,
   1,130 bytes, alongside the built `retained_readers/` pair — nowhere
   durable). "BUILT and SAVED" is true today and false at the first
   scratchpad cleanup; the digests would then pin an artifact nobody
   holds.
2. **The ride-along condition binds no one.** "RIDES ANY FUTURE evidence
   regeneration (R2-F's native-evidence gate is the natural carrier)" —
   "natural carrier" is an observation, not an assignment; §11.2 (Pinned
   to R2-F) does **not** carry a T-5 row.

**Remedy (cheap, exact):** commit the 1.1 KB patch to a durable home
(`dev-docs/` beside this register, or the repo) with its digests restated,
and add the T-5 pair to §11.2 or the activation record **as an R2-F
native-evidence-gate obligation by name**. Until both legs land, the
register row's own closure terms ("saved", "rides") are unenforceable —
P2 because it is a defect in a register row of the object under review,
per L1-19.

### 2.7 The M5b settled GO — consumed; P3 accountings verified

GO (0 P0/P1; the 1 P2 is §2.6's subject). [P3-1]: budget row **filed**
(bucket (a) item 6). [P3-2]: forward notices **heeded** — fmt clean at
the tuple; the closure lane's new g23 file carries no NoFf mentions
(T-3 pin untripped, suite green). [P3-3]: the attribution record
(store/tests.rs +2 = D3; forward.rs +6 = ARM fixture; four surfaces
byte-identical) is **inherited by this review** as offered — the
activation record should carry it forward rather than re-derive.

### 2.8 The M5b settled GO's own [P2-1] routing vs the checkpoint

For the record's coherence: the settled review recommended [P2-1]
"BLOCKING-FOR-A1 alongside C-1"; the lane owner instead closed it by the
finding's own alternative remedy before this review launched, and the
register row says "The A1 activation review judges this ruling as its
input." Judged: §2.6. The sequence is legitimate (the remedy was the
finder's own); the residue is [P2-1] of this report.

### 2.9 The PARTIAL statement's non-citation rule

Standing and respected corpus-wide (bucket (a) item 15). Requirement
forward: the activation record restates it verbatim — "7 proven + 6
refusal-pinned of 39 … PARTIAL, not met … must not be cited as green" —
because activation is exactly the moment a later reader would want to
cite that battery.

---

## 3. Mandate 3 — the activation package's record requirements

What the activation commit/record must contain for L1-16 evidence forms,
the freeze's sanctioned mechanisms, and the ledger to remain coherent
post-A1. Enumerated from the program's own rules; each with its source.

**The diff (one coordinated, small, reviewed change — L3-09: writer,
migration dispatch, and no-ff start "activate together only in A1"):**

1. Flip `#[cfg(test)] mod v1_lifecycle;` + compile sentinel
   (`merge/mod.rs:23-26`) — chartered in the accepted escape amendment
   §9.1 (:1989-1993) together with the `Serialize` derive flip
   (`model/v1/record.rs:19`).
2. Un-gate the `open_v0` adapter and r3 re-exports
   (`record_wire/mod.rs:13-23`, `merge/mod.rs:44-50`) — the three
   `cfg(test)` gates the Correctness axis verified as the ZERO-exposure
   proof; their removal IS the exposure event.
3. Replace `record_wire/decode.rs:82-86`'s `unreachable!` v1 arm with
   real v1 dispatch (PanicInvariantAudit §2.1 and checkpoint :1117 —
   "`decode.rs:86` removal tied to the A1 diff"); delete the
   `PRODUCTION_R3` pin's v0-only arm.
4. Remove the `--no-ff` start rejection (`validate.rs:12-16`, pinned at
   `:201`).
5. Rename-or-absorb the `*_for_r3_tests`/`*_for_test` names that become
   production implementations (audit §2.2's 47-definition census), and —
   the audit's **explicit A1-review check** — verify the genuinely
   test-only `_for_test` helpers **stay** cfg(test)-gated when their
   modules un-gate (four live inside `v1_rollback`/`catalog_lease`).
6. Version-selection machinery (M5b freeze Q2: "stays in A1").
7. Refactor stop-condition conformance (`:2984-2992`): no M6/M7/M8 wire
   variants A1 cannot execute; **archive projection compiles/publishes
   V1 only** (header-only registry for V2-V4); **no v0 writer makes
   `mode: no_ff` newly writable**.

**What the diff must NOT touch:**

8. The catalog A1-coexistence gate stays fail-closed: no production
   `recover_or_create` caller, no production catalog activation at
   `.gwz/checked-artifacts` until the R2-F relocation
   (`Phase4Closure` §2.7; tuple §11.3(1)).
9. Sealed source-associated publication (§4.1) and the eight remaining
   §14 stop conditions for post-A1 R2-E/R2-F packages; the ninth applies
   re-scoped per `ThinA1` §2.11.
10. No I2 wire/type/outcome change — activation is dispatch/cfg only;
    the contracts already carry all v1 wire (no new contract train, per
    the operator's no-further-pre-A1-trains rule).

**Same-commit companions (the freeze's sanctioned mechanisms):**

11. Tripwire updates AS the activation's own evidence: T-1/T-2's
    pre-A1 assertions convert to their post-A1 forms; T-3's 16-file NoFf
    pin re-pinned to the grown surface; T-4 + the F-3 call-graph guard
    re-derived (F-3's covenant: "must not be weakened without the R4b-G
    lane owner's ruling" — the activation IS that ruling and must say
    so); boundary-checker digest pins and publication-seam counts
    recomputed from the pristine overlay (ritual 2/6).
12. L3-09's verification inverts: production dispatch tests now prove v1
    creation IS reachable and v0 paths unbroken; the retained-reader
    envelope matrix stays green unchanged (v0.10.2 readers still reject
    v1 — T-5's protected property, now load-bearing).
13. Matrix evidence in L1-16 form at (or carrying by stated construction
    to) the activation commit — three-arm; note the C-1 closure's three
    tests currently have **no native Windows/ARM execution record**
    (runs 19-22 predate them) — dispatch or pre-attribute in the ledger,
    joining the register's existing [P3-4] expected-green row.
14. The Windows classification ledger's M5b class-membership note
    (`GwzWindowsMatrix-Classification.md:74-79`): its "cfg(test)-only
    pre-A1" clause changes truth-value at activation — dated update.

**The record (the register's closure):**

15. The branch-(a) acceptance: this dual's settled-tree acceptance of
    `v1_lifecycle` + the **operator-signed D3/D4 named-residual
    disposition** covering the current site set (§2.2), naming the
    abandonment hole (§2.1) with its witness or disposition ([P3-2]),
    the perf pricing consumed, and C-2's successor owner ([P3-3]).
16. The PARTIAL statement verbatim (§2.9); the C-1 inherited residue
    done-or-owned (§2.4); the T-5 artifact made durable + carrier
    assigned ([P2-1]); the Q3 schedule line citing packet §4 step 3
    ([P3-1]); the CI-wiring item's owner ([P3-4]); F5 §9's unadopted
    items dispositioned in one line each ([P3-6]).
17. Machine pins: the activation lands as a coordinated commit —
    `gwz.conf/gwz.lock.yml` re-pinned equal to member HEADs (it
    currently pins gwz-core at `d2cf755`, 2026-08-22, ~15 commits stale,
    `dirty: true` — [P3-5]); the checkpoint's Exact-tuple table restated
    literally (currently `78badbc`, two commits stale — P4 by the
    Correctness axis's own precedent, self-healing).
18. The ChangeBudget row for the activation package — which
    re-encounters the OPEN charging convention; the operator's ruling
    should land at or before that filing (§6.1).
19. The checkpoint gate-chain paragraph rewritten (thin-A1's discharge),
    with the R2-F/R5 **release** gates and D2 restated as unmoved — and
    the packet's "one ordering that must not happen" carried forward:
    no release cut with `disable_filters` code while D2 is open.

---

## 4. Mandate 3½/5 — the CI-wiring item, judged

Verified at the tuple: **none** of `check_merge_docs.py`,
`check_m4_scenario_map.py`, `test_v1_lifecycle_privacy_probe.py`,
`run_r4bg_aggregate_gates.py` appears in any of the six workflows
(`checked-artifact-boundary`, `linux-identity-probe`, `platform-matrix`,
`release`, `retained-readers`, `windows-matrix`). The item stands OPEN
exactly as registered (tuple §11.1/§11.3(7) + the R4b-G note). J-7's
ruling was "the CI-wiring items belong on the A1-activation input
register **with owners**" — the register carries the items but **no
owner is named**. [P3-4]: the activation record assigns one (the
activation package itself is the natural owner for the M4 map + driver,
which become the activation's own regression gates; L2-05's manifest
problem — a gwz-core-only checkout cannot see `gwz-cli/docs/*` — needs
its stated fix). Non-gating for activation to *launch*; unowned it leaks
at close.

---

## 5. Mandate 4 — the second lane, confirmed

**Activation depends on neither package. Verified against the
controlling texts:** thin-A1 §1 (A1's gates are R2-D settle + M5b bound
proofs, both standing — bucket (a) item 4); the operator instruction
(`GwzFasterProposal.md` §2: escape amendment and freezes "not A1 gates";
lane split: "If a second agent is not yet handed off, leave those items
listed as 'blocked on operator handoff' and continue R2-D").

**Blocked status correctly stated everywhere it appears:** checkpoint
:704 (R4b-G close: "operator-escape implementation packages (second
lane, still BLOCKED ON OPERATOR HANDOFF — unchanged)"), :820, :879,
:1003-1010 (the split, including "the v0 wedge runbook
reproductions/publication (Q9)"), :1118 (item-5 conversion packages).
The runbook document itself says "owed to current users independent of
A1". No live text states otherwise — sweep-verified.

**The one register-worthy residue** is the schedule sentence, [P3-1]
(§1.4 item 2): Q3's "at or before A1" vs the packet §4 step 3's
"With/after A1". The packet licenses the order this activation takes;
the activation record cites it, and the apparent contradiction closes.
Substantive exposure of activating before the escape tooling is the
Safety axis's question, and the record the operator priced it in is
already on file (packet row D1: "a wedged merge bricks the whole
workspace until escapes ship").

---

## 6. Mandate 5 — open-item hygiene

1. **ChangeBudget charging-convention row — OPEN, non-gating:
   CONFIRMED RIGHT.** It is a bookkeeping convention (2,028 uniform /
   582 review-artifact, both reproducible, diff-verified by the Evidence
   axis); no code, no gate; the R4b-G dual closed GO/GO with it OPEN on
   record. Two caveats filed: (i) the figures live in
   `GwzM5-8R4bG-Evidence.md` §13.1 and the checkpoint — `ChangeBudget.md`
   itself carries only the F-1 row's cross-reference, so "the ChangeBudget
   row is filed OPEN" reads one document too generously (P4, wording);
   (ii) the Evidence axis's close expected "the operator closes the OPEN
   ledger-convention row at ratification" — still open; the activation
   package's own row filing re-encounters it (§3 item 18). The ruling is
   the operator's; surface it there.
2. **L2-05 CI-wiring** — §4; OPEN, register-carried, unowned ([P3-4]).
3. **Probe branches cleanup** — operator's call, standing, twice
   recorded (checkpoint :390, :1217-1219); non-gating; nothing owed by
   the activation record.
4. **R2-E re-reservations** — all records standing and census-closed:
   **38 re-reserved keys with records + the 11-key cleanup family's
   Step-4.1 record among them + 18 runtime-restated keys** (the freeze
   §3.5 rows read directly; 165 = 107 + 2 + 38 + 18 — tuple §4.8,
   verified against the freeze rows). The two staleness notes already on
   record (tuple §4.6(3)/§11.3(3) lacking back-pointers to the restate
   ruling; §11.3(6) still calling the cursor decision "unresolved" after
   D3 landed dual-GO/GO) ride the next docs pass — P4.

---

## 7. Findings

- **[P2-1] The T-5 narrowing's record is unenforceable in both closure
  legs**: its evidence artifact (candidate patch + built pair,
  digests `e4ea14de…`/`a1cb103c…`) lives only in the session-scoped
  `/private/tmp` scratchpad, and its ride-along ("RIDES ANY FUTURE
  evidence regeneration") names a "natural carrier" but assigns no owner
  — §11.2 carries no T-5 row. Substance verified true in code and
  accepted (§2.6). Remedy: commit the 1.1 KB patch durably with digests
  restated; add the T-5 pair to §11.2/the activation record as a named
  R2-F native-evidence-gate obligation.
- **[P3-1] The escape-implementation schedule is undispositioned against
  this activation order**: Q3's "at or before A1" (adopted; restated in
  the accepted amendment) vs the packages' BLOCKED-ON-HANDOFF state. The
  reconciling record exists — `GwzM5-8A1DecisionPacket.md` §4 step 3,
  "With/after A1" — but is registered nowhere. One line in the
  activation record, citing it.
- **[P3-2] The service-level abandonment executable witness is a caught
  leak**: routed as "an A1-checklist input"
  (`GwzM5-8M5bImpl-ReviewState.md:465-472`), on no register row,
  uncommitted at the tuple. Deliver the cross-mode probe with the
  activation package or disposition it inside the branch-(a) record —
  the hole itself is a named member of the acceptance-debt disposition
  either way ("at A1 it must either be fixed or block").
- **[P3-3] Ownerless-at-close register items**: C-2's four fixtures; the
  18-UNBOUND per-scenario record debt; the archive-equivalence mechanism
  decision; cheap closures (i)/(ii). All correctly register-carried
  today; all lose their owner the moment the register closes. The
  activation record does each or names its post-A1 owner.
- **[P3-4] The CI-wiring item is unowned**, contra J-7's own terms
  ("with owners"); all four tools verified absent from every workflow at
  the tuple (§4).
- **[P3-5] The machine pins cannot cite this tuple**:
  `gwz.conf/gwz.lock.yml` pins gwz-core `d2cf755` (~15 commits stale,
  `dirty: true`); activation must land as a coordinated commit with the
  lock re-pinned equal to member HEADs (the checkpoint tuple-table lag
  is P4 per the Correctness axis's precedent).
- **[P3-6] The register's summary is not the register**: the
  checkpoint's "The A1 register, final" 8-item list omits
  register-carried items (CI-wiring + tools; the [P3-4]
  expected-green ledger row) and the report-carried inheritances
  ([P3-2] witness; M5b settled [P3-3] attribution). The activation
  record must consume tuple §11 + Evidence §12.7-12.9 + the M5b settled
  review §7 directly. Same pass: one-line dispositions for F5 §9's
  never-adopted items (2, 6, 7, 8, 10 — item 2's "cargo fmt --check in
  CI" is live only in `release.yml`), so the name "pre-A1 checklist"
  closes honestly.
- **P4 notes (recorded, not graded):** convention-row wording (§6.1(i));
  tuple-table two-commit lag (§3 item 17); the two register staleness
  notes and §11.3(6) (§6.4); the untracked
  `dev-docs/GwxMergeFindingsDraft.md` (dated 2026-08-23, non-register
  retrospective draft) in the root tree at review time — commit or
  relocate at the next docs pass; the C-1 closure tests' pending native
  execution (§3 item 13).

---

## 8. Verdict

**GO — conditional — from the completeness side.**
**0 P0 / 0 P1 / 1 P2 / 6 P3.**

**Inventory: 17 delivered · 12 judged-here · 14 post-A1/release-gated ·
2 LEAKED — both caught by this sweep and placed with named remedies;
after this report, zero routed obligations remain outside the three
buckets.**

The register is real, current, and — measured against a 364-line sweep
of every routing phrase in the corpus — **complete to within the seven
findings above**, every one of which has an exact, cheap, named remedy.
The two thin-A1 preconditions stand delivered and verified at this
tuple; the pre-A1 queue's three items are delivered with their records;
the acceptance-debt exception has not silently widened; the second lane
is correctly non-gating and correctly labeled everywhere it appears.

The conditions on this GO, for round 2 to verify on the record rather
than waive: **(i)** [P2-1]'s two legs land (durable artifact + named
carrier) in the activation train; **(ii)** the activation record
contains the §3 enumeration — above all the **operator-signed D3/D4
named-residual disposition over the current site set**, which is the
branch-(a) hinge no reviewer can supply, and without which branch (b)
makes the activation NO-GO by L1-19's own arithmetic. Activation that
lands with §3's record is a closure; anything less re-opens this axis.

Filed by the Completeness-axis reviewer, 2026-08-24, round 1 of 2, at
the tuple gwz-core `26f48f5` / gwz-cli `3cca145` / gwz-py `929efb0` /
taut `f008419`. This report is the review's only file write.
