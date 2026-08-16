# Thin-A1 amendment — Consistency-axis review, round 1

Date: 2026-08-16
Axis: **Consistency** (adversarial, peer-blind; the Safety-axis review
runs in parallel and was not read — it did not exist in the tree at
review time). Round 1 of the two-round remediation cap.

Object: `dev-docs/GwzM5-8ThinA1Amendment.md` (DRAFT, 148 lines),
introduced at `523af06` ("Record the thin-A1 operator decision; draft
the gate-chain amendment") and unchanged since. Review executed at
HEAD `d6cd2cb` ("File the escape amendment State-axis round-1 report
(second lane)"); `git diff HEAD` clean for `dev-docs/`, so working-tree
text equals committed text. The train commit `523af06` touched exactly
`CurrentProgramCheckpoint.md` (+131/−15), `GwzFasterProposal.md` (new),
the amendment (new), and one session yaml — nothing else.

Scope note: the operator decision itself (`GwzFasterProposal.md`) is
not under review. Under review is whether the amendment documents it
correctly, completely, and without contradiction.

Grounding set read: `GwzFasterProposal.md`; `CurrentProgramCheckpoint.md`
(current and at parent `2fc98ab` for the superseded SCOPE CORRECTION);
`GwzM5-8R4bP1P2-RemPlan-4.md` (cited regions + full R4b-G/gate sweep);
`GwzM5-8R2D-Plan.md` (full); `GwzProcessOptimization.md` (full);
`AgentProcessRules.md` L1-06/L1-08/L1-28 (+§7.3 via the checkpoint
header); sweeps over `GwzM5-8Refactor.md`,
`GwzM5-8R4bR2ConsumerCheckpoint.md` and its RemPlan, `GwzM5-8M5bNoFfDesign.md`
§8.2, `GwzM5-8A1DecisionPacket.md`, `GwzM5-8R4bG-EvidenceInventory.md`,
`GwzM5-8R4bR1InterfaceCheckpoint.md`, `GwzM5-8R4bR1Interface-RemPlan.md`,
`GwzMergeCheckpoint-v0.10.5.md`, `GwzM5-8ProgressReviewF5.md` §9.
Quote verification was done byte-wise (whitespace-normalized only;
dash/apostrophe code points compared exactly).

## Findings — P0

None.

## Findings — P1

### [P1-1] Live gating sentences in the controlling R2 documents are missing from §2, falsifying the amendment's completeness claim

The amendment closes §2 with (`GwzM5-8ThinA1Amendment.md:80-82`):

> "No other live sentence is known to sequence R3-R6 in front of R4b-G
> or A1."

Three live sentences in the two controlling R2 documents do exactly
that and are not named:

- `GwzM5-8R4bR2ConsumerCheckpoint.md:21-23`: "R3-R6 in
  `GwzM5-8R4bP1P2-RemPlan-4.md` remain required after R2. R4b-G does
  not begin until those phases and their independent settled-tree
  reviews pass."
- `GwzM5-8R4bR2ConsumerCheckpoint.md:404`: "Only then may RemPlan-4 R3
  begin. R4b-G remains behind R3-R6."
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:605-607`: "R2-D through
  R2-F follow accepted R2-C. R3-R6 and R4b-G shared-router/cross-driver
  integration follow, then the two final full-tree R4b reviews. M5b and
  A1 remain later."

These are not status paragraphs; they are normative sequencing text in
documents the adopted R2-D plan lists as controlling
(`GwzM5-8R2D-Plan.md:21-24`: the ConsumerCheckpoint for R2 package
definitions, its RemPlan "controls where it differs"; the
ConsumerCheckpoint §14 stop conditions "bind every phase", plan §4).
An implementor consulting the R2 authorities finds R4b-G/A1 still
gated on R2-E through R6, with no precedence trail to the amendment —
the exact condition L1-08's verification clause forbids ("both old and
new documents contain an unambiguous precedence trail",
`AgentProcessRules.md:241-251`).

Minimal remediation: add the three clauses to §2 under the amendment's
own remediation mechanism (§2: "it is added here by remediation, not by
a new document"), with the §2.2-style treatment (superseded as
A1/R4b-G gates; the chain remains the hardening program's own order;
"M5b and A1 remain later" superseded by the thin-A1 ordering), and give
the two documents a disposition in §5 (post-review one-line banner or
an explicit reliance on the checkpoint pointer — stated, not implied).

### [P1-2] The frozen M5b dependency statement still routes R4b-G behind the R2-R6 chain, and §5 records the M5b design as "Untouched"

`GwzM5-8M5bNoFfDesign.md:987-989` (§8.2 "Dependency statement
(explicit, per the task)", part of the design FROZEN GO/GO at
`66117b0`):

> "The R4b-G gate itself is behind the RemPlan-4 R2-R6 chain (scope
> correction of 2026-08-16, `CurrentProgramCheckpoint.md:108-120`); M5b
> inherits that transitively and takes no dependency on its internals."

This clause cites, as its authority, the very SCOPE CORRECTION the
amendment supersedes in §2.4. M5b is one of thin A1's two inputs, and
the checkpoint routes merge timing through this exact statement
("IMPL merge waits for R4b-G per the frozen dependency statement",
`CurrentProgramCheckpoint.md:291-292`). Yet the amendment's §5 table
(`GwzM5-8ThinA1Amendment.md:140`) records: "I2 contracts, M5b design,
A1 decision packet | Untouched." — an affirmative statement that the
M5b design carries nothing this amendment affects. A reader computing
when M5b-IMPL may merge from the frozen design concludes R2-E-R6 must
settle first, contradicting the new controlling rule.

Minimal remediation: name the §8.2 clause in §2 (superseded the same
way as §2.2-2.3: R4b-G sits behind the R2-D settled gate alone; M5b's
transitive inheritance now binds to that gate; the design is not
rewritten), and correct the §5 M5b row from "Untouched" to
"dependency-statement clause §8.2 superseded per §2.x; not rewritten".
(The A1 decision packet row was verified accurate: it contains no
gate-chain sequencing sentence — its pre-A1 references are decision
timing for D1/D3, both discharged.)

## Findings — P2

### [P2-1] Two of the three RemPlan-4 line citations in §2 are wrong (quotes themselves byte-accurate)

- §2.1 (`GwzM5-8ThinA1Amendment.md:46`) cites
  "`GwzM5-8R4bP1P2-RemPlan-4.md:4-6` (Status)". The Status field opens
  at :5 and the quoted words — "R2 is next and R4b-G remains gated by
  R2-R6" — span **:6-7**; :4 is blank and the cited range excludes the
  line carrying "R2-R6".
- §2.3 (`GwzM5-8ThinA1Amendment.md:56`) cites
  "`GwzM5-8R4bP1P2-RemPlan-4.md:1240`". The file is 1238 lines; "Only
  then does step 4 of the controlling six-step sequence—R4b-G—begin."
  is at **:1238**. Line 1240 does not exist.

§2.2's ":1142-1143" is exact. All three quoted texts were verified
byte-accurate (dash types included: hyphenated "R2-R6" matching
RemPlan-4 and the old checkpoint; en-dashed "R2–R6" in §2.5 matching
`GwzM5-8R2D-Plan.md:19`). Since these citations become the permanent
supersession record that the post-review banner points at, fix the two
ranges (":5-7" or ":6-7"; ":1238").

### [P2-2] §3's support citation "per the checkpoint's process metrics table" points at an empty placeholder

`GwzM5-8ThinA1Amendment.md:102-106`: "…the three places the program's
history shows duals finding architectural defects (per the checkpoint's
process metrics table)." The checkpoint's only metrics table
(`CurrentProgramCheckpoint.md:472-476`, "Metrics (per checkpoint; §6 of
the optimization plan)") contains a single row: "(baseline starts with
the next accepted checkpoint)" — no data. The claim itself is
supportable, but from `GwzProcessOptimization.md:114-117` ("the
recorded frame-divergence catches (State GO vs Arch NON-GO) happened at
settled-tree and interface gates — which keep dual review"), not from
the checkpoint table. Minimal remediation: recite to
`GwzProcessOptimization.md` §4.2's guardrail evidence, or drop the
parenthetical.

### [P2-3] The Idle↔Preparing conditional is dropped from the caps restatement

Instruction (`GwzFasterProposal.md:55-57`): dual only at "(a) the Phase
0 interface freeze, (b) the Phase 1 admission kernel **if you still
treat Idle↔Preparing as a durable-transition kernel**, and (c) the
Phase 5 settled-tree gate." The amendment
(`GwzM5-8ThinA1Amendment.md:40-42`) restates: "**three duals maximum**
(Phase 0 interface freeze, Phase 1 admission kernel, Phase 5
settled-tree gate)" — the (b) conditional is silently dropped, turning
a conditional dual into an apparently unconditional one. The
checkpoint's consequence bullet (`CurrentProgramCheckpoint.md:105-111`)
drops it identically, so the two same-train texts agree with each other
but both strengthen the instruction. No execution impact today: the
condition presently holds — the adopted plan treats it as a kernel
("admission's Idle↔Preparing is a durable-transition kernel (mandated
dual under §4.2)", `GwzM5-8R2D-Plan.md:316-317`) — and "maximum" keeps
the cap a cap. Minimal remediation: restore the conditional or append
"(condition presently holds per `GwzM5-8R2D-Plan.md` §4 Phase 1)".

### [P2-4] Secondary surviving sentences (status-class, deferred, or draft) also escape §2's closing claim

Each still sequences R2-R6/R3-R6 in front of R4b-G; each has a
mitigating frame, which is why they are P2 and not P1 — but the §2
claim "No other live sentence is known…" is falsified by greppable
text, and none carries a precedence trail:

- `GwzM5-8R4bR1InterfaceCheckpoint.md:5-7` (Status: "…R2 may proceed,
  while R4b-G remains gated by RemPlan-4 R2-R6") and `:445` ("RemPlan-4
  R3-R6 still follow R2 before R4b-G."). Mitigation: status paragraphs
  are checkpoint-superseded by the rulebook §7.3 blanket rule.
- `GwzM5-8R4bR1Interface-RemPlan.md:5-6` (Status: same clause) and
  `:556` ("RemPlan-4 R3-R6 still precede R4b-G.").
- `GwzM5-8Refactor.md:2242`: "Checked-artifact R2-R6 precede R4b-G.
  Status: see `CurrentProgramCheckpoint.md`." Mitigation: the inline
  pointer. Aggravation: this sentence is the live end of a pointer
  chain — `GwzMergeCheckpoint-v0.10.5.md:128-130` resumes "R4b-G, M5b,
  and A1 activation in the dependency order defined by
  `GwzM5-8Refactor.md`", and the current checkpoint's Next-ordered-
  actions item 6 (`CurrentProgramCheckpoint.md:469-470`) still cites
  that resume order.
- `GwzM5-8R4bG-EvidenceInventory.md:5-8` (DRAFT pending adoption;
  premise: "executing R4b-G (after RemPlan-4's R2-R6 chain settles the
  tree) is a confirmation pass"), `:121-123` (quotes the superseded
  SCOPE CORRECTION as gate authority), `:567`/`:597`/`:629` (evidence
  churn analysis premised on pre-G R2-R6). Mitigation: DRAFT, adopted
  by nobody yet.

Minimal remediation: one line each in §2 by remediation, or a single
§5 disposition row routing all four to the same post-review banner /
next docs pass; the EvidenceInventory additionally needs its premise
re-based to post-R2-D at adoption (its content, not just its banner,
assumed R5 three-platform jobs and R6 re-reviews complete before G).

## Findings — P3 (informational)

- **[P3-1] Typography in "verbatim" quotes.** The §1 blockquote differs
  from `GwzFasterProposal.md` §2 by one code point (U+0027 for the
  proposal's U+2019 in "M5b's"); the checkpoint's "quoted verbatim" §2
  block normalizes three (’→', "→", "→"). Words are otherwise
  byte-identical (verified programmatically). Cosmetic; note if a
  byte-exactness gate ever hashes these quotes.
- **[P3-2] "the ubuntu ext4 probe"** (`GwzM5-8ThinA1Amendment.md:100`)
  names a leg no live document names. The checkpoint records "Three of
  four platform legs green" with the ubuntu-24.04-arm leg red
  (1094/266/1, its own tracked non-gating package;
  `CurrentProgramCheckpoint.md:227-231`). The §3 sentence is accurate
  per-leg but silent on the red arm leg; align naming with the
  checkpoint and optionally note the exception.
- **[P3-3] Necessary-vs-sufficient phrasing** in §4
  (`GwzM5-8ThinA1Amendment.md:125-126`): "no production
  `recover_or_create` caller lands before the Phase 4.3 coexistence
  decision" could be misread as licensing a caller after that decision,
  pre-A1. The binding forms are §1's quoted residual ("stays without a
  production caller") and `GwzM5-8R2D-Plan.md:470-472` ("first
  activation is an R2-E/A1-adjacent decision behind the Phase 4.3
  coexistence criterion"). Cite the full form.
- **[P3-4] Same-train checkpoint residue** (checkpoint-side): Next-
  ordered-actions item 6 (`CurrentProgramCheckpoint.md:469-470`) still
  reads "platform-matrix acceptance … → R4b-G → M5b → A1 per
  `GwzMergeCheckpoint-v0.10.5.md` resume order" without the R2-D settle
  link the rewritten gate-chain paragraph installs 230 lines earlier.
- **[P3-5] "already-accepted … durable cursor"**
  (`GwzM5-8ThinA1Amendment.md:89-93`) repeats the instruction's own
  wording (`GwzFasterProposal.md:46-48`), while the same-train
  checkpoint carries both "wire accepted" (:203-204) and "D3 amendment:
  State re-verdict GO at `e9396a9`; Code re-read pending; acceptance
  ritual (§7 contract annotations) on its GO" (:293-295). Fidelity to
  the instruction is correct; the checkpoint-internal tension is noted
  for the record, no amendment change required.

## Verified clean

- **Fidelity to §2 of the instruction.** The §1 blockquote is the Thin
  A1 paragraph, complete (P3-1 typography aside). The named accepted
  residual is echoed with all three components intact and unweakened
  (call graphs through A1; `recover_or_create` callerless; legacy
  writers until R2-E), matching `GwzFasterProposal.md:38-44`, and its
  §5-items-1-5 mapping matches the adopted plan's defer-out list. The
  no-further-pre-A1-I2-trains rule is carried with the correct
  operative content (non-gating, second-lane; A1 ships on frozen
  contracts + accepted amendments, restated at §3 bullet 1). The
  escape-review-in-flight carve-out is rendered exactly
  (`GwzM5-8ThinA1Amendment.md:139` vs `GwzFasterProposal.md:175-177`
  and the checkpoint's second-lane bullet), and honored in practice
  (HEAD `d6cd2cb` files that report as second-lane).
- **Verbatim accuracy.** All six §2 quotes byte-accurate under
  whitespace normalization, including dash discipline per source; the
  §2.4 two-fragment quote with marked elision matches the pre-train
  checkpoint at `2fc98ab:137-147`, and the elided middle is genuinely
  non-gating material. §2.2's line range exact; §2.5/§2.6 source
  sentences exist where claimed (plan §1 first bullet; §6; §4 Phase 3
  "Gate: dual at the phase settle" / Phase 4 "Gate: dual"; §9 item 6
  tier confirmation).
- **Internal consistency with the same-train checkpoint.** Three-dual
  tier record, second-lane list, and moved-off/non-gating list agree
  between the two texts (P2-3's shared conditional drop and P3-4/P3-5
  aside). The checkpoint quotes the full §2 and names the file as the
  L1-28 decision, matching the amendment's authority paragraph. The
  SCOPE CORRECTION replacement happened in the same commit that files
  the draft, exactly as §2.4 and §5 claim.
- **L1-08 form.** Superseded clauses named verbatim (§2); new
  controlling rule stated (§1); reason given (§3); affected documents
  listed (§5), with "affected interfaces and tests" discharged by the
  no-wire/no-type/no-outcome declaration; review class correct —
  process/scope amendment → mandated dual per `GwzProcessOptimization.md`
  §4.2 ("amendments" in the mandatory-dual list), cross-model per §4.3,
  two-round cap per §4.1. §4.2 prescribes no axis pairing, so
  Consistency+Safety is a legal instantiation. L1-28 and L1-06
  satisfied (decision embodied in controlling artifacts; single-writer
  lane respected).
- **RemPlan-4 discipline.** The amendment does not rewrite RemPlan-4
  (train diff confirms: RemPlan-4 untouched at `523af06`) and defers
  the one-line banner to after this dual review — exactly the
  instruction's Step B ordering.
- **Scope containment.** No I2 wire reopened, no types, no
  C3/R2-E/escape scope folded in; §6's discharge and remediation terms
  match the instruction's cap language.

## Verdict

**NO-GO** — 2 × P1, 4 × P2, 5 × P3; GO requires zero P0/P1.

Both P1s are omissions of supersession targets, not misstatements of
the operator decision; the gate-chain rule itself is documented
faithfully. The amendment pre-agreed the fix path in §2 ("added here by
remediation, not by a new document"), so a single merged remediation
round — adding the ConsumerCheckpoint/ConsumerCheckpoint-RemPlan and
M5b §8.2 clauses to §2, correcting the §5 M5b row, fixing two line
citations, and the P2-2/P2-3/P2-4 one-liners — clears every P0/P1/P2
found on this axis within the two-round cap.
