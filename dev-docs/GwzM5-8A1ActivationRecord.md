# GwzM5-8 A1 Activation Record

Date: 2026-08-25 (drafted post round-2 GO/GO; finalized at the landing
commit). Author: the implementation lane (lane owner). Status at
drafting: **AWAITING THE OPERATOR SIGNATURE (§1) — the landing commit
halts on it.**

This is the activation record the A1 review's verdicts are conditioned
on. It consumes the A1 input register **directly** from
`GwzM5-8R2DSettledTuple.md` §11 + `GwzM5-8R4bG-Evidence.md` §12.7-12.9
+ `GwzM5-8M5bImplSettled-Review.md` §7 (per L9 below; the checkpoint's
eight-item summary is a pointer, not the register). It folds, verbatim,
the Completeness axis's landing specification **L1-L14**
(`GwzM5-8A1Activation-ReviewCompleteness.md` §"Round 2", "THE LANDING
SPECIFICATION") and the Safety axis's round-2 attachments
(`GwzM5-8A1Activation-ReviewSafety.md` §R2.2 conditions (a)-(c),
§R2.7/[P2-R2-1], §R2.9). The verdicts it lands under:

> **GO — round 2, Safety axis: the delivered A1 activation package is
> accepted against the round-1 enumeration; R4's ordinary-start V0
> floor is an accepted named residual with binding record conditions;
> 0 P0 · 0 P1 · 1 P2 (landing-train: the four-class probe-harness
> cure) · 5 P3.**

> **VERDICT: GO — round 2, Completeness axis — 0 P0 / 0 P1 / 0 P2 /
> 2 P3 ([P3-7] decode.rs:123 typed-twin conversion, [P3-8] unnamed
> carriers); conditioned on the landing record set carrying L1-L14
> verbatim — above all L1's operator-signed D3/D4 disposition, the
> recorded landing halt — and on L7/L8 joining the fold list.**

---

## 1. L1 — the branch-(a) acceptance and the operator-signed D3/D4 disposition

**Fold (verbatim requirement):** "The branch-(a) acceptance: this
dual's settled-tree acceptance of `v1_lifecycle` + the operator-signed
D3/D4 named-residual disposition over the current site set
(`cursor.rs:283-308/:310-335` fallthroughs, `phase.rs:187,198`,
`finalization.rs:217`, `cursor.rs:498`), naming as members: the
service-level abandonment hole (L5), C-2's owner (L6), the perf
pricing consumed (318.71 s, no optimization owed), and the R4
partial-engagement scope (L2). The landing halts on this signature."

**Discharge.** The subsumption statement ([P2-3]): the R2-D settled
dual (round 2, GO/GO, `GwzM5-8R2DSettled-Review{Code,State}.md`)
accepted the settled tree containing `v1_lifecycle` whole; the D3/D4
dirty-boolean residual sites are interior to that accepted object, and
their disposition here names them individually rather than re-opening
the acceptance. The current site set, re-verified in the activation
tree (all paths under
`src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/`
unless noted): `cursor.rs:283-308` (`stash_complete` fallthrough),
`cursor.rs:310-335` (`reset_complete` fallthrough), `cursor.rs:498`
(dirty-summary default comparison), `phase.rs:187` and `:198`
(dirty-summary comparisons), and
`…/authority/observe/finalization.rs:217` (`is_dirty` disjunct).

Named members of the residual set: the service-level abandonment
witness (§5/L5 — discharged by the committed probe, see there); C-2's
owner (§6/L6 — the R2-F fixtures/native-evidence lane); the perf
pricing consumed (318.71 s citable, signed 2026-08-24; no optimization
owed by A1); and the R4 partial-engagement scope (§2/L2 — owned by
M5c).

**THE OPERATOR SIGNATURE:**

> ⟨SLOT: the operator's one-line reply, verbatim, with date. Branch
> (a) — "the D3/D4 dirty-boolean residual over the site set above is
> ACCEPTED AS A NAMED RESIDUAL; remediation, if any, is post-A1 work
> scheduled by the operator" — or branch (b), remediate first, in
> which case this record does not land until the remediation train
> and its re-verdicts complete.⟩

## 2. L2 — the activation's true scope (and Safety condition (a))

**Fold (verbatim):** "A1 enables the v1 writer — `--no-ff` writes v1
end-to-end and the seven-shape whitelist migration is live — with
`ACTIVE_WRITER_FLOOR = V0`; ordinary/custom starts continue on v0
until the production v1 ordinary-start owner lands (root participants,
dry-run prediction, drift/conflict surfaces, v0 event stream); raising
the floor is the one-line `version.rs:39` change."

**The carrier milestone, named ([P3-8] / Safety condition (a)):**
**M5c — the production v1 ordinary-start owner.** Minted by this
record as a first-class milestone on the register. M5c owns: the v1
ordinary-start surfaces (root participants, dry-run prediction,
drift/conflict response, the event stream on v1), and **the
`version.rs:39` floor raise, which lands WITH M5c as ONE reviewed
change** (contract §9's "one reviewed change" discipline transfers to
the remainder — Safety §R2.2 condition (a)). Scheduling is the
operator's, relative to R2-E/R2-F.

Safety condition (b) is discharged by the dated annotation on the
frozen creation matrix: `GwzM5-8I2CompatibilityContract.md` §2, dated
2026-08-25, quote-preserving. Safety condition (c) is recorded as a
standing obligation on the release lane: the retained-reader "A1
generation" manifest work describes the SHIPPED behaviour
(ordinary=v0, no-ff=v1), not the contract row, until M5c lands.

## 3. L3 — the PARTIAL byte-equivalence statement

Restated verbatim from the R4b-G acceptance: the byte-equivalence
battery is **PARTIAL — 7 proven + 6 refusal-pinned of 39 — not met,
and must not be cited as green** (`GwzM5-8R4bG-Evidence.md` §12.8).
Unchanged by the activation package; the scenario map still binds
39/41/13 (re-verified by the Completeness axis in round 2).

## 4. L4 — the escape packages' schedule

**Fold (verbatim):** "The escape implementation packages follow
activation per `GwzM5-8A1DecisionPacket.md` §4 step 3 ('With/after
A1'), which is the controlling schedule for Q3's 'at or before A1';
they remain second-lane, BLOCKED ON OPERATOR HANDOFF."

## 5. L5 — the service-level abandonment witness

The witness was verified ABSENT from the delivered package
(Completeness §R2.4). Disposition: **the commit-the-probe arm** — the
landing train commits the probe (NotStarted action, Normal and NoFf,
asserting EQUAL service-level refusal shapes), with the structural
corroboration on record at `GwzM5-8M5bImpl-ReviewState.md:465-472`.
Probe as landed: ⟨SLOT: test name + file:line + verbatim green tail
from the landing-train report⟩.

## 6. L6 — owners at register close

- **C-2's four unfixtured scenarios** → the **R2-F
  fixtures/native-evidence lane** (rides the same regeneration train
  as the archived T-5 candidate pair, §11.2's OWNED-CARRIER row).
- **The 18-UNBOUND per-scenario record debt, the archive-equivalence
  mechanism decision, and the two archive shapes riding [P2-1] option
  (i)** → **R2-E's archive/GC consumer sub-package** (one named
  owner; chartered with R2-E's binding-obligation ledger). The
  gc_archived no-production-caller allowance (`archive.rs:110`,
  reasoned, route named) is the same owner's.
- **Cheap closures (i)/(ii) of `GwzM5-8R4bG-Evidence.md` §12.7**
  (registry rows for the unbound scenarios; one parametric
  `adapt_open` refusal test over every non-whitelisted scenario):
  ruled **NOT satisfied by the precheck walk test** — the precheck
  reads state+mode and never drives `adapt_open`, so the walk test
  covers a different layer. Both closures are **OWNED, together, by
  the same R2-E archive/GC consumer sub-package** (the registry rows
  and the parametric test land as one package).

## 7. L7 — L2-05 and the battery scope

"L2-05 stays unwired with the multi-repo-checkout blocker recorded in
the workflow comment (`checked-artifact-boundary.yml`; the manifest
asserts against `gwz-cli/docs/*` resolved from the workspace root and
the job checks out gwz-core alone); owner: **the R2-F release-lane CI
work — the multi-repo checkout of tuple §11.3 item 7 is the cure**
(Safety [P3-R2-3]: accepted, not discharged; STAYS OPEN on the
register); the driver's fault/byte-equivalence batteries stay
local/release-lane by recorded scope decision." The stale-docs drift
class is independently CI-guarded by gwz-cli's `g00` docs-match test
(Safety §R2.3).

## 8. L8 — the landing is a coordinated commit

The landing is one coordinated `gwz commit`: `gwz.conf/gwz.lock.yml`
re-pinned equal to member HEADs at the commit; the checkpoint
Exact-tuple table restated literally; the boundary digests and driver
count markers applied at their final-tree values; the probe-harness
cure applied per the recorded lane-owner ruling — **as corrected by
Safety [P2-R2-1] to the FOUR cure classes**: (a) the allow-emit fix
(verified sound — the checker remains the rejector; F-3 unweakened),
(b) the digest re-pins, (c) the sentinel reinstated UN-GATED
(`const _: &str = v1_lifecycle::COMPILER_ROOT_SENTINEL;` +
`pub(super) const COMPILER_ROOT_SENTINEL…`), (d) the harness
seam-surgery string updated to the reshaped store re-export block.
Applied values: ⟨SLOT: the final pin table old→new with attribution,
and the final count-marker values, from the landing-train report⟩.

The base tuple (restated literally from the checkpoint): gwz-core
`26f48f5` / gwz-cli `3cca145` / gwz-py `929efb0` / taut `f008419`.

## 9. L9 — register consumption

"The activation record consumes the A1 input register from
`GwzM5-8R2DSettledTuple.md` §11 + `GwzM5-8R4bG-Evidence.md`
§12.7-12.9 + `GwzM5-8M5bImplSettled-Review.md` §7 directly; the
checkpoint's eight-item summary is a pointer, not the register."

Consumed at this landing: C-1 (closed at `26f48f5`), the signed perf
pricing (318.71 s), the T-5 narrowing (judged by the review;
candidate pair archived, §11.2 carrier row), the M5b settled GO, the
PARTIAL statement (§3), the ~3.5k acceptance-debt named exception,
and the D3/D4 residual dispositions (§1 — pending the signature).
Open past landing, each with a named carrier: the operator signature
itself (§1 — the halt); C-2 + 18-UNBOUND + archive-equivalence (§6);
L2-05 CI wiring (§7); the T-5 pair regeneration (§11.2 R2-F row);
M5c (§2); the escape packages (§4).

## 10. L10 — F5 §9 one-line dispositions (items 2, 6, 7, 8, 10)

So the name "pre-A1 checklist" closes honestly
(`GwzM5-8ProgressReviewF5.md` §9):

- **Item 2** (atomic P1 slice; commit-per-accepted-checkpoint; fmt in
  CI): the practice legs are long adopted (the landing ritual IS
  commit-per-accepted-checkpoint); `cargo fmt --check` remains
  release-lane only — **ADOPTED-AS-IS** (lane-owner ruling; fmt runs
  in every landing gate locally and in `release.yml`).
- **Item 6** (per-predicate rejection names): **NOT ADOPTED pre-A1.**
  The typed-refusal families ship with per-family messages, not
  per-predicate names. Post-A1 hardening candidate; no owner minted.
- **Item 7** (consolidate duplicated semantics; gc worklist as P4
  criterion): **NOT ADOPTED as a discrete pre-A1 pass.** Any overlap
  with landed R4b work is incidental, not a discharge. Post-A1
  candidate; no owner minted.
- **Item 8** (doc consolidation): **PARTIALLY SUPERSEDED** — the
  supersession-banner/TD-§1/doc-gate legs landed 2026-08-11 and the
  docs gate now pins 147 assertions; the remaining §6 consolidation
  legs are not adopted. Post-A1 docs-pass candidate; no owner minted.
- **Item 10** (process rebalance): **ADOPTED IN PART BY PRACTICE**
  (Track P physical spikes preceded the R2-D freeze); the
  ledger-by-tooling and ceiling-revision legs are not adopted.
  Process items; no A1 obligation.

## 11. L11 — the matrix at the landing commit

`GwzWindowsMatrix-Classification.md:74-79` receives a dated update at
landing (the M5b class-membership note's "cfg(test)-only pre-A1"
clause changes truth-value at activation), and a three-arm matrix
(Windows + platform + ARM) is dispatched AT the landing commit, with
the activation package's added tests and the C-1 closure's 3 tests
pre-attributed expected-green in the ledger. Dispatch: ⟨SLOT: run IDs
at the landing commit⟩.

## 12. L12 — the ChangeBudget row

The activation package's row is filed in `GwzM5-8ChangeBudget.md`
under the OPEN charging convention, with the operator's
convention-ruling row (2,028 uniform / 582 review-artifact)
resurfaced beside it. Figures: ⟨SLOT: final per-repo files/LOC from
the landing-train report⟩.

## 13. L13 — the decode arm

Completeness [P3-7]: the discharging diff reintroduced a fresh
`unreachable!` v1 arm at `record_wire/decode.rs:123` — the exact
shape the panic audit condemned at this family (F5 §9 item 5).
Disposition: **the typed-twin conversion LANDS with the train.**
As landed: ⟨SLOT: the converted arm, file:line, from the
landing-train report⟩.

## 14. L14 — the moved behaviour pin's consequence

Recorded in the evidence map: activating migration reduces
v0-fault-injector coverage on the authority path for the seven
whitelisted shapes
(`g23::finalization::resumed_finalization_persists_each_phase_before_a_nested_mutation_fault`'s
final window now asserts the migration, `source_version == V1`; the
first three windows keep v0 coverage through the DI seam, verified by
the Safety axis — `AbsentV1Router` never migrates and fails closed).
**Restoration owner: Safety [P3-R2-2] at R2-E** — the eligible-row
upgrade-failure fallback (whitelisted row, atomic upgrade `Err(_)` →
v0 completes) gets one executed composed-path test; the two axes
converged on this same arm. Until it lands, the reduction stands
recorded here.

## 15. Safety round-2 P3 dispositions

- **[P3-R2-1]** `AtomicUpgradeFault`'s injected variants are
  production-compiled but inert (single production caller passes
  `None`). Hygiene (cfg-split signature or sealed constructor):
  **R2-E.**
- **[P3-R2-2]** — §14 above. **R2-E.**
- **[P3-R2-3]** — §7 above. Open on the register; cure named.
- **[P3-R2-4]** The builder's py tail was not evidence (stale
  `.so`); the axis rebuilt via maturin and reproduced 330/0. Standing
  rule adopted: **the landing py gate runs against a freshly built
  native module, and the tail states the `.so` timestamp.**
- **[P3-R2-5]** `classify_open_record`'s doc comment corrected by the
  train (the v0 arm fully decodes); the R4 conditions (a)-(c) are
  folded at §2.

## 16. The landing tuple and gates

⟨SLOT: at the landing commit — the coordinated commit hashes per
repo, the final gate tails verbatim (boundary checker green incl.
harness 69/69; four-partition census; g23; cli; py with fresh-.so
timestamp; clippy; fmt; the R4b-G batteries end-to-end green), and
the exact-ref push record for gwz-core.⟩
