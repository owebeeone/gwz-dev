# Thin-A1 amendment — Safety-axis review, round 1

Date: 2026-08-16

Axis: **Safety (adversarial)** — round 1 of the mandated dual review
(process/scope amendment tier, `GwzProcessOptimization.md` §4.2;
two-round remediation cap in force). Peer-blind: the parallel
Consistency-axis report was not read; its file is untracked in the
worktree and was not opened.

Object: `dev-docs/GwzM5-8ThinA1Amendment.md` (DRAFT), drafted at
`523af06` ("Record the thin-A1 operator decision; draft the gate-chain
amendment"). Review began at gwz-dev HEAD `d6cd2cb` and ended at
`c6602af` (second-lane escape-review filings landed mid-review);
`git diff d6cd2cb..c6602af -- dev-docs/GwzM5-8ThinA1Amendment.md` is
empty — the object is byte-identical across the window. Code evidence:
gwz-core committed state (files under `src/checked_artifact/` read via
`git -C gwz-core show HEAD:<path>` per the in-flight-edit caution;
all other reads from the working tree). All commands read-only; this
file is the review's only write.

Scope discipline: the operator's authority to descope (L1-28,
`GwzFasterProposal.md`) is **not under review**. Under review: whether
§3's soundness claim survives the actual code and contracts, and
whether §4's unwaived list suffices to keep thin A1 from silently
weakening a proven property. Remediations proposed below name
exceptions and restore disclosures; none reverses the descope.

## 1. What was physically traced and holds

The core §3 claim — "the coupling was a gate-chain sentence, not a
physical dependency of writing v1 records" — was tested against the
v1 writer's actual call graph. For the **write edges themselves** it
holds:

- **Merge-record writes.** v1 records ride the same frozen store seam
  v0 uses: `workspace_ops/merge/store/mod.rs:120-153` (`write_open`)
  → `write_atomic_verified` (`:287-323`: create-new temp, `sync_all`,
  `rename_durable`, parent `sync_dir`, byte read-back). Records live
  under `.gwz/merge/`, not the catalog Final directory. The only
  checked-artifact edge is `store/mod.rs:135` →
  `checked_artifact::entry::prepare_merge_store_parents` (committed
  `entry.rs:144-151`) → `CheckedArtifact::prepare_parent` on
  `STASH_BUNDLE_DIR` = `.gwz/stash/bundles` (`stash.rs:13`) — the
  legacy managed-parent edge the named residual explicitly keeps
  (R2-D defer-out §5 item 1).
- **Preservation-bundle writes.** The v1 bundle adapter
  (`merge/preserve/checked_bundle.rs:19-112`) calls only the v0
  entry-boundary operations
  (`observe/classify/replace_merge_preservation_bundle`). Same graph
  v0 runs in production.
- **No catalog edge.** Nothing on the v1 write path reaches
  admission, `recover_or_create`, or the catalog owner. Production
  gating verified on today's tree: `#[cfg(test)] mod v1_lifecycle;`
  (`workspace_ops/merge/mod.rs:23-24`); production decoders pin
  v1-off with typed `required_wave: A1` rejection
  (`GwzM5-8ProgressReviewF5.md` §2.2 structural sweep).
- **The durable kernel the writer exercises is R2-D scope and R2-D
  stays an A1 gate**: the four legacy rename edges
  (`transition.rs`/`residue.rs`) and the legacy Windows anchor are
  converted/retired by R2-D Phases 4.1/4.2 pre-A1, under the retained
  L2-04 hard gate, the RemPlan-4 :1082-1085 stop clauses, and the
  dual Phase 5 settled gate (`GwzM5-8R2D-Plan.md` §4).

**Mandate item 2 — interference residual (audit P3-3).** Verified
closed pre-A1 and in the A1→R2-E window. The audit's own hazard
condition is a completed catalog: "Not co-reachable with a live
catalog today (bootstrap not production-activated); becomes interior
ambiguity for the ten-slot grammar the moment a catalog completes"
(`GwzM5-8R2C2PublicationAudit.md:80-89`). The residual keeps
`recover_or_create` production-caller-free **through A1** (amendment
§1), the amendment retains "no production `recover_or_create` caller
lands before the Phase 4.3 coexistence decision" (§4), Phase 4.3 is
inside the retained A1 gate R2-D and decides before any activation
(`GwzM5-8R2D-Plan.md:414-424`, §9.2), and enforcement is structural,
not prose: the sealed-primitive call-count pin
(`interface_tests/capability_permit.rs:110`), the subsystem-wide
boundary checker with bare-identifier counting, and the per-commit
lane gate. First production catalog activation is an R2-E-window
reviewed decision. No unguarded interference path was found; the
residual scenario that remains is at R2-E activation time and is
fail-closed (see P3-3).

**Mandate item 5 — the M5b leg.** "T-6 + clean-tree re-cut" is a
faithful compression of what survives to the A1 boundary. The frozen
design's settle list (`GwzM5-8M5bNoFfDesign.md:950-955`) is: suites
green, ledger row, tripwires T-1..T-6, checkpoint update, clean-tree
re-cut. T-1..T-5 are unreachability tripwires "all inverted by A1's
checklist except T-6, which survives A1" (`:754-755`); the ledger/
checkpoint rows are process bookkeeping; suites-green is subsumed by
any settled tuple. T-6 **is** the F-1 v0 gate (Code F-1 P1: production
v0 resume executes forged two-parent-over-ff-able actions), defined
with its two named suites at `:783-790`, landed and green per the
checkpoint's M5b-IF record. Nothing bound was silently dropped.

**Mandate item 4 — unwaived items that check out.** Executed fault
evidence per converted family stays enforced where it always was: the
RemPlan §10 rule binds the *converting package*, and every converting
package (R2-D phases) remains pre-A1; §4 restates it. Migration
ineligibility is owned by the frozen I2 Compatibility Contract
(whitelist-only, archived records never migrate, conflicted status
never migrates — `GwzM5-8I2CompatibilityContract.md` §§2-5),
test-gated until A1 and reviewed at the retained A1 activation
review; its enforcement lives in no deferred package. L2-04 is
retained in §4 (precision issue at P3-1).

The findings below are where the soundness argument and the unwaived
list fail adversarial scrutiny.

## 2. Findings

### [P1-1] §3 misstates the acceptance state of the lifecycle A1 enables; the deferred chain holds its only scheduled independent acceptance and the closure of accepted-real P2 findings

**Amendment**: "The v1 writer rides the accepted R4b reverse
lifecycle, the frozen I2 contracts, and the already-accepted
amendments … None of those artifacts depend on consumer conversion
(R2-E), closure matrices (R2-F), or R3-R6 for their reviewed
correctness claims." (§3, first bullet.)

**Contradicting evidence.** "Accepted" is true of the architecture
(SHA-pinned design, ReviewState-5/ReviewFS-5) and the R1 interface
checkpoint — and false of the implementation the writer rides:

- `GwzM5-8ProgressReviewF5.md:56-59`: "P2 (rollback, ~1,941 lines),
  P3 (status/protocol, ~1,096), P4 (archive/GC, ~508) are implemented
  but **not yet independently accepted** — ~3.5k production lines of
  review debt ahead of R4b-G's settled-tree double review."
- `GwzM5-8M5bNoFfDesign.md:980-986` (frozen GO/GO **2026-08-16**, the
  same day as this amendment): M5b-IMPL waits on R4b-G because it
  "edits the same `v1_lifecycle` tree whose P2/P3/P4 lanes (~3.5k
  production lines) are implemented but **not yet independently
  accepted**".
- The settled-tree double review that was scheduled to create that
  acceptance is exactly the deferred R6
  (`GwzM5-8R4bP1P2-RemPlan-4.md:1140-1145`), and R3/R4 are the
  remediation vehicles for the accepted-real findings D3 and D4
  (`RemPlan-4:52-64`): D3 — "Complete rollback authority erases
  tracked private-prefix facts, invents live gitlink state, omits
  valid terminal rows, and has a selected-root ownership hole between
  physical completion and its durable successor" (Arch P2-3/4/5/6;
  Faults P2-1); D4 — "A fresh preservation ambiguity can lose to an
  executor diagnostic, while C7 counts a declarative vocabulary
  without executing each row through the production service" (Arch
  P2-7/8; Faults P2-2). RemPlan-4 "replaces those interfaces before
  changing their implementations" (:30-31); R1 froze only the
  checked-artifact substrate (:1034-1049); the D3/D4 implementation
  conversion is R3 (:1087-1100) and R4 (:1102-1112), whose own
  interface checkpoint (R3.1) has not run.
- The D3-class mechanism is live in the enabled code today: global
  dirty-summary equality as a recovery predicate in the v1 reverse
  path —
  `v1_lifecycle/authority/observe/reverse/preservation/cursor.rs:277/:280/:302/:345`,
  `…/preservation/phase.rs:187/:198`,
  `…/observe/finalization.rs:217` — precisely the "global dirty
  booleans as recovery authority" R3 item 3 removes.

**Failure scenario** (not corruption; recovery misclassification —
this program's core quality bar): post-A1, a user's `--no-ff` v1
merge crashes; abort/continue exercises rollback authority and
reconciliation carrying the open D3/D4 mechanisms (fact-erasing/
state-inventing rollback authority; a fresh preservation ambiguity
masked by an executor diagnostic). The reviews scheduled to close
those mechanisms are now behind the enablement instead of in front of
it.

**Compounding gap in §4**: "P0/P1/P2 handling … untouched" plus the
retained "A1 activation review" is self-colliding as written. If
L1-19 is genuinely untouched, the open accepted-real P2s (D3/D4) and
the ~3.5k-line acceptance debt block at that review and thin A1
stalls at its own retained gate; if they are not to block there, that
is a waiver and must be written as one. The amendment says neither.

**Minimal remediation (keeps the descope).** Add one paragraph to
§1's named residual and cross-reference it in §4: (i) name, as part
of the accepted residual, the v1_lifecycle P2/P3/P4 acceptance debt
(~3.5k production lines, F5 §2.1) and the open D3/D4 P2 classes whose
closure is R3/R4/R6; (ii) state that they ride to the **A1 activation
review** — dual, per §4.2's activation tier — which must either
subsume a settled-tree acceptance of the `v1_lifecycle` tree with an
operator-signed named-residual disposition for D3/D4, or treat them
as blocking per L1-19. This converts a silent weakening into a named
A1-gate exception.

### [P1-2] §2's completeness claim is false: live controlling sentences sequencing R2-R6/R3-R6 ahead of R4b-G are not named, and a live stop clause becomes unsatisfiable post-A1

**Amendment**: "No other live sentence is known to sequence R3-R6 in
front of R4b-G or A1. If the dual review finds one, it is added here
by remediation, not by a new document." (§2, closing.)

**Found (this review):**

1. `GwzM5-8R4bR2ConsumerCheckpoint.md:404` — "Only then may RemPlan-4
   R3 begin. **R4b-G remains behind R3-R6.**" Body text of the §15
   acceptance gate in a document the adopted R2-D plan lists as
   controlling (`GwzM5-8R2D-Plan.md:20-22`). Not a status paragraph;
   rulebook §7.3 does not auto-supersede it.
2. `GwzM5-8M5bNoFfDesign.md:987-989` — "The R4b-G gate itself is
   behind the RemPlan-4 R2-R6 chain … M5b inherits that
   transitively." Dependency statement of the **frozen GO/GO** M5b
   design — which §5 of this amendment declares "Untouched".
3. `GwzM5-8R4bG-EvidenceInventory.md:8/:121/:374/:597` — the R4b-G
   execution checklist is built as a "confirmation pass" after "the
   R2-R6 chain settles the tree", and :121 restates the superseded
   SCOPE CORRECTION verbatim. If R4b-G now runs at thin A1, this
   document's frame is affected; it appears in neither §2 nor §5.
4. Status lines `GwzM5-8R4bR1InterfaceCheckpoint.md:7` and
   `GwzM5-8R4bR1Interface-RemPlan.md:6` ("R4b-G remains gated by
   RemPlan-4 R2-R6") — arguably auto-superseded as status paragraphs
   under rulebook §7.3, but the amendment should say so rather than
   leave it to inference (`GwzM5-8Refactor.md:2242` already defers to
   the checkpoint and needs nothing).

**Stop-clause collision.** `GwzM5-8R4bR2ConsumerCheckpoint.md:375-386`
(§14) has nine stop conditions; the ninth stops R2 on "production
merge-v1 decode, write, migration, or dispatch reachability". Under
thin A1, R2-E and R2-F execute **after** A1 — i.e., in trees where
production merge-v1 is reachable by design. The amendment restates
only the five physical clauses of RemPlan-4 :1082-1085 (§4) and the
operator instruction cites "ConsumerCheckpoint §14" as still binding
(`GwzFasterProposal.md` §3 Step D). Unremediated, the first post-A1
R2-E package sits on a live controlling rule that orders it to stop.

**Failure scenario**: dueling controlling texts at the first post-A1
R2-E package — §14.9 and §15's closing sentence say stop/sequence,
the amendment says proceed. Whichever way the executor resolves it,
a controlling document is silently ignored; manufacturing exactly
that ambiguity is what L1-08's name-the-superseded-clauses duty
exists to prevent, and this amendment is that L1-08 deliverable.

**Minimal remediation**: add items 1-4 to §2 with dispositions
(superseded-as-A1-gate for :404 and M5b :987-989; reframe or banner
the EvidenceInventory in §5; declare the two status lines §7.3-
superseded), and give §14.9 an explicit re-scope: it binds pre-A1 R2
packages; from A1 on, production v1 reachability is governed by the
A1 activation review, with the remaining eight §14 bullets unchanged.

### [P1-3] The three-dual cap contradicts adopted §4.2 at two durable-transition kernels without acknowledging the override, and its supporting evidence citation points at an empty table

**Amendment**: §1 — "three duals maximum … all interior steps
single-axis with automatic escalation on P0/P1/P2"; §2.6 supersedes
the plan's Phase 3/Phase 4 "Gate: dual" sentences; §3 — the retained
duals sit on "the three places the program's history shows duals
finding architectural defects (per the checkpoint's process metrics
table)".

**Contradiction.** `GwzProcessOptimization.md:106-114` (adopted,
controlling): "Dual peer-blind review stays mandatory at: interface
freezes, **durable-transition kernels**, amendments, aggregate
settled-tree gates, activation, and release boundaries." The adopted
R2-D plan classifies the Phase 3 settle as exactly that kernel
("Gate: dual at the phase settle (the managed intent state machine is
a durable-transition kernel)", `GwzM5-8R2D-Plan.md:387-388`) and
justifies Phase 4's dual as "this phase rewrites the durable kernel
under live production flows" (`:426-428`). The operator instruction
reclassifies neither (its only classification hedge is Phase 1's
"if you still treat Idle↔Preparing as a durable-transition kernel").
The amendment supersedes the *plan's* sentences but neither names
§4.2's clause in §2 nor records the reduction as an operator override
of the adopted process rule — while simultaneously invoking §4.2 as
the authority for its own review tier (status block). The operator
may override a rule the operator adopted (L1-28); a process/scope
amendment doing so silently is the "silent contradiction" branch, and
under L1-08 the overridden clause must be named.

**Evidence citation.** The checkpoint's process-metrics table
contains no data: header plus "(baseline starts with the next
accepted checkpoint)" (`CurrentProgramCheckpoint.md:481-485` at
`c6602af`). §3's "per the checkpoint's process metrics table" cites
evidence that does not exist.

**The record that does exist cuts the other way on sufficiency.**
Escalation-on-P0/P1/P2 fires only when the scheduled axis finds
something; the program's recorded cases are axis-divergent: R2-C2
settled round 3 — **State-2 GO (0 open P0-P2)** while **Code-3 NO-GO
(P2-1, probe-proven** directory-interior acquisition-window gap**)**
on a durable-kernel object (`CurrentProgramCheckpoint.md:358-361`);
the M5b freeze's production-v0 P1 (F-1) was a single-axis-invisible
Code catch at a dual gate; and the escape-amendment dual round filed
mid-review returned Code 3×P1 vs State 6×P1 with blind convergence on
**one** shared finding (`CurrentProgramCheckpoint.md:187-196`).
Phases 3 and 4 are the C2-class objects of this program.

**Minimal remediation (keeps the cap).** Add to §2: "GwzProcess-
Optimization.md §4.2's mandatory-dual row for durable-transition
kernels is operator-overridden (L1-28, GwzFasterProposal.md §2) for
the R2-D Phase 3 and Phase 4 settles only; compensating controls:
single-axis with automatic escalation, the retained dual Phase 5
settled-tree gate behind both phases, and the §6 tripwire — any
escaped P0-P2 attributable to the relaxed tier reverts it." Fix or
delete the metrics-table citation; the cap can rest honestly on
operator authority plus those named controls. (Recommended, not
required: schedule the Code axis for the Phase 4 settle, per the
round-3 precedent.)

### [P2-1] §4's unwaived list omits the R2-F/R5 native-evidence release gates — the only remaining native-platform barrier between the enabled v1 writer and shipped users

§4 restates D2 as release-gated but not its sibling:
`GwzM5-8R2CCatalogBootstrapAmendment.md:667` — "Linux and Windows
native fault evidence remains an **R2-F release gate**" — and the
R5-class exact-tree three-platform jobs
(`GwzM5-8R4bP1P2-RemPlan-4.md:1131-1138`, "A compile or release build
alone is not platform evidence"; `GwzM5-8R2F-EvidenceMap.md`
adopted-in-part). With §1 branding R2-F "hardening … scheduled after
A1 or in parallel", a release manager can read the descope as moving
the release gate too, and the first release cut carrying the
production-compiled v1 writer ships without native fault evidence for
the converted durable kernel. One §4 bullet fixes it: "the :667
native-evidence release gate and R5-class exact-tree platform jobs
are unchanged; thin A1 moves no release gate."

### [P2-2] §3's platform-green claim omits the known-red leg

§3: the residual graphs are "currently green on Windows (matrix run
13, 1322/0/1), macOS (1359/0/1), and the ubuntu ext4 probe." The same
checkpoint section records the fourth leg: "the ubuntu-24.04-arm leg
stays **1094/266/1** (one EBADF substrate fault + cascades, its own
tracked package)", moved off the A1 gate list by the same decision.
A soundness argument resting on "today's graphs are green" must name
the excluded leg and its accepted disposition; as written it
overstates the evidence base for the accepted residual. Add the arm
leg and its off-gate disposition to §3.

### [P3-1] §2.6 supersedes the Phase 3/Phase 4 "Gate: dual" sentences wholesale; Phase 4's sentence also carries "retained-reader harness (L2-04) is a hard gate in addition to review"

§4 retains L2-04 generally, which closes the waiver reading, but
§2.6 should say the supersession is of the **tier term only** — the
L2-04 hard-gate clause and the rationale clauses in those sentences
stand.

### [P3-2] Citation drift in §2's verbatim quotes

§2.1 cites `GwzM5-8R4bP1P2-RemPlan-4.md:4-6` (text sits at :5-7);
§2.3 cites `:1240` (text sits at :1238). Line-pinned quotes in a
controlling amendment should match the committed file; refresh at
remediation.

### [P3-3] Informational — the Phase 4.3 criterion is satisfiable by a recorded-but-unexecuted relocation decision

Audit P3-3's hazard arises "the moment a catalog completes"
(`GwzM5-8R2C2PublicationAudit.md:86-88`). Phase 4.3 requires
grammar-legalization **or** a recorded quarantine/relocation decision
before activation, while relocation *execution* is an R2-F candidate.
If R2-E activates the catalog on a recorded-but-unexecuted decision,
still-present legacy grammar classifies malformed/foreign → global
ambiguity — fail-closed, never auto-deleted, but availability-
affecting. This is unchanged by thin A1 (it existed under the old
chain); the R2-E activation package should price it. No thin-A1
action required.

## 3. Verdict

**NO-GO** — round 1 of 2. Findings: **0 P0 / 3 P1 / 2 P2 / 3 P3**.

No P0: no proven property is broken by thin A1 as documented — the
v1 write edges are physically the v0 graphs plus the R2-D-retained
kernel, and the P3-3 interference residual is guarded pre-A1 and in
the A1→R2-E window by verified structural gates. Every P1 is a
naming/disclosure defect in the amendment's own L1-08 deliverable
class, remediable in one merged round without touching the operator's
descope: name the lifecycle acceptance debt and the D3/D4 P2 classes
as an explicit A1-activation-review exception (P1-1); name the missed
live sentences and re-scope ConsumerCheckpoint §14.9 (P1-2); name the
§4.2 override and fix the empty-table citation (P1-3).
