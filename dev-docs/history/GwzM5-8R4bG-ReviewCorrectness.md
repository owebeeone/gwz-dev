# R4b-G gate — CORRECTNESS-axis review (round 1)

Date: 2026-08-24. Axis: **Correctness** (adversarial, round 1 of a two-round
cap; one of the two independent full-tree R4b reviews the gate closes on;
peer-blind — the Evidence-axis report appeared in `dev-docs/` as
`GwzM5-8R4bG-ReviewEvidence.md` during this session and was deliberately not
opened).

Object: **the four-repo tuple** — gwz-core `78badbc` ("Land the R4b-G gate
tooling: privacy probe, call-graph guard, M4 scenario checker, aggregate
driver"), gwz-cli `3cca145`, gwz-py `929efb0`, taut `f008419`. All four pins
verified by `git rev-parse HEAD`; all four trees clean; gwz-core
`origin/main` = `78badbc` (0 ahead/behind). **No drift.** The delta
`b91bdeb → 78badbc` was verified to be exactly the five files under
`gwz-core/scripts/checks/` (+582/−0, zero `src/` lines), so every b91bdeb
execution record carries to this tuple for Rust content, and my own
executions below were run at `78badbc` itself.

Contract set read: `GwzM5-8R4bG-Evidence.md` end to end (§1–§13),
`GwzM5-8R4bG-EvidenceInventory.md` (§5 runbook as corrected),
`GwzM5-8R4bTransitionDesign.md` §13–§15 (the frozen batteries and exit
condition), `GwzM5-8R4bReverseLifecycleInterface.md` (section map + driver
seams), `GwzM5-8R2DSettledTuple.md` (LOC audit, §11 register),
`GwzM5-8R0Inventory.md` §4/§5.2 (the M4 scenario definitions),
`GwzM5-8ThinA1Amendment.md` controlling clauses, `GwzM5-8M5bNoFfDesign.md`
must-wait clause, `CurrentProgramCheckpoint.md` RESUMED records including the
signed J-1 adjudication, `GwzWindowsMatrix-Classification.md` (run 22).

All commands ran on this host (Darwin 25.5.0 / arm64, rustc 1.95.0 via the
repo toolchain pin, Python 3.13.12) with
`CARGO_TARGET_DIR=<scratchpad>/r4bg-code-target`. No git write operation was
performed; the only file written is this report (plus scratch copies outside
every repository, deleted after use).

---

## 1. Verdict

**GO — conditional**, as a tree-acceptance gate, with **1 P2 and 6 P3
findings**, no P0, no P1.

The condition is not decorative. Finding C-1 (= the evidence's R-1) is a
**P2 filed as an A1-activation input**: the frozen byte-equivalence battery
clause ("across all M4", `GwzM5-8R4bTransitionDesign.md:1476-1477`,
`GwzM5-8Refactor.md:2265` O8) is measured NOT MET at 13/39, and under L1-19
that P2 makes **the A1 activation gate NO-GO while it stands unresolved**.
R4b-G may close as GO only if its acceptance record (a) states the
byte-equivalence battery as PARTIAL — proven for the 7 adapted cases,
refusal-pinned for the 6 unlisted, unbound for 22, fixtureless for 4 — and
(b) binds C-1/C-2 onto the A1-activation input register that
`GwzM5-8ThinA1Amendment.md:43-55` (§1, branch (a)/(b)) already defines for
exactly this debt class. If the lane or operator rejects that routing and
reads O8's every-M4 clause as R4b-G-blocking, the mechanical consequence of
this report is **NO-GO** until the cheap closures in C-1 land. Everything
else I found is P3: real, named, none of it disqualifying for a tree whose
v1 surface is mechanically unreachable in production (verified, §6).

Basis in one paragraph: I re-executed the entire mechanical gate train myself
at `78badbc` and it is green to the exact recorded counts (§7); I audited the
two highest-stakes batteries' test substance deeply and they prove what the
gate needs where they claim to (§3); the three new guards are fail-closed and
non-vacuous — I reproduced the privacy probe's widening demonstration
(exactly one probe fails, positive control green) and independently broke
each guard in the directions that matter (§4); the cross-repo suites pass at
the pins (139/0, 330/0, cross-driver 14/14) (§8); and the seven judgment
calls resolve as recorded, with J-1's lane-owner adjudication **ratified**
(§5). The gate's one genuinely open defect is the measured 13/39, and the
program already owns the mechanism for consuming it.

Findings, most severe first:

| ID | Sev | One line |
| --- | --- | --- |
| C-1 (=R-1) | **P2** | O8 "every M4" byte-equivalence measured NOT MET (13/39); no mechanism can even bind the ten archive shapes; the three finalizing-era mid-prefix shapes have an unstated, untested adaptation disposition — routed to the A1-activation register, where L1-19 makes it blocking |
| C-2 (=R-2) | P3 | 4 of 39 scenarios have no durable fixture at all; undispositionable either way until fixtures exist |
| C-3 | P3 | privacy probe evadable by a two-edit widening (struct → `pub(crate)` + aliased re-export in `v1_lifecycle/mod.rs`): demonstrated to compile with an outside consumer while all five probes stay green |
| C-4 | P3 | M4 scenario checker silently ignores malformed test tokens (demonstrated exit 0 on a mangled name); driver pins only `M4 scenario map: ok`, not the counts |
| C-5 | P3 | the driver's `fault` battery runs only the v1_lifecycle 254+1; the 400-test `checked_artifact::` fault census and full lib train are in no battery, while evidence row 2.1 folds them into the same gate's claim |
| C-6 | P3 | the record overstates the compiler seal: gwz-core's crate root carries `#![allow(clippy::disallowed_methods)]` (lib.rs:4-7); the forbid is 8 checker-verified modules, not the crate |
| C-7 | P3 | bookkeeping: live must-wait text contradicting the signed J-1 adjudication; no checkpoint record names `78badbc` (the reviewed object); F-7 signed/unsigned contradiction inside the evidence doc |

---

## 2. Findings in full, with failure scenarios

### C-1 [P2] — the every-M4 byte-equivalence clause is measured NOT MET, and the sharpest unbound cases are finalizing-era

The §12 enumeration (39 scenarios from `GwzM5-8R0Inventory.md` §4/§5.2) is
faithful — I verified it against R0's tables and spot-checked the claimed
evidence tests' substance (`archived_v0_unknown_fields_and_raw_bytes_survive_
status_and_retention` really does assert byte-stability of the archive file
under status and GC; the prefix characterization really does pin v0
restartability). The measured result stands: **7 ADAPTED + 6 UNLISTED = 13
bound; 22 UNBOUND; 4 NO FIXTURE.**

Three sharpenings from this review:

1. **The archive family (10/10 unbound) is partly a category error in the
   obligation, not only a coverage hole.** The registry's two corpora
   (`fixture_corpus`, `valid_unlisted_corpus`) model open-record adaptation
   only; v0 archives are never rewritten (archive = durable rename of the
   terminal open YAML; v1 reads them projection-only, and
   `record_wire::archive`'s 29 tests pin byte-stability and the
   contradiction taxonomy). There is **no registry vocabulary in which an
   archive shape could be bound**. What O8's archive clause actually still
   owes is different and real: proof that operations *finishing under v1*
   produce archives byte-equivalent to v0's for the same scenario. No such
   comparison exists. The closure needs a mechanism decision, which is
   A1-shaped work.
2. **The concrete failure scenario the 22 leave open** (the strongest one I
   could construct): a user's workspace holds a v0 record interrupted
   mid-candidate-publication — F-BASELINE, F-MARKER, or F-LOCK, all
   `Finalizing`-state, i.e. inside the only state the whitelist adapts. No
   test drives `adapt_open` on these shapes
   (`characterization_publication_prefix_v0.rs` contains no
   adapt/upgrade/ValidUnlisted reference — verified by grep; only three g23
   files touch the adapter). The adapter's descriptor does incorporate the
   observed root relations (`open_v0/descriptor.rs`: `publication_step`,
   candidate/composition/hash relations, `normalized_i2_root_observation`),
   which *should* make a mid-prefix descriptor differ from
   `changed/prefix-boundary`'s and fall through to ValidUnlisted — but
   nothing pins that. If a mid-prefix observation ever coincides with the
   boundary descriptor, v1 adapts the record with a boundary-tuned
   `next_action` and the publication restart re-drives the prefix from the
   wrong position. Today that is unreachable (§6); at A1 it is live.
3. **Cheap closure paths exist and should be named in the acceptance
   record:** (i) registry rows (adapted or declared-refusal) for the 14
   unbound progress shapes — most are non-finalizing and the whitelist's own
   closure assertion (`assert_i2_valid_unlisted_fixture` proves every rule
   is open+finalizing) makes refusal the forced answer, so these are
   mechanical to file; (ii) one parametric test driving `adapt_open` over
   every non-whitelisted fixture asserting `ValidUnlisted` + byte
   preservation — this alone would pin the F-row scenario above; (iii) an
   archive-equivalence mechanism decision for the ten archive shapes.

**Grade rationale (the mandate's question answered).** The registry is more
than a ledger — it is the tree's only machine-checked statement of
adapt-vs-refuse per scenario, `include_str!`-bound into both the production
adapter and the tests — but its load-bearing moment is A1 activation, not
tree acceptance: at this tuple the entire adaptation surface is
`#[cfg(test)]` at three independent levels (§6), so the unbound dispositions
have zero reachable consumers. Blocking R4b-G on them reduces no live risk
and delays the M5b-IMPL settled review that GO unblocks; letting them ride
silently would falsify the battery. Hence: **P2, open, owned by the
A1-activation input register under ThinA1 §1 (a)/(b), where L1-19 makes it
blocking** — and R4b-G's own record must carry the PARTIAL battery verdict
verbatim, not a green.

### C-2 [P3] — four scenarios have no fixture and cannot be dispositioned

`B-NOT-STARTED`, `B-PREPARING-EMPTY` (R0 §4 row B's own recorded gaps,
unchanged since baseline), `AC-NOPUB-UNBORN`, `AP-PRESERVED` (the archive
projections R0 §5.2 asks for; the existing evidence is live-workspace).
Failure scenario: any future claim about these shapes — adapt, refuse, or
project — is untestable until a fixture exists; a regression touching them
is invisible to every battery. Routing: A1-activation register, with owners.
Concur with the evidence's own P3 grading.

### C-3 [P3] — the privacy probe pins path-nameability, not type-reachability

Reproduction and adversarial probes, all on scratch copies of the tree,
none in any repository:

- **The recorded widening demonstration reproduces exactly**: with
  `mod transition;` → `pub(crate) mod transition;` and `PreparedV1Rewrite`
  → `pub(crate)`, the suite fails precisely
  `test_prepared_v1_rewrite_is_unnameable_outside_the_perimeter`
  (`AssertionError: 0 == 0`) with the positive control and the other three
  seals green. Non-vacuous, fail-closed in the primary direction.
- **Single-edit re-export widening is closed by the language**: adding
  `pub(crate) use transition::PreparedV1Rewrite as …;` to
  `v1_lifecycle/mod.rs` against the pristine seals fails to compile
  (E0365 + E0603). Good.
- **The two-edit variant evades all five probes**: widen the struct to
  `pub(crate)` *and* add an aliased re-export at the `v1_lifecycle` root —
  this **compiles with an outside consumer in `finalize_dispatch.rs`
  naming the re-export** (demonstrated), while every probe stays green
  because the canonical path `v1_lifecycle::transition::PreparedV1Rewrite`
  still E0603s (`transition` stays private). The only tripwire is the
  `PROTECTED_SOURCE_TREE_DIGESTS` pin on `v1_lifecycle/mod.rs`, whose
  refresh the F-3 comment itself calls routine and property-free.

Failure scenario: a well-meaning refactor exposes the prepared rewrite (or a
proof token, same shape) through a root-level alias during an otherwise
legitimate tree change; the digest is refreshed as part of that change; all
privacy probes stay green; the battery's "cannot construct" claim is false
from that commit on. Remedy (cheap): three more negative probes importing
the sealed names at the `v1_lifecycle::` module root (asserting
E0432/E0603), closing the alias/re-export channel for all three seals.

### C-4 [P3] — the M4 scenario checker is fail-closed against the wrong rot direction only

Demonstrated on doctored scratch copies (`--doc`/`--registry`/`--test-list`
arguments; nothing in-repo touched):

- missing map → exit 1; stale well-formed test name → exit 1 ("map names a
  test that does not exist"); deleted registry row → exit 1 ("map cites a
  registry case that does not exist"). Good.
- **a malformed map token passes silently**: renaming a mapped test to a
  form that fails the `TEST_PATH` regex (my probe used an uppercase suffix)
  drops it from checking entirely — exit 0, and the only trace is the
  unpinned count falling `38 named tests` → `37`. The checker's question 1
  ("does every test the map names still exist?") is only asked of tokens
  that already look like test paths.
- the driver's marker is `M4 scenario map: ok` — the counts are printed but
  not pinned, so the silent drop above also passes the driver.
- nit: the docstring's "claimed by exactly one scenario" is not what the
  code checks (set semantics; `terminal/completed` is deliberately claimed
  twice by the map itself).

Failure scenario: an editor mangles one row's token while "tidying" the
table; the map silently stops checking that scenario's test; a later rename
of the real test goes unnoticed — the exact rot class W4's checker exists to
catch. Remedy: pin the expected counts (in the checker or by widening the
driver marker to the full ok line), and/or fail on backticked tokens that
resemble paths but match neither `TEST_PATH` nor `CASE_ID`.

### C-5 [P3] — the driver's `fault` battery under-covers the gate it is named for

`run_r4bg_aggregate_gates.py`'s `fault` battery runs
`workspace_ops::merge::v1_lifecycle::` (254) plus the release
`root_fault_matrix` (1) — nothing else. Evidence row 2.1's PASS for the same
gate is claimed on a wider base: "checked_artifact:: 400/0 carries the
165-key fault census … g23 fault/recovery suites inside the 111". The g23
suites at least ride the `byte-equivalence` battery; **the 400-test
checked_artifact census and the remaining 914 lib tests are in no battery at
all.** The driver is honest that a zero exit is not an R4b-G pass, but the
battery bears the name "aggregate fault/restart matrices
(TransitionDesign:1469-1475)", and a future operator re-running "the seven
batteries" will believe the aggregate fault gate ran when 400 of its
evidence-cited tests did not. Failure scenario: a checked-store fault-census
regression lands; all seven driver batteries stay green; the recorded gate
train's row 2.1 is silently false at the new tree. Remedy: add a `fault:3`
(`checked_artifact::`, marker `400 passed`) or renarrow the battery title
and row 2.1 to match each other. Two related nits, recorded not graded: the
substring markers are one-sided (`"254 passed"` ⊆ `"1254 passed"`; growth
that engulfs a marker passes), and child-process libtest output is scanned
too — `workspace_mutator_lock`'s self-exec tests print an inner
`1 passed; 1569 filtered` summary into the blob (I chased this to ground; it
is benign today because return codes bind, but marker design should not rely
on it).

### C-6 [P3] — the compiler-seal posture is overstated in the record

`gwz-core/src/lib.rs:4-7` is `#![allow(clippy::disallowed_methods, …)]` for
the whole crate; the forbid exists in exactly the 8
`PROTECTED_COMPILER_MODULES` files (checker-verified, `check_checked_
artifact_boundaries.py:994-999`), of which only `authority/observe.rs` is
inside `v1_lifecycle/`. That is a coherent design — the raw v1 writer
(`store/rewrite.rs`, audited §3.2 below) is *supposed* to write, and it is
sealed by privacy, not lint — but the evidence's §3.3 sentence "gwz-core
keeps its own clippy.toml plus `#![forbid(clippy::disallowed_methods)]`"
reads crate-wide and is not. This matters for J-3's optics: v1 test code
freely calls `std::fs::write` (e.g. the fault matrix's `seed_recovery`,
`root_fault_matrix.rs:174`) while gwz-cli was flagged for the identical call
under the imposed conf. Failure scenario: a future reviewer or the A1 brief
cites the crate-wide forbid as a standing guarantee and skips re-checking a
new raw write outside the 8 files. Remedy: one-line correction in the
evidence/inventory naming the true topology (crate allow + 8 verified forbid
islands + structural digests + privacy seals). No code change owed.

### C-7 [P3] — bookkeeping the acceptance record must close (J-6, extended)

1. The J-1 must-wait text is still live in two places after the signed
   adjudication: `GwzM5-8M5bNoFfDesign.md:976-989` (unannotated) and
   `CurrentProgramCheckpoint.md:1118` ("IMPL merge waits for R4b-G per the
   frozen…"). Whichever way round 2 rules, the record must stop
   contradicting itself.
2. **No checkpoint record names `78badbc`** — the object this gate accepts.
   The §"Exact tuple" table (checkpoint:815-824) was refreshed to
   "`b91bdeb` = origin/main", which is now false (origin/main is
   `78badbc`); the table's own note promises literal restatement at the
   next docs-only commit.
3. F-7 status is self-contradictory inside the evidence doc: line ~641
   carries "RULING — SIGNED by the lane owner, 2026-08-23" while §9.2/§13.4
   still say drafted/unsigned; the settled tuple's §11.1 register carries
   the signed row (318.71 s, PRICED-NOT-OPTIMIZED). The signed state is the
   real one; the doc should say so once.
4. Checkpoint:641 still says F-6 "dispatched"; run 22 closed it
   (`GwzWindowsMatrix-Classification.md` head: 32672188425 at `b91bdeb`,
   1530/0/1, platform sibling ARM 1569/0/1 + macOS 1580/0/1). Related
   disclosure the record should carry: **no native matrix run exists at
   `78badbc` itself**; the delta from `b91bdeb` is five Python files, so run
   22's evidence carries to this tuple by construction — say that, rather
   than leaving the gap for the next reviewer to re-derive.

---

## 3. The seven batteries: what each proves vs. what the gate needs (mandate 1)

I ran `scripts/checks/run_r4bg_aggregate_gates.py` myself at `78badbc`,
partitioned per its selector exactly as the recorded run was: invocation 1 =
`compatibility byte-equivalence unknown-field` (all ok), 2 = `privacy` (ok,
70.4 s), 3 = `fault:1` (ok, 351.9 s, `254 passed`), 4 = `fault:2` (ok,
454.5 s wall, `1 passed`), 5 = `call-graph:1 call-graph:3
settled-tree-review` (ok/ok/REVIEW), 6 = `call-graph:2` (ok, 555.4 s). All
14 commands green with their exact markers; PARTIAL printed on every
partitioned invocation; the seventh gate printed REVIEW and was never
counted green. Judgments per battery:

1. **Aggregate fault (v1 lifecycle):** SUBSTANCE MATCHES NEED at the
   lifecycle layer. Deep audit (`tests/reverse_preservation/root_fault_
   matrix.rs`, 448 lines, plus the surrounding 9,061 lines of
   reverse/store/c7 test trees): the root matrix crosses two root owners ×
   pinned case censuses (54/58 — count drift fails the test) × three
   boundaries (BeforePhysical failure, AfterPhysical crash,
   AfterDurableSuccessor crash), asserts the interrupted durable state is
   recoverable, the pending action journal matches the injected target,
   resume converges to Terminal(Aborted) under alternating Abort/Preserve
   requests, **re-execution of the target physical action is counted
   exactly** (1 / 1-if-parent-durable / 0), and exactly one preservation
   stash survives. That is TransitionDesign:1469-1472's obligation
   discharged with a non-repetition proof, not a smoke test. The battery's
   *scope* defect is C-5 (driver ≠ row 2.1), not its substance.
2. **v0 compatibility:** PROVES WHAT IT NEEDS TO. Registry mechanically
   validated (7 rules / 7 bindings; suite 14), `include_str!`-bound at
   `g23/compatibility_v0.rs:10` and consumed by the production adapter;
   merge-doc gate 11 sources / 147 assertions. The gate's reach is bounded
   by C-1's registry coverage, which is the finding.
3. **Byte-equivalence:** SUBSTANCE STRONG WHERE BOUND; DENOMINATOR IS C-1.
   Deep audit (`g23/atomic_upgrade_v0.rs` + `compatibility_v0.rs`): per
   adapted case — descriptor equality + canonical-JSON SHA256 against the
   registry, I2-vs-v1 restart-action identity, accepted-lock byte identity
   (`accepted.lock.exact_yaml == candidate.lock_yaml`) + SHA256, four-point
   upgrade fault matrix with exact durable state per fault, multiple-open
   rejection, unknown-field survival at six depths with three
   verifier-negative controls, future-lock-member retention into the
   accepted audit, and **before/after head+status observation of the root
   and every participant repo proving the upgrade mutates no repository**.
   The unlisted half asserts refusal + byte-exact preservation + the
   whitelist's own closure (every rule open+finalizing). This is the full
   O8 property — for 13 of 39.
4. **Unknown-field:** matches need: `record_wire::` 75 (archive 29 with the
   byte-stability and contradiction tests spot-read, unknown_fields 28,
   decode 18) + the exact-manifest-per-transition-effect matrix test.
5. **Privacy:** non-vacuous and fail-closed in the primary direction
   (reproduced); residual C-3.
6. **Call-graph:** both halves real. Structural: checker `ok (15 visible
   entries, 5 classified modules)` — the identical pre-remediation string,
   confirming no digest pin moved at `78badbc`. The 69-test suite green in
   555.4 s including the 4 new guard tests (also run individually, §4.2).
7. **Settled-tree review:** correctly not mechanisable; the driver refuses
   to count it green. This report is half of it.

## 4. The three new guards (mandate 2)

### 4.1 v1→v0 persistence guard (masked-token scan; load-bearing for J-1)

Fail-closed and non-vacuous, verified four ways: (i) its four unit tests run
individually — including the compiler probe that first proves the violation
*compiles* (`MergeStore`/`FileMergeStore` are `pub(crate)` and reachable from
`v1_lifecycle/`), so the checker, not rustc, is the enforcement; (ii) the
seam floor is fail-closed — restructuring the `use store::{…}` re-export
away yields "underivable", tested; (iii) masking is load-bearing — I
counted 10 raw `enter_finalizing` hits in `v1_lifecycle/**` (all string
literals) and 0 `MergeStore|FileMergeStore` hits, and the checker runs green
over that tree, so the masked scan reports the true zero where naive grep
reports ten false positives; (iv) the string-literal negative control
asserts the digest finding fires *without* the seam finding. Escape
analysis: any call path must name a seam token (aliasing, full paths, and
trait bounds all still name the terminal identifier; a closure injected from
outside the tree moves the call site outside the boundary, which is the
boundary working). CI: the guard and its tests ride
`checked-artifact-boundary.yml:30-35` per commit — verified in the workflow
file. The J-1 load-bearing comment is present and explicit.

### 4.2 Privacy compile probe

5/5 green at `78badbc` (70.4 s in the driver); widening demonstration
reproduced exactly (§2, C-3); positive control proves the sealed names
exist, so a rename cannot green the negatives vacuously. Residual: C-3's
two-edit evasion. Not CI-wired (lane owner's item, J-7).

### 4.3 M4 scenario checker

Fail-closed on missing map, stale well-formed test names, and registry
drift in both directions (all demonstrated on scratch copies); the 0.1 s
runtime that looked too cheap is genuine (warm-cache `cargo test --list`).
Residual: C-4's malformed-token fail-open. Cannot run in bare-checkout CI
(needs the sibling `dev-docs/`) — disclosed in its own docstring, J-7 class.

## 5. Judgment calls J-1..J-7 (mandate 4)

- **J-1 — RATIFIED: the adjudication's ACCEPTED-WITH-RECORD stands.** Facts
  re-verified independently: `3e60529`/`8c1624a` are ancestors of
  `78badbc`; the settled tuple's LOC audit rows 3-4 show prod net **0** for
  both; the must-wait clause and its checkpoint restatement are still live
  text (C-7.1). The four legs hold, and leg 3 is now more than recorded —
  I proved the guard fail-closed (§4.1). Reason (b) (moving target) is
  defused by the measured zero production delta; reason (c) could never be
  discharged retroactively, and a standing guard that mechanically proves
  the leaned-on property *at the accepted tuple, including M5b's own six
  test suites in the scanned 128 files*, is the correct discharge shape for
  a tree-acceptance gate. Reason (a)'s violation is what "with record"
  records. The named remedy (M5b-IMPL settled review BEFORE A1) is **not
  triggered** — but the M5b-IMPL settled review itself remains owed on its
  recorded tier, unblocked by this GO, and C-7.1's annotations are the
  price of ratification.
- **J-2 — CONFIRMED.** The R2-D 5.2 settled dual (GO/GO at `b91bdeb`, zero
  open P0-P2, reports present in dev-docs) is like-for-like with RemPlan-4
  R6 at the checked-artifact layer; the R4b tree outside `checked_artifact/`
  inherited no unreviewed precondition because its full-tree review is
  exactly this dual, and the lifecycle acceptance debt is explicitly routed
  to A1 (ThinA1 §1). Row 2.7a is discharged.
- **J-3 — CONCUR with `-p gwz-core`.** The two-line correction is recorded
  in place (read) and both lines re-executed green here (clippy
  `-p gwz-core` with CONF_DIR: zero diagnostics; plain workspace clippy:
  clean; fmt clean). C-6 sharpens the rationale: the perimeter inside
  gwz-core is itself 8 forbid islands under a crate-level allow, so
  imposing the conf on gwz-cli would demand of a CLI driver what the owning
  crate does not demand of itself outside the islands.
- **J-4 — CONCUR.** R3 is post-A1; the probe exists and is live; the three
  seals are stated independently of the rollback-prefix seam, so an R3
  reshuffle changes W3's future gate, not this one.
- **J-5 — CONCUR.** `RollbackPrefixIssuer`: 0 hits (re-verified);
  `issue_verified_rollback_prefix` remains `pub(super)` at
  `rollback_prefix.rs:240` (re-verified). A recorded DEFERRED with the
  seam's privacy stated is the only honest disposition available.
- **J-6 — see C-7.** Partially closed since the evidence pass (tuple table
  now names `b91bdeb`), re-opened one commit by the tooling landing.
- **J-7 — RULED: acceptable for this gate, register the debt.** The
  host-special gates (regen, d0, L2-05 doc gates, M4 map, privacy probe,
  driver) are all green here on a second independent execution, and the one
  guard that is load-bearing for an adjudication (F-3) is the one that IS
  CI-wired — that allocation is right. A gate only a developer machine can
  run is still not a gate; the CI-wiring items belong on the A1-activation
  input register with owners (the tuple's §11.3(7) already carries L2-05;
  the three new tools join it).

## 6. The exit condition, mechanically re-verified

`GwzM5-8R4bTransitionDesign.md:1522-1527`: typed-kernel/checked-store
commitment — audited at the writer itself (`store/rewrite.rs`: lease +
source-digest gate, TOCTOU reopen, known-diff verification, unknown-manifest
overlay, staged-byte and published-byte re-verification, injectable
`CommitFault`s consumed by the store matrix); matrix coverage — §3;
accepted-workspace/exact-lock consumption — the finalization suites and the
byte assertions of §3.3. Unreachability, all re-verified at `78badbc`:
`#[cfg(test)] mod v1_lifecycle;` (merge/mod.rs:23-24), the **entire
`open_v0` adapter module** `#[cfg(test)]` (record_wire/mod.rs:13-14), every
r3-test re-export `#[cfg(test)]` (merge/mod.rs:44-50), and
`recover_or_create` with no production caller (test providers and bootstrap
tests only). Three independent cfg gates stand between production and the
v1 writer/migration dispatch. Passing R4b does not activate v1 — confirmed
in the code, not just the prose.

## 7. The executed gate train at `78badbc` (this review's own runs)

| Gate | Result |
| --- | --- |
| lib suite, 4 disjoint exhaustive partitions (400 + 914 + 254 + 1 of 1570 listed) | **1569 / 0, 1 ignored** — byte-identical to the settled tuple's figure |
| Integration binaries ×4 (diff_render_spike, protocol, publish_workflow, rename) | **50 / 0** |
| Seven-battery driver, 14 commands across 6 invocations | all mechanical batteries **ok** with exact markers; `settled-tree-review` = REVIEW |
| `root_fault_matrix` (release, via `fault:2`) | green, **454.5 s wall** on this host — a fourth measurement joining 318.71 s / 327.6 s / 576.03 s; the pricing record's host-variance framing is corroborated, the phantom "~343 s" remains nonexistent |
| Boundary checker + 69-test suite + release-boundary 6 | `ok (15 visible entries, 5 classified modules)` / 69 OK / 6 OK |
| Privacy probes | 5/5, plus the widening reproduction (1 exact failure on the widened copy) |
| clippy `-p gwz-core` (CONF_DIR) and plain `--workspace`, both `-D warnings`; `cargo fmt --all --check` | clean / clean / clean |
| `protocol/regen.py --check`; `d0_roundtrip_check.py` (taut on PYTHONPATH) | OK / all round-trips passed |
| §7.1 corrections re-verified | `clippy.toml` = 19 entries; `FinalizationFault` = 8 variants |

## 8. Cross-repo at the pins (mandate 5)

- **gwz-cli `3cca145`:** `cargo test -p gwz --all-targets` → **139 / 0**
  across 8 targets, built against gwz-core `78badbc` through the shared
  workspace — a genuine integration result at the review tuple itself.
- **gwz-py `929efb0`:** `run_tests.py` (its own venv, maturin develop
  rebuild against `78badbc`, `regen_protocol.py --check` OK against taut
  `f008419`) → **330 / 0**; `test_merge_cli_cross_driver.py` re-run
  individually → **14 / 14** (4 functions parametrized — the checklist's
  "4" and the evidence's "14" are the same tests counted at different
  granularity). The cross-driver seam is real: it builds the actual `gwz`
  binary and asserts CLI-vs-native equivalence on shared workspaces.
  Disclosure reproduced: the harness rewrote `gwz-py/Cargo.lock` (committed
  lock pins gwz-core 0.10.3 vs the tuple's 0.10.4); restored by file copy
  immediately, tree clean — the standing cross-repo observation for the A1
  review is confirmed live. The cross-driver fixture also writes build
  artifacts to the workspace-root `target/` by its own `--target-dir`
  choice (pre-existing directory, untracked).
- **taut `f008419`:** exercised through both protocol gates (regen check,
  d0 round-trips), green. No separate gate claimed for it; none owed.
- **Driver-seam drift check:** the surfaces the drivers consume (status,
  archive/GC read paths, merge CLI envelope) sit on the v0 production
  surface; the v1 lifecycle is invisible to both drivers at this tuple
  (§6), so the frozen `GwzM5-8R4bReverseLifecycleInterface.md` contracts
  cannot yet be violated by a driver — the parity suites above are the
  binding evidence, and they bind.

## 9. Provenance and disclosures

Executed 2026-08-24, Darwin 25.5.0 / arm64; rustc/cargo 1.95.0 (repo
toolchain pin; host default 1.96.0 unused inside the repos); Python 3.13.12
for gwz-core's checkers, gwz-py's own 3.12 venv for its harness. All cargo
work under the session scratchpad `CARGO_TARGET_DIR`; adversarial guard
probes ran on tree copies under the scratchpad, deleted afterwards. Python
3.13.12's unittest exits 5 on zero collected tests (verified), closing the
vacuous-"OK" channel for the driver's suite commands on this host. No git
write operation was performed in any repository. `gwz-py/Cargo.lock` was
transiently rewritten by gwz-py's own harness and restored byte-identically
by copy (sha-verified against a pre-run snapshot). All four member trees
verified clean at their pins after all work. The peer Evidence-axis report
was not read. This file is the only repository write of this review.

**Verdict restated: GO — conditional on the acceptance record carrying the
PARTIAL byte-equivalence verdict and binding C-1 [P2] / C-2 [P3] to the
A1-activation input register (where L1-19 makes C-1 blocking); J-1
ratified; 1 P2, 6 P3, 0 P0/P1.**

---

---

# ROUND 2 (final) — focused re-verdict on the merged remediation

Date: 2026-08-24. Axis: **Correctness**, round 2 of the two-round cap —
final. Object: **the four-repo tuple with gwz-core moved `78badbc` →
`1bd885f`** ("Harden the R4b-G gate tooling per the dual's round-2
prescriptions"); gwz-cli `3cca145`, gwz-py `929efb0`, taut `f008419`
unmoved. Verified: `1bd885f` = `origin/main`, all four trees clean; the
code delta is **exactly the three `scripts/checks/` files the coordinator
named** (`check_m4_scenario_map.py` +26/−2, `run_r4bg_aggregate_gates.py`
+28/−8, `test_v1_lifecycle_privacy_probe.py` +60/−7), **zero `src/` lines,
zero digest-pin lines** — so every Rust execution result of round 1
carries to this pin byte-for-byte, and everything re-run below was run at
`1bd885f`. The record remediation is gwz-dev `aeb4411` (+ `d378db9`/
`48ee7fe`/`d5967c9` context). Same host, same discipline: no git write
operation; the only repository write is this appended section.

## R2.1 The GO-condition is satisfied on the record — verified

- **(a) PARTIAL stated in the acceptance record.** `GwzM5-8R4bG-Evidence.md`
  §12.8 carries the byte-equivalence battery verdict **block-quoted, in the
  form round 1 required**: "PARTIAL, not met … 7 proven + 6 refusal-pinned
  of 39 … 22 UNBOUND and 4 NO FIXTURE … A green byte-equivalence battery is
  **not** claimed by this gate and must not be cited from it." Row 2.3b/O8
  stays FAIL. Nothing about the measured numbers moved.
- **(b) C-1/C-2 bound to the A1-activation register.** §12.7 (the two axes'
  reconciled record) grades **C-1 P2, OPEN, BLOCKING-FOR-A1 per L1-19**,
  routed under `GwzM5-8ThinA1Amendment.md:43-55` branch (a)/(b); the
  settled tuple's §11.1 gained the dated 2026-08-24 block carrying C-1 and
  C-2 in the register the A1 review consumes. The **four live-residue rows
  are named**: my `F-BASELINE`/`F-MARKER`/`F-LOCK` (finalizing-state,
  adaptation disposition unstated and untested) plus the Evidence axis's
  `J-NO-PUBLICATION-UNBORN` (class membership ambiguous). The three cheap
  closures are recorded for A1 to inherit.
- **The new categorical-exclusion strengthening is genuine and correctly
  used.** I verified `GwzM5-8I2CompatibilityContract.md:117-125` and
  `:159-165` against the frozen text — the 22 unbound rows are indeed
  contractually dispositioned by class ("Marker/lock-only prefixes …
  preservation, rollback, and terminal rows are not A1 migration rules";
  "Zero whitelist matches is not an error … Archived v0 uses only the
  archive decoder/projection"). §12.7 uses the cite to establish that the
  unbound space is *class-dispositioned and byte-pinned*, while **keeping
  the P2 open** for the missing per-scenario record and the four rows where
  the class cite is insufficient — that is exactly the honest shape; the
  contract cite dissolves nothing it should not.

Pre-A1 exposure remains zero on my own round-1 triple-`cfg(test)`
verification, which §12.7 now cites; nothing in the `1bd885f` delta touches
it (scripts only).

## R2.2 Findings re-verified at `1bd885f`, one by one

**C-3 — CLOSED; residual statement judged and ACCEPTED.** The three
`ROOT_BINDING` probes assert **both** an error code (E0432/E0603) **and the
rejected path** (`v1_lifecycle::<name>`), so an unrelated build failure
cannot green them; the raw-writer root probe correctly runs from **inside**
the lifecycle (a private root `use` is nameable by every descendant, and
that seal claims to hold against the rest of the lifecycle too). Executed:
the full 8-probe suite green on the real tree (150.7 s, via the driver's
`privacy` battery). Both directions demonstrated by me on scratch copies:
(i) all three seals widened with **unaliased** root re-exports → **all
three root probes fire** (failures=3, each `AssertionError: 0 == 0` — the
probe import compiles); (ii) the lane's `demo/widen` tree — all three
seals widened with **`as`-renamed** re-exports and an outside consumer
naming `V1RewritePlan` baked into `finalize_dispatch.rs`, i.e. the seal
demonstrably broken in every compile — **8/8 probes stay green**. That
quantifies the docstring's residual exactly as stated: name-based probes
cannot predict an alias; the channel is held by
`PROTECTED_SOURCE_TREE_DIGESTS`. **Judgment on the residual statement:
honest and accepted** — with one P4 wording note, in the guard's favor:
the docstring says the digest "pins `v1_lifecycle/mod.rs` where any such
re-export must be written", but a widening re-export could also live in a
new child module file; the hold survives because the pin is a **tree**
digest over the whole `v1_lifecycle` subtree (round 1 demonstrated it
firing on a `tests/fixtures.rs` append), so the digest holds the aliased
channel wherever the `use` lands. The stated reason is narrower than the
actual guard; no action owed.

**C-4 — CLOSED.** Question 4 (malformed path-shaped tokens hard-fail) is
in the checker and verified red with named lines on all three demo
classes: the lane's `mangled.md` ("malformed test path") and
`mangled_case.md` ("malformed registry case id" **plus** the now-unclaimed
registry row — a double catch), and **my exact round-1 uppercase mangle,
which passed silently at `78badbc`, now exits 1 at `1bd885f`**. All exit
codes captured directly (my first capture in this round was polluted by a
`$(basename)` command substitution resetting `$?` — re-run cleanly, all
three exit 1). The narrowness of the rule (`::` ⇒ test path, `/` ⇒ case
id) is right: the map's other vocabulary carries neither separator. The
second half — the driver now pins the **full count line**
(`ok (39 scenario rows, 38 named tests, 13 registry rows all claimed)`) —
verified green on the real tree and verified **red** on the lane's
`demo/ws` workspace copy, whose map has one well-formed row
(`A-EXECUTING`) deleted: the checker alone passes it (`ok (38 scenario
rows, 36 named tests, 13 registry rows all claimed)`, exit 0 — reproduced
with `--test-list`), and the driver's `byte-equivalence:1` fails on the
absent marker (exit 1). The fail-open crack I demonstrated in round 1 is
closed from both sides.

**C-5 — CLOSED.** The `fault` battery now carries all four disjoint,
exhaustive lib partitions, with the composition and its origin stated in
the driver's docstring. Executed by me: `fault:3` → ok, `400 passed`
(38.3 s, the checked-artifact 165-key census); `fault:4` → ok, `914
passed` (52.2 s, the remainder). With round 1's `fault:1` (254) and
`fault:2` (1, release) on byte-identical Rust, the battery's four
partitions reconcile to the full **1569 / 0 / 1** — the driver's fault
battery now matches evidence row 2.1's claimed scope.

**C-6 — CLOSED.** The evidence's §3.3 now states the true topology in
place: **8 checker-verified forbid islands, not a crate-wide forbid**,
with the crate-root `#![allow(clippy::disallowed_methods)]` named at
`lib.rs:4-7` and the enforcement chain attributed to the checker's
masked-scan verification plus the structural digests plus the privacy
seals. Matches what I established in round 1.

**C-7 — CLOSED, with one self-healing residue.** (1) The must-wait clause
carries a dated, quote-preserving annotation in both places
(`GwzM5-8M5bNoFfDesign.md:988` block: "clause text above is frozen and is
not edited"; `CurrentProgramCheckpoint.md:1147-1157`: sentence "retained,
not struck, as the text the J-1 adjudication is against", the owed
M5b-IMPL settled review restated). (2) J-1's leg 4 was restated to its
true source — the D3 dual's round-2 re-verdicts, not the settled dual — and
I verified the corrected cites against the reports themselves
(`GwzM5-8D3Impl-ReviewCode.md:447-448`: all five M5b files byte-identical,
`store/tests.rs` exactly +2/−0; `GwzM5-8D3Impl-ReviewState.md:550-552`:
M5b four-filter 37/0, five files `git status`-clean). **The fact my
ratification rested on is intact; only its citation moved. J-1's
ratification stands.** (3) F-7 swept: SIGNED/INSERTED state restated
against every stale "unsigned"/"not inserted" sentence, signature date
corrected to 2026-08-24, and the phantom "~343 s" re-attributed to the
lane owner's session notes rather than "the A1 brief". (4) The checkpoint
tuple row was re-pinned with the carries-by-construction note — to
`78badbc`, which the round-2 code commit has already made **one commit
stale again**; the same carries-by-construction statement applies verbatim
(`1bd885f` is three script files over `78badbc`, zero `src/`). P4, the
literal-restatement treadmill's inherent lag, self-healing at the next
docs-only commit per the table's own note; recorded here so the next
reader need not re-derive it.

**C-1 / C-2 — OPEN BY DESIGN, where round 1 required them to be.** C-1
[P2] and C-2 [P3] now live on the A1-activation input register (§12.7 +
settled tuple §11.1), C-1 explicitly BLOCKING-FOR-A1 under L1-19. They are
no longer unrecorded debts of this gate; they are the A1 review's named
inputs. No new P0-P3 findings were opened by this round's verification;
the two P4 notes above (digest-hold wording; tuple-row lag) are recorded,
not graded.

## R2.3 Round-2 provenance and disclosures

Executed 2026-08-24 at `1bd885f`, same host and discipline as round 1. New
runs this round: driver `fault:3`/`fault:4`, `privacy` (8 probes, 150.7 s),
`byte-equivalence` (full-count marker green, g23 111/0); the three
malformed-map demos plus my round-1 mangle, exit codes captured directly;
the `demo/ws` dropped-row demo through both the bare checker (exit 0) and
the driver (exit 1); the aliased-residual suite run (8/8 green) and my
de-aliased variant (3/3 root probes fire). One environment incident,
disclosed: the host's data volume hit 100% (437 of 460 GiB used
machine-wide; my round-1 dev+release target contributed ~4 GiB) and one
suite attempt against the widen copy failed with ENOSPC before any
compile ran — I deleted my release-profile artifacts and incremental
caches, re-ran on a fresh target directory, and the results above are from
the clean re-run; my derived copies and targets were deleted afterwards.
The lane's `demo/{widen,ws,m4}` copies were left in place. All four trees
verified clean at the pins after all work. The peer report remains unread;
peer-axis facts cited here (J-NO-PUBLICATION-UNBORN, the leg-4
mis-attribution, Evidence [P3-x] ids) come from the coordinator's resume
message and the lane's merged records, not from the report.

## R2.4 FINAL VERDICT

**GO — final, for the R4b-G gate at the tuple gwz-core `1bd885f` /
gwz-cli `3cca145` / gwz-py `929efb0` / taut `f008419`. The round-1
conditional converts: the condition was satisfied on the record and
verified here, not waived.** Grades at close: **0 open P0/P1/P2 against
the gate object** — C-3, C-4, C-5, C-6, C-7 CLOSED-VERIFIED at `1bd885f`;
C-1 [P2] and C-2 [P3] stand OPEN on the A1-activation input register,
C-1 BLOCKING-FOR-A1 per L1-19, which is their required placement, with
the four live-residue rows and three cheap closures named for the A1
review to consume. J-1 remains ratified on its corrected citation.

One-paragraph justification: every mechanical claim this gate accepts has
now been executed twice on byte-identical Rust — the full 1569/0/1 train,
the seven batteries with the fault battery finally congruent to its own
name, cross-repo 139/0 and 330/0 — and the three guards the gate newly
relies on have each been driven to failure in the direction that matters
and shown to fail closed (root-binding probes firing on the unaliased
widening; malformed and dropped map rows red at checker or driver; the
census partitions pinned by count), with the one genuinely unclosable
channel (`as`-renamed re-exports) measured, declared in the guard's own
text, and held by the tree digest. The gate's single real defect — the
byte-equivalence battery's 13/39 — is not repaired and is not pretended
repaired: it is recorded PARTIAL in the acceptance record in words that
cannot be cited as green, graded P2, and bolted to the A1 activation
review's register where L1-19 makes it blocking, with its live residue
named row by row. A tree-acceptance gate whose evidence is twice-executed,
whose guards are demonstrably fail-closed, and whose one open debt is
measured, recorded, and routed to the review that can actually retire it,
has done what a gate is for. **R4b-G is GO.**
